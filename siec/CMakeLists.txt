cmake_minimum_required(VERSION 3.24)
project(siec)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(siec main.cpp
            src/factory.cpp)
