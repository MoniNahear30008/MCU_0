/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
 * @file compiler_gcc.h
 *
 * @brief Compiler abstractions for gcc compiler
 */

#ifndef COMPILER_GCC_H
#define COMPILER_GCC_H

/**
 * @brief Static inline functions
 *
 * Compiler does not ensure that it will
 * be always inlined!!
 */

#define COMP_INLINE         static inline

/**
 * @brief Always inline
 *
 * Use this macro before function defintion to
 * force this function as inline
 */
#define COMP_ALWAYS_INLINE      static inline __attribute__((always_inline))

/**
 * @brief Never inline
 *
 * Use this macro before a function definition to
 * ensure that function is never inlined
 */
#define COMP_NEVER_INLINE       __attribute__((noinline))

/**
 * @brief Error attributes
 *
 * Can be used to report an error during compilation.
 */
#define COMP_ERROR(errMsg)      __attribute__((error(errMsg)))

/**
 * @brief Warning attributes
 *
 * Can be used on a function declaration to warn user
 * that this function is not emited during link time
 * and is being used.
 */
#define COMP_WARNING(warnMsg)   __attribute__((warning(warnMsg)))

/**
 * @brief Interrupt function
 *
 * Macro to specify that the function is an interrupt function.
 */
#define COMP_IRQ(irq)       __attribute__((interrupt(irq)))

/**
 * @brief Packing of structures/variables
 */
#define COMP_PACKED         __attribute__((packed))

/**
 * @brief Packing of structures/variables
 */
#define COMP_PACK(x)        x __PACKED__

#endif /* COMPILER_GCC_H */
