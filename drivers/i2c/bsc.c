/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_i2cdrv_bsc_impl IIC Driver Design
    @ingroup grp_i2cdrv

    @addtogroup grp_i2cdrv_bsc_impl
    @{
*/

/* Header files */
#include <stdint.h>
#include <string.h>
#include <compiler.h>
#include <bcm_time.h>
#include <bcm_err.h>
#include <cfg_rdb.h>
#include <i2c.h>
#include <bcm_utils.h>
#include <lw_queue.h>
#include "iic_rdb.h"

/*static const BCM_CompIDType BCM_LogCompID = BCM_IIC_ID;
static const uint16_t BCM_LogFileID = 0xB2U;*/

/**
    @name BSC driver Design IDs
    @{
    @brief Design IDs for BSC drive
*/
#define BRCM_SWDSGN_IIC_MAX_RETRIES_MACRO                (0xB201U)    /**< @brief #IIC_SLAVE_ADDR_RETRIES    */
#define BRCM_SWDSGN_IIC_MSG_TYPE                         (0xB202U)    /**< @brief #IIC_MsgType               */
#define BRCM_SWDSGN_IIC_MSG_GLOBAL                       (0xB203U)    /**< @brief #IIC_Msg                   */
#define BRCM_SWDSGN_IIC_BSC_SET_BIT_PROC                 (0xB204U)    /**< @brief #IIC_BscSetBit */
#define BRCM_SWDSGN_IIC_BSC_CLR_NGET_BIT_PROC            (0xB205U)    /**< @brief #IIC_BscClrNGetBit */
#define BRCM_SWDSGN_IIC_BSCRWDEV_TYPE                    (0xB206U)    /**< @brief #IIC_BscRWDevType          */
#define BRCM_SWDSGN_IIC_BSCREGS_GLOBAL                   (0xB207U)    /**< @brief #IIC_BSC_REGS              */
#define BRCM_SWDSGN_IIC_BSCRDWRDEV_GLOBAL                (0xB208U)    /**< @brief #IIC_BscRWDev              */
#define BRCM_SWDSGN_IIC_BSC_SENDCMD_PROC                 (0xB20AU)    /**< @brief #IIC_BscSendCmd            */
#define BRCM_SWDSGN_IIC_BSC_SLAVE2ADDR_PROC              (0xB20BU)    /**< @brief #IIC_SlaveAddr2Bytes       */
#define BRCM_SWDSGN_IIC_BSC_WRITE_BYTE_PROC              (0xB20CU)    /**< @brief #IIC_BscWriteByte          */
#define BRCM_SWDSGN_IIC_BSC_READ_CMD_PROC                (0xB20DU)    /**< @brief #IIC_BscReadCmd            */
#define BRCM_SWDSGN_IIC_BSC_REA_BYTE_PROC                (0xB20EU)    /**< @brief #IIC_BscReadByte           */
#define BRCM_SWDSGN_IIC_BSC_STATE_HANDLER_PROC           (0xB20FU)    /**< @brief #IIC_XferStateHandler      */
#define BRCM_SWDSGN_IIC_BSC_START_TRANSFER_PROC          (0xB210U)    /**< @brief #IIC_BscStartTransfer      */
#define BRCM_SWDSGN_IIC_INIT_PROC                        (0xB211U)    /**< @brief #IIC_Init               */
#define BRCM_SWDSGN_IIC_TRANSGER_PROC                    (0xB212U)    /**< @brief #IIC_Transfer           */
#define BRCM_SWDSGN_IIC_GETSTATUS_PROC                   (0xB213U)    /**< @brief #IIC_GetStatus          */
#define BRCM_SWDSGN_IIC_DEINIT_PROC                      (0xB214U)    /**< @brief #IIC_DeInit             */
#define BRCM_SWDSGN_IIC_IRQHANDLER_PROC                  (0xB215U)    /**< @brief #IIC_IRQHandler            */

#define BRCM_SWDSGN_IIC_BSC_SEND_RESTART_PROC            (0xB218U)    /**< @brief #IIC_BscSendRestartCmd     */
#define BRCM_SWDSGN_IIC_BSC_SEND_SLVADDR_0_PROC          (0xB219U)    /**< @brief #IIC_BscSendSlaveAddrByte0 */
#define BRCM_SWDSGN_IIC_BSC_SEND_SLVADDR_1_PROC          (0xB21AU)    /**< @brief #IIC_BscSendSlaveAddrByte1 */
#define BRCM_SWDSGN_IIC_ENABLE_INTERRUPT_PROC            (0xB21BU)    /**< @brief #IIC_EnableInterrupt */
#define BRCM_SWDSGN_IIC_CLEAR_INTERRUPT_PROC             (0xB21CU)    /**< @brief #IIC_ClearInterrupt  */
#define BRCM_SWDSGN_IIC_GET_INTERRUPT_STATUS_PROC        (0xB21DU)    /**< @brief #IIC_GetInterruptStatus  */
#define BRCM_SWDSGN_IIC_CFG_REGS_GLOBAL                  (0xB21EU)    /**< @brief #IIC_CFG_Regs */
#define BRCM_SWDSGN_IIC_INT_POWERUP_OUT_RESET_PROC       (0xB21FU)    /**< @brief #IIC_IntPowerUpOutReset */
#define BRCM_SWDSGN_IIC_INT_INRESET_PROC                 (0xB220U)    /**< @brief #IIC_IntInReset */
#define BRCM_SWDSGN_IIC_GET_ALL_INTERRUPT_STATUS_PROC    (0xB221U)    /**< @brief #IIC_GetAllInterruptStatus */
#define BRCM_SWDSGN_IIC_INT_WAIT_UNTIL_DONE_PROC         (0xB222U)    /**< @brief #IIC_IntWaitUntilDone */


/** @} */

/** @brief Number of retries for slave address

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
 */
#define IIC_SLAVE_ADDR_RETRIES                  (3UL)

/**
    @brief IIC Message type

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT

 */
typedef struct sIIC_MsgType {
    int32_t                 retVal;     /**< @brief Return value            */
    int32_t                 clientID;
    uint32_t                clientMask;
    uint32_t                numPkts;    /**< @brief Number of IIC packets   */
    IIC_PktType*            pkts;       /**< @brief Array of IIC packets    */
} IIC_MsgType;

/**
    @brief IIC private global data structure to store I2C command and message.

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static IIC_MsgType IIC_Msg[IIC_MAX_HW_ID][LWQ_BUFFER_MAX_LENGTH];

/**
    @brief Peripheral global configuration register.
    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static CFG_RDBType * const IIC_CFG_Regs = (CFG_RDBType *)CFG_BASE;

/**
    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
typedef struct sIIC_BscRWDevType {
    IIC_BscStateType    state;
    IIC_XferStateType   xferState;
    IIC_PktType         *pkts;
    uint32_t            numPkts;
    uint8_t             slaveAddr[2UL];
    uint32_t            retryCnt;
    uint32_t            pktIdx;
    uint32_t            bufIdx;
    uint32_t            txFifoMode;
    volatile uint32_t   allocStatus;
    LWQ_BufferType      procQ;
} IIC_BscRWDevType;

/**
    @brief Global variable containing base address for I2C slave device

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static IIC_RDBType *const IIC_BSC_REGS[IIC_MAX_HW_ID] =
{
#if IIC_MAX_HW_ID >= 1UL
    (IIC_RDBType *)I2C0_BASE,
#endif
#if IIC_MAX_HW_ID >= 2UL
    (IIC_RDBType *)I2C1_BASE,
#endif
#if IIC_MAX_HW_ID > 2UL
#error "IIC_MAX_HW_ID > 2"
#endif
};

/**
    @brief Global variable for I2C slave device handler

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static IIC_BscRWDevType IIC_BscRWDev[IIC_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @brief Set a bit

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT

*/
static inline void IIC_BscSetBit(volatile uint32_t * const aAddr, uint32_t aBitNo)
{
    *aAddr = *aAddr | (0x1UL << aBitNo);
}

/**
    @brief Clears a bit and returns index

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT

*/
static inline uint32_t IIC_BscClrNGetBit(volatile uint32_t * const aAddr)
{
    uint32_t idx = 0xFFFFFFFFUL;
    uint32_t val = *aAddr;

    if (0UL != val) {
        idx = CPU_BitReverse(val);
        idx = CPU_Clz(idx);
        val &= ~(0x1UL << idx);
        *aAddr = val;
    }
    return idx;
}

#ifdef BCM8915X_I2C_POLLING
/**
    @brief Wait for command completion.
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
static int IIC_IntWaitUntilDone(IIC_HwIDType aId)
{
    int retVal = BCM_ERR_OK;
    int timeout_counter = 1000;
    uint32_t sbusy = 0;
    uint32_t status;
    status = IIC_BSC_REGS[aId]->bscisr;
    if ((status & IIC_BSCISR_NOACK_MASK) != 0UL) {
            retVal = BCM_ERR_NACK;
    } else {

        do {
              status = IIC_BSC_REGS[aId]->bscisr;
              if ((status & IIC_BSCISR_NOACK_MASK) != 0UL) {
                 retVal = BCM_ERR_NACK;
                 break;
              }
              sbusy = (status & IIC_BSCISR_BSC_SES_DONE_MASK);
              if(sbusy > 0UL) {
                break;
              }
              timeout_counter--;
              BCM_CpuNDelay(100UL);
        } while((sbusy == 0UL) && (timeout_counter > 0));

        if((timeout_counter == 0) && (sbusy == 0UL)) {
            retVal = BCM_ERR_TIME_OUT;
        } else {
           IIC_BSC_REGS[aId]->bscisr = status;
           IIC_BSC_REGS[aId]->bsccs &= ~(IIC_BSCCS_CMD_MASK | IIC_BSCCS_ACK_MASK);
        }
    }

    return retVal;
}
#endif

/**
    @brief Send a command to the slave device

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aCmd         Command to be given to bsc

    @return void

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    IIC_BSC_REGS[aID]->bscier = 0UL

    reg =  IIC_BSC_REGS[aID]->bsccs
    reg &= ~(IIC_BSCCS_CMD_MASK (BIT_OR) IIC_BSCCS_ACK_MASK)
    reg = reg (BIT_OR) aCmd
    IIC_BSC_REGS[aID]->bscier = IIC_BSCIER_BSC_INT_EN_MASK
    IIC_BSC_REGS[aID]->bsccs = reg
    @endcode
*/
static int32_t IIC_BscSendCmd(IIC_HwIDType aID, uint32_t aCmd)
{
    int ret = BCM_ERR_BUSY;
    uint32_t reg;

    /* Disable BSC Interrupt */
    IIC_BSC_REGS[aID]->bscier = 0UL;

    /* configure BSC command and enable interrupt */
    reg =  IIC_BSC_REGS[aID]->bsccs;
    reg &= ~(IIC_BSCCS_CMD_MASK | IIC_BSCCS_ACK_MASK);
    reg |= aCmd;
    IIC_BSC_REGS[aID]->bscier = IIC_BSCIER_BSC_INT_EN_MASK;
    IIC_BSC_REGS[aID]->bsccs = reg;
#ifdef BCM8915X_I2C_POLLING
    ret = IIC_IntWaitUntilDone(aID);
    if(ret == BCM_ERR_OK) {
        ret = BCM_ERR_BUSY;
    }
#endif
    return ret;
}

/**
    @brief Send the 2-byte address of the slave device

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aPkt         I2C packet pointer

    @return void

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    if (aPkt->flags & IIC_PKT_FLG_SLAVE_10BIT) == 0UL
        if (aPkt->flags & IIC_PKT_FLG_OP_RD) == 0UL)
            slaveAddr = aPkt->slaveAddr << 1UL
        else
            slaveAddr = (aPkt->slaveAddr << 1UL) (BIT_OR) 0x1UL
        IIC_BscRWDev[aID].slaveAddr[0] = (uint8_t)slaveAddr
    else
        if (aPkt->flags & IIC_PKT_FLG_OP_RD) == 0UL)
            slaveAddr = (((aPkt->slaveAddr >> 7UL) & 0x6UL) (BIT_OR) 0xF0UL)
        else
            slaveAddr = (((aPkt->slaveAddr >> 7UL) & 0x6UL) (BIT_OR) 0xF1UL)
        IIC_BscRWDev[aID].slaveAddr[0] = (uint8_t)slaveAddr
        IIC_BscRWDev[aID].slaveAddr[1] = (uint8_t)(aPkt->slaveAddr & 0xFFUL)
    @endcode
*/
static void IIC_SlaveAddr2Bytes(IIC_HwIDType aID, IIC_PktType *const aPkt)
{
    uint32_t slaveAddr;
    if ((aPkt->flags & IIC_PKT_FLG_SLAVE_10BIT) == 0UL) {
        if ((aPkt->flags & IIC_PKT_FLG_OP_RD) == 0UL) {
            slaveAddr = aPkt->slaveAddr << 1UL;
        } else {
            slaveAddr = (aPkt->slaveAddr << 1UL) | 0x1UL;
        }
        IIC_BscRWDev[aID].slaveAddr[0] = (uint8_t)slaveAddr;
    } else {
        if ((aPkt->flags & IIC_PKT_FLG_OP_RD) == 0UL) {
            slaveAddr = (((aPkt->slaveAddr >> 7UL) & 0x6UL) | 0xF0UL);
        } else {
            slaveAddr = (((aPkt->slaveAddr >> 7UL) & 0x6UL) | 0xF1UL);
        }
        IIC_BscRWDev[aID].slaveAddr[0] = (uint8_t)slaveAddr;
        IIC_BscRWDev[aID].slaveAddr[1] = (uint8_t)(aPkt->slaveAddr & 0xFFUL);
    }
}

/**
    @brief Send Restart Command

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aNextState   Pointer to state variable

    @retval  #BCM_ERR_BUSY  In progress

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    *stateInfo = &IIC_BscRWDev[aID]
    *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx]

    stateInfo->retryCnt = 0UL
    stateInfo->bufIdx = 0UL
    if (pkt->flags & IIC_PKT_FLG_RESTART) != 0UL
        IIC_BscSendCmd(aID, IIC_BSCCS_CMD_START (BIT_OR) BSC_ACK_GEN_REPEATED_START)
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE0
    return BCM_ERR_BUSY
    @endcode
*/
static int32_t IIC_BscSendRestartCmd(IIC_HwIDType aID,
                                     IIC_XferStateType *aNextState)
{
    int32_t ret = BCM_ERR_BUSY;
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID];
    IIC_PktType *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];

    stateInfo->retryCnt = 0UL;
    stateInfo->bufIdx = 0UL;
    if ((pkt->flags & IIC_PKT_FLG_RESTART) != 0UL) {
        ret = IIC_BscSendCmd(aID, IIC_BSCCS_CMD_START | BSC_ACK_GEN_REPEATED_START);
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE0;
    }
    return ret;
}

/**
    @brief Send Slave Addr Byte-0

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aNextState   Pointer to state variable

    @retval  #BCM_ERR_BUSY  In progress

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    *stateInfo = &IIC_BscRWDev[aID]
    *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx]

    IIC_SlaveAddr2Bytes(aID, pkt)
    IIC_BSC_REGS[aID]->bscdat = stateInfo->slaveAddr[0UL]
    if (pkt->flags & IIC_PKT_FLG_SLAVE_10BIT) != 0UL
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE1
    else if (pkt->flags & IIC_PKT_FLG_OP_RD) != 0UL
        *aNextState = IIC_XFER_STATE_READ_CMD
    else
        *aNextState = IIC_XFER_STATE_WRITE_BYTE

    return BCM_ERR_BUSY
    @endcode
*/
static int32_t IIC_BscSendSlaveAddrByte0(IIC_HwIDType aID,
                                         IIC_XferStateType *aNextState)
{
    int32_t ret = BCM_ERR_BUSY;
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID];
    IIC_PktType *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];

    IIC_SlaveAddr2Bytes(aID, pkt);
    IIC_BSC_REGS[aID]->bscdat = stateInfo->slaveAddr[0UL];
    if ((pkt->flags & IIC_PKT_FLG_SLAVE_10BIT) != 0UL) {
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE1;
    } else if ((pkt->flags & IIC_PKT_FLG_OP_RD) != 0UL) {
        *aNextState = IIC_XFER_STATE_READ_CMD;
    } else {
        *aNextState = IIC_XFER_STATE_WRITE_BYTE;
    }

#ifdef BCM8915X_I2C_POLLING
    ret = IIC_IntWaitUntilDone(aID);
    if(ret == BCM_ERR_OK) {
        ret = BCM_ERR_BUSY;
    }
#endif
    return ret;
}

/**
    @brief Send Slave Addr Byte-1

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aError       Error flag
    @param[in] aNextState   Pointer to state variable

    @retval  #BCM_ERR_BUSY          In progress.
    @retval  #BCM_ERR_EAGAIN        Call state machine again.
    @retval  #BCM_ERR_MAX_ATTEMPS   Max retry error.

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    *stateInfo = &IIC_BscRWDev[aID]
    *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx]

    if (BCM_ERR_OK == aError)
        IIC_BSC_REGS[aID]->bscdat = stateInfo->slaveAddr[1UL]
        if (pkt->flags & IIC_PKT_FLG_OP_RD) != 0UL)
            *aNextState = IIC_XFER_STATE_READ_CMD
        else
            *aNextState = IIC_XFER_STATE_WRITE_BYTE
        ret = BCM_ERR_BUSY
    else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES)
        ret = BCM_ERR_EAGAIN
        stateInfo->retryCnt++
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE0
    else
        ret = BCM_ERR_MAX_ATTEMPS

    return ret
    @endcode
*/
static int32_t IIC_BscSendSlaveAddrByte1(IIC_HwIDType aID,
                                         int32_t aError,
                                         IIC_XferStateType *aNextState)
{
    int32_t ret = BCM_ERR_OK;
    int32_t ret1 =  BCM_ERR_OK;
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID];
    IIC_PktType *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];

    if (BCM_ERR_OK == aError) {
        IIC_BSC_REGS[aID]->bscdat = stateInfo->slaveAddr[1UL];
        if ((pkt->flags & IIC_PKT_FLG_OP_RD) != 0UL) {
            *aNextState = IIC_XFER_STATE_READ_CMD;
        } else {
            *aNextState = IIC_XFER_STATE_WRITE_BYTE;
        }
        ret = BCM_ERR_BUSY;
    } else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES) {
        ret = BCM_ERR_EAGAIN;
        stateInfo->retryCnt++;
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE0;
    } else {
        ret = BCM_ERR_MAX_ATTEMPS;
    }
#ifdef BCM8915X_I2C_POLLING
    ret1 = IIC_IntWaitUntilDone(aID);
    if(ret1 == BCM_ERR_OK) {
        ret1 = ret;
    }
#else
    ret1 = ret;
#endif
    return ret1;
}

/**
    @brief Write Byte

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aError       Error flag
    @param[in] aNextState   Pointer to state variable

    @retval  #BCM_ERR_BUSY          In progress.
    @retval  #BCM_ERR_EAGAIN        Call state machine again.
    @retval  #BCM_ERR_MAX_ATTEMPS   Max retry error.

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID]

    if (BCM_ERR_OK == aError)
        IIC_PktType *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx]

        stateInfo->retryCnt = IIC_SLAVE_ADDR_RETRIES
        IIC_BSC_REGS[aID]->bscdat = pkt->data[stateInfo->bufIdx++]
        if (stateInfo->bufIdx == pkt->length)
            stateInfo->pktIdx++
            stateInfo->bufIdx = 0UL
            if (stateInfo->pktIdx == stateInfo->numPkts)
                *aNextState = IIC_XFER_STATE_STOP_CMD
            else
                *aNextState = IIC_XFER_STATE_RESTART_CMD
        else
            *aNextState = IIC_XFER_STATE_WRITE_BYTE

        ret = BCM_ERR_BUSY
    else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES)
        ret = BCM_ERR_EAGAIN
        stateInfo->retryCnt++
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE0
    else
        ret = BCM_ERR_MAX_ATTEMPS

    return ret
    @endcode
*/
static int32_t IIC_BscWriteByte(IIC_HwIDType aID,
                                int32_t aError,
                                IIC_XferStateType *aNextState)
{
    int32_t ret = BCM_ERR_OK;
    int32_t ret1 =  BCM_ERR_OK;
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID];

    if (BCM_ERR_OK == aError) {
        IIC_PktType *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];

        stateInfo->retryCnt = IIC_SLAVE_ADDR_RETRIES;
        IIC_BSC_REGS[aID]->bscdat = pkt->data[stateInfo->bufIdx++];
        if (stateInfo->bufIdx == pkt->length) {
            stateInfo->pktIdx++;
            stateInfo->bufIdx = 0UL;
            if (stateInfo->pktIdx == stateInfo->numPkts) {
                *aNextState = IIC_XFER_STATE_STOP_CMD;
            } else {
                *aNextState = IIC_XFER_STATE_RESTART_CMD;
            }
        } else {
            *aNextState = IIC_XFER_STATE_WRITE_BYTE;
        }
        ret = BCM_ERR_BUSY;
    } else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES) {
        ret = BCM_ERR_EAGAIN;
        stateInfo->retryCnt++;
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE0;
    } else {
        ret = BCM_ERR_MAX_ATTEMPS;
    }
#ifdef BCM8915X_I2C_POLLING
    ret1 = IIC_IntWaitUntilDone(aID);
    if(ret1 == BCM_ERR_OK) {
        ret1 = ret;
    }
#else
    ret1 = ret;
#endif
    return ret1;
}

/**
    @brief Read Command

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aError       Error flag
    @param[in] aNextState   Pointer to state variable

    @retval  #BCM_ERR_BUSY          In progress.
    @retval  #BCM_ERR_EAGAIN        Call state machine again.
    @retval  #BCM_ERR_MAX_ATTEMPS   Max retry error.

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID]

    if (BCM_ERR_OK == aError)
        IIC_PktType *pkt = NULL

        stateInfo->retryCnt = IIC_SLAVE_ADDR_RETRIES
        pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx]
        if stateInfo->bufIdx == (pkt->length - 1UL)
            IIC_BscSendCmd(aID, IIC_BSCCS_CMD_READ_BYTE (BIT_OR) BSC_ACK_NACK_AFTER_READ)
        else
            IIC_BscSendCmd(aID, IIC_BSCCS_CMD_READ_BYTE (BIT_OR) BSC_ACK_ACK_AFTER_READ)

        ret = BCM_ERR_BUSY
        *aNextState = IIC_XFER_STATE_READ_BYTE
    else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES)
        ret = BCM_ERR_EAGAIN
        stateInfo->retryCnt++
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE0
    else
         ret = BCM_ERR_MAX_ATTEMPS

    return ret
    @endcode

*/
static int32_t IIC_BscReadCmd(IIC_HwIDType aID,
                              int32_t aError,
                              IIC_XferStateType *aNextState)
{
    int32_t ret;
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID];

    if (BCM_ERR_OK == aError) {
        IIC_PktType *pkt = NULL;

        stateInfo->retryCnt = IIC_SLAVE_ADDR_RETRIES;
        pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];
        if (stateInfo->bufIdx == (pkt->length - 1UL)) {
            ret = IIC_BscSendCmd(aID, IIC_BSCCS_CMD_READ_BYTE | BSC_ACK_NACK_AFTER_READ);
        } else {
            ret = IIC_BscSendCmd(aID, IIC_BSCCS_CMD_READ_BYTE | BSC_ACK_ACK_AFTER_READ);
        }
        /* ret = BCM_ERR_BUSY;*/
        *aNextState = IIC_XFER_STATE_READ_BYTE;
    } else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES) {
        ret = BCM_ERR_EAGAIN;
        stateInfo->retryCnt++;
        *aNextState = IIC_XFER_STATE_SLVADDR_BYTE0;
    } else {
         ret = BCM_ERR_MAX_ATTEMPS;
    }

    return ret;
}

/**
    @brief Read Byte

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aError       Error flag
    @param[in] aNextState   Pointer to state variable

    @retval  #BCM_ERR_EAGAIN        Call state machine again.
    @retval  #BCM_ERR_NACK          I2C Bus NACK
    @retval  #BCM_ERR_BUS_FAILURE   I2C Bus failure

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    if (BCM_ERR_OK == aError)
        IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID]
        IIC_PktType *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx]

        pkt->data[stateInfo->bufIdx++] =
            (uint8_t)(IIC_BSC_REGS[aID]->bscdat & IIC_BSCDAT_DAT_MASK)
        if (stateInfo->bufIdx == pkt->length)
            stateInfo->pktIdx++
            stateInfo->bufIdx = 0UL
            if (stateInfo->pktIdx == stateInfo->numPkts)
                *aNextState = IIC_XFER_STATE_STOP_CMD
            else
                *aNextState = IIC_XFER_STATE_RESTART_CMD
        else
            *aNextState = IIC_XFER_STATE_READ_CMD
        ret = BCM_ERR_EAGAIN
    else
        ret = aError

    return ret
    @endcode
*/
static int32_t IIC_BscReadByte(IIC_HwIDType aID,
                               int32_t aError,
                               IIC_XferStateType *aNextState)
{
    int32_t ret;

    if (BCM_ERR_OK == aError) {
        IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID];
        IIC_PktType *pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];

        pkt->data[stateInfo->bufIdx++] =
            (uint8_t)(IIC_BSC_REGS[aID]->bscdat & IIC_BSCDAT_DAT_MASK);
        if (stateInfo->bufIdx == pkt->length) {
            stateInfo->pktIdx++;
            stateInfo->bufIdx = 0UL;
            if (stateInfo->pktIdx == stateInfo->numPkts) {
                *aNextState = IIC_XFER_STATE_STOP_CMD;
            } else {
                *aNextState = IIC_XFER_STATE_RESTART_CMD;
            }
        } else {
            *aNextState = IIC_XFER_STATE_READ_CMD;
        }
        ret = BCM_ERR_EAGAIN;
    } else {
        ret = aError;
    }
    return ret;
}

/**
    @brief Transfer state handler

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aError       Error flag

    @retval  #BCM_ERR_OK        State machine is completed transfer
    @retval  #BCM_ERR_BUSY      Transfer in progress

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID]

    do
        ret = BCM_ERR_BUSY
        bsccsReg = IIC_BSC_REGS[aID]->bsccs
        state = IIC_BscRWDev[aID].xferState
        nextState = state

        if (bsccsReg & IIC_BSCCS_BUSY_MASK) == 0UL)
            switch (state)
            case IIC_XFER_STATE_START_CMD:
                stateInfo->retryCnt = 0UL
                stateInfo->pktIdx = 0UL
                stateInfo->bufIdx = 0UL
                IIC_BscSendCmd(aID, IIC_BSCCS_CMD_START)
                ret = BCM_ERR_BUSY
                nextState = IIC_XFER_STATE_SLVADDR_BYTE0
                break
            case IIC_XFER_STATE_RESTART_CMD:
                ret = IIC_BscSendRestartCmd(aID, &nextState)
                break
            case IIC_XFER_STATE_SLVADDR_BYTE0:
                ret = IIC_BscSendSlaveAddrByte0(aID, &nextState)
                break
            case IIC_XFER_STATE_SLVADDR_BYTE1:
                ret = IIC_BscSendSlaveAddrByte1(aID, aError, &nextState)
                break
            case IIC_XFER_STATE_WRITE_BYTE:
                ret = IIC_BscWriteByte(aID, aError, &nextState)
                break
            case IIC_XFER_STATE_READ_CMD:
                ret = IIC_BscReadCmd(aID, aError, &nextState)
                break
            case IIC_XFER_STATE_READ_BYTE:
                ret = IIC_BscReadByte(aID, aError, &nextState)
                break
            case IIC_XFER_STATE_STOP_CMD:
                IIC_BscSendCmd(aID, IIC_BSCCS_CMD_STOP)
                ret = BCM_ERR_BUSY
                nextState = IIC_XFER_STATE_END
                break
            case IIC_XFER_STATE_END:
                ret = BCM_ERR_OK
                break
            default:
                ret = BCM_ERR_INVAL_PARAMS
                break
            }
        }

        if (BCM_ERR_BUSY != ret) && (BCM_ERR_EAGAIN != ret)
            stateInfo->xferState = IIC_XFER_STATE_START_CMD
            stateInfo->pktIdx = 0UL
            stateInfo->bufIdx = 0UL
        else
            stateInfo->xferState = nextState

        aError = BCM_ERR_OK
    while (BCM_ERR_EAGAIN == ret)

    return ret
    @endcode
*/
static int32_t IIC_XferStateHandler(IIC_HwIDType aID, int32_t aError)
{
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID];
    IIC_XferStateType state;
    IIC_XferStateType nextState;
    int32_t ret;
    IIC_BSCCS_TYPE bsccsReg;
    #ifdef BCM8915X_I2C_POLLING
    uint8_t jobID = 0U;
    uint32_t status;
    uint32_t reg;
    #endif
    do {
        ret = BCM_ERR_BUSY;
        bsccsReg = IIC_BSC_REGS[aID]->bsccs;
        state = IIC_BscRWDev[aID].xferState;
        nextState = state;
        #ifdef BCM8915X_I2C_POLLING
        (void)LWQ_Pop(&IIC_BscRWDev[aID].procQ, &jobID);
        #endif
        if ((bsccsReg & IIC_BSCCS_BUSY_MASK) == 0UL) {
            switch (state) {
            case IIC_XFER_STATE_START_CMD:
                stateInfo->retryCnt = 0UL;
                stateInfo->pktIdx = 0UL;
                stateInfo->bufIdx = 0UL;
                ret = IIC_BscSendCmd(aID, IIC_BSCCS_CMD_START);
                #ifdef BCM8915X_I2C_POLLING
                /* Generate no immediate action */
                if(ret == BCM_ERR_BUSY) {
                    reg =  IIC_BSC_REGS[aID]->bsccs;
                    reg &= ~(IIC_BSCCS_CMD_MASK | IIC_BSCCS_ACK_MASK);
                    IIC_BSC_REGS[aID]->bsccs = reg;
                    ret = IIC_IntWaitUntilDone(aID);
                }
                #endif
                ret = BCM_ERR_BUSY;
                nextState = IIC_XFER_STATE_SLVADDR_BYTE0;
                break;
            case IIC_XFER_STATE_RESTART_CMD:
                ret = IIC_BscSendRestartCmd(aID, &nextState);
                break;
            case IIC_XFER_STATE_SLVADDR_BYTE0:
                ret = IIC_BscSendSlaveAddrByte0(aID, &nextState);
                break;
            case IIC_XFER_STATE_SLVADDR_BYTE1:
                ret = IIC_BscSendSlaveAddrByte1(aID, aError, &nextState);
                break;
            case IIC_XFER_STATE_WRITE_BYTE:
                ret = IIC_BscWriteByte(aID, aError, &nextState);
                break;
            case IIC_XFER_STATE_READ_CMD:
                ret = IIC_BscReadCmd(aID, aError, &nextState);
                break;
            case IIC_XFER_STATE_READ_BYTE:
                ret = IIC_BscReadByte(aID, aError, &nextState);
                break;
            case IIC_XFER_STATE_STOP_CMD:
                ret = IIC_BscSendCmd(aID, IIC_BSCCS_CMD_STOP);
                #ifndef BCM8915X_I2C_POLLING
                ret = BCM_ERR_BUSY;
                #endif
                nextState = IIC_XFER_STATE_END;
                break;
            case IIC_XFER_STATE_END:
                ret = BCM_ERR_OK;
                #ifdef BCM8915X_I2C_POLLING
                IIC_Msg[aID][jobID].retVal = ret;
                #endif
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
        }

        if ((BCM_ERR_BUSY != ret) && (BCM_ERR_EAGAIN != ret)) {
            stateInfo->xferState = IIC_XFER_STATE_START_CMD;
            stateInfo->pktIdx = 0UL;
            stateInfo->bufIdx = 0UL;
        } else {
            stateInfo->xferState = nextState;
        }

        aError = BCM_ERR_OK;
        #ifdef BCM8915X_I2C_POLLING
        status = IIC_BSC_REGS[aID]->bscisr;
        /* if NACK return error */
        if ((status & IIC_BSCISR_NOACK_MASK) != 0UL) {
            ret = BCM_ERR_NACK;
            break;
        }
        #endif
    } while (BCM_ERR_EAGAIN == ret);

    return ret;
}

/**
    @brief Initiate the transfer for a packet

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID

    @return void

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    if (IIC_XFER_STATE_START_CMD == IIC_BscRWDev[aID].xferState)
        && (LWQ_Length(IIC_BscRWDev[aID].procQ) > 0UL)
        uint8_t jobID = LWQ_PeekFirst(IIC_BscRWDev[aID].procQ)

        IIC_BscRWDev[aID].pkts = IIC_Msg[aID][jobID].pkts
        IIC_BscRWDev[aID].numPkts = IIC_Msg[aID][jobID].numPkts
        ret = IIC_XferStateHandler(aID, BCM_ERR_OK)
        if (BCM_ERR_BUSY != ret)
            Report error
    @endcode
*/
static int32_t IIC_BscStartTransfer(IIC_HwIDType aID)
{
    int32_t ret = BCM_ERR_OK;
    #ifdef BCM8915X_I2C_POLLING
    IIC_XferStateType prev_state, next_state;
    #endif
    if ((IIC_XFER_STATE_START_CMD == IIC_BscRWDev[aID].xferState)
        && (LWQ_Length(IIC_BscRWDev[aID].procQ) > 0UL)){
        uint8_t jobID = LWQ_PeekFirst(IIC_BscRWDev[aID].procQ);
        /* Set i2c State to Busy */
        if(jobID >= 0xFU) {
            jobID = 0xEU;
        }
        IIC_BscRWDev[aID].pkts = IIC_Msg[aID][jobID].pkts;
        IIC_BscRWDev[aID].numPkts = IIC_Msg[aID][jobID].numPkts;
        /* Trigger the transfer state machine */
     #ifdef BCM8915X_I2C_POLLING
        do {
              prev_state = IIC_BscRWDev[aID].xferState;
              ret = IIC_XferStateHandler(aID, BCM_ERR_OK);
              next_state = IIC_BscRWDev[aID].xferState;
              if((prev_state != IIC_XFER_STATE_WRITE_BYTE) && (prev_state != IIC_XFER_STATE_READ_BYTE)) {
                  if(prev_state == next_state) {
                    break;
                  }
              }

        } while(ret == BCM_ERR_BUSY);
    #else
        ret = IIC_XferStateHandler(aID, BCM_ERR_OK);
    #endif

        if (BCM_ERR_BUSY != ret) {
            /*BCM_LOG_DEBUG(0U, BCM_STATE_RUN, ret, "Failed to start I2C BSC transfer.\n");*/
        }
    }
    return ret;
}

/** @brief  Power up and bring out of reset I2C

    param[in]   aId  Index of the I2C port

    return      void
    @trace #BRCM_SWREQ_IIC_INIT
    @trace #BRCM_SWARCH_IIC_INIT_PROC
 */
static void IIC_IntPowerUpOutReset(IIC_HwIDType aId)
{
    /* Bring peripheral out of reset */
    IIC_CFG_Regs->periph_reset |= (1UL << (CFG_PERIPH_RESET_I2C_0_SHIFT + aId));
}

/** @brief Bring I2C in reset

    param[in]   aId  Index of the I2C port

    return      void
    @trace #BRCM_SWREQ_IIC_INIT
    @trace #BRCM_SWARCH_IIC_INIT_PROC
 */
static void IIC_IntInReset(IIC_HwIDType aId)
{
    /* Bring peripheral in reset */
    IIC_CFG_Regs->periph_reset &= (uint32_t)(~(1UL << (CFG_PERIPH_RESET_I2C_0_SHIFT + aId)));
}

/**
    @brief Initialiaze the IIC BSC device

    @behavior Sync, Non-reentrant

    @pre I2C should be in de-initialized state

    @param[in] aID          I2C HW ID
    @param[in] aConfig      I2C Configurations.

    @retval  #BCM_ERR_OK             Init is succesfull
    @retval  #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid HW aID
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid Speed
    @retval  #BCM_ERR_INVAL_STATE    I2C is in Init state
    @retval  #BCM_ERR_NOSUPPORT      TX FIFO mode is enabled
    @retval  #BCM_ERR_UNKNOWN        Switch default case. Should never occur.

    @post None

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWREQ_IIC_INIT

    @code{.c}
    int32_t ret = BCM_ERR_OK

    if ((NULL == aConfig) (LOGIC-OR)
        (aID >= IIC_MAX_HW_ID) (LOGIC-OR)
        (aConfig->speed > IIC_SPEED_MAX) (LOGIC-OR)
        (aConfig->speed < IIC_SPEED_100K))
        ret = BCM_ERR_INVAL_PARAMS

    if (IIC_BscRWDev[aID].state != IIC_BSC_STATE_UNINIT)
        ret = BCM_ERR_INVAL_STATE

    if (BCM_BOOL_TRUE == aConfig->txFifoMode)
        ret = BCM_ERR_NOSUPPORT

    speed = aConfig->speed

    IIC_BSC_REGS[aID]->bscfcr = IIC_BSC_REGS[aID]->bscfcr (BIT_OR) IIC_BSCFCR_FLUSH_MASK
    IIC_BSC_REGS[aID]->bscfcr &= ~IIC_BSCFCR_FIFOEN_MASK
    IIC_BSC_REGS[aID]->bscclken = IIC_BSC_REGS[aID]->bscclken (BIT_OR) IIC_BSCCLKEN_CLKEN_MASK
    IIC_BSC_REGS[aID]->bsctout = (IIC_BSCTOUT_TOUT_MASK)

    switch (speed)
    case IIC_SPEED_100K:
        reg = IIC_BSC_REGS[aID]->bscclken
        reg &= ~(IIC_BSCCLKEN_MPHASE_MASK (BIT_OR) IIC_BSCCLKEN_NPHASE_MASK)
        IIC_BSC_REGS[aID]->bscclken = reg

        reg = IIC_BSC_REGS[aID]->bsctim
        reg &= ~(IIC_BSCTIM_P_MASK (BIT_OR) IIC_BSCTIM_DIV_MASK)
        reg = reg (BIT_OR) ((0x0UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK)
        reg = reg (BIT_OR) ((0x0UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK)
        IIC_BSC_REGS[aID]->bsctim = reg
        break
    case IIC_SPEED_400K:
        reg = IIC_BSC_REGS[aID]->bscclken
        reg &= ~(IIC_BSCCLKEN_MPHASE_MASK (BIT_OR) IIC_BSCCLKEN_NPHASE_MASK)
        IIC_BSC_REGS[aID]->bscclken = reg

        reg = IIC_BSC_REGS[aID]->bsctim
        reg &= ~(IIC_BSCTIM_P_MASK (BIT_OR) IIC_BSCTIM_DIV_MASK)
        reg = reg (BIT_OR) ((0x2UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK)
        reg = reg (BIT_OR) ((0x3UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK)
        IIC_BSC_REGS[aID]->bsctim = reg
        break
    case IIC_SPEED_MAX:
        reg = IIC_BSC_REGS[aID]->bscclken
        reg &= ~(IIC_BSCCLKEN_MPHASE_MASK (BIT_OR) IIC_BSCCLKEN_NPHASE_MASK)
        IIC_BSC_REGS[aID]->bscclken = reg

        reg = IIC_BSC_REGS[aID]->bsctim
        reg &= ~(IIC_BSCTIM_P_MASK (BIT_OR) IIC_BSCTIM_DIV_MASK)
        reg = reg (BIT_OR) ((0x0UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK)
        reg = reg (BIT_OR) ((0x3UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK)
        IIC_BSC_REGS[aID]->bsctim = reg
        break
    default:
        ret = BCM_ERR_UNKNOWN
        break

    if (BCM_ERR_OK == ret)
        IIC_BSC_REGS[aID]->bscclken &= (~IIC_BSCCLKEN_AUTO_SENSE_MASK)
        IIC_BSC_REGS[aID]->bscier = 0UL
        IIC_BSC_REGS[aID]->bscisr = IIC_BSCIER_ALL_INTERRUPTS_MASK
        IIC_BSC_REGS[aID]->bsccs &= ~(IIC_BSCCS_CMD_MASK (BIT_OR) IIC_BSCCS_ACK_MASK)
        IIC_BSC_REGS[aID]->bsccs = IIC_BSC_REGS[aID]->bsccs (BIT_OR) IIC_BSCCS_EN_MASK
        IIC_BscRWDev[aID].allocStatus = (1UL << LWQ_BUFFER_MAX_LENGTH) - 1UL
        LWQ_Init(&IIC_BscRWDev[aID].procQ)
        IIC_BscRWDev[aID].state = IIC_BSC_STATE_IDLE

    err_exit:
    return ret
    @endcode
*/
int32_t IIC_Init(IIC_HwIDType aID, const IIC_ConfigType * const aConfig)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t reg;
    IIC_SpeedType speed;
#ifdef BCM8915X_PARAM_VALIDATION
    if ((NULL == aConfig) ||
        (aID >= IIC_MAX_HW_ID) ||
        (aConfig->speed > IIC_SPEED_MAX) ||
        (aConfig->speed < IIC_SPEED_100K)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (IIC_BscRWDev[aID].state != IIC_BSC_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (0UL != aConfig->txFifoMode) {
        ret = BCM_ERR_NOSUPPORT;
    } else {
#endif
        BCM_MemSet(&IIC_BscRWDev[aID], 0U, sizeof(IIC_BscRWDevType));

        speed = aConfig->speed;
        /*  Bring the peripheral out of reset */
        IIC_IntPowerUpOutReset(aID);
        BCM_DelayUs(1000);

        /* Enable BSC Controller clocks */
        IIC_BSC_REGS[aID]->bscclken |= IIC_BSCCLKEN_CLKEN_MASK;
        /* Enable controller */
        IIC_BSC_REGS[aID]->bsccs |= (IIC_BSCCS_SDA_MASK | IIC_BSCCS_SCL_MASK | IIC_BSCCS_EN_MASK);
        IIC_CFG_Regs->misc_ctrl &= ~(CFG_MISC_CTRL_I2C_SCALER_RATIO_MASK);
        IIC_CFG_Regs->misc_ctrl |= 0x400UL;
        IIC_BSC_REGS[aID]->bsctout = (IIC_BSCTOUT_TOUT_MASK);
        IIC_BSC_REGS[aID]->i2c_slave_cpu_ctrl = 0x80000098UL;
        IIC_BSC_REGS[aID]->i2c_slave_reg_rw_en = 0xF00UL;
        BCM_DelayUs(10);
        /* configure the bus speed */
        switch (speed) {
        case IIC_SPEED_100K:
            reg = IIC_BSC_REGS[aID]->bscclken;
            reg &= ~(IIC_BSCCLKEN_MPHASE_MASK | IIC_BSCCLKEN_NPHASE_MASK);
            reg |= (0x5UL << IIC_BSCCLKEN_MPHASE_SHIFT);
            reg |= (0x4UL << IIC_BSCCLKEN_NPHASE_SHIFT);
            IIC_BSC_REGS[aID]->bscclken = reg;
            BCM_DelayUs(10);
            reg = IIC_BSC_REGS[aID]->bsctim;
            reg &= ~(IIC_BSCTIM_P_MASK | IIC_BSCTIM_DIV_MASK);
            reg |= (0x0UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK;
            reg |= (0x0UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK;
            IIC_BSC_REGS[aID]->bsctim = reg;
            BCM_DelayUs(10);
            break;
        case IIC_SPEED_400K:
            reg = IIC_BSC_REGS[aID]->bscclken;
            reg &= ~(IIC_BSCCLKEN_MPHASE_MASK | IIC_BSCCLKEN_NPHASE_MASK);
            reg |= (0x1UL << IIC_BSCCLKEN_NPHASE_SHIFT);
            IIC_BSC_REGS[aID]->bscclken = reg;
            BCM_DelayUs(10);
            reg = IIC_BSC_REGS[aID]->bsctim;
            reg &= ~(IIC_BSCTIM_P_MASK | IIC_BSCTIM_DIV_MASK);
            reg |= (0x0UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK;
            reg |= (0x0UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK;
            IIC_BSC_REGS[aID]->bsctim = reg;
            BCM_DelayUs(10);
            break;
        case IIC_SPEED_MAX:
            reg = IIC_BSC_REGS[aID]->bscclken;
            reg &= ~(IIC_BSCCLKEN_MPHASE_MASK | IIC_BSCCLKEN_NPHASE_MASK);
            reg |= (0x5UL << IIC_BSCCLKEN_NPHASE_SHIFT);
            IIC_BSC_REGS[aID]->bscclken = reg;
            BCM_DelayUs(10);
            reg = IIC_BSC_REGS[aID]->bsctim;
            reg &= ~(IIC_BSCTIM_P_MASK | IIC_BSCTIM_DIV_MASK);
            reg |= (0x0UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK;
            reg |= (0x2UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK;
            IIC_BSC_REGS[aID]->bsctim = reg;
            BCM_DelayUs(10);
            break;
        default:
            /* Never reach here */
            ret = BCM_ERR_UNKNOWN;
            break;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    if (BCM_ERR_OK == ret) {
        /* disable autosense by default */
        IIC_BSC_REGS[aID]->bscclken &= (~IIC_BSCCLKEN_AUTO_SENSE_MASK);
        /* Disable FIFO */
        IIC_BSC_REGS[aID]->bscfcr |= IIC_BSCFCR_FLUSH_MASK;
        IIC_BSC_REGS[aID]->bscfcr &= ~IIC_BSCFCR_FIFOEN_MASK;
        /* Enable FIFO */
        IIC_BSC_REGS[aID]->bscfcr &= ~IIC_BSCFCR_FLUSH_MASK;
        IIC_BSC_REGS[aID]->bscfcr |= IIC_BSCFCR_FIFOEN_MASK;
        /* Disable all the interrupts */
        IIC_BSC_REGS[aID]->bscier = 0UL;
        /* Clear all the interrupts */
        IIC_BSC_REGS[aID]->bscisr = IIC_BSCIER_ALL_INTERRUPTS_MASK;
        /* Enable BSC controller and set CMD = No Action */
        IIC_BSC_REGS[aID]->bsccs &= ~(IIC_BSCCS_CMD_MASK | IIC_BSCCS_ACK_MASK);
        BCM_DelayUs(10);
        IIC_BscRWDev[aID].allocStatus = (1UL << LWQ_BUFFER_MAX_LENGTH) - 1UL;
        LWQ_Init(&IIC_BscRWDev[aID].procQ);

        /* Set BSC state to Idle */
        IIC_BscRWDev[aID].state = IIC_BSC_STATE_IDLE;
    }
    return ret;
}

/** @brief IIC transfer request

    This API queues a transfer request to IIC driver. When this request
    processing is complete, client shall get status using #IIC_GetStatus API.

    @behavior Async, Re-entrant

    @pre I2C should be in initialized state

    @param[in]      aID         Index of the IIC bus controller
    @param[in]      aPkts       Pointer to array of IIC transfer packets
    @param[in]      aNumPkts    Number of IIC transfer packets
    @param[in]      aClientID    ClientID of caller (Unique id of the caller used to verify while notifying the transfer status using #IIC_GetStatus)
    @param[in]      aClientMask Mask to notify client (as used by the client to specify event mask, it is applicable when OS event is used)
    @param[out]     aJobID      Handle to query status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transfer request is successfully queued
                                        to IIC driver
    @retval     #BCM_ERR_UNKNOWN        Error in queuing transfer request.
    @retval     #BCM_ERR_NOMEM          Transfer request queue is full. Retry the transfer.
    @retval     #BCM_ERR_INVAL_STATE    I2C is in in deinit state
    @retval     #BCM_ERR_INVAL_PARAMS   aNumPkts is zero
    @retval     #BCM_ERR_INVAL_PARAMS   aPkts is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aID
    @retval     #BCM_ERR_INVAL_PARAMS   aJobID is NULL

    @post None

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    int32_t ret = BCM_ERR_NOMEM

    if ((aID >= IIC_MAX_HW_ID)
        (LOGIC_OR) (NULL == aPkts) (LOGIC_OR) (0UL == aNumPkts))
        ret =BCM_ERR_INVAL_PARAMS
    else if (IIC_BSC_STATE_UNINIT == IIC_BscRWDev[aID].state)
        ret =BCM_ERR_INVAL_STATE
    else
        jobID = IIC_BscClrNGetBit(&IIC_BscRWDev[aID].allocStatus)
        if (jobID < LWQ_BUFFER_MAX_LENGTH)
            IIC_Msg[aID][jobID].retVal = BCM_ERR_BUSY
            IIC_Msg[aID][jobID].numPkts = aNumPkts
            IIC_Msg[aID][jobID].pkts = aPkts
            IIC_Msg[aID][jobID].clientID = aClientID
            IIC_Msg[aID][jobID].clientMask = aClientMask
            ret = LWQ_Push(&IIC_BscRWDev[aID].procQ, (uint8_t)jobID)
            if (BCM_ERR_OK == ret)
                *aJobID = (aID << 4UL) (BIT_OR) jobID
            else
                IIC_BscSetBit(&IIC_BscRWDev[aID].allocStatus, jobID)
                ret = BCM_ERR_UNKNOWN
        IIC_BscStartTransfer(aID)

    return ret
    @endcode
*/
int32_t IIC_Transfer(IIC_HwIDType aID,
                        IIC_PktType * const aPkts,
                        uint32_t aNumPkts,
                        int32_t aClientID,
                        uint32_t aClientMask,
                        uint32_t *aJobID)
{
    int32_t ret = BCM_ERR_NOMEM;
    uint32_t jobID;

    if ((aID >= IIC_MAX_HW_ID)
        || (NULL == aPkts) || (0UL == aNumPkts)){
        ret =BCM_ERR_INVAL_PARAMS;
    } else if (IIC_BSC_STATE_UNINIT == IIC_BscRWDev[aID].state) {
        ret =BCM_ERR_INVAL_STATE;
    } else {
        jobID = IIC_BscClrNGetBit(&IIC_BscRWDev[aID].allocStatus);
        if (jobID < LWQ_BUFFER_MAX_LENGTH) {
            IIC_Msg[aID][jobID].retVal = BCM_ERR_BUSY;
            IIC_Msg[aID][jobID].numPkts = aNumPkts;
            IIC_Msg[aID][jobID].pkts = aPkts;
            IIC_Msg[aID][jobID].clientID = aClientID;
            IIC_Msg[aID][jobID].clientMask = aClientMask;
            ret = LWQ_Push(&IIC_BscRWDev[aID].procQ, (uint8_t)jobID);
            if (BCM_ERR_OK == ret) {
                *aJobID = (aID << 4UL) | jobID;
            } else {
                IIC_BscSetBit(&IIC_BscRWDev[aID].allocStatus, jobID);
                ret = BCM_ERR_UNKNOWN;
            }
        }
        if (BCM_ERR_OK == ret) {
            #ifdef BCM8915X_I2C_POLLING
            ret = IIC_BscStartTransfer(aID);
            #else
            /* Ignore the return values from start transfer for interrupt mode. */
            (void)IIC_BscStartTransfer(aID);
            #endif
        }
    }

    return ret;
}

/**
    @brief Get transfer status

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID
    @param[in] aClientID    Client ID of caller
    @param[in] aJobID       jobID

    @retval  #BCM_ERR_OK             Transfer is complete
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid Job ID
    @retval  #BCM_ERR_AUTH_FAILED    Invalid Client ID
    @retval  #BCM_ERR_BUSY           Transfer is in progress

    @post None

    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

    @code{.c}
    uint32_t jobID = aJobID & LWQ_BUFFER_MAX_LENGTH
    int32_t retVal = BCM_ERR_BUSY

    if (aID != (aJobID >> 4UL))
        retVal = BCM_ERR_INVAL_PARAMS
    else if (aClientID != IIC_Msg[aID][jobID].clientID)
        retVal = BCM_ERR_AUTH_FAILED
    else if (BCM_ERR_BUSY != IIC_Msg[aID][jobID].retVal)
        retVal = IIC_Msg[aID][jobID].retVal
        IIC_BscSetBit(&IIC_BscRWDev[aID].allocStatus, jobID)
    else
        retVal = BCM_ERR_BUSY

    return retVal
    @endcode
*/
int32_t IIC_GetStatus(IIC_HwIDType aID,
                                int32_t aClientID,
                                const uint32_t aJobID)
{
    uint32_t jobID = aJobID & LWQ_BUFFER_MAX_LENGTH;
    int32_t retVal = BCM_ERR_BUSY;

    if (aID != (aJobID >> 4UL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aClientID != IIC_Msg[aID][jobID].clientID) {
        retVal = BCM_ERR_AUTH_FAILED;
    } else if (BCM_ERR_BUSY != IIC_Msg[aID][jobID].retVal) {
        retVal = IIC_Msg[aID][jobID].retVal;
        IIC_BscSetBit(&IIC_BscRWDev[aID].allocStatus, jobID);
    } else {
        retVal = BCM_ERR_BUSY;
    }

    return retVal;
}

/**
    @brief De-initialize the IIC BSC device

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID

    @retval  #BCM_ERR_OK             De-Init is successful
    @retval  #BCM_ERR_INVAL_STATE    I2C driver is in wrong state
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid HW ID

    @post None

    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWREQ_IIC_INIT

    @code{.c}
    int32_t ret = BCM_ERR_OK

    if (aID < IIC_MAX_HW_ID)
        if (IIC_BscRWDev[aID].state == IIC_BSC_STATE_UNINIT)
            ret = BCM_ERR_INVAL_STATE
        else
            IIC_BSC_REGS[aID]->bscier = ~(IIC_BSCIER_ALL_INTERRUPTS_MASK)
            IIC_BSC_REGS[aID]->bscisr = IIC_BSCISR_ALL_INTERRUPTS_MASK
            IIC_BSC_REGS[aID]->bscclken = ~IIC_BSCCLKEN_CLKEN_MASK
            IIC_BSC_REGS[aID]->bsccs &= ~IIC_BSCCS_EN_MASK
            IIC_BscRWDev[aID].state = IIC_BSC_STATE_UNINIT

            BCM_MemSet(&(IIC_BscRWDev[aID]), 0, sizeof(IIC_BscRWDevType))
            BCM_MemSet(&(IIC_Msg[aID]), 0, sizeof(IIC_MsgType) * LWQ_BUFFER_MAX_LENGTH)
    else
        ret = BCM_ERR_INVAL_PARAMS

    return ret
    @endcode
*/
int32_t IIC_DeInit(IIC_HwIDType aID)
{
    int32_t ret = BCM_ERR_OK;

    if (aID < IIC_MAX_HW_ID) {

        /* Check Bsc device State */
        if (IIC_BscRWDev[aID].state == IIC_BSC_STATE_UNINIT) {
            ret = BCM_ERR_INVAL_STATE;
        } else {
            /* Disable all the interrupts */
            IIC_BSC_REGS[aID]->bscier = (uint32_t)~(IIC_BSCIER_ALL_INTERRUPTS_MASK);
            /* Clear all the interrupts */
            IIC_BSC_REGS[aID]->bscisr = IIC_BSCISR_ALL_INTERRUPTS_MASK;
            /* Disable BSC clock and disable the controller */
            IIC_BSC_REGS[aID]->bscclken = (uint32_t)~IIC_BSCCLKEN_CLKEN_MASK;
            IIC_BSC_REGS[aID]->bsccs &= ~IIC_BSCCS_EN_MASK;

            /* Set device state to Uninit */
            IIC_BscRWDev[aID].state = IIC_BSC_STATE_UNINIT;

            BCM_MemSet(&(IIC_BscRWDev[aID]), 0, sizeof(IIC_BscRWDevType));
            BCM_MemSet(&(IIC_Msg[aID]), 0, sizeof(IIC_MsgType) * LWQ_BUFFER_MAX_LENGTH);
            /* Keep the peripheral in reset */
            IIC_IntInReset(aID);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief IRQ handler for the IIC BSC device

    @behavior Sync, Re-entrant

    @pre I2C should be in initialized state

    @param[in] aID          I2C HW ID

    @return clientSignal structure

    @post None

    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_KERNEL_HANDLER

    @code{.c}
    clientSignal.done = BCM_BOOL_FALSE
    clientSignal.clientID = 0L
    clientSignal.clientMask = 0UL
    uint32_t intStatus = 0UL

    if (aID < IIC_MAX_HW_ID)
        intStatus = IIC_BSC_REGS[aID]->bscisr
        IIC_BSC_REGS[aID]->bscisr = intStatus
        IIC_BSC_REGS[aID]->bsccs &= ~(IIC_BSCCS_CMD_MASK (BIT_OR) IIC_BSCCS_ACK_MASK)
        if ((intStatus & IIC_BSCISR_NOACK_MASK) != 0UL)
            err = BCM_ERR_NACK
        else if ((intStatus & IIC_BSCISR_BSCERR_MASK) != 0UL)
            err = BCM_ERR_BUS_FAILURE
        else
            err = BCM_ERR_OK

        state = IIC_BscRWDev[aID].state
        if (IIC_BSC_STATE_UNINIT == state)
            Report error
        else
            ret = IIC_XferStateHandler(aID, err)
            if (BCM_ERR_BUSY != ret)
                uint8_t jobID = 0U
                (void)LWQ_Pop(&IIC_BscRWDev[aID].procQ, &jobID)
                clientSignal.done = BCM_BOOL_TRUE
                clientSignal.clientID = IIC_Msg[aID][jobID].clientID
                clientSignal.clientMask = IIC_Msg[aID][jobID].clientMask
                IIC_Msg[aID][jobID].retVal = ret
                IIC_BscStartTransfer(aID)

    return clientSignal
    @endcode
*/
IIC_ClientSignalType IIC_IRQHandler(IIC_HwIDType aID)
{
    IIC_BscStateType state;
    int32_t ret;
    int32_t err;
    IIC_ClientSignalType clientSignal;
    clientSignal.done = 0UL;
    clientSignal.clientID = 0L;
    clientSignal.clientMask = 0UL;
    uint32_t intStatus = 0UL;

    if (aID < IIC_MAX_HW_ID) {
        intStatus = IIC_BSC_REGS[aID]->bscisr;
        IIC_BSC_REGS[aID]->bscisr = intStatus;
        IIC_BSC_REGS[aID]->bsccs &= ~(IIC_BSCCS_CMD_MASK | IIC_BSCCS_ACK_MASK);
        if ((intStatus & IIC_BSCISR_NOACK_MASK) != 0UL) {
            err = BCM_ERR_NACK;
        } else if ((intStatus & IIC_BSCISR_BSCERR_MASK) != 0UL) {
            err = BCM_ERR_BUS_FAILURE;
        } else {
            err = BCM_ERR_OK;
        }

        state = IIC_BscRWDev[aID].state;
        if (IIC_BSC_STATE_UNINIT == state) {
            /*BCM_LOG_DEBUG(0U, BCM_STATE_RUN, BCM_ERR_UNINIT, "Failed to handle I2C BSC interrupt.\n");*/
        } else {
            /* TODO: Check if any pkt transfer was in progress. Need to add idle xfer state */
            ret = IIC_XferStateHandler(aID, err);

            if (BCM_ERR_BUSY != ret) {
                uint8_t jobID = 0U;
                (void)LWQ_Pop(&IIC_BscRWDev[aID].procQ, &jobID);
                clientSignal.done = 1UL;
                clientSignal.clientID = IIC_Msg[aID][jobID].clientID;
                clientSignal.clientMask = IIC_Msg[aID][jobID].clientMask;
                IIC_Msg[aID][jobID].retVal = ret;
            }
        }
    }

    return clientSignal;
}

/** @brief This API enables the I2C interrupt

    This API enable the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the IIC bus controller
    @param[in]      intrType      Interrupt type
    @param[in]      aEnable       Enable or disable the selected interrupt type

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   IIC driver interrupt enable is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_IIC_INIT
    @trace #BRCM_SWARCH_IIC_ENABLE_INTERRUPT_PROC
    @limitations None
    @code{.c}
    Set bsc interrupt enable register for the selected interrupt type.
    @endcode
*/
int IIC_EnableInterrupt(IIC_HwIDType aId, IIC_InterruptType intrType, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t intMask = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= IIC_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
#endif
        if(intrType == IIC_INTERRUPT_ALL_INTR) {
            intMask = (IIC_BSCIER_NOACK_EN_MASK | IIC_BSCIER_FIFOINT_EN_MASK |
                       IIC_BSCIER_ERRINT_EN_MASK | IIC_BSCIER_BSC_INT_EN_MASK | IIC_BSCIER_CMD_RUN_INT_EN_MASK);
        } else {
            if((intrType & IIC_INTERRUPT_FIFO_EMPTY_INTR) > 0UL) {
                intMask |= IIC_BSCIER_FIFOINT_EN_MASK;
            }
            if((intrType & IIC_INTERRUPT_NOACK_INTR) > 0UL) {
                intMask |= IIC_BSCIER_NOACK_EN_MASK;
            }
            if((intrType & IIC_INTERRUPT_BUSY_INTR) > 0UL) {
                intMask |= IIC_BSCIER_BSC_INT_EN_MASK;
            }
            if((intrType & IIC_INTERRUPT_ERROR_INTR) > 0UL) {
                intMask |= (IIC_BSCIER_ERRINT_EN_MASK);
            }
            if((intrType & IIC_INTERRUPT_CMD_RUN_INTR) > 0UL) {
                intMask |= IIC_BSCIER_CMD_RUN_INT_EN_MASK;
            }

        }
        if(aEnable > 0UL) {
                IIC_BSC_REGS[aId]->bscier = intMask;
            } else {
                IIC_BSC_REGS[aId]->bscier &= ~intMask;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief This API clears the IIC interrupt

    This API clear the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the IIC bus controller
    @param[in]      intrType      Clear the selected interrupt types

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   IIC driver interrupt clear is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_IIC_INIT
    @trace  #BRCM_SWARCH_IIC_CLEAR_INTERRUPT_PROC
    @limitations None
    @code{.c}
       Clear the interrupt by writing into bsc interrupt status register.
    @endcode
*/
int IIC_ClearInterrupt(IIC_HwIDType aId, IIC_InterruptType intrType)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t clearMask = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= IIC_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
#endif
        if(intrType == IIC_INTERRUPT_ALL_INTR) {
            clearMask = (IIC_BSCISR_NOACK_MASK | IIC_BSCISR_TXFIFOEMPTY_MASK |
                       IIC_BSCISR_BSCERR_MASK | IIC_BSCISR_BSC_SES_DONE_MASK | IIC_BSCISR_COMMAND_RUN_MASK);
        } else {
            if((intrType & IIC_INTERRUPT_FIFO_EMPTY_INTR) > 0UL) {
                clearMask |= IIC_BSCISR_TXFIFOEMPTY_MASK;
            }
            if((intrType & IIC_INTERRUPT_NOACK_INTR) > 0UL) {
                clearMask |= IIC_BSCISR_NOACK_MASK;
            }
            if((intrType & IIC_INTERRUPT_BUSY_INTR) > 0UL) {
                clearMask |= IIC_BSCISR_BSC_SES_DONE_MASK;
            }
            if((intrType & IIC_INTERRUPT_ERROR_INTR) > 0UL) {
                clearMask |= (IIC_BSCISR_BSCERR_MASK);
            }
            if((intrType & IIC_INTERRUPT_CMD_RUN_INTR) > 0UL) {
                clearMask |= IIC_BSCISR_COMMAND_RUN_MASK;
            }

        }

        IIC_BSC_REGS[aId]->bscisr = clearMask;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Get the I2C interrupt status for all the interrupts

    This API Gets the interrupt status for all the I2C interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the I2C peripheral
    @param[out]    intrStatus    Get I2C interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   UART driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_IIC_INIT
    @trace  #BRCM_SWARCH_IIC_GET_INTERRUPT_STATUS_PROC

    @limitations None
    @code{.unparsed}
      Get the interrupt status depending on interrupt type specified in
      intrType
      Option to select all the interrupt or individual interrupt
    @endcode

*/
static void IIC_GetAllInterruptStatus(IIC_HwIDType aId, uint32_t *intrStatus)
{
    uint32_t status = 0UL;

    *intrStatus = 0;
    status =  IIC_BSC_REGS[aId]->bscisr;
    *intrStatus = (((status & IIC_BSCISR_NOACK_MASK) > 0UL) ? IIC_INTERRUPT_NOACK_INTR : 0UL) | (((status & IIC_BSCISR_TXFIFOEMPTY_MASK) > 0UL) ? IIC_INTERRUPT_FIFO_EMPTY_INTR : 0UL) |
                    (((status & IIC_BSCISR_BSC_SES_DONE_MASK) > 0UL) ? IIC_INTERRUPT_BUSY_INTR : 0UL) |
                    (((status & IIC_BSCISR_BSCERR_MASK) > 0UL) ? IIC_INTERRUPT_ERROR_INTR : 0UL) | (((status & IIC_BSCISR_COMMAND_RUN_MASK) > 0UL) ? IIC_INTERRUPT_CMD_RUN_INTR : 0UL);
}


/** @brief This API gets the IIC interrupt status

    This API Get the interrupt status for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     aID           Index of the IIC bus controller
    @param[in]     intrType      Get the interrupt status for the selected interrupt types
    @param[out]    intrStatus    Get IIC interrupt status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   IIC driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_IIC_INIT
    @trace  #BRCM_SWARCH_IIC_GET_INTERRUPT_STATUS_PROC
    @limitations None
    @code{.unparsed}
      Get the interrupt status depending on interrupt type specified in
      intrType
      Option to select all the interrupt or individual interrupt
    @endcode
*/
int IIC_GetInterruptStatus(IIC_HwIDType aId, IIC_InterruptType intrType, uint32_t *intrStatus)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_INDIVIDUAL_INTR
    uint32_t status = IIC_BSC_REGS[aId]->bscisr;
#endif
#ifdef BCM8915X_PARAM_VALIDATION
    if (aId >= IIC_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {
#endif
        *intrStatus = 0;
        if(intrType == IIC_INTERRUPT_ALL_INTR) {
            IIC_GetAllInterruptStatus(aId, intrStatus);
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else {
            if(intrType == IIC_INTERRUPT_FIFO_EMPTY_INTR) {
                *intrStatus |= (((status & IIC_BSCISR_TXFIFOEMPTY_MASK) > 0UL) ? IIC_INTERRUPT_FIFO_EMPTY_INTR : 0UL);
            }
            if(intrType == IIC_INTERRUPT_NOACK_INTR) {
                *intrStatus |= (((status & IIC_BSCISR_NOACK_MASK) > 0UL) ? IIC_INTERRUPT_NOACK_INTR : 0UL);
            }
            if(intrType == IIC_INTERRUPT_BUSY_INTR) {
                *intrStatus |= (((status & IIC_BSCISR_BSC_SES_DONE_MASK) > 0UL) ? IIC_INTERRUPT_BUSY_INTR : 0UL);
            }
            if(intrType == IIC_INTERRUPT_ERROR_INTR) {
                *intrStatus |= (((status & IIC_BSCISR_BSCERR_MASK) > 0UL) ? IIC_INTERRUPT_ERROR_INTR : 0UL);
            }
            if(intrType == IIC_INTERRUPT_CMD_RUN_INTR) {
                *intrStatus |= (((status & IIC_BSCISR_COMMAND_RUN_MASK) > 0UL) ? IIC_INTERRUPT_CMD_RUN_INTR : 0UL);
            }
        }
#endif
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @} */

