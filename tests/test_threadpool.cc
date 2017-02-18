#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../include/threadpool.h"

using namespace reiase;

volatile int global_cnt = 0;
int testnum = 0;

int calc_load(int x) {
  int sum = 0;
  x = x % 10000;
  for (int i = 0; i < x; i++)
    sum += x * x;
  return sum;
}

struct processResp {
  int num;
  processResp() {}
  processResp(int number) { num = number; }
  void operator()() {
    pthread_t tid = pthread_self();
    global_cnt += calc_load(num);
  }
};
int main(int argc, char *argv[]) {
  ThreadPool<processResp> pool(3, 10000000);
  while (global_cnt < 9000000) { /* code */
    pool.add_task(processResp(global_cnt));
    global_cnt++;
  }

  // time_t timerstart, timeend;
  // time(&timerstart);
  clock_t start = clock();
  pool.init();
  while (pool.task_queue_size > 0) {
    usleep(50);
  }
  // time(&timeend);
  clock_t end = clock();
  // double second = difftime(timeend, timerstart);
  // printf("run time %f\n", second);
  printf("run time %f\n", (double)(end - start) / CLOCKS_PER_SEC);

  sleep(1);
  pool.stop();
  return 0;
}
