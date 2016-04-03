/* RTC_B library example: UseBothPeriodicInterrupts
 *
 * Configure RTC with no useful date information, but run a prescaler-based
 * ISR once a second which will flag our sketch to print text over the Serial port.
 * Additionally, configure a much faster-interval interrupt (256 times/sec, which should
 * trigger use of RTCPS0 instead of RTCPS1) to print a single dot (".") over the Serial
 * port.
 *
 * As a simple experiment, we also try to register a 16-times-a-second interrupt which
 * lies within the realm of RTCPS1's territory; but since RTCPS1's IRQ is already registered
 * to our once-a-second interrupt, this fails.
 */

#include <RTC_B.h>

void setup() {
  boolean t;

  Serial.begin(115200);
  Serial.println("RTC_B example: UseBothPeriodicInterrupts");
  Serial.println("The RTC_B permits two types of \"periodic\" interrupts:");
  Serial.println("A 1-64 tick/second interrupt and a 128-16384 tick/second interrupt.");
  Serial.println("We're going to use both here to produce ticks at two intervals; once a second \"Tick!\"");
  Serial.println("  and once every 1/256th of a second producing a \".\" dot");
  Serial.println();

  rtc.begin();
  t = rtc.attachPeriodicInterrupt(1, flagTick);  // Runs flagTick() once per second
  if (!t) {
    Serial.println("ERROR: Could not register a once-per-second Periodic Interrupt!");
  }

  t = rtc.attachPeriodicInterrupt(256, flagFastTick);  // Runs flagFastTick() 256 times per second
  if (!t) {
    Serial.println("ERROR: Could not register a 256-times-a-second Periodic Interrupt!");
  }

  t = rtc.attachPeriodicInterrupt(16, flagFastTick);  // This should fail!
  if (!t) {
    Serial.println("Wasn't able to register a 16-times-a-second Periodic Interrupt because RTCPS1");
    Serial.println("  is already in use by flagTick (1/sec)!");
  }
}

volatile boolean tick = false, fasttick = false;

void loop() {
  if (fasttick) {
    Serial.print('.');
    fasttick = false;  // Be sure this gets cleared after we service it!
  }
  if (tick) {
    Serial.println();
    Serial.print("Tick!");
    tick = false;  // Be sure this gets cleared after we service it!
  }
}

void flagTick()
{
  tick = true;
  wakeup(); // only required to wake CPU from LPM3, sleep() or sleepSeconds()
  // The use of wakeup() isn't necessary for this example since our loop() has us running in a busy-wait without
  // any low-power sleep modes.
}

void flagFastTick()
{
  fasttick = true;
  wakeup();
}
