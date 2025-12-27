cmake_minimum_required(VERSION 3.16)

project(simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(simulation simulation.cpp)

target_compile_definitions(simulation
    PRIVATE
        _POSIX_C_SOURCE=200809L
)

target_link_libraries(simulation 
    PRIVATE 
        Threads::Threads)
