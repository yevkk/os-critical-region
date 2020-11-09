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
add_executable(test tests/test.cpp src/DekkerLock.cpp)

target_include_directories(demo_dekker_lock PUBLIC
        ${CMAKE_SOURCE_DIR}/src)
target_include_directories(test PUBLIC
        ${CMAKE_SOURCE_DIR}/tests/utils
        ${CMAKE_SOURCE_DIR}/src)

<<<<<<< HEAD

=======
target_link_libraries(main Threads::Threads)
target_link_libraries(demo_bakery_lock Threads::Threads)
target_link_libraries(demo_dekker_lock Threads::Threads)
target_link_libraries(test Threads::Threads)
>>>>>>> fa11eb9... [Task-1] Implement mutual exclusion checker, which tests whether mutual exclusion is working in Lockable primitives
