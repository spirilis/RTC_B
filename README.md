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

