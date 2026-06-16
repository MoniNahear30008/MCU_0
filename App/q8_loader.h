/**

    @version 
*/

#ifndef Q8_LOADER_H
#define Q8_LOADER_H

#include <stdint.h>
#include <bcm_err.h>

BCM_ErrorType ProcQ8Code(uint16_t qn, uint16_t packetSize, uint16_t lastPacket);


#endif