cmake_minimum_required(VERSION 3.5)

project(radRacer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-lncursesw")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(radRacer main.cpp)

target_link_libraries(radRacer PRIVATE Threads::Threads)
