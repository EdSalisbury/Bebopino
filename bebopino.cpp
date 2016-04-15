#include "bebopino.h"


void PrintHex(unsigned char *cmd, uint32_t length)
{
    for (uint32_t x = 0 ; x < length ; ++x)
    {
        char buffer[3];
        memset(buffer, 0, 3);
        //sprintf(buffer, "%02X", cmd[x]);
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
    // Set up sequence numbers
    seq = new uint8_t[MAX_SEQ];
    for (uint8_t i = 0 ; i < MAX_SEQ ; ++i)
        seq[i] = 0;

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
    wifi->listenUDP(MUX_RECV, 4242);
    wifi->registerUDP(MUX_SEND, "192.168.1.116", 55056);
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

unsigned char *Bebopino::NetworkFrameGenerator(unsigned char *cmd, uint32_t length)
{
    uint32_t framelen = length + 7;
    unsigned char *buffer = new unsigned char[framelen];

    uint8_t id = BD_NET_CD_NONACK_ID;

    memset(buffer, 0, framelen);
    memcpy(buffer, &ARNETWORKAL_FRAME_TYPE_DATA, 1);
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
    int i = 0;
    Serial.println("Free RAM: " + String(freeRam()));
    for (i = 0 ; i < 300 ; ++i)
    {
        unsigned char *cmd = GeneratePCMD(1, 0, 0, 0, 0);
        unsigned char *frame = NetworkFrameGenerator(cmd, 13);

        PrintHex(frame, 20);
        Serial.println("");

        free(frame);
        free(cmd);
    }
    Serial.println("Free RAM: " + String(freeRam()));
}

void Bebopino::ReceiveData(uint8_t mux_id)
{
    //Serial.println("Checking for data");
    uint8_t buffer[128];
    memset(buffer, 0, 128);
    //Serial.println("Free RAM: " + String(freeRam()));


    // while (millis() - start < timeout) {

    //     if(m_puart->available() > 0) {
    //         a = m_puart->read();
    //         data += a;
    //     }
    uint32_t len = wifi->recv(&mux_id, buffer, sizeof(buffer), 100);

    //uint32_t ESP8266::recv(uint8_t mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
    if (len > 0)
    {
        Serial.println("Length: " + String(len));
        Serial.println((char*)buffer);

        //return (char*)buffer;
    }
    else
    {
        return;
    }
}


