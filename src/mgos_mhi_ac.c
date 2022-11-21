/*
 * Copyright 2022 nikwest <github@nikwest.de>, MEL original by d4rkmen <darkmen@i.ua>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <driver/spi_slave.h>

#include "mgos.h"
#include "mgos_mhi_ac_internal.h"
#include "mgos_rpc.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#define RCV_HOST    HSPI_HOST
#else
#define RCV_HOST    SPI2_HOST
#endif

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

static struct mgos_mhi_ac *mhi = NULL;

static uint16_t mgos_mhi_calc_crc(uint8_t* frame, int len) {
  uint16_t checksum = 0;
  for (int i = 0; i < len; i++) {
    checksum += frame[i];
  }
  return checksum;
}

void mgos_mhi_ac_create() {
  LOG(LL_DEBUG, ("Creating MHI-AC object..."));
  mhi = calloc(1, sizeof(struct mgos_mhi_ac));
  if (!mhi) return;
  // Init all the structure members
  memset((void *) mhi, 0, sizeof(struct mgos_mhi_ac));

  // Initialize SPI slave, using IDF esp32 as mongoose SPI slave is n/a
  esp_err_t ret;

  //Configuration for the SPI bus
  const struct mgos_config_spi *cfg = mgos_sys_config_get_spi();
  spi_bus_config_t buscfg={
      .mosi_io_num=cfg->mosi_gpio,
      .miso_io_num=cfg->miso_gpio,
      .sclk_io_num=cfg->sclk_gpio,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };

  //Configuration for the SPI slave interface
  spi_slave_interface_config_t slvcfg={
      .mode=3, // CPOL=1, CPHA=1
      .spics_io_num=-1,
      .queue_size=3,
      .flags=0,
      .post_setup_cb=NULL,
      .post_trans_cb=NULL
  };

  //Initialize SPI slave interface
  ret=spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
  assert(ret==ESP_OK);

  LOG(LL_INFO, ("MHI-AC service created"));

  return;
}

void mgos_mhi_ac_destroy() {
  LOG(LL_DEBUG, ("Destroing MHI-AC object...."));
  if (mhi) {

  }
  free(mhi);
  mhi = NULL;
  return;
}

bool mgos_mhi_ac_set_power(enum mgos_mhi_ac_param_power power) {
  if (!mhi) return false;
  switch (power) {
    case MGOS_MHI_AC_PARAM_POWER_OFF:
      mhi->miso.frame.data[0] &= ~(1 << 0);
      mhi->miso.frame.data[0] |= (1 << 1);
      return true;
    case MGOS_MHI_AC_PARAM_POWER_ON:
      mhi->miso.frame.data[0] |= (1 << 0);
      mhi->miso.frame.data[0] |= (1 << 1);
      return true;
    default:
      return false;
  }
}

bool mgos_mhi_ac_set_mode(enum mgos_mhi_ac_param_mode mode) {
  if (!mhi) return false;
  switch (mode) {
    case MGOS_MHI_AC_PARAM_MODE_AUTO:
      mhi->miso.frame.data[0] &= ~(1 << 2);
      mhi->miso.frame.data[0] &= ~(1 << 3);
      mhi->miso.frame.data[0] &= ~(1 << 4);
      mhi->miso.frame.data[0] |= (1 << 5);
     return true;
    case MGOS_MHI_AC_PARAM_MODE_COOL:
      mhi->miso.frame.data[0] &= ~(1 << 2);
      mhi->miso.frame.data[0] |= (1 << 3);
      mhi->miso.frame.data[0] &= ~(1 << 4);
      mhi->miso.frame.data[0] |= (1 << 5);
      return true;
    case MGOS_MHI_AC_PARAM_MODE_DRY:
      mhi->miso.frame.data[0] |= (1 << 2);
      mhi->miso.frame.data[0] &= ~(1 << 3);
      mhi->miso.frame.data[0] &= ~(1 << 4);
      mhi->miso.frame.data[0] |= (1 << 5);
      return true;
    case MGOS_MHI_AC_PARAM_MODE_FAN:
      mhi->miso.frame.data[0] |= (1 << 2);
      mhi->miso.frame.data[0] |= (1 << 3);
      mhi->miso.frame.data[0] &= ~(1 << 4);
      mhi->miso.frame.data[0] |= (1 << 5);
      return true;
    case MGOS_MHI_AC_PARAM_MODE_HEAT:
      mhi->miso.frame.data[0] &= ~(1 << 2);
      mhi->miso.frame.data[0] &= ~(1 << 3);
      mhi->miso.frame.data[0] |= (1 << 4);
      mhi->miso.frame.data[0] |= (1 << 5);
      return true;
    default:
      return false;
  }
}

bool mgos_mhi_ac_set_setpoint(float setpoint) {
  if (!mhi) return false;
  if (setpoint > 31 || setpoint < 10) return false;
  mhi->miso.frame.data[2] = round(setpoint * 2);
  mhi->miso.frame.data[2] |= (1 << 7);
  return true;
}

bool mgos_mhi_ac_set_ext_temp(float temp) {
  if (!mhi) return false;
  if (temp > 48.25 || temp < -15.25) return false;
  mhi->miso.frame.data[3] = round(temp * 4) + 61;
  return true;
}

bool mgos_mhi_ac_set_fan(enum mgos_mhi_ac_param_fan fan) {
  if (!mhi) return false;
  switch (fan) {
    case MGOS_MHI_AC_PARAM_FAN_HIGH:
      mhi->miso.frame.data[1] &= ~(1 << 0);
      mhi->miso.frame.data[1] |= (1 << 1);
      mhi->miso.frame.data[1] |= (1 << 3);
      return true;
    case MGOS_MHI_AC_PARAM_FAN_LOW:
      mhi->miso.frame.data[1] &= ~(1 << 0);
      mhi->miso.frame.data[1] &= ~(1 << 1);
      mhi->miso.frame.data[1] |= (1 << 3);
      return true;
    case MGOS_MHI_AC_PARAM_FAN_MED:
      mhi->miso.frame.data[1] |= (1 << 0);
      mhi->miso.frame.data[1] &= ~(1 << 1);
      mhi->miso.frame.data[1] |= (1 << 3);
      return true;
    case MGOS_MHI_AC_PARAM_FAN_TURBO:
      mhi->miso.frame.data[6] |= (1 << 4);
      return true;
    default:
      return false;
  }
}

bool mgos_mhi_ac_set_vane_vert(enum mgos_mhi_ac_param_vane_vert vane_vert) {
  if (!mhi) return false;
  switch (vane_vert) {
    case MGOS_MHI_AC_PARAM_VANE_VERT_AUTO:
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_VERT_LEFTEST:
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_VERT_LEFT:
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_VERT_CENTER:
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_VERT_RIGHT:
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_VERT_RIGHTEST:
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_VERT_LEFTRIGHT:
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_VERT_SWING:
      mhi->miso.frame.data[0] |= (1 << 6);
      mhi->miso.frame.data[0] |= (1 << 7);
      return true;
    default:
      return false;
  }
}

bool mgos_mhi_ac_set_vane_horiz(enum mgos_mhi_ac_param_vane_horiz vane_horiz) {
  if (!mhi) return false;
  switch (vane_horiz) {
    case MGOS_MHI_AC_PARAM_VANE_HORIZ_1:
      mhi->miso.frame.data[1] &= ~(1 << 4);
      mhi->miso.frame.data[1] &= ~(1 << 5);
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_HORIZ_2:
      mhi->miso.frame.data[1] |= (1 << 4);
      mhi->miso.frame.data[1] &= ~(1 << 5);
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_HORIZ_3:
      mhi->miso.frame.data[1] &= ~(1 << 4);
      mhi->miso.frame.data[1] |= (1 << 5);
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_HORIZ_4:
      mhi->miso.frame.data[1] |= (1 << 4);
      mhi->miso.frame.data[1] |= (1 << 5);
      mhi->miso.frame.data[1] |= (1 << 7);
      return true;
    case MGOS_MHI_AC_PARAM_VANE_HORIZ_SWING:
      mhi->miso.frame.data[0] |= (1 << 6);
      mhi->miso.frame.data[0] |= (1 << 7);
      return true;
    default:
      return false;
  }
}

enum mgos_mhi_ac_param_power mgos_mhi_ac_get_power() {
  if (mhi)
    return (mhi->mosi.frame.data[0] & (1 << 0)) ? MGOS_MHI_AC_PARAM_POWER_ON : MGOS_MHI_AC_PARAM_POWER_OFF;
  else
    return MGOS_MHI_AC_PARAM_POWER_OFF;
}

enum mgos_mhi_ac_param_mode mgos_mhi_ac_get_mode() {
  if (!mhi) return MGOS_MHI_AC_PARAM_MODE_AUTO;
  u_int8_t d = mhi->mosi.frame.data[0];

  if(        CHECK_BIT(d, 2) == 0 
          && CHECK_BIT(d, 3) == 0 
          && CHECK_BIT(d, 4) == 0 ) {
    return MGOS_MHI_AC_PARAM_MODE_AUTO;
  } else if( CHECK_BIT(d, 2) == 0 
          && CHECK_BIT(d, 3) == 1 
          && CHECK_BIT(d, 4) == 0 ) {
    return MGOS_MHI_AC_PARAM_MODE_COOL;
  } else if( CHECK_BIT(d, 2) == 1 
          && CHECK_BIT(d, 3) == 0 
          && CHECK_BIT(d, 4) == 0 ) {
    return MGOS_MHI_AC_PARAM_MODE_DRY;
  } else if( CHECK_BIT(d, 2) == 1 
          && CHECK_BIT(d, 3) == 1 
          && CHECK_BIT(d, 4) == 0 ) {
    return MGOS_MHI_AC_PARAM_MODE_FAN;
  } else if( CHECK_BIT(d, 2) == 0 
          && CHECK_BIT(d, 3) == 0
          && CHECK_BIT(d, 4) == 1 ) {
    return MGOS_MHI_AC_PARAM_MODE_HEAT;
  }
  return MGOS_MHI_AC_PARAM_MODE_AUTO;
}

float mgos_mhi_ac_get_setpoint() {
  if (!mhi) return 0.0;
  u_int8_t d = mhi->mosi.frame.data[2] & ~(1 << 6);
  return (float) d / 2.0;
}

enum mgos_mhi_ac_param_fan mgos_mhi_ac_get_fan() {
  u_int8_t d1 = mhi->mosi.frame.data[1];
  u_int8_t d6 = mhi->mosi.frame.data[6];

  if(CHECK_BIT(d6, 6) == 1) {
    return MGOS_MHI_AC_PARAM_FAN_TURBO;
  }

  if(        CHECK_BIT(d1, 0) == 0 
          && CHECK_BIT(d1, 1) == 0 ) {
    return MGOS_MHI_AC_PARAM_FAN_LOW;
  } 
  
  if(        CHECK_BIT(d1, 0) == 1 
          && CHECK_BIT(d1, 1) == 0 ) {
    return MGOS_MHI_AC_PARAM_FAN_MED;
  } 

  if(        CHECK_BIT(d1, 0) == 0 
          && CHECK_BIT(d1, 1) == 1 ) {
    return MGOS_MHI_AC_PARAM_FAN_HIGH;
  } 

  return MGOS_MHI_AC_PARAM_FAN_MED;
}

enum mgos_mhi_ac_param_vane_vert mgos_mhi_ac_get_vane_vert() {
  // n/a
  return MGOS_MHI_AC_PARAM_VANE_VERT_AUTO;
}

enum mgos_mhi_ac_param_vane_horiz mgos_mhi_ac_get_vane_horiz() {
  u_int8_t d0 = mhi->mosi.frame.data[0];
  u_int8_t d1 = mhi->mosi.frame.data[1];

  if(CHECK_BIT(d0, 7) == 0 && CHECK_BIT(d1, 7) == 0) {
    LOG(LL_WARN, ("Lastest status not visible, changed by remote"));
  }

  if(CHECK_BIT(d0, 6) == 1) {
    return MGOS_MHI_AC_PARAM_VANE_HORIZ_SWING;
  }

  if(        CHECK_BIT(d1, 4) == 0 
          && CHECK_BIT(d1, 5) == 0 ) {
    return MGOS_MHI_AC_PARAM_VANE_HORIZ_1;
  } 
  
  if(        CHECK_BIT(d1, 4) == 1 
          && CHECK_BIT(d1, 5) == 0 ) {
    return MGOS_MHI_AC_PARAM_VANE_HORIZ_2;
  } 

  if(        CHECK_BIT(d1, 4) == 0 
          && CHECK_BIT(d1, 5) == 1 ) {
    return MGOS_MHI_AC_PARAM_VANE_HORIZ_3;
  } 

  return MGOS_MHI_AC_PARAM_VANE_HORIZ_4;
}

bool mgos_mhi_ac_get_operating() {
  u_int8_t d = mhi->mosi.frame.data[0];
  return CHECK_BIT(d, 0) == 1;
}

float mgos_mhi_ac_get_room_temperature() {
  if (!mhi) return 0.0;
  u_int8_t d = mhi->mosi.frame.data[3] & ~(1 << 7);
  return (float) (d - 61)/4.0;
}

bool mgos_mhi_ac_get_connected() {
  
  return true;
}

// RPC handlers
static void get_params_handler(struct mg_rpc_request_info *ri, void *cb_arg,
                               struct mg_rpc_frame_info *fi,
                               struct mg_str args) {
  struct mbuf fb;
  struct json_out out = JSON_OUT_MBUF(&fb);

  mbuf_init(&fb, MGOS_MHI_AC_JSON_SIZE);

  bool connected = mgos_mhi_ac_get_connected();
  bool operating = mgos_mhi_ac_get_operating();
  bool isee =  mgos_mhi_ac_get_isee();
  float room = mgos_mhi_ac_get_room_temperature();
  enum mgos_mhi_ac_param_power power = mgos_mhi_ac_get_power();
  enum mgos_mhi_ac_param_mode mode = mgos_mhi_ac_get_mode();
  float setpoint = mgos_mhi_ac_get_setpoint();
  enum mgos_mhi_ac_param_fan fan = mgos_mhi_ac_get_fan();
  enum mgos_mhi_ac_param_vane_vert vane_vert = mgos_mhi_ac_get_vane_vert();
  enum mgos_mhi_ac_param_vane_horiz vane_horiz = mgos_mhi_ac_get_vane_horiz();

  json_printf(
      &out,
      "{connected: %B, power: %d, mode: %d, setpoint: %.1f, fan: %d, "
      "vane_vert: %d, vane_horiz: %d, isee: %B, operating: %B, room: %.1f}",
      connected, power, mode, setpoint, fan,
      vane_vert, vane_horiz,  isee, operating, room);

  mg_rpc_send_responsef(ri, "%.*s", fb.len, fb.buf);
  ri = NULL;

  mbuf_free(&fb);

  (void) cb_arg;
  (void) fi;
}

static void set_params_handler(struct mg_rpc_request_info *ri, void *cb_arg,
                               struct mg_rpc_frame_info *fi,
                               struct mg_str args) {
  struct mbuf fb;
  struct json_out out = JSON_OUT_MBUF(&fb);

  mbuf_init(&fb, MGOS_MHI_AC_JSON_SIZE);

  bool success = true;  // true - if no errors
  enum mgos_mhi_ac_param_power power;
  enum mgos_mhi_ac_param_mode mode;
  float setpoint;
  enum mgos_mhi_ac_param_fan fan;
  enum mgos_mhi_ac_param_vane_vert vane_vert;
  enum mgos_mhi_ac_param_vane_horiz vane_horiz;

  if (json_scanf(args.p, args.len, "{power: %d}", &power) == 1) {
    if (!mgos_mhi_ac_set_power(power)) success = false;
  }
  if (json_scanf(args.p, args.len, "{mode: %d}", &mode) == 1) {
    if (!mgos_mhi_ac_set_mode(mode)) success = false;
  }
  if (json_scanf(args.p, args.len, "{setpoint: %f}", &setpoint) == 1) {
    if (!mgos_mhi_ac_set_setpoint(setpoint)) success = false;
  }
  if (json_scanf(args.p, args.len, "{fan: %d}", &fan) == 1) {
    if (!mgos_mhi_ac_set_fan(fan)) success = false;
  }
  if (json_scanf(args.p, args.len, "{vane_vert: %d}", &vane_vert) == 1) {
    if (!mgos_mhi_ac_set_vane_vert(vane_vert)) success = false;
  }
  if (json_scanf(args.p, args.len, "{vane_horiz: %d}", &vane_horiz) ==
      1) {
    if (!mgos_mhi_ac_set_vane_horiz(vane_horiz)) success = false;
  }
  // Not implemented
  // if (json_scanf(args.p, args.len, "{isee: %d}", &params.isee) == 1) {
  // 	mgos_mel_ac_set_isee(mel, params.isee);
  // }
  // Sending back changes made
  json_printf(&out,
              "{success: %B, power: %d, mode: %d, setpoint: %.1f, fan: %d, "
              "vane_vert: %d, vane_horiz: %d}",
              success, power, mode, setpoint, fan,
              vane_vert, vane_horiz);
  mg_rpc_send_responsef(ri, "%.*s", fb.len, fb.buf);
  ri = NULL;

  mbuf_free(&fb);

  (void) cb_arg;
  (void) fi;
}

bool mgos_mhi_ac_init(void) {
  if (mgos_sys_config_get_mhi_ac_enable()) {
    mgos_mhi_ac_create();

    if (mgos_sys_config_get_mhi_ac_rpc_enable()) {
      struct mg_rpc *c = mgos_rpc_get_global();
      mg_rpc_add_handler(c, "MHI-AC.GetParams", "{}", get_params_handler, NULL);
      mg_rpc_add_handler(c, "MHI-AC.SetParams",
                         "{power: %d, mode: %d, setpoint: %.1f, fan: %d, "
                         "vane_vert: %d, vane_horiz: %d}",
                         set_params_handler, NULL);
    }
  }
  return true;
}

void mgos_mhi_ac_deinit(void) {
  mgos_mhi_ac_destroy();
}
