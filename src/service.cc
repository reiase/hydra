#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "logging.hh"
#include "service.hh"

namespace reiase {
namespace service {
int CreateServiceSocket(int port) {
  auto getport = [&]() -> int {
    return port > 0 ? port : (std::rand() % 50000 + 1024);
  };

  struct sockaddr_in sin;
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  int retval = 0;

  do {
    int iport = getport();
    memset(&sin, sizeof(sin), 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(iport);
    sin.sin_addr.s_addr = INADDR_ANY;
    retval = bind(sock, (struct sockaddr *)&sin, sizeof(sin));
    if (retval == -1) {
      perror(NULL);
    } else {
      LOG("create service on port: %d", iport);
    }
  } while (retval != 0);
  listen(sock, 1);
  return sock;
}
}  // service
}  // reiase
