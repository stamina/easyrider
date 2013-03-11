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
#ifndef _COMMAND_HANDLERS_H_INCLUDED
#define _COMMAND_HANDLERS_H_INCLUDED

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "command.h"
#include "settings.h"

extern volatile uint16_t g_state; // the current state
extern volatile uint16_t g_senses; // active senses
extern volatile uint16_t g_dyn_senses; // dynamic senses (set at runtime, basically overriding sense pin readouts)
extern volatile uint16_t g_dyn_senses_status; // on/off status
extern volatile uint16_t g_adc_voltage[4]; // current ADC voltages: 0 is accu, 1-3 is xyz accelerometer
extern volatile uint8_t g_mcu_reset; // let the mcu reset by the watchdog
extern const char g_firmware_version[] PROGMEM;
extern tSettings g_settings;
extern tSettings g_rom_settings;

char byte_buf[9];
char uart_buf[512];

const char *btob(uint8_t x);
char *display_state_info(void);
char *display_voltage_info(void);
char *display_conf_info(void);
void command_handle_show_version(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
void command_handle_show_state(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
void command_handle_set_sense(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
void command_handle_set_conf(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
void command_handle_show_conf(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
void command_handle_show_voltage(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
void command_handle_help(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
void command_handle_reset(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
void register_commands(void);

static const char help_info[] PROGMEM = "help: show command overview";
static const char reset_info[] PROGMEM = "reset: reboot the microcontroller";
static const char show_version_info[] PROGMEM = "show version: show firmware version";
static const char show_state_info[] PROGMEM = "show state: show current state, senses and event queue";
static const char show_voltage_info[] PROGMEM = "show voltage: show current battery and accelerometer x/y/z axis voltages";
static const char set_sense_info[] PROGMEM = "set dsense [16bits] (enable/disable) [16bits] (high/low): set dynamic senses";
static const char set_conf_info[] PROGMEM = "set conf [variable abbr.] [value]: set configuration values / RESET DEVICE to use new values!";
static const char show_conf_info[] PROGMEM = "show conf: show EEPROM configuration";

#endif
