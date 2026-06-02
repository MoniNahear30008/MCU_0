/**
    @file ext_awg.h
    @brief External AWG header file

    @version 
*/

#ifndef DRV_BRD_H
#define DRV_BRD_H

#include <bcm_err.h>

extern uint16_t awg_vector[4096];

BCM_ErrorType InitDrvBrd();
BCM_ErrorType ConfigAWG();
BCM_ErrorType SetParamValue(uint8_t paramID, uint32_t value);
BCM_ErrorType AwgControl(uint8_t run);
BCM_ErrorType BRCM_i2c_write();
BCM_ErrorType BRCM_i2c_read();

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

#endif