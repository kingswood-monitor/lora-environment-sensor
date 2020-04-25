#include <Arduino.h>

#include "config.h"

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

float dew_point(float temp_c, float rel_hum)
{
    float ans = (temp_c - (14.55 + 0.114 * temp_c) * (1 - (0.01 * rel_hum)) - pow(((2.5 + 0.007 * temp_c) * (1 - (0.01 * rel_hum))), 3) - (15.9 + 0.117 * temp_c) * pow((1 - (0.01 * rel_hum)), 14));
    return ans;
}