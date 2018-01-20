#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <EEPROM.h>
#include <math.h>
#include "RTClib.h"

#define FINAL_EPOCH_TIME_SECONDS        3471314400    // this is the expected epoch time of your life expectancy

// Hardware Consts
RTC_PCF8523 rtc;
#define eeprom_addr 0
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Globals
long eepromValueCache = 0;    // EEPROM reads can be slow, so use a local memory cache.

// Setup
void setup() 
{
  Serial.begin(115200);
  Serial.println("Hello World");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
  }

  // following line sets the RTC to the date & time this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  lcd.begin(16, 2);

  //pinMode(A3, OUTPUT);
  //digitalWrite(A3, HIGH);
  //pinMode(A2, OUTPUT);
  //digitalWrite(A2, LOW);
}

// Main Function
void loop()
{
  DateTime now = rtc.now();
  Serial.println(now.unixtime());
  long current_epoch_time = now.unixtime();
  long diff = FINAL_EPOCH_TIME_SECONDS - current_epoch_time;
  Serial.println(diff);

  printToLcd(0, formatLong(years(diff), 5) + " years");
  printToLcd(1, printFormat(days(diff), hours(diff), minutes(diff), seconds(diff)));
  
  delay(1000);
}


// LCD Helpers
void printToLcd(int row, String s)
{
  lcd.setCursor(0, row);
  lcd.print( s + "  " );
}

void clearLcd()
{   // turn it off & on again
    lcd.noDisplay();
    lcd.display();
}

// EEPROM Helpers
long EepromGet()
{
  if ( eepromValueCache > 0 )   { return eepromValueCache; }
  
  long t;
  EEPROM.get( eeprom_addr, t );
  eepromValueCache = t;
  return t;
}

void EepromPut(long t)
{ // ATmega's EEPROM has a lifetime of 100k write/erase cycles. Try to use this sparingly.
  EEPROM.put( eeprom_addr, t );
  eepromValueCache = t;
}

// Time Helpers
long seconds(long epoch_seconds)
{
  return epoch_seconds % 60;
}

long minutes(long epoch_seconds)
{
  return (epoch_seconds / 60) % 60;
}

long hours(long epoch_seconds)
{
  return (epoch_seconds / 60 / 60) % 24;
}

long days(long epoch_seconds)
{
  return(epoch_seconds / 60 / 60 / 24) % 365;
}

long years(long epoch_seconds)
{
  return epoch_seconds / 60 / 60 / 24 / 365;
}



// Print Format Helpers

String printFormat(long days, long hours, long minutes, long seconds)
{
  return formatLong(days, 3) + "d "
          + formatLong(hours, 2) + ":"
          + formatLong(minutes, 2) + ":"
          + formatLong(seconds, 2);
}

String formatLong(long n, int digitLen)     { return reduceFormatLong("", n, digitLen); }

String reduceFormatLong(String acc, long n, int digitLen)
{
  if (digitLen > 1)
  {
    long upperBound = pow(10, (digitLen - 1));
    if (n < upperBound) {
      return reduceFormatLong("0" + acc, n, (digitLen - 1));
    }
  }
  return acc + String(n);
}




