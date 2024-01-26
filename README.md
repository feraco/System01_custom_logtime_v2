# Environmental and Magnetic Field Data Logger

## Overview
This repository hosts the Arduino code for an environmental and magnetic field data logger using the Adafruit BME280 and HMC5883_U sensors. The project is designed for precise and regular logging of environmental parameters alongside magnetic field strength, making it ideal for a variety of scientific and educational applications.

## Key Features

- **Environmental Data Logging**: Utilizes the BME280 sensor to record temperature, humidity, and barometric pressure.
- **Magnetic Field Measurement**: Employs the HMC5883_U sensor to measure magnetic field strength in three dimensions.
- **Timed Data Capture**: Automatically logs data at regular intervals (default set to 1 minute).
- **SD Card Storage**: Saves all data logs in a structured format on an SD card for easy access and analysis.
- **Real-Time Clock Synchronization**: Synchronizes with an external time source (like a Raspberry Pi) to ensure accurate time stamps for each data entry.

## Hardware Requirements

- Arduino Uno
- Adafruit BME280 sensor
- Adafruit HMC5883_U sensor
- SD Card Module
- SD Card

## Software Requirements

- Arduino IDE
- Adafruit_BME280 Library
- Adafruit_HMC5883_U Library
- Other dependencies: Wire, SPI, SD, TimeLib

## Setup

1. **Assemble the Circuit**: Connect the BME280 and HMC5883_U sensors to the Arduino Uno following standard I2C communication protocols. Attach the SD card module.
2. **Install Libraries**: Use the Arduino IDE to install the `Adafruit_BME280` and `Adafruit_HMC5883_U` libraries along with the necessary dependencies.
3. **Upload the Code**: Open this script in the Arduino IDE and upload it to your Arduino Uno.
4. **Data Logging**: Once uploaded, the system begins logging environmental and magnetic field data to the SD card at the set intervals.

## Usage

After the setup, the data logger will start recording data at the specified intervals. Each entry includes a timestamp, temperature, humidity, barometric pressure, and magnetic field strength. Retrieve the SD card to access and analyze the logged data.

## Contributing

Your contributions to enhance or extend the capabilities of this data logger are welcome. Please read the contributing guide for more information.
