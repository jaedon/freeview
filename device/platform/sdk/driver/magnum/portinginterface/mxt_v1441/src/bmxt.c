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

BDBG_MODULE(bmxt);

/* 
           3128  3383  3383  4528  4517  3472  4538 4538 3384 7366 7366
           ABC0   A01    B0    A0    A0    A0   AB0   C0   A0   A0   B0

family     3128  3128  3128  3128  3128  3128  4538 4538 4538 4538 4538
MTSIF TX      1     1     1     2     2     2     2    2    2    1    2
MTSIF RX      0     0     0     0     1     1     1    1    1    1    1
IB           16    16    16    16    16    16    16   16   18   16   26
PB            9     9     9     9    16    16    32   32   32   16   32
PB_REMAP      9     9     9     9    12    12    10   10   18    8   24
PB_TBG        0     0     0     0     0     0     0   10    0    0   24
TSMF          2     2     8     2     8     8    10   10   16    8   16

- 7145 A0 has no DEMOD_XPT_FE block. therefore, all resources are 0
- 7145 B0 is equivalent to 7366 A0
- 4548 is a bondout of 7366
- 45216 A0 has same resources as 7366 B0
- 7364 A0 has same resources as 7366 B0

note: bmxt_3128_demod_xpt_fe.h is using the 3472 RDB, because 3128 RDB doesn't have MTSIF_TX1 nor _RX0
note: when these numbers change, BMXT_MAX_NUM_* may need to get updated
*/

#define BMXT_NUM_RESOURCE_TYPES 7

static const unsigned mxt_rc_3128[BMXT_NUM_RESOURCE_TYPES] =   { 1, 0, 16, 9,  9,  0,  2  }; /* and 3383Ax */
static const unsigned mxt_rc_3383b0[BMXT_NUM_RESOURCE_TYPES] = { 1, 0, 16, 9,  9,  0,  8  };
static const unsigned mxt_rc_4528[BMXT_NUM_RESOURCE_TYPES] =   { 2, 0, 16, 9,  9,  0,  2  };
static const unsigned mxt_rc_4517[BMXT_NUM_RESOURCE_TYPES] =   { 2, 1, 16, 16, 12, 0,  8  }; /* and 3472 */
static const unsigned mxt_rc_4538a0[BMXT_NUM_RESOURCE_TYPES] = { 2, 1, 16, 32, 10, 0,  10 };
static const unsigned mxt_rc_4538c0[BMXT_NUM_RESOURCE_TYPES] = { 2, 1, 16, 32, 10, 10, 10 };
static const unsigned mxt_rc_3384[BMXT_NUM_RESOURCE_TYPES] =   { 2, 1, 18, 32, 18, 0,  16 };
static const unsigned mxt_rc_7366a0[BMXT_NUM_RESOURCE_TYPES] = { 1, 1, 16, 16, 8,  0,  8 };  /* and 7145B0, 4548 */
static const unsigned mxt_rc_7366b0[BMXT_NUM_RESOURCE_TYPES] = { 2, 1, 26, 32, 24, 24, 16 }; /* and 45216, 7364 */
static const unsigned mxt_rc_null[BMXT_NUM_RESOURCE_TYPES] =   { 0, 0, 0,  0,  0,  0,  0  }; /* 7145A0 */

static void BMXT_P_GetNumResourcesPtr(BMXT_Chip chip, BMXT_ChipRev rev, const unsigned **ptr)
{
    *ptr = NULL;

    switch (chip) {
        case BMXT_Chip_e3128: *ptr = mxt_rc_3128; break;
        case BMXT_Chip_e3383:
            if (rev<BMXT_ChipRev_eB0) { *ptr = mxt_rc_3128; break; }
            else { *ptr = mxt_rc_3383b0; break; }
        case BMXT_Chip_e4528: *ptr = mxt_rc_4528; break;
        case BMXT_Chip_e4517: *ptr = mxt_rc_4517; break;
        case BMXT_Chip_e3472: *ptr = mxt_rc_4517; break;
        case BMXT_Chip_e4538:
            if (rev<BMXT_ChipRev_eC0) { *ptr = mxt_rc_4538a0; break; }
            else { *ptr = mxt_rc_4538c0; break; }
        case BMXT_Chip_e3384: *ptr = mxt_rc_3384; break;
        case BMXT_Chip_e7366:
            if (rev<BMXT_ChipRev_eB0) { *ptr = mxt_rc_7366a0; break; }
            else { *ptr = mxt_rc_7366b0; break; }
        case BMXT_Chip_e7145:
            if (rev<BMXT_ChipRev_eB0) { *ptr = mxt_rc_null; break; }
            else { *ptr = mxt_rc_7366b0; break; }
        case BMXT_Chip_e4548: *ptr = mxt_rc_7366a0; break;
        case BMXT_Chip_e45216: *ptr = mxt_rc_7366b0; break;
        case BMXT_Chip_e7364: *ptr = mxt_rc_7366b0; break;
        default: break;
    }
}

unsigned BMXT_GetNumResources(BMXT_Chip chip, BMXT_ChipRev rev, BMXT_ResourceType resource)
{
    const unsigned *ptr = NULL;
    unsigned val;
    BMXT_P_GetNumResourcesPtr(chip, rev, &ptr);
    if (ptr==NULL) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch (resource) {
        case BMXT_ResourceType_eMtsifTx:     val = ptr[0]; break;
        case BMXT_ResourceType_eMtsifRx:     val = ptr[1]; break;
        case BMXT_ResourceType_eInputBand:   val = ptr[2]; break;
        case BMXT_ResourceType_eParser:      val = ptr[3]; break;
        case BMXT_ResourceType_eParserRemap: val = ptr[4]; break;
        case BMXT_ResourceType_eParserTbg:   val = ptr[5]; break;
        case BMXT_ResourceType_eTsmf:        val = ptr[6]; break;
        default: { BERR_TRACE(BERR_INVALID_PARAMETER); return 0; }
    }
    return val;
}

void BMXT_P_GetNumResourcesAll(BMXT_Chip chip, BMXT_ChipRev rev, BMXT_P_Resources *pNum)
{
    const unsigned *ptr = NULL;
    BMXT_P_GetNumResourcesPtr(chip, rev, &ptr);
    if (ptr==NULL) {
        BERR_TRACE(BERR_INVALID_PARAMETER);
        return;
    }
    pNum->mtsifTx = ptr[0];
    pNum->mtsifRx = ptr[1];
    pNum->ib =      ptr[2];
    pNum->pb =      ptr[3];
    pNum->pbRemap = ptr[4];
    pNum->pbTbg =   ptr[5];
    pNum->tsmf =    ptr[6];
}

uint32_t BMXT_P_GetRegOffsetFe(BMXT_Handle handle)
{
    BMXT_Chip chip = handle->settings.chip;

    if (BMXT_IS_RPC_BASED(chip)) {
        /* the RPC call takes offset-based (i.e. 0-based) register addresses */
        return 0;
    }
    else if (BMXT_IS_REG_BASED(chip)) {
        return 0;
    }
    else {
        switch (chip) {
            case BMXT_Chip_e3128:  return 0x000c5000;
            case BMXT_Chip_e3472:  return 0x00105000;
            case BMXT_Chip_e4517:  return 0x00095000;
            case BMXT_Chip_e4528:  return 0x00095000;
            case BMXT_Chip_e4538:  return 0x00096000;
            case BMXT_Chip_e4548:  return 0x01272000;
            case BMXT_Chip_e45216: return 0x06b02000;
            default: goto error;
        }
    }
error:
    BERR_TRACE(BERR_UNKNOWN);
    return 0;
}

BERR_Code BMXT_Open(BMXT_Handle *pHandle, BCHP_Handle hChp, BREG_Handle hReg, const BMXT_Settings *pSettings)
{  
    return pSettings->api.Open(pHandle, hChp, hReg, pSettings);
}

void BMXT_Close(BMXT_Handle handle)
{
    handle->settings.api.Close(handle);
}

BERR_Code BMXT_GetMtsifStatus(BMXT_Handle handle, BMXT_MtsifStatus *pStatus)
{
    return handle->settings.api.GetMtsifStatus(handle, pStatus);
}

BERR_Code BMXT_GetInputBandStatus(BMXT_Handle handle, BMXT_InputBandStatus *pStatus)
{
    return handle->settings.api.GetInputBandStatus(handle, pStatus);
}

BERR_Code BMXT_GetParserConfig(BMXT_Handle handle, unsigned parserNum, BMXT_ParserConfig *pConfig)
{
    return handle->settings.api.GetParserConfig(handle, parserNum, pConfig);
}

BERR_Code BMXT_SetParserConfig(BMXT_Handle handle, unsigned parserNum, const BMXT_ParserConfig *pConfig)
{
    return handle->settings.api.SetParserConfig(handle, parserNum, pConfig);
}

BERR_Code BMXT_GetParserMapping(BMXT_Handle handle, BMXT_ParserBandMapping *pMapping)
{
    return handle->settings.api.GetParserMapping(handle, pMapping);
}

BERR_Code BMXT_SetParserMapping(BMXT_Handle handle, const BMXT_ParserBandMapping *pMapping)
{
    return handle->settings.api.SetParserMapping(handle, pMapping);
}

BERR_Code BMXT_GetInputBandConfig(BMXT_Handle handle, unsigned bandNum, BMXT_InputBandConfig *pConfig)
{
    return handle->settings.api.GetInputBandConfig(handle, bandNum, pConfig);
}

BERR_Code BMXT_SetInputBandConfig(BMXT_Handle handle, unsigned bandNum, const BMXT_InputBandConfig *pConfig)
{
    return handle->settings.api.SetInputBandConfig(handle, bandNum, pConfig);
}

uint32_t BMXT_ReadIntStatusRegister(BMXT_Handle handle, BMXT_IntReg intReg)
{
    return handle->settings.api.ReadIntStatusRegister(handle, intReg);
}

BERR_Code BMXT_TSMF_GetFldVerifyConfig(BMXT_Handle handle, unsigned tsmfNum, BMXT_TSMFFldVerifyConfig *pConfig)
{
    return handle->settings.api.TSMF_GetFldVerifyConfig(handle, tsmfNum, pConfig);
}

BERR_Code BMXT_TSMF_SetFldVerifyConfig(BMXT_Handle handle, unsigned tsmfNum, const BMXT_TSMFFldVerifyConfig *pConfig)
{
    return handle->settings.api.TSMF_SetFldVerifyConfig(handle, tsmfNum, pConfig);
}

BERR_Code BMXT_TSMF_EnableAutoMode(BMXT_Handle handle, BMXT_TSMFInputSel input, unsigned tsmfNum, uint8_t relativeTsNum)
{
    return handle->settings.api.TSMF_EnableAutoMode(handle, input, tsmfNum, relativeTsNum);
}

BERR_Code BMXT_TSMF_EnableSemiAutoMode(BMXT_Handle handle, BMXT_TSMFInputSel input, unsigned tsmfNum, uint32_t slotMapLo, uint32_t slotMapHi, uint8_t relativeTsNum)
{
    return handle->settings.api.TSMF_EnableSemiAutoMode(handle, input, tsmfNum, slotMapLo, slotMapHi, relativeTsNum);
}

BERR_Code BMXT_TSMF_DisableTsmf(BMXT_Handle handle, unsigned tsmfNum)
{
    return handle->settings.api.TSMF_DisableTsmf(handle, tsmfNum);
}

BERR_Code BMXT_TSMF_SetParserConfig(BMXT_Handle handle, unsigned parserNum, unsigned tsmfNum, bool enable)
{
    return handle->settings.api.TSMF_SetParserConfig(handle, parserNum, tsmfNum, enable);
}

void BMXT_GetPowerSaverSettings(BMXT_Handle handle, BMXT_PowerSaverSettings *pSettings)
{
    handle->settings.api.GetPowerSaverSettings(handle, pSettings);
}

BERR_Code BMXT_SetPowerSaverSettings(BMXT_Handle handle, const BMXT_PowerSaverSettings *pSettings)
{
    return handle->settings.api.SetPowerSaverSettings(handle, pSettings);
}

BERR_Code BMXT_3128_P_GetDefaultSettings(BMXT_Settings *pSettings);
BERR_Code BMXT_3128_Open(BMXT_Handle *, BCHP_Handle, BREG_Handle, const BMXT_Settings *);
void BMXT_3128_Close(BMXT_Handle);
BERR_Code BMXT_3128_GetMtsifStatus(BMXT_Handle, BMXT_MtsifStatus *);
BERR_Code BMXT_3128_GetInputBandStatus(BMXT_Handle handle, BMXT_InputBandStatus *);
BERR_Code BMXT_3128_GetParserConfig(BMXT_Handle, unsigned, BMXT_ParserConfig *);
BERR_Code BMXT_3128_SetParserConfig(BMXT_Handle, unsigned, const BMXT_ParserConfig *);
BERR_Code BMXT_3128_GetParserMapping(BMXT_Handle, BMXT_ParserBandMapping *);
BERR_Code BMXT_3128_SetParserMapping(BMXT_Handle, const BMXT_ParserBandMapping *);
BERR_Code BMXT_3128_GetInputBandConfig(BMXT_Handle, unsigned, BMXT_InputBandConfig *);
BERR_Code BMXT_3128_SetInputBandConfig(BMXT_Handle, unsigned, const BMXT_InputBandConfig *);
uint32_t BMXT_3128_ReadIntStatusRegister(BMXT_Handle, BMXT_IntReg);
BERR_Code BMXT_3128_TSMF_GetFldVerifyConfig(BMXT_Handle, unsigned, BMXT_TSMFFldVerifyConfig *);
BERR_Code BMXT_3128_TSMF_SetFldVerifyConfig(BMXT_Handle, unsigned, const BMXT_TSMFFldVerifyConfig *);
BERR_Code BMXT_3128_TSMF_EnableAutoMode(BMXT_Handle, BMXT_TSMFInputSel, unsigned, uint8_t);
BERR_Code BMXT_3128_TSMF_EnableSemiAutoMode(BMXT_Handle, BMXT_TSMFInputSel, unsigned, uint32_t, uint32_t, uint8_t);
BERR_Code BMXT_3128_TSMF_DisableTsmf(BMXT_Handle, unsigned);
BERR_Code BMXT_3128_TSMF_SetParserConfig(BMXT_Handle, unsigned, unsigned, bool);
void BMXT_3128_GetPowerSaverSettings(BMXT_Handle, BMXT_PowerSaverSettings *);
BERR_Code BMXT_3128_SetPowerSaverSettings(BMXT_Handle, const BMXT_PowerSaverSettings *);

BERR_Code BMXT_4538_P_GetDefaultSettings(BMXT_Settings *pSettings);
BERR_Code BMXT_4538_Open(BMXT_Handle *, BCHP_Handle, BREG_Handle, const BMXT_Settings *);
void BMXT_4538_Close(BMXT_Handle);
BERR_Code BMXT_4538_GetMtsifStatus(BMXT_Handle, BMXT_MtsifStatus *);
BERR_Code BMXT_4538_GetInputBandStatus(BMXT_Handle handle, BMXT_InputBandStatus *);
BERR_Code BMXT_4538_GetParserConfig(BMXT_Handle, unsigned, BMXT_ParserConfig *);
BERR_Code BMXT_4538_SetParserConfig(BMXT_Handle, unsigned, const BMXT_ParserConfig *);
BERR_Code BMXT_4538_GetParserMapping(BMXT_Handle, BMXT_ParserBandMapping *);
BERR_Code BMXT_4538_SetParserMapping(BMXT_Handle, const BMXT_ParserBandMapping *);
BERR_Code BMXT_4538_GetInputBandConfig(BMXT_Handle, unsigned, BMXT_InputBandConfig *);
BERR_Code BMXT_4538_SetInputBandConfig(BMXT_Handle, unsigned, const BMXT_InputBandConfig *);
uint32_t BMXT_4538_ReadIntStatusRegister(BMXT_Handle, BMXT_IntReg);
BERR_Code BMXT_4538_TSMF_GetFldVerifyConfig(BMXT_Handle, unsigned, BMXT_TSMFFldVerifyConfig *);
BERR_Code BMXT_4538_TSMF_SetFldVerifyConfig(BMXT_Handle, unsigned, const BMXT_TSMFFldVerifyConfig *);
BERR_Code BMXT_4538_TSMF_EnableAutoMode(BMXT_Handle, BMXT_TSMFInputSel, unsigned, uint8_t);
BERR_Code BMXT_4538_TSMF_EnableSemiAutoMode(BMXT_Handle, BMXT_TSMFInputSel, unsigned, uint32_t, uint32_t, uint8_t);
BERR_Code BMXT_4538_TSMF_DisableTsmf(BMXT_Handle, unsigned);
BERR_Code BMXT_4538_TSMF_SetParserConfig(BMXT_Handle, unsigned, unsigned, bool);
void BMXT_4538_GetPowerSaverSettings(BMXT_Handle, BMXT_PowerSaverSettings *);
BERR_Code BMXT_4538_SetPowerSaverSettings(BMXT_Handle, const BMXT_PowerSaverSettings *);

void BMXT_P_Set3128Api(BMXT_Settings *pSettings)
{
#if BMXT_3128_FAMILY
    pSettings->api.Open = BMXT_3128_Open;
    pSettings->api.Close = BMXT_3128_Close;
    pSettings->api.GetMtsifStatus = BMXT_3128_GetMtsifStatus;
    pSettings->api.GetInputBandStatus = BMXT_3128_GetInputBandStatus;
    pSettings->api.GetParserMapping = BMXT_3128_GetParserMapping;
    pSettings->api.SetParserMapping = BMXT_3128_SetParserMapping;
    pSettings->api.GetParserConfig = BMXT_3128_GetParserConfig;
    pSettings->api.SetParserConfig = BMXT_3128_SetParserConfig;
    pSettings->api.GetInputBandConfig = BMXT_3128_GetInputBandConfig;
    pSettings->api.SetInputBandConfig = BMXT_3128_SetInputBandConfig;
    pSettings->api.ReadIntStatusRegister = BMXT_3128_ReadIntStatusRegister;
    pSettings->api.TSMF_GetFldVerifyConfig = BMXT_3128_TSMF_GetFldVerifyConfig;
    pSettings->api.TSMF_SetFldVerifyConfig = BMXT_3128_TSMF_SetFldVerifyConfig;
    pSettings->api.TSMF_EnableAutoMode = BMXT_3128_TSMF_EnableAutoMode;
    pSettings->api.TSMF_EnableSemiAutoMode = BMXT_3128_TSMF_EnableSemiAutoMode;
    pSettings->api.TSMF_DisableTsmf = BMXT_3128_TSMF_DisableTsmf;
    pSettings->api.TSMF_SetParserConfig = BMXT_3128_TSMF_SetParserConfig;
    pSettings->api.GetPowerSaverSettings = BMXT_3128_GetPowerSaverSettings;
    pSettings->api.SetPowerSaverSettings = BMXT_3128_SetPowerSaverSettings;
#else
    BSTD_UNUSED(pSettings);
#endif
}

void BMXT_P_Set4538Api(BMXT_Settings *pSettings)
{
#if BMXT_4538_FAMILY
    pSettings->api.Open = BMXT_4538_Open;
    pSettings->api.Close = BMXT_4538_Close;
    pSettings->api.GetMtsifStatus = BMXT_4538_GetMtsifStatus;
    pSettings->api.GetInputBandStatus = BMXT_4538_GetInputBandStatus;
    pSettings->api.GetParserMapping = BMXT_4538_GetParserMapping;
    pSettings->api.SetParserMapping = BMXT_4538_SetParserMapping;
    pSettings->api.GetParserConfig = BMXT_4538_GetParserConfig;
    pSettings->api.SetParserConfig = BMXT_4538_SetParserConfig;
    pSettings->api.GetInputBandConfig = BMXT_4538_GetInputBandConfig;
    pSettings->api.SetInputBandConfig = BMXT_4538_SetInputBandConfig;
    pSettings->api.ReadIntStatusRegister = BMXT_4538_ReadIntStatusRegister;
    pSettings->api.TSMF_GetFldVerifyConfig = BMXT_4538_TSMF_GetFldVerifyConfig;
    pSettings->api.TSMF_SetFldVerifyConfig = BMXT_4538_TSMF_SetFldVerifyConfig;
    pSettings->api.TSMF_EnableAutoMode = BMXT_4538_TSMF_EnableAutoMode;
    pSettings->api.TSMF_EnableSemiAutoMode = BMXT_4538_TSMF_EnableSemiAutoMode;
    pSettings->api.TSMF_DisableTsmf = BMXT_4538_TSMF_DisableTsmf;
    pSettings->api.TSMF_SetParserConfig = BMXT_4538_TSMF_SetParserConfig;
    pSettings->api.GetPowerSaverSettings = BMXT_4538_GetPowerSaverSettings;
    pSettings->api.SetPowerSaverSettings = BMXT_4538_SetPowerSaverSettings;
#else
    BSTD_UNUSED(pSettings);
#endif
}

void BMXT_3128_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_3128_FAMILY
    BMXT_3128_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e3128;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set3128Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_3383_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_3128_FAMILY
    BMXT_3128_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e3383;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set3128Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_3472_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_3128_FAMILY
    BMXT_3128_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e3472;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set3128Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_4517_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_3128_FAMILY
    BMXT_3128_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e4517;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set3128Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_4528_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_3128_FAMILY
    BMXT_3128_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e4528;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set3128Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_4538_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_4538_FAMILY
    BMXT_4538_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e4538;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set4538Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_3384_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_4538_FAMILY
    BMXT_4538_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e3384;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set4538Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_7366_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_4538_FAMILY
    BMXT_4538_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e7366;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set4538Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_7145_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_4538_FAMILY
    BMXT_4538_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e7145;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set4538Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_4548_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_4538_FAMILY
    BMXT_4538_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e4548;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set4538Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_45216_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_4538_FAMILY
    BMXT_4538_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e45216;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set4538Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void BMXT_7364_GetDefaultSettings(BMXT_Settings *pSettings)
{
#if BMXT_4538_FAMILY
    BMXT_4538_P_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e7364;
    pSettings->chipRev = BMXT_ChipRev_eA0;
    BMXT_P_Set4538Api(pSettings);
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}
