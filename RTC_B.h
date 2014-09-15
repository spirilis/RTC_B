/* RTC_B library for the MSP430 RTC_B peripheral found on MSP430FR58xx/59xx devices.
 *
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
 *
 */

#ifndef _RTC_B_H_
#define _RTC_B_H_

#include <Energia.h>
#include <Stream.h>

#if !defined(__MSP430_HAS_RTC_B__)
#error "Your microcontroller does not contain the RTC_B peripheral."
#endif

typedef enum RTC_DOW
{
  NO_ALARM = -1,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY,
  SUNDAY
} RTC_DOW;

typedef void(*RTC_INTERRUPT_HANDLER)(void);

class RTC_B {
    public:
        RTC_B();

        void begin(void);
        void begin(RTC_DOW dow, unsigned int month, unsigned int day, unsigned int year, unsigned int hour, unsigned int minute, unsigned int second);
        void end(void);
        void debug(Stream *);

        // Wolverine-related save/restore
        #ifdef __MSP430_HAS_FRAM__
        void save(void);
        boolean restore(void);
        void restoreFromBuffer(const uint8_t *buf);
        #endif

        // Time setting
        void setDOW(RTC_DOW);
        void setMonth(unsigned int);
        void setDay(unsigned int);
        void setYear(unsigned int);
        void setHour(unsigned int);
        void setMinute(unsigned int);
        void setSecond(unsigned int);

        // Time getting
        RTC_DOW getDOW(void) { _rtcrdy_semaphore(); return (RTC_DOW)RTCDOW; };
        unsigned int getMonth(void) { _rtcrdy_semaphore(); return RTCMON; };
        unsigned int getDay(void) { _rtcrdy_semaphore(); return RTCDAY; };
        unsigned int getYear(void) { _rtcrdy_semaphore(); return RTCYEAR; };
        unsigned int getHour(void) { _rtcrdy_semaphore(); return RTCHOUR; };
        unsigned int getMinute(void) { _rtcrdy_semaphore(); return RTCMIN; };
        unsigned int getSecond(void) { _rtcrdy_semaphore(); return RTCSEC; };

        unsigned int getTime(uint8_t *buf);  // Returns length of buffer (8 bytes)
        unsigned int getTimeString(char *buf); // Returns string length
        unsigned int getTimeString(char *buf, const uint8_t *timebuf); // Returns string length
        void setTimeStringFormat(boolean use_24hr, boolean use_shortwords, boolean day_before_month, boolean short_date_notation, boolean include_seconds);

        // Interval interrupts
        boolean attachPeriodicInterrupt(unsigned int prescale, RTC_INTERRUPT_HANDLER userFunc);
        boolean detachPeriodicInterrupt(RTC_INTERRUPT_HANDLER userFunc);

        // Alarm-based interrupt
        boolean attachScheduledInterrupt(int day, RTC_DOW dow, int hour, int min, RTC_INTERRUPT_HANDLER userFunc);
        boolean detachScheduledInterrupt(void);

    private:
        uint16_t string_fmt_bits;
        void _rtcrdy_semaphore(void);
};

#define RTC_B_STRINGFMT_BIT_USE24HR 0x0001
#define RTC_B_STRINGFMT_BIT_USESHORTWORDS 0x0002
#define RTC_B_STRINGFMT_BIT_DAYBEFOREMONTH 0x0004  // European style
#define RTC_B_STRINGFMT_BIT_SHORTDATE 0x0008
#define RTC_B_STRINGFMT_BIT_INCLUDESECONDS 0x0010

#define RTC_B_FRAM_SEGMENT_MAGIC_COOKIE 0xA8

extern RTC_B rtc;

#endif
