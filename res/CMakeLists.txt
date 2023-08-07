# set(RSRC_DIR ${CMAKE_CURRENT_LIST_DIR}/res)
# set(RSRC_BUILD_DIR ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/res)

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


target_include_directories(embed PUBLIC include)
target_include_directories(embed PUBLIC lodepng)
target_link_directories(embed PRIVATE lodepng)
target_link_libraries(
    embed
    lodepng
)

function( add_resource input )
    string( MAKE_C_IDENTIFIER ${input} input_identifier )
    set( output "${input_identifier}.cpp" )
    add_custom_command(
        OUTPUT ${output}
        COMMAND embed ${output} ${input_identifier} ${CMAKE_CURRENT_LIST_DIR}/${input}
        DEPENDS ${input}
    )

    set( RESOURCES ${RESOURCES} ${output} PARENT_SCOPE )
endfunction()

add_resource("cube.obj")
add_resource("suzanne.obj")
add_resource("test.png")
add_resource("dirt.png")

add_library(resources STATIC ${RESOURCES})