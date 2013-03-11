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
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include  "command_usart.h"
#include  "usart.h"

static char g_buffer[256]; // max of 256 chars for input
static uint8_t g_buffer_pos;
static uint8_t g_command_status;

#define B_SIZE ((int) (sizeof(g_buffer) / sizeof(g_buffer[0]))) 

void command_usart_input(uint8_t event, char *cmd) {
  uint8_t status, current;
  if (event == 0) {
    if (uart_available()) { // data ready
      current = uart_get();
      status = command_usart_parse(&current);
      if (status == CHAR_NORMAL) {
        if (g_buffer_pos >= B_SIZE) { // reset when full
          g_buffer_pos = 0;
        }
        uart_put(current); // echo valid char back
        g_buffer[g_buffer_pos] = current; // add to buffer
        g_command_status = CMD_IN_PROCESS;
        g_buffer_pos++;
      } else if (status == CHAR_ENTER) { // enter pressed
          g_buffer[g_buffer_pos] = '\0'; // mark current pos as end
          g_buffer_pos = 0;
          g_command_status = CMD_RDY;
      }
    }
    if (g_command_status == CMD_RDY) { // copy buffer to command
      strcpy(cmd, g_buffer);
      g_command_status = CMD_IN_PROCESS;
    } else { // set command as "still empty"
      cmd[0] = '\0'; 
    }
  }
}

void command_usart_output(const char *cmd, uint8_t flashmem, uint8_t clear) {
  if (clear == 1 || clear == 3) {
    uart_put_str("\x1B[2J\x0D"); // clear entire screen
  }
  if (flashmem) {
    uart_put_str_P(cmd);
  } else {
    uart_put_str(cmd);
  }
  if (clear == 2 || clear == 3) {
    uart_put_str("\x1B[1B"); // next line
    uart_put_str("\x1B[2K\x0D"); // clear line and CR
  }
}

uint8_t command_usart_parse(uint8_t *current) {
  if (*current == 0x0d) {
    return CHAR_ENTER;
  } else if (*current == 0x08) {
    return CHAR_BACKSPACE;
  } else if ((*current >= 0x20) && (*current < 0x7F)) {
    return CHAR_NORMAL;
  }
  return CHAR_FILTER;
}


