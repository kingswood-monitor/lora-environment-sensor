#pragma once

#include <ClosedCube_HDC1080.h>
#include <VEML7700.h>
#include <DFRobot_BMP388_I2C.h>

// Data refresh rate (milliseconds)
#define REFRESH_MILLIS 1000 // sensor refresh rate (milliseconds)

// Device configuration
#define SERVER_ADDRESS 0

#define HAS_BATTERY true
#define VBATPIN A9

// Forward declarations
bool init_device();

bool init_sensors();
bool init_dhc1080();
bool init_veml7700();
bool init_bmp388();
float read_battery();