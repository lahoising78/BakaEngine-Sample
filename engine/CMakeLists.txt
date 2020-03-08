project(Baka)

file(
    GLOB baka_src
    "include/*.h"
    "src/*.cpp"
)

add_library(
    baka_engine
    "${baka_src}"
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_include_directories(baka_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(baka_engine ${SDL2_LIBRARIES})