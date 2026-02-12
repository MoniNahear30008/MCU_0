#
# Copyright 2024 Broadcom Limited.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##  @defgroup grp_macsecdrv MACSec driver
#   @ingroup grp_ethernet
#
#   @addtogroup grp_macsecdrv
#   @{
#    @section sec_macsec_overview Overview
#    MACSec protocol is defined in IEEE 802.1AE specification, which provides secure communication
#    between trusted components in the network. When MACSec is enabled, a bi-directional secure
#    link is established after the exchange and verification of the security keys between the
#    connected devices. Once the trust is established, integrity and data encryption is used to
#    safe-guard the transmitted data over the wire. We must note that MACsec cannot protect against
#    attacks facilitated by the trusted components themselves.
#
#   @file
#   @brief Makefile for MACSec Driver
#   @version 0.1 Initial version
#   @}

BRCM_COMP_NAME := macsecdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

BRCM_COMP_REQ_SRC += doc/macsecdrv_req.c
BRCM_COMP_REQ_SRC += doc/crypto_secy_req.c
BRCM_COMP_REQ_SRC += doc/macsec_rdb_req.c
