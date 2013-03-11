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
#ifndef _USART_H_INCLUDED
#define _USART_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define USART_BAUDRATE 19200 // slow, but we don't care, we want a low % of transfer errors
#define RX_BUFFER_SIZE 256 // receive buffer
#define TX_BUFFER_SIZE 256 // transmit buffer

// ring buffer declaration
static volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
static volatile uint8_t tx_buffer_head;
static volatile uint8_t tx_buffer_tail;
static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_buffer_head;
static volatile uint8_t rx_buffer_tail;

// proto's
void uart_init(void);
void uart_put(uint8_t c);
void uart_put_str(const char *str);
void uart_put_str_P(const char *str);
void uart_put_int(const uint16_t dec);
uint8_t uart_get(void);
uint8_t uart_available(void);

#endif

