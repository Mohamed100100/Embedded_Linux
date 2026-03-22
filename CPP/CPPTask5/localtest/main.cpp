#include "SevenSegment.hpp"
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    // Define GPIO pins for the seven-segment display (example pins)
    std::vector<int> segmentPins = {2, 3, 4, 17, 27, 22, 10}; // GPIO pin numbers

    // Create a SevenSegment object
    SevenSegment display(segmentPins);

    // Write digits 0-9 to the seven-segment display
    for (int digit = 0; digit <= 9; ++digit) {
        display.writeDigit(digit);
        
        // Add a delay to observe the output (1 second)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Clear the display
    display.clear();

    // Example of reading the current digit
    std::cout << "Last displayed digit was: " << display.readDigit() << std::endl;

    return 0;
}