#include "esp8266.h"

const long BAUD_RATE = 9600;
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

void Esp8266::SendUdp(String dest_ip, long dest_port, String data)
{
    comm->setTimeout(TIMEOUT);
    String cmd = "AT+CIPSTART=\"UDP\",\"" + dest_ip + "\"," + dest_port;
    comm->println(cmd);
    // Serial.println("Received: \"" + comm->readString() + "\"");
    if (comm->find("ALREADY CONNECTED"))
    {
      // OK  
    }
    else if (comm->find("ERROR"))
    {
        Serial.println("Error opening " + dest_ip + ":" + dest_port + ".");
        return;
    }

    cmd = "AT+CIPSEND=" + String(data.length());
    
    Serial.println("Sending command: " + cmd);
    comm->println(cmd);
    if (comm->find(">"))
    {
        Serial.println("Sending data");
        comm->print(data);
    }
    if (comm->find("SEND OK"))
    {
        Serial.println("Data sent");
    }
    else
    {
        Serial.println("Unable to send data");
    }
}
