#ifndef REACTOR_H
#define REACTOR_H

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "logging.hh"
#include "protocol.h"
#include "service.hh"

namespace reiase {
namespace service {

#define MAX_EVENTS 1024

template <typename PROTO>
class Reactor : public Service<PROTO> {
 public:
  void bind(int s) { service_fd = s; }

 private:
  int accept_connection(int service_fd) {
    sockaddr_in addrClient;
    int addrClientSize = sizeof(addrClient);
    int client_fd = accept(service_fd, (struct sockaddr *)&addrClient,
                           (socklen_t *)&addrClientSize);
    return client_fd;
  }

  virtual void main_loop(void) {
    this->enable = 1;
    epoll_fd = epoll_create1(0);
    struct epoll_event ev = {0};
    struct epoll_event *evs = NULL;
    if (epoll_fd < 0) {
      std::cerr << "error creating epoll fd." << std::endl;
    }

    ev.data.fd = service_fd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, service_fd, &ev) == -1) {
      std::cerr << "error listening service socket" << std::endl;
    }

    evs = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_EVENTS);

    std::map<int, PROTO> sessions;
    while (this->enable) {
      int nfd = epoll_wait(epoll_fd, evs, MAX_EVENTS, 5000);
      // LOG("events: %d", nfd);
      for (int i = 0; i < nfd; i++) {
        struct epoll_event &e = evs[i];
        if (service_fd == e.data.fd) {
          int client_fd = accept_connection(service_fd);
          if (-1 == client_fd) {
            std::cerr << "error accept new connection." << std::endl;
          }
          PROTO s(client_fd);
          s.setHandler(this->handler);
          sessions[client_fd] = s;
          sessions[client_fd].onInit();
          sessions[client_fd].setHandler(this->handler);

          ev.data.fd = client_fd;
          ev.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
          if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev)) {
            std::cerr << "error add new session" << std::endl;
          }  // LOG("onRead from resp_proto %d", buf_size);

        } else {
          auto &session = sessions[e.data.fd];
          if (e.events & EPOLLIN) {
            // INFO("read on %d", e.data.fd);
            session.onRead();
          }
          if (e.events & EPOLLOUT) {
            session.onWrite();
          }
          if ((e.events & EPOLLERR) || (e.events & EPOLLRDHUP)) {
            //  INFO("close %d", e.data.fd);
            int should_close = session.onError();
            if (should_close) {
              ev.data.fd = session.fd;
              epoll_ctl(epoll_fd, EPOLL_CTL_DEL, session.fd, &ev);
              auto it = sessions.find(session.fd);
              if (it != sessions.end())
                sessions.erase(it);
              else
                LOG("failed to close", "");
              session.destory();
            }
          }
        }  // end for
      }
    }
  };
  /*typedef typename Protocol<PROTO>::MSGTYPE MSGTYPE;
  typedef typename Protocol<PROTO>::HANDLER HANDLER;
  void setHandler(HANDLER h) {
    handler = h;
    LOG("1 set handler %d %d", !!h, !!handler);
    }*/

 private:
  // int enable;
  int epoll_fd;
  int service_fd;

  // HANDLER handler;
  std::vector<PROTO> sessions;
};
};  // service
};  // reiase

#endif /* REACTOR_H */
