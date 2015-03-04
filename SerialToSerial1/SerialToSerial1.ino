#include <Wire.h>
#include <GPRSbee.h>

#define PROMPT_MESSAGE "Enter AT Command or 'on' / 'off': "

#define NDOGO_PWRKEY_PIN 18
#define NDOGO_VBAT_PIN 23
#define NDOGO_STATUS_PIN 19

void setup() 
{
  //Start Serial ports
  Serial.begin(9600);
  Serial1.begin(9600);
  
  //Intialise the GPRSbee

  //Ndogo
  gprsbee.initNdogoSIM800(Serial1, NDOGO_PWRKEY_PIN, NDOGO_VBAT_PIN, NDOGO_STATUS_PIN);
   
  //Mbili
  //gprsbee.init(Serial1, BEECTS, BEEDTR);
  //gprsbee.setPowerSwitchedOnOff(true); 
  
  //Show prompt message
  Serial.print(PROMPT_MESSAGE);
}

void loop() 
{
}

void serialEvent()
{
  //Copy the buffer data
  String input = Serial.readString();

  //Remove any whitespace or CR/LF
  input.trim();
  
  //Handle on/off or AT command
  if (input == "off")
  {
    Serial.println(input);
    gprsbee.off();
  }
  else if (input == "on")
  {
    Serial.println(input);
    gprsbee.on();
  }
  else
    Serial1.println(input);
}

void serialEvent1()
{
  while (Serial1.available())
  {
    char c = Serial1.read();
    Serial.print((char)c);
  }
  Serial1.print("\r");
}
