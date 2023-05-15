/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/*
	Viaccess Protection API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.115
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "va_def.h"

#include "va_prot.h"
#include "va_ui_adapt.h"

#include "vkernel.h"

#include "pal_output.h"

#include <string.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define __VIACCESS50__	1



/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/

#ifdef CONFIG_MW_CAS_VIACCESS_MACROVISION
static UINT8 s_bMacrovisionOn;
#endif

#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
static UINT8 s_bFingerprintOn;
static UINT8 s_szFingerprint[kVA_PROT_MAX_FINGERPRINTING_STRING_SIZE+1];
#endif

#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK 		// 이건 좀 확인해봐야 겠다... VA-4SD 하믄서 넣으라고 해서 넣긴 했는데..
static UINT8 s_bFreeChannelBlockOn;
#endif

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/


/*
	initialize
*/
void VA_PROT_Init()
{
	HxLOG_Print("[Enter]\n");

#ifdef CONFIG_MW_CAS_VIACCESS_MACROVISION
	s_bMacrovisionOn = FALSE;
#endif
#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
	s_bFingerprintOn = FALSE;
#endif
#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK
	s_bFreeChannelBlockOn = FALSE;
#endif

	HxLOG_Print("[Exit]\n");
}

/*
	exit
*/
void VA_PROT_Exit()
{
	HxLOG_Print("[Enter]\n");

	HxLOG_Print("[Exit]\n");
}


/*
	VA_PROT_StartMacrovision
		start macrovision
	arguments
		void
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_ALREADY_DONE
*/
INT VA_PROT_StartMacrovision()
{
	HxLOG_Info("[Enter]\n");

#ifdef CONFIG_MW_CAS_VIACCESS_MACROVISION

	// if Macrovision is already working
	if (s_bMacrovisionOn)
	{
		HxLOG_Info("kVA_ALREADY_DONE\n");
		return kVA_ALREADY_DONE;
	}

	PAL_OUT_SetMacrovision(eMACROVISION_AGC);

	s_bMacrovisionOn = TRUE;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#else

	HxLOG_Print("[Exit]\n");
	// return kVA_NOT_IMPLEMENTED;
	// return kVA_OK always. (by Corentin, Viaccess Engineer, ACS 3.1, 20100719)
	return kVA_OK;

#endif
}

/*
	VA_PROT_StopMacrovision
		stop macrovision
	arguments
		void
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_ALREADY_DONE
*/
INT VA_PROT_StopMacrovision()
{
	HxLOG_Info("[Enter]\n");

#ifdef CONFIG_MW_CAS_VIACCESS_MACROVISION

	// if Macrovision is already stopped
	if (s_bMacrovisionOn == FALSE)
	{
		HxLOG_Info("kVA_ALREADY_DONE\n");
		return kVA_ALREADY_DONE;
	}

	PAL_OUT_SetMacrovision(eMACROVISION_OFF);

	s_bMacrovisionOn = FALSE;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#else

	HxLOG_Print("[Exit]\n");
	// return kVA_NOT_IMPLEMENTED;
	// return kVA_OK always. (by Corentin, Viaccess Engineer, ACS 3.1, 20100719)
	return kVA_OK;

#endif
}

/*
	VA_PROT_StartFingerprinting
		start fingerprinting
	arguments
		pFingerPrintStr
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_ALREADY_DONE
		kVA_INVALID_PARAMETER
*/
INT VA_PROT_StartFingerprinting(char *pFingerPrintStr)
{
	HxLOG_Info("[Enter][%s]\n", pFingerPrintStr);

#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT

	// if fingerprinting is already working
	if (s_bFingerprintOn)
	{
		HxLOG_Info("kVA_ALREADY_DONE\n");
		return kVA_ALREADY_DONE;
	}

	if (pFingerPrintStr == NULL || strlen(pFingerPrintStr) > kVA_PROT_MAX_FINGERPRINTING_STRING_SIZE)
	{
		HxLOG_Error("kVA_INVALID_PARAMETER (%x)\n", pFingerPrintStr);
		return kVA_INVALID_PARAMETER;
	}

	// start fingerprinting
	s_bFingerprintOn = TRUE;
	VA_memcpy(s_szFingerprint, pFingerPrintStr, kVA_PROT_MAX_FINGERPRINTING_STRING_SIZE);
	VA_UI_Adapt_ShowFingerprint(s_szFingerprint);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#else

	HxLOG_Print("[Exit]\n");
	return kVA_NOT_IMPLEMENTED;

#endif
}

/*
	VA_PROT_StopFingerprinting
		stop fingerprinting
	arguments
		void
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_ALREADY_DONE
*/
INT VA_PROT_StopFingerprinting()
{
	HxLOG_Info("[Enter]\n");

#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT

	// if fingerprinting is already stopped
	if (s_bFingerprintOn == FALSE)
	{
		HxLOG_Info("kVA_ALREADY_DONE\n");
		return kVA_ALREADY_DONE;
	}

	// stop fingerprinting
	VA_UI_Adapt_HideFingerprint();
	s_bFingerprintOn = FALSE;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#else

	HxLOG_Print("[Exit]\n");
	return kVA_NOT_IMPLEMENTED;

#endif
}

/*
	VA_PROT_SetDigitalOutputProtection
		manage activation and deactivation of STB's digital outputs
	arguments
		wProtection
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
*/
INT VA_PROT_SetDigitalOutputProtection(WORD wProtection)
{
	HxLOG_Info("[Enter][%x]\n", wProtection);

#if 1

	HxLOG_Print("[Exit]\n");
	return kVA_NOT_IMPLEMENTED;

#else // TODO

	// activate or deactivate each digital output

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#endif
}

/*
	VA_PROT_SuspendAnalogChannels
		suspend the display of all analog channels
	arguments
		void
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_ALREADY_DONE
*/
INT VA_PROT_SuspendAnalogChannels()
{
	HxLOG_Info("[Enter]\n");

#if 1

	HxLOG_Print("[Exit]\n");
	return kVA_NOT_IMPLEMENTED;

#else // TODO

	// if analog channel is already suspended
	if (0)
	{
		HxLOG_Info("kVA_ALREADY_DONE\n");
		return kVA_ALREADY_DONE;
	}

	// suspend analog channel

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#endif
}

/*
	VA_PROT_ResumeAnalogChannels
		resume the display of all analog channels
	arguments
		void
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_ALREADY_DONE
*/
INT VA_PROT_ResumeAnalogChannels()
{
	HxLOG_Info("[Enter]\n");

#if 1

	HxLOG_Print("[Exit]\n");
	return kVA_NOT_IMPLEMENTED;

#else // TODO

	// if analog channel is already resumed
	if (0)
	{
		HxLOG_Info("kVA_ALREADY_DONE\n");
		return kVA_ALREADY_DONE;
	}

	// resume analog channel

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#endif
}

/*
	VA_PROT_SuspendFreeChannels
		suspend the display of free-to-air channels
	arguments
		void
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_ALREADY_DONE
*/
INT VA_PROT_SuspendFreeChannels()
{
	HxLOG_Info("[Enter]\n");

#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK

	// if free channel is already suspended
	if (s_bFreeChannelBlockOn)
	{
		HxLOG_Info("kVA_ALREADY_DONE\n");
		return kVA_ALREADY_DONE;
	}

	// suspend free channel
	VA_UI_Adapt_FreeChannelBlock(TRUE);
	s_bFreeChannelBlockOn = TRUE;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#else

	HxLOG_Print("[Exit]\n");
	return kVA_NOT_IMPLEMENTED;

#endif
}


/*
	VA_PROT_ResumeFreeChannels
		resume the display of free-to-air channels
	arguments
		void
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_ALREADY_DONE
*/
INT VA_PROT_ResumeFreeChannels()
{
	HxLOG_Info("[Enter]\n");

#ifdef CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK

	// if free channel is already resumed
	if (!s_bFreeChannelBlockOn)
	{
		HxLOG_Info("kVA_ALREADY_DONE\n");
		return kVA_ALREADY_DONE;
	}

	// resume free channel
	VA_UI_Adapt_FreeChannelBlock(FALSE);
	s_bFreeChannelBlockOn = FALSE;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#else

	HxLOG_Print("[Exit]\n");
	return kVA_NOT_IMPLEMENTED;

#endif
}

// ACS3.1부터 아래 함수 지원해야 함.
#if !defined(__VIACCESS50__)
INT VA_PROT_SetCopyProtection(BYTE byCopyProtection)
{
#if defined(CONFIG_PROD_BXRHD) || defined(CONFIG_PROD_BXRHDPLUS) // TODO : 모델 config 사용하지 않도록 정리 필요.

	// BXR-HD는 Macrovision, CGMS-A가 없으니 return kVA_NOT_IMPLEMENTED
	// CGMS-A가 별도 define이 없어서 모델명으로 처리
	HxLOG_Print("[Not Implemented]\n");
	return kVA_NOT_IMPLEMENTED;

#else
	CopyRight_t eCgmsMode;

	HxLOG_Info("[Enter](%x,macrovision:%d, cgms:%d)\n", byCopyProtection, byCopyProtection & 0x1, (byCopyProtection > 1) & 0x3);

	#ifdef CONFIG_MW_CAS_VIACCESS_MACROVISION
		if (byCopyProtection & 0x1)
			PAL_OUT_SetMacrovision(eMACROVISION_AGC);
		else
			PAL_OUT_SetMacrovision(eMACROVISION_OFF);
	#endif

	#ifdef CONFIG_MW_CAS_VIACCESS_CGMSA
		switch ((byCopyProtection > 1) & 0x3)
		{
			case 0:		eCgmsMode = eCOPYRIGHT_COPY_FREE; break;
			case 1:		eCgmsMode = eCOPYRIGHT_COPY_NOMORE; break;
			case 2:		eCgmsMode = eCOPYRIGHT_COPY_ONCE; break;
			case 3:		eCgmsMode = eCOPYRIGHT_COPY_NEVER; break;
			default:		break;	// never reach here
		}

		PAL_OUT_SetVideoBlankCopyRight(eCgmsMode);
	#endif

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#endif
}
#else
/*****************************************
//ACS5.0 부터 아래 함수 지원
******************************************/
INT VA_PROT_GetCapabilities(WORD *pOutputProtection,WORD *pCopyProtection)
{
	return kVA_OK;
}

INT VA_PROT_SetCopyProtection(DWORD dwAcsId, WORD wCopyProtection)
{
	return kVA_OK;
}

INT VA_PROT_StartFingerprintingSession(DWORD dwAcsId,
											const char *pszFingerprintStr,
											UINT8 uiFontSize,
											tVA_PROT_Color stForegroundColor,
											tVA_PROT_Color stBackgroundColor)
{
	return kVA_OK;
}

INT VA_PROT_DisplayFingerprint( DWORD dwAcsId,
									UINT8 uiHorizontalPercent,
									UINT8 uiVerticalPercent,
									UINT16 uiDuration)
{
	return kVA_OK;
}

INT VA_PROT_StopFingerprintingSession(DWORD dwAcsId)
{
	return kVA_OK;
}
/*
이미 잇다.
INT VA_PROT_SuspendFreeChannels(void)
{
	return kVA_OK;
}

INT VA_PROT_ResumeFreeChannels(void)
{
	return kVA_OK;
}
*/
INT VA_PROT_SetCertificateList( tVA_PROT_CertificateListType eCertificateListType,
									UINT32 uiLength,
									const BYTE *pCertificateList)
{
	return kVA_OK;
}

INT VA_PROT_SetCiPlusKeyLifetime(UINT32 uiKeyLifetime)
{
	return kVA_OK;
}

#endif
