/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_pciedrv_ifc PCIe Interface
    @ingroup grp_pciedrv

    @addtogroup grp_pciedrv_ifc
    @{
    @section sec_pcie_overview Overview
    PCIe express interface supporting Gen3 protocol. PCI controller can be configured either
    in Endpoint mode (Using API #PCIE_EpInit) or Root complex mode (using API #PCIE_RcInit).
    It supports following in the selected initialization mode-
    -# Initializing PCIe controller as Endpoint or Root complex.
    -# Configure PCIe Block with endpoint parameters <br>
        i) Device ID <br>
        ii) BAR space <br>
        iii) Internal memory mapping and attributes <br>
    -# RC space programming in root complex mode initialization.
    -# It supports legacy and MSI interrupts.
    -# Enable and disable PCIe link
    -# Deinitialize PCIe driver
    Hotplug and dynamic re-enumeration shall be handled by HW and no
    software intervention is provided.
    @note Root complex APIs are in initial stage, it might change in future.

    | HwID        | HW Instance   |
    |:-----------:|:-------------:|
    | 0           |   PCIE_0      |

    @section api_list List of PCIE APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref PCIE_EpInit | Initializes PCIE endpoint |
    | @ref PCIE_RcInit | Initializes PCIE root complex |
    | @ref PCIE_SetInterface | Enable/disable PCIE interface |
    | @ref PCIE_DeInit | De-initialize PCIE interface |
    | @ref PCIE_InterruptConfigSet | PCIE interrupt configuration |
    | @ref PCIE_EnableInterrupts | PCIE Enable/disable interrupts |
    | @ref PCIE_GetPendingInterrupts | PCIE get pending interrupts |
    | @ref PCIE_DrvIndirectIO | PCIE Indirect write to registers |
    | @ref PCIE_DrvIndirectIORead | PCIE indirect read to registers  |
    | @ref PCIE_DrvCfgRead | PCIE configuration space read |
    | @ref PCIE_DrvCfgWrite | PCIE configuration space write |
    | @ref PCIE_GetPcieRstStatus | PCIE get reset status |
    | @ref PCIE_GetLinkStatus | PCIE get Link status |
    @file pciedrv.h
    @brief PCIe Driver interface

    @version 0.1 Initial version
*/

#ifndef PCIEDRV_H
#define PCIEDRV_H

#include <stdint.h>
#include <bcm_utils.h>
#include <bcm_err.h>

/**
    @name Architecture IDs
    @{
    @brief Architecture IDs for PCIe Driver
*/
#define BRCM_SWARCH_PCIE_HW_ID_TYPE                        (0x8000U) /**< @brief #PCIE_HwIDType             */
#define BRCM_SWARCH_PCIE_BAR_NUM_TYPE                      (0x8001U) /**< @brief #PCIE_BARNumType           */
#define BRCM_SWARCH_PCIE_RC_SPACE_NUM_TYPE                 (0x8002U) /**< @brief #PCIE_RCSpaceNumType       */
#define BRCM_SWARCH_PCIE_USE_INTR_TYPE                     (0x8003U) /**< @brief #PCIE_UseIntrType          */
#define BRCM_SWARCH_PCIE_MEM_SIZE_TYPE                     (0x8004U) /**< @brief #PCIE_MemSizeType          */
#define BRCM_SWARCH_PCIE_MEM_ATTRIB_TYPE                   (0x8005U) /**< @brief #PCIE_MemAttribTypes       */
#define BRCM_SWARCH_PCIE_BARMAP_CONFIG_TYPE                (0x8006U) /**< @brief #PCIE_BARMapConfigType     */
#define BRCM_SWARCH_PCIE_BARCONFIG_TYPE                    (0x8007U) /**< @brief #PCIE_BARConfigType        */
#define BRCM_SWARCH_PCIE_EP_CONFIG_TYPE                    (0x8008U) /**< @brief #PCIE_EpConfigType         */
#define BRCM_SWARCH_PCIE_RCMAP_CONFIG_TYPE                 (0x8009U) /**< @brief #PCIE_RcMapConfigType      */
#define BRCM_SWARCH_PCIE_RCSPACE_CONFIG_TYPE               (0x800AU) /**< @brief #PCIE_RcSpaceConfigType    */
#define BRCM_SWARCH_PCIE_RC_CONFIG_TYPE                    (0x800BU) /**< @brief #PCIE_RcConfigType         */
#define BRCM_SWARCH_PCIE_MAX_NUM_OF_INTR_REMAP_INPUT_MACRO (0x800CU) /**< @brief #PCIE_MAX_NUM_OF_INTR_REMAP_INPUT */

#define BRCM_SWARCH_PCIE_INTR_EP_CFG_TYPE                  (0x800EU) /**< @brief #PCIE_IntrEpCfgType          */
#define BRCM_SWARCH_PCIE_SYS_MSI_EQCTRL_TYPE               (0x800FU) /**< @brief #PCIE_SysMsiEqCtrlType          */
#define BRCM_SWARCH_PCIE_INTR_RC_CFG_TYPE                  (0x8010U) /**< @brief #PCIE_IntrRcCfgType          */
#define BRCM_SWARCH_PCIE_INTRCFG_TYPE                      (0x8011U) /**< @brief #PCIE_IntrCfgType          */
#define BRCM_SWARCH_PCIE_EP_INIT_PROC                      (0x8012U) /**< @brief #PCIE_EpInit               */
#define BRCM_SWARCH_PCIE_SET_INTERFACE_PROC                (0x8013U) /**< @brief #PCIE_SetInterface            */
#define BRCM_SWARCH_PCIE_DE_INIT_PROC                      (0x8014U) /**< @brief #PCIE_DeInit               */
#define BRCM_SWARCH_PCIE_RC_INIT_PROC                      (0x8015U) /**< @brief #PCIE_RcInit               */
#define BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC         (0x8016U) /**< @brief #PCIE_InterruptConfigSet   */
#define BRCM_SWARCH_PCIE_ENABLE_INTERRUPTS_PROC            (0x8017U) /**< @brief #PCIE_EnableInterrupts     */
#define BRCM_SWARCH_PCIE_GET_PENDING_INTERRUPTS_PROC       (0x8018U) /**< @brief #PCIE_GetPendingInterrupts */
#define BRCM_SWARCH_PCIE_DRV_INDIRECT_IO_PROC              (0x801AU) /**< @brief #PCIE_DrvIndirectIO      */
#define BRCM_SWARCH_PCIE_DRV_INDIRECT_IO_READ_PROC         (0x801BU) /**< @brief #PCIE_DrvIndirectIORead      */
#define BRCM_SWARCH_PCIE_DRV_CFG_READ_PROC                 (0x801CU) /**< @brief #PCIE_DrvCfgRead      */
#define BRCM_SWARCH_PCIE_DRV_CFG_WRITE_PROC                (0x801DU) /**< @brief #PCIE_DrvCfgWrite      */
#define BRCM_SWARCH_PCIE_GET_PCIE_RST_STATUS_PROC          (0x801EU) /**< @brief #PCIE_GetPcieRstStatus      */
#define BRCM_SWARCH_PCIE_GET_LINK_STATUS_PROC              (0x801FU) /**< @brief #PCIE_GetLinkStatus */


/** @} */

/**
    @name PCIe controller index
    @{
    @brief Type for PCIe controller hardware ID

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_HwIDType;  /**< @brief PCIE controller index type */
#define PCIE_HW_ID_0     (0UL)   /**< @brief PCIE controller ID 0 */
#define PCIE_HW_ID_MAX   (1UL)   /**< @brief MAX PCIe Controllers */
/** @} */

/**
    @name PCIe BAR Number
    @{
    @brief BAR Number used for PCIe interface

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_BARNumType;
#define PCIE_BAR_NUM_1              (0UL)
#define PCIE_BAR_NUM_2              (1UL)
#define PCIE_BAR_NUM_3              (2UL)
#define PCIE_BAR_NUM_MAX            (2UL)
#define PCIE_BAR_NUM_RC_MAX         (1UL)
/** @} */

/**
    @name PCIe RC space Number
    @{
    @brief PCIE maximum number of RC spaces.
    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_RCSpaceNumType;
#define PCIE_RC_SPACE_NUM_1              (0UL)
#define PCIE_RC_SPACE_NUM_2              (1UL)
#define PCIE_RC_SPACE_NUM_3              (2UL)
#define PCIE_RC_SPACE_NUM_MAX            (2UL)
/** @} */

/**
    @name PCIe Interrupt type
    @{
    @brief PCIe select interrupt type to use.

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint8_t PCIE_UseIntrType;
#define PCIE_USE_INTR_INTX     (0U) /**< @brief Legacy PCI interrupt Pin is used */
#define PCIE_USE_INTR_MSI      (1U) /**< @brief PCIe message based interrupt MSI is used */
#define PCIE_USE_INTR_MSIX     (2U) /**< @brief PCIe message based extended MSI interrupt */
/** @} */

/**
    @name PCIe Memory size type
    @{
    @brief Memory sizes used by PCIe driver

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_MemSizeType;
#define PCIE_MEM_SIZE_NONE          (0UL)
#define PCIE_MEM_SIZE_1K            (1UL)
#define PCIE_MEM_SIZE_2K            (2UL)
#define PCIE_MEM_SIZE_4K            (3UL)
#define PCIE_MEM_SIZE_8K            (4UL)
#define PCIE_MEM_SIZE_16K           (5UL)
#define PCIE_MEM_SIZE_32K           (6UL)
#define PCIE_MEM_SIZE_64K           (7UL)
#define PCIE_MEM_SIZE_128K          (8UL)
#define PCIE_MEM_SIZE_256K          (9UL)
#define PCIE_MEM_SIZE_512K          (10UL)
#define PCIE_MEM_SIZE_1M            (11UL)
#define PCIE_MEM_SIZE_2M            (12UL)
#define PCIE_MEM_SIZE_4M            (13UL)
#define PCIE_MEM_SIZE_8M            (14UL)
#define PCIE_MEM_SIZE_16M           (15UL)
#define PCIE_MEM_SIZE_32M           (16UL)
#define PCIE_MEM_SIZE_64M           (17UL)
#define PCIE_MEM_SIZE_128M          (18UL)
#define PCIE_MEM_SIZE_256M          (19UL)
#define PCIE_MEM_SIZE_512M          (20UL)
#define PCIE_MEM_SIZE_1G            (21UL)
#define PCIE_MEM_SIZE_MAX           (21UL)
/** @} */

/**
    @name PCIe Memory attribute flags
    @{
    @brief PCIe memory attribute flags

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_MemAttribTypes;
#define PCIE_MEM_ATTRIB_READ       (1UL) /**< @brief Read access enable  */
#define PCIE_MEM_ATTRIB_WRITE      (2UL) /**< @brief Write access enable */
#define PCIE_MEM_ATTRIB_RD_CACHE   (4UL) /**< @brief Read Cache enable   */
#define PCIE_MEM_ATTRIB_WR_CACHE   (8UL) /**< @brief Write Cache enable  */
/** @} */
/**
   @brief PCIe INTC_REMAP_BITPOS_REG[0-3] is used to map interrupt inputs
   16 [15:0] to 32 interrupt outputs generated for MSI.
   @trace #BRCM_SWREQ_PCIE_DRV
*/
#define PCIE_MAX_NUM_OF_INTR_REMAP_INPUT (16UL)

/**
    @name PCIE_IntrEpCfgType
    @{
    @brief PCIe interrupt configuration (Legacy or MSI) for the selected interrupt type.

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_IntrEpCfgType {
    uint32_t multiMsgCap;       /**< @brief This value controls value of multiple message generated. */
    uint32_t msiPvMaskCapable;  /**< @brief This value controls the per vector masking capability in the MSI control field. */
    uint32_t msiCapEna;         /**< @brief PCIE capability setting. */
    uint32_t intrRemap[PCIE_MAX_NUM_OF_INTR_REMAP_INPUT]; /**< Remap interrupt input to generated MSI interrupt output 0-31 . */
    uint32_t intrClrDelay;      /**< @brief Auto interrupt clear delay timer value for auto mode. */
} PCIE_IntrEpCfgType;
/** @} */

/**
    @name PCIE_SysMsiEqCtrlType
    @{
    @brief MSI event queues[0-5] settings for RC.

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_SysMsiEqCtrlType {
    uint32_t eqEnable;   /**< @brief Event Queue enable */
    uint32_t intrNEvent; /**< @brief The number of MSI messages must be received before interrupt is generated, max value 0x3F. */
    uint32_t intrNDelay; /**< @brief Generate interrupt after 2^intrNDelay, recommended value 2 or 3.*/
    uint32_t msiIntrEnEq; /**< @brief MSI interrupt enable for selected event Queue. */
} PCIE_SysMsiEqCtrlType;
/** @} */

/**
    @name PCIE_IntrEpCfgType
    @{
    @brief PCIe interrupt configuration (Legacy or MSI) for the selected interrupt type.

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_IntrRcCfgType {
    uint32_t rcMsiAddrLow;        /**< @brief This register controls the lower half of the address of the MSI message that are generated.*/
    uint32_t rcMsiAddrHigh;       /**< @brief This register controls the upper half of the address of the MSI message that are generated.*/
    uint32_t pcieSysMsiEventQueuePageAddrLow; /**< @brief PCIe SYS MSI event queue page address */
    uint32_t pcieSysMsiEventQueuePageAddrUp; /**< @brief PCIe SYS MSI event queue upper page address */
    PCIE_SysMsiEqCtrlType msiEqCtrl[6UL]; /**< @brief MSI event queue control settings for 6 event queue. */
} PCIE_IntrRcCfgType;
/** @} */

/**
    @name PCIE_IntrCfgType
    @{
    @brief PCIe interrupt configuration (Legacy or MSI) for the selected interrupt type.

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_IntrCfgType {
    PCIE_IntrEpCfgType *epConfig;  /**< Interrupt config for end point. If no configuration needed, pass NULL. */
    PCIE_IntrRcCfgType *rcConfig;  /**< Interrupt config for Root complex. If no configuration is needed, pass NULL.  */
} PCIE_IntrCfgType;
/** @} */

/** @brief PCIe Page Config

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_BARMapConfigType {
    uint32_t offset;                /**< @brief PCIe BAR offset */
    uint32_t mapAddr;               /**< @brief Internal address map for the segment */
    uint32_t size;                  /**< @brief Size of this mapping */
    PCIE_MemAttribTypes memAttrib;  /**< @brief Memory attributes */
} PCIE_BARMapConfigType;

/** @brief PCIe BAR Space configuration

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_BARConfigType {
    uint32_t numMaps;                    /**< @brief Number of mappings */
    PCIE_BARMapConfigType const *mapCfg; /**< @brief Mapping config */
} PCIE_BARConfigType;

/** @brief PCIe Endpoint device configuration

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_EpConfigType {
    uint16_t deviceId;                                  /**< @brief PCIe Device ID */
    PCIE_BARConfigType barConfig[PCIE_BAR_NUM_MAX+1UL]; /**< @brief All BAR Configs */
} PCIE_EpConfigType;

/** @brief PCIe RC Page Config

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_RcMapConfigType {
    uint32_t offset;                /**< @brief PCIe RC internal memory map offset (optional). */
    uint32_t mapAddr;               /**< @brief Internal address map for the segment, local memory map region used for address translation.  */
    uint32_t size;                  /**< @brief Size of this mapping. */
    PCIE_MemAttribTypes memAttrib;  /**< @brief Memory attributes. */
} PCIE_RcMapConfigType;

/** @brief PCIe RC Space configuration

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_RcSpaceConfigType {
    uint32_t numMaps;                      /**< @brief Number of mappings */
    uint32_t baseAddress;           /**< @brief Base address. It is start address to be matched to incoming addresses. */
    PCIE_RcMapConfigType const *rcMapCfg;  /**< @brief Mapping config */
} PCIE_RcSpaceConfigType;

/** @brief PCIe Root Complex configuration

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_RcConfigType {
    PCIE_RcSpaceConfigType rcSpaceConfig[PCIE_RC_SPACE_NUM_MAX+1UL]; /**< @brief All RC space configs */
    PCIE_BARConfigType barConfig[PCIE_BAR_NUM_RC_MAX]; /**< @brief RC BAR Configs */
    uint32_t sscEn;                        /**< @brief Enable/Disable LCPLL SSC (spread spectrum clocking), it is disabled by default.*/
} PCIE_RcConfigType;

/** @brief Initializes PCIe Endpoint

    Initializes and configures the PCIe endpoint. It does not
    enable the interface. It needs to be enabled using PCIE_SetInterface() API

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]  hwId     Index of the PCIe controller
    @param[in]  aConfig Endpoint configuration

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             PCIe init was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid
    @retval #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval #BCM_ERR_INVAL_PARAMS   Size specified in the config parameter is invalid
    @retval #BCM_ERR_INVAL_STATE    Driver is already initalized

    @post PCIe driver initialized

    @trace #BRCM_SWREQ_PCIE_DRV

    @limitations None
*/
int32_t PCIE_EpInit(PCIE_HwIDType hwId, const PCIE_EpConfigType * const aConfig);

/* PCIE Root Complex initialization APIs */

/** @brief Initializes PCIe Root Complex

    Initializes and configures the PCIe root complex. It does not
    enable the interface. It needs to be enabled using PCIE_SetInterface() API

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]  hwId     Index of the PCIe controller
    @param[in]  aConfig  Root Complex configuration

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             PCIe init was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid
    @retval #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval #BCM_ERR_INVAL_PARAMS   Size specified in the config parameter is invalid
    @retval #BCM_ERR_INVAL_STATE    Driver is already initalized

    @post PCIe driver initialized

    @trace #BRCM_SWREQ_PCIE_DRV

    @limitations None
*/
int32_t PCIE_RcInit(PCIE_HwIDType hwId, const PCIE_RcConfigType * const aConfig);

/** @brief Enable / Disable PCIe interface

    @behavior Sync, Non re-entrant

    @pre PCIe Driver initalized

    @param[in]  hwId     Index of the PCIe controller
    @param[in]  aEnable TRUE = Enable PCIe interface,
                        FALSE = Disable PCIe interface

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             PCIe init was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid
    @retval #BCM_ERR_INVAL_STATE    Driver is not initialized

    @post PCIe interface enabled / disabled

    @trace #BRCM_SWREQ_PCIE_DRV

    @limitations None
*/
int32_t PCIE_SetInterface(PCIE_HwIDType hwId, uint8_t aEnable);

/** @brief DeInitializes PCIe

    Disables the PCIe Interface and de-initializes the PCIe driver

    @behavior Sync, Non re-entrant

    @pre PCIe Driver is initialized

    @param[in]  hwId     Index of the PCIe controller

    Return values are documented in reverse-chronological order

    @retval     #BCM_ERR_OK             PCIe deinitialization successful
    @retval     #BCM_ERR_INVAL_PARAMS   hwId is not valid
    @retval     #BCM_ERR_INVAL_STATE    Driver is not in initialized state

    @post API to Deinitialize PCIe driver

    @trace #BRCM_SWREQ_PCIE_DRV

    @limitations None

*/
int32_t PCIE_DeInit(PCIE_HwIDType hwId);

/* PCIE Interrupt APIs. */

/** @brief Interrupt configuration.

    This API configures the interrupt config parameters for the selected interrupt type used.

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]  hwId     Index of the PCIe controller
    @param[in]  intrType Select PCIE interrupt type need to be configured.
    @param[in]  aConfig  Interrupt configuration for used interrupt types.

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             API was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid
    @retval #BCM_ERR_INVAL_PARAMS   aConfig is NULL

    @post None

    @trace #BRCM_SWREQ_PCIE_DRV

    @limitations None
*/
int32_t PCIE_InterruptConfigSet(PCIE_HwIDType hwId, PCIE_UseIntrType intrType, PCIE_IntrCfgType *aConfig);

/** @brief Enables/Disables Interrupts.

    This API enables/disables the interrupt for the selected bit pattern mask set
    for the interrupt type.

    @behavior Sync, Non re-entrant

    @pre Interrupt need to be configured using #PCIE_InterruptConfigSet API.

    @param[in]  hwId       Index of the PCIe controller
    @param[in]  enableMask Enable mask is bit pattern for the selected interrupt type.
    @param[in]  aEnable    TRUE = Enable Interrupt,
                           FALSE = Disable Interrupt

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             API was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid

    @post None

    @trace #BRCM_SWREQ_PCIE_DRV

    @limitations None
*/
int32_t PCIE_EnableInterrupts(PCIE_HwIDType hwId, uint32_t enableMask, uint8_t aEnable);

/** @brief Gets Pending Interrupts.

    This API gets the pending interrupts bit pattern mask after reading
    the interrupt status register.

    @behavior Sync, Non re-entrant

    @pre Interrupt need to be configured using #PCIE_InterruptConfigSet API.

    @param[in]  hwId            Index of the PCIe controller
    @param[out] pendingIntrMask Pending interrupt bit pattern mask.

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             API was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid

    @post None

    @trace #BRCM_SWREQ_PCIE_DRV

    @limitations None
*/
int32_t PCIE_GetPendingInterrupts(PCIE_HwIDType hwId, uint32_t *pendingIntrMask);

/**
    @brief Indirect write to PCIE register

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aAddress    Indirect register address
    @param[in]  aMask         Mask bits
    @param[in]  aValue        Value to be written

    @returns void

    @post None

    @limitations None

    @trace #BRCM_SWREQ_PCIE_DRV
*/

void PCIE_DrvIndirectIO(volatile uint32_t * const aRegAddr,
                           uint32_t aMask,
                           uint32_t aValue);

/**
    @brief Indirect read to PCIE register

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aAddress    Indirect register address
    @param[out]  aValue      Read value from register address

    @returns void

    @post None

    @limitations None

    @trace #BRCM_SWREQ_PCIE_DRV
*/

void PCIE_DrvIndirectIORead(volatile uint32_t * const aRegAddr,
                                 uint32_t *aValue);


/** @brief Gets PCIe link status.

    This API gets the PCIe link status.

    @behavior Sync, Non re-entrant

    @pre PCIe is in initialized state.

    @param[in]  hwId           Index of the PCIe controller
    @param[out] alinkStatus PCIe link status 1 : Link Status Up 0 : Link Status Down

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             API was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid

    @post None
    @limitations None
    @trace #BRCM_SWREQ_PCIE_DRV
*/
int32_t PCIE_GetLinkStatus(PCIE_HwIDType hwId, uint32_t *alinkStatus);

/**
    @brief PCIe configuration read.

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aAddress    Configuration register address
    @param[out]  aValue       Value to be read from configuration register space.

    @returns void

    @post None

    @limitations None

    @trace #BRCM_SWREQ_PCIE_DRV
 */
void PCIE_DrvCfgRead(uint32_t aRegAddr,
                          uint32_t *aValue);

/**
    @brief PCIe configuration write.

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aAddress  Configuration register address
    @param[in]  aValue      Write value to configuration address space

    @returns void

    @post None

    @limitations None

    @trace #BRCM_SWREQ_PCIE_DRV
*/
void PCIE_DrvCfgWrite(uint32_t aRegAddr,
                           uint32_t aValue);

/** @brief Get PCIe PERST status.

    This API Wait for PERST signal in EP or RC mode as per configuration.

    @behavior Sync, Non re-entrant

    @pre PCIe is in initialized state.

    @param[in]  hwId           Index of the PCIe controller
    @param[out] aPerstStatus PCIe PERST status 1 : PERST Asserted 0 : PERST Not Asserted

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             API was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid

    @post None
    @limitations None
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    Get the PCIe PERST status.
    @endcode

*/
int32_t PCIE_GetPcieRstStatus(PCIE_HwIDType hwId, uint32_t *aPerstStatus);

#endif /* PCIEDRV_H */

/** @} */
