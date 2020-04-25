/**
 * sensors.h
 *
 * Read sensors and encode data in Protobuf
 */

bool init_sensors();
uint8_t read_and_encode_sensors(int packet_id, uint8_t *buffer, uint8_t buffer_size);
