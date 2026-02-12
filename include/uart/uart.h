/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_uartdrv_ifc Universal Asynchronous Receiver Transmitter (UART)
    @ingroup grp_uartdrv

    @addtogroup grp_uartdrv_ifc
    @{
    @section sec_uart_overview Overview
    UART Bare Metal driver interface provides layer 1 API to initialize and configure
    the HW in desired configuration. It is configured in non-blocking mode with interrupt enabled.
    The transfer APIs initiates the transfer and returns immediately. The transfer completion status
    is notified by the interrupt. The application can use callback to get notified when the transfer
    is complete.

    - UART initialization routine needs to be called for each HW instance.
    - UART controller is out of reset during system initialization to ensure defined
    state of controller.


    @note Please refer to respective technical reference manual for more details about the HW features.

    @note
    UART driver can be used by logging framework, shell and/or by any other
    client who wants to communicate over UART.

    @section sec_uart_fsm Usage
    -  UART HW instance is accessed based on HwID passed as first parameter in each API.
       UART controller base address is accessed based on index HwID passed as 0 to (UART_MAX_HW_ID - 1)
       depending on HW instance.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   UART_0      |
       | 1           |   UART_1      |

    -  UART_DrvInit is called to initialize the UART controller HW instance
       in desired configuration.
    -  UART_DrvDeInit is called to un-initialize the UART controller HW instance
       and free any resources of previous instance.
    -  Interrupt handler function needs to be registered by the platform SW integration.
    -  TX and RX callback functions need to be registered and called by the integration layer
       to get interrupt completion status.
    -  UART_DrvSend is called once UART is in initialized state.

    @limitations UART driver is designed to work in interrupt mode or DMA transfer mode.
    For DMA transfer mode user has to setup DMA transfer using peripheral DMA (PL08X)
    and call UART DMA enable request API (UART_DrvDMAReqSendEnable/UART_DrvDMAReqReceiveEnable).
    @section api_list List of UART APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref UART_DrvInit | Initialize UART |
    | @ref UART_DrvSend | Send data over UART |
    | @ref UART_DrvReceive | Receive data over UART |
    | @ref UART_DrvGetTxStatus | Checks for transmit buffer status and transmit FIFO status |
    | @ref UART_DrvDeInit | De-initialize UART |
    | @ref UART_DrvSendBreak | Send Break |
    | @ref UART_DrvClrBreak | Clear Break |
    | @ref UART_DrvGetState | Get the state of the UART |
    | @ref UART_DrvDMAReqSendEnable | Request transmit DMA Enable |
    | @ref UART_DrvDMAReqReceiveEnable | Request receive DMA Enable |
    | @ref UART_IRQHandler | UART IRQ Handle |
    | @ref UART_EnableInterrupt | Enable UART interrupt |
    | @ref UART_ClearInterrupt | Clear UART interrupt |
    | @ref UART_GetInterruptStatus | Get UART interrupt status |

    @file uart.h
    @brief Interface for UART driver
    @section apis This section defines the interface for UART driver
    @version 0.1 Imported from docx
*/

#ifndef UART_H
#define UART_H

#include <stdint.h>

/**
    @name UART driver API IDs
    @{
    @brief API IDs for UART drier
 */
#define BRCM_SWARCH_UART_HWID_TYPE                       (0x8201U)    /**< @brief #UART_HwIDType     */
#define BRCM_SWARCH_UART_FIFO_LVL_TYPE                    (0x8202U)    /**< @brief #UART_FifoLvlType  */
#define BRCM_SWARCH_UART_BAUD_TYPE                        (0x8203U)    /**< @brief #UART_BaudType     */
#define BRCM_SWARCH_UART_STOP_BITS_TYPE                   (0x8204U)    /**< @brief #UART_StopBitsType */
#define BRCM_SWARCH_UART_PARITY_TYPE                      (0x8205U)    /**< @brief #UART_ParityType   */
#define BRCM_SWARCH_UART_STATE_TYPE                       (0x8206U)    /**< @brief #UART_StateType    */
#define BRCM_SWARCH_UART_TX_STATUS_TYPE                   (0x8207U)    /**< @brief #UART_TxStatusType */
#define BRCM_SWARCH_UART_ERROR_TYPE                       (0x8208U)    /**< @brief #UART_ErrorType    */
#define BRCM_SWARCH_UART_INTERRUPT_TYPE                   (0x820AU)    /**< @brief #UART_InterruptType */
#define BRCM_SWARCH_UART_CONFIG_TYPE                      (0x820EU)    /**< @brief #UART_ConfigType    */
#define BRCM_SWARCH_UART_DRV_INIT_PROC                    (0x820FU)    /**< @brief #UART_DrvInit         */
#define BRCM_SWARCH_UART_DRV_SEND_PROC                    (0x8210U)    /**< @brief #UART_DrvSend         */
#define BRCM_SWARCH_UART_DRV_GET_TX_STATUS_PROC           (0x8211U)    /**< @brief #UART_DrvGetTxStatus  */
#define BRCM_SWARCH_UART_DRV_DEINIT_PROC                  (0x8212U)    /**< @brief #UART_DrvDeInit       */
#define BRCM_SWARCH_UART_DRV_SEND_BREAK_PROC              (0x8214U)    /**< @brief #UART_DrvSendBreak    */
#define BRCM_SWARCH_UART_DRV_CLR_BREAK_PROC               (0x8215U)    /**< @brief #UART_DrvClrBreak     */
#define BRCM_SWARCH_UART_DRV_GET_STATE_PROC               (0x8216U)    /**< @brief #UART_DrvGetState     */
#define BRCM_SWARCH_UART_DRV_DMA_REQ_SEND_ENABLE_PROC     (0x8217U)    /**< @brief #UART_DrvDMAReqSendEnable */
#define BRCM_SWARCH_UART_DRV_DMA_REQ_RECEIVE_ENABLE_PROC  (0x8218U)    /**< @brief #UART_DrvDMAReqReceiveEnable */
#define BRCM_SWARCH_UART_IRQ_HANDLER_PROC                 (0x8219U)    /**< @brief #UART_IRQHandler     */
#define BRCM_SWARCH_UART_WORD_LENGTH_TYPE                 (0x821AU)    /**< @brief #UART_WordLengthType  */
#define BRCM_SWARCH_UART_ENABLE_INTERRUPT_PROC            (0x821BU)    /**< @brief #UART_EnableInterrupt */
#define BRCM_SWARCH_UART_CLEAR_INTERRUPT_PROC             (0x821CU)    /**< @brief #UART_ClearInterrupt  */
#define BRCM_SWARCH_UART_GET_INTERRUPT_STATUS_PROC        (0x821DU)    /**< @brief #UART_GetInterruptStatus  */
#define BRCM_SWARCH_UART_DRV_RECEIVE_PROC                 (0x821EU)    /**< @brief #UART_DrvReceive         */


/** @} */


/**
    @name UART_HwIDType
    @{
    @brief Index of the UART controller

    @trace #BRCM_SWREQ_UART_INIT
*/
typedef uint32_t UART_HwIDType;
#define UART_HWID_0  (0UL)    /**< UART0 controller index */
#define UART_HWID_1  (1UL)    /**< UART1 controller index */
/** @} */

/**
    @name UART_FifoLvlType
    @{
    @brief HW RX/TX FIFO trigger point for the callback

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_FifoLvlType;       /**< @brief UART FIFO level   */
#define UART_FIFO_LVL_1DIV8      (0UL)   /**< @brief FIFO crosses 1/8  */
#define UART_FIFO_LVL_1DIV4      (1UL)   /**< @brief FIFO crosses 1/4  */
#define UART_FIFO_LVL_1DIV2      (2UL)   /**< @brief FIFO crosses 1/2  */
#define UART_FIFO_LVL_3DIV4      (3UL)   /**< @brief FIFO crosses 3/4  */
#define UART_FIFO_LVL_7DIV8      (4UL)   /**< @brief FIFO crosses 7/8  */
/** @} */

/**
    @name UART_BaudRateType
    @{
    @brief Baud rate of the UART port

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_BaudType;         /**< @brief typedef for UART baud rate */
#define UART_BAUD_9600              (0UL)   /**< @brief Baud rate 9600 bps */
#define UART_BAUD_19200             (1UL)   /**< @brief Baud rate 19200 bps */
#define UART_BAUD_38400             (2UL)   /**< @brief Baud rate 38400 bps */
#define UART_BAUD_115200            (3UL)   /**< @brief Baud rate 115200 bps */
#define UART_BAUD_230400            (4UL)   /**< @brief Baud rate 230400 bps */
#define UART_BAUD_500000            (5UL)   /**< @brief Baud rate 500 Kbps */
#define UART_BAUD_1000000           (6UL)   /**< @brief Baud rate 1Mbps */
#define UART_BAUD_2000000           (7UL)   /**< @brief Baud rate 2Mbps */
#define UART_BAUD_5000000           (8UL)   /**< @brief Baud rate 5Mbps */
/** @} */

/**
    @name UART_WordLengthType
    @{
    @brief Word length, it selects number of data bits transmitted or received in a frame.

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_WordLengthType;  /**< @brief typedef for UART Word length */
#define UART_WORD_LENGTH_5B         (0UL)   /**< @brief Word Length 5 bits */
#define UART_WORD_LENGTH_6B         (1UL)   /**< @brief Word Length 6 bits */
#define UART_WORD_LENGTH_7B         (2UL)   /**< @brief Word Length 7 bits */
#define UART_WORD_LENGTH_8B         (3UL)   /**< @brief Word Length 8 bits */

/** @} */

/**
    @name UART_StopBitsType
    @{
    @brief Number of stop bits for the UART transaction

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_StopBitsType;         /**< @brief typedef for UART stop bits */
#define UART_STOP_BITS1             (1UL)   /**< @brief 1 stop bit */
#define UART_STOP_BITS2             (2UL)   /**< @brief 2 stop bits */
/** @} */

/**
    @name UART_ParityType
    @{
    @brief Parity for the UART transaction

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_ParityType;           /**< @brief typedef for the UART parity */
#define UART_PARITY_NONE            (0UL)   /**< @brief No parity */
#define UART_PARITY_EVEN            (1UL)   /**< @brief Even parity */
#define UART_PARITY_ODD             (2UL)   /**< @brief Odd parity */
/** @} */

/**
    @name UART_StateType
    @{
    @brief State of the UART driver

    @trace #BRCM_SWREQ_UART_INIT
*/
typedef uint32_t UART_StateType;            /**< @brief typedef for UART states         */
#define UART_STATE_UNINIT                 (0UL)   /**< @brief UART driver is not initialized and value must be zero*/
#define UART_STATE_INIT                   (1UL)   /**< @brief UART driver is initialized      */
/** @} */

/**
    @name UART_TxStatusType
    @{
    @brief Status of the UART Tx

    @trace #BRCM_SWREQ_UART_COMMUNICATION
 */
typedef uint32_t UART_TxStatusType;         /**< @brief typedef for UART states         */
#define UART_TX_STATUS_BUFFER_IDLE  (1UL)   /**< @brief Buffer is empty. UART driver
                                                    can accept new buffer               */
#define UART_TX_STATUS_FIFO_IDLE    (2UL)   /**< @brief FIFO is empty,
                                                        no pendind data to send         */
/** @} */


/**
    @name UART_ErrorType
    @{
    @brief UART Error Type

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_ErrorType;                  /**< @brief typedef for UART errors */
#define UART_ERROR_FRAMING    (0x00000001UL)   /**< @brief Framing Error */
#define UART_ERROR_PARITY     (0x00000002UL)   /**< @brief Parity Error */
#define UART_ERROR_BREAK      (0x00000004UL)   /**< @brief Break Error */
#define UART_ERROR_OVERRUN    (0x00000008UL)   /**< @brief Overrun Error */
#define UART_ERROR_TIMEOUT    (0x00000010UL)   /**< @brief Timeout Error */
/** @} */

/**
    @name UART_InterruptType
    @{
    @brief UART Interrupt Type

    @trace #BRCM_SWREQ_UART_INTERRUPT
 */
typedef uint32_t UART_InterruptType; /**< @brief Select UART interrupt type. */
#define UART_INTERRUPT_RX_INTR         (1UL<<0UL)     /**< @brief Receive interrupt triggers when rx FIFO reaches programmed trigger level. */
#define UART_INTERRUPT_TX_INTR         (1UL<<1UL) /**< @brief Transmit interrupt triggers when tx FIFO reaches programmed trigger level. */
#define UART_INTERRUPT_RX_TIMEOUT_INTR (1UL<<2UL) /**< @brief Receive timeout interrupt asserted when the receive FIFO is not empty
                                                    and no further data is received over a 32-bit period. */
#define UART_INTERRUPT_ERROR_INTR      (1UL<<3UL) /**< @brief The error interrupt is asserted when an error occurs in the reception of data
                                            by the UART. */
#define UART_INTERRUPT_ALL              (0xFUL)     /**< @brief Select all the interrupts */
/** @} */

/**
    @name UART_ConfigType
    @{
    @brief UART configuration structure

    @trace #BRCM_SWREQ_UART_INIT
*/
typedef struct sUART_ConfigType {
    UART_FifoLvlType   txFifoLvl;       /**< @brief Transmit Fifo Level */
    UART_FifoLvlType   rxFifoLvl;       /**< @brief Receive Fifo Level */
    UART_BaudType      baud;            /**< @brief Baud rate of the UART  */
    UART_StopBitsType  stopBits;        /**< @brief Number of stop bits */
    UART_ParityType    parity;          /**< @brief Parity bits */
    UART_WordLengthType wordLength;     /**< @brief Word length transmitted or received in frame */
    uint32_t           loopBackMode;    /**< @brief UART loop back mode enable flag.
                                             If Set to 1, UART shall be configured in Loopback mode */
} UART_ConfigType;
/** @} */

/** @brief Initialize UART

    This API initializes the given UART controller with the given configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId             Index of the UART controller
    @param[in]      aConfig     Pointer to the configuration structure

    @return     void

    @post UART driver state changes to UART_INIT

    @trace  #BRCM_SWREQ_UART_INIT

    @limitations None
*/
void UART_DrvInit(UART_HwIDType aId, UART_ConfigType *aConfig);

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

    @trace  #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
int32_t UART_DrvSend(UART_HwIDType aId, const uint8_t *aData, uint32_t aSize,
        UART_ConfigType *aConfig);

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

    @limitations None
*/
int32_t UART_DrvReceive(UART_HwIDType aId, uint8_t *aData, uint32_t *aSize,
        UART_ConfigType *aConfig);


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

    @trace #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
extern int32_t UART_DrvGetTxStatus(UART_HwIDType aId,
                                   UART_TxStatusType *aStatus);

/** @brief De-initialize UART

    This API de-initializes UART. Resets the given UART controller and put back
    to its original state (same as Power on reset).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post UART driver state changes to UART_UNINIT

    @trace  #BRCM_SWREQ_UART_INIT

    @limitations None
*/
int32_t UART_DrvDeInit(UART_HwIDType aId);

/** @brief Send Break

    This API is used to send Break on the respective hardware ID. After calling
    this API, a continuous low-level is outputted on the UART until cleared by
    calling the #UART_DrvClrBreak API.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
extern int32_t UART_DrvSendBreak(UART_HwIDType aId);

/** @brief Clear Break

    This API is used to Clear Break on the respective hardware ID.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
extern int32_t UART_DrvClrBreak(UART_HwIDType aId);

/** @brief Get the state of the UART

    This API retrieves the current state of the UART controller in aState
    argument

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[out]     aState      Current state of UART

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_UART_INIT

    @limitations None
*/
int32_t UART_DrvGetState(UART_HwIDType aId,
                      UART_StateType *const aState);

/** @brief Request transmit DMA Enable

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      enable      Enable Transmit DMA request

    @return     #BCM_ERR_OK             Success
                #BCM_ERR_INVAL_STATE    UART is not initialized
                #BCM_ERR_INVAL_PARAMS   aId is invalid

    @trace  #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
int32_t UART_DrvDMAReqSendEnable(UART_HwIDType aId, uint32_t enable);

/** @brief Request receive DMA Enable

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      enable      Enable receive DMA request

    @return     #BCM_ERR_OK             Success
                #BCM_ERR_INVAL_STATE    UART is not initialized
                #BCM_ERR_INVAL_PARAMS   aId is invalid

    @trace  #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
int32_t UART_DrvDMAReqReceiveEnable(UART_HwIDType aId, uint32_t enable);

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

    @trace  #BRCM_SWREQ_UART_INTERRUPT

    @limitations None
*/
int UART_EnableInterrupt(UART_HwIDType aId, UART_InterruptType intrType, uint32_t aEnable);

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

    @trace  #BRCM_SWREQ_UART_INTERRUPT

    @limitations None
*/
int UART_ClearInterrupt(UART_HwIDType aId, UART_InterruptType intrType);

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

    @trace  #BRCM_SWREQ_UART_INTERRUPT

    @limitations None
*/
int UART_GetInterruptStatus(UART_HwIDType aId, UART_InterruptType intrType, uint32_t *intrStatus);

/** @brief  UART IRQ Handler

  This API is called when PL011 interrupt is asserted

  @param[in]    aId       Index of the UART port.
  @param[in]    aConfig   Pointer to the configuration structure

  @behavior Async, Non-reentrant

  @pre None

  @return       int32_t
  @post None

    @trace #BRCM_SWREQ_UART_COMMUNICATION

  @limitations None
 */
int32_t UART_IRQHandler(UART_HwIDType aId, UART_ConfigType *aConfig);


#endif /* UART_H */

/** @} */
