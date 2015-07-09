#pragma once

void statusdisplay_create(Window *window);
void statusdisplay_delete(void);
void statusdisplay_update_battery(BatteryChargeState state);
void statusdisplay_update_connection(bool connected);