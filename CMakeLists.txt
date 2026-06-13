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

# == Bluetooth ==
# set bluepad32 root
set(BLUEPAD32_ROOT ./lib/bluepad32)
# Use BTstack from Pico SDK
set(BTSTACK_ROOT ${PICO_SDK_PATH}/lib/btstack)
# ===============

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
 my_platform.c
)

# Add the standard include files to the build
target_include_directories(TestStar PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${BLUEPAD32_ROOT}/src/components/bluepad32/include
)

pico_set_program_name(TestStar "TestStar")
pico_set_program_version(TestStar "0.1")

# Needed for btstack_config.h / sdkconfig.h
# so that libbluepad32 can include them
include_directories(TestStar ./)

# Needed when using BTstack
include_directories(${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/include)
include_directories(${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/include)
# Need for certain IDEs, like CLion. Otherwise it won't find btstack
include_directories(${BTSTACK_ROOT}/src)

# Add the standard library to the build
target_link_libraries(TestStar
    pico_stdlib
    pico_multicore
    pico_cyw43_arch_none
    pico_btstack_classic
    pico_btstack_cyw43
    bluepad32
    hardware_clocks
    hardware_dma
    hardware_irq
    hardware_i2c
    pico_hdmi
)

add_subdirectory(${BLUEPAD32_ROOT}/src/components/bluepad32 libbluepad32)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(TestStar 0)
pico_enable_stdio_usb(TestStar 1)

pico_add_extra_outputs(TestStar)

