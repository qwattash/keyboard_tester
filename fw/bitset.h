/*
  Copyright 2018  Alfredo Mazzinghi

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

/**
 * @file
 * Macros to define and access an uint8_t array bitset
 */

#ifndef _BITSET_H_
#define _BITSET_H_

#include <stdint.h>
#include <string.h>

/**
 * Number of bits in an uint8_t
 */
#define NBBY 8

/**
 * Size of the bitset internal array
 */
#define BITSET_SIZE(bits) (((bits) / NBBY) + 1)

/**
 * Declare opaque bitset data structure
 */
#define BITSET_DECLARE(name, bits)					\
	typedef struct _##name {					\
		uint8_t _b[BITSET_SIZE((bits))];			\
	} name

/**
 * Index to block index
 */
#define BITSET_BLOCK(index) (index / NBBY)

/**
 * Index to offset in a block
 */
#define BITSET_OFF(index) (index % NBBY)

/**
 * Get bit at the given index
 */
#define BITSET_GET(bset, index)					\
	(((bset)._b[BITSET_BLOCK(index)] & (1 << BITSET_OFF(index))) >> \
	 BITSET_OFF(index))

/**
 * Set bit at the given index
 */
#define BITSET_SET(bset, index) do {					\
		(bset)._b[BITSET_BLOCK(index)] |= (1 << BITSET_OFF(index)); \
	} while (0)

/**
 * Clear bit at the given index
 */
#define BITSET_CLEAR(bset, index) do {					\
		(bset)._b[BITSET_BLOCK(index)] &= ~(1 << BITSET_OFF(index)); \
	} while (0)

/**
 * Iterate over each bit in the bitset
 */
#define BITSET_FOREACH(idxvar, valvar, bset)				\
	for ((idxvar) = 0, (valvar) = ((bset)._b[0] & 0x01);		\
	     idxvar < sizeof((bset)._b) * NBBY;				\
	     idxvar++, valvar = BITSET_GET(bset, idxvar))

/**
 * Clear bitset
 */
#define BITSET_CLEAR_ALL(bset) memset(&(bset)._b, 0, sizeof((bset)._b));

#endif /* _BITSET_H_ */
