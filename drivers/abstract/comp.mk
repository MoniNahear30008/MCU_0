#
# Copyright 2024 Broadcom.  All rights reserved.
#
# Broadcom Proprietary and Confidential
#
####################################################################################

##   @defchapter grp_driver Peripheral Drivers
#   
#   @addtogroup grp_driver
#   @{
#   Peripheral Driver interface to user SDK
#   <table>
#   <tr> <th> Peripheral Name </th><th>Class</th> <th>Mode of Transfer</th><th>Driver Name</th><th>Driver API Description</th></tr>
#   <tr><td>Clock and Reset </td><td>System Driver </td><td>NA</td><td>clkrst_drv.c  </td><td>@ref grp_clkrstdrv </td></tr>
#   <tr><td>QSPI </td><td>IO Driver </td><td>Interrupt or DMA </td><td>qspi.c  </td><td>@ref grp_qspidrv </td></tr>
#   <tr><td>I2C </td><td>IO Driver </td><td>Interrupt </td><td>i2c.c   </td><td> @ref grp_i2cdrv</td></tr>
#   <tr><td>CAN </td><td>IO Driver </td><td>Interrupt </td><td>can.c   </td><td>@ref grp_candrv </td></tr>
#   <tr><td>PCIe </td><td>IO Driver </td><td>DMA </td><td>pcie.c   </td><td>@ref grp_pciedrv </td></tr>
#   <tr><td>Ethernet </td><td>IO Driver </td><td>DMA </td><td>xgmac_drv.c   </td><td>@ref grp_xgmacdrv </td></tr>
#   <tr><td>MACSEC </td><td>MACSEC Driver </td><td>DMA </td><td>macsec.c   </td><td>@ref grp_macsecdrv </td></tr>
#   <tr><td>GPIO </td><td>IO Driver </td><td>NA</td><td>gpio.c  </td><td>@ref grp_gpiodrv </td></tr>
#   <tr><td>Safety Island </td><td>System Driver </td><td>Interrupt</td><td>safetyisland_drv.c  </td><td>@ref grp_safetyislanddrv </td></tr>
#   <tr><td>HSREF </td><td>IO Driver </td><td>DMA </td><td>hsref.c </td><td>@ref grp_hsrefdrv </td></tr>
#   <tr><td>TXSLZR </td><td>IO Driver </td><td>DMA </td><td>txslzr.c </td><td>@ref grp_txslzrdrv </td></tr>
#   <tr><td>ACQCMN </td><td>IO Driver </td><td>DMA </td><td>acqcmn.c </td><td>@ref grp_acqcmndrv </td></tr>
#   <tr><td>Cache </td><td>System Driver </td><td>Interrupt</td><td>cache.c  </td><td>@ref grp_cachedrv </td></tr>
#   <tr><td>SDC </td><td>System Driver </td><td>Interrupt</td><td>sdc.c  </td><td>@ref grp_sdcdrv </td></tr>
#   <tr><td>HSAFE</td><td>IO Driver </td><td>NA</td><td>hsafe.c  </td><td>@ref grp_hsafedrv </td></tr>
#   <tr><td>NSU</td><td>IO Driver </td><td>NA</td><td>nsu_drv.c </td><td>@ref grp_nsudrv </td></tr>
#   <tr><td>Interrupt Controller </td><td>System driver </td><td>Interrupt</td><td>intrctrl.c  </td><td>@ref grp_intrctrldrv </td></tr>
#   <tr><td>HKADC </td><td>IO Driver </td><td>DMA </td><td>hkadc.c </td><td> @ref grp_hkadcdrv</td></tr>
#   <tr><td>HSADC </td><td>IO Driver </td><td>DMA </td><td>hsadc.c </td><td> @ref grp_hsadcdrv</td></tr>
#   <tr><td>HKDAC </td><td>IO Driver </td><td>DMA </td><td>hkdac.c </td><td> @ref grp_hkdacdrv</td></tr>
#   <tr><td>IPC</td><td>System Driver </td><td>NA</td><td>ipc_drv.c </td><td>@ref grp_ipcdrv </td></tr>
#   <tr><td>WDT</td><td>System Driver </td><td>Interrupt</td><td>wdt_drv.c </td><td>@ref grp_wdtdrv </td></tr>
#   <tr><td>UART</td><td>IO Driver </td><td>NA</td><td>uart_drv.c </td><td>@ref grp_uartdrv </td></tr>
#   <tr><td>AWG</td><td>IO Driver </td><td>NA</td><td>awg_drv.c </td><td>@ref grp_awgdrv </td></tr>
#   <tr><td>DMA330</td><td>System Driver </td><td>Interrupt</td><td>dma330_drv.c </td><td>@ref grp_dma330drv </td></tr>
#   <tr><td>PL08x</td><td>System Driver </td><td>Interrupt</td><td>pl08x.c </td><td>@ref grp_dma_v2 </td></tr>
#   <tr><td>Unicam</td><td>System Driver </td><td>Interrupt</td><td>unicam_drv.c </td><td>@ref grp_unicamdrv </td></tr>
#   
#   </table>
#   @file drivers/abstract/comp.mk
#   @brief Makefile for System Abstractions
#   @version 0.1 Initial version
#
#   @}
#

BRCM_COMP_NAME := driver 
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

# Requirement documentation
BRCM_COMP_REQ_SRC += driver_req.c
