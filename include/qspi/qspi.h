/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_qspidrv_ifc QSPI
    @ingroup grp_qspidrv

    @addtogroup grp_qspidrv_ifc
    @{
    @section sec_qspi_overview Overview

    QSPI is based on SPI. It is similar to SPI protocol except that it has additional data lines.
    There is an SPI master that interfaces with any SPI enabled slave device in Single-Lane, Dual-Lane
    or QUAD-Lane modes.
    The QSPI can be used in normal SPI mode or in Serial Memory mode to connect to external Flash memories.
    The normal SPI has four communication lines: Chip Select, Clock, MOSI, and MISO.
    For QSPI, additional data lines are available.
    The command/data/address are sent through single, dual or quad IO, based on the mode selected.
    As data is sent over multiple lines, it helps in increasing bandwidth compared to a standard SPI transfer.
    With the support of the Quad SPI protocol, the QSPI allows the system to use high-performance serial
    flash memories, which are small and inexpensive, in place of larger and more expensive parallel flash memories.
    The QSPI API set provides non-Blocking APIs which can be used to perform the following functionalities on the QSPI
    Slave device:
        -# Support frame format of SPI/D-SPI/Q-SPI in Master Mode.
        -# Send a command to QSPI slave device using QSPI_CommandXferType data type without exchange of any other data.
        -# Register/memory read/write using QSPI_DrvWrite/QSPI_DrvRead APIs.
        -# Each frame consist of one or more of the following phases: OPCODE, ADDRESS, MODEBITS, DUMMY DATA.
        -# The programming phase of opcode, address, modebits etc. needs to be completed before starting QSPI
           transaction.
        -#  QSPI HW instance is accessed based on HwID passed as the first parameter in each API.
       QSPI controller base address is accessed based on index HwID passed as 0 to QSPI_MAX_HW_ID - 1,
       depending on HW instance.
       @section sec_qspi_crc CRC programming
          -# User configures CRC control by calling #QSPI_CrcControlConfigSet
          -# User needs to enable CRC before starting write/read transaction.
          -# Program TX and RX length including 4 bytes of CRC (DATA_Len + 4) in transmit and receive APIs.
          -# User needs to wait if TX fifo is empty and TX CRC done status is true. User needs to read CRC value by calling #QSPI_GetCrcTxData
          and user writes this value using #QSPI_WriteTxCrcData in data register at the end.
          -# For receiver side, once all the data is received and RX CRC done status is true, user needs to compare received last 4 bytes of
          data with the automatic updated CRC value stored in register using #QSPI_GetCrcRxData. If both of the values are the same, the message
          is good.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   QSPI_0      |
       | 1           |   QSPI_1      |
       | 2           |   QSPI_2      |
       | 3           |   QSPI_3      |

    @section api_list List of QSPI APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref QSPI_DrvInit | Initialize QSPI driver |
    | @ref QSPI_DrvDeInit | De-initialize QSPI peripheral |
    | @ref QSPI_ModeConfigure | QSPI mode configure |
    | @ref QSPI_SetConfigProperty | QSPI set config property |
    | @ref QSPI_GetConfigProperty | QSPI get config property  |
    | @ref QSPI_DrvXfer | (Deprecated) QSPI transfer function to write data and read the response from QSPI device |
    | @ref QSPI_DrvWrite | Writes data on QSPI peripheral |
    | @ref QSPI_DrvRead | Reads data on QSPI peripheral |
    | @ref QSPI_DrvDMAReqWriteEnable | DMA write request enable |
    | @ref QSPI_DrvDMAReqReadEnable | DMA read request enable |
    | @ref QSPI_TxFifoFlush | Flush the TX FIFO |
    | @ref QSPI_RxFifoFlush | Flush the RX FIFO |
    | @ref QSPI_DrvStatusGet | Get the device status of QSPI peripheral |
    | @ref QSPI_IRQHandler | QSPI IRQ Handler |
    | @ref QSPI_EnableInterrupt | Enable QSPI interrupt |
    | @ref QSPI_ClearInterrupt | Clear QSPI interrupt |
    | @ref QSPI_GetInterruptStatus | Get QSPI interrupt status |
    | @ref QSPI_CrcControlConfigSet | Set CRC control configuration |
    | @ref QSPI_CrcControlConfigGet | Get CRC control configuration |
    | @ref QSPI_CrcEnable | Enable CRC |
    | @ref QSPI_CrcTxDoneStatus | CRC generation status update TX |
    | @ref QSPI_CrcRxDoneStatus | CRC generation status update RX |
    | @ref QSPI_GetCrcTxData | Get CRC Tx data |
    | @ref QSPI_GetCrcRxData | Get CRC Rx data |
    | @ref QSPI_WriteTxCrcData | Write Tx CRC data in QSPI |
    | @ref QSPI_CrcResetSet      | QSPI CRC reset  |
    @file qspi.h
    @brief qspi driver application interface
    @section apis This section defines the interface for QSPI driver
    @version 0.1 Imported from docx
*/

#ifndef QSPI_H
#define QSPI_H

/**
    @name QSPI Driver API IDs
    @{
    @brief API IDs for QSPI Driver
*/
#define BRCM_SWARCH_QSPI_HWID_TYPE                      (0x8700U) /**< @brief #QSPI_HwIDType */
#define BRCM_SWARCH_QSPI_CPOL_CPHA_CTRL_TYPE            (0x8701U) /**< @brief #QSPI_CpolCphaCtrlType */
#define BRCM_SWARCH_QSPI_DATA_MODE_TYPE                 (0x8702U) /**< @brief #QSPI_DataModeType */
#define BRCM_SWARCH_QSPI_TRANSMIT_CMD_TYPE              (0x8703U) /**< @brief #QSPI_TransmitCmdType */
#define BRCM_SWARCH_QSPI_MODE_BITS_TYPE                 (0x8704U) /**< @brief #QSPI_ModeBitsType */
#define BRCM_SWARCH_QSPI_MODE_BITS_LEN_TYPE             (0x8705U) /**< @brief #QSPI_ModeBitsLenType */
#define BRCM_SWARCH_QSPI_ADDR_MODE_TYPE                 (0x8706U) /**< @brief #QSPI_AddrModeType */
#define BRCM_SWARCH_QSPI_ADDR_MODE_LEN_TYPE             (0x8707U) /**< @brief #QSPI_AddrModeLenType */
#define BRCM_SWARCH_QSPI_OPCODE_MODE_TYPE               (0x8708U) /**< @brief #QSPI_OpcodeModeType */
#define BRCM_SWARCH_QSPI_CONFIG_TYPE                    (0x8709U) /**< @brief #QSPI_ConfigType */
#define BRCM_SWARCH_QSPI_CONFIG_PROP_TYPE               (0x870AU) /**< @brief #QSPI_ConfigPropType */
#define BRCM_SWARCH_QSPI_CLK_DIV_RATIO_TYPE             (0x870BU) /**< @brief #QSPI_ClkDivRatioType */
#define BRCM_SWARCH_QSPI_SLAVE_SEL_TYPE                 (0x870CU) /**< @brief #QSPI_SlaveSelType */
#define BRCM_SWARCH_QSPI_DEV_STATUS_TYPE                (0x870DU) /**< @brief #QSPI_DevStatusType */
#define BRCM_SWARCH_QSPI_COMMAND_XFER_TYPE              (0x870EU) /**< @brief #QSPI_CommandXferType */
#define BRCM_SWARCH_QSPI_INTERRUPT_TYPE                 (0x870FU) /**< @brief #QSPI_InterruptType */

#define BRCM_SWARCH_QSPI_DRV_INIT_PROC                  (0x8710U) /**< @brief #QSPI_DrvInit */
#define BRCM_SWARCH_QSPI_DRV_DEINIT_PROC                (0x8711U) /**< @brief #QSPI_DrvDeInit */
#define BRCM_SWARCH_QSPI_MODE_CONFIGURE_PROC            (0x8712U) /**< @brief #QSPI_ModeConfigure */
#define BRCM_SWARCH_QSPI_SET_CONFIG_PROPERTY_PROC       (0x8713U) /**< @brief #QSPI_SetConfigProperty */
#define BRCM_SWARCH_QSPI_GET_CONFIG_PROPERTY_PROC       (0x8714U) /**< @brief #QSPI_GetConfigProperty */
#define BRCM_SWARCH_QSPI_DRV_XFER_PROC                  (0x8715U) /**< @brief #QSPI_DrvXfer */
#define BRCM_SWARCH_QSPI_DRV_WRITE_PROC                 (0x8716U) /**< @brief #QSPI_DrvWrite */
#define BRCM_SWARCH_QSPI_DRV_READ_PROC                  (0x8717U) /**< @brief #QSPI_DrvRead */
#define BRCM_SWARCH_QSPI_DRV_DMA_REQ_WRITE_ENABLE_PROC  (0x8718U) /**< @brief #QSPI_DrvDMAReqWriteEnable */
#define BRCM_SWARCH_QSPI_DRV_DMA_REQ_READ_ENABLE_PROC   (0x8719U) /**< @brief #QSPI_DrvDMAReqReadEnable */
#define BRCM_SWARCH_QSPI_TX_FIFO_FLUSH_PROC             (0x871AU) /**< @brief #QSPI_TxFifoFlush */
#define BRCM_SWARCH_QSPI_RX_FIFO_FLUSH_PROC             (0x871BU) /**< @brief #QSPI_RxFifoFlush */
#define BRCM_SWARCH_QSPI_DRV_STATUS_GET_PROC            (0x871CU) /**< @brief #QSPI_DrvStatusGet */
#define BRCM_SWARCH_QSPI_IRQ_HANDLER_PROC               (0x871DU) /**< @brief #QSPI_IRQHandler     */
#define BRCM_SWARCH_QSPI_ENABLE_INTERRUPT_PROC          (0x871EU) /**< @brief #QSPI_EnableInterrupt */
#define BRCM_SWARCH_QSPI_CLEAR_INTERRUPT_PROC           (0x871FU) /**< @brief #QSPI_ClearInterrupt  */
#define BRCM_SWARCH_QSPI_GET_INTERRUPT_STATUS_PROC      (0x8720U) /**< @brief #QSPI_GetInterruptStatus  */
#define BRCM_SWARCH_QSPI_MODE_OP_CONFIG_TYPE            (0x8721U) /**< @brief #QSPI_ModeOpConfigType */
#define BRCM_SWARCH_QSPI_STATE_TYPE                     (0x8722U) /**< @brief #QSPI_StateType */
#define BRCM_SWARCH_QSPI_CRC_REV_INPUT_DATA_TYPE        (0x8723U) /**< @brief #QSPI_CrcRevInputDataType */
#define BRCM_SWARCH_QSPI_CRC_REV_OUTPUT_DATA_TYPE       (0x8724U) /**< @brief #QSPI_CrcRevOutputDataType */
#define BRCM_SWARCH_QSPI_CRC_CONTROL_CFG_TYPE           (0x8725U) /**< @brief #QSPI_CrcControlCfgType */
#define BRCM_SWARCH_QSPI_CRC_CONTROL_CONFIG_SET_PROC    (0x8726U) /**< @brief #QSPI_CrcControlConfigSet  */
#define BRCM_SWARCH_QSPI_CRC_CONTROL_CONFIG_GET_PROC    (0x8727U) /**< @brief #QSPI_CrcControlConfigGet  */
#define BRCM_SWARCH_QSPI_CRC_ENABLE_PROC                (0x8728U) /**< @brief #QSPI_CrcEnable  */
#define BRCM_SWARCH_QSPI_CRC_TX_DONE_STATUS_PROC        (0x8729U) /**< @brief #QSPI_CrcTxDoneStatus  */
#define BRCM_SWARCH_QSPI_CRC_RX_DONE_STATUS_PROC        (0x872AU) /**< @brief #QSPI_CrcRxDoneStatus  */
#define BRCM_SWARCH_QSPI_GET_CRC_TX_DATA_PROC           (0x872BU) /**< @brief #QSPI_GetCrcTxData  */
#define BRCM_SWARCH_QSPI_GET_CRC_RX_DATA_PROC           (0x872CU) /**< @brief #QSPI_GetCrcRxData  */
#define BRCM_SWARCH_QSPI_WRITE_TX_CRC_DATA_PROC         (0x872DU) /**< @brief #QSPI_WriteTxCrcData  */
#define BRCM_SWARCH_QSPI_TRANSFER_TYPE                  (0x872EU) /**< @brief #QSPI_TransferType */
#define BRCM_SWARCH_QSPI_CRC_RESET_SET_PROC             (0x872FU) /**< @brief #QSPI_CrcResetSet */

/** @} */


/**
    @name QSPI_HwIDType
    @{
    @brief QSPI hardware/controller identifier

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_HwIDType;          /**< @brief QSPI controller ID type  */
#define QSPI_HWID_0             (0UL)    /**< @brief QSPI contoller 0 ID      */
#define QSPI_HWID_1             (1UL)    /**< @brief QSPI contoller 1 ID      */
#define QSPI_HWID_2             (2UL)    /**< @brief QSPI contoller 2 ID      */
#define QSPI_HWID_3             (3UL)    /**< @brief QSPI contoller 3 ID (Flash SPI)      */
/** @} */

/**
    @name QSPI_StateType
    @{
    @brief State of the QSPI driver

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
 */
typedef uint32_t QSPI_StateType;            /**< @brief typedef for QSPI states         */
#define QSPI_STATE_UNINIT                 (0UL)   /**< @brief QSPI driver is not initialized and value must be zero*/
#define QSPI_STATE_INIT                   (1UL)   /**< @brief QSPI driver is initialized      */
/** @} */

/**
    @name QSPI_TransferType
    @{
    @brief Select QSPI transfer type

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
 */
typedef uint8_t QSPI_TransferType;            /**< @brief typedef for QSPI transfer type         */
#define QSPI_TRANSFER_PIO                 (1U << 0U)   /**< @brief QSPI Transfer mode PIO */
#define QSPI_TRANSFER_DMA                 (1U << 1U)   /**< @brief QSPI Transfer mode DMA  */
#define QSPI_TRANSFER_OPCODE              (1U << 2U)   /**< @brief QSPI Transfer opcode value  */
#define QSPI_TRANSFER_DATA_ONLY           (1U << 3U)   /**< @brief QSPI Transfer data only in PIO mode (SPI is already started it will write/Read only data for continuous data transfer)  */

/** @} */


/**
    @name QSPI_CpolCphaCtrlType
    @{
    @brief QSPI clock polarity and clock phase control

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_CpolCphaCtrlType;        /**< @brief QSPI clock polarity and clock phase conrol options */
#define QSPI_CPOL_CPHA_CTRL_00    (0UL)   /**< @brief Mode 0 CPOL=0, CPHA=0 SCK defaults to 0 data is sampled on rising edge */
#define QSPI_CPOL_CPHA_CTRL_01    (1UL)   /**< @brief Mode 1 CPOL=0, CPHA=1 SCK defaults to 0 data is sampled on falling edge
                                                Note: Holdoff_len shuld be 1 or greater */
#define QSPI_CPOL_CPHA_CTRL_10    (2UL)   /**< @brief Mode 2 CPOL=1, CPHA=0 SCK defaults to 1 data is sampled on falling edge */
#define QSPI_CPOL_CPHA_CTRL_11    (3UL)   /**< @brief Mode 3 CPOL=1, CPHA=1 SCK defaults to 1 data is sampled on rising edge
                                                Note: Holdoff_len shuld be 1 or greater */
/** @} */

/**
    @name QSPI_DataModeType
    @{
    @brief QSPI Data mode options

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_DataModeType;        /**< @brief QSPI Data Mode options */
#define QSPI_DATA_MODE_NO_BYTE_SENT  (0UL)   /**< @brief QSPI No byte sent in data phase */
#define QSPI_DATA_MODE_SINGLE        (1UL)   /**< @brief QSPI Single SPI mode */
#define QSPI_DATA_MODE_DUAL          (2UL)   /**< @brief QSPI Dual SPI mode */
#define QSPI_DATA_MODE_QUAD          (3UL)   /**< @brief QSPI Quad SPI mode */
/** @} */

/**
    @name QSPI_ModeBitsType
    @{
    @brief QSPI Data mode options

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_ModeBitsType;  /**< @brief QSPI Data Mode Bits options */
#define QSPI_MODE_BITS_NO_BYTE_SENT  (0UL)   /**< @brief QSPI No byte sent in mode bits phase */
#define QSPI_MODE_BITS_SINGLE        (1UL)   /**< @brief QSPI Single SPI mode */
#define QSPI_MODE_BITS_DUAL          (2UL)   /**< @brief QSPI Dual SPI mode */
#define QSPI_MODE_BITS_QUAD          (3UL)   /**< @brief QSPI Quad SPI mode */
/** @} */

/**
    @name QSPI_ModeBitsLenType
    @{
    @brief QSPI Mode Bits length options

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_ModeBitsLenType;    /**< @brief QSPI Mode Bits Length options */
#define QSPI_MODE_BITS_LEN_BYTE_1  (0UL)   /**< @brief QSPI mode Bits Length 1 byte */
#define QSPI_MODE_BITS_LEN_BYTE_2  (1UL)   /**< @brief QSPI mode Bits Length 2 byte */
#define QSPI_MODE_BITS_LEN_BYTE_3  (2UL)   /**< @brief QSPI mode Bits Length 3 byte */
#define QSPI_MODE_BITS_LEN_BYTE_4  (3UL)   /**< @brief QSPI mode Bits Length 4 byte */
/** @} */

/**
    @name QSPI_AddrModeType
    @{
    @brief QSPI Address mode options

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_AddrModeType;       /**< @brief QSPI Address Mode options */
#define QSPI_ADDR_MODE_NO_BYTE_SENT     (0UL)   /**< @brief QSPI No byte sent in address phase */
#define QSPI_ADDR_MODE_SINGLE           (1UL)   /**< @brief QSPI Single SPI mode */
#define QSPI_ADDR_MODE_DUAL             (2UL)   /**< @brief QSPI Dual SPI mode */
#define QSPI_ADDR_MODE_QUAD             (3UL)   /**< @brief QSPI Quad SPI mode */
/** @} */

/**
    @name QSPI_AddrModeLenType
    @{
    @brief QSPI Address mode length options

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_AddrModeLenType;     /**< @brief QSPI Address Mode Length options */
#define QSPI_ADDR_MODE_LEN_BYTE_1  (0UL)   /**< @brief QSPI Addr mode Length 1 byte */
#define QSPI_ADDR_MODE_LEN_BYTE_2  (1UL)   /**< @brief QSPI Addr mode Length 2 byte */
#define QSPI_ADDR_MODE_LEN_BYTE_3  (2UL)   /**< @brief QSPI Addr mode Length 3 byte */
#define QSPI_ADDR_MODE_LEN_BYTE_4  (3UL)   /**< @brief QSPI Addr mode Length 4 byte */
/** @} */

/**
    @name QSPI_OpcodeModeType
    @{
    @brief QSPI Opcode mode options

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_OpcodeModeType;      /**< @brief QSPI Opcode Mode options */
#define QSPI_OPCODE_MODE_NO_BYTE_SENT   (0UL)  /**< @brief QSPI No byte sent in opcode phase */
#define QSPI_OPCODE_MODE_SINGLE         (1UL)  /**< @brief QSPI Single SPI mode */
#define QSPI_OPCODE_MODE_DUAL           (2UL)  /**< @brief QSPI Dual SPI mode */
#define QSPI_OPCODE_MODE_QUAD           (3UL)  /**< @brief QSPI Quad SPI mode */
/** @} */

/**
    @name QSPI_TransmitCmdType
    @{
    @brief Select between TX or RX SPI transaction

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint8_t QSPI_TransmitCmdType;
#define QSPI_TRANSMIT_CMD_READ  (0U) /**< RX SPI transaction. */
#define QSPI_TRANSMIT_CMD_WRITE (1U) /**< TX SPI transaction. */
/** @} */

/**
    @name QSPI_ConfigPropType
    @{
    @brief QSPI config property to be selected to set at desired value.

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_ConfigPropType;
#define QSPI_CONFIG_PROP_RX_FSM_NO_TRANSIT      (0x1UL) /**< @brief FSM transition enable on Rx Fifo Full Condition.
                                                                                                 0    : FSM transitions from Idle State for Rx SPI transaction even when Rx Fifo is full
                                                                                                         Rx Fifo will be overwritten (No change from previous design).
                                                                                                 1    : FSM does not transitions from Idle State for Rx SPI transaction even when Rx Fifo is full. */
#define QSPI_CONFIG_PROP_TRANS_REPEAT           (0x2UL) /**< Sets how many times the SPI transaction is to repeat, valid range value 1-FFh (254 times). */
#define QSPI_CONFIG_PROP_CSB_HIGH_LEN           (0x3UL) /**< Sets how many SCK periods CSB remains high after each transaction, valid range value 0-FFh (256 times). */
#define QSPI_CONFIG_PROP_CSB_AUTO_POLL_EN       (0x4UL) /**< Automatic Polling Mode Enable. */
#define QSPI_CONFIG_PROP_CSB_AUTO_POLL_MODE_SEL (0x5UL) /**< Automatic Polling Mode select 0 : AND all unmasked expected bits to generate automatic polling interrupt
                                                                                                       1 : OR all unmasked expected bits to generate automatic polling interrupt. */
#define QSPI_CONFIG_PROP_CSB_AUTO_POLL_SELF_CLR (0x6UL) /**< Automatic Polling self clearing mode. */
#define QSPI_CONFIG_PROP_HOLDN_CTRL             (0x7UL) /**< Controls the state of pin HOLDn in Single and Dual modes. */
#define QSPI_CONFIG_PROP_WPN_CTRL               (0x8UL) /**< Controls the state of pin WPn in Single and Dual modes. */
#define QSPI_CONFIG_PROP_CSB_AUTO_POLL_EXP_VAL  (0x9UL) /**< Expected value used for comparision of up to 4 bytes when in automatic polling mode. */
#define QSPI_CONFIG_PROP_CSB_AUTO_POLL_MASK_VAL (0xAUL) /**< Mask applied to up to 4 bytes when in Automatic polling mode. */
#define QSPI_CONFIG_PROP_TXFIFO_WAIT_EN         (0xBUL) /**< TX FIFO wait enable. */
#define QSPI_CONFIG_PROP_RXFIFO_WAIT_EN         (0xCUL) /**< RX FIFO wait enable. */
#define QSPI_CONFIG_PROP_RXDLY_EN               (0xDUL) /**< RX Delay enable. When enabled, RX data is sampled 10 ns after expected edge to help with timing. */
/** @} */

/**
    @name QSPI_InterruptType
    @{
    @brief QSPI Interrupt Type

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
 */
typedef uint32_t QSPI_InterruptType; /**< @brief Select QSPI interrupt type. */
#define QSPI_INTERRUPT_RX_EMPTY_INTR              (1UL<<0UL) /**< @brief RX FIFO Empty interrupt. */
#define QSPI_INTERRUPT_RX_THR_INTR                (1UL<<1UL) /**< @brief RX FIFO threshold interrupt. */
#define QSPI_INTERRUPT_RX_FULL_INTR               (1UL<<2UL) /**< @brief RX full interrupt. */
#define QSPI_INTERRUPT_RX_OVERFLOW_INTR           (1UL<<3UL) /**< @brief RX overflow interrupt */
#define QSPI_INTERRUPT_SAFETY_RX_UNDFLW_INTR      (1UL<<4UL) /**< @brief Safety Island RX underflow interrupt. */
#define QSPI_INTERRUPT_SAFETY_RX_OVRFLW_INTR      (1UL<<5UL) /**< @brief Safety Island RX overflow interrupt. */
#define QSPI_INTERRUPT_SAFETY_TX_UNDFLW_INTR      (1UL<<6UL) /**< @brief Safety Island TX underflow interrupt. */
#define QSPI_INTERRUPT_SAFETY_TX_OVRFLW_INTR      (1UL<<7UL) /**< @brief Safety Island TX overflow interrupt */
#define QSPI_INTERRUPT_TX_EMPTY_INTR              (1UL<<8UL) /**< @brief TX FIFO Empty interrupt. */
#define QSPI_INTERRUPT_TX_THR_INTR                (1UL<<9UL) /**< @brief TX FIFO threshold interrupt. */
#define QSPI_INTERRUPT_TX_FULL_INTR               (1UL<<10UL) /**< @brief TX full interrupt. */
#define QSPI_INTERRUPT_TX_UNDERFLOW_INTR          (1UL<<11UL) /**< @brief TX underflow interrupt */
#define QSPI_INTERRUPT_AUTO_POLL_INTR             (1UL<<12UL) /**< @brief Automatic Polling interrupt. */
#define QSPI_INTERRUPT_DONE_INTR                  (1UL<<13UL) /**< @brief Transaction done interrupt. */
#define QSPI_INTERRUPT_ALL                        (0x3FFFUL)  /**< @brief Select all interrupt */
/** @} */

/**
    @name QSPI_ClkDivRatioType
    @{
    @brief SPI clock divison ratio -  SCK period = 2*(N+1) system clock period.

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
 */
typedef uint32_t QSPI_ClkDivRatioType;
#define QSPI_CLK_DIV_RATIO_BY_4   (0x1UL) /**< @brief QSPI SCK clock divide by 4 */
#define QSPI_CLK_DIV_RATIO_BY_6   (0x2UL) /**< @brief QSPI SCK clock divide by 6 */
#define QSPI_CLK_DIV_RATIO_BY_8   (0x3UL) /**< @brief QSPI SCK clock divide by 8 */
#define QSPI_CLK_DIV_RATIO_BY_32  (0xFUL) /**< @brief QSPI SCK clock divide by 32 */
#define QSPI_CLK_DIV_RATIO_BY_64  (0x1FUL) /**< @brief QSPI SCK clock divide by 64 */
#define QSPI_CLK_DIV_RATIO_BY_96  (0x2FUL) /**< @brief QSPI SCK clock divide by 96 */
#define QSPI_CLK_DIV_RATIO_BY_128 (0x3FUL) /**< @brief QSPI SCK clock divide by 128 */
/** @} */

/**
    @name QSPI_SlaveSelType
    @{
    @brief Select SPI Slave using chip select control.

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
 */
typedef uint32_t QSPI_SlaveSelType;
#define QSPI_SLAVE_SEL_IND_0      (0x0UL) /**< @brief Slave 0 Individual select */
#define QSPI_SLAVE_SEL_IND_1      (0x1UL) /**< @brief Slave 1 Individual select */
#define QSPI_SLAVE_SEL_IND_2      (0x2UL) /**< @brief Slave 2 Individual select */
#define QSPI_SLAVE_SEL_IND_3      (0x3UL) /**< @brief Slave 3 Individual select */
#define QSPI_SLAVE_SEL_ENC_2      (0x4UL) /**< @brief Slave 0 Encoded select */
#define QSPI_SLAVE_SEL_ENC_0      (0x5UL) /**< @brief Slave 1 Encoded select */
#define QSPI_SLAVE_SEL_ENC_1      (0x6UL) /**< @brief Slave 2 Encoded select */
#define QSPI_SLAVE_SEL_NA         (0x7UL) /**< @brief Slave select not applicable none. */
/** @} */

/**
    @name QSPI_CrcRevInputDataType
    @{
    @brief QSPI CRC reverse input data selection.

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_CrcRevInputDataType;
#define QSPI_CRC_REV_INPUT_DATA_NONE (0UL) /**< @brief Bit order not affected. */
#define QSPI_CRC_REV_INPUT_DATA_BYTE (1UL) /**< @brief Bit reversal done by byte. */
#define QSPI_CRC_REV_INPUT_DATA_HALF_WORD (2UL) /**< @brief Bit reversal done by half-word. */
#define QSPI_CRC_REV_INPUT_DATA_WORD (3UL) /**< @brief Bit reversal done by half-word. */
/** @} */

/**
    @name QSPI_CrcRevOutputDataType
    @{
    @brief These are the controls on the output CRC data.

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef uint32_t QSPI_CrcRevOutputDataType;
#define QSPI_CRC_REV_OUTPUT_DATA_NONE (0UL) /**< @brief Bit order is not affected. */
#define QSPI_CRC_REV_OUTPUT_DATA_BIT (1UL) /**< @brief Bit reversal output data. */
#define QSPI_CRC_REV_OUTPUT_DATA_XOR (2UL) /**< @brief XOR the output CRC with XOR value. */
#define QSPI_CRC_REV_OUTPUT_DATA_XOR_BYTE_SWAP (4UL) /**< @brief Byte swap the XORed output CRC. */
/** @} */

/**
    @name QSPI_ConfigType
    @{
    @brief QSPI init config structure

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef struct sQSPI_ConfigType {       /**< @brief Config structure used to configure the QSPI peripheral. */
    QSPI_ClkDivRatioType clkDiv;          /**< @brief SCK clock divide select. SCK period = 2*(N+1) system clock period. */
    QSPI_CpolCphaCtrlType clkPolClkPhase; /**< @brief clock polarity and clock phase control. */
    uint32_t holdOffLen;                  /**< @brief Set the number of holdoff SCK cycles after CSB is asserted. */
    uint32_t txFifoThr;                   /**< @brief QSPI Tx Fifo threshold. TX interrupt and DMA request is asserted
                                                                                    when the threshold is reached. */
    uint32_t rxFifoThr;                   /**< @brief QSPI Rx Fifo threshold. RX interrupt and DMA request is asserted
                                                                                     when the threshold is reached. */
    QSPI_SlaveSelType slaveSel;           /**< @brief Select SPI Slave using chip select control.*/
    uint8_t fdxModeEn;                    /**< @brief Full-Duplex mode. Only available in single Lane mode. */
    uint8_t txFIFOWaitEn;                 /**< @brief This enables stopping SCK when TX FIFO becomes empty. */
    uint8_t rxFIFOWaitEn;                 /**< @brief This enables stopping SCK when RX FIFO becomes full. */
    uint8_t arbiterIfEn;                  /**< @brief Arbiter interface enable, this is applicable to Boot Flash QSPI */
    uint8_t quadModeEn;                   /**< @brief  Quad-SPI mode enable. */
} QSPI_ConfigType;
/** @} */

/**
    @name QSPI_DevStatusType
    @{
    @brief QSPI device configuration status structure

    @trace #BRCM_SWREQ_QSPI_DRIVER_INTERFACE
*/
typedef struct sQSPI_DevStatusType {          /**< @brief Get the configured device status of the QSPI peripheral */
    uint8_t qspiBusy;                  /**< @brief QSPI FSM busy */
    uint8_t txFIFOUnderFlowStat;       /**< @brief QSPI TX FIFO under flow status */
    uint8_t txFIFOthrStat;             /**< @brief QSPI TX FIFO threshold status */
    uint8_t txFIFOEmptyStat;           /**< @brief QSPI TX FIFO empty status */
    uint8_t txFIFOFullnessStat;        /**< @brief QSPI TX FIFO fullness status indicates how many valid 4-byte
                                                   words are present in the TX FIFO (0-32) */
    uint8_t rxFIFOOverFlowStat;        /**< @brief QSPI RX FIFO overflow flow status */
    uint8_t rxFIFOthrStat;             /**< @brief QSPI RX FIFO threshold status */
    uint8_t rxFIFOEmptyStat;           /**< @brief QSPI RX FIFO empty status */
    uint8_t rxFIFOFullnessStat;        /**< @brief QSPI RX FIFO fullness status indicates how many valid 4-byte
                                                   words are present in the RX FIFO (0-32) */

} QSPI_DevStatusType;
/** @} */

/**
    @name sQSPI_ModeConfigType
    @{
    @brief QSPI mode operation configuration structure

    @trace #BRCM_SWREQ_QSPI_COMMUNICATION
*/
typedef struct sQSPI_ModeOpConfigType {  /**< @brief Mode operation structure is used to select instruction and opcode mode for transfer */
    QSPI_AddrModeType addrMode;             /**< @brief QSPI address mode */
    QSPI_AddrModeLenType addrModeLen;       /**< @brief QSPI address mode length */
    QSPI_DataModeType dataMode;             /**< @brief QSPI data mode */
    QSPI_OpcodeModeType opcodeMode;         /**< @brief QSPI opcode mode for instruction byte to be sent. */
    QSPI_ModeBitsType modeBits;             /**< @brief QSPI mode bits select. */
    QSPI_ModeBitsLenType modeBitsLen;       /**< @brief QSPI mode bits length select.  */
    uint8_t quadModeEn;                     /**< @brief  Quad-SPI mode enable. */
} QSPI_ModeOpConfigType;
/** @} */

/**
    @name sQSPI_CommandXferType
    @{
    @brief QSPI transfer command structure

    @trace #BRCM_SWREQ_QSPI_COMMUNICATION
*/
typedef struct sQSPI_CommandXferType { /**< @brief Transfer command structure used to select instruction and opcode for transfer. */
    uint32_t dataLen;      /**< @brief SPI Data length in bytes to be sent or received. If CRC is enabled user should pass data length as (DATA_Len + 4). The 4 bytes are for CRC.
                                                       When set to maximum value 0xFFFF_FFFF the Data Phase enters continuous mode. To exit continuous mode set Data_len to 0x0. */
    uint32_t modeBits;     /**< @brief QSPI mode bits for selected mode of the device. */
    uint32_t dummyCycles;  /**< @brief Sets the number of dummy SCK cycles inserted between Modebits and Data payload . */
    uint8_t opcodeVal;    /**< @brief QSPI instruction opcode value for selected opcode. */
    QSPI_TransferType xferMode; /**< @brief Select transfer mode as DMA or PIO mode. */
} QSPI_CommandXferType;
/** @} */

/**
    @name QSPI_CrcControlCfgType
    @{
    @brief QSPI CRC control configuration structure.

    @trace #BRCM_SWREQ_QSPI_DRIVER_INIT
*/
typedef struct sQSPI_CrcControlCfgType {       /**< @brief QSPI CRC control configuration settings. */
    uint32_t crcInitVal;            /**< @brief Initial value for the CRC calculation unit, common for Tx and Rx. */
    uint32_t crcXorVal;             /**< @brief Value of the vector to be XORed with the output CRC after the reversal default value is 0xFFFF-FFFF.
                                                                         Common for Rx and Tx units. */
    uint32_t rdFrameCrc;            /**< @brief This bit controls the CRC computation over complete read frame (including Opcode, Address, Modebits, data) or
                                                                          only data. 1 : CRC computed over complete read frame
                                                                          0 : CRC computed only over received data.  */
    QSPI_CrcRevInputDataType revInputData; /**< @brief Reverse input data.      */
    QSPI_CrcRevOutputDataType revOutputData; /**< @brief Reverse output data. */
    uint32_t inclCrcInStream;           /**< @brief Include the computed CRC at the end of the stream. This bit is used by the CRC generator to offset the programmed Data_len by 4 bytes to
                                                                  accomodate the calculated CRC to be sent at the end of the stream. E.g. if the data-stream is 65 Bytes, Data_len register need to be programmed as (65+4) 69 Bytes.
                                                                  If this bit is set and CRC is enabled user should pass data length as (DATA_Len + 4). The 4 bytes are for CRC. */
} QSPI_CrcControlCfgType;
/** @} */


/* ================================================================
 *  QSPI interface
 *================================================================*/

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

    @limitations None
*/
extern int32_t QSPI_DrvInit(QSPI_HwIDType aID, const QSPI_ConfigType * const aConfig);

/** @brief De-initialize QSPI peripheral.

    This API un-initializes QSPI peripheral and frees necessary software variables, data
    structures.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the QSPI peripheral

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT

    @limitations None
*/
extern int32_t QSPI_DrvDeInit(QSPI_HwIDType aID);

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

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION

    @limitations None
*/
extern int32_t QSPI_ModeConfigure(QSPI_HwIDType aID, const QSPI_ModeOpConfigType * const aModeConfig);

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
    @limitations None
*/
extern int32_t QSPI_SetConfigProperty(QSPI_HwIDType aID, QSPI_ConfigPropType confProp, uint32_t aVal);

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

    @limitations None
*/
extern int32_t QSPI_GetConfigProperty(QSPI_HwIDType aID, QSPI_ConfigPropType confProp, uint32_t *aVal);

/** @brief This API is deprecated it is recommended to call #QSPI_DrvWrite and #QSPI_DrvRead for the
    same purpose.
    QSPI transfer function to write data and read the response from QSPI device.
    This API performs transaction of specified number of bytes to QSPI peripheral connected as slave.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in] xferCmd   Pointer to QSPI command structure holding the instruction code and instruction frame register information
    @param[in] address   Some instructions require address of qspi peripheral to be sent [optional otherwise pass -1 if not required]
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

    @limitation None
 */
extern int32_t QSPI_DrvXfer(QSPI_HwIDType aID, QSPI_CommandXferType *xferCmd, uint32_t address, char *txData, uint32_t txSize, char *rxData, uint32_t rxSize);

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

    @limitation None
 */
extern int32_t QSPI_DrvWrite(QSPI_HwIDType aID, QSPI_CommandXferType *xferCmd, uint32_t address, char *txData, uint32_t txSize);

/** @brief Reads data on QSPI peripheral.

    This API is used to read "rxSize" number of bytes on QSPI module and to stor in rxData buffer.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in] xferCmd   Pointer to QSPI command structure holding the instruction code and instruction frame register information
    @param[in] address    Address to be accessed in QSPI peripheral
    @param[out] rxData   Pointer to receive buffer holding the data to read from memory
    @param[in] rxSize      Number of bytes to read.  If only opcode needs to be sent, rxSize should be passed as 0.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Write operation is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param
    @retval     #BCM_ERR_EAGAIN   Transfer failed try again
    @retval     #BCM_ERR_INVAL_STATE   Device is in invalid state
    @post None

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION

    @limitation None
 */
extern int32_t QSPI_DrvRead(QSPI_HwIDType aID, QSPI_CommandXferType *xferCmd, uint32_t address, char *rxData, uint32_t rxSize);

/** @brief DMA write request enable.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in]      aId         Index of the QSPI controller
    @param[in]      aEnable    Enable (1UL)/Disable(0UL) Transmit DMA request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Write operation is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param
    @retval     #BCM_ERR_EAGAIN   Transfer failed try again
    @retval     #BCM_ERR_INVAL_STATE   Device is in invalid state
    @post None

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION

    @limitation None
 */
extern int32_t QSPI_DrvDMAReqWriteEnable(QSPI_HwIDType aID, uint32_t aEnable);

/** @brief DMA read request enable.

    @behavior Sync, Non-reentrant

    @pre QSPI initialization (QSPI_DrvInit) needs to be completed before calling this function

    @param[in]      aId         Index of the QSPI controller
    @param[in]      aEnable     Enable (1UL)/Disable(0UL) Receive DMA request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   Write operation is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param
    @retval     #BCM_ERR_EAGAIN   Transfer failed try again
    @retval     #BCM_ERR_INVAL_STATE   Device is in invalid state
    @post None

    @trace    #BRCM_SWREQ_QSPI_COMMUNICATION

    @limitation None
 */
extern int32_t QSPI_DrvDMAReqReadEnable(QSPI_HwIDType aID, uint32_t aEnable);

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

    @limitations None
*/
extern int32_t QSPI_TxFifoFlush(QSPI_HwIDType aID);

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

    @limitations None
*/
extern int32_t QSPI_RxFifoFlush(QSPI_HwIDType aID);

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

    @limitations None
*/
extern int32_t QSPI_DrvStatusGet(QSPI_HwIDType aID, QSPI_DevStatusType *devStatus);

/** @brief Enable the QSPI interrupt

    This API enables the interrupt for the selected interrupt types

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

    @limitations None
*/
int QSPI_EnableInterrupt(QSPI_HwIDType aId, QSPI_InterruptType intrType, uint32_t aEnable);

/** @brief Clear the Index of the QSPI peripheral interrupt

    This API clears the interrupt for the selected interrupt types

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID           Index of the QSPI peripheral
    @param[in]      intrType      Clear the selected interrupt types

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver interrupt clear is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_DRIVER_INIT

    @limitations None
*/
int QSPI_ClearInterrupt(QSPI_HwIDType aId, QSPI_InterruptType intrType);

/** @brief Get the QSPI interrupt status

    This API gets the interrupt status for the selected interrupt types

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

    @limitations None
*/
int QSPI_GetInterruptStatus(QSPI_HwIDType aId, QSPI_InterruptType intrType, uint32_t *intrStatus);

/** @brief  QSPI IRQ Handler

  This API is called when QSPI interrupt is asserted.

  @param[in]    aId  Index of the QSPI port. Passed by osil
  @param[in]    aConfig     Pointer to the configuration structure

  @behavior Async, Non-reentrant

  @pre None

  @return       void
  @post None

    @trace #BRCM_SWREQ_QSPI_IRQ_HANDLER

  @limitations None
 */
void QSPI_IRQHandler(QSPI_HwIDType aId, QSPI_ConfigType *aConfig);

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

    @limitations None
*/
int QSPI_CrcControlConfigSet(QSPI_HwIDType aId, QSPI_CrcControlCfgType *crcCfg);

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

    @limitations None
*/
int QSPI_CrcControlConfigGet(QSPI_HwIDType aId, QSPI_CrcControlCfgType *crcCfg);

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

    @limitations None
*/
int QSPI_CrcResetSet(QSPI_HwIDType aId);


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

    @limitations None
*/
int QSPI_CrcEnable(QSPI_HwIDType aId, uint8_t enable);

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

    @limitations None
*/
int QSPI_CrcTxDoneStatus(QSPI_HwIDType aId, uint8_t *done);

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

    @limitations None
*/
int QSPI_CrcRxDoneStatus(QSPI_HwIDType aId, uint8_t *done);

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

    @limitations None
*/
int QSPI_GetCrcTxData(QSPI_HwIDType aId, uint32_t *crcTxData);

/** @brief Gets CRC value from RX data register.

    This API provides CRC value after reading the CRC RX data register.

    @behavior Sync, Non-reentrant

    @pre CRC RX done status should be true to get the latest CRC value after the calculation.

    @param[in]      aId      Index of the QSPI peripheral
    @param[out]    crcRxData   Reads CRC value from Rx data register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION

    @limitations None
*/
int QSPI_GetCrcRxData(QSPI_HwIDType aId, uint32_t *crcRxData);

/** @brief Writes Tx CRC value in QSPI data register.

    This API provides CRC value after reading the CRC TX data register.

    @behavior Sync, Non-reentrant

    @pre CRC TX done status should be true to get the latest CRC value, this will be written at the end of
    the packet transfer immediately after ongoing transfer while CS is asserted and TX FIFO wait is enabled.

    @param[in]      aId             Index of the QSPI peripheral
    @param[in]      crcTxData   Writes CRC value in QSPI data register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK   QSPI driver initialization is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param

    @post None

    @trace  #BRCM_SWREQ_QSPI_COMMUNICATION

    @limitations None
*/
int QSPI_WriteTxCrcData(QSPI_HwIDType aId, uint32_t crcTxData);
#endif /* QSPI_H */

/** @} */
