cmake_minimum_required(VERSION 3.13)

option(TARGET_NATIVE "Build for build platform" OFF)

# clean target that clears the build folder



if(TARGET_NATIVE)
    project(game-engine CXX C ASM)

    include_directories(include)
    file(GLOB_RECURSE SOURCES src/*.cpp)
    file(GLOB_RECURSE RESOURCES res/build/generated/*.cpp)
    add_subdirectory(lib)

    add_executable(build
        native_main.cpp
        ${SOURCES}
        ${RESOURCES}
    )

    find_package(SDL2 REQUIRED)

    target_include_directories(build PUBLIC include)
    target_include_directories(build PUBLIC lodepng)
    target_include_directories(build PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_directories(build PRIVATE lodepng)
    set_target_properties(build PROPERTIES COMPILE_FLAGS "-g -lSDL2")
    set_target_properties(build PROPERTIES LINK_FLAGS "-fsanitize=undefined")
    set_property(TARGET build PROPERTY CXX_STANDARD 20)
    add_compile_definitions(PLATFORM_NATIVE=1)

    target_link_libraries(
        build
        lodepng
        ${SDL2_LIBRARIES}
    )
else()
    include(pico-sdk/pico_sdk_init.cmake)

    project(game-engine CXX C ASM)

    pico_sdk_init()

    include_directories(include)
    file(GLOB_RECURSE SOURCES src/*.cpp)
    file(GLOB_RECURSE RESOURCES res/build/generated/*.cpp)
    # remove the CMakeCXXCompilerId.cpp 

    add_subdirectory(lib)
    # add_subdirectory(res)

    add_executable(build
        main.cpp
        ${SOURCES}
        ${RESOURCES}
    )

    # target_link_directories(build PRIVATE resources)

    target_link_libraries(build
        pico_stdlib
        pico_multicore
        hardware_gpio
        hardware_pio
        hardware_pwm
        hardware_spi
        hardware_dma
        hardware_adc
        
        # ${CMAKE_CURRENT_SOURCE_DIR}/res/build/libresources.a
    )

    pico_generate_pio_header(build 
        ${CMAKE_CURRENT_LIST_DIR}/src/hardware/screen_double.pio
    )

    pico_add_extra_outputs(build)
    target_compile_options(build PRIVATE -DPARAM_ASSERTIONS_ENABLE_ALL=1)
    add_compile_definitions(PLATFORM_PICO=1)
    target_compile_features(build PUBLIC cxx_std_20)

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




