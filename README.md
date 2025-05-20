# Smart-Dose
## Overview
Smart-Dose is a microcontroller-based solution designed to assist patients with regular injection schedules. It combines real-time time tracking, injection site rotation, OLED display feedback, EEPROM-based memory, and buzzer alerts to provide a reliable and user-friendly reminder system.

This project is especially useful for individuals who require frequent subcutaneous or intramuscular injections, helping avoid repeated usage of the same injection site and ensuring timely administration.

## Features
- **Injection Site Management**: Rotates through 6 predefined injection sites, avoiding the last two used locations to prevent overuse.
- **Time Tracking**: Logs the exact time of the most recent injection and calculates the next due time (after 8 hours).
- **OLED Display**: Real-time feedback with site and time display using a 128x32 OLED screen.
- **EEPROM Storage**: Persistent memory storage ensures site and time history is not lost on power reset.
- **Wi-Fi Time Sync**: Fetches real-time data from NTP servers for accuracy.
- **Buzzer Alerts**: Emits periodic alerts to notify users when it's time for the next dose.
- **Button Controls**: Simple push-button interface to log time, view previous entries, and calculate next schedule.

## Respository Contents
- ```bash
  Smart-Dose/
  ├── InjectionLocationTracker/
  │   └── InjectionLocationTracker.ino         # Code for site tracking and display
  ├── TimeTrackerWithBuzzer/
  │   └── TimeTrackerWithBuzzer.ino            # Code for time logging, buzzer, and scheduling
  ├── images/                                  # Folder to store demo images or circuit diagrams (optional)
  └── README.md


## Hardware Requirements
- NodeMCU (ESP8266) or compatible microcontroller
- 128x32 OLED display (SSD1306-based)
- 2x Push buttons (for time and site tracking)
- Active Buzzer module
- Breadboard and jumper wires
- Optional: Resistors for pull-up/pull-down configuration

## Software Requirements
- Arduino IDE
- ESP8266 Board Package
- Required libraries:
  ```bash
  Adafruit SSD1306
  Adafruit GFX
  EEPROM
  ESP8266WiFi
  time

Install missing libraries using Arduino Library Manager.

## Getting Started
1. Clone the Repository
   ```bash
   git clone https://github.com/urugondavishnu/Smart-Dose.git
   cd Smart-Dose
2. Configure Wi-Fi (for TimeTrackerWithBuzzer)
   Open `TimeTrackerWithBuzzer.ino` and edit these lines:
   ```bash
   const char* ssid     = "your_SSID";
   const char* password = "your_PASSWORD";
3. Upload to NodeMCU
   - Open the .ino file in Arduino IDE
   - Select board: Tools → Board → NodeMCU 1.0 (ESP-12E Module)
   - Select the correct COM port
   - Click Upload

## Usage
- TimeTrackerWithBuzzer
   - Press the Current Time button once to display current time.
   - Press it twice quickly to log the current time (stored in EEPROM).
   - Press the Previous Time button once to display the last saved time.
   - Press it twice quickly to calculate and display the next dose time (+8 hours).
   - A buzzer rings every 2 hours to remind the user.

- InjectionLocationTracker
   - Displays the last injection site on startup.
   - Press button to select next site; ensures no repetition of last two.
   - Sites: Left Arm, Right Arm, Left Leg, Right Leg, Abdomen, Hip
 
## Methodology
- Location Rotation Logic:
  - Stored in EEPROM as numeric codes (0–5) for each site.
  - Next site is chosen randomly, excluding last two.

- Time Logic:
   - Synchronized from NTP.
   - Stored in HH:MM format.
   - Next dose calculated by adding 8 hours.
   - Buzzer sounds every 2 hours as a general reminder.

## Results
- Successfully rotates between injection sites avoiding overuse.
- Displays all information clearly on OLED.
- Recovers from power failures by reading last site and time from EEPROM.
- Tamper-proof with button-based interactions.
- Wi-Fi-based accurate time handling without RTC.

## To Do / Future Improvements
- Add web interface for logs and settings.
- Integrate battery-powered setup.
- Add mobile notification integration using Blynk or MQTT.

## Contributing
Contributions are welcome! Please fork the repository and create a pull request with your enhancements or bug fixes.

## Contact
For questions or feedback, please reach out to [urugondavishnu](https://github.com/urugondavishnu).
