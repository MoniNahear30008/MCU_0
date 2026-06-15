/**
    @file ext_awg.h
    @brief External AWG header file

    @version 
*/

#ifndef DRV_BRD_H
#define DRV_BRD_H

#include <bcm_err.h>

extern uint16_t awg_vector[4096];
extern uint16_t awgLen;
extern uint16_t fir_vector[128];
extern uint16_t firLen;
extern uint16_t win_vector[1024];
extern uint16_t winLen;


BCM_ErrorType InitDrvBrd();
BCM_ErrorType ConfigAWG();
void ConfigWin();
BCM_ErrorType SetParamValue(uint8_t paramID, uint32_t value);
BCM_ErrorType AwgControl(uint8_t run);
BCM_ErrorType BRCM_i2c_write(uint32_t slaveAddr, uint8_t *data, uint32_t len);
BCM_ErrorType BRCM_i2c_read(uint32_t slaveAddr, uint8_t *data, uint32_t len);
void ConfigFIR();


typedef enum Param_IDs
{
    Chirp_AWG_gain = 0,
    PM1,
    PM2,
    LO,
    TxSOA1,
    TxSOA2,
    Tx3_0_9,
    Tx3_10_19,
    Tx3_20_29,
    Tx3_30_39
} Param_IDs;

#define HSADC_ACQ_0_WND_CONTROL 0x10602064
#define HSADC_ACQ_0_WND_DATA7   0x10602068
#define HSADC_ACQ_0_WND_DATA6   0x1060206c
#define HSADC_ACQ_0_WND_DATA5   0x10602070
#define HSADC_ACQ_0_WND_DATA4   0x10602074
#define HSADC_ACQ_0_WND_DATA3   0x10602078
#define HSADC_ACQ_0_WND_DATA2   0x1060207c
#define HSADC_ACQ_0_WND_DATA1   0x10602080
#define HSADC_ACQ_0_WND_DATA0   0x10602084

#define HSADC_ACQ_0_FIR_COEFF0  0x10602110
#define HSADC_ACQ_0_FIR_CONTROL 0x10602090

#define HSADC_ACQ_0_ACQ_CONTROL0 0x10602040


#endif