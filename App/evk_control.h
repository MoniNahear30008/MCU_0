/**
    @file evk_control.h
    @brief EVK Control header file

    @version 
*/

#ifndef EVK_CTRL_H
#define EVK_CTRL_H

#include <bcm_err.h>

BCM_ErrorType ConfigUart();
BCM_ErrorType SendMsg(uint8_t *msg, uint32_t size);
void SendMainMenu();


#endif