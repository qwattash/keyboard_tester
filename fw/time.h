/*
  Copyright 2018-2019  Alfredo Mazzinghi

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include <stdbool.h>
#include <stdint.h>

struct time {
	uint16_t ms;
	uint16_t ns;
};


/* Clock ticks per nanosecond */
#define CLOCK_HZ 8000000UL
/* Clock ticks per nanosecond */
#define CLOCK_nHZ (CLOCK_HZ / 1000000)
/* Maximum ticks in register size */
#define TICK_MAX ((uint16_t)-1)
/* 
 * Maximum number of nanoseconds that can be counted with different
 * clock prescaler settings. This assumes a 16bit timer.
 */
#define PRESCALER_1_MAX_NS ((uint32_t)TICK_MAX * 1 / CLOCK_nHZ)
#define PRESCALER_8_MAX_NS ((uint32_t)TICK_MAX * 8 / CLOCK_nHZ)
#define PRESCALER_64_MAX_NS ((uint32_t)TICK_MAX * 64 / CLOCK_nHZ)
#define PRESCALER_256_MAX_NS ((uint32_t)TICK_MAX * 256 / CLOCK_nHZ)
#define PRESCALER_1024_MAX_NS ((uint32_t)TICK_MAX * 1024 / CLOCK_nHZ)

/**
 * Clock prescaler configurations for timer N
 */
#define PRESCALER_1(N) (1 << CS##N##0)
#define PRESCALER_8(N) (1 << CS##N##1)
#define PRESCALER_64(N) ((1 << CS##N##1) | (1 << CS##N##0))
#define PRESCALER_256(N) (1 << CS##N##2)
#define PRESCALER_1024(N) ((1 << CS##N##2) | (1 << CS##N##0))
#define PRESCALER_MASK(N) ((1 << CS##N##2) | (1 << CS##N##1) | (1 << CS##N##0))

uint8_t get_timer3_clocksource(uint32_t nano);
uint16_t get_timer3_ticks(uint8_t clksrc, uint32_t nano);
