📟 STM32 SPI Based Sensor Data Acquisition System with LCD Display
🔥 Project Overview

This project demonstrates a real-time embedded data acquisition system using the STM32 microcontroller. It reads analog sensor values using an SPI-based ADC (MCP3008), processes the data, and displays the output on a 16x2 LCD display in real time.

The system is designed and tested in Proteus simulation environment.

🧠 Working Principle
A potentiometer is used as an analog sensor input
MCP3008 converts analog signal to digital using SPI communication
STM32 reads digital data via SPI protocol
Processed data is displayed on a 16x2 LCD screen
🔄 System Architecture
Potentiometer (Sensor)
        ↓
   MCP3008 (SPI ADC)
        ↓ SPI Communication
     STM32F103
        ↓ Processing
   16x2 LCD Display
📦 Components Used
STM32F103C8T6
MCP3008 ADC
16x2 LCD Display
Potentiometer (10K)
⚙️ Features
SPI communication between STM32 and ADC
Real-time analog to digital conversion
LCD display output (8-bit mode)
Embedded bare-metal programming
Proteus simulation support
📊 Output

The LCD displays real-time sensor values:

ADC: 512
V: 2.51V

Values change dynamically based on potentiometer position.

🔌 Hardware Connections
MCP3008 → STM32 (SPI)
MCP3008	STM32
CLK	PA5
MOSI	PA7
MISO	PA6
CS	PA4
LCD (8-bit mode)
LCD Pin	STM32 Pin
RS	PB0
E	PB1
D0–D7	PB7–PB14
🧪 Simulation Tool
Proteus Design Suite
🚀 Applications
Embedded sensor monitoring systems
Industrial data acquisition
IoT sensor preprocessing systems
Educational SPI protocol demonstration
🎯 Learning Outcomes
SPI protocol implementation in STM32
ADC interfacing using MCP3008
LCD interfacing in 8-bit mode
Embedded system integration
Register-level programming
💡 Future Improvements
UART debugging interface
Multi-channel ADC support
Data filtering (moving average)
EEPROM data logging
IoT cloud integration
👨‍💻 Author

ECE / Embedded Systems Student
Focused on STM32 & Embedded Firmware Development
