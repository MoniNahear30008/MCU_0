/*****************************************************************************
 Copyright 2025 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/** @defgroup grp_driver_req Driver Requirements
    @ingroup grp_driver

    @addtogroup grp_driver_req
    @{

    @file driver_req.c
    @brief Driver Requirements

    @version 0.1.0 Initial version
*/

/**
    @brief Provide guideline for driver development
      -# Software shall be portable with different compilers
      -# Software shall be completely reusable with different configurations on different platforms/component
      -# Software shall have static design to the extent possible
    @rationale
      -# This is bare metal driver without any OS support.
      -# The components are not directly involved in cybersecurity related activities. Hence software component
       architecture is to be considered as cybersecurity irrelevant.
      -# Since components are neither part of the fault detection nor the fault reporting pipeline, component
       architecture is functional safety irrelevant.
    @analysis
      -# The components shall be modular in design to ease usage independently.
      -# The drivers are bare metal drivers. The user has to implement asynchronous event/interrupt reporting
      mechanism.
      -# The driver design shall follow static design. Static design ensures safety at a large extent. 
         It also reduces overhead of dynamic analysis to a large extent. 
         Dynamic memory allocations shall be avoided for data structures.
    @alternate_arch None
    @verification
      -# Component configuration is available to platforms/applications.
      -# Added component is modular enough to reuse between applications and platforms.
      -# Added component is extendable to add new features.
    @cybersecurity no
    @funcsafety no

*/
#define BRCM_SWREQ_DRIVER
/** @} */
