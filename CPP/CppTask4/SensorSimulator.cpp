#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <pin_number>\n";
        return 1;
    }

    int pinNumber = std::atoi(argv[1]);

    const std::string filename = "TemperatureData";

    std::string FullName(filename);
    FullName += "_Pin";
    FullName += std::to_string(pinNumber);
    FullName += ".txt";

    float temperature = 25.0f;

    while (true) {
        temperature += 0.2f;
        if (temperature > 40.0f)
            temperature = 25.0f;

        // OVERWRITE previous value
        std::ofstream file(FullName, std::ios::trunc);
        if (!file) {
            std::cerr << "Failed to open file\n";
            return 1;
        }

        file << temperature;
        file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
