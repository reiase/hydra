#include <cstdlib>
#include "../include/eservice.hh"
using namespace reiase;

int main(int argc, char *argv[]) {
  service::EService es;
  int s = service::Service::CreateServiceSocket(atoi(argv[1]));
  es.bind(s);
  es.start();
  es.join();
  return 0;
}
