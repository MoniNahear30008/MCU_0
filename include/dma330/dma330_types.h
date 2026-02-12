/*****************************************************************************
  Copyright 2024 Broadcom Limited.  All rights reserved.
  Broadcom Proprietary and Confidential
******************************************************************************/
#ifndef DMA330_TYPES_H
#define DMA330_TYPES_H

#include <dma330.h>

typedef volatile struct sDMA330_RegsType{
    uint32_t    DSR;
    uint32_t    DPC;
    uint32_t    RSVD1[6];
    uint32_t    INTEN;
    uint32_t    INT_EVENT_RIS;
    uint32_t    INTMIS;
    uint32_t    INTCLR;
    uint32_t    FSRD;
    uint32_t    FSRC;
    uint32_t    FTRD;
    uint32_t    RSVD2[1];
    uint32_t    FTR0;
    uint32_t    FTR1;
    uint32_t    FTR2;
    uint32_t    FTR3;
    uint32_t    FTR4;
    uint32_t    FTR5;
    uint32_t    FTR6;
    uint32_t    FTR7;
    uint32_t    RSVD3[40];
    uint32_t    CSR_CPC[16];
#define CHANNEL_STATE_STOPPED       (0x0UL)
#define CHANNEL_STATE_EXECUTING     (0x1UL)
#define CHANNEL_STATE_CACHE_MISS    (0x2UL)
#define CHANNEL_STATE_UPDATING_PC   (0x3UL)
#define CHANNEL_STATE_WFE           (0x4UL)
#define CHANNEL_STATE_AT_BARRIER    (0x5UL)
#define CHANNEL_STATE_WFP           (0x7UL)
#define CHANNEL_STATE_KILLING       (0x8UL)
#define CHANNEL_STATE_COMPL         (0x9UL)
#define CHANNEL_STATE_FAULT_COMPL   (0xEUL)
#define CHANNEL_STATE_FAULTING      (0xFUL)

    uint32_t    RSVD4[176];
    uint32_t    SAR0;
    uint32_t    DAR0;
    uint32_t    CCR0;
    uint32_t    LC0_0;
    uint32_t    LC1_0;
    uint32_t    RSVD5[3];
    uint32_t    SAR1;
    uint32_t    DAR1;
    uint32_t    CCR1;
    uint32_t    LC0_1;
    uint32_t    LC1_1;
    uint32_t    RSVD6[3];
    uint32_t    SAR2;
    uint32_t    DAR2;
    uint32_t    CCR2;
    uint32_t    LC0_2;
    uint32_t    LC1_2;
    uint32_t    RSVD7[3];
    uint32_t    SAR3;
    uint32_t    DAR3;
    uint32_t    CCR3;
    uint32_t    LC0_3;
    uint32_t    LC1_3;
    uint32_t    RSVD8[3];
    uint32_t    SAR4;
    uint32_t    DAR4;
    uint32_t    CCR4;
    uint32_t    LC0_4;
    uint32_t    LC1_4;
    uint32_t    RSVD9[3];
    uint32_t    SAR5;
    uint32_t    DAR5;
    uint32_t    CCR5;
    uint32_t    LC0_5;
    uint32_t    LC1_5;
    uint32_t    RSVD10[3];
    uint32_t    SAR6;
    uint32_t    DAR6;
    uint32_t    CCR6;
    uint32_t    LC0_6;
    uint32_t    LC1_6;
    uint32_t    RSVD11[3];
    uint32_t    SAR7;
    uint32_t    DAR7;
    uint32_t    CCR7;
    uint32_t    LC0_7;
    uint32_t    LC1_7;
    uint32_t    RSVD12[515];
    uint32_t    DBGSTATUS;
    uint32_t    DBGCMD;
    uint32_t    DBGINST0;
    uint32_t    DBGINST1;
    uint32_t    RSVD13[60];
    uint32_t    CR0;
    uint32_t    CR1;
    uint32_t    CR2;
    uint32_t    CR3;
    uint32_t    CR4;
    uint32_t    CRD;
    uint32_t    RSVD14[26];
    uint32_t    WD;
    uint32_t    RSVD15[87];
    uint32_t    periph_id_0;
    uint32_t    periph_id_1;
    uint32_t    periph_id_2;
    uint32_t    periph_id_3;
    uint32_t    pcell_id_0;
    uint32_t    pcell_id_1;
    uint32_t    pcell_id_2;
    uint32_t    pcell_id_3;
} DMA330_RegsType;

uint32_t DMA330_AbortIRQHandler(DMA330_ConfigType aCfgTbl);


#endif /* DMA330_TYPES_H */
