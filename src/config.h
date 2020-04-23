#include "packet.pb.h"

#define FIRMWARE_VERSION "1.0.0"

// Data refresh rate (milliseconds)
#define REFRESH_MILLIS 1000 // sensor refresh rate (milliseconds)

// Sensor type
#define SENSOR_TYPE SensorType_Environment

// Logical addresses on the network
#define SERVER_ADDRESS 0
#define LOCATION_ID 2 // 2:outdoors, 3:greenhouse
