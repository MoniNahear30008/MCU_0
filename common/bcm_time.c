/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @defgroup grp_time_impl Implementation for the time abstract routines
    @ingroup grp_abstract

    @addtogroup grp_time_impl
    @{
    Implementations for the functions provided in bcm_time.h are provided here.

    @file bcm_time.c
    @brief Implementations for the time related APIs

    @version 1.0 Initial version
*/
#include <bcm_utils.h>
#include <stddef.h>
#include <bcm_time.h>
#include <cpu.h>
#include <compiler.h>

/**
    @name BCM Time Utils design IDs
    @{
    @brief Design IDs for BCM Time Utils
*/
#define BRCM_SWDSGN_BCM_GETSYSTEMTIME_PROC    (0xBA01U)  /**< @brief #BCM_GetSystemTime      */
#define BRCM_SWDSGN_BCM_GETTIME_PROC          (0xBA02U)  /**< @brief #BCM_GetTime            */
#define BRCM_SWDSGN_BCM_GETSYSTEMTIMENS_PROC  (0xBA03U)  /**< @brief #BCM_GetSystemTimeNs    */
#define BRCM_SWDSGN_BCM_GETTIMENS_PROC        (0xBA04U)  /**< @brief #BCM_GetTimeNs          */
#define BRCM_SWDSGN_BCM_GETTIMEABSDIFF_PROC   (0xBA05U)  /**< @brief #BCM_GetTimeAbsDiff     */
#define BRCM_SWDSGN_BCM_DELAYNS_PROC          (0xBA06U)  /**< @brief #BCM_DelayNs            */
#define BRCM_SWDSGN_BCM_DELAYUS_PROC          (0xBA07U)  /**< @brief #BCM_DelayUs            */
/** @} */

/**
    @code{.unparsed}
    timeNow = BCM_GetNsFromCpuCycles(CPU_GetCycleCnt())
    if UINT32_MAX < (timeNow / BCM_NS_PER_SEC)
        aTime->s = 0xFFFFFFFFUL;
    else
        aTime->s = (uint32_t)(timeNow / BCM_NS_PER_SEC);
    aTime->ns = (uint32_t)(timeNow % BCM_NS_PER_SEC);
    @endcode

    @trace #BRCM_SWARCH_BCM_GETSYSTEMTIME_PROC
    @trace #BRCM_SWREQ_BCM_TIME
*/
void BCM_GetSystemTime(BCM_TimeType * const aTime)
{
    const uint64_t timeNow = BCM_GetNsFromCpuCycles(CPU_GetCycleCnt());

    if (UINT32_MAX < (timeNow / BCM_NS_PER_SEC)) {
        aTime->s = 0xFFFFFFFFUL;
    } else {
        aTime->s = (uint32_t)((uint32_t)timeNow / (uint32_t)BCM_NS_PER_SEC);
    }

    aTime->ns = (uint32_t)((uint32_t)timeNow % (uint32_t)BCM_NS_PER_SEC);

    return;
}

/**
    @code{.unparsed}
    BCM_GetSystemTime(aTime);
    @endcode

    @trace #BRCM_SWARCH_BCM_GETTIME_PROC
    @trace #BRCM_SWREQ_BCM_TIME
*/
void BCM_GetTime(BCM_TimeType * const aTime)
{
    BCM_GetSystemTime(aTime);
    return;
}

/**
    @code{.unparsed}
    return BCM_GetNsFromCpuCycles(CPU_GetCycleCnt())
    @endcode

    @trace #BRCM_SWARCH_BCM_GETSYSTEMTIMENS_PROC
    @trace #BRCM_SWREQ_BCM_TIME
*/
uint64_t BCM_GetSystemTimeNs(void)
{
    return BCM_GetNsFromCpuCycles(CPU_GetCycleCnt());
}

/**
    @code{.unparsed}
    return BCM_GetSystemTimeNs()
    @endcode

    @trace #BRCM_SWARCH_BCM_GETTIMENS_PROC
    @trace #BRCM_SWREQ_BCM_TIME
*/
uint64_t BCM_GetTimeNs(void)
{
    return BCM_GetSystemTimeNs();
}

/**
    @code{.unparsed}
    BCM_TimeType diff = {0UL, 0UL}

    if aTime1.s >= aTime2.s:
        if aTime1.ns >= aTime2.ns:
            s = aTime1.s - aTime2.s
            ns = aTime1.ns - aTime2.ns
        else:
            if aTime1.s > aTime2.s:
                s = aTime1.s - aTime2.s - 1UL
                ns = BCM_NS_PER_SEC - aTime2.ns + aTime1.ns
             else:
                s = 0UL
                ns = aTime2.ns - aTime1.ns
    else:
        if aTime1.ns >= aTime2.ns:
            s = aTime2.s - aTime1.s - 1UL
            ns = BCM_NS_PER_SEC - aTime1.ns + aTime2.ns
         else:
            s = aTime2.s - aTime1.s
            ns = aTime2.ns - aTime1.ns

    diff.s = s
    diff.ns = ns

    return diff
    @endcode

    @trace #BRCM_SWARCH_BCM_GETTIMEABSDIFF_PROC
    @trace #BRCM_SWREQ_BCM_TIME
*/
BCM_TimeType BCM_GetTimeAbsDiff(const BCM_TimeType aTime1, const BCM_TimeType aTime2)
{
    BCM_TimeType diff = {0UL, 0UL};
    uint32_t ns,s;

    if (aTime1.s >= aTime2.s) {
        if(aTime1.ns >= aTime2.ns) {
            s = aTime1.s - aTime2.s;
            ns = aTime1.ns - aTime2.ns;
        } else {
            if (aTime1.s > aTime2.s) {
                s = aTime1.s - aTime2.s - 1UL;
                ns = (BCM_NS_PER_SEC - aTime2.ns) + aTime1.ns;
            } else {
                s = 0UL;
                ns = aTime2.ns - aTime1.ns;
            }
        }
    } else {
        if (aTime1.ns >= aTime2.ns) {
            s = aTime2.s - aTime1.s - 1UL;
            ns = (BCM_NS_PER_SEC - aTime1.ns) + aTime2.ns;
        } else {
            s = aTime2.s - aTime1.s;
            ns = aTime2.ns - aTime1.ns;
        }
    }
    diff.s = s;
    diff.ns = ns;

    return diff;
}

/**
    Max supported delay depends on #CPU_CYCLE_CNT_MAX and #CPU_CLOCK as only one
    overflow is handled.
    e.g. CPU_CYCLE_CNT_MAX: 0xFFFFFFFF, CPU_CLOCK:6M,  max delay supported delay is 7.16s

    @code{.unparsed}
     Read current cpu cycle count in NS
     Calculate target cpu cycle count in NS based on overflow
     Poll for cpu cycle count until target is reached
    @endcode

    @trace #BRCM_SWARCH_BCM_GET_CPU_CYCLES_FROM_NS_PROC
    @trace #BRCM_SWARCH_BCM_GETSYSTEMTIMENS_PROC
    @trace #BRCM_SWREQ_BCM_TIME
*/
void BCM_DelayNs(uint32_t aTimeNs)
{
    uint64_t maxTime = BCM_GetNsFromCpuCycles(CPU_CYCLE_CNT_MAX);
    uint64_t refTime = BCM_GetSystemTimeNs();
    uint64_t targetTime = refTime + aTimeNs;
    uint64_t currTime;

    if (maxTime >= targetTime) {
        do {
            currTime = BCM_GetSystemTimeNs();
        } while(targetTime >= currTime && currTime >= refTime);
    } else {
        targetTime -= maxTime;
        do {
            currTime = BCM_GetSystemTimeNs();
        } while(targetTime >= currTime || currTime >= refTime);
    }

    return;
}

/**
    @code{.unparsed}
    quot = aTimeUs / 1024UL
    rem = aTimeUs % 1024UL

    loop quot times:
        BCM_CpuNDelay(1024UL * 1000UL)

    BCM_CpuNDelay(rem * 1000UL)
    @endcode

    @trace #BRCM_SWARCH_BCM_DELAYUS_PROC
    @trace #BRCM_SWREQ_BCM_TIME
*/
void BCM_DelayUs(uint32_t aTimeUs)
{
    uint32_t i;
    const uint32_t quot = aTimeUs / 1024UL;
    const uint32_t rem = aTimeUs % 1024UL;

    for (i = 0UL; i < quot; i++) {
        BCM_CpuNDelay(1024UL * 1000UL);
    }
    BCM_CpuNDelay(rem * 1000UL);

    return;
}

/** @} */
