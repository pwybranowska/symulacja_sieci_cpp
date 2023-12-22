cmake_minimum_required(VERSION 3.24)
project(siec)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Wconversion -Wno-error=pessimizing-move)

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

set(SOURCES_FILES_TESTS
        test/main_gtest.cpp

        test/test_nodes.cpp
        test/test_package.cpp
        test/test_storage_types.cpp
        test/test_Factory.cpp
#        test/test_factory_io.cpp
)

add_executable(siec_test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

target_include_directories(siec_test PUBLIC
        $googletest-master/googlemock/include
        $googletest-master/googletest/include
        mocks
)

add_subdirectory(googletest-master)

target_link_libraries(siec_test gmock)