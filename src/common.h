#pragma once
#include <pebble.h>

struct tm* get_current_time();
char* timestamp_seconds();
char* timestamp_minutes();
void set_root_layer(Window *window);

TextLayer* build_default_textlayer(GRect bounds, GFont font);
TextLayer* build_textlayer(GRect bounds, GFont font, GColor colour, GTextAlignment alignment);
BitmapLayer* build_bitmaplayer(GRect bounds, GColor colour);

char *translate_error(AppMessageResult result);

#define FORMAT_STRING(dest, format, ...)(snprintf(dest, sizeof(dest), format, __VA_ARGS__))
  
  
// The pebble logging macros only send messages to the phone, and the logs there are only
// viewable whilst connected with cloudpebble from current experience, so aren't useful
// for diagnosing things that have happened whilst not actively debugging, so are of 
// minimal value
#define LOG_ANY(level, message)(APP_LOG(level, "%s -- %s", timestamp_seconds(), message))
#define LOGF_ANY(level, message, ...) {\
  char buffer[256];\
  snprintf(buffer, sizeof(buffer), "%s -- %s", timestamp_seconds(), message);\
  APP_LOG(level, buffer, __VA_ARGS__);\
}

#define LOGF_DEBUG_VERBOSE(message, ...) (LOGF_ANY(APP_LOG_LEVEL_DEBUG_VERBOSE, message, __VA_ARGS__))
#define LOGF_DEBUG(message, ...) (LOGF_ANY(APP_LOG_LEVEL_DEBUG, message, __VA_ARGS__))
#define LOGF_INFO(message, ...) (LOGF_ANY(APP_LOG_LEVEL_INFO, message, __VA_ARGS__))
#define LOGF_WARNING(message, ...) (LOGF_ANY(APP_LOG_LEVEL_WARNING, message, __VA_ARGS__))
#define LOGF_ERROR(message, ...) (LOGF_ANY(APP_LOG_LEVEL_ERROR, message, __VA_ARGS__))

#define LOG_DEBUG_VERBOSE(message) (LOG_ANY(APP_LOG_LEVEL_DEBUG_VERBOSE, message))
#define LOG_DEBUG(message) (LOG_ANY(APP_LOG_LEVEL_DEBUG, message))
#define LOG_INFO(message) (LOG_ANY(APP_LOG_LEVEL_INFO, message))
#define LOG_WARNING(message) (LOG_ANY(APP_LOG_LEVEL_WARNING, message))
#define LOG_ERROR(message) (LOG_ANY(APP_LOG_LEVEL_ERROR, message))