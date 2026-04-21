/**
    @file DrvBrd.c
    @brief Driver board control source file

    @version
*/

#include <bcm_err.h>
#include <qspi.h>
#include <gpio.h>
#include <i2c.h>
#include "BCM8915X_BareMetal_helper.h"
#include <bcm_utils.h>
#include "BCM8915x_CM7.h"
#include "DrvBrd.h"

#define SPIO_NUM QSPI_HWID_2
/*
    I2C1 devices:
    - 0x48: TEC temperature sensor
    - 0x4A: Laser temperature sensor
    - 0x4B: Out_0: MAN_BFTEC, OUT_1 MAN_PICTEC
 
*/
#define IIC0_IRQ               (44U)
#define IIC1_IRQ               (45U)
#define I2C_CLIENT_MASK 0xFF
#define I2C_CLIENT_ID   0xFF
#define IIC_INSTANCE0                  (0UL)
#define IIC_INSTANCE1                  (1UL)

#define TEC_TEMP_ADD    0x48
#define LASER_TEMP_ADD  0x4A
#define TEC_CTRL_ADD    0x4B

#define IIC_VALID_MEM_LOCATION         (2U)
#define IIC_TEST_MAX_BURST_LEN         (6UL)
#define IIC_TEST_SPEED                 (IIC_SPEED_100K)

uint8_t location[IIC_VALID_MEM_LOCATION] = {0x8, 0x02};
uint8_t read_location[IIC_VALID_MEM_LOCATION] = {0,0};
void I2C0_IrqHandler();
void I2C1_IrqHandler();
uint8_t wr_data[IIC_TEST_MAX_BURST_LEN] = {0x40, 0x39, 0x89, 0x85, 0x52, 0x21};
uint8_t rd_data[IIC_TEST_MAX_BURST_LEN];
uint16_t awg_vector[4096] = {0};
uint32_t expected_res = 0;

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
    retVal = GPIO_DrvInitChannel(GPIO_HW_ID_0, GPIO_CHANNEL_10, &gpioCfgDef);

    // AWG trigger pin
    gpioCfgDef.dout = GPIO_LEVEL_HIGH;
    retVal = GPIO_DrvInitChannel(GPIO_HW_ID_0, GPIO_CHANNEL_9, &gpioCfgDef);

    return retVal;
}

static uint32_t qspi_wr_en(uint32_t qspi_id, uint32_t en) 
{
        BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

  QSPI_ModeOpConfigType qspi_mode;
  QSPI_CommandXferType xfer_type;

  qspi_mode.addrMode = QSPI_ADDR_MODE_NO_BYTE_SENT;
  qspi_mode.modeBits = QSPI_MODE_BITS_NO_BYTE_SENT;
  qspi_mode.dataMode = QSPI_DATA_MODE_NO_BYTE_SENT;
  qspi_mode.opcodeMode=QSPI_OPCODE_MODE_SINGLE;
  qspi_mode.modeBitsLen=0;
  qspi_mode.addrModeLen=0;
  qspi_mode.quadModeEn = 0;

  xfer_type.dataLen = 0;
  xfer_type.modeBits= 0;
  xfer_type.dummyCycles=0;
  xfer_type.xferMode=QSPI_TRANSFER_DMA;

  QSPI_ModeConfigure(qspi_id, &qspi_mode);

  if (en) 
  {
    xfer_type.opcodeVal= 0x06;
  } 
  else 
  {
    xfer_type.opcodeVal = 0x04;
  }
  
  retVal = QSPI_DrvWrite(qspi_id, &xfer_type, 0x0, (char*)wr_data[0], 1);
  return retVal;
}

static uint32_t qspi_rd_sts(uint32_t qspi_id) 
{
  uint32_t rx_data = 0;

  QSPI_ModeOpConfigType qspi_mode;
  QSPI_CommandXferType xfer_type;

  qspi_mode.addrMode = QSPI_ADDR_MODE_NO_BYTE_SENT;
  qspi_mode.modeBits = QSPI_MODE_BITS_NO_BYTE_SENT;
  qspi_mode.dataMode = QSPI_DATA_MODE_SINGLE;
  qspi_mode.opcodeMode=QSPI_OPCODE_MODE_SINGLE;
  qspi_mode.modeBitsLen= 0;
  qspi_mode.addrModeLen= 0;
  qspi_mode.quadModeEn = 0;

  xfer_type.dataLen = 2;
  xfer_type.modeBits= 0;
  xfer_type.dummyCycles=0;
  xfer_type.opcodeVal = 0x05; /*RDSR*/
  xfer_type.xferMode = QSPI_TRANSFER_PIO;

  QSPI_ModeConfigure(qspi_id, &qspi_mode);
  QSPI_DrvRead(qspi_id, &xfer_type, 0x0, (char*)&rx_data, 4);

  return(rx_data);
}

static BCM_ErrorType ConfSpi()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

    retVal = cfg_gpio_alt_fn_as_qspi2();
    ASSERT(retVal == BCM_ERR_OK);

    QSPI_ConfigType qspi_config ={
        .clkDiv = QSPI_CLK_DIV_RATIO_BY_32,
        .clkPolClkPhase = QSPI_CPOL_CPHA_CTRL_00,
        .holdOffLen = 0,
        .rxFifoThr = 15,
        .slaveSel = QSPI_SLAVE_SEL_IND_0,
        .txFifoThr = 15,
        .fdxModeEn = 0,
        .txFIFOWaitEn = 0, 
        .rxFIFOWaitEn = 0,
        .quadModeEn = 0,
        .arbiterIfEn = 0
        };

    retVal = QSPI_DrvInit(SPIO_NUM, &qspi_config);
    ASSERT(retVal == BCM_ERR_OK);

    retVal = QSPI_SetConfigProperty(SPIO_NUM, QSPI_CONFIG_PROP_RXDLY_EN, 1);
    ASSERT(retVal == BCM_ERR_OK);

    qspi_wr_en(SPIO_NUM, 1);

    BCM_DelayUs(200);
  
    uint32_t rdata = qspi_rd_sts(SPIO_NUM);

      if ((rdata & (1<<1)) == 0)
    {
        expected_res = 0;
    }
    else 
    {
        expected_res = 1;
    }

    return retVal;
}

static BCM_ErrorType Confi2c()
{
  BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

  IIC_ConfigType aConfig;
  IIC_InterruptType intrType = IIC_INTERRUPT_FIFO_EMPTY_INTR |
                               IIC_INTERRUPT_NOACK_INTR      |
                               IIC_INTERRUPT_BUSY_INTR       |
                               IIC_INTERRUPT_ERROR_INTR      |
                               IIC_INTERRUPT_CMD_RUN_INTR    ;


  aConfig.speed = IIC_TEST_SPEED;
  aConfig.txFifoMode = 0;
  cfg_gpio_alt_fn_as_i2c0();
  /* Register ISR address to NVIC Table and Enable NVIC interrupt */
  /* Used CMSIS HW Abstractation layer API */
  NVIC_SetVector((IRQn_Type)IIC0_IRQ,      (uint32_t)I2C0_IrqHandler);
  NVIC_EnableIRQ(IIC0_IRQ);
  /* Initialization */
  retVal = IIC_Init(IIC_INSTANCE0, &aConfig);
  ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
  /* Enable interrupt*/
  retVal = IIC_EnableInterrupt(IIC_INSTANCE0, intrType, 1U);
  ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

  /* Clear interrupt*/
  retVal = IIC_ClearInterrupt(IIC_INSTANCE0, intrType);
  ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    return retVal;
}

BCM_ErrorType __attribute__((unused)) I2CTest()
{
  BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
  int counter = 10000;

  IIC_PktType aPkts[2];
  uint32_t aJobID;

  aPkts[0].flags = (0UL); /* Write flag*/
  aPkts[0].slaveAddr = 0x26;
  aPkts[0].length = 1U;
  aPkts[0].data = location; /* 6 Byte data on memory location IIC_VALID_MEM_LOCATION onwards*/
  retVal = IIC_Transfer(IIC_INSTANCE0, aPkts, 1U, I2C_CLIENT_ID, I2C_CLIENT_MASK, &aJobID);
  ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
  do {
  retVal = IIC_GetStatus(IIC_INSTANCE0, I2C_CLIENT_ID, aJobID);
  BCM_DelayUs(100);
  counter--;
  } while ((retVal == BCM_ERR_BUSY) && (counter > 0));

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

BCM_ErrorType __attribute__((unused)) InitDrvBrd()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
 
    retVal = ConfigGPIO();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = ConfSpi();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = ConfigAWG();

    retVal = Confi2c();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
    
    // retVal = ConfigTempSensors();
    // ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

   return retVal;
}

BCM_ErrorType ConfigAWG()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
 
    QSPI_CommandXferType cmd = {
        .dataLen = 1,
        .dummyCycles = 0,
        .modeBits = 0,
        .opcodeVal = 0x0,
        .xferMode = QSPI_TRANSFER_DATA_ONLY
    };

    retVal = QSPI_DrvWrite(SPIO_NUM, &cmd, 0x0, (char *)awg_vector, 20);
    return retVal;
}

BCM_ErrorType SetParamValue(uint8_t paramID, uint32_t value)
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
 
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

void I2C1_IrqHandler()
{
  IIC_IRQHandler(IIC_INSTANCE1);
}

void I2C0_IrqHandler()
{
  IIC_IRQHandler(IIC_INSTANCE0);
}


