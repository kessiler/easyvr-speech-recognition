#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #include "Platform.h"
  #include "SoftwareSerial.h"
#ifndef CDC_ENABLED
  // Shield Jumper on SW
  SoftwareSerial port(12,13);
#else
  // Shield Jumper on HW (for Leonardo)
  #define port Serial1
#endif
#else // Arduino 0022 - use modified NewSoftSerial
  #include "WProgram.h"
  #include "NewSoftSerial.h"
  NewSoftSerial port(12,13);
#endif

#include "EasyVR.h"

EasyVR easyvr(port);
EasyVRBridge bridge;
int8_t lang  = EasyVR::ENGLISH;
int8_t group = 1;
uint8_t train = 0;

char name[15];


void setup()
{
        pinMode(30, OUTPUT);
#ifndef CDC_ENABLED
        // bridge mode?
        if (bridge.check())
        {
          cli();
          bridge.loop(0, 1, 12, 13);
        }
	Serial.begin(9600);
	Serial.println("Bridge not started!");
#else
        // bridge mode?
        if (bridge.check())
        {
          port.begin(9600);
          bridge.loop(port);
        }
	Serial.println("Bridge connection aborted!");
#endif
	port.begin(9600); 

	while (!easyvr.detect())
	{
		Serial.println("EasyVR not detected!");
		delay(1000);
	}
	
	easyvr.setPinOutput(EasyVR::IO1, LOW);
	Serial.print("EasyVR detected, version ");
	Serial.println(easyvr.getID());
	easyvr.setTimeout(5);
	easyvr.setLanguage(lang);
        int8_t count = easyvr.getCommandCount(group);
	Serial.print("Load "); 
        Serial.print(count);
	Serial.print(" commands in group ");
	Serial.println(group);
        for (int8_t idx = 0; idx < count; ++idx)
        {
    	      Serial.print("Commands: ");
              if (easyvr.dumpCommand(group, idx, name, train))
              {
                  Serial.print(idx);
                  Serial.print(" = ");
                  Serial.print(name);
                  Serial.print(", Trained ");
                  Serial.println(train, DEC);
              }
        }
}

void loop()
{
	Serial.print("Say a command in Group ");
	Serial.println(group);
	easyvr.recognizeCommand(group);
	while (!easyvr.hasFinished());
	int16_t idx;
	idx = easyvr.getCommand();
	if (idx >= 0)
	{
    		Serial.print("Command: ");
    		int command = easyvr.getCommand();
    		Serial.print(command);
    		if (easyvr.dumpCommand(group, idx, name, train))
    		{
    		  Serial.print(" = ");
    		  Serial.println(name);
    		}
    
                switch(command) {
                    case 6:
                      digitalWrite(30, HIGH);
                      break;
                    case 7:
                      digitalWrite(30, LOW);
                      break;
                }
	}
}

