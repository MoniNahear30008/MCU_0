/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_qspidrv_drv_impl QSPI Driver Design
    @ingroup grp_qspidrv

    @addtogroup grp_qspidrv_drv_impl
    @{

    @file drivers/qspi/qspi_drv.c
    @brief QSPI Driver Design

    @version 0.1 Initial version
*/
#include "rdb/qspi_rdb.h"
#include "rdb/bspi_rdb.h"
#include <cfg_rdb.h>
#include <qspi.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <bcm_utils.h>

/**
    @name QSPI Driver Design IDs
    @{
    @brief Driver Design IDs for QSPI

*/
#define BRCM_SWDSGN_QSPI_DRV_INIT_PROC                    (0xC710U) /**< @brief #QSPI_DrvInit */
#define BRCM_SWDSGN_QSPI_DRV_DEINIT_PROC                  (0xC711U) /**< @brief #QSPI_DrvDeInit */
#define BRCM_SWDSGN_QSPI_MODE_CONFIGURE_PROC              (0xC712U) /**< @brief #QSPI_ModeConfigure */
#define BRCM_SWDSGN_QSPI_SET_CONFIG_PROPERTY_PROC         (0xC713U) /**< @brief #QSPI_SetConfigProperty */
#define BRCM_SWDSGN_QSPI_GET_CONFIG_PROPERTY_PROC         (0xC714U) /**< @brief #QSPI_GetConfigProperty */
#define BRCM_SWDSGN_QSPI_DRV_XFER_PROC                    (0xC715U) /**< @brief #QSPI_DrvXfer */
#define BRCM_SWDSGN_QSPI_DRV_WRITE_PROC                   (0xC716U) /**< @brief #QSPI_DrvWrite */
#define BRCM_SWDSGN_QSPI_DRV_READ_PROC                    (0xC717U) /**< @brief #QSPI_DrvRead */
#define BRCM_SWDSGN_QSPI_DRV_DMA_REQ_WRITE_ENABLE_PROC    (0xC718U) /**< @brief #QSPI_DrvDMAReqWriteEnable */
#define BRCM_SWDSGN_QSPI_DRV_DMA_REQ_READ_ENABLE_PROC     (0xC719U) /**< @brief #QSPI_DrvDMAReqReadEnable */
#define BRCM_SWDSGN_QSPI_TX_FIFO_FLUSH_PROC               (0xC71AU) /**< @brief #QSPI_TxFifoFlush */
#define BRCM_SWDSGN_QSPI_RX_FIFO_FLUSH_PROC               (0xC71BU) /**< @brief #QSPI_RxFifoFlush */
#define BRCM_SWDSGN_QSPI_DRV_STATUS_GET_PROC              (0xC71CU) /**< @brief #QSPI_DrvStatusGet */
#define BRCM_SWDSGN_QSPI_IRQ_HANDLER_PROC                 (0xC71DU) /**< @brief #QSPI_IRQHandler     */
#define BRCM_SWDSGN_QSPI_ENABLE_INTERRUPT_PROC            (0xC71EU) /**< @brief #QSPI_EnableInterrupt */
#define BRCM_SWDSGN_QSPI_CLEAR_INTERRUPT_PROC             (0xC71FU) /**< @brief #QSPI_ClearInterrupt  */
#define BRCM_SWDSGN_QSPI_GET_INTERRUPT_STATUS_PROC        (0xC720U) /**< @brief #QSPI_GetInterruptStatus  */
#define BRCM_SWDSGN_QSPI_INTWAIT_UNTILDONE_PROC           (0xC721U) /**< @brief #QSPI_IntWaitUntilDone */
#define BRCM_SWDSGN_QSPI_INTGET_DEV_BUSYSTATUS_PROC       (0xC722U) /**< @brief #QSPI_IntGetDevBusyStatus */
#define BRCM_SWDSGN_QSPI_INTSTART_SPI_PROC                (0xC723U) /**< @brief #QSPI_IntStartSpi  */

#define BRCM_SWDSGN_QSPI_REGS_GLOBAL                      (0xC724U) /**< @brief #QSPI_Regs  */
#define BRCM_SWDSGN_QSPI_WORD_LEN_MACRO                   (0xC725U) /**< @brief #QSPI_WORD_LEN */
#define BRCM_SWDSGN_QSPI_RWDEV_TYPE                       (0xC726U) /**< @brief #QSPI_RWDevType */
#define BRCM_SWDSGN_QSPI_RWDEV_GLOBAL                     (0xC727U) /**< @brief #QSPI_RWDev  */
#define BRCM_SWDSGN_QSPI_GET_ALL_INTERRUPT_STATUS_PROC    (0xC728U) /**< @brief #QSPI_GetAllInterruptStatus */
#define BRCM_SWDSGN_QSPI_GET_TX_INTERRUPT_STATUS_PROC     (0xC729U) /**< @brief #QSPI_GetTxInterruptStatus */
#define BRCM_SWDSGN_QSPI_GET_RX_INTERRUPT_STATUS_PROC     (0xC72AU) /**< @brief #QSPI_GetRxInterruptStatus */
#define BRCM_SWDSGN_QSPI_GET_SAFETY_INTERRUPT_STATUS_PROC (0xC72BU) /**< @brief #QSPI_GetSafetyInterruptStatus */
#define BRCM_SWDSGN_QSPI_INT_POWERUP_OUTRESET_PROC        (0xC72CU) /**< @brief #QSPI_IntPowerUpOutReset */
#define BRCM_SWDSGN_QSPI_INT_INRESET_PROC                 (0xC72DU) /**< @brief #QSPI_IntInReset */
#define BRCM_SWDSGN_QSPI_CRC_CONTROL_CONFIG_SET_PROC      (0xC72EU) /**< @brief #QSPI_CrcControlConfigSet  */
#define BRCM_SWDSGN_QSPI_CRC_CONTROL_CONFIG_GET_PROC      (0xC72FU) /**< @brief #QSPI_CrcControlConfigGet  */
#define BRCM_SWDSGN_QSPI_CRC_ENABLE_PROC                  (0xC730U) /**< @brief #QSPI_CrcEnable  */
#define BRCM_SWDSGN_QSPI_CRC_TX_DONE_STATUS_PROC          (0xC731U) /**< @brief #QSPI_CrcTxDoneStatus  */
#define BRCM_SWDSGN_QSPI_CRC_RX_DONE_STATUS_PROC          (0xC732U) /**< @brief #QSPI_CrcRxDoneStatus  */
#define BRCM_SWDSGN_QSPI_GET_CRC_TX_DATA_PROC             (0xC733U) /**< @brief #QSPI_GetCrcTxData  */
#define BRCM_SWDSGN_QSPI_GET_CRC_RX_DATA_PROC             (0xC734U) /**< @brief #QSPI_GetCrcRxData  */
#define BRCM_SWDSGN_QSPI_WRITE_TX_CRC_DATA_PROC           (0xC735U) /**< @brief #QSPI_WriteTxCrcData  */
#define BRCM_SWDSGN_QSPI_CFG_REGS_GLOBAL                  (0xC736U) /**< @brief #QSPI_CFG_Regs  */
#define BRCM_SWDSGN_QSPI_BSPI_REGS_GLOBAL                 (0xC737U) /**< @brief #QSPI_BSPIRegs  */
#define BRCM_SWDSGN_QSPI_INT_PIO_READ_PROC                (0xC738U) /**< @brief #QSPI_IntPioRead */
#define BRCM_SWDSGN_QSPI_INT_PIO_WRITE_PROC               (0xC739U) /**< @brief #QSPI_IntPioWrite */
#define BRCM_SWDSGN_QSPI_CRC_RESET_SET_PROC               (0xC73AU) /**< @brief #QSPI_CrcResetSet */
#define BRCM_SWDSGN_QSPI_INT_CONFIG_READ_PROC             (0xC73BU) /**< @brief #QSPI_IntConfigRead */

/** @} */

/**
    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace #BRCM_SWARCH_QSPI_DRV_INIT_PROC
    @trace #BRCM_SWARCH_QSPI_MODE_CONFIGURE_PROC
    @trace #BRCM_SWARCH_QSPI_SET_CONFIG_PROPERTY_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_XFER_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_WRITE_PROC
*/
static QSPI_RDBType * const QSPI_Regs[QSPI_MAX_HW_ID] =
{
#if (QSPI_MAX_HW_ID == 0)
#error "QSPI_MAX_HW_ID == 0"
#endif
    (QSPI_RDBType *)QSPI0_BASE,
#if (QSPI_MAX_HW_ID > 1)
    (QSPI_RDBType *)QSPI1_BASE,
#endif
#if (QSPI_MAX_HW_ID > 2)
    (QSPI_RDBType *)QSPI2_BASE,
#endif
#if (QSPI_MAX_HW_ID > 3)
    (QSPI_RDBType *)QSPI3_BASE,
#endif
#if (QSPI_MAX_HW_ID > 4)
#error "QSPI_MAX_HW_ID > 4 is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace #BRCM_SWARCH_QSPI_DRV_INIT_PROC
    @trace #BRCM_SWARCH_QSPI_MODE_CONFIGURE_PROC
    @trace #BRCM_SWARCH_QSPI_SET_CONFIG_PROPERTY_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_XFER_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_WRITE_PROC
*/
static BSPI_RDBType * const QSPI_BSPIRegs = (BSPI_RDBType *)FLASH_BSPI_BASE;

/**
    @brief Peripheral global configuration register.
    @trace #BRCM_SWARCH_QSPI_DRV_INIT_PROC
    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
static CFG_RDBType * const QSPI_CFG_Regs = (CFG_RDBType *)CFG_BASE;

/**

    @trace #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace #BRCM_SWARCH_QSPI_DRV_XFER_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_READ_PROC
*/
#define QSPI_WORD_LEN (4UL)

/**
    @brief Driver database structure.
    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace #BRCM_SWARCH_QSPI_DRV_INIT_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_XFER_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_READ_PROC
*/
typedef struct sQSPI_RWDevType {
    QSPI_StateType state;
    uint8_t crcEnable;
    uint8_t incCrcInTx;
} QSPI_RWDevType;

/**
    @brief Global data to store driver status
    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace #BRCM_SWARCH_QSPI_DRV_INIT_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_XFER_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_READ_PROC
*/
static QSPI_RWDevType QSPI_RWDev[QSPI_MAX_HW_ID] COMP_SECTION(".bss.drivers");


/**
    @trace #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace #BRCM_SWARCH_QSPI_DRV_XFER_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_READ_PROC
*/
static int QSPI_IntWaitUntilDone(QSPI_HwIDType aId)
{
    int retVal = BCM_ERR_OK;
    int timeout_counter = 1000;
    uint32_t sbusy;
    uint32_t sdata;
    do {
          sbusy = (QSPI_Regs[aId]->stat & QSPI_STAT_BUSY_MASK) >> QSPI_STAT_BUSY_SHIFT;
          sdata = (QSPI_Regs[aId]->stat & QSPI_STAT_TXFIFOEMPTY_MASK) >> QSPI_STAT_TXFIFOEMPTY_SHIFT;
          /* If TX/RX fifo wait is enabled CS will be low device will be in busy state, it is not requied to wait. */
          if(((QSPI_Regs[aId]->ctrl4_config & QSPI_CTRL4_CONFIG_TXFIFO_WAIT_EN_MASK) > 0UL) && (sdata > 0UL)) {
              break;
          }
          sdata = (QSPI_Regs[aId]->stat & QSPI_STAT_RXFIFOOVERFLOWSTAT_MASK) >> QSPI_STAT_RXFIFOOVERFLOWSTAT_SHIFT;
          if(((QSPI_Regs[aId]->ctrl4_config & QSPI_CTRL4_CONFIG_RXFIFO_WAIT_EN_MASK) > 0UL) && (sdata > 0UL)) {
              break;
          }
          timeout_counter--;
          BCM_CpuNDelay(1000UL);
    } while((sbusy > 0UL) && (timeout_counter > 0));

    if((timeout_counter == 0) && (sbusy > 0UL)) {
        retVal = BCM_ERR_TIME_OUT;
    }
    return retVal;
}

/** @brief  Power up and bring out of reset QSPI

    param[in]   aId  Index of the QSPI port

    return      void
    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace #BRCM_SWARCH_QSPI_DRV_INIT_PROC
 */
static void QSPI_IntPowerUpOutReset(QSPI_HwIDType aId)
{
    if(aId < (QSPI_MAX_HW_ID - 1UL)) {
        /* Not power down QSPI peripheral */
        QSPI_CFG_Regs->periph_pwrdn &= (uint32_t)(~(1UL << (CFG_PERIPH_PWRDN_PWRDN_SPI_0_SHIFT + aId)));
        /* Bring peripheral out of reset */
        QSPI_CFG_Regs->periph_reset |= (1UL << (CFG_PERIPH_RESET_SPI_0_SHIFT + aId));
    }
}

/** @brief Bring QSPI in reset

    param[in]   aId  Index of the QSPI port

    return      void
    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace #BRCM_SWARCH_QSPI_DRV_INIT_PROC
 */
static void QSPI_IntInReset(QSPI_HwIDType aId)
{
    if(aId < (QSPI_MAX_HW_ID - 1UL)) {
        /* Bring peripheral in reset */
        QSPI_CFG_Regs->periph_reset &= (uint32_t)(~(1UL << (CFG_PERIPH_RESET_SPI_0_SHIFT + aId)));
    }
}


/**
    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace #BRCM_SWARCH_QSPI_DRV_STATUS_GET_PROC
*/
static uint8_t QSPI_IntGetDevBusyStatus(QSPI_HwIDType aId)
{
    uint8_t sbusy = 0;
    if(aId < QSPI_MAX_HW_ID) {
        sbusy = (uint8_t)((QSPI_Regs[aId]->stat & QSPI_STAT_BUSY_MASK) >> QSPI_STAT_BUSY_SHIFT);
    }

    return sbusy;
}

/**
    @trace #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace #BRCM_SWARCH_QSPI_DRV_XFER_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_QSPI_DRV_READ_PROC
*/
static void QSPI_IntStartSpi(QSPI_HwIDType aId, uint32_t start_spi)
{
    if(aId < QSPI_MAX_HW_ID) {
      if(start_spi > 0UL) {
        QSPI_Regs[aId]->ctrl3_config |= QSPI_CTRL3_CONFIG_START_SPI_MASK ;
      } else {
        QSPI_Regs[aId]->ctrl3_config &= ~QSPI_CTRL3_CONFIG_START_SPI_MASK ;
      }
    }

}


/** @brief Initialize QSPI peripheral.

    This API initializes QSPI peripheral in default SPI mode, necessary software variables, data
    structures.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral
    @param[in]      aConfig     Pointer to QSPI driver configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_DRV_INIT_PROC

    @limitations None

    @code{.unparsed}
     Set the clock divider
     Set clock polarity and clock phase
     Set TX/RX Fifo wait enable
     Set TX/RX Fifo threshold level
     Select slave select and quad mode enable
     Enable SPI peripheral
   @endcode
*/
int32_t QSPI_DrvInit(QSPI_HwIDType aID, const QSPI_ConfigType * const aConfig)
{
    int retVal = BCM_ERR_OK;
    #ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID || aConfig == NULL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aConfig->clkDiv > QSPI_CLK_DIV_RATIO_BY_128) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }  else if (aConfig->clkPolClkPhase > QSPI_CPOL_CPHA_CTRL_11) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }  else if (aConfig->holdOffLen > 0xFUL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }  else if (aConfig->txFifoThr > 0x1FUL || aConfig->rxFifoThr > 0x1FUL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }  else if (aConfig->slaveSel > QSPI_SLAVE_SEL_NA) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
    #endif
        /* Enable peripheral block and bring peripheral out of reset */
        QSPI_IntPowerUpOutReset(aID);
        if(aConfig->clkDiv > 0UL) {
            QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_CLOCK_DIV_MASK;
            QSPI_Regs[aID]->ctrl4_config |= ((aConfig->clkDiv & 0x3FU) << QSPI_CTRL4_CONFIG_CLOCK_DIV_SHIFT);
        }
        QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_CPOL_CPHA_CTRL_MASK;
        QSPI_Regs[aID]->ctrl4_config |= ((aConfig->clkPolClkPhase & 0x3U) << QSPI_CTRL4_CONFIG_CPOL_CPHA_CTRL_SHIFT);
        if(aConfig->holdOffLen > 0UL) {
            QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_HOLDOFF_LEN_MASK;
            QSPI_Regs[aID]->ctrl4_config |= ((aConfig->holdOffLen & 0xFU) << QSPI_CTRL4_CONFIG_HOLDOFF_LEN_SHIFT);
        }
        if(aConfig->txFIFOWaitEn > 0UL) {
            QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_TXFIFO_WAIT_EN_MASK;
        }
        if(aConfig->rxFIFOWaitEn > 0UL) {
            QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_RXFIFO_WAIT_EN_MASK;
        }
        if(aConfig->txFifoThr > 0UL) {
            QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_TXFIFOTHRESH_MASK;
            QSPI_Regs[aID]->ctrl4_config |= ((aConfig->txFifoThr & 0x1FU) << QSPI_CTRL4_CONFIG_TXFIFOTHRESH_SHIFT);
        }

        if(aConfig->rxFifoThr > 0UL) {
            QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_RXFIFOTHRESH_MASK;
            QSPI_Regs[aID]->ctrl4_config |= ((aConfig->rxFifoThr & 0x1FU) << QSPI_CTRL4_CONFIG_RXFIFOTHRESH_SHIFT);
        }
        if(aConfig->fdxModeEn > 0UL) {
            QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_FDX_MODE_MASK;
        }
        if(aConfig->arbiterIfEn > 0UL) {
            QSPI_Regs[aID]->ctrl3_config |= QSPI_CTRL3_CONFIG_ARBITER_IF_EN_MASK;
        }

        /* QSPI will have SPI bus control to access SPI flash */
        if(aID == QSPI_HWID_3) {
            QSPI_Regs[aID]->ctrl3_config &= ~QSPI_CTRL3_CONFIG_SLAVE_SEL_MASK;
            QSPI_BSPIRegs->bspi_registers_mast_n_boot_ctrl |= BSPI_BMNBC_BSPI_REGISTERS_MAST_N_BOOT_CTRL_MAST_N_BOOT_MASK;

            QSPI_BSPIRegs->bspi_registers_mast_n_boot_ctrl &= ~BSPI_BMNBC_BSPI_REGISTERS_MAST_N_BOOT_CTRL_SLAVE_SEL_MASK;
            QSPI_BSPIRegs->bspi_registers_mast_n_boot_ctrl |= ((aConfig->slaveSel & 0x7U) << BSPI_BMNBC_BSPI_REGISTERS_MAST_N_BOOT_CTRL_SLAVE_SEL_SHIFT);

        } else {
            QSPI_Regs[aID]->ctrl3_config &= ~QSPI_CTRL3_CONFIG_SLAVE_SEL_MASK;
            QSPI_Regs[aID]->ctrl3_config |= ((aConfig->slaveSel & 0x7U) << QSPI_CTRL3_CONFIG_SLAVE_SEL_SHIFT);
        }
        if(aConfig->quadModeEn > 0UL) {
            QSPI_Regs[aID]->ctrl5_config |= QSPI_CTRL5_CONFIG_QUADMODE_EN_MASK;
            QSPI_Regs[aID]->ctrl3_config |= (QSPI_OPCODE_MODE_QUAD << QSPI_CTRL3_CONFIG_OPCODE_MODE_SHIFT);
            QSPI_Regs[aID]->ctrl3_config |= (QSPI_ADDR_MODE_QUAD << QSPI_CTRL3_CONFIG_ADDR_MODE_SHIFT);
            QSPI_Regs[aID]->ctrl3_config |= (QSPI_DATA_MODE_QUAD << QSPI_CTRL3_CONFIG_DATA_MODE_SHIFT);
            QSPI_Regs[aID]->ctrl3_config |= (QSPI_MODE_BITS_QUAD << QSPI_CTRL3_CONFIG_MODEBITS_MODE_SHIFT);
        }

        QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_TXFIFOFLUSH_MASK;
        QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_RXFIFOFLUSH_MASK;

        QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_SPI_ENABLE_MASK;

        QSPI_RWDev[aID].state = QSPI_STATE_INIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief De-initialize QSPI peripheral.

    This API un-initializes QSPI peripheral and frees necessary software variables, data
     structures.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral

    @return     void

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_DRV_DEINIT_PROC

    @limitations None

    @code{.unparsed}
     Disable the SPI
    @endcode
*/
int32_t QSPI_DrvDeInit(QSPI_HwIDType aID)
{
    int retVal = BCM_ERR_OK;

    if (aID < QSPI_MAX_HW_ID)
    {
        QSPI_RWDev[aID].state = QSPI_STATE_UNINIT;
        QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_SPI_ENABLE_MASK;
        /* Reset the QSPI peripheral block.*/
        QSPI_IntInReset(aID);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

/** @brief Configure QSPI mode of operation.

    This API configures mode of operation sent to  QSPI peripheral for instruction opcode and mode bits.
    Mode of operation needs to be configured before any read/write transaction.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral.
    @param[in]      aModeConfig     Pointer to QSPI mode operation structure.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace #BRCM_SWARCH_QSPI_MODE_CONFIGURE_PROC

    @limitations None

    @code{.unparsed}
     Configure the address mode, mode bits mode,
     data mode and opcode mode
    @endcode
*/
int32_t QSPI_ModeConfigure(QSPI_HwIDType aID, const QSPI_ModeOpConfigType * const aModeConfig)
{
    int retVal = BCM_ERR_OK;
    #ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID || aModeConfig == NULL) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (aModeConfig->addrMode > QSPI_ADDR_MODE_QUAD) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aModeConfig->addrModeLen > QSPI_ADDR_MODE_LEN_BYTE_4) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aModeConfig->dataMode > QSPI_DATA_MODE_QUAD) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aModeConfig->opcodeMode > QSPI_OPCODE_MODE_QUAD) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aModeConfig->modeBits > QSPI_MODE_BITS_QUAD) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }  else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
    #endif
        if(aModeConfig->quadModeEn > 0UL) {
            QSPI_Regs[aID]->ctrl5_config |= QSPI_CTRL5_CONFIG_QUADMODE_EN_MASK;
        } else {
            QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_QUADMODE_EN_MASK;
        }
        QSPI_Regs[aID]->ctrl3_config &= ~(QSPI_CTRL3_CONFIG_ADDR_MODE_MASK | QSPI_CTRL3_CONFIG_ADDR_LEN_MASK);
        QSPI_Regs[aID]->ctrl3_config |= (aModeConfig->addrMode << QSPI_CTRL3_CONFIG_ADDR_MODE_SHIFT);
        QSPI_Regs[aID]->ctrl3_config |= (aModeConfig->addrModeLen << QSPI_CTRL3_CONFIG_ADDR_LEN_SHIFT);

        QSPI_Regs[aID]->ctrl3_config &= ~QSPI_CTRL3_CONFIG_OPCODE_MODE_MASK;
        QSPI_Regs[aID]->ctrl3_config |= (aModeConfig->opcodeMode<< QSPI_CTRL3_CONFIG_OPCODE_MODE_SHIFT);

        QSPI_Regs[aID]->ctrl3_config &= ~(QSPI_CTRL3_CONFIG_MODEBITS_MODE_MASK | QSPI_CTRL3_CONFIG_MODEBITS_LEN_MASK);
        QSPI_Regs[aID]->ctrl3_config |= (aModeConfig->modeBits << QSPI_CTRL3_CONFIG_MODEBITS_MODE_SHIFT);
        QSPI_Regs[aID]->ctrl3_config |= (aModeConfig->modeBitsLen<< QSPI_CTRL3_CONFIG_MODEBITS_LEN_SHIFT);

        QSPI_Regs[aID]->ctrl3_config &= ~QSPI_CTRL3_CONFIG_DATA_MODE_MASK;
        QSPI_Regs[aID]->ctrl3_config |= (aModeConfig->dataMode << QSPI_CTRL3_CONFIG_DATA_MODE_SHIFT);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Set selected config property

    This API enables/disables the selected config property.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral.
    @param[in]      confProp  Select the config property
    @param[in]      aVal        Value to be configured for the selected config property.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INTERFACE
    @trace  #BRCM_SWARCH_QSPI_SET_CONFIG_PROPERTY_PROC


    @limitations None

    @code{.unparsed}
     Set the QSPI_ConfigPropType values.
    @endcode
*/
int32_t QSPI_SetConfigProperty(QSPI_HwIDType aID, QSPI_ConfigPropType confProp, uint32_t aVal)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }  else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        switch(confProp) {
            case QSPI_CONFIG_PROP_RX_FSM_NO_TRANSIT :
                 QSPI_Regs[aID]->ctrl3_config &= ~QSPI_CTRL3_CONFIG_RX_FSMNOTRANSIT_EN_MASK;
                 QSPI_Regs[aID]->ctrl3_config |= ((aVal & 0x1UL) << QSPI_CTRL3_CONFIG_RX_FSMNOTRANSIT_EN_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_TRANS_REPEAT :
                 QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_SPI_TRANS_REPEAT_MASK;
                 QSPI_Regs[aID]->ctrl5_config |= ((aVal & 0xFFUL) << QSPI_CTRL5_CONFIG_SPI_TRANS_REPEAT_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_CSB_HIGH_LEN :
                 QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_CSB_HIGH_LEN_MASK;
                 QSPI_Regs[aID]->ctrl5_config |= ((aVal & 0xFFUL) << QSPI_CTRL5_CONFIG_CSB_HIGH_LEN_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_EN :
                 QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_AUTO_POLL_ENABLE_MASK;
                 QSPI_Regs[aID]->ctrl5_config |= ((aVal & 0x1UL) << QSPI_CTRL5_CONFIG_AUTO_POLL_ENABLE_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_MODE_SEL :
                 QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_AUTO_POLL_MODE_MASK;
                 QSPI_Regs[aID]->ctrl5_config |= ((aVal & 0x1UL) << QSPI_CTRL5_CONFIG_AUTO_POLL_MODE_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_SELF_CLR :
                 QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_AUTO_POLL_SELFCLEAR_MASK;
                 QSPI_Regs[aID]->ctrl5_config |= ((aVal & 0x1UL) << QSPI_CTRL5_CONFIG_AUTO_POLL_SELFCLEAR_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_HOLDN_CTRL :
                 QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_HOLDN_CTRL_MASK;
                 QSPI_Regs[aID]->ctrl5_config |= ((aVal & 0x1UL) << QSPI_CTRL5_CONFIG_HOLDN_CTRL_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_WPN_CTRL :
                 QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_WPN_CTRL_MASK;
                 QSPI_Regs[aID]->ctrl5_config |= ((aVal & 0x1UL) << QSPI_CTRL5_CONFIG_WPN_CTRL_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_EXP_VAL :
                 QSPI_Regs[aID]->ctrl6_autopollexp = aVal;
                 break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_MASK_VAL :
                 QSPI_Regs[aID]->ctrl7_autopollmask = aVal;
                 break;
            case QSPI_CONFIG_PROP_TXFIFO_WAIT_EN :
                 QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_TXFIFO_WAIT_EN_MASK;
                 QSPI_Regs[aID]->ctrl4_config |= ((aVal & 0x1UL) << QSPI_CTRL4_CONFIG_TXFIFO_WAIT_EN_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_RXFIFO_WAIT_EN :
                 QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_RXFIFO_WAIT_EN_MASK;
                 QSPI_Regs[aID]->ctrl4_config |= ((aVal & 0x1UL) << QSPI_CTRL4_CONFIG_RXFIFO_WAIT_EN_SHIFT);
                 break;
            case QSPI_CONFIG_PROP_RXDLY_EN :
                 QSPI_Regs[aID]->ctrl5_config &= ~QSPI_CTRL5_CONFIG_RXDLY_EN_MASK;
                 QSPI_Regs[aID]->ctrl5_config |= ((aVal & 0x1UL) << QSPI_CTRL5_CONFIG_RXDLY_EN_SHIFT);
                 break;
            default:
                 retVal = BCM_ERR_NOSUPPORT;
                 break;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Get selected config property

    This API gets the configured value of the selected config property.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral.
    @param[in]      confProp  Select the config property.
    @param[out]    aVal        Get the configured value for the selected config property.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INTERFACE
    @trace  #BRCM_SWARCH_QSPI_GET_CONFIG_PROPERTY_PROC

    @limitations None

    @code{.unparsed}
     Get the QSPI_ConfigPropType values.
    @endcode
*/
int32_t QSPI_GetConfigProperty(QSPI_HwIDType aID, QSPI_ConfigPropType confProp, uint32_t *aVal)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        switch(confProp) {
            case QSPI_CONFIG_PROP_RX_FSM_NO_TRANSIT :
                *aVal = ((QSPI_Regs[aID]->ctrl3_config & QSPI_CTRL3_CONFIG_RX_FSMNOTRANSIT_EN_MASK) > 0UL) ? 1UL : 0UL;
                break;
            case QSPI_CONFIG_PROP_TRANS_REPEAT :

                *aVal = ((QSPI_Regs[aID]->ctrl5_config & QSPI_CTRL5_CONFIG_SPI_TRANS_REPEAT_MASK)) >> QSPI_CTRL5_CONFIG_SPI_TRANS_REPEAT_SHIFT;
                 break;
            case QSPI_CONFIG_PROP_CSB_HIGH_LEN :
                *aVal = (QSPI_Regs[aID]->ctrl5_config & QSPI_CTRL5_CONFIG_CSB_HIGH_LEN_MASK) >> QSPI_CTRL5_CONFIG_CSB_HIGH_LEN_SHIFT;
                 break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_EN :
                *aVal = (QSPI_Regs[aID]->ctrl5_config & QSPI_CTRL5_CONFIG_AUTO_POLL_ENABLE_MASK) >> QSPI_CTRL5_CONFIG_AUTO_POLL_ENABLE_SHIFT;
                break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_MODE_SEL :
                *aVal = (QSPI_Regs[aID]->ctrl5_config & QSPI_CTRL5_CONFIG_AUTO_POLL_MODE_MASK) >>  QSPI_CTRL5_CONFIG_AUTO_POLL_MODE_SHIFT;
                break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_SELF_CLR :
                *aVal = (QSPI_Regs[aID]->ctrl5_config & QSPI_CTRL5_CONFIG_AUTO_POLL_SELFCLEAR_MASK) >>  QSPI_CTRL5_CONFIG_AUTO_POLL_SELFCLEAR_SHIFT;
                break;
            case QSPI_CONFIG_PROP_HOLDN_CTRL :
                *aVal = (QSPI_Regs[aID]->ctrl5_config & QSPI_CTRL5_CONFIG_HOLDN_CTRL_MASK) >> QSPI_CTRL5_CONFIG_HOLDN_CTRL_SHIFT;
                break;
            case QSPI_CONFIG_PROP_WPN_CTRL :
                *aVal = (QSPI_Regs[aID]->ctrl5_config & QSPI_CTRL5_CONFIG_WPN_CTRL_MASK) >> QSPI_CTRL5_CONFIG_WPN_CTRL_SHIFT;
                break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_EXP_VAL :
                *aVal = (QSPI_Regs[aID]->ctrl6_autopollexp & QSPI_CTRL6_AUTOPOLLEXP_AUTO_POLL_EXP_MASK);
                break;
            case QSPI_CONFIG_PROP_CSB_AUTO_POLL_MASK_VAL :
                *aVal = (QSPI_Regs[aID]->ctrl7_autopollmask & QSPI_CTRL7_AUTOPOLLMASK_AUTO_POLL_MASK_MASK);
                break;
            case QSPI_CONFIG_PROP_TXFIFO_WAIT_EN :
                *aVal = (QSPI_Regs[aID]->ctrl4_config & QSPI_CTRL4_CONFIG_TXFIFO_WAIT_EN_MASK) >> QSPI_CTRL4_CONFIG_TXFIFO_WAIT_EN_SHIFT;
                break;
            case QSPI_CONFIG_PROP_RXFIFO_WAIT_EN :
                *aVal = (QSPI_Regs[aID]->ctrl4_config & QSPI_CTRL4_CONFIG_RXFIFO_WAIT_EN_MASK) >> QSPI_CTRL4_CONFIG_RXFIFO_WAIT_EN_SHIFT;
                break;
            case QSPI_CONFIG_PROP_RXDLY_EN :
                 *aVal = (QSPI_Regs[aID]->ctrl5_config & QSPI_CTRL5_CONFIG_RXDLY_EN_MASK) >> QSPI_CTRL5_CONFIG_RXDLY_EN_SHIFT;
                 break;
            default :
                retVal = BCM_ERR_NOSUPPORT;
                break;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief This API is deprecated it is recommended to call #QSPI_DrvWrite and #QSPI_DrvRead for the
    same purpose.
    QSPI transfer function to write data and read the response from QSPI device.

    This API performs transaction of specified number of bytes to QSPI peripheral connected as slave.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in] xferCmd   Pointer to QSPI command structure holding the instruction code and instruction frame register information
    @param[in] address   Some instruction require address of qspi peripheral to be sent [optional otherwise pass -1 if not required]
    @param[in] txData    Pointer to transmit buffer holding the data to write into memory
    @param[in] txSize    Number of byte to write
    @param[out] rxData    Pointer to receive buffer holding the data to read from memory
    @param[in] rxSize    Number of byte to read

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Write operation is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param
    @retval     #BCM_ERR_EAGAIN   Transfer failed try again
    @retval     #BCM_ERR_INVAL_STATE   Device is in invalid state
    @post None

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace    #BRCM_SWARCH_QSPI_DRV_XFER_PROC

    @limitation None
    @code{.unparsed}
     Set device address
     Set mode bits, opcode value and dummy cycle length
     If TX fifo is not full
        Write data to be transmitted
      If TX threshold is reached start SPI transfer
      Wait for transmit to complete
      Prepare for receiving the response from the device
      Start SPI for receiving the data
      Check if RX FIFO is not empty
           Read the data register that in turn reads the data from RX FIFO
            Read the data till desired number of byte is received from the device
    @endcode

 */
int32_t QSPI_DrvXfer(QSPI_HwIDType aID, QSPI_CommandXferType *xferCmd, uint32_t address, char *txData, uint32_t txSize, char *rxData, uint32_t rxSize)
{
    int retVal = BCM_ERR_OK;
    #ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID || NULL == xferCmd) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else if (NULL == txData && NULL == rxData) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if(txData != NULL && txSize == 0UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if(rxData != NULL && rxSize == 0UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }  else {
    #endif

        /* Transmit data  */
        if(txData != NULL && txSize > 0UL) {
              retVal = QSPI_DrvWrite(aID, xferCmd, address, txData, txSize);
        }
        /* Stop QSPI */
        if(retVal == BCM_ERR_OK) {
          retVal = QSPI_IntWaitUntilDone(aID);
        }

        QSPI_IntStartSpi(aID, 0UL);

        /* Receive the data */
        if(retVal == BCM_ERR_OK && rxData != NULL && rxSize > 0UL ) {
            retVal = QSPI_DrvRead(aID, xferCmd, address, rxData, rxSize);
        }
    #ifdef BCM8915X_PARAM_VALIDATION
    }
    #endif
    /* Stop QSPI */
    if(retVal == BCM_ERR_OK) {
            retVal = QSPI_IntWaitUntilDone(aID);
    }
    QSPI_IntStartSpi(aID, 0UL);
    return retVal;
}



/** @brief Writes data on QSPI peripheral in PIO mode.

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace    #BRCM_SWARCH_QSPI_DRV_WRITE_PROC
*/
static int32_t QSPI_IntPioWrite(QSPI_HwIDType aID, QSPI_CommandXferType *xferCmd, char *txData, uint32_t txSize)
{
    int retVal = BCM_ERR_OK;
    uint32_t len = 0UL;
    uint32_t data = 0UL;
    uint32_t count = 0UL;
    uint32_t remain = 0UL;
    uint32_t fifoThrStatus = 0UL;
    uint32_t fifoEmptyStatus = 0UL;
    uint32_t fifoFullStatus = 0UL;
    uint8_t flag = 0U;


    if((QSPI_RWDev[aID].crcEnable > 0U) && (QSPI_RWDev[aID].incCrcInTx > 0U)) {
       if(txSize > 4UL) {
           len = (txSize - 4UL);
       } else {
           retVal = BCM_ERR_INVAL_PARAMS;
       }
    } else {
       len = txSize;
    }
    remain = len;
    /* This data transfer implemented for Little Endian machine. It needs to be modified for big endian machine */
    while((count < len) && (BCM_ERR_OK == retVal)) {
        fifoThrStatus = (QSPI_Regs[aID]->stat & QSPI_STAT_TXFIFOTHRESHSTAT_MASK) >> QSPI_STAT_TXFIFOTHRESHSTAT_SHIFT;
        fifoFullStatus = (QSPI_Regs[aID]->intr & QSPI_INT_TXFULLINT_MASK) >> QSPI_INT_TXFULLINT_SHIFT;
        if (fifoFullStatus > 0UL) {
            retVal = QSPI_IntWaitUntilDone(aID);
        }
        if(BCM_ERR_OK == retVal) {
            /* Tx Fifo threshold status reached */
            if((fifoThrStatus > 0UL) && (flag == 0U) && ((xferCmd->xferMode & QSPI_TRANSFER_DATA_ONLY) < 1U)) {
                QSPI_IntStartSpi(aID, 1UL);
                flag = 1;
            }
            if(remain >= QSPI_WORD_LEN) {
                BCM_MemCpy(&data, (txData + count), QSPI_WORD_LEN);
                count += QSPI_WORD_LEN;
                remain = len - count;
                QSPI_Regs[aID]->dataregister = data;
            } else {
               BCM_MemCpy(&data, (txData + count), remain);
               count += remain;
               remain = 0UL;
               QSPI_Regs[aID]->dataregister = data;
            }
        }

    }
    fifoEmptyStatus = (QSPI_Regs[aID]->stat & QSPI_STAT_TXFIFOEMPTY_MASK) >> QSPI_STAT_TXFIFOEMPTY_SHIFT;
    if((retVal == BCM_ERR_OK) && (fifoEmptyStatus < 1UL)) {
        retVal = QSPI_IntWaitUntilDone(aID);
    }
    return retVal;
}

/** @brief Writes data on QSPI peripheral.

    This API is used to write "txSize" number of bytes on QSPI module data pointed by txData.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in] xferCmd   Pointer to QSPI command structure holding the instruction code and instruction frame register information
    @param[in] address   Address to be accessed in QSPI peripheral
    @param[in] txData    Pointer to transmit buffer holding the data to write into memory
    @param[in] txSize     Number of bytes to write. If only opcode needs to be sent, txSize should be passed as 0.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Write operation is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param
    @retval     #BCM_ERR_EAGAIN   Transfer failed try again
    @retval     #BCM_ERR_INVAL_STATE   Device is in invalid state
    @post None

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace    #BRCM_SWARCH_QSPI_DRV_WRITE_PROC

    @limitation None
    @code{.unparsed}
       Set device address
       Set mode bits, opcode value and dummy cycle length
       If TX fifo is not full
          Write data to be transmitted
        If TX threshold is reached start SPI transfer
        Wait for transmit to complete
    @endcode
 */
int32_t QSPI_DrvWrite(QSPI_HwIDType aID, QSPI_CommandXferType *xferCmd, uint32_t address, char *txData, uint32_t txSize)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID || NULL == xferCmd) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else if (NULL == txData) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if(txData != NULL && txSize == 0UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if((xferCmd->xferMode & QSPI_TRANSFER_DATA_ONLY) < 1U) {
            QSPI_Regs[aID]->ctrl0_addr = (address > 0UL) ? address : 0UL;
            QSPI_Regs[aID]->ctrl1_modebits = xferCmd->modeBits;
            QSPI_Regs[aID]->ctrl3_config &= ~(QSPI_CTRL3_CONFIG_OPCODE_VAL_MASK);
            QSPI_Regs[aID]->ctrl3_config |= xferCmd->opcodeVal;
            QSPI_Regs[aID]->ctrl3_config &= ~(QSPI_CTRL3_CONFIG_DUMMY_LEN_MASK);
            QSPI_Regs[aID]->ctrl3_config |= (xferCmd->dummyCycles << QSPI_CTRL3_CONFIG_DUMMY_LEN_SHIFT);
            if(xferCmd->dataLen > 0UL){
              QSPI_Regs[aID]->ctrl2_data_len = (xferCmd->dataLen - 1UL);
            } else {
              QSPI_Regs[aID]->ctrl2_data_len = 0UL;
            }
            /* If only opcode command needs to be sent. */
            if(((xferCmd->opcodeVal > 0UL) || (xferCmd->modeBits > 0UL)) && (((xferCmd->xferMode & QSPI_TRANSFER_OPCODE) > 0U) || (txSize == 0UL)))
            {
              /* Enable the write command. */
              QSPI_Regs[aID]->ctrl3_config |= QSPI_CTRL3_CONFIG_WRITE_READ_N_MASK;

              QSPI_IntStartSpi(aID, 1UL);
              /* In case only command is sent wait for command to complete. */
              retVal = QSPI_IntWaitUntilDone(aID);
            }
        }
        /* Transmit data  */
        if((retVal == BCM_ERR_OK) && (txData != NULL) && (txSize > 0UL)) {

          if(txSize < 32UL) {
              QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_TXFIFOTHRESH_MASK;
              QSPI_Regs[aID]->ctrl4_config |= ((txSize - 1UL) << QSPI_CTRL4_CONFIG_TXFIFOTHRESH_SHIFT);
          }
          /* Enable the write command. */
          QSPI_Regs[aID]->ctrl3_config |= QSPI_CTRL3_CONFIG_WRITE_READ_N_MASK;

          if((xferCmd->xferMode & QSPI_TRANSFER_DMA) > 0U) {
              QSPI_IntStartSpi(aID, 1UL);
          } else {
              retVal = QSPI_IntPioWrite(aID, xferCmd, txData, txSize);
          }
        }
    #ifdef BCM8915X_PARAM_VALIDATION
    }
    #endif
    /* Wait for QSPI to be free. */
    if((xferCmd->xferMode & QSPI_TRANSFER_DMA) == 0U) {
        if(retVal == BCM_ERR_OK) {
            retVal = QSPI_IntWaitUntilDone(aID);
        }

    }
    return retVal;
}

/** @brief Read data on QSPI peripheral in PIO mode.

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace    #BRCM_SWARCH_QSPI_DRV_WRITE_PROC

*/

static int32_t QSPI_IntPioRead(QSPI_HwIDType aID, char *rxData, uint32_t rxSize)
{
    int retVal = BCM_ERR_OK;
    uint32_t len = 0UL;
    uint32_t count = 0UL;
    uint32_t data = 0UL;
    uint32_t remain = 0UL;
    uint8_t busyStatus = 0U;
    uint32_t fifoEmptyStatus = 0UL;
    int counter = 1000;
    count = 0UL;
    len = remain = rxSize;
    do {
         fifoEmptyStatus = (QSPI_Regs[aID]->stat & QSPI_STAT_RXFIFOEMPTY_MASK) >> QSPI_STAT_RXFIFOEMPTY_SHIFT;
         busyStatus = QSPI_IntGetDevBusyStatus(aID);
         if((fifoEmptyStatus < 1UL)) {
             data = QSPI_Regs[aID]->dataregister;
             if(remain >= QSPI_WORD_LEN) {
                 BCM_MemCpy((rxData + count), &data, QSPI_WORD_LEN);
                 count += QSPI_WORD_LEN;
                 remain = len - count;
             } else {
                 BCM_MemCpy((rxData + count), &data, remain);
                 count += remain;
                 remain = 0UL;
             }
         } else if(busyStatus != 0U) {
           do {
                /* Wait for the received data in FIFO */
                BCM_CpuNDelay(1000UL);
                fifoEmptyStatus = (QSPI_Regs[aID]->stat & QSPI_STAT_RXFIFOEMPTY_MASK) >> QSPI_STAT_RXFIFOEMPTY_SHIFT;
                if(counter > 0) {
                    --counter;
                }
           } while ((counter > 0) && (fifoEmptyStatus != 0UL));
           if(counter == 0) {
                retVal = BCM_ERR_TIME_OUT;
                break;
           }
         } else {
               retVal = BCM_ERR_INVAL_STATE;
               break;
         }
    } while(count < len);
    return retVal;
}

/** @brief Configure data length.

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace    #BRCM_SWARCH_QSPI_DRV_READ_PROC

    @code{.unparsed}
         Set mode bits, opcode value and dummy cycle length

    @endcode
 */
static int32_t QSPI_IntConfigRead(QSPI_HwIDType aID, QSPI_CommandXferType *xferCmd, uint32_t rxSize)
{
    int retVal = BCM_ERR_OK;

    if(xferCmd->dataLen > 0UL){
    QSPI_Regs[aID]->ctrl2_data_len = (xferCmd->dataLen - 1UL);
    } else {
      QSPI_Regs[aID]->ctrl2_data_len = 0UL;
    }
    /* Send only opcode */
    if(((xferCmd->opcodeVal > 0UL) || (xferCmd->modeBits > 0UL)) && (((xferCmd->xferMode & QSPI_TRANSFER_OPCODE) > 0U) || (rxSize == 0UL)))
    {
        /* Enable the read command. */
        QSPI_Regs[aID]->ctrl3_config &= ~QSPI_CTRL3_CONFIG_WRITE_READ_N_MASK;
        QSPI_IntStartSpi(aID, 1UL);
        /* In case only command is sent wait for command to complete. */
        retVal = QSPI_IntWaitUntilDone(aID);
    }
    return retVal;
}

/** @brief Read data on QSPI peripheral.

    This API is used to read "rxSize" number of bytes on QSPI module and to stor in rxData buffer.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in] xferCmd   Pointer to QSPI command structure holding the instruction code and instruction frame register information
    @param[in] address   Address to be accessed in QSPI peripheral
    @param[out] rxData   Pointer to receive buffer holding the data to read from memory
    @param[in] rxSize      Number of bytes to read.  If only opcode needs to be sent, rxSize should be passed as 0.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Write operation is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param
    @retval     #BCM_ERR_EAGAIN   Transfer failed try again
    @retval     #BCM_ERR_INVAL_STATE   Device is in invalid state
    @post None

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace    #BRCM_SWARCH_QSPI_DRV_READ_PROC

    @limitation None

    @code{.unparsed}
       Set device address
       Set mode bits, opcode value and dummy cycle length
        Prepare for receiving the response from the device
        Start SPI for receiving the data
        Check if RX FIFO is not empty
             Read the data register that in turn reads the data from RX FIFO
              Read the data till desired number of byte is received from the device
    @endcode
 */
int32_t QSPI_DrvRead(QSPI_HwIDType aID, QSPI_CommandXferType *xferCmd, uint32_t address, char *rxData, uint32_t rxSize)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID || NULL == xferCmd) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    } else if (NULL == rxData) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }else if(rxData != NULL && rxSize == 0UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }  else {
#endif
        if((xferCmd->xferMode & QSPI_TRANSFER_DATA_ONLY) < 1U) {
            QSPI_Regs[aID]->ctrl0_addr = (address > 0UL) ? address : 0UL;
            QSPI_Regs[aID]->ctrl1_modebits = xferCmd->modeBits;
            QSPI_Regs[aID]->ctrl3_config &= ~(QSPI_CTRL3_CONFIG_OPCODE_VAL_MASK);
            QSPI_Regs[aID]->ctrl3_config |= xferCmd->opcodeVal;
            QSPI_Regs[aID]->ctrl3_config &= ~(QSPI_CTRL3_CONFIG_DUMMY_LEN_MASK);
            QSPI_Regs[aID]->ctrl3_config |= (xferCmd->dummyCycles << QSPI_CTRL3_CONFIG_DUMMY_LEN_SHIFT);
            retVal = QSPI_IntConfigRead(aID, xferCmd, rxSize);
        }
        /* Receive the data */
        if((retVal == BCM_ERR_OK) && (rxData != NULL) && (rxSize > 0UL) ) {
           if((xferCmd->xferMode & QSPI_TRANSFER_DATA_ONLY) < 1U) {
               /* Enable the read command. */
               QSPI_Regs[aID]->ctrl3_config &= ~QSPI_CTRL3_CONFIG_WRITE_READ_N_MASK;
               /* Start QSPI */
               if((xferCmd->xferMode & QSPI_TRANSFER_OPCODE) < 1U) {
                   QSPI_IntStartSpi(aID, 1UL);
               }
           }
           if(((xferCmd->xferMode & QSPI_TRANSFER_PIO) > 0U) || ((xferCmd->xferMode & QSPI_TRANSFER_DATA_ONLY) > 0U)) {
               /* This data transfer implemented for Little Endian machine. It needs to be modified for big endian machine */
               retVal = QSPI_IntPioRead(aID, rxData, rxSize);
           }
        }

 #ifdef BCM8915X_PARAM_VALIDATION
    }
 #endif
     if((xferCmd->xferMode & QSPI_TRANSFER_DMA) == 0U) {
         /* Wait for QSPI to be free. */
         if(retVal == BCM_ERR_OK) {
           retVal = QSPI_IntWaitUntilDone(aID);
         }
     }
     return retVal;
}

/** @brief DMA write request Enable.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in]      aId          Index of the QSPI controller
    @param[in]      enable     Enable (1UL)/Disable(0UL) Transmit DMA request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Write operation is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param
    @retval     #BCM_ERR_EAGAIN   Transfer failed try again
    @retval     #BCM_ERR_INVAL_STATE   Device is in invalid state
    @post None

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace    #BRCM_SWARCH_QSPI_DRV_DMA_REQ_WRITE_ENABLE_PROC

    @limitation None
    @code{.unparsed}
     Set the TX DMA enable in QSPI Control4 register.
    @endcode

 */
int32_t QSPI_DrvDMAReqWriteEnable(QSPI_HwIDType aID, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    #ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    } else {
    #endif
        if(aEnable > 0UL) {
            QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_TXDMAENABLE_MASK;
        } else {
            QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_TXDMAENABLE_MASK;
        }
    #ifdef BCM8915X_PARAM_VALIDATION
    }
    #endif

    return retVal;
}

/** @brief DMA read request Enable.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in]      aId         Index of the QSPI controller
    @param[in]      aEnable    Enable (1UL)/Disable(0UL) Receive DMA request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Write operation is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param
    @retval     #BCM_ERR_EAGAIN   Transfer failed try again
    @retval     #BCM_ERR_INVAL_STATE   Device is in invalid state
    @post None

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace    #BRCM_SWARCH_QSPI_DRV_DMA_REQ_READ_ENABLE_PROC

    @limitation None

    @code{.unparsed}
    Set the RX DMA enable in QSPI Control4 register.
    @endcode
 */
int32_t QSPI_DrvDMAReqReadEnable(QSPI_HwIDType aID, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        if(aEnable > 0UL) {
            QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_RXDMAENABLE_MASK;
        } else {
            QSPI_Regs[aID]->ctrl4_config &= ~QSPI_CTRL4_CONFIG_RXDMAENABLE_MASK;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;

}

/** @brief Flush the TX FIFO.

    This API is used to flush the TX FIFO.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INTERFACE
    @trace  #BRCM_SWARCH_QSPI_TX_FIFO_FLUSH_PROC

    @limitations None

    @code{.unparsed}
      Set the TX Flush FIFO in QSPI Control4 register.
    @endcode
*/
int32_t QSPI_TxFifoFlush(QSPI_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_TXFIFOFLUSH_MASK;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}

/** @brief Flush the RX FIFO.

    This API is used to flush the RX FIFO.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INTERFACE
    @trace  #BRCM_SWARCH_QSPI_RX_FIFO_FLUSH_PROC

    @limitations None

    @code{.unparsed}
      Set the TX Flush FIFO in QSPI Control4 register.
    @endcode
*/
int32_t QSPI_RxFifoFlush(QSPI_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        QSPI_Regs[aID]->ctrl4_config |= QSPI_CTRL4_CONFIG_RXFIFOFLUSH_MASK;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Get the device status of QSPI peripheral.

    This API initializes QSPI peripheral in default SPI mode, necessary software variables, data
    structures.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral
    @param[out]      devStatus   Pointer to QSPI device status structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INTERFACE
    @trace  #BRCM_SWARCH_QSPI_DRV_STATUS_GET_PROC

    @limitations None
    @code{.unparsed}
      Read the QSPI device status register
      Populate the device status in devStatus structure
    @endcode

*/
int32_t QSPI_DrvStatusGet(QSPI_HwIDType aID, QSPI_DevStatusType *devStatus)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t status;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aID >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (QSPI_RWDev[aID].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        BCM_MemSet(devStatus, 0U, sizeof(QSPI_DevStatusType));
        status  = QSPI_Regs[aID]->stat;
        devStatus->qspiBusy = ((status & QSPI_STAT_BUSY_MASK) > 0UL) ? 1U : 0U;
        devStatus->rxFIFOFullnessStat = (uint8_t)((status & QSPI_STAT_RXFIFOFULLNESSSTAT_MASK) >> QSPI_STAT_RXFIFOFULLNESSSTAT_SHIFT);
        devStatus->rxFIFOEmptyStat = ((status & QSPI_STAT_RXFIFOEMPTY_MASK) > 0UL) ? 1U : 0U;
        devStatus->rxFIFOthrStat = ((status & QSPI_STAT_RXFIFOTHRESHSTAT_MASK) > 0UL) ? 1U : 0U;
        devStatus->rxFIFOOverFlowStat = ((status & QSPI_STAT_RXFIFOOVERFLOWSTAT_MASK) > 0UL) ? 1U : 0U;
        devStatus->txFIFOFullnessStat = (uint8_t)((status & QSPI_STAT_TXFIFOFULLNESSSTAT_MASK) >> QSPI_STAT_TXFIFOFULLNESSSTAT_SHIFT);
        devStatus->txFIFOEmptyStat = ((status & QSPI_STAT_TXFIFOEMPTY_MASK) > 0UL) ? 1U : 0U;
        devStatus->txFIFOthrStat = ((status & QSPI_STAT_TXFIFOTHRESHSTAT_MASK) > 0UL) ? 1U : 0U;
        devStatus->txFIFOUnderFlowStat = ((status & QSPI_STAT_TXFIFOUNDERFLOWSTAT_MASK) > 0UL) ? 1U : 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;

}

/** @brief Enable the QSPI interrupt

    This API enable the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the QSPI peripheral
    @param[in]      intrType      Interrupt type
    @param[in]      aEnable       Enable or disable the selected interrupt type

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt enable is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_ENABLE_INTERRUPT_PROC

    @limitations None
    @code{.unparsed}
      Enable the interrupt type depending on interrupt type specified in
      intrType
      Option to select all the interrupt or individual interrupt
   @endcode
*/
int QSPI_EnableInterrupt(QSPI_HwIDType aId, QSPI_InterruptType intrType, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t intMask = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        if(intrType == QSPI_INTERRUPT_ALL) {
            intMask = (QSPI_INTMASK_RXEMPTYMASK_MASK | QSPI_INTMASK_RXTHRESHMASK_MASK | QSPI_INTMASK_RXFULLMASK_MASK |
                       QSPI_INTMASK_RXOVERFLOWMASK_MASK | QSPI_INTMASK_TXEMPTYMASK_MASK | QSPI_INTMASK_TXTHRESHMASK_MASK |
                       QSPI_INTMASK_TXFULLMASK_MASK | QSPI_INTMASK_TXUNDERFLOWMASK_MASK | QSPI_INTMASK_AUTOPOLLMASK_MASK |
                       QSPI_INTMASK_DONEMASK_MASK);
        } else {
            if((intrType & QSPI_INTERRUPT_RX_EMPTY_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_RXEMPTYMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_RX_THR_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_RXTHRESHMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_RX_FULL_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_RXFULLMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_RX_OVERFLOW_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_RXOVERFLOWMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_TX_EMPTY_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_TXEMPTYMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_TX_THR_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_TXTHRESHMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_TX_FULL_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_TXFULLMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_TX_UNDERFLOW_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_TXUNDERFLOWMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_AUTO_POLL_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_AUTOPOLLMASK_MASK;
            }
            if((intrType & QSPI_INTERRUPT_DONE_INTR) > 0UL) {
                intMask |= QSPI_INTMASK_DONEMASK_MASK;
            }
        }
        if(aEnable > 0UL) {
                QSPI_Regs[aId]->intmask |= intMask;
        } else {
                QSPI_Regs[aId]->intmask &= ~intMask;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Clear the Index of the QSPI peripheral interrupt

    This API clear the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the QSPI peripheral
    @param[in]      intrType      Clear the selected interrupt types

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt clear is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_CLEAR_INTERRUPT_PROC

    @limitations None
    @code{.unparsed}
       Clear the interrupt status
     @endcode
*/
int QSPI_ClearInterrupt(QSPI_HwIDType aId, QSPI_InterruptType intrType)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t status;

#ifdef BCM8915X_PARAM_VALIDATION
    if (aId>= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        /* Interrupt status register is clear on read  */
        status =  QSPI_Regs[aId]->intr;
        status =  QSPI_Regs[aId]->intr;

        (void)status;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Get the QSPI interrupt status for all the interrupts

    This API Gets the interrupt status for all the QSPI interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the QSPI peripheral
    @param[out]    intrStatus    Get QSPI interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_GET_INTERRUPT_STATUS_PROC

    @limitations None
    @code{.unparsed}
      Get the interrupt status depending on interrupt type specified in
      intrType
      Option to select all the interrupt or individual interrupt
    @endcode

*/
static void QSPI_GetAllInterruptStatus(QSPI_HwIDType aId, uint32_t *intrStatus)
{
    uint32_t status;

    *intrStatus = 0;
    status =  QSPI_Regs[aId]->intr;
    *intrStatus = (((status & QSPI_INT_RXEMPTYINT_MASK) > 0UL) ? QSPI_INTERRUPT_RX_EMPTY_INTR : 0UL) | (((status & QSPI_INT_RXTHRESHINT_MASK) > 0UL) ? QSPI_INTERRUPT_RX_THR_INTR : 0UL) |
                          (((status & QSPI_INT_RXFULLINT_MASK) > 0UL) ? QSPI_INTERRUPT_RX_FULL_INTR : 0UL) | (((status & QSPI_INT_RXOVERFLOWINT_MASK) > 0UL) ? QSPI_INTERRUPT_RX_OVERFLOW_INTR : 0UL) |
                          (((status & QSPI_INT_SAFETYRXUNDFLWINT_MASK) > 0UL) ? QSPI_INTERRUPT_SAFETY_RX_UNDFLW_INTR : 0UL) | (((status & QSPI_INT_SAFETYRXOVRFLWINT_MASK) > 0UL) ? QSPI_INTERRUPT_SAFETY_RX_OVRFLW_INTR : 0UL) |
                          (((status & QSPI_INT_SAFETYTXUNDFLWINT_MASK) > 0UL) ? QSPI_INTERRUPT_SAFETY_TX_UNDFLW_INTR : 0UL) | (((status & QSPI_INT_SAFETYTXOVRFLWINT_MASK) > 0UL) ? QSPI_INTERRUPT_SAFETY_TX_OVRFLW_INTR : 0UL) |
                          (((status & QSPI_INT_TXEMPTYINT_MASK) > 0UL) ? QSPI_INTERRUPT_TX_EMPTY_INTR : 0UL) | (((status & QSPI_INT_TXTHRESHINT_MASK) > 0UL) ? QSPI_INTERRUPT_TX_THR_INTR : 0UL) |
                          (((status & QSPI_INT_TXFULLINT_MASK) > 0UL) ? QSPI_INTERRUPT_TX_FULL_INTR : 0UL) | (((status & QSPI_INT_TXUNDERFLOWINT_MASK) > 0UL) ? QSPI_INTERRUPT_TX_UNDERFLOW_INTR : 0UL) |
                          (((status & QSPI_INT_AUTOPOLLINT_MASK) > 0UL) ? QSPI_INTERRUPT_AUTO_POLL_INTR : 0UL) | (((status & QSPI_INT_DONEINT_MASK) > 0UL) ? QSPI_INTERRUPT_DONE_INTR : 0UL);
}

#ifdef BCM8915X_INDIVIDUAL_INTR
/** @brief Get the QSPI Tx interrupt status

    This API Gets the interrupt status for the selected transmit interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the QSPI peripheral
    @param[in]     intrType      Get the interrupt status for the selected interrupt types
    @param[out]    intrStatus    Get QSPI interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_GET_INTERRUPT_STATUS_PROC

    @limitations None
    @code{.unparsed}
      Get the interrupt status depending on interrupt type specified in
      intrType
      Option to select all the interrupt or individual interrupt
    @endcode

*/
static void QSPI_GetTxInterruptStatus(QSPI_HwIDType aId, QSPI_InterruptType intrType, uint32_t *intrStatus)
{
    uint32_t status;
    status =  QSPI_Regs[aId]->intr;
    if(intrType == QSPI_INTERRUPT_TX_EMPTY_INTR) {
        *intrStatus |= (((status & QSPI_INT_TXEMPTYINT_MASK) > 0UL) ? QSPI_INTERRUPT_TX_EMPTY_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_TX_THR_INTR) {
        *intrStatus |= (((status & QSPI_INT_TXTHRESHINT_MASK) > 0UL) ? QSPI_INTERRUPT_TX_THR_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_TX_FULL_INTR) {
        *intrStatus |= (((status & QSPI_INT_TXFULLINT_MASK) > 0UL) ? QSPI_INTERRUPT_TX_FULL_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_TX_UNDERFLOW_INTR) {
        *intrStatus |= (((status & QSPI_INT_TXUNDERFLOWINT_MASK) > 0UL) ? QSPI_INTERRUPT_TX_UNDERFLOW_INTR : 0UL);
    }

}

/** @brief Get the QSPI Rx interrupt status

    This API Get the interrupt status for the selected receive interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the QSPI peripheral
    @param[in]     intrType      Get the interrupt status for the selected interrupt types
    @param[out]    intrStatus    Get QSPI interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_GET_INTERRUPT_STATUS_PROC

    @limitations None
    @code{.unparsed}
      Get the interrupt status depending on interrupt type specified in
      intrType
      Option to select all the interrupt or individual interrupt
    @endcode

*/
static void QSPI_GetRxInterruptStatus(QSPI_HwIDType aId, QSPI_InterruptType intrType, uint32_t *intrStatus)
{
    uint32_t status;
    status =  QSPI_Regs[aId]->intr;

    if(intrType == QSPI_INTERRUPT_RX_EMPTY_INTR) {
        *intrStatus |= (((status & QSPI_INT_RXEMPTYINT_MASK) > 0UL) ? QSPI_INTERRUPT_RX_EMPTY_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_RX_THR_INTR) {
        *intrStatus |= (((status & QSPI_INT_RXTHRESHINT_MASK) > 0UL) ? QSPI_INTERRUPT_RX_THR_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_RX_FULL_INTR) {
        *intrStatus |= (((status & QSPI_INT_RXFULLINT_MASK) > 0UL) ? QSPI_INTERRUPT_RX_FULL_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_RX_OVERFLOW_INTR) {
        *intrStatus |= (((status & QSPI_INT_RXOVERFLOWINT_MASK) > 0UL) ? QSPI_INTERRUPT_RX_OVERFLOW_INTR : 0UL);
    }

}

/** @brief Get the QSPI safety interrupt status

    This API Get the interrupt status for the selected safety interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the QSPI peripheral
    @param[in]     intrType      Get the interrupt status for the selected interrupt types
    @param[out]    intrStatus    Get QSPI interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_GET_INTERRUPT_STATUS_PROC

    @limitations None
    @code{.unparsed}
      Get the interrupt status depending on interrupt type specified in
      intrType
      Option to select all the interrupt or individual interrupt
    @endcode

*/
static void QSPI_GetSafetyInterruptStatus(QSPI_HwIDType aId, QSPI_InterruptType intrType, uint32_t *intrStatus)
{
    uint32_t status;
    status =  QSPI_Regs[aId]->intr;

    if(intrType == QSPI_INTERRUPT_SAFETY_RX_UNDFLW_INTR) {
        *intrStatus |= (((status & QSPI_INT_SAFETYRXUNDFLWINT_MASK) > 0UL) ? QSPI_INTERRUPT_SAFETY_RX_UNDFLW_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_SAFETY_RX_OVRFLW_INTR) {
        *intrStatus |= (((status & QSPI_INT_SAFETYRXOVRFLWINT_MASK) > 0UL) ? QSPI_INTERRUPT_SAFETY_RX_OVRFLW_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_SAFETY_TX_UNDFLW_INTR) {
        *intrStatus |= (((status & QSPI_INT_SAFETYTXUNDFLWINT_MASK) > 0UL) ? QSPI_INTERRUPT_SAFETY_TX_UNDFLW_INTR : 0UL);
    }
    if(intrType == QSPI_INTERRUPT_SAFETY_TX_OVRFLW_INTR) {
        *intrStatus |= (((status & QSPI_INT_SAFETYTXOVRFLWINT_MASK) > 0UL) ? QSPI_INTERRUPT_SAFETY_TX_OVRFLW_INTR : 0UL);
    }

}
#endif

/** @brief Get the QSPI interrupt status

    This API Get the interrupt status for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the QSPI peripheral
    @param[in]     intrType      Get the interrupt status for the selected interrupt types
    @param[out]    intrStatus    Get QSPI interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_GET_INTERRUPT_STATUS_PROC

    @limitations None
    @code{.unparsed}
      Get the interrupt status depending on interrupt type specified in
      intrType
      Option to select all the interrupt or individual interrupt
    @endcode

*/
int QSPI_GetInterruptStatus(QSPI_HwIDType aId, QSPI_InterruptType intrType, uint32_t *intrStatus)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_INDIVIDUAL_INTR
    uint32_t status = QSPI_Regs[aId]->intr;
#endif
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        *intrStatus = 0;
        if(intrType == QSPI_INTERRUPT_ALL) {
            QSPI_GetAllInterruptStatus(aId, intrStatus);
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else {
            QSPI_GetTxInterruptStatus(aId, intrType, intrStatus);
            QSPI_GetRxInterruptStatus(aId, intrType, intrStatus);
            QSPI_GetSafetyInterruptStatus(aId, intrType, intrStatus);
            if(intrType == QSPI_INTERRUPT_AUTO_POLL_INTR) {
                *intrStatus |= (((status & QSPI_INT_AUTOPOLLINT_MASK) > 0UL) ? QSPI_INTERRUPT_AUTO_POLL_INTR : 0UL);
            }
            if(intrType == QSPI_INTERRUPT_DONE_INTR) {
                *intrStatus |= (((status & QSPI_INT_DONEINT_MASK) > 0UL) ? QSPI_INTERRUPT_DONE_INTR : 0UL);
            }
        }
#endif
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;

}

/** @brief  QSPI IRQ Handler

  This API is called when QSPI interrupt is asserted.

  @param[in]    aId  Index of the QSPI port. Passed by osil
  @param[in]    aConfig     Pointer to the configuration structure

  @behavior Async, Non-reentrant

  @pre None

  @return       void
  @post None

  @trace #BRCM_SWREQ_QSPI_IRQ_HANDLER
  @trace #BRCM_SWARCH_QSPI_IRQ_HANDLER_PROC

  @limitations None
  @code{.unparsed}
     Read the interrupt status
     Handle the individual interrupt
     Clear the interrupt
  @endcode

 */
void QSPI_IRQHandler(QSPI_HwIDType aId, QSPI_ConfigType *aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    /* Application needs to implement IRQ handler based on use case. */
    retVal = QSPI_ClearInterrupt(aId, QSPI_INTERRUPT_ALL);
    (void)retVal;
}

/** @brief QSPI CRC control configuration.

    This API configures CRC control settings used by the CRC generation/check block. This API
    will not enable CRC, user needs to call #QSPI_CrcEnable to enable the CRC.

    @behavior Sync, Non-reentrant

    @pre QSPI is in initialized state.

    @param[in]      aId         Index of the QSPI peripheral
    @param[in]      crcCfg     Pointer to CRC control configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_CRC_CONTROL_CONFIG_SET_PROC

    @limitations None

    @code{.unparsed}
     Program CRC initial seed value.
     If required user can program XOR vector value otherwise default value is already set as 0xFFFF-FFFF
     Set include CRC in stream
     Program if CRC is required in complete read frame or data only.
     Select reverse bit options for input and output data.
    @endcode
*/
int QSPI_CrcControlConfigSet(QSPI_HwIDType aId, QSPI_CrcControlCfgType *crcCfg)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((aId >= QSPI_MAX_HW_ID) || (crcCfg == NULL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        /* CRC reset before configuring CRC control */
        QSPI_Regs[aId]->ctrl8_crcctrlreg |= QSPI_CTRL8_CRCCTRLREG_CRC_RESET_MASK;
        if(crcCfg->crcInitVal > 0UL) {
            QSPI_Regs[aId]->ctrl9_crc_init_val = crcCfg->crcInitVal;
        }
        if(crcCfg->crcXorVal > 0UL) {
            QSPI_Regs[aId]->ctrl10_crc_xor_val = crcCfg->crcXorVal;
        }

        QSPI_Regs[aId]->ctrl8_crcctrlreg &= ~QSPI_CTRL8_CRCCTRLREG_INCL_CRC_IN_STREAM_MASK;
        QSPI_Regs[aId]->ctrl8_crcctrlreg |= ((crcCfg->inclCrcInStream & 1UL) << QSPI_CTRL8_CRCCTRLREG_INCL_CRC_IN_STREAM_SHIFT);
        QSPI_RWDev[aId].incCrcInTx = (crcCfg->inclCrcInStream > 0UL) ? 1U: 0U ;
        QSPI_Regs[aId]->ctrl8_crcctrlreg &= ~QSPI_CTRL8_CRCCTRLREG_REV_IN_CTRL_MASK;
        QSPI_Regs[aId]->ctrl8_crcctrlreg |= ((crcCfg->revInputData & 0x3UL) << QSPI_CTRL8_CRCCTRLREG_REV_IN_CTRL_SHIFT);
        QSPI_Regs[aId]->ctrl8_crcctrlreg &= ~QSPI_CTRL8_CRCCTRLREG_REV_OUT_CTRL_MASK;
        QSPI_Regs[aId]->ctrl8_crcctrlreg |= ((crcCfg->revOutputData & 0x7UL) << QSPI_CTRL8_CRCCTRLREG_REV_OUT_CTRL_SHIFT);

        QSPI_Regs[aId]->ctrl8_crcctrlreg &= ~QSPI_CTRL8_CRCCTRLREG_RD_FRAME_CRC_MASK;
        QSPI_Regs[aId]->ctrl8_crcctrlreg |= ((crcCfg->rdFrameCrc & 1UL) << QSPI_CTRL8_CRCCTRLREG_RD_FRAME_CRC_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
        }
#endif

    return retVal;
}

/** @brief Gets CRC control configuration.

    This API gets CRC control settings used by the CRC generation/check block.

    @behavior Sync, Non-reentrant

    @pre QSPI is in initialized state.

    @param[in]      aId         Index of the QSPI peripheral
    @param[out]    crcCfg     Pointer to CRC control configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_CRC_CONTROL_CONFIG_GET_PROC

    @limitations None

    @code{.unparsed}
     Get the CRC control configuration.
    @endcode
*/
int QSPI_CrcControlConfigGet(QSPI_HwIDType aId, QSPI_CrcControlCfgType *crcCfg)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= QSPI_MAX_HW_ID || crcCfg == NULL) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        crcCfg->crcInitVal = QSPI_Regs[aId]->ctrl9_crc_init_val;
        crcCfg->crcXorVal = QSPI_Regs[aId]->ctrl10_crc_xor_val;
        crcCfg->inclCrcInStream = ((QSPI_Regs[aId]->ctrl8_crcctrlreg & QSPI_CTRL8_CRCCTRLREG_INCL_CRC_IN_STREAM_MASK) >> QSPI_CTRL8_CRCCTRLREG_INCL_CRC_IN_STREAM_SHIFT);
        crcCfg->revInputData = ((QSPI_Regs[aId]->ctrl8_crcctrlreg & QSPI_CTRL8_CRCCTRLREG_REV_IN_CTRL_MASK) >> QSPI_CTRL8_CRCCTRLREG_REV_IN_CTRL_SHIFT);
        crcCfg->revOutputData = ((QSPI_Regs[aId]->ctrl8_crcctrlreg & QSPI_CTRL8_CRCCTRLREG_REV_OUT_CTRL_MASK) >> QSPI_CTRL8_CRCCTRLREG_REV_OUT_CTRL_SHIFT);
        crcCfg->rdFrameCrc = ((QSPI_Regs[aId]->ctrl8_crcctrlreg & QSPI_CTRL8_CRCCTRLREG_RD_FRAME_CRC_MASK) >> QSPI_CTRL8_CRCCTRLREG_RD_FRAME_CRC_SHIFT);

#ifdef BCM8915X_PARAM_VALIDATION
        }
#endif
    return retVal;
}

/** @brief QSPI CRC reset.

    This API is used to reset the CRC calculation unit and set the data register to the value stored
     in the CRC_INIT register. It is self clear by the hardware.

    @behavior Sync, Non-reentrant

    @pre QSPI is in initialized state.

    @param[in]      aId         Index of the QSPI peripheral

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT
    @trace  #BRCM_SWARCH_QSPI_CRC_RESET_SET_PROC
    @limitations None

     @code{.unparsed}
    Set CRC reset.
     @endcode
*/
int QSPI_CrcResetSet(QSPI_HwIDType aId)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        /* CRC reset before configuring CRC control */
        QSPI_Regs[aId]->ctrl8_crcctrlreg |= QSPI_CTRL8_CRCCTRLREG_CRC_RESET_MASK;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}


/** @brief QSPI CRC enable/disable.

    This API enables/disables CRC calculation unit and sets the data register to the value stored in the CRC_INIT register.
    When CRC is disabled the CRC calculation will gracefully stop after completing the ongoing transaction.

    @behavior Sync, Non-reentrant

    @pre QSPI is in initialized state and it needs to be called before transmit or receive is initiated.

    @param[in]      aId         Index of the QSPI peripheral
    @param[in]      enable    1 : Enables CRC 0 : Disables CRC

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace  #BRCM_SWARCH_QSPI_CRC_ENABLE_PROC

    @limitations None

    @code{.unparsed}
     Enable CRC in ctrl8_crcctrlreg
     TX FIFO wait enable
    @endcode
*/
int QSPI_CrcEnable(QSPI_HwIDType aId, uint8_t enable)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        if(enable > 0U) {
            QSPI_Regs[aId]->ctrl8_crcctrlreg |= QSPI_CTRL8_CRCCTRLREG_CRC_ENABLE_MASK;
            QSPI_RWDev[aId].crcEnable = 1U;
            QSPI_Regs[aId]->ctrl4_config |= QSPI_CTRL4_CONFIG_TXFIFO_WAIT_EN_MASK;
        } else {
            QSPI_Regs[aId]->ctrl8_crcctrlreg &= ~QSPI_CTRL8_CRCCTRLREG_CRC_ENABLE_MASK;
            QSPI_RWDev[aId].crcEnable = 0U;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief QSPI CRC transmit done status.

    This API provides CRC TX done status if CRC is updated in TX CRC data register after transfer of data packets.
    Once CRC TX done status is true user must call #QSPI_GetCrcTxData to get CRC value to be transmitted at the end of
    transmitted packet.

    @behavior Sync, Non-reentrant

    @pre QSPI data transfer is completed or in progress and CRC is enabled.

    @param[in]      aId      Index of the QSPI peripheral
    @param[out]    done    1 : CRC update is done in CRC TX data 0 : CRC is not updated in CRC TX data

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post after TX done status is true user should call #QSPI_GetCrcTxData

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace  #BRCM_SWARCH_QSPI_CRC_TX_DONE_STATUS_PROC

    @limitations None
    @code{.unparsed}
     Get CRC TX done status
    @endcode
*/
int QSPI_CrcTxDoneStatus(QSPI_HwIDType aId, uint8_t *done)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((aId >= QSPI_MAX_HW_ID) || (done == NULL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        *done = ((QSPI_Regs[aId]->tx_crc_status & QSPI_CRC_TX_STATUS_DONE_MASK) > 0UL) ? 1U : 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}

/** @brief QSPI CRC receive done status.

    This API provides CRC RX done status if CRC is updated in RX CRC data register after it received the configured data
    packets.
    Once CRC RX done status is true user must call #QSPI_GetCrcRxData to get CRC value of the received packets.

    @behavior Sync, Non-reentrant

    @pre QSPI data receive is completed or in progress and CRC is enabled.

    @param[in]      aId      Index of the QSPI peripheral
    @param[out]    done    1 : CRC update is completed in CRC RX data 0 : CRC is not updated in CRC RX data

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post after TX done status is true user should call #QSPI_GetCrcRxData

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace  #BRCM_SWARCH_QSPI_CRC_RX_DONE_STATUS_PROC

    @limitations None
    @code{.unparsed}
     Get CRC RX done status
    @endcode
*/
int QSPI_CrcRxDoneStatus(QSPI_HwIDType aId, uint8_t *done)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((aId >= QSPI_MAX_HW_ID) || (done == NULL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        *done = ((QSPI_Regs[aId]->rx_crc_status & QSPI_CRC_RX_STATUS_DONE_MASK) > 0UL) ? 1U : 0U;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}

/** @brief Gets CRC value from TX data register.

    This API provides CRC value after reading the CRC TX data register.

    @behavior Sync, Non-reentrant

    @pre CRC TX done status should be true to get the latest CRC value after the calculation.

    @param[in]      aId             Index of the QSPI peripheral
    @param[out]    crcTxData   Reads CRC value from TX data register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace  #BRCM_SWARCH_QSPI_GET_CRC_TX_DATA_PROC

    @limitations None
    @code{.unparsed}
     Get CRC TX data.
    @endcode
*/
int QSPI_GetCrcTxData(QSPI_HwIDType aId, uint32_t *crcTxData)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((aId >= QSPI_MAX_HW_ID) || (crcTxData == NULL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        *crcTxData = QSPI_Regs[aId]->tx_crc_datareg;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}

/** @brief Gets CRC value from RX data register.

    This API provides CRC value after reading the CRC RX data register.

    @behavior Sync, Non-reentrant

    @pre CRC RX done status should be true to get the latest CRC value after the calculation.

    @param[in]      aId      Index of the QSPI peripheral
    @param[out]    crcRxata   Reads CRC value from Rx data register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace  #BRCM_SWARCH_QSPI_GET_CRC_RX_DATA_PROC

    @limitations None
*/
int QSPI_GetCrcRxData(QSPI_HwIDType aId, uint32_t *crcRxData)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((aId >= QSPI_MAX_HW_ID) || (crcRxData == NULL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
         *crcRxData = QSPI_Regs[aId]->rx_crc_datareg;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Writes Tx CRC value in QSPI data register.

    This API provides CRC value after reading the CRC TX data register.

    @behavior Sync, Non-reentrant

    @pre CRC TX done status should be true to get the latest CRC value, this will be written at the end of
    the packet transfer.

    @param[in]      aId             Index of the QSPI peripheral
    @param[in]      crcTxData   Writes CRC value in QSPI data register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION
    @trace  #BRCM_SWARCH_QSPI_WRITE_TX_CRC_DATA_PROC

    @limitations None

    @code{.unparsed}
     Write CRC TX data.
    @endcode
*/
int QSPI_WriteTxCrcData(QSPI_HwIDType aId, uint32_t crcTxData)
{
    int retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= QSPI_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (QSPI_RWDev[aId].state != QSPI_STATE_INIT) {
        retVal = BCM_ERR_UNINIT;
    }  else {
#endif
        QSPI_Regs[aId]->dataregister = crcTxData;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}

/** @} */
