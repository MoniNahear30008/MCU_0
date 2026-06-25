/* Include Library*/
#include <string.h>
#include <stdio.h>

/* Driver library*/
#include <hsafe.h>
#include <acqcmn.h>
#include <hsadc.h>
#include <bcm_time.h>
#include "BCM8915X_BareMetal_helper.h"
#include "ts_i2c.h"
#include "i2c.h"
#include <bcm_err.h>
#include <bcm_utils.h>
#include "BCM8915x_CM7.h"
#include "evk_control.h"
#include "DrvBrd.h"
#include "globals.h"
#include "regs.h"
#include "fe_control.h"
#include "q8_image.h"
#include "q8_loader.h"

static void I2C_Test();

static BCM_ErrorType initDevice()
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

    
    reg_rd(CFG_REV_CHIP_ID, &chip_id);
    ASSERT(chip_id == 168333656);

    // ACQ set in reset. 
    reg_wr(RIG_CLKNRST_ACQ_TOP_RESET_CONTROL, 0x000A);
    // Release the reset for ACQ
    reg_wr(RIG_CLKNRST_ACQ_TOP_RESET_CONTROL, 0x000F);

    uint8_t ocpClkSel = 1; //(MHz) : 0: 625, 1: 500, 2: 416.67, 3: 312.5
    uint8_t fftClkSel = 1; //(MHz) : 0: 312.5, 1:416.67, 2:500
    uint32_t mode_5g = 0; //1.25G mode of operation

//    /*Initialize the ADCs and Configure the HSAFE*/
    retVal = bringup_hsafe(0, 0, mode_5g, ocpClkSel, fftClkSel);
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    // Setup some basic fixed coding. 
    reg_rmw(NSU_NSU_CONTROL,0,0,1); // Enable NSU
    reg_rmw(ACQ_COMMON_TXSLZR_TRG_OUT_CTRL,14,0,2);
    reg_rmw(ACQ_COMMON_HSREF_TRG_OUT_CTRL,14,8,0);

    hsadc_meminit();

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

      /*Configure the Speed and Sampling Mode*/
    uint32_t cap_size = 256*16; // 512:32 1k: 64 2k: 128; 4k: 256 8K: 512

    for (int hsadc_id=0; hsadc_id <4; hsadc_id+=1)
    {
        retVal = HSADC_DrvConfigSamplingMode(hsadc_id, (mode_5g==0) ? HSADC_SAMPLING_MODE_1P25G : HSADC_SAMPLING_MODE_5G);
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
        retVal = HSADC_DrvConfigAcqController(hsadc_id, 0xf, 0, 0, (cap_size/16-1));
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
        retVal = HSADC_DrvConfigCaptureMode(hsadc_id, HSADC_CAPTURE_MODE_SINGLE);
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
    }

    acq_config(0);

    // Enable ACQ_CLK output @ 312.5 MHz
    reg_rmw(HSAFE_CLKGEN_CONFIG0, 2, 2, 1);
    reg_rmw(HSAFE_CLKGEN_CONFIG0, 8, 7, 2);
    reg_rmw(HSAFE_CLKGEN_CONFIG0, 15, 12, 0);
    reg_rmw(HSAFE_PLL_CONFIG4, 9, 9, 1);
    reg_rmw(HSAFE_PLL_CONFIG5, 11, 9, 3);                              //6:100, 4:200; 3: 312.5

    return retVal;
}

/* Main function */
void main()
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

	retVal = initDevice();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = TP_Config();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = ConfigUart();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = InitDrvBrd();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    retVal = ProgQ8Code(0, q8_0_image_single, sizeof(q8_0_image_single) / sizeof(q8_0_image_single[0]));
    ASSERT(retVal == BCM_ERR_OK);

    retVal = RunQ8(1);
//    I2C_Test();

    newMsg = 0;
    // main loop
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

static void __attribute__((unused)) I2C_Test()
{
    BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    while (1)
    {
        uint8_t tempBuf[10] = {0x40, 0x39, 0x89, 0x85, 0x52, 0x21, 0,0, 0, 0}; // dummy data to write to TEC temp sensor
        TP_Control(1);
        retVal = BRCM_i2c_write(0x26, tempBuf, 10);   /* Calling I2C transfer function */
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
        TP_Control(0);
        BCM_DelayUs(100);
        retVal = BRCM_i2c_read(0x26, tempBuf, 1);   /* Calling I2C transfer function */
        ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
        BCM_DelayUs(1000);
    }
}