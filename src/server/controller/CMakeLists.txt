find_package(nlohmann_json REQUIRED)
include_directories(${nlohmann_json_INCLUDE_DIRS})

add_library(controller STATIC 
    user_controller.cpp
    auth_controller.cpp
)

target_include_directories(controller 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        nlohmann_json::nlohmann_json
)

target_link_libraries(controller 
    PUBLIC 
        service
        security
        middleware
        nlohmann_json::nlohmann_json
)
