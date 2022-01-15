/*
Based on https://www.airgradient.com/diy/ and https://github.com/ChaseCares/AirGradient-InfluxDB

For build instructions please visit https://www.airgradient.com/diy/

Please install ESP8266 board manager (tested with version 3.0.0)

The following libraries installed:
"ESP8266 and ESP32 OLED driver for SSD1306 displays" by ThingPulse, Fabrice Weinberg (tested with Version 4.2.1)
"arduino-home-assistant" by Dawid Chyrzynski (tested with Version 1.3.0)


MIT License

---modifications---

The original example code was modified to add additional features.
	Changes:
	- removed captive portal, Wi-Fi credentials are now defined below
	- added a calibration offset for temperature (as Jeff Gerling noted temperatures are usually a bit high)
		the calibration value is subtracted from the reading
*/

#include <functional>
#include <optional>
#include <sstream>
#include <iterator>
#include <map>

#include <SSD1306Wire.h>
#include <AirGradient.h>
#include <Wire.h>

#include "DeviceConfig.hpp"

// Sanity check
#if ENABLE_MQTT && !ENABLE_WI_FI
	#error Wi-Fi must be enabled for MQTT to work properly
#endif
#if !HAS_SHT && !HAS_PM2_5 && !HAS_CO2
	#error Must have at least one sensor enabled
#endif

#if ENABLE_WI_FI
	#include <ESP8266WiFi.h>
#endif

#if ENABLE_MQTT
	#include <ArduinoHA.h>

HADevice device;

WiFiClient client;

HAMqtt mqtt(client, device);
	#if HAS_PM2_5
HASensor MQTT_PM2_5("PM2_5");
	#endif
	#if HAS_CO2
HASensor MQTT_CO2("CO2");
	#endif
	#if HAS_SHT
HASensor MQTT_temperature("Temperature");
HASensor MQTT_humidity("Humidity");
	#endif
#endif

AirGradient ag = AirGradient();

SSD1306Wire display(0x3c, SDA, SCL);

template <typename... Args>
std::string toStr(Args &&...args);

void setup() {
	Serial.begin(9600);

	display.init();
	display.flipScreenVertically();
	showTextRectangle(DEVICE_NAME, "Starting");

#if (HAS_PM2_5)
	ag.PMS_Init();
#endif
#if (HAS_CO2)
	ag.CO2_Init();
#endif
#if (HAS_SHT)
	ag.TMP_RH_Init(0x44);
#endif

#if ENABLE_WI_FI
	// Setup and wait for Wi-Fi
	WiFi.begin(WI_FI_SSID, WI_FI_PASSWORD);
	Serial.println("");

	showTextRectangle("Setting", "up WiFi");
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(WI_FI_SSID);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.print("MAC address: ");
	Serial.println(WiFi.macAddress());
	Serial.print("Hostname: ");
	Serial.println(String(WiFi.hostname()));
#endif

	Serial.println("");

#if ENABLE_MQTT
	// Unique ID must be set
	byte mac[WL_MAC_ADDR_LENGTH];
	WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));

	device.setName(DEVICE_NAME);

	#if HAS_PM2_5
	MQTT_PM2_5.setUnitOfMeasurement("µg/m3");
	MQTT_PM2_5.setDeviceClass("pm25");
	MQTT_PM2_5.setIcon("mdi:air-filter");
  MQTT_PM2_5.setName(DEVICE_NAME " Particulate");
	#endif

	#if HAS_CO2
	MQTT_CO2.setUnitOfMeasurement("PPM");
	MQTT_CO2.setDeviceClass("carbon_dioxide");
	MQTT_CO2.setIcon("mdi:molecule-co2");
	MQTT_CO2.setName(DEVICE_NAME " Carbon Dioxide");
	#endif

	#if HAS_SHT
	MQTT_temperature.setUnitOfMeasurement("°C");
	MQTT_temperature.setDeviceClass("temperature");
	MQTT_temperature.setIcon("mdi:thermometer");
  MQTT_temperature.setName(DEVICE_NAME " Temperature");

	MQTT_humidity.setUnitOfMeasurement("%");
	MQTT_humidity.setDeviceClass("humidity");
	MQTT_humidity.setIcon("mdi:water-percent");
  MQTT_humidity.setName(DEVICE_NAME " Humidity");
	#endif

	if (MQTT_AUTHENTICATION) {
		mqtt.begin(MQTT_ADDR, MQTT_USERNAME, MQTT_PASSWORD);
	} else {
		mqtt.begin(MQTT_ADDR);
	}
#endif

}

void loop() {
	  #if HAS_PM2_5
        int PM2 = ag.getPM2_Raw();
        showTextRectangle("PM2",String(PM2));
        delay(DISPLAY_INTERVAL * 1000);
    #endif

    #if HAS_CO2
        int CO2 = ag.getCO2_Raw();
        showTextRectangle("CO2",String(CO2));
        delay(DISPLAY_INTERVAL * 1000);
    #endif

    #if HAS_SHT
        TMP_RH result = ag.periodicFetchData();
        float caliTemp = (result.t - TEMP_OFFSET);
        showTextRectangle(String(result.t)+"°C",String(result.rh)+"%");
        delay(DISPLAY_INTERVAL * 1000);
    #endif

#if ENABLE_MQTT
	mqtt.loop();
  Serial.println("sending to mqtt");

  #if HAS_PM2_5
	MQTT_PM2_5.setValue(PM2);
	#endif
	#if HAS_CO2
	MQTT_CO2.setValue(CO2);
	#endif
	#if HAS_SHT
	MQTT_temperature.setValue(caliTemp);
	MQTT_humidity.setValue(result.rh);
	#endif
#endif
}

void showTextRectangle(String line1, String line2) {
	display.clear();
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.setFont(ArialMT_Plain_24);
	display.drawString(32, 16, line1);
	display.drawString(32, 36, line2);
	display.display();
}

void showTextRectangle(const char *line1, const char *line2) {
	showTextRectangle(String(line1), String(line2));
}

void showTextRectangle(std::string line1, std::string line2) {
	showTextRectangle(String(line1.c_str()), String(line2.c_str()));
}

template <typename... Args>
std::string toStr(Args &&...args) {
	std::ostringstream ostr;
	(ostr << std::dec << ... << args);
	return ostr.str();
}
