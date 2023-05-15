/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: $
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bchp.h"
#include "bkni.h"
#include "bmxt_priv.h"
#include "bmxt.h"
#if (!BMXT_REG_SUPPORT)
#include "bmxt_3128_demod_xpt_fe.h"
#else
#include "bmxt_3128_demod_xpt_fe.h" /* 3128-impl always includes the 3128 header*/
#endif
#if BMXT_HAB_SUPPORT
#include "bhab.h"
#endif
#if BMXT_RPC_SUPPORT
#include "brpc.h"
#if BMXT_3383_SUPPORT
#include "brpc_3255.h"
#else
#include "brpc_docsis.h"
#endif
#endif

BDBG_MODULE(bmxt_3128);

#include "bmxt_common.c"

BERR_Code BMXT_3128_P_GetDefaultSettings(BMXT_Settings *pSettings)
{
    return BMXT_P_GetDefaultSettings_common(pSettings);
}

BERR_Code BMXT_3128_Open(BMXT_Handle *pHandle, BCHP_Handle hChp, BREG_Handle hReg, const BMXT_Settings *pSettings)
{
    return BMXT_Open_common(pHandle, hChp, hReg, pSettings);
}

void BMXT_3128_Close(BMXT_Handle handle)
{
    BMXT_Close_common(handle);
}

BERR_Code BMXT_3128_GetMtsifStatus(BMXT_Handle handle, BMXT_MtsifStatus *pStatus)
{
    return BMXT_GetMtsifStatus_common(handle, pStatus);
}

BERR_Code BMXT_3128_GetInputBandStatus(BMXT_Handle handle, BMXT_InputBandStatus *pStatus)
{
    return BMXT_GetInputBandStatus_common(handle, pStatus);
}

BERR_Code BMXT_3128_GetParserConfig(BMXT_Handle handle, unsigned parserNum, BMXT_ParserConfig *pConfig)
{
    return BMXT_GetParserConfig_common(handle, parserNum, pConfig);
}

BERR_Code BMXT_3128_SetParserConfig(BMXT_Handle handle, unsigned parserNum, const BMXT_ParserConfig *pConfig)
{
    return BMXT_SetParserConfig_common(handle, parserNum, pConfig);
}

BERR_Code BMXT_3128_GetParserMapping(BMXT_Handle handle, BMXT_ParserBandMapping *pMapping)
{
    return BMXT_GetParserMapping_common(handle, pMapping);
}

BERR_Code BMXT_3128_SetParserMapping(BMXT_Handle handle, const BMXT_ParserBandMapping *pMapping)
{
    return BMXT_SetParserMapping_common(handle, pMapping);
}

BERR_Code BMXT_3128_GetInputBandConfig(BMXT_Handle handle, unsigned bandNum, BMXT_InputBandConfig *pConfig)
{
    return BMXT_GetInputBandConfig_common(handle, bandNum, pConfig);
}

BERR_Code BMXT_3128_SetInputBandConfig(BMXT_Handle handle, unsigned bandNum, const BMXT_InputBandConfig *pConfig)
{
    return BMXT_SetInputBandConfig_common(handle, bandNum, pConfig);
}

uint32_t BMXT_3128_ReadIntStatusRegister(BMXT_Handle handle, BMXT_IntReg intReg)
{
    return BMXT_ReadIntStatusRegister_common(handle, intReg);
}

/****************************************************************
TSMF APIs
****************************************************************/

BERR_Code BMXT_3128_TSMF_GetFldVerifyConfig(BMXT_Handle handle, unsigned tsmfNum, BMXT_TSMFFldVerifyConfig *pConfig)
{
    return BMXT_TSMF_GetFldVerifyConfig_common(handle, tsmfNum, pConfig);
}

BERR_Code BMXT_3128_TSMF_SetFldVerifyConfig(BMXT_Handle handle, unsigned tsmfNum, const BMXT_TSMFFldVerifyConfig *pConfig)
{
    return BMXT_TSMF_SetFldVerifyConfig_common(handle, tsmfNum, pConfig);
}

BERR_Code BMXT_3128_TSMF_EnableAutoMode(BMXT_Handle handle, BMXT_TSMFInputSel input, unsigned tsmfNum, uint8_t relativeTsNum)
{
    return BMXT_TSMF_EnableAutoMode_common(handle, input, tsmfNum, relativeTsNum);
}

BERR_Code BMXT_3128_TSMF_EnableSemiAutoMode(BMXT_Handle handle, BMXT_TSMFInputSel input, unsigned tsmfNum, uint32_t slotMapLo, uint32_t slotMapHi, uint8_t relativeTsNum)
{
    return BMXT_TSMF_EnableSemiAutoMode_common(handle, input, tsmfNum, slotMapLo, slotMapHi, relativeTsNum);
}

BERR_Code BMXT_3128_TSMF_DisableTsmf(BMXT_Handle handle, unsigned tsmfNum)
{
    return BMXT_TSMF_DisableTsmf_common(handle, tsmfNum);
}

BERR_Code BMXT_3128_TSMF_SetParserConfig(BMXT_Handle handle, unsigned parserNum, unsigned tsmfNum, bool enable)
{
    return BMXT_TSMF_SetParserConfig_common(handle, parserNum, tsmfNum, enable);
}

/****************************************************************
Power APIs
****************************************************************/

void BMXT_3128_GetPowerSaverSettings(BMXT_Handle handle, BMXT_PowerSaverSettings *pSettings)
{
    BMXT_GetPowerSaverSettings_common(handle, pSettings);
}

BERR_Code BMXT_3128_SetPowerSaverSettings(BMXT_Handle handle, const BMXT_PowerSaverSettings *pSettings)
{
    return BMXT_SetPowerSaverSettings_common(handle, pSettings);
}

/****************************************************************
Debug
****************************************************************/

#define MAKE_REG(name) BCHP_##name
#define MAKE_STR(name) #name
#define PRINT_REG(name) BKNI_Printf("%-s = %08x\n", MAKE_STR(name), BMXT_RegRead32(handle, MAKE_REG(name)))

void BMXT_3128_P_RegDump(BMXT_Handle handle, unsigned param)
{
    /* param==1 prints a small subset */
    if (param!=1) {
        PRINT_REG(DEMOD_XPT_FE_FE_CTRL);
        PRINT_REG(DEMOD_XPT_FE_INTR_STATUS0_REG);
        PRINT_REG(DEMOD_XPT_FE_INTR_STATUS1_REG);
        PRINT_REG(DEMOD_XPT_FE_INTR_STATUS0_REG_EN);
        PRINT_REG(DEMOD_XPT_FE_INTR_STATUS1_REG_EN);
        PRINT_REG(DEMOD_XPT_FE_PWR_CTRL);
        PRINT_REG(DEMOD_XPT_FE_MAX_PID_CHANNEL);
        PRINT_REG(DEMOD_XPT_FE_IB0_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB0_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB1_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB1_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB2_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB2_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB3_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB3_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB4_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB4_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB5_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB5_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB6_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB6_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB7_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB7_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB8_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB8_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB9_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB9_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB10_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB10_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB11_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB11_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB12_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB12_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB13_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB13_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB14_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB14_SYNC_COUNT);
        PRINT_REG(DEMOD_XPT_FE_IB15_CTRL);
        PRINT_REG(DEMOD_XPT_FE_IB15_SYNC_COUNT);
    }
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL2);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER1_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER1_CTRL2);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER2_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER2_CTRL2);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER3_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER3_CTRL2);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER4_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER4_CTRL2);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER5_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER5_CTRL2);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER6_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER6_CTRL2);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER7_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER7_CTRL2);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER8_CTRL1);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER8_CTRL2);
    if (param!=1) {
        PRINT_REG(DEMOD_XPT_FE_IB_SYNC_DETECT_CTRL);
        PRINT_REG(DEMOD_XPT_FE_TSMF0_CTRL);
        PRINT_REG(DEMOD_XPT_FE_TSMF0_SLOT_MAP_LO);
        PRINT_REG(DEMOD_XPT_FE_TSMF0_SLOT_MAP_HI);
        PRINT_REG(DEMOD_XPT_FE_TSMF0_STATUS);
        PRINT_REG(DEMOD_XPT_FE_TSMF1_CTRL);
        PRINT_REG(DEMOD_XPT_FE_TSMF1_SLOT_MAP_LO);
        PRINT_REG(DEMOD_XPT_FE_TSMF1_SLOT_MAP_HI);
        PRINT_REG(DEMOD_XPT_FE_TSMF1_STATUS);
        PRINT_REG(DEMOD_XPT_FE_TV_STATUS_0);
        PRINT_REG(DEMOD_XPT_FE_TV_STATUS_1);
        PRINT_REG(DEMOD_XPT_FE_TV_STATUS_2);
        PRINT_REG(DEMOD_XPT_FE_TV_STATUS_3);
        PRINT_REG(DEMOD_XPT_FE_TV_STATUS_4);
        PRINT_REG(DEMOD_XPT_FE_MTSIF_TX0_TV_CTRL);
    }
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID);
    PRINT_REG(DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID);
    if (param!=1) {
        PRINT_REG(DEMOD_XPT_FE_ATS_COUNTER_CTRL);
        PRINT_REG(DEMOD_XPT_FE_ATS_TS_MOD300);
        PRINT_REG(DEMOD_XPT_FE_ATS_TS_BINARY);
        PRINT_REG(DEMOD_XPT_FE_MTSIF_TX0_CTRL1);
        PRINT_REG(DEMOD_XPT_FE_MTSIF_TX0_BLOCK_OUT);
        PRINT_REG(DEMOD_XPT_FE_MTSIF_TX0_STATUS);
        PRINT_REG(DEMOD_XPT_FE_MTSIF_TX0_SECRET_WORD);
        PRINT_REG(DEMOD_XPT_FE_MTSIF_LEGACY_CTRL);
        PRINT_REG(DEMOD_XPT_FE_MTSIF_LEGACY_STATUS);
    }

    if (param!=1) {
        unsigned i;
        BKNI_Printf("DEMOD_XPT_FE_PID_TABLE[0..255]\n");
        for (i=0; i<255; i++) {
            BKNI_Printf("[%3u] %08x\n", i, BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_PID_TABLE_i_ARRAY_BASE + (4*i)));
        }
        BKNI_Printf("DEMOD_XPT_FE_SPID_TABLE[0..255]\n");
        for (i=0; i<255; i++) {
            BKNI_Printf("[%3u] %08x\n", i, BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_SPID_TABLE_i_ARRAY_BASE + (4*i)));
        }
    }
}
