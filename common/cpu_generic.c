/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_abstract_generic_impl CPU Generic Implementation
    @ingroup grp_abstract

    @addtogroup grp_abstract_generic_impl
    @{

    @file cpu_generic.c
    @brief Implementation of generic CPU APIs

    @version 0.1 Initial version
*/

#include <stdint.h>
#include <stddef.h>
#include <cpu.h>
#include <stdio.h>
/*#include <execinfo.h>*/
#include <stdlib.h>
#include <unistd.h>

/**
    @name CPU Generic Design IDs
    @{
    @brief Design IDs for CPU Generic
*/
#define BRCM_SWDSGN_CPU_LEADZERO_GENERIC_PROC       (0xB100U)    /**< @brief #CPU_Clz               */
#define BRCM_SWDSGN_CPU_BITREVERSE_GENERIC_PROC     (0xB101U)    /**< @brief #CPU_BitReverse        */
#define BRCM_SWDSGN_CPU_BREAKPOINT_GENERIC_PROC     (0xB102U)    /**< @brief #CPU_Breakpoint        */
#define BRCM_SWDSGN_CPU_GETPRIVILEGELEVEL_PROC      (0xB103U)    /**< @brief #CPU_GetPrivilegeLevel */
/** @} */


/** @brief Count of leading zeroes

    returns the count of leading zeroes

    @trace  #BRCM_SWARCH_CPU_CLZ_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION

    @code{.unparsed}
    uint32_t idx
    for (idx = 0UL; idx < 32UL; idx++)
        if (aValue & (1U << (31UL - idx)))
            break
    return idx
    @endcode
*/
uint32_t CPU_Clz(uint32_t aValue)
{
    uint32_t idx;
    for (idx = 0UL; idx < 32UL; idx++) {
        if ( (aValue & (1UL << (31UL - idx))) != 0x0UL ) {
            break;
        }
    }
    return idx;
}

/** @brief Reverse the bits

    Reverses the order of bits in the passed vale

    @trace  #BRCM_SWARCH_CPU_BITREVERSE_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION

    @code{.unparsed}
    uint32_t x = aValue
    x = (((x & 0xAAAAAAAAUL) >> 1UL) | ((x & 0x55555555UL) << 1UL))
    x = (((x & 0xCCCCCCCCUL) >> 2UL) | ((x & 0x33333333UL) << 2UL))
    x = (((x & 0xF0F0F0F0UL) >> 4UL) | ((x & 0x0F0F0F0FUL) << 4UL))
    x = (((x & 0xFF00FF00UL) >> 8UL) | ((x & 0x00FF00FFUL) << 8UL))
    return ((x >> 16UL) | (x << 16UL))
    @endcode
*/
uint32_t CPU_BitReverse(uint32_t aValue)
{
    uint32_t x = aValue;

    x = (((x & 0xAAAAAAAAUL) >> 1UL) | ((x & 0x55555555UL) << 1UL));
    x = (((x & 0xCCCCCCCCUL) >> 2UL) | ((x & 0x33333333UL) << 2UL));
    x = (((x & 0xF0F0F0F0UL) >> 4UL) | ((x & 0x0F0F0F0FUL) << 4UL));
    x = (((x & 0xFF00FF00UL) >> 8UL) | ((x & 0x00FF00FFUL) << 8UL));

    return ((x >> 16UL) | (x << 16UL));
}

#if 0
/** @brief Stops execution, prints backtrace and exits

    @trace  #BRCM_SWARCH_CPU_BREAKPOINT_PROC
    @trace  #BRCM_SWREQ_CPU_ABSTRACTION

    @anchor STDERR_FILENO

    @code{.unparsed}
    size = backtrace(array, 32UL);
    backtrace_symbols_fd(array, size, CPU_BACKTRACE);
    exit(1);
    @endcode
*/
void CPU_Breakpoint(void)
{
    void *array[32UL];
    uint32_t size;
    size = backtrace(array, 32UL);
    backtrace_symbols_fd(array, size, CPU_BACKTRACE);
    exit(1);
}

/**
    @trace #BRCM_SWARCH_CPU_GET_PRIVILEGE_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
*/
CPU_PrivilegeLevelType CPU_GetPrivilegeLevel()
{
    return CPU_PRIVILEGE_LEVEL_USER;
}
#endif
/** @} */
