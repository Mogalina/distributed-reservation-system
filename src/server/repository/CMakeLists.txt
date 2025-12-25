add_library(repository STATIC
    user_repository.cpp
)

target_include_directories(repository 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(repository 
    PUBLIC 
        database
        models
)
