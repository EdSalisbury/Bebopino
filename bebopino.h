#include "Arduino.h"
#include "SoftwareSerial.h"
#include "constants.h"
#include "ESP8266.h"


//#define SSID "BebopDrone-A024060"
#define SSID "edsfamily"
#define PASSWORD "dinah-moe-humm"
#define MUX_RECV 0
#define MUX_SEND 1
#define MAX_ID 50

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

typedef struct
{
    bool landed;
    bool taking_off;
    bool hovering;
    bool flying;
    bool landing;
    bool emergency;
} flying_state_t;

typedef struct
{
    int8_t pitch;
    int8_t roll;
    int8_t yaw;
    int8_t gaz;
} pcmd_t;

class SoftwareSerial;

class Bebopino
{
private:
    ESP8266 *wifi;
    uint8_t seq[MAX_ID];
    SoftwareSerial *mySerial;
    uint8_t battery;
    flying_state_t flying_state;
    uint64_t flying_time;
    pcmd_t pcmd;

public:
    Bebopino();
    void Connect();
    int freeRam();
    void GeneratePCMD();
    void ReceiveData(uint8_t mux_id);
    void NetworkFrameGenerator(BYTE *&frame, uint32_t &frame_len, BYTE *data,
                                uint32_t data_length,
                                uint8_t type = ARNETWORKAL_FRAME_TYPE_DATA,
                                uint8_t id = BD_NET_CD_NONACK_ID);
    network_frame_t NetworkFrameParser(BYTE *data);
    void PacketReceiver(BYTE *packet, uint32_t length);
    void CreateAck(network_frame_t frame);
    void CreatePong(network_frame_t frame);
    void WritePacket(BYTE *packet, uint32_t length);
    void FlatTrim();
    void TakeOff();
    void Land();
    void GenerateAllStates();
    void Emergency();
    uint8_t ValidatePitch(uint8_t val);
    void Up(uint8_t val);
    void Down(uint8_t val);
    void Forward(uint8_t val);
    void Backward(uint8_t val);
    void Left(uint8_t val);
    void Right(uint8_t val);
    void Clockwise(uint8_t val);
    void CounterClockwise(uint8_t val);
    void Stop();
};
