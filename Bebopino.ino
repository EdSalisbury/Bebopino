#include "bebopino.h"

Bebopino *bebop;
uint32_t prev_time = 0;
const uint32_t update_interval = 25; // 40hz
uint8_t step = 0;
int button_state = 0;
uint32_t start = 0;

void setup()
{
    pinMode(4, INPUT);
    Serial.begin(9600);
    Serial.println(F("Connecting to wifi"));
    bebop = new Bebopino();
    bebop->Discovery();
    bebop->Connect();
}

void loop()
{
    button_state = digitalRead(4);
    if (button_state == HIGH)
    {
        step = 1;
        start = millis();
    }

    uint32_t time = millis();
    if (time - prev_time >= update_interval)
    {
        prev_time = time;
        bebop->GeneratePCMD();
    }

    switch(step)
    {
        case 1:
            if (millis() - start > 5000)
            {
                Serial.println("Taking off");
                bebop->TakeOff();
                step = 9;
            }
            break;
        case 2:
            if (millis() - start > 8000)
            {
                bebop->Clockwise(50);
                step++;
            }
            break;
        case 3:
            if (millis() - start > 9000)
            {
                bebop->CounterClockwise(50);
                step++;
            }
            break;
        case 4:
            if (millis() - start > 10000)
            {
                bebop->Stop();
                step++;
            }
            break;
        case 5:
            if (millis() - start > 11000)
            {
                bebop->Forward(10);
                step++;
            }
            break;
        case 6:
            if (millis() - start > 12000)
            {
                bebop->Up(50);
                step++;
            }
            break;
        case 7:
            if (millis() - start > 13000)
            {
                bebop->Down(50);
                step++;
            }
            break;
        case 8:
            if (millis() - start > 14000)
            {
                bebop->Stop();
                step++;
            }
            break;
        case 9:
            if (millis() - start > 15000)
            {
                Serial.println("Landing");
                bebop->Land();
            }
            break;
    }

    // Check if there's any new data
    bebop->ReceiveData(MUX_RECV);
}
