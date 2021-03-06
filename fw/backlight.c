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

#include <assert.h>
#include <stdbool.h>

#include <avr/io.h>

#include <LUFA/Drivers/Peripheral/TWI.h>

#include "backlight.h"
#include "error.h"
#include "keyboard_tester.h"

struct LedColor bright_white = {255, 255, 255};
struct LedColor white = {128, 128, 128};
struct LedColor red = {128, 0, 0};
struct LedColor green = {0, 128, 0};
struct LedColor blue = {0, 0, 128};
struct LedColor yellow = {128, 128, 0};
struct LedColor cyan = {0, 128, 128};
struct LedColor magenta = {128, 0, 128};
struct LedColor custom = {142, 13, 216};
struct LedColor custom1 = {5, 50, 175};
struct LedColor custom2 = {90, 50, 85};
struct LedColor black = {0, 0, 0};

static int is3733_unlock_cmd(uint8_t addr);
static int is3733_set_cmd_page(uint8_t addr, uint8_t page);
static int is3733_read_cmd(
  uint8_t *value, uint8_t addr, uint8_t page, uint8_t offset);
static int is3733_read_cmd_buf(
  uint8_t *value, size_t size, uint8_t addr, uint8_t page, uint8_t offset);
static int is3733_write_cmd(
  uint8_t value, uint8_t addr, uint8_t page, uint8_t offset);
static int is3733_write_cmd_buf(
  const uint8_t *value, size_t size, uint8_t addr,
  uint8_t page, uint8_t offset);
static int is3733_read_reg(uint8_t *value, uint8_t addr, uint8_t offset);
static int is3733_write_reg(uint8_t value, uint8_t addr, uint8_t offset);

static int
is3733_read_cmd(uint8_t *value, uint8_t addr, uint8_t page, uint8_t offset)
{
  int rc = ERR_I2C;

  /* DEBUG("[%s] addr:%hhx pg:%hhx off:%hhx\r\n", __func__, */
  /* addr, page, offset); */

  /* Set the requested page */
  if (is3733_set_cmd_page(addr, page) != ERR_OK)
    return rc;

  /* Unlock command register for writing */
  if (is3733_unlock_cmd(addr) != ERR_OK)
    return rc;

  /* Select the given offset in the page */
  if (TWI_StartTransmission(addr | TWI_ADDRESS_WRITE, 10) ==
      TWI_ERROR_NoError) {
    if (!TWI_SendByte(offset)) {
      /* DEBUG("[%s] Can not select register offset %hhx:%hhx:%hhx\r\n", __func__, */
      /* addr, page, offset); */
      goto fail;
    }
    TWI_StopTransmission();
  }

  /* Start the read transmission at the selected page::offset */
  if (TWI_StartTransmission(addr | TWI_ADDRESS_READ, 10) == TWI_ERROR_NoError) {
    if (!TWI_ReceiveByte(value, true)) {
      /* DEBUG("[%s] Can not read cmd register %hhx:%hhx:%hhx\r\n", __func__, */
      /* 	    addr, page, offset); */
      goto fail;
    }
    rc = ERR_OK;
  }

  /* DEBUG("[%s] %hhx:%hhx:%hhx -> %hhx\r\n", __func__, */
  /* 	addr, page, offset, *value); */

 fail:
  TWI_StopTransmission();
  return rc;
}

static int
is3733_read_cmd_buf(uint8_t *value, size_t size, uint8_t addr,
		    uint8_t page, uint8_t offset)
{
  int rc = ERR_I2C;

  /* DEBUG("[%s] %hhx:%hhx:%hhx -> buf@%p\r\n", __func__, */
  /* 	addr, page, offset, value); */

  /* Set the requested page */
  if (is3733_set_cmd_page(addr, page) != ERR_OK)
    return rc;

  /* Unlock command register for writing the offset. */
  if (is3733_unlock_cmd(addr) != ERR_OK)
    return rc;

  for (int i = 0; i < size; i++) {
    /* Select the offset in the page */
    if (TWI_StartTransmission(addr | TWI_ADDRESS_WRITE, 10) == TWI_ERROR_NoError) {
      if (!TWI_SendByte(offset + i)) {
	/* DEBUG("[%s] Can not select register offset %hhx:%hhx:%hhx\r\n", __func__, */
	/*       addr, page, offset + i); */
	goto fail;
      }
      TWI_StopTransmission();
    }

     /* Start read of size bites starting at offset. */
    if (TWI_StartTransmission(addr | TWI_ADDRESS_READ, 10) == TWI_ERROR_NoError) {
      if (!TWI_ReceiveByte(&value[i], true)) {
	/* DEBUG("[%s] Can not read cmd register byte %hhx:%hhx:%hhx\r\n", */
	/*       __func__, addr, page, offset + i); */
	goto fail;
      }
      TWI_StopTransmission();
    }
  }
  return ERR_OK;

 fail:
  TWI_StopTransmission();
  return rc;
}

static int
is3733_write_cmd(uint8_t value, uint8_t addr, uint8_t page, uint8_t offset)
{
  int rc = ERR_I2C;

  /* DEBUG("[%s] %hhx:%hhx:%hhx <- %hhx\r\n", __func__, */
  /* 	addr, page, offset, value); */

  /* Select the requested page */
  if (is3733_set_cmd_page(addr, page) != ERR_OK)
    return rc;

  /* Unlock the command register for writing */
  if (is3733_unlock_cmd(addr) != ERR_OK)
    return rc;

  /* Write the data at the requested page::offset */
  if (TWI_StartTransmission(addr | TWI_ADDRESS_WRITE, 10) == TWI_ERROR_NoError) {
    if (!TWI_SendByte(offset)) {
      /* DEBUG("[%s] Can not send offset byte\r\n", __func__); */
      goto fail;
    }
    if (!TWI_SendByte(value)) {
      /* DEBUG("[%s] Can not send value byte\r\n", __func__); */
      goto fail;
    }
    rc = ERR_OK;
  }

 fail:
  TWI_StopTransmission();
  return rc;
}

static int
is3733_write_cmd_buf(const uint8_t *value, size_t size, uint8_t addr,
		      uint8_t page, uint8_t offset)
{
  int rc = ERR_I2C;

  /* DEBUG("[%s] %hhx:%hhx:%hhx <- buf@%p\r\n", __func__, */
  /* 	addr, page, offset, value); */

  /* Select the requested page */
  if (is3733_set_cmd_page(addr, page) != ERR_OK)
    return rc;

  /* Unlock command register for writing again */
  if (is3733_unlock_cmd(addr) != ERR_OK)
    return rc;

  /* Select offset in the page and write bytes */
  if (TWI_StartTransmission(addr | TWI_ADDRESS_WRITE, 10) == TWI_ERROR_NoError) {
    if (!TWI_SendByte(offset)) {
      /* DEBUG("[%s] Can not select register offset %hhx:%hhx:%hhx\r\n", */
      /* 	    __func__, addr, page, offset); */
      goto fail;
    }

    for (int i = 0; i < size; i++) {
      if (!TWI_SendByte(value[i])) {
	/* DEBUG("[%s] Can not write cmd register byte %hhx:%hhx:%hhx[%d]\r\n", */
	/*       __func__, addr, page, offset, i); */
	goto fail;
      }
    }
    rc = ERR_OK;
  }

 fail:
  TWI_StopTransmission();
  return rc;
}

static int
is3733_read_reg(uint8_t *value, uint8_t addr, uint8_t offset)
{
  int rc = ERR_I2C;

  /* DEBUG("[%s] %hhx:%hhx\r\n", __func__, addr, offset); */

  if (TWI_StartTransmission(addr | TWI_ADDRESS_WRITE, 10) ==
      TWI_ERROR_NoError) {
    if (!TWI_SendByte(offset)) {
      /* DEBUG("[%s] Can not select register offset %hhx:%hhx\r\n", __func__, */
      /* 	    addr, offset); */
      goto fail;
    }
    TWI_StopTransmission();
  }
  if (TWI_StartTransmission(addr | TWI_ADDRESS_READ, 10) ==
      TWI_ERROR_NoError) {
    if (!TWI_ReceiveByte(value, true)) {
      /* DEBUG("[%s] Can not read register %hhx:%hhx\r\n", __func__, */
      /* 	    addr, offset); */
      goto fail;
    }
    rc = ERR_OK;
  }

  /* DEBUG("[%s] %hhx:%hhx -> %hhx\r\n", __func__, addr, offset, *value); */

 fail:
  TWI_StopTransmission();
  return rc;
}

static int
is3733_write_reg(uint8_t value, uint8_t addr, uint8_t offset)
{
  int rc = ERR_I2C;

  /* DEBUG("[%s] %hhx:%hhx <- %hhx\r\n", __func__, addr, offset, value); */

  if (TWI_StartTransmission(addr | TWI_ADDRESS_WRITE, 10) ==
      TWI_ERROR_NoError) {
    if (!TWI_SendByte(offset)) {
      /* DEBUG("[%s] Can not send offset byte\r\n", __func__); */
      goto fail;
    }
    if (!TWI_SendByte(value)) {
      /* DEBUG("[%s] Can not send value byte\r\n", __func__); */
      goto fail;
    }
    rc = ERR_OK;
  }

 fail:
  TWI_StopTransmission();
  return rc;
}

/**
 * Select a page in the command register.
 * This automatically unlocks the command register for writing.
 * After the page is selected, it is possible to send the byte
 * offset in the page along with the data.
 */
static int
is3733_set_cmd_page(uint8_t addr, uint8_t page)
{
  int rc = ERR_I2C;

  if (is3733_unlock_cmd(addr) != ERR_OK)
    return rc;

  if (TWI_StartTransmission(addr | TWI_ADDRESS_WRITE, 10) ==
      TWI_ERROR_NoError) {
    if (!TWI_SendByte(BCR_COMMAND)) {
      /* DEBUG("[%s] Can not select command register.\r\n", __func__); */
      goto fail;
    }
    if (!TWI_SendByte(page)) {
      /* DEBUG("[%s] Can not send page address.\r\n", __func__); */
      goto fail;
    }
    rc = ERR_OK;
  }

 fail:
  TWI_StopTransmission();
  return rc;
}

/**
 * Unlock next write to command register.
 */
static int
is3733_unlock_cmd(uint8_t addr)
{
  return is3733_write_reg(WRITE_LOCK_ENABLE_MAGIC, addr, BCR_WRITE_LOCK);
}

void
backlight_init(struct IS3733_State *state, uint8_t addr)
{
  /*
   * Set port E mode to general purpose I/O
   * output pin: 2 -- no pullup
   */
  PORTE = 0;
  DDRE = (1 << DDE2);
  // enable led driver pulling SDB (port PE2) to HIGH
  PORTE |= (1 << PE2);

  /* Initialize TWI at 500kHz */
  TWI_Init(TWI_BIT_PRESCALE_1, TWI_BITLENGTH_FROM_FREQ(1, 500000));

  memset(state, 0, sizeof(*state));
  state->bus_addr = addr;
}

void
backlight_reset(struct IS3733_State *state)
{
  uint8_t tmp, rc;
  uint8_t addr = state->bus_addr;

  // clear state of all LEDs
  memset(state, 0, sizeof(*state));
  state->bus_addr = addr;

  DEBUG("[%s] Reset backlight driver @%hhx\r\n", __func__, addr);

  // read reset register
  rc = is3733_read_cmd(&tmp, state->bus_addr, CRP_FUNCTION, LFO_RESET);
  state->is_command.c_func[LFO_RESET] = tmp;
  if (rc != ERR_OK) {
    DEBUG("Can not reset backlight\r\n");
    return;
  }

  // clear software shutdown
  rc = is3733_write_cmd(LED_FN_CONF_SSD, state->bus_addr, CRP_FUNCTION,
			LFO_CONF);
  if (rc != ERR_OK) {
    DEBUG("Can not clear software shutdown\r\n");
    return;
  }
  state->is_command.c_func[LFO_CONF] = LED_FN_CONF_SSD;

  // clear global current control register
  rc = is3733_write_cmd(0x00, state->bus_addr, CRP_FUNCTION,
			LFO_GLOBAL_CURRENT_CTRL);
  if (rc != ERR_OK) {
    DEBUG("Can not reset Global Current Control\r\n");
    return;
  }
  state->is_command.c_func[LFO_GLOBAL_CURRENT_CTRL] = 0x0;

  /*
   * Enable LED pull-up and pull-down resistors.
   */
  rc = is3733_write_cmd(0x7, state->bus_addr, CRP_FUNCTION,
			LFO_SW_PULLUP);
  if (rc += ERR_OK) {
    DEBUG("Can not configure SW Pullup\r\n");
    return;
  }
  rc = is3733_write_cmd(0x0, state->bus_addr, CRP_FUNCTION,
			LFO_CS_PULLDOWN);
  if (rc += ERR_OK) {
    DEBUG("Can not configure CS Pulldown\r\n");
    return;
  }

  /*
   * Enable leds in our matrix.
   * The left keypad is attached to the CS4-CS6 and SW1-SW6
   * portion of the grid.
   * The right keypad is attached to the CS1-CS3 and SW1-SW6
   * portion of the grid.
   */
  state->is_command.c_onoff[LCO_ONOFF + 0x00] = 0x3f;
  state->is_command.c_onoff[LCO_ONOFF + 0x02] = 0x3f;
  state->is_command.c_onoff[LCO_ONOFF + 0x04] = 0x3f;
  state->is_command.c_onoff[LCO_ONOFF + 0x06] = 0x3f;
  state->is_command.c_onoff[LCO_ONOFF + 0x08] = 0x3f;
  state->is_command.c_onoff[LCO_ONOFF + 0x0A] = 0x3f;
  rc = is3733_write_cmd_buf(&state->is_command.c_onoff[LCO_ONOFF], 0x0B,
  			    state->bus_addr, CRP_LED_CTRL, LCO_ONOFF);
  if (rc != ERR_OK) {
    DEBUG("Can not configure LED on\r\n");
    return;
  }

  /* Make sure we clear the PWM page. */
  rc = is3733_write_cmd_buf(&state->is_command.c_pwm[0], 0xC0, state->bus_addr,
			    CRP_LED_PWM, 0);
  if (rc != ERR_OK) {
    DEBUG("Can not reset LED PWM\r\n");
    return;
  }
}

void
backlight_disable(struct IS3733_State *state)
{

}

int
backlight_brightness(struct IS3733_State *state, uint8_t value) {

  int rc;

  rc = is3733_write_cmd(value, state->bus_addr, CRP_FUNCTION,
			LFO_GLOBAL_CURRENT_CTRL);
  if (rc != ERR_OK) {
    DEBUG("Can not set backlight brightness to %hhx\r\n", value);
    return rc;
  }
  state->is_command.c_func[LFO_GLOBAL_CURRENT_CTRL] = value;

  return rc;
}

int
backlight_set(struct IS3733_State *state, uint16_t row, uint16_t col, struct LedColor lc)
{
  int rc = ERR_BACKLIGHT;
  uint8_t col_offset, col_index;
  uint8_t led_status;
  int index;

  if (row > 3) {
    return rc;
  }
  if (col > 15) {
    return rc;
  }
  col_offset = (col > 7) ? 1 : 0;
  col_index = (col > 7) ? col - 8 : col;
  row = row * 3;

  /* Check status for all LED channels */
  led_status = state->is_command.c_onoff[LCO_ONOFF + row * 2 + col_offset];
  if ((led_status & (1 << col_index)) == 0) {
    return rc;
  }
  led_status = state->is_command.c_onoff[LCO_ONOFF + (row + 1) * 2 + col_offset];
  if ((led_status & (1 << col_index)) == 0) {
    return rc;
  }

  led_status = state->is_command.c_onoff[LCO_ONOFF + (row + 2) * 2 + col_offset];
  if ((led_status & (1 << col_index)) == 0) {
    return rc;
  }

  /* Update PWM for each channel */
  index = row * 0x10 + col;
  state->is_command.c_pwm[index] = lc.b;
  DEBUG("[%s] B(%hx, %hx) @ PWM[%x] <- %hhx\r\n", __func__, row, col,
  	index, lc.b);
  rc = is3733_write_cmd(lc.b, state->bus_addr, CRP_LED_PWM, index);
  if (rc != ERR_OK) {
    return rc;
  }

  index = (row + 1) * 0x10 + col;
  state->is_command.c_pwm[index] = lc.g;
  DEBUG("[%s] G(%hx, %hx) @ PWM[%x] <- %hhx\r\n", __func__, row, col,
  	index, lc.g);
  rc = is3733_write_cmd(lc.g, state->bus_addr, CRP_LED_PWM, index);
  if (rc != ERR_OK) {
    return rc;
  }

  index = (row + 2) * 0x10 + col;
  state->is_command.c_pwm[index] = lc.r;
  DEBUG("[%s] R(%hx, %hx) @ PWM[%x] <- %hhx\r\n", __func__, row, col,
  	index, lc.r);
  rc = is3733_write_cmd(lc.r, state->bus_addr, CRP_LED_PWM, index);
  if (rc != ERR_OK) {
    return rc;
  }

  return ERR_OK;
}

int
backlight_abm_set(struct IS3733_State *state, uint16_t row, uint16_t col,
		  enum ABMChannel abm)
{
  int rc;
  int index;

  /* Convert from matrix row to base LED row */
  row = row * 3;

  /* Set ABM channel for each LED */
  index = row * 0x10 + col;
  rc = is3733_write_cmd(0x01, state->bus_addr, CRP_AUTO_BREATH_MODE,
			index);
  DEBUG("[%s] B(%hx, %hx) @ ABM[%x] <- %hhx\r\n", __func__, row, col,
  	index, abm);
  if (rc != ERR_OK) {
    return rc;
  }

  index = (row + 1) * 0x10 + col;
  rc = is3733_write_cmd(0x01, state->bus_addr, CRP_AUTO_BREATH_MODE,
			index);
  DEBUG("[%s] G(%hx, %hx) @ ABM[%x] <- %hhx\r\n", __func__, row, col,
  	index, abm);
  if (rc != ERR_OK) {
    return rc;
  }

  index = (row + 2) * 0x10 + col;
  rc = is3733_write_cmd(0x01, state->bus_addr, CRP_AUTO_BREATH_MODE,
			index);
  DEBUG("[%s] R(%hx, %hx) @ ABMd[%x] <- %hhx\r\n", __func__, row, col,
  	index, abm);
  if (rc != ERR_OK) {
    return rc;
  }

  return ERR_OK;
}


int
backlight_off(struct IS3733_State *state, uint16_t row, uint16_t col)
{
  int rc = ERR_BACKLIGHT;

  return rc;
}

int
backlight_check_trigger(struct IS3733_State *state)
{
  int rc = ERR_I2C;
  uint8_t conf = state->is_command.c_func[LFO_CONF];

  DEBUG("[%s] Trigger open-short detection\r\n", __func__);

  /* Set global current control for open short detect (datasheet) */
  rc = is3733_write_cmd(0x01, state->bus_addr, CRP_FUNCTION,
			LFO_GLOBAL_CURRENT_CTRL);
  if (rc != ERR_OK) {
    DEBUG("[%s] Can not set GCC=0x01\r\n", __func__);
    return rc;
  }

  /* Clear OSD bit */
  conf &= ~(uint8_t)(LED_FN_CONF_OSD);

  rc = is3733_write_cmd(conf, state->bus_addr, CRP_FUNCTION, LFO_CONF);
  if (rc != ERR_OK) {
    DEBUG("[%s] Can not clear backlight open-short-detect bit %hhx\r\n",
	  __func__, conf);
    return rc;
  }
  state->is_command.c_func[LFO_CONF] = conf;

  /* Set OSD bit */
  conf |= (uint8_t)(LED_FN_CONF_OSD);

  rc = is3733_write_cmd(conf, state->bus_addr, CRP_FUNCTION, LFO_CONF);
  if (rc != ERR_OK) {
    DEBUG("[%s] Can not set backlight open-short-detect bit %hhx\r\n",
	  __func__, conf);
    return rc;
  }
  state->is_command.c_func[LFO_CONF] = conf;

  return ERR_OK;
}

int
backlight_check(struct IS3733_State *state)
{
  int rc = ERR_I2C;

  DEBUG("[%s] Check open-short detection result\r\n", __func__);

  rc = is3733_read_cmd_buf(&state->is_command.c_onoff[LCO_OPEN],
  			   LCO_END - LCO_OPEN,
  			   state->bus_addr,
  			   CRP_LED_CTRL, LCO_OPEN);
  if (rc != ERR_OK) {
    DEBUG("Can not read backlight open-short-detect page\r\n");
    return rc;
  }

  DEBUG("Led OPEN region dump:\r\n");
  for (int idx = LCO_OPEN; idx < LCO_SHORT; idx += 4) {
    DEBUG("[%hhx] %hhx %hhx %hhx %hhx\r\n",
	  idx,
	  state->is_command.c_onoff[idx],
	  state->is_command.c_onoff[idx + 1],
	  state->is_command.c_onoff[idx + 2],
	  state->is_command.c_onoff[idx + 3]);
  }

  DEBUG("Led SHORT region dump:\r\n");
  for (int idx = LCO_SHORT; idx < LCO_END; idx += 4) {
    DEBUG("[%hhx] %hhx %hhx %hhx %hhx\r\n",
	  idx,
	  state->is_command.c_onoff[idx],
	  state->is_command.c_onoff[idx + 1],
	  state->is_command.c_onoff[idx + 2],
	  state->is_command.c_onoff[idx + 3]);
  }

  return ERR_OK;
}

int
backlight_set_all(struct IS3733_State *state, int lr, struct LedColor lc) {
  int rc;

  rc = is3733_write_cmd(0x01, state->bus_addr, CRP_FUNCTION, LFO_CONF);
  if (rc != ERR_OK)
    return rc;

  if (lr == 0) {
    backlight_set(state, 0, 3, lc);
    backlight_set(state, 0, 5, lc);
    backlight_set(state, 1, 3, lc);
    backlight_set(state, 1, 4, lc);
    backlight_set(state, 1, 5, lc);
  }
  else {
    backlight_set(state, 0, 0, lc);
    backlight_set(state, 0, 2, lc);
    backlight_set(state, 1, 0, lc);
    backlight_set(state, 1, 1, lc);
    backlight_set(state, 1, 2, lc);
  }

  return ERR_OK;
}

int
backlight_set_pattern(struct IS3733_State *state) {

  int rc;

  //backlight_abm_set(state, 1, 3, ABM_PWM);
  /* backlight_abm_set(state, 1, 4, ABM_DISABLE); */
  /* backlight_abm_set(state, 1, 5, ABM_DISABLE); */
  rc = is3733_write_cmd(0x01, state->bus_addr, CRP_FUNCTION, LFO_CONF);
  if (rc != ERR_OK)
    return rc;

  backlight_brightness(state, 255);
  backlight_set(state, 0, 3, white);
  backlight_set(state, 0, 5, custom2);
  backlight_set(state, 1, 3, red);
  backlight_set(state, 1, 4, green);
  backlight_set(state, 1, 5, blue);

  backlight_set(state, 0, 0, custom);
  backlight_set(state, 0, 2, red);
  backlight_set(state, 1, 0, red);
  backlight_set(state, 1, 1, green);
  backlight_set(state, 1, 2, blue);

  return ERR_OK;
}

#if 0
int
backlight_set_animation(struct IS3733_State *state) {

  int rc;

  /* Clear B_EN */
  rc = is3733_write_cmd(state->is_command.c_func[LFO_CONF] & ~(0x02),
			state->bus_addr, CRP_FUNCTION, LFO_CONF);
  if (rc != ERR_OK)
    return rc;

  backlight_brightness(state, 255);
  backlight_set(state, 0, 3, white);
  backlight_set(state, 0, 5, white);
  backlight_set(state, 1, 3, red);
  backlight_set(state, 1, 4, green);
  backlight_set(state, 1, 5, blue);

  /* Select auto breath mode index for each of the second row LEDs */
  backlight_abm_set(state, 1, 3, ABM_CHANNEL_1);
  backlight_abm_set(state, 1, 4, ABM_CHANNEL_2);
  backlight_abm_set(state, 1, 5, ABM_CHANNEL_3);

  /* Configure ABM1
   * Loop 2 times start=off end=off 1.68s rise 3.36s hold
   */
  rc = is3733_write_cmd((0x03 << 5) | (0x05 << 1), state->bus_addr,
			CRP_FUNCTION, LFO_ABM1_C1);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd((0x03 << 5) | (0x05 << 1), state->bus_addr,
			CRP_FUNCTION, LFO_ABM1_C2);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd(0x00, state->bus_addr, CRP_FUNCTION, LFO_ABM1_C3);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd(0x02, state->bus_addr, CRP_FUNCTION, LFO_ABM1_C4);
  if (rc != ERR_OK)
    return rc;

  /* Configure ABM2
   * Loop 2 times start=on end=off 3.36s rise 1.68s hold
   */
  rc = is3733_write_cmd((0x04 << 5) | (0x04 << 1), state->bus_addr,
			CRP_FUNCTION, LFO_ABM2_C1);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd((0x04 << 5) | (0x04 << 1), state->bus_addr,
			CRP_FUNCTION, LFO_ABM2_C2);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd(0x01 << 4, state->bus_addr, CRP_FUNCTION, LFO_ABM2_C3);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd(0x02, state->bus_addr, CRP_FUNCTION, LFO_ABM2_C4);
  if (rc != ERR_OK)
    return rc;

  /* Configure ABM3
   * Loop 4 times start=off end=on 0.84s rise 1.68s hold
   */
  rc = is3733_write_cmd((0x02 << 5) | (0x04 << 1), state->bus_addr,
			CRP_FUNCTION, LFO_ABM3_C1);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd((0x02 << 5) | (0x04 << 1), state->bus_addr,
			CRP_FUNCTION, LFO_ABM3_C2);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd(0x01 << 4, state->bus_addr, CRP_FUNCTION, LFO_ABM3_C3);
  if (rc != ERR_OK)
    return rc;
  rc = is3733_write_cmd(0x01 << 6, state->bus_addr, CRP_FUNCTION, LFO_ABM3_C4);
  if (rc != ERR_OK)
    return rc;

  /* Set B_EN */
  rc = is3733_write_cmd(state->is_command.c_func[LFO_CONF] | (1 << 1),
			state->bus_addr, CRP_FUNCTION, LFO_CONF);
  if (rc != ERR_OK)
    return rc;

  /* Reset time update register */
  rc = is3733_write_cmd(0x00, state->bus_addr, CRP_FUNCTION, LFO_TIME_UPDATE);
  if (rc != ERR_OK)
    return rc;
  return ERR_OK;
}
#endif
