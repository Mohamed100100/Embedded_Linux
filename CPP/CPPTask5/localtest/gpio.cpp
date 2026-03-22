

#include <fcntl.h>
#include <iostream>
#include "gpio.hpp"
#include <unistd.h>
#include <cstring>


Gpio::Gpio(int pinNumber, GpioDirection_t direction){
    PinNumber = pinNumber;
    exportPin(pinNumber);
    setDirection(direction);
}

Gpio_Status_t Gpio::exportPin(int pinNumber){
    Gpio_Status_t status = GPIO_NOT_OK;
    int exportFd = open(EXPORT_PATH, O_WRONLY);
    if(exportFd == -1){
        status = GPIO_FAILED_TO_OPEN_EXPORT_FILE;
        std::cerr<<"Failed to open export file"<<std::endl;
    }else{
        std::string pinStr = std::to_string(pinNumber + GPIO_OFFSET);
        ssize_t written = write(exportFd, pinStr.data(), pinStr.length());
        if(written == -1){
            status = GPIO_FAILED_TO_WRITE_EXPORT_FILE;
            std::cerr<<"Failed to export pin "<<pinNumber<<std::endl;
        }else{
            status = GPIO_OK;
            std::cout<<"GPIO Pin "<<pinNumber<<" exported successfully"<<std::endl;
        }
        close(exportFd);
    }
    return status;
}

Gpio_Status_t Gpio::unexportPin(int pinNumber){
    Gpio_Status_t status = GPIO_NOT_OK;
    int unexportFd = open(UNEXPORT_PATH, O_WRONLY);
    if(unexportFd == -1){
        status = GPIO_FAILED_TO_OPEN_UNEXPORT_FILE;
        std::cerr<<"Failed to open unexport file"<<std::endl;
    }else{
        std::string pinStr = std::to_string(pinNumber + GPIO_OFFSET);
        ssize_t written = write(unexportFd, pinStr.data(), pinStr.length());
        if(written == -1){
            status = GPIO_FAILED_TO_WRITE_UNEXPORT_FILE;
            std::cerr<<"Failed to unexport pin "<<pinNumber<<std::endl;
        }else{
            status = GPIO_OK;
            std::cout<<"GPIO Pin "<<pinNumber<<" unexported successfully"<<std::endl;
        }
        close(unexportFd);
    }
    return status;
}

Gpio_Status_t Gpio::setDirection(GpioDirection_t direction){
    Gpio_Status_t status = GPIO_NOT_OK;
    if(direction != INPUT && direction != OUTPUT){
        status = GPIO_WRONG_DIRECTION;
        std::cerr<<"Invalid direction for pin "<<PinNumber<<std::endl;
    }else{
        // ex >> /sys/class/gpio/gpio517/direction
        std::string directionPath = std::string(GPIO_PATH) + "gpio" + std::to_string(PinNumber + GPIO_OFFSET) + DIRECTION_PATH;
        std::cout<<directionPath<<std::endl;
        int directionFd = open(directionPath.c_str(), O_WRONLY);
        if(directionFd == -1){
            status = GPIO_FAILED_TO_OPEN_DIRECTION_FILE;
            std::cerr<<"Failed to open direction file for pin "<<PinNumber<<std::endl;
        }else{
            const char* dirStr = (direction == INPUT) ? DIRECTION_IN : DIRECTION_OUT;
            ssize_t written = write(directionFd, dirStr, strlen(dirStr));
            if(written == -1){
                status = GPIO_FAILED_TO_WRITE_DIRECTION_FILE;
                std::cerr<<"Failed to set direction for pin "<<PinNumber<<std::endl;
            }else{
                status = GPIO_OK;
                std::cout<<"Direction for pin "<<PinNumber<<" set successfully"<<std::endl;
            }
            close(directionFd);
        }
    }
    return status;
}

Gpio_Status_t Gpio::writeValue(GpioValue_t value){
    Gpio_Status_t status = GPIO_NOT_OK;
    if(value != GPIO_LOW && value != GPIO_HIGH){
        status = GPIO_WRONG_VALUE;
        std::cerr<<"Invalid value to write for pin "<<PinNumber<<std::endl;
    }else{

        // ex >> /sys/class/gpio/gpio517/value
        std::string valuePath = std::string(GPIO_PATH) + "gpio" + std::to_string(PinNumber + GPIO_OFFSET) + VALUE_PATH;
        int valueFd = open(valuePath.c_str(), O_WRONLY);
        if(valueFd == -1){
            status = GPIO_FAILED_TO_OPEN_VALUE_FILE;
            std::cerr<<"Failed to open value file for pin "<<PinNumber<<std::endl;
        }else{
            const char* valStr = (value == GPIO_LOW) ? VALUE_LOW : VALUE_HIGH;
            ssize_t written = write(valueFd, valStr, strlen(valStr));
            if(written == -1){
                status = GPIO_FAILED_TO_WRITE_VALUE_FILE;
                std::cerr<<"Failed to set value for pin "<<PinNumber<<std::endl;
            }else{
                status = GPIO_OK;
                std::cout<<"Value for pin "<<PinNumber<<" set successfully"<<std::endl;
            }
            close(valueFd);
        }
    }
    return status;
}

Gpio_Status_t Gpio::readValue(GpioValue_t &valueRef){
    Gpio_Status_t status = GPIO_NOT_OK;
    // ex >> /sys/class/gpio/gpio517/value
    std::string valuePath = std::string(GPIO_PATH) + "gpio" + std::to_string(PinNumber + GPIO_OFFSET) + VALUE_PATH;
    int valueFd = open(valuePath.c_str(), O_RDONLY);
    if(valueFd == -1){
        status = GPIO_FAILED_TO_OPEN_VALUE_FILE;
        std::cerr<<"Failed to open value file for pin "<<PinNumber<<std::endl;
    }else{
        char buffer[2] = {0};
        ssize_t readBytes = read(valueFd, buffer, 1);
        if(readBytes == -1){
            std::cerr<<"Failed to read value for pin "<<PinNumber<<std::endl;
            close(valueFd);
            // default to error case
            status = GPIO_FAILED_TO_READ_VALUE_FILE;
        }else{
            close(valueFd);
            if(buffer[0] == '0'){
                valueRef = GPIO_LOW;
            }else{
                valueRef = GPIO_HIGH;
            }
        }
    }
    return status;
}

Gpio_Status_t Gpio::setValue(GpioValue_t value){
    return writeValue(value);
}

Gpio_Status_t Gpio::getValue(GpioValue_t &valueRef){
    return readValue(valueRef);
}

void Gpio::operator<<(const GpioValue_t &valueRef){
    Gpio_Status_t status = this->writeValue(valueRef);
    if(status != GPIO_OK){
        std::cerr<<"Error during writing value to pin "<<PinNumber<<std::endl;
    }
};

void Gpio::operator>>(GpioValue_t &valueRef){
    Gpio_Status_t status = this->readValue(valueRef);
    if(status != GPIO_OK){
        std::cerr<<"Error during reading value from pin "<<PinNumber<<std::endl;
    }
};

Gpio::~Gpio(){
    Gpio_Status_t status = unexportPin(PinNumber);
    if(status != GPIO_OK){
        std::cerr<<"Error during unexporting pin "<<PinNumber<<std::endl;
    }else{
        std::cout<<"GPIO Pin "<<PinNumber<<" cleaned up successfully"<<std::endl;
    }
}
