#pragma once
#include "IStream.hpp"
#include "OStream.hpp"
#include "gpio.hpp"
#include <vector>

class SevenSegment : public IStream, public OStream {
private:
    std::vector<Gpio> segmentPins;
    int currentDigit;  // Store the currently displayed digit

public:
    SevenSegment(std::vector<int> &segmentPins);
    virtual ~SevenSegment();
    
    int readDigit() override;
    void writeDigit(int digit) override;
    
    void clear();  // Turn off all segments
};