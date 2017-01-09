#ifndef SERVICE_HH
#define SERVICE_HH

#include <functional>
#include <thread>

namespace reiase {
namespace service {

class Service {
 public:
  static int CreateServiceSocket(int port = -1);

  template <typename REQ, typename RSP>
  void rpc(std::function<RSP(REQ)> func);

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

 private:
  virtual void main_loop(void){};

 protected:
  int enable;
  std::thread *worker;
};

}  // service
}  // reiase

#endif /* SERVICE_HH */
