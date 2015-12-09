#include <pebble.h>
#include "debugout.h"
#include "common.h"

static TextLayer *s_debug_out;
static GFont s_simple_font;
static char s_debug_buffer[1024];

static bool s_last_log_was_resp = false;
static GColor s_font_colour;
static bool s_initialised = false;

#define MAXLINES 16
static char s_prefixes[MAXLINES][6]; // hh:mm and that's it.
static char s_lines[MAXLINES][50];
static int s_count = 0;
static int s_first = 0;
static int s_last = MAXLINES;

static int Next(int i) {
  i++;
  if(i >= MAXLINES) i = 0;
  return i;
}

static int Previous(int i) {
  i--;
  if(i < 0) i = MAXLINES - 1;
  return i;
}

static void Render() {
  static char s_buffer[1024];
  s_buffer[0] = '\0';
  int i = s_first;
  int x = 0;
  //LOGF_DEBUG("RENDER - i=%d, x=%d", i, x);
  do {
    x++;
    //LOGF_DEBUG("---- %d prefix=%s, line=%s", x, s_prefixes[i], s_lines[i]);
    strcat(s_buffer, s_prefixes[i]);
    strcat(s_buffer, "-");
    strcat(s_buffer, s_lines[i]);
    strcat(s_buffer, "\n");
    i = Next(i);
  } while (x<s_count && i != s_first);
  int l ;
  l = strlen(s_buffer);
  //LOGF_DEBUG("----- Final text\n%s", s_buffer);
  if(l > 0) s_buffer[l - 1] = '\0';
  text_layer_set_text(s_debug_out, s_buffer);
}
void HAXXrender() {Render();}

static void RollUpPrevious(void) {
  if(s_count < 4)
    return;

  char *last[4];
  int l = s_last;
  for(int i = 0; i < 4; i++) {
    last[i] = s_lines[l];
    l = Previous(l);
  }
  if(strcmp(last[0], last[1]) == 0 &&
     (strcmp(last[0], last[2]) == 0 || strcmp(last[0], last[3]) == 0) ) {

    int previous = Previous(s_last);


    if(strcmp(last[0], last[2]) == 0) {
      LOGF_DEBUG("Doing ... %s", last[0]);
      strcpy(s_lines[previous], "...");
      strcpy(s_prefixes[previous], "");
    } else if(strcmp(last[2], "...") == 0) {
      LOGF_DEBUG("Doing replace %s", last[0]);
      //strcpy(s_lines[previous],    s_lines[s_last]); No point copying something that is the same :)
      //strcpy(s_prefixes[previous], s_prefixes[s_last]);
      strcpy(s_prefixes[previous], "X");
      s_last = previous;
      s_count--;
    }
  }

}

static void AddLine(char *prefix, char *body) {
  //RollUpPrevious();
  int i = Next(s_last);
  if(s_count < MAXLINES) 
    s_count++;
  else
    s_first = Next(s_first);
  //if(s_count > 0 && i == s_first)
  //  s_first = Next(s_first);
  strcpy(s_prefixes[i], prefix);
  strcpy(s_lines[i], body);
  //FORMAT_STRING(s_lines[i], "%s-%d", body, i);
  s_last = i; // just set last at the start & use that?

  Render();
}

void debugout_log(char *message) {
  return;
  AddLine(timestamp_minutes(), message);
}

void debugout_append(char *message) {
  return;
  strcat(s_lines[s_last], message);
  Render();
}

void debugout_append_line(char *message) {
  return;
  AddLine("", message);
}







void debug_log_err(char *message, char *reason) {
  return;
  //s_REQ_IN_PROCESS = false;
  //char err_line[100];
  //FORMAT_STRING(err_line, "%s\n%s", message, reason);
  debugout_log(message);
  debugout_append_line(reason);
}


void debugout_logerr(char *message, char *reason) {
  return;
  debug_log_err(message, reason);
}
void debugout_logerrcode(char *message, AppMessageResult reason) {
  return;
  debug_log_err(message, translate_error(reason));
}


void log_requested() {
  return;
  //if(s_last_log_was_resp)
    //delete_last_line();
  
  debugout_log("Req.");
  //s_REQ_IN_PROCESS = true;
}
void log_received(char *received) {
  return;
  debugout_append(received);
  
  // don't want to be binning last line if some error has also slipped in.
  //if(s_REQ_IN_PROCESS)
   //s_last_log_was_resp = true;
  
  //s_REQ_IN_PROCESS = false;
  
  // do something to reset the colour - red should now be black
}


/*

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
*/

void debugout_visible(bool visible) {
  return;
  layer_set_hidden((Layer*)s_debug_out, !visible);
}

void debugout_create(void) {
  return;
  //s_simple_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  s_simple_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_10));
  s_debug_out = build_textlayer(GRect(2,2, 140, 164), s_simple_font, GColorBlack, GTextAlignmentLeft);
  s_font_colour = GColorBlack;
  text_layer_set_background_color(s_debug_out, GColorWhite);

  debugout_visible(false);
  debugout_log("Initialised");
  
  s_initialised = true;
}
void debugout_delete(void) {
  return;
  s_initialised = false;
  
  fonts_unload_custom_font(s_simple_font);
  text_layer_destroy(s_debug_out);
}
