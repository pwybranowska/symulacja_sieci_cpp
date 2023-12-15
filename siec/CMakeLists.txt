cmake_minimum_required(VERSION 3.24)
project(siec)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        include

)

set(SOURCE_FILES
        src/factory.cpp
        src/package.cpp
        src/helpers.cpp
        src/nodes.cpp
        src/storage_types.cpp

        )

add_executable(siec ${SOURCE_FILES} main.cpp)
