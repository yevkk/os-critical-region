cmake_minimum_required(VERSION 3.9)
project(lab2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Threads REQUIRED)

link_libraries(Threads::Threads)

add_executable(main main.cpp)
add_executable(demo_bakery_lock demo/demoBakeryLock.cpp)
add_executable(demo_dekker_lock demo/demoDekkerLock.cpp src/DekkerLock.cpp)

target_include_directories(demo_dekker_lock PUBLIC
        ${CMAKE_SOURCE_DIR}/src)


