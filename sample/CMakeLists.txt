# project(Sample)

# file(
#     GLOB sample_src
#     "include/*.h"
#     "src/*.cpp"
# )

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/build/")
# add_executable(app "${sample_src}")

# target_link_libraries(app PUBLIC baka_engine)

cmake_minimum_required(VERSION 3.10)
project(Sample)

# -Wpedantic -Werror --verbose")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic") 
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror") 
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --verbose") 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/")

add_subdirectory(../engine "${CMAKE_CURRENT_SOURCE_DIR}/build/baka_engine/")

file(
    GLOB sample_src
    "include/*.h"
    "src/*.cpp"
)

add_executable(app "${sample_src}")
target_link_libraries(app PUBLIC baka_engine)