cmake_minimum_required(VERSION 3.16)

project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(server)
add_subdirectory(utils)
add_subdirectory(database)
add_subdirectory(middleware)
add_subdirectory(security)
add_subdirectory(models)
add_subdirectory(repository)
add_subdirectory(service)
add_subdirectory(http)
add_subdirectory(controller)

add_executable(runner main.cpp)

target_link_libraries(runner
    PRIVATE
        server
        utils
        database
        middleware
        security
        models
        repository
        service
        http
        controller
)
