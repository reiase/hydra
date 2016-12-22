#include <iostream>
#include <thread>
#include <vector>

#include <unistd.h>

#include "../include/zservice.hh"

int main(int argc, char *argv[]) {
  ZService ss(1);

  ss.rpc([](std::string x) -> std::string {
    // std::cout << "hi" << std::endl;
    return x + x;
  });

  std::vector<std::thread *> ts;
  for (int j = 0; j < 5; j++) {
    auto w = new std::thread([&]() -> void {
      ZClient cc(1);
      cc.rpc(ss.addr);
      std::cout << ss.addr << std::endl;
      for (int i = 0; i < 10000; i++) {
        std::stringstream ss;
        for (int k = 0; k < 1000; k++) {
          ss << " " << k;
        }
        cc.call(ss.str());
        if (i % 1000) {
        } else {
          std::cout << i << std::endl;
        }
        //":" << cc.call(ss.str()) << std::endl;
      }
      std::cout << "stop" << std::endl;
      // cc.destory();
    });
    ts.push_back(w);
  }
  for (int j = 0; j < 5; j++) {
    std::cout << "join" << std::endl;
    ts[j]->join();
    std::cout << "joinded" << std::endl;
  }
  // ZClient cc(1);
  // cc.rpc(ss.addr);
  // std::cout << ss.addr << std::endl;
  // for (int i = 0; i < 10000; i++) {
  //   if (i % 1000)
  //     cc.call("hi!");
  //   else
  //     std::cout << i << ":" << cc.call("hi!") << std::endl;
  // }
  std::cout << "stop" << std::endl;
  ss.stop();
  std::cout << "join" << std::endl;
  ss.join();
  std::cout << "destory" << std::endl;
  ss.destory();
  std::cout << "!!!" << std::endl;
  return 0;
}
