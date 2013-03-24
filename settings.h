/*
 *
 *  Copyright (C) 2013 Bas Brugman
 *  http://www.visionnaire.nl
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#ifndef _SETTINGS_H_INCLUDED
#define _SETTINGS_H_INCLUDED

typedef struct {                     
  uint16_t deep_sleep_counter; // default: 1200, based on TIMER1 ticks, default 0.5sec/tick = 10mins 
  uint16_t settle_time; // default 800: 5ms ticks, so 4 secs
  uint16_t alarm_settle_time; // default 800: 5ms ticks, so 4 secs
  uint8_t indicator_sound; // default 1: beep when indicators blink
  uint16_t blink_speed; // default 9766: 1 tick is 0.0000512 secs
  uint8_t alarm_counter; // default 4 times: 2 times on and 2 times off
  uint8_t alarm_trigger; // default 18: voltage offset 18 * 5V/1024 parts
  uint8_t alarm_trigger_counter; // default 4: we need 4 times a postive alarm trigger, each measurement is done TIMER1 (0.5s) tick
  uint8_t alarm_thres_min; // default 140: voltage minimum 0.685
  uint16_t alarm_thres_max; // default 550: voltage maximum 2.685
  uint8_t backpedal; // default 0: indicators and beep when moving backswards  
  uint16_t backpedal_thres_min; // default 297: about 1.45V, i.e. min. 25cm/s backwards acceleration detected
  uint8_t startup_sound; // default 0: random, index of music array + 1
  uint8_t alarm_sound; // default 1: index of music array + 1
  char passwd[32]; // default: admin, uart shell password
} tSettings;

#endif
