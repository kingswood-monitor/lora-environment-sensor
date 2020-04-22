#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#include "radio.h"
#include "config.h"

RH_RF95 driver(RFM95_CS, RFM95_INT);
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

// Dont put this on the stack:
uint8_t ok[3] = "OK";
uint8_t ok_len = 3;

bool init_radio(uint8_t client_address)
{
    // has_lora = true; // TODO get radio status
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

bool send_packet(uint16_t packet_id, uint8_t *buffer, uint8_t bytes, uint8_t server_address)
{
    if (manager.sendtoWait(buffer, bytes, server_address))
    {
        Serial.print(F("["));
        Serial.print(packet_id);
        Serial.print(F("->0x"));
        Serial.print(server_address, HEX);
        Serial.print(F("] "));
        Serial.print(bytes);
        Serial.print(F(" bytes <- "));

        digitalWrite(LED_BUILTIN, HIGH);

        if (manager.recvfromAckTimeout(ok, &ok_len, 2000))
        {
            Serial.println((char *)ok);
            digitalWrite(LED_BUILTIN, LOW);
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
        Serial.println(server_address);
        return false;
    }
}