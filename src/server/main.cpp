#include <iostream>
#include "utils/utils.hpp"
#include "server/server.hpp"
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

  // Initialize the server
  server::Server server(variables["SERVER_HOST"], 
                        std::stoi(variables["SERVER_PORT"]),
                        authController,
                        eventController);

  // Start listening for incoming connections                      
  server.start();                      

  return 0;
}
