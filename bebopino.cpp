#include "bebopino.h"


void PrintHex(BYTE *cmd, uint32_t length)
{
    for (uint32_t x = 0 ; x < length ; ++x)
    {
        char buffer[3];
        memset(buffer, 0, 3);
        sprintf(buffer, "%02X", cmd[x]);
        Serial.print(String(buffer) + " ");
    }
}

Bebopino::Bebopino()
{
    //Serial.begin(9600);

    // Set up sequence se=
    for (uint8_t id = 0 ; id < MAX_ID ; ++id)
    {
        seq[id] = 0;
    }

    // Initializations
    battery = 0;
    flying_time = 0;
    pcmd.pitch = 0;
    pcmd.roll = 0;
    pcmd.yaw = 0;
    pcmd.gaz = 0;

    mySerial = new SoftwareSerial(2, 3);

    wifi = new ESP8266(*mySerial);

    if (!wifi->setOprToStation())
    {
        Serial.println(F("Cannot find WiFi module!"));
        return;
    }

    if (!wifi->joinAP(SSID, PASSWORD))
    {
        Serial.println(F("Cannot join WiFi network!"));
        return;
    }

    //Enable multiple IP connections
    wifi->enableMUX();

    //Create connections
    //0 = Receive
    //1 = Send
    wifi->listenUDP(MUX_RECV, 43210);
    if (!wifi->registerUDP(MUX_SEND, F("192.168.42.1"), 54321))
    {
        Serial.println(F("Unable to connect to drone"));
        return;
    }

    Serial.println(F("Connected"));
}

uint8_t Bebopino::ValidatePitch(uint8_t val)
{
    if (val > 100)
        return 100;
    else if (val < 100)
        return 0;

    return val | 0;
}


void Bebopino::GeneratePCMD()
{
    uint32_t length = 13;
    BYTE buffer[length];

    uint8_t flag = 1;
    memset(buffer, 0, length);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_PCMD, 2);
    memcpy(buffer + 4, &flag, 1);
    memcpy(buffer + 5, &this->pcmd.roll, 1);
    memcpy(buffer + 6, &this->pcmd.pitch, 1);
    memcpy(buffer + 7, &this->pcmd.yaw, 1);
    memcpy(buffer + 8, &this->pcmd.gaz, 1);
    BYTE *frame;
    uint32_t frame_length;
    NetworkFrameGenerator(frame, frame_length, buffer, length);
    WritePacket(frame, frame_length);
}

void Bebopino::TakeOff()
{
    uint32_t length = 4;
    BYTE buffer[length];
    memset(buffer, 0, length);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_TAKEOFF, 2);
    BYTE *frame;
    uint32_t frame_length;
    NetworkFrameGenerator(frame, frame_length, buffer, length);
    WritePacket(frame, frame_length);
}

void Bebopino::Land()
{
    uint32_t length = 4;
    BYTE buffer[length];
    memset(buffer, 0, length);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_LANDING, 2);
    BYTE *frame;
    uint32_t frame_length;
    NetworkFrameGenerator(frame, frame_length, buffer, length);
    WritePacket(frame, frame_length);
}

void Bebopino::FlatTrim()
{
    uint32_t length = 4;
    BYTE buffer[length];
    memset(buffer, 0, length);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_FLATTRIM, 2);

    BYTE *frame;
    uint32_t frame_length;
    NetworkFrameGenerator(frame, frame_length, buffer, length);
    WritePacket(frame, frame_length);
}

void Bebopino::GenerateAllStates()
{
    uint32_t length = 4;
    BYTE buffer[length];
    memset(buffer, 0, length);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_COMMON, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_COMMON_CLASS_COMMON, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_COMMON_COMMON_CMD_ALLSTATES, 2);
    BYTE *frame;
    uint32_t frame_length;
    NetworkFrameGenerator(frame, frame_length, buffer, length);
    WritePacket(frame, frame_length);
}

void Bebopino::Emergency()
{
    uint32_t length = 4;
    BYTE buffer[length];
    memset(buffer, 0, length);
    memcpy(buffer, &ARCOMMANDS_ID_PROJECT_ARDRONE3, 1);
    memcpy(buffer + 1, &ARCOMMANDS_ID_ARDRONE3_CLASS_PILOTING, 1);
    memcpy(buffer + 2, &ARCOMMANDS_ID_ARDRONE3_PILOTING_CMD_EMERGENCY, 2);
    BYTE *frame;
    uint32_t frame_length;
    NetworkFrameGenerator(frame, frame_length, buffer, length);
    WritePacket(frame, frame_length);
}

void Bebopino::Up(uint8_t val)
{
    this->pcmd.gaz = ValidatePitch(val);
}

void Bebopino::Down(uint8_t val)
{
    this->pcmd.gaz = ValidatePitch(val) * -1;
}

void Bebopino::Forward(uint8_t val)
{
    this->pcmd.pitch = ValidatePitch(val);
}

void Bebopino::Backward(uint8_t val)
{
    this->pcmd.pitch = ValidatePitch(val) * -1;
}

void Bebopino::Left(uint8_t val)
{
    this->pcmd.roll = ValidatePitch(val) * -1;
}

void Bebopino::Right(uint8_t val)
{
    this->pcmd.roll = ValidatePitch(val);
}

void Bebopino::Clockwise(uint8_t val)
{
    this->pcmd.yaw = ValidatePitch(val);
}

void Bebopino::CounterClockwise(uint8_t val)
{
    this->pcmd.yaw = ValidatePitch(val) * -1;
}

void Bebopino::Stop()
{
    this->pcmd.pitch = 0;
    this->pcmd.roll = 0;
    this->pcmd.yaw = 0;
    this->pcmd.gaz = 0;
}

void Bebopino::Connect()
{
    Serial.println(F("Connecting to Drone"));
    FlatTrim();
    GenerateAllStates();
    Serial.println(F("Ready"));
}

void Bebopino::ReceiveData(uint8_t mux_id)
{
    BYTE buffer[64];
    memset(buffer, 0, 64);
    uint32_t len = wifi->recv(mux_id, buffer, sizeof(buffer), 100);

    if (len > 0)
    {
        PacketReceiver(buffer, len);
    }
    else
    {
        return;
    }
}

void Bebopino::NetworkFrameGenerator(BYTE *&frame, uint32_t &frame_length,
    BYTE *data, uint32_t data_length, uint8_t type, uint8_t id)
{
    uint8_t seq_id = 0;
    if (id > MAX_ID)
    {
        Serial.println("ID " + String(id) + " not found in sequence array");
    }
    else
    {
        seq_id = seq[id];
    }

    frame_length = data_length + 7;
    frame = new BYTE[frame_length];
    memset(frame, 0, frame_length);
    memcpy(frame, &type, 1);
    memcpy(frame + 1, &id, 1);
    memcpy(frame + 2, &seq_id, 1);
    memcpy(frame + 3, &frame_length, 4);
    memcpy(frame + 7, data, data_length);

    if (seq_id > 0)
    {
        if (seq[id] == 255)
            seq[id] = 0;
        else
            seq[id]++;
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
    if (frame.data_size > 0)
    {
        frame.data = (BYTE *) malloc(frame.data_size);
        memset(frame.data, 0, frame.data_size);
        memcpy(&frame.data, data + 7, frame.data_size);
    }

    return frame;
}

void Bebopino::PacketReceiver(BYTE *packet, uint32_t length)
{
    Serial.println(F("Received packet: ["));
    PrintHex(packet, length);
    Serial.println("]");

    network_frame_t frame = NetworkFrameParser(packet);

    if (frame.type == ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK)
    {
        CreateAck(frame);
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
                                Serial.println(F("FlatTrim Updated"));
                                break;
                            case ARCOMMANDS_ID_ARDRONE3_PILOTINGSTATE_CMD_FLYINGSTATECHANGED:
                                uint32_t state;
                                memcpy(&state, frame.data + 4, 4);
                                switch (state)
                                {
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDED:
                                        this->flying_state.landed = true;
                                        Serial.println(F("Landed"));
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_TAKINGOFF:
                                        this->flying_state.taking_off = true;
                                        Serial.println(F("Taking off"));
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_HOVERING:
                                        this->flying_state.hovering = true;
                                        Serial.println(F("Hovering"));
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_FLYING:
                                        this->flying_state.flying = true;
                                        Serial.println(F("Flying"));
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_LANDING:
                                        this->flying_state.landing = true;
                                        Serial.println(F("Landing"));
                                        break;
                                    case ARCOMMANDS_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED_STATE_EMERGENCY:
                                        this->flying_state.emergency = true;
                                        Serial.println(F("Emergency"));
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
        CreatePong(frame);
    }
}

void Bebopino::WritePacket(BYTE *packet, uint32_t length)
{
    Serial.print(F("Sending packet: ["));
    PrintHex(packet, length);
    wifi->send(MUX_SEND, packet, length);
    Serial.println("]");
    free(packet);
}

void Bebopino::CreateAck(network_frame_t frame)
{
    uint32_t length = 2;
    BYTE data[length];
    memcpy(data, &frame.seq, 1);
    uint16_t id = frame.id + ARNETWORKAL_MANAGER_DEFAULT_ID_MAX / 2;

    BYTE *new_frame = {};
    uint32_t new_frame_length;
    NetworkFrameGenerator(new_frame, new_frame_length, data, length, ARNETWORKAL_FRAME_TYPE_ACK, id);
    WritePacket(new_frame, new_frame_length);
}

void Bebopino::CreatePong(network_frame_t frame)
{
    BYTE *new_frame = {};
    uint32_t new_frame_length;
    NetworkFrameGenerator(new_frame, new_frame_length, frame.data,
        frame.data_size, ARNETWORKAL_FRAME_TYPE_DATA,
        ARNETWORK_MANAGER_INTERNAL_BUFFER_ID_PONG);
    WritePacket(new_frame, new_frame_length);
}
