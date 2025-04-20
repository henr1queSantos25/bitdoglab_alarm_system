# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(bitdoglab_alarm_system C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(bitdoglab_alarm_system bitdoglab_alarm_system.c libs/ssd1306.c libs/buttons.c libs/led_5x5.c libs/buzzer.c libs/ledRGB.c)

pico_generate_pio_header(bitdoglab_alarm_system ${CMAKE_CURRENT_LIST_DIR}/bitdoglab_alarm_system.pio)

pico_set_program_name(bitdoglab_alarm_system "bitdoglab_alarm_system")
pico_set_program_version(bitdoglab_alarm_system "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(bitdoglab_alarm_system 1)
pico_enable_stdio_usb(bitdoglab_alarm_system 1)

# Add the standard library to the build
target_link_libraries(bitdoglab_alarm_system
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(bitdoglab_alarm_system PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(bitdoglab_alarm_system 
        hardware_pio
        hardware_adc
        hardware_i2c
        hardware_pwm)

pico_add_extra_outputs(bitdoglab_alarm_system)

