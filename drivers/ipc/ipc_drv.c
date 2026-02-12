/*****************************************************************************
 Copyright 2025 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_ipcdrv_impl Implementation
    @ingroup grp_ipcdrv

    @addtogroup grp_ipcdrv_impl
    @{
    @file ipcdrv.c
    @brief IPC driver implementation

    @version 0.1 Initial version
*/

#include "ipc_rdb.h"
#include <ipc.h>
#include <stdlib.h>
#include <bcm_err.h>
#include <compiler.h>
#include <bcm_utils.h>

/**
    @name IPC driver design IDs
    @{
    @brief Design IDs for the IPC driver
*/
#define BRCM_SWDSGN_IPC_INIT_STATE_GLOBAL           (0xB001U) /**< @brief #IPC_DrvInitState          */
#define BRCM_SWDSGN_IPC_REGS_GLOBAL                 (0xB002U) /**< @brief #IPC_Regs                  */
#define BRCM_SWDSGN_IPC_DRV_IRQ_HANDLER_TX_PROC     (0xB010U) /**< @brief #IPC_DrvIRQHandlerTx       */
#define BRCM_SWDSGN_IPC_DRV_IRQ_HANDLER_RX_PROC     (0xB011U) /**< @brief #IPC_DrvIRQHandlerRx       */
#define BRCM_SWDSGN_IPC_DRV_INIT_PROC               (0xB012U) /**< @brief #IPC_DrvInit               */
#define BRCM_SWDSGN_IPC_DRV_SEND_MSG_PROC           (0xB013U) /**< @brief #IPC_DrvSendMsg            */
#define BRCM_SWDSGN_IPC_DRV_RECV_MSG_PROC           (0xB014U) /**< @brief #IPC_DrvRecvMsg            */
#define BRCM_SWDSGN_IPC_DRV_ACK_PROC                (0xB015U) /**< @brief #IPC_DrvAck                */
#define BRCM_SWDSGN_IPC_DRV_DEINIT_PROC             (0xB016U) /**< @brief #IPC_DrvDeinit             */
#define BRCM_SWDSGN_IPC_DRV_CONCLUDE_PROC           (0xB017U) /**< @brief #IPC_DrvConclude           */
#define BRCM_SWDSGN_IPC_INT_LOG2N_PROC              (0xB018U) /**< @brief #IPC_Intlog2n           */
/** @} */

/** @brief Pointer to the IPC registers block

    @trace #BRCM_SWARCH_IPC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_IPC_DRV_SEND_MSG_PROC
    @trace #BRCM_SWARCH_IPC_DRV_RECV_MSG_PROC
    @trace #BRCM_SWARCH_IPC_DRV_ACK_PROC
    @trace #BRCM_SWARCH_IPC_DRV_CONCLUDE_PROC
    @trace #BRCM_SWARCH_IPC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_IPC_DRV_IRQ_HANDLER_TX_PROC
    @trace #BRCM_SWARCH_IPC_DRV_IRQ_HANDLER_RX_PROC
    @trace #BRCM_SWREQ_IPC
*/
static IPC_RDBType *const IPC_Regs = (IPC_RDBType*)IPC_BASE;

/** @brief IPC driver init state global

    @trace #BRCM_SWARCH_IPC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_IPC_DRV_SEND_MSG_PROC
    @trace #BRCM_SWARCH_IPC_DRV_RECV_MSG_PROC
    @trace #BRCM_SWARCH_IPC_DRV_ACK_PROC
    @trace #BRCM_SWARCH_IPC_DRV_CONCLUDE_PROC
    @trace #BRCM_SWARCH_IPC_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_IPC
*/
static BCM_BoolType  IPC_DrvInitState  COMP_SECTION(".bss.drivers");

/**
    @brief Initialize the IPC driver

    @behaviour Sync, Non-reentrant

    @pre None

    @param[in]      aProc   IPC processor ID

    @retval     #BCM_ERR_OK               Init successful
    @retval     #BCM_ERR_INVAL_STATE      Already initialized

    @post None

    @trace #BRCM_SWREQ_IPC
    @trace #BRCM_SWARCH_IPC_DRV_INIT_PROC

    @code{.unparsed}
    Check driver initialization state.
    Set driver state as TRUE
    @endcode

*/
int32_t  IPC_DrvInit(IPC_ProcIDType aProc)
{
    int32_t ret = BCM_ERR_OK;

    if (BCM_BOOL_TRUE == IPC_DrvInitState) {
        ret = BCM_ERR_INVAL_STATE;
    } else {
        IPC_DrvInitState = BCM_BOOL_TRUE;
        ret = BCM_ERR_OK;
    }

    return ret;
}

/**
    @brief Deinit the IPC driver

    @behaviour Sync, Non-reentrant

    @pre None

    @retval     #BCM_ERR_OK               Deinit successful
    @retval     #BCM_ERR_UNINIT           Driver has not been initialized

    @post None

    @trace #BRCM_SWREQ_IPC
    @trace #BRCM_SWARCH_IPC_DRV_DEINIT_PROC

    @code{.unparsed}

    Check driver initialization state.
    Set driver state as FALSE

    @endcode
*/
int32_t  IPC_DrvDeinit(void)
{
    int32_t ret;

    if (BCM_BOOL_FALSE == IPC_DrvInitState) {
        ret = BCM_ERR_UNINIT;
    } else {
        IPC_DrvInitState = BCM_BOOL_FALSE;
        ret = BCM_ERR_OK;
    }

    return (ret);
}


/**
    @brief Set M0 policy and/or send a message over an IPC channel

    @pre None

    @param[in]     aSrcProc        IPC source rocessor ID
    @param[in]     aDstProc        IPC destination processor ID
    @param[inout]  aMsg            Message to send cmd/data or update M0 policy, to update the policy use
                                   attrib as IPC_MSG_ATTRIB_POLICY
    
    @retval     #BCM_ERR_OK               Message sent successfully
    @retval     #BCM_ERR_UNINIT           Driver has not been initialized
    @retval     #BCM_ERR_INVAL_PARAMS     aMsg is NULL or aSrcProc/aDstProc not exist
    @retval     #BCM_ERR_BUSY             Resources busy. Caller may retry later

    @post None

    @note After a successful send, the caller may monitor the reply field in the message
        header to determine if the remote party has finished processing the message. The driver
        shall facilitate this monitoring by sending (OS) event notifications to the caller to
        indicate that the reply bit may have been set.

    @trace #BRCM_SWREQ_IPC
    @trace #BRCM_SWARCH_IPC_DRV_SEND_MSG_PROC

    @code{.unparsed}

    Check driver initialization state, message, aSrcProc and aDstProc.
    Find the available channel
    Configure IPC policy, Interrupt configuration and door bell with command/data
    Update channel number on aMsg

    @endcode

*/
int32_t
IPC_DrvSendMsg(const IPC_ProcIDType aSrcProc,
               const IPC_ProcIDType aDstProc, IPC_MsgType *aMsg)
{
    IPC_ChIDType  ch;
    int32_t       ret = BCM_ERR_OK;
    uint32_t cmdData = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (BCM_BOOL_FALSE == IPC_DrvInitState) {
        ret = BCM_ERR_UNINIT;
    } else if ( (NULL == aMsg) || (IPC_PROC_ID_MAX <= aSrcProc)
            || (IPC_PROC_ID_MAX <= aDstProc) ) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif /* PARAM_VALIDATION */
        if (aMsg != NULL) {
            if ((aMsg->attrib & IPC_MSG_ATTRIB_POLICY) > 0UL) {
                for ( ch = 0; ch < IPC_NUM_CHANNELS; ch++ ) {
                    if ( IPC_DOORBELL_FULL_MASK != (IPC_DOORBELL_FULL_MASK & IPC_Regs->doorbell[ch]) ) {
                        break;   /* found a channel available */
                    }
                }
                if ( ch >= IPC_NUM_CHANNELS ) {
                    ret = BCM_ERR_BUSY;   /* no channel available currently */
                } else {
                    /* @event           CERT-C INT34
                       @issue          Coverity infers that the LS would overflow
                       @risk           No risk
                       @justification  aDstProc got validated.
                                       aDstProc would not exceed totalSize  */
                    /* coverity[cert_int34_c_violation] */
                    IPC_Regs->ipc_policy[ch]  |=  ((IPC_MSG_ATTRIB_PRIVILEGED | IPC_MSG_ATTRIB_NORMAL) & (aMsg->attrib)) | (((uint32_t)0x1UL) << aSrcProc) | (((uint32_t)0x1UL) << aDstProc);
                    /* @event           CERT-C INT34
                       @issue          Coverity infers that the LS would overflow
                       @risk           No risk
                       @justification  aDstProc got validated.
                                       aDstProc would not exceed totalSize  */
                    /* coverity[cert_int34_c_violation] */
                    IPC_Regs->ipc_int_cfg[ch] |=  (0x1UL) << aDstProc;
                    aMsg->chId = ch;   /* remember the channel ID assigned */
                    ret = BCM_ERR_OK;  
                }
            } 
            if ((ret == BCM_ERR_OK) && ((aMsg->attrib & (IPC_MSG_ATTRIB_CMD|IPC_MSG_ATTRIB_DATA)) > 0UL)) {
                /* write IPC_DOORBELL_FULL_MASK to Doorbell register can trigger an IPC Tx interrupt */
                cmdData = ((IPC_MSG_ATTRIB_CMD == (IPC_MSG_ATTRIB_CMD & (aMsg->attrib)))
                        ? ((aMsg->cmd) << IPC_DOORBELL_CONTENT_SHIFT)  | (IPC_DOORBELL_FULL_MASK)
                        : ((uint32_t)(((uint32_t)aMsg->data) << IPC_DOORBELL_CONTENT_SHIFT) | (IPC_DOORBELL_CMD_OR_PTR_MASK)
                            | IPC_DOORBELL_FULL_MASK));
                IPC_Regs->doorbell[aMsg->chId] = cmdData;
                ret = BCM_ERR_OK;  
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return (ret);
}


/**
    @brief Recieve a message corresponding to a given IPC processor ID

    @pre None

    @param[in]  aDstProc        IPC destination processor ID
    @param[in]  aChId           IPC channel ID
    @param[out] aMsg            Pointer to the recieved message

    @retval     #BCM_ERR_OK               Message corresponding to the sender processor populated
    @retval     #BCM_ERR_UNINIT           Driver has not been initialized
    @retval     #BCM_ERR_INVAL_PARAMS     aMsg is NULL or aDstProc not exist
    @retval     #BCM_ERR_NOT_FOUND        No messages

    @post None

    @note  After a successful recieve and subsequent processing of the recieved message,
           the caller needs to issue a call to #IPC_DrvAck to signal the remote party
           that the command processing is complete.

    @trace #BRCM_SWREQ_IPC
    @trace #BRCM_SWARCH_IPC_DRV_RECV_MSG_PROC

    @code{.unparsed}

    Check driver initialization state, aMsg, aDstProc.
    Based on aDstProc find the channel number.
    Read channel doorbell register.
    Based on command or data read command value or data address

    @endcode


*/
int32_t
IPC_DrvRecvMsg(const IPC_ProcIDType aDstProc, IPC_ChIDType aChId, IPC_MsgType *const aMsg)
{
    IPC_ChIDType  ch = aChId;
    uint32_t      procBmp = 0UL;
    int32_t       ret = BCM_ERR_OK;
  #ifdef BCM8915X_PARAM_VALIDATION
    if (BCM_BOOL_FALSE == IPC_DrvInitState) {
        ret = BCM_ERR_UNINIT;
    } else if ( (NULL == aMsg) || (IPC_PROC_ID_MAX <= aDstProc) ) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
  #endif /* PARAM_VALIDATION */
    /* @event           CERT-C INT34
                @issue          Coverity infers that the LS would overflow
                @risk           No risk
                @justification  aDstProc got validated.
                                aDstProc would not exceed totalSize  */
    /* coverity[cert_int34_c_violation] */
    procBmp = (1UL << (uint32_t)aDstProc);

    if ( IPC_CH_ID_MAX <= ch ) {  /* valid channel ID not given */
        /* seeking the channel holding the IPC message for the destination Processor (aDstProc) */
        for ( ch = 0; ch < IPC_NUM_CHANNELS; ch++ ) {
            if ( (procBmp & IPC_Regs->ipc_int_cfg[ch]) != 0x0U ) {
                ret = BCM_ERR_OK;      /* found the channel holding desired IPC message */
                break;
            }
        }
        if ( ch >= IPC_NUM_CHANNELS ) {
            ret = BCM_ERR_NOT_FOUND;   /* found no  channel holding desired IPC message */
        }
    }

    if ( BCM_ERR_OK == ret ) { /* given aChId is valid, or found the desired channel successfully */
        IPC_DOORBELL_TYPE   dbell = IPC_Regs->doorbell[ch];
        if ( (dbell & IPC_DOORBELL_CMD_OR_PTR_MASK) > 0UL) {
            aMsg->attrib =  IPC_MSG_ATTRIB_DATA;
            aMsg->data   =  (uint8_t*) (dbell >> IPC_DOORBELL_CONTENT_SHIFT);
       } else {
           aMsg->attrib =  IPC_MSG_ATTRIB_CMD;
           aMsg->cmd    =  dbell >> IPC_DOORBELL_CONTENT_SHIFT;

       }
        aMsg->chId = ch;
    }

  #ifdef BCM8915X_PARAM_VALIDATION
    }
  #endif
    return (ret);
}


/**
    @brief Signal the sender prpcessor that the processing of a previously recieved message is complete

    @pre None

    @param[in]  aDstProc        IPC destination processor ID
    @param[in]  aChId           IPC channel ID

    @retval     #BCM_ERR_OK            Remote party has been intimated of the message processing completion
    @retval     #BCM_ERR_UNINIT        Driver has not been initialized
    @retval     #BCM_ERR_INVAL_PARAMS  aDstProc or aChId not exist

    @post None

    @trace #BRCM_SWREQ_IPC
    @trace #BRCM_SWARCH_IPC_DRV_ACK_PROC

    @code{.unparsed}

    Check driver initialization state, aChId and aDstProc.
    Write doorbell register with 0
    @endcode

*/
int32_t  IPC_DrvAck(const IPC_ProcIDType aDstProc, IPC_ChIDType aChId)
{
    int32_t       ret = BCM_ERR_OK;

  #ifdef BCM8915X_PARAM_VALIDATION
    if (BCM_BOOL_FALSE == IPC_DrvInitState) {
        ret = BCM_ERR_UNINIT;
    } else if ( (aChId    < IPC_CH_ID_MIN  ) || (IPC_CH_ID_MAX   <= aChId   ) ||
                (aDstProc < IPC_PROC_ID_MIN) || (IPC_PROC_ID_MAX <= aDstProc)  ) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
  #endif /* PARAM_VALIDATION */

    IPC_Regs->doorbell[aChId]     =  0UL;
    ret = BCM_ERR_OK;

  #ifdef BCM8915X_PARAM_VALIDATION
    }
  #endif
    return (ret);
}

/**
    @brief Conclude the process of the previous message passing and close the corresponding channel

    @pre None

    @param[in]  aSrcProc        IPC source processor ID
    @param[in]  aChId           IPC channel ID

    @retval     #BCM_ERR_OK               Concluded successfully
    @retval     #BCM_ERR_UNINIT           Driver has not been initialized
    @retval     #BCM_ERR_INVAL_PARAMS     aDstProc or aChId not exist

    @post None

    @trace #BRCM_SWREQ_IPC
    @trace #BRCM_SWARCH_IPC_DRV_CONCLUDE_PROC

    @code{.unparsed}

    Check driver initialization state, aChId and aSrcProc.
    Clear aSrcProc in ipc_policy register

    @endcode

*/
int32_t  IPC_DrvConclude(const IPC_ProcIDType aSrcProc, IPC_ChIDType aChId)
{
    int32_t       ret = BCM_ERR_OK;
  #ifdef BCM8915X_PARAM_VALIDATION
    if (BCM_BOOL_FALSE == IPC_DrvInitState) {
        ret = BCM_ERR_UNINIT;
    } else if ( (aChId    < IPC_CH_ID_MIN  ) || (IPC_CH_ID_MAX   <= aChId   ) ||
                (aSrcProc < IPC_PROC_ID_MIN) || (IPC_PROC_ID_MAX <= aSrcProc)  ) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
  #endif /* PARAM_VALIDATION */
    /* @event           CERT-C INT34
                @issue          Coverity infers that the LS would overflow
                @risk           No risk
                @justification  aSrcProc got validated.
                                aSrcProc would not exceed totalSize  */
    /* coverity[cert_int34_c_violation] */
    IPC_Regs->ipc_policy[aChId] &=  ~(1UL << (uint32_t)aSrcProc);
    ret = BCM_ERR_OK;

  #ifdef BCM8915X_PARAM_VALIDATION
    }
  #endif
    return (ret);
}

/**
    @brief Get bit position

    @trace #BRCM_SWARCH_IPC_DRV_IRQ_HANDLER_RX_PROC
    @trace #BRCM_SWARCH_IPC_DRV_IRQ_HANDLER_TX_PROC
    @trace #BRCM_SWREQ_IPC
*/

static uint32_t IPC_Intlog2n(uint32_t n)
{
    uint32_t chIndex = 0UL;
    uint32_t log2nOut = 0UL;
    for (chIndex = 0UL; chIndex < IPC_NUM_CHANNELS; chIndex ++)
    {
        if ((n & (1UL << chIndex)) > 0UL) {
            log2nOut = chIndex;
            break;
        }
    }
    return log2nOut;
}

/** @brief IPC Tx interrupt handler

    This function to handle the IPC Tx interrupt which is triggered by sending
    a message over an IPC channel. This is a sample interrupt handler

    @behavior Sync, Non-reentrant

    @pre None

    @return     void

    @post None

    @trace #BRCM_SWREQ_IPC
    @trace #BRCM_SWARCH_IPC_DRV_IRQ_HANDLER_TX_PROC

    @code{.unparsed}

    Receive message using IPC_DrvRecvMsg.
    Acknowledge using IPC_DrvAck

    @endcode


    @limitations None
*/
void IPC_DrvIRQHandlerTx(void)
{
    IPC_ChIDType  ch = 0U;
    uint32_t      chBmp = (IPC_Regs->ipc_tx) | (IPC_Regs->ipc_flag);
    IPC_MsgType targetMsg;
    uint32_t aDstMap = 0UL;
    int32_t ret = 0L;

    IPC_ProcIDType aDstProc = 0U;

    for ( ch = 0U; ch < IPC_NUM_CHANNELS; ch++ ) {
        if ( (chBmp & (1UL << (uint32_t)ch)) > 0x0UL ) {
              aDstMap = IPC_Regs->ipc_int_cfg[ch] & IPC_INTCFG_PROCESSORID_MASK;
              aDstProc = (uint8_t)IPC_Intlog2n(aDstMap);
              (void) IPC_DrvRecvMsg(aDstProc, ch, &targetMsg);
              (void) IPC_DrvAck(aDstProc, ch);
        }
    }
    (void) targetMsg;
    (void) ret;
}


/** @brief IPC Rx interrupt handler

    This function to handle the IPC Rx interrupt which is triggered when
    an IPC processor acknowledgs a received IPC message. Sample Rx interrupt handler.

    @behavior Sync, Non-reentrant

    @pre None

    @return     void

    @post None

    @trace #BRCM_SWREQ_IPC
    @trace #BRCM_SWARCH_IPC_DRV_IRQ_HANDLER_RX_PROC

    @code{.unparsed}
     Read Rx interrupt pending register.
     Get the channel Number.
     Source Proc ID is available as part of policy. Update Rx pending register
      based on policy.



    @endcode

    @limitations None
*/
void  IPC_DrvIRQHandlerRx(void)
{
    uint32_t      chNo = 0UL;

    /* GetChannel number*/
    chNo = IPC_Intlog2n(IPC_Regs->ipc_rx);

    /* Clear interrupt*/
    IPC_Regs->ipc_rx |= (IPC_Regs->ipc_rx & IPC_Regs->ipc_policy[chNo]);

}

/** @} */

