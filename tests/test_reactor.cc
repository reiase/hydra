#include <cstdlib>
#include "../include/resp.hh"
#include "../include/resp_proto.h"
#include "../include/resp_proto.h"
#include "../include/reactor.h"

using namespace reiase;

int main(int argc, char *argv[]) {
  service::Reactor<service::RESPProto> es;
  int s = service::CreateServiceSocket();
  es.bind(s);
  int cnt = 0;
  es.rpc([&cnt](const resp::Msg &x) -> resp::Msg {
    resp::Msg y;
    cnt++;
    y = cnt;
    return y;
  });
  es.join();
  return 0;
}
