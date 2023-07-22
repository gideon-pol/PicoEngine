cmake_minimum_required(VERSION 3.13)


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


project(game-engine CXX C ASM)

include_directories(include)
file(GLOB SOURCES src/*.cpp)
add_subdirectory(lib)


add_executable(native
    native_main.cpp
    ${SOURCES}
)

target_include_directories(native PUBLIC lodepng)

target_link_directories(native PRIVATE lodepng)

target_link_libraries(
    native
    lodepng
)

