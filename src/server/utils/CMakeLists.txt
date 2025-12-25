find_package(OpenSSL REQUIRED)

add_library(utils STATIC
    utils.cpp
)

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(utils
    PUBLIC
        OpenSSL::Crypto
)