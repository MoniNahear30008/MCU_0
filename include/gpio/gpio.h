/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
  @defgroup grp_gpiodrv_ifc GPIO Interface
  @ingroup grp_gpiodrv
  @addtogroup grp_gpiodrv_ifc
  @{

  @section sec_gpio_overview  Overview
    GPIO interface contains up to 81 GPIO pins. Each pin can be configured as
    an individual input or output channel. The channels can be clustered as
    Port based or ChannelGroup based.

  @section sec_gpio_port_cluster  Port Based Clustering
    The 81 GPIO channels can be divided into 3 ports:
    - GPIO_PORT_0 contains 32 channels (GPIO_CHANNEL_0  - GPIO_CHANNEL_31)
    - GPIO_PORT_1 contains 32 channels (GPIO_CHANNEL_32 - GPIO_CHANNEL_63)
    - GPIO_PORT_2 contains 17 channels (GPIO_CHANNEL_64 - GPIO_CHANNEL_80)
    @note
      A port can be initialized and operated by these APIs: #GPIO_DrvInitPort,
      #GPIO_DrvPortRead, and #GPIO_DrvPortWrite

  @section sec_gpio_channel_group_cluster  ChannelGroup Based Clustering
    A ChannelGroups is a group of channels defined by the data structure
    #GPIO_ChannelGroupType which specifies a port, starting channel ID and
    a bitmap mask of channel members.
    For example:
    - The ChannelGroup {.port = 0, .offset=4, .mask= 0x033F}
      contains 8 channels:  GPIO_CHANNEL_13/_12/_9/_8/_7/_6/_5/_4 .
    - The ChannelGroup {.port = 1, .offset=28,.mask= 0x000F}
      contains 4 channels:  GPIO_CHANNEL_63/_62/_61/_60 .
    @note
      Each ChannelGroup can have up to 32 channel members.
      Make sure channel members should not cross port boundary.
    @note
      A ChannelGroup can be operated by the APIs
      #GPIO_DrvChannelGroupRead and #GPIO_DrvChannelGroupWrite

    @section sec_gpio_program Programmable Parameters
    The following key parameters are programmable through the data structure
    #GPIO_ConfigType:
    - GPIO pin mode (Input/Output/Alternate) control
    - GPIO output type (Push-pull/open drain) control
    - GPIO pull-up, pull-down control
    - GPIO output data control
    - GPIO alternate function control
    - GPIO input hysteresis control
    - GPIO drive strength control
    - GPIO input disable control
    - GPIO slew rate control
    - GPIO de glitch filter count control
    - GPIO de glitch filter bypass control
    - GPIO din raw control
    - GPIO din force control
    - GPIO din force value control
    - GPIO din invert control
    - GPIO dout invert control
    - Mask to select GPIO control features

    @section api_list List of GPIO APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref GPIO_DrvInitChannel | Initialize specified GPIO channel |
    | @ref GPIO_DrvInitPort | Initialize GPIO port |
    | @ref GPIO_DrvDeInit | De-initialize GPIO |
    | @ref GPIO_DrvSetChannelConfig | Apply specified configuration to the specific channel |
    | @ref GPIO_DrvChannelRead | Read level of a GPIO channel |
    | @ref GPIO_DrvChannelWrite | Set the level of a GPIO channel |
    | @ref GPIO_DrvPortRead | Read GPIO port level |
    | @ref GPIO_DrvPortWrite | Set GPIO port level |
    | @ref GPIO_DrvChannelGroupRead | Read GPIO channel group level |
    | @ref GPIO_DrvChannelGroupWrite | Set GPIO channel group level |
    | @ref GPIO_DrvChannelFlip | Flip GPIO channel level |
    | @ref INTRCTRL_DrvEnableGpioInterrupt | Enable GPIO interrupt     |
    | @ref INTRCTRL_DrvGpioInterruptStatus | Get GPIO interrupt status |
    | @ref INTRCTRL_DrvGpioEventStatus     | Get GPIO event status     |
    | @ref INTRCTRL_DrvClearGpioInterrupt  | Get GPIO clear interrupts |

  @file gpio.h
  @brief GPIO interface
  @section apis This section defines the interface for GPIO driver
  @version 0.1 Imported from docx
*/

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/**
    @name GPIO driver API IDs
    @{
    @brief API IDs for GPIO driver
*/
#define BRCM_SWARCH_GPIO_HW_ID_TYPE                      (0x8301U)  /**< @brief #GPIO_HwIDType              */
#define BRCM_SWARCH_GPIO_STATE_TYPE                      (0x8302U)  /**< @brief #GPIO_StateType             */
#define BRCM_SWARCH_GPIO_CHANNEL_TYPE                    (0x8303U)  /**< @brief #GPIO_ChannelType           */
#define BRCM_SWARCH_GPIO_PORT_TYPE                       (0x8304U)  /**< @brief #GPIO_PortType              */
#define BRCM_SWARCH_GPIO_CHANNEL_GROUP_TYPE              (0x8305U)  /**< @brief #GPIO_ChannelGroupType      */
#define BRCM_SWARCH_GPIO_PORT_LEVEL_TYPE                 (0x8306U)  /**< @brief #GPIO_PortLevelType         */
#define BRCM_SWARCH_GPIO_CFG_MODE_TYPE                   (0x8307U)  /**< @brief #GPIO_CfgModeType           */
#define BRCM_SWARCH_GPIO_CFG_OUTPUT_TYPE                 (0x8308U)  /**< @brief #GPIO_CfgOutputType         */
#define BRCM_SWARCH_GPIO_CFG_PUPD_TYPE                   (0x8309U)  /**< @brief #GPIO_CfgPuPdType           */
#define BRCM_SWARCH_GPIO_LEVEL_TYPE                      (0x830AU)  /**< @brief #GPIO_LevelType             */
#define BRCM_SWARCH_GPIO_CFG_ALT_FUNC_TYPE               (0x830BU)  /**< @brief #GPIO_CfgAltFuncType        */
#define BRCM_SWARCH_GPIO_CFG_HYSTERESIS_TYPE             (0x830CU)  /**< @brief #GPIO_CfgHysteresisType     */
#define BRCM_SWARCH_GPIO_CFG_DRIVE_STRENGTH_TYPE         (0x830DU)  /**< @brief #GPIO_CfgDriveStrengthType  */
#define BRCM_SWARCH_GPIO_CFG_INPUT_TYPE                  (0x830EU)  /**< @brief #GPIO_CfgInputType          */
#define BRCM_SWARCH_GPIO_CFG_SLEW_RATE_TYPE              (0x830FU)  /**< @brief #GPIO_CfgSlewRateType       */
#define BRCM_SWARCH_GPIO_CFG_DEGLITCH_CNT_TYPE           (0x8310U)  /**< @brief #GPIO_CfgDeglitchCntType    */
#define BRCM_SWARCH_GPIO_CFG_DEGLITCH_BYPASS_TYPE        (0x8311U)  /**< @brief #GPIO_CfgDeglitchBypassType */
#define BRCM_SWARCH_GPIO_CFG_DIN_RAW_TYPE                (0x8312U)  /**< @brief #GPIO_CfgDinRawType         */
#define BRCM_SWARCH_GPIO_CFG_DIN_FORCE_TYPE              (0x8313U)  /**< @brief #GPIO_CfgDinForceType       */
#define BRCM_SWARCH_GPIO_CFG_DIN_FORCE_VAL_TYPE          (0x8314U)  /**< @brief #GPIO_CfgDinForceValType    */
#define BRCM_SWARCH_GPIO_CFG_DIN_INVERT_TYPE             (0x8315U)  /**< @brief #GPIO_CfgDinInvertType      */
#define BRCM_SWARCH_GPIO_CFG_DOUT_INVERT_TYPE            (0x8316U)  /**< @brief #GPIO_CfgDoutInvertType     */
#define BRCM_SWARCH_GPIO_CONFIG_TYPE                     (0x8317U)  /**< @brief #GPIO_ConfigType            */
#define BRCM_SWARCH_GPIO_CFG_MASK_TYPE                   (0x8318U)  /**< @brief #GPIO_CfgMaskType           */

#define BRCM_SWARCH_GPIO_DRV_INIT_CHANNEL_PROC           (0x8319U)  /**< @brief #GPIO_DrvInitChannel        */
#define BRCM_SWARCH_GPIO_DRV_INIT_PORT_PROC              (0x831AU)  /**< @brief #GPIO_DrvInitPort           */
#define BRCM_SWARCH_GPIO_DRV_DEINIT_PROC                 (0x831BU)  /**< @brief #GPIO_DrvDeInit             */
#define BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC     (0x831CU)  /**< @brief #GPIO_DrvSetChannelConfig   */
#define BRCM_SWARCH_GPIO_DRV_CHANNEL_READ_PROC           (0x831DU)  /**< @brief #GPIO_DrvChannelRead        */
#define BRCM_SWARCH_GPIO_DRV_CHANNEL_WRITE_PROC          (0x831EU)  /**< @brief #GPIO_DrvChannelWrite       */
#define BRCM_SWARCH_GPIO_DRV_PORT_READ_PROC              (0x831FU)  /**< @brief #GPIO_DrvPortRead           */
#define BRCM_SWARCH_GPIO_DRV_PORT_WRITE_PROC             (0x8320U)  /**< @brief #GPIO_DrvPortWrite          */
#define BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_READ_PROC     (0x8321U)  /**< @brief #GPIO_DrvChannelGroupRead   */
#define BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_WRITE_PROC    (0x8322U)  /**< @brief #GPIO_DrvChannelGroupWrite  */
#define BRCM_SWARCH_GPIO_DRV_CHANNEL_FLIP_PROC           (0x8323U)  /**< @brief #GPIO_DrvChannelFlip        */

/** @} */

/**
    @name GPIO_HwIDType
    @{
    @brief Index of the GPIO controller

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint32_t GPIO_HwIDType; /**< @brief GPIO HW Id type */
#define GPIO_HW_ID_0      (0UL) /**< @brief GPIO HW Id 0    */
/** @} */

/**
    @name GPIO_StateType
    @{
    @brief State of GPIO driver

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_StateType;           /**< @brief GPIO state type */
#define GPIO_STATE_UNINIT           (0U)  /**< @brief GPIO is uninitialized & value must be zero*/
#define GPIO_STATE_INITIALIZED      (1U)  /**< @brief GPIO is initialized */
/** @} */

/**
    @name GPIO_ChannelType
    @{
    @brief GPIO channel type
    Specifies the numeric ID of a GPIO channel

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
typedef uint32_t GPIO_ChannelType;  /**< @brief GPIO channel type */
#define GPIO_CHANNEL_0      (0UL)   /**< @brief Channel 0 ID      */
#define GPIO_CHANNEL_1      (1UL)   /**< @brief Channel 1 ID      */
#define GPIO_CHANNEL_2      (2UL)   /**< @brief Channel 2 ID      */
#define GPIO_CHANNEL_3      (3UL)   /**< @brief Channel 3 ID      */
#define GPIO_CHANNEL_4      (4UL)   /**< @brief Channel 4 ID      */
#define GPIO_CHANNEL_5      (5UL)   /**< @brief Channel 5 ID      */
#define GPIO_CHANNEL_6      (6UL)   /**< @brief Channel 6 ID      */
#define GPIO_CHANNEL_7      (7UL)   /**< @brief Channel 7 ID      */
#define GPIO_CHANNEL_8      (8UL)   /**< @brief Channel 8 ID      */
#define GPIO_CHANNEL_9      (9UL)   /**< @brief Channel 9 ID      */
#define GPIO_CHANNEL_10     (10UL)  /**< @brief Channel 10 ID     */
#define GPIO_CHANNEL_11     (11UL)  /**< @brief Channel 11 ID     */
#define GPIO_CHANNEL_12     (12UL)  /**< @brief Channel 12 ID     */
#define GPIO_CHANNEL_13     (13UL)  /**< @brief Channel 13 ID     */
#define GPIO_CHANNEL_14     (14UL)  /**< @brief Channel 14 ID     */
#define GPIO_CHANNEL_15     (15UL)  /**< @brief Channel 15 ID     */
#define GPIO_CHANNEL_16     (16UL)  /**< @brief Channel 16 ID     */
#define GPIO_CHANNEL_17     (17UL)  /**< @brief Channel 17 ID     */
#define GPIO_CHANNEL_18     (18UL)  /**< @brief Channel 18 ID     */
#define GPIO_CHANNEL_19     (19UL)  /**< @brief Channel 19 ID     */
#define GPIO_CHANNEL_20     (20UL)  /**< @brief Channel 20 ID     */
#define GPIO_CHANNEL_21     (21UL)  /**< @brief Channel 21 ID     */
#define GPIO_CHANNEL_22     (22UL)  /**< @brief Channel 22 ID     */
#define GPIO_CHANNEL_23     (23UL)  /**< @brief Channel 23 ID     */
#define GPIO_CHANNEL_24     (24UL)  /**< @brief Channel 24 ID     */
#define GPIO_CHANNEL_25     (25UL)  /**< @brief Channel 25 ID     */
#define GPIO_CHANNEL_26     (26UL)  /**< @brief Channel 26 ID     */
#define GPIO_CHANNEL_27     (27UL)  /**< @brief Channel 27 ID     */
#define GPIO_CHANNEL_28     (28UL)  /**< @brief Channel 28 ID     */
#define GPIO_CHANNEL_29     (29UL)  /**< @brief Channel 29 ID     */
#define GPIO_CHANNEL_30     (30UL)  /**< @brief Channel 30 ID     */
#define GPIO_CHANNEL_31     (31UL)  /**< @brief Channel 31 ID     */
#define GPIO_CHANNEL_32     (32UL)  /**< @brief Channel 32 ID     */
#define GPIO_CHANNEL_33     (33UL)  /**< @brief Channel 33 ID     */
#define GPIO_CHANNEL_34     (34UL)  /**< @brief Channel 34 ID     */
#define GPIO_CHANNEL_35     (35UL)  /**< @brief Channel 35 ID     */
#define GPIO_CHANNEL_36     (36UL)  /**< @brief Channel 36 ID     */
#define GPIO_CHANNEL_37     (37UL)  /**< @brief Channel 37 ID     */
#define GPIO_CHANNEL_38     (38UL)  /**< @brief Channel 38 ID     */
#define GPIO_CHANNEL_39     (39UL)  /**< @brief Channel 39 ID     */
#define GPIO_CHANNEL_40     (40UL)  /**< @brief Channel 40 ID     */
#define GPIO_CHANNEL_41     (41UL)  /**< @brief Channel 41 ID     */
#define GPIO_CHANNEL_42     (42UL)  /**< @brief Channel 42 ID     */
#define GPIO_CHANNEL_43     (43UL)  /**< @brief Channel 43 ID     */
#define GPIO_CHANNEL_44     (44UL)  /**< @brief Channel 44 ID     */
#define GPIO_CHANNEL_45     (45UL)  /**< @brief Channel 45 ID     */
#define GPIO_CHANNEL_46     (46UL)  /**< @brief Channel 46 ID     */
#define GPIO_CHANNEL_47     (47UL)  /**< @brief Channel 47 ID     */
#define GPIO_CHANNEL_48     (48UL)  /**< @brief Channel 48 ID     */
#define GPIO_CHANNEL_49     (49UL)  /**< @brief Channel 49 ID     */
#define GPIO_CHANNEL_50     (50UL)  /**< @brief Channel 50 ID     */
#define GPIO_CHANNEL_51     (51UL)  /**< @brief Channel 51 ID     */
#define GPIO_CHANNEL_52     (52UL)  /**< @brief Channel 52 ID     */
#define GPIO_CHANNEL_53     (53UL)  /**< @brief Channel 53 ID     */
#define GPIO_CHANNEL_54     (54UL)  /**< @brief Channel 54 ID     */
#define GPIO_CHANNEL_55     (55UL)  /**< @brief Channel 55 ID     */
#define GPIO_CHANNEL_56     (56UL)  /**< @brief Channel 56 ID     */
#define GPIO_CHANNEL_57     (57UL)  /**< @brief Channel 57 ID     */
#define GPIO_CHANNEL_58     (58UL)  /**< @brief Channel 58 ID     */
#define GPIO_CHANNEL_59     (59UL)  /**< @brief Channel 59 ID     */
#define GPIO_CHANNEL_60     (60UL)  /**< @brief Channel 60 ID     */
#define GPIO_CHANNEL_61     (61UL)  /**< @brief Channel 61 ID     */
#define GPIO_CHANNEL_62     (62UL)  /**< @brief Channel 62 ID     */
#define GPIO_CHANNEL_63     (63UL)  /**< @brief Channel 63 ID     */
#define GPIO_CHANNEL_64     (64UL)  /**< @brief Channel 64 ID     */
#define GPIO_CHANNEL_65     (65UL)  /**< @brief Channel 65 ID     */
#define GPIO_CHANNEL_66     (66UL)  /**< @brief Channel 66 ID     */
#define GPIO_CHANNEL_67     (67UL)  /**< @brief Channel 67 ID     */
#define GPIO_CHANNEL_68     (68UL)  /**< @brief Channel 68 ID     */
#define GPIO_CHANNEL_69     (69UL)  /**< @brief Channel 69 ID     */
#define GPIO_CHANNEL_70     (70UL)  /**< @brief Channel 70 ID     */
#define GPIO_CHANNEL_71     (71UL)  /**< @brief Channel 71 ID     */
#define GPIO_CHANNEL_72     (72UL)  /**< @brief Channel 72 ID     */
#define GPIO_CHANNEL_73     (73UL)  /**< @brief Channel 73 ID     */
#define GPIO_CHANNEL_74     (74UL)  /**< @brief Channel 74 ID     */
#define GPIO_CHANNEL_75     (75UL)  /**< @brief Channel 75 ID     */
#define GPIO_CHANNEL_76     (76UL)  /**< @brief Channel 76 ID     */
#define GPIO_CHANNEL_77     (77UL)  /**< @brief Channel 77 ID     */
#define GPIO_CHANNEL_78     (78UL)  /**< @brief Channel 78 ID     */
#define GPIO_CHANNEL_79     (79UL)  /**< @brief Channel 79 ID     */
#define GPIO_CHANNEL_80     (80UL)  /**< @brief Channel 80 ID     */
#define GPIO_CHANNEL_MAX    (81UL)  /**< @brief Channel MAXID     */
/** @} */

/**
    @name GPIO_PortType
    @{
    @brief GPIO Port type
    Specifies the numeric ID of a GPIO port

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
typedef uint8_t GPIO_PortType;  /**< @brief GPIO Port type */
#define GPIO_PORT_0     (0U)   /**< @brief GPIO Port ID 0 :<br>GPIO_CHANNEL_0  - GPIO_CHANNEL_31 */
#define GPIO_PORT_1     (1U)   /**< @brief GPIO Port ID 1 :<br>GPIO_CHANNEL_32 - GPIO_CHANNEL_63 */
#define GPIO_PORT_2     (2U)   /**< @brief GPIO Port ID 2 :<br>GPIO_CHANNEL_64 - GPIO_CHANNEL_80 */
#define GPIO_PORT_MAX   (3U)   /**< @brief GPIO MAX port */
/** @} */

/**
    @name GPIO_ChannelGroupType
    @{
    @brief GPIO channel group
    Type for the definition of a channel group, which consists of several
    adjoining channels (up to 32) within a port.

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
typedef struct sGPIO_ChannelGroupType {
    uint32_t      mask  ; /**< @brief Element mask which defines the position of the channels inside channel group   */
    GPIO_PortType port  ; /**< @brief Port on which the Channel group is defined                                     */
    uint8_t       offset; /**< @brief Position of the Channel Group on the port, counted from the LSB
                               For Example : The ChannelGroup {.mask= 0x033F, .offset=4, .port = 0}
                               contains 8 channels:  GPIO_CHANNEL_13/_12/_9/_8/_7/_6/_5/_4 .                         */
} GPIO_ChannelGroupType;
/** @} */

/**
    @name GPIO_PortLevelType
    @{
    @brief GPIO port level type
    Type for the value of a GPIO port, where each bit in the value represents
    the level for each channel.

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
typedef uint32_t GPIO_PortLevelType;
/** @} */

/**
    @name GPIO_CfgModeType
    @{
    @brief GPIO pin mode

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgModeType;           /**< @brief GPIO config mode             */
#define GPIO_CFG_MODE_INPUT          (0x0U) /**< @brief GPIO Input  mode             */
#define GPIO_CFG_MODE_OUTPUT         (0x1U) /**< @brief GPIO Output mode             */
#define GPIO_CFG_MODE_ALT_FUNC       (0x2U) /**< @brief GPIO Alternate Function mode */
#define GPIO_CFG_MODE_RSVD           (0x3U) /**< @brief GPIO mode reserved           */
#define GPIO_CFG_MODE_MAX            (0x4U) /**< @brief GPIO maximum mode            */
/** @} */

/**
    @name GPIO_CfgOutputType
    @{
    @brief GPIO output type control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgOutputType;         /**< @brief GPIO output type control */
#define GPIO_CFG_OUTPUT_PUSH_PULL    (0x0U) /**< @brief GPIO Output Push-pull    */
#define GPIO_CFG_OUTPUT_OPEN_DRAIN   (0x1U) /**< @brief GPIO Output Open-drain   */
#define GPIO_CFG_OUTPUT_MAX          (0x2U) /**< @brief GPIO Maximum Output type */
/** @} */

/**
    @name GPIO_CfgPuPdType
    @{
    @brief GPIO pull-up, pull-down control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgPuPdType;           /**< @brief GPIO pull-up, pull-down control */
#define GPIO_CFG_PUPD_NONE           (0x0U) /**< @brief No pull-up, no pull-down        */
#define GPIO_CFG_PUPD_PULL_UP        (0x1U) /**< @brief Pull-up                         */
#define GPIO_CFG_PUPD_PULL_DOWN      (0x2U) /**< @brief Pull-down                       */
#define GPIO_CFG_PUPD_RSVD           (0x3U) /**< @brief Reserved                        */
#define GPIO_CFG_PUPD_MAX            (0x4U) /**< @brief PUPD MAX                        */
/** @} */

/**
    @name GPIO_LevelType
    @{
    @brief GPIO channel level
    Type for the possible levels that a GPIO channel can have

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
typedef uint8_t GPIO_LevelType;         /**< @brief GPIO channel level type */
#define GPIO_LEVEL_LOW          (0x00U) /**< @brief Physical State Low      */
#define GPIO_LEVEL_HIGH         (0x01U) /**< @brief Physical State High     */
/** @} */

/**
    @name GPIO_CfgAltFuncType
    @{
    @brief GPIO alternate function control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgAltFuncType;         /**< @brief GPIO alternate function control  */
#define GPIO_CFG_ALT_FUNC_0           (0x0U) /**< @brief Alternate function 0             */
#define GPIO_CFG_ALT_FUNC_1           (0x1U) /**< @brief Alternate function 1             */
#define GPIO_CFG_ALT_FUNC_2           (0x2U) /**< @brief Alternate function 2             */
#define GPIO_CFG_ALT_FUNC_3           (0x3U) /**< @brief Alternate function 3             */
#define GPIO_CFG_ALT_FUNC_4           (0x4U) /**< @brief Alternate function 4             */
#define GPIO_CFG_ALT_FUNC_5           (0x5U) /**< @brief Alternate function 5             */
#define GPIO_CFG_ALT_FUNC_MAX         (0x6U) /**< @brief Alternate function MAX           */
/** @} */

/**
    @name GPIO_CfgHysteresisType
    @{
    @brief GPIO input hysteresis control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgHysteresisType;          /**< @brief GPIO input hysteresis control */
#define GPIO_CFG_HYSTERESIS_DISABLE       (0x0U) /**< @brief input hysteresis disabled     */
#define GPIO_CFG_HYSTERESIS_ENABLE        (0x1U) /**< @brief input hysteresis enabled      */
/** @} */

/**
    @name GPIO_CfgDriveStrengthType
    @{
    @brief GPIO drive strength control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgDriveStrengthType;         /**< @brief GPIO drive strength control */
#define GPIO_CFG_DRIVE_STRENGTH_2MA         (0x0U) /**< @brief GPIO drive strength 2mA     */
#define GPIO_CFG_DRIVE_STRENGTH_4MA         (0x1U) /**< @brief GPIO drive strength 4mA     */
#define GPIO_CFG_DRIVE_STRENGTH_6MA         (0x2U) /**< @brief GPIO drive strength 6mA     */
#define GPIO_CFG_DRIVE_STRENGTH_8MA         (0x3U) /**< @brief GPIO drive strength 8mA     */
#define GPIO_CFG_DRIVE_STRENGTH_10MA        (0x4U) /**< @brief GPIO drive strength 10mA    */
#define GPIO_CFG_DRIVE_STRENGTH_12MA        (0x5U) /**< @brief GPIO drive strength 12mA    */
#define GPIO_CFG_DRIVE_STRENGTH_14MA        (0x6U) /**< @brief GPIO drive strength 14mA    */
#define GPIO_CFG_DRIVE_STRENGTH_16MA        (0x7U) /**< @brief GPIO drive strength 16mA    */
#define GPIO_CFG_DRIVE_STRENGTH_MAX         (0x8U) /**< @brief GPIO drive strength Max     */
/** @} */

/**
    @name GPIO_CfgInputType
    @{
    @brief GPIO input disable control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgInputType;                 /**< @brief GPIO input disable control */
#define GPIO_CFG_INPUT_ENABLE               (0x0U) /**< @brief input enabled , din = pad  */
#define GPIO_CFG_INPUT_DISABLE              (0x1U) /**< @brief input disabled, din = 0    */
/** @} */

/**
    @name GPIO_CfgSlewRateType
    @{
    @brief GPIO slew rate control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgSlewRateType;              /**< @brief GPIO slew rate control */
#define GPIO_CFG_SLEW_RATE_FAST             (0x0U) /**< @brief Fast edges             */
#define GPIO_CFG_SLEW_RATE_SLEWED           (0x1U) /**< @brief Slewed edges           */
/** @} */

/**
    @name GPIO_CfgDeglitchCntType
    @{
    @brief GPIO de glitch filter count control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgDeglitchCntType;          /**< @brief GPIO deglitch filter counter,
                                                        when pad is input and deglitch_bypass = 0 */
#define GPIO_CFG_DEGLITCH_CNT_0           (0x0U)  /**< @brief count 0 cycle  for din deglitch    */
#define GPIO_CFG_DEGLITCH_CNT_1           (0x1U)  /**< @brief count 1 cycle  for din deglitch    */
#define GPIO_CFG_DEGLITCH_CNT_2           (0x2U)  /**< @brief count 2 cycles for din deglitch    */
#define GPIO_CFG_DEGLITCH_CNT_3           (0x3U)  /**< @brief count 3 cycles for din deglitch    */
#define GPIO_CFG_DEGLITCH_CNT_4           (0x4U)  /**< @brief count 4 cycles for din deglitch    */
#define GPIO_CFG_DEGLITCH_CNT_5           (0x5U)  /**< @brief count 5 cycles for din deglitch    */
#define GPIO_CFG_DEGLITCH_CNT_6           (0x6U)  /**< @brief count 6 cycles for din deglitch    */
#define GPIO_CFG_DEGLITCH_CNT_7           (0x7U)  /**< @brief count 7 cycles for din deglitch    */
#define GPIO_CFG_DEGLITCH_CNT_MAX         (0x8U)  /**< @brief count max cycles for din deglitch  */
/** @} */

/**
    @name GPIO_CfgDeglitchBypassType
    @{
    @brief GPIO deglitch filter bypass control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgDeglitchBypassType;             /**< @brief GPIO deglitch filter bypass control */
#define GPIO_CFG_DEGLITCH_BYPASS_DISABLE        (0x0U)  /**< @brief Do not bypass din deglitch filter   */
#define GPIO_CFG_DEGLITCH_BYPASS_ENABLE         (0x1U)  /**< @brief Bypass din deglitch filter          */
/** @} */

/**
    @name GPIO_CfgDinRawType
    @{
    @brief GPIO din raw control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgDinRawType;          /**< @brief GPIO din raw control, muxes between the raw din value
                                                   or the synchronized/deglitched din value     */
#define GPIO_CFG_DIN_RAW_SYNC        (0x0U)  /**< @brief Use synchronized/deglitched din value */
#define GPIO_CFG_DIN_RAW_RAW         (0x1U)  /**< @brief Use raw din value                     */
/** @} */

/**
    @name GPIO_CfgDinForceType
    @{
    @brief GPIO din force control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgDinForceType;          /**< @brief GPIO din force control */
#define GPIO_CFG_DIN_FORCE_NONE         (0x0U) /**< @brief No force               */
#define GPIO_CFG_DIN_FORCE_VALUE        (0x1U) /**< @brief Force din_frcval       */
/** @} */

/**
    @name GPIO_CfgDinForceValType
    @{
    @brief GPIO din force control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgDinForceValType;        /**< @brief GPIO din force control */
#define GPIO_CFG_DIN_FORCE_VAL_LOW       (0x0U) /**< @brief No force               */
#define GPIO_CFG_DIN_FORCE_VAL_HIGH      (0x1U) /**< @brief Force din_frcval       */
/** @} */

/**
    @name GPIO_CfgDinInvertType
    @{
    @brief GPIO din invert control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgDinInvertType;       /**< @brief GPIO din invert control */
#define GPIO_CFG_DIN_INVERT_DISABLE   (0x0U) /**< @brief Does not invert din     */
#define GPIO_CFG_DIN_INVERT_ENABLE    (0x1U) /**< @brief Inverts din             */
/** @} */

/**
    @name GPIO_CfgDoutInvertType
    @{
    @brief GPIO dout invert control

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint8_t GPIO_CfgDoutInvertType;        /**< @brief GPIO dout invert control */
#define GPIO_CFG_DOUT_INVERT_DISABLE    (0x0U) /**< @brief Does not invert dout     */
#define GPIO_CFG_DOUT_INVERT_ENABLE     (0x1U) /**< @brief Inverts dout             */
/** @} */

/**
    @name GPIO_CfgMaskType
    @{
    @brief Mask to select GPIO control features

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef uint32_t GPIO_CfgMaskType;                /**< @brief GPIO control feature mask                                  */
#define GPIO_CFG_MASK_MODE        (0x1UL << 0UL ) /**< @brief Mask to select GPIO mode control feature                   */
#define GPIO_CFG_MASK_OTYPE       (0x1UL << 1UL ) /**< @brief Mask to select GPIO Output type control feature            */
#define GPIO_CFG_MASK_PUPD        (0x1UL << 2UL ) /**< @brief Mask to select GPIO Pull Up Down control feature           */
#define GPIO_CFG_MASK_DOUT        (0x1UL << 3UL ) /**< @brief Mask to select GPIO Output data control feature            */
#define GPIO_CFG_MASK_BSET        (0x1UL << 4UL ) /**< @brief Mask to select GPIO Bit set control feature                */
#define GPIO_CFG_MASK_BCLR        (0x1UL << 5UL ) /**< @brief Mask to select GPIO Bit clear control feature              */
#define GPIO_CFG_MASK_ALTF        (0x1UL << 6UL ) /**< @brief Mask to select GPIO Alternate function control feature     */
#define GPIO_CFG_MASK_HIST        (0x1UL << 7UL ) /**< @brief Mask to select GPIO hysteresis control feature             */
#define GPIO_CFG_MASK_SEL         (0x1UL << 8UL ) /**< @brief Mask to select GPIO drive strength control feature         */
#define GPIO_CFG_MASK_IND         (0x1UL << 9UL ) /**< @brief Mask to select GPIO input disable control feature          */
#define GPIO_CFG_MASK_SRC         (0x1UL << 10UL) /**< @brief Mask to select GPIO slew rate control feature              */
#define GPIO_CFG_MASK_DG_CNT      (0x1UL << 11UL) /**< @brief Mask to select GPIO de glitch filter count control feature */
#define GPIO_CFG_MASK_DG_BYP      (0x1UL << 12UL) /**< @brief Mask to select GPIO de glitch filter bypass control feature*/
#define GPIO_CFG_MASK_DIN_RAW     (0x1UL << 13UL) /**< @brief Mask to select GPIO din raw control feature                */
#define GPIO_CFG_MASK_DIN_FRC     (0x1UL << 14UL) /**< @brief Mask to select GPIO din force control feature              */
#define GPIO_CFG_MASK_DIN_FRCVAL  (0x1UL << 15UL) /**< @brief Mask to select GPIO din force value control feature        */
#define GPIO_CFG_MASK_DIN_INV     (0x1UL << 16UL) /**< @brief Mask to select GPIO din invert control feature             */
#define GPIO_CFG_MASK_DOUT_INV    (0x1UL << 17UL) /**< @brief Mask to select GPIO din invert control feature             */
#define GPIO_CFG_MASK_GRP_0       0x0000FUL       /**< @brief GPIO control feature group0 mask                           */
#define GPIO_CFG_MASK_GRP_1       0x000F0UL       /**< @brief GPIO control feature group1 mask                           */
#define GPIO_CFG_MASK_GRP_2       0x00F00UL       /**< @brief GPIO control feature group2 mask                           */
#define GPIO_CFG_MASK_GRP_3       0x0F000UL       /**< @brief GPIO control feature group3 mask                           */
#define GPIO_CFG_MASK_GRP_4       0x30000UL       /**< @brief GPIO control feature group4 mask                           */
#define GPIO_CFG_MASK_GRP_ALL     0x3FFFFUL       /**< @brief GPIO control feature group all mask                        */
/** @} */

/**
    @name GPIO_ConfigType
    @{
    @brief GPIO configuration structure

    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef struct sGPIO_ConfigType {
    GPIO_CfgModeType            mode;       /**< @brief GPIO pin mode (Input/Output/Alternate) control  */
    GPIO_CfgOutputType          oType;      /**< @brief GPIO output type (Push-pull/open drain) control */
    GPIO_CfgPuPdType            pupd;       /**< @brief GPIO pull-up, pull-down control                 */
    GPIO_LevelType              dout;       /**< @brief GPIO output data control                        */
    GPIO_CfgAltFuncType         altFunc;    /**< @brief GPIO alternate function control                 */
    GPIO_CfgHysteresisType      hys;        /**< @brief GPIO input hysteresis control                   */
    GPIO_CfgDriveStrengthType   strength;   /**< @brief GPIO drive strength control                     */
    GPIO_CfgInputType           ind;        /**< @brief GPIO input disable control                      */
    GPIO_CfgSlewRateType        slewRate;   /**< @brief GPIO slew rate control                          */
    GPIO_CfgDeglitchCntType     dgCnt;      /**< @brief GPIO de glitch filter count control             */
    GPIO_CfgDeglitchBypassType  dgByp;      /**< @brief GPIO de glitch filter bypass control            */
    GPIO_CfgDinRawType          dinRawSync; /**< @brief GPIO din raw control                            */
    GPIO_CfgDinForceType        dinFrc;     /**< @brief GPIO din force control                          */
    GPIO_CfgDinForceValType     dinFrcVal;  /**< @brief GPIO din force value control                    */
    GPIO_CfgDinInvertType       dinInvert;  /**< @brief GPIO din invert control                         */
    GPIO_CfgDoutInvertType      doutInvert; /**< @brief GPIO dout invert control                        */
    GPIO_CfgMaskType            aCfgMask;   /**< @brief Mask to select GPIO control features            */

} GPIO_ConfigType;
/** @} */

/** @brief Initialize specified GPIO channel

    API to Initialize a specific channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the GPIO controller
    @param[in]      aChannelId  GPIO channel ID.
    @param[in]      aConfig     Pointer to GPIO configuration structure
    @return         int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid channel
    @return     #BCM_ERR_INVAL_PARAMS   NULL configuration

    @post GPIO driver state changed to #GPIO_STATE_INITIALIZED

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations  None
*/
int32_t GPIO_DrvInitChannel(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig);

/** @brief Initialize specified GPIO Port

    API to Initialize a GPIO port.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId            Index of the GPIO controller
    @param[in]      aPortId         GPIO port ID<br>
                                        GPIO_PORT_0: (32 channels)<br>
                                             GPIO_CHANNEL_0  - GPIO_CHANNEL_31<br>
                                        GPIO_PORT_1: (32 channels)<br>
                                             GPIO_CHANNEL_32 - GPIO_CHANNEL_63<br>
                                        GPIO_PORT_2: (17 channels)<br>
                                             GPIO_CHANNEL_64 - GPIO_CHANNEL_80
    @param[in]      aConfig         Pointer to GPIO configuration structure
    @return         int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid port
    @return     #BCM_ERR_INVAL_PARAMS   NULL configuration

    @post GPIO driver state changed to #GPIO_STATE_INITIALIZED

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations None
*/
int32_t GPIO_DrvInitPort(GPIO_HwIDType hwId, const GPIO_PortType aPortId, GPIO_ConfigType *aConfig);

/** @brief De-initialize GPIO module

    This API de-initializes a GPIO port. Resets the given GPIO controller and puts back
    to its original state (same as Power on reset).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  hwId                    Index of the GPIO controller
    @return     int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post       GPIO driver state changed to #GPIO_STATE_UNINIT

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations None
*/
int32_t GPIO_DrvDeInit(GPIO_HwIDType hwId);

/** @brief Set configuration to specified GPIO channel

    API to apply specific configuration to specified channel

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId        Index of the GPIO controller
    @param[in]    aChannelId  ID of GPIO Channel
    @param[in]    aConfig     Pointer to GPIO configuration structure
    @return       int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid channel
    @return     #BCM_ERR_INVAL_PARAMS   NULL configuration
    @retval     #BCM_ERR_NODEV          GPIO instance not initialized

    @post None

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations None
*/
int32_t GPIO_DrvSetChannelConfig(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig);

/** @brief Read level of specified GPIO channel

    API to read the level of a GPIO channel.

    @behavior Sync, Non-reentrant

    @pre None
    @param[in]    hwId               Index of the GPIO controller
    @param[in]    aChannelId         ID of GPIO Channel
    @param[Out]   aLevel             Pointer to GPIO channel level
    @return       int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid channel
    @return     #BCM_ERR_INVAL_PARAMS   NULL aLevel pointer
    @retval     #BCM_ERR_NODEV          GPIO instance not initialized

    @post None

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations None
*/
int32_t GPIO_DrvChannelRead(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_LevelType *aLevel);

/** @brief Set the level of a GPIO channel

    API to set the level of a GPIO Channel.

    @note If the respective GPIO pin is not configured as GPIO output, this
    API has no effect on the pin output.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId        Index of the GPIO controller
    @param[in]    aChannelId  ID of GPIO Channel
    @param[in]    aLevel      GPIO level to be set
    @return       int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid channel
    @retval     #BCM_ERR_NODEV          GPIO instance not initialized

    @post None

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations None
*/
int32_t GPIO_DrvChannelWrite(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_LevelType aLevel);

/** @brief Read level of specified GPIO port

    API to read the level of a GPIO Port.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId        Index of the GPIO controller
    @param[in]    aPortId     GPIO port ID
    @param[Out]   aLevel      Pointer to GPIO port level
    @return       int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid port
    @return     #BCM_ERR_INVAL_PARAMS   NULL aLevel pointer
    @retval     #BCM_ERR_NODEV          GPIO instance not initialized

    @post None

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations None
*/
int32_t GPIO_DrvPortRead(GPIO_HwIDType hwId, GPIO_PortType aPortId, GPIO_PortLevelType *aLevel);

/** @brief Set GPIO port level

    API to set the level of a GPIO port.

    @note When this API is called, GPIO Channels that are not configured for
    GPIO output remains unchanged.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId        Index of the GPIO controller
    @param[in]   aPortId     GPIO port ID
    @param[in]   aLevel      Port Level to be written
    @return      int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid port
    @retval     #BCM_ERR_NODEV          GPIO instance not initialized

    @post None

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations None
*/
int32_t GPIO_DrvPortWrite(GPIO_HwIDType hwId, GPIO_PortType aPortId, GPIO_PortLevelType aLevel);

/** @brief Read level of GPIO channel group

    API to read a subset of channels.
    @note
    -# If the respective GPIO pin in the Channel Group is not configured for
       GPIO function, the level returned for that channel is always #GPIO_LEVEL_LOW.
    -# If the respective GPIO pin is configured as an output, the level
       returned for that channel reflects the actual level set as output on the GPIO pin.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]    hwId                Index of the GPIO controller
    @param[in]    aChannelGroupIdPtr  Pointer to a Channel group.
                                      @note If the pointer to channel group
                                      parameters is NULL, the value returned
                                      is #GPIO_LEVEL_LOW
    @param[Out]   aLevel             Pointer to GPIO port level
    @return     int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid port
    @return     #BCM_ERR_INVAL_PARAMS   NULL aLevel pointer
    @retval     #BCM_ERR_NODEV          GPIO instance not initialized

    @post None

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations  Maximum channel group size is 32 adjoining channels for a particular port
*/
int32_t GPIO_DrvChannelGroupRead(GPIO_HwIDType hwId, const GPIO_ChannelGroupType* aChannelGroupIdPtr, GPIO_PortLevelType *aLevel);

/** @brief Set GPIO channel group level

    API to set a subset of channels to a specified level.
    @note When this API is called, GPIO Channels in the Channel Group that are
    not configured for GPIO output remains unchanged.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId                Index of the GPIO controller
    @param[in]    aChannelGroupIdPtr  Pointer to a channel group
    @param[in]    aLevel              Channel group levels to be written
    @return       int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid port
    @retval     #BCM_ERR_NODEV          GPIO instance not initialized

    @post None

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations  Maximum channel group size is 32 adjoining channels for a particular port
*/
int32_t GPIO_DrvChannelGroupWrite(GPIO_HwIDType hwId, const GPIO_ChannelGroupType* aChannelGroupIdPtr, GPIO_PortLevelType aLevel);

/** @brief Flip GPIO channel level

    API to flip (change from High to Low or from Low to High) the level of a
    GPIO channel and return the levels of the channels after flip.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    hwId        Index of the GPIO controller
    @param[in]    aChannelId  ID of GPIO Channel
    @param[Out]   aLevel      Pointer to GPIO channel level
    @return       int32_t

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @return     #BCM_ERR_INVAL_PARAMS   Invalid port
    @return     #BCM_ERR_INVAL_PARAMS   NULL aLevel pointer
    @retval     #BCM_ERR_NODEV          GPIO instance not initialized

    @post None

    @trace  #BRCM_SWREQ_GPIO_INIT_CONFIG

    @limitations None
*/
int32_t GPIO_DrvChannelFlip(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_LevelType *aLevel);

#endif /* GPIO_H */
/** @} */
