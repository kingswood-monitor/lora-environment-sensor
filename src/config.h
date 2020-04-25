#include "packet.pb.h"

/***********************************************************************
 * EEPROM configuration settings                                      */

// Device location
#define CFG_LOCATION Location_greenhouse
extern Location location;

// Uncomment the line below write the LOCATION_ID to EEPROM memory
// #define WRITE_LOCATION_ID_TO_EEPROM
/*********************************************************************/

// Data refresh rate (milliseconds)
#define CFG_REFRESH_MILLIS 1000 // sensor refresh rate (milliseconds)

// Sensor type
#define SENSOR_TYPE SensorType_environment

// Firmare version
#define FIRMWARE_VERSION "1.0.3"
