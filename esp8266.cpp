#include "esp8266.h"

const long BAUD_RATE = 115200;
const int TIMEOUT = 20000;

Esp8266::Esp8266(int rx_pin, int tx_pin)
{
    comm = new SoftwareSerial(rx_pin, tx_pin, false);
    comm->begin(BAUD_RATE);
    delay(1000);
    comm->write("AT\r\n");
}

bool Esp8266::Connect(String ssid, String password)
{
    comm->setTimeout(TIMEOUT);
    comm->println("AT+CWMODE=1");
    String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
    comm->println(cmd);
    if (comm->find("OK"))
    {
        return true;
    }
    return false;
}
