
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <iomanip>
#include <ctime>
#include <string>

constexpr const char* filename = "TemperatureData";
constexpr float TEMPERATURE_SENSOR_ERROR = -1.0f;


class LMSensor {
    
    private :
        int pin;
        int TempFd = -1;
    public :
        LMSensor() = delete;                                             // Delete default constructor
        LMSensor(LMSensor &other) = delete;                              // Delete copy constructor
        LMSensor& operator=(const LMSensor &other) = delete;             // Delete copy assignment operator
        LMSensor(LMSensor &&other) = default;                            // Default move constructor
        LMSensor& operator=(LMSensor &&other) = default;                 // Default move assignment operator
        ~LMSensor() = default;                                           // Default destructor 
        
        // Parameterized Constructor
        LMSensor(int pinNumber){
            // Initialize the sensor on the specified pin
            pin = pinNumber;
            std::string FullName(filename);
            FullName += "_Pin";
            FullName += std::to_string(pinNumber);
            FullName += ".txt";

            // Open the file representing the sensor
            TempFd = open(FullName.c_str(), O_RDONLY);
            if(TempFd == -1){
                std::cerr << "Error opening file: " << FullName << std::endl;
            }else{
                std::cout << "File opened successfully: " << FullName << std::endl;
                std::cout << "The Program is listining to Temperature changes on Pin " << pinNumber << std::endl;
            }
        }
        // Function to read temperature from the sensor
        float readTemperature(){
            if(TempFd == -1){
                std::cerr << "File not opened properly." << std::endl;
                return TEMPERATURE_SENSOR_ERROR; // Indicate error
            }else{
                char buffer[16];
                // Reset file offset to the beginning
                lseek(TempFd, 0, SEEK_SET); 

                // Read temperature data from the file
                ssize_t bytesRead = read(TempFd, buffer, sizeof(buffer) - 1);
                if(bytesRead <= 0){
                    std::cerr << "Error reading temperature data." << std::endl;
                    return TEMPERATURE_SENSOR_ERROR; // Indicate error
                }else{
                    buffer[bytesRead] = '\0'; // Null-terminate the string
                    float temperature = std::stof(buffer);
                    return temperature;
                }
            }
        }
};

// Class to hold temperature data
class TemperatureData{
    public:
        float temperature;
};


class MainApp {
    private:
        std::weak_ptr<TemperatureData> PtrToTempData;              // Weak pointer to TemperatureData
        std::string LogFilePath;                                   // Path to the log file
    public:
        MainApp() = delete;                                        // Delete default constructor
        MainApp(const MainApp &other) = delete;                    // Delete copy constructor
        MainApp& operator=(const MainApp &other) = delete;         // Delete copy assignment operator
        MainApp(MainApp &&other) = default;                        // Default move constructor
        MainApp& operator=(MainApp &&other) = default;             // Default move assignment operator
        ~MainApp() = default;                                      // Default destructor

        // Parameterized Constructor
        MainApp(std::string LogFilePath,std::shared_ptr<TemperatureData> &tempDataPtr){
            this->LogFilePath = LogFilePath;
            PtrToTempData = tempDataPtr;
            std::cout << "MainApp initialized with log file path: " << LogFilePath << std::endl;
        }

        // Function to log temperature data to a file
        void Log(){
            if(auto tempDataSharedPtr = PtrToTempData.lock()){
                int fd = open(LogFilePath.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0644);
                if(fd == -1){
                    std::cerr << "Error opening log file: " << LogFilePath << std::endl;
                    return;
                }else{
                    /*  This part for get the Time Stamp */
                    std::time_t now = std::time(nullptr);
                    std::tm* localTime = std::localtime(&now);

                    std::ostringstream timeStampStream;
                    timeStampStream << "Timestamp: " << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << " ";
                    std::string timeStamp = timeStampStream.str();

                    std::string logEntry = timeStamp + "Temperature: " + std::to_string(tempDataSharedPtr->temperature) + "\n";
                    /*********************************************************************/
                    ssize_t bytesWritten = write(fd, logEntry.c_str(), logEntry.size());
                    if(bytesWritten == -1){
                        std::cerr << "Error writing to log file." << std::endl;
                    }else{
                        std::cout << "Logged data: " << logEntry;
                    }
                    close(fd);
                }
            }else{
                std::cerr << "TemperatureData pointer is no longer valid." << std::endl;
            }
        }

        // Function to display temperature data
        void Display(){
            if(auto tempDataSharedPtr = PtrToTempData.lock()){
                std::cout << "Current Temperature: " << tempDataSharedPtr->temperature << "°C" << std::endl;
            }else{
                std::cerr << "TemperatureData pointer is no longer valid." << std::endl;
            }
        }
};

int main(int argc, char* argv[]){
    // Check for correct number of command line arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <pin_number>\n";
        return 1;
    }

    // Extract pin number from command line arguments
    int pinNumber = std::atoi(argv[1]);

    // Create LMSensor and TemperatureData instances using smart pointers
    std::unique_ptr<LMSensor> sensorPtr = std::make_unique<LMSensor>(pinNumber);

    // Shared pointer for TemperatureData
    std::shared_ptr<TemperatureData> tempDataPtr = std::make_shared<TemperatureData>();

    // Create MainApp instance with log file path and TemperatureData pointer as weak pointer
    MainApp app("TemperatureLog.txt", tempDataPtr);

    while(true){
        // Read temperature from the sensor
        float currentTemp = sensorPtr->readTemperature();
        if(currentTemp != TEMPERATURE_SENSOR_ERROR){
            tempDataPtr->temperature = currentTemp;
            app.Display();
            app.Log();
        }else{
            std::cerr << "Failed to read temperature." << std::endl;
        }

        // Read temperature every 5 seconds
        sleep(5); 
    }
}