/*****************************************************************************
 Copyright 2025 Broadcom.  All rights reserved.
 Broadcom Proprietary and Confidential
******************************************************************************/
/**
    @file ipc_rdb.h
    @brief RDB File for IPC

    @version BCM89580_A0_20230125_RDB
*/

#ifndef IPC_RDB_H
#define IPC_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t IPC_DOORBELL_TYPE;
#define IPC_DOORBELL_CONTENT_MASK (0xfffffffcUL)
#define IPC_DOORBELL_CONTENT_SHIFT (2UL)
#define IPC_DOORBELL_CMD_OR_PTR_MASK (0x2UL)
#define IPC_DOORBELL_CMD_OR_PTR_SHIFT (1UL)
#define IPC_DOORBELL_FULL_MASK (0x1UL)
#define IPC_DOORBELL_FULL_SHIFT (0UL)




typedef uint32_t IPC_INTCFG_TYPE;
#define IPC_INTCFG_PROCESSORID_MASK (0xffffffUL)
#define IPC_INTCFG_PROCESSORID_SHIFT (0UL)




typedef uint32_t IPC_POLICY_TYPE;
#define IPC_POLICY_PRIVILEGED_MASK (0x80000000UL)
#define IPC_POLICY_PRIVILEGED_SHIFT (31UL)
#define IPC_POLICY_NORMAL_MASK (0x40000000UL)
#define IPC_POLICY_NORMAL_SHIFT (30UL)
#define IPC_POLICY_PROCESSORID_MASK (0xffffffUL)
#define IPC_POLICY_PROCESSORID_SHIFT (0UL)




typedef uint32_t IPC_IPCTX_TYPE;
#define IPC_IPCTX_DOORBELLID_MASK (0xffffUL)
#define IPC_IPCTX_DOORBELLID_SHIFT (0UL)




typedef uint32_t IPC_FLAG_TYPE;
#define IPC_FLAG_MASK (0xffffUL)
#define IPC_FLAG_SHIFT (0UL)




typedef uint32_t IPC_IPCRX_TYPE;
#define IPC_IPCRX_RX_MASK (0xffffUL)
#define IPC_IPCRX_RX_SHIFT (0UL)




typedef uint32_t IPC_TXMSK_TYPE;
#define IPC_TXMSK_TX_MASK_MASK (0xffffUL)
#define IPC_TXMSK_TX_MASK_SHIFT (0UL)




typedef uint32_t IPC_RXMSK_TYPE;
#define IPC_RXMSK_RX_MASK_MASK (0xffffUL)
#define IPC_RXMSK_RX_MASK_SHIFT (0UL)



#define IPC_DOORBELL_SIZE  (16UL)

#define IPC_INT_CFG_SIZE  (16UL)

#define IPC_POLICY_SIZE  (16UL)


typedef volatile struct sIPC_RDBType {
    IPC_DOORBELL_TYPE doorbell[IPC_DOORBELL_SIZE]; /* OFFSET: 0x0 */
    IPC_INTCFG_TYPE ipc_int_cfg[IPC_INT_CFG_SIZE]; /* OFFSET: 0x40 */
    IPC_POLICY_TYPE ipc_policy[IPC_POLICY_SIZE]; /* OFFSET: 0x80 */
    IPC_IPCTX_TYPE ipc_tx; /* OFFSET: 0xc0 */
    IPC_FLAG_TYPE ipc_flag; /* OFFSET: 0xc4 */
    IPC_IPCRX_TYPE ipc_rx; /* OFFSET: 0xc8 */
    IPC_TXMSK_TYPE ipc_tx_msk; /* OFFSET: 0xcc */
    IPC_RXMSK_TYPE ipc_rx_msk; /* OFFSET: 0xd0 */
} IPC_RDBType;


#define IPC_BASE                        (0x10264000UL)



#define IPC_MAX_HW_ID                   (1UL)


#define IPC_NUM_CHANNELS                (16UL)

#endif /* IPC_RDB_H */
