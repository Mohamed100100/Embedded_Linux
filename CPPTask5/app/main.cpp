#include "SevenSegment.hpp"
#include "IOStream.hpp"
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

void displayOnStream(Stream* stream, int digit) {
    if (stream != nullptr) {
        stream->writeDigit(digit);
    }
}

int readFromStream(Stream* stream) {
    if (stream != nullptr) {
        return stream->readDigit();
    }
    return -1;
}

int main() {
    // Define GPIO pins for the seven-segment display
    std::vector<int> segmentPins = {2, 3, 4, 17, 27, 22, 10};

    // Create Stream pointers to different display types
    // Using raw pointers
    Stream* sevenSegmentDisplay = new SevenSegment(segmentPins);
    Stream* terminalDisplay = new IOStream();

    std::cout << "\n===== Displaying digits on Seven Segment Display =====" << std::endl;
    for (int digit = 0; digit <= 9; ++digit) {
        displayOnStream(sevenSegmentDisplay, digit);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\n===== Displaying digits on Terminal =====" << std::endl;
    for (int digit = 0; digit <= 9; ++digit) {
        displayOnStream(terminalDisplay, digit);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "\n===== Reading digit from Terminal =====" << std::endl;
    int userDigit = readFromStream(terminalDisplay);
    
    std::cout << "\n===== Displaying user input on both displays =====" << std::endl;
    displayOnStream(terminalDisplay, userDigit);
    displayOnStream(sevenSegmentDisplay, userDigit);

    // Clean up
    delete sevenSegmentDisplay;
    delete terminalDisplay;

    return 0;
}