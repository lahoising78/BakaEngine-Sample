cmake_minimum_required(VERSION 3.10)
project(Baka)

set(CMAKE_CXX_FLAGS "-Wall -Wpedantic -Werror")

add_subdirectory(engine)
add_subdirectory(sample)