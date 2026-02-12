/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_gpiodrv_impl GPIO Driver Design
    @ingroup grp_gpiodrv

    @addtogroup grp_gpiodrv_impl
    @{

    @file gpio_drv.c

    @version 0.1 Initial version
*/
#include <gpio.h>
#include <gpio_rdb.h>
#include <bcm_comp.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <compiler.h>

/**
    @name GPIO driver Design IDs
    @{
    @brief Design IDs for GPIO driver
*/
#define BRCM_SWDSGN_GPIO_DRV_DEV_TYPE                       (0xB101U)    /**< @brief #GPIO_DrvDevType              */
#define BRCM_SWDSGN_GPIO_DRV_DEV_GLOBAL                     (0xB102U)    /**< @brief #GPIO_DrvDev                  */
#define BRCM_SWDSGN_GPIO_REGS_GLOBAL                        (0xB103U)    /**< @brief #GPIO_Regs                    */

#define BRCM_SWDSGN_GPIO_INT_GET_PIN_LEVEL_PROC             (0xB104U)    /**< @brief #GPIO_IntGetPinLevel          */
#define BRCM_SWDSGN_GPIO_INT_GET_PORT_LEVEL_PROC            (0xB105U)    /**< @brief #GPIO_IntGetPortLevel         */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_LEVEL_PROC             (0xB106U)    /**< @brief #GPIO_IntSetPinLevel          */
#define BRCM_SWDSGN_GPIO_INT_SET_PORT_LEVEL_PROC            (0xB107U)    /**< @brief #GPIO_IntSetPortLevel         */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_MODE_PROC              (0xB108U)    /**< @brief #GPIO_IntSetPinMode           */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_OUTPUT_TYPE_PROC       (0xB109U)    /**< @brief #GPIO_IntSetPinOutputType     */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_PUPD_PROC              (0xB10AU)    /**< @brief #GPIO_IntSetPinPupd           */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_BIT_PROC               (0xB10BU)    /**< @brief #GPIO_IntSetPinBit            */
#define BRCM_SWDSGN_GPIO_INT_CLR_PIN_BIT_PROC               (0xB10CU)    /**< @brief #GPIO_IntClrPinBit            */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_ALTFUNC_PROC           (0xB10DU)    /**< @brief #GPIO_IntSetPinAltFunc        */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_HYSTERESIS_PROC        (0xB10EU)    /**< @brief #GPIO_IntSetPinHysteresis     */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_DRIVE_STRENGTH_PROC    (0xB10FU)    /**< @brief #GPIO_IntSetPinDriveStrength  */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_INPUT_PROC             (0xB110U)    /**< @brief #GPIO_IntSetPinInput          */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_SLEW_RATE_PROC         (0xB111U)    /**< @brief #GPIO_IntSetPinSlewRate       */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_DEGLITCH_FLT_CNT_PROC  (0xB112U)    /**< @brief #GPIO_IntSetPinDeglitchFltCnt */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_DEGLITCH_FLT_BYP_PROC  (0xB113U)    /**< @brief #GPIO_IntSetPinDeglitchFltByp */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_DIN_RAW_PROC           (0xB114U)    /**< @brief #GPIO_IntSetPinDinRaw         */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_DIN_FRC_PROC           (0xB115U)    /**< @brief #GPIO_IntSetPinDinFrc         */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_DIN_FRCVAL_PROC        (0xB116U)    /**< @brief #GPIO_IntSetPinDinFrcVal      */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_DIN_INVERT_PROC        (0xB117U)    /**< @brief #GPIO_IntSetPinDinInvert      */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_DOUT_INVERT_PROC       (0xB118U)    /**< @brief #GPIO_IntSetPinDoutInvert     */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_CONFIG_PROC            (0xB119U)    /**< @brief #GPIO_IntSetPinConfig         */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_CONFIG_GRP0_PROC       (0xB11AU)    /**< @brief #GPIO_IntSetPinConfigGrp0     */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_CONFIG_GRP1_PROC       (0xB11BU)    /**< @brief #GPIO_IntSetPinConfigGrp1     */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_CONFIG_GRP2_PROC       (0xB11CU)    /**< @brief #GPIO_IntSetPinConfigGrp2     */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_CONFIG_GRP3_PROC       (0xB11DU)    /**< @brief #GPIO_IntSetPinConfigGrp3     */
#define BRCM_SWDSGN_GPIO_INT_SET_PIN_CONFIG_GRP4_PROC       (0xB11EU)    /**< @brief #GPIO_IntSetPinConfigGrp4     */


#define BRCM_SWDSGN_GPIO_DRV_INIT_CHANNEL_PROC              (0xB130U)    /**< @brief #GPIO_DrvInitChannel          */
#define BRCM_SWDSGN_GPIO_DRV_INIT_PORT_PROC                 (0xB131U)    /**< @brief #GPIO_DrvInitPort             */
#define BRCM_SWDSGN_GPIO_DRV_DEINIT_PROC                    (0xB132U)    /**< @brief #GPIO_DrvDeInit               */
#define BRCM_SWDSGN_GPIO_DRV_SET_CHANNEL_CONFIG_PROC        (0xB133U)    /**< @brief #GPIO_DrvSetChannelConfig     */
#define BRCM_SWDSGN_GPIO_DRV_CHANNEL_READ_PROC              (0xB134U)    /**< @brief #GPIO_DrvChannelRead          */
#define BRCM_SWDSGN_GPIO_DRV_CHANNEL_WRITE_PROC             (0xB135U)    /**< @brief #GPIO_DrvChannelWrite         */
#define BRCM_SWDSGN_GPIO_DRV_PORT_READ_PROC                 (0xB136U)    /**< @brief #GPIO_DrvPortRead             */
#define BRCM_SWDSGN_GPIO_DRV_PORT_WRITE_PROC                (0xB137U)    /**< @brief #GPIO_DrvPortWrite            */
#define BRCM_SWDSGN_GPIO_DRV_CHANNEL_GROUP_READ_PROC        (0xB138U)    /**< @brief #GPIO_DrvChannelGroupRead     */
#define BRCM_SWDSGN_GPIO_DRV_CHANNEL_GROUP_WRITE_PROC       (0xB139U)    /**< @brief #GPIO_DrvChannelGroupWrite    */
#define BRCM_SWDSGN_GPIO_DRV_CHANNEL_FLIP_PROC              (0xB140U)    /**< @brief #GPIO_DrvChannelFlip          */

/** @} */

/**
    @trace #BRCM_SWARCH_GPIO_DRV_INIT_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_INIT_PORT_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_PORT_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_PORT_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_FLIP_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static GPIO_RDBType *const GPIO_Regs[GPIO_MAX_HW_ID] = {
#if (GPIO_MAX_HW_ID == 0)
#error "GPIO_MAX_HW_ID == 0"
#endif
    (GPIO_RDBType *) GPIO_CTRL_BASE,
#if (GPIO_MAX_HW_ID > 1)
#error "GPIO_MAX_HW_ID > 1 is not supported by the driver"
#endif /* GPIO_MAX_HW_ID > 1 */
};

/**
    @name GPIO Driver device type
    @{
    @brief GPIO Driver device type struct

    @trace #BRCM_SWARCH_GPIO_DRV_INIT_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_PORT_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_PORT_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_FLIP_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
 */
typedef struct sGPIO_DrvDevType {
    GPIO_StateType   state; /**< @brief State of the driver */
} GPIO_DrvDevType;
/** @} */

/**
    @brief GPIO hardware context instance

    @trace #BRCM_SWARCH_GPIO_DRV_INIT_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_PORT_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_PORT_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_READ_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_WRITE_PROC
    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_FLIP_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static GPIO_DrvDevType GPIO_DrvDev[GPIO_MAX_HW_ID] COMP_SECTION(".bss.drivers");

/**
    @brief Get GPIO pin Level

    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_READ_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntGetPinLevel(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_LevelType *lvl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t regIdx    = 0UL;
    uint32_t pinOffset = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (aPin >= GPIO_CHANNEL_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == lvl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
    regIdx    = aPin / GPIO_PINS_PER_PORT;
    pinOffset = aPin % GPIO_PINS_PER_PORT;

    *lvl = (((GPIO_Regs[hwId]->din[regIdx] >> pinOffset) & GPIO_PINS_ONE_BIT_ACC_MASK) > 0UL ?
              GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Get GPIO port Level

    @trace #BRCM_SWARCH_GPIO_DRV_PORT_READ_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntGetPortLevel(GPIO_HwIDType hwId, GPIO_PortType aPort, GPIO_PortLevelType *lvl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t regIdx = (uint32_t)aPort;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (aPort >= GPIO_PORT_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == lvl) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        *lvl = (GPIO_PortLevelType)(GPIO_Regs[hwId]->din[regIdx]);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set GPIO pin Level

    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_WRITE_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinLevel(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_LevelType aLevel)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;
        GPIO_Regs[hwId]->dout[regIdx] = (GPIO_LEVEL_LOW == aLevel) ?
                    (GPIO_Regs[hwId]->dout[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->dout[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO port Level

    @trace #BRCM_SWARCH_GPIO_DRV_PORT_WRITE_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPortLevel(GPIO_HwIDType hwId, GPIO_PortType aPort, GPIO_PortLevelType aLevel)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t regIdx = (uint32_t)aPort ;
#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPort >= GPIO_PORT_MAX)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        GPIO_Regs[hwId]->dout[regIdx] = aLevel;

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin mode (Input/Output/Alternate)

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinMode(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgModeType mode)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx    = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(mode >= GPIO_CFG_MODE_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = (aPin / (GPIO_PINS_PER_PORT / 2UL));
        pinOffset = (2UL * (aPin % (GPIO_PINS_PER_PORT / 2UL)));
        GPIO_Regs[hwId]->mode[regIdx] &= ~(GPIO_PINS_TWO_BIT_ACC_MASK << pinOffset);
        GPIO_Regs[hwId]->mode[regIdx] |= ((mode & GPIO_PINS_TWO_BIT_ACC_MASK) << pinOffset);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin output type (Push-pull/open drain)

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinOutputType(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgOutputType oType)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx    = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(oType >= GPIO_CFG_OUTPUT_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->otype[regIdx] = (GPIO_CFG_OUTPUT_PUSH_PULL == oType) ?
                    (GPIO_Regs[hwId]->otype[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->otype[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin pull-up, pull-down control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinPupd(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgPuPdType pupd)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(pupd >= GPIO_CFG_PUPD_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = (aPin / (GPIO_PINS_PER_PORT / 2UL));
        pinOffset = (2UL * (aPin % (GPIO_PINS_PER_PORT / 2UL)));
        GPIO_Regs[hwId]->pupd[regIdx] &= ~(GPIO_PINS_TWO_BIT_ACC_MASK << pinOffset);
        GPIO_Regs[hwId]->pupd[regIdx] |= ((pupd & GPIO_PINS_TWO_BIT_ACC_MASK) << pinOffset);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin bit set control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinBit(GPIO_HwIDType hwId, GPIO_ChannelType aPin)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;
        GPIO_Regs[hwId]->bset[regIdx] = (GPIO_Regs[hwId]->bset[regIdx] |
                                        (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin bit clear control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntClrPinBit(GPIO_HwIDType hwId, GPIO_ChannelType aPin)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;
        GPIO_Regs[hwId]->bclr[regIdx] = (GPIO_Regs[hwId]->bclr[regIdx] |
                                        (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin alternate function control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinAltFunc(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgAltFuncType altfunc)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(altfunc >= GPIO_CFG_ALT_FUNC_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = (aPin / 10UL);
        pinOffset = (3UL * (aPin % 10UL)) ;
        GPIO_Regs[hwId]->altfunc[regIdx] &= ~(GPIO_PINS_THREE_BIT_ACC_MASK << pinOffset);
        GPIO_Regs[hwId]->altfunc[regIdx] |= ((altfunc & GPIO_PINS_THREE_BIT_ACC_MASK) << pinOffset);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin input hysteresis control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinHysteresis(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgHysteresisType hys)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->hys_en[regIdx] = (GPIO_CFG_HYSTERESIS_DISABLE == hys) ?
                    (GPIO_Regs[hwId]->hys_en[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->hys_en[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin drive strength control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinDriveStrength(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgDriveStrengthType drvStrength)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(drvStrength >= GPIO_CFG_DRIVE_STRENGTH_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = (aPin / 10UL);
        pinOffset = (3UL * (aPin % 10UL)) ;
        GPIO_Regs[hwId]->sel[regIdx] &= ~(GPIO_PINS_THREE_BIT_ACC_MASK << pinOffset);
        GPIO_Regs[hwId]->sel[regIdx] |= ((drvStrength & GPIO_PINS_THREE_BIT_ACC_MASK) << pinOffset);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set GPIO pin input disable control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinInput(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgInputType ind)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->ind[regIdx] = (GPIO_CFG_INPUT_ENABLE == ind) ?
                    (GPIO_Regs[hwId]->ind[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->ind[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set GPIO pin slew rate control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinSlewRate(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgSlewRateType src)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->src[regIdx] = (GPIO_CFG_SLEW_RATE_FAST == src) ?
                    (GPIO_Regs[hwId]->src[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->src[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set GPIO pin deglitch filter count control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinDeglitchFltCnt(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgDeglitchCntType dg_cnt)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(dg_cnt >= GPIO_CFG_DEGLITCH_CNT_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = (aPin / 10UL);
        pinOffset = (3UL * (aPin % 10UL)) ;
        GPIO_Regs[hwId]->dg_cnt[regIdx] &= ~(GPIO_PINS_THREE_BIT_ACC_MASK << pinOffset);
        GPIO_Regs[hwId]->dg_cnt[regIdx] |= ((dg_cnt & GPIO_PINS_THREE_BIT_ACC_MASK) << pinOffset);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin deglitch filter bypass control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinDeglitchFltByp(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgDeglitchBypassType dg_bypass)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->src[regIdx] = (GPIO_CFG_DEGLITCH_BYPASS_DISABLE == dg_bypass) ?
                    (GPIO_Regs[hwId]->dg_bypass[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->dg_bypass[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin din raw control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinDinRaw(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgDinRawType din_raw)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->din_raw[regIdx] = (GPIO_CFG_DIN_RAW_SYNC == din_raw) ?
                    (GPIO_Regs[hwId]->din_raw[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->din_raw[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin din force control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinDinFrc(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgDinForceType din_frc)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->din_frc[regIdx] = (GPIO_CFG_DIN_FORCE_NONE == din_frc) ?
                    (GPIO_Regs[hwId]->din_frc[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->din_frc[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin din force value control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinDinFrcVal(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgDinForceValType din_frcval)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->din_frcval[regIdx] = (GPIO_CFG_DIN_FORCE_VAL_LOW == din_frcval) ?
                    (GPIO_Regs[hwId]->din_frcval[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->din_frcval[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin din invert control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinDinInvert(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgDinInvertType din_inv)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->din_inv[regIdx] = (GPIO_CFG_DIN_INVERT_DISABLE == din_inv) ?
                    (GPIO_Regs[hwId]->din_inv[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->din_inv[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Set GPIO pin dout invert control

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinDoutInvert(GPIO_HwIDType hwId, GPIO_ChannelType aPin, GPIO_CfgDoutInvertType dout_inv)
{
    int32_t ret = BCM_ERR_OK ;
    uint32_t regIdx = 0UL;
    uint32_t pinOffset = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if((hwId >= GPIO_MAX_HW_ID) || (aPin >= GPIO_CHANNEL_MAX)){
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        regIdx    = aPin / GPIO_PINS_PER_PORT;
        pinOffset = aPin % GPIO_PINS_PER_PORT;

        GPIO_Regs[hwId]->dout_inv[regIdx] = (GPIO_CFG_DOUT_INVERT_DISABLE == dout_inv) ?
                    (GPIO_Regs[hwId]->dout_inv[regIdx] & (~(GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset))) :
                    (GPIO_Regs[hwId]->dout_inv[regIdx] | (GPIO_PINS_ONE_BIT_ACC_MASK << pinOffset));

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Configure specified GPIO pin

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinConfigGrp0(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if(0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_MODE)) {
        ret = GPIO_IntSetPinMode(hwId, aChannelId, aConfig->mode);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_OTYPE))) {
        ret = GPIO_IntSetPinOutputType(hwId, aChannelId, aConfig->oType);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_PUPD))) {
        ret = GPIO_IntSetPinPupd(hwId, aChannelId, aConfig->pupd);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_DOUT))) {
        ret = GPIO_IntSetPinLevel(hwId, aChannelId, aConfig->dout);
    }
    return ret ;
}

/**
    @brief Configure specified GPIO pin

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinConfigGrp1(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if(0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_BSET)) {
        ret = GPIO_IntSetPinBit(hwId, aChannelId);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_BCLR))) {
        ret = GPIO_IntClrPinBit(hwId, aChannelId);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_ALTF))) {
        ret = GPIO_IntSetPinAltFunc(hwId, aChannelId, aConfig->altFunc);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_HIST))) {
        ret = GPIO_IntSetPinHysteresis(hwId, aChannelId, aConfig->hys);
    }
    return ret ;
}

/**
    @brief Configure specified GPIO pin

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinConfigGrp2(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if(0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_SEL)) {
        ret = GPIO_IntSetPinDriveStrength(hwId, aChannelId, aConfig->strength);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_IND))) {
        ret = GPIO_IntSetPinInput(hwId, aChannelId, aConfig->ind);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_SRC))) {
        ret = GPIO_IntSetPinSlewRate(hwId, aChannelId, aConfig->slewRate);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_DG_CNT))) {
        ret = GPIO_IntSetPinDeglitchFltCnt(hwId, aChannelId, aConfig->dgCnt);
    }
    return ret ;
}

/**
    @brief Configure specified GPIO pin

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinConfigGrp3(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if(0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_DG_BYP)) {
        ret = GPIO_IntSetPinDeglitchFltByp(hwId, aChannelId, aConfig->dgByp);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_DIN_RAW))) {
        ret = GPIO_IntSetPinDinRaw(hwId, aChannelId, aConfig->dinRawSync);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_DIN_FRC))) {
        ret = GPIO_IntSetPinDinFrc(hwId, aChannelId, aConfig->dinFrc);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_DIN_FRCVAL))) {
        ret = GPIO_IntSetPinDinFrcVal(hwId, aChannelId, aConfig->dinFrcVal);
    }
    return ret ;
}

/**
    @brief Configure specified GPIO pin

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinConfigGrp4(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if(0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_DIN_INV)) {
        ret = GPIO_IntSetPinDinInvert(hwId, aChannelId, aConfig->dinInvert);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_DOUT_INV))) {
        ret = GPIO_IntSetPinDoutInvert(hwId, aChannelId, aConfig->doutInvert);
    }
    return ret ;
}

/**
    @brief Configure specified GPIO pin

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
static int32_t GPIO_IntSetPinConfig(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;

    if(0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_GRP_0)) {
        ret = GPIO_IntSetPinConfigGrp0(hwId, aChannelId, aConfig);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_GRP_1))) {
        ret = GPIO_IntSetPinConfigGrp1(hwId, aChannelId, aConfig);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_GRP_2))) {
        ret = GPIO_IntSetPinConfigGrp2(hwId, aChannelId, aConfig);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_GRP_3))) {
        ret = GPIO_IntSetPinConfigGrp3(hwId, aChannelId, aConfig);
    }

    if((ret == BCM_ERR_OK) && (0UL != (aConfig->aCfgMask & GPIO_CFG_MASK_GRP_4))) {
        ret = GPIO_IntSetPinConfigGrp4(hwId, aChannelId, aConfig);
    }
    return ret ;
}

/**
    @brief Initialize specified GPIO channel

    @code{.unparsed}
    Check hardware instance with max supported device.
    Initialize the pin based on specified configuration
    Move state to GPIO_STATE_INITIALIZED state.
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_INIT_CHANNEL_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvInitChannel(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(aChannelId >= GPIO_CHANNEL_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(NULL == aConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        ret = GPIO_IntSetPinConfig(hwId, aChannelId, aConfig);

        if(ret == BCM_ERR_OK) {
            GPIO_DrvDev[hwId].state = GPIO_STATE_INITIALIZED;
        } else {
            GPIO_DrvDev[hwId].state = GPIO_STATE_UNINIT;
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Initialize specified GPIO Port

    @code{.unparsed}
    Check hardware instance with max supported device.
    Initialize the port based on specified configuration
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_INIT_PORT_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvInitPort(GPIO_HwIDType hwId, const GPIO_PortType aPortId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;
    GPIO_ChannelType ch_start = 0UL, ch_end = 0UL, ch_id = 0UL;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(aPortId >= GPIO_PORT_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(NULL == aConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        if(aPortId == GPIO_PORT_0) {
            ch_start = GPIO_CHANNEL_0 ; ch_end = GPIO_CHANNEL_31;
        } else if(aPortId == GPIO_PORT_1) {
            ch_start = GPIO_CHANNEL_32; ch_end = GPIO_CHANNEL_63;
        } else {
            ch_start = GPIO_CHANNEL_64; ch_end = (GPIO_CHANNEL_MAX - 1UL);
        }
        for(ch_id = ch_start; ch_id <= ch_end; ch_id++) {
            ret = GPIO_DrvInitChannel(hwId, ch_id, aConfig) ;
            if(ret != BCM_ERR_OK) {
                break ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief De-initialize GPIO module

    @code{.unparsed}
    Check hardware instance with max supported device.
    Move state to GPIO_STATE_UNINIT.
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvDeInit(GPIO_HwIDType hwId)
{
    int32_t ret = BCM_ERR_OK ;

#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
        GPIO_DrvDev[hwId].state = GPIO_STATE_UNINIT;
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set configuration to specified GPIO channel

    @code{.unparsed}
    Check hardware instance with max supported device.
    Sets the channel based on specified configuration
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_SET_CHANNEL_CONFIG_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvSetChannelConfig(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK ;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (aChannelId >= GPIO_CHANNEL_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else if (NULL == aConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
#endif
    ret = GPIO_IntSetPinConfig(hwId, aChannelId, aConfig);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Read level of specified GPIO channel

    @code{.unparsed}
    Check hardware instance with max supported device.
    Read level of specified GPIO channel
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_READ_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvChannelRead(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_LevelType *aLevel)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if(hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (aChannelId >= GPIO_CHANNEL_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == aLevel) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
    ret = GPIO_IntGetPinLevel(hwId, aChannelId, aLevel);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set the level of a GPIO channel

    @code{.unparsed}
    Check hardware instance with max supported device.
    Write level of specified GPIO channel
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_WRITE_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvChannelWrite(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_LevelType aLevel)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(aChannelId >= GPIO_CHANNEL_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
    ret = GPIO_IntSetPinLevel(hwId, aChannelId, aLevel);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Read level of specified GPIO port

    @code{.unparsed}
    Check hardware instance with max supported device.
    Read level of specified GPIO port
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_PORT_READ_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvPortRead(GPIO_HwIDType hwId, GPIO_PortType aPortId, GPIO_PortLevelType *aLevel)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(aPortId >= GPIO_PORT_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == aLevel) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ret = GPIO_IntGetPortLevel(hwId, aPortId, aLevel);
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set GPIO port level

    @code{.unparsed}
    Check hardware instance with max supported device.
    Write level of specified GPIO port
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_PORT_WRITE_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvPortWrite(GPIO_HwIDType hwId, GPIO_PortType aPortId, GPIO_PortLevelType aLevel)
{
    int32_t ret = BCM_ERR_OK;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(aPortId >= GPIO_PORT_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ret = GPIO_IntSetPortLevel(hwId, aPortId, aLevel);

#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Read level of GPIO channel group

    @code{.unparsed}
    Check hardware instance with max supported device.
    Read level of GPIO channel group
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_READ_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvChannelGroupRead(GPIO_HwIDType hwId, const GPIO_ChannelGroupType* aChannelGroupIdPtr, GPIO_PortLevelType *aLevel)
{
    int32_t ret = BCM_ERR_OK;
    GPIO_PortLevelType portLvl = 0UL;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == aChannelGroupIdPtr) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if ((aChannelGroupIdPtr->port >= GPIO_PORT_MAX) ||
                (aChannelGroupIdPtr->offset >= GPIO_PINS_PER_PORT)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(NULL == aLevel) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ret = GPIO_IntGetPortLevel(hwId, aChannelGroupIdPtr->port, &portLvl);
        *aLevel = ((portLvl & (aChannelGroupIdPtr->mask << (aChannelGroupIdPtr->offset & 0x1FU))) >> (aChannelGroupIdPtr->offset & 0x1FU));
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/**
    @brief Set GPIO channel group level

    @code{.unparsed}
    Check hardware instance with max supported device.
    Write level of GPIO channel group
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_GROUP_WRITE_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvChannelGroupWrite(GPIO_HwIDType hwId, const GPIO_ChannelGroupType* aChannelGroupIdPtr, GPIO_PortLevelType aLevel)
{
    int32_t ret = BCM_ERR_OK ;
    GPIO_PortLevelType currentLvl;

#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (NULL == aChannelGroupIdPtr) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if ((aChannelGroupIdPtr->port >= GPIO_PORT_MAX) ||
               (aChannelGroupIdPtr->offset >= GPIO_PINS_PER_PORT)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ret = GPIO_IntGetPortLevel(hwId, aChannelGroupIdPtr->port, &currentLvl);
        if(BCM_ERR_OK == ret) {
            currentLvl &= ~((aChannelGroupIdPtr->mask) << (aChannelGroupIdPtr->offset & 0x1FU));
            currentLvl |= ((aLevel & aChannelGroupIdPtr->mask) << (aChannelGroupIdPtr->offset & 0x1FU));
            ret = GPIO_IntSetPortLevel(hwId, aChannelGroupIdPtr->port, currentLvl);
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret ;
}

/**
    @brief Flip GPIO channel level

    @code{.unparsed}
    Check hardware instance with max supported device and channel ID
    Get the current level of the specified channel.
    Invert channel state.
    set and get the new channel state
    @endcode

    @trace #BRCM_SWARCH_GPIO_DRV_CHANNEL_FLIP_PROC
    @trace #BRCM_SWREQ_GPIO_INIT_CONFIG
*/
int32_t GPIO_DrvChannelFlip(GPIO_HwIDType hwId, GPIO_ChannelType aChannelId, GPIO_LevelType *aLevel)
{
    int32_t ret = BCM_ERR_OK ;
    GPIO_LevelType lvl = GPIO_LEVEL_LOW;
    GPIO_LevelType newLvl;
#ifdef BCM8915X_PARAM_VALIDATION
    if (hwId >= GPIO_MAX_HW_ID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(aChannelId >= GPIO_CHANNEL_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
    }  else if(GPIO_DrvDev[hwId].state == GPIO_STATE_UNINIT) {
        ret = BCM_ERR_NODEV;
    } else {
#endif
        ret = GPIO_IntGetPinLevel(hwId, aChannelId, &lvl);
        if(BCM_ERR_OK == ret) {
            newLvl = (GPIO_LEVEL_LOW == lvl) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
            ret = GPIO_IntSetPinLevel(hwId, aChannelId, newLvl);
            if(ret == BCM_ERR_OK) {
                ret = GPIO_IntGetPinLevel(hwId, aChannelId, &lvl);
                *aLevel = lvl ;
            }
        }
#ifdef BCM8915X_PARAM_VALIDATION
    }
#endif
    return ret;
}

/** @} */
