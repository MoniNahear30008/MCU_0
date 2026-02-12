/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_i2cdrv_ifc Interface
    @ingroup grp_i2cdrv

    @addtogroup grp_i2cdrv_ifc
    @{
    @section sec_i2c_overview Overview
    IIC or I2C is multi-master multi-slave serial bus. IIC bus is a two
    wire protocol, one wire for data (SDA) and another for the clock (SCL).
    Communication on the bus always starts by master and followed by slave
    response.

    IIC driver supports the following features:
    - Initialize IIC bus in master mode
    - Automatically send a start/stop condition on the bus during transaction
    - Works in interrupt mode
    - Capability to handle all IIC buses in the system
    - IIC HW instance is accessed based on HwID passed as first parameter in each API.
    IIC controller base address is accessed based on index HwID passed as 0 to (IIC_MAX_HW_ID - 1)
    depending on HW instance.

     | HwID     | HW Instance   |
     |:-----------:|:-------------:|
     | 0            |   I2C_0       |
     | 1            |   I2C_1       |

    IIC driver manages all the buses in the chipset and initiates a transaction
    with IIC slave based on the I2C packet type.

    @section sec_i2c_pkt IIC Packets
    IIC driver manages all the buses and initiates transaction with an IIC
    slave based on the IIC packet type (see #IIC_PktType). IIC transfer packet
    defines how the transaction on the IIC bus shall happen. Each IIC packet
    defines the transaction type (Read/Write) and length of the transaction.
    Driver shall take care of generating START/RESTART and STOP condition at the
    start/restart/end of each transaction. Client/application may need to
    form multiple packets in one transaction based upon the IIC slave protocol.
    @note First packet is needed, which sends slave address and register address followed by
     data to be written.

    @limitations IIC driver handles only master mode.
    @limitations Hardware transmit FIFO is currently not supported by driver.
    @section api_list List of I2C APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref IIC_Init | Initialize IIC bus |
    | @ref IIC_Transfer | IIC transfer request |
    | @ref IIC_GetStatus | Get response of a queued IIC request |
    | @ref IIC_DeInit | De-initialize IIC bus controller |
    | @ref IIC_IRQHandler | I2C IRQ Handler |
    | @ref IIC_EnableInterrupt | I2C enable interrupt |
    | @ref IIC_ClearInterrupt | I2C clear interrupt |
    | @ref IIC_GetInterruptStatus | I2C get interrupt status |
    @file i2c.h
    @brief IIC driver interface
    @section apis This section defines the interface for I2C driver
    @version 0.62 Imported from docx
*/

#ifndef IIC_H
#define IIC_H

#include <stdint.h>

/**
    @name IIC driver API IDs
    @{
    @brief API IDs for IIC driver
*/
#define BRCM_SWARCH_IIC_HWID_TYPE                 (0x8101U)    /**< @brief #IIC_HwIDType    */
#define BRCM_SWARCH_IIC_SPEED_TYPE                (0x8102U)    /**< @brief #IIC_SpeedType   */
#define BRCM_SWARCH_IIC_CONFIG_TYPE               (0x8103U)    /**< @brief #IIC_ConfigType  */
#define BRCM_SWARCH_IIC_PKT_FLG_MACRO             (0x8104U)    /**< @brief #IIC_PktFlagType */
#define BRCM_SWARCH_IIC_PKT_TYPE                  (0x8105U)    /**< @brief #IIC_PktType     */
#define BRCM_SWARCH_IIC_BSC_STATE_TYPE            (0x8106U)    /**< @brief #IIC_BscStateType */
#define BRCM_SWARCH_IIC_XFER_STATE_TYPE           (0x8107U)    /**< @brief #IIC_XferStateType */
#define BRCM_SWARCH_IIC_INTERRUPT_TYPE            (0x8108U)    /**< @brief #IIC_InterruptType */
#define BRCM_SWARCH_IIC_INIT_PROC                 (0x8109U)    /**< @brief #IIC_Init        */
#define BRCM_SWARCH_IIC_TRANSFER_PROC             (0x810AU)    /**< @brief #IIC_Transfer    */
#define BRCM_SWARCH_IIC_GETSTATUS_PROC            (0x810BU)    /**< @brief #IIC_GetStatus   */
#define BRCM_SWARCH_IIC_DEINIT_PROC               (0x810CU)    /**< @brief #IIC_DeInit      */
#define BRCM_SWARCH_IIC_IRQHANDLER_PROC          (0x810DU)    /**< @brief #IIC_IRQHandler     */
#define BRCM_SWARCH_IIC_ENABLE_INTERRUPT_PROC     (0x810EU)    /**< @brief #IIC_EnableInterrupt */
#define BRCM_SWARCH_IIC_CLEAR_INTERRUPT_PROC      (0x810FU)    /**< @brief #IIC_ClearInterrupt  */
#define BRCM_SWARCH_IIC_GET_INTERRUPT_STATUS_PROC (0x8110U)    /**< @brief #IIC_GetInterruptStatus  */
#define BRCM_SWARCH_IIC_CLIENT_SIGNAL_TYPE        (0x8111U)    /**< @brief #IIC_ClientSignalType */
/** @} */


/**
    @name IIC_HwIDType
    @{
    @brief Index of the IIC controller
    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef uint32_t IIC_HwIDType;
#define IIC_HWID_0  (0UL)    /**< I2C0 controller index */
#define IIC_HWID_1  (1UL)    /**< I2C1 controller index */

/** @} */


/**
    @name IIC bus speed
    @{
    @brief IIC bus speed.

    Actual speed depends on the external pull up resistor and slave clock
    stretching. Actual bus speed will never exceed selected speed.

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef uint32_t IIC_SpeedType;
#define IIC_SPEED_100K          (1UL)   /**< @brief 100 KHz */
#define IIC_SPEED_400K          (2UL)   /**< @brief 400 KHz */
#define IIC_SPEED_MAX           (3UL)   /**< @brief Max speed supported by hardware */
/** @} */

/**
    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef struct sIIC_ClientSignalType {
    uint32_t    done;
    int32_t     clientID;
    uint32_t    clientMask;
} IIC_ClientSignalType;


/**
    @brief IIC Configuration

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef struct sIIC_ConfigType {
    IIC_SpeedType speed;             /**< @brief Speed of the IIC bus */
    uint32_t      txFifoMode;        /**< @brief Disable/enable hardware transmit FIFO
                                     (In the current version FIFO mode is not
                                     supported):
                                     - 0UL: Disable hardware transmit FIFO
                                     - 1UL: Enable hardware transmit FIFO */
} IIC_ConfigType;

/**
    @name IIC packet flags
    @{
    @brief IIC packet flags

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef uint32_t IIC_PktFlagType;
#define IIC_PKT_FLG_SLAVE_10BIT     (0x1UL) /**< @brief 10bit slave device */
#define IIC_PKT_FLG_OP_RD           (0x2UL) /**< @brief IIC read. If this flag is
                                             not used, it is assumed as an IIC
                                             write. */
#define IIC_PKT_FLG_RESTART         (0x4UL) /**< @brief Slave needs restart */
/** @} */

/**
    @brief IIC transfer packet

    IIC transfer packet structure.

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef struct sIIC_PktType {
    IIC_PktFlagType flags;     /**< @brief Packet flags (combination of
                                     one/more of the following):
                                            - #IIC_PKT_FLG_SLAVE_10BIT
                                            - #IIC_PKT_FLG_OP_RD
                                            - #IIC_PKT_FLG_RESTART */
    uint32_t slaveAddr;         /**< @brief Device address of the IIC slave.
                                     This slaveAddr is 7bit/10bit address
                                     (without any shift). */
    uint32_t length;            /**< @brief Length of the transfer in bytes */
    uint8_t *data;              /**< @brief Pointer to memory address where data
                                     is to be read from (for IIC write) or to
                                     be written to (for IIC read). */
} IIC_PktType;

/**
    @name IIC_InterruptType
    @{
    @brief IIC Interrupt Type

    @trace #BRCM_SWREQ_IIC_INIT
 */
typedef uint32_t IIC_InterruptType; /**< @brief Select IIC interrupt type. */
#define IIC_INTERRUPT_FIFO_EMPTY_INTR  (1UL<<0UL) /**< @brief TX FIFO empty interrupt. */
#define IIC_INTERRUPT_NOACK_INTR       (1UL<<1UL) /**< @brief No acknowledge interrupt. */
#define IIC_INTERRUPT_BUSY_INTR        (1UL<<2UL) /**< @brief IIC busy status interrupt. */
#define IIC_INTERRUPT_ERROR_INTR       (1UL<<3UL) /**< @brief IIC error status interrupt. */
#define IIC_INTERRUPT_CMD_RUN_INTR     (1UL<<4UL) /**< @brief IIC command run interrupt. */
#define IIC_INTERRUPT_ALL_INTR         (0x1FUL)   /**< @brief Select all IIC interrupts. */

/** @} */

/** @name IIC Bsc state IDs
    @{
    @brief IIC Bsc state type
    @trace #BRCM_SWREQ_IIC_INIT
*/
typedef uint32_t IIC_BscStateType;
#define IIC_BSC_STATE_UNINIT        (0UL) /**< @brief value must be zero*/
#define IIC_BSC_STATE_IDLE          (1UL)
/** @} */

/** @name IIC transfer state IDs
    @{
    @brief IIC transfer state type
    @trace #BRCM_SWREQ_IIC_INIT
*/
typedef uint32_t IIC_XferStateType;
#define IIC_XFER_STATE_START_CMD        (0UL) /**< @brief value must be zero*/
#define IIC_XFER_STATE_RESTART_CMD      (1UL)
#define IIC_XFER_STATE_SLVADDR_BYTE0    (2UL)
#define IIC_XFER_STATE_SLVADDR_BYTE1    (3UL)
#define IIC_XFER_STATE_START_DATA       (4UL)
#define IIC_XFER_STATE_WRITE_BYTE       (5UL)
#define IIC_XFER_STATE_READ_CMD         (6UL)
#define IIC_XFER_STATE_READ_BYTE        (7UL)
#define IIC_XFER_STATE_STOP_CMD         (8UL)
#define IIC_XFER_STATE_END              (9UL)
/** @} */

/** @brief Initialize IIC bus

    This API queues a request to initialize the given IIC bus controller with
    the given configuration.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID           Index of the IIC bus
    @param[in]      aConfig       Pointer to IIC configuration structure

    @return     int32_t

    @post TBD

    @trace  #BRCM_SWREQ_IIC_INIT

    @limitations This API shall be called only from task context.
*/
extern int32_t IIC_Init(IIC_HwIDType aID,
                    const IIC_ConfigType * const aConfig);

/** @brief IIC transfer request

    This API queues a transfer request to IIC driver. When this request
    processing is complete, client shall get status using #IIC_GetStatus API.

    @behavior Async, Re-entrant

    @pre TBD

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

    @post TBD

    @trace  #BRCM_SWREQ_IIC_TRANSFER

    @limitations This API shall be called only from task context.
*/
extern int32_t IIC_Transfer(IIC_HwIDType aID,
                           IIC_PktType * const aPkts,
                           uint32_t aNumPkts,
                           int32_t aClientID,
                           uint32_t aClientMask,
                           uint32_t *aJobID);

/** @brief Get response of a queued IIC request

    This API shall be called by the client after it is signaled about the
    completion of the IIC request.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the IIC bus controller ID
    @param[in]      aClientID    Client ID of caller
    @param[in]      aJobID     Handle as provided by IIC_Transfer

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             IIC driver response is successfully
                                        retrieved
    @retval     #BCM_ERR_BUSY           I2C state is busy. Retry.
    @retval     #BCM_ERR_AUTH_FAILED    Invalid aJobID.
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aID
    @retval     #BCM_ERR_MAX_ATTEMPS    When retry reach max limit
    @retval     Others                  As returned by #IIC_SysCmdReq

    @post TBD

    @trace  #BRCM_SWREQ_IIC_TRANSFER

    @limitations any limitations for this api
*/
extern int32_t IIC_GetStatus(IIC_HwIDType aID,
                                int32_t aClientID,
                                const uint32_t aJobID);

/** @brief De-initialize IIC bus controller

    This API queues a request to de-initialize the given IIC bus controller. On
    successful de-initialization, bus controller shall be in uninitialized
    state.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the IIC bus controller

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             De-Init request is successfully queued
                                        to IIC driver queue
    @retval     #BCM_ERR_INVAL_STATE    I2C is in invalid state.
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     Others                  As returned by #IIC_SysCmdReq

    @post TBD

    @trace  #BRCM_SWREQ_IIC_INIT

    @limitations This API shall be called only from task context.
*/
extern int32_t IIC_DeInit(IIC_HwIDType aID);

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

    @limitations None
*/
int IIC_EnableInterrupt(IIC_HwIDType aId, IIC_InterruptType intrType, uint32_t aEnable);

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

    @limitations None
*/
int IIC_ClearInterrupt(IIC_HwIDType aId, IIC_InterruptType intrType);

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

    @limitations None
*/
int IIC_GetInterruptStatus(IIC_HwIDType aId, IIC_InterruptType intrType, uint32_t *intrStatus);

/** @brief  I2C IRQ Handler

  This API is called when IIC interrupt is asserted

  @param[in]    aId  Index of the IIC port.

  @behavior Async, Non-reentrant

  @pre None

  @return      clientSignal structure
  @post None

    @trace #BRCM_SWREQ_IIC_KERNEL_HANDLER

  @limitations None
 */
IIC_ClientSignalType IIC_IRQHandler(IIC_HwIDType aID);

#endif /* IIC_H */
/** @} */
