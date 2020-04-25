#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#include "radio.h"
#include "config.h"
// #include "IdGuard.h"

// feather32u4 LoRa pin assignments
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

// LoRa server address
#define SERVER_ADDRESS 0
#define TRANSMIT_POWER 5 // valid levels +5 to +23

RH_RF95 driver(RFM95_CS, RFM95_INT);
RHReliableDatagram manager(driver, CFG_LOCATION);

void set_device_id(int device_id);

// Dont put this on the stack:
uint8_t ok[3] = "OK";
uint8_t ok_len = 3;

// void set_device_id(int device_id)
// {
//     IdGuard.writeIdAndRestartDevice(device_id);
//     Serial.print("Device is set to: ");
//     Serial.println(device_id);
//     Serial.println("Comment out enabling line in setup()");
// }

bool init_radio()
{
    // client_address = IdGuard.readId();
    driver.setTxPower(TRANSMIT_POWER);
    driver.init();
    return manager.init();
}

bool has_radio()
{
    return manager.init();
}

int16_t rssi()
{
    return driver.lastRssi();
}

int snr()
{
    return driver.lastSNR();
}

int frequency_error()
{
    return driver.frequencyError();
}

bool send_measurement(uint16_t packet_id, uint8_t *buffer, uint8_t bytes)
{
    if (manager.sendtoWait(buffer, bytes, SERVER_ADDRESS))
    {
        Serial.print(F("["));
        Serial.print(packet_id);
        Serial.print(F("->0x"));
        Serial.print(SERVER_ADDRESS, HEX);
        Serial.print(F("] "));
        Serial.print(bytes);
        Serial.print(F(" bytes <- "));

        digitalWrite(LED_BUILTIN, HIGH);

        if (manager.recvfromAckTimeout(ok, &ok_len, 2000))
        {
            Serial.println((char *)ok);
            digitalWrite(LED_BUILTIN, LOW);

            driver.sleep();

            return true;
        }
        else
        {
            Serial.println(F("[FAIL] No reply, is server running?"));
            return false;
        }
    }
    else
    {
        Serial.print(F("[FAIL] SendtoWait: Server 0x"));
        Serial.println(SERVER_ADDRESS);
        return false;
    }
}