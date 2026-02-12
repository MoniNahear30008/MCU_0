/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/* Linker script for building ROM test app for bcm8915x target */

main_stack_size = 0x2000; /* 8KB */
data_size = 0x2000; /* 8KB */
bss_size = 0x4000; /* 8KB */
patch_region_size  = 0x800;   /* 2KB */
header_size = 0x100;
bl_sz = 0x200000 - 0x8000 - header_size;
rom_drv_scratch_pad = 0x6000;  /* 24KB */
rom_drv_scratch_size = 0x8000; /* 32KB */
debug_log_size    = 0x600;     /* 1536B */
debug_log_start = ORIGIN(DTCM) + LENGTH(DTCM) - debug_log_size;
patch_region_start = debug_log_start - patch_region_size;
BL_ALIGN_SIZE = 128;

/* Define output sections */
SECTIONS
{
    . = ORIGIN(SRAM);
    .text_hdr (NOLOAD):
    {
        __text_header__ = .;
        . += header_size;
        __text_header_end__ = .;
    } > SRAM

    .text :
    {
        __text_start__ = .;
        KEEP(*(.reset_func*))
        . = 768;              /* Align to 1024 */
        *(.vector_tbl*)
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        __version_data_start__ = .;
        KEEP(*(.version*))
        __version_data_end__ = .;
        KEEP(*(.vcast.ro*))
        . = ALIGN(BL_ALIGN_SIZE);
        __text_end__ = .;
        size_check = (__text_end__ <= bl_sz);
        ASSERT(size_check, "***LINKER ERROR: .text section overflows the limit.");
    } > SRAM

    /* Download area for loader testing */
    .download ALIGN(1024) (NOLOAD):
    {
        __download_area0_start__ = .;
        *(.download.area*)
        __download_area0_end__ = .;
    } > SRAM

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(.bss.*)
        *(COMMON)
        *(.bss.err*)
        . = ALIGN(4);
        . = . + bss_size;
        __bss_end__ = .;
    } > SRAM

    .rom_drv_scr (NOLOAD):
    {
        __rom_drv_scratch_section_start__ = .;
        . = ALIGN(4);
        . = . + rom_drv_scratch_pad;
        . = ALIGN(4);
        __rom_drv_scratch_start__ = .;
        *(.crypto.drivers.*)
        . = __rom_drv_scratch_section_start__ + rom_drv_scratch_size;
        __rom_drv_scratch_end__ = .;
    } > ROM_DRV_SCR

    . = ORIGIN(DTCM);

    .privileged.data (NOLOAD) :
    {
        . = ALIGN(8);
        . = . + main_stack_size;
        __main_stack__ = .;
         __task_stack_start__ = .;
        . = ALIGN(4);
        __privileged_bss_start__ = .;
        __bss_os_start__ = .;
        *(.bss.os*)
        __bss_os_end__ = .;
        __drivers_bss_start__ = .;
        *(.bss.drivers*)
        __drivers_bss_end__ = .;
        . = ALIGN(4);
        __privileged_bss_end__ = .;
    } > DTCM

    .data :
    {
        . = ALIGN(4);
        . = . + data_size;
        . = ALIGN(4);
    } > DTCM

    /* Patch exe region is towards the end of DTCM
       Data can be allocated from above (whatever is left out by ROM) */
    .patch patch_region_start(NOLOAD):
    {
        __patch_area_start__ = .;
        *(.patch.PATCH_PatchRegion)
        __patch_area_end__ = .;
    } > DTCM

    /* Debug log region is at the end of DTCM */
    .debug_log debug_log_start(NOLOAD):
    {
        __debug_log_area_start__ = .;
        *(.debug_buffer*)
        __debug_log_area_end__ = .;
    } > DTCM

    . = ORIGIN(FLASH);
    .flash (NOLOAD):
    {
        *(.imgl.mmap.addr0*)
    } > FLASH

}
