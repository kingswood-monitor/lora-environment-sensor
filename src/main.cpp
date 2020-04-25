/*
    Kingswood Monitoring System
    LoRa environment sensor

    Hardware: Adafruit feather32u4 RFM95
    OS:       Arduino
*/

#include <Arduino.h>
#include <Adafruit_SleepyDog.h>

#include "sensors.h"
#include "radio.h"

#include "config.h"
#include "util.h"

void setup()
{
  if (init_device())
    Serial.println("Device started [OK]");

  if (init_sensors())
    Serial.println("Sensors started [OK]");

  if (init_radio())
    Serial.println("Radio started  [OK]");
}

// Packet ID
uint16_t packet_id = 0;
uint8_t packet_buffer[255];

void loop()
{
  uint8_t bytes_written = read_and_encode_sensors(packet_id++, packet_buffer, 255);
  send_measurement(packet_id, packet_buffer, bytes_written);

  // delay(CFG_REFRESH_SECONDS * 1000);
  int sleepMS = Watchdog.sleep(CFG_REFRESH_SECONDS * 1000);
}