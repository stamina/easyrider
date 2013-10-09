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
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "command.h"
#include "usart.h"
#include "command_usart.h"
#include "command_handlers.h"
#include "easyrider.h"

#include "sound/alarm.h"
#include "sound/frogger.h"
#include "sound/furelise.h"
#include "sound/larry.h"
#include "sound/pipi.h"
#include "sound/popcorn.h"

// state transition struct array: handler assigment for state/event combo's
tTransition g_trans[] = {
  {check_claxon_on, EV_CLAXON_ON, process_claxon_on},
  {check_claxon_off, EV_CLAXON_OFF, process_claxon_off},
  {check_ri_on, EV_RI_ON, process_ri_on},
  {check_ri_off, EV_RI_OFF, process_ri_off},
  {check_li_on, EV_LI_ON, process_li_on},
  {check_li_off, EV_LI_OFF, process_li_off},
  {check_ign_on, EV_IGN_ON, process_ign_on},
  {check_ign_off, EV_IGN_OFF, process_ign_off},
  {check_alarm_on, EV_ALARM_ON, process_alarm_on},
  {check_alarm_off, EV_ALARM_OFF, process_alarm_off},
  {check_brake_on, EV_BRAKE_ON, process_brake_on},
  {check_brake_off, EV_BRAKE_OFF, process_brake_off},
  {check_pilot_on, EV_PILOT_ON, process_pilot_on},
  {check_pilot_off, EV_PILOT_OFF, process_pilot_off},
  {check_light_on, EV_LIGHT_ON, process_light_on},
  {check_light_off, EV_LIGHT_OFF, process_light_off},
  {check_battery_read, EV_READ_BATT, process_battery},
  {check_warning_on, EV_WARNING_ON, process_warning_on},
  {check_warning_off, EV_WARNING_OFF, process_warning_off},
  {check_backpedal_on, EV_BACKPEDAL, process_backpedal}
};

static uint16_t get_substate(uint16_t st) {
  return (g_state & st);
}

static void set_substate(uint16_t st) {
  g_state |= st;
}

static void remove_substate(uint16_t st) {
  g_state &= ~st;
}

// Gets called from the main loop, reads all senses and puts them in the event queue.
// The complete queue is handled (emptied) in each main loop iteration in a FIFO way, so better
// put the higher priority events at the top, i.e. claxon sound is more important then indicator light.
static void dispatch_events() {
  check_brake();
  check_claxon();
  check_ri();
  check_li();
  check_light();
  check_pilot();
  check_ign();
  check_settle();
  check_sound();
  check_alarm();
  check_alarm_settle();
  check_alarm_trigger();
  check_battery();
  check_warning();
  check_sleep();
  if (g_settings.backpedal) {
    check_backpedal();
  }
}

uint8_t check_battery_read() {
  return ((g_state & ~(ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_SLEEP)) == g_state);
}

uint8_t check_ri_on() {
  return (((g_state & (ST_NORMAL | ST_RI | ST_CLAXON | ST_BRAKE | ST_LIGHT | ST_PILOT)) == g_state) &&
        ((g_state & ~(ST_LI | ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_WARNING | ST_SLEEP)) == g_state));
}

uint8_t check_li_on() {
  return (((g_state & (ST_NORMAL | ST_LI | ST_CLAXON | ST_BRAKE | ST_LIGHT | ST_PILOT)) == g_state) &&
        ((g_state & ~(ST_RI | ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_WARNING | ST_SLEEP)) == g_state));
}

uint8_t check_brake_on() {
  return (((g_state & (ST_NORMAL | ST_RI | ST_LI | ST_CLAXON | ST_LIGHT | ST_PILOT | ST_WARNING)) == g_state) &&
        ((g_state & ~(ST_BRAKE | ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_SLEEP)) == g_state));
}

uint8_t check_warning_on() {
  // warning blinks override RI/LI
  return (((g_state & (ST_NORMAL | ST_WARNING | ST_RI | ST_LI | ST_CLAXON | ST_LIGHT | ST_PILOT | ST_BRAKE )) == g_state) &&
        ((g_state & ~(ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_SLEEP)) == g_state));
}

uint8_t check_pilot_on() {
  return (((g_state & (ST_NORMAL | ST_RI | ST_LI | ST_CLAXON | ST_LIGHT | ST_BRAKE | ST_WARNING)) == g_state) &&
        ((g_state & ~(ST_PILOT | ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_SLEEP)) == g_state));
}

uint8_t check_light_on() {
  return (((g_state & (ST_NORMAL | ST_RI | ST_LI | ST_CLAXON | ST_PILOT | ST_BRAKE | ST_WARNING)) == g_state) &&
        ((g_state & ~(ST_LIGHT | ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_SLEEP)) == g_state));
}

uint8_t check_backpedal_on() {
  return (((g_state & (ST_NORMAL | ST_LIGHT | ST_CLAXON | ST_PILOT | ST_BRAKE )) == g_state) &&
        ((g_state & ~(ST_WARNING | ST_RI | ST_LI | ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_SLEEP)) == g_state));
}

uint8_t check_claxon_on() {
  return (((g_state & (ST_NORMAL | ST_RI | ST_LI | ST_BRAKE | ST_LIGHT | ST_PILOT | ST_WARNING)) == g_state) &&
        ((g_state & ~(ST_CLAXON | ST_ALARM | ST_ALARM_SETTLE | ST_SETTLE | ST_SLEEP)) == g_state));
}

uint8_t check_ign_on() {
  return ((g_state & (ST_SLEEP | ST_ALARM_SETTLE | ST_ALARM)) == g_state);
}

uint8_t check_alarm_on() {
  //BUG: change this..
  return ((g_state & ST_SLEEP) == g_state);
}

uint8_t check_ri_off() {
 return ((ST_RI & g_state) == ST_RI);
}

uint8_t check_li_off() {
 return ((ST_LI & g_state) == ST_LI);
}

uint8_t check_claxon_off() {
 return ((ST_CLAXON & g_state) == ST_CLAXON);
}

uint8_t check_pilot_off() {
 return ((ST_PILOT & g_state) == ST_PILOT);
}

uint8_t check_brake_off() {
 return ((ST_BRAKE & g_state) == ST_BRAKE);
}

uint8_t check_warning_off() {
 return ((ST_WARNING & g_state) == ST_WARNING);
}

uint8_t check_light_off() {
 return ((ST_LIGHT & g_state) == ST_LIGHT);
}

uint8_t check_ign_off() {
  return ((g_state & ~(ST_SLEEP | ST_ALARM | ST_ALARM_SETTLE)) == g_state);
}

uint8_t check_alarm_off() {
  // alarm can't be switched off in alarm state, only explicitly in alarm_settle mode
  // or automatically when ignition is switched on
  return ((g_state & (ST_ALARM_SETTLE)) == g_state);
}

void check_ri() {
  // pin senses
  if ((g_senses & FLAG_SENSE_LIGHT_RI) == FLAG_SENSE_LIGHT_RI) {
    if (~STATUS_C90_SENSE_LIGHT_RI) { // low, RI on
      set_event(EV_RI_ON);
    }
    if (STATUS_C90_SENSE_LIGHT_RI) { // RI off
      set_event(EV_RI_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_LIGHT_RI) == FLAG_SENSE_LIGHT_RI) {
    if ((g_dyn_senses_status & FLAG_SENSE_LIGHT_RI) == FLAG_SENSE_LIGHT_RI) { // high, RI off
      set_event(EV_RI_OFF);
    } else {
      set_event(EV_RI_ON); // RI on
    }
  }
}

void check_li() {
  // pin senses
  if ((g_senses & FLAG_SENSE_LIGHT_LI) == FLAG_SENSE_LIGHT_LI) {
    if (~STATUS_C90_SENSE_LIGHT_LI) { // low, LI on
      set_event(EV_LI_ON);
    }
    if (STATUS_C90_SENSE_LIGHT_LI) { // LI off
      set_event(EV_LI_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_LIGHT_LI) == FLAG_SENSE_LIGHT_LI) {
    if ((g_dyn_senses_status & FLAG_SENSE_LIGHT_LI) == FLAG_SENSE_LIGHT_LI) { // high, LI off
      set_event(EV_LI_OFF);
    } else {
      set_event(EV_LI_ON); // LI on
    }
  }
}

void check_claxon() {
  // pin senses
  if ((g_senses & FLAG_SENSE_CLAXON) == FLAG_SENSE_CLAXON) {
    if (~STATUS_C90_SENSE_CLAXON) {
      set_event(EV_CLAXON_ON);
    }
    if (STATUS_C90_SENSE_CLAXON) {
      set_event(EV_CLAXON_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_CLAXON) == FLAG_SENSE_CLAXON) {
    if ((g_dyn_senses_status & FLAG_SENSE_CLAXON) == FLAG_SENSE_CLAXON) {
      set_event(EV_CLAXON_OFF);
    } else {
      set_event(EV_CLAXON_ON);
    }
  }
}

void check_pilot() {
  // pin senses
  if ((g_senses & FLAG_SENSE_PILOT) == FLAG_SENSE_PILOT) {
    if (~STATUS_C90_SENSE_PILOT) {
      set_event(EV_PILOT_ON);
    }
    if (STATUS_C90_SENSE_PILOT) {
      set_event(EV_PILOT_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_PILOT) == FLAG_SENSE_PILOT) {
    if ((g_dyn_senses_status & FLAG_SENSE_PILOT) == FLAG_SENSE_PILOT) {
      set_event(EV_PILOT_OFF);
    } else {
      set_event(EV_PILOT_ON);
    }
  }
}

void check_light() {
  // pin senses
  if ((g_senses & FLAG_SENSE_LIGHT) == FLAG_SENSE_LIGHT) {
    if (~STATUS_C90_SENSE_LIGHT) {
      set_event(EV_LIGHT_ON);
    }
    if (STATUS_C90_SENSE_LIGHT) {
      set_event(EV_LIGHT_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_LIGHT) == FLAG_SENSE_LIGHT) {
    if ((g_dyn_senses_status & FLAG_SENSE_LIGHT) == FLAG_SENSE_LIGHT) {
      set_event(EV_LIGHT_OFF);
    } else {
      set_event(EV_LIGHT_ON);
    }
  }
}

void check_brake() {
  // pin senses
  if ((g_senses & FLAG_SENSE_BRAKE) == FLAG_SENSE_BRAKE) {
    if (~STATUS_C90_SENSE_BRAKE) {
      set_event(EV_BRAKE_ON);
    }
    if (STATUS_C90_SENSE_BRAKE) {
      set_event(EV_BRAKE_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_BRAKE) == FLAG_SENSE_BRAKE) {
    if ((g_dyn_senses_status & FLAG_SENSE_BRAKE) == FLAG_SENSE_BRAKE) {
      set_event(EV_BRAKE_OFF);
    } else {
      set_event(EV_BRAKE_ON);
    }
  }
}

void check_ign() {
  // pin senses
  if ((g_senses & FLAG_SENSE_IGN) == FLAG_SENSE_IGN) {
    if (~STATUS_C90_SENSE_IGN) {
      set_event(EV_IGN_ON);
    }
    if (STATUS_C90_SENSE_IGN) {
      set_event(EV_IGN_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_IGN) == FLAG_SENSE_IGN) {
    if ((g_dyn_senses_status & FLAG_SENSE_IGN) == FLAG_SENSE_IGN) {
      set_event(EV_IGN_OFF);
    } else {
      set_event(EV_IGN_ON);
    }
  }
}

void check_alarm() {
  // pin senses
  if ((g_senses & FLAG_SENSE_ALARM) == FLAG_SENSE_ALARM) {
    if (~STATUS_C90_SENSE_ALARM) {
      set_event(EV_ALARM_ON);
    }
    if (STATUS_C90_SENSE_ALARM) {
      set_event(EV_ALARM_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_ALARM) == FLAG_SENSE_ALARM) {
    if ((g_dyn_senses_status & FLAG_SENSE_ALARM) == FLAG_SENSE_ALARM) {
      set_event(EV_ALARM_OFF);
    } else {
      set_event(EV_ALARM_ON);
    }
  }
}

void check_battery() {
  if (check_battery_read()) {
    set_event(EV_READ_BATT);
  }
}

void check_sleep() {
  if (g_state == ST_SLEEP) {
    if (g_sleep_counter == 0) {
      sleep_now();
    }
  } else {
    g_sleep_counter = g_settings.deep_sleep_counter;
  }
}

void check_backpedal() {
  if (check_backpedal_on()) {
    set_event(EV_BACKPEDAL);
  }
}

void check_warning() {
  // pin senses
  if ((g_senses & FLAG_SENSE_WARNING) == FLAG_SENSE_WARNING) {
    if (~STATUS_C90_SENSE_WARNING) {
      set_event(EV_WARNING_ON);
    }
    if (STATUS_C90_SENSE_WARNING) {
      set_event(EV_WARNING_OFF);
    }
  }
  // dynamic senses
  if ((g_dyn_senses & FLAG_SENSE_WARNING) == FLAG_SENSE_WARNING) {
    if ((g_dyn_senses_status & FLAG_SENSE_WARNING) == FLAG_SENSE_WARNING) {
      set_event(EV_WARNING_OFF);
    } else {
      set_event(EV_WARNING_ON);
    }
  }
}

void check_settle() {
  if (g_state == ST_SETTLE) {
    if (!FLAG_SETTLE) { // first time in settle mode
      uint8_t song_idx = g_settings.startup_sound;
      FLAG_SETTLE = 1;
      // activate all lights
      PORT_C90_LIGHT_INDICATOR_COCKPIT |= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
      PORT_C90_LIGHT_STATUS_COCKPIT |= (1 << PIN_C90_LIGHT_STATUS_COCKPIT);
      PORT_C90_LIGHT_RI_F |= (1 << PIN_C90_LIGHT_RI_F);
      PORT_C90_LIGHT_RI_B |= (1 << PIN_C90_LIGHT_RI_B);
      PORT_C90_LIGHT_LI_F |= (1 << PIN_C90_LIGHT_LI_F);
      PORT_C90_LIGHT_LI_B |= (1 << PIN_C90_LIGHT_LI_B);
      PORT_C90_BRAKE |= (1 << PIN_C90_BRAKE);
      PORT_C90_PILOT |= (1 << PIN_C90_PILOT);
      PORT_C90_LIGHT |= (1 << PIN_C90_LIGHT);
      PORT_C90_CLAXON &= ~(1 << PIN_C90_CLAXON); // silence a possible claxon from the alarm
      // check voltage and activate according sound
      g_music_duration = 0;
      // check for random startup song
      if (song_idx == 0) {
        srand(g_adc_voltage[0] + g_adc_voltage[1] + g_adc_voltage[2] + g_adc_voltage[3]); // kinda random seed
        song_idx = 2 + (uint8_t)(rand() / (RAND_MAX / 5));
      }
      g_current_music = (uint16_t*)pgm_read_word(&g_music[song_idx-1]);
      g_selected_music = (uint16_t*)pgm_read_word(&g_music[song_idx-1]);
      FLAG_MUSIC = 1;
      g_current_settle_time = g_settings.settle_time;
    }
    if (!g_current_settle_time) { // settle time is over
      FLAG_SETTLE = 0; // reset settle
      g_state = ST_NORMAL;
      PORT_C90_LIGHT_INDICATOR_COCKPIT &= ~(1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
      PORT_C90_LIGHT_STATUS_COCKPIT &= ~(1 << PIN_C90_LIGHT_STATUS_COCKPIT);
      PORT_C90_LIGHT_RI_F &= ~(1 << PIN_C90_LIGHT_RI_F);
      PORT_C90_LIGHT_RI_B &= ~(1 << PIN_C90_LIGHT_RI_B);
      PORT_C90_LIGHT_LI_F &= ~(1 << PIN_C90_LIGHT_LI_F);
      PORT_C90_LIGHT_LI_B &= ~(1 << PIN_C90_LIGHT_LI_B);
      PORT_C90_CLAXON &= ~(1 << PIN_C90_CLAXON);
      PORT_C90_BRAKE &= ~(1 << PIN_C90_BRAKE);
      PORT_C90_PILOT &= ~(1 << PIN_C90_PILOT);
      PORT_C90_LIGHT &= ~(1 << PIN_C90_LIGHT);
    }
  }
}

void check_alarm_settle() {
  if (g_state == ST_ALARM_SETTLE) {
    if (!FLAG_ALARM_SETTLE) { // first time in alarm settle mode
      uint8_t song_idx = g_settings.alarm_sound;
      // check for random startup song
      if (song_idx == 0) {
        srand(g_adc_voltage[0] + g_adc_voltage[1] + g_adc_voltage[2] + g_adc_voltage[3]); // kinda random seed
        song_idx = 1 + (uint8_t)(rand() / (RAND_MAX / 6));
      }
      FLAG_ALARM_SETTLE = 1;
      // activate indicators
      PORT_C90_LIGHT_INDICATOR_COCKPIT |= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
      PORT_C90_LIGHT_RI_F |= (1 << PIN_C90_LIGHT_RI_F);
      PORT_C90_LIGHT_RI_B |= (1 << PIN_C90_LIGHT_RI_B);
      PORT_C90_LIGHT_LI_F |= (1 << PIN_C90_LIGHT_LI_F);
      PORT_C90_LIGHT_LI_B |= (1 << PIN_C90_LIGHT_LI_B);
      g_music_duration = 0;
      g_current_music = (uint16_t*)pgm_read_word(&g_music[song_idx-1]);
      g_selected_music = (uint16_t*)pgm_read_word(&g_music[song_idx-1]);
      FLAG_MUSIC = 1;
      g_current_alarm_settle_time = g_settings.alarm_settle_time;
      g_alarm_snapshot[0] = 0;
      g_alarm_snapshot[1] = 0;
      g_alarm_snapshot[2] = 0;
    }
    // save the AVERAGE accelerometer values as the alarm trigger reference during the settlement
    if (g_alarm_snapshot[0]) {
      g_alarm_snapshot[0] = ((g_alarm_snapshot[0] + g_adc_voltage[1])/2);
    } else {
      g_alarm_snapshot[0] = g_adc_voltage[1];
    }
    if (g_alarm_snapshot[1]) {
      g_alarm_snapshot[1] = ((g_alarm_snapshot[1] + g_adc_voltage[2])/2);
    } else {
      g_alarm_snapshot[1] = g_adc_voltage[2];
    }
    if (g_alarm_snapshot[2]) {
      g_alarm_snapshot[2] = ((g_alarm_snapshot[2] + g_adc_voltage[3])/2);
    } else {
      g_alarm_snapshot[2] = g_adc_voltage[3];
    }
    // alarm settle time is over: only start alarm with valid accelerometer voltages
    if ((!g_current_alarm_settle_time) && ((g_alarm_snapshot[0] > g_settings.alarm_thres_min) && (g_alarm_snapshot[0] < g_settings.alarm_thres_max)) &&
       ((g_alarm_snapshot[1] > g_settings.alarm_thres_min) && (g_alarm_snapshot[1] < g_settings.alarm_thres_max)) &&
       ((g_alarm_snapshot[2] > g_settings.alarm_thres_min) && (g_alarm_snapshot[2] < g_settings.alarm_thres_max))) {
      // prepare for alarm mode
      FLAG_ALARM_SETTLE = 0;
      all_lights_off();
      g_alarm_counter = g_settings.alarm_counter;
      g_trigger_counter = 0;
      g_state = ST_ALARM;
    }
  }
}

// triggers alarm if movement (voltage change) is high enough 
// 1.5g setting:  Sensitivity: 800 mV/g  -1g = 850mV  0g = 1650mV 1g = 2450mV
// 1 degree tilt is about 9mv, so by default: if the voltage differs 90mv (~10 degrees), for a certain period, the alarm will trigger
void check_alarm_trigger() {
  if (g_state == ST_ALARM) {
    if (FLAG_ALARM_TRIGGER) {
      if (FLAG_ALARM_BLINK) {
        FLAG_ALARM_BLINK = 0;
        PORT_C90_LIGHT_RI_F ^= (1 << PIN_C90_LIGHT_RI_F);
        PORT_C90_LIGHT_RI_B ^= (1 << PIN_C90_LIGHT_RI_B);
        PORT_C90_LIGHT_LI_F ^= (1 << PIN_C90_LIGHT_LI_F);
        PORT_C90_LIGHT_LI_B ^= (1 << PIN_C90_LIGHT_LI_B);
        PORT_C90_LIGHT_INDICATOR_COCKPIT ^= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
        PORT_C90_CLAXON ^= (1 << PIN_C90_CLAXON);
        g_alarm_counter--;
        if (!g_alarm_counter) {
          all_lights_off();
          FLAG_ALARM_TRIGGER = 0; // reset alarm trigger
          g_state = ST_ALARM_SETTLE; // go back to new alarm settle mode
          FLAG_ALARM_SETTLE = 0; // reset alarm settle
        }
      }
    } else if (FLAG_ALARM_BLINK) {
      FLAG_ALARM_BLINK = 0;
      if (g_alarm_blink_counter % 4 == 0) { // blink for 1 tick every timer1 4 counts
        PORT_C90_LIGHT_INDICATOR_COCKPIT |= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
      } else {
        PORT_C90_LIGHT_INDICATOR_COCKPIT &= ~(1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
      }
      if ((abs(g_adc_voltage[1] - g_alarm_snapshot[0]) > g_settings.alarm_trigger) ||
       (abs(g_adc_voltage[2] - g_alarm_snapshot[1]) > g_settings.alarm_trigger) || 
       (abs(g_adc_voltage[3] - g_alarm_snapshot[2]) > g_settings.alarm_trigger)) {
        g_trigger_counter++;
        if (g_trigger_counter >= g_settings.alarm_trigger_counter) {
          FLAG_ALARM_TRIGGER = 1;
        }
      }
    }
  }
}

uint8_t get_event() {
  uint8_t ev;
  if (g_buffer_tail != g_buffer_head) { // not empty
    ev = g_event_buffer[g_buffer_tail];
    g_buffer_tail++;
    if (g_buffer_tail >= C90_EVENT_BUFFER_SIZE) {
      g_buffer_tail = 0;
    }
    return ev;
  } else {
    return EV_VOID; // just keep returning a void event
  }
}

void set_event(uint8_t ev) {
  g_event_buffer[g_buffer_head] = ev; // insert event at head position
  g_buffer_head++; // advance head
  if (g_buffer_head >= C90_EVENT_BUFFER_SIZE) {
    g_buffer_head = 0; // cycle back to start
  }
  if (g_buffer_head == g_buffer_tail) {
    g_buffer_tail++; // also move tail, basically destroying an event to make space
  }
  if (g_buffer_tail >= C90_EVENT_BUFFER_SIZE) {
    g_buffer_tail = 0; // cycle back to start
  }
}

void process_ri_off() {
  if (FLAG_DEBOUNCE_RI) { // check for 3 positives based on 5ms timer
    g_ri_debounce = (g_ri_debounce << 1);
    FLAG_DEBOUNCE_RI = 0; // reset to wait for next timer event
    if (((g_ri_debounce & 0b00000111) << 5) == 0) {
      remove_substate(ST_RI); // debouncing done, remove state
      // disable indicators
      PORT_C90_LIGHT_RI_F &= ~(1 << PIN_C90_LIGHT_RI_F); // front
      PORT_C90_LIGHT_RI_B &= ~(1 << PIN_C90_LIGHT_RI_B); // rear
      if (g_settings.indicator_sound) {
        PORT_C90_BUZZER &= ~(1 << PIN_C90_BUZZER);
      }
      PORT_C90_LIGHT_INDICATOR_COCKPIT &= ~(1 << PIN_C90_LIGHT_INDICATOR_COCKPIT); // cockpit
    }
  }
}

void process_ri_on() {
  if (FLAG_DEBOUNCE_RI) { // check for 3 positives based on 5ms timer
    g_ri_debounce = (g_ri_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_RI = 0; // reset to wait for next timer event
    if (((g_ri_debounce & 0b00000111) == 0b00000111) && (FLAG_BLINK_RI)) { // blink indicators
      PORT_C90_LIGHT_RI_F ^= (1 << PIN_C90_LIGHT_RI_F);
      PORT_C90_LIGHT_RI_B ^= (1 << PIN_C90_LIGHT_RI_B);
      PORT_C90_LIGHT_INDICATOR_COCKPIT ^= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
      if (g_settings.indicator_sound) {
        PORT_C90_BUZZER ^= (1 << PIN_C90_BUZZER);
      }
      FLAG_BLINK_RI = 0; // reset to wait for next timer event
      if (!get_substate(ST_RI)) { // first time to blink
        TCNT1 = 0; // reset timer so the blink starts in an even pace
        set_substate(ST_RI);
      }
    }
  }
}

void process_li_off() {
  if (FLAG_DEBOUNCE_LI) { // check for 3 positives based on 5ms timer
    g_li_debounce = (g_li_debounce << 1);
    FLAG_DEBOUNCE_LI = 0; // reset to wait for next timer event
    if (((g_li_debounce & 0b00000111) << 5) == 0) {
      remove_substate(ST_LI); // debouncing done, remove state
      // disable indicators
      PORT_C90_LIGHT_LI_F &= ~(1 << PIN_C90_LIGHT_LI_F); // front
      PORT_C90_LIGHT_LI_B &= ~(1 << PIN_C90_LIGHT_LI_B); // rear
      if (g_settings.indicator_sound) {
        PORT_C90_BUZZER &= ~(1 << PIN_C90_BUZZER);
      }
      PORT_C90_LIGHT_INDICATOR_COCKPIT &= ~(1 << PIN_C90_LIGHT_INDICATOR_COCKPIT); // cockpit
    }
  }
}

void process_li_on() {
  if (FLAG_DEBOUNCE_LI) { // check for 3 positives based on 5ms timer
    g_li_debounce = (g_li_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_LI = 0; // reset to wait for next timer event
    if (((g_li_debounce & 0b00000111) == 0b00000111) && (FLAG_BLINK_LI)) { // blink indicators
      PORT_C90_LIGHT_LI_F ^= (1 << PIN_C90_LIGHT_LI_F); // front
      PORT_C90_LIGHT_LI_B ^= (1 << PIN_C90_LIGHT_LI_B); // rear
      PORT_C90_LIGHT_INDICATOR_COCKPIT ^= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT); // cockpit
      if (g_settings.indicator_sound) {
        PORT_C90_BUZZER ^= (1 << PIN_C90_BUZZER);
      }
      FLAG_BLINK_LI = 0; // reset to wait for next timer event
      if (!get_substate(ST_LI)) { // first time to blink
        TCNT1 = 0; // set 0.5 sec timer counter explicitly to 0, so the first blink happens exactly 0.5 secs later
        set_substate(ST_LI);
      }
    }
  }
}

void process_claxon_on() {
  if (FLAG_DEBOUNCE_CLAXON) { // check for 3 positives based on 5ms timer
    g_claxon_debounce = (g_claxon_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_CLAXON = 0; // reset to wait for next timer event
    if ((g_claxon_debounce & 0b00000111) == 0b00000111) { // start claxon
      PORT_C90_CLAXON |= (1 << PIN_C90_CLAXON);
      if (!get_substate(ST_CLAXON)) {
        set_substate(ST_CLAXON);
      }
    }
  }
}

void process_claxon_off() {
  if (FLAG_DEBOUNCE_CLAXON) { // check for 3 positives based on 5ms timer
    g_claxon_debounce = (g_claxon_debounce << 1);
    FLAG_DEBOUNCE_CLAXON = 0; // reset to wait for next timer event
    if (((g_claxon_debounce & 0b00000111) << 5) == 0) {
      remove_substate(ST_CLAXON); // debouncing done, remove state
      PORT_C90_CLAXON &= ~(1 << PIN_C90_CLAXON);
    }
  }
}

void process_warning_on() {
  if (FLAG_DEBOUNCE_WARNING) { // check for 3 positives based on 5ms timer
    g_warning_debounce = (g_warning_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_WARNING = 0; // reset to wait for next timer event
    if (((g_warning_debounce & 0b00000111) == 0b00000111) && (FLAG_BLINK_WARNING)) { // blink indicators
      PORT_C90_LIGHT_RI_F ^= (1 << PIN_C90_LIGHT_RI_F);
      PORT_C90_LIGHT_RI_B ^= (1 << PIN_C90_LIGHT_RI_B);
      PORT_C90_LIGHT_LI_F ^= (1 << PIN_C90_LIGHT_LI_F);
      PORT_C90_LIGHT_LI_B ^= (1 << PIN_C90_LIGHT_LI_B);
      PORT_C90_LIGHT_INDICATOR_COCKPIT ^= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
      if (g_settings.indicator_sound) {
        PORT_C90_BUZZER ^= (1 << PIN_C90_BUZZER);
      }
      FLAG_BLINK_WARNING = 0; // reset to wait for next timer event
      if (!get_substate(ST_WARNING)) { // first time to blink
        TCNT1 = 0; // reset timer so the blink starts in an even pace
        // initial ON for indicators to prevent a  RI/LI ON messing with the synchronization, i.e. warning lights override indicator switches
        PORT_C90_LIGHT_RI_F |= (1 << PIN_C90_LIGHT_RI_F);
        PORT_C90_LIGHT_RI_B |= (1 << PIN_C90_LIGHT_RI_B);
        PORT_C90_LIGHT_LI_F |= (1 << PIN_C90_LIGHT_LI_F);
        PORT_C90_LIGHT_LI_B |= (1 << PIN_C90_LIGHT_LI_B);
        PORT_C90_LIGHT_INDICATOR_COCKPIT |= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
        set_substate(ST_WARNING);
      }
    }
  }
}

void process_warning_off() {
  if (FLAG_DEBOUNCE_WARNING) { // check for 3 positives based on 5ms timer
    g_warning_debounce = (g_warning_debounce << 1);
    FLAG_DEBOUNCE_WARNING = 0; // reset to wait for next timer event
    if (((g_warning_debounce & 0b00000111) << 5) == 0) {
      remove_substate(ST_WARNING); // debouncing done, remove state
      // disable indicators
      PORT_C90_LIGHT_LI_F &= ~(1 << PIN_C90_LIGHT_LI_F); // front
      PORT_C90_LIGHT_LI_B &= ~(1 << PIN_C90_LIGHT_LI_B); // rear
      PORT_C90_LIGHT_RI_F &= ~(1 << PIN_C90_LIGHT_RI_F); // front
      PORT_C90_LIGHT_RI_B &= ~(1 << PIN_C90_LIGHT_RI_B); // rear
      if (g_settings.indicator_sound) {
        PORT_C90_BUZZER &= ~(1 << PIN_C90_BUZZER);
      }
      PORT_C90_LIGHT_INDICATOR_COCKPIT &= ~(1 << PIN_C90_LIGHT_INDICATOR_COCKPIT); // cockpit
    }
  }
}

// check for an acceleration of 25cm/s, 20mV change in the negative Y-axis
void process_backpedal() {
  if (FLAG_READ_BACKPEDAL) {
    FLAG_READ_BACKPEDAL = 0; // reset to wait for next timer event
    if ((g_adc_voltage[2] < g_settings.backpedal_thres_min) && FLAG_BLINK_BACKPEDAL) {
      FLAG_BACKPEDAL = 1;
      PORT_C90_LIGHT_RI_F ^= (1 << PIN_C90_LIGHT_RI_F);
      PORT_C90_LIGHT_RI_B ^= (1 << PIN_C90_LIGHT_RI_B);
      PORT_C90_LIGHT_LI_F ^= (1 << PIN_C90_LIGHT_LI_F);
      PORT_C90_LIGHT_LI_B ^= (1 << PIN_C90_LIGHT_LI_B);
      PORT_C90_LIGHT_INDICATOR_COCKPIT ^= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
      if (g_settings.indicator_sound) {
        PORT_C90_BUZZER ^= (1 << PIN_C90_BUZZER);
      }
      FLAG_BLINK_BACKPEDAL = 0;
    } else if ((g_adc_voltage[2] >= g_settings.backpedal_thres_min) && FLAG_BACKPEDAL && FLAG_BLINK_BACKPEDAL) {
      FLAG_BACKPEDAL = 0;
      PORT_C90_LIGHT_LI_F &= ~(1 << PIN_C90_LIGHT_LI_F); // front
      PORT_C90_LIGHT_LI_B &= ~(1 << PIN_C90_LIGHT_LI_B); // rear
      PORT_C90_LIGHT_RI_F &= ~(1 << PIN_C90_LIGHT_RI_F); // front
      PORT_C90_LIGHT_RI_B &= ~(1 << PIN_C90_LIGHT_RI_B); // rear
      if (g_settings.indicator_sound) {
        PORT_C90_BUZZER &= ~(1 << PIN_C90_BUZZER);
      }
      PORT_C90_LIGHT_INDICATOR_COCKPIT &= ~(1 << PIN_C90_LIGHT_INDICATOR_COCKPIT); // cockpit
    }
  }
}

void process_ign_on() {
  if (FLAG_DEBOUNCE_IGN) { // check for 3 positives based on 5ms timer
    g_ign_debounce= (g_ign_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_IGN = 0; // reset to wait for next timer event
    if ((g_ign_debounce & 0b00000111) == 0b00000111) {
      g_state = ST_SETTLE;
    }
  }
}

void process_ign_off() {
  if (FLAG_DEBOUNCE_IGN) { // check for 3 positives based on 5ms timer
    g_ign_debounce = (g_ign_debounce << 1);
    FLAG_DEBOUNCE_IGN = 0; // reset to wait for next timer event
    if (((g_ign_debounce & 0b00000111) << 5) == 0) {
      all_lights_off();
      PORT_C90_BUZZER &= ~(1 << PIN_C90_BUZZER); // buzzer off
      FLAG_SETTLE = 0; // reset settle
      g_state = ST_SLEEP;
    }
  }
}

void process_brake_on() {
  if (FLAG_DEBOUNCE_BRAKE) { // check for 3 positives based on 5ms timer
    g_brake_debounce = (g_brake_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_BRAKE = 0; // reset to wait for next timer event
    if ((g_brake_debounce & 0b00000111) == 0b00000111) { // start brake
      PORT_C90_BRAKE |= (1 << PIN_C90_BRAKE);
      if (!get_substate(ST_BRAKE)) {
        set_substate(ST_BRAKE);
      }
    }
  }
}

void process_brake_off() {
  if (FLAG_DEBOUNCE_BRAKE) { // check for 3 positives based on 5ms timer
    g_brake_debounce = (g_brake_debounce << 1);
    FLAG_DEBOUNCE_BRAKE = 0; // reset to wait for next timer event
    if (((g_brake_debounce & 0b00000111) << 5) == 0) {
      remove_substate(ST_BRAKE); // debouncing done, remove state
      PORT_C90_BRAKE &= ~(1 << PIN_C90_BRAKE); // disable brake
    }
  }
}

void process_pilot_on() {
  if (FLAG_DEBOUNCE_PILOT) { // check for 3 positives based on 5ms timer
    g_pilot_debounce = (g_pilot_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_PILOT = 0; // reset to wait for next timer event
    if ((g_pilot_debounce & 0b00000111) == 0b00000111) { // start pilot
      PORT_C90_PILOT |= (1 << PIN_C90_PILOT);
      if (!get_substate(ST_PILOT)) {
        set_substate(ST_PILOT);
      }
    }
  }
}

void process_pilot_off() {
  if (FLAG_DEBOUNCE_PILOT) { // check for 3 positives based on 5ms timer
    g_pilot_debounce = (g_pilot_debounce << 1);
    FLAG_DEBOUNCE_PILOT = 0; // reset to wait for next timer event
    if (((g_pilot_debounce & 0b00000111) << 5) == 0) {
      remove_substate(ST_PILOT); // debouncing done, remove state
      PORT_C90_PILOT &= ~(1 << PIN_C90_PILOT); // disable pilot
    }
  }
}

void process_light_on() {
  if (FLAG_DEBOUNCE_LIGHT) { // check for 3 positives based on 5ms timer
    g_light_debounce = (g_light_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_LIGHT = 0; // reset to wait for next timer event
    if ((g_light_debounce & 0b00000111) == 0b00000111) { // start light
      PORT_C90_LIGHT |= (1 << PIN_C90_LIGHT);
      if (!get_substate(ST_LIGHT)) {
        set_substate(ST_LIGHT);
      }
    }
  }
}

void process_light_off() {
  if (FLAG_DEBOUNCE_LIGHT) { // check for 3 positives based on 5ms timer
    g_light_debounce = (g_light_debounce << 1);
    FLAG_DEBOUNCE_LIGHT = 0; // reset to wait for next timer event
    if (((g_light_debounce & 0b00000111) << 5) == 0) {
      remove_substate(ST_LIGHT); // debouncing done, remove state
      PORT_C90_LIGHT &= ~(1 << PIN_C90_LIGHT); // disable light
    }
  }
}

void process_alarm_off() {
  if (FLAG_DEBOUNCE_ALARM) { // check for 3 positives based on 5ms timer
    g_alarm_debounce = (g_alarm_debounce << 1);
    FLAG_DEBOUNCE_ALARM = 0; // reset to wait for next timer event
    if (((g_alarm_debounce & 0b00000111) << 5) == 0) {
      all_lights_off();
      FLAG_ALARM_SETTLE = 0; // reset alarm settle
      g_state = ST_SLEEP;
    }
  }
}

void process_alarm_on() {
  if (FLAG_DEBOUNCE_ALARM) { // check for 3 positives based on 5ms timer
    g_alarm_debounce = (g_alarm_debounce << 1) | 0x01;
    FLAG_DEBOUNCE_ALARM = 0; // reset to wait for next timer event
    if ((g_alarm_debounce & 0b00000111) == 0b00000111) { // start alarm
      FLAG_ALARM_SETTLE = 0; // reset alarm settle
      g_state = ST_ALARM_SETTLE;
    }
  }
}

// check if battery voltage is OK: between 12.0 and 16.0 volts
// Vref = 5v, if Vref isnt exactly 5.00v, but a bit off, tweak C90_OFFSET_ADC_READING
// for 12v battery readout: my voltage divider ratio: 2.2K -- 1K, so Vmeasure ==  0.3125 * Vin
// 10bit ADC value 0-1023 == Vmeasure/(5/1024)
//test
void process_battery() {
  if (FLAG_READ_BATT) {
    if (g_adc_voltage[0] < 768) { // battery too low
      // slow blink cockpit status led
      if (g_battery_blink_counter % 100 == 0) { 
        PORT_C90_LIGHT_STATUS_COCKPIT ^= (1 << PIN_C90_LIGHT_STATUS_COCKPIT);
      }
    } else if (g_adc_voltage[0] > 1000) { // battery too high
      // fast blink cockpit status led
      if (g_battery_blink_counter % 50 == 0) { 
        PORT_C90_LIGHT_STATUS_COCKPIT ^= (1 << PIN_C90_LIGHT_STATUS_COCKPIT);
      }
    } else { // battery Ok
      PORT_C90_LIGHT_STATUS_COCKPIT |= (1 << PIN_C90_LIGHT_STATUS_COCKPIT);
    }
    FLAG_READ_BATT = 0;
  }
}

// all lighting pins go low
void all_lights_off() {
  PORT_C90_LIGHT_INDICATOR_COCKPIT &= ~(1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
  PORT_C90_LIGHT_STATUS_COCKPIT &= ~(1 << PIN_C90_LIGHT_STATUS_COCKPIT);
  PORT_C90_LIGHT_RI_F &= ~(1 << PIN_C90_LIGHT_RI_F);
  PORT_C90_LIGHT_RI_B &= ~(1 << PIN_C90_LIGHT_RI_B);
  PORT_C90_LIGHT_LI_F &= ~(1 << PIN_C90_LIGHT_LI_F);
  PORT_C90_LIGHT_LI_B &= ~(1 << PIN_C90_LIGHT_LI_B);
  PORT_C90_BRAKE &= ~(1 << PIN_C90_BRAKE);
  PORT_C90_PILOT &= ~(1 << PIN_C90_PILOT);
  PORT_C90_LIGHT &= ~(1 << PIN_C90_LIGHT);
  PORT_C90_CLAXON &= ~(1 << PIN_C90_CLAXON); // claxon too
}

void reset_ports() {
  // sense pins: as input, set high and activate internal pullups
  DDR_C90_SENSE_LIGHT_RI &= ~(1 << PIN_C90_SENSE_LIGHT_RI);
  PORT_C90_SENSE_LIGHT_RI |= (1 << PIN_C90_SENSE_LIGHT_RI);
  DDR_C90_SENSE_LIGHT_LI &= ~(1 << PIN_C90_SENSE_LIGHT_LI);
  PORT_C90_SENSE_LIGHT_LI |= (1 << PIN_C90_SENSE_LIGHT_LI);
  DDR_C90_SENSE_CLAXON &= ~(1 << PIN_C90_SENSE_CLAXON);
  PORT_C90_SENSE_CLAXON |= (1 << PIN_C90_SENSE_CLAXON);
  DDR_C90_SENSE_BRAKE &= ~(1 << PIN_C90_SENSE_BRAKE);
  PORT_C90_SENSE_BRAKE |= (1 << PIN_C90_SENSE_BRAKE);
  DDR_C90_SENSE_PILOT &= ~(1 << PIN_C90_SENSE_PILOT);
  PORT_C90_SENSE_PILOT |= (1 << PIN_C90_SENSE_PILOT);
  DDR_C90_SENSE_ALARM &= ~(1 << PIN_C90_SENSE_ALARM);
  PORT_C90_SENSE_ALARM |= (1 << PIN_C90_SENSE_ALARM);
  DDR_C90_SENSE_LIGHT &= ~(1 << PIN_C90_SENSE_LIGHT);
  PORT_C90_SENSE_LIGHT |= (1 << PIN_C90_SENSE_LIGHT);
  DDR_C90_SENSE_IGN &= ~(1 << PIN_C90_SENSE_IGN);
  PORT_C90_SENSE_IGN |= (1 << PIN_C90_SENSE_IGN);
  DDR_C90_SENSE_WARNING &= ~(1 << PIN_C90_SENSE_WARNING);
  PORT_C90_SENSE_WARNING |= (1 << PIN_C90_SENSE_WARNING);
  DDR_C90_SENSE_RESERVE &= ~(1 << PIN_C90_SENSE_RESERVE);
  PORT_C90_SENSE_RESERVE |= (1 << PIN_C90_SENSE_RESERVE);
  // relay pins: as output, set low at start
  // outputs
  DDR_C90_LIGHT_INDICATOR_COCKPIT |= (1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
  DDR_C90_LIGHT_STATUS_COCKPIT |= (1 << PIN_C90_LIGHT_STATUS_COCKPIT);
  DDR_C90_LIGHT_RI_F |= (1 << PIN_C90_LIGHT_RI_F);
  DDR_C90_LIGHT_RI_B |= (1 << PIN_C90_LIGHT_RI_B);
  DDR_C90_LIGHT_LI_F |= (1 << PIN_C90_LIGHT_LI_F);
  DDR_C90_LIGHT_LI_B |= (1 << PIN_C90_LIGHT_LI_B);
  DDR_C90_CLAXON |= (1 << PIN_C90_CLAXON);
  DDR_C90_BRAKE |= (1 << PIN_C90_BRAKE);
  DDR_C90_PILOT |= (1 << PIN_C90_PILOT);
  DDR_C90_LIGHT |= (1 << PIN_C90_LIGHT);
  // lows
  PORT_C90_LIGHT_INDICATOR_COCKPIT &= ~(1 << PIN_C90_LIGHT_INDICATOR_COCKPIT);
  PORT_C90_LIGHT_STATUS_COCKPIT &= ~(1 << PIN_C90_LIGHT_STATUS_COCKPIT);
  PORT_C90_LIGHT_RI_F &= ~(1 << PIN_C90_LIGHT_RI_F);
  PORT_C90_LIGHT_RI_B &= ~(1 << PIN_C90_LIGHT_RI_B);
  PORT_C90_LIGHT_LI_F &= ~(1 << PIN_C90_LIGHT_LI_F);
  PORT_C90_LIGHT_LI_B &= ~(1 << PIN_C90_LIGHT_LI_B);
  PORT_C90_CLAXON &= ~(1 << PIN_C90_CLAXON);
  PORT_C90_BRAKE &= ~(1 << PIN_C90_BRAKE);
  PORT_C90_PILOT &= ~(1 << PIN_C90_PILOT);
  PORT_C90_LIGHT &= ~(1 << PIN_C90_LIGHT);
  // buzzer
  DDR_C90_BUZZER |= (1 << PIN_C90_BUZZER); // output
  PORT_C90_BUZZER &= ~(1 << PIN_C90_BUZZER); // low
  // heart beat led
  DDR_C90_HEARTBEAT_LED |= (1 << PIN_C90_HEARTBEAT_LED); // output
  PORT_C90_HEARTBEAT_LED |= (1 << PIN_C90_HEARTBEAT_LED); // high
  // activate all physical senses
  enable_psenses();
}

void enable_psenses() {
  g_senses = 0b11111111;
  g_senses = ((g_senses << 8) | 0b11111111);
}

void disable_psenses() {
  g_senses = 0;
}

// setup the ADC in free running mode, so we can select the read channels ourselves
void enable_adc() {
  ADMUX = (ADMUX & 0xF8) | g_adc_read_pin; // clear the bottom 3 bits before setting new pin
  ADMUX |= (1 << REFS0); // voltage reference = AVcc
  DIDR0 = 0x0F;  // disable digital inputs for ADC0-ADC3 
  ADCSRA |= (1 << ADEN); // enable ADC
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1 ) | (1 << ADPS0); // prescaler = 128
  ADCSRA |= (1 << ADIE); // enable interrupt
  ADCSRA |= (1 << ADSC); // start a conversion
}

void disable_adc() {
  ADCSRA &= ~(1 << ADEN);
}

ISR(ADC_vect) {
  // first read ADCL, then ADCH to keep it an atomic operation
  g_adc_voltage[g_adc_read_pin] = ADCL;
  g_adc_voltage[g_adc_read_pin] = (g_adc_voltage[g_adc_read_pin] | (ADCH << 8));
  g_adc_voltage[g_adc_read_pin] += C90_OFFSET_ADC_READING;
  g_adc_read_pin++;
  if (g_adc_read_pin > 3) {
    g_adc_read_pin = 0;
  }
  ADMUX = (ADMUX & 0xF8) | g_adc_read_pin;
  ADCSRA |= (1 << ADSC); // start another conversion again
}

void start_sense_timer()
{
  // Configure timer 0 (8-bit) for CTC mode (Clear on Timer Compare)
  TCCR0B |= (1 << WGM02); // CTC Mode
  TCCR0A |= (1 << WGM01) | (0 << WGM00); // CTC Mode
  TCCR0B |= ((1 << CS00) | (1 << CS02)); // prescale 1024
  TCCR0B &= ~(1 << CS01); // prescale 1024
  TIMSK0 |= (1 << OCIE0A); // Enable CTC interrupt
  OCR0A = 97; // Set CTC compare value, approx. 5 ms -> 200 times/sec
}

void start_blink_timer()
{
  // Configure timer 1 (16-bit) for CTC mode (Clear on Timer Compare)
  TCCR1A &= ~((1 << WGM11) | (1 << WGM10)); // CTC Mode
  TCCR1B |= (1 << WGM12); // CTC Mode
  TCCR1B |= ((1 << CS10) | (1 << CS12)); // prescale 1024
  TCCR1B &= ~(1 << CS11); // prescale 1024
  TIMSK1 |= (1 << OCIE1A); // Enable Compare A interrupt
  OCR1A = g_settings.blink_speed; // Set CTC compare value, default approx. 0.5 secs
}

void start_buzzer_timer(uint8_t isr_on) {
  // Configure timer 3 (16-bit) for CTC mode (Clear on Timer Compare)
  TCCR3A &= ~((1 << WGM31) | (1 << WGM30)); // CTC Mode
  TCCR3B |= (1 << WGM32); // CTC Mode
  TCCR3B |= ((1 << CS31) | (1 << CS30)); // prescale 64
  TCCR3B &= ~(1 << CS32); // prescale 64
  if (isr_on) {
    TIMSK3 |= (1 << OCIE3A); // Enable Compare A interrupt
  }
  OCR3A = 68; // Set CTC compare value, approx. 4600hz: with a square wave that is a 2400hz tone
}

// runs every 5ms
ISR(TIMER0_COMPA_vect)
{
  FLAG_DEBOUNCE_RI = 1;
  FLAG_DEBOUNCE_LI = 1;
  FLAG_DEBOUNCE_CLAXON = 1;
  FLAG_DEBOUNCE_IGN = 1;
  FLAG_DEBOUNCE_BRAKE = 1;
  FLAG_DEBOUNCE_PILOT = 1;
  FLAG_DEBOUNCE_LIGHT = 1;
  FLAG_DEBOUNCE_ALARM = 1;
  FLAG_DEBOUNCE_WARNING = 1;
  FLAG_READ_BATT = 1;
  FLAG_READ_BACKPEDAL = 1;
  if (g_battery_blink_counter > 200) {
    g_battery_blink_counter = 0;
  }
  g_battery_blink_counter++;
  if (g_current_settle_time) {
    g_current_settle_time--;
  }
  if (g_current_alarm_settle_time) {
    g_current_alarm_settle_time--;
  }
  if (g_music_duration) {
    g_music_duration--;
  }
}

// indicator on/off pace, runs every 0.5 secs by default
ISR(TIMER1_COMPA_vect)
{
  FLAG_BLINK_RI = 1;
  FLAG_BLINK_LI = 1;
  FLAG_ALARM_BLINK = 1;
  FLAG_BLINK_WARNING = 1;
  FLAG_BLINK_BACKPEDAL = 1;
  if (g_sleep_counter) {
    g_sleep_counter--;
  }
  g_alarm_blink_counter++;
}

ISR(TIMER3_COMPA_vect)
{
  // toggle buzzer pin to create a 50% duty cycle
  PORT_C90_BUZZER ^= (1 << PIN_C90_BUZZER);
}

void check_sound() {
  if (FLAG_MUSIC) {
    if (!g_music_duration) {
      if (pgm_read_word(g_current_music)) { // get note data until MUSIC_END
        if (g_current_music == g_selected_music) { // still at start: first byte is the tempo
          g_music_tempo = pgm_read_word(g_current_music);
          g_current_music++;
        }
        g_music_duration = calc_note_duration(pgm_read_word(g_current_music));
        g_current_music++;
        if (pgm_read_word(g_current_music) == MUSIC_P) { // pause check (silence for a certain time)
          TIMSK3 &= ~(1 << OCIE3A); // disable interrupt
          PORT_C90_BUZZER &= ~(1 << PIN_C90_BUZZER);
        } else {
          TIMSK3 |= (1 << OCIE3A); // enable interrupt
        }
        OCR3A = pgm_read_word(g_current_music);
        TCNT3 = 0;
        g_current_music++;
      } else { // the end of song
        FLAG_MUSIC = 0;
        TIMSK3 &= ~(1 << OCIE3A); // disable interrupt
        PORT_C90_BUZZER &= ~(1 << PIN_C90_BUZZER);
      }
    }
  }
}

void sleep_now() {
  reset_ports();
  disable_adc();
  DDRA = 0;
  PORTA = 0xff;
  wdt_disable();
  PORT_C90_HEARTBEAT_LED &= ~(1 << PIN_C90_HEARTBEAT_LED); // low
  disable_psenses();
  power_all_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();         
}

void initialize() {
  MCUCR = 0x80; // disable JTAG at runtime (2 calls in a row needed)
  MCUCR = 0x80;
  wdt_enable(WDTO_2S);  // enable 2 sec watchdog
  sei(); //  enable global interrupts
  enable_adc();
  g_state = ST_SLEEP; // always start in sleep mode
  g_buffer_head = g_buffer_tail = 0; // init event buffer
  // init settings
  if (eeprom_read_word(&g_rom_settings.deep_sleep_counter) != g_settings.deep_sleep_counter) {
    g_settings.deep_sleep_counter = eeprom_read_word(&g_rom_settings.deep_sleep_counter);
  }
  if (eeprom_read_word(&g_rom_settings.settle_time) != g_settings.settle_time) {
    g_settings.settle_time = eeprom_read_word(&g_rom_settings.settle_time);
  }
  if (eeprom_read_word(&g_rom_settings.alarm_settle_time) != g_settings.alarm_settle_time) {
    g_settings.alarm_settle_time = eeprom_read_word(&g_rom_settings.alarm_settle_time);
  }
  if (eeprom_read_byte(&g_rom_settings.indicator_sound) != g_settings.indicator_sound) {
    g_settings.indicator_sound = eeprom_read_byte(&g_rom_settings.indicator_sound);
  }
  if (eeprom_read_word(&g_rom_settings.blink_speed) != g_settings.blink_speed) {
    g_settings.blink_speed = eeprom_read_word(&g_rom_settings.blink_speed);
  }
  if (eeprom_read_byte(&g_rom_settings.alarm_counter) != g_settings.alarm_counter) {
    g_settings.alarm_counter = eeprom_read_byte(&g_rom_settings.alarm_counter);
  }
  if (eeprom_read_byte(&g_rom_settings.alarm_trigger) != g_settings.alarm_trigger) {
    g_settings.alarm_trigger = eeprom_read_byte(&g_rom_settings.alarm_trigger);
  }
  if (eeprom_read_byte(&g_rom_settings.alarm_trigger_counter) != g_settings.alarm_trigger_counter) {
    g_settings.alarm_trigger_counter = eeprom_read_byte(&g_rom_settings.alarm_trigger_counter);
  }
  if (eeprom_read_byte(&g_rom_settings.alarm_thres_min) != g_settings.alarm_thres_min) {
    g_settings.alarm_thres_min = eeprom_read_byte(&g_rom_settings.alarm_thres_min);
  }
  if (eeprom_read_word(&g_rom_settings.alarm_thres_max) != g_settings.alarm_thres_max) {
    g_settings.alarm_thres_max = eeprom_read_word(&g_rom_settings.alarm_thres_max);
  }
  if (eeprom_read_byte(&g_rom_settings.backpedal) != g_settings.backpedal) {
    g_settings.backpedal = eeprom_read_byte(&g_rom_settings.backpedal);
  }
  if (eeprom_read_word(&g_rom_settings.backpedal_thres_min) != g_settings.backpedal_thres_min) {
    g_settings.backpedal_thres_min = eeprom_read_word(&g_rom_settings.backpedal_thres_min);
  }
  if (eeprom_read_byte(&g_rom_settings.startup_sound) != g_settings.startup_sound) {
    g_settings.startup_sound = eeprom_read_byte(&g_rom_settings.startup_sound);
  }
  if (eeprom_read_byte(&g_rom_settings.alarm_sound) != g_settings.alarm_sound) {
    g_settings.alarm_sound = eeprom_read_byte(&g_rom_settings.alarm_sound);
  }
  char passwd[32];
  eeprom_read_block((void*)passwd, (const void*)g_rom_settings.passwd, 32);
  if (strcmp(passwd, g_settings.passwd) != 0) {
    strcpy(g_settings.passwd, passwd);
  }
  g_sleep_counter = g_settings.deep_sleep_counter;
  reset_ports();
  start_sense_timer();
  start_blink_timer();
  start_buzzer_timer(0);
  uart_init();
  command_init(command_usart_input, command_usart_output);
  register_commands();
}

// duration of note based on the 5ms timer counter
// 200x 5ms ticks * 60sec/tempo in bmp * (beat note type / length)
// length is normally 1, 2, 4, 8 or 16 parts of a full note
uint16_t calc_note_duration(const uint16_t length) {
  return 200*60/g_music_tempo*g_music_denominator/length;
}

int main(void) {
  initialize();
  while(1) {
    command_process();
    dispatch_events();
    while ((g_event = get_event()) != EV_VOID) { // handle the complete event queue in one go
      for (uint8_t i=0; i<TR_CNT; i++) {
        //TODO: swap if-statements
        if (g_trans[i].check_func()) { // check if state matches
          if (g_event == g_trans[i].event) { // check if event matches
            g_trans[i].process_func(); // call event handler
            break;
          }
        }
      }
    }
    if (!g_mcu_reset) { 
      wdt_reset(); // reset the watchdog, i.e. don't reset the mcu
    }
  }
  return 0;
}

// EOF
