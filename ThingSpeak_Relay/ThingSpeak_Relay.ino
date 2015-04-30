#include <Wire.h>
#include <SPI.h>

//SODAQ libraries
#include <RTCTimer.h>
#include <GPRSbee.h>

//The delay between the sensor readings, milliseconds
#define REFRESH_DELAY 120000

//Baud rate for Serial/Serial1
#define BAUD_RATE 57600

//Uncomment this if you want to debug the GPRSbee
//#define DEBUG_GPRSBEE

//GPRSbee constants
#define TIME_OUT 6000
#define MAX_ATTEMPTS 5
#define GPRSBEE_BUFFER 256

//Network constants
#define APN "internet"
#define APN_USERNAME ""
#define APN_PASSWORD ""

//SpeakThings constants
#define SERVER "api.thingspeak.com"
#define PORT 80
#define CHANNEL_ID "XXXXX" //Change to your channel's ID
#define WRITE_API_KEY "XXXXXXXXXXXXXXXX" //Change to your channel's write key
#define READ_API_KEY "XXXXXXXXXXXXXXXX" //Change to your channel's read key

//Data labels used on the channel for command and status
#define COMMAND_LABEL "field1"
#define STATUS_LABEL "field2"

//Logic we should avoid using '0' as this can mean error from ThingSpeak
#define OFF_SIGNAL 5
#define ON_SIGNAL 10

//Other constants
#define RELAY_PIN 10 //Use D10 for controlling the relay
#define NEW_LINE "\r\n"

//RTC Timer
RTCTimer timer;

void setup() 
{
  //Initialise the serial connection
  Serial.begin(BAUD_RATE);
  
  //Initialise sensors
  setupComponents();
  
  //Setup GPRSbee
  setupComms();
    
  //Setup timer events
  setupTimer();
}

void loop() 
{
  //Update the timer 
  timer.update();
}

void updateComponents(uint32_t ts)
{
  Serial.println("Updating...");
  
  //Read the command from ThingSpeak
  readCommand();
    
  //Send the status of the
  updateStatus();
}

void readCommand()
{     
  //Construct the TCP HTTP request for the command
  String request = "GET /channels/" + String(CHANNEL_ID) + "/field/" + String(COMMAND_LABEL)
    + "/last?key=" + String(READ_API_KEY) + NEW_LINE + NEW_LINE;
  Serial.println("Checking command status...");
 
  //Send the TCP HTTP request for the command 
  sendHTTPRequest(APN, APN_USERNAME, APN_PASSWORD, SERVER, PORT, request.c_str(), request.length(), MAX_ATTEMPTS);
  
  //The result should be a single number, 0 if there is an error
  int32_t command = parseHTTPResponseLong(TIME_OUT);
  gprsbee.closeTCP();
  
  //Update the relay based on the command
  if (command == OFF_SIGNAL)
  {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Received: LOW");
  }
  else if (command == ON_SIGNAL)
  {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Received: HIGH");
  }
  else
  {
    Serial.println("Received: Unknown");
  }
}

void updateStatus()
{  
  //Get the current state of the relay
  uint32_t status = digitalRead(RELAY_PIN)==LOW ? OFF_SIGNAL : ON_SIGNAL;
  
  //Construct the TCP HTTP request for the status
  String request = "GET /update?key=" + String(WRITE_API_KEY) + "&" + String(STATUS_LABEL) + "=" 
    + String(status) + NEW_LINE + NEW_LINE;
  Serial.println("Updating relay status...");
    
  //Send TCP HTTP request for status
  sendHTTPRequest(APN, APN_USERNAME, APN_PASSWORD, SERVER, PORT, request.c_str(), request.length(), MAX_ATTEMPTS);
  
  //The result should be a single number, 0 if there is an error
  int32_t result = parseHTTPResponseLong(TIME_OUT);
  
  if (result==0)
    Serial.println("Status update failed");
  else
    Serial.println("Status updated");
  Serial.println();
  
  //Close the TCP connection
  gprsbee.closeTCP();
}

void setupComponents()
{
  //Setup the relay, start with it off
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
}

void setupTimer()
{
  //Instruct the RTCTimer how to get the current time reading
  timer.setNowCallback(getNow);
  
  //Schedule the reading every second
  timer.every(REFRESH_DELAY, updateComponents);
  
  //Call the method for the first time
  updateComponents(getNow());
}

void setupComms()
{
  //Start Serial1 the Bee port
  Serial1.begin(BAUD_RATE);
  
  //Intialise the GPRSbee
  #ifdef ARDUINO_AVR_SODAQ_NDOGO
    gprsbee.initNdogoSIM800(Serial1, SIM800PWRKEY, SIM800VBAT, SIM800STATUS, GPRSBEE_BUFFER);
    gprsbee.setSkipCGATT(true);
  #endif
  #ifdef ARDUINO_AVR_SODAQ_MBILI 
    gprsbee.init(Serial1, BEECTS, BEEDTR, GPRSBEE_BUFFER);
    gprsbee.setPowerSwitchedOnOff(true); 
  #endif

  #ifdef DEBUG_GPRSBEE
    gprsbee.setDiag(Serial);
  #endif
}

uint32_t getNow()
{
  return millis();
}

bool sendHTTPRequest(const char *_apn, const char *_apnuser, const char *_apnpwd, 
    const char *_server, int _port, const char* _request, const uint16_t _length,
    const uint8_t _maxAttempts)
{
  //Open connection
  bool connection = false;
  bool sent = false;

  uint8_t count = 0;
  while ((!sent) && (count < _maxAttempts)) 
  {
    count++;
    connection = gprsbee.openTCP(_apn, _apnuser, _apnpwd, _server, _port);

    //Try and send the data if the connection was opened
    if (connection) 
    {
      sent = gprsbee.sendDataTCP((uint8_t*) _request, _length);
      
      //If sending failed close the connection and start from the top
      if (!sent) 
        gprsbee.closeTCP();
    } 
    else 
       gprsbee.closeTCP();
  }

  return sent;
}

int32_t parseHTTPResponseLong(uint16_t _timeout)
{
  const char* response = NULL;
  
  //Read the first line of the response
  gprsbee.receiveLineTCP(&response, _timeout);
  
  //If nothing has been read
  if ((response == NULL) || (strlen(response) == 0))
    return 0;
  
  //Return the long integer conversion "0" if failed
  return atol(response);  
}
