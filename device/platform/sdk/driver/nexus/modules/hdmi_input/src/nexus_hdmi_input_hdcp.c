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
 * $brcm_Workfile: nexus_hdmi_input_hdcp.c $
 * $brcm_Revision: 22 $
 * $brcm_Date: 7/9/12 10:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/7425/src/nexus_hdmi_input_hdcp.c $
 * 
 * 22   7/9/12 10:25a rgreen
 * SW7425-1988: Remove temp fix for coverity
 * 
 * 21   7/5/12 10:51p rgreen
 * SW7425-1988: Temp fix for compilation error for autobuild
 * 
 * 20   4/18/12 1:17p rgreen
 * SW7425-1988: Schedule hdcpRxChanged task during isr time rather than
 *  setting/waiting for an event during task time
 * 
 * 19   2/10/12 10:39a rgreen
 * SW7425-2226: Return NOT_AVAILABLE error if HSM support has not been
 *  compiled in
 * 
 * 18   11/2/11 4:25p rgreen
 * SW7425-1351: Always attempt to load encrypted Keyset even if another
 *  Keyset has already been loaded;  Add error trace macro
 * 
 * 17   10/17/11 5:52p rgreen
 * SW7425-1351: Use new HSM command ID to identify keyloading to use.
 *  Remove references to unused OTP ROM ; return success and print error
 *  traces when HDCP support is not available
 * 
 * 16   5/6/11 4:20p rgreen
 * SW7422-186: Add API stubs for disabled security
 * 
 * 15   4/20/11 6:17p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 14   4/18/11 3:45p rgreen
 * SW7422-186: Update hdcpRxChange callback name
 * 
 * 13   4/18/11 1:51p rgreen
 * SW7422-186:  Merge HDMI HDCP Repeater Support
 * 
 * SW7422-186/7   4/15/11 6:08p rgreen
 * SW7422-186: Remove hdcpKeyset from hdmiInputSettings; Add to  hdmiInput
 *  handle
 * 
 * SW7422-186/6   4/15/11 2:56p rgreen
 * SW7422-186: Updates from API Review
 * 
 * SW7422-186/5   4/13/11 1:55p rgreen
 * SW7422-186: Replace RequestKsvFifoEvent with generic hdcpEvent
 * 
 * SW7422-186/4   4/11/11 2:55p rgreen
 * SW7422-186: Add  HDMI Repeater support.
 * 
 * SW7422-186/3   4/7/11 2:38p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * SW7422-186/2   3/31/11 2:38p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * SW7422-186/1   3/28/11 5:38p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * 12   3/7/11 6:11p rgreen
 * SWDTV-5816: Fix 35230 compilation warning
 * 
 * 11   3/4/11 4:05p rgreen
 * SWDTV-4664,SW7422-129: return status for BHDR_HDCP_Enable/Disable Key
 *  loading Move contents of BHDR_HDCP_CheckKeyset functionality to
 *  BHDR_HDCP_DisableKeyLoading which will check key load results
 * 
 * 10   3/3/11 4:45p rgreen
 * SWDTV-4664: Temp Fix for 35230 compilation
 * 
 * 9   3/3/11 1:25p rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 *  OTP ROM based chips
 * 
 * 8   3/3/11 1:04p rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 *  OTP ROM based chips
 * 
 * 7   2/25/11 5:43p rgreen
 * SW7422-129: Use compilation macro to enable/disable HDCP Key loading
 *  support based on HSM availability
 * 
 * 6   2/24/11 8:21p rgreen
 * SW7422-129: Make call to Check KeySet after load is complete
 * 
 * 5   2/17/11 1:11a rgreen
 * SW7422-129: Fix compilation if key ladder is disabled
 * 
 * 4   2/16/11 6:20p rgreen
 * SW7422-129,SW35230-2374: Merge changes
 * 
 * SW7422-129/4   2/10/11 3:49p rgreen
 * SW7422-129: Remove hdcpSettings from hdmiInputSettings; create separate
 *  hdcpSettings.  Add Test Key Set from HDCP Spec for GetDefaultSettings
 * 
 * SW7422-129/3   2/9/11 11:07a rgreen
 * SW7422-129: Fix compilation warning
 * 
 * SW7422-129/2   2/9/11 10:52a rgreen
 * SW7422-129,SW35230-2374: Update HDCP Key Processing/Status; distinguish
 *  between OTP RAM and OTP ROM
 * 
 * SW7422-129/1   2/2/11 6:27p rgreen
 * SW7422-129: Add APIs for HDCP Key processing/status
 * 
 * 3   2/1/11 2:52p rgreen
 * SW7422-129: temp fix for 7425 compilation
 * 
 * 2   12/23/10 2:26p rgreen
 * SW35230-2374: Update naming convention for
 *  NEXUS_HdmiInputHdcpKeySetOtpStatus
 * 
 * 1   12/17/10 4:44p rgreen
 * SW35230-2374: Add separate files for HDCP related functions
 * 
 **************************************************************************/
#include "nexus_hdmi_input_module.h"
#include "nexus_hdmi_input_hdcp.h"
#include "bhdr_hdcp.h"

#if BHDR_CONFIG_HDCP_KEY_OTP_RAM  && BHSM_SECURE_PI_SUPPORT_KEYLADDER
#include "bhsm_keyladder_enc.h"
#endif


BDBG_MODULE(nexus_hdmi_input_hdcp);

#if NEXUS_HAS_SECURITY
#include "priv/nexus_security_priv.h"

#define LOCK_SECURITY() NEXUS_Module_Lock(g_NEXUS_hdmiInputModuleSettings.modules.security)
#define UNLOCK_SECURITY() NEXUS_Module_Unlock(g_NEXUS_hdmiInputModuleSettings.modules.security)


#if !NEXUS_NUM_HDMI_INPUTS
#error Platform must define NEXUS_NUM_HDMI_INPUTS
#endif



extern NEXUS_ModuleHandle g_NEXUS_hdmiInputModule;
extern NEXUS_HdmiInputModuleSettings g_NEXUS_hdmiInputModuleSettings;


void NEXUS_HdmiInput_P_HdcpStateChange_isr(void *context, int param2, void *data)
{
    NEXUS_HdmiInputHandle hdmiInput ;
    BSTD_UNUSED(param2) ;
    BSTD_UNUSED(data) ;	

    hdmiInput = (NEXUS_HdmiInputHandle) context;
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

    NEXUS_IsrCallback_Fire_isr(hdmiInput->hdcpRxChanged) ;
}

static NEXUS_Error NEXUS_HdmiInput_P_HdcpKeyLoad(NEXUS_HdmiInputHandle hdmiInput)
{

    NEXUS_Error errCode = NEXUS_SUCCESS ;
#if BHDR_CONFIG_HDCP_KEY_OTP_RAM && BHSM_SECURE_PI_SUPPORT_KEYLADDER
    BHSM_Handle hHsm ;	
    BHSM_GenerateRouteKeyIO_t		generateRouteKeyIO;
    uint32_t KeyParameter ;
    
    uint8_t i;
    
    BDBG_ASSERT(g_NEXUS_hdmiInputModuleSettings.modules.security);
    
    LOCK_SECURITY();

        NEXUS_Security_GetHsm_priv(&hHsm);	
        
        BKNI_Memset( &generateRouteKeyIO, 0, sizeof(BHSM_GenerateRouteKeyIO_t))  ;	
        
        /* generate Key3 from custom key */
        generateRouteKeyIO.bASKMModeEnabled = 0;
#if HSM_IS_ASKM_40NM_ZEUS_2_0
        generateRouteKeyIO.subCmdID   = BCMD_VKLAssociationQueryFlag_eNoQuery;
#else
        generateRouteKeyIO.subCmdID   = BCMD_KeyGenSubCmdID_eKeyGen;
#endif
        generateRouteKeyIO.rootKeySrc =  BCMD_RootKeySrc_eCusKey; 
        generateRouteKeyIO.customerSubMode = BCMD_CustomerSubMode_eGeneralPurpose1;
        
        generateRouteKeyIO.ucSwizzle1Index = 0;
        generateRouteKeyIO.swizzleType = BCMD_SwizzleType_eSwizzle0;
        
        generateRouteKeyIO.bUseCustKeyHighDecrypt = false ;		
        generateRouteKeyIO.bUseCustKeyLowDecrypt =	false ;
        
        generateRouteKeyIO.virtualKeyLadderID = BCMD_VKL0;
        generateRouteKeyIO.keyLayer = BCMD_KeyRamBuf_eKey3 ;
        
        generateRouteKeyIO.key3Op				   = BCMD_Key3Op_eKey3NoProcess;
        generateRouteKeyIO.bIsKeyLadder3DESEncrypt = false;
        generateRouteKeyIO.bSwapAesKey			   = false;
        
        generateRouteKeyIO.keyLadderType = BCMD_KeyLadderType_e3DESABA;
        generateRouteKeyIO.keySize= BCMD_KeySize_e64;
        generateRouteKeyIO.ucKeyDataLen = 8 ;
        
        generateRouteKeyIO.bIsRouteKeyRequired = true;
        generateRouteKeyIO.keyDestBlckType  = BCMD_KeyDestBlockType_eHdmi;
        generateRouteKeyIO.keyDestEntryType = BCMD_KeyDestEntryType_eOddKey;
        generateRouteKeyIO.keyDestIVType	= BCMD_KeyDestIVType_eNoIV;
        generateRouteKeyIO.caKeySlotType    = BCMD_XptSecKeySlot_eType0 ;
        generateRouteKeyIO.keyMode = BCMD_KeyMode_eRegular;

					
        BDBG_WRN(("Begin Loading Encrypted keyset...")) ;

        for (i = 0; i < NEXUS_HDMI_HDCP_NUM_KEYS  ; i++ )
        {
        	generateRouteKeyIO.keyLadderType = hdmiInput->hdcpKeyset.alg ;
        
        	generateRouteKeyIO.ucCustKeyLow = hdmiInput->hdcpKeyset.custKeySel ;
        	generateRouteKeyIO.ucKeyVarLow = hdmiInput->hdcpKeyset.custKeyVarL;
        	
        	generateRouteKeyIO.ucCustKeyHigh = hdmiInput->hdcpKeyset.custKeySel;	
        	generateRouteKeyIO.ucKeyVarHigh = hdmiInput->hdcpKeyset.custKeyVarH;	
        
        	BKNI_Memset(&generateRouteKeyIO.aucKeyData, 0, sizeof(generateRouteKeyIO.aucKeyData));				
        
        	KeyParameter = hdmiInput->hdcpKeyset.privateKey[i].hdcpKeyHi ;
        	generateRouteKeyIO.aucKeyData[0]  =  (KeyParameter >> 24) & 0xff ;
        	generateRouteKeyIO.aucKeyData[1]  =  (KeyParameter >> 16) & 0xff ;
        	generateRouteKeyIO.aucKeyData[2]  =  (KeyParameter >>  8) & 0xff ;
        	generateRouteKeyIO.aucKeyData[3]  =  (KeyParameter) 	  & 0xff ;
        	
        	KeyParameter = hdmiInput->hdcpKeyset.privateKey[i].hdcpKeyLo;
        	generateRouteKeyIO.aucKeyData[4]  =  (KeyParameter >> 24) & 0xff ;
        	generateRouteKeyIO.aucKeyData[5]  =  (KeyParameter >> 16) & 0xff ;
        	generateRouteKeyIO.aucKeyData[6]  =  (KeyParameter >>  8) & 0xff ;
        	generateRouteKeyIO.aucKeyData[7]  =  (KeyParameter) 	  & 0xff ;			
        	
        	generateRouteKeyIO.unKeySlotNum =	i ;
        	
        	errCode= BHSM_GenerateRouteKey (hHsm, &generateRouteKeyIO) ;	
        	BDBG_MSG(("generateRouteKeyIO key 3 unStatus = 0x%08X", generateRouteKeyIO.unStatus)) ;
        	if (errCode != 0) 
        	{
        		BDBG_ERR(("For Key #%02d of %d BHSM_GenerateRouteKey key3 errCode: %x", 
        			i + 1, BAVC_HDMI_HDCP_N_PRIVATE_KEYS, errCode));
			BERR_TRACE(errCode) ;
        		goto done;
        	}	
        	BDBG_MSG(("Loaded Encrypted Key  %02d of %d  %#08x%08x", 
        	    i + 1, BAVC_HDMI_HDCP_N_PRIVATE_KEYS, 
        	    hdmiInput->hdcpKeyset.privateKey[i].hdcpKeyHi,
        	    hdmiInput->hdcpKeyset.privateKey[i].hdcpKeyLo)) ;
        }
    	
        BDBG_WRN(("Done  Loading Encrypted keyset...")) ;

done:
    /* always unlock security module when done */
	UNLOCK_SECURITY();
#else
	BDBG_WRN(("**************************************************************")) ;
	BDBG_WRN(("BHSM KEYLADDER support required for hdmiInput HDCP Key Support")) ;
	BDBG_WRN(("**************************************************************")) ;
	errCode = NEXUS_NOT_AVAILABLE ;
    BSTD_UNUSED(hdmiInput) ;
#endif
    return errCode ;
}



/**
Summary:
Get HDCP Default Keyset
**/
void NEXUS_HdmiInput_HdcpGetDefaultKeyset(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpKeyset *pKeyset /* [out] */
    )
{
	static const uint8_t hdcpSpecB1RxBksv[NEXUS_HDMI_HDCP_KSV_LENGTH] =
		{0x14, 0xF7, 0x61, 0x03, 0xB7} ;
	
	static const 
		NEXUS_HdmiInputHdcpKey hdcpSpecB1RxKeySet[NEXUS_HDMI_HDCP_NUM_KEYS] =
	{	
			/* LSB.. MSB */
			{ 0, 0, 0, 0, 0x691e138f, 0x58a44d00},		{ 0, 0, 0, 0, 0x0950e658, 0x35821f00},
			{ 0, 0, 0, 0, 0x0d98b9ab, 0x476a8a00},		{ 0, 0, 0, 0, 0xcac5cb52, 0x1b18f300},
			{ 0, 0, 0, 0, 0xb4d89668, 0x7f14fb00},		{ 0, 0, 0, 0, 0x818f4878, 0xc98be000},
			{ 0, 0, 0, 0, 0x412c11c8, 0x64d0a000},		{ 0, 0, 0, 0, 0x44202428, 0x5a9db300},
			{ 0, 0, 0, 0, 0x6b56adbd, 0xb228b900},		{ 0, 0, 0, 0, 0xf6e46c4a, 0x7ba49100},
			{ 0, 0, 0, 0, 0x589d5e20, 0xf8005600},		{ 0, 0, 0, 0, 0xa03fee06, 0xb77f8c00},
			{ 0, 0, 0, 0, 0x28bc7c9d, 0x8c2dc000},		{ 0, 0, 0, 0, 0x059f4be5, 0x61125600},
			{ 0, 0, 0, 0, 0xcbc1ca8c, 0xdef07400},		{ 0, 0, 0, 0, 0x6adbfc0e, 0xf6b83b00},
			{ 0, 0, 0, 0, 0xd72fb216, 0xbb2ba000},		{ 0, 0, 0, 0, 0x98547846, 0x8e2f4800},
			{ 0, 0, 0, 0, 0x38472762, 0x25ae6600},		{ 0, 0, 0, 0, 0xf2dd23a3, 0x52493d00},
			{ 0, 0, 0, 0, 0x543a7b76, 0x31d2e200},		{ 0, 0, 0, 0, 0x2561e6ed, 0x1a584d00},
			{ 0, 0, 0, 0, 0xf7227bbf, 0x82603200},		{ 0, 0, 0, 0, 0x6bce3035, 0x461bf600},
			{ 0, 0, 0, 0, 0x6b97d7f0, 0x09043600},		{ 0, 0, 0, 0, 0xf9498d61, 0x05e1a100}, 
			{ 0, 0, 0, 0, 0x063405d1, 0x9d8ec900},		{ 0, 0, 0, 0, 0x90614294, 0x67c32000},		
			{ 0, 0, 0, 0, 0xc34facce, 0x51449600},		{ 0, 0, 0, 0, 0x8a8ce104, 0x45903e00},		
			{ 0, 0, 0, 0, 0xfc2d9c57, 0x10002900},		{ 0, 0, 0, 0, 0x80b1e569, 0x3b94d700},		
			{ 0, 0, 0, 0, 0x437bdd5b, 0xeac75400},		{ 0, 0, 0, 0, 0xba90c787, 0x58fb7400},		
			{ 0, 0, 0, 0, 0xe01d4e36, 0xfa5c9300},		{ 0, 0, 0, 0, 0xae119a15, 0x5e070300},
			{ 0, 0, 0, 0, 0x01fb788a, 0x40d30500},		{ 0, 0, 0, 0, 0xb34da0d7, 0xa5590000},
			{ 0, 0, 0, 0, 0x409e2c4a, 0x633b3700},		{ 0, 0, 0, 0, 0x412056b4, 0xbb732500}
	} ;


    BSTD_UNUSED(hdmiInput);

    BKNI_Memset(pKeyset, 0, sizeof(NEXUS_HdmiInputHdcpKeyset)) ;
	
    /* following should be set by app */
    pKeyset->alg = 1 ;
    pKeyset->custKeySel = 0 ;
    pKeyset->custKeyVarH = 0 ;
    pKeyset->custKeyVarL = 0 ;

    /* copy HDCP Test Key Set from HDCP Specification  to default Key Set */
    /* !!! FYI This Test Key Set *IS NOT* compatible with production transmitter */ 
    BKNI_Memcpy(pKeyset->privateKey, hdcpSpecB1RxKeySet, 
        sizeof(NEXUS_HdmiInputHdcpKey) * NEXUS_HDMI_HDCP_NUM_KEYS) ;
    
    /* copy HDCP BKsv from HDCP Specification to default Bksv */
    BKNI_Memcpy(&pKeyset->rxBksv, hdcpSpecB1RxBksv, NEXUS_HDMI_HDCP_KSV_LENGTH) ;
    
}


/**
Summary:
Initialize/Load HDCP Key Set 
**/
NEXUS_Error NEXUS_HdmiInput_HdcpSetKeyset(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiInputHdcpKeyset *pKeyset
    )
{
    NEXUS_Error errCode = NEXUS_SUCCESS ;
    BHDR_HDCP_Status hdcpStatus ;
#if BHDR_CONFIG_HDCP_KEY_OTP_RAM
    BHDR_HDCP_Settings hdcpSettings ;
#endif

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

    /* Install callbacks */
    BDBG_ASSERT(pKeyset) ;

    BDBG_CASSERT(BAVC_HDMI_HDCP_KSV_LENGTH == NEXUS_HDMI_HDCP_KSV_LENGTH);
    BDBG_CASSERT(BAVC_HDMI_HDCP_N_PRIVATE_KEYS == NEXUS_HDMI_HDCP_NUM_KEYS);
    BDBG_CASSERT(sizeof(BHDR_HDCP_EncryptedKeyStruct) == sizeof(NEXUS_HdmiInputHdcpKey));


#if BHDR_CONFIG_HDCP_KEY_OTP_RAM
    /* update the HDCP BKSV value */
    BKNI_Memcpy(&hdmiInput->hdcpKeyset.rxBksv,
	    pKeyset->rxBksv.data, BAVC_HDMI_HDCP_KSV_LENGTH);

    /* update the HDCP Private keyset value */
    BKNI_Memcpy(hdmiInput->hdcpKeyset.privateKey,
	    pKeyset->privateKey, 
	    sizeof( NEXUS_HdmiInputHdcpKey) * NEXUS_HDMI_HDCP_NUM_KEYS) ;

    hdmiInput->hdcpKeyset.alg = pKeyset->alg ;
    hdmiInput->hdcpKeyset.custKeySel = pKeyset->custKeySel ;
    hdmiInput->hdcpKeyset.custKeyVarH = pKeyset->custKeyVarH ;
    hdmiInput->hdcpKeyset.custKeyVarL = pKeyset->custKeyVarL ;
#endif

    /* use storage type to determine load type */
    BHDR_HDCP_GetStatus(hdmiInput->hdr, &hdcpStatus) ;


#if BHDR_CONFIG_HDCP_KEY_OTP_RAM
    BHDR_HDCP_GetSettings(hdmiInput->hdr, &hdcpSettings) ;
        BKNI_Memcpy(&hdcpSettings.rxBksv, &hdmiInput->hdcpKeyset.rxBksv, 
            NEXUS_HDMI_HDCP_KSV_LENGTH) ;
    BHDR_HDCP_SetSettings(hdmiInput->hdr, &hdcpSettings) ;  
#endif

   /* always reload HDCP Keys whenever requested */

    BHDR_HDCP_EnableKeyLoading(hdmiInput->hdr) ;

    switch (hdcpStatus.eKeyStorage)
    {
    case NEXUS_HdmiInputHdcpKeyStorage_eOtpRAM :
        /* load Keyset into OTP RAM  */
        errCode = NEXUS_HdmiInput_P_HdcpKeyLoad(hdmiInput) ;
        break ;
    
    case NEXUS_HdmiInputHdcpKeyStorage_eOtpROM :
        /* keys are already loaded in OTP ROM; nothing to do */
        errCode = NEXUS_SUCCESS ;
        break ;
    
    default :
        errCode = NEXUS_UNKNOWN ;
        BDBG_ERR(("Unknown HDCP Key Storage... unable to load keyset")) ;
    }

    /* disable key loading if key loading was successful */
    if (!errCode) 
	    BHDR_HDCP_DisableKeyLoading(hdmiInput->hdr) ;

    return errCode ;
}




/**
Summary:
Get HDCP Default Settings
**/
void NEXUS_HdmiInput_HdcpGetDefaultSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpSettings *pSettings /* [out] */
    )
{
    BHDR_HDCP_Settings hdcpDefaultSettings ;
	
    BSTD_UNUSED(hdmiInput);

    BKNI_Memset(pSettings, 0, sizeof(NEXUS_HdmiInputHdcpSettings)) ;

    BHDR_HDCP_GetDefaultSettings(&hdcpDefaultSettings) ;

    BDBG_MSG(("Default HDCP Repeater Setting %d", hdcpDefaultSettings.bRepeater)) ;

    pSettings->repeater = hdcpDefaultSettings.bRepeater ;
    pSettings->maxDepthSupported = hdcpDefaultSettings.uiMaxLevels ;
    pSettings->maxDeviceCountSupported = hdcpDefaultSettings.uiMaxDevices ;
	
}


void NEXUS_HdmiInput_HdcpGetSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpSettings *pSettings /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = hdmiInput->hdcpSettings;
}


/**
Summary:
Set HDCP Settings
**/
NEXUS_Error NEXUS_HdmiInput_HdcpSetSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiInputHdcpSettings *pSettings
    )
{
    NEXUS_Error errCode = NEXUS_SUCCESS ;
    BERR_Code rc ;
    BHDR_HDCP_Status hdcpStatus ;
#if BHDR_CONFIG_HDCP_KEY_OTP_RAM
    BHDR_HDCP_Settings hdcpSettings ;
#endif

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_ASSERT(NULL != pSettings);

    /* Install callbacks */
    BDBG_ASSERT(pSettings) ;

    BDBG_CASSERT(BAVC_HDMI_HDCP_KSV_LENGTH == NEXUS_HDMI_HDCP_KSV_LENGTH);
    BDBG_CASSERT(BAVC_HDMI_HDCP_N_PRIVATE_KEYS == NEXUS_HDMI_HDCP_NUM_KEYS);
    BDBG_CASSERT(sizeof(BHDR_HDCP_EncryptedKeyStruct) == sizeof(NEXUS_HdmiInputHdcpKey));

    BKNI_Memset(&hdcpStatus, 0, sizeof(BHDR_HDCP_Status)) ;

    NEXUS_IsrCallback_Set(hdmiInput->hdcpRxChanged, &pSettings->hdcpRxChanged) ;

    rc = BHDR_HDCP_InstallHdcpStatusChangeCallback(hdmiInput->hdr,
    	NEXUS_HdmiInput_P_HdcpStateChange_isr, hdmiInput, 0);
    if (rc) {BERR_TRACE(rc); goto error;}


#if BHDR_CONFIG_HDCP_KEY_OTP_RAM
    /* enable key loading as needed */
    BHDR_HDCP_GetSettings(hdmiInput->hdr, &hdcpSettings) ;
        BKNI_Memcpy(&hdcpSettings.rxBksv, &hdmiInput->hdcpKeyset.rxBksv, 
            NEXUS_HDMI_HDCP_KSV_LENGTH) ;
        hdcpSettings.bRepeater = pSettings->repeater ;
    BHDR_HDCP_SetSettings(hdmiInput->hdr, &hdcpSettings) ;  
#endif

#if BHDR_CONFIG_HDCP_KEY_OTP_ROM
    /* if the case of OTP ROM ; if the keyset are already loaded skip loading again */
    BHDR_HDCP_GetStatus(hdmiInput->hdr, &hdcpStatus) ;
    if (hdcpStatus.eOtpState == (BHDR_HDCP_OtpState)NEXUS_HdmiInputHdcpKeySetOtpState_eCrcMatch)
    {
        BDBG_WRN(("HDCP keyset has already been loaded...")) ;

        /* make sure key loading is disabled for core */
        BHDR_HDCP_DisableKeyLoading(hdmiInput->hdr) ;
        errCode = NEXUS_SUCCESS ;
    }
    else
    {
        BDBG_WRN(("Verify HDCP Key Set...")) ;
        BHDR_HDCP_EnableKeyLoading(hdmiInput->hdr) ;
    }
#endif

    BKNI_Memcpy(&hdmiInput->hdcpSettings, pSettings, sizeof(NEXUS_HdmiInputHdcpSettings)) ;

error:
    return errCode ;
}


/**
Summary:
Get HDCP Status
**/
NEXUS_Error NEXUS_HdmiInput_HdcpGetStatus(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpStatus *pStatus /* [out] */
    )
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_ASSERT(NULL != pStatus);

    BDBG_CASSERT(BHDR_HDCP_OtpState_eMax == (BHDR_HDCP_OtpState)NEXUS_HdmiInputHdcpKeySetOtpState_eMax) ;
    BDBG_CASSERT(sizeof(*pStatus) == sizeof(BHDR_HDCP_Status)) ;

    errCode = BHDR_HDCP_GetStatus(hdmiInput->hdr, (BHDR_HDCP_Status *) pStatus) ;

    return errCode ;
}

/**
Summary:
Load Downstream KSV FIFO
**/
NEXUS_Error NEXUS_HdmiInput_HdcpLoadKsvFifo(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiHdcpDownStreamInfo *pDownstreamInfo,
    const NEXUS_HdmiHdcpKsv *pDownstreamKsvs, /* attr{nelem=numDevices} */
    unsigned numDevices
	)
{
    NEXUS_Error rc = NEXUS_SUCCESS ;
    BDBG_CASSERT(sizeof(BHDR_HDCP_RepeaterDownStreamInfo) == sizeof(NEXUS_HdmiHdcpDownStreamInfo)) ;

    /* warn of numDevices/ pDowstreamInfo inconsistency */
    if (numDevices != pDownstreamInfo->devices)
    {
    	BDBG_WRN((" !!! ")) ;
    	BDBG_WRN(("numDevices %d is not equal to pDownstreamInfo->devices %d",
			numDevices, pDownstreamInfo->devices)) ;
    	BDBG_WRN((" !!! ")) ;
    }

#if BHDR_CONFIG_HDCP_REPEATER
		BHDR_HDCP_LoadRepeaterKsvFifo(hdmiInput->hdr, 
			(BHDR_HDCP_RepeaterDownStreamInfo *) pDownstreamInfo, 
			(uint8_t *) pDownstreamKsvs) ;
#else
		BDBG_WRN(("HDCP Repeater Support is DISABLED!!!")) ;
		BSTD_UNUSED(hdmiInput) ;
		BSTD_UNUSED(pDownstreamInfo) ;
		BSTD_UNUSED(pDownstreamKsvs) ;
#endif

	return rc ;
}
/******************************************************/
#else

void NEXUS_HdmiInput_P_HdcpProcessChange(void *pContext)
{
	BSTD_UNUSED(pContext) ;
}


void NEXUS_HdmiInput_HdcpGetDefaultKeyset(
	NEXUS_HdmiInputHandle hdmiInput,
	NEXUS_HdmiInputHdcpKeyset *pKeyset /* [out] */
)
{
	BSTD_UNUSED(hdmiInput) ;
	BSTD_UNUSED(pKeyset) ;
}



NEXUS_Error NEXUS_HdmiInput_HdcpSetKeyset(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiInputHdcpKeyset *pKeyset
)
{
    BSTD_UNUSED(hdmiInput) ;
    BSTD_UNUSED(pKeyset) ;
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}


/* No security - use stubs for HDCP */
void NEXUS_HdmiInput_HdcpGetDefaultSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpSettings *pSettings /* [out] */
    )
{
    BSTD_UNUSED(hdmiInput);

    BKNI_Memset(pSettings, 0, sizeof(NEXUS_HdmiInputHdcpSettings)) ;
}


void NEXUS_HdmiInput_HdcpGetSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpSettings *pSettings /* [out] */
    )
{
    BSTD_UNUSED(hdmiInput);

    BKNI_Memset(pSettings, 0, sizeof(NEXUS_HdmiInputHdcpSettings)) ;
}

NEXUS_Error NEXUS_HdmiInput_HdcpSetSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiInputHdcpSettings *pSettings
    )
{
    BSTD_UNUSED(hdmiInput);
    BSTD_UNUSED(pSettings);

    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NEXUS_SUCCESS ;
}



NEXUS_Error NEXUS_HdmiInput_HdcpGetStatus(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpStatus *pStatus /* [out] */
    )
{
    BSTD_UNUSED(hdmiInput);

    BKNI_Memset(pStatus, 0, sizeof(NEXUS_HdmiInputHdcpStatus)) ;
    BERR_TRACE(BERR_NOT_SUPPORTED) ;
    return NEXUS_SUCCESS ;
}

NEXUS_Error NEXUS_HdmiInput_HdcpLoadKsvFifo(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiHdcpDownStreamInfo *pDownstreamInfo,
    const NEXUS_HdmiHdcpKsv *pDownstreamKsvs, /* attr{nelem=numDevices} */
    unsigned numDevices
	)
{
    BSTD_UNUSED(hdmiInput) ;
    BSTD_UNUSED(pDownstreamInfo) ;
    BSTD_UNUSED(pDownstreamKsvs) ;
    BSTD_UNUSED(numDevices) ;
 
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NEXUS_SUCCESS ;
}


#endif




