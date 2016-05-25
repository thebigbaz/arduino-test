#include "Arduino.h"
//The setup unction is called once at startup of the sketch

bool bollox(){
	return (true);
}

void setup()
{
// Add your initialization code here
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
	while(bollox())
	{
		delay(20);
	}
}
