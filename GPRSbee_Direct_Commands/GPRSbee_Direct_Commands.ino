
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
  //gprsbee.initNdogoSIM800(Serial1, NDOGO_PWRKEY_PIN, NDOGO_VBAT_PIN, NDOGO_STATUS_PIN);
   
  //Mbili
  gprsbee.init(Serial1, BEECTS, BEEDTR);
  gprsbee.setPowerSwitchedOnOff(true); 

  //Setup debugging
  gprsbee.setDiag(Serial);
  
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
  
  //Echo the input command
  Serial.println(input);
  
  //Handle on/off or AT command
  if (input == "off")
    gprsbee.off();
  else if (input == "on")
    gprsbee.on();
  else
    //Send command string
    gprsbee.sendCommandWaitForOK(input.c_str());
  
  //Show prompt message again
  Serial.print(PROMPT_MESSAGE);
}
