#include "bebopino.h"

Bebopino *bebop;
uint32_t prev_time = 0;
const uint32_t interval = 25; // 40hz

void setup()
{
    Serial.begin(9600);
    bebop = new Bebopino();
    bebop->Connect();
}

void loop()
{
    uint32_t time = millis();
    if (time - prev_time >= interval)
    {
        prev_time = time;
        bebop->WritePacket(bebop->NetworkFrameGenerator(bebop->GeneratePCMD()));
    }

    // Check if there's any new data
    bebop->ReceiveData(MUX_RECV);
}
