/*****************************************************************************
 Copyright 2024 Broadcom Corporation.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
#ifndef PCIE_HELPER_H
#define PCIE_HELPER_H

/**
    @name Architecture IDs
    @{
    @brief Architecture IDs for PCIe Driver
*/
#define BRCM_SWARCH_PCIE_SERDES_REGS_IND_RD_PROC                        (0xC101U) /**< @brief #PCIE_SerdesRegsIndRd  */
#define BRCM_SWARCH_PCIE_SERDES_REGS_IND_WR_PROC                        (0xC102U) /**< @brief #PCIE_SerdesRegsIndWr  */

/** @} */

typedef union {

uint32_t w;

struct {

uint32_t SERDES_REG_ADDR      : 4;  // [3:0]

uint32_t SERDES_BLOCK_ADDR    : 12; // [15:4]

uint32_t SERDES_LANE_NUM      : 2;  // [17:16]

uint32_t SERDES_LANE_OFFSET   : 3;  // [20:18]

uint32_t SERDES_UNICAST_BCAST : 6;  // [26:21]

uint32_t SERDES_DEV_ID        : 5;  // [31:27]

} b;

} SERDES_PMI_ADDR_TYPE;


typedef union {

uint32_t w;

struct {

uint32_t PMI_WDATA_REG      : 16; // [15:0]

uint32_t RESERVED_14        : 14; // [29:16]

uint32_t RCMD               :  1; // [30]

uint32_t WCMD               :  1; // [31]

} b;

} SERDES_PMI_WDATA_TYPE;



typedef union {

uint32_t w;

struct {

uint32_t PMI_RDATA_REG : 16; // [15:0]

uint32_t RESERVED_15   : 15; // [30:16]

uint32_t VALID         :  1; // [31]

} b;

} SERDES_PMI_RDATA_TYPE;

int32_t PCIE_SerdesRegsIndRd(uint32_t blk_addr, uint32_t reg_addr, uint32_t broadcast, uint32_t lane_num, uint32_t *rData);

int32_t PCIE_SerdesRegsIndWr(uint32_t blk_addr, uint32_t reg_addr, uint32_t wdata, uint32_t broadcast, uint32_t lane_num);

void PCIE_ConfigMemBusEnable(void);


#endif /* PCIE_HELPER_H */
