//#include <SoftwareSerial.h>

// GPS Setup
#define rxGPS 0
#define txGPS 1
//SoftwareSerial serialGPS = SoftwareSerial(rxGPS, txGPS);
String stringGPS = "";

void setup() {
  pinMode(rxGPS, INPUT);
  pinMode(txGPS, OUTPUT);


  while(!SerialUSB);                 // Start the Serial connection Sodaq Autonomo
  SerialUSB.println("Starting");

  // GPS Setup
  Serial.begin(9600);
  digitalWrite(txGPS,HIGH);
  SerialUSB.println("Setting transmit port to HIGH");

  // Cut first gibberish
  while(Serial.available())
    {
      SerialUSB.println("Read first line as dummy");
      if (Serial.read() == '\r')
        break;
    }
  SerialUSB.println("First dummy line deleted");
}

void loop()
{
  String s = checkGPS();
  //if (s != "false")
  //  SerialUSB.print(s);
  if(s && s.substring(0, 6) == "$GPGGA")
  {
    //SerialUSB.print("GPS string is: ");
    //SerialUSB.println(s);
    String north = s.substring(18,29);
    String east = s.substring(30,42);
    SerialUSB.println(s);
    SerialUSB.print("North: ");
    SerialUSB.println(north);
    SerialUSB.print("East:  ");
    SerialUSB.println(east);
  }
}

// Check GPS and returns string if full line recorded, else false
String checkGPS()
{
  if (Serial.available())
  {
    char c = Serial.read();
    //SerialUSB.print("Value read is: ");
    //SerialUSB.print(c);
    if (c != '\n' && c != '\r')
    {
      stringGPS  = stringGPS + c;
      //SerialUSB.println(stringGPS);
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
  return "false";
}
