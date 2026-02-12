/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_time_ifc Time Abstractions
    @ingroup grp_abstract

    @addtogroup grp_time_ifc
    @{
    This section describes the various time related utility APIs.

    @file bcm_time.h
    @brief Time related APIs

    This file specifies various time related APIs.

    @version 1.3 Import comments from docx
*/

#ifndef BCM_TIME_H
#define BCM_TIME_H

#include <inttypes.h>

/**
    @name BCM Time Utils Architecture IDs
    @{
    @brief Architecture IDs for BCM Time Utils
*/
#define BRCM_SWARCH_BCM_NS_TO_MS_MACRO             (0x8901U)    /**< @brief #BCM_NS_TO_MS           */
#define BRCM_SWARCH_BCM_NS_PER_SEC_MACRO           (0x8902U)    /**< @brief #BCM_NS_PER_SEC         */
#define BRCM_SWARCH_BCM_TIME_TYPE                  (0x8903U)    /**< @brief #BCM_TimeType           */
#define BRCM_SWARCH_BCM_GETSYSTEMTIME_PROC         (0x8904U)    /**< @brief #BCM_GetSystemTime      */
#define BRCM_SWARCH_BCM_GETTIME_PROC               (0x8905U)    /**< @brief #BCM_GetTime            */
#define BRCM_SWARCH_BCM_GETTIMEABSDIFF_PROC        (0x8906U)    /**< @brief #BCM_GetTimeAbsDiff     */
#define BRCM_SWARCH_BCM_DELAYNS_PROC               (0x8907U)    /**< @brief #BCM_DelayNs            */
#define BRCM_SWARCH_BCM_DELAYUS_PROC               (0x8908U)    /**< @brief #BCM_DelayUs            */
#define BRCM_SWARCH_BCM_GETTIMENS_PROC             (0x8909U)    /**< @brief #BCM_GetTimeNs          */
#define BRCM_SWARCH_BCM_GETSYSTEMTIMENS_PROC       (0x890AU)    /**< @brief #BCM_GetSystemTimeNs    */
#define BRCM_SWARCH_BCM_SYSTEMHRTIMERSTART_PROC    (0x890BU)    /**< @brief #BCM_SystemHRTimerStart */
#define BRCM_SWARCH_BCM_SYSTEMHRTIMERSTOP_PROC     (0x890CU)    /**< @brief #BCM_SystemHRTimerStop  */
#define BRCM_SWARCH_BCM_SEC_TO_MS_MACRO            (0x890DU)    /**< @brief #BCM_SEC_TO_MS          */
#define BRCM_SWARCH_BCM_MS_TO_NS_MACRO             (0x890EU)    /**< @brief #BCM_MS_TO_NS           */
#define BRCM_SWARCH_BCM_MS_TO_SEC_MACRO            (0x8910U)    /**< @brief #BCM_MS_TO_SEC          */
/** @} */

#define CPU_CYCLE_CNT_MAX (0xFFFFFFFFUL)

/**
    @brief Nanoseconds to milliseconds

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aNenoSec        Time in nanoseconds

    @return  Time in miliseconds

    @post None

    @trace #BRCM_SWREQ_BCM_TIME
*/
#define BCM_NS_TO_MS(aNenoSec)     ((aNenoSec) / 1000000ULL)

/**
    @brief milliseconds to Nanoseconds

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aMiliSec        Time in miliseconds

    @return  Time in nanoseconds

    @post None

    @trace #BRCM_SWREQ_BCM_TIME
*/
#define BCM_MS_TO_NS(aMiliSec)     ((aMiliSec) * 1000000ULL)

/**
    @brief Nanoseconds per second

    @trace #BRCM_SWREQ_BCM_TIME
*/
#define BCM_NS_PER_SEC      (1000000000UL)

/**
    @brief Second to Millisecond

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aTimeSec        Time in seconds

    @return  Time in miliseconds

    @post None

    @trace #BRCM_SWREQ_BCM_TIME
*/
#define BCM_SEC_TO_MS(aTimeSec)    (1000UL * (aTimeSec))

/**
    @brief Millisecond to Second

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aTimeMiliSec        Time in miliseconds

    @return  Time in seconds

    @post None

    @trace #BRCM_SWREQ_BCM_TIME
*/
#define BCM_MS_TO_SEC(aTimeMiliSec)    ((aTimeMiliSec) / 1000UL)

/**
    @brief Type for time structure

    @trace #BRCM_SWREQ_BCM_TIME
*/
typedef struct sBCM_TimeType {
    uint32_t s;         /**< @brief Seconds */
    uint32_t ns;        /**< @brief Nano seconds */
} BCM_TimeType;

/** @brief Get the current system time

    This API retrieves the current system time in privileged context.

    @behavior Sync, Re-entrant

    @pre None

    @param[out]  aTime       Pointer to Time structure

    @return     void

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations
    + This API shall only be called from privileged context.
    + aTime shall be a valid pointer.
*/
extern void BCM_GetSystemTime(BCM_TimeType * const aTime);

/** @brief Get the current system time

    This API obtains the current system time from unprivileged context

    @behavior Sync, Re-entrant

    @pre None

    @param[out]  aTime       Pointer to Time structure

    @return     void

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations
    + This API shall only be called from unprivileged context.
    + aTime shall be a valid pointer.
*/
extern void BCM_GetTime(BCM_TimeType * const aTime);

/** @brief Get the current system time in nanoseconds

    This API obtains the current system time from unprivileged context

    @behavior Sync, Re-entrant

    @pre None

    @param void

    @return     Time in nanoseconds

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations This API shall only be called from unprivileged context.
*/
extern uint64_t BCM_GetTimeNs(void);

/** @brief Get the current system time in nanoseconds

    This API obtains the current system time from privileged context

    @behavior Sync, Re-entrant

    @pre None

    @param void

    @return     Time in nanoseconds

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations This API shall only be called from privileged context.
*/
extern uint64_t BCM_GetSystemTimeNs(void);

/** @brief Get absolute difference between two time values

    This API obtains absolute difference between two time values.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aTime1      Time 1 value
    @param[in]   aTime2      Time 2 value

    @return Absolute difference between aTime1 and aTime2.

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations This API shall only be called from unprivileged context.
*/
BCM_TimeType BCM_GetTimeAbsDiff(const BCM_TimeType aTime1, const BCM_TimeType aTime2);

/** @brief Do busy waiting

    This API does busy waiting (up to Nanoseconds granularity) using internal timer block.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aTimeNs      Time in nanoseconds

    @return void

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations
    + This API shall only be called from privileged context.
    + This API shall be used only after timer is initialized.
    + This API is expensive in terms of system performance, hence must be used carefully.
    + aTimeNs should be less than or equal to 0xFFFFFF00UL, otherwise the function has no effect.
*/
extern void BCM_DelayNs(uint32_t aTimeNs);

/** @brief Do busy waiting

    This API does cpu busy waiting (upto Microseconds granularity).

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aTimeUs      Time in microseconds

    @return void

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations
    + This API shall only be called from privileged context.
    + This API shall be used only after timer is initialized.
    + The delay will be equal to or more than the requested value.
    + This API is expensive in terms of system performance, hence must be used carefully.
*/
extern void BCM_DelayUs(uint32_t aTimeUs);


/** @brief High resolution timer

    This API starts high resolution timer.

    @behavior Sync, Re-entrant

    @pre None

    @param  none

    @return void

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations    This API shall only be called from privileged context.
*/
/*  @event         misra_c_2012_rule_8_6_violation, MISRA C-2012 Declarations and Definitions
    @issue         Function BCM_SystemHRTimerStart is declared but never defined.
    @risk          None
    @justification Issue reported due to Coverity build configuration. */
/* coverity[misra_c_2012_rule_8_6_violation] */
extern void BCM_SystemHRTimerStart(void);

/** @brief

    This API stops High Resolution Timer

    @behavior Sync, Re-entrant

    @pre None

    @param  none

    @return void

    @post None

    @trace  #BRCM_SWREQ_BCM_TIME

    @limitations    This API shall only be called from privileged context.
*/
/*  @event         misra_c_2012_rule_8_6_violation, MISRA C-2012 Declarations and Definitions
    @issue         Function BCM_SystemHRTimerStop is declared but never defined.
    @risk          None
    @justification Issue reported due to Coverity build configuration. */
/* coverity[misra_c_2012_rule_8_6_violation] */
extern void BCM_SystemHRTimerStop(void);

#endif /* BCM_TIME_H */
/** @} */
