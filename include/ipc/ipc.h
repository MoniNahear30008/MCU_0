/*****************************************************************************
 Copyright 2025 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
  @defgroup grp_ipcdrv_ifc  IPC Service
  @ingroup grp_ipcdrv
  @addtogroup grp_ipcdrv_ifc
  @{

  @section sec_ipc_overview  Overview
    The generic IPC (inter-processor communication) mechanism provides up to 16
    channels to serve up to 24 processors, to make those processors can share
    information between each other by sending IPC messages over IPC channels.

  @section sec_ipc_message  IPC Messages
    The contents of an IPC message shall be interpreted as :
    - An IPC command (entire IPC request may fit into a 30-bit word, not using share-memory mailbox). Or
    - A pointer to a share-memory block which contains more complex multi-byte IPC message.

  @section api_op_scenario  IPC operation scenario
    - A requester processor calls @ref IPC_DrvSendMsg API to send an IPC message to the service processor.
        An IPC Tx interrupt will be triggerred to service processor.
    - The Tx interrupt handler of service processor calls @IPC_DrvRecvMsg to retrive the IPC message and acknowledge
      requestor processor by calling @ref IPC_DrvAck through a specific channel ID.
         Refer to sample Tx interrupt handler IPC_DrvIRQHandlerTx.
    - Once service processor acknowledges, Requestor processor gets a Rx interrupt. 
    - Requestor processor Rx handler needs to clear Rx interrupt. Sample handler @ref IPC_DrvIRQHandlerRx.
    - The requestor processor calls @ref IPC_DrvConclude() API to conclude the process of message passing
        and close the corresponding channel.

  @section api_list  List of IPC APIs
    | API Name | Brief Decription   |
    |:-----------:|:---------------:|
    | @ref IPC_DrvInit     | Initialize IPC and register for each IPC processor  |
    | @ref IPC_DrvDeinit   | De-Initialize IPC                                                   |
    | @ref IPC_DrvSendMsg  | API to send IPC message over a channel to the other processor, and trigger an IPC Tx interrupt|
    | @ref IPC_DrvRecvMsg  | API to recsive the IPC message sent from other processors           |
    | @ref IPC_DrvAck      | Acknowledge the message sent from the sender processor, and trigger an IPC Rx interrupt |
    | @ref IPC_DrvConclude | Conclude the process of the previous message passing and close the corresponding channel   |
    | @ref IPC_DrvIRQHandlerTx | IPC sample Tx interrupt handler. |
    | @ref IPC_DrvIRQHandlerRx | IPC sample Rx interrupt handler. |

  @file ipc.h
  @brief IPC interface
  @section apis This section defines the interface for IPC driver
  @version 0.1 Imported from docx
*/

#ifndef IPC_H
#define IPC_H
#include <stdint.h>

/**
    @name IPC driver API IDs
    @{
    @brief Architecture IDs for the IPC driver
*/
#define BRCM_SWARCH_IPC_CH_ID_TYPE         (0x8C01U) /**< @brief #IPC_ChIDType       */
#define BRCM_SWARCH_IPC_PROC_ID_TYPE       (0x8C02U) /**< @brief #IPC_ProcIDType     */
#define BRCM_SWARCH_IPC_MSG_ATTRIB_TYPE    (0x8C03U) /**< @brief #IPC_MsgAttribType  */
#define BRCM_SWARCH_IPC_MSG_TYPE           (0x8C04U) /**< @brief #IPC_MsgType        */

#define BRCM_SWARCH_IPC_DRV_INIT_PROC      (0x8C12U) /**< @brief #IPC_DrvInit     */
#define BRCM_SWARCH_IPC_DRV_DEINIT_PROC    (0x8C13U) /**< @brief #IPC_DrvDeinit   */
#define BRCM_SWARCH_IPC_DRV_SEND_MSG_PROC  (0x8C14U) /**< @brief #IPC_DrvSendMsg  */
#define BRCM_SWARCH_IPC_DRV_RECV_MSG_PROC  (0x8C15U) /**< @brief #IPC_DrvRecvMsg  */
#define BRCM_SWARCH_IPC_DRV_ACK_PROC       (0x8C16U) /**< @brief #IPC_DrvAck      */
#define BRCM_SWARCH_IPC_DRV_CONCLUDE_PROC  (0x8C17U) /**< @brief #IPC_DrvConclude */
#define BRCM_SWARCH_IPC_DRV_IRQ_HANDLER_TX_PROC    (0x8C18U) /**< @brief #IPC_DrvIRQHandlerTx */
#define BRCM_SWARCH_IPC_DRV_IRQ_HANDLER_RX_PROC    (0x8C19U) /**< @brief #IPC_DrvIRQHandlerRx */
/** @} */

/**
    @name IPC_ChIDType
    @{
    @brief Index of the Channels for passing IPC messages

    @trace #BRCM_SWREQ_IPC
 */
typedef uint8_t IPC_ChIDType;        /**< @brief IPC channel ID type          */
#define IPC_CH_ID_MAX       (16U)   /**< @brief 16 IPC channels totally      */
#define IPC_CH_ID_MIN       ( 0U)   /**< @brief IPC channel ID can be 0 - 15 */
/** @} */

/**
    @name IPC_ProcIDType
    @{
    @brief Index of the Processors using IPC

    @trace #BRCM_SWREQ_IPC
 */
typedef uint32_t IPC_ProcIDType;                              /**< @brief IPC Processor ID type  */
#define IPC_PROC_ID_MCU0           (0U)                      /**< @brief MCU0 PROC ID*/
#define IPC_PROC_ID_MCU_UTLT       (1U)                      /**< @brief MCU_UTLT PROC ID*/
#define IPC_PROC_ID_A55            (2U)                      /**< @brief A55  PROC ID*/
#define IPC_PROC_ID_PCIE_MST       (9U)                     /**< @brief PCIe master*/
#define IPC_PROC_ID_Q8_DATA_0      (12U)                     /**< @brief Q8_DATA_0PROC ID*/
#define IPC_PROC_ID_Q8_DATA_1      (13U)                     /**< @brief Q8_DATA_1PROC ID*/
#define IPC_PROC_ID_Q8_DATA_2      (14U)                     /**< @brief Q8_DATA_2PROC ID*/
#define IPC_PROC_ID_Q8_DATA_3      (15U)                     /**< @brief Q8_DATA_3PROC ID*/
#define IPC_PROC_ID_RESERVED       (16U)                     /**< @brief Q8_DATA_3PROC ID*/
#define IPC_PROC_ID_MAX            (IPC_PROC_ID_RESERVED)   /**< @brief MAX ID      */
#define IPC_PROC_ID_MIN            (IPC_PROC_ID_MCU0)        /**< @brief IPC processors ID can be 0 - 23 */
/** @} */

/**
    @name IPC_MsgAttribType
    @{
    @brief IPC message attributes

    @trace #BRCM_SWREQ_IPC
*/
typedef uint32_t IPC_MsgAttribType;               /**< @brief IPC message arrtibute type        */
#define IPC_MSG_ATTRIB_PRIVILEGED (0x01UL << 31UL) /**< @brief Allow privileged write to channel */
#define IPC_MSG_ATTRIB_NORMAL     (0x01UL << 30UL) /**< @brief Allow   normal   write to channel */
#define IPC_MSG_ATTRIB_CMD        (0x01UL <<  1UL) /**< @brief IPC message is a command          */
#define IPC_MSG_ATTRIB_DATA       (0x01UL <<  3UL) /**< @brief IPC message is a pointer to share memory address */
#define IPC_MSG_ATTRIB_POLICY     (0x01UL <<  2UL) /**< @brief IPC policy update*/
/** @} */

/**
    @name IPC_MsgType
    @{
    @brief IPC Message data structure type

    @trace #BRCM_SWREQ_IPC
*/
typedef struct sIPC_MsgType {
    IPC_MsgAttribType  attrib;  /**< @brief IPC message attribute   */
    IPC_ChIDType       chId;    /**< @brief IPC channel ID          */
    union {
        uint32_t  cmd;   /**< @brief An IPC commands (entire IPC request may fit into a 30-bit word, not using share-memory mailbox) */
        uint8_t *data;  /**< @brief A pointer to a share-memory block which contains more complex multi-byte IPC message */
    };
} IPC_MsgType;
/** @} */


/**
    @brief Initialize the IPC driver

    @behaviour Sync, Non-reentrant

    @pre None

    @param[in]      aProc   IPC processor ID

    @retval     #BCM_ERR_OK               Init successful
    @retval     #BCM_ERR_INVAL_STATE      Already initialized

    @post None

    @trace #BRCM_SWREQ_IPC
*/
int32_t IPC_DrvInit(IPC_ProcIDType aProc);

/**
    @brief Deinit the IPC driver

    @behaviour Sync, Non-reentrant

    @pre None

    @retval     #BCM_ERR_OK               Deinit successful
    @retval     #BCM_ERR_UNINIT           Driver has not been initialized

    @post None

    @trace #BRCM_SWREQ_IPC
*/
int32_t IPC_DrvDeinit(void);

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
*/
int32_t IPC_DrvSendMsg(const IPC_ProcIDType aSrcProc,
                       const IPC_ProcIDType aDstProc, IPC_MsgType *aMsg);

/**
    @brief Recieve a message corresponding to a given IPC processor ID

    @pre None

    @param[in]  aDstProc        IPC destination processor ID
    @param[in]  aChId           IPC channel ID
    @param[out] aMsg            Pointer to the recieved message

    @retval     #BCM_ERR_OK               Message corresponding to the given service ID
                                          populated
    @retval     #BCM_ERR_UNINIT           Driver has not been initialized
    @retval     #BCM_ERR_INVAL_PARAMS     aMsg is NULL or aDstProc not exist
    @retval     #BCM_ERR_NOT_FOUND        No messages

    @post None

    @note  After a successful recieve and subsequent processing of the recieved message,
           the caller needs to issue a call to #IPC_DrvAck to signal the remote party
           that the command processing is complete.

    @trace #BRCM_SWREQ_IPC
*/
int32_t  IPC_DrvRecvMsg(const IPC_ProcIDType aDstProc, IPC_ChIDType aChId, IPC_MsgType *const aMsg);

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
*/
int32_t  IPC_DrvAck(const IPC_ProcIDType aDstProc, IPC_ChIDType aChId);

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
*/
int32_t  IPC_DrvConclude(const IPC_ProcIDType aSrcProc, IPC_ChIDType aChId);

/** @brief IPC Tx interrupt handler

    This function to handle the IPC Tx interrupt which is triggered by sending
    a message over an IPC channel. Sample Tx Interrupt handler

    @behavior Sync, Non-reentrant

    @pre None

    @return     void

    @post None

    @trace #BRCM_SWREQ_IPC

    @limitations None
*/
void  IPC_DrvIRQHandlerTx(void);

/** @brief IPC Rx interrupt handler

    This function to handle the IPC Rx interrupt which is triggered when
    an IPC processor acknowledgs a received IPC message. Sample Rx interrupt handler

    @behavior Sync, Non-reentrant

    @pre None

    @return     void

    @post None

    @trace #BRCM_SWREQ_IPC

    @limitations None
*/
void  IPC_DrvIRQHandlerRx(void);

#endif /* IPC_H */

/** @} */

