/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_cpu_utils_impl ARM Utilities
    @ingroup grp_cortex

    @addtogroup grp_cpu_utils_impl
    @{

    @file cpu_utils.c
    @brief ARM specific endianness functions

    This file contains the Endianness conversion APIs

    @version 0.1 Import from Document
*/
#include <compiler.h>
#include <stdint.h>
#include <cpu_utils.h>
#if defined(__x86__)
#include <byteswap.h>
#endif
/**
    @name CPU Endianness Design IDs
    @{
    @brief API Design IDs for CPU Endianness
*/
#define BRCM_SWDSGN_CPU_NATIVETOBE16_PROC        (0xBD01U)    /**< @brief #CPU_NativeToBE16 */
#define BRCM_SWDSGN_CPU_NATIVETOLE16_PROC        (0xBD02U)    /**< @brief #CPU_NativeToLE16 */
#define BRCM_SWDSGN_CPU_BETONATIVE16_PROC        (0xBD03U)    /**< @brief #CPU_BEToNative16 */
#define BRCM_SWDSGN_CPU_LETONATIVE16_PROC        (0xBD04U)    /**< @brief #CPU_LEToNative16 */
#define BRCM_SWDSGN_CPU_NATIVETOBE32_PROC        (0xBD05U)    /**< @brief #CPU_NativeToBE32 */
#define BRCM_SWDSGN_CPU_NATIVETOLE32_PROC        (0xBD06U)    /**< @brief #CPU_NativeToLE32 */
#define BRCM_SWDSGN_CPU_BETONATIVE32_PROC        (0xBD07U)    /**< @brief #CPU_BEToNative32 */
#define BRCM_SWDSGN_CPU_LETONATIVE32_PROC        (0xBD08U)    /**< @brief #CPU_LEToNative32 */
#define BRCM_SWDSGN_CPU_BETONATIVE64_PROC        (0xBD09U)    /**< @brief #CPU_BEToNative64 */
#define BRCM_SWDSGN_CPU_NATIVETOBE64_PROC        (0xBD0AU)    /**< @brief #CPU_NativeToBE64 */
#define BRCM_SWDSGN_CPU_NATIVETOLE64_PROC        (0xBD0BU)    /**< @brief #CPU_NativeToLE64 */
#define BRCM_SWDSGN_CPU_LETONATIVE64_PROC        (0xBD0CU)    /**< @brief #CPU_LEToNative64 */
/** @} */
#if defined(__x86__)
/** @brief converts 16-bit data from Native's endian format to big endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_NATIVETOBE16_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    val = aData
    return bswap_16(aData)
    @endcode
*/
uint16_t CPU_NativeToBE16(uint16_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return aData;
#else
    return bswap_16(aData);
#endif
}

/** @brief converts 16-bit data from Native's endian format to little endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_NATIVETOLE16_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return aData
    @endcode
*/
uint16_t CPU_NativeToLE16(uint16_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return bswap_16(aData);
#else
    return aData;
#endif
}

/** @brief converts 16-bit data from big endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_BETONATIVE16_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return CPU_NativeToBE16(aData)
    @endcode
*/
uint16_t CPU_BEToNative16(uint16_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return aData;
#else
    return __bswap_16(aData);
#endif
}

/** @brief converts 16-bit data from little endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_LETONATIVE16_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return aData
    @endcode
*/
uint16_t CPU_LEToNative16(uint16_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return bswap_16(aData);
#else
    return aData;
#endif
}

/** @brief converts 32-bit data from Native's endian format to big endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_NATIVETOBE32_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return bswap_32(aData)
    @endcode
*/
uint32_t CPU_NativeToBE32(uint32_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return aData;
#else
    return bswap_32(aData);
#endif
}

/** @brief converts 32-bit data from Native's endian format to little endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_NATIVETOLE32_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return aData
    @endcode
*/
uint32_t CPU_NativeToLE32(uint32_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return bswap_32(aData);
#else
    return aData;
#endif
}

/** @brief converts 32-bit data from big endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_BETONATIVE32_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return CPU_NativeToBE32(aData)
    @endcode
*/
uint32_t CPU_BEToNative32(uint32_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return aData;
#else
    return bswap_32(aData);
#endif
}

/** @brief converts 32-bit data from little endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_LETONATIVE32_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return aData
    @endcode
*/
uint32_t CPU_LEToNative32(uint32_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return bswap_32(aData);
#else
    return aData;
#endif
}

/** @brief converts 64-bit data from big endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_BETONATIVE64_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return (((uint64_t)(CPU_NativeToBE32(
                 (uint32_t)((aData << 32UL) >> 32UL))) << 32UL) |
                 (uint32_t)CPU_NativeToBE32(((uint32_t)(aData >> 32UL))))
    @endcode
*/
uint64_t CPU_BEToNative64(uint64_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return aData;
#else
    return (((uint64_t)(CPU_BEToNative32(
                 (uint32_t)(aData))) << 32UL) |
                 CPU_BEToNative32(((uint32_t)(aData >> 32UL))));
#endif
}

/** @brief converts 64-bit data from Native's endian format to big endian format.

    @trace #BRCM_SWARCH_CPU_NATIVETOBE64_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return CPU_BEToNative64(aData)
    @endcode
*/
uint64_t CPU_NativeToBE64(uint64_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return aData;
#else
    return (((uint64_t)(CPU_NativeToBE32(
                 (uint32_t)(aData))) << 32UL) |
                 CPU_NativeToBE32(((uint32_t)(aData >> 32UL))));
#endif
}

/** @brief converts 64-bit data from Native's endian format to little endian format.

    @trace #BRCM_SWARCH_CPU_NATIVETOLE64_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    return aData
    @endcode
*/
uint64_t CPU_NativeToLE64(uint64_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return (((uint64_t)(CPU_NativeToLE32(
                 (uint32_t)(aData))) << 32UL) |
                 CPU_NativeToLE32(((uint32_t)(aData >> 32UL))));
#else
    return aData;
#endif
}

/** @brief converts 64-bit data from little endian To Native's endian format

    @trace #BRCM_SWARCH_CPU_LETONATIVE64_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION
    @code{.c}
    @endcode
*/
uint64_t CPU_LEToNative64(uint64_t aData)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return (((uint64_t)(CPU_LEToNative32(
                 (uint32_t)(aData))) << 32UL) |
                 CPU_LEToNative32(((uint32_t)(aData >> 32UL))));
#else
    return aData;
#endif
}
#else

/** @brief converts 16-bit data from Native's endian format to big endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_NATIVETOBE16_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    COMP_ASM( "rev16 %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc")
    @endcode
*/
uint16_t CPU_NativeToBE16(uint16_t aData)
{
    uint16_t val;

#if defined(__GNU__) || defined(__ARMCC__) || defined(__GHS__)
    COMP_ASM( "rev16 %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc");
#else
#error Unsupported compiler
#endif

    return val;
}

/** @brief converts 16-bit data from Native's endian format to little endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_NATIVETOLE16_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return aData
    @endcode
*/
uint16_t CPU_NativeToLE16(uint16_t aData)
{
    return aData;
}

/** @brief converts 16-bit data from big endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_BETONATIVE16_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return CPU_NativeToBE16(aData)
    @endcode
*/
uint16_t CPU_BEToNative16(uint16_t aData)
{
    return CPU_NativeToBE16(aData);
}

/** @brief converts 16-bit data from little endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_LETONATIVE16_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return aData
    @endcode
*/
uint16_t CPU_LEToNative16(uint16_t aData)
{
    return aData;
}

/** @brief converts 32-bit data from Native's endian format to big endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_NATIVETOBE32_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    COMP_ASM( "rev %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc")
    @endcode
*/
uint32_t CPU_NativeToBE32(uint32_t aData)
{
    uint32_t val;

#if defined(__GNU__) || defined(__ARMCC__) || defined(__GHS__)
    COMP_ASM( "rev %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc");
#else
#error Unsupported compiler
#endif

    return val;
}

/** @brief converts 32-bit data from Native's endian format to little endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_NATIVETOLE32_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return aData
    @endcode
*/
uint32_t CPU_NativeToLE32(uint32_t aData)
{
    return aData;
}

/** @brief converts 32-bit data from big endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_BETONATIVE32_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return CPU_NativeToBE32(aData)
    @endcode
*/
uint32_t CPU_BEToNative32(uint32_t aData)
{
    return CPU_NativeToBE32(aData);
}

/** @brief converts 32-bit data from little endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_LETONATIVE32_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return aData
    @endcode
*/
uint32_t CPU_LEToNative32(uint32_t aData)
{
    return aData;
}

/** @brief converts 64-bit data from big endian format to Native's endian format.

    @limitations None
    @trace #BRCM_SWARCH_CPU_BETONATIVE64_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return (((uint64_t)(CPU_NativeToBE32(
                 (uint32_t)((aData << 32UL) >> 32UL))) << 32UL) |
                 (uint32_t)CPU_NativeToBE32(((uint32_t)(aData >> 32UL))))
    @endcode
*/
uint64_t CPU_BEToNative64(uint64_t aData)
{
    return (((uint64_t)(CPU_NativeToBE32(
                 (uint32_t)((aData << 32UL) >> 32UL))) << 32UL) |
                 (uint32_t)CPU_NativeToBE32(((uint32_t)(aData >> 32UL))));
}

/** @brief converts 64-bit data from Native's endian format to big endian format.

    @trace #BRCM_SWARCH_CPU_NATIVETOBE64_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return CPU_BEToNative64(aData)
    @endcode
*/
uint64_t CPU_NativeToBE64(uint64_t aData)
{
    return CPU_BEToNative64(aData);
}

/** @brief converts 64-bit data from Native's endian format to little endian format.

    @trace #BRCM_SWARCH_CPU_NATIVETOLE64_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return aData
    @endcode
*/
uint64_t CPU_NativeToLE64(uint64_t aData)
{
    return aData;
}

/** @brief converts 64-bit data from Native's endian format to little endian format.

    @trace #BRCM_SWARCH_CPU_LETONATIVE64_PROC
    @trace #BRCM_SWREQ_CPU_ABSTRACTION_CORTEX
    @code{.c}
    return aData
    @endcode
*/
uint64_t CPU_LEToNative64(uint64_t aData)
{
    return aData;
}
#endif
/** @} */
