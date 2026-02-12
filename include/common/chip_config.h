/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
#ifndef CHIP_CONFIG_H
#define CHIP_CONFIG_H

#define CPU_CLOCK                 (600000000UL)  /**< 600 MHz */
#define BCM_NS_PER_CPU_CYCLE_QUOT                       (1000000000ULL / CPU_CLOCK)
#define BCM_NS_PER_CPU_CYCLE_REM                        (1000000000ULL % CPU_CLOCK)

#endif /* BCM8910X_CONFIG_H */
