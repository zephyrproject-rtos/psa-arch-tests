/** @file
 * Copyright (c) 2024, Arm Limited or its affiliates. All rights reserved.
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

#include "pal_wd_rpi.h"


#define REG_ALIAS_SET_BITS ((0x2u) << (12u))
#define REG_ALIAS_CLR_BITS ((0x3u) << (12u))

#define SET_ADDR_ALIAS(addr) ((void *)(REG_ALIAS_SET_BITS + (addr)))
#define CLR_ADDR_ALIAS(addr) ((void *)(REG_ALIAS_CLR_BITS + (addr)))

#define HW_SET_BITS(addr, mask) \
    do{*(uint32_t *) SET_ADDR_ALIAS((volatile void *) addr) = mask;}while(0)

#define HW_CLEAR_BITS(addr, mask) \
    do{*(uint32_t *) CLR_ADDR_ALIAS((volatile void *) addr) = mask;}while(0)

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_rpi_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    /* Disable Timer */
    HW_CLEAR_BITS(&((wd_timer_t *)base_addr)->CTRL, WATCHDOG_CTRL_ENABLE_BITS);

    uint32_t dbg_bits = WATCHDOG_CTRL_PAUSE_DBG0_BITS |
                        WATCHDOG_CTRL_PAUSE_DBG1_BITS |
                        WATCHDOG_CTRL_PAUSE_JTAG_BITS;
    /* Pause on debug */
    HW_SET_BITS(&((wd_timer_t *)base_addr)->CTRL, dbg_bits);

    if (time_us == 0)
    {
        HW_SET_BITS(&((wd_timer_t *)base_addr)->CTRL, WATCHDOG_CTRL_TRIGGER_BITS);
    }
    else
    {
        uint32_t load_value = time_us * timer_tick_us;
        if (load_value > WATCHDOG_LOAD_BITS)
        {
            load_value = WATCHDOG_LOAD_BITS;
        }
        /* Set Load value */
        ((wd_timer_t *)base_addr)->LOAD = load_value;
    }

    return 0;
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_rpi_enable(addr_t base_addr)
{
    /* Enable counter */
    HW_SET_BITS(&((wd_timer_t *)base_addr)->CTRL, WATCHDOG_CTRL_ENABLE_BITS);

    return 0;
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_rpi_disable(addr_t base_addr)
{
    /* Disable Timer */
    HW_CLEAR_BITS(&((wd_timer_t *)base_addr)->CTRL, WATCHDOG_CTRL_ENABLE_BITS);

    return 0;
}

/**
    @brief           - Checks whether hardware watchdog timer is enabled
    @param           - base_addr       : Base address of the watchdog module
    @return          - Enabled : 1, Disabled : 0
**/
int pal_wd_rpi_is_enabled(addr_t base_addr)
{
    return (((wd_timer_t *)base_addr)->CTRL & WATCHDOG_CTRL_ENABLE_BITS ? 1 : 0);
}
