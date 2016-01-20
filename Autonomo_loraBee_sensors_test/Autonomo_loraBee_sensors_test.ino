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

#define lightSensorPort A2
#define rotationSensorPort A6
#define tempSensorPort A10
const int debug=0;                   // Set to 1 for debug output to serial console
const int payloadLength=18;
const int sleepTime=600000;           // In normal mode send values every 10 minutes
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

// Read temperature 10 times and calculate the average
float senseTemperature()
{
  float ttemp=0;
  for (int i=0; i<10; i++) {
    tempValue=analogRead(tempSensorPort);
    float R = 1023.0/((float)tempValue)-1.0;
    R = 100000.0*R;
    float temperatureSensor=1.0/(log(R/100000.0)/B+1/298.15)-273.15; //convert to temperature via datasheet ;
    temperatureSensor=temperatureSensor-2;                  // adjust temperature because of sensor mismatch
    if (debug == 1) {
      debugSerial.print(i);
      debugSerial.print(" ");
      debugSerial.print("Temperatuur : ");
      debugSerial.println(temperatureSensor);
    }
    ttemp=ttemp+temperatureSensor;
  }
  temperature=ttemp/10;
  if (debug == 1) {
    debugSerial.print("Temperatuur : ");
    debugSerial.println(temperature);
  }
  return temperature;  
}

// Read lightsensor 10 times and calculate the average
int senseLight()
{
  int tlight=0;
  for (int i=0; i<10; i++) {
    int lightSensor = analogRead(lightSensorPort); 
    Rsensor=(float)(1023-lightSensor)*10/lightSensor;
    if (debug == 1) {
      debugSerial.print(i);
      debugSerial.print(" ");
      debugSerial.print("Lichtsterkte : ");
      debugSerial.println(lightSensor);
    }
    tlight=tlight+lightSensor;
  }
  lightValue=tlight/10;
  if (debug == 1) {
    debugSerial.print("Lichtsterkte : ");
    debugSerial.println(lightValue);
  }
  return lightValue;  
}

// Read rotationsensor 10 times and calculate the average
int senseRotation()
{
  int trotation=0;
  for (int i=0; i<10; i++) {
    int rotationSensor = analogRead(rotationSensorPort); 
    if (debug == 1) {
      debugSerial.print(i);
      debugSerial.print(" ");
      debugSerial.print("Rotatie : ");
      debugSerial.println(rotationSensor);
    }
    trotation=trotation+rotationSensor;
  }
  rotationValue=trotation/10;
  if (debug == 1) {
    debugSerial.print("Rotatie : ");
    debugSerial.println(rotationValue);
  }
  return rotationValue;  
}
void setup()
{
  digitalWrite(BEE_VCC, HIGH);
  // debugSerial.begin(57600);       // This is for the Sodaq Mbili
  while(!SerialUSB);                 // Start the Serial connection Sodaq Autonomo
  debugSerial.println("Starting");
  pinMode(lightSensorPort, INPUT);
  pinMode(rotationSensorPort, INPUT);
  pinMode(tempSensorPort, INPUT);
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

  debugSerial.println("Sleeping for 5 seconds before starting sending out packets.");
  for (uint8_t i = 5; i > 0; i--)
  {
    if (debug == 1) debugSerial.println(i);
    delay(1000);
  }
}

void loop()
{
  debugSerial.println();  
  debugSerial.println("Sensor values");

  lightValue=senseLight();
  debugSerial.print("Light intensity : ");
  debugSerial.println(lightValue);  

  rotationValue=senseRotation();
  debugSerial.print("Rotation        : ");
  debugSerial.println(rotationValue);

  temperature=senseTemperature();
  debugSerial.print("Temperature     : ");
  debugSerial.println(temperature);  

  debugSerial.println();  

  static char outstr[15];

  int temperature100 = temperature*100;

  char stringToSend[40];
  
  if (debug == 1){
    sprintf(stringToSend, "Light = %d, Rotation = %d, Temperature*100 = %d", lightValue, rotationValue, temperature100);
  } 
  else {
    sprintf(stringToSend, "L:%d,R:%d,T:%d", lightValue, rotationValue, temperature100);
  }
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
    //      1 1            ?? geen idee wat deze betekent
    //      2 testPayload  De payload in Hex die verzonden wordt
    //      3 8            Lengte van de payload
    //      4 3            ?? geen idee wat deze betekent
    switch (LoRaBee.sendReqAck(1, payload, payloadLength, 3))
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
    delay(sleepTimeDebug);
  } else {
    delay(sleepTime);
  }
}
