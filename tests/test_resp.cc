#include <iostream>
#include <memory>
#include <string>

#include "../include/resp.hh"

using namespace reiase;

int main(int argc, char *argv[]) {
  auto msg1 = std::make_shared<resp::Message>(resp::AUTO);
  msg1->decode(":1");
  std::cout << msg1->encode() << std::endl;

  std::cout << "----" << std::endl;
  auto msg2 = std::make_shared<resp::Message>(resp::AUTO);
  msg2->decode("*2");
  msg2->decode(":1");
  msg2->decode("+2");
  std::cout << msg2->encode() << std::endl;

  std::cout << "----" << std::endl;
  auto msg3 = std::make_shared<resp::Message>(resp::AUTO);
  msg3->decode("*2");
  msg3->decode(":1");
  msg3->decode("*2");
  msg3->decode(":2");
  msg3->decode(":3");
  std::cout << msg3->encode() << std::endl;

  std::cout << "----" << std::endl;
  auto msg4 = std::make_shared<resp::Message>(resp::AUTO);
  msg4->decode("*2");
  msg4->decode("$6");
  msg4->decode("1");
  msg4->decode("123");
  msg4->decode(":3");
  std::cout << msg4->encode() << std::endl;

  std::cout << "----" << std::endl;
  std::string raw = "*2\r\n$6\r\n1\r\n123\r\n:3\r\n";
  auto msg5 = std::make_shared<resp::Message>(resp::AUTO);
  for (int i = 0; msg5->finished() == 0; i++) {
    msg5->feed(raw[i]);
  }
  std::cout << msg5->encode() << std::endl;

  return 0;
}
