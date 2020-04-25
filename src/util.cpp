#include <Arduino.h>

#include "IdGuard.h"
#include "config.h"
#include "util.h"

Location location;

void get_set_config();

bool init_device()
{
    Serial.begin(115200);
    delay(2000);

    get_set_config();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.print(F("** Environment Sensor v."));
    Serial.println(FIRMWARE_VERSION);
    Serial.println();

    return true;
}

void get_set_config()
{
#ifdef WRITE_LOCATION_ID_TO_EEPROM
    IdGuard.error_led_pin = LED_BUILTIN;
    IdGuard.writeIdAndRestartDevice((int)CFG_LOCATION);
    location = CFG_LOCATION;

    Serial.println(F("INFO: Wrote settings to EEPROM"));
    Serial.println(F("INFO: Remember to comment WRITE_LOCATION_ID_TO_EEPROM in util.h"));
#else
    location = (Location)IdGuard.readId();

    Serial.println(F("INFO: Loaded configuration from EEPROM"));
#endif
    Serial.print(F("INFO: LOCATION_ID: "));
    Serial.println(location);
}