/*
  Copyright 2018, 2019  Alfredo Mazzinghi

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

#ifndef _BACKLIGHT_H_
#define _BACKLIGHT_H_

#include <stdint.h>

/**
 * Backlight Driver I2C BUS address (base)
 * b1010 000x
 * b1010 000x
 * The least significant bit (addr[0]) is the R/W bit.
 * addr[0] == 0 -> write
 * addr[0] == 1 -> read
 */
#define I2C_BACKLIGHT_BUSADDR 0xA0

/**
 * Magic value to write in BCR_WRITE_LOCK register
 * to enable the next write to the command register.
 */
#define WRITE_LOCK_ENABLE_MAGIC 0xC5

struct LedColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

extern struct LedColor bright_white;
extern struct LedColor white;
extern struct LedColor red;
extern struct LedColor green;
extern struct LedColor blue;
extern struct LedColor yellow;
extern struct LedColor cyan;
extern struct LedColor magenta;
extern struct LedColor custom;
extern struct LedColor custom1;
extern struct LedColor custom2;
extern struct LedColor black;

struct CommandRegisterState {
  /** LED control register onoff section */
  uint8_t c_onoff[0x48];
  /** Pulse Width modulation register state */
  uint8_t c_pwm[0xc0];
  /** Auto Breadth Mode register state */
  uint8_t c_abm[0xc0];
  /** Function register state */
  uint8_t c_func[0x12];
};

/**
 * Driver state of the is3733 chip.
 */
struct IS3733_State {
  /** I2C bus address of the device */
  uint8_t bus_addr;
  /** Command register state */
  struct CommandRegisterState is_command;
  /** Interrupt mask register */
  uint8_t is_intr_mask;
};

/**
 * Byte address of the configuration registers.
 * This is written after the I2C BUS address to
 * select the register to write to.
 */
enum BacklightConfigureRegister {
	/** Command register address */
	BCR_COMMAND = 0xFD,
	/** Command register write lock */
	BCR_WRITE_LOCK = 0xFE,
	/** Interrupt mask register */
	BCR_INTR_MASK = 0xF0,
	/** Interrupt status register */
	BCR_INTR_STATUS = 0xF1,
};

/**
 * Data values for the BCR_COMMAND register configuration.
 * Each value selects a different page to which the following
 * data to the BCR_COMMAND register will be written to.
 */
enum CommandRegisterPage {
	/** LED control register page */
	CRP_LED_CTRL = 0x00,
	/** LED PWM register page */
	CRP_LED_PWM = 0x01,
	/** LED Auto Breath Mode */
	CRP_AUTO_BREATH_MODE = 0x02,
	/** Function register page */
	CRP_FUNCTION = 0x03,
};

/**
 * Offsets in the led control register page (CRP_LED_CTRL)
 * in the BCR_COMMAND register configuration.
 */
enum LedControlOffset {
	/** On/Off state for each led */
	LCO_ONOFF = 0x00,
	/** Readonly open detection state */
	LCO_OPEN = 0x18,
	/** Readonly LED short detection state */
	LCO_SHORT = 0x30,
	LCO_END = 0x48,
};

/**
 * Offsets in the led function register page (CRP_FUNCTION)
 * in the BCR_COMMAND register configuration.
 */
enum LedFunctionOffset {
	LFO_CONF = 0x00,
	LFO_GLOBAL_CURRENT_CTRL = 0x01,
	LFO_ABM1_C1 = 0x02,
	LFO_ABM1_C2 = 0x03,
	LFO_ABM1_C3 = 0x04,
	LFO_ABM1_C4 = 0x05,
	LFO_ABM2_C1 = 0x06,
	LFO_ABM2_C2 = 0x07,
	LFO_ABM2_C3 = 0x08,
	LFO_ABM2_C4 = 0x09,
	LFO_ABM3_C1 = 0x0A,
	LFO_ABM3_C2 = 0x0B,
	LFO_ABM3_C3 = 0x0C,
	LFO_ABM3_C4 = 0x0D,
	LFO_TIME_UPDATE = 0x0E,
	LFO_SW_PULLUP = 0x0F,
	LFO_CS_PULLDOWN = 0x10,
	LFO_RESET = 0x11,
};


/**
 * Values of the Led Function Configuration Register in the CRP_FUNCTION
 * page.
 */
enum LedFunctionConfiguration {
	LED_FN_CONF_SSD = 1,
	LED_FN_CONF_B_EN = (1 << 1),
	LED_FN_CONF_OSD = (1 << 2),
	LED_FN_CONF_SYNC_MASTER = (1 << 6),
	LED_FN_CONF_SYNC_SLAVE = (1 << 7),
};

/**
 * Auto breath mode channel
 */
enum ABMChannel {
	ABM_PWM = 0x00,
	ABM_CHANNEL_1 = 0x01,
	ABM_CHANNEL_2 = 0x02,
	ABM_CHANNEL_3 = 0x03,
};

/**
 * Initialize the backlight driver.
 */
void backlight_init(struct IS3733_State *state, uint8_t addr);

void backlight_reset(struct IS3733_State *state);
void backlight_disable(struct IS3733_State *state);

/* Rows and columns here are 0-based */
int backlight_set(struct IS3733_State *state, uint16_t row, uint16_t col, struct LedColor lc);
int backlight_set_all(struct IS3733_State *state, int half, struct LedColor lc);
int backlight_abm_set(struct IS3733_State *state, uint16_t row, uint16_t col, enum ABMChannel abm);
int backlight_off(struct IS3733_State *state, uint16_t row, uint16_t col);
int backlight_brightness(struct IS3733_State *state, uint8_t value);

/**
 * Check backlight Led open and short.
 */
int backlight_check_trigger(struct IS3733_State *state);
int backlight_check(struct IS3733_State *state);

/**
 * Top level operations
 */
int backlight_set_pattern(struct IS3733_State *state);
int backlight_set_animation(struct IS3733_State *state);


#endif /* _BACKLIGHT_H_ */
