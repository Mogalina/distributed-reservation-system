add_library(service STATIC
    user_service.cpp
)

target_include_directories(service
    PUBLIC
        ${CMAKE_CURRENT_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(service 
    PUBLIC 
        repository
        models
)
