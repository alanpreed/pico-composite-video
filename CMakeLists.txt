# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/usr/share/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-composite-video C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(pico-composite-video)

pico_generate_pio_header(${PROJECT_NAME}  
        ${CMAKE_CURRENT_LIST_DIR}/cvideo.pio
)
# add_executable(pico-composite-video main.c)

target_sources(pico-composite-video PRIVATE main.c cvideo.c renderer.c pong.c)


pico_set_program_name(pico-composite-video "pico-composite-video")
pico_set_program_version(pico-composite-video "0.1")

pico_enable_stdio_uart(pico-composite-video 1)

# Add the standard library to the build
target_link_libraries(pico-composite-video pico_stdlib)

# Add any user requested libraries
target_link_libraries(pico-composite-video
        hardware_dma
        hardware_pio
        hardware_timer
        hardware_clocks
        )

pico_add_extra_outputs(pico-composite-video)

