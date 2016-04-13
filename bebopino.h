#include <string.h>
#include "constants.h"
#include "Arduino.h"
#include "esp8266.h"

class Bebopino
{
private:
    Esp8266 *wifi;

public:
    void takeoff();
    void land();
    Bebopino();
};
