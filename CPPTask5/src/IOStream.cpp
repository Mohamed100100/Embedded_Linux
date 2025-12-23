

#include "IOStream.hpp"
#include <iostream>

IOStream::IOStream() : Stream(), IStream(), OStream() {
    std::cout << "IOStream constructor called" << std::endl;
}
IOStream::~IOStream() {
    std::cout << "IOStream destructor called" << std::endl;
}

