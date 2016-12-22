#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <zmq.h>

#include <assert.h>

#include "network_utils.hh"

void zmq_buf_free(void *data, void *hint) { free(data); }

class Z {
private:
  void *zctx = nullptr;

public:
  Z(int ioThreads = 1) {
    zctx = zmq_ctx_new();
    zmq_ctx_set(zctx, ZMQ_IO_THREADS, ioThreads);
  }

  ~Z() { zmq_ctx_term(zctx); }

  void *ctx() { return zctx; }
};

class ZSocket {
public:
  std::shared_ptr<Z> zctx = nullptr;
  void *sock = nullptr;
  std::string addr;
  volatile int enable = 0;

  ZSocket(Z *z) {
    zctx = std::shared_ptr<Z>(z);
    enable = 0;
  }

  ZSocket(int ioThreads = 1) : ZSocket(new Z(ioThreads)) {}

  ZSocket(ZSocket &z) {
    zctx = z.zctx;
    enable = 1;
  }

  ~ZSocket() {
    if (sock != nullptr)
      zmq_close(sock);
  }

  void destory() {
    zmq_close(sock);
    sock == nullptr;
    zctx.reset();
  }

  std::string choose_address(std::string prefix = "tcp://") {
    std::string interface;
    std::string ip;

    std::stringstream ss;
    int port = std::rand() % 50000 + 1024;
    ps::GetAvailableInterfaceAndIP(&interface, &ip);
    ss << prefix << ip << ":" << port;
    return ss.str();
  }

  std::string bind(int mode, std::string prefix = "tcp://") {
    sock = zmq_socket(zctx->ctx(), mode);
    enable = 1;
    int retval = 0;
    do {
      addr = choose_address();
      retval = zmq_bind(sock, addr.c_str());
    } while (retval != 0);
    return addr;
  }

  void connect(int mode, std::string address) {
    sock = zmq_socket(zctx->ctx(), mode);
    enable = 1;
    addr = address;
    zmq_connect(sock, addr.c_str());
  }

  void send(std::string raw) {
    int retval = zmq_send(sock, raw.c_str(), raw.size(), 0);
    assert(raw.size() == retval);
  }

  std::string recv() {
    std::string result = "";
    int retval = 0;
    char buf[4096] = {0};

    zmq_pollitem_t items[1];
    items[0].socket = sock;
    items[0].events = ZMQ_POLLIN;
    do {
      int rc = zmq_poll(items, 1, 100);
      if (rc < 1) {
        if (result.size() > 0)
          continue;
        if (!enable)
          break;
        continue;
      }
      retval = zmq_recv(sock, buf, 4096, 0);
      if (result.size() + 4096 < retval)
        result.append(buf, 4096);
      else
        result.append(buf, retval - result.size());
    } while (result.size() + 4096 < retval);
    return result;
  }
};

class ZService : public ZSocket {
private:
  std::thread *worker;

public:
  ZService(int ioThreads = 1) : ZSocket(ioThreads) {}

  ZService(ZSocket &z) : ZSocket(z) {}

  // create a publish service
  std::string pub() { return bind(ZMQ_PUB); }

  void rpc(std::function<std::string(std::string)> func) {
    std::string retval = bind(ZMQ_REP);
    worker = new std::thread([&]() -> void {
      std::cout << "start service!" << std::endl;
      while (enable) {
        std::string req = recv();
        if (!enable)
          break;
        std::string rsp = func(req);
        send(rsp);
      }
      std::cout << "stop service!" << std::endl;
    });
  }

  void push(std::string raw) {
    int retval = zmq_send(sock, raw.c_str(), raw.size(), ZMQ_DONTWAIT);
    assert(raw.size() == retval);
  }

  void start() { enable = 1; };
  void stop() { enable = 0; };
  void join() {
    worker->join();
    std::cout << "stop service!" << std::endl;
  };
  void run();
};

class ZClient : public ZSocket {
public:
  ZClient(int ioThreads = 1) : ZSocket(ioThreads) {}

  ZClient(ZSocket &z) : ZSocket(z) {}

  ZClient(std::string _addr, int ioThreads = 1) : ZClient(ioThreads) {
    addr = _addr;
  }

  void rpc(std::string address) { connect(ZMQ_REQ, address); }

  std::string call(std::string req) {
    send(req);
    return recv();
  }
};
