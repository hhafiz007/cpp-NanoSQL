cmake_minimum_required(VERSION 3.13)
project(git-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

set(SOURCE_FILES src/Server.cpp)
list(APPEND SOURCE_FILES src/indexParser.cpp)

add_executable(server ${SOURCE_FILES})