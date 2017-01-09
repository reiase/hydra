#include <iostream>
#include <map>

#include <fcntl.h>
#include <netinet/in.h>

#include "eservice.hh"
#include "logging.hh"

namespace reiase {

namespace service {

int Session::onInit() {
  LOG("new connection!", "");
  int opts = fcntl(fd, F_GETFL) | O_NONBLOCK;
  fcntl(fd, F_SETFL, opts);
  mode = EPOLLIN;
  return 1;
}

int Session::onRead() {
  char buffer[4096];
  int retval = 1;
  while (retval > 0) {
    retval = read(fd, buffer, 4096);
    if (retval > 0) ibuffer.append(buffer, retval);
  }
  LOG("buffer:%s", ibuffer.c_str());
  return 0;
}

int Session::onWrite() {}

static int accept_connection(int service_fd) {
  sockaddr_in addrClient;
  int addrClientSize = sizeof(addrClient);
  int client_fd = accept(service_fd, (struct sockaddr *)&addrClient,
                         (socklen_t *)&addrClientSize);
  return client_fd;
}

#define MAX_EVENTS 1024
void EService::main_loop(void) {
  enable = 1;
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

  std::map<int, Session> sessions;
  while (enable) {
    int nfd = epoll_wait(epoll_fd, evs, MAX_EVENTS, 500);
    for (int i = 0; i < nfd; i++) {
      struct epoll_event &e = evs[i];
      if (service_fd == e.data.fd) {
        int client_fd = accept_connection(service_fd);
        if (-1 == client_fd) {
          std::cerr << "error accept new connection." << std::endl;
        }
        Session s(client_fd);
        sessions[client_fd] = s;

        ev.data.fd = client_fd;
        ev.events = EPOLLIN;
        if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev)) {
          std::cerr << "error add new session" << std::endl;
        }
      } else {
        auto &session = sessions[e.data.fd];
        int need_update = 0;
        if (e.events & EPOLLIN) {
          INFO("read on %d", e.data.fd);
          need_update = session.onRead();
        }
        if (e.events & EPOLLOUT) {
          need_update = session.onWrite();
        }
        if (e.events & EPOLLERR) {
          INFO("error on %d", e.data.fd);
          int should_close = session.onError();
          if (should_close) {
            ev.data.fd = session.fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, session.fd, &ev);
            auto it = sessions.find(session.fd);
            if (it != sessions.end()) sessions.erase(it);
            session.destory();
          }
        }
        if (need_update) {
          ev.data.fd = e.data.fd;
          ev.events = session.mode;
          if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_MOD, e.data.fd, &ev)) {
            LOG("error update fd %d", e.data.fd);
          } // end if
        } //end if 
      } //end for
    }
  }
}

}  // service
}  // reiase
