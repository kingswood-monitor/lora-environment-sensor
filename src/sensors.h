/**
 * sensors.h
 *
 * Read sensors and encode data in Protobuf
 */

#define MAX_PROTOBUF_BYTES 120

bool init_sensors();
uint8_t build_packet(int packet_id, uint8_t *buffer, uint8_t buffer_size);
