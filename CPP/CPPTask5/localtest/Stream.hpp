#pragma once

class Stream {
    public:
        Stream();
        virtual ~Stream();
        Stream(const Stream& other)            = delete;
        Stream& operator=(const Stream& other) = delete;
        Stream(Stream&& other)                 = default;
        Stream& operator=(Stream&& other)      = default;
        virtual void writeDigit(int digit)     = 0;
        virtual int readDigit()                = 0;
};