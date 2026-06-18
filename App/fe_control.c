/**
    @file fe_control.c
    @brief Driver board control source file

    @version
*/

#include <stdint.h>
#include "BCM8915X_BareMetal_helper.h"
#include "regs.h"

void acq_sft_trg()
{

    // enable + soft acq trigger
    reg_rmw(ACQ_COMMON_SFT_TRG_EN_ACQ_TRG, 0, 0, 1);
    reg_rmw(ACQ_COMMON_SFT_TRG_PULSE_ACQ_TRG, 0, 0, 1);

    // Wait for 100ns and disable
    BCM_DelayNs(100);
    reg_rmw(ACQ_COMMON_SFT_TRG_EN_ACQ_TRG     , 0, 0, 0);
}

void hsadc_meminit ()
{
    uint32_t tmp;
    // Initalize HSADC_ACQ0 memories
    reg_wr(HSADC_ACQ_0_MEM_INIT_CTRL,0x00DB);
    reg_wr(HSADC_ACQ_0_MEM_INIT_CTRL,0x01FF);
    BCM_DelayUs(1000);

    reg_rd(HSADC_INTR_0_ACQ_HSADC_MEM_INIT_SEC_INTR_EISR, &tmp);
    ASSERT(tmp == 0x0492);

    // Initalize HSADC_ACQ1 memories
    reg_wr(HSADC_ACQ_1_MEM_INIT_CTRL,0x00DB);
    reg_wr(HSADC_ACQ_1_MEM_INIT_CTRL,0x01FF);
    BCM_DelayUs(1000);
    reg_rd(HSADC_INTR_1_ACQ_HSADC_MEM_INIT_SEC_INTR_EISR, &tmp);
    ASSERT(tmp == 0x0492);

    // Initalize HSADC_ACQ2 memories
    reg_wr(HSADC_ACQ_2_MEM_INIT_CTRL,0x00DB);
    reg_wr(HSADC_ACQ_2_MEM_INIT_CTRL,0x01FF);
    BCM_DelayUs(1000);
    reg_rd(HSADC_INTR_2_ACQ_HSADC_MEM_INIT_SEC_INTR_EISR, &tmp);
    ASSERT(tmp == 0x0492);

    // Initalize HSADC_ACQ3 memories
    reg_wr(HSADC_ACQ_3_MEM_INIT_CTRL,0x00DB);
    reg_wr(HSADC_ACQ_3_MEM_INIT_CTRL,0x01FF);
    BCM_DelayUs(1000);
    reg_rd(HSADC_INTR_3_ACQ_HSADC_MEM_INIT_SEC_INTR_EISR, &tmp);
    ASSERT(tmp == 0x0492);
}

void acq_config(uint32_t adcno)
{
    uint32_t cap_start_addr=0;
    uint32_t dec_rate = 0;
    uint32_t cap_size = 4096;
    uint32_t cap_mode = 1;
    uint32_t fft_en = 1;
    uint32_t fft_size = 2048;
    uint32_t ch_en = 1;
    uint32_t axis_info_en = 1;

    // Initalize control0
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL0+4096*adcno,7,4,ch_en);
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL0+4096*adcno,13,12,dec_rate);
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL0+4096*adcno,15,14,0);


    // Initalize control2
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL2+4096*adcno,25,16,cap_start_addr);
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL2+4096*adcno,10,0,cap_size);


    // Initalize control3
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL3+4096*adcno,25,16,cap_size);
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL3+4096*adcno,13,0,(cap_size+1)*4-5);

// Initalize control4
    if(ch_en==7)
        reg_rmw(HSADC_ACQ_0_ACQ_CONTROL4+4096*adcno,15,0,(cap_size+1)*2-10);
    else
        reg_rmw(HSADC_ACQ_0_ACQ_CONTROL4+4096*adcno,15,0,(cap_size+1)*3-10);


    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL0+4096*adcno,3,0,cap_mode);
    if(axis_info_en)
    {
        uint32_t tmp = 1u << fft_en;
        reg_rmw(HSADC_ACQ_0_AXIS_INFO_CONTROL+4096*adcno,1,0,tmp);
    }

    // FFT configuration
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL0+4096*adcno,11,11,fft_en);
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL0+4096*adcno,10,8,fft_size);

    uint32_t fft_prediv = fft_size-2;
    reg_rmw(HSADC_ACQ_0_ACQ_CONTROL4+4096*adcno,19,16,fft_prediv);

}
