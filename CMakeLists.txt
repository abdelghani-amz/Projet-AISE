project(AISE)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_library(commands commands.cpp)
add_executable(server  server.cpp)
target_link_libraries(server  commands)
target_link_libraries(server  pthread)