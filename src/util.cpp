#include <Arduino.h>
#include "util.h"

// LoRa
RH_RF95 driver(RFM95_CS, RFM95_INT);
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

// Sensors
ClosedCube_HDC1080 hdc1080;
VEML7700 veml;
DFRobot_BMP388_I2C bmp388;

bool has_dhc1080;
bool has_veml7700;
bool has_bmp388;

/*******************************************************
 * Initialisers                                       */

bool init_device()
{
    Serial.begin(115200);
    delay(2000);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.println("** Environment Sensor");

    return true;
}

bool init_radio()
{
    return manager.init();
}

/*******************************************************
 *  Sensors                                           */

bool init_sensors()
{
    if (has_dhc1080 = init_dhc1080())
        Serial.println("DHC1080 [OK]");
    if (has_veml7700 = init_veml7700())
        Serial.println("VEML7700 [OK]");
    if (has_bmp388 = init_bmp388())
        Serial.println("BMP388 [OK]");

    return true;
}

bool init_dhc1080()
{
    hdc1080.begin(0x40);
    return (hdc1080.readDeviceId() == 4176);
}

bool init_veml7700()
{
    veml.begin();
    float lux;
    return !veml.getALSLux(lux);
}

bool init_bmp388()
{
    bmp388.begin();
    return (!((bmp388.readTemperature() == 0) & (bmp388.readPressure() == 0)));
}

float read_battery()
{
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    return measuredvbat;
}
