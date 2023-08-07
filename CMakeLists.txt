cmake_minimum_required(VERSION 3.13)

option(TARGET_NATIVE "Build for build platform" OFF)

if(TARGET_NATIVE)
    project(game-engine CXX C ASM)

    include_directories(include)
    file(GLOB_RECURSE SOURCES src/*.cpp)
    add_subdirectory(lib)
    add_subdirectory(res)

    add_executable(build
        native_main.cpp
        ${SOURCES}
        # ${RESOURCES}
    )

    find_package(SDL2 REQUIRED)

    target_include_directories(build PUBLIC include)
    target_include_directories(build PUBLIC lodepng)
    target_include_directories(build PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_directories(build PRIVATE lodepng)
    target_link_directories(build PRIVATE resources)
    set_target_properties(build PROPERTIES COMPILE_FLAGS "-g -lSDL2" )

    target_link_libraries(
        build
        lodepng
        resources
        ${SDL2_LIBRARIES}
    )
else()
    include(pico-sdk/pico_sdk_init.cmake)

    project(game-engine CXX C ASM)

    pico_sdk_init()

    include_directories(include)
    file(GLOB_RECURSE SOURCES src/*.cpp)
    add_subdirectory(lib)
    add_subdirectory(res)

    add_executable(build
        main.cpp
        ${SOURCES}
        # ${RESOURCES}
    )

    target_link_directories(build PRIVATE resources)

    target_link_libraries(
        build
        pico_stdlib
        pico_multicore
        resources
    )

    pico_add_extra_outputs(build)
    target_compile_options(build PRIVATE -DPARAM_ASSERTIONS_ENABLE_ALL=1)

    if (TARGET tinyusb_device)
        pico_enable_stdio_usb(build 1)
        pico_enable_stdio_uart(build 0)
    elseif(PICO_ON_DEVICE)
        message(WARNING "not building because TinyUSB submodule is not initialized in the SDK")
    endif()
endif()



# function( add_resource input )
#     string( MAKE_C_IDENTIFIER ${input} input_identifier )
#     set( output "res/${input_identifier}.cpp" )
#     set( RESOURCES ${RESOURCES} ${output} PARENT_SCOPE )
# endfunction()

# add_subdirectory(res)

# add_resource("teapot.obj")




# 




