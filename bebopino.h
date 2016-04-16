#include <string.h>
#include "constants.h"
#include "Arduino.h"
#include "ESP8266.h"
#include "SoftwareSerial.h"

#define SSID "edsfamily"
#define PASSWORD "dinah-moe-humm"
#define MUX_RECV 0
#define MUX_SEND 1
#define MAX_ID 512

typedef unsigned char BYTE;

typedef struct network_frame
{
    uint8_t type;
    uint8_t id;
    uint8_t seq;
    uint32_t frame_size;
    uint32_t data_size;
    BYTE* data;

} network_frame_t;

class SoftwareSerial;

class Bebopino
{
private:
    ESP8266 *wifi;
    uint16_t *seq;
    SoftwareSerial *mySerial;

public:
    Bebopino();
    void Connect();
    int freeRam();
    BYTE *GeneratePCMD(int flag, int roll, int pitch, int yaw, int gaz);
    char *GenerateTakeoffCmd();
    char *GenerateLandingCmd();
    void ReceiveData(uint8_t mux_id);
    BYTE *NetworkFrameGenerator(BYTE *cmd, uint32_t length, uint8_t type, uint8_t id);
    network_frame_t NetworkFrameParser(BYTE *data);
    BYTE *CreateAck(network_frame_t frame);
};
