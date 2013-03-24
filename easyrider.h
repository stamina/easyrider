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
#ifndef _EASYRIDER_H_INCLUDED
#define _EASYRIDER_H_INCLUDED

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "settings.h"

#define STATUS_C90_SENSE_LIGHT_RI                   PINA & (1 << PIN_C90_SENSE_LIGHT_RI) 
#define STATUS_C90_SENSE_LIGHT_LI                   PINA & (1 << PIN_C90_SENSE_LIGHT_LI) 
#define STATUS_C90_SENSE_PILOT                      PINA & (1 << PIN_C90_SENSE_PILOT) 
#define STATUS_C90_SENSE_LIGHT                      PINA & (1 << PIN_C90_SENSE_LIGHT) 
#define STATUS_C90_SENSE_BRAKE                      PINB & (1 << PIN_C90_SENSE_BRAKE) 
#define STATUS_C90_SENSE_ALARM                      PINB & (1 << PIN_C90_SENSE_ALARM) 
#define STATUS_C90_SENSE_IGN                        PINB & (1 << PIN_C90_SENSE_IGN) 
#define STATUS_C90_SENSE_CLAXON                     PINB & (1 << PIN_C90_SENSE_CLAXON) 
#define STATUS_C90_SENSE_WARNING                    PIND & (1 << PIN_C90_SENSE_WARNING) 
#define STATUS_C90_SENSE_RESERVE                    PIND & (1 << PIN_C90_SENSE_RESERVE) 

#define PIN_C90_SENSE_LIGHT_RI                      PINA4 
#define PIN_C90_SENSE_LIGHT_LI                      PINA5 
#define PIN_C90_SENSE_PILOT                         PINA6 
#define PIN_C90_SENSE_LIGHT                         PINA7 
#define PIN_C90_SENSE_BRAKE                         PINB0 
#define PIN_C90_SENSE_ALARM                         PINB1 
#define PIN_C90_SENSE_IGN                           PINB2 
#define PIN_C90_SENSE_CLAXON                        PINB3 
#define PIN_C90_SENSE_WARNING                       PIND0
#define PIN_C90_SENSE_RESERVE                       PIND1

#define PORT_C90_SENSE_LIGHT_RI                     PINA 
#define PORT_C90_SENSE_LIGHT_LI                     PINA 
#define PORT_C90_SENSE_PILOT                        PINA 
#define PORT_C90_SENSE_LIGHT                        PINA 
#define PORT_C90_SENSE_BRAKE                        PINB 
#define PORT_C90_SENSE_ALARM                        PINB 
#define PORT_C90_SENSE_IGN                          PINB 
#define PORT_C90_SENSE_CLAXON                       PINB 
#define PORT_C90_SENSE_WARNING                      PIND 
#define PORT_C90_SENSE_RESERVE                      PIND 

#define DDR_C90_SENSE_LIGHT_RI                      DDRA 
#define DDR_C90_SENSE_LIGHT_LI                      DDRA 
#define DDR_C90_SENSE_CLAXON                        DDRB
#define DDR_C90_SENSE_BRAKE                         DDRB
#define DDR_C90_SENSE_PILOT                         DDRA
#define DDR_C90_SENSE_LIGHT                         DDRA
#define DDR_C90_SENSE_ALARM                         DDRB
#define DDR_C90_SENSE_IGN                           DDRB
#define DDR_C90_SENSE_WARNING                       DDRD 
#define DDR_C90_SENSE_RESERVE                       DDRD 

#define DDR_C90_LIGHT_RI_F                          DDRC 
#define DDR_C90_LIGHT_RI_B                          DDRC 
#define DDR_C90_LIGHT_LI_F                          DDRC 
#define DDR_C90_LIGHT_LI_B                          DDRC 
#define DDR_C90_LIGHT_INDICATOR_COCKPIT             DDRC 
#define DDR_C90_LIGHT_STATUS_COCKPIT                DDRC 
#define DDR_C90_CLAXON                              DDRD
#define DDR_C90_BRAKE                               DDRC
#define DDR_C90_PILOT                               DDRD
#define DDR_C90_LIGHT                               DDRC
#define DDR_C90_BUZZER                              DDRD
#define DDR_C90_HEARTBEAT_LED                       DDRD

#define PORT_C90_LIGHT_RI_F                         PORTC
#define PORT_C90_LIGHT_LI_F                         PORTC
#define PORT_C90_LIGHT_RI_B                         PORTC
#define PORT_C90_LIGHT_LI_B                         PORTC
#define PORT_C90_LIGHT_INDICATOR_COCKPIT            PORTC
#define PORT_C90_LIGHT_STATUS_COCKPIT               PORTC
#define PORT_C90_CLAXON                             PORTD
#define PORT_C90_BRAKE                              PORTC
#define PORT_C90_PILOT                              PORTD
#define PORT_C90_LIGHT                              PORTC
#define PORT_C90_BUZZER                             PORTD
#define PORT_C90_HEARTBEAT_LED                      PORTD

#define PIN_C90_LIGHT_RI_F                          PINC0
#define PIN_C90_LIGHT_LI_F                          PINC1
#define PIN_C90_LIGHT_RI_B                          PINC3
#define PIN_C90_LIGHT_LI_B                          PINC4
#define PIN_C90_LIGHT_INDICATOR_COCKPIT             PINC2
#define PIN_C90_LIGHT_STATUS_COCKPIT                PINC5
#define PIN_C90_CLAXON                              PIND6
#define PIN_C90_BRAKE                               PINC7
#define PIN_C90_PILOT                               PIND7
#define PIN_C90_LIGHT                               PINC6
#define PIN_C90_BUZZER                              PIND5
#define PIN_C90_HEARTBEAT_LED                       PIND4

#define PIN_C90_ADC_BATTERY                         PINA0
#define PIN_C90_ADC_ACCEL_X                         PINA1
#define PIN_C90_ADC_ACCEL_Y                         PINA2
#define PIN_C90_ADC_ACCEL_Z                         PINA3

#define EV_VOID                                  0 
#define EV_ANY                                   1
#define EV_RI_ON                                 2
#define EV_LI_ON                                 3
#define EV_RI_OFF                                4
#define EV_LI_OFF                                5
#define EV_CLAXON_ON                             6
#define EV_CLAXON_OFF                            7
#define EV_BRAKE_ON                              8
#define EV_BRAKE_OFF                             9
#define EV_PILOT_ON                             10 
#define EV_PILOT_OFF                            11
#define EV_LIGHT_ON                             12
#define EV_LIGHT_OFF                            13
#define EV_ALARM_ON                             14 
#define EV_ALARM_OFF                            15 
#define EV_READ_BATT                            16
#define EV_READ_ACCEL                           17
#define EV_IGN_ON                               18
#define EV_IGN_OFF                              19
#define EV_WARNING_ON                           20
#define EV_WARNING_OFF                          21
#define EV_BACKPEDAL                            22

#define ST_NORMAL                                1
#define ST_RI                                    2
#define ST_LI                                    4
#define ST_CLAXON                                8
#define ST_BRAKE                                16
#define ST_LIGHT                                32 
#define ST_ALARM                                64
#define ST_PILOT                               128
#define ST_ALARM_SETTLE                        256
#define ST_SLEEP                               512
#define ST_SETTLE                             1024
#define ST_WARNING                            2048

#define FLAG_SENSE_LIGHT_RI                      1
#define FLAG_SENSE_LIGHT_LI                      2
#define FLAG_SENSE_PILOT                         4
#define FLAG_SENSE_LIGHT                         8
#define FLAG_SENSE_BRAKE                        16
#define FLAG_SENSE_ALARM                        32
#define FLAG_SENSE_IGN                          64
#define FLAG_SENSE_CLAXON                      128
#define FLAG_SENSE_WARNING                     256
#define FLAG_SENSE_RESERVE                     512

#define TR_CNT (sizeof(g_trans)/sizeof(*g_trans))          // number of transition table entries

#define C90_EVENT_BUFFER_SIZE                       64     // size of event queue (circular buffer)
#define C90_OFFSET_ADC_READING                       1     // tweak this if your 5V VRef is a bit off

#define MUSIC_END 0                                        // end flag of music 
#define MUSIC_P 1                                          // music pause

#define a0   5682 	/* PWM: 27.50 Hz, note freq: 27.50 Hz, error 0.00% */
#define a0x  5363 	/* PWM: 29.13 Hz, note freq: 29.14 Hz, error 0.00% */
#define b0   5062 	/* PWM: 30.87 Hz, note freq: 30.87 Hz, error 0.00% */
#define c0   9556 	/* PWM: 16.35 Hz, note freq: 16.35 Hz, error 0.00% */
#define c0x  9019 	/* PWM: 17.32 Hz, note freq: 17.32 Hz, error 0.00% */
#define d0   8513 	/* PWM: 18.35 Hz, note freq: 18.35 Hz, error 0.00% */
#define d0x  8035 	/* PWM: 19.45 Hz, note freq: 19.45 Hz, error 0.00% */
#define e0   7584 	/* PWM: 20.60 Hz, note freq: 20.60 Hz, error 0.00% */
#define f0   7159 	/* PWM: 21.83 Hz, note freq: 21.83 Hz, error 0.00% */
#define f0x  6757 	/* PWM: 23.12 Hz, note freq: 23.12 Hz, error 0.00% */
#define g0   6378 	/* PWM: 24.50 Hz, note freq: 24.50 Hz, error 0.01% */
#define g0x  6020 	/* PWM: 25.96 Hz, note freq: 25.96 Hz, error 0.01% */
#define a1   2841 	/* PWM: 55.00 Hz, note freq: 55.00 Hz, error 0.00% */
#define a1x  2681 	/* PWM: 58.28 Hz, note freq: 58.27 Hz, error 0.02% */
#define b1   2531 	/* PWM: 61.73 Hz, note freq: 61.74 Hz, error 0.00% */
#define c1   4778 	/* PWM: 32.70 Hz, note freq: 32.70 Hz, error 0.00% */
#define c1x  4510 	/* PWM: 34.65 Hz, note freq: 34.65 Hz, error 0.01% */
#define d1   4257 	/* PWM: 36.70 Hz, note freq: 36.71 Hz, error 0.01% */
#define d1x  4018 	/* PWM: 38.89 Hz, note freq: 38.89 Hz, error 0.01% */
#define e1   3792 	/* PWM: 41.21 Hz, note freq: 41.20 Hz, error 0.00% */
#define f1   3579 	/* PWM: 43.66 Hz, note freq: 43.65 Hz, error 0.01% */
#define f1x  3378 	/* PWM: 46.26 Hz, note freq: 46.25 Hz, error 0.01% */
#define g1   3189 	/* PWM: 49.00 Hz, note freq: 49.00 Hz, error 0.01% */
#define g1x  3010 	/* PWM: 51.91 Hz, note freq: 51.91 Hz, error 0.01% */
#define a2   1420 	/* PWM: 110.04 Hz, note freq: 110.00 Hz, error 0.03% */
#define a2x  1341 	/* PWM: 116.52 Hz, note freq: 116.54 Hz, error 0.02% */
#define b2   1265 	/* PWM: 123.52 Hz, note freq: 123.47 Hz, error 0.04% */
#define c2   2389 	/* PWM: 65.40 Hz, note freq: 65.41 Hz, error 0.00% */
#define c2x  2255 	/* PWM: 69.29 Hz, note freq: 69.30 Hz, error 0.01% */
#define d2   2128 	/* PWM: 73.43 Hz, note freq: 73.42 Hz, error 0.01% */
#define d2x  2009 	/* PWM: 77.78 Hz, note freq: 77.78 Hz, error 0.01% */
#define e2   1896 	/* PWM: 82.41 Hz, note freq: 82.41 Hz, error 0.00% */
#define f2   1790 	/* PWM: 87.29 Hz, note freq: 87.31 Hz, error 0.02% */
#define f2x  1689 	/* PWM: 92.51 Hz, note freq: 92.50 Hz, error 0.01% */
#define g2   1594 	/* PWM: 98.02 Hz, note freq: 98.00 Hz, error 0.03% */
#define g2x  1505 	/* PWM: 103.82 Hz, note freq: 103.83 Hz, error 0.01% */
#define a3   710  	/* PWM: 220.07 Hz, note freq: 220.00 Hz, error 0.03% */
#define a3x  670  	/* PWM: 233.21 Hz, note freq: 233.08 Hz, error 0.05% */
#define b3   633  	/* PWM: 246.84 Hz, note freq: 246.94 Hz, error 0.04% */
#define c3   1194 	/* PWM: 130.86 Hz, note freq: 130.81 Hz, error 0.04% */
#define c3x  1127 	/* PWM: 138.64 Hz, note freq: 138.59 Hz, error 0.04% */
#define d3   1064 	/* PWM: 146.85 Hz, note freq: 146.83 Hz, error 0.01% */
#define d3x  1004 	/* PWM: 155.63 Hz, note freq: 155.56 Hz, error 0.04% */
#define e3   948  	/* PWM: 164.82 Hz, note freq: 164.81 Hz, error 0.00% */
#define f3   895  	/* PWM: 174.58 Hz, note freq: 174.61 Hz, error 0.02% */
#define f3x  845  	/* PWM: 184.91 Hz, note freq: 185.00 Hz, error 0.05% */
#define g3   797  	/* PWM: 196.05 Hz, note freq: 196.00 Hz, error 0.03% */
#define g3x  752  	/* PWM: 207.78 Hz, note freq: 207.65 Hz, error 0.06% */
#define a4   355  	/* PWM: 440.14 Hz, note freq: 440.00 Hz, error 0.03% */
#define a4x  335  	/* PWM: 466.42 Hz, note freq: 466.16 Hz, error 0.05% */
#define b4   316  	/* PWM: 494.46 Hz, note freq: 493.88 Hz, error 0.12% */
#define c4   597  	/* PWM: 261.73 Hz, note freq: 261.63 Hz, error 0.04% */
#define c4x  564  	/* PWM: 277.04 Hz, note freq: 277.18 Hz, error 0.05% */
#define d4   532  	/* PWM: 293.70 Hz, note freq: 293.66 Hz, error 0.01% */
#define d4x  502  	/* PWM: 311.25 Hz, note freq: 311.13 Hz, error 0.04% */
#define e4   474  	/* PWM: 329.64 Hz, note freq: 329.63 Hz, error 0.00% */
#define f4   447  	/* PWM: 349.55 Hz, note freq: 349.23 Hz, error 0.09% */
#define f4x  422  	/* PWM: 370.26 Hz, note freq: 369.99 Hz, error 0.07% */
#define g4   399  	/* PWM: 391.60 Hz, note freq: 392.00 Hz, error 0.10% */
#define g4x  376  	/* PWM: 415.56 Hz, note freq: 415.30 Hz, error 0.06% */
#define a5   178  	/* PWM: 877.81 Hz, note freq: 880.00 Hz, error 0.25% */
#define a5x  168  	/* PWM: 930.06 Hz, note freq: 932.33 Hz, error 0.24% */
#define b5   158  	/* PWM: 988.92 Hz, note freq: 987.77 Hz, error 0.12% */
#define c5   299  	/* PWM: 522.58 Hz, note freq: 523.25 Hz, error 0.13% */
#define c5x  282  	/* PWM: 554.08 Hz, note freq: 554.37 Hz, error 0.05% */
#define d5   266  	/* PWM: 587.41 Hz, note freq: 587.33 Hz, error 0.01% */
#define d5x  251  	/* PWM: 622.51 Hz, note freq: 622.25 Hz, error 0.04% */
#define e5   237  	/* PWM: 659.28 Hz, note freq: 659.26 Hz, error 0.00% */
#define f5   224  	/* PWM: 697.54 Hz, note freq: 698.46 Hz, error 0.13% */
#define f5x  211  	/* PWM: 740.52 Hz, note freq: 739.99 Hz, error 0.07% */
#define g5   199  	/* PWM: 785.18 Hz, note freq: 783.99 Hz, error 0.15% */
#define g5x  188  	/* PWM: 831.12 Hz, note freq: 830.61 Hz, error 0.06% */
#define a6   89   	/* PWM: 1755.62 Hz, note freq: 1760.00 Hz, error 0.25% */
#define a6x  84   	/* PWM: 1860.12 Hz, note freq: 1864.66 Hz, error 0.24% */
#define b6   79   	/* PWM: 1977.85 Hz, note freq: 1975.53 Hz, error 0.12% */
#define c6   149  	/* PWM: 1048.66 Hz, note freq: 1046.50 Hz, error 0.21% */
#define c6x  141  	/* PWM: 1108.16 Hz, note freq: 1108.73 Hz, error 0.05% */
#define d6   133  	/* PWM: 1174.81 Hz, note freq: 1174.66 Hz, error 0.01% */
#define d6x  126  	/* PWM: 1240.08 Hz, note freq: 1244.51 Hz, error 0.36% */
#define e6   119  	/* PWM: 1313.03 Hz, note freq: 1318.51 Hz, error 0.42% */
#define f6   112  	/* PWM: 1395.09 Hz, note freq: 1396.91 Hz, error 0.13% */
#define f6x  106  	/* PWM: 1474.06 Hz, note freq: 1479.98 Hz, error 0.40% */
#define g6   100  	/* PWM: 1562.50 Hz, note freq: 1567.98 Hz, error 0.35% */
#define g6x  94   	/* PWM: 1662.23 Hz, note freq: 1661.22 Hz, error 0.06% */
#define a7   44   	/* PWM: 3551.14 Hz, note freq: 3520.00 Hz, error 0.88% */
#define a7x  42   	/* PWM: 3720.24 Hz, note freq: 3729.31 Hz, error 0.24% */
#define b7   40   	/* PWM: 3906.25 Hz, note freq: 3951.07 Hz, error 1.15% */
#define c7   75   	/* PWM: 2083.33 Hz, note freq: 2093.00 Hz, error 0.46% */
#define c7x  70   	/* PWM: 2232.14 Hz, note freq: 2217.46 Hz, error 0.66% */
#define d7   67   	/* PWM: 2332.09 Hz, note freq: 2349.32 Hz, error 0.74% */
#define d7x  63   	/* PWM: 2480.16 Hz, note freq: 2489.02 Hz, error 0.36% */
#define e7   59   	/* PWM: 2648.31 Hz, note freq: 2637.02 Hz, error 0.43% */
#define f7   56   	/* PWM: 2790.18 Hz, note freq: 2793.83 Hz, error 0.13% */
#define f7x  53   	/* PWM: 2948.11 Hz, note freq: 2959.96 Hz, error 0.40% */
#define g7   50   	/* PWM: 3125.00 Hz, note freq: 3135.96 Hz, error 0.35% */
#define g7x  47   	/* PWM: 3324.47 Hz, note freq: 3322.44 Hz, error 0.06% */
#define a8   22   	/* PWM: 7102.27 Hz, note freq: 7040.00 Hz, error 0.88% */
#define a8x  21   	/* PWM: 7440.48 Hz, note freq: 7458.62 Hz, error 0.24% */
#define b8   20   	/* PWM: 7812.50 Hz, note freq: 7902.13 Hz, error 1.15% */
#define c8   37   	/* PWM: 4222.97 Hz, note freq: 4186.01 Hz, error 0.88% */
#define c8x  35   	/* PWM: 4464.29 Hz, note freq: 4434.92 Hz, error 0.66% */
#define d8   33   	/* PWM: 4734.85 Hz, note freq: 4698.64 Hz, error 0.76% */
#define d8x  31   	/* PWM: 5040.32 Hz, note freq: 4978.03 Hz, error 1.24% */
#define e8   30   	/* PWM: 5208.33 Hz, note freq: 5274.04 Hz, error 1.26% */
#define f8   28   	/* PWM: 5580.36 Hz, note freq: 5587.65 Hz, error 0.13% */
#define f8x  26   	/* PWM: 6009.62 Hz, note freq: 5919.91 Hz, error 1.49% */
#define g8   25   	/* PWM: 6250.00 Hz, note freq: 6271.93 Hz, error 0.35% */
#define g8x  24   	/* PWM: 6510.42 Hz, note freq: 6644.88 Hz, error 2.07% */

// Music Time Signature:
// 4   < the numerator shows how many beats are in the measure (not important for the buzzer sound)
// -
// 4   < the denominator shows what type of note gets the beat: full, half, quarter, eight, sixteenth
static const uint8_t g_music_denominator = 4; 
static volatile uint16_t g_music_duration;
static uint8_t g_music_tempo;
const char g_logo1[] PROGMEM = "    _";
const char g_logo2[] PROGMEM = ".-.-.=\x5C-.";
const char g_logo3[] PROGMEM = "(_)=='(_)";
const char g_logo4[] PROGMEM = "2013 Bas Brugman - http://www.visionnaire.nl - type help for command overview";
const char* const g_logo[] PROGMEM = {
  g_logo1, g_logo2, g_logo3, g_logo4
};
char *g_appname = "EasyRider";
char *g_user = "user";
const char g_firmware_version[] PROGMEM = "EasyRider version 1.0 March 2013";

extern const uint16_t g_music_alarm[] PROGMEM;
extern const uint16_t g_music_pipi[] PROGMEM;
extern const uint16_t g_music_popcorn[] PROGMEM;
extern const uint16_t g_music_frogger[] PROGMEM;
extern const uint16_t g_music_larry[] PROGMEM;
extern const uint16_t g_music_furelise[] PROGMEM;
static const uint16_t * const g_music[] PROGMEM = { g_music_alarm, g_music_pipi, g_music_popcorn,
  g_music_larry, g_music_frogger, g_music_furelise };

// state transition struct             
typedef struct {                     
   uint8_t (*check_func)(void);                    
   uint8_t event;                       
   void (*process_func)(void);                    
} tTransition;                         

tSettings g_settings = {
  1200,800,800,1,9766,6,18,4,140,550,0,297,0,1,"admin"
};

tSettings EEMEM g_rom_settings = {
  1200,800,800,1,9766,6,18,4,140,550,0,297,0,1,"admin"
};

// proto's
static void dispatch_events(void);
static uint8_t get_event(void);
static void set_event(uint8_t ev);
static uint16_t calc_note_duration(const uint16_t length);
static void initialize(void);
static void sleep_now(void);
static void start_blink_timer(void);
static void start_sense_timer(void);
static void start_buzzer_timer(uint8_t isr_on);
static void reset_ports(void);
static void set_substate(uint16_t st);
static void remove_substate(uint16_t st);
static uint16_t get_substate(uint16_t st);
static void process_ri_on(void);       
static void process_ri_off(void);      
static void process_li_on(void);
static void process_li_off(void);
static void process_claxon_on(void);
static void process_claxon_off(void);
static void process_ign_on(void);
static void process_ign_off(void);
static void process_alarm_on(void);
static void process_alarm_off(void);
static void process_brake_on(void);
static void process_brake_off(void);
static void process_pilot_on(void);
static void process_pilot_off(void);
static void process_light_on(void);
static void process_light_off(void);
static void process_battery(void);
static void process_warning_off(void);
static void process_warning_on(void);
static void process_backpedal(void);
static uint8_t check_ri_on(void);
static uint8_t check_li_on(void);
static uint8_t check_ri_off(void);
static uint8_t check_li_off(void);
static uint8_t check_claxon_on(void);
static uint8_t check_claxon_off(void);
static uint8_t check_ign_on(void);
static uint8_t check_ign_off(void);
static uint8_t check_alarm_on(void);
static uint8_t check_alarm_off(void);
static uint8_t check_brake_on(void);
static uint8_t check_brake_off(void);
static uint8_t check_pilot_on(void);
static uint8_t check_pilot_off(void);
static uint8_t check_light_on(void);
static uint8_t check_light_off(void);
static uint8_t check_battery_read(void);
static uint8_t check_warning_on(void);
static uint8_t check_warning_off(void);
static uint8_t check_backpedal_on(void);
static void check_ri(void);
static void check_li(void);
static void check_claxon(void);
static void check_ign(void);
static void check_settle(void);
static void check_alarm(void);
static void check_alarm_settle(void);
static void check_sound(void);
static void check_pilot(void);
static void check_light(void);
static void check_brake(void);
static void check_battery(void);
static void check_sleep(void);
static void check_warning(void);
static void check_backpedal(void);
static void check_alarm_trigger(void);
static void all_lights_off(void);
static void enable_psenses(void);
static void disable_psenses(void);
static void enable_adc(void);
static void disable_adc(void);

// flags
static volatile uint8_t FLAG_BLINK_RI; // right indicator blink needed
static volatile uint8_t FLAG_BLINK_LI; // left indicator blink needed
static volatile uint8_t FLAG_BLINK_WARNING; // all indicators blink needed
static volatile uint8_t FLAG_BLINK_BACKPEDAL; // all indicators blink needed
static volatile uint8_t FLAG_SETTLE; // settle in progress flag
static volatile uint8_t FLAG_ALARM_SETTLE; // alarm settle in progress flag
static volatile uint8_t FLAG_ALARM_BLINK; // alarm indicator blink needed
static volatile uint8_t FLAG_ALARM_TRIGGER; // alarm trigger needed
static volatile uint8_t FLAG_MUSIC; // time to play some music
static volatile uint8_t FLAG_DEBOUNCE_RI; // right indicator debounce
static volatile uint8_t FLAG_DEBOUNCE_LI; // left indicator debounce
static volatile uint8_t FLAG_DEBOUNCE_CLAXON; // claxon debounce
static volatile uint8_t FLAG_DEBOUNCE_IGN; // ignition key debounce
static volatile uint8_t FLAG_DEBOUNCE_ALARM; // alarm debounce
static volatile uint8_t FLAG_DEBOUNCE_BRAKE; // brake debounce
static volatile uint8_t FLAG_DEBOUNCE_PILOT; // pilot debounce
static volatile uint8_t FLAG_DEBOUNCE_LIGHT; // light debounce
static volatile uint8_t FLAG_DEBOUNCE_WARNING; // warning debounce
static volatile uint8_t FLAG_READ_BATT; // battery voltage read needed
static volatile uint8_t FLAG_READ_BACKPEDAL; // read backward movement of accelerometer
static volatile uint8_t FLAG_BACKPEDAL; // backpedalling on progress
  
// globals
volatile uint16_t g_state; // the current state
static volatile uint8_t g_event; // the current event
volatile uint16_t g_senses; // active senses
volatile uint16_t g_dyn_senses; // dynamic senses (set at runtime, basically overriding sense pin readouts)
volatile uint16_t g_dyn_senses_status; // on/off status
volatile uint8_t g_mcu_reset; // let the mcu reset by the watchdog
static volatile uint8_t g_event_buffer[C90_EVENT_BUFFER_SIZE]; // the event queue (circular buffer)
static volatile uint8_t g_buffer_head; // index of first item to process
static volatile uint8_t g_buffer_tail; // index of last item to process
static volatile uint16_t g_current_settle_time; // time to stay in settle state 
static volatile uint16_t g_current_alarm_settle_time; // time to stay in alarm settle state 
static uint16_t const *g_selected_music; // pointer to current song
static uint16_t const *g_current_music; // pointer advancing within current song
static volatile uint8_t g_adc_read_pin; // current pin to read ADC voltage
volatile uint16_t g_adc_voltage[4]; // current ADC voltages: 0 is accu, 1-3 is xyz accelerometer
static volatile uint16_t g_adc_avg_voltage[3]; // average ADC voltages: 0-2 is xyz accelerometer
static volatile uint8_t g_battery_blink_counter; // counter to determine battery status blink speed
static volatile uint8_t g_alarm_blink_counter; // counter to determine alarm indicator blinking interval
static volatile uint8_t g_alarm_counter; // counter to determine the amount of alarm signals in a row
static volatile uint16_t g_alarm_snapshot[3]; // alarm snapshot array which contains the 3-axis' voltages
static volatile uint8_t g_indicator_sound; // buzzer beep when indicators blink
static volatile uint16_t g_sleep_counter; // counter for power down mode
static volatile uint8_t g_trigger_counter; // number of positive triggers needed to sound the alarm
  
// debounce bitmasks
static volatile uint8_t g_ri_debounce; // right indicators light
static volatile uint8_t g_li_debounce; // left indicators light
static volatile uint8_t g_claxon_debounce; // claxon sound
static volatile uint8_t g_ign_debounce; // ignition sound
static volatile uint8_t g_brake_debounce; // brake light
static volatile uint8_t g_pilot_debounce; // pilot light
static volatile uint8_t g_light_debounce; // main light
static volatile uint8_t g_alarm_debounce; // alarm switch
static volatile uint8_t g_warning_debounce; // warning switch

#endif
