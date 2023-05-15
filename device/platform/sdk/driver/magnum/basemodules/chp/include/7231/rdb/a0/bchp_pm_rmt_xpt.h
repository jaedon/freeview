#ifndef BCHP_PM_RMT_XPT_H__
#define BCHP_PM_RMT_XPT_H__

#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr1.h"
#include "bchp_xpt_dpcr2.h"
#include "bchp_xpt_dpcr3.h"
#include "bchp_xpt_dpcr_pp.h"
#include "bchp_xpt_fe.h"
#include "bchp_xpt_full_pid_parser.h"
#include "bchp_xpt_gr.h"
#include "bchp_xpt_mpod.h"
#include "bchp_xpt_msg.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_pb1.h"
#include "bchp_xpt_pb2.h"
#include "bchp_xpt_pb3.h"
#include "bchp_xpt_pb4.h"
#include "bchp_xpt_pb5.h"
#include "bchp_xpt_pcroffset.h"
#include "bchp_xpt_pmu.h"
#include "bchp_xpt_psub.h"
#include "bchp_xpt_rave.h"
#include "bchp_xpt_rmx0.h"
#include "bchp_xpt_rmx0_io.h"
#include "bchp_xpt_rmx1.h"
#include "bchp_xpt_rmx1_io.h"
#include "bchp_xpt_rsbuff.h"
#include "bchp_xpt_wakeup.h"
#include "bchp_xpt_xcbuff.h"
#include "bchp_xpt_xmemif.h"
#include "bchp_xpt_xpu.h"
#include "brmt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XPT_PM_SRAM_LIST_NUM_RANGES (36) /* number of ranges in XPT_PM_SRAM_LIST[] */
#define XPT_PM_SRAM_LIST_TOTAL_SIZE (67244) /* sum of sizes (in bytes) of all ranges in XPT_PM_SRAM_LIST[] */

#define XPT_PM_NON_SRAM_LIST_NUM_RANGES (95) /* number of ranges in XPT_PM_NON_SRAM_LIST[] */
#define XPT_PM_NON_SRAM_LIST_TOTAL_SIZE (3092) /* sum of sizes (in bytes) of all ranges in XPT_PM_NON_SRAM_LIST[] */

const BRMT_RegisterRange XPT_PM_SRAM_LIST[] = 
{
    { "WAKEUP_MEM",                              BCHP_XPT_WAKEUP_PKT_MEM_i_ARRAY_BASE,              (BCHP_XPT_WAKEUP_PKT_TYPE3_i_ARRAY_BASE+(BCHP_XPT_WAKEUP_PKT_TYPE3_i_ARRAY_END*4)) },
    { "FE_PID_TABLE",                            BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE,                (BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE+(BCHP_XPT_FE_PID_TABLE_i_ARRAY_END*4)) },
    { "FE_SPID_TABLE",                           BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE,               (BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE+(BCHP_XPT_FE_SPID_TABLE_i_ARRAY_END*4)) },
    { "RSBUFF_PMEM_IB",                          BCHP_XPT_RSBUFF_BASE_POINTER_IBP0,                 BCHP_XPT_RSBUFF_WATERMARK_IBP9 },
    { "RSBUFF_PMEM_PB",                          BCHP_XPT_RSBUFF_BASE_POINTER_PBP0,                 BCHP_XPT_RSBUFF_WATERMARK_PBP5 },
    { "XCBUFF_PMEM_RAVE_IB",                     BCHP_XPT_XCBUFF_BASE_POINTER_RAVE_IBP0,            BCHP_XPT_XCBUFF_WATERMARK_RAVE_IBP9 },
    { "XCBUFF_PMEM_RAVE_PB",                     BCHP_XPT_XCBUFF_BASE_POINTER_RAVE_PBP0,            BCHP_XPT_XCBUFF_WATERMARK_RAVE_PBP5 },
    { "XCBUFF_PMEM_MSG_IB",                      BCHP_XPT_XCBUFF_BASE_POINTER_MSG_IBP0,             BCHP_XPT_XCBUFF_WATERMARK_MSG_IBP9 },
    { "XCBUFF_PMEM_MSG_PB",                      BCHP_XPT_XCBUFF_BASE_POINTER_MSG_PBP0,             BCHP_XPT_XCBUFF_WATERMARK_MSG_PBP5 },
    { "XCBUFF_PMEM_RMX0_IB",                     BCHP_XPT_XCBUFF_BASE_POINTER_RMX0_IBP0,            BCHP_XPT_XCBUFF_WATERMARK_RMX0_IBP9 },
    { "XCBUFF_PMEM_RMX0_PB",                     BCHP_XPT_XCBUFF_BASE_POINTER_RMX0_PBP0,            BCHP_XPT_XCBUFF_WATERMARK_RMX0_PBP5 },
    { "XCBUFF_PMEM_RMX1_IB",                     BCHP_XPT_XCBUFF_BASE_POINTER_RMX1_IBP0,            BCHP_XPT_XCBUFF_WATERMARK_RMX1_IBP9 },
    { "XCBUFF_PMEM_RMX1_PB",                     BCHP_XPT_XCBUFF_BASE_POINTER_RMX1_PBP0,            BCHP_XPT_XCBUFF_WATERMARK_RMX1_PBP5 },
    { "RAVE_PMEM",                               BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR,                BCHP_XPT_RAVE_SCD32_RESERVED_STATE3 },
    { "RAVE_CXMEM_A",                            BCHP_XPT_RAVE_CXMEM_LOi_ARRAY_BASE,                (BCHP_XPT_RAVE_CXMEM_LOi_ARRAY_BASE+(BCHP_XPT_RAVE_CXMEM_LOi_ARRAY_END*4)) },
    { "RAVE_CXMEM_B",                            BCHP_XPT_RAVE_CXMEM_HIi_ARRAY_BASE,                (BCHP_XPT_RAVE_CXMEM_HIi_ARRAY_BASE+(BCHP_XPT_RAVE_CXMEM_HIi_ARRAY_END*4)) },
    { "RAVE_DMEM",                               BCHP_XPT_RAVE_DMEMi_ARRAY_BASE,                    (BCHP_XPT_RAVE_DMEMi_ARRAY_BASE+(BCHP_XPT_RAVE_DMEMi_ARRAY_END*4)) },
    { "RAVE_SMEM",                               BCHP_XPT_RAVE_TPIT0_PID_TABLEi_ARRAY_BASE,         BCHP_XPT_RAVE_TPIT_STATE_CONTEXT23 },
    { "PCROFFSET_PID_TABLE",                     BCHP_XPT_PCROFFSET_PID_CONFIG_TABLE_i_ARRAY_BASE,  (BCHP_XPT_PCROFFSET_PID_CONFIG_TABLE_i_ARRAY_BASE+(BCHP_XPT_PCROFFSET_PID_CONFIG_TABLE_i_ARRAY_END*4)) },
    { "PCROFFSET_CONFIG_MEM",                    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL,        BCHP_XPT_PCROFFSET_CONTEXT15_RESERVED_CFG_0 },
    { "MSG_BUF_CTRL1_TABLE",                     BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ARRAY_BASE,         (BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ARRAY_BASE+(BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ARRAY_END*4)) },
    { "MSG_BUF_CTRL2_TABLE",                     BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ARRAY_BASE,         (BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ARRAY_BASE+(BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ARRAY_END*4)) },
    { "MSG_DMA_BP_TABLE",                        BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE,            (BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE+(BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_END*4)) },
    { "MSG_DMA_RP_TABLE",                        BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_BASE,            (BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_BASE+(BCHP_XPT_MSG_DMA_RP_TABLE_i_ARRAY_END*4)) },
    { "MSG_DMA_VP_TABLE",                        BCHP_XPT_MSG_DMA_VP_TABLE_i_ARRAY_BASE,            (BCHP_XPT_MSG_DMA_VP_TABLE_i_ARRAY_BASE+(BCHP_XPT_MSG_DMA_VP_TABLE_i_ARRAY_END*4)) },
    { "MSG_GEN_FILT_EN_MEM",                     BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE,             (BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE+(BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_END*4)) },
    { "MSG_MCAST_16_MEM",                        BCHP_XPT_MSG_MCAST_16_ADDR_i_ARRAY_BASE,           (BCHP_XPT_MSG_MCAST_16_ADDR_i_ARRAY_BASE+(BCHP_XPT_MSG_MCAST_16_ADDR_i_ARRAY_END*4)) },
    { "MSG_GEN_FILT_COEF_MEM",                   BCHP_XPT_MSG_GEN_FILT_COEF_i_ARRAY_BASE,           (BCHP_XPT_MSG_GEN_FILT_COEF_i_ARRAY_BASE+(BCHP_XPT_MSG_GEN_FILT_COEF_i_ARRAY_END*4)) },
    { "MSG_GEN_FILT_MASK_MEM",                   BCHP_XPT_MSG_GEN_FILT_MASK_i_ARRAY_BASE,           (BCHP_XPT_MSG_GEN_FILT_MASK_i_ARRAY_BASE+(BCHP_XPT_MSG_GEN_FILT_MASK_i_ARRAY_END*4)) },
    { "MSG_GEN_FILT_EXCL_MEM",                   BCHP_XPT_MSG_GEN_FILT_EXCL_i_ARRAY_BASE,           (BCHP_XPT_MSG_GEN_FILT_EXCL_i_ARRAY_BASE+(BCHP_XPT_MSG_GEN_FILT_EXCL_i_ARRAY_END*4)) },
    { "MSG_PID2BUF_MEM",                         BCHP_XPT_MSG_PID2BUF_MAP_i_ARRAY_BASE,             (BCHP_XPT_MSG_PID2BUF_MAP_i_ARRAY_BASE+(BCHP_XPT_MSG_PID2BUF_MAP_i_ARRAY_END*4)) },
    { "FPP_SCMEM0",                              BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_0_i_ARRAY_BASE, (BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_0_i_ARRAY_BASE+(BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_0_i_ARRAY_END*4)) },
    { "FPP_SCMEM1",                              BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_1_i_ARRAY_BASE, (BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_1_i_ARRAY_BASE+(BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_1_i_ARRAY_END*4)) },
    { "FPP_SCMEM2",                              BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_2_i_ARRAY_BASE, (BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_2_i_ARRAY_BASE+(BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_2_i_ARRAY_END*4)) },
    { "FPP_SCMEM3",                              BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_3_i_ARRAY_BASE, (BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_3_i_ARRAY_BASE+(BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_3_i_ARRAY_END*4)) },
    { "RAVE_IMEM",                               BCHP_XPT_XPU_IMEMi_ARRAY_BASE,                     (BCHP_XPT_XPU_IMEMi_ARRAY_BASE+(BCHP_XPT_XPU_IMEMi_ARRAY_END*4)) },
    {0, 0, 0}
};

const BRMT_RegisterRange XPT_PM_NON_SRAM_LIST[] = 
{
    { "XPT_BUS_IF_MISC_CTRL0",                   BCHP_XPT_BUS_IF_MISC_CTRL0,                        BCHP_XPT_BUS_IF_REVISION },
    { "XPT_BUS_IF_INTR_STATUS_REG",              BCHP_XPT_BUS_IF_INTR_STATUS_REG,                   BCHP_XPT_BUS_IF_MAX_SCDS },
    { "XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF",        BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF,             BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB5 },
    { "XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB",          BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB,               BCHP_XPT_XMEMIF_SEC_REGION_HI },
    { "XPT_PMU_CLK_CTRL",                        BCHP_XPT_PMU_CLK_CTRL,                             BCHP_XPT_PMU_RBUS_RSP_VAL },
    { "XPT_WAKEUP_CTRL",                         BCHP_XPT_WAKEUP_CTRL,                              BCHP_XPT_WAKEUP_INTR_STATUS_REG_EN },
    { "XPT_RMX0_IO_FORMAT",                      BCHP_XPT_RMX0_IO_FORMAT,                           BCHP_XPT_RMX0_IO_PKT_DLY_CNT },
    { "XPT_RMX0_IO_TV_STATUS",                   BCHP_XPT_RMX0_IO_TV_STATUS,                        BCHP_XPT_RMX0_IO_TV_STATUS },
    { "XPT_RMX1_IO_FORMAT",                      BCHP_XPT_RMX1_IO_FORMAT,                           BCHP_XPT_RMX1_IO_PKT_DLY_CNT },
    { "XPT_RMX1_IO_TV_STATUS",                   BCHP_XPT_RMX1_IO_TV_STATUS,                        BCHP_XPT_RMX1_IO_TV_STATUS },
    { "XPT_FE_FE_CTRL",                          BCHP_XPT_FE_FE_CTRL,                               BCHP_XPT_FE_FE_CTRL },
    { "XPT_FE_INTR_STATUS0_REG",                 BCHP_XPT_FE_INTR_STATUS0_REG,                      BCHP_XPT_FE_PWR_CTRL },
    { "XPT_FE_MAX_PID_CHANNEL",                  BCHP_XPT_FE_MAX_PID_CHANNEL,                       BCHP_XPT_FE_MAX_PID_CHANNEL },
    { "XPT_FE_IB0_CTRL",                         BCHP_XPT_FE_IB0_CTRL,                              BCHP_XPT_FE_IB7_SYNC_COUNT },
    { "XPT_FE_MINI_PID_PARSER0_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER0_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER1_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER1_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER1_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER2_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER2_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER2_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER3_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER3_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER3_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER4_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER4_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER4_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER5_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER5_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER5_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER6_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER6_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER6_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER7_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER7_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER7_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER8_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER8_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER8_CTRL2 },
    { "XPT_FE_MINI_PID_PARSER9_CTRL1",           BCHP_XPT_FE_MINI_PID_PARSER9_CTRL1,                BCHP_XPT_FE_MINI_PID_PARSER9_CTRL2 },
    { "XPT_FE_IB_SYNC_DETECT_CTRL",              BCHP_XPT_FE_IB_SYNC_DETECT_CTRL,                   BCHP_XPT_FE_IB_SYNC_DETECT_CTRL },
    { "XPT_FE_TSMF0_CTRL",                       BCHP_XPT_FE_TSMF0_CTRL,                            BCHP_XPT_FE_TSMF1_STATUS },
    { "XPT_FE_TV_STATUS_0",                      BCHP_XPT_FE_TV_STATUS_0,                           BCHP_XPT_FE_TV_STATUS_4 },
    { "XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID", BCHP_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID,   BCHP_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID },
    { "XPT_FE_ATS_COUNTER_CTRL",                 BCHP_XPT_FE_ATS_COUNTER_CTRL,                      BCHP_XPT_FE_ATS_TS_BINARY },
    { "XPT_DPCR0_PID_CH",                        BCHP_XPT_DPCR0_PID_CH,                             BCHP_XPT_DPCR0_STC_EXT_CTRL },
    { "XPT_DPCR0_MAX_PCR_ERROR",                 BCHP_XPT_DPCR0_MAX_PCR_ERROR,                      BCHP_XPT_DPCR0_MAX_PCR_ERROR },
    { "XPT_DPCR0_STC_EXT_CTRL27",                BCHP_XPT_DPCR0_STC_EXT_CTRL27,                     BCHP_XPT_DPCR0_LAST_PCR_LO },
    { "XPT_DPCR0_STC_BASE_LSBS",                 BCHP_XPT_DPCR0_STC_BASE_LSBS,                      BCHP_XPT_DPCR0_SOFT_PCR_CTRL },
    { "XPT_DPCR0_PHASE_ERROR_CLAMP",             BCHP_XPT_DPCR0_PHASE_ERROR_CLAMP,                  BCHP_XPT_DPCR0_PHASE_ERROR_CLAMP },
    { "XPT_DPCR1_PID_CH",                        BCHP_XPT_DPCR1_PID_CH,                             BCHP_XPT_DPCR1_STC_EXT_CTRL },
    { "XPT_DPCR1_MAX_PCR_ERROR",                 BCHP_XPT_DPCR1_MAX_PCR_ERROR,                      BCHP_XPT_DPCR1_MAX_PCR_ERROR },
    { "XPT_DPCR1_STC_EXT_CTRL27",                BCHP_XPT_DPCR1_STC_EXT_CTRL27,                     BCHP_XPT_DPCR1_LAST_PCR_LO },
    { "XPT_DPCR1_STC_BASE_LSBS",                 BCHP_XPT_DPCR1_STC_BASE_LSBS,                      BCHP_XPT_DPCR1_SOFT_PCR_CTRL },
    { "XPT_DPCR1_PHASE_ERROR_CLAMP",             BCHP_XPT_DPCR1_PHASE_ERROR_CLAMP,                  BCHP_XPT_DPCR1_PHASE_ERROR_CLAMP },
    { "XPT_DPCR2_PID_CH",                        BCHP_XPT_DPCR2_PID_CH,                             BCHP_XPT_DPCR2_STC_EXT_CTRL },
    { "XPT_DPCR2_MAX_PCR_ERROR",                 BCHP_XPT_DPCR2_MAX_PCR_ERROR,                      BCHP_XPT_DPCR2_MAX_PCR_ERROR },
    { "XPT_DPCR2_STC_EXT_CTRL27",                BCHP_XPT_DPCR2_STC_EXT_CTRL27,                     BCHP_XPT_DPCR2_LAST_PCR_LO },
    { "XPT_DPCR2_STC_BASE_LSBS",                 BCHP_XPT_DPCR2_STC_BASE_LSBS,                      BCHP_XPT_DPCR2_SOFT_PCR_CTRL },
    { "XPT_DPCR2_PHASE_ERROR_CLAMP",             BCHP_XPT_DPCR2_PHASE_ERROR_CLAMP,                  BCHP_XPT_DPCR2_PHASE_ERROR_CLAMP },
    { "XPT_DPCR3_PID_CH",                        BCHP_XPT_DPCR3_PID_CH,                             BCHP_XPT_DPCR3_STC_EXT_CTRL },
    { "XPT_DPCR3_MAX_PCR_ERROR",                 BCHP_XPT_DPCR3_MAX_PCR_ERROR,                      BCHP_XPT_DPCR3_MAX_PCR_ERROR },
    { "XPT_DPCR3_STC_EXT_CTRL27",                BCHP_XPT_DPCR3_STC_EXT_CTRL27,                     BCHP_XPT_DPCR3_LAST_PCR_LO },
    { "XPT_DPCR3_STC_BASE_LSBS",                 BCHP_XPT_DPCR3_STC_BASE_LSBS,                      BCHP_XPT_DPCR3_SOFT_PCR_CTRL },
    { "XPT_DPCR3_PHASE_ERROR_CLAMP",             BCHP_XPT_DPCR3_PHASE_ERROR_CLAMP,                  BCHP_XPT_DPCR3_PHASE_ERROR_CLAMP },
    { "XPT_DPCR_PP_PP_CTRL",                     BCHP_XPT_DPCR_PP_PP_CTRL,                          BCHP_XPT_DPCR_PP_PP_FIXED_OFFSET },
    { "XPT_PSUB_PSUB0_CTRL0",                    BCHP_XPT_PSUB_PSUB0_CTRL0,                         BCHP_XPT_PSUB_PSUB7_STAT2 },
    { "XPT_PSUB_DEBUG_REG",                      BCHP_XPT_PSUB_DEBUG_REG,                           BCHP_XPT_PSUB_PR_FALLBACK_CTRL },
    { "XPT_RSBUFF_BO_IBP0",                      BCHP_XPT_RSBUFF_BO_IBP0,                           BCHP_XPT_RSBUFF_BO_IBP9 },
    { "XPT_RSBUFF_BO_PBP0",                      BCHP_XPT_RSBUFF_BO_PBP0,                           BCHP_XPT_RSBUFF_BO_PBP5 },
    { "XPT_RSBUFF_IBP_BUFFER_ENABLE",            BCHP_XPT_RSBUFF_IBP_BUFFER_ENABLE,                 BCHP_XPT_RSBUFF_PR_FALLBACK_CTRL },
    { "XPT_PB0_CTRL1",                           BCHP_XPT_PB0_CTRL1,                                BCHP_XPT_PB0_PLAYBACK_PARSER_BAND_ID },
    { "XPT_PB1_CTRL1",                           BCHP_XPT_PB1_CTRL1,                                BCHP_XPT_PB1_PLAYBACK_PARSER_BAND_ID },
    { "XPT_PB2_CTRL1",                           BCHP_XPT_PB2_CTRL1,                                BCHP_XPT_PB2_PLAYBACK_PARSER_BAND_ID },
    { "XPT_PB3_CTRL1",                           BCHP_XPT_PB3_CTRL1,                                BCHP_XPT_PB3_PLAYBACK_PARSER_BAND_ID },
    { "XPT_PB4_CTRL1",                           BCHP_XPT_PB4_CTRL1,                                BCHP_XPT_PB4_PLAYBACK_PARSER_BAND_ID },
    { "XPT_PB5_CTRL1",                           BCHP_XPT_PB5_CTRL1,                                BCHP_XPT_PB5_PLAYBACK_PARSER_BAND_ID },
    { "XPT_MPOD_CFG",                            BCHP_XPT_MPOD_CFG,                                 BCHP_XPT_MPOD_RES_FIELD },
    { "XPT_MPOD_TV_STATUS",                      BCHP_XPT_MPOD_TV_STATUS,                           BCHP_XPT_MPOD_TV_STATUS },
    { "XPT_RMX0_CTRL",                           BCHP_XPT_RMX0_CTRL,                                BCHP_XPT_RMX0_FIXED_OFFSET },
    { "XPT_RMX1_CTRL",                           BCHP_XPT_RMX1_CTRL,                                BCHP_XPT_RMX1_FIXED_OFFSET },
    { "XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_PBP",      BCHP_XPT_XCBUFF_RAVE_CTRL_BUFFER_EN_PBP,           BCHP_XPT_XCBUFF_RMX1_PAUSE_STATUS_IBP },
    { "XPT_XCBUFF_OVERFLOW_THRESHOLD",           BCHP_XPT_XCBUFF_OVERFLOW_THRESHOLD,                BCHP_XPT_XCBUFF_BO_RAVE_IBP9 },
    { "XPT_XCBUFF_BO_RAVE_PBP0",                 BCHP_XPT_XCBUFF_BO_RAVE_PBP0,                      BCHP_XPT_XCBUFF_BO_RAVE_PBP5 },
    { "XPT_XCBUFF_BO_MSG_IBP0",                  BCHP_XPT_XCBUFF_BO_MSG_IBP0,                       BCHP_XPT_XCBUFF_BO_MSG_IBP9 },
    { "XPT_XCBUFF_BO_MSG_PBP0",                  BCHP_XPT_XCBUFF_BO_MSG_PBP0,                       BCHP_XPT_XCBUFF_BO_MSG_PBP5 },
    { "XPT_XCBUFF_BO_RMX0_IBP0",                 BCHP_XPT_XCBUFF_BO_RMX0_IBP0,                      BCHP_XPT_XCBUFF_BO_RMX0_IBP9 },
    { "XPT_XCBUFF_BO_RMX0_PBP0",                 BCHP_XPT_XCBUFF_BO_RMX0_PBP0,                      BCHP_XPT_XCBUFF_BO_RMX0_PBP5 },
    { "XPT_XCBUFF_BO_RMX1_IBP0",                 BCHP_XPT_XCBUFF_BO_RMX1_IBP0,                      BCHP_XPT_XCBUFF_BO_RMX1_IBP9 },
    { "XPT_XCBUFF_BO_RMX1_PBP0",                 BCHP_XPT_XCBUFF_BO_RMX1_PBP0,                      BCHP_XPT_XCBUFF_BO_RMX1_PBP5 },
    { "XPT_XCBUFF_NO_RD_HANG_CTRL",              BCHP_XPT_XCBUFF_NO_RD_HANG_CTRL,                   BCHP_XPT_XCBUFF_PR_FALLBACK_CTRL },
    { "XPT_RAVE_XPU_CONFIG",                     BCHP_XPT_RAVE_XPU_CONFIG,                          BCHP_XPT_RAVE_AVS_SCV_FILTER_VALUE_4_TO_7 },
    { "XPT_RAVE_AV_STATUS",                      BCHP_XPT_RAVE_AV_STATUS,                           BCHP_XPT_RAVE_WATCHDOG_TIMER_VALUE },
    { "XPT_RAVE_MISC_CONTROL",                   BCHP_XPT_RAVE_MISC_CONTROL,                        BCHP_XPT_RAVE_BASE_ADDRESSES },
    { "XPT_RAVE_FW_WATERMARK",                   BCHP_XPT_RAVE_FW_WATERMARK,                        BCHP_XPT_RAVE_MISC_CONTROL3 },
    { "XPT_RAVE_RC0_SP_CONTROL",                 BCHP_XPT_RAVE_RC0_SP_CONTROL,                      BCHP_XPT_RAVE_ATSOFFSET_MAX_ERROR },
    { "XPT_RAVE_INT_CX0",                        BCHP_XPT_RAVE_INT_CX0,                             BCHP_XPT_RAVE_INT_MISC },
    { "XPT_RAVE_TPIT_TIME_TICK",                 BCHP_XPT_RAVE_TPIT_TIME_TICK,                      BCHP_XPT_RAVE_TPIT_EVE_TIMEOUT },
    { "XPT_RAVE_EMM_TID_MODE",                   BCHP_XPT_RAVE_EMM_TID_MODE,                        BCHP_XPT_RAVE_EMM_CTRL_ID },
    { "XPT_RAVE_EMM_DATA_ID_1",                  BCHP_XPT_RAVE_EMM_DATA_ID_1,                       BCHP_XPT_RAVE_EMM_MASK_ID_8 },
    { "XPT_PCROFFSET_INTERRUPT0_STATUS",         BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS,              BCHP_XPT_PCROFFSET_STC3 },
    { "XPT_PCROFFSET_STC_BROADCAST_SEL",         BCHP_XPT_PCROFFSET_STC_BROADCAST_SEL,              BCHP_XPT_PCROFFSET_STC3_MATCH },
    { "XPT_PCROFFSET_CONTEXT15_RESERVED_CFG_1",  BCHP_XPT_PCROFFSET_CONTEXT15_RESERVED_CFG_1,       BCHP_XPT_PCROFFSET_CONTEXT15_RESERVED_CFG_4 },
    { "XPT_MSG_MSG_CTRL1",                       BCHP_XPT_MSG_MSG_CTRL1,                            BCHP_XPT_MSG_DMA_BUFFER_INIT },
    { "XPT_MSG_PHY_ADDR_LO",                     BCHP_XPT_MSG_PHY_ADDR_LO,                          BCHP_XPT_MSG_ID_REJECT },
    { "XPT_MSG_C_MATCH0",                        BCHP_XPT_MSG_C_MATCH0,                             BCHP_XPT_MSG_C_MATCH4 },
    { "XPT_MSG_BUF_DAT_AVAIL_00_31",             BCHP_XPT_MSG_BUF_DAT_AVAIL_00_31,                  BCHP_XPT_MSG_DAT_ERR_INTR_EN },
    { "XPT_MSG_BUF_ERR_00_31",                   BCHP_XPT_MSG_BUF_ERR_00_31,                        BCHP_XPT_MSG_MSG_ERR_CK_FAIL_CNT },
    { "XPT_MSG_MSG_DEBUG_0",                     BCHP_XPT_MSG_MSG_DEBUG_0,                          BCHP_XPT_MSG_MSG_DEBUG_4 },
    { "XPT_GR_REVISION",                         BCHP_XPT_GR_REVISION,                              BCHP_XPT_GR_SW_INIT_1 },
    { "XPT_FULL_PID_PARSER_PARSER_CFG",          BCHP_XPT_FULL_PID_PARSER_PARSER_CFG,               BCHP_XPT_FULL_PID_PARSER_PSG_ERR_MODE },
    {0, 0, 0}
};

#ifdef __cplusplus
}
#endif
#endif
