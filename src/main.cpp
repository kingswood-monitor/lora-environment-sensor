/*
    Kingswood Monitoring System
    LoRa environment sensor

    Hardware: Adafruit feather32u4 RFM95
    OS:       Arduino
*/

#include <pb_encode.h>

#include "packet.pb.h"
#include "util.h"

/*******************************************************
 * Radiohead                                          */

extern RHReliableDatagram manager;

/*******************************************************
* Sensors                                             */

extern ClosedCube_HDC1080 hdc1080;
extern VEML7700 veml;
extern DFRobot_BMP388_I2C bmp388;
extern bool has_hdc1080;
extern bool has_veml7700;
extern bool has_bmp388;
extern bool has_lora;

/*******************************************************
 * Protobuf                                           */

#define MAX_PROTOBUF_BYTES 100
const Meta META = {
  device_id : CLIENT_ADDRESS,
  device_type : DeviceType_EnvironmentSensor,
};

// Packet ID
uint32_t packet_id = 0;

// Forward declarations
bool write_readings(pb_ostream_t *stream, const pb_field_iter_t *field, void *const *arg);

void setup()
{
  if (init_device())
    Serial.println("Device started [OK]");

  if (init_radio())
    Serial.println("Radio started  [OK]");

  if (!manager.init())
    Serial.println("init failed");

  if (init_sensors())
  {
    Serial.println("Sensors started [OK]");
  }
}

// uint8_t data[] = "Hello World!";
uint8_t buffer[MAX_PROTOBUF_BYTES];
// Dont put this on the stack:
uint8_t ack_buf[3]; // holds 'OK'

void loop()
{
  Packet packet = Packet_init_zero;

  // Build packet
  packet.packet_id = packet_id++;
  packet.has_meta = true;
  packet.meta = META;
  packet.measurements.funcs.encode = write_readings;

  // Encode the proto buffer
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  if (!pb_encode(&ostream, Packet_fields, &packet))
  {
    Serial.println("Encoding Error");
    return;
  }

  // Transmit the packet
  if (manager.sendtoWait(buffer, ostream.bytes_written, SERVER_ADDRESS))
  {
    Serial.print(F("["));
    Serial.print(packet.packet_id);
    Serial.print(F("->0x"));
    Serial.print(SERVER_ADDRESS, HEX);
    Serial.print(F("] "));
    Serial.print(ostream.bytes_written);
    Serial.print(F(" bytes <- "));

    digitalWrite(LED_BUILTIN, HIGH);

    // Now wait for a reply from the server
    uint8_t len = sizeof(ack_buf);
    uint8_t from;
    if (manager.recvfromAckTimeout(ack_buf, &len, 2000, &from))
    {
      Serial.println((char *)ack_buf);
      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      Serial.println(F("[FAIL] No reply, is server running?"));
    }
  }
  else
  {
    Serial.print(F("[FAIL] SendtoWait: Server 0x"));
    Serial.println(SERVER_ADDRESS);
  }

  delay(REFRESH_MILLIS);
}

/**********************************************************************************************
 * PROTOBUF callback functions
 **********************************************************************************************/

bool send_data(Measurement *m, pb_ostream_t *ostream, const pb_field_iter_t *field)
{
  return pb_encode_tag_for_field(ostream, field) &&
         pb_encode_submessage(ostream, Measurement_fields, m);
}

bool write_readings(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg)
{
  Measurement measurement = Measurement_init_default;

  if (has_hdc1080)
  {
    measurement.sensor = Sensor_HDC1080;

    measurement.which_type = Measurement_temperature_tag;
    measurement.type.temperature = hdc1080.readTemperature();
    if (!send_data(&measurement, ostream, field))
      return false;

    measurement.which_type = Measurement_humidity_tag;
    measurement.type.humidity = hdc1080.readHumidity();
    if (!send_data(&measurement, ostream, field))
      return false;
  }
  if (has_veml7700)
  {
    measurement.sensor = Sensor_VEML7700;

    float lux;
    veml.getALSLux(lux);
    measurement.which_type = Measurement_light_tag;
    measurement.type.light = lux;
    if (!send_data(&measurement, ostream, field))
      return false;
  }
  if (has_bmp388)
  {
    measurement.sensor = Sensor_BMP388;

    measurement.which_type = Measurement_pressure_tag;
    measurement.type.pressure = bmp388.readPressure() / 100;
    if (!send_data(&measurement, ostream, field))
      return false;

    measurement.which_type = Measurement_temperature_tag;
    measurement.type.temperature = bmp388.readTemperature();
    if (!send_data(&measurement, ostream, field))
      return false;
  }

  if (HAS_BATTERY)
  {
    measurement.sensor = Sensor_BATTERY;

    measurement.which_type = Measurement_voltage_tag;
    measurement.type.voltage = read_battery();
    if (!send_data(&measurement, ostream, field))
      return false;
  }

  if (has_lora)
  {
    Serial.println("saving lora");

    measurement.sensor = Sensor_LORA;

    measurement.which_type = Measurement_rssi_tag;
    measurement.type.rssi = 0;
    if (!send_data(&measurement, ostream, field))
      return false;

    measurement.which_type = Measurement_snr_tag;
    measurement.type.snr = 0;
    if (!send_data(&measurement, ostream, field))
      return false;

    measurement.which_type = Measurement_frequency_error_tag;
    measurement.type.frequency_error = 0;
    if (!send_data(&measurement, ostream, field))
      return false;
  }

  return true;
}
