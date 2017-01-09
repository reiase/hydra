#ifndef RESP_HH
#define RESP_HH

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace reiase {
namespace resp {

enum MSGTYPE { SIMPLESTRING, BULKSTRING, INTEGER, ERROR, ARRAY, AUTO };

class Message : public std::enable_shared_from_this<Message> {
 public:
  void reset(void);
  int finished(void) { return vstack.size() == 0; }
  std::string encode();
  int decode(std::string frame);
  int feed(char c);

  template <typename T>
  std::shared_ptr<Message> push_back(T x);
  std::shared_ptr<Message> push_back() { return push_back(AUTO); }

  inline std::string toString() { return svalue; }
  inline int toInt() { return ivalue; }
  inline const std::vector<std::shared_ptr<Message>>& toList() {
    return avalue;
  }

 public:
  Message(int x) : type(INTEGER), ivalue(x) {}
  Message(std::string x) : type(SIMPLESTRING), svalue(x) {
    if (x.find("\r\n") != std::string::npos) {
      type = BULKSTRING;
    } else {
      type = SIMPLESTRING;
    }
  }
  Message(MSGTYPE t = AUTO) : type(t), ivalue(0) { reset(); }

 private:
  int decode_raw(std::string frame, std::vector<Message*>& vstack);

 private:
  MSGTYPE type;
  int ivalue;
  std::string svalue;
  std::vector<std::shared_ptr<Message>> avalue;

  std::vector<Message*> vstack;
  std::string buffer;
  int mask;
};
};
};

#endif /* RESP_HH */
