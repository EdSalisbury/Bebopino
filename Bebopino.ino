#include "SoftwareSerial.h"
#include "bebopino.h"

Bebopino *bebop;

void setup()
{
    Serial.begin(9600);
    bebop = new Bebopino();
    //bebop->Connect();
}

void loop()
{
    //Serial.println("Free RAM: " + String(bebop->freeRam()));
    //bebop->ReceiveData(0);
    //if (data.length() > 0)
    //{
    //    Serial.println("Received: [" + data + "]");
    //}
}
