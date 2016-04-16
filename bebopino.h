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
    uint32_t size;
    BYTE* data;

} network_frame_t;

typedef struct
{
    bool landed;
    bool taking_off;
    bool hovering;
    bool flying;
    bool landing;
    bool emergency;
} flying_state_t;

class SoftwareSerial;

class Bebopino
{
private:
    ESP8266 *wifi;
    uint16_t *seq;
    SoftwareSerial *mySerial;
    uint8_t battery;
    flying_state_t flying_state;
    uint64_t flying_time;

public:
    Bebopino();
    void Connect();
    int freeRam();
    BYTE *GeneratePCMD(int flag, int roll, int pitch, int yaw, int gaz);
    BYTE *GenerateTakeoffCmd();
    BYTE *GenerateLandingCmd();
    void ReceiveData(uint8_t mux_id);
    BYTE *NetworkFrameGenerator(BYTE *cmd, uint8_t type = ARNETWORKAL_FRAME_TYPE_DATA, uint8_t id = BD_NET_CD_NONACK_ID);
    network_frame_t NetworkFrameParser(BYTE *data);
    void PacketReceiver(BYTE *packet);
    BYTE *CreateAck(network_frame_t frame);
    BYTE *CreatePong(network_frame_t frame);
    void WritePacket(BYTE *packet);
    BYTE *GenerateFlatTrimCmd();
    BYTE *GenerateAllStates();
    BYTE *GenerateEmergencyCmd();
};
