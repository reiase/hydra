#ifndef RESP_PROTO_H
#define RESP_PROTO_H

#include <fcntl.h>
#include <unistd.h>

#include "logging.h"
#include "protocol.h"
#include "resp.h"

namespace reiase {
namespace service {

class RESPProto;

template <>
struct proto_traits<RESPProto> {
  typedef typename resp::Msg MSGTYPE;
};

class RESPProto : public Protocol<RESPProto> {
 public:
  void onInitImpl() {
    int opts = fcntl(fd, F_GETFL) | O_NONBLOCK;
    fcntl(fd, F_SETFL, opts);
    parser.reset();
  };

  void onReadImpl() {
    char buffer[buf_size];
    int retval = 1;
    while (retval > 0) {
      retval = read(fd, buffer, buf_size);
      for (int i = 0; i < retval; i++) {
        parser.feed(buffer[i]);
        if (parser.ready()) {
          auto req = parser.pop();
          auto rsp = handler(*req.get());
          std::string obuffer = rsp.encode();
          write(fd, obuffer.c_str(), obuffer.size());
          parser.reset();
          //          LOG("onRead rsp: ", obuffer.c_str());
        }  // end if
      }    // end for
    }      // end while
  }

 public:
  RESPProto(){};
  RESPProto(int x) : Protocol(x){};
  RESPProto(const RESPProto &p) : Protocol(p){};

 private:
  resp::MsgParser parser;
};

};  // service
};  // reiase

#endif /* RESP_PROTO_H */
