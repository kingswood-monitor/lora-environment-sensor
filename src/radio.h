bool init_radio();
bool has_radio();
int16_t rssi();
int snr();
int frequency_error();
bool send_measurement(uint16_t packet_id, uint8_t *buffer, uint8_t bytes);