/**

    @version 
*/

#ifndef Q8_LOADER_H
#define Q8_LOADER_H

#include <stdint.h>
#include <bcm_err.h>

BCM_ErrorType ProcQ8Code(uint16_t qn, uint16_t packetSize, uint16_t PacketNum);
BCM_ErrorType RunQ8(uint16_t nq);
BCM_ErrorType ProgQ8Code(uint16_t qn, uint32_t *bin, uint32_t binSize);

#endif