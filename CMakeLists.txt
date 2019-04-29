cmake_minimum_required(VERSION 2.6)

project(EchoServer)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-pthread")

add_executable(EchoServer
        ./base
        server_thread.cpp)
