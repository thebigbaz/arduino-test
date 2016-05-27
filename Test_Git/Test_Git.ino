#include "Arduino.h"

//The setup unction is called once at startup of the sketch

int zReading = A0;

bool redWireState = true;
bool purpleWireState = true;

void triggerAlarm();

void setup()
{
// Add your initialization code here
	Serial.begin(9600);
	pinMode(D8,OUTPUT);
	pinMode(D6, INPUT_PULLUP);
	pinMode(D7, INPUT_PULLUP);
    redWireState = true;
    purpleWireState = true;

}

// The loop function is called in an endless loop
void loop()
{

	int zPoint = analogRead(zReading);
    if (purpleWireState == true && digitalRead(D6) == HIGH){
    	purpleWireState = false;
    }

    if (redWireState == true && digitalRead(D7) == HIGH && purpleWireState == true){
    	redWireState = false;
    }

    if (redWireState == true && purpleWireState == false){
    	triggerAlarm();
    	purpleWireState = true;
    }

    zPoint = analogRead(zReading);
//Add your repeated code here
    if (redWireState == true )
    {
		while( ((zPoint < 340) || (zPoint > 600 ))  )
		{
			Serial.print("X triggered = ");
			Serial.println(zPoint);

			triggerAlarm();

			zPoint = analogRead(zReading);

		}
    }
	Serial.print("X = ");
	Serial.println(zPoint);
	delay(100);
}


void triggerAlarm(){
	for(int i = 0; i < 10;i++)
	{
		digitalWrite(D8,HIGH);
		delay(500);
		digitalWrite(D8,LOW);
		delay(500);
	}
}
