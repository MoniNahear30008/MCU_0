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


#endif