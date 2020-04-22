#include <Arduino.h>

// feather32u4 LoRa pin assignments
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

bool init_radio(uint8_t client_address);
bool send_packet(uint16_t packet_id, uint8_t *buffer, uint8_t bytes, uint8_t server_address);