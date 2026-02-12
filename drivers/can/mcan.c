/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_candrv_mcan_impl CAN Driver Design
    @ingroup grp_candrv

    @addtogroup grp_candrv_mcan_impl
    @{

    @file mcan.c
    @brief CAN driver interface implementation
    @version 0.1 Draft
*/

#include <string.h>
#include <cfg_rdb.h>
#include <can.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <compiler.h>
#include "can_rdb.h"

/**
  @name CAN Driver design IDs
  @{
  @brief Design IDs for CAN Driver
*/
#define BRCM_SWDSGN_CAN_HWINITTIMEOUTCNT_MACRO          (0xB101U)    /**< @brief #CAN_HW_INIT_TIMEOUT_CNT          */
#define BRCM_SWDSGN_CAN_HWTIMEOUTCNT_MACRO              (0xB102U)    /**< @brief #CAN_HW_TIMEOUT_CNT               */
#define BRCM_SWDSGN_CAN_MAXDATASIZE_MACRO               (0xB103U)    /**< @brief #CAN_MAX_DATA_SIZE                */
#define BRCM_SWDSGN_CAN_FDMAXDATASIZE_MACRO             (0xB104U)    /**< @brief #CAN_FD_MAX_DATA_SIZE             */
#define BRCM_SWDSGN_CAN_MCAN_LINE0ILSMASK_MACRO         (0xB105U)    /**< @brief #CAN_MCAN_LINE0_ILS_MASK          */
#define BRCM_SWDSGN_CAN_MCAN_LINE1ILSMASK_MACRO         (0xB106U)    /**< @brief #CAN_MCAN_LINE1_ILS_MASK          */
#define BRCM_SWDSGN_CAN_MCAN_MSGRAMSTDFLTRBASE_MACRO    (0xB107U)    /**< @brief #CAN_MCAN_MSGRAM_STDFLTR_BASE     */
#define BRCM_SWDSGN_CAN_MCAN_MSGRAMEXTFLTRBASE_MACRO    (0xB108U)    /**< @brief #CAN_MCAN_MSGRAM_EXTFLTR_BASE     */
#define BRCM_SWDSGN_CAN_MCAN_MSGRAMRXBUFBASE_MACRO      (0xB109U)    /**< @brief #CAN_MCAN_MSGRAM_RXBUF_BASE       */
#define BRCM_SWDSGN_CAN_MCAN_MSGRAMRXFIFO0BASE_MACRO    (0xB10AU)    /**< @brief #CAN_MCAN_MSGRAM_RXFIFO0_BASE     */
#define BRCM_SWDSGN_CAN_MCAN_MSGRAMRXFIFO1BASE_MACRO    (0xB10BU)    /**< @brief #CAN_MCAN_MSGRAM_RXFIFO1_BASE     */
#define BRCM_SWDSGN_CAN_MCAN_MSGRAMTXEVENTBASE_MACRO    (0xB10CU)    /**< @brief #CAN_MCAN_MSGRAM_TXEVENTFIFO_BASE */
#define BRCM_SWDSGN_CAN_MCAN_MSGRAMTXBUFBASE_MACRO      (0xB10DU)    /**< @brief #CAN_MCAN_MSGRAM_TXBUF_BASE       */
#define BRCM_SWDSGN_CAN_CBACTION_TYPE                   (0xB10EU)    /**< @brief #CAN_CbActionType                 */
#define BRCM_SWDSGN_CAN_MCAN_STATE_TYPE                 (0xB10FU)    /**< @brief #CAN_McanStateType                */
#define BRCM_SWDSGN_CAN_RXPKTINFO_TYPE                  (0xB110U)    /**< @brief #CAN_RxPktInfoType                */
#define BRCM_SWDSGN_CAN_TXPKTINFO_TYPE                  (0xB111U)    /**< @brief #CAN_TxPktInfoType                */
#define BRCM_SWDSGN_CAN_ERRINFO_TYPE                    (0xB112U)    /**< @brief #CAN_ErrInfoType                  */
#define BRCM_SWDSGN_CAN_MCAN_RWDEV_TYPE                 (0xB113U)    /**< @brief #CAN_McanRWDevType                */
#define BRCM_SWDSGN_CAN_MCAN_REGS_TYPE                  (0xB114U)    /**< @brief #CAN_McanRegsType                 */
#define BRCM_SWDSGN_CAN_MCAN_RWDEV_GLOBAL               (0xB115U)    /**< @brief #CAN_McanRWDev                    */
#define BRCM_SWDSGN_CAN_MCAN_REGS_GLOBAL                (0xB116U)    /**< @brief #CAN_McanRegs                     */
#define BRCM_SWDSGN_CAN_MCAN_REPORTERROR_PROC           (0xB117U)    /**< @brief #CAN_McanReportError              */
#define BRCM_SWDSGN_CAN_MCAN_RESETINTERNALVARS_PROC     (0xB118U)    /**< @brief #CAN_McanResetInternalVars        */
#define BRCM_SWDSGN_CAN_MCAN_SETINIT_PROC               (0xB119U)    /**< @brief #CAN_McanSetInit                  */
#define BRCM_SWDSGN_CAN_MCAN_ENALLINTERRUPTS_PROC       (0xB11AU)    /**< @brief #CAN_McanEnAllInterrupts          */
#define BRCM_SWDSGN_CAN_MCAN_RESETCCE_PROC              (0xB11BU)    /**< @brief #CAN_McanResetCCE                 */
#define BRCM_SWDSGN_CAN_MCAN_RESETINIT_PROC             (0xB11CU)    /**< @brief #CAN_McanResetInit                */
#define BRCM_SWDSGN_CAN_MCAN_DISALLINTERRUPTS_PROC      (0xB11DU)    /**< @brief #CAN_McanDisAllInterrupts         */
#define BRCM_SWDSGN_CAN_MCAN_DLCTOMSGLEN_PROC           (0xB11EU)    /**< @brief #CAN_McanDlcToMsgLen              */
#define BRCM_SWDSGN_CAN_MCAN_DATASZTOELEMSZ_PROC        (0xB11FU)    /**< @brief #CAN_McanDataSzToElemSz           */
#define BRCM_SWDSGN_CAN_MCAN_CONFIGFILTERMASKS_PROC     (0xB120U)    /**< @brief #CAN_McanCfgFilterMasks           */
#define BRCM_SWDSGN_CAN_MCAN_SETMODE_PROC               (0xB121U)    /**< @brief #CAN_McanSetMode                  */
#define BRCM_SWDSGN_CAN_MCAN_CANCELTXALL_PROC           (0xB122U)    /**< @brief #CAN_McanCancelTxAll              */
#define BRCM_SWDSGN_CAN_MCAN_TXBUFWRITE_PROC            (0xB123U)    /**< @brief #CAN_McanTxBufWrite               */
#define BRCM_SWDSGN_CAN_MCAN_FIFOQUEUEWRITE_PROC        (0xB124U)    /**< @brief #CAN_McanFifoQueueWrite           */
#define BRCM_SWDSGN_CAN_MCAN_SETCCE_PROC                (0xB125U)    /**< @brief #CAN_McanSetCCE                   */
#define BRCM_SWDSGN_CAN_MCAN_CFGLOBALFILTERS_PROC       (0xB126U)    /**< @brief #CAN_McanCfgGlobalFilters         */
#define BRCM_SWDSGN_CAN_MCAN_SETBAUDRATE_PROC           (0xB127U)    /**< @brief #CAN_McanSetBaudrate              */
#define BRCM_SWDSGN_CAN_MCAN_CONFIGURETXRX_PROC         (0xB128U)    /**< @brief #CAN_McanConfigureTxRx            */
#define BRCM_SWDSGN_CAN_MCAN_UPDATERXFLAGS_PROC         (0xB129U)    /**< @brief #CAN_McanUpdateRxFlags            */
#define BRCM_SWDSGN_CAN_MCAN_PROCESSRXBUFFER_PROC       (0xB12AU)    /**< @brief #CAN_McanProcessRxBuffer          */
#define BRCM_SWDSGN_CAN_MCAN_PROCESSRXFIFO_PROC         (0xB12BU)    /**< @brief #CAN_McanProcessRxFIFO            */
#define BRCM_SWDSGN_CAN_MCAN_PROCESSCANRX_PROC          (0xB12CU)    /**< @brief #CAN_McanProcessCanRx             */
#define BRCM_SWDSGN_CAN_MCAN_MESSAGERAM_INIT_PROC       (0xB12DU)    /**< @brief #CAN_McanMsgRAMInit               */
#define BRCM_SWDSGN_CAN_MCAN_PROCESSTX_PROC             (0xB12EU)    /**< @brief #CAN_McanProcessTx                */
#define BRCM_SWDSGN_CAN_MCAN_PROCESSERROR_PROC          (0xB12FU)    /**< @brief #CAN_McanProcessError             */
#define BRCM_SWDSGN_CAN_MCAN_PROCESSCANTX_PROC          (0xB130U)    /**< @brief #CAN_McanProcessCanTx             */
#define BRCM_SWDSGN_CAN_MCAN_PROCESSCANERROR_PROC       (0xB131U)    /**< @brief #CAN_McanProcessCanError          */
#define BRCM_SWDSGN_CAN_MCAN_GETERRCNTS_PROC            (0xB132U)    /**< @brief #CAN_McanGetErrCnts               */
#define BRCM_SWDSGN_CAN_MCAN_GETERRCODE_PROC            (0xB133U)    /**< @brief #CAN_McanGetErrCode               */
#define BRCM_SWDSGN_CAN_MCAN_ISBUSOFF_PROC              (0xB134U)    /**< @brief #CAN_McanIsBusOff                 */
#define BRCM_SWDSGN_CAN_MCAN_BUSOFFRECOVERY_PROC        (0xB135U)    /**< @brief #CAN_McanBusOffRecovery           */
#define BRCM_SWDSGN_CAN_MCAN_GETRXMSG_PROC              (0xB136U)    /**< @brief #CAN_McanGetRxMsg                 */
#define BRCM_SWDSGN_CAN_MCAN_DEQUEUERXPKT_PROC          (0xB137U)    /**< @brief #CAN_McanDeQueueRxPkt             */
#define BRCM_SWDSGN_CAN_MCAN_GETTXMSG_PROC              (0xB138U)    /**< @brief #CAN_McanGetTxMsg                 */
#define BRCM_SWDSGN_CAN_MCAN_WRITE_PROC                 (0xB139U)    /**< @brief #CAN_McanWrite                    */
#define BRCM_SWDSGN_CAN_MCAN_SLEEP_PROC                 (0xB13AU)    /**< @brief #CAN_McanSleep                    */
#define BRCM_SWDSGN_CAN_MCAN_WAKEUP_PROC                (0xB13BU)    /**< @brief #CAN_McanWakeUp                   */
#define BRCM_SWDSGN_CAN_MCAN_DEINIT_PROC                (0xB13CU)    /**< @brief #CAN_McanDeInit                   */
#define BRCM_SWDSGN_CAN_MCAN_INIT_PROC                  (0xB13DU)    /**< @brief #CAN_McanInit                     */
#define BRCM_SWDSGN_CAN_DRV_INIT_PROC                   (0xB140U)    /**< @brief #CAN_DrvInit              */
#define BRCM_SWDSGN_CAN_DRV_DEINIT_PROC                 (0xB141U)    /**< @brief #CAN_DrvDeInit            */
#define BRCM_SWDSGN_CAN_DRV_WRITE_PROC                  (0xB142U)    /**< @brief #CAN_DrvWrite             */
#define BRCM_SWDSGN_CAN_DRV_SLEEP_PROC                  (0xB143U)    /**< @brief #CAN_DrvSleep             */
#define BRCM_SWDSGN_CAN_DRV_WAKEUP_PROC                 (0xB144U)    /**< @brief #CAN_DrvWakeUp            */
#define BRCM_SWDSGN_CAN_DRV_GETERRCNTS_PROC             (0xB145U)    /**< @brief #CAN_DrvGetErrCnts        */
#define BRCM_SWDSGN_CAN_IRQLINE0HANDLER_PROC            (0xB146U)    /**< @brief #CAN_IRQLine0Handler         */
#define BRCM_SWDSGN_CAN_IRQLINE1HANDLER_PROC            (0xB147U)    /**< @brief #CAN_IRQLine1Handler         */
#define BRCM_SWDSGN_CAN_DRV_PROCESSPACKETS_PROC         (0xB148U)    /**< @brief #CAN_DrvProcessPkts          */
#define BRCM_SWDSGN_CAN_DRV_ENABLE_INTERRUPT_PROC       (0xB149U)    /**< @brief #CAN_DrvEnableInterrupt      */
#define BRCM_SWDSGN_CAN_DRV_CLEAR_INTERRUPT_PROC        (0xB14AU)    /**< @brief #CAN_DrvClearInterrupt          */
#define BRCM_SWDSGN_CAN_DRV_GET_INTERRUPT_STATUS_PROC   (0xB14BU)    /**< @brief #CAN_DrvGetInterruptStatus          */
#define BRCM_SWDSGN_CAN_INT_POWERUP_OUTRESET_PROC       (0xB14CU)    /**< @brief #CAN_IntPowerUpOutReset */
#define BRCM_SWDSGN_CAN_INT_INRESET_PROC                (0xB14DU)    /**< @brief #CAN_IntInReset */
#define BRCM_SWDSGN_CAN_CLZ_PROC                        (0xB14EU)    /**< @brief #CAN_Clz */
#define BRCM_SWDSGN_CAN_CFG_REGS_GLOBAL                 (0xB14FU)    /**< @brief #CAN_CFG_Regs                 */
#define BRCM_SWDSGN_CAN_GET_RXMSG_PROC                  (0xB150U)    /**< @brief #CAN_GetRxMsg  */
#define BRCM_SWDSGN_CAN_PUT_RXPKT_PROC                  (0xB151U)    /**< @brief #CAN_PutRxPkt */
#define BRCM_SWDSGN_CAN_PROCESS_RCVDPKTS_PROC           (0xB152U)    /**< @brief #CAN_ProcessRcvdPkts */
#define BRCM_SWDSGN_CAN_PROCESS_XMTD_PKTS_PROC          (0xB153U)    /**< @brief #CAN_ProcessXmtdPkts */
#define BRCM_SWDSGN_CAN_PROCESS_ERR_PKTS_PROC           (0xB154U)    /**< @brief #CAN_ProcessErrPkts */
#define BRCM_SWDSGN_CAN_PROCESS_BUSOFF_PROC             (0xB155U)    /**< @brief #CAN_ProcessBusOff */

/** @} */


/**
    @brief CAN configuration register

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
static CFG_RDBType * const CAN_CFG_Regs = (CFG_RDBType *)CFG_BASE;


/**
    @brief CAN CCCR INIT timeout polling counter

    @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
#define CAN_HW_INIT_TIMEOUT_CNT             (10000UL)

/**
    @brief CAN hardware timeout polling counter

    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
#define CAN_HW_TIMEOUT_CNT                  (1000UL)

/**
    @brief CAN packet max length

    @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_COMMUNICATION
*/
#define CAN_MAX_DATA_SIZE                   (8UL)

/**
    @brief CAN FD packet max length

    @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
#define CAN_FD_MAX_DATA_SIZE                (64UL)

/**
    @brief MCAN Line0 interrupt select mask

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
#define CAN_MCAN_LINE0_ILS_MASK                                                 \
    (~(CAN_0_ILS_RF0NL_MASK | CAN_0_ILS_RF0WL_MASK | CAN_0_ILS_RF0FL_MASK       \
       | CAN_0_ILS_RF0LL_MASK | CAN_0_ILS_RF1NL_MASK | CAN_0_ILS_RF1WL_MASK     \
       | CAN_0_ILS_RF1FL_MASK | CAN_0_ILS_RF1LL_MASK | CAN_0_ILS_HPML_MASK      \
       | CAN_0_ILS_TCL_MASK | CAN_0_ILS_TCFL_MASK | CAN_0_ILS_TFEL_MASK         \
       | CAN_0_ILS_TEFNL_MASK | CAN_0_ILS_TEFWL_MASK | CAN_0_ILS_TEFFL_MASK     \
       | CAN_0_ILS_TEFLL_MASK | CAN_0_ILS_DRXL_MASK)) /**< @brief LIN0_INT_MASK */

/**
    @brief MCAN Line1 interrupt select mask

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
#define CAN_MCAN_LINE1_ILS_MASK                                         \
    (CAN_0_ILS_TSWL_MASK | CAN_0_ILS_MRAFL_MASK | CAN_0_ILS_TOOL_MASK   \
     | CAN_0_ILS_BECL_MASK | CAN_0_ILS_BEUL_MASK | CAN_0_ILS_ELOL_MASK  \
     | CAN_0_ILS_EPL_MASK | CAN_0_ILS_EWL_MASK | CAN_0_ILS_BOL_MASK     \
     | CAN_0_ILS_WDIL_MASK | CAN_0_ILS_PEAL_MASK | CAN_0_ILS_PEDL_MASK  \
     | CAN_0_ILS_ARAL_MASK) /**< @brief LIN1_INT_MASK */

/**
    @brief MCAN MSGRAM standard filter base

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
#define CAN_MCAN_MSGRAM_STDFLTR_BASE(aHwIdx)                            \
    (volatile uint32_t *)((uintptr_t)(CAN_McanRegs[aHwIdx].MSGRAM                   \
        + CAN_McanRWDev[aHwIdx].cfgData->msgRAMCfg.stdFltrCfg.startAddr))

/**
    @brief MCAN MSGRAM extended standard filter base

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
#define CAN_MCAN_MSGRAM_EXTFLTR_BASE(aHwIdx)                            \
    (volatile uint32_t *)((uintptr_t)(CAN_McanRegs[aHwIdx].MSGRAM                   \
        + CAN_McanRWDev[aHwIdx].cfgData->msgRAMCfg.extFltrCfg.startAddr))

/**
    @brief MCAN MSGRAM Rx buffer base

    @trace  #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
#define CAN_MCAN_MSGRAM_RXBUF_BASE(aHwIdx)                              \
    (volatile uint32_t *)((uintptr_t)(CAN_McanRegs[aHwIdx].MSGRAM                   \
        + CAN_McanRWDev[aHwIdx].cfgData->msgRAMCfg.rxBufCfg.startAddr))

/**
    @brief MCAN MSGRAM Rx FIFO0 base

    @trace  #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
#define CAN_MCAN_MSGRAM_RXFIFO0_BASE(aHwIdx)                            \
    (volatile uint32_t *)((uintptr_t)(CAN_McanRegs[aHwIdx].MSGRAM                   \
        + CAN_McanRWDev[aHwIdx].cfgData->msgRAMCfg.fifo0Cfg.startAddr))

/**
    @brief MCAN MSGRAM Rx FIFO1 base

    @trace  #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
#define CAN_MCAN_MSGRAM_RXFIFO1_BASE(aHwIdx)                            \
    (volatile uint32_t *)((uintptr_t)(CAN_McanRegs[aHwIdx].MSGRAM                   \
        + CAN_McanRWDev[aHwIdx].cfgData->msgRAMCfg.fifo1Cfg.startAddr))

/**
    @brief MCAN MSGRAM TX event FIFO base

    @trace  #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
#define CAN_MCAN_MSGRAM_TXEVENTFIFO_BASE(aHwIdx)                        \
    (volatile uint32_t *)((uintptr_t)(CAN_McanRegs[aHwIdx].MSGRAM                   \
        + CAN_McanRWDev[aHwIdx].cfgData->msgRAMCfg.txEventCfg.startAddr))

/**
    @brief MCAN MSGRAM Tx buffer base

    @trace  #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
#define CAN_MCAN_MSGRAM_TXBUF_BASE(aHwIdx)                              \
    (uint32_t *)((uintptr_t)(CAN_McanRegs[aHwIdx].MSGRAM                            \
        + CAN_McanRWDev[aHwIdx].cfgData->msgRAMCfg.txBufCfg.startAddr))

/**
    @name CAN_CbActionType
    @{
    @brief Packet action types

    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_COMMUNICATION
*/
typedef uint32_t CAN_CbActionType;        /**< @brief Callback action type  */
#define CAN_CBACTION_DISABLED  (0UL)      /**< @brief callback disabled     */
#define CAN_CBACTION_ENABLED   (1UL)      /**< @brief callback enabled      */
/** @} */

/**
    @name CAN_McanStateType
    @{
    @brief MCAN states

    @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_DRV_GETERRCNTS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
typedef uint32_t CAN_McanStateType;         /**< @brief MCAN state type         */
#define CAN_MCAN_STATE_UNINIT        (0UL)  /**< @brief CAN_MCAN_STATE_UNINIT   */
#define CAN_MCAN_STATE_STOPPED       (1UL)  /**< @brief CAN_MCAN_STATE_STOPPED  */
#define CAN_MCAN_STATE_RUNNING       (2UL)  /**< @brief CAN_MCAN_STATE_RUNNING  */
#define CAN_MCAN_STATE_SLEEP         (3UL)  /**< @brief CAN_MCAN_STATE_SLEEP    */
/** @} */

/**
    @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_GETERRCNTS_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
typedef struct sCAN_McanRegsType {
    CAN_RDBType *REGS;              /**< @brief CAN RDB */
    uint32_t    MSGRAM;             /**< @brief MSGRAM  */
} CAN_McanRegsType;

/**
    @brief RX packet information

    Structure for definition of RX packet buffers

    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
typedef struct sCAN_RxPktInfoType {
    CAN_RxMsgObjType    rxMsg;                          /**< @brief RX message information  */
    uint32_t            cbEn;                           /**< @brief RX notification callback
                                                                    enabled/pending flag    */
    uint8_t             rxBuf[CAN_FD_MAX_DATA_SIZE];    /**< @brief RX Data Buffer          */
} CAN_RxPktInfoType;

/**
    @brief TX packet infomation

    Structure for definition of TX packet buffers

    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
typedef struct sCAN_TxPktInfoType {
    CAN_TxMsgObjType txMsgObj;          /**< @brief TX message object                   */
    uint32_t         cbEn;              /**< @brief TX completion notiication callback
                                                    enabled/pending                     */
} CAN_TxPktInfoType;

/**
    @brief CAN error message

    Structure for definition of error messages

    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
typedef struct sCAN_ErrInfoType {
    CAN_ErrorType   errorCode;          /**< @brief Error code                  */
    uint32_t        cbEn;               /**< @bruef Error notiifcation callback
                                                    enabled/pending flag        */
} CAN_ErrInfoType;

/**
    @brief Driver private data (one instance of this structure for
    each CAN controller)

    @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_GETERRCNTS_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
typedef struct sCAN_McanRWDevType {
     CAN_McanStateType      state;                                      /**< @brief CAN state           */
     uint32_t               busOff;                                     /**< @brief busOff              */
     const CAN_ConfigType   *cfgData;                                   /**< @brief cfgData             */
     uint32_t               rxPktQueueIdx;                              /**< @brief rxPktQueueIdx       */
     uint32_t               rxPktDeQueueIdx;                            /**< @brief rxPktDeQueueIdx     */
     uint32_t               txPktQueueIdx;                              /**< @brief txPktQueueIdx       */
     uint32_t               txPktDeQueueIdx;                            /**< @brief txPktDeQueueIdx     */
     uint32_t               errQueueIdx;                                /**< @brief errQueueIdx         */
     uint32_t               errDeQueueIdx;                              /**< @brief errDeQueueIdx       */
     uint32_t               msgMarkerIdx;                               /**< @brief msgMarkerIdx        */
     uint8_t                stdFilterIdx;                               /**< @brief stdFilterIdx        */
     uint8_t                extFilterIdx;                               /**< @brief extFilterIdx        */
     CAN_RxPktInfoType      rxPackets[CAN_RXPKTQ_SIZE];                 /**< @brief Rx Packets Buffer   */
     CAN_TxPktInfoType      txPackets[CAN_MSGRAM_MAX_TX_BUFFER_ELEMS];  /**< @brief Tx Packets Buffer   */
     CAN_ErrInfoType        errors[CAN_ERRPKTQ_SIZE];                   /**< @brief Error Buffer        */
} CAN_McanRWDevType;


/**
    @brief CAN Driver Read/Write global structure

    @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_GETERRCNTS_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
static CAN_McanRWDevType CAN_McanRWDev[CAN_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_GETERRCNTS_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
static const CAN_McanRegsType CAN_McanRegs[CAN_MAX_HW_ID] =
{
#if (CAN_MAX_HW_ID == 0)
    #error "CAN_MAX_HW_ID == 0"
#endif
    {
        .REGS = (CAN_RDBType *)CAN_CNTRL_BASE,
        .MSGRAM = CAN_MSGRAM_BASE,
    },
#if (CAN_MAX_HW_ID > 1)
    #error "CAN_MAX_HW_ID > 1 is not supported by the driver"
#endif
};

/**
    @brief  Reporting error function

    @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace #BRCM_SWARCH_CAN_DRV_GETERRCNTS_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
static void CAN_McanReportError(uint8_t aInstanceID, uint16_t aApiID, int32_t aErr,
                                    uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    /* Reporting error : TBD */
    /*const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CAN_ID, aInstanceID, aApiID, aErr, 4UL, values);*/
}

/** @brief Count of leading zeroes

    returns the count of leading zeroes

    @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace #BRCM_SWREQ_CAN_INIT

    @code{.unparsed}
    uint32_t idx
    for (idx = 0UL; idx < 32UL; idx++)
        if ((aValue & (1UL << (31UL - idx))) > 0UL)
            break
    return idx
    @endcode
*/
static uint32_t CAN_Clz(uint32_t aValue)
{
    uint32_t idx = 0UL;
    for (idx = 0UL; idx < 32UL; idx++) {
        if ((aValue & (1UL << (31UL - idx))) > 0UL) {
            break;
        }
    }
    return idx;
}

/**
  @brief Reset internal variables

  @param[in]    Id  Index of the MCAN Controller

  @return       void

  @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
  @trace  #BRCM_SWREQ_CAN_INIT
*/
static void CAN_McanResetInternalVars(CAN_HwIDType aID)
{
    CAN_McanRWDev[aID].busOff = 0UL;
    CAN_McanRWDev[aID].rxPktQueueIdx = 0UL;
    CAN_McanRWDev[aID].rxPktDeQueueIdx = 0UL;
    CAN_McanRWDev[aID].txPktQueueIdx = 0UL;
    CAN_McanRWDev[aID].txPktDeQueueIdx = 0UL;
    CAN_McanRWDev[aID].errQueueIdx = 0UL;
    CAN_McanRWDev[aID].errDeQueueIdx = 0UL;
    CAN_McanRWDev[aID].msgMarkerIdx = 0UL;
    CAN_McanRWDev[aID].stdFilterIdx = 0U;
    CAN_McanRWDev[aID].extFilterIdx = 0U;
}

/** @brief  Power up and bring out of reset CAN peropheral

    param[in]   aId  Index of the CAN port

    return      void
    @trace  #BRCM_SWREQ_CAN_INIT
    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
 */
static int32_t CAN_IntPowerUpOutReset(CAN_HwIDType aId)
{
    int32_t retVal = BCM_ERR_OK;
    int32_t counter = 1000;
    uint32_t mem_status = 0UL;

    /* Bring peripheral out of reset */
    /* @event            CERT-C INT30
    issue            Coverity infers that 26UL + aId can wrap
    risk             No risk
    justification    aId is checked in calling function for its limit so it will not wrap. */
    /* coverity[cert_int30_c_violation] */
    /* coverity[cert_int34_c_violation] */
    CAN_CFG_Regs->periph_reset |= (1UL << (CFG_PERIPH_RESET_CAN_SHIFT + aId));
    BCM_CpuNDelay(10000UL);
    /* Mem init */
    CAN_CFG_Regs->misc_ctrl |= CFG_MISC_CTRL_CAN_MEM_INIT_EN_MASK;
    do {
        mem_status= ((CAN_CFG_Regs->misc_status & CFG_MISC_STATUS_CAN_MEM_INIT_DONE_STATUS_MASK) >> CFG_MISC_STATUS_CAN_MEM_INIT_DONE_STATUS_SHIFT);
        BCM_CpuNDelay(1000UL);
        counter--;
    } while((mem_status == 0UL) && (counter > 0));

    if((counter == 0) && (mem_status == 0UL)) {
        retVal = BCM_ERR_TIME_OUT;
    }
    return retVal;

}

/** @brief Bring CAN peripheral in reset

    param[in]   aId  Index of the CAN port

    return      void
    @trace  #BRCM_SWREQ_CAN_INIT
    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
 */
static void CAN_IntInReset(CAN_HwIDType aId)
{
    /* Bring peripheral in reset */
    /* @event            CERT-C INT30
    issue            Coverity infers that 26UL + aId can wrap
    risk             No risk
    justification    aId is checked in calling function for its limit so it will not wrap. */
    /* coverity[cert_int30_c_violation] */
    /* coverity[cert_int34_c_violation] */
    CAN_CFG_Regs->periph_reset &= (uint32_t)(~(1UL << (CFG_PERIPH_RESET_CAN_SHIFT + aId)));
}

/**
    @brief  Sets CCCR Init bit

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
static int32_t CAN_McanSetInit(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t timeout = CAN_HW_INIT_TIMEOUT_CNT;

    if (0UL == (CAN_McanRegs[aID].REGS->cccr & CAN_0_CCCR_INIT_MASK)) {
        CAN_McanRegs[aID].REGS->cccr |= CAN_0_CCCR_INIT_MASK;
        while (0UL < timeout) {
            if (0UL != (CAN_McanRegs[aID].REGS->cccr & CAN_0_CCCR_INIT_MASK)) {
                break;
            }
            timeout--;
        }
        if (0UL == timeout) {
            retVal = BCM_ERR_TIME_OUT;
            CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_SETINIT_PROC, retVal,
                                0UL, 0UL, 0UL, 0UL);
        }
    }
    return retVal;
}

/**
    @brief Enable all the CAN interrupts.

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
static void CAN_McanEnAllInterrupts(CAN_HwIDType aID)
{
    CAN_McanRegs[aID].REGS->ie = CAN_IE_ALL_MASK;
    CAN_McanRegs[aID].REGS->ils |= ( CAN_MCAN_LINE1_ILS_MASK | CAN_MCAN_LINE0_ILS_MASK );
    CAN_McanRegs[aID].REGS->ile |= ( CAN_0_ILE_EINT0_MASK | CAN_0_ILE_EINT1_MASK );
}

/**
    @brief Configuration change enable reset

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
static int32_t CAN_McanResetCCE(CAN_HwIDType aID)
{
    int32_t ret = BCM_ERR_OK;
    ret = CAN_McanSetInit(aID);

    if (BCM_ERR_OK == ret) {
        CAN_McanRegs[aID].REGS->cccr &= ~CAN_0_CCCR_CCE_MASK;
    }

    return ret;
}

/**
    @brief MCAN reset

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
static int32_t CAN_McanResetInit(CAN_HwIDType aID)
{
    uint32_t timeout = CAN_HW_INIT_TIMEOUT_CNT;
    int32_t retVal = BCM_ERR_OK;

    CAN_McanRegs[aID].REGS->cccr &= ~CAN_0_CCCR_INIT_MASK;
    while (0UL < timeout) {
        if (0UL == (CAN_McanRegs[aID].REGS->cccr & CAN_0_CCCR_INIT_MASK)) {
            break;
        }
        timeout--;
    }
    if (0UL == timeout) {
        retVal = BCM_ERR_TIME_OUT;
        CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_RESETINIT_PROC, retVal,
                            0UL, 0UL, 0UL, 0UL);
    }

    return retVal;
}

/**
    @brief MCAN disable all the interrupts

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
static void CAN_McanDisAllInterrupts(CAN_HwIDType aID)
{
    /* Disable Interrupts for both lines */
    CAN_McanRegs[aID].REGS->ile = (uint32_t)~(CAN_0_ILE_EINT0_MASK | CAN_0_ILE_EINT1_MASK);
    CAN_McanRegs[aID].REGS->ie = (uint32_t)~(CAN_IE_ALL_MASK);
}

/**
  @brief Conversion of MCAN DLC code to message length

  @param[in]    DLC      DLC Code
  @param[in]    aFDFCfg     CAN message format type

  @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
  @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
  @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
  @trace #BRCM_SWREQ_CAN_COMMUNICATION

  @return       void
*/
static uint32_t CAN_McanDlcToMsgLen(uint32_t aCanDLC, CAN_MsgFormatType aFDFCfg)
{
    uint32_t msgLength;

    if ((CAN_MSGFORMAT_CANFD != aFDFCfg) && (CAN_MAX_DATA_SIZE < aCanDLC)) {
        msgLength = 8UL;   /* For Standard CAN Message if DLC greater than 8, data length set to 8 as per CAN specification */
    } else {
        switch (aCanDLC) {
        case 9:
            msgLength = 12UL;
            break;
        case 10:
            msgLength = 16UL;
            break;
        case 11:
            msgLength = 20UL;
            break;
        case 12:
            msgLength = 24UL;
            break;
        case 13:
            msgLength = 32UL;
            break;
        case 14:
            msgLength = 48UL;
            break;
        case 15:
            msgLength = 64UL;
            break;
        default:
            msgLength = aCanDLC;
            break;
        }
    }

    return msgLength;
}

/**
 @brief Conversion of Element Data Size to Message Length

 @param[in]    aSize     Element Data Size

 @trace #BRCM_SWARCH_CAN_DRV_WRITE_PROC
 @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
 @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
 @trace #BRCM_SWREQ_CAN_COMMUNICATION

 @return       void
*/

static uint32_t CAN_McanDataSzToElemSz(CAN_MsgRAMDataSzType aSize)
{
    uint32_t elemSz;

    switch (aSize) {
    case CAN_MSGRAMDATASZ_8:
        elemSz = 8UL;
        break;
    case CAN_MSGRAMDATASZ_12:
        elemSz = 12UL;
        break;
    case CAN_MSGRAMDATASZ_16:
        elemSz = 16UL;
        break;
    case CAN_MSGRAMDATASZ_20:
        elemSz = 20UL;
        break;
    case CAN_MSGRAMDATASZ_24:
        elemSz = 24UL;
        break;
    case CAN_MSGRAMDATASZ_32:
        elemSz = 32UL;
        break;
    case CAN_MSGRAMDATASZ_48:
        elemSz = 48UL;
        break;
    default:
        elemSz = 64UL;
        break;
    }
    return elemSz;
}

/**
  @brief Configures individual filter masks

  @param[in]    Id              Index of the MCAN Controller
  @param[in]    filterObj    Pointer to configured filter object
  @param[in]    filterConfig    Filter Element Configuration
                             Values:
                             1: Store in Rx FIFO 0 if Filter matches
                             2: Store in Rx FIFO 1 if Filter matches
                             7: Store in Rx Buffer

  @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
  @trace  #BRCM_SWREQ_CAN_INIT

  @return       void
*/
static void CAN_McanCfgFilterMasks(CAN_HwIDType aID,
                                    const CAN_RxFilterObjType * const filterObj,
                                    uint32_t filterConfig,
                                    const CAN_MsgRAMCfgType * const msgRAMCfgPtr)
{
    volatile uint32_t *filterElem;

    /* @event         CERT INT30-C
         @issue         Coverity infers that the unsigned integer addition may wrap
         @risk           No risk
         @justification Start address plus number of elements is compare with msgram size.
                             It will not excedd the msgram size.
      */

    if (CAN_MSGID_STANDARD == filterObj->msgIDType) {
        /* coverity[cert_int30_c_violation] */
        filterElem = CAN_MCAN_MSGRAM_STDFLTR_BASE(aID);

        filterElem[CAN_McanRWDev[aID].stdFilterIdx] = ( (filterObj->filterType << 30UL)
                                                            | (filterConfig << 27UL)
                                                            | (filterObj->filterID1 << 16UL)
                                                            | (filterObj->filterID2) );
        /* coverity[cert_int30_c_violation] */
        if (CAN_McanRWDev[aID].stdFilterIdx >= (msgRAMCfgPtr->stdFltrCfg.numElements - 1U)) {
            CAN_McanRWDev[aID].stdFilterIdx = 0U;
        } else {
            /* coverity[cert_int30_c_violation] */
            CAN_McanRWDev[aID].stdFilterIdx++;
        }
    } else {
        /* coverity[cert_int30_c_violation] */
        filterElem = CAN_MCAN_MSGRAM_EXTFLTR_BASE(aID);
        /* coverity[cert_int30_c_violation] */
        filterElem[CAN_McanRWDev[aID].extFilterIdx++] = ( (filterConfig << 29UL)
                                                            | (filterObj->filterID1) );
        /* coverity[cert_int30_c_violation] */
        filterElem[CAN_McanRWDev[aID].extFilterIdx] = ( (filterObj->filterType << 30UL)
                                                            | (filterObj->filterID2) );
        /* coverity[cert_int30_c_violation] */
        if (CAN_McanRWDev[aID].extFilterIdx >= ((msgRAMCfgPtr->extFltrCfg.numElements << 1UL) - 1U)) {
            CAN_McanRWDev[aID].extFilterIdx = 0U;
        } else {
            /* coverity[cert_int30_c_violation] */
            CAN_McanRWDev[aID].extFilterIdx++;
        }
    }
}

/**
  @brief This API is used to change the state of MCAN Driver / Controller

  @param[in]    aID      Index of the MCAN Controller
  @param[in]    state    new state to set for CAN Driver / Controller

  @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
  @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
  @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
  @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
  @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
  @trace  #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
  @trace  #BRCM_SWREQ_CAN_INIT

  @return       void
 */
static int32_t CAN_McanSetMode(CAN_HwIDType aID, CAN_McanStateType aState)
{
    int32_t retVal = BCM_ERR_OK;

    if (CAN_MAX_HW_ID <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        switch (aState) {
        case CAN_MCAN_STATE_STOPPED:
            CAN_McanRWDev[aID].state = CAN_MCAN_STATE_STOPPED;
            /* Set cccr.INIT */
            retVal = CAN_McanSetInit(aID);
            break;
        case CAN_MCAN_STATE_RUNNING:
            CAN_McanRWDev[aID].state = CAN_MCAN_STATE_RUNNING;
            CAN_McanEnAllInterrupts(aID);
            /* Reset cccr.CCE */
            retVal = CAN_McanResetCCE(aID);
            if (BCM_ERR_OK == retVal) {
                /* Reset cccr.INIT */
                retVal = CAN_McanResetInit(aID);
            }
            break;
        case CAN_MCAN_STATE_SLEEP:
            CAN_McanRWDev[aID].state = CAN_MCAN_STATE_SLEEP;
            CAN_McanDisAllInterrupts(aID);
            break;
        case CAN_MCAN_STATE_UNINIT:
            CAN_McanRWDev[aID].state = CAN_MCAN_STATE_UNINIT;
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
        }
    }

    return retVal;
}

/**
  @brief This API is used to abort transmission of ALL pending CAN Messages

  @param[in]    Id      Index of the MCAN Controller

  @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
  @trace  #BRCM_SWARCH_CAN_DRV_WRITE_PROC
  @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
  @trace  #BRCM_SWREQ_CAN_INIT

  @return       int32_t Error code
 */
static int32_t CAN_McanCancelTxAll(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_CANCELLED;
    uint32_t pendingMask;
    uint32_t timeout = CAN_HW_TIMEOUT_CNT;

    /* Cancellation of message in Tx Buffer */
    pendingMask = CAN_McanRegs[aID].REGS->txbrp;
    CAN_McanRegs[aID].REGS->txbcr = pendingMask;

    while (0UL < timeout) {
        if (0UL == ((CAN_McanRegs[aID].REGS->txbcf) ^ pendingMask)) {
            break;
        }
        timeout--;
    }
    if (0UL == timeout) {
        retVal = BCM_ERR_TIME_OUT;
    }

    return retVal;
}

/**
    @brief Write TX Data to MCAN Tx buffer

    @trace  #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_McanTxBufWrite(CAN_HwIDType aID, CAN_TxMsgObjType * const TxObj)
{
    uint8_t i;
    uint32_t j;
    CAN_MsgFormatType fdf_config;
    uint32_t brs_config;
    uint32_t efc_config = 0x1UL;
    uint32_t *txElem;
    uint32_t id_mask = 0UL;
    uint8_t dataIdx;
    int32_t retVal = BCM_ERR_OK;
    uint32_t msgLength;
    void *dataBufPtr;
    uint32_t elemDataSize;

    if (0UL != CAN_McanRWDev[aID].cfgData->fdCfg.enable) {
        fdf_config = TxObj->msgFormat;
        if (0UL != CAN_McanRWDev[aID].cfgData->fdCfg.bitRateSwitchEn) {
            brs_config = TxObj->bitRateSwitchEn;
        } else {
            brs_config = 0UL;
        }
    } else {
        fdf_config = CAN_MSGFORMAT_STANDARD;
        brs_config = 0UL;
    }

    /* @event         CERT INT34-C
         @issue         Coverity infers that left shift may exceed the limit
         @risk           No risk
         @justification Number of dedicated buffer is checked in initialization function.
      */
    /* coverity[cert_int34_c_violation] */
    if (CAN_McanRegs[aID].REGS->txbrp == ((0x01UL << CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.numDedicatedBufs) - 1UL)) {
        /* All Buffers Pending, No Dedicated Tx Buffers available */
        retVal = BCM_ERR_BUSY;
    } else {

        elemDataSize = CAN_McanDataSzToElemSz(CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.elementSz);

        for (i = 0U; i < (uint8_t)CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.numDedicatedBufs; i++) {
            if (0U == (CAN_McanRegs[aID].REGS->txbrp & (0x01UL << i))) {
                 /* TX Request not pending for this buffer, add message to this buffer... */
                 txElem = CAN_MCAN_MSGRAM_TXBUF_BASE(aID) + ((2U + (elemDataSize >> 2)) * i);
                 id_mask = (uint32_t)TxObj->msgInfo.msgIDType;

                 if (CAN_MSGID_STANDARD == TxObj->msgInfo.msgIDType) {
                     txElem[0UL] = ((id_mask << CAN_TXELEMENT_XTD_SHIFT) | (TxObj->msgInfo.msgID << CAN_TXELEMENT_STDID_SHIFT));
                 } else {
                     txElem[0UL] = ((id_mask << CAN_TXELEMENT_XTD_SHIFT) | (TxObj->msgInfo.msgID));
                 }

                 if (CAN_TXESICFG_PASSIVE == TxObj->errStateIndCfg) {     /* Configure ESI bit */
                     txElem[0UL] |= (CAN_TXESICFG_PASSIVE << CAN_TXELEMENT_ESI_SHIFT);
                 }

                 if ((0UL != CAN_McanRWDev[aID].cfgData->rmtFrmEn) && (0UL != TxObj->remoteTxRequestEn)) {     /* Configure RTR bit */
                     txElem[0UL] |= (1UL << CAN_TXELEMENT_RTR_SHIFT);
                 }

                 if (CAN_McanRWDev[aID].msgMarkerIdx >= ((CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.numDedicatedBufs + CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.fifoQueueSize) - 1UL)) {
                     CAN_McanRWDev[aID].msgMarkerIdx = 0UL;
                 } else {
                     CAN_McanRWDev[aID].msgMarkerIdx++;
                 }

                 txElem[1UL] = ((CAN_McanRWDev[aID].msgMarkerIdx << CAN_TXELEMENT_MARKER_SHIFT)
                                | (efc_config << CAN_TXELEMENT_EFC_SHIFT)
                                | (fdf_config << CAN_TXELEMENT_FDF_SHIFT)
                                | (brs_config << CAN_TXELEMENT_BRS_SHIFT)
                                | (TxObj->msgInfo.length << CAN_TXELEMENT_DLC_SHIFT));

                 msgLength = CAN_McanDlcToMsgLen(TxObj->msgInfo.length, fdf_config);
                 dataBufPtr = &txElem[2UL];
                 BCM_MemSet(dataBufPtr, 0, msgLength);    /* clear data buffer memory */
                 /* Store data bytes into TX Element */
                 for(j = 0U; j < msgLength; j++) {
                    /* @event      CERT INT31-C
                     @issue         Coverity infers that the unsigned integer value truncation
                     @risk           No risk
                     @justification message length index will not exceed 8 bit data value.
                        */
                   /* coverity[cert_int31_c_violation] */
                     dataIdx = (uint8_t)(j >> 2U);
                     txElem[2U + dataIdx] |= (((uint32_t)TxObj->msgInfo.dataPtr[j]) << ((j%4U) * 8U));
                 }

                 /* Set appropriate bit in txbar to request transmission */
                 CAN_McanRegs[aID].REGS->txbar = (0x01UL << i);
                 retVal = BCM_ERR_OK;
                 break;
            }
        }

    }
    return retVal;
}

/**
    @brief Write TX data to CAN FIFO Queue

    @trace  #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_McanFifoQueueWrite(CAN_HwIDType aID, CAN_TxMsgObjType * const TxObj)
{
    uint32_t j;
    uint8_t index;
    CAN_MsgFormatType fdf_config;
    uint32_t brs_config;
    uint32_t efc_config = 0x1UL;
    uint32_t *txElem;
    uint32_t id_mask = 0UL;
    uint8_t dataIdx;
    int32_t retVal = BCM_ERR_OK;
    uint32_t msgLength;
    void *dataBufPtr;
    uint32_t elemDataSize;

    if (0UL != CAN_McanRWDev[aID].cfgData->fdCfg.enable) {
        fdf_config = TxObj->msgFormat;
        if (0UL != CAN_McanRWDev[aID].cfgData->fdCfg.bitRateSwitchEn) {
            brs_config = TxObj->bitRateSwitchEn;
        } else {
            brs_config = 0UL;
        }
    } else {
        fdf_config = CAN_MSGFORMAT_STANDARD;
        brs_config = 0UL;
    }

    elemDataSize = CAN_McanDataSzToElemSz(CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.elementSz);

    /* TxObj->txBufCfg == CAN_TXBUFCFG_FIFO_QUEUE */
    if (0x00UL == (CAN_McanRegs[aID].REGS->txfqs & CAN_0_TXFQS_TFQF_MASK)) {
        /* TX FIFO / Queue not full */
        index = (uint8_t)((CAN_McanRegs[aID].REGS->txfqs & CAN_0_TXFQS_TFQPI_MASK) >> CAN_0_TXFQS_TFQPI_SHIFT);
        txElem = CAN_MCAN_MSGRAM_TXBUF_BASE(aID) + ((2UL + (elemDataSize >> 2UL)) * index);
        id_mask = (uint32_t)TxObj->msgInfo.msgIDType;

        if (CAN_MSGID_STANDARD == TxObj->msgInfo.msgIDType) {
            txElem[0UL] = ((id_mask << CAN_TXELEMENT_XTD_SHIFT) | (TxObj->msgInfo.msgID << CAN_TXELEMENT_STDID_SHIFT));
        } else {
            txElem[0UL] = ((id_mask << CAN_TXELEMENT_XTD_SHIFT) | (TxObj->msgInfo.msgID));
        }

        if (CAN_TXESICFG_PASSIVE == TxObj->errStateIndCfg) {     /* Configure ESI bit */
            txElem[0UL] |= (CAN_TXESICFG_PASSIVE << CAN_TXELEMENT_ESI_SHIFT);
        }

        if ((0UL != CAN_McanRWDev[aID].cfgData->rmtFrmEn) && (0UL != TxObj->remoteTxRequestEn)) {     /* Configure RTR bit */
            txElem[0UL] |= (1UL << CAN_TXELEMENT_RTR_SHIFT);
        }

        if (CAN_McanRWDev[aID].msgMarkerIdx >= ((CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.numDedicatedBufs + CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.fifoQueueSize) - 1UL)) {
            CAN_McanRWDev[aID].msgMarkerIdx = 0UL;
        } else {
            CAN_McanRWDev[aID].msgMarkerIdx++;
        }

        txElem[1UL] = ((CAN_McanRWDev[aID].msgMarkerIdx << CAN_TXELEMENT_MARKER_SHIFT)
                        | (efc_config << CAN_TXELEMENT_EFC_SHIFT)
                        | (fdf_config << CAN_TXELEMENT_FDF_SHIFT)
                        | (brs_config << CAN_TXELEMENT_BRS_SHIFT)
                        | (TxObj->msgInfo.length << CAN_TXELEMENT_DLC_SHIFT));

        msgLength = CAN_McanDlcToMsgLen(TxObj->msgInfo.length, fdf_config);
        dataBufPtr = &txElem[2UL];
        /* clear data buffer memory */
        BCM_MemSet(dataBufPtr, 0, msgLength);
        /* Store data bytes into TX Element */
        for(j = 0U; j < msgLength; j++) {
            /* @event      CERT INT31-C
             @issue         Coverity infers that the unsigned integer value truncation
             @risk           No risk
             @justification message length index will not exceed 8 bit data value.
                */
           /* coverity[cert_int31_c_violation] */
            dataIdx = (uint8_t)(j >> 2U);
            txElem[2U + dataIdx] |= (((uint32_t)TxObj->msgInfo.dataPtr[j]) << ((j%4U) * 8U));
        }

        /* Set appropriate bit in txbar to request transmission */
        CAN_McanRegs[aID].REGS->txbar = (0x01UL << index);
        retVal = BCM_ERR_OK;
    } else {
           retVal = BCM_ERR_BUSY;
    }

    return retVal;
}

/**
    @brief Set the MCAN Configuration Change Enable (CCE Bit)

    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
static int32_t CAN_McanSetCCE(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;

    retVal = CAN_McanSetMode(aID, CAN_MCAN_STATE_STOPPED);
    if (BCM_ERR_OK == retVal) {
        CAN_McanRegs[aID].REGS->cccr |= CAN_0_CCCR_CCE_MASK;
    }

    return retVal;
}

/**
  @brief Configures filter elements for RX messages

  @param[in]    Id      Index of the MCAN Controller
  @param[in]    Config  Pointer to configuration structure

  @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
  @trace  #BRCM_SWREQ_CAN_INIT

  @return
*/
static void CAN_McanCfgGlobalFilters(CAN_HwIDType aID)
{
    uint32_t num_filter_objects;
    uint32_t i;

    num_filter_objects = CAN_McanRWDev[aID].cfgData->filterCfgPtr->filterObjSz;
    /* Store pointer to RX Objects configuration */
    const CAN_RxFilterObjType * const rxFilterObjPtr = CAN_McanRWDev[aID].cfgData->filterCfgPtr->filterObjPtr;
    const CAN_MsgRAMCfgType * const msgRAMCfgPtr = &CAN_McanRWDev[aID].cfgData->msgRAMCfg;
    /* Reject Remote and non-matching frames */
    CAN_McanRegs[aID].REGS->gfc = ((CAN_McanRWDev[aID].cfgData->filterCfgPtr->rxNoMatchStd << CAN_0_GFC_ANFS_SHIFT) |
                           (CAN_McanRWDev[aID].cfgData->filterCfgPtr->rxNoMatchExt << CAN_0_GFC_ANFE_SHIFT));

    if (CAN_McanRWDev[aID].cfgData->rmtFrmEn < 1UL) {
        CAN_McanRegs[aID].REGS->gfc |= (CAN_0_GFC_RRFS_MASK | CAN_0_GFC_RRFE_MASK);     /* Reject Remote Frames */
    }

    for (i = 0UL; i < num_filter_objects; i++) {
        /* Configure Filter masks */
        CAN_McanCfgFilterMasks(aID, &rxFilterObjPtr[i],
                                rxFilterObjPtr[i].bufCfg,
                                msgRAMCfgPtr);
    }
}

/**
  @brief Sets Baud Rate parameters of MCAN Controller

  @param[in]    Id      Index of the MCAN Controller

  @return      BCM_ERR_OK              Baudrate set successfully
  @return      BCM_ERR_INVALID_PARAMS  Invalid Controller index or
                                     baudrate parameters

  @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
  @trace  #BRCM_SWREQ_CAN_INIT
*/
static int32_t CAN_McanSetBaudrate(CAN_HwIDType aID)
{
    uint32_t ntseg1;
    uint32_t ntseg2;
    uint32_t nsjw;
    uint32_t nbrp;
    uint32_t nbtp;
    int32_t retVal = BCM_ERR_OK;
    uint32_t dtseg1;
    uint32_t dtseg2;
    uint32_t dsjw;
    uint32_t dbrp;
    uint32_t dbtp;

    ntseg1 = (uint32_t)CAN_McanRWDev[aID].cfgData->baudRateCfg.propSeg + (uint32_t)CAN_McanRWDev[aID].cfgData->baudRateCfg.seg1;
    ntseg2 = (uint32_t)CAN_McanRWDev[aID].cfgData->baudRateCfg.seg2;
    nsjw = (uint32_t)CAN_McanRWDev[aID].cfgData->baudRateCfg.syncJumpWidth;
    nbrp = (uint32_t)CAN_McanRWDev[aID].cfgData->baudRateCfg.prescale;


    /* Calculate timequantas */
    dtseg1 = (uint32_t)CAN_McanRWDev[aID].cfgData->fdCfg.rateCfg.propSeg + (uint32_t)CAN_McanRWDev[aID].cfgData->fdCfg.rateCfg.seg1;
    dtseg2 = (uint32_t)CAN_McanRWDev[aID].cfgData->fdCfg.rateCfg.seg2;
    dsjw = (uint32_t)CAN_McanRWDev[aID].cfgData->fdCfg.rateCfg.syncJumpWidth;
    dbrp = (uint32_t)CAN_McanRWDev[aID].cfgData->fdCfg.rateCfg.prescale;

    if (((CAN_NBTP_NTSEG1_MIN > ntseg1) || (CAN_NBTP_NTSEG1_MAX < ntseg1))
            || (CAN_NBTP_NTSEG2_MAX < ntseg2)
            || (CAN_NBTP_NSJW_MAX < nsjw)
            || (CAN_NBTP_NBRP_MAX < nbrp)) {
             retVal = BCM_ERR_INVAL_PARAMS;
     } else if ((CAN_DBTP_DTSEG1_MAX < dtseg1)
                    || (CAN_DBTP_DTSEG2_MAX < dtseg2)
                    || (CAN_DBTP_DSJW_MAX < dsjw)
                    || (CAN_DBTP_DBRP_MAX < dbrp)) {
                     retVal = BCM_ERR_INVAL_PARAMS;
    } else if (NULL == CAN_McanRWDev[aID].cfgData->filterCfgPtr) {
             retVal = BCM_ERR_INVAL_PARAMS;
     } else {
        nbtp = (((uint32_t)nsjw) << CAN_0_NBTP_NSJW_SHIFT) | (nbrp << CAN_0_NBTP_NBRP_SHIFT)|
            (((uint32_t)ntseg1) << CAN_0_NBTP_NTSEG1_SHIFT) | ((uint32_t)ntseg2);
        CAN_McanRegs[aID].REGS->nbtp = nbtp;

        /* Reset timestamp, timeout registers */
        CAN_McanRegs[aID].REGS->tscc &= ~(CAN_0_TSCC_TCP_MASK
                                            | CAN_0_TSCC_TSS_MASK);
        CAN_McanRegs[aID].REGS->tocc &= ~(CAN_0_TOCC_TOP_MASK
                                            | CAN_0_TOCC_TOS_MASK
                                            | CAN_0_TOCC_ETOC_MASK);

        if (0UL != CAN_McanRWDev[aID].cfgData->fdCfg.enable) {
            /* Enable FDOE */
            CAN_McanRegs[aID].REGS->cccr |= CAN_0_CCCR_FDOE_MASK;

            if (0UL != CAN_McanRWDev[aID].cfgData->fdCfg.bitRateSwitchEn) {

                /* Set dbtp value for CAN FD configuration */
                dbtp = (((uint32_t)dsjw) | (dbrp << CAN_0_DBTP_DBRP_SHIFT) |
                    (((uint32_t)dtseg1) << CAN_0_DBTP_DTSEG1_SHIFT) | (((uint32_t)dtseg2) << CAN_0_DBTP_DTSEG2_SHIFT));

                if (0UL != CAN_McanRWDev[aID].cfgData->fdCfg.delayCompEn) {
                    dbtp |= (0x1UL << CAN_0_DBTP_TDC_SHIFT);
                    CAN_McanRegs[aID].REGS->tdcr = (((uint32_t)CAN_McanRWDev[aID].cfgData->fdCfg.rateCfg.delayCompOffset << CAN_0_TDCR_TDCO_SHIFT) |
                                            ((uint32_t)CAN_McanRWDev[aID].cfgData->fdCfg.rateCfg.delayCompFilter));
                }
                /* Enable BRSE */
                CAN_McanRegs[aID].REGS->cccr |= CAN_0_CCCR_BRSE_MASK;
                CAN_McanRegs[aID].REGS->dbtp = dbtp;
            }
        }
    }

    return retVal;
}


/**
  @brief Configures Tx/Rx Parameters

  @param[in]    Id      Index of the MCAN Controller

  @return      none

  @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
  @trace  #BRCM_SWREQ_CAN_INIT
*/
static void CAN_McanConfigureTxRx(CAN_HwIDType aID)
{
    if (0UL != CAN_McanRWDev[aID].cfgData->loopbackModeEn) {
        /* Enable Test Register */
        CAN_McanRegs[aID].REGS->cccr |= CAN_0_CCCR_TEST_MASK;
        /* LBCK mode enabled */
        CAN_McanRegs[aID].REGS->test |= CAN_0_TEST_LBCK_MASK;
        /* Monitor mode enabled */
        CAN_McanRegs[aID].REGS->cccr |= CAN_0_CCCR_MON_MASK;
    } else {
        /* LBCK mode disabled */
        CAN_McanRegs[aID].REGS->test &= ~CAN_0_TEST_LBCK_MASK;
        /* Monitor mode disabled */
        CAN_McanRegs[aID].REGS->cccr &= ((~CAN_0_CCCR_MON_MASK) & (~CAN_0_CCCR_TEST_MASK));
    }


    if (0UL != CAN_McanRWDev[aID].cfgData->pxhdEn) {
        CAN_McanRegs[aID].REGS->cccr |= CAN_0_CCCR_PXHD_MASK;      /* Disable Protocol Exception Handling   */
    } else {
        CAN_McanRegs[aID].REGS->cccr &= ~CAN_0_CCCR_PXHD_MASK;     /* Enable Protocol Exception Handling    */
    }
    CAN_McanRegs[aID].REGS->txbcie = CAN_0_TXBCIE_CFIE_MASK;
    CAN_McanRegs[aID].REGS->txbcr = CAN_0_TXBCR_CR_MASK;

    CAN_McanCfgGlobalFilters(aID);
}

/**
  @brief Updates the RX packet flags

  @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
  @trace #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
static void CAN_McanUpdateRxFlags(CAN_HwIDType aID, CAN_RxMsgFlagsType *const aFlags,
                                        volatile uint32_t *const aRxElem)
{
    uint32_t canDLC;
    if (0UL != (aRxElem[1UL] & CAN_RXELEMENT_R1_FDF_MASK)) {
        *aFlags = CAN_RXMSGFLAGS_FDF;
    }
    if (0UL != (aRxElem[1UL] & CAN_RXELEMENT_R1_BRS_MASK)) {
        *aFlags |= CAN_RXMSGFLAGS_BRS;
    }
    if (0UL != (aRxElem[0UL] & CAN_RXELEMENT_R0_RTR_MASK)) {
        *aFlags |= CAN_RXMSGFLAGS_RTR;
    }
    if (0UL != (aRxElem[0UL] & CAN_RXELEMENT_R0_XTD_MASK)) {
        *aFlags |= CAN_RXMSGFLAGS_XTD;
    }
    if (0UL != (aRxElem[0UL] & CAN_RXELEMENT_R0_ESI_MASK)) {
        *aFlags |= CAN_RXMSGFLAGS_ESI;
    }
    canDLC = (aRxElem[1UL] & CAN_RXELEMENT_R1_DLC_MASK) >> CAN_RXELEMENT_DLC_SHIFT;
    *aFlags |= (canDLC << 8UL);
}

/**
  @brief Processes messages received in RX buffer

  @param[in]    Id      Index of the MCAN Controller

  @return       void

  @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
  @trace #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
static void CAN_McanProcessRxBuffer(CAN_HwIDType aID)
{
    uint64_t ndat_val;
    uint8_t buf_index;
    volatile uint32_t *rxElem;
    uint32_t i;
    uint32_t dataIdx;
    uint32_t canDLC;
    uint32_t msgLength;
    uint32_t rxPktQueueIdx;
    uint32_t elemDataSize;
    CAN_RxMsgFlagsType rxFlags = 0UL;
    CAN_MsgFormatType msgFmt;
    uint8_t rxBufPutIdx = 0U;
    uint32_t clzCnt = 0U;

    ndat_val = (uint64_t)CAN_McanRegs[aID].REGS->ndat2;
    ndat_val = ndat_val << 32UL;
    ndat_val |= (CAN_McanRegs[aID].REGS->ndat1);

    elemDataSize = CAN_McanDataSzToElemSz(CAN_McanRWDev[aID].cfgData->msgRAMCfg.rxBufCfg.elementSz);

    while ((0ULL != ndat_val) && (CAN_CBACTION_ENABLED != CAN_McanRWDev[aID].rxPackets[CAN_McanRWDev[aID].rxPktQueueIdx].cbEn)) {
        /* New data exists and Callback is not pending for this packet (no loop-around) */
        rxPktQueueIdx = CAN_McanRWDev[aID].rxPktQueueIdx;
        /* Check which buffer has new data */
        /* clzCnt = CORTEX_Clz(ndat_val >> 32); */
        /* Added to pass */
        clzCnt = CAN_Clz((uint32_t)(ndat_val >> 32ULL));
        if (clzCnt == 32UL) {
            /*clzCnt += CORTEX_Clz(ndat_val & 0xFFFFFFFFUL);*/
            /* Added to pass */
            clzCnt += CAN_Clz((uint32_t)(ndat_val & 0xFFFFFFFFULL));
        }

        /* @event         CERT INT31-C
             @issue         Coverity infers that the unsigned integer value truncation
             @risk           No risk
             @justification Count leading zero values will not exceed 63 bit.
          */
        /* coverity[cert_int31_c_violation] */
        buf_index = (63U - (uint8_t)(clzCnt));

        /* @event         CERT INT30-C
             @issue         Coverity infers that the unsigned integer addition may wrap
             @risk           No risk
             @justification Start address plus number of elements is compare with msgram size.
                                 It will not excedd the msgram size.
        */
        /* coverity[cert_int30_c_violation] */
        rxElem = CAN_MCAN_MSGRAM_RXBUF_BASE(aID) + ((2U + (elemDataSize >> 2U)) * buf_index);
        CAN_McanUpdateRxFlags(aID, &rxFlags, rxElem);
        canDLC = (rxElem[1UL] & CAN_RXELEMENT_R1_DLC_MASK) >> CAN_RXELEMENT_DLC_SHIFT;
        if (0UL != (rxFlags & CAN_RXMSGFLAGS_FDF)) {
            msgFmt = CAN_MSGFORMAT_CANFD;
        } else {
            msgFmt = CAN_MSGFORMAT_STANDARD;
        }
        msgLength = CAN_McanDlcToMsgLen(canDLC, msgFmt);

        rxBufPutIdx = 0U;

        if (0UL == (rxElem[0UL] & CAN_RXELEMENT_R0_XTD_MASK)) {
            /* Standard Identifier */
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgID = ((rxElem[0UL] & CAN_RXELEMENT_R0_ID_MASK) >> CAN_RXELEMENT_STDID_SHIFT);
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgIDType = CAN_MSGID_STANDARD;
        } else {
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgID = (rxElem[0UL] & CAN_RXELEMENT_R0_ID_MASK);
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgIDType = CAN_MSGID_EXTENDED;
        }

        CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgFlags = rxFlags; /* Store flags */;

        CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.dataPtr = NULL;
        CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length = msgLength;

        if (0UL < CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length) {
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.dataPtr = &CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxBuf[rxBufPutIdx];
        }

        /* Store data */
        for(i = 0UL; i < CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length; i++) {
            dataIdx = i >> 2U;
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxBuf[rxBufPutIdx] = (uint8_t)((rxElem[2UL + dataIdx] >> ((i%4U)*8U)) & 0xFFUL);
            /* coverity[cert_int30_c_violation] */
            rxBufPutIdx++;
        }
        /* Enable callback for this packet */
        CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].cbEn = CAN_CBACTION_ENABLED;

        if (32U > buf_index) {
            /* Clear New Data bit from ndat1 */
            CAN_McanRegs[aID].REGS->ndat1 = (uint32_t)(0x1UL << buf_index);
        } else {
            /* Clear New Data bit from ndat2 */
            CAN_McanRegs[aID].REGS->ndat2 = (uint32_t)(0x1UL << (buf_index - 32U));
        }
        ndat_val &= (~((uint64_t)0x1U << buf_index));
        CAN_McanRWDev[aID].rxPktQueueIdx = BCM_IncMod(CAN_McanRWDev[aID].rxPktQueueIdx, CAN_RXPKTQ_SIZE);
    }
}

/**
  @brief Processes messages received in RX FIFO 0 or 1

  @param[in]    Id      Index of the MCAN Controller

  @return       void

  @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
  @trace #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
static void CAN_McanProcessRxFIFO(CAN_HwIDType aID,
                                   uint32_t rxFifoCfg)
{
    uint8_t fifoLevel;
    uint32_t index;
    uint32_t i;
    uint32_t dataIdx;
    volatile uint32_t *rxElem;
    uint32_t canDLC;
    uint32_t msgLength;
    uint32_t elemDataSize;
    uint32_t rxPktQueueIdx;
    CAN_RxMsgFlagsType rxFlags = 0UL;
    CAN_MsgFormatType msgFmt;
    uint8_t rxBufPutIdx = 0U;

    if (CAN_RXFILTERBUFCFG_FIFO_1 == rxFifoCfg) {
        /* Read message from RX FIFO 1 */
        /* coverity[cert_int30_c_violation] */
        elemDataSize = CAN_McanDataSzToElemSz(CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.elementSz);
        fifoLevel = (uint8_t)(CAN_McanRegs[aID].REGS->rxf1s & CAN_0_RXF1S_F1FL_MASK);

        while ((0U != fifoLevel) && (CAN_CBACTION_ENABLED != CAN_McanRWDev[aID].rxPackets[CAN_McanRWDev[aID].rxPktQueueIdx].cbEn)) {
            index = (CAN_McanRegs[aID].REGS->rxf1s & CAN_0_RXF1S_F1GI_MASK) >> 8UL;
            /* coverity[cert_int30_c_violation] */
            rxElem = CAN_MCAN_MSGRAM_RXFIFO1_BASE(aID) + ((2UL + (elemDataSize >> 2U)) * index);
            rxPktQueueIdx = CAN_McanRWDev[aID].rxPktQueueIdx;
            CAN_McanUpdateRxFlags(aID, &rxFlags, rxElem);

            canDLC = (rxElem[1] & CAN_RXELEMENT_R1_DLC_MASK) >> CAN_RXELEMENT_DLC_SHIFT;
            if (0UL != (rxFlags & CAN_RXMSGFLAGS_FDF)) {
                msgFmt = CAN_MSGFORMAT_CANFD;
            } else {
                msgFmt = CAN_MSGFORMAT_STANDARD;
            }
            msgLength = CAN_McanDlcToMsgLen(canDLC, msgFmt);

            rxBufPutIdx = 0U;

            if (0UL == (rxElem[0UL] & CAN_RXELEMENT_R0_XTD_MASK)) {
                /* Standard Identifier */
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgID = ((rxElem[0UL] & CAN_RXELEMENT_R0_ID_MASK) >> CAN_RXELEMENT_STDID_SHIFT);
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgIDType = CAN_MSGID_STANDARD;
            } else {
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgID = (rxElem[0UL] & CAN_RXELEMENT_R0_ID_MASK);
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgIDType = CAN_MSGID_EXTENDED;
            }

            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgFlags = rxFlags; /* Store flags */;

            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.dataPtr = NULL;
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length = msgLength;

            if (0UL < CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length) {
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.dataPtr = &CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxBuf[rxBufPutIdx];
            }

            /* Store data */
            for(i = 0UL; i < CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length; i++) {
                dataIdx = i >> 2U;
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxBuf[rxBufPutIdx] = (uint8_t)((rxElem[2UL + dataIdx] >> ((i%4U)*8U)) & 0xFFUL);
                /* coverity[cert_int30_c_violation] */
                rxBufPutIdx++;
            }
            /* Enable callback for this packet */
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].cbEn = CAN_CBACTION_ENABLED;

            /* Updating index in FIFO0 */
            CAN_McanRegs[aID].REGS->rxf1a = (uint32_t)index;
            fifoLevel--;

            CAN_McanRWDev[aID].rxPktQueueIdx = BCM_IncMod(CAN_McanRWDev[aID].rxPktQueueIdx, CAN_RXPKTQ_SIZE);
        }
    } else {
        /* Read message from RX FIFO 0 */
        elemDataSize = CAN_McanDataSzToElemSz(CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.elementSz);
        fifoLevel = (uint8_t)(CAN_McanRegs[aID].REGS->rxf0s & CAN_0_RXF0S_F0FL_MASK);

        while ((0U != fifoLevel) && (CAN_CBACTION_ENABLED != CAN_McanRWDev[aID].rxPackets[CAN_McanRWDev[aID].rxPktQueueIdx].cbEn)) {
            index = (CAN_McanRegs[aID].REGS->rxf0s & CAN_0_RXF0S_F0GI_MASK) >> 8UL;
            /* coverity[cert_int30_c_violation] */
            rxElem = CAN_MCAN_MSGRAM_RXFIFO0_BASE(aID) + ((2UL + (elemDataSize >> 2U)) * index);
            rxPktQueueIdx = CAN_McanRWDev[aID].rxPktQueueIdx;
            CAN_McanUpdateRxFlags(aID, &rxFlags, rxElem);

            canDLC = (rxElem[1] & CAN_RXELEMENT_R1_DLC_MASK) >> CAN_RXELEMENT_DLC_SHIFT;
            if (0UL != (rxFlags & CAN_RXMSGFLAGS_FDF)) {
                msgFmt = CAN_MSGFORMAT_CANFD;
            } else {
                msgFmt = CAN_MSGFORMAT_STANDARD;
            }
            msgLength = CAN_McanDlcToMsgLen(canDLC, msgFmt);

            rxBufPutIdx = 0U;

            if (0UL == (rxElem[0] & CAN_RXELEMENT_R0_XTD_MASK)) {
                /* Standard Identifier */
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgID = ((rxElem[0UL] & CAN_RXELEMENT_R0_ID_MASK) >> CAN_RXELEMENT_STDID_SHIFT);
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgIDType = CAN_MSGID_STANDARD;
            } else {
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgID = (rxElem[0UL] & CAN_RXELEMENT_R0_ID_MASK);
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.msgIDType = CAN_MSGID_EXTENDED;
            }

            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgFlags = rxFlags; /* Store flags */;

            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.dataPtr = NULL;
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length = msgLength;

            if (0UL < CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length) {
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.dataPtr = &CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxBuf[rxBufPutIdx];
            }

            /* Store data */
            for(i = 0UL; i < CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxMsg.msgInfo.length; i++) {
                dataIdx = i >> 2U;
                CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].rxBuf[rxBufPutIdx] = (uint8_t)((rxElem[2UL + dataIdx] >> ((i%4U)*8U)) & 0xFFUL);
                /* coverity[cert_int30_c_violation] */
                rxBufPutIdx++;
            }
            /* Enable callback for this packet */
            CAN_McanRWDev[aID].rxPackets[rxPktQueueIdx].cbEn = CAN_CBACTION_ENABLED;

            /* Updating index in FIFO0 */
            CAN_McanRegs[aID].REGS->rxf0a = (uint32_t)index;
            fifoLevel--;

            CAN_McanRWDev[aID].rxPktQueueIdx = BCM_IncMod(CAN_McanRWDev[aID].rxPktQueueIdx, CAN_RXPKTQ_SIZE);
        }
    }
}

/**
 @brief Processes message reception from Handler

 @param[in]    Id      Index of the MCAN Controller
 @param[in]    Config  Configuration to use for message reception
                         RX Buffer, RX FIFO 0, or RX FIFO 1

 @return       void

  @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
  @trace #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
static void CAN_McanProcessCanRx(CAN_HwIDType aID, uint32_t intr)
{
    int32_t retVal = BCM_ERR_OK;

    if (0UL != (intr & CAN_0_IR_DRX_MASK)) {
        /* Message Stored to Dedicated RX Buffer */
        CAN_McanProcessRxBuffer(aID);
    }

    if (0UL != (intr & CAN_0_IR_RF0N_MASK)) {
        /* Message Stored to Dedicated RX FIFO 0 */
        if (0UL != (intr & CAN_0_IR_RF0L_MASK)) {
            /* To be Supported, RX FIFO 0 Message Lost */
            retVal = BCM_ERR_NOSUPPORT;
        }
        CAN_McanProcessRxFIFO(aID, CAN_RXFILTERBUFCFG_FIFO_0);
    }

    if (0UL != (intr & CAN_0_IR_RF1N_MASK)) {
        /* Message Stored to Dedicated RX FIFO 1 */
        if (0UL != (intr & CAN_0_IR_RF1L_MASK)) {
            /* To be Supported, RX FIFO 1 Message Lost */
            retVal = BCM_ERR_NOSUPPORT;
        }
        CAN_McanProcessRxFIFO(aID, CAN_RXFILTERBUFCFG_FIFO_1);
    }

    if (BCM_ERR_OK != retVal) {
        /* @event         CERT INT31-C
             @issue         Coverity infers that the unsigned integer value truncation
             @risk           No risk
             @justification aID value is 0 and it is checked.
           */
        /* coverity[cert_int31_c_violation] */
        CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_PROCESSCANRX_PROC, retVal,
                            0UL, 0UL, 0UL, 0UL);
    }
    return;
}

/**
  @brief Initializes Message RAM Elements used by MCAN Controller

  @param[in]    Id      Index of the MCAN Controller

  @return      BCM_ERR_OK              Message RAM configured successfully
  @return      BCM_ERR_INVALID_PARAMS  Invalid Message RAM parameter

  @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
  @trace  #BRCM_SWREQ_CAN_INIT
*/
static int32_t CAN_McanMsgRAMInit(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
    /* @event         CERT INT30-C
         @issue         Coverity infers that the unsigned integer addition may wrap
         @risk           No risk
         @justification Start address plus number of elements is compare with msgram size.
                             It will not excedd the msgram size.
      */
    /* coverity[cert_int30_c_violation] */
    if ((CAN_MSGRAM_SIZE < (CAN_McanRWDev[aID].cfgData->msgRAMCfg.stdFltrCfg.startAddr
                                + CAN_McanRWDev[aID].cfgData->msgRAMCfg.stdFltrCfg.numElements))
        || (CAN_MSGRAM_MAX_STD_FILTER_ELEMS < CAN_McanRWDev[aID].cfgData->msgRAMCfg.stdFltrCfg.numElements)
        || (CAN_MSGRAM_SIZE < (CAN_McanRWDev[aID].cfgData->msgRAMCfg.extFltrCfg.startAddr
                                + CAN_McanRWDev[aID].cfgData->msgRAMCfg.extFltrCfg.numElements))
        || (CAN_MSGRAM_MAX_EXT_FILTER_ELEMS < CAN_McanRWDev[aID].cfgData->msgRAMCfg.extFltrCfg.numElements)
        || (CAN_MSGRAM_SIZE < (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.numElements
                                + CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.startAddr))
        || (CAN_MSGRAM_MAX_RX_FIFO0_ELEMS < CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.numElements)
        || (CAN_MSGRAM_SIZE < (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.numElements
                                + CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.startAddr))
        || (CAN_MSGRAM_MAX_RX_FIFO1_ELEMS < CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.numElements)) {
             retVal = BCM_ERR_INVAL_PARAMS;
             CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_MESSAGERAM_INIT_PROC, retVal,
                                    0UL, 0UL, 0UL, 0UL);
    } else 
    /* coverity[cert_int30_c_violation] */
    if ((CAN_MSGRAM_SIZE < (CAN_McanRWDev[aID].cfgData->msgRAMCfg.txEventCfg.numElements
                                + CAN_McanRWDev[aID].cfgData->msgRAMCfg.txEventCfg.startAddr))
        || (CAN_MSGRAM_MAX_TX_EVENT_ELEMS < CAN_McanRWDev[aID].cfgData->msgRAMCfg.txEventCfg.numElements)
        || (CAN_MSGRAM_SIZE <= CAN_McanRWDev[aID].cfgData->msgRAMCfg.rxBufCfg.startAddr)
        || (CAN_MSGRAM_SIZE <= CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.startAddr)
        || (CAN_MSGRAM_MAX_TX_BUFFER_ELEMS < (CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.fifoQueueSize
                                                + CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.numDedicatedBufs))) {
             retVal = BCM_ERR_INVAL_PARAMS;
             CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_MESSAGERAM_INIT_PROC, retVal,
                                    0UL, 0UL, 0UL, 0UL);
    } else {

        /* Standard ID Filter Configuration */
        CAN_McanRegs[aID].REGS->sidfc = (uint32_t)((CAN_McanRWDev[aID].cfgData->msgRAMCfg.stdFltrCfg.numElements << CAN_0_SIDFC_LSS_SHIFT) |
                (CAN_McanRWDev[aID].cfgData->msgRAMCfg.stdFltrCfg.startAddr));

        /* Extended ID Filter Configuration */
        CAN_McanRegs[aID].REGS->xidfc = (uint32_t)((CAN_McanRWDev[aID].cfgData->msgRAMCfg.extFltrCfg.numElements << CAN_0_XIDFC_LSE_SHIFT) |
                (CAN_McanRWDev[aID].cfgData->msgRAMCfg.extFltrCfg.startAddr));

        /* RX FIFO 0 Configuration */
        CAN_McanRegs[aID].REGS->rxf0c = (uint32_t)((CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.mode << CAN_0_RXF0C_F0OM_SHIFT) |
                (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.waterMark << CAN_0_RXF0C_F0WM_SHIFT)
                | (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.numElements << CAN_0_RXF0C_F0S_SHIFT)
                | (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.startAddr));

        /* RX FIFO 1 Configuration */
        CAN_McanRegs[aID].REGS->rxf1c = (uint32_t)((CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.mode << CAN_0_RXF1C_F1OM_SHIFT) |
                (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.waterMark << CAN_0_RXF1C_F1WM_SHIFT)
                | (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.numElements << CAN_0_RXF1C_F1S_SHIFT)
                | (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.startAddr));

        /* TX Event FIFO Configuration */
        CAN_McanRegs[aID].REGS->txefc = (uint32_t)((CAN_McanRWDev[aID].cfgData->msgRAMCfg.txEventCfg.waterMark << CAN_0_TXEFC_EFWM_SHIFT) |
                (CAN_McanRWDev[aID].cfgData->msgRAMCfg.txEventCfg.numElements << CAN_0_TXEFC_EFS_SHIFT)
                | (CAN_McanRWDev[aID].cfgData->msgRAMCfg.txEventCfg.numElements));

        /* TX Buffers Configuration */
        CAN_McanRegs[aID].REGS->txbc = (uint32_t)((CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.mode << CAN_0_TXBC_TFQM_SHIFT) |
                (CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.fifoQueueSize << CAN_0_TXBC_TFQS_SHIFT)
                | (CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.numDedicatedBufs << CAN_0_TXBC_NDTB_SHIFT)
                | (CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.startAddr));

        if (0UL != CAN_McanRWDev[aID].cfgData->fdCfg.enable) {
            /* Up to 64 bytes for CAN FD */
            CAN_McanRegs[aID].REGS->rxesc = (uint32_t)((CAN_McanRWDev[aID].cfgData->msgRAMCfg.rxBufCfg.elementSz << CAN_0_RXESC_RBDS_SHIFT) |
                    (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo1Cfg.elementSz << CAN_0_RXESC_F1DS_SHIFT)
                    | (CAN_McanRWDev[aID].cfgData->msgRAMCfg.fifo0Cfg.elementSz));

            CAN_McanRegs[aID].REGS->txesc = (uint32_t)CAN_McanRWDev[aID].cfgData->msgRAMCfg.txBufCfg.elementSz;
        }
        else {
            /* 8 bytes by default */
            CAN_McanRegs[aID].REGS->rxesc = 0UL;
            CAN_McanRegs[aID].REGS->txesc = 0UL;
        }
    }
    return retVal;
}

/**
    @brief  Processes TX Event FIFO

    @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
static void CAN_McanProcessTx(CAN_HwIDType aID)
{
    uint8_t fifoLevel;
    uint32_t index;
    volatile uint32_t *txEventElem;
    uint32_t txPktQueueIdx;

    /* Read message from TX Event FIFO */
    fifoLevel = (uint8_t)(CAN_McanRegs[aID].REGS->txefs & CAN_0_TXEFS_EFFL_MASK);

    while ((0U != fifoLevel) && (CAN_CBACTION_ENABLED != CAN_McanRWDev[aID].txPackets[CAN_McanRWDev[aID].txPktQueueIdx].cbEn)) {
        index = (CAN_McanRegs[aID].REGS->txefs & CAN_0_TXEFS_EFGI_MASK) >> 8UL;
        /* @event         CERT INT30-C
             @issue         Coverity infers that the unsigned integer addition may wrap
             @risk           No risk
             @justification Start address plus number of elements is compare with msgram size.
                         It will not excedd the msgram size.
          */
        /* coverity[cert_int30_c_violation] */
        txEventElem = CAN_MCAN_MSGRAM_TXEVENTFIFO_BASE(aID) + (2UL * index);

        txPktQueueIdx = CAN_McanRWDev[aID].txPktQueueIdx;

        if (0U == (txEventElem[0UL] & CAN_TXEVENT_FIFO_ELEMENT_R0_XTD_MASK)) {
            /* Standard Identifier */
            CAN_McanRWDev[aID].txPackets[txPktQueueIdx].txMsgObj.msgInfo.msgID = ((txEventElem[0] & CAN_TXEVENT_FIFO_ELEMENT_R0_ID_MASK) >> 18UL);
            CAN_McanRWDev[aID].txPackets[txPktQueueIdx].txMsgObj.msgInfo.msgIDType = CAN_MSGID_STANDARD;
        } else {
            /* Extended Identifier */
            CAN_McanRWDev[aID].txPackets[txPktQueueIdx].txMsgObj.msgInfo.msgID = ((txEventElem[0UL] & CAN_TXEVENT_FIFO_ELEMENT_R0_ID_MASK));
            CAN_McanRWDev[aID].txPackets[txPktQueueIdx].txMsgObj.msgInfo.msgIDType = CAN_MSGID_EXTENDED;
        }

        CAN_McanRWDev[aID].txPackets[txPktQueueIdx].txMsgObj.msgInfo.length = (txEventElem[1UL] & CAN_TXEVENT_FIFO_ELEMENT_R1_DLC_MASK) >> 16UL;
        CAN_McanRWDev[aID].txPackets[txPktQueueIdx].txMsgObj.msgInfo.dataPtr = NULL;
        CAN_McanRWDev[aID].txPackets[txPktQueueIdx].txMsgObj.msgFormat = ((txEventElem[1UL] & CAN_TXEVENT_FIFO_ELEMENT_R1_FDF_MASK) >> 21UL);
        CAN_McanRWDev[aID].txPackets[txPktQueueIdx].txMsgObj.bitRateSwitchEn = ((txEventElem[1UL] & CAN_TXEVENT_FIFO_ELEMENT_R1_BRS_MASK) >> 20UL);

        CAN_McanRWDev[aID].txPackets[txPktQueueIdx].cbEn = CAN_CBACTION_ENABLED;

        /* Updating index in TX Event FIFO */
        CAN_McanRegs[aID].REGS->txefa = (uint32_t)index;
        fifoLevel--;

        CAN_McanRWDev[aID].txPktQueueIdx = BCM_IncMod(CAN_McanRWDev[aID].txPktQueueIdx,
                CAN_MSGRAM_MAX_TX_BUFFER_ELEMS);
    };
}

/**
    @brief  Processes MCAN Errors

    @trace #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
static void CAN_McanProcessError(CAN_HwIDType aID, CAN_ErrorType errorCode)
{
    uint32_t errQueueIdx;

    /* Store Error Code and enable Callback */
    errQueueIdx = CAN_McanRWDev[aID].errQueueIdx;
    CAN_McanRWDev[aID].errors[errQueueIdx].cbEn = CAN_CBACTION_ENABLED;
    CAN_McanRWDev[aID].errors[errQueueIdx].errorCode = errorCode;

    CAN_McanRWDev[aID].errQueueIdx = BCM_IncMod(CAN_McanRWDev[aID].errQueueIdx,
            CAN_ERRPKTQ_SIZE);
}

/**
  @brief Handles Transmission Complete events

  @param[in]    Id      Index of the MCAN Controller
  @param[in]    Flags   Interrupt flags set

  @return       void

  @trace #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
  @trace #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
static void CAN_McanProcessCanTx(CAN_HwIDType aID, uint32_t intr)
{
    if (0UL != (intr & CAN_0_IR_TEFN_MASK)) {
        if (0UL != (intr & CAN_0_IR_TEFL_MASK)) {
            /* TX Event FIFO Element Lost */
            /* @event         CERT INT31-C
             @issue         Coverity infers that the unsigned integer value truncation
             @risk           No risk
             @justification aID value is 0 and it is checked.
                */
           /* coverity[cert_int31_c_violation] */
            CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_PROCESSCANTX_PROC, BCM_ERR_NOMEM,
                                    0UL, 0UL, 0UL, 0UL);
        }
        CAN_McanProcessTx(aID);
    }
}


/**
  @brief Handles CAN Protocol Error events

  @param[in]    Id      Index of the MCAN Controller
  @param[in]    Flags   Interrupt flags set

  @return       void

  @trace #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
  @trace #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
static void CAN_McanProcessCanError(CAN_HwIDType aID, uint32_t intr)
{
    CAN_ErrorType errorCode  = 0UL;

    if (0UL != (intr & CAN_0_IR_PEA_MASK)) {
        errorCode = (CAN_McanRegs[aID].REGS->psr & CAN_0_PSR_LEC_MASK);
    } else if (0UL != (intr & CAN_0_IR_PED_MASK)) {
        errorCode = (CAN_McanRegs[aID].REGS->psr & CAN_0_PSR_DLEC_MASK);
    } else {
        /* No bus error */
        errorCode = 0UL;
    }
    if(errorCode > 0UL) {
        CAN_McanProcessError(aID, errorCode);
    }
    return;
}

/**
  @brief Reads the CAN error counters

  @trace #BRCM_SWARCH_CAN_DRV_GETERRCNTS_PROC
  @trace #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_McanGetErrCnts(CAN_HwIDType aID, CAN_ErrCntType * aErrCntDesc)
{
    int32_t retVal = BCM_ERR_OK;

    if ((CAN_MAX_HW_ID <= aID) || (NULL == aErrCntDesc)) {
         retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CAN_MCAN_STATE_RUNNING != CAN_McanRWDev[aID].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        aErrCntDesc->tec = (CAN_McanRegs[aID].REGS->ecr & CAN_0_ECR_TEC_MASK);
        aErrCntDesc->rec = (CAN_McanRegs[aID].REGS->ecr & CAN_0_ECR_REC_MASK) >> CAN_0_ECR_REC_SHIFT;
        aErrCntDesc->rp = (CAN_McanRegs[aID].REGS->ecr & CAN_0_ECR_RP_MASK) >> CAN_0_ECR_RP_SHIFT;
        aErrCntDesc->cel = (CAN_McanRegs[aID].REGS->ecr & CAN_0_ECR_CEL_MASK) >> CAN_0_ECR_CEL_SHIFT;
    }
    if (BCM_ERR_OK != retVal) {
        CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_GETERRCNTS_PROC, retVal,
                          0UL, 0UL, 0UL, 0UL);
    }
    return retVal;
}

/**
   @brief Get error code
   @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
   @trace #BRCM_SWREQ_CAN_COMMUNICATION
 */
static int32_t CAN_McanGetErrCode(CAN_HwIDType aID, CAN_ErrorType * const aErrCode)
{
    int32_t retVal = BCM_ERR_OK;

    if (CAN_McanRWDev[aID].errQueueIdx == CAN_McanRWDev[aID].errDeQueueIdx) {
        /* No more errors to process */
        retVal = BCM_ERR_EAGAIN;
    } else {
        if (CAN_CBACTION_ENABLED == CAN_McanRWDev[aID].errors[CAN_McanRWDev[aID].errDeQueueIdx].cbEn) {
            *aErrCode = CAN_McanRWDev[aID].errors[CAN_McanRWDev[aID].errDeQueueIdx].errorCode;
        }

        /* DeQueue the Packet */
        CAN_McanRWDev[aID].errors[CAN_McanRWDev[aID].errDeQueueIdx].cbEn = CAN_CBACTION_DISABLED;    /* reset callback flag */
        CAN_McanRWDev[aID].errDeQueueIdx = BCM_IncMod(CAN_McanRWDev[aID].errDeQueueIdx, CAN_ERRPKTQ_SIZE);
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
   @brief Power off CAN bus
   @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
   @trace #BRCM_SWREQ_CAN_COMMUNICATION
 */
static int32_t CAN_McanIsBusOff(CAN_HwIDType aID, uint32_t * const aIsBusOff)
{
    int32_t retVal = BCM_ERR_OK;

    if (aIsBusOff != NULL) {
        *aIsBusOff = CAN_McanRWDev[aID].busOff;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

/**
   @brief Recover the CAN bus after power off

   @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
   @trace #BRCM_SWREQ_CAN_COMMUNICATION
 */
static void CAN_McanBusOffRecovery(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;

    if ((0UL != CAN_McanRWDev[aID].busOff) && (0UL != CAN_McanRWDev[aID].cfgData->autoBusOffRecoveryEn)) {
        CAN_McanResetInternalVars(aID);
        retVal = CAN_McanSetMode(aID, CAN_MCAN_STATE_RUNNING);
        /* Start Automatic Bus-Off Recovery */
        if (BCM_ERR_OK != retVal) {
            CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_BUSOFFRECOVERY_PROC, retVal,
                                0UL, 0UL, 0UL, 0UL);
        }
    }
}

/**
    @brief Gets receive message

    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_COMMUNICATION
 */
static int32_t CAN_McanGetRxMsg(CAN_HwIDType aID, CAN_RxMsgObjType ** aMsg)
{
    int32_t retVal = BCM_ERR_OK;

    if (CAN_McanRWDev[aID].rxPktDeQueueIdx == CAN_McanRWDev[aID].rxPktQueueIdx) {
        /* No more rx packets to process */
        retVal = BCM_ERR_EAGAIN;
    } else {
        if (CAN_CBACTION_ENABLED == CAN_McanRWDev[aID].rxPackets[CAN_McanRWDev[aID].rxPktDeQueueIdx].cbEn) {
            *aMsg = &(CAN_McanRWDev[aID].rxPackets[CAN_McanRWDev[aID].rxPktDeQueueIdx].rxMsg);
        }

        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
    @brief Dequeues receive message
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_COMMUNICATION
 */
static void CAN_McanDeQueueRxPkt(CAN_HwIDType aID)
{
    CAN_McanRWDev[aID].rxPackets[CAN_McanRWDev[aID].rxPktDeQueueIdx].cbEn = CAN_CBACTION_DISABLED;    /* reset callback flag after processing packet */
    CAN_McanRWDev[aID].rxPktDeQueueIdx = BCM_IncMod(CAN_McanRWDev[aID].rxPktDeQueueIdx, CAN_RXPKTQ_SIZE);
}

/**
    @brief Gets the transmit message.
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_COMMUNICATION
 */
static int32_t CAN_McanGetTxMsg(CAN_HwIDType aID, CAN_TxMsgObjType * aMsg)
{
    int32_t retVal = BCM_ERR_OK;

    if (CAN_McanRWDev[aID].txPktDeQueueIdx == CAN_McanRWDev[aID].txPktQueueIdx) {
        /* No more tx packets to process */
        retVal = BCM_ERR_EAGAIN;
    } else {
        if (CAN_CBACTION_ENABLED == CAN_McanRWDev[aID].txPackets[CAN_McanRWDev[aID].txPktDeQueueIdx].cbEn) {
            *aMsg = CAN_McanRWDev[aID].txPackets[CAN_McanRWDev[aID].txPktDeQueueIdx].txMsgObj;
        }

        /* DeQueue the packet */
        CAN_McanRWDev[aID].txPackets[CAN_McanRWDev[aID].txPktDeQueueIdx].cbEn = CAN_CBACTION_DISABLED;    /* reset callback flag after processing packet */
        CAN_McanRWDev[aID].txPktDeQueueIdx = BCM_IncMod(CAN_McanRWDev[aID].txPktDeQueueIdx, CAN_MSGRAM_MAX_TX_BUFFER_ELEMS);
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
  @brief This API is used to write a message to CAN TX Buffer or FIFO / Queue

  @param[in]    Id      Index of the MCAN Controller
  @param[in]    TxObj   Pointer to Transmit Object containing message information

  @return       int32_t Error Code

  @trace  #BRCM_SWARCH_CAN_DRV_WRITE_PROC
  @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_McanWrite(CAN_HwIDType aID, CAN_TxMsgObjType * const TxObj)
{
    int32_t retVal = BCM_ERR_OK;

    if ((CAN_MAX_HW_ID <= aID) || (NULL == TxObj)) {
         retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CAN_MCAN_STATE_RUNNING != CAN_McanRWDev[aID].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else if (((CAN_MSGFORMAT_CANFD == TxObj->msgFormat)
                && (CAN_FD_MAX_DATA_SIZE < TxObj->msgInfo.length))
            || ((CAN_MSGFORMAT_STANDARD == TxObj->msgFormat)
                && (CAN_MAX_DATA_SIZE < TxObj->msgInfo.length))) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (CAN_TXBUFCFG_BUFFER == TxObj->txBufCfg) {
            retVal = CAN_McanTxBufWrite(aID, TxObj);
        } else {
            retVal = CAN_McanFifoQueueWrite(aID, TxObj);
        }
    }
    return retVal;
}

/**
  @brief This API is used to put the CAN Controller in Power-down / Sleep mode

  @param[in]    Id      Index of the MCAN Controller

  @return       int32_t Error Code

  @trace #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
  @trace #BRCM_SWREQ_CAN_POWER
*/
static int32_t CAN_McanSleep(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t timeout = CAN_HW_TIMEOUT_CNT;
    uint32_t line = __LINE__;

    if (CAN_MAX_HW_ID <= aID) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CAN_MCAN_STATE_RUNNING != CAN_McanRWDev[aID].state) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_STATE;
    } else {

        retVal = CAN_McanCancelTxAll(aID); /* Cancel Transmission in progress */
        if (BCM_ERR_CANCELLED == retVal) {

            CAN_McanRegs[aID].REGS->cccr |= CAN_0_CCCR_CSR_MASK;
            while (0UL < timeout) {
                if (0UL != (CAN_McanRegs[aID].REGS->cccr & CAN_0_CCCR_INIT_MASK)) {
                    break;
                }
                timeout--;
            }
            if (0UL == timeout) {
                line = __LINE__;
                retVal = BCM_ERR_TIME_OUT;
            }

            if (BCM_ERR_CANCELLED == retVal) {

                timeout = CAN_HW_TIMEOUT_CNT;
                while (0UL < timeout) {
                    if (0UL != (CAN_McanRegs[aID].REGS->cccr & CAN_0_CCCR_CSA_MASK)) {
                        break;
                    }
                    timeout--;
                }
                if (0UL == timeout) {
                    line = __LINE__;
                    retVal = BCM_ERR_TIME_OUT;
                }

                if (BCM_ERR_CANCELLED == retVal) {
                    line = __LINE__;
                    retVal = CAN_McanSetMode(aID, CAN_MCAN_STATE_SLEEP);
                }
            }
        }
    }

    if (BCM_ERR_OK != retVal) {
        CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_SLEEP_PROC,
                                retVal, 0UL, 0UL, 0UL, line);
    }
    return retVal;
}

/**
  @brief This API is used to bring the CAN Controller out of Power-down / Sleep mode

  @param[in]    Id      Index of the MCAN Controller

  @return       int32_t Error Code

  @trace #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
  @trace #BRCM_SWREQ_CAN_POWER
*/
static int32_t CAN_McanWakeUp(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t timeout = CAN_HW_TIMEOUT_CNT;
    uint32_t line = __LINE__;

    if (CAN_MAX_HW_ID <= aID) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CAN_MCAN_STATE_SLEEP != CAN_McanRWDev[aID].state) {
        line = __LINE__;
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        /* Reset CSR */
        CAN_McanRegs[aID].REGS->cccr &= ~CAN_0_CCCR_CSR_MASK;
        while (0UL < timeout) {
            if (0UL == (CAN_McanRegs[aID].REGS->cccr & CAN_0_CCCR_CSA_MASK)) {
                break;
            }
            timeout--;
        }
        if (0UL == timeout) {
            line = __LINE__;
            retVal = BCM_ERR_TIME_OUT;
        }
        if (BCM_ERR_OK == retVal) {
            line = __LINE__;
            /* Restart CAN Communication */
            retVal = CAN_McanSetMode(aID, CAN_MCAN_STATE_RUNNING);
        }
    }

    if (BCM_ERR_OK != retVal) {
        CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_WAKEUP_PROC,
                                retVal, 0UL, 0UL, 0UL, line);
    }
    return retVal;
}

/**
  @brief This API is used to de-initialize the MCAN Controller

  @param[in]    Id  Index of the MCAN Controller

  @return       void

  @trace #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
  @trace #BRCM_SWREQ_CAN_INIT
*/
static void CAN_McanDeInit(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t line = __LINE__;

    if (CAN_MAX_HW_ID <= aID) {
         retVal = BCM_ERR_INVAL_PARAMS;
    } else {

        if (CAN_MCAN_STATE_RUNNING == CAN_McanRWDev[aID].state) {
            /* Cancel pending transmissions */
            retVal = CAN_McanCancelTxAll(aID);
            if (BCM_ERR_CANCELLED == retVal) {
                retVal = BCM_ERR_OK;
            }
        }
        if(retVal == BCM_ERR_OK) {
            CAN_McanDisAllInterrupts(aID);
            CAN_McanResetInternalVars(aID);
            retVal = CAN_McanSetMode(aID, CAN_MCAN_STATE_UNINIT);
            CAN_IntInReset(aID);
        }
    }

    if (BCM_ERR_OK != retVal) {
        CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_DEINIT_PROC, retVal,
                                0UL, 0UL, 0UL, line);
    }
    return;
}

/**
  @brief Initialize MCAN Controller

  This API is used to initialize the MCAN Controller

  @param[in]    Id      Index of the MCAN Controller
  @param[in]    Config  Pointer to configuration structure

  @return       int32_t

  @trace #BRCM_SWARCH_CAN_DRV_INIT_PROC
  @trace #BRCM_SWREQ_CAN_INIT
*/
static int32_t CAN_McanInit(CAN_HwIDType aID, const CAN_ConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t line = __LINE__;
    if (CAN_MAX_HW_ID <= aID) {
         retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CAN_MCAN_STATE_UNINIT != CAN_McanRWDev[aID].state) {
            line = __LINE__;
            retVal = BCM_ERR_INVAL_STATE;
    } else {
        BCM_MemSet(&CAN_McanRWDev[0], 0U, sizeof(CAN_McanRWDevType));
        CAN_McanRWDev[aID].cfgData = aConfig;
        retVal = CAN_IntPowerUpOutReset(aID);
        if(BCM_ERR_OK == retVal) {
            retVal = CAN_McanSetCCE(aID);
        }
        if (BCM_ERR_OK == retVal) {
            /* Initialize and clear Message RAM */
            retVal = CAN_McanMsgRAMInit(aID);
        }
        if (BCM_ERR_OK == retVal) {
            /* Set bit timing and filter parameters */
            retVal = CAN_McanSetBaudrate(aID);
        }
         if (BCM_ERR_OK == retVal) {
            /* Configure Tx/Rx parameters */
            CAN_McanConfigureTxRx(aID);
            retVal = CAN_McanSetMode(aID, CAN_MCAN_STATE_RUNNING);
        }
    }

    if (BCM_ERR_OK != retVal) {
        CAN_McanReportError((uint8_t)aID, BRCM_SWDSGN_CAN_MCAN_INIT_PROC, retVal,
                                0UL, 0UL, 0UL, line);
    }
    return retVal;
}

/** @brief Initialize CAN driver

    This API initializes CAN controller, necessary software variables, data
    structures, and the Message RAM.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the CAN controller
    @param[in]      aConfig     Pointer to CAN driver configuration

    @return     int32_t

    @post None

    @limitations None
    @trace  #BRCM_SWARCH_CAN_DRV_INIT_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
int32_t CAN_DrvInit(CAN_HwIDType aID, const CAN_ConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    retVal = CAN_McanInit(aID, aConfig);
    return retVal;
}

/** @brief De-initialize CAN driver

    This API un-initializes CAN controller, and frees necessary software variables, data
    structures, and the Message RAM.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the CAN controller

    @return     void

    @post None

    @limitations None

    @trace  #BRCM_SWARCH_CAN_DRV_DEINIT_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
void CAN_DrvDeInit(CAN_HwIDType aID)
{
    CAN_McanDeInit(aID);
}

/** @brief CAN write request

    This API issues the Write request to CAN driver. Write request is processed
    by the driver and when completed, client is notified via TX complete
    callback.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller
    @param[inout]   aTxObj      Message object for transmission

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Write request successful
    @retval     #BCM_ERR_INVAL_PARAMS   @p aTxObj is NULL and/or<BR>
                                        @p aID is invalid
    @retval     #BCM_ERR_BUSY           TX Queue is full or <BR>
                                        TX buffers are full
    @retval     #BCM_ERR_INVAL_PARAMS   @p aTxObj->msgInfo.length is
                                        greater than 15
    @retval     #BCM_ERR_INVAL_STATE    CAN controller is in invalid state
    @retval     #BCM_ERR_UNINIT         CAN driver is not initialized

    @post TBD

    @limitations None

    @trace  #BRCM_SWARCH_CAN_DRV_WRITE_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
int32_t CAN_DrvWrite(CAN_HwIDType aID, CAN_TxMsgObjType * const aTxObj)
{
    int32_t retVal = BCM_ERR_OK;

    retVal = CAN_McanWrite(aID, aTxObj);
    return retVal;
}

/** @brief Low-Power/Sleep mode

    This API moves the CAN controller to Low-Power/Sleep mode.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             CAN Controller successful transition to
                                        Low-Power mode
    @retval     #BCM_ERR_TIME_OUT       Time out while waiting for CAN
                                        controller to transition to Low-Power
                                        mode
    @retval     #BCM_ERR_INVAL_STATE    CAN driver in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS   @p aID is invalid
    @retval     #BCM_ERR_UNINIT         CAN driver is not initialized

    @post TBD

    @limitations None

    @trace  #BRCM_SWARCH_CAN_DRV_SLEEP_PROC
    @trace  #BRCM_SWREQ_CAN_POWER
*/
int32_t CAN_DrvSleep(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;

    retVal = CAN_McanSleep(aID);
    return retVal;
}

/** @brief Wakeup controller from Low-Power/Sleep mode

    This API brings the CAN controller out of Low-Power/Sleep mode.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             CAN controller successful woken up from
                                        Low-Power mode
    @retval     #BCM_ERR_TIME_OUT       Time out while waiting for CAN
                                        controller to wake up from Low-Power
                                        mode
    @retval     #BCM_ERR_INVAL_STATE    CAN driver in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS   @p aID is invalid
    @retval     #BCM_ERR_UNINIT         CAN driver is not initialized

    @post TBD

    @limitations None

    @trace  #BRCM_SWARCH_CAN_DRV_WAKEUP_PROC
    @trace  #BRCM_SWREQ_CAN_POWER
*/
int32_t CAN_DrvWakeUp(CAN_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;

    retVal = CAN_McanWakeUp(aID);
    return retVal;
}

/** @brief Get Error Counter information

    This API provides Error Counter information.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID            Index of the CAN controller
    @param[out]     aErrCntDesc    Pointer to Error Counter Description Structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Error Counter info retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   @p aErrCntDesc is NULL and/or<BR>
                                        @p aID is invalid
    @retval     #BCM_ERR_INVAL_STATE    CAN driver in invalid state
    @retval     #BCM_ERR_UNINIT         CAN driver is not initialized


    @post TBD

     @limitations None

    @trace  #BRCM_SWARCH_CAN_DRV_GETERRCNTS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
int32_t CAN_DrvGetErrCnts(CAN_HwIDType aID, CAN_ErrCntType * const aErrCntDesc)
{
    int32_t retVal = BCM_ERR_OK;;

    retVal = CAN_McanGetErrCnts(aID, aErrCntDesc);
    return retVal;
}

/**
    @brief Gets the receive message
    @trace #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_COMMUNICATION
 */
static int32_t CAN_GetRxMsg(CAN_HwIDType aID,
                            CAN_RxMsgObjType ** const aMsg)
{
    int32_t retVal = BCM_ERR_OK;

    retVal = CAN_McanGetRxMsg(aID, aMsg);
    return retVal;
}

/**
    @brief Dequeue the received packet for defferred can processing
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
 */
static void CAN_PutRxPkt(CAN_HwIDType aID)
{

    CAN_McanDeQueueRxPkt(aID);

}

/**
    @brief Process received packets for defferred can processing
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_ProcessRcvdPkts(CAN_HwIDType aID, const CAN_ConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    CAN_RxMsgObjType *pRxMsg = NULL;

    retVal = CAN_GetRxMsg(aID, &pRxMsg);
    if (BCM_ERR_OK == retVal) {
        /* Invoke Callback function */
        aConfig->canRxInd(aID, pRxMsg);
        CAN_PutRxPkt(aID);
    }
    return retVal;
}

/**
    @brief Process transmit packets for defferred can processing
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_ProcessXmtdPkts(CAN_HwIDType aID, const CAN_ConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    CAN_TxMsgObjType txMsg;

    retVal = CAN_McanGetTxMsg(aID, &txMsg);
    if (BCM_ERR_OK == retVal) {
        /* Invoke Callback function */
        aConfig->canTxDoneInd(aID, &txMsg);
    }
    return retVal;
}


/**
    @brief Process error packets for defferred can processing
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_ProcessErrPkts(CAN_HwIDType aID, const CAN_ConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    CAN_ErrorType errCode = 0UL;

    retVal = CAN_McanGetErrCode(aID, &errCode);
    if (BCM_ERR_OK == retVal) {
        /* Invoke Callback function */
        aConfig->canErrorInd(aID, errCode);
    }

    return retVal;
}


/**
    @brief  Process Bus_off interrupt from MCAN

    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_ProcessBusOff(CAN_HwIDType aID, const CAN_ConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t raiseBusOffCb = 0UL;


    retVal = CAN_McanIsBusOff(aID, &raiseBusOffCb);
    if (BCM_ERR_OK == retVal) {
        if (0UL != raiseBusOffCb) {
            /* Invoke Callback function */
            aConfig->canBusOffInd(aID);

            CAN_McanBusOffRecovery(aID);

        }
    }

    return retVal;
}

/** @brief Handle deferred CAN processing

    This interface handles deferred CAN processing. This interface shall be
    called to process the CAN Packets (TX/RX), error and bus-off from
    Client task.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller
    @param[in]      aConfig     Pointer to CAN driver configuration

    @return     int32_t

    @post TBD

    @note This interface shall be called from Client Task, whenever Client Task is
    signalled from the CAN driver.
    @limitations None
    @trace  #BRCM_SWARCH_CAN_DRV_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
int32_t CAN_DrvProcessPkts(CAN_HwIDType aID, const CAN_ConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    do {
        retVal = CAN_ProcessRcvdPkts(aID, aConfig);
    } while (BCM_ERR_OK == retVal);

    do {
        retVal = CAN_ProcessXmtdPkts(aID, aConfig);
    } while (BCM_ERR_OK == retVal);

    do {
        retVal = CAN_ProcessErrPkts(aID, aConfig);
    } while (BCM_ERR_OK == retVal);

    retVal = CAN_ProcessBusOff(aID, aConfig);

    return retVal;
}

/**
    @brief CAN IRQ LINE0 Handler

    @trace  #BRCM_SWARCH_CAN_IRQLINE0HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
int32_t CAN_IRQLine0Handler(CAN_HwIDType aID)
{
    uint32_t intr;
    int32_t retVal = BCM_ERR_OK;
    if ((CAN_MAX_HW_ID <= aID)
        || (CAN_MCAN_STATE_RUNNING != CAN_McanRWDev[aID].state)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {

        intr = CAN_McanRegs[aID].REGS->ir;
        /* Clear the interrupt */
        CAN_McanRegs[aID].REGS->ir = intr;

        if (0UL != intr) {
            if (0UL != (intr & (CAN_0_IR_RF0N_MASK | CAN_0_IR_RF1N_MASK | CAN_0_IR_DRX_MASK))) {
                CAN_McanProcessCanRx(aID, intr);
            }
            if (0UL != (intr & CAN_0_IR_TEFN_MASK)) {
                /* TX Event FIFO New Entry */
                CAN_McanProcessCanTx(aID, intr);
            }
        }
    }

    return retVal;
}

/**
    @brief [Usage of CAN_IRQLine1_Handler]

    @trace  #BRCM_SWARCH_CAN_IRQLINE1HANDLER_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/

int32_t CAN_IRQLine1Handler(CAN_HwIDType aID)
{
    uint32_t intr;
    uint32_t psr;
    int32_t retVal = BCM_ERR_OK;

    if ((CAN_MAX_HW_ID <= aID)
        || (CAN_MCAN_STATE_RUNNING != CAN_McanRWDev[aID].state)) {
        retVal = BCM_ERR_INVAL_PARAMS;

    } else {

        intr = CAN_McanRegs[aID].REGS->ir;
        /* Clear the interrupt */
        CAN_McanRegs[aID].REGS->ir = intr;

        psr = CAN_McanRegs[aID].REGS->psr;
        if (0UL != intr) {
            if (0UL < (intr & (CAN_0_IR_PEA_MASK | CAN_0_IR_PED_MASK))) {
                CAN_McanProcessCanError(aID, intr);
            }

            if ((0UL < (intr & CAN_0_IR_BO_MASK)) && (0UL != (psr & CAN_0_PSR_BO_MASK))) {
                /* Set flag for BusOff Handler */
                CAN_McanRWDev[aID].busOff = 1UL;

                retVal = CAN_McanSetCCE(aID);
                if (BCM_ERR_OK == retVal) {
                    retVal = CAN_McanCancelTxAll(aID);   /* Cancel Pending Transmissions */
                }
            }
        }
    }
    return retVal;
}

/** @brief Enable the CAN interrupts.

    This API enables/disables the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the CAN controller
    @param[in]      intrType      Interrupt type
    @param[in]      aEnable       Enable (1) or disable (0) the selected interrupt type

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   CAN driver interrupt enable is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_CAN_INIT
    @trace  #BRCM_SWARCH_CAN_DRV_ENABLE_INTERRUPT_PROC

    @limitations None

    @code{.unparsed}
       Enable the interrupt type depending on interrupt type specified in
       intrType
       Option to select all the interrupt or individual interrupt
    @endcode
*/
int32_t CAN_DrvEnableInterrupt(CAN_HwIDType aId, CAN_InterruptType intrType, uint32_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t intMask = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (CAN_MAX_HW_ID <= aId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CAN_MCAN_STATE_UNINIT == CAN_McanRWDev[aId].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        if(intrType == CAN_INTERRUPT_ALL_INTERRUPT) {
            if(aEnable > 0UL) {
                CAN_McanEnAllInterrupts(aId);
            } else {
                CAN_McanDisAllInterrupts(aId);
            }
        } else {
            if((intrType & CAN_INTERRUPT_RXFIFO0_NEW_MSG) > 0UL) {
                intMask |= CAN_0_IE_RF0NE_MASK;
            }
            if((intrType & CAN_INTERRUPT_RXFIFO1_NEW_MSG) > 0UL) {
                intMask |= CAN_0_IE_RF1NE_MASK;
            }
            if((intrType & CAN_INTERRUPT_TXFIFO_NEW_MSG) > 0UL) {
                intMask |= CAN_0_IE_TEFNE_MASK;
            }
            if((intrType & CAN_INTERRUPT_TRANSMIT_COMPLETE) > 0UL) {
                intMask |= CAN_0_IE_TCE_MASK;
            }
            if((intrType & CAN_INTERRUPT_DRX_BUFF_MSG) > 0UL) {
                intMask |= CAN_0_IE_DRXE_MASK;
            }
            if((intrType & CAN_INTERRUPT_PROTOCOL_BUS_OFF_STATUS) > 0UL) {
                intMask |= CAN_0_IE_BOE_MASK;
            }
            if((intrType & CAN_INTERRUPT_PROTOCOL_ERROR_DATA_PHASE) > 0UL) {
                intMask |= CAN_0_IE_PEDE_MASK;
            }
            if((intrType & CAN_INTERRUPT_PROTOCOL_ERROR_ARBITRATION_PHASE) > 0UL) {
                intMask |= CAN_0_IE_PEAE_MASK;
            }

            if(aEnable > 0UL) {
                CAN_McanRegs[aId].REGS->ie = intMask;
            } else {
                CAN_McanRegs[aId].REGS->ie &= ~intMask;
            }
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Clear the interrupts

    This API clears the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the CAN controller
    @param[in]      intrType      Clear the selected interrupt types

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK    CAN interrupt clear is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_CAN_INIT
    @trace  #BRCM_SWARCH_CAN_DRV_CLEAR_INTERRUPT_PROC

    @limitations None

    @code{.unparsed}
       Clear the interrupt status by writing 1 to corresponding bits
    @endcode
*/
int32_t CAN_DrvClearInterrupt(CAN_HwIDType aId, CAN_InterruptType intrType)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t intMask = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (CAN_MAX_HW_ID <= aId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CAN_MCAN_STATE_UNINIT == CAN_McanRWDev[aId].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        if(intrType == CAN_INTERRUPT_ALL_INTERRUPT) {
            CAN_McanRegs[aId].REGS->ir |= CAN_IE_ALL_MASK;
        } else {
            if(intrType == CAN_INTERRUPT_RXFIFO0_NEW_MSG) {
                intMask |= CAN_0_IR_RF0N_MASK;
            }
            if(intrType == CAN_INTERRUPT_RXFIFO1_NEW_MSG) {
                intMask |= CAN_0_IR_RF1N_MASK;
            }
            if(intrType == CAN_INTERRUPT_TXFIFO_NEW_MSG) {
                intMask |= CAN_0_IR_TEFN_MASK;
            }
            if(intrType == CAN_INTERRUPT_TRANSMIT_COMPLETE) {
                intMask |= CAN_0_IR_TC_MASK;
            }
            if(intrType == CAN_INTERRUPT_DRX_BUFF_MSG) {
                intMask |= CAN_0_IR_DRX_MASK;
            }
            if(intrType == CAN_INTERRUPT_PROTOCOL_BUS_OFF_STATUS) {
                intMask |= CAN_0_IR_BO_MASK;
            }
            if(intrType == CAN_INTERRUPT_PROTOCOL_ERROR_DATA_PHASE) {
                intMask |= CAN_0_IR_PED_MASK;
            }
            if(intrType == CAN_INTERRUPT_PROTOCOL_ERROR_ARBITRATION_PHASE) {
                intMask |= CAN_0_IR_PEA_MASK;
            }

            CAN_McanRegs[aId].REGS->ir |= intMask;

        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @brief Get the CAN interrupt status

    This API gets the interrupt status for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID            Index of the CAN controller
    @param[in]     intrType      Get the interrupt status for the selected interrupt types
    @param[out]    intrStatus    Get CAN interrupt status as bit field corresponding to CAN IR register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   CAN driver interrupt get successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_CAN_INIT
    @trace  #BRCM_SWARCH_CAN_DRV_GET_INTERRUPT_STATUS_PROC

    @limitations None
    @code{.unparsed}
       Get the interrupt status by reading CAN IR register and update the corresponding bits
    @endcode
*/
int32_t CAN_DrvGetInterruptStatus(CAN_HwIDType aId, CAN_InterruptType intrType, uint32_t *intrStatus)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t status = 0UL;
#ifdef BCM8915X_INDIVIDUAL_INTR
    uint32_t psr = CAN_McanRegs[aId].REGS->psr;
#endif
#ifdef BCM8915X_PARAM_VALIDATION
    if (CAN_MAX_HW_ID <= aId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CAN_MCAN_STATE_UNINIT == CAN_McanRWDev[aId].state) {
        retVal = BCM_ERR_UNINIT;
    } else {
#endif
        status = CAN_McanRegs[aId].REGS->ir;

        if(intrType == CAN_INTERRUPT_ALL_INTERRUPT) {
            *intrStatus = status;
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else {
            if(intrType == CAN_INTERRUPT_RXFIFO0_NEW_MSG) {
                *intrStatus |= (status & CAN_0_IR_RF0N_MASK);
            }
            if(intrType == CAN_INTERRUPT_RXFIFO1_NEW_MSG) {
                *intrStatus |= (status & CAN_0_IR_RF1N_MASK);
            }
            if(intrType == CAN_INTERRUPT_TXFIFO_NEW_MSG) {
                *intrStatus |= (status & CAN_0_IR_TEFN_MASK);
            }
            if(intrType == CAN_INTERRUPT_TRANSMIT_COMPLETE) {
                *intrStatus |= (status & CAN_0_IR_TC_MASK);
            }
            if(intrType == CAN_INTERRUPT_DRX_BUFF_MSG) {
                *intrStatus |= (status & CAN_0_IR_DRX_MASK);
            }
            if(intrType == CAN_INTERRUPT_PROTOCOL_BUS_OFF_STATUS) {
                *intrStatus |= ((((status & CAN_0_IR_BO_MASK) > 0UL) && ((psr & CAN_0_PSR_BO_MASK) > 0UL)) ? CAN_INTERRUPT_PROTOCOL_BUS_OFF_STATUS : 0UL);
            }
            if(intrType == CAN_INTERRUPT_PROTOCOL_ERROR_DATA_PHASE) {
                *intrStatus |= (status & CAN_0_IR_PED_MASK);
            }
            if(intrType == CAN_INTERRUPT_PROTOCOL_ERROR_ARBITRATION_PHASE) {
                *intrStatus |= (status & CAN_0_IR_PEA_MASK);
            }
        }
#endif

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/** @} */

