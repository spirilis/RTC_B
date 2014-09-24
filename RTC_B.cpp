/* RTC_B library for the MSP430 RTC_B peripheral found on
 * MSP430FR58xx/59xx MCUs.
 *
 * This library was written for the Energia Project.
 *
 * Copyright (c) 2014, Eric Brundick <spirilis@linux.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <Energia.h>
#include "RTC_B.h"
#include <string.h>

volatile RTC_INTERRUPT_HANDLER _rtcb_alarmHandler;
volatile RTC_INTERRUPT_HANDLER _rtcb_prescaleHandlers[2];

RTC_B::RTC_B()
{
    _rtcb_prescaleHandlers[0] = NULL;
    _rtcb_prescaleHandlers[1] = NULL;
    _rtcb_alarmHandler = NULL;
    string_fmt_bits = 0x0000;
}

void RTC_B::_rtcrdy_semaphore()
{
    while (!(RTCCTL1 & RTCRDY_H))
        ;
}

boolean RTC_B::begin()
{
    #ifdef __MSP430_HAS_CS__
    if (CSCTL5 & LFXTOFFG)
        return false;
    #endif
    RTCCTL0 = 0x00;
    RTCCTL1 = RTCHOLD_H;
    RTCCTL2 = 0x00;
    RTCCTL3 = 0x00;
    RTCSEC = 0;
    RTCMIN = 0;
    RTCHOUR = 0;
    RTCDOW = 0;
    RTCDAY = 0;
    RTCMON = 0;
    RTCYEAR = 0;
    RTCAMIN = 0;
    RTCAHOUR = 0;
    RTCADOW = 0;
    RTCADAY = 0;
    RTCPS0CTL = RT0IP__256;
    RTCPS1CTL = RT1IP__256;
    RTCPS = 0;
    RTCCTL1 &= ~RTCHOLD_H;
    return true;
}

boolean RTC_B::begin(RTC_DOW dow, unsigned int month, unsigned int day,
                  unsigned int year, unsigned int hour, unsigned int minute,
                  unsigned int second)
{
    #ifdef __MSP430_HAS_CS__
    if (CSCTL5 & LFXTOFFG)
        return false;
    #endif
    RTCCTL0 = 0x00;
    RTCCTL1 = RTCHOLD_H;
    RTCCTL2 = 0x00;
    RTCCTL3 = 0x00;
    setSecond(second);
    setMinute(minute);
    setHour(hour);
    setDOW(dow);
    setDay(day);
    setMonth(month);
    setYear(year);
    RTCAMIN = 0;
    RTCAHOUR = 0;
    RTCADOW = 0;
    RTCADAY = 0;
    RTCPS0CTL = RT0IP__256;
    RTCPS1CTL = RT1IP__256;
    RTCPS = 0;
    RTCCTL1 &= ~RTCHOLD_H;
    return true;
}

void RTC_B::setDOW(RTC_DOW dow)
{
    if (dow < MONDAY) dow = MONDAY;
    if (dow > SUNDAY) dow = SUNDAY;
    RTCDOW = dow;
}

void RTC_B::setMonth(unsigned int month)
{
    if (month < 1) month = 1;
    if (month > 12) month = 12;
    RTCMON = month;
}

void RTC_B::setDay(unsigned int day)
{
    if (day < 1) day = 1;
    if (day > 31) day = 31;
    RTCDAY = day;
}

void RTC_B::setYear(unsigned int year)
{
    if (year > 4095) year = 4095;
    RTCYEAR = year;
}

void RTC_B::setHour(unsigned int hour)
{
    if (hour > 23) hour = 23;
    RTCHOUR = hour;
}

void RTC_B::setMinute(unsigned int minute)
{
    if (minute > 59) minute = 59;
    RTCMIN = minute;
}

void RTC_B::setSecond(unsigned int second)
{
    if (second > 59) second = 59;
    RTCSEC = second;
}

void RTC_B::end()
{
    RTCCTL1 |= RTCHOLD_H;
    RTCPS0CTL &= ~RT0PSIE;
    RTCPS1CTL &= ~RT1PSIE;
    RTCCTL0 &= ~(RTCAIE | RTCTEVIE | RTCOFIE);
}

unsigned int RTC_B::getTime(uint8_t *buf)
{
    RTC_DOW dow;
    unsigned int month, day, year, hour, minute, second;

    _rtcrdy_semaphore();
    dow = (RTC_DOW)RTCDOW;
    month = RTCMON;
    day = RTCDAY;
    year = RTCYEAR;
    hour = RTCHOUR;
    minute = RTCMIN;
    second = RTCSEC;

    buf[0] = year & 0xFF;
    buf[1] = year >> 8;
    buf[2] = (uint8_t)dow;
    buf[3] = (uint8_t)month;
    buf[4] = (uint8_t)day;
    buf[5] = (uint8_t)hour;
    buf[6] = (uint8_t)minute;
    buf[7] = (uint8_t)second;

    return 8;
}

static const char *rtc_dayofweek_long[] = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};

static const char *rtc_dayofweek_short[] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

static const char *rtc_month_long[] = {
    "INVALID",
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

static const char *rtc_month_short[] = {
    "INV", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

unsigned int RTC_B::getTimeString(char *outbuf, const uint8_t *timebuf)
{
    int i;
    char tmp[16];

    outbuf[0] = '\0';
    if (string_fmt_bits & RTC_B_STRINGFMT_BIT_SHORTDATE) {
        // Use MON/DAY/YEAR HH:MM:SS style notation
        if (string_fmt_bits & RTC_B_STRINGFMT_BIT_DAYBEFOREMONTH) {
            itoa(timebuf[4], tmp, 10);  // Day
            strcat(outbuf, tmp);
            strcat(outbuf, "/");
            itoa(timebuf[3], tmp, 10);  // Month
            strcat(outbuf, tmp);
        } else {
            itoa(timebuf[3], tmp, 10);  // Month
            strcat(outbuf, tmp);
            strcat(outbuf, "/");
            itoa(timebuf[4], tmp, 10);  // Day
            strcat(outbuf, tmp);
        }
        strcat(outbuf, "/");
        itoa(*((unsigned int *)(&timebuf[0])), tmp, 10);
        strcat(outbuf, tmp);
        strcat(outbuf, " ");
    } else {
        // Use Dow, Month Day, Year HH:MM:SS style notation
        if (string_fmt_bits & RTC_B_STRINGFMT_BIT_USESHORTWORDS) {
            strcat(outbuf, rtc_dayofweek_short[timebuf[2]]);
        } else {
            strcat(outbuf, rtc_dayofweek_long[timebuf[2]]);
        }
        strcat(outbuf, " ");
        if (string_fmt_bits & RTC_B_STRINGFMT_BIT_DAYBEFOREMONTH) {
            itoa(timebuf[4], tmp, 10);  // Day
            strcat(outbuf, tmp);
            strcat(outbuf, " ");
            if (string_fmt_bits & RTC_B_STRINGFMT_BIT_USESHORTWORDS) {  // Month
                strcat(outbuf, rtc_month_short[timebuf[3]]);
            } else {
                strcat(outbuf, rtc_month_long[timebuf[3]]);
            }
        } else {
            if (string_fmt_bits & RTC_B_STRINGFMT_BIT_USESHORTWORDS) {  // Month
                strcat(outbuf, rtc_month_short[timebuf[3]]);
            } else {
                strcat(outbuf, rtc_month_long[timebuf[3]]);
            }
            strcat(outbuf, " ");
            itoa(timebuf[4], tmp, 10);  // Day
            strcat(outbuf, tmp);
        }
        strcat(outbuf, " ");
        itoa(*((unsigned int *)(&timebuf[0])), tmp, 10);
        strcat(outbuf, tmp);
        strcat(outbuf, " ");
    }
    // Add HH:MM[:SS] and return
    if (string_fmt_bits & RTC_B_STRINGFMT_BIT_USE24HR) {
        itoa(timebuf[5] > 0 ? timebuf[5] : 12, tmp, 10);
    } else {
        itoa(timebuf[5] >= 12 ? timebuf[5] == 12 ? 12 : timebuf[5] - 12 : timebuf[5] == 0 ? 12 : timebuf[5], tmp, 10);
    }
    strcat(outbuf, tmp);
    strcat(outbuf, ":");
    tmp[0] = '0';
    itoa(timebuf[6], tmp+1, 10);
    if (timebuf[6] < 10) {
        strcat(outbuf, tmp);
    } else {
        strcat(outbuf, tmp+1);
    }
    if (string_fmt_bits & RTC_B_STRINGFMT_BIT_INCLUDESECONDS) {
        strcat(outbuf, ":");
        itoa(timebuf[7], tmp+1, 10);
        if (timebuf[7] < 10) {
            strcat(outbuf, tmp);
        } else {
            strcat(outbuf, tmp+1);
        }
    }

    if ( !(string_fmt_bits & RTC_B_STRINGFMT_BIT_USE24HR) ) {
        if (timebuf[5] >= 12) {
            strcat(outbuf, " PM");
        } else {
            strcat(outbuf, " AM");
        }
    }
    return strlen(outbuf);
}

unsigned int RTC_B::getTimeString(char *outbuf)
{
    uint8_t timebuf[8];

    getTime(timebuf);
    return getTimeString(outbuf, timebuf);
}

void RTC_B::setTimeStringFormat(boolean use_24hr, boolean use_shortwords,
                                boolean day_before_month, boolean short_date_notation,
                                boolean include_seconds)
{
    if (use_24hr)
        string_fmt_bits |= RTC_B_STRINGFMT_BIT_USE24HR;
    else
        string_fmt_bits &= ~RTC_B_STRINGFMT_BIT_USE24HR;

    if (use_shortwords)
        string_fmt_bits |= RTC_B_STRINGFMT_BIT_USESHORTWORDS;
    else
        string_fmt_bits &= ~RTC_B_STRINGFMT_BIT_USESHORTWORDS;

    if (day_before_month)
        string_fmt_bits |= RTC_B_STRINGFMT_BIT_DAYBEFOREMONTH;
    else
        string_fmt_bits &= ~RTC_B_STRINGFMT_BIT_DAYBEFOREMONTH;

    if (short_date_notation)
        string_fmt_bits |= RTC_B_STRINGFMT_BIT_SHORTDATE;
    else
        string_fmt_bits &= ~RTC_B_STRINGFMT_BIT_SHORTDATE;

    if (include_seconds)
        string_fmt_bits |= RTC_B_STRINGFMT_BIT_INCLUDESECONDS;
    else
        string_fmt_bits &= ~RTC_B_STRINGFMT_BIT_INCLUDESECONDS;
}

#ifdef __MSP430_HAS_FRAM__
__attribute__((section(".text")))
uint8_t _rtcb_fram_save_segment[9];

void RTC_B::save()
{
    getTime((uint8_t *)_rtcb_fram_save_segment);
    _rtcb_fram_save_segment[8] = RTC_B_FRAM_SEGMENT_MAGIC_COOKIE;
}

void RTC_B::restoreFromBuffer(const uint8_t *buf)
{
    RTCCTL1 |= RTCHOLD_H;

    RTCDOW = buf[2];
    RTCMON = buf[3];
    RTCDAY = buf[4];
    RTCHOUR = buf[5];
    RTCMIN = buf[6];
    RTCSEC = buf[7];
    RTCYEAR = buf[0] | (buf[1] << 8);

    RTCCTL1 &= ~RTCHOLD_H;
}

boolean RTC_B::restore()
{
    if (_rtcb_fram_save_segment[8] != RTC_B_FRAM_SEGMENT_MAGIC_COOKIE)
        return false;

    restoreFromBuffer((uint8_t *)_rtcb_fram_save_segment);
    return true;
}
#endif

boolean RTC_B::attachPeriodicInterrupt(unsigned int prescale, RTC_INTERRUPT_HANDLER userFunc)
{
    int i;

    for (i=15; i >= 0; i--) {
        if (prescale & (1 << i))
            break;
    }
    if (i < 0 || i > 14)
        return false;  // Invalid prescaler specified!

    if (i < 7) {  // Divider 1-64 == Requesting RTCPS1
        if (_rtcb_prescaleHandlers[1] != NULL)
            return false;  // But an RTCPS1 handler is already specified...
        RTCPS1CTL &= ~RT1PSIFG;
        _rtcb_prescaleHandlers[1] = userFunc;
        RTCPS1CTL = ((6-i) << 2) | RT1PSIE;
    } else {  // Divider 128-16384 == Requesting RTCPS0
        if (_rtcb_prescaleHandlers[0] != NULL)
            return false;  // But an RTCPS0 handler is already specified...
        RTCPS0CTL &= ~RT1PSIFG;
        _rtcb_prescaleHandlers[0] = userFunc;
        RTCPS0CTL = ((14-i) << 2) | RT0PSIE;
    }

    return true;
}

boolean RTC_B::detachPeriodicInterrupt(RTC_INTERRUPT_HANDLER userFunc)
{
    if (_rtcb_prescaleHandlers[0] == userFunc) {
        RTCPS0CTL = 0;
        _rtcb_prescaleHandlers[0] = NULL;
        return true;
    }

    if (_rtcb_prescaleHandlers[1] == userFunc) {
        RTCPS1CTL = 0;
        _rtcb_prescaleHandlers[1] = NULL;
        return true;
    }

    return false;
}

boolean RTC_B::attachScheduledInterrupt(int day, RTC_DOW dow, int hour, int min, RTC_INTERRUPT_HANDLER userFunc)
{
    boolean is_set = false;

    RTCCTL0 &= ~(RTCAIFG | RTCAIE);

    if (_rtcb_alarmHandler != NULL)
        return false;  // Already have one scheduled!

    if (day > 31) day = 31;
    if (hour > 23) hour = 23;
    if (min > 59) min = 59;

    if (day > 0) {
        RTCADAY = 0x80 | day;
        is_set = true;
    } else {
        RTCADAY = 0;
    }

    if (hour >= 0) {
        RTCAHOUR = 0x80 | hour;
        is_set = true;
    } else {
        RTCAHOUR = 0;
    }

    if (min >= 0) {
        RTCAMIN = 0x80 | min;
        is_set = true;
    } else {
        RTCAMIN = 0;
    }

    if (dow != NO_ALARM) {
        RTCADOW = 0x80 | dow;
        is_set = true;
    } else {
        RTCADOW = 0;
    }

    if (!is_set) {
        // User ran this function but didn't bother configuring any alarm triggers!
        return false;
    }

    _rtcb_alarmHandler = userFunc;
    RTCCTL0 &= ~RTCAIFG;
    RTCCTL0 |= RTCAIE;
}

boolean RTC_B::detachScheduledInterrupt()
{
    if (_rtcb_alarmHandler == NULL)
        return false;

    RTCCTL0 &= ~(RTCAIFG | RTCAIE);
    _rtcb_alarmHandler = NULL;
    RTCADAY = 0;
    RTCADOW = 0;
    RTCAHOUR = 0;
    RTCAMIN = 0;

    return true;
}

void RTC_B::debug(Stream *out)
{
    uint8_t timebuf[8];

    out->println("-= RTC_B library DEBUG =-");
    out->print("RTC state: ");
    if (RTCCTL1 & RTCHOLD_H)
        out->println("HALTED");
    else
        out->println("RUNNING");
    out->print("Pulling RTC data... ");
    getTime(timebuf);
    out->println("done.");
    out->print("DOW: ");
    out->println(rtc_dayofweek_long[timebuf[2]]);
    out->print("Month: ");
    out->println(rtc_month_long[timebuf[3]]);
    out->print("Day: ");
    out->println(timebuf[4]);
    out->print("Year: ");
    out->println(*((unsigned int *)(&timebuf[0])));
    out->print("Hour: ");
    out->println(timebuf[5]);
    out->print("Minute: ");
    out->println(timebuf[6]);
    out->print("Second: ");
    out->println(timebuf[7]);
    out->print("RTCPS0 ISR: ");
    if (_rtcb_prescaleHandlers[0] != NULL) {
        out->print("configured; ptr=0x");
        out->println((uint16_t)_rtcb_prescaleHandlers[0], HEX);
    } else {
        out->println("unconfigured");
    }
    out->print("RTCPS0 prescaler bit setting: ");
    out->println(RTCPS0CTL >> 2);

    out->print("RTCPS1 ISR: ");
    if (_rtcb_prescaleHandlers[1] != NULL) {
        out->print("configured; ptr=0x");
        out->println((uint16_t)_rtcb_prescaleHandlers[1], HEX);
    } else {
        out->println("unconfigured");
    }
    out->print("RTCPS1 prescaler bit setting: ");
    out->println(RTCPS1CTL >> 2);

    if (RTCCTL0 & RTCAIE) {
        out->println("Alarm: enabled");
        out->println("Alarm configuration:");
        if (RTCAMIN & 0x80) {
            out->print("(minute: "); out->print(RTCAMIN & 0x7F); out->println(')');
        } else {
            out->println("(minute: not used)");
        }
        if (RTCAHOUR & 0x80) {
            out->print("(hour: "); out->print(RTCAHOUR & 0x7F); out->println(')');
        } else {
            out->println("(hour: not used)");
        }
        if (RTCADOW & 0x80) {
            out->print("(day of week: "); out->print(rtc_dayofweek_long[RTCADOW & 0x7F]); out->println(')');
        } else {
            out->println("(day of week: not used)");
        }
        if (RTCADAY & 0x80) {
            out->print("(day: "); out->print(RTCADAY & 0x7F); out->println(')');
        } else {
            out->println("(day: not used)");
        }
    } else {
        out->println("Alarm: disabled");
    }

    out->println("--------------------------");
}

__attribute__((interrupt(RTC_VECTOR)))
void rtcb_Interrupt_Vector(void)
{
    boolean still_asleep = stay_asleep;  // Support the wakeup() call inside ISR

    switch (RTCIV) {
        case RTCIV_RTCAIFG:
            _rtcb_alarmHandler();
            break;
        case RTCIV_RT0PSIFG:
            _rtcb_prescaleHandlers[0]();
            break;
        case RTCIV_RT1PSIFG:
            _rtcb_prescaleHandlers[1]();
            break;
    }

    // Support the wakeup() call inside ISR
    if (still_asleep != stay_asleep)
        __bic_SR_register_on_exit(LPM4_bits);
}



RTC_B rtc;
