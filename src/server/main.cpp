#include <iostream>
#include "utils/utils.hpp"
#include "server/server.hpp"
#include "server/thread_pool.hpp"
#include "service/background_service.hpp"
#include "database/sqlite_database.hpp"
#include "repository/user_repository.hpp"
#include "repository/event_repository.hpp"
#include "service/user_service.hpp"
#include "service/event_service.hpp"
#include "controller/auth_controller.hpp"
#include "controller/event_controller.hpp"
#include "security/jwt.hpp"

int main() {
  // Load environment variables
  std::unordered_map<std::string, std::string> variables;
  utils::parseEnvironmentFile(".env", variables);

  // Initialize database
  database::SqliteDatabase db;
  if (!db.open("database/reservations.db")) {
    std::cerr << "Failed to open database" << std::endl;
    return 1;
  }

  // Initialize security
  security::Security security(variables["JWT_SECRET_KEY"]);

  // Initialize repositories
  repository::UserRepository userRepository(db);
  repository::EventRepository eventRepository(db);

  // Initialize services
  service::UserService userService(userRepository);
  service::EventService eventService(eventRepository, userRepository);

  // Initialize controllers
  controller::AuthController authController(userService, security);
  controller::EventController eventController(eventService, security);

  // Initialize background service
  service::BackgroundService backgroundService(eventRepository);

  // Start background service
  backgroundService.start(std::stoi(variables["BG_CHECK_INTERVAL_MS"]),
                          std::stoi(variables["BG_RESERVATION_TIME_S"]));

  // Initialize the server
  server::Server server(variables["SERVER_HOST"], 
                        std::stoi(variables["SERVER_PORT"]),
                        authController,
                        eventController);

  // Start listening for incoming connections                      
  std::thread serverThread([&server](){ server.start(); });
  
  // Stop server after predefined period of time
  std::cout << "Server will be running " 
            << std::stoi(variables["SERVER_RUNTIME_MIN"]) << " minutes...\n";
  std::this_thread::sleep_for(
    std::chrono::minutes(std::stoi(variables["SERVER_RUNTIME_MIN"]))
  );
  
  std::cout << "Stopping server...\n";
  backgroundService.stop();
  server.stop();

  exit(0);
}
