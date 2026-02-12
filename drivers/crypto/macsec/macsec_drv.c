/*****************************************************************************
 Copyright 2024 Broadcom Limited.  All rights reserved.

 Broadcom Proprietary and Confidential
******************************************************************************/
/**
   @defgroup grp_macsecdrv_internal_impl MACSec driver implementation
   @ingroup grp_macsecdrv

   @addtogroup grp_macsecdrv_internal_impl
   @{
   @file macsec_drv.c
   @brief macsec driver implementation.
   This file contains the MACSec driver implementation

*/
#define MACSEC_IRQ_SUPPORT_DISABLE
#include <string.h>
#include <bcm_comp.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include "aes.h"
#include <crc.h>
#include <crypto_secy.h>
#include <ethxcvr.h>
#include "macsec_bus.h"
#include "macsec_platform.h"
#ifndef MACSEC_IRQ_SUPPORT_DISABLE
#include <system_mcu.h>
#endif
#include "rdb/macsec_sysmisc_rdb.h"
#include "rdb/macsec_e_eip62_rdb.h"
#include "rdb/macsec_i_eip62_rdb.h"

/* MSPU top */
#include "rdb/macsec_mspu_mac_rdb.h"
#include "rdb/macsec_mspu_top_rdb.h"

/* classification control */
#include "rdb/macsec_e_cls_ctrl_rdb.h"
#include "rdb/macsec_i_cls_ctrl_rdb.h"

/* classification packet control */
#include "rdb/macsec_e_cls_p_ctrl_rdb.h"
#include "rdb/macsec_i_cls_p_ctrl_rdb.h"

/* TCAM mask and policy registers */
#include "rdb/macsec_e_tcamkm_rdb.h"
#include "rdb/macsec_e_tcamkp_rdb.h"
#include "rdb/macsec_i_tcamkm_rdb.h"
#include "rdb/macsec_i_tcamkp_rdb.h"

/* flow control */
#include "rdb/macsec_e_fctrl_en_rdb.h"
#include "rdb/macsec_e_fctrl_rdb.h"
#include "rdb/macsec_i_fctrl_en_rdb.h"
#include "rdb/macsec_i_fctrl_rdb.h"

/* transform record */
#include "rdb/macsec_e_transrec_rdb.h"
#include "rdb/macsec_i_transrec_rdb.h"

/* SC/SA map registers */
#include "rdb/macsec_e_scsa_map_rdb.h"
#include "rdb/macsec_i_scsa_map_rdb.h"

/* RxCAM key and enable registers */
#include "rdb/macsec_i_rxsc_camkey_rdb.h"
#include "rdb/macsec_i_rxsc_camen_rdb.h"

/* Stats and counters */
#include "rdb/macsec_e_secy_cntrs_rdb.h"
#include "rdb/macsec_i_secy_cntrs_rdb.h"
#include "rdb/macsec_e_sa_cntrs_rdb.h"
#include "rdb/macsec_i_sa_cntrs_rdb.h"
#include "rdb/macsec_e_secy_cntrs_ctrl_rdb.h"
#include "rdb/macsec_e_sa_cntrs_ctrl_rdb.h"
#include "rdb/macsec_i_secy_cntrs_ctrl_rdb.h"
#include "rdb/macsec_i_sa_cntrs_ctrl_rdb.h"

#include "rdb/macsec_e_oppe_rdb.h"

/* BRCM global */
#include "rdb/macsec_e_brcm_global_rdb.h"
#include "rdb/macsec_i_brcm_global_rdb.h"

/* interrupt */
#include "rdb/macsec_e_eip160_aic_regs_rdb.h"
#include "rdb/macsec_eip165_aic_regs_rdb.h"

/**
    @name MACSEC driver Design IDs
    @{
    @brief Design IDs for MACSec driver
*/

#define BRCM_SWDSGN_MACSEC_SECYID_MAGIC_MACRO                   (0xB100U) /**< @brief #MACSEC_SECYID_MAGIC            */
#define BRCM_SWDSGN_MACSEC_CP_MACDAETH_MAX_RULES_MACRO          (0xB101U) /**< @brief #MACSEC_CP_MACDAETH_MAX_RULES   */
#define BRCM_SWDSGN_MACSEC_CP_MACDAETRANGE_MAX_RULES_MACRO      (0xB102U) /**< @brief #MACSEC_CP_MACDAETRANGE_MAX_RULES */
#define BRCM_SWDSGN_MACSEC_MAX_SC_MACRO                         (0xB103U) /**< @brief #MACSEC_MAX_SC                  */
#define BRCM_SWDSGN_MACSEC_MAX_TCAM_ENTRIES_MACRO               (0xB104U) /**< @brief #MACSEC_MAX_TCAM_ENTRIES        */
#define BRCM_SWDSGN_MACSEC_MAX_VPORTS_MACRO                     (0xB105U) /**< @brief #MACSEC_MAX_VPORTS              */
#define BRCM_SWDSGN_MACSEC_MAX_XFORM_RECORDS_MACRO              (0xB106U) /**< @brief #MACSEC_MAX_XFORM_RECORDS       */
#define BRCM_SWDSGN_MACSEC_XREC_MACRO                           (0xB107U) /**< @brief #MACSEC_XREC_WORDS_PER_ENTRY    */
#define BRCM_SWDSGN_MACSEC_LOAD_UNSAFE_TIMEOUT_CNT_MACRO        (0xB108U) /**< @brief #MACSEC_LOAD_UNSAFE_TIMEOUT_CNT */
#define BRCM_SWDSGN_MACSEC_MAX_REPLAY_WINDOW_MACRO              (0xB109U) /**< @brief #MACSEC_MAX_REPLAY_WINDOW       */
#define BRCM_SWDSGN_MACSEC_HANDLE_INFO_INIT_MACRO               (0xB10AU) /**< @brief #MACSEC_HANDLE_INFO_INIT        */
#define BRCM_SWDSGN_MACSEC_MAC_BYTES_TO_UINT_MACRO              (0xB10BU) /**< @brief #MACSEC_MAC_BYTES_TO_UINT_HI    */
#define BRCM_SWDSGN_MACSEC_BYTE_SWAPPED_MACRO                   (0xB10CU) /**< @brief #MACSEC_BYTE_SWAPPED            */
#define BRCM_SWDSGN_MACSEC_BYTES_TO_LE32_MACRO                  (0xB10DU) /**< @brief #MACSEC_BYTES_TO_LE32           */
#define BRCM_SWDSGN_MACSEC_DIR_TYPE                             (0xB10EU) /**< @brief #MACSEC_DirType                 */
#define BRCM_SWDSGN_MACSEC_DROP_TYPE                            (0xB10FU) /**< @brief #MACSEC_DropActionType          */
#define BRCM_SWDSGN_MACSEC_NONSAACTION_TYPE                     (0xB110U) /**< @brief #MACSEC_NonSAActionType         */
#define BRCM_SWDSGN_MACSEC_ETHFRAME_TYPE                        (0xB112U) /**< @brief #MACSEC_EthFrameType            */
#define BRCM_SWDSGN_MACSEC_ETHNUMTAGS_TYPE                      (0xB113U) /**< @brief #MACSEC_EthNumTagsType          */
#define BRCM_SWDSGN_MACSEC_SAACTION_TYPE                        (0xB114U) /**< @brief #MACSEC_SAActionType            */
#define BRCM_SWDSGN_MACSEC_RULEMASK_TYPE                        (0xB115U) /**< @brief #MACSEC_RuleMaskType            */
#define BRCM_SWDSGN_MACSEC_XRECW0_TYPE                          (0xB117U) /**< @brief #MACSEC_XRecW0Type              */
#define BRCM_SWDSGN_MACSEC_XREC_ID_TYPE                         (0xB118U) /**< @brief #MACSEC_XRecIDType              */
#define BRCM_SWDSGN_MACSEC_XRECSAUPDCTRL_TYPE                   (0xB138U) /**< @brief #MACSEC_XRecSAUpdCtrlType       */
#define BRCM_SWDSGN_MACSEC_HANDLEINFO_TYPE                      (0xB13AU) /**< @brief #MACSEC_HandleInfoType          */
#define BRCM_SWDSGN_MACSEC_TCAMRULE_TYPE                        (0xB13BU) /**< @brief #MACSEC_TCAMRuleType            */
#define BRCM_SWDSGN_MACSEC_EG_IG_SEC_YSTATS_TYPE                (0xB13CU) /**< @brief #MACSEC_EgIgSecYStatsType       */
#define BRCM_SWDSGN_MACSEC_EG_SASTATS_TYPE                      (0xB13DU) /**< @brief #MACSEC_EgSAStatsType           */
#define BRCM_SWDSGN_MACSEC_IG_SASTATS_TYPE                      (0xB13EU) /**< @brief #MACSEC_IgSAStatsType           */
#define BRCM_SWDSGN_MACSEC_SECY_CACHED_STATS_TYPE               (0xB13FU) /**< @brief #MACSEC_SecYCachedStatsType     */
#define BRCM_SWDSGN_MACSEC_EGRESS_XRECAES128_TYPE               (0xB140U) /**< @brief #MACSEC_EgressXRecAES128Type        */
#define BRCM_SWDSGN_MACSEC_EGRESS_XRECAES256_TYPE               (0xB141U) /**< @brief #MACSEC_EgressXRecAES256Type        */
#define BRCM_SWDSGN_MACSEC_EGRESS_XRECAES128XPN_TYPE            (0xB142U) /**< @brief #MACSEC_EgressXRecAES128XpnType     */
#define BRCM_SWDSGN_MACSEC_EGRESS_XRECAES256XPN_TYPE            (0xB143U) /**< @brief #MACSEC_EgressXRecAES256XpnType     */
#define BRCM_SWDSGN_MACSEC_EGRESS_XRECDESC_TYPE                 (0xB144U) /**< @brief #MACSEC_EgressXRecDescType          */
#define BRCM_SWDSGN_MACSEC_INGRESS_XRECAES128_TYPE              (0xB145U) /**< @brief #MACSEC_IngressXRecAES128Type       */
#define BRCM_SWDSGN_MACSEC_INGRESS_XRECAES256_TYPE              (0xB146U) /**< @brief #MACSEC_IngressXRecAES256Type       */
#define BRCM_SWDSGN_MACSEC_INGRESS_XRECAES128XPN_TYPE           (0xB147U) /**< @brief #MACSEC_IngressXRecAES128XpnType    */
#define BRCM_SWDSGN_MACSEC_INGRESS_XRECAES256XPN_TYPE           (0xB148U) /**< @brief #MACSEC_IngressXRecAES256XpnType    */
#define BRCM_SWDSGN_MACSEC_INGRESS_XRECDESC_TYPE                (0xB149U) /**< @brief #MACSEC_IngressXRecDescType     */
#define BRCM_SWDSGN_MACSEC_SA_CONTEXT_TYPE                      (0xB14CU) /**< @brief #MACSEC_SAContextType           */
#define BRCM_SWDSGN_MACSEC_TX_SCCONTEXT_TYPE                    (0xB14DU) /**< @brief #MACSEC_TxSCContextType         */
#define BRCM_SWDSGN_MACSEC_RX_SCCONTEXT_TYPE                    (0xB14EU) /**< @brief #MACSEC_RxSCContextType         */
#define BRCM_SWDSGN_MACSEC_SECY_CONTEXT_TYPE                    (0xB14FU) /**< @brief #MACSEC_SecYContextType         */
#define BRCM_SWDSGN_MACSEC_CONTEXT_TYPE                         (0xB150U) /**< @brief #MACSEC_ContextType             */
#define BRCM_SWDSGN_MACSEC_HWCONTEXT_TYPE                       (0xB151U) /**< @brief #MACSEC_HwContextType           */
#define BRCM_SWDSGN_MACSEC_SYSMISC_OFFSETOF_MACRO               (0xB152U) /**< @brief #MACSEC_SYSMISC_OFFSET_OF            */
#define BRCM_SWDSGN_MACSEC_E_CLS_CTRL_OFFSETOF_MACRO            (0xB153U) /**< @brief #MACSEC_E_CLS_CTRL_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_E_CLS_P_CTRL_OFFSETOF_MACRO          (0xB154U) /**< @brief #MACSEC_E_CLS_P_CTRL_OFFSET_OF       */
#define BRCM_SWDSGN_MACSEC_I_CLS_CTRL_OFFSETOF_MACRO            (0xB155U) /**< @brief #MACSEC_I_CLS_CTRL_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_I_CLS_P_CTRL_OFFSETOF_MACRO          (0xB156U) /**< @brief #MACSEC_I_CLS_P_CTRL_OFFSET_OF       */
#define BRCM_SWDSGN_MACSEC_E_EIP62_OFFSETOF_MACRO               (0xB157U) /**< @brief #MACSEC_E_EIP62_OFFSET_OF            */
#define BRCM_SWDSGN_MACSEC_I_EIP62_OFFSETOF_MACRO               (0xB158U) /**< @brief #MACSEC_I_EIP62_OFFSET_OF            */
#define BRCM_SWDSGN_MACSEC_MSPU_MAC_OFFSETOF_MACRO              (0xB159U) /**< @brief #MACSEC_MSPU_MAC_OFFSET_OF           */
#define BRCM_SWDSGN_MACSEC_E_TCAMKM_OFFSETOF_MACRO              (0xB15BU) /**< @brief #MACSEC_E_TCAMKM_OFFSET_OF           */
#define BRCM_SWDSGN_MACSEC_I_TCAMKM_OFFSETOF_MACRO              (0xB15CU) /**< @brief #MACSEC_I_TCAMKM_OFFSET_OF           */
#define BRCM_SWDSGN_MACSEC_E_TCAMKP_OFFSETOF_MACRO              (0xB15DU) /**< @brief #MACSEC_E_TCAMKP_OFFSET_OF           */
#define BRCM_SWDSGN_MACSEC_E_FCTRL_OFFSETOF_MACRO               (0xB15EU) /**< @brief #MACSEC_E_FCTRL_OFFSET_OF            */
#define BRCM_SWDSGN_MACSEC_E_FCTRL_EN_OFFSETOF_MACRO            (0xB15FU) /**< @brief #MACSEC_E_FCTRL_EN_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_I_FCTRL_OFFSETOF_MACRO               (0xB160U) /**< @brief #MACSEC_I_FCTRL_OFFSET_OF            */
#define BRCM_SWDSGN_MACSEC_I_FCTRL_EN_OFFSETOF_MACRO            (0xB161U) /**< @brief #MACSEC_I_FCTRL_EN_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_E_TRANSREC_OFFSETOF_MACRO            (0xB162U) /**< @brief #MACSEC_E_TRANSREC_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_I_TRANSREC_OFFSETOF_MACRO            (0xB163U) /**< @brief #MACSEC_I_TRANSREC_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_E_SCSA_MAP_OFFSETOF_MACRO            (0xB164U) /**< @brief #MACSEC_E_SCSA_MAP_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_I_SCSA_MAP_OFFSETOF_MACRO            (0xB165U) /**< @brief #MACSEC_I_SCSA_MAP_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_I_RXCAMKEY_OFFSETOF_MACRO            (0xB166U) /**< @brief #MACSEC_I_RXSC_CAMKEY_OFFSET_OF      */
#define BRCM_SWDSGN_MACSEC_I_RXCAMEN_OFFSETOF_MACRO             (0xB167U) /**< @brief #MACSEC_I_RXSC_CAMEN_OFFSET_OF       */
#define BRCM_SWDSGN_MACSEC_E_OPPE_OFFSETOF_MACRO                (0xB168U) /**< @brief #MACSEC_E_OPPE_OFFSET_OF             */
#define BRCM_SWDSGN_MACSEC_E_SECY_CNTRS_OFFSETOF_MACRO          (0xB169U) /**< @brief #MACSEC_E_SECY_CNTRS_OFFSET_OF       */
#define BRCM_SWDSGN_MACSEC_I_SECY_CNTRS_OFFSETOF_MACRO          (0xB16AU) /**< @brief #MACSEC_I_SECY_CNTRS_OFFSET_OF       */
#define BRCM_SWDSGN_MACSEC_E_SA_CNTRS_OFFSETOF_MACRO            (0xB16BU) /**< @brief #MACSEC_E_SA_CNTRS_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_I_SA_CNTRS_OFFSETOF_MACRO            (0xB16CU) /**< @brief #MACSEC_I_SA_CNTRS_OFFSET_OF         */
#define BRCM_SWDSGN_MACSEC_E_SA_CNTRS_CTRL_OFFSETOF_MACRO       (0xB16DU) /**< @brief #MACSEC_E_SA_CNTRS_CTRL_OFFSET_OF    */
#define BRCM_SWDSGN_MACSEC_I_SA_CNTRS_CTRL_OFFSETOF_MACRO       (0xB16EU) /**< @brief #MACSEC_I_SA_CNTRS_CTRL_OFFSET_OF    */
#define BRCM_SWDSGN_MACSEC_E_SECY_CNTRS_CTRL_OFFSETOF_MACRO     (0xB16FU) /**< @brief #MACSEC_E_SECY_CNTRS_CTRL_OFFSET_OF  */
#define BRCM_SWDSGN_MACSEC_I_SECY_CNTRS_CTRL_OFFSETOF_MACRO     (0xB170U) /**< @brief #MACSEC_I_SECY_CNTRS_CTRL_OFFSET_OF  */
#define BRCM_SWDSGN_MACSEC_HWCTXT_GLOBAL                        (0xB171U) /**< @brief #MACSEC_HwCtxt                  */
#define BRCM_SWDSGN_MACSEC_SEED_GLOBAL                          (0xB172U) /**< @brief #MACSEC_Seed                    */
#define BRCM_SWDSGN_MACSEC_DRV_SCITO_U64_PROC                   (0xB173U) /**< @brief #MACSEC_DrvSCIToU64             */
#define BRCM_SWDSGN_MACSEC_DRV_CREATE_HANDLE_PROC               (0xB174U) /**< @brief #MACSEC_DrvCreateHandle         */
#define BRCM_SWDSGN_MACSEC_DRV_HANDLE_UPDATE_HASH_PROC          (0xB175U) /**< @brief #MACSEC_DrvHandleUpdateHash     */
#define BRCM_SWDSGN_MACSEC_DRV_GET_HANDLE_INFO_PROC             (0xB176U) /**< @brief #MACSEC_DrvGetHandleInfo        */
#define BRCM_SWDSGN_MACSEC_DRV_HANDLE_TO_SECY_PROC              (0xB177U) /**< @brief #MACSEC_DrvHandleToSecY         */
#define BRCM_SWDSGN_MACSEC_DRV_HANDLE_TO_SA_PROC                (0xB178U) /**< @brief #MACSEC_DrvHandleToSA           */

#define BRCM_SWDSGN_MACSEC_DRV_COPY_KEY_TO_REC_PROC             (0xB179U) /**< @brief #MACSEC_DrvCopyKeyToRec         */
#define BRCM_SWDSGN_MACSEC_DRV_XREC_UPDATE_PROC                 (0xB17AU) /**< @brief #MACSEC_DrvXRecUpdate           */
#define BRCM_SWDSGN_MACSEC_DRV_XREC_READ_PROC                   (0xB17BU) /**< @brief #MACSEC_DrvXRecRead             */
#define BRCM_SWDSGN_MACSEC_DRV_IN_FLIGHT_SYNC_PROC              (0xB17CU) /**< @brief #MACSEC_DrvInFlightSync         */
#define BRCM_SWDSGN_MACSEC_DRV_ADD_TCAMRULE_PROC                (0xB17FU) /**< @brief #MACSEC_DrvAddTCAMRule          */
#define BRCM_SWDSGN_MACSEC_DRV_REMOVE_TCAMRULE_PROC             (0xB180U) /**< @brief #MACSEC_DrvRemoveTCAMRule       */
#define BRCM_SWDSGN_MACSEC_DRV_ADD_CPRULE_INTERNAL_PROC         (0xB181U) /**< @brief #MACSEC_DrvAddCPRuleInternal    */
#define BRCM_SWDSGN_MACSEC_DRV_NON_MATCHED_FLOW_PROC            (0xB182U) /**< @brief #MACSEC_DrvNonMatchedFlow       */
#define BRCM_SWDSGN_MACSEC_DRV_GLOBAL_INIT_PROC                 (0xB183U) /**< @brief #MACSEC_DrvGlobalInit           */
#define BRCM_SWDSGN_MACSEC_DRV_UNI_MAC_INIT_PROC                (0xB184U) /**< @brief #MACSEC_DrvUniMacInit           */
#define BRCM_SWDSGN_MACSEC_DRV_REMOVE_CPRULE_INTERNAL_PROC      (0xB185U) /**< @brief #MACSEC_DrvRemoveCPRuleInternal */
#define BRCM_SWDSGN_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC      (0xB186U) /**< @brief #MACSEC_DrvRemoveSecYLookupRule */
#define BRCM_SWDSGN_MACSEC_DRV_ALLOC_RX_CAMENTRY_PROC           (0xB187U) /**< @brief #MACSEC_DrvAllocRxCAMEntry      */
#define BRCM_SWDSGN_MACSEC_DRV_SASET_IN_USE_PROC                (0xB188U) /**< @brief #MACSEC_DrvSASetInUse           */
#define BRCM_SWDSGN_MACSEC_DRV_EGRESS_SASWITCH_PROC             (0xB189U) /**< @brief #MACSEC_DrvEgressSASwitch       */
#define BRCM_SWDSGN_MACSEC_DRV_SET_FRAME_VALIDATE_PROC          (0xB18AU) /**< @brief #MACSEC_DrvSetFrameValidate     */
#define BRCM_SWDSGN_MACSEC_DRV_SET_REPLAY_PROTECT_PROC          (0xB18BU) /**< @brief #MACSEC_DrvSetReplayProtect     */
#define BRCM_SWDSGN_MACSEC_DRV_SET_PROTECT_FRAME_PROC           (0xB18CU) /**< @brief #MACSEC_DrvSetProtectFrame      */
#define BRCM_SWDSGN_MACSEC_DRV_GET_SEC_YSTATS_INTERNAL_PROC     (0xB18DU) /**< @brief #MACSEC_DrvGetSecYStatsInternal */
#define BRCM_SWDSGN_MACSEC_DRV_GET_EG_SASTATS_PROC              (0xB18EU) /**< @brief #MACSEC_DrvGetEgSAStats         */
#define BRCM_SWDSGN_MACSEC_DRV_GET_IG_SASTATS_PROC              (0xB18FU) /**< @brief #MACSEC_DrvGetIgSAStats         */
#define BRCM_SWDSGN_MACSEC_DRV_GET_RX_SCSTATS_PROC              (0xB190U) /**< @brief #MACSEC_DrvGetRxSCStats         */
#define BRCM_SWDSGN_MACSEC_DRV_GET_TX_SCSTATS_PROC              (0xB191U) /**< @brief #MACSEC_DrvGetTxSCStats         */
#define BRCM_SWDSGN_MACSEC_DRV_GET_SEC_YSTATS_PROC              (0xB192U) /**< @brief #MACSEC_DrvGetSecYStats         */
#define BRCM_SWDSGN_MACSEC_DRV_SAGET_STATUS_PROC                (0xB193U) /**< @brief #MACSEC_DrvSAGetStatus          */
#define BRCM_SWDSGN_MACSEC_DRV_SASET_NEXT_PN_PROC               (0xB194U) /**< @brief #MACSEC_DrvSASetNextPN          */
#define BRCM_SWDSGN_MACSEC_DRV_SAGET_NEXT_PN_PROC               (0xB195U) /**< @brief #MACSEC_DrvSAGetNextPN          */
#define BRCM_SWDSGN_MACSEC_DRV_DE_INIT_PROC                     (0xB196U) /**< @brief #MACSEC_DrvDeInit               */
#define BRCM_SWDSGN_MACSEC_DRV_DELETE_SEC_Y_PROC                (0xB197U) /**< @brief #MACSEC_DrvDeleteSecY           */
#define BRCM_SWDSGN_MACSEC_DRV_DELETE_SA_PROC                   (0xB198U) /**< @brief #MACSEC_DrvDeleteSA             */
#define BRCM_SWDSGN_MACSEC_DRV_DELETE_RX_SC_PROC                (0xB199U) /**< @brief #MACSEC_DrvDeleteRxSC           */
#define BRCM_SWDSGN_MACSEC_DRV_DELETE_TX_SC_PROC                (0xB19AU) /**< @brief #MACSEC_DrvDeleteTxSC           */
#define BRCM_SWDSGN_MACSEC_DRV_SADISABLE_PROC                   (0xB19BU) /**< @brief #MACSEC_DrvSADisable            */
#define BRCM_SWDSGN_MACSEC_DRV_SAENABLE_PROC                    (0xB19CU) /**< @brief #MACSEC_DrvSAEnable             */
#define BRCM_SWDSGN_MACSEC_DRV_ADD_SEC_YLOOKUP_RULE_PROC        (0xB19DU) /**< @brief #MACSEC_DrvAddSecYLookupRule    */
#define BRCM_SWDSGN_MACSEC_DRV_CREATE_RX_SA_PROC                (0xB19EU) /**< @brief #MACSEC_DrvCreateRxSA           */
#define BRCM_SWDSGN_MACSEC_DRV_SET_PNTHRESHOLD_PROC             (0xB19FU) /**< @brief #MACSEC_DrvSetPNThreshold       */
#define BRCM_SWDSGN_MACSEC_DRV_CREATE_TX_SA_PROC                (0xB1A0U) /**< @brief #MACSEC_DrvCreateTxSA           */
#define BRCM_SWDSGN_MACSEC_DRV_CREATE_RX_SC_PROC                (0xB1A1U) /**< @brief #MACSEC_DrvCreateRxSC           */
#define BRCM_SWDSGN_MACSEC_DRV_CREATE_TX_SC_PROC                (0xB1A2U) /**< @brief #MACSEC_DrvCreateTxSC           */
#define BRCM_SWDSGN_MACSEC_DRV_SET_CIPHER_SUITE_PROC            (0xB1A3U) /**< @brief #MACSEC_DrvSetCipherSuite       */
#define BRCM_SWDSGN_MACSEC_DRV_CREATE_SEC_Y_PROC                (0xB1A4U) /**< @brief #MACSEC_DrvCreateSecY           */
#define BRCM_SWDSGN_MACSEC_DRV_REMOVE_CPRULE_PROC               (0xB1A5U) /**< @brief #MACSEC_DrvRemoveCPRule         */
#define BRCM_SWDSGN_MACSEC_DRV_ADD_CPRULE_OPT1_PROC             (0xB1A6U) /**< @brief #MACSEC_DrvAddCPRuleOpt1        */
#define BRCM_SWDSGN_MACSEC_DRV_ADD_CPRULE_OPT0_PROC             (0xB1A7U) /**< @brief #MACSEC_DrvAddCPRuleOpt0        */
#define BRCM_SWDSGN_MACSEC_DRV_GET_CIPHER_CAP_PROC              (0xB1A8U) /**< @brief #MACSEC_DrvGetCipherCap         */
#define BRCM_SWDSGN_MACSEC_DRV_INIT_PROC                        (0xB1A9U) /**< @brief #MACSEC_DrvInit                 */
#define BRCM_SWDSGN_MACSEC_DRV_SATHRLD_IRQHANDLER_PROC          (0xB1AAU) /**< @brief #MACSEC_DrvSAThrldIRQHandler    */
#define BRCM_SWDSGN_MACSEC_DRV_SAEXPIRED_IRQHANDLER_PROC        (0xB1ABU) /**< @brief #MACSEC_DrvSAExpiredIRQHandler  */
#define BRCM_SWDSGN_MACSEC_DRV_IRQHANDLER_PROC                  (0xB1ACU) /**< @brief #MACSEC_DrvIRQHandler           */

#define BRCM_SWDSGN_MACSEC_I_TCAMKP_OFFSETOF_MACRO              (0xB1AFU) /**< @brief #MACSEC_I_TCAMKP_OFFSET_OF      */
#define BRCM_SWDSGN_MACSEC_SET_BIT_PROC                         (0xB1B0U) /**< @brief #MACSEC_SetBit                  */
#define BRCM_SWDSGN_MACSEC_CLR_N_GET_BIT_PROC                   (0xB1B1U) /**< @brief #MACSEC_ClrNGetBit              */
#define BRCM_SWDSGN_MACSEC_E_BRCM_GLB_OFFSETOF_MACRO            (0xB1B2U) /**< @brief #MACSEC_E_BRCM_GLOBAL_OFFSET_OF */
#define BRCM_SWDSGN_MACSEC_I_BRCM_GLB_OFFSETOF_MACRO            (0xB1B3U) /**< @brief #MACSEC_I_BRCM_GLOBAL_OFFSET_OF */
#define BRCM_SWDSGN_MACSEC_DRV_EN_QTAG_PARSE_PROC               (0xB1B4U) /**< @brief #MACSEC_DrvEnQtagParse          */
#define BRCM_SWDSGN_MACSEC_VLAN_PRI_MAPTBL_PROC                 (0xB1B5U) /**< @brief #MACSEC_VlanPriorityMapTbl      */
#define BRCM_SWDSGN_MACSEC_DRV_FORMAT_FRAMEDATA_PROC            (0xB1B6U) /**< @brief #MACSEC_DrvFormatFrameData      */
#define BRCM_SWDSGN_MACSEC_AIC_LOCAL_EN_CTRL_TYPE               (0xB1B7U) /**< @brief #MACSEC_AicLocalEnCtrlType      */
#define BRCM_SWDSGN_MACSEC_AIC_GLOBAL_EN_CTRL_TYPE              (0xB1B8U) /**< @brief #MACSEC_AicGlobalEnCtrlType     */
#define BRCM_SWDSGN_MACSEC_AIC_GLOBAL_POL_CTRL_TYPE             (0xB1B9U) /**< @brief #MACSEC_AicGlobalPolCtrlType    */
#define BRCM_SWDSGN_MACSEC_EIP_AIC_OFFSET_OF_MACRO              (0xB1BAU) /**< @brief #MACSEC_EIP165_AIC_OFFSET_OF    */
#define BRCM_SWDSGN_MACSEC_E_EIP_AIC_OFFSET_OF_MACRO            (0xB1BBU) /**< @brief #MACSEC_E_EIP160_AIC_OFFSET_OF  */
#define BRCM_SWDSGN_MACSEC_MACSEC_MSPU_TOP_OFFSET_OF_MACRO      (0xB1BCU) /**< @brief #MACSEC_MSPU_TOP_OFFSET_OF      */
#define BRCM_SWDSGN_MACSEC_SYSMISC_REG1_OFFSETOF_MACRO          (0xB1BDU) /**< @brief #MACSEC_SYSMISC_REG1_OFFSET_OF  */
#define BRCM_SWDSGN_MACSEC_DRV_GET_CIPHER_SUITE_PROC            (0xB1C0U) /**< @brief #MACSEC_DrvGetCipherSuite       */
#define BRCM_SWDSGN_MACSEC_DRV_SET_CTRL_PORT_PROC               (0xB1C1U) /**< @brief #MACSEC_DrvCtrlPortSet          */
#define BRCM_SWDSGN_MACSEC_DRV_GET_RULE_HASH_ADDR_PROC          (0xB1C2U) /**< @brief #MACSEC_DrvGetRuleHashAddr      */
#define BRCM_SWDSGN_MACSEC_DRV_GENERATE_RULE_HASH_PROC          (0xB1C3U) /**< @brief #MACSEC_DrvGenerateRuleHash     */
#define BRCM_SWDSGN_MACSEC_DRV_ADD_SECY_LOOKUP_RULE_INTERNAL_PROC (0xB1C4U) /**< @brief #MACSEC_DrvAddSecYLookupRuleInternal */
#define BRCM_SWDSGN_MACSEC_DRV_IS_XPN_CIPHER_SUITE_SET_PROC     (0xB1C5U) /**< @brief #MACSEC_DrvIsXpnCipherSuiteSet   */
#define BRCM_SWDSGN_MACSEC_DRV_GET_STATUS_PROC                  (0xB1C6U) /**< @brief #MACSEC_DrvGetStatus             */
#define BRCM_SWDSGN_MACSEC_SECY_GET_AES_AUTHKEY_PROC            (0xB1C7U) /**< @brief #MACSEC_SECYGetAESAuthKey */
#define BRCM_SWDSGN_MACSEC_INT_DRV_GET_CTXT_PROC                (0xB1C8U) /**< @brief #MACSEC_IntDrvGetCtxt */

/** @} */

/**
    @brief SecY Magic

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_SECYID_MAGIC           (0x53454359ULL)

/**
    @brief Max CP rules allowed for MAC DA-ETH match

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_CP_MACDAETH_MAX_RULES        (8UL)

/**
    @brief Max CP rules allowed MAC DA-ETH Range match

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DEINIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_CP_MACDAETRANGE_MAX_RULES    (2U)

/**
    @brief Maximum secure channels

    Maximum secure channel supported by hardware

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_MAX_SC                   (16UL)

/**
    @brief Maximum TCAM entries

    Max TCAM entries supported by hardware

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_MAX_TCAM_ENTRIES         (32UL)

/**
    @brief Maximum VPORTs supported by hardware

    Maximum number of virtual ports supported
    by hardware

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_MAX_VPORTS               (16UL)

/**
    @brief Maximum transform records

    Maximum number of transform records
    supported by hardware.

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_MAX_XFORM_RECORDS        (32UL)

/**
    @name Transform records max sizes
    @{
    @brief Maximum words in Transform record

    Maximum number of words in transform record
    and size of each record.

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_XREC_WORDS_PER_ENTRY     (24UL)
#define MACSEC_XREC_SIZE_PER_ENTRY      (24UL * 4UL)
/** @} */

/**
    @brief In-Flight unsafe counter

    Timeout counter value for Inflight sync

    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_LOAD_UNSAFE_TIMEOUT_CNT  (1000000UL)

/**
    @brief Replay protection max window size

    Maximum size of the replay protection window
    as defined by the specification.

    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_MAX_REPLAY_WINDOW        (0xFFFFFFFFUL)

/**
    @brief Reset handle information struct

    Clears the handle information memory.

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_HANDLE_INFO_INIT(aInfo)      (BCM_MemSet(&(aInfo), 0xFFU, sizeof(aInfo)))

/**
    @name Converts MAC bytes to 32-bit
    @{
    @brief Helper macro to convert MAC bytes to
    32-bit data.

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_MAC_BYTES_TO_UINT_HI(mac)      \
    ((uint32_t)(mac[0UL]) << 0UL | (uint32_t)(mac[1UL]) << 8UL |\
     (uint32_t)(mac[2UL]) << 16UL | (uint32_t)(mac[3UL]) << 24UL)

#define MACSEC_MAC_BYTES_TO_UINT_LOW(mac)      \
    (((uint32_t)(mac[5UL]) << 8UL) | ((uint32_t)(mac[4]) << 0UL))

/** @} */

/**
    @brief swap bytes in 16-bit data

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_BYTE_SWAPPED(x)     ((((uint32_t)(x) & 0xFFUL) << 8U) | (((uint32_t)(x) >> 8U) & 0xFFUL))

/**
    @brief Converts byte array to 32-bit

    Converts byte array of size 4 to 32-bit data
    in Little endian order.

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_BYTES_TO_LE32(aBytes, aOffset)    \
    (((uint32_t)((aBytes)[(aOffset)]) << 0UL) | ((uint32_t)((aBytes)[(aOffset) + 1UL]) << 8UL) |\
     ((uint32_t)((aBytes)[(aOffset) + 2UL]) << 16UL) | ((uint32_t)((aBytes)[(aOffset) + 3UL]) << 24UL))


/**
    @name MACSec direction
    @{
    @brief MACSec direction type (egress/ingress)

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_DirType;
#define MACSEC_DIR_EGRESS   (1UL)
#define MACSEC_DIR_INGRESS  (2UL)
/** @} */

/**
    @name Packet drop action
    @{
    @brief Drop action types in the hardware

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_DropActionType;     /**< @brief Drop action type */
#define MACSEC_DROP_CRC_ERROR        (0U)   /**< @brief drop frame with CRC error */
#define MACSEC_DROP_FRAME_ERROR      (1U)   /**< @brief drop frame with bad packet error */
#define MACSEC_DROP_INTERNAL         (2U)   /**< @brief internal drop by crypto core */
#define MACSEC_DROP_NA               (3U)   /**< @brief do not drop (for debug only) */
/** @} */

/**
    @name Non-SA action types
    @{
    @brief Non-SA action types

    Action to be taken in the hardware for
    which no SA record is found.

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_NonSAActionType;    /**< @brief Non-matched SA action type */
#define MACSEC_NONSAACTION_BYPASS   (0U)    /**< @brief action bypass frame */
#define MACSEC_NONSAACTION_DROP     (1U)    /**< @brief action drop frame with error */
/** @} */

/**
    @name Ethernet frame types
    @{
    @brief Ethernet frame types

    Types of the ethernet frames defined in
    classification engine.

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_EthFrameType;
#define MACSEC_ETHFRAME_OTHER                   (0U)    /**< @brief 802.3 Ethernet II frame */
#define MACSEC_ETHFRAME_ETH                     (1U)    /**< @brief 802.2 LLC/SNAP frame */
#define MACSEC_ETHFRAME_LLC                     (2U)    /**< @brief 802.2 LLC */
#define MACSEC_ETHFRAME_SNAP                    (3U)    /**< @brief 802.2 SNAP */
/** @} */

/**
    @name Number of tags
    @{
    @brief Number of 802.1Q tags

    Number of 802.1Q tags in the ethernet
    frame.

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_EthNumTagsType;
#define MACSEC_ETHNUMTAGS_0                        (0U)    /**< @brief No VLAN tags and no MPLS labels */
#define MACSEC_ETHNUMTAGS_1                        (1U)    /**< @brief 1 VLAN tag or 1 MPLS label */
#define MACSEC_ETHNUMTAGS_2                        (2U)    /**< @brief 2 VLAN tag or 2 MPLS label */
#define MACSEC_ETHNUMTAGS_3                        (3U)    /**< @brief 3 VLAN tag or 3 MPLS label */
#define MACSEC_ETHNUMTAGS_4                        (4U)    /**< @brief 4 VLAN tag or 4 MPLS label */
#define MACSEC_ETHNUMTAGS_5                        (5U)    /**< @brief 5 MPLS label */
#define MACSEC_ETHNUMTAGS_6                        (6U)    /**< @brief VLAN tags > 4  or MPLS labels > 5  */
/** @} */

/**
    @name SA action types
    @{
    @brief SA action types

    Types of actions taken by hardware for given
    SA record.

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_SAActionType;
#define MACSEC_SAACTION_BYPASS      (0U)    /**< @brief bypass operation  */
#define MACSEC_SAACTION_DROP        (1U)    /**< @brief drop operation */
#define MACSEC_SAACTION_INGRESS     (2U)    /**< @brief ingress operation */
#define MACSEC_SAACTION_EGRESS      (3U)    /**< @brief Egress operation */
/** @} */

/**
    @name Rule types
    @{
    @brief Types of rules in the hardware

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_RuleMaskType;
#define MACSEC_RULEMASK_HDRPARSE    (1UL)
#define MACSEC_RULEMASK_TCAM        (2UL)
/** @} */

/**
   @name MACSEC_XRecW0Type
   @{
   @brief Transform record control word masks and shifts

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_XRecW0Type;
#define MACSEC_XRECW0_TOP_MASK              (0xFUL)
#define MACSEC_XRECW0_TOP_SHIFT             (0UL)
#define MACSEC_XRECW0_IV0_PRESENT_MASK      (0x1UL << 5UL)
#define MACSEC_XRECW0_IV1_PRESENT_MASK      (0x1UL << 6UL)
#define MACSEC_XRECW0_IV2_PRESENT_MASK      (0x1UL << 7UL)
#define MACSEC_XRECW0_KEEP_SECTAG_MASK      (0x1UL << 8UL)
#define MACSEC_XRECW0_KEEP_ICV_MASK         (0x1UL << 9UL)
#define MACSEC_XRECW0_ROLLOVER_MASK         (0x1UL << 10UL)
#define MACSEC_XRECW0_UPDATE_SEQ_MASK       (0x1UL << 13UL)
#define MACSEC_XRECW0_IV_FORMAT_MASK        (0x1UL << 14UL)
#define MACSEC_XRECW0_ENCRYPT_AUTH_MASK     (0x1UL << 15UL)
#define MACSEC_XRECW0_LOAD_KEY_MASK         (0x1UL << 16UL)
#define MACSEC_XRECW0_CRYPTO_ALGO_MASK      (0x7UL << 17UL)
#define MACSEC_XRECW0_CRYPTO_ALGO_SHIFT     (17UL)
#define MACSEC_XRECW0_DIGEST_TYPE_MASK      (0x3UL << 21UL)
#define MACSEC_XRECW0_DIGEST_TYPE_SHIFT     (21UL)
#define MACSEC_XRECW0_AUTH_ALGO_MASK        (0x7UL << 23UL)
#define MACSEC_XRECW0_AUTH_ALGO_SHIFT       (23UL)
#define MACSEC_XRECW0_AN_MASK               (0x3UL << 26UL)
#define MACSEC_XRECW0_AN_SHIFT              (26UL)
#define MACSEC_XRECW0_SEQ_TYPE_MASK         (0x3UL << 28UL)
#define MACSEC_XRECW0_SEQ_TYPE_SHIFT        (28UL)
#define MACSEC_XRECW0_SEQ_MASK              (0x1UL << 30UL)
#define MACSEC_XRECW0_CNTXID_PRESENT_MASK   (0x1UL << 31UL)
#define MACSEC_XRECW0_TOP_EGRESS            (0x6UL)
#define MACSEC_XRECW0_TOP_INGRESS           (0xFUL)
#define MACSEC_XRECW0_CRYPTO_ALGO_AES128    (5UL)
#define MACSEC_XRECW0_CRYPTO_ALGO_AES256    (7UL)
#define MACSEC_XRECW0_AUTH_ALGO_AES_GHASH   (4UL)
#define MACSEC_XRECW0_SEQ_32BIT        (1UL)
#define MACSEC_XRECW0_SEQ_64BIT        (2UL)
/** @} */

/**
   @name MACSEC_XRecIDType
   @{
   @brief Transform record Context ID masks and bits

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_XRecIDType;
#define MACSEC_XREC_ID_SECY_IDX_SHIFT    (0UL)
#define MACSEC_XREC_ID_SECY_IDX_MASK     (0x1FUL)
#define MACSEC_XREC_ID_AN_SHIFT          (5UL)
#define MACSEC_XREC_ID_AN_MASK           (0x7UL << MACSEC_XREC_ID_AN_SHIFT)
#define MACSEC_XREC_ID_MAGIC_SHIFT       (16UL)
#define MACSEC_XREC_ID_MAGIC_MASK        (0xFFFFUL << MACSEC_XREC_ID_MAGIC_SHIFT)
#define MACSEC_XREC_ID_MAGIC             (0x4944UL)   /* 'ID'*/
#define MACSEC_XREC_ID_CREATE(aSecyIdx, aAN) \
    (((((uint32_t)aSecyIdx) << MACSEC_XREC_ID_SECY_IDX_SHIFT) & MACSEC_XREC_ID_SECY_IDX_MASK) |\
    ((((uint32_t)aAN) << MACSEC_XREC_ID_AN_SHIFT) & MACSEC_XREC_ID_AN_MASK) |\
    ((uint32_t)MACSEC_XREC_ID_MAGIC << MACSEC_XREC_ID_MAGIC_SHIFT))
/** @} */

/**
    @name Transform record update control
    @{
    @brief SA update control word

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_XRecSAUpdCtrlType;
#define MACSEC_XRECSAUPDCTRL_SA_INDEX_MASK         (0x1FFFUL)
#define MACSEC_XRECSAUPDCTRL_SA_INDEX_SHIFT        (0x0UL)
#define MACSEC_XRECSAUPDCTRL_UPDATE_TIME_MASK      (0x2000UL)
#define MACSEC_XRECSAUPDCTRL_UPDATE_TIME_SHIFT     (13UL)
#define MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK   (0x4000UL)
#define MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_SHIFT  (14UL)
#define MACSEC_XRECSAUPDCTRL_SA_IDX_VALID_MASK     (0x8000UL)
#define MACSEC_XRECSAUPDCTRL_SA_IDX_VALID_SHIFT    (15UL)
#define MACSEC_XRECSAUPDCTRL_SC_IDX_MASK           (0x7FFF0000UL)
#define MACSEC_XRECSAUPDCTRL_SC_IDX_SHIFT          (16UL)
#define MACSEC_XRECSAUPDCTRL_UPDATE_EN_MASK        (0x80000000UL)
#define MACSEC_XRECSAUPDCTRL_UPDATE_EN_SHIFT       (31UL)
/** @} */

/**
    @name MACSEC_AicEnCtrlType
    @{
    @brief AIC enable control masks (Local: Egress and ingress has its own AIC)

    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_AicLocalEnCtrlType;
#define MACSEC_AIC_LOCAL_EN_CTRL_SA_EXPIRED_MASK     (0x1UL << 17UL)
#define MACSEC_AIC_LOCAL_EN_CTRL_SA_PN_THRLD_MASK    (0x1UL << 16UL)
/** @} */

/**
    @name MACSEC_AicGlobalEnCtrlType
    @{
    @brief AIC enable control mask (Global AIC)

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_AicGlobalEnCtrlType;
#define MACSEC_AIC_GLOBAL_EN_CTRL_INGRESS_MASK       (0x1UL << 9UL)  /**< @brief ingress EIP Global IRQ */
#define MACSEC_AIC_GLOBAL_EN_CTRL_EGRESS_MASK        (0x1UL << 10UL) /**< @brief egress EIP global IRQ  */
/** @} */

/**
    @name MACSEC_AicGlobalPolCtrlType
    @{
    @brief AIC interrupt polarity control (Global AIC)

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef uint32_t MACSEC_AicGlobalPolCtrlType;
#define MACSEC_AIC_GLOBAL_POL_CTRL_INGRESS_MASK  (0x1UL << 9UL)  /**< @brief ingress EIP polarity high */
#define MACSEC_AIC_GLOBAL_POL_CTRL_EGRESS_MASK   (0x1UL << 10UL) /**< @brief egress EIP polarity high */
/** @} */

/**
    @name MACSEC_HandleMaskType
    @{
    @brief Handle masks and shifts

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
/** @} */

/**
    @brief Handler information structure

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_HandleInfoType {
    MACSEC_HwIDType     hwIdx;
    MACSEC_RuleMaskType ruleType;
    uint8_t             ctxIdx;
    uint8_t             secyIdx;
    uint8_t             SAIdx;
    uint8_t             SCIdx;
    uint8_t             AN;
    uint8_t             ruleIdxEgress;
    uint8_t             ruleIdxIngress;
    uint8_t             ruleMask;
    uint8_t             dir;
    uint16_t            hash;
} MACSEC_HandleInfoType;

/**
    @brief TCAM rule structure

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_TCAMRuleType {
    MACSEC_EthFrameType     frameType;
    MACSEC_EthNumTagsType   numTags;
    uint32_t                macSecTagged;
    uint32_t                controlPkt;
    const uint8_t           *macDA;
    const uint8_t           *macSA;
    const uint32_t          *frameData;
    const uint8_t           *matchMacDAMask;
    const uint8_t           *matchMacSAMask;
    const uint32_t          *matchFrameDataMask;
    uint32_t                matchEthFrameType;
    uint32_t                matchNumTags;
    uint32_t                matchTagged;
} MACSEC_TCAMRuleType;

/**
    @brief Ingress/Egress SecY stats struct

    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_EgIgSecYStatsType {
    uint64_t pktsUntagged;
    uint64_t pktsNoTag;
    uint64_t pktsBadTag;
    uint64_t pktsNoSCI;
    uint64_t pktsUnknownSCI;
} MACSEC_EgIgSecYStatsType;

/**
    @brief Egress SA stats struct

    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_EgSAStatsType {
    uint64_t octetsEncryptedProtected;
    uint64_t pktsEncryptedProtected;
    uint64_t pktsTooLong;
    uint64_t pktsSANotInUse;
} MACSEC_EgSAStatsType;

/**
    @brief Egress SA stats struct

    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_IgSAStatsType {
    uint64_t octetsDecrypted;
    uint64_t octetsValidated;
    uint64_t pktsUnchecked;
    uint64_t pktsDelayed;
    uint64_t pktsLate;
    uint64_t pktsOK;
    uint64_t pktsInvalid;
    uint64_t pktsNotValid;
    uint64_t pktsNotUsingSA;
    uint64_t pktsUnusedSA;
} MACSEC_IgSAStatsType;

/**
    @brief SecY cached stats structure

    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_SecYCachedStatsType {
    uint64_t octetsEncryptedProtected;
    uint64_t pktsEncryptedProtected;
    uint64_t pktsTooLong;
    uint64_t inOctetsDecrypted;
    uint64_t inOctetsValidated;
    uint64_t inPktsNotUsingSA;
    uint64_t inPktsUnusedSA;
} MACSEC_SecYCachedStatsType;

/**
    @brief Egress transform record for AES128

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_EgressXRecAES128Type {
    uint32_t ctrl;
    uint32_t id;
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t hKey0;
    uint32_t hKey1;
    uint32_t hKey2;
    uint32_t hKey3;
    uint32_t seq;
    uint32_t iv0;
    uint32_t iv1;
    uint32_t rsvd0;
    uint32_t rsvd1;
    uint32_t saUpdCtrl;
    uint32_t rsvd2;
    uint32_t rsvd3;
    uint32_t rsvd4;
    uint32_t rsvd5;
    uint32_t rsvd6;
    uint32_t rsvd7;
    uint32_t startTime;
    uint32_t stopTime;
} MACSEC_EgressXRecAES128Type;
_Static_assert(sizeof(MACSEC_EgressXRecAES128Type) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");


/**
    @brief Egress transform record for AES-256

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_EgressXRecAES256Type {
    uint32_t ctrl;
    uint32_t id;
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t key4;
    uint32_t key5;
    uint32_t key6;
    uint32_t key7;
    uint32_t hKey0;
    uint32_t hKey1;
    uint32_t hKey2;
    uint32_t hKey3;
    uint32_t seq;
    uint32_t iv0;
    uint32_t iv1;
    uint32_t rsvd0;
    uint32_t rsvd1;
    uint32_t saUpdCtrl;
    uint32_t rsvd2;
    uint32_t rsvd3;
    uint32_t startTime;
    uint32_t stopTime;
} MACSEC_EgressXRecAES256Type;
_Static_assert(sizeof(MACSEC_EgressXRecAES256Type) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief Egress transform record for AES-128-XPN

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_EgressXRecAES128XpnType {
    uint32_t ctrl;
    uint32_t id;
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t hKey0;
    uint32_t hKey1;
    uint32_t hKey2;
    uint32_t hKey3;
    uint32_t seq0;
    uint32_t seq1;
    uint32_t rsvd0;
    uint32_t is0;
    uint32_t is1;
    uint32_t is2;
    uint32_t iv0;
    uint32_t iv1;
    uint32_t rsvd1;
    uint32_t saUpdCtrl;
    uint32_t rsvd3;
    uint32_t rsvd4;
    uint32_t startTime;
    uint32_t stopTime;
} MACSEC_EgressXRecAES128XpnType;
_Static_assert(sizeof(MACSEC_EgressXRecAES128XpnType) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief Egress transform record for AES-256-XPN

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_EgressXRecAES256XpnType {
    uint32_t ctrl;
    uint32_t id;
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t key4;
    uint32_t key5;
    uint32_t key6;
    uint32_t key7;
    uint32_t hKey0;
    uint32_t hKey1;
    uint32_t hKey2;
    uint32_t hKey3;
    uint32_t seq0;
    uint32_t seq1;
    uint32_t saUpdCtrl;
    uint32_t is0;
    uint32_t is1;
    uint32_t is2;
    uint32_t iv0;
    uint32_t iv1;
    uint32_t startTime;
    uint32_t stopTime;
} MACSEC_EgressXRecAES256XpnType;
_Static_assert(sizeof(MACSEC_EgressXRecAES256XpnType) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief Egress transform record

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef union uMACSEC_EgressXRecDescType {
    MACSEC_EgressXRecAES128Type recAes128;
    MACSEC_EgressXRecAES256Type recAes256;
    MACSEC_EgressXRecAES128XpnType recAes128Xpn;
    MACSEC_EgressXRecAES256XpnType recAes256Xpn;
    uint32_t words[MACSEC_XREC_WORDS_PER_ENTRY];
} MACSEC_EgressXRecDescType;
_Static_assert(sizeof(MACSEC_EgressXRecDescType) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief Ingress transform record for AES-128

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_IngressXRecAES128Type {
    uint32_t ctrl;
    uint32_t id;
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t hKey0;
    uint32_t hKey1;
    uint32_t hKey2;
    uint32_t hKey3;
    uint32_t seq;
    uint32_t mask;
    uint32_t iv0;
    uint32_t iv1;
    uint32_t rsvd0;
    uint32_t saUpdCtrl;
    uint32_t rsvd1;
    uint32_t rsvd2;
    uint32_t rsvd3;
    uint32_t rsvd4;
    uint32_t rsvd5;
    uint32_t rsvd6;
    uint32_t startTime;
    uint32_t stopTime;
} MACSEC_IngressXRecAES128Type;
_Static_assert(sizeof(MACSEC_IngressXRecAES128Type) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief Ingress transform record for AES-256

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_IngressXRecAES256Type {
    uint32_t ctrl;
    uint32_t id;
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t key4;
    uint32_t key5;
    uint32_t key6;
    uint32_t key7;
    uint32_t hKey0;
    uint32_t hKey1;
    uint32_t hKey2;
    uint32_t hKey3;
    uint32_t seq;
    uint32_t mask;
    uint32_t iv0;
    uint32_t iv1;
    uint32_t rsvd0;
    uint32_t saUpdCtrl;
    uint32_t rsvd2;
    uint32_t rsvd3;
    uint32_t startTime;
    uint32_t stopTime;
} MACSEC_IngressXRecAES256Type;
_Static_assert(sizeof(MACSEC_IngressXRecAES256Type) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief Ingress transform record for AES-128-XPN

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_IngressXRecAES128XpnType {
    uint32_t ctrl;
    uint32_t id;
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t hKey0;
    uint32_t hKey1;
    uint32_t hKey2;
    uint32_t hKey3;
    uint32_t seq0;
    uint32_t seq1;
    uint32_t mask;
    uint32_t is0;
    uint32_t is1;
    uint32_t is2;
    uint32_t rsvd0;
    uint32_t rsvd1;
    uint32_t rsvd2;
    uint32_t saUpdCtrl;
    uint32_t rsvd3;
    uint32_t rsvd4;
    uint32_t startTime;
    uint32_t stopTime;
} MACSEC_IngressXRecAES128XpnType;
_Static_assert(sizeof(MACSEC_IngressXRecAES128XpnType) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief Ingress transform record for AES-256-XPN

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_IngressXRecAES256XpnType {
    uint32_t ctrl;
    uint32_t id;
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t key4;
    uint32_t key5;
    uint32_t key6;
    uint32_t key7;
    uint32_t hKey0;
    uint32_t hKey1;
    uint32_t hKey2;
    uint32_t hKey3;
    uint32_t seq0;
    uint32_t seq1;
    uint32_t mask;
    uint32_t is0;
    uint32_t is1;
    uint32_t is2;
    uint32_t saUpdCtrl;
    uint32_t rsvd0;
    uint32_t startTime;
    uint32_t stopTime;
} MACSEC_IngressXRecAES256XpnType;
_Static_assert(sizeof(MACSEC_IngressXRecAES256XpnType) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief Ingress transform record

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef union uMACSEC_IngressXRecDescType {
    MACSEC_IngressXRecAES128Type recAes128;
    MACSEC_IngressXRecAES256Type recAes256;
    MACSEC_IngressXRecAES128XpnType recAes128Xpn;
    MACSEC_IngressXRecAES256XpnType recAes256Xpn;
    uint32_t words[MACSEC_XREC_WORDS_PER_ENTRY];
} MACSEC_IngressXRecDescType;
_Static_assert(sizeof(MACSEC_IngressXRecDescType) == MACSEC_XREC_SIZE_PER_ENTRY, "xrec not aligned");

/**
    @brief SA context structure

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_SAContextType {
    uint32_t                        inUse;
    uint32_t                        ssci;
    uint32_t                        createdTime;
    uint32_t                        startTime;
    uint32_t                        stopTime;
    uint32_t                        trafficStartTime;
    uint32_t                        trafficStopTime;
    uint32_t                        thrldExpIntrCnt;
    uint32_t                        expiredIntrCnt;
    uint8_t                         xRecSAUpdCtrlOff;
    uint8_t                         xRecSeq0Offset;
    uint8_t                         xRecSeq1Offset;
    uint8_t                         xRecReplayWindowOff;
    uint32_t                        contextID;
    uint16_t                        saIdx;
    uint16_t                        rsvd;
    uint64_t                        handle;
    uint8_t                         keyID[16U];
} MACSEC_SAContextType;

/**
    @brief Transmit SC context structure

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_TxSCContextType {
    SECY_SCIType                sci;
    uint32_t                    active;
    uint32_t                    transmitting;
    uint32_t                    encodingSA;
    MACSEC_EgSAStatsType        stats;
    MACSEC_SAContextType        sa[MACSEC_AN_MAX + 1UL];
} MACSEC_TxSCContextType;

/**
    @brief Recieve SC context structure

    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_RxSCContextType {
    SECY_SCIType          sci;
    uint32_t                active;
    uint32_t                receiving;
    uint32_t                rxCAMIdx;
    uint32_t                saActiveMask;
    MACSEC_IgSAStatsType    stats;
    MACSEC_SAContextType    sa[MACSEC_AN_MAX + 1UL];
} MACSEC_RxSCContextType;

/**
    @brief SecY context structure

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_SecYContextType {
    BCM_HandleType          handle;
    SECY_CipherType         cipherID;
    uint32_t                enCtrlPort;
    uint8_t                 confOffset;
    uint8_t                 rsvd[3U];
    uint32_t                txVportIdx;
    uint32_t                rxVPortIdx;
    uint32_t                txTcamRules;
    uint32_t                rxTcamRules;
    uint32_t                rxSCAllocMask;
    uint32_t                confProtect;
    uint32_t                protectFrame;
    uint32_t                replayProtect;
    uint32_t                replayWindow;
    SECY_ValidateFrameType validateFrame;
    MACSEC_SecYCachedStatsType stats;
    SECY_CfgType      cfg;
    MACSEC_TxSCContextType  txSC;
    MACSEC_RxSCContextType  rxSC[MACSEC_MAX_RXSC_PER_SECY];
} MACSEC_SecYContextType;

/**
    @brief MACSec context structure

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_ContextType {
    uint32_t                vportAllocMask;
    uint32_t                hwErr;
    uint32_t                saAllocMask;
    uint32_t                rxCAMAllocMask;
    uint32_t                cpDAETRuleAllocMask;
    uint16_t                cpDAETRuleHash[MACSEC_CP_MACDAETH_MAX_RULES];
    uint32_t                cpDAETRangeRuleAllocMask;
    uint16_t                cpDAETRangeRuleHash[MACSEC_CP_MACDAETRANGE_MAX_RULES];
    uint32_t                cpDARangeRuleAllocMask;
    uint16_t                cpDARangeRuleHash;
    uint32_t                cpDAConstRuleAllocMask;
    uint16_t                cpDAConstRuleHash;
    uint32_t                tcamRuleAllocMask;
    uint16_t                tcamRuleHash[MACSEC_MAX_TCAM_ENTRIES];
} MACSEC_ContextType;

/**
    @brief Hardware context structure

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
typedef struct sMACSEC_HwContextType {
    uint32_t                initialized;
    MACSEC_ContextType      egress;
    MACSEC_ContextType      ingress;
    uint32_t                secyAllocMask;
    MACSEC_SecYContextType  secy[MACSEC_MAX_SECY_PER_PORT];
} MACSEC_HwContextType;

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_SYSMISC_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_SYSMISC_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_SYSMISC_REG1_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_SYSMISC_REGS1_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_CLS_CTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_CLS_CTRL_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_CLS_P_CTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_CLS_P_CTRL_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_CLS_CTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_CLS_CTRL_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_CLS_P_CTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_CLS_P_CTRL_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_EIP62_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_EIP62_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_EIP62_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_EIP62_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_MSPU_MAC_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_MSPU_MAC_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_TCAMKM_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_TCAMKM_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_TCAMKM_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_TCAMKM_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_TCAMKP_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_TCAMKP_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_TCAMKP_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_TCAMKP_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CTRL_PORT_SET_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_FCTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_FCTRL_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_FCTRL_EN_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_FCTRL_EN_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_FCTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_FCTRL_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_FCTRL_EN_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_FCTRL_EN_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_TRANSREC_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_TRANSREC_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_TRANSREC_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_TRANSREC_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_SCSA_MAP_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_SCSA_MAP_RDBType, member)

/**

    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_SCSA_MAP_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_SCSA_MAP_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_RXSC_CAMKEY_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_RXSC_CAMKEY_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_RXSC_CAMEN_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_RXSC_CAMEN_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_OPPE_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_OPPE_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_E_SECY_CNTRS_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_SECY_CNTRS_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#define MACSEC_I_SECY_CNTRS_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_SECY_CNTRS_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_E_SA_CNTRS_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_SA_CNTRS_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_I_SA_CNTRS_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_SA_CNTRS_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_E_SA_CNTRS_CTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_SA_CNTRS_CTRL_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_I_SA_CNTRS_CTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_SA_CNTRS_CTRL_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_E_SECY_CNTRS_CTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_SECY_CNTRS_CTRL_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_I_SECY_CNTRS_CTRL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_SECY_CNTRS_CTRL_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_E_BRCM_GLOBAL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_E_BRCM_GLOBAL_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_I_BRCM_GLOBAL_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_I_BRCM_GLOBAL_RDBType, member)


/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_E_EIP160_AIC_OFFSET_OF(member)   (uint32_t) offsetof(MACSEC_E_EIP160_AIC_REGS_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_EIP165_AIC_OFFSET_OF(member)     (uint32_t) offsetof(MACSEC_EIP165_AIC_REGS_RDBType, member)

/**
    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
 */
#define MACSEC_MSPU_TOP_OFFSET_OF(member)  (uint32_t) offsetof(MACSEC_MSPU_TOP_RDBType, member)

/**
    @brief Hardware context structure

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static MACSEC_HwContextType MACSEC_HwCtxt[MACSEC_MAX_PORT_NUM] COMP_SECTION(".bss.crypto.drivers.macsec");

/**
    @brief Handle seed global

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static uint8_t MACSEC_Seed[MACSEC_MAX_PORT_NUM] COMP_SECTION(".bss.crypto.drivers.macsec");

/**
    @brief MacSec VLAN Priority Map Table

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static const uint32_t MACSEC_VlanPriorityMapTbl[8] = {
    [0] = 0UL,
    [1] = 1UL,
    [2] = 2UL,
    [3] = 3UL,
    [4] = 4UL,
    [5] = 5UL,
    [6] = 6UL,
    [7] = 7UL,
};

/**
    @brief Set a bit

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

*/
static inline void MACSEC_SetBit(volatile uint32_t * const aAddr, uint32_t aBitNo)
{
    if(aBitNo < 32UL) {
        *aAddr = *aAddr | (0x1UL << aBitNo);
    }
}

/**
    @brief Clears a bit and returns index

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

*/
static inline uint32_t MACSEC_ClrNGetBit(volatile uint32_t * const aAddr)
{
    uint32_t idx = 0xFFFFFFFFUL;
    uint32_t val = *aAddr;

    if (0UL != val) {
        idx = CPU_BitReverse(val);
        idx = CPU_Clz(idx);
        val &= ~(0x1UL << idx);
        *aAddr = val;
    }
    return idx;
}

/**
    @brief SCI array to 64-bit value

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static inline uint64_t MACSEC_DrvSCIToU64(const SECY_SCIType *const aSCI)
{
    uint64_t sci64;
    uint16_t swapped;

    BCM_MemCpy(&sci64, aSCI->macAddr, 6UL);
    swapped = (uint16_t)MACSEC_BYTE_SWAPPED(aSCI->portNum);
    BCM_MemCpy(((char *)&sci64) + 6UL, &swapped, sizeof(uint16_t));

    return sci64;
}

/**
    @brief Returns SecY context from given handle

    @code{.unparsed}

    uint8_t secyIdx
    uint8_t hwIdx
    uint16_t hash
    MACSEC_SecYContextType *context = NULL

    hwIdx = (aHandle & SECY_HANDLE_HWID_MASK) >> SECY_HANDLE_HWID_SHIFT
    secyIdx = (aHandle & SECY_HANDLE_SECYID_MASK) >> SECY_HANDLE_SECYID_SHIFT
    hash = (aHandle & SECY_HANDLE_HASH_MASK) >> SECY_HANDLE_HASH_SHIFT

    if (hwIdx < MACSEC_MAX_PORT_NUM) && (secyIdx < MACSEC_MAX_SECY_PER_PORT)
        ctxIdx = MACSEC_ConvPort2CtxIdx(hwIdx)
        if MACSEC_HwCtxt[ctxIdx].secy[secyIdx].hash == hash
            context = &MACSEC_HwCtxt[ctxIdx].secy[secyIdx]
    return context

    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static MACSEC_SecYContextType* MACSEC_DrvHandleToSecY(BCM_HandleType aHandle)
{
    uint8_t secyIdx;
    uint8_t hwIdx;
    MACSEC_SecYContextType *context = NULL;

    hwIdx   = (uint8_t)((aHandle & SECY_HANDLE_HWID_MASK  ) >> SECY_HANDLE_HWID_SHIFT  );
    secyIdx = (uint8_t)((aHandle & SECY_HANDLE_SECYID_MASK) >> SECY_HANDLE_SECYID_SHIFT);

    if ((hwIdx < (uint8_t)MACSEC_MAX_PORT_NUM) && (secyIdx < (uint8_t)MACSEC_MAX_SECY_PER_PORT)) {
        uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(hwIdx);
        if (MACSEC_HwCtxt[ctxIdx].secy[secyIdx].handle == aHandle) {
            context = &MACSEC_HwCtxt[ctxIdx].secy[secyIdx];
        }
    }
    return context;
}


/**
   @brief Gets cipher suite

   @code{.unparsed}
    MACSEC_SecYContextType *secy
    int ret = BCM_ERR_INVAL_PARAMS
    if aCipherID and aOffset is not NULL
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if secy is not NULL
             *aCipherID = secy->cipherID;
             *aOffset = secy->confOffset;
    return ret
   @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_GETCIPHERCAP_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
   */
static int32_t MACSEC_DrvGetCipherSuite(MACSEC_SecYHdlType aSecYHdl, SECY_CipherType* aCipherID,
                                        SECY_ConfOffsetType* aOffset) {
     MACSEC_SecYContextType *secy;
     int ret = BCM_ERR_INVAL_PARAMS;

     if ((NULL != aCipherID) && (NULL != aOffset)) {
         secy = MACSEC_DrvHandleToSecY(aSecYHdl);
         if (NULL != secy) {
             *aCipherID = secy->cipherID;
             *aOffset = secy->confOffset;
             ret = BCM_ERR_OK;
         }
     }

     return ret;
}

/**
    Helper API to generate the AES Authentication Key

    This API uses AES algorithm to generate new Key
    -# aHkey = AES(aKey,0)

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

    @code{.unparsed}
    - Get Cipher Suite based on aSecyHdl
    - Use Cipher and aKey to generate aHkey
    @endcode

*/
static int32_t MACSEC_SECYGetAESAuthKey(BCM_HandleType aHdl, BCM_HandleType aSecyHdl, const uint8_t *const aKey, uint8_t *const aHKey)
{
    int32_t ret;
    SECY_CipherType     cipherID = 0ULL;
    SECY_ConfOffsetType offset;
    uint8_t zeroText[16UL];
    CRYPTO_KeyType keyType = CRYPTO_KEY_AES128;

    ret = MACSEC_DrvGetCipherSuite(aSecyHdl, &cipherID, &offset);
    if (BCM_ERR_OK == ret) {
        switch (cipherID) {
            case SECY_CIPHER_GCM_AES_128:
            case SECY_CIPHER_GCM_AES_XPN_128:
                keyType = CRYPTO_KEY_AES128;
            break;
            case SECY_CIPHER_GCM_AES_256:
            case SECY_CIPHER_GCM_AES_XPN_256:
                keyType = CRYPTO_KEY_AES256;
            break;
            default:
                ret = BCM_ERR_NOSUPPORT;
            break;
        }
        if (BCM_ERR_OK == ret) {
            BCM_MemSet(zeroText, 0x0U, sizeof(zeroText));
            ret = AES_ECBEncryptBlock(keyType, aKey, zeroText, aHKey);
        }
    }

    return ret;
}


/**
    @brief Create handle

    Function to create 64-bit handle with given information

    @code{.unparsed}
    BCM_HandleType handle

    handle = (aInfo->hwIdx << SECY_HANDLE_HWID_SHIFT) & SECY_HANDLE_HWID_MASK
    handle |= (aInfo->secyIdx << SECY_HANDLE_SECYID_SHIFT) & SECY_HANDLE_SECYID_MASK
    handle |= (aInfo->SAIdx << SECY_HANDLE_SAID_SHIFT)  & SECY_HANDLE_SAID_MASK
    handle |= (aInfo->AN << SECY_HANDLE_AN_SHIFT) & SECY_HANDLE_AN_MASK
    handle |= (aInfo->SCIdx << SECY_HANDLE_SCID_SHIFT) & SECY_HANDLE_SCID_MASK
    handle |= ((uint64_t)aInfo->ruleIdxEgress << SECY_HANDLE_RULEID_EGRESS_SHIFT) & SECY_HANDLE_RULEID_EGRESS_MASK
    handle |= ((uint64_t)aInfo->ruleIdxIngress << SECY_HANDLE_RULEID_INGRESS_SHIFT) & SECY_HANDLE_RULEID_INGRESS_MASK
    handle |= (aInfo->ruleMask << SECY_HANDLE_RULEMATCH_SHIFT) & SECY_HANDLE_RULEMATCH_MASK
    handle |= (aInfo->ruleType << SECY_HANDLE_RULE_TYPE_SHIFT) & SECY_HANDLE_RULE_TYPE_MASK
    handle |= (aInfo->dir << SECY_HANDLE_DIR_SHIFT) & SECY_HANDLE_DIR_MASK
    MACSEC_Seed[aInfo->hwIdx]++
    handle |= (MACSEC_Seed[aInfo->hwIdx] << SECY_HANDLE_SEED_SHIFT) & SECY_HANDLE_SEED_MASK
    return handle
   @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static BCM_HandleType MACSEC_DrvCreateHandle(const MACSEC_HandleInfoType *const aInfo)
{
    BCM_HandleType handle;

    handle = (((uint64_t)aInfo->hwIdx) << SECY_HANDLE_HWID_SHIFT) & SECY_HANDLE_HWID_MASK;
    handle |= ((uint64_t)aInfo->secyIdx << SECY_HANDLE_SECYID_SHIFT) & SECY_HANDLE_SECYID_MASK;
    handle |= ((uint64_t)aInfo->SAIdx << SECY_HANDLE_SAID_SHIFT)  & SECY_HANDLE_SAID_MASK;
    handle |= ((uint64_t)aInfo->AN << SECY_HANDLE_AN_SHIFT) & SECY_HANDLE_AN_MASK;
    handle |= ((uint64_t)aInfo->SCIdx << SECY_HANDLE_SCID_SHIFT) & SECY_HANDLE_SCID_MASK;
    handle |= ((uint64_t)aInfo->ruleIdxEgress << SECY_HANDLE_RULEID_EGRESS_SHIFT) & SECY_HANDLE_RULEID_EGRESS_MASK;
    handle |= ((uint64_t)aInfo->ruleIdxIngress << SECY_HANDLE_RULEID_INGRESS_SHIFT) & SECY_HANDLE_RULEID_INGRESS_MASK;
    handle |= ((uint64_t)aInfo->ruleMask << SECY_HANDLE_RULEMATCH_SHIFT) & SECY_HANDLE_RULEMATCH_MASK;
    handle |= ((uint64_t)aInfo->ruleType << SECY_HANDLE_RULE_TYPE_SHIFT) & SECY_HANDLE_RULE_TYPE_MASK;
    handle |= ((uint64_t)aInfo->dir << SECY_HANDLE_DIR_SHIFT) & SECY_HANDLE_DIR_MASK;
    if(MACSEC_Seed[aInfo->hwIdx] < 0xFFU) {
        MACSEC_Seed[aInfo->hwIdx]++;
    }
    handle |= ((uint64_t)MACSEC_Seed[aInfo->hwIdx] << SECY_HANDLE_SEED_SHIFT) & SECY_HANDLE_SEED_MASK;
    return handle;
}

/**
    @brief Update handle hash value

    @code{.unparsed}

    *aHandle |= (aHash << SECY_HANDLE_HASH_SHIFT) & SECY_HANDLE_HASH_MASK;

    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvHandleUpdateHash(BCM_HandleType *const aHandle, uint16_t aHash)
{
    *aHandle |= ((uint64_t)aHash << SECY_HANDLE_HASH_SHIFT) & SECY_HANDLE_HASH_MASK;
}

/**
    @brief Get handle information

    @code{.unparsed}

    ret = BCM_ERR_INVAL_PARAMS

    aInfo->hwIdx = (aHandle & SECY_HANDLE_HWID_MASK) >> SECY_HANDLE_HWID_SHIFT
    if aInfo->hwIdx < MACSEC_MAX_PORT_NUM
        aInfo->ctxIdx = MACSEC_ConvPort2CtxIdx(aInfo->hwIdx)
        if (uint32_t)BCM_BOOL_TRUE == MACSEC_HwCtxt[aInfo->ctxIdx].initialized
            aInfo->secyIdx = (aHandle & SECY_HANDLE_SECYID_MASK) >> SECY_HANDLE_SECYID_SHIFT
            aInfo->ruleIdxEgress = (aHandle & SECY_HANDLE_RULEID_EGRESS_MASK) >> SECY_HANDLE_RULEID_EGRESS_SHIFT
            aInfo->ruleIdxIngress = (aHandle & SECY_HANDLE_RULEID_INGRESS_MASK) >> SECY_HANDLE_RULEID_INGRESS_SHIFT
            aInfo->SAIdx = (aHandle & SECY_HANDLE_SAID_MASK) >> SECY_HANDLE_SAID_SHIFT
            aInfo->SCIdx = (aHandle & SECY_HANDLE_SCID_MASK) >> SECY_HANDLE_SCID_SHIFT
            aInfo->AN = (aHandle & SECY_HANDLE_AN_MASK) >> SECY_HANDLE_AN_SHIFT
            aInfo->ruleMask = (aHandle & SECY_HANDLE_RULEMATCH_MASK) >>
                               SECY_HANDLE_RULEMATCH_SHIFT
            aInfo->ruleType = aHandle & SECY_HANDLE_RULE_TYPE_MASK >>
                                SECY_HANDLE_RULE_TYPE_SHIFT
            aInfo->dir = (aHandle & SECY_HANDLE_DIR_MASK) >> SECY_HANDLE_DIR_SHIFT
            aInfo->hash = (aHandle & SECY_HANDLE_HASH_MASK) >> SECY_HANDLE_HASH_SHIFT
            ret = BCM_ERR_OK
        else
            ret = BCM_ERR_UNINIT
    return ret

    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static int32_t MACSEC_DrvGetHandleInfo(BCM_HandleType aHandle, MACSEC_HandleInfoType *const aInfo)
{
    int32_t  ret = BCM_ERR_INVAL_PARAMS;
    uint8_t  handle8 = (uint8_t) (aHandle & 0xFFULL);
    uint32_t infoHwIdx;

    infoHwIdx = aInfo->hwIdx = (uint32_t)((aHandle & SECY_HANDLE_HWID_MASK) >> SECY_HANDLE_HWID_SHIFT);
    if (infoHwIdx < MACSEC_MAX_PORT_NUM) {   /* valid hwID */
        uint8_t infoCtxIdx = aInfo->ctxIdx = (uint8_t)(MACSEC_ConvPort2CtxIdx(infoHwIdx) & 0xFFUL);  /* retrieve the Context Index */
        if ((uint32_t)BCM_BOOL_TRUE == MACSEC_HwCtxt[infoCtxIdx].initialized) {
            aInfo->secyIdx        =  (uint8_t)((aHandle & SECY_HANDLE_SECYID_MASK) >> SECY_HANDLE_SECYID_SHIFT);                 /* bit[36:32] */
            aInfo->ruleIdxEgress  =  (uint8_t)((aHandle & SECY_HANDLE_RULEID_EGRESS_MASK) >> SECY_HANDLE_RULEID_EGRESS_SHIFT);   /* bit[11:08] */
            aInfo->ruleIdxIngress =  (uint8_t)((aHandle & SECY_HANDLE_RULEID_INGRESS_MASK) >> SECY_HANDLE_RULEID_INGRESS_SHIFT); /* bit[15:12] */
            aInfo->SAIdx          =  (uint8_t)((aHandle & SECY_HANDLE_SAID_MASK) >> SECY_HANDLE_SAID_SHIFT);  /* bit[20:16] */
            aInfo->SCIdx          =  (uint8_t)((aHandle & SECY_HANDLE_SCID_MASK) >> SECY_HANDLE_SCID_SHIFT);  /* bit[25:21] */
            aInfo->AN             =  (uint8_t)((aHandle & SECY_HANDLE_AN_MASK) >> SECY_HANDLE_AN_SHIFT);      /* bit[28:26] */
            aInfo->ruleMask       =           ((handle8 & SECY_HANDLE_RULEMATCH_MASK8) >> SECY_HANDLE_RULEMATCH_SHIFT); /* bit[7:4] */
            aInfo->ruleType       = (uint32_t)((aHandle & SECY_HANDLE_RULE_TYPE_MASK ) >> SECY_HANDLE_RULE_TYPE_SHIFT); /* bit[3:2] */
            aInfo->dir            =           ((handle8 & SECY_HANDLE_DIR_MASK8) >> SECY_HANDLE_DIR_SHIFT);             /* bit[1:0] */
            aInfo->hash           =  (uint16_t)((aHandle & SECY_HANDLE_HASH_MASK) >> SECY_HANDLE_HASH_SHIFT);  /* bit[63:48] */
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_ERR_UNINIT;   /* the context is not initialized */
        }
    }

    return ret;
}

/**
    @brief Returns SA context from given SA handle

    @code{.unparsed}

    MACSEC_HandleInfoType info
    MACSEC_SecYContextType *secy
    MACSEC_SAContextType *sa = NULL

    MACSEC_HANDLE_INFO_INIT(info)

    if MACSEC_DrvGetHandleInfo(aHandle, &info) == BCM_ERR_OK
        if (info.hwIdx < MACSEC_MAX_PORT_NUM) && (info.secyIdx < MACSEC_MAX_SECY_PER_PORT)
            && (info.SAIdx < MACSEC_MAX_XFORM_RECORDS) && (info.AN <= MACSEC_AN_MAX)
                 secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx]
                 if MACSEC_DIR_EGRESS == info.dir
                     if secy->txSC.sa[info.AN].handle == aHandle
                         sa = &secy->txSC.sa[info.AN]
                 else
                     if secy->rxSC[info.SCIdx].sa[info.AN].handle == aHandle
                         sa = &secy->rxSC[info.SCIdx].sa[info.AN]
    return sa
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static MACSEC_SAContextType* MACSEC_DrvHandleToSA(BCM_HandleType aHandle)
{
    MACSEC_HandleInfoType   info;
    MACSEC_SecYContextType *secy;
    MACSEC_SAContextType   *sa = NULL;

    MACSEC_HANDLE_INFO_INIT(info);
    if (MACSEC_DrvGetHandleInfo(aHandle, &info) == BCM_ERR_OK) {
        uint8_t infoAN = info.AN;
        if ((info.hwIdx < (uint32_t)MACSEC_MAX_PORT_NUM) && (info.secyIdx < (uint8_t)MACSEC_MAX_SECY_PER_PORT)
              && (info.SAIdx < MACSEC_MAX_XFORM_RECORDS) && (infoAN <= MACSEC_AN_MAX)) {
                 secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx];
                if (MACSEC_DIR_EGRESS == info.dir) {     /*  Egress direction      */
                    MACSEC_SAContextType *saEgress = &secy->txSC.sa[infoAN];
                    if ( saEgress->handle == aHandle) {  /*  handle ID hit         */
                        sa = saEgress;                   /*  update the  egress SA */
                    }
                } else {                                 /* Ingress direction      */
                    MACSEC_SAContextType *saIngress = &secy->rxSC[info.SCIdx].sa[infoAN];
                    if (saIngress->handle == aHandle) {  /*  handle ID hit         */
                        sa = saIngress;                  /*  update the ingress SA */
                    }
                }
        }
    }
    return sa;
}

/**
    @brief Copy key to transform record memory

    @code{.unparsed}
    uint32_t i, j
    uint32_t word
    for i = 0UL; i < aSize / 4UL; i++
        word = 0UL
        for j = 0; j < 4; j++
            word |= (uint32_t)aSrc[i  * 4UL + j] << 8UL * j
        aDest[i] = word
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvCopyKeyToRec(uint32_t *aDest, const uint8_t *const aSrc, uint32_t aSize)
{
    uint32_t i, j;
    uint32_t word;

    for (i = 0UL; i < (aSize / 4UL); i++) {
        word = 0UL;
        for (j = 0UL; j < 4UL; j++) {
            word |= (uint32_t)aSrc[i * 4UL + j] << 8UL * j;
        }
        aDest[i] = word;
    }
}

/**
    @brief Update the transform record memory

    @code{.unparsed}
    uint32_t i
    uint32_t blkSz = sizeof(MACSEC_E_XFORM_REGS[aID]->xform_rec0_w) / MACSEC_MAX_XFORM_RECORDS
    uint32_t offset = (blkSz * aSAIdx) / 4UL

    offset += aRecStartIdx

    if MACSEC_DIR_EGRESS == aDir
        for i = 0; i < aNumWords; i++
            MACSEC_E_XFORM_REGS[aID]->xform_rec0_w[offset + i] = aData[i];
    else
        for i = 0; i < aNumWords; i++
            MACSEC_I_XFORM_REGS[aID]->xform_rec0_w[offset + i] = aData[i];

    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvXRecUpdate(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint32_t aSAIdx,
                                 uint32_t aRecStartIdx,
                                 uint32_t aNumWords,
                                 uint32_t *const aData,
                                 MACSEC_DirType aDir)
{
    uint32_t i;

    MACSEC_E_TRANSREC_RDBType *eXFORM;
    /* @event         CERT EXP44-C
            @issue            Operand to sizeof has side effect accessing volatile object
            @risk              No risk
            @justification   Sizeof is calculating size of array and it is static object */
    /* coverity[cert_exp44_c_violation] */
    uint32_t blkSz = sizeof(eXFORM->xform_rec0_w) / MACSEC_MAX_XFORM_RECORDS;
    /* @event            CERT INT30-C
            @issue            Coverity infers that (blkSz * aSAIdx) can wrap
            @risk             No risk
            @justification    It will be within range. */
    /* coverity[cert_int30_c_violation] */
    uint32_t offset = (blkSz * aSAIdx) / 4UL;
    if((BCM_UINT32_MAX - offset) > aRecStartIdx) {
        offset += aRecStartIdx;
    }

    /* @event         Coverity, IDENTICAL_BRANCHES
           @issue         Identical code for different branches
           @risk          The same code is executed regardless of whether condition is true, because the 'then' and 'else' branches are identical.
           @justification Coverity issue reported despite the branches being non-identical */
    /* coverity[identical_branches] */
    if (MACSEC_DIR_EGRESS == aDir) {
        for (i = 0; i < aNumWords; i++) {
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TRANSREC_OFFSET_OF(xform_rec0_w[offset + i]), aData[i]);
        }
    } else {
        for (i = 0; i < aNumWords; i++) {
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TRANSREC_OFFSET_OF(xform_rec0_w[offset + i]), aData[i]);
        }
    }
    /* Unused coverity voilation. */
    (void)offset;
}

/**
    @brief Transform record read function

    @code{.unparsed}
    uint32_t i
    volatile uint32_t *recAddr

    if MACSEC_DIR_EGRESS == aDir
        recAddr = &MACSEC_E_XFORM_REGS[aID]->xform_regs[aSAIdx].rec0_w0
    else
        recAddr = &MACSEC_I_XFORM_REGS[aID]->xform_regs[aSAIdx].rec0_w0

    uint32_t i
    uint32_t blkSz = sizeof(MACSEC_E_XFORM_REGS[aID]->xform_rec0_w) / MACSEC_MAX_XFORM_RECORDS
    uint32_t offset = (blkSz * aSAIdx) / 4UL

    offset += aRecStartIdx

    if MACSEC_DIR_EGRESS == aDir
        for i = 0; i < aNumWords; i++
            aData[i] = MACSEC_E_XFORM_REGS[aID]->xform_rec0_w[offset + i];
    else
        for i = 0; i < aNumWords; i++
            aData[i] = MACSEC_I_XFORM_REGS[aID]->xform_rec0_w[offset + i];
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvXRecRead(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint32_t aSAIdx,
                               uint32_t aRecStartIdx,
                               uint32_t aNumWords,
                               uint32_t *const aData,
                               MACSEC_DirType aDir)
{
    uint32_t i;

    MACSEC_E_TRANSREC_RDBType *eTRANSREC;
    /* @event         CERT EXP44-C
            @issue            Operand to sizeof has side effect accessing volatile object
            @risk              No risk
            @justification   Sizeof is calculating size of array and it is static object */
    /* coverity[cert_exp44_c_violation] */
    uint32_t blkSz = sizeof(eTRANSREC->xform_rec0_w) / MACSEC_MAX_XFORM_RECORDS;
    /* @event            CERT INT30-C
            @issue            Coverity infers that (blkSz * aSAIdx) can wrap
            @risk             No risk
            @justification    It will be within range. */
    /* coverity[cert_int30_c_violation] */
    uint32_t offset = (blkSz * aSAIdx) / 4UL;
    if((BCM_UINT32_MAX - offset) > aRecStartIdx) {
        offset += aRecStartIdx;
    }

    /* @event         Coverity, IDENTICAL_BRANCHES
       @issue         Identical code for different branches
       @risk          The same code is executed regardless of whether condition is true, because the 'then' and 'else' branches are identical.
       @justification Coverity issue reported despite the branches being non-identical */
    /* coverity[identical_branches] */
    if (MACSEC_DIR_EGRESS == aDir) {
        for (i = 0; i < aNumWords; i++) {
            aData[i] = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TRANSREC_OFFSET_OF(xform_rec0_w[offset + i]));
        }
    } else {
        for (i = 0; i < aNumWords; i++) {
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            aData[i] = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TRANSREC_OFFSET_OF(xform_rec0_w[offset + i]));
        }
    }
    /* Unused coverity voilation. */
    (void)offset;
}

/**
    @brief In-Flight sync

    @code{.unparsed}
    uint32_t inFlight
    int32_t ret
    uint32_t i

    MACSEC_E_FCTRL_EN_REGS[aID]->in_flight |=
        MACSEC_E_FCTRL_EN_SAM_IN_FLIGHT_LOAD_UNSAFE_MASK
    for i = 0UL; i < MACSEC_LOAD_UNSAFE_TIMEOUT_CNT; i++
        inFlight = MACSEC_E_FCTRL_EN_REGS[aID]->in_flight
        if inFlight & MACSEC_E_FCTRL_EN_SAM_IN_FLIGHT_UNSAFE_MASK == 0UL
           break
    if i < MACSEC_LOAD_UNSAFE_TIMEOUT_CNT
        ret = BCM_ERR_OK
    else
        ret = BCM_ERR_TIME_OUT
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static int32_t MACSEC_DrvInFlightSync(BCM_HandleType aConnHdl, MACSEC_HwIDType aID)
{
    uint32_t inFlight;
    int32_t ret;
    uint32_t i;
    uint32_t waitCnt = MACSEC_LOAD_UNSAFE_TIMEOUT_CNT;

    MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_EN_OFFSET_OF(in_flight), MACSEC_E_FCTRL_EN_SAM_IN_FLIGHT_LOAD_UNSAFE_MASK);
    for (i = 0UL; i < waitCnt; i++) {
        inFlight = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_EN_OFFSET_OF(in_flight));
        if ((inFlight & MACSEC_E_FCTRL_EN_SAM_IN_FLIGHT_UNSAFE_MASK) == 0UL) {
            break;
        }
    }
    if (i < waitCnt) {
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_TIME_OUT;
    }
    return ret;
}

/**
    @brief Format frame data as per Hardware

    @code{.unparsed}
    if enTagMatch is TRUE
        vlan = *aFrameData & 0xFFFF;
        vlanMask = *aMask & 0xFFFF;
        ethType = (*aFrameData & 0xFFFF0000) >> 16;
        ethTypeMask = (*aMask & 0xFFFF0000) >> 16;
    else
        ethType = *aFrameData & 0xFFFF;
        ethTypeMask = *aMask & 0xFFFF;
        vlan = (*aFrameData & 0xFFFF0000) >> 16;
        vlanMask = (*aMask & 0xFFFF0000) >> 16;

    *aFrameData = ethType | (vlan << 16);
    *aMask = ethTypeMask | (vlanMask << 16);
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvFormatFrameData(uint32_t enTagMatch, uint32_t* aFrameData,  uint32_t* aMask)
{
    uint32_t ethType     =  0UL;
    uint32_t ethTypeMask =  0UL;
    uint32_t vlan        =  0UL;
    uint32_t vlanMask    =  0UL;
    uint32_t frmLo       =  *aFrameData & 0x0000FFFFUL;         /* lower 16 bits of aFrameData */
    uint32_t mskLo       =  *aMask      & 0x0000FFFFUL;         /* lower 16 bits of aMask      */
    uint32_t frmHi       = (*aFrameData & 0xFFFF0000UL) >> 16U; /* upper 16 bits of aFrameData */
    uint32_t mskHi       = (*aMask      & 0xFFFF0000UL) >> 16U; /* upper 16 bits of aMask      */

    if ((uint32_t)BCM_BOOL_TRUE == enTagMatch) {
        vlan        =  frmLo;  /*  vlan   use the lower 16 bits of aFrameData */
        vlanMask    =  mskLo;
        ethType     =  frmHi;  /* ethType use the upper 16 bits of aFrameData */
        ethTypeMask =  mskHi;
    } else {
        ethType     =  frmLo;  /* ethType use the lower 16 bits of aFrameData */
        ethTypeMask =  mskLo;
        vlan        =  frmHi;  /*  vlan   use the upper 16 bits of aFrameData */
        vlanMask    =  mskHi;
    }

    *aFrameData = ethType | (vlan     << 16U);
    *aMask =  ethTypeMask | (vlanMask << 16U);
}

/**
    @brief Add a TCAM rule in the hardware

    @code{.unparsed}

    uint32_t key[6UL] = {0UL}
    uint32_t mask[6UL] = {0UL}
    uint32_t keyPolicy = 0UL

    if aRule->matchNumTags == (uint32_t)BCM_BOOL_TRUE
        key[0UL] |= aRule->numTags << MACSEC_E_TCAMKM_TCAM_KEY0_0_NUM_TAGS_SHIFT
        mask[0UL] |= MACSEC_E_TCAMKM_TCAM_KEY0_0_NUM_TAGS_MASK
    if aRule->matchEthFrameType == (uint32_t)BCM_BOOL_TRUE
        key[0UL] |= aRule->frameType << MACSEC_E_TCAMKM_TCAM_KEY0_0_FRAME_TYPE_SHIFT
        mask[0UL] |= MACSEC_E_TCAMKM_TCAM_KEY0_0_FRAME_TYPE_MASK
    if aRule->matchTagged == (uint32_t)BCM_BOOL_TRUE
        mask[0UL] |= MACSEC_E_TCAMKM_TCAM_KEY0_0_MACSEC_TAGGED_MASK
        key[0UL] |= aRule->macSecTagged << MACSEC_E_TCAMKM_TCAM_KEY0_0_MACSEC_TAGGED_SHIFT

    key[1UL] = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDA);
    key[2UL] |= MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDA);
    key[2UL] |= aRule->macSA[0UL] << 16UL
    key[2UL] |= aRule->macSA[1UL] << 24UL

    key[3UL] |= aRule->macSA[2UL] << 0UL
    key[3UL] |= aRule->macSA[3UL] << 8UL
    key[3UL] |= aRule->macSA[4UL] << 16UL
    key[3UL] |= aRule->macSA[5UL] << 24UL

    data = aRule->frameData[0UL];
    dataMask = aRule->matchFrameDataMask[0UL];
    MACSEC_DrvFormatFrameData(aRule->matchNumTags, &data, &dataMask)

    key[4UL] |= data & MACSEC_E_TCAMKM_TCAM_KEY4_0_PACKET_DATA_LO_MASK
    mask[4UL] |= (uint32_t) dataMask &
                    MACSEC_E_TCAMKM_TCAM_KEY4_0_PACKET_DATA_LO_MASK

    key[5UL] |= aRule->frameData[1UL] & MACSEC_E_TCAMKM_TCAM_KEY5_0_PACKET_DATA_HI_MASK
    mask[5UL] |= aRule->matchFrameDataMask[1UL] &
                    MACSEC_E_TCAMKM_TCAM_KEY5_0_PACKET_DATA_HI_MASK

    mask[1UL] |= MACSEC_MAC_BYTES_TO_UINT_HI(aRule->matchMacDAMask)
    mask[2UL] |= MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->matchMacDAMask)

    mask[2UL] |= aRule->matchMacSAMask[0UL] << 16UL
    mask[2UL] |= aRule->matchMacSAMask[1UL] << 24UL

    mask[3UL] |= aRule->matchMacSAMask[2UL] << 0UL
    mask[3UL] |= aRule->matchMacSAMask[3UL] << 8UL
    mask[3UL] |= Rule->matchMacSAMask[4UL] << 16UL
    mask[3UL] |= aRule->matchMacSAMask[5UL] << 24UL

    keyPolicy |= aVportIdx << MACSEC_E_TCAMKP_TCAM_POLICY_0_VPORT_INDEX_SHIFT
    keyPolicy |= aRule->controlPkt << MACSEC_E_TCAMKP_TCAM_POLICY_0_CONTROL_PKT_SHIFT

    if MACSEC_DIR_EGRESS == aDir
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].key0_0 = key[0U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].key1_0 = key[1U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].key2_0 = key[2U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].key3_0 = key[3U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].key4_0 = key[4U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].key5_0 = key[5U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].mask0_0 = mask[0U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].mask1_0 = mask[1U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].mask2_0 = mask[2U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].mask3_0 = mask[3U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].mask4_0 = mask[4U]
        MACSEC_E_TCAMKM_REGS[aID]->km_regs[aIdx].mask5_0 = mask[5U]
        MACSEC_E_TCAMKP_REGS[aID]->tcam_policy[aIdx] = keyPolicy
    else
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].key0_0 = key[0U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].key1_0 = key[1U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].key2_0 = key[2U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].key3_0 = key[3U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].key4_0 = key[4U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].key5_0 = key[5U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].mask0_0 = mask[0U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].mask1_0 = mask[1U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].mask2_0 = mask[2U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].mask3_0 = mask[3U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].mask4_0 = mask[4U]
        MACSEC_I_TCAMKM_REGS[aID]->km_regs[aIdx].mask5_0 = mask[5U]
        MACSEC_I_TCAMKP_REGS[aID]->tcam_policy[aIdx] = keyPolicy

    if MACSEC_DIR_EGRESS == aDir
        MACSEC_E_FCTRL_EN_REGS[aID]->entry_enable_ctrl = (aIdx <<
            MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_SET_SHIFT) |
            MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SET_ENABLE_MASK
    else
        MACSEC_I_FCTRL_EN_REGS[aID]->entry_enable_ctrl = (aIdx <<
            MACSEC_I_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_SET_SHIFT) |
            MACSEC_I_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SET_ENABLE_MASK
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvAddTCAMRule(BCM_HandleType aConnHdl, MACSEC_HwIDType aID,
                                 const MACSEC_TCAMRuleType *const aRule, uint32_t aIdx,
                                 uint32_t aVportIdx,
                                 MACSEC_DirType aDir)
{
    uint32_t key[6UL] = {0UL};
    uint32_t mask[6UL] = {0UL};
    uint32_t keyPolicy = 0UL;
    uint32_t data = 0UL;
    uint32_t dataMask = 0UL;

    /* TCAM Key0: num_tags, packet_type, frame_type, macsec_tagged*/
    if (aRule->matchNumTags == (uint32_t)BCM_BOOL_TRUE) {
        key[0UL] |= aRule->numTags << MACSEC_E_TCAMKM_TCAM_KEY0_0_NUM_TAGS_SHIFT;
        mask[0UL] |= MACSEC_E_TCAMKM_TCAM_KEY0_0_NUM_TAGS_MASK;
    }
    if (aRule->matchEthFrameType == (uint32_t)BCM_BOOL_TRUE) {
        key[0UL] |= aRule->frameType << MACSEC_E_TCAMKM_TCAM_KEY0_0_FRAME_TYPE_SHIFT;
        mask[0UL] |= MACSEC_E_TCAMKM_TCAM_KEY0_0_FRAME_TYPE_MASK;
    }
    if (aRule->matchTagged == (uint32_t)BCM_BOOL_TRUE) {
        mask[0UL] |= MACSEC_E_TCAMKM_TCAM_KEY0_0_MACSEC_TAGGED_MASK;
        key[0UL] |= aRule->macSecTagged << MACSEC_E_TCAMKM_TCAM_KEY0_0_MACSEC_TAGGED_SHIFT;
    }

    /* TCAM key1: mac_da_31_0 */
    key[1UL] = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDA);
    /* TCAM key 2: macda[47:32] and macsa[15:0] */
    key[2UL] |= MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDA);
    key[2UL] |= (uint32_t)aRule->macSA[0UL] << 16UL;
    key[2UL] |= (uint32_t)aRule->macSA[1UL] << 24UL;

    /* key 3: macsa[47:16] */
    key[3UL] |= (uint32_t)aRule->macSA[2UL] << 0UL;
    key[3UL] |= (uint32_t)aRule->macSA[3UL] << 8UL;
    key[3UL] |= (uint32_t)aRule->macSA[4UL] << 16UL;
    key[3UL] |= (uint32_t)aRule->macSA[5UL] << 24UL;

    data = aRule->frameData[0UL];
    dataMask = aRule->matchFrameDataMask[0UL];
    MACSEC_DrvFormatFrameData(aRule->matchNumTags, &data, &dataMask);

    /* key4: packet data low [32:0]*/
    key[4UL] |= data & MACSEC_E_TCAMKM_TCAM_KEY4_0_PACKET_DATA_LO_MASK;
    mask[4UL] |= (uint32_t) dataMask & MACSEC_E_TCAMKM_TCAM_KEY4_0_PACKET_DATA_LO_MASK;
    /*key5: packet data high [23:0] */
    key[5UL] |= aRule->frameData[1UL] & MACSEC_E_TCAMKM_TCAM_KEY5_0_PACKET_DATA_HI_MASK;
    mask[5UL] |= (uint32_t) aRule->matchFrameDataMask[1UL] & MACSEC_E_TCAMKM_TCAM_KEY5_0_PACKET_DATA_HI_MASK;

    mask[1UL] |= MACSEC_MAC_BYTES_TO_UINT_HI(aRule->matchMacDAMask);
    mask[2UL] |= MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->matchMacDAMask);

    mask[2UL] |= (uint32_t)aRule->matchMacSAMask[0UL] << 16UL;
    mask[2UL] |= (uint32_t)aRule->matchMacSAMask[1UL] << 24UL;

    mask[3UL] |= (uint32_t)aRule->matchMacSAMask[2UL] << 0UL;
    mask[3UL] |= (uint32_t)aRule->matchMacSAMask[3UL] << 8UL;
    mask[3UL] |= (uint32_t)aRule->matchMacSAMask[4UL] << 16UL;
    mask[3UL] |= (uint32_t)aRule->matchMacSAMask[5UL] << 24UL;

    keyPolicy |= aVportIdx << MACSEC_E_TCAMKP_TCAM_POLICY_0_VPORT_INDEX_SHIFT;
    keyPolicy |= aRule->controlPkt << MACSEC_E_TCAMKP_TCAM_POLICY_0_CONTROL_PKT_SHIFT;

    /* write TCAM data */
    if (MACSEC_DIR_EGRESS == aDir) {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].key0_0), key[0U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].key1_0), key[1U]);
        /* @event         misra_c_2012_rule_violation, zero_deref
           @issue         Explicit null dereferenced (FORWARD_NULL)
           @risk          Undefined behaviour
           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
        /* coverity[misra_c_2012_rule_violation] */
        /* coverity[zero_deref] */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].key2_0), key[2U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].key3_0), key[3U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].key4_0), key[4U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].key5_0), key[5U]);

        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].mask0_0), mask[0U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].mask1_0), mask[1U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].mask2_0), mask[2U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].mask3_0), mask[3U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].mask4_0), mask[4U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKM_OFFSET_OF(km_regs[aIdx].mask5_0), mask[5U]);

        /* write TCAM policy */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_TCAMKP_OFFSET_OF(tcam_policy[aIdx]), keyPolicy);
    } else {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].key0_0), key[0U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].key1_0), key[1U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].key2_0), key[2U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].key3_0), key[3U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].key4_0), key[4U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].key5_0), key[5U]);

        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].mask0_0), mask[0U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].mask1_0), mask[1U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].mask2_0), mask[2U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].mask3_0), mask[3U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].mask4_0), mask[4U]);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKM_OFFSET_OF(km_regs[aIdx].mask5_0), mask[5U]);

        /* write TCAM policy */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_TCAMKP_OFFSET_OF(tcam_policy[aIdx]), keyPolicy);
    }
    /* Enable TCAM rule */
    if (MACSEC_DIR_EGRESS == aDir) {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_EN_OFFSET_OF(entry_enable_ctrl),
                (aIdx << MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_SET_SHIFT) |
                MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SET_ENABLE_MASK);
    } else {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_FCTRL_EN_OFFSET_OF(entry_enable_ctrl),
                (aIdx << MACSEC_I_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_SET_SHIFT) |
                MACSEC_I_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SET_ENABLE_MASK);
    }
}
/**
    @brief Delete TCAM entry from hardware

    @code{.unparsed}

    uint32_t *allocMaskAddr
    volatile uint32_t *entryEnableReg
    uint32_t *vportMaskAddr
    uint32_t vPortIdx
    uint32_t keyPolicy
    int32_t ret = BCM_ERR_OK
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID)

    if aRuleIdx < MACSEC_MAX_TCAM_ENTRIES
        if SECY_RULE_DIR_EGRESS == aDir
            allocMaskAddr = &MACSEC_HwCtxt[ctxIdx].egress.tcamRuleAllocMask
            keyPolicy = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE,
                            MACSEC_E_TCAMKP_OFFSET_OF(tcam_policy[aRuleIdx]))
            vPortIdx = keyPolicy & MACSEC_E_TCAMKP_TCAM_POLICY_0_VPORT_INDEX_MASK
            vportMaskAddr = &MACSEC_HwCtxt[ctxIdx].egress.vportAllocMask
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_EN_OFFSET_OF(entry_enable_ctrl),
                    (aRuleIdx << MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_CLEAR_SHIFT) |
                                 MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_CLEAR_ENABLE_MASK)
        else
            allocMaskAddr = &MACSEC_HwCtxt[ctxIdx].ingress.tcamRuleAllocMask
            keyPolicy = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE,
                            MACSEC_E_TCAMKP_OFFSET_OF(tcam_policy[aRuleIdx]))
            vPortIdx = keyPolicy & MACSEC_E_TCAMKP_TCAM_POLICY_0_VPORT_INDEX_MASK
            vportMaskAddr = &MACSEC_HwCtxt[ctxIdx].ingress.vportAllocMask
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_FCTRL_EN_OFFSET_OF(entry_enable_ctrl),
                    (aRuleIdx << MACSEC_I_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_CLEAR_SHIFT) |
                                 MACSEC_I_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_CLEAR_ENABLE_MASK)
        if (BCM_ERR_OK == ret) && (hash == aHash)
            *entryEnableReg = (aRuleIdx <<
                              MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_CLEAR_SHIFT) |
                              MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_CLEAR_ENABLE_MASK
            ret = MACSEC_DrvInFlightSync(aConnHdl, aID)
            if BCM_ERR_OK == ret
               MACSEC_SetBit(allocMaskAddr, aRuleIdx)
               MACSEC_SetBit(vportMaskAddr, vPortIdx)
        else
            ret = BCM_ERR_INVAL_PARAMS
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static int32_t MACSEC_DrvRemoveTCAMRule(BCM_HandleType aConnHdl, MACSEC_HwIDType aID,
                                        uint32_t aRuleIdx, MACSEC_DirType aDir)
{
    uint32_t *allocMaskAddr;
    uint32_t *vportMaskAddr;
    uint32_t vPortIdx;
    uint32_t keyPolicy;
    int32_t ret = BCM_ERR_OK;
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

    if (aRuleIdx < MACSEC_MAX_TCAM_ENTRIES) {
        if (SECY_RULE_DIR_EGRESS == aDir) {
            allocMaskAddr = &MACSEC_HwCtxt[ctxIdx].egress.tcamRuleAllocMask;
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            keyPolicy = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE,
                            MACSEC_E_TCAMKP_OFFSET_OF(tcam_policy[aRuleIdx]));
            vPortIdx = keyPolicy & MACSEC_E_TCAMKP_TCAM_POLICY_0_VPORT_INDEX_MASK;
            vportMaskAddr = &MACSEC_HwCtxt[ctxIdx].egress.vportAllocMask;
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_EN_OFFSET_OF(entry_enable_ctrl),
                    (aRuleIdx << MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_CLEAR_SHIFT) |
                                 MACSEC_E_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_CLEAR_ENABLE_MASK);
        } else {
            allocMaskAddr = &MACSEC_HwCtxt[ctxIdx].ingress.tcamRuleAllocMask;
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            keyPolicy = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE,
                            MACSEC_I_TCAMKP_OFFSET_OF(tcam_policy[aRuleIdx]));
            vPortIdx = keyPolicy & MACSEC_I_TCAMKP_TCAM_POLICY_0_VPORT_INDEX_MASK;
            vportMaskAddr = &MACSEC_HwCtxt[ctxIdx].ingress.vportAllocMask;
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_FCTRL_EN_OFFSET_OF(entry_enable_ctrl),
                    (aRuleIdx << MACSEC_I_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_SAM_INDEX_CLEAR_SHIFT) |
                                 MACSEC_I_FCTRL_EN_SAM_ENTRY_ENABLE_CTRL_CLEAR_ENABLE_MASK);
        }
        ret = MACSEC_DrvInFlightSync(aConnHdl, aID);
        if (BCM_ERR_OK == ret) {
            MACSEC_SetBit(allocMaskAddr, aRuleIdx);
            MACSEC_SetBit(vportMaskAddr, vPortIdx);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}


/**
   @brief Get macsec context
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

static MACSEC_ContextType* MACSEC_IntDrvGetCtxt(MACSEC_HwIDType aID, MACSEC_DirType aDir)
{
    MACSEC_ContextType *context;
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

    if (MACSEC_DIR_EGRESS == aDir) {
        context = &MACSEC_HwCtxt[ctxIdx].egress;
    } else {
        context = &MACSEC_HwCtxt[ctxIdx].ingress;
    }
    return context;
}
/**
    @brief

    @code{.unparsed}

    int32_t ret = BCM_ERR_OK
    uint32_t match0 = 0UL
    uint32_t match1 = 0UL
    uint32_t match2 = 0UL
    uint32_t match3 = 0UL
    uint32_t cpMatchEnable = 0UL
    uint32_t ruleIdx = 0UL
    MACSEC_ContextType *context

    if MACSEC_DIR_EGRESS == aDir
        context = &MACSEC_HwCtxt[ctxIdx].egress
    else
        context = &MACSEC_HwCtxt[ctxIdx].ingress

    if (aRule->ruleMask >= SECY_RULE_MATCH_DA_ONLY) &&
            (aRule->ruleMask <= SECY_RULE_MATCH_DA_ETHTYPE)
        ruleIdx = MACSEC_ClrNGetBit(&context->cpDAETRuleAllocMask)
        if ruleIdx >= MACSEC_CP_MACDAETH_MAX_RULES
            ret = BCM_ERR_NOMEM
        *aHashAddr = &context->cpDAETRuleHash[ruleIdx]

    switch (aRule->ruleMask)
        case SECY_RULE_MATCH_DA_ONLY:
            match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart)
            match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart)
            if MACSEC_DIR_EGRESS == aDir
                MACSEC_E_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_match_0 = match0
                MACSEC_E_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_et_match_0 = match1
            else
                MACSEC_I_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_match_0 = match0
                MACSEC_I_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_et_match_0 = match1
            cpMatchEnable = (0x1UL << ruleIdx)
            break
        case SECY_RULE_MATCH_ETHTYPE_ONLY:
            match1 = MACSEC_BYTE_SWAPPED(aRule->ethType) <<
                        MACSEC_E_CLS_P_CTRL_CMDEM0_ETHER_TYPE_MATCH_SHIFT
            if MACSEC_DIR_EGRESS == aDir
                MACSEC_E_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_et_match_0 = match1
            else
                MACSEC_I_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_et_match_0 = match1
            cpMatchEnable = 0x1UL << (MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_E_TYPE_0_SHIFT + ruleIdx)
            break
        case SECY_RULE_MATCH_DA_ETHTYPE:
            match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart)
            match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart)
            match1 |= MACSEC_BYTE_SWAPPED(aRule->ethType) <<
                        MACSEC_E_CLS_P_CTRL_CMDEM0_ETHER_TYPE_MATCH_SHIFT
            if MACSEC_DIR_EGRESS == aDir
                MACSEC_E_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_match_0 = match0
                MACSEC_E_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_et_match_0 = match1
            else
                MACSEC_I_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_match_0 = match0
                MACSEC_I_CLS_P_REGS[aID]->daetmatch_regs[ruleIdx].cp_mac_da_et_match_0 = match1
            cpMatchEnable = (0x1UL << ruleIdx)
            cpMatchEnable |= (0x1UL << (MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_E_TYPE_0_SHIFT  + ruleIdx))
            break
        case SECY_RULE_MATCH_DA_RANGE_ONLY:
            if context->cpDARangeRuleAllocMask != 0UL
                match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart)
                match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart)
                match2 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAEnd)
                match3 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAEnd)
                if MACSEC_DIR_EGRESS == aDir
                    MACSEC_E_CLS_P_REGS[aID]->cp_mac_da_start_lo = match0
                    MACSEC_E_CLS_P_REGS[aID]->cp_mac_da_start_hi = match1
                    MACSEC_E_CLS_P_REGS[aID]->cp_mac_da_end_lo = match2
                    MACSEC_E_CLS_P_REGS[aID]->cp_mac_da_end_hi = match3
                else
                    MACSEC_I_CLS_P_REGS[aID]->cp_mac_da_start_lo = match0
                    MACSEC_I_CLS_P_REGS[aID]->cp_mac_da_start_hi = match1
                    MACSEC_I_CLS_P_REGS[aID]->cp_mac_da_end_lo = match2
                    MACSEC_I_CLS_P_REGS[aID]->cp_mac_da_end_hi = match3
                cpMatchEnable = MACSEC_E_CLS_P_CTRL_SCP_MATCH_ENABLE_RANGE_MASK
                context->cpDARangeRuleAllocMask = 0UL
                *aHashAddr = &context->cpDARangeRuleHash
            else
                ret = BCM_ERR_NOMEM
            break
        case SECY_RULE_MATCH_DA_RANGE_ETHTYPE:
            ruleIdx = MACSEC_ClrNGetBit(&context->cpDAETRangeRuleAllocMask)
            if ruleIdx < MACSEC_CP_MACDAETRANGE_MAX_RULES
                match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart)
                match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart)
                match1 |= (uint32_t)MACSEC_BYTE_SWAPPED(aRule->ethType) <<
                            MACSEC_E_CLS_P_CTRL_CP_MAC_DA_START0_HI_ETHER_TYPE_MATCH_SHIFT
                match2 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAEnd)
                match3 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAEnd)
                if MACSEC_DIR_EGRESS == aDir
                    MACSEC_E_CLS_P_REGS[aID]->daetrange_regs[ruleIdx].cp_mac_da_start0_lo = match0
                    MACSEC_E_CLS_P_REGS[aID]->daetrange_regs[ruleIdx].cp_mac_da_start0_hi = match1
                    MACSEC_E_CLS_P_REGS[aID]->daetrange_regs[ruleIdx].cp_mac_da_end0_lo = match2
                    MACSEC_E_CLS_P_REGS[aID]->daetrange_regs[ruleIdx].cp_mac_da_end0_hi = match3
                else
                    MACSEC_I_CLS_P_REGS[aID]->daetrange_regs[ruleIdx].cp_mac_da_start0_lo = match0
                    MACSEC_I_CLS_P_REGS[aID]->daetrange_regs[ruleIdx].cp_mac_da_start0_hi = match1
                    MACSEC_I_CLS_P_REGS[aID]->daetrange_regs[ruleIdx].cp_mac_da_end0_lo = match2
                    MACSEC_I_CLS_P_REGS[aID]->daetrange_regs[ruleIdx].cp_mac_da_end0_hi = match3
                cpMatchEnable = (0x1UL << (MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_COMB8_SHIFT + ruleIdx))
                *aHashAddr = &context->cpDAETRangeRuleHash[ruleIdx]
            else
                ret = BCM_ERR_NOMEM
            break
    if BCM_ERR_OK == ret
        if MACSEC_DIR_EGRESS == aDir
            MACSEC_E_CLS_P_REGS[aID]->match_enable |= cpMatchEnable
        else
            MACSEC_I_CLS_P_REGS[aID]->match_enable |= cpMatchEnable
        *aRuleIdx = ruleIdx
err_exit:
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static int32_t MACSEC_DrvAddCPRuleInternal(BCM_HandleType aConnHdl, MACSEC_HwIDType aID,
                                           const SECY_RuleOpt0Type *const aRule,
                                           MACSEC_DirType aDir, uint32_t *const aRuleIdx,
                                           uint16_t **aHashAddr)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t match0 = 0UL;
    uint32_t match1 = 0UL;
    uint32_t match2 = 0UL;
    uint32_t match3 = 0UL;
    uint32_t cpMatchEnable = 0UL;
    uint32_t ruleIdx = 0UL;
    MACSEC_ContextType *context;

    context = MACSEC_IntDrvGetCtxt(aID, aDir);

    if ((aRule->ruleMask >= SECY_RULE_MATCH_DA_ONLY) &&
            (aRule->ruleMask <= SECY_RULE_MATCH_DA_ETHTYPE)) {
        ruleIdx = MACSEC_ClrNGetBit(&context->cpDAETRuleAllocMask);
        if (ruleIdx >= MACSEC_CP_MACDAETH_MAX_RULES) {
            ret = BCM_ERR_NOMEM;
        } else {
            *aHashAddr = &context->cpDAETRuleHash[ruleIdx];
        }
    }
    if(BCM_ERR_OK == ret) {
        switch (aRule->ruleMask) {
            case SECY_RULE_MATCH_DA_ONLY:
                match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart);
                match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart);
                if (MACSEC_DIR_EGRESS == aDir) {
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].mac_da_match_0),match0);
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].mac_da_et_match_0),match1);
                } else {
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].mac_da_match_0), match0);
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].mac_da_et_match_0), match1);
                }
                cpMatchEnable = (0x1UL << ruleIdx);
                break;
            case SECY_RULE_MATCH_ETHTYPE_ONLY:
                match1 = (uint32_t)MACSEC_BYTE_SWAPPED(aRule->ethType) <<
                                                       MACSEC_E_CLS_P_CTRL_CMDEM0_ETHER_TYPE_MATCH_SHIFT;
                if (MACSEC_DIR_EGRESS == aDir) {
                    /* @event         misra_c_2012_rule_violation, zero_deref
                       @issue         Explicit null dereferenced (FORWARD_NULL)
                       @risk          Undefined behaviour
                       @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                    /* coverity[misra_c_2012_rule_violation] */
                    /* coverity[zero_deref] */
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].mac_da_et_match_0), match1);
                } else {
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].mac_da_et_match_0), match1);
                }
                cpMatchEnable = (0x1UL << (MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_E_TYPE_0_SHIFT + ruleIdx));
                break;
            case SECY_RULE_MATCH_DA_ETHTYPE: /* Hardware has OR logic (either DA or EthType matches packet will be bypassed).
                                                Since the interface expects AND logic, disable this switch case and return NOSUPPORT */
#if 0
                match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart);
                match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart);
                match1 |= (uint32_t)MACSEC_BYTE_SWAPPED(aRule->ethType) <<
                            MACSEC_E_CLS_P_CTRL_CMDEM0_ETHER_TYPE_MATCH_SHIFT;
                if (MACSEC_DIR_EGRESS == aDir) {
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].cp_mac_da_match_0), match0);
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].cp_mac_da_et_match_0), match1);
                } else {
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].cp_mac_da_match_0), match0);
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetmatch_regs[ruleIdx].cp_mac_da_et_match_0), match1);
                }
                cpMatchEnable = (0x1UL << ruleIdx);
                cpMatchEnable |= (0x1UL << (MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_E_TYPE_0_SHIFT  + ruleIdx));
#endif
                ret = BCM_ERR_NOSUPPORT;
                break;
            case SECY_RULE_MATCH_DA_RANGE_ONLY:
                if (context->cpDARangeRuleAllocMask != 0UL) {
                    match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart);
                    match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart);
                    match2 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAEnd);
                    match3 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAEnd);
                    if (MACSEC_DIR_EGRESS == aDir) {
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(mac_da_start_lo), match0);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(mac_da_start_hi), match1);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(mac_da_end_lo), match2);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(mac_da_end_hi), match3);
                    } else {
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(mac_da_start_lo), match0);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(mac_da_start_hi), match1);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(mac_da_end_lo), match2);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(mac_da_end_hi), match3);
                    }
                    cpMatchEnable = MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_RANGE_MASK;
                    context->cpDARangeRuleAllocMask = 0UL;
                    *aHashAddr = &context->cpDARangeRuleHash;
                } else {
                    ret = BCM_ERR_NOMEM;
                }
                break;
            case SECY_RULE_MATCH_DA_RANGE_ETHTYPE:
                ruleIdx = MACSEC_ClrNGetBit(&context->cpDAETRangeRuleAllocMask);
                if (ruleIdx < MACSEC_CP_MACDAETRANGE_MAX_RULES) {
                    match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart);
                    match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart);
                    match1 |= (uint32_t)MACSEC_BYTE_SWAPPED(aRule->ethType) <<
                                                            MACSEC_E_CLS_P_CTRL_CP_MAC_DA_START0_HI_ETHER_TYPE_MATCH_SHIFT;
                    match2 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAEnd);
                    match3 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAEnd);
                    if (MACSEC_DIR_EGRESS == aDir) {
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetrange_regs[ruleIdx].mac_da_start0_lo), match0);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetrange_regs[ruleIdx].mac_da_start0_hi), match1);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetrange_regs[ruleIdx].mac_da_end0_lo), match2);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(daetrange_regs[ruleIdx].mac_da_end0_hi), match3);
                    } else {
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetrange_regs[ruleIdx].mac_da_start0_lo), match0);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetrange_regs[ruleIdx].mac_da_start0_hi), match1);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetrange_regs[ruleIdx].mac_da_end0_lo), match2);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(daetrange_regs[ruleIdx].mac_da_end0_hi), match3);
                    }
                    cpMatchEnable = (0x1UL << (MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_COMB8_SHIFT + ruleIdx));
                    *aHashAddr = &context->cpDAETRangeRuleHash[ruleIdx];
                } else {
                    ret = BCM_ERR_NOMEM;
                }
                break;
            case SECY_RULE_MATCH_DA_CONST:
                if (context->cpDAConstRuleAllocMask != 0UL) {
                    match0 = MACSEC_MAC_BYTES_TO_UINT_HI(aRule->macDAStart);
                    match1 = MACSEC_MAC_BYTES_TO_UINT_LOW(aRule->macDAStart);
                    if (MACSEC_DIR_EGRESS == aDir) {
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(mac_da_48_bits_lo), match0);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(mac_da_48_bits_hi), match1);
                    } else {
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(mac_da_48_bits_lo), match0);
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(mac_da_48_bits_hi), match1);
                    }
                    cpMatchEnable = MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_CONST_48_MASK;
                    context->cpDAConstRuleAllocMask = 0UL;
                    *aHashAddr = &context->cpDAConstRuleHash;
                } else {
                    ret = BCM_ERR_NOMEM;
                }
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
        }
    }
    if (BCM_ERR_OK == ret) {
        if (MACSEC_DIR_EGRESS == aDir) {
            MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(match_enable), cpMatchEnable);
        } else {
            MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(match_enable), cpMatchEnable);
        }
        *aRuleIdx = ruleIdx;
    }
    return ret;
}

/**
   @brief Non-match flow configuration

   This function configures the non matching flow(for which no
   valid SA is found) in hardware.

   @code{.unparsed}
    ===============================================================
        non-match flow register layout (same for control frames and
        non-control frames)
    ===============================================================
    |   bits[31:24]  | bits[23:16]  | bits[15:8]  | bits[7:0]  |
    ------------------------------------------------------------
    |  KaY Tagged    | bad Tag      | sec tagged  | untagged   |
    ------------------------------------------------------------

   @endcode

   @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
   @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS

*/
static void MACSEC_DrvNonMatchedFlow(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, MACSEC_DirType aDir)
{
    uint32_t flowCntrl = 0UL;
    uint32_t flowCntrlNcp= 0UL;

    if (MACSEC_DIR_EGRESS == aDir) {
        flowCntrl = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sam_nm_flow_cp));
    } else {
        flowCntrl = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(sam_nm_flow_cp));
    }

    /* Bypass Untagged Control frame (control frame without SecTAG) */
    flowCntrl &= ~MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_FLOW_TYPE0_MASK;
    flowCntrl &= ~MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_DEST_PORT0_MASK;

    /* drop tagged Control frame (control frame with SecTAG) */
    flowCntrl |= ((uint32_t)MACSEC_NONSAACTION_DROP << MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_FLOW_TYPE1_SHIFT);
    flowCntrl &= ~MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_DROP_ACTION1_MASK;
    flowCntrl |= ((uint32_t)MACSEC_DROP_INTERNAL << MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_DROP_ACTION1_SHIFT);

    /* drop control frame bad-tagged */
    flowCntrl |= ((uint32_t)MACSEC_NONSAACTION_DROP << MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_FLOW_TYPE2_SHIFT);
    flowCntrl &= ~MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_DROP_ACTION2_MASK;
    flowCntrl |= ((uint32_t)MACSEC_DROP_INTERNAL << MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_DROP_ACTION2_SHIFT);

    /* Drop control frame tagged KaY packets */
    flowCntrl |= ((uint32_t)MACSEC_NONSAACTION_DROP << MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_FLOW_TYPE3_SHIFT);
    flowCntrl &= ~MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_DROP_ACTION3_MASK;
    flowCntrl |= ((uint32_t)MACSEC_DROP_INTERNAL << MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_DROP_ACTION3_SHIFT);

    /* Drop non-control untagged, bad-tagged and KaY tagged */
    flowCntrlNcp = flowCntrl;
    flowCntrlNcp |= ((uint32_t)MACSEC_NONSAACTION_DROP << MACSEC_E_CLS_CTRL_SAM_NM_FLOW_NCP_FLOW_TYPE0_SHIFT);
    flowCntrlNcp &= ~MACSEC_E_CLS_CTRL_SAM_NM_FLOW_NCP_DROP_ACTION0_MASK;
    flowCntrlNcp |= ((uint32_t)MACSEC_DROP_INTERNAL << MACSEC_E_CLS_CTRL_SAM_NM_FLOW_CP_DROP_ACTION0_SHIFT);

    /* bypass non-control KaY tagged packet */
    flowCntrlNcp &= ~MACSEC_E_CLS_CTRL_SAM_NM_FLOW_NCP_FLOW_TYPE3_MASK;
    flowCntrlNcp &= ~MACSEC_E_CLS_CTRL_SAM_NM_FLOW_NCP_DEST_PORT3_MASK;

    if (MACSEC_DIR_EGRESS == aDir) {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sam_nm_flow_cp), flowCntrl);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sam_nm_flow_ncp), flowCntrlNcp);

    } else {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(sam_nm_flow_cp), flowCntrl);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(sam_nm_flow_ncp), flowCntrlNcp);
    }
}


/**
    @brief MacSec Enable QTag Parse (PCP-DEI-VID)

    When QTag Parsed is enabled, then Hardware parse and matches the
    below fields from the VLAN Tag Header(If mask in set)
    - VLAN ID
    - VLAN Priority (the priority field is remapped using fields map_tbl)
    - VLAN Inner Ether Type
    - Payload

    @code{.unparsed}
    for i=0 to 7
        mapTag |= MACSEC_VlanPriorityMapTbl << i * 3

    MACSEC_ReadModifyWrite(aConnHdl, sam_cp_tag, mapTag)
    MACSEC_ReadModifyWrite(aConnHdl, sam_cp_tag, 0x10000000)
    MACSEC_ReadModifyWrite(aConnHdl, sam_cp_enb, 0x01)
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvEnQtagParse(BCM_HandleType aConnHdl, uint32_t aID, uint32_t aDir)
{
    uint32_t i;
    uint32_t mapTag = 0UL;

    for (i = 0UL; i <= 7UL; i++) {
        mapTag |= ((MACSEC_VlanPriorityMapTbl[i] & 0x07UL) << (i * 3UL));
    }

    if (MACSEC_DIR_EGRESS == aDir) {
        MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sam_cp_tag), (mapTag & MACSEC_E_CLS_CTRL_SAM_CP_TAG_MAP_TBL_MASK));
        MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sam_cp_tag), MACSEC_E_CLS_CTRL_SAM_CP_TAG_QTAG_UP_EN_MASK);
        MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(tag_enb), MACSEC_E_CLS_P_CTRL_CP_TAG_ENB_PARSE_STAG0_MASK);
    } else {
        MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(sam_cp_tag), (mapTag & MACSEC_I_CLS_CTRL_SAM_CP_TAG_MAP_TBL_MASK));
        MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(sam_cp_tag), MACSEC_I_CLS_CTRL_SAM_CP_TAG_QTAG_UP_EN_MASK);
        MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(tag_enb), MACSEC_I_CLS_P_CTRL_CP_TAG_ENB_PARSE_STAG0_MASK);
    }

}

/**
    @brief

    @code{.unparsed}

    uint32_t miscCntrl
    uint32_t cntxCntrl
    uint32_t samNmParams
    MACSEC_ContextType *context
    uint32_t intCtrl
    ctxIdx = MACSEC_ConvPort2CtxIdx(aID)
    if MACSEC_DIR_EGRESS == aDir)
        context = &MACSEC_HwCtxt[ctxIdx].egress
    else
        context = &MACSEC_HwCtxt[ctxIdx].ingress
        context->rxCAMAllocMask = (0x1UL << MACSEC_MAX_SC) - 1

    context->vportAllocMask = (0x1UL << MACSEC_MAX_VPORTS) - 1UL
    context->saAllocMask = (0x1ULL << MACSEC_MAX_XFORM_RECORDS) - 1UL
    context->tcamRuleAllocMask = (0x1ULL << MACSEC_MAX_TCAM_ENTRIES) - 1ULL
    context->cpDAETRuleAllocMask = (0x1UL << MACSEC_CP_MACDAETH_MAX_RULES) - 1UL
    context->cpDAETRangeRuleAllocMask = (0x1UL << MACSEC_CP_MACDAETRANGE_MAX_RULES) - 1UL
    context->cpDARangeRuleAllocMask = 1UL

    MACSEC_HwCtxt[ctxIdx].secyAllocMask = (0x1UL << MACSEC_MAX_SECY_PER_PORT) - 1UL

    if MACSEC_DIR_EGRESS == aDir
        miscCntrl = MACSEC_E_CLS_REGS[aID]->misc_control
        cntxCntrl = MACSEC_E_EIP62_REGS[aID]->context_ctrl
        samNmParams = MACSEC_E_CLS_REGS[aID]->sam_nm_params
    else
        miscCntrl = MACSEC_I_CLS_REGS[aID]->misc_control
        cntxCntrl = MACSEC_I_EIP62_REGS[aID]->context_ctrl
        samNmParams = MACSEC_I_CLS_REGS[aID]->sam_nm_params

        miscCntrl &= ~MACSEC_I_CLS_CTRL_MISC_CONTROL_VALIDATE_FRAMES_MASK
        miscCntrl |= (SECY_VALIDATE_FRAME_STRICT <<
                        MACSEC_I_CLS_CTRL_MISC_CONTROL_VALIDATE_FRAMES_SHIFT)
        miscCntrl |= MACSEC_I_CLS_CTRL_MISC_CONTROL_NM_MACSEC_EN_MASK;
    }

    samNmParams &= ~MACSEC_E_CLS_CTRL_SAM_NM_PARAMS_CHECK_PN_MASK

    if (uint32_t)BCM_BOOL_TRUE == aCfg->checkKaY)
        samNmParams |= MACSEC_E_CLS_CTRL_SAM_NM_PARAMS_CHECK_KAY_MASK
    else
        samNmParams &= ~MACSEC_E_CLS_CTRL_SAM_NM_PARAMS_CHECK_KAY_MASK

    miscCntrl |= MACSEC_E_CLS_CTRL_MISC_CONTROL_DROP_ON_CRCERR_MASK
    miscCntrl |= MACSEC_E_CLS_CTRL_MISC_CONTROL_DROP_ON_PKTERR_MASK
    miscCntrl &= ~MACSEC_E_CLS_CTRL_MISC_CONTROL_ERR_DROP_ACTION_MASK
    miscCntrl |= (MACSEC_DROP_FRAME_ERROR << MACSEC_E_CLS_CTRL_MISC_CONTROL_ERR_DROP_ACTION_SHIFT)

    miscCntrl &= ~MACSEC_E_CLS_CTRL_MISC_CONTROL_XFORM_REC_SIZE_MASK
    miscCntrl |= (0x2UL << MACSEC_E_CLS_CTRL_MISC_CONTROL_XFORM_REC_SIZE_SHIFT)

    cntxCntrl &= ~MACSEC_E_EIP62_EIP62_CONTEXT_CTRL_CONTEXT_SIZE_MASK
    cntxCntrl |= (0x18UL << MACSEC_E_EIP62_EIP62_CONTEXT_CTRL_CONTEXT_SIZE_SHIFT)

    cntxCntrl |= MACSEC_E_EIP62_EIP62_CONTEXT_CTRL_PKT_NUM_THR_MODE_MASK

    intCtrl = MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_THRESHOLD_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_ROLLOVER_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_EN_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_EN_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_EN_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_ROLLOVER_EN_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_EN_MASK

    if MACSEC_DIR_EGRESS == aDir
        MACSEC_E_CLS_REGS[aID]->misc_control = miscCntrl
        MACSEC_E_EIP62_REGS[aID]->context_ctrl = cntxCntrl
        MACSEC_E_EIP62_REGS[aID]->context_upd_ctrl = 0x3
        MACSEC_E_CLS_REGS[aID]->dyn_latency_control = MACSEC_E_CLS_CTRL_DLC_DYN_LAT_ENABLE_MASK
        MACSEC_E_CLS_P_REGS[aID]->match_enable |=
            MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_MACSEC_KAY_MASK
        MACSEC_E_CLS_REGS[aID]->sam_nm_params = samNmParams
        MACSEC_E_EIP62_REGS[aID]->int_ctrl_stat = intCtrl
    else
        MACSEC_I_CLS_REGS[aID]->misc_control = miscCntrl
        MACSEC_I_EIP62_REGS[aID]->context_ctrl = cntxCntrl
        MACSEC_I_EIP62_REGS[aID]->context_upd_ctrl = 0x3
        MACSEC_I_CLS_REGS[aID]->dyn_latency_control = MACSEC_I_CLS_CTRL_DLC_DYN_LAT_ENABLE_MASK
        MACSEC_I_CLS_P_REGS[aID]->match_enable |=
            MACSEC_I_CLS_P_CTRL_CP_MATCH_ENABLE_MACSEC_KAY_MASK
        MACSEC_I_CLS_REGS[aID]->sam_nm_params = samNmParams
        MACSEC_I_EIP62_REGS[aID]->int_ctrl_stat = intCtrl

    if (uint32_t)BCM_BOOL_TRUE == aCfg->enableQtagParse
        MACSEC_DrvEnQtagParse(aConnHdl, aID, aDir)

    MACSEC_SYSMISC_REGS->interrupt_mask |= (0x1UL <<
            (MACSEC_SYSMISC_INTERRUPT_MASK_MSPU_0_INTMASK_SHIFT + aID))
    MACSEC_TOP_REGS[aID]->mib_ctrl |= MACSEC_MSPU_TOP_MIB_CTRL_CNT_RD_CLR_DIS_MASK
    MACSEC_TOP_REGS[aID]->sw_mib_ctrl |= MACSEC_MSPU_TOP_SW_MIB_CTRL_CNT_RD_CLR_DIS_MASK
    MACSEC_TOP_REGS[aID]->interrupt_enable2 |= 0x1UL
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvGlobalInit(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, const SECY_HwCfgType *const aCfg,
                                 MACSEC_DirType aDir)
{
    uint32_t miscCntrl;
    uint32_t cntxCntrl;
    uint32_t samNmParams;
    MACSEC_ContextType *context;
    uint32_t intCtrl;
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

    if (MACSEC_DIR_EGRESS == aDir) {
        context = &MACSEC_HwCtxt[ctxIdx].egress;
    } else {
        context = &MACSEC_HwCtxt[ctxIdx].ingress;
        context->rxCAMAllocMask = (0x1UL << MACSEC_MAX_SC) - 1UL;
    }

    context->vportAllocMask             = (0x1UL << MACSEC_MAX_VPORTS) - 1UL;
    context->saAllocMask                = (uint32_t)((0x1ULL << MACSEC_MAX_XFORM_RECORDS) - 1ULL);
    context->tcamRuleAllocMask          = (uint32_t)((0x1ULL << MACSEC_MAX_TCAM_ENTRIES ) - 1ULL);
    context->cpDAETRuleAllocMask        = (0x1UL << MACSEC_CP_MACDAETH_MAX_RULES    ) - 1UL;
    context->cpDAETRangeRuleAllocMask   = (0x1UL << MACSEC_CP_MACDAETRANGE_MAX_RULES) - 1UL;
    context->cpDARangeRuleAllocMask     = 1UL;
    context->cpDAConstRuleAllocMask     = 1UL;
    MACSEC_HwCtxt[ctxIdx].secyAllocMask = (0x1UL << MACSEC_MAX_SECY_PER_PORT) - 1UL;

    if (MACSEC_DIR_EGRESS == aDir) {
        miscCntrl = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(misc_control));
        cntxCntrl = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(context_ctrl));
        samNmParams = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sam_nm_params));
    } else {
        miscCntrl = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(misc_control));
        cntxCntrl = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(context_ctrl));
        samNmParams = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(sam_nm_params));

        /* Global 802.1AE validateFrame configuration: Strict
         * Drop all the frames for which no MACSec SC is found
         */
        miscCntrl &= ~MACSEC_I_CLS_CTRL_MISC_CONTROL_VALIDATE_FRAMES_MASK;
        miscCntrl |= (SECY_VALIDATE_FRAME_STRICT <<
                        MACSEC_I_CLS_CTRL_MISC_CONTROL_VALIDATE_FRAMES_SHIFT);
        miscCntrl |= MACSEC_I_CLS_CTRL_MISC_CONTROL_NM_MACSEC_EN_MASK;
    }

    /* On same port, we can have 32-bit and 64-bit ciphers used at the
     * same time, set 'check_pn = 0' and let the pn check safely happen
     * during the transfor record pickup
     */
    samNmParams &= ~MACSEC_E_CLS_CTRL_SAM_NM_PARAMS_CHECK_PN_MASK;

    /* enable the detection of special KaY frames if requested by configuration */
    if ((uint32_t)BCM_BOOL_TRUE == aCfg->checkKaY) {
        samNmParams |= MACSEC_E_CLS_CTRL_SAM_NM_PARAMS_CHECK_KAY_MASK;
    } else {
        samNmParams &= ~MACSEC_E_CLS_CTRL_SAM_NM_PARAMS_CHECK_KAY_MASK;
    }

    /* global packet drop error settings:
     * drop short frame error and CRC error packet (internal drop)
     */
    miscCntrl |= MACSEC_E_CLS_CTRL_MISC_CONTROL_DROP_ON_CRCERR_MASK;
    miscCntrl |= MACSEC_E_CLS_CTRL_MISC_CONTROL_DROP_ON_PKTERR_MASK;
    miscCntrl &= ~MACSEC_E_CLS_CTRL_MISC_CONTROL_ERR_DROP_ACTION_MASK;
    miscCntrl |= ((uint32_t)MACSEC_DROP_FRAME_ERROR << MACSEC_E_CLS_CTRL_MISC_CONTROL_ERR_DROP_ACTION_SHIFT);

    /* configure the transform record size to 96 words (6 X 128 bit) */
    miscCntrl &= ~MACSEC_E_CLS_CTRL_MISC_CONTROL_XFORM_REC_SIZE_MASK;
    miscCntrl |= (0x2UL << MACSEC_E_CLS_CTRL_MISC_CONTROL_XFORM_REC_SIZE_SHIFT);

    /* configure EI62 core to max fetch value of 96 words */
    cntxCntrl &= ~MACSEC_E_EIP62_EIP62_CONTEXT_CTRL_CONTEXT_SIZE_MASK;
    cntxCntrl |= (0x18UL << MACSEC_E_EIP62_EIP62_CONTEXT_CTRL_CONTEXT_SIZE_SHIFT);

    /* set the packet number thrshd for exact equal match (to trigger the E8 interrupt) */
    cntxCntrl |= MACSEC_E_EIP62_EIP62_CONTEXT_CTRL_PKT_NUM_THR_MODE_MASK;

    /* bits[15:0] are status bits and [31:16] are enable bits.
     * Writing status bits will clear the pending interrupts along
     * with enable bits.
     * Sequence threshold interrupt will be enabled when sequence
     * threshold is programmed.
     */
    intCtrl = MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_THRESHOLD_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_ROLLOVER_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_EN_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_EN_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_EN_MASK |
              MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_EN_MASK;

    if (MACSEC_DIR_EGRESS == aDir) {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(misc_control), miscCntrl);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(context_ctrl), cntxCntrl);
        /* don't update the context for the bad frames (crc or frame error) */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(context_upd_ctrl), 0x3);
        /* Enable dynamic latency control with 0 latency words */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(dyn_latency_control), MACSEC_E_CLS_CTRL_DLC_DYN_LAT_ENABLE_MASK);
        /* Mark KaY frames as control frames */
        MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(match_enable), MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_MACSEC_KAY_MASK);

        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sam_nm_params), samNmParams);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_BRCM_GLOBAL_OFFSET_OF(prescale), MACSEC_TIMESTAMP_PRESCALE);

        intCtrl |= MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_THRESHOLD_EN_MASK |
               MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_ROLLOVER_EN_MASK;
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(int_ctrl_stat), intCtrl);

        /* Enable egress interrupt in AIC (required for SA threshold and SA expiry irq) */
        intCtrl = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_EIP160_AIC_OFFSET_OF(enable_ctrl));
        intCtrl |= MACSEC_AIC_LOCAL_EN_CTRL_SA_PN_THRLD_MASK | MACSEC_AIC_LOCAL_EN_CTRL_SA_EXPIRED_MASK;
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_EIP160_AIC_OFFSET_OF(enable_ctrl), intCtrl);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE,
                MACSEC_EIP165_AIC_OFFSET_OF(enable_ctrl), MACSEC_AIC_GLOBAL_EN_CTRL_EGRESS_MASK);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_EIP165_AIC_OFFSET_OF(pol_ctrl),
                MACSEC_AIC_GLOBAL_POL_CTRL_EGRESS_MASK);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_EIP165_AIC_OFFSET_OF(enabled_stat_ack),
                MACSEC_EIP165_AIC_REGS_AESA_ENABLED_STATUS_ACK_MASK);
    } else {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(misc_control), miscCntrl);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(context_ctrl), cntxCntrl);
        /* don't update the context for the bad frames (crc or frame error) */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(context_upd_ctrl), 0x3);
        /* Enable dynamic latency control with 0 latency words */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(dyn_latency_control), MACSEC_I_CLS_CTRL_DLC_DYN_LAT_ENABLE_MASK);
        /* Mark KaY frames as control frames */
        MACSEC_ReadModifyWrite(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(match_enable), MACSEC_I_CLS_P_CTRL_CP_MATCH_ENABLE_MACSEC_KAY_MASK);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_CTRL_OFFSET_OF(sam_nm_params), samNmParams);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(int_ctrl_stat), intCtrl);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_BRCM_GLOBAL_OFFSET_OF(prescale), MACSEC_TIMESTAMP_PRESCALE);
    }

    if ((uint32_t)BCM_BOOL_TRUE == aCfg->enableQtagParse) {
        MACSEC_DrvEnQtagParse(aConnHdl, aID, aDir);
    }
}

/**
   @code{.unparsed}
    uint32_t cmdConfig
    uint32_t ipgLen
    int32_t ret = BCM_ERR_OK

    if (aCfg->speed >= MACSEC_MACSPEED_100MBPS) && (aCfg->speed <= MACSEC_MACSPEED_2500MBPS)
        cmdConfig = MACSEC_MAC_REGS[aID]->line_command_config
        cmdConfig &= ~MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ETH_SPEED_MASK
        cmdConfig |= (aCfg->speed << MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT)
        cmdConfig |= MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_TX_ENA_MASK
        cmdConfig |= MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_RX_ENA_MASK
        MACSEC_MAC_REGS[aID]->line_command_config = cmdConfig

        ipgLen = MACSEC_MAC_REGS[aID]->line_tx_ipg_length
        ipgLen &= ~MACSEC_MSPU_MAC_LUTIL_TX_IPG_LENGTH_TX_IPG_LENGTH_MASK
        ipgLen |= (0xCUL << MACSEC_MSPU_MAC_LUTIL_TX_IPG_LENGTH_TX_IPG_LENGTH_SHIFT)
        MACSEC_MAC_REGS[aID]->line_tx_ipg_length = ipgLen

        cmdConfig = MACSEC_MAC_REGS[aID]->switch_command_config
        cmdConfig &= ~MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ETH_SPEED_MASK
        cmdConfig |= (aCfg->speed << MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT)
        cmdConfig |= MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_TX_ENA_MASK
        cmdConfig |= MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_RX_ENA_MASK
        MACSEC_MAC_REGS[aID]->switch_command_config = cmdConfig

        ipgLen = MACSEC_MAC_REGS[aID]->switch_tx_ipg_length
        ipgLen &= ~MACSEC_MSPU_MAC_SUTIL_TX_IPG_LENGTH_TX_IPG_LENGTH_MASK
        ipgLen |= (0xCUL << MACSEC_MSPU_MAC_SUTIL_TX_IPG_LENGTH_TX_IPG_LENGTH_SHIFT)
        MACSEC_MAC_REGS[aID]->switch_tx_ipg_length = ipgLen
    else
        ret = BCM_ERR_INVAL_PARAMS;
    return ret
   @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvUniMacInit(BCM_HandleType aConnHdl, uint32_t aID, const SECY_HwCfgType *const aCfg)
{
    uint32_t cmdConfig;
    uint32_t ipgLen;
    int32_t ret = BCM_ERR_OK;
    ETHXCVR_SpeedType aSpeed = ETHXCVR_SPEED_1000MBPS;
    /* Get the internal PHY speed */
    ret = ETHXCVR_DrvGetSpeed(ETHXCVR_ID_CU_PHY, &aSpeed);
    if(BCM_ERR_OK == ret) {
        /* configure the line side mac speed and enable rx/tx */
        cmdConfig = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config));
        cmdConfig &= ~MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ETH_SPEED_MASK;
        if(aSpeed == ETHXCVR_SPEED_100MBPS) {
            cmdConfig |= (MACSEC_MACSPEED_100MBPS << MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT);
        } else {
            cmdConfig |= (MACSEC_MACSPEED_1000MBPS << MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT);
        }
        cmdConfig |= MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_TX_ENA_MASK;
        cmdConfig |= MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_RX_ENA_MASK;
        cmdConfig &= ~MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_CRC_FWD_MASK;
        cmdConfig &= ~MACSEC_MSPU_MAC_LUCC_COMMAND_CONFIG_ENA_EXT_CONFIG_MASK;
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_command_config), cmdConfig);

        /* set IPG len to 12 bytes */
        ipgLen = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_tx_ipg_length));
        ipgLen &= ~MACSEC_MSPU_MAC_LUTIL_TX_IPG_LENGTH_TX_IPG_LENGTH_MASK;
        ipgLen |= (0xCUL << MACSEC_MSPU_MAC_LUTIL_TX_IPG_LENGTH_TX_IPG_LENGTH_SHIFT);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_tx_ipg_length), ipgLen);

        /* set frame lenght to 0x3FFF to support jumbo frame */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(line_frm_length), 0x3FFF);

        /* configure the switch side mac speed and enable rx/tx */
        cmdConfig = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config));
        cmdConfig &= ~MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ETH_SPEED_MASK;
        if(aSpeed == ETHXCVR_SPEED_100MBPS) {
            cmdConfig |= (MACSEC_MACSPEED_100MBPS << MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT);
        } else {
            cmdConfig |= (MACSEC_MACSPEED_1000MBPS << MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ETH_SPEED_SHIFT);
        }
        cmdConfig |= MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_TX_ENA_MASK;
        cmdConfig |= MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_RX_ENA_MASK;
        cmdConfig &= ~MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_CRC_FWD_MASK;
        cmdConfig &= ~MACSEC_MSPU_MAC_SUCC_COMMAND_CONFIG_ENA_EXT_CONFIG_MASK;
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_command_config), cmdConfig);

        /* set IPG len to 12 bytes */
        ipgLen = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_tx_ipg_length));
        ipgLen &= ~MACSEC_MSPU_MAC_SUTIL_TX_IPG_LENGTH_TX_IPG_LENGTH_MASK;
        ipgLen |= (0xCUL << MACSEC_MSPU_MAC_SUTIL_TX_IPG_LENGTH_TX_IPG_LENGTH_SHIFT);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_tx_ipg_length), ipgLen);

        /* set frame lenght to 0x3FFF to support jumbo frame */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_MSPU_MAC, MACSEC_MSPU_MAC_OFFSET_OF(switch_frm_length), 0x3FFF);
    }
    return ret;
}

/**
   @code{.unparsed}

   if aHashAddrEgress is not NULL
    ruleAddr = (uint32_t)aHashAddrEgress
    hash = BCM_CRC16((uint8_t *)&ruleAddr, sizeof(ruleAddr), SECY_HANDLE_CRC16_POLY, 0);

   if aHashAddrIngress is not NULL
    ruleAddr = (uint32_t)aHashAddrIngress
    hash = BCM_CRC16((uint8_t *)&ruleAddr, sizeof(ruleAddr), SECY_HANDLE_CRC16_POLY, hash);

    aRuleHdl = aRuleHdl & (~SECY_HANDLE_HASH_MASK);
    hash = BCM_CRC16(&aRuleHdl, sizeof(BCM_HandleType), SECY_HANDLE_CRC16_POLY, hash);
    return hash

   @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static uint16_t MACSEC_DrvGenerateRuleHash(BCM_HandleType aRuleHdl, uint16_t *aHashAddrEgress,
                                                                    uint16_t *aHashAddrIngress)
{
    uint16_t crc = 0U;

    if (NULL != aHashAddrEgress) {
        crc = BCM_CRC16((uint8_t *)&aHashAddrEgress , sizeof(aHashAddrEgress) , SECY_HANDLE_CRC16_POLY, crc);
    }
    if (NULL != aHashAddrIngress) {
        crc = BCM_CRC16((uint8_t *)&aHashAddrIngress, sizeof(aHashAddrIngress), SECY_HANDLE_CRC16_POLY, crc);
    }
    aRuleHdl = aRuleHdl & (~SECY_HANDLE_HASH_MASK);
    crc = BCM_CRC16((uint8_t *)&aRuleHdl, sizeof(BCM_HandleType), SECY_HANDLE_CRC16_POLY, crc);
    return crc;
}

/**
   @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    uint32_t *allocMask
    MACSEC_ContextType *context
    uint32_t cpMatchEnable = 0UL
    ctxIdx = MACSEC_ConvPort2CtxIdx(aID)

    if MACSEC_DIR_EGRESS == aDir
        context = &MACSEC_HwCtxt[ctxIdx].egress
    else
        context = &MACSEC_HwCtxt[ctxIdx].ingress

    switch (match)
        case SECY_RULE_MATCH_DA_ONLY:
            allocMask = &context->cpDAETRuleAllocMask
            cpMatchEnable = (0x1UL << aRuleIdx)
            break
        case SECY_RULE_MATCH_ETHTYPE_ONLY:
            allocMask = &context->cpDAETRuleAllocMask
            cpMatchEnable = (0x1UL << (8UL + aRuleIdx))
            break
        case SECY_RULE_MATCH_DA_ETHTYPE:
            allocMask = &context->cpDAETRuleAllocMask
            cpMatchEnable = (0x1UL << aRuleIdx)
            cpMatchEnable |= (0x1UL << (8UL + aRuleIdx))
            break
        case SECY_RULE_MATCH_DA_RANGE_ONLY:
            allocMask = &context->cpDARangeRuleAllocMask
            cpMatchEnable = MACSEC_E_CLS_P_CTRL_SCP_MATCH_ENABLE_RANGE_MASK
            break
        case SECY_RULE_MATCH_DA_RANGE_ETHTYPE:
            allocMask = &context->cpDAETRangeRuleAllocMask
            cpMatchEnable = (0x1UL << (MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_COMB8_SHIFT + aRuleIdx))
            break
        default:
            ret = BCM_ERR_NOT_FOUND;
            break;
    if BCM_ERR_OK == ret
        if 0U == ((0x1 << aRuleIdx) & *allocMask
            if MACSEC_DIR_EGRESS == aDir)
                MACSEC_E_CLS_P_REGS[aID]->match_enable &= ~cpMatchEnable
            else
                MACSEC_I_CLS_P_REGS[aID]->match_enable &= ~cpMatchEnable
            MACSEC_SetBit(allocMask, aRuleIdx)
        else
            ret = BCM_ERR_NOT_FOUND
    return ret;
   @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static int32_t MACSEC_DrvRemoveCPRuleInternal(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint8_t aRuleIdx,
                                              SECY_RuleMatchType match, MACSEC_DirType aDir)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t *allocMask=NULL;
    MACSEC_ContextType *context;
    uint32_t cpMatchEnable = 0UL;
    uint32_t data;
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

    if (MACSEC_DIR_EGRESS == aDir) {
        context = &MACSEC_HwCtxt[ctxIdx].egress;
    } else {
        context = &MACSEC_HwCtxt[ctxIdx].ingress;
    }

    switch (match) {
        case SECY_RULE_MATCH_DA_ONLY:
            if (aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES) {
                allocMask = &context->cpDAETRuleAllocMask;
                cpMatchEnable = (0x1UL << aRuleIdx);
                ret = BCM_ERR_OK;
            }
            break;
        case SECY_RULE_MATCH_ETHTYPE_ONLY:
            if (aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES) {
                allocMask = &context->cpDAETRuleAllocMask;
                cpMatchEnable = (0x1UL << (8UL + aRuleIdx));
                ret = BCM_ERR_OK;
            }
            break;
        case SECY_RULE_MATCH_DA_ETHTYPE:
            if (aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES) {
                allocMask = &context->cpDAETRuleAllocMask;
                cpMatchEnable = (0x1UL << aRuleIdx);
                cpMatchEnable |= (0x1UL << (8UL + aRuleIdx));
                ret = BCM_ERR_OK;
            }
            break;
        case SECY_RULE_MATCH_DA_RANGE_ONLY:
            allocMask = &context->cpDARangeRuleAllocMask;
            cpMatchEnable = MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_RANGE_MASK;
            ret = BCM_ERR_OK;
            break;
        case SECY_RULE_MATCH_DA_RANGE_ETHTYPE:
            if (aRuleIdx < MACSEC_CP_MACDAETRANGE_MAX_RULES) {
                allocMask = &context->cpDAETRangeRuleAllocMask;
                cpMatchEnable = (0x1UL << (MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_COMB8_SHIFT + aRuleIdx));
                ret = BCM_ERR_OK;
            }
            break;
        case SECY_RULE_MATCH_DA_CONST:
            allocMask = &context->cpDAConstRuleAllocMask;
            cpMatchEnable = MACSEC_E_CLS_P_CTRL_CP_MATCH_ENABLE_CONST_48_MASK;
            ret = BCM_ERR_OK;
            break;
        default:
            BCM_ASSERT(0UL == (uint32_t)&aID);
            break;
    }
    if (BCM_ERR_OK == ret) {
        /* @event      CERT INT34-C
                   @issue       Coverity infers that aRuleIdx can exceed the limit.
                   @risk           No risk
                   @justification aRuleIdx is uint8 type variable this will not exceed the limit after left shift. */
        /* coverity[cert_int34_c_violation] */
        if ( 0U == ((0x1U << aRuleIdx) & ((uint8_t)(*allocMask & 0xFFUL))) ) {
            if (MACSEC_DIR_EGRESS == aDir) {
                data = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(match_enable));
                data &= ~cpMatchEnable;
                MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_P_CTRL_OFFSET_OF(match_enable), data);
            } else {
                data = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(match_enable));
                data &= ~cpMatchEnable;
                MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_CLS_P_CTRL_OFFSET_OF(match_enable), data);
            }
            MACSEC_SetBit(allocMask, aRuleIdx);
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }
    return ret;
}

/**
   @code{.unparsed}

    uint16_t *hashAddr = NULL
    MACSEC_ContextType *context
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID)

    if MACSEC_DIR_EGRESS == aDir
        context = &MACSEC_HwCtxt[ctxIdx].egress
    else
        context = &MACSEC_HwCtxt[ctxIdx].ingress

    if MACSEC_RULEMASK_HDRPARSE == aRuleType
        switch match
            case SECY_RULE_MATCH_DA_ONLY
                if aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES
                    hashAddr = &context->cpDAETRuleHash[aRuleIdx]
                break
            case SECY_RULE_MATCH_ETHTYPE_ONLY
                if aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES
                    hashAddr = &context->cpDAETRuleHash[aRuleIdx]
                break
            case SECY_RULE_MATCH_DA_ETHTYPE
                if aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES
                    hashAddr = &context->cpDAETRuleHash[aRuleIdx]
                break
            case SECY_RULE_MATCH_DA_RANGE_ONLY
                hashAddr = &context->cpDARangeRuleHash
                break
            case SECY_RULE_MATCH_DA_RANGE_ETHTYPE
                if aRuleIdx < MACSEC_CP_MACDAETRANGE_MAX_RULES
                    hashAddr = &context->cpDAETRangeRuleHash[aRuleIdx]
                break
            case SECY_RULE_MATCH_DA_CONST
                hashAddr = &context->cpDAConstRuleHash
                break
    else
        if aRuleIdx < MACSEC_MAX_TCAM_ENTRIES
            hashAddr = &context->tcamRuleHash[aRuleIdx]
    return hashAddr
   @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static uint16_t * MACSEC_DrvGetRuleHashAddr(MACSEC_HwIDType aID, MACSEC_RuleMaskType aRuleType,
                                            uint8_t aRuleIdx, SECY_RuleMatchType match,
                                            MACSEC_DirType aDir)
{
    uint16_t *hashAddr = NULL;
    MACSEC_ContextType *context;
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

    if (MACSEC_DIR_EGRESS == aDir) {
        context = &MACSEC_HwCtxt[ctxIdx].egress;
    } else {
        context = &MACSEC_HwCtxt[ctxIdx].ingress;
    }

    if (MACSEC_RULEMASK_HDRPARSE == aRuleType) {
        switch (match) {
            case SECY_RULE_MATCH_DA_ONLY:
                if (aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES) {
                    hashAddr = &context->cpDAETRuleHash[aRuleIdx];
                }
                break;
            case SECY_RULE_MATCH_ETHTYPE_ONLY:
                if (aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES) {
                    hashAddr = &context->cpDAETRuleHash[aRuleIdx];
                }
                break;
            case SECY_RULE_MATCH_DA_ETHTYPE:
                if (aRuleIdx < MACSEC_CP_MACDAETH_MAX_RULES) {
                    hashAddr = &context->cpDAETRuleHash[aRuleIdx];
                }
                break;
            case SECY_RULE_MATCH_DA_RANGE_ONLY:
                hashAddr = &context->cpDARangeRuleHash;
                break;
            case SECY_RULE_MATCH_DA_RANGE_ETHTYPE:
                if (aRuleIdx < MACSEC_CP_MACDAETRANGE_MAX_RULES) {
                    hashAddr = &context->cpDAETRangeRuleHash[aRuleIdx];
                }
                break;
            case SECY_RULE_MATCH_DA_CONST:
                hashAddr = &context->cpDAConstRuleHash;
                break;
            default:
                BCM_ASSERT(0UL == (uint32_t)&aID);
                break;
        }
    } else {
        if (aRuleIdx < MACSEC_MAX_TCAM_ENTRIES) {
            hashAddr = &context->tcamRuleHash[aRuleIdx];
        }
    }
    return hashAddr;
}

/**
    @brief Delete secy lookup rule

    @code{.unparsed}

    MACSEC_HandleInfoType info
    MACSEC_SecYContextType *secy
    uint16_t *hashAddrEgress = NULL
    uint16_t *hashAddrIngress = NULL
    uint16_t ruleHash = 0U
    int32_t ret = BCM_ERR_INVAL_PARAMS

    MACSEC_HANDLE_INFO_INIT(info)
    ret = MACSEC_DrvGetHandleInfo(aRuleHdl, &info)
    if ret is BCM_ERR_OK
        if SECY_RULE_DIR_EGRESS == info.dir
            hashAddrEgress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, MACSEC_RULEMASK_TCAM,
                                    info.ruleIdxEgress, 0U, SECY_RULE_DIR_EGRESS)
        else if SECY_RULE_DIR_INGRESS == info.dir)
            hashAddrIngress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, MACSEC_RULEMASK_TCAM,
                                    info.ruleIdxIngress, 0U, SECY_RULE_DIR_INGRESS)
        else if SECY_RULE_DIR_BOTH == info.dir
            hashAddrEgress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, MACSEC_RULEMASK_TCAM,
                                    info.ruleIdxEgress, 0U, SECY_RULE_DIR_EGRESS);
            hashAddrIngress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, MACSEC_RULEMASK_TCAM,
                                    info.ruleIdxIngress, 0U, SECY_RULE_DIR_INGRESS);
        else
            ret = ERR_INVALID_PARAMS

        if ret is ERR_OK
            ruleHash = MACSEC_DrvGenerateRuleHash(aRuleHdl, hashAddrEgress, hashAddrIngress)
            if ruleHash == info.hash || ((uint32_t)BCM_BOOL_TRUE == aForceRemove
                ctxIdx = MACSEC_ConvPort2CtxIdx(info.hwIdx)
                secy = &MACSEC_HwCtxt[ctxIdx].secy[info.secyIdx]
                if SECY_RULE_DIR_EGRESS & info.dir != 0UL
                    ret = MACSEC_DrvRemoveTCAMRule(aConnHdl, info.hwIdx, info.ruleIdxEgress,
                                                SECY_RULE_DIR_EGRESS)
                    if BCM_ERR_OK == ret
                        secy->txTcamRules &= ~(0x1UL << info.ruleIdxEgress)
                        *hashAddrEgress = 0U
                if SECY_RULE_DIR_INGRESS & info.dir != 0U
                    ret = MACSEC_DrvRemoveTCAMRule(aConnHdl, info.hwIdx, info.ruleIdxIngress,
                                                SECY_RULE_DIR_INGRESS)
                    if BCM_ERR_OK == ret
                        secy->rxTcamRules &= ~(0x1UL << info.ruleIdxIngress)
                        *hashAddrIngress = 0U
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvRemoveSecYLookupRule(BCM_HandleType aConnHdl, MACSEC_RuleHdlType aRuleHdl, BCM_BoolType aForceRemove)
{
    uint32_t ctxIdx;
    MACSEC_HandleInfoType info;
    MACSEC_SecYContextType *secy;
    uint16_t *hashAddrEgress = NULL;
    uint16_t *hashAddrIngress = NULL;
    uint16_t ruleHash = 0U;
    int32_t ret;

    MACSEC_HANDLE_INFO_INIT(info);
    ret = MACSEC_DrvGetHandleInfo(aRuleHdl, &info);
    if (BCM_ERR_OK == ret) {
        if (SECY_RULE_DIR_EGRESS == info.dir) {
            hashAddrEgress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, MACSEC_RULEMASK_TCAM,
                                    info.ruleIdxEgress, 0U, SECY_RULE_DIR_EGRESS);
        } else if (SECY_RULE_DIR_INGRESS == info.dir) {
            hashAddrIngress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, MACSEC_RULEMASK_TCAM,
                                    info.ruleIdxIngress, 0U, SECY_RULE_DIR_INGRESS);
        } else if (SECY_RULE_DIR_BOTH == info.dir) {
            hashAddrEgress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, MACSEC_RULEMASK_TCAM,
                                    info.ruleIdxEgress, 0U, SECY_RULE_DIR_EGRESS);
            hashAddrIngress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, MACSEC_RULEMASK_TCAM,
                                    info.ruleIdxIngress, 0U, SECY_RULE_DIR_INGRESS);
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }

        if (BCM_ERR_OK == ret) {
            ruleHash = MACSEC_DrvGenerateRuleHash(aRuleHdl, hashAddrEgress, hashAddrIngress);
            if ((ruleHash == info.hash) || (BCM_BOOL_TRUE == aForceRemove)) {
                ctxIdx = MACSEC_ConvPort2CtxIdx(info.hwIdx);
                secy = &MACSEC_HwCtxt[ctxIdx].secy[info.secyIdx];
                if ((SECY_RULE_DIR_EGRESS & info.dir) != 0U) {
                    if (0U != MACSEC_HwCtxt[ctxIdx].egress.tcamRuleHash[info.ruleIdxEgress]) {
                        ret = MACSEC_DrvRemoveTCAMRule(aConnHdl, info.hwIdx, info.ruleIdxEgress,
                                SECY_RULE_DIR_EGRESS);
                        if (BCM_ERR_OK == ret) {
                            secy->txTcamRules &= ~(0x1UL << info.ruleIdxEgress);
                            if (NULL != hashAddrEgress) {
                                *hashAddrEgress = 0U;
                            }
                        }
                    } else {
                        ret = BCM_ERR_INVAL_PARAMS;
                    }
                }
                if ((SECY_RULE_DIR_INGRESS & info.dir) != 0U) {
                    if (0U != MACSEC_HwCtxt[ctxIdx].ingress.tcamRuleHash[info.ruleIdxIngress]) {
                        ret = MACSEC_DrvRemoveTCAMRule(aConnHdl, info.hwIdx, info.ruleIdxIngress,
                                SECY_RULE_DIR_INGRESS);
                        if (BCM_ERR_OK == ret) {
                            secy->rxTcamRules &= ~(0x1UL << info.ruleIdxIngress);
                            if (NULL != hashAddrIngress) {
                                *hashAddrIngress = 0U;
                            }
                        }
                    } else {
                        ret = BCM_ERR_INVAL_PARAMS;
                    }
                }
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        }
    }
    return ret;
}

/**
   @brief Allocate an RxCAM entry for Rx SC

   @code{.unparsed}
    SECY_SCIType reservedSCI
    MACSEC_SecYContextType *secy
    uint32_t i
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if NULL != aSCI
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            BCM_MemSet(&reservedSCI, 0xFFU, sizeof(reservedSCI))
            if BCM_MemCmp(&reservedSCI, aSCI, sizeof(SECY_SCIType) != 0UL)
                for i = 0UL; i < MACSEC_MAX_RXSC_PER_SECY; i++
                    if (uint32_t)BCM_BOOL_FALSE == secy->rxSC[i].inUse
                        BCM_MemCpy(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType))
                        secy->rxSC[i].inUse = TRUE
                        ret = BCM_ERR_OK
                        break
                if i == MACSEC_MAX_RXSC_PER_SECY
                    ret = BCM_ERR_NOMEM
    return ret;
   @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

static int32_t MACSEC_DrvAllocRxCAMEntry(BCM_HandleType aConnHdl, MACSEC_HwIDType aID,
                                        const SECY_SCIType *const aSCI,
                                        uint32_t aVportIdx,
                                        uint32_t *const aRxSCIdx,
                                        BCM_BoolType aImplicitSCI)
{
    uint64_t sci64;
    uint32_t rxCAMIdx;
    int32_t ret = BCM_ERR_OK;
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

    sci64 = MACSEC_DrvSCIToU64(aSCI);

    rxCAMIdx = MACSEC_ClrNGetBit(&MACSEC_HwCtxt[ctxIdx].ingress.rxCAMAllocMask);
    if (rxCAMIdx < MACSEC_MAX_SC) {
        /*
        if (pkt.SecTAG.SC)
            key.SCI = pkt.SecTAG.SCI
        else if (pkt.SecTAG.ES and pkt.SecTAG.SCB)
            key.SCI = {pkt.MAC_SA, 0x0000}
        else if (pkt.SecTAG.ES and not pkt.SecTAG.SCB)
            key.SCI = {pkt.MAC_SA, 0x0001}
        else {ES,SCB} = (01 or 00)
            key.SCI = 0xFFFF_FFFF_FFFF_FFFF
        */
        if (BCM_BOOL_FALSE == aImplicitSCI) {
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_RXSC_CAMKEY_OFFSET_OF(sci_regs[rxCAMIdx].sci_lo_0),
                                        ((uint32_t) (sci64 & 0xFFFFFFFFUL)));
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_RXSC_CAMKEY_OFFSET_OF(sci_regs[rxCAMIdx].sci_hi_0),
                                        ((uint32_t)(sci64 >> 32UL) & 0xFFFFFFFFUL));
        } else {
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_RXSC_CAMKEY_OFFSET_OF(sci_regs[rxCAMIdx].sci_lo_0), 0xFFFFFFFFUL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_RXSC_CAMKEY_OFFSET_OF(sci_regs[rxCAMIdx].sci_hi_0), 0xFFFFFFFFUL);
        }
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_RXSC_CAMKEY_OFFSET_OF(sci_regs[rxCAMIdx].ctrl_0),
                (aVportIdx << MACSEC_I_RXSC_CAMKEY_RXSC_CAM_CTRL_10_VPORT_INDEX_SHIFT));
        /* Enable the RXCAM entry */
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_RXSC_CAMEN_OFFSET_OF(cam_enable_ctrl),
                (rxCAMIdx << MACSEC_I_RXSC_CAMEN_RXSC_CAM_ENABLE_CTRL_SC_INDEX_SET_SHIFT) |
                MACSEC_I_RXSC_CAMEN_RXSC_CAM_ENABLE_CTRL_SET_ENABLE_MASK);
        *aRxSCIdx = rxCAMIdx;
    } else {
        ret = BCM_ERR_NOMEM;
    }
    return ret;
}

/**
    @brief Enable/Disable SA entry in hardware

    @code{.unparsed}

    uint32_t map
    if MACSEC_DIR_EGRESS == aDir
        if TRUE == aInUse
            MACSEC_E_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].m0 =
                (aSAIdx << MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_INDEX0_SHIFT) |
                MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_UPDATE_TIME0_MASK |
                MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_IN_USE0_MASK
        else
            MACSEC_E_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].m0 &=
                ~MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_IN_USE0_MASK
    else
        switch (aAN)
            case 0:
                if (uint32_t)BCM_BOOL_TRUE == aInUse
                    map = MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map1_0
                    map &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_INDEX0_MASK
                    map |= (aSAIdx << MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_INDEX0_SHIFT)
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_UPDATE_TIME0_MASK
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_IN_USE0_MASK
                    MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map1_0 = map
                else
                    MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map1_0 &=
                        ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_IN_USE0_MASK
                break
            case 1:
                if (uint32_t)BCM_BOOL_TRUE == aInUse
                    map = MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map1_0
                    map &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_INDEX1_MASK
                    map |= (aSAIdx << MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_INDEX1_SHIFT)
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_UPDATE_TIME1_MASK
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_IN_USE1_MASK
                    MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map1_0 = map
                else
                    MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map1_0 &=
                        ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_IN_USE1_MASK
                break
            case 2:
                if (uint32_t)BCM_BOOL_TRUE == aInUse
                    map = MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map2_0
                    map &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_INDEX2_MASK
                    map |= (aSAIdx << MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_INDEX2_SHIFT)
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_UPDATE_TIME2_MASK
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_IN_USE2_MASK
                    MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map2_0 = map
                else
                    MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map2_0 &=
                        ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_IN_USE2_MASK
                break
            case 3:
                if (uint32_t)BCM_BOOL_TRUE == aInUse) {
                    map = MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map2_0;
                    map &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_INDEX3_MASK;
                    map |= (aSAIdx << MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_INDEX3_SHIFT);
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_UPDATE_TIME3_MASK;
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_IN_USE3_MASK;

                    MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map2_0 = map;
                else
                    MACSEC_I_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].map2_0 &=
                        ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_IN_USE3_MASK
                break
            default:
                BCM_ASSERT(0U)
                break
    return MACSEC_DrvInFlightSync(aConnHdl, aID);
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static int32_t MACSEC_DrvSASetInUse(BCM_HandleType aConnHdl, MACSEC_HwIDType aID,
                                    uint32_t aSCIdx, uint32_t aInUse, uint32_t aSAIdx,
                                    MACSEC_ANType aAN, MACSEC_DirType aDir)
{
    uint32_t map;
    uint32_t data;

    if (MACSEC_DIR_EGRESS == aDir) {
        if ((uint32_t)BCM_BOOL_TRUE == aInUse) {
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].m0),
                    (aSAIdx << MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_INDEX0_SHIFT) |
                    MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_UPDATE_TIME0_MASK |
                    MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_IN_USE0_MASK);
        } else {
            data = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].m0));
            data &= ~MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_IN_USE0_MASK;
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].m0), data);
        }
    } else {
        switch (aAN) {
            case 0:
                if ((uint32_t)BCM_BOOL_TRUE == aInUse) {
                    map = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map1_0));
                    map &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_INDEX0_MASK;
                    map |= (aSAIdx << MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_INDEX0_SHIFT);
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_UPDATE_TIME0_MASK;
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_IN_USE0_MASK;

                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map1_0), map);
                } else {
                    data = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map1_0));
                    data &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_IN_USE0_MASK;
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map1_0), data);
                }
                break;
            case 1:
                if ((uint32_t)BCM_BOOL_TRUE == aInUse) {
                    map = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map1_0));
                    map &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_INDEX1_MASK;
                    map |= (aSAIdx << MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_INDEX1_SHIFT);
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_UPDATE_TIME1_MASK;
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_IN_USE1_MASK;

                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map1_0), map);
                } else {
                    data = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map1_0));
                    data &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP1_0_SA_IN_USE1_MASK;
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map1_0), data);
                }
                break;
            case 2:
                if ((uint32_t)BCM_BOOL_TRUE == aInUse) {
                    /* @event         misra_c_2012_rule_violation, zero_deref
                       @issue         Explicit null dereferenced (FORWARD_NULL)
                       @risk          Undefined behaviour
                       @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                    /* coverity[misra_c_2012_rule_violation] */
                    /* coverity[zero_deref] */
                    map = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map2_0));
                    map &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_INDEX2_MASK;
                    map |= (aSAIdx << MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_INDEX2_SHIFT);
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_UPDATE_TIME2_MASK;
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_IN_USE2_MASK;

                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map2_0), map);
                } else {
                    data = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map2_0));
                    data &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_IN_USE2_MASK;
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map2_0), data);
                }
                break;
            case 3:
                if ((uint32_t)BCM_BOOL_TRUE == aInUse) {
                    map = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map2_0));
                    map &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_INDEX3_MASK;
                    map |= (aSAIdx << MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_INDEX3_SHIFT);
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_UPDATE_TIME3_MASK;
                    map |= MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_IN_USE3_MASK;

                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map2_0), map);
                } else {
                    data = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map2_0));
                    data &= ~MACSEC_I_SCSA_MAP_SAM_SC_SA_MAP2_0_SA_IN_USE3_MASK;
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].map2_0), data);
                }
                break;
            default:
                BCM_ASSERT(0UL == (uint32_t)&aConnHdl);
                break;
        }
    }
    /* perform in-flight sync */
    return MACSEC_DrvInFlightSync(aConnHdl, aID);
}

/**
    @brief Transmit SA switching

    @code{.unparsed}
    MACSEC_XRecSAUpdCtrlType saUpd
    uint32_t saMap
    int32_t ret = BCM_ERR_OK

    saUpd = (aNew->saIdx << MACSEC_XRECSAUPDCTRL_SA_INDEX_SHIFT)
    saUpd |= MACSEC_XRECSAUPDCTRL_SA_IDX_VALID_MASK
    saUpd |= MACSEC_XRECSAUPDCTRL_UPDATE_TIME_MASK
    MACSEC_DrvXRecUpdate(aConnHdl, aID, aOld->saIdx,
                         aOld->xRecSAUpdCtrlOff >> 2UL,
                         1UL, &saUpd, MACSEC_DIR_EGRESS)
    MACSEC_E_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].m0 =
        ((uint32_t)aNew->saIdx << MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_INDEX0_SHIFT) |
         MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_INDEX_UPDATE0_MASK |
         MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_UPDATE_TIME0_MASK

    ret = MACSEC_DrvInFlightSync(aConnHdl, aID)
    if BCM_ERR_OK == ret
        ret = BCM_ERR_UNKNOWN
        saMap = MACSEC_E_SCSA_MAP_REGS[aID]->scsamap_regs[aSCIdx].m0
        if ((saMap & MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_INDEX0_MASK) == aNew->saIdx) &&
            ((saMap & MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_IN_USE0_MASK) != 0UL)
            ret = BCM_ERR_OK
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static int32_t MACSEC_DrvEgressSASwitch(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint32_t aSCIdx,
                                        MACSEC_SAContextType *const aOld,
                                        MACSEC_SAContextType *const aNew)
{
    MACSEC_XRecSAUpdCtrlType saUpd;
    uint32_t saMap;
    int32_t ret = BCM_ERR_OK;

    saUpd = ((uint32_t)aNew->saIdx << MACSEC_XRECSAUPDCTRL_SA_INDEX_SHIFT);
    saUpd |= MACSEC_XRECSAUPDCTRL_SA_IDX_VALID_MASK;
    saUpd |= MACSEC_XRECSAUPDCTRL_UPDATE_TIME_MASK;
    MACSEC_DrvXRecUpdate(aConnHdl, (uint32_t)aID, (uint32_t)aOld->saIdx,
                         (uint32_t)(aOld->xRecSAUpdCtrlOff) >> 2U,
                         1UL, &saUpd, MACSEC_DIR_EGRESS);
    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].m0),
        ((uint32_t)aNew->saIdx << MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_INDEX0_SHIFT) |
         MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_INDEX_UPDATE0_MASK |
         MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_UPDATE_TIME0_MASK);

    ret = MACSEC_DrvInFlightSync(aConnHdl, aID);
    if (BCM_ERR_OK == ret) {
        ret = BCM_ERR_UNKNOWN;
        /* @event         misra_c_2012_rule_violation, zero_deref
           @issue         Explicit null dereferenced (FORWARD_NULL)
           @risk          Undefined behaviour
           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
        /* coverity[misra_c_2012_rule_violation] */
        /* coverity[zero_deref] */
        saMap = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SCSA_MAP_OFFSET_OF(scsamap_regs[aSCIdx].m0));
        if (((saMap & MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_INDEX0_MASK) == aNew->saIdx) &&
            ((saMap & MACSEC_E_SCSA_MAP_SAM_SC_SA_MAP1_10_SA_IN_USE0_MASK) != 0UL)) {
            ret = BCM_ERR_OK;
        }
    }
    return ret;
}

/**
    @brief check if the current cipher suite for secy is xpn

    @code{.unparsed}

    if secy->cipher == XPN128 or secy->cipher == XPN256
        xpn = (uint32_t)BCM_BOOL_TRUE
    else
        xpn = (uint32_t)BCM_BOOL_FALSE

    return xpn
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static BCM_BoolType MACSEC_DrvIsXpnCipherSuiteSet(MACSEC_SecYContextType *const aSecY)
{
    BCM_BoolType xpn = BCM_BOOL_FALSE;

    if ((SECY_CIPHER_GCM_AES_XPN_128 == aSecY->cipherID) ||
        (SECY_CIPHER_GCM_AES_XPN_256 == aSecY->cipherID)) {
        xpn = BCM_BOOL_TRUE;
    }
    return xpn;
}

/**
    @brief Enable/Disable frame protection

    This API sets/resets the "protectFrame" bit in the egress flow
    control register

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_HandleInfoType info
    int32_t ret = BCM_ERR_OK

    MACSEC_HANDLE_INFO_INIT(info)
    secy = MACSEC_DrvHandleToSecY(aSecYHdl)
    if NULL != secy
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
        if (secy->validateFrame == SECY_VALIDATE_FRAME_NULL) && ((uint32_t)BCM_BOOL_TRUE == aEnable)
            ret = BCM_ERR_INVAL_PARAMS
        else
            flowcntrl = MACSEC_Read(..., MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[secy->txVportIdx].ctrl1_0)
            if aEnable == (uint32_t)BCM_BOOL_TRUE
                flowcntrl |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_PROTECT_FRAME_MASK
            else
                flowcntrl &= ~MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_PROTECT_FRAME_MASK

            MACSEC_Write(..., MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[secy->txVportIdx].ctrl1_0, flowcntrl)
            secy->protectFrame = aEnable
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PROTECT_FRAME_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

int32_t MACSEC_DrvSetProtectFrame(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl,
                                                                 BCM_BoolType aEnable)
{
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    uint32_t flowCntrl0;
    int32_t ret = BCM_ERR_OK;

    MACSEC_HANDLE_INFO_INIT(info);
    secy = MACSEC_DrvHandleToSecY(aSecYHdl);
    if (NULL != secy) {
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        if ((secy->validateFrame == SECY_VALIDATE_FRAME_NULL) && (BCM_BOOL_TRUE == aEnable)) {
            ret = BCM_ERR_INVAL_PARAMS;
        } else {
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            flowCntrl0 = MACSEC_Read(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE,
                            MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[secy->txVportIdx].ctrl1_0));
            if (BCM_BOOL_TRUE == aEnable) {
                flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_PROTECT_FRAME_MASK;
            } else {
                flowCntrl0 &= ~MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_PROTECT_FRAME_MASK;
            }
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE,
                MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[secy->txVportIdx].ctrl1_0), flowCntrl0);
            secy->protectFrame = (uint32_t)aEnable;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief Set Frame validate in hardware

    @code{.unparsed}

    MACSEC_SecYContextType *secy
    MACSEC_HandleInfoType info
    int32_t ret = BCM_ERR_OK

    secy = MACSEC_DrvHandleToSecY(aSecYHdl)
    if NULL != secy
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
        if aValidateFrame <= SECY_VALIDATE_FRAME_NULL
            flowcntrl0 = MACSEC_Read(MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0)
            if aValidateFram == VALIDATE_FRAME_NULL
                ret = MACSEC_DrvSetProtectFrame(..., FALSE)
                if ret == ERR_OK
                    flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_RETAIN_ICV_MASK
                    flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_RETAIN_SECTAG_MASK
                    flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_UNTAGGED_MASK
                    flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_MASK
                    flowCntrl0 |= (aValidateFrame) << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_SHIFT
                iterate over all the RX SCs and SAs and update the update the transform record control word to retain ICV and SecTAG
                    Loop i = 0 to MAX_RXSC_PER_SECY
                        if secy->rxSC[i].active == (uint32_t)BCM_BOOL_TRUE
                            Loop an = 0 to 4
                                if secy->rxSC[i].sa[an].handle != 0 and sa[an].inUse == FALSE
                                    MACSEC_DrvXRecRead(..., secy->rxSC[i].sa[an].saIdx, 0, 1U,
                                        &cntrlW, MACSEC_DIR_INGRESS)
                                    cntrlW |= MACSEC_XRECW0_KEEP_SECTAG_MASK
                                    cntrlW |= MACSEC_XRECW0_KEEP_ICV_MASK
                                    MACSEC_DrvXRecUpdate(..., secy->rxSC[i].sa[an].saIdx, 0, 1U,
                                        &cntrlW, DIR_INGRESS)
                    MACSEC_Write(MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0, flowcntrl0)
                    secy->validateFrame = aValidateFrame
            else
                if aValidateFrame == STRICT
                    flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_UNTAGGED_MASK
                else
                    flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_UNTAGGED_MASK
                flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_MASK
                flowCntrl0 |= (aValidateFrame) << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_SHIFT
                MACSEC_Write(MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0, flowcntrl0)
                secy->validateFrame = aValidateFrame
        else
            ret = ERR_INVAL_PARAMS
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SET_FRAME_VALIDATE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvSetFrameValidate(BCM_HandleType aHandle, MACSEC_SecYHdlType aSecYHdl,
                                                       SECY_ValidateFrameType aValidateFrame)

{
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    uint32_t flowCntrl0 = 0UL;
    uint32_t saCntrlW = 0UL;
    int      i;
    uint32_t an;
    int32_t ret = BCM_ERR_OK;

    MACSEC_HANDLE_INFO_INIT(info);
    secy = MACSEC_DrvHandleToSecY(aSecYHdl);
    if (NULL != secy) {
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        if (aValidateFrame <= SECY_VALIDATE_FRAME_NULL) {
            /* @event         misra_c_2012_rule_violation, zero_deref
                @issue         Explicit null dereferenced (FORWARD_NULL)
                @risk          Undefined behaviour
                @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            flowCntrl0 = MACSEC_Read(aHandle, info.hwIdx, MACSEC_BLOCK_CORE,
                            MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0));
            if (SECY_VALIDATE_FRAME_NULL == aValidateFrame) {
                /* Setting validateFrame to NULL also causes
                   secure frame generation contol to become false (thus allowing the port to
                   behave as if no SecY is present)
                */
                ret = MACSEC_DrvSetProtectFrame(aHandle, aSecYHdl, BCM_BOOL_FALSE);
                if (BCM_ERR_OK == ret) {
                    flowCntrl0 |=  MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_RETAIN_ICV_MASK;
                    flowCntrl0 |=  MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_RETAIN_SECTAG_MASK;
                    flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_UNTAGGED_MASK;
                    flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_MASK;
                    flowCntrl0 |= (aValidateFrame) << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_SHIFT;
                    for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                        if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active) {
                            for (an = 0UL; an <= MACSEC_AN_MAX; an++) {
                                if ((0ULL != secy->rxSC[i].sa[an].handle) &&
                                    ((uint32_t)BCM_BOOL_FALSE == secy->rxSC[i].sa[an].inUse)) {
                                    /* Read SA control word from transform record and set the
                                       bits to retain the SecTAG and ICV both */
                                    MACSEC_DrvXRecRead(aHandle, (uint32_t)info.hwIdx, (uint32_t)secy->rxSC[i].sa[an].saIdx,
                                                       0UL, 1UL, &saCntrlW, MACSEC_DIR_INGRESS);
                                    saCntrlW |= MACSEC_XRECW0_KEEP_SECTAG_MASK;
                                    saCntrlW |= MACSEC_XRECW0_KEEP_ICV_MASK;
                                    MACSEC_DrvXRecUpdate(aHandle, (uint32_t)info.hwIdx,
                                                         (uint32_t)secy->rxSC[i].sa[an].saIdx, 0UL, 1UL, &saCntrlW,
                                                         MACSEC_DIR_INGRESS);
                                }
                            }
                        }
                    }
                    /* @event         misra_c_2012_rule_violation, zero_deref
                       @issue         Explicit null dereferenced (FORWARD_NULL)
                       @risk          Undefined behaviour
                       @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                    /* coverity[misra_c_2012_rule_violation] */
                    /* coverity[zero_deref] */
                    MACSEC_Write(aHandle, info.hwIdx, MACSEC_BLOCK_CORE,
                                 MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0), flowCntrl0);
                    secy->validateFrame = aValidateFrame;
                }
            } else {
                if (SECY_VALIDATE_FRAME_STRICT == aValidateFrame) {
                    flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_UNTAGGED_MASK;
                } else {
                    flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_UNTAGGED_MASK;
                }
                flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_MASK;
                flowCntrl0 |= (aValidateFrame) << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_SHIFT;
                /* @event         misra_c_2012_rule_violation, zero_deref
                   @issue         Explicit null dereferenced (FORWARD_NULL)
                   @risk          Undefined behaviour
                   @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                /* coverity[misra_c_2012_rule_violation] */
                /* coverity[zero_deref] */
                MACSEC_Write(aHandle, info.hwIdx, MACSEC_BLOCK_CORE,
                             MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0), flowCntrl0);
                secy->validateFrame = aValidateFrame;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief Enable/Disable replay protection

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_HandleInfoType info
    int32_t ret = BCM_ERR_INVAL_PARAMS

    MACSEC_HANDLE_INFO_INIT(info)
    secy = MACSEC_DrvHandleToSecY(aSecYHdl)
    if NULL != secy
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
        BCM_ASSERT(BCM_ERR_OK == ret)
        if (uint32_t)BCM_BOOL_TRUE == aEnable
            if ((secy->cipher == XPN128) || (secy->cipher == XPN256) && (aWindow <= (MACSEC_MAX_REPLAY_WINDOW >> 1UL)) ||
                (aWindow <= MACSEC_MAX_REPLAY_WINDOW)
                secy->replayWindow = aWindow;
                secy->replayProtect = (uint32_t)BCM_BOOL_TRUE;
                ret = BCM_ERR_OK;
            else
                if aWindow <= 0xFFFFFFFF
                    secy->replayProtect = FALSE
                    secy->replayWindow = 0UL
                    ret = BCM_ERR_OK
        else
            secy->replayProtect = (uint32_t)BCM_BOOL_FALSE
            ret = ERR_OK

        if ret == ERR_OK
            flowCntrl0 = MACSEC_Read(MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0))
            if aEnable == (uint32_t)BCM_BOOL_TRUE
                flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_REPLAY_PROTECT_MASK
            Loop i = 0 to i < MAX_RXSC_PER_SECY
                if secy->rxSC[i] == (uint32_t)BCM_BOOL_TRUE
                    Loop an = 0 to 4
                        if secy->rxSC[i].sa[an].inUse == (uint32_t)BCM_BOOL_FALSE
                            MACSEC_DrvXRecUpdate(secy->rxSC[i].sa[an].saIdx, secy->rxSC[i].sa[an].xRecReplayWindowOff >> 2UL,
                                1UL, &secy->replayWindow, DIR_INGRESS);
                        else
                            LOG_ERR("SA in use. Can not update the replay window")
            else
                flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_REPLAY_PROTECT_MASK

            MACSEC_Write(..., MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0, flowCntrl0))

    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SET_REPLAY_PROTECT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvSetReplayProtect(BCM_HandleType aHandle, MACSEC_SecYHdlType aSecYHdl,
                                                  uint32_t aEnable , uint32_t aWindow )
{
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    uint32_t flowCntrl0 = 0UL;
    int      i;
    uint32_t an;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    MACSEC_HANDLE_INFO_INIT(info);
    secy = MACSEC_DrvHandleToSecY(aSecYHdl);
    if (NULL != secy) {
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        if ((uint32_t)BCM_BOOL_TRUE == aEnable) {
            /* max replay window for 32-bit PN is 2^32-1 and for 64-bit PN max window allowed is
               2^32-1 (replay window shall not cross half the range of PN) */
            if (BCM_BOOL_TRUE == MACSEC_DrvIsXpnCipherSuiteSet(secy)) {
                if (aWindow <= (MACSEC_MAX_REPLAY_WINDOW >> 1UL)) {
                    secy->replayWindow = aWindow;
                    secy->replayProtect = (uint32_t)BCM_BOOL_TRUE;
                    ret = BCM_ERR_OK;
                }
            } else {
                    secy->replayWindow = aWindow;
                    secy->replayProtect = (uint32_t)BCM_BOOL_TRUE;
                    ret = BCM_ERR_OK;
            }
        } else {
            secy->replayProtect = (uint32_t)BCM_BOOL_FALSE;
            secy->replayWindow = 0UL;
            ret = BCM_ERR_OK;
        }
        if (BCM_ERR_OK == ret) {
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            flowCntrl0 = MACSEC_Read(aHandle, info.hwIdx, MACSEC_BLOCK_CORE,
                            MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0));
            if ((uint32_t)BCM_BOOL_TRUE == aEnable) {
                flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_REPLAY_PROTECT_MASK;
                /* Replay window configuration is part of the XREC which is created only
                   when SA is created. Iterate through all the SAs for all the active SCs
                   and update the window if that SA is not in use (created but inUse flag is
                   not set). If there are no SAs created, just cache the window size and
                   use it later when SAs are created */
                for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                    if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active) {
                        for (an = 0UL; an <= MACSEC_AN_MAX; an++) {
                            if (0ULL != secy->rxSC[i].sa[an].handle) {
                                if ((uint32_t)BCM_BOOL_FALSE == secy->rxSC[i].sa[an].inUse) {
                                    MACSEC_DrvXRecUpdate(aHandle, (uint32_t)(info.hwIdx),
                                                         (uint32_t)(secy->rxSC[i].sa[an].saIdx),
                                                         (uint32_t)(secy->rxSC[i].sa[an].xRecReplayWindowOff) >> 2U,
                                                         1UL, &secy->replayWindow, MACSEC_DIR_INGRESS);
                                } else {
                                    BCM_LOG_ERROR(0U, BCM_STATE_RUN, ret, "Can not set replay window on active SA.\
                                        port = %d, an = %d", info.hwIdx, an);
                                }
                            }
                        }
                    }
                }
            } else {
                flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_REPLAY_PROTECT_MASK;
            }
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            MACSEC_Write(aHandle, info.hwIdx, MACSEC_BLOCK_CORE,
                         MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0),
                         flowCntrl0);
        }
    }
    return ret;
}

/**
    @brief Gets SecY stats information from hardware

    @code{.unparsed}
    MACSEC_E_SECY_CNTRS_CNTRSRDBType *eSecY
    MACSEC_I_SECY_CNTRS_CNTRSRDBType *iSecY
    uint32_t lo
    uint32_t hi

    if MACSEC_DIR_EGRESS == aDir
        eSecY = &MACSEC_E_SECY_CNTRS_REGS[aID]->regs[aSecYIdx]
        if NULL != aStats
            lo = eSecY->m0_outpktsuntagged_lo
            hi = eSecY->m0_outpktsuntagged_hi
            aStats->pktsUntagged = lo | ((uint64_t)hi << 32UL)
        if TRUE == aClear
            eSecY->m0_outpktsuntagged_lo = 0UL
            eSecY->m0_outpktsuntagged_hi = 0UL
    else
        iSecY = &MACSEC_I_SECY_CNTRS_REGS[aID]->regs[aSecYIdx]
        if NULL != aStats
            lo = iSecY->m0_inpktsuntagged_lo
            hi = iSecY->m0_inpktsuntagged_hi
            aStats->pktsUntagged = lo | ((uint64_t)hi << 32UL)

            lo = iSecY->m0_inpktsnotag_lo
            hi = iSecY->m0_inpktsnotag_hi
            aStats->pktsNoTag = lo | ((uint64_t)hi << 32UL)

            lo = iSecY->m0_inpktsbadtag_lo
            hi = iSecY->m0_inpktsbadtag_hi
            aStats->pktsBadTag = lo | ((uint64_t)hi << 32UL)

            lo = iSecY->m0_inpktsnosci_lo
            hi = iSecY->m0_inpktsnosci_hi
            aStats->pktsNoSCI = lo | ((uint64_t)hi << 32UL)

            lo = iSecY->m0_inpktsunknownsci_lo
            hi = iSecY->m0_inpktsunknownsci_hi
            aStats->pktsUnknownSCI = lo | ((uint64_t)hi << 32UL)
        }
        if TRUE == aClear
            iSecY->m0_inpktsuntagged_lo = 0UL
            iSecY->m0_inpktsuntagged_hi = 0UL
            iSecY->m0_inpktsnotag_lo = 0UL
            iSecY->m0_inpktsnotag_hi = 0UL
            iSecY->m0_inpktsbadtag_lo = 0UL
            iSecY->m0_inpktsbadtag_hi = 0UL
            iSecY->m0_inpktsnosci_lo = 0UL
            iSecY->m0_inpktsnosci_hi = 0UL
            iSecY->m0_inpktsunknownsci_lo = 0UL
            iSecY->m0_inpktsunknownsci_hi = 0UL
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

static void MACSEC_DrvGetSecYStatsInternal(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint32_t aSecYIdx,
                                    MACSEC_EgIgSecYStatsType *const aStats,
                                    MACSEC_DirType aDir, uint32_t aClear)
{
    uint32_t lo;
    uint32_t hi;

    if (MACSEC_DIR_EGRESS == aDir) {
        if (NULL != aStats) {

            lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_outpktsuntagged_lo));
            hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_outpktsuntagged_hi));

            aStats->pktsUntagged = lo | ((uint64_t)hi << 32UL);
        }
        if ((uint32_t)BCM_BOOL_TRUE == aClear) {

            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_outpktsuntagged_lo), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_outpktsuntagged_hi), 0UL);
        }
    } else {
        if (NULL != aStats) {
            lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsuntagged_lo));
            hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsuntagged_hi));

            aStats->pktsUntagged = lo | ((uint64_t)hi << 32UL);

            lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsnotag_lo));
            hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsnotag_hi));
            aStats->pktsNoTag = lo | ((uint64_t)hi << 32UL);

            lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsbadtag_lo));
            hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsbadtag_hi));
            aStats->pktsBadTag = lo | ((uint64_t)hi << 32UL);

            lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsnosci_lo));
            hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsnosci_hi));
            aStats->pktsNoSCI = lo | ((uint64_t)hi << 32UL);

            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsunknownsci_lo));
            hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsunknownsci_hi));
            aStats->pktsUnknownSCI = lo | ((uint64_t)hi << 32UL);
        }
        if ((uint32_t)BCM_BOOL_TRUE == aClear) {
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsuntagged_lo), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsuntagged_hi), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsnotag_lo), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsnotag_hi), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsbadtag_lo), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsbadtag_hi), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsnosci_lo), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsnosci_hi), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsunknownsci_lo), 0UL);
            MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SECY_CNTRS_OFFSET_OF(regs[aSecYIdx].m0_inpktsunknownsci_hi), 0UL);
        }
    }
}

/**
    @brief Get and/or clears Egress SA statistics

    @code{.unparsed}
    MACSEC_E_SA_CNTRS_CNTRSRDBType *eSA
    uint32_t lo
    uint32_t hi

    eSA = &MACSEC_E_SA_CNTRS_REGS[aID]->regs[aSAIdx]
    lo = eSA->m0_outoctetsencryptedprotected_lo
    hi = eSA->m0_outoctetsencryptedprotected_hi
    aStats->octetsEncryptedProtected = lo | ((uint64_t)hi << 32UL)
    lo = eSA->m0_outpktsencryptedprotected_lo
    hi = eSA->m0_outpktsencryptedprotected_hi
    aStats->pktsEncryptedProtected = lo | ((uint64_t)hi << 32UL)
    lo = eSA->m0_outpktstoolong_lo
    hi = eSA->m0_outpktstoolong_hi
    aStats->pktsTooLong = lo | ((uint64_t)hi << 32UL)
    lo = eSA->m0_outpktssanotinuse_lo
    hi = eSA->m0_outpktssanotinuse_hi
    aStats->pktsSANotInUse = lo | ((uint64_t)hi << 32UL)

    if (uint32_t)BCM_BOOL_TRUE == aClear
        eSA->m0_outoctetsencryptedprotected_lo = 0UL
        eSA->m0_outoctetsencryptedprotected_hi = 0UL
        eSA->m0_outpktsencryptedprotected_lo = 0UL
        eSA->m0_outpktsencryptedprotected_hi = 0UL
        eSA->m0_outpktstoolong_lo = 0UL
        eSA->m0_outpktstoolong_hi = 0UL
        eSA->m0_outpktssanotinuse_lo = 0UL
        eSA->m0_outpktssanotinuse_hi = 0UL
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

static void MACSEC_DrvGetEgSAStats(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint32_t aSAIdx,
                                   MACSEC_EgSAStatsType *const aStats, uint32_t aClear)
{
    uint32_t lo;
    uint32_t hi;


    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outoctetsencryptedprotected_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outoctetsencryptedprotected_hi));
    aStats->octetsEncryptedProtected = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktsencryptedprotected_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktsencryptedprotected_hi));
    aStats->pktsEncryptedProtected = lo | ((uint64_t)hi << 32UL);

    /* @event         misra_c_2012_rule_violation, zero_deref
       @issue         Explicit null dereferenced (FORWARD_NULL)
       @risk          Undefined behaviour
       @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
    /* coverity[misra_c_2012_rule_violation] */
    /* coverity[zero_deref] */
    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktstoolong_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktstoolong_hi));
    aStats->pktsTooLong = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktssanotinuse_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktssanotinuse_hi));
    aStats->pktsSANotInUse = lo | ((uint64_t)hi << 32UL);

    if ((uint32_t)BCM_BOOL_TRUE == aClear) {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outoctetsencryptedprotected_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outoctetsencryptedprotected_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktsencryptedprotected_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktsencryptedprotected_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktstoolong_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktstoolong_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktssanotinuse_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_outpktssanotinuse_hi), 0UL);
    }
}

/**
    @brief Get and/or clears Ingress SA statistics

    @code{.unparsed}
    MACSEC_I_SA_CNTRS_CNTRSRDBType *iSA
    uint32_t lo
    uint32_t hi

    iSA = &MACSEC_I_SA_CNTRS_REGS[aID]->regs[aSAIdx]
    lo = iSA->m0_inoctetsdecrypted_lo
    hi = iSA->m0_inoctetsdecrypted_hi
    aStats->octetsDecrypted = lo | ((uint64_t)hi << 32UL)
    lo = iSA->m0_inoctetsvalidated_lo
    hi = iSA->m0_inoctetsvalidated_hi
    aStats->octetsValidated = lo | ((uint64_t)hi << 32UL)
    lo = iSA->m0_inpktslate_lo
    hi = iSA->m0_inpktslate_hi
    aStats->pktsLate = lo | hi << 32UL
    lo = iSA->m0_inpktsdelayed_lo
    hi = iSA->m0_inpktsdelayed_hi
    aStats->pktsDelayed = lo | ((uint64_t)hi << 32UL)
    lo = iSA->m0_inpktsinvalid_lo
    hi = iSA->m0_inpktsinvalid_hi
    aStats->pktsInvalid = lo | ((uint64_t)hi << 32UL)
    lo = iSA->m0_inpktsnotvalid_lo
    hi = iSA->m0_inpktsnotvalid_hi
    aStats->pktsNotValid = lo | ((uint64_t)hi << 32UL)
    lo = iSA->m0_inpktsunchecked_lo
    hi = iSA->m0_inpktsunchecked_hi
    aStats->pktsUnchecked = lo | ((uint64_t)hi << 32UL)
    lo = iSA->m0_inpktsnotusingsa_lo
    hi = iSA->m0_inpktsnotusingsa_hi
    aStats->pktsNotUsingSA = lo | ((uint64_t)hi << 32UL)
    lo = iSA->m0_inpktsunusedsa_lo
    hi = iSA->m0_inpktsunusedsa_hi
    aStats->pktsUnusedSA = lo | ((uint64_t)hi << 32UL)
    lo = iSA->m0_inpktsok_lo
    hi = iSA->m0_inpktsok_hi
    aStats->pktsOK = lo | ((uint64_t)hi << 32UL)
    if (uint32_t)BCM_BOOL_TRUE == aClear
        iSA->m0_inoctetsdecrypted_lo = 0UL
        iSA->m0_inoctetsdecrypted_hi = 0UL
        iSA->m0_inoctetsvalidated_lo = 0UL
        iSA->m0_inoctetsvalidated_hi = 0UL
        iSA->m0_inpktslate_lo = 0UL
        iSA->m0_inpktslate_hi = 0UL
        iSA->m0_inpktsdelayed_lo = 0UL
        iSA->m0_inpktsdelayed_hi = 0UL
        iSA->m0_inpktsinvalid_lo = 0UL
        iSA->m0_inpktsinvalid_hi = 0UL
        iSA->m0_inpktsnotvalid_lo = 0U
        iSA->m0_inpktsnotvalid_hi = 0UL
        iSA->m0_inpktsunchecked_lo = 0UL
        iSA->m0_inpktsunchecked_hi = 0UL
        iSA->m0_inpktsnotusingsa_lo = 0UL
        iSA->m0_inpktsnotusingsa_hi = 0UL
        iSA->m0_inpktsunusedsa_lo = 0UL
        iSA->m0_inpktsunusedsa_hi = 0UL
        iSA->m0_inpktsok_lo = 0UL
        iSA->m0_inpktsok_hi = 0UL
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

static void MACSEC_DrvGetIgSAStats(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint32_t aSAIdx,
                            MACSEC_IgSAStatsType *const aStats, uint32_t aClear)
{
    uint32_t lo;
    uint32_t hi;


    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inoctetsdecrypted_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inoctetsdecrypted_hi));
    aStats->octetsDecrypted = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inoctetsvalidated_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inoctetsvalidated_hi));
    aStats->octetsValidated = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktslate_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktslate_hi));
    aStats->pktsLate = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsdelayed_lo));
    /* @event         misra_c_2012_rule_violation, zero_deref
       @issue         Explicit null dereferenced (FORWARD_NULL)
       @risk          Undefined behaviour
       @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
    /* coverity[misra_c_2012_rule_violation] */
    /* coverity[zero_deref] */
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsdelayed_hi));
    aStats->pktsDelayed = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsinvalid_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsinvalid_hi));
    aStats->pktsInvalid = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsnotvalid_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsnotvalid_hi));
    aStats->pktsNotValid = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsunchecked_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsunchecked_hi));
    aStats->pktsUnchecked = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsnotusingsa_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsnotusingsa_hi));
    aStats->pktsNotUsingSA = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsunusedsa_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsunusedsa_hi));
    aStats->pktsUnusedSA = lo | ((uint64_t)hi << 32UL);

    lo = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsok_lo));
    hi = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsok_hi));
    aStats->pktsOK = lo | ((uint64_t)hi << 32UL);
    if ((uint32_t)BCM_BOOL_TRUE == aClear) {
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inoctetsdecrypted_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inoctetsdecrypted_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inoctetsvalidated_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inoctetsvalidated_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktslate_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktslate_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsdelayed_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsdelayed_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsinvalid_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsinvalid_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsnotvalid_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsnotvalid_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsunchecked_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsunchecked_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsnotusingsa_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsnotusingsa_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsunusedsa_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsunusedsa_hi), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsok_lo), 0UL);
        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_I_SA_CNTRS_OFFSET_OF(regs[aSAIdx].m0_inpktsok_hi), 0UL);
    }
}

/**
    @brief Get Rx SC statistics

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_HandleInfoType info
    MACSEC_RxSCContextType *sc
    MACSEC_IgSAStatsType stats
    uint32_t i
    uint32_t j
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if (NULL != aSCI) && (NULL != aStats)
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            for i = 0UL; i < MACSEC_MAX_RXSC_PER_SECY; i++
                if (uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active
                    if BCM_MemCmp(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType)) == 0U
                        sc = &secy->rxSC[i]
                        break

            if (i < MACSEC_MAX_RXSC_PER_SECY) {
                ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
                BCM_ASSERT(BCM_ERR_OK == ret)
                BCM_MemSet(aStats, 0x0U, sizeof(SECY_RxSCStatsType))
                for j = 0UL; j <= MACSEC_AN_MAX; j++
                    if 0UL != sc->sa[j].handle
                        MACSEC_DrvGetIgSAStats(aConnHdl, info.hwIdx, sc->sa[j].saIdx, &stats, (uint32_t)BCM_BOOL_FALSE)
                        aStats->inPktsLate += stats.pktsLate
                        aStats->inPktsDelayed += stats.pktsDelayed
                        aStats->inPktsInvalid += stats.pktsInvalid
                        aStats->inPktsNotValid += stats.pktsNotValid
                        aStats->inPktsUnchecked += stats.pktsUnchecked
                        aStats->inPktsOK += stats.pktsOK

                aStats->inPktsLate += sc->stats.pktsLate
                aStats->inPktsDelayed += sc->stats.pktsDelayed
                aStats->inPktsInvalid += sc->stats.pktsInvalid
                aStats->inPktsNotValid += sc->stats.pktsNotValid
                aStats->inPktsUnchecked += sc->stats.pktsUnchecked
                aStats->inPktsOK += sc->stats.pktsOK
                ret = BCM_ERR_OK
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_GET_RX_SCSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvGetRxSCStats(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl,
                               SECY_SCIType *aSCI, SECY_RxSCStatsType *const aStats)
{
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    MACSEC_RxSCContextType *sc = NULL;
    MACSEC_IgSAStatsType stats = {0ULL};
    int     i, j;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aSCI) && (NULL != aStats)) {
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            /* is RxSC created? */
            for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active) {
                    if (BCM_MemCmp(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType)) == 0) {
                        sc = &secy->rxSC[i];
                        break;
                    }
                }
            }
            if (i < MACSEC_MAX_RXSC_PER_SECY) {
                ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
                BCM_ASSERT(BCM_ERR_OK == ret);
                BCM_MemSet(aStats, 0x0U, sizeof(SECY_RxSCStatsType));
                for (j = 0; j <= (int)MACSEC_AN_MAX; j++) {
                    if (0ULL != sc->sa[j].handle) {
                        MACSEC_DrvGetIgSAStats(aConnHdl, info.hwIdx, sc->sa[j].saIdx, &stats, (uint32_t)BCM_BOOL_FALSE);

                        /*@event CERT-C INT30
                                                issue          Coverity infers that (stats + *) can wrap
                                                risk            No risk
                                                justification It is statistics, it is ok if it can wrap around. */
                        /* coverity[cert_int30_c_violation] */
                        aStats->inPktsLate += stats.pktsLate;
                        /* coverity[cert_int30_c_violation] */
                        aStats->inPktsDelayed += stats.pktsDelayed;
                        /* coverity[cert_int30_c_violation] */
                        aStats->inPktsInvalid += stats.pktsInvalid;
                        /* coverity[cert_int30_c_violation] */
                        aStats->inPktsNotValid += stats.pktsNotValid;
                        /* coverity[cert_int30_c_violation] */
                        aStats->inPktsUnchecked += stats.pktsUnchecked;
                        /* coverity[cert_int30_c_violation] */
                        aStats->inPktsOK += stats.pktsOK;
                    }
                }
                /* coverity[cert_int30_c_violation] */
                aStats->inPktsLate += sc->stats.pktsLate;
                /* coverity[cert_int30_c_violation] */
                aStats->inPktsDelayed += sc->stats.pktsDelayed;
                /* coverity[cert_int30_c_violation] */
                aStats->inPktsInvalid += sc->stats.pktsInvalid;
                /* coverity[cert_int30_c_violation] */
                aStats->inPktsNotValid += sc->stats.pktsNotValid;
                /* coverity[cert_int30_c_violation] */
                aStats->inPktsUnchecked += sc->stats.pktsUnchecked;
                /* coverity[cert_int30_c_violation] */
                aStats->inPktsOK += sc->stats.pktsOK;
                ret = BCM_ERR_OK;
            }
        }
    }
    return ret;
}

/**
    @brief Get Tx SC statistics

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_HandleInfoType info
    MACSEC_EgSAStatsType egStats
    uint32_t i
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if (NULL != aSCI) && (NULL != aStats)
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            if BCM_MemCmp(aSCI, &secy->txSC.sci, sizeof(SECY_SCIType)) == 0UL
                MACSEC_HANDLE_INFO_INIT(info)
                ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
                BCM_ASSERT(BCM_ERR_OK == ret)
                BCM_MemSet(aStats, 0x0U, sizeof(SECY_TxSCStatsType))
                if (uint32_t)BCM_BOOL_TRUE == secy->txSC.active
                    for i = 0UL; i <= MACSEC_AN_MAX; i++
                        if 0UL != secy->txSC.sa[i].hash
                            MACSEC_DrvGetEgSAStats(aConnHdl, info.hwIdx, secy->txSC.sa[i].saIdx, &egStats,
                                                   (uint32_t)BCM_BOOL_FALSE)
                            if (uint32_t)BCM_BOOL_TRUE == secy->confProtect
                                aStats->outPktsEncrypted += egStats.pktsEncryptedProtected
                            else
                                aStats->outPktsProtected += egStats.pktsEncryptedProtected
                    if (uint32_t)BCM_BOOL_TRUE == secy->confProtect
                        aStats->outPktsEncrypted += secy->txSC.stats.pktsEncryptedProtected
                    else
                        aStats->outPktsProtected += secy->txSC.stats.pktsEncryptedProtected
                    ret = BCM_ERR_OK
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_GET_TX_SCSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvGetTxSCStats(BCM_HandleType aConnHdl , MACSEC_SecYHdlType aSecYHdl,
                               SECY_SCIType *aSCI, SECY_TxSCStatsType *const aStats)
{
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    MACSEC_EgSAStatsType egStats = {0ULL};
    uint32_t i;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aSCI) && (NULL != aStats)) {
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            if (BCM_MemCmp(aSCI, &secy->txSC.sci, sizeof(SECY_SCIType)) == 0) {
                MACSEC_HANDLE_INFO_INIT(info);
                ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
                BCM_ASSERT(BCM_ERR_OK == ret);
                BCM_MemSet(aStats, 0x0U, sizeof(SECY_TxSCStatsType));
                if ( (uint32_t)BCM_BOOL_TRUE == secy->txSC.active ) {
                    uint64_t secyTxScStsPktsEncryptProtected = secy->txSC.stats.pktsEncryptedProtected;
                    uint32_t secyConfProtect = secy->confProtect;
                    for ( i = 0UL; i <= MACSEC_AN_MAX; i++ ) {
                        if (0ULL != secy->txSC.sa[i].handle) {
                            uint64_t egStPktsEncryptProtected;
                            MACSEC_DrvGetEgSAStats(aConnHdl, info.hwIdx, secy->txSC.sa[i].saIdx,
                                                             &egStats, (uint32_t)BCM_BOOL_FALSE);
                            egStPktsEncryptProtected = egStats.pktsEncryptedProtected;
                            if ( (uint32_t)BCM_BOOL_TRUE == secyConfProtect ) {
                                /* @event      CERT-C INT30
                                                               issue       Coverity infers that (stats + *) can wrap
                                                               risk           No risk
                                                               justification It is statistics, it is ok if it can wrap around. */
                                /* coverity[cert_int30_c_violation] */
                                aStats->outPktsEncrypted += egStPktsEncryptProtected;
                            } else {
                                /* coverity[cert_int30_c_violation] */
                                aStats->outPktsProtected += egStPktsEncryptProtected;
                            }
                        }
                    }
                    if ( (uint32_t)BCM_BOOL_TRUE == secyConfProtect ) {
                        /* coverity[cert_int30_c_violation] */
                        aStats->outPktsEncrypted += secyTxScStsPktsEncryptProtected;
                    } else {
                        /* coverity[cert_int30_c_violation] */
                        aStats->outPktsProtected += secyTxScStsPktsEncryptProtected;
                    }
                    ret = BCM_ERR_OK;
                }
            }
        }
    }
    return ret;
}

/**
    @brief Get SecY stats

    @code{.unparsed}

    MACSEC_SecYContextType *secy
    MACSEC_RxSCContextType *rxSC
    MACSEC_HandleInfoType info
    MACSEC_EgIgSecYStatsType secyStats
    MACSEC_EgSAStatsType egStats
    MACSEC_IgSAStatsType igStats
    uint32_t i
    uint32_t j
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if NULL != aStats
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            MACSEC_HANDLE_INFO_INIT(info)
            ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
            BCM_ASSERT(BCM_ERR_OK == ret)
            BCM_MemSet(aStats, 0x0U, sizeof(SECY_StatsType))
            MACSEC_DrvGetSecYStatsInternal(aConnHdl, info.hwIdx, secy->txVportIdx, &secyStats,
                                           MACSEC_DIR_EGRESS, (uint32_t)BCM_BOOL_FALSE)
            aStats->outPktsTooLong = secyStats.pktsUntagged
            MACSEC_DrvGetSecYStatsInternal(aConnHdl, info.hwIdx, secy->rxVPortIdx, &secyStats,
                                           MACSEC_DIR_INGRESS, (uint32_t)BCM_BOOL_FALSE)
            aStats->inPktsUntagged = secyStats.pktsUntagged
            aStats->inPktsNoTag = secyStats.pktsNoTag
            aStats->inPktsBadTag = secyStats.pktsBadTag
            aStats->inPktsNoSAError = secyStats.pktsNoSCI
            aStats->inPktsNoSA = secyStats.pktsUnknownSCI
            if (uint32_t)BCM_BOOL_TRUE == secy->txSC.active
                for i = 0UL; i <= MACSEC_AN_MAX; i++
                    if 0UL != secy->txSC.sa[i].handle
                        MACSEC_DrvGetEgSAStats(aConnHdl, info.hwIdx, secy->txSC.sa[i].saIdx, &egStats, (uint32_t)BCM_BOOL_FALSE)
                        if (uint32_t)BCM_BOOL_TRUE == secy->confProtect
                            aStats->outOctetsEncrypted += egStats.octetsEncryptedProtected
                        else
                            aStats->outOctetsProtected += egStats.octetsEncryptedProtected
                        aStats->outPktsTooLong += egStats.pktsTooLong
                if (uint32_t)BCM_BOOL_TRUE == secy->confProtect
                    aStats->outOctetsEncrypted += secy->txSC.stats.octetsEncryptedProtected
                else
                    aStats->outOctetsProtected += secy->txSC.stats.octetsEncryptedProtected
                aStats->outPktsTooLong += secy->txSC.stats.pktsTooLong

            if (uint32_t)BCM_BOOL_TRUE == secy->confProtect
                aStats->outOctetsEncrypted += secy->stats.octetsEncryptedProtected
            else
                aStats->outOctetsProtected += secy->stats.octetsEncryptedProtected
            aStats->outPktsTooLong += secy->stats.pktsTooLong

            for i = 0UL; i < MACSEC_MAX_RXSC_PER_SECY; i++
                rxSC = &secy->rxSC[i];
                if (uint32_t)BCM_BOOL_TRUE == rxSC->active
                    for j = 0UL; j <= MACSEC_AN_MAX; j++
                        if 0UL != rxSC->sa[j].handle
                            MACSEC_DrvGetIgSAStats(aConnHdl, info.hwIdx, rxSC->sa[i].saIdx, &igStats, (uint32_t)BCM_BOOL_FALSE)
                            aStats->inOctetsDecrypted += igStats.octetsDecrypted
                            aStats->inOctetsValidated += igStats.octetsValidated
                            aStats->inPktsNoSA += igStats.pktsUnusedSA
                            aStats->inPktsNoSAError += igStats.pktsNotUsingSA
                aStats->inOctetsDecrypted += rxSC->stats.octetsDecrypted
                aStats->inOctetsValidated += rxSC->stats.octetsValidated
                aStats->inPktsNoSA += rxSC->stats.pktsUnusedSA
                aStats->inPktsNoSAError += rxSC->stats.pktsNotUsingSA

            aStats->inOctetsDecrypted += secy->stats.inOctetsDecrypted
            aStats->inOctetsValidated += secy->stats.inOctetsValidated
            aStats->inPktsNoSA += secy->stats.inPktsUnusedSA
            aStats->inPktsNoSAError += secy->stats.inPktsNotUsingSA
            ret = BCM_ERR_OK
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

int32_t MACSEC_DrvGetSecYStats(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_StatsType *const aStats)
{
    MACSEC_SecYContextType *secy;
    MACSEC_RxSCContextType *rxSC;
    MACSEC_HandleInfoType info;
    MACSEC_EgIgSecYStatsType secyStats;
    MACSEC_EgSAStatsType egStats;
    MACSEC_IgSAStatsType igStats;
    int     i, j;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (NULL != aStats) {
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            MACSEC_HANDLE_INFO_INIT(info);
            ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
            BCM_ASSERT(BCM_ERR_OK == ret);
            BCM_MemSet(aStats, 0x0U, sizeof(SECY_StatsType));
            MACSEC_DrvGetSecYStatsInternal(aConnHdl, info.hwIdx, secy->txVportIdx, &secyStats,
                                           MACSEC_DIR_EGRESS, (uint32_t)BCM_BOOL_FALSE);
            aStats->outPktsUntagged = secyStats.pktsUntagged;

            MACSEC_DrvGetSecYStatsInternal(aConnHdl, info.hwIdx, secy->rxVPortIdx, &secyStats,
                                           MACSEC_DIR_INGRESS, (uint32_t)BCM_BOOL_FALSE);

            aStats->inPktsUntagged = secyStats.pktsUntagged;
            aStats->inPktsNoTag = secyStats.pktsNoTag;
            aStats->inPktsBadTag = secyStats.pktsBadTag;
            aStats->inPktsNoSAError = secyStats.pktsNoSCI;
            aStats->inPktsNoSA = secyStats.pktsUnknownSCI;

            /*
             * if Tx is not deleted, iterate over all SA's
             * which are not deleted and collect the stats.
             * When SA is deleted for TxSC, stats had been
             * collected in the TX SC context.
             * When Tx SC is deleted, stats are cached
             * in SecY context.
             */
            if ((uint32_t)BCM_BOOL_TRUE == secy->txSC.active) {
                for (i = 0; i <= (int)MACSEC_AN_MAX; i++) {
                    if (0ULL != secy->txSC.sa[i].handle) {
                        MACSEC_DrvGetEgSAStats(aConnHdl, info.hwIdx, secy->txSC.sa[i].saIdx, &egStats, (uint32_t)BCM_BOOL_FALSE);
                        if ((uint32_t)BCM_BOOL_TRUE == secy->confProtect) {
                            /*@event CERT-C INT30
                            issue          Coverity infers that (stats + *) can wrap
                            risk            No risk
                            justification It is statistics, it is ok if it can wrap around. */
                            /* coverity[cert_int30_c_violation] */
                            aStats->outOctetsEncrypted += egStats.octetsEncryptedProtected;
                        } else {
                            /* coverity[cert_int30_c_violation] */
                            aStats->outOctetsProtected += egStats.octetsEncryptedProtected;
                        }
                        /* coverity[cert_int30_c_violation] */
                        aStats->outPktsTooLong += egStats.pktsTooLong;
                    }
                }
                if ((uint32_t)BCM_BOOL_TRUE == secy->confProtect) {
                    /* coverity[cert_int30_c_violation] */
                    aStats->outOctetsEncrypted += secy->txSC.stats.octetsEncryptedProtected;
                } else {
                    /* coverity[cert_int30_c_violation] */
                    aStats->outOctetsProtected += secy->txSC.stats.octetsEncryptedProtected;
                }
                /* coverity[cert_int30_c_violation] */
                aStats->outPktsTooLong += secy->txSC.stats.pktsTooLong;
            }
            if ((uint32_t)BCM_BOOL_TRUE == secy->confProtect) {
                /* coverity[cert_int30_c_violation] */
                aStats->outOctetsEncrypted += secy->stats.octetsEncryptedProtected;
            } else {
                /* coverity[cert_int30_c_violation] */
                aStats->outOctetsProtected += secy->stats.octetsEncryptedProtected;
            }
            /* coverity[cert_int30_c_violation] */
            aStats->outPktsTooLong += secy->stats.pktsTooLong;

            for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                rxSC = &secy->rxSC[i];
                if ((uint32_t)BCM_BOOL_TRUE == rxSC->active) {
                    for (j = 0; j <= (int)MACSEC_AN_MAX; j++) {
                        if (0ULL != rxSC->sa[j].handle) {
                            MACSEC_DrvGetIgSAStats(aConnHdl, info.hwIdx, rxSC->sa[i].saIdx, &igStats, (uint32_t)BCM_BOOL_FALSE);
                            /* coverity[cert_int30_c_violation] */
                            aStats->inOctetsDecrypted += igStats.octetsDecrypted;
                            /* coverity[cert_int30_c_violation] */
                            aStats->inOctetsValidated += igStats.octetsValidated;
                            /* coverity[cert_int30_c_violation] */
                            aStats->inPktsNoSA += igStats.pktsUnusedSA;
                            /* coverity[cert_int30_c_violation] */
                            aStats->inPktsNoSAError += igStats.pktsNotUsingSA;
                        }
                    }
                }
                /* coverity[cert_int30_c_violation] */
                aStats->inOctetsDecrypted += rxSC->stats.octetsDecrypted;
                /* coverity[cert_int30_c_violation] */
                aStats->inOctetsValidated += rxSC->stats.octetsValidated;
                /* coverity[cert_int30_c_violation] */
                aStats->inPktsNoSA += rxSC->stats.pktsUnusedSA;
                /* coverity[cert_int30_c_violation] */
                aStats->inPktsNoSAError += rxSC->stats.pktsNotUsingSA;
            }
            /* coverity[cert_int30_c_violation] */
            aStats->inOctetsDecrypted += secy->stats.inOctetsDecrypted;
            /* coverity[cert_int30_c_violation] */
            aStats->inOctetsValidated += secy->stats.inOctetsValidated;
            /* coverity[cert_int30_c_violation] */
            aStats->inPktsNoSA += secy->stats.inPktsUnusedSA;
            /* coverity[cert_int30_c_violation] */
            aStats->inPktsNoSAError += secy->stats.inPktsNotUsingSA;
            ret = BCM_ERR_OK;
        }
    }
    return ret;
}

/**
    @brief Get SA status

    @code{.unparsed}

    MACSEC_SAContextType *sa
    MACSEC_HandleInfoType info
    uint32_t startTimeOff
    uint32_t stopTimeOff
    int ret = BCM_ERR_INVAL_PARAMS

    if NULL != aStatus
        MACSEC_HANDLE_INFO_INIT(info)
        sa = MACSEC_DrvHandleToSA(aSAHdl)
        if NULL != sa
            ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info)
            BCM_ASSERT(BCM_ERR_OK == ret)
            BCM_MemSet(aStatus, 0x0, sizeof(SECY_SAStatusType))
            aStatus->inUse = sa->inUse
            startTimeOff = (uint8_t) offsetof(MACSEC_EgressXRecAES128Type, startTime)
            stopTimeOff = (uint8_t) offsetof(MACSEC_EgressXRecAES128Type, stopTime)
            if MACSEC_DIR_EGRESS == info.dir
                MACSEC_DrvXRecRead(aConnHdl, info.hwIdx, sa->saIdx, startTimeOff >> 2UL, 1UL,
                        &aStatus->startedTime, MACSEC_DIR_EGRESS)
                if FALSE == sa->inUse
                    MACSEC_DrvXRecRead(aConnHdl, info.hwIdx, sa->saIdx, stopTimeOff >> 2UL, 1UL,
                            &aStatus->stoppedTime, MACSEC_DIR_EGRESS)
            else
                MACSEC_DrvXRecRead(aConnHdl, info.hwIdx, sa->saIdx, startTimeOff >> 2UL, 1UL,
                        &aStatus->startedTime, MACSEC_DIR_INGRESS)
                if FALSE == sa->inUse
                    MACSEC_DrvXRecRead(aConnHdl, info.hwIdx, sa->saIdx, stopTimeOff >> 2UL, 1UL,
                            &aStatus->stoppedTime, MACSEC_DIR_INGRESS)
            aStatus->ssci = sa->ssci
            aStatus->createdTime = sa->createdTime
            BCM_MemCpy(aStatus->keyID, sa->keyID, sizeof(aStatus->keyID));
            ret = BCM_ERR_OK
    return ret

    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_GET_SECYSTATS_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

int32_t MACSEC_DrvSAGetStatus(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl, SECY_SAStatusType *const aStatus)
{
    MACSEC_SAContextType *sa;
    MACSEC_HandleInfoType info;
    uint32_t startTimeOff;
    uint32_t stopTimeOff;
    int ret = BCM_ERR_INVAL_PARAMS;

    if (NULL != aStatus) {
        MACSEC_HANDLE_INFO_INIT(info);
        sa = MACSEC_DrvHandleToSA(aSAHdl);
        if (NULL != sa) {
            ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info);
            BCM_ASSERT(BCM_ERR_OK == ret);
            BCM_MemSet(aStatus, 0x0, sizeof(SECY_SAStatusType));
            aStatus->inUse = sa->inUse;
            startTimeOff = (uint8_t) offsetof(MACSEC_EgressXRecAES128Type, startTime);
            stopTimeOff = (uint8_t) offsetof(MACSEC_EgressXRecAES128Type, stopTime);
            if (MACSEC_DIR_EGRESS == info.dir) {
                MACSEC_DrvXRecRead(aConnHdl, (uint32_t)info.hwIdx, (uint32_t)sa->saIdx, startTimeOff >> 2UL, 1UL,
                        &sa->trafficStartTime, MACSEC_DIR_EGRESS);
                if ((uint32_t)BCM_BOOL_FALSE == sa->inUse) {
                    MACSEC_DrvXRecRead(aConnHdl, (uint32_t)info.hwIdx, (uint32_t)sa->saIdx, stopTimeOff >> 2UL, 1UL,
                            &sa->trafficStopTime, MACSEC_DIR_EGRESS);
                }
            } else {
                MACSEC_DrvXRecRead(aConnHdl, (uint32_t)info.hwIdx, (uint32_t)sa->saIdx, startTimeOff >> 2UL, 1UL,
                        &sa->trafficStartTime, MACSEC_DIR_INGRESS);
                if ((uint32_t)BCM_BOOL_FALSE == sa->inUse) {
                    MACSEC_DrvXRecRead(aConnHdl, (uint32_t)info.hwIdx, (uint32_t)sa->saIdx, stopTimeOff >> 2UL, 1UL,
                            &sa->trafficStopTime, MACSEC_DIR_INGRESS);
                }
            }
            aStatus->ssci = sa->ssci;
            aStatus->createdTime = sa->createdTime;
            aStatus->startedTime = sa->startTime;
            aStatus->stoppedTime = sa->stopTime;
            BCM_MemCpy(aStatus->keyID, sa->keyID, sizeof(aStatus->keyID));
            ret = BCM_ERR_OK;
        }
    }
    return ret;
}

/**
    @brief Get Port Status

    @code{.unparsed}
    ret = BCM_ERR_OK;
    ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

    aPortStatus->hwStatus = 0UL;

    hwContext = &MACSEC_HwCtxt[ctxIdx];
    if macsec is init
        search for valid secy instance.

    secy = &hwContext->secy[i];
    if protectFrame is TRUE and validateFrame is STRICT
        set SECURE bit in hwStatus
        if confProtect is TRUE
            set CONFIDENTIALITY bit in hwStatus

        find for active RXSC
        if (transmitting is TRUE && receiving TRUE) || enCtrlPort is TRUE
            set OPERATIONAL bit in hwStatus

    return ret;
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
#ifdef RESERVED_FOR_FUTURE /**< @brief Keep this function which may be used at a later time */
static int32_t MACSEC_DrvGetStatus(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, SECY_PortStatusType *const aPortStatus)
{
    int32_t ret = BCM_ERR_OK;
    int     i;
    MACSEC_HwContextType *hwContext;
    MACSEC_SecYContextType  *secy;

    if (aID < MACSEC_MAX_PORT_NUM) {
        /* @event         misra_c_2012_rule_violation, zero_deref
           @issue         Explicit null dereferenced (FORWARD_NULL)
           @risk          Undefined behaviour
           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
        /* coverity[misra_c_2012_rule_violation] */
        /* coverity[zero_deref] */
        ret = MACSEC_DummyRead(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(misc_control));

        if (BCM_ERR_OK == ret) {   /* Dummy read to check for MACSEC Support */
            uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);
            aPortStatus->hwStatus = 0UL;

            hwContext = &MACSEC_HwCtxt[ctxIdx];
            if ((uint32_t)BCM_BOOL_TRUE == hwContext->initialized) {   /* MACSEC Initialized */
                for (i = 0; i < MACSEC_MAX_SECY_PER_PORT; i++) {
                    /* Find valid secy instance */
                    if (hwContext->secy[i].handle != 0U) {
                        break;
                    }
                }
                if (i < MACSEC_MAX_SECY_PER_PORT) {
                    secy = &hwContext->secy[i];
                    if ((secy->protectFrame == (uint32_t)BCM_BOOL_TRUE)
                            && (secy->validateFrame == SECY_VALIDATE_FRAME_STRICT)) {
                        aPortStatus->hwStatus |= SECY_HW_STATUS_SECURED_MASK;   /* Link is secure with Macsec Tag */
                        if (secy->confProtect == (uint32_t)BCM_BOOL_TRUE) {
                            aPortStatus->hwStatus |= SECY_HW_STATUS_CONFIDENTIALITY_MASK; /* Link is secure with Macsec Tag + Data Encription */
                        }
                    }

                    if (0UL != (aPortStatus->hwStatus & SECY_HW_STATUS_SECURED_MASK)) {
                        for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                            /* Find the Active RX SC */
                            if (secy->rxSC[i].active == (uint32_t)BCM_BOOL_TRUE) {
                                break;
                            }
                        }

                        if (i < MACSEC_MAX_RXSC_PER_SECY) {
                            if ((secy->enCtrlPort == (uint32_t)BCM_BOOL_TRUE)
                                    && (secy->txSC.transmitting == (uint32_t)BCM_BOOL_TRUE)
                                    && (secy->rxSC[i].receiving == (uint32_t)BCM_BOOL_TRUE)) {
                                aPortStatus->hwStatus |= SECY_HW_STATUS_OPERATIONAL_MASK; /*  Only Tagged  Traffic allowed */
                            }
                        }
                    } else {
                        if (secy->enCtrlPort == (uint32_t)BCM_BOOL_TRUE) {
                            aPortStatus->hwStatus |= SECY_HW_STATUS_OPERATIONAL_MASK;     /* Only UnTagged Traffic allowed */
                        }
                    }

                    aPortStatus->cipher = secy->cipherID;
                }
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}
#endif

/**
    @brief

    @code{.unparsed}
    MACSEC_SAContextType *context
    MACSEC_HandleInfoType info
    uint32_t nextPNAddr
    uint32_t nextPNCntrl = 0UL
    uint32_t i
    int32_t ret = BCM_ERR_OK

    MACSEC_HANDLE_INFO_INIT(info)
    context = MACSEC_DrvHandleToSA(aSAHdl)
    if NULL != context
        ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        if MACSEC_DIR_INGRESS == info.dir
            if current cipher suite is xpn
                MACSEC_I_EIP62_REGS[info.hwIdx]->nextpn_lo = aPN & 0xFFFFFFFFUL
                MACSEC_I_EIP62_REGS[info.hwIdx]->nextpn_hi = (aPN >> 32UL) & 0xFFFFFFFFUL
            else
                if aPN <= 0xFFFFFFFFUL
                    MACSEC_I_EIP62_REGS[info.hwIdx]->nextpn_lo = aPN & 0xFFFFFFFFUL
                    MACSEC_I_EIP62_REGS[info.hwIdx]->nextpn_hi = (aPN >> 32UL) & 0xFFFFFFFFUL
                else
                    ret = BCM_ERR_INVAL_PARAMS
            if BCM_ERR_OK == ret
                MACSEC_I_EIP62_REGS[info.hwIdx]->nextpn_ctx_id = context->contextID
                nextPNAddr = (info.SAIdx * MACSEC_XREC_SIZE_PER_ENTRY) + context->xRecSeq0Offset
                nextPNAddr = nextPNAddr >> 4UL
                if current cipher suite ix XPN128 or XPN256
                    nextPNCntrl |= MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_SEQUENCE_NUMBER_SIZE_MASK

                nextPNCntrl |= (nextPNAddr <<
                                MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_NEXTPN_CONTEXT_ADDRESS_SHIFT)
                nextPNCntrl |= MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_ENABLE_UPDATE_MASK
                MACSEC_I_EIP62_REGS[info.hwIdx]->nextpn_ctrl = nextPNCntrl
                for i = 0UL; i < MACSEC_LOAD_UNSAFE_TIMEOUT_CNT; i++
                    nextPNCntrl = MACSEC_I_EIP62_REGS[info.hwIdx]->nextpn_ctrl
                    if 0UL == (nextPNCntrl & MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_ENABLE_UPDATE_MASK)
                        break
                if i <= MACSEC_LOAD_UNSAFE_TIMEOUT_CNT
                    if 0UL == (nextPNCntrl & MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_NEXTPN_WRITTEN_MASK
                        ret = BCM_ERR_UNKNOWN
                else
                    ret = BCM_ERR_TIME_OUT
        else
            ret = BCM_ERR_INVAL_PARAMS
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SASET_NEXT_PN_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

int32_t MACSEC_DrvSASetNextPN(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl, uint64_t aPN)
{
    MACSEC_SAContextType *context;
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    uint32_t nextPNAddr;
    uint32_t nextPNCntrl = 0UL;
    uint32_t i;
    int32_t ret = BCM_ERR_OK;

    MACSEC_HANDLE_INFO_INIT(info);
    context = MACSEC_DrvHandleToSA(aSAHdl);
    if (NULL != context) {
        ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info);
        if ((BCM_ERR_OK == ret) && (MACSEC_DIR_INGRESS == info.dir)) {
            uint32_t aPnLo = (uint32_t) (aPN & 0xFFFFFFFFUL);
            uint32_t aPnHi = (uint32_t)(aPN >> 32UL) & 0xFFFFFFFFUL;
            secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx];
            if (BCM_BOOL_TRUE == MACSEC_DrvIsXpnCipherSuiteSet(secy)) {
                MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(nextpn_lo), aPnLo);
                MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(nextpn_hi), aPnHi);
            } else {
                if (aPN <= 0xFFFFFFFFUL) {
                    MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(nextpn_lo), aPnLo);
                    MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(nextpn_hi), aPnHi);
                } else {
                    ret = BCM_ERR_INVAL_PARAMS;
                }
            }
            if (BCM_ERR_OK == ret) {
                MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(nextpn_ctx_id), context->contextID);
                nextPNAddr = ((info.SAIdx * MACSEC_XREC_SIZE_PER_ENTRY) + context->xRecSeq0Offset) >> 4UL;
                if (BCM_BOOL_TRUE == MACSEC_DrvIsXpnCipherSuiteSet(secy)) {
                    nextPNCntrl |= MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_SEQUENCE_NUMBER_SIZE_MASK;
                }
                nextPNCntrl |= (nextPNAddr << MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_NEXTPN_CONTEXT_ADDRESS_SHIFT) |
                                MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_ENABLE_UPDATE_MASK;
                MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(nextpn_ctrl), nextPNCntrl);
                /* wait for PN update to complete */
                for (i = 0UL; i < MACSEC_LOAD_UNSAFE_TIMEOUT_CNT; i++) {
                    nextPNCntrl = MACSEC_Read(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(nextpn_ctrl));
                    if (0UL == (nextPNCntrl & MACSEC_E_EIP62_EIP62_NEXTPN_CTRL_ENABLE_UPDATE_MASK)) {
                        break;
                    }
                }
                /*
                  When PN is written successfully, "next_pn_written" bit is set by the hardware which shall be checked here but only in the case
                  where nextPN number being written is not same as in context memory. If the sequence number in the context has already reached
                  this value, "next_pn_written" bit will not be set by the hardware.
                */
                if (i >= MACSEC_LOAD_UNSAFE_TIMEOUT_CNT) {
                    ret = BCM_ERR_TIME_OUT;
                }
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief Get next packet number for given SA

    @code{.unparsed}

    MACSEC_SAContextType *context
    MACSEC_HandleInfoType info
    uint32_t seq[2UL] = {0UL}
    int32_t ret = BCM_ERR_OK

    MACSEC_HANDLE_INFO_INIT(info)
    context = MACSEC_DrvHandleToSA(aSAHdl)
    if NULL != context
        ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info)
        BCM_ASSERT(BCM_ERR_OK == ret)
        if current cipher suite is XPN128 or XPN256
            MACSEC_DrvXRecRead(aConnHdl, info.hwIdx, context->saIdx, context->xRecSeq0Offset >> 2UL, 2UL, seq,
                               info.dir)
            *aPN = seq[0U] & 0xFFFFFFFFFULL
            *aPN |= ((uint64_t)seq[1U] << 32UL)
        else
            MACSEC_DrvXRecRead(aConnHdl, info.hwIdx, context->saIdx, context->xRecSeq0Offset >> 2UL, 1UL, seq,
                               info.dir)
            *aPN = seq[0U] & 0xFFFFFFFFFUL
        if MACSEC_DIR_EGRESS == info.dir
            *aPN = *aPN + 1UL
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SAGET_NEXT_PN_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvSAGetNextPN(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl, uint64_t *aPN)
{
    MACSEC_SAContextType *context;
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    uint32_t seq[2UL] = {0UL};
    int32_t ret = BCM_ERR_OK;

    MACSEC_HANDLE_INFO_INIT(info);
    context = MACSEC_DrvHandleToSA(aSAHdl);
    if (NULL != context) {
        ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx];
        if (BCM_BOOL_TRUE == MACSEC_DrvIsXpnCipherSuiteSet(secy)) {
            MACSEC_DrvXRecRead(aConnHdl, (uint32_t)info.hwIdx, (uint32_t)context->saIdx, (uint32_t)(context->xRecSeq0Offset) >> 2U, 2UL, seq, info.dir);
            *aPN = seq[0U] & 0xFFFFFFFFFULL;
            *aPN |= ((uint64_t)seq[1U] << 32UL);
        } else {
            MACSEC_DrvXRecRead(aConnHdl, (uint32_t)info.hwIdx, (uint32_t)context->saIdx, (uint32_t)(context->xRecSeq0Offset) >> 2U, 1UL, seq, info.dir);
            *aPN = seq[0U] & 0xFFFFFFFFFUL;
        }
        if (MACSEC_DIR_EGRESS == info.dir) {
            /* @event      CERT-C INT30
                           issue       Coverity infers that (*aPN + 1UL) can wrap
                           risk           No risk
                           justification It is next packet number, it is ok, it can wrap around to 0. */
           /* coverity[cert_int30_c_violation] */
            *aPN = *aPN + 1UL;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief Complete hardware deinitialization

    @code{.unparsed}

    MACSEC_HwContextType *hwContext
    int32_t ret = BCM_ERR_OK

    if aID < MACSEC_MAX_PORT_NUM
        ctxIdx = MACSEC_ConvPort2CtxIdx(aID)
        hwContext = &MACSEC_HwCtxt[ctxIdx]
        MACSEC_BusDrvDeInit(aConnHdl, aID)
        BCM_MemSet(hwContext, 0x0, sizeof(MACSEC_HwContextType))
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DEINIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvDeInit(BCM_HandleType aConnHdl, MACSEC_HwIDType aID)
{
    MACSEC_HwContextType *hwContext;
    int32_t ret = BCM_ERR_OK;

    if (aID < MACSEC_MAX_PORT_NUM) {
        uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);
        hwContext = &MACSEC_HwCtxt[ctxIdx];
        if ((uint32_t)BCM_BOOL_TRUE == hwContext->initialized) {
            /* this shall internally deassert the reset of the block and clear all the internal logic */
            MACSEC_BusDrvDeInit(aConnHdl, aID);
            BCM_MemSet(hwContext, 0x0, sizeof(MACSEC_HwContextType));
            MACSEC_ClearSysInterrupt(aID);
            MACSEC_DisableSysInterrupt(aID);
        } else {
            ret = BCM_ERR_UNINIT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief SA disable

    Makes an active SA inactive

    @code{.unparsed}
    MACSEC_HandleInfoType info
    MACSEC_SAContextType *sa
    MACSEC_SecYContextType *secy
    MACSEC_RxSCContextType *rxSC
    int32_t ret = BCM_ERR_OK

    MACSEC_HANDLE_INFO_INIT(info)
    sa = MACSEC_DrvHandleToSA(aSAHdl)
    if NULL != sa
        if TRUE == sa->inUse
            ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info)
            BCM_ASSERT(BCM_ERR_OK == ret)
            secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx]
            if MACSEC_DIR_EGRESS == info.dir
                if secy->txSC.encodingSA == info.AN
                    ret = MACSEC_DrvSASetInUse(aConnHdl, info.hwIdx, info.SCIdx, (uint32_t)BCM_BOOL_FALSE, 0UL, info.AN,
                                               info.dir)
                    if BCM_ERR_OK == ret
                        secy->txSC.transmitting = (uint32_t)BCM_BOOL_FALSE
                sa->inUse = (uint32_t)BCM_BOOL_FALSE
            else
                ret = MACSEC_DrvSASetInUse(aConnHdl, info.hwIdx, info.SCIdx, (uint32_t)BCM_BOOL_FALSE, 0UL, info.AN,
                                           info.dir)
                if BCM_ERR_OK == ret
                    rxSC = &secy->rxSC[info.SCIdx]
                    rxSC->saActiveMask &= ~(0x1UL << info.AN)
                    if 0UL == rxSC->saActiveMask
                        rxSC->receiving = (uint32_t)BCM_BOOL_FALSE
                    sa->inUse = (uint32_t)BCM_BOOL_FALSE
        else
            ret = BCM_ERR_INVAL_STATE
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SADISABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvSADisable(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl)
{
    MACSEC_HandleInfoType info;
    MACSEC_SAContextType *sa;
    MACSEC_SecYContextType *secy;
    MACSEC_RxSCContextType *rxSC;
    MACSEC_HwIDType hwID;
    int32_t ret = BCM_ERR_OK;

    MACSEC_HANDLE_INFO_INIT(info);
    sa = MACSEC_DrvHandleToSA(aSAHdl);
    if (NULL != sa) {
        ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        hwID = info.hwIdx;
        secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx];
        if (MACSEC_DIR_EGRESS == info.dir) {
            if (secy->txSC.encodingSA == info.AN) {
                ret = MACSEC_DrvSASetInUse(aConnHdl, info.hwIdx, info.SCIdx, (uint32_t)BCM_BOOL_FALSE, 0UL, info.AN,
                        info.dir);
                if (BCM_ERR_OK == ret) {
                    secy->txSC.transmitting = (uint32_t)BCM_BOOL_FALSE;
                }
            }
            sa->inUse = (uint32_t)BCM_BOOL_FALSE;
            sa->stopTime = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_BRCM_GLOBAL_OFFSET_OF(tick));
        } else {
            ret = MACSEC_DrvSASetInUse(aConnHdl, info.hwIdx, info.SCIdx, (uint32_t)BCM_BOOL_FALSE, 0UL, info.AN,
                    info.dir);
            if (BCM_ERR_OK == ret) {
                rxSC = &secy->rxSC[info.SCIdx];
                rxSC->saActiveMask &= ~(0x1UL << info.AN);
                if (0UL == rxSC->saActiveMask) {
                    rxSC->receiving = (uint32_t)BCM_BOOL_FALSE;
                }
                sa->inUse = (uint32_t)BCM_BOOL_FALSE;
                sa->stopTime = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_BRCM_GLOBAL_OFFSET_OF(tick));
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief Delete Secure association

    @code{.unparsed}
    MACSEC_HandleInfoType info
    MACSEC_SAContextType *sa
    MACSEC_EgSAStatsType egStats
    MACSEC_TxSCContextType *txSC
    MACSEC_RxSCContextType *rxSC
    MACSEC_IgSAStatsType igStats
    int32_t ret = BCM_ERR_OK

    MACSEC_HANDLE_INFO_INIT(info)
    sa = MACSEC_DrvHandleToSA(aSAHdl)
    if NULL != sa
        ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info)
        BCM_ASSERT(BCM_ERR_OK == ret)
        if (uint32_t)BCM_BOOL_FALSE == sa->inUse
            if MACSEC_DIR_EGRESS == info.dir
                txSC = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx].txSC
                MACSEC_DrvGetEgSAStats(aConnHdl, info.hwIdx, info.SAIdx, &egStats, TRUE)
                txSC->stats.octetsEncryptedProtected += egStats.octetsEncryptedProtected
                txSC->stats.pktsEncryptedProtected += egStats.pktsEncryptedProtected
                txSC->stats.pktsTooLong += egStats.pktsTooLong
                txSC->stats.pktsSANotInUse += egStats.pktsSANotInUse
                MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].egress.saAllocMask, info.SAIdx)
                sa->handle = 0UL
            else
                rxSC = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx].rxSC[info.SCIdx]
                MACSEC_DrvGetIgSAStats(aConnHdl, info.hwIdx, info.SAIdx, &igStats, TRUE)
                rxSC->stats.octetsDecrypted += igStats.octetsDecrypted
                rxSC->stats.octetsValidated += igStats.octetsValidated
                rxSC->stats.pktsUnchecked += igStats.pktsUnchecked
                rxSC->stats.pktsDelayed += igStats.pktsDelayed
                rxSC->stats.pktsLate += igStats.pktsLate
                rxSC->stats.pktsOK += igStats.pktsOK
                rxSC->stats.pktsInvalid += igStats.pktsInvalid
                rxSC->stats.pktsNotValid += igStats.pktsNotValid
                rxSC->stats.pktsNotUsingSA += igStats.pktsNotUsingSA
                rxSC->stats.pktsUnusedSA += igStats.pktsUnusedSA
                MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].ingress.saAllocMask, info.SAIdx)
                sa->handle = 0U
        else
            ret = BCM_ERR_BUSY
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

int32_t MACSEC_DrvDeleteSA(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl)
{
    MACSEC_HandleInfoType info;
    MACSEC_SAContextType *sa;
    MACSEC_EgSAStatsType egStats = {0ULL};
    MACSEC_TxSCContextType *txSC;
    MACSEC_RxSCContextType *rxSC;
    MACSEC_IgSAStatsType igStats = {0ULL};
    int32_t ret = BCM_ERR_OK;

    ret = MACSEC_DrvSADisable(aConnHdl, aSAHdl);
    if (BCM_ERR_OK == ret) {
        MACSEC_HANDLE_INFO_INIT(info);
        sa = MACSEC_DrvHandleToSA(aSAHdl);
        if (NULL != sa) {
            ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info);
            BCM_ASSERT(BCM_ERR_OK == ret);
            if (MACSEC_DIR_EGRESS == info.dir) {
                txSC = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx].txSC;
                MACSEC_DrvGetEgSAStats(aConnHdl, info.hwIdx, info.SAIdx, &egStats, (uint32_t)BCM_BOOL_TRUE);

                /* @event      CERT INT30-C
                                 @issue       Coverity infers that (stats + *) can wrap
                                 @risk           No risk
                                 @justification It is statistics, it is ok if it can wrap around. */
                /* coverity[cert_int30_c_violation] */
                txSC->stats.octetsEncryptedProtected += egStats.octetsEncryptedProtected;
                /* coverity[cert_int30_c_violation] */
                txSC->stats.pktsEncryptedProtected += egStats.pktsEncryptedProtected;
                /* coverity[cert_int30_c_violation] */
                txSC->stats.pktsTooLong += egStats.pktsTooLong;
                /* coverity[cert_int30_c_violation] */
                txSC->stats.pktsSANotInUse += egStats.pktsSANotInUse;
                MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].egress.saAllocMask, info.SAIdx);
                sa->handle = 0ULL;
            } else {
                rxSC = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx].rxSC[info.SCIdx];
                MACSEC_DrvGetIgSAStats(aConnHdl, info.hwIdx, info.SAIdx, &igStats, (uint32_t)BCM_BOOL_TRUE);
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.octetsDecrypted += igStats.octetsDecrypted;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.octetsValidated += igStats.octetsValidated;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.pktsUnchecked += igStats.pktsUnchecked;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.pktsDelayed += igStats.pktsDelayed;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.pktsLate += igStats.pktsLate;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.pktsOK += igStats.pktsOK;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.pktsInvalid += igStats.pktsInvalid;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.pktsNotValid += igStats.pktsNotValid;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.pktsNotUsingSA += igStats.pktsNotUsingSA;
                /* coverity[cert_int30_c_violation] */
                rxSC->stats.pktsUnusedSA += igStats.pktsUnusedSA;
                MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].ingress.saAllocMask, info.SAIdx);
                sa->handle = 0ULL;
            }
        }
    }

    return ret;
}

/**
    @brief Delete Rx Secure channel

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_RxSCContextType *rxSC
    MACSEC_HandleInfoType info
    uint32_t rxCAMIdx
    uint32_t rxSCIdx
    uint32_t i
    int32_t ret = BCM_ERR_INVAL_PARAMS

    MACSEC_HANDLE_INFO_INIT(info)
    if NULL != aSCI
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
            BCM_ASSERT(BCM_ERR_OK == ret)
            for i = 0UL; i < MACSEC_MAX_RXSC_PER_SECY; i++
                if (uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active
                    if BCM_MemCmp(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType)) == 0U
                        rxSCIdx = i
                        break
            if i < MACSEC_MAX_RXSC_PER_SECY
                rxSC = &secy->rxSC[rxSCIdx]
                if (uint32_t)BCM_BOOL_FALSE == rxSC->receiving
                    for i = 0UL; i <= MACSEC_AN_MAX; i++
                        if 0UL != rxSC->sa[i].handle
                           break
                if i > MACSEC_AN_MAX
                    rxCAMIdx = rxSC->rxCAMIdx
                    MACSEC_I_RXCAMEN_REGS[info.hwIdx]->cam_enable_ctrl =
                        (rxCAMIdx << MACSEC_I_RXSC_CAMEN_RXSC_CAM_ENABLE_CTRL_SC_INDEX_CLEAR_SHIFT) |
                         MACSEC_I_RXSC_CAMEN_RXSC_CAM_ENABLE_CTRL_CLEAR_ENABLE_MASK
                    ret = MACSEC_DrvInFlightSync(aConnHdl, info.hwIdx)
                    if BCM_ERR_OK == ret
                        secy->stats.inOctetsDecrypted += rxSC->stats.octetsDecrypted
                        secy->stats.inOctetsValidated += rxSC->stats.octetsValidated
                        secy->stats.inPktsNotUsingSA += rxSC->stats.pktsNotUsingSA
                        secy->stats.inPktsUnusedSA += rxSC->stats.pktsUnusedSA
                        MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].ingress.rxCAMAllocMask, rxCAMIdx)
                        MACSEC_SetBit(&secy->rxSCAllocMask, rxSCIdx)
                        BCM_MemSet(rxSC, 0x0UL, sizeof(MACSEC_RxSCContextType))
                else
                    ret = BCM_ERR_BUSY
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvDeleteRxSC(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_SCIType *aSCI)
{
    MACSEC_SecYContextType *secy;
    MACSEC_RxSCContextType *rxSC;
    MACSEC_HandleInfoType info;
    BCM_HandleType saHdl;
    uint32_t rxCAMIdx = 0;
    uint32_t rxSCIdx = 0;
    int      i;
    uint32_t anIdx;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    MACSEC_HANDLE_INFO_INIT(info);
    if (NULL != aSCI) {
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
            BCM_ASSERT(BCM_ERR_OK == ret);

            /* is RxSC created? */
            for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active) {
                    if (BCM_MemCmp(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType)) == 0) {
                        rxSCIdx = (uint32_t) i;
                        break;
                    }
                }
            }

            if (i < MACSEC_MAX_RXSC_PER_SECY) {
                /* Delete and Disable All created SAs */
                for (anIdx = 0; anIdx <= MACSEC_AN_MAX; anIdx++) {
                    if (0ULL != secy->rxSC[i].sa[anIdx].handle) {
                        saHdl = secy->rxSC[i].sa[anIdx].handle;
                        if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].sa[anIdx].inUse) {
                            ret = MACSEC_DrvDeleteSA(aConnHdl, saHdl);
                            if(BCM_ERR_OK != ret) {
                                break;
                            }
                        }
                    }
                }

                if (BCM_ERR_OK == ret) {
                    rxSC = &secy->rxSC[rxSCIdx];
                    rxCAMIdx = rxSC->rxCAMIdx;
                    /* disable the RXCAM entry */

                    MACSEC_Write(aConnHdl, info.hwIdx, (uint32_t)MACSEC_BLOCK_CORE, MACSEC_I_RXSC_CAMEN_OFFSET_OF(cam_enable_ctrl),
                            (rxCAMIdx << MACSEC_I_RXSC_CAMEN_RXSC_CAM_ENABLE_CTRL_SC_INDEX_CLEAR_SHIFT) |
                                        (uint32_t)MACSEC_I_RXSC_CAMEN_RXSC_CAM_ENABLE_CTRL_CLEAR_ENABLE_MASK);

                    ret = MACSEC_DrvInFlightSync(aConnHdl, info.hwIdx);
                    if (BCM_ERR_OK == ret) {
                        secy->stats.inOctetsDecrypted += rxSC->stats.octetsDecrypted;
                        secy->stats.inOctetsValidated += rxSC->stats.octetsValidated;
                        secy->stats.inPktsNotUsingSA += rxSC->stats.pktsNotUsingSA;
                        secy->stats.inPktsUnusedSA += rxSC->stats.pktsUnusedSA;
                        MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].ingress.rxCAMAllocMask, rxCAMIdx);
                        MACSEC_SetBit(&secy->rxSCAllocMask, rxSCIdx);
                        BCM_MemSet(rxSC, 0x0U, sizeof(MACSEC_RxSCContextType));
                    }
                }
            }
        }
    }
    return ret;
}

/**
    @brief Delete Tx SC

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_HandleInfoType info
    MACSEC_TxSCContextType *txSC
    uint32_t i
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if NULL != aSCI
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            if BCM_MemCmp(aSCI, &secy->txSC.sci, sizeof(SECY_SCIType)) == 0U
                ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
                BCM_ASSERT(BCM_ERR_OK == ret)
                txSC = &secy->txSC
                if (uint32_t)BCM_BOOL_FALSE == txSC->transmitting
                    for i = 0UL; i <= MACSEC_AN_MAX; i++
                        if 0UL != txSC->sa[i].hash
                            break
                    if i > MACSEC_AN_MAX
                        secy->stats.octetsEncryptedProtected += txSC->stats.octetsEncryptedProtected
                        secy->stats.pktsEncryptedProtected += txSC->stats.pktsEncryptedProtected
                        secy->stats.pktsTooLong += txSC->stats.pktsTooLong
                        BCM_MemSet(txSC, 0x0U, sizeof(MACSEC_TxSCContextType))
                        ret = BCM_ERR_OK
                    else
                        ret = BCM_ERR_BUSY
                else
                    ret = BCM_ERR_BUSY
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_TX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvDeleteTxSC(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_SCIType *aSCI)
{
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    MACSEC_TxSCContextType *txSC;
    BCM_HandleType saHdl;
    uint32_t anIdx;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSCI) {
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            /* does SCI exists? */
            if (BCM_MemCmp(aSCI, &secy->txSC.sci, sizeof(SECY_SCIType)) == 0) {
                ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
                BCM_ASSERT(BCM_ERR_OK == ret);

                /* Delete and Disable All created SAs */
                for (anIdx = 0; anIdx <= MACSEC_AN_MAX; anIdx++) {
                    if (0ULL != secy->txSC.sa[anIdx].handle) {
                        saHdl = secy->txSC.sa[anIdx].handle;
                        if ((uint32_t)BCM_BOOL_TRUE == secy->txSC.sa[anIdx].inUse) {
                            ret = MACSEC_DrvDeleteSA(aConnHdl, saHdl);
                            if (BCM_ERR_OK != ret) {
                                break;
                            }
                        }
                    }
                }

                if (BCM_ERR_OK == ret) {
                    txSC = &secy->txSC;
                    /*@event CERT-C INT30
                                    issue          Coverity infers that (stats + *) can wrap
                                    risk            No risk
                                    justification It is statistics, it is ok if it can wrap around. */
                    /* coverity[cert_int30_c_violation] */
                    secy->stats.octetsEncryptedProtected += txSC->stats.octetsEncryptedProtected;
                    /* coverity[cert_int30_c_violation] */
                    secy->stats.pktsEncryptedProtected += txSC->stats.pktsEncryptedProtected;
                    /* coverity[cert_int30_c_violation] */
                    secy->stats.pktsTooLong += txSC->stats.pktsTooLong;
                    BCM_MemSet(txSC, 0x0U, sizeof(MACSEC_TxSCContextType));
                }
            }
        }
    }
    return ret;
}

/**
    @brief

    @code{.unparsed}
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    uint32_t i;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    MACSEC_HANDLE_INFO_INIT(info);
    secy = MACSEC_DrvHandleToSecY(aSecYHdl);
    if (NULL != secy) {
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        if ((uint32_t)BCM_BOOL_FALSE == secy->txSC.active) {
            for (i = 0UL; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active) {
                    break;
                }
            }
            if (i == MACSEC_MAX_RXSC_PER_SECY) {
                if ((0UL == secy->txTcamRules) && (0UL == secy->rxTcamRules)) {
                    MACSEC_E_FCTRL_REGS[info.hwIdx]->samflow_regs[secy->txVportIdx].ctrl1_0 = 0UL;
                    MACSEC_E_FCTRL_REGS[info.hwIdx]->samflow_regs[secy->txVportIdx].ctrl2_0 = 0UL;
                    MACSEC_I_FCTRL_REGS[info.hwIdx]->samflow_regs[secy->rxVPortIdx].ctrl1_0 = 0UL;
                    MACSEC_I_FCTRL_REGS[info.hwIdx]->samflow_regs[secy->rxVPortIdx].ctrl2_0 = 0UL;
                    MACSEC_DrvGetSecYStatsInternal(aConnHdl, info.hwIdx, info.secyIdx, NULL,
                                                   MACSEC_DIR_EGRESS, (uint32_t)BCM_BOOL_TRUE);
                    MACSEC_DrvGetSecYStatsInternal(aConnHdl, info.hwIdx, info.secyIdx, NULL,
                                                   MACSEC_DIR_INGRESS, (uint32_t)BCM_BOOL_TRUE);
                    MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].egress.vportAllocMask,
                                  secy->txVportIdx);
                    MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].ingress.vportAllocMask,
                                  secy->rxVPortIdx);
                    BCM_MemSet(secy, 0x0U, sizeof(MACSEC_SecYContextType));
                    MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].secyAllocMask, info.secyIdx);
                    ret = BCM_ERR_OK;
                } else {
                    ret = BCM_ERR_BUSY;
                }
            } else {
                ret = BCM_ERR_BUSY;
            }
        } else {
            ret = BCM_ERR_BUSY;
        }
    }
    return ret;
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_DELETE_SECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

int32_t MACSEC_DrvDeleteSecY(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl)
{
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    MACSEC_HandleInfoType lookupInfo;
    int      i;
    uint32_t tcamIdx;
    uint32_t txTcamRules;
    uint32_t rxTcamRules;
    MACSEC_RuleHdlType ruleHdl;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    MACSEC_HANDLE_INFO_INIT(info);
    secy = MACSEC_DrvHandleToSecY(aSecYHdl);
    if (NULL != secy) {
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);

        /* Delete All RXSC */
        for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
            if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active) {
                ret = MACSEC_DrvDeleteRxSC(aConnHdl, aSecYHdl, &secy->rxSC[i].sci);
                if (BCM_ERR_OK != ret) {
                    break;
                }
            }
        }

        /* Delete TXSC */
        if ((BCM_ERR_OK == ret)
                && ((uint32_t)BCM_BOOL_TRUE == secy->txSC.active)) {
            ret = MACSEC_DrvDeleteTxSC(aConnHdl, aSecYHdl, &secy->txSC.sci);
        }

        /* Remove LookUp Rule */
        if (BCM_ERR_OK == ret) {
            MACSEC_HANDLE_INFO_INIT(lookupInfo);
            BCM_MemCpy(&lookupInfo, &info, sizeof(MACSEC_HandleInfoType));
            lookupInfo.ruleType = MACSEC_RULEMASK_TCAM;
            tcamIdx = 0UL;
            txTcamRules = secy->txTcamRules;
            rxTcamRules = secy->rxTcamRules;
            do {
                /*Remove all TX LookUp Rules*/
                if ((txTcamRules & (0x1UL << tcamIdx)) != 0UL) {
                    lookupInfo.ruleIdxEgress = (uint8_t)tcamIdx;
                    lookupInfo.dir = SECY_RULE_DIR_EGRESS;
                    ruleHdl = MACSEC_DrvCreateHandle(&lookupInfo);
                    MACSEC_DrvHandleUpdateHash(&ruleHdl, MACSEC_HwCtxt[info.ctxIdx].egress.tcamRuleHash[tcamIdx]);
                    ret = MACSEC_DrvRemoveSecYLookupRule(aConnHdl, ruleHdl, BCM_BOOL_TRUE);
                    if(BCM_ERR_OK != ret) {
                        break;
                    }
                    txTcamRules &= ~(0x1UL << tcamIdx);
                }
                /*Remove all RX LookUp Rules*/
                if ((rxTcamRules & (0x1UL << tcamIdx)) != 0UL) {
                    lookupInfo.ruleIdxIngress = (uint8_t)tcamIdx;
                    lookupInfo.dir = SECY_RULE_DIR_INGRESS;
                    ruleHdl = MACSEC_DrvCreateHandle(&lookupInfo);
                    MACSEC_DrvHandleUpdateHash(&ruleHdl, MACSEC_HwCtxt[info.ctxIdx].ingress.tcamRuleHash[tcamIdx]);
                    ret = MACSEC_DrvRemoveSecYLookupRule(aConnHdl, ruleHdl, BCM_BOOL_TRUE);
                    if(BCM_ERR_OK != ret) {
                        break;
                    }
                    rxTcamRules &= ~(0x1UL << tcamIdx);
                }
                tcamIdx++;
            } while (((txTcamRules != 0UL)
                    || (rxTcamRules != 0UL))
                    && (tcamIdx < MACSEC_MAX_TCAM_ENTRIES));
        }

        if (BCM_ERR_OK == ret) {
            MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[secy->txVportIdx].ctrl1_0), 0UL);
            MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[secy->txVportIdx].ctrl2_0), 0UL);
            /* @event         misra_c_2012_rule_violation, zero_deref
               @issue         Explicit null dereferenced (FORWARD_NULL)
               @risk          Undefined behaviour
               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
            /* coverity[misra_c_2012_rule_violation] */
            /* coverity[zero_deref] */
            MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl1_0), 0UL);
            MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[secy->rxVPortIdx].ctrl2_0), 0UL);

            /* clear the SecY stats */
            MACSEC_DrvGetSecYStatsInternal(aConnHdl, info.hwIdx, info.secyIdx, NULL,
                    MACSEC_DIR_EGRESS, (uint32_t)BCM_BOOL_TRUE);
            MACSEC_DrvGetSecYStatsInternal(aConnHdl, info.hwIdx, info.secyIdx, NULL,
                    MACSEC_DIR_INGRESS, (uint32_t)BCM_BOOL_TRUE);
            /* free Tx and Rx vPorts */
            MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].egress.vportAllocMask,
                    secy->txVportIdx);
            MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].ingress.vportAllocMask,
                    secy->rxVPortIdx);
            BCM_MemSet(secy, 0x0U, sizeof(MACSEC_SecYContextType));
            MACSEC_SetBit(&MACSEC_HwCtxt[info.ctxIdx].secyAllocMask, info.secyIdx);
        }
    }
    return ret;
}

/**
    @brief SA enable

    @code{.unparsed}
    MACSEC_HandleInfoType info = {0UL}
    MACSEC_SecYContextType *secy
    MACSEC_SAContextType *newSA
    MACSEC_SAContextType *encodingSA
    uint32_t rxCAMIdx
    MACSEC_HwIDType hwID
    int32_t ret = BCM_ERR_INVAL_PARAMS

    MACSEC_HANDLE_INFO_INIT(info)
    newSA = MACSEC_DrvHandleToSA(aSA)
    if NULL != newSA
        ret = MACSEC_DrvGetHandleInfo(aSA, &info)
        BCM_ASSERT(BCM_ERR_OK == ret)
        hwID = info.hwIdx
        secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx]
        if MACSEC_DIR_EGRESS == info.dir
            newSA = &secy->txSC.sa[info.AN]
            if (uint32_t)BCM_BOOL_FALSE == secy->txSC.transmitting
                ret = MACSEC_DrvSASetInUse(aConnHdl, hwID, info.SCIdx, (uint32_t)BCM_BOOL_TRUE, info.SAIdx, 0UL,
                                           MACSEC_DIR_EGRESS);
                if BCM_ERR_OK == ret
                    newSA->inUse = (uint32_t)BCM_BOOL_TRUE
                    secy->txSC.encodingSA = info.AN
                    secy->txSC.transmitting = (uint32_t)BCM_BOOL_TRUE
            else
                encodingSA = &secy->txSC.sa[secy->txSC.encodingSA]
                if secy->txSC.encodingSA != info.AN
                    ret = MACSEC_DrvEgressSASwitch(aConnHdl, hwID, info.SCIdx, encodingSA, newSA)
                else
                    ret = MACSEC_DrvSASetInUse(aConnHdl, hwID, info.SCIdx, (uint32_t)BCM_BOOL_TRUE, info.SAIdx, 0UL,
                                               MACSEC_DIR_EGRESS);
                if BCM_ERR_OK == ret
                    newSA->inUse = (uint32_t)BCM_BOOL_TRUE
                    secy->txSC.encodingSA = info.AN
        else
            rxCAMIdx = secy->rxSC[info.SCIdx].rxCAMIdx
            ret = MACSEC_DrvSASetInUse(aConnHdl, hwID, rxCAMIdx, (uint32_t)BCM_BOOL_TRUE, info.SAIdx, info.AN, MACSEC_DIR_INGRESS)
            if BCM_ERR_OK == ret
                secy->rxSC[info.SCIdx].receiving = (uint32_t)BCM_BOOL_TRUE
                newSA->inUse = (uint32_t)BCM_BOOL_TRUE
                secy->rxSC[info.SCIdx].saActiveMask |= (0x1UL << info.AN)
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SAENABLE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvSAEnable(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSA)
{
    MACSEC_HandleInfoType info = {0UL};
    MACSEC_SecYContextType *secy;
    MACSEC_SAContextType *newSA;
    MACSEC_SAContextType *encodingSA;
    uint32_t rxCAMIdx;
    MACSEC_HwIDType hwID;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    MACSEC_HANDLE_INFO_INIT(info);
    newSA = MACSEC_DrvHandleToSA(aSA);
    if (NULL != newSA) {
        ret = MACSEC_DrvGetHandleInfo(aSA, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        hwID = info.hwIdx;
        secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx];
        if (MACSEC_DIR_EGRESS == info.dir) {
            newSA = &secy->txSC.sa[info.AN];
            if ((uint32_t)BCM_BOOL_FALSE == secy->txSC.transmitting) {
                /* Link this SA to SC in SCSA map register  and enable SC */
                ret = MACSEC_DrvSASetInUse(aConnHdl, hwID, info.SCIdx, (uint32_t)BCM_BOOL_TRUE, info.SAIdx, 0U, MACSEC_DIR_EGRESS);
                if (BCM_ERR_OK == ret) {
                    newSA->inUse = (uint32_t)BCM_BOOL_TRUE;
                    newSA->startTime = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_BRCM_GLOBAL_OFFSET_OF(tick));
                    secy->txSC.encodingSA = info.AN;
                    secy->txSC.transmitting = (uint32_t)BCM_BOOL_TRUE;
                }
            } else {
                /*
                 * need to switch to new SA: Link this SA to currently
                 * encoding SA in transform record and update sa_index_update
                 * field in the SCSA map register (while doing this, disable the
                 * SA expired interrupt in the XFORM record)
                 */
                encodingSA = &secy->txSC.sa[secy->txSC.encodingSA];
                if (secy->txSC.encodingSA != info.AN) {
                    ret = MACSEC_DrvEgressSASwitch(aConnHdl, hwID, info.SCIdx, encodingSA, newSA);
                } else {
                    /* just re-enable the same SA
                    ret = MACSEC_DrvSASetInUse(aConnHdl, hwID, info.SCIdx, TRUE, info.SAIdx, 0UL, MACSEC_DIR_EGRESS);
                    */
                    /* Not to re-enable, just return invalid state error */
                    ret = BCM_ERR_INVAL_STATE;
                }
                if (BCM_ERR_OK == ret) {
                    encodingSA->inUse = (uint32_t)BCM_BOOL_FALSE;
                    encodingSA->stopTime = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_BRCM_GLOBAL_OFFSET_OF(tick));
                    newSA->inUse = (uint32_t)BCM_BOOL_TRUE;
                    newSA->startTime = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_BRCM_GLOBAL_OFFSET_OF(tick));
                    secy->txSC.encodingSA = info.AN;
                }
            }
        } else {
            rxCAMIdx = secy->rxSC[info.SCIdx].rxCAMIdx;
            ret = MACSEC_DrvSASetInUse(aConnHdl, hwID, rxCAMIdx, (uint32_t)BCM_BOOL_TRUE, info.SAIdx, info.AN, MACSEC_DIR_INGRESS);
            if (BCM_ERR_OK == ret) {
                secy->rxSC[info.SCIdx].receiving = (uint32_t)BCM_BOOL_TRUE;
                newSA->inUse = (uint32_t)BCM_BOOL_TRUE;
                newSA->startTime = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_BRCM_GLOBAL_OFFSET_OF(tick));
                secy->rxSC[info.SCIdx].saActiveMask |= (0x1UL << info.AN);
            }
        }
    }
    return ret;
}

/**
    @brief Add SecY lookup rule in hardware

    @code{.unparsed}
    uint8_t ctxtIdx
    MACSEC_TCAMRuleType tcamRule
    uint32_t tcamRuleIdx
    uint32_t vPort
    MACSEC_HwContextType *hwCtxt
    int32_t ret = BCM_ERR_OK

    ctxtIdx = (uint8_t)MACSEC_ConvPort2CtxIdx(aID)
    hwCtxt = &MACSEC_HwCtxt[ctxtIdx]

    if info.dir is EGRESS
        tcamRuleIdx = MACSEC_ClrNGetBit(&hwCtxt->egress.tcamRuleAllocMask)
        vPort = hwCtxt->secy[aSecYIdx].txVportIdx
     else
        tcamRuleIdx = MACSEC_ClrNGetBit(&hwCtxt->ingress.tcamRuleAllocMask)
        vPort = hwCtxt->secy[aSecYIdx].rxVPortIdx

    if tcamRuleIdx < MACSEC_MAX_TCAM_ENTRIES
        BCM_MemSet(&tcamRule, 0x0U, sizeof(MACSEC_TCAMRuleType))
        tcamRule.pktType = MACSEC_ETHPKT_OTHER
        tcamRule.frameType = MACSEC_ETHFRAME_OTHER
        if MACSEC_DIR_EGRESS == aRule->dir
            tcamRule.matchEthFrameType = (uint32_t)BCM_BOOL_TRUE
        if (uint32_t)BCM_BOOL_TRUE == aRule->macSecTagged
            tcamRule.macSecTagged = (uint32_t)BCM_BOOL_TRUE
            tcamRule.matchTagged = (uint32_t)BCM_BOOL_TRUE
        if (uint32_t)BCM_BOOL_TRUE == aRule->vlanTagged
            tcamRule.numTags = MACSEC_ETHNUMTAGS_2
            tcamRule.matchNumTags = (uint32_t)BCM_BOOL_TRUE
        tcamRule.macDA = aRule->macDA
        tcamRule.macSA = aRule->macSA
        tcamRule.frameData = aRule->frameData
        tcamRule.matchMacDAMask = aRule->matchMacDAMask
        tcamRule.matchMacSAMask = aRule->matchMacSAMask
        tcamRule.matchFrameDataMask = aRule->matchFrameDataMask
        MACSEC_DrvAddTCAMRule(aConnHdl, aID, &tcamRule, tcamRuleIdx, vPort, aRule->dir)
        if dir is EGRESS
            hwCtxt->secy[aSecYIdx].txTcamRules |= (0x1UL << tcamRuleIdx)
        else
            hwCtxt->secy[aSecYIdx].rxTcamRules |= (0x1UL << tcamRuleIdx)
        *aRuleIdx = tcamRuleIdx
    else
        ret = BCM_ERR_NOMEM
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static int32_t MACSEC_DrvAddSecYLookupRuleInternal(BCM_HandleType aConnHdl, MACSEC_HwIDType aID,
                                                   uint8_t aSecYIdx, const SECY_RuleOpt1Type *const aRule,
                                                   uint32_t *const aRuleIdx)
{
    uint32_t ctxtIdx;
    MACSEC_TCAMRuleType tcamRule;
    uint32_t tcamRuleIdx;
    uint32_t vPort;
    MACSEC_HwContextType *hwCtxt;
    int32_t ret = BCM_ERR_OK;

    ctxtIdx = MACSEC_ConvPort2CtxIdx(aID);
    hwCtxt = &MACSEC_HwCtxt[ctxtIdx];

    if (SECY_RULE_DIR_EGRESS == aRule->dir) {
        tcamRuleIdx = MACSEC_ClrNGetBit(&hwCtxt->egress.tcamRuleAllocMask);
        vPort = hwCtxt->secy[aSecYIdx].txVportIdx;
    } else {
        tcamRuleIdx = MACSEC_ClrNGetBit(&hwCtxt->ingress.tcamRuleAllocMask);
        vPort = hwCtxt->secy[aSecYIdx].rxVPortIdx;
    }
    if (tcamRuleIdx < MACSEC_MAX_TCAM_ENTRIES) {
        BCM_MemSet(&tcamRule, 0x0U, sizeof(MACSEC_TCAMRuleType));
        tcamRule.frameType = MACSEC_ETHFRAME_OTHER;
        if (MACSEC_DIR_EGRESS == aRule->dir) {
            tcamRule.matchEthFrameType = (uint32_t)BCM_BOOL_TRUE;
        }
        if ((uint32_t)BCM_BOOL_TRUE == aRule->macSecTagged) {
            tcamRule.macSecTagged = (uint32_t)BCM_BOOL_TRUE;
            tcamRule.matchTagged = (uint32_t)BCM_BOOL_TRUE;
        }
        if ((uint32_t)BCM_BOOL_TRUE == aRule->vlanTagged) {
            tcamRule.numTags = MACSEC_ETHNUMTAGS_2;
            tcamRule.matchNumTags = (uint32_t)BCM_BOOL_TRUE;
        }
        tcamRule.macDA = aRule->macDA;
        tcamRule.macSA = aRule->macSA;
        tcamRule.frameData = aRule->frameData;
        tcamRule.matchMacDAMask = aRule->matchMacDAMask;
        tcamRule.matchMacSAMask = aRule->matchMacSAMask;
        tcamRule.matchFrameDataMask = aRule->matchFrameDataMask;
        MACSEC_DrvAddTCAMRule(aConnHdl, aID, &tcamRule, tcamRuleIdx, vPort, aRule->dir);
        if (SECY_RULE_DIR_EGRESS == aRule->dir) {
            hwCtxt->secy[aSecYIdx].txTcamRules |= (0x1UL << tcamRuleIdx);
        } else {
            hwCtxt->secy[aSecYIdx].rxTcamRules |= (0x1UL << tcamRuleIdx);
        }
        *aRuleIdx = tcamRuleIdx;
    } else {
        ret = BCM_ERR_NOMEM;
    }
    return ret;
}

/**
    @brief Add SecY lookup rule in hardware

    @code{.unparsed}
    MACSEC_HandleInfoType info = {0UL}
    SECY_RuleOpt1Type rule
    uint32_t ruleIdxEgress = 0UL
    uint32_t ruleIdxIngress = 0UL
    MACSEC_HwContextType *hwCtxt
    uint16_t crc
    uint16_t *ruleAddrEgress = NULL
    uint16_t *ruleAddrIngress = NULL
    uint8_t mask[6UL] = {0}
    BCM_BoolType matchAllTraffic = (uint32_t)BCM_BOOL_FALSE
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if aRule is not NULL
        MACSEC_HANDLE_INFO_INIT(info);
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if secy is not NULL
            ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
            if ret is ERR_OK
                hwCtxt = &MACSEC_HwCtxt[info.ctxIdx]
                BCM_MemCpy(&rule, aRule, sizeof(rule))
                if ((BCM_MemCmp(&rule.matchMacSAMask, mask, sizeof(mask)) == 0) &&
                    (BCM_MemCmp(&rule.matchMacDAMask, mask, sizeof(mask)) == 0) &&
                    (rule.matchFrameDataMask[0U] == 0UL) &&
                    (rule.matchFrameDataMask[1U] == 0UL))
                        if SECY_RULE_DIR_BOTH == aRule->dir
                            matchAllTraffic = (uint32_t)BCM_BOOL_TRUE
                if SECY_RULE_DIR_EGRESS & aRule->dir != 0UL
                    rule.dir = SECY_RULE_DIR_EGRESS
                    ret = MACSEC_DrvAddSecYLookupRuleInternal(aConnHdl, info.hwIdx, info.secyIdx,
                            &rule, &ruleIdxEgress)
                    if BCM_ERR_OK == ret
                        ruleAddrEgress = &hwCtxt->egress.tcamRuleHash[ruleIdxEgress]

                if SECY_RULE_DIR_INGRESS & aRule->dir) != 0UL
                    if (uint32_t)BCM_BOOL_TRUE == matchAllTraffic
                       rule.macSecTagged = (uint32_t)BCM_BOOL_TRUE
                    rule.dir = SECY_RULE_DIR_INGRESS
                    ret = MACSEC_DrvAddSecYLookupRuleInternal(aConnHdl, info.hwIdx, info.secyIdx,
                            &rule, &ruleIdxIngress)
                    if ret is ERR_OK
                        ruleAddrIngress = &hwCtxt->ingress.tcamRuleHash[ruleIdxIngress]

                if BCM_ERR_OK == ret
                    info.dir = aRule->dir
                    info.ruleType = MACSEC_RULEMASK_TCAM
                    info.ruleIdxEgress = ruleIdxEgress
                    info.ruleIdxIngress = ruleIdxIngress
                    *aRuleHandle = MACSEC_DrvCreateHandle(&info)
                    crc = MACSEC_DrvGenerateRuleHash(*aRuleHandle, ruleAddrEgress, ruleAddrIngress)
                    if NULL != ruleAddrEgress
                        *ruleAddrEgress = crc
                    if NULL != ruleAddrIngress
                        *ruleAddrIngress = crc
                    MACSEC_DrvHandleUpdateHash(aRuleHandle, crc)
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADD_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

int32_t MACSEC_DrvAddSecYLookupRule(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl,
                                                        const SECY_RuleOpt1Type *const aRule,
                                                             MACSEC_RuleHdlType *const aRuleHandle)
{
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info = {0UL};
    SECY_RuleOpt1Type rule;
    uint32_t ruleIdxEgress = 0UL;
    uint32_t ruleIdxIngress = 0UL;
    MACSEC_HwContextType *hwCtxt;
    uint16_t crc;
    uint16_t *ruleAddrEgress = NULL;
    uint16_t *ruleAddrIngress = NULL;
    uint8_t mask[6UL] = {0};
    BCM_BoolType matchAllTraffic = BCM_BOOL_FALSE;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (NULL != aRule) {
        if ((aRule->dir >= SECY_RULE_DIR_EGRESS) && (aRule->dir <= SECY_RULE_DIR_BOTH)) {
        MACSEC_HANDLE_INFO_INIT(info);
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
            if (BCM_ERR_OK == ret) {
                hwCtxt = &MACSEC_HwCtxt[info.ctxIdx];
                /* if all the match mask bits are zero, we assume that there is a single SecY
                    instance for this port and user wants to map all the traffic (egress
                    and ingress) to this SecY. In this case, for egress, we will add TCAM
                    rule with all compare masks set to 0 and for ingress we will addd TCAM
                    rule with only MACsec tagged mask set to 1 */
                BCM_MemCpy(&rule, aRule, sizeof(rule));
                if ((BCM_MemCmp(&rule.matchMacSAMask, mask, sizeof(mask)) == 0) &&
                    (BCM_MemCmp(&rule.matchMacDAMask, mask, sizeof(mask)) == 0) &&
                    (rule.matchFrameDataMask[0U] == 0UL) &&
                    (rule.matchFrameDataMask[1U] == 0UL)) {
                        if (SECY_RULE_DIR_BOTH == aRule->dir) {
                            matchAllTraffic = BCM_BOOL_TRUE;
                        }
                }
                if ((SECY_RULE_DIR_EGRESS & aRule->dir) != 0UL) {
                    rule.dir = SECY_RULE_DIR_EGRESS;
                    ret = MACSEC_DrvAddSecYLookupRuleInternal(aConnHdl, info.hwIdx, info.secyIdx,
                            &rule, &ruleIdxEgress);
                    if (BCM_ERR_OK == ret) {
                        ruleAddrEgress = &hwCtxt->egress.tcamRuleHash[ruleIdxEgress];
                    }
                }

                if ((SECY_RULE_DIR_INGRESS & aRule->dir) != 0UL) {
                    if (BCM_BOOL_TRUE == matchAllTraffic) {
                        rule.macSecTagged = (uint32_t)BCM_BOOL_TRUE;
                    }
                    rule.dir = SECY_RULE_DIR_INGRESS;
                    ret = MACSEC_DrvAddSecYLookupRuleInternal(aConnHdl, info.hwIdx, info.secyIdx,
                            &rule, &ruleIdxIngress);
                    if (BCM_ERR_OK == ret) {
                        ruleAddrIngress = &hwCtxt->ingress.tcamRuleHash[ruleIdxIngress];
                    }
                }

                if (BCM_ERR_OK == ret) {
                    info.dir            = (uint8_t)aRule->dir;
                    info.ruleType       = MACSEC_RULEMASK_TCAM;
                    info.ruleIdxEgress  = (uint8_t)(ruleIdxEgress & 0xFFUL);
                    info.ruleIdxIngress = (uint8_t)(ruleIdxIngress & 0xFFUL);
                    *aRuleHandle = MACSEC_DrvCreateHandle(&info);
                    crc = MACSEC_DrvGenerateRuleHash(*aRuleHandle, ruleAddrEgress, ruleAddrIngress);
                    if (NULL != ruleAddrEgress) {
                        *ruleAddrEgress = crc;
                    }
                    if (NULL != ruleAddrIngress) {
                        *ruleAddrIngress = crc;
                    }
                    MACSEC_DrvHandleUpdateHash(aRuleHandle, crc);
                }
            }
        }
        }
    }
    return ret;
}

/**
    @brief Set Control Port Enable/Disable

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_HandleInfoType info = {0UL}
    uint32_t vPort
    uint32_t flowCntrl0
    int32_t ret = BCM_ERR_INVAL_PARAMS

    MACSEC_HANDLE_INFO_INIT(info)
    secy = MACSEC_DrvHandleToSecY(aSecYHdl)
    if NULL != secy
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
        if BCM_ERR_OK == ret
            hwID = info.hwIdx
            rxvPort = secy->rxVPortIdx
            flowCntrl0 = MACSEC_Read(samflow_regs[rxvPort].ctrl1_0)
            if aEnable
                flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_TAGGED_DATA_MASK
                flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_UNTAGGED_DATA_MASK
            else
                flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_TAGGED_DATA_MASK
                flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_UNTAGGED_DATA_MASK
            MACSEC_Write(samflow_regs[rxvPort].ctrl1_0, flowCntrl0)

            flowCntrl0 = MACSEC_Read(samflow_regs[txvPort].ctrl1_0)
            if aEnable
                flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_DATA_PACKETS_MASK
            else
                flowCntrl0 &= ~MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_DATA_PACKETS_MASK
            MACSEC_Write(samflow_regs[txvPort].ctrl1_0, flowCntrl0)
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CTRL_PORT_SET_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/

int32_t MACSEC_DrvCtrlPortSet(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, uint32_t aEnable)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    MACSEC_HwIDType hwID;
    uint32_t txVPort;
    uint32_t rxVPort;
    uint32_t flowCntrl0;

    MACSEC_HANDLE_INFO_INIT(info);
    secy = MACSEC_DrvHandleToSecY(aSecYHdl);
    if (NULL != secy) {
        ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
        BCM_ASSERT(BCM_ERR_OK == ret);
        hwID = info.hwIdx;
        txVPort = secy->txVportIdx;
        rxVPort = secy->rxVPortIdx;
        secy->enCtrlPort = aEnable;

        /*Ingress*/

        /* @event         misra_c_2012_rule_violation, zero_deref
           @issue         Explicit null dereferenced (FORWARD_NULL)
           @risk          Undefined behaviour
           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
        /* coverity[misra_c_2012_rule_violation] */
        /* coverity[zero_deref] */
        flowCntrl0 = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[rxVPort].ctrl1_0));
        if ((uint32_t)BCM_BOOL_TRUE == aEnable) {
            flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_TAGGED_DATA_MASK;
            flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_UNTAGGED_DATA_MASK;
        } else {
            flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_TAGGED_DATA_MASK;
            flowCntrl0 &= ~MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_UNTAGGED_DATA_MASK;
        }

        /* @event         misra_c_2012_rule_violation, zero_deref
           @issue         Explicit null dereferenced (FORWARD_NULL)
           @risk          Undefined behaviour
           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
        /* coverity[misra_c_2012_rule_violation] */
        /* coverity[zero_deref] */
        MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[rxVPort].ctrl1_0), flowCntrl0);

        /*Egress*/

        /* @event         misra_c_2012_rule_violation, zero_deref
           @issue         Explicit null dereferenced (FORWARD_NULL)
           @risk          Undefined behaviour
           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
        /* coverity[misra_c_2012_rule_violation] */
        /* coverity[zero_deref] */
        flowCntrl0 = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[txVPort].ctrl1_0));
        if ((uint32_t)BCM_BOOL_TRUE == aEnable) {
            flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_DATA_PACKETS_MASK;
        } else {
            flowCntrl0 &= ~MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_ALLOW_DATA_PACKETS_MASK;
        }

        /* @event         misra_c_2012_rule_violation, zero_deref
           @issue         Explicit null dereferenced (FORWARD_NULL)
           @risk          Undefined behaviour
           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
        /* coverity[misra_c_2012_rule_violation] */
        /* coverity[zero_deref] */
        MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[txVPort].ctrl1_0), flowCntrl0);
    }
    /* Unused variable voilations. */
    (void)txVPort;
    (void)rxVPort;
    return ret;
}

/**
    @brief Create Receive Secure association

    This function creates a receive secure assocation
    record in the hardware and returns the handle
    to user.

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_SAContextType *saCtxt
    MACSEC_HandleInfoType info
    MACSEC_IngressXRecDescType desc
    uint8_t hwID
    uint8_t vPort
    uint32_t saIdx
    uint32_t saCntrlW = 0UL
    uint8_t zeroText[16UL]
    uint8_t hashKey[16UL]
    uint64_t sci64
    uint32_t saAddr
    uint16_t crc
    uint32_t i
    uint32_t implicit = (uint32_t)BCM_BOOL_FALSE
    uint32_t flowCntrl1
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if (NULL != aSCI) && (aAN <= MACSEC_AN_MAX) && (NULL != aKey) && (NULL != aSAHdl)
        MACSEC_HANDLE_INFO_INIT(info)
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
            hwID = info.hwIdx
            vPort = secy->rxVPortIdx
            ret = BCM_ERR_INVAL_PARAMS
            for i = 0UL i < MACSEC_MAX_RXSC_PER_SECY; i++
                if (uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active
                    if BCM_MemCmp(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType)) == 0U
                        break

            if i < MACSEC_MAX_RXSC_PER_SECY
                saIdx = MACSEC_ClrNGetBit(&MACSEC_HwCtxt[info.ctxIdx].ingress.saAllocMask)
                if saIdx < MACSEC_MAX_XFORM_RECORDS
                    saCntrlW |= (MACSEC_XRECW0_TOP_INGRESS << MACSEC_XRECW0_TOP_SHIFT)
                    saCntrlW |= MACSEC_XRECW0_IV0_PRESENT_MASK
                    saCntrlW |= MACSEC_XRECW0_IV1_PRESENT_MASK
                    saCntrlW |= MACSEC_XRECW0_UPDATE_SEQ_MASK
                    saCntrlW |= MACSEC_XRECW0_ENCRYPT_AUTH_MASK
                    saCntrlW |= MACSEC_XRECW0_LOAD_KEY_MASK
                    saCntrlW |= (0x2UL << MACSEC_XRECW0_DIGEST_TYPE_SHIFT)
                    saCntrlW |= (MACSEC_XRECW0_AUTH_ALGO_AES_GHASH << MACSEC_XRECW0_AUTH_ALGO_SHIFT)
                    saCntrlW |= ((uint32_t)aAN << MACSEC_XRECW0_AN_SHIFT)
                    saCntrlW |= MACSEC_XRECW0_SEQ_MASK
                    saCntrlW |= MACSEC_XRECW0_CNTXID_PRESENT_MASK

                    if SECY_VALIDATE_FRAME_NULL == secy->validateFrame
                        saCntrlW |= MACSEC_XRECW0_KEEP_SECTAG_MASK
                        saCntrlW |= MACSEC_XRECW0_KEEP_ICV_MASK
                    if secy->cipher == XPN128 or secy->cipher == XPN256
                        saCntrlW |= MACSEC_XRECW0_SEQ_64BIT << MACSEC_XRECW0_SEQ_TYPE_SHIFT
                        saCntrlW |= MACSEC_XRECW0_IV2_PRESENT_MASK
                    else
                        saCntrlW |= MACSEC_XRECW0_SEQ_32BIT << MACSEC_XRECW0_SEQ_TYPE_SHIFT
                        saCntrlW |= MACSEC_XRECW0_IV_FORMAT_MASK

                    BCM_MemSet(&desc, 0x0U, sizeof(desc))
                    BCM_MemSet(zeroText, 0x0U, sizeof(zeroText))

                    sci64 = MACSEC_DrvSCIToU64(aSCI)
                    saCtxt = &secy->rxSC[i].sa[aAN]
                    if (aSCI->portNum == 0UL) || (aSCI->portNum == 1UL) {
                        implicit = (uint32_t)BCM_BOOL_TRUE
                    switch (secy->cipherID)
                        case SECY_CIPHER_GCM_AES_128:
                            saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES128 <<
                                        MACSEC_XRECW0_CRYPTO_ALGO_SHIFT)
                            AES_ECBEncryptBlock(CRYPTO_KEY_AES128, aKey->sak, zeroText, hashKey)
                            MACSEC_DrvCopyKeyToRec(&desc.recAes128.key0, aKey->sak, 16UL)
                            MACSEC_DrvCopyKeyToRec(&desc.recAes128.hKey0, hashKey, 16UL)
                            desc.recAes128.seq = aLowestPN
                            if (uint32_t)BCM_BOOL_TRUE == implicit
                                desc.recAes128.iv0 = (sci64 & 0xFFFFFFFFULL)
                                desc.recAes128.iv1 = (sci64 >> 32UL) & 0xFFFFFFFFUL
                            desc.recAes128.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK
                            if 0UL != secy->replayWindow
                                desc.recAes128.mask = secy->replayWindow
                            saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_IngressXRecAES128Type,
                                                                saUpdCtrl)
                            saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES128Type, seq)
                            saCtxt->xRecReplayWindowOff = (uint8_t) offsetof(MACSEC_IngressXRecAES128Type, mask)
                            break
                        case SECY_CIPHER_GCM_AES_256:
                            saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES256 <<
                                         MACSEC_XRECW0_CRYPTO_ALGO_SHIFT)
                            AES_ECBEncryptBlock(CRYPTO_KEY_AES256, aKey->sak, zeroText, hashKey)
                            MACSEC_DrvCopyKeyToRec(&desc.recAes256.key0, aKey->sak, 32UL)
                            MACSEC_DrvCopyKeyToRec(&desc.recAes256.hKey0, hashKey, 16UL)
                            desc.recAes256.seq = aLowestPN
                            if TRUE == implicit
                                desc.recAes256.iv0 = (sci64 & 0xFFFFFFFFULL)
                                desc.recAes256.iv1 = (sci64 >> 32UL) & 0xFFFFFFFFUL
                            desc.recAes256.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK
                            if 0UL != secy->replayWindow
                                desc.recAes256.mask = secy->replayWindow
                            saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_IngressXRecAES256Type,
                                                                saUpdCtrl)
                            saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES256Type, seq)
                            saCtxt->xRecReplayWindowOff = (uint8_t) offsetof(MACSEC_IngressXRecAES256Type, mask)
                            break
                        case SECY_CIPHER_GCM_AES_XPN_128:
                            saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES128 <<
                                        MACSEC_XRECW0_CRYPTO_ALGO_SHIFT)
                            saCntrlW |= MACSEC_XRECW0_IV2_PRESENT_MASK
                            AES_ECBEncryptBlock(CRYPTO_KEY_AES128, aKey->sak, zeroText, hashKey)
                            MACSEC_DrvCopyKeyToRec(&desc.recAes128Xpn.key0, aKey->sak, 16UL)
                            MACSEC_DrvCopyKeyToRec(&desc.recAes128Xpn.hKey0, hashKey, 16UL)
                            desc.recAes128Xpn.seq0 = aLowestPN & 0xFFFFFFFFFULL
                            desc.recAes128Xpn.seq1 = (aLowestPN >> 32UL) & 0xFFFFFFFFULL
                            desc.recAes128Xpn.is0 = MACSEC_BYTES_TO_LE32(aKey->salt, 0UL) ^ (aSSCI)
                            desc.recAes128Xpn.is1 = MACSEC_BYTES_TO_LE32(aKey->salt, 4UL)
                            desc.recAes128Xpn.is2 = MACSEC_BYTES_TO_LE32(aKey->salt, 8UL)
                            desc.recAes128Xpn.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK
                            if 0UL != secy->replayWindow
                                desc.recAes128Xpn.mask = secy->replayWindow
                            saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_IngressXRecAES128XpnType,
                                                                saUpdCtrl)
                            saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES128XpnType, seq0)
                            saCtxt->xRecSeq1Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES128XpnType, seq1)
                            saCtxt->xRecReplayWindowOff = (uint8_t) offsetof(MACSEC_IngressXRecAES128XpnType,
                                                                   mask)
                            break
                        case SECY_CIPHER_GCM_AES_XPN_256:
                            saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES256 <<
                                         MACSEC_XRECW0_CRYPTO_ALGO_SHIFT)
                            saCntrlW |= MACSEC_XRECW0_IV2_PRESENT_MASK
                            AES_ECBEncryptBlock(CRYPTO_KEY_AES256, aKey->sak, zeroText, hashKey)
                            MACSEC_DrvCopyKeyToRec(&desc.recAes256Xpn.key0, aKey->sak, 32UL)
                            MACSEC_DrvCopyKeyToRec(&desc.recAes256Xpn.hKey0, hashKey, 16UL)
                            desc.recAes256Xpn.seq0 = aLowestPN & 0xFFFFFFFFFULL
                            desc.recAes256Xpn.seq1 = (aLowestPN >> 32UL) & 0xFFFFFFFFULL
                            desc.recAes256Xpn.is0 = MACSEC_BYTES_TO_LE32((aKey->salt), 0UL) ^ (aSSCI)
                            desc.recAes256Xpn.is1 = MACSEC_BYTES_TO_LE32((aKey->salt), 4UL)
                            desc.recAes256Xpn.is2 = MACSEC_BYTES_TO_LE32((aKey->salt), 8UL)
                            desc.recAes256Xpn.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK
                            if 0UL != secy->replayWindow
                                desc.recAes256Xpn.mask = secy->replayWindow
                            saCtxt->xRecSAUpdCtrlOff =
                                (uint8_t) offsetof(MACSEC_IngressXRecAES256XpnType, saUpdCtrl)
                            saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES256XpnType, seq0)
                            saCtxt->xRecSeq1Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES256XpnType, seq1)
                            saCtxt->xRecReplayWindowOff = (uint8_t) offsetof(MACSEC_IngressXRecAES256XpnType,
                                                                   mask)
                            break
                        default:
                            BCM_ASSERT(0U)
                            break
                    }
                    saCtxt->saIdx = saIdx
                    info.SAIdx = saIdx
                    info.SCIdx = i
                    info.AN = aAN
                    info.dir = MACSEC_DIR_INGRESS
                    *aSAHdl = MACSEC_DrvCreateHandle(&info)

                    saCtxt->contextID = MACSEC_XREC_ID_CREATE(info.secyIdx, info.AN)
                    saCtxt->ssci = aSSCI
                    BCM_MemCpy(saCtxt->keyID, aKey->keyId, sizeof(saCtxt->keyID))

                    desc.words[0UL] = saCntrlW
                    desc.words[1UL] = saCtxt->contextID
                    desc.words[saCtxt->xRecReplayWindowOff >> 2UL] = secy->replayWindow
                    MACSEC_DrvXRecUpdate(aConnHdl, hwID, saIdx, 0UL, MACSEC_XREC_WORDS_PER_ENTRY,
                                         desc.words, MACSEC_DIR_INGRESS)
                    flowCntrl1 = secy->confOffset << MACSEC_I_FCTRL_SAM_FLOW_CTRL2_10_CONFIDENTIALITY_OFFSET_SHIFT & MACSEC_I_FCTRL_SAM_FLOW_CTRL2_10_CONFIDENTIALITY_OFFSET_MASK
                    flowCntrl1 |= secy->cfg.preSecTagAuthLen << MACSEC_I_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_LEN_SHIFT & MACSEC_I_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_LEN_MASK
                    flowCntrl1 |= secy->cfg.preSecTagAuthStart << MACSEC_I_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_START_SHIFT & MACSEC_I_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_START_MASK
                    flowCntrl1 |= secy->cfg.secTagOffset << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_SECTAG_OFFSET_SHIFT & MACSEC_I_FCTRL_SAM_FLOW_CTRL2_10_SECTAG_OFFSET_MASK
                    MACSEC_Write(MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[vPort].ctrl2_0), flowCntrl1)

                    saCtxt->createdTime = MACSEC_Read(MACSEC_I_BRCM_GLOBAL_OFFSET_OF(tick))
                    saCtxt->startTime = saCtxt->createdTime
                    saCtxt->stopTime  = saCtxt->createdTime
                    saCtxt->trafficStartTime = 0
                    saCtxt->trafficStopTime  = 0
                    ret = BCM_ERR_OK
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CTRL_PORT_SET_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvCreateRxSA(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, MACSEC_ANType aAN,
                             uint64_t aLowestPN, SECY_SCIType *aSCI, uint32_t aSSCI,
                             const SECY_KeyType *const aKey, MACSEC_SAHdlType *const aSAHdl)
{
    MACSEC_SecYContextType *secy;
    MACSEC_SAContextType *saCtxt;
    MACSEC_HandleInfoType info;
    MACSEC_IngressXRecDescType desc;
    uint8_t  hwID;
    uint8_t  vPort;
    uint32_t saIdx;
    uint32_t saCntrlW = 0UL;
    uint8_t  zeroText[16UL];
    uint64_t sci64;
    int      i;
    uint32_t flowCntrl1;
    uint32_t implicit = (uint32_t)BCM_BOOL_FALSE;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;
    uint8_t  aHKey[16UL] = {0U};

    if ((NULL != aSCI) && (aAN <= MACSEC_AN_MAX) && (NULL != aKey) && (NULL != aSAHdl)) {
        MACSEC_HANDLE_INFO_INIT(info);
        ret = MACSEC_SECYGetAESAuthKey(aConnHdl, aSecYHdl, &aKey->sak[0], &aHKey[0]);
        if(ret != BCM_ERR_OK) {
            ret = BCM_ERR_INVAL_MAGIC;
        } else {
            secy = MACSEC_DrvHandleToSecY(aSecYHdl);
            if (NULL != secy) {
                ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
                BCM_ASSERT(BCM_ERR_OK == ret);
                hwID  = (uint8_t)(info.hwIdx & 0xFFUL);
                vPort = (uint8_t)(secy->rxVPortIdx & 0xFFUL);
                ret   = BCM_ERR_INVAL_PARAMS;
                /* is RxSC created? */
                for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                    if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active) {
                        if (BCM_MemCmp(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType)) == 0) {
                            break;
                        }
                    }
                }
                if (i < MACSEC_MAX_RXSC_PER_SECY) {
                    /* Allocate SA record (XFORM record) for this SA */
                    saIdx = MACSEC_ClrNGetBit(&MACSEC_HwCtxt[info.ctxIdx].ingress.saAllocMask);
                    if (saIdx < MACSEC_MAX_XFORM_RECORDS) {
                        saCntrlW |= (MACSEC_XRECW0_TOP_INGRESS << MACSEC_XRECW0_TOP_SHIFT);
                        saCntrlW |= MACSEC_XRECW0_IV0_PRESENT_MASK;
                        saCntrlW |= MACSEC_XRECW0_IV1_PRESENT_MASK;
                        saCntrlW |= MACSEC_XRECW0_UPDATE_SEQ_MASK;
                        saCntrlW |= MACSEC_XRECW0_ENCRYPT_AUTH_MASK;
                        saCntrlW |= MACSEC_XRECW0_LOAD_KEY_MASK;
                        saCntrlW |= (0x2UL << MACSEC_XRECW0_DIGEST_TYPE_SHIFT);
                        saCntrlW |= (MACSEC_XRECW0_AUTH_ALGO_AES_GHASH << MACSEC_XRECW0_AUTH_ALGO_SHIFT);
                        saCntrlW |= ((uint32_t)aAN << MACSEC_XRECW0_AN_SHIFT);
                        saCntrlW |= MACSEC_XRECW0_SEQ_MASK;
                        saCntrlW |= MACSEC_XRECW0_CNTXID_PRESENT_MASK;

                        if (SECY_VALIDATE_FRAME_NULL == secy->validateFrame) {
                            saCntrlW |= MACSEC_XRECW0_KEEP_SECTAG_MASK;
                            saCntrlW |= MACSEC_XRECW0_KEEP_ICV_MASK;
                        }

                        if (BCM_BOOL_TRUE == MACSEC_DrvIsXpnCipherSuiteSet(secy)) {
                            saCntrlW |= MACSEC_XRECW0_SEQ_64BIT << MACSEC_XRECW0_SEQ_TYPE_SHIFT;
                            saCntrlW |= MACSEC_XRECW0_IV2_PRESENT_MASK;
                        } else {
                            saCntrlW |= MACSEC_XRECW0_SEQ_32BIT << MACSEC_XRECW0_SEQ_TYPE_SHIFT;
                            saCntrlW |= MACSEC_XRECW0_IV_FORMAT_MASK;
                        }

                        BCM_MemSet(&desc, 0x0U, sizeof(desc));
                        BCM_MemSet(zeroText, 0x0U, sizeof(zeroText));

                        sci64 = MACSEC_DrvSCIToU64(aSCI);
                        saCtxt = &secy->rxSC[i].sa[aAN];
                        if ((uint32_t)BCM_BOOL_FALSE == secy->cfg.includeSCI) {
                            implicit = (uint32_t)BCM_BOOL_TRUE;
                        }
                        switch (secy->cipherID) {
                            case SECY_CIPHER_GCM_AES_128:
                                saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES128 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT);
                                MACSEC_DrvCopyKeyToRec(&desc.recAes128.key0, aKey->sak, 16UL);
                                MACSEC_DrvCopyKeyToRec(&desc.recAes128.hKey0, aHKey, 16UL);
                                desc.recAes128.seq = (uint32_t)(aLowestPN & 0xFFFFFFFFUL);
                                if ((uint32_t)BCM_BOOL_TRUE == implicit) {
                                    desc.recAes128.iv0 = (uint32_t)(sci64 & 0xFFFFFFFFUL);
                                    desc.recAes128.iv1 = (uint32_t)(sci64 >> 32UL) & 0xFFFFFFFFUL;
                                }
                                desc.recAes128.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK;
                                if (0UL != secy->replayWindow) {
                                    desc.recAes128.mask = secy->replayWindow;
                                }
                                saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_IngressXRecAES128Type,
                                                                    saUpdCtrl);
                                saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES128Type, seq);
                                saCtxt->xRecReplayWindowOff = (uint8_t) offsetof(MACSEC_IngressXRecAES128Type, mask);
                                break;
                            case SECY_CIPHER_GCM_AES_256:
                                saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES256 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT);
                                MACSEC_DrvCopyKeyToRec(&desc.recAes256.key0, aKey->sak, 32UL);
                                MACSEC_DrvCopyKeyToRec(&desc.recAes256.hKey0, aHKey, 16UL);
                                desc.recAes256.seq = (uint32_t)(aLowestPN & 0xFFFFFFFFUL);
                                if ((uint32_t)BCM_BOOL_TRUE == implicit) {
                                    desc.recAes256.iv0 = (uint32_t)(sci64 & 0xFFFFFFFFUL);
                                    desc.recAes256.iv1 = (uint32_t)(sci64 >> 32UL) & 0xFFFFFFFFUL;
                                }
                                desc.recAes256.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK;
                                if (0UL != secy->replayWindow) {
                                    desc.recAes256.mask = secy->replayWindow;
                                }
                                saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_IngressXRecAES256Type,
                                                                    saUpdCtrl);
                                saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES256Type, seq);
                                saCtxt->xRecReplayWindowOff = (uint8_t) offsetof(MACSEC_IngressXRecAES256Type, mask);
                                break;
                            case SECY_CIPHER_GCM_AES_XPN_128:
                                saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES128 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT);
                                saCntrlW |= MACSEC_XRECW0_IV2_PRESENT_MASK;
                                MACSEC_DrvCopyKeyToRec(&desc.recAes128Xpn.key0, aKey->sak, 16UL);
                                MACSEC_DrvCopyKeyToRec(&desc.recAes128Xpn.hKey0, aHKey, 16UL);
                                desc.recAes128Xpn.seq0 = (uint32_t) (aLowestPN & 0xFFFFFFFFUL);
                                desc.recAes128Xpn.seq1 = (uint32_t)((aLowestPN >> 32UL) & 0xFFFFFFFFUL);
                                desc.recAes128Xpn.is0 = MACSEC_BYTES_TO_LE32(aKey->salt, 0UL) ^ (aSSCI);
                                desc.recAes128Xpn.is1 = MACSEC_BYTES_TO_LE32(aKey->salt, 4UL);
                                desc.recAes128Xpn.is2 = MACSEC_BYTES_TO_LE32(aKey->salt, 8UL);
                                desc.recAes128Xpn.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK;
                                if (0UL != secy->replayWindow) {
                                    desc.recAes128Xpn.mask = secy->replayWindow;
                                }
                                saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_IngressXRecAES128XpnType,
                                                                    saUpdCtrl);
                                saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES128XpnType, seq0);
                                saCtxt->xRecSeq1Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES128XpnType, seq1);
                                saCtxt->xRecReplayWindowOff = (uint8_t) offsetof(MACSEC_IngressXRecAES128XpnType,
                                                                       mask);
                                break;
                            case SECY_CIPHER_GCM_AES_XPN_256:
                                saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES256 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT);
                                saCntrlW |= MACSEC_XRECW0_IV2_PRESENT_MASK;
                                MACSEC_DrvCopyKeyToRec(&desc.recAes256Xpn.key0, aKey->sak, 32UL);
                                MACSEC_DrvCopyKeyToRec(&desc.recAes256Xpn.hKey0, aHKey, 16UL);
                                desc.recAes256Xpn.seq0 = (uint32_t)(aLowestPN & 0xFFFFFFFFUL);
                                desc.recAes256Xpn.seq1 = (uint32_t)((aLowestPN >> 32UL) & 0xFFFFFFFFUL);
                                desc.recAes256Xpn.is0 = MACSEC_BYTES_TO_LE32((aKey->salt), 0UL) ^ (aSSCI);
                                desc.recAes256Xpn.is1 = MACSEC_BYTES_TO_LE32((aKey->salt), 4UL);
                                desc.recAes256Xpn.is2 = MACSEC_BYTES_TO_LE32((aKey->salt), 8UL);
                                desc.recAes256Xpn.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK;
                                if (0UL != secy->replayWindow) {
                                    desc.recAes256Xpn.mask = secy->replayWindow;
                                }
                                saCtxt->xRecSAUpdCtrlOff =
                                    (uint8_t) offsetof(MACSEC_IngressXRecAES256XpnType, saUpdCtrl);
                                saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES256XpnType, seq0);
                                saCtxt->xRecSeq1Offset = (uint8_t) offsetof(MACSEC_IngressXRecAES256XpnType, seq1);
                                saCtxt->xRecReplayWindowOff = (uint8_t) offsetof(MACSEC_IngressXRecAES256XpnType,
                                                                       mask);
                                break;
                            default:
                                BCM_ASSERT(0UL == (uint32_t)&aConnHdl);
                                break;
                        }

                        /* create handle and use the handle as unique context ID in SA record */
                        saCtxt->saIdx = (uint16_t)saIdx;
                        info.SAIdx    =  (uint8_t)saIdx;
                        info.SCIdx    =  (uint8_t)i;
                        info.AN       =  (uint8_t)aAN;
                        info.dir      =  (uint8_t)MACSEC_DIR_INGRESS;
                        *aSAHdl = MACSEC_DrvCreateHandle(&info);
                        saCtxt->contextID = MACSEC_XREC_ID_CREATE(info.secyIdx, info.AN);
                        saCtxt->ssci = aSSCI;
                        saCtxt->handle = *aSAHdl;
                        BCM_MemCpy(saCtxt->keyID, aKey->keyId, sizeof(saCtxt->keyID));

                        desc.words[0UL] = saCntrlW; /* first word in record is control word */
                        desc.words[1UL] = saCtxt->contextID;  /* second word is unique ID */
                        desc.words[saCtxt->xRecReplayWindowOff >> 2UL] = secy->replayWindow;
                        /* write the xform record */
                        MACSEC_DrvXRecUpdate(aConnHdl, (uint32_t)hwID, (uint32_t)saIdx, 0UL, MACSEC_XREC_WORDS_PER_ENTRY,
                                             desc.words, MACSEC_DIR_INGRESS);
                        /* configure ingress flow */
                        flowCntrl1 = ((uint32_t)secy->confOffset << MACSEC_I_FCTRL_SAM_FLOW_CTRL2_10_CONFIDENTIALITY_OFFSET_SHIFT) &
                                      MACSEC_I_FCTRL_SAM_FLOW_CTRL2_10_CONFIDENTIALITY_OFFSET_MASK;
                        flowCntrl1 |= ((uint32_t)secy->cfg.preSecTagAuthLen << MACSEC_I_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_LEN_SHIFT) &
                                      MACSEC_I_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_LEN_MASK;
                        flowCntrl1 |= ((uint32_t)secy->cfg.preSecTagAuthStart << MACSEC_I_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_START_SHIFT) &
                                      MACSEC_I_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_START_MASK;
                        flowCntrl1 |= ((uint32_t)secy->cfg.secTagOffset << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_SECTAG_OFFSET_SHIFT) &
                                      MACSEC_I_FCTRL_SAM_FLOW_CTRL2_10_SECTAG_OFFSET_MASK;
                        /* @event         misra_c_2012_rule_violation, zero_deref
                           @issue         Explicit null dereferenced (FORWARD_NULL)
                           @risk          Undefined behaviour
                           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                        /* coverity[misra_c_2012_rule_violation] */
                        /* coverity[zero_deref] */
                        MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE,
                                               MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[vPort].ctrl2_0),  flowCntrl1);
                        (void) vPort;  /* this line is for MISRA C-2012 Rule 2.2: Assigning value to vPort, but that value is not used */

                        /* Capture the SA timing */
                        saCtxt->createdTime = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_BRCM_GLOBAL_OFFSET_OF(tick));
                        saCtxt->startTime = saCtxt->createdTime;
                        saCtxt->stopTime  = saCtxt->createdTime;
                        saCtxt->trafficStartTime = 0UL;
                        saCtxt->trafficStopTime  = 0UL;
                        ret = BCM_ERR_OK;
                    }
                }
            }
        }
    }
    return ret;
}
/**
    @brief Configures PN threshold value in the hardware

    @code{.unparsed}
    MACSEC_HandleInfoType info = {0UL}
    MACSEC_SecYContextType *secy
    MACSEC_SAContextType *sa
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if 0UL != aPNThrld
        MACSEC_HANDLE_INFO_INIT(info)
        sa = MACSEC_DrvHandleToSA(aSAHdl)
        if NULL != sa
            if TRUE == sa->inUse
                ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info)
                BCM_ASSERT(BCM_ERR_OK == ret)
                secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx]
                if current cipher suite is XPN128 or XPN256
                    MACSEC_E_EIP62_REGS[info.hwIdx]->seq_num_threshold64_lo = aPNThrld & 0xFFFFFFFFUL
                    MACSEC_E_EIP62_REGS[info.hwIdx]->seq_num_threshold64_hi =
                        ((aPNThrld >> 32UL) & 0xFFFFFFFFUL)
                else
                    if aPNThrld < 0xFFFFFFFF
                        MACSEC_E_EIP62_REGS[info.hwIdx]->seq_num_threshold = aPNThrld
                ret = BCM_ERR_OK
            else
                ret = BCM_ERR_INVAL_STATE
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_SET_PNTHRESHOLD_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvSetPNThreshold(BCM_HandleType aConnHdl, MACSEC_SAHdlType aSAHdl, uint64_t aPNThrld)
{
    MACSEC_HandleInfoType info = {0UL};
    MACSEC_SecYContextType *secy;
    MACSEC_SAContextType *sa;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (0UL != aPNThrld) {
        MACSEC_HANDLE_INFO_INIT(info);
        sa = MACSEC_DrvHandleToSA(aSAHdl);
        if (NULL != sa) {
            if ((uint32_t)BCM_BOOL_TRUE == sa->inUse) {
                ret = MACSEC_DrvGetHandleInfo(aSAHdl, &info);
                BCM_ASSERT(BCM_ERR_OK == ret);
                secy = &MACSEC_HwCtxt[info.ctxIdx].secy[info.secyIdx];
                if (BCM_BOOL_TRUE == MACSEC_DrvIsXpnCipherSuiteSet(secy)) {
                    MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(seq_num_threshold64_lo), (uint32_t) aPNThrld & 0xFFFFFFFFUL);
                    MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(seq_num_threshold64_hi), (uint32_t)(aPNThrld >> 32UL) & 0xFFFFFFFFUL);
                } else {
                    if (aPNThrld < 0xFFFFFFFFULL) {
                        MACSEC_Write(aConnHdl, info.hwIdx, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(seq_num_threshold), (uint32_t)aPNThrld);
                    }
                }
                ret = BCM_ERR_OK;
            } else {
                ret = BCM_ERR_INVAL_STATE;
            }
        }
    }
    return ret;
}

/**
    @brief Create trasmit secure association

    @code{.unparsed}
    MACSEC_SecYContextType *secy
    MACSEC_SAContextType *saCtxt
    MACSEC_HandleInfoType info
    MACSEC_EgressXRecDescType desc
    uint8_t hwID
    uint8_t vPort
    uint32_t saIdx
    uint32_t saCntrlW = 0UL
    uint8_t zeroText[16UL]
    uint8_t hashKey[16UL]
    uint64_t sci64
    uint32_t saAddr
    uint16_t crc
    uint32_t flowCntrl0
    uint32_t flowCntrl1
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if (NULL != aSCI) && (aAN <= MACSEC_AN_MAX) && (NULL != aKey) && (NULL != aSA)
        MACSEC_HANDLE_INFO_INIT(info)
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
            BCM_ASSERT(BCM_ERR_OK == ret)
            hwID = info.hwIdx
            vPort = secy->txVportIdx
            if BCM_MemCmp(aSCI, &secy->txSC.sci, sizeof(SECY_SCIType)) == 0UL
                if (uint32_t)BCM_BOOL_TRUE != secy->txSC.sa[aAN].inUse
                    saIdx = MACSEC_ClrNGetBit(&MACSEC_HwCtxt[info.ctxIdx].egress.saAllocMask)
                    if saIdx < MACSEC_MAX_XFORM_RECORDS
                        saCntrlW |= (MACSEC_XRECW0_TOP_EGRESS << MACSEC_XRECW0_TOP_SHIFT)
                        saCntrlW |= MACSEC_XRECW0_IV0_PRESENT_MASK
                        saCntrlW |= MACSEC_XRECW0_IV1_PRESENT_MASK
                        saCntrlW |= MACSEC_XRECW0_UPDATE_SEQ_MASK
                        saCntrlW |= MACSEC_XRECW0_IV_FORMAT_MASK
                        saCntrlW |= MACSEC_XRECW0_ENCRYPT_AUTH_MASK
                        saCntrlW |= MACSEC_XRECW0_LOAD_KEY_MASK
                        saCntrlW |= (0x2UL << MACSEC_XRECW0_DIGEST_TYPE_SHIFT)
                        saCntrlW |= (MACSEC_XRECW0_AUTH_ALGO_AES_GHASH << MACSEC_XRECW0_AUTH_ALGO_SHIFT)
                        saCntrlW |= ((uint32_t)aAN << MACSEC_XRECW0_AN_SHIFT)
                        saCntrlW |= MACSEC_XRECW0_CNTXID_PRESENT_MASK
                        if secy->cipher == XPN128 OR secy->cipher == XPN256
                            saCntrlW |= MACSEC_XRECW0_SEQ_64BIT << MACSEC_XRECW0_SEQ_TYPE_SHIFT
                        else
                            saCntrlW |= MACSEC_XRECW0_SEQ_32BIT << MACSEC_XRECW0_SEQ_TYPE_SHIFT
                        BCM_MemSet(&desc, 0x0U, sizeof(desc))
                        BCM_MemSet(zeroText, 0x0U, sizeof(zeroText))
                        sci64 = MACSEC_DrvSCIToU64(aSCI)
                        saCtxt = &secy->txSC.sa[aAN]

                        switch (secy->cipherID)
                            case SECY_CIPHER_GCM_AES_128:
                                saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES128 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT)
                                AES_ECBEncryptBlock(CRYPTO_KEY_AES128, aKey->sak, zeroText, hashKey)
                                MACSEC_DrvCopyKeyToRec(&desc.recAes128.key0, aKey->sak, 16UL)
                                MACSEC_DrvCopyKeyToRec(&desc.recAes128.hKey0, hashKey, 16UL)
                                desc.recAes128.seq = aNextPN
                                desc.recAes128.iv0 = (sci64 & 0xFFFFFFFFULL)
                                desc.recAes128.iv1 = (sci64 >> 32UL) & 0xFFFFFFFFUL
                                desc.recAes128.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK
                                saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_EgressXRecAES128Type,
                                                           saUpdCtrl)
                                saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES128Type, seq)
                                break
                            case SECY_CIPHER_GCM_AES_256:
                                saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES256 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT)
                                AES_ECBEncryptBlock(CRYPTO_KEY_AES256, aKey->sak, zeroText, hashKey)
                                MACSEC_DrvCopyKeyToRec(&desc.recAes256.key0, aKey->sak, 32UL)
                                MACSEC_DrvCopyKeyToRec(&desc.recAes256.hKey0, hashKey, 16UL)
                                desc.recAes256.seq = aNextPN
                                desc.recAes256.iv0 = (sci64 & 0xFFFFFFFFULL)
                                desc.recAes256.iv1 = (sci64 >> 32UL) & 0xFFFFFFFFUL
                                desc.recAes256.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK
                                saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_EgressXRecAES256Type,
                                                                    saUpdCtrl)
                                saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES256Type, seq)
                                break
                            case SECY_CIPHER_GCM_AES_XPN_128:
                                saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES128 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT)
                                AES_ECBEncryptBlock(CRYPTO_KEY_AES128, aKey->sak, zeroText, hashKey)
                                MACSEC_DrvCopyKeyToRec(&desc.recAes128Xpn.key0, aKey->sak, 16UL)
                                MACSEC_DrvCopyKeyToRec(&desc.recAes128Xpn.hKey0, hashKey, 16UL)
                                desc.recAes128Xpn.seq0 = aNextPN & 0xFFFFFFFFFULL
                                desc.recAes128Xpn.seq1 = (aNextPN >> 32UL) & 0xFFFFFFFFULL
                                desc.recAes128Xpn.is0 = MACSEC_BYTES_TO_LE32((aKey->salt), 0UL) ^ (aSSCI)
                                desc.recAes128Xpn.is1 = MACSEC_BYTES_TO_LE32((aKey->salt), 4UL)
                                desc.recAes128Xpn.is2 = MACSEC_BYTES_TO_LE32((aKey->salt), 8UL)
                                desc.recAes128Xpn.iv0 = (sci64 & 0xFFFFFFFFULL)
                                desc.recAes128Xpn.iv1 = (sci64 >> 32UL) & 0xFFFFFFFFUL
                                desc.recAes128Xpn.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK
                                saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_EgressXRecAES128XpnType,
                                                                    saUpdCtrl)
                                saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES128XpnType, seq0)
                                saCtxt->xRecSeq1Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES128XpnType, seq1)
                                break
                            case SECY_CIPHER_GCM_AES_XPN_256:
                                saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES256 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT)
                                AES_ECBEncryptBlock(CRYPTO_KEY_AES256, aKey->sak, zeroText, hashKey)
                                MACSEC_DrvCopyKeyToRec(&desc.recAes256Xpn.key0, aKey->sak, 32UL)
                                MACSEC_DrvCopyKeyToRec(&desc.recAes256Xpn.hKey0, hashKey, 16UL)
                                desc.recAes256Xpn.seq0 = aNextPN & 0xFFFFFFFFFULL
                                desc.recAes256Xpn.seq1 = (aNextPN >> 32UL) & 0xFFFFFFFFULL
                                desc.recAes256Xpn.is0 = MACSEC_BYTES_TO_LE32(aKey->salt, 0UL) ^ (aSSCI)
                                desc.recAes256Xpn.is1 = MACSEC_BYTES_TO_LE32(aKey->salt, 4UL)
                                desc.recAes256Xpn.is2 = MACSEC_BYTES_TO_LE32(aKey->salt, 8UL)
                                desc.recAes256Xpn.iv0 = (sci64 & 0xFFFFFFFFULL)
                                desc.recAes256Xpn.iv1 = (sci64 >> 32UL) & 0xFFFFFFFFUL
                                desc.recAes256Xpn.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK
                                saCtxt->xRecSAUpdCtrlOff =
                                    (uint8_t) offsetof(MACSEC_EgressXRecAES256XpnType, saUpdCtrl)
                                saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES256XpnType, seq0)
                                saCtxt->xRecSeq1Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES256XpnType, seq1)
                                break
                            default:
                                BCM_ASSERT(0U)
                                break

                        info.SAIdx = saIdx
                        info.SCIdx = vPort
                        info.AN = aAN
                        info.dir = MACSEC_DIR_EGRESS
                        *aSA = MACSEC_DrvCreateHandle(&info)
                        saCtxt->saIdx = saIdx
                        saCtxt->contextID = MACSEC_XREC_ID_CREATE(info.secyIdx, info.AN)
                        saCtxt->ssci = aSSCI
                        secy->confProtect = aConfidentiality
                        BCM_MemCpy(saCtxt->keyID, aKey->keyId, sizeof(saCtxt->keyID))

                        desc.words[0UL] = saCntrlW
                        desc.words[1UL] = saCtxt->contextID
                        MACSEC_DrvXRecUpdate(aConnHdl, hwID, saIdx, 0UL, MACSEC_XREC_WORDS_PER_ENTRY,
                                             desc.words, MACSEC_DIR_EGRESS)
                        flowCntrl0 = MACSEC_Read(MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vPort].ctrl1_0))
                        if aConfidentiality == (uint32_t)BCM_BOOL_TRUE
                            flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_CONF_PROTECT_MASK
                        else
                            flowCntrl0 &= ~MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_CONF_PROTECT_MASK
                        MACSEC_Write(MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vPort].ctrl1_0), flowCntrl0)

                        flowCntrl1  = (secy->confOffset << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_10_CONFIDENTIALITY_OFFSET_SHIFT) & MACSEC_E_FCTRL_SAM_FLOW_CTRL2_10_CONFIDENTIALITY_OFFSET_MASK
                        flowCntrl1 |= (secy->cfg.preSecTagAuthLen << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_LEN_SHIFT) & MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_LEN_MASK
                        flowCntrl1 |= (secy->cfg.preSecTagAuthStart << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_START_SHIFT) & MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_START_MASK
                        flowCntrl1 |= (secy->cfg.secTagOffset << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_SECTAG_OFFSET_SHIFT) & MACSEC_E_FCTRL_SAM_FLOW_CTRL2_10_SECTAG_OFFSET_MASK
                        MACSEC_Write(MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vPort].ctrl2_0), flowCntrl1)

                        saCtxt->createdTime = MACSEC_Read(MACSEC_E_BRCM_GLOBAL_OFFSET_OF(tick))
                        saCtxt->startTime = saCtxt->createdTime
                        saCtxt->stopTime  = saCtxt->createdTime
                        saCtxt->trafficStartTime = 0UL
                        saCtxt->trafficStopTime  = 0UL
                    else
                        ret = BCM_ERR_NOMEM
                else
                    ret = BCM_ERR_BUSY
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SA_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvCreateTxSA(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, MACSEC_ANType aAN,
                             uint64_t aNextPN, SECY_SCIType *aSCI, uint32_t aSSCI,
                             uint32_t aConfidentiality, const SECY_KeyType *const aKey,
                             MACSEC_SAHdlType  *const aSA)
{
    MACSEC_SecYContextType *secy;
    MACSEC_SAContextType *saCtxt;
    MACSEC_HandleInfoType info;
    MACSEC_EgressXRecDescType desc;
    uint8_t hwID;
    uint8_t vPort;
    uint32_t saIdx;
    uint32_t saCntrlW = 0UL;
    uint8_t zeroText[16UL];
    uint64_t sci64;
    uint32_t flowCntrl0;
    uint32_t flowCntrl1;
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint8_t aHKey[16UL] = {0U};

    if ((NULL != aSCI) && (aAN <= MACSEC_AN_MAX) && (NULL != aKey) && (NULL != aSA)) {
        MACSEC_HANDLE_INFO_INIT(info);
        ret = MACSEC_SECYGetAESAuthKey(aConnHdl, aSecYHdl, &aKey->sak[0], &aHKey[0]);
        if(ret != BCM_ERR_OK) {
            ret = BCM_ERR_INVAL_MAGIC;
        } else {
            secy = MACSEC_DrvHandleToSecY(aSecYHdl);
            if (NULL != secy) {
                ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
                BCM_ASSERT(BCM_ERR_OK == ret);
                hwID  = (uint8_t)(info.hwIdx & 0xFFUL);
                vPort = (uint8_t)(secy->txVportIdx & 0xFFUL);
                if (BCM_MemCmp(aSCI, &secy->txSC.sci, sizeof(SECY_SCIType)) == 0) {
                    /* Is this SA in use for encoding? */
                    if ((uint32_t)BCM_BOOL_TRUE != secy->txSC.sa[aAN].inUse) {
                        /* Allocate SA record (XFORM record) for this SA */
                        saIdx = MACSEC_ClrNGetBit(&MACSEC_HwCtxt[info.ctxIdx].egress.saAllocMask);
                        if (saIdx < MACSEC_MAX_XFORM_RECORDS) {
                            saCntrlW |= (MACSEC_XRECW0_TOP_EGRESS << MACSEC_XRECW0_TOP_SHIFT);
                            saCntrlW |= MACSEC_XRECW0_IV0_PRESENT_MASK;
                            saCntrlW |= MACSEC_XRECW0_IV1_PRESENT_MASK;
                            saCntrlW |= MACSEC_XRECW0_UPDATE_SEQ_MASK;
                            saCntrlW |= MACSEC_XRECW0_IV_FORMAT_MASK; /* use seq no as part of IV */
                            saCntrlW |= MACSEC_XRECW0_ENCRYPT_AUTH_MASK;
                            saCntrlW |= MACSEC_XRECW0_LOAD_KEY_MASK;
                            saCntrlW |= (0x2UL << MACSEC_XRECW0_DIGEST_TYPE_SHIFT);
                            saCntrlW |= (MACSEC_XRECW0_AUTH_ALGO_AES_GHASH << MACSEC_XRECW0_AUTH_ALGO_SHIFT);
                            saCntrlW |= ((uint32_t)aAN << MACSEC_XRECW0_AN_SHIFT);
                            saCntrlW |= MACSEC_XRECW0_CNTXID_PRESENT_MASK;

                            if (BCM_BOOL_TRUE == MACSEC_DrvIsXpnCipherSuiteSet(secy)) {
                                saCntrlW |= MACSEC_XRECW0_SEQ_64BIT << MACSEC_XRECW0_SEQ_TYPE_SHIFT;
                            } else {
                                saCntrlW |= MACSEC_XRECW0_SEQ_32BIT << MACSEC_XRECW0_SEQ_TYPE_SHIFT;
                            }

                            BCM_MemSet(&desc, 0x0U, sizeof(desc));
                            BCM_MemSet(zeroText, 0x0U, sizeof(zeroText));
                            sci64 = MACSEC_DrvSCIToU64(aSCI);
                            saCtxt = &secy->txSC.sa[aAN];

                            switch (secy->cipherID) {
                                case SECY_CIPHER_GCM_AES_128:
                                    saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES128 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT);
                                    MACSEC_DrvCopyKeyToRec(&desc.recAes128.key0, aKey->sak, 16UL);
                                    MACSEC_DrvCopyKeyToRec(&desc.recAes128.hKey0, aHKey, 16UL);
                                    desc.recAes128.seq = (uint32_t)(aNextPN & 0xFFFFFFFFUL);
                                    desc.recAes128.iv0 = (uint32_t)(sci64 & 0xFFFFFFFFUL);
                                    desc.recAes128.iv1 = (uint32_t)(sci64 >> 32UL) & 0xFFFFFFFFUL;
                                    desc.recAes128.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK;
                                    saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_EgressXRecAES128Type,
                                                               saUpdCtrl);
                                    saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES128Type, seq);
                                    break;
                                case SECY_CIPHER_GCM_AES_256:
                                    saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES256 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT);
                                    MACSEC_DrvCopyKeyToRec(&desc.recAes256.key0, aKey->sak, 32UL);
                                    MACSEC_DrvCopyKeyToRec(&desc.recAes256.hKey0, aHKey, 16UL);
                                    desc.recAes256.seq = (uint32_t)(aNextPN & 0xFFFFFFFFUL);
                                    desc.recAes256.iv0 = (uint32_t)(sci64 & 0xFFFFFFFFUL);
                                    desc.recAes256.iv1 = (uint32_t)(sci64 >> 32UL) & 0xFFFFFFFFUL;
                                    desc.recAes256.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK;
                                    saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_EgressXRecAES256Type,
                                                                        saUpdCtrl);
                                    saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES256Type, seq);
                                    break;
                                case SECY_CIPHER_GCM_AES_XPN_128:
                                    saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES128 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT);
                                    MACSEC_DrvCopyKeyToRec(&desc.recAes128Xpn.key0, aKey->sak, 16UL);
                                    MACSEC_DrvCopyKeyToRec(&desc.recAes128Xpn.hKey0, aHKey, 16UL);
                                    desc.recAes128Xpn.seq0 = (uint32_t)(aNextPN & 0xFFFFFFFFUL);
                                    desc.recAes128Xpn.seq1 = (uint32_t)((aNextPN >> 32UL) & 0xFFFFFFFFUL);
                                    desc.recAes128Xpn.is0 = MACSEC_BYTES_TO_LE32((aKey->salt), 0UL) ^ (aSSCI);
                                    desc.recAes128Xpn.is1 = MACSEC_BYTES_TO_LE32((aKey->salt), 4UL);
                                    desc.recAes128Xpn.is2 = MACSEC_BYTES_TO_LE32((aKey->salt), 8UL);
                                    desc.recAes128Xpn.iv0 = (uint32_t)(sci64 & 0xFFFFFFFFUL);
                                    desc.recAes128Xpn.iv1 = (uint32_t)(sci64 >> 32UL) & 0xFFFFFFFFUL;
                                    desc.recAes128Xpn.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK;
                                    saCtxt->xRecSAUpdCtrlOff = (uint8_t) offsetof(MACSEC_EgressXRecAES128XpnType,
                                                                        saUpdCtrl);
                                    saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES128XpnType, seq0);
                                    saCtxt->xRecSeq1Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES128XpnType, seq1);
                                    break;
                                case SECY_CIPHER_GCM_AES_XPN_256:
                                    saCntrlW |= (MACSEC_XRECW0_CRYPTO_ALGO_AES256 << MACSEC_XRECW0_CRYPTO_ALGO_SHIFT);
                                    MACSEC_DrvCopyKeyToRec(&desc.recAes256Xpn.key0, aKey->sak, 32UL);
                                    MACSEC_DrvCopyKeyToRec(&desc.recAes256Xpn.hKey0, aHKey, 16UL);
                                    desc.recAes256Xpn.seq0 = (uint32_t)(aNextPN & 0xFFFFFFFFUL);
                                    desc.recAes256Xpn.seq1 = (uint32_t)((aNextPN >> 32UL) & 0xFFFFFFFFUL);
                                    desc.recAes256Xpn.is0 = MACSEC_BYTES_TO_LE32(aKey->salt, 0UL) ^ (aSSCI);
                                    desc.recAes256Xpn.is1 = MACSEC_BYTES_TO_LE32(aKey->salt, 4UL);
                                    desc.recAes256Xpn.is2 = MACSEC_BYTES_TO_LE32(aKey->salt, 8UL);
                                    desc.recAes256Xpn.iv0 = (uint32_t)(sci64 & 0xFFFFFFFFUL);
                                    desc.recAes256Xpn.iv1 = (uint32_t)(sci64 >> 32UL) & 0xFFFFFFFFUL;
                                    desc.recAes256Xpn.saUpdCtrl = MACSEC_XRECSAUPDCTRL_SA_EXPIRED_IRQ_MASK;
                                    saCtxt->xRecSAUpdCtrlOff =
                                        (uint8_t) offsetof(MACSEC_EgressXRecAES256XpnType, saUpdCtrl);
                                    saCtxt->xRecSeq0Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES256XpnType, seq0);
                                    saCtxt->xRecSeq1Offset = (uint8_t) offsetof(MACSEC_EgressXRecAES256XpnType, seq1);
                                    break;
                                default:
                                    BCM_ASSERT(0UL == (uint32_t)&aConnHdl);
                                    break;
                            }

                            /* create handle and use the handle as unique context ID in SA record */
                            info.SAIdx = (uint8_t)saIdx;
                            info.SCIdx = (uint8_t)vPort; /* for egress SCI index is same as vPort# */
                            info.AN    = (uint8_t)aAN;
                            info.dir   = (uint8_t)MACSEC_DIR_EGRESS;
                            *aSA = MACSEC_DrvCreateHandle(&info);
                            saCtxt->handle    = *aSA;
                            saCtxt->saIdx     = (uint16_t)saIdx;
                            saCtxt->contextID = MACSEC_XREC_ID_CREATE(info.secyIdx, info.AN);
                            saCtxt->ssci      = aSSCI;
                            secy->confProtect = aConfidentiality;
                            BCM_MemCpy(saCtxt->keyID, aKey->keyId, sizeof(saCtxt->keyID));

                            /* write the xform record */
                            desc.words[0UL] = saCntrlW; /* first word in record is control word */
                            desc.words[1UL] = saCtxt->contextID;    /* second word context ID */
                            MACSEC_DrvXRecUpdate(aConnHdl, (uint32_t)hwID, (uint32_t)saIdx, 0UL, MACSEC_XREC_WORDS_PER_ENTRY,
                                                 desc.words, MACSEC_DIR_EGRESS);


                            /* configure the vport egress flow */

                            /* @event         misra_c_2012_rule_violation, zero_deref
                               @issue         Explicit null dereferenced (FORWARD_NULL)
                               @risk          Undefined behaviour
                               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                            /* coverity[misra_c_2012_rule_violation] */
                            /* coverity[zero_deref] */
                            flowCntrl0 = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE,
                                MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vPort].ctrl1_0));
                            if ((uint32_t)BCM_BOOL_TRUE == aConfidentiality) {
                                flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_CONF_PROTECT_MASK;
                            } else {
                                flowCntrl0 &= ~MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_CONF_PROTECT_MASK;
                            }
                            /* @event         misra_c_2012_rule_violation, zero_deref
                               @issue         Explicit null dereferenced (FORWARD_NULL)
                               @risk          Undefined behaviour
                               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                            /* coverity[misra_c_2012_rule_violation] */
                            /* coverity[zero_deref] */
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE,
                                         MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vPort].ctrl1_0), flowCntrl0);

                            flowCntrl1  = ((uint32_t)secy->confOffset << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_10_CONFIDENTIALITY_OFFSET_SHIFT) &
                                          MACSEC_E_FCTRL_SAM_FLOW_CTRL2_10_CONFIDENTIALITY_OFFSET_MASK;
                            flowCntrl1 |= ((uint32_t)secy->cfg.preSecTagAuthLen << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_LEN_SHIFT) &
                                          MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_LEN_MASK;
                            flowCntrl1 |= ((uint32_t)secy->cfg.preSecTagAuthStart << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_START_SHIFT) &
                                          MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_PRE_SECTAG_AUTH_START_MASK;
                            flowCntrl1 |= ((uint32_t)secy->cfg.secTagOffset << MACSEC_E_FCTRL_SAM_FLOW_CTRL2_0_SECTAG_OFFSET_SHIFT) &
                                          MACSEC_E_FCTRL_SAM_FLOW_CTRL2_10_SECTAG_OFFSET_MASK;
                            /* @event         misra_c_2012_rule_violation, zero_deref
                               @issue         Explicit null dereferenced (FORWARD_NULL)
                               @risk          Undefined behaviour
                               @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                            /* coverity[misra_c_2012_rule_violation] */
                            /* coverity[zero_deref] */
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE,
                                MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vPort].ctrl2_0), flowCntrl1);
                            /* read the global timer and capture the timestamp
                             * for createdTime
                             */
                            saCtxt->createdTime = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_BRCM_GLOBAL_OFFSET_OF(tick));
                            saCtxt->startTime = saCtxt->createdTime;
                            saCtxt->stopTime  = saCtxt->createdTime;
                            saCtxt->trafficStartTime = 0UL;
                            saCtxt->trafficStopTime  = 0UL;
                        } else {
                            ret = BCM_ERR_NOMEM;
                        }
                    } else {
                        ret = BCM_ERR_BUSY;
                    }
                }
            }
        }
    }
    return ret;
}
/**
    @brief

    @code{.unparsed}
    SECY_SCIType reservedSCI
    MACSEC_SecYContextType *secy
    uint32_t i
    uint32_t rxSCIdx
    uint32_t rxCAMIdx
    MACSEC_HandleInfoType info
    uint32_t validSCI = TRUE
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if NULL != aSCI
        MACSEC_HANDLE_INFO_INIT(info)
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            BCM_MemSet(&reservedSCI, 0xFFU, sizeof(reservedSCI))
            if BCM_MemCmp(&reservedSCI, aSCI, sizeof(SECY_SCIType) != 0UL
                BCM_MemSet(&reservedSCI, 0x0U, sizeof(SECY_SCIType))
                if BCM_MemCmp(&reservedSCI, aSCI->macAddr, 6UL) != 0UL
                    validSCI = TRUE
            if TRUE == validSCI
                for i = 0UL i < MACSEC_MAX_RXSC_PER_SECY; i++
                    if TRUE == secy->rxSC[i].active
                        if BCM_MemCmp(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType) == 0U
                            break
                if i == MACSEC_MAX_RXSC_PER_SECY
                    ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info)
                    BCM_ASSERT(BCM_ERR_OK == ret)
                    ret = MACSEC_DrvAllocRxCAMEntry(aConnHdl, info.hwIdx, aSCI, secy->rxVPortIdx, &rxCAMIdx)
                    if BCM_ERR_OK == ret
                        rxSCIdx = MACSEC_ClrNGetBit(&secy->rxSCAllocMask)
                        if rxSCIdx < MACSEC_MAX_RXSC_PER_SECY
                            BCM_MemCpy(&secy->rxSC[rxSCIdx].sci, aSCI, sizeof(SECY_SCIType))
                            secy->rxSC[rxSCIdx].active = TRUE
                            secy->rxSC[rxSCIdx].rxCAMIdx = rxCAMIdx
                            ret = BCM_ERR_OK
                        else
                            ret = BCM_ERR_NOMEM
                    else
                        ret = BCM_ERR_NOMEM
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_RX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvCreateRxSC(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_SCIType *aSCI)
{
    SECY_SCIType reservedSCI;
    MACSEC_SecYContextType *secy;
    int      i;
    uint32_t rxSCIdx;
    uint32_t rxCAMIdx = 0;
    MACSEC_HandleInfoType info;
    uint32_t validSCI = (uint32_t)BCM_BOOL_FALSE;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSCI) {
        MACSEC_HANDLE_INFO_INIT(info);
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            BCM_MemSet(&reservedSCI, 0xFFU, sizeof(reservedSCI));
            if (BCM_MemCmp(&reservedSCI, aSCI, sizeof(SECY_SCIType)) != 0) {
                BCM_MemSet(&reservedSCI, 0x0U, sizeof(SECY_SCIType));
                if (BCM_MemCmp((&reservedSCI), aSCI->macAddr, 6L) != 0) {
                    validSCI = (uint32_t)BCM_BOOL_TRUE;
                }
            }
            if ((uint32_t)BCM_BOOL_TRUE == validSCI) {
                /* Make sure same SCI is not already created for same SecY */
                for (i = 0; i < MACSEC_MAX_RXSC_PER_SECY; i++) {
                    if ((uint32_t)BCM_BOOL_TRUE == secy->rxSC[i].active) {
                        if (BCM_MemCmp(&secy->rxSC[i].sci, aSCI, sizeof(SECY_SCIType)) == 0) {
                            break;
                        }
                    }
                }
                if (i == MACSEC_MAX_RXSC_PER_SECY) {
                    /* find a free RXCAM entry for this */
                    ret = MACSEC_DrvGetHandleInfo(aSecYHdl, &info);
                    BCM_ASSERT(BCM_ERR_OK == ret);
                    ret = MACSEC_DrvAllocRxCAMEntry(aConnHdl, info.hwIdx, aSCI, secy->rxVPortIdx, &rxCAMIdx,
                                                    (BCM_BoolType)((secy->cfg.includeSCI == (uint32_t)BCM_BOOL_TRUE) ? BCM_BOOL_FALSE : BCM_BOOL_TRUE));
                    if (BCM_ERR_OK == ret) {
                        rxSCIdx = MACSEC_ClrNGetBit(&secy->rxSCAllocMask);
                        if (rxSCIdx < (uint32_t)MACSEC_MAX_RXSC_PER_SECY) {
                            BCM_MemCpy(&secy->rxSC[rxSCIdx].sci, aSCI, sizeof(SECY_SCIType));
                            secy->rxSC[rxSCIdx].active = (uint32_t)BCM_BOOL_TRUE;
                            secy->rxSC[rxSCIdx].rxCAMIdx = rxCAMIdx;
                            ret = BCM_ERR_OK;
                        } else {
                            ret = BCM_ERR_NOMEM;
                        }
                    } else {
                        ret = BCM_ERR_NOMEM;
                    }
                }
            }
        }
    }
    return ret;
}

/**
   @code{.unparsed}
    SECY_SCIType reservedSCI
    MACSEC_SecYContextType *secy
    uint32_t i
    int32_t ret = BCM_ERR_INVAL_PARAMS

    if NULL != aSCI
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            BCM_MemSet(&reservedSCI, 0xFFU, sizeof(reservedSCI))
            if BCM_MemCmp(&reservedSCI, aSCI, sizeof(SECY_SCIType) != 0UL)
                for i = 0UL; i < MACSEC_MAX_TXSC_PER_SECY; i++
                    if (uint32_t)BCM_BOOL_FALSE == secy->txSC[i].inUse)
                        BCM_MemCpy(&secy->txSC[i].sci, aSCI, sizeof(SECY_SCIType))
                        secy->txSC[i].inUse = (uint32_t)BCM_BOOL_TRUE
                        ret = BCM_ERR_OK
                        break
                if i == MACSEC_MAX_TXSC_PER_SECY)
                    ret = BCM_ERR_NOMEM
    return ret;
   @endcode
    @trace #BRCM_SWARCH_MACSEC_DRV_CREATE_TX_SC_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvCreateTxSC(BCM_HandleType aConnHdl, MACSEC_SecYHdlType aSecYHdl, SECY_SCIType *aSCI)
{
    SECY_SCIType reservedSCI;
    MACSEC_SecYContextType *secy;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSCI) {
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            BCM_MemSet(&reservedSCI, 0xFFU, sizeof(reservedSCI));
            if (BCM_MemCmp(&reservedSCI, aSCI, sizeof(SECY_SCIType)) != 0) {
                BCM_MemSet(&reservedSCI, 0x0U, sizeof(SECY_SCIType));
                if (BCM_MemCmp(&reservedSCI, aSCI->macAddr, 6L) != 0) {
                    if ((uint32_t)BCM_BOOL_FALSE == secy->txSC.active) {
                        BCM_MemCpy(&secy->txSC.sci, aSCI, sizeof(SECY_SCIType));
                        secy->txSC.active = (uint32_t)BCM_BOOL_TRUE;
                        ret = BCM_ERR_OK;
                    } else {
                        ret = BCM_ERR_BUSY;
                    }
                }
            }
        }
    }
    return ret;
}

/**
   @brief Sets cipher suite

   @code{.unparsed}
    MACSEC_SecYContextType *secy
    int ret = BCM_ERR_INVAL_PARAMS
    if (SECY_CONF_OFFSET_0 == aOffset) || (SECY_CONF_OFFSET_30 == aOffset) ||
        (SECY_CONF_OFFSET_50 == aOffset)
        secy = MACSEC_DrvHandleToSecY(aSecYHdl)
        if NULL != secy
            if TRUE == secy->cfg.xpnEnable
                if (SECY_CIPHER_GCM_AES_XPN_128 == aCipherID) ||
                    (SECY_CIPHER_GCM_AES_XPN_256 == aCipherID)
                    secy->cipherID = aCipherID
                    if SECY_CONF_OFFSET_30 == aOffset
                        secy->confOffset = 30U
                    else if SECY_CONF_OFFSET_50 == aOffset
                        secy->confOffset = 50U
                    else
                        secy->confOffset = 0U
                    ret = BCM_ERR_OK
            else
                if (SECY_CIPHER_GCM_AES_128 == aCipherID) ||
                    (SECY_CIPHER_GCM_AES_256 == aCipherID)
                    secy->cipherID = aCipherID
                    if SECY_CONF_OFFSET_30 == aOffset
                        secy->confOffset = 30U;
                    else if SECY_CONF_OFFSET_50 == aOffset
                        secy->confOffset = 50U
                    else
                        secy->confOffset = 0U
                    ret = BCM_ERR_OK
    return ret
   @endcode
    @trace #BRCM_SWARCH_MACSEC_DRV_SETCIPHERSUITE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvSetCipherSuite(MACSEC_SecYHdlType aSecYHdl, SECY_CipherType aCipherID,
                                        SECY_ConfOffsetType aOffset) {
    MACSEC_SecYContextType *secy;
    int ret = BCM_ERR_INVAL_PARAMS;

    if ((SECY_CONF_OFFSET_0 == aOffset) || (SECY_CONF_OFFSET_30 == aOffset) ||
        (SECY_CONF_OFFSET_50 == aOffset)) {
        secy = MACSEC_DrvHandleToSecY(aSecYHdl);
        if (NULL != secy) {
            if ((uint32_t)BCM_BOOL_TRUE == secy->cfg.xpnEnable) {
                if ((SECY_CIPHER_GCM_AES_XPN_128 == aCipherID) ||
                    (SECY_CIPHER_GCM_AES_XPN_256 == aCipherID) ||
                    (SECY_CIPHER_GCM_AES_128 == aCipherID) ||
                    (SECY_CIPHER_GCM_AES_256 == aCipherID)) {
                    secy->cipherID = aCipherID;
                    if (SECY_CONF_OFFSET_30 == aOffset) {
                        secy->confOffset = 30U;
                    } else if (SECY_CONF_OFFSET_50 == aOffset) {
                        secy->confOffset = 50U;
                    } else {
                        secy->confOffset = 0U;
                    }
                    ret = BCM_ERR_OK;
                }
            } else {
                if ((SECY_CIPHER_GCM_AES_128 == aCipherID) ||
                    (SECY_CIPHER_GCM_AES_256 == aCipherID)) {
                    secy->cipherID = aCipherID;
                    if (SECY_CONF_OFFSET_30 == aOffset) {
                        secy->confOffset = 30U;
                    } else if (SECY_CONF_OFFSET_50 == aOffset) {
                        secy->confOffset = 50U;
                    } else {
                        secy->confOffset = 0U;
                    }
                    ret = BCM_ERR_OK;
                }
            }
        }
    }
    return ret;
}

/**
    @brief Create SecY instance

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    uint32_t secyIdx
    MACSEC_HwContextType *hwContext
    MACSEC_SecYContextType *secy
    MACSEC_HandleInfoType info
    uint32_t vportIdx
    uint32_t secyAddr
    uint16_t crc

    if aID < MACSEC_MAX_PORT_NUM
        MACSEC_HANDLE_INFO_INIT(info)
        ctxIdx = MACSEC_ConvPort2CtxIdx(aID)
        hwContext = &MACSEC_HwCtxt[ctxIdx]
        if (uint32_t)BCM_BOOL_TRUE == hwContext->initialized
            secyIdx = MACSEC_ClrNGetBit(&hwContext->secyAllocMask)
            if secyIdx < MACSEC_MAX_SECY_PER_PORT
                secy = &hwContext->secy[secyIdx]
                vportIdx = MACSEC_ClrNGetBit(&hwContext->egress.vportAllocMask)
                if vportIdx < MACSEC_MAX_VPORTS
                    secy->txVportIdx = vportIdx
                    flowCntrl0 = 0
                    flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_PROTECT_FRAME_MASK
                    flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_CONF_PROTECT_MASK
                    flowCntrl0 |= (MACSEC_DROP_INTERNAL << MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_DROP_ACTION_SHIFT)
                    flowCntrl0 |= (MACSEC_SAACTION_EGRESS << MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_FLOW_TYPE_SHIFT)
                    if aCfg->includeSCI == (uint32_t)BCM_BOOL_TRUE
                        flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_INCLUDE_SCI_MASK
                    MACSEC_Write(MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vportIdx].ctrl1_0), flowCntrl0)

                    vportIdx = MACSEC_ClrNGetBit(&hwContext->ingress.vportAllocMask)
                    if vportIdx < MACSEC_MAX_VPORTS
                        secy->rxVPortIdx = vportIdx
                        flowCntrl0 = 0
                        flowCntrl0 |= (MACSEC_DROP_FRAME_ERROR << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_DROP_ACTION_SHIFT)
                        flowCntrl0 |= (MACSEC_SAACTION_INGRESS << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_FLOW_TYPE_SHIFT)
                        flowCntrl0 |= (SECY_VALIDATE_FRAME_STRICT << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_SHIFT)
                        flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_UNTAGGED_MASK
                        flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_REPLAY_PROTECT_MASK
                        MACSEC_Write(MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[vportIdx].ctrl1_0), flowCntrl0)
                    else
                        MACSEC_SetBit(&hwContext->egress.vportAllocMask, secy->txVportIdx)
                        ret = BCM_ERR_NOMEM
                else
                    ret = BCM_ERR_NOMEM
                if BCM_ERR_OK == ret
                    if aCfg->includeSCI == (uint32_t)BCM_BOOL_FALSE
                        secy->rxSCAllocMask = 1
                    else
                        secy->rxSCAllocMask = (1UL << MACSEC_MAX_RXSC_PER_SECY) - 1UL
                    secy->replayProtect = (uint32_t)BCM_BOOL_TRUE
                    secy->replayWindow = 0UL
                    secy->protectFrame = (uint32_t)BCM_BOOL_TRUE
                    secy->validateFrame = SECY_VALIDATE_FRAME_STRICT
                    BCM_MemCpy(&secy->cfg, aCfg, sizeof(SECY_CfgType))
                    MACSEC_HANDLE_INFO_INIT(info)
                    info.hwIdx = aID
                    info.secyIdx = secyIdx
                    *aSecYHdl = MACSEC_DrvCreateHandle(&info)
                    secyAddr = (uint32_t)secy
                    crc = BCM_CRC16((uint8_t *)&secyAddr, sizeof(secyAddr), SECY_HANDLE_CRC16_POLY,
                                    0x0U)
                    crc = BCM_CRC16((uint8_t *)aSecYHdl, sizeof(BCM_HandleType),
                                    SECY_HANDLE_CRC16_POLY, crc)
                    MACSEC_DrvHandleUpdateHash(aSecYHdl, crc)
                    secy->hash = crc
                else
                    MACSEC_SetBit(&hwContext->secyAllocMask, secyIdx)
            else
                ret = BCM_ERR_NOMEM
        else
            ret = BCM_ERR_UNINIT
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_CREATESECY_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvCreateSecY(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, const SECY_CfgType *const aCfg,
                             MACSEC_SecYHdlType *const aSecYHdl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t secyIdx;
    MACSEC_HwContextType *hwContext;
    MACSEC_SecYContextType *secy;
    MACSEC_HandleInfoType info;
    uint32_t vportIdx;
    uint16_t crc;
    uint32_t flowCntrl0 = 0UL;

    if (aID < MACSEC_MAX_PORT_NUM) {
        MACSEC_HANDLE_INFO_INIT(info);
        uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);
        hwContext = &MACSEC_HwCtxt[ctxIdx];
        if ((uint32_t)BCM_BOOL_TRUE == hwContext->initialized) {
            /* Allocate new SecY entity */
            secyIdx = MACSEC_ClrNGetBit(&hwContext->secyAllocMask);
            if (secyIdx < (uint32_t)MACSEC_MAX_SECY_PER_PORT) {
                secy = &hwContext->secy[secyIdx];
                /* Allocate egress vports and ingress vports
                 * (flows) for this SecY
                 */
                vportIdx = MACSEC_ClrNGetBit(&hwContext->egress.vportAllocMask);
                if (vportIdx < MACSEC_MAX_VPORTS) {
                    secy->txVportIdx = vportIdx;
                    /* configure egress flow control registers */
                    flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_PROTECT_FRAME_MASK;
                    flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_CONF_PROTECT_MASK;
                    if ((uint32_t)BCM_BOOL_TRUE == aCfg->includeSCI) {
                        flowCntrl0 |= MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_INCLUDE_SCI_MASK;
                    }
                    /* for egress since the packet drop decision happens before its fed to transform, its
                       safe to set the drop action to internal even when core does not have packet buffer */
                    flowCntrl0 |= (MACSEC_DROP_INTERNAL << MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_DROP_ACTION_SHIFT);
                    /* flow action as egress */
                    flowCntrl0 |= (MACSEC_SAACTION_EGRESS << MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_FLOW_TYPE_SHIFT);

                    /* @event         misra_c_2012_rule_violation, zero_deref
                        @issue         Explicit null dereferenced (FORWARD_NULL)
                        @risk          Undefined behaviour
                        @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                    /* coverity[misra_c_2012_rule_violation] */
                    /* coverity[zero_deref] */
                    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE,
                        MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vportIdx].ctrl1_0), flowCntrl0);

                    vportIdx = MACSEC_ClrNGetBit(&hwContext->ingress.vportAllocMask);
                    if (vportIdx < MACSEC_MAX_VPORTS) {
                        secy->rxVPortIdx = vportIdx;
                        flowCntrl0 = 0UL;
                        /* for ingress, in the absence of the packet buffer, packets are decrypted/validated
                           with inline rate of the media, so engine can not drop the packet internally.
                           In case of error, configure engine to signal the packet frame error to next
                           MAC so that receiver (switch) can drop this frame */
                        flowCntrl0 |= (MACSEC_DROP_FRAME_ERROR << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_DROP_ACTION_SHIFT);
                        flowCntrl0 |= (MACSEC_SAACTION_INGRESS << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_FLOW_TYPE_SHIFT);

                        /* initialize the RX flow with validateFrames = Strict, validateUntagged = Strict
                           replayProtect = True */
                        flowCntrl0 |= (SECY_VALIDATE_FRAME_STRICT << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_TAGGED_SHIFT);
                        flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_VALIDATE_FRAMES_UNTAGGED_MASK;
                        flowCntrl0 |= MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_REPLAY_PROTECT_MASK;

                        /* @event         misra_c_2012_rule_violation, zero_deref
                           @issue         Explicit null dereferenced (FORWARD_NULL)
                           @risk          Undefined behaviour
                           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                        /* coverity[misra_c_2012_rule_violation] */
                        /* coverity[zero_deref] */
                        MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE,
                            MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[vportIdx].ctrl1_0), flowCntrl0);
                    } else {
                        MACSEC_SetBit(&hwContext->egress.vportAllocMask, secy->txVportIdx);
                        ret = BCM_ERR_NOMEM;
                    }
                } else {
                    ret = BCM_ERR_NOMEM;
                }
                if (BCM_ERR_OK == ret) {
                    /* allow only one peer RX SC if SecY is not encoding SCI explicity on transmit channel */
                    if ((uint32_t)BCM_BOOL_FALSE == aCfg->includeSCI) {
                        secy->rxSCAllocMask = 1UL;
                    } else {
                        secy->rxSCAllocMask = (1UL << MACSEC_MAX_RXSC_PER_SECY) - 1UL;
                    }
                    secy->replayProtect = (uint32_t)BCM_BOOL_TRUE;
                    secy->replayWindow = 0UL;
                    secy->protectFrame = (uint32_t)BCM_BOOL_TRUE;
                    secy->validateFrame = SECY_VALIDATE_FRAME_STRICT;
                    BCM_MemCpy(&secy->cfg, aCfg, sizeof(SECY_CfgType));
                    MACSEC_HANDLE_INFO_INIT(info);
                    info.hwIdx = aID;
                    info.secyIdx = (uint8_t)secyIdx;
                    *aSecYHdl = MACSEC_DrvCreateHandle(&info);
                    /* hash the address of the context structure and information bits of the
                     * handle which includes random seed to make it unique
                     */
                    crc = BCM_CRC16((uint8_t *)&secy, sizeof(secy), SECY_HANDLE_CRC16_POLY,
                                    0x0U);
                    crc = BCM_CRC16((uint8_t *)aSecYHdl, sizeof(BCM_HandleType),
                                    SECY_HANDLE_CRC16_POLY, crc);
                    MACSEC_DrvHandleUpdateHash(aSecYHdl, crc);
                    secy->handle = *aSecYHdl;
                } else {
                    MACSEC_SetBit(&hwContext->secyAllocMask, secyIdx);
                }
            } else {
                ret = BCM_ERR_NOMEM;
            }
        } else {
            ret = BCM_ERR_UNINIT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief

    @code{.unparsed}

    int32_t ret = BCM_ERR_OK
    MACSEC_HandleInfoType info
    MACSEC_HANDLE_INFO_INIT(info)
    uint16_t *hashAddrEgress = NULL
    uint16_t *hashAddrIngress = NULL
    uint16_t ruleHash = 0U

    ret = MACSEC_DrvGetHandleInfo(aRuleHdl, &info)
    if ret is ERR_OK
        if info.dir == DIR_EGRESS
            hashAddrEgress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, info.ruleType,
                                                       info.ruleIdxEgress, info.ruleMask,
                                                         SECY_RULE_DIR_EGRESS)
        else if info.dir == INGRESS
            hashAddrIngress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, info.ruleType,
                                                          info.ruleIdxIngress, info.ruleMask,
                                                          SECY_RULE_DIR_INGRESS)
        else if info.dir == BOTH
            hashAddrEgress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, info.ruleType,
                                                         info.ruleIdxEgress, info.ruleMask,
                                                         SECY_RULE_DIR_EGRESS)
            hashAddrIngress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, info.ruleType,
                                                          info.ruleIdxIngress, info.ruleMask,
                                                          SECY_RULE_DIR_INGRESS)
         else
            ret = BCM_ERR_INVAL_PARAMS

        if ret is ERR_OK
            ruleHash = MACSEC_DrvGenerateRuleHash(aRuleHdl, hashAddrEgress, hashAddrIngress)
            if ruleHash == info.hash
                if SECY_RULE_DIR_EGRESS & info.dir != 0U
                    if MACSEC_RULEMASK_HDRPARSE == info.ruleType
                        ret = MACSEC_DrvRemoveCPRuleInternal(aConnHdl, info.hwIdx,
                                                             info.ruleIdxEgress, info.ruleMask,
                                                             SECY_RULE_DIR_EGRESS)
                    else if MACSEC_RULEMASK_TCAM == info.ruleType
                        ret = MACSEC_DrvRemoveTCAMRule(aConnHdl, info.hwIdx, info.ruleIdxEgress,
                                                       SECY_RULE_DIR_EGRESS)
                    else
                        ret = BCM_ERR_INVAL_PARAMS;
                    if BCM_ERR_OK == ret
                        *hashAddrEgress = 0U

                if SECY_RULE_DIR_INGRESS & info.dir != 0U
                    if MACSEC_RULEMASK_HDRPARSE == info.ruleType
                        ret = MACSEC_DrvRemoveCPRuleInternal(aConnHdl, info.hwIdx,
                                                             info.ruleIdxIngress, info.ruleMask,
                                                             SECY_RULE_DIR_INGRESS)
                    else if MACSEC_RULEMASK_TCAM == info.ruleType
                        ret = MACSEC_DrvRemoveTCAMRule(aConnHdl, info.hwIdx, info.ruleIdxIngress,
                                                       SECY_RULE_DIR_INGRESS)
                    else
                        ret = BCM_ERR_INVAL_PARAMS
                    if BCM_ERR_OK == ret
                        *hashAddrIngress = 0U
            else
                ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVECPRULE_PROC
    @trace #BRCM_SWARCH_MACSEC_DRV_REMOVE_SECYLOOKUP_RULE_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvRemoveCPRule(BCM_HandleType aConnHdl, MACSEC_RuleHdlType aRuleHdl)
{
    int32_t ret = BCM_ERR_OK;;
    MACSEC_HandleInfoType info;
    MACSEC_HANDLE_INFO_INIT(info);
    uint16_t *hashAddrEgress = NULL;
    uint16_t *hashAddrIngress = NULL;
    uint16_t ruleHash = 0U;

    ret = MACSEC_DrvGetHandleInfo(aRuleHdl, &info);
    if (BCM_ERR_OK == ret) {
        if (SECY_RULE_DIR_EGRESS == info.dir) {
            hashAddrEgress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, info.ruleType,
                                                       info.ruleIdxEgress, info.ruleMask,
                                                         SECY_RULE_DIR_EGRESS);
        } else if (SECY_RULE_DIR_INGRESS == info.dir) {
            hashAddrIngress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, info.ruleType,
                                                          info.ruleIdxIngress, info.ruleMask,
                                                          SECY_RULE_DIR_INGRESS);
        } else if (SECY_RULE_DIR_BOTH == info.dir) {
            hashAddrEgress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, info.ruleType,
                                                         info.ruleIdxEgress, info.ruleMask,
                                                         SECY_RULE_DIR_EGRESS);
            hashAddrIngress = MACSEC_DrvGetRuleHashAddr(info.hwIdx, info.ruleType,
                                                          info.ruleIdxIngress, info.ruleMask,
                                                          SECY_RULE_DIR_INGRESS);
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }

        if (BCM_ERR_OK == ret) {
            ruleHash = MACSEC_DrvGenerateRuleHash(aRuleHdl, hashAddrEgress, hashAddrIngress);
            if (ruleHash == info.hash) {
                if ((SECY_RULE_DIR_EGRESS & info.dir) != 0U) {
                    if (MACSEC_RULEMASK_HDRPARSE == info.ruleType) {
                        ret = MACSEC_DrvRemoveCPRuleInternal(aConnHdl, info.hwIdx,
                                                             info.ruleIdxEgress, info.ruleMask,
                                                             SECY_RULE_DIR_EGRESS);
                    } else if (MACSEC_RULEMASK_TCAM == info.ruleType) {
                        ret = MACSEC_DrvRemoveTCAMRule(aConnHdl, info.hwIdx, info.ruleIdxEgress,
                                                       SECY_RULE_DIR_EGRESS);
                    } else {
                        ret = BCM_ERR_INVAL_PARAMS;
                    }
                    if ((BCM_ERR_OK == ret) && (NULL != hashAddrEgress)) {
                        *hashAddrEgress = 0U;
                    }
                }

                if ((SECY_RULE_DIR_INGRESS & info.dir) != 0U) {
                    if (MACSEC_RULEMASK_HDRPARSE == info.ruleType) {
                        ret = MACSEC_DrvRemoveCPRuleInternal(aConnHdl, info.hwIdx,
                                                             info.ruleIdxIngress, info.ruleMask,
                                                             SECY_RULE_DIR_INGRESS);
                    } else if (MACSEC_RULEMASK_TCAM == info.ruleType) {
                        ret = MACSEC_DrvRemoveTCAMRule(aConnHdl, info.hwIdx, info.ruleIdxIngress,
                                                       SECY_RULE_DIR_INGRESS);
                    } else {
                        ret = BCM_ERR_INVAL_PARAMS;
                    }
                    if ((BCM_ERR_OK == ret) && (NULL != hashAddrIngress)) {
                        *hashAddrIngress = 0U;
                    }
                }
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
            }
        }
    }
    return ret;
}

/**
    @brief

    @code{.unparsed}

    int32_t ret = BCM_ERR_OK
    MACSEC_HandleInfoType info
    MACSEC_HwContextType *hwContext
    MACSEC_ContextType *context
    MACSEC_TCAMRuleType rule
    uint32_t tcamRuleIdxEgress
    uint32_t tcamRuleIdxIngress
    uint32_t vPortIdx
    uint16_t crc
    uint32_t tcamRuleAddr
    MACSEC_EgressFlowCfgType eflow = {0}
    MACSEC_IngressFlowCfgType iflow = {0}

    if (aHwID < MACSEC_MAX_PORT_NUM) and (NULL != aRule) and (NULL != aRuleHandle)
        uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aHwID)
        hwContext = &MACSEC_HwCtxt[ctxIdx]
        if TRUE == hwContext->initialized
            BCM_MemSet(&rule, 0x0U, sizeof(MACSEC_TCAMRuleType))
            rule.controlPkt = TRUE
            rule.macDA = aRule->macDA
            rule.macSA = aRule->macSA
            rule.frameData = aRule->frameData
            rule.matchMacDAMask = aRule->matchMacDAMask
            rule.matchMacSAMask = aRule->matchMacSAMask
            rule.matchFrameDataMask = aRule->matchFrameDataMask
            rule.pktType = MACSEC_ETHPKT_OTHER
            rule.frameType = MACSEC_ETHFRAME_OTHER
            if TRUE == aRule->vlanTagged
                rule.numTags = MACSEC_ETHNUMTAGS_2
                rule.matchNumTags = TRUE
            if TRUE == aRule->macSecTagged
                rule.macSecTagged = TRUE
                rule.matchTagged = TRUE
            rule.matchEthFrameType = TRUE;
            if SECY_RULE_DIR_EGRESS == aRule->dir
                context = &hwContext->egress
                tcamRuleIdxEgress = MACSEC_ClrNGetBit(&context->tcamRuleAllocMask)
                if tcamRuleIdxEgress < MACSEC_MAX_TCAM_ENTRIES
                    vPortIdx = MACSEC_ClrNGetBit(&context->vportAllocMask)
                    if vPortIdx < MACSEC_MAX_VPORTS
                        MACSEC_DrvAddTCAMRule(aConnHdl, aHwID, &rule, tcamRuleIdxEgress, vPortIdx,
                                              MACSEC_DIR_EGRESS)
                        flowCntrl0 = MACSEC_SAACTION_BYPASS << MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_FLOW_TYPE_SHIFT
                        flowCntrl0 |= (MACSEC_DROP_INTERNAL << MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_DROP_ACTION_SHIFT)
                        MACSEC_Write(MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vPortIdx].ctrl1_0), flowCntrl0)
                        ret = ERR_OK
                    else
                        MACSEC_SetBit(&context->tcamRuleAllocMask, tcamRuleIdxEgress)
                        ret = BCM_ERR_NOMEM
                else
                    ret = BCM_ERR_NOMEM
            else if SECY_RULE_DIR_INGRESS == aRule->dir
                context = &hwContext->ingress
                tcamRuleIdxIngress = MACSEC_ClrNGetBit(&context->tcamRuleAllocMask)
                if tcamRuleIdxIngress < MACSEC_MAX_TCAM_ENTRIES
                    vPortIdx = MACSEC_ClrNGetBit(&context->vportAllocMask)
                    if vPortIdxIngress < MACSEC_MAX_VPORTS)
                        MACSEC_DrvAddTCAMRule(aConnHdl, aHwID, &rule, tcamRuleIdxIngress, vPortIdx,
                                              MACSEC_DIR_INGRESS)
                        flowCntrl0 = (MACSEC_SAACTION_BYPASS << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_FLOW_TYPE_SHIFT)
                        flowCntrl0 |= (MACSEC_DROP_FRAME_ERROR << MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_DROP_ACTION_SHIFT)
                        MACSEC_Write(MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[vPortIdx].ctrl1_0), flowCntrl0)
                        ret = ERR_OK
                    else
                        MACSEC_SetBit(&context->tcamRuleAllocMask, tcamRuleIdxIngress)
                        ret = BCM_ERR_NOMEM
                else
                    ret = BCM_ERR_NOMEM
            else
                ret = BCM_ERR_INVAL_PARAMS
        else
            ret = BCM_ERR_UNINIT
    else
        ret = BCM_ERR_INVAL_PARAMS

    if BCM_ERR_OK == ret
        MACSEC_HANDLE_INFO_INIT(info)
        info.hwIdx = aHwID
        info.ruleIdxEgress = ruleIdxEgress
        info.ruleIdxIngress = ruleIdxIngress
        info.ruleType = MACSEC_RULEMASK_TCAM
        info.dir = aRule->dir
        *aRuleHandle = MACSEC_DrvCreateHandle(&info)
        crc = MACSEC_DrvGenerateRuleHash(*aRuleHandle, ruleAddrEgress, ruleAddrIngress)
        if NULL != ruleAddrEgress
            *ruleAddrEgress = crc
        if NULL != ruleAddrIngress
            *ruleAddrIngress = crc
        MACSEC_DrvHandleUpdateHash(aRuleHandle, crc)
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT1_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvAddCPRuleOpt1(BCM_HandleType aConnHdl, MACSEC_HwIDType aHwID,
                                          const SECY_RuleOpt1Type *const aRuleOpt,
                                               MACSEC_RuleHdlType *const aRuleHdl)
{
    int32_t ret = BCM_ERR_OK;
    MACSEC_HandleInfoType info;
    MACSEC_HwContextType *hwContext;
    MACSEC_ContextType *context;
    MACSEC_TCAMRuleType rule;
    uint32_t ruleIdxEgress = 0UL;
    uint32_t ruleIdxIngress = 0UL;
    uint16_t *ruleAddrEgress = NULL;
    uint16_t *ruleAddrIngress = NULL;
    uint32_t vPortIdx;
    uint16_t crc;
    uint32_t flowCntrl0;

    if ((aHwID < MACSEC_MAX_PORT_NUM) && (NULL != aRuleOpt) && (NULL != aRuleHdl)) {
        uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aHwID);
        hwContext = &MACSEC_HwCtxt[ctxIdx];
        if ((uint32_t)BCM_BOOL_TRUE == hwContext->initialized) {
            BCM_MemSet(&rule, 0x0U, sizeof(MACSEC_TCAMRuleType));
            rule.controlPkt = (uint32_t)BCM_BOOL_TRUE;
            rule.macDA = aRuleOpt->macDA;
            rule.macSA = aRuleOpt->macSA;
            rule.frameData = aRuleOpt->frameData;
            rule.matchMacDAMask = aRuleOpt->matchMacDAMask;
            rule.matchMacSAMask = aRuleOpt->matchMacSAMask;
            rule.matchFrameDataMask = aRuleOpt->matchFrameDataMask;
            rule.frameType = MACSEC_ETHFRAME_OTHER;
            if ((uint32_t)BCM_BOOL_TRUE == aRuleOpt->vlanTagged) {
                rule.numTags = MACSEC_ETHNUMTAGS_2;
                rule.matchNumTags  = (uint32_t)BCM_BOOL_TRUE;
            }
            if ((uint32_t)BCM_BOOL_TRUE == aRuleOpt->macSecTagged) {
                rule.macSecTagged  = (uint32_t)BCM_BOOL_TRUE;
                rule.matchTagged   = (uint32_t)BCM_BOOL_TRUE;
            }
            rule.matchEthFrameType = (uint32_t)BCM_BOOL_TRUE;
            if ((SECY_RULE_DIR_EGRESS & aRuleOpt->dir) != 0UL) {
                context = &hwContext->egress;
                ruleIdxEgress = MACSEC_ClrNGetBit(&context->tcamRuleAllocMask);
                if (ruleIdxEgress < MACSEC_MAX_TCAM_ENTRIES) {
                    vPortIdx = MACSEC_ClrNGetBit(&context->vportAllocMask);
                    if (vPortIdx < MACSEC_MAX_VPORTS) {
                        MACSEC_DrvAddTCAMRule(aConnHdl, aHwID, &rule, ruleIdxEgress, vPortIdx,
                                              MACSEC_DIR_EGRESS);
                        /* Control packet rule is added in TCAM. Now setup the vport to bypass
                           this packet (write 0 to flow control 1_0 and 2_0) */
                        flowCntrl0 = MACSEC_SAACTION_BYPASS <<
                            MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_FLOW_TYPE_SHIFT;
                        flowCntrl0 |= (MACSEC_DROP_INTERNAL <<
                            MACSEC_E_FCTRL_SAM_FLOW_CTRL1_0_DROP_ACTION_SHIFT);
                        /* @event         misra_c_2012_rule_violation, zero_deref
                           @issue         Explicit null dereferenced (FORWARD_NULL)
                           @risk          Undefined behaviour
                           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                        /* coverity[misra_c_2012_rule_violation] */
                        /* coverity[zero_deref] */
                        MACSEC_Write(aConnHdl, aHwID, MACSEC_BLOCK_CORE,
                            MACSEC_E_FCTRL_OFFSET_OF(samflow_regs[vPortIdx].ctrl1_0), flowCntrl0);
                        ruleAddrEgress = &hwContext->egress.tcamRuleHash[ruleIdxEgress];
                        ret = BCM_ERR_OK;
                    } else {
                        /* free up the TCAM entry */
                        MACSEC_SetBit(&context->tcamRuleAllocMask, ruleIdxEgress);
                        ret = BCM_ERR_NOMEM;
                    }
                } else {
                    ret = BCM_ERR_NOMEM;
                }
            }

            if ((SECY_RULE_DIR_INGRESS & aRuleOpt->dir) != 0UL) {
                context = &hwContext->ingress;
                ruleIdxIngress = MACSEC_ClrNGetBit(&context->tcamRuleAllocMask);
                if (ruleIdxIngress < MACSEC_MAX_TCAM_ENTRIES) {
                    vPortIdx = MACSEC_ClrNGetBit(&context->vportAllocMask);
                    if (vPortIdx < MACSEC_MAX_VPORTS) {
                        MACSEC_DrvAddTCAMRule(aConnHdl, aHwID, &rule, ruleIdxIngress, vPortIdx,
                                              MACSEC_DIR_INGRESS);
                        /* Control packet rule is added in TCAM. Now setup the vport to bypass
                           this packet */
                        flowCntrl0 = (MACSEC_SAACTION_BYPASS <<
                            MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_FLOW_TYPE_SHIFT);
                        flowCntrl0 |= (MACSEC_DROP_FRAME_ERROR <<
                            MACSEC_I_FCTRL_SAM_FLOW_CTRL1_0_DROP_ACTION_SHIFT);
                        /* @event         misra_c_2012_rule_violation, zero_deref
                           @issue         Explicit null dereferenced (FORWARD_NULL)
                           @risk          Undefined behaviour
                           @justification Coverity issue thrown only by ARC compiler, not found in GCC. */
                        /* coverity[misra_c_2012_rule_violation] */
                        /* coverity[zero_deref] */
                        MACSEC_Write(aConnHdl, aHwID, MACSEC_BLOCK_CORE,
                            MACSEC_I_FCTRL_OFFSET_OF(samflow_regs[vPortIdx].ctrl1_0), flowCntrl0);
                        ruleAddrIngress = &hwContext->ingress.tcamRuleHash[ruleIdxIngress];
                        ret = BCM_ERR_OK;
                    } else {
                        /* free up the TCAM entry */
                        MACSEC_SetBit(&context->tcamRuleAllocMask, ruleIdxIngress);
                        ret = BCM_ERR_NOMEM;
                    }
                } else {
                    ret = BCM_ERR_NOMEM;
                }
            }
        } else {
            ret = BCM_ERR_UNINIT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK == ret) {
        MACSEC_HANDLE_INFO_INIT(info);
        info.hwIdx          = aHwID;
        info.ruleIdxEgress  = (uint8_t)ruleIdxEgress;
        info.ruleIdxIngress = (uint8_t)ruleIdxIngress;
        info.ruleType       = MACSEC_RULEMASK_TCAM;
        info.dir            = (uint8_t)aRuleOpt->dir;
        *aRuleHdl = MACSEC_DrvCreateHandle(&info);
        crc = MACSEC_DrvGenerateRuleHash(*aRuleHdl, ruleAddrEgress, ruleAddrIngress);
        if (NULL != ruleAddrEgress) {
            *ruleAddrEgress = crc;
        }
        if (NULL != ruleAddrIngress) {
            *ruleAddrIngress = crc;
        }
        MACSEC_DrvHandleUpdateHash(aRuleHdl, crc);
    }
    return ret;
}

/**
    @brief Add control packet rule option 0

    @code{.unparsed}
    int32_t ret = BCM_ERR_OK
    uint32_t ruleIdxEgress = 0UL
    uint32_t ruleIdxIngress = 0UL
    MACSEC_HandleInfoType info
    MACSEC_HwContextType *hwContext
    uint16_t *hashAddr = NULL
    uint16_t crc

    if (aHwID < MACSEC_MAX_PORT_NUM) && (NULL != aRule) &&
        (NULL != aRuleHandle)
        ctxIdx = MACSEC_ConvPort2CtxIdx(aHwID)
        hwContext = &MACSEC_HwCtxt[ctxIdx]
        if TRUE == hwContext->initialized
            if SECY_RULE_DIR_EGRESS & aRule->dir != 0
                ret = MACSEC_DrvAddCPRuleInternal(aConnHdl, aHwID, aRule, MACSEC_DIR_EGRESS, &ruleIdxEgress, &hashAddrEgress)
            if SECY_RULE_DIR_INGRESS & aRule->dir != 0
                ret = MACSEC_DrvAddCPRuleInternal(aConnHdl, aHwID, aRule, MACSEC_DIR_INGRESS, &ruleIdIngress, &hashAddrIngress)
        else
            ret = BCM_ERR_UNINIT
    else
        ret = BCM_ERR_INVAL_PARAMS
    if BCM_ERR_OK == ret
        MACSEC_HANDLE_INFO_INIT(info)
        info.hwIdx = aHwID
        info.ruleIdxEgress = ruleIdxEgress
        info.ruleIdxIngress = ruleIdxIngress
        info.ruleMask = aRule->ruleMask
        info.ruleType = MACSEC_RULEMASK_HDRPARSE
        info.dir = aRule->dir
        *aRuleHandle = MACSEC_DrvCreateHandle(&info)
        crc = MACSEC_DrvGenerateRuleHash(*aRuleHandle, hashAddrEgress, hashAddrIngress)
        if NULL != hashAddrEgress
            *hashAddrEgress = crc
        if NULL != hashAddrIngress
            *hashAddrIngress = crc
        MACSEC_DrvHandleUpdateHash(aRuleHandle, crc)
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_ADDCPRULEOPT0_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvAddCPRuleOpt0(BCM_HandleType aConnHdl, MACSEC_HwIDType aHwID,
                                          const SECY_RuleOpt0Type *const aRuleOpt,
                                               MACSEC_RuleHdlType *const aRuleHdl)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t ruleIdxEgress = 0UL;
    uint32_t ruleIdxIngress = 0UL;
    MACSEC_HandleInfoType info;
    MACSEC_HwContextType *hwContext;
    uint16_t *hashAddrEgress = NULL;
    uint16_t *hashAddrIngress = NULL;
    uint16_t crc = 0U;

    if ((aHwID < MACSEC_MAX_PORT_NUM) && (NULL != aRuleOpt) &&
        (NULL != aRuleHdl)) {
        uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aHwID);
        hwContext = &MACSEC_HwCtxt[ctxIdx];
        if ((uint32_t)BCM_BOOL_TRUE == hwContext->initialized) {
            if ((SECY_RULE_DIR_EGRESS & aRuleOpt->dir) != 0UL) {
                ret = MACSEC_DrvAddCPRuleInternal(aConnHdl, aHwID, aRuleOpt, MACSEC_DIR_EGRESS,
                        &ruleIdxEgress, &hashAddrEgress);
            }
            if ((SECY_RULE_DIR_INGRESS & aRuleOpt->dir) != 0UL) {
                ret = MACSEC_DrvAddCPRuleInternal(aConnHdl, aHwID, aRuleOpt, MACSEC_DIR_INGRESS,
                    &ruleIdxIngress, &hashAddrIngress);
            }
        } else {
            ret = BCM_ERR_UNINIT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    if (BCM_ERR_OK == ret) {
        MACSEC_HANDLE_INFO_INIT(info);
        info.hwIdx = aHwID;
        info.ruleIdxEgress  = (uint8_t)(ruleIdxEgress & 0xFFUL);
        info.ruleIdxIngress = (uint8_t)(ruleIdxIngress & 0xFFUL);
        info.ruleMask = (uint8_t)aRuleOpt->ruleMask;
        info.ruleType = (uint8_t)MACSEC_RULEMASK_HDRPARSE;
        info.dir      = (uint8_t)aRuleOpt->dir;
        *aRuleHdl = MACSEC_DrvCreateHandle(&info);
        crc = MACSEC_DrvGenerateRuleHash(*aRuleHdl, hashAddrEgress, hashAddrIngress);
        if (NULL != hashAddrEgress) {
            *hashAddrEgress = crc;
        }
        if (NULL != hashAddrIngress) {
            *hashAddrIngress = crc;
        }
        MACSEC_DrvHandleUpdateHash(aRuleHdl, crc);
    }
    return ret;
}

/**
   @brief Get cipher capabilities supported by hardware.

   @code{.unparsed}
    int32_t ret = BCM_ERR_OK

    if NULL != aCap
        switch (aCipherID)
        case SECY_CIPHER_GCM_AES_128:
        case SECY_CIPHER_GCM_AES_256:
        case SECY_CIPHER_GCM_AES_XPN_128:
        case SECY_CIPHER_GCM_AES_XPN_256:
            *aCap = SECY_CIPHER_CAP_CONF_OFFSET
            break
        default:
            ret = BCM_ERR_INVAL_PARAMS
            break
    else
        ret = BCM_ERR_INVAL_PARAMS

    return ret
   @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_GETCIPHERCAP_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvGetCipherCap(MACSEC_HwIDType aID, SECY_CipherType aCipherID,
                                      SECY_CipherCapType *const aCap)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL != aCap) {
        switch (aCipherID) {
        case SECY_CIPHER_GCM_AES_128:
        case SECY_CIPHER_GCM_AES_256:
        case SECY_CIPHER_GCM_AES_XPN_128:
        case SECY_CIPHER_GCM_AES_XPN_256:
            *aCap = SECY_CIPHER_CAP_CONF_OFFSET;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief Initialize MACSec engine and driver

    @code{.unparsed}
   int32_t ret = BCM_ERR_OK
    if (aID < MACSEC_MAX_PORT_NUM) && (NULL != aConfig)
        ctxIdx = MACSEC_ConvPort2CtxIdx(aID)
        if (uint32_t)BCM_BOOL_FALSE == MACSEC_HwCtxt[ctxIdx].initialized
            MACSEC_SYSMISC_REGS->mspu_ctrl |= 0x1UL << aID
            MACSEC_SYSMISC_REGS->mspu_ctrl |= 0x1UL << ( MACSEC_SYSMISC_MSPU_CTRL_0_OUTBOUND_SEL_SHIFT + aID)

            ret = MACSEC_BusDrvEnable(aConnHdl, aID, aConfig)
            if BCM_ERR_OK == ret
                MACSEC_DrvGlobalInit(aConnHdl, aID, aConfig, MACSEC_DIR_EGRESS)
                MACSEC_DrvGlobalInit(aConnHdl, aID, aConfig, MACSEC_DIR_INGRESS)
                MACSEC_DrvNonMatchedFlow(aConnHdl, aID, MACSEC_DIR_EGRESS)
                MACSEC_DrvNonMatchedFlow(aConnHdl, aID, MACSEC_DIR_INGRESS)
                MACSEC_HwCtxt[ctxIdx].initialized = (uint32_t)BCM_BOOL_TRUE
            else
                MACSEC_SYSMISC_REGS->mspu_ctrl &= ~(0x1UL << aID)
                MACSEC_SYSMISC_REGS->mspu_ctrl &= ~(0x1UL << (MACSEC_SYSMISC_MSPU_CTRL_0_OUTBOUND_SEL_SHIFT + aID))
        els
            ret = BCM_ERR_INVAL_STATE
    else
        ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
int32_t MACSEC_DrvInit(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, const SECY_HwCfgType *const aConfig)
{
   int32_t ret = BCM_ERR_OK;
    MACSEC_HwCtxt[0].initialized = (uint32_t)BCM_BOOL_FALSE;
    if ((aID < MACSEC_MAX_PORT_NUM) && (NULL != aConfig)) {
        uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

        if ((uint32_t)BCM_BOOL_FALSE == MACSEC_HwCtxt[ctxIdx].initialized) {
            /* Enable inbound and outbound for port before accessing any MACSec registers */
            ret = MACSEC_BusDrvEnable(aConnHdl, aID, aConfig);
            if (BCM_ERR_OK == ret) {
                MACSEC_DrvGlobalInit(aConnHdl, aID, aConfig, MACSEC_DIR_EGRESS);
                MACSEC_DrvGlobalInit(aConnHdl, aID, aConfig, MACSEC_DIR_INGRESS);

                /* configure policy for the non matched flow (frames for which no entry is found in TCAM) */
                MACSEC_DrvNonMatchedFlow(aConnHdl, aID, MACSEC_DIR_EGRESS);
                MACSEC_DrvNonMatchedFlow(aConnHdl, aID, MACSEC_DIR_INGRESS);

                MACSEC_BusDrvGlobalInit(aConnHdl, aID);
                MACSEC_EnableSysInterrupt(aID);
                MACSEC_HwCtxt[ctxIdx].initialized = (uint32_t)BCM_BOOL_TRUE;
            } else {
                /* disable the MACSec port */
                MACSEC_BusDrvDisable(aConnHdl, aID);
            }
        } else {
            ret = BCM_ERR_INVAL_STATE;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

#ifndef MACSEC_IRQ_SUPPORT_DISABLE
/**
    @brief SA PN threshold interrupt

    This function handles SA threshold hit
    interrupt. It raise the callback to
    integration layer.

    @code{.unparsed}
    MACSEC_SAContextType *sa
    uint32_t summaryReg
    uint32_t saIdx
    uint32_t i
    int32_t ret
    uint32_t contextID = 0UL
    uint32_t line = __LINE__

    summaryReg = MACSEC_E_OPPE_REGS[aID]->sa_pn_thr_summary1
    MACSEC_E_OPPE_REGS[aID]->sa_pn_thr_summary1 = summaryReg
    for i = 0UL; (i < MACSEC_MAX_XFORM_RECORDS) && (summaryReg != 0UL); i++
        ret = BCM_ERR_OK
        saIdx = 32UL - CPU_Clz(summaryReg)
        MACSEC_DrvXRecRead(aConnHdl, aID, saIdx, 1UL, 1UL, &contextID,
                           MACSEC_DIR_EGRESS)
        if 0UL != contextID
            secyIdx = (contextID & MACSEC_XREC_ID_SECY_IDX_MASK) >> MACSEC_XREC_ID_SECY_IDX_SHIFT
            an = (contextID & MACSEC_XREC_ID_AN_MASK) >> MACSEC_XREC_ID_AN_SHIFT
            uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);
            hwContext = &MACSEC_HwCtxt[ctxIdx];
            sa = &hwContext->secy[secyIdx].txSC.sa[an];
            if TRUE == sa->inUse
                MACSEC_SAThrdExpInd(aID, sa->handle)
            else
                ret = BCM_ERR_INVAL_STATE
                line = __LINE__
            else
                ret = BCM_ERR_UNKNOWN
                line = __LINE__
        else
            ret = BCM_ERR_UNKNOWN
            line = __LINE__
        if BCM_ERR_OK != ret
            Report error
        summaryReg &= ~(0x1UL << saIdx)
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvSAThrldIRQHandler(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint32_t aStatus)
{
    MACSEC_SAContextType *sa;
    uint32_t summaryReg;
    uint32_t saIdx;
    uint32_t i;
    int32_t ret;
    uint32_t secyIdx;
    uint32_t an;
    uint32_t contextID = 0UL;
    MACSEC_HwContextType *hwContext = NULL;
    uint32_t line = __LINE__;

    summaryReg = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_OPPE_OFFSET_OF(sa_pn_thr_summary1));
    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_OPPE_OFFSET_OF(sa_pn_thr_summary1), summaryReg);
    for (i = 0UL; (i < MACSEC_MAX_XFORM_RECORDS) && (summaryReg != 0UL); i++) {
        ret = BCM_ERR_OK;
        saIdx = 31UL - CPU_Clz(summaryReg);
        MACSEC_DrvXRecRead(aConnHdl, (uint32_t)aID, saIdx, 1UL, 1UL, &contextID,
                           MACSEC_DIR_EGRESS);
        if (MACSEC_XREC_ID_MAGIC == ((contextID & MACSEC_XREC_ID_MAGIC_MASK) >> MACSEC_XREC_ID_MAGIC_SHIFT)) {
            secyIdx = (contextID & MACSEC_XREC_ID_SECY_IDX_MASK) >> MACSEC_XREC_ID_SECY_IDX_SHIFT;
            an = (contextID & MACSEC_XREC_ID_AN_MASK) >> MACSEC_XREC_ID_AN_SHIFT;
            if ((secyIdx < MACSEC_MAX_SECY_PER_PORT) && (an <= MACSEC_AN_MAX)) {
                uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);
                hwContext = &MACSEC_HwCtxt[ctxIdx];
                sa = &hwContext->secy[secyIdx].txSC.sa[an];
                if ((uint32_t)BCM_BOOL_TRUE == sa->inUse) {
                    sa->thrldExpIntrCnt++;
                    MACSEC_SAThrdExpInd(aID, sa->handle);
                } else {
                    ret = BCM_ERR_INVAL_STATE;
                    line = __LINE__;
                }
            } else {
                ret = BCM_ERR_UNKNOWN;
                line = __LINE__;
            }
        } else {
            ret = BCM_ERR_UNKNOWN;
            line = __LINE__;
        }
        if (BCM_ERR_OK != ret) {
            BCM_LOG_DEBUG(0U, BCM_STATE_RUN, ret, "Failed to handle SA PN threshold. \
                             SA index = 0x%x, Line number = %d.\n", saIdx, line);
        }
        summaryReg &= ~(0x1UL << saIdx);
    }
}

/**
    @brief SA expiry interrupt handler

    Interrupt handler called when SA expires (on PN
    rollover).

    @code{.unparsed}
    MACSEC_SAContextType *sa
    MACSEC_HandleInfoType info
    uint32_t summaryReg
    uint32_t saIdx
    uint32_t i
    int32_t ret
    uint32_t contextID = 0UL
    uint32_t line = __LINE__

    summaryReg = MACSEC_E_CLS_REGS[aID]->sa_exp_summary1
    MACSEC_E_CLS_REGS[aID]->sa_exp_summary1 = summaryReg
    for i = 0UL; (i < MACSEC_MAX_XFORM_RECORDS) && (summaryReg != 0UL); i++
        ret = BCM_ERR_OK
        saIdx = 32UL - CPU_Clz(summaryReg)
        MACSEC_DrvXRecRead(aConnHdl, aID, saIdx, 1UL, 1UL, &contextID,
                           MACSEC_DIR_EGRESS)
        if contextID magic is valid
            secyIdx = (contextID & MACSEC_XREC_ID_SECY_IDX_MASK) >> MACSEC_XREC_ID_SECY_IDX_SHIFT;
            an = (contextID & MACSEC_XREC_ID_AN_MASK) >> MACSEC_XREC_ID_AN_SHIFT;
            if secyIdx < MACSEC_MAX_SECY_PER_PORT) && an <= MACSEC_AN_MAX
                hwContext = &MACSEC_HwCtxt[ctxIdx];
                sa = &hwContext->secy[secyIdx].txSC.sa[an];
                MACSEC_HANDLE_INFO_INIT(info)
                ret = MACSEC_DrvGetHandleInfo(sa->handle, &info)
                if BCM_ERR_OK == ret
                    if (TRUE == MACSEC_HwCtxt[ctxIdx].secy[info.secyIdx].txSC.transmitting) {
                        MACSEC_HwCtxt[ctxIdx].secy[info.secyIdx].txSC.transmitting = (uint32_t)BCM_BOOL_FALSE
                        sa->inUse = (uint32_t)BCM_BOOL_FALSE
                        MACSEC_SAExpiredInd(aID, sa->handle)
            else
                ret = BCM_ERR_UNKNOWN
                line = __LINE__
        else
            ret = BCM_ERR_UNKNOWN
            line = __LINE__
        if BCM_ERR_OK != ret)
            Report error
        summaryReg &= ~(0x1UL << saIdx)
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
static void MACSEC_DrvSAExpiredIRQHandler(BCM_HandleType aConnHdl, MACSEC_HwIDType aID, uint32_t aStatus)
{
    MACSEC_SAContextType *sa;
    MACSEC_HandleInfoType info;
    uint32_t summaryReg;
    uint32_t saIdx;
    uint32_t i;
    int32_t ret;
    uint32_t secyIdx;
    uint32_t an;
    uint32_t contextID = 0UL;
    uint32_t line = __LINE__;
    MACSEC_HwContextType *hwContext = NULL;
    uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(aID);

    summaryReg = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sa_exp_summary1));
    MACSEC_Write(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_CLS_CTRL_OFFSET_OF(sa_exp_summary1), summaryReg);
    for (i = 0UL; (i < MACSEC_MAX_XFORM_RECORDS) && (summaryReg != 0UL); i++) {
        ret = BCM_ERR_OK;
        saIdx = 31UL - CPU_Clz(summaryReg);
        MACSEC_DrvXRecRead(aConnHdl, (uint32_t)aID, saIdx, 1UL, 1UL, &contextID,
                           MACSEC_DIR_EGRESS);
        if (MACSEC_XREC_ID_MAGIC == ((contextID & MACSEC_XREC_ID_MAGIC_MASK) >> MACSEC_XREC_ID_MAGIC_SHIFT)) {
            secyIdx = (contextID & MACSEC_XREC_ID_SECY_IDX_MASK) >> MACSEC_XREC_ID_SECY_IDX_SHIFT;
            an = (contextID & MACSEC_XREC_ID_AN_MASK) >> MACSEC_XREC_ID_AN_SHIFT;
            if ((secyIdx < MACSEC_MAX_SECY_PER_PORT) && (an <= MACSEC_AN_MAX)) {
                hwContext = &MACSEC_HwCtxt[ctxIdx];
                sa = &hwContext->secy[secyIdx].txSC.sa[an];
                MACSEC_HANDLE_INFO_INIT(info);
                ret = MACSEC_DrvGetHandleInfo(sa->handle, &info);
                if (BCM_ERR_OK == ret) {
                    if ((uint32_t)BCM_BOOL_TRUE == MACSEC_HwCtxt[ctxIdx].secy[info.secyIdx].txSC.transmitting) {
                        MACSEC_HwCtxt[ctxIdx].secy[info.secyIdx].txSC.transmitting = (uint32_t)BCM_BOOL_FALSE;
                        sa->inUse = (uint32_t)BCM_BOOL_FALSE;
                        sa->stopTime = MACSEC_Read(aConnHdl, aID, MACSEC_BLOCK_CORE, MACSEC_E_BRCM_GLOBAL_OFFSET_OF(tick));
                        sa->expiredIntrCnt++;
                        MACSEC_SAExpiredInd(aID, sa->handle);
                    }
                }
            } else {
                ret = BCM_ERR_UNKNOWN;
                line = __LINE__;
            }
        } else {
            ret = BCM_ERR_UNKNOWN;
            line = __LINE__;
        }
        if (BCM_ERR_OK != ret) {
            BCM_LOG_DEBUG(0U, BCM_STATE_RUN, ret, "Failed to handle SA expiry interrupt. \
                             SA index = 0x%x, Line number = %d.\n", saIdx, line);
        }
        summaryReg &= ~(0x1UL << saIdx);
    }
}

/**
    @brief

    @code{.unparsed}
    MACSEC_HwContextType *hwContext
    uint32_t intrStatusEgress
    uint32_t intrStatusIngress
    MACSEC_HwIDType hwID
    uint32_t status
    uint32_t i
    int32_t ret = BCM_ERR_OK
    uint32_t line = __LINE__
    uint32_t hwErrMask = MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_MASK |
                         MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_MASK |
                         MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_MASK |
                         MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_MASK

    status = MACSEC_SYSMISC_REGS->interrupt_stat
    if 0UL != status
        for i = 0UL i < MACSEC_MAX_PORT_NUM; i++
            ret = BCM_ERR_OK
            if 0UL != (status & (0x1UL << i)
                hwID = i
                ctxIdx = MACSEC_ConvPort2CtxIdx(hwID)
                hwContext = &MACSEC_HwCtxt[ctxIdx]
                intrStatusEgress = MACSEC_E_EIP62_REGS[hwID]->int_ctrl_stat
                MACSEC_E_EIP62_REGS[hwID]->int_ctrl_stat = intrStatusEgress

                intrStatusIngress = MACSEC_I_EIP62_REGS[hwID]->int_ctrl_stat
                MACSEC_I_EIP62_REGS[hwID]->int_ctrl_stat = intrStatusIngress
                if TRUE == hwContext->initialized
                    if 0UL != (intrStatusEgress & hwErrMask)
                        if 0UL != (intrStatusEgress &
                                    MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_MASK)
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_PKT_INPUT_ERR, (uint32_t)BCM_BOOL_TRUE)
                        if 0UL != (intrStatusEgress &
                                    MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_MASK)
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_PKT_PROCESS_ERR, (uint32_t)BCM_BOOL_TRUE)
                        if 0UL != (intrStatusEgress &
                                    MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_MASK)
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_CONTEXT_ERR, (uint32_t)BCM_BOOL_TRUE)
                        if 0UL != (intrStatusEgress &
                                    MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_MASK)
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_FATAL_ERR, (uint32_t)BCM_BOOL_TRUE)
                    if 0UL != (intrStatusEgress &
                                MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_THRESHOLD_MASK)
                        MACSEC_DrvSAThrldIRQHandler(aConnHdl, hwID, intrStatusEgress)
                    if 0UL != (intrStatusEgress &
                                MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_ROLLOVER_MASK)
                        MACSEC_DrvSAExpiredIRQHandler(aConnHdl, hwID, intrStatusEgress)
                    if 0UL != (intrStatusIngress & hwErrMask)
                        if 0UL != (intrStatusIngress &
                                    MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_MASK)
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_PKT_INPUT_ERR, (uint32_t)BCM_BOOL_FALSE)
                        if 0UL != (intrStatusIngress &
                                    MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_MASK)
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_PKT_PROCESS_ERR, (uint32_t)BCM_BOOL_FALSE)
                        if 0UL != (intrStatusIngress &
                                    MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_MASK)
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_CONTEXT_ERR, (uint32_t)BCM_BOOL_FALSE)
                        if 0UL != (intrStatusIngress &
                                    MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_MASK)
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_FATAL_ERR, (uint32_t)BCM_BOOL_FALSE)
                    if 0UL != (intrStatusIngress &
                                (MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_THRESHOLD_MASK |
                                 MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_ROLLOVER_MASK))
                        ret = BCM_ERR_UNKNOWN
                        line = __LINE__
                else
                    ret = BCM_ERR_UNINIT
                    line = __LINE__
    if BCM_ERR_OK != ret
        Report error
    @endcode

    @trace #BRCM_SWARCH_MACSEC_DRV_INIT_PROC
    @trace #BRCM_SWREQ_MACSEC_DRV_SECY_OPERATIONS
*/
void MACSEC_DrvIRQHandler(BCM_HandleType aConnHdl)
{
    MCU_VersionType mcuVer = {0};
    MACSEC_HwContextType *hwContext;
    uint32_t intrStatusEgress;
    uint32_t intrStatusIngress;
    MACSEC_HwIDType hwID;
    uint32_t status;
    uint32_t i;
    int32_t ret = BCM_ERR_OK;
    uint32_t line = __LINE__;
    uint32_t hwErrMask = MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_MASK |
                         MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_MASK |
                         MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_MASK |
                         MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_MASK;

    (void)MCU_GetVersion(&mcuVer);

    if (0UL != status) {
        for (i = 0UL; i < MACSEC_MAX_PORT_NUM; i++) {
            ret = BCM_ERR_OK;
            if (0UL != (status & (0x1UL << i))) {
                hwID = i;
                uint32_t ctxIdx = MACSEC_ConvPort2CtxIdx(hwID);
                hwContext = &MACSEC_HwCtxt[ctxIdx];
                if ((uint32_t)BCM_BOOL_TRUE == hwContext->initialized) {
                /* read egress and ingress interrupt status register and process it */
                    intrStatusEgress = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(int_ctrl_stat));
                    MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(int_ctrl_stat), intrStatusEgress);

                    intrStatusIngress = MACSEC_Read(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(int_ctrl_stat));
                    MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(int_ctrl_stat), intrStatusIngress);
                    if (0UL != (intrStatusEgress & hwErrMask)) {
                        if (0UL != (intrStatusEgress &
                                    MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_MASK)) {
                            intrStatusEgress &= ~MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_EN_MASK;
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(int_ctrl_stat),
                                    intrStatusEgress);
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_PKT_INPUT_ERR, (uint32_t)BCM_BOOL_TRUE);
                        }
                        if (0UL != (intrStatusEgress &
                                    MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_MASK)) {
                            intrStatusEgress &= ~MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_EN_MASK;
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(int_ctrl_stat),
                                    intrStatusEgress);
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_PKT_PROCESS_ERR, (uint32_t)BCM_BOOL_TRUE);
                        }
                        if (0UL != (intrStatusEgress &
                                    MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_MASK)) {
                            intrStatusEgress &= ~MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_EN_MASK;
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(int_ctrl_stat),
                                    intrStatusEgress);
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_CONTEXT_ERR, (uint32_t)BCM_BOOL_TRUE);
                        }
                        if (0UL != (intrStatusEgress &
                                    MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_MASK)) {
                            intrStatusEgress &= ~MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_EN_MASK;
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_E_EIP62_OFFSET_OF(int_ctrl_stat),
                                    intrStatusEgress);
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_FATAL_ERR, (uint32_t)BCM_BOOL_TRUE);
                        }
                        hwContext->egress.hwErr = intrStatusEgress & hwErrMask;
                    }
                    if (0UL != (intrStatusEgress &
                                MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_THRESHOLD_MASK)) {
                        MACSEC_DrvSAThrldIRQHandler(aConnHdl, hwID, intrStatusEgress);
                    }
                    if (0UL != (intrStatusEgress &
                                MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_ROLLOVER_MASK)) {
                        MACSEC_DrvSAExpiredIRQHandler(aConnHdl, hwID, intrStatusEgress);
                    }
                    if (0UL != (intrStatusIngress & hwErrMask)) {
                        if (0UL != (intrStatusIngress &
                                    MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_MASK)) {
                            intrStatusIngress &= ~MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_INPUT_ERROR_EN_MASK;
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(int_ctrl_stat),
                                    intrStatusIngress);
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_PKT_INPUT_ERR, (uint32_t)BCM_BOOL_FALSE);
                        }
                        if (0UL != (intrStatusIngress &
                                    MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_MASK)) {
                            intrStatusIngress &= ~MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_PROCESSING_ERROR_EN_MASK;
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(int_ctrl_stat),
                                    intrStatusIngress);
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_PKT_PROCESS_ERR, (uint32_t)BCM_BOOL_FALSE);
                        }
                        if (0UL != (intrStatusIngress &
                                    MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_MASK)) {
                            intrStatusIngress &= ~MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_CONTEXT_ERROR_EN_MASK;
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(int_ctrl_stat),
                                    intrStatusIngress);
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_CONTEXT_ERR, (uint32_t)BCM_BOOL_FALSE);
                        }
                        if (0UL != (intrStatusIngress &
                                    MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_MASK)) {
                            intrStatusIngress &= ~MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_FATAL_ERROR_EN_MASK;
                            MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_CORE, MACSEC_I_EIP62_OFFSET_OF(int_ctrl_stat),
                                    intrStatusIngress);
                            MACSEC_HwErrInd(hwID, MACSEC_HWERR_EVENT_FATAL_ERR, (uint32_t)BCM_BOOL_FALSE);
                        }
                        hwContext->ingress.hwErr = intrStatusIngress & hwErrMask;
                    }
                    /* seq threshold and sa expired interrupt shall never hit for ingress */
                    if (0UL != (intrStatusIngress &
                                (MACSEC_I_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_THRESHOLD_MASK |
                                 MACSEC_E_EIP62_EIP62_INT_CTRL_STAT_SEQNUM_ROLLOVER_MASK))) {
                        ret = BCM_ERR_UNKNOWN;
                        line = __LINE__;
                    }
                    MACSEC_Write(aConnHdl, hwID, MACSEC_BLOCK_MSPU_TOP, MACSEC_MSPU_TOP_OFFSET_OF(interrupt_status2), 0x0UL);
                } else {
                    ret = BCM_ERR_UNINIT;
                    line = __LINE__;
                }
                status &= ~(1UL << i);
                if (0UL == status) {
                    break;
                }
            }
        }
    }
    if (BCM_ERR_OK != ret) {
        BCM_LOG_ERROR(0U, BCM_STATE_RUN, ret, "Failed to handle MACSEC inerrupt. \
                        Read address  = 0x%x, Line number = %d.\n", status, line);
    }
    MACSEC_ClearSysInterrupt(0);

}
#endif

/** @} */
