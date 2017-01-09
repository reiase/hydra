#include "resp.hh"
#include <iostream>

namespace reiase {
namespace resp {

void Message::reset() {
  ivalue = 0;
  svalue = "";
  avalue.clear();
  vstack.clear();
  vstack.push_back(this);
}

template <typename T>
std::shared_ptr<Message> Message::push_back(T x) {
  if (type != AUTO && type != ARRAY) throw;
  if (type == AUTO) type = ARRAY;

  auto ptr = std::make_shared<Message>(x);
  avalue.push_back(ptr);
  return ptr;
}

int Message::decode(std::string frame) {
  if (vstack.size() == 0) return -1;
  int retval = vstack.back()->decode_raw(frame, vstack);
  if (retval == 1) vstack.pop_back();
  return retval;
}

int Message::feed(char c) {
  mask = (mask << 8) + ((int)c & 0xff);
  buffer.push_back(c);
  if ((mask & 0xffff) == 3338) {
    decode(buffer.substr(0, buffer.size() - 2));
    buffer = "";
  }
  return finished();
}

std::string Message::encode() {
  std::stringstream ss;
  switch (type) {
    case INTEGER:
      ss << ':' << ivalue << "\r\n";
      break;
    case SIMPLESTRING:
      ss << '+' << svalue << "\r\n";
      break;
    case ERROR:
      ss << '-' << svalue << "\r\n";
      break;
    case BULKSTRING:
      ss << '$' << svalue.size() << "\r\n" << svalue << "\r\n";
      break;
    case ARRAY:
      ss << '*' << ivalue << "\r\n";
      for (auto i = avalue.begin(); i != avalue.end(); i++) {
        ss << (*i)->encode();
      }
      ss << "\r\n";
      break;
    default:
      ss << "-ERROR\r\n";
  }

  return ss.str();
}

int Message::decode_raw(std::string frame, std::vector<Message*>& vstack) {
  if (type == BULKSTRING && svalue.size() < ivalue) {
    svalue.append(frame);
    if (svalue.size() < ivalue) {
      svalue.append("\r\n");
      return 0;
    } else {
      return 1;
    }
  }
  if (frame[0] == ':') {
    switch (type) {
      case AUTO:
        type = INTEGER;
      case INTEGER:
        break;
      default:
        return -1;
    }
    ivalue = atoi(frame.c_str() + 1);
    return 1;
  }
  if (frame[0] == '+' || frame[0] == '-') {
    switch (type) {
      case AUTO:
        type = frame[0] == '+' ? SIMPLESTRING : ERROR;
      case SIMPLESTRING:
        break;
      default:
        return -1;
    }
    svalue = frame.substr(1);
    return 1;
  }
  if (frame[0] == '$') {
    switch (type) {
      case AUTO:
        type = BULKSTRING;
      case BULKSTRING:
        break;
      default:
        return -1;
    }
    ivalue = atoi(frame.c_str() + 1);
    return 0;
  }
  if (frame[0] == '*') {
    switch (type) {
      case AUTO:
        type = ARRAY;
      case ARRAY:
        break;
      default:
        return -1;
    }
    ivalue = atoi(frame.c_str() + 1);
    for (int i = 0; i < ivalue; i++) {
      push_back();
    }
    for (auto i = avalue.rbegin(); i != avalue.rend(); i++) {
      vstack.push_back(i->get());
    }
    return 0;
  }
  return -1;
}
};
};
