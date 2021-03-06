

#include <Arduino.h>
#include <ClosedCube_HDC1080.h>
#include <VEML7700.h>
#include <DFRobot_BMP388_I2C.h>

#include <pb_encode.h>
#include "packet.pb.h"

#include "config.h"
#include "util.h"
#include "sensors.h"
#include "radio.h"

#define MAX_PROTOBUF_BYTES 115

#define VBATPIN A9
#define HAS_BATTERY true

ClosedCube_HDC1080 hdc1080;
VEML7700 veml;
DFRobot_BMP388_I2C bmp388;

const Meta META = {
    location : CFG_LOCATION,
    sensor_type : SENSOR_TYPE,
    firmware_version : FIRMWARE_VERSION,
};

bool has_hdc1080 = false;
bool has_veml7700 = false;
bool has_bmp388 = false;
bool has_lora = false;

bool init_hdc1080();
bool init_veml7700();
bool init_bmp388();

bool encode_measurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg);
bool encode_field(Measurement *m, pb_ostream_t *ostream, const pb_field_iter_t *field);

bool init_sensors()
{
    has_hdc1080 = init_hdc1080();
    if (has_hdc1080)
        Serial.println("[OK] Sensor HDC1080");

    has_veml7700 = init_veml7700();
    if (has_veml7700)
        Serial.println("[OK] Sensor VEML7700");

    has_bmp388 = init_bmp388();
    if (has_bmp388)
        Serial.println("[OK] Sensor BMP388");

    has_lora = has_radio();
    if (has_lora)
        Serial.println("[OK] Sensor LoRa radio");

    return true;
}

bool init_hdc1080()
{
    hdc1080.begin(0x40);
    return (hdc1080.readDeviceId() == 4176);
}

bool init_veml7700()
{
    veml.begin();
    // default intergation time of 100ms results in reading clipped to 2126.52
    // 50ms / Gain 1/8 seems to be 60klux max, for direct sunlight of 50k
    veml.setIntegrationTime(VEML7700::als_itime_t::ALS_INTEGRATION_50ms);
    veml.setGain(VEML7700::als_gain_t::ALS_GAIN_d8);

    float lux;
    return !veml.getALSLux(lux);
}

bool init_bmp388()
{
    bmp388.begin();
    return (!((bmp388.readTemperature() == 0) & (bmp388.readPressure() == 0)));
}

float read_battery()
{
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    return measuredvbat;
}

uint8_t read_and_encode_sensors(int packet_id, uint8_t *buffer, uint8_t buffer_size)
{
    Packet packet = Packet_init_zero;

    // Build packet
    packet.packet_id = packet_id;
    packet.has_meta = true;
    packet.meta = META;
    packet.measurements.funcs.encode = encode_measurements;

    // Encode the proto buffer
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, MAX_PROTOBUF_BYTES);
    if (!pb_encode(&ostream, Packet_fields, &packet))
    {
        Serial.println("Encoding Error");
        Serial.println(ostream.errmsg);
        return ostream.bytes_written;
    }

    return ostream.bytes_written;
}

bool encode_measurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg)
{
    Measurement measurement = Measurement_init_default;

    if (has_hdc1080)
    {
        measurement.sensor = Sensor_HDC1080;

        measurement.which_type = Measurement_temperature_tag;
        measurement.type.temperature = hdc1080.readTemperature();
        if (!encode_field(&measurement, ostream, field))
            return false;

        measurement.which_type = Measurement_humidity_tag;
        measurement.type.humidity = hdc1080.readHumidity();
        if (!encode_field(&measurement, ostream, field))
            return false;

        // Calculate dewpoint and attach to the virtual sensor f
        float temp_c = hdc1080.readTemperature();
        float relative_humidity = hdc1080.readHumidity();
        float dew_point_c = dew_point(temp_c, relative_humidity);

        measurement.sensor = Sensor_VIRTUAL;
        measurement.which_type = Measurement_dewpoint_tag;
        measurement.type.dewpoint = dew_point_c;
        if (!encode_field(&measurement, ostream, field))
            return false;
    }
    if (has_veml7700)
    {
        measurement.sensor = Sensor_VEML7700;

        float raw, lux;
        veml.getALSLux(raw);
        lux = raw * 0.9216; // 50ms / Gain 1/8
        Serial.print(raw);
        Serial.print(lux);

        measurement.which_type = Measurement_light_tag;
        measurement.type.light = lux;

        if (!encode_field(&measurement, ostream, field))
            return false;
    }
    if (has_bmp388)
    {
        measurement.sensor = Sensor_BMP388;

        measurement.which_type = Measurement_pressure_tag;
        measurement.type.pressure = bmp388.readPressure() / 100;
        if (!encode_field(&measurement, ostream, field))
            return false;

        measurement.which_type = Measurement_temperature_tag;
        measurement.type.temperature = bmp388.readTemperature();
        if (!encode_field(&measurement, ostream, field))
            return false;
    }

    if (HAS_BATTERY)
    {
        measurement.sensor = Sensor_BATTERY;

        measurement.which_type = Measurement_voltage_tag;
        measurement.type.voltage = read_battery();
        if (!encode_field(&measurement, ostream, field))
            return false;
    }

    if (has_lora)
    {
        measurement.sensor = Sensor_LORA;

        measurement.which_type = Measurement_rssi_tag;
        measurement.type.rssi = rssi();
        if (!encode_field(&measurement, ostream, field))
            return false;

        measurement.which_type = Measurement_snr_tag;
        measurement.type.snr = snr();
        if (!encode_field(&measurement, ostream, field))
            return false;

        measurement.which_type = Measurement_frequency_error_tag;
        measurement.type.frequency_error = frequency_error();
        if (!encode_field(&measurement, ostream, field))
            return false;
    }

    return true;
}

bool encode_field(Measurement *m, pb_ostream_t *ostream, const pb_field_iter_t *field)
{
    return pb_encode_tag_for_field(ostream, field) &&
           pb_encode_submessage(ostream, Measurement_fields, m);
}
