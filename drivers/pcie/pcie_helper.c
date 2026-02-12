/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
#include <bcm_utils.h>
#include <bcm_err.h>
#include <pcie_helper.h>
#include "rdb/cfg_aer_rdb.h"
#include <pcie.h>

/**
    @name PCIe Driver Design IDs
    @{
    @brief Design IDs for PCIe Driver
*/
#define BRCM_SWDSGN_PCIE_REG_CFG_AER_MACRO                              (0xB100U) /**< @brief #PCIE_REG_HSIO_COM       */
#define BRCM_SWDSGN_PCIE_SERDES_REGS_IND_RD_PROC                        (0xB101U) /**< @brief #PCIE_SerdesRegsIndRd  */
#define BRCM_SWDSGN_PCIE_SERDES_REGS_IND_WR_PROC                        (0xB102U) /**< @brief #PCIE_SerdesRegsIndWr  */
#define BRCM_SWDSGN_PCIE_CONFIG_MEM_BUS_ENABLE_PROC                     (0xB103U) /**< @brief #PCIE_ConfigMemBusEnable  */

/** @} */


/**
    @brief PCIE indirect address register.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_INTERRUPT_CONFIG_SET_PROC
*/
#define PCIE_REG_CFG_AER            ((CFG_AER_RDBType       *)CFG_AER0_BASE)

/**
    @brief PCIE indirect address register Read.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC

*/
int32_t PCIE_SerdesRegsIndRd(uint32_t blk_addr, uint32_t reg_addr, uint32_t broadcast, uint32_t lane_num, uint32_t *rData)
{

    SERDES_PMI_ADDR_TYPE serdes_pmi_addr_reg;

    SERDES_PMI_WDATA_TYPE serdes_pmi_wdata_reg;

    SERDES_PMI_RDATA_TYPE serdes_pmi_rdata_reg;

    int valid_timeout = 100;
    serdes_pmi_addr_reg.w = 0UL;
    serdes_pmi_wdata_reg.w = 0UL;
    serdes_pmi_rdata_reg.w = 0UL;

    PCIE_REG_CFG_AER->config_ind_addr = 0x1130UL;/* serdes_pmi_addr */


    serdes_pmi_addr_reg.b.SERDES_DEV_ID = 0x1UL;

    if (broadcast == 0x1UL) {

    serdes_pmi_addr_reg.b.SERDES_UNICAST_BCAST = 0xFUL;

    serdes_pmi_addr_reg.b.SERDES_LANE_OFFSET = 0x7UL;

    }

    else {

    serdes_pmi_addr_reg.b.SERDES_UNICAST_BCAST = 0x0UL;

    serdes_pmi_addr_reg.b.SERDES_LANE_OFFSET = 0x0UL;

    }

    if (broadcast == 0x1UL) {

    serdes_pmi_addr_reg.b.SERDES_LANE_NUM = 0x3UL;

    } else {

    serdes_pmi_addr_reg.b.SERDES_LANE_NUM = lane_num;

    }

    serdes_pmi_addr_reg.b.SERDES_BLOCK_ADDR = blk_addr;

    serdes_pmi_addr_reg.b.SERDES_REG_ADDR = reg_addr;


    PCIE_REG_CFG_AER->config_ind_data = serdes_pmi_addr_reg.w;

    BCM_DelayUs(10UL);


    PCIE_REG_CFG_AER->config_ind_addr = 0x1134UL; /* serdes_pmi_wdata */
    BCM_DelayUs(10UL);

    serdes_pmi_wdata_reg.b.WCMD = 0x0UL;

    serdes_pmi_wdata_reg.b.RCMD = 0x1UL;

    PCIE_REG_CFG_AER->config_ind_data = serdes_pmi_wdata_reg.w;

     BCM_DelayUs(10UL);


    PCIE_REG_CFG_AER->config_ind_addr = 0x1138UL; /*  serdes_pmi_rdata */


    do {

      serdes_pmi_rdata_reg.w = PCIE_REG_CFG_AER->config_ind_data;
      BCM_DelayUs(10UL);
      valid_timeout--;

    } while ((serdes_pmi_rdata_reg.b.VALID == 0UL) && (valid_timeout > 0));



    if ((serdes_pmi_rdata_reg.b.VALID == 0UL) && (valid_timeout == 0)) {
       return BCM_ERR_TIME_OUT;
    }

    *rData = serdes_pmi_rdata_reg.w;

    return BCM_ERR_OK;

}

/**
    @brief PCIE indirect address register write.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC

*/
int32_t PCIE_SerdesRegsIndWr(uint32_t blk_addr, uint32_t reg_addr, uint32_t wdata, uint32_t broadcast, uint32_t lane_num)
{

    SERDES_PMI_ADDR_TYPE serdes_pmi_addr_reg;

    SERDES_PMI_WDATA_TYPE serdes_pmi_wdata_reg;

    serdes_pmi_addr_reg.w = 0UL;
    serdes_pmi_wdata_reg.w = 0UL;

    PCIE_REG_CFG_AER->config_ind_addr = 0x1130UL; /* serdes_pmi_addr */


    serdes_pmi_addr_reg.b.SERDES_DEV_ID = 0x1UL;

    if (broadcast == 0x1UL) {

      serdes_pmi_addr_reg.b.SERDES_UNICAST_BCAST = 0xFUL;

      serdes_pmi_addr_reg.b.SERDES_LANE_OFFSET = 0x7UL;

    }

    else {

      serdes_pmi_addr_reg.b.SERDES_UNICAST_BCAST = 0x0UL;

      serdes_pmi_addr_reg.b.SERDES_LANE_OFFSET = 0x0UL;

    }

    if (broadcast == 0x1UL) {

      serdes_pmi_addr_reg.b.SERDES_LANE_NUM = 0x3UL;

    } else {

      serdes_pmi_addr_reg.b.SERDES_LANE_NUM = lane_num;

    }

    serdes_pmi_addr_reg.b.SERDES_BLOCK_ADDR = blk_addr;

    serdes_pmi_addr_reg.b.SERDES_REG_ADDR = reg_addr;


    PCIE_REG_CFG_AER->config_ind_data = serdes_pmi_addr_reg.w;

    BCM_DelayUs(10UL);


    PCIE_REG_CFG_AER->config_ind_addr = 0x1134UL; /* serdes_pmi_wdata */
    BCM_DelayUs(10UL);

    serdes_pmi_wdata_reg.b.WCMD = 0x1UL;
    serdes_pmi_wdata_reg.b.RCMD = 0x0UL;
    serdes_pmi_wdata_reg.b.PMI_WDATA_REG = wdata;
    PCIE_REG_CFG_AER->config_ind_data = serdes_pmi_wdata_reg.w;

    return BCM_ERR_OK;
}

/**
    @brief Enable Bus mater and memory space access for the Endpoint.

    @trace #BRCM_SWREQ_PCIE_DRV
    @trace #BRCM_SWARCH_PCIE_RC_INIT_PROC
    @trace #BRCM_SWARCH_PCIE_EP_INIT_PROC

*/
void PCIE_ConfigMemBusEnable(void)
{
    uint32_t value0 = 0UL;
    PCIE_DrvCfgRead(0x4UL,&value0);
    value0 |= 6UL;
    PCIE_DrvCfgWrite(0x4UL,value0); /* write Status Command to enable Master and mem-enable*/
}

