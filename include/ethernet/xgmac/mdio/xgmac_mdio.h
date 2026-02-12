/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/

/**
    @defgroup grp_xgmacmdio_drv_ifc PHY MDIO Interface
    @ingroup grp_xgmacdrv

    @addtogroup grp_xgmacmdio_drv_ifc
    @{
    @section sec_xgmac_mdio_overview Overview

    Ethernet MDIO controller to interface and control PHY slave devices.
        -# Station management Agent (SMA) supports the application in accessing any PHY registers through a two-wire
           interface.
        -# Supports both clause 45 and clause 22 MDIO protocol.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   XGMAC_0      |

    @section api_list List of MDIO APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref XGMAC_MdioInit | Initialize MDIO bus |
    | @ref XGMAC_MdioDeInit | De-initialize MDIO bus  |
    | @ref XGMAC_MdioWrite | Write the data to slave device using MDIO |
    | @ref XGMAC_MdioRead | Read the data from slave device using MDIO |
    | @ref XGMAC_MdioReset | XGMAC packet filter enable |
    @file xgmac_mdio.h
    @brief Interface for XGMAC MDIO driver
    This header file defines interface for XGMAC MDIO driver.

    @version 0.1 Initial Version
*/

#ifndef XGMAC_MDIO_H
#define XGMAC_MDIO_H

#include <xgmac.h>

/**
    @name XGMAC MDIO API IDs
    @{
    @brief API IDs for XGMAC MDIO Driver
*/
#define BRCM_SWARCH_XGMAC_MDIO_CLOCK_RANGE_SEL_TYPE       (0x8600U) /**< @brief #XGMAC_MdioClockRangeSelType  */
#define BRCM_SWARCH_XGMAC_CSR_CLK_DIVIDER_VALUE_TYPE      (0x8601U) /**< @brief #XGMAC_CsrClkDividerValueType */ 
#define BRCM_SWARCH_XGMAC_MDIO_ACC_CLAUSE_TYPE            (0x8602U) /**< @brief #XGMAC_MdioAccClauseType      */
#define BRCM_SWARCH_XGMAC_MDIO_BUS_CFG_TYPE               (0x8603U) /**< @brief #XGMAC_MdioBusCfgType         */
#define BRCM_SWARCH_XGMAC_MDIO_INIT_PROC                  (0x8604U) /**< @brief #XGMAC_MdioInit               */
#define BRCM_SWARCH_XGMAC_MDIO_DEINIT_PROC                (0x8605U) /**< @brief #XGMAC_MdioDeInit             */
#define BRCM_SWARCH_XGMAC_MDIO_WRITE_PROC                 (0x8606U) /**< @brief #XGMAC_MdioWrite              */
#define BRCM_SWARCH_XGMAC_MDIO_READ_PROC                  (0x8607U) /**< @brief #XGMAC_MdioRead               */
#define BRCM_SWARCH_XGMAC_MDIO_RESET_PROC                 (0x8608U) /**< @brief #XGMAC_MdioReset              */
/** @} */

/**
    @name XGMAC_MdioClockRangeSelType
    @{
    @brief MDIO clock range select

    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
 */
typedef uint32_t XGMAC_MdioClockRangeSelType;
#define XGMAC_MDIO_CLOCK_RANGE_SEL_NORMAL   0  /**< @brief Clock range select in normal mode. */
#define XGMAC_MDIO_CLOCK_RANGE_SEL_FAST     1  /**< @brief Clock range select for fast MDIO clock generation. */
/** @} */

/**
    @name XGMAC_CsrClkDividerValueType
    @{
    @brief MDIO clock generation using CSR clock when CRS bit is set for fast MDC clock generation.

    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
 */
typedef uint32_t XGMAC_CsrClkDividerValueType;  /**< If clock range select is set as 1, it generate MDC clcok based on following divider values. */
#define XGMAC_CSR_CLK_DIVIDER_VALUE_4  (0U)         /**<  If CRS bit set, divider value 4 is used to generate MDC clock frequency: csr_clk_i/4 */
#define XGMAC_CSR_CLK_DIVIDER_VALUE_6  (1U)         /**<  If CRS bit set, divider value 6 is used to generate MDC clock frequency: csr_clk_i/6 */
#define XGMAC_CSR_CLK_DIVIDER_VALUE_8  (2U)         /**<  If CRS bit set, divider value 8 is used to generate MDC clock frequency: csr_clk_i/8 */
#define XGMAC_CSR_CLK_DIVIDER_VALUE_10 (3U)         /**<  If CRS bit set, divider value 10 is used to generate MDC clock frequency: csr_clk_i/10 */
#define XGMAC_CSR_CLK_DIVIDER_VALUE_12 (4U)         /**<  If CRS bit set, divider value 12 is used to generate MDC clock frequency: csr_clk_i/12 */
#define XGMAC_CSR_CLK_DIVIDER_VALUE_14 (5U)         /**<  If CRS bit set, divider value 14 is used to generate MDC clock frequency: csr_clk_i/14 */
#define XGMAC_CSR_CLK_DIVIDER_VALUE_16 (6U)         /**<  If CRS bit set, divider value 16 is used to generate MDC clock frequency: csr_clk_i/16 */
#define XGMAC_CSR_CLK_DIVIDER_VALUE_18 (7U)         /**<  If CRS bit set, divider value 18 is used to generate MDC clock frequency: csr_clk_i/18 */
/** @} */

/**
    @name MDIO_AccClauseType
    @{
    @brief MDIO register access clause

    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION
 */
typedef uint8_t XGMAC_MdioAccClauseType;
#define XGMAC_MDIO_ACC_CLAUSE_CL22       (0x1U)    /**< @brief MDIO CL22 register access */
#define XGMAC_MDIO_ACC_CLAUSE_CL45       (0x2U)    /**< @brief MDIO CL45 register access */
/** @} */

/**
    @name XGMAC_MdioBusCfgType
    @{
    @brief MDIO Bus configuration type

    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT
 */
typedef struct sXGMAC_MdioBusCfgType {
    XGMAC_MdioClockRangeSelType  clkRangeSel;    /**< @brief If CRS bit is selected, it overides MDIO clock generation to enable faster clock. */
    XGMAC_CsrClkDividerValueType csrClkDiv;      /**< @brief CSR clock divider needs to be provided when CRS is selected to overide MDC clock generation. */
    uint32_t                     clkCsrInput;    /**< @brief Clock input in MHz to stations management agent (SMA). When CRS bit is not set, divider is based on application clock range. */
    uint8_t                      preambleSuppressEn; /**< @brief If set, it suppresses 32-bit preamble (all 1's) in transmit packet. */
} XGMAC_MdioBusCfgType;
/** @} */

/**
    @brief Initialize MDIO

    This API initializes MDIO bus.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the XGMAC
    @param[in]      aConfig     Pointer to bus configuration structure

    Return values are documented in reverse-chronological order
    @retval         #BCM_ERR_OK             success
    @retval         #BCM_ERR_INVAL_PARAMS   aConfig in NULL
    @retval         #BCM_ERR_INVAL_PARAMS   Some configuration in aConfig is not correct
    @retval         #BCM_ERR_INVAL_STATE    Bus already initialized.
    @retval         #BCM_ERR_UNKNOWN        integration error

    @post None

    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT

    @limitations None
*/

int XGMAC_MdioInit(XGMAC_HwIDType aId, XGMAC_MdioBusCfgType *aConfig);

/** @brief De-Initialize MDIO bus

    API to de-initialize and reset the MDIO bus in default state.

    @behavior Sync, Re-entrant

    @pre TBD
    @param[in]      aID         Index of the XGMAC

    Return values are documented in reverse-chronological order.
    @retval     #BCM_ERR_OK   MDIO driver de-initialization is successful.
    @retval     #BCM_ERR_UNINIT Device or resource is not un-initialized.
    @retval     #BCM_ERR_INVAL_PARAMS Invalid param.

    @post None

    @trace #BRCM_SWREQ_XGMAC_MDIO_INIT

    @limitations None
*/
int XGMAC_MdioDeInit(XGMAC_HwIDType aId);

/** @brief MDIO write

    API to do MDIO write operation.

    @behavior Sync, Re-entrant

    @pre            None

    @param[in]      aID         Index of the XGMAC
    @param[in]      phyAddr     5-bit PHY address
    @param[in]      clauseType  Register access clause type clause 22 or clause 45.
    @param[in]      phyRegAddr  PHY register address to be accessed CL22 (lower 5-bits)/CL45 (16-bits), Bit position (16-20)
                                is used for device type (5-bit device address) for clause 45.
    @param[in]      aData       16-bit data to be written to slave device

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   aRegAddr > 31 (CL22 mode)
    @retval     #BCM_ERR_INVAL_PARAMS   aDevType > 31
    @retval     #BCM_ERR_NOSUPPORT      Operation unsupported as controller is
                                        in slave mode of operation.
    @retval     #BCM_ERR_UNKNOWN        integration error

    @post None

    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION

    @limitations None
*/
int XGMAC_MdioWrite(XGMAC_HwIDType aId, uint32_t phyAddr, XGMAC_MdioAccClauseType clauseType, uint32_t phyRegAddr, uint16_t aData);

/** @brief MDIO read

    API to do MDIO read operation.

    @behavior Sync, Re-entrant

    @pre            None

    @param[in]      aID         Index of the XGMAC
    @param[in]      phyAddr     5-bit PHY address
    @param[in]      clauseType  Register access clause type clause 22 or clause 45.
    @param[in]      phyRegAddr  PHY register address to be accessed CL22 (lower 5-bits)/CL45 (0-15)(16-bits), Bit position (16-20)
                                is used for device type (5-bit device address) for clause 45.
    @param[out]     aData       Pointer to 16-bit data

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   aRegAddr > 31 (CL22 mode)
    @retval     #BCM_ERR_INVAL_PARAMS   aDevType > 31
    @retval     #BCM_ERR_NOSUPPORT      Operation unsupported as controller is
                                        in slave mode of operation.
    @retval     #BCM_ERR_UNKNOWN        integration error

    @post None

    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION

    @limitations None
*/
int XGMAC_MdioRead(XGMAC_HwIDType aId, uint32_t phyAddr, XGMAC_MdioAccClauseType clauseType, uint32_t phyRegAddr, uint16_t *const aData);

/** @brief MDIO reset

    API to do reset MII bus.

    @behavior Sync, Re-entrant

    @pre            None

    @param[in]      aID         Index of the XGMAC

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success

    @retval     #BCM_ERR_UNKNOWN        integration error

    @post None

    @trace #BRCM_SWREQ_XGMAC_MDIO_OPERATION

    @limitations None
*/
int XGMAC_MdioReset(XGMAC_HwIDType aId);
#endif /* XGMAC_MDIO_H */
/** @} */

