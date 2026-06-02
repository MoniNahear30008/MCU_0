/*****************************************************************************
 *  Copyright 2024 Broadcom Corporation.  All rights reserved.
 *
 *  Broadcom Proprietary and Confidential
 ****************************************************************************/

#ifndef TS_I2C_H
#define TS_I2C_H

#define IIC_INSTANCE0                  (0UL)
#define IIC_INSTANCE1                  (1UL)
#define IIC_VALID_MEM_LOCATION         (2U)
#define IIC_TEST_SLAVE_ADDR0           (0x26U)
#define IIC_TEST_SLAVE_ADDR1           (0x44U)
#define IIC_TEST_SPEED                 (IIC_SPEED_100K)
#define IIC_TEST_MAX_BURST_LEN         (6UL)


#define IIC_TEST_SEQ_1  (1U)

extern int32_t BRCM_SEQ_i2c0_txfer();

#endif /* TS_I2C_H */
