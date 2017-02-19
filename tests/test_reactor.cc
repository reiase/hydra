#include "../include/line_proto.h"
#include "../include/reactor.h"
#include "../include/resp_proto.h"
#include <cstdlib>

using namespace reiase;

int main(int argc, char *argv[]) {
  service::Reactor<service::RESPProto> es(4, 100000);
  int s = service::CreateServiceSocket();
  es.bind(s);
  int cnt = 0;
  es.rpc([&cnt](const resp::Msg &x) -> resp::Msg {
    resp::Msg y; //= x;
    cnt++;
    y = cnt;
    return y;
  });
  es.join();

  // int s2 = service::CreateServiceSocket();
  // service::Reactor<service::LineProto> es2;
  // es2.bind(s2);
  // es2.rpc([](const std::string &x) -> std::string { return x; });
  //
  // es.join();
  // es2.join();
  return 0;
}
