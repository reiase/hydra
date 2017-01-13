#include <cstdlib>
#include "../include/eservice.hh"
#include "../include/resp.hh"
using namespace reiase;

int main(int argc, char *argv[]) {
  service::EService es;
  int s = service::Service::CreateServiceSocket();
  es.bind(s);
  int cnt = 0;
  es.rpc([&cnt](const resp::Msg &x) -> resp::Msg {
    resp::Msg y;
    cnt++;
    y = "PONG";
    // printf("%d\n", cnt);
    // printf(x.encode().c_str());
    return y;
  });
  es.join();
  return 0;
}
