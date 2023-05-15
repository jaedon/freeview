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
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif

#include <svc_cas.h>

#include <db_svc.h>
#include <db_param.h>

#include <va_ui.h>
#include <va_ui_adapt.h>
#include <va_util.h>
#include <va_prot.h>

#include "./local_include/_xmgr_cas_va_util.h"
#include <xmgr_cas_res_str.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/





/*******************************************************************/
/********************      Global variable      ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____AP_CAS_VA_UTIL_LOCAL_FUNCTION______________________________________________________

extern HERROR AP_CAS_VA_Preview(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle);
extern HERROR AP_CAS_VA_DestroyPreview(void);
extern void VA_UI_Adapt_MsgHandler(VA_UI_EVENT *pEvent);
#if (defined(CONFIG_MW_CAS_VIACCESS) && defined(CONFIG_MW_MM_PVR))
//extern int PAL_FS_GetHddSerialNumber(const char *dev, char **hddsn);		// Please use "UAPI_FS_GetHddSerialNumber()"
#endif

#ifdef CONFIG_DEBUG
extern HUINT8 *VA_UI_Debug_GetCamlockLevel(HINT32 nCamlockCtrlLevel);

STATIC VA_UI_EVENT_TYPE	local_ap_cas_va_dscr_cmd(HUINT8 *pszCmd)
{
	VA_UI_EVENT_TYPE eEventType = eVA_UI_LAST_EVENT;

	if (pszCmd == NULL)
		return eCAS_MSG_TYPE_None;

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"ok") == 0)
	{
		eEventType = eVA_UI_DESCRAMBLING_OK_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"preview") == 0)
	{
		eEventType = eVA_UI_DESCRAMBLING_OK_PREVIEW_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"err") == 0)
	{
		eEventType = eVA_UI_DESCRAMBLING_KO_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"right") == 0)
	{
		eEventType = eVA_UI_DESCRAMBLING_KO_NO_RIGHT_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"mr") == 0)
	{
		eEventType = eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"black") == 0)
	{
		eEventType = eVA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"key") == 0)
	{
		eEventType = eVA_UI_DESCRAMBLING_KO_MISSING_KEY_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"cw") == 0)
	{
		eEventType = eVA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"pair") == 0)
	{	// dscr은 아니지만 임시로..
		eEventType = eVA_UI_PAIRING_MISMATCH_EVENT;
	}
	else
	{
		eEventType = eCAS_MSG_TYPE_None;
	}

	HxLOG_Info("descrambling test mode (%s / 0x%x)\n", pszCmd, eEventType);

	return eEventType;
}

STATIC VA_UI_EVENT_TYPE	local_ap_cas_va_sc_cmd(HUINT8 *pszCmd)
{
	VA_UI_EVENT_TYPE eEventType = eVA_UI_LAST_EVENT;

	if (pszCmd == NULL)
		return eCAS_MSG_TYPE_None;

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"insert") == 0)
	{
		eEventType = eVA_UI_SMARTCARD_INSERTED_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"extract") == 0)
	{
		eEventType = eVA_UI_SMARTCARD_EXTRACTED_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"ready") == 0)
	{
		eEventType = eVA_UI_SMARTCARD_READY_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"need") == 0)
	{
		eEventType = eVA_UI_SMARTCARD_NEEDED_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"fail") == 0)
	{
		eEventType = eVA_UI_SMARTCARD_FAILED_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"accept") == 0)
	{
		eEventType = eVA_UI_SMARTCARD_NOT_ACCEPTED_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"memory") == 0)
	{
		eEventType = eVA_UI_SMARTCARD_INSUFFICIENT_MEMORY_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"invalid") == 0)
	{
		eEventType = eVA_UI_SMARTCARD_INVALIDATED_EVENT;
	}
	else
	{
		eEventType = eCAS_MSG_TYPE_None;
	}

	HxLOG_Info("smartcard test mode (%s / 0x%x)\n", pszCmd, eEventType);

	return eEventType;
}

STATIC VA_UI_EVENT_TYPE	local_ap_cas_va_opmsg_cmd(HUINT8 *pszCmd)
{
	VA_UI_EVENT_TYPE eEventType = eVA_UI_LAST_EVENT;

	if (pszCmd == NULL)
		return eCAS_MSG_TYPE_None;

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"new") == 0)
	{
		eEventType = eVA_UI_NEW_OPERATOR_MESSAGE_EVENT;
	}
	else
	{
		eEventType = eCAS_MSG_TYPE_None;
	}

	HxLOG_Info("op message test mode (%s / 0x%x)\n", pszCmd, eEventType);

	return eEventType;
}

STATIC VA_UI_EVENT_TYPE	local_ap_cas_va_camlock_cmd(HUINT8 *pszCmd)
{
	VA_UI_EVENT_TYPE eEventType = eVA_UI_LAST_EVENT;

	if (pszCmd == NULL)
		return eCAS_MSG_TYPE_None;

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"ok") == 0)
	{
		eEventType = eVA_UI_CAMLOCK_ACTIVATED_OK_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"err") == 0)
	{
		eEventType = eVA_UI_CAMLOCK_ACTIVATED_KO_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"nocard") == 0)
	{
		eEventType = eVA_UI_CAMLOCK_ACTIVATED_NO_CARD_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"noactive") == 0)
	{
		eEventType = eVA_UI_CAMLOCK_NOT_ACTIVATED_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"disable") == 0)
	{
		eEventType = eVA_UI_CAMLOCK_DISABLED_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"ko") == 0)
	{
		eEventType = VA_UI_CAMLOCK_ACTIVATED_KO;
	}
	else
	{
		eEventType = eCAS_MSG_TYPE_None;
	}

	HxLOG_Info("cam lock test mode (%s / 0x%x)\n", pszCmd, eEventType);

	return eEventType;
}

STATIC HERROR local_ap_cas_va_camlockcontrol_cmd(HUINT8 *pszCmd)
{
	HxLOG_Info("cam lock control mode (%s)\n", pszCmd);

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"show") == 0)
	{
		HxLOG_Print("current cam lock control value is %s\n", VA_UI_Debug_GetCamlockLevel(VA_GetCamLockControlLevel()));
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"disable") == 0)
	{
		VA_SetCamLockControlLevel(VA_CAMLOCK_CONTROL_NO);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"low") == 0)
	{
		VA_SetCamLockControlLevel(VA_CAMLOCK_CONTROL_LOW);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"medium") == 0)
	{
		VA_SetCamLockControlLevel(VA_CAMLOCK_CONTROL_MEDIUM);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"high") == 0)
	{
		VA_SetCamLockControlLevel(VA_CAMLOCK_CONTROL_HIGH);
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC VA_UI_EVENT_TYPE local_ap_cas_va_security_cmd(HUINT8 *pszCmd)
{
	VA_UI_EVENT_TYPE eEventType;

	if (pszCmd == NULL)
		return eCAS_MSG_TYPE_None;

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"start") == 0)
	{
		eEventType = eVA_UI_START_SECURITY_RESTRICTION_EVENT;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"stop") == 0)
	{
		eEventType = eVA_UI_STOP_SECURITY_RESTRICTION_EVENT;
	}
	else
	{
		return ERR_FAIL;
	}

	HxLOG_Info("security test mode (%s / 0x%x)\n", pszCmd, eEventType);

	return eEventType;
}

#if 0	// compile warning
STATIC VA_UI_EVENT_TYPE	local_ap_cas_va_terminal_cmd(HUINT8 *pszCmd)
{
	VA_UI_EVENT_TYPE eEventType = eVA_UI_LAST_EVENT;

	return eEventType;
}
#endif

STATIC HERROR local_ap_cas_va_preview_cmd(HUINT8 *pszCmd)
{
	HxLOG_Info("preview test mode (%s)\n", pszCmd);

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"show") == 0)
	{
		AP_CAS_VA_Preview(0, 0);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"hide") == 0)
	{
		AP_CAS_VA_DestroyPreview();
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR local_ap_cas_va_fingerprint_cmd(HUINT8 *pszCmd)
{
	HxLOG_Info("fingerprint test mode (%s)\n", pszCmd);

#if defined(CONFIG_MW_CAS_VIACCESS_FINGERPRINT)
	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"show") == 0)
	{
		VA_PROT_StartFingerprinting("00000705269");
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"hide") == 0)
	{
		VA_PROT_StopFingerprinting();
	}
	else
	{
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

STATIC HERROR local_ap_cas_va_freechannelblock_cmd(HUINT8 *pszCmd)
{
	HxLOG_Info("free channel block test mode (%s)\n", pszCmd);

#if defined(CONFIG_MW_CAS_VIACCESS_FREECHANNEL_BLOCK)
	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"suspend") == 0)
	{
		VA_PROT_SuspendFreeChannels();
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"resume") == 0)
	{
		VA_PROT_ResumeFreeChannels();
	}
	else
	{
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

STATIC HERROR local_ap_cas_va_eeprom_cmd(HUINT8 *pszCmd)
{
	STATIC int Caution = 1;

	HxLOG_Info("eeprom test mode (%s)\n", pszCmd);

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"opmsg") == 0)
	{
#ifdef CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG
		VK_Print("Not support.\n");
#else
		VA_EraseAllOpMessage();
#endif
		return ERR_OK;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"opdata") == 0)
	{
#ifdef CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG
		VK_Print("Not support.\n");
#else
		VA_EraseAllOpData();
#endif
		return ERR_OK;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"userdata") == 0)
	{
#ifdef CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG
		VK_Print("Not support.\n");
#else
		VA_EraseAllUserData();
#endif
		return ERR_OK;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"acsdata") == 0)
	{
		if (Caution)
		{
			HxLOG_Print("Caution : BBX data will be erased !! If you really want it, type command again.\n");
			Caution = 0;
		}
		else
		{
			VA_EraseAllAcsData();
			Caution = 1;
		}
		return ERR_OK;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"extdata") == 0)
	{
		VA_EraseAllExtData();
		return ERR_OK;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"cwe") == 0)
	{
#ifdef CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG
		VK_Print("Not support.\n");
#else
		VA_ClearCWE();
#endif
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}
}

#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____AP_CAS_VA_UTIL_PUBLIC_FUNCTION______________________________________________________

#if 1 //To comile for Octo2
void AP_CAS_VA_GetClassMarkStr(VA_UI_CLASS_LIST *pClassList, HUINT8 *pStr)
{
	HUINT8 szStr[10], szStr2[100];
	int nStart, nEnd, nPrevEnd, bSplitted;
	int i, j, k;

	HxLOG_Print("[Enter]\n");

	nStart = nEnd = -1;
	bSplitted = FALSE;

	szStr2[0] = '\0';

	// each bit represents a class (LSB represents class 0)
	for (i = pClassList->ucSize - 1, k = 0; i >= 0; i--)
	{
		for (j = 0; j < 8; j++)
		{
			if (pClassList->aucClassMark[i] & (1 << j))
			{
				// the first "1" found
				if (nStart < 0)
				{
					nStart = k*8 + j;
					nEnd = k*8 + j;
				}
				else
				{
					nPrevEnd = nEnd;
					nEnd = k*8 + j;

					if (nPrevEnd != nEnd  - 1)	// if "0" is presented between "1"s
					{
						if (nStart == nPrevEnd)
							snprintf((char*)szStr, 10, "%d", nStart);
						else
							snprintf((char*)szStr, 10, "%d-%d", nStart, nPrevEnd);

						if (bSplitted)
 							strncat(szStr2, ", ", 99);
						strncat(szStr2, szStr, 99);

						bSplitted = TRUE;
						nStart = nEnd;
					}
				}
			}
		}
 		k++;
	}

	if (nStart >= 0)
	{
		if (nStart == nEnd)
			snprintf((char*)szStr, 10, "%d", nStart);
		else
			snprintf((char*)szStr, 10, "%d-%d", nStart, nEnd);

		if (bSplitted)
 			strncat(szStr2, ", ", 99);
		strncat(szStr2, szStr, 99);
	}

	// if string is too long, then show classes without "Class" prefix
	if (strlen(szStr2) > 20)
		strncpy(pStr, szStr2, TEXT_SIZE128-1);
	else
		snprintf(pStr, TEXT_SIZE128, "%s %s", "Class" , szStr2); // (HUINT8 *)GetStrRsc(LOC_CAS_VA_CLASS_ID) // hjlee3, just for compile

	HxLOG_Print("[Exit]\n");
}

void AP_CAS_VA_GetWordFromString(HUINT8 *pszDest, HUINT8 *pszSrc, HUINT8 nIndex)
{
	HINT32	i, n, j;

	HxLOG_Print("[Enter]\n");

	n = 0;
	for (i=0; i < (int)SvcCas_UtilStrLen(pszSrc) && n != nIndex; i++)
	{
		if (pszSrc[i] == '|')
			n++;
	}

	/* copy to output buffer */
	for (j = i; j < (int)SvcCas_UtilStrLen(pszSrc) && pszSrc[j] != '|'; j++)
		pszDest[j-i] = pszSrc[j];

	pszDest[j-i] = '\0';

	HxLOG_Print("[Exit]\n");
}

void	AP_CAS_VA_GetConvertOpMessage(VA_UI_OP_MESSAGE *pstOpMsg, HUINT8 *pStr, HUINT32 ulLen, HUINT32 *pulConvLen)
{
	if(pstOpMsg != NULL)
	{
		switch(pstOpMsg->ulEncode)
		{
		case eVA_OpMsg_Encodeing_Ascii:
			MWC_UTIL_DvbStrncpy(pStr, pstOpMsg->szMsg, ulLen);
			if(pulConvLen != NULL)
				*pulConvLen = pstOpMsg->ulCount;
			break;

		case eVA_OpMsg_Encodeing_UTF_8:
			break;

		case eVA_OpMsg_Encodeing_UTF_16:
#if defined(CONFIG_OP_NTVPLUS)
			{
				HUINT32 lSingleLen;

				lSingleLen = HLIB_CP_ConvertUCS2ToCodepage(eHxCP_ISO_8859_5, (const HUINT16 *)pstOpMsg->szMsg, pStr, ulLen);
				if(pulConvLen != NULL)
					*pulConvLen = lSingleLen;
			}
#endif
			break;

		case eVA_OpMsg_Encodeing_UTF_32:
			break;

		case eVA_OpMsg_Encodeing_GB2312_80:
			break;
		}
	}
}

void VA_MemDump(HINT8 *pszName, HUINT8 *pData, HUINT32 ulLen, HUINT8 *pszCol)
{
	int x;

	HxLOG_Print("\n%s[%s Dump]%s\n", pszCol, pszName, C_CODE_RESET);
	for (x = 0; x < ulLen; x++)
	{
		if((x != 0) && (x % 16 == 0))
			HxLOG_Print("\n");

		HxLOG_Print("%s%02X %s", pszCol, pData[x], C_CODE_RESET);
	}
	HxLOG_Print("\n");
}
#endif

#ifdef CONFIG_DEBUG
HERROR AP_CAS_VA_CMD(HUINT8 *pszCmd, HUINT8 *pszSubCmd)
{
	STATIC VA_UI_EVENT stEvent;
	HERROR ret = ERR_FAIL;

	if (pszCmd == NULL)
	{
		HxLOG_Error("Wrong command !!\n");
		return ERR_FAIL;
	}

	/* help */
	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"help") == 0)
	{
		HxLOG_Print("\n---------- commands ------------------------------------------------------------\n");
		HxLOG_Print(" dscr         ok | preview | err | right | mr | black | key \n");
		HxLOG_Print(" sc           insert | extract | ready | need | fail | accept | memory | invalid \n");
		HxLOG_Print(" opmsg        new \n");
		HxLOG_Print(" camlock      ok | err | nocard | noactive | disable \n");
		HxLOG_Print(" camlockctrl  show | disable | low | medium | high \n");
		HxLOG_Print(" security     start | stop \n");
		HxLOG_Print(" preview      show | hide \n");
		HxLOG_Print(" finger       show | hide \n");
		HxLOG_Print(" freechblock  suspend | resume \n");
		HxLOG_Print(" erase        opmsg | opdata | userdata | acsdata | extdata | cwe \n");
		HxLOG_Print("--------------------------------------------------------------------------------\n\n");
		return ERR_OK;
	}

	stEvent.ulScSlot = 0;
	stEvent.ulAcsId = 0;
	stEvent.ulStbStreamHandle = HANDLE_NULL /* kVA_UI_NOT_USED */;

	/* tVA_UI_EventType 에 열겨된 순서대로 */
	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"dscr") == 0)
	{
		stEvent.eEventType = local_ap_cas_va_dscr_cmd(pszSubCmd);
		goto send_event;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"sc") == 0)
	{
		stEvent.eEventType = local_ap_cas_va_sc_cmd(pszSubCmd);
		goto send_event;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"opmsg") == 0)
	{
		stEvent.eEventType = local_ap_cas_va_opmsg_cmd(pszSubCmd);
		goto send_event;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"camlock") == 0)
	{
		stEvent.eEventType = local_ap_cas_va_camlock_cmd(pszSubCmd);
		goto send_event;
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"camlockctrl") == 0)
	{
		ret = local_ap_cas_va_camlockcontrol_cmd(pszSubCmd);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"security") == 0)
	{
		stEvent.eEventType = local_ap_cas_va_security_cmd(pszSubCmd);
		goto send_event;
	}
	/* 이하는 tVA_UI_EventType 에 없는 것들 */
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"preview") == 0)
	{
		ret = local_ap_cas_va_preview_cmd(pszSubCmd);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"finger") == 0)
	{
		ret = local_ap_cas_va_fingerprint_cmd(pszSubCmd);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"freechblock") == 0)
	{
		ret = local_ap_cas_va_freechannelblock_cmd(pszSubCmd);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"erase") == 0)
	{
		ret = local_ap_cas_va_eeprom_cmd(pszSubCmd);
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8 *)"hdd") == 0)
	{
		//#ifdef CONFIG_MW_MULTIMEDIA
		//jwlee add the define CONFIG_MW_MM_PVR. A HDD is used only internel or external PVR models.
		#if (defined(CONFIG_MW_CAS_VIACCESS) && defined(CONFIG_MW_MM_PVR))
		HUINT8 *pszHddSn;
		SVC_SYS_GetHddSerialNumber(pszSubCmd, &pszHddSn);
		HxLOG_Print("%s Serial Number : %s\n", pszSubCmd, pszHddSn);
		#else
		HxLOG_Print("svc_cas_DevFsGetHddSerialNumber() not supported !!\n");
		#endif
	}
	else
	{
	}

	return ret;

send_event:

	if (stEvent.eEventType == eCAS_MSG_TYPE_None)
	{
		return ERR_FAIL;
	}

	if (0)
		VA_UI_Adapt_SendEvent(&stEvent, eCAS_MSG_TYPE_Fail, eCasShowType_Both);
	else
		VA_UI_Adapt_MsgHandler(&stEvent);

	return ERR_OK;
}
#endif

