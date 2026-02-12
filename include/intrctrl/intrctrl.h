/*****************************************************************************
 Copyright 2024 Broadcom.  All rights reserved.

 Broadcom Proprietary and Confidential

 ******************************************************************************/

/**
    @defgroup grp_intrctrldrv_ifc Interrupt Controller
    @ingroup grp_intrctrldrv

    @addtogroup grp_intrctrldrv_ifc
    @{
    @section sec_intrctrl_overview Overview
    @note
    -# Interrupt controller driver deals with level 1 interrupts servicing. This driver allows user to
       get the status of shared interrupts status.

    @note Please refer to respective technical reference manual for more details about the HW features.

    -# Interrupt Controller instance is accessed based on HwID passed as first parameter in each API.

       | HwID        | HW Instance   |
       |:-----------:|:-------------:|
       | 0           |   INTRCTRL |

    @section api_list List of Interrupt controller APIs
    | API Name | Brief Decription   |
    |:-----------:|:-------------:|
    | @ref INTRCTRL_McuIrq6Status          | Get MCU IRQ6 pending status           |
    | @ref INTRCTRL_McuIrq7Status          | Get MCU IRQ7 pending status           |
    | @ref INTRCTRL_Q8Irq7Status           | Get Q8 IRQ7 pending status            |
    | @ref INTRCTRL_GicSpiIrq6Status       | Get GICSPI IRQ6 pending status        |
    | @ref INTRCTRL_DrvSmIntrEnable        | Safety monitor interrupt configuration|
    | @ref INTRCTRL_DrvNmiIntrEnable       | NMI interrupt enable                  |
    | @ref INTRCTRL_DrvNmiIntrStatus       | NMI interrupt status                  |
    | @ref INTRCTRL_DrvEnableGpioInterrupt | Enable GPIO interrupt                 |
    | @ref INTRCTRL_DrvGpioInterruptStatus | Get GPIO interrupt status             |
    | @ref INTRCTRL_DrvGpioEventStatus     | Get GPIO event status                 |
    | @ref INTRCTRL_DrvClearGpioInterrupt  | Get GPIO clear interrupts             |

    @file intrctrl.h
    @brief Interface for interrupt controller driver
    @section apis This section defines the interface for interrupt controller driver

    @version 0.1 Initial Version
*/

#ifndef INTRCTRL_H
#define INTRCTRL_H

#include <stdint.h>

/**
    @name Interrupt Controller driver API IDs
    @{
    @brief API IDs for Interrupt controller driver
 */
#define BRCM_SWARCH_INTRCTRL_HW_ID_TYPE                        (0x8901U)    /**< @brief #INTRCTRL_HwIDType               */
#define BRCM_SWARCH_INTRCTRL_MCU_IRQ6_STS_TYPE                 (0x8902U)    /**< @brief #INTRCTRL_McuIrq6StsType         */
#define BRCM_SWARCH_INTRCTRL_MCU_IRQ7_STS_TYPE                 (0x8903U)    /**< @brief #INTRCTRL_McuIrq7StsType         */
#define BRCM_SWARCH_INTRCTRL_Q8_IRQ7_STS_TYPE                  (0x8904U)    /**< @brief #INTRCTRL_Q8Irq7StsType          */
#define BRCM_SWARCH_INTRCTRL_GICSPI_IRQ6_STS_TYPE              (0x8905U)    /**< @brief #INTRCTRL_GicSPIIrq6StsType      */

#define BRCM_SWARCH_INTRCTRL_MCU_IRQ6_STATUS_PROC              (0x8906U)    /**< @brief #INTRCTRL_McuIrq6Status          */
#define BRCM_SWARCH_INTRCTRL_MCU_IRQ7_STATUS_PROC              (0x8907U)    /**< @brief #INTRCTRL_McuIrq7Status          */
#define BRCM_SWARCH_INTRCTRL_Q8_IRQ7_STATUS_PROC               (0x8908U)    /**< @brief #INTRCTRL_Q8Irq7Status           */
#define BRCM_SWARCH_INTRCTRL_GICSPI_IRQ6_STATUS_PROC           (0x8909U)    /**< @brief #INTRCTRL_GicSpiIrq6Status       */

#define BRCM_SWARCH_INTRCTRL_DRV_SM_INTR_ENABLE_PROC           (0x890AU)    /**< @brief #INTRCTRL_DrvSmIntrEnable        */
#define BRCM_SWARCH_INTRCTRL_NMI_INTR_TYPE                     (0x890BU)    /**< @brief #INTRCTRL_NmiIntrType            */
#define BRCM_SWARCH_INTRCTRL_NMI_INTR0_TYPE                    (0x890CU)    /**< @brief #INTRCTRL_NmiIntrType0           */
#define BRCM_SWARCH_INTRCTRL_NMI_INTR1_TYPE                    (0x890DU)    /**< @brief #INTRCTRL_NmiIntrType1           */
#define BRCM_SWARCH_INTRCTRL_NMI_INTR2_TYPE                    (0x890EU)    /**< @brief #INTRCTRL_NmiIntrType2           */
#define BRCM_SWARCH_INTRCTRL_SM_BLK_INTR_TYPE                  (0x890FU)    /**< @brief #INTRCTRL_SmBlkIntrType          */
#define BRCM_SWARCH_INTRCTRL_SM_INTR_BLK_TYPE                  (0x8910U)    /**< @brief #INTRCTRL_SmIntrBlkType          */
#define BRCM_SWARCH_INTRCTRL_DRV_NMI_INTR_ENABLE_PROC          (0x8911U)    /**< @brief #INTRCTRL_DrvNmiIntrEnable       */
#define BRCM_SWARCH_INTRCTRL_DRV_NMI_INTR_STS_PROC             (0x8912U)    /**< @brief #INTRCTRL_DrvNmiIntrStatus       */

#define BRCM_SWARCH_INTRCTRL_GPIO_INTR_TYPE                    (0x8913U)    /**< @brief #INTRCTRL_GpioIntrType           */
#define BRCM_SWARCH_INTRCTRL_DRV_ENABLE_GPIO_INTERRUPT_PROC    (0x8914U)    /**< @brief #INTRCTRL_DrvEnableGpioInterrupt */
#define BRCM_SWARCH_INTRCTRL_DRV_GPIO_INTERRUPT_STATUS_PROC    (0x8915U)    /**< @brief #INTRCTRL_DrvGpioInterruptStatus */
#define BRCM_SWARCH_INTRCTRL_DRV_GPIO_EVENT_STATUS_PROC        (0x8916U)    /**< @brief #INTRCTRL_DrvGpioEventStatus     */
#define BRCM_SWARCH_INTRCTRL_DRV_CLEAR_GPIO_INTERRUPT_PROC     (0x8917U)    /**< @brief #INTRCTRL_DrvClearGpioInterrupt  */

/** @} */

/**
    @brief Index of the Interrupt controller

    @trace #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
 */
typedef uint32_t INTRCTRL_HwIDType;

/**
    @name INTRCTRL_McuIrq6StsType
    @{
    @brief Shared(IRQ line number 6) Interrupt status for MCU0/MCU1

    @trace #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
 */
typedef uint32_t INTRCTRL_McuIrq6StsType;                                       /**< @brief  MCU0/MCU1 IRQ6 interrupt pending Status*/
#define INTRCTRL_MCU_IRQ6_STS_MASK                                 (0x3FFFUL)   /**< @brief  IRQ6 Pending Mask*/
#define INTRCTRL_MCU_IRQ6_STS_MCU1_MEM_INIT_INTR                   (0x2000UL)   /**< @brief  MCU1 memory initialization Interrupt Pending*/
#define INTRCTRL_MCU_IRQ6_STS_Q8_0_MEM_INIT_SEC_INTR               (0x1000UL)   /**< @brief  Q8 instance 0 Memory initialization Sec Interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_Q8_1_MEM_INIT_SEC_INTR               (0x800UL)    /**< @brief  Q8 instance 1 Memory initialization Sec Interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_Q8_2_MEM_INIT_SEC_INTR               (0x400UL)    /**< @brief  Q8 instance 2 Memory initialization Sec Interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_Q8_3_MEM_INIT_SEC_INTR               (0x200UL)    /**< @brief  Q8 instance 3 Memory initialization Sec Interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_LSP_SYS_MEM_INIT_INTR                (0x100UL)    /**< @brief  Low speed peripheral system memory initialization interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_ACQ_HSREF_MEM_INIT_SEC_INTR          (0x80UL)     /**< @brief  Acquisation HSREF ADC memory initialization SEC interrupt pending */
#define INTRCTRL_MCU_IRQ6_STS_ADC0_ACQ_HSADC_MEM_INIT_SEC_INTR     (0x40UL)     /**< @brief  ADC0 Acquisation HSADC memory initialization SEC interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_ADC1_ACQ_HSADC_MEM_INIT_SEC_INTR     (0x20UL)     /**< @brief  ADC1 Acquisation HSADC memory initialization SEC interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_ADC2_ACQ_HSADC_MEM_INIT_SEC_INTR     (0x10UL)     /**< @brief  ADC2 Acquisation HSADC memory initialization SEC interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_ADC3_ACQ_HSADC_MEM_INIT_SEC_INTR     (0x8UL)      /**< @brief  ADC3 Acquisation HSADC memory initialization SEC interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_ACQ_AWG_MEM_INIT_SEC_INTR            (0x4UL)      /**< @brief  Acquisation AWG memory initialization SEC interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_DDR_MEM_INIT_INTR                    (0x2UL)      /**< @brief  DDR memory initialization interrupt pending*/
#define INTRCTRL_MCU_IRQ6_STS_HSIO_MIPI_MEM_INIT_SEC_INTR          (0x1UL)      /**< @brief  High Speed IO MIPI memory initialization SEC interrupt pending*/
/** @} */

/**
    @name INTRCTRL_McuIrq7StsType
    @{
    @brief Shared(line number 7) Interrupt status for MCU0/MCU1

    @trace #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
 */
typedef uint32_t INTRCTRL_McuIrq7StsType;                                   /**< @brief  MCU0/MCU1 IRQ7 interrupt pending Status*/
#define INTRCTRL_MCU_IRQ7_STS_MASK                                 (0x3UL)   /**< @brief  IRQ7 Pending Mask*/
#define INTRCTRL_MCU_IRQ7_STS_GP_SRAM_MEM_SEC_INTR                 (0x2UL)   /**< @brief  GP SRAM Memory SEC interrpt pending*/
#define INTRCTRL_MCU_IRQ7_STS_GP_SRAM_MEM_INIT_DONE_INTR           (0x1UL)   /**< @brief  GP SRAM Memory initialization done interrpt pending*/
/** @} */

/**
    @name INTRCTRL_Q8Irq7Sts
    @{
    @brief Shared(line number 7) Interrupt status for Q8

    @trace #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
 */
typedef uint32_t INTRCTRL_Q8Irq7StsType;                                /**< @brief Q8 IRQ7 interrupt pending Status */
#define INTRCTRL_Q8_IRQ7_STS_MASK                            (0xF00UL)   /**< @brief  IRQ7 Pending Mask*/
#define INTRCTRL_Q8_IRQ7_STS_ACQ_NSU_SYSTICK3_INTR           (0x800UL)   /**< @brief  Acquisation NSU system ticks 3 interrupt pending*/
#define INTRCTRL_Q8_IRQ7_STS_ACQ_NSU_SYSTICK2_INTR           (0x400UL)   /**< @brief  Acquisation NSU system ticks 2 interrupt pending*/
#define INTRCTRL_Q8_IRQ7_STS_ACQ_NSU_SYSTICK1_INTR           (0x200UL)   /**< @brief  Acquisation NSU system ticks 1 interrupt pending*/
#define INTRCTRL_Q8_IRQ7_STS_ACQ_NSU_SYSTICK0_INTR           (0x100UL)   /**< @brief  Acquisation NSU system ticks 0 interrupt pending*/
/** @} */


/**
    @name INTRCTRL_GicSPIIrq6StsType
    @{
    @brief Shared(line number 6) Interrupt status for GIC SPI

    @trace #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS
 */
typedef uint32_t INTRCTRL_GicSPIIrq6StsType;                                       /**< @brief  GIC SPI interrupt pending Status*/
#define INTRCTRL_GICSPI_IRQ6_STS_MASK                                 (0x3FFFUL)   /**< @brief  GIC SPI mask*/
#define INTRCTRL_GICSPI_IRQ6_STS_MCU1_MEM_INIT_INTR                   (0x2000UL)   /**< @brief  MCU1 memory initialization Interrupt Pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_Q8_0_MEM_INIT_SEC_INTR               (0x1000UL)   /**< @brief  Q8 instance 0 Memory initialization Sec Interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_Q8_1_MEM_INIT_SEC_INTR               (0x800UL)    /**< @brief  Q8 instance 1 Memory initialization Sec Interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_Q8_2_MEM_INIT_SEC_INTR               (0x400UL)    /**< @brief  Q8 instance 2 Memory initialization Sec Interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_Q8_3_MEM_INIT_SEC_INTR               (0x200UL)    /**< @brief  Q8 instance 3 Memory initialization Sec Interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_LSP_SYS_MEM_INIT_INTR                (0x100UL)    /**< @brief  Low speed peripheral system memory initialization interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_ACQ_HSREF_MEM_INIT_SEC_INTR          (0x80UL)     /**< @brief  Acquisation HSREF ADC memory initialization SEC interrupt pending */
#define INTRCTRL_GICSPI_IRQ6_STS_ADC0_ACQ_HSADC_MEM_INIT_SEC_INTR     (0x40UL)     /**< @brief  ADC0 Acquisation HSADC memory initialization SEC interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_ADC1_ACQ_HSADC_MEM_INIT_SEC_INTR     (0x20UL)     /**< @brief  ADC1 Acquisation HSADC memory initialization SEC interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_ADC2_ACQ_HSADC_MEM_INIT_SEC_INTR     (0x10UL)     /**< @brief  ADC2 Acquisation HSADC memory initialization SEC interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_ADC3_ACQ_HSADC_MEM_INIT_SEC_INTR     (0x8UL)      /**< @brief  ADC3 Acquisation HSADC memory initialization SEC interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_ACQ_AWG_MEM_INIT_SEC_INTR            (0x4UL)      /**< @brief  Acquisation AWG memory initialization SEC interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_DDR_MEM_INIT_INTR                    (0x2UL)      /**< @brief  DDR memory initialization interrupt pending*/
#define INTRCTRL_GICSPI_IRQ6_STS_HSIO_MIPI_MEM_INIT_SEC_INTR          (0x1UL)      /**< @brief  High Speed IO MIPI memory initialization SEC interrupt pending*/
/** @} */

/**
    @name INTRCTRL_SmIntrBlkType
    @{
    @brief Interrupt block for safety monitor

    @trace #BRCM_SWREQ_INTRCTRL_SM_INTR
 */
typedef uint16_t INTRCTRL_SmIntrBlkType;                      /**< @brief Safety monitor interrupt block type */
#define INTRCTRL_SM_INTR_BLK_MCU0               (0x1U)        /**< @brief  Safety monitor interrupt for MCU0        */
#define INTRCTRL_SM_INTR_BLK_NO_CONNECT         (0x2U)        /**< @brief  Safety monitor interrupt for no_connect  */
#define INTRCTRL_SM_INTR_BLK_MCU1_LOCKUP        (0x4U)        /**< @brief  Safety monitor interrupt for MCU1 lockup */
#define INTRCTRL_SM_INTR_BLK_AHB_MON            (0x8U)        /**< @brief  Safety monitor interrupt for AHB monitor */
#define INTRCTRL_SM_INTR_BLK_FABRIC             (0x10U)       /**< @brief  Safety monitor interrupt for fabric      */
#define INTRCTRL_SM_INTR_BLK_BRWNOUT            (0x20U)       /**< @brief  Safety monitor interrupt for brwnout     */
#define INTRCTRL_SM_INTR_BLK_TEMP_ALRM          (0x40U)       /**< @brief  Safety monitor interrupt for temperature alarm   */
#define INTRCTRL_SM_INTR_BLK_DED_FAULT          (0x80U)       /**< @brief  Safety monitor interrupt for DED fault   */
#define INTRCTRL_SM_INTR_BLK_GPSRAM_FAULT       (0x100U)      /**< @brief  Safety monitor interrupt for GPSRAM fault*/
#define INTRCTRL_SM_INTR_BLK_WWDT               (0x200U)      /**< @brief  Safety monitor interrupt for WWDT        */
#define INTRCTRL_SM_INTR_BLK_QSPI               (0x400U)      /**< @brief  Safety monitor interrupt for QSPI        */
#define INTRCTRL_SM_INTR_BLK_ACQ_HSADC          (0x800U)      /**< @brief  Safety monitor interrupt for acquisation HSADC   */
#define INTRCTRL_SM_INTR_BLK_PMIC               (0x1000U)     /**< @brief  Safety monitor interrupt for PMIC        */
/** @} */

/**
    @name INTRCTRL_SmBlkIntrType
    @{
    @brief Interrupt Type based on safety monitor interrupt block

    @trace #BRCM_SWREQ_INTRCTRL_SM_INTR
 */
typedef uint32_t INTRCTRL_SmBlkIntrType;                                        /**< @brief  Safety monitor interrupt type based on SM intrrupt block*/
#define INTRCTRL_SM_BLK_INTR_MCU0_NOC_FAULT                       (0x40UL)      /**< @brief  Safety monitor interrupt for MCU0 NOC fault*/
#define INTRCTRL_SM_BLK_INTR_MCU0_LOCKUP_FAULT                    (0x20UL)      /**< @brief  Safety monitor interrupt for MCU0 lockup fault*/
#define INTRCTRL_SM_BLK_INTR_MCU0_DCCM_FAULT                      (0x10UL)      /**< @brief  Safety monitor interrupt for MCU0 DCCM fault*/
#define INTRCTRL_SM_BLK_INTR_MCU0_GP_SRAM_MEM_DED_FAULT           (0x8UL)       /**< @brief  Safety monitor interrupt for MCU0 GP SRAM memory DED fault*/
#define INTRCTRL_SM_BLK_INTR_MCU0_HSM_LITE_DED_FAULT              (0x4UL)       /**< @brief  Safety monitor interrupt for MCU0 HSM lite DED fault*/
#define INTRCTRL_SM_BLK_INTR_MCU0_LSP_BSPI_FAULT                  (0x2UL)       /**< @brief  Safety monitor interrupt for MCU0 BSPI Fault*/
#define INTRCTRL_SM_BLK_INTR_MCU0_DED_FAULT                       (0x1UL)       /**< @brief  Safety monitor interrupt for MCU0 DED fault*/

#define INTRCTRL_SM_BLK_INTR_NO_CONNECT_LSP_SYSPLL_LOCK           (0x10UL)       /**< @brief  Safety monitor interrupt for no_connect system PLL lock*/
#define INTRCTRL_SM_BLK_INTR_NO_CONNECT_HSM_LITE_MEM_INIT_DONE    (0x8UL)        /**< @brief  Safety monitor interrupt for no_connect HSM lite memory init done */
#define INTRCTRL_SM_BLK_INTR_NO_CONNECT_BSPI_MEM_INIT_DONE        (0x4UL)        /**< @brief  Safety monitor interrupt for no_connect BSPI memory init done  */
#define INTRCTRL_SM_BLK_INTR_NO_CONNECT_D1TCM_INIT_DONE           (0x2UL)        /**< @brief  Safety monitor interrupt for no_connect  D1 TCM init done*/
#define INTRCTRL_SM_BLK_INTR_NO_CONNECT_D0TCM_INIT_DONE           (0x1UL)        /**< @brief  Safety monitor interrupt for no_connect  D0 TCM init done*/

#define INTRCTRL_SM_BLK_INTR_MCU1_Q8_FF_0                         (0x10UL)      /**< @brief  Safety monitor interrupt for MCU1 Q8 0 Fatal fault*/
#define INTRCTRL_SM_BLK_INTR_MCU1_Q8_FF_1                         (0x8UL)       /**< @brief  Safety monitor interrupt for MCU1 Q8 1 Fatal fault*/
#define INTRCTRL_SM_BLK_INTR_MCU1_Q8_FF_2                         (0x4UL)       /**< @brief  Safety monitor interrupt for MCU1 Q8 2 Fatal fault*/
#define INTRCTRL_SM_BLK_INTR_MCU1_Q8_FF_3                         (0x2UL)       /**< @brief  Safety monitor interrupt for MCU1 Q8 4 Fatal fault*/
#define INTRCTRL_SM_BLK_INTR_MCU1_LOCKUP_FAULT                    (0x1UL)       /**< @brief  Safety monitor interrupt for MCU1 lock up fault*/

#define INTRCTRL_SM_BLK_INTR_AHB_MON_BOOT_FLT_SPI_TIMOUT          (0x8UL)        /**< @brief  Safety monitor interrupt for AHB monitor boot fault SPI Timeout*/
#define INTRCTRL_SM_BLK_INTR_AHB_MON_BOOT_FLT_SLV0_TIMOUT         (0x4UL)        /**< @brief  Safety monitor interrupt for AHB monitor boot fault slave 0 Timeout*/
#define INTRCTRL_SM_BLK_INTR_AHB_MON_BOOT_FLT_SLV1_TIMOUT         (0x2UL)        /**< @brief  Safety monitor interrupt for AHB monitor boot fault slave 1 Timeout*/
#define INTRCTRL_SM_BLK_INTR_AHB_MON_BOOT_FLT_SLV2_TIMOUT         (0x1UL)        /**< @brief  Safety monitor interrupt for AHB monitor boot fault slave 2 Timeout*/

#define INTRCTRL_SM_BLK_INTR_FABRIC_NOC_SVCNW_NOC_FAULT           (0x800000UL)   /**< @brief  Safety monitor interrupt for fabric NOC SVCNW NOC fault*/
#define INTRCTRL_SM_BLK_INTR_FABRIC_GLB_AHB_NOC_FAULT             (0x400000UL)   /**< @brief  Safety monitor interrupt for fabric global AHB NOC fault*/
#define INTRCTRL_SM_BLK_INTR_FABRIC_HSIO_PCIE_NOC_FAULT           (0x200000UL)   /**< @brief  Safety monitor interrupt for fabric HSIO PCIE NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_HSIO_MIPI_NOC_FAULT           (0x100000UL)   /**< @brief  Safety monitor interrupt for fabric HSIO MIPI NOC fault*/
#define INTRCTRL_SM_BLK_INTR_FABRIC_HSIO_ETH_NOC_FAULT            (0x80000UL)    /**< @brief  Safety monitor interrupt for fabric HSIO ETH NOC fault*/
#define INTRCTRL_SM_BLK_INTR_FABRIC_SDC_LLC_NOC_FAULT             (0x40000UL)    /**< @brief  Safety monitor interrupt for fabric SDC LLC NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_ACQ_NOC_PARITY_FAULT          (0x20000UL)    /**< @brief  Safety monitor interrupt for fabric ACQ NOC parity fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_LSP_SYS_NOC_FAULT             (0x10000UL)    /**< @brief  Safety monitor interrupt for fabric LSP System NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_APPROC_NOC_FAULT              (0x8000UL)     /**< @brief  Safety monitor interrupt for fabric Approc NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_Q8_3_NOC_FAULT                (0x4000UL)     /**< @brief  Safety monitor interrupt for fabric Q8 3 NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_Q8_2_NOC_FAULT                (0x2000UL)     /**< @brief  Safety monitor interrupt for fabric Q8 2 NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_Q8_1_NOC_FAULT                (0x1000UL)     /**< @brief  Safety monitor interrupt for fabric Q8 1 NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_Q8_0_NOC_FAULT                (0x800UL)      /**< @brief  Safety monitor interrupt for fabric Q8 0 NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_GP_DMA_NOC_FAULT              (0x400UL)      /**< @brief  Safety monitor interrupt for fabric GP DMA NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_GP_SRAM_NOC_FAULT             (0x200UL)      /**< @brief  Safety monitor interrupt for fabric GP SRAM NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_MCU1_NOC_FAULT                (0x100UL)      /**< @brief  Safety monitor interrupt for fabric MCU1 NOC fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_MINOR_FAULT                   (0x80UL)       /**< @brief  Safety monitor interrupt for fabric minor fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_MAJOR_FAULT                   (0x40UL)       /**< @brief  Safety monitor interrupt for fabric major fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_CRITICAL_FAULT                (0x20UL)       /**< @brief  Safety monitor interrupt for fabric critical fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_BISTFAIL_FAULT                (0x10UL)       /**< @brief  Safety monitor interrupt for fabric BIST fail fault */
#define INTRCTRL_SM_BLK_INTR_FABRIC_MINOR_FAULT1                  (0x8UL)        /**< @brief  Safety monitor interrupt for fabric  minor fault 1*/
#define INTRCTRL_SM_BLK_INTR_FABRIC_MAJOR_FAULT1                  (0x4UL)        /**< @brief  Safety monitor interrupt for fabric  major fault 1*/
#define INTRCTRL_SM_BLK_INTR_FABRIC_CRITICAL_FAULT1               (0x2UL)        /**< @brief  Safety monitor interrupt for fabric  critical fault 1*/
#define INTRCTRL_SM_BLK_INTR_FABRIC_BISTFAIL_FAULT1               (0x1UL)        /**< @brief  Safety monitor interrupt for fabric  BIST fail fault 1*/

#define INTRCTRL_SM_BLK_INTR_BRWNOUT_PWR_FAULT                    (0x1UL)        /**< @brief  Safety monitor interrupt for brwnout power fault        */

#define INTRCTRL_SM_BLK_INTR_PWR_TEMP_ALRM                        (0x1UL)        /**< @brief  Safety monitor interrupt for power temperature fault  */

#define INTRCTRL_SM_BLK_INTR_DED_FAULT_APPROCERRIRQ0_INV          (0x10000UL)    /**< @brief  Safety monitor interrupt for DED fault on APPROC nERRIRQ0*/
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_APPROCERRIRQ1_INV          (0x8000UL)     /**< @brief  Safety monitor interrupt for DED fault on APPROC nERRIRQ1*/
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_APPROCERRIRQ2_INV          (0x4000UL)     /**< @brief  Safety monitor interrupt for DED fault on APPROC nERRIRQ2*/
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_APPROCERRIRQ3_INV          (0x2000UL)     /**< @brief  Safety monitor interrupt for DED fault on APPROC nERRIRQ3*/
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_APPROCERRIRQ4_INV          (0x1000UL)     /**< @brief  Safety monitor interrupt for DED fault on APPROC nERRIRQ4*/
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_HSIO_MIPI                  (0x800UL)      /**< @brief  Safety monitor interrupt for DED fault on HSIO MIPI*/
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_ACQ_AWG                    (0x400UL)      /**< @brief  Safety monitor interrupt for DED fault on ACQ AWG*/
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_ADC3_ACQ_HSADC              (0x200UL)      /**< @brief  Safety monitor interrupt for DED fault on ADC3 ACQ HSADC       */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_ADC2_ACQ_HSADC              (0x100UL)      /**< @brief  Safety monitor interrupt for DED fault on ADC2 ACQ HSADC       */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_ADC1_ACQ_HSADC              (0x80UL)       /**< @brief  Safety monitor interrupt for DED fault on ADC1 ACQ HSADC       */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_ADC0_ACQ_HSADC              (0x40UL)       /**< @brief  Safety monitor interrupt for DED fault on ADC0 ACQ HSADC       */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_LSP_SYS                    (0x20UL)       /**< @brief  Safety monitor interrupt for DED fault on LSP SYS             */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_Q8_3                       (0x10UL)       /**< @brief  Safety monitor interrupt for DED fault on Q8 3                */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_Q8_2                       (0x8UL)        /**< @brief  Safety monitor interrupt for DED fault on Q8 2                */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_Q8_1                       (0x4UL)        /**< @brief  Safety monitor interrupt for DED fault on Q8 1                */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_Q8_0                       (0x2UL)        /**< @brief  Safety monitor interrupt for DED fault on Q8 0                */
#define INTRCTRL_SM_BLK_INTR_DED_FAULT_MCU1                       (0x1UL)        /**< @brief  Safety monitor interrupt for DED fault on MCU1                */

#define INTRCTRL_SM_BLK_INTR_GPSRAM_MEM_PROTECT_FAULT             (0x1UL)        /**< @brief  Safety monitor interrupt for GPSRAM memory protect fault*/

#define INTRCTRL_SM_BLK_INTR_WWDT_0                               (0x80UL)       /**< @brief  Safety monitor interrupt for WWDT 0        */
#define INTRCTRL_SM_BLK_INTR_WWDT_1                               (0x40UL)       /**< @brief  Safety monitor interrupt for WWDT 1       */
#define INTRCTRL_SM_BLK_INTR_WWDT_2                               (0x20UL)       /**< @brief  Safety monitor interrupt for WWDT 2       */
#define INTRCTRL_SM_BLK_INTR_WWDT_3                               (0x10UL)       /**< @brief  Safety monitor interrupt for WWDT 3       */
#define INTRCTRL_SM_BLK_INTR_WWDT_4                               (0x8UL)        /**< @brief  Safety monitor interrupt for WWDT 4       */
#define INTRCTRL_SM_BLK_INTR_WWDT_5                               (0x4UL)        /**< @brief  Safety monitor interrupt for WWDT 5       */
#define INTRCTRL_SM_BLK_INTR_WWDT_6                               (0x2UL)        /**< @brief  Safety monitor interrupt for WWDT 6       */
#define INTRCTRL_SM_BLK_INTR_WWDT_7                               (0x1UL)        /**< @brief  Safety monitor interrupt for WWDT 7       */

#define INTRCTRL_SM_BLK_INTR_QSPI_0_TX_UNDFLW                     (0x800UL)      /**< @brief  Safety monitor interrupt for QSPI0 Tx under flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_0_TX_OVRFLW                     (0x400UL)      /**< @brief  Safety monitor interrupt for QSPI0 Tx over flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_0_RX_UNDFLW                     (0x200UL)      /**< @brief  Safety monitor interrupt for QSPI0 Rx under flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_0_RX_OVRFLW                     (0x100UL)      /**< @brief  Safety monitor interrupt for QSPI0 Rx over flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_1_TX_UNDFLW                     (0x80UL)       /**< @brief  Safety monitor interrupt for QSPI1 Tx under flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_1_TX_OVRFLW                     (0x40UL)       /**< @brief  Safety monitor interrupt for QSPI1 Tx over flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_1_RX_UNDFLW                     (0x20UL)       /**< @brief  Safety monitor interrupt for QSPI1 Rx under flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_1_RX_OVRFLW                     (0x10UL)       /**< @brief  Safety monitor interrupt for QSPI1 Rx over flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_2_TX_UNDFLW                     (0x8UL)        /**< @brief  Safety monitor interrupt for QSPI2 Tx under flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_2_TX_OVRFLW                     (0x4UL)        /**< @brief  Safety monitor interrupt for QSPI2 Tx over flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_2_RX_UNDFLW                     (0x2UL)        /**< @brief  Safety monitor interrupt for QSPI2 Rx under flow        */
#define INTRCTRL_SM_BLK_INTR_QSPI_2_RX_OVRFLW                     (0x1UL)        /**< @brief  Safety monitor interrupt for QSPI2 Rx over flow        */

#define INTRCTRL_SM_BLK_INTR_ACQ_HSADC_0_FUNC_ERR                 (0x8UL)        /**< @brief  Safety monitor interrupt for acquisation HSADC0   */
#define INTRCTRL_SM_BLK_INTR_ACQ_HSADC_1_FUNC_ERR                 (0x4UL)        /**< @brief  Safety monitor interrupt for acquisation HSADC1   */
#define INTRCTRL_SM_BLK_INTR_ACQ_HSADC_2_FUNC_ERR                 (0x2UL)        /**< @brief  Safety monitor interrupt for acquisation HSADC2   */
#define INTRCTRL_SM_BLK_INTR_ACQ_HSADC_3_FUNC_ERR                 (0x1UL)        /**< @brief  Safety monitor interrupt for acquisation HSADC3   */

#define INTRCTRL_SM_BLK_INTR_PMIC_GPIO_SW_0                       (0x1UL)        /**< @brief  Safety monitor interrupt for PMIC GPIO SW 0       */
/** @} */

/**
    @name INTRCTRL_NmiIntrType0
    @{
    @brief Interrupt Type for NMI interrupt

    @trace #BRCM_SWREQ_INTRCTRL_NMI_INTR
 */
typedef uint32_t INTRCTRL_NmiIntrType0;                                         /**< @brief NMI interrupt type */
#define INTRCTRL_NMI_INTR0_GLB_AHB_NOC_FAULT                (0x80000000UL)  /**< @brief NMI interrupt for Global AHB NOC Fault */
#define INTRCTRL_NMI_INTR0_HSIO_PCIE_NOC_FAULT            (0x40000000UL)  /**< @brief NMI interrupt for PCIE NOC Fault  */
#define INTRCTRL_NMI_INTR0_HSIO_MIPI_NOC_FAULT            (0x20000000UL)  /**< @brief NMI interrupt for MIPI NOC Fault  */
#define INTRCTRL_NMI_INTR0_HSIO_ETH_NOC_FAULT             (0x10000000UL)  /**< @brief NMI interrupt for ETH  NOC Fault   */
#define INTRCTRL_NMI_INTR0_SDC_LLC_NOC_FAULT              (0x8000000UL)   /**< @brief NMI interrupt for SDC LLC NOC Fault    */
#define INTRCTRL_NMI_INTR0_ACQ_NOC_PARITY_FAULT           (0x4000000UL)   /**< @brief NMI interrupt for Acquisation NOC Parity Fault */
#define INTRCTRL_NMI_INTR0_LSP_SYS_NOC_FAULT              (0x2000000UL)   /**< @brief NMI interrupt for System Noc Fault    */
#define INTRCTRL_NMI_INTR0_APPROC_NOC_FAULT               (0x1000000UL)   /**< @brief NMI interrupt for Approc Noc Fault     */
#define INTRCTRL_NMI_INTR0_Q8_3_NOC_FAULT                 (0x800000UL)    /**< @brief NMI interrupt for Q8 instance 3 Noc Fault       */
#define INTRCTRL_NMI_INTR0_Q8_2_NOC_FAULT                 (0x400000UL)    /**< @brief NMI interrupt for Q8 instance 2 Noc Fault       */
#define INTRCTRL_NMI_INTR0_Q8_1_NOC_FAULT                 (0x200000UL)    /**< @brief NMI interrupt for Q8 instance 1 Noc Fault       */
#define INTRCTRL_NMI_INTR0_Q8_0_NOC_FAULT                 (0x100000UL)    /**< @brief NMI interrupt for Q8 instance 0 Noc Fault       */
#define INTRCTRL_NMI_INTR0_GP_DMA_NOC_FAULT               (0x80000UL)     /**< @brief NMI interrupt for GP DMA Noc Fault     */
#define INTRCTRL_NMI_INTR0_GP_SRAM_NOC_FAULT                (0x40000UL)     /**< @brief NMI interrupt for GP SRAM Noc Fault       */
#define INTRCTRL_NMI_INTR0_MCU1_NOC_FAULT                 (0x20000UL)     /**< @brief NMI interrupt for MCU1 Noc Fault       */
#define INTRCTRL_NMI_INTR0_MINOR_FAULT                    (0x10000UL)     /**< @brief NMI interrupt for Minor Fault          */
#define INTRCTRL_NMI_INTR0_MAJOR_FAULT                    (0x8000UL)      /**< @brief NMI interrupt for Major Fault          */
#define INTRCTRL_NMI_INTR0_CRITICAL_FAULT                 (0x4000UL)      /**< @brief NMI interrupt for Critical Fault       */
#define INTRCTRL_NMI_INTR0_BISTFAIL                       (0x2000UL)      /**< @brief NMI interrupt for BIST fail             */
#define INTRCTRL_NMI_INTR0_MINOR_FAULT1                   (0x1000UL)      /**< @brief NMI interrupt for Minor Fault1         */
#define INTRCTRL_NMI_INTR0_MAJOR_FAULT1                   (0x800UL)       /**< @brief NMI interrupt for Major Fault1         */
#define INTRCTRL_NMI_INTR0_CRITICAL_FAULT1                (0x400UL)       /**< @brief NMI interrupt for Critical Fault1      */
#define INTRCTRL_NMI_INTR0_BISTFAIL1                      (0x200UL)       /**< @brief NMI interrupt for BIST fail1            */
#define INTRCTRL_NMI_INTR0_BOOT_SPI_APB_TIMEOUT           (0x100UL)       /**< @brief NMI interrupt for Boot Spi Apb Timeout */
#define INTRCTRL_NMI_INTR0_AHB_MON_SLV0_TIMOUT            (0x80UL)        /**< @brief NMI interrupt for AHB Monitor Slv0 Timout  */
#define INTRCTRL_NMI_INTR0_AHB_MON_SLV1_TIMOUT            (0x40UL)        /**< @brief NMI interrupt for AHB Monitor Slv1 Timout  */
#define INTRCTRL_NMI_INTR0_AHB_MON_SLV2_TIMOUT            (0x20UL)        /**< @brief NMI interrupt for AHB Monitor Slv2 Timout  */
#define INTRCTRL_NMI_INTR0_MCU1_LOCKUP_FAULT              (0x10UL)        /**< @brief NMI interrupt for MCU1 Lockup Fault    */
#define INTRCTRL_NMI_INTR0_MCU0_D1TCM_INIT_ERR            (0x8UL)         /**< @brief NMI interrupt for MCU0 D1TCM Init Err  */
#define INTRCTRL_NMI_INTR0_MCU0_D0TCM_INIT_ERR            (0x4UL)         /**< @brief NMI interrupt for MCU0 D0TCM Init Err  */
#define INTRCTRL_NMI_INTR0_MCU0_DED                       (0x2UL)         /**< @brief NMI interrupt for MCU0 DED             */
#define INTRCTRL_NMI_INTR0_MCU0_NOC_FAULT                    (0x1UL)         /**< @brief NMI interrupt for MCU0 NOC Fault       */

/** @} */
/**
    @name INTRCTRL_NmiIntrType1
    @{
    @brief Interrupt Type for NMI interrupt

    @trace #BRCM_SWREQ_INTRCTRL_NMI_INTR
 */
typedef uint32_t INTRCTRL_NmiIntrType1;                                         /**< @brief NMI interrupt type */
#define INTRCTRL_NMI_INTR1_Q8_0_PFATALERROR               (0x80000000UL)  /**< @brief NMI interrupt for Q8 instace 0 PFATAL error      */
#define INTRCTRL_NMI_INTR1_Q8_1_PFATALERROR               (0x40000000UL)  /**< @brief NMI interrupt for Q8 instace 1 PFATAL error      */
#define INTRCTRL_NMI_INTR1_Q8_2_PFATALERROR               (0x20000000UL)  /**< @brief NMI interrupt for Q8 instace 2 PFATAL error      */
#define INTRCTRL_NMI_INTR1_Q8_3_PFATALERROR               (0x10000000UL)  /**< @brief NMI interrupt for Q8 instace 3 PFATAL error      */
#define INTRCTRL_NMI_INTR1_GP_SRAM_MEM_INIT_ERROR         (0x8000000UL)   /**< @brief NMI interrupt for GP SRAM Memory Init error*/
#define INTRCTRL_NMI_INTR1_GP_SRAM_MEM_DED                (0x4000000UL)   /**< @brief NMI interrupt for GP SRAM Memory DED       */
#define INTRCTRL_NMI_INTR1_GP_SRAM_MEM_PROTECT            (0x2000000UL)   /**< @brief NMI interrupt for GP SRAM Memory protect   */
#define INTRCTRL_NMI_INTR1_HSM_LITE_MEM_INIT_ERR          (0x1000000UL)   /**< @brief NMI interrupt for HSM LITE Memory Init Error */
#define INTRCTRL_NMI_INTR1_LSP_BSPI_MEM_INIT_ERR          (0x800000UL)    /**< @brief NMI interrupt for BSPI Memory Init Error */
#define INTRCTRL_NMI_INTR1_HSIO_MIPI_DED                  (0x400000UL)    /**< @brief NMI interrupt for MIPI DED         */
#define INTRCTRL_NMI_INTR1_ACQ_AWG_DED                    (0x200000UL)    /**< @brief NMI interrupt for AWG DED           */
#define INTRCTRL_NMI_INTR1_ADC3_ACQ_HSADC_DED             (0x100000UL)    /**< @brief NMI interrupt for ADC3 Acquisation HSADC DED    */
#define INTRCTRL_NMI_INTR1_ADC2_ACQ_HSADC_DED             (0x80000UL)     /**< @brief NMI interrupt for ADC2 Acquisation HSADC DED    */
#define INTRCTRL_NMI_INTR1_ADC1_ACQ_HSADC_DED             (0x40000UL)     /**< @brief NMI interrupt for ADC1 Acquisation HSADC DED    */
#define INTRCTRL_NMI_INTR1_ADC0_ACQ_HSADC_DED             (0x20000UL)     /**< @brief NMI interrupt for ADC0 Acquisation HSADC DED    */
#define INTRCTRL_NMI_INTR1_LSP_SYS_DED                    (0x10000UL)     /**< @brief NMI interrupt for LSP SYS DED           */
#define INTRCTRL_NMI_INTR1_Q8_3_DED                       (0x8000UL)      /**< @brief NMI interrupt for Q8 instace 3 DED*/
#define INTRCTRL_NMI_INTR1_Q8_2_DED                       (0x4000UL)      /**< @brief NMI interrupt for Q8 instace 2 DED*/
#define INTRCTRL_NMI_INTR1_Q8_1_DED                       (0x2000UL)      /**< @brief NMI interrupt for Q8 instace 1 DED*/
#define INTRCTRL_NMI_INTR1_Q8_0_DED                       (0x1000UL)      /**< @brief NMI interrupt for Q8 instace 0 DED*/
#define INTRCTRL_NMI_INTR1_MCU1_DED                       (0x800UL)       /**< @brief NMI interrupt for MCU1 DED    */
#define INTRCTRL_NMI_INTR1_HSM_LITE_DED                   (0x400UL)       /**< @brief NMI interrupt for HSM LITE DED*/
#define INTRCTRL_NMI_INTR1_LSP_BSPI_DED                   (0x200UL)       /**< @brief NMI interrupt for BSPI DED */
#define INTRCTRL_NMI_INTR1_PWR_TEMPERATURE_ALARM          (0x100UL)       /**< @brief NMI interrupt for power temperature alarm */
#define INTRCTRL_NMI_INTR1_PWR_BRWNOUT_ALARM              (0x80UL)        /**< @brief NMI interrupt for power brwnout alarm     */
#define INTRCTRL_NMI_INTR1_PWR_BRWNOUT_FAULT              (0x40UL)        /**< @brief NMI interrupt for power brwnout fault     */
#define INTRCTRL_NMI_INTR1_NOC_SVCNW_NOC_FAULT            (0x1UL)         /**< @brief NMI interrupt for NOC SVCNW NOC FAULT   */

/** @} */
/**
    @name INTRCTRL_NmiIntrType2
    @{
    @brief Interrupt Type for NMI interrupt

    @trace #BRCM_SWREQ_INTRCTRL_NMI_INTR
 */
typedef uint32_t INTRCTRL_NmiIntrType2;                                         /**< @brief NMI interrupt type */
#define INTRCTRL_NMI_INTR2_RIGEL_CTRL_GPIO_SW_0            (0x20000000UL)   /**< @brief NMI interrupt for Ctrl GPIO SW 0   */
#define INTRCTRL_NMI_INTR2_ADC0_ACQ_HSADC_FUNC_ERR         (0x10000000UL)   /**< @brief NMI interrupt for ADC0 HSADC function error*/
#define INTRCTRL_NMI_INTR2_ADC1_ACQ_HSADC_FUNC_ERR         (0x8000000UL)    /**< @brief NMI interrupt for ADC1 HSADC function error*/
#define INTRCTRL_NMI_INTR2_ADC2_ACQ_HSADC_FUNC_ERR         (0x4000000UL)    /**< @brief NMI interrupt for ADC2 HSADC function error*/
#define INTRCTRL_NMI_INTR2_ADC3_ACQ_HSADC_FUNC_ERR         (0x2000000UL)    /**< @brief NMI interrupt for ADC3 HSADC function error*/
#define INTRCTRL_NMI_INTR2_QSPI0_TXFIFO_UNDFLW             (0x1000000UL)    /**< @brief NMI interrupt for QSPI0 TXFIFO Under flow    */
#define INTRCTRL_NMI_INTR2_QSPI0_TXFIFO_OVRFLW             (0x800000UL)     /**< @brief NMI interrupt for QSPI0 TXFIFO Over flow    */
#define INTRCTRL_NMI_INTR2_QSPI0_RXFIFO_UNDFLW             (0x400000UL)     /**< @brief NMI interrupt for QSPI0 RXFIFO Under flow    */
#define INTRCTRL_NMI_INTR2_QSPI0_RXFIFO_OVRFLW             (0x200000UL)     /**< @brief NMI interrupt for QSPI0 RXFIFO Over flow    */
#define INTRCTRL_NMI_INTR2_QSPI1_TXFIFO_UNDFLW             (0x100000UL)     /**< @brief NMI interrupt for QSPI1 TXFIFO Under flow    */
#define INTRCTRL_NMI_INTR2_QSPI1_TXFIFO_OVRFLW             (0x80000UL)      /**< @brief NMI interrupt for QSPI1 TXFIFO Over flow    */
#define INTRCTRL_NMI_INTR2_QSPI1_RXFIFO_UNDFLW             (0x40000UL)      /**< @brief NMI interrupt for QSPI1 RXFIFO Under flow    */
#define INTRCTRL_NMI_INTR2_QSPI1_RXFIFO_OVRFLW             (0x20000UL)      /**< @brief NMI interrupt for QSPI1 RXFIFO Over flow    */
#define INTRCTRL_NMI_INTR2_QSPI2_TXFIFO_UNDFLW             (0x10000UL)      /**< @brief NMI interrupt for QSPI2 TXFIFO Under flow    */
#define INTRCTRL_NMI_INTR2_QSPI2_TXFIFO_OVRFLW             (0x8000UL)       /**< @brief NMI interrupt for QSPI2 TXFIFO Over flow    */
#define INTRCTRL_NMI_INTR2_QSPI2_RXFIFO_UNDFLW             (0x4000UL)       /**< @brief NMI interrupt for QSPI2 RXFIFO Under flow    */
#define INTRCTRL_NMI_INTR2_QSPI2_RXFIFO_OVRFLW             (0x2000UL)       /**< @brief NMI interrupt for QSPI2 RXFIFO Over flow    */
#define INTRCTRL_NMI_INTR2_WWDT0                           (0x1000UL)       /**< @brief NMI interrupt for WWDT0 */
#define INTRCTRL_NMI_INTR2_WWDT1                           (0x800UL)        /**< @brief NMI interrupt for WWDT1 */
#define INTRCTRL_NMI_INTR2_WWDT2                           (0x400UL)        /**< @brief NMI interrupt for WWDT2 */
#define INTRCTRL_NMI_INTR2_WWDT3                           (0x200UL)        /**< @brief NMI interrupt for WWDT3 */
#define INTRCTRL_NMI_INTR2_WWDT4                           (0x100UL)        /**< @brief NMI interrupt for WWDT4 */
#define INTRCTRL_NMI_INTR2_WWDT5                           (0x80UL)         /**< @brief NMI interrupt for WWDT5 */
#define INTRCTRL_NMI_INTR2_WWDT6                           (0x40UL)         /**< @brief NMI interrupt for WWDT6 */
#define INTRCTRL_NMI_INTR2_WWDT7                           (0x20UL)         /**< @brief NMI interrupt for WWDT7 */
#define INTRCTRL_NMI_INTR2_APPROC_NERRIRQ_0                (0x10UL)         /**< @brief NMI interrupt for APPROC NERRIRQ 0 */
#define INTRCTRL_NMI_INTR2_APPROC_NERRIRQ_1                (0x8UL)          /**< @brief NMI interrupt for APPROC NERRIRQ 1 */
#define INTRCTRL_NMI_INTR2_APPROC_NERRIRQ_2                (0x4UL)          /**< @brief NMI interrupt for APPROC NERRIRQ 2 */
#define INTRCTRL_NMI_INTR2_APPROC_NERRIRQ_3                (0x2UL)          /**< @brief NMI interrupt for APPROC NERRIRQ 3 */
#define INTRCTRL_NMI_INTR2_APPROC_NERRIRQ_4                (0x1UL)          /**< @brief NMI interrupt for APPROC NERRIRQ 4 */

/** @} */

/**
    @brief NMI interrupt Type

    @trace #BRCM_SWREQ_INTRCTRL_NMI_INTR
*/
typedef struct sINTRCTRL_NmiIntrType {
    INTRCTRL_NmiIntrType0 intrType0;              /**< @brief Interrupt type 0  */
    INTRCTRL_NmiIntrType1 intrType1;              /**< @brief Interrupt type 1  */
    INTRCTRL_NmiIntrType2 intrType2;              /**< @brief Interrupt type 2  */
} INTRCTRL_NmiIntrType;


/**
    @name INTRCTRL_GpioIntrType
    @{
    @brief INTRCTRL GPIO Interrupt Type

    @trace #BRCM_SWREQ_INTRCTRL_GENERAL_INTR
 */
typedef uint32_t INTRCTRL_GpioIntrType;      /**< @brief Select GPIO interrupt type. */
#define INTRCTRL_GPIO_INTR_GRP0  (0x0FUL)    /**< @brief GPIO interrupt Group0       */
#define INTRCTRL_GPIO_INTR_0     (1UL<< 0UL) /**< @brief GPIO interrupt 0.           */
#define INTRCTRL_GPIO_INTR_1     (1UL<< 1UL) /**< @brief GPIO interrupt 1.           */
#define INTRCTRL_GPIO_INTR_2     (1UL<< 2UL) /**< @brief GPIO interrupt 2.           */
#define INTRCTRL_GPIO_INTR_3     (1UL<< 3UL) /**< @brief GPIO interrupt 3.           */
#define INTRCTRL_GPIO_INTR_GRP1  (0xF0UL)    /**< @brief GPIO interrupt Group1.      */
#define INTRCTRL_GPIO_INTR_4     (1UL<< 4UL) /**< @brief GPIO interrupt 4.           */
#define INTRCTRL_GPIO_INTR_5     (1UL<< 5UL) /**< @brief GPIO interrupt 5.           */
#define INTRCTRL_GPIO_INTR_6     (1UL<< 6UL) /**< @brief GPIO interrupt 6.           */
#define INTRCTRL_GPIO_INTR_7     (1UL<< 7UL) /**< @brief GPIO interrupt 7.           */
#define INTRCTRL_GPIO_INTR_ALL   (0xFFUL   ) /**< @brief GPIO interrupt All.         */
/** @} */

/** @brief Get MCU Pending status

    This API gets MCU shared (IRQ line number 6) interrupt pending status of MCU 0/1 .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the Interrupt controller
    @param[out]     intrStatus     status of various interrupts @ref INTRCTRL_McuIrq6StsType

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   invalid memory for intrStatus
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS

    @limitations None
*/
int32_t INTRCTRL_McuIrq6Status(INTRCTRL_HwIDType hwId, INTRCTRL_McuIrq6StsType *intrStatus);


/** @brief Get MCU Pending status

    This API gets MCU shared (IRQ line number 7) interrupt pending status of MCU 0/1 .

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the Interrupt controller
    @param[out]     intrStatus     status of various interrupts @ref INTRCTRL_McuIrq7StsType

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   invalid memory for intrStatus
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS

    @limitations None
*/
int32_t INTRCTRL_McuIrq7Status(INTRCTRL_HwIDType hwId, INTRCTRL_McuIrq7StsType *intrStatus);

/** @brief Get Q8 Pending status

    This API gets Q8 shared (IRQ line number 7) interrupt pending status of Q8 0/1/2/3 .

    @behavior Sync, Non-reentrant

    @pre None
    @param[in]      hwId         Index of the Interrupt controller
    @param[out]     intrStatus     status of various interrupts @ref INTRCTRL_Q8Irq7StsType

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   invalid memory for intrStatus
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS

    @limitations None
*/
int32_t INTRCTRL_Q8Irq7Status(INTRCTRL_HwIDType hwId, INTRCTRL_Q8Irq7StsType *intrStatus);

/** @brief Get GIC SPI Pending status

    This API gets GIC SPI shared (IRQ line number 6) interrupt pending status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      hwId         Index of the Interrupt controller
    @param[out]     intrStatus     status of various interrupts @ref INTRCTRL_GicSPIIrq6StsType

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   invalid memory for intrStatus
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SHARED_IRQ_STATUS

    @limitations None
*/
int32_t INTRCTRL_GicSpiIrq6Status(INTRCTRL_HwIDType hwId, INTRCTRL_GicSPIIrq6StsType *intrStatus);


/** @brief Configure safety monitor interrupts

    This API enables/disables safety monitor interrupt.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId            Index of the Interrupt controller
    @param[in]     intrBlock       Interrupt block to enable/Disable
    @param[in]     intrType        Interrupt type based on the block
    @param[in]     enDis           Enables(1)/Disables(0) interrupt

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_SM_INTR

    @limitations None
*/

int32_t INTRCTRL_DrvSmIntrEnable(INTRCTRL_HwIDType hwId, INTRCTRL_SmIntrBlkType intrBlock, INTRCTRL_SmBlkIntrType intrType, uint8_t enDis);

/** @brief Configure NMI interrupts

    This API enables/disables NMI interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId            Index of the Interrupt controller
    @param[in]     nmiIntrType     NMI Interrupt type
    @param[in]     enDis           Enables(1)/Disables(0) interrupt

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid memory for intrType
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_NMI_INTR

    @limitations None
*/

int32_t INTRCTRL_DrvNmiIntrEnable(INTRCTRL_HwIDType hwId, INTRCTRL_NmiIntrType *nmiIntrType, uint8_t enDis);

/** @brief Get NMI interrupt pending status

    This API gets NMI interrupts status.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]     hwId            Index of the Interrupt controller
    @param[out]     nmiIntrType     Interrupt Status

    @return     int32_t

    Return values are
    @retval     #BCM_ERR_NODEV          No device found
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid memory for intrType
    @retval     #BCM_ERR_OK             Success

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_NMI_INTR

    @limitations None
*/

int32_t INTRCTRL_DrvNmiIntrStatus(INTRCTRL_HwIDType hwId, INTRCTRL_NmiIntrType *nmiIntrType);


/** @brief Enable GPIO interrupts

    This API enable GPIO interrupts based on interrupt type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId       Index of the INTRCTRL
    @param[in]   intrType   Specified interrupt type
    @param[out]  aEnable    Enable/disable control
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_GENERAL_INTR

    @limitations None
*/
int32_t INTRCTRL_DrvEnableGpioInterrupt(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType intrType, uint32_t aEnable);

/** @brief GPIO interrupt status

    This API gets GPIO interrupt status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId      Index of the INTRCTRL
    @param[in]   intrSts   Pointer to interrupt status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL interrupt status

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_GENERAL_INTR

    @limitations None
*/
int32_t INTRCTRL_DrvGpioInterruptStatus(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType *intrSts);

/** @brief GPIO event status

    This API gets GPIO event status

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId      Index of the INTRCTRL
    @param[in]   eventSts  Pointer to event status
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index
    @retval     #BCM_ERR_INVAL_PARAMS   NULL event status

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_GENERAL_INTR

    @limitations None
*/
int32_t INTRCTRL_DrvGpioEventStatus(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType *eventSts);

/** @brief GPIO clear interrupts

    This API clears GPIO interrupt based on interrupt type

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   hwId       Index of the INTRCTRL
    @param[in]   intrType   Interrupt type to clear the interrupt
    @return      int32_t

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_INTRCTRL_GENERAL_INTR

    @limitations None
*/
int32_t INTRCTRL_DrvClearGpioInterrupt(INTRCTRL_HwIDType hwId, INTRCTRL_GpioIntrType intrType);

#endif /* INTRCTRL_H */

/** @} */
