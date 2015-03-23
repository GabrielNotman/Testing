
#include <Wire.h>
#include <GPRSbee.h>

#define PROMPT_MESSAGE "Enter AT Command or 'on' / 'off': "

void setup() 
{
  //Start Serial ports
  Serial.begin(9600);
  Serial1.begin(9600);
  
  //Intialise the GPRSbee
  #ifdef ARDUINO_AVR_SODAQ_NDOGO
    gprsbee.initNdogoSIM800(Serial1, SIM800PWRKEY, SIM800VBAT, SIM800STATUS);
  #endif
  #ifdef ARDUINO_AVR_SODAQ_MBILI 
    gprsbee.init(Serial1, BEECTS, BEEDTR);
    gprsbee.setPowerSwitchedOnOff(true); 
  #endif

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
  
  //Uppercase for on/off
  String upperCase = input;
  upperCase.toUpperCase();
  
  //Handle on/off or AT command
  if (upperCase == "OFF")
    gprsbee.off();
  else if (upperCase == "ON")
    gprsbee.on();
  else
    //Send command string
    gprsbee.sendCommandWaitForOK(input.c_str());
  
  //Show prompt message again
  Serial.print(PROMPT_MESSAGE);
}
