#include <mutex>
#include <vector>

#include <zservice.hh>

template <typename T> class Partitioner {
private:
  int numParts;

public:
  Partitioner(int num_parts) : numParts(num_parts) {}

  int part(T key) { return key % numParts; }
};

template <typename T> class ParameterStorage {
private:
  std::vector<T> storage;
  std::mutex mtx;

public:
  void fill(int key, T value = 0.0) {
    if (storage.size() < key + 1) {
      storage.resize(key + 1, value);
    }
  }

  template <typename INPUT, typename OUTPUT>
  void get(INPUT first, INPUT last, OUTPUT result) {
    std::lock_guard<std::mutex> lck(mtx);
    while (first != last) {
      fill(*first);
      *result = *first;
      first++;
    }
  }

  template <typename INPUT, typename INPUT2, typename UPDATER>
  void update(INPUT first, INPUT last, INPUT2 vfirst, UPDATER updater) {
    std::lock_guard<std::mutex> lock(mtx);
    while (first != last) {
      T input = *vfirst;
      fill(*first);
      storage[*first] = updater(storage[*first], input);
      first++;
      vfirst++;
    }
  }
};

class ParameterManager : ZService {
private:
  std::vector<std::string> slaves;
  std::vector<std::string> workers;

  std::string status = "initializing";

public:
  ParameterManager() : ParameterManager(1) {}
  ParameterManager(int ioThreads = 1) : ZService(ioThreads) {
    status = "initializing";
  }

  void run(void) {
    rpc([&](std::string x) -> std::string {});
  }
};
