#include <Wire.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

//SODAQ Mbili libraries
#include <RTCTimer.h>
#include <Sodaq_DS3231.h>
#include <Sodaq_PcInt.h>

//The delay between the sensor readings 
#define READ_DELAY 1

//RTC Timer
RTCTimer timer;

//RTC Interrupt pin
#define RTC_PIN A7
#define RTC_INT_PERIOD EveryMinute

void setup() 
{
  //Initialise the serial connection
  Serial.begin(9600);
  
  //Setup timer events
  setupTimer();
  
  //Setup sleep mode
  setupSleep();
  
  //Make first call
  Serial.println("Power On");
  showTime(getNow());
}

void loop() 
{
  //Update the timer 
  timer.update();
  
  //Sleep
  systemSleep();
}

void showTime(uint32_t ts)
{
  //Retrieve and display the current date/time
  String dateTime = getDateTime();
  Serial.println(dateTime);
}

void setupTimer()
{
  //Instruct the RTCTimer how to get the current time reading
  timer.setNowCallback(getNow);

  //Schedule the reading every second
  timer.every(READ_DELAY, showTime);
}

void wakeISR()
{
  //Leave this blank
}

void setupSleep()
{
  pinMode(RTC_PIN, INPUT_PULLUP);
  PcInt::attachInterrupt(RTC_PIN, wakeISR);

  //Setup the RTC in interrupt mode
  rtc.begin();
  rtc.enableInterrupts(RTC_INT_PERIOD);
  
  //Set the sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void systemSleep()
{
  //This method handles any sensor specific sleep setup
  sensorsSleep();
  
  //Wait until the serial ports have finished transmitting
  Serial.flush();
  Serial1.flush();
  
  //The next timed interrupt will not be sent until this is cleared
  rtc.clearINTStatus();
    
  //Disable ADC
  ADCSRA &= ~_BV(ADEN);
  
  //Sleep time
  noInterrupts();
  sleep_enable();
  interrupts();
  sleep_cpu();
  sleep_disable();
 
  //Enbale ADC
  ADCSRA |= _BV(ADEN);
  
  //This method handles any sensor specific wake setup
  sensorsWake();
}

void sensorsSleep()
{
  //Add any code which your sensors require before sleep
}

void sensorsWake()
{
  //Add any code which your sensors require after waking
}

String getDateTime()
{
  String dateTimeStr;
  
  //Create a DateTime object from the current time
  DateTime dt(rtc.makeDateTime(rtc.now().getEpoch()));

  //Convert it to a String
  dt.addToString(dateTimeStr);
  
  return dateTimeStr;  
}

uint32_t getNow()
{
  return rtc.now().getEpoch();
}
