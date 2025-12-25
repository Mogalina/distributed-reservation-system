add_library(middleware INTERFACE)

target_include_directories(middleware 
    INTERFACE 
        ${CMAKE_CURRENT_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)
