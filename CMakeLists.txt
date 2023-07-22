cmake_minimum_required(VERSION 3.13)

# set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)


# if(EXISTS GEN_DIR)
# 	message("Creating generated output directory '${CMAKE_CURRENT_BINARY_DIR}/${GEN_DIR}'")
# 	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${GEN_DIR}")
# endif()

set( RESOURCES "" )

add_executable(embed embed.cpp)

function( add_resource input )
    

    string( MAKE_C_IDENTIFIER ${input} input_identifier )
    set( output "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/res/${input_identifier}" )
    add_custom_command(
        OUTPUT ${output}.cpp
        COMMAND mkdir -p res
        COMMAND embed res/${input_identifier} ${CMAKE_CURRENT_LIST_DIR}/res/${input}
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/res/${input}
    )

    set( RESOURCES ${RESOURCES} ${output}.cpp PARENT_SCOPE )
endfunction()

add_resource("main.font")


# include(pico-sdk/pico_sdk_init.cmake)

# project(game-engine CXX C ASM)

# pico_sdk_init()

# include_directories(include)
# file(GLOB SOURCES src/*.cpp)
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



# Resource file list

project(game-engine CXX C ASM)

include_directories(include)
file(GLOB SOURCES src/*.cpp)
add_subdirectory(lib)


add_executable(native
    native_main.cpp
    ${SOURCES}
    ${RESOURCES}
    # data.c
)



target_include_directories(native PUBLIC lodepng)

target_link_directories(native PRIVATE lodepng)

target_link_libraries(
    native
    lodepng
)

# function( add_resource input )
#     string( MAKE_C_IDENTIFIER ${input} input_identifier )
#     set( output "${CMAKE_CURRENT_LIST_DIR}/build/res/${input_identifier}.o" )
#     target_link_libraries( native ${output} )

#     add_custom_command(
#         OUTPUT ${output}
#         COMMAND ${CMAKE_LINKER} --relocatable --format binary --output ${output} ${CMAKE_CURRENT_LIST_DIR}/${input}
#         DEPENDS ${input}
#     )

#     set( RC_DEPENDS ${RC_DEPENDS} ${output} PARENT_SCOPE )
# endfunction()

#define an add_resource function that uses "embed.cpp" to


add_custom_target(resources ALL DEPENDS ${RESOURCES})