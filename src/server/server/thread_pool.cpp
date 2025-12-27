#include "thread_pool.hpp"

namespace server {

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
  for (size_t i = 0; i < numThreads; ++i) {
    workers.emplace_back([this] {
      for (;;) {
        std::function<void()> task;
        {
          // Acquire lock to safely access the task queue
          std::unique_lock<std::mutex> lock(this->queueMutex);

          // Wait until there is a task to execute or the pool is stopping
          this->condition.wait(lock, [this] {
            return this->stop || !this->tasks.empty();
          });

          // If stopping and no tasks remain, exit the thread
          if (this->stop && this->tasks.empty()) {
            return;
          }

          // Retrieve the next task from the queue
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }

        // Execute the task outside the lock
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    // Signal all threads to stop processing
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
  }

  // Wake up all worker threads
  condition.notify_all();

  // Wait for all threads to finish
  for (std::thread &worker : workers) {
    worker.join();
  }
}

}  // namespace server
