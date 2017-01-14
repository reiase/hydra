#ifndef SERVICE_HH
#define SERVICE_HH

#include <functional>
#include <thread>

#include "logging.hh"
#include "protocol.h"

namespace reiase {
namespace service {

int CreateServiceSocket(int port = -1);

template <typename PROTO>
class Service {
 public:
  // typedef typename proto_traits<PROTO>::MSGTYPE MSGTYPE;
  // typedef typename std::function<MSGTYPE(const MSGTYPE &)> HANDLER;
  typedef typename Protocol<PROTO>::MSGTYPE MSGTYPE;
  typedef typename Protocol<PROTO>::HANDLER HANDLER;
  void setHandler(HANDLER h) {
    handler = h;
    LOG("1 set handler %d %d %d %d", !!h, !!handler,
        h.target_type().hash_code(), handler.target_type().hash_code());
  };

  void rpc(HANDLER func) {
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

  virtual void main_loop(void){};

 public:
  int enable;
  std::thread *worker;
  HANDLER handler;
};

}  // service
}  // reiase

#endif /* SERVICE_HH */
