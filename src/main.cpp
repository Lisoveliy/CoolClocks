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

const char length = 31;
const char numberLength = 5;
ScreenType state = Clock;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
MD_MAX72XX *display = new MD_MAX72XX(MD_MAX72XX::FC16_HW, D7, D5, D8, 4);
unsigned long cycleTime = 0;
InputController *button = new InputController(D1);

Time *GetTimeDiff(time_t timeFrom, time_t timeTo)
{
	time_t deltaTime = abs(timeTo - timeFrom);
	Serial.print("deltatime");
	Serial.print(deltaTime);

	char hours = deltaTime / 3600;
	char minutes = (deltaTime - (hours * 3600)) / 60;
	char seconds = (deltaTime - (hours * 3600)) - (minutes * 60);
	Serial.print((short)hours);
	Serial.print(":");
	Serial.print((short)minutes);
	Serial.print(":");
	Serial.print((short)seconds);
	Serial.println();

	return new Time(hours, minutes, seconds);
}

void setup()
{
	cycleTime = millis();

	display->begin();
	display->control(MD_MAX72XX::INTENSITY, 1);
	display->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

	Serial.begin(9600);

	WiFi.mode(WIFI_STA);
	WiFi.begin(WiFiConfig::ssid, WiFiConfig::password);

	pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print("Connecting to WiFi ...");
		if (!timeClient.isTimeSet())
		{
			display->setChar(length, 'W');
			display->setChar(length - 7, 'i');
			display->setChar(length - 10, 'F');
			display->setChar(length - 17, 'i');
			display->update();
		}
		delay(1000);
	}
	button->UpdateInput();
	unsigned long timenow = millis();
	Serial.println(button->GetState());
	if (button->ButtonIsReleased())
	{
		if (state == 0)
		{
			state = ScreenType::Timer;
		}
		else
		{
			state = ScreenType::Clock;
		}
	}
	if (timenow - cycleTime >= 100)
	{
		digitalWrite(LED_BUILTIN, !button->GetState());
		if (button->GetState())
			if (!timeClient.isTimeSet())
			{
				display->setChar(length, 'L');
				display->setChar(length - 5, 'O');
				display->setChar(length - 10, 'A');
				display->setChar(length - 15, 'D');
				display->update();
				return;
			}
		timeClient.update();
		display->clear();
		switch (state)
		{
		case ScreenType::Clock:
			DisplayController::setTime(new Time(
										   timeClient.getHours() + 3,
										   timeClient.getMinutes(),
										   timeClient.getSeconds()),
									   display, length, numberLength);
			break;
		case ScreenType::Timer:
			DisplayController::setTime(GetTimeDiff(timeClient.getEpochTime(), 1717312500), display, length, numberLength);
			break;
		}

		display->update();
		cycleTime = millis();
	}
}