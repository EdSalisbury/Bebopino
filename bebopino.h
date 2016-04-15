#include <string.h>
#include "constants.h"
#include "Arduino.h"
#include "ESP8266.h"
#include "SoftwareSerial.h"

#define SSID "edsfamily"
#define PASSWORD "dinah-moe-humm"
#define MUX_RECV 0
#define MUX_SEND 1
#define MAX_SEQ 20

class SoftwareSerial;

class Bebopino
{
private:
    ESP8266 *wifi;
    uint8_t *seq;
    SoftwareSerial *mySerial;

public:
    Bebopino();
    void Connect();
    int freeRam();
    unsigned char *GeneratePCMD(int flag, int roll, int pitch, int yaw, int gaz);
    char *GenerateTakeoffCmd();
    char *GenerateLandingCmd();
    void ReceiveData(uint8_t mux_id);
    unsigned char *NetworkFrameGenerator(unsigned char *cmd, uint32_t length);
};
