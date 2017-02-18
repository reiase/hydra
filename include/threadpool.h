#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <cstdio>
#include <queue>
#include <stdarg.h>
#include <utility>
#include <vector>

#include <pthread.h>

namespace reiase {

template <typename Task> class ThreadPool;
template <typename Task> struct Thread {
  Thread() {
    cond = PTHREAD_COND_INITIALIZER;
    lock = PTHREAD_MUTEX_INITIALIZER;
    exit_flag = 0;
  }

  void set_thread(pthread_t t) { thread = t; }
  void set_task(Task &t) { task = t; }
  void signal() { pthread_cond_signal(&cond); }
  void cancel() {
    exit_flag = 1;
    signal();
  }
  void join() { pthread_join(thread, NULL); }
  void stop(void) {
    cancel();
    join();
  }
  void info(const char *fmt, ...) {
    char buff[1024];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buff, fmt, ap);
    va_end(ap);
    fprintf(stderr, "worker %p=>%d: %s\n", this, thread, buff);
  }

public:
  ThreadPool<Task> *pool;
  pthread_mutex_t lock;
  int exit_flag;

  pthread_t thread;
  pthread_cond_t cond;
  Task task;
};

template <typename Task> class ThreadPool {
public:
  ThreadPool() : ThreadPool(0) {}
  ThreadPool(int workers, int tasks = 100)
      : max_workers(workers), max_tasks(tasks), task_queue_size(0),
        worker_queue_size(0) {}

  void init(void) { enlarge(max_workers); }

  void enlarge(size_t new_size = 0) {
    if (new_size > 0)
      max_workers = new_size;
    while (workers.size() < max_workers)
      add_new_worker();
  }

  void add_task(Task task) {
    queue_task(task);
    if (worker_queue_size > 0) { // try assign new task to worker
      auto worker = get_worker();
      if (NULL == worker)
        return;
      int retval = assign_task(*worker);
      // printf("assign task result %d\n", retval);
      if (!retval)
        queue_worker(worker);
      else
        worker->signal();
    }
  }

  void add_new_worker() {
    pthread_mutex_lock(&worker_lock);
    if (workers.size() >= max_workers)
      return;

    Thread<Task> *thread = new Thread<Task>();
    thread->pool = this;
    pthread_t t;
    int retval = pthread_create(&t, NULL, ThreadPool::thread_main, thread);
    if (retval != 0) {
      printf("error %d\n", retval);
      throw;
    }
    thread->set_thread(t);
    workers.push_back(thread);
    pthread_mutex_unlock(&worker_lock);
  }

  static void *thread_main(void *args) {
    Thread<Task> *self = static_cast<Thread<Task> *>(args);
    ThreadPool<Task> *pool = self->pool;
    //  self->info("create thread");
    pthread_mutex_lock(&self->lock);
    int ready = 0;
    while (1) {
      if (pool->task_queue_size > 0) // task queue is not empty
        ready = pool->assign_task(*self);
      else
        ready = 0;

      // self->info("ready? %d", ready);
      if (!ready) { // if not ready, waiting for signal
        // self->info("waiting signal");
        pool->queue_worker(self);
        pthread_cond_wait(&self->cond, &self->lock);
      }
      if (self->exit_flag) {
        pthread_exit(NULL);
        self->info("quit");
      }

      // run task
      // self->info("running");
      self->task();
      // self->info("task done");
    }
    pthread_mutex_unlock(&self->lock);
  }

  void stop(void) {
    for (int i = 0; i < workers.size(); i++)
      workers[i]->stop();
  }

private:
  void queue_worker(Thread<Task> *worker) {
    pthread_mutex_lock(&worker_lock);
    worker_queue.push(worker);
    worker_queue_size = worker_queue.size();
    pthread_mutex_unlock(&worker_lock);
  }

  Thread<Task> *get_worker() {
    Thread<Task> *worker = NULL;
    pthread_mutex_lock(&worker_lock);
    if (!worker_queue.empty()) {
      worker = worker_queue.front();
      worker_queue.pop();
    }
    worker_queue_size = worker_queue.size();
    pthread_mutex_unlock(&worker_lock);
    return worker;
  }

  int assign_task(Thread<Task> &thread) {
    int retval;
    pthread_mutex_lock(&task_lock);
    if (!task_queue.empty()) {
      thread.set_task(task_queue.front());
      task_queue.pop();
      retval = 1;
    } else {
      retval = 0;
    }
    task_queue_size = task_queue.size();
    pthread_mutex_unlock(&task_lock);
    return retval;
  }

  int queue_task(Task task) {
    pthread_mutex_lock(&task_lock);
    task_queue.push(task);
    task_queue_size = task_queue.size();
    pthread_mutex_unlock(&task_lock);
  }

public:
  pthread_mutex_t task_lock;
  pthread_mutex_t worker_lock;
  volatile size_t task_queue_size;

private:
  size_t max_workers;
  size_t max_tasks;

  std::queue<Task> task_queue;

  std::vector<Thread<Task> *> workers;
  std::queue<Thread<Task> *> worker_queue;
  volatile size_t worker_queue_size;
};

} // reiase

#endif /* THREADPOOL_H */
