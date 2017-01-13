#ifndef ESERVICE_H
#define ESERVICE_H

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <functional>
#include <string>
#include <vector>

#include "logging.hh"
#include "resp.hh"
#include "service.hh"

namespace reiase {
namespace service {

class Session {
 public:
  void connect();
  void reconnect();
  void attatch();
  void destory() {
    LOG("close %d", fd);
    onClose();
    close(fd);
  };

 public:
  int onInit();
  int onRead();
  int onWrite();
  int onError() { return 1; };
  void onClose(){};

  Session(){};
  Session(int x) : fd(x) {}
  Session(const Session &s) : fd(s.fd), mode(s.mode), handler(s.handler) {}

  void setHandler(std::function<resp::Msg(const resp::Msg &)> h) {
    handler = h;
    LOG("set handler %d %d", !!h, !!handler);
  }
  void emit(void);

 public:
  int fd;
  int mode;

 private:
  std::string ibuffer;
  std::string obuffer;
  resp::MsgParser parser;

  std::function<resp::Msg(const resp::Msg &)> handler;
};

class EService : public Service {
 public:
  void bind(int sfd) { service_fd = sfd; }

 private:
  virtual void main_loop();

 private:
  int epoll_fd;
  int service_fd;

  std::vector<Session> sessions;
};

}  // service
}  // reiase

#endif /* ESERVICE_H */
