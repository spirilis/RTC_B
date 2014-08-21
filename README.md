RTC\_B
=====

A deluxe Energia library for using the RTC\_B peripheral on TI's MSP430 Wolverine MCUs

# Usage
RTC\_B has a simple API for configuring and using the RTC onboard the TI MSP430 microcontrollers which feature it.
The RTC\_B hardware is described in detail in the MSP430FR58xx/59xx User's Guide - http://ti.com/lit/slau367

This RTC supports Calendar mode, with a single Alarm configurable to latch on Day of Week, Day of Month, Hour
or Minute.  Any combination of these criteria may be switched on to provide a flexible alarm system.

Additionally the RTC has 2 levels of periodic interrupt capability; RTCPS0 supports 1/128 to 1/16384-second
periodic IRQs, and RTCPS1 supports every-2-seconds, 1/second and 1/2-1/64-second periodic IRQs.

This library supports *1/second* down to *1/16384-second* ticks using the attachPeriodicInterrupt() feature.

A single instance of the library's class is predefined for you; it is called `rtc`.

Some of the functions deal with a __Time Buffer__ format, which is a packed 8-byte binary sequence that can
store the date & time.  The exact format of this buffer is as follows:
* Bytes 0-1: Year, in Little-Endian 16-bit unsigned integer format
* Byte 2: Day of Week (0-6, 0 = MONDAY, 6 = SUNDAY)
* Byte 3: Month (1-12)
* Byte 4: Day (1-31)
* Byte 5: Hour (0-23)
* Byte 6: Minute (0-59)
* Byte 7: Second (0-59)

This format is useful for storing date/timestamp information in an in-memory or in-FRAM database, and you
can arbitrarily print 8-byte time buffers to text using the 2-argument variation of `.getTimeString()`.
Note this buffer should always start on an even-memory-address boundary, as the MSP430 will attempt to
cast the first 2 bytes as an unsigned integer, which won't work correctly if the buffer begins on an odd-numbered
memory address.

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
the **RTCCTL1** bit **RTCRDY** is cleared if the RTC was accessed during its asynchronous-update period.
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

Please note- All Time Getting functions may cause a busy-wait to be performed if the MCU attempts to read the RTC registers
during the short once-a-second period when **RTCCTL1**'s **RTCRDY** bit is cleared.  The function will pause until the **RTCRDY**
bit is re-set.

`rtc.setTimeStringFormat(use_24hr, use_shortwords, day_before_month, short_date_notation, include_seconds)`  
Note- All arguments are in **boolean** format.  This function tunes the rules used to produce string-based time output
when using the `.getTimeString()` functions.
* __Arguments:__ As follows-
* *use_24hr* denotes the use of military time (24-hour) vs. 12-hour AM/PM representation.
* *use_shortwords* denotes the use of 3-letter words for day-of-week and month, e.g. Wed, Sat, Mar, Nov, Dec.
* *day_before_month* denotes the printing of day before month, ala European-style (e.g. 30/6/2014 for June 30).
American style has this turned off (e.g. 6/30/2014).
* *short_date_notation* denotes the use of slash-based numeric dates, i.e. "6/30/2014" instead of "Monday, June 30 2014"
* *include_seconds* denotes the inclusion of the seconds field in the timefield, i.e. "5:30:59" instead of "5:30".
* __Returns:__ Nothing


`rtc.getTime(uint8_t *buf)`  
Pull the current RTC date & time info and store it in an 8-byte buffer located at _\*buf_.  This is stored in
_Time Buffer_ format.
* __Arguments:__ Pointer to unsigned 8-bit integer buffer containing at least 8 bytes
* __Returns:__ Length of data written (always returns 8).

`rtc.getTimeString(char *buf)`  
Pull the current RTC date & time info and store it in textual format, based on the rules tuned via
`.setTimeStringFormat()`, in the character buffer located at _\*buf_.
* __Arguments:__ Pointer to character buffer of suitable size
* __Returns:__ strlen(buf) after the function is done.

`rtc.getTimeString(char *buf, const uint8_t *timebuf)`  
A companion to `.getTimeString()` above, but allows you to generate a textual representation of any arbitrary
date/time spec in _Time Buffer_ format, for example timestamps logged in an in-FRAM or in-memory table or database.
* __Arguments:__ Pointer to character buffer, Pointer to an 8-byte unsigned 8-bit integer time buffer
* __Returns:__ strlen(buf) after the function is done.

`rtc.getDOW()`  
Return current day of week in *RTC_DOW* enum format.
* __Arguments:__ None
* __Returns:__ Day of week

`rtc.getMonth()`  
Return current month in unsigned integer format.
* __Arguments:__ None
* __Returns:__ Month (1-12)

`rtc.getDay()`  
Return current day in unsigned integer format.
* __Arguments:__ None
* __Returns:__ Day (1-31)

`rtc.getYear()`  
Return current year in unsigned integer format.
* __Arguments:__ None
* __Returns:__ Year (0000-4095)

`rtc.getHour()`  
Return current hour in unsigned integer format.
* __Arguments:__ None
* __Returns:__ Hour (0-23)

`rtc.getMinute()`  
Return current minute in unsigned integer format.
* __Arguments:__ None
* __Returns:__ Minute (0-59)

`rtc.getSecond()`  
Return current seconds field in unsigned integer format.
* __Arguments:__ None
* __Returns:__ Seconds (0-59)

## Interrupts

The RTC\_B peripheral allows two types of interrupts; A single scheduled-event alarm, and periodic ticks.

The *scheduled-event alarm* is an alarm which runs at a specified time based on an optional combination of Day-of-Week,
Day, Hour and Minute information.  Any one of these metrics may be used, and multiple metrics may be combined.  For example,
to set an alarm that kicks off at 9:00AM every Monday, setting the Day-of-Week metric to MONDAY and Hour metric to 9 would
enable this behavior.  But leaving the Day-of-Week metric cleared (as NO\_ALARM) would enable an alarm that kicks off
at 9:00AM every day of the week.

There are two types of periodic ticks; one that is capable of kicking off from as often as 64 times a second to as infrequent
as once a second (Actually, it can kick off once every 2 seconds, but for simplicity this library doesn't support that),
and one that kicks off as frequent as 16384 times a second to as infrequent as 128 times a second.

Both are configured with the same `.attachPeriodicInterrupt()` function, but the RTCPS IRQ used to service the interrupt
is chosen based on your specified divider.  Only one function may be registered to a particular RTCPS interrupt at a time;
any attempts to add more interrupt functions will fail (returning _false_).  Interrupt functions may be deactivated by
supplying the pointer to the user callback function to `.detachPeriodicInterrupt()`.

`rtc.attachScheduledInterrupt(int day, RTC_DOW dayofweek, int hour, int min, RTC_INTERRUPT_HANDLER userFunc)`  
This configures the Alarm interrupt.  Any options which are specified with -1 are disincluded in the alarm definition
(their associated **AE** bit, i.e. Alarm Enable bit, is cleared so the alarm subsystem doesn't include them in its
criteria).  The disinclusion keyword for dayofweek is NO\_ALARM.  The *userFunc* argument is a simple void func(void) callback.
* __Arguments:__ Date/time metrics as explained above along with a pointer to a user callback function.
* __Returns:__ True if no prior alarm was configured, false if an alarm was already configured.

`rtc.detachScheduledInterrupt()`  
Deactivate the Alarm, if one was set.
* __Arguments:__ None
* __Returns:__ True if an alarm was previously set (and is now deactivated), false if no alarm was set.

`rtc.attachPeriodicInterrupt(unsigned int divider, RTC_INTERRUPT_HANDLER userFunc)`  
This configures one of the RTCPS IRQs to kick off the *userFunc* callback every 1/**divider** seconds.
Take note up above that there are 2 available IRQs that may be configured here; attempting to register an
interrupt on a divider handled by the RTCPS IRQ handling an existing interrupt will result in this function
returning *false* and nothing happening.  Also note the divider is treated as a single-binary-digit number whose
value is equal to the highest bit# set.  For example, 16384 is interpreted as 16384, but 16383 is interpreted as 8192
since the highest bit set in the number 16383 is the 13th bit (8192).  Likewise, 129 is treated as 128, and 260 is treated
as 256.
* __Arguments:__ Period specification as divider applied to 1 second, plus the user callback function
* __Returns:__ True if no prior interrupt was configured for the requisite RTCPS IRQ, false if one was already configured for that RTCPS IRQ.

`rtc.detachPeriodicInterrupt(RTC_INTERRUPT_HANDLER userFunc)`  
Search the RTCPS IRQ handlers for a configured interrupt whose callback function is equal to *userFunc*.  If one is
found, deconfigure that RTCPS IRQ.  If not, return *false*.
* __Arguments:__ Pointer to user callback function
* __Returns:__ True if an RTCPS IRQ was found configured to be handled by the specified userFunc, false if not.

## Library Examples

The RTC\_B library includes several examples I put together in order to test and validate the functionality of the
library, as well as show off its features.

* __PrintOncePerSecond__ configures the RTC with `.begin()` to a zero time/datestamp and then attaches a periodic
interrupt that fires once a second to trigger a flag variable which signals loop() to print out a "Tick".
* __UseBothPeriodicInterrupts__ is similar to _PrintOncePerSecond_, but it runs a second periodic interrupt that kicks
off once every 1/256th of a second to print out a simple dot (".") in parallel with the periodic "Tick!" message.
* __AlarmOnceAnHour__ configures the RTC with `.begin()` to a zero time/datestamp and then adds an alarm that kicks off
1 minute after the hour; so about 1 minute into the sketch, the alarm should go off, and it should go off every hour
after that.
* __SaveAcrossResets__ tests the `.save()` and `.restore()` concept available on the TI MSP430 Wolverine FRAM chips.  This
periodically prints a tick (once a second), but also issues the `rtc.save()` during that tick.  Upon reset, the `setup()` function
attempts to use `rtc.restore()` to restore the date/timestamp information, instead setting it to a hardcoded set of values
if that fails (as it always will the first time the sketch is run after upload).
