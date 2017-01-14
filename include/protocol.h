#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <functional>
#include <string>

#include <unistd.h>

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
  static Protocol<PROTO> *create() { return new PROTO(); }
  static Protocol<PROTO> *create(int s) { return new PROTO(s); }

 public:
  void onInit() { static_cast<PROTO *>(this)->onInitImpl(); }
  void onRead() { static_cast<PROTO *>(this)->onReadImpl(); }
  void onWrite() { static_cast<PROTO *>(this)->onWriteImpl(); }
  int onClose() { return static_cast<PROTO *>(this)->onCloseImpl(); }
  int onError() { return static_cast<PROTO *>(this)->onErrorImpl(); }

  void onInitImpl() {}
  void onReadImpl() {}
  void onWriteImpl() {}
  int onCloseImpl() { return 1; }
  int onErrorImpl() { return 1; }

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
