# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(TestStar C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Use runtime HDMI mode switching
set(PICO_HDMI_RT_RUNTIME_MODE_ATTRS ON CACHE BOOL "" FORCE)
set(PICO_HDMI_RUNTIME_MODES ON CACHE BOOL "" FORCE)

add_subdirectory(lib/pico_hdmi)

# Add executable. Default name is the project name, version 0.1
add_executable(TestStar
 main.c
 audio.c
 video.c
)

pico_set_program_name(TestStar "TestStar")
pico_set_program_version(TestStar "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(TestStar 0)
pico_enable_stdio_usb(TestStar 1)
# Optional: build at 1280x720 @ 60 Hz (requires overclock to 372 MHz @ 1.3V).
# Default is 640x480 @ 60 Hz at the stock 126 MHz clock.
# Enable with: cmake -DBOUNCING_BOX_720P=ON ..
option(TestStar_720P "Build TestStar in 1280x720 mode (requires overclock)" OFF)
if (TestStar_720P)
    target_compile_definitions(TestStar PRIVATE VIDEO_MODE_1280x720)
    target_compile_definitions(pico_hdmi PRIVATE VIDEO_MODE_1280x720)
endif()

# Add the standard library to the build
target_link_libraries(TestStar
    pico_stdlib
    pico_multicore
    hardware_clocks
    hardware_dma
    hardware_irq
    hardware_i2c
    pico_hdmi
)

# Add the standard include files to the build
target_include_directories(TestStar PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(TestStar)

