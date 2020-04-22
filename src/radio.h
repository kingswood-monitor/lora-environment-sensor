#include <Arduino.h>
#define CLIENT_ADDRESS 1

// feather32u4 LoRa pin assignments
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

bool init_radio();
bool send_packet(uint16_t packet_id, uint8_t *buffer, int bytes, int server_address);