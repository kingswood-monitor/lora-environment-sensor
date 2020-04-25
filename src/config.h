#include "packet.pb.h"

/***********************************************************************
 * EEPROM configuration settings                                      */

// Device location
#define CFG_LOCATION Location_greenhouse
// Data refresh rate (milliseconds)
#define REFRESH_MILLIS 1000 // sensor refresh rate (milliseconds)

/*********************************************************************/

// LoRa server address
#define SERVER_ADDRESS 0

// Sensor type
#define SENSOR_TYPE SensorType_environment

// Firmare version
#define FIRMWARE_VERSION "1.0.2"
