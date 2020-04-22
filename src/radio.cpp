#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#include "radio.h"

RH_RF95 driver(RFM95_CS, RFM95_INT);
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

// Dont put this on the stack:
uint8_t ack_buf[3]; // holds 'OK'

bool init_radio()
{
    // has_lora = true; // TODO get radio status
    return manager.init();
}

bool send_packet(uint16_t packet_id, uint8_t *buffer, int bytes, int server_address)
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

        // Now wait for a reply from the server
        uint8_t len = sizeof(ack_buf);
        uint8_t from;
        if (manager.recvfromAckTimeout(ack_buf, &len, 2000, &from))
        {
            Serial.println((char *)ack_buf);
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