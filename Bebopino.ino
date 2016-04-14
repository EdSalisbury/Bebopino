#include "SoftwareSerial.h"
#include "bebopino.h"

void setup()
{
    Serial.begin(9600);
    Bebopino *bebop = new Bebopino();
}

void loop()
{
  // put your main code here, to run repeatedly:

}
