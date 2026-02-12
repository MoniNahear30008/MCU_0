/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_txslzrdrv_ifc TXSLZR
    @ingroup grp_txslzrdrv

    @addtogroup grp_txslzrdrv_ifc
    @{
    @section sec_txslzr_overview Overview
    -# Tx serializer Bare Metal driver interface provides layer 1 APIs to initialize and configure
    the TXSLZR in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each TXSLZR HW instance needs to be initialized separately using TXSLZR_DrvInit API.
    -# It is user's responsibility to configure DMA separately to handle data transfers.
    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_txslzrdrv_fsm Usage
    -# TXSLZR HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |  TXSLZR_ACQ_0 |

    @section api_list List of TXSLZR APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref TXSLZR_DrvInit             | Initialize TXSLZR                                                |
    | @ref TXSLZR_DrvDeInit           | De-initialize TXSLZR                                             |
    | @ref TXSLZR_DrvPageConfig       | Page configuration                                               |
    | @ref TXSLZR_DrvPatGenEnable     | Enable pattern generator                                         |
    | @ref TXSLZR_DrvPatGenWrPage     | Writes page into pattern generator memory                        |
    | @ref TXSLZR_DrvPatGenRdPage     | Read page from pattern generator memory                          |
    | @ref TXSLZR_DrvPrbsConfigSet    | Sets PRBS configuration.                                         |
    | @ref TXSLZR_DrvPrbsEnable       | Enable/Disable the PRBS generator                                |
    | @ref TXSLZR_DrvEnableInterrupt  | Enable TXSLZR interrupts                                         |
    | @ref TXSLZR_DrvInterruptStatus  | Gets TXSLZR interrupt status                                     |
    | @ref TXSLZR_DrvEventStatus      | Gets TXSLZR event status                                         |
    | @ref TXSLZR_DrvClearInterrupt   | Clears TXSLZR interrupt                                          |
    | @ref TXSLZR_DrvSeqModeConfig    | TXSLZR sequencer mode configuration                              |
    | @ref TXSLZR_DrvSeqControl       | TXSLZR sequencer control                                         |
    | @ref TXSLZR_DrvSeqSpiCfg        | TXSLZR sequencer SPI configuration                               |
    | @ref TXSLZR_DrvSeqMemRdWr       | TXSLZR sequencer memory read/write                               |
    | @ref TXSLZR_DrvSwSpiRdWr        | TXSLZR sequencer software SPI read/write                         |

    @file txslzr.h
    @brief Interface for TXSLZR driver
    @section apis This section defines the interface for TXSLZR driver
    @version 0.1 Initial Version
*/

#ifndef TXSLZR_H
#define TXSLZR_H

#include <stdint.h>

/**
    @name TXSLZR driver API IDs
    @{
    @brief API IDs for TXSLZR driver
 */
#define BRCM_SWARCH_TXSLZR_HW_ID_TYPE                     (0x85C0U)    /**< @brief #TXSLZR_HwIDType             */
#define BRCM_SWARCH_TXSLZR_STATE_TYPE                     (0x85C1U)    /**< @brief #TXSLZR_StateType            */
#define BRCM_SWARCH_TXSLZR_NUM_OF_PAGE_TYPE               (0x85C2U)    /**< @brief #TXSLZR_NumOfPageType        */
#define BRCM_SWARCH_TXSLZR_ADDR_MODE_TYPE                 (0x85C3U)    /**< @brief #TXSLZR_AddrModeType         */
#define BRCM_SWARCH_TXSLZR_PRBS_POLY_TYPE                 (0x85C4U)    /**< @brief #TXSLZR_PrbsPolyType         */
#define BRCM_SWARCH_TXSLZR_PAGE_INFO_TYPE                 (0x85C5U)    /**< @brief #TXSLZR_PageInfoType         */
#define BRCM_SWARCH_TXSLZR_PRBS_CONFIG_TYPE               (0x85C6U)    /**< @brief #TXSLZR_PrbsConfigType       */
#define BRCM_SWARCH_TXSLZR_INTERRUPT_TYPE                 (0x85C7U)    /**< @brief #TXSLZR_InterruptType        */

#define BRCM_SWARCH_TXSLZR_DRV_INIT_PROC                  (0x85D0U)    /**< @brief #TXSLZR_DrvInit              */
#define BRCM_SWARCH_TXSLZR_DRV_DEINIT_PROC                (0x85D1U)    /**< @brief #TXSLZR_DrvDeInit            */
#define BRCM_SWARCH_TXSLZR_DRV_PAGE_CONFIG_PROC           (0x85D2U)    /**< @brief #TXSLZR_DrvPageConfig        */
#define BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_ENABLE_PROC        (0x85D3U)    /**< @brief #TXSLZR_DrvPatGenEnable      */
#define BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_WR_PAGE_PROC       (0x85D7U)    /**< @brief #TXSLZR_DrvPatGenWrPage      */
#define BRCM_SWARCH_TXSLZR_DRV_PAT_GEN_RD_PAGE_PROC       (0x85DBU)    /**< @brief #TXSLZR_DrvPatGenRdPage      */
#define BRCM_SWARCH_TXSLZR_DRV_PRBS_CONFIG_SET_PROC       (0x85DCU)    /**< @brief #TXSLZR_DrvPrbsConfigSet     */
#define BRCM_SWARCH_TXSLZR_DRV_PRBS_ENABLE_PROC           (0x85DDU)    /**< @brief #TXSLZR_DrvPrbsEnable        */
#define BRCM_SWARCH_TXSLZR_DRV_ENABLE_INTERRUPT_PROC      (0x85E0U)    /**< @brief #TXSLZR_DrvEnableInterrupt   */
#define BRCM_SWARCH_TXSLZR_DRV_INTERRUPT_STATUS_PROC      (0x85E1U)    /**< @brief #TXSLZR_DrvInterruptStatus   */
#define BRCM_SWARCH_TXSLZR_DRV_EVENT_STATUS_PROC          (0x85E2U)    /**< @brief #TXSLZR_DrvEventStatus       */
#define BRCM_SWARCH_TXSLZR_DRV_CLEAR_INTERRUPT_PROC       (0x85E3U)    /**< @brief #TXSLZR_DrvClearInterrupt    */

#define BRCM_SWARCH_TXSLZR_SPICONTROL_TYPE                 (0x85E4U)   /**< @brief #TXSLZR_SpiControlType*/
#define BRCM_SWARCH_TXSLZR_SPICONFIG_TYPE                  (0x85E5U)   /**< @brief #TXSLZR_SpiConfigType*/
#define BRCM_SWARCH_TXSLZR_SWSPIRDWR_TYPE                  (0x85E6U)   /**< @brief #TXSLZR_SwSpiRdWrType*/
#define BRCM_SWARCH_TXSLZR_SEQMEMRDWR_TYPE                 (0x85E7U)   /**< @brief #TXSLZR_SeqMemRdWrType*/
#define BRCM_SWARCH_TXSLZR_DRV_SEQSPICFG_PROC              (0x85E8U)   /**< @brief #TXSLZR_DrvSeqSpiCfg*/
#define BRCM_SWARCH_TXSLZR_DRV_SEQCONTROL_PROC             (0x85E9U)   /**< @brief #TXSLZR_DrvSeqControl*/
#define BRCM_SWARCH_TXSLZR_DRV_SEQMEMRDWR_PROC             (0x85EAU)   /**< @brief #TXSLZR_DrvSeqMemRdWr*/
#define BRCM_SWARCH_TXSLZR_DRV_SWSPIRDWR_PROC              (0x85EBU)   /**< @brief #TXSLZR_DrvSwSpiRdWr*/
#define BRCM_SWARCH_TXSLZR_DRV_SEQ_MODE_CFG_PROC           (0x85ECU)   /**< @brief #TXSLZR_DrvSeqModeConfig*/

/** @} */

/**
    @name TXSLZR_HwIDType
    @{
    @brief Index of the TXSLZR module
    @trace #BRCM_SWREQ_TXSLZR_INIT
 */
typedef uint32_t TXSLZR_HwIDType;  /**< @brief TXSLZR HW Id type */
#define TXSLZR_HW_ID_0      (0UL)  /**< @brief TXSLZR HW Id 0    */
/** @} */

/**
    @name TXSLZR_StateType
    @{
    @brief State of TXSLZR
    @trace #BRCM_SWREQ_TXSLZR_INIT
 */
typedef uint8_t TXSLZR_StateType;         /**< @brief TXSLZR state type */
#define TXSLZR_STATE_UNINIT         0U    /**< @brief TXSLZR is uninitialized and value must be zero*/
#define TXSLZR_STATE_INITIALIZED    1U    /**< @brief TXSLZR is initialized */
/** @} */

/**
    @name TXSLZR_NumOfPageType
    @{
    @brief TXSLZR number of page Type

    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
 */
typedef uint32_t TXSLZR_NumOfPageType;      /**< @brief Select TXSLZR number of pages */
#define TXSLZR_NUM_OF_PAGE_1         0UL    /**< @brief TXSLZR number of page 1       */
#define TXSLZR_NUM_OF_PAGE_2         1UL    /**< @brief TXSLZR number of page 2       */
#define TXSLZR_NUM_OF_PAGE_4         2UL    /**< @brief TXSLZR number of page 4       */
#define TXSLZR_NUM_OF_PAGE_8         3UL    /**< @brief TXSLZR number of page 8       */
#define TXSLZR_NUM_OF_PAGE_16        4UL    /**< @brief TXSLZR number of page 16      */
/** @} */

/**
    @name TXSLZR_AddrModeType
    @{
    @brief TXSLZR address mode type

    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
 */
typedef uint32_t TXSLZR_AddrModeType;        /**< @brief TXSLZR Address mode type.  */
#define TXSLZR_ADDR_MODE_DEFAULT        0UL  /**< @brief TXSLZR non increment mode  */
#define TXSLZR_ADDR_MODE_AUTO_INC       1UL  /**< @brief TXSLZR auto increment mode */
/** @} */

/**
    @name TXSLZR_PrbsPolyType
    @{
    @brief TXSLZR PRBS polynomial type

    @trace #BRCM_SWREQ_TXSLZR_PRBS
 */
typedef uint32_t TXSLZR_PrbsPolyType;         /**< @brief TXSLZR PRBS polynomial type. */
#define TXSLZR_PRBS_POLY_7              0UL  /**< @brief TXSLZR PRBS polynomial 7     */
#define TXSLZR_PRBS_POLY_15             1UL  /**< @brief TXSLZR PRBS polynomial 15    */
/** @} */

/**
    @name TXSLZR_PageInfoType
    @{
    @brief TXSLZR pattern generator Page Info Type

    @trace #BRCM_SWREQ_TXSLZR_PAT_GEN
 */
typedef struct sTXSLZR_PageInfoType { /**< @brief TXSLZR pattern generator Page Info Type */
    uint32_t pageStartAddr;           /**< @brief TXSLZR page start address.              */
    uint32_t pageSize ;               /**< @brief TXSLZR page size                        */
    uint32_t pageData[256UL];         /**< @brief TXSLZR page data buffer                 */
} TXSLZR_PageInfoType;
/** @} */

/**
    @name TXSLZR_PrbsConfigType
    @{
    @brief TXSLZR PRBS configuration Type

    @trace #BRCM_SWREQ_TXSLZR_PRBS
 */
typedef struct sTXSLZR_PrbsConfigType { /**< @brief TXSLZR PRBS configuration type */
    TXSLZR_PrbsPolyType poly;           /**< @brief TXSLZR PRBS polynomial type.   */
    uint32_t invert;                    /**< @brief TXSLZR PRBS output invert      */
    uint32_t reverse ;                  /**< @brief TXSLZR PRBS output reverse     */
} TXSLZR_PrbsConfigType;
/** @} */

/**
    @name TXSLZR_InterruptType
    @{
    @brief TXSLZR Interrupt Type

    @trace #BRCM_SWREQ_TXSLZR_INTERRUPT
 */
typedef uint32_t TXSLZR_InterruptType;                     /**< @brief Select TXSLZR interrupt type.               */
#define TXSLZR_INTERRUPT_SEQ_SINGLE_SHOT_DONE  (1UL<< 0UL) /**< @brief TXSLZR sequencer single shot done interrupt */
#define TXSLZR_INTERRUPT_ALL                   (0x1UL)     /**< @brief TXSLZR All interrupts */
/** @} */

/** @brief Initializes TXSLZR

    This API initializes the given TXSLZR hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId         Index of the TXSLZR controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_INIT

    @limitations None
*/
int32_t TXSLZR_DrvInit(TXSLZR_HwIDType  hwId);

/** @brief De-initializes TXSLZR

    This API de-initializes TXSLZR.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId         Index of the TXSLZR controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_INIT

    @limitations None
*/
int32_t TXSLZR_DrvDeInit(TXSLZR_HwIDType hwId);

/** @brief TXSLZR page configuration

    This API sets page configuration parameters.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the TXSLZR controller
    @param[in]  numOfPage     Number of pages
    @param[in]  pageSize      Page size
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   (numOfPage * pageSize) greater than or equal 256
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_PAT_GEN

    @limitations None
*/
int32_t TXSLZR_DrvPageConfig(TXSLZR_HwIDType hwId, TXSLZR_NumOfPageType numOfPage, uint32_t pageSize);

/** @brief TXSLZR enable/disable pattern generator

    This API enables/disables pattern generator.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId          Index of the TXSLZR controller
    @param[in]  enaDis       Enable(1)/Disable(0) pattern generator
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_PAT_GEN

    @limitations None
*/
int32_t TXSLZR_DrvPatGenEnable(TXSLZR_HwIDType hwId, uint8_t enaDis);

/** @brief TXSLZR Write page into pattern generator memory.

    This API writes page into pattern generator memory.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId       Index of the TXSLZR controller
    @param[in]  pageInfo   Pointer to page information
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL page info
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_PAT_GEN

    @limitations None
*/
int32_t TXSLZR_DrvPatGenWrPage(TXSLZR_HwIDType hwId, TXSLZR_PageInfoType *pageInfo);

/** @brief TXSLZR Read page from pattern generator memory.

    This API reads page from pattern generator memory.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId       Index of the TXSLZR controller
    @param[in/out]  pageInfo   Pointer to page information
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL page info
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_PAT_GEN

    @limitations None
*/
int32_t TXSLZR_DrvPatGenRdPage(TXSLZR_HwIDType hwId, TXSLZR_PageInfoType *pageInfo);

/** @brief TXSLZR PRBS configuration.

    This API sets PRBS configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId        Index of the TXSLZR controller
    @param[in/out]  prbsConfig  Pointer to PRBS configuration structure
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL PRBS configuration
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_PRBS

    @limitations None
*/
int32_t TXSLZR_DrvPrbsConfigSet(TXSLZR_HwIDType hwId, TXSLZR_PrbsConfigType *prbsConfig);

/** @brief TXSLZR PRBS enable/disable control.

    This API enables/disables the PRBS generator.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId        Index of the TXSLZR controller
    @param[in/out]  ena_dis     enable(1)/disable(0) PRBS generator
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_PRBS

    @limitations None
*/
int32_t TXSLZR_DrvPrbsEnable(TXSLZR_HwIDType hwId, uint32_t ena_dis);

/** @brief TXSLZR enable interrupts

    This API enables interrupts based on interrupt type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId       Index of the TXSLZR controller
    @param[in]   intrType   Specified interrupt type
    @param[out]  aEnable    Enable/disable control
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_INTERRUPT

    @limitations None
*/
int32_t TXSLZR_DrvEnableInterrupt(TXSLZR_HwIDType hwId, TXSLZR_InterruptType intrType, uint32_t aEnable);

/** @brief TXSLZR interrupt status

    This API gets interrupt status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId      Index of the TXSLZR controller
    @param[in]   intrSts   Pointer to interrupt status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL interrupt status
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_INTERRUPT

    @limitations None
*/
int32_t TXSLZR_DrvInterruptStatus(TXSLZR_HwIDType hwId, TXSLZR_InterruptType *intrSts);

/** @brief TXSLZR event status

    This API gets event status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId      Index of the TXSLZR controller
    @param[in]   intrSts   Pointer to event status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL event status
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_INTERRUPT

    @limitations None
*/
int32_t TXSLZR_DrvEventStatus(TXSLZR_HwIDType hwId, TXSLZR_InterruptType *eventSts);

/** @brief TXSLZR clear interrupts

    This API clears interrupt based on interrupt type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId       Index of the TXSLZR controller
    @param[in]   intrType   Interrupt type to clear the interrupt
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_NODEV          TXSLZR instance not initialized

    @post None

    @trace  #BRCM_SWREQ_TXSLZR_INTERRUPT

    @limitations None
*/
int32_t TXSLZR_DrvClearInterrupt(TXSLZR_HwIDType hwId, TXSLZR_InterruptType intrType);


/** @brief TXSLZR sequencer mode

    This API configures TXSLZR sequencer mode i.e. single shot or continuous mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the TXSLZR controller
    @param[in]      mode             0: Single shot mode
                                     1: Continuous mode

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_TXSLZR_SEQUENCER

    @limitations None
*/
int32_t TXSLZR_DrvSeqModeConfig(TXSLZR_HwIDType  hwId,  uint8_t mode);

/** @brief TXSLZR sequencer control

    This API controls TXSLZR sequencer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the TXSLZR controller
    @param[in]      enDis            1 : Enable sequencer
                                     0 : Disable sequencer

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post   NA

    @trace  #BRCM_SWREQ_TXSLZR_SEQUENCER

    @limitations None
*/
int32_t TXSLZR_DrvSeqControl(TXSLZR_HwIDType  hwId,  uint8_t enDis);

/**
    @name TXSLZR_SpiControlType
    @{
    @brief SPI Control for sequencer

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
*/
typedef uint32_t TXSLZR_SpiControlType;                        /**< @brief SPI control configuration,
                                                                           user has to use @ref TXSLZR_SPI_CONTROL_BUILD to
                                                                           build SPI control Type */
#define TXSLZR_SPICONTROL_SEQ_SPI_WAIT_TIME_SHIFT   (16UL)     /**< @brief SPI wait time(WT) shift */
#define TXSLZR_SPICONTROL_SPI_CLK_DIVIDER_SHIFT     (8UL)      /**< @brief SPI Clock Divider (CLDIV) shift */
#define TXSLZR_SPICONTROL_SPI_CMD_SIZE_SHIFT        (6UL)      /**< @brief SPI command size (CSZ) shift */
#define TXSLZR_SPICONTROL_SPI_CMD_CNT_SHIFT         (4UL)      /**< @brief SPI command count (CCNT) shift*/
#define TXSLZR_SPICONTROL_SPI_CPOL_SHIFT            (3UL)      /**< @brief SPI polarity(PL) shift*/
#define TXSLZR_SPICONTROL_SPI_CPHA_SHIFT            (2UL)      /**< @brief SPI phase (PH) shift*/
#define TXSLZR_SPICONTROL_SPI_SEQ_EN_SHIFT          (0UL)      /**< @brief SPI sequencer enable (SE) shift */

#define TXSLZR_SPICONTROL_BUILD(SE,PH,PL,CCNT,CSZ,CLDIV,WT)  ((WT << TXSLZR_SPICONTROL_SEQ_SPI_WAIT_TIME_SHIFT) | (CLDIV << TXSLZR_SPICONTROL_SPI_CLK_DIVIDER_SHIFT) | (CSZ << TXSLZR_SPICONTROL_SPI_CMD_SIZE_SHIFT) | (CCNT << TXSLZR_SPICONTROL_SPI_CMD_CNT_SHIFT) | (PL << TXSLZR_SPICONTROL_SPI_CPOL_SHIFT) | (PH << TXSLZR_SPICONTROL_SPI_CPHA_SHIFT) |(SE << TXSLZR_SPICONTROL_SPI_SEQ_EN_SHIFT))                           /**< @brief SPI control builder*/

/** @} */

/**
    @brief Tx serializer SPI control

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
 */

typedef struct sTXSLZR_SpiConfigType {
    TXSLZR_SpiControlType spiControl;
    uint32_t seqSpiCfg;                    /* 0: Static SPI config. The lower 8 bits or the complete 16 bits of spiData are send out on SPI.
                                                   -  8b/16b configuration is chosen based on SPI_CMD_SIZE
                                              1: Dynamic SPI config. This is valid in 16b SPI mode (SPI_CMD_SIZE = 1).
                                                   -  The 16b SPI data sent is specified as {spiData[7:0],dataRptCnt[6:0],dataEndFlag}
                                                   -  CURR_RPT_CNT is a dynamic count value (counts up from 0 to dataRptCnt-1)
                                                      when dataRptCnt is non-zero for the entry.*/
    uint32_t spiSeqOut;                   /*0: Sequencer data (bits 31:16 of sequencer memory) output in serial
                                               form over SPI interface
                                            1: Lower 3 bits of sequencer data output as parallel data. Bits 18:16
                                               of sequencer memory sent out over {csn, mosi, sck}*/
} TXSLZR_SpiConfigType;


/** @brief TXSLZR sequencer SPI config

    This API configures TXSLZR sequencer SPI.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the TXSLZR controller
    @param[in]      spiCfg           Spi configuration

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid spiCfg

    @post   NA

    @trace  #BRCM_SWREQ_TXSLZR_SEQUENCER

    @limitations None
*/
int32_t TXSLZR_DrvSeqSpiCfg(TXSLZR_HwIDType  hwId,  TXSLZR_SpiConfigType* spiCfg);



/**
    @brief TXSLZR sequencer read/write

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
 */

typedef struct sTXSLZR_SeqMemRdWrType {
    uint8_t readWrite;                      /**< @brief 0 :Read 1:Write sequencer memory*/
    uint8_t startAddress;                   /**< @brief Start Address for sequencer indirect address */
    uint8_t dataSize;                       /**< @brief Size of sequencer data */
    uint8_t *dataEndFlag;                   /**< @brief End flag. Sequencer will stop after processing this entry for
                                                        single shot mode and will restart processing from
                                                        0th entry for continuous mode. */
    uint8_t *dataRptCnt;                    /**< @brief Repeat count. The entry will be executed dataRptCnt+1 times*/
    uint16_t *spiData;                      /**< @brief SPI data to be send out, after the current entry is processed*/
} TXSLZR_SeqMemRdWrType;

/** @brief TXSLZR sequencer Memory

    This API reads/write TXSLZR sequencer memory.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the TXSLZR controller
    @param[inout]   seqMem           Sequencer memory

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid seqMem

    @post   NA

    @trace  #BRCM_SWREQ_TXSLZR_SEQUENCER

    @limitations None
*/
int32_t TXSLZR_DrvSeqMemRdWr(TXSLZR_HwIDType  hwId,  TXSLZR_SeqMemRdWrType *seqMem);

/**
    @brief House keeping ADC sequencer software SPI

    @trace #BRCM_SWREQ_TXSLZR_SEQUENCER
 */

typedef struct sTXSLZR_SwSpiRdWrType {
    uint8_t readWrite;                      /**< @brief 0: Read 1: Write SW SPI Data*/
    uint8_t readDone;                       /**< @brief 0: Read Inprogress 1 : read completed*/
    uint8_t readCmd;                        /**< @brief 0:1 byte 1: 2 bytes, 2: 4 bytes */
    uint8_t readCfg;                        /**< @brief Bytes to read 0: 1byte 1: 2 bytes */
    uint16_t data;                         /**< @brief Software spi command to be sent out/read */
    uint16_t turnaround;                    /**< @brief Number of SCK clock cycles the CSn is asserted between mosi write to read */
} TXSLZR_SwSpiRdWrType;

/** @brief TXSLZR sequencer Software SPI control

    This API reads/write TXSLZR sequencer software SPI control.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the TXSLZR controller
    @param[inout]   swSpi           Sequencer memory

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid swSpi

    @post   NA

    @trace  #BRCM_SWREQ_TXSLZR_SEQUENCER

    @limitations None
*/
int32_t TXSLZR_DrvSwSpiRdWr(TXSLZR_HwIDType  hwId,  TXSLZR_SwSpiRdWrType *swSpi);

#endif /* TXSLZR_H */

/** @} */
