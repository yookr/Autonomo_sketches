#include "Sodaq_RN2483.h"

#define loraSerial Serial1

// USE YOUR OWN KEYS!
const uint8_t devAddr[4] = 
{
    0x00, 0x00, 0x02, 0x03
};

// USE YOUR OWN KEYS!
const uint8_t appSKey[16] =
{
    0x0D, 0x0E, 0x0A, 0x0D,
    0x0B, 0x0E, 0x0E, 0x0F,
    0x0C, 0x0A, 0x0F, 0x0E,
    0x0B, 0x0A, 0x0B, 0x0E,
};

// USE YOUR OWN KEYS!
const uint8_t nwkSKey[16] =
{
    0x0D, 0x0E, 0x0A, 0x0D,
    0x0B, 0x0E, 0x0E, 0x0F,
    0x0C, 0x0A, 0x0F, 0x0E,
    0x0B, 0x0A, 0x0B, 0x0E,
};

const uint8_t testPayload[] = 
{
    0x30, 0x31, 0xFF, 0xDE, 0xAD
};

void setup()
{
    loraSerial.begin(LoRaBee.getDefaultBaudRate());

    if (!LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true))
    {
        debugSerial.println("Connection to the network failed!");
        return;
    }

    if (LoRaBee.sendReqAck(1, testPayload, 5, 3) != NoError)
    {
        debugSerial.println("Failed to send the packet!");
    }
}

void loop()
{
}
