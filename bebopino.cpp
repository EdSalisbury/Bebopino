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

    // Initializations
    battery = 0;
    flying_time = 0;

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

BYTE *Bebopino::GenerateTakeoffCmd()
{
    BYTE *buffer = new BYTE[5];
    memset(buffer, 0, 5);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_TAKEOFF, 2);
    return buffer;
}

BYTE *Bebopino::GenerateLandingCmd()
{
    BYTE *buffer = new BYTE[5];
    memset(buffer, 0, 5);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_LANDING, 2);
    return buffer;
}

BYTE *Bebopino::GeneratePCMD(int flag, int roll, int pitch, int yaw, int gaz)
{
    BYTE *buffer = new BYTE[14];
    memset(buffer, 0, 14);
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

BYTE *Bebopino::GenerateFlatTrimCmd()
{
    BYTE *buffer = new BYTE[5];
    memset(buffer, 0, 5);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_COMMON, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_COMMON_CLASS_COMMON, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_COMMON_COMMON_CMD_ALLSTATES, 2);
    return buffer;
}

BYTE *Bebopino::GenerateAllStates()
{
    BYTE *buffer = new BYTE[5];
    memset(buffer, 0, 5);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_FLATTRIM, 2);
    return buffer;
}

void Bebopino::Connect()
{
    Serial.println("Connecting");

    BYTE *cmd = GeneratePCMD(1, 0, 0, 0, 0);
    BYTE *frame = NetworkFrameGenerator(cmd);

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

BYTE *Bebopino::NetworkFrameGenerator(BYTE *cmd, uint8_t type, uint8_t id)
{
    uint32_t framelen = strlen((char*)cmd) + 8;
    BYTE *buffer = new BYTE[framelen];
    memset(buffer, 0, framelen);
    memcpy(buffer, &type, 1);
    memcpy(buffer + 1, &id, 1);
    memcpy(buffer + 2, &(seq[id]), 1);
    memcpy(buffer + 3, &framelen, 4);
    memcpy(buffer + 7, cmd, strlen((char*)cmd));

    if (seq[id] == 255)
        seq[id] = 0;
    else
        seq[id]++;

    return buffer;
}

network_frame_t Bebopino::NetworkFrameParser(BYTE *data)
{
    network_frame_t frame;
    frame.type = data[0];
    frame.id = data[1];
    frame.seq = data[2];
    memcpy(&frame.size, data + 3, 4);

    uint32_t data_size = frame.size - 7;
    if (frame.size > 7)
    {
        frame.data = (BYTE *) malloc(data_size) + 1;
        memset(frame.data, 0, data_size + 1);
        memcpy(&frame.data, data + 7, data_size);
    }

    return frame;
}

BYTE *Bebopino::CreateAck(network_frame_t frame)
{
    BYTE data[2];
    memcpy(data, &frame.seq, 1);
    uint16_t id = frame.id + ARNETWORKAL_MANAGER_DEFAULT_ID_MAX / 2;
    return NetworkFrameGenerator(data, ARNETWORKAL_FRAME_TYPE_ACK, id);
}

void Bebopino::PacketReceiver(BYTE *packet)
{
    network_frame_t frame = NetworkFrameParser(packet);

    if (frame.type == ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK)
    {
        WritePacket(CreateAck(frame));
    }

    if (frame.id == BD_NET_DC_EVENT_ID ||
        frame.id == BD_NET_DC_NAVDATA_ID)
    {
        uint8_t cmd_project = frame.data[0];
        uint8_t cmd_class = frame.data[1];
        uint16_t cmd_id;
        memcpy(&cmd_id, frame.data + 2, 2);

        switch (cmd_project)
        {
            case ARCOMMANDS_ID_PROJECT_COMMON:
                switch (cmd_class)
                {
                    case ARCOMMANDS_ID_COMMON_CLASS_COMMONSTATE:
                        switch (cmd_id)
                        {
                            case ARCOMMANDS_ID_COMMON_COMMONSTATE_CMD_BATTERYSTATECHANGED:
                                this->battery = frame.data[4];
                                Serial.println("Battery is at " + String(this->battery));
                                break;
                        }
                        break;
                }
                break;
            case ARCOMMANDS_ID_PROJECT_ARDRONE3:
                switch (cmd_class)
                {
                    case ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTINGSTATE:
                        switch (cmd_id)
                        {
                            case ARCOMMANDS_ID_ARDRONE3_PILOTINGSTATE_CMD_FLATTRIMCHANGED:
                                break;
                            case ARCOMMANDS_ID_ARDRONE3_PILOTINGSTATE_CMD_FLYINGSTATECHANGED:
                                uint32_t state;
                                memcpy(&state, frame.data + 4, 4);
                                switch (state)
                                {
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED:
                                        this->flying_state.landed = true;
                                        Serial.println("Landed");
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_TAKINGOFF:
                                        this->flying_state.taking_off = true;
                                        Serial.println("Taking off");
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_HOVERING:
                                        this->flying_state.hovering = true;
                                        Serial.println("Hovering");
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_FLYING:
                                        this->flying_state.flying = true;
                                        Serial.println("Flying");
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDING:
                                        this->flying_state.landing = true;
                                        Serial.println("Landing");
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_EMERGENCY:
                                        this->flying_state.emergency = true;
                                        Serial.println("Emergency");
                                        break;
                                }
                                break;
                        }
                        break;
                }
                break;
            }
        }

    if (frame.id == ARNETWORK_MANAGER_INTERNAL_BUFFER_ID_PING)
    {
        uint64_t time;
        memcpy(&time, frame.data, 8);
        WritePacket(CreatePong(frame));
    }
}

void Bebopino::WritePacket(BYTE *packet)
{
    wifi->send(MUX_SEND, packet, strlen((char*)packet));
    free(packet);
}

BYTE *Bebopino::CreatePong(network_frame_t frame)
{
    return NetworkFrameGenerator(frame.data, ARNETWORKAL_FRAME_TYPE_DATA, ARNETWORK_MANAGER_INTERNAL_BUFFER_ID_PONG);
}
