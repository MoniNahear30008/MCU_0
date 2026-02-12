/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
 ******************************************************************************/

/** @defgroup grp_dma330drv_req Requirements
    @ingroup grp_dma330drv

    @addtogroup grp_dma330drv_req
    @{

    This is a requirement to implement the DMA330 driver

    @file dma330_req.c
    @brief Requirements for DMA330 driver
    @version 0.1 Initial version
*/

/**
    @brief DMA330 initialization and de-initialization

    DMA330 driver shall provide a mechanism for initialization and un-initialize DMA330
    driver after all the operations are completed.

    @rationale
    These driver requirements are basically derived from:
    -# Requirement by Rigel_External_Architecture_Specification_v0.6, Which need to
       transfer data from HSADC to GPSRAM.

    @analysis
    Driver provides the API to initialize a DMA330 hardware instance with given parameters which includes:
      -# Channel configurations
      -# Channel Size
    During Initialization of DMA driver updates,
      -# Boot control
      -# code ram address

    @verification
      -# Channel size
         -# DMA330 supports max of DMA_CHANNEL_ID_MAX channels. Master can configure one or more channels.
      -# Code Ram
         -# Master needs to provide separate code ram address for each channels

    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_DMA330_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_CLKRST_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CPU_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE

*/
#define BRCM_SWREQ_DMA330_INIT

/**
    @brief Configure DMA330 xfer setup, trigger and IRQ

    DMA330 driver shall provide a mechanism for setting up xfer, trigger and sample interrupt handling.

    @rationale
    These driver requirements are basically derived from:
    -# Requirement by Rigel_External_Architecture_Specification_v0.6, Which need to
       transfer data from through,
       -# Memory to Memory
       -# Memory to Peripheral
       -# Peripheral to Memory

    @analysis
    Driver provides the API to configure transfers, trigger on DMA330 hardware instance with given parameters which includes:
      -# Request type
      -# Channel Configuration type
    During Transfer setup of DMA driver updates,
      -# Code address based on,
          -# Transfer type, Src/Dst skip
    During Trigger of DMA driver updates,
      -# Code address to DMA hardware and
      -# Enables interrupts

    @verification
      -# Transfer Setup
         -# DMA330 updates opcode to the code ram address.
      -# Trigger
         -# Data transfer triggered based on masters request.
         -# Upon transfer completion DMA trigger interrupt to master.

    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_DMA330_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_DMA330_INIT

    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_DMA330_V1_DRIVER


/** @} */
