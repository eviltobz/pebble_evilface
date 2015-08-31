#include <pebble.h>
#include "eventhandler.h"

#include "common.h"
#include "debugout.h"
#include "statusdisplay.h"
#include "timedisplay.h"
#include "weatherdisplay.h"
  
bool should_show_debug(AccelAxisType axis) {
  if(axis == ACCEL_AXIS_X) // Add Z axis too?
    return false;

  static time_t previous = 0;
  time_t current = time(NULL);
  time_t diff = current - previous;
  previous = current;
  
  if(diff < 4)
    return true;

  return false;
}

static void log_axis(AccelAxisType axis, int32_t direction) {
  static int x1, x2, y1, y2, z1, z2;
  if(axis == ACCEL_AXIS_X && direction < 0) x1++;
  if(axis == ACCEL_AXIS_X && direction > 0) x2++;
  if(axis == ACCEL_AXIS_Y && direction < 0) y1++;
  if(axis == ACCEL_AXIS_Y && direction > 0) y2++;
  if(axis == ACCEL_AXIS_Z && direction < 0) z1++;
  if(axis == ACCEL_AXIS_Z && direction > 0) z2++;
  char tap[50];
  FORMAT_STRING(tap, "TAP X%d/%d, Y%d/%d, Z%d/%d", x1, x2, y1, y2, z1, z2);
  debugout_log(tap);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  //log_axis(axis, direction);

  if(should_show_debug(axis)) {
    debugout_visible(true);
  } else {
    debugout_visible(false);
    
    // toggle weather displays to show Home or Work
  }
  
  /*
  // HACK - debugout just doing simple toggle for now...
  static bool visible = true;
  debugout_visible(visible);
  visible = !visible;
  */
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  timedisplay_update(tick_time);
  weatherdisplay_update(tick_time);
}

static void SECONDHACK_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  tick_handler(tick_time, units_changed);
  //HACK_draw_next_weather();
  char line[50];
  static int count = 1;
  FORMAT_STRING(line, "%d - Hacky log info to test", count++);
  debugout_log(line);

  static int meh = 0;
  meh++;
  if(meh == 8) {
    debugout_log("one, two, miss a few");
  }
}



void eventhandler_subscribe() {
  accel_tap_service_subscribe(tap_handler);
  
  battery_state_service_subscribe(statusdisplay_update_battery);
  bluetooth_connection_service_subscribe(statusdisplay_update_connection);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  tick_timer_service_subscribe(SECOND_UNIT, SECONDHACK_tick_handler);
  
  
  // Not currently dealing with events raised from communication with phone. 
  // That's all in weatherdisplay, but we may want to pull it here later too, so then
  // the weather display could just take relevant weather info struct as an arg
  
}

void eventhandler_unsubscribe() {
  accel_tap_service_unsubscribe();
  
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  
  tick_timer_service_unsubscribe();
}
