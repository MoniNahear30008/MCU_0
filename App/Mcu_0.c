/* Include Library*/
#include <string.h>
#include <stdio.h>

//#include <assrt.h>

#include "BCM8915x_CM7.h"

/* Driver library*/
#include <hsafe.h>
#include <acqcmn.h>
#include <hsadc.h>
#include <bcm_time.h>
#include "BCM8915X_BareMetal_helper.h"
#include <awg.h>
#include <gpio.h>

#define GPIO_HWID (0UL)

uint32_t AWG_Data[32];

uint32_t retMem __attribute__((section ("SRAM"))) = 0xff;

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
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t cap_size = 32*16; // Capture Size
    uint32_t mode_5g = 0; //1.25G mode of operation

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

static BCM_ErrorType __attribute__((unused)) ConfigAwg()
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;
	AWG_ChannelConfigType cfg;
	cfg.incrementMode = AWG_INCREMENT_MODE_PHASE_ACCUMULATOR;
    cfg.numberPages = 1;
    cfg.activePageLatch = 0;
    cfg.clockDivider = 1;
    cfg.incrementCtrl = 1;
    cfg.rampResetCtrl = 0;
    cfg.syncCtrl = 0;
 
	retVal = AWG_DrvInit(0);

	retVal = AWG_DrvChannelConfig(0, 0, &cfg);

    retVal = AWG_DrvChannelConfigFrequency(0, 0, 1000); //1MHz

    for (uint32_t i=0; i<32; i+=1)
        AWG_Data[i] = i << 8;

    AWG_Data[31] |= 8;      // Set bit 3 of last data point to sync the waveform

    AWG_ChannelWaveformMemType chWaveform;
    chWaveform.pageNumber = 0;
    chWaveform.numberOfLocation = 32;
    chWaveform.opMode = AWG_WAVEFORM_MEM_OP_WRITE;
    chWaveform.data = AWG_Data;
    retVal = AWG_DrvWaveformMemAccess(0, 0, &chWaveform);

    // for (uint32_t i=0; i<32; i+=1)
    //     AWG_Data[i] = 0;

    // chWaveform.opMode = AWG_WAVEFORM_MEM_OP_READ;
    // retVal = AWG_DrvWaveformMemAccess(0, 0, &chWaveform);

    // Enable and run the channel
    AWG_ChannelControlType chControl;
    chControl.enable = 1;
    retVal = AWG_DrvChannelEnableControl(0, 0, chControl);
    retVal = AWG_DrvChannelRunControl(0, 0, 1); // Run the channel

    while (1)
    {
        /* code */
    }
    
	return retVal;
}

static BCM_ErrorType __attribute__((unused)) ConfigGPIO()
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

    GPIO_ConfigType gOutCfgDef ={
                                    .mode       = GPIO_CFG_MODE_OUTPUT,
                                    .oType      = GPIO_CFG_OUTPUT_PUSH_PULL,
                                    .pupd       = GPIO_CFG_PUPD_PULL_UP,
                                    .dout       = GPIO_LEVEL_LOW,
                                    .hys        = GPIO_CFG_HYSTERESIS_DISABLE,
                                    .strength   = GPIO_CFG_DRIVE_STRENGTH_8MA,
                                    .ind        = GPIO_CFG_INPUT_ENABLE,
                                    .slewRate   = GPIO_CFG_SLEW_RATE_SLEWED,
                                    .doutInvert = GPIO_CFG_DOUT_INVERT_DISABLE,
                                    .aCfgMask   = GPIO_CFG_MASK_MODE | GPIO_CFG_MASK_OTYPE | GPIO_CFG_MASK_PUPD | GPIO_CFG_MASK_DOUT | GPIO_CFG_MASK_HIST | GPIO_CFG_MASK_SEL | GPIO_CFG_MASK_IND | GPIO_CFG_MASK_SRC | GPIO_CFG_MASK_DOUT_INV
                                };

    GPIO_ChannelType test_chnl = GPIO_CHANNEL_10;

    retVal = GPIO_DrvInitChannel(GPIO_HWID, test_chnl, &gOutCfgDef);

    while (1)
    {
    retVal = GPIO_DrvChannelWrite(GPIO_HWID, test_chnl, GPIO_LEVEL_HIGH);
    retVal = GPIO_DrvChannelWrite(GPIO_HWID, test_chnl, GPIO_LEVEL_LOW);
    }
 
	return retVal;
}

/* Main function */
void main()
{
	BCM_ErrorType retVal = BCM_ERR_INVAL_PARAMS;

	retVal = initDevice();
    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

	// retVal = ConfigAwg();
    // ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

//	retVal = ConfigHSADC();
//    ASSERT(retVal != BCM_ERR_INVAL_PARAMS);

    // retVal = ConfigGPIO();
    // ASSERT(retVal != BCM_ERR_INVAL_PARAMS);
}

