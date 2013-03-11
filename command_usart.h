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
#ifndef _COMMAND_USART_H_INCLUDED
#define _COMMAND_USART_H_INCLUDED

#define CMD_IN_PROCESS 0
#define CMD_RDY 1

#define CHAR_NORMAL 1
#define CHAR_ENTER 2
#define CHAR_BACKSPACE 3
#define CHAR_FILTER 4

// proto's
void command_usart_input(uint8_t event, char *cmd);
void command_usart_output(const char *cmd, uint8_t flashmem, uint8_t clear);
uint8_t command_usart_parse(uint8_t *current);

#endif

