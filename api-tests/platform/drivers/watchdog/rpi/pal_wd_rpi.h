/** @file
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
**/

#ifndef _PAL_WD_RPI_H_
#define _PAL_WD_RPI_H_

#include "pal_common.h"

#define WDOG_TIMER_MAX_VALUE           0xFFFFFFFF

typedef struct {
    uint32_t  CTRL;          /* Offset: 0x000 (R/W) Watchdog Control Register */
    uint32_t  LOAD;          /* Offset: 0x004 (R/W) Watchdog Load Register */
    uint32_t  REASON;        /* Offset: 0x008 (R/ ) Watchdog Reason Register */
    uint32_t  SCRATCH[8];    /* Offset: 0x00C (R/ ) Watchdog Reason Register */
} wd_timer_t;

#define WATCHDOG_CTRL_TRIGGER_BITS     (0x80000000)
#define WATCHDOG_CTRL_ENABLE_BITS      (0x40000000)
#define WATCHDOG_CTRL_PAUSE_DBG1_BITS  (0x04000000)
#define WATCHDOG_CTRL_PAUSE_DBG0_BITS  (0x02000000)
#define WATCHDOG_CTRL_PAUSE_JTAG_BITS  (0x01000000)

#define WATCHDOG_LOAD_BITS             (0x00ffffff)

int pal_wd_rpi_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);
int pal_wd_rpi_enable(addr_t base_addr);
int pal_wd_rpi_disable(addr_t base_addr);
int pal_wd_rpi_is_enabled(addr_t base_addr);

#endif /* _PAL_WD_RPI_H_ */
