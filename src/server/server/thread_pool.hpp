#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace server {

// A simple fixed-size thread pool for executing asynchronous tasks.
//
// The ThreadPool maintains a set of worker threads that continuously wait for 
// tasks to be added to a shared queue. When a task is available, one worker 
// thread picks it up and executes it.
class ThreadPool {
public:
  // Constructs a ThreadPool with a fixed number of worker threads.
  //
  // Each worker thread runs in an infinite loop, waiting for tasks to become
  // available or for the pool to be stopped.
  explicit ThreadPool(size_t numThreads);

  // Adds a new task to the thread pool.
  //
  // The task is pushed into the queue and one waiting worker thread is 
  // notified.
  template<class F>
  void enqueue(F&& f) {
    {
      // Lock the queue to safely add a new task
      std::unique_lock<std::mutex> lock(queueMutex);
      tasks.emplace(std::forward<F>(f));
    }

    // Notify one worker that a new task is available
    condition.notify_one();
  }

  // Destructor that shuts down the thread pool.
  //
  // Signals all worker threads to stop, wakes them up, and waits for their
  // completion (join).
  ~ThreadPool();

private:
  // Worker threads managed by the pool
  std::vector<std::thread> workers;

  // Queue of pending tasks
  std::queue<std::function<void()>> tasks;

  // Mutex protecting access to the task queue
  std::mutex queueMutex;

  // Condition variable for task availability and shutdown signaling
  std::condition_variable condition;

  // Flag indicating whether the pool is stopping
  bool stop;
};

}  // namespace server
