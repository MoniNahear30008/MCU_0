/**

    @version
*/

#include <stdint.h>
#include <bcm_err.h>
#include "BCM8915X_BareMetal_helper.h"
#include "globals.h"
#include "regs.h"

uint32_t q8_addr;

static uint32_t ReadRegBits(uint32_t address, uint32_t bithi, uint32_t bitlo )
{
    uint32_t rx;
    int32_t x, tmp;
    reg_rd(address, &rx);
    x = (int32_t)rx;
    if (x<0)
    {
        tmp = x + 0x80000000;
        x =(uint32_t)tmp & 0x80000000;
    }
    uint32_t width = bithi - bitlo + 1;
    uint32_t mask  = (1u << width) - 1u;
    uint32_t out   = (x >> bitlo) & mask;
    return out;
}

static uint32_t check_field(uint32_t addr, uint32_t bithi, uint32_t bitlow, uint32_t value)
{
    uint32_t done=0;
	uint32_t k=0;
    uint32_t tmp=0;
	while((done==0) && (k<100))
    {
        BCM_DelayUs(1);
   		k=k+1;
   		tmp = ReadRegBits(addr, bithi, bitlow);
       	if (tmp==value)
        {
            done = 1;
        }
    }
    return done;
}

void InitQ8(uint16_t qn)
{
    q8_addr = 0x00100000 + qn * 0x10000;
}

BCM_ErrorType RunQ8(uint16_t nq)
{
    BCM_ErrorType retVal = BCM_ERR_OK;
    uint32_t isDone;

    // Config of Q8
    reg_wr(RIG_CLKNRST_Q8_TOP_RESET_CONTROL, 0);

    for (uint32_t qn = 0; qn < nq; qn++)
    {
        reg_wr(Q8_CSR_0_ALTRESETVEC + qn * 0x1000, 0x00100000 + qn * 0x10000);
        reg_rmw(Q8_CSR_0_GENERAL_CTRL1 + qn * 0x1000, 23, 22, 2);
        reg_rmw(Q8_CSR_0_GENERAL_CTRL1 + qn * 0x1000, 5, 2, 0);
        reg_rmw(Q8_CSR_0_RESETS + qn * 0x1000, 1, 1, 1);
        BCM_DelayUs(1);

        reg_rmw(Q8_CSR_0_RESETS + qn *0x1000, 1, 1, 0);
        reg_rmw(Q8_CSR_0_RESETS + qn *0x1000, 0, 0, 1);
        reg_rmw(Q8_CSR_0_RESETS + qn *0x1000, 2, 2, 0);
        reg_rmw(Q8_CSR_0_GENERAL_CTRL1 + qn * 0x1000, 0, 0, 1);
        BCM_DelayUs(1);

        reg_rmw(Q8_CSR_0_RESETS + qn * 0x1000, 0, 0, 0);
        reg_rmw(Q8_CSR_0_RESETS + qn * 0x1000, 2, 2, 1);
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 29, 29, 1);
    
        // Memory Initlization
        // Data TCM
        reg_rmw(Q8_CSR_0_SRAM_RESET0 + qn * 0x1000, 31, 0, 0);
        uint32_t value = 0xf;
        reg_rmw(Q8_CSR_0_SRAM_RESET0 + qn * 0x1000, 31, 0, value);
        isDone = check_field(Q8_CSR_0_MEMINIT_DONE0 + qn * 0x1000, 31, 0, value);     // 'DATATCM_F'
        ASSERT(isDone == 1);

        value = 0xff;
        reg_rmw(Q8_CSR_0_SRAM_RESET0 + qn * 0x1000, 31, 0, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE0 + qn * 0x1000, 31, 0, value);      // 'DATATCM_FF'
        ASSERT(isDone == 1);

        value = 0xfff;
        reg_rmw(Q8_CSR_0_SRAM_RESET0 + qn * 0x1000, 31, 0, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE0 +qn * 0x1000, 31, 0, value);      // 'DATATCM_FFF'
        ASSERT(isDone == 1);

        value = 0xffff;
        reg_rmw(Q8_CSR_0_SRAM_RESET0 + qn * 0x1000, 31, 0, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE0 + qn * 0x1000, 31, 0, value);      // 'DATATCM_FFFF'
        ASSERT(isDone == 1);

        // Data CacheA
        reg_rmw(Q8_CSR_0_SRAM_RESET1 + qn * 0x1000, 31 , 0, 0);
        value = 0xf;
        reg_rmw(Q8_CSR_0_SRAM_RESET1 + qn * 0x1000, 31, 0, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE1 + qn * 0x1000, 31, 0, value);      // 'DATACACHEA_F'
        ASSERT(isDone == 1);

        value = 0xff;
        reg_rmw(Q8_CSR_0_SRAM_RESET1 + qn * 0x1000, 31 , 0, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE1 + qn*0x1000, 31, 0, value);      // 'DATACACHEA_FF'
        ASSERT(isDone == 1);

        value = 0xfff;
        reg_rmw(Q8_CSR_0_SRAM_RESET1 + qn * 0x1000, 31, 0, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE1 + qn * 0x1000, 31, 0, value);      // 'DATACACHEA_FFF'
        ASSERT(isDone == 1);

        value = 0xffff;
        reg_rmw(Q8_CSR_0_SRAM_RESET1 + qn * 0x1000, 31, 0, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE1 + qn * 0x1000, 31, 0, value);      // 'DATACACHEA_FFFF'
        ASSERT(isDone == 1);

        // Data ICache
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 15, 2, 15);
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 15, 2, 0);
        value = 0xf;
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 15, 2, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE2 + qn * 0x1000, 15, 2, value);      //'ICACHE_F'
        ASSERT(isDone == 1);

        value = 0xff;
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 15, 2, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE2 + qn * 0x1000, 15, 2, value);      // 'ICACHE_FF'
        ASSERT(isDone == 1);

        value = 0xfff;
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 15, 2, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE2 + qn * 0x1000, 15, 2, value);      // 'ICACHE_FFF'
        ASSERT(isDone == 1);

        value = 0x3fff;
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 15, 2, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE2 + qn * 0x1000, 15, 2, value);      // 'ICACHE_3FFF'
        ASSERT(isDone == 1);

        // Data DCacheB
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 1, 0, 0);
        value = 0x3;
        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 1, 0, value);
        isDone = check_field (Q8_CSR_0_MEMINIT_DONE2 + qn * 0x1000, 1, 0, value);       // 'DCACHEB_3'
        ASSERT(isDone == 1);

        reg_rmw(Q8_CSR_0_SRAM_RESET2 + qn * 0x1000, 29, 29, 0);
        check_field (Q8_CSR_0_MEMINIT_ERROR2 + qn * 0x1000, 31, 31, 0);                 // 'MEMINIT_ERR'
        BCM_DelayUs(5);
    }

    for (uint32_t qn = 0; qn < nq; qn++)
    {
        reg_rmw(Q8_CSR_0_GENERAL_CTRL1 + qn * 0x1000, 0, 0, 1);
        reg_rmw(Q8_CSR_0_RESETS + qn * 0x1000, 0, 0, 1);
        reg_rmw(Q8_CSR_0_GENERAL_CTRL1 + qn * 0x1000, 0, 0, 0);
        reg_rmw(Q8_CSR_0_RESETS + qn * 0x1000, 0, 0, 0);
        BCM_DelayUs(50);
    } 
    return retVal;
}

BCM_ErrorType ProcQ8Code(uint16_t qn, uint16_t packetSize, uint16_t lastPacket)
{
    BCM_ErrorType retVal = BCM_ERR_OK;

    for (uint16_t i = 0; i < packetSize; i++)
    {
        reg_wr(q8_addr, gp_buffer[i]);
        q8_addr += 4;
    }
    return retVal;
}