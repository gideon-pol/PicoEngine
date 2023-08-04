cmake_minimum_required(VERSION 3.13)

set(RSRC_DIR ${CMAKE_CURRENT_LIST_DIR}/res)
set(RSRC_BUILD_DIR ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/res)

set( RESOURCES "" )


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
    set( output "res/${input_identifier}.cpp" )
    add_custom_command(
        OUTPUT ${output}
        # TODO: generate using CMake rather than Linux shell
        COMMAND mkdir -p res
        COMMAND embed res ${input_identifier} ${RSRC_DIR}/${input}
        DEPENDS res/${input}
    )

    set( RESOURCES ${RESOURCES} ${output} PARENT_SCOPE )
endfunction()

add_resource("cube.obj")
add_resource("suzanne.obj")
add_resource("test.png")
add_resource("dirt.png")
# add_resource("main.font")

# include(pico-sdk/pico_sdk_init.cmake)

# project(game-engine CXX C ASM)

# pico_sdk_init()

# include_directories(include)
# file(GLOB_RECURSE SOURCES src/*.cpp)
# add_subdirectory(lib)

# add_executable(pico-build
#     main.cpp
#     ${SOURCES}
# )

# target_link_libraries(
#     pico-build
#     pico_stdlib
#     pico_multicore
# )

# pico_add_extra_outputs(pico-build)
# target_compile_options(pico-build PRIVATE -DPARAM_ASSERTIONS_ENABLE_ALL=1)

# if (TARGET tinyusb_device)
#     pico_enable_stdio_usb(pico-build 1)
#     pico_enable_stdio_uart(pico-build 0)
# elseif(PICO_ON_DEVICE)
#     message(WARNING "not building because TinyUSB submodule is not initialized in the SDK")
# endif()


project(game-engine CXX C ASM)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_subdirectory(lib)

add_executable(native
    native_main.cpp
    ${SOURCES}
    ${RESOURCES}
)

target_include_directories(native PUBLIC include)
target_include_directories(native PUBLIC lodepng)
target_link_directories(native PRIVATE lodepng)
set_target_properties(native PROPERTIES COMPILE_FLAGS "-g" )

target_link_libraries(
    native
    lodepng
)




