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
  typedef typename Protocol<PROTO>::MSGTYPE MSGTYPE;
  typedef typename Protocol<PROTO>::HANDLER HANDLER;
  void setHandler(HANDLER h) { handler = h; };

  void rpc(HANDLER func) {
    setHandler(func);
    start();
  };

  template <typename MSG>
  void pull(HANDLER func);

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
