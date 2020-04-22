#pragma once

#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#include <ClosedCube_HDC1080.h>
#include <VEML7700.h>
#include <DFRobot_BMP388_I2C.h>

// Data refresh rate (milliseconds)
#define REFRESH_MILLIS 1000 // sensor refresh rate (milliseconds)

// Device configuration
#define SERVER_ADDRESS 0
#define CLIENT_ADDRESS 1
#define HAS_BATTERY true
#define VBATPIN A9

// feather32u4 LoRa pin assignments
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

// Forward declarations
bool init_device();
bool init_radio();
bool init_sensors();
bool init_dhc1080();
bool init_veml7700();
bool init_bmp388();
float read_battery();