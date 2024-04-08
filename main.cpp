#include <ESP8266WiFi.h>
#include "head.h"

void obtainServer();
void initLed();


void setup()
{
	Serial.begin(9600);
	initLed();
}

extern byte Mode = 0;
extern rgb3 Color = 0;
byte curMode = 0;
rgb3 curColor = 0;
uint count = 0;
void loop()
{
	extern const mode_s mode[];
	if (curMode == Mode)
		mode[curMode].obtain();
	else
		mode[curMode = Mode].init();

	if ((count++) % 8 == 0) obtainServer();
	
	delay(10);
}