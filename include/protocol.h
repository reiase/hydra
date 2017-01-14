#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <functional>
#include <string>

#include "logging.hh"

namespace reiase {
namespace service {

template <typename PROTO>
struct proto_traits;

#define DEFAULT_BUFSIZE 4096
template <typename PROTO>
class Protocol {
 public:
  typedef typename proto_traits<PROTO>::MSGTYPE MSGTYPE;
  typedef typename std::function<MSGTYPE(const MSGTYPE &)> HANDLER;

 public:
  static Protocol<PROTO> *create() { return new Protocol<PROTO>(); }

 public:
  void onInit(){};
  void onRead(){};
  void onWrite(){};
  void onClose(){};
  int onError() { return 1; };

  void destory() {
    onClose();
    close(fd);
  };
  void copy(const Protocol<PROTO> &p) {
    fd = p.fd;
    handler = p.handler;
    buf_size = p.buf_size;
    static_cast<PROTO *>(this)->deep_copy(p);
  }
  void deep_copy(const Protocol<PROTO> &p) {}
  void operator=(const Protocol<PROTO> &p) { copy(p); }

  void setHandler(HANDLER h) { handler = h; };
  void setBufSize(int x) { buf_size = x; }

 public:
  Protocol() { buf_size = DEFAULT_BUFSIZE; };
  Protocol(int x) : fd(x) { buf_size = DEFAULT_BUFSIZE; };
  Protocol(const Protocol<PROTO> &p) { copy(p); };

 public:
  HANDLER handler;
  int fd;
  int buf_size;
};

};  // service
};  // reiase

#endif /* PROTOCOL_H */
