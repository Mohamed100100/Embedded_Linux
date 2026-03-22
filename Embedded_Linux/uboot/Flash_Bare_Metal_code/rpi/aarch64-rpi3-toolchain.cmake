#
# CMake Toolchain File for Raspberry Pi 3B+ (AArch64 Bare-Metal)
#

# MUST be set before project() is called
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Toolchain path
set(TOOLCHAIN_PATH "$ENV{HOME}/x-tools/aarch64-rpi3-linux-gnu/bin")
set(CROSS_COMPILE "aarch64-rpi3-linux-gnu-")

# Force the compilers (use CACHE FORCE to override any cached values)
set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/${CROSS_COMPILE}gcc" CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PATH}/${CROSS_COMPILE}gcc" CACHE FILEPATH "ASM compiler" FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/${CROSS_COMPILE}g++" CACHE FILEPATH "C++ compiler" FORCE)

# Toolchain utilities
set(CMAKE_OBJCOPY "${TOOLCHAIN_PATH}/${CROSS_COMPILE}objcopy" CACHE FILEPATH "objcopy" FORCE)
set(CMAKE_OBJDUMP "${TOOLCHAIN_PATH}/${CROSS_COMPILE}objdump" CACHE FILEPATH "objdump" FORCE)
set(CMAKE_SIZE "${TOOLCHAIN_PATH}/${CROSS_COMPILE}size" CACHE FILEPATH "size" FORCE)
set(CMAKE_AR "${TOOLCHAIN_PATH}/${CROSS_COMPILE}ar" CACHE FILEPATH "ar" FORCE)
set(CMAKE_RANLIB "${TOOLCHAIN_PATH}/${CROSS_COMPILE}ranlib" CACHE FILEPATH "ranlib" FORCE)

# Skip compiler tests (we're cross-compiling, can't run test executables)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

# Don't try to link during compiler testing
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)