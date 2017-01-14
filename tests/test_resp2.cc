#include <iostream>
#include <memory>
#include <string>

#include "../include/resp.hh"

using namespace reiase;

int main(int argc, char *argv[]) {
  auto parser = resp::MsgParser::create();

  parser->feed(":1");
  if (parser->ready())
    std::cout << parser->result()->encode() << std::endl;
  else
    std::cout << "error parsing integer" << std::endl;

  std::cout << "----" << std::endl;

  parser->reset();
  parser->feed("*2");
  parser->feed(":2");
  parser->feed("*2");
  parser->feed(":2");
  parser->feed(":3");
  if (parser->ready())
    std::cout << parser->result()->encode() << std::endl;
  else
    std::cout << "error parsing array" << std::endl;

  std::cout << "----" << std::endl;

  parser->reset();
  parser->feed("*2");
  parser->feed("$6");
  parser->feed("1");
  parser->feed("123");
  parser->feed(":3");
  if (parser->ready()) std::cout << parser->result()->encode() << std::endl;
  else std::cout << "error parsing bulkstring" << std::endl;

  std::cout << "----" << std::endl;

  parser->reset();
  std::string raw = "*2\r\n$6\r\n1\r\n123\r\n:3\r\n";
  for (int i = 0; !parser->ready(); i++) {
    parser->feed(raw[i]);
  }
  std::cout << parser->result()->encode() << std::endl;

  return 0;
}
