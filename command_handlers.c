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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include  "command_handlers.h"

const char *btob(uint8_t x) {
  byte_buf[0] = '\0';
  uint8_t z;
  for (z = 128; z > 0; z >>= 1) {
    strcat(byte_buf, ((x & z) == z) ? "1" : "0");
  }
  return byte_buf;
}

void register_commands() {
  command_register("help", help_info, COMMAND_NO_PRIV, command_handle_help);
  command_register("reset", reset_info, COMMAND_NO_PRIV, command_handle_reset);
  command_register("show version", show_version_info, COMMAND_NO_PRIV, command_handle_show_version);
  command_register("show state", show_state_info, COMMAND_NO_PRIV, command_handle_show_state);
  command_register("show conf", show_conf_info, COMMAND_NO_PRIV, command_handle_show_conf);
  command_register("show voltage", show_voltage_info, COMMAND_NO_PRIV, command_handle_show_voltage);
  command_register("set dsense", set_sense_info, COMMAND_NO_PRIV, command_handle_set_sense);
  command_register("set conf", set_conf_info, COMMAND_NO_PRIV, command_handle_set_conf);
}

char *display_state_info() {
  strcpy(uart_buf, "st: ");
  strcat(uart_buf, btob(g_state >> 8));
  strcat(uart_buf, ".");
  strcat(uart_buf, btob(g_state));
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat(uart_buf, "ds: ");
  strcat(uart_buf, btob(g_dyn_senses >> 8));
  strcat(uart_buf, ".");
  strcat(uart_buf, btob(g_dyn_senses));
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat(uart_buf, "dss:");
  strcat(uart_buf, btob(g_dyn_senses_status >> 8));
  strcat(uart_buf, ".");
  strcat(uart_buf, btob(g_dyn_senses_status));
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  return uart_buf;
}

char *display_voltage_info() {
  char accu_str[5];
  char x_str[5];
  char y_str[5];
  char z_str[5];
  utoa(g_adc_voltage[0],accu_str,10);
  utoa(g_adc_voltage[1],x_str,10);
  utoa(g_adc_voltage[2],y_str,10);
  utoa(g_adc_voltage[3],z_str,10);
  strcpy(uart_buf, "battery: ");
  strcat(uart_buf, accu_str);
  strcat(uart_buf, " x: ");
  strcat(uart_buf, x_str);
  strcat(uart_buf, " y: ");
  strcat(uart_buf, y_str);
  strcat(uart_buf, " z: ");
  strcat(uart_buf, z_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  return uart_buf;
}

char *display_conf_info() {
  char tmp_str[32];
  utoa(g_settings.deep_sleep_counter,tmp_str,10);
  strcpy(uart_buf, "deep sleep counter (dsc): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.settle_time,tmp_str,10);
  strcat(uart_buf, "settle time (st): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.alarm_settle_time,tmp_str,10);
  strcat(uart_buf, "alarm settle time (ast): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.indicator_sound,tmp_str,10);
  strcat(uart_buf, "indicator sound (is): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.blink_speed,tmp_str,10);
  strcat(uart_buf, "blink speed (bs): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.alarm_counter,tmp_str,10);
  strcat(uart_buf, "alarm counter (ac): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.alarm_trigger,tmp_str,10);
  strcat(uart_buf, "alarm trigger (at): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.alarm_thres_min,tmp_str,10);
  strcat(uart_buf, "alarm min. threshold (amint): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.alarm_thres_max,tmp_str,10);
  strcat(uart_buf, "alarm max.threshold (amaxt): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.backpedal,tmp_str,10);
  strcat(uart_buf, "backpedal (bp): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.backpedal_thres_min,tmp_str,10);
  strcat(uart_buf, "backpedal threshold (bpt): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.startup_sound,tmp_str,10);
  strcat(uart_buf, "startup sound (ss): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  utoa(g_settings.alarm_sound,tmp_str,10);
  strcat(uart_buf, "alarm sound (as): ");
  strcat(uart_buf, tmp_str);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat(uart_buf, "password (p): ");
  strcat(uart_buf, g_settings.passwd);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  return uart_buf;
}

void command_handle_show_version(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]) {
  strcpy_P(cmd_output, g_firmware_version);
}

void command_handle_show_state(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]) {
  strcpy(cmd_output, display_state_info());
}

void command_handle_set_sense(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]) {
  g_senses = 0; // deactivate physical senses, because they can conflict
  g_dyn_senses = (uint16_t)(strtol(args[0], NULL, 2));
  g_dyn_senses_status = (uint16_t)(strtol(args[1], NULL, 2));
  strcpy(cmd_output, "Senses updated.");
}

void command_handle_set_conf(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]) {
  uint8_t success = 1;
  if (strcmp("dsc", args[0]) == 0) {
    eeprom_update_word(&g_rom_settings.deep_sleep_counter, (uint16_t)(atoi(args[1])));
  } else if (strcmp("st", args[0]) == 0) {
    eeprom_update_word(&g_rom_settings.settle_time, (uint16_t)(atoi(args[1])));
  } else if (strcmp("ast", args[0]) == 0) {
    eeprom_update_word(&g_rom_settings.alarm_settle_time, (uint16_t)(atoi(args[1])));
  } else if (strcmp("is", args[0]) == 0) {
    eeprom_update_byte(&g_rom_settings.indicator_sound, (uint8_t)(atoi(args[1])));
  } else if (strcmp("bs", args[0]) == 0) {
    eeprom_update_word(&g_rom_settings.blink_speed, (uint16_t)(atoi(args[1])));
  } else if (strcmp("ac", args[0]) == 0) {
    eeprom_update_byte(&g_rom_settings.alarm_counter, (uint8_t)(atoi(args[1])));
  } else if (strcmp("at", args[0]) == 0) {
    eeprom_update_byte(&g_rom_settings.alarm_trigger, (uint8_t)(atoi(args[1])));
  } else if (strcmp("amint", args[0]) == 0) {
    eeprom_update_byte(&g_rom_settings.alarm_thres_min, (uint8_t)(atoi(args[1])));
  } else if (strcmp("amaxt", args[0]) == 0) {
    eeprom_update_word(&g_rom_settings.alarm_thres_max, (uint16_t)(atoi(args[1])));
  } else if (strcmp("bp", args[0]) == 0) {
    eeprom_update_byte(&g_rom_settings.backpedal, (uint8_t)(atoi(args[1])));
  } else if (strcmp("bpt", args[0]) == 0) {
    eeprom_update_word(&g_rom_settings.backpedal_thres_min, (uint16_t)(atoi(args[1])));
  } else if (strcmp("ss", args[0]) == 0) {
    eeprom_update_byte(&g_rom_settings.startup_sound, (uint8_t)(atoi(args[1])));
  } else if (strcmp("as", args[0]) == 0) {
    eeprom_update_byte(&g_rom_settings.alarm_sound, (uint8_t)(atoi(args[1])));
  } else if (strcmp("p", args[0]) == 0) {
    eeprom_update_block(args[1], &g_rom_settings.passwd, 32);
  } else {
    success = 0;
  }
  if (success) {
    strcpy(cmd_output, "Update successful, RESET the device to use new values!");
  } else {
    strcpy(cmd_output, "Configuration options error! Use set conf [abbreviation] [value].");
  }
}

void command_handle_show_conf(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]) {
  strcpy(cmd_output, display_conf_info());
}

void command_handle_show_voltage(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]) {
  strcpy(cmd_output, display_voltage_info());
}

void command_handle_help(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]) {
  strcpy_P(uart_buf, help_info);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat_P(uart_buf, reset_info);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat_P(uart_buf, show_version_info);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat_P(uart_buf, show_state_info);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat_P(uart_buf, show_voltage_info);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat_P(uart_buf, set_sense_info);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat_P(uart_buf, set_conf_info);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcat_P(uart_buf, show_conf_info);
  strcat(uart_buf, "\x1B[1B\x1B[2K\x0D");
  strcpy(cmd_output, uart_buf);
}

void command_handle_reset(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]) {
  g_mcu_reset = 1;
  strcpy(cmd_output, "Resetting....");
}
