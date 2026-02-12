/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_pciedrv_imp PCIe Driver Design
    @ingroup grp_pciedrv

    @addtogroup grp_pciedrv_imp
    @{

    @file drivers/pcie/pcie_drv.c
    @brief PCIE Driver Design and implementation

    @version 0.1 Initial version
*/

#include <bcm_time.h>
#include <bcm_utils.h>
#include <hsio_common_rdb.h>
#include "rdb/pcie_common_rdb.h"
#include "rdb/imap_ctrl_rdb.h"
#include "rdb/imap1_rdb.h"
#include "rdb/cfg_priv0_rdb.h"
#include "rdb/cfg_priv1_rdb.h"
#include <hsafe_rdb.h>
#include "rdb/cfg_aer_rdb.h"
#include <pcie.h>


/**
    @name PCIe Driver Design IDs
    @{
    @brief Design IDs for PCIe Driver
*/
#define BRCM_SWDSGN_PCIE_REG_MACRO                              (0xB100U) /**< @brief #PCIE_REG_HSIO_COM              */
#define BRCM_SWDSGN_PCIE_BAR_SIZE_TYPE                          (0xB101U) /**< @brief #PCIE_BARSizeType               */
#define BRCM_SWDSGN_PCIE_MIN_PAGE_SIZE_MACRO                    (0xB102U) /**< @brief #PCIE_MIN_PAGE_SIZE             */
#define BRCM_SWDSGN_PCIE_NUM_PAGES_IN_BAR_MACRO                 (0xB103U) /**< @brief #PCIE_NUM_PAGES_IN_BAR          */
#define BRCM_SWDSGN_PCIE_BAR_SIZE2BYTES_MACRO                   (0xB104U) /**< @brief #PCIE_BAR_SIZE2BYTES            */
#define BRCM_SWDSGN_PCIE_DRVSTATE_TYPE                          (0xB105U) /**< @brief #PCIE_DrvStateType              */

#define BRCM_SWDSGN_PCIE_DRV_RWDEV_TYPE                         (0xB107U) /**< @brief #PCIE_DrvRWDevType              */
#define BRCM_SWDSGN_PCIE_CONTROLLER_MODE_TYPE                   (0xB108U) /**< @brief #PCIE_ControllerModeType        */
#define BRCM_SWDSGN_PCIE_MEM_SIZE2_BARSIZE_GLOBAL               (0xB109U) /**< @brief #PCIE_MemSize2BARSize           */
#define BRCM_SWDSGN_PCIE_DRV_RWDEV_GLOBAL                       (0xB10AU) /**< @brief #PCIE_DrvRWDev                  */

#define BRCM_SWDSGN_PCIE_INTC_REMAP_BITPOS_REG0_GLOBAL          (0xB10BU) /**< @brief #PCIE_INTC_REMAP_BITPOS_REG0    */
#define BRCM_SWDSGN_PCIE_INTC_INTR_ENABLE_REG0_GLOBAL           (0xB10CU) /**< @brief #PCIE_INTC_INTR_ENABLE_REG0    */

#define BRCM_SWDSGN_PCIE_INTC_INTR_CLR_MODE_0_GLOBAL            (0xB10EU) /**< @brief #PCIE_INTC_INTR_CLR_MODE_0    */
#define BRCM_SWDSGN_PCIE_INTC_INTR_STATUS_GLOBAL                (0xB10FU) /**< @brief #PCIE_INTC_INTR_STATUS    */
#define BRCM_SWDSGN_PCIE_SYS_EQ_PAGE_ADDRL_GLOBAL               (0xB111U) /**< @brief #PCIE_SYS_EQ_PAGE_ADDRL    */
#define BRCM_SWDSGN_PCIE_SYS_MSI_PAGE_ADDRL_GLOBAL              (0xB112U) /**< @brief #PCIE_SYS_MSI_PAGE_ADDRL    */
#define BRCM_SWDSGN_PCIE_SYS_EQ_PAGE_ADDRH_GLOBAL               (0xB113U) /**< @brief #PCIE_SYS_EQ_PAGE_ADDRH    */
#define BRCM_SWDSGN_PCIE_SYS_MSI_PAGE_ADDRH_GLOBAL              (0xB114U) /**< @brief #PCIE_SYS_MSI_PAGE_ADDRH    */
#define BRCM_SWDSGN_PCIE_SYS_MSI_EQ_CTRL_0_GLOBAL               (0xB116U) /**< @brief #PCIE_SYS_MSI_EQ_CTRL_0    */
#define BRCM_SWDSGN_PCIE_PERST_SOURCE_SELECT_REG_GLOBAL         (0xB117U) /**< @brief #PCIE_PERST_SOURCE_SELECT_REG */
#define BRCM_SWDSGN_PCIE_RC_SPACE_REG_GLOBAL                    (0xB118U) /**< @brief #PCIE_RC_SPACE_REG    */
#define BRCM_SWDSGN_PCIE_LINK_REG_GLOBAL                        (0xB119U) /**< @brief #PCIE_LinkReg    */
#define BRCM_SWDSGN_PCIE_BAR12_VALID_SIZE_GLOBAL                (0xB11AU) /**< @brief #PCIE_BAR12ValidSize    */
#define BRCM_SWDSGN_PCIE_BAR3_VALID_SIZE_GLOBAL                 (0xB11BU) /**< @brief #PCIE_BAR3ValidSize    */
#define BRCM_SWDSGN_PCIE_BAR_VALID_SIZE_GLOBAL                  (0xB11CU) /**< @brief #PCIE_BARValidSize    */
#define BRCM_SWDSGN_PCIE_IMAP_SELECT_GLOBAL                     (0xB11DU) /**< @brief #PCIE_IMapSelect    */
#define BRCM_SWDSGN_PCIE_BAR_CONFIG_REG_GLOBAL                  (0xB11EU) /**< @brief #PCIE_BARConfigReg    */
#define BRCM_SWDSGN_PCIE_IMAP_CTL_REG_GLOBAL                    (0xB11FU) /**< @brief #PCIE_IMAPCtlReg    */
#define BRCM_SWDSGN_PCIE_DRV_SET_IMAP_VALUE_PROC                (0xB121U) /**< @brief #PCIE_DrvSetImapValue            */
#define BRCM_SWDSGN_PCIE_DRV_FIND_BARSIZE_PROC                  (0xB122U) /**< @brief #PCIE_DrvFindBARSize            */
#define BRCM_SWDSGN_PCIE_DRV_VALIDATE_BARCFG_PROC               (0xB123U) /**< @brief #PCIE_DrvValidateBARCfg         */
#define BRCM_SWDSGN_PCIE_DRV_BARCONFIG_PROC                     (0xB124U) /**< @brief #PCIE_DrvBARConfig            */
#define BRCM_SWDSGN_PCIE_DEF_SUB_SYS_VENDOR_ID_MACRO            (0xB125U) /**< @brief #PCIE_DEF_SUB_SYS_VENDOR_ID     */
#define BRCM_SWDSGN_PCIE_SET_DEVICE_ID_PROC                     (0xB126U) /**< @brief #PCIE_DrvSetDeviceId            */
#define BRCM_SWDSGN_PCIE_DRV_INDIRECT_IO_RROC                   (0xB127U) /**< @brief #PCIE_DrvIndirectIO             */
#define BRCM_SWDSGN_PCIE_DRV_INDIRECT_IOREAD_RROC               (0xB128U) /**< @brief #PCIE_DrvIndirectIORead        */
#define BRCM_SWDSGN_PCIE_EP_INIT_PROC                           (0xB129U) /**< @brief #PCIE_EpInit               */
#define BRCM_SWDSGN_PCIE_SET_INTERFACE_PROC                     (0xB12AU) /**< @brief #PCIE_SetInterface         */
#define BRCM_SWDSGN_PCIE_DE_INIT_PROC                           (0xB12BU) /**< @brief #PCIE_DeInit               */
#define BRCM_SWDSGN_PCIE_RC_INIT_PROC                           (0xB12CU) /**< @brief #PCIE_RcInit               */
#define BRCM_SWDSGN_PCIE_INTERRUPT_CONFIG_SET_PROC              (0xB12DU) /**< @brief #PCIE_InterruptConfigSet   */
#define BRCM_SWDSGN_PCIE_ENABLE_INTERRUPTS_PROC                 (0xB12EU) /**< @brief #PCIE_EnableInterrupts     */
#define BRCM_SWDSGN_PCIE_GET_PENDING_INTERRUPTS_PROC            (0xB12FU) /**< @brief #PCIE_GetPendingInterrupts */
#define BRCM_SWDSGN_PCIE_DRV_GET_PAGE_CONFIG_PROC               (0xB131U) /**< @brief #PCIE_DrvGetPageConfig */
#define BRCM_SWDSGN_PCIE_INTR_REMAP_BITPOS_REG_TYPE             (0xB132U) /**< @brief #PCIE_INTR_REMAP_BITPOS_REG_TYPE  */
#define BRCM_SWDSGN_PCIE_SYS_MSI_CTRL_TYPE                      (0xB133U) /**< @brief #PCIE_SYS_MSI_CTRL_TYPE            */
#define BRCM_SWDSGN_PCIE_INT_DRV_RC_CONFIG_PROC                 (0xB134U) /**< @brief #PCIE_IntDrvRCConfig */
#define BRCM_SWDSGN_PCIE_INT_DRV_VALIDATE_RC_CFG_PROC           (0xB135U) /**< @brief #PCIE_IntDrvValidateRCCfg */
#define BRCM_SWDSGN_PCIE_DRV_CFG_READ_PROC                      (0xB136U) /**< @brief #PCIE_DrvCfgRead      */
#define BRCM_SWDSGN_PCIE_DRV_CFG_WRITE_PROC                     (0xB137U) /**< @brief #PCIE_DrvCfgWrite      */
#define BRCM_SWDSGN_PCIE_GET_PCIE_RST_STATUS_PROC               (0xB138U) /**< @brief #PCIE_GetPcieRstStatus      */
#define BRCM_SWDSGN_PCIE_GET_LINK_STATUS_PROC                   (0xB139U) /**< @brief #PCIE_GetLinkStatus */
#define BRCM_SWDSGN_PCIE_CFG_ADDR_GLOBAL                        (0xB13AU) /**< @brief #PCIE_CfgAddr    */
#define BRCM_SWDSGN_PCIE_CFG_DATA_GLOBAL                        (0xB13BU) /**< @brief #PCIE_CfgData    */
#define BRCM_SWDSGN_PCIE_INT_DRV_VALIDATE_BARMAPCONFIG_PROC     (0xB13CU) /**< @brief #PCIE_IntDrvValidateBARMapConfig */
#define BRCM_SWDSGN_PCIE_INT_DRV_VALIDATE_RCMAPCONFIG_PROC      (0xB13DU) /**< @brief #PCIE_IntDrvValidateRCMapConfig */
#define BRCM_SWDSGN_PCIE_SYS_RC_INTX_EN_GLOBAL                  (0xB13EU) /**< @brief #PCIE_SYS_RC_INTX_EN */

/** @} */

/**
    @name PCIe Registers
    @{
    @brief PCIe HW Block Register sets

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_SET_INTERFACE_PROC
    @trace #BRCM_SWARCH_PCIE_DE_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PCIE_ENABLE_INTERRUPTS_PROC
    @trace #BRCM_SWARCH_PCIE_GET_PENDING_INTERRUPTS_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
#define PCIE_REG_HSIO_COM           ((HSIO_COMMON_RDBType   *)HSIO_COMMON_BASE)
#define PCIE_REG_EP_COMMON          ((PCIE_COMMON_RDBType   *)EPCOMMON_BASE)
#define PCIE_REG_IMAP_CTL           ((IMAP_CTRL_RDBType     *)IMAP_CTRL_BASE)
#define PCIE_REG_IMAP1              ((IMAP1_RDBType         *)IMAP1_BASE)
#define PCIE_REG_CFG_PRIV0          ((CFG_PRIV0_RDBType     *)CFG_PRIV0_BASE)
#define PCIE_REG_CFG_PRIV1          ((CFG_PRIV1_RDBType     *)CFG_PRIV1_BASE)
#define PCIE_REG_HSAFE              ((HSAFE_RDBType         *)HSAFE_BASE)
#define PCIE_REG_CFG_AER            ((CFG_AER_RDBType       *)CFG_AER0_BASE)
/** @} */

/**
    @brief PCIE input interrupt remapping register.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
*/
static uint32_t *PCIE_INTC_REMAP_BITPOS_REG0 = (uint32_t *) (0xE02d3000UL);
/**
    @brief PCIE input interrupt enable register.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
*/
static uint32_t * const PCIE_INTC_INTR_ENABLE_REG0 = (uint32_t *) (0xE02d30F0UL);

/**
    @brief PCIE input interrupt clear mode register.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
*/
static uint32_t *const PCIE_INTC_INTR_CLR_MODE_0 = (uint32_t *)(0xE02d23A8UL);
/**
    @brief PCIE input interrupt status INTC_INTR_STATUS_REG0 register.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
*/
static volatile uint32_t * const PCIE_INTC_INTR_STATUS = (uint32_t *) (0xE02d3190UL);

/**
    @brief PCIE MSI event queue page address.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
*/
static uint32_t * const PCIE_SYS_EQ_PAGE_ADDRL = (uint32_t *)(0xE02d2200UL);
/**
    @brief PCIE MSI message page address low.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
*/
static uint32_t * const PCIE_SYS_MSI_PAGE_ADDRL = (uint32_t *)(0xE02d2204UL);
/**
    @brief PCIE MSI event queue page address high.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
*/
static uint32_t * const PCIE_SYS_EQ_PAGE_ADDRH = (uint32_t *)(0xE02d22C0UL);

/**
    @brief PCIE MSI message page address high.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
*/
static uint32_t * const PCIE_SYS_MSI_PAGE_ADDRH = (uint32_t *)(0xE02d22C4UL);

/**
    @brief PCIE MSI event queue control register[0-5].

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
*/
static uint32_t *PCIE_SYS_MSI_EQ_CTRL_0 = (uint32_t *)(0xE02d2210UL);

/**
    @brief Enable PCIe INTX interrupts in RC mode [0-3].

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
*/
static volatile uint32_t * const PCIE_SYS_RC_INTX_EN = (uint32_t *)(0xE02d2330UL);


/**
    @brief PCIE PERST source select De-aasert rc-perst_b register.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
*/
static volatile uint32_t * const PCIE_PERST_SOURCE_SELECT_REG = (uint32_t *)(0xE02d2000UL);

/**
    @brief PCIE RC space. Program upto 3 start addresses to be matched to incoming addresses.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
*/
static volatile uint32_t * const PCIE_RC_SPACE_REG[PCIE_RC_SPACE_NUM_MAX + 1UL] = {(uint32_t *)(0xE02d1DD0UL),
                                                                          (uint32_t *)(0xE02d1DD4UL),
                                                                          (uint32_t *)(0xE02d1DD8UL)};

/**
    @brief PCIE Link register.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_GET_LINK_STATUS_PROC
*/
static volatile uint32_t *PCIE_LinkReg = (uint32_t *)(0xE02d2F0CUL);

/**
    @brief PCIE config address.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_DRV_CFG_READ_PROC
    @trace #BRCM_SWARCH_PCIE_DRV_CFG_WRITE_PROC
*/
static volatile uint32_t *PCIE_CfgAddr = (uint32_t *)(0xE02d21F8UL);

/**
    @brief PCIE config data register.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_DRV_CFG_READ_PROC
    @trace #BRCM_SWARCH_PCIE_DRV_CFG_WRITE_PROC
 */
static volatile uint32_t *PCIE_CfgData = (uint32_t *)(0xE02d21FCUL);

/**
    @name PCIe MSI control register
    @{
    @brief PCIe MSI control register settings values

    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PCIE_ENABLE_INTERRUPTS_PROC
    @trace #BRCM_SWARCH_PCIE_GET_PENDING_INTERRUPTS_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_INTR_REMAP_BITPOS_REG_TYPE;
#define PCIE_INTR_REMAP_BITPOS_REG4_SHIFT (24UL)  /**< @brief Interrupt4 input shift value. */
#define PCIE_INTR_REMAP_BITPOS_REG3_SHIFT (18UL)  /**< @brief Interrupt3 input shift value. */
#define PCIE_INTR_REMAP_BITPOS_REG2_SHIFT (12UL)  /**< @brief Interrupt2 input shift value. */
#define PCIE_INTR_REMAP_BITPOS_REG1_SHIFT (6UL)   /**< @brief Interrupt1 input shift value. */
#define PCIE_INTR_REMAP_BITPOS_REG0_SHIFT (0UL)   /**< @brief Interrupt0 input shift value. */
#define PCIE_INTR_REMAP_BITPOS_REG_MASK (0x3FUL)  /**< @brief Interrupt remap value to map input interrupt to 0-63 output values for MSI/MSIx generated output. */
/** @} */

/**
    @name PCIe MSI control register
    @{
    @brief PCIe MSI control register settings values

    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PCIE_ENABLE_INTERRUPTS_PROC
    @trace #BRCM_SWARCH_PCIE_GET_PENDING_INTERRUPTS_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_SYS_MSI_CTRL_TYPE;
#define PCIE_SYS_MSI_CTRL_MSI_INTR_EN_EQ_MASK (0x1UL) /**< @brief MSI interrupt enable for selected event Queue. */
#define PCIE_SYS_MSI_CTRL_MSI_INTR_EN_EQ_SHIFT (11UL)
#define PCIE_SYS_MSI_CTRL_INTR_NDELAY_MASK (0xFUL)    /**< @brief Generate interrupt after 2^intrNDelay, recommended value 2 or 3.*/
#define PCIE_SYS_MSI_CTRL_INTR_NDELAY_SHIFT (7UL)
#define PCIE_SYS_MSI_CTRL_INTR_NEVENT_MASK (0x3FUL)   /**< @brief The number of MSI messages must be received before interrupt is generated. */
#define PCIE_SYS_MSI_CTRL_INTR_NEVENT_SHIFT (1UL)
#define PCIE_SYS_MSI_CTRL_EQ_EN_MASK (0x1UL)          /**< @brief Event Queue enable */
#define PCIE_SYS_MSI_CTRL_EQ_EN_SHIFT (0UL)
/** @} */

/**
    @brief PCIe driver state type

    @{
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_SET_INTERFACE_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_DrvStateType;
#define PCIE_DRVSTATE_UNINIT         (0UL) /**< @brief PCIe driver state - Uninitialised */
#define PCIE_DRVSTATE_INIT           (1UL) /**< @brief PCIe driver state - Initialised */
/** @} */

/**
    @brief PCIe Controller mode type

    @{
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_SET_INTERFACE_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_ControllerModeType;
#define PCIE_CONTROLLER_MODE_EP         (0UL) /**< @brief PCIe Controller mode - EP (Endpoint) */
#define PCIE_CONTROLLER_MODE_RC         (1UL) /**< @brief PCIe Controller mode - RC (Root Complex) */
/** @} */


/**
    @brief PCIe driver RW device type

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_SET_INTERFACE_PROC

    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef struct sPCIE_DrvRWDevType {
    PCIE_DrvStateType            state;   /**< @brief Current controller state */
    PCIE_ControllerModeType      controllerMode; /**< Configured PCIe controller mode either EP/RC mode */
} PCIE_DrvRWDevType;

/**
    @brief PCIe driver RW Device

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_SET_INTERFACE_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
static PCIE_DrvRWDevType PCIE_DrvRWDev[PCIE_HW_ID_MAX] COMP_SECTION(".bss.drivers");

/** @brief Minimum BAR page size

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
#define PCIE_MIN_PAGE_SIZE          (4UL*1024UL)

/** @brief Number of pages in a BAR

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
#define PCIE_NUM_PAGES_IN_BAR       (IMAP1_PG0_SIZE)

/** @brief Default Subsystem Vendor ID

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
#define PCIE_DEF_SUB_SYS_VENDOR_ID     (0xA00FUL)

/** @brief Convert BAR Size to actual size

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
#define PCIE_BAR_SIZE2BYTES(aBARSize)         ((32UL*1024UL)<<(aBARSize))

/**
    @name PCIe BAR Sizes
    @{
    @brief PCIe BAR Sizes used by the HW

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
typedef uint32_t PCIE_BARSizeType;
#define PCIE_BAR_SIZE_NONE          (0UL)
#define PCIE_BAR_SIZE_64K           (1UL)
#define PCIE_BAR_SIZE_128K          (2UL)
#define PCIE_BAR_SIZE_256K          (3UL)
#define PCIE_BAR_SIZE_512K          (4UL)
#define PCIE_BAR_SIZE_1M            (5UL)
#define PCIE_BAR_SIZE_2M            (6UL)
#define PCIE_BAR_SIZE_4M            (7UL)
#define PCIE_BAR_SIZE_8M            (8UL)
#define PCIE_BAR_SIZE_16M           (9UL)
#define PCIE_BAR_SIZE_32M           (10UL)
#define PCIE_BAR_SIZE_64M           (11UL)
#define PCIE_BAR_SIZE_128M          (12UL)
#define PCIE_BAR_SIZE_256M          (13UL)
#define PCIE_BAR_SIZE_512M          (14UL)
#define PCIE_BAR_SIZE_1G            (15UL)
/** @} */

/**
    @brief Mapping table for PCIe size to BAR size used by hardware

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
static const uint32_t PCIE_MemSize2BARSize[PCIE_MEM_SIZE_MAX+1UL] = {
    [PCIE_MEM_SIZE_1K   ] =  PCIE_BAR_SIZE_NONE,
    [PCIE_MEM_SIZE_2K   ] =  PCIE_BAR_SIZE_NONE,
    [PCIE_MEM_SIZE_4K   ] =  PCIE_BAR_SIZE_NONE,
    [PCIE_MEM_SIZE_8K   ] =  PCIE_BAR_SIZE_NONE,
    [PCIE_MEM_SIZE_16K  ] =  PCIE_BAR_SIZE_NONE,
    [PCIE_MEM_SIZE_32K  ] =  PCIE_BAR_SIZE_NONE,
    [PCIE_MEM_SIZE_64K  ] =  PCIE_BAR_SIZE_64K,
    [PCIE_MEM_SIZE_128K ] =  PCIE_BAR_SIZE_128K,
    [PCIE_MEM_SIZE_256K ] =  PCIE_BAR_SIZE_256K,
    [PCIE_MEM_SIZE_512K ] =  PCIE_BAR_SIZE_512K,
    [PCIE_MEM_SIZE_1M   ] =  PCIE_BAR_SIZE_1M,
    [PCIE_MEM_SIZE_2M   ] =  PCIE_BAR_SIZE_2M,
    [PCIE_MEM_SIZE_4M   ] =  PCIE_BAR_SIZE_4M,
    [PCIE_MEM_SIZE_8M   ] =  PCIE_BAR_SIZE_8M,
    [PCIE_MEM_SIZE_16M  ] =  PCIE_BAR_SIZE_16M,
    [PCIE_MEM_SIZE_32M  ] =  PCIE_BAR_SIZE_32M,
    [PCIE_MEM_SIZE_64M  ] =  PCIE_BAR_SIZE_64M,
    [PCIE_MEM_SIZE_128M ] =  PCIE_BAR_SIZE_128M,
    [PCIE_MEM_SIZE_256M ] =  PCIE_BAR_SIZE_256M,
    [PCIE_MEM_SIZE_512M ] =  PCIE_BAR_SIZE_512M,
    [PCIE_MEM_SIZE_1G   ] =  PCIE_BAR_SIZE_1G,
};

/**
    @brief Supported sizes for BAR1 & BAR2.

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
static const uint32_t PCIE_BAR12ValidSize[] = {
    PCIE_BAR_SIZE_256K,
    PCIE_BAR_SIZE_512K,
    PCIE_BAR_SIZE_1M,
    PCIE_BAR_SIZE_2M,
    PCIE_BAR_SIZE_4M,
    PCIE_BAR_SIZE_8M,
    PCIE_BAR_SIZE_256M,
    PCIE_BAR_SIZE_512M,
    PCIE_BAR_SIZE_1G,
    PCIE_BAR_SIZE_NONE, /* End of table */
};

/**
    @brief Supported sizes for BAR3.

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
static const uint32_t PCIE_BAR3ValidSize[] = {
    PCIE_BAR_SIZE_256M,
    PCIE_BAR_SIZE_512M,
    PCIE_BAR_SIZE_1G,
    PCIE_BAR_SIZE_NONE, /* End of table */
};

/**
    @brief Supported sizes for each BAR.

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
static const uint32_t * const PCIE_BARValidSize[] = {
    PCIE_BAR12ValidSize, PCIE_BAR12ValidSize, PCIE_BAR3ValidSize
};

/**
    @brief IMAP page select.

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
static const uint32_t PCIE_IMapSelect[] = {
    HSIO_COMMON_PAGE_SELECT_IMAP0,
    HSIO_COMMON_PAGE_SELECT_IMAP1,
    HSIO_COMMON_PAGE_SELECT_IMAP2,
};

/**
    @brief BAR config register.

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
static volatile uint32_t * const PCIE_BARConfigReg[] = {
    &PCIE_REG_CFG_PRIV0->config_2,  /* !! only for configuring BAR1 Size and enable.
                                          Other fields do not match the below register !! */
    &PCIE_REG_CFG_PRIV1->bar2_config,
    &PCIE_REG_CFG_PRIV1->bar3_config,
};

/**
    @brief IMAP control register.

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
*/
static volatile uint32_t * const PCIE_IMAPCtlReg[] = {
    &PCIE_REG_IMAP_CTL->imap0,
    &PCIE_REG_IMAP_CTL->imap1,
    &PCIE_REG_IMAP_CTL->imap2
};

/**
    @brief Set IMAP value

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]     imapVal IMAP value

    Return values are documented in reverse-chronological order

    @returns void

    @post Subsytem Vendor id is setup

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    Set IMAP page select value
    @endcode
*/
static void PCIE_DrvSetImapValue(const uint32_t imapVal)
{
    /* Select IMAP for the given imap value */
    uint32_t pcie_ctrl_2 = PCIE_REG_HSIO_COM->pcie_ctrl_2;
    pcie_ctrl_2 &= ~HSIO_COMMON_PCIE_CTRL_32BIT_2_IMAP_PAGE_SELECT_MASK;
    pcie_ctrl_2 |= (imapVal << HSIO_COMMON_PCIE_CTRL_32BIT_2_IMAP_PAGE_SELECT_SHIFT);
    PCIE_REG_HSIO_COM->pcie_ctrl_2 = pcie_ctrl_2;
    BCM_DelayUs(100UL);
}

/**
    @brief Set Subsystem Vendor ID

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]     aDevId  Device id

    Return values are documented in reverse-chronological order

    @returns void

    @post Subsytem Vendor id is setup

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    update the device id to the USER_DEVICE_ID field on hsio common pcie_ctrl_o register
    @endcode
*/
static void PCIE_DrvSetDeviceId(uint32_t aDevId)
{
    uint32_t ctrl0;
    /* Set vendor ID */
    ctrl0 = PCIE_REG_HSIO_COM->pcie_ctrl_0;
    ctrl0 &= ~HSIO_COMMON_PCIE_CTRL_32BIT_0_STRAP_USER_DEVICE_ID_MASK;
    ctrl0 |= aDevId & HSIO_COMMON_PCIE_CTRL_32BIT_0_STRAP_USER_DEVICE_ID_MASK;
    PCIE_REG_HSIO_COM->pcie_ctrl_0 = ctrl0;
}

/**
    @brief Convert normal size to PCIe BAR size enumeration

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]     aActSize Actual size in bytes
    @param[out]    aBARSize PCIe memory size enumeration

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             Size converted successfully
    @retval #BCM_ERR_INVAL_PARAMS   Given size is not supported

    @post None

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    next2Pow = 1024
    memSize = 1K
    while next2Pow < aActSize:
        next2Pow *= 2
        memSize++
    if memSize > PCIE_MEM_SIZE_1G
        return BCM_ERR_INVAL_PARAMS

    find the smallest supported BAR size >= memSize
        set it to aBARSize

    @endcode
*/
static int32_t PCIE_DrvFindBARSize(uint32_t aActSize, PCIE_BARSizeType * const aBARSize)
{
    uint32_t next2Pow = 1024UL;
    PCIE_MemSizeType memSize = PCIE_MEM_SIZE_1K;
    int32_t ret = BCM_ERR_OK;

    while (next2Pow < aActSize) {
        if (memSize > PCIE_MEM_SIZE_1G) {
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
        next2Pow <<= 1;
        memSize++;
    }

    if (BCM_ERR_OK == ret) {
        uint32_t i;
        /* BCM_TEST_ADD_PROBE(BCM_PCIE_ID, PCIEDRV_PTSEQ_EP_BAR_CONFIG,
                              PCIEDRV_PTST_DRV_FIND_MEM_SIZE_EXCEED,
                              memSize += 20UL) */
        /* Find the first supported BAR size bigger than the required */
        for (i = memSize; i < BCM_ARRAY_LEN(PCIE_MemSize2BARSize); i++) {
            if (PCIE_MemSize2BARSize[i] != PCIE_BAR_SIZE_NONE) {
                *aBARSize = PCIE_MemSize2BARSize[i];
                ret = BCM_ERR_OK;
                break;
            }
        }
    }

    return ret;
}

/**
    @brief Valid BAR config

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aBARCfg    BAR Configuration
    @param[out]  aBARSize   BAR Size value in enumeration

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             BAR configuration is valid
    @retval #BCM_ERR_INVAL_PARAMS   Given BAR configuration is not valid

    @post None

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    if number of maps more than max
        return BCM_ERR_INVAL_PARAM

    if any of the map address is not aligned with minimum page size
        return BCM_ERR_INVAL_PARAM

    if map offsets are not in ascending order
        return BCM_ERR_INVAL_PARAM

    totalSize = lastMapOffset + lastMapSize

    @endcode
*/
static int32_t PCIE_DrvValidateBARCfg(PCIE_BARConfigType const * const aBARCfg,
                                      PCIE_MemSizeType * const aBARSize)
{
    uint32_t i;
    int32_t ret = BCM_ERR_OK;
    uint32_t totalSize = 0UL;

    if (aBARCfg->numMaps > PCIE_NUM_PAGES_IN_BAR) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        for (i = 0UL; i < aBARCfg->numMaps; i++) {
            /* internal address map not aligned */
            if (0UL != (aBARCfg->mapCfg[i].mapAddr & (PCIE_MIN_PAGE_SIZE-1UL))) {
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
            /* offsets should be in ascending order */
            if (aBARCfg->mapCfg[i].offset < totalSize) {
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
            totalSize = aBARCfg->mapCfg[i].offset + aBARCfg->mapCfg[i].size;
        }
    }

    if (BCM_ERR_OK == ret) {
        ret = PCIE_DrvFindBARSize(totalSize, aBARSize);
    }

    return ret;
}

/**
    @brief Valid RC config

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aRCCfg      RC Configuration
    @param[out]  aBARSize   BAR/RC Size value in enumeration

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             RC configuration is valid
    @retval #BCM_ERR_INVAL_PARAMS   Given RC configuration is not valid

    @post None

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    if number of maps more than max
        return BCM_ERR_INVAL_PARAM

    if any of the map address is not aligned with minimum page size
        return BCM_ERR_INVAL_PARAM

    if map offsets are not in ascending order
        return BCM_ERR_INVAL_PARAM

    totalSize = lastMapOffset + lastMapSize

    @endcode
*/
static int32_t PCIE_IntDrvValidateRCCfg(PCIE_RcSpaceConfigType const * const aRCCfg,
                                            PCIE_MemSizeType * const aBARSize)
{
    uint32_t i;
    int32_t ret = BCM_ERR_OK;
    uint32_t totalSize = 0UL;

    if (aRCCfg->numMaps > PCIE_NUM_PAGES_IN_BAR) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        for (i = 0UL; i < aRCCfg->numMaps; i++) {
            /* internal address map not aligned */
            if (0UL != (aRCCfg->rcMapCfg[i].mapAddr & (PCIE_MIN_PAGE_SIZE-1UL))) {
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
            /* offsets should be in ascending order */
            if (aRCCfg->rcMapCfg[i].offset < totalSize) {
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
            totalSize = aRCCfg->rcMapCfg[i].offset + aRCCfg->rcMapCfg[i].size;
        }
    }

    if (BCM_ERR_OK == ret) {
        ret = PCIE_DrvFindBARSize(totalSize, aBARSize);
    }

    return ret;
}


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

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    Set the address in the indirect block
    read the data from indirect block
    Add the fields to be updated in the read data and writ it to the
        indirect data register
    @endcode
*/
void PCIE_DrvIndirectIO(volatile uint32_t * const aRegAddr,
                                   uint32_t aMask,
                                   uint32_t aValue)
{
    uintptr_t regAddr = (uintptr_t)aRegAddr;
    uint32_t orgVal = 0UL;
    uint32_t mask = (((uint32_t)regAddr & 0x1000UL) > 0UL) ? 0xFFFFUL : 0xFFFUL;

    /* Indirect address - only the offset need to be specified */
    PCIE_REG_CFG_AER->config_ind_addr = ((uint32_t)regAddr & mask);
    /* Indirect data */
    orgVal = PCIE_REG_CFG_AER->config_ind_data;
    PCIE_REG_CFG_AER->config_ind_data = (aValue & aMask) | (orgVal & ~aMask);
    /* Wait for indirect write to complete. */
    BCM_DelayUs(15UL);
}

/**
    @brief Indirect read to PCIE register

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aAddress    Indirect register address
    @param[out]  aValue      Read value from register address

    @returns void

    @post None

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    Set the address in the indirect block
    read the data from indirect block
    @endcode
*/
void PCIE_DrvIndirectIORead(volatile uint32_t * const aRegAddr,
                                   uint32_t *aValue)
{
    uintptr_t regAddr = (uintptr_t)aRegAddr;
    uint32_t orgVal = 0UL;
    uint32_t mask = (((uint32_t)regAddr & 0x1000UL) > 0UL) ? 0xFFFFUL : 0xFFFUL;
    /* Indirect address - only the offset need to be specified */
    PCIE_REG_CFG_AER->config_ind_addr = ((uint32_t)regAddr & mask);
    /* Wait for indirect read to complete. */
    BCM_DelayUs(10UL);
    /* Indirect data read */
    orgVal = PCIE_REG_CFG_AER->config_ind_data;
    *aValue = orgVal;
}


/**
    @brief Gets IMAP page attributes from BAR configuration.

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   memAttrib    Memory attribute
    @param[out]  pageCfg  Page attributes.

    @returns void

    @post None

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
       Get Page configuration attribute.
    @endcode
*/
static void PCIE_DrvGetPageConfig(PCIE_MemAttribTypes memAttrib,
                                       uint32_t *pageCfg)
{
    if (0UL != (memAttrib & PCIE_MEM_ATTRIB_READ)) {
        *pageCfg |= IMAP1_PAGE_READ_EN;
    }
    if (0UL != (memAttrib & PCIE_MEM_ATTRIB_WRITE)) {
        *pageCfg |= IMAP1_PAGE_WRITE_EN;
    }
    if (0UL != (memAttrib & PCIE_MEM_ATTRIB_WR_CACHE)) {
        *pageCfg |= IMAP1_PAGE_WCACHE_EN;
    }
    if (0UL != (memAttrib & PCIE_MEM_ATTRIB_RD_CACHE)) {
        *pageCfg |= IMAP1_PAGE_RCACHE_EN;
    }
}

/**
    @brief Validate EP page map address.
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
       Validate page map address
    @endcode
*/
static int32_t PCIE_IntDrvValidateBARMapConfig(PCIE_BARMapConfigType const *mapCfg, const PCIE_BARConfigType * const aBARCfg, uint32_t pageSizeMask)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t idx = 0UL;
    for (idx = 0UL; idx < aBARCfg->numMaps; idx++) {
        /* If any of the map segment offset is not multiple of page size */
        if (0UL != (mapCfg[idx].offset & pageSizeMask)) {
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
        /* If any of the map segment size is not multiple of page size */
        if (0UL != (mapCfg[idx].size & pageSizeMask)) {
            BCM_LOG("PCIe MAP Offset / Size not matching : [offset, size, pageSize] [%d, %d, %d] \n",
                     mapCfg[idx].offset, mapCfg[idx].size, (pageSizeMask + 1));
        }
    }
    return ret;
}

/**
    @brief Set up BAR Configure

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]  aBARNum     BAR Number
    @param[in]  aBARCfg      BAR configuration

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             PCIe init was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid
    @retval #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval #BCM_ERR_INVAL_PARAMS   Size specified in the config parameter is invalid
    @retval #BCM_ERR_INVAL_STATE    Driver is already initalized

    @post PCIe driver initialized

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    if aBARCfg->mapCfg is NULL or aBARCfg->numMaps is zero
        disable the BAR in following register
            BAR1 => config_2
            BAR2 => bar2_config
            BAR3 => bar3_config
    else
        call  PCIE_DrvValidateBARCfg(aBARCfg, &BARSize);
        if error
            return error code

    find the smallest supported imap size for the BAR
        that is more than the requested size
    if not found
        return BCM_ERR_INVAL_PARAM

    pageSize = imapSize/PCIE_NUM_PAGES_IN_BAR

    check if all the map sizes are aligned with the pageSize
    if not
        return BCM_ERR_INVAL_PARAM
    check if all the map offsets are aligned with the pageStart
    if not
        log a debug message

    Set ref clock and BAR number in pcie_ctrl_2
    Set IMAP write enable in imap register
    set the vendor id 0xAFFF to CFG_PRIV1_id_val2 register

    Set the total BAR size in following register (through indirect write)
        BAR1 => config_2
        BAR2 => bar2_config
        BAR3 => bar3_config

    for all the pages in the BAR
        if the page need to be enabled (based on the mapCfg param)
            Set the IMAP size in following register
                BAR1 => imap0
                BAR2 => imap1
                BAR3 => imap2
            Set the write enable, read enable, cache config,
                and internal address for page 0
                in IMAP1->pg0[page] register

    Set IMAP Size
    Set IMAP write disable in imap register

    return BCM_ERR_OK
    @endcode
*/
static int32_t PCIE_DrvBARConfig(PCIE_BARNumType aBARNum, const PCIE_BARConfigType * const aBARCfg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;
    PCIE_BARSizeType imapSize = 0UL;
    PCIE_BARSizeType BARSize = 0UL;
    uint32_t pageSize = 0UL;
    uint32_t pageSizeMask = 0UL;
    PCIE_BARMapConfigType const *mapCfg = aBARCfg->mapCfg;
    PCIE_BARMapConfigType mapCfgZero[1] = {};

    if ((NULL == aBARCfg->mapCfg) || (0UL == aBARCfg->numMaps)) {
        mapCfg = mapCfgZero; /* Disable the BAR and Disable all the maps */
    } else {
        ret = PCIE_DrvValidateBARCfg(aBARCfg, &BARSize);

        if (BCM_ERR_OK == ret) {
            ret = BCM_ERR_INVAL_PARAMS;
            /* BCM_TEST_ADD_PROBE(BCM_PCIE_ID, PCIEDRV_PTSEQ_EP_BAR_CONFIG,
                                  PCIEDRV_PTST_DRV_EP_BAR_CFG_BAR_SIZE,
                                  BARSize += 15UL) */
            /* Find the next supported imap size */
            for (i = 0UL; PCIE_BARValidSize[aBARNum][i] != PCIE_BAR_SIZE_NONE; i++) {
                if (PCIE_BARValidSize[aBARNum][i] >= BARSize) {
                    imapSize = PCIE_BARValidSize[aBARNum][i];
                    /* @event            CERT-C INT34
                       @issue            Coverity infers that the shift might overflow
                       @risk             No risk
                       @justification    imapSize max value is 1GB (15). Shifting 32K by 15 bit is within the limit */
                    /* coverity[cert_int34_c_violation] */
                    uint32_t totalSize = PCIE_BAR_SIZE2BYTES(imapSize);
                    pageSize = totalSize/PCIE_NUM_PAGES_IN_BAR;
                    /* @event            CERT-C INT30
                       @issue            Coverity infers that page size can wrap
                       @risk             No risk
                       @justification    Validation of BAR size is already done so it will be non-zero always. */
                    /* coverity[cert_int30_c_violation] */
                    pageSizeMask = pageSize - 1UL;
                    ret = BCM_ERR_OK;
                    break;
                }
            }
        }

        if (BCM_ERR_OK == ret) {
            ret  = PCIE_IntDrvValidateBARMapConfig(mapCfg, aBARCfg, pageSizeMask);
        }
    }

    if (ret == BCM_ERR_OK) {
        /* Select IMAP for the given BAR */
        PCIE_DrvSetImapValue(PCIE_IMapSelect[aBARNum]);
        /* Write Enable for IMAP config */
        PCIE_REG_IMAP_CTL->imap |= IMAP_CTRL_PCIE_IMAP_CTRL_00_CS_CFG_OPEN_MASK;

        /* BAR Size Configuration - done through indirect access */

        PCIE_DrvIndirectIO(PCIE_BARConfigReg[aBARNum],
                           CFG_PRIV0_CONFIG_2_BAR1_SIZE_MASK |
                           CFG_PRIV0_CONFIG_2_BAR1_64ENA_MASK,
                           BARSize | CFG_PRIV0_CONFIG_2_BAR1_64ENA_MASK);
        /* Write Disable for IMAP config */
        PCIE_REG_IMAP_CTL->imap &= ~(IMAP_CTRL_PCIE_IMAP_CTRL_00_CS_CFG_OPEN_MASK);
        PCIE_REG_IMAP_CTL->imap |= IMAP_CTRL_PCIE_IMAP_CTRL_00_IMAP0123_WR_ENA_MASK;

        uint32_t mapIdx = 0UL;
        uint32_t offset = 0UL;

        for (i = 0UL; i < PCIE_NUM_PAGES_IN_BAR; i++) {
            uint32_t pageCfg = 0UL;
            uint32_t offsetStart = mapCfg[mapIdx].offset;
            uint32_t offsetEnd = offsetStart + mapCfg[mapIdx].size;
            uint32_t mapAddr = 0UL;
            /* Enable the page only if it is needed by the current map config */
            if ((offset >= offsetStart) && (offset < offsetEnd)) {
                /* IMAP Page attributes */
                pageCfg = IMAP1_PAGE_VALID;
                PCIE_DrvGetPageConfig(mapCfg[mapIdx].memAttrib, &pageCfg);
                if((BCM_UINT32_MAX - mapCfg[mapIdx].mapAddr) > offset) {
                    mapAddr = ((mapCfg[mapIdx].mapAddr + offset) & 0xFFFFFFFFUL) - offsetStart;
                } else {
                    ret = BCM_ERR_INVAL_PARAMS;
                }
            }
            /* The LS12bits of the address is ignored - otherwise the
               address to be written as is */
            PCIE_REG_IMAP1->pg0[i] = pageCfg | (IMAP1_ADDR_MASK & mapAddr);
            /* @event           CERT-C INT34
                @issue          Coverity infers that the addition would overflow
                @risk           No risk
                @justification  pageSize is computed as totalSize/PCIE_NUM_PAGES_IN_BAR.
                                offset would not exceed totalSize                          */
            /* coverity[cert_int30_c_violation] */
            offset += pageSize;
            if ((offset >= offsetEnd) && ((mapIdx+1UL) < aBARCfg->numMaps)) {
                /* Go to the next map */
                mapIdx++;
            }
        }

        /* Setup IMAP Size */
        *PCIE_IMAPCtlReg[aBARNum] = imapSize;
        /* Write Disable for IMAP config */
        PCIE_REG_IMAP_CTL->imap &= ~(IMAP_CTRL_PCIE_IMAP_CTRL_00_IMAP0123_WR_ENA_MASK);

    }

    return ret;
}

/**
    @brief Validate RC page map address.
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
       Validate page map address
    @endcode
*/
static int32_t PCIE_IntDrvValidateRCMapConfig(PCIE_RcMapConfigType const *mapCfg, const PCIE_RcSpaceConfigType * const aRCCfg, uint32_t pageSizeMask)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t idx = 0UL;

    for (idx = 0UL; idx < aRCCfg->numMaps; idx++) {
        /* If any of the map segment offset is not multiple of page size */
        if (0UL != (mapCfg[idx].offset & pageSizeMask)) {
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
        /* If any of the map segment size is not multiple of page size */
        if (0UL != (mapCfg[idx].size & pageSizeMask)) {
            BCM_LOG("PCIe MAP Offset / Size not matching : [offset, size, pageSize] [%d, %d, %d] \n",
                     mapCfg[idx].offset, mapCfg[idx].size, (pageSizeMask + 1));
        }
    }
    return ret;
}
/**
    @brief Set up RC space Configure

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]  aRCSpaceNum   RC space Number
    @param[in]  aRCCfg             RC space configuration

    Return values are documented in reverse-chronological order

    @retval #BCM_ERR_OK             PCIe init was successful
    @retval #BCM_ERR_INVAL_PARAMS   hwId is not valid
    @retval #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval #BCM_ERR_INVAL_PARAMS   Size specified in the config parameter is invalid
    @retval #BCM_ERR_INVAL_STATE    Driver is already initalized

    @post PCIe driver initialized

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    if aRCCfg->rcMapCfg is NULL or aRCCfg->numMaps is zero
        disable the RC map
    else
        call  PCIE_IntDrvValidateRCCfg(aRCCfg, &RCSize);
        if error
            return error code

    find the smallest supported imap size for the RC
        that is more than the requested size
        Program RC space with base address and RC space size
    if not found
        return BCM_ERR_INVAL_PARAM

    pageSize = imapSize/PCIE_NUM_PAGES_IN_BAR

    check if all the map sizes are aligned with the pageSize
    if not
        return BCM_ERR_INVAL_PARAM
    check if all the map offsets are aligned with the pageStart
    if not
        log a debug message
    Set IMAP page select
        RC space0 ->IMAP0
        RC space1 ->IMAP1
        RC space2 ->IMAP2
    Set the write enable, read enable, cache config,
        and internal address for page 0
        in IMAP1->pg0[page] register

    Set IMAP Size
    Set IMAP write disable in imap register

    return BCM_ERR_OK
    @endcode
*/
static int32_t PCIE_IntDrvRCConfig(PCIE_RCSpaceNumType aRCNum, const PCIE_RcSpaceConfigType * const aRCCfg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t idx = 0UL;
    PCIE_BARSizeType imapSize = 0UL;
    PCIE_BARSizeType BARSize = 0UL;
    uint32_t totalSize = 0UL;
    uint32_t pageSize = 0UL;
    uint32_t pageSizeMask = 0UL;
    /*volatile uint32_t *ptr = NULL;*/
    PCIE_RcMapConfigType const *mapCfg = aRCCfg->rcMapCfg;
    PCIE_RcMapConfigType mapCfgZero[1] = {};
    BCM_MemSet(mapCfgZero,0U,sizeof(PCIE_RcMapConfigType));
    if ((NULL == aRCCfg->rcMapCfg) || (0UL == aRCCfg->numMaps)) {
        mapCfg = mapCfgZero; /* Disable the RC space for all the maps */
    } else {
        ret = PCIE_IntDrvValidateRCCfg(aRCCfg, &BARSize);

        if (BCM_ERR_OK == ret) {
            ret = BCM_ERR_INVAL_PARAMS;
            /* BCM_TEST_ADD_PROBE(BCM_PCIE_ID, PCIEDRV_PTSEQ_EP_BAR_CONFIG,
                                  PCIEDRV_PTST_DRV_EP_BAR_CFG_BAR_SIZE,
                                  BARSize += 15UL) */
            /* Find the next supported imap size */
            for (idx = 0UL; PCIE_BARValidSize[aRCNum][idx] != PCIE_BAR_SIZE_NONE; idx++) {
                if (PCIE_BARValidSize[aRCNum][idx] >= BARSize) {
                    imapSize = PCIE_BARValidSize[aRCNum][idx];

                    /* @event            CERT-C INT34
                       @issue            Coverity infers that the shift might overflow
                       @risk             No risk
                       @justification    imapSize max value is 1GB (15). Shifting 32K by 15 bit is within the limit */
                    /* coverity[cert_int34_c_violation] */
                    totalSize = PCIE_BAR_SIZE2BYTES(imapSize);
                    pageSize = (totalSize/PCIE_NUM_PAGES_IN_BAR);
                    /* @event            CERT-C INT30
                       @issue            Coverity infers that page size can wrap
                       @risk             No risk
                       @justification    Validation of BAR size is already done so it will be non-zero always. */
                    /* coverity[cert_int30_c_violation] */
                    pageSizeMask = pageSize - 1UL;
                    ret = BCM_ERR_OK;
                    break;
                }
            }
        }

        if (BCM_ERR_OK == ret) {
            ret = PCIE_IntDrvValidateRCMapConfig(mapCfg, aRCCfg, pageSizeMask);
        }
    }

    if (ret == BCM_ERR_OK) {
        /* Select IMAP for the given RC space */
        uint32_t mapIdx = 0UL;
        uint32_t offset = 0UL;

        /* Set RC space and its internal mapping */
        PCIE_DrvSetImapValue(PCIE_IMapSelect[aRCNum]);

        PCIE_REG_IMAP_CTL->imap |= IMAP_CTRL_PCIE_IMAP_CTRL_00_IMAP0123_WR_ENA_MASK;
        /* Configure RC space base address for incoming addresses that is matched to the incoming addr to determine validity. */
        *PCIE_RC_SPACE_REG[aRCNum] = aRCCfg->baseAddress;

        for (idx = 0UL; idx < PCIE_NUM_PAGES_IN_BAR; idx++) {
            uint32_t pageCfg = 0UL;
            uint32_t offsetStart = mapCfg[mapIdx].offset;
            uint32_t offsetEnd = offsetStart + mapCfg[mapIdx].size;
            uint32_t mapAddr = 0UL;
            /* Enable the page only if it is needed by the current map config */
            if ((offset >= offsetStart) && (offset < offsetEnd)) {
                /* IMAP Page attributes */
                pageCfg = IMAP1_PAGE_VALID;
                PCIE_DrvGetPageConfig(mapCfg[mapIdx].memAttrib, &pageCfg);
                if((BCM_UINT32_MAX - mapCfg[mapIdx].mapAddr) > offset) {
                    mapAddr = ((mapCfg[mapIdx].mapAddr + offset) & 0xFFFFFFFFUL) - offsetStart;
                } else {
                    ret = BCM_ERR_INVAL_PARAMS;
                    break;
                }
            }
            /* The LS12bits of the address is ignored - otherwise the
               address to be written as is */
            PCIE_REG_IMAP1->pg0[idx] = pageCfg | (IMAP1_ADDR_MASK & mapAddr);
            /* @event           CERT-C INT34
                @issue          Coverity infers that the addition would overflow
                @risk           No risk
                @justification  pageSize is computed as totalSize/PCIE_NUM_PAGES_IN_BAR.
                                offset would not exceed totalSize                          */
            /* coverity[cert_int30_c_violation] */
            offset += pageSize;
            if ((offset >= offsetEnd) && ((mapIdx+1UL) < aRCCfg->numMaps)) {
                /* Go to the next map */
                mapIdx++;
            }
        }

        /* Setup IMAP Size */
        *PCIE_IMAPCtlReg[aRCNum] = imapSize;

        /* Write Disable for IMAP config */
        PCIE_REG_IMAP_CTL->imap &= ~(IMAP_CTRL_PCIE_IMAP_CTRL_00_IMAP0123_WR_ENA_MASK);

    }

    return ret;
}


/** @brief Initializes PCIe Endpoint

    Initializes and configures the PCIe endpoint. It does not
    enable the interface. It needs to be enabled using PCIE_SetEnable() API

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
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_SET_INTERFACE_PROC
    @limitations None

    @code{.unparsed}
    if aID >= PCIE_HW_ID_MAX or aConfig is NULL
        return BCM_ERR_INVAL_PARAMS
    if driver not initialized
        return BCM_ERR_INVAL_STATE

    Deassert RESCAL reset
    Wait for the RESCAL done and RESCAL valid bit
        If timeout return BCM_ERR_TIME_OUT

    Set the RESCAL data and override enable to hsio_ctrl_5

    Keep PCIe logic in reset (HSIO_CTRL PERST)
    for all the configuration in aConfig->barConfig
        call PCIE_DrvBARConfig(barConfig[i])
        if error
            return error code
    Update the device id with aConfig->deviceId
    PCIE_DrvRWDev[aID].state = PCIE_DRVSTATE_INIT
    @endcode
*/
int32_t PCIE_EpInit(PCIE_HwIDType hwId, const PCIE_EpConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;
    if (PCIE_HW_ID_MAX <= hwId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
#ifdef BCM8915X_PARAM_VALIDATION
    else if (NULL == aConfig) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (PCIE_DRVSTATE_UNINIT != PCIE_DrvRWDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif

    if (BCM_ERR_OK == retVal) {

        /* enable HSIO block clock */
        /*uint32_t pon =  (PCIE_REG_HSAFE->rescal_status0 >> HSAFE_RESCAL_STATUS0_PON_SHIFT) &
                        HSAFE_RESCAL_STATUS0_PON_MASK;*/

        /* Keep the logic in reset */
        PCIE_REG_HSIO_COM->hsio_ctrl_0 &= ~HSIO_COMMON_HSIO_CTRL_32BIT_0_PCIE_PERST_N_OVRD_VAL_MASK;
        PCIE_REG_HSIO_COM->hsio_ctrl_0 |=  HSIO_COMMON_HSIO_CTRL_32BIT_0_PCIE_PERST_N_OVRD_SEL_MASK;
        uint32_t hsio_ctrl_5 = PCIE_REG_HSIO_COM->hsio_ctrl_5;
        hsio_ctrl_5 &= ~HSIO_COMMON_HSIO_CTRL_32BIT_5_PCIE_RES_CAL_OVRD_VAL_MASK;
        hsio_ctrl_5 |= (0x7UL << HSIO_COMMON_HSIO_CTRL_32BIT_5_PCIE_RES_CAL_OVRD_VAL_SHIFT) &
                        HSIO_COMMON_HSIO_CTRL_32BIT_5_PCIE_RES_CAL_OVRD_VAL_MASK;
        hsio_ctrl_5 |= HSIO_COMMON_HSIO_CTRL_32BIT_5_PCIE_RES_CAL_OVRD_SEL_MASK;
        hsio_ctrl_5 |= HSIO_COMMON_HSIO_CTRL_32BIT_5_RES_CAL_VLD_OVRD_VAL_MASK;
        hsio_ctrl_5 |= HSIO_COMMON_HSIO_CTRL_32BIT_5_RES_CAL_VLD_OVRD_SEL_MASK;
        PCIE_REG_HSIO_COM->hsio_ctrl_5 = hsio_ctrl_5;

        /* PCIE_REG_HSIO_COM->hsio_ctrl_5 = 0x1D777UL; */

        BCM_DelayUs(500UL);

        PCIE_DrvSetImapValue(0);
        /* Clear axi_secure_bypass */
        PCIE_REG_EP_COMMON->pcie_cmn_4 &= ~PCIE_COMMON_PCIE_CMN_4_RG_AXI_SECURE_BYPASS_MASK;

        PCIE_DrvSetImapValue(1);
        /* Clear cycle retry bit */
        PCIE_DrvIndirectIO(&PCIE_REG_CFG_PRIV0->config_2,
                           CFG_PRIV0_CONFIG_2_CFG_CYCLE_RETRY_MASK,
                           0UL);

        for (i = 0UL; i <= PCIE_BAR_NUM_MAX; i++) {
            retVal = PCIE_DrvBARConfig(i, &aConfig->barConfig[i]);
            if (BCM_ERR_OK != retVal) {
                break;
            }
        }

        PCIE_DrvSetImapValue(0);
    }
    if (BCM_ERR_OK == retVal) {
        if(aConfig->deviceId > 0U) {
            PCIE_DrvSetDeviceId(aConfig->deviceId);
        }
        PCIE_DrvRWDev[hwId].state = PCIE_DRVSTATE_INIT;
        PCIE_DrvRWDev[hwId].controllerMode = PCIE_CONTROLLER_MODE_EP;
    }
    return retVal;
}


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
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC

    @limitations None
   @code{.unparsed}
        if hwId >= PCIE_HW_ID_MAX or aConfig is NULL
        return BCM_ERR_INVAL_PARAMS
     if driver not initialized
        return BCM_ERR_INVAL_STATE
        Select RC mode by setting HSIO_COMMON_PCIE_CTRL_32BIT_0_STRAP_PCIE_RC_MODE_MASK
        Set rc_perst_b
        Set reference clock LCPLL program it to use internal clock (3'b000)
        De-assert rc_perst_b
        RC space address programming and setting RC space size.
    @endcode
*/
int32_t PCIE_RcInit(PCIE_HwIDType hwId, const PCIE_RcConfigType * const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t ii = 0UL;
    uint32_t data = 0UL;
    uint32_t hsio_ctrl_5 = 0UL;
    uint32_t lcPllLock = 0UL;
    int32_t timeoutCounter = 1000;

#ifdef BCM8915X_PARAM_VALIDATION
    if (PCIE_HW_ID_MAX <= hwId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (NULL == aConfig) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (PCIE_DRVSTATE_UNINIT != PCIE_DrvRWDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif

        /* Keep the logic in reset */
        PCIE_REG_HSIO_COM->hsio_ctrl_0 &= ~HSIO_COMMON_HSIO_CTRL_32BIT_0_PCIE_PERST_N_OVRD_VAL_MASK;
        PCIE_REG_HSIO_COM->hsio_ctrl_0 |=  HSIO_COMMON_HSIO_CTRL_32BIT_0_PCIE_PERST_N_OVRD_SEL_MASK;

        PCIE_REG_HSIO_COM->pcie_ctrl_2 &= ~(HSIO_COMMON_PCIE_CTRL_32BIT_2_REFCLKZ_IN_SELECT_MASK);
        BCM_DelayUs(1000UL);
        hsio_ctrl_5 = PCIE_REG_HSIO_COM->hsio_ctrl_5;
        hsio_ctrl_5 |= HSIO_COMMON_HSIO_CTRL_32BIT_5_RES_CAL_VLD_OVRD_SEL_MASK;
        hsio_ctrl_5 |= HSIO_COMMON_HSIO_CTRL_32BIT_5_RES_CAL_VLD_OVRD_VAL_MASK;
        hsio_ctrl_5 |= HSIO_COMMON_HSIO_CTRL_32BIT_5_PCIE_RES_CAL_OVRD_SEL_MASK;
        hsio_ctrl_5 &= ~HSIO_COMMON_HSIO_CTRL_32BIT_5_PCIE_RES_CAL_OVRD_VAL_MASK;
        hsio_ctrl_5 |= (0x7UL << HSIO_COMMON_HSIO_CTRL_32BIT_5_PCIE_RES_CAL_OVRD_VAL_SHIFT);
        PCIE_REG_HSIO_COM->hsio_ctrl_5 = hsio_ctrl_5;

        BCM_DelayUs(1000UL);

        PCIE_DrvSetImapValue(0);
        /* Select RC mode */
        PCIE_REG_HSIO_COM->pcie_ctrl_0 |= HSIO_COMMON_PCIE_CTRL_32BIT_0_STRAP_PCIE_RC_MODE_MASK;

        /* Clear Bit[2] - `CLK_CONTROL__EP_PERST_SOURCE_SELECT to select rc_perst_b */
        data = *PCIE_PERST_SOURCE_SELECT_REG;
        data &= 0xFFFFFFFBUL;
        *PCIE_PERST_SOURCE_SELECT_REG = data;
        BCM_DelayUs(1000UL);
        lcPllLock = (PCIE_REG_EP_COMMON->stat00 & PCIE_COMMON_LCPLL_STATUS_LOCK_MASK);
        BCM_LOG("LC PLL lock before init 0x%x \n",lcPllLock);
        /* Reference clock change to use internal clock LCPLL and bring out of reset */
        PCIE_REG_EP_COMMON->ctrl00 |= PCIE_COMMON_LCPLL_00_PWR_ON_MASK;
        BCM_DelayUs(25UL);
        PCIE_REG_EP_COMMON->ctrl00 |= PCIE_COMMON_LCPLL_00_RESETB_MASK;
        BCM_DelayUs(200UL);
        PCIE_REG_EP_COMMON->ctrl00 |= PCIE_COMMON_LCPLL_00_POST_RESETB_MASK;
        PCIE_REG_EP_COMMON->pcie_cmn_4 &= ~PCIE_COMMON_PCIE_CMN_4_PCIE_REFCLKZ_IN_SELECT_MASK;
        BCM_DelayUs(200UL);
        if(aConfig->sscEn > 0UL) {
            data = PCIE_REG_EP_COMMON->ctrl00;
            data |= PCIE_COMMON_LCPLL_00_SSC_MODE_MASK;
            PCIE_REG_EP_COMMON->ctrl00 = data;
        }

        /* Check the LCPLL lock */
        do {
            lcPllLock = (PCIE_REG_EP_COMMON->stat00 & PCIE_COMMON_LCPLL_STATUS_LOCK_MASK);
            timeoutCounter--;
            BCM_DelayUs(10UL);
        } while ((lcPllLock == 0UL) && (timeoutCounter > 0));

        if((lcPllLock == 0UL) && (timeoutCounter == 0)){
            retVal = BCM_ERR_TIME_OUT;
        } else {

            /* enableb_ch[9:4] = 0x0 */
            PCIE_REG_EP_COMMON->ctrl00 &= ~(0x3E0UL);
            BCM_DelayUs(100UL);
            /* RC address space programming. */
            for(ii = 0UL;ii<= PCIE_RC_SPACE_NUM_MAX; ii++) {
                retVal = PCIE_IntDrvRCConfig(ii, &aConfig->rcSpaceConfig[ii]);
                if (BCM_ERR_OK != retVal) {
                    break;
                }
            }
            /* RC BAR configuration. */
            for (ii = 0UL; ii < PCIE_BAR_NUM_RC_MAX; ii++) {
                if(aConfig->barConfig[ii].mapCfg != NULL) {
                    retVal = PCIE_DrvBARConfig(ii, &aConfig->barConfig[ii]);
                    if (BCM_ERR_OK != retVal) {
                        break;
                    }
                }
            }

            PCIE_DrvSetImapValue(0);

        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    if (BCM_ERR_OK == retVal) {
        /* PCIE_DrvSetDeviceId(aConfig->deviceId); */
        PCIE_DrvRWDev[hwId].state = PCIE_DRVSTATE_INIT;
        PCIE_DrvRWDev[hwId].controllerMode = PCIE_CONTROLLER_MODE_RC;
    }
    return retVal;
}

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

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_SET_INTERFACE_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC


    @code{.unparsed}
        if hwId >= PCIE_HW_ID_MAX or aConfig is NULL
        return BCM_ERR_INVAL_PARAMS
     if driver not initialized
        return BCM_ERR_INVAL_STATE
     if aEnable
        Clear PCIe logic reset (HSIO_CTRL PERST)
     else
        Set PCIe logic reset (HSIO_CTRL PERST)

    @endcode
*/
int32_t PCIE_SetInterface(PCIE_HwIDType hwId, uint8_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t data;
#ifdef BCM8915X_PARAM_VALIDATION
    if (PCIE_HW_ID_MAX <= hwId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (PCIE_DRVSTATE_INIT != PCIE_DrvRWDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
        if (aEnable > 0U) {
            PCIE_REG_HSIO_COM->hsio_ctrl_0 &= ~HSIO_COMMON_HSIO_CTRL_32BIT_0_PCIE_PERST_N_OVRD_SEL_MASK;
            BCM_DelayUs(100UL);
            if(PCIE_DrvRWDev[hwId].controllerMode == PCIE_CONTROLLER_MODE_RC) {
                /* De-assert, rc_perst_b. CLK_CONTROL__EP_PERST_SOURCE_SELECT */
                data = *PCIE_PERST_SOURCE_SELECT_REG;
                data = (data | 0x1UL);
                *PCIE_PERST_SOURCE_SELECT_REG = data;
                BCM_DelayUs(20000UL);

                /* Enabling the cfg_bus_master_enable and cfg_bar1_mem_en. */
                PCIE_DrvSetImapValue(1);
                PCIE_DrvIndirectIO((volatile uint32_t *)(uintptr_t)0x4UL,
                                   0x6UL,
                                   6UL);
                PCIE_DrvSetImapValue(0);
            }
        } else {
            PCIE_REG_HSIO_COM->hsio_ctrl_0 |=  HSIO_COMMON_HSIO_CTRL_32BIT_0_PCIE_PERST_N_OVRD_SEL_MASK;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;

}

/** @brief De-Initializes PCIe

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

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_SET_INTERFACE_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC

    @limitations None

    @code{.unparsed}
    if hwId <= PCIE_HW_ID_MAX
        return BCM_ERR_INVAL_PARAMS
    if driver not initalized
        retVal = BCM_ERR_INVAL_STATE
    Disable PCIe logic
    for all the BARs
        Disable the BAR configuration
    Set default sus system vendor id
    clear &PCIE_DrvRWDev[hwId]
     @endcode

*/
int32_t PCIE_DeInit(PCIE_HwIDType hwId)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (PCIE_HW_ID_MAX <= hwId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (PCIE_DRVSTATE_INIT != PCIE_DrvRWDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
        uint32_t i;
        PCIE_BARConfigType BARCfg = {};
        /* Disable PCIe logic */
        (void)PCIE_SetInterface(hwId, 0U);
        /* Disable All the BARs */
        for (i = 0UL; i <= PCIE_BAR_NUM_MAX; i++) {
            (void)PCIE_DrvBARConfig(i, &BARCfg);
        }
        PCIE_DrvSetDeviceId(PCIE_DEF_SUB_SYS_VENDOR_ID);
        PCIE_DrvRWDev[hwId].state = PCIE_DRVSTATE_UNINIT;
        PCIE_DrvRWDev[hwId].controllerMode = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;

}


/** @brief Interrupt configuration.

    This API configures the parameter for the selected interrupt type used.

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

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC

    @limitations None

    @code{.unparsed}
    if hwId <= PCIE_HW_ID_MAX
        return BCM_ERR_INVAL_PARAMS
    if driver not initalized
        retVal = BCM_ERR_INVAL_STATE
    if EP config is not NULL and controller is initializaed as EP
         set multi message capabiliy
         Set msi PV MASK value
         Set interrupt remap 16 input interrupt to map to interrupt output.
         Set interrupt auto clear mode or software mode.
    if RC config is not NULL and controller is initialized as RC
        Set MSI page address where MSI message is recieved in RC.
        Set MSI event queue page address where MSI event queue is received in RC.
        Set MSI event queue control register.
    @endcode
*/
int32_t PCIE_InterruptConfigSet(PCIE_HwIDType hwId, PCIE_UseIntrType intrType, PCIE_IntrCfgType *aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t idx = 0UL;
    uint32_t value = 0UL;
    uint32_t intrRemapReg = 0UL;
    uint32_t *memPtr = NULL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (PCIE_HW_ID_MAX <= hwId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (PCIE_DRVSTATE_INIT != PCIE_DrvRWDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else if((intrType == PCIE_USE_INTR_MSIX) && (PCIE_DrvRWDev[hwId].controllerMode == PCIE_CONTROLLER_MODE_RC)){
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if((aConfig->epConfig != NULL) && (PCIE_DrvRWDev[hwId].controllerMode == PCIE_CONTROLLER_MODE_EP)) {
            PCIE_REG_EP_COMMON->pcie_cmn_4 &= ~PCIE_COMMON_PCIE_CMN_4_RG_AXI_SECURE_BYPASS_MASK;

            /* Select IMAP for the given BAR */
            PCIE_DrvSetImapValue(PCIE_IMapSelect[0U]);
            /* Write Enable for IMAP config */
            PCIE_REG_IMAP_CTL->imap |= IMAP_CTRL_PCIE_IMAP_CTRL_00_CS_CFG_OPEN_MASK;

            PCIE_DrvIndirectIORead(&PCIE_REG_CFG_PRIV1->id_val4, &value);
            value &= (uint32_t)(~CFG_PRIV1_ID_VAL4_MULTI_MSG_CAP_MASK);
            value |= ((aConfig->epConfig->multiMsgCap & 0x7UL) << CFG_PRIV1_ID_VAL4_MULTI_MSG_CAP_SHIFT);
            value |= ((aConfig->epConfig->msiPvMaskCapable & 0x1UL) << CFG_PRIV1_ID_VAL4_MSI_PV_MASK_CAPABLE_SHIFT);

            PCIE_DrvIndirectIO(&PCIE_REG_CFG_PRIV1->id_val4, CFG_PRIV1_ID_VAL4_MULTI_MSG_CAP_MASK | CFG_PRIV1_ID_VAL4_MSI_PV_MASK_CAPABLE_MASK,
                                   value);
            /* Write Enable for IMAP config */
            PCIE_REG_IMAP_CTL->imap &= ~IMAP_CTRL_PCIE_IMAP_CTRL_00_CS_CFG_OPEN_MASK;
            /* Reset IMAP value */
            PCIE_DrvSetImapValue(0);

            memPtr = PCIE_INTC_REMAP_BITPOS_REG0;
            while(idx < 15UL) {
                intrRemapReg =   ((aConfig->epConfig->intrRemap[idx++] & 0x3FUL) << PCIE_INTR_REMAP_BITPOS_REG0_SHIFT);
                intrRemapReg |=  ((aConfig->epConfig->intrRemap[idx++] & 0x3FUL) << PCIE_INTR_REMAP_BITPOS_REG1_SHIFT);
                intrRemapReg |=  ((aConfig->epConfig->intrRemap[idx++] & 0x3FUL) << PCIE_INTR_REMAP_BITPOS_REG2_SHIFT);
                intrRemapReg |=  ((aConfig->epConfig->intrRemap[idx++] & 0x3FUL) << PCIE_INTR_REMAP_BITPOS_REG3_SHIFT);
                intrRemapReg |=  ((aConfig->epConfig->intrRemap[idx++] & 0x3FUL) << PCIE_INTR_REMAP_BITPOS_REG4_SHIFT);
                *memPtr = intrRemapReg;
                memPtr++;
            }

            intrRemapReg =  (aConfig->epConfig->intrRemap[15U]  & 0x3FUL);
            *memPtr = intrRemapReg;
            /* Set IMAP value while accessing private registers */
            PCIE_DrvSetImapValue(PCIE_IMapSelect[0U]);
            /* Write Enable for IMAP config */
            PCIE_REG_IMAP_CTL->imap |= IMAP_CTRL_PCIE_IMAP_CTRL_00_CS_CFG_OPEN_MASK;
            /* Clear cycle retry bit */
            PCIE_DrvIndirectIO(&PCIE_REG_CFG_PRIV0->config_2,
                           CFG_PRIV0_CONFIG_2_CFG_CYCLE_RETRY_MASK,
                           0UL);
            /* Write Disable for IMAP config */
            PCIE_REG_IMAP_CTL->imap &= ~IMAP_CTRL_PCIE_IMAP_CTRL_00_CS_CFG_OPEN_MASK;
            /* Reset IMAP value */
            PCIE_DrvSetImapValue(0);
            /* In Rigel interrupt clear is done  using SW mode, auto clear is not used. */
            *PCIE_INTC_INTR_CLR_MODE_0 = 0UL;
        }
        if((aConfig->rcConfig != NULL) && (PCIE_DrvRWDev[hwId].controllerMode == PCIE_CONTROLLER_MODE_RC)) {
            *PCIE_SYS_MSI_PAGE_ADDRL = aConfig->rcConfig->rcMsiAddrLow;
            *PCIE_SYS_MSI_PAGE_ADDRH = aConfig->rcConfig->rcMsiAddrHigh;
            *PCIE_SYS_EQ_PAGE_ADDRL = aConfig->rcConfig->pcieSysMsiEventQueuePageAddrLow;
            *PCIE_SYS_EQ_PAGE_ADDRH= aConfig->rcConfig->pcieSysMsiEventQueuePageAddrUp;
            memPtr = PCIE_SYS_MSI_EQ_CTRL_0;
            for(idx = 0UL; idx < 6UL; idx++) {
                value = ((aConfig->rcConfig->msiEqCtrl[idx].eqEnable & 0x1UL) << PCIE_SYS_MSI_CTRL_EQ_EN_SHIFT);
                value |= ((aConfig->rcConfig->msiEqCtrl[idx].intrNEvent & 0x3FUL) << PCIE_SYS_MSI_CTRL_INTR_NEVENT_SHIFT);
                value |= ((aConfig->rcConfig->msiEqCtrl[idx].intrNDelay & 0xFUL) << PCIE_SYS_MSI_CTRL_INTR_NDELAY_SHIFT);
                value |= ((aConfig->rcConfig->msiEqCtrl[idx].msiIntrEnEq & 0x1UL) << PCIE_SYS_MSI_CTRL_MSI_INTR_EN_EQ_SHIFT);
                *memPtr = value;
                memPtr++;
            }
            /* Enable legacy INTX interrupts in RC mode  */
            *PCIE_SYS_RC_INTX_EN = 0xFUL;
        }
#ifdef BCM8915X_PARAM_VALIDATION

    }
#endif

    return retVal;
}

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

    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PCIE_ENABLE_INTERRUPTS_PROC

    @limitations None
    @code{.unparsed}
    if hwId <= PCIE_HW_ID_MAX
        return BCM_ERR_INVAL_PARAMS
    if driver not initalized
        retVal = BCM_ERR_INVAL_STATE
     Enable/disable interrupts represented by the bit field (0-31) 32 interrupts is enabled by setting bits 0-31 depending
     on enable mask value.
    @endcode
*/
int32_t PCIE_EnableInterrupts(PCIE_HwIDType hwId, uint32_t enableMask, uint8_t aEnable)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t value = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (PCIE_HW_ID_MAX <= hwId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (PCIE_DRVSTATE_INIT != PCIE_DrvRWDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif
         if(aEnable > 0U) {
             value = enableMask;
             *PCIE_INTC_INTR_ENABLE_REG0 = value;
         } else {
             value = *PCIE_INTC_INTR_ENABLE_REG0;
             value &= (uint32_t)(~enableMask);
             *PCIE_INTC_INTR_ENABLE_REG0 = value;
         }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

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
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
    @trace #BRCM_SWARCH_PCIE_GET_PENDING_INTERRUPTS_PROC

    @limitations None

    @code{.unparsed}
    if hwId <= PCIE_HW_ID_MAX
        return BCM_ERR_INVAL_PARAMS
    if driver not initalized
        retVal = BCM_ERR_INVAL_STATE
    Read PCIE interrupt status register and update the pending interrupt status bit fields (0-31).
    @endcode
*/
int32_t PCIE_GetPendingInterrupts(PCIE_HwIDType hwId, uint32_t *pendingIntrMask)
{
    int32_t retVal = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (PCIE_HW_ID_MAX <= hwId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (PCIE_DRVSTATE_INIT != PCIE_DrvRWDev[hwId].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
#endif

        *pendingIntrMask = *PCIE_INTC_INTR_STATUS;

#ifdef BCM8915X_PARAM_VALIDATION

    }
#endif
    return retVal;

}

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
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_GET_LINK_STATUS_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    Get the PCIe link status.
    @endcode

*/
int32_t PCIE_GetLinkStatus(PCIE_HwIDType hwId, uint32_t *alinkStatus)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t data = 0UL;
    int32_t counter = 1000;

#ifdef BCM8915X_PARAM_VALIDATION
        if (PCIE_HW_ID_MAX <= hwId) {
            retVal = BCM_ERR_INVAL_PARAMS;
        } else if (PCIE_DRVSTATE_INIT != PCIE_DrvRWDev[hwId].state) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
#endif
            do {
                data = *PCIE_LinkReg;
                data = (data & 0x10UL);
                counter--;
                BCM_DelayUs(10UL);
            } while ((data == 0UL) &&  (counter > 0));
            if(data > 0UL) {
                *alinkStatus = 1UL;
            } else {
                *alinkStatus = 0UL;
            }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}

/**
    @brief PCIE configuration read

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aAddress    Configuration register address
    @param[out]  aValue       Value to be read from configuration register space.

    @returns void

    @post None

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_DRV_CFG_READ_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    Set the address of configuration address space
    read the data from indirect block
    @endcode
*/
void PCIE_DrvCfgRead(uint32_t aRegAddr,
                          uint32_t *aValue)
{
    /* Cfg Wr */
    *PCIE_CfgAddr = (aRegAddr & 0xFFFUL);
    /* Keep delay to get updated values.*/
    BCM_DelayUs(50UL);
    /* Cfg Rd */
    *aValue = *PCIE_CfgData;
}

/**
    @brief PCIe configuration write

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]   aAddress  Configuration register address
    @param[in]  aValue      Write value to configuration address space

    @returns void

    @post None

    @limitations None

    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_DRV_CFG_WRITE_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    Set the address of configuration address space
    Write the data to indirect block
    @endcode
*/
void PCIE_DrvCfgWrite(uint32_t aRegAddr,
                           uint32_t aValue)
{
    /* Cfg Wr address */
    *PCIE_CfgAddr = (aRegAddr & 0xFFFUL);
    /* Cfg data Write */
    *PCIE_CfgData = aValue;
    /* Keep the delay to complete config write.*/
    BCM_DelayUs(100UL);
}

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
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_GET_PCIE_RST_STATUS_PROC
    @trace #BRCM_SWREQ_PCIE_DRV
    @code{.unparsed}
    Get the PCIe PERST status.
    @endcode

*/
int32_t PCIE_GetPcieRstStatus(PCIE_HwIDType hwId, uint32_t *aPerstStatus)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t data = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
        if (PCIE_HW_ID_MAX <= hwId) {
            retVal = BCM_ERR_INVAL_PARAMS;
        } else if (PCIE_DRVSTATE_INIT != PCIE_DrvRWDev[hwId].state) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
#endif
           data = PCIE_REG_EP_COMMON->pcie_stat_0;
           if (PCIE_DrvRWDev[hwId].controllerMode == PCIE_CONTROLLER_MODE_EP) {
               *aPerstStatus = ((data & PCIE_COMMON_PCIE_STAT_32BIT_0_EP_MODE_PERST_MASK) >> PCIE_COMMON_PCIE_STAT_32BIT_0_EP_MODE_PERST_SHIFT);
           } else {
               *aPerstStatus = ((data & PCIE_COMMON_PCIE_STAT_32BIT_0_RC_MODE_PERST_MASK) >> PCIE_COMMON_PCIE_STAT_32BIT_0_RC_MODE_PERST_SHIFT);
           }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return retVal;
}


/** @} */

