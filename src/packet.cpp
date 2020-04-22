#include <Arduino.h>
#include <ClosedCube_HDC1080.h>
#include <VEML7700.h>
#include <DFRobot_BMP388_I2C.h>

#include <pb_encode.h>
#include "packet.pb.h"

#include "config.h"
#include "packet.h"
#include "radio.h"

#define VBATPIN A9
#define HAS_BATTERY true

ClosedCube_HDC1080 hdc1080;
VEML7700 veml;
DFRobot_BMP388_I2C bmp388;

bool has_hdc1080 = false;
bool has_veml7700 = false;
bool has_bmp388 = false;
bool has_lora = false;

const Meta META = {
    device_id : CLIENT_ADDRESS,
    device_type : DEVICE_TYPE,
};

bool write_readings(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg);

bool init_dhc1080()
{
    hdc1080.begin(0x40);
    return (hdc1080.readDeviceId() == 4176);
}

bool init_veml7700()
{
    veml.begin();
    float lux;
    return !veml.getALSLux(lux);
}

bool init_bmp388()
{
    bmp388.begin();
    return (!((bmp388.readTemperature() == 0) & (bmp388.readPressure() == 0)));
}

bool init_sensors()
{
    if (has_hdc1080 = init_dhc1080())
        Serial.println("DHC1080 [OK]");
    if (has_veml7700 = init_veml7700())
        Serial.println("VEML7700 [OK]");
    if (has_bmp388 = init_bmp388())
        Serial.println("BMP388 [OK]");
    if (has_lora = has_radio())
        Serial.println("Radio [OK]");

    return true;
}

float read_battery()
{
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    return measuredvbat;
}

uint8_t build_packet(int packet_id, uint8_t *buffer, uint8_t buffer_size)
{
    Packet packet = Packet_init_zero;

    // Build packet
    packet.packet_id = packet_id;
    packet.has_meta = true;
    packet.meta = META;
    packet.measurements.funcs.encode = write_readings;

    // Encode the proto buffer
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, 100);
    if (!pb_encode(&ostream, Packet_fields, &packet))
    {
        Serial.println("Encoding Error");
        Serial.println(ostream.errmsg);
        return ostream.bytes_written;
    }

    return ostream.bytes_written;
}

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
        measurement.type.rssi = rssi();
        if (!send_data(&measurement, ostream, field))
            return false;

        measurement.which_type = Measurement_snr_tag;
        measurement.type.snr = snr();
        if (!send_data(&measurement, ostream, field))
            return false;

        measurement.which_type = Measurement_frequency_error_tag;
        measurement.type.frequency_error = frequency_error();
        if (!send_data(&measurement, ostream, field))
            return false;
    }

    return true;
}
