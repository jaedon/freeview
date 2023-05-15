/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_security.c $
 * $brcm_Revision: 23 $
 * $brcm_Date: 9/28/12 3:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/7550/src/nexus_security.c $
 * 
 * 23   9/28/12 3:58p atruong
 * SWSECURITY-175: Symbolic link removal for 7408 and 7550 HSM PI
 * 
 * 22   7/9/12 1:12p vsilyaev
 * SW7420-2316: SW7420-2316: Moved NEXUS_OBJECT_CLASS(NEXUS_KeySlot) into
 *  the core module
 * 
 * 21   6/19/12 6:21p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 20   11/28/11 6:48p nitinb
 * SW7425-1708: Fixed build error for 7550
 * 
 * 19   3/17/11 6:41p mphillip
 * SW7408-250: Remap algorithm enum for all keyslot types, not just CA
 * 
 * 18   3/2/11 4:33p gmohile
 * SW7408-190 : set power before opening HSM PI
 * 
 * 17   2/23/11 4:17p mphillip
 * SW7422-253: Update code for changed sc bit API
 * 
 * 16   12/23/10 3:40p mphillip
 * SW7422-104: Fixup 7550 and related chips with the enum changes
 * 
 * 15   8/6/10 4:38p mphillip
 * SW7405-4325: Update 7550/7408 to keyslot implementation in core
 * 
 * 14   5/12/10 2:25p erickson
 * SW7550-419: remove unnecessary keyHandle->pidChannel code
 *
 * 13   5/11/10 7:13p qcheng
 * SW7550-419 : 7550's BHSM_FreeCAKeySlot appears to have a wrong API
 *
 * 12   5/10/10 10:48a erickson
 * SW7550-415: general cleanup. fix error recovery. add BERR_TRACE.
 *  propogate return codes. add BDBG_OBJECT_ASSERT. add
 *  NEXUS_Security_GetDefaultClearKey.
 *
 * 11   5/6/10 6:43p qcheng
 * SW7550-102 : Add Security support for 7550
 *
 * 10   4/7/10 2:03p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 *  PVR function
 *
 * 8   4/5/10 5:42p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 *  PVR function
 *
 * 7   4/2/10 7:23p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 *  PVR function
 *
 * 6   3/23/10 11:39a qcheng
 * SW7408-82 : Add security support for 7408 A0
 *
 * 5   3/22/10 7:54p qcheng
 * SW7408-82 : Add security support for 7408 A0
 *
 * 4   3/4/10 11:51a qcheng
 * SW7408-82 : Add security support for 7408 A0
 *
 * 3   1/19/10 12:29p qcheng
 * SW7550-199 : Add Customer Mode support in Nexus security module.
 *
 * 2   1/6/10 3:59p qcheng
 * SW7550-162: The transport_scrambling_control flag of the TS packet
 *  cannot be referred to.
 *
 * 1   12/7/09 11:33a qcheng
 * SW7550-102 : Add Security support for 7550
 *
 **************************************************************************/
#include "nexus_security_module.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "priv/nexus_security_priv.h"
#include "priv/nexus_core.h"
#include "priv/nexus_security_standby_priv.h"


#include "nexus_power_management.h"

#include "bhsm.h"
#include "bhsm_keyladder.h"

BDBG_MODULE(nexus_security);

NEXUS_ModuleHandle NEXUS_P_SecurityModule;
static struct {
    NEXUS_SecurityModuleSettings settings;
    BHSM_Handle hsm;
/*    BHSM_ChannelHandle hsmChannel[BHSM_HwModule_eMax]; */
} g_security;

static BERR_Code NEXUS_Security_P_InitHsm(const NEXUS_SecurityModuleSettings * pSettings);
static void NEXUS_Security_P_UninitHsm(void);

/****************************************
* Module functions
***************/

void NEXUS_SecurityModule_GetDefaultSettings(NEXUS_SecurityModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->customerMode = NEXUS_SecurityCustomerMode_eGeneric;
}

NEXUS_ModuleHandle NEXUS_SecurityModule_Init(const NEXUS_SecurityModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;
    NEXUS_SecurityModuleSettings defaultSettings;
    BERR_Code rc;

    BDBG_ASSERT(!NEXUS_P_SecurityModule);

    if (!pSettings) {
        NEXUS_SecurityModule_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow;
    NEXUS_P_SecurityModule = NEXUS_Module_Create("security", &moduleSettings);
    if (!NEXUS_P_SecurityModule) {
        return NULL;
    }

    g_security.settings = *pSettings;
    
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, true);

    rc = NEXUS_Security_P_InitHsm(pSettings);
    if (rc) {
        NEXUS_Module_Destroy(NEXUS_P_SecurityModule);
        NEXUS_P_SecurityModule = NULL;
    }
    
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, false);

    return NEXUS_P_SecurityModule;
}

void NEXUS_SecurityModule_Uninit()
{
    NEXUS_Security_P_UninitHsm();
    NEXUS_Module_Destroy(NEXUS_P_SecurityModule);
    NEXUS_P_SecurityModule = NULL;
}

static BERR_Code NEXUS_Security_P_InitHsm(const NEXUS_SecurityModuleSettings * pSettings)
{
    BHSM_Settings hsmSettings;
    BERR_Code rc;

    BHSM_GetDefaultSettings(&hsmSettings, g_pCoreHandles->chp);
    switch( pSettings->customerMode ) {
    case NEXUS_SecurityCustomerMode_eDvs042:
        hsmSettings.eCustMode = BHSM_CustMode_eDVS042;
        break;

    case NEXUS_SecurityCustomerMode_eDesCts:
        hsmSettings.eCustMode = BHSM_CustMode_eDESCTS;
        break;

    case NEXUS_SecurityCustomerMode_eDvbCsa:
        hsmSettings.eCustMode = BHSM_CustMode_eDVBCSA;
        break;

    case NEXUS_SecurityCustomerMode_eGeneric:
        hsmSettings.eCustMode = BHSM_CustMode_eGeneric;
        break;

    default:
        return NEXUS_INVALID_PARAMETER;
    }    

    rc = BHSM_Open(&g_security.hsm, g_pCoreHandles->reg, g_pCoreHandles->chp, g_pCoreHandles->bint, &hsmSettings);
    if (rc) {return BERR_TRACE(rc);}

    return rc;
}

void NEXUS_Security_P_UninitHsm()
{
    BHSM_Close(g_security.hsm);
}

void NEXUS_Security_GetHsm_priv(BHSM_Handle *pHsm)
{
    NEXUS_ASSERT_MODULE();
    *pHsm = g_security.hsm;
}

void NEXUS_Security_GetDefaultKeySlotSettings( NEXUS_SecurityKeySlotSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->keySlotEngine = NEXUS_SecurityEngine_eCa;
    pSettings->keySlotSource = NEXUS_SecurityKeySource_eFirstRam;
}

void NEXUS_Security_GetKeySlotInfo( NEXUS_KeySlotHandle keyHandle, NEXUS_SecurityKeySlotInfo *pKeyslotInfo )
{
    NEXUS_P_Core_GetKeySlotInfo(keyHandle,pKeyslotInfo);
}


#define NEXUS_SECURITY_CACP_INVALID_PIDCHANNEL 0xFFFFFFFF

static NEXUS_Error NEXUS_Security_AllocateKeySlotForType( NEXUS_KeySlotHandle *pKeyHandle, NEXUS_SecurityEngine engine )
{
    BERR_Code rc;
    NEXUS_KeySlotHandle pHandle;
    unsigned int keyslotNumber;

    pHandle = NEXUS_P_Core_AllocateKeySlotHandle();
    if ( !pHandle) { return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); }

    rc = BHSM_AllocateCAKeySlot(g_security.hsm, &keyslotNumber);
    if (rc)
        goto error;

/*    pHandle->keyslotType = type; */
    pHandle->keyslotType = 0;
    pHandle->keySlotNumber = keyslotNumber;
    pHandle->cryptoEngine = engine;
    *pKeyHandle = pHandle;

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, true);
    return NEXUS_SUCCESS;

error:
    NEXUS_P_Core_FreeKeySlotHandle(pHandle);
    *pKeyHandle = NULL;
    return BERR_TRACE(rc);
}


NEXUS_KeySlotHandle NEXUS_Security_LocateCaKeySlotAssigned( unsigned long pidchannel )
{
    BERR_Code rc;
    NEXUS_KeySlotHandle pHandle;
    unsigned int keyslotNumber;

    pHandle = NEXUS_P_Core_AllocateKeySlotHandle();
    if ( !pHandle) { BERR_Code rc; rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); return NULL; }

    rc = BHSM_LocateCAKeySlotAssigned( g_security.hsm, pidchannel, &keyslotNumber );
    if (rc) {
        rc = BERR_TRACE(rc);
        goto error;
    }

    pHandle->keyslotType = 0;
    pHandle->keySlotNumber = keyslotNumber;
    pHandle->cryptoEngine = NEXUS_SecurityEngine_eCa;
    return pHandle;

error:
    NEXUS_P_Core_FreeKeySlotHandle(pHandle);
    return NULL;
}

NEXUS_Error NEXUS_Security_AddPidChannelToKeySlot( NEXUS_KeySlotHandle keyHandle, unsigned int pidChannel )
{
    BHSM_ConfigPidKeyPointerTableIO_t configPidKeyPointerTableIO;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);

    BKNI_Memset(&configPidKeyPointerTableIO, 0, sizeof(configPidKeyPointerTableIO));
    configPidKeyPointerTableIO.unPidChannel = pidChannel;
    configPidKeyPointerTableIO.unKeySlotNum = keyHandle->keySlotNumber;

    rc = BHSM_ConfigPidKeyPointerTable( g_security.hsm, &configPidKeyPointerTableIO);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_Security_RemovePidChannelFromKeySlot( NEXUS_KeySlotHandle keyHandle, unsigned int pidChannel )
{
    BHSM_ConfigPidKeyPointerTableIO_t configPidKeyPointerTableIO;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);

    BKNI_Memset(&configPidKeyPointerTableIO, 0, sizeof(configPidKeyPointerTableIO));
    configPidKeyPointerTableIO.unPidChannel = pidChannel;
    configPidKeyPointerTableIO.unKeySlotNum = BHSM_KEY_POINTOR_INIT_VAL;

    rc = BHSM_ConfigPidKeyPointerTable( g_security.hsm, &configPidKeyPointerTableIO);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}


void NEXUS_Security_GetDefaultInvalidateKeySettings( NEXUS_SecurityInvalidateKeySettings  *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error NEXUS_Security_InvalidateKey(NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityInvalidateKeySettings *pSettings )
{
    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_INVALID_PARAMETER);
}


NEXUS_Error NEXUS_Security_AllocateM2mKeySlot( NEXUS_KeySlotHandle * pKeyHandle )
{
    BERR_Code rc;
    NEXUS_KeySlotHandle pHandle;
    unsigned int keyslotNumber;

    BDBG_MSG(("Allocating M2M keyslot"));

    BDBG_ASSERT(pKeyHandle);
    pHandle = NEXUS_P_Core_AllocateKeySlotHandle();
    if ( !pHandle) { return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); }

    rc = BHSM_AllocateM2MKeySlot(g_security.hsm, &keyslotNumber);
    if (rc)
    {
        NEXUS_P_Core_FreeKeySlotHandle(pHandle);
        *pKeyHandle = NULL;
        BDBG_ERR(("Failed to allocate M2M keyslot (rc = %x)",rc));
        return BERR_TRACE(rc);
    }

    pHandle->keyslotType = 0;
    pHandle->keySlotNumber = keyslotNumber;
    pHandle->cryptoEngine = NEXUS_SecurityEngine_eM2m;
    BDBG_MSG(("Allocated M2M keyslot %d",keyslotNumber));

    *pKeyHandle = pHandle;
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, true);
    return NEXUS_SUCCESS;
}


void NEXUS_Security_GetDefaultAlgorithmSettings( NEXUS_SecurityAlgorithmSettings * pSettings )
{
    int i;
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->algorithm       = NEXUS_SecurityAlgorithm_e3DesAba;
    pSettings->algorithmVar    = NEXUS_SecurityAlgorithmVariant_eEcb;
    pSettings->terminationMode = NEXUS_SecurityTerminationMode_eClear;
    pSettings->aesCounterSize  = NEXUS_SecurityAesCounterSize_e32Bits;
    pSettings->dvbScramLevel   = NEXUS_SecurityDvbScrambleLevel_eTs;
    pSettings->operation       = NEXUS_SecurityOperation_ePassThrough;
    pSettings->testKey2Select           = false;
    pSettings->bRestrictEnable          = true;
    pSettings->bGlobalEnable            = true;
    pSettings->bScAtscMode              = false;
    pSettings->bAtscModEnable           = false;
    pSettings->bGlobalDropPktEnable     = false;
    pSettings->bRestrictDropPktEnable   = false;
    pSettings->bGlobalRegionOverwrite   = false;
    pSettings->bEncryptBeforeRave       = false;
    for (i=0;i<NEXUS_SecurityPacketType_eMax;i++)
        pSettings->modifyScValue[i]     = true;
    pSettings->enableExtIv              = false;
    pSettings->enableExtKey             = false;
    pSettings->mscBitSelect             = false;
    pSettings->bDisallowGG              = false;
    pSettings->bDisallowGR              = false;
    pSettings->bDisallowRG              = false;
    pSettings->bDisallowRR              = false;
}

static BHSM_Algorithm_e NEXUS_Security_MapNexusAlgorithmToHsm(NEXUS_SecurityAlgorithm algorithm) {
    BHSM_Algorithm_e rvAlgorithm = (BHSM_Algorithm_e)algorithm;
    switch (algorithm) {
    case NEXUS_SecurityAlgorithm_eDvb:
        rvAlgorithm = BHSM_Algorithm_eDvb;
        break;
    case NEXUS_SecurityAlgorithm_eMulti2:
        rvAlgorithm = BHSM_Algorithm_eMulti2;
        break;
    case NEXUS_SecurityAlgorithm_eDes:
        rvAlgorithm = BHSM_Algorithm_eDes;
        break;
    case NEXUS_SecurityAlgorithm_e3DesAba:
        rvAlgorithm = BHSM_Algorithm_e3DesAba;
        break;
    case NEXUS_SecurityAlgorithm_e3DesAbc:
        rvAlgorithm = BHSM_Algorithm_e3DesAbc;
        break;
    case NEXUS_SecurityAlgorithm_eAes:
        rvAlgorithm = BHSM_Algorithm_eAes;
        break;
    case NEXUS_SecurityAlgorithm_eAes192:
        rvAlgorithm = BHSM_Algorithm_eAes192;
        break;
    case NEXUS_SecurityAlgorithm_eTivo:
        rvAlgorithm = BHSM_Algorithm_eTivo;
        break;
    case NEXUS_SecurityAlgorithm_eC2:
        rvAlgorithm = BHSM_Algorithm_eC2;
        break;
    case NEXUS_SecurityAlgorithm_eCss:
        rvAlgorithm = BHSM_Algorithm_eCss;
        break;
    case NEXUS_SecurityAlgorithm_eM6Ke:
        rvAlgorithm = BHSM_Algorithm_eM6Ke;
        break;
    case NEXUS_SecurityAlgorithm_eM6:
        rvAlgorithm = BHSM_Algorithm_eM6;
        break;
    case NEXUS_SecurityAlgorithm_eRc4:
        rvAlgorithm = BHSM_Algorithm_eRc4;
        break;
    case NEXUS_SecurityAlgorithm_eCbcMac:
        rvAlgorithm = BHSM_Algorithm_eCbcMac;
        break;
    case NEXUS_SecurityAlgorithm_eWMDrmPd:
        rvAlgorithm = BHSM_Algorithm_eWMDrmPd;
        break;
    case NEXUS_SecurityAlgorithm_eAes128G:
        rvAlgorithm = BHSM_Algorithm_eAes128G;
        break;
    case NEXUS_SecurityAlgorithm_eHdDVD:
        rvAlgorithm = BHSM_Algorithm_eHdDVD;
        break;
    case NEXUS_SecurityAlgorithm_eBrDVD:
        rvAlgorithm = BHSM_Algorithm_eBrDVD;
        break;
    default:
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    return rvAlgorithm;
}

NEXUS_Error NEXUS_Security_ConfigAlgorithm( NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityAlgorithmSettings *pSettings )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHSM_ConfigAlgorithmIO_t configAlgorithmIO;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);

    BKNI_Memset(&configAlgorithmIO, 0, sizeof (configAlgorithmIO));

    if ( ( keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCa ) ||
         ( keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCaCp && pSettings->dest == NEXUS_SecurityAlgorithmConfigDestination_eCa) )
    {
        BDBG_MSG(("Configure CA Algorithm"));

        configAlgorithmIO.unKeySlot = keyHandle->keySlotNumber;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.algorithm = NEXUS_Security_MapNexusAlgorithmToHsm(pSettings->algorithm);
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.cipherDVBMode = pSettings->algorithmVar;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.terminationMode = pSettings->terminationMode;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.IVMode = pSettings->ivMode;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.solitaryMode = pSettings->solitarySelect;

        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bGlobalEnable = pSettings->bGlobalEnable ? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bRestrictEnable = pSettings->bRestrictEnable ? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bGlobalScMod = pSettings->modifyScValue[NEXUS_SecurityPacketType_eGlobal]? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bRestrictScMod = pSettings->modifyScValue[NEXUS_SecurityPacketType_eRestricted]? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.uScValue = pSettings->scValue[NEXUS_SecurityPacketType_eGlobal];

        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bAtscScrambleCtrl= pSettings->bAtscModEnable ? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bAtscMod = pSettings->bScAtscMode ? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bGlobalDropPktCtrl = pSettings->bGlobalDropPktEnable? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bRestrictDropPktCtrl = pSettings->bRestrictDropPktEnable? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bGlobalRegOverwrite = pSettings->bGlobalRegionOverwrite? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bEncryptBeforeRave = pSettings->bEncryptBeforeRave? 1:0;

        configAlgorithmIO.cryptoAlg.cacpCryptAlg.ucMulti2KeySelect = pSettings->multi2KeySelect;

        configAlgorithmIO.ekeyDestBlckType = BHSM_KeyDestBlockType_eCA;

        /*Configure Odd key Algorithm*/
        configAlgorithmIO.eKeyType = BHSM_KeyType_eOddKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCaKeyOdd1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);

        /*Configure Even key Algorithm*/
        configAlgorithmIO.eKeyType = BHSM_KeyType_eEvenKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCaKeyEven1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);

        /*Configure Clear key Algorithm */ /* We do not need configure Clear key Algorithm for CA */
    /*
        configAlgorithmIO.eKeyType = BHSM_KeyType_eClearKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCaKeyOdd1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if ( rc != NEXUS_SUCCESS )
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    */

    }
    else if ( (keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCp) ||
              ( keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCaCp &&
                ( pSettings->dest == NEXUS_SecurityAlgorithmConfigDestination_eCps || pSettings->dest == NEXUS_SecurityAlgorithmConfigDestination_eCpd )))
    {
        BDBG_MSG(("Configure CP Algorithm"));

        configAlgorithmIO.unKeySlot = keyHandle->keySlotNumber;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.algorithm = NEXUS_Security_MapNexusAlgorithmToHsm(pSettings->algorithm);
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.cipherDVBMode = pSettings->algorithmVar;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.terminationMode = pSettings->terminationMode;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.IVMode = pSettings->ivMode;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.solitaryMode = pSettings->solitarySelect;

        configAlgorithmIO.cryptoAlg.cacpCryptAlg.ucMulti2KeySelect = pSettings->multi2KeySelect; /*Dos we care about this in CP ??? */

        /* If configure CPS , we also need configure same algorithm for CPD.  If configure CPD, we also need configure same algorithm for CPS */
        if ( pSettings->dest == NEXUS_SecurityAlgorithmConfigDestination_eCps )
            configAlgorithmIO.ekeyDestBlckType = BHSM_KeyDestBlockType_eCPD;
        else if ( pSettings->dest == NEXUS_SecurityAlgorithmConfigDestination_eCpd )
            configAlgorithmIO.ekeyDestBlckType = BHSM_KeyDestBlockType_eCPS;
        else
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);


        /*Configure Odd key Algorithm*/
        configAlgorithmIO.eKeyType = BHSM_KeyType_eOddKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCpKeyOdd1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);

        /*Configure Even key Algorithm*/
        configAlgorithmIO.eKeyType = BHSM_KeyType_eEvenKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCpKeyEven1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);

        /*Configure Clear key Algorithm*/
        configAlgorithmIO.eKeyType = BHSM_KeyType_eClearKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCpKeyOdd1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);

        /*Now we configure CPS / CPD algorithm and key mode as required */
        if ( pSettings->dest == NEXUS_SecurityAlgorithmConfigDestination_eCps )
            configAlgorithmIO.ekeyDestBlckType = BHSM_KeyDestBlockType_eCPS;
        else if ( pSettings->dest == NEXUS_SecurityAlgorithmConfigDestination_eCpd )
            configAlgorithmIO.ekeyDestBlckType = BHSM_KeyDestBlockType_eCPD;
        else
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);

        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bGlobalEnable = pSettings->bGlobalEnable? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bRestrictEnable = pSettings->bRestrictEnable ? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bGlobalScMod = pSettings->modifyScValue[NEXUS_SecurityPacketType_eGlobal]? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bRestrictScMod = pSettings->modifyScValue[NEXUS_SecurityPacketType_eRestricted]? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.uScValue = pSettings->scValue[NEXUS_SecurityPacketType_eGlobal];

        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bAtscScrambleCtrl= pSettings->bAtscModEnable ? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bAtscMod = pSettings->bScAtscMode ? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bGlobalDropPktCtrl = pSettings->bGlobalDropPktEnable? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bRestrictDropPktCtrl = pSettings->bRestrictDropPktEnable? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bGlobalRegOverwrite = pSettings->bGlobalRegionOverwrite? 1:0;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.bEncryptBeforeRave = pSettings->bEncryptBeforeRave? 1:0;

        /*Configure Odd key Algorithm*/
        configAlgorithmIO.eKeyType = BHSM_KeyType_eOddKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCpKeyOdd1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);

        /*Configure Even key Algorithm*/
        configAlgorithmIO.eKeyType = BHSM_KeyType_eEvenKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCpKeyEven1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);

        /*Configure Clear key Algorithm*/
        configAlgorithmIO.eKeyType = BHSM_KeyType_eClearKey;
        configAlgorithmIO.cryptoAlg.cacpCryptAlg.eKeyOffset = BHSM_KeyAddr_eCpKeyOdd1;
        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);
    }
    else if (keyHandle->cryptoEngine == NEXUS_SecurityEngine_eM2m) {
        BDBG_MSG(("Configure M2M Algorithm"));

        configAlgorithmIO.unKeySlot = keyHandle->keySlotNumber;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.m2mSecAlg = NEXUS_Security_MapNexusAlgorithmToHsm(pSettings->algorithm);
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.cipherDVBMode = pSettings->algorithmVar;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.ucAESCounterKeyMode = pSettings->terminationMode;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.IVMode = pSettings->ivMode;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.solitaryMode = pSettings->solitarySelect;

        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bUseExtKey = pSettings->enableExtKey? 1:0;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bUseExtIV = pSettings->enableExtIv? 1:0;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bEnableTimestamp = pSettings->enableTimestamps ? 1:0;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bDisallow_GG = pSettings->bDisallowGG ? 1:0;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bDisallow_GR = pSettings->bDisallowGR ? 1:0;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bDisallow_RG = pSettings->bDisallowRG ? 1:0;
        configAlgorithmIO.cryptoAlg.m2mCryptAlg.bDisallow_RR = pSettings->bDisallowRR ? 1:0;

        configAlgorithmIO.ekeyDestBlckType = BHSM_KeyDestBlockType_eM2M;

        switch( pSettings->operation ) {
        case NEXUS_SecurityOperation_eEncrypt:
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.ucAuthCtrl = BHSM_M2mAuthCtrl_eScramble;
            break;

        case NEXUS_SecurityOperation_eDecrypt:
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.ucAuthCtrl = BHSM_M2mAuthCtrl_eDescramble;
            break;

        case NEXUS_SecurityOperation_ePassThrough:
            configAlgorithmIO.cryptoAlg.m2mCryptAlg.ucAuthCtrl = BHSM_M2mAuthCtrl_ePassThrough;
            break;

        default:
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        configAlgorithmIO.cryptoAlg.m2mCryptAlg.eKeyOffset = BHSM_KeyAddr_eM2MKey1;

        rc = BHSM_ConfigAlgorithm( g_security.hsm, &configAlgorithmIO);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }


    return rc;
}

void NEXUS_Security_GetDefaultClearKey( NEXUS_SecurityClearKey *pClearKey )
{
    BKNI_Memset(pClearKey, 0, sizeof(*pClearKey));
}

NEXUS_Error NEXUS_Security_LoadClearKey(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityClearKey * pClearKey)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHSM_LoadCwIvIO_t       loadCwIvIO;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);

    if ( pClearKey->keySize != 8 && pClearKey->keySize != 16  ) {
        BDBG_ERR(("Key Size = %d is invalid", pClearKey->keySize ));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if ( ( keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCa ) ||
         ( keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCaCp && pClearKey->dest == NEXUS_SecurityAlgorithmConfigDestination_eCa) )
    {
        loadCwIvIO.eKeyLadderModule = BHSM_KeyLadderModule_eSecurity;
        loadCwIvIO.unKeySize = pClearKey->keySize;

        BKNI_Memcpy(loadCwIvIO.aucCwIv , pClearKey->keyData, pClearKey->keySize);

        loadCwIvIO.unKeySlot = keyHandle->keySlotNumber;

        switch(pClearKey->keyEntryType ){
            case NEXUS_SecurityKeyType_eOdd:
                loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eCaKeyOdd1;
                break;

            case NEXUS_SecurityKeyType_eEven:
                loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eCaKeyEven1;
                break;

            case NEXUS_SecurityKeyType_eClear:
                loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eCaKeyOdd1;
                break;

            case NEXUS_SecurityKeyType_eIv:
                loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eIva;
                break;

            default:
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        loadCwIvIO.eOperation = BHSM_Operation_eCA;
        rc = BHSM_LoadCwIv(g_security.hsm, &loadCwIvIO);
        if (rc) return BERR_TRACE(rc);
    }
    else if ( (keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCp) ||
              ( keyHandle->cryptoEngine == NEXUS_SecurityEngine_eCaCp &&
                ( pClearKey->dest == NEXUS_SecurityAlgorithmConfigDestination_eCps || pClearKey->dest == NEXUS_SecurityAlgorithmConfigDestination_eCpd )))
    {
        loadCwIvIO.eKeyLadderModule = BHSM_KeyLadderModule_eSecurity;
        loadCwIvIO.unKeySize = pClearKey->keySize;

        BKNI_Memcpy(loadCwIvIO.aucCwIv , pClearKey->keyData, pClearKey->keySize);

        loadCwIvIO.unKeySlot = keyHandle->keySlotNumber;

        switch(pClearKey->keyEntryType ){
            case NEXUS_SecurityKeyType_eOdd:
                loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eCpKeyOdd1;
                break;

            case NEXUS_SecurityKeyType_eEven:
                loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eCpKeyEven1;
                break;

            case NEXUS_SecurityKeyType_eClear:
                loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eCpKeyOdd1;
                break;

            case NEXUS_SecurityKeyType_eIv:
                loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eIva;
                break;

            default:
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        loadCwIvIO.eOperation = BHSM_Operation_eCP;
        rc = BHSM_LoadCwIv (g_security.hsm, &loadCwIvIO);
        if (rc) return BERR_TRACE(rc);
    }
    else if (keyHandle->cryptoEngine == NEXUS_SecurityEngine_eM2m) {
        loadCwIvIO.eKeyLadderModule = BHSM_KeyLadderModule_eMmscram;
        loadCwIvIO.unKeySize = pClearKey->keySize;

        BKNI_Memcpy(loadCwIvIO.aucCwIv , pClearKey->keyData, pClearKey->keySize);

        loadCwIvIO.unKeySlot = keyHandle->keySlotNumber;

        switch(pClearKey->keyEntryType ){
        case NEXUS_SecurityKeyType_eIv:
            loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eM2MIva;
            break;

        default:
            loadCwIvIO.eKeyAddr = BHSM_KeyAddr_eM2MKey1;
            break;
        }

        loadCwIvIO.eOperation = BHSM_Operation_eCP;
        rc = BHSM_LoadCwIv (g_security.hsm, &loadCwIvIO);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    return NEXUS_SUCCESS;
}


NEXUS_KeySlotHandle NEXUS_Security_AllocateKeySlot( const NEXUS_SecurityKeySlotSettings *pSettings )
{
    NEXUS_KeySlotHandle pKeyHandle = NULL;

    if (!pSettings) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    switch (pSettings->keySlotEngine) {
        case NEXUS_SecurityEngine_eCa:
        case NEXUS_SecurityEngine_eCp:
        case NEXUS_SecurityEngine_eCaCp:
            BDBG_MSG(("Allocating CA (or other) keyslot"));
            if (NEXUS_Security_AllocateKeySlotForType(&pKeyHandle,pSettings->keySlotEngine))
                return NULL;
            break;

        case NEXUS_SecurityEngine_eM2m:
            BDBG_MSG(("Allocating M2M keyslot"));
            if (NEXUS_Security_AllocateM2mKeySlot(&pKeyHandle))
                return NULL;
            break;

/*
        case NEXUS_SecurityEngine_eRmx:
            BDBG_MSG(("Allocating RMX keyslot"));
            if (NEXUS_Security_AllocateKeySlotForType(&pKeyHandle,pSettings->keySlotEngine,BCMD_XptSecKeySlot_eType6))
                return NULL;
            break;
*/

        default:
            BDBG_ERR(("Keyslot type (%d) is not currently supported !", pSettings->keySlotEngine ));
            return NULL;
    }

    return pKeyHandle;
}

void NEXUS_Security_FreeKeySlot( NEXUS_KeySlotHandle keyHandle )
{
    BERR_Code rc = 0;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);
    switch (keyHandle->cryptoEngine)
    {
        case NEXUS_SecurityEngine_eCa:
        case NEXUS_SecurityEngine_eCp:
        case NEXUS_SecurityEngine_eCaCp:
            BDBG_MSG(("Freeing CA/CP keyslot (%d)",keyHandle->keySlotNumber));
	        rc = BHSM_FreeCAKeySlot(g_security.hsm, keyHandle->keySlotNumber);
            if (rc)
                BDBG_ERR(("Error (%d) freeing CA keyslot",rc));
            break;

        case NEXUS_SecurityEngine_eM2m:
            BDBG_MSG(("Freeing m2m keyslot (%d)",keyHandle->keySlotNumber));
            rc = BHSM_FreeM2MKeySlot(g_security.hsm, keyHandle->keySlotNumber);
            if (rc)
                BDBG_ERR(("Error (%d) freeing M2M keyslot",rc));
            break;

        default:
            BDBG_ERR(("Keyslot type (%d) is not currently supported !",keyHandle->cryptoEngine ));
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            break;
    }

    if (!rc)
        NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHsm, false);

    NEXUS_P_Core_FreeKeySlotHandle(keyHandle);
}

void NEXUS_Security_GetDefaultMulti2Settings( NEXUS_SecurityMulti2Settings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error NEXUS_Security_ConfigMulti2( NEXUS_KeySlotHandle keyHandle, const NEXUS_SecurityMulti2Settings *pSettings )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHSM_ConfigMulti2IO_t config_multi2;

    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);

    BKNI_Memset(&config_multi2, 0, sizeof(config_multi2));
    config_multi2.ucMulti2RndCnt = pSettings->multi2Rounds;
    BKNI_Memcpy(config_multi2.aucSysKey, pSettings->multi2SysKey, BHSM_MULTI2_SYS_KEY_SIZE);
    config_multi2.ucSysKeyDest = pSettings->multi2KeySelect;
    rc = BHSM_ConfigMulti2(g_security.hsm, &config_multi2);
    if (rc) return BERR_TRACE(rc);

    return rc;
}

NEXUS_Error NEXUS_SecurityModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(enabled);

    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(enabled);

    return NEXUS_SUCCESS;
#endif    
}

/* These functions are dangerous and not meant for general use. It is needed for
test code that needs the HSM handle. It must be extern'd. */
void b_get_hsm(BHSM_Handle *hsm)
{
    *hsm = g_security.hsm;
}
void b_get_reg(BREG_Handle *reg)
{
    *reg = g_pCoreHandles->reg;
}
void b_get_int(BINT_Handle *bint)
{
    *bint = g_pCoreHandles->bint;
}
void b_get_chp(BCHP_Handle *chp)
{
    *chp = g_pCoreHandles->chp;
}
