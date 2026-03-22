
#pragma once

#include "Stream.hpp"

class OStream : virtual public Stream {
    public:
        OStream();
        virtual ~OStream();
        virtual void writeDigit(int digit) override;
};