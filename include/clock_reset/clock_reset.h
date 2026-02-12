/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_clkrstdrv_ifc Clock Reset
    @ingroup grp_clkrstdrv

    @addtogroup grp_clkrstdrv_ifc
    @{
    @section sec_clkrst_overview Overview
    -# The Clock and reset module used to configure system PLL clock and resets based on application specific needs.
       It provides layer 1 APIs to configure the PLL dividers in desired configuration. It is configured
       in non-blocking mode and APIs provided are non-reenterant.
    @note Please refer to respective technical reference manual for more details about the HW features.

    -# HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   Instance 0  |

    @section api_list List of Clock and reset APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref CLKRST_M7Reset              | M7 reset control|
    | @ref CLKRST_Q8Reset              | Q8 reset control|
    | @ref CLKRST_A55Reset             | A55 Reset Control configuration |
    | @ref CLKRST_SystemClockDivConfig | System Clock divider configuration |

    @file clock_reset.h
    @brief Interface for clock and reset driver
    @section apis This section defines the interface for clock reset driver

    @version 0.1 Initial Version
*/

#ifndef CLOCK_RESET_H
#define CLOCK_RESET_H

#include <stdint.h>

/**
    @name clock and reset driver API IDs
    @{
    @brief API IDs for clock and reset driver
 */
#define BRCM_SWARCH_CLKRST_HW_ID_TYPE                          (0x8A11U)    /**< @brief #CLKRST_HwIDType                  */
#define BRCM_SWARCH_CLKRST_M7_RESET_TYPE                       (0x8A12U)    /**< @brief #CLKRST_M7ResetType               */
#define BRCM_SWARCH_CLKRST_Q8_RESET_TYPE                       (0x8A13U)    /**< @brief #CLKRST_Q8ResetType               */
#define BRCM_SWARCH_CLKRST_A55_RESET_TYPE                      (0x8A14U)    /**< @brief #CLKRST_A55ResetType              */
#define BRCM_SWARCH_CLKRST_M7_RESETCONTROL_TYPE                (0x8A16U)    /**< @brief #CLKRST_M7ResetControlType        */
#define BRCM_SWARCH_CLKRST_Q8_RESETCONTROL_TYPE                (0x8A17U)    /**< @brief #CLKRST_Q8ResetControlType        */
#define BRCM_SWARCH_CLKRST_A55_RESET_CTRL_TYPE                 (0x8A18U)    /**< @brief #CLKRST_A55ResetControlType       */
#define BRCM_SWARCH_CLKRST_SYSTEM_CLOCK_TYPE                   (0x8A1AU)    /**< @brief #CLKRST_SystemClockType           */
#define BRCM_SWARCH_CLKRST_SYSTEM_CLOCK_CTRL_TYPE              (0x8A1BU)    /**< @brief #CLKRST_SystemClockControlType    */

#define BRCM_SWARCH_CLKRST_M7_RESET_PROC                       (0x8A1CU)    /**< @brief #CLKRST_M7Reset                   */
#define BRCM_SWARCH_CLKRST_Q8_RESET_PROC                       (0x8A1DU)    /**< @brief #CLKRST_Q8Reset                   */
#define BRCM_SWARCH_CLKRST_A55_RESET_PROC                      (0x8A1EU)    /**< @brief #CLKRST_A55Reset                  */
#define BRCM_SWARCH_CLKRST_SYSTEM_CLOCK_DIV_CONFIG_PROC        (0x8A20U)    /**< @brief #CLKRST_SystemClockDivConfig      */
/** @} */

/**
    @brief Index of the controller

    @trace #BRCM_SWREQ_CLKRST_CONFIG
 */
typedef uint32_t CLKRST_HwIDType;

/**
    @name CLKRST_M7ResetType
    @{
    @brief M7 Reset type

    @trace #BRCM_SWREQ_CLKRST_CONFIG
 */

typedef uint32_t CLKRST_M7ResetType;              /**< @brief M7 Reset type */
#define CLKRST_M7_RESET_MCU1_RESETB       (1UL)   /**< @brief ResetB for MCU1 in CPU subsystem*/
#define CLKRST_M7_RESET_DEBUG_PRT_RESETB  (2UL)   /**< @brief ResetB for debug access ports of MCU0 and MCU1 in CPU subsystem*/
/** @} */

/**
    @name CLKRST_M7ResetControlType
    @{
    @brief M7 reset control

    @trace #BRCM_SWREQ_CLKRST_CONFIG
 */
typedef struct sCLKRST_M7ResetControlType {   /**< @brief  M7 reset control type*/
    CLKRST_M7ResetType resetType;             /**< @brief  M7 reset type*/
    uint32_t           resetValue;            /**< @brief  0- Force Reset assertion 1 - Force Reset de-assertion
                                                           2 - Toggle (0 followed by 1)  */
} CLKRST_M7ResetControlType;
/** @} */

/**
    @name CLKRST_A55ResetType
    @{
    @brief A55 Reset type

    @trace #BRCM_SWREQ_CLKRST_CONFIG
 */
typedef uint32_t CLKRST_A55ResetType;                   /**< @brief A55 Reset type */
#define CLKRST_A55_RESET_HRESET                 (0UL)   /**< @brief HRESET for the AHB bridges in A55 subsystem */
#define CLKRST_A55_RESET_PRESET                 (1UL)   /**< @brief PRESETn for the AHB bridges in A55 subsytem*/
#define CLKRST_A55_RESET_RESETN                 (2UL)   /**< @brief ARM resetn for A55_PROC */
#define CLKRST_A55_RESET_DEBUG_PRT_RESETN       (3UL)   /**< @brief RESETN for the debug access port in A55 subsystem*/
#define CLKRST_A55_RESET_1588_TMR_RESET         (4UL)   /**< @brief ARM 1588 timer RESETN for A55 subsystem*/
/** @} */

/**
    @name CLKRST_A55ResetControlType
    @{
    @brief A55 reset control

    @trace #BRCM_SWREQ_CLKRST_CONFIG
 */
typedef struct sCLKRST_A55ResetControlType {  /**< @brief A55 Reset control type */
    CLKRST_A55ResetType resetType;            /**< @brief A55 reset type */
    uint32_t            resetValue;           /**< @brief 0- Force Reset assertion 1 - Force Reset de-assertion
                                                          2 - Toggle (0 followed by 1)  */
} CLKRST_A55ResetControlType;
/** @} */

/**
    @name CLKRST_Q8ResetType
    @{
    @brief Q8 Reset type

    @trace #BRCM_SWREQ_CLKRST_CONFIG
 */
typedef uint32_t CLKRST_Q8ResetType;                   /**< @brief Q8 Reset type */
#define CLKRST_Q8_RESET_HRESET                 (0UL)   /**< @brief HRESETn for all Q8 instance*/
#define CLKRST_Q8_RESET_PRESET                 (1UL)   /**< @brief APB Reset for the specified(using hwID) Q8 instance*/
#define CLKRST_Q8_RESET_CRESETB                (2UL)   /**< @brief CRESET for the specified (using hwID) Q8 instance*/
#define CLKRST_Q8_RESET_APB_SLAVE_RESET        (3UL)   /**< @brief APBSlave Reset for the specified Q8 instance*/
/** @} */

/**
    @name CLKRST_Q8ResetControlType
    @{
    @brief Q8 reset control

    @trace #BRCM_SWREQ_CLKRST_CONFIG
 */
typedef struct sCLKRST_Q8ResetControlType {  /**< @brief Q8 Reset control type */
    CLKRST_Q8ResetType resetType;            /**< @brief  Q8 reset type*/
    uint32_t           resetValue;           /**< @brief  0 - Force Reset assertion 1 - Force Reset de-assertion
                                                          2 - Toggle (0 followed by 1)  */
} CLKRST_Q8ResetControlType;
/** @} */


/**
    @name CLKRST_SystemClockType
    @{
    @brief System Clock type

    @trace #BRCM_SWREQ_CLKRST_SYSCLK_CONFIG
 */
typedef uint32_t CLKRST_SystemClockType;                                          /**< @brief System Clock type */
#define CLKRST_SYSTEM_CLOCK_CPU_CLK_M7_MCU1_DIV                           (5UL)   /**< @brief System CPU PLL clock out 1 - M7 MCU1 */
#define CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_AHB_CLK_DIV                 (6UL)   /**< @brief System Bus PLL clock out 0 - Global AHB clock */
#define CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_PERI_AHB_CLK_DIV            (7UL)   /**< @brief System Bus PLL clock out 0 - Global peripheral AHB clock */
#define CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_Q8_APB_CLK_DIV              (8UL)   /**< @brief System Bus PLL clock out 0 - Global Q8 APB clock*/

#define CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_CLK_DIV                            (0UL)   /**< @brief System Bus PLL clock out 0*/
#define CLKRST_SYSTEM_CLOCK_CPU_CLK_DIV                                   (1UL)   /**< @brief System CPU PLL clock out 1*/
#define CLKRST_SYSTEM_CLOCK_Q8_CORE_CLK_DIV                               (3UL)   /**< @brief System Q8 PLL clock out4  */
#define CLKRST_SYSTEM_CLOCK_CAN_PHY_CLK_DIV                               (4UL)   /**< @brief System CAN PHY PLL clock out5   */
/** @} */

/**
    @name CLKRST_SystemClockControlType
    @{
    @brief Clock Type and its supported dividers
       | clockType                                              | divRatio            |
       |:------------------------------------------------------:|:-------------------:|
       | CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_CLK_DIV                 |   0 to 15           |
       | CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_AHB_CLK_DIV      |   2,3,4             |
       | CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_PERI_AHB_CLK_DIV |   2,3,4             |
       | CLKRST_SYSTEM_CLOCK_GLOBAL_BUS_GLOBAL_Q8_APB_CLK_DIV   |   2,3             |
       | CLKRST_SYSTEM_CLOCK_CPU_CLK_DIV                        |   0 to 15           |
       | CLKRST_SYSTEM_CLOCK_CPU_CLK_M7_MCU1_DIV                |   1 to 2 of MCU0 clock|
       | CLKRST_SYSTEM_CLOCK_Q8_CORE_CLK_DIV                    |   0 to 15           |
       | CLKRST_SYSTEM_CLOCK_CAN_PHY_CLK_DIV                    |   0 to 15           |
    @trace #BRCM_SWREQ_CLKRST_SYSCLK_CONFIG
 */
typedef struct sCLKRST_SystemClockControlType { /**< @brief System clock control type */
    CLKRST_SystemClockType clockType;           /**< @brief Type of clock to be configured */
    uint32_t               divRatio;            /**< @brief Divider ratio*/
} CLKRST_SystemClockControlType;
/** @} */

/** @brief M7 reset

    This API controls various M7 resets.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwID               Index of the M7
    @param[in]  m7ResetControl     M7 reset control

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK               M7 reset configured
    @retval     #BCM_ERR_INVAL_PARAMS     Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CLKRST_CONFIG

    @limitations None
*/
int32_t CLKRST_M7Reset(CLKRST_HwIDType hwId,  CLKRST_M7ResetControlType m7ResetControl);

/** @brief Q8 reset

    This API controls various Q8 resets.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwID               Instance of Q8 (0-3)
    @param[in]  Q8ResetControl     Q8 reset control

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK               Q8 reset configured
    @retval     #BCM_ERR_INVAL_PARAMS     Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CLKRST_CONFIG

    @limitations None
*/
int32_t CLKRST_Q8Reset(CLKRST_HwIDType hwId,  CLKRST_Q8ResetControlType q8ResetControl);

/** @brief A55 reset

    This API controls various A55 resets.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwID               Instance of A55
    @param[in]  a55ResetControl    A55 reset control

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              A55 reset configured
    @retval     #BCM_ERR_INVAL_PARAMS    Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CLKRST_CONFIG

    @limitations None
*/
int32_t CLKRST_A55Reset(CLKRST_HwIDType hwId,  CLKRST_A55ResetControlType a55ResetControl);

/** @brief System clock  divider config

    This API configures clock divider of system clock.
    System PLL is derived from 6GHz fixed VCO.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwID                   Instance of HW IP
    @param[in]  systemClockConfig      System clock configuration

    @return     int32_t
    Return values are documented in reverse-chronological order

    @retval     #BCM_ERR_OK               System clock configured
    @retval     #BCM_ERR_INVAL_PARAMS     Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_CLKRST_SYSCLK_CONFIG

    @limitations None
*/
int32_t CLKRST_SystemClockDivConfig(CLKRST_HwIDType hwId, CLKRST_SystemClockControlType systemClockConfig);

#endif /* CLOCK_RESET_H */

/** @} */
