/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.
 Broadcom Proprietary and Confidential.
******************************************************************************/

/**
    @defgroup grp_unicam_hw_impl Core Driver Implementation
    @ingroup grp_unicamdrv

    @addtogroup grp_unicam_hw_impl
    @{
    @section sec_unicam_impl_overview Overview
    This layer implements exposed driver APIs through UNICAM register
    interfaces.

    @section sec_unicam_impl_fsm State Machine
    @todo Add state machine

    @section sec_unicam_impl_seq Sequence Diagrams
    @todo Add state machine

    @file unicam_drv.c
    @brief UNICAM core driver implementation
    This source file contains the implementation of functions for UNICAM
    @version 0.1 Initial version
*/

#include <unicam.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <unicam_rdb.h>
#include <string.h>


/**
    @name UNICAM HW Design IDs
    @{
    @brief Design IDs for UNICAM
*/
#define BRCM_SWDSGN_UNICAM_DRV_INT_MASK_ALL_MACRO              (0xB801U)    /**< @brief #UNICAM_DRV_INT_MASK_ALL         */
#define BRCM_SWDSGN_UNICAM_DRV_INT_ERROR_MASK_MACRO            (0xB802U)    /**< @brief #UNICAM_DRV_INT_ERROR_MASK       */
#define BRCM_SWDSGN_UNICAM_DRV_DATA_ERROR_MASK_MACRO           (0xB803U)    /**< @brief #UNICAM_DRV_DATA_ERROR_MASK      */
#define BRCM_SWDSGN_UNICAM_DRV_CLK_ERROR_MASK_MACRO            (0xB804U)    /**< @brief #UNICAM_DRV_CLK_ERROR_MASK       */
#define BRCM_SWDSGN_UNICAM_DRV_ERROR_TOLERANCE_MACRO           (0xB805U)    /**< @brief #UNICAM_DRV_ERROR_TOLERANCE      */
#define BRCM_SWDSGN_UNICAM_DRV_DONE_MACRO                      (0xB806U)    /**< @brief #UNICAM_DRV_DONE_EXP_NONE        */
#define BRCM_SWDSGN_UNICAM_DRV_REG_MACRO                       (0xB807U)    /**< @brief #UNICAM_DRV_REG                  */
#define BRCM_SWDSGN_UNICAM_DRV_FIELD_WRITE_MACRO               (0xB808U)    /**< @brief #UNICAM_DRV_FIELD_WRITE          */
#define BRCM_SWDSGN_UNICAM_DRV_ED_START_LINES_MACRO            (0xB809U)    /**< @brief #UNICAM_DRV_ED_START_LINES       */
#define BRCM_SWDSGN_UNICAM_DRV_ED_END_LINES_MACRO              (0xB80AU)    /**< @brief #UNICAM_DRV_ED_END_LINES         */
#define BRCM_SWDSGN_UNICAM_DRV_STATE_TYPE                      (0xB80BU)    /**< @brief #UNICAM_DrvStateType             */
#define BRCM_SWDSGN_UNICAM_DRV_CONTEXT_TYPE                    (0xB80DU)    /**< @brief #UNICAM_DrvContextType           */
#define BRCM_SWDSGN_UNICAM_DRV_CONTEXT_GLOBAL                  (0xB80EU)    /**< @brief #UNICAM_DrvContext               */
#define BRCM_SWDSGN_UNICAM_DRV_BUF_CONTEXT_TYPE                (0xB80FU)    /**< @brief #UNICAM_DrvBufContextType        */
#define BRCM_SWDSGN_UNICAM_DRV_BUF_CONTEXT_GLOBAL              (0xB810U)    /**< @brief #UNICAM_DrvBufContext            */
#define BRCM_SWDSGN_UNICAM_DRV_MODE_GLOBAL                     (0xB811U)    /**< @brief #UNICAM_DrvMode                  */
#define BRCM_SWDSGN_UNICAM_DRV_REGS_GLOBAL                     (0xB812U)    /**< @brief #UNICAM_DRV_REGS                 */
#define BRCM_SWDSGN_UNICAM_DRV_REPORT_ERROR_PROC               (0xB813U)    /**< @brief #UNICAM_DrvReportError           */
#define BRCM_SWDSGN_UNICAM_DRV_SET_STATE_PROC                  (0xB814U)    /**< @brief #UNICAM_DrvSetState              */
#define BRCM_SWDSGN_UNICAM_DRV_INITSTATE_PROC                  (0xB815U)    /**< @brief #UNICAM_DrvInitState             */
#define BRCM_SWDSGN_UNICAM_DRV_INITBUF_CONTEXT_PROC            (0xB816U)    /**< @brief #UNICAM_DrvInitBufContext        */
#define BRCM_SWDSGN_UNICAM_DRV_RESET_PROC                      (0xB817U)    /**< @brief #UNICAM_DrvReset                 */
#define BRCM_SWDSGN_UNICAM_DRV_PROCESS_FS_INTERRUPT_PROC       (0xB818U)    /**< @brief #UNICAM_DrvProcessFSInterrupt    */
#define BRCM_SWDSGN_UNICAM_DRV_PROCESS_FE_INTERRUPT_PROC       (0xB819U)    /**< @brief #UNICAM_DrvProcessFEInterrupt    */
#define BRCM_SWDSGN_UNICAM_DRV_PROCESS_DI_INTERRUPT_PROC       (0xB81AU)    /**< @brief #UNICAM_DrvProcessDIInterrupt    */
#define BRCM_SWDSGN_UNICAM_DRV_PROCESS_INTERRUPT_PROC          (0xB81BU)    /**< @brief #UNICAM_DrvProcessInterrupt      */
#define BRCM_SWDSGN_UNICAM_DRV_PROCESS_ERROR_INTERRUPT_PROC    (0xB81CU)    /**< @brief #UNICAM_DrvProcessErrorInterrupt */
#define BRCM_SWDSGN_UNICAM_DRV_INIT_PROC                       (0xB81DU)    /**< @brief #UNICAM_DrvInit                  */
#define BRCM_SWDSGN_UNICAM_DRV_DEINIT_PROC                     (0xB81EU)    /**< @brief #UNICAM_DrvDeinit                */
#define BRCM_SWDSGN_UNICAM_DRV_START_PROC                      (0xB81FU)    /**< @brief #UNICAM_DrvStart                 */
#define BRCM_SWDSGN_UNICAM_DRV_STOP_PROC                       (0xB820U)    /**< @brief #UNICAM_DrvStop                  */
#define BRCM_SWDSGN_UNICAM_DRV_GET_FORMAT_INFO_PROC            (0xB821U)    /**< @brief #UNICAM_DrvGetFormatInfo         */
#define BRCM_SWDSGN_UNICAM_DRV_CSI_CONFIGURE_TIMING_PROC       (0xB822U)    /**< @brief #UNICAM_DrvCSI_ConfigureTiming   */
#define BRCM_SWDSGN_UNICAM_DRV_CSI_CONFIGURE_IMGID_PROC        (0xB823U)    /**< @brief #UNICAM_DrvCSI_ConfigureImageID  */
#define BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_LSYNC_PROC            (0xB824U)    /**< @brief #UNICAM_DrvCSI_ConfigureLineSync */
#define BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_TG_PROC               (0xB825U)    /**< @brief #UNICAM_DrvConfigureTG           */
#define BRCM_SWDSGN_UNICAM_DRV_BPP2_EXP_CODE_PROC              (0xB826U)    /**< @brief #UNICAM_DrvBPP2ExpCode           */
#define BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_HDR_PROC              (0xB827U)    /**< @brief #UNICAM_DrvConfigureHDR          */
#define BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_DATA_PROC             (0xB828U)    /**< @brief #UNICAM_DrvConfigureData         */
#define BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_FSZ_PROC              (0xB829U)    /**< @brief #UNICAM_DrvConfigureFrameSize    */
#define BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_PRI_PROC              (0xB82AU)    /**< @brief #UNICAM_DrvConfigurePri          */
#define BRCM_SWDSGN_UNICAM_DRV_PRE_CONFIGURE_CHECK_PROC        (0xB82BU)    /**< @brief #UNICAM_DrvPreConfigureCheck     */
#define BRCM_SWDSGN_UNICAM_DRV_PROGRAM_BUF_CHECK_PROC          (0xB82CU)    /**< @brief #UNICAM_DrvProgramBufCheck       */
#define BRCM_SWDSGN_UNICAM_DRV_PROGRAM_BUF_PROC                (0xB82DU)    /**< @brief #UNICAM_DrvProgramBuf            */
#define BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_CSI_PROC              (0xB82EU)    /**< @brief #UNICAM_DrvConfigureCSI          */
#define BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_PROC                  (0xB82FU)    /**< @brief #UNICAM_DrvConfigure             */
#define BRCM_SWDSGN_UNICAM_DRV_GET_EVENT_PROC                  (0xB830U)    /**< @brief #UNICAM_DrvGetEvent              */
#define BRCM_SWDSGN_UNICAM_DRV_GET_CUR_BUF_IDX_PROC            (0xB831U)    /**< @brief #UNICAM_DrvGetCurBufIdx          */
#define BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUFFER_PROC             (0xB832U)    /**< @brief #UNICAM_DrvGetEmbBuffer          */
#define BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUF_PTR_PROC            (0xB833U)    /**< @brief #UNICAM_DrvGetEmbBufPtr          */
#define BRCM_SWDSGN_UNICAM_DRV_UPDATE_FRAME_NUM_PROC           (0xB835U)    /**< @brief #UNICAM_DrvUpdateFrameNum        */
#define BRCM_SWDSGN_UNICAM_DRV_IRQ_HANDLER_PROC                    (0xB836U)    /**< @brief #UNICAM_DrvIRQHandler       */
#define BRCM_SWDSGN_UNICAM_DRV_IRQ_ERR_HANDLER_PROC                (0xB837U)    /**< @brief #UNICAM_DrvIRQErrHandler    */
#define BRCM_SWDSGN_UNICAM_UPDATE_CAMDB_SA_EA_PROC                (0xB838U)    /**< @brief #UNICAM_UpdateCamdbSaEa*/
#define BRCM_SWDSGN_UNICAM_DRV_DIS_DATA_LANE_PROC                (0xB83AU)    /**< @brief #UNICAM_DrvDisableDataLane*/
#define BRCM_SWDSGN_UNICAM_DRV_PRE_CONFIR_PARAM_CHK_PROC        (0xB83BU)    /**< @brief #UNICAM_DrvPreConfigParamCheck*/
/** @} */

/**
    @brief Interrupt mask for UNICAM

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT
*/
#define UNICAM_DRV_INT_MASK_ALL  (UNICAM_ICTL_AXIERR_INT_EN_MASK | \
                UNICAM_ICTL_SBE_EN_MASK | \
                UNICAM_ICTL_PBE_EN_MASK | \
                UNICAM_ICTL_HOE_EN_MASK | \
                UNICAM_ICTL_PLE_EN_MASK | \
                UNICAM_ICTL_CLSTE_EN_MASK | \
                UNICAM_ICTL_DLSTE_EN_LN0_MASK | \
                UNICAM_ICTL_DLSTE_EN_LN1_MASK | \
                UNICAM_ICTL_DLSTE_EN_LN2_MASK | \
                UNICAM_ICTL_DLSTE_EN_LN3_MASK | \
                UNICAM_ICTL_CRCE_EN_MASK)

/**
    @brief Error status mask for UNICAM

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM
*/
#define UNICAM_DRV_INT_ERROR_MASK (UNICAM_STA_TO_HW_SYNC_MASK | \
                          UNICAM_STA_READ_256CROSS_MASK | \
                          UNICAM_STA_WRITE_256CROSS_MASK | \
                          UNICAM_STA_AXIERR_MASK | \
                          UNICAM_STA_FIFO_W_ERR_MASK | \
                          UNICAM_STA_PS_MASK | \
                          UNICAM_STA_DL_MASK | \
                          UNICAM_STA_BFO_MASK | \
                          UNICAM_STA_OFO_MASK | \
                          UNICAM_STA_IFO_MASK | \
                          UNICAM_STA_CRCE_MASK | \
                          UNICAM_STA_PLE_MASK | \
                          UNICAM_STA_HOE_MASK | \
                          UNICAM_STA_PBE_MASK | \
                          UNICAM_STA_SBE_MASK)

/**
    @brief Data lane error mask for UNICAM

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM
*/
#define UNICAM_DRV_DATA_ERROR_MASK (UNICAM_DAT0_DLSTEN_MASK | \
                UNICAM_DAT0_DLFON_MASK | \
                UNICAM_DAT0_DLSEN_MASK)

/**
    @brief Clock error mask for UNICAM

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM
*/
#define UNICAM_DRV_CLK_ERROR_MASK (UNICAM_CLK_CLSTE_MASK)

/**
    @brief UNICAM error tolerance

    Wait for so many errors before reporting to upper layer.

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM
*/
#define UNICAM_DRV_ERROR_TOLERANCE (5UL)

/**
    @name UNICAM done status for exposures
    @{
    @brief Macro for UNICAM done status for exposures

    Macro to keep track of signals for LONG and SHORT exposures.

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT
*/
#define UNICAM_DRV_DONE_EXP_NONE  (0x0UL) /**< @brief None completed           */
#define UNICAM_DRV_DONE_EXP_LONG  (0x1UL) /**< @brief Long exposure completed  */
#define UNICAM_DRV_DONE_EXP_SHORT (0x2UL) /**< @brief Short exposure completed */
#define UNICAM_DRV_DONE_EXP_ALL   (0x3UL) /**< @brief Both exposure completed  */
/** @} */

/**
    @brief Register pointer expansion

    Gets an element in UNICAM register structure.

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_START_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT
*/
#define UNICAM_DRV_REG(x)  (UNICAM_DRV_REGS[UNICAM_MAX_HW_ID-1UL]->x)

/**
    @brief Write @a value to @a reg with field details specified

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT
*/
#define UNICAM_DRV_FIELD_WRITE(reg, shift, mask, value) \
            UNICAM_DRV_REG(reg) &= ~(mask); \
            UNICAM_DRV_REG(reg) |= (((value) << (shift)) & (mask));

/**
    @brief Get ED before frame for exposure index

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
*/
#define UNICAM_DRV_ED_START_LINES(x, y) (((uint32_t) y[(x)] & \
                            UNICAM_MODECFG_META_LINES_START_MASK) >> \
                            UNICAM_MODECFG_META_LINES_START_SHIFT)

/**
    @brief Get ED after frame for exposure index

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
*/
#define UNICAM_DRV_ED_END_LINES(x, y)   (((uint32_t) y[(x)] & \
                            UNICAM_MODECFG_META_LINES_END_MASK) >> \
                            UNICAM_MODECFG_META_LINES_END_SHIFT)

/**
    @name UNICAM State
    @{
    @brief UNICAM state type

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_START_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EVENT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_CUR_BUF_IDX_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUFFER_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUF_PTR_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT
*/
typedef uint32_t UNICAM_DrvStateType;       /**< @brief typedef for UNICAM states */
#define UNICAM_DRV_STATE_POWEROFF    (0UL)  /**< @brief UNICAM State Poweroff     */
#define UNICAM_DRV_STATE_INITIALISED (1UL)  /**< @brief UNICAM State Initialized  */
#define UNICAM_DRV_STATE_READY       (2UL)  /**< @brief UNICAM State Ready        */
#define UNICAM_DRV_STATE_ENABLED     (3UL)  /**< @brief UNICAM State Enabled      */
#define UNICAM_DRV_STATE_ERROR       (4UL)  /**< @brief UNICAM State Error        */
/** @} */


/**
    @brief UNICAM internal context

    UNICAM internal context for maintaining state of the driver.

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_START_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EVENT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_CUR_BUF_IDX_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUFFER_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUF_PTR_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT
*/
typedef struct sUNICAM_DrvContextType {
    UNICAM_DrvStateType state;     /**< @brief Current state of UNICAM driver */
    UNICAM_DrvStateType prevState; /**< @brief Previous state of UNICAM driver
                                                                              */
    volatile uint32_t event;       /**< @brief Event received from UNICAM HW  */
    uint8_t numDataLanes;          /**< @brief Number of data lanes enabled   */
    uint32_t interruptEnable;      /**< @brief Interrupts enabled for UNICAM  */
    uint32_t interruptEnableSE;    /**< @brief Interrupts enabled for short
                                                                    exposure  */
    uint32_t edInterruptMask;      /**< @brief Interrupt mask for ED          */
    uint32_t status;               /**< @brief Status saved in case of error  */
    uint32_t dataLaneStatus[4UL];  /**< @brief Data lane status saved in case
                                                                     of error */
    uint32_t clockLaneStatus;      /**< @brief Clocklane status saved in case
                                                                     of error */
    uint32_t numContErrors;        /**< @brief Number of continous errors     */
    uint32_t fsRxd;                /**< @brief Frame-Start status for exposure
                                                                              */
    uint32_t feRxd;                /**< @brief Frame-End status for exposure  */
    uint32_t isHDR;                /**< @brief Is HDR enabled?                */
    uint32_t dcsRxd;               /**< @brief DCS status for exposure        */
    uint32_t dcsEndRxd;            /**< @brief DCS-End status for exposure    */
    uint32_t dcsRxdMask;           /**< @brief Mask for DCS received          */
    uint32_t dcsEndRxdMask;        /**< @brief Mask for DCS-END received      */
    uint32_t curFrameNum;          /**< @brief Current frame number           */
} UNICAM_DrvContextType;

/**
    @brief Global context for UNICAM_DrvContextType

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_START_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EVENT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_CUR_BUF_IDX_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUFFER_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUF_PTR_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT
*/
static UNICAM_DrvContextType UNICAM_DrvContext /*COMP_SECTION(".bss.drivers")*/;

/**
    @brief UNICAM buffer context

    UNICAM internal context for maintaining buffers

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
*/
typedef struct sUNICAM_DrvBufContextType {
    UNICAM_BufParamType buf;                                /**< @brief Buffer parameters                     */
    UNICAM_CurED_ParamType curED[UNICAM_OUTPUT_IDX_MAX];    /**< @brief Current ED address                    */
    UNICAM_CurED_ParamType curEDEnd[UNICAM_OUTPUT_IDX_MAX]; /**< @brief Current ED end address                */
    uint32_t activeEDIndex;                                 /**< @brief Active index for ED address           */
    uint32_t toggleImgBuf;                                  /**< @brief Is ping pong enabled for image buffer */
    uint32_t activeIBIndex;                                 /**< @brief Active image buffer index             */
    uint32_t filledBufIdx;                                  /**< @brief Filled buffer index                   */
} UNICAM_DrvBufContextType;

/**
    @brief Global context for UNICAM_DrvBufContextType

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_CUR_BUF_IDX_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUFFER_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUF_PTR_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
*/
static UNICAM_DrvBufContextType UNICAM_DrvBufContext /*COMP_SECTION(".bss.drivers")*/;

/**
    @brief Global context for UNICAM_DrvMode

    @trace #BRCM_SWARCH_UNICAM_DRV_START_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
*/
static UNICAM_ModeType UNICAM_DrvMode /*COMP_SECTION(".bss.drivers")*/;

/**
    @brief UNICAM register memory

    Pointer to UNICAM register memory.

    @anchor UNICAM_RDBType
    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_START_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT
*/
static UNICAM_RDBType *const UNICAM_DRV_REGS[UNICAM_MAX_HW_ID] = {
#if (UNICAM_MAX_HW_ID == 0)
#error "UNICAM_MAX_HW_ID == 0"
#endif
    (UNICAM_RDBType *)UNICAM_BASE,
#if (UNICAM_MAX_HW_ID > 1)
#error "UNICAM_MAX_HW_ID > 1 is not supported"
#endif
};

/** @brief UNICAM Report Error method

    Wrapper for BCM_ReportError.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aApiID      API ID for the method
    @param[in]      aErr        Error type
    @param[in]      aVal1       First uint32_t data to be printed
    @param[in]      aVal2       Second uint32_t data to be printed
    @param[in]      aVal3       Third uint32_t data to be printed

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_START_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT

    @limitations None

    @code{.unparsed}
    Call #BCM_ReportError
    @endcode
*/
static void UNICAM_DrvReportError(uint16_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    /*const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
/    BCM_ReportError(BCM_UCM_ID, 0U, aApiID, aErr, 4UL, values);*/
}

/** @brief Interface to set internal state

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aForceState   Force state change even if the
                                  current state is same
    @param[in]      aState        New state

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT

    @limitations None

    @code{.unparsed}
    if current state is not same as new state or aForceState is 1
        set current state to prevState
        set new state to current state
    @endcode
*/
static void UNICAM_DrvSetState(uint32_t aForceState, UNICAM_DrvStateType aState)
{
    if ((aState != UNICAM_DrvContext.state) || (1UL == aForceState)) {
        UNICAM_DrvContext.prevState = UNICAM_DrvContext.state;
        UNICAM_DrvContext.state = aState;
    }
}

/** @brief UNICAM Context initializer

    Initialize members of UNICAM context

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT

    @limitations None

    @code{.unparsed}
    UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_POWEROFF)
    context.numDataLanes = 0
    context.interruptEnable = 0
    context.interruptEnableSE = 0
    context.edInterruptMask = 0
    context.status = 0
    context.dataLaneStatus[0] = 0
    context.dataLaneStatus[1] = 0
    context.dataLaneStatus[2] = 0
    context.dataLaneStatus[3] = 0
    context.clockLaneStatus = 0
    context.numContErrors = 0
    context.fsRxd = UNICAM_DRV_DONE_EXP_NONE
    context.feRxd = UNICAM_DRV_DONE_EXP_NONE
    context.isHDR = 0
    context.dcsRxd = 0
    context.dcsEndRxd = UNICAM_DRV_DONE_EXP_NONE
    context.dcsRxdMask = UNICAM_DRV_DONE_EXP_NONE
    UNICAM_DrvContext.event = 0
    UNICAM_DrvContext.curFrameNum = 0
    @endcode
*/
static void UNICAM_DrvInitState(void)
{
    /* Initialize State */
    UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_POWEROFF);
    UNICAM_DrvContext.numDataLanes = 0U;
    UNICAM_DrvContext.interruptEnable = 0UL;
    UNICAM_DrvContext.interruptEnableSE = 0UL;
    UNICAM_DrvContext.edInterruptMask = 0UL;
    UNICAM_DrvContext.status = 0UL;
    UNICAM_DrvContext.dataLaneStatus[0] = 0UL;
    UNICAM_DrvContext.dataLaneStatus[1] = 0UL;
    UNICAM_DrvContext.dataLaneStatus[2] = 0UL;
    UNICAM_DrvContext.dataLaneStatus[3] = 0UL;
    UNICAM_DrvContext.clockLaneStatus = 0UL;
    UNICAM_DrvContext.numContErrors = 0UL;
    UNICAM_DrvContext.fsRxd = UNICAM_DRV_DONE_EXP_NONE;
    UNICAM_DrvContext.feRxd = UNICAM_DRV_DONE_EXP_NONE;
    UNICAM_DrvContext.isHDR = 0UL;
    UNICAM_DrvContext.dcsRxd = UNICAM_DRV_DONE_EXP_NONE;
    UNICAM_DrvContext.dcsEndRxd = UNICAM_DRV_DONE_EXP_NONE;
    UNICAM_DrvContext.dcsRxdMask = 0UL;
    UNICAM_DrvContext.dcsEndRxdMask = 0UL;
    UNICAM_DrvContext.event = 0UL;
    UNICAM_DrvContext.curFrameNum = 0UL;
}

/** @brief UNICAM Buffer Context initializer

    Initialize members of UNICAM Buffer context

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    context.curED[UNICAM_OUTPUT_IDX_LONG].ed.addr = 0
    context.curED[UNICAM_OUTPUT_IDX_SHORT].ed.addr = 0
    context.curEDEnd[UNICAM_OUTPUT_IDX_LONG].ed.addr = 0
    context.curEDEnd[UNICAM_OUTPUT_IDX_SHORT].ed.addr = 0
    context.activeEDIndex = 0
    context.toggleImgBuf = 0
    context.activeIBIndex = 0
    context.filledBufIdx = 0
    UNICAM_DrvMode = UNICAM_MODE_MAX
    @endcode
*/
static void UNICAM_DrvInitBufContext(void)
{
    UNICAM_DrvBufContext.curED[UNICAM_OUTPUT_IDX_LONG].ed.addr = 0UL;
    UNICAM_DrvBufContext.curED[UNICAM_OUTPUT_IDX_SHORT].ed.addr = 0UL;
    UNICAM_DrvBufContext.curEDEnd[UNICAM_OUTPUT_IDX_LONG].ed.addr = 0UL;
    UNICAM_DrvBufContext.curEDEnd[UNICAM_OUTPUT_IDX_SHORT].ed.addr = 0UL;
    UNICAM_DrvBufContext.activeEDIndex = 0UL;
    UNICAM_DrvBufContext.toggleImgBuf = 0UL;
    UNICAM_DrvBufContext.activeIBIndex = 0UL;
    UNICAM_DrvBufContext.filledBufIdx = 0UL;
    UNICAM_DrvMode = UNICAM_MODE_MAX;
}

/** @brief Program priority control

    Program priority control configurations

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT

    @limitations None

    @code{.unparsed}
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_PT_SHIFT,
                   UNICAM_PRI_PT_MASK,
                   3UL)
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_NP_SHIFT,
                   UNICAM_PRI_NP_MASK,
                   8UL)
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_PP_SHIFT,
                   UNICAM_PRI_PP_MASK,
                   0xeUL)
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_BS_SHIFT,
                   UNICAM_PRI_BS_MASK,
                   0UL)
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_BL_SHIFT,
                   UNICAM_PRI_BL_MASK,
                   0UL)
    UNICAM_DRV_REG(campri) |= UNICAM_PRI_PE_MASK
    @endcode
*/
static void UNICAM_DrvConfigurePri(void)
{
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_PT_SHIFT,
                   UNICAM_PRI_PT_MASK,
                   3UL /* 90% */);
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_NP_SHIFT,
                   UNICAM_PRI_NP_MASK,
                   8UL); 
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_PP_SHIFT,
                   UNICAM_PRI_PP_MASK,
                   0xeUL);
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_BS_SHIFT,
                   UNICAM_PRI_BS_MASK,
                   0UL);
    UNICAM_DRV_FIELD_WRITE(campri,
                   UNICAM_PRI_BL_SHIFT,
                   UNICAM_PRI_BL_MASK,
                   0UL);
    UNICAM_DRV_REG(campri) |= UNICAM_PRI_PE_MASK;
}

/** @brief Reset UNICAM

    Reset UNICAM hardware

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT

    @limitations None

    @code{.unparsed}
    Save control registers
    uint32_t ictrl = UNICAM_DRV_REG(camictl)
    uint32_t dcs = UNICAM_DRV_REG(camdcs)
    uint32_t ictlSE = UNICAM_DRV_REG(camictl_se)

    Assert soft reset
    UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPR_MASK)
    UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPR_DPHY_MASK)

    De-Assert soft reset
    UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPR_DPHY_MASK)
    UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPR_MASK)

    Restore lost bits
    UNICAM_DRV_REG(camictl) = ictrl
    UNICAM_DRV_REG(camdcs) = dcs
    UNICAM_DRV_REG(camictl_se) = ictlSE

    Reconfig Priority control
    UNICAM_DrvConfigurePri()
    @endcode
*/
static void UNICAM_DrvReset(void)
{
    uint32_t ictrl = UNICAM_DRV_REG(camictl);
    uint32_t dcs = UNICAM_DRV_REG(camdcs);
    uint32_t ictlSE = UNICAM_DRV_REG(camictl_se);

    /* Assert soft reset */
    UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPR_MASK);
    UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPR_DPHY_MASK);

    /* De-Assert soft reset */
    UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPR_DPHY_MASK);
    UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPR_MASK);

    /* Restore lost bits */
    UNICAM_DRV_REG(camictl) = ictrl;
    UNICAM_DRV_REG(camdcs) = dcs;
    UNICAM_DRV_REG(camictl_se) = ictlSE;

    /* Configure Priority Control (Lost config due to reset) */
    UNICAM_DrvConfigurePri();
}

/**
    @brief Update CAM DB SA and EA

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

*/
static void UNICAM_UpdateCamdbSaEa(UNICAM_BufferType *ucmBuf, uint32_t nextIdx) 
{
    if ((0UL != ucmBuf->startAddr) && (0UL != ucmBuf->endAddr)) {
        UNICAM_DRV_REG(camdbsa0) = ucmBuf->startAddr;
        UNICAM_DRV_REG(camdbea0) = (ucmBuf->endAddr);
        if (0UL != UNICAM_DrvContext.isHDR) {
            ucmBuf = &(UNICAM_DrvBufContext.buf.dataBuf[nextIdx][UNICAM_OUTPUT_IDX_SHORT]);
            if ((0UL != ucmBuf->startAddr) && (0UL != ucmBuf->endAddr)) {
                UNICAM_DRV_REG(camdbsa1) = ucmBuf->startAddr;
                UNICAM_DRV_REG(camdbea1) = (ucmBuf->endAddr);
            }
        }
        UNICAM_DRV_REG(camctl) |= UNICAM_CTL_TRIG_FB_UPDATE_MASK;
    }
}

/** @brief Process Frame-Start interrupt

    Process Frame-Start interrupt and perform required control operations

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aImageStatus       CAMISTA value
    @param[in]   aImageStatusSE     CAMISTA_SE value

    @retval     #UNICAM_INT_FS if FS received for all enabled streams

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

    @code{.unparsed}
    retMask = 0
    if 0UL != (aImageStatus & UNICAM_ISTA_FSI_MASK)
        context.fsRxd |= UNICAM_DRV_DONE_EXP_LONG
    if 0UL !- (aImageStatusSE & UNICAM_ISTA_SE_FSI_SE_MASK)
        context.fsRxd |= UNICAM_DRV_DONE_EXP_SHORT
    if (0UL == context.isHDR) ||
        (UNICAM_DRV_DONE_EXP_ALL == context.fsRxd)
        nextIdx = (context.activeEDIndex +
                1UL)% UNICAM_MAX_NUM_DATA_BUFS
        ucmBuf = &(context.buf.dataBuf[nextIdx][UNICAM_OUTPUT_IDX_LONG])
        if (0UL != ucmBuf->startAddr) && (0UL != ucmBuf->endAddr)
            UNICAM_DRV_REG(camdbsa0) = ucmBuf->startAddr
            UNICAM_DRV_REG(camdbea0) = (ucmBuf->endAddr)
            if 0UL != context.isHDR
                ucmBuf = &(bufContext.buf.dataBuf[nextIdx][UNICAM_OUTPUT_IDX_SHORT])
                if (0UL != ucmBuf->startAddr) && (0UL != ucmBuf->endAddr)
                    UNICAM_DRV_REG(camdbsa1) = ucmBuf->startAddr
                    UNICAM_DRV_REG(camdbea1) = (ucmBuf->endAddr)
            UNICAM_DRV_REG(camctl) |= UNICAM_CTL_TRIG_FB_UPDATE_MASK
        if 0UL != bufContext.toggleImgBuf
            nextIdx = (bufContext.activeIBIndex +1UL)% bufContext.buf.numImgBufs
            ucmBuf = &(bufContext.buf.imgBuf[nextIdx][UNICAM_OUTPUT_IDX_LONG])
            if (0UL != ucmBuf->startAddr) && (0UL != ucmBuf->endAddr)
                UNICAM_DRV_REG(camibsa0) = ucmBuf->startAddr
                UNICAM_DRV_REG(camibea0) = (ucmBuf->endAddr - 16UL)
                if 0UL != context.isHDR
                    ucmBuf =
                        &(bufContext.buf.imgBuf[nextIdx][UNICAM_OUTPUT_IDX_SHORT])
                    if (0UL != ucmBuf->startAddr) && (0UL != ucmBuf->endAddr)
                        UNICAM_DRV_REG(camibsa1) = ucmBuf->startAddr;
                        UNICAM_DRV_REG(camibea1) = (ucmBuf->endAddr - 16UL)
                UNICAM_DRV_REG(camctl) |= UNICAM_CTL_TRIG_FB_UPDATE_MASK

        UNICAM_DrvContext.fsRxd = UNICAM_DRV_DONE_EXP_NONE
        retMask |= UNICAM_INT_FS

    return retMask
    @endcode
*/

static uint32_t UNICAM_DrvProcessFSInterrupt(uint32_t aImageStatus,
                                             uint32_t aImageStatusSE)
{
    UNICAM_BufferType *ucmBuf;
    uint32_t retMask = 0UL;

    if (0UL != (aImageStatus & UNICAM_ISTA_FSI_MASK)) {
        UNICAM_DrvContext.fsRxd |= UNICAM_DRV_DONE_EXP_LONG;
    }

    if (0UL != (aImageStatusSE & UNICAM_ISTA_SE_FSI_SE_MASK)) {
        UNICAM_DrvContext.fsRxd |= UNICAM_DRV_DONE_EXP_SHORT;
    }

    if ((0UL == UNICAM_DrvContext.isHDR) ||
        (UNICAM_DRV_DONE_EXP_ALL == UNICAM_DrvContext.fsRxd)) {
        /* Update hardware address */
        uint32_t nextIdx = (UNICAM_DrvBufContext.activeEDIndex +
                1UL)% UNICAM_MAX_NUM_DATA_BUFS;

        ucmBuf = &(UNICAM_DrvBufContext.buf.dataBuf[nextIdx][UNICAM_OUTPUT_IDX_LONG]);
        UNICAM_UpdateCamdbSaEa(ucmBuf, nextIdx);


        if (0UL != UNICAM_DrvBufContext.toggleImgBuf) {
            nextIdx = (UNICAM_DrvBufContext.activeIBIndex +
                            1UL)% UNICAM_DrvBufContext.buf.numImgBufs;
            ucmBuf = &(UNICAM_DrvBufContext.buf.imgBuf[nextIdx][UNICAM_OUTPUT_IDX_LONG]);
            if ((0UL != ucmBuf->startAddr) && (0UL != ucmBuf->endAddr)) {
                UNICAM_DRV_REG(camibsa0) = ucmBuf->startAddr;
                UNICAM_DRV_REG(camibea0) = (ucmBuf->endAddr - 16UL);
                if (0UL != UNICAM_DrvContext.isHDR) {
                    ucmBuf =
                        &(UNICAM_DrvBufContext.buf.imgBuf[nextIdx][UNICAM_OUTPUT_IDX_SHORT]);
                    if ((0UL != ucmBuf->startAddr) && (0UL != ucmBuf->endAddr)) {
                        UNICAM_DRV_REG(camibsa1) = ucmBuf->startAddr;
                        UNICAM_DRV_REG(camibea1) = (ucmBuf->endAddr - 16UL);
                    }
                }
                UNICAM_DRV_REG(camctl) |= UNICAM_CTL_TRIG_FB_UPDATE_MASK;
            }
        }

        UNICAM_DrvContext.fsRxd = UNICAM_DRV_DONE_EXP_NONE;
        retMask |= UNICAM_INT_FS;
    }

   return retMask;
}

/** @brief Process Frame-End interrupt

    Process Frame-End interrupt and perform required control operations

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aImageStatus       CAMISTA value
    @param[in]   aImageStatusSE     CAMISTA_SE value

    @retval     #UNICAM_INT_FE if FE received for all enabled streams

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

    @code{.unparsed}
    retMask = 0UL

    if 0UL != (aImageStatus & UNICAM_ISTA_FEI_MASK)
        bufContext.feRxd |= UNICAM_DRV_DONE_EXP_LONG

    if 0UL != (aImageStatusSE & UNICAM_ISTA_SE_FEI_SE_MASK)
        bufContext.feRxd |= UNICAM_DRV_DONE_EXP_SHORT

    if (0UL == bufContext.isHDR) || (UNICAM_DRV_DONE_EXP_ALL == context.feRxd)
        if 0UL != bufContext.toggleImgBuf
            bufContext.filledBufIdx = bufContext.activeIBIndex
            bufContext.activeIBIndex = (bufContext.activeIBIndex +
                            1UL)% bufContext.buf.numImgBufs
        bufContext.feRxd = UNICAM_DRV_DONE_EXP_NONE
        retMask |= UNICAM_INT_FE

    return retMask
    @endcode
*/
static uint32_t UNICAM_DrvProcessFEInterrupt(uint32_t aImageStatus,
                                             uint32_t aImageStatusSE)
{
    uint32_t retMask = 0UL;

    if (0UL != (aImageStatus & UNICAM_ISTA_FEI_MASK)) {
        UNICAM_DrvContext.feRxd |= UNICAM_DRV_DONE_EXP_LONG;
    }

    if (0UL != (aImageStatusSE & UNICAM_ISTA_SE_FEI_SE_MASK)) {
        UNICAM_DrvContext.feRxd |= UNICAM_DRV_DONE_EXP_SHORT;
    }

    if ((0UL == UNICAM_DrvContext.isHDR) ||
        (UNICAM_DRV_DONE_EXP_ALL == UNICAM_DrvContext.feRxd)) {

        if (0UL != UNICAM_DrvBufContext.toggleImgBuf) {
            UNICAM_DrvBufContext.filledBufIdx = UNICAM_DrvBufContext.activeIBIndex;
            UNICAM_DrvBufContext.activeIBIndex = (UNICAM_DrvBufContext.activeIBIndex +
                            1UL)% UNICAM_DrvBufContext.buf.numImgBufs;
        }
        UNICAM_DrvContext.feRxd = UNICAM_DRV_DONE_EXP_NONE;
        retMask |= UNICAM_INT_FE;
    }

    return retMask;
}

/** @brief Process data interrupt

    Process data interrupt and perform required control operations

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aDataStatus       CAMDCS value
    @param[in]  aImageStatusSE    CAMISTA_SE value

    @retval     uint32_t mask of UNICAM_INT_DI     if data available before frame
                             UNICAM_INT_DI_END if data available after frame

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

    @code{.unparsed}
    retMask = 0UL
    updateIndex = 0UL
    ictlSE = 0UL

    if 0UL != (aDataStatus & (UNICAM_DCS_DI_MASK))
        ucmBuf = &(bufContext.buf.dataBuf
                    [bufContext.activeEDIndex][UNICAM_OUTPUT_IDX_LONG])
        bufContext.curED[UNICAM_OUTPUT_IDX_LONG].ed.addr = ucmBuf->startAddr
        context.dcsRxd |= UNICAM_DRV_DONE_EXP_LONG

    if 0UL != (aImageStatusSE & (UNICAM_ISTA_SE_DI_SE_MASK))
        ucmBuf = &(bufContext.buf.dataBuf
                    [bufContext.activeEDIndex][UNICAM_OUTPUT_IDX_SHORT])
        bufContext.curED[UNICAM_OUTPUT_IDX_SHORT].ed.addr = ucmBuf->startAddr
        context.dcsRxd |= UNICAM_DRV_DONE_EXP_SHORT

    if 0UL != (aDataStatus & (UNICAM_DCS_DI_END_MASK))
        ucmBuf = &(bufContext.buf.dataBuf
                    [bufContext.activeEDIndex][UNICAM_OUTPUT_IDX_LONG])
        bufContext.curEDEnd[UNICAM_OUTPUT_IDX_LONG].ed.addr = ucmBuf->startAddr
        UNICAM_DrvBufContext.curEDEnd[UNICAM_OUTPUT_IDX_LONG].frameNum =
                                            UNICAM_DrvContext.curFrameNum
        context.dcsEndRxd |= UNICAM_DRV_DONE_EXP_LONG

    if 0UL != (aImageStatusSE & (UNICAM_ISTA_SE_DI_END_SE_MASK))
        ucmBuf = &(bufContext.buf.dataBuf
                    [bufContext.activeEDIndex][UNICAM_OUTPUT_IDX_SHORT])
        bufContext.curEDEnd[UNICAM_OUTPUT_IDX_SHORT].ed.addr = ucmBuf->startAddr
        UNICAM_DrvBufContext.curEDEnd[UNICAM_OUTPUT_IDX_SHORT].frameNum =
                                            UNICAM_DrvContext.curFrameNum
        context.dcsEndRxd |= UNICAM_DRV_DONE_EXP_SHORT

    ictlSE = UNICAM_DRV_REG(camictl_se)

    if (0UL != context.dcsRxdMask) && (context.dcsRxdMask == context.dcsRxd)
        context.dcsRxd = UNICAM_DRV_DONE_EXP_NONE
        retMask |= UNICAM_INT_DI
        if (0UL == (aDataStatus & UNICAM_DCS_EDL_END_MASK)) &&
            (0UL == (ictlSE & UNICAM_ICTL_SE_DI_END_SE_EN_MASK))
            updateIndex = 1UL;

    if (0UL != context.dcsEndRxdMask) &&
        (context.dcsEndRxdMask == context.dcsEndRxd)
        context.dcsEndRxd = UNICAM_DRV_DONE_EXP_NONE
        retMask |= UNICAM_INT_DI_END
        updateIndex = 1UL

    if 0UL != updateIndex
        bufContext.activeEDIndex = (bufContext.activeEDIndex +
                                       1UL)% UNICAM_MAX_NUM_DATA_BUFS
    return retMask
    @endcode
*/
static uint32_t UNICAM_DrvProcessDIInterrupt(uint32_t aDataStatus,
                                             uint32_t aImageStatusSE)
{
    uint32_t retMask = 0UL;
    uint32_t updateIndex = 0UL;
    uint32_t ictlSE = 0UL;
    UNICAM_BufferType *ucmBuf;

    /* store updated address */
    if (0UL != (aDataStatus & (UNICAM_DCS_DI_MASK))) {
        ucmBuf = &(UNICAM_DrvBufContext.buf.dataBuf
                    [UNICAM_DrvBufContext.activeEDIndex][UNICAM_OUTPUT_IDX_LONG]);
        UNICAM_DrvBufContext.curED[UNICAM_OUTPUT_IDX_LONG].ed.addr = ucmBuf->startAddr;
        UNICAM_DrvContext.dcsRxd |= UNICAM_DRV_DONE_EXP_LONG;
    }

    if (0UL != (aImageStatusSE & (UNICAM_ISTA_SE_DI_SE_MASK))) {
        ucmBuf = &(UNICAM_DrvBufContext.buf.dataBuf
                    [UNICAM_DrvBufContext.activeEDIndex][UNICAM_OUTPUT_IDX_SHORT]);
        UNICAM_DrvBufContext.curED[UNICAM_OUTPUT_IDX_SHORT].ed.addr = ucmBuf->startAddr;
        UNICAM_DrvContext.dcsRxd |= UNICAM_DRV_DONE_EXP_SHORT;
    }

    if (0UL != (aDataStatus & (UNICAM_DCS_DI_END_MASK))) {
        ucmBuf = &(UNICAM_DrvBufContext.buf.dataBuf
                    [UNICAM_DrvBufContext.activeEDIndex][UNICAM_OUTPUT_IDX_LONG]);
        UNICAM_DrvBufContext.curEDEnd[UNICAM_OUTPUT_IDX_LONG].ed.addr = ucmBuf->startAddr;
        UNICAM_DrvBufContext.curEDEnd[UNICAM_OUTPUT_IDX_LONG].frameNum =
                                            UNICAM_DrvContext.curFrameNum;
        UNICAM_DrvContext.dcsEndRxd |= UNICAM_DRV_DONE_EXP_LONG;
    }

    if (0UL != (aImageStatusSE & (UNICAM_ISTA_SE_DI_END_SE_MASK))) {
        ucmBuf = &(UNICAM_DrvBufContext.buf.dataBuf
                    [UNICAM_DrvBufContext.activeEDIndex][UNICAM_OUTPUT_IDX_SHORT]);
        UNICAM_DrvBufContext.curEDEnd[UNICAM_OUTPUT_IDX_SHORT].ed.addr = ucmBuf->startAddr;
        UNICAM_DrvBufContext.curEDEnd[UNICAM_OUTPUT_IDX_SHORT].frameNum =
                                            UNICAM_DrvContext.curFrameNum;
        UNICAM_DrvContext.dcsEndRxd |= UNICAM_DRV_DONE_EXP_SHORT;
    }

    /* Read ictl for SE */
    ictlSE = UNICAM_DRV_REG(camictl_se);

    if ((0UL != UNICAM_DrvContext.dcsRxdMask) &&
        (UNICAM_DrvContext.dcsRxdMask == UNICAM_DrvContext.dcsRxd)) {
        UNICAM_DrvContext.dcsRxd = UNICAM_DRV_DONE_EXP_NONE;
        retMask |= UNICAM_INT_DI;
        /* Check if we need to update Data buffer Index */
        if ((0UL == (aDataStatus & UNICAM_DCS_EDL_END_MASK)) &&
            (0UL == (ictlSE & UNICAM_ICTL_SE_DI_END_SE_EN_MASK))) {
            updateIndex = 1UL;
        }
    }

    if ((0UL != UNICAM_DrvContext.dcsEndRxdMask) &&
        (UNICAM_DrvContext.dcsEndRxdMask == UNICAM_DrvContext.dcsEndRxd)) {
        UNICAM_DrvContext.dcsEndRxd = UNICAM_DRV_DONE_EXP_NONE;
        retMask |= UNICAM_INT_DI_END;
        updateIndex = 1UL;
    }

    /* Update data buffer index */
    if (0UL != updateIndex) {
        UNICAM_DrvBufContext.activeEDIndex = (UNICAM_DrvBufContext.activeEDIndex +
                                       1UL)% UNICAM_MAX_NUM_DATA_BUFS;
    }

    return retMask;
}

/**
    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @code{.c}
    event = context.event
    mask = 0UL
    imageStatus = 0UL
    dataStatus = 0UL
    imageStatusSE = 0UL

    imageStatus = UNICAM_DRV_REG(camista)
    imageStatusSE = UNICAM_DRV_REG(camista_se)
    UNICAM_DRV_REG(camista) = imageStatus
    UNICAM_DRV_REG(camista_se) = imageStatusSE
    UNICAM_DRV_REG(camsta) =  UNICAM_STA_PI0_MASK |
                          UNICAM_STA_PI1_MASK |
                          UNICAM_STA_BUF1_RDY_MASK |
                          UNICAM_STA_BUF0_RDY_MASK
    dataStatus = UNICAM_DRV_REG(camdcs)
    UNICAM_DRV_REG(camdcs) = dataStatus
    if (0UL != (imageStatus & UNICAM_ISTA_FSI_MASK)) ||
        (0UL != (imageStatusSE & UNICAM_ISTA_SE_FSI_SE_MASK))
        event |= UNICAM_DrvProcessFSInterrupt(imageStatus, imageStatusSE)

    if (0UL != (imageStatus & UNICAM_ISTA_FEI_MASK)) ||
        (0UL != (imageStatusSE & UNICAM_ISTA_SE_FEI_SE_MASK))
        event |= UNICAM_DrvProcessFEInterrupt(imageStatus, imageStatusSE)

    if (0UL != (dataStatus & (UNICAM_DCS_DI_MASK|UNICAM_DCS_DI_END_MASK))) ||
       (0UL != (imageStatusSE & (UNICAM_ISTA_SE_DI_SE_MASK |
                                  UNICAM_ISTA_SE_DI_END_SE_MASK)))
        event |= UNICAM_DrvProcessDIInterrupt(dataStatus, imageStatusSE)

    Set_BitMask(&context.event, event)

    if 0UL != (event & UNICAM_INT_FS)
        mask |= UNICAM_EVENT_MASK_CTRL

    if 0UL != (event & UNICAM_INT_FE)
        mask |= UNICAM_EVENT_MASK_CTRL

    if 0UL != (event & UNICAM_INT_DI)
        mask |= UNICAM_EVENT_MASK_CTRL
        mask |= UNICAM_EVENT_MASK_DATA_SOF

    if 0UL != (event & UNICAM_INT_DI_END)
        mask |= UNICAM_EVENT_MASK_DATA_EOF

    return mask
    @endcode
*/
extern uint32_t UNICAM_DrvProcessInterrupt(void)
{
    uint32_t event = UNICAM_DrvContext.event;
    uint32_t mask = 0UL;
    uint32_t imageStatus = 0UL;
    uint32_t dataStatus = 0UL;
    uint32_t imageStatusSE = 0UL;

    /* Read Image status */
    imageStatus = UNICAM_DRV_REG(camista);

    /* Read Image status for Short */
    imageStatusSE = UNICAM_DRV_REG(camista_se);

    /* Clear status */
    UNICAM_DRV_REG(camista) = imageStatus;

    /* Clear SE status */
    UNICAM_DRV_REG(camista_se) = imageStatusSE;

    /* Clear status */
    UNICAM_DRV_REG(camsta) = (UNICAM_STA_PI0_MASK |
                          UNICAM_STA_PI1_MASK |
                          UNICAM_STA_BUF1_RDY_MASK |
                          UNICAM_STA_BUF0_RDY_MASK);

    /* Read Data Status for Long */
    dataStatus = UNICAM_DRV_REG(camdcs);

    /* Clear Data Status */
    UNICAM_DRV_REG(camdcs) = dataStatus;

    if ((0UL != (imageStatus & UNICAM_ISTA_FSI_MASK)) ||
        (0UL != (imageStatusSE & UNICAM_ISTA_SE_FSI_SE_MASK))) {
        event |= UNICAM_DrvProcessFSInterrupt(imageStatus, imageStatusSE);
    }

    if ((0UL != (imageStatus & UNICAM_ISTA_FEI_MASK)) ||
        (0UL != (imageStatusSE & UNICAM_ISTA_SE_FEI_SE_MASK))) {
        event |= UNICAM_DrvProcessFEInterrupt(imageStatus, imageStatusSE);
    }

    if ((0UL != (dataStatus & (UNICAM_DCS_DI_MASK|UNICAM_DCS_DI_END_MASK))) ||
        (0UL != (imageStatusSE & (UNICAM_ISTA_SE_DI_SE_MASK |
                                  UNICAM_ISTA_SE_DI_END_SE_MASK)))) {
        event |= UNICAM_DrvProcessDIInterrupt(dataStatus, imageStatusSE);
    }

    UNICAM_DrvContext.event = event;

    if (0UL != (event & UNICAM_INT_FS)) {
        mask |= UNICAM_EVENT_MASK_CTRL;
    }

    if (0UL != (event & UNICAM_INT_FE)) {
        mask |= UNICAM_EVENT_MASK_CTRL;
    }

    if (0UL != (event & UNICAM_INT_DI)) {
        mask |= UNICAM_EVENT_MASK_CTRL;
        mask |= UNICAM_EVENT_MASK_DATA_SOF;
    }

    if (0UL != (event & UNICAM_INT_DI_END)) {
        mask |= UNICAM_EVENT_MASK_DATA_EOF;
    }

    return mask;
}

/**
    @trace #BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @code{.c}
    event = context.event
    mask = 0UL;
    status = 0UL;
    dataLaneStatus[4] = {0UL}
    clockLaneStatus = 0UL
    unicamError = 0UL

    status = UNICAM_DRV_REG(camsta)
    UNICAM_DRV_REG(camsta) = UNICAM_DRV_INT_ERROR_MASK

    dataLaneStatus[0] = UNICAM_DRV_REG(camdat[0])
    UNICAM_DRV_REG(camdat[0]) = dataLaneStatus[0]
    if context.numDataLanes > 1U
        dataLaneStatus[1] = UNICAM_DRV_REG(camdat[1])
        UNICAM_DRV_REG(camdat[1]) = dataLaneStatus[1]
    if context.numDataLanes > 2U
        dataLaneStatus[2] = UNICAM_DRV_REG(camdat[2])
        UNICAM_DRV_REG(camdat[2]) = dataLaneStatus[2]
    if context.numDataLanes > 3U
        dataLaneStatus[3] = UNICAM_DRV_REG(camdat[3]
        UNICAM_DRV_REG(camdat[3]) = dataLaneStatus[3]

    clockLaneStatus = UNICAM_DRV_REG(camclk)

    if (0UL != (dataLaneStatus[0] & UNICAM_DRV_DATA_ERROR_MASK)) ||
            (0UL != (dataLaneStatus[1] & UNICAM_DRV_DATA_ERROR_MASK)) ||
            (0UL != (dataLaneStatus[2] & UNICAM_DRV_DATA_ERROR_MASK)) ||
            (0UL != (dataLaneStatus[3] & UNICAM_DRV_DATA_ERROR_MASK)) ||
            (0UL != (clockLaneStatus & UNICAM_DRV_CLK_ERROR_MASK))
        unicamError = 1UL
        context.status |= status
        context.dataLaneStatus[0] |= dataLaneStatus[0]
        context.dataLaneStatus[1] |= dataLaneStatus[1]
        context.dataLaneStatus[2] |= dataLaneStatus[2]
        context.dataLaneStatus[3] |= dataLaneStatus[3]
        context.clockLaneStatus |= clockLaneStatus
        UNICAM_DrvReset()

    if 0UL != (status & UNICAM_DRV_INT_ERROR_MASK)
        unicamError = 1UL
        UNICAM_DrvContext.numContErrors = UNICAM_DRV_ERROR_TOLERANCE
        Report_Error

    if 0UL != unicamError
        context.numContErrors++
        if UNICAM_DRV_ERROR_TOLERANCE < context.numContErrors)
            event |= UNICAM_INT_ERROR
            UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_ERROR)
    else
        context.numContErrors = 0UL

    SetBitMask(&context.event, event)

    if 0UL != (event & UNICAM_INT_ERROR)
        mask |= UNICAM_EVENT_MASK_CTRL

    return mask
    @endcode
*/
extern uint32_t UNICAM_DrvProcessErrorInterrupt(void)
{
    uint32_t event = UNICAM_DrvContext.event;
    uint32_t mask = 0UL;
    uint32_t status = 0UL;
    uint32_t dataLaneStatus[4] = {0UL};
    uint32_t clockLaneStatus = 0UL;
    uint32_t unicamError = 0UL;

    /* Read status */
    status = UNICAM_DRV_REG(camsta);

    /* Clear status */
    UNICAM_DRV_REG(camsta) = UNICAM_DRV_INT_ERROR_MASK;

    /* Read & clear data lanes error status */
    dataLaneStatus[0] = UNICAM_DRV_REG(camdat[0]);
    UNICAM_DRV_REG(camdat[0]) = dataLaneStatus[0];
    if (UNICAM_DrvContext.numDataLanes > 1U) {
        dataLaneStatus[1] = UNICAM_DRV_REG(camdat[1]);
        UNICAM_DRV_REG(camdat[1]) = dataLaneStatus[1];
    }
    if (UNICAM_DrvContext.numDataLanes > 2U) {
        dataLaneStatus[2] = UNICAM_DRV_REG(camdat[2]);
        UNICAM_DRV_REG(camdat[2]) = dataLaneStatus[2];
    }
    if (UNICAM_DrvContext.numDataLanes > 3U) {
        dataLaneStatus[3] = UNICAM_DRV_REG(camdat[3]);
        UNICAM_DRV_REG(camdat[3]) = dataLaneStatus[3];
    }

    /* Clock lane status */
    clockLaneStatus = UNICAM_DRV_REG(camclk);

    /* Clear status */
    if ((0UL != (dataLaneStatus[0] & UNICAM_DRV_DATA_ERROR_MASK)) ||
            (0UL != (dataLaneStatus[1] & UNICAM_DRV_DATA_ERROR_MASK)) ||
            (0UL != (dataLaneStatus[2] & UNICAM_DRV_DATA_ERROR_MASK)) ||
            (0UL != (dataLaneStatus[3] & UNICAM_DRV_DATA_ERROR_MASK)) ||
            (0UL != (clockLaneStatus & UNICAM_DRV_CLK_ERROR_MASK))) {

        unicamError = 1UL;

        /* Save status in case of error */
        UNICAM_DrvContext.status |= status;
        UNICAM_DrvContext.dataLaneStatus[0] |= dataLaneStatus[0];
        UNICAM_DrvContext.dataLaneStatus[1] |= dataLaneStatus[1];
        UNICAM_DrvContext.dataLaneStatus[2] |= dataLaneStatus[2];
        UNICAM_DrvContext.dataLaneStatus[3] |= dataLaneStatus[3];
        UNICAM_DrvContext.clockLaneStatus |= clockLaneStatus;

        /* Soft-Reset */
        UNICAM_DrvReset();
    }

    if (0UL != (status & UNICAM_DRV_INT_ERROR_MASK)) {
        unicamError = 1UL;
        UNICAM_DrvContext.numContErrors = UNICAM_DRV_ERROR_TOLERANCE;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_PROCESS_ERROR_INTERRUPT_PROC,
                BCM_ERR_UNKNOWN,
                (uint32_t) (__LINE__),
                status,
                (dataLaneStatus[0]|dataLaneStatus[1]|dataLaneStatus[2]|
                dataLaneStatus[3]),
                clockLaneStatus);
    }

    if (0UL != unicamError) {
        UNICAM_DrvContext.numContErrors++;
        if (UNICAM_DRV_ERROR_TOLERANCE < UNICAM_DrvContext.numContErrors) {
            event |= UNICAM_INT_ERROR;
            /* Set to Error state */
            UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_ERROR);
        }
    } else {
        UNICAM_DrvContext.numContErrors = 0UL;
    }
    UNICAM_DrvContext.event = event;

    if (0UL != (event & UNICAM_INT_ERROR)) {
        mask |= UNICAM_EVENT_MASK_CTRL;
    }

    return mask;
}

/** @brief Initialize UNICAM

    This API initializes UNICAM driver.

    @behavior Sync, Non-reentrant

    @pre This API shall only be called in the #UNICAM_DRV_STATE_POWEROFF state

    @param   void

    @retval  #BCM_ERR_OK             Initialization is successful
    @retval  #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state

    @post Once successfully initialized, the UNICAM driver shall be in
          #UNICAM_DRV_STATE_INITIALISED state

    @trace #BRCM_SWARCH_UNICAM_DRV_INIT_PROC
    @trace #BRCM_SWREQ_UNICAM_INIT

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK
    if UNICAM_DRV_STATE_POWEROFF != context.state
        ret = BCM_ERR_INVAL_STATE
        Report_Error
    else
        DrvInitState()
        context.interruptEnable = UNICAM_DRV_INT_MASK_ALL
        context.interruptEnableSE = 0UL
        context.interruptEnable |= UNICAM_ICTL_FSIE_MASK
        context.interruptEnableSE |= UNICAM_ICTL_SE_FSI_SE_EN_MASK
        UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_MEN_MASK)
        if prodesign
            UNICAM_DRV_REG(camana) |= (UNICAM_ANA_LDO_PU_MASK)
            UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_BPD_MASK)
            UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_APD_MASK)
            UNICAM_DRV_REG(camana) |= (UNICAM_ANA_AR_MASK)
            UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_AR_MASK)
        end prodesign
        UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_INITIALISED)
    return ret
    @endcode
*/
int32_t UNICAM_DrvInit(void)
{
    int32_t ret = BCM_ERR_OK;

    /* Check State */
    if (UNICAM_DRV_STATE_POWEROFF != UNICAM_DrvContext.state) {
        ret = BCM_ERR_INVAL_STATE;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_INIT_PROC,
                ret, UNICAM_DrvContext.state, 0UL, 0UL, 0UL);
    } else {
        /* Initialize internal params */
        UNICAM_DrvInitState();

        /* Apply default interrupt mask */
        UNICAM_DrvContext.interruptEnable = UNICAM_DRV_INT_MASK_ALL;

        /* Apply default interrupt mask(SE) */
        UNICAM_DrvContext.interruptEnableSE = 0UL;

        /* Enable FS for timestamp */
        UNICAM_DrvContext.interruptEnable |= UNICAM_ICTL_FSIE_MASK;

        /* Enable FS for timestamp(SE) */
        UNICAM_DrvContext.interruptEnableSE |= UNICAM_ICTL_SE_FSI_SE_EN_MASK;

        /* Enable MEM */
        UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_MEN_MASK);
        /* ANA LDO_PU Enable */
        UNICAM_DRV_REG(camana) |= (UNICAM_ANA_LDO_PU_MASK);
        /* ANA BPD */
        UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_BPD_MASK);
        /* ANA APD */
        UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_APD_MASK);
        /* Assert AR in ANA */
        UNICAM_DRV_REG(camana) |= (UNICAM_ANA_AR_MASK);
        /* De-Assert AR in ANA */
        UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_AR_MASK);
        UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_INITIALISED);
    }
    return ret;
}

/** @brief Enable UNICAM

    This enables UNICAM hardware block.

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_DRV_STATE_READY while calling this API

    @param   void

    @retval  #BCM_ERR_OK             Start is successful
    @retval  #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state

    @post Once successfully started, UNICAM shall be in #UNICAM_DRV_STATE_POWEROFF
          state

    @trace #BRCM_SWARCH_UNICAM_DRV_START_PROC
    @trace #BRCM_SWREQ_UNICAM

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK

    if UNICAM_DRV_STATE_READY != context.state
        ret = BCM_ERR_INVAL_STATE
        Report_Error
    else
        UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_DDL_MASK)
        if UNICAM_MODE_CSI == UNICAM_DrvMode
            UNICAM_DRV_REG(camdat[0]) &= ~(UNICAM_DAT0_DLPDN_MASK)
            if context.numDataLanes > 1U
                UNICAM_DRV_REG(camdat[1]) &= ~(UNICAM_DAT0_DLPDN_MASK)
            if context.numDataLanes > 2U
                UNICAM_DRV_REG(camdat[2]) &= ~(UNICAM_DAT0_DLPDN_MASK)
            if context.numDataLanes > 3U)
                UNICAM_DRV_REG(camdat[3]) &= ~(UNICAM_DAT0_DLPDN_MASK)
        if UNICAM_MODE_CSI == UNICAM_DrvMode
            UNICAM_DRV_REG(camclk) |= (UNICAM_CLK_CLLPE_MASK)
            UNICAM_DRV_REG(camdat[0]) |= (UNICAM_DAT0_DLLPEN_MASK)
            if context.numDataLanes > 1U
                UNICAM_DRV_REG(camdat[1]) |= (UNICAM_DAT0_DLLPEN_MASK)
            if context.numDataLanes > 2U
                UNICAM_DRV_REG(camdat[2]) |= (UNICAM_DAT0_DLLPEN_MASK)
            if context.numDataLanes > 3U
                UNICAM_DRV_REG(camdat[3]) |= (UNICAM_DAT0_DLLPEN_MASK)
        else
            UNICAM_DRV_REG(cam_vc5_tgctl) |= (UNICAM_VC5_TGCTL_TG_ENABLE_MASK)
        if UNICAM_MODE_CSI == UNICAM_DrvMode
            UNICAM_DRV_REG(camdat[0]) |= (UNICAM_DAT0_DLEN_MASK)
            if context.numDataLanes > 1U
                UNICAM_DRV_REG(camdat[1]) |= (UNICAM_DAT0_DLEN_MASK)
            if context.numDataLanes > 2U
                UNICAM_DRV_REG(camdat[2]) |= (UNICAM_DAT0_DLEN_MASK)
            if context.numDataLanes > 3U
                UNICAM_DRV_REG(camdat[3]) |= (UNICAM_DAT0_DLEN_MASK)
            UNICAM_DRV_REG(camclk) |= (UNICAM_CLK_CLE_MASK)
        UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_ENABLED)
        ictrl = UNICAM_DRV_REG(camictl)
        ictrl |= context.interruptEnable
        UNICAM_DRV_REG(camictl) = ictrl
        UNICAM_DRV_REG(camdcs) |= context.edInterruptMask
        UNICAM_DRV_REG(camdcs) |= UNICAM_DCS_DBOB_MASK

        UNICAM_DRV_REG(camictl_se) |= context.interruptEnableSE
        UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_SOE_MASK)
        UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPE_MASK)
        UNICAM_DRV_REG(camictl) |= (0x3UL << UNICAM_ICTL_LIP_SHIFT)
        UNICAM_DRV_REG(camdcs) |= (0x3UL << UNICAM_DCS_LDP_SHIFT)
        if UNICAM_MODE_TG == UNICAM_DrvMode
            UNICAM_DRV_REG(cam_vc5_tgctl) |= (UNICAM_VC5_TGCTL_START_MASK)
    return ret
    @endcode
*/
int32_t UNICAM_DrvStart(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t ictrl;

    if (UNICAM_DRV_STATE_READY != UNICAM_DrvContext.state) {
        ret = BCM_ERR_INVAL_STATE;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_START_PROC,
                ret, UNICAM_DrvContext.state, 0UL, 0UL, 0UL);
    } else {
        /* Enable Data Lane */
        UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_DDL_MASK);
        /* Data Lane Power On */
        if (UNICAM_MODE_CSI == UNICAM_DrvMode) {
            UNICAM_DRV_REG(camdat[0]) &= ~(UNICAM_DAT0_DLPDN_MASK);
            if (UNICAM_DrvContext.numDataLanes > 1U) {
                UNICAM_DRV_REG(camdat[1]) &= ~(UNICAM_DAT0_DLPDN_MASK);
            }
            if (UNICAM_DrvContext.numDataLanes > 2U) {
                UNICAM_DRV_REG(camdat[2]) &= ~(UNICAM_DAT0_DLPDN_MASK);
            }
            if (UNICAM_DrvContext.numDataLanes > 3U) {
                UNICAM_DRV_REG(camdat[3]) &= ~(UNICAM_DAT0_DLPDN_MASK);
            }
        }
        /* Check Mode */
        if (UNICAM_MODE_CSI == UNICAM_DrvMode) {
            /* Enable Clock lane */
            UNICAM_DRV_REG(camclk) |= (UNICAM_CLK_CLLPE_MASK);
            /* Enable Data Lanes */
            UNICAM_DRV_REG(camdat[0]) |= (UNICAM_DAT0_DLLPEN_MASK);
            if (UNICAM_DrvContext.numDataLanes > 1U) {
                UNICAM_DRV_REG(camdat[1]) |= (UNICAM_DAT0_DLLPEN_MASK);
            }
            if (UNICAM_DrvContext.numDataLanes > 2U) {
                UNICAM_DRV_REG(camdat[2]) |= (UNICAM_DAT0_DLLPEN_MASK);
            }
            if (UNICAM_DrvContext.numDataLanes > 3U) {
                UNICAM_DRV_REG(camdat[3]) |= (UNICAM_DAT0_DLLPEN_MASK);
            }
        } else {
            /* Enable TG */
            UNICAM_DRV_REG(cam_vc5_tgctl) |= (UNICAM_VC5_TGCTL_TG_ENABLE_MASK);
        }
        if (UNICAM_MODE_CSI == UNICAM_DrvMode) {
            /* Enable Data Lanes */
            UNICAM_DRV_REG(camdat[0]) |= (UNICAM_DAT0_DLEN_MASK);
            if (UNICAM_DrvContext.numDataLanes > 1U) {
                UNICAM_DRV_REG(camdat[1]) |= (UNICAM_DAT0_DLEN_MASK);
            }
            if (UNICAM_DrvContext.numDataLanes > 2U) {
                UNICAM_DRV_REG(camdat[2]) |= (UNICAM_DAT0_DLEN_MASK);
            }
            if (UNICAM_DrvContext.numDataLanes > 3U) {
                UNICAM_DRV_REG(camdat[3]) |= (UNICAM_DAT0_DLEN_MASK);
            }
            /* Now enable only clock lane */
            UNICAM_DRV_REG(camclk) |= (UNICAM_CLK_CLE_MASK);
        }
        UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_ENABLED);
        /* Enable interruptEnable */
        ictrl = UNICAM_DRV_REG(camictl);
        ictrl |= UNICAM_DrvContext.interruptEnable;
        UNICAM_DRV_REG(camictl) = ictrl;
        UNICAM_DRV_REG(camdcs) |= UNICAM_DrvContext.edInterruptMask;
        /* Temporary work around suggested for CRINGIP-645 */
        UNICAM_DRV_REG(camdcs) |= UNICAM_DCS_DBOB_MASK;
        UNICAM_DRV_REG(camictl_se) |= UNICAM_DrvContext.interruptEnableSE;
        /* Enable OutPut Engine */
        UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_SOE_MASK);
        /* Camera Peripheral Enable */
        UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPE_MASK);
        /* Load Image Buffer Address */
        UNICAM_DRV_REG(camictl) |= (0x3UL << UNICAM_ICTL_LIP_SHIFT);
        /* Load Data Buffer Address */
        UNICAM_DRV_REG(camdcs) |= (0x3UL << UNICAM_DCS_LDP_SHIFT);
        /* Start streaming from TG */
        if (UNICAM_MODE_TG == UNICAM_DrvMode) {
            UNICAM_DRV_REG(cam_vc5_tgctl) |= (UNICAM_VC5_TGCTL_START_MASK);
        }
    }

    return ret;
}

/**
    @brief Disable data lane

    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWREQ_UNICAM
*/
static void UNICAM_DrvDisableDataLane(void)
{
    if (UNICAM_DrvContext.numDataLanes > 1U) {
        UNICAM_DRV_REG(camdat[1]) &= ~(UNICAM_DAT0_DLEN_MASK);
    }
    if (UNICAM_DrvContext.numDataLanes > 2U) {
        UNICAM_DRV_REG(camdat[2]) &= ~(UNICAM_DAT0_DLEN_MASK);
    }
    if (UNICAM_DrvContext.numDataLanes > 3U) {
        UNICAM_DRV_REG(camdat[3]) &= ~(UNICAM_DAT0_DLEN_MASK);
    }
}

/** @brief Disable UNICAM

    This API disables UNICAM hardware block.

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_DRV_STATE_STREAMING state while calling this API

    @param   void

    @retval  #BCM_ERR_OK             Stop is successful
    @retval  #BCM_ERR_TIME_OUT       UNICAM stop failed
    @retval  #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state

    @post Once successfully disabled, UNICAM shall be in #UNICAM_DRV_STATE_READY
         state

    @trace #BRCM_SWARCH_UNICAM_DRV_STOP_PROC
    @trace #BRCM_SWREQ_UNICAM

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK
    status = 0
    loopCount = 0xffff

    if UNICAM_DRV_STATE_ENABLED != UNICAM_DrvContext.state &&
       UNICAM_DRV_STATE_ERROR != UNICAM_DrvContext.state
        Report_Error
    else
        if UNICAM_MODE_TG == UNICAM_DrvMode
            UNICAM_DRV_REG(cam_vc5_tgctl) &= ~(UNICAM_VC5_TGCTL_START_MASK)
            UNICAM_DRV_REG(cam_vc5_tgctl) &= ~(UNICAM_VC5_TGCTL_TG_ENABLE_MASK)
        UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_SOE_MASK)
        do
            status = UNICAM_DRV_REG(camsta)
            if 0UL == (loopCount--)
                ret = BCM_ERR_TIME_OUT
                Report_Error
                break
        while 0UL == (status & UNICAM_STA_OES_MASK)
        if BCM_ERR_OK == ret
            ictrl = UNICAM_DRV_REG(camictl)
            dcs = UNICAM_DRV_REG(camdcs)
            ictlSE = UNICAM_DRV_REG(camictl_se)

            UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPR_MASK)
            UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPR_DPHY_MASK)
            UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPE_MASK)
            UNICAM_DRV_REG(camana) |= (UNICAM_ANA_DDL_MASK)

            UNICAM_DRV_REG(camictl) &= ~(context.interruptEnable)
            UNICAM_DRV_REG(camdcs) &= ~(context.edInterruptMask)
            UNICAM_DRV_REG(camictl_se) &= ~(context.interruptEnableSE)

            if UNICAM_MODE_CSI == UNICAM_DrvMode
                UNICAM_DRV_REG(camdat[0]) &= ~(UNICAM_DAT0_DLEN_MASK)
                if (UNICAM_DrvContext.numDataLanes > 1U)
                    UNICAM_DRV_REG(camdat[1]) &= ~(UNICAM_DAT0_DLEN_MASK)
                if (UNICAM_DrvContext.numDataLanes > 2U)
                    UNICAM_DRV_REG(camdat[2]) &= ~(UNICAM_DAT0_DLEN_MASK)
                if (UNICAM_DrvContext.numDataLanes > 3U)
                    UNICAM_DRV_REG(camdat[3]) &= ~(UNICAM_DAT0_DLEN_MASK)

                UNICAM_DRV_REG(camdat[0]) &= ~(UNICAM_DAT0_DLLPEN_MASK)
                if (UNICAM_DrvContext.numDataLanes > 1U)
                    UNICAM_DRV_REG(camdat[1]) &= ~(UNICAM_DAT0_DLLPEN_MASK);
                if (UNICAM_DrvContext.numDataLanes > 2U)
                    UNICAM_DRV_REG(camdat[2]) &= ~(UNICAM_DAT0_DLLPEN_MASK);
                if (UNICAM_DrvContext.numDataLanes > 3U)
                    UNICAM_DRV_REG(camdat[3]) &= ~(UNICAM_DAT0_DLLPEN_MASK);

                UNICAM_DRV_REG(camdat[0]) |= (UNICAM_DAT0_DLPDN_MASK);
                if (UNICAM_DrvContext.numDataLanes > 1U)
                    UNICAM_DRV_REG(camdat[1]) |= (UNICAM_DAT0_DLPDN_MASK);
                if (UNICAM_DrvContext.numDataLanes > 2U)
                    UNICAM_DRV_REG(camdat[2]) |= (UNICAM_DAT0_DLPDN_MASK);
                if (UNICAM_DrvContext.numDataLanes > 3U)
                    UNICAM_DRV_REG(camdat[3]) |= (UNICAM_DAT0_DLPDN_MASK);
                UNICAM_DRV_REG(camclk) &= ~(UNICAM_CLK_CLE_MASK);
                UNICAM_DRV_REG(camclk) &= ~(UNICAM_CLK_CLLPE_MASK);
            UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPR_DPHY_MASK)
            UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPR_MASK)

            ictrl &= ~(UNICAM_DrvContext.interruptEnable)
            dcs &= ~(UNICAM_DrvContext.edInterruptMask)
            ictlSE &= ~(UNICAM_DrvContext.interruptEnableSE)
            UNICAM_DRV_REG(camictl) = ictrl
            UNICAM_DRV_REG(camdcs) = dcs
            UNICAM_DRV_REG(camictl_se) = ictlSE
            UNICAM_DrvConfigurePri()
            if UNICAM_DRV_STATE_ERROR != UNICAM_DrvContext.state
                UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_READY)
            else
                UNICAM_DrvSetState(1UL, UNICAM_DRV_STATE_ERROR)
    return ret
    @endcode
*/
int32_t UNICAM_DrvStop(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t status = 0UL;
    uint32_t loopCount = 0xffffUL;

    if ((UNICAM_DRV_STATE_ENABLED != UNICAM_DrvContext.state) &&
        (UNICAM_DRV_STATE_ERROR != UNICAM_DrvContext.state)) {
        ret = BCM_ERR_INVAL_STATE;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_STOP_PROC,
                ret, UNICAM_DrvContext.state, 0UL, 0UL, 0UL);
    } else {
        /* Disable TG */
        if (UNICAM_MODE_TG == UNICAM_DrvMode) {
            /* Stop Streaming */
            UNICAM_DRV_REG(cam_vc5_tgctl) &= ~(UNICAM_VC5_TGCTL_START_MASK);
            /* Disable Tg */
            UNICAM_DRV_REG(cam_vc5_tgctl) &= ~(UNICAM_VC5_TGCTL_TG_ENABLE_MASK);
        }
        /* Shutdown Output Engine */
        UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_SOE_MASK);
        /* Check for Shutdown Output Engine Status */
        do {
            status = UNICAM_DRV_REG(camsta);
            if (0UL == (loopCount--)) {
                ret = BCM_ERR_TIME_OUT;
                UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_STOP_PROC,
                        ret, 0UL, 0UL, 0UL, 0UL);
                break;
            }
        } while(0UL == (status & UNICAM_STA_OES_MASK));
        if (BCM_ERR_OK == ret) {
            /* Read values before reset for restore after reset */
            uint32_t ictrl = UNICAM_DRV_REG(camictl);
            uint32_t dcs = UNICAM_DRV_REG(camdcs);
            uint32_t ictlSE = UNICAM_DRV_REG(camictl_se);

            /* Assert Soft Reset */
            UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPR_MASK);
            UNICAM_DRV_REG(camctl) |= (UNICAM_CTL_CPR_DPHY_MASK);
            /* Disable peripheral */
            UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPE_MASK);

            /* Disable Data Lane */
            UNICAM_DRV_REG(camana) |= (UNICAM_ANA_DDL_MASK);

            /* Disable interrupts */
            UNICAM_DRV_REG(camictl) &= ~(UNICAM_DrvContext.interruptEnable);
            UNICAM_DRV_REG(camdcs) &= ~(UNICAM_DrvContext.edInterruptMask);
            UNICAM_DRV_REG(camictl_se) &= ~(UNICAM_DrvContext.interruptEnableSE);

            /* Check Mode */
            if (UNICAM_MODE_CSI == UNICAM_DrvMode) {
                /* Disable Data Lanes */
                UNICAM_DRV_REG(camdat[0]) &= ~(UNICAM_DAT0_DLEN_MASK);
                UNICAM_DrvDisableDataLane();
                
                /* Disable Data Lane Low Power Enable */
                UNICAM_DRV_REG(camdat[0]) &= ~(UNICAM_DAT0_DLLPEN_MASK);
                if (UNICAM_DrvContext.numDataLanes > 1U) {
                    UNICAM_DRV_REG(camdat[1]) &= ~(UNICAM_DAT0_DLLPEN_MASK);
                }
                if (UNICAM_DrvContext.numDataLanes > 2U) {
                    UNICAM_DRV_REG(camdat[2]) &= ~(UNICAM_DAT0_DLLPEN_MASK);
                }
                if (UNICAM_DrvContext.numDataLanes > 3U) {
                    UNICAM_DRV_REG(camdat[3]) &= ~(UNICAM_DAT0_DLLPEN_MASK);
                }
                /* Data Lane Power Down */
                UNICAM_DRV_REG(camdat[0]) |= (UNICAM_DAT0_DLPDN_MASK);
                if (UNICAM_DrvContext.numDataLanes > 1U) {
                    UNICAM_DRV_REG(camdat[1]) |= (UNICAM_DAT0_DLPDN_MASK);
                }
                if (UNICAM_DrvContext.numDataLanes > 2U) {
                    UNICAM_DRV_REG(camdat[2]) |= (UNICAM_DAT0_DLPDN_MASK);
                }
                if (UNICAM_DrvContext.numDataLanes > 3U) {
                    UNICAM_DRV_REG(camdat[3]) |= (UNICAM_DAT0_DLPDN_MASK);
                }
                /* Disable Clock lane */
                UNICAM_DRV_REG(camclk) &= ~(UNICAM_CLK_CLE_MASK);
                UNICAM_DRV_REG(camclk) &= ~(UNICAM_CLK_CLLPE_MASK);
            }
            /* De-assert Soft Reset */
            UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPR_DPHY_MASK);
            UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_CPR_MASK);

            /* Restore lost bits */
            /* Disable Interrupts in the read values */
            ictrl &= ~(UNICAM_DrvContext.interruptEnable);
            dcs &= ~(UNICAM_DrvContext.edInterruptMask);
            ictlSE &= ~(UNICAM_DrvContext.interruptEnableSE);
            /* Restore values to registers */
            UNICAM_DRV_REG(camictl) = ictrl;
            UNICAM_DRV_REG(camdcs) = dcs;
            UNICAM_DRV_REG(camictl_se) = ictlSE;

            /* Configure Priority Control (Lost config due to reset) */
            UNICAM_DrvConfigurePri();

            if (UNICAM_DRV_STATE_ERROR != UNICAM_DrvContext.state) {
                /* Change State to Enabled */
                UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_READY);
            } else {
                UNICAM_DrvSetState(1UL, UNICAM_DRV_STATE_ERROR);
            }
        }
    }

    return ret;
}

/** @brief De-initialize UNICAM

    This API de-initializes UNICAM driver.

    @behavior Sync, Non-reentrant

    @pre None

    @param   void

    @retval  #BCM_ERR_OK             De-initialization is successful
    @retval  #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong  state

    @post Once successfully de-initialized, the UNICAM driver shall be in
          #UNICAM_DRV_STATE_POWEROFF state

    @trace #BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_UNICAM

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK

    if UNICAM_DRV_STATE_POWEROFF == context.state
        ret = BCM_ERR_INVAL_STATE
        Report_Error
    else
        if prodesign
            UNICAM_DRV_REG(camana) |= (UNICAM_ANA_AR_MASK)
            UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_AR_MASK)
            UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_LDO_PU_MASK)
            UNICAM_DRV_REG(camana) |= (UNICAM_ANA_APD_MASK)
            UNICAM_DRV_REG(camana) |= (UNICAM_ANA_BPD_MASK)
        end prodesign
        UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_MEN_MASK)
        DrvInitState()
    return ret
    @endcode
*/
int32_t UNICAM_DrvDeinit(void)
{
    int32_t ret = BCM_ERR_OK;

    if (UNICAM_DRV_STATE_POWEROFF == UNICAM_DrvContext.state) {
        ret = BCM_ERR_INVAL_STATE;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_DEINIT_PROC,
                ret, 0UL, 0UL, 0UL, 0UL);
    } else {
        if ((UNICAM_DRV_STATE_ENABLED == UNICAM_DrvContext.state) ||
            ((UNICAM_DRV_STATE_ERROR == UNICAM_DrvContext.state) &&
             (UNICAM_DRV_STATE_ENABLED == UNICAM_DrvContext.prevState))) {
           ret = UNICAM_DrvStop();
        }
        /* Assert AR in ANA */
        UNICAM_DRV_REG(camana) |= (UNICAM_ANA_AR_MASK);
        /* De-Assert AR in ANA */
        UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_AR_MASK);
        /* Disable LDO_PU */
        UNICAM_DRV_REG(camana) &= ~(UNICAM_ANA_LDO_PU_MASK);
        /* ANA APD */
        UNICAM_DRV_REG(camana) |= (UNICAM_ANA_APD_MASK);
        /* ANA BPD */
        UNICAM_DRV_REG(camana) |= (UNICAM_ANA_BPD_MASK);

        /* Disable MEM */
        UNICAM_DRV_REG(camctl) &= ~(UNICAM_CTL_MEN_MASK);

        /* Initialize internal params */
        UNICAM_DrvInitState();
    }
    return ret;
}

/** @brief Retrive format informations

    Retrive BPP and config ID from format.

    @behavior Sync, Reentrant

    @pre None

    @param[in]   aFormat  Format type as in #UNICAM_FormatType
    @param[out]  aBPP     Address of BPP to be retrieved
    @param[out]  aID      Address of config ID to be retrieved

    @retval     #BCM_ERR_OK           Format information retrieval is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid format supplied

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK
    bpp = 0U
    id = 0U

    switch (aFormat)
        case UNICAM_FORMAT_RAW6
            bpp = 6U
            id = 0x28U
            break
        case UNICAM_FORMAT_RAW7
            bpp = 7U
            id = 0x29U
            break
        case UNICAM_FORMAT_RAW8
            bpp = 8U
            id = 0x2aU
            break
        case UNICAM_FORMAT_RAW10
            bpp = 10U
            id = 0x2bU
            break
        case UNICAM_FORMAT_RAW12
            bpp = 12U
            id = 0x2cU
            break
        case UNICAM_FORMAT_RAW14
            bpp = 14U
            id = 0x2dU
            break
        case UNICAM_FORMAT_RAW16
            bpp = 16U
            id = 0x2eU
            break
        default
            ret = BCM_ERR_INVAL_PARAMS
            break

    if BCM_ERR_OK == ret
        if NULL != aBPP
            *aBPP = bpp
        if NULL != aID
            *aID = id

    return ret
    @endcode
*/
static int32_t UNICAM_DrvGetFormatInfo(UNICAM_FormatType aFormat,
                                       uint8_t *aBPP,
                                       uint8_t *aID)
{
    int32_t ret = BCM_ERR_OK;
    uint8_t bpp = 0U;
    uint8_t id = 0U;

    /* To get image id's and bpp */
    switch (aFormat) {
        case UNICAM_FORMAT_RAW6:
            bpp = 6U;
            id = 0x28U;
            break;
        case UNICAM_FORMAT_RAW7:
            bpp = 7U;
            id = 0x29U;
            break;
        case UNICAM_FORMAT_RAW8:
            bpp = 8U;
            id = 0x2aU;
            break;
        case UNICAM_FORMAT_RAW10:
            bpp = 10U;
            id = 0x2bU;
            break;
        case UNICAM_FORMAT_RAW12:
            bpp = 12U;
            id = 0x2cU;
            break;
        case UNICAM_FORMAT_RAW14:
            bpp = 14U;
            id = 0x2dU;
            break;
        case UNICAM_FORMAT_RAW16:
            bpp = 16U;
            id = 0x2eU;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }

    if (BCM_ERR_OK == ret) {
        if (NULL != aBPP) {
            *aBPP = bpp;
        }

        if (NULL != aID) {
            *aID = id;
        }
    }

    return ret;
}

/** @brief Program CSI Timing

    Program clock and data lane timing for CSI

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aCSICfg  CSI interface configuration

    @retval     #BCM_ERR_OK           CSI Lane timing configuration is successful
    @retval     #BCM_ERR_INVAL_PARAMS hsByteClkTime is zero

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    lpClkTime = BCM_NS_PER_SEC / UNICAM_LP_CLK_FREQ
    hsByteClkTime = (BCM_NS_PER_SEC / aCSICfg->hsClkFrequency) * 8UL
    ret = BCM_ERR_OK

    if 0UL == hsByteClkTime
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else
        cycleTime = aCSICfg->tclkTermEnable / lpClkTime
        cycleTime = (0U < cycleTime) ? (cycleTime - 1U) : 0U
        UNICAM_DRV_FIELD_WRITE(camclt,
                           UNICAM_CLT_1_SHIFT,
                           UNICAM_CLT_1_MASK,
                           (uint32_t) cycleTime)
        cycleTime = (uint8_t)  (aCSICfg->tclkSettle / lpClkTime)
        UNICAM_DRV_FIELD_WRITE(camclt,
                           UNICAM_CLT_2_SHIFT,
                           UNICAM_CLT_2_MASK,
                           (uint32_t) cycleTime)
        UNICAM_DRV_REG(camclk) = 0UL
        cycleTime = (uint8_t) (aCSICfg->tdTermEnable / lpClkTime)
        cycleTime = (1U <= cycleTime) ? (cycleTime - 1U) : 0U
        UNICAM_DRV_FIELD_WRITE(camdlt,
                           UNICAM_DLT_1_SHIFT,
                           UNICAM_DLT_1_MASK,
                           (uint32_t) cycleTime)
        cycleTime = (uint8_t)  (aCSICfg->thsSettle / lpClkTime)
        UNICAM_DRV_FIELD_WRITE(camdlt,
                           UNICAM_DLT_2_SHIFT,
                           UNICAM_DLT_2_MASK,
                           (uint32_t) cycleTime)
        cycleTime = (uint8_t) (aCSICfg->trxEnable / hsByteClkTime)
        cycleTime = (2U <= cycleTime) ? (cycleTime - 2U) : 0U
        UNICAM_DRV_FIELD_WRITE(camdlt,
                           UNICAM_DLT_3_SHIFT,
                           UNICAM_DLT_3_MASK,
                           (uint32_t) cycleTime)
        laneTerm = ((uint32_t) aCSICfg->dataLaneCtl &
                     UNICAM_CSICFG_DATALANE_DLTREN_MASK) >>
                     UNICAM_CSICFG_DATALANE_DLTREN_SHIFT

        laneSync = ((uint32_t) aCSICfg->dataLaneCtl &
                     UNICAM_CSICFG_DATALANE_DLSMN_MASK) >>
                     UNICAM_CSICFG_DATALANE_DLSMN_SHIFT

        UNICAM_DRV_FIELD_WRITE(camdat[0], UNICAM_DAT0_DLTREN_SHIFT,
                           UNICAM_DAT0_DLTREN_MASK, laneTerm)
        UNICAM_DRV_FIELD_WRITE(camdat[0], UNICAM_DAT0_DLSMN_SHIFT,
                           UNICAM_DAT0_DLSMN_MASK, laneSync)
        if UNICAM_DrvContext.numDataLanes > 1U
            UNICAM_DRV_FIELD_WRITE(camdat[1], UNICAM_DAT0_DLTREN_SHIFT,
                               UNICAM_DAT0_DLTREN_MASK, laneTerm)
            UNICAM_DRV_FIELD_WRITE(camdat[1], UNICAM_DAT0_DLSMN_SHIFT,
                               UNICAM_DAT0_DLSMN_MASK, laneSync)
        if UNICAM_DrvContext.numDataLanes > 2U
            UNICAM_DRV_FIELD_WRITE(camdat[2], UNICAM_DAT0_DLTREN_SHIFT,
                               UNICAM_DAT0_DLTREN_MASK, laneTerm)
            UNICAM_DRV_FIELD_WRITE(camdat[2], UNICAM_DAT0_DLSMN_SHIFT,
                               UNICAM_DAT0_DLSMN_MASK, laneSync)
        if UNICAM_DrvContext.numDataLanes > 3U
            UNICAM_DRV_FIELD_WRITE(camdat[3], UNICAM_DAT0_DLTREN_SHIFT,
                               UNICAM_DAT0_DLTREN_MASK, laneTerm)
            UNICAM_DRV_FIELD_WRITE(camdat[3], UNICAM_DAT0_DLSMN_SHIFT,
                               UNICAM_DAT0_DLSMN_MASK, laneSync)
        UNICAM_DRV_FIELD_WRITE(camctl, UNICAM_CTL_PFT_SHIFT, UNICAM_CTL_PFT_MASK,
                           (uint32_t) aCSICfg->packetFrameTimeout)

    return ret
    @endcode
*/
static int32_t UNICAM_DrvCSI_ConfigureTiming(const UNICAM_CSIConfigType *aCSICfg)
{
    uint32_t lpClkTime = (uint32_t) (BCM_NS_PER_SEC / UNICAM_LP_CLK_FREQ);
    uint32_t hsByteClkTime = (uint32_t) ((BCM_NS_PER_SEC /
                                          aCSICfg->hsClkFrequency) * 8UL);
    uint8_t cycleTime;
    uint32_t laneTerm, laneSync;
    int32_t ret = BCM_ERR_OK;

    if (0UL == hsByteClkTime) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_CSI_CONFIGURE_TIMING_PROC,
                ret, (uint32_t) __LINE__, hsByteClkTime, 0UL, 0UL);
    } else {

        /* CAMCLT */
        cycleTime = (uint8_t) (aCSICfg->tclkTermEnable / lpClkTime);
        cycleTime = (0U < cycleTime) ? (cycleTime - 1U) : 0U;
        UNICAM_DRV_FIELD_WRITE(camclt,
                           UNICAM_CLT_1_SHIFT,
                           UNICAM_CLT_1_MASK,
                           (uint32_t) cycleTime);

        cycleTime = (uint8_t)  (aCSICfg->tclkSettle / lpClkTime);
        UNICAM_DRV_FIELD_WRITE(camclt,
                           UNICAM_CLT_2_SHIFT,
                           UNICAM_CLT_2_MASK,
                           (uint32_t) cycleTime);
        /* CAMCLK */
        UNICAM_DRV_REG(camclk) = 0UL;

        /* CAMDLT */
        cycleTime = (uint8_t) (aCSICfg->tdTermEnable / lpClkTime);
        cycleTime = (1U <= cycleTime) ? (cycleTime - 1U) : 0U;
        UNICAM_DRV_FIELD_WRITE(camdlt,
                           UNICAM_DLT_1_SHIFT,
                           UNICAM_DLT_1_MASK,
                           (uint32_t) cycleTime);
        cycleTime = (uint8_t)  (aCSICfg->thsSettle / lpClkTime);
        UNICAM_DRV_FIELD_WRITE(camdlt,
                           UNICAM_DLT_2_SHIFT,
                           UNICAM_DLT_2_MASK,
                           (uint32_t) cycleTime);
        cycleTime = (uint8_t) (aCSICfg->trxEnable / hsByteClkTime);
        cycleTime = (2U <= cycleTime) ? (cycleTime - 2U) : 0U;
        UNICAM_DRV_FIELD_WRITE(camdlt,
                           UNICAM_DLT_3_SHIFT,
                           UNICAM_DLT_3_MASK,
                           (uint32_t) cycleTime);
        /* CAMDAT */
        laneTerm = ((uint32_t) aCSICfg->dataLaneCtl &
                     UNICAM_CSICFG_DATALANE_DLTREN_MASK) >>
                     UNICAM_CSICFG_DATALANE_DLTREN_SHIFT;

        laneSync = ((uint32_t) aCSICfg->dataLaneCtl &
                     UNICAM_CSICFG_DATALANE_DLSMN_MASK) >>
                     UNICAM_CSICFG_DATALANE_DLSMN_SHIFT;

        UNICAM_DRV_FIELD_WRITE(camdat[0], UNICAM_DAT0_DLTREN_SHIFT,
                           UNICAM_DAT0_DLTREN_MASK, laneTerm);
        UNICAM_DRV_FIELD_WRITE(camdat[0], UNICAM_DAT0_DLSMN_SHIFT,
                           UNICAM_DAT0_DLSMN_MASK, laneSync);

        if (UNICAM_DrvContext.numDataLanes > 1U) {
            UNICAM_DRV_FIELD_WRITE(camdat[1], UNICAM_DAT0_DLTREN_SHIFT,
                               UNICAM_DAT0_DLTREN_MASK, laneTerm);
            UNICAM_DRV_FIELD_WRITE(camdat[1], UNICAM_DAT0_DLSMN_SHIFT,
                               UNICAM_DAT0_DLSMN_MASK, laneSync);
        }
        if (UNICAM_DrvContext.numDataLanes > 2U) {
            UNICAM_DRV_FIELD_WRITE(camdat[2], UNICAM_DAT0_DLTREN_SHIFT,
                               UNICAM_DAT0_DLTREN_MASK, laneTerm);
            UNICAM_DRV_FIELD_WRITE(camdat[2], UNICAM_DAT0_DLSMN_SHIFT,
                               UNICAM_DAT0_DLSMN_MASK, laneSync);
        }
        if (UNICAM_DrvContext.numDataLanes > 3U) {
            UNICAM_DRV_FIELD_WRITE(camdat[3], UNICAM_DAT0_DLTREN_SHIFT,
                               UNICAM_DAT0_DLTREN_MASK, laneTerm);
            UNICAM_DRV_FIELD_WRITE(camdat[3], UNICAM_DAT0_DLSMN_SHIFT,
                               UNICAM_DAT0_DLSMN_MASK, laneSync);
        }
        /* CAMCTL */
        UNICAM_DRV_FIELD_WRITE(camctl, UNICAM_CTL_PFT_SHIFT, UNICAM_CTL_PFT_MASK,
                           (uint32_t) aCSICfg->packetFrameTimeout);
    }

    return ret;
}

/** @brief Program CSI image ID

    Program image IDs for CSI

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aCSICfg       CSI interface configuration
    @param[in]  aIsHDR        Is HDR enabled
    @param[in]  aLongImageID  Image ID for long exposure
    @param[in]  aShortImageID Image ID for short exposure

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    idi0Val = 0UL
    idi1Val = 0UL
    vcNum = 0U

    if 0U != aIsHDR
        idi0Val = ((((uint32_t)aCSICfg->vcNum[UNICAM_OUTPUT_IDX_LONG]) << 6UL) |
                  (uint32_t) aLongImageID)
        idi1Val = ((((uint32_t) aCSICfg->vcNum[UNICAM_OUTPUT_IDX_SHORT]) << 6UL) |
                  (uint32_t) aShortImageID)
        UNICAM_DRV_FIELD_WRITE(camidi0,
               UNICAM_IDI0_IDI0_SHIFT,
               UNICAM_IDI0_IDI0_MASK,
               idi0Val)
        UNICAM_DRV_FIELD_WRITE(camidi0,
               UNICAM_IDI0_IDI1_SHIFT,
               UNICAM_IDI0_IDI1_MASK,
               idi1Val)
    else
        UNICAM_DRV_REG(camidi0) = 0
        UNICAM_DRV_FIELD_WRITE(camidi0,
               UNICAM_IDI0_IDI0_SHIFT,
               UNICAM_IDI0_IDI0_MASK,
               (((uint32_t) vcNum << 6UL) | (uint32_t) aLongImageID))
    UNICAM_DRV_REG(camidi1) = 0x03020100UL
    UNICAM_DRV_REG(camidi2) = 0x43424140UL

    UNICAM_DRV_FIELD_WRITE(camidi3,
           UNICAM_IDI3_IDI12_SHIFT,
           UNICAM_IDI3_IDI12_MASK,
           (uint32_t) aCSICfg->custImageIds[0])
    UNICAM_DRV_FIELD_WRITE(camidi3,
           UNICAM_IDI3_IDI13_SHIFT,
           UNICAM_IDI3_IDI13_MASK,
           (uint32_t) aCSICfg->custImageIds[1])
    UNICAM_DRV_FIELD_WRITE(camidi3,
           UNICAM_IDI3_IDI14_SHIFT,
           UNICAM_IDI3_IDI14_MASK,
           (uint32_t) aCSICfg->custImageIds[2])
    UNICAM_DRV_FIELD_WRITE(camidi3,
           UNICAM_IDI3_IDI15_SHIFT,
           UNICAM_IDI3_IDI15_MASK,
           (uint32_t) aCSICfg->custImageIds[3])
    @endcode
*/
static void UNICAM_DrvCSI_ConfigureImageID(const UNICAM_CSIConfigType *aCSICfg,
                                              uint8_t aIsHDR,
                                              uint8_t aLongImageID,
                                              uint8_t aShortImageID)
{
    uint32_t idi0Val = 0UL;
    uint32_t idi1Val = 0UL;
    uint8_t  vcNum = 0U;

    if (0U != aIsHDR) {
        idi0Val = ((((uint32_t)aCSICfg->vcNum[UNICAM_OUTPUT_IDX_LONG]) << 6UL) |
                  (uint32_t) aLongImageID);
        idi1Val = ((((uint32_t) aCSICfg->vcNum[UNICAM_OUTPUT_IDX_SHORT]) << 6UL) |
                  (uint32_t) aShortImageID);
        UNICAM_DRV_FIELD_WRITE(camidi0,
               UNICAM_IDI0_IDI0_SHIFT,
               UNICAM_IDI0_IDI0_MASK,
               idi0Val);
        UNICAM_DRV_FIELD_WRITE(camidi0,
               UNICAM_IDI0_IDI1_SHIFT,
               UNICAM_IDI0_IDI1_MASK,
               idi1Val);
    } else {
        UNICAM_DRV_REG(camidi0) = 0x0UL;
        UNICAM_DRV_FIELD_WRITE(camidi0,
               UNICAM_IDI0_IDI0_SHIFT,
               UNICAM_IDI0_IDI0_MASK,
               (((uint32_t) vcNum << 6UL) | (uint32_t) aLongImageID));
    }

    /* FS/FE/LS/LE for HDR & non_HDR mode */
    /* As per the CSI2 std : 0x00->FS, 0x01->FE, 0x02->LS & 0x03->LE */
    UNICAM_DRV_REG(camidi1) = 0x03020100UL;
    UNICAM_DRV_REG(camidi2) = 0x43424140UL;

    /* Program Custom Image IDs */
    UNICAM_DRV_FIELD_WRITE(camidi3,
           UNICAM_IDI3_IDI12_SHIFT,
           UNICAM_IDI3_IDI12_MASK,
           (uint32_t) aCSICfg->custImageIds[0]);
    UNICAM_DRV_FIELD_WRITE(camidi3,
           UNICAM_IDI3_IDI13_SHIFT,
           UNICAM_IDI3_IDI13_MASK,
           (uint32_t) aCSICfg->custImageIds[1]);
    UNICAM_DRV_FIELD_WRITE(camidi3,
           UNICAM_IDI3_IDI14_SHIFT,
           UNICAM_IDI3_IDI14_MASK,
           (uint32_t) aCSICfg->custImageIds[2]);
    UNICAM_DRV_FIELD_WRITE(camidi3,
           UNICAM_IDI3_IDI15_SHIFT,
           UNICAM_IDI3_IDI15_MASK,
           (uint32_t) aCSICfg->custImageIds[3]);
}

/** @brief Program Line Sync parameters

    Program Line Sync parameters for CSI.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aCSICfg  CSI interface configuration
    @param[in]  aIsHDR   Is HDR enabled?

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    linctlVal = 0UL

    linctlVal =  (uint32_t) aCSICfg->passLineSync
    if 0U == (linctlVal & UNICAM_LINCTL_PASS_LE_MASK)
        linctlVal |= UNICAM_LINCTL_CTL_GEN_LE_SYNC_MASK
    UNICAM_DRV_REG(camlinctl) = linctlVal
    @endcode
*/
static void UNICAM_DrvCSI_ConfigureLineSync(const UNICAM_CSIConfigType *aCSICfg,
                                   uint8_t aIsHDR)
{
    uint32_t linctlVal = 0UL;

    linctlVal =  (uint32_t) aCSICfg->passLineSync;
    if (0U == (linctlVal & UNICAM_LINCTL_PASS_LE_MASK)) {
        linctlVal |= UNICAM_LINCTL_CTL_GEN_LE_SYNC_MASK;
    }
    UNICAM_DRV_REG(camlinctl) = linctlVal;
}

/** @brief Program TG

    Program UNICAM TG registers

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aCSICfg  CSI interface configuration
    @param[in]  aCmnCfg  Common interface configuration
    @param[in]  aBPP     BPP of the stream

    @retval     #BCM_ERR_OK           TG configuration programming is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid burst length
    @retval     #BCM_ERR_INVAL_PARAMS fps or height is 0
    @retval     #BCM_ERR_NOSUPPORT    Unsupported TG mode

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK

    if 0U != (aTGCfg->tgCtl & UNICAM_VC5_TGCTL_MODE_MASK)
        ret = BCM_ERR_NOSUPPORT
    else if (0U == aCmnCfg->fps) || (0U == aCmnCfg->height)
        ret = BCM_ERR_INVAL_PARAMS
    else if aTGCfg->axiMaxBurstLength <= aTGCfg->axiMinBurstLength
        ret = BCM_ERR_INVAL_PARAMS
    else
        hBlank = 0UL
        vBlank = 0UL

        UNICAM_DRV_REG(cam_vc5_tgctl) = aTGCfg->tgCtl
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgctl,
               UNICAM_VC5_TGCTL_BPP_SHIFT,
               UNICAM_VC5_TGCTL_BPP_MASK,
               (16UL - (uint32_t) aBPP))

        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgisz,
               UNICAM_VC5_TGISZ_TG_WIDTH_SHIFT,
               UNICAM_VC5_TGISZ_TG_WIDTH_MASK,
               (uint32_t) aCmnCfg->width)
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgisz,
               UNICAM_VC5_TGISZ_TG_HEIGHT_SHIFT,
               UNICAM_VC5_TGISZ_TG_HEIGHT_MASK,
               (uint32_t) aCmnCfg->height)

        vBlank = (12UL * (aTGCfg->clkFrequency / 100UL)) / 1000UL
        hBlank = (((aTGCfg->clkFrequency /
                        aCmnCfg->fps) - vBlank) /
                        aCmnCfg->height) -
                        aCmnCfg->width
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgbl,
               UNICAM_VC5_TGBL_VBLANK_SHIFT,
               UNICAM_VC5_TGBL_VBLANK_MASK,
               (uint32_t) vBlank)
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgbl,
               UNICAM_VC5_TGBL_HBLANK_SHIFT,
               UNICAM_VC5_TGBL_HBLANK_MASK,
               (uint32_t) hBlank)
        UNICAM_DRV_FIELD_WRITE(cam_vc5_amm,
               UNICAM_VC5_AMM_MIN_SHIFT,
               UNICAM_VC5_AMM_MIN_MASK,
               (uint32_t) aTGCfg->axiMinBurstLength)
        UNICAM_DRV_FIELD_WRITE(cam_vc5_amm,
               UNICAM_VC5_AMM_MAX_SHIFT,
               UNICAM_VC5_AMM_MAX_MASK,
               (uint32_t) aTGCfg->axiMaxBurstLength)

        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgbsz,
               UNICAM_VC5_TGBSZ_BXSIZE_SHIFT,
               (UNICAM_VC5_TGBSZ_BXSIZE_MASK |
               UNICAM_VC5_TGBSZ_BYSIZE_MASK),
               (uint32_t)aTGCfg->blkSize)

    return ret
    @endcode
*/
static int32_t UNICAM_DrvConfigureTG(const UNICAM_TGConfigType *aTGCfg,
                                     UNICAM_CommonIfcConfigType *aCmnCfg,
                                     uint8_t aBPP)
{
    int32_t ret = BCM_ERR_OK;

    /* CAM_VC5_TGCTL */
    /* Check for AXI/TC Mode */
    if (0U != (aTGCfg->tgCtl & UNICAM_VC5_TGCTL_MODE_MASK)) {
        ret = BCM_ERR_NOSUPPORT;
    } else if ((0U == aCmnCfg->fps) || (0U == aCmnCfg->height)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (aTGCfg->axiMaxBurstLength <= aTGCfg->axiMinBurstLength) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        uint32_t hBlank = 0UL;
        uint32_t vBlank = 0UL;

        UNICAM_DRV_REG(cam_vc5_tgctl) = aTGCfg->tgCtl;
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgctl,
               UNICAM_VC5_TGCTL_BPP_SHIFT,
               UNICAM_VC5_TGCTL_BPP_MASK,
               (16UL - (uint32_t) aBPP));

        /* CAM_VC5_TGISZ */
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgisz,
               UNICAM_VC5_TGISZ_TG_WIDTH_SHIFT,
               UNICAM_VC5_TGISZ_TG_WIDTH_MASK,
               (uint32_t) aCmnCfg->width);
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgisz,
               UNICAM_VC5_TGISZ_TG_HEIGHT_SHIFT,
               UNICAM_VC5_TGISZ_TG_HEIGHT_MASK,
               (uint32_t) aCmnCfg->height);

        /* CAM_VC5_TGBL */
        vBlank = (12UL * (aTGCfg->clkFrequency / 100UL)) / 1000UL;
        hBlank = (((aTGCfg->clkFrequency /
                        aCmnCfg->fps) - vBlank) /
                        aCmnCfg->height) -
                        aCmnCfg->width;
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgbl,
               UNICAM_VC5_TGBL_VBLANK_SHIFT,
               UNICAM_VC5_TGBL_VBLANK_MASK,
               (uint32_t) vBlank);
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgbl,
               UNICAM_VC5_TGBL_HBLANK_SHIFT,
               UNICAM_VC5_TGBL_HBLANK_MASK,
               (uint32_t) hBlank);
        /* CAM_VC5_AMM */
        UNICAM_DRV_FIELD_WRITE(cam_vc5_amm,
               UNICAM_VC5_AMM_MIN_SHIFT,
               UNICAM_VC5_AMM_MIN_MASK,
               (uint32_t) aTGCfg->axiMinBurstLength);
        UNICAM_DRV_FIELD_WRITE(cam_vc5_amm,
               UNICAM_VC5_AMM_MAX_SHIFT,
               UNICAM_VC5_AMM_MAX_MASK,
               (uint32_t) aTGCfg->axiMaxBurstLength);

        /* CAM_VC5_TGBSZ */
        UNICAM_DRV_FIELD_WRITE(cam_vc5_tgbsz,
               UNICAM_VC5_TGBSZ_BXSIZE_SHIFT,
               (UNICAM_VC5_TGBSZ_BXSIZE_MASK |
               UNICAM_VC5_TGBSZ_BYSIZE_MASK),
               (uint32_t)aTGCfg->blkSize);
    }

    return ret;
}

/** @brief Pixel format code for BPP

    Return pixel format code for a given BPP

    @behavior Sync, Reentrant

    @pre None

    @param[in] aBPP     Bits per pixel

    @retval   uint32_t Pixel format code

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    switch (aBPP)
        case 8U
            code = 0x3UL
            break
        case 10U
            code = 0x4UL
            break
        case 12U
            code = 0x5UL
            break
        case 14U
            code = 0x6UL
            break
        case 16U
            code = 0x7UL
            break
        default
            code = 0x0UL
            break
    return code
    @endcode
*/
static uint32_t UNICAM_DrvBPP2ExpCode(uint8_t aBPP)
{
    uint32_t code;

    switch (aBPP) {
        case 8U:
            code = 0x3UL;
            break;
        case 10U:
            code = 0x4UL;
            break;
        case 12U:
            code = 0x5UL;
            break;
        case 14U:
            code = 0x6UL;
            break;
        case 16U:
            code = 0x7UL;
            break;
        default:
            code = 0x0UL;
            break;
    }

    return code;
}

/** @brief Program HDR configurations

    Program HDR configurations for UNICAM

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aIfcConfig Common interface configurations
    @param[in]  aCSICfg    CSI interface configuration
    @param[in]  aModeCfg   UNICAM Mode configuration
    @param[in]  aLongBPP   BPP for long exposure
    @param[in]  aShortBPP  BPP for short exposure

    @retval     void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    if (0UL != aIfcConfig->hdr)
        if aModeCfg->format[UNICAM_OUTPUT_IDX_LONG] !=
            aModeCfg->format[UNICAM_OUTPUT_IDX_SHORT]
            UNICAM_DRV_FIELD_WRITE(camexp1ctl,
                               UNICAM_EXP1CTL_PIXF_EXP0_SHIFT,
                               UNICAM_EXP1CTL_PIXF_EXP0_MASK,
                               UNICAM_DrvBPP2ExpCode(aLongBPP))
            UNICAM_DRV_FIELD_WRITE(camexp1ctl,
                               UNICAM_EXP1CTL_PIXF_EXP1_SHIFT,
                               UNICAM_EXP1CTL_PIXF_EXP1_MASK,
                               UNICAM_DrvBPP2ExpCode(aShortBPP))
            UNICAM_DRV_REG(camhdrctl) |= UNICAM_HDRCTL_MULTI_RES_HDR_MASK
        else
            UNICAM_DRV_REG(camhdrctl) &= ~(UNICAM_HDRCTL_MULTI_RES_HDR_MASK)

        UNICAM_DRV_FIELD_WRITE(camhdrctl,
                           UNICAM_HDRCTL_HDR_CSI2_SE_VC_SHIFT,
                           UNICAM_HDRCTL_HDR_CSI2_SE_VC_MASK,
                           (uint32_t) aCSICfg->vcNum[UNICAM_OUTPUT_IDX_SHORT])
        UNICAM_DRV_FIELD_WRITE(camhdrctl,
                           UNICAM_HDRCTL_HDR_CSI2_LE_VC_SHIFT,
                           UNICAM_HDRCTL_HDR_CSI2_LE_VC_MASK,
                           (uint32_t) aCSICfg->vcNum[UNICAM_OUTPUT_IDX_LONG])
        UNICAM_DRV_REG(camhdrctl) |= UNICAM_HDRCTL_HDR_CSI2_VC_EN_MASK
        UNICAM_DRV_REG(camhdrctl) |= UNICAM_HDRCTL_HDR_EN_MASK
    else
        UNICAM_DRV_REG(camhdrctl) = 0
    @endcode
*/
static void UNICAM_DrvConfigureHDR(UNICAM_CommonIfcConfigType *aIfcConfig,
                                   const UNICAM_CSIConfigType *aCSICfg,
                                   const UNICAM_ModeConfigType *aModeCfg,
                                   uint8_t aLongBPP,
                                   uint8_t aShortBPP)
{
    if (0UL != aIfcConfig->hdr) {
        if (aModeCfg->format[UNICAM_OUTPUT_IDX_LONG] !=
            aModeCfg->format[UNICAM_OUTPUT_IDX_SHORT]) {
            UNICAM_DRV_FIELD_WRITE(camexp1ctl,
                               UNICAM_EXP1CTL_PIXF_EXP0_SHIFT,
                               UNICAM_EXP1CTL_PIXF_EXP0_MASK,
                               UNICAM_DrvBPP2ExpCode(aLongBPP));
            UNICAM_DRV_FIELD_WRITE(camexp1ctl,
                               UNICAM_EXP1CTL_PIXF_EXP1_SHIFT,
                               UNICAM_EXP1CTL_PIXF_EXP1_MASK,
                               UNICAM_DrvBPP2ExpCode(aShortBPP));
            UNICAM_DRV_REG(camhdrctl) |= UNICAM_HDRCTL_MULTI_RES_HDR_MASK;
        } else {
            UNICAM_DRV_REG(camhdrctl) &= ~(UNICAM_HDRCTL_MULTI_RES_HDR_MASK);
        }

        UNICAM_DRV_FIELD_WRITE(camhdrctl,
                           UNICAM_HDRCTL_HDR_CSI2_SE_VC_SHIFT,
                           UNICAM_HDRCTL_HDR_CSI2_SE_VC_MASK,
                           (uint32_t) aCSICfg->vcNum[UNICAM_OUTPUT_IDX_SHORT]);
        UNICAM_DRV_FIELD_WRITE(camhdrctl,
                           UNICAM_HDRCTL_HDR_CSI2_LE_VC_SHIFT,
                           UNICAM_HDRCTL_HDR_CSI2_LE_VC_MASK,
                           (uint32_t) aCSICfg->vcNum[UNICAM_OUTPUT_IDX_LONG]);
        UNICAM_DRV_REG(camhdrctl) |= UNICAM_HDRCTL_HDR_CSI2_VC_EN_MASK;
        UNICAM_DRV_REG(camhdrctl) |= UNICAM_HDRCTL_HDR_EN_MASK;
    } else {
        UNICAM_DRV_REG(camhdrctl) = 0x0UL;
    }
}

/** @brief Program Data configurations

    Program UNICAM data configurations

    @behavior Sync, Non-reentrant

    @pre None

    @param[in] aIfcConfig Common interface configurations

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    metaStart = UNICAM_DRV_ED_START_LINES(UNICAM_OUTPUT_IDX_LONG,
                                      aIfcConfig->metaLines)
    metaEnd = UNICAM_DRV_ED_END_LINES(UNICAM_OUTPUT_IDX_LONG,
                                  aIfcConfig->metaLines)
    UNICAM_DRV_FIELD_WRITE(camdcs,
                       UNICAM_DCS_EDL_SHIFT,
                       UNICAM_DCS_EDL_MASK,
                       metaStart)
    UNICAM_DRV_FIELD_WRITE(camdcs,
                       UNICAM_DCS_EDL_END_SHIFT,
                       UNICAM_DCS_EDL_END_MASK,
                       metaEnd)
    metaStart = UNICAM_DRV_ED_START_LINES(UNICAM_OUTPUT_IDX_SHORT,
                                      aIfcConfig->metaLines)
    metaEnd = UNICAM_DRV_ED_END_LINES(UNICAM_OUTPUT_IDX_SHORT,
                                  aIfcConfig->metaLines)
    UNICAM_DRV_FIELD_WRITE(camidata_se,
                       UNICAM_IDATA_SE_EDL_SE_SHIFT,
                       UNICAM_IDATA_SE_EDL_SE_MASK,
                       metaStart)
    UNICAM_DRV_FIELD_WRITE(camidata_se,
                       UNICAM_IDATA_SE_EDL_END_SE_SHIFT,
                       UNICAM_IDATA_SE_EDL_END_SE_MASK,
                       metaEnd)
    @endcode
*/
static void UNICAM_DrvConfigureData(UNICAM_CommonIfcConfigType *aIfcConfig)
{
    uint32_t metaStart;
    uint32_t metaEnd;

    metaStart = UNICAM_DRV_ED_START_LINES(UNICAM_OUTPUT_IDX_LONG, aIfcConfig->metaLines);
    metaEnd = UNICAM_DRV_ED_END_LINES(UNICAM_OUTPUT_IDX_LONG, aIfcConfig->metaLines);

    UNICAM_DRV_FIELD_WRITE(camdcs,
                       UNICAM_DCS_EDL_SHIFT,
                       UNICAM_DCS_EDL_MASK,
                       metaStart);
    UNICAM_DRV_FIELD_WRITE(camdcs,
                       UNICAM_DCS_EDL_END_SHIFT,
                       UNICAM_DCS_EDL_END_MASK,
                       metaEnd);

    metaStart = UNICAM_DRV_ED_START_LINES(UNICAM_OUTPUT_IDX_SHORT, aIfcConfig->metaLines);
    metaEnd = UNICAM_DRV_ED_END_LINES(UNICAM_OUTPUT_IDX_SHORT, aIfcConfig->metaLines);

    UNICAM_DRV_FIELD_WRITE(camidata_se,
                       UNICAM_IDATA_SE_EDL_SE_SHIFT,
                       UNICAM_IDATA_SE_EDL_SE_MASK,
                       metaStart);
    UNICAM_DRV_FIELD_WRITE(camidata_se,
                       UNICAM_IDATA_SE_EDL_END_SE_SHIFT,
                       UNICAM_IDATA_SE_EDL_END_SE_MASK,
                       metaEnd);
}

/** @brief Program Frame-Size configuration

    Program frame-size configurations

    @behavior Sync, Non-reentrant

    @pre None

    @param[in] aIfcConfig Common interface configuration
    @param[in] aModeCfg   Mode configuration
    @param[in] aBPP       Bits per pixel

    @return void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    if 0UL != aIfcConfig->hdr
        UNICAM_DRV_FIELD_WRITE(camfrsz,
                       UNICAM_FRSZ_CTL_FRAME_HEIGHT_SHIFT,
                       UNICAM_FRSZ_CTL_FRAME_HEIGHT_MASK,
                       (uint32_t) aIfcConfig->height)
    else
        uint8_t metalinesStart = (aIfcConfig->metaLines[UNICAM_OUTPUT_IDX_LONG] &
                                  UNICAM_MODECFG_META_LINES_START_MASK) >>
                                    UNICAM_MODECFG_META_LINES_START_SHIFT
        uint8_t metalinesEnd = (aIfcConfig->metaLines[UNICAM_OUTPUT_IDX_LONG] &
                                    UNICAM_MODECFG_META_LINES_END_MASK) >>
                                        UNICAM_MODECFG_META_LINES_END_SHIFT
        UNICAM_DRV_FIELD_WRITE(camfrsz,
                       UNICAM_FRSZ_CTL_FRAME_HEIGHT_SHIFT,
                       UNICAM_FRSZ_CTL_FRAME_HEIGHT_MASK,
                       ((uint32_t) aIfcConfig->height +
                       (uint32_t) metalinesStart +
                       (uint32_t) metalinesEnd))

    UNICAM_DRV_FIELD_WRITE(camfrsz,
                   UNICAM_FRSZ_CTL_LINE_LENGTH_SHIFT,
                   UNICAM_FRSZ_CTL_LINE_LENGTH_MASK,
                   (((uint32_t) aIfcConfig->width) *
                    (uint32_t) aBPP +
                    4UL) >> 3UL)

    if UNICAM_MODE_TG == aModeCfg->peripheralMode
        UNICAM_DRV_FIELD_WRITE(camisp_hwsyc_ctl,
                       UNICAM_ISP_HWSYC_CTL_LINE_LENGTHMEM_SHIFT,
                       UNICAM_ISP_HWSYC_CTL_LINE_LENGTHMEM_MASK,
                       (((uint32_t) aIfcConfig->width) *
                         (((uint32_t) aBPP + 7UL) & ~7UL) +
                        4UL) >> 3UL)
    else
        UNICAM_DRV_FIELD_WRITE(camisp_hwsyc_ctl,
                       UNICAM_ISP_HWSYC_CTL_LINE_LENGTHMEM_SHIFT,
                       UNICAM_ISP_HWSYC_CTL_LINE_LENGTHMEM_MASK,
                       (((uint32_t) aIfcConfig->width) *
                         (uint32_t) aBPP +
                        4UL) >> 3UL)
    @endcode
*/
static void UNICAM_DrvConfigureFrameSize(UNICAM_CommonIfcConfigType *aIfcConfig,
                                      const UNICAM_ModeConfigType *aModeCfg,
                                      uint8_t aBPP)
{
    if (0UL != aIfcConfig->hdr) {
        UNICAM_DRV_FIELD_WRITE(camfrsz,
                       UNICAM_FRSZ_CTL_FRAME_HEIGHT_SHIFT,
                       UNICAM_FRSZ_CTL_FRAME_HEIGHT_MASK,
                       (uint32_t) aIfcConfig->height);
    } else {
        uint8_t metalinesStart = (aIfcConfig->metaLines[UNICAM_OUTPUT_IDX_LONG] &
                                  UNICAM_MODECFG_META_LINES_START_MASK) >>
                                    UNICAM_MODECFG_META_LINES_START_SHIFT;
        uint8_t metalinesEnd = (aIfcConfig->metaLines[UNICAM_OUTPUT_IDX_LONG] &
                                    UNICAM_MODECFG_META_LINES_END_MASK) >>
                                        UNICAM_MODECFG_META_LINES_END_SHIFT;
        UNICAM_DRV_FIELD_WRITE(camfrsz,
                       UNICAM_FRSZ_CTL_FRAME_HEIGHT_SHIFT,
                       UNICAM_FRSZ_CTL_FRAME_HEIGHT_MASK,
                       ((uint32_t) aIfcConfig->height +
                       (uint32_t) metalinesStart +
                       (uint32_t) metalinesEnd));
    }

    UNICAM_DRV_FIELD_WRITE(camfrsz,
                   UNICAM_FRSZ_CTL_LINE_LENGTH_SHIFT,
                   UNICAM_FRSZ_CTL_LINE_LENGTH_MASK,
                   (((uint32_t) aIfcConfig->width) *
                    (uint32_t) aBPP +
                    4UL) >> 3UL);

    if (UNICAM_MODE_TG == aModeCfg->peripheralMode) {
        UNICAM_DRV_FIELD_WRITE(camisp_hwsyc_ctl,
                       UNICAM_ISP_HWSYC_CTL_LINE_LENGTHMEM_SHIFT,
                       UNICAM_ISP_HWSYC_CTL_LINE_LENGTHMEM_MASK,
                       (((uint32_t) aIfcConfig->width) *
                         (((uint32_t) aBPP + 7UL) & ~7UL) +
                        4UL) >> 3UL);
    } else {
        UNICAM_DRV_FIELD_WRITE(camisp_hwsyc_ctl,
                       UNICAM_ISP_HWSYC_CTL_LINE_LENGTHMEM_SHIFT,
                       UNICAM_ISP_HWSYC_CTL_LINE_LENGTHMEM_MASK,
                       (((uint32_t) aIfcConfig->width) *
                         (uint32_t) aBPP +
                        4UL) >> 3UL);
    }
}

/**
    @brief Pre configure parameter check

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
*/
static int32_t UNICAM_DrvPreConfigParamCheck(const UNICAM_ImageType *aCfg,
                                   UNICAM_CommonIfcConfigType *aIfcConfig,
                                   UNICAM_BufParamType *aBuf) 
{
    int32_t ret = BCM_ERR_OK;

    if ((UNICAM_DRV_STATE_INITIALISED != UNICAM_DrvContext.state) &&
        (UNICAM_DRV_STATE_READY != UNICAM_DrvContext.state)) {
        ret = BCM_ERR_INVAL_STATE;
        UNICAM_DrvReportError(0U,
                ret, UNICAM_DrvContext.state, 0UL, 0UL, 0UL);
    } else if (NULL == aCfg) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    } else if (NULL == aBuf) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    } else if (NULL == aIfcConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    } else if (UNICAM_IMG_CONFIG_UNICAM != aCfg->imgHdr.cfgType) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__, aCfg->imgHdr.cfgType, 0UL, 0UL);
    } else if (UNICAM_MAGIC != aCfg->imgHdr.cfgMagic) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__, aCfg->imgHdr.cfgMagic, 0UL, 0UL);
    } else if (aCfg->modeConfig.peripheralMode >= UNICAM_MODE_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__,
                aCfg->modeConfig.peripheralMode, 0UL, 0UL);
    } else if (UNICAM_MAX_NUM_IMG_BUFS < aBuf->numImgBufs) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

/** @brief Pre-Configure check function

    Perform pre-configuration check for parameters and state

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aP_Cfg        UNICAM sub-component configuration
    @param[in]  aIfcConfig    UNICAM common interface configuration
    @param[in]  aBuf          UNICAM buffer configuration
    @param[out] aLongBPP      BPP for long exposure
    @param[out] aShortBPP     BPP for short exposure
    @param[out] aLongImageID  Image ID for long exposure
    @param[out] aShortImageID Image ID for short exposure

    @retval    #BCM_ERR_OK           Configuration parameter check is successful
    @retval    #BCM_ERR_INVAL_PARAMS Invalid Pixel format for short channel
    @retval    #BCM_ERR_INVAL_PARAMS Invalid pixel format for Long channel
    @retval    #BCM_ERR_INVAL_PARAMS Invalid number of lanes or hs clock frequency
    @retval    #BCM_ERR_INVAL_PARAMS Peripheral mode is neither CSI nor TG
    @retval    #BCM_ERR_INVAL_PARAMS Invalid number of image buffers
    @retval    #BCM_ERR_INVAL_PARAMS Invalid peripheral mode
    @retval    #BCM_ERR_INVAL_PARAMS Invalid magic
    @retval    #BCM_ERR_INVAL_PARAMS Invalid config type
    @retval    #BCM_ERR_INVAL_PARAMS aIfcConfig is NULL
    @retval    #BCM_ERR_INVAL_PARAMS aBuf is NULL
    @retval    #BCM_ERR_INVAL_PARAMS aCfg is NULL
    @retval    #BCM_ERR_INVAL_STATE  State is neither UNICAM_DRV_STATE_INITIALISED nor
                                     UNICAM_DRV_STATE_READY

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    if (UNICAM_DRV_STATE_INITIALISED != UNICAM_DrvContext.state) &&
        (UNICAM_DRV_STATE_READY != UNICAM_DrvContext.state)
        ret = BCM_ERR_INVAL_STATE
        Report_Error
    else if (NULL == aCfg)
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else if (NULL == aBuf
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else if (NULL == aIfcConfig
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else if (ISYS_CONFIG_UNICAM != aCfg->imgHdr.cfgType
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else if (UNICAM_MAGIC != aCfg->imgHdr.cfgMagic
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else if (aCfg->modeConfig.peripheralMode >= UNICAM_MODE_MAX
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else if (UNICAM_MAX_NUM_IMG_BUFS < aBuf->numImgBufs
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else if ((UNICAM_MODE_CSI != aCfg->modeConfig.peripheralMode) &&
        (UNICAM_MODE_TG != aCfg->modeConfig.peripheralMode)
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else if ((UNICAM_MODE_CSI == aCfg->modeConfig.peripheralMode) &&
               ((((aCfg->InterfaceConfig.csiConfig.dataLaneCtl &
                   UNICAM_CSICFG_DATALANE_NUM_LINES_MASK) >>
                   UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT) >
                   UNICAM_MAX_DATA_LANES) ||
        (0UL == aCfg->InterfaceConfig.csiConfig.hsClkFrequency))
        ret = BCM_ERR_INVAL_PARAMS
        Report_Error
    else
       ret = UNICAM_DrvGetFormatInfo(aCfg->modeConfig.format[
                                                    UNICAM_OUTPUT_IDX_LONG],
                                                    aLongBPP,
                                                    aLongImageID)
        if BCM_ERR_OK != ret
            Report_Error
        else
            if 0UL != aIfcConfig->hdr
                ret = UNICAM_DrvGetFormatInfo(aCfg->modeConfig.format[
                                              UNICAM_OUTPUT_IDX_SHORT],
                                              aShortBPP,
                                              aShortImageID)
                if BCM_ERR_OK != ret
                    Report_Error
    return ret
    @endcode
*/
static int32_t UNICAM_DrvPreConfigureCheck(const UNICAM_ImageType *aCfg,
                                   UNICAM_CommonIfcConfigType *aIfcConfig,
                                   UNICAM_BufParamType *aBuf,
                                   uint8_t *aLongBPP,
                                   uint8_t *aShortBPP,
                                   uint8_t *aLongImageID,
                                   uint8_t *aShortImageID)
{
    int32_t ret = BCM_ERR_OK;

    /* Check State */
    ret = UNICAM_DrvPreConfigParamCheck(aCfg, aIfcConfig, aBuf);
    if ( ret != BCM_ERR_OK) {
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__,
                0UL, 0UL, 0UL);
    } else if ((UNICAM_MODE_CSI != aCfg->modeConfig.peripheralMode) &&
        (UNICAM_MODE_TG != aCfg->modeConfig.peripheralMode)) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
                ret, (uint32_t) __LINE__,
                aCfg->modeConfig.peripheralMode, 0UL, 0UL);
    } else if ((UNICAM_MODE_CSI == aCfg->modeConfig.peripheralMode) &&
               ((((aCfg->InterfaceConfig.csiConfig.dataLaneCtl &
                   UNICAM_CSICFG_DATALANE_NUM_LINES_MASK) >>
                   UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT) >
                   UNICAM_MAX_DATA_LANES) ||
                (0UL == aCfg->InterfaceConfig.csiConfig.hsClkFrequency))) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(0U,
            ret,
            (uint32_t) __LINE__,
            (uint32_t) (((uint32_t) aCfg->InterfaceConfig.csiConfig.dataLaneCtl &
                         UNICAM_CSICFG_DATALANE_NUM_LINES_MASK) >>
                         UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT),
            (uint32_t) aCfg->InterfaceConfig.csiConfig.hsClkFrequency,
            0UL);
    } else {
        ret = UNICAM_DrvGetFormatInfo(aCfg->modeConfig.format[
                                                    UNICAM_OUTPUT_IDX_LONG],
                                                    aLongBPP,
                                                    aLongImageID);
        if (BCM_ERR_OK != ret) {
            UNICAM_DrvReportError(0U,
                    ret, (uint32_t) __LINE__,
                    aCfg->modeConfig.format[UNICAM_OUTPUT_IDX_LONG], 0UL, 0UL);
        } else {
            if (0UL != aIfcConfig->hdr) {
                ret = UNICAM_DrvGetFormatInfo(aCfg->modeConfig.format[
                                              UNICAM_OUTPUT_IDX_SHORT],
                                              aShortBPP,
                                              aShortImageID);
                if (BCM_ERR_OK != ret) {
                    UNICAM_DrvReportError(0U,
                            ret, (uint32_t) __LINE__,
                            aCfg->modeConfig.format[UNICAM_OUTPUT_IDX_SHORT],
                            0UL, 0UL);
                }
            }
        }
    }

    return ret;
}

/** @brief Check buffer parameters

    Check buffer parameters

    @behavior Sync, Non-reentrant

    @pre None

    @param  void

    @retval #BCM_ERR_OK           Buffer check is successful
    @retval #BCM_ERR_INVAL_PARAMS Invalid data buffer for short exposure
    @retval #BCM_ERR_INVAL_PARAMS Invalid image buffer for short exposure
    @retval #BCM_ERR_INVAL_PARAMS Invalid data buffer for long exposure
    @retval #BCM_ERR_INVAL_PARAMS Invalid image buffer for long exposure

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    int32_t ret

    if ((0UL == aUCMLongImgBuf->startAddr) ||
        (0UL == aUCMLongImgBuf->endAddr)) {
        ret = BCM_ERR_INVAL_PARAMS;
        Report_Error
    } else if ((0UL != aEDEnableLong) && ((0UL == aUCMLongDataBuf->startAddr) ||
                                         (0UL == aUCMLongDataBuf->endAddr))) {
        ret = BCM_ERR_INVAL_PARAMS;
        Report_Error
    } else if ((0UL != UNICAM_DrvContext.isHDR) &&
               ((0UL == aUCMShortImgBuf->startAddr) ||
                (0UL == aUCMShortImgBuf->endAddr))) {
        ret = BCM_ERR_INVAL_PARAMS;
        Report_Error
    } else if ((0UL != UNICAM_DrvContext.isHDR) && (0UL != aEDEnableShort) &&
               ((0UL == aUCMShortDataBuf->startAddr) ||
                (0UL == aUCMShortDataBuf->endAddr))) {
        ret = BCM_ERR_INVAL_PARAMS;
        Report_Error
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;

    return ret
    @endcode
*/
static int32_t UNICAM_DrvProgramBufCheck(uint32_t aEDEnableLong,
                                         uint32_t aEDEnableShort,
                                         UNICAM_BufferType *aUCMLongImgBuf,
                                         UNICAM_BufferType *aUCMShortImgBuf,
                                         UNICAM_BufferType *aUCMLongDataBuf,
                                         UNICAM_BufferType *aUCMShortDataBuf,
                                         uint32_t aIsHDR)
{
    int32_t ret;

    if ((0UL == aUCMLongImgBuf->startAddr) ||
        (0UL == aUCMLongImgBuf->endAddr)) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_PROGRAM_BUF_CHECK_PROC,
                ret, (uint32_t) __LINE__,
                aUCMLongImgBuf->startAddr, aUCMLongImgBuf->endAddr, 0UL);
    } else if ((0UL != aEDEnableLong) && ((0UL == aUCMLongDataBuf->startAddr) ||
                                         (0UL == aUCMLongDataBuf->endAddr))) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_PROGRAM_BUF_CHECK_PROC,
                ret, (uint32_t) __LINE__, aUCMLongDataBuf->startAddr,
                aUCMLongDataBuf->endAddr, 0UL);
    } else if ((0UL != UNICAM_DrvContext.isHDR) &&
               ((0UL == aUCMShortImgBuf->startAddr) ||
                (0UL == aUCMShortImgBuf->endAddr))) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_PROGRAM_BUF_CHECK_PROC,
                ret, (uint32_t) __LINE__, aUCMShortImgBuf->startAddr,
                aUCMShortImgBuf->endAddr, 0UL);
    } else if ((0UL != UNICAM_DrvContext.isHDR) && (0UL != aEDEnableShort) &&
               ((0UL == aUCMShortDataBuf->startAddr) ||
                (0UL == aUCMShortDataBuf->endAddr))) {
        ret = BCM_ERR_INVAL_PARAMS;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_PROGRAM_BUF_CHECK_PROC,
                ret, (uint32_t) __LINE__, aUCMShortDataBuf->startAddr,
                aUCMShortDataBuf->endAddr, 0UL);
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

/** @brief Program buffer addresses

    Program data and image addresses

    @behavior Sync, Non-reentrant

    @pre None

    @param  void

    @retval #BCM_ERR_OK           Buffer address programming is successful
    @retval #BCM_ERR_INVAL_PARAMS Invalid data buffer for short exposure
    @retval #BCM_ERR_INVAL_PARAMS Invalid image buffer for short exposure
    @retval #BCM_ERR_INVAL_PARAMS Invalid data buffer for long exposure
    @retval #BCM_ERR_INVAL_PARAMS Invalid image buffer for long exposure

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    uint32_t edEnableLong = UNICAM_DRV_REG(camdcs) &
                           (UNICAM_DCS_EDL_MASK | UNICAM_DCS_EDL_END_MASK)
    uint32_t edEnableShort = UNICAM_DRV_REG(camidata_se) &
                            (UNICAM_IDATA_SE_EDL_SE_MASK |
                            UNICAM_IDATA_SE_EDL_END_SE_MASK)
    UNICAM_BufferType *ucmLongImgBuf = &UNICAM_DrvBufContext.buf.imgBuf[
                       UNICAM_DrvBufContext.activeIBIndex][UNICAM_OUTPUT_IDX_LONG]
    UNICAM_BufferType *ucmShortImgBuf = &UNICAM_DrvBufContext.buf.imgBuf[
                      UNICAM_DrvBufContext.activeIBIndex][UNICAM_OUTPUT_IDX_SHORT]
    UNICAM_BufferType *ucmLongDataBuf = &UNICAM_DrvBufContext.buf.dataBuf[
                       UNICAM_DrvBufContext.activeIBIndex][UNICAM_OUTPUT_IDX_LONG]
    UNICAM_BufferType *ucmShortDataBuf = &UNICAM_DrvBufContext.buf.dataBuf[
                      UNICAM_DrvBufContext.activeIBIndex][UNICAM_OUTPUT_IDX_SHORT]

    UNICAM_DrvContext.isHDR = (UNICAM_OUTPUT_IDX_MAX ==
                            UNICAM_DrvBufContext.buf.numExposures) ? 1UL : 0UL

    ret = UNICAM_DrvProgramBufCheck(edEnableLong,
                                    edEnableShort,
                                    ucmLongImgBuf,
                                    ucmShortImgBuf,
                                    ucmLongDataBuf,
                                    ucmShortDataBuf,
                                    UNICAM_DrvContext.isHDR)
    if BCM_ERR_OK != ret
        Report_Error
    else
        UNICAM_DRV_REG(camibsa0) = ucmLongImgBuf->startAddr
        UNICAM_DRV_REG(camibea0) = (ucmLongImgBuf->endAddr - 16UL)

         if 0UL != edEnableLong
            UNICAM_DRV_REG(camdbsa0) = ucmLongDataBuf->startAddr
            UNICAM_DRV_REG(camdbea0) = (ucmLongDataBuf->endAddr)
            if 0UL != (UNICAM_DRV_REG(camdcs) & UNICAM_DCS_EDL_MASK)
                UNICAM_DrvContext.edInterruptMask |= UNICAM_DCS_DIE_MASK
                UNICAM_DrvContext.dcsRxdMask |= UNICAM_DRV_DONE_EXP_LONG
            }
            if 0UL != (UNICAM_DRV_REG(camdcs) & UNICAM_DCS_EDL_END_MASK)
                UNICAM_DrvContext.edInterruptMask |= UNICAM_DCS_DIE_END_MASK
                UNICAM_DrvContext.dcsEndRxdMask |= UNICAM_DRV_DONE_EXP_LONG
            }
        }

        if 0UL != UNICAM_DrvContext.isHDR
            UNICAM_DRV_REG(camibsa1) = ucmShortImgBuf->startAddr
            UNICAM_DRV_REG(camibea1) = (ucmShortImgBuf->endAddr - 16UL)

            if 0UL != edEnableShort
                UNICAM_DRV_REG(camdbsa1) = ucmShortDataBuf->startAddr
                UNICAM_DRV_REG(camdbea1) = (ucmShortDataBuf->endAddr)

                if 0UL != (UNICAM_DRV_REG(camidata_se) &
                                                UNICAM_IDATA_SE_EDL_SE_MASK)
                    UNICAM_DrvContext.interruptEnableSE |=
                                                    UNICAM_ICTL_SE_DI_SE_EN_MASK
                    UNICAM_DrvContext.dcsRxdMask |= UNICAM_DRV_DONE_EXP_SHORT
                if 0UL != (UNICAM_DRV_REG(camidata_se) &
                                            UNICAM_IDATA_SE_EDL_END_SE_MASK)
                    UNICAM_DrvContext.interruptEnableSE |=
                                                 UNICAM_ICTL_SE_DI_END_SE_EN_MASK
                    UNICAM_DrvContext.dcsEndRxdMask |= UNICAM_DRV_DONE_EXP_SHORT

    return ret
    @endcode
*/
static int32_t UNICAM_DrvProgramBuf(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t edEnableLong = UNICAM_DRV_REG(camdcs) &
                           (UNICAM_DCS_EDL_MASK | UNICAM_DCS_EDL_END_MASK);
    uint32_t edEnableShort = UNICAM_DRV_REG(camidata_se) &
                            (UNICAM_IDATA_SE_EDL_SE_MASK |
                            UNICAM_IDATA_SE_EDL_END_SE_MASK);
    UNICAM_BufferType *ucmLongImgBuf = &UNICAM_DrvBufContext.buf.imgBuf[
                       UNICAM_DrvBufContext.activeIBIndex][UNICAM_OUTPUT_IDX_LONG];
    UNICAM_BufferType *ucmShortImgBuf = &UNICAM_DrvBufContext.buf.imgBuf[
                      UNICAM_DrvBufContext.activeIBIndex][UNICAM_OUTPUT_IDX_SHORT];
    UNICAM_BufferType *ucmLongDataBuf = &UNICAM_DrvBufContext.buf.dataBuf[
                       UNICAM_DrvBufContext.activeIBIndex][UNICAM_OUTPUT_IDX_LONG];
    UNICAM_BufferType *ucmShortDataBuf = &UNICAM_DrvBufContext.buf.dataBuf[
                      UNICAM_DrvBufContext.activeIBIndex][UNICAM_OUTPUT_IDX_SHORT];

    /*  Enable HDR buffer configs */
    UNICAM_DrvContext.isHDR = (UNICAM_OUTPUT_IDX_MAX ==
                            UNICAM_DrvBufContext.buf.numExposures) ? 1UL : 0UL;

    ret = UNICAM_DrvProgramBufCheck(edEnableLong,
                                    edEnableShort,
                                    ucmLongImgBuf,
                                    ucmShortImgBuf,
                                    ucmLongDataBuf,
                                    ucmShortDataBuf,
                                    UNICAM_DrvContext.isHDR);
    if (BCM_ERR_OK != ret) {
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_PROGRAM_BUF_PROC,
                ret, (uint32_t) __LINE__,
                ucmLongImgBuf->startAddr, ucmLongImgBuf->endAddr, 0UL);
    } else {
        /* Load Image Buffer Address */
        UNICAM_DRV_REG(camibsa0) = ucmLongImgBuf->startAddr;
        UNICAM_DRV_REG(camibea0) = (ucmLongImgBuf->endAddr - 16UL);

        /* Check for embedded data enable */
        if (0UL != edEnableLong) {
            UNICAM_DRV_REG(camdbsa0) = ucmLongDataBuf->startAddr;
            UNICAM_DRV_REG(camdbea0) = (ucmLongDataBuf->endAddr);
            /* Update DI mask */
            if (0UL != (UNICAM_DRV_REG(camdcs) & UNICAM_DCS_EDL_MASK)) {
                UNICAM_DrvContext.edInterruptMask |= UNICAM_DCS_DIE_MASK;
                UNICAM_DrvContext.dcsRxdMask |= UNICAM_DRV_DONE_EXP_LONG;
            }
            /* Update DI(END) mask */
            if (0UL != (UNICAM_DRV_REG(camdcs) & UNICAM_DCS_EDL_END_MASK)) {
                UNICAM_DrvContext.edInterruptMask |= UNICAM_DCS_DIE_END_MASK;
                UNICAM_DrvContext.dcsEndRxdMask |= UNICAM_DRV_DONE_EXP_LONG;
            }
        }

        if (0UL != UNICAM_DrvContext.isHDR) {
            UNICAM_DRV_REG(camibsa1) = ucmShortImgBuf->startAddr;
            UNICAM_DRV_REG(camibea1) = (ucmShortImgBuf->endAddr - 16UL);

            /* Check for embedded data enable */
            if (0UL != edEnableShort) {
                /* Load Data Buffer Address */
                UNICAM_DRV_REG(camdbsa1) = ucmShortDataBuf->startAddr;
                UNICAM_DRV_REG(camdbea1) = (ucmShortDataBuf->endAddr);

                /* Update DI(SE) mask */
                if (0UL != (UNICAM_DRV_REG(camidata_se) &
                                                UNICAM_IDATA_SE_EDL_SE_MASK)) {
                    UNICAM_DrvContext.interruptEnableSE |=
                                                    UNICAM_ICTL_SE_DI_SE_EN_MASK;
                    UNICAM_DrvContext.dcsRxdMask |= UNICAM_DRV_DONE_EXP_SHORT;
                }
                /* Update DI END(SE) mask */
                if (0UL != (UNICAM_DRV_REG(camidata_se) &
                                            UNICAM_IDATA_SE_EDL_END_SE_MASK)) {
                    UNICAM_DrvContext.interruptEnableSE |=
                                                 UNICAM_ICTL_SE_DI_END_SE_EN_MASK;
                    UNICAM_DrvContext.dcsEndRxdMask |= UNICAM_DRV_DONE_EXP_SHORT;
                }
            }
        }
    }

    return ret;
}

/** @brief Configure CSI parameters

    Configure CSI parameters

    @behavior Sync, Non-reentrant

    @pre None

    @param[in] aCfg          Unicam configuration
    @param[in] aIfcConfig    Common interface configurations
    @param[in] aLongImageID  Image ID for long exposure
    @param[in] aShortImageID Image ID for short exposure

    @retval    #BCM_ERR_OK           CSI configuration is successfully programmed
    @retval    #BCM_ERR_INVAL_PARAMS hsByteClkTime is zero

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK
    const UNICAM_CSIConfigType *csiConfig = &aCfg->InterfaceConfig.csiConfig

    ret = UNICAM_DrvCSI_ConfigureTiming(csiConfig)
    if BCM_ERR_OK != ret
        Report_Error
    else
        ret = UNICAM_DrvCSI_ConfigureImageID(csiConfig,
                                             aIfcConfig->hdr,
                                             aLongImageID,
                                             aShortImageID)
        if BCM_ERR_OK != ret
            Report_Error
        else
            UNICAM_DrvCSI_ConfigureLineSync(csiConfig, aIfcConfig->hdr)
    return ret
    @endcode
*/
static int32_t UNICAM_DrvConfigureCSI(const UNICAM_ImageType *aCfg,
                               UNICAM_CommonIfcConfigType *aIfcConfig,
                               uint8_t aLongImageID,
                               uint8_t aShortImageID)
{
    int32_t ret = BCM_ERR_OK;
    const UNICAM_CSIConfigType *csiConfig = &aCfg->InterfaceConfig.csiConfig;

    ret = UNICAM_DrvCSI_ConfigureTiming(csiConfig);
    if (BCM_ERR_OK != ret) {
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_CSI_PROC,
                ret, (uint32_t) __LINE__, aCfg->imgHdr.cfgMagic, 0UL, 0UL);
    } else {
        /* Image ID */
        UNICAM_DrvCSI_ConfigureImageID(csiConfig,
                                       aIfcConfig->hdr,
                                       aLongImageID,
                                       aShortImageID);
        UNICAM_DrvCSI_ConfigureLineSync(csiConfig, aIfcConfig->hdr);
    }

    return ret;
}

/** @brief Configure UNICAM

    This API configures UNICAM sub-component based on configuration type

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_DRV_STATE_INITIALISED or #UNICAM_DRV_STATE_READY
         state while calling this API

    @param[in]  aP_Cfg       UNICAM sub-component configuration
    @param[in]  aIfcConfig   UNICAM common interface configuration
    @param[in]  aBuf         UNICAM buffer configuration

    @retval     #BCM_ERR_OK           UNICAM configuration is successful
    @retval     #BCM_ERR_INVAL_PARAMS Invalid data buffer for short exposure
    @retval     #BCM_ERR_INVAL_PARAMS Invalid image buffer for short exposure
    @retval     #BCM_ERR_INVAL_PARAMS Invalid data buffer for long exposure
    @retval     #BCM_ERR_INVAL_PARAMS Invalid image buffer for long exposure
    @retval     #BCM_ERR_INVAL_PARAMS Invalid burst length
    @retval     #BCM_ERR_INVAL_PARAMS fps or height is 0
    @retval     #BCM_ERR_NOSUPPORT    Unsupported TG mode
    @retval     #BCM_ERR_INVAL_PARAMS hsByteClkTime is zero
    @retval     #BCM_ERR_INVAL_PARAMS Invalid Pixel format for short channel
    @retval     #BCM_ERR_INVAL_PARAMS Invalid pixel format for Long channel
    @retval     #BCM_ERR_INVAL_PARAMS Invalid number of lanes or hs clock frequency
    @retval     #BCM_ERR_INVAL_PARAMS Peripheral mode is neither CSI nor TG
    @retval     #BCM_ERR_INVAL_PARAMS Invalid number of image buffers
    @retval     #BCM_ERR_INVAL_PARAMS Invalid peripheral mode
    @retval     #BCM_ERR_INVAL_PARAMS Invalid magic
    @retval     #BCM_ERR_INVAL_PARAMS Invalid config type
    @retval     #BCM_ERR_INVAL_PARAMS aIfcConfig is NULL
    @retval     #BCM_ERR_INVAL_PARAMS aBuf is NULL
    @retval     #BCM_ERR_INVAL_PARAMS aCfg is NULL
    @retval     #BCM_ERR_INVAL_STATE  State is neither UNICAM_DRV_STATE_INITIALISED nor
                                      UNICAM_DRV_STATE_READY

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC
    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None

    @code{.unparsed}
    ret = UNICAM_DrvPreConfigureCheck(aCfg, aIfcConfig, aBuf, &longBPP, &shortBPP,
                                     &longImageID, &shortImageID)
    if (BCM_ERR_OK != ret
        Report_Error
    else
        UNICAM_DrvInitBufContext()
        bufContext.buf = *aBuf
        if 1UL < bufContext.buf.numImgBufs
            bufContext.toggleImgBuf = 1UL
        if 0UL != aIfcConfig->hdr
            bpp = longBPP > shortBPP ? longBPP : shortBPP
        else
            bpp = longBPP
        UNICAM_DrvMode = aCfg->modeConfig.peripheralMode
        context.numDataLanes = ((aCfg->InterfaceConfig.csiConfig.dataLaneCtl &
                                        UNICAM_CSICFG_DATALANE_NUM_LINES_MASK) >>
                                        UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT)
        if UNICAM_MODE_CSI == aCfg->modeConfig.peripheralMode
            ret = UNICAM_DrvConfigureCSI(aCfg, aIfcConfig, longImageID, shortImageID)
        else
            const UNICAM_TGConfigType *tgConfig = &aCfg->InterfaceConfig.tgConfig
            ret = UNICAM_DrvConfigureTG(tgConfig, aIfcConfig, bpp)
        if BCM_ERR_OK != ret
            Report_Error
        else
            UNICAM_DRV_FIELD_WRITE(camctl, UNICAM_CTL_CPM_SHIFT, UNICAM_CTL_CPM_MASK,
                           (uint32_t) aCfg->modeConfig.peripheralMode)
            UNICAM_DRV_FIELD_WRITE(camipipe, UNICAM_IPIPE_PUM_SHIFT,
                           (UNICAM_IPIPE_DDM_MASK | UNICAM_IPIPE_PUM_MASK),
                           (uint32_t) aCfg->modeConfig.imgPipelineCtl)
            UNICAM_DrvConfigureData(aIfcConfig)
            UNICAM_DrvConfigureFrameSize(aIfcConfig, &aCfg->modeConfig, bpp)
            UNICAM_DrvConfigureHDR(&aCfg->InterfaceConfig.csiConfig,
                                   &aCfg->modeConfig, longBPP, shortBPP)
            UNICAM_DRV_REG(camibls) = aIfcConfig->stride
            UNICAM_DrvConfigurePri()
            UNICAM_DRV_FIELD_WRITE(camctl, UNICAM_CTL_OET_SHIFT,
                               UNICAM_CTL_OET_MASK, 0xffUL)
            UNICAM_DRV_FIELD_WRITE(cam_vc5_atrig, UNICAM_VC5_ATRIG_TRIG_SHIFT,
                       UNICAM_VC5_ATRIG_TRIG_MASK, 0xffUL)
            ret = UNICAM_DrvProgramBuf()
            if BCM_ERR_OK != ret
                Report_Error
            else
                 if 0UL != bufContext.toggleImgBuf
                    context.interruptEnable |= (UNICAM_ICTL_FEIE_MASK)
                    if 0UL != context.isHDR
                        context.interruptEnableSE |=
                                            (UNICAM_ICTL_SE_FEI_SE_EN_MASK)
                else
                    context.interruptEnable &= ~(UNICAM_ICTL_FEIE_MASK)
                    if 0UL != context.isHDR
                        context.interruptEnableSE &=
                                               ~(UNICAM_ICTL_SE_FEI_SE_EN_MASK)
                 UNICAM_DRV_REG(camictl) |= UNICAM_ICTL_IBOB_MASK
                 UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_READY)
    return ret
    @endcode
*/
int32_t UNICAM_DrvConfigure(const UNICAM_ImageType *aCfg,
                                   UNICAM_CommonIfcConfigType *aIfcConfig,
                                   UNICAM_BufParamType *aBuf)
{
    int32_t ret;
    uint8_t longBPP = 0U, shortBPP = 0U;
    uint8_t longImageID = 0U, shortImageID = 0U;
    uint8_t bpp = 0U;

    /* Perform pre-configure check */
    ret = UNICAM_DrvPreConfigureCheck(aCfg, aIfcConfig, aBuf, &longBPP, &shortBPP,
                                     &longImageID, &shortImageID);
    if (BCM_ERR_OK != ret) {
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_PROC,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    } else {
        UNICAM_DrvInitBufContext();
        UNICAM_DrvBufContext.buf = *aBuf;
        if (1UL < UNICAM_DrvBufContext.buf.numImgBufs) {
            UNICAM_DrvBufContext.toggleImgBuf = 1UL;
        }
        if (0UL != aIfcConfig->hdr) {
            bpp = longBPP > shortBPP ? longBPP : shortBPP;
        } else {
            bpp = longBPP;
        }
        UNICAM_DrvMode = aCfg->modeConfig.peripheralMode;
        UNICAM_DrvContext.numDataLanes = ((aCfg->InterfaceConfig.csiConfig.dataLaneCtl &
                                        UNICAM_CSICFG_DATALANE_NUM_LINES_MASK) >>
                                        UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT);
        /* Program registers here */
        /* Mode specific configuration */
        if (UNICAM_MODE_CSI == aCfg->modeConfig.peripheralMode) {
            ret = UNICAM_DrvConfigureCSI(aCfg, aIfcConfig, longImageID, shortImageID);
        } else {
            const UNICAM_TGConfigType *tgConfig = &aCfg->InterfaceConfig.tgConfig;
            /* TG */
            ret = UNICAM_DrvConfigureTG(tgConfig, aIfcConfig, bpp);
        }
        if (BCM_ERR_OK != ret) {
            UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        } else {
            /* Configure Mode */
            UNICAM_DRV_FIELD_WRITE(camctl, UNICAM_CTL_CPM_SHIFT, UNICAM_CTL_CPM_MASK,
                           (uint32_t) aCfg->modeConfig.peripheralMode);
            /* Configure image pipeline */
            UNICAM_DRV_FIELD_WRITE(camipipe, UNICAM_IPIPE_PUM_SHIFT,
                           (UNICAM_IPIPE_DDM_MASK | UNICAM_IPIPE_PUM_MASK),
                           (uint32_t) aCfg->modeConfig.imgPipelineCtl);
            /* Configure data */
            UNICAM_DrvConfigureData(aIfcConfig);
            /* Configure frame size */
            UNICAM_DrvConfigureFrameSize(aIfcConfig, &aCfg->modeConfig, bpp);
            /* Configure HDR */
            UNICAM_DrvConfigureHDR(aIfcConfig, &aCfg->InterfaceConfig.csiConfig,
                                   &aCfg->modeConfig, longBPP, shortBPP);
            /* Configure stride */
            UNICAM_DRV_REG(camibls) = aIfcConfig->stride;
            /* Configure Priority Control */
            UNICAM_DrvConfigurePri();
            /* Program OET */
            UNICAM_DRV_FIELD_WRITE(camctl, UNICAM_CTL_OET_SHIFT,
                               UNICAM_CTL_OET_MASK, 0xffUL);
            /* Load TG configuration */
            UNICAM_DRV_FIELD_WRITE(cam_vc5_atrig, UNICAM_VC5_ATRIG_TRIG_SHIFT,
                               UNICAM_VC5_ATRIG_TRIG_MASK, 0xffUL);
            ret = UNICAM_DrvProgramBuf();
            if (BCM_ERR_OK != ret) {
                UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_CONFIGURE_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            } else {
                /* Enable FE Interrupt for DualPass(FRAME mode) */
                if (0UL != UNICAM_DrvBufContext.toggleImgBuf) {
                    UNICAM_DrvContext.interruptEnable |= (UNICAM_ICTL_FEIE_MASK);
                    if (0UL != UNICAM_DrvContext.isHDR) {
                        UNICAM_DrvContext.interruptEnableSE |=
                                            (UNICAM_ICTL_SE_FEI_SE_EN_MASK);
                    }
                } else {
                    UNICAM_DrvContext.interruptEnable &=
                                                  ~(UNICAM_ICTL_FEIE_MASK);
                    if (0UL != UNICAM_DrvContext.isHDR) {
                        UNICAM_DrvContext.interruptEnableSE &=
                                              ~(UNICAM_ICTL_SE_FEI_SE_EN_MASK);
                    }
                }
                /* Enable Wrap-around */
                UNICAM_DRV_REG(camictl) |= UNICAM_ICTL_IBOB_MASK;
                UNICAM_DrvSetState(0UL, UNICAM_DRV_STATE_READY);
            }
        }
    }
    return ret;
}

/** @brief Get outstanding events in UNICAM driver

    This API retrieves outstanding events in UNICAM driver

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  void

    @retval uint32_t Mask of event's occurred

    @post Events are cleared

    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EVENT_PROC
    @trace #BRCM_SWREQ_UNICAM

    @limitations None

    @code{.unparsed}
    UNICAM_DrvContext.event =  0UL
    if 0UL < UNICAM_DrvContext.numContErrors
        Report_Error
    return ret
    @endcode
*/
uint32_t UNICAM_DrvGetEvent(void)
{
    uint32_t ret = 0UL;

    UNICAM_DrvContext.event = 0UL;

    if (0UL < UNICAM_DrvContext.numContErrors) {
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_GET_EVENT_PROC,
                              BCM_ERR_UNKNOWN,
                              (uint32_t) __LINE__,
                              UNICAM_DrvContext.status,
                              (UNICAM_DrvContext.dataLaneStatus[0] |
                               UNICAM_DrvContext.dataLaneStatus[1] |
                               UNICAM_DrvContext.dataLaneStatus[2] |
                               UNICAM_DrvContext.dataLaneStatus[3]),
                              UNICAM_DrvContext.clockLaneStatus);
    }

    return ret;
}

/** @brief Retrieves latest filled buffer's index

    This API retrieves latest filled buffer's index.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  void

    @retval uint32_t Current buffer index

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_GET_CUR_BUF_IDX_PROC
    @trace #BRCM_SWREQ_UNICAM

    @limitations None

    @code{.unparsed}
    return bufContext.filledBufIdx;
    @endcode
*/
uint32_t UNICAM_DrvGetCurBufIdx(void)
{
    return UNICAM_DrvBufContext.filledBufIdx;
}

/** @brief Retrieve latest ED buffer

    This API retrieves and copies the latest embedded data buffer to the supplied
    client buffer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aEDParam Pointer to the ED buffer

    @retval     #BCM_ERR_OK             ED buffer retrieval is successful
    @retval     #BCM_ERR_EAGAIN         ED buffer got updated, try again
    @retval     #BCM_ERR_NOT_FOUND      No buffer available in driver
    @retval     #BCM_ERR_INVAL_PARAMS   aEDParam is NULL
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUFFER_PROC
    @trace #BRCM_SWREQ_UNICAM

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK

    if UNICAM_DRV_STATE_POWEROFF == UNICAM_DrvContext.state
        ret = BCM_ERR_INVAL_STATE
        Report_Error
    else
        if NULL == aEDParam
            ret = BCM_ERR_INVAL_PARAMS
            Report_Error
        else
            expIdx = aEDParam->expIdx
            if 0UL != aEDParam->beforeFrame
                edIntern = &UNICAM_DrvBufContext.curED[expIdx]
            else
                edIntern = &UNICAM_DrvBufContext.curEDEnd[expIdx]
            if NULL == aEDParam->ed.ptr
                ret = BCM_ERR_INVAL_PARAMS
                Report_Error
            else if 0UL == edIntern->ed.addr
                ret = BCM_ERR_NOT_FOUND
            else
                for (i = 0UL i < 2UL i++
                    uint32_t addr = edIntern->ed.addr
                    uint32_t size = aEDParam->size

                    DCACHE_SysInvalRange((uint32_t)
                        (edIntern->ed.addr + aEDParam->offset), size)
                    BCM_MemCpy(aEDParam->ed.ptr,
                              (edIntern->ed.ptr + aEDParam->offset),
                              size)
                    if 0UL == aEDParam->beforeFrame
                        aEDParam->frameNum = edIntern->frameNum
                    else
                        aEDParam->frameNum = 0UL
                    if addr == edIntern->ed.addr
                        break
                    else
                        ret = BCM_ERR_EAGAIN
                        Report_Error
    return ret
    @endcode
*/
int32_t UNICAM_DrvGetEmbBuffer(UNICAM_CurED_ParamType *aEDParam)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i;
    uint32_t expIdx;
    UNICAM_CurED_ParamType *edIntern;

    if (UNICAM_DRV_STATE_POWEROFF == UNICAM_DrvContext.state) {
        ret = BCM_ERR_INVAL_STATE;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUFFER_PROC,
                ret, 0UL, 0UL, 0UL, 0UL);
    } else {
        if (NULL == aEDParam) {
            ret = BCM_ERR_INVAL_PARAMS;
            UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUFFER_PROC,
                    ret, __LINE__, 0UL, 0UL, 0UL);
        } else {
            expIdx = aEDParam->expIdx;
            if (0UL != aEDParam->beforeFrame) {
                /* DCS */
                edIntern = &UNICAM_DrvBufContext.curED[expIdx];
            } else {
                /* DCS_END */
                edIntern = &UNICAM_DrvBufContext.curEDEnd[expIdx];
            }
            if (NULL == aEDParam->ed.ptr) {
                ret = BCM_ERR_INVAL_PARAMS;
                UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUFFER_PROC,
                                 ret,
                                 (uint32_t) __LINE__,
                                 aEDParam->ed.addr,
                                 0UL,
                                 0UL);
            } else if (0UL == edIntern->ed.addr) {
                ret = BCM_ERR_NOT_FOUND;
            } else {
                for (i = 0UL; i < 2UL; i++) {
                    uint32_t addr = edIntern->ed.addr;
                    uint32_t size = aEDParam->size;

                    /*DCACHE_SysInvalRange((uint32_t)
                        (edIntern->ed.addr + aEDParam->offset), size);*/
                    BCM_MemCpy(aEDParam->ed.ptr,
                              (edIntern->ed.ptr + aEDParam->offset),
                              size);
                    if (0UL == aEDParam->beforeFrame) {
                        aEDParam->frameNum = edIntern->frameNum;
                    } else {
                        aEDParam->frameNum = 0UL;
                    }
                    if (addr == edIntern->ed.addr) {
                        break;
                    } else {
                        ret = BCM_ERR_EAGAIN;
                        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUFFER_PROC,
                                        ret, 0UL, 0UL, 0UL, 0UL);
                    }
                }
            }
        }
    }

    return ret;
}

/** @brief Retrieve latest ED buffer pointer

    This API retrieves latest available embedded data buffer pointers.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aEDParam Pointer to current embedded buffer pointers

    @retval     #BCM_ERR_OK             ED buffer retrieval is successful
    @retval     #BCM_ERR_EAGAIN         ED buffer got updated, try again
    @retval     #BCM_ERR_NOT_FOUND      No buffer available in driver
    @retval     #BCM_ERR_INVAL_PARAMS   aEDParam is NULL
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUF_PTR_PROC
    @trace #BRCM_SWREQ_UNICAM

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK

    if UNICAM_DRV_STATE_POWEROFF == UNICAM_DrvContext.state
        ret = BCM_ERR_INVAL_STATE
        Report_Error
    else
        if NULL == aEDParam
            ret = BCM_ERR_INVAL_PARAMS
            Report_Error
        else
            expIdx =aEDParam->expIdx
            if 0UL == UNICAM_DrvBufContext.curED[expIdx].ed.addr
                ret = BCM_ERR_NOT_FOUND
            else
                for i = 0UL i < 2UL i++
                    uint32_t addr =
                            (uint32_t) UNICAM_DrvBufContext.curED[expIdx].ed.addr

                    aEDParam->ed.addr = addr
                    DCACHE_SysInvalRange(
                              (uint32_t)UNICAM_DrvBufContext.curED[expIdx].ed.addr,
                               aEDParam->size)
                    if addr == aEDParam->ed.addr
                        break
                    else
                        ret = BCM_ERR_EAGAIN
                        Report_Error
    return ret
    @endcode
*/
int32_t UNICAM_DrvGetEmbBufPtr(UNICAM_CurED_ParamType *aEDParam)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i;
    uint32_t expIdx;

    if (UNICAM_DRV_STATE_POWEROFF == UNICAM_DrvContext.state) {
        ret = BCM_ERR_INVAL_STATE;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUF_PTR_PROC,
                ret, 0UL, 0UL, 0UL, 0UL);
    } else {
        if (NULL == aEDParam) {
            ret = BCM_ERR_INVAL_PARAMS;
            UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUF_PTR_PROC,
                    ret, __LINE__, 0UL, 0UL, 0UL);
        } else {
            expIdx =aEDParam->expIdx;
            if (0UL == UNICAM_DrvBufContext.curED[expIdx].ed.addr) {
                ret = BCM_ERR_NOT_FOUND;
            } else {
                for (i = 0UL; i < 2UL; i++) {
                    uint32_t addr = (uint32_t) UNICAM_DrvBufContext.curED[expIdx].ed.addr;

                    aEDParam->ed.addr = addr;
                    /*DCACHE_SysInvalRange((uint32_t)UNICAM_DrvBufContext.curED[expIdx].ed.addr,
                                                     aEDParam->size);*/
                    if (addr == aEDParam->ed.addr) {
                        break;
                    } else {
                        ret = BCM_ERR_EAGAIN;
                        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_GET_EMB_BUF_PTR_PROC,
                                        ret, 0UL, 0UL, 0UL, 0UL);
                    }
                }
            }
        }
    }

    return ret;
}

/** @brief UNICAM IRQ Handler

    IRQ handler for UNICAM control/data interrupts.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre None

    @param  void

    @param  void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None
    unicamStatus = UNICAM_DrvProcessInterrupt();
    if (0UL != (unicamStatus & UNICAM_EVENT_MASK_CTRL)) {
        ret = BCM_SetEvent(UNICAM_TASK, UNICAM_EVENT);
    }
    if(0UL != (unicamStatus & UNICAM_EVENT_MASK_DATA_SOF)) {
        ret = BCM_SetEvent(UNICAM_TUNER_TASK, UNICAM_META_EVENT);
    }
    if(0UL != (unicamStatus & UNICAM_EVENT_MASK_DATA_EOF)) {
        ret = BCM_SetEvent(UNICAM_TUNER_TASK, UNICAM_STATS_EVENT);
    }
    @endcode
*/
void UNICAM_DrvIRQHandler(void)
{
    uint32_t unicamStatus;

    /* Process Interrupt */
    unicamStatus = UNICAM_DrvProcessInterrupt();

    if (0UL != (unicamStatus & UNICAM_EVENT_MASK_CTRL)) {
    }
#ifndef UNICAM_DISABLE_TUNERS
    if(0UL != (unicamStatus & UNICAM_EVENT_MASK_DATA_SOF)) {
    }

    if(0UL != (unicamStatus & UNICAM_EVENT_MASK_DATA_EOF)) {
    }
#endif /* UNICAM_DISABLE_TUNERS */
}

/** @brief UNICAM Error IRQ Handler

    IRQ handler for UNICAM error interrupts.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre None

    @param  void

    @param  void

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_IRQ_ERR_HANDLER_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None
    @code{.c}
    unicamStatus = UNICAM_DrvProcessErrorInterrupt();
    if (0UL != (unicamStatus & UNICAM_EVENT_MASK_CTRL)) {
        ret = BCM_SetEvent(UNICAM_TASK, UNICAM_EVENT);
    }
    @endcode
*/
void UNICAM_DrvIRQErrHandler(void)
{
    uint32_t unicamStatus;

    /* Process Interrupt */
    unicamStatus = UNICAM_DrvProcessErrorInterrupt();

    /* Signal UNICAM Driver Client */
    if (0UL != (unicamStatus & UNICAM_EVENT_MASK_CTRL)) {
    }
}

/** @} */
/** @brief Update source frame number
    @behavior Sync, Non-reentrant

    @pre UNICAM is enabled

    @param[in]      aFrameNum       Frame number

    Return values are documented in reverse-chronological order

    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      SVC failed
    @retval     #BCM_ERR_INVAL_STATE  UNICAM is in invalid state for this API

    @post None

    @trace #BRCM_SWARCH_UNICAM_DRV_UPDATE_FRAME_NUM_PROC
    @trace #BRCM_SWREQ_UNICAM

    @limitations None

    @code{.unparsed}
    ret = BCM_ERR_OK

    if UNICAM_DRV_STATE_ENABLED != UNICAM_DrvContext.state
        ret = BCM_ERR_INVAL_STATE
        Report_Error
    else
        UNICAM_DrvContext.curFrameNum = aFrameNum

    return ret;
    @endcode
*/
int32_t UNICAM_DrvUpdateFrameNum(uint32_t aFrameNum)
{
    int32_t ret = BCM_ERR_OK;

    /* Check state */
    if (UNICAM_DRV_STATE_ENABLED != UNICAM_DrvContext.state) {
        ret = BCM_ERR_INVAL_STATE;
        UNICAM_DrvReportError(BRCM_SWDSGN_UNICAM_DRV_UPDATE_FRAME_NUM_PROC,
                ret,
                (uint32_t) (__LINE__),
                (uint32_t) UNICAM_DrvContext.state, 0UL, 0UL);
    } else {
        UNICAM_DrvContext.curFrameNum = aFrameNum;
    }

    return ret;
}

/** @} */

