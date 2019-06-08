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

#include <avr/cpufunc.h>
#include <avr/io.h>

#include "keyboard_tester.h"
#include "time.h"

/**
 * Compute best prescaler configuration for given interval.
 */
uint8_t
get_timer3_clocksource(uint32_t nano)
{

	if (nano > PRESCALER_1024_MAX_NS)
		return 0;
	if (nano > PRESCALER_256_MAX_NS) {
		DEBUG("PRESCALER 1204 max:%ldns\r\n", PRESCALER_1024_MAX_NS);
		return PRESCALER_1024(3);
	}
	if (nano > PRESCALER_64_MAX_NS) {
		DEBUG("PRESCALER 256 max:%ldns\r\n", PRESCALER_256_MAX_NS);
		return PRESCALER_256(3);
	}
	if (nano > PRESCALER_8_MAX_NS) {
		DEBUG("PRESCALER 64 max:%ldns\r\n", PRESCALER_64_MAX_NS);
		return PRESCALER_64(3);
	}
	if (nano > PRESCALER_1_MAX_NS) {
		DEBUG("PRESCALER 8 max:%ldns\r\n", PRESCALER_8_MAX_NS);
		return PRESCALER_8(3);
	}
	DEBUG("PRESCALER 1 max:%ldns\r\n", PRESCALER_1_MAX_NS);
	return PRESCALER_1(3);
}

/**
 * Compute number of ticks for the given clocksource and interval
 */
uint16_t
get_timer3_ticks(uint8_t clksrc, uint32_t nano)
{

	switch (clksrc & PRESCALER_MASK(3)) {
	case PRESCALER_1(3):
		return CLOCK_nHZ * nano;
	case PRESCALER_8(3):
		return (CLOCK_nHZ * nano) / 8;
	case PRESCALER_64(3):
		return (CLOCK_nHZ * nano) / 64;
	case PRESCALER_256(3):
		return (CLOCK_nHZ * nano) / 256;
	case PRESCALER_1024(3):
		return (CLOCK_nHZ * nano) / 1024;
	default:
		return 0;
	}
}
