# AirGradientHomeAssistant

[![CI](https://github.com/agileek/AirGradientHomeAssistant/actions/workflows/CI.yaml/badge.svg)](https://github.com/agileek/AirGradientHomeAssistant/actions/workflows/CI.yaml)

Publish AirGradient to Home Assistant through MQTT


**Based on the work done by https://www.airgradient.com/diy/ and https://github.com/ChaseCares/AirGradient-InfluxDB**


## Getting Started, the terminal way

* Install arduino-cli
* `make init`

Now, copy DeviceConfig.hpp.template into DeviceConfig.hpp and configure the stuff: `cp ./DeviceConfig.hpp.template ./DeviceConfig.hpp`

Then, compile it: `make compile`

Finally, upload it: `make upload` (you can only use make upload, as it automatically compile)


## Getting Started, the visual way

### Step 0 | Prerequisites

- Assemble the AirGradient device - Instructions [here](https://www.airgradient.com/diy/)
- Install Arduino - Download [here](https://www.arduino.cc/en/software)
- Download this repository - Click the green “Code” button and then “Download ZIP”
- Unzip the folder, navigate to the AirQualityMonitor and copy the example config file. Paste it into the same directory and rename it `DeviceConfig`

### Step 1 | Settting up Arduino

**Add ESP8266 platform information.**

- Open the Arduino sketch file. Then open the Preferences menu, and add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to *Additional Board Manager URLs* [Image](./Images/Preferences.png)
- Open the Board Manager and search for and install: `esp8266` by `ESP8266 comunity` [Image](./Images/BoardManager.png)
- Load the board preferences by navigating to Tool -> Board: -> ESP8266 Boards, then select `LOLIN(WEMOS) D1 R2 & mini` [Image](./Images/BM-D1mini.png)

**Add the required libraries.**

- Open library manager, search for and install: `AirGradient Air Quality Sensor` [Image](./Images/LM-AirGradient.png)

If only using sensors, with no connectivity skip to step two

- Search for and install: `AirGradient Air Quality Sensor` by AirGradient [Image](./Images/LM-AirGradient.png)
- Search for and install: `ESP8266 and ESP32 OLED driver for SSD1306 displays` by ThingPulse [Image](./Images/LM-OLED.png)

Only needed if you enable MQTT

- Search for and install: `home-assistant-integration` by Dawid Chyrzynski [Image](./Images/LM-HA.png) You will be asked if you would like to install dependencies, select *Install all* [Image](./Images/LM-Dependencies.png)

### Step 2 | Configuration

Navigate to DeviceConfig.hpp tab and enable any features you would like to use. Then fill out their corresponding credentials.

Once customization is complete, plug the device (esp8266) into your computer then click upload.
