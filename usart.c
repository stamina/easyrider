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
#include <stdlib.h>
#include  "usart.h"

// Initialization
// NOTE that I'm using the UART1 rx/tx pins of the atmega1284p, not UART0
void uart_init() {
  cli(); // Disable global interrupts
  UBRR1 = ((F_CPU / (USART_BAUDRATE * 8UL)) - 1); // set baudrate, using 8 as multiplier because we set U2X1
	UCSR1A = (1 << U2X1); // enable 2x speed
  // Turn on the reception and transmission circuitry and Reception Complete Interrupt
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); // set 8 bits data size (default no parity bit, 1 stop bit)
  tx_buffer_head = tx_buffer_tail = 0; // init buffer
  rx_buffer_head = rx_buffer_tail = 0; // init buffer
  sei(); // Enable global interrupts
}

// Transmit a byte
void uart_put(uint8_t c) {
	uint8_t i;
	i = tx_buffer_head + 1; // advance head
	if (i >= TX_BUFFER_SIZE) i = 0; // go to first index if buffer full
  // NOTE: the hard wait below is not desirable when big UART transfers are part of the main program loop.
  // Change it to an if (tx_buffer_tail != i)
	while (tx_buffer_tail == i); // wait until space in buffer
  tx_buffer[i] = c; // put char in buffer
  tx_buffer_head = i; // set new head
  // Turn on the reception and transmission circuitry and Reception Complete and
  // USART Data Register Empty interrupts
  UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1) | (1<<UDRIE1);
}

// Receive a byte, always call uart_available() first
uint8_t uart_get(void) {
  uint8_t c, i;
  i = rx_buffer_tail + 1; // advance tail
  if (i >= RX_BUFFER_SIZE) i = 0; // got to first index if buffer full
  c = rx_buffer[i]; // get char from buffer
  rx_buffer_tail = i; // set new tail
  return c; // return char
}

// Writes a string from flash to the uart
void uart_put_str(const char *str) {
	while (*str) {
		uart_put(*str);
    str++;
	}
}

// Writes an integer as a string
void uart_put_int(const uint16_t dec) {
  char str[10];
  char *p = str;
  utoa(dec,str,10);
	while (*p) {
		uart_put(*p);
    p++;
	}
}

// Writes a string from Flash (PROGMEM) to the uart
void uart_put_str_P(const char *str) {
	char c;
	while (1) {
		c = pgm_read_byte(str++);
		if (!c) break;
		uart_put(c);
	}
}

// USART1 data register empty Interrupt
// Move a character from the transmit buffer to the data register.
// If the transmit buffer is empty the UDRE interrupt is disabled until another uart_put() is called
ISR(USART1_UDRE_vect) {
	uint8_t i;
	if (tx_buffer_head == tx_buffer_tail) {
    UCSR1B &= ~(1 << UDRIE1); // buffer is empty, disable interrupt
	} else { // fill transmit register with next byte to send
		i = tx_buffer_tail + 1; // get tail + 1
		if (i >= TX_BUFFER_SIZE) i = 0; // go to first index if buffer full
		UDR1 = tx_buffer[i]; // send byte
		tx_buffer_tail = i; // set new tail
	}
}

// Receive Complete Interrupt
ISR(USART1_RX_vect) {
	uint8_t c, i;
	c = UDR1; // receive byte
	i = rx_buffer_head + 1; // advance head 
	if (i >= RX_BUFFER_SIZE) i = 0; // go to first index if buffer full
	if (i != rx_buffer_tail) { // not full
		rx_buffer[i] = c; // put in read buffer
		rx_buffer_head = i; // set new head
	}
}

// Return the number of bytes waiting in the receive buffer.
// Call this before uart_get() to check if it will need
// to wait for a byte to arrive.
uint8_t uart_available(void) {
	uint8_t head, tail;
	head = rx_buffer_head;
	tail = rx_buffer_tail;
	if (head >= tail) return head - tail; // return count of bytes inbetween
	return RX_BUFFER_SIZE + head - tail; // head has rolled over to start, return count of bytes inbetween
}

