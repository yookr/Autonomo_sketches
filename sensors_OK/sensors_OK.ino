/*
/* Grove - Light Sensor demo v1.0
* 
* signal wire to A0.
* By: http://www.seeedstudio.com
*/
#include <math.h>
const int ledPin=13;                 //Connect the LED Grove module to Pin13, Digital 13
const int thresholdvalue=10;         //The threshold for which the LED should turn on. 
float Rsensor;                       //Resistance of sensor in K
int tempValue;
float temperature;
int B=4275;                          //Grootte van de weerstand
const int R0 = 100000;               // R0 = 100k
float resistance;

void setup() {
  while(!SerialUSB);                 //Start the Serial connection
  // pinMode(ledPin,OUTPUT);         //Set the LED on Digital 12 as an OUTPUT
}

void loop() {
  int lightValue = analogRead(A2); 
  Rsensor=(float)(1023-lightValue)*10/lightValue;
  SerialUSB.print("Lichtsterkte : ");
  SerialUSB.println(lightValue);
  //SerialUSB.println(Rsensor,DEC);//show the light intensity on the serial monitor;

  int rotationValue = analogRead(A6);
  SerialUSB.print("Rotatie : ");
  SerialUSB.println(rotationValue);

  tempValue=analogRead(A10);
  float R = 1023.0/((float)tempValue)-1.0;
  R = 100000.0*R;
  float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15; //convert to temperature via datasheet ;
  temperature=temperature-2;                  // adjust temperature because of sensor mismatch
  SerialUSB.print("Temperatuur : ");
  SerialUSB.println(temperature);
  
  SerialUSB.println();
  delay(5000);
}
