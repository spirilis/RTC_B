/* RTC_B library example: AlarmOnceAnHour
 *
 * Configure RTC with no useful date information, but set an alarm for minute #1
 * so it goes off roughly 1 minute after the MCU starts.
 * If you keep it running, it should keep firing that interrupt once an hour.
 */

#include <RTC_B.h>

void setup() {
  Serial.begin(115200);
  Serial.println("RTC_B example: AlarmOnceAnHour");

  rtc.begin();
  // Register an alarm on minute #1, ignoring the Day, DayOfWeek (NO_ALARM), and Hour fields.
  rtc.attachScheduledInterrupt(-1, NO_ALARM, -1, 1, flagTick);
  Serial.println("Waiting for alarm to go off...");
}

volatile boolean tick = false;

void loop() {
  if (tick) {
    Serial.println("ALARM!");
    tick = false;  // Be sure this gets cleared after we service it!
  }
  delay(100);
}

void flagTick()
{
  tick = true;
}
