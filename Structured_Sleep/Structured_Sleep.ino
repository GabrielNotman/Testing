#include <Wire.h>
#include <avr/sleep.h>

//SODAQ Mbili libraries
#include <RTCTimer.h>
#include <Sodaq_DS3231.h>
#include <Sodaq_PcInt.h>

//Baud rates for the serial port
#define PC_BAUD 57600

//The delay in seconds between the sensor readings 
//This should be a whole multiple (units!) of the RTC_INT_PERIOD
#define SENSOR_INTERVAL 60

//Minimum number of readings before uploading data
#define UPLOAD_READING_INTERVAL 1

//RTC Interrupt pin and period
#define RTC_PIN A7
#define RTC_INT_PERIOD EveryMinute

//ISR flags
volatile bool timer_flag = false;

size_t readingCount =0;

//RTC Timer
RTCTimer timer;

void setup() 
{
  //Initialise the serial connection
  Serial.begin(PC_BAUD);
  Serial.println("Power up...");
  
  //Initialise sensors
  setupSensors();
 
  //Setup timer events
  setupTimer();
  
  //Setup communications module
  setupCommunication();
  
  //Setup sleep mode
  setupSleep();
}

void loop() 
{
  if (timer_flag) {
    //Clear this first in case the main code takes > 1 min
    timer_flag = false;
    rtc.clearINTStatus();
    
    //Update the timer 
    timer.update();
  }
  
  //Sleep
  systemSleep();  
}

void setupSleep()
{
  pinMode(RTC_PIN, INPUT_PULLUP);
  PcInt::attachInterrupt(RTC_PIN, wakeISR);
  
  //Schedule the wakeup interrupt for every minute
  rtc.enableInterrupts(RTC_INT_PERIOD);
  rtc.clearINTStatus();
  
  //Set the sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void wakeISR()
{
  //Check if the trigger was FALLING
  if (!digitalRead(RTC_PIN)) {
    timer_flag = true;
  }
}

void systemSleep()
{
  //Wait until the serial ports have finished transmitting
  Serial.flush();
  Serial1.flush();

  Serial.end();
  
  //Disable ADC
  ADCSRA &= ~_BV(ADEN);
  
  //Sleep time
  noInterrupts();
  
  if (!timer_flag) {
    sleep_enable();
    interrupts();
    sleep_cpu();
  
    //Zzzz Zzzz
    
    sleep_disable();
  }
  interrupts();
 
  //Enbale ADC
  ADCSRA |= _BV(ADEN);

  Serial.begin(PC_BAUD);
}

void takeReading(uint32_t ts)
{
  //Add the code required for reading your sensors
  
  readingCount++;
  if (readingCount >= UPLOAD_READING_INTERVAL) {
    if (uploadData()) {
      readingCount = 0;
    }
  }
}

void setupSensors()
{
  //Initialise the wire protocol for the TPH sensors
  Wire.begin();
  
  //Initialise the DS3231 RTC
  rtc.begin();
  
  //Add the code required for initialising your sensors
}

void setupTimer()
{
  //Instruct the RTCTimer how to get the current time reading
  timer.setNowCallback(getNow);

  //Schedule the reading interval
  timer.every(SENSOR_INTERVAL, takeReading);
}

void setupCommunication()
{
  //Add the code required for setting up your communication device
}

bool uploadData()
{
  Serial.println("Uploading data...");
  
  bool result = false;

  //Add code for uploading data
  //Set result to true if successful  
  
  if (result) {
    Serial.println("Upload succeeded...");    
  } else {
    Serial.println("Upload failed...");
  }
  return result;
}

uint32_t getNow()
{
  return rtc.now().getEpoch();
}

