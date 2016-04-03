/* RTC_B library example: SaveAcrossResets
 *
 * Configure RTC with date information derived from the FRAM buffer if it's
 * been initialized, otherwise set the RTC with the hard-coded date & time
 * information specified in this sketch.
 * 
 * Then output the current date & time once a second over the Serial port.
 */

#include <RTC_B.h>

void setup() {
  Serial.begin(115200);
  Serial.println("RTC_B example: SaveAcrossResets");

  rtc.begin();

  if (!rtc.restore()) {
    /* This will run only once when you upload the sketch; after subsequent RESETs or power cycles
     * it should always get skipped as the rtc.restore() function will have configured the RTC info
     * from the date/time information stored in FRAM.  rtc.restore() returns "true" if that has occurred.
     *
     * A magic cookie byte in that FRAM segment is used to determine whether the segment has been
     * written to since the last sketch upload.
     */
    rtc.end();
    rtc.begin(WEDNESDAY, 8, 20, 2014, 15, 21, 00);  // 8/20/2014 15:21
    rtc.save();
  }

  rtc.attachPeriodicInterrupt(1, flagTick);  // Runs flagTick() once per second
  rtc.setTimeStringFormat(true, true, false, false, true);
}

volatile boolean tick = false;

void loop() {
  char current_time[64];  // Temporary char[] buffer to hold time string

  if (tick) {
    Serial.print("Date/time: ");
    rtc.getTimeString(current_time);  /* This loads current_time[] with a string representation of
                                       * the current date & time.
                                       */
    Serial.println(current_time);
    tick = false;  // Be sure this gets cleared after we service it!
  }
}

void flagTick()
{
  tick = true;
  rtc.save();  // Commit RTC info to FRAM automatically so it's restored upon RESET
  wakeup(); // not necessary but included for example; use if this tick should wake CPU up from
            // LPM3, sleep() or sleepSeconds()
}
