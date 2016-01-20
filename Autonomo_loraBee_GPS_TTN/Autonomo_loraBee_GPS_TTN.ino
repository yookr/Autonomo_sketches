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

#define rxGPS 0
#define txGPS 1
//SoftwareSerial serialGPS = SoftwareSerial(rxGPS, txGPS);
String stringGPS = "";

const int debug=0;                   // Set to 1 for debug output to serial console
const int payloadLength=23;
const int sleepTime=60000;           // In normal mode send values every 10 minutes
const int sleepTimeDebug=30000;       // In debug mode send values every 30 seconds

#include <math.h>
const int ledPin=13;                 // Connect the LED Grove module to Pin13, Digital 13
const int thresholdvalue=10;         // The threshold for which the LED should turn on. 
float Rsensor;                       // Resistance of sensor in K
int tempValue;
int lightValue;
int rotationValue;
float temperature;
int B=4275;                          // Grootte van de weerstand
const int R0 = 100000;               // R0 = 100k
float resistance;

const uint8_t devAddr[4] =
{
  0x02, 0x01, 0x36, 0x04
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

// Check GPS and returns string if full line recorded, else false
String checkGPS()
{
  if (Serial.available())
  {
    char c = Serial.read();
    if (debug == 1) 
    {
      debugSerial.print("Value read is: ");
      debugSerial.println(c);
    }
  
    if (c != '\n' && c != '\r')
    {
      stringGPS  = stringGPS + c;
      if (debug ==1) debugSerial.println(stringGPS);
    }
    else
    {
      if (stringGPS != "")
      {
        String tmp = stringGPS;
        stringGPS = "";
        return tmp;
      }
    }
  }
  if (debug == 1) debugSerial.println("Serial port not available!");
  return "false";
} // end checkGPS


void setup()
{
  digitalWrite(BEE_VCC, HIGH);
  // debugSerial.begin(57600);          // This is for the Sodaq Mbili
  if (debug == 1 ) 
  {
    while(!SerialUSB);   // Start the Serial connection Sodaq Autonomo only when on USB
    debugSerial.println("Starting");
  }

  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  LoRaBee.setDiag(debugSerial); // optional
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true))
  {
    if (debug == 1) debugSerial.println("Network settings succesfully written in the bee module.");
  }
  else
  {
    if (debug == 1) debugSerial.println("Error writing network setting in the bee module!");
  }

  if (debug == 1) debugSerial.println("Sleeping for 5 seconds before starting sending out packets.");
  for (uint8_t i = 5; i > 0; i--)
  {
    if (debug == 1) debugSerial.println(i);
    delay(1000);
  }

  // GPS Setup
  Serial.begin(9600);
  digitalWrite(txGPS,HIGH);
  if (debug ==1) SerialUSB.println("Setting transmit port to HIGH");

  // Cut first gibberish
  while(Serial.available())
  {
    if (debug == 1) SerialUSB.println("Read first line as dummy");
    if (Serial.read() == '\r')
      break;
  }
  if (debug == 1) SerialUSB.println("First dummy line deleted");
} // end setup

void loop()
{
  String s = checkGPS();
  String north = s.substring(18,29);
  String east = s.substring(30,42);
  //if (s != "false")
  //  SerialUSB.print(s);
  if(s && s.substring(0, 6) == "$GPGGA")
  {
    //SerialUSB.print("GPS string is: ");
    //SerialUSB.println(s);
    if (debug == 1)
    {
      SerialUSB.println(s);
      SerialUSB.print("North: ");
      SerialUSB.println(north);
      SerialUSB.print("East:  ");
      SerialUSB.println(east);
    }

    if (debug ==1 ) debugSerial.println();  

    static char outstr[15];

    String stringToSend;
  
    stringToSend = north + east;
    debugSerial.print("Sending: ");
    debugSerial.println(stringToSend);


    // send 10 packets, with at least a 5 seconds delay after each transmission (more seconds if the device is busy)
    uint8_t payload[20];
    uint8_t j = payloadLength+1;
    while (j > 0)
    {
      payload[j-1] = stringToSend[j-1], HEX;
      j--;
    }
  
    uint8_t i = 1;
    while (i > 0)
    {

      // original: switch (LoRaBee.sendReqAck(1, testPayload, 8, 3))
      // parameters:
      //      1 1            Application identifier
      //      2 testPayload  De payload in Hex die verzonden wordt
      //      3 8            Lengte van de payload
      //      4 3            Number of acknowledgments if sendReqAck is used, not used with send
      // switch (LoRaBee.sendReqAck(1, payload, payloadLength, 3))  // For use with Kerlink SPN
      // switch (LoRaBee.send(1, payload, payloadLength))           // For use with TTN network
      switch (LoRaBee.send(1, payload, payloadLength))
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
      delay(5000);  // 5 sec waiting before retry if transmission was unsuccessful
    }
  
    if (debug == 1){
      debugSerial.print("Sleeping for ");
      debugSerial.print(sleepTimeDebug/1000);
      debugSerial.println(" seconds.");
      delay(sleepTimeDebug);
    } else {
      delay(sleepTime);
    }
  }
} // end loop
