cmake_minimum_required(VERSION 3.10)

# set(CMAKE_CXX_FLAGS "-Wall -Wpedantic -Werror")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/")

add_subdirectory(../engine "${CMAKE_CURRENT_SOURCE_DIR}/build/baka_engine/")
project(Sample)

file(
    GLOB sample_src
    "include/*.h"
    "src/*.cpp"
)

add_executable(app "${sample_src}")
target_include_directories(app PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(app PUBLIC baka_engine)
baka_import_dlls("app")