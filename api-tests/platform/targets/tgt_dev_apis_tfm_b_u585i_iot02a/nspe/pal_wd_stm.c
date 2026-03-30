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
#include "pal_wd_stm.h"
#include "pal_wd_platform.h"
#include "pal_common.h"

int pal_wd_stm_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    if ((time_us == 0u) || (timer_tick_us == 0u) ||
        (time_us > WDOG_TIMER_MAX_VALUE)) {
        return PAL_STATUS_ERROR;
    }

    uint32_t ticks = time_us / timer_tick_us;
    if (ticks == 0u) {
        ticks = 1u;
    }

    if (ticks < PAL_WD_COUNTER_MIN) {
        ticks = PAL_WD_COUNTER_MIN;
    } else if (ticks > PAL_WD_COUNTER_MAX) {
        ticks = PAL_WD_COUNTER_MAX;
    }

    PAL_WD_ENABLE_CLOCK();
    PAL_WD_CONFIGURE_PRESCALER(base_addr);
    PAL_WD_SET_COUNTER(base_addr, ticks);
    PAL_WD_CONFIGURE_WINDOW(base_addr, ticks);
    PAL_WD_EXTRA_HW_CONFIG(base_addr);

    return PAL_STATUS_SUCCESS;
}

int pal_wd_stm_enable(addr_t base_addr)
{
    PAL_WD_START(base_addr);
    return PAL_STATUS_SUCCESS;
}

int pal_wd_stm_disable(addr_t base_addr)
{
    (void)base_addr;
    return PAL_STATUS_SUCCESS;
}

int pal_wd_stm_is_enabled(addr_t base_addr)
{
    return PAL_WD_IS_ENABLED(base_addr) ? PAL_STATUS_SUCCESS : PAL_STATUS_ERROR;
}