/**
    @file DrvBrd.c
    @brief Driver board control source file

    @version
*/

#include <math.h>
#include <bcm_err.h>
#include <qspi.h>
#include <gpio.h>
#include <i2c.h>
#include "BCM8915X_BareMetal_helper.h"
#include <bcm_utils.h>
#include "BCM8915x_CM7.h"
#include "DrvBrd.h"
#include "evk_control.h"
#include "regs.h"
#include "globals.h"

#define SPIO_NUM QSPI_HWID_2
/*
    I2C1 devices:
    - 0x48: TEC temperature sensor
    - 0x4A: Laser temperature sensor
    - 0x4B: Out_0: MAN_BFTEC, OUT_1 MAN_PICTEC
 
*/
#define IIC1_IRQ               (45U)
#define I2C_CLIENT_MASK 0xFF
#define I2C_CLIENT_ID   0xFF
#define IIC_INSTANCE1                  (1UL)

#define TEC_TEMP_ADD    0x48
#define LASER_TEMP_ADD  0x4A
#define TEC_CTRL_ADD    0x4B

#define IIC_VALID_MEM_LOCATION         (2U)
#define IIC_TEST_MAX_BURST_LEN         (6UL)
#define IIC_TEST_SPEED                 (IIC_SPEED_100K)

#define QSPI_HW_ID_2 (2UL) /* QSPI Instance 2 */

uint16_t awg_vector[4096] = {0};
uint16_t awgLen = 0;
int16_t fir_vector[128] = {0};
uint16_t firLen = 0;
uint16_t win_vector[1024] = {0};
uint16_t winLen = 0;

/* QSPI configuration structure*/
QSPI_ConfigType qspi_config =
{
  .clkDiv = QSPI_CLK_DIV_RATIO_BY_4, /* clock dev ratio set by 4. QSPI peripheral operates @50MHz clock */
  .clkPolClkPhase = QSPI_CPOL_CPHA_CTRL_00, /* Mode 0 CPOL=0, CPHA=0 SCK defaults to 0 data is sampled on rising edge */
  .holdOffLen = 0,  /*Set the number of holdoff SCK cycles*/
  .rxFifoThr = 15,  /*QSPI Rx Fifo threshold, RX interrupt and DMA request are asserted
                                            when the threshold is reached */
  .slaveSel = QSPI_SLAVE_SEL_IND_0, /* Select individual Slave 0 */
  .txFifoThr = 15,  /*QSPI Tx Fifo threshold, TX interrupt and DMA request are asserted
                                            when the threshold is reached */
  .fdxModeEn = 0,  /*Full-Duplex mode. Only available in single Lane mode.*/
  .txFIFOWaitEn = 0, /* Disable stopping SCK when TX FIFO becomes empty.*/
  .rxFIFOWaitEn = 0, /* Disable stopping SCK when RX FIFO becomes full*/
  .quadModeEn = 0,   /* Quad-SPI mode disabled */
  .arbiterIfEn = 0  /*Arbiter interface disabled*/
};

/* QSPI mode configuration structure*/
QSPI_ModeOpConfigType qspi_mode = 
{
  .addrMode = QSPI_ADDR_MODE_SINGLE, /*Address Mode set to Single SPI mode*/
  .dataMode = QSPI_DATA_MODE_SINGLE,  /*Data Mode options set to Single SPI mode*/
  .opcodeMode = QSPI_OPCODE_MODE_SINGLE,  /*Opcode Mode options set to Single SPI mode*/
  .modeBits = QSPI_MODE_BITS_NO_BYTE_SENT,  /*Data Mode Bits options set as  No byte sent is sent during the QSPI mode bits phase*/
  .modeBitsLen = QSPI_MODE_BITS_LEN_BYTE_1,   /*Selecting QSPI mode Bits Length of 1 byte*/
  .quadModeEn = 0,  /* QUAD SPI mode disabled*/
  .addrModeLen = QSPI_ADDR_MODE_LEN_BYTE_3  /*Selecting QSPI Addr mode Length of 3 byte*/
};

void I2C1_IrqHandler()
{
  IIC_IRQHandler(IIC_INSTANCE1);
}

static BCM_ErrorType ConfigGPIO()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

    GPIO_ConfigType gpioCfgDef =
    {
        .mode       = GPIO_CFG_MODE_OUTPUT,
        .oType      = GPIO_CFG_OUTPUT_PUSH_PULL,
        .pupd       = GPIO_CFG_PUPD_PULL_UP,
        .dout       = GPIO_LEVEL_LOW,
        .hys        = GPIO_CFG_HYSTERESIS_DISABLE,
        .strength   = GPIO_CFG_DRIVE_STRENGTH_8MA,
        .ind        = GPIO_CFG_INPUT_ENABLE,
        .slewRate   = GPIO_CFG_SLEW_RATE_SLEWED,
        .doutInvert = GPIO_CFG_DOUT_INVERT_DISABLE,
        .aCfgMask   = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_OTYPE | GPIO_CFG_MASK_PUPD | GPIO_CFG_MASK_DOUT | GPIO_CFG_MASK_HIST | GPIO_CFG_MASK_SEL | GPIO_CFG_MASK_IND | GPIO_CFG_MASK_SRC | GPIO_CFG_MASK_DOUT_INV
    };

    // AWG Reset pin
    retVal = GPIO_DrvInitChannel(GPIO_HW_ID_0, GPIO_CHANNEL_43, &gpioCfgDef);

    // AWG trigger pin
    gpioCfgDef.dout = GPIO_LEVEL_HIGH;
    retVal = GPIO_DrvInitChannel(GPIO_HW_ID_0, GPIO_CHANNEL_44, &gpioCfgDef);

    return retVal;
}

static uint32_t __attribute__((unused)) spi_wr_opcode(uint32_t devAdd, uint32_t *buf, uint32_t len) 
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    len = 0;
  QSPI_ModeOpConfigType qspi_mode_1;
  QSPI_CommandXferType xfer_type;

  /* Sending opcodeVal to flash memory to perform write_flash_en using DMA */
  qspi_mode_1.addrMode = QSPI_ADDR_MODE_NO_BYTE_SENT;  /* No byte sent is sent during the QSPI address phase*/
  qspi_mode_1.modeBits = QSPI_MODE_BITS_NO_BYTE_SENT; /* No byte sent is sent during the QSPI mode bits phase*/
  qspi_mode_1.dataMode = QSPI_DATA_MODE_NO_BYTE_SENT; /*No byte sent is sent during the QSPI data mode phase*/
  qspi_mode_1.opcodeMode=QSPI_OPCODE_MODE_SINGLE;  /*QSPI Single SPI mode*/
  qspi_mode_1.modeBitsLen=QSPI_MODE_BITS_LEN_BYTE_1;   /*Selecting QSPI mode Bits Length of 1 byte*/
  qspi_mode_1.addrModeLen=0;  /*Selecting QSPI Addr mode Length of 1 byte*/
  qspi_mode_1.quadModeEn = 0;  /* QUAD SPI mode disabled*/

  xfer_type.dataLen = len;  /*selecting 0 bytes of data length to be sent or received*/
  xfer_type.modeBits= 0;  /*QSPI mode bits for selected mode of the device.*/
  xfer_type.dummyCycles=0;  /*number of dummy SCK cycles inserted between Modebits and Data payload*/
  xfer_type.xferMode=QSPI_TRANSFER_DMA; /* Select QSPI Transfer mode as DMA */

  /*This API configures mode of operation sent to  QSPI peripheral for instruction opcode and mode bits.
    Mode of operation needs to be configured before any read/write transaction.*/
  retVal = QSPI_ModeConfigure(QSPI_HW_ID_2, &qspi_mode_1);

  {
    xfer_type.opcodeVal = 0x04;  /* Sending the write_en=0 command opcode(04h) to flash memory(W25Q32JW) */
    /* Write opcodeVal*/
    retVal = QSPI_DrvWrite(QSPI_HW_ID_2, &xfer_type, devAdd, (char *)buf, len);
  }

  return(retVal);
}

static uint32_t __attribute__((unused)) spi_wr(uint32_t devAdd, uint32_t *buf, uint32_t len) 
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

//  QSPI_ModeOpConfigType qspi_mode_1;
  QSPI_CommandXferType xfer_type;

  // /* Sending opcodeVal to flash memory to perform write_flash_en using DMA */
  // qspi_mode_1.addrMode = QSPI_ADDR_MODE_NO_BYTE_SENT;  /* No byte sent is sent during the QSPI address phase*/
  // qspi_mode_1.modeBits = QSPI_MODE_BITS_NO_BYTE_SENT; /* No byte sent is sent during the QSPI mode bits phase*/
  // qspi_mode_1.dataMode = QSPI_DATA_MODE_NO_BYTE_SENT; /*No byte sent is sent during the QSPI data mode phase*/
  // qspi_mode_1.opcodeMode=QSPI_OPCODE_MODE_SINGLE;  /*QSPI Single SPI mode*/
  // qspi_mode_1.modeBitsLen=QSPI_MODE_BITS_LEN_BYTE_1;   /*Selecting QSPI mode Bits Length of 1 byte*/
  // qspi_mode_1.addrModeLen=0;  /*Selecting QSPI Addr mode Length of 1 byte*/
  // qspi_mode_1.quadModeEn = 0;  /* QUAD SPI mode disabled*/

  xfer_type.dataLen = len;  /*selecting len bytes of data length to be sent or received*/
  xfer_type.modeBits= 0;  /*QSPI mode bits for selected mode of the device.*/
  xfer_type.dummyCycles=0;  /*number of dummy SCK cycles inserted between Modebits and Data payload*/
  xfer_type.xferMode=QSPI_TRANSFER_PIO; /* Select QSPI Transfer mode as DMA */
//  xfer_type.opcodeVal = 0x04;  /* Sending the write_en=0 command opcode(04h) to flash memory(W25Q32JW) */

  /*This API configures mode of operation sent to  QSPI peripheral for instruction opcode and mode bits.
    Mode of operation needs to be configured before any read/write transaction.*/
//  retVal = QSPI_ModeConfigure(QSPI_HW_ID_2, &qspi_mode_1);

  {
    /* Write opcodeVal*/
    retVal = QSPI_DrvWrite(QSPI_HW_ID_2, &xfer_type, devAdd, (char *)buf, len);
  }

  return(retVal);
}

BCM_ErrorType __attribute__((unused)) spi_rd(uint32_t devAdd, uint32_t *buf, uint32_t len) 
{
  BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
  
  /*This API configures mode of operation sent to  QSPI peripheral for instruction opcode and mode bits.
    Mode of operation needs to be configured before any read/write transaction.*/
  retVal = QSPI_ModeConfigure(QSPI_HW_ID_2, &qspi_mode);

  QSPI_CommandXferType qspi_cmd = {
    .dummyCycles = 0,  /*number of dummy SCK cycles inserted between Modebits and Data payload*/
    .dataLen = 2,   /*selecting 2 bytes of data length to be sent or received*/
    .modeBits = 0,   /*QSPI mode bits for selected mode of the device.*/
    .opcodeVal = 0x90, /* Sending the read dev_Id command opcode(90h) to flash memory(W25Q32JW) */
    .xferMode = QSPI_TRANSFER_PIO  /* Select QSPI Transfer mode as PIO */
  };

  /* Read the flash memory(W25Q32JW) device ID */
  retVal = QSPI_DrvRead(QSPI_HW_ID_2, &qspi_cmd, devAdd, (char *)buf, len);

  return retVal;
}

static BCM_ErrorType ConfSpi()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

    retVal = cfg_gpio_alt_fn_as_qspi2();
    ASSERT(retVal == BCM_ERR_OK);

    /*Initialising QSPI_2 instance*/
    qspi_config.clkDiv = QSPI_CLK_DIV_RATIO_BY_6; /*QSPI_2 was tested with 33MHz*/
    retVal = QSPI_DrvInit(QSPI_HW_ID_2, &qspi_config);
    ASSERT(retVal == BCM_ERR_OK);

    /* Set selected config property for QSPI_2 */
    retVal = QSPI_SetConfigProperty(QSPI_HW_ID_2, QSPI_CONFIG_PROP_RXDLY_EN, 1);
    ASSERT(retVal == BCM_ERR_OK);

  /*This API configures mode of operation sent to  QSPI peripheral for instruction opcode and mode bits.
    Mode of operation needs to be configured before any read/write transaction.*/
    retVal = QSPI_ModeConfigure(QSPI_HW_ID_2, &qspi_mode);
    ASSERT(retVal == BCM_ERR_OK);

    return retVal;
}

static BCM_ErrorType Confi2c()
{
  BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
      /*IIC Configuration*/
    IIC_ConfigType aConfig = {
      .speed = IIC_TEST_SPEED,  /* I2C speed set to 100K*/
      .txFifoMode = 0            /* Disable hardware transmit FIFO*/
    };
  
  /* Alternate function for I2C1 instance */
    cfg_gpio_alt_fn_as_i2c1();

    /* Register ISR address to NVIC Table and Enable NVIC interrupt */
    /* Used CMSIS HW Abstractation layer API */
    NVIC_SetVector((IRQn_Type)IIC1_IRQ, (uint32_t)I2C1_IrqHandler);
    NVIC_EnableIRQ(IIC1_IRQ);

    /* Initialization of I2C1 with specified configuration*/
    retVal = IIC_Init(IIC_INSTANCE1, &aConfig);
    ASSERT(retVal == BCM_ERR_OK);


  return retVal;
}

static BCM_ErrorType __attribute__((unused)) ConfigTempSensors()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t i2cBuf[2] = {0};
    uint32_t aJobID;

    IIC_PktType i2cPkt = {
        .flags = 0,
        .slaveAddr = TEC_TEMP_ADD,
        .length = 2,
        .data = &i2cBuf[0]
    };

    i2cBuf[0] = 0x0C; // Channel 0 power up command for TEC temperature sensor
    retVal = IIC_Transfer(IIC_HWID_0, &i2cPkt, 1U, I2C_CLIENT_ID, I2C_CLIENT_MASK, &aJobID);
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    uint8_t counter = 100;
    do {
        retVal = IIC_GetStatus(IIC_HWID_0, I2C_CLIENT_ID, aJobID);
        BCM_DelayUs(100);
        counter--;
    } while ((retVal == BCM_ERR_BUSY) && (counter > 0));
    
    return retVal;
}

BCM_ErrorType InitDrvBrd()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
 
    retVal = ConfigGPIO();
    ASSERT(retVal == BCM_ERR_OK);

    retVal = Confi2c();
    ASSERT(retVal == BCM_ERR_OK);

    retVal = ConfSpi();
    ASSERT(retVal == BCM_ERR_OK);

    retVal = spi_rd(0x00000055, gp_buffer, 4);
    ASSERT(retVal == BCM_ERR_OK);

    retVal = spi_wr_opcode(0x00000055, gp_buffer, 4);
    ASSERT(retVal == BCM_ERR_OK);

    // retVal = ConfigAWG();

    
    // retVal = ConfigTempSensors();
    // ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

   return retVal;
}

BCM_ErrorType ConfigAWG()
{
    BCM_ErrorType retVal = BCM_ERR_OK;
 
    // QSPI_CommandXferType cmd = {
    //     .dataLen = 1,
    //     .dummyCycles = 0,
    //     .modeBits = 0,
    //     .opcodeVal = 0x0,
    //     .xferMode = QSPI_TRANSFER_DATA_ONLY
    // };

    // retVal = QSPI_DrvWrite(SPIO_NUM, &cmd, 0x0, (char *)awg_vector, 20);
    return retVal;
}

void ConfigFIR()
{
    uint32_t start_addr, addr, tmp;
    for (uint32_t ch = 0; ch < 4; ch++)
    {
	    start_addr = HSADC_ACQ_0_FIR_COEFF0 + (ch * 4096);
      for (uint32_t i = 0; i < firLen; i++)
      {
        addr = start_addr - i*4;
       	if(fir_vector[i]<0)
		      tmp = (fir_vector[i]+1024) & 0x000003FF;
	    else
		      tmp = fir_vector[i] & 0x000003FF;
        reg_rmw(addr, 9, 0, tmp);
      }
      reg_rmw(HSADC_ACQ_0_FIR_CONTROL, 0, 0, 1);
    }
}

void ConfigWin()
{
    uint32_t tmp = (uint32_t)log2(winLen)-6;
    uint32_t addr;
    for (uint32_t ch = 0; ch < 4; ch++)
    {
      addr = 0;
      for (size_t i = 0; i < winLen/4; i+=8)
      {
        reg_wr(HSADC_ACQ_0_WND_DATA0+4096*ch,win_vector[i+0]);
        reg_wr(HSADC_ACQ_0_WND_DATA1+4096*ch,win_vector[i+1]);
        reg_wr(HSADC_ACQ_0_WND_DATA2+4096*ch,win_vector[i+2]);
        reg_wr(HSADC_ACQ_0_WND_DATA3+4096*ch,win_vector[i+3]);
        reg_wr(HSADC_ACQ_0_WND_DATA4+4096*ch,win_vector[i+4]);
        reg_wr(HSADC_ACQ_0_WND_DATA5+4096*ch,win_vector[i+5]);
        reg_wr(HSADC_ACQ_0_WND_DATA6+4096*ch,win_vector[i+6]);
        reg_wr(HSADC_ACQ_0_WND_DATA7+4096*ch,win_vector[i+7]);

        reg_rmw(HSADC_ACQ_0_WND_CONTROL+4096*ch,15,8,addr);
        reg_rmw(HSADC_ACQ_0_WND_CONTROL+4096*ch,2,2,1);
        addr=addr+1;
      }
      
      reg_rmw(HSADC_ACQ_0_ACQ_CONTROL0+4096*ch,10,8,tmp);
      reg_rmw(HSADC_ACQ_0_WND_CONTROL+4096*ch,0,0,1);
  }    
}

BCM_ErrorType SetParamValue(uint8_t paramID, uint32_t value)
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
 
    uint8_t tempBuf[10] = {0x40, 0x39, 0x89, 0x85, 0x52, 0x21, 0,0, 0, 0}; // dummy data to write to TEC temp sensor
    BRCM_i2c_write(0x26, tempBuf, 10);   /* Calling I2C transfer function */

    switch (paramID)
    {
    case Chirp_AWG_gain:
        /* code */
        break;
    
    default:
        break;
    }
    return retVal;
}

BCM_ErrorType AwgControl(uint8_t run)
{
    BCM_ErrorType retVal = BCM_ERR_OK;

    // retVal = AWG_DrvChannelRunControl(0, 0, run);
    return retVal;

}

BCM_ErrorType BRCM_i2c_write(uint32_t slaveAddr, uint8_t *data, uint32_t len)
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t counter = 10000;  /* Counter variable is 10000 */

    /* IIC_InterruptType*/
    IIC_InterruptType intrType = IIC_INTERRUPT_FIFO_EMPTY_INTR |    
                               IIC_INTERRUPT_NOACK_INTR      |
                               IIC_INTERRUPT_BUSY_INTR       |
                               IIC_INTERRUPT_ERROR_INTR      |
                               IIC_INTERRUPT_CMD_RUN_INTR    ;   

    /* Enable all I2C1 interrupts*/
    CHK_RETVAL(retVal = IIC_EnableInterrupt(IIC_INSTANCE1, intrType, 1U));

    /* Clear all I2C1 interrupts*/
    CHK_RETVAL(retVal = IIC_ClearInterrupt(IIC_INSTANCE1, intrType));

    /*Write data on I2C device TPS65224RAHRQ1 I2C address IIC_TEST_SLAVE_ADDR0 on memory location IIC_VALID_MEM_LOCATION */
    IIC_PktType aPkts;  /* IIC transfer packets */
    aPkts.flags = (0UL); /* I2C Write flag*/
    aPkts.slaveAddr = slaveAddr; /* Slave address */
    aPkts.length = len;   /*data length of 1 byte */
    aPkts.data = data; /* 1 Byte data on IO-expander*/
  
    /*This API queues a transfer request to IIC driver. When this request
      processing is complete, client shall get status using #IIC_GetStatus API*/
    uint32_t aJobID;  /*I2C handle-output from baremetal driver*/
    CHK_RETVAL(retVal = IIC_Transfer(IIC_INSTANCE1, &aPkts, 1U/*Num_of_pkts*/, I2C_CLIENT_ID, I2C_CLIENT_MASK, &aJobID));
    do 
    {
        /*Get transfer status*/
      retVal = IIC_GetStatus(IIC_INSTANCE1, I2C_CLIENT_ID, aJobID);
      BCM_DelayUs(100); 
      counter--;  /* Decreasing the counter value */
    } 
    while ((retVal == BCM_ERR_BUSY) && (counter > 0));  /* Checking retval and counter variable */

    /* Checking whether retVal is ERR_OK; if not, the execution will go to the error handler */
    if((retVal != BCM_ERR_OK) && (counter == 0))
    {   
      goto err;
    }

err:
    /* Disable interrupt*/
    CHK_RETVAL(retVal = IIC_EnableInterrupt(IIC_INSTANCE1, intrType, 0U));
    return retVal;
}

BCM_ErrorType __attribute__((unused)) BRCM_i2c_read(uint32_t slaveAddr, uint8_t *data, uint32_t len)
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t counter = 10000;  /* Counter variable is 10000 */

    /* IIC_InterruptType*/
    IIC_InterruptType intrType = IIC_INTERRUPT_FIFO_EMPTY_INTR |    
                               IIC_INTERRUPT_NOACK_INTR      |
                               IIC_INTERRUPT_BUSY_INTR       |
                               IIC_INTERRUPT_ERROR_INTR      |
                               IIC_INTERRUPT_CMD_RUN_INTR    ;   

    /* Enable all I2C1 interrupts*/
    CHK_RETVAL(retVal = IIC_EnableInterrupt(IIC_INSTANCE1, intrType, 1U));

    /* Clear all I2C1 interrupts*/
    CHK_RETVAL(retVal = IIC_ClearInterrupt(IIC_INSTANCE1, intrType));

    /*Write data on I2C device TPS65224RAHRQ1 I2C address IIC_TEST_SLAVE_ADDR0 on memory location IIC_VALID_MEM_LOCATION */
    IIC_PktType aPkts;  /* IIC transfer packets */
    aPkts.flags = IIC_PKT_FLG_OP_RD; /* I2C read flag*/
    aPkts.slaveAddr = slaveAddr; /*IIC_TEST_SLAVE_ADDR0;*/
    aPkts.length = len;  /*data length of 1 byte */
    aPkts.data = data; /* 1 Byte data from IO-expander*/
  
    /*This API queues a transfer request to IIC driver. When this request
      processing is complete, client shall get status using #IIC_GetStatus API*/
    uint32_t aJobID;  /*I2C handle-output from baremetal driver*/
    counter = 100000;   /* Counter variable is 100000 */

   /*This API queues a transfer request to IIC driver. When this request
      processing is complete, client shall get status using #IIC_GetStatus API*/
    CHK_RETVAL(retVal = IIC_Transfer(IIC_INSTANCE1, &aPkts, 1U/*Num_of_pkts*/, I2C_CLIENT_ID, I2C_CLIENT_MASK, &aJobID));
    do 
    {
      /*Get transfer status*/
      retVal = IIC_GetStatus(IIC_INSTANCE1, I2C_CLIENT_ID, aJobID);
      BCM_DelayUs(100);
      counter--;  /* Decreasing the counter value */
    } 
    while ((retVal == BCM_ERR_BUSY) && (counter > 0));   /* Checking retval and counter variable */

    /* Checking whether retVal is ERR_OK; if not, the execution will go to the error handler */
    if((retVal != BCM_ERR_OK) && (counter == 0))
    {    
      goto err;
    }
    

err:
    /* Disable interrupt*/
      retVal = IIC_EnableInterrupt(IIC_INSTANCE1, intrType, 0U);
      if (counter == 0)
      {
          return BCM_ERR_TIME_OUT;
      }
      return retVal;
}



