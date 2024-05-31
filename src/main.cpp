#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <config/wifiConfig.cpp>
#include <displayController.cpp>
#include <math.h>
#include <headers/clockState.h>
#include <inputController.cpp>
#include <EEPROM.h>

// State
const char timeZone = 3;
const char length = 31;
const char numberLength = 5;
ScreenType state = Clock;
unsigned long cycleTime = 0;

// Software
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.ru.pool.ntp.org", timeZone * 3600);

// Hardware
MD_MAX72XX *display = new MD_MAX72XX(MD_MAX72XX::FC16_HW, D7, D5, D8, 4);
InputController *button = new InputController(D1);

Time *GetTimeDiff(time_t timeFrom, time_t timeTo)
{
	time_t deltaTime = abs(timeTo - timeFrom);

	char hours = deltaTime / 3600;
	char minutes = (deltaTime - (hours * 3600)) / 60;
	char seconds = (deltaTime - (hours * 3600)) - (minutes * 60);

	return new Time(hours, minutes, seconds);
}

void setup()
{
	EEPROM.begin(1);
	state = (ScreenType)EEPROM.read(0);
	EEPROM.end();
	
	cycleTime = millis();
	display->begin();
	display->control(MD_MAX72XX::INTENSITY, 1);
	display->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

	Serial.begin(9600);

	WiFi.mode(WIFI_STA);
	WiFi.begin(WiFiConfig::ssid, WiFiConfig::password);
}

void loop()
{
	unsigned long timenow = millis();

	while (!WiFi.isConnected())
	{
		Serial.println("Connecting to WiFi ...");
		if (!timeClient.isTimeSet())
		{
			display->clear();
			DisplayController::print("WIFI", 4, length, display);
			display->update();
		}
		delay(1000);
	}

	button->UpdateInput();
	if (button->ButtonIsReleased())
	{
		if (state == 0)
		{
			EEPROM.begin(1);
			EEPROM.write(0, ScreenType::Timer);
			state = ScreenType::Timer;
			EEPROM.commit();
			EEPROM.end();
		}
		else
		{
			EEPROM.begin(1);
			EEPROM.write(0, ScreenType::Clock);
			state = ScreenType::Clock;
			EEPROM.commit();
			EEPROM.end();
		}

		Serial.print("Mode changed to ");
		Serial.println(state);
	}
	if (timenow - cycleTime >= 50)
	{
		timeClient.update();
		display->clear();

		switch (state)
		{
		case ScreenType::Clock:
			DisplayController::setTime(new Time(
										   timeClient.getHours(),
										   timeClient.getMinutes(),
										   timeClient.getSeconds()),
									   display, length, numberLength);
			break;
		case ScreenType::Timer:
			DisplayController::setTime(GetTimeDiff(timeClient.getEpochTime() - timeZone * 3600, 1717312500), display, length, numberLength);
			break;
		default:
			DisplayController::print("PRESS", 5, length, display);
		}

		display->update();
		cycleTime = millis();
	}
}