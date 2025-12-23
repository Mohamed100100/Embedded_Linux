

#pragma once

#include "IStream.hpp"
#include "OStream.hpp"

class IOStream : public IStream, public OStream {
    public:
        IOStream();
        virtual ~IOStream();
        virtual int readDigit();
        virtual void writeDigit(int digit);
};