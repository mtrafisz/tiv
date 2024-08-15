cmake_minimum_required(VERSION 3.15)
project(tiv)

if (WIN32)
    message (FATAL_ERROR "No software for you, windows user :3")
endif()

add_library(sha256 STATIC "third-party/crypto/sha256.c")
add_library(logger STATIC "third-party/simple-linux-logger/logger.c")
add_library(array STATIC "third-party/array/car.c")

file (GLOB_RECURSE LOCAL_SOURCES "src/*.c")
add_executable(${PROJECT_NAME} ${LOCAL_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    "third-party/crypto"
    "third-party/simple-linux-logger"
    "third-party/array"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    sha256
    logger
    array
)
