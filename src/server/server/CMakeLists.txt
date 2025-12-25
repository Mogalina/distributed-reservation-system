add_library(server STATIC
    server.cpp
)

target_include_directories(server
    PUBLIC
        ${CMAKE_CURRENT_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(server
    PRIVATE
        _POSIX_C_SOURCE=200809L
)

target_link_libraries(server
    PUBLIC
        controller
        http
)