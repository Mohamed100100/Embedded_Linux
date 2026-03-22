#include "FileActions.h"
#include <iostream>
#include <string>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <file_path>\n";
    std::cout << "Example: " << programName << " test.txt\n";
}

int main(int argc, char* argv[]) {
    // Check if file path argument is provided
    if (argc != 2) {
        std::cerr << "Error: File path argument required!\n\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string filePath = argv[1];
    std::cout << "Using file: " << filePath << "\n\n";

    // Test 1: Basic write operations
    std::cout << "=== Test 1: Write Operations ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"write", 123},
            {"write", 456},
            {"write", 789}
        });
        fa.executeActions();
    }
    std::cout << "\n";

    // Test 2: Read operations
    std::cout << "=== Test 2: Read Operations ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"read", 50}  // Read up to 50 bytes
        });
        fa.executeActions();
    }
    std::cout << "\n";

    // Test 3: Mixed operations
    std::cout << "=== Test 3: Mixed Operations ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"write", 999},
            {"read", 10},
            {"write", 111}
        });
        fa.executeActions();
    }
    std::cout << "\n";

    // Test 4: Close operation
    std::cout << "=== Test 4: Close Operation ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"write", 555},
            {"close",0},
            {"write", 666}  // This should fail after close
        });
        fa.executeActions();
    }
    std::cout << "\n";

    // Test 5: Invalid action
    std::cout << "=== Test 5: Invalid Action ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"invalid_action", 100}
        });
        fa.executeActions();
    }
    std::cout << "\n";

    std::cout << "All tests completed!\n";
    return 0;
}