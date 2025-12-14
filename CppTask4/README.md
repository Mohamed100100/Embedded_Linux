# LM Temperature Sensor Monitoring System

## Overview

A robust C++ temperature monitoring application that demonstrates real-time sensor data acquisition, logging, and display capabilities. The project consists of two main components: a sensor simulator and a monitoring application that reads, displays, and logs temperature data using modern C++ practices.

## Features

- **Real-time Temperature Monitoring**: Continuous reading of temperature data from simulated LM sensor
- **Data Logging**: Automatic timestamped logging of temperature readings to file
- **Smart Pointer Management**: Uses `std::unique_ptr`, `std::shared_ptr`, and `std::weak_ptr` for safe memory management
- **Flexible Pin Configuration**: Pin number specified via command-line argument for any sensor pin
- **File-based Sensor Simulation**: Sensor data is simulated through file I/O operations
- **POSIX File Operations**: Low-level file handling using POSIX system calls (`open`, `read`, `write`, `lseek`)
- **Modern C++ Practices**: Rule of 5 implementation with move semantics and deleted copy operations

## Architecture

### Components

#### 1. **LMSensor Class**
- Represents a temperature sensor connected to a specific pin
- Reads temperature data from a file (`TemperatureData_Pin<X>.txt`)
- Implements Rule of 5 with deleted copy operations and defaulted move operations
- Returns error code (`-1.0f`) on read failures

#### 2. **TemperatureData Class**
- Simple data holder for temperature readings
- Shared between components using smart pointers

#### 3. **MainApp Class**
- Manages temperature data display and logging
- Uses `std::weak_ptr` to avoid circular dependencies
- Logs temperature with timestamps to `TemperatureLog.txt`
- Displays current temperature to console

#### 4. **SensorSimulator**
- Standalone program that simulates sensor behavior
- Generates temperature data (25°C to 40°C) and writes to file
- Updates every second, simulating real sensor readings

## Project Structure

```
CppTask4/
├── LMSensor.cpp          # Main monitoring application
├── SensorSimulator.cpp   # Sensor data simulator
├── README.md             # This file
├── TemperatureLog.txt    # Generated log file (after running)
└── TemperatureData_Pin<N>.txt  # Simulated sensor data file (after running, N = pin number)
```

## Requirements

- **Compiler**: C++14 or higher (for `std::make_unique`)
- **Operating System**: Linux (uses POSIX system calls)
- **Libraries**: Standard C++ Library, POSIX API

## Building the Project

### Compile the Sensor Simulator
```bash
g++ -o sensor_sim SensorSimulator.cpp
```

### Compile the Main Application
```bash
g++ -o lm_monitor LMSensor.cpp
```

## Usage

### Step 1: Start the Sensor Simulator
In one terminal, start the simulator with a pin number:
```bash
./sensor_sim 10
```

This will create `TemperatureData_Pin10.txt` and continuously update it with simulated temperature readings (25°C - 40°C).

### Step 2: Run the Monitoring Application
In another terminal, start the monitoring application with the same pin number:
```bash
./lm_monitor 10
```

**Note**: The pin number argument must match the pin number used in the sensor simulator.

The application will:
- Open the sensor data file for the specified pin
- Read temperature every 5 seconds
- Display current temperature on console
- Log temperature with timestamp to `TemperatureLog.txt`

### Sample Output

**Console Output:**
```
File opened successfully: TemperatureData_Pin10.txt
The Program is listining to Temperature changes on Pin 10
MainApp initialized with log file path: TemperatureLog.txt
Current Temperature: 25.2°C
Logged data: Timestamp: 2025-12-14 10:30:15 Temperature: 25.200000
Current Temperature: 25.4°C
Logged data: Timestamp: 2025-12-14 10:30:20 Temperature: 25.400000
...
```

**Log File (TemperatureLog.txt):**
```
Timestamp: 2025-12-14 10:30:15 Temperature: 25.200000
Timestamp: 2025-12-14 10:30:20 Temperature: 25.400000
Timestamp: 2025-12-14 10:30:25 Temperature: 25.600000
...
```

## Configuration

### Sensor Pin Configuration
The pin number is now specified as a command-line argument when running the application:
```bash
./lm_monitor <pin_number>
```

Example for pin 5:
```bash
./sensor_sim 5    # Terminal 1
./lm_monitor 5    # Terminal 2
```

### Sensor Reading Interval
Modify the sleep duration in the main loop:
```cpp
sleep(5);  // Change to desired interval in seconds
```

### Log File Path
Change the log file path when creating the `MainApp` instance:
```cpp
MainApp app("TemperatureLog.txt", tempDataPtr);  // Modify path here
```

## C++ Concepts Demonstrated

### 1. **Smart Pointers**
- `std::unique_ptr<LMSensor>`: Exclusive ownership of sensor object
- `std::shared_ptr<TemperatureData>`: Shared ownership of temperature data
- `std::weak_ptr<TemperatureData>`: Non-owning reference in MainApp to prevent circular dependencies

### 2. **Rule of 5**
Both `LMSensor` and `MainApp` implement the Rule of 5:
- Deleted copy constructor
- Deleted copy assignment operator
- Defaulted move constructor
- Defaulted move assignment operator
- Defaulted destructor

### 3. **POSIX File Operations**
- `open()`: Open files with specific flags
- `read()`: Read data from file descriptor
- `write()`: Write data to file descriptor
- `lseek()`: Reposition file offset
- `close()`: Close file descriptor

### 4. **Modern C++ Features**
- `constexpr`: Compile-time constants
- `delete`: Explicitly deleted functions
- `default`: Explicitly defaulted functions
- String manipulation with `std::to_string()`
- Time handling with `<chrono>` and `<ctime>`

## Error Handling

The application handles various error conditions:
- File opening failures (sensor data file and log file)
- File reading failures
- Invalid sensor data
- Expired weak pointers

Error code: `TEMPERATURE_SENSOR_ERROR = -1.0f` indicates sensor read failure.

## Stopping the Application

Press `Ctrl+C` in both terminals to stop:
1. The monitoring application
2. The sensor simulator

## Command-Line Arguments

### LMSensor Monitor
```bash
./lm_monitor <pin_number>
```
- `<pin_number>`: The GPIO pin number to monitor (required)

### Sensor Simulator
```bash
./sensor_sim <pin_number>
```
- `<pin_number>`: The GPIO pin number to simulate (required)

## Notes

- The sensor simulator must be running before starting the monitoring application
- **The pin number must match between simulator and monitor** - both must use the same pin number
- Multiple sensors can be monitored simultaneously by running multiple instances with different pin numbers
- Log file grows continuously; implement log rotation for production use
- File-based simulation is suitable for development; real hardware would use GPIO/I2C interfaces
