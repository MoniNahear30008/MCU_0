/* Include Library*/
#include <string.h>
#include <stdio.h>

/* Driver library*/
#include <hsafe.h>
#include <acqcmn.h>
#include <hsadc.h>
#include <bcm_time.h>
#include "BCM8915X_BareMetal_helper.h"
#include <gpio.h>
#include "ts_i2c.h"
#include "i2c.h"
#include <bcm_err.h>
#include <bcm_utils.h>
#include "BCM8915x_CM7.h"
#include "evk_control.h"
#include "DrvBrd.h"
#include "globals.h"

static BCM_ErrorType initDevice()
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t ocpClkSel = 1; //(MHz) : 0: 625, 1: 500, 2: 416.67, 3: 312.5
    uint8_t fftClkSel = 1; //(MHz) : 0: 312.5, 1:416.67, 2:500
    uint32_t mode_5g = 0; //1.25G mode of operation

//    /*Initialize the ADCs and Configure the HSAFE*/
    retVal = bringup_hsafe(0, 0, mode_5g, ocpClkSel, fftClkSel);
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    /*HSADC Initialization*/
    for (int hsadc_id=0; hsadc_id <4; hsadc_id+=1)
    {
        retVal = HSADC_DrvInit(hsadc_id);
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
    }

    /*Calib Configurations*/
    HSADC_CalibConfigType calib_cfg;
    HSADC_CalibStatusType calib_status;
    uint8_t fgen = 1;
    uint8_t bgen = 1;

    calib_cfg.calStatesEn = 3*fgen + 4*bgen;
    calib_cfg.calBgRdbDuration = 12000;

    for (int hsadc_id=0; hsadc_id < 4; hsadc_id +=1)
    {
        retVal = HSADC_DrvInitCalibration(hsadc_id, &calib_cfg);
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

        /*Trigger Calibration*/
        retVal = HSADC_DrvTriggerCalib(hsadc_id);
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
    }

    BCM_DelayUs(5000);

    for (int hsadc_inst=0; hsadc_inst<4; hsadc_inst+=1)
    {
        do
        {
            retVal = HSADC_DrvFftGetCalibStatus(hsadc_inst, &calib_status);
            ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
           BCM_DelayUs(100);
        } while (calib_status.calStatus==1);
        calib_status.calStatus = 0xf;
    }

    return retVal;
}

static BCM_ErrorType __attribute__((unused)) ConfigHSADC()
{
    /*Variable Declerations*/
    uint8_t ocpClkSel = 1; //(MHz) : 0: 625, 1: 500, 2: 416.67, 3: 312.5
    uint8_t fftClkSel = 1; //(MHz) : 0: 312.5, 1:416.67, 2:500
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t cap_size = 32*16; // Capture Size
    uint32_t mode_5g = 0; //1.25G mode of operation

   /*Initialize the ADCs and Configure the HSAFE*/
    retVal = bringup_hsafe(0, 0, mode_5g, ocpClkSel, fftClkSel);
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    /*HSADC Initialization*/
    for (int hsadc_id=0; hsadc_id <4; hsadc_id+=1) 
    {
        retVal = HSADC_DrvInit(hsadc_id);
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
    }

    /*Calib Configurations*/
    {
        HSADC_CalibConfigType calib_cfg;
        HSADC_CalibStatusType calib_status;
        uint8_t fgen = 1;
        uint8_t bgen = 1;

        calib_cfg.calStatesEn = 3*fgen + 4*bgen;
        calib_cfg.calBgRdbDuration = 12000;

        for (int hsadc_id=0; hsadc_id < 4; hsadc_id +=1) {
            retVal = HSADC_DrvInitCalibration(hsadc_id, &calib_cfg);
            ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
            /*Trigger Calibration*/
            retVal = HSADC_DrvTriggerCalib(hsadc_id);
            ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
        }

        BCM_DelayUs(5000);

        for (int hsadc_inst=0; hsadc_inst<4; hsadc_inst+=1) {
            do {
                retVal = HSADC_DrvFftGetCalibStatus(hsadc_inst, &calib_status);
                ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
                BCM_DelayUs(100);
            } while (calib_status.calStatus==1);
            calib_status.calStatus = 0xf;
        }
    }


    /*Configure the Speed and Sampling Mode*/
    for (int hsadc_id=0; hsadc_id <4; hsadc_id+=1)
    {
        retVal = HSADC_DrvConfigSamplingMode(hsadc_id, (mode_5g==0) ? HSADC_SAMPLING_MODE_1P25G : HSADC_SAMPLING_MODE_5G);
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
        retVal = HSADC_DrvConfigAcqController(hsadc_id, 0xf, 0, 0, (cap_size/16-1));
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
        retVal = HSADC_DrvConfigCaptureMode(hsadc_id, HSADC_CAPTURE_MODE_SINGLE);
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
    }
    return retVal;
}

static BCM_ErrorType __attribute__((unused)) ConfigGPIO(GPIO_ChannelType aChannelId)
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

    GPIO_ConfigType gOutCfgDef ={
                                    .mode       = GPIO_CFG_MODE_OUTPUT,
                                    .oType      = GPIO_CFG_OUTPUT_OPEN_DRAIN,
                                    .pupd       = GPIO_CFG_PUPD_PULL_UP,
                                    .dout       = GPIO_LEVEL_LOW,
                                    .hys        = GPIO_CFG_HYSTERESIS_DISABLE,
                                    .strength   = GPIO_CFG_DRIVE_STRENGTH_8MA,
                                    .ind        = GPIO_CFG_INPUT_DISABLE,
                                    .slewRate   = GPIO_CFG_SLEW_RATE_SLEWED,
                                    .doutInvert = GPIO_CFG_DOUT_INVERT_DISABLE,
                                    .aCfgMask   = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_OTYPE | GPIO_CFG_MASK_PUPD | GPIO_CFG_MASK_DOUT | GPIO_CFG_MASK_HIST | GPIO_CFG_MASK_SEL | GPIO_CFG_MASK_IND | GPIO_CFG_MASK_SRC | GPIO_CFG_MASK_DOUT_INV
                                };

    retVal = GPIO_DrvInitChannel(GPIO_HW_ID_0, aChannelId, &gOutCfgDef);
	return retVal;
}

/* Main function */
void main()
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

	retVal = initDevice();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = ConfigGPIO(TP_GPIO);
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = ConfigUart();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = InitDrvBrd();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    newMsg = 0;
    // main loop
    while (0)
    {
        uint8_t tempBuf[10] = {0x40, 0x39, 0x89, 0x85, 0x52, 0x21, 0,0, 0, 0}; // dummy data to write to TEC temp sensor
        // retVal = GPIO_DrvChannelWrite(GPIO_HW_ID_0, TP_GPIO, GPIO_LEVEL_HIGH);
        // retVal = BRCM_i2c_write(0x26, tempBuf, 10);   /* Calling I2C transfer function */
        // retVal = GPIO_DrvChannelWrite(GPIO_HW_ID_0, TP_GPIO, GPIO_LEVEL_LOW);
        // BCM_DelayUs(100);
        retVal = BRCM_i2c_read(0x26, tempBuf, 1);   /* Calling I2C transfer function */
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
        BCM_DelayUs(1000);
    }

    while (1)
    {
        BCM_DelayUs(100);
//        __WFI();

        /* code */
        if (newMsg)
        {
            ProcHostMsg();
        }
    }
}
