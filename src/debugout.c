#include <pebble.h>
#include "debugout.h"
#include "common.h"

static TextLayer *s_debug_out;
static GFont s_simple_font;
static char s_debug_buffer[1024];

static bool s_last_log_was_resp = false;
static GColor s_font_colour;
static bool s_initialised = false;

static int find_index_of(char *str, char character) {
  //LOGF_DEBUG("find_index_of in %d chars", strlen(str));
  u_int p = 0;
  while(str[p] != character && p < strlen(str))
    p++;
  
  if(p<strlen(str))
    p++;
  
  //LOGF_DEBUG("find_index_of returning: %d", p);
  return p;
}
// why can't i do this pointer manipulation in find_index_of?!?!?
int count_characters(const char *str, char character)
{
    const char *p = str;
    int count = 0;

    do {
        if (*p == character)
            count++;
    } while (*(p++));

    return count;
}


static bool s_REQ_IN_PROCESS = false;
static void scroll_debug_buffer(void) {
  static char temp_buffer[sizeof(s_debug_buffer)];
  if(count_characters(s_debug_buffer, '\n') > 12) {
    //LOG_DEBUG("scroll_debug_buffer - scrolling");
    int second_line_index = find_index_of(s_debug_buffer, '\n');
    strcpy(temp_buffer, (s_debug_buffer + second_line_index));
    strcpy(s_debug_buffer, temp_buffer);
  }
  //else
    //LOG_DEBUG("scroll_debug_buffer - no work to do :)");
  
  if(strlen(s_debug_buffer) > (sizeof(s_debug_buffer)-30)) {
    //LOGF_DEBUG("Debug overflowing - %d chars", strlen(s_debug_buffer));
    strcpy(s_debug_buffer, "");
  }
}
static void delete_last_line(void) {
  //LOGF_DEBUG("find last index of in %d chars", strlen(s_debug_buffer));
  u_int p = strlen(s_debug_buffer);
  while(s_debug_buffer[p] != '\n' && p > 0)
    p--;
  
  //LOGF_DEBUG("find last index of was %d - setting to null terminator", p);
  
  s_debug_buffer[p] = 0;
}
static void update_display_text(void) {
  if(!s_initialised)
    return;
  
  text_layer_set_text_color(s_debug_out, s_font_colour);
  text_layer_set_text(s_debug_out, s_debug_buffer);
}
static void debug_append_internal(char *message) {
  strcat(s_debug_buffer, message);
  update_display_text();
  
  s_last_log_was_resp = false;
}

void debug_append_line(char *message){
  scroll_debug_buffer();
  
  char line[100];
  char newline[2];
  if(strlen(s_debug_buffer) > 0)
    strcpy(newline, "\n");
  else 
    strcpy(newline, "");
  
  FORMAT_STRING(line, "%s%s-%s", newline, timestamp_minutes(), message);
  if(s_REQ_IN_PROCESS){
    //FORMAT_STRING(line, "%s!!%s-%s", newline, timestamp_minutes(), message);
    s_font_colour = GColorRed;
  }
  else {
    //FORMAT_STRING(line, "%s%s-%s", newline, timestamp_minutes(), message);
    s_font_colour = GColorBlack;
  }
  
  // have buffers for the last few messages, not counting timestamps
  // if they're the same, show first, ..., last
  // keep moving last as it posts - stop having the same message loooooadsa times
  // moving to an explicit multiline buffer should help with this a bit anyways.
  
  debug_append_internal(line);
};
void debug_append(char *message) {
  if(strlen(s_debug_buffer) == 0)
    debug_append_line(message);
  else
    debug_append_internal(message);
}
void debug_log_err(char *message, char *reason) {
  s_REQ_IN_PROCESS = false;
  char err_line[100];
  FORMAT_STRING(err_line, "%s\n%s", message, reason);
  debug_append_line(err_line);
}
void log_requested() {
  if(s_last_log_was_resp)
    delete_last_line();
  
  debug_append_line("Req.");
  s_REQ_IN_PROCESS = true;
}
void log_received(char *received) {
  debug_append(received);
  
  // don't want to be binning last line if some error has also slipped in.
  if(s_REQ_IN_PROCESS)
   s_last_log_was_resp = true;
  
  s_REQ_IN_PROCESS = false;
  
  // do something to reset the colour - red should now be black
}



// GOOD INTERFACE...
void debugout_logerr(char *message, char *reason) {
  debug_log_err(message, reason);
}
void debugout_logerrcode(char *message, AppMessageResult reason) {
  debug_log_err(message, translate_error(reason));
}
void debugout_logline(char *message) {
  debug_append_line(message);
}
void debugout_append(char *message) {
  debug_append(message);
}

void debugout_visible(bool visible) {
  layer_set_hidden((Layer*)s_debug_out, !visible);
}

/*
static void toggle_display(void) {
  static bool visible = true;
  //layer_set_hidden((Layer*)s_debug_out, visible);
  debugout_visible(visible);
  visible = !visible;
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  toggle_display();
}
*/


void debugout_create(void) {
  //s_simple_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  s_simple_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_10));
  s_debug_out = build_textlayer(GRect(2,2, 140, 164), s_simple_font, GColorBlack, GTextAlignmentLeft);
  s_font_colour = GColorBlack;
  text_layer_set_background_color(s_debug_out, GColorWhite);
  //toggle_display();
  debugout_visible(false);
  
  //accel_tap_service_subscribe(tap_handler);
  s_initialised = true;
  update_display_text();
}
void debugout_delete(void) {
  s_initialised = false;
  
  //accel_tap_service_unsubscribe();
  fonts_unload_custom_font(s_simple_font);
  
  text_layer_destroy(s_debug_out);
}