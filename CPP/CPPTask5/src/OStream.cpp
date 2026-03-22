


#include "OStream.hpp"
#include <iostream>


OStream::OStream() : Stream() {
    std::cout << "OStream constructor called" << std::endl;
}

OStream::~OStream() {
    std::cout << "OStream destructor called" << std::endl;
}

void OStream::writeDigit(int digit) {
    std::cout << digit << std::endl;
}

