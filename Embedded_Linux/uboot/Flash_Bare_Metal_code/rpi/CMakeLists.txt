#
# CMakeLists.txt - Bare-Metal LED Blinky for Raspberry Pi 3B+
#

cmake_minimum_required(VERSION 3.16)

# Project definition
project(blinky
    VERSION 1.0.0
    DESCRIPTION "Bare-metal LED blinky for Raspberry Pi 3B+"
    LANGUAGES C ASM
)

# ============================================================
# Verify we're using the cross-compiler
# ============================================================
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
if(CMAKE_C_COMPILER MATCHES "x86|/usr/bin/cc|/usr/bin/gcc")
    message(FATAL_ERROR 
        "Wrong compiler detected! Using host compiler instead of cross-compiler.\n"
        "Please clean build directory and reconfigure:\n"
        "  rm -rf build && mkdir build && cd build\n"
        "  cmake -DCMAKE_TOOLCHAIN_FILE=../aarch64-rpi3-toolchain.cmake .."
    )
endif()

# ============================================================
# Configuration Options
# ============================================================

set(BINARY_NAME "blinky" CACHE STRING "Output binary name")
set(LED_GPIO_PIN 26 CACHE STRING "GPIO pin number for LED")
set(BLINK_DELAY 500000 CACHE STRING "Delay loop count for blinking")
set(LOAD_ADDRESS 0x80000 CACHE STRING "Memory address where binary is loaded")

# ============================================================
# Source Files
# ============================================================

set(ASM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.s
)

set(C_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

set(LINKER_SCRIPT
    ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

# ============================================================
# Compiler Flags
# ============================================================

set(COMMON_FLAGS
    -march=armv8-a
    -mcpu=cortex-a53
    -ffreestanding
    -nostdlib
    -nostartfiles
    -O2
    -Wall
    -Wextra
    -g
)

# ============================================================
# Build the ELF Executable
# ============================================================

add_executable(${BINARY_NAME}.elf
    ${C_SOURCES}
    ${ASM_SOURCES}
)

# Compile definitions
target_compile_definitions(${BINARY_NAME}.elf PRIVATE
    LED_GPIO_PIN=${LED_GPIO_PIN}
    BLINK_DELAY=${BLINK_DELAY}
)

# Compiler flags
target_compile_options(${BINARY_NAME}.elf PRIVATE
    ${COMMON_FLAGS}
)

# Linker flags
target_link_options(${BINARY_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostdlib
    -nostartfiles
    -Wl,-Map=${CMAKE_BINARY_DIR}/${BINARY_NAME}.map
)

# Linker script dependency
set_target_properties(${BINARY_NAME}.elf PROPERTIES
    LINK_DEPENDS ${LINKER_SCRIPT}
)

# ============================================================
# Post-Build Commands
# ============================================================

# Create raw binary image
add_custom_command(TARGET ${BINARY_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary 
            $<TARGET_FILE:${BINARY_NAME}.elf>
            ${CMAKE_BINARY_DIR}/${BINARY_NAME}.img
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${BINARY_NAME}.elf>
    COMMAND ${CMAKE_COMMAND} -E echo "Binary image created: ${BINARY_NAME}.img"
    COMMAND ls -la ${CMAKE_BINARY_DIR}/${BINARY_NAME}.img
    COMMENT "Creating binary image"
    VERBATIM
)

# Generate disassembly
add_custom_command(TARGET ${BINARY_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -d 
            $<TARGET_FILE:${BINARY_NAME}.elf>
            > ${CMAKE_BINARY_DIR}/${BINARY_NAME}.lst
    COMMENT "Generating disassembly"
    VERBATIM
)

# ============================================================
# Custom Targets
# ============================================================

add_custom_target(info
    COMMAND ${CMAKE_SIZE} ${CMAKE_BINARY_DIR}/${BINARY_NAME}.elf
    COMMAND ${CMAKE_OBJDUMP} -h ${CMAKE_BINARY_DIR}/${BINARY_NAME}.elf
    DEPENDS ${BINARY_NAME}.elf
    COMMENT "Binary information"
)

add_custom_target(verify
    COMMAND ${CMAKE_OBJDUMP} -f ${CMAKE_BINARY_DIR}/${BINARY_NAME}.elf
    COMMAND ${CMAKE_COMMAND} -E echo "--- First instructions ---"
    COMMAND ${CMAKE_OBJDUMP} -d ${CMAKE_BINARY_DIR}/${BINARY_NAME}.elf | head -30
    DEPENDS ${BINARY_NAME}.elf
    COMMENT "Verifying entry point"
)

add_custom_target(disasm
    COMMAND cat ${CMAKE_BINARY_DIR}/${BINARY_NAME}.lst
    DEPENDS ${BINARY_NAME}.elf
    COMMENT "Displaying disassembly"
)

add_custom_target(install_sd
    COMMAND ${CMAKE_COMMAND} -E echo "Looking for boot partition..."
    COMMAND /bin/bash -c "if [ -d /media/$ENV{USER}/boot ]; then \
        cp ${CMAKE_BINARY_DIR}/${BINARY_NAME}.img /media/$ENV{USER}/boot/ && \
        echo 'Copied to /media/$ENV{USER}/boot/${BINARY_NAME}.img'; \
    elif [ -d /media/$ENV{USER}/bootfs ]; then \
        cp ${CMAKE_BINARY_DIR}/${BINARY_NAME}.img /media/$ENV{USER}/bootfs/ && \
        echo 'Copied to /media/$ENV{USER}/bootfs/${BINARY_NAME}.img'; \
    else \
        echo 'Mount SD card first or copy manually:'; \
        echo '  cp ${CMAKE_BINARY_DIR}/${BINARY_NAME}.img /path/to/boot/'; \
    fi"
    DEPENDS ${BINARY_NAME}.elf
    COMMENT "Installing to SD card"
)

# ============================================================
# Configuration Summary
# ============================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "  Configuration Summary")
message(STATUS "========================================")
message(STATUS "  Binary name:    ${BINARY_NAME}")
message(STATUS "  LED GPIO pin:   ${LED_GPIO_PIN}")
message(STATUS "  Blink delay:    ${BLINK_DELAY}")
message(STATUS "  Load address:   ${LOAD_ADDRESS}")
message(STATUS "  C Compiler:     ${CMAKE_C_COMPILER}")
message(STATUS "  Linker script:  ${LINKER_SCRIPT}")
message(STATUS "========================================")
message(STATUS "")