
//
//    FILE: dht11_test.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.01
// PURPOSE: DHT library test sketch for DHT11 && Arduino
//     URL:
//
// Released to the public domain
//

#include <dht.h>

dht DHT;

#define DHT11_PIN 5

void setup()
{
  while(!SerialUSB);
  SerialUSB.println("DHT TEST PROGRAM ");
  SerialUSB.print("LIBRARY VERSION: ");
  SerialUSB.println(DHT_LIB_VERSION);
  SerialUSB.println();
  SerialUSB.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}

void loop()
{
  // READ DATA
  SerialUSB.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
		SerialUSB.print("OK,\t"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		SerialUSB.print("Checksum error,\t"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		SerialUSB.print("Time out error,\t"); 
		break;
    case DHTLIB_ERROR_CONNECT:
        SerialUSB.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        SerialUSB.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        SerialUSB.print("Ack High error,\t");
        break;
    default: 
		SerialUSB.print("Unknown error,\t"); 
		break;
  }
  // DISPLAY DATA
  SerialUSB.print(DHT.humidity, 1);
  SerialUSB.print(",\t");
  SerialUSB.println(DHT.temperature, 1);

  delay(2000);
}
//
// END OF FILE
//
