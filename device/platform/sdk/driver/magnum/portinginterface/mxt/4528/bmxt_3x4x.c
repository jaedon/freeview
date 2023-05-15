/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmxt_3x4x.c $
 * $brcm_Revision: Hydra_Software_Devel/47 $
 * $brcm_Date: 1/7/13 4:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/mxt/3128/bmxt_3x4x.c $
 * 
 * Hydra_Software_Devel/47   1/7/13 4:35p jtna
 * SW7425-4359: add BMXT_Get/SetPowerSaverSettings
 * 
 * Hydra_Software_Devel/46   11/14/12 12:05p jtna
 * SW7425-1483: fix 3383 TSMF reg write
 * 
 * Hydra_Software_Devel/45   11/2/12 2:57p jtna
 * SW7346-928: default MTSIF RX polarity to 0
 * 
 * Hydra_Software_Devel/44   11/1/12 4:46p jtna
 * SW7346-928: program MTSIF TX&RX words. make use of BCHP_SET_FIELD_DATA
 * macro
 * 
 * Hydra_Software_Devel/43   10/17/12 2:19p jtna
 * SW7425-1483: fix reg offset for 3383
 * 
 * Hydra_Software_Devel/42   10/3/12 2:37p jtna
 * SW7425-4008: fix compilation error
 * 
 * Hydra_Software_Devel/41   10/3/12 11:33a jtna
 * SW7425-4008: add 4538 mxt support
 * 
 * Hydra_Software_Devel/40   9/28/12 2:45p jtna
 * SW7425-1483: refactor to use chip-specific header
 * 
 * Hydra_Software_Devel/39   9/27/12 4:58p jtna
 * SW7425-1483: get rid of function pointers
 * 
 * Hydra_Software_Devel/38   7/23/12 2:43p jtna
 * SW3472-14: extend parser mapping functionality for platforms with 16PBs
 * 
 * Hydra_Software_Devel/37   7/19/12 4:39p jtna
 * SW7425-1483: use BHAB_ReadRegister and BHAB_WriteRegister for 3128,
 * instead of BHAB_SendHabCommand
 * 
 * Hydra_Software_Devel/36   7/19/12 4:16p jtna
 * SW3472-14: add 3472 MXT support. change MTSIF in API naming to
 * MTSIF_TX, in order to differentiate with MTSIF_RX
 * 
 * Hydra_Software_Devel/35   6/28/12 6:07p jtna
 * SW7425-3341: fix 3383 B0 TSMF enable/sel
 * 
 * Hydra_Software_Devel/34   5/14/12 4:07p jtna
 * SW7425-2752: fix compiler warning
 * 
 * Hydra_Software_Devel/33   5/13/12 5:22p jtna
 * SW7425-1483: rename bchp_demod_xpt_fe.h -> bmxt_demod_xpt_fe.h
 * 
 * Hydra_Software_Devel/32   3/29/12 11:26a jtna
 * SW7425-2125: fix 4517 MTSIF config
 * 
 * Hydra_Software_Devel/31   3/6/12 6:01p jtna
 * SW7425-2574: allow BMXT to build without any RPC references
 * 
 * Hydra_Software_Devel/30   3/6/12 11:48a jtna
 * SW7425-2267: don't skip initialization during Open() for 3383
 * 
 * Hydra_Software_Devel/29   2/20/12 1:58p jtna
 * SW7425-2267: make use of register R/W RPC and enable all APIs for 3383
 * MXT
 * 
 * Hydra_Software_Devel/26   1/23/12 5:34p jtna
 * SW7425-2125: add 4517 MXT support
 * 
 * Hydra_Software_Devel/25   1/11/12 2:11p jtna
 * SW3128-12: change argument to BHAB_SendHabCommand()
 * 
 * Hydra_Software_Devel/24   1/10/12 5:50p jtna
 * SW3128-84: clear interrupt status on Open()
 * 
 * Hydra_Software_Devel/23   1/5/12 5:15p jtna
 * SW3128-84: add BMXT_ReadIntStatusRegister()
 * 
 * Hydra_Software_Devel/22   1/3/12 11:45a jtna
 * SW7425-1483: change BMXT_GetNumResources to not require BMXT handle
 * 
 * Hydra_Software_Devel/21   12/5/11 12:26p jtna
 * SW3128-34: add BMXT_MtsifConfig.TxClockPolarity
 * 
 * Hydra_Software_Devel/20   11/29/11 11:32a jtna
 * SW7425-1859: Coverity Defect ID:36138 DEADCODE
 * 
 * Hydra_Software_Devel/19   11/15/11 3:13p jtna
 * SW7425-1655: set DEMOD_XPT_FE_ATS_COUNTER_CTRL.INC_MUX_SEL and
 * .EXT_RESET_ENABLE
 * 
 * Hydra_Software_Devel/18   10/31/11 2:56p jtna
 * SW7425-1483: fix possible compiler warning
 * 
 * Hydra_Software_Devel/17   10/27/11 4:08p jtna
 * SW7425-1483: use correct regoffset for given chip
 * 
 * Hydra_Software_Devel/16   10/27/11 2:46p jtna
 * SW7425-1483: remove redundant internal variables. some more code clean-
 * up
 * 
 * Hydra_Software_Devel/15   10/26/11 6:31p jtna
 * SW7425-1483: enforce API consistency. general clean-up.
 * 
 * Hydra_Software_Devel/14   10/26/11 3:14p jtna
 * SW7425-1483: deprecate BMXT_NUM_* and use chip-specific numbers instead
 * 
 * Hydra_Software_Devel/13   10/21/11 7:15p jtna
 * SW7425-1483: convert previous implementation into 3x4x-specific
 * implementation
 * 
 * Hydra_Software_Devel/12   10/14/11 6:08p jtna
 * SW7425-1327: BMXT_GetDefaultSettings sets AcceptNulls = true, to mimic
 * what FE FW does
 * 
 * Hydra_Software_Devel/11   10/5/11 4:18p jtna
 * SW7425-1189: add debug BDBG_MSGs
 * 
 * Hydra_Software_Devel/10   10/5/11 11:46a jtna
 * SW7425-1202: fix BMXT_Close()
 * 
 * Hydra_Software_Devel/9   9/28/11 4:47p jtna
 * SW7425-1303: add 4528 support
 * 
 * Hydra_Software_Devel/8   9/28/11 3:42p jtna
 * SW7425-1365: refactor to allow support for other chips
 * 
 * Hydra_Software_Devel/7   9/13/11 12:28p jtna
 * SW7425-1238: update comment
 * 
 * Hydra_Software_Devel/6   9/12/11 4:42p jtna
 * SW7425-1238: add TSMF support via RPC
 * 
 * Hydra_Software_Devel/5   9/7/11 6:01p jtna
 * SW7425-1238: use BMXT_RegRead32/RegWrite32 for register R/W
 * 
 * Hydra_Software_Devel/4   9/7/11 5:31p jtna
 * SW7425-1238: add TSMF support
 * 
 * Hydra_Software_Devel/3   8/26/11 5:44p jtna
 * SW7425-1202: general clean-up
 *
 ***************************************************************************/

#include "bstd.h"
#include "bchp.h"
#include "bkni.h"
#include "bmxt_priv.h"
#include "bmxt.h"
#include "bmxt_demod_xpt_fe.h"
#if BMXT_3383_SUPPORT
#include "brpc_3255.h"
#endif

BDBG_MODULE(bmxt_3x4x);

#define BDBG_MSG_TRACE(x) /*BDBG_MSG(x)*/

/* Distance between Item X regs and Item X+1 */
#define PID_CHNL_STEPSIZE  (4)
#define SPID_CHNL_STEPSIZE (4)

#define PARSER_REG_STEPSIZE (BCHP_DEMOD_XPT_FE_MINI_PID_PARSER1_CTRL1 - BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1)
#define IB_REG_STEPSIZE     (BCHP_DEMOD_XPT_FE_IB1_CTRL - BCHP_DEMOD_XPT_FE_IB0_CTRL)

#ifdef BCHP_DEMOD_XPT_FE_MTSIF_TX1_CTRL1
    #define MTSIF_TX_STEPSIZE (BCHP_DEMOD_XPT_FE_MTSIF_TX1_CTRL1 - BCHP_DEMOD_XPT_FE_MTSIF_TX0_CTRL1)
#else
    #define MTSIF_TX_STEPSIZE 10 /* dummy */
#endif

#ifdef BCHP_DEMOD_XPT_FE_MTSIF_RX1_CTRL1
    #define MTSIF_RX_STEPSIZE (BCHP_DEMOD_XPT_FE_MTSIF_RX1_CTRL1 - BCHP_DEMOD_XPT_FE_MTSIF_RX0_CTRL1)
#else
    #define MTSIF_RX_STEPSIZE 10 /* dummy */
#endif

/* prevent BREG_Read32/Write32 calls */
#undef BREG_Read32
#define BREG_Read32(x,y) BDBG_CASSERT(0);
#undef BREG_Write32
#define BREG_Write32(x,y,z) BDBG_CASSERT(0);

static const unsigned BMXT_P_CHIP2NUM[] = {3128, 3383, 3472, 4517, 4528};

uint32_t BMXT_P_GetParserCtrlRegAddr(
    BMXT_Handle handle,
    unsigned parserNum,
    unsigned reg0
    )
{
    BERR_Code rc = BERR_SUCCESS; 
    BSTD_UNUSED(handle);
    BSTD_UNUSED(parserNum);
    
    if (BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1 == reg0 || BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL2 == reg0) {
        return reg0 + (parserNum* PARSER_REG_STEPSIZE);
    }
    else {
        /* Read or write of addr 0 will probably cause a segfault, but we were passed an unsupported reg0 anyway */
        BDBG_ERR(("Parser reg %lu is unsupported", reg0));
    }
  
    return rc;
}

#define BMXT_STD_REGOFFSET_FE 0x00105000 /* base RDB is 3472 A0 */

static uint32_t BMXT_RegRead32(BMXT_Handle handle, uint32_t reg)
{
    BERR_Code rc;
    BMXT_Chip chip = handle->settings.chip;
    BHAB_Handle hab = handle->hHab;

    reg -= handle->regOffsetFe;

    if (chip==BMXT_Chip_e3128 || chip==BMXT_Chip_e4528 || chip==BMXT_Chip_e4517 || chip==BMXT_Chip_e3472 || chip==BMXT_Chip_e4538) {
#if BMXT_3128_SUPPORT || BMXT_4528_SUPPORT || BMXT_4517_SUPPORT || BMXT_3472_SUPPORT || BMXT_4538_SUPPORT
        uint32_t val;
        rc = BHAB_ReadRegister(hab, reg, &val);
        if (rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
            return 0;
        }
        return val;
#endif
    }
    else if (chip==BMXT_Chip_e3383) {
#if BMXT_3383_SUPPORT
        BRPC_Param_ECM_ReadXptBlock block;
        BERR_Code rcR = BERR_SUCCESS;
        BSTD_UNUSED(hab);
        block.startRegisterAddress = reg; /* addresses are offset-based and inclusive */
        block.endRegisterAddress = reg;
        rc = BRPC_CallProc(handle->hRpc, BRPC_ProcId_ECM_ReadXptBlock, 
            (const uint32_t *)&block, sizeof(block)/sizeof(uint32_t), 
            (uint32_t *)&block, sizeof(block)/sizeof(uint32_t), &rcR);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); return 0; }
        if (rcR!=BERR_SUCCESS) { rc = BERR_TRACE(rcR); return 0; } /* propagate the failure */
        return block.returnRegisterValues[0];
#endif
    }
    else {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return 0;
    }

    return 0;
}

static void BMXT_RegWrite32(BMXT_Handle handle, uint32_t reg, uint32_t data)
{
    BERR_Code rc;
    BMXT_Chip chip = handle->settings.chip;
    BHAB_Handle hab = handle->hHab;

    reg -= handle->regOffsetFe;

    if (chip==BMXT_Chip_e3128 || chip==BMXT_Chip_e4528 || chip==BMXT_Chip_e4517 || chip==BMXT_Chip_e3472 || chip==BMXT_Chip_e4538) {
#if BMXT_3128_SUPPORT || BMXT_4528_SUPPORT || BMXT_4517_SUPPORT || BMXT_3472_SUPPORT || BMXT_4538_SUPPORT
        rc = BHAB_WriteRegister(hab, reg, &data);
        if (rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
        }
#endif
    }
    else if (chip==BMXT_Chip_e3383) {
#if BMXT_3383_SUPPORT
        BRPC_Param_ECM_WriteXptBlock block;
        BERR_Code rcR = BERR_SUCCESS;
        BSTD_UNUSED(hab);
        block.startRegisterAddress = reg; /* addresses are offset-based and inclusive */
        block.endRegisterAddress = reg;
        block.writeRegisterValues[0] = data;
        rc = BRPC_CallProc(handle->hRpc, BRPC_ProcId_ECM_WriteXptBlock, 
            (const uint32_t *)&block, sizeof(block)/sizeof(uint32_t), 
            NULL, 0, &rcR);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
        if (rcR!=BERR_SUCCESS) { rc = BERR_TRACE(rcR); } /* propagate the failure */
#endif
    }
    else {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    return; 
}

BERR_Code BMXT_GetDefaultSettings(
    BMXT_Settings *pSettings
    )
{
    unsigned i;

    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    for (i=0; i<BMXT_MAX_NUM_MTSIF_TX; i++) {
        pSettings->MtsifTxCfg[i].TxInterfaceWidth = 8; /* 8 bits wide */
        pSettings->MtsifTxCfg[i].TxClockPolarity = 1;
        pSettings->MtsifTxCfg[i].Enable = false;
    }

    for (i=0; i<BMXT_MAX_NUM_MTSIF_RX; i++) {
        pSettings->MtsifRxCfg[i].RxInterfaceWidth = 8;
        pSettings->MtsifRxCfg[i].RxClockPolarity = 0;
        pSettings->MtsifRxCfg[i].Enable = false;
    }
    
    for (i=0; i<BMXT_MAX_NUM_PARSERS; i++) {
        pSettings->ParserCfg[i].ErrorInputIgnore = false;
        pSettings->ParserCfg[i].TsMode = BMXT_ParserTimestampMode_eMod300; 
        pSettings->ParserCfg[i].AcceptNulls = true;
        pSettings->ParserCfg[i].AllPass = true;
        pSettings->ParserCfg[i].DssMode = false;
        pSettings->ParserCfg[i].InputBandNumber = i; /* Default to 1-to-1 mapping */
        pSettings->ParserCfg[i].Enable = true;
    }

    for (i=0; i<BMXT_MAX_NUM_INPUT_BANDS; i++) {
        pSettings->InputCfg[i].DssMode = false;
    }

    /* Default virtual parser to equal the underlying physical parser */
    for (i=0; i<BMXT_MAX_NUM_PARSERS; i++) {
        pSettings->ParserMapping.FrontEnd[i].VirtualParserBandNum = i;
        pSettings->ParserMapping.FrontEnd[i].VirtualParserIsPlayback = false;
    }

    return BERR_SUCCESS;
}

BERR_Code BMXT_Open(
    BMXT_Handle *pHandle,
    BCHP_Handle hChp,
    BREG_Handle hReg,
    const BMXT_Settings *pSettings
    )
{
    BMXT_Handle mxt = NULL;
    BERR_Code rc = BERR_SUCCESS;
    uint32_t reg;
    unsigned i;

    BDBG_ENTER(BMXT_Open);
    BDBG_MSG_TRACE(("BMXT_Open(%u,%s)", BMXT_P_CHIP2NUM[pSettings->chip], pSettings->hHab?"hab":"rpc"));

    BDBG_ASSERT(pHandle);
    BDBG_ASSERT(pSettings);

    if (pSettings->hHab==NULL && pSettings->hRpc==NULL) {
        BDBG_ERR(("One of BMXT_DefaultSettings.hHab or .hRpc must be set"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto done;
    }
    if (pSettings->hHab && pSettings->hRpc) {
        BDBG_ERR(("Only one of BMXT_DefaultSettings.hHab or .hRpc must be set"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto done;
    }

    /* Alloc memory from the system heap. */
    mxt = BKNI_Malloc(sizeof(BMXT_P_TransportData));
    if (mxt==NULL) {
        BDBG_ERR(("BKNI_Malloc() failed!"));
        mxt = NULL;
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto done;
    }
        
    BKNI_Memset((void *)mxt, 0, sizeof(BMXT_P_TransportData));
    mxt->hChp = hChp;
    mxt->hReg = hReg;
    mxt->hHab = pSettings->hHab;
    mxt->hRpc = pSettings->hRpc;
    mxt->settings = *pSettings;

    mxt->regOffsetFe = BMXT_P_GetRegOffsetFe(mxt);
    mxt->num.mtsifTx = BMXT_GetNumResources(mxt->settings.chip, mxt->settings.chipRev, BMXT_ResourceType_eMtsifTx);
    mxt->num.mtsifRx = BMXT_GetNumResources(mxt->settings.chip, mxt->settings.chipRev, BMXT_ResourceType_eMtsifRx);
    mxt->num.ib = BMXT_GetNumResources(mxt->settings.chip, mxt->settings.chipRev, BMXT_ResourceType_eInputBand);
    mxt->num.pb = BMXT_GetNumResources(mxt->settings.chip, mxt->settings.chipRev, BMXT_ResourceType_eParser);
    mxt->num.tsmf = BMXT_GetNumResources(mxt->settings.chip, mxt->settings.chipRev, BMXT_ResourceType_eTsmf);
    BDBG_ASSERT(mxt->num.mtsifTx <= BMXT_MAX_NUM_MTSIF_TX);
    BDBG_ASSERT(mxt->num.mtsifRx <= BMXT_MAX_NUM_MTSIF_RX);
    BDBG_ASSERT(mxt->num.ib <= BMXT_MAX_NUM_INPUT_BANDS);
    BDBG_ASSERT(mxt->num.pb <= BMXT_MAX_NUM_PARSERS);
    BDBG_ASSERT(mxt->num.tsmf <= BMXT_MAX_NUM_TSMF);

    mxt->num.remapPb = 9; /* all chips have at least 9 PBs, all of which are remappable */
    if (mxt->num.pb==16) {
        mxt->num.remapPb = 12;
    }

    /* SW7425-1655 */
    reg = BMXT_RegRead32(mxt, BCHP_DEMOD_XPT_FE_ATS_COUNTER_CTRL);
    reg |= (
        BCHP_FIELD_DATA(DEMOD_XPT_FE_ATS_COUNTER_CTRL, INC_MUX_SEL, 1) |
        BCHP_FIELD_DATA(DEMOD_XPT_FE_ATS_COUNTER_CTRL, EXT_RESET_ENABLE, 1)
        );
    BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_ATS_COUNTER_CTRL, reg);

    /* clear interrupt status regs */
    BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_INTR_STATUS0_REG, 0);
    BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_INTR_STATUS1_REG, 0);
    
    for (i=0; i<mxt->num.mtsifTx; i++) {
        uint32_t Reg, RegAddr;
        unsigned IfWidth = 3; /* 8 bit wide */

        if (pSettings->MtsifTxCfg[i].TxInterfaceWidth != 8) {
            BDBG_ERR(("Support for other than 8-bit wide MTSIF needs to be added"));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            BKNI_Free(mxt);
            mxt = NULL;
            goto done;
        }

        RegAddr = BCHP_DEMOD_XPT_FE_MTSIF_TX0_CTRL1 + (i*MTSIF_TX_STEPSIZE);
        Reg = BMXT_RegRead32(mxt, RegAddr);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, MTSIF_TX_IF_WIDTH, IfWidth);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, MTSIF_TX_CLOCK_POL_SEL, pSettings->MtsifTxCfg[i].TxClockPolarity);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, TX_ENABLE, pSettings->MtsifTxCfg[i].Enable==true ? 1 : 0);
        BMXT_RegWrite32(mxt, RegAddr, Reg);

        RegAddr = BCHP_DEMOD_XPT_FE_MTSIF_TX0_SECRET_WORD + (i*MTSIF_TX_STEPSIZE);
        BMXT_RegWrite32(mxt, RegAddr, 0x829eecde);
    }

#ifdef BCHP_DEMOD_XPT_FE_MTSIF_RX0_CTRL1
    for (i=0; i<mxt->num.mtsifRx; i++) {
        uint32_t Reg, RegAddr;
        unsigned IfWidth = 3; /* 8 bit wide */

        if (pSettings->MtsifRxCfg[i].RxInterfaceWidth != 8) {
            BDBG_ERR(("Support for other than 8-bit wide MTSIF needs to be added"));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            BKNI_Free(mxt);
            mxt = NULL;
            goto done;
        }

        RegAddr = BCHP_DEMOD_XPT_FE_MTSIF_RX0_CTRL1 + (i*MTSIF_RX_STEPSIZE);
        Reg = BMXT_RegRead32(mxt, RegAddr);       
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_IF_WIDTH, IfWidth);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_CLOCK_POL_SEL, pSettings->MtsifRxCfg[i].RxClockPolarity);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, PARSER_ENABLE, pSettings->MtsifRxCfg[i].Enable==true ? 1 : 0);
        BMXT_RegWrite32(mxt, RegAddr, Reg);

        RegAddr = BCHP_DEMOD_XPT_FE_MTSIF_RX0_SECRET_WORD + (i*MTSIF_RX_STEPSIZE);
        BMXT_RegWrite32(mxt, RegAddr, 0x829eecde);
    }
#endif
    
    for (i=0; i<mxt->num.tsmf; i++) {
        BMXT_TSMFFldVerifyConfig tsmfConfig;
        BKNI_Memset(&tsmfConfig, 0, sizeof(tsmfConfig));
        BMXT_TSMF_SetFldVerifyConfig(mxt, i, &tsmfConfig);
        BMXT_TSMF_EnableAutoMode(mxt, 0, i, 0);
        BMXT_TSMF_EnableSemiAutoMode(mxt, 0, i, 0, 0, 0);
        BMXT_TSMF_DisableTsmf(mxt, i);
    }

    /* unlike BE XPT, for FE XPT, the FE FW programs some of our registers. 
       honor that programming and don't override it */
#if 0
    for (i=0; i<256; i++) {
        uint32_t Reg;
        /* bit 28: IGNORE_PID_VERSION. bit 22: PARSER_OUTPUT_PIPE_SEL. 
           On BE XPT, bit 22 defaults to 0, but on FE XPT, it defaults to 1. 
           we must keep the default */
        Reg = (1 << 28) | (1 << 22);
        BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_PID_TABLE_i_ARRAY_BASE + (4*i), Reg);
        Reg = 0;
        BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_SPID_TABLE_i_ARRAY_BASE + (4*i), Reg);
    }
#endif

    BMXT_SetParserMapping(mxt, &pSettings->ParserMapping);

    for (i=0; i<mxt->num.pb; i++) {
        BMXT_SetParserConfig(mxt, i, pSettings->ParserCfg + i);
        BMXT_TSMF_SetParserConfig(mxt, i, 0, false);
    }

    for (i=0; i<mxt->num.ib; i++) {
        BMXT_SetInputBandConfig(mxt, i, pSettings->InputCfg + i);
    }

done:
    *pHandle = mxt;
    BDBG_LEAVE(BMXT_Open);
  
    return rc;
}

void BMXT_Close(
    BMXT_Handle handle
    )
{
    unsigned int Index;
    BDBG_ASSERT(handle);

    for (Index=0; Index<handle->num.pb; Index++) {
        handle->settings.ParserCfg[Index].Enable = false;
        BMXT_SetParserConfig(handle, Index, handle->settings.ParserCfg + Index);
    }

    /* dont need the mxt handle any more */
    BKNI_Free(handle);
}

BERR_Code BMXT_GetParserConfig(
    BMXT_Handle handle,
    unsigned parserNum,
    BMXT_ParserConfig *pConfig
    )
{
    uint32_t Reg, RegAddr;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ASSERT(handle);
    BDBG_ASSERT(pConfig);

    if (parserNum >= handle->num.pb) {
        BDBG_ERR(("ParserNum %lu is out of range!", parserNum));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else {
        RegAddr = BMXT_P_GetParserCtrlRegAddr(handle, parserNum, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1);
        Reg = BMXT_RegRead32(handle, RegAddr);

        pConfig->ErrorInputIgnore = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE);
        pConfig->TsMode = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE);
        pConfig->AcceptNulls = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD);
        pConfig->AllPass = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD);
        pConfig->DssMode = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PACKET_TYPE) ? true : false;
        pConfig->InputBandNumber = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL);
        pConfig->Enable = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ENABLE);
    }

    return rc;
}

BERR_Code BMXT_SetParserConfig(
    BMXT_Handle handle,
    unsigned parserNum,
    const BMXT_ParserConfig *pConfig
    )
{
    uint32_t Reg, RegAddr;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ASSERT(handle);
    BDBG_ASSERT(pConfig);
    BDBG_MSG_TRACE(("BMXT_SetParserConfig(%u,%s)", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc"));

    if (parserNum >= handle->num.pb) {
        BDBG_ERR(("ParserNum %lu is out of range!", parserNum));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else {
        handle->settings.ParserCfg[parserNum]  = *pConfig;
        
        /* The parser config registers are at consecutive addresses. */
        RegAddr = BMXT_P_GetParserCtrlRegAddr(handle, parserNum, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1);
        Reg = BMXT_RegRead32(handle, RegAddr);
        
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE, pConfig->ErrorInputIgnore);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE, pConfig->TsMode);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD, pConfig->AcceptNulls);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD, pConfig->AllPass);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PKT_LENGTH, pConfig->DssMode ? 130 : 188);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PACKET_TYPE, pConfig->DssMode ? 1 : 0);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL, pConfig->InputBandNumber);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ENABLE, pConfig->Enable ? 1 : 0);

        BMXT_RegWrite32(handle, RegAddr, Reg);
    }

    return rc;
}

BERR_Code BMXT_GetParserMapping(
    BMXT_Handle handle,
    BMXT_ParserBandMapping *pMapping
    )
{
    uint32_t Reg;
    unsigned i;
    BDBG_ASSERT(handle);
    BDBG_ASSERT(pMapping);

    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID);
    pMapping->FrontEnd[0].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID);
    pMapping->FrontEnd[0].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_PARSER_SEL) ? true : false;
    pMapping->FrontEnd[1].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID);
    pMapping->FrontEnd[1].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_PARSER_SEL) ? true : false;
    pMapping->FrontEnd[2].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID);
    pMapping->FrontEnd[2].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_PARSER_SEL) ? true : false;
    pMapping->FrontEnd[3].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID);
    pMapping->FrontEnd[3].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_PARSER_SEL) ? true : false;

    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID);
    pMapping->FrontEnd[4].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_BAND_ID);
    pMapping->FrontEnd[4].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_PARSER_SEL) ? true : false;
    pMapping->FrontEnd[5].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_BAND_ID);
    pMapping->FrontEnd[5].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_PARSER_SEL) ? true : false;
    pMapping->FrontEnd[6].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_BAND_ID);
    pMapping->FrontEnd[6].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_PARSER_SEL) ? true : false;
    pMapping->FrontEnd[7].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_BAND_ID);
    pMapping->FrontEnd[7].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_PARSER_SEL) ? true : false;

    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID);
    pMapping->FrontEnd[8].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_BAND_ID);
    pMapping->FrontEnd[8].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_PARSER_SEL) ? true : false;

    /* populate with dummy values. we'll do checking during BMXT_SetParserMapping */
    for (i=9; i<BMXT_MAX_NUM_PARSERS; i++) {
        pMapping->FrontEnd[i].VirtualParserBandNum = i;
        pMapping->FrontEnd[i].VirtualParserIsPlayback = false;
    }

    /* if the HW actually exists, populate with real values */
    if (handle->num.remapPb==12) {
#ifdef BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID_PARSER9_BAND_ID_MASK
        pMapping->FrontEnd[9].VirtualParserBandNum =     BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_BAND_ID);
        pMapping->FrontEnd[9].VirtualParserIsPlayback =  BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_PARSER_SEL) ? true : false;
#endif
#ifdef BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID_PARSER10_BAND_ID_MASK
        pMapping->FrontEnd[10].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_BAND_ID);
        pMapping->FrontEnd[10].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_PARSER_SEL) ? true : false;
#endif
#ifdef BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID_PARSER11_BAND_ID_MASK
        pMapping->FrontEnd[11].VirtualParserBandNum =    BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_BAND_ID);
        pMapping->FrontEnd[11].VirtualParserIsPlayback = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_PARSER_SEL) ? true : false;
#endif
    }
    return BERR_SUCCESS;
}

#define BMXT_NUM_REMAPPABLE_FE_PARSERS (9)

BERR_Code BMXT_SetParserMapping(
    BMXT_Handle handle, 
    const BMXT_ParserBandMapping *pMapping
    )
{
    uint32_t Reg;
    BERR_Code rc = BERR_SUCCESS;
    unsigned i;
 
    BDBG_ASSERT(handle);
    BDBG_ASSERT(pMapping);
    BDBG_MSG_TRACE(("BMXT_SetParserMapping(%u,%s)", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc"));

    /* Only certain parsers can be remapped. Check for attempted remapping of parsers that don't support it. */
    for (i=handle->num.remapPb; i<handle->num.pb; i++) {
        const BMXT_BandMap *Mapping = &pMapping->FrontEnd[i];
        
        if (Mapping->VirtualParserBandNum!=i || Mapping->VirtualParserIsPlayback) {
            BDBG_WRN(("PB%u cannot be remapped to %u", i, Mapping->VirtualParserBandNum));
        }
        /* warn, but keep going */
    }

    handle->settings.ParserMapping = *pMapping;

    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID, pMapping->FrontEnd[0].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_PARSER_SEL, pMapping->FrontEnd[0].VirtualParserIsPlayback ? 1 : 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID, pMapping->FrontEnd[1].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_PARSER_SEL, pMapping->FrontEnd[1].VirtualParserIsPlayback ? 1 : 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID, pMapping->FrontEnd[2].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_PARSER_SEL, pMapping->FrontEnd[2].VirtualParserIsPlayback ? 1 : 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID, pMapping->FrontEnd[3].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_PARSER_SEL, pMapping->FrontEnd[3].VirtualParserIsPlayback ? 1 : 0);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, Reg);

    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_BAND_ID, pMapping->FrontEnd[4].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_PARSER_SEL, pMapping->FrontEnd[4].VirtualParserIsPlayback ? 1 : 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_BAND_ID, pMapping->FrontEnd[5].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_PARSER_SEL, pMapping->FrontEnd[5].VirtualParserIsPlayback ? 1 : 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_BAND_ID, pMapping->FrontEnd[6].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_PARSER_SEL, pMapping->FrontEnd[6].VirtualParserIsPlayback ? 1 : 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_BAND_ID, pMapping->FrontEnd[7].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_PARSER_SEL, pMapping->FrontEnd[7].VirtualParserIsPlayback ? 1 : 0);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, Reg);

    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_BAND_ID, pMapping->FrontEnd[8].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_PARSER_SEL, pMapping->FrontEnd[8].VirtualParserIsPlayback ? 1 : 0);
#ifdef BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID_PARSER9_BAND_ID_MASK
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_BAND_ID, pMapping->FrontEnd[9].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_PARSER_SEL, pMapping->FrontEnd[9].VirtualParserIsPlayback ? 1 : 0);
#endif
#ifdef BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID_PARSER10_BAND_ID_MASK
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_BAND_ID, pMapping->FrontEnd[10].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_PARSER_SEL, pMapping->FrontEnd[10].VirtualParserIsPlayback ? 1 : 0);
#endif
#ifdef BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID_PARSER11_BAND_ID_MASK
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_BAND_ID, pMapping->FrontEnd[11].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_PARSER_SEL, pMapping->FrontEnd[11].VirtualParserIsPlayback ? 1 : 0);
#endif
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, Reg);

    return rc;
}

BERR_Code BMXT_GetInputBandConfig(
    BMXT_Handle handle,
    unsigned bandNum,
    BMXT_InputBandConfig *pConfig
    )
{
    uint32_t Reg;
    uint32_t RegAddr;

    BERR_Code rc = BERR_SUCCESS;

    BDBG_ASSERT(pConfig);

    if (bandNum>=handle->num.ib) {
        BDBG_ERR(("BandNum %lu is out of range!", bandNum));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else {
        /* The parser config registers are at consecutive addresses. */
        RegAddr = BCHP_DEMOD_XPT_FE_IB0_CTRL + (bandNum * IB_REG_STEPSIZE);
        Reg = BMXT_RegRead32(handle, RegAddr);
        
        pConfig->DssMode = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_IB0_CTRL, IB_PKT_LENGTH) == 130 ? true : false;
    }

    return rc;
}

BERR_Code BMXT_SetInputBandConfig(
    BMXT_Handle handle,
    unsigned bandNum,
    const BMXT_InputBandConfig *pConfig
    )
{
    uint32_t Reg, RegAddr;
    BERR_Code rc = BERR_SUCCESS;
    BDBG_ASSERT(pConfig);
    BDBG_MSG_TRACE(("BMXT_SetInputBandConfig(%u,%s)", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc"));

    if (bandNum>=handle->num.ib) {
        BDBG_ERR(("BandNum %lu is out of range!", bandNum));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else {
        handle->settings.InputCfg[bandNum]  = *pConfig;
        
        /* The parser config registers are at consecutive addresses. */
        RegAddr = BCHP_DEMOD_XPT_FE_IB0_CTRL + (bandNum * IB_REG_STEPSIZE);
        Reg = BMXT_RegRead32(handle, RegAddr);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_IB0_CTRL, IB_PKT_LENGTH, pConfig->DssMode ? 130 : 188);
        BMXT_RegWrite32(handle, RegAddr, Reg);
    }

    return rc;
}

uint32_t BMXT_ReadIntStatusRegister(
    BMXT_Handle handle, 
    BMXT_IntReg intReg
    )
{
    uint32_t reg = 0, val = 0;

    switch (intReg) {
        case BMXT_IntReg_eFE_INTR_STATUS0:
            reg = BCHP_DEMOD_XPT_FE_INTR_STATUS0_REG;
            break;
        case BMXT_IntReg_eFE_INTR_STATUS1:
            reg = BCHP_DEMOD_XPT_FE_INTR_STATUS1_REG;
            break;
        default:
            return BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    val = BMXT_RegRead32(handle, reg);
    BMXT_RegWrite32(handle, reg, 0);
    return val;
}

/***************************************************************************
TSMF APIs
****************************************************************************/
#define BMXT_TSMF_P_REG_OFFSET (BCHP_DEMOD_XPT_FE_TSMF1_CTRL - BCHP_DEMOD_XPT_FE_TSMF0_CTRL)

static BERR_Code BMXT_TSMF_P_GetOffset(
    unsigned tsmfNum,
    uint32_t *offset
    )
{
    BERR_Code ret = BERR_SUCCESS;

    if (tsmfNum >= BMXT_MAX_NUM_TSMF) {
        BDBG_ERR(("Exceed Maxium number of TSMF supported"));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    *offset = BMXT_TSMF_P_REG_OFFSET * tsmfNum;
    return ret;
}

BERR_Code BMXT_TSMF_GetFldVerifyConfig(
    BMXT_Handle handle,
    unsigned tsmfNum,
    BMXT_TSMFFldVerifyConfig *pConfig
    )
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ENTER(BMXT_TSMF_GetFldVerifyConfig);
    BDBG_MSG_TRACE(("BMXT_TSMF_GetFldVerifyConfig(%u,%s)", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc"));
    
    if (tsmfNum>=handle->num.tsmf) {
        BDBG_ERR(("TSMFNum %lu is out of range!", tsmfNum));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    rc = BMXT_TSMF_P_GetOffset(tsmfNum, &RegOffset);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return rc;
    }
    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset);
    pConfig->CrcChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_CRC_CK_DIS);
    pConfig->RelTsStatusChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_REL_TS_STATUS_CK_DIS);
    pConfig->FrameTypeChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_FRAME_TYPE_CK_DIS);
    pConfig->RelTsModeChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_REL_TS_MODE_CK_DIS);
    pConfig->SyncChkFDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_SYNC_CK_DIS);
    pConfig->CCCkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_CC_CK_DIS);
    pConfig->AdapChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_ADAP_CK_DIS);
    pConfig->ScChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_SC_CK_DIS);
    pConfig->TsPriorChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_TS_PRIOR_CK_DIS);
    pConfig->PusiChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_PUSI_CK_DIS);
    pConfig->TeiChkDis = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_TEI_CK_DIS);
    pConfig->VersionChgMode = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_VER_CHANGE_MODE);

    BDBG_LEAVE(BMXT_TSMF_GetFldVerifyConfig);
    return rc;
}

BERR_Code BMXT_TSMF_SetFldVerifyConfig(
    BMXT_Handle handle,
    unsigned tsmfNum,
    const BMXT_TSMFFldVerifyConfig *pConfig
    )
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ENTER(BMXT_TSMF_SetFldVerifyConfig);
    BDBG_MSG_TRACE(("BMXT_TSMF_SetFldVerifyConfig(%u,%s)", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc"));

    if (tsmfNum>=handle->num.tsmf) {
        BDBG_ERR(("TSMFNum %lu is out of range!", tsmfNum));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    
    rc = BMXT_TSMF_P_GetOffset(tsmfNum, &RegOffset);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return rc;
    }
    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_CRC_CK_DIS, pConfig->CrcChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_REL_TS_STATUS_CK_DIS, pConfig->RelTsStatusChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_FRAME_TYPE_CK_DIS, pConfig->FrameTypeChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_REL_TS_MODE_CK_DIS, pConfig->RelTsModeChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_SYNC_CK_DIS, pConfig->SyncChkFDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_CC_CK_DIS, pConfig->CCCkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_ADAP_CK_DIS, pConfig->AdapChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_SC_CK_DIS, pConfig->ScChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_TS_PRIOR_CK_DIS, pConfig->TsPriorChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_PUSI_CK_DIS, pConfig->PusiChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_TEI_CK_DIS, pConfig->TeiChkDis);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_VER_CHANGE_MODE, pConfig->VersionChgMode);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset, Reg);

    BDBG_LEAVE(BMXT_TSMF_SetFldVerifyConfig);

    return rc;
}

BERR_Code BMXT_TSMF_EnableAutoMode( 
    BMXT_Handle handle,
    BMXT_TSMFInputSel input,
    unsigned tsmfNum,
    uint8_t relativeTsNum
    )
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ENTER(BMXT_TSMF_EnableAutoMode);
    BDBG_MSG_TRACE(("BXPT_TSMF_EnableAutoMode(%u,%s): %u %u %u", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc", input, tsmfNum, relativeTsNum));
            
    rc = BMXT_TSMF_P_GetOffset(tsmfNum, &RegOffset);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return rc;
    }
    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_REL_TS_NO, relativeTsNum);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_INPUT_SEL, input);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_AUTO_EN, 1);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_ENABLE, 1);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset, Reg);

    BDBG_LEAVE(BMXT_TSMF_EnableAutoMode);
    return rc;
}

BERR_Code BMXT_TSMF_EnableSemiAutoMode(
    BMXT_Handle handle,
    BMXT_TSMFInputSel input,
    unsigned tsmfNum,
    uint32_t slotMapLo,
    uint32_t slotMapHi,
    uint8_t relativeTsNum
    )
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ENTER(BMXT_TSMF_EnableSemiAutoMode);
    BDBG_MSG_TRACE(("BXPT_TSMF_EnableSemiAutoMode(%u,%s)", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc", input, tsmfNum, slotMapLo, slotMapHi, relativeTsNum));
    
    rc = BMXT_TSMF_P_GetOffset(tsmfNum, &RegOffset);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return rc;
    }

    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_TSMF0_SLOT_MAP_LO + RegOffset, slotMapLo);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_TSMF0_SLOT_MAP_HI + RegOffset, slotMapHi);

    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_INPUT_SEL, input);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_AUTO_EN, 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_ENABLE, 1);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_REL_TS_NO, relativeTsNum);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset, Reg);

    BDBG_LEAVE(BMXT_TSMF_EnableSemiAutoMode);
    return rc;
}

BERR_Code BMXT_TSMF_DisableTsmf( 
    BMXT_Handle handle,
    unsigned tsmfNum
    )
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ENTER(BMXT_TSMF_DisableTsmf);
    BDBG_MSG_TRACE(("BMXT_TSMF_DisableTsmf(%u,%s)", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc", tsmfNum));

    rc = BMXT_TSMF_P_GetOffset(tsmfNum, &RegOffset);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return rc;
    }
    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_ENABLE, 0);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset, Reg);

    BDBG_LEAVE(BMXT_TSMF_DisableTsmf);
    return rc;
}

BERR_Code BMXT_TSMF_SetParserConfig( 
    BMXT_Handle handle,
    unsigned parserNum,
    unsigned tsmfNum,
    bool enable
    )
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ENTER(BMXT_TSMF_SetParserConfig);
    BDBG_MSG_TRACE(("BMXT_TSMF_SetParserConfig(%u,%s)", BMXT_P_CHIP2NUM[handle->settings.chip], handle->hHab?"hab":"rpc", parserNum, tsmfNum, enable));
    
    if ((parserNum>=handle->num.pb) || (tsmfNum>=handle->num.tsmf)) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return rc;
    }

    RegOffset =(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER1_CTRL1 - BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1) * parserNum;
    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1 + RegOffset);


    /* if NUM_TSMF=2, TSMF_EN=25 and TSMF_SEL=26 */
    /* if NUM_TSMF>2, TSMF_EN=23 and TSMF_SEL=24~26 */
    if (handle->settings.chip!=BMXT_Chip_e3383) {
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TSMF_SEL, tsmfNum);
        BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TSMF_EN, enable);
    }
    else {
        unsigned TSMF_SEL_MASK, TSMF_SEL_SHIFT, TSMF_EN_MASK, TSMF_EN_SHIFT;
        if (handle->num.tsmf>2) {
            TSMF_SEL_MASK = 0x07000000;
            TSMF_SEL_SHIFT = 24;
            TSMF_EN_MASK = 0x00800000;
            TSMF_EN_SHIFT = 23;
        }
        else {
            TSMF_SEL_MASK = 0x04000000;
            TSMF_SEL_SHIFT = 26;
            TSMF_EN_MASK = 0x02000000;
            TSMF_EN_SHIFT =  25;
        }
        Reg &= ~( TSMF_SEL_MASK | TSMF_EN_MASK );
        Reg |=(
            ((tsmfNum << TSMF_SEL_SHIFT) & TSMF_SEL_MASK) |
            ((enable << TSMF_EN_SHIFT) & TSMF_EN_MASK));
    }

    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1 + RegOffset, Reg);
    BDBG_LEAVE(BMXT_TSMF_SetParserConfig);
    return rc;
}

void BMXT_GetPowerSaverSettings(BMXT_Handle handle, BMXT_PowerSaverSettings *pSettings)
{
    BDBG_ASSERT(handle);
    BDBG_ASSERT(pSettings);
    *pSettings = handle->powerSettings;
}

BERR_Code BMXT_SetPowerSaverSettings(BMXT_Handle handle, const BMXT_PowerSaverSettings *pSettings)
{
    BMXT_Chip chip;

    BDBG_ASSERT(handle);
    BDBG_ASSERT(pSettings);
    chip = handle->settings.chip;

    if (chip == BMXT_Chip_e4538) {
        BERR_Code rc;
        uint8_t buf[4];
        buf[0] = 0x4E;
        buf[1] = pSettings->disableMtsif ? 1 : 0;
        rc = BHAB_SendHabCommand(handle->hHab, buf, 3, buf, 3, true, true, 3);
        if (rc!=BERR_SUCCESS) {
            return BERR_TRACE(rc);
        }        
    }
    
    handle->powerSettings = *pSettings;
    return BERR_SUCCESS;
}

