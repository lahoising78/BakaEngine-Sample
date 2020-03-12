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

find_package(Vulkan REQUIRED)

target_include_directories(baka_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(baka_engine ${SDL2_LIBRARIES})

target_include_directories(baka_engine PRIVATE Vulkan::Vulkan)
target_link_libraries(baka_engine Vulkan::Vulkan)

file(
    GLOB shaders
    "shaders/*.sprv"
)

foreach( f ${shaders} )
    get_filename_component(filename "${f}" NAME)
    set(dst "${CMAKE_HOME_DIRECTORY}/build/baka_engine/shaders/${filename}")
    message("copy ${f} to ${dst}")
    configure_file("${f}" "${dst}" COPYONLY )
endforeach()

# configure_file("shaders/default_vert.sprv" "${CMAKE_HOME_DIRECTORY}/build/baka_engine/shaders/vert.sprv" COPYONLY)
# configure_file("shaders/default_frag.sprv" "${CMAKE_HOME_DIRECTORY}/build/baka_engine/shaders/frag.sprv" COPYONLY)