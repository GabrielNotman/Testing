
#include <Wire.h>
#include <GPRSbee.h>

#define APN "internet"
#define NORMAL_PREFIX "http://"
#define SECURE_PREFIX "https://"
#define NORMAL_URL "www.google.com"
#define SECURE_URL "www.google.com"

#define ON_DELAY 8000
#define COMMAND_DELAY 5000 
#define GET_DELAY 20000

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

  //Setup debugging
  gprsbee.setDiag(Serial);
  
  //Run HTTP test
  HTTPGetTest();

  //Run HTTPS test
  HTTPSGetTest(); 
  
  //GPRSbee Library HTTP GET
  int buffSize = 1024;
  char buff[buffSize];
  gprsbee.doHTTPGET(APN, NORMAL_URL, buff, buffSize);
}

void loop() 
{
}

void ATCommand(String _command, int _waitDelay = COMMAND_DELAY)
{
  float delaySecs = (float)_waitDelay / 1000.0f;
  Serial.println("Sending AT Command, waiting " + String(delaySecs) + " seconds for a response");
  gprsbee.sendCommandWaitForOK(_command.c_str(), _waitDelay);
  delay(_waitDelay);
}

void HTTPGetTest()
{
  gprsbee.on(); 
  delay(ON_DELAY);
  
  //Initialise the GPRS connection
  ATCommand("ATE0");
  ATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  ATCommand("AT+SAPBR=3,1,\"APN\",\"" + String(APN) + "\"");
  ATCommand("AT+SAPBR=1,1"); 
  ATCommand("AT+SAPBR=2,1"); 
  
  //Try HTTPS GET
  ATCommand("AT+HTTPINIT");
  ATCommand("AT+HTTPPARA=\"CID\",1"); 
  ATCommand("AT+HTTPPARA=\"URL\",\"" + String(NORMAL_PREFIX) + String(NORMAL_URL) +"\""); 
  ATCommand("AT+HTTPPARA=\"REDIR\",1");
  ATCommand("AT+HTTPSSL=0");
  ATCommand("AT+HTTPSSL?");
  ATCommand("AT+HTTPACTION=0", GET_DELAY); 
  ATCommand("AT+HTTPREAD", GET_DELAY);
  
  //Terminate HTTP & GPRS and then switch off
  ATCommand("AT+HTTPTERM"); 
  ATCommand("AT+SAPBR=0,1"); 
  gprsbee.off();
}

void HTTPSGetTest()
{
  gprsbee.on(); 
  delay(ON_DELAY);
  
  //Initialise the GPRS connection
  ATCommand("ATE0");
  ATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  ATCommand("AT+SAPBR=3,1,\"APN\",\"" + String(APN) + "\"");
  ATCommand("AT+SAPBR=1,1"); 
  ATCommand("AT+SAPBR=2,1"); 
  
  //Try HTTPS GET
  ATCommand("AT+HTTPINIT");
  ATCommand("AT+HTTPPARA=\"CID\",1"); 
  ATCommand("AT+HTTPPARA=\"URL\",\"" + String(SECURE_PREFIX) + String(SECURE_URL) +"\""); 
  ATCommand("AT+HTTPPARA=\"REDIR\",1");
  ATCommand("AT+HTTPSSL=1");
  ATCommand("AT+HTTPSSL?");
  ATCommand("AT+HTTPACTION=0", GET_DELAY); 
  ATCommand("AT+HTTPREAD", GET_DELAY);
  
  //Terminate HTTP & GPRS and then switch off
  ATCommand("AT+HTTPTERM"); 
  ATCommand("AT+SAPBR=0,1"); 
  gprsbee.off();
}



