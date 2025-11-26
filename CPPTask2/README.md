# FileActions Class

A C++ class for managing file operations with a queue-based action system. This class allows you to register multiple file operations (read, write, close) and execute them sequentially.

## Features

- **Action-based file operations**: Queue multiple file operations and execute them in order
- **Supported operations**:
  - `write`: Write integer values to a file
  - `read`: Read specified number of bytes from a file
  - `close`: Close the file descriptor
- **Error handling**: Comprehensive error checking for file operations
- **RAII-style resource management**: Automatic cleanup in destructor



## Compilation

```bash
g++ -o fileactions FileActions.cpp
```


## Usage

### Basic Command
```bash
./fileactions <file_path>
```

### Example
```bash
./fileactions test.txt
```

## API Documentation

### Constructor
```cpp
FileActions(std::string copyPath)
```
Opens a file at the specified path with read/write permissions.

**Parameters:**
- `copyPath`: Path to the file to be opened


### Methods

#### `registerActions()`
```cpp
void registerActions(std::initializer_list<std::pair<std::string,int>> pairObjects)
```
Register a list of actions to be executed.

**Parameters:**
- `pairObjects`: List of action pairs where:
  - `first`: Action name ("write", "read", "close")
  - `second`: Action parameter (value to write, bytes to read, or 0 for close)

**Example:**
```cpp
fa.registerActions({
    {"write", 123},
    {"read", 10},
    {"close", 0}
});
```

#### `executeActions()`
```cpp
void executeActions()
```
Execute all registered actions in the order they were registered.

## Code Example

```cpp
#include "FileActions.h"

int main() {
    // Create FileActions object
    FileActions fa("myfile.txt");
    
    // Register multiple actions
    fa.registerActions({
        {"write", 100},    // Write "100" to file
        {"write", 200},    // Write "200" to file
        {"read", 6},       // Read 6 bytes
        {"close", 0}       // Close the file
    });
    
    // Execute all registered actions
    fa.executeActions();
    
    return 0;
}
```

## Test Suite

The provided main.cpp includes 5 comprehensive tests:

### Test 1: Write Operations
Writes three integer values sequentially to the file.

### Test 2: Read Operations
Reads up to 50 bytes from the file.

### Test 3: Mixed Operations
Tests interleaved write and read operations.

### Test 4: Close Operation
Tests closing the file and attempting operations after close.

### Test 5: Invalid Action
Tests error handling for unrecognized action names.

## Expected Output

```
Using file: test.txt

=== Test 1: Write Operations ===
Successfully write : 123
Successfully write : 456
Successfully write : 789
Destructor called 

=== Test 2: Read Operations ===
Successfully Read
123456789
Destructor called 

=== Test 3: Mixed Operations ===
Successfully write : 999
Successfully Read
123456789999
Successfully write : 111
Destructor called 

=== Test 4: Close Operation ===
Successfully write : 555
Successfully Close
Error while writing : invalid file discriptor 
Destructor called 

=== Test 5: Invalid Action ===
Wrong action Name : invalid_action
Destructor called 

All tests completed!
```

## Implementation Details

### File Descriptor Management
- Uses dynamic memory allocation for file descriptor storage
- Maintains a reference to the file descriptor pointer
- Automatic cleanup in destructor

### Action Queue
- Actions stored as `std::vector<std::pair<std::string, int>>`
- Sequential execution in registration order

### Error Handling
- Null pointer checks before operations
- Invalid file descriptor validation
- Operation-specific error messages


