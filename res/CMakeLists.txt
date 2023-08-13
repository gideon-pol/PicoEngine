cmake_minimum_required(VERSION 3.13)
# set(RSRC_DIR ${CMAKE_CURRENT_LIST_DIR}/res)
# set(RSRC_BUILD_DIR ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/res)

project(embedding CXX C ASM)

set( RESOURCES "" )

# add_subdirectory(${PARENT_DIR})
# include_directories(include)
# file(GLOB_RECURSE SOURCES src/*.cpp)
# add_subdirectory(lib)

add_executable(
    embed 
    embed.cpp
    ${SOURCES}
)

set_property(TARGET embed PROPERTY CXX_STANDARD 20)
add_compile_definitions(PLATFORM_EMBED=1)

add_subdirectory(lib)

target_include_directories(embed PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(embed PUBLIC lodepng)
target_link_directories(embed PRIVATE lodepng)
target_link_libraries(
    embed
    lodepng
)

function( add_resource input )
    string( MAKE_C_IDENTIFIER ${input} input_identifier )
    set( output "generated/${input_identifier}.cpp" )
    add_custom_command(
        OUTPUT ${output}
        COMMAND embed ${output} ${input_identifier} ${CMAKE_CURRENT_LIST_DIR}/${input}
        DEPENDS ${input}
    )

    set( RESOURCES ${RESOURCES} ${output} PARENT_SCOPE )
endfunction()

# add_resource("cube.obj")
add_resource("suzanne.obj")
add_resource("sphere.obj")
add_resource("quad.obj")
add_resource("test.png")
add_resource("dirt.png")
add_resource("earth.png")


file(MAKE_DIRECTORY build/generated)
include_directories(resources PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_library(resources STATIC ${RESOURCES})

# set_property(TARGET resources PROPERTY CXX_STANDARD 20)