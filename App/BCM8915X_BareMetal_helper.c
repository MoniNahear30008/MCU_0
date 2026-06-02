/*****************************************************************************
 *  Copyright 2024 Broadcom Corporation.  All rights reserved.
 *
 *  Broadcom Proprietary and Confidential
 ****************************************************************************/

#include "BCM8915X_BareMetal_helper.h"
#include <bcm_utils.h>
#include "BCM8915x_CM7.h"
#include <hsafe.h>
#include <acqcmn.h>
#include <hsadc.h>
#include <gpio.h>

void reg_rd(uint32_t addr, uint32_t *data)
{
    *data = *(volatile uint32_t *)addr ;
}

void reg_wr(uint32_t addr, uint32_t data)
{
    *(volatile uint32_t *)(addr) = data;
}

void reg_rmw(uint32_t addr, uint8_t msb, uint8_t lsb, uint32_t value)
{
    uint32_t reg_val = 0;
    uint32_t width = msb - lsb + 1;
    uint32_t mask = ((1UL << width) - 1) << lsb;

    value <<= lsb ;
    value &= mask ;
    reg_rd(addr, &reg_val);

    reg_val &= ~mask;

    reg_val |= value;

    reg_wr(addr, reg_val);
}

int32_t byte_compare(uint8_t *buf1, uint8_t *buf2, uint32_t size)
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t i;

    for (i = 0U; i < size; i++) {
        if (buf1[i] != buf2[i]) {
            retVal = BCM_ERR_DATA_INTEG;
            break;
        } else {
            retVal = BCM_ERR_OK;
        }
    }

    return retVal;
}

int32_t test_helper_byte_compare(uint8_t *buf1, uint8_t *buf2, uint32_t size)
{
    BCM_ErrorType retVal;

    retVal = byte_compare(buf1, buf2, size);
    if (retVal == BCM_ERR_OK) {
        retVal = BCM_ERR_OK;
    } else {
        retVal = BCM_ERR_DATA_INTEG;
    }

    return retVal;
}

int32_t test_helper_wfi_verify_data(uint8_t *buf1, uint8_t *buf2, uint32_t size, volatile uint8_t *flag)
{
    BCM_ErrorType retVal;

    while (1) {
        if (*flag) {
            retVal = byte_compare(buf1, buf2, size);
            break;
        }
        else {
            /* Wait for Interrupt */
            __WFI();
        }
    }
    if (retVal == BCM_ERR_OK) {
        retVal = BCM_ERR_OK;
    } else {
        retVal = BCM_ERR_DATA_INTEG;
    }

    return retVal;
}


/*Function to bringup AFE and HSACQ CMN block*/
int32_t bringup_hsafe(uint32_t hsafe_id, uint32_t hsacq_cmn_id, uint8_t hsadc_5g_mode, uint8_t ocpClkSel, uint8_t fftClkSel) 
{
    uint8_t RescalStatus = 0;
    uint8_t Valid = 0; 
    uint8_t pllLock = 0;
    uint8_t calStatus=0, Value = 0;
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

    /*Initialize Driveers*/
    CHK_RETVAL(retVal = ACQCMN_DrvInit(hsacq_cmn_id));

    CHK_RETVAL(retVal = HSAFE_DrvInit(hsafe_id));

    /*Enable Clocks*/
    CHK_RETVAL(retVal = ACQCMN_DrvClkCtrl(hsacq_cmn_id, ACQCMN_CLK_ALL, 1));

    /*Rescal Configuration*/
    CHK_RETVAL(retVal = HSAFE_DrvRescalResetb(hsafe_id, 0));
    BCM_DelayUs(20000);

    CHK_RETVAL(retVal = HSAFE_DrvRescalResetb(hsafe_id, 1));
    BCM_DelayUs(20000);

    CHK_RETVAL(retVal = HSAFE_DrvRescalPowerConfig(hsafe_id, 0));

    /*Check for Rescal Status*/
    do {
        CHK_RETVAL(retVal = HSAFE_DrvRescalStatus(hsafe_id, &RescalStatus, &Valid, &Value));
        BCM_DelayUs(100);
    } while (RescalStatus ==0);

    /*Configure PLL - Wait for PLL Lock*/
    CHK_RETVAL(retVal = HSAFE_DrvPllConfig(hsafe_id));
    BCM_DelayUs(2000);

    do {
        CHK_RETVAL(retVal = HSAFE_DrvPllLockGet(hsafe_id, &pllLock));
    } while (pllLock == 0);

    /*Release Rescal Reset. Wait for Rescal Done*/
    CHK_RETVAL(retVal = HSAFE_DrvRccalReset(hsafe_id, 0));

    do {
        CHK_RETVAL(retVal = HSAFE_DrvRcCalStatus(hsafe_id, &calStatus));
        BCM_DelayUs(2000);
    } while (calStatus==0);

    for (int hsadc_inst=0; hsadc_inst < 4; hsadc_inst+=1) {
        if (hsadc_5g_mode & (0x1<<hsadc_inst)) {
            CHK_RETVAL(retVal = HSAFE_DrvHsAdcConfig(hsafe_id, hsadc_inst, HSADC_SAMPLING_MODE_5G));
        } else {
            CHK_RETVAL(retVal = HSAFE_DrvHsAdcConfig(hsafe_id, hsadc_inst, HSADC_SAMPLING_MODE_1P25G));
        }
    }

    /*Release HSADC_HSAFE_Pll Reset*/
    CHK_RETVAL(retVal = HSAFE_DrvPllClkGenRstb(hsafe_id, 1));

    /*Release HSADC Reset*/
    for (int hsadc_inst=0; hsadc_inst<4; hsadc_inst+=1) {
        CHK_RETVAL(retVal = HSAFE_DrvHsAdcReset(hsafe_id, hsadc_inst, 1));
    }

    /*Release HSREF Reset*/
    CHK_RETVAL(retVal = HSAFE_DrvHsRefReset(hsafe_id, 1));

    BCM_DelayUs(2000);

    CHK_RETVAL(retVal = HSAFE_DrvOcpClockCfg(hsafe_id, ocpClkSel, fftClkSel));

    BCM_DelayUs(2000);

    /*Enable ADC Clocks*/
    CHK_RETVAL(retVal = HSAFE_DrvAdcClkCfg(hsafe_id, 1));

    BCM_DelayUs(2000);

err:
    return retVal;

}


/* Configure GPIO PIN as UART0 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_uart0()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .oType = GPIO_CFG_OUTPUT_PUSH_PULL,
        .pupd = GPIO_CFG_PUPD_PULL_UP,
        .hys = GPIO_CFG_HYSTERESIS_DISABLE,
        .strength = GPIO_CFG_DRIVE_STRENGTH_8MA,
        .doutInvert = GPIO_CFG_DOUT_INVERT_DISABLE,
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_OTYPE | GPIO_CFG_MASK_ALTF | GPIO_CFG_MASK_PUPD | GPIO_CFG_MASK_HIST | GPIO_CFG_MASK_SEL | GPIO_CFG_MASK_DOUT_INV
    };
    /* From Rigel_gpio_Ctrl.xlsx, find in which peripheral is mapped GPIO_30 as SDA & GPIO_31 as SCL*/

    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_30, &ch_cfg);
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_31, &ch_cfg);

    return retVal;
}

/* Configure GPIO PIN as UART1 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_uart1()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .oType = GPIO_CFG_OUTPUT_PUSH_PULL,
        .pupd = GPIO_CFG_PUPD_PULL_UP,
        .hys = GPIO_CFG_HYSTERESIS_DISABLE,
        .strength = GPIO_CFG_DRIVE_STRENGTH_8MA,
        .doutInvert = GPIO_CFG_DOUT_INVERT_DISABLE,
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_OTYPE | GPIO_CFG_MASK_ALTF | GPIO_CFG_MASK_PUPD | GPIO_CFG_MASK_HIST | GPIO_CFG_MASK_SEL | GPIO_CFG_MASK_DOUT_INV
    };
    /* From Rigel_gpio_Ctrl.xlsx, find in which peripheral is mapped GPIO_32 as SDA & GPIO_33 as SCL*/

    retVal = GPIO_DrvInitChannel(0, GPIO_CHANNEL_32, &ch_cfg);
    retVal = GPIO_DrvInitChannel(0, GPIO_CHANNEL_33, &ch_cfg);

    return retVal;
}

/* Configure GPIO PIN as I2C0 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_i2c0()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* From Rigel_gpio_Ctrl.xlsx, find in which peripheral is mapped GPIO_34 as SDA & GPIO_35 as SCL*/
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_34, &ch_cfg); 
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_35, &ch_cfg); 

    return retVal;
}

/* Configure GPIO PIN as I2C1 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_i2c1()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* From Rigel_gpio_Ctrl.xlsx, find in which peripheral is mapped GPIO_36 as SDA & GPIO_37 as SCL*/
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_36, &ch_cfg);
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_37, &ch_cfg);

    return retVal;
}

/* Configure GPIO PIN as CAN - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_can()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* From Rigel_gpio_Ctrl.xlsx, find in which peripheral is mapped GPIO_38 as CAN_TX & GPIO_39 as CAN_RX*/
    retVal = GPIO_DrvInitChannel(0, GPIO_CHANNEL_38, &ch_cfg);
    retVal = GPIO_DrvInitChannel(0, GPIO_CHANNEL_39, &ch_cfg);
    return retVal;
}

/* Configure GPIO PIN as TMR0_ETR - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu0_etr()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_53 as TMR0_ETR */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_53, &ch_cfg);
    return retVal;
}

/* Configure GPIO PIN as TMR0_IN0 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu0_in0()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_54 as TMR0_IN0 */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_54, &ch_cfg);
    return retVal;
}

/* Configure GPIO PIN as TMR0_IN1 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu0_in1()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_55 as TMR0_IN1 */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_55, &ch_cfg);
    return retVal;
}

/* Configure GPIO PIN as TMR1_ETR - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu1_etr()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_56 as TMR1_ETR */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_56, &ch_cfg);
    return retVal;
}

/* Configure GPIO PIN as TMR1_IN0 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu1_in0()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_57 as TMR1_IN0 */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_57, &ch_cfg);
    return retVal;
}

/* Configure GPIO PIN as TMR1_IN1 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu1_in1()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_58 as TMR1_IN1 */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_58, &ch_cfg);
    return retVal;
}

/* Configure GPIO PIN as TMR0_COUT0 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu0_cout0()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_59 as TMR0_COUT0 */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_59, &ch_cfg);

   return retVal;
}

/* Configure GPIO PIN as TMR0_COUT1 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu0_cout1()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_60 as TMR0_COUT1 */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_60, &ch_cfg);
    return retVal;
}

/* Configure GPIO PIN as TMR1_COUT0 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu1_cout0()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_61 as TMR1_COUT0 */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_61, &ch_cfg);

    return retVal;
}

/* Configure GPIO PIN as TMR0_COUT1 - using Alternate function setting */
int32_t cfg_gpio_alt_fn_as_ccu1_cout1()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    GPIO_ConfigType ch_cfg = {
        .mode = GPIO_CFG_MODE_ALT_FUNC,
        .altFunc = GPIO_CFG_ALT_FUNC_1, /* From Rigel_gpio_ctrl.xlsx, find in which Alternate function this peripheral is present */
        .aCfgMask = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_ALTF
    };
    /* GPIO_62 as TMR1_COUT1 */
    retVal = GPIO_DrvInitChannel(0UL, GPIO_CHANNEL_62, &ch_cfg);

    return retVal;
}

/* TC10 handle interrupts */
void Helper_TC10HandleInterrupts(Helper_WakeupEventType *const aEvents, Helper_TC10PortContextType *aCntxt)
{
    Helper_BooleanType clearIsLocalWakeUpFlag = HELPER_BOOLEAN_FALSE;
    uint32_t statusRegVal = 0U;
    uint32_t aCtrlRegVal  = 0U;
    uint32_t ctrlRegVal   = 0U;
    uint32_t ctrl2RegVal  = 0U;

    reg_rd((0xE0566000UL + 0x1fc0), &aCtrlRegVal);
    reg_rd((0xE0566000UL + 0x1fc4), &statusRegVal);
    reg_rd((0xE0566000UL + 0x1fcc), &ctrl2RegVal);

    ctrlRegVal = aCtrlRegVal ;

    /* Mask AFE wake-up event if it wasn't enabled */
    if (0U == (aCtrlRegVal & BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK)) {
        statusRegVal &= (uint16_t)(~(uint16_t)BRPHY1GV2_TEST_TC10_STATUS_REG1_AFE_WVMDI_LAT_MASK);
    }

    /* If MDI wake is not enabled and remote wake, clear the remote wake request */
    if ((0U != (ctrl2RegVal & BRPHY1GV2_TEST_TC10_CONTROL_2_REG1_WAKE_BY_WUP_CF_MASK)) &&
        (HELPER_BOOLEAN_FALSE == aCntxt->isLocalWakeUp)) {
        statusRegVal &= (uint16_t)(~(uint16_t)(BRPHY1GV2_TEST_TC10_STATUS_REG1_WAKEUP_LAT_MASK |
                                              BRPHY1GV2_TEST_TC10_STATUS_REG1_AFE_WVMDI_LAT_MASK));
        ctrlRegVal &= (uint16_t)(~(uint16_t)BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK);
    }

    /* If sleep abort is enabled, don't send sleep event (otherwise TX may gets disabled) */
    if (0U != (aCtrlRegVal & BRPHY1GV2_TEST_TC10_CONTROL_REG1_SLEEPABORT_REQ_MASK)) {
        statusRegVal &= (uint16_t)~(uint16_t)BRPHY1GV2_TEST_TC10_STATUS_REG1_SLEEP_LAT_MASK;
    }

    /* AFE Wake-Up Pulse */
    if (0U != (statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_AFE_WVMDI_LAT_MASK)) {
        aCntxt->stats.tc10AfeWakeUpPulse++;
        /* disable AFE wake-up event for two reasons:
           1. for every remote wake-up, two AFE_WVMDI_LAT events are observed
              => ignore the second.
           2. AFE_WVMDI_LAT event is generated on all other brphy ports that
              are awake => mask off AFE_WVMDI_LAT upon first occurence */
        ctrlRegVal &= (uint16_t)(~(uint16_t)BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK);

        if (HELPER_BOOLEAN_TRUE == aCntxt->isLocalWakeUp) {
            /* self triggered, AFE wake-up */
            aCntxt->stats.tc10LocalAfeWakeUp++;

            /* In hypothetical cases,
                1. where AFE is awakened locally and a WUP is received
                   from remote too,(and both are processed together)
                   remote's WUP is lost
                2. where AFE is awakened locally and a WUR is received
                   from remote,(and both are processed together), WUR
                   is recognised below and wakeUpReason gets
                   updated accordingly.
                Hence it is important to maintain the order of
                processing events in this routine
            */
            aCntxt->wakeReason = HELPER_WAKEUPREASON_LOCAL;
            *aEvents |= HELPER_WAKEUP_EVENT_LOCAL_WAKEUP;

            /* update isLocalWakeUp variable at the end of the routine as it may be needed for other decisions */
            clearIsLocalWakeUpFlag = HELPER_BOOLEAN_TRUE;
        } else {
            aCntxt->stats.tc10RemoteAfeWakeUp++;

            /* MDI or WUP? */
            if (0U == (statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_AFE_WVWAKE_LAT_MASK)) {
                /* WUP received from remote */
                aCntxt->wakeReason = HELPER_WAKEUPREASON_REMOTE;
                *aEvents |= HELPER_WAKEUP_EVENT_REMOTE_WAKEUP;
            } else {
                /* PIN based */
                aCntxt->wakeReason = HELPER_WAKEUPREASON_PIN;
                *aEvents |= HELPER_WAKEUP_EVENT_REMOTE_WAKEUP;
            }
        }
    }

    /* Wake-up Request */
    if (0U != (statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_WAKEUP_LAT_MASK)) {
        /* wake-up request */
        *aEvents |= HELPER_WAKEUP_EVENT_REMOTE_WAKEUP;
    }

    aCntxt->status.tc10State = (uint8_t)((statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_SLP_WUP_STATE_MASK)
                            >> BRPHY1GV2_TEST_TC10_STATUS_REG1_SLP_WUP_STATE_SHIFT);

    if (0U != (statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_SLEEPFAIL_LAT_MASK)) {
        aCntxt->stats.tc10SleepFail++;
        *aEvents |= HELPER_WAKEUP_EVENT_SLEEP_FAIL;
    }
    if (0U != (statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_SLEEP_LAT_MASK)) {
        aCntxt->stats.tc10Sleep++;
        *aEvents |= HELPER_WAKEUP_EVENT_SLEEP;
    }
    if (0U == (statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_INHIBIT_LAT_MASK)) {
        aCntxt->stats.tc10Inhibit++;

        /* Reset the sw cache so that any future WUP from remote are not misinterpreted */
        clearIsLocalWakeUpFlag = HELPER_BOOLEAN_TRUE;

        /* enable AFE wake-up event */
        ctrlRegVal |= BRPHY1GV2_TEST_TC10_CONTROL_REG1_AFE_WUP_INTR_MASK;
        aCntxt->wakeReason = HELPER_WAKEUPREASON_NONE;
        *aEvents |= HELPER_WAKEUP_EVENT_INHIBIT;
    }
    if (0U != (statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_WURFAIL_LAT_MASK)) {
        aCntxt->stats.tc10WakeUpFail++;
        *aEvents |= HELPER_WAKEUP_EVENT_REMOTE_WAKE_FAIL;
    }
    if (0U != (statusRegVal & BRPHY1GV2_TEST_TC10_STATUS_REG1_WAKEUP_LAT_MASK)) {
        aCntxt->stats.tc10RemoteWakeUpRequest++;
    }

    if (HELPER_BOOLEAN_TRUE == clearIsLocalWakeUpFlag) {
        aCntxt->isLocalWakeUp = HELPER_BOOLEAN_FALSE;
    }

    reg_wr((0xE0566000UL + 0x1fc0), ctrlRegVal);

    return;
}
