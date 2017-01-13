#ifndef SERVICE_HH
#define SERVICE_HH

#include <functional>
#include <thread>

#include "resp.hh"
#include "logging.hh"

namespace reiase {
namespace service {

class SessionHandler {};

class Service {
 public:
  static int CreateServiceSocket(int port = -1);
  void setHandler(std::function<resp::Msg(const resp::Msg &)> h) {
    handler = h;
    LOG("1 set handler %d %d", !!h, !!handler);
  }

  void rpc(std::function<resp::Msg(const resp::Msg&)> func){
    setHandler(func);
    start();
  };

  template <typename MSG>
  void pull(std::function<void(MSG)> func);

  void start(void) {
    enable = 1;
    worker = new std::thread([&]() -> void { main_loop(); });
  };
  void stop(void) { enable = 0; }
  void join(void) {
    worker->join();
    delete worker;
  };

 private:
  virtual void main_loop(void){};

 protected:
  int enable;
  std::thread *worker;
  std::function<resp::Msg(const resp::Msg&)> handler;
};

}  // service
}  // reiase

#endif /* SERVICE_HH */
