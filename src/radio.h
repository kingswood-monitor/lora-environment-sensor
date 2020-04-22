#include <Arduino.h>

#define SERVER_ADDRESS 0

// feather32u4 LoRa pin assignments
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

void set_device_id(int device_id);
bool init_radio();
bool has_radio();
int16_t rssi();
int snr();
int frequency_error();

// int client_address;

bool send_packet(uint16_t packet_id, uint8_t *buffer, uint8_t bytes, uint8_t server_address);