#include "SevenSegment.hpp"
#include <vector>
#include <iostream>

SevenSegment::SevenSegment(std::vector<int> &segmentPins) 
    : Stream(), IStream(), OStream(), currentDigit(-1) {
    if (segmentPins.size() != 7) {
        std::cerr << "Error: SevenSegment requires exactly 7 segment pins." << std::endl;
    } else {
        // CRITICAL FIX: Reserve space BEFORE adding elements
        // This prevents vector reallocation and unwanted destructor calls
        this->segmentPins.reserve(segmentPins.size());
        
        for (const auto& pin : segmentPins) {
            this->segmentPins.emplace_back(pin, GpioDirection_t::OUTPUT);
        }
        std::cout << "SevenSegment constructor called" << std::endl;
    }
}

void SevenSegment::writeDigit(int digit) {
    if (digit < 0 || digit > 9) {
        std::cerr << "Error: Invalid digit for SevenSegment: " << digit << std::endl;
        return;
    }
    
    static const std::vector<std::vector<GpioValue_t>> digitToSegments = {
        {GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_LOW},  // 0
        {GPIO_LOW,  GPIO_HIGH, GPIO_HIGH, GPIO_LOW,  GPIO_LOW,  GPIO_LOW,  GPIO_LOW},  // 1
        {GPIO_HIGH, GPIO_HIGH, GPIO_LOW,  GPIO_HIGH, GPIO_HIGH, GPIO_LOW,  GPIO_HIGH}, // 2
        {GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_LOW,  GPIO_LOW,  GPIO_HIGH}, // 3
        {GPIO_LOW,  GPIO_HIGH, GPIO_HIGH, GPIO_LOW,  GPIO_LOW,  GPIO_HIGH, GPIO_HIGH}, // 4
        {GPIO_HIGH, GPIO_LOW,  GPIO_HIGH, GPIO_HIGH, GPIO_LOW,  GPIO_HIGH, GPIO_HIGH}, // 5
        {GPIO_HIGH, GPIO_LOW,  GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH}, // 6
        {GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_LOW,  GPIO_LOW,  GPIO_LOW,  GPIO_LOW},  // 7
        {GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH}, // 8
        {GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_LOW,  GPIO_HIGH, GPIO_HIGH}  // 9
    };
    
    const auto& segments = digitToSegments[digit];
    for (size_t i = 0; i < segmentPins.size(); ++i) {
        segmentPins[i].setValue(segments[i]);
    }
    
    currentDigit = digit;
    std::cout << "Displayed digit: " << digit << std::endl;
}

int SevenSegment::readDigit() {
    if (currentDigit >= 0 && currentDigit <= 9) {
        return currentDigit;
    }
    return IStream::readDigit();
}

void SevenSegment::clear() {
    for (auto& pin : segmentPins) {
        pin.setValue(GPIO_LOW);
    }
    currentDigit = -1;
    std::cout << "Seven segment display cleared" << std::endl;
}

SevenSegment::~SevenSegment() {
    std::cout << "SevenSegment destructor called" << std::endl;
    if (!segmentPins.empty()) {
        clear();
    }
}