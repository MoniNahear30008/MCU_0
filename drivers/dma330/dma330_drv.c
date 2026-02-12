/*****************************************************************************
  Copyright 2024 Broadcom Limited.  All rights reserved.

  Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_dma330drv_drv_impl System DMA
    @ingroup grp_dma330drv

    @addtogroup grp_dma330drv_drv_impl
    @{

    @file drivers/dma330/dma330_drv.c
    @brief DMA330 Driver Design

    @version 0.1 Initial version
*/

#include <stdint.h>
#include <string.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <dma330.h>
#include <dma330_types.h>
#include <dma330_rdb.h>
#include "clkrst_rdb.h"
#include "cpu_rdb.h"

/**
    @name DMA330 Driver Design IDs
    @{
    @brief Driver Design IDs for DMA330

*/
#define BRCM_SWDSGN_DMA330_INIT_PROC                     (0xB101U) /**< @brief #DMA330_Init*/
#define BRCM_SWDSGN_DMA330_XFER_SETUP_PROC               (0xB102U) /**< @brief #DMA330_XferSetup    */
#define BRCM_SWDSGN_DMA330_GET_XFER_STATUS_PROC          (0xB103U) /**< @brief #DMA330_GetXferStatus    */
#define BRCM_SWDSGN_DMA330_XFER_TRIGGER_PROC             (0xB104U) /**< @brief #DMA330_XferTrigger  */
#define BRCM_SWDSGN_DMA330_DEV_GLOBAL                    (0xB106U) /**< @brief #DMA330_Dev      */
#define BRCM_SWDSGN_DMA330_REGS_GLOBAL                   (0xB107U) /**< @brief #DMA330_REGS      */
#define BRCM_SWDSGN_DMA330_DRV_CHANN_IRQ_HANDLER_PROC    (0xB108U) /**< @brief #DMA330_DrvChannIRQHandler  */
#define BRCM_SWDSGN_DMA330_DMA330_CPUREGS_TYPE           (0xB109U) /**< @brief #DMA330_CpuRegs */
#define BRCM_SWDSGN_DMA330_CLKRST_REGS_TYPE              (0xB10AU) /**< @brief #DMA330_clkrstRegs */
#define BRCM_SWDSGN_DMA330_MOV_PROC                      (0xB10BU) /**< @brief #DMA330_Mov             */
#define BRCM_SWDSGN_DMA330_SRCINC_PROC                   (0xB10CU) /**< @brief #DMA330_SrcInc          */
#define BRCM_SWDSGN_DMA330_DSTINC_PROC                   (0xB10DU) /**< @brief #DMA330_DstInc          */
#define BRCM_SWDSGN_DMA330_CCR_PROC                      (0xB10EU) /**< @brief #DMA330_CCR             */
#define BRCM_SWDSGN_DMA330_LOAD_PROC                     (0xB10FU) /**< @brief #DMA330_Load            */
#define BRCM_SWDSGN_DMA330_STORE_PROC                    (0xB110U) /**< @brief #DMA330_Store           */
#define BRCM_SWDSGN_DMA330_LOADPERIPH_PROC               (0xB111U) /**< @brief #DMA330_LoadPeriph      */
#define BRCM_SWDSGN_DMA330_STOREPERIPH_PROC              (0xB112U) /**< @brief #DMA330_StorePeriph     */
#define BRCM_SWDSGN_DMA330_WAITFORPERIPH_PROC            (0xB113U) /**< @brief #DMA330_WaitForPeriph   */
#define BRCM_SWDSGN_DMA330_LOOPEND_PROC                  (0xB115U) /**< @brief #DMA330_LoopEnd         */
#define BRCM_SWDSGN_DMA330_LOOP_PROC                     (0xB116U) /**< @brief #DMA330_Loop            */
#define BRCM_SWDSGN_DMA330_SEV_PROC                      (0xB117U) /**< @brief #DMA330_Sev             */
#define BRCM_SWDSGN_DMA330_WRITEBARRIER_PROC             (0xB118U) /**< @brief #DMA330_WriteBarrier    */
#define BRCM_SWDSGN_DMA330_END_PROC                      (0xB119U) /**< @brief #DMA330_End             */
#define BRCM_SWDSGN_DMA330_SETCHANNSTATE_PROC            (0xB11AU) /**< @brief #DMA330_SetChannState   */
#define BRCM_SWDSGN_DMA330_GETCHANNSTATE_PROC            (0xB11BU) /**< @brief #DMA330_GetChannState   */
#define BRCM_SWDSGN_DMA330_BURSTSZTOBYTES_PROC           (0xB11DU) /**< @brief #DMA330_BurstSzToBytes  */
#define BRCM_SWDSGN_DMA330_ISPERIPXFER_PROC              (0xB11EU) /**< @brief #DMA330_IsPeripXfer     */
#define BRCM_SWDSGN_DMA330_TRIGGER_PROC                  (0xB11FU) /**< @brief #DMA330_Trigger         */
#define BRCM_SWDSGN_DMA330_KILL_PROC                     (0xB120U) /**< @brief #DMA330_Kill            */
#define BRCM_SWDSGN_DMA330_CHANNSTATEHANDLER_PROC        (0xB121U) /**< @brief #DMA330_ChannStateHandler*/
#define BRCM_SWDSGN_DMA330_WAITLOADSTORE_PROC            (0xB123U) /**< @brief #DMA330_WaitLoadStore   */
#define BRCM_SWDSGN_DMA330_XFERSETUPINTERNAL_PROC        (0xB124U) /**< @brief #DMA330_XferSetupInternal*/
#define BRCM_SWDSGN_DMA330_ABORTIRQHANDLER_PROC          (0xB125U) /**< @brief #DMA330_AbortIRQHandler */
#define BRCM_SWDSGN_DMA330_DMAMOV_REG_TYPE               (0xB126U) /**< @brief #DMA330_DMAMOV_RegType*/
#define BRCM_SWDSGN_DMA330_LP_COUNTER_TYPE               (0xB127U) /**< @brief #DMA330_DMALP_CntType*/
#define BRCM_SWDSGN_DMA330_COND_TYPE                     (0xB128U) /**< @brief #DMA330_CondType*/
#define BRCM_SWDSGN_DMA330_CHANNRW_TYPE                  (0xB129U) /**< @brief #DMA330_ChannRWType*/
#define BRCM_SWDSGN_DMA330_RWDEV_TYPE                    (0xB12AU)   /**< @brief #DMA330_RWDevType*/
#define BRCM_SWDSGN_DMA330_INT_EN_PROC                   (0xB12BU) /**< @brief #DMA330_IntEn */

#define BRCM_SWDSGN_DMA330_CHANNEL_MASK_MACRO            (0xB12CU)   /**< @brief #DMA330_CHANNEL_MASK*/
#define BRCM_SWDSGN_DMA330_PERIP_ID_MACRO                (0xB12DU)   /**< @brief #DMA330_PERIP_ID_MASK*/

/** @} */

/**
    @brief Channel macro

    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
#define DMA330_CHANNEL_MASK(idx)     (0x1UL << (idx))

/**
    @brief Peripheral macro

    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
#define DMA330_PERIP_ID_MASK         (0x1FU)
#define DMA330_PERIP_ID_SHIFT        (3U)


/**
    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef uint8_t DMA330_DMAMOV_RegType;
#define DMA330_DMAMOV_REG_SAR      (0x00U)
#define DMA330_DMAMOV_REG_CCR      (0x01U)
#define DMA330_DMAMOV_REG_DAR      (0x02U)


/**
    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef uint8_t DMA330_DMALP_CntType;
#define DMA330_LP_COUNTER_0    (0x0U)
#define DMA330_LP_COUNTER_1    (0x1U)

/**
    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef uint32_t DMA330_CondType;
#define DMA330_COND_SINGLE     (0x0U)
#define DMA330_COND_BURST      (0x1U)
#define DMA330_COND_ANY        (0x2U)

/**
    @brief Channel context

    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
    @trace #BRCM_SWARCH_DMA330_GET_XFER_STATUS_PROC
    @trace #BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC
    @trace #BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef struct sDMA330_ChannRWType {
    volatile DMA330_ChannStateType      state;
    volatile DMA330_XferStatusType      xferStatus;
    uint32_t                codeSz;
    uint32_t                xferReqSz;
    uint32_t                trigIdx;
    uint32_t                error;
    uint32_t                intCnt;
    uint32_t                abortCnt;
    uint32_t                trigCnt;
} DMA330_ChannRWType;

/**
    @brief Channel detail

    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
    @trace #BRCM_SWARCH_DMA330_GET_XFER_STATUS_PROC
    @trace #BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC
    @trace #BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
typedef struct sDMA330_RWDevType{
    uint32_t                init;
    DMA330_ChannRWType      channel[DMA330_CHANNEL_ID_MAX + 1UL];
} DMA330_RWDevType;

/**
    @brief DMA330 context

    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
    @trace #BRCM_SWARCH_DMA330_GET_XFER_STATUS_PROC
    @trace #BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC
    @trace #BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
static DMA330_RWDevType DMA330_Dev;


/**
    @brief DMA330 registers

    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
    @trace #BRCM_SWARCH_DMA330_GET_XFER_STATUS_PROC
    @trace #BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC
    @trace #BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
static DMA330_RegsType * const DMA330_REGS = (DMA330_RegsType *) DMA330_BASE;

/**
    @brief CPU regs

    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
static CPU_RDBType * const DMA330_CpuRegs = (CPU_RDBType *)MCU_BASE;
/**
    @trace #BRCM_SWARCH_DMA330_INIT_PROC
    @trace #BRCM_SWREQ_DMA330_INIT
*/
static CLKRST_RDBType * const DMA330_clkrstRegs = (CLKRST_RDBType *)RIG_CLKNRST_BASE;

/**
     @brief Helper API to form move instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_Mov(volatile uint8_t *const aCode, uint32_t *aOffset,
                        DMA330_DMAMOV_RegType aReg, uint32_t aVal)
{
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = DMA330_OP_MOV;
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = aReg;
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)(aVal & 0xFFU);
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)((aVal >> 8U) & 0xFFU);
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)((aVal >> 16U) & 0xFFU);
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)((aVal >> 24U) & 0xFFU);
}

/**
     @brief Helper API to write source increment

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_SrcInc(volatile uint8_t *const aCode, uint32_t *aOffset,
                         int16_t aVal)
{
    if (aVal >=0) {
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_SRCINC;
    } else {
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_SRCINC_NEG;
    }
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = ((uint8_t)aVal & 0xFFU);
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)((uint16_t)aVal >> 8U) & 0xFFU;
}

/**
     @brief Helper API to enable interrupt

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
/* @event           CERT-C INT34
    @issue          Coverity infers that the shifting would overflow
    @risk           No risk
    @justification  eventNo will be exceed 19   */

static void DMA330_IntEn(uint32_t aIntEn, uint8_t eventNo)
{
    if (aIntEn == 1UL) {
        /* coverity[cert_int34_c_violation] */
        DMA330_REGS->INTEN |= (1UL << eventNo);
    } else {
        /* coverity[cert_int34_c_violation] */
        DMA330_REGS->INTEN &= ~(1UL << eventNo);
    }
}

/**
     @brief Helper API to write destination increment

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */

static void DMA330_DstInc(volatile uint8_t *const aCode, uint32_t *aOffset,
                         int16_t aVal)
{
    if (aVal >=0) {
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_DSTINC;
    } else {
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_DSTINC_NEG;
    }
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = ((uint8_t)aVal & 0xFFU);
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)((uint16_t)aVal >> 8U) & 0xFFU;
}

/**
     @brief Helper API to write in to channel control register

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_CCR(volatile uint8_t *const aCode, uint32_t *aOffset,
                        const DMA330_BurstSize aBurstSz,
                        const DMA330_BurstLen aBurstLen,
                        const DMA330_XferType aXferType)
{
    uint32_t ccr = 0UL;

    /* Source settings */
    ccr |= ((aBurstSz << DMA330_CCR_SRC_BURST_SZ_SHIFT) & DMA330_CCR_SRC_BURST_SZ_MASK);
    ccr |= ((aBurstLen << DMA330_CCR_HW_SRC_BURST_LEN_SHIFT) & DMA330_CCR_SRC_BURST_LEN_MASK);
    ccr |= (0x2UL << DMA330_CCR_SRC_PROT_CNTRL_SHIFT);

    /* Destination settings */
    ccr |= ((((uint32_t)aBurstSz) << DMA330_CCR_DEST_BURST_SZ_SHIFT) &  DMA330_CCR_DEST_BURST_SZ_MASK);
    ccr |= ((((uint32_t)aBurstLen) << DMA330_CCR_DEST_BURST_LEN_SHIFT) & DMA330_CCR_DEST_BURST_LEN_MASK);
    ccr |= (0x2UL << DMA330_CCR_DEST_PROT_CNTRL_SHIFT);

    /* Cache settings */
    ccr |= (0x3UL << DMA330_CCR_DEST_CACHE_CNTRL_SHIFT);

    switch (aXferType) {
    case DMA330_XFER_TYPE_MEM_TO_MEM:
        ccr |= DMA330_CCR_HW_SRC_INC_MASK;
        ccr |= DMA330_CCR_DEST_INC_MASK;
        break;
    case DMA330_XFER_TYPE_MEM_TO_PERIPH:
        ccr |= DMA330_CCR_HW_SRC_INC_MASK;
        break;
    case DMA330_XFER_TYPE_PERIPH_TO_MEM:
        ccr |= DMA330_CCR_DEST_INC_MASK;
        break;
    case DMA330_XFER_TYPE_PERIPH_TO_PERIPH:
        break;
    default:
        /* Do nothing */
        break;
    }
    DMA330_Mov(aCode, aOffset, DMA330_DMAMOV_REG_CCR, ccr);
}

/**
     @brief Helper API to write DMA load instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */

static void DMA330_Load(volatile uint8_t *const aCode, uint32_t *aOffset)
{
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = DMA330_OP_LD;
}

/**
     @brief Helper API to write DMA store instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */

static void DMA330_Store(volatile uint8_t *const aCode, uint32_t *aOffset)
{
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = DMA330_OP_ST;
}

/**
     @brief Helper API to write load peripheral instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_LoadPeriph(volatile uint8_t *const aCode, uint32_t *aOffset,
                                DMA330_CondType aCond,
                                DMA330_PeriphIDType aPheripID)
{
    switch (aCond) {
    case DMA330_COND_SINGLE: /* bs = 0 */
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_LDP;
        break;
    default: /* deafult is BURST, bs = 1 */
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_LDP | (0x1U << 0x1U);
        break;
    }
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (aPheripID & DMA330_PERIP_ID_MASK) << DMA330_PERIP_ID_SHIFT;
}

/**
     @brief Helper API to write store peripheral instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_StorePeriph(volatile uint8_t *const aCode, uint32_t *aOffset,
                                DMA330_CondType aCond,
                                DMA330_PeriphIDType aPheripID)
{
    switch (aCond) {
    case DMA330_COND_SINGLE: /* bs = 0 */
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_STP;
        break;
    default: /* deafult is BURST, bs = 1 */
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_STP | (0x1U << 0x1U);
        break;
    }
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (aPheripID & DMA330_PERIP_ID_MASK) << DMA330_PERIP_ID_SHIFT;
}

/**
     @brief Helper API to write wait for peripheral instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_WaitForPeriph(volatile uint8_t *const aCode,  uint32_t *aOffset,
                                DMA330_CondType aCond,
                                DMA330_PeriphIDType aPheripID)
{
    switch (aCond) {
    case DMA330_COND_SINGLE: /* bs = 0, p = 0 */
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_WFP;
        break;
    case DMA330_COND_BURST: /* bs = 1, p = 0 */
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_WFP | (0x1U << 0x1U);
        break;
    default: /* bs = 0, p = 1 */
        /* coverity[cert_int30_c_violation] */
        aCode[(*aOffset)++] = DMA330_OP_WFP | (0x1U << 0U);
        break;
    }
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (aPheripID & DMA330_PERIP_ID_MASK) << DMA330_PERIP_ID_SHIFT;
}

/**
     @brief Helper API to write loop counter instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_Loop(volatile uint8_t *const aCode, uint32_t *aOffset,
                        DMA330_DMALP_CntType aLoopReg,
                        uint32_t aCounter)
{
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)((DMA330_OP_LP | (aLoopReg << 1U)) & 0xFFU);
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)(aCounter&0xFFU);
}

/**
     @brief Helper API to write loop end instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_LoopEnd(volatile uint8_t *const aCode, uint32_t *aOffset,
                            DMA330_DMALP_CntType aLoopReg,
                            uint32_t aBackJump)
{
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)((DMA330_OP_LPEND | (1U << 4U) | (aLoopReg << 2U)) & 0xFFU);
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (uint8_t)(aBackJump & 0xFFU);
}

/**
     @brief Helper API to write send event

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */

static void DMA330_Sev(volatile uint8_t *const aCode, uint32_t *aOffset,
                        uint8_t aEventNum)
{
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = DMA330_OP_SEV;
    /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = (aEventNum & 0x1FU) << 3U;
}

/**
     @brief Helper API to write write-barrier instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_WriteBarrier(volatile uint8_t *const aCode, uint32_t *aOffset)
{
   /* coverity[cert_int30_c_violation] */
   aCode[(*aOffset)++] = DMA330_OP_WMB;
}

/**
     @brief Helper API to write DMA end instruction

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */
static void DMA330_End(volatile uint8_t *const aCode, uint32_t *aOffset)
{ 
   /* coverity[cert_int30_c_violation] */
    aCode[(*aOffset)++] = DMA330_OP_END;
}

/**
     @brief Helper API to write channel state

     @trace #BRCM_SWREQ_DMA330_INIT
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
     @trace #BRCM_SWARCH_DMA330_INIT_PROC
     @trace #BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC
 */
static inline void DMA330_SetChannState(DMA330_ChannelIDType aID,
        DMA330_ChannStateType state)
{
    /*(void)ATOMIC_Set(&DMA330_Dev.channel[aID].state, state);*/
    DMA330_Dev.channel[aID].state =  state;
}

/**
     @brief Helper API to get channel state

     @trace #BRCM_SWREQ_DMA330_INIT
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
     @trace #BRCM_SWARCH_DMA330_INIT_PROC
     @trace #BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC

 */
static inline DMA330_XferStatusType DMA330_GetChannState(DMA330_ChannelIDType aID)
{
    /*return ATOMIC_Get(&DMA330_Dev.channel[aID].state);*/
    return DMA330_Dev.channel[aID].state;
}

/**
     @brief Helper to convert burst to bytes

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */

static uint32_t DMA330_BurstSzToBytes(DMA330_BurstSize aWidth)
{
    uint32_t bytes = 0;
    if (aWidth <= DMA330_BURST_SZ_MAX) {
        bytes = 1UL << aWidth;
    }
    return bytes;
}

/**
     @brief Helper to check the tranfer type is based on peripheral.

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_INIT_PROC
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */

static uint32_t DMA330_IsPeripXfer(const DMA330_ChannCfgType *aChann)
{
    uint32_t  isPerip = 1UL;
    if (DMA330_XFER_TYPE_MEM_TO_MEM == aChann->xferType) {
        isPerip = 0UL;
    }
    return isPerip;
}

/**
     @brief Helper for DMA transfer trigger

     @trace #BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC
     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
 */

static void DMA330_Trigger(DMA330_ChannelIDType aChann,
                           volatile uint8_t *const aAddr,
                           uint32_t aIntEn)
{
    /*INTR_FlagType mask = INTR_Suspend();
    CORTEX_DSB();*/
    uint32_t codeAddr = 0UL;

    DMA330_REGS->DBGINST0 = (aChann << 24UL) | (0xa0UL << 16UL) |
        /*(1UL << 17UL) |*/ (aChann << 8UL);
    codeAddr = (uint32_t)aAddr;
    DMA330_REGS->DBGINST1 = codeAddr & 0xF0FFFFFFUL;
    /*DMA330_REGS->DBGINST1 = (uint32_t)0x0180000;*/
    DMA330_REGS->DBGCMD = 0UL;
    /* @event           CERT-C INT30
        @issue          Coverity infers that the increment would overflow
        @risk           No risk
        @justification  trigCnt will be reset for every Init and based on user
                        ISR   */
    /* coverity[cert_int30_c_violation] */
    DMA330_Dev.channel[aChann].trigCnt++;
    /*INTR_Resume(mask);*/
}

/**
    @brief Helper for IRQ

    @trace #BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_DMA330_V1_DRIVER
*/

static void DMA330_Kill(DMA330_ChannelIDType aChann)
{
    DMA330_REGS->DBGINST0 = (0x1UL << 16UL) | (aChann << 8UL) | (1UL);
    DMA330_REGS->DBGINST1 = 0UL;
    DMA330_REGS->DBGCMD = 0UL;
}

/**
    @brief Helper for IRQ

    @trace #BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_DMA330_V1_DRIVER
*/

static void DMA330_ChannStateHandler(DMA330_ChannelIDType aID, uint32_t aErr,
                                     DMA330_ConfigType aCfgTbl)
{
    DMA330_ChannStateType state;
    const DMA330_ChannCfgType *channCfg;

    state = DMA330_GetChannState(aID);
    if (state == DMA330_CHANN_STATE_BUSY) {
        channCfg = &aCfgTbl.channCfg[aID];
        if (aErr != 0UL) {
            /* Based on error. Interrupt
             * handler can be updated it*/
        } else {
            /* Update based on interrupt*/
        }

        if (1UL == DMA330_IsPeripXfer(channCfg)) {
            if (aErr != 0UL) {
                DMA330_SetChannState(aID, DMA330_CHANN_STATE_ERR);
            } else {
                DMA330_SetChannState(aID, DMA330_CHANN_STATE_SETUP);
            }
        } else {
            if (aErr != 0UL) {
                DMA330_SetChannState(aID, DMA330_CHANN_STATE_ERR);
            } else {
                DMA330_SetChannState(aID, DMA330_CHANN_STATE_IDLE);
            }
        }
    } else {
        /* Report error */
    }
}

/**
    @brief Helper for setting up offset and condition

    @trace #BRCM_SWREQ_DMA330_V1_DRIVER
    @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
*/

static int32_t DMA330_WaitLoadStore(volatile uint8_t *addrBase, uint32_t *offset, const DMA330_ChannCfgType *const  aChann)
{
    int32_t ret = BCM_ERR_OK;
    switch (aChann->xferType) {
        case DMA330_XFER_TYPE_MEM_TO_PERIPH:
            DMA330_Load(addrBase, offset);
            DMA330_WaitForPeriph(addrBase, offset, DMA330_COND_BURST, aChann->periphID);
            DMA330_StorePeriph(addrBase, offset, DMA330_COND_BURST, aChann->periphID);
            break;
        case DMA330_XFER_TYPE_PERIPH_TO_MEM:
            DMA330_WaitForPeriph(addrBase, offset, DMA330_COND_BURST, aChann->periphID);
            DMA330_LoadPeriph(addrBase, offset, DMA330_COND_BURST, aChann->periphID);
            DMA330_Store(addrBase, offset);
            break;
        case DMA330_XFER_TYPE_MEM_TO_MEM:
            DMA330_Load(addrBase, offset);
            DMA330_Store(addrBase, offset);
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }
    return ret;
}
/**
    @brief Helper for setting up DMA transfer

    @trace #BRCM_SWREQ_DMA330_V1_DRIVER
    @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
*/
static int32_t DMA330_XferSetupInternal(DMA330_ChannelIDType aID,
                                        const DMA330_ChannCfgType *aChann,
                                        DMA330_XferReqType *const aXferReq,
                                        uint32_t aReqSZ)
{
    uint32_t i,j;
    uint32_t srcWid;
    uint32_t offset;
    uint32_t loopCnt;
    uint32_t loop_start0;
    uint32_t loop_start1;
    volatile uint8_t *addrBase;
    int32_t ret = BCM_ERR_OK;

    addrBase = aChann->codeAddr;
    DMA330_Dev.channel[aID].codeSz = 0UL;
    offset = 0UL;

    for (i=0; i < aReqSZ; i++) {
        srcWid = DMA330_BurstSzToBytes(aChann->burstSz);
#ifdef BCM8915X_PARAM_VALIDATION
        if ((srcWid == 0UL) || (offset > aChann->codeSize)) {
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
#endif
        /* @event           CERT-C INT30
           @issue          Coverity infers that the offset increment would overflow
           @risk           No risk
           @justification  offset got validated for every request.
           offset would not exceed totalSize                          */
        if (BCM_ERR_OK == ret) {
            DMA330_Mov(addrBase, &offset, DMA330_DMAMOV_REG_SAR, aXferReq[i].srcAddr);
            DMA330_Mov(addrBase, &offset, DMA330_DMAMOV_REG_DAR, aXferReq[i].destAddr);
            DMA330_CCR(addrBase, &offset,
                    aChann->burstSz, aChann->burstLen, aChann->xferType);
            for (j=0UL; j < aXferReq[i].nRows; j++) {
                ret = BCM_ERR_NOMEM;
                /* coverity[cert_int30_c_violation] */
                loopCnt = (aXferReq[i].rowSize) / (srcWid * (aChann->burstLen + 1UL));
                if (loopCnt < 0xFFFFUL) {
                    ret = BCM_ERR_OK;
                    if (0UL != (loopCnt & 0xFF00UL)) {
                        DMA330_Loop(addrBase, &offset, DMA330_LP_COUNTER_1, (loopCnt >> 0x8UL) -1U); /* DMA330 increments the counter by 1 */
                        loop_start1 = offset;
                        DMA330_Loop(addrBase, &offset, DMA330_LP_COUNTER_0, 255U); /* DMA330 increments the counter by 1 */
                        loop_start0 = offset;
                        ret = DMA330_WaitLoadStore(addrBase, &offset, aChann);
                        /* @event           CERT-C INT30
                           @issue          Coverity infers that the offset - loop_start0/1 wrap
                           @risk           No risk
                           @justification  This is indicate DMA need to do a backJump.*/
                        /* coverity[cert_int30_c_violation] */
                        DMA330_LoopEnd(addrBase, &offset, DMA330_LP_COUNTER_0, offset - loop_start0);
                        /* coverity[cert_int30_c_violation] */
                        DMA330_LoopEnd(addrBase, &offset, DMA330_LP_COUNTER_1, offset - loop_start1);
                        loopCnt &= 0xFFUL;
                    }
                    if (0UL != loopCnt) {
                        DMA330_Loop(addrBase, &offset, DMA330_LP_COUNTER_0, loopCnt - 1U);/* DMA330 increments the counter by 1 */
                        loop_start0 = offset;
                        ret = DMA330_WaitLoadStore(addrBase, &offset, aChann);
                        /* coverity[cert_int30_c_violation] */
                        DMA330_LoopEnd(addrBase, &offset, DMA330_LP_COUNTER_0, offset - loop_start0);
                    }
                    switch (aChann->xferType) {
                        case DMA330_XFER_TYPE_MEM_TO_PERIPH:
                            DMA330_SrcInc(addrBase, &offset, aXferReq[i].srcSkip);
                            break;
                        case DMA330_XFER_TYPE_PERIPH_TO_MEM:
                            DMA330_DstInc(addrBase, &offset, aXferReq[i].dstSkip);
                            break;
                        case DMA330_XFER_TYPE_MEM_TO_MEM:
                            DMA330_SrcInc(addrBase, &offset, aXferReq[i].srcSkip);
                            DMA330_DstInc(addrBase, &offset, aXferReq[i].dstSkip);
                            break;
                        default:
                            ret = BCM_ERR_INVAL_PARAMS;
                            break;
                    }
                }
            }
            if (1UL == DMA330_IsPeripXfer(aChann)) {
                DMA330_Sev(addrBase, &offset, (aChann->periphID % 18U));
                DMA330_End(addrBase, &offset);
            }
            if (DMA330_Dev.channel[aID].codeSz == 0UL) {
                DMA330_Dev.channel[aID].codeSz = offset;
            }
        }
    }
    if ((BCM_ERR_OK == ret) && (0UL == DMA330_IsPeripXfer(aChann))) {
        DMA330_WriteBarrier(addrBase, &offset);
        DMA330_Sev(addrBase, &offset, (uint8_t)(aID & 0xFFU));
        DMA330_End(addrBase, &offset);
    }

    return ret;
}



/** @brief Trigger DMA transfer for Memory to Memory or memory to peripheral

     This function triggers the DMA transfer which was
     setup using DMA330_XferSetup API. Upon completion of the
     transfer, DMA driver will raise a callback in interrupt
     context.

     @behavior Sync, Non-reentrant

     @pre    None

     @param[in]  aID         DMA channel ID
     @param[in]  aCfgTbl     DMA configuration structure

     @retval      BCM_ERR_OK              DMA was triggered successfully
     @retval      BCM_ERR_INVAL_PARAMS    aReqIdx is invalid
     @retval      BCM_ERR_UNINIT          DMA driver not initialized
     @retval      BCM_ERR_INVAL_STATE     DMA is not setup

     @post   None

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_TRIGGER_PROC

     @code{.unparsed}
     Validate device initialization, channel and aReqIdx
     Get channel state
     If channel is in good state, trigger the DMA transfer
     @endcode

    @limitations None
*/
int32_t DMA330_XferTrigger(DMA330_ChannelIDType aID,
                            uint32_t aReqIdx,
                            DMA330_ConfigType aCfgTbl)
{

    DMA330_ChannStateType state;
    volatile uint8_t *addrBase;
    int32_t ret = BCM_ERR_OK;

    if (0UL == DMA330_Dev.init) {
        ret = BCM_ERR_UNINIT;
    } else if (aID > DMA330_CHANNEL_ID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (aReqIdx >=  DMA330_Dev.channel[aID].xferReqSz) {
    /* aReqIdx withing the setup region? */
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        state = DMA330_GetChannState(aID);

        /* Is this channel configured by system? */
        if (state == DMA330_CHANN_STATE_UNINIT) {
            ret = BCM_ERR_INVAL_PARAMS;
        }

        /*
         * is request setup by DMA?
         */
        if (state != DMA330_CHANN_STATE_SETUP) {
            ret = BCM_ERR_INVAL_STATE;
        }
        /* set channel state busy and trigger the DMA*/
        if (ret == BCM_ERR_OK) {
            DMA330_SetChannState(aID, DMA330_CHANN_STATE_BUSY);
            DMA330_Dev.channel[aID].trigIdx = aReqIdx;
            addrBase = aCfgTbl.channCfg[aID].codeAddr + (aReqIdx * DMA330_Dev.channel[aID].codeSz);
            DMA330_Trigger(aID, addrBase, 1UL);
            /* Always enable interrupt*/
            if (1UL == DMA330_IsPeripXfer(&aCfgTbl.channCfg[aID])) {
                DMA330_IntEn(1UL, (aCfgTbl.channCfg[aID].periphID % 18U));
            } else {
                DMA330_IntEn(1UL, (uint8_t)(aID & 0xFFUL));
            }

        }
    }
    return ret;
}
/** @brief Initialize DMA330

    This API initializes the given DMA330 hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      CfgTbl         DMA configuration structure

    @return     int32_t
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_OK             Success


    @post None

    @trace   #BRCM_SWARCH_DMA330_INIT_PROC
    @trace  #BRCM_SWREQ_DMA330_INIT
    @code{.unparsed}
        if (0UL == DMA_Dev.init)
        if (config->channCfgSz <= (DMA_CHANNEL_ID_MAX + 1UL))
            for (i = 0UL; i < config->channCfgSz; i++)
                if (config->channCfg[i].xferType > DMA_XFER_TYPE_MAX)
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                if (config->channCfg[i].burstSz > DMA_BURST_SZ_MAX)
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                if ((1UL == DMA330_IsPeripXfer(&config->channCfg[i])) &&
                        (config->channCfg[i].xferDoneCb == NULL))
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                if ((config->channCfg[i].codeAddr == NULL) || (config->channCfg[i].codeSize == 0UL))
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                  channel initialized and idle now
                DMA_SetChannState(i, DMA_CHANN_STATE_IDLE);
            if (err == BCM_ERR_OK)
                DMA_Dev.init = 1UL;
    @endcode

    @limitations None
*/
int32_t DMA330_Init(DMA330_ConfigType aCfgTbl)
{
    uint32_t i;
    int32_t err = BCM_ERR_OK;
    const DMA330_ConfigType * config = &aCfgTbl;

    BCM_MemSet(&DMA330_Dev, 0, sizeof(DMA330_Dev));
    /*
     * Verify the configuration parameters and if
     * its valid, set the DMA driver state to Initialized
     */

    if (0UL == DMA330_Dev.init) {
        if (config->channCfgSz <= (DMA330_CHANNEL_ID_MAX + 1UL)) {
            for (i = 0UL; i < config->channCfgSz; i++) {
                if (config->channCfg[i].xferType > DMA330_XFER_TYPE_MAX) {
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                if (config->channCfg[i].burstSz > DMA330_BURST_SZ_MAX) {
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                if ((config->channCfg[i].codeAddr == NULL) || (config->channCfg[i].codeSize == 0UL)) {
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                /*
                 * channel initialized and idle now
                 */
                DMA330_SetChannState(i, DMA330_CHANN_STATE_IDLE);
            }
            if (err == BCM_ERR_OK) {
                DMA330_clkrstRegs->cpu_dma_sys_reset_control = 0UL;
                DMA330_CpuRegs->dma330_boot_ctrl |= CPU_BOOT_CTYPE_MANAGER_NS_MASK;
                DMA330_CpuRegs->dma330_boot_addr = (uint32_t)config->channCfg[0U].codeAddr;
                DMA330_Dev.init = 1UL;
            }
        } else {
            err = BCM_ERR_INVAL_PARAMS;
        }

    }
    return err;
}



/** @brief Initiate DMA transfer from Memory to Memory or memory to peripheral

     This function initiates Memory to Memory or memory to peripheral transfer using DMA.
     Only a fixed type of transfer is allowed on a channel
     (which is passed to DMA driver during initialization).
     This API will chain the DMA requests (@aXferReq) and trigger
     the DMA automatically. Upon completion of all transfers
     it will raise a callback to client.

     @pre None
     @param[in]  aID         DMA channel ID
     @param[in]  aXferReq    Pointer to DMA transfer requests array
     @param[in]  aReqSz      Size of the DMA transfer request array
     @param[in]  aCfgTbl     DMA configuration structure

     @post   None
     @retval      BCM_ERR_OK              Setup was successful
     @retval      BCM_ERR_INVAL_PARAMS    aID is invalid
     @retval      BCM_ERR_INVAL_PARAMS    aXferReq is NULL
     @retval      BCM_ERR_UNINIT          DMA driver not initialized
     @retval      BCM_ERR_NOMEM           Transfer request too big
     @code{.unparsed}
     Validate device initialized state, Channel id, XferReq and aReqSZ
     Check channel state
     if channel state is good and if the transfer type is MEM to MEM/ 
      PERF to MEM/ MEM to PERF configure the channel using DMA330_XferSetupInternal.
     @endcode

     @trace #BRCM_SWREQ_DMA330_V1_DRIVER
     @trace #BRCM_SWARCH_DMA330_XFER_SETUP_PROC
 */

int32_t DMA330_XferSetup(DMA330_ChannelIDType aID, DMA330_XferReqType *aXferReq,
        uint32_t aReqSZ, DMA330_ConfigType aCfgTbl)
{
    DMA330_ChannStateType state;
    const DMA330_ChannCfgType *chann;
    uint32_t peripXfer;
    int32_t ret = BCM_ERR_OK;

#ifdef BCM8915X_PARAM_VALIDATION
    if (0UL == DMA330_Dev.init) {
        ret = BCM_ERR_UNINIT;
    } else if ((aID > DMA330_CHANNEL_ID_MAX) || (aXferReq == NULL) ||
            (aReqSZ == 0UL)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        state = DMA330_GetChannState(aID);

        /* Is this channel configured by system? */
        if (state == DMA330_CHANN_STATE_UNINIT) {
            ret = BCM_ERR_INVAL_PARAMS;
        }
        chann = &aCfgTbl.channCfg[aID];
        peripXfer = DMA330_IsPeripXfer(chann);

        if (0UL == peripXfer) {
            if ((state != DMA330_CHANN_STATE_IDLE)
                && (state != DMA330_CHANN_STATE_ERR)
                && (state != DMA330_CHANN_STATE_SETUP)){
                ret = BCM_ERR_INVAL_STATE;
            }
        } else {
            if ((state == DMA330_CHANN_STATE_BUSY) ||
                    (state == DMA330_CHANN_STATE_ERR)) {
                ret = BCM_ERR_INVAL_STATE;
            }
        }
        if (BCM_ERR_OK == ret) {
            /*
             * set channel state busy while we are setting
             * up the descriptor and microcode
             */
            switch(chann->xferType) {
                case DMA330_XFER_TYPE_MEM_TO_MEM:
                case DMA330_XFER_TYPE_MEM_TO_PERIPH:
                case DMA330_XFER_TYPE_PERIPH_TO_MEM:
                    ret = DMA330_XferSetupInternal(aID, chann, aXferReq, aReqSZ);
                    break;
                case DMA330_XFER_TYPE_PERIPH_TO_PERIPH:
                    ret = BCM_ERR_NOSUPPORT;
                    break;
                default:
                    ret = BCM_ERR_INVAL_PARAMS;
                    break;
            }

            if (ret == BCM_ERR_OK) {
                DMA330_Dev.channel[aID].xferReqSz = aReqSZ;
                DMA330_SetChannState(aID, DMA330_CHANN_STATE_SETUP);
            } else {
                DMA330_SetChannState(aID, DMA330_CHANN_STATE_IDLE);
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Get transfer Status
   This API returns the status of the DMA last DMA transfer which
   was triggered using DMA_Mem2MemXfer.

   @behavior Sync, Non-reentrant
   @pre None

   @param[in]   aID         DMA channel ID

  @return int32_t
  Return values are documented in reverse-chronological order
  @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
  @retval     #BCM_ERR_INVAL_STATE    Device in invalid state
  @retval     #BCM_ERR_OK             Success

  @post   None

  @trace   #BRCM_SWREQ_DMA330_V1_DRIVER
  @trace   #BRCM_SWARCH_DMA330_GET_XFER_STATUS_PROC
  @code{.unparsed}
    if (0UL != DMA330_Dev.init)
        if ((aID < DMA_CHANNEL_ID_MAX)
            && (aStatus != NULL))
            state = DMA_GetChannState(aID);
            switch (state)
            case DMA_CHANN_STATE_UNINIT:
                ret = BCM_ERR_INVAL_STATE;
                *aStatus = DMA_XFER_STATUS_SUCCESS;
                break;
            case DMA_CHANN_STATE_IDLE:
                *aStatus = DMA_XFER_STATUS_SUCCESS;
                break;
            case DMA_CHANN_STATE_SETUP:
                *aStatus = DMA_XFER_STATUS_SUCCESS;
                break;
            case DMA_CHANN_STATE_BUSY:
                *aStatus = DMA_XFER_STATUS_BUSY;
                break;
            case DMA_CHANN_STATE_ERR:
                *aStatus = DMA_XFER_STATUS_ERROR;
                break;
            default:
                ret = BCM_ERR_INVAL_STATE;
                break;
        else
            ret = BCM_ERR_INVAL_PARAMS;
    else
        ret = BCM_ERR_UNINIT;
  @endcode

 @limitations None
*/

int32_t DMA330_GetXferStatus(DMA330_ChannelIDType aID,
                            DMA330_XferStatusType *aStatus)
{
    DMA330_ChannStateType state;
    int ret = BCM_ERR_OK;

    if (0UL != DMA330_Dev.init) {
        if ((aID <= DMA330_CHANNEL_ID_MAX)
            && (aStatus != NULL)) {
            state = DMA330_GetChannState(aID);
            switch (state) {
            case DMA330_CHANN_STATE_UNINIT:
                ret = BCM_ERR_INVAL_STATE;
                *aStatus = DMA330_XFER_STATUS_SUCCESS;
                break;
            case DMA330_CHANN_STATE_IDLE:
                *aStatus = DMA330_XFER_STATUS_SUCCESS;
                break;
            case DMA330_CHANN_STATE_SETUP:
                *aStatus = DMA330_XFER_STATUS_SUCCESS;
                break;
            case DMA330_CHANN_STATE_BUSY:
                *aStatus = DMA330_XFER_STATUS_BUSY;
                break;
            case DMA330_CHANN_STATE_ERR:
                *aStatus = DMA330_XFER_STATUS_ERROR;
                break;
            default:
                ret = BCM_ERR_INVAL_STATE;
                break;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_UNINIT;
    }
    return ret;
}

/** @brief Channel IRQ handler
   Sample API for channel IRQ handler.

   @behavior Sync, Non-reentrant
   @pre None

   @param[in]   aID            DMA channel ID
   @param[in]   aCfgTbl     DMA configration table

   @return uint32_t
   Return values are documented in reverse-chronological order
   @retval     #0UL   Interrupt Not Serviced
   @retval     #1UL    Interrupt Serviced

   @post   None
   @trace   #BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC
   @trace   #BRCM_SWREQ_DMA330_V1_DRIVER
   @code{.unparsed}
    intStatus = DMA330_REGS->INTMIS;
    if (aID < DMA_CHANNEL_ID_MAX)
        if ((intStatus & DMA330_CHANNEL_MASK(aID)) != 0UL)
            clear corresponding interrupt 
            channCfg = &aCfgTbl.channCfg[aID];
            DMA330_Dev.channel[aID].intCnt++;
            DMA330_Dev.channel[aID].error = 0UL;
            Disable interrupt
            DMA330_ChannStateHandler(aID , DMA330_Dev.channel[aID].error, aCfgTbl);
            if (0UL == DMA330_IsPeripXfer(channCfg))
                signalClient = 1UL;
    else
         report error

   @endcode

 @limitations None
*/
uint32_t DMA330_DrvChannIRQHandler(DMA330_ChannelIDType aID, DMA330_ConfigType aCfgTbl)
{
    uint32_t signalClient = 0UL;
    uint32_t intStatus = 0UL, intNo = 0UL;
    const DMA330_ChannCfgType *channCfg;

    intStatus = DMA330_REGS->INTMIS;

    if (1UL == DMA330_IsPeripXfer(&aCfgTbl.channCfg[aID])) {
        intNo = ((uint32_t)aCfgTbl.channCfg[aID].periphID % 18UL);
    } else {
        intNo = aID;
    }
    if (aID <= DMA330_CHANNEL_ID_MAX) {
        if ((intStatus & DMA330_CHANNEL_MASK(intNo)) != 0UL) {
            DMA330_REGS->INTCLR = 1UL << intNo;
            channCfg = &aCfgTbl.channCfg[aID];
            /* @event           CERT-C INT30
                @issue          Coverity infers that the increment would overflow
                @risk           No risk
                @justification  intCnt will be reset for every Init and based on user
                                ISR                          */
            /* coverity[cert_int30_c_violation] */
            DMA330_Dev.channel[aID].intCnt++;
            DMA330_Dev.channel[aID].error = 0UL;
            /* Disable interrupt*/
            DMA330_IntEn(0UL, (uint8_t)intNo);
            DMA330_ChannStateHandler(aID , DMA330_Dev.channel[aID].error, aCfgTbl);
            if (0UL == DMA330_IsPeripXfer(channCfg)) {
                signalClient = 1UL;
            }
        }
    } else {
        /* report error */
    }
    return signalClient;
}

/** @brief Fault IRQ handler
   Sample API for Fault IRQ handler.

   @behavior Sync, Non-reentrant
   @pre None

   @param[in]   aCfgTbl     DMA configration table

   @return uint32_t
   Return values are documented in reverse-chronological order
   @retval     0                Fault Status
   @retval     1                Fault on Channel 0

   @post   None

   @trace   #BRCM_SWARCH_DMA330_DRV_CHANN_IRQ_HANDLER_PROC
   @trace   #BRCM_SWREQ_DMA330_V1_DRIVER
   @code{.unparsed}

    faultStatus = DMA330_REGS->FSRC;
    for (i = 0UL; i < DMA_CHANNEL_ID_MAX; i++)
        if ((faultStatus & DMA330_CHANNEL_MASK(i)) != 0UL)
            DMA330_Dev.channel[i].abortCnt++;
            DMA330_Dev.channel[i].error = 1UL;
            DMA330_Kill(i);
            DMA330_ChannStateHandler(i, DMA330_Dev.channel[i].error, aCfgTbl);
    return faultStatus;

   @endcode

 @limitations None
*/

uint32_t DMA330_AbortIRQHandler(DMA330_ConfigType aCfgTbl)
{
    uint32_t i;
    uint32_t faultStatus;
    faultStatus = DMA330_REGS->FSRC;

    for (i = 0UL; i <= DMA330_CHANNEL_ID_MAX; i++) {
        if ((faultStatus & DMA330_CHANNEL_MASK(i)) != 0UL) {
            /* @event           CERT-C INT30
                @issue          Coverity infers that the increment would overflow
                @risk           No risk
                @justification  abortCnt will be reset for every Init and based on user
                                ISR                          */
            /* coverity[cert_int30_c_violation] */
            DMA330_Dev.channel[i].abortCnt++;
            DMA330_Dev.channel[i].error = 1UL;
            /* Disable interrupt*/
            if (1UL == DMA330_IsPeripXfer(&aCfgTbl.channCfg[i])) {
                DMA330_IntEn(0UL, (aCfgTbl.channCfg[i].periphID % 18U));
            } else {
                DMA330_IntEn(0UL, (uint8_t)(i & 0xFFUL));
            }
            DMA330_Kill(i);
            DMA330_ChannStateHandler(i, DMA330_Dev.channel[i].error, aCfgTbl);
        }
    }
    return faultStatus;
}

/** @} */
