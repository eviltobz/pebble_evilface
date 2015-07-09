#pragma once
void show_watchface(void);
void hide_watchface(void);

void update_time(struct tm *tick_time);
void update_date(struct tm *tick_time);

void update_battery(BatteryChargeState state);