/*
    Kingswood Monitoring System
    LoRa environment sensor

    Hardware: Adafruit feather32u4 RFM95
    OS:       Arduino
*/

#include <Arduino.h>

#include "util.h"
#include "radio.h"
#include "packet.h"
#include "config.h"

// Packet ID
uint16_t packet_id = 0;

void setup()
{
  // This sets the device_id in flash memory.
  // Device will appear to hang - uncomment, flash, recomment, then restart.
  // set_device_id(2);

  if (init_device())
    Serial.println("Device started [OK]");

  if (init_radio())
    Serial.println("Radio started  [OK]");

  if (init_sensors())
    Serial.println("Sensors started [OK]");
}

void loop()
{
  uint8_t packet[255];
  uint8_t bytes_written = build_packet(packet_id++, packet, 255);

  send_packet(packet_id, packet, bytes_written, SERVER_ADDRESS);

  delay(REFRESH_MILLIS);
}