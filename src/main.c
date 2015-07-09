#include <pebble.h>
#include "main.h"
#include "watchface.h"

int main() {
  show_watchface();
  app_event_loop();
  return 0;
}
