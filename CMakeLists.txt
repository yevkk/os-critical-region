cmake_minimum_required(VERSION 3.9)
project(lab2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Threads REQUIRED)

link_libraries(Threads::Threads)

set(TOP_DIR ${CMAKE_SOURCE_DIR})
set(SRC_DIR ${TOP_DIR}/src)

add_executable(main main.cpp)
add_executable(demo_bakery_lock demo/demoBakeryLock.cpp)
add_executable(demo_dekker_lock demo/demoDekkerLock.cpp ${SRC_DIR}/DekkerLock.cpp)
add_executable(test tests/test.cpp ${SRC_DIR}/DekkerLock.cpp)

target_include_directories(demo_dekker_lock PUBLIC ${SRC_DIR})
target_include_directories(test PUBLIC
        ${TOP_DIR}/tests/utils
        ${SRC_DIR}
        )

set(SRC_LIST
        ${SRC_DIR}/BakeryLock.hpp
        ${SRC_DIR}/ImprovedBakeryLock.hpp
        ${SRC_DIR}/LockPrimitives.hpp
        )

add_executable(benchmark demo/benchmark.cpp)
target_include_directories(benchmark PUBLIC ${CMAKE_SOURCE_DIR}/src})
