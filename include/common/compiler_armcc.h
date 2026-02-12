/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
 * @file compiler_armcc.h
 *
 * @brief Compiler abstractions for armcc compiler
 */

#ifndef COMPILER_ARMCC_H
#define COMPILER_ARMCC_H

/**
 * @brief Static inline functions
 *
 * Compiler does not ensure that it will
 * be always inlined!!
 */

#define COMP_INLINE             static __inline

/**
 * @brief Always inline
 *
 * Use this macro before function defintion to
 * force this function as inline
 */
#define COMP_ALWAYS_INLINE      static __attribute__((always_inline))

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
#define COMP_ERROR(aMsg)     __attribute__((error(aMsg)))

/**
 * @brief Warning attributes
 *
 * Can be used on a function declaration to warn user
 * that this function is not emited during link time
 * and is being used.
 */
#define COMP_WARNING(aMsg)   __attribute__((warning(aMsg)))

/**
 * @brief Interrupt function
 *
 * Macro to specify that a data shall be placed in a section (s)
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

#endif /* COMPILER_ARMCC_H */
