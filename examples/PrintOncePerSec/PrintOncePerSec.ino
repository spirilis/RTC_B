/* RTC_B library example: PrintOncePerSec
 *
 * Configure RTC with no useful date information, but run a prescaler-based
 * ISR once a second which will flag our sketch to print text over the Serial port.
 */

#include <RTC_B.h>

void setup() {
  Serial.begin(115200);
  Serial.println("RTC_B example: PrintOncePerSec");

  rtc.begin();
  rtc.attachPeriodicInterrupt(1, flagTick);  // Runs flagTick() once per second
}

volatile boolean tick = false;

void loop() {
  if (tick) {
    Serial.println("Tick!");
    tick = false;  // Be sure this gets cleared after we service it!
  }
}

void flagTick()
{
  tick = true;
  wakeup(); // Not necessary here but use this if the tick function wakes CPU from LPM3,
            // sleep() or sleepSeconds()
}
