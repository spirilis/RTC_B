RTC_B
=====

A deluxe Energia library for using the RTC_B peripheral on TI's MSP430 Wolverine MCUs

# Usage
RTC_B has a simple API for configuring and using the RTC onboard the TI MSP430 microcontrollers which feature it.
The RTC_B is described in detail in the MSP430FR58xx/59xx User's Guide - http://ti.com/lit/slau367

This RTC supports Calendar mode, with a single Alarm configurable to latch on Day of Week, Day of Month, Hour
or Minute.  Any combination of these criteria may be switched on to provide a flexible alarm system.

Additionally the RTC performs 2 levels of periodic interrupt capability; RTCPS0 supports 1/128 to 1/16384-second
periodic IRQs, and RTCPS1 supports 2/second, 1/second and 1/2-1/64-second periodic IRQs.

This library supports *1/second* down to *1/16384-second* ticks using the attachPeriodicInterrupt() feature.
