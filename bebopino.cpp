#include "bebopino.h"

Bebopino::Bebopino()
{
    Serial.begin(9600);
    Serial.println("Connecting to wifi");
    wifi = new Esp8266(2, 3);
    bool ret = wifi->Connect("edsfamily", "dinah-moe-humm");
    if (ret)
    {
        Serial.println("Connected");
    }
    else
    {
        Serial.println("Unable to connect!");
    }
}

// void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
// {
//        Serial.print("0x");
//        for (int i=0; i<length; i++) {
//          if (data[i]<0x10) {Serial.print("0");}
//          Serial.print(data[i],HEX);
//          Serial.print(" ");
//        }
// }

// void PrintHex16(uint16_t *data, uint8_t length) // prints 16-bit data in hex with leading zeroes
// {
//        Serial.print("0x");
//        for (int i=0; i<length; i++)
//        {
//          uint8_t MSB=byte(data[i]>>8);
//          uint8_t LSB=byte(data[i]);

//          if (MSB<0x10) {Serial.print("0");} Serial.print(MSB,HEX); Serial.print(" ");
//          if (LSB<0x10) {Serial.print("0");} Serial.print(LSB,HEX); Serial.print(" ");
//        }
// }

// void Bebopino::takeoff()
// {
//   char buffer[4];
//   memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
//   memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
//   memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_TAKEOFF, 2);
//   Serial.println("Taking off");
//   char tmp[16];
//   sprintf(tmp, "0x%.4X %.4X %.4X %.4X", buffer[0], buffer[1], buffer[2], buffer[3]);

//   Serial.println(tmp);
// }

// void Bebopino::land()
// {
//   char buffer[4];
//   memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
//   memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
//   memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_LANDING, 2);
//   Serial.println("Landing");
//   char tmp[16];
//   sprintf(tmp, "0x%.4X %.4X %.4X %.4X", buffer[0], buffer[1], buffer[2], buffer[3]);

//   Serial.println(tmp);
// }





