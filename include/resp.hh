#ifndef RESP_HH
#define RESP_HH

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace reiase {
namespace resp {

enum MSGTYPE {
  AUTO = 0,
  INTEGER = 1 << 0,
  STRING = 1 << 1,
  ARRAY = 1 << 2,

  SIMPLESTRING = 1 << 16,
  BULKSTRING = 1 << 17,
  ERROR = 1 << 18
};

class Msg {
 public:
  template <typename T>
  std::shared_ptr<Msg> push_back(T x) {
    if (isString()) delete value.svalue;
    if (!isArray()) value.avalue = new std::vector<std::shared_ptr<Msg>>();
    setFlag(ARRAY);

    auto ptr = std::make_shared<Msg>(x);
    value.avalue->push_back(std::move(ptr));
    return ptr;
  }
  std::shared_ptr<Msg> push_back() { return push_back(AUTO); }

 public:
  inline bool isAuto() const { return flag == 0; }
  inline bool isInteget() const { return (flag & INTEGER) > 0; }
  inline bool isString() const { return flag & STRING; }
  inline bool isArray() const { return flag & ARRAY; }
  inline bool isSimpleString() const { return flag & SIMPLESTRING; }
  inline bool isBulkString() const { return flag & BULKSTRING; }
  inline bool isError() const { return flag & ERROR; }

  inline void setError() {
    if (isSimpleString())
      setFlag(ERROR | STRING);
    else
      throw;
  }

  inline void setBulk() {
    value.svalue = new std::string();
    setFlag(BULKSTRING | STRING);
  }

 public:
  const int& asInt() const { return value.ivalue; };
  const std::string& asString() const { return *value.svalue; }
  const std::vector<std::shared_ptr<Msg>>& asList() const {
    return *value.avalue;
  }

  std::string& strbuf() { return *value.svalue; }

 public:
  Msg() : flag(AUTO) {}
  Msg(MSGTYPE t) : flag(t) {}
  Msg(int x) : flag(INTEGER) { value.ivalue = x; }
  Msg(std::string s) : flag(STRING) { *this = s; }
  inline void reset(int fg = AUTO) {
    if (isString()) delete value.svalue;
    if (isArray()) {
      value.avalue->clear();
      delete value.avalue;
    }
    flag = fg;
  }
  ~Msg() { reset(); }

  void operator=(int x) {
    reset(INTEGER);
    value.ivalue = x;
  }

  void operator=(std::string s) {
    reset(STRING);
    value.svalue = new std::string(s);
    if (s.find("\r\n") != std::string::npos) {
      flag |= SIMPLESTRING;
    } else {
      flag |= BULKSTRING;
    }
  }

  void operator=(std::vector<std::shared_ptr<Msg>> a) {
    reset(ARRAY);
    value.avalue = new std::vector<std::shared_ptr<Msg>>(a);
  }

  void operator=(const Msg& m) {
    if (m.isInteget()) *this = m.asInt();
    if (m.isString()) *this = m.asString();
    if (m.isArray()) *this = m.asList();
  }

 public:
  inline std::string encode() const {
    std::stringstream ss;
    if (isInteget()) ss << ':' << value.ivalue << "\r\n";
    if (isString()) {
      if (isSimpleString()) ss << '+';
      if (isBulkString()) ss << '$' << value.svalue->size() << "\r\n";
      if (isError()) ss << '-';
      ss << *value.svalue << "\r\n";
    }
    if (isArray()) {
      ss << '*' << value.avalue->size() << "\r\n";
      for (auto i = value.avalue->begin(); i != value.avalue->end(); i++)
        ss << (*i)->encode();
    }
    return ss.str();
  };
  int getFlag() { return flag; }

 private:
  int flag;
  union _value {
    int ivalue;
    std::string* svalue;
    std::vector<std::shared_ptr<Msg>>* avalue;
  } value;

  void setFlag(int fg) { flag = fg; }
  void addFlag(int fg) { flag = flag | fg; }
};

class MsgParser {
 public:
  int reset(void) {
    buffer.clear();
    msg = std::make_shared<Msg>();
    vstack.clear();
    vstack.push_back(msg.get());
    return 1;
  }

  inline int ready(void) { return vstack.size() == 0; }

  inline int feed(std::string frame) {
    if (vstack.size() == 0) return -1;
    int retval = decode(frame);
    if (retval == 1) vstack.pop_back();
    return retval;
  }

  inline int feed(char c) {
    mask = (mask << 8) + (c & 0xFF);
    buffer.push_back(c);
    if ((mask & 0xFFFF) == 3338) {
      feed(buffer.substr(0, buffer.size() - 2));
      buffer.clear();
    }
    return ready();
  };

  const std::shared_ptr<Msg>& result() { return msg; }
  std::shared_ptr<Msg> pop() { return std::move(msg); }

  static std::shared_ptr<MsgParser> create() {
    auto ptr = std::make_shared<MsgParser>();
    ptr->reset();
    return ptr;
  }

 private:
  int decode(std::string frame) {
    if (vstack.size() == 0) return -1;
    Msg* current = vstack.back();
    if (current->isBulkString()) {
      std::string& bs = current->strbuf();
      if (bs.size() < size) {
        bs.append(frame);
        if (bs.size() < size) {
          bs.append("\r\n");
          return 0;
        } else {
          return 1;
        }  // end if else
      }
    }  // end if

    if (frame[0] == ':') {
      *current = atoi(frame.c_str() + 1);
      return 1;
    }

    if (frame[0] == '+' || frame[0] == '-') {
      *current = frame.substr(1);
      if (frame[0] == '-') current->setError();
      return 1;
    };

    if (frame[0] == '$') {
      size = atoi(frame.c_str() + 1);
      current->setBulk();
      return 0;
    }

    if (frame[0] == '*') {
      size = atoi(frame.c_str() + 1);
      current->reset();
      for (int i = 0; i < size; i++) current->push_back();
      vstack.pop_back();
      auto list = current->asList();
      for (auto i = list.rbegin(); i != list.rend(); i++)
        vstack.push_back(i->get());

      return 0;
    } else {  // inline cmd
      *current = frame;
      return 1;
    }

    return -1;
  }

 private:
  int mask;
  int size;
  std::string buffer;
  std::vector<Msg*> vstack;
  std::shared_ptr<Msg> msg;
};
};
};
#endif /* RESP_HH */
