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

#include "mgos_mhi_ac.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MGOS_MHI_AC_FRAME_HEADER_SIZE 3
#define MGOS_MHI_AC_FRAME_DATA_SIZE 15

#define MGOS_MHI_AC_JSON_SIZE 200

struct mgos_mhi_ac_frame {
  uint8_t header[MGOS_MHI_AC_FRAME_HEADER_SIZE];
  uint8_t data[MGOS_MHI_AC_FRAME_DATA_SIZE];
  uint16_t crc;
};

union mgos_mhi_ac_buffer {
  struct mgos_mhi_ac_frame frame;
  uint8_t bytes[sizeof(struct mgos_mhi_ac_frame)];
};

struct mgos_mhi_ac {
  union mgos_mhi_ac_buffer mosi;
  union mgos_mhi_ac_buffer miso;
};

#ifdef __cplusplus
}
#endif
