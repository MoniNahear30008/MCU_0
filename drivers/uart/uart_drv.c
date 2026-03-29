/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_uartdrv_drv_impl UART Driver Design
    @ingroup grp_uartdrv

    @addtogroup grp_uartdrv_drv_impl
    @{

    @file drivers/uart/uart_drv.c
    @brief UART Driver Design

    @version 0.1 Initial version
*/

#include "uart_rdb.h"
#include <cfg_rdb.h>
#include <uart.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <compiler.h>

/**
    @name UART Driver Design IDs
    @{
    @brief Driver Design IDs for UART

*/
#define BRCM_SWDSGN_UART_DRV_INIT_PROC                   (0xB101U)    /**< @brief #UART_DrvInit        */
#define BRCM_SWDSGN_UART_DRV_SEND_PROC                   (0xB102U)    /**< @brief #UART_DrvSend        */
#define BRCM_SWDSGN_UART_DRV_GET_TX_STATUS_PROC          (0xB103U)    /**< @brief #UART_DrvGetTxStatus */
#define BRCM_SWDSGN_UART_DRV_DEINIT_PROC                 (0xB104U)    /**< @brief #UART_DrvDeInit      */
#define BRCM_SWDSGN_UART_DRV_SEND_BREAK_PROC             (0xB106U)    /**< @brief #UART_DrvSendBreak   */
#define BRCM_SWDSGN_UART_DRV_CLR_BREAK_PROC              (0xB107U)    /**< @brief #UART_DrvClrBreak    */
#define BRCM_SWDSGN_UART_DRV_GET_STATE_PROC              (0xB108U)    /**< @brief #UART_DrvGetState    */
#define BRCM_SWDSGN_UART_IRQ_HANDLER_PROC                (0xB10BU)    /**< @brief #UART_IRQHandler     */
#define BRCM_SWDSGN_UART_ERR_MASK_MACRO                  (0xB10CU)    /**< @brief #UART_ERR_MASK       */
#define BRCM_SWDSGN_UART_RWDEV_TYPE                      (0xB10DU)    /**< @brief #UART_RWDevType      */
#define BRCM_SWDSGN_UART_RWDEV_GLOBAL                    (0xB10EU)    /**< @brief #UART_RWDev          */
#define BRCM_SWDSGN_UART_REGS_GLOBAL                     (0xB10FU)    /**< @brief #UART_Regs           */
#define BRCM_SWDSGN_UART_DRV_RX_HANDLER_PROC             (0xB110U)    /**< @brief #UART_DrvRxHandler   */
#define BRCM_SWDSGN_UART_DRV_TX_HANDLER_PROC             (0xB111U)    /**< @brief #UART_DrvTxHandler   */
#define BRCM_SWDSGN_UART_INT_GET_BAUD_RATE_PROC          (0xB112U)    /**< @brief #UART_IntGetBaudRate */
#define BRCM_SWDSGN_UART_DRV_DMA_REQ_SEND_ENABLE_PROC    (0xB113U)    /**< @brief #UART_DrvDMAReqSendEnable */
#define BRCM_SWDSGN_UART_DRV_DMA_REQ_RECEIVE_ENABLE_PROC (0xB114U)    /**< @brief #UART_DrvDMAReqReceiveEnable */
#define BRCM_SWDSGN_UART_CFG_REGS_GLOBAL                 (0xB115U)    /**< @brief #UART_CFGRegs           */
#define BRCM_SWDSGN_UART_INT_POWERUP_OUTRESET_PROC       (0xB116U)    /**< @brief #UART_IntPowerUpOutReset */
#define BRCM_SWDSGN_UART_INT_INRESET_PROC                (0xB117U)    /**< @brief #UART_IntInReset */
#define BRCM_SWDSGN_UART_DRV_RECEIVE_PROC                (0xB118U)    /**< @brief #UART_DrvReceive         */
#define BRCM_SWDSGN_UART_ENABLE_INTERRUPT_PROC           (0xB119U)    /**< @brief #UART_EnableInterrupt */
#define BRCM_SWDSGN_UART_CLEAR_INTERRUPT_PROC            (0xB11AU)    /**< @brief #UART_ClearInterrupt  */
#define BRCM_SWDSGN_UART_GET_INTERRUPT_STATUS_PROC       (0xB11BU)    /**< @brief #UART_GetInterruptStatus  */
#define BRCM_SWDSGN_UART_GET_ALL_INTERRUPT_STATUS_PROC   (0xB11CU)    /**< @brief #UART_GetAllInterruptStatus*/

/** @} */

/**
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
#define UART_ERR_MASK (UART_UARTDR_OE_MASK | UART_UARTDR_BE_MASK | UART_UARTDR_PE_MASK | UART_UARTDR_FE_MASK)

/**
    @trace #BRCM_SWARCH_UART_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UART_DRV_SEND_PROC
    @trace #BRCM_SWARCH_UART_DRV_GET_TX_STATUS_PROC
    @trace #BRCM_SWARCH_UART_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UART_DRV_SEND_BREAK_PROC
    @trace #BRCM_SWARCH_UART_DRV_CLR_BREAK_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWARCH_UART_DRV_GET_STATE_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
typedef struct sUART_RWDevType {
    UART_StateType state;               /**< State of the UART */
    uint32_t timeout;                   /**< Timeout for uartfr busy loop */
    const uint8_t *data;
    uint32_t size;
    uint32_t curIdx;
    uint8_t *rdata;
    uint32_t rsize;
    uint32_t rcurIdx;
} UART_RWDevType;

/**
    @trace #BRCM_SWARCH_UART_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UART_DRV_SEND_PROC
    @trace #BRCM_SWARCH_UART_DRV_GET_TX_STATUS_PROC
    @trace #BRCM_SWARCH_UART_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UART_DRV_SEND_BREAK_PROC
    @trace #BRCM_SWARCH_UART_DRV_CLR_BREAK_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
static UART_RWDevType UART_RWDev[UART_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @trace #BRCM_SWARCH_UART_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UART_DRV_SEND_PROC
    @trace #BRCM_SWARCH_UART_DRV_GET_TX_STATUS_PROC
    @trace #BRCM_SWARCH_UART_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UART_DRV_SEND_BREAK_PROC
    @trace #BRCM_SWARCH_UART_DRV_CLR_BREAK_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
static UART_RDBType * const UART_Regs[UART_MAX_HW_ID] =
{
#if (UART_MAX_HW_ID == 0)
#error "UART_MAX_HW_ID == 0"
#endif
    (UART_RDBType *)UART0_BASE,
#if (UART_MAX_HW_ID > 1)
    (UART_RDBType *)UART1_BASE,
#endif
#if (UART_MAX_HW_ID > 2)
#error "UART_MAX_HW_ID > 2 is not supported by the driver"
#endif
};

/**
    @brief Peripheral global configuration register.
    @trace #BRCM_SWARCH_UART_DRV_INIT_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
static CFG_RDBType * const UART_CFGRegs = (CFG_RDBType *)CFG_BASE;


/** @brief  Handle RX Interrupt

    This API is used to handle RX interrupt and give a callback to the
    registered client

    @param[in]      aId         Index of the UART controller
    @param[in]      aData       Pointer to data to be received
    @param[inout]   aSize       Length of data to be received is passed as input, it gives actual
                                number of bytes received as output in same variable.
    @param[in]      aConfig     Pointer to the configuration structure

    return       Return if FIFO empty #BCM_ERR_OK
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
 */
static int32_t UART_DrvRxHandler(UART_HwIDType aId, uint8_t *aData, uint32_t *aSize, UART_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;
    UART_ErrorType dataErr = 0UL;
    uint32_t dataReg = 0UL;
    /*uint32_t maxCount = 2UL * 16UL;*/ /* Twice RX Fifo depth */
    uint32_t curIdx = 0;
    uint32_t flag = UART_Regs[aId]->uartfr;

    /* Mask the RX Interrupt */
    UART_Regs[aId]->uartimsc &= ~UART_UARTIMSC_RXIM_MASK;
    while (curIdx < *aSize) {
        if ((flag & UART_UARTFR_RXFE_MASK) == UART_UARTFR_RXFE_MASK) {
            break;
        }
        /*if(maxCount == 0UL) {
                  break;
              } */
        dataReg = UART_Regs[aId]->uartdr;
        /* Read the data register. If there is any error (Overrun, Break,
           Parity or Framing) ignore the byte otherwise copy the byte to
           internal buffer(rxBuff) */
        if ((dataReg & UART_ERR_MASK) != 0UL) {
            if ((dataReg & UART_UARTDR_OE_MASK) == UART_UARTDR_OE_MASK) {
                /* User defined code*/
            }
            if ((dataReg & UART_UARTDR_BE_MASK) == UART_UARTDR_BE_MASK) {
                /* User defined code*/
            }
            if ((dataReg & UART_UARTDR_PE_MASK) == UART_UARTDR_PE_MASK) {
                /* User defined code*/
            }
            if ((dataReg & UART_UARTDR_FE_MASK) == UART_UARTDR_FE_MASK) {
                /* User defined code*/
            }
            dataErr = (UART_ErrorType)((dataReg & UART_ERR_MASK) >> UART_UARTDR_FE_SHIFT);
            aData[curIdx] = ((uint8_t)dataReg) & ((uint8_t)UART_UARTDR_DATA_MASK);
            /*aConfig[aId].errCb(dataErr, data);*/
            (void)dataErr;
        } else {
            aData[curIdx] = ((uint8_t)dataReg) & ((uint8_t)UART_UARTDR_DATA_MASK);
            /*aConfig[aId].rxCb(&data, 1UL);*/
        }
        curIdx++;
        flag = UART_Regs[aId]->uartfr;
    }
    /* Unmask the RX Interrupt */
    UART_Regs[aId]->uartimsc |= UART_UARTIMSC_RXIM_MASK;
    *aSize = curIdx;
    return ret;
}


/** @brief  Handle TX Interrupt

    This API is used to handle TX interrupt and give a callback to the
    registered client

    param[in]    Id  Index of the UART port
    param[in]    UART configuration structure
    return       void
    @trace #BRCM_SWARCH_UART_DRV_SEND_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
static int32_t UART_DrvTxHandler(UART_HwIDType aId, UART_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;
    /* Mask the TX Interrupt */
    UART_Regs[aId]->uartimsc &= ~UART_UARTIMSC_TXIM_MASK;
    while (UART_RWDev[aId].curIdx < UART_RWDev[aId].size) {
        if((UART_Regs[aId]->uartfr & UART_UARTFR_TXFF_MASK) == UART_UARTFR_TXFF_MASK) {
            ret = BCM_ERR_BUSY;
            break;
        }

        UART_Regs[aId]->uartdr = (uint32_t)UART_RWDev[aId].data[UART_RWDev[aId].curIdx++];
        if (UART_RWDev[aId].curIdx == UART_RWDev[aId].size) {
            UART_RWDev[aId].curIdx = 0UL;
            UART_RWDev[aId].size = 0UL;
            UART_RWDev[aId].data = NULL;
            /*aConfig[aId].txCb();*/
            break;
        }
    }

    /* Unmask the TX Interrupt */
    UART_Regs[aId]->uartimsc |= UART_UARTIMSC_TXIM_MASK;
    return ret;
}

/** @brief Send data over UART

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      aData       Pointer to data to be sent
    @param[in]      aSize       Length of data to be sent
    @param[in]      aConfig     Pointer to the configuration structure

    @return     #BCM_ERR_OK             Success
                #BCM_ERR_INVAL_STATE    UART is not initialized
                #BCM_ERR_INVAL_PARAMS   aId is invalid
    @trace #BRCM_SWARCH_UART_DRV_SEND_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
    @limitations None

    @code{.unparsed}
        If Transmit FIFO is not full
             Write data into data register.
    @endcode
*/
int32_t UART_DrvSend(UART_HwIDType aId, const uint8_t* aData, uint32_t aSize, UART_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;
    if ((UART_MAX_HW_ID <= aId)
        || (NULL == aData)
        || (0UL == aSize)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (UART_STATE_UNINIT == UART_RWDev[aId].state) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (UART_RWDev[aId].curIdx != UART_RWDev[aId].size) {
        ret = BCM_ERR_BUSY;
    } else {
        UART_RWDev[aId].curIdx = 0UL;
        UART_RWDev[aId].size = aSize;
        UART_RWDev[aId].data = aData;
        ret = UART_DrvTxHandler(aId, aConfig);
    }

    return ret;
}

/** @brief Receive data over UART

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      aData       Pointer to data to be received
    @param[inout]   aSize       Length of the data in bytes to be received is passed as input and it provides actual
                                number of received bytes as output in the same variable.
    @param[in]      aConfig     Pointer to the configuration structure

    @return     #BCM_ERR_OK             Success
                #BCM_ERR_INVAL_STATE    UART is not initialized
                #BCM_ERR_INVAL_PARAMS   aId is invalid or aData and aSize is NULL

    @trace  #BRCM_SWREQ_UART_COMMUNICATION
    @trace  #BRCM_SWARCH_UART_DRV_RECEIVE_PROC

    @limitations None

    @code{.unparsed}
        If Receive FIFO is not empty
                read data from data register and store in buffer.
    @endcode

*/
int32_t UART_DrvReceive(UART_HwIDType aId, uint8_t * aData, uint32_t *aSize,
        UART_ConfigType *aConfig) {
    int32_t ret = BCM_ERR_OK;
    if ((UART_MAX_HW_ID <= aId)
        || (NULL == aData)
        || (NULL == aSize)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (UART_STATE_UNINIT == UART_RWDev[aId].state) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        UART_RWDev[aId].rcurIdx = 0UL;
        UART_RWDev[aId].rsize = *aSize;
        UART_RWDev[aId].rdata = aData;
        ret = UART_DrvRxHandler(aId, aData, aSize, aConfig);
    }

    return ret;

}

/** @brief  UART IRQ Handler

    This API is called when PL011 interrupt is asserted

    param[in]    Id  Index of the UART port. Passed by osil.

    return       void
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
 */
int32_t UART_IRQHandler(UART_HwIDType aId, UART_ConfigType *aConfig)
{
    uint32_t intr;
    int32_t retVal = BCM_ERR_OK;

    if (aId >= UART_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (UART_RWDev[aId].state != UART_STATE_INIT) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        intr = UART_Regs[aId]->uartmis;
        /* Clear the interrupt */
        UART_Regs[aId]->uarticr = intr;

        if (intr != 0UL) {
            // if ((intr & UART_UARTMIS_RXMIS_MASK) == UART_UARTMIS_RXMIS_MASK) {
            //     /* Clear the interrupt */
            //     UART_Regs[aId]->uarticr |= UART_UARTICR_RXIC_MASK;
            //     retVal = UART_DrvRxHandler(aId, UART_RWDev[aId].rdata, &UART_RWDev[aId].rsize, aConfig);

            // }
            // if ((retVal == BCM_ERR_OK) && ((intr & UART_UARTMIS_RTMIS_MASK) == UART_UARTMIS_RTMIS_MASK)) {
            //     /* Clear the interrupt */
            //     UART_Regs[aId]->uarticr |= UART_UARTICR_RTIC_MASK;
            //     /*aConfig[aId].errCb((UART_ErrorType)UART_ERROR_TIMEOUT, 0U);*/
            //     retVal = UART_DrvRxHandler(aId, UART_RWDev[aId].rdata, &UART_RWDev[aId].rsize, aConfig);
            // }
            if ((retVal == BCM_ERR_OK) && ((intr & UART_UARTMIS_TXMIS_MASK) == UART_UARTMIS_TXMIS_MASK)) {
                /* Clear the interrupt */
                UART_Regs[aId]->uarticr |= UART_UARTICR_TXIC_MASK;
                retVal = UART_DrvTxHandler(aId, aConfig);
            }
        }
    }

    return retVal;
}

/** @brief  Get the baud rate in bit per second (bps)


    param[in]   baudType  Baud type which need to be translated in bps rate.

    return      uint32_t
    @trace #BRCM_SWARCH_UART_DRV_INIT_PROC
    @trace #BRCM_SWREQ_UART_INIT
 */
static uint32_t UART_IntGetBaudRate(UART_BaudType baudType)
{
    uint32_t baud;
    switch(baudType) {
        case UART_BAUD_9600:
            baud = 9600UL;
        break;
        case UART_BAUD_19200:
            baud = 19200UL;
        break;
        case UART_BAUD_38400:
            baud = 38400UL;
        break;
        case UART_BAUD_115200:
            baud = 115200UL;
        break;
        case UART_BAUD_230400:
            baud = 230400UL;
            break;
        case UART_BAUD_500000:
            baud = 500000UL;
            break;
        case UART_BAUD_1000000:
            baud = 1000000UL;
            break;
        case UART_BAUD_2000000:
            baud = 2000000UL;
            break;
        case UART_BAUD_5000000:
            baud = 5000000UL;
            break;
        default:
            baud = 115200UL;
        break;
    }
    return baud;
}

/** @brief  Power up and bring out of reset UART

    param[in]   aId  Index of the UART port

    return      void
    @trace #BRCM_SWARCH_UART_DRV_INIT_PROC
    @trace #BRCM_SWREQ_UART_INIT
 */
static void UART_IntPowerUpOutReset(UART_HwIDType aId)
{

    /* Not power down UART peripheral */
    /* @event           CERT-C INT30
                @issue          Coverity infers that the addition would overflow
                @risk           No risk
                @justification  aId got validated in calling function.
                                aId would not exceed totalSize                          */
    /* coverity[cert_int30_c_violation] */
    /* coverity[cert_int34_c_violation] */
    UART_CFGRegs->periph_pwrdn &= (uint32_t)(~(1UL << (CFG_PERIPH_PWRDN_PWRDN_URT_0_SHIFT + aId)));
    /* Bring peripheral out of reset */
    /* @event           CERT-C INT30
                @issue          Coverity infers that the addition would overflow
                @risk           No risk
                @justification  aId got validated in calling function.
                                aId would not exceed totalSize                          */
    /* coverity[cert_int30_c_violation] */
    /* coverity[cert_int34_c_violation] */
    UART_CFGRegs->periph_reset |= (1UL << (CFG_PERIPH_RESET_URT_0_SHIFT + aId));
}

/** @brief Bring UART in reset

    param[in]   aId  Index of the UART port

    return      void
    @trace #BRCM_SWARCH_UART_DRV_INIT_PROC
    @trace #BRCM_SWREQ_UART_INIT
 */
static void UART_IntInReset(UART_HwIDType aId)
{
    /* Bring peripheral in reset */
    /* @event           CERT-C INT34
                @issue          Coverity infers that the addition would overflow
                @risk           No risk
                @justification  aId got validated in calling function.
                                aId would not exceed totalSize                          */
    /* coverity[cert_int30_c_violation] */
    /* coverity[cert_int34_c_violation] */
    UART_CFGRegs->periph_reset |= (1UL << (CFG_PERIPH_RESET_URT_0_SHIFT + aId));
}


/** @brief  Initialize PL011
 *
 *  This API is used to initialize the UART hardware with the given parameters
 *
 *  param[in]   Id           Index of the UART port
 *  param[in]   aConfig  Pointer to the UART Configuration structure
 *
 *  return      void
 *
 *  @trace #BRCM_SWARCH_UART_DRV_INIT_PROC
 *  @trace #BRCM_SWREQ_UART_INIT
 */
void UART_DrvInit(UART_HwIDType aId, UART_ConfigType *aConfig)
{
    uint32_t baud;
    uint32_t stopBits = UART_STOP_BITS1;
    uint32_t parity = UART_PARITY_NONE;
    uint32_t divisor;
    uint32_t timeout;
    uint32_t err = 0UL;
    UART_RWDev[aId].state = UART_STATE_UNINIT;
    if (aId < UART_MAX_HW_ID) {
        const UART_ConfigType *const config = aConfig;
        if (UART_RWDev[aId].state == UART_STATE_UNINIT) {
            baud = UART_IntGetBaudRate(config->baud);
            if (config->stopBits == UART_STOP_BITS1) {
                stopBits = 0UL;
            } else if (config->stopBits == UART_STOP_BITS2) {
                stopBits = UART_UARTLCR_STP2_MASK;
            } else {
                err = 1UL;
            }
            if (err == 0UL) {
                if (config->parity == UART_PARITY_NONE) {
                    parity = 0UL;
                } else if (config->parity == UART_PARITY_EVEN) {
                    parity = UART_UARTLCR_PEN_MASK | UART_UARTLCR_EPS_MASK;
                } else if (config->parity == UART_PARITY_ODD) {
                    parity = UART_UARTLCR_PEN_MASK;
                } else {
                    err = 1UL;
                }
                if (err == 0UL) {
                    timeout = 100000UL;
                    UART_RWDev[aId].timeout = timeout;
                    /* Enable peripheral block and bring peripheral out of reset */
                    UART_IntPowerUpOutReset(aId);
                    /* Disable FIFO */
                    UART_Regs[aId]->uartlcr &= ~UART_UARTLCR_FEN_MASK;
                    /* Wait for uart to finish the last transmit before
                     * programming the control registers. */
                    while (((UART_Regs[aId]->uartfr) & UART_UARTFR_BUSY_MASK)
                            == UART_UARTFR_BUSY_MASK) {
                        if (timeout == 0UL) {
                            break;
                        }
                        timeout--;
                    }
                    /* Disable UART */
                    UART_Regs[aId]->uartcr &= ~UART_UARTCR_UARTEN_MASK;
                    /* Set TX and RX FIFO threshold level */
                    UART_Regs[aId]->uartifls = (config->txFifoLvl << UART_UARTIFLS_TXIFLSEL_SHIFT)
                        | (config->rxFifoLvl << UART_UARTIFLS_RXIFLSEL_SHIFT);
                    /* Configure Baud Rate */
                    divisor = (UART_CLOCK / 16UL) / baud;
                    UART_Regs[aId]->uartibrd = divisor;
                    /* @event           CERT-C INT30
                                @issue          Coverity infers that the calculation may wrap
                                @risk           No risk
                                @justification  divisor and baud would not cause wrap */
                    /* coverity[cert_int30_c_violation] */
                    UART_Regs[aId]->uartfbrd = ((UART_CLOCK - (divisor * 16UL * baud)) * 64UL) / (16UL * baud);
                    /* Set the number of stop bits, parity, word length
                       and enable FIFO */
                    UART_Regs[aId]->uartlcr = stopBits | parity
                        | UART_UARTLCR_FEN_MASK | (UART_UARTLCR_WLEN_MASK & (aConfig->wordLength << UART_UARTLCR_WLEN_SHIFT));
                    /* Enable Loop back */
                    if (config->loopBackMode == 1UL) {
                        UART_Regs[aId]->uartcr |= UART_UARTCR_LBE_MASK;
                    }
                    /* Set the state to Init */
                    UART_RWDev[aId].state = UART_STATE_INIT;
                    /* Unmask RX, RX Timeout, TX interrupts */
                    UART_Regs[aId]->uartimsc |= UART_UARTIMSC_TXIM_MASK;
                    UART_Regs[aId]->uartimsc |= UART_UARTIMSC_RXIM_MASK;
                    UART_Regs[aId]->uartimsc |= UART_UARTIMSC_RTIM_MASK;
                    /* Enable TX, RX, UART */
                    UART_Regs[aId]->uartcr |= (UART_UARTCR_TXE_MASK | UART_UARTCR_RXE_MASK
                            | UART_UARTCR_UARTEN_MASK);
                }
            }
        }
    }
    return;
}

/** @brief  Deinitialize PL011

    This API is used to deinitialize the UART. It resets the controller and puts
    back to its original state.

    param[in]   Id      Index of the UART port

    return      BCM_ERR_OK              Success
                BCM_ERR_INVALID_PARAMS  Invalid hardware index
    @trace #BRCM_SWARCH_UART_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_UART_INIT
 */
int32_t UART_DrvDeInit(UART_HwIDType aId)
{
    uint32_t timeout;
    int32_t err = BCM_ERR_OK;

    if (aId < UART_MAX_HW_ID) {
        if (UART_RWDev[aId].state == UART_STATE_INIT) {
            timeout = UART_RWDev[aId].timeout;
            /* Mask all the interrupts */
            UART_Regs[aId]->uartimsc &= ~(UART_UARTIMSC_OEIM_MASK | UART_UARTIMSC_BEIM_MASK
                | UART_UARTIMSC_PEIM_MASK | UART_UARTIMSC_FEIM_MASK | UART_UARTIMSC_RTIM_MASK | UART_UARTIMSC_TXIM_MASK
                | UART_UARTIMSC_RXIM_MASK | UART_UARTIMSC_DSRMIM_MASK | UART_UARTIMSC_DCDMIM_MASK
                | UART_UARTIMSC_CTSMIM_MASK | UART_UARTIMSC_RIMIM_MASK);
            /* Clear all the interrupts */
            UART_Regs[aId]->uarticr = UART_UARTICR_RTIC_MASK | UART_UARTICR_TXIC_MASK
                | UART_UARTICR_RXIC_MASK | UART_UARTICR_DSRMIC_MASK | UART_UARTICR_DCDMIC_MASK | UART_UARTICR_CTSMIC_MASK
                | UART_UARTICR_RIMIC_MASK;
            /* Disable the FIFO mode */
            UART_Regs[aId]->uartlcr &= ~UART_UARTLCR_FEN_MASK;
            /* Wait for UART to become idle */
            while (((UART_Regs[aId]->uartfr) & UART_UARTFR_BUSY_MASK)
                        == UART_UARTFR_BUSY_MASK) {
               if (timeout == 0UL) {
                    break;
                }
               timeout--;
            }
            /* Disable UART */
            UART_Regs[aId]->uartcr &= ~UART_UARTCR_UARTEN_MASK;
            /* Bring UART in reset */
            UART_IntInReset(aId);
            /* Set the state to uninit */
            UART_RWDev[aId].state = UART_STATE_UNINIT;
        }
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }

    return err;
}

/** @brief  Send Break
 *
 *  This API is used to send Break on the respective hardware ID
 *
 *  param[in]   Id      Index of the UART port
 *
 *  return      void
    @trace #BRCM_SWARCH_UART_DRV_SEND_BREAK_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
 */
int32_t UART_DrvSendBreak(UART_HwIDType aId)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    if (aId < UART_MAX_HW_ID) {
        UART_Regs[aId]->uartlcr |= UART_UARTLCR_BRK_MASK;
        ret = BCM_ERR_OK;
    }
    return ret;
}


/** @brief  Clear Break
 *
 *  This API is used to clear Break on the respective hardware ID
 *
 *  param[in]   Id      Index of the UART port
 *
 *  return      void
    @trace #BRCM_SWARCH_UART_DRV_CLR_BREAK_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
 */
int32_t UART_DrvClrBreak(UART_HwIDType aId)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    if (aId < UART_MAX_HW_ID) {
        UART_Regs[aId]->uartlcr &= ~UART_UARTLCR_BRK_MASK;
        ret = BCM_ERR_OK;
    }
    return ret;
}

/** @brief  Get the state of the PL011
 *
 *  This API returns the current state of the UART in State argument
 *
 *  param[in]   Id      Index of the UART port
 *  param[out]  aState   Current state of the UART
 *
 *  return      BCM_ERR_OK              Success
 *              BCM_ERR_INVALID_PARAMS  Invalid hardware index
    @trace #BRCM_SWARCH_UART_DRV_GET_STATE_PROC
    @trace #BRCM_SWREQ_UART_INIT
 */
int32_t UART_DrvGetState(UART_HwIDType aId, UART_StateType *const aState)
{
    int32_t err = BCM_ERR_OK;

    if (aId < UART_MAX_HW_ID) {
        *aState = UART_RWDev[aId].state;
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }

    return err;
}

/** @brief Checks for transmit buffer status and transmit FIFO status

    This API checks if
      1) all buffer is pushed into FIFO
      2) all data in FIFO is send over line

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId      Index of the UART controller
    @param[out]     aStatus  UART TX buffer status
                    UART_TX_STATUS_BUFFER_IDLE  if Buffer is written to FIFO
                    UART_TX_STATUS_FIFO_IDLE    if FIFO is also empty along with BUFFER_IDLE

    @return     #BCM_ERR_OK             Success
                #BCM_ERR_INVAL_STATE    UART is not initialized
                #BCM_ERR_INVAL_PARAMS   aId is invalid

    @post None
    @trace #BRCM_SWARCH_UART_DRV_GET_TX_STATUS_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
int32_t UART_DrvGetTxStatus(UART_HwIDType aId, UART_TxStatusType *aStatus)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    if (aId < UART_MAX_HW_ID) {
        *aStatus = 0UL;
        if (0UL == UART_RWDev[aId].size) {
            *aStatus |= UART_TX_STATUS_BUFFER_IDLE;
        }
        if (0UL != (UART_Regs[aId]->uartfr & UART_UARTFR_TXFE_MASK)) {
            *aStatus |= UART_TX_STATUS_FIFO_IDLE;
        }
        ret = BCM_ERR_OK;
    }
    return ret;
}

/** @brief Enable the UART interrupt

    This API enables the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the UART controller
    @param[in]      intrType      Interrupt type
    @param[in]      aEnable       Enable or disable the selected interrupt type

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   UART driver interrupt enable is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace #BRCM_SWARCH_UART_ENABLE_INTERRUPT_PROC
    @trace  #BRCM_SWREQ_UART_INTERRUPT

    @limitations None
*/
int UART_EnableInterrupt(UART_HwIDType aId, UART_InterruptType intrType, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t intMask = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= UART_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
#endif
        if(intrType == UART_INTERRUPT_ALL) {
            intMask = (UART_UARTIMSC_RXIM_MASK | UART_UARTIMSC_TXIM_MASK |
                       UART_UARTIMSC_RTIM_MASK | UART_UARTIMSC_FEIM_MASK | UART_UARTIMSC_PEIM_MASK |
                       UART_UARTIMSC_BEIM_MASK | UART_UARTIMSC_OEIM_MASK);
        } else {
            if(intrType == UART_INTERRUPT_RX_INTR) {
                intMask |= UART_UARTIMSC_RXIM_MASK;
            }
            if(intrType == UART_INTERRUPT_TX_INTR) {
                intMask |= UART_UARTIMSC_TXIM_MASK;
            }
            if(intrType == UART_INTERRUPT_RX_TIMEOUT_INTR) {
                intMask |= UART_UARTIMSC_RTIM_MASK;
            }
            if(intrType == UART_INTERRUPT_ERROR_INTR) {
                intMask |= (UART_UARTIMSC_FEIM_MASK | UART_UARTIMSC_PEIM_MASK | UART_UARTIMSC_BEIM_MASK | UART_UARTIMSC_OEIM_MASK);
            }

        }
        if(aEnable > 0UL) {
                UART_Regs[aId]->uartimsc |= intMask;
            } else {
                UART_Regs[aId]->uartimsc &= ~intMask;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Clear the UART interrupt

    This API clears the interrupts for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the UART controller
    @param[in]      intrType      Clear the selected interrupt types

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   UART driver interrupt clear is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWARCH_UART_CLEAR_INTERRUPT_PROC
    @trace  #BRCM_SWREQ_UART_INTERRUPT

    @limitations None
*/
int UART_ClearInterrupt(UART_HwIDType aId, UART_InterruptType intrType)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t clearMask = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= UART_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
#endif
        if(intrType == UART_INTERRUPT_ALL) {
            clearMask = (UART_UARTICR_RXIC_MASK | UART_UARTICR_TXIC_MASK |
                       UART_UARTICR_RTIC_MASK | UART_UARTICR_FEIC_MASK | UART_UARTICR_PEIC_MASK |
                       UART_UARTICR_BEIC_MASK | UART_UARTICR_OEIC_MASK);
        } else {
            if(intrType == UART_INTERRUPT_RX_INTR) {
                clearMask |= UART_UARTICR_RXIC_MASK;
            }
            if(intrType == UART_INTERRUPT_TX_INTR) {
                clearMask |= UART_UARTICR_TXIC_MASK;
            }
            if(intrType == UART_INTERRUPT_RX_TIMEOUT_INTR) {
                clearMask |= UART_UARTICR_RTIC_MASK;
            }
            if(intrType == UART_INTERRUPT_ERROR_INTR) {
                clearMask |= (UART_UARTICR_FEIC_MASK | UART_UARTICR_PEIC_MASK | UART_UARTICR_BEIC_MASK | UART_UARTICR_OEIC_MASK);
            }

        }

        UART_Regs[aId]->uarticr |= clearMask;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Get the UART interrupt status for all the interrupts

    This API Gets the interrupt status for all the UART interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the UART peripheral
    @param[out]    intrStatus    Get UART interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   UART driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_UART_INTERRUPT
    @trace  #BRCM_SWARCH_UART_GET_INTERRUPT_STATUS_PROC

    @limitations None
    @code{.unparsed}
      Get the interrupt status depending on interrupt type specified in
      intrType
      Option to select all of the interrupts or an individual interrupt
    @endcode

*/
static void UART_GetAllInterruptStatus(UART_HwIDType aId, uint32_t *intrStatus)
{
    uint32_t status = 0UL;
    uint32_t errorIntrMask = (UART_UARTMIS_FEMIS_MASK | UART_UARTMIS_PEMIS_MASK | UART_UARTMIS_BEMIS_MASK | UART_UARTMIS_OEMIS_MASK);

    *intrStatus = 0;
    status =  UART_Regs[aId]->uartmis;
    *intrStatus = (((status & UART_UARTMIS_RXMIS_MASK) > 0UL) ? UART_INTERRUPT_RX_INTR : 0UL) | (((status & UART_UARTMIS_TXMIS_MASK) > 0UL) ? UART_INTERRUPT_TX_INTR : 0UL) |
                          (((status & UART_UARTMIS_RTMIS_MASK) > 0UL) ? UART_INTERRUPT_RX_TIMEOUT_INTR : 0UL) |
                          (((status & errorIntrMask) > 0UL) ? UART_INTERRUPT_ERROR_INTR : 0UL);
}

/** @brief Get the UART interrupt status

    This API Gets the interrupt status for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the UART controller
    @param[in]     intrType      Get the interrupt status for the selected interrupt types
    @param[out]    intrStatus    Get UART interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   UART driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWARCH_UART_GET_INTERRUPT_STATUS_PROC
    @trace  #BRCM_SWREQ_UART_INTERRUPT

    @limitations None
*/
int UART_GetInterruptStatus(UART_HwIDType aId, UART_InterruptType intrType, uint32_t *intrStatus)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_INDIVIDUAL_INTR
    uint32_t errorIntrMask = (UART_UARTMIS_FEMIS_MASK | UART_UARTMIS_PEMIS_MASK | UART_UARTMIS_BEMIS_MASK | UART_UARTMIS_OEMIS_MASK);
    uint32_t status = UART_Regs[aId]->uartmis;
#endif
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= UART_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
#endif
        *intrStatus = 0;
        if(intrType == UART_INTERRUPT_ALL) {
            UART_GetAllInterruptStatus(aId, intrStatus);
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else {
            if(intrType == UART_INTERRUPT_TX_INTR) {
                *intrStatus |= (((status & UART_UARTMIS_TXMIS_MASK) > 0UL) ? UART_INTERRUPT_TX_INTR : 0UL);
            }
            if(intrType == UART_INTERRUPT_RX_INTR) {
                *intrStatus |= (((status & UART_UARTMIS_RXMIS_MASK) > 0UL) ? UART_INTERRUPT_RX_INTR : 0UL);
            }
            if(intrType == UART_INTERRUPT_RX_TIMEOUT_INTR) {
                *intrStatus |= (((status & UART_UARTMIS_RTMIS_MASK) > 0UL) ? UART_INTERRUPT_RX_TIMEOUT_INTR : 0UL);
            }
            if(intrType == UART_INTERRUPT_ERROR_INTR) {
                *intrStatus |= (((status & errorIntrMask) > 0UL) ? UART_INTERRUPT_ERROR_INTR : 0UL);
            }
        }
#endif
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Request transmit DMA Enable

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      enable      Enable Transmit DMA request

    @return     #BCM_ERR_OK             Success
                #BCM_ERR_INVAL_STATE    UART is not initialized
                #BCM_ERR_INVAL_PARAMS   aId is invalid

    @trace  #BRCM_SWREQ_UART_COMMUNICATION
    @trace  #BRCM_SWARCH_UART_DRV_DMA_REQ_SEND_ENABLE_PROC
    @limitations None
*/
int32_t UART_DrvDMAReqSendEnable(UART_HwIDType aId, uint32_t enable)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= UART_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
#endif
    if(enable > 0UL) {
        UART_Regs[aId]->uartdmacr |= UART_UARTDMACR_TXDMAE_MASK;
    } else {
        UART_Regs[aId]->uartdmacr &= ~(UART_UARTDMACR_TXDMAE_MASK);
    }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}

/** @brief Request receive DMA Enable

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      enable      Enable receive DMA request

    @return     #BCM_ERR_OK             Success
                #BCM_ERR_INVAL_STATE    UART is not initialized
                #BCM_ERR_INVAL_PARAMS   aId is invalid

    @trace  #BRCM_SWREQ_UART_COMMUNICATION
    @trace  #BRCM_SWARCH_UART_DRV_DMA_REQ_RECEIVE_ENABLE_PROC
    @limitations None
*/
int32_t UART_DrvDMAReqReceiveEnable(UART_HwIDType aId, uint32_t enable)
{
    int32_t retVal = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= UART_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
#endif
    if(enable > 0UL) {
        UART_Regs[aId]->uartdmacr |= UART_UARTDMACR_RXDMAE_MASK;
    } else {
        UART_Regs[aId]->uartdmacr &= ~(UART_UARTDMACR_RXDMAE_MASK);
    }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    return retVal;
}


/** @} */

