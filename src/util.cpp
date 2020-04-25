#include <Arduino.h>

#include "config.h"
#include "util.h"

bool init_device()
{
    Serial.begin(115200);
    delay(2000);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.print(F("** Environment Sensor v."));
    Serial.println(FIRMWARE_VERSION);
    Serial.println();

    return true;
}
