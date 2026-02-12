/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_hsadcdrv_drv_impl HSADC Driver Design
    @ingroup grp_hsadcdrv

    @addtogroup grp_hsadcdrv_drv_impl
    @{

    @file drivers/hsadc/hsadc_drv.c
    @brief HSADC Driver Design

    @version 0.1 Initial version
*/

#include <hsadc.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <hsadc_rdb.h>
#include <hsadc_intr_rdb.h>

/**
    @name HSADC Driver Design IDs
    @{
    @brief Driver Design IDs for HSADC
*/

#define BRCM_SWDSGN_HSADC_DRV_INIT_PROC                       (0xB101U)    /**< @brief #HSADC_DrvInit                  */
#define BRCM_SWDSGN_HSADC_DRV_DEINIT_PROC                     (0xB102U)    /**< @brief #HSADC_DrvDeInit                */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_SAMPLING_MODE_PROC       (0xB103U)    /**< @brief #HSADC_DrvConfigSamplingMode    */
#define BRCM_SWDSGN_HSADC_DRV_INIT_CALIBRATION_PROC           (0xB104U)    /**< @brief #HSADC_DrvInitCalibration       */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_CAPTURE_MODE_PROC        (0xB105U)    /**< @brief #HSADC_DrvConfigCaptureMode     */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_ACQCONTROLLER_PROC       (0xB106U)    /**< @brief #HSADC_DrvConfigAcqController   */
#define BRCM_SWDSGN_HSADC_DRV_CAPTURE_STATUS_PROC             (0xB107U)    /**< @brief #HSADC_DrvCaptureStatus         */
#define BRCM_SWDSGN_HSADC_DRV_FFT_CONFIG_PROC                 (0xB108U)    /**< @brief #HSADC_DrvFftConfig             */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_READ_DONEADDRESS_PROC    (0xB109U)    /**< @brief #HSADC_DrvConfigReadDoneAddress */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_READ_CAPSIZE_PROC        (0xB10AU)    /**< @brief #HSADC_DrvConfigReadCapSize     */
#define BRCM_SWDSGN_HSADC_DRV_WINDOWMEM_CONFIG_PROC           (0xB10CU)    /**< @brief #HSADC_DrvWindowMemConfig       */
#define BRCM_SWDSGN_HSADC_DRV_WINDOW_CONTROL_PROC             (0xB10DU)    /**< @brief #HSADC_DrvWindowControl         */
#define BRCM_SWDSGN_HSADC_DRV_CLAMP_CONFIG_PROC               (0xB10EU)    /**< @brief #HSADC_DrvClampConfig           */
#define BRCM_SWDSGN_HSADC_DRV_TOFOFFSET_CONFIG_PROC           (0xB10FU)    /**< @brief #HSADC_DrvTofOffsetConfig       */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_FIRCOEFF_PROC            (0xB110U)    /**< @brief #HSADC_DrvConfigFirCoeff        */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_FILTERGAIN_PROC          (0xB111U)    /**< @brief #HSADC_DrvConfigFilterGain      */
#define BRCM_SWDSGN_HSADC_DRV_FILTER_CONTROL_PROC             (0xB112U)    /**< @brief #HSADC_DrvFilterControl         */
#define BRCM_SWDSGN_HSADC_DRV_FFT_CONFIG_AXIS_CAPTURE_PROC    (0xB113U)    /**< @brief #HSADC_DrvFftConfigAxisCapture  */
#define BRCM_SWDSGN_HSADC_DRV_FFT_GET_AXIS_INFORMATION_PROC   (0xB114U)    /**< @brief #HSADC_DrvFftGetAxisInformation */
#define BRCM_SWDSGN_HSADC_DRV_FFT_GET_CALIB_STATUS_PROC       (0xB115U)    /**< @brief #HSADC_DrvFftGetCalibStatus     */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_CH_INTR_PROC             (0xB116U)    /**< @brief #HSADC_DrvConfigChIntr          */
#define BRCM_SWDSGN_HSADC_DRV_CLEAR_CH_INTR_PROC              (0xB117U)    /**< @brief #HSADC_DrvClearChIntr           */
#define BRCM_SWDSGN_HSADC_DRV_GET_CH_INTR_STATUS_PROC         (0xB118U)    /**< @brief #HSADC_DrvGetChIntrStatus       */
#define BRCM_SWDSGN_HSADC_DRV_CONFIG_INTR_PROC                (0xB119U)    /**< @brief #HSADC_DrvConfigIntr            */
#define BRCM_SWDSGN_HSADC_DRV_CLEAR_INTR_PROC                 (0xB11AU)    /**< @brief #HSADC_DrvClearIntr             */
#define BRCM_SWDSGN_HSADC_DRV_GET_INTR_STATUS_PROC            (0xB11BU)    /**< @brief #HSADC_DrvGetIntrStatus         */
#define BRCM_SWDSGN_HSADC_DRV_TRIGGER_CALIB_PROC              (0xB11CU)    /**< @brief #HSADC_DrvTriggerCalib          */
#define BRCM_SWDSGN_HSADC_REGS_GLOBAL                         (0xB11DU)    /**< @brief #HSADC_Regs                     */
#define BRCM_SWDSGN_HSADC_INTR_REGS_GLOBAL                    (0xB11EU)    /**< @brief #HSADC_IntrRegs                 */
#define BRCM_SWDSGN_HSADC_DRV_DEV_TYPE                        (0xB11FU)    /**< @brief #HSADC_DrvDevType               */
#define BRCM_SWDSGN_HSADC_DRV_DEV_GLOBAL                      (0xB120U)    /**< @brief #HSADC_DrvDev                   */
#define BRCM_SWDSGN_HSADC_INT_GET_ALL_CH_INT_STS_PROC         (0xB121U)    /**< @brief #HSADC_IntGetAllChInterruptStatus*/
#define BRCM_SWDSGN_HSADC_INT_GET_CH_EVENT_STS_PROC           (0xB122U)    /**< @brief #HSADC_IntGetChEventStatus*/
#define BRCM_SWDSGN_HSADC_INT_CFG_MEM_INIT_INTR_PROC          (0xB123U)    /**< @brief #HSADC_IntConfigMemInitIntr*/
#define BRCM_SWDSGN_HSADC_INT_HSADC_INT_CLR_MEM_INIT_PROC     (0xB124U)    /**< @brief #HSADC_IntClearMemInit*/
#define BRCM_SWDSGN_HSADC_INT_GET_ALL_CAL_STS_PROC            (0xB125U)    /**< @brief #HSADC_IntGetAllCalibStatus*/
#define BRCM_SWDSGN_HSADC_INT_GET_ALL_INT_STATUS_PROC         (0xB126U)    /**< @brief #HSADC_IntGetAllInterruptStatus*/
#define BRCM_SWDSGN_HSADC_INT_GET_EVENT_STATUS_PROC           (0xB127U)    /**< @brief #HSADC_IntGetEventStatus*/
#define BRCM_SWDSGN_HSADC_INT_GET_MEM_INIT_INT_STS_PROC       (0xB128U)    /**< @brief #HSADC_IntGetMemInitInterruptStatus*/
#define BRCM_SWDSGN_HSADC_DRV_GET_BUF_STS_PROC                (0xB129U)    /**< @brief #HSADC_DrvGetBufStatus          */

/** @} */


/**
 @trace #BRCM_SWREQ_HSADC_INIT
 @trace #BRCM_SWARCH_HSADC_DRV_INIT_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_DEINIT_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_SAMPLING_MODE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_INIT_CALIBRATION_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_CAPTURE_MODE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_ACQCONTROLLER_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CAPTURE_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_READ_DONEADDRESS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_READ_CAPSIZE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_WINDOWMEM_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_WINDOW_CONTROL_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLAMP_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_TOFOFFSET_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_FIRCOEFF_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_FILTERGAIN_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FILTER_CONTROL_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_AXIS_CAPTURE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_GET_AXIS_INFORMATION_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_GET_CALIB_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_CH_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLEAR_CH_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_GET_CH_INTR_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLEAR_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_TRIGGR_CALIB_PROC
*/

static HSADC_RDBType * const HSADC_Regs[HSADC_MAX_HW_ID] =
{
#if (HSADC_MAX_HW_ID == 0U)
#error "HSADC_MAX_HW_ID == 0U"
#endif
#if (HSADC_MAX_HW_ID > 0U)
    (HSADC_RDBType *)HSADC_ACQ_0_BASE,
#endif
#if (HSADC_MAX_HW_ID > 1U)
    (HSADC_RDBType *)HSADC_ACQ_1_BASE,
#endif
#if (HSADC_MAX_HW_ID > 2U)
    (HSADC_RDBType *)HSADC_ACQ_2_BASE,
#endif
#if (HSADC_MAX_HW_ID > 3U)
    (HSADC_RDBType *)HSADC_ACQ_3_BASE,
#endif
#if (HSADC_MAX_HW_ID > 4U)
#error "HSADC_MAX_HW_ID > 4 is not supported by the driver"
#endif
};

/**
    @trace #BRCM_SWREQ_HSADC_INIT
    @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_CH_INTR_PROC
    @trace #BRCM_SWARCH_HSADC_DRV_CLEAR_CH_INTR_PROC
    @trace #BRCM_SWARCH_HSADC_DRV_GET_CH_INTR_STATUS_PROC
    @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_INTR_PROC
    @trace #BRCM_SWARCH_HSADC_DRV_CLEAR_INTR_PROC
    @trace #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC
*/
static HSADC_INTR_RDBType * const HSADC_IntrRegs[HSADC_MAX_HW_ID] =
{
#if (HSADC_MAX_HW_ID == 0U)
#error "HSADC_MAX_HW_ID == 0U"
#endif
#if (HSADC_MAX_HW_ID > 0U)
    (HSADC_INTR_RDBType *)HSADC_INTR_0_BASE,
#endif
#if (HSADC_MAX_HW_ID > 1U)
    (HSADC_INTR_RDBType *)HSADC_INTR_1_BASE,
#endif
#if (HSADC_MAX_HW_ID > 2U)
    (HSADC_INTR_RDBType *)HSADC_INTR_2_BASE,
#endif
#if (HSADC_MAX_HW_ID > 3U)
    (HSADC_INTR_RDBType *)HSADC_INTR_3_BASE,
#endif
#if (HSADC_MAX_HW_ID > 4U)
#error "HSADC_MAX_HW_ID > 4 is not supported by the driver"
#endif

};


/**
 @trace #BRCM_SWREQ_HSADC_INIT
 @trace #BRCM_SWARCH_HSADC_DRV_INIT_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_DEINIT_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_SAMPLING_MODE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_INIT_CALIBRATION_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_CAPTURE_MODE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_ACQCONTROLLER_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CAPTURE_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_READ_DONEADDRESS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_READ_CAPSIZE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_WINDOWMEM_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_WINDOW_CONTROL_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLAMP_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_TOFOFFSET_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_FIRCOEFF_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_FILTERGAIN_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FILTER_CONTROL_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_AXIS_CAPTURE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_GET_AXIS_INFORMATION_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_GET_CALIB_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_CH_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLEAR_CH_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_GET_CH_INTR_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLEAR_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC
*/
typedef struct sHSADC_DrvDevType {
    HSADC_StateType   state;            /**< State of the driver  */
} HSADC_DrvDevType;

/**
 @trace #BRCM_SWREQ_HSADC_INIT
 @trace #BRCM_SWARCH_HSADC_DRV_INIT_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_DEINIT_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_SAMPLING_MODE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_INIT_CALIBRATION_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_CAPTURE_MODE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_ACQCONTROLLER_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CAPTURE_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_READ_DONEADDRESS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_READ_CAPSIZE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_WINDOWMEM_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_WINDOW_CONTROL_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLAMP_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_TOFOFFSET_CONFIG_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_FIRCOEFF_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_FILTERGAIN_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FILTER_CONTROL_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_AXIS_CAPTURE_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_GET_AXIS_INFORMATION_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_FFT_GET_CALIB_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_CH_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLEAR_CH_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_GET_CH_INTR_STATUS_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CONFIG_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_CLEAR_INTR_PROC
 @trace #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC
*/
static HSADC_DrvDevType HSADC_DrvDev[HSADC_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/** @brief Initialize HSADC

    This API initializes the given HSADC hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSADC controller

    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_INIT

    @trace  #BRCM_SWREQ_HSADC_INIT
    @trace  #BRCM_SWARCH_HSADC_DRV_INIT_PROC

    @code{.unparsed}
        check hardware instance with max supported device
        Enable Memory for FFTBUF, MEM, ADCBUF channels
        Enable Memory Check enable for FFTBUF, MEM, ADCBUF
        Release resetb for FFTBUF, MEM, ADCBUF
        Poll init done for FFTBUF, MEM, ADCBUF till set
        if initdone
           state to HSADC_STATE_INITIALIZED state
        else
           state to HSADC_STATE_UNINIT;

    @endcode
    @limitations None
*/

int32_t HSADC_DrvInit(HSADC_HwIDType  hwId)
{
    int ret = BCM_ERR_OK;
    int maxRetry = 0L;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID || (HSADC_DrvDev[hwId].state != HSADC_STATE_UNINIT)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif

        /* Step 1 Enable Memory for HSADC channels*/
        HSADC_Regs[hwId]->mem_init_ctrl = (HSADC_MEM_INIT_CONTROL_ADCBUF_MEM_INIT_EN_MASK |
                HSADC_MEM_INIT_CONTROL_FFTBUF_MEM_INIT_EN_MASK |
                HSADC_MEM_INIT_CONTROL_FFT_MEM_INIT_EN_MASK);

        /* Step2 Check enable*/
        HSADC_Regs[hwId]->mem_init_ctrl |= (HSADC_MEM_INIT_CONTROL_ADCBUF_MEM_INT_CHK_EN_MASK |
                HSADC_MEM_INIT_CONTROL_FFTBUF_MEM_INT_CHK_EN_MASK |
                HSADC_MEM_INIT_CONTROL_FFT_MEM_INT_CHK_EN_MASK);

        /* Step 3 release all resetb*/
        HSADC_Regs[hwId]->mem_init_ctrl |= (HSADC_MEM_INIT_CONTROL_ADCBUF_MEM_RSTB_MASK |
                HSADC_MEM_INIT_CONTROL_FFTBUF_MEM_RSTB_MASK |
                HSADC_MEM_INIT_CONTROL_FFT_MEM_RSTB_MASK);

        /* Step 4 Get the status*/
        maxRetry = 200L;
        do {
            if (((HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr) &
                    (HSADC_INTR_AHMISIE1_ADCBUF_MEM_INIT_DONE_INTR_W2CLR_MASK |
                     HSADC_INTR_AHMISIE1_FFT_MEM_INIT_DONE_INTR_W2CLR_MASK |
                     HSADC_INTR_AHMISIE1_FFTBUF_MEM_INIT_DONE_INTR_W2CLR_MASK)) ==
                    (HSADC_INTR_AHMISIE1_ADCBUF_MEM_INIT_DONE_INTR_W2CLR_MASK |
                     HSADC_INTR_AHMISIE1_FFT_MEM_INIT_DONE_INTR_W2CLR_MASK |
                     HSADC_INTR_AHMISIE1_FFTBUF_MEM_INIT_DONE_INTR_W2CLR_MASK)) {
                HSADC_DrvDev[hwId].state = HSADC_STATE_INITIALIZED;
                break;
            }
        } while ((maxRetry--) > 0); /*Wait till All the memory blocks are initialized*/
        if (maxRetry < 0L) {
            ret = BCM_ERR_UNINIT;
            HSADC_DrvDev[hwId].state = HSADC_STATE_UNINIT;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief De-initialize HSADC

    This API de-initializes HSADC. Resets the given HSADC controller and puts back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSADC controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_UNINIT
    @trace  #BRCM_SWREQ_HSADC_INIT
    @trace  #BRCM_SWARCH_HSADC_DRV_DEINIT_PROC

    @code{.unparsed}
        Check hardware instance with max supported device
        Put FFTBUF, MEM, ADCBUF to reset state
        State to HSADC_STATE_UNINIT;

    @endcode

    @limitations None
*/

int32_t HSADC_DrvDeInit (HSADC_HwIDType  hwId)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* Put back device to reset state*/
        HSADC_Regs[hwId]->mem_init_ctrl &= ~(HSADC_MEM_INIT_CONTROL_ADCBUF_MEM_RSTB_MASK |
                HSADC_MEM_INIT_CONTROL_FFTBUF_MEM_RSTB_MASK |
                HSADC_MEM_INIT_CONTROL_FFT_MEM_RSTB_MASK);

        /* Move state to unInit*/
        HSADC_DrvDev[hwId].state = HSADC_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC configure sampling mode

    This API configures HSADC with specified sampling mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSADC controller
    @param[in]      samplingMode High speed ADC sampling mode
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_SAMPLING_MODE_PROC

    @code{.unparsed}
        Check hardware instance and Sampling mode.
        Write Sampling mode (5G/1.25G) on acq_control0 register
    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigSamplingMode(HSADC_HwIDType  hwId, HSADC_SamplingModeType samplingMode)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t control = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) { /* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (samplingMode >= HSADC_SAMPLING_MODE_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /* Configure sampling mode*/
        control = HSADC_Regs[hwId]->acq_control0;
        control &= (~HSADC_ACQ_CONTROL0_SAMPLING_MODE_MASK);
        control |= ((samplingMode == HSADC_SAMPLING_MODE_5G) ? 2UL : 0UL) << HSADC_ACQ_CONTROL0_SAMPLING_MODE_SHIFT ;
        HSADC_Regs[hwId]->acq_control0 = control;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC Calibration configuration

    This API configures and enables HSADC hardware background or foreground calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      calibConfig    HSADC calibration configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_CALIB_INPROGRESS

    @trace  #BRCM_SWREQ_HSADC_CALIBRATION
    @trace  #BRCM_SWARCH_HSADC_DRV_INIT_CALIBRATION_PROC

    @code{.unparsed}
        Check hardware instance and calibiration config.
        Write calibStateEn on adccal_control0 register
        Write rdbduration on adccal_control1 register
        Write adccal_control2,adccal_control3, adccal_control4 to reset values
    @endcode


    @limitations None
*/
int32_t HSADC_DrvInitCalibration(HSADC_HwIDType  hwId,  HSADC_CalibConfigType *calibConfig)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t control0 = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if(calibConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (calibConfig->calStatesEn > 0xFU) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*Update calState and rdb duration*/
        control0 = HSADC_Regs[hwId]->adccal_control0;
        control0 &= ~(HSADC_ADCCAL_CONTROL0_CAL_STATES_EN_MASK);
        HSADC_Regs[hwId]->adccal_control0 = (control0 | calibConfig->calStatesEn);
        HSADC_Regs[hwId]->adccal_control1 = calibConfig->calBgRdbDuration;

        /* Update Reset Value now, These register will be updated based on
         * lab results - Design team*/
        HSADC_Regs[hwId]->adccal_control2 = 0x31A3UL;
        HSADC_Regs[hwId]->adccal_control3 = 0x880000UL;
        HSADC_Regs[hwId]->adccal_control4 = 0x1UL;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC Triggers Calibration

    This API triggers the start of calibration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post  None

    @trace  #BRCM_SWREQ_HSADC_CALIBRATION
    @trace  #BRCM_SWARCH_HSADC_DRV_TRIGGR_CALIB_PROC

    @code{.unparsed}
        Check hardware instance.
        Write calibration trigger
    @endcode


    @limitations None
*/
int32_t HSADC_DrvTriggerCalib(HSADC_HwIDType  hwId)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* Trigger calibration*/
        HSADC_Regs[hwId]->adccal_control0 |= (HSADC_ADCCAL_CONTROL0_CAL_START_MASK);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;

}


/** @brief HSADC configure capture mode

    This API configures HSADC with specified capture mode.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSADC controller
    @param[in]      captureMode  High speed ADC sampling mode
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_CAPTURE_MODE_PROC

    @code{.unparsed}
         Check hardware instance and captureMode.
         Write captureMode in to acq_control0 register

    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigCaptureMode(HSADC_HwIDType  hwId, HSADC_CaptureModeType captureMode)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acqControl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /*Configure Capture mode*/
        acqControl = HSADC_Regs[hwId]->acq_control0;
        acqControl &= ~(HSADC_ACQ_CONTROL0_CAP_MODE_MASK);
        HSADC_Regs[hwId]->acq_control0 = acqControl | (captureMode);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC configure Acquisition

    This API configures HSADC Acquisition controller .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId             Index of the HSADC controller
    @param[in]      channelEnable   Channel to be enabled,
                                    In 1.25G sampling mode each bit indicated the respective channel. Only 1,3,7 and 15 are valid values.
                                    In 5G sampling mode all 4 bits have to be set.
    @param[in]      decimationRate  Decimation Rate 0U: No decimation  1: decimate by 2  2: decimate by 4
    @param[in]      aggregateCount  Number of captures to aggregate. Range from 1 to 32
    @param[in]      captureSize     Capture size in multiple of 16 samples.max of 32k samples. Each sample is 2 bytes of data. Example: To capture 4K samples program 4096/16 -1 = 255
    @return     int32_t


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_ACQCONTROLLER_PROC

    @code{.unparsed}
         Check hardware instance.
         Write Capture size in to acq_control2 register
         Write Channel Enable decimation rate and aggregation count to acq_control0
    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigAcqController(HSADC_HwIDType  hwId, uint8_t channelEnable, uint8_t decimationRate, uint8_t aggregateCount, uint16_t captureSize)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acqControl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /*Configure capture size*/
        acqControl = HSADC_Regs[hwId]->acq_control2;
        acqControl &= ~(HSADC_ACQ_CONTROL2_CAP_SIZE_MASK);
        HSADC_Regs[hwId]->acq_control2 = (acqControl | captureSize);

        /*Configure channels, decimation rate and agg count*/
        acqControl = HSADC_Regs[hwId]->acq_control0;
        acqControl &= ~(HSADC_ACQ_CONTROL0_CH_EN_MASK | HSADC_ACQ_CONTROL0_DEC_RATE_MASK |
                       HSADC_ACQ_CONTROL0_AGG_CNT_MASK );
        HSADC_Regs[hwId]->acq_control0 = (acqControl | ((uint32_t)channelEnable << HSADC_ACQ_CONTROL0_CH_EN_SHIFT) |
                                           ((uint32_t)decimationRate << HSADC_ACQ_CONTROL0_DEC_RATE_SHIFT) |
                                           ((uint32_t)aggregateCount << HSADC_ACQ_CONTROL0_AGG_CNT_SHIFT));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief HSADC capture status

    This API retrives HSADC capture status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSADC controller
    @param[out]     captureBusy  1 - Indicated that the capture is ongoing.
                                     In single mode it will be high during any given capture.
                                     In continous mode it will be high till the last capture in continous mode.
                                     In aggregation mode it will be high till the completion of a given aggregation.
                                 0U - Idel
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CAPTURE_STATUS_PROC

    @code{.unparsed}
         Check hardware instance and Null pointer for capture busy
         Read acq_control0 and store capture busy
    @endcode


    @limitations None
*/
int32_t HSADC_DrvCaptureStatus(HSADC_HwIDType  hwId, uint8_t *captureBusy)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) { /*Check hwId*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (captureBusy == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*retrive capture busy*/
        *captureBusy = (uint8_t)(HSADC_Regs[hwId]->acq_control0 >> HSADC_ACQ_CONTROL0_CAP_BUSY_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC FFT configuration

    This API configures HSADC FFT pre-divider, sample size, axis infomarion and enables the FFT.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[in]      fftConfig     HSADC FFT configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_PROC

    @code{.unparsed}
         Check hardware instance and fftConfig with supported values.
         Write fftReadPeriod to acq_control4
         Write captureStartAddress to acq_control2
         Write fftSize and fftEnable to acq_control0
    @endcode


    @limitations None
*/
int32_t HSADC_DrvFftConfig(HSADC_HwIDType  hwId, HSADC_FftConfigType *fftConfig)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acqControl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check hwId*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (fftConfig == NULL) {/* Check device state*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (fftConfig->fftEnable > 1U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (fftConfig->fftSize > 7U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /* Write Control 4*/
        acqControl = HSADC_Regs[hwId]->acq_control4;
        acqControl &= ~(HSADC_ACQ_CONTROL4_FFT_RD_PERIOD_MASK);
        HSADC_Regs[hwId]->acq_control4 = (acqControl | fftConfig->fftReadPeriod);

        /* Write Control 2*/
        acqControl = HSADC_Regs[hwId]->acq_control2;
        acqControl &= ~(HSADC_ACQ_CONTROL2_CAP_START_ADDR_MASK);
        HSADC_Regs[hwId]->acq_control2 = (acqControl | fftConfig->captureStartAddress);

        /* Write Control 0U*/
        acqControl = HSADC_Regs[hwId]->acq_control0;
        acqControl &= ~(HSADC_ACQ_CONTROL0_FFT_EN_MASK | HSADC_ACQ_CONTROL0_FFT_SIZE_MASK );

        HSADC_Regs[hwId]->acq_control0 = acqControl | ((uint32_t)fftConfig->fftEnable << HSADC_ACQ_CONTROL0_FFT_EN_SHIFT) |
                                         ((uint32_t)fftConfig->fftSize << HSADC_ACQ_CONTROL0_FFT_SIZE_SHIFT) ;


#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief Configures HSADC read done address
 *
    This API configures the read done address.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[out]     rdDoneAddr          Read Address to be configured.
                                        In 1.25G only the bottom 9 address bits have a meaning.
                                        In 5G all 11 bits have a meaning.
                                        Note: Reading from this address location will
                                        indicate that the OCP has read the captured data during the read phase.
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_READ_DONEADDRESS_PROC

    @code{.unparsed}
         Check hardware instance.
         Write rdDoneAddr to acq_control3
    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigReadDoneAddress(HSADC_HwIDType  hwId, uint16_t rdDoneAddr)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acqControl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check hwId*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* Write readDoneAddress*/
        acqControl = HSADC_Regs[hwId]->acq_control3;
        acqControl &= ~(HSADC_ACQ_CONTROL3_RD_DONE_ADDR_MASK);
        HSADC_Regs[hwId]->acq_control3 = acqControl | ((uint32_t)rdDoneAddr << HSADC_ACQ_CONTROL3_RD_DONE_ADDR_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC configure Read capture size

    This API defines how long time is allocated for either the FFT logic or
    OCP to read the ADC buffer data.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[in]      readCapSize         This is in multiples of 16 samples.
                                        In non-FFT or FFT mode, this should be programmed
                                        to be few clock cycles (~5) less than the spacing between two triggers.
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_READ_CAPSIZE_PROC

    @code{.unparsed}
         Check hardware instance.
         Write readCapSize to acq_control3
    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigReadCapSize(HSADC_HwIDType  hwId, uint16_t readCapSize)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t acqControl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) { /*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* Write readcapturesize*/
        acqControl = HSADC_Regs[hwId]->acq_control3;
        acqControl &= ~(HSADC_ACQ_CONTROL3_RD_CAP_SIZE_MASK);
        HSADC_Regs[hwId]->acq_control3 = acqControl | ((uint32_t)readCapSize << HSADC_ACQ_CONTROL3_RD_CAP_SIZE_SHIFT);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC Window memory read/Write

    This API reads/writes data to the window memory based on fftSize. This will be used only when FFT is
    enabled.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[in]      windowConfig  HSADC window data
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_WINDOWMEM_CONFIG_PROC

    @code{.unparsed}
         Check hardware instance and windowConfig.
         if opMode is HSADC_WINDOW_OP_WRITE
            Loop till fftSize
               Write data[0U] .. data[7] to wnd_data0..wnd_data7
               Write windowAddr and Write strobe
         else
            Loop till fftSize
               Write windowAddr and Read strobe
               Read wnd_data0..wnd_data7 and store in data0..data7
    @endcode


    @limitations None
*/
int32_t HSADC_DrvWindowMemConfig(HSADC_HwIDType  hwId, HSADC_WindowMemType *windowConfig)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t windowAddr = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (windowConfig == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if ((windowConfig->fftSize > 7U) || (windowConfig->fftSize <= 2U)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (windowConfig->opMode > HSADC_WINDOW_OP_WRITE) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /* Each address in window can hold 32 coefficients i.e. 8x 32bit
         * registers are used */
        if (windowConfig->opMode == HSADC_WINDOW_OP_WRITE) {
            for (windowAddr = 0UL; windowAddr < (windowConfig->fftSize/8UL); windowAddr ++) {
                HSADC_Regs[hwId]->wnd_data0 = windowConfig->data[0U + (windowAddr*8U)];
                HSADC_Regs[hwId]->wnd_data1 = windowConfig->data[1U + (windowAddr*8U)];
                HSADC_Regs[hwId]->wnd_data2 = windowConfig->data[2U + (windowAddr*8U)];
                HSADC_Regs[hwId]->wnd_data3 = windowConfig->data[3U + (windowAddr*8U)];
                HSADC_Regs[hwId]->wnd_data4 = windowConfig->data[4U + (windowAddr*8U)];
                HSADC_Regs[hwId]->wnd_data5 = windowConfig->data[5U + (windowAddr*8U)];
                HSADC_Regs[hwId]->wnd_data6 = windowConfig->data[6U + (windowAddr*8U)];
                HSADC_Regs[hwId]->wnd_data7 = windowConfig->data[7U + (windowAddr*8U)];

                /* Write Address and Write Strobe*/
                HSADC_Regs[hwId]->wnd_control = (HSADC_WND_CONTROL_WR_STROBE_MASK | (windowAddr << HSADC_WND_CONTROL_ADDR_SHIFT));
            }
        } else {
            for (windowAddr = 0UL; windowAddr < (windowConfig->fftSize/8UL); windowAddr ++) {
                /* Write Address and Read Strobe*/
                HSADC_Regs[hwId]->wnd_control = (HSADC_WND_CONTROL_RD_STROBE_MASK | (windowAddr << HSADC_WND_CONTROL_ADDR_SHIFT));
                windowConfig->data[0U + (windowAddr*8U)] = HSADC_Regs[hwId]->wnd_data0;
                windowConfig->data[1U + (windowAddr*8U)] = HSADC_Regs[hwId]->wnd_data1;
                windowConfig->data[2U + (windowAddr*8U)] = HSADC_Regs[hwId]->wnd_data2;
                windowConfig->data[3U + (windowAddr*8U)] = HSADC_Regs[hwId]->wnd_data3;
                windowConfig->data[4U + (windowAddr*8U)] = HSADC_Regs[hwId]->wnd_data4;
                windowConfig->data[5U + (windowAddr*8U)] = HSADC_Regs[hwId]->wnd_data5;
                windowConfig->data[6U + (windowAddr*8U)] = HSADC_Regs[hwId]->wnd_data6;
                windowConfig->data[7U + (windowAddr*8U)] = HSADC_Regs[hwId]->wnd_data7;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC enables/disables window function

    This API enables/disables HSADC window function. This should be set only after
    configuring window @ref HSADC_DrvWindowMemConfig.


    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId          Index of the HSADC controller
    @param[in]      enWindow     0U - Disable window function 1 - enables window function
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_WINDOW_CONTROL_PROC

    @code{.unparsed}
         Check hardware instance.
         Write enWindow to wnd_control
    @endcode


    @limitations None
*/
int32_t HSADC_DrvWindowControl(HSADC_HwIDType  hwId, uint8_t enWindow)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t control = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* Disable/ Enable Window control*/
        control = HSADC_Regs[hwId]->wnd_control;
        control &= ~HSADC_WND_CONTROL_EN_MASK;
        HSADC_Regs[hwId]->wnd_control = control | enWindow;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief HSADC clamp config

    This API configures clamp controls.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId              Index of the HSADC controller
    @param[in]      clampConfig       HSADC clamp configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CLAMP_CONFIG_PROC

    @code{.unparsed}
         Check hardware instance.
         Write adcClampDelay adcClampDuration and adcClampEnable to  clamp_control
    @endcode


    @limitations None
*/
int32_t HSADC_DrvClampConfig(HSADC_HwIDType  hwId, HSADC_ClampControlConfigType *clampConfig)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /*Configure clamp delay, clamp duration and clamp enable*/
        HSADC_Regs[hwId]->clamp_control = (uint32_t)(((uint32_t)clampConfig->adcClampDelay << HSADC_CLAMP_CONTROL_ADC_DELAY_SHIFT) |
                                           ((uint32_t)clampConfig->adcClampDuration << HSADC_CLAMP_CONTROL_ADC_DURATION_SHIFT) |
                                           (uint32_t)clampConfig->adcClampEnable );

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC TOF configuration

    This API configures time of flight offset.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId           Index of the HSADC controller
    @param[in]      tofOffset     HSADC time of flight offset.
                                  This is a signed value that will be added to the incoming ADC signal to push
                                  the signal to positive direction.
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_TOFOFFSET_CONFIG_PROC

    @code{.unparsed}
         Check hardware instance.
         Write tofOffset to tof_offset
    @endcode


    @limitations None
*/
int32_t HSADC_DrvTofOffsetConfig(HSADC_HwIDType  hwId, uint16_t tofOffset)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /*Configure TOF offset*/
        HSADC_Regs[hwId]->tof_offset = tofOffset;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC FIR coeffecient configuration

    This API configures filter coefficients.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the HSADC controller
    @param[in]      firCoefficient HSADC FIR Coefficient array. Totally support 32 co-efficients.
                                   This is a signed 10-bit value.
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_FIRCOEFF_PROC

    @code{.unparsed}
         Check hardware instance and NULL check for firCoefficient.
         Write firCoefficient0..31 to fir_coeff0..fir_coeff31
    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigFirCoeff(HSADC_HwIDType  hwId, uint16_t *firCoefficient)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if(firCoefficient == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*Write all 32 fir coefficient*/
        HSADC_Regs[hwId]->fir_coeff0  = firCoefficient[0U];
        HSADC_Regs[hwId]->fir_coeff1  = firCoefficient[1U];
        HSADC_Regs[hwId]->fir_coeff2  = firCoefficient[2U];
        HSADC_Regs[hwId]->fir_coeff3  = firCoefficient[3U];
        HSADC_Regs[hwId]->fir_coeff4  = firCoefficient[4U];
        HSADC_Regs[hwId]->fir_coeff5  = firCoefficient[5U];
        HSADC_Regs[hwId]->fir_coeff6  = firCoefficient[6U];
        HSADC_Regs[hwId]->fir_coeff7  = firCoefficient[7U];
        HSADC_Regs[hwId]->fir_coeff8  = firCoefficient[8U];
        HSADC_Regs[hwId]->fir_coeff9  = firCoefficient[9U];
        HSADC_Regs[hwId]->fir_coeff10 = firCoefficient[10U];
        HSADC_Regs[hwId]->fir_coeff11 = firCoefficient[11U];
        HSADC_Regs[hwId]->fir_coeff12 = firCoefficient[12U];
        HSADC_Regs[hwId]->fir_coeff13 = firCoefficient[13U];
        HSADC_Regs[hwId]->fir_coeff14 = firCoefficient[14U];
        HSADC_Regs[hwId]->fir_coeff15 = firCoefficient[15U];
        HSADC_Regs[hwId]->fir_coeff16 = firCoefficient[16U];
        HSADC_Regs[hwId]->fir_coeff17 = firCoefficient[17U];
        HSADC_Regs[hwId]->fir_coeff18 = firCoefficient[18U];
        HSADC_Regs[hwId]->fir_coeff19 = firCoefficient[19U];
        HSADC_Regs[hwId]->fir_coeff20 = firCoefficient[20U];
        HSADC_Regs[hwId]->fir_coeff21 = firCoefficient[21U];
        HSADC_Regs[hwId]->fir_coeff22 = firCoefficient[22U];
        HSADC_Regs[hwId]->fir_coeff23 = firCoefficient[23U];
        HSADC_Regs[hwId]->fir_coeff24 = firCoefficient[24U];
        HSADC_Regs[hwId]->fir_coeff25 = firCoefficient[25U];
        HSADC_Regs[hwId]->fir_coeff26 = firCoefficient[26U];
        HSADC_Regs[hwId]->fir_coeff27 = firCoefficient[27U];
        HSADC_Regs[hwId]->fir_coeff28 = firCoefficient[28U];
        HSADC_Regs[hwId]->fir_coeff29 = firCoefficient[29U];
        HSADC_Regs[hwId]->fir_coeff30 = firCoefficient[30U];
        HSADC_Regs[hwId]->fir_coeff31 = firCoefficient[31U];
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC post filter gain configuration

    This API configures post filter gain. This can be applied even when filter is
    in disabled state.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the HSADC controller
    @param[in]      dGain          This set the post filter gain.
                                   The gain is applied even when the filter is disabled.
                                   0U -  1x   1 - 2x   2 - 4x   3 - 8x
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_FILTERGAIN_PROC

    @code{.unparsed}
         Check hardware instance and dGain.
         Write dGain to fir_control
    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigFilterGain(HSADC_HwIDType  hwId, uint8_t dGain)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t firControl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) { /*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (dGain > 3U) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /* Set gain on fir control*/
        firControl = HSADC_Regs[hwId]->fir_control;
        firControl &= ~HSADC_FIR_CONTROL_DGAIN_MASK;
        HSADC_Regs[hwId]->fir_control =  firControl | ((uint32_t)dGain << HSADC_FIR_CONTROL_DGAIN_SHIFT);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC enables/disables Filter

    This API enables/disables filter path, This should be set after enabling the
    clock and after writing the coeffecient using @ref HSADC_DrvConfigFirCoeff.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the HSADC controller
    @param[in]      enControl      1- enable filter path 0U - Disable
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_FILTER_CONTROL_PROC

    @code{.unparsed}
         Check hardware instance.
         Write enControl to fir_control
    @endcode


    @limitations None
*/
int32_t HSADC_DrvFilterControl(HSADC_HwIDType  hwId, uint8_t enControl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t firControl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /*Enable FIR control*/
        firControl = HSADC_Regs[hwId]->fir_control;
        firControl &= ~HSADC_FIR_CONTROL_FIR_EN_MASK;
        HSADC_Regs[hwId]->fir_control =  firControl | enControl;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC configure FFT axis capture

    This API configures FFT axis capture information.
    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId                    Index of the HSADC controller
    @param[in]     fftAxisCapConfig HSADC FFT axis capture configuration
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_FFT_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_FFT_CONFIG_AXIS_CAPTURE_PROC

    @code{.unparsed}
         Check hardware instance.
         Write fftAxisCapConfig to axis_info_control
    @endcode


    @limitations None
*/
int32_t HSADC_DrvFftConfigAxisCapture(HSADC_HwIDType  hwId,  HSADC_FftAxisCapConfigType fftAxisCapConfig)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (fftAxisCapConfig > HSADC_FFT_AXIS_CAP_CONFIG_FFT) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        /*Configure Axis capture mode*/
        HSADC_Regs[hwId]->axis_info_control = (fftAxisCapConfig == HSADC_FFT_AXIS_CAP_CONFIG_ADCBUF) ? 1UL : 2UL ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief HSADC gets FFT axis information

    This API gets HSADC FFT axis information. Capturing of axis information depends on
    HSADC_FftAxisCapConfig value in @ref HSADC_DrvFftConfigAxisCapture.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[out]     fftAxisInfo HSADC FFT axis information
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_FFT_GET_AXIS_INFORMATION_PROC

    @code{.unparsed}
         Check hardware instance.
         Get axis_info_read_lo and axis_info_read_hi from fftAxisInfoReadLow and fftAxisInfoReadHigh.
    @endcode


    @limitations None
*/
int32_t HSADC_DrvFftGetAxisInformation(HSADC_HwIDType  hwId,  HSADC_FftGetAxisInformationType *fftAxisInfo)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (fftAxisInfo == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
         /* Read axis into low and high*/
         fftAxisInfo->fftAxisInfoReadLow = HSADC_Regs[hwId]->axis_info_read_lo;
         fftAxisInfo->fftAxisInfoReadHigh = HSADC_Regs[hwId]->axis_info_read_hi;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC Calibration Status

    This API gets HSADC calibration status. Calibration status in not valid if calibration is not enabled.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[out]     calibStatus           HSADC calibration status
                                          1 : Calibration in progress
                                          0 : Calibration completed
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post HSADC driver state changes to HSADC_CALIB_INPROGRESS/HSADC_ENABLE

    @trace  #BRCM_SWREQ_HSADC_CALIBRATION
    @trace  #BRCM_SWARCH_HSADC_DRV_FFT_GET_CALIB_STATUS_PROC

    @code{.unparsed}
         Check hardware instance.
         Get calStatus from adccal_control0
    @endcode


    @limitations None
*/
int32_t HSADC_DrvFftGetCalibStatus(HSADC_HwIDType  hwId,  HSADC_CalibStatusType *calibStatus)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) { /* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        /* read Calibration busy status*/
        calibStatus->calStatus = (uint16_t)(HSADC_Regs[hwId]->adccal_control0 >> HSADC_ADCCAL_CONTROL0_CAL_BUSY_SHIFT) ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @brief HSADC Enable Channel interrupt

    This API enables/disables HSADC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                 Index of the HSADC controller
    @param[in]      chId                 Index of the HSADC Channel
    @param[in]      intrType             HSADC interrupt type
    @param[in]      enaDis               0U - Disable specified interrupt 1 - Enable Specified interrupt
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @port  None

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_CH_INTR_PROC

    @code{.unparsed}
         Check hardware instance, Channel Id and interrupt type.
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_FFT_DONE
            Enable FFT_DONE_INTR in acq_hsadc_buf_intr_eier
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_CAP_DONE
            Enable CAP_DONE in acq_hsadc_buf_intr_eier
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_DMA_ERR
            Enable DMA_ERR in acq_hsadc_func_err_intr_eier
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR
            Enable OCP_RD_ERR in acq_hsadc_func_err_intr_eier
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR
            Enable FFT_MEM_WR in acq_hsadc_func_err_intr_eier
    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigChIntr(HSADC_HwIDType  hwId,  HSADC_ChannelIDType chId, HSADC_ChannelIntrType intrType, uint32_t enaDis)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {/*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (chId > HSADC_CHANNEL_ID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (enaDis > 1UL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (((intrType & HSADC_CHANNEL_INTR_FFT_DONE) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /*FFT Done*/
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eier &= ~(1UL << (chId + HSADC_INTR_AHBIE2_ENABLE_CH0_FFT_DONE_INTR_SHIFT));
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eier |= (enaDis << (chId + HSADC_INTR_AHBIE2_ENABLE_CH0_FFT_DONE_INTR_SHIFT));
        }
        if (((intrType & HSADC_CHANNEL_INTR_CAP_DONE) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /*CAP Done*/
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eier &= ~(1UL << chId);
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eier |= (enaDis << chId);
        }
        if (((intrType & HSADC_CHANNEL_INTR_DMA_ERR) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /*DMA error*/
           /* coverity[cert_int30_c_violation] */
           /* coverity[cert_int34_c_violation] */
           HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier &= ~(1UL << chId);
           /* coverity[cert_int30_c_violation] */
           /* coverity[cert_int34_c_violation] */
           HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier |= (enaDis << chId);
        }
        if (((intrType & HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /*OCP Read Error*/
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier &= ~(1UL << (chId + HSADC_INTR_AHFEIE2_ENABLE_BUF0_OCP_RD_ERR_INTR_SHIFT));
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier |= (enaDis << (chId + HSADC_INTR_AHFEIE2_ENABLE_BUF0_OCP_RD_ERR_INTR_SHIFT));
        }
        if (((intrType & HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /*FFT mem write error*/
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier &= ~(1UL << (chId + HSADC_INTR_AHFEIE2_ENABLE_FFT_MEM0_WR_ERR_INTR_SHIFT));
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier |= (enaDis << (chId + HSADC_INTR_AHFEIE2_ENABLE_FFT_MEM0_WR_ERR_INTR_SHIFT));
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @brief HSADC clears Channel interrupt

    This API clears HSADC channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      chId                 Index of the HSADC Channel
    @param[in]      HSADC_IntType        HSADC interrupt type
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @port  None

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_CLEAR_CH_INTR_PROC

    @code{.unparsed}
         Check hardware instance, Channel Id and interrupt type.
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_FFT_DONE
            clear FFT_DONE_INTR in acq_hsadc_buf_intr_eisr
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_CAP_DONE
            clear CAP_DONE in acq_hsadc_buf_intr_eisr
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_DMA_ERR
            clear DMA_ERR in acq_hsadc_func_err_intr_eisr
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR
            clear OCP_RD_ERR in acq_hsadc_func_err_intr_eisr
         if intrType == HSADC_CHANNEL_INTR_ALL OR HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR
            clear FFT_MEM_WR in acq_hsadc_func_err_intr_eisr

    @endcode


    @limitations None
*/
int32_t HSADC_DrvClearChIntr(HSADC_HwIDType  hwId,  HSADC_ChannelIDType chId, HSADC_ChannelIntrType intrType)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) { /*Check HwID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (chId > HSADC_CHANNEL_ID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (intrType > HSADC_CHANNEL_INTR_ALL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (((intrType & HSADC_CHANNEL_INTR_FFT_DONE) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /*FFT Done*/
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eisr |= (1UL << (chId + HSADC_INTR_AHBIE1_CH0_FFT_DONE_INTR_W2CLR_SHIFT));
        }
        if (((intrType & HSADC_CHANNEL_INTR_CAP_DONE) > 0UL) ||  intrType == HSADC_CHANNEL_INTR_ALL) { /*CAP done*/
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eisr |= (1UL << chId);
        }
        if (((intrType & HSADC_CHANNEL_INTR_DMA_ERR) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /*DMA error*/
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eisr |= (1UL << chId);
        }
        if (((intrType & HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /* OCP read error*/
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eisr |= (1UL << (chId + HSADC_INTR_AHFEIE1_BUF0_OCP_RD_ERR_INTR_W2CLR_SHIFT));
        }
        if (((intrType & HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR) > 0UL) || intrType == HSADC_CHANNEL_INTR_ALL) { /*FFT memory write*/
            /* coverity[cert_int30_c_violation] */
            /* coverity[cert_int34_c_violation] */
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eisr |= (1UL << (chId + HSADC_INTR_AHFEIE1_FFT_MEM0_WR_ERR_INTR_W2CLR_SHIFT));
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Get all the Interrupt status

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_CH_INTR_STATUS_PROC
*/
static void HSADC_IntGetAllChInterruptStatus (HSADC_HwIDType  hwId, HSADC_ChannelIDType chId, uint32_t *aStatus) 
{
    uint32_t status = 0;

    /* Read FFT Done DMA Err */
    status = HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eipr;
    /* coverity[cert_int30_c_violation] */
    /* coverity[cert_int34_c_violation] */
    *aStatus = ((status & (1UL << (chId + HSADC_INTR_AHBIE_P_CH0_FFT_DONE_INTR_SHIFT))) > 0UL ? HSADC_CHANNEL_INTR_FFT_DONE: 0UL) | ((status & (1UL << chId)) > 0UL ? HSADC_CHANNEL_INTR_CAP_DONE : 0UL) ;
    status = HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eipr;
    *aStatus |= (( status  & (1UL << chId)) > 0UL ? HSADC_CHANNEL_INTR_DMA_ERR : 0UL) |
                    ((status  & (1UL << (chId + HSADC_INTR_AHFEIE_P_BUF0_OCP_RD_ERR_INTR_SHIFT))) > 0UL ? HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR :0UL) |
                    ((status  & (1UL << (chId + HSADC_INTR_AHFEIE_P_FFT_MEM0_WR_ERR_INTR_SHIFT))) > 0UL ? HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR : 0UL );
}

/**
    @brief Get all the event status

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_CH_INTR_STATUS_PROC
*/
static void HSADC_IntGetChEventStatus (HSADC_HwIDType hwId, HSADC_ChannelIDType chId, uint32_t *aStatus) 
{
    uint32_t status = 0;
        /* Read Event status for all the interrupt*/
    status = HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eisr;
    /* coverity[cert_int30_c_violation] */
    /* coverity[cert_int34_c_violation] */
    *aStatus = ((status & (1UL << (chId + HSADC_INTR_AHBIE1_CH0_FFT_DONE_INTR_W2CLR_SHIFT))) > 0UL ? HSADC_CHANNEL_INTR_FFT_DONE: 0UL) | ((status & (1UL << chId)) > 0UL ? HSADC_CHANNEL_INTR_CAP_DONE : 0UL) ;
    status = HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eisr;
    *aStatus |= (( status  & (1UL << chId)) > 0UL ? HSADC_CHANNEL_INTR_DMA_ERR : 0UL) |
        ((status  & (1UL << (chId + HSADC_INTR_AHFEIE1_BUF0_OCP_RD_ERR_INTR_W2CLR_SHIFT))) > 0UL ? HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR :0UL) |
        ((status  & (1UL << (chId + HSADC_INTR_AHFEIE1_FFT_MEM0_WR_ERR_INTR_W2CLR_SHIFT))) > 0UL ? HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR : 0UL );

}


/** @brief HSADC Get Channel interrupt Status

    This API gets HSADC channel based interrupts status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      chId                 Index of the HSADC Channel
    @param[in]      HSADC_IntType        HSADC interrupt type
    @param[out]     intrStatus           HSADC interrupt Status
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @port  None

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_CH_INTR_STATUS_PROC

    @code{.unparsed}
         Check hardware instance, Channel Id and interrupt type.
         if intrType == HSADC_CHANNEL_INTR_ALL
            Read acq_hsadc_buf_intr_eipr, acq_hsadc_func_err_intr_eipr and provide all
            interrupt status
         else
            if intrType == HSADC_CHANNEL_INTR_FFT_DONE
                read FFT_DONE_INTR in acq_hsadc_buf_intr_eipr
            if intrType == HSADC_CHANNEL_INTR_CAP_DONE
               read CAP_DONE in acq_hsadc_buf_intr_eipr
            if intrType ==  HSADC_CHANNEL_INTR_DMA_ERR
               read DMA_ERR in acq_hsadc_func_err_intr_eipr
            if intrType ==  HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR
               read OCP_RD_ERR in acq_hsadc_func_err_intr_eipr
            if intrType ==  HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR
               read FFT_MEM_WR in acq_hsadc_func_err_intr_eipr
        if intrType == HSADC_CHANNEL_INTR_GET_EVENT_ALL
            Read acq_hsadc_buf_intr_eisr, acq_hsadc_func_err_intr_eisr and provide all
            event status

    @endcode


    @limitations None
*/
/* @event           CERT-C INT30
   @issue          Coverity infers that the addition may wraps
   @risk           No risk
   @justification  chId got validated.*/
/* @event           CERT-C INT34
   @issue          Coverity infers that the shifting cause overflow
   @risk           No risk
   @justification  chId got validated.*/
 
int32_t HSADC_DrvGetChIntrStatus(HSADC_HwIDType  hwId,  HSADC_ChannelIDType chId, HSADC_ChannelIntrType intrType, uint32_t *intrStatus)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) { /*Check HW ID*/
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (chId > HSADC_CHANNEL_ID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        *intrStatus = 0UL;
        if (intrType == HSADC_CHANNEL_INTR_ALL) { /*Read Pending status for all the interrupts*/
            HSADC_IntGetAllChInterruptStatus (hwId, chId, intrStatus); 
        } else if ((intrType & HSADC_CHANNEL_INTR_FFT_DONE) > 0UL) { /* Read FFT Done*/
                /* coverity[cert_int30_c_violation] */
                /* coverity[cert_int34_c_violation] */
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eipr & (1UL << (chId + HSADC_INTR_AHBIE_P_CH0_FFT_DONE_INTR_SHIFT))) > 0UL ? HSADC_CHANNEL_INTR_FFT_DONE: 0UL ;
        } else if ((intrType & HSADC_CHANNEL_INTR_CAP_DONE) > 0UL) { /* read Cap done status*/
                /* coverity[cert_int30_c_violation] */
                /* coverity[cert_int34_c_violation] */
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_buf_intr_eipr & (1UL << chId)) > 0UL? HSADC_CHANNEL_INTR_CAP_DONE : 0UL;
        } else if ((intrType & HSADC_CHANNEL_INTR_DMA_ERR) > 0UL) { /*Read DMA error*/
                /* coverity[cert_int30_c_violation] */
                /* coverity[cert_int34_c_violation] */
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eipr & (1UL << chId)) > 0UL? HSADC_CHANNEL_INTR_DMA_ERR : 0UL;
        } else if ((intrType & HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR) > 0UL) { /* Read OCP read error*/
                /* coverity[cert_int30_c_violation] */
                /* coverity[cert_int34_c_violation] */
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eipr & (1UL << (chId + HSADC_INTR_AHFEIE_P_BUF0_OCP_RD_ERR_INTR_SHIFT))) > 0UL? HSADC_CHANNEL_INTR_BUF_OCP_RD_ERR :0UL ;
        } else if ((intrType & HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR) > 0UL) { /*Read FFT mem error*/
                /* coverity[cert_int30_c_violation] */
                /* coverity[cert_int34_c_violation] */
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eipr & (1UL << (chId + HSADC_INTR_AHFEIE_P_FFT_MEM0_WR_ERR_INTR_SHIFT))) > 0UL? HSADC_CHANNEL_INTR_FFT_MEM_WR_ERR : 0UL ;
        } else {
            HSADC_IntGetChEventStatus(hwId, chId, intrStatus);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Clear memory initialization interrupts

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_INTR_PROC

*/
static void HSADC_IntConfigMemInitIntr (HSADC_HwIDType hwId,  HSADC_NonChannelIntrType intrType, uint32_t enaDis) 
{
    if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_ERR) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_WNDBUF_MEM_INIT_ERR_INTR_MASK;
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_WNDBUF_MEM_INIT_ERR_INTR_SHIFT;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_DONE) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_WNDBUF_MEM_INIT_DONE_INTR_MASK;
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_WNDBUF_MEM_INIT_DONE_INTR_SHIFT;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_ERR) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_FFTBUF_MEM_INIT_ERR_INTR_MASK;
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_FFTBUF_MEM_INIT_ERR_INTR_SHIFT;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_DONE) > 0UL) { 
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_FFTBUF_MEM_INIT_DONE_INTR_MASK;
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_FFTBUF_MEM_INIT_DONE_INTR_SHIFT;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_ERR) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_FFT_MEM_INIT_ERR_INTR_MASK;
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_FFT_MEM_INIT_ERR_INTR_SHIFT;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_DONE) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_FFT_MEM_INIT_DONE_INTR_MASK;
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_FFT_MEM_INIT_DONE_INTR_SHIFT;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_ERR) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_ADCBUF_MEM_INIT_ERR_INTR_MASK;
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_ADCBUF_MEM_INIT_ERR_INTR_SHIFT;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_DONE) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_ADCBUF_MEM_INIT_DONE_INTR_MASK;
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_ADCBUF_MEM_INIT_DONE_INTR_SHIFT;
    } else {
        /* do nothing*/
    }

}

/** @brief HSADC enable non channel interrupt

    This API enables/disables HSADC non channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      intrType             HSADC interrupt type
    @param[in]      enaDis               0UL - Disable specified interrupt 1 - Enable Specified interrupt
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @port  None

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_CONFIG_INTR_PROC

    @code{.unparsed}
         Check hardware instance, Interrupt type.
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_CALIB_ERR
             Enable/Disable CALIB_ERR in acq_hsadc_func_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_CALIB_DONE
             Enable/Disable CALIB_DONE in acq_hsadc_func_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE
             Enable/Disable MODE_COMPLETE in acq_hsadc_func_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR
             Enable/Disable FFT_DATAPATH_ERR in acq_hsadc_func_err_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_FFT_IN_ERR
             Enable/Disable FFT_IN_ERR in acq_hsadc_func_err_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_ERR
             Enable/Disable WNDBUF_MEM_INIT_ERR in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_DONE
             Enable/Disable WNDBUF_MEM_INIT_DONE in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_WNDBUF_SEC
             Enable/Disable WNDBUF_SEC in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_ERR
             Enable/Disable FFTBUF_MEM_INIT_ERR in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_DONE
             Enable/Disable FFTBUF_MEM_INIT_DONE in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_ERR
             Enable/Disable FFT_MEM_INIT_ERR in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_DONE
             Enable/Disable FFT_MEM_INIT_DONE in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_ERR
             Enable/Disable ADCBUF_MEM_INIT_ERR in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_DONE
             Enable/Disable ADCBUF_MEM_INIT_DONE in acq_hsadc_mem_init_sec_intr_eier
         if intrType == HSADC_NONCHANNEL_INTR_ALL OR HSADC_NONCHANNEL_INTR_WNDBUF_DED
             Enable/Disable WNDBUF_DED in acq_hsadc_ded_intr_eier
    @endcode


    @limitations None
*/
int32_t HSADC_DrvConfigIntr(HSADC_HwIDType  hwId,  HSADC_NonChannelIntrType intrType, uint32_t enaDis)
{
   int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (intrType > HSADC_NONCHANNEL_INTR_ALL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (enaDis > 1UL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if ((intrType & HSADC_NONCHANNEL_INTR_CALIB_ERR) > 0UL) {
            HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eier &= ~HSADC_INTR_AHFIE2_ENABLE_ADC_CALIB_ERR_INTR_MASK;
            HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eier |= enaDis << HSADC_INTR_AHFIE2_ENABLE_ADC_CALIB_ERR_INTR_SHIFT;
        }
        if ((intrType & HSADC_NONCHANNEL_INTR_CALIB_DONE) > 0UL) {
            HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eier &= ~HSADC_INTR_AHFIE2_ENABLE_ADC_CALIB_DONE_INTR_MASK;
            HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eier |= enaDis << HSADC_INTR_AHFIE2_ENABLE_ADC_CALIB_DONE_INTR_SHIFT;
        }
        if ((intrType & HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE) > 0UL) {
            HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eier &= ~HSADC_INTR_AHFIE2_ENABLE_CAP_MODE_COMPLETE_INTR_MASK;
            HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eier |= enaDis << HSADC_INTR_AHFIE2_ENABLE_CAP_MODE_COMPLETE_INTR_SHIFT;
        }
        if ((intrType & HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR) > 0UL) {
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier &= ~HSADC_INTR_AHFEIE2_ENABLE_FFT_DATAPATH_ERR_INTR_MASK;
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier |= enaDis << HSADC_INTR_AHFEIE2_ENABLE_FFT_DATAPATH_ERR_INTR_SHIFT;
        }
        if ((intrType & HSADC_NONCHANNEL_INTR_FFT_IN_ERR) > 0UL) {
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier &= ~HSADC_INTR_AHFEIE2_ENABLE_FFT_IN_ERR_INTR_MASK;
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eier |= enaDis << HSADC_INTR_AHFEIE2_ENABLE_FFT_IN_ERR_INTR_SHIFT;
        }
        if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_SEC) > 0UL) {
            HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier &= ~HSADC_INTR_AHMISIE2_ENABLE_WNDBUF_SEC_INTR_MASK;
            HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eier |= enaDis << HSADC_INTR_AHMISIE2_ENABLE_WNDBUF_SEC_INTR_SHIFT;
        }
        if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_DED) > 0UL) {
            HSADC_IntrRegs[hwId]->acq_hsadc_ded_intr_eier &= ~HSADC_INTR_ACQ_HSADC_DED_INTR_EIPR_P_WNDBUF_MASK;
            HSADC_IntrRegs[hwId]->acq_hsadc_ded_intr_eier |= enaDis << HSADC_INTR_ACQ_HSADC_DED_INTR_EIPR_P_WNDBUF_SHIFT;
        }

        HSADC_IntConfigMemInitIntr (hwId,  intrType, enaDis) ;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Clear memory initialization interrupts

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_CLEAR_INTR_PROC
*/
static void HSADC_IntClearMemInit (HSADC_HwIDType hwId,  HSADC_NonChannelIntrType intrType) 
{
    if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_ERR) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_WNDBUF_MEM_INIT_ERR_INTR_W2CLR_MASK;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_DONE) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_WNDBUF_MEM_INIT_DONE_INTR_W2CLR_MASK;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_ERR) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_FFTBUF_MEM_INIT_ERR_INTR_W2CLR_MASK;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_DONE) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_FFTBUF_MEM_INIT_DONE_INTR_W2CLR_MASK;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_ERR) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_FFT_MEM_INIT_ERR_INTR_W2CLR_MASK;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_DONE) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_FFT_MEM_INIT_DONE_INTR_W2CLR_MASK;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_ERR) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_ADCBUF_MEM_INIT_ERR_INTR_W2CLR_MASK;
    }
    if ((intrType & HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_DONE) > 0UL) {
        HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_ADCBUF_MEM_INIT_DONE_INTR_W2CLR_MASK;
    }

}


/** @brief HSADC clears non Channel based interrupt

    This API clears HSADC non channel based interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      intrType             HSADC interrupt type
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @port  None

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_CLEAR_INTR_PROC

    @code{.unparsed}
         Check hardware instance, Interrupt type.
         if intrType == HSADC_NONCHANNEL_INTR_ALL
             clear CALIB_ERR, CALIB_DONE, MODE_COMPLETE in acq_hsadc_func_intr_eisr
             clear FFT_DATAPATH_ERR, FFT_IN_ERR in acq_hsadc_func_err_intr_eisr
             clear WNDBUF_MEM_INIT_ERR, WNDBUF_MEM_INIT_DONE, WNDBUF_SEC, FFTBUF_MEM_INIT_ERR
                   FFTBUF_MEM_INIT_DONE, FFT_MEM_INIT_ERR, FFT_MEM_INIT_DONE, ADCBUF_MEM_INIT_ERR,
                   ADCBUF_MEM_INIT_DONE in  acq_hsadc_mem_init_sec_intr_eisr
             clear WNDBUF_DED in acq_hsadc_ded_intr_eisr
         else
             if intrType == HSADC_NONCHANNEL_INTR_CALIB_ERR
                 clear CALIB_ERR in acq_hsadc_func_intr_eisr
             if intrType == HSADC_NONCHANNEL_INTR_CALIB_DONE
                 clear CALIB_DONE in acq_hsadc_func_intr_eisr
             if intrType == HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE
                 clear MODE_COMPLETE in acq_hsadc_func_intr_eisr
             if intrType == HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR
                 clear FFT_DATAPATH_ERR in acq_hsadc_func_err_intr_eisr
             if intrType == HSADC_NONCHANNEL_INTR_FFT_IN_ERR
                 clear FFT_IN_ERR in acq_hsadc_func_err_intr_eisr
             if intrType == HSADC_NONCHANNEL_INTR_WNDBUF_SEC
                 clear WNDBUF_SEC in acq_hsadc_mem_init_sec_intr_eisr
             HSADC_IntClearMemInit (HSADC_HwIDType hwId,  HSADC_NonChannelIntrType intrType) 
             if intrType == HSADC_NONCHANNEL_INTR_WNDBUF_DED
                 clear WNDBUF_DED in acq_hsadc_ded_intr_eisr
    @endcode


    @limitations None
*/
int32_t HSADC_DrvClearIntr(HSADC_HwIDType  hwId,  HSADC_NonChannelIntrType intrType)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (intrType > HSADC_NONCHANNEL_INTR_ALL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if (intrType == HSADC_NONCHANNEL_INTR_ALL) { /*Clear All Interrupt*/
            HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eisr = (HSADC_INTR_AHFIE1_ADC_CALIB_ERR_INTR_W2CLR_MASK |
                  HSADC_INTR_AHFIE1_ADC_CALIB_DONE_INTR_W2CLR_MASK | HSADC_INTR_AHFIE1_CAP_MODE_COMPLETE_INTR_W2CLR_MASK);
            HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eisr = (HSADC_INTR_AHFEIE1_FFT_DATAPATH_ERR_INTR_W2CLR_MASK |
                    HSADC_INTR_AHFEIE1_FFT_IN_ERR_INTR_W2CLR_MASK);

        } else { /* Clear any specified interrupt*/
            if ((intrType & HSADC_NONCHANNEL_INTR_CALIB_ERR) > 0UL) {
                HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eisr |= HSADC_INTR_AHFIE1_ADC_CALIB_ERR_INTR_W2CLR_MASK;
            }
            if ((intrType & HSADC_NONCHANNEL_INTR_CALIB_DONE) > 0UL) {
                HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eisr |= HSADC_INTR_AHFIE1_ADC_CALIB_DONE_INTR_W2CLR_MASK;
            }
            if ((intrType & HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE) > 0UL) {
                HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eisr |= HSADC_INTR_AHFIE1_CAP_MODE_COMPLETE_INTR_W2CLR_MASK;
            }
            if ((intrType & HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR) > 0UL) {
                HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eisr |= HSADC_INTR_AHFEIE1_FFT_DATAPATH_ERR_INTR_W2CLR_MASK;
            }
            if ((intrType & HSADC_NONCHANNEL_INTR_FFT_IN_ERR) > 0UL) {
                HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eisr |= HSADC_INTR_AHFEIE1_FFT_IN_ERR_INTR_W2CLR_MASK;
            }
            if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_SEC) > 0UL) {
                HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr |= HSADC_INTR_AHMISIE1_WNDBUF_SEC_INTR_W2CLR_MASK;
            }
            if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_DED) > 0UL) {
                HSADC_IntrRegs[hwId]->acq_hsadc_ded_intr_eisr |= HSADC_INTR_ACQ_HSADC_DED_INTR_EISR_WNDBUF_W2CLR_MASK;
            }
            HSADC_IntClearMemInit (hwId,  intrType) ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
}
#endif
    return ret;
}
/**
    @brief Get all calibration Interrupt status

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC
*/
static void HSADC_IntGetAllCalibStatus (HSADC_HwIDType  hwId, uint8_t intEvent, uint32_t *aStatus) 
{
    uint32_t status = 0UL;
    if (intEvent == 1U) { /* read interrupt */
        status = HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eipr;

        *aStatus = ((status & HSADC_INTR_AHFIE_P_ADC_CALIB_ERR_INTR_MASK) > 0UL ? HSADC_NONCHANNEL_INTR_CALIB_ERR : 0UL) |
            ((status & HSADC_INTR_AHFIE_P_ADC_CALIB_DONE_INTR_MASK) > 0UL ? HSADC_NONCHANNEL_INTR_CALIB_DONE : 0UL) |
            ((status & HSADC_INTR_AHFIE_P_CAP_MODE_COMPLETE_INTR_MASK) > 0UL ? HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE : 0UL) ;
    } else {
        status = HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eisr;

        *aStatus = ((status & HSADC_INTR_AHFIE1_ADC_CALIB_ERR_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_CALIB_ERR : 0UL) |
            ((status & HSADC_INTR_AHFIE1_ADC_CALIB_DONE_INTR_W2CLR_MASK ) > 0UL? HSADC_NONCHANNEL_INTR_CALIB_DONE : 0UL) |
            ((status & HSADC_INTR_AHFIE1_CAP_MODE_COMPLETE_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE : 0UL) ;
    }
}

/**
    @brief Get all the Interrupt status

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC

*/
static void HSADC_IntGetAllInterruptStatus (HSADC_HwIDType  hwId, uint32_t *aStatus) 
{
    uint32_t status = 0;

    /* Get all interrupt status*/
    HSADC_IntGetAllCalibStatus (hwId, 1, aStatus);

    status =  HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eipr ;
    *aStatus |= ((((status & HSADC_INTR_AHFEIE_P_FFT_DATAPATH_ERR_INTR_MASK) > 0UL) ? HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR : 0UL) |
            (((status & HSADC_INTR_AHFEIE_P_FFT_IN_ERR_INTR_MASK) > 0UL) ? HSADC_NONCHANNEL_INTR_FFT_IN_ERR : 0UL));
    status = HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr;
    *aStatus |= ((status & HSADC_INTR_AHMISIE_P_WNDBUF_MEM_INIT_ERR_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_ERR : 0UL) |
        ((status & HSADC_INTR_AHMISIE_P_WNDBUF_MEM_INIT_DONE_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_DONE: 0UL) |
        ((status & HSADC_INTR_AHMISIE_P_WNDBUF_SEC_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_WNDBUF_SEC          : 0UL) |
        ((status & HSADC_INTR_AHMISIE_P_FFTBUF_MEM_INIT_ERR_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_ERR : 0UL) |
        ((status & HSADC_INTR_AHMISIE_P_FFTBUF_MEM_INIT_DONE_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_DONE : 0UL)|
        ((status & HSADC_INTR_AHMISIE_P_FFT_MEM_INIT_ERR_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_ERR    : 0UL) |
        ((status & HSADC_INTR_AHMISIE_P_FFT_MEM_INIT_DONE_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_DONE   : 0UL) |
        ((status & HSADC_INTR_AHMISIE_P_ADCBUF_MEM_INIT_ERR_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_ERR : 0UL) |
        ((status & HSADC_INTR_AHMISIE_P_ADCBUF_MEM_INIT_DONE_INTR_MASK)> 0UL ? HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_DONE : 0UL);
    *aStatus |= (((HSADC_IntrRegs[hwId]->acq_hsadc_ded_intr_eipr & HSADC_INTR_ACQ_HSADC_DED_INTR_EIPR_P_WNDBUF_MASK) > 0UL) ? HSADC_NONCHANNEL_INTR_WNDBUF_DED : 0UL);

}

/**
    @brief Get all the event status

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC

*/
static void HSADC_IntGetEventStatus (HSADC_HwIDType hwId, uint32_t *aStatus) 
{
    uint32_t status = 0;

    /* Read Event status for all the interrupt*/
    HSADC_IntGetAllCalibStatus (hwId, 0, aStatus);
    status =  HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eisr ;
    *aStatus |= (((status & HSADC_INTR_AHFEIE1_FFT_DATAPATH_ERR_INTR_W2CLR_MASK) > 0UL) ? HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR : 0UL) |
        (((status & HSADC_INTR_AHFEIE1_FFT_IN_ERR_INTR_W2CLR_MASK) > 0UL) ? HSADC_NONCHANNEL_INTR_FFT_IN_ERR : 0UL);
    status = HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eisr;
    *aStatus |= ((status & HSADC_INTR_AHMISIE1_WNDBUF_MEM_INIT_ERR_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_ERR : 0UL) |
        ((status & HSADC_INTR_AHMISIE1_WNDBUF_MEM_INIT_DONE_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_DONE: 0UL) |
        ((status & HSADC_INTR_AHMISIE1_WNDBUF_SEC_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_WNDBUF_SEC          : 0UL) |
        ((status & HSADC_INTR_AHMISIE1_FFTBUF_MEM_INIT_ERR_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_ERR : 0UL) |
        ((status & HSADC_INTR_AHMISIE1_FFTBUF_MEM_INIT_DONE_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_DONE : 0UL)|
        ((status & HSADC_INTR_AHMISIE1_FFT_MEM_INIT_ERR_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_ERR    : 0UL) |
        ((status & HSADC_INTR_AHMISIE1_FFT_MEM_INIT_DONE_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_DONE   : 0UL) |
        ((status & HSADC_INTR_AHMISIE1_ADCBUF_MEM_INIT_ERR_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_ERR : 0UL) |
        ((status & HSADC_INTR_AHMISIE1_ADCBUF_MEM_INIT_DONE_INTR_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_DONE : 0UL);
    *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_ded_intr_eisr & HSADC_INTR_ACQ_HSADC_DED_INTR_EISR_WNDBUF_W2CLR_MASK) > 0UL? HSADC_NONCHANNEL_INTR_WNDBUF_DED : 0UL;
}

#ifdef BCM8915X_INDIVIDUAL_INTR
/**
    @brief Get Memory initialization interrupt status

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC

*/
static void HSADC_IntGetMemInitInterruptStatus (HSADC_HwIDType hwId, HSADC_NonChannelIntrType intrType, uint32_t *aStatus) 
{

    /* Read Event status for all the interrupt*/
    if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_ERR)>0UL) {
        *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr &HSADC_INTR_AHMISIE_P_WNDBUF_MEM_INIT_DONE_INTR_MASK) >> HSADC_INTR_AHMISIE_P_WNDBUF_MEM_INIT_DONE_INTR_SHIFT;
    } else if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_DONE) >0UL) {
        *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr & HSADC_INTR_AHMISIE_P_WNDBUF_MEM_INIT_DONE_INTR_MASK) >> HSADC_INTR_AHMISIE_P_WNDBUF_MEM_INIT_DONE_INTR_SHIFT;
    } else if ((intrType & HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_ERR) >0UL) {
        *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr & HSADC_INTR_AHMISIE_P_FFTBUF_MEM_INIT_ERR_INTR_MASK)>>HSADC_INTR_AHMISIE_P_FFTBUF_MEM_INIT_ERR_INTR_SHIFT;
    } else if ((intrType & HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_DONE) >0UL) {
        *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr &HSADC_INTR_AHMISIE_P_FFTBUF_MEM_INIT_DONE_INTR_MASK) >>HSADC_INTR_AHMISIE_P_FFTBUF_MEM_INIT_DONE_INTR_SHIFT;
    } else if ((intrType & HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_ERR) >0UL) {
        *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr & HSADC_INTR_AHMISIE_P_FFT_MEM_INIT_ERR_INTR_MASK) >> HSADC_INTR_AHMISIE_P_FFT_MEM_INIT_ERR_INTR_SHIFT;
    } else if ((intrType & HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_DONE) >0UL) {
        *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr &HSADC_INTR_AHMISIE_P_FFT_MEM_INIT_DONE_INTR_MASK) >>HSADC_INTR_AHMISIE_P_FFT_MEM_INIT_DONE_INTR_SHIFT;
    } else if ((intrType & HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_ERR) >0UL) {
        *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr & HSADC_INTR_AHMISIE_P_ADCBUF_MEM_INIT_ERR_INTR_MASK) >> HSADC_INTR_AHMISIE_P_ADCBUF_MEM_INIT_ERR_INTR_SHIFT ;
    } else if ((intrType & HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_DONE) >0UL) {
        *aStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr & HSADC_INTR_AHMISIE_P_ADCBUF_MEM_INIT_DONE_INTR_MASK) >> HSADC_INTR_AHMISIE_P_ADCBUF_MEM_INIT_DONE_INTR_SHIFT;
    } else {
        /*Do nothing*/
    }
}
#endif

/** @brief HSADC Get non Channel interrupt Status

    This API gets HSADC non channel based interrupts status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId                  Index of the HSADC controller
    @param[in]      intrType             HSADC interrupt type
    @param[out]     intrStatus           HSADC interrupt Status
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @port  None

    @trace  #BRCM_SWREQ_HSADC_INTERRUPT
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_INTR_STATUS_PROC

    @code{.unparsed}
    if intrType == HSADC_NONCHANNEL_INTR_ALL
        Get CALIB_ERR, CALIB_DONE, MODE_COMPLETE from acq_hsadc_func_intr_eipr
        Get FFT_DATAPATH_ERR, FFT_IN_ERR from acq_hsadc_func_err_intr_eipr
        Get WNDBUF_MEM_INIT_ERR, WNDBUF_MEM_INIT_DONE, WNDBUF_SEC, FFTBUF_MEM_INIT_ERR
              FFTBUF_MEM_INIT_DONE, FFT_MEM_INIT_ERR, FFT_MEM_INIT_DONE, ADCBUF_MEM_INIT_ERR,
              ADCBUF_MEM_INIT_DONE from  acq_hsadc_mem_init_sec_intr_eipr
        Get WNDBUF_DED from acq_hsadc_ded_intr_eipr
    else
        if intrType == HSADC_NONCHANNEL_INTR_CALIB_ERR
            Get CALIB_ERR from acq_hsadc_func_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_CALIB_DONE
            Get CALIB_DONE from acq_hsadc_func_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE
            Get MODE_COMPLETE from acq_hsadc_func_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR
            Get FFT_DATAPATH_ERR from acq_hsadc_func_err_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_FFT_IN_ERR
            Get FFT_IN_ERR from acq_hsadc_func_err_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_ERR
            Get WNDBUF_MEM_INIT_ERR from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_WNDBUF_MEM_INIT_DONE
            Get WNDBUF_MEM_INIT_DONE from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_WNDBUF_SEC
            Get WNDBUF_SEC from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_ERR
            Get FFTBUF_MEM_INIT_ERR from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_FFTBUF_MEM_INIT_DONE
            Get FFTBUF_MEM_INIT_DONE from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_ERR
            Get FFT_MEM_INIT_ERR from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_FFT_MEM_INIT_DONE
            Get FFT_MEM_INIT_DONE from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_ERR
            Get ADCBUF_MEM_INIT_ERR from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_ADCBUF_MEM_INIT_DONE
            Get ADCBUF_MEM_INIT_DONE from acq_hsadc_mem_init_sec_intr_eipr
        if intrType == HSADC_NONCHANNEL_INTR_WNDBUF_DED
            Get WNDBUF_DED from acq_hsadc_ded_intr_eipr
     if intrType == HSADC_NONCHANNEL_INTR_GET_EVENT_ALL
        Get CALIB_ERR, CALIB_DONE, MODE_COMPLETE from acq_hsadc_func_intr_eisr
        Get FFT_DATAPATH_ERR, FFT_IN_ERR from acq_hsadc_func_err_intr_eisr
        Get WNDBUF_MEM_INIT_ERR, WNDBUF_MEM_INIT_DONE, WNDBUF_SEC, FFTBUF_MEM_INIT_ERR
              FFTBUF_MEM_INIT_DONE, FFT_MEM_INIT_ERR, FFT_MEM_INIT_DONE, ADCBUF_MEM_INIT_ERR,
              ADCBUF_MEM_INIT_DONE from  acq_hsadc_mem_init_sec_intr_eisr
        Get WNDBUF_DED from acq_hsadc_ded_intr_eisr

     @endcode


    @limitations None
*/

int32_t HSADC_DrvGetIntrStatus(HSADC_HwIDType  hwId,  HSADC_NonChannelIntrType intrType, uint32_t *intrStatus)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else {
#endif
        if (intrType == HSADC_NONCHANNEL_INTR_ALL) { /*Get all Interrupt status*/
            HSADC_IntGetAllInterruptStatus (hwId, intrStatus); 
        }
#ifdef BCM8915X_INDIVIDUAL_INTR
        else { /* Read specified interrupt pending*/
            if ((intrType & HSADC_NONCHANNEL_INTR_CALIB_ERR) >0UL) {
                *intrStatus = (HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eipr & HSADC_INTR_AHFIE_P_ADC_CALIB_ERR_INTR_MASK) >> HSADC_INTR_AHFIE_P_ADC_CALIB_ERR_INTR_SHIFT;
            } else if ((intrType & HSADC_NONCHANNEL_INTR_CALIB_DONE) >0UL) {
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eipr & HSADC_INTR_AHFIE_P_ADC_CALIB_DONE_INTR_MASK) >> HSADC_INTR_AHFIE_P_ADC_CALIB_DONE_INTR_SHIFT;
            } else if ((intrType & HSADC_NONCHANNEL_INTR_CAP_MODE_COMPLETE) >0UL) {
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_func_intr_eipr & HSADC_INTR_AHFIE_P_CAP_MODE_COMPLETE_INTR_MASK) >> HSADC_INTR_AHFIE_P_CAP_MODE_COMPLETE_INTR_SHIFT ;
            } else if ((intrType & HSADC_NONCHANNEL_INTR_FFT_DATAPATH_ERR)  >0UL) {
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eipr & HSADC_INTR_AHFEIE_P_FFT_DATAPATH_ERR_INTR_MASK) >> HSADC_INTR_AHFEIE_P_FFT_DATAPATH_ERR_INTR_SHIFT ;
            } else if ((intrType & HSADC_NONCHANNEL_INTR_FFT_IN_ERR) >0UL) {
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_func_err_intr_eipr & HSADC_INTR_AHFEIE_P_FFT_IN_ERR_INTR_MASK) >> HSADC_INTR_AHFEIE_P_FFT_IN_ERR_INTR_SHIFT;
            } else if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_SEC) >0UL) {
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_mem_init_sec_intr_eipr & HSADC_INTR_AHMISIE_P_WNDBUF_SEC_INTR_MASK) >> HSADC_INTR_AHMISIE_P_WNDBUF_SEC_INTR_SHIFT;
            } else if ((intrType & HSADC_NONCHANNEL_INTR_WNDBUF_DED) >0UL) {
                *intrStatus |= (HSADC_IntrRegs[hwId]->acq_hsadc_ded_intr_eipr & HSADC_INTR_ACQ_HSADC_DED_INTR_EIPR_P_WNDBUF_MASK);
            } else {
                HSADC_IntGetMemInitInterruptStatus (hwId, intrType, intrStatus) ;
            }
        }
#endif
        if (intrType == HSADC_NONCHANNEL_INTR_GET_EVENT_ALL) { /* Read all events*/
           HSADC_IntGetEventStatus (hwId, intrStatus) ;
        }
#ifdef BCM8915X_PARAM_VALIDATION
}
#endif
    return ret;
}

/** @brief HSADC get buffer Status

    This API gets HSADC buffer status based on the hwId.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the HSADC controller
    @param[in]      flag         Channel index and buffer clear. MSB 4 bits represents channel number.
                                 LSB 4 bits as follow,
                                 X1 : clear lost frame, X represents the channel number.
                                 X2 : clear buffer status, X represents the channel number.
    @param[out]     bufStatus    HSADC buffer A and B status.
                                 bit 0 and 1 represents Buffer A status
                                 bit 3 and 4 represents Buffer B status
                                 0 : Idle 1 : write phase
                                 2 : Read Done(RD_Done not observed)
                                 3 : Read Done(RD_Done observed)
    @param[out]     lostFrame    HSADC lost frame counter
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid bufStatus/calibStatus
    @retval     #BCM_ERR_NODEV          No valid device

    @post NONE 

    @trace  #BRCM_SWREQ_HSADC_CONFIGURATION
    @trace  #BRCM_SWARCH_HSADC_DRV_GET_BUF_STS_PROC

    @code{.unparsed}
         Check hardware instance, bufStatus and lostFrame.
         Read buffer status based on the channel.

    @endcode


    @limitations None
*/
int32_t HSADC_DrvGetBufStatus(HSADC_HwIDType  hwId,  uint8_t flag, uint8_t *bufStatus, uint16_t *lostFrame)
{
    int32_t ret = BCM_ERR_OK;
    uint8_t chIndex = (flag & 0xF0U) >> 4U;
    volatile uint32_t *bufSts = NULL; 
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= HSADC_MAX_HW_ID) { /* Check HWID*/
        ret = BCM_ERR_INVAL_PARAMS;
    } else if  (HSADC_DrvDev[hwId].state == HSADC_STATE_UNINIT) {/* Check device state*/
        ret = BCM_ERR_NODEV;
    } else if (bufStatus == NULL || lostFrame == NULL) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (chIndex >= 4U ) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        bufSts = (volatile uint32_t *)((uintptr_t)((uint32_t)&HSADC_Regs[hwId]->acq_ch0_buf_status + chIndex));
        
        /* read buffer status*/
        *bufStatus = (uint8_t)(*bufSts & 0xFU);
        *lostFrame = (uint16_t)((*bufSts & HSADC_ACQ_BUF_STATUS_LOST_FRAMES_MASK) >> HSADC_ACQ_BUF_STATUS_LOST_FRAMES_SHIFT);
        if ((flag & 0x1U) == 0x1U) {
            *(bufSts + hwId) |= HSADC_ACQ_BUF_STATUS_LOST_FRAMES_CLR_MASK; 
        }
        if ((flag & 0x2U) == 0x2U) {
            *(bufSts + hwId) |= HSADC_ACQ_BUF_STATUS_BUF_STATUS_CLR_MASK; 
        }

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}


/** @} */


