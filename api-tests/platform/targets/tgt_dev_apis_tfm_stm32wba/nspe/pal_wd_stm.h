/** @file
 * Copyright (c) 2026, STMicroelectronics.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/
#ifndef _PAL_WD_STM_H_
#define _PAL_WD_STM_H_

#include "pal_common.h"

#define WDOG_TIMER_MAX_VALUE           0xFFFFFFFFu

/*
 * Initializes the watchdog timer for a requested timeout.
 *
 * Parameters:
 *  - base_addr:      watchdog base address (platform-specific).
 *  - time_us:        requested timeout in microseconds.
 *  - timer_tick_us:  watchdog clock tick period in microseconds.
 *
 * Returns:
 *  - PAL_STATUS_SUCCESS on success
 *  - PAL_STATUS_ERROR   on invalid parameters or configuration failure
 */
int pal_wd_stm_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);

/*
 * Starts/enables the watchdog.
 *
 * Parameters:
 *  - base_addr: watchdog base address (platform-specific).
 *
 * Returns:
 *  - PAL_STATUS_SUCCESS on success
 *  - PAL_STATUS_ERROR   on failure
 */
int pal_wd_stm_enable(addr_t base_addr);

/*
 * Disables the watchdog, if supported by the underlying IP.
 *
 * Parameters:
 *  - base_addr: watchdog base address (platform-specific).
 *
 * Returns:
 *  - PAL_STATUS_SUCCESS if the watchdog was disabled
 *  - PAL_STATUS_ERROR   if disable is not supported or failed
 */
int pal_wd_stm_disable(addr_t base_addr);

/*
 * Checks whether the watchdog is currently enabled.
 *
 * Parameters:
 *  - base_addr: watchdog base address (platform-specific).
 *
 * Returns:
 *  - PAL_STATUS_SUCCESS if the watchdog is enabled
 *  - PAL_STATUS_ERROR   otherwise
 */
int pal_wd_stm_is_enabled(addr_t base_addr);

#endif /* _PAL_WD_STM_H_ */