cmake_minimum_required (VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O3")
project(SC2_ZERG)

find_package (Threads)

add_executable(
main
main.cpp
utility.cpp
EntityData.cpp
Simulation.cpp
Entity.cpp
EventQueue.cpp
Event.cpp
)

target_include_directories(
main
PRIVATE
lib
lib/fixed_point/include/
)

target_link_libraries(
main
${CMAKE_THREAD_LIBS_INIT}
)
