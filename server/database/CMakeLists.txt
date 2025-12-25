add_library(database STATIC
    sqlite_database.cpp
)

target_include_directories(database
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(SQLite3 REQUIRED)

target_link_libraries(database
    PRIVATE
        SQLite::SQLite3
)
