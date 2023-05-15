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

#define PB_REG_STEPSIZE (BCHP_DEMOD_XPT_FE_MINI_PID_PARSER1_CTRL1 - BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1)
#define IB_REG_STEPSIZE (BCHP_DEMOD_XPT_FE_IB1_CTRL - BCHP_DEMOD_XPT_FE_IB0_CTRL)

#ifdef BCHP_DEMOD_XPT_FE_MTSIF_TX1_CTRL1
#define MTSIF_TX_STEPSIZE (BCHP_DEMOD_XPT_FE_MTSIF_TX1_CTRL1 - BCHP_DEMOD_XPT_FE_MTSIF_TX0_CTRL1)
#else
#define MTSIF_TX_STEPSIZE 0
#endif

#ifdef BCHP_DEMOD_XPT_FE_MTSIF_RX1_CTRL1
#define MTSIF_RX_STEPSIZE (BCHP_DEMOD_XPT_FE_MTSIF_RX1_CTRL1 - BCHP_DEMOD_XPT_FE_MTSIF_RX0_CTRL1)
#else
#define MTSIF_RX_STEPSIZE 0
#endif

#define PARSER_OUTPUT_PIPE_SEL_TO_INDEX(mxt, pops)    (BMXT_IS_3128_FAMILY(mxt->settings.chip) ? pops-1 : pops)
#define PARSER_OUTPUT_PIPE_SEL_FROM_INDEX(mxt, index) (BMXT_IS_3128_FAMILY(mxt->settings.chip) ? index+1 : index)

#define VIRTUAL_HANDLE_REG_OFFSET 0x80000000 /* hard-coded for now */
static uint32_t BMXT_RegRead32(BMXT_Handle handle, uint32_t reg)
{
    BERR_Code rc;
    BMXT_Chip chip = handle->settings.chip;

    /* if virtual handle, then use master device's chip to determine reg R/W method */
    if (handle->settings.hVirtual) {
        handle = handle->settings.hVirtual;
        chip = handle->settings.chip;
        reg += VIRTUAL_HANDLE_REG_OFFSET;
    }
    BDBG_ASSERT(chip<=BMXT_Chip_eMax);

    if (!BMXT_IS_REG_BASED(chip)) {
        reg = reg - BCHP_DEMOD_XPT_FE_FE_CTRL + handle->regOffsetFe;
    }

    if (BMXT_IS_HAB_BASED(chip)) {
#if BMXT_HAB_SUPPORT
        BHAB_Handle hab = handle->hHab;
        uint32_t val;
        rc = BHAB_ReadRegister(hab, reg, &val);
        if (rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
            return 0;
        }
        return val;
#endif
    }
    else if (BMXT_IS_RPC_BASED(chip)) {
#if BMXT_RPC_SUPPORT
        BRPC_Param_ECM_ReadXptBlock block;
        BERR_Code rcR = BERR_SUCCESS;
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
#if BMXT_REG_SUPPORT
        BREG_Handle hReg = handle->hReg;
        BSTD_UNUSED(rc);
        return BREG_Read32(hReg, reg);
#endif
    }

    return 0;
}

static void BMXT_RegWrite32(BMXT_Handle handle, uint32_t reg, uint32_t data)
{
    BERR_Code rc;
    BMXT_Chip chip = handle->settings.chip;

    /* if virtual handle, then use master device's chip to determine reg R/W method */
    if (handle->settings.hVirtual) {
        handle = handle->settings.hVirtual;
        chip = handle->settings.chip;
        reg += VIRTUAL_HANDLE_REG_OFFSET;
    }
    BDBG_ASSERT(chip<=BMXT_Chip_eMax);

    if (!BMXT_IS_REG_BASED(chip)) {
        reg = reg - BCHP_DEMOD_XPT_FE_FE_CTRL + handle->regOffsetFe;
    }

    if (BMXT_IS_HAB_BASED(chip)) {
#if BMXT_HAB_SUPPORT
        BHAB_Handle hab = handle->hHab;
        rc = BHAB_WriteRegister(hab, reg, &data);
        if (rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
        }
#endif
    }
    else if (BMXT_IS_RPC_BASED(chip)) {
#if BMXT_RPC_SUPPORT
        BRPC_Param_ECM_WriteXptBlock block;
        BERR_Code rcR = BERR_SUCCESS;
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
#if BMXT_REG_SUPPORT
        BREG_Handle hReg = handle->hReg;
        BSTD_UNUSED(rc);
        BREG_Write32(hReg, reg, data);
#endif
    }

    return;
}

static BERR_Code BMXT_P_GetDefaultSettings_common(BMXT_Settings *pSettings)
{
    unsigned i;

    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    for (i=0; i<BMXT_MAX_NUM_MTSIF_TX; i++) {
        pSettings->MtsifTxCfg[i].TxInterfaceWidth = 8;
        pSettings->MtsifTxCfg[i].TxClockPolarity = 1;
        pSettings->MtsifTxCfg[i].Enable = false;
    }

    for (i=0; i<BMXT_MAX_NUM_MTSIF_RX; i++) {
        pSettings->MtsifRxCfg[i].RxInterfaceWidth = 8;
        pSettings->MtsifRxCfg[i].RxClockPolarity = 0;
        pSettings->MtsifRxCfg[i].Enable = false;
    }

    return BERR_SUCCESS;
}

static BERR_Code BMXT_Open_common(BMXT_Handle *pHandle, BCHP_Handle hChp, BREG_Handle hReg, const BMXT_Settings *pSettings)
{
    BMXT_Handle mxt = NULL;
    BERR_Code rc = BERR_SUCCESS;
    uint32_t reg, addr;
    unsigned i;

    BDBG_ASSERT(pHandle);
    BDBG_ASSERT(pSettings);

    if (pSettings->hVirtual) {
        /* pSettings->hHab and ->hRpc are dont-care's for now. do nothing */
    }
    else if (!BMXT_IS_REG_BASED(pSettings->chip)) {
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
    }
    else {
        if (hReg==NULL) {
            BDBG_ERR(("BMXT_Open requires a BREG handle"));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto done;
        }
        if (pSettings->hHab || pSettings->hRpc) {
            BDBG_WRN(("BMXT_DefaultSettings.hHab or .hRpc is set, but will be unused"));
        }
    }

    /* alloc memory from the system heap */
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
    BMXT_P_GetNumResourcesAll(mxt->settings.chip, mxt->settings.chipRev, &mxt->num);
    BDBG_ASSERT(mxt->num.mtsifTx <= BMXT_MAX_NUM_MTSIF_TX);
    BDBG_ASSERT(mxt->num.mtsifRx <= BMXT_MAX_NUM_MTSIF_RX);
    BDBG_ASSERT(mxt->num.ib <= BMXT_MAX_NUM_INPUT_BANDS);
    BDBG_ASSERT(mxt->num.pb <= BMXT_MAX_NUM_PARSERS);
    BDBG_ASSERT(mxt->num.pbRemap <= mxt->num.pb);
    BDBG_ASSERT(mxt->num.tsmf <= BMXT_MAX_NUM_TSMF);
    BDBG_MSG(("BMXT_Open: resources: %u %u %u %u %u %u", mxt->num.mtsifTx, mxt->num.mtsifRx, mxt->num.ib, mxt->num.pb, mxt->num.pbRemap, mxt->num.tsmf));

    /* create a handle, but avoid any register access for fake handles */
    if (!mxt->num.mtsifTx && !mxt->num.mtsifRx && !mxt->num.ib && !mxt->num.pb && !mxt->num.tsmf && !mxt->num.pbRemap) {
        goto done;
    }

    /* SW7425-1655 */
    reg = BMXT_RegRead32(mxt, BCHP_DEMOD_XPT_FE_ATS_COUNTER_CTRL);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_ATS_COUNTER_CTRL, INC_MUX_SEL, 1);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_ATS_COUNTER_CTRL, EXT_RESET_ENABLE, 1);
    BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_ATS_COUNTER_CTRL, reg);

    /* clear interrupt status regs */
    BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_INTR_STATUS0_REG, 0);
    BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_INTR_STATUS1_REG, 0);

    for (i=0; i<mxt->num.mtsifTx; i++) {
        if (pSettings->MtsifTxCfg[i].TxInterfaceWidth != 8) {
            BDBG_ERR(("Support for other than 8-bit wide MTSIF needs to be added"));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            BKNI_Free(mxt);
            mxt = NULL;
            goto done;
        }

        addr = BCHP_DEMOD_XPT_FE_MTSIF_TX0_CTRL1 + (i*MTSIF_TX_STEPSIZE);
        reg = BMXT_RegRead32(mxt, addr);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, MTSIF_TX_IF_WIDTH, 3); /* 8-bit wide */
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, MTSIF_TX_CLOCK_POL_SEL, pSettings->MtsifTxCfg[i].TxClockPolarity);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, TX_ENABLE, pSettings->MtsifTxCfg[i].Enable==true ? 1 : 0);
        BMXT_RegWrite32(mxt, addr, reg);

        addr = BCHP_DEMOD_XPT_FE_MTSIF_TX0_SECRET_WORD + (i*MTSIF_TX_STEPSIZE);
        BMXT_RegWrite32(mxt, addr, pSettings->MtsifTxCfg[i].Encrypt ? 0:0x829eecde);
    }

    for (i=0; i<mxt->num.mtsifRx; i++) {
        if (pSettings->MtsifRxCfg[i].RxInterfaceWidth != 8) {
            BDBG_ERR(("Support for other than 8-bit wide MTSIF needs to be added"));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            BKNI_Free(mxt);
            mxt = NULL;
            goto done;
        }

        addr = BCHP_DEMOD_XPT_FE_MTSIF_RX0_CTRL1 + (i*MTSIF_RX_STEPSIZE);
        reg = BMXT_RegRead32(mxt, addr);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_IF_WIDTH, 3); /* 8-bit wide */
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_CLOCK_POL_SEL, pSettings->MtsifRxCfg[i].RxClockPolarity);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, PARSER_ENABLE, pSettings->MtsifRxCfg[i].Enable==true ? 1 : 0);
        BMXT_RegWrite32(mxt, addr, reg);

        addr = BCHP_DEMOD_XPT_FE_MTSIF_RX0_SECRET_WORD + (i*MTSIF_RX_STEPSIZE);
        BMXT_RegWrite32(mxt, addr, pSettings->MtsifRxCfg[i].Decrypt ? 0:0x829eecde);
    }

    for (i=0; i<mxt->num.tsmf; i++) {
        BMXT_TSMFFldVerifyConfig tsmfConfig;
        BKNI_Memset(&tsmfConfig, 0, sizeof(tsmfConfig));
        BMXT_TSMF_SetFldVerifyConfig(mxt, i, &tsmfConfig);
        BMXT_TSMF_EnableAutoMode(mxt, 0, i, 0);
        BMXT_TSMF_EnableSemiAutoMode(mxt, 0, i, 0, 0, 0);
        BMXT_TSMF_DisableTsmf(mxt, i);
    }

#if 0
    for (i=0; i<256; i++) {
        BKNI_Printf("PID_TABLE %3u: %#x\n", i, BMXT_RegRead32(mxt, BCHP_DEMOD_XPT_FE_PID_TABLE_i_ARRAY_BASE + (4*i)));
    }
#endif

    for (i=0; i<mxt->num.pb; i++) {
        uint32_t val = 0;
        unsigned pipeSel = PARSER_OUTPUT_PIPE_SEL_FROM_INDEX(mxt, 0); /* default to TX0 */

        reg = BMXT_RegRead32(mxt, BCHP_DEMOD_XPT_FE_PID_TABLE_i_ARRAY_BASE + (4*i));
        #if 0
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, SECURITY_OUTPUT_PIPE_SEL, 0);
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, IGNORE_PID_VERSION, 0);
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL, 0); /* not playback */
        #endif
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, PARSER_OUTPUT_PIPE_SEL, pipeSel);
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT, i);
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, MPOD_BYPASS_EN, 0);
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE, 1);
        #if 0
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER, 0);
        BCHP_SET_FIELD_DATA(val, DEMOD_XPT_FE_PID_TABLE_i, PID_CHANNEL_PID, 0); /* don't care */
        #endif
        BDBG_MSG(("PID_TABLE %3u: %#x -> %#x", i, reg, val));
        BMXT_RegWrite32(mxt, BCHP_DEMOD_XPT_FE_PID_TABLE_i_ARRAY_BASE + (4*i), val);
    }

    /* set IB, PB and IB->PB mapping defaults */
    {
        BMXT_InputBandConfig ibConfig;
        for (i=0; i<mxt->num.ib; i++) {
            BMXT_GetInputBandConfig(mxt, i, &ibConfig);
            ibConfig.DssMode = false;
            BMXT_GetInputBandConfig(mxt, i, &ibConfig);
        }
    }

    {
        BMXT_ParserConfig pbConfig;
        for (i=0; i<mxt->num.pb; i++) {
            BMXT_GetParserConfig(mxt, i, &pbConfig);
            pbConfig.ErrorInputIgnore = false;
            pbConfig.TsMode = BMXT_ParserTimestampMode_eMod300;
            pbConfig.AcceptNulls = true;
            pbConfig.AllPass = true;
            pbConfig.DssMode = false;
            pbConfig.InputBandNumber = i; /* Default to 1-to-1 mapping */
            pbConfig.Enable = false;
            BMXT_SetParserConfig(mxt, i, &pbConfig);
            BMXT_TSMF_SetParserConfig(mxt, i, 0, false);
        }
    }

    {
        BMXT_ParserBandMapping parserMapping;
        BMXT_GetParserMapping(mxt, &parserMapping);
        for (i=0; i<mxt->num.pb; i++) {
            parserMapping.FrontEnd[i].VirtualParserBandNum = i;
            parserMapping.FrontEnd[i].VirtualParserIsPlayback = false;
        }
        BMXT_SetParserMapping(mxt, &parserMapping);
    }

done:
    *pHandle = mxt;

    return rc;
}

static void BMXT_Close_common(BMXT_Handle handle)
{
    unsigned i;
    BMXT_ParserConfig pbConfig;
    BDBG_ASSERT(handle);

    for (i=0; i<handle->num.pb; i++) {
        BMXT_GetParserConfig(handle, i, &pbConfig);
        pbConfig.Enable = false;
        BMXT_SetParserConfig(handle, i, &pbConfig);
    }

    BKNI_Free(handle);
}

static BERR_Code BMXT_GetMtsifStatus_common(BMXT_Handle handle, BMXT_MtsifStatus *pStatus)
{
    unsigned i;
    uint32_t addr, val;
    BDBG_ASSERT(handle);
    BDBG_ASSERT(pStatus);

    BKNI_Memset(pStatus, 0, sizeof(BMXT_MtsifStatus));

    for (i=0; i<handle->num.mtsifTx; i++) {
        addr = BCHP_DEMOD_XPT_FE_MTSIF_TX0_CTRL1 + (i*MTSIF_TX_STEPSIZE);
        val = BMXT_RegRead32(handle, addr);
        pStatus->tx[i].interfaceWidth = BCHP_GET_FIELD_DATA(val, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, MTSIF_TX_IF_WIDTH);
        pStatus->tx[i].clockPolarity = BCHP_GET_FIELD_DATA(val, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, MTSIF_TX_CLOCK_POL_SEL);
        pStatus->tx[i].enabled = BCHP_GET_FIELD_DATA(val, DEMOD_XPT_FE_MTSIF_TX0_CTRL1, TX_ENABLE);
    }
    for (i=0; i<handle->num.mtsifRx; i++) {
        addr = BCHP_DEMOD_XPT_FE_MTSIF_RX0_CTRL1 + (i*MTSIF_RX_STEPSIZE);
        val = BMXT_RegRead32(handle, addr);
        pStatus->rx[i].interfaceWidth = BCHP_GET_FIELD_DATA(val, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_IF_WIDTH);
        pStatus->rx[i].clockPolarity = BCHP_GET_FIELD_DATA(val, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_CLOCK_POL_SEL);
        pStatus->rx[i].enabled = BCHP_GET_FIELD_DATA(val, DEMOD_XPT_FE_MTSIF_RX0_CTRL1, PARSER_ENABLE);
    }

    return BERR_SUCCESS;
}

static BERR_Code BMXT_GetInputBandStatus_common(BMXT_Handle handle, BMXT_InputBandStatus *pStatus)
{
    unsigned i;
    unsigned const IB_SYNC_STEPSIZE = (BCHP_DEMOD_XPT_FE_IB1_SYNC_COUNT - BCHP_DEMOD_XPT_FE_IB0_SYNC_COUNT);
    uint32_t addr;
    BDBG_ASSERT(handle);
    BDBG_ASSERT(pStatus);

    BKNI_Memset(pStatus, 0, sizeof(BMXT_InputBandStatus));

    for (i=0; i<handle->num.ib; i++) {
        addr = BCHP_DEMOD_XPT_FE_IB0_SYNC_COUNT + (i*IB_SYNC_STEPSIZE);
        pStatus->syncCount[i] = BMXT_RegRead32(handle, addr);
    }

    return BERR_SUCCESS;
}


#define PARSER_INPUT_SEL_MASK (BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1_PARSER_INPUT_SEL_MASK >> BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1_PARSER_INPUT_SEL_SHIFT)

static BERR_Code BMXT_GetParserConfig_common(BMXT_Handle handle, unsigned parserNum, BMXT_ParserConfig *pConfig)
{
    uint32_t Reg, RegAddr;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ASSERT(handle);
    BDBG_ASSERT(pConfig);

    if (parserNum >= handle->num.pb) {
        BDBG_ERR(("GetParserConfig%u: parserNum out of range!", parserNum));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    RegAddr = BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1 + (parserNum * PB_REG_STEPSIZE);
    Reg = BMXT_RegRead32(handle, RegAddr);

    pConfig->ErrorInputIgnore = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE);
    pConfig->TsMode = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE);
    pConfig->AcceptNulls = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD);
    pConfig->AllPass = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD);
    pConfig->DssMode = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PACKET_TYPE) ? true : false;
    pConfig->InputBandNumber = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL) +
        (BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL_MSB) ? (PARSER_INPUT_SEL_MASK+1) : 0);
    pConfig->Enable = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ENABLE);

    /* MTSIF TX select */
    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_PID_TABLE_i_ARRAY_BASE + (4*parserNum));
    if (BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT)!=parserNum) {
        BDBG_ERR(("GetParserConfig%u: Unable to get MTSIF_TX selection", parserNum));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto done;
    }
    Reg = PARSER_OUTPUT_PIPE_SEL_TO_INDEX(handle, BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_PID_TABLE_i, PARSER_OUTPUT_PIPE_SEL));
    if (Reg >= handle->num.mtsifTx) {
        BDBG_ERR(("GetParserConfig%u: Unexpected MTSIF_TX selection %u", parserNum, Reg));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto done;
    }
    else {
        pConfig->mtsifTxSelect = Reg;
    }

done:
    BDBG_MSG(("GetParserConfig%u: IB%2u, enable%u, MTSIF%u, DSS%u", parserNum, pConfig->InputBandNumber, pConfig->Enable, (rc==BERR_SUCCESS) ? pConfig->mtsifTxSelect : (unsigned)-1, pConfig->DssMode));
    return rc;
}

static BERR_Code BMXT_SetParserConfig_common(BMXT_Handle handle, unsigned parserNum, const BMXT_ParserConfig *pConfig)
{
    uint32_t Reg, RegAddr;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_ASSERT(handle);
    BDBG_ASSERT(pConfig);

    if (parserNum >= handle->num.pb) {
        BDBG_ERR(("SetParserConfig%u: parserNum out of range!", parserNum));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if (pConfig->mtsifTxSelect >= handle->num.mtsifTx) {
        BDBG_ERR(("SetParserConfig%u: MTSIF_TX %lu is out of range!", parserNum, pConfig->mtsifTxSelect));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    RegAddr = BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1 + (parserNum * PB_REG_STEPSIZE);
    Reg = BMXT_RegRead32(handle, RegAddr);

    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE, pConfig->ErrorInputIgnore);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE, pConfig->TsMode);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD, pConfig->AcceptNulls);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD, pConfig->AllPass);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PKT_LENGTH, pConfig->DssMode ? 130 : 188);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PACKET_TYPE, pConfig->DssMode ? 1 : 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL_MSB, (pConfig->InputBandNumber > PARSER_INPUT_SEL_MASK) ? 1 : 0);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL, (pConfig->InputBandNumber & PARSER_INPUT_SEL_MASK));
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ENABLE, pConfig->Enable ? 1 : 0);

    BMXT_RegWrite32(handle, RegAddr, Reg);
    BDBG_MSG(("SetParserConfig%u: IB%2u, enable%u, MTSIF%u, DSS%u", parserNum, pConfig->InputBandNumber, pConfig->Enable, pConfig->mtsifTxSelect, pConfig->DssMode));

    /* MTSIF TX select */
    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_PID_TABLE_i_ARRAY_BASE + (4*parserNum));
    if (BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT)!=parserNum) {
        /* assume that pid channel x and parser x have the same number. if not, we could traverse through
           the pid table and find out but it's simpler just to enforce this assumption */
        BDBG_ERR(("SetParserConfig%u: Unable to set MTSIF_TX selection on parser", parserNum));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto done;
    }
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_PID_TABLE_i, PARSER_OUTPUT_PIPE_SEL, PARSER_OUTPUT_PIPE_SEL_FROM_INDEX(handle, pConfig->mtsifTxSelect));
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_PID_TABLE_i_ARRAY_BASE + (4*parserNum), Reg);

done:
    return rc;
}

static BERR_Code BMXT_GetParserMapping_common(BMXT_Handle handle, BMXT_ParserBandMapping *pMapping)
{
    uint32_t reg;
    unsigned i;
    BDBG_ASSERT(handle);
    BDBG_ASSERT(pMapping);

    BKNI_Memset(pMapping, 0, sizeof(BMXT_ParserBandMapping)); /* to clear VirtualParserIsPlayback */

    if (handle->num.pbRemap > 0) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID);
        pMapping->FrontEnd[0].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID);
        pMapping->FrontEnd[1].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID);
        pMapping->FrontEnd[2].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID);
        pMapping->FrontEnd[3].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID);

        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 4);
        pMapping->FrontEnd[4].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID);
        pMapping->FrontEnd[5].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID);
        pMapping->FrontEnd[6].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID);
        pMapping->FrontEnd[7].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID);
    }

    if (handle->num.pbRemap > 8) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 8);
        pMapping->FrontEnd[8].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID);
        pMapping->FrontEnd[9].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID);
        pMapping->FrontEnd[10].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID);
        pMapping->FrontEnd[11].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID);
    }

    if (handle->num.pbRemap > 12) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 12);
        pMapping->FrontEnd[12].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID);
        pMapping->FrontEnd[13].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID);
        pMapping->FrontEnd[14].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID);
        pMapping->FrontEnd[15].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID);
    }

    if (handle->num.pbRemap > 16) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 16);
        pMapping->FrontEnd[16].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID);
        pMapping->FrontEnd[17].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID);
        pMapping->FrontEnd[18].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID);
        pMapping->FrontEnd[19].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID);
    }

    if (handle->num.pbRemap > 20) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 20);
        pMapping->FrontEnd[20].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID);
        pMapping->FrontEnd[21].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID);
        pMapping->FrontEnd[22].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID);
        pMapping->FrontEnd[23].VirtualParserBandNum = BCHP_GET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID);
    }

    /* overwrite all non-remappable parsers with default mapping */
    for (i=handle->num.pbRemap; i<BMXT_MAX_NUM_PARSERS; i++) {
        pMapping->FrontEnd[i].VirtualParserBandNum = i;
    }

    return BERR_SUCCESS;
}

static BERR_Code BMXT_SetParserMapping_common(BMXT_Handle handle, const BMXT_ParserBandMapping *pMapping)
{
    uint32_t reg;
    BERR_Code rc = BERR_SUCCESS;
    unsigned i;

    BDBG_ASSERT(handle);
    BDBG_ASSERT(pMapping);

    /* only certain parsers can be remapped */
    for (i=handle->num.pbRemap; i<handle->num.pb; i++) {
        const BMXT_BandMap *map = &pMapping->FrontEnd[i];

        if (map->VirtualParserBandNum!=i) {
            BDBG_WRN(("PB%u cannot be remapped to %u", i, map->VirtualParserBandNum));
        }
        if (map->VirtualParserIsPlayback) {
            BDBG_WRN(("PB%u cannot be remapped as a playback parser"));
        }
        /* warn, but keep going */
    }

    /* check that 4-7 is the same as 0-3, and assume that 8-11 is the same as well.
       4538 RDB only has upto 9, but 3384 has upto 18 */
    BDBG_CASSERT(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID_PARSER0_BAND_ID_MASK  == BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID_PARSER4_BAND_ID_MASK);
    BDBG_CASSERT(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID_PARSER0_BAND_ID_SHIFT == BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID_PARSER4_BAND_ID_SHIFT);
    BDBG_CASSERT(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID_PARSER1_BAND_ID_MASK  == BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID_PARSER5_BAND_ID_MASK);
    BDBG_CASSERT(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID_PARSER1_BAND_ID_SHIFT == BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID_PARSER5_BAND_ID_SHIFT);
    BDBG_CASSERT(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID_PARSER2_BAND_ID_MASK  == BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID_PARSER6_BAND_ID_MASK);
    BDBG_CASSERT(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID_PARSER2_BAND_ID_SHIFT == BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID_PARSER6_BAND_ID_SHIFT);
    BDBG_CASSERT(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID_PARSER3_BAND_ID_MASK  == BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID_PARSER7_BAND_ID_MASK);
    BDBG_CASSERT(BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID_PARSER3_BAND_ID_SHIFT == BCHP_DEMOD_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID_PARSER7_BAND_ID_SHIFT);

    reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID, pMapping->FrontEnd[0].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID, pMapping->FrontEnd[1].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID, pMapping->FrontEnd[2].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID, pMapping->FrontEnd[3].VirtualParserBandNum);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, reg);

    reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 4);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID, pMapping->FrontEnd[4].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID, pMapping->FrontEnd[5].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID, pMapping->FrontEnd[6].VirtualParserBandNum);
    BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID, pMapping->FrontEnd[7].VirtualParserBandNum);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 4, reg);

    /* if the register exists, then writing garbage to non-existent fields is harmless */
    if (handle->num.pbRemap > 8) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 8);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID, pMapping->FrontEnd[8].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID, pMapping->FrontEnd[9].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID, pMapping->FrontEnd[10].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID, pMapping->FrontEnd[11].VirtualParserBandNum);
        BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 8, reg);
    }

    if (handle->num.pbRemap > 12) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 12);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID, pMapping->FrontEnd[12].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID, pMapping->FrontEnd[13].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID, pMapping->FrontEnd[14].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID, pMapping->FrontEnd[15].VirtualParserBandNum);
        BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 12, reg);
    }

    if (handle->num.pbRemap > 16) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 16);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID, pMapping->FrontEnd[16].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID, pMapping->FrontEnd[17].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID, pMapping->FrontEnd[18].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID, pMapping->FrontEnd[19].VirtualParserBandNum);
        BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 16, reg);
    }

    if (handle->num.pbRemap > 20) {
        reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 20);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID, pMapping->FrontEnd[20].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID, pMapping->FrontEnd[21].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID, pMapping->FrontEnd[22].VirtualParserBandNum);
        BCHP_SET_FIELD_DATA(reg, DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID, pMapping->FrontEnd[23].VirtualParserBandNum);
        BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID + 20, reg);
    }

    return rc;
}

static BERR_Code BMXT_GetInputBandConfig_common(BMXT_Handle handle, unsigned bandNum, BMXT_InputBandConfig *pConfig)
{
    uint32_t Reg, RegAddr;
    BDBG_ASSERT(pConfig);

    if (bandNum>=handle->num.ib) {
        BDBG_ERR(("BandNum %lu is out of range!", bandNum));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    RegAddr = BCHP_DEMOD_XPT_FE_IB0_CTRL + (bandNum * IB_REG_STEPSIZE);
    Reg = BMXT_RegRead32(handle, RegAddr);
    pConfig->DssMode = (BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_IB0_CTRL, IB_PKT_LENGTH) == 130);
    pConfig->ParallelInputSel = BCHP_GET_FIELD_DATA(Reg, DEMOD_XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL);

    return BERR_SUCCESS;
}

static BERR_Code BMXT_SetInputBandConfig_common(BMXT_Handle handle, unsigned bandNum, const BMXT_InputBandConfig *pConfig)
{
    uint32_t Reg, RegAddr;
    BDBG_ASSERT(pConfig);

    if (bandNum>=handle->num.ib) {
        BDBG_ERR(("BandNum %lu is out of range!", bandNum));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    RegAddr = BCHP_DEMOD_XPT_FE_IB0_CTRL + (bandNum * IB_REG_STEPSIZE);
    Reg = BMXT_RegRead32(handle, RegAddr);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_IB0_CTRL, IB_PKT_LENGTH, pConfig->DssMode ? 130 : 188);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL, pConfig->ParallelInputSel ? 1 : 0);
    BMXT_RegWrite32(handle, RegAddr, Reg);

    return BERR_SUCCESS;
}

static uint32_t BMXT_ReadIntStatusRegister_common(BMXT_Handle handle, BMXT_IntReg intReg)
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

/****************************************************************
TSMF APIs
****************************************************************/

#define BMXT_TSMF_P_REG_OFFSET (BCHP_DEMOD_XPT_FE_TSMF1_CTRL - BCHP_DEMOD_XPT_FE_TSMF0_CTRL)

static BERR_Code BMXT_TSMF_P_GetOffset(unsigned tsmfNum, uint32_t *offset)
{
    BERR_Code ret = BERR_SUCCESS;

    if (tsmfNum >= BMXT_MAX_NUM_TSMF) {
        BDBG_ERR(("Exceed Maxium number of TSMF supported"));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    *offset = BMXT_TSMF_P_REG_OFFSET * tsmfNum;
    return ret;
}

static BERR_Code BMXT_TSMF_GetFldVerifyConfig_common(BMXT_Handle handle, unsigned tsmfNum, BMXT_TSMFFldVerifyConfig *pConfig)
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;

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

    return rc;
}

static BERR_Code BMXT_TSMF_SetFldVerifyConfig_common(BMXT_Handle handle, unsigned tsmfNum, const BMXT_TSMFFldVerifyConfig *pConfig)
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;

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

    return rc;
}

static BERR_Code BMXT_TSMF_EnableAutoMode_common(BMXT_Handle handle, BMXT_TSMFInputSel input, unsigned tsmfNum, uint8_t relativeTsNum)
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;
    BDBG_MSG(("TSMF_EnableAutoMode: IB%u -> TSMF%u, rel%u", input, tsmfNum, relativeTsNum));

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

    return rc;
}

static BERR_Code BMXT_TSMF_EnableSemiAutoMode_common(BMXT_Handle handle, BMXT_TSMFInputSel input, unsigned tsmfNum, uint32_t slotMapLo, uint32_t slotMapHi, uint8_t relativeTsNum)
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;
    BDBG_MSG(("TSMF_EnableSemiAutoMode: IB%u -> TSMF%u, slot%u:%u, rel%u", input, tsmfNum, slotMapLo, slotMapHi, relativeTsNum));

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

    return rc;
}

static BERR_Code BMXT_TSMF_DisableTsmf_common(BMXT_Handle handle, unsigned tsmfNum)
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;
    BDBG_MSG(("TSMF_DisableTsmf: TSMF%u", tsmfNum));

    rc = BMXT_TSMF_P_GetOffset(tsmfNum, &RegOffset);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return rc;
    }
    Reg = BMXT_RegRead32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset);
    BCHP_SET_FIELD_DATA(Reg, DEMOD_XPT_FE_TSMF0_CTRL, TSMF_ENABLE, 0);
    BMXT_RegWrite32(handle, BCHP_DEMOD_XPT_FE_TSMF0_CTRL + RegOffset, Reg);

    return rc;
}

static BERR_Code BMXT_TSMF_SetParserConfig_common(BMXT_Handle handle, unsigned parserNum, unsigned tsmfNum, bool enable)
{
    uint32_t Reg, RegOffset = 0;
    BERR_Code rc = BERR_SUCCESS;
    BDBG_MSG(("TSMF_SetParserConfig: TSMF%u -> PB%u, enable%u", tsmfNum, parserNum, enable));

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
    return rc;
}

/****************************************************************
Power APIs
****************************************************************/
static void BMXT_GetPowerSaverSettings_common(BMXT_Handle handle, BMXT_PowerSaverSettings *pSettings)
{
    BDBG_ASSERT(handle);
    BDBG_ASSERT(pSettings);
    *pSettings = handle->powerSettings;
}

static BERR_Code BMXT_SetPowerSaverSettings_common(BMXT_Handle handle, const BMXT_PowerSaverSettings *pSettings)
{
    BMXT_Chip chip;
    BDBG_ASSERT(handle);
    BDBG_ASSERT(pSettings);
    chip = handle->settings.chip;

    if (chip == BMXT_Chip_e4538) {
#if BMXT_HAB_SUPPORT
        BERR_Code rc;
        uint8_t buf[4];
        buf[0] = 0x4E;
        buf[1] = pSettings->disableMtsif ? 1 : 0;
        rc = BHAB_SendHabCommand(handle->hHab, buf, 3, buf, 3, true, true, 3);
        if (rc!=BERR_SUCCESS) {
            return BERR_TRACE(rc);
        }
#endif
    }

    handle->powerSettings = *pSettings;
    return BERR_SUCCESS;
}