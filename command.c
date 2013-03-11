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

#include  "command.h"

#define CMD_HANDLER_SIZE  64

static char g_cmd_input[256];
static char g_cmd_output[256];
static char g_cmd_args[CMD_MAX_ARGS][CMD_MAX_ARGS_SIZE];
static uint8_t g_handler_pos;
static tCommandHandler g_handlers[CMD_HANDLER_SIZE];
static void (*g_input)(uint8_t, char*);
static void (*g_output)(const char*, uint8_t, uint8_t);

const char CMD_NOT_FOUND[] PROGMEM = "Command not found.";

void command_init(void (*input)(uint8_t event, char* cmd), void (*output)(const char* str, uint8_t flashmem, uint8_t clear)) {  
  g_input = input;
  g_output = output;
  // register generic commands
  show_prompt();
}

uint8_t command_register(const char *cmd, const char *info, uint8_t priv, void (*handler)(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE])) {
  g_handlers[g_handler_pos].cmd = malloc(sizeof(tCommand));
  if (g_handlers[g_handler_pos].cmd == NULL) {
   return 0;  
  }
  g_handlers[g_handler_pos].cmd->cmd_info = info;
  g_handlers[g_handler_pos].cmd->cmd_text = cmd;
  g_handlers[g_handler_pos].cmd->privileged = priv;
  g_handlers[g_handler_pos].cmd_cb_t = handler;
  g_handler_pos++;
  return 1;
}

void command_process() {
  uint8_t idx;
  g_input(0, g_cmd_input);
  if (*g_cmd_input) { // received a command string
    idx = command_launch();
    if (idx != CMD_HANDLER_SIZE) {
      g_handlers[idx].cmd_cb_t(g_cmd_output, g_cmd_args); // call handler
      g_output(g_cmd_output, 0, 3);
    } else {
      g_output(CMD_NOT_FOUND, 1, 3);
    }
    show_prompt();
  }
} 

uint8_t command_launch() {
  uint8_t i, x, n = 0;
  char const *cmd;
  char *arg_part;
  clear_cmd_args(); // clear previous args
  // check input command to all registered commands
  for (i = 0; i < CMD_HANDLER_SIZE; i++) {
    cmd = g_handlers[i].cmd->cmd_text;
    x = 0;
    while (*cmd && (*cmd == g_cmd_input[x])) {
      x++;
      cmd++;
    }
    // input command string matches a fully registered command string
    if ((x) && (x == strlen(g_handlers[i].cmd->cmd_text))) {
      arg_part = strtok(&g_cmd_input[x], " ");
      while (*arg_part) {
        if (strlen(arg_part) <= CMD_MAX_ARGS_SIZE) {
          strcpy(g_cmd_args[n++], arg_part);
        }
        arg_part = strtok(NULL, " ");
      }
      return i; // handler index
    }
  }
  return CMD_HANDLER_SIZE; // no handler found
}

void clear_cmd_args() {
  for (int i = 0; i < CMD_MAX_ARGS; i++) {
    g_cmd_args[i][0] = '\0';
  }
}

void show_prompt() {
  for (uint8_t i = 0; i < 4; i++) {
    g_output((const char*)pgm_read_word(&g_logo[i]), 1, 2);
  }
  g_output(g_user, 0, 0);
  g_output("@", 0, 0);
  g_output(g_appname, 0, 0);
  g_output("> ", 0, 0);
}
