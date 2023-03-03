set(CMAKE_C_STANDARD 20)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion")

cmake_minimum_required(VERSION 3.2)

project(snake)

include_directories(lib/console-engine/include)

add_subdirectory(lib/console-engine)

add_executable(snake src/main.cc)

target_link_libraries(snake console)
