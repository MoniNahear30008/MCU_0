/*****************************************************************************
 Copyright 2021-2024 Broadcom Limited.  All rights reserved.

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
/**
    @defgroup grp_macsecdrv_req Requirements
    @ingroup grp_macsecdrv

    @addtogroup grp_macsecdrv_req
    @{

    @file macsecdrv_req.c
    @brief Requirements for MACSec driver
    This file contains the requirements for MACSec driver
    @version 1.1.0 Update macsecdrv requirement names
    @version 1.0.0 Changelist1
*/

/**
   @brief Requirements for MACSec Driver

   MACSec driver controls and configure the the underlying MACSec engine,
   which provides the SecY operations as defined in IEEE 802.1AE specification
   (in this requirement, 801AE specification always means IEEE std 802.1X AE 2018).
   A SecY entity provides the secure transmission of the frames on controlled
   port and insecure transmission of frames on uncontrolled port.

   These are the abbreviations used in the below:
      -# DUT: Device under test where this verification is being done
      -# TS: Test station (peer device) which is connected to DUT (network traffic generator
             with MACSec support)
      -# DCS: default cipher suit (GCM-AES-128)
      -# SAK: Secure association key
      -# SC: Secure channel
      -# SA: Secure association

   MACSec driver shall provide:
        -# Mechanism to create a SecY entity, which provides the
          secure transmission of the frames on controlled port
          and insecure transmission of the frames on uncontrolled port.
        -# Mechanism to get the cipher suites and its capabilities supported by the hardware
        -# Mechanism to select the cipher suite.
        -# Mechanism to create transmit secure channel (TxSC).
        -# Mechanism to create receive secure channel (RxSC).
        -# Mechanism to create transmit secure association (TxSA) with given security key.
        -# Mechanism to create receive secure association (RxSA) with given security key.
        -# Mechanism to enable/disable TxSA.
        -# Mechanism to enable/disable RxSA.
        -# Mechanism to read the SA status: created time, started time, nextPN etc.
        -# Mechanism to delete existing TxSA.
        -# Mechanism to delete existing RxSA.
        -# Mechanism to delete TxSC.
        -# Mechanism to delete RxSC.
        -# Mechanism to allow preconfigured EthType (EAPOL PDUs, ETS PDUs) PDUs to flow in
           plain text (bypass encryption/decryption engine)
        -# Mechanism to read the secure channel and secure association
           statistics as defined the 802.1AE specification.
        -# Notification to client on PN threshold expiry and PN rollover.
        -# Debug APIs like disabling frame protection, replay protection, frame validation
           etc for ease of deployment of MACSec in the network.

   @rationale
   These driver requirements are basically derived from:
      -# Requirements by the KaY entity (defined in 802.1AE spec Annex A.11)
         which needs to control the SecY entity on the MACSec hardware for example,
         creating secure channels, creating secure association, configuring the cipher
         suite for the SA etc.
      -# Requirements by the Management and control interface (defined in 802.1AE spec Annex A.12)
         which needs to provide mechanism to control and configure the SecY entity.

   @analysis
   Driver provides the API to initialize a SecY entity with given parameters which includes:
      -# Configuration for the MAC (MAC speed).
      -# if XPN (extended packet numbering) shall be enabled or not.

   During creation of the SecY, driver configures the underlying engine to only allow KaY packets to
   flow (all other types of the packets shall be dropped by the engine for egress and ingress both until
   secure association is established).Creation of SecY returns a unique ID to client which can be
   used for the creation of SCs and SAs later.

   secure channel and secure association creation:
   After the initialization of the SecY entity, client can call API of the driver to create
   secure channels and secure associations.  Creation of the secure association requires client to:
      -# Provide secure channel on which this SA works
      -# Provide the Association number for the SA
      -# Enable/disable confidentiality and its offset (0/30/50) for transmit SA.
      -# key (SAK) to be used for this SA along with the Salt value (for XPN ciphers)
      -# if its transmit SA, NextPN to be used for this SA.
      -# if its receive SA, lowestPN and nextPN values for this SA.

   Creation of the Secure association returns a unique handle to client which can be later used to
   enable/disable or delete this SA.  Also, when SA is created,
   it is kept in disabled state unless client explicitly enabled it later.

    SA Enable/Disable:
    Driver provides APIs to enable/disable the SA at run time. When SA is disabled, all the
    packets arriving to the corresponding secure channel is dropped.
    (equivalent to "controlledPortEnabled" set to false as per 1AE spec). When SA is enabled,
    data traffic is allowed to flow through this SA.

    SA status:
    Client can get the status of the SA by calling driver API which provides information about:
        -# inUse (boolean true/false): True if this SA is enabled, false otherwise.
        -# timeCreated: Time stamp when this SA was created.
        -# timeStarted: Time stamp when this SA was enabled.
        -# timeStopped: Time stamp when this SA was disabled.
        -# ssci: SSCI value for this SA.
        -# key id: 16-bytes of the key id corresponding to this SA.

    Statistics counters:
        -# Statistics counters per Secure channel and per SA
        -# Stats are defined as per Table 13-7 of the MACSec specification.

    Notifications:
        -# When PN number exceeds the configured threshold, a notification is send to client (event).
           It can be used by the the client to re-establish the new secure association with
           new SAK before the PN rollover.
        -# When PN number rolls over (sequence number reaches to 0xFFFFFFFF for 32-bit PN or
           0xFFFFFFFFFFFFFFFF for 64-bit PN), SA is disabled and a notification is send to client
           (event). Client can use this notification to re-establish the fresh association or
           take any other action.

    @verification
        -# Integrity protection only:
            -# When DUT and TS both are configured to use integrity only, DUT shall not encrypt
               the frame data while sending and it shall only verify the ICV for the received
               frames from the TS (no decryption of the frame data).
            -# DUT shall send the packet with SecTAG (E-bit set to 0) and ICV appended and
               TS shall accept the frame.
            -# DUT shall accept frames with valid ICV and drop invalid frames (wrong ICV).
            -# Corresponding stats counters shall be incremented (refer to Table 13-7 of IEEE
               spec 802.1AE-2018).
        -# Integrity with confidentiality:
            -# When DUT and TS both are configured to use integrity with confidentiality, sender
               shall encrypt the payload, append ICV to frame and receiver shall verify the ICV
               and decrypt the frame data.
            -# DUT shall send the packet with SecTAG E bit set and ICV appended to frame. Frame
               payload (everything after SecTAG) shall be encrypted.
            -# DUT shall accept valid frames (with correct ICV value), decrypt it successfully.
            -# DUT shall drop invalid frames (with wrong ICV value).
            -# Corresponding stats counters shall be incremented (refer to Table 13-7 of IEEE
               spec 802.1AE-2018).
        -# Integrity with confidentiality offset:
            -# When DUT and TS both are configured to use integrity with confidentiality offset,
               a selected number of initial octets of the frame (30 or 50 octets), are only
               integrity protected and appears in the plain (no encryption) immediately after the
               SecTAG in frame.
            -# DUT shall send the packet with SecTAG E-bit set and ICV appended to frame. Frame
               payload (excluding the initial octets selected by confidentiality offset) shall
               be encrypted.
            -# SA statistics counters shall correctly provide the number of packets being encrypted.
        -# VLAN in clear:
            -# DUT can be configured to allow 802.1Q tagged packet (ethType = 0x8100) to be clear
               text (after MAC SA and before SecTAG) in MACSec frame.
            -# 802.1Q tag shall not be part of the confidentiality and integrity protection and
               shall appear before the SecTAG.
            -# DUT shall append SecTAG after the 802.1Q tag while sending the frame.
            -# DUT shall compute the ICV for the whole frame excluding the 802.1Q tag and verify it
               and decrypt the frame data (if confidentiality is enabled in SecTAG).
        -# Frame bypass mode for transmit/receive:
            -# When DUT is configured to bypass specific frame on transmit path, it shall bypass
               confidentiality protection and integrity addition for the frame and send it as is
               (without any modification).
            -# When DUT is configured to bypass a specific frame on the receive path,
               it shall pass that frame without any modification.
        -# No data flow on PN rollover
            -# When the PN reaches to its max value (2^32-1 for 32-bit and 2^64-1 for 64-bit PN),
               SA is automatically disabled.
            -# If this is a transmit SA, all the frames submitted to this SA for trasmission
               shall be dropped.
            -# If its a receive SA, all the frames received on this SA, shall be dropped.
            -# A notification shall be sent to upper layer about SA expriry event.
        -# Notification on transmit SA threshold:
            -# DUT can be configured to notify when transmit SA reaches a threshold value.
            -# DUT shall send an event to upper layer when the threshold is hit.
        -# Implicit SCI
            -# If DUT is connected to at most one peer (operPointToPointMAC set TRUE:
               IEEE spec 802.1AE-2018 section 6.5), DUT shall accept the frame with
               SecTAG having SC bit = 0 and ES bit = 0.
            -# When DUT is configured to not use explicit SCI for SecY or operPointToPointMAC
               is true, it shall not send SCI information as part of the MACsec frame
               (SecTAG shall have SC bit = 0 and ES bit = 0).

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_UTILS
	@dependencies #BRCM_SWREQ_MACSEC_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_SECY
    @dependencies #BRCM_SWREQ_MACSEC_SYSMISC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_TCAMKM_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_TCAMKP_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_CLS_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_CLS_P_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_CLS_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_CLS_P_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_EIP62_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_EIP62_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_MSPU_MAC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_MSPU_TOP_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_TCAMKM_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_TCAMKP_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_FCTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_FCTRL_EN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_OPPE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_SECY_CNTRS_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_SECY_CNTRS_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_SA_CNTRS_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_SA_CNTRS_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_SA_CNTRS_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_SECY_CNTRS_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_SA_CNTRS_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_SECY_CNTRS_CTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_FCTRL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_FCTRL_EN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_RXSC_CAMKEY_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_RXSC_CAMEN_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_SCSA_MAP_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_SCSA_MAP_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_TRANSREC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_TRANSREC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_BRCM_GLOBAL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_BRCM_GLOBAL_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_EIP165_AIC_REGS_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_EIP160_AIC_REGS_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_ETHXCVR_STATUS
    @dependencies #BRCM_SWREQ_ETHXCVR_COMMON
    @vtrace #MACSEC_MAX_SECY_PER_PORT
	@vtrace #MACSEC_MAX_PORT_NUM
    @vtrace #MACSEC_MAX_RXSC_PER_SECY
    @vtrace #MACSEC_TIMESTAMP_PRESCALE
	@vtrace #AES_ECBEncryptBlock
	@vtrace #CRYPTO_KEY_AES128
	@vtrace #CRYPTO_KEY_AES256
	@vtrace #BCM_CRC16
	@vtrace #BCM_STATE_RUN
	@vtrace #RESERVED_FOR_FUTURE
	@vtrace #CRYPTO_KeyType
    @vtrace #MACSECDRV_LEGACY
	@vtrace #MACSEC_SAThrdExpInd
	@vtrace #MACSEC_SAExpiredInd
	@vtrace #MCU_CHIP_MODEL_MASK
	@vtrace #MCU_CHIP_MODEL_8958X
	@vtrace #MACSEC_HWERR_EVENT_PKT_INPUT_ERR
	@vtrace #MCU_CHIP_REVISION_A0
	@vtrace #MCU_VersionType
	@vtrace #MACSEC_HWERR_EVENT_FATAL_ERR
	@vtrace #MACSEC_HWERR_EVENT_PKT_PROCESS_ERR
	@vtrace #MACSEC_HWERR_EVENT_CONTEXT_ERR
	@vtrace #MACSEC_HwErrInd
	@vtrace #MCU_GetVersion
	@vtrace #MCU_CHIP_REVISION_B0
	@vtrace #MACSEC_IRQ_SUPPORT_DISABLE
       @cybersecurity no
       @funcsafety no
       @systemrequirement RGL-5524
*/
#define BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

/**
    @brief MACSec Kernel interface

    Define command IDs and relevant data structures for TIM driver to process in
    privileged space.

    @rationale
    Driver APIs are expected to be executed in unprivileged space, and driver
    internally needs to switch to privileged space for access to hardware
    registers and driver data structures. This should act as a bridge to feed
    the data from unprivileged space to privileged space.

    @analysis
      -# Define the SVC commands for user space functions
      -# Define data structure which holds the user space parameters and commands
         (SVC structure). This structure shall contains the return value field,
         which shall be populated by the privileged handler to return the status
         of the processed command.
      -# When user space function is called, populate the required fields in the
         SVC structure and call the SVC request function to switch to privileged
         space.
      -# Check the status of the command and return status to user.

    @verification
        -# Review commands and correspoding parameters in the SVC structure field
           are populated correctly.

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_SECY
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/

#define BRCM_SWREQ_MACSEC_KERNEL_INTERFACE

/**
    @brief MACSec Kernel interface

    Define command IDs and relevant data structures for TIM driver to process in
    privileged space.

    @rationale
    Driver APIs are expected to be executed in unprivileged space, and driver
    internally needs to switch to privileged space for access to hardware
    registers and driver data structures. This should act as a bridge to feed
    the data from unprivileged space to privileged space.

    @analysis
      -# Define the SVC commands for user space functions
      -# Define data structure which holds the user space parameters and commands
         (SVC structure). This structure shall contains the return value field,
         which shall be populated by the privileged handler to return the status
         of the processed command.
      -# When user space function is called, populate the required fields in the
         SVC structure and call the SVC request function to switch to privileged
         space.
      -# Check the status of the command and return status to user.

    @verification
        -# Review commands and corresponding parameters in the SVC structure field
           are populated correctly.

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_SECY
    @dependencies #BRCM_SWREQ_BCM_UTILS
    
    @vtrace #MACSECDRV_SYSCMD_HANDLER
    @vtrace #ENABLE_MACSECDRV_MDIO
    @vtrace #ENABLE_MACSECDRV_AXI
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/

#define BRCM_SWREQ_MACSECDRV_KERNEL_INTERFACE

/**
    @brief MACSec Kernel Handler

    Defines driver kernel handler to process commands issued by client.

    @rationale
    To switch to privileged mode for reading/writing to hardware based
    on the command passed from user space.

    @analysis
      -# Install the MACSec driver SVC handler in the SVC table.
      -# In SVC command handler, verify the SVC magic.
      -# Verify the command ID and process it.

    @verification
        -# Review command handler consistency for integration to SVC function
           tables.

    @dependencies #BRCM_SWREQ_BCM_COMPONENT
    @dependencies #BRCM_SWREQ_BCM_OS_WRAPPER
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_BCM_STDLIB_WRAPPER
    @dependencies #BRCM_SWREQ_COMP_ABSTRACTION
    @dependencies #BRCM_SWREQ_CPU_ABSTRACTION
    @dependencies #BRCM_SWREQ_MACSECDRV_KERNEL_INTERFACE
    @dependencies #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
    @dependencies #BRCM_SWREQ_MACSEC_E_EIP62_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_I_EIP62_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_E_OPPE_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_MACSEC_MSPU_MAC_RDB_REFERENCE
    @dependencies #BRCM_SWREQ_SECY
    @dependencies #BRCM_SWREQ_BCM_UTILS

    @vtrace #MACSECDRV_IRQ_PROCESS_TASK
    @vtrace #MACSECDRV_SA_THRLD_EXPIRED_EVENT
    @vtrace #MACSECDRV_SA_EXPIRED_EVENT
    @vtrace #MACSECDRV_HWERR_EVENT
    @cybersecurity no
    @funcsafety no
    @systemrequirement NONE
*/
#define BRCM_SWREQ_MACSECDRV_KERNEL_HANDLER
/** @} */
