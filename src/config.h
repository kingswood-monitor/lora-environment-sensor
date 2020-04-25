#include "packet.pb.h"

#define FIRMWARE_VERSION "1.0.2"

// Data refresh rate (milliseconds)
#define REFRESH_MILLIS 1000 // sensor refresh rate (milliseconds)

// Sensor type
#define SENSOR_TYPE SensorType_environment

// Logical addresses on the network
#define SERVER_ADDRESS 0
#define CFG_LOCATION Location_greenhouse
