#include <pebble.h>
#include "common.h"
#include "timedisplay.h"
#include "weatherdisplay.h"
#include "statusdisplay.h"
#include "debugout.h"
#include "eventhandler.h"
  
static Window *s_window;

static void handle_window_unload(Window* window) {
  eventhandler_unsubscribe();
  timedisplay_delete();
  statusdisplay_delete();
  weatherdisplay_delete();
  debugout_delete();
  
  window_destroy(s_window);
}

void show_watchface(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlue);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  
  set_root_layer(s_window);
  
  timedisplay_create(s_window);
  statusdisplay_create(s_window);
  weatherdisplay_create(s_window);
  
  // debug overlay as last item
  debugout_create();

  window_stack_push(s_window, true);
  
  eventhandler_subscribe();
}

void hide_watchface(void) {
  window_stack_remove(s_window, true);
}

int main() {
  show_watchface();
  app_event_loop();
  return 0;
}

