/*********************************************************************************************************************************************************
 *  Copyright 2026 Broadcom Corporation.  All rights reserved.                                                                                           * 
 *                                                                                                                                                       *
 *  Broadcom Proprietary and Confidential                                                                                                                *
 *********************************************************************************************************************************************************/

/*********************************************************************************************************************************************************
 * This sample program is only a reference to demonstrate the usage of Baremetal APIs. This program might not work for all environments.           
 *********************************************************************************************************************************************************/

 /* ------------------------------------------------------------------------------------------------------------------------------------------------------*
 *                         Reference application                                                                                                          *
 * -------------------------------------------------------------------------------------------------------------------------------------------------------*
 * Description : This test configures and enables the I2C0 device, transfers data to the IO-expander, and verifies whether the transfer is successful.    *
 *                                                                                                                                                        * 
 *   1. Configure the corresponding GPIO pin for I2C0 alternate function.                                                                                 * 
 *   2. Initialize I2C0 with the specified configuration.                                                                                                 *
 *   2. Enable and clear all(If exists) I2C0 interrupts                                                                                                   *
 *   3. Initiate the I2C transfer request using the slave address and the specified packet details.                                                       *
 *   4. Verify that the data transfer is successful.                                                                                                      *
 * -------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <bcm_utils.h>
#include "BCM8915x_CM7.h"
#include "BCM8915X_BareMetal_helper.h"
#include <i2c.h>
#include "ts_i2c.h"

#define IIC0_IRQ               (44U)  /* IIC0 IRQ number */

#define I2C_CLIENT_MASK 0xFF  /* Setting the client mask to 0xFF */
#define I2C_CLIENT_ID   0xFF  /* Setting the client ID to 0xFF */

void I2C0_IrqHandler(); 

uint8_t wr_data[IIC_TEST_MAX_BURST_LEN] = {0x40, 0x39, 0x89, 0x85, 0x52, 0x21}; /* Write data to be sent to the IO-expander */
uint8_t rd_data[IIC_TEST_MAX_BURST_LEN];  /* Buffer to store read data */

/* I2C instance 0 IRQ Handler function */
void I2C0_IrqHandler()
{
  IIC_IRQHandler(IIC_INSTANCE0); /* Sample Baremetal ISR*/
}

/********************************************** Function for I2C transfer ****************************************************/
int32_t BRCM_SEQ_i2c_txfer()
{
  BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
  int counter = 10000;  /* Counter variable is 10000 */

  IIC_ConfigType aConfig;  /*IIC Configuration*/
  IIC_InterruptType intrType = IIC_INTERRUPT_FIFO_EMPTY_INTR |    /* IIC_InterruptType*/
                               IIC_INTERRUPT_NOACK_INTR      |
                               IIC_INTERRUPT_BUSY_INTR       |
                               IIC_INTERRUPT_ERROR_INTR      |
                               IIC_INTERRUPT_CMD_RUN_INTR    ;   

  aConfig.speed = IIC_TEST_SPEED;  /* I2C speed set to 100K*/
  aConfig.txFifoMode = 0;    /* Disable hardware transmit FIFO*/ 
  IIC_PktType aPkts[2];  /* IIC transfer packets */
  uint32_t aJobID;  /*I2C handle-output from baremetal driver*/

  /* Alternate function for I2C0 instance */
  cfg_gpio_alt_fn_as_i2c0();

  /* Register ISR address to NVIC Table and Enable NVIC interrupt */
  /* Used CMSIS HW Abstractation layer API */
  NVIC_SetVector((IRQn_Type)IIC0_IRQ,      (uint32_t)I2C0_IrqHandler);
  NVIC_EnableIRQ(IIC0_IRQ);

  /* Initialization of I2C0 with specified configuration*/
  CHK_RETVAL(retVal = IIC_Init(IIC_INSTANCE0, &aConfig));
  /* Enable all I2C0 interrupts*/
  CHK_RETVAL(retVal = IIC_EnableInterrupt(IIC_INSTANCE0, intrType, 1U));

  /* Clear all I2C0 interrupts*/
  CHK_RETVAL(retVal = IIC_ClearInterrupt(IIC_INSTANCE0, intrType));

  /*Write data on I2C device TPS65224RAHRQ1 I2C address IIC_TEST_SLAVE_ADDR0 on memory location IIC_VALID_MEM_LOCATION */
  aPkts[0].flags = (0UL); /* I2C Write flag*/
  aPkts[0].slaveAddr = 0x26; /* Slave address */
  aPkts[0].length = 1U;   /*data length of 1 byte */
  aPkts[0].data = wr_data; /* 1 Byte data on IO-expander*/
  
  /*This API queues a transfer request to IIC driver. When this request
    processing is complete, client shall get status using #IIC_GetStatus API*/
  CHK_RETVAL(retVal = IIC_Transfer(IIC_INSTANCE0, aPkts, 1U/*Num_of_pkts*/, I2C_CLIENT_ID, I2C_CLIENT_MASK, &aJobID));
  do {
    /*Get transfer status*/
  retVal = IIC_GetStatus(IIC_INSTANCE0, I2C_CLIENT_ID, aJobID);
  BCM_DelayUs(100); 
  counter--;  /* Decreasing the counter value */
  } while ((retVal == BCM_ERR_BUSY) && (counter > 0));  /* Checking retval and counter variable */

  if((retVal != BCM_ERR_OK) && (counter == 0)){   /* Checking whether retVal is ERR_OK; if not, the execution will go to the error handler */
     goto err;
  }
  counter = 100000;   /* Counter variable is 100000 */
  aPkts[0].flags = IIC_PKT_FLG_OP_RD; /* I2C read flag*/
  aPkts[0].slaveAddr = 0x26; /*IIC_TEST_SLAVE_ADDR0;*/
  aPkts[0].length = 1U;  /*data length of 1 byte */
  aPkts[0].data = rd_data; /* 1 Byte data from IO-expander*/

  /*This API queues a transfer request to IIC driver. When this request
    processing is complete, client shall get status using #IIC_GetStatus API*/
  CHK_RETVAL(retVal = IIC_Transfer(IIC_INSTANCE0, aPkts, 1U/*Num_of_pkts*/, I2C_CLIENT_ID, I2C_CLIENT_MASK, &aJobID));
  do {
    /*Get transfer status*/
  retVal = IIC_GetStatus(IIC_INSTANCE0, I2C_CLIENT_ID, aJobID);
  BCM_DelayUs(100);
  counter--;  /* Decreasing the counter value */
  } while ((retVal == BCM_ERR_BUSY) && (counter > 0));   /* Checking retval and counter variable */

  if((retVal != BCM_ERR_OK) && (counter == 0)){    /* Checking whether retVal is ERR_OK; if not, the execution will go to the error handler */
     goto err;
  }
  /* Disable interrupt*/
  CHK_RETVAL(retVal = IIC_EnableInterrupt(IIC_INSTANCE0, intrType, 0U));

err:
  /* DeInitialize I2C0 instance */
  retVal |= IIC_DeInit(IIC_INSTANCE0);
     return retVal;
}

/************************************************* Main function for I2C application *********************************************/
int32_t BCM8915X_Main()
{
  BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
  CHK_RETVAL(retVal = BRCM_SEQ_i2c_txfer());   /* Calling I2C transfer function */
  while(1);
err:
    return retVal;
}

