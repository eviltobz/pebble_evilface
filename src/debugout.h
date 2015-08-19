#pragma once
#include <pebble.h>

// need to tidy this interface up
//void debug_append(char *message);
//void debug_log_err(char *message, char *reason);
void log_requested();
void log_received(char *received);
//void log_inbox_dropped(char *reason);
//void log_outbox_failed(char *reason);

// good methods go down here VVV
void debugout_logerr(char *message, char *reason);
void debugout_logerrcode(char *message, AppMessageResult reason);
void debugout_logline(char *message);
void debugout_append(char *message);

void debugout_create(void);
void debugout_delete(void);