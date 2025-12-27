#pragma once

#include "repository/event_repository.hpp"
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>

namespace service {

// Periodic background worker for maintenance tasks.
//
// The service can be started and stopped safely and is designed to run 
// independently of the main application flow.
class BackgroundService {
public:
  BackgroundService(repository::EventRepository& repo) 
    : repo_(repo), running_(false) {}

  // Starts the background worker thread.
  //
  // This method launches a new thread that periodically checks for expired 
  // reservations and generates stock reports.
  void start(int checkIntervalMs = 5000, int reservationTimeoutSec = 5);

  // Stops the background worker thread.
  //
  // Signals the worker thread to stop and waits for it to terminate.
  void stop();

private:
  // Writes a system stock report to the log file
  void saveReport();

  // Reference to the event repository
  repository::EventRepository& repo_;

  // Indicates whether the background thread should continue running
  std::atomic<bool> running_;

  // Worker thread executing background tasks
  std::thread worker_;
};

}  // namespace service