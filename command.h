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
#ifndef _COMMAND_H_INCLUDED
#define _COMMAND_H_INCLUDED

#define COMMAND_PRIV       1
#define COMMAND_NO_PRIV    0
#define CMD_MAX_ARGS_SIZE 32
#define CMD_MAX_ARGS      10

extern char *g_appname;
extern char *g_user;
extern const char* const g_logo[] PROGMEM;

typedef struct {
  const char *cmd_info; // points to PROGMEM
  const char *cmd_text;
  uint8_t privileged;
} tCommand;

typedef struct {
  tCommand *cmd;
  void (*cmd_cb_t)(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]);
} tCommandHandler;

// proto's
void command_init(void (*input)(uint8_t event, char* cmd), void (*output)(const char* str, uint8_t flashmem, uint8_t clear));  
void command_process(void);
uint8_t command_register(const char *cmd, const char *info, uint8_t priv, void (*handler)(char *cmd_output, char (*args)[CMD_MAX_ARGS_SIZE]));
uint8_t command_launch(void);
void clear_cmd_args(void);
void show_prompt(void);

#endif

