/*
    Kingswood Monitoring System
    LoRa environment sensor

    Hardware: Adafruit feather32u4 RFM95
    OS:       Arduino
*/

#include <Arduino.h>

#include "packet.h"
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
  uint8_t bytes_written = build_packet(packet_id++, packet_buffer, 255);
  send_packet(packet_id, packet_buffer, bytes_written, SERVER_ADDRESS);

  delay(REFRESH_MILLIS);
}