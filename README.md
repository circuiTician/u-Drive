The uD library is an all-in-one robotics development framework designed for students, makers, and hobbyists. It eliminates the "hardware hell"—the endless cycle of wiring microcontrollers, motor drivers, and converters—by providing a robust, pre-integrated "Plug and Code" ecosystem.
📦 Prerequisites & Dependencies To ensure the library functions correctly, you must have the following dependencies installed in your Arduino IDE: LibraryPurpose GitHub Link Adafruit NeoPixel Controls onboard RGB LED feedback Download Here - https://github.com/adafruit/Adafruit_NeoPixel.

Note on Built-in Libraries: The SD.h and SPI.h libraries are included by default with the Arduino core (specifically for ESP32/Arduino platforms). You do not need to install these manually.
🚀 Key Features: Plug and Code Integration: Unified control for motors, IR arrays, and ultrasonic sensors.Sensor Suite Ready: Native support for 3-way Ultrasonic and IR sensor arrays (Left, Middle, Right).Built-in Safety: Real-time battery voltage monitoring and motor driver fault detection.Visual & Audio Feedback: Onboard programmable RGB LEDs and a dedicated buzzer for status alerts.Data Logging: Integrated SD card support for logging sensor telemetry.
📂 Repository Structure/src: Core library files (uD.h and uD.cpp)./examples: Ready-to-run sketches:FunctionalTest: Comprehensive hardware verification.ObstacleAvoidance: Autonomous navigation logic.LineFollower: High-speed line following algorithms./docs: Pinout diagrams and hardware reference sheets.
🛠️ Getting Started1. Installation: Download this repository as a .zip file.In the Arduino IDE, go to Sketch > Include Library > Add .ZIP Library...Select the downloaded file. 2. Basic Setup: Include the library in your sketch and initialize the hardware in the setup() function:C++

#include <uD.h>

void setup() {
  Serial.begin(115200);
  uD.begin(); // Initializes motors, sensors, and status systems
}

Running an Example: Open examples/FunctionalTest to verify your hardware. This sketch will automatically cycle through: 
Motor movement (FWD, BWD, LFT, RIT).
RGB LED color tests. 
Buzzer feedback patterns.
Live sensor status (IR & Ultrasonic).
Battery voltage reports.
🤖 Advanced Algorithms. 

The uD library is optimized for high-frequency control loops. For peak performance in competitive tasks, ensure your main loop() function avoids blocking delay() calls, allowing the robot to make micro-adjustments in real-time.
🛒 Purchase & Support: Interested in getting your own uD board? Visit our store to pick up the latest version:https://circuitician.com/u-drive-esp32-motor-driver/
License: MIT License
