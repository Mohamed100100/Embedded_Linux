# Collect all source files
set(LIB_SOURCES
    Stream.cpp
    IStream.cpp
    OStream.cpp
    SevenSegment.cpp
)

# Create shared library
add_library(SevenSegmentLib SHARED ${LIB_SOURCES})

# Make headers available to library and users
target_include_directories(SevenSegmentLib
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)