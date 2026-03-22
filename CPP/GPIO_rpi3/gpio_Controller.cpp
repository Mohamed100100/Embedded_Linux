#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>



constexpr unsigned int GPIO_PIN_OFFSET=512;

class GPIO {
private:
  int *Valuefd = nullptr;
  int *Directionfd = nullptr;

public:
  GPIO() = delete;

  GPIO(int GpioPinNumber) {
    int ExportFd = open("/sys/class/gpio/export", O_WRONLY);

    if (ExportFd < 0) {
        perror("Export");
        std::cout << "Failed to open (export file) to select pin : "<<GpioPinNumber<<"\n";
        Valuefd = nullptr;
        Directionfd = nullptr;
    }else{
        std::string ActualPinNumber = std::to_string(GpioPinNumber + GPIO_PIN_OFFSET);
        ssize_t written = write(ExportFd,ActualPinNumber.data(),ActualPinNumber.length());

        if (written == -1) {
            perror("Export");
            std::cout << "Failed to select GPIO PIN : "<<GpioPinNumber<<"\n";
        }else{
            std::cout << "Successfully GPIO PIN Ready : "<<GpioPinNumber<<"\n";
        }

        close(ExportFd);
        std::string DirectionFileName = "/sys/class/gpio/gpio" +ActualPinNumber + "/direction"; 
        int localDirectionfd = open(DirectionFileName.data(),O_WRONLY);

        if (localDirectionfd < 0) {
            perror("Direction");
            std::cout << "Failed to open (Direction file) : "<<GpioPinNumber<<"\n";
            Valuefd = nullptr;
            Directionfd = nullptr;
        }else{
            std::string ValueFileName = "/sys/class/gpio/gpio" +ActualPinNumber + "/value"; 
            int localValuefd = open(ValueFileName.data(),O_RDWR);

            if (localValuefd < 0) {
                perror("Value");
                std::cout << "Failed to open (Value file) : "<<GpioPinNumber<<"\n";
                close(localDirectionfd);
                Valuefd = nullptr;
                Directionfd = nullptr;
            }else{
                Directionfd = new int(localDirectionfd);
                Valuefd      = new int(localValuefd);
            }
        }

    }
}
    
        void SetPinVal(int value){

            // str.insert("d");
            std::string valueStr;
            switch(value){
                case 0 : valueStr = "0";break;
                case 1 : valueStr = "1";break;
                default: std::cout << "Wrong Value : "<<value<<"\n";return;
            }

            ssize_t written = write(*Valuefd,valueStr.data(),valueStr.length());
            if (written == -1) {
                perror("Value");
                std::cout << "Failed to Write the value \n";
            }else{
                // the pin set successfully
            }
        }

        void SetPinDirecthion(int direc){

            // str.insert("d");
            std::string direcStr;
            switch(direc){
                case 0 : direcStr = "in";break;
                case 1 : direcStr = "out";break;
                default: std::cout << "Wrong Value : "<<direc<<"\n";return;
            }

            ssize_t written = write(*Directionfd,direcStr.data(),direcStr.length());
            if (written == -1) {
                perror("Direction");
                std::cout << "Failed to set pin direction \n";
            }else{
                // the pin set successfully
            }
        }


        ~GPIO(){
            close(*Directionfd);
            close(*Valuefd);

            delete Directionfd;
            delete Valuefd;

            
        }
  
};


int main (){

    GPIO pin17(17);
    
    pin17.SetPinDirecthion(1);
    pin17.SetPinVal(0);

}