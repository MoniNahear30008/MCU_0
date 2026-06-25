/**

    @version 
*/

#ifndef FE_CONTROL_H
#define FE_CONTROL_H

#include <stdint.h>

void hsadc_meminit();
void acq_config(uint32_t adcno);
void acq_sft_trg();

#endif