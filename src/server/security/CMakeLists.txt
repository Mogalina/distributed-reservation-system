include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories($ENV{HOME}/jwt-cpp/include)

find_package(nlohmann_json REQUIRED)
include_directories(${nlohmann_json_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_library(security STATIC 
    jwt.cpp
)

target_include_directories(security 
    PUBLIC 
        $ENV{HOME}/jwt-cpp/include
        OpenSSL::SSL
        ${OPENSSL_INCLUDE_DIR}
        ${nlohmann_json_INCLUDE_DIRS}
)

target_link_libraries(security 
    PUBLIC 
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
)
