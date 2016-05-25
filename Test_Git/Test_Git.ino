#include "Arduino.h"
//The setup unction is called once at startup of the sketch

bool not_bollox(){
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
	while(not_bollox())
	{
		delay(20);
	}
}
