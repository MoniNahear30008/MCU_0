/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_compiler Compiler Abstractions
    @ingroup grp_abstract

    @addtogroup grp_compiler
    @{
    @section sec_compiler_overview Overview
    This file specfies compiler abstractions used by diffrent software
    components in the system.

    @limitations The behaviour of these abstractions are
    compiler dependent

    @file compiler.h
    @brief Compiler abstractions

    This file specfies compiler abstractions used by diffrent software
    components in the system. The behaviour of these abstractions are
    compiler dependent.

    @version 0.1 Initial Version
*/

#ifndef COMPILER_H
#define COMPILER_H

#if defined(GNU)
#include "compiler_gcc.h"
#elif defined(ARMCC)
#include "compiler_armcc.h"
#elif defined(GHS)
#include "compiler_ghs.h"
#elif defined(METAWARE)
#include "compiler_metaware.h"
#else
#error Unsupported compiler
#endif


/**
    @name Compiler Abstraction Architecture IDs
    @{
    @brief Architecture IDs for Compiler Abstraction
*/
#define BRCM_SWARCH_COMP_SECTION_MACRO    (0x8401U)    /**< @brief #COMP_SECTION */
#define BRCM_SWARCH_COMP_ALIGN_MACRO      (0x8402U)    /**< @brief #COMP_ALIGN   */
#define BRCM_SWARCH_COMP_ASM_MACRO        (0x8403U)    /**< @brief #COMP_ASM     */
/** @} */

/* @start_no_export_python */

/**
    @brief Macro to place content in specific section

    Macro to specify that a data shall be placed in a section (s)

    @trace #BRCM_SWREQ_COMP_ABSTRACTION
*/
#if defined(GNU) || defined(ARMCC) || defined(GHS) || defined(METAWARE)
#define COMP_SECTION(s)     __attribute__((section(s)))
#else
#error Unsupported Compiler
#endif

/**
    @brief Alignment

    Alignment using compiler directive

    @trace #BRCM_SWREQ_COMP_ABSTRACTION
*/
#if defined(GNU) || defined(ARMCC) || defined(GHS) || defined(METAWARE)
#define COMP_ALIGN(a)       __attribute__((aligned(a)))
#else
#error Unsupported Compiler
#endif

/**
    @brief Assembly

    Inline assembly

    @trace #BRCM_SWREQ_COMP_ABSTRACTION
*/
#if defined(GNU)
#define COMP_ASM  __asm volatile
#elif defined(ARMCC)
#define COMP_ASM  __asm volatile
#elif defined(GHS)
#define COMP_ASM  __asm volatile
#elif defined(METAWARE)
#define COMP_ASM  __asm volatile
#else
#error Unsupported Compiler
#endif

#endif /* COMPILER_H */
/* @end_no_export_python */
/** @} */
