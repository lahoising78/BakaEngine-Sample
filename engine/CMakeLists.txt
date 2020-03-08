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

target_include_directories(baka_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")