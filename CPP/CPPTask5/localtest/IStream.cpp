
#include "IStream.hpp"
#include <iostream>


IStream::IStream() : Stream() {
    std::cout << "IStream constructor called" << std::endl;
}

IStream::~IStream() {
    std::cout << "IStream destructor called" << std::endl;
}

int IStream::readDigit() {
    int digit;
    std::cout << "Enter a digit: ";
    std::cin >> digit;
    return digit;
}

