cmake_minimum_required(VERSION 3.10)
project(EpollServer)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

add_executable(epoll_server src/main.cpp src/server.cpp)
