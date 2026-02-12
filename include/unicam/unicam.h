/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.
 Broadcom Proprietary and Confidential.
******************************************************************************/

/**
    @defgroup grp_unicam_ifc Interface
    @ingroup grp_unicamdrv

    @addtogroup grp_unicam_ifc
    @{
    @section sec_unicam_overview Overview
    -# Unicam Bare Metal driver interface provides layer 1 APIs to initialize configure
    the MIPI CSI to desired configuration. It is configured in non-blocking mode and APIs provided are non-reenterant.
    -# It is user's responsibility to update interrupt handlers for data transfers.
    @note Please refer to respective technical reference manual for more details about the HW features.


    @section api_list List of unicam APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref UNICAM_DrvInit                   | Unicam driver initialization |
    | @ref UNICAM_DrvDeinit                 | De-initialize Unicam|
    | @ref UNICAM_DrvStart                  | Enables Unicam |
    | @ref UNICAM_DrvStop                   | Disabled Unicam |
    | @ref UNICAM_DrvConfigure              | Configure Unicam |
    | @ref UNICAM_DrvGetEvent               | Get outstanding events |
    | @ref UNICAM_DrvGetCurBufIdx           | Retrieves latest filled buffer's index |
    | @ref UNICAM_DrvGetEmbBuffer           | Retrieve latest ED buffer |
    | @ref UNICAM_DrvGetEmbBufPtr           | Retrieve latest ED buffer pointer |
    | @ref UNICAM_DrvUpdateFrameNum         | Update source Frame number|
    | @ref UNICAM_DrvProcessInterrupt       | Sample interrupt API that can be defined in vector table |
    | @ref UNICAM_DrvProcessErrorInterrupt  | Sample UNICAM Error Interrupt Handler that can be defined in vector table |

    
    @file unicam.h
    @brief UNICAM driver interface
    This header file contains the interface functions for UNICAM
    @version 0.1 Initial Version
*/



#ifndef UNICAM_H
#define UNICAM_H

#include <stdint.h>

/**
    @name UNICAM interface IDs
    @{
    @brief Interface IDs for UNICAM
*/
#define BRCM_SWARCH_UNICAM_EVENT_MASK_MACRO              (0x8501U)    /**< @brief #UNICAM_EVENT_MASK_CTRL                */
#define BRCM_SWARCH_UNICAM_INT_MACRO                     (0x8502U)    /**< @brief #UNICAM_INT_ERROR                      */
#define BRCM_SWARCH_UNICAM_MAX_NUM_IMG_BUFS_MACRO        (0x8504U)    /**< @brief #UNICAM_MAX_NUM_IMG_BUFS               */
#define BRCM_SWARCH_UNICAM_MAX_NUM_DATA_BUFS_MACRO       (0x8505U)    /**< @brief #UNICAM_MAX_NUM_DATA_BUFS              */
#define BRCM_SWARCH_UNICAM_MAGIC_MACRO                   (0x8506U)    /**< @brief #UNICAM_MAGIC                          */
#define BRCM_SWARCH_UNICAM_MAX_CUSTOM_IMAGE_IDS_MACRO    (0x8507U)    /**< @brief #UNICAM_MAX_CUSTOM_IMAGE_IDS           */
#define BRCM_SWARCH_UNICAM_MODECFG_META_LINES_MACRO      (0x8508U)    /**< @brief #UNICAM_MODECFG_META_LINES_START_MASK  */
#define BRCM_SWARCH_UNICAM_CSICFG_DATALANE_MACRO         (0x8509U)    /**< @brief #UNICAM_CSICFG_DATALANE_NUM_LINES_MASK */
#define BRCM_SWARCH_UNICAM_MODE_TYPE                     (0x850AU)    /**< @brief #UNICAM_ModeType                       */
#define BRCM_SWARCH_UNICAM_OUTPUT_IDX_TYPE               (0x850BU)    /**< @brief #UNICAM_OutputIdxType                  */
#define BRCM_SWARCH_UNICAM_FORMAT_TYPE                   (0x850CU)    /**< @brief #UNICAM_FormatType                     */
#define BRCM_SWARCH_UNICAM_COMMON_IFC_CONFIG_TYPE        (0x850DU)    /**< @brief #UNICAM_CommonIfcConfigType            */
#define BRCM_SWARCH_UNICAM_MODE_CONFIG_TYPE              (0x850EU)    /**< @brief #UNICAM_ModeConfigType                 */
#define BRCM_SWARCH_UNICAM_CSI_CONFIG_TYPE               (0x850FU)    /**< @brief #UNICAM_CSIConfigType                  */
#define BRCM_SWARCH_UNICAM_TG_CONFIG_TYPE                (0x8510U)    /**< @brief #UNICAM_TGConfigType                   */
#define BRCM_SWARCH_UNICAM_IMAGE_TYPE                    (0x8511U)    /**< @brief #UNICAM_ImageType                      */
#define BRCM_SWARCH_UNICAM_BUFFER_TYPE                   (0x8512U)    /**< @brief #UNICAM_BufferType                     */
#define BRCM_SWARCH_UNICAM_BUF_PARAM_TYPE                (0x8513U)    /**< @brief #UNICAM_BufParamType                   */
#define BRCM_SWARCH_UNICAM_CUR_ED_PARAM_TYPE             (0x8514U)    /**< @brief #UNICAM_CurED_ParamType                */
#define BRCM_SWARCH_UNICAM_IO_TYPE                       (0x8515U)    /**< @brief #UNICAM_IOType                         */
#define BRCM_SWARCH_UNICAM_DRV_PROCESS_INT_PROC          (0x8516U)    /**< @brief #UNICAM_DrvProcessInterrupt            */
#define BRCM_SWARCH_UNICAM_DRV_PROCESS_ERR_INT_PROC      (0x8517U)    /**< @brief #UNICAM_DrvProcessErrorInterrupt       */
#define BRCM_SWARCH_UNICAM_DEFAULT_ALLOC_LINES_MACRO     (0x8518U)    /**< @brief #UNICAM_DEFAULT_ALLOC_LINES */
#define BRCM_SWARCH_UNICAM_IMG_HDR_TYPE                  (0x8519U)    /**< @brief #UNICAM_ImgHdrType */
#define BRCM_SWARCH_UNICAM_DRV_IRQ_HANDLER_PROC          (0x851AU)    /**< @brief #UNICAM_DrvIRQHandler                  */
#define BRCM_SWARCH_UNICAM_DRV_IRQ_ERR_HANDLER_PROC      (0x851BU)    /**< @brief #UNICAM_DrvIRQErrHandler               */
#define BRCM_SWARCH_UNICAM_DRV_INIT_PROC                 (0x851CU)    /**< @brief #UNICAM_DrvInit                        */
#define BRCM_SWARCH_UNICAM_DRV_DEINIT_PROC               (0x851DU)    /**< @brief #UNICAM_DrvDeinit                      */
#define BRCM_SWARCH_UNICAM_DRV_START_PROC                (0x851EU)    /**< @brief #UNICAM_DrvStart                       */
#define BRCM_SWARCH_UNICAM_DRV_STOP_PROC                 (0x851FU)    /**< @brief #UNICAM_DrvStop                        */
#define BRCM_SWARCH_UNICAM_DRV_CONFIGURE_PROC            (0x8520U)    /**< @brief #UNICAM_DrvConfigure                   */
#define BRCM_SWARCH_UNICAM_DRV_GET_EVENT_PROC            (0x8521U)    /**< @brief #UNICAM_DrvGetEvent                    */
#define BRCM_SWARCH_UNICAM_DRV_GET_CUR_BUF_IDX_PROC      (0x8522U)    /**< @brief #UNICAM_DrvGetCurBufIdx                */
#define BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUFFER_PROC       (0x8523U)    /**< @brief #UNICAM_DrvGetEmbBuffer                */
#define BRCM_SWARCH_UNICAM_DRV_GET_EMB_BUF_PTR_PROC      (0x8524U)    /**< @brief #UNICAM_DrvGetEmbBufPtr                */
#define BRCM_SWARCH_UNICAM_DRV_UPDATE_FRAME_NUM_PROC     (0x8525U)    /**< @brief #UNICAM_DrvUpdateFrameNum              */
#define BRCM_SWARCH_UNICAM_IMG_CONFIG_MODE               (0x8526U)    /**< @brief #UNICAM_IMG_CONFIG_MODE             */


/** @} */

/**
    @name UNICAM event masks
    @{
    @brief Macros for UNICAM event masks

    @trace #BRCM_SWREQ_UNICAM
*/
#define UNICAM_EVENT_MASK_CTRL      (0x1UL)     /**< @brief Macro that notifies UNICAM control event                */
#define UNICAM_EVENT_MASK_DATA_SOF  (0x2UL)     /**< @brief Macro that notifies SOF embedded data event from UNICAM */
#define UNICAM_EVENT_MASK_DATA_EOF  (0x4UL)     /**< @brief Macro that notifies EOF embedded data event from UNICAM */
/** @} */

/**
    @name UNICAM interrupt mask type
    @{
    @brief Interrupt mask type for UNICAM

    UNICAM interrupt mask type.

    @trace  #BRCM_SWREQ_UNICAM
*/
#define UNICAM_INT_ERROR    (0x10UL)  /**< @brief Error interrupt       */
#define UNICAM_INT_FS       (0x20UL)  /**< @brief Frame Start interrupt */
#define UNICAM_INT_FE       (0x40UL)  /**< @brief Frame End interrupt   */
#define UNICAM_INT_DI       (0x80UL)  /**< @brief Data interrupt        */
#define UNICAM_INT_DI_END   (0x100UL) /**< @brief Data End interrupt    */
/** @} */

/**
    @brief Default lines to be allocated

    Default number of lines to be allocated for UNICAM image

    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
*/
#define UNICAM_DEFAULT_ALLOC_LINES (5UL)


/**
    @name Image header Configuration type
    @{
    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_IMG_CONFIG_MODE     (1UL) /**< @brief Mode Configuration   */
#define UNICAM_IMG_CONFIG_ISP      (2UL) /**< @brief ISP Configuration    */
#define UNICAM_IMG_CONFIG_UNICAM   (3UL) /**< @brief UNICAM Configuration */
#define UNICAM_IMG_CONFIG_RPR      (4UL) /**< @brief RPR Configuration    */
/** @} */

/**
    @brief Maximum number of UNICAM image buffers per exposure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MAX_NUM_IMG_BUFS  (3UL)

/**
    @brief Maximum number of UNICAM data buffers per exposure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MAX_NUM_DATA_BUFS (2UL)

/**
    @brief Magic for UNICAM configuration

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MAGIC         (0x554e4943UL)

/**
    @brief Maximum number of custom Image IDs for UNICAM

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MAX_CUSTOM_IMAGE_IDS (4UL)

/** @name Mode configuration meta-lines custom mask
    @{
    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MODECFG_META_LINES_START_MASK  (0x0fU) /**< @brief Mask for meta-lines before frame  */
#define UNICAM_MODECFG_META_LINES_START_SHIFT (0U)    /**< @brief Shift for meta-lines before frame */
#define UNICAM_MODECFG_META_LINES_END_MASK    (0xf0U) /**< @brief Mask for meta-lines after frame   */
#define UNICAM_MODECFG_META_LINES_END_SHIFT   (4U)    /**< @brief Shift for meta-lines after frame  */
/** @} */

/** @name  CSI configuration data-lane custom masks
    @{

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_CSICFG_DATALANE_NUM_LINES_MASK  (0x07U) /**< @brief Mask for number of lines  */
#define UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT (0U)    /**< @brief Shift for number of lines */
#define UNICAM_CSICFG_DATALANE_DLSMN_MASK      (0x08U) /**< @brief Mask for DLSMN            */
#define UNICAM_CSICFG_DATALANE_DLSMN_SHIFT     (3U)    /**< @brief Shift for DLSMN           */
#define UNICAM_CSICFG_DATALANE_DLTREN_MASK     (0x10U) /**< @brief Mask for DLTREN           */
#define UNICAM_CSICFG_DATALANE_DLTREN_SHIFT    (4U)    /**< @brief Shift for DLTREN          */
/** @} */

/**
    @name UNICAM Peripheral mode types
    @{
    @brief UNICAM Peripheral mode types

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef uint8_t UNICAM_ModeType;   /**< @brief UNICAM_ModeType typedef   */
#define UNICAM_MODE_CPI    (0U)    /**< @brief CPI Mode                  */
#define UNICAM_MODE_CSI    (1U)    /**< @brief CSI Mode                  */
#define UNICAM_MODE_TG     (2U)    /**< @brief TG Mode                   */
#define UNICAM_MODE_HISPI  (3U)    /**< @brief HISPI Mode                */
#define UNICAM_MODE_MAX    (4U)    /**< @brief Number of supported modes */
/** @} */

/**
    @name UNICAM output index
    @{
    @brief Output index for UNICAM output

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef uint32_t UNICAM_OutputIdxType;
#define UNICAM_OUTPUT_IDX_LONG  (0UL)  /**< @brief Long exposure              */
#define UNICAM_OUTPUT_IDX_SHORT (1UL)  /**< @brief Short exposure             */
#define UNICAM_OUTPUT_IDX_MAX   (2UL)  /**< @brief Maximum number of exposure */
/** @} */

/** @name UNICAM_RAWFormatType
    @{
    @brief UNICAM Image format

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef uint32_t UNICAM_FormatType; /**< @brief UNICAM Input image format type */
#define UNICAM_FORMAT_RAW6  (1U)  /**< @brief RAW6  */
#define UNICAM_FORMAT_RAW7  (2U)  /**< @brief RAW7  */
#define UNICAM_FORMAT_RAW8  (3U)  /**< @brief RAW8  */
#define UNICAM_FORMAT_RAW10 (4U)  /**< @brief RAW10 */
#define UNICAM_FORMAT_RAW12 (5U)  /**< @brief RAW12 */
#define UNICAM_FORMAT_RAW14 (6U)  /**< @brief RAW14 */
#define UNICAM_FORMAT_RAW16 (7U)  /**< @brief RAW16 */
/** @} */

/**
    @brief UNICAM common interface configuration structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct sUNICAM_CommonIfcConfigType {
    uint8_t hdr;                              /**< @brief Is HDR enabled      */
    uint8_t fps;                              /**< @brief FPS of the stream   */
    uint8_t metaLines[UNICAM_OUTPUT_IDX_MAX]; /**< @brief Metadata lines info */
    uint16_t shortLinesOffset;                /**< @brief ShortLine offset    */
    uint16_t width;                           /**< @brief Width of the image  */
    uint16_t height;                          /**< @brief Height of the image */
    uint16_t stride;                          /**< @brief Stride of the image */
} UNICAM_CommonIfcConfigType;

/**
    @brief UNICAM mode configuration structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct sUNICAM_ModeConfigType {
    uint8_t peripheralMode;                /**< @brief Peripheral mode        */
    uint8_t imgPipelineCtl;                /**< @brief Pipeline control       */
    uint8_t format[UNICAM_OUTPUT_IDX_MAX]; /**< @brief Format of the exposure */
} UNICAM_ModeConfigType;

/**
    @brief UNICAM CSI configuration structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct sUNICAM_CSIConfigType {
    uint8_t tclkTermEnable;                           /**< @brief Clock term enable        */
    uint8_t tclkSettle;                               /**< @brief Clock settle time        */
    uint8_t tdTermEnable;                             /**< @brief Data term enable         */
    uint8_t thsSettle;                                /**< @brief HS settle time           */
    uint8_t trxEnable;                                /**< @brief TRX enable               */
    uint8_t dataLaneCtl;                              /**< @brief Data lane control params */
    uint8_t passLineSync;                             /**< @brief Pass line sync           */
    uint8_t packetFrameTimeout;                       /**< @brief Packet frame timeout     */
    uint8_t vcNum[UNICAM_OUTPUT_IDX_MAX];             /**< @brief VC num of exposures      */
    uint8_t pad[2];                                   /**< @brief Padding                  */
    uint32_t hsClkFrequency;                          /**< @brief HS clock frequency       */
    uint8_t custImageIds[UNICAM_MAX_CUSTOM_IMAGE_IDS];/**< @brief Custom IDs               */
} UNICAM_CSIConfigType;

/**
    @brief UNICAM TG configuration structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct sUNICAM_TGConfigType {
    uint8_t blkSize;           /**< @brief TG block size             */
    uint8_t axiMinBurstLength; /**< @brief Minimum AXI burst length  */
    uint8_t axiMaxBurstLength; /**< @brief Maximum AXI burst length */
    uint8_t pad;               /**< @brief Padding                   */
    uint32_t tgCtl;            /**< @brief TG control parameters     */
    uint32_t clkFrequency;     /**< @brief TG clock frequency        */
} UNICAM_TGConfigType;

/**
    @brief UNICAM image header

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef struct sUNICAM_ImgHdrType {
    uint32_t cfgType;   /**< @brief Image UID UNICAM_IMG* */
    uint32_t cfgMagic;  /**< @brief Magic for component use */
} UNICAM_ImgHdrType;


/**
    @brief UNICAM Configuration image structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct sUNICAM_ImageType {
    UNICAM_ImgHdrType imgHdr;       /**< @brief image header as #UNICAM_ImgHdrType */
    UNICAM_ModeConfigType modeConfig;   /**< @brief mode as #UNICAM_ModeType               */
    union {
        UNICAM_CSIConfigType csiConfig; /**< @brief CSI configuration  */
        UNICAM_TGConfigType  tgConfig;  /**< @brief TG configuration   */
    } InterfaceConfig;
} UNICAM_ImageType;

/**
    @brief UNICAM buffer Type

    Structure for UNICAM buffer

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct sUNICAM_BufferType {
    uint32_t startAddr;   /**< @brief Start address */
    uint32_t endAddr;     /**< @brief End address   */
} UNICAM_BufferType;

/**
    @brief UNICAM buffer parameter

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
  */
typedef struct sUNICAM_BufParamType {
    UNICAM_BufferType imgBuf[UNICAM_MAX_NUM_IMG_BUFS][UNICAM_OUTPUT_IDX_MAX];   /**< @brief Image buffer                        */
    UNICAM_BufferType dataBuf[UNICAM_MAX_NUM_DATA_BUFS][UNICAM_OUTPUT_IDX_MAX]; /**< @brief Data buffer                         */
    uint32_t numImgBufs;                                                        /**< @brief Number of image buffer per exposure */
    uint32_t numExposures;                                                      /**< @brief Number of exposures                 */
} UNICAM_BufParamType;

/**
    @brief Current ED parameter

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
  */
typedef struct sUNICAM_CurED_ParamType {
    union {
        uint8_t *ptr;
        uint32_t addr;
    } ed;                 /**< @brief Embedded data pointer      */
    uint32_t expIdx;      /**< @brief Exposure index             */
    uint32_t beforeFrame; /**< @brief Is the ED before frame?    */
    uint32_t offset;      /**< @brief offset for the data        */
    uint32_t size;        /**< @brief Size of the data           */
    uint32_t frameNum;    /**< @brief Frame number for STATS     */
} UNICAM_CurED_ParamType;

/**
    @brief UNICAM IO structure

    IO structure to implement SVC handler for UNICAM

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef struct sUNICAM_IOType {
    int32_t status;                /**< @brief status to be returned by server */
    const UNICAM_ImageType *cfg;   /**< @brief cfg as #UNICAM_ImageType        */
    uint32_t event;                /**< @brief event to be returned by server  */
    uint32_t bufIdx;               /**< @brief bufIdx to be returned by server */
    UNICAM_BufParamType *buf;      /**< @brief Buffer parameters to be set     */
    UNICAM_CurED_ParamType *curED; /**< @brief Embedded buffer to be set by
                                                                       server  */
    UNICAM_CommonIfcConfigType* ifcCfg;
    uint32_t *frameNum;            /**< @brief Frame number                    */
} UNICAM_IOType;

/** @brief Process the UNICAM control interrupts

    This interface processes the UNICAM control interrupts and returns the
    notification for client.

    @behavior Sync, Re-entrant

    @pre None

    @param  void

    @return     Following possible bit-masks are returned:
                - #UNICAM_EVENT_MASK_CTRL
                - #UNICAM_EVENT_MASK_DATA_SOF
                - #UNICAM_EVENT_MASK_DATA_EOF

    @post None

    @note This interface shall be called from UNICAM interrupt service routines

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

    @snippet unicam_osil_base.c Usage of UNICAM_DrvProcessInterrupt
*/
extern uint32_t UNICAM_DrvProcessInterrupt(void);

/** @brief Process the UNICAM error interrupts

    This interface processes the UNICAM error interrupts and returns the
    notification for client.

    @behavior Sync, Re-entrant

    @pre None

    @param  void

    @return     Following possible bit-masks are returned:
                - #UNICAM_EVENT_MASK_CTRL

    @post None

    @note This interface shall be called from UNICAM interrupt service routines

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

    @snippet unicam_osil_base.c Usage of UNICAM_DrvProcessErrorInterrupt
*/
extern uint32_t UNICAM_DrvProcessErrorInterrupt(void);

/** @brief UNICAM Interrupt Handler to be installed in Vector table

    This interface calls @ref UNICAM_DrvProcessInterrupt and signal CAMERA_TASK.

    @behavior Sync, Non-reentrant

    @pre To be executed in Interrupt context

    @param      void

    @retval     void

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

*/
extern void UNICAM_DrvIRQHandler(void);

/** @brief UNICAM Error Interrupt Handler to be installed in Vector table

    This interface calls @ref UNICAM_DrvProcessErrorInterrupt and signal CAMERA_TASK.

    @behavior Sync, Non-reentrant

    @pre To be executed in Interrupt context

    @param      void

    @retval     void

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

*/
extern void UNICAM_DrvIRQErrHandler(void);

/** @brief Initialize UNICAM

    This API initializes UNICAM driver.

    @behavior Sync, Non-reentrant

    @param      void

    @retval     #BCM_ERR_OK             Initialization is successful
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully initialized, the UNICAM driver shall be in
          #UNICAM_STATE_INITIALISED state

    @trace  #BRCM_SWREQ_UNICAM_INIT

    @limitations None
*/
extern int32_t UNICAM_DrvInit(void);

/** @brief De-initialize UNICAM

    This API de-initializes UNICAM driver.

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @retval     #BCM_ERR_OK             De-initialization is successful
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong  state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully de-initialized, the UNICAM driver shall be in
          #UNICAM_STATE_POWEROFF state

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern int32_t UNICAM_DrvDeinit(void);

/** @brief Enable UNICAM

    This enables UNICAM hardware block.

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_STATE_READY while calling this API

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Start is successful
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully started, UNICAM shall be in #UNICAM_STATE_POWEROFF
          state

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern int32_t UNICAM_DrvStart(void);

/** @brief Disable UNICAM

    This API disables UNICAM hardware block.

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_STATE_STREAMING state while calling this API

    @param      void

    @retval     #BCM_ERR_OK             Stop is successful
    @retval     #BCM_ERR_TIME_OUT       UNICAM stop failed
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully disabled, UNICAM shall be in #UNICAM_STATE_READY
         state

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern int32_t UNICAM_DrvStop(void);

/** @brief Configure UNICAM

    This API configures UNICAM sub-component based on configuration type

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_STATE_INITIALISED or #UNICAM_STATE_READY
         state while calling this API

    @param[in]  aImage       UNICAM sub-component configuration
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
    @retval     #BCM_ERR_INVAL_STATE  State is neither UNICAM_STATE_INITIALISED nor
                                      UNICAM_STATE_READY
    @retval     #BCM_ERR_UNKNOWN      SVC send command has failed

    @post Once successfully configured, UNICAM shall be in #UNICAM_STATE_READY state

    @trace  #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None
*/
extern int32_t UNICAM_DrvConfigure(const UNICAM_ImageType *aCfg,
                                   UNICAM_CommonIfcConfigType *aIfcConfig,
                                   UNICAM_BufParamType *aBuf);

/** @brief Get outstanding events in UNICAM driver

    This API retrieves outstanding events in UNICAM driver

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  void

    @retval uint32_t Mask of event's occurred
                     UNICAM_INT_ERROR   Error interrupt
                     UNICAM_INT_FS      Frame Start interrupt
                     UNICAM_INT_FE      Frame End interrupt
                     UNICAM_INT_DI      Data interrupt
                     UNICAM_INT_DI_END  Data End interrupt

    @post Events are cleared

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern uint32_t UNICAM_DrvGetEvent(void);

/** @brief Retrieves latest filled buffer's index

    This API retrieves latest filled buffer's index.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  void

    @retval uint32_t Current buffer index

    @post None

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern uint32_t UNICAM_DrvGetCurBufIdx(void);

/** @brief Retrieve latest ED buffer

    This API retrieves and copies the latest embedded data buffer to the supplied
    client buffer.

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]  aEDParam Pointer to the ED buffer

    @retval     #BCM_ERR_OK             ED buffer retrieval is successful
    @retval     #BCM_ERR_EAGAIN         ED buffer got updated, try again
    @retval     #BCM_ERR_NOT_FOUND      No buffer available in driver
    @retval     #BCM_ERR_INVAL_PARAMS   aEDParam is NULL
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post None

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern int32_t UNICAM_DrvGetEmbBuffer(UNICAM_CurED_ParamType *aEDParam);

/** @brief Retrieve latest ED buffer pointer

    This API retrieves latest available embedded data buffer pointers.

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]  aEDParam Pointer to current embedded buffer pointers

    @retval     #BCM_ERR_OK             ED buffer retrieval is successful
    @retval     #BCM_ERR_EAGAIN         ED buffer got updated, try again
    @retval     #BCM_ERR_NOT_FOUND      No buffer available in driver
    @retval     #BCM_ERR_INVAL_PARAMS   aEDParam is NULL
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post None

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern int32_t UNICAM_DrvGetEmbBufPtr(UNICAM_CurED_ParamType *aEDParam);

/** @brief Update source frame number
    @behavior Sync, Non-reentrant

    @pre UNICAM is enabled

    @param[in]      aFrameNum       Frame number

    Return values are documented in reverse-chronological order

    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      SVC failed
    @retval     #BCM_ERR_INVAL_STATE  UNICAM is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern int32_t UNICAM_DrvUpdateFrameNum(uint32_t aFrameNum);

#endif /* UNICAM_H */
/** @} */
