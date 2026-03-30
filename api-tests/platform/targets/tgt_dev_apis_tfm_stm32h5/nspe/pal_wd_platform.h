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
#ifndef _PAL_WD_PLATFORM_H_
#define _PAL_WD_PLATFORM_H_

#pragma once

#include <stdint.h>
#include "pal_common.h"

#define PAL_WD_COUNTER_MIN           (0x40u)
#define PAL_WD_COUNTER_MAX           (0x7Fu)
#define PAL_WD_CAN_BE_DISABLED       0u

#define __IO volatile
typedef struct {
    __IO uint32_t CR;   /* Control register */
    __IO uint32_t CFR;  /* Configuration register */
    __IO uint32_t SR;   /* Status register */
} WWDG_TypeDef;

/* Addresses & masks */
#define WWDG_BASE_ADDR       ((addr_t)0x40002C00u) /* WWDG register Base address (from Reference Manual) */
#define WWDG                 ((WWDG_TypeDef *)WWDG_BASE_ADDR)

/* Bits, prescaler, etc... */
#define WWDG_CR_T_Pos        (0U)
#define WWDG_CR_T_Msk        (0x7FUL << WWDG_CR_T_Pos)
#define WWDG_CR_T            WWDG_CR_T_Msk
#define WWDG_CR_WDGA_Pos     (7U)
#define WWDG_CR_WDGA_Msk     (0x1UL << WWDG_CR_WDGA_Pos)
#define WWDG_CR_WDGA         WWDG_CR_WDGA_Msk

#define WWDG_CFR_W_Pos       (0U)
#define WWDG_CFR_W_Msk       (0x7FUL << WWDG_CFR_W_Pos)
#define WWDG_CFR_W           WWDG_CFR_W_Msk

#define WWDG_CFR_WDGTB_Pos   (11U)
#define WWDG_CFR_WDGTB_Msk   (0x7UL << WWDG_CFR_WDGTB_Pos)
#define WWDG_CFR_WDGTB       WWDG_CFR_WDGTB_Msk

#define LL_WWDG_PRESCALER_128    ((0x4UL | 0x2UL | 0x1UL) << WWDG_CFR_WDGTB_Pos)

#ifndef MODIFY_REG
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  \
    do {                                     \
        (REG) = (((REG) & ~(CLEARMASK)) | (SETMASK)); \
    } while (0)
#endif

static inline void PAL_WD_ENABLE_CLOCK(void)
{
    /* clock WWDG */
    #define RCC_APB1ENR1_WWDGEN_Pos  (11U) /* WWDG ENABLE = bit 11 */
    #define RCC_APB1ENR1_WWDGEN_Msk  (0x1UL << RCC_APB1ENR1_WWDGEN_Pos)
    #define RCC_APB1ENR1_WWDGEN      RCC_APB1ENR1_WWDGEN_Msk

    volatile uint32_t *reg = (uint32_t *)0x44020C9C; /* = RCC register Base address (0x44020C00) + RCC_APB1ENR1 offest (0x9C) */
    *reg |= RCC_APB1ENR1_WWDGEN;
}

static inline void PAL_WD_CONFIGURE_PRESCALER(addr_t base_addr)
{
    WWDG_TypeDef *wd = (WWDG_TypeDef *)base_addr;
    MODIFY_REG(wd->CFR, WWDG_CFR_WDGTB, LL_WWDG_PRESCALER_128);
}

static inline void PAL_WD_SET_COUNTER(addr_t base_addr, uint32_t value)
{
    WWDG_TypeDef *wd = (WWDG_TypeDef *)base_addr;
    MODIFY_REG(wd->CR, WWDG_CR_T, (value & 0x7Fu));
}

static inline void PAL_WD_CONFIGURE_WINDOW(addr_t base_addr, uint32_t value)
{
    WWDG_TypeDef *wd = (WWDG_TypeDef *)base_addr;
    MODIFY_REG(wd->CFR, WWDG_CFR_W, (value & 0x7Fu));
}

static inline void PAL_WD_EXTRA_HW_CONFIG(addr_t base_addr)
{
    (void)base_addr;
    /* nothing for now */
}

static inline void PAL_WD_START(addr_t base_addr)
{
#if 0
   WWDG_TypeDef *wd = (WWDG_TypeDef *)base_addr;
   wd->CR |= WWDG_CR_WDGA;
#else
    (void)base_addr;
    /* nothing for now */
#endif
}

static inline void PAL_WD_STOP(addr_t base_addr)
{
    (void)base_addr;
    /* not supported */
}

static inline int PAL_WD_IS_ENABLED(addr_t base_addr)
{
    WWDG_TypeDef *wd = (WWDG_TypeDef *)base_addr;
    return ((wd->CR & WWDG_CR_WDGA) != 0u);
}

#endif /* _PAL_WD_PLATFORM_H_ */