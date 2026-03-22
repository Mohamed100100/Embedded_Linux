cmake_minimum_required(VERSION 3.10)

# ============================================
# COMPILER SETTINGS (Set before project())
# ============================================
# Compiler path and prefix for cross-compilation
set(COMPILER_PATH "/usr/bin" CACHE PATH "Path to compiler directory")
set(COMPILER_PREFIX "" CACHE STRING "Compiler prefix (e.g., aarch64-rpi3-linux-gnu-)")

# Set compilers with prefix support
if(COMPILER_PREFIX)
    set(CMAKE_C_COMPILER "${COMPILER_PATH}/${COMPILER_PREFIX}gcc" CACHE FILEPATH "C Compiler" FORCE)
    set(CMAKE_CXX_COMPILER "${COMPILER_PATH}/${COMPILER_PREFIX}g++" CACHE FILEPATH "C++ Compiler" FORCE)
else()
    set(CMAKE_C_COMPILER "${COMPILER_PATH}/gcc" CACHE FILEPATH "C Compiler" FORCE)
    set(CMAKE_CXX_COMPILER "${COMPILER_PATH}/g++" CACHE FILEPATH "C++ Compiler" FORCE)
endif()

project(SevenSegmentProject)

set(CMAKE_CXX_STANDARD 17)

# ============================================
# SCP DEPLOY SETTINGS
# ============================================
set(REMOTE_USER "user" CACHE STRING "Remote username for SCP")
set(REMOTE_IP "192.168.1.100" CACHE STRING "Remote IP address for SCP")
set(REMOTE_PATH "/home/user/deploy" CACHE PATH "Remote destination path")

# Print configuration info
message(STATUS "===========================================")
message(STATUS "C Compiler:   ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Remote:       ${REMOTE_USER}@${REMOTE_IP}:${REMOTE_PATH}")
message(STATUS "===========================================")

# ============================================
# BUILD SUBDIRECTORIES
# ============================================
add_subdirectory(src)
add_subdirectory(app)

# ============================================
# CUSTOM SCP DEPLOY TARGET
# ============================================
add_custom_target(deploy
    COMMENT "Deploying to ${REMOTE_USER}@${REMOTE_IP}:${REMOTE_PATH}"
    COMMAND scp $<TARGET_FILE:MainApp> ${REMOTE_USER}@${REMOTE_IP}:${REMOTE_PATH}/
    COMMAND scp $<TARGET_FILE:SevenSegmentLib> ${REMOTE_USER}@${REMOTE_IP}:${REMOTE_PATH}/
    DEPENDS MainApp SevenSegmentLib
)

add_custom_target(deploy-app
    COMMENT "Deploying MainApp to ${REMOTE_USER}@${REMOTE_IP}:${REMOTE_PATH}"
    COMMAND scp $<TARGET_FILE:MainApp> ${REMOTE_USER}@${REMOTE_IP}:${REMOTE_PATH}/
    DEPENDS MainApp
)

add_custom_target(deploy-lib
    COMMENT "Deploying Library to ${REMOTE_USER}@${REMOTE_IP}:${REMOTE_PATH}"
    COMMAND scp $<TARGET_FILE:SevenSegmentLib> ${REMOTE_USER}@${REMOTE_IP}:${REMOTE_PATH}/
    DEPENDS SevenSegmentLib
)