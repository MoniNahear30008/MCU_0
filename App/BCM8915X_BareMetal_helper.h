/*****************************************************************************
 *  Copyright 2024 Broadcom Corporation.  All rights reserved.
 *
 *  Broadcom Proprietary and Confidential
 ****************************************************************************/

#ifndef BCM8915X_BAREMETAL_HELPER_H 
#define BCM8915X_BAREMETAL_HELPER_H 
#include<bcm_err.h>
#include<bcm_time.h>
#include <cmsis_gcc.h>

#define CHK_RETVAL(x)                                          \
{                                                              \
  if (((x) != BCM_ERR_OK)) { \
    goto err;                                                  \
  }                                                            \
}

#define ASSERT(expr)                          \
    do {                                      \
        if (!(expr)) {                        \
            __disable_irq();                  \
            __BKPT(0);                        \
            while (1);                        \
        }                                     \
    } while (0)

typedef uint16_t BRPHY1GV2_TEST_TC10_CONTROL_REG1_TYPE;
#define BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_MASK (0x4U)
#define BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_SHIFT (2U)

typedef uint16_t BRPHY1GV2_TEST_TC10_STATUS_REG1_TYPE;
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_UNUSED_MASK (0xf000U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_UNUSED_SHIFT (12U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_WAKEUP_STAT_MASK (0x800U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_WAKEUP_STAT_SHIFT (11U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_SLP_WUP_STATE_MASK (0x700U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_SLP_WUP_STATE_SHIFT (8U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_WURFAIL_LAT_MASK (0x80U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_WURFAIL_LAT_SHIFT (7U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_AFE_WVWAKE_LAT_MASK (0x40U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_AFE_WVWAKE_LAT_SHIFT (6U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_WAKEUP_LAT_MASK (0x20U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_WAKEUP_LAT_SHIFT (5U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_INHIBIT_MASK (0x10U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_INHIBIT_SHIFT (4U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_INHIBIT_LAT_MASK (0x8U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_INHIBIT_LAT_SHIFT (3U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_SLEEP_LAT_MASK (0x4U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_SLEEP_LAT_SHIFT (2U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_SLEEPFAIL_LAT_MASK (0x2U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_SLEEPFAIL_LAT_SHIFT (1U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_AFE_WVMDI_LAT_MASK (0x1U)
#define BRPHY1GV2_TEST_TC10_STATUS_REG1_AFE_WVMDI_LAT_SHIFT (0U)

typedef uint16_t BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_TYPE;
#define BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_WAKE_BY_WUP_CF_MASK (0x10U)
#define BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_WAKE_BY_WUP_CF_SHIFT (4U)

#define BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEP_INTR_MASK  (0x0020U)
#define BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK  (0x0010U)


typedef uint8_t Helper_BooleanType;
#define HELPER_BOOLEAN_FALSE  (0U) /**< @brief False */
#define HELPER_BOOLEAN_TRUE   (1U) /**< @brief True  */

typedef uint8_t Helper_TC10StateType;           /**< @brief ETHXCVRM TC10 Modes */
#define HELPER_TC10_STATE_START       (0x00U)   /**< @brief Start               */
#define HELPER_TC10_STATE_WAKE_TX     (0x02U)   /**< @brief Wake Transmit       */
#define HELPER_TC10_STATE_WAKETX_DONE (0x03U)   /**< @brief Wake Transmit done  */
#define HELPER_TC10_STATE_AWAKE       (0x04U)   /**< @brief Awake               */
#define HELPER_TC10_STATE_ASSERT_PSD  (0x05U)   /**< @brief Assert PSD signal to trigger psd_out going low */
#define HELPER_TC10_STATE_SLEEP       (0x06U)   /**< @brief Sleep/DeepSleep mode  */

typedef uint8_t Helper_TC10ModeType;            /**< @brief ETHXCVRM TC10 Modes  */
#define HELPER_TC10_MODE_NORMAL1      (0x00U)   /**< @brief TC10 is enabled      */
#define HELPER_TC10_MODE_NORMAL2      (0x01U)   /**< @brief TC10 is disabled     */
#define HELPER_TC10_MODE_HOST         (0x02U)   /**< @brief After power-up device will go to Host mode, (all registers are accessible) */
#define HELPER_TC10_MODE_DEEPSLEEP    (0x03U)   /**< @brief After power-up device will go to DEEPSLEEP mode */

/**
    @name ETHXCVR_WakeupReasonType
    @brief Defines the transceiver wake up reasons
    @{
    @trace #BRCM_SWREQ_ETHXCVR_COMMON
*/
typedef uint8_t Helper_WakeupReasonType;
#define HELPER_WAKEUPREASON_NONE     (0U) /**< @brief No WakeUp Detected */
#define HELPER_WAKEUPREASON_UNKNOWN  (1U) /**< @brief WakeUp Happened But No wake up reason detected */
#define HELPER_WAKEUPREASON_REMOTE   (2U) /**< @brief WakeUp Request Received From the Remote PHY   */
#define HELPER_WAKEUPREASON_LOCAL    (3U) /**< @brief WakeUp Request Asserted By Local PHY   */
#define HELPER_WAKEUPREASON_POWER_ON (4U) /**< @brief Power on wake up detected   */
#define HELPER_WAKEUPREASON_PIN      (5U) /**< @brief Pin wake up detected        */

typedef uint32_t Helper_WakeupEventType;                 /**< @brief Wake-up event types          */
#define HELPER_WAKEUP_EVENT_REMOTE_WAKEUP     (0x01UL)   /**< @brief Wake-up received from remote
                                                                   (Wake-up Pulse/Wake-up request) */
#define HELPER_WAKEUP_EVENT_LOCAL_WAKEUP      (0x02UL)   /**< @brief Wake-up Pulse generated      */
#define HELPER_WAKEUP_EVENT_REMOTE_WAKE_FAIL  (0x04UL)   /**< @brief Wake-up request failed       */
#define HELPER_WAKEUP_EVENT_SLEEP             (0x08UL)   /**< @brief Sleep trigger                */
#define HELPER_WAKEUP_EVENT_SLEEP_FAIL        (0x10UL)   /**< @brief Sleep request failed         */
#define HELPER_WAKEUP_EVENT_INHIBIT           (0x20UL)   /**< @brief Inhibit                      */
#define HELPER_WAKEUP_EVENT_PIN_WAKEUP        (0x40UL)   /**< @brief Wake-up through pin          */
#define HELPER_WAKEUP_EVENT_WAKEUP_TRIGGER    (0x80UL)   /**< @brief Wake-up triggered by self    */

typedef struct sHelper_StatsType {
    uint32_t tc10WakeUpFail;          /**< @brief Wake up fail count                             */
    uint32_t tc10RemoteWakeUpRequest; /**< @brief Remote WakeUp Requests received                */
    uint32_t tc10Inhibit;             /**< @brief Inbihit count                                  */
    uint32_t tc10Sleep;               /**< @brief LPS command reception count                    */
    uint32_t tc10SleepFail;           /**< @brief Sleep Fail count                               */
    uint32_t tc10AfeWakeUpPulse;      /**< @brief AFE WakeUp received                            */
    uint32_t tc10LocalSleepTrigger;   /**< @brief Count of Sleep trigger by the device locally   */
    uint32_t tc10LocalWakeUpTrigger;  /**< @brief Count of WakeUp trigger by the device locally  */
    uint32_t tc10RemoteAfeWakeUp;     /**< @brief Count of AFE WakeUp due to remote              */
    uint32_t tc10LocalAfeWakeUp;      /**< @brief Count of AFE WakeUp due to local trigger       */
} Helper_StatsType;


typedef struct sHelper_StatusType {
    Helper_TC10StateType   tc10State;      /**< @brief TC10 State Machine     */
    Helper_TC10ModeType    tc10Mode;       /**< @brief Mode of TC10 operation */
} Helper_StatusType;

typedef struct sHelper_TC10PortContextType {
    Helper_StatusType       status;              /**< @brief Status information     */
    Helper_StatsType        stats;               /**< @brief Statistics information */
    Helper_WakeupReasonType wakeReason;          /**< @brief Wakeup reason          */
    Helper_BooleanType      isLocalWakeUp;       /**< @brief Boolean is True when wake-up is self triggered, False otherwise */
    Helper_BooleanType      allowMdiWake;        /**< @brief caching the mdi wake config */
} Helper_TC10PortContextType;

extern void reg_rd(uint32_t addr, uint32_t *data);
extern void reg_wr(uint32_t addr, uint32_t data);
extern void reg_rmw(uint32_t addr, uint8_t msb, uint8_t lsb, uint32_t value);
extern int32_t byte_compare(uint8_t *buf1, uint8_t *buf2, uint32_t size);
extern int32_t test_helper_byte_compare(uint8_t *buf1, uint8_t *buf2, uint32_t size);
extern int32_t test_helper_wfi_verify_data(uint8_t *buf1, uint8_t *buf2, uint32_t size, volatile uint8_t *flag);
extern int32_t bringup_hsafe(uint32_t hsafe_id, uint32_t hsacq_cmn_id, uint8_t hsadc_5g_mode, uint8_t ocpClkSel, uint8_t fftClkSel);
extern int32_t cfg_gpio_alt_fn_as_uart0();
extern int32_t cfg_gpio_alt_fn_as_uart1();
extern int32_t cfg_gpio_alt_fn_as_i2c0();
extern int32_t cfg_gpio_alt_fn_as_i2c1();
extern int32_t cfg_gpio_alt_fn_as_can();
extern int32_t cfg_gpio_alt_fn_as_ccu0_etr();
extern int32_t cfg_gpio_alt_fn_as_ccu0_in0();
extern int32_t cfg_gpio_alt_fn_as_ccu0_in1();
extern int32_t cfg_gpio_alt_fn_as_ccu1_etr();
extern int32_t cfg_gpio_alt_fn_as_ccu1_in0();
extern int32_t cfg_gpio_alt_fn_as_ccu1_in1();
extern int32_t cfg_gpio_alt_fn_as_ccu0_cout0();
extern int32_t cfg_gpio_alt_fn_as_ccu0_cout1();
extern int32_t cfg_gpio_alt_fn_as_ccu1_cout0();
extern int32_t cfg_gpio_alt_fn_as_ccu1_cout1();
extern void Helper_TC10HandleInterrupts(Helper_WakeupEventType *const aEvents, Helper_TC10PortContextType *aCntxt);
#endif /* BCM8915X_BAREMETAL_HELPER_H */
