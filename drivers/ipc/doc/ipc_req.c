/*****************************************************************************
 Copyright 2025 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/

/** @defgroup grp_ipcdrv_req Requirements
    @ingroup grp_ipcdrv

    @addtogroup grp_ipcdrv_req
    @{
    @section grp_ipcdrv_req_overview Requirements Overview

    This is a requirement to implement the ipc driver

    @file ipcdrv_req.c
    @brief Requirements for the ipc driver
    @version 0.1 Initial version
*/

/**
    @brief IPC driver requirement

    IPC driver shall implement the following functionality:
        -# Setup and initialize the IPC channels
        -# Provide an interface for the system SW to send commands to the
           remote parties. Also provide a mechanism to notify the sender once
           the remote party finishes processing the sent command.
        -# Similarly, provide a mechanism to recieve commands from a remote party
           and dispach them to a designated piece of SW executing locally. Also
           provide a mechanism for that SW to signal the completion of the command
           processing.

    @rationale
    For various processors in the system to be able to request services from one
    another.

    @analysis
    API's shall be provided to init and deinit the driver. An access policy to the IPC
    recources shall be defined at build-time and a "master" CPU shall be responsible
    for setting up the IPC such that these policies are enforced at runtime.

    Apart from that, command exchange should just amount to programming the appropriate
    IPC registers and handling IPC interrupts.

    @verification
    -# Able to enforce the IPC access policies
    -# Able to send and receive the IPC messages

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_UTILS
    @dependencies #BRCM_SWREQ_IPC_RDB_REFERENCE
    @cybersecurity no
    @funcsafety no
    @systemrequirement RGL-5471

*/
#define BRCM_SWREQ_IPC

/** @} */
