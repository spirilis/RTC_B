RTC\_B
=====

A deluxe Energia library for using the RTC\_B peripheral on TI's MSP430 Wolverine MCUs

# Usage
RTC\_B has a simple API for configuring and using the RTC onboard the TI MSP430 microcontrollers which feature it.
The RTC\_B hardware is described in detail in the MSP430FR58xx/59xx User's Guide - http://ti.com/lit/slau367

This RTC supports Calendar mode, with a single Alarm configurable to latch on Day of Week, Day of Month, Hour
or Minute.  Any combination of these criteria may be switched on to provide a flexible alarm system.

Additionally the RTC has 2 levels of periodic interrupt capability; RTCPS0 supports 1/128 to 1/16384-second
periodic IRQs, and RTCPS1 supports 2/second, 1/second and 1/2-1/64-second periodic IRQs.

This library supports *1/second* down to *1/16384-second* ticks using the attachPeriodicInterrupt() feature.

## Initialization & Configuration

Always use `rtc.begin()` or its long variant to initialize the RTC before doing anything else with it.
This is required to put the RTC in a known state.

`rtc.begin()`  
Initialize RTC to a known state, with all date & time counters set to 0 (January 1, 0000 at 00:00:00).
Upon exit, the RTC will begin counting.  All alarms and prescaler IRQs will be disabled & parameters cleared.
* __Arguments:__ None
* __Returns:__ Nothing

`rtc.begin(RTC_DOW dow, unsigned int month, unsigned int day, unsigned int year, unsigned int hour, unsigned int minute, unsigned int second)`  
Initialize RTC to a known state but with all date & time counters preinitialized to the specified arguments.
The __RTC_DOW__ argument is an enum which takes keywords MONDAY, TUESDAY, WEDNESDAY, etc.
* __Arguments:__ Day of Week, Month (1-12), Day (1-31), Year (0000-4095), Hour (0-23), Minute (0-59), Second (0-59)
* __Returns:__ Nothing

`rtc.end()`  
Disables the RTC (sets __RTCHOLD__) and shuts off all the interrupt handlers.
* __Arguments:__ None
* __Returns:__ Nothing

`rtc.debug(Stream *out)`  
Uses the Energia Stream interface (ala Serial.print, Serial.println, etc) to display a table of debugging information
about the current state of the RTC to the stream output device specified by __*out__.  This is often a pointer to Serial,
i.e.: _rtc.debug(&Serial)_
* __Arguments:__ Pointer to Stream object intended to receive the output
* __Returns:__ Nothing

`rtc.save()`  
Save the current date & time into a FRAM-backed buffer.  This can be restored later after a reset or power cycle with
the _rtc.restore()_ function.  Note this function reads from the RTC, which means it may introduce a busy-wait delay while
the *RTCCTL1* bit *RTCRDY* is cleared if the RTC was accessed during its asynchronous-update period.
* __Arguments:__ None
* __Returns:__ Nothing

`rtc.restore()`  
Restore the current date & time from the FRAM-backed buffer, if it was initialized previously.  There is a magic cookie byte
in that buffer which is checked; if it's not set properly, this function aborts and returns *false*.
* __Arguments:__ None
* __Returns:__ boolean (true/false) indicating whether the FRAM-backed buffer had valid data or not

`rtc.restoreFromBuffer(const uint8_t *buffer)`  
Restore the current date & time from the specified 8-byte buffer.
* __Arguments:__ Pointer to an 8-byte buffer of unsigned 8-bit integers in __Time Buffer__ format.
* __Returns:__ Nothing

## Setting time

Typically time will be set using the appropriate `.begin()` or `.restore()` methods, but if you need to change your time
metrics on the fly or prefer to do it separately from those, a full set of functions exist to do so.

`rtc.setDOW(RTC_DOW dayofweek)`  
Sets the Day of Week (MONDAY, TUESDAY, WEDNESDAY, etc)
* __Arguments:__ RTC\_DOW enum indicating day of week.
* __Returns:__ Nothing

`rtc.setMonth(unsigned int month)`  
Sets the month (1-12).
* __Arguments:__ Month (1-12).  Out of bounds input is clamped.
* __Returns:__ Nothing

`rtc.setDay(unsigned int day)`  
Sets the day (1-31).
* __Arguments:__ Day (1-31).  Out of bounds input is clamped.
* __Returns:__ Nothing

`rtc.setYear(unsigned int year)`  
Sets the year (0000-4095).
* __Arguments:__ Year (0000-4095). Out of bounds input is clamped.
* __Returns:__ Nothing

`rtc.setHour(unsigned int hour)`  
Sets the hour (0-23).
* __Arguments:__ Hour (0-23).  Out of bounds input is clamped.
* __Returns:__ Nothing

`rtc.setMinute(unsigned int minute)`  
Sets the minute (0-59).
* __Arguments:__ Minute (0-59).  Out of bounds input is clamped.
* __Returns:__ Nothing

`rtc.setSecond(unsigned int seconds)`  
Sets the seconds (0-59).
* __Arguments:__ Seconds (0-59).  Out of bounds input is clamped.
* __Returns:__ Nothing

## Getting time

You may extract the time information either in __Time Buffer__ format using `.getTime()`, or store a character string
representation using the `.getTimeString()` variations of functions.  The string format is tunable using the
`.setTimeStringFormat()` function.

`rtc.setTimeStringFormat(use_24hr, use_shortwords, day_before_month, short_date_notation, include_seconds)`  
Note- All arguments are in **boolean** format.  This function tunes the rules used to produce string-based time output
when using the `.getTimeString()` functions.
*use_24hr* denotes the use of military time (24-hour) vs. 12-hour AM/PM representation.
*use_shortwords* denotes the use of 3-letter words for day-of-week and month, e.g. Wed, Sat, Mar, Nov, Dec.
*day_before_month* denotes the printing of day before month, ala European-style (e.g. 30/6/2014 for June 30).
American style has this turned off (e.g. 6/30/2014).
*short_date_notation* denotes the use of slash-based numeric dates, i.e. "6/30/2014" instead of "Monday, June 30 2014"
*include_seconds* denotes the inclusion of the seconds field in the timefield, i.e. "5:30:59" instead of "5:30".
* __Arguments:__ Several boolean parameters as explained above
* __Returns:__ Nothing


