/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/**
    @defgroup grp_hsafedrv_ifc HSAFE
    @ingroup grp_hsafedrv

    @addtogroup grp_hsafedrv_ifc
    @{
    @section sec_hsafe_overview Overview
    @note
    -# High speed analog front end Bare Metal driver interface provides layer 1 APIs to initialize, configure
    the HSAFE in desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# Each HSAFE HW instance needs to be initialized separately using HSAFE_DrvInit API.
    -# User should configure HSAFE block before configuring any acquisition block.
    -# HSAFE block supplies clock to acquisition block.

    @note Please refer to respective technical reference manual for more details about the HW features.
    @section sec_hsafedrv_fsm Usage
    -# HSAFE HW instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   HSAFE |

    @section api_list List of HSAFE APIs
    | API Name | Brief Description   |
    |:-----------:|:-------------:|
    | @ref HSAFE_DrvInit | Initialize HSAFE |
    | @ref HSAFE_DrvDeInit | De-initialize HSAFE |
    | @ref HSAFE_DrvPllConfig | PLL configuration|
    | @ref HSAFE_DrvPllClkGenRstb | PLL clock Generation configuration|
    | @ref HSAFE_DrvPllPowerConfig | PLL power configuration|
    | @ref HSAFE_DrvPllReset | PLL reset configuration|
    | @ref HSAFE_DrvPllLockGet | PLL lock get|
    | @ref HSAFE_DrvHsAdcPowerConfig| HSADC power configuration|
    | @ref HSAFE_DrvHsAdcReset | HSADC reset configuration|
    | @ref HSAFE_DrvHsRefPowerConfig | HSREF power configuration|
    | @ref HSAFE_DrvHsRefReset | HSREF reset configuration|
    | @ref HSAFE_DrvHsAdcConfig|HSADC configuration|
    | @ref HSAFE_DrvRccalReset | RC calibration reset|
    | @ref HSAFE_DrvRccalPowerConfig | RC calibration power configuration|
    | @ref HSAFE_DrvRcCalStatus| RC calibration status|
    | @ref HSAFE_DrvRescalResetb| Rescal resetB configuration|
    | @ref HSAFE_DrvRescalPowerConfig| Rescal power configuration|
    | @ref HSAFE_DrvRescalStatus | Res calibration status|
    | @ref HSAFE_DrvAdcClkCfg| ADC clock Configuration|
    | @ref HSAFE_DrvAwgDacClockCfg| AWG/DAV clock configuration|
    | @ref HSAFE_DrvOcpClockCfg|OCP clock configuration|
    | @ref HSAFE_DrvTxslzrReset| Tx serializer reset configuration|
    | @ref HSAFE_DrvTxslzrPowerConfig|Tx serializer power configuration|
    | @ref HSAFE_DrvResCalConfig | Configure resCal value|


    @file hsafe.h
    @brief Interface for HSAFE driver
    @section apis This section defines the interface for HSAFE driver
    @version 0.1 Initial Version
*/

#ifndef HSAFE_H
#define HSAFE_H

#include <stdint.h>

/**
    @name HSAFE driver API IDs
    @{
    @brief API IDs for HSAFE drier
 */
#define BRCM_SWARCH_HSAFE_HW_ID_TYPE                       (0x8601U)    /**< @brief #HSAFE_HwIDType */
#define BRCM_SWARCH_HSAFE_DRV_INIT_PROC                    (0x8602U)    /**< @brief #HSAFE_DrvInit  */
#define BRCM_SWARCH_HSAFE_DRV_DEINIT_PROC                  (0x8603U)    /**< @brief #HSAFE_DrvDeInit  */
#define BRCM_SWARCH_HSAFE_DRV_PLL_CONFIG_PROC              (0x8604U)    /**< @brief #HSAFE_DrvPllConfig*/
#define BRCM_SWARCH_HSAFE_DRV_PLL_CLK_GEN_RSTB_PROC        (0x8605U)    /**< @brief #HSAFE_DrvPllClkGenRstb*/
#define BRCM_SWARCH_HSAFE_DRV_PLL_POWER_CONFIG_PROC        (0x8606U)    /**< @brief #HSAFE_DrvPllPowerConfig*/
#define BRCM_SWARCH_HSAFE_DRV_PLL_RESET_PROC               (0x8607U)    /**< @brief #HSAFE_DrvPllReset*/
#define BRCM_SWARCH_HSAFE_DRV_PLL_LOCK_GET_PROC            (0x8608U)    /**< @brief #HSAFE_DrvPllLockGet*/
#define BRCM_SWARCH_HSAFE_DRV_HSADC_POWER_CONFIG_PROC      (0x8609U)    /**< @brief #HSAFE_DrvHsAdcPowerConfig*/
#define BRCM_SWARCH_HSAFE_DRV_HSADC_RESET_PROC             (0x860AU)    /**< @brief #HSAFE_DrvHsAdcReset*/
#define BRCM_SWARCH_HSAFE_DRV_HSREF_POWERCONFIG_PROC       (0x860BU)    /**< @brief #HSAFE_DrvHsRefPowerConfig*/
#define BRCM_SWARCH_HSAFE_DRV_HSREF_RESET_PROC             (0x860CU)    /**< @brief #HSAFE_DrvHsRefReset*/
#define BRCM_SWARCH_HSAFE_DRV_HSADC_CONFIG_PROC            (0x860DU)    /**< @brief #HSAFE_DrvHsAdcConfig*/
#define BRCM_SWARCH_HSAFE_DRV_RCCAL_RESET_PROC             (0x860EU)    /**< @brief #HSAFE_DrvRccalReset*/
#define BRCM_SWARCH_HSAFE_DRV_RCCAL_POWERCONFIG_PROC       (0x860FU)    /**< @brief #HSAFE_DrvRccalPowerConfig*/
#define BRCM_SWARCH_HSAFE_DRV_RCCAL_STATUS_PROC            (0x8610U)    /**< @brief #HSAFE_DrvRcCalStatus*/
#define BRCM_SWARCH_HSAFE_DRV_RESCAL_RESETB_PROC           (0x8611U)    /**< @brief #HSAFE_DrvRescalResetb*/
#define BRCM_SWARCH_HSAFE_DRV_RESCAL_POWERCONFIG_PROC      (0x8612U)    /**< @brief #HSAFE_DrvRescalPowerConfig*/
#define BRCM_SWARCH_HSAFE_DRV_RESCAL_STATUS_PROC           (0x8613U)    /**< @brief #HSAFE_DrvRescalStatus*/
#define BRCM_SWARCH_HSAFE_DRV_ADC_CLKCFG_PROC              (0x8614U)    /**< @brief #HSAFE_DrvAdcClkCfg*/
#define BRCM_SWARCH_HSAFE_DRV_AWGDAC_CLOCK_CFG_PROC        (0x8615U)    /**< @brief #HSAFE_DrvAwgDacClockCfg*/
#define BRCM_SWARCH_HSAFE_DRV_OCP_CLOCK_CFG_PROC           (0x8616U)    /**< @brief #HSAFE_DrvOcpClockCfg*/
#define BRCM_SWARCH_HSAFE_DRV_TXSLZR_RESET_PROC            (0x8617U)    /**< @brief #HSAFE_DrvTxslzrReset*/
#define BRCM_SWARCH_HSAFE_DRV_TXSLZR_POWERCONFIG_PROC      (0x8618U)    /**< @brief #HSAFE_DrvTxslzrPowerConfig*/
#define BRCM_SWARCH_HSAFE_STATE_TYPE                       (0x8619U)    /**< @brief #HSAFE_StateType */
#define BRCM_SWARCH_HSAFE_DRV_RESCAL_CONFIG_PROC           (0x861AU)    /**< @brief #HSAFE_DrvResCalConfig */


/** @} */

/**
    @brief Index of the HSAFE controller

    @trace #BRCM_SWREQ_HSAFE_INIT
 */
typedef uint32_t HSAFE_HwIDType;

/**
    @name HSAFE_StateType
    @{
    @brief State of HSAFE

    @trace #BRCM_SWREQ_HSAFE_INIT
*/

typedef uint32_t HSAFE_StateType;                /**< @brief HSAFE state type */
#define HSAFE_STATE_UNINIT           (0UL)       /**< @brief HSAFE is uninitialized & value must be zero*/
#define HSAFE_STATE_INITIALIZED      (1UL)       /**< @brief HSAFE is initialized */
/** @} */

/** @brief Initialize HSAFE

    This API initializes the given HSAFE hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller

    @return     int32_t

    @post None

    @trace  #BRCM_SWREQ_HSAFE_INIT

    @limitations None
*/

int32_t HSAFE_DrvInit(HSAFE_HwIDType  hwId);

/** @brief De-initialize HSAFE

    This API de-initializes HSAFE. Resets the given HSAFE controller and put back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSAFE_INIT

    @limitations None
*/
int32_t HSAFE_DrvDeInit (HSAFE_HwIDType  hwId);

/** @brief HSAFE PLL configuration

    This API configures HSAFE PLL.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvPllConfig(HSAFE_HwIDType hwId);

/** @brief HSAFE Clock gen reset

    This API resets main PLL clock generator.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      clkGenRstb   0 - put under reset 1 - Take out of reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid clkGenRstb value
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION
    @limitations None
*/
int32_t HSAFE_DrvPllClkGenRstb(HSAFE_HwIDType hwId, uint8_t clkGenRstb);

/** @brief HSAFE PLL power

    This API powers down or powers up PLL .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      pwrUpDown    0 - Power Down 1 - Power Up

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvPllPowerConfig(HSAFE_HwIDType hwId, uint8_t pwrUpDown);

/** @brief HSAFE PLL Reset

    This API resets the whole PLL .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rstPll       0 - reset whole PLL 1 - Bring PLL out of reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION

    @limitations None
*/

int32_t HSAFE_DrvPllReset(HSAFE_HwIDType hwId, uint8_t rstPll);

/** @brief Get PLL lock status

    This API gets PLL lock status .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[out]     pllLock      PLL lock status

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for pllLock
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_PLL_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvPllLockGet(HSAFE_HwIDType hwId, uint8_t *pllLock);

/** @brief High speed ADC power

    This API configures high speed ADC power based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      adcInstance  HSADC instance [0-3]
    @param[in]      pwr          0 - Disables selected HSADC
                                 1 - Enables selected HSADC

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adcInstance
    @return     #BCM_ERR_INVAL_PARAMS   Invalid pwr
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvHsAdcPowerConfig(HSAFE_HwIDType hwId, uint8_t adcInstance, uint8_t pwr);

/** @brief High speed ADC reset

    This API configures high speed ADC reset based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      adcInstance  HSADC instance [0-3]
    @param[in]      rstAdc       0 - Resets selected HSADC
                                 1 - out of reset selected HSADC

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adcInstance
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rstAdc
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/

int32_t HSAFE_DrvHsAdcReset(HSAFE_HwIDType hwId, uint8_t adcInstance, uint8_t rstAdc);

/** @brief High speed Reference ADC power

    This API configures high speed reference ADC power based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      pwr          0 - Disables selected HSADC
                                 1 - Enables selected HSADC

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid pwr
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/

int32_t HSAFE_DrvHsRefPowerConfig(HSAFE_HwIDType hwId, uint8_t pwr);

/** @brief High speed reference ADC reset

    This API configures high speed reference ADC reset based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rstAdc       0 - Resets selected HSADC
                                 1 - out of reset selected HSADC

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rstAdc
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvHsRefReset(HSAFE_HwIDType hwId, uint8_t rstAdc);

/** @brief High speed ADC configuration

    This API configures high speed ADC clock based on ADC instance .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      adcInstance  HSADC instance [0-3]
    @param[in]      adcMode      0 - 5G
                                 1 - 1.25G

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adcInstance
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adcMode
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvHsAdcConfig(HSAFE_HwIDType hwId, uint8_t adcInstance, uint8_t adcMode);

/** @brief Calibration reset

    This API resets the RC calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rccalReset   1 : RCCAL under reset 0: RCCAL release reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rccalReset
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION

    @limitations None
*/
int32_t HSAFE_DrvRccalReset(HSAFE_HwIDType hwId, uint8_t rccalReset);

/** @brief Bias power configuration

    This API configures bias power.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      power        0 : Power Down Bias 1 : Power Up Bias

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid power
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION

    @limitations None
*/
int32_t HSAFE_DrvRccalPowerConfig(HSAFE_HwIDType hwId, uint8_t power);

/** @brief RC calibration status

    This API gets RC calibration completion state.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[out]     calibStat    1 : calibration done
                                 0: calibration not completed

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for calibStat
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION

    @limitations None
*/
int32_t HSAFE_DrvRcCalStatus(HSAFE_HwIDType hwId, uint8_t *calibStat);

/** @brief Rescal resetB

    This API resets Resistor calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rescalReset  0 : Reset Rescal 1: bring Rescal out of reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rescalReset
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION

    @limitations None
*/
int32_t HSAFE_DrvRescalResetb(HSAFE_HwIDType hwId, uint8_t rescalReset);

/** @brief Rescal power configuration

    This API configures Rescal power.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      rescalPower  0 : power down Rescal 1: power up Rescal

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rescalPower
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION

    @limitations None
*/
int32_t HSAFE_DrvRescalPowerConfig(HSAFE_HwIDType hwId, uint8_t rescalPower);

/** @brief Get Resistor calibration status

    This API gets Resistor calibration status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[out]     rescalStatus 1 : Res calibration completed
                                 0 : Res calibration not done
    @param[out]     valid        0 : Res calibration not valid
                                 1 : Res calibration valid
    @param[out]     value        Res calibration value output

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for rescalStatus
    @return     #BCM_ERR_INVAL_PARAMS   Invalid memory for valid
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_CALIBRATION

    @limitations None
*/
int32_t HSAFE_DrvRescalStatus(HSAFE_HwIDType hwId, uint8_t *rescalStatus, uint8_t *valid, uint8_t *value);

/** @brief Enables or Disables ADC Clock

    This API Enables or Disables ADC clock .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSAFE controller
    @param[in]      enDis        0 - Disables ADC clock 1 - Enables ADC clock

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid enDis value
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @limitations None
*/
int32_t HSAFE_DrvAdcClkCfg(HSAFE_HwIDType hwId, uint8_t enDis);

/** @brief Enables or Disables AWG/HKDAC Clock

    This API Enables or Disables AWG/HKDAC clock .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSAFE controller
    @param[in]      clkSel        0 : 100MHz  1 : 125MHz 2 : 200MHz
                                  3 : 250MHz 4 : 156.25MHz 6 : 312.5MHz
    @param[in]      clkgenEn      0 - Disables AWG/DAC clock 1 - Enables AWG/DAC clock

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid clkSel
    @return     #BCM_ERR_INVAL_PARAMS   Invalid clkgenEn
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvAwgDacClockCfg(HSAFE_HwIDType hwId, uint8_t clkSel, uint8_t clkgenEn);

/** @brief Clockrate for OCP clock

    This API configures OCP clock rate for HSADC and HSREF.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId              Index of the HSAFE controller
    @param[in]      hsadcOcpClkSel    0 : ck16 (625Mhz) 1 : ck20 (500MHz) 2 : ck24 (416.67MHz) 3 : ck32 (312.5MHz)
    @param[in]      hsrefOcpClkSel    0 : ck32 (312.5MHz) 1 : ck24 (416.67MHz) 2 : ck20 (500MHz) 3 : no clock

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hsadcOcpClkSel
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hsrefOcpClkSel
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvOcpClockCfg(HSAFE_HwIDType hwId, uint8_t hsadcOcpClkSel, uint8_t hsrefOcpClkSel);

/** @brief Configure Tx serializer Reset

    This API configures Tx serializer Reset .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId       Index of the HSAFE controller
    @param[in]      resetb     0: Assert TX reset 1: De-assert TX reset

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION
    @limitations None
*/
int32_t HSAFE_DrvTxslzrReset(HSAFE_HwIDType hwId, uint8_t resetb);

/** @brief Configure Tx serializer power

    This API Enables/Disables Tx serializer power configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId       Index of the HSAFE controller
    @param[in]      power      0: power Down 1: power up

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid power value
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvTxslzrPowerConfig(HSAFE_HwIDType hwId, uint8_t power);

/** @brief Configure Rescal value

    This API configures rescal value going to HSADC and Tx serializer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSAFE controller
    @param[in]      rescalValue   Rescal value going to HSADC and Tx serializer

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid power value
    @return     #BCM_ERR_NODEV          HSAFE instance not initialized

    @post None

    @trace  #BRCM_SWREQ_HSAFE_ACQ_CONFIGURATION

    @limitations None
*/
int32_t HSAFE_DrvResCalConfig(HSAFE_HwIDType hwId, uint8_t rescalValue);

#endif /* HSAFE_H */

/** @} */
