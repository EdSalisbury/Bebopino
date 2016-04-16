#include "bebopino.h"


void PrintHex(unsigned char *cmd, uint32_t length)
{
    for (uint32_t x = 0 ; x < length ; ++x)
    {
        char buffer[3];
        memset(buffer, 0, 3);
        sprintf(buffer, "%02X", cmd[x]);
        //Serial.print(String(buffer) + " ");
        //Serial.print(String(cmd[x]) + " ");
        Serial.print(String(buffer) + " ");

    }
}


int Bebopino::freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

Bebopino::Bebopino()
{
    // Set up sequence numbers (TODO: Change to hash table instead)
    seq = new uint16_t[MAX_ID];
    for (uint16_t id = 0 ; id < MAX_ID ; ++id)
        seq[id] = 0;

    mySerial = new SoftwareSerial(2, 3, false);
    wifi = new ESP8266(*mySerial);
    Serial.begin(9600);

    if (!wifi->setOprToStation())
    {
        Serial.println("Cannot find WiFi module!");
        return;
    }

    if (!wifi->joinAP(SSID, PASSWORD))
    {
        Serial.println("Cannot join WiFi network!");
        return;
    }

    // Enable multiple IP connections
    wifi->enableMUX();

    // Create connections
    // 0 = Receive
    // 1 = Send
    Serial.println(wifi->listenUDP(MUX_RECV, 4242));
    Serial.println(wifi->registerUDP(MUX_SEND, "192.168.1.116", 55056));
    Serial.println("Connected to WiFi with IP address " + String(wifi->getLocalIP().c_str()));
}

char *Bebopino::GenerateTakeoffCmd()
{
    char *buffer = new char[4];
    memset(buffer, 0, 4);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_TAKEOFF, 2);
    return buffer;
}

char *Bebopino::GenerateLandingCmd()
{
    char *buffer = new char[4];
    memset(buffer, 0, 4);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_LANDING, 2);
    return buffer;
}

unsigned char *Bebopino::GeneratePCMD(int flag, int roll, int pitch, int yaw, int gaz)
{
    unsigned char *buffer = new unsigned char[13];
    memset(buffer, 0, 13);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_PCMD, 2);
    memcpy(buffer + 4, &flag, 1);
    memcpy(buffer + 5, &roll, 1);
    memcpy(buffer + 6, &pitch, 1);
    memcpy(buffer + 7, &yaw, 1);
    memcpy(buffer + 8, &gaz, 1);
    return buffer;
}

BYTE *Bebopino::NetworkFrameGenerator(BYTE *cmd, uint32_t length,
    uint8_t type = ARNETWORKAL_FRAME_TYPE_DATA,
    uint8_t id = BD_NET_CD_NONACK_ID)
{
    uint32_t framelen = length + 7;
    BYTE *buffer = new BYTE[framelen];

    memset(buffer, 0, framelen);
    memcpy(buffer, &type, 1);
    memcpy(buffer + 1, &id, 1);
    memcpy(buffer + 2, &(seq[id]), 1);
    memcpy(buffer + 3, &framelen, 4);
    memcpy(buffer + 7, cmd, length);

    if (seq[id] == 255)
        seq[id] = 0;
    else
        seq[id]++;

    return buffer;
}

void Bebopino::Connect()
{
    Serial.println("Connecting");

    BYTE *cmd = GeneratePCMD(1, 0, 0, 0, 0);
    BYTE *frame = NetworkFrameGenerator(cmd, 13);

    free(frame);
    free(cmd);
}



void Bebopino::ReceiveData(uint8_t mux_id)
{
    BYTE buffer[128];
    memset(buffer, 0, 128);
    uint32_t len = wifi->recv(mux_id, buffer, sizeof(buffer), 100);

    if (len > 0)
    {
        PrintHex((BYTE *)buffer, len);
    }
    else
    {
        return;
    }
}

network_frame_t Bebopino::NetworkFrameParser(BYTE *data)
{
    network_frame_t frame;
    frame.type = data[0];
    frame.id = data[1];
    frame.seq = data[2];
    memcpy(&frame.frame_size, data + 3, 4);

    frame.data_size = frame.frame_size - 7;
    if (frame.frame_size > 7)
    {
        frame.data = (BYTE *) malloc(frame.data_size) + 1;
        memcpy(&frame.data, data + 7, frame.data_size);
    }

    return frame;
}

BYTE *Bebopino::CreateAck(network_frame_t frame)
{
    BYTE data[2];
    memcpy(data, &frame.seq, 1);
    uint16_t id = frame.id + ARNETWORKAL_MANAGER_DEFAULT_ID_MAX / 2;
    return NetworkFrameGenerator(data, 2, ARNETWORKAL_FRAME_TYPE_ACK, id);
}
