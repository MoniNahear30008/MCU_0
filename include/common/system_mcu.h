/*****************************************************************************
 Copyright 2017-2024 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_system_mcu_ifc MCU interface
    @ingroup grp_system_abstract

    @addtogroup grp_system_mcu_ifc
    @{

    @file system_mcu.h
    @brief MCU interface

    @version 0.1 Initial version
*/

#ifndef SYSTEM_MCU_H
#define SYSTEM_MCU_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_comp.h>
#include <bcm_utils.h>

/**
    @name MCU API IDs
    @{
    @brief API IDs for MCU
*/
#define BRCM_SWARCH_MCU_GETVERSION_PROC             (0x8000U)    /**< @brief #MCU_GetVersion            */
#define BRCM_SWARCH_MCU_GETVERSION_EXT_PROC         (0x8001U)    /**< @brief #MCU_GetVersionExt         */

#define BRCM_SWARCH_MCU_VERSION_TYPE                (0x8010U)    /**< @brief #MCU_VersionType           */
#define BRCM_SWARCH_MCU_VERSION_EXT_TYPE            (0x8011U)    /**< @brief #MCU_VersionExtType        */
#define BRCM_SWARCH_MCU_DEVICE_TYPE                 (0x8012U)    /**< @brief #MCU_DeviceType            */
#define BRCM_SWARCH_MCU_CHIP_VERSION_MACRO          (0x8013U)    /**< @brief #MCU_CHIP_VERSION_A0       */
#define BRCM_SWARCH_MCU_INFO_TYPE                   (0x8014U)    /**< @brief #MCU_InfoType              */
#define BRCM_SWARCH_MCU_SPEED_TYPE                  (0x8015U)    /**< @brief #MCU_SpeedType             */
#define BRCM_SWARCH_MCU_SECURITY_MODE_TYPE          (0x8016U)    /**< @brief #MCU_SecurityModeType      */
#define BRCM_SWARCH_MCU_CHIP_MODEL_TYPE             (0x8017U)    /**< @brief #MCU_ModelType             */
#define BRCM_SWARCH_MCU_ETH_INVALID_PORT_MACRO      (0x8018U)    /**< @brief #MCU_ETH_INVALID_PORT      */
#define BRCM_SWARCH_MCU_ETH_PORTMAP_SIZE_MAX_MACRO  (0x8019U)    /**< @brief #MCU_ETH_PORTMAP_SIZE_MAX  */
#define BRCM_SWARCH_MCU_ETHUTILS_PORTMAP_TYPE       (0x801AU)    /**< @brief #MCU_EthUtilsPortMapType   */
#define BRCM_SWARCH_MCU_CHIP_REV_TYPE               (0x801BU)    /**< @brief #MCU_ChipRevisionType      */
#define BRCM_SWARCH_MCU_CHIP_FAMILY_TYPE            (0x801CU)    /**< @brief #MCU_ChipFamilyType        */

#define BRCM_SWARCH_MCU_TIMEOUT_FAULT_TYPE          (0x8020U)    /**< @brief #MCU_TimeoutFaultType      */
#define BRCM_SWARCH_MCU_MEMORY_FAULT_TYPE           (0x8021U)    /**< @brief #MCU_MemoryFaultType       */
#define BRCM_SWARCH_MCU_MISC_FAULT_TYPE             (0x8022U)    /**< @brief #MCU_MiscFaultType         */

/** @} */

/**
    @name MCU Chip model
    @{
    @brief CHIP Model macros
    @trace #BRCM_SWREQ_MCU
*/
typedef uint32_t MCU_ModelType;
#define MCU_CHIP_MODEL_MASK  (0xFFFFFUL)
#define MCU_CHIP_MODEL_8908X (0x89080UL)
#define MCU_CHIP_MODEL_8910X (0x89100UL)
#define MCU_CHIP_MODEL_89158 (0x89158UL)
#define MCU_CHIP_MODEL_8956X (0x89560UL)
#define MCU_CHIP_MODEL_89571 (0x89571UL)
#define MCU_CHIP_MODEL_89572 (0x89572UL)
#define MCU_CHIP_MODEL_89581 (0x89581UL)
#define MCU_CHIP_MODEL_89586 (0x89586UL)
#define MCU_CHIP_MODEL_89587 (0x89587UL)
/** @} */

/**
    @name MCU Chip Family Type
    @{
    @brief CHIP Family macros
    @trace #BRCM_SWREQ_MCU
*/
typedef uint32_t MCU_ChipFamilyType;
#define MCU_CHIP_FAMILY_8908X_MASK  (0xFUL)
#define MCU_CHIP_FAMILY_8908X (0xEUL)
#define MCU_CHIP_FAMILY_8910X_MASK  (0xFUL)
#define MCU_CHIP_FAMILY_8910X (0x6UL)
#define MCU_CHIP_FAMILY_MASK  (0xFFFF0UL)
#define MCU_CHIP_FAMILY_8956X (0x89560UL)
#define MCU_CHIP_FAMILY_8957X (0x89570UL)
#define MCU_CHIP_FAMILY_8958X (0x89580UL)
#define MCU_CHIP_FAMILY_8915X (0x89150UL)
/** @} */

/**
    @name MCU Chip revision Type
    @{
    @brief MCU Chip revision number
    @trace #BRCM_SWREQ_MCU
*/
typedef uint32_t MCU_ChipRevisionType;
#define MCU_CHIP_REVISION_A0    (0UL)   /**< @brief MCU Revision A0 */
#define MCU_CHIP_REVISION_B0    (1UL)   /**< @brief MCU Revision B0 */
#define MCU_CHIP_REVISION_B1    (2UL)   /**< @brief MCU Revision B1*/
#define MCU_CHIP_REVISION_C0    (2UL)   /**< @brief MCU Revision C0 */
/** @} */

/**
    @name MCU revision number (legacy)
    @{
    @brief MCU Chip revision number
    @note These macros have been retained for legacy reasons.
    @trace #BRCM_SWREQ_MCU
*/
#define MCU_CHIP_VERSION_A0     (MCU_CHIP_REVISION_A0)  /**< @brief MCU Revision A0 */
#define MCU_CHIP_VERSION_B0     (MCU_CHIP_REVISION_B0)  /**< @brief MCU Revision B0 */
#define MCU_CHIP_VERSION_C0     (MCU_CHIP_REVISION_C0)  /**< @brief MCU Revision C0 */
/** @} */

/**
   @name Device Master Slave ID
   @{
   @trace #BRCM_SWREQ_MCU
*/
typedef uint32_t MCU_DeviceType;
#define MCU_DEVICE_MASTER          (0UL)  /**< @brief Device master mode */
#define MCU_DEVICE_SLAVE_1         (1UL)  /**< @brief Device ID slave_1 mode */
#define MCU_DEVICE_SLAVE_2         (2UL)  /**< @brief Device ID slave_2 mode */
/** @} */

/**
   @name Stacking port speed in Mbps
   @{
   @trace #BRCM_SWREQ_MCU
 */
typedef uint32_t MCU_SpeedType;
#define MCU_SPEED_INVALID     (0UL)     /**< @brief Invalid speed */
#define MCU_SPEED_1000MBPS    (1000UL)  /**< @brief 1Gbps speed */
#define MCU_SPEED_2500MBPS    (2500UL)  /**< @brief 2.5Gbps speed */
#define MCU_SPEED_5000MBPS    (5000UL)  /**< @brief 5Gbps speed */
/** @} */

/**
    @name MCU Security Mode Type
    @{
    @brief MCU security modes

    @trace #BRCM_SWREQ_MCU
*/
typedef uint32_t MCU_SecurityModeType;     /**< @brief typedef for MCU Security mode */
#define MCU_SECURITY_MODE_UNKNOWN (0x0UL)  /**< @brief Unknown mode */
#define MCU_SECURITY_MODE_NONE    (0x1UL)  /**< @brief Unsecured */
#define MCU_SECURITY_MODE_ECC     (0x2UL)  /**< @brief Secured through ECC SECP384R */
#define MCU_SECURITY_MODE_ECC384  (0x2UL)  /**< @brief Secured through ECC SECP384R */
#define MCU_SECURITY_MODE_RSA     (0x3UL)  /**< @brief Secured through RSA */
#define MCU_SECURITY_MODE_ECC256  (0x4UL)  /**< @brief Secured through ECC SECP256R */
#define MCU_SECURITY_MODE_MAX     (MCU_SECURITY_MODE_ECC256) /**< @brief Maximum mode value supported */
/** @} */

/**
    @brief Invalid port number

    @trace #BRCM_SWREQ_MCU
*/
#define MCU_ETH_INVALID_PORT     (0xFFUL)

/**
    @brief Size of the PortMap structure

    @trace #BRCM_SWREQ_MCU
*/
#define MCU_ETH_PORTMAP_SIZE_MAX     ((1UL + MCU_DEVICE_SLAVE_1) * 17UL)

/**
    @brief MCU version information structure

    @trace #BRCM_SWREQ_MCU
 */
typedef struct sMCU_VersionType {
    uint32_t             manuf;         /**< @brief manufacturer ID */
    MCU_ModelType        model;         /**< @brief model number */
    MCU_ChipRevisionType rev;           /**< @brief revision number */
    MCU_SecurityModeType securityMode;  /**< @brief Security Mode */
} MCU_VersionType;
BCM_STATIC_SIZE_ASSERT(MCU_VersionType, 16UL, 16UL)

/**
    @brief MCU version Extended information structure
    This holds the chip family information along with the basic chip version.

    @trace #BRCM_SWREQ_MCU
 */
typedef struct sMCU_VersionExtType {
    uint32_t             manuf;         /**< @brief manufacturer ID */
    MCU_ModelType        model;         /**< @brief model number    */
    MCU_ChipRevisionType rev;           /**< @brief revision number */
    MCU_SecurityModeType securityMode;  /**< @brief Security Mode   */
    MCU_ChipFamilyType   family;        /**< @brief family number   */
    uint32_t             rsvd[8UL];
} MCU_VersionExtType;
BCM_STATIC_SIZE_ASSERT(MCU_VersionExtType, 52UL, 52UL)


/**
    @brief Timeout faults

    These faults may occur during bootup and cannot be controlled or disabled.
    This may only be used for status query

    @trace #BRCM_SWREQ_MCU
*/
typedef uint16_t MCU_TimeoutFaultType;
#define MCU_TIMEOUT_FAULT_MEMORY_INIT_MASK      (0x0001U)   /**< @brief Memory initialization timeout mask @xsd name:mem_init;default:1 */
#define MCU_TIMEOUT_FAULT_MEMORY_INIT_SHIFT     (0U)        /**< @brief Memory initialization timeout shift */
#define MCU_TIMEOUT_FAULT_RSVD0_MASK            (0x000EU)   /**< @brief @xsd hex;constant:0x7  */
#define MCU_TIMEOUT_FAULT_RSVD0_SHIFT           (1U)        /**< @brief reserved            */
#define MCU_TIMEOUT_FAULT_OTP0_INIT_MASK        (0x0010U)   /**< @brief Non-seucre OTP initialization timeout mask @xsd name:otp0_init;default:1*/
#define MCU_TIMEOUT_FAULT_OTP0_INIT_SHIFT       (4U)        /**< @brief Non-seucre OTP initialization timeout shift */
#define MCU_TIMEOUT_FAULT_OTP1_INIT_MASK        (0x0020U)   /**< @brief Secure OTP initialization timeout mask @xsd name:otp1_init;default:1    */
#define MCU_TIMEOUT_FAULT_OTP1_INIT_SHIFT       (5U)        /**< @brief Secure OTP initialization timeout shift */
#define MCU_TIMEOUT_FAULT_RSVD1_MASK            (0x00C0U)   /**< @brief @xsd hex;constant:0x3*/
#define MCU_TIMEOUT_FAULT_RSVD1_SHIFT           (6U)        /**< @brief @xsd hide*/
#define MCU_TIMEOUT_FAULT_PLL0_LOCK_MASK        (0x0100U)   /**< @brief PLL Lock timeout mask  @xsd name:pll_lock;default:1 */
#define MCU_TIMEOUT_FAULT_PLL0_LOCK_SHIFT       (8U)        /**< @brief PLL Lock timeout shift  */
#define MCU_TIMEOUT_FAULT_RSVD2_MASK            (0xFE00U)   /**< @brief @xsd hex;constant:0x7F*/
#define MCU_TIMEOUT_FAULT_RSVD2_SHIFT           (9U)        /**< @brief @xsd hide*/

/**
    @brief Memory faults

    Memory related faults that may occur during run time may be controlled.
    However, chip reset behavior defines the default value until software
    configuration is applied.

    @trace #BRCM_SWREQ_MCU
*/
typedef uint16_t MCU_MemoryFaultType;
#define MCU_MEMORY_FAULT_DTCM0_MASK             (0x0001U)   /**< @brief DTCM0 ECC fault mask  @xsd name:dtcm0;default:1 */
#define MCU_MEMORY_FAULT_DTCM0_SHIFT            (0U)        /**< @brief DTCM0 ECC fault shift   */
#define MCU_MEMORY_FAULT_RSVD0_MASK             (0x000EU)   /**< @brief @xsd hex;constant:0x7*/
#define MCU_MEMORY_FAULT_RSVD0_SHIFT            (1U)        /**< @brief @xsd hide */
#define MCU_MEMORY_FAULT_ITCM0_MASK             (0x0010U)   /**< @brief ITCM0 ECC fault mask  @xsd name:itcm0;default:1 */
#define MCU_MEMORY_FAULT_ITCM0_SHIFT            (4U)        /**< @brief ITCM0 ECC fault shift   */
#define MCU_MEMORY_FAULT_RSVD1_MASK             (0xFFE0U)   /**< @brief @xsd hex;constant:0x7FF*/
#define MCU_MEMORY_FAULT_RSVD1_SHIFT            (5U)        /**< @brief @xsd hide */

/**
    @brief Clock faults

    Clock related faults that may occur during run time may be controlled.
    However, chip reset behavior defines the default value until software
    configuration is applied.

    @trace #BRCM_SWREQ_MCU
*/
typedef uint16_t MCU_MiscFaultType;
#define MCU_MISC_FAULT_PLL_LOCK_LOST_MASK   (0x0001U)   /**< @brief PLL lock lost mask @xsd name:pll_lock;default:1 */
#define MCU_MISC_FAULT_PLL_LOCK_LOST_SHIFT  (0U)        /**< @brief PLL lock lost shift */
#define MCU_MISC_FAULT_PVTMON0_MASK         (0x0002U)   /**< @brief VTMON 0 fault mask @xsd name:pvtmon0;default:1 */
#define MCU_MISC_FAULT_PVTMON0_SHIFT        (1U)        /**< @brief VTMON 0 fault shift */
#define MCU_MISC_FAULT_PVTMON1_MASK         (0x0004U)   /**< @brief VTMON 1 fault mask @xsd name:pvtmon1;default:0 */
#define MCU_MISC_FAULT_PVTMON1_SHIFT        (2U)        /**< @brief VTMON 1 fault shift */
#define MCU_MISC_FAULT_RSVD0_MASK           (0x3FF8U)   /**< @brief @xsd hex;constant:0x07FF*/
#define MCU_MISC_FAULT_RSVD0_SHIFT          (3U)        /**< @brief @xsd hide */
#define MCU_MISC_FAULT_WDT0_MASK            (0x4000U)   /**< @brief Processor subsystem watchdog mask  @xsd name:wdt0;default:1 */
#define MCU_MISC_FAULT_WDT0_SHIFT           (14U)       /**< @brief Processor subsystem watchdog shift  */
#define MCU_MISC_FAULT_WDT1_MASK            (0x8000U)   /**< @brief safety monitor watchdog mask  @xsd name:wdt1;default:1      */
#define MCU_MISC_FAULT_WDT1_SHIFT           (15U)       /**< @brief safety monitor watchdog shift       */

/**
    @brief Structure for PortMap

    Structure used to maintain the mapping between unified and
    local port numbers

    @trace #BRCM_SWREQ_MCU
*/
typedef struct sMCU_EthUtilsPortMapType {
    uint8_t num;          /**< @brief Port number */
    uint8_t isStackPort;  /**< @brief Flag indicating whether this is a stack port */
} MCU_EthUtilsPortMapType;
BCM_STATIC_SIZE_ASSERT(MCU_EthUtilsPortMapType, 2UL, 2UL)

/**
    @name MCU information
    @{
    @brief MCU information

    @trace #BRCM_SWREQ_MCU
*/
typedef struct sMCU_InfoType {
    uint8_t            stackingEn; /**< @brief Stacking Enable, 0: Disable, 1: Enable */
    MCU_DeviceType     mstSlvMode; /**< @brief 0: Master, 1: Slave-1                  */
    uint8_t            stackPort0; /**< @brief Stacking Port 0 info                   */
    uint8_t            stackPort1; /**< @brief Stacking Port 1 info                   */
    uint8_t            impPort;    /**< @brief Port number enabled in IMP mode (port8) */
    uint16_t           vlan;       /**< @brief VLAN Id to be used for private communication */
    uint8_t            macAddr[6U];/**< @brief MAC address to be used for private communication */
    uint32_t           ipAddr;     /**< @brief IP address to be used for private communication */
    uint32_t           gwIPAddr;   /**< @brief Gateway IP address to be used for private communication */
    uint32_t           netmask;    /**< @brief Netmask to be used for private communication */
    uint32_t           bootInfo;   /**< @brief 24 bits of boot Information sent along within DHCP/TFTP request*/
    uint32_t           tftpServerAddr;  /**< @brief TFTP server IP address */
    uint8_t            remoteMacAddr[2U][6U];/**< @brief Slave MAC addresses (to be used for private communication) */
    uint8_t            remoteIPAddr[2U][4U]; /**< @brief IP address of slaves (to be used for private communication) */
    MCU_SpeedType      stackPort0Speed; /**< @brief Stacking Port 0 speed in Mbps */
    MCU_SpeedType      stackPort1Speed; /**< @brief Stacking Port 1 speed in Mbps */
    uint8_t            stackPort0CntrlID; /**< @brief Stacking Port 0 controller index */
    uint8_t            stackPort1CntrlID; /**< @brief Stacking Port 1 controller index */
    MCU_EthUtilsPortMapType localPortMap[MCU_ETH_PORTMAP_SIZE_MAX]; /**< @brief Port mapping between unified and local port numbers */
    MCU_EthUtilsPortMapType unifiedPortMap[MCU_ETH_PORTMAP_SIZE_MAX]; /**< @brief Port mapping between local and unified port numbers  */
    BCM_BoolType       prodModeEn; /**< @brief Production mode enabled or disabled */
} MCU_InfoType;
BCM_STATIC_SIZE_ASSERT(MCU_InfoType, 212UL, 212UL)
/** @} */

/** @brief Get MCU version

    This API retrieves the MCU version.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aVersion    Pointer to version structure

    @return void

    @post None

    @trace  #BRCM_SWREQ_MCU

    @limitations None
*/
extern void MCU_GetVersion(MCU_VersionType *const aVersion);

/** @brief Get MCU extended version

    This API retrieves the MCU version along with the chip family information.

    @behavior Sync, Re-entrant

    @pre MCU is initialized

    @param[in]      aVersion    Pointer to version structure

    @return void

    @post None

    @trace  #BRCM_SWREQ_MCU

    @limitations None
*/
extern void MCU_GetVersionExt(MCU_VersionExtType *const aVersion);

#endif /* SYSTEM_MCU_H */
/** @} */
