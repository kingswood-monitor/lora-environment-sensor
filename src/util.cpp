#include <Arduino.h>
#include "util.h"

bool init_device()
{
    Serial.begin(115200);
    delay(2000);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.println("** Environment Sensor");

    return true;
}
