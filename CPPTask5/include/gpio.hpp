#pragma once

constexpr const char* GPIO_PATH      = "/sys/class/gpio/";
constexpr const char* EXPORT_PATH    = "/sys/class/gpio/export";
constexpr const char* UNEXPORT_PATH  = "/sys/class/gpio/unexport";
constexpr const char* VALUE_PATH     = "/value";
constexpr const char* DIRECTION_PATH = "/direction";
constexpr const char* DIRECTION_IN   = "in";
constexpr const char* DIRECTION_OUT  = "out";
constexpr const char* VALUE_HIGH     = "1";
constexpr const char* VALUE_LOW      = "0";
constexpr const int   GPIO_OFFSET    = 512;

typedef enum {
    GPIO_OK = 0,
    GPIO_NOT_OK,
    GPIO_FAILED_TO_OPEN_EXPORT_FILE,
    GPIO_FAILED_TO_WRITE_EXPORT_FILE,
    GPIO_FAILED_TO_OPEN_UNEXPORT_FILE,
    GPIO_FAILED_TO_WRITE_UNEXPORT_FILE,
    GPIO_FAILED_TO_OPEN_DIRECTION_FILE,
    GPIO_FAILED_TO_WRITE_DIRECTION_FILE,
    GPIO_FAILED_TO_OPEN_VALUE_FILE,
    GPIO_FAILED_TO_WRITE_VALUE_FILE,
    GPIO_FAILED_TO_READ_VALUE_FILE,
    GPIO_WRONG_VALUE,
    GPIO_WRONG_DIRECTION
}Gpio_Status_t;


typedef enum {
    INPUT,
    OUTPUT
} GpioDirection_t;

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH = 1,
    GPIO_ERROR = -1
} GpioValue_t;

class Gpio{
    private: 
        int PinFd;
        int PinNumber;
        Gpio_Status_t exportPin(int pinNumber);
        Gpio_Status_t unexportPin(int pinNumber);
        Gpio_Status_t setDirection(GpioDirection_t direction);
        Gpio_Status_t writeValue(GpioValue_t value);
        Gpio_Status_t readValue(GpioValue_t &valueRef);

    public:
        Gpio(int pinNumber, GpioDirection_t direction);
        Gpio()                        = delete;
        Gpio(Gpio& other)             = delete; 
        Gpio& operator=(Gpio& other)  = delete;
        Gpio& operator=(Gpio&& other) = default;
        Gpio( Gpio&& other)           = default;
        ~Gpio();

        Gpio_Status_t setValue(GpioValue_t value);
        Gpio_Status_t getValue(GpioValue_t &valueRef);

        void operator<<(const GpioValue_t &valueRef);

        void operator>>(GpioValue_t &valueRef);

};

