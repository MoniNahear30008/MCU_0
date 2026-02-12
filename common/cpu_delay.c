/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @addtogroup grp_abstract_stdlib
    @{
    @section sec_stdlib_overview Overview
    This file specifies common utility wrappers for C Standard library APIs

    @file utils.h
    @brief C Standard library wrapper functiions

    @version 1.3 Export comments from docx
*/
#include <compiler.h>
#include <inttypes.h>
#include <bcm_utils.h>
#include <chip_config.h>

/**
    @name BCM Time Utils Architecture IDs
    @{
    @brief Architecture IDs for BCM Time Utils
*/
#define BRCM_SWDSGN_BCM_NS_PER_CPU_CYCLE_MACRO          (0xB901U)   /**< @brief #BCM_NS_PER_CPU_CYCLE_QUOT      */
#define BRCM_SWDSGN_BCM_GET_CPU_CYCLES_FROM_NS_PROC     (0xB902U)   /**< @brief #BCM_GetCpuCyclesFromNs         */
#define BRCM_SWDSGN_BCM_GET_NS_FROM_CPU_CYCLES_PROC     (0xB903U)   /**< @brief #BCM_GetNsFromCpuCycles         */
#define BRCM_SWDSGN_BCM_CPUNDELAY_PROC                  (0xB904U)   /**< @brief #BCM_CpuNDelay                  */
/** @} */

/**
    @{
    @brief Nanoseconds per CPU cycle quotient and reminder macros

    @trace  #BRCM_SWARCH_BCM_GET_CPU_CYCLES_FROM_NS_PROC
    @trace  #BRCM_SWARCH_BCM_GET_NS_FROM_CPU_CYCLES_PROC
    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
#define BCM_NS_PER_CPU_CYCLE_QUOT                       (1000000000ULL / CPU_CLOCK)
#define BCM_NS_PER_CPU_CYCLE_REM                        (1000000000ULL % CPU_CLOCK)
/** @} */

/**
    @trace  #BRCM_SWARCH_BCM_GET_CPU_CYCLES_FROM_NS_PROC
    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
uint64_t BCM_GetCpuCyclesFromNs(const uint64_t aNs)
{
    uint64_t ret = 0ULL;
    uint64_t nsCpuCycleQuot = BCM_NS_PER_CPU_CYCLE_QUOT;

#if BCM_NS_PER_CPU_CYCLE_QUOT == 0ULL
#error "Implementation of BCM_GetCpuCyclesFromNs only works when CPU_CLOCK is less than or equal to 1GHz"
#elif BCM_NS_PER_CPU_CYCLE_REM == 0ULL
    ret = aNs / (uint64_t) nsCpuCycleQuot;
#elif (2ULL * BCM_NS_PER_CPU_CYCLE_REM) == CPU_CLOCK
    const uint64_t fracNum = 2ULL;
    const uint64_t fracDen = (2ULL * (uint64_t) nsCpuCycleQuot) + 1ULL;
    ret = (aNs * fracNum) / fracDen;
#elif (3ULL * BCM_NS_PER_CPU_CYCLE_REM) == (2ULL * CPU_CLOCK)
    if ((UINT64_MAX / 3ULL) >= aNs) {
        ret = (aNs * 3ULL) / ((3ULL * nsCpuCycleQuot) + 2ULL);
    } else {
        ret = UINT64_MAX - 1UL;
    }
#else
#error "No implementation of BCM_GetCpuCyclesFromNs for the configured CPU_CLOCK"
#endif

    /* BCM_TEST_ADD_PROBE(BCM_UTL_ID, BRCM_SWPTSEQ_BCM_CPU_N_DELAY,
                          BRCM_SWPTST_BCM_CPUNDELAY_OVERFLOW,
                          ret = (UINT32_MAX * 2ULL)) */
    return ret + 1UL;
}

/**
    @trace  #BRCM_SWARCH_BCM_GET_NS_FROM_CPU_CYCLES_PROC
    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
uint64_t BCM_GetNsFromCpuCycles(const uint64_t aCycles)
{
    uint64_t ret = 0ULL;
    uint64_t nsCpuCycleQuot = BCM_NS_PER_CPU_CYCLE_QUOT;

#if BCM_NS_PER_CPU_CYCLE_QUOT == 0ULL
#error "Implementation of BCM_GetNsFromCpuCycles only works when CPU_CLOCK is less than or equal to 1GHz"
#elif BCM_NS_PER_CPU_CYCLE_REM == 0ULL
    ret = aCycles * (uint64_t) nsCpuCycleQuot;
#elif (2ULL * BCM_NS_PER_CPU_CYCLE_REM) == CPU_CLOCK
    ret = (aCycles * (uint64_t) nsCpuCycleQuot) + (aCycles / 2ULL);
#elif (3ULL * BCM_NS_PER_CPU_CYCLE_REM) == (2ULL * CPU_CLOCK)
    if ((UINT64_MAX / ((3ULL * nsCpuCycleQuot) + 2ULL)) >= aCycles) {
        ret = (((3ULL * nsCpuCycleQuot) + 2ULL) * aCycles) / 3ULL;
    } else {
        ret = UINT64_MAX;
    }
#else
#error "No implementation of BCM_GetNsFromCpuCycles for the configured CPU_CLOCK"
#endif


    /* BCM_TEST_ADD_PROBE(BCM_UTL_ID, BRCM_SWPTSEQ_BCM_GET_SYS_TIME,
                          BRCM_SWPTST_BCM_GETSYSTIME_OVERFLOW,
                          ret = (UINT64_MAX - 1ULL)) */
    return ret;
}

/**
    @trace  #BRCM_SWARCH_BCM_CPUNDELAY_PROC
    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
void BCM_CpuNDelay(uint32_t aTimeNS)
{
    /* Convert time to cpu cycles */
    const uint64_t cycleCnt = BCM_GetCpuCyclesFromNs(aTimeNS);

#if defined(__CORTEX_MX__) || defined(__CORTEX_RX__)
    /*
        One loop itration takes about 2 cycles.
        One is added to the iterCnt execute the loop atleast once
    */
        uint32_t iterCnt = 0UL;
        if ((UINT32_MAX - 1ULL) >= (cycleCnt / 2ULL)) {
            iterCnt = (uint32_t)((cycleCnt / 2ULL) + 1ULL);
        } else {
            iterCnt = UINT32_MAX;
        }
    /* BCM_TEST_ADD_PROBE(BCM_UTL_ID, BRCM_SWPTSEQ_BCM_CPU_N_DELAY,
                          BRCM_SWPTST_BCM_CPUNDELAY_OVERFLOW,
                          iterCnt = 0x10UL) */
    /* nop added to avoid ARM HW optimizing the loop performance */
    COMP_ASM("    dsb                 \n"
             "cpu_delay_loop:         \n"
             "    subs %0, #1         \n"
             "    nop                 \n"
             "    nop                 \n"
             "    bne  cpu_delay_loop \n"
             : "+r" (iterCnt):);
#elif defined(__ARC_V2_EM__)
    /*
        The core loop below takes 4 CPU cycles to execute per iteration
        One is added to the iterCnt execute the loop atleast once
    */
    uint32_t iterCnt = (uint32_t)(((cycleCnt / 4ULL)) + 1ULL);
    COMP_ASM("              sync                \n"
             "              mov %%lp_count, %0  \n"
             "              lp loopExit%=       \n"
             "              nop                 \n"
             "              nop                 \n"
             "              nop                 \n"
             "              nop                 \n"
             "loopExit%=:   nop                 \n"
            :: "r" (iterCnt) :
    );
#elif defined(__x86__)
    uint32_t iterCnt = (uint32_t)(cycleCnt * 2ULL);
    while(iterCnt > 0) {
          iterCnt--;
    }
#else
#error "CpuNDely not supported for this CPU"
#endif
}
/** @} */
