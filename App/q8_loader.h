/**

    @version 
*/

#ifndef Q8_LOADER_H
#define Q8_LOADER_H

#include <stdint.h>
#include <bcm_err.h>

extern uint32_t q8Code[128];

BCM_ErrorType ProcQ8Code();


#endif