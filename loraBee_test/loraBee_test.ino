/*
* Copyright (c) 2015 SODAQ. All rights reserved.
*
* This file is part of Sodaq_RN2483.
*
* Sodaq_RN2483 is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation, either version 3 of
* the License, or(at your option) any later version.
*
* Sodaq_RN2483 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with Sodaq_RN2483.  If not, see
* <http://www.gnu.org/licenses/>.
*/

#include <Sodaq_RN2483.h>

// MBili
//#define debugSerial Serial
// Autonomo
#define debugSerial SerialUSB
#define loraSerial Serial1
#define BEE_VCC 17

const uint8_t devAddr[4] =
{
  0x02, 0x01, 0x36, 0x03
};

// USE YOUR OWN KEYS!
const uint8_t appSKey[16] =
{
  0x2B, 0x7E, 0x15, 0x16, 
  0x28, 0xAE, 0xD2, 0xA6, 
  0xAB, 0xF7, 0x15, 0x88, 
  0x09, 0xCF, 0x4F, 0x3C,
};

// USE YOUR OWN KEYS!
const uint8_t nwkSKey[16] =
{
  0x2B, 0x7E, 0x15, 0x16, 
  0x28, 0xAE, 0xD2, 0xA6, 
  0xAB, 0xF7, 0x15, 0x88, 
  0x09, 0xCF, 0x4F, 0x3C,
};

uint8_t testPayload[] =
{
  // 0x30, 0x31, 0xFF, 0xDE, 0xAD
  0x00, 0x57, 0x6f, 0x72, 0x6b, 0x69, 0x6e, 0x67
};

void setup()
{
  digitalWrite(BEE_VCC, HIGH);
  debugSerial.begin(57600);
  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  LoRaBee.setDiag(debugSerial); // optional
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true))
  {
    debugSerial.println("Connection to the network was successful.");
  }
  else
  {
    debugSerial.println("Connection to the network failed!");
  }
}

void loop()
{
  debugSerial.println("Sleeping for 5 seconds before starting sending out test packets.");
  for (uint8_t i = 5; i > 0; i--)
  {
    debugSerial.println(i);
    delay(1000);
  }

  // send 10 packets, with at least a 5 seconds delay after each transmission (more seconds if the device is busy)
  uint8_t i = 10;
  while (i > 0)
  {
    testPayload[0] = i; // change first byte

    // original: switch (LoRaBee.sendReqAck(1, testPayload, 5, 3))
    switch (LoRaBee.sendReqAck(1, testPayload, 8, 3))
    {
    case NoError:
      debugSerial.println("Successful transmission.");
      i--;
      break;
    case NoResponse:
      debugSerial.println("There was no response from the device.");
      break;
    case Timeout:
      debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      delay(20000);
      break;
    case PayloadSizeError:
      debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
      break;
    case InternalError:
      debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe program will now halt.");
      while (1) {};
      break;
    case Busy:
      debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
      delay(10000);
      break;
    case NetworkFatalError:
      debugSerial.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe program will now halt.");
      while (1) {};
      break;
    case NotConnected:
      debugSerial.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe program will now halt.");
      while (1) {};
      break;
    case NoAcknowledgment:
      debugSerial.println("There was no acknowledgment sent back!");
      break;
    default:
      break;
    }
    delay(5000);
  }

  while (1) { } // block forever
}
