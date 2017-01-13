#include <cstdlib>
#include "../include/eservice.hh"
#include "../include/resp.hh"
using namespace reiase;

int main(int argc, char *argv[]) {
  service::EService es;
  int s = service::Service::CreateServiceSocket();
  es.bind(s);
  es.rpc([](resp::Msg x) -> resp::Msg {return x;});
  es.join();
  return 0;
}
