#include <Wire.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

//SODAQ Mbili libraries
#include <RTCTimer.h>
#include <Sodaq_DS3231.h>

//The delay between the sensor readings 
#define READ_DELAY 1

//RTC Timer
RTCTimer timer;

//WD flag
volatile bool hz_flag = false;

void setup() 
{
  //Initialise the serial connection
  Serial.begin(9600);
  
  //Setup timer events
  setupTimer();
  
  //Setup watchdog timer
  setupWDT();
  
  //Make first call
  Serial.println("Power On");
  showTime(0);
}

void loop() 
{
  if (hz_flag)
  {
    wdt_reset();
    WDTCSR |= _BV(WDIE);
    hz_flag = false;
    
    //Update the timer 
    timer.update();
  }
  
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

void setupWDT()
{
  MCUSR &= ~(1<<WDRF);
  WDTCSR |= _BV(WDCE) | _BV(WDE);
  WDTCSR = _BV(WDE) | _BV(WDIE) | _BV(WDP2) | _BV(WDP1);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  wdt_reset();
}

ISR (WDT_vect) 
{
  hz_flag = true;
} 

void systemSleep()
{
  //Wait until the serial ports have finished transmitting
  Serial.flush();
  Serial1.flush();
  
  //Disable ADC
  ADCSRA &= ~_BV(ADEN);
  
  //Only go back to sleep if the WD has not fired since last sleep
  noInterrupts();
  if (!hz_flag)
  {
    sleep_enable();
    interrupts();
    sleep_cpu();
    sleep_disable();
  }
  interrupts();  
  
  //Enbale ADC
  ADCSRA |= _BV(ADEN);
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
