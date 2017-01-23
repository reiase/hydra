#ifndef LINE_PROTO_H
#define LINE_PROTO_H

#include <fcntl.h>
#include <unistd.h>

#include "logging.h"
#include "protocol.h"
#include "resp.h"

namespace reiase {
namespace service {

class LineProto;

template <>
struct proto_traits<LineProto> {
  typedef typename std::string MSGTYPE;
};

class LineProto : public Protocol<LineProto> {
 public:
  void onInitImpl() {
    int opts = fcntl(fd, F_GETFL) | O_NONBLOCK;
    fcntl(fd, F_SETFL, opts);
    line.clear();
  };

  void onReadImpl() {
    char buffer[buf_size];
    int retval = 1;
    while (retval > 0) {
      retval = read(fd, buffer, buf_size);

      for (int i = 0; i < retval; i++) {
        line.push_back(buffer[i]);
        if (buffer[i] == '\n'){
          auto obuffer = handler(line);
          write(fd, obuffer.c_str(), obuffer.size());
          line.clear();
        }
      }    // end for
    }      // end while
  }

 public:
  LineProto(){};
  LineProto(int x) : Protocol(x){};
  LineProto(const LineProto &p) : Protocol(p){};

 private:
  std::string line;
};

};  // service
};  // reiase

#endif /* RESP_PROTO_H */
