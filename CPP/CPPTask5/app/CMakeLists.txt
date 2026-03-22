# Create executable
add_executable(MainApp main.cpp)

# Link the library
target_link_libraries(MainApp PRIVATE SevenSegmentLib)