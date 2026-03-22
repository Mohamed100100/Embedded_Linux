
#pragma once
#include "Stream.hpp"
class IStream : public virtual Stream {
    public:
        IStream();
        virtual ~IStream();
        virtual int readDigit() override;
};