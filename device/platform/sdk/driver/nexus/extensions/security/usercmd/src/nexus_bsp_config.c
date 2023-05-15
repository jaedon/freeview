/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_bsp_config.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/29/11 7:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/security/usercmd/7422/src/nexus_bsp_config.c $
 * 
 * 2   6/29/11 7:24p atruong
 * SW7425-812:HSM PI API modification for new HMAC-SHA support
 * 
 * 1   1/28/11 7:31p atruong
 * SW7422-235: Enable Nexus Security extension modules for 97422
 *
 ******************************************************************************/


#include "nexus_security_module.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "priv/nexus_security_priv.h"
#include "priv/nexus_core.h"

#include "nexus_bsp_config.h"

#include "bhsm.h"
#include "bhsm_misc.h"

BDBG_MODULE(nexus_bsp_config);

NEXUS_Error NEXUS_Security_RaveControl(
    const NEXUS_SecurityRaveCtrlSettings *pSettings
    )
{
    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_SetMiscBitsIO_t		setMiscBitsIO;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;
    /*
      * formulate the request structure
     */
	
    setMiscBitsIO.setMiscBitsSubCmd     = BCMD_SetMiscBitsSubCmd_eRaveBits;
	setMiscBitsIO.bDisableClear         = pSettings->bDisableClear;
	setMiscBitsIO.bRAVEEncryptionBypass = pSettings->bRAVEEncryptionBypass;
	setMiscBitsIO.bEnableReadDMem       = pSettings->bEnableReadDMem;
	setMiscBitsIO.bEnableReadIMem       = pSettings->bEnableReadIMem;
	setMiscBitsIO.bEnableWriteIMem      = pSettings->bEnableWriteIMem;

    rc = BHSM_SetMiscBits(hHsm, &setMiscBitsIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_SetMiscBits() for NEXUS_Security_RaveControl\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (setMiscBitsIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Security_DMABypassControl(
	const NEXUS_SecurityDMABypassSettings* pSettings
    )
{
    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_SetMiscBitsIO_t		setMiscBitsIO;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;
    /*
      * formulate the request structure
     */
	
    setMiscBitsIO.setMiscBitsSubCmd         = BCMD_SetMiscBitsSubCmd_eReserved1;
	setMiscBitsIO.bDMAM2MPacketBypassEnable = pSettings->bDMAM2MPacketBypassEnable;
	setMiscBitsIO.bDMAXPTPacketBypassEnable = pSettings->bDMAXPTPacketBypassEnable;
	setMiscBitsIO.bNMAPacketBypassEnable    = pSettings->bNMAPacketBypassEnable;

    rc = BHSM_SetMiscBits(hHsm, &setMiscBitsIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_SetMiscBits() for NEXUS_Security_DMABypassControl\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (setMiscBitsIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Security_M2MEndianSwapControl(
	const NEXUS_SecurityM2MEndianSwapSettings* pSettings
    )
{
    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_SetMiscBitsIO_t		setMiscBitsIO;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;
    /*
      * formulate the request structure
     */
	
    setMiscBitsIO.setMiscBitsSubCmd         = BCMD_SetMiscBitsSubCmd_eM2MEndianSwapBits;
	setMiscBitsIO.bCBCMACDataInSwapEnable   = pSettings->bCBCMACDataInSwapEnable;
	setMiscBitsIO.bCBCMACDataOutSwapEnable  = pSettings->bCBCMACDataOutSwapEnable;
	setMiscBitsIO.bCBCMACKeySwapEnable      = pSettings->bCBCMACKeySwapEnable;

    rc = BHSM_SetMiscBits(hHsm, &setMiscBitsIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_SetMiscBits() for NEXUS_Security_M2MEndianSwapControl\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (setMiscBitsIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_Security_ForceSCControl(
    const NEXUS_SecurityForceSCSettings *pSettings
    )

{
    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_SetMiscBitsIO_t		setMiscBitsIO;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;
    /*
      * formulate the request structure
     */
	
    setMiscBitsIO.setMiscBitsSubCmd         = BCMD_SetMiscBitsSubCmd_eForceSCBit;
	setMiscBitsIO.bBandSelect_PB            = pSettings->bBandSelect_PB;
	setMiscBitsIO.bandNumber                = pSettings->bandNumber;
	setMiscBitsIO.bSCForceEnAll             = pSettings->bSCForceEnAll;
	setMiscBitsIO.bSCForceEnNZ              = pSettings->bSCForceEnNZ;
	setMiscBitsIO.SCForceEnValue            = pSettings->SCForceEnValue;

    rc = BHSM_SetMiscBits(hHsm, &setMiscBitsIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_SetMiscBits() for NEXUS_Security_ForceSCControl\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (setMiscBitsIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_Security_XPTM2MStatusQuery(
    NEXUS_SecurityXPTM2MStatusOutput *pStatusQuery  
    )


{
    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_SetMiscBitsIO_t		setMiscBitsIO;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;
    /*
      * formulate the request structure
     */
	
    setMiscBitsIO.setMiscBitsSubCmd         = BCMD_SetMiscBitsSubCmd_eXPTM2MStatusDump;

    rc = BHSM_SetMiscBits(hHsm, &setMiscBitsIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_SetMiscBits() for NEXUS_Security_XPTM2MStatusQuery\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (setMiscBitsIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }

	pStatusQuery->unM2MSecurityErrStatus = setMiscBitsIO.unM2MSecurityErrStatus;
	pStatusQuery->unXPTSecurityErrStatus = setMiscBitsIO.unXPTSecurityErrStatus;

    return NEXUS_SUCCESS;
}



NEXUS_Error NEXUS_Security_DefineSecureRegion(
    const NEXUS_SecuritySRegionSettings *pSettings
    )
{
    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_SetArchIO_t			setArchIO;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;

	/* Some error checking */
	if (pSettings->sRegStartAddress & 0x0007)
	{
        BDBG_ERR(("sRegStartAddress has to be an o-word (8-byte aligned) address\n"));
        return NEXUS_INVALID_PARAMETER;
	}
	if (pSettings->sRegEndAddress & 0x0007)
	{
        BDBG_ERR(("sRegEndAddress has to be an o-word (8-byte aligned) address\n"));
        return NEXUS_INVALID_PARAMETER;
	}

#ifdef NEXUS_SECURITY_MSIPTV
	if ( (pSettings->archType == NEXUS_SecurityArchSelect_eSel02) && (pSettings->DRAMType == NEXUS_SecurityDRAMSelect_eSel00))
	{
		NEXUS_Security_MsiptvSetSecureRegion (pSettings->sRegStartAddress, pSettings->sRegEndAddress);
	}
#endif
    /*
      * formulate the request structure
     */
	
	setArchIO.unLowerRangeAddress = pSettings->sRegStartAddress>>3;
	setArchIO.unUpperRangeAddress = pSettings->sRegEndAddress>>3;
	setArchIO.ArchSel             = pSettings->archType;
	setArchIO.PCIArch             = pSettings->pciArchType;
	setArchIO.DRAMSel             = pSettings->DRAMType;
    
	rc = BHSM_SetArch(hHsm, &setArchIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_SetArch() for NEXUS_Security_DefineSecureRegion\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (setArchIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }


    return NEXUS_SUCCESS;

}



NEXUS_Error NEXUS_Security_AVDSRegistersSetUp(
		const NEXUS_SecurityAVDSRegRangeSettings   *pSettings
	)

{

    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_SetVichRegParIO_t		setVichRegParIO;
	uint32_t					index;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;

	/* Some error checking */
	if (pSettings->nRange > NEXUS_MAX_AVDSVD_RANGE)
	{
        BDBG_ERR(("nRange can not exceed %d\n", NEXUS_MAX_AVDSVD_RANGE));
        return NEXUS_INVALID_PARAMETER;
	}

    /*
      * formulate the request structure
     */
	setVichRegParIO.virtualKeyLadderID = pSettings->vkl;
	setVichRegParIO.keyLayer           = pSettings->keyLayer;
	setVichRegParIO.nRanges            = pSettings->nRange;
	setVichRegParIO.VDECId             = pSettings->VDECId;
	for (index = 0; index < pSettings->nRange; index++)
	{
		setVichRegParIO.unRangeLo[index] = pSettings->loRange[index];
		setVichRegParIO.unRangeHi[index] = pSettings->hiRange[index];

	}
	/* Last part is the signature */
	BKNI_Memcpy((void *)setVichRegParIO.aucSignature, 
				(void *)pSettings->signature, 
				NEXUS_HMACSHA256_SIGNATURE_SIZE);
	
    rc = BHSM_SetVichRegPar(hHsm, &setVichRegParIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_SetVichRegPar() for NEXUS_Security_AVDSRegistersSetUp\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (setVichRegParIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }

    return NEXUS_SUCCESS;


}


NEXUS_Error NEXUS_Security_AVDSRegistersModify(
	const NEXUS_SecurityAVDSRegModifySettings   *pSettings
	)

{

    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_StartAVDIO_t			startAVDIO;
	uint32_t					index;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;

	/* Some error checking */
	if (pSettings->nAVDReg > NEXUS_MAX_AVDSVD_RANGE)
	{
        BDBG_ERR(("nAVDReg can not exceed %d\n", NEXUS_MAX_AVDSVD_RANGE));
        return NEXUS_INVALID_PARAMETER;
	}

    /*
      * formulate the request structure
     */
	startAVDIO.virtualKeyLadderID = pSettings->vkl;
	startAVDIO.keyLayer           = pSettings->keyLayer;
	startAVDIO.avdID              = pSettings->avdID;
	startAVDIO.numAVDReg          = pSettings->nAVDReg;
	for (index = 0; index < pSettings->nAVDReg; index++)
	{
		startAVDIO.avdRegAddr[index] = pSettings->regAddr[index];
		startAVDIO.avdRegVal[index]  = pSettings->regVal[index];

	}
	/* Last part is the signature */
	BKNI_Memcpy((void *)startAVDIO.aucSignature, 
				(void *)pSettings->signature, 
				NEXUS_HMACSHA256_SIGNATURE_SIZE);
	
    rc = BHSM_StartAVD(hHsm, &startAVDIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_StartAVD() for NEXUS_Security_AVDSRegistersModify\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (startAVDIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }

    return NEXUS_SUCCESS;


}



