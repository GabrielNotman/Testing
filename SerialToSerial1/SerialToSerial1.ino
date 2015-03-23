#include <Wire.h>
#include <GPRSbee.h>

#define PROMPT_MESSAGE "Enter AT Command or 'on' / 'off': "

void setup() 
{
  //Start Serial ports
  Serial.begin(57600);
  Serial1.begin(57600);
  
  //Intialise the GPRSbee
  #ifdef ARDUINO_AVR_SODAQ_NDOGO
    gprsbee.initNdogoSIM800(Serial1, SIM800PWRKEY, SIM800VBAT, SIM800STATUS);
  #endif
  #ifdef ARDUINO_AVR_SODAQ_MBILI 
    gprsbee.init(Serial1, BEECTS, BEEDTR);
    gprsbee.setPowerSwitchedOnOff(true); 
  #endif 
  
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
  
  //Uppercase for on/off
  String upperCase = input;
  upperCase.toUpperCase();
  
  //Handle on/off or AT command
  if (upperCase == "OFF")
  {
    Serial.println(input);
    gprsbee.off();
  }
  else if (upperCase == "ON")
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
