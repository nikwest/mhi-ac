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

#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MGOS_MHI_AC_EV_BASE MGOS_EVENT_BASE('M', 'H', 'I')
#define MGOS_EVENT_GRP_MHI_AC MGOS_MHI_AC_EV_BASE

enum mgos_mhi_ac_event {
  MGOS_MHI_AC_EV_INITIALIZED = MGOS_MHI_AC_EV_BASE,
};

enum mgos_mhi_ac_param_power {
  MGOS_MHI_AC_PARAM_POWER_OFF = 0,
  MGOS_MHI_AC_PARAM_POWER_ON = 1
};

enum mgos_mhi_ac_param_mode {
  MGOS_MHI_AC_PARAM_MODE_HEAT = 1,
  MGOS_MHI_AC_PARAM_MODE_DRY = 2,
  MGOS_MHI_AC_PARAM_MODE_COOL = 3,
  MGOS_MHI_AC_PARAM_MODE_FAN = 7,
  MGOS_MHI_AC_PARAM_MODE_AUTO = 8
};

enum mgos_mhi_ac_param_fan {
  //MGOS_MHI_AC_PARAM_FAN_AUTO = 0,
 // MGOS_MHI_AC_PARAM_FAN_QUIET = 1,
  MGOS_MHI_AC_PARAM_FAN_LOW = 2,
  MGOS_MHI_AC_PARAM_FAN_MED = 3,
  // MGOS_MHI_AC_PARAM_FAN_MEDHI = 4,
  MGOS_MHI_AC_PARAM_FAN_HIGH = 5,
  MGOS_MHI_AC_PARAM_FAN_TURBO = 6
};

enum mgos_mhi_ac_param_vane_horiz {
//  MGOS_MHI_AC_PARAM_VANE_HORIZ_AUTO = 0,
  MGOS_MHI_AC_PARAM_VANE_HORIZ_1 = 1,  // 15 %
  MGOS_MHI_AC_PARAM_VANE_HORIZ_2 = 2,  // 30 %
  MGOS_MHI_AC_PARAM_VANE_HORIZ_3 = 3,  // 45 %
  MGOS_MHI_AC_PARAM_VANE_HORIZ_4 = 4,  // 60 %
//  MGOS_MHI_AC_PARAM_VANE_HORIZ_5 = 5,  // 75 %
  MGOS_MHI_AC_PARAM_VANE_HORIZ_SWING = 7
};

enum mgos_mhi_ac_param_vane_vert {
  MGOS_MHI_AC_PARAM_VANE_VERT_AUTO = 0,
  MGOS_MHI_AC_PARAM_VANE_VERT_LEFTEST = 1,
  MGOS_MHI_AC_PARAM_VANE_VERT_LEFT = 2,
  MGOS_MHI_AC_PARAM_VANE_VERT_CENTER = 3,
  MGOS_MHI_AC_PARAM_VANE_VERT_RIGHT = 4,
  MGOS_MHI_AC_PARAM_VANE_VERT_RIGHTEST = 5,
  MGOS_MHI_AC_PARAM_VANE_VERT_LEFTRIGHT = 8,
  MGOS_MHI_AC_PARAM_VANE_VERT_SWING = 12
};

void mgos_mhi_ac_create();
void mgos_mhi_ac_destroy();

// Setters
bool mgos_mhi_ac_set_power(enum mgos_mhi_ac_param_power power);
bool mgos_mhi_ac_set_mode(enum mgos_mhi_ac_param_mode mode);
bool mgos_mhi_ac_set_setpoint(float setpoint);
bool mgos_mhi_ac_set_ext_temp(float temp);
bool mgos_mhi_ac_set_fan(enum mgos_mhi_ac_param_fan fan);
bool mgos_mhi_ac_set_vane_vert(enum mgos_mhi_ac_param_vane_vert vane_vert);
bool mgos_mhi_ac_set_vane_horiz(enum mgos_mhi_ac_param_vane_horiz vane_horiz);
// Getters
enum mgos_mhi_ac_param_power mgos_mhi_ac_get_power();
enum mgos_mhi_ac_param_mode mgos_mhi_ac_get_mode();
float mgos_mhi_ac_get_setpoint();
enum mgos_mhi_ac_param_fan mgos_mhi_ac_get_fan();
enum mgos_mhi_ac_param_vane_vert mgos_mhi_ac_get_vane_vert();
enum mgos_mhi_ac_param_vane_horiz mgos_mhi_ac_get_vane_horiz();
bool mgos_mhi_ac_get_isee();
bool mgos_mhi_ac_get_operating();
float mgos_mhi_ac_get_room_temperature();
bool mgos_mhi_ac_get_connected();

// library
bool mgos_mhi_ac_init(void);
void mgos_mhi_ac_deinit(void);

#ifdef __cplusplus
}
#endif
