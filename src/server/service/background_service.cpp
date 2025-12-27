#include "background_service.hpp"

namespace service {

void BackgroundService::start(int checkIntervalMs, int reservationTimeoutSec) {
  running_ = true;

  // Launch background worker thread
  worker_ = std::thread([this, checkIntervalMs, reservationTimeoutSec]() {
    while (running_) {
      // Wait before next execution cycle
      std::this_thread::sleep_for(
        std::chrono::milliseconds(checkIntervalMs)
      );

      // Cancel expired reservations
      int cancelled = repo_.cancelExpiredReservations(reservationTimeoutSec);
      if (cancelled > 0) {
        std::cout << "[Background] Cancelled " 
                  << cancelled << " expired reservations.\n";
      }

      // Save system status report
      saveReport();
    }
  });
}

void BackgroundService::stop() {
  running_ = false;

  // Ensure clean thread shutdown
  if (worker_.joinable()) {
    worker_.join();
  }
}

void BackgroundService::saveReport() {
  std::ofstream logFile("system_status.log", std::ios::app);
  
  if (logFile.is_open()) {
    time_t now = time(0);

    // Write timestamp
    logFile << "Time: " << ctime(&now);

    // Write stock report
    logFile << repo_.generateStockReport();

    // Separator for readability
    logFile << "-----------------------------\n";

    logFile.close();
  }
}

}  // namespace service
