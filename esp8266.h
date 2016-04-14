#include <string.h>
#include "Arduino.h"
#include "SoftwareSerial.h"

class SoftwareSerial;

class Esp8266
{
private:
    SoftwareSerial *comm;

public:
    Esp8266(int rx_pin, int tx_pin);
    bool Connect(String ssid, String password);
    void SendUdp(String dest_ip, long dest_port, String data);
};

