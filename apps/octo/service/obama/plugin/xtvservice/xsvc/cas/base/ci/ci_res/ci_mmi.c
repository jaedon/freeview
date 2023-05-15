/* $Header:   //BASE/archives/PDR_LIGHT/mw/cas/ci/ci_res/ci_mmi.c-arc   1.5   Jan 13 2007 00:18:38   htson  $ */
/***************************************************************
* $Workfile:   ci_mmi.c  $
* $Modtime:   Jan 12 2007 06:19:26  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <octo_common.h>

#include <pal_ci.h>
#include <cb_list.h>

#include "ci.h"
#include "ci_reshandler.h"

#include <svc_cas.h>
#include <_svc_cas_sub_dev_api.h>



/***************************************************************
* local definition
***************************************************************/
#define CI_MMI_CLOSE_IMMEDIATE		0x00
#define CI_MMI_CLOSE_DELAY			0x01

// display_control_cmd in display_control()
#define SET_MMI_MODE										0x01
#define GET_DISPLAY_CHAR_TABLE_LIST						0x02
#define GET_INPUT_CHAR_TABLE_LIST						0x03
#define GET_OVERLAY_GRAPHICS_CHARACTERISTICS			0x04
#define GET_FULL_SCREEN_GRAPHICS_CHARACTERISTICS		0x05

// mmi_mode in display_control()
#define CI_MMI_HIGH_LEVEL									0x01
#define CI_MMI_LOW_LEVEL_OVERLAY_GRAPHICS				0x02
#define CI_MMI_LOW_LEVEL_FULL_SCREEN_GRAPHICS			0x03

// display_reply_id in display_reply()
#define CI_MMI_MODE_ACK									0x01
#define CI_LIST_DISPLAY_CHAR_TABLES						0x02
#define CI_LIST_INPUT_CHAR_TABLES						0x03
#define CI_LIST_GRAPHIC_OVERLAY_CHARACTERISTICS		0x04
#define CI_LIST_FULL_SCREEN_GRAPHIC_CHARACTERISTICS	0x05
#define CI_UNKNOWN_DISPLAY_CONTROL_CMD					0xF0
#define CI_UNKNOWN_MMI_MODE								0xF1
#define CI_UNKNOWN_CHAR_TABLE							0xF2

// Answ object answ_id in answ()
#define CI_MMI_ANSW_ANSWER		0x01
#define CI_MMI_ANSW_CANCEL		0x00

/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE	g_hCbNotifyMmiReq;

//2009.02.06. BSPARK, Mmi Open State를 확인하기 위함 --> 이상한 코드군요. 수정 필요합니다!
HBOOL g_bMmiOpened = FALSE;

/***************************************************************
* static variables and structures
***************************************************************/
static const unsigned char s_CiCharTable[] =
{
	" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
};

static MMI_INSTANCE  s_aCiMmi[MAX_CI_MMI_SS];
static unsigned char s_ucCiMmiCnt;

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
static unsigned long fWaitingUserInputInMMI = 0;
#endif

/***************************************************************
* static function prototypes
***************************************************************/
static int CiMmi_ProcMmiClose(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcDisplayControl(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcTextLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcEnq(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcMenuLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcListLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);

static int CiMmi_AddInst(unsigned short usSsNo);
static int CiMmi_DeleteInst(unsigned short usSsNo);
static MMI_INSTANCE * CiMmi_GetNewInst(void);
static MMI_INSTANCE * CiMmi_GetInst(unsigned short usSsNo);

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
static int CiMmi_SetUserInputWaitingFlag(unsigned long value);
#endif

/***************************************************************
* function bodies
***************************************************************/
int CiMmi_Init(void)
{
	HxSTD_memset(s_aCiMmi, 0, sizeof(MMI_INSTANCE) * MAX_CI_MMI_SS);

	s_ucCiMmiCnt = 0;

	return ERR_OK;
}

int CiMmi_SsOpenedProc(unsigned short usSsNo)
{
	MMI_INSTANCE *hMmi;
	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;

	HxLOG_Info("CiMmi_SsOpenedProc() called...\n");

	if (CiMmi_AddInst(usSsNo) < 0)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_SsOpenedProc()---> Error : can't add MMI Instance for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	/******************************************
	  notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_SsOpenedProc()---> Error : can't find MMI Instance for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIMMI]---> CiMmi_SsOpenedProc()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo);

	Callback_Reset(g_hCbNotifyMmiReq);
	while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
	{
		(*pfnMmiReq)((MMI_HANDLE)hMmi, MMI_OPEN, NULL, 0);
	}
	g_bMmiOpened = TRUE;

	return ERR_OK;
}

int CiMmi_SsClosedProc(unsigned short usSsNo)
{
	MMI_INSTANCE *hMmi;
	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;

	/******************************************
	  notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_SsClosedProc()---> Error : can't find MMI Instance for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIMMI]---> CiMmi_SsClosedProc()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo);

	Callback_Reset(g_hCbNotifyMmiReq);
	while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
	{
		pfnMmiReq((MMI_HANDLE)hMmi, MMI_CLOSE, NULL, 0);
	}
	g_bMmiOpened = FALSE;

	if (CiMmi_DeleteInst(usSsNo) < 0)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_SsClosedProc()---> CiMmi_DeleteInst() fail. SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIMMI]---> CiMmi_SsClosedProc()---> success SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

int CiMmi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	HxLOG_Info("CiMmi_ApduProc() called...\n");

	switch (ulApduTag)
	{
	/* used in both mode(Low-Level/High-Level) */
	case CI_CLOSE_MMI:
		HxLOG_Info("[CIMMI]---> CiMmi_ApduProc()---> Received CI_CLOSE_MMI tag...\n");
		nRtnCode = CiMmi_ProcMmiClose(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_DISPLAY_CONTROL:
		HxLOG_Info("[CIMMI]---> CiMmi_ApduProc()---> Received CI_DISPLAY_CONTROL tag...\n");
		nRtnCode = CiMmi_ProcDisplayControl(usSsNo, pucApduData, ulApduLen);
		break;
	/* High-Level MMI objects */
	case CI_TEXT_LAST:
		HxLOG_Info("[CIMMI]---> CiMmi_ApduProc()---> Received CI_TEXT_LAST tag...\n");
		nRtnCode = CiMmi_ProcTextLast(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_TEXT_MORE:
		HxLOG_Error("[CIMMI][temporal ERROR - Host DO NOT support this APDU]---> CiMmi_ApduProc()---> Received CI_TEXT_MORE tag...\n"); // 여기 타면 현재 처리 불가. (구현 필요.)
		break;
	case CI_ENQ:
		HxLOG_Info("[CIMMI]---> CiMmi_ApduProc()---> Received CI_ENQ tag...\n");
		nRtnCode = CiMmi_ProcEnq(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_MENU_LAST:
		HxLOG_Info("[CIMMI]---> CiMmi_ApduProc()---> Received CI_MENU_LAST tag...\n");
		nRtnCode = CiMmi_ProcMenuLast(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_MENU_MORE:
		HxLOG_Info("[CIMMI][temporal ERROR - Host DO NOT support this APDU]---> CiMmi_ApduProc()---> Received CI_MENU_MORE tag...\n"); // 여기 타면 현재 처리 불가. (구현 필요.)
		break;
	case CI_LIST_LAST:
		HxLOG_Info("[CIMMI]---> CiMmi_ApduProc()---> Received CI_LIST_LAST tag...\n");
		nRtnCode = CiMmi_ProcListLast(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_LIST_MORE:
		HxLOG_Info("[CIMMI][temporal ERROR - Host DO NOT support this APDU]---> CiMmi_ApduProc()---> Received CI_LIST_MORE tag...\n"); // 여기 타면 현재 처리 불가. (구현 필요.)
		break;
	/* Low-Level MMI Keypad object not supported, out of consideration */
	/* Low-Level MMI Display object not supported */
	case CI_SUBTITLE_SEG_LAST:			// host <-- app, 현재 지원하지 않음.
	case CI_SUBTITLE_SEG_MORE:			// host --> app, 현재 지원하지 않음.
	case CI_DISPLAY_MSG:				// host <-- app, 현재 지원하지 않음.
	case CI_SCENE_END_MARK	:			// host <-- app, 현재 지원하지 않음.
	case CI_SCENE_DONE:					// host <-- app, 현재 지원하지 않음.
	case CI_SCENE_CONTROL:				// host --> app, 현재 지원하지 않음.
	case CI_SUBTITLE_DOWNLOAD_LAST:		// host <-- app, 현재 지원하지 않음.
	case CI_SUBTITLE_DOWNLOAD_MORE:		// host --> app, 현재 지원하지 않음.
	case CI_FLUSH_DOWNLOAD:				// host <-- app, 현재 지원하지 않음.
	case CI_DOWNLOAD_REPLY:				// host <-- app, 현재 지원하지 않음.
		nRtnCode = -1;
		HxLOG_Error("[CIMMI]---> CiMmi_ApduProc()---> Error : not support Apdu Tag[0x%lx] !!!\n", ulApduTag);
		break;
	default:
		nRtnCode = -1;
		HxLOG_Error("[CIMMI]---> CiMmi_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	HxLOG_Info("[CIMMI]---> CiMmi_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode);

	return nRtnCode;
}

static int CiMmi_ProcMmiClose(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char ucDelaySec;
	MMI_INSTANCE *hMmi;
	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;

	HxLOG_Info("CiMmi_ProcMmiClose() called...\n");

	if(pucApduData == NULL || ulApduLen < 1)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMmiClose()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMmiClose()---> Error : can't find MMI Instance for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	if (CIRH_ReqCloseSs(usSsNo) < 0)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMmiClose()---> Error : CIRH_ReqCloseSs() for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	if (pucApduData[0] == CI_MMI_CLOSE_IMMEDIATE)
	{
		HxLOG_Info("[CIMMI]---> CiMmi_ProcMmiClose()---> CI_MMI_CLOSE_IMMEDIATE\n");
		ucDelaySec = 0;
	}
	else if (pucApduData[0] == CI_MMI_CLOSE_DELAY)
	{
		HxLOG_Info("[CIMMI]---> CiMmi_ProcMmiClose()---> CI_MMI_CLOSE_DELAY\n");
		ucDelaySec = pucApduData[1];
		HxLOG_Info("[CIMMI]---> CiMmi_ProcMmiClose()---> delay %d sec\n", ucDelaySec);
	}
	else
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMmiClose()---> Error : Unknown cmd_id[0x%x] !!!\n", pucApduData[0]);
		return -1;
	}

	/******************************************
	  notify mmi
	 ******************************************/
	HxLOG_Info("[CIMMI]---> CiMmi_ProcMmiClose()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo);

	Callback_Reset(g_hCbNotifyMmiReq);
	while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
	{
		pfnMmiReq((MMI_HANDLE)hMmi, MMI_CLOSE, NULL, ucDelaySec);
	}

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
	CiMmi_SetUserInputWaitingFlag(0);
#endif

	return ERR_OK;
}

static int CiMmi_ProcDisplayControl(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char	ucCmd;
	unsigned char	aucMsg[120];
	unsigned long	ulLen;
	unsigned char	ucLenFieldSize;
	int	i = 0;

	HxLOG_Info("CiMmi_ProcDisplayControl() called...\n");

	if(pucApduData == NULL || ulApduLen < 1)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	ucCmd = pucApduData[0];

	aucMsg[i++] = (unsigned char)((CI_DISPLAY_REPLY >> 16) & 0xff);
	aucMsg[i++] = (unsigned char)((CI_DISPLAY_REPLY >> 8) & 0xff);
	aucMsg[i++] = (unsigned char)(CI_DISPLAY_REPLY & 0xff);

	switch (ucCmd)
	{
	case SET_MMI_MODE:
		if (pucApduData[1] == CI_MMI_HIGH_LEVEL)
		{
			aucMsg[i++] = 0x02;/* length */
			aucMsg[i++] = CI_MMI_MODE_ACK;/* display_reply_id */
			aucMsg[i++] = CI_MMI_HIGH_LEVEL;/* mmi_mode */
			if (CIRH_SendApdu(usSsNo, aucMsg, i) != ERR_OK)
			{
				HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl(CI_MMI_MODE_ACK)---> Error : fail to send APDU for SsNo[%d] !!!\n", usSsNo);
				return -1;
			}
			HxLOG_Info("[CIMMI]---> CiMmi_ProcDisplayControl()---> SET_MMI_MODE SsNo[%d]\n", usSsNo);
		}
		else
		{
			aucMsg[i++] = 0x01;
			aucMsg[i++] = CI_UNKNOWN_MMI_MODE;
			if (CIRH_SendApdu(usSsNo, aucMsg, i) != ERR_OK)
			{
				HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl(CI_UNKNOWN_MMI_MODE)---> Error : fail to send APDU for SsNo[%d] !!!\n", usSsNo);
				return -1;
			}
			HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl()---> Error : unknown mmi mode for SsNo[%d] !!!\n", usSsNo);
			return -1;
		}
		break;

	case GET_DISPLAY_CHAR_TABLE_LIST:
		ulLen = sizeof(s_CiCharTable) + 1;
		CIRH_ConvertApduLengthField(ulLen, &aucMsg[i], &ucLenFieldSize);
		i += ucLenFieldSize;
		aucMsg[i++] = CI_LIST_DISPLAY_CHAR_TABLES;
		HxSTD_memcpy(&aucMsg[i], s_CiCharTable, sizeof(s_CiCharTable)); // HxSTD_memcpy(&aucMsg[i++], s_CiCharTable, sizeof(s_CiCharTable));
		if (CIRH_SendApdu(usSsNo, aucMsg, i + sizeof(s_CiCharTable)) != ERR_OK)
		{
			HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl(CI_LIST_DISPLAY_CHAR_TABLES)---> Error : fail to send APDU for SsNo[%d] !!!\n", usSsNo);
			return -1;
		}
		HxLOG_Info("[CIMMI]---> CiMmi_ProcDisplayControl()---> GET_DISPLAY_CHAR_TABLE_LIST SsNo[%d]\n", usSsNo);
		break;

	case GET_INPUT_CHAR_TABLE_LIST:
		ulLen = sizeof(s_CiCharTable) + 1;
		CIRH_ConvertApduLengthField(ulLen, &aucMsg[i], &ucLenFieldSize);
		i += ucLenFieldSize;
		aucMsg[i++] = CI_LIST_INPUT_CHAR_TABLES;
		HxSTD_memcpy(&aucMsg[i], s_CiCharTable, sizeof(s_CiCharTable)); // HxSTD_memcpy(&aucMsg[i++], s_CiCharTable, sizeof(s_CiCharTable));
		if (CIRH_SendApdu(usSsNo, aucMsg, i + sizeof(s_CiCharTable)) != ERR_OK)
		{
			HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl(CI_LIST_INPUT_CHAR_TABLES)---> Error : fail to send APDU for SsNo[%d] !!!\n", usSsNo);
			return -1;
		}
		HxLOG_Info("[CIMMI]---> CiMmi_ProcDisplayControl()---> GET_INPUT_CHAR_TABLE_LIST SsNo[%d]\n", usSsNo);
		break;

	case GET_OVERLAY_GRAPHICS_CHARACTERISTICS:
	case GET_FULL_SCREEN_GRAPHICS_CHARACTERISTICS:
		aucMsg[i++] = 0x01;
		aucMsg[i++] = CI_UNKNOWN_DISPLAY_CONTROL_CMD;
		if (CIRH_SendApdu(usSsNo, aucMsg, i) != ERR_OK)
		{
			HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl(CI_UNKNOWN_DISPLAY_CONTROL_CMD)---> Error : fail to send APDU for SsNo[%d] !!!\n", usSsNo);
			return -1;
		}
		HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl()---> Error : not support display control cmd for SsNo[%d] !!!\n", usSsNo);
		return -1;

	default:
		HxLOG_Error("[CIMMI]---> CiMmi_ProcDisplayControl()---> Error : unknown display control cmd for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

static int CiMmi_ProcTextLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char *pucText, ucCalledCb = FALSE;
	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;
	MMI_INSTANCE *hMmi;

	HxLOG_Info("CiMmi_ProcTextLast() called...\n");

	if(pucApduData == NULL || ulApduLen == 0)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcTextLast()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	if (ulApduLen > MAX_CI_STR_LEN)	// text_char(0~40)
	{
		HxLOG_Error("[CIMMI]--->CiMmi_ProcTextLast()---> Error : ulApduLen %ld > %d\n", ulApduLen, MAX_CI_STR_LEN);
		ulApduLen = MAX_CI_STR_LEN;	// or send close_mmi() to CAM
	}

	pucText = (unsigned char *)OxCAS_Malloc(ulApduLen);	// be freed by receiver(client)
	if (pucText == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcTextLast()---> Error : memory not enough for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxSTD_memset(pucText, 0, ulApduLen);
	HxSTD_memcpy(pucText, pucApduData, ulApduLen);

	/******************************************
	  notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcTextLast()---> Error : can't find MMI Instance for SsNo[%d] !!!\n", usSsNo);
		OxCAS_Free(pucText);
		return -1;
	}

	HxLOG_Info("[CIMMI]---> CiMmi_ProcTextLast()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo);

	Callback_Reset(g_hCbNotifyMmiReq);
	while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
	{
		(*pfnMmiReq)((MMI_HANDLE)hMmi, MMI_TEXT, (void *)pucText, ulApduLen);
		ucCalledCb = TRUE;
	}

	if(ucCalledCb != TRUE)
	{
		OxCAS_Free(pucText);
	}

	return ERR_OK;
}

static int CiMmi_ProcEnq(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	MMI_ENQ_PARAM *pCiEnqParam;
	void *pvParam;
	unsigned long ulParamSize;
	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;
	MMI_INSTANCE *hMmi;

 	HxLOG_Info("CiMmi_ProcEnq() called...\n");

	if(pucApduData == NULL || ulApduLen < 2)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcEnq()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

#if 0
	if (ulApduLen > 2+MAX_CI_STR_LEN)	// blind_answer(1)+answer_text_length(1)+text_char(0~40)
	{
		// send close_mmi() to CAM & Host App.
		// to be added code by jhcho
	}
#endif

	ulParamSize = sizeof(MMI_ENQ_PARAM) + ulApduLen - 1; // because of null terminated string

	HxLOG_Info("[CIMMI]---> CiMmi_ProcEnq()---> ulParamSize[%ld]\n", ulParamSize);

	pvParam = (void *)OxCAS_Malloc((unsigned int)ulParamSize); // be freed by receiver(client)
	if (pvParam == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcEnq()---> Error : memory not enough for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	pCiEnqParam = (MMI_ENQ_PARAM *)pvParam;
	pCiEnqParam->ucBlindOnOff = pucApduData[0] & 0x01;
	pCiEnqParam->ucAnswerLen = pucApduData[1];
	pCiEnqParam->pszPrompt = (unsigned char *)pvParam + sizeof(MMI_ENQ_PARAM);
	HxSTD_memcpy(pCiEnqParam->pszPrompt, &pucApduData[2], ulApduLen - 2);
	((unsigned char *)pvParam)[ulParamSize -1] = 0x00;

	HxLOG_Info("[CIMMI]---> CiMmi_ProcEnq()---> Blind[%d], Answer Length[%d], Text[%s].\n", pCiEnqParam->ucBlindOnOff, pCiEnqParam->ucAnswerLen, pCiEnqParam->pszPrompt);

	/******************************************
	  notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcEnq()---> Error : can't find MMI Instance for SsNo[%d] !!!\n", usSsNo);
		OxCAS_Free(pvParam);
		return -1;
	}

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
	CiMmi_SetUserInputWaitingFlag(1);
#endif

	HxLOG_Info("[CIMMI]---> CiMmi_ProcEnq()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo);

	Callback_Reset(g_hCbNotifyMmiReq);
	while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
	{
		(*pfnMmiReq)((MMI_HANDLE)hMmi, MMI_ENQ, (void *)pvParam, ulParamSize);
	}

	return ERR_OK;
}

static int CiMmi_ProcMenuLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	MMI_MENU_LIST_PARAM	*pCiMenuParam;
	unsigned char		ucNumOfChoice;
	void 				*pvParam;
	unsigned long		ulParamSize;
	unsigned char 		*pucData;
	unsigned long		ulTextLen;
	unsigned long		ulApduTag;
	unsigned char 		**apucText;	// 25 : recommended value in R206-001
	unsigned short		*ausTextLen;	// 25 : recommended value in R206-001
	unsigned char		ucLenFieldSize;
	int	i;

	CI_NOTIFY_MMI_REQ_FUNC	pfnMmiReq;
	MMI_INSTANCE			*hMmi;

 	HxLOG_Info("CiMmi_ProcMenuLast() called...\n");

	pucData = pucApduData;
	if(pucData == NULL || ulApduLen == 0)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	/* get item number */
	ucNumOfChoice = *pucData++;
	if (ucNumOfChoice == 0xFF)
		ucNumOfChoice = 0;

	HxLOG_Info("[CIMMI]---> CiMmi_ProcMenuLast()---> NumOfChoice %d\n", ucNumOfChoice);

	apucText = (unsigned char **)OxCAS_Malloc(sizeof(unsigned char *) * (ucNumOfChoice+3));
	if (apucText == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : memory not enough for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	/* BS: Menu List에 전달되는 String 들 각각의 Size 정의 */
	ausTextLen = (unsigned short *)OxCAS_Malloc(sizeof(unsigned short) * (ucNumOfChoice+3));
	if (ausTextLen == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : memory not enough for SsNo[%d] !!!\n", usSsNo);
		OxCAS_Free(apucText);
		return -1;
	}

 	HxLOG_Info("[CIMMI]---> CiMmi_ProcMenuLast()---> ucNumOfChoice %d\n", ucNumOfChoice);

	// get sum of text length
	ulTextLen = 0;
	for (i=0; i<ucNumOfChoice+3; i++)
	{
		// check if tag is CI_TEXT_LAST or not
		ulApduTag = CIRH_GetApduTag(pucData);
		if (ulApduTag != CI_TEXT_LAST)
		{
			HxLOG_Error("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : fail to find CI_TEXT_LAST for SsNo[%d], Tag[0x%lx] !!!\n", usSsNo, ulApduTag);
			OxCAS_Free(apucText);
			OxCAS_Free(ausTextLen);
			return -1;
		}

		ausTextLen[i] = (unsigned short)CIRH_GetApduDataLength(pucData);
		if (ausTextLen[i] > MAX_CI_STR_LEN)	// text_char(0~40)
		{
			HxLOG_Info("[CIMMI]--->CiMmi_ProcMenuLast()---> Error : ausTextLen[%d] %d > %d\n", i, ausTextLen[i], MAX_CI_STR_LEN); // 대응이 되는 에러로 일반 Dbg 로 찍는다.
			//send close_mmi() to CAM & Host App.
			// to be added code by jhcho
		}

		ulTextLen += ausTextLen[i] + 1;	// null terminted string
		ucLenFieldSize = 1;

		pucData += (3 + ucLenFieldSize); // tag(3)+length_field(1)

		if(ausTextLen[i] == 0)
			apucText[i] = NULL;
		else
			apucText[i] = pucData;

		pucData += ausTextLen[i];
	}

	ulParamSize = sizeof(MMI_MENU_LIST_PARAM) + (unsigned long)ucNumOfChoice * sizeof(unsigned char *) + ulTextLen;

	pvParam = (void *)OxCAS_Malloc((unsigned int)ulParamSize);
	if (pvParam == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : memory not enough for SsNo[%d] !!!\n", usSsNo);
		OxCAS_Free(apucText);
		OxCAS_Free(ausTextLen);
		return -1;
	}

	pCiMenuParam = (MMI_MENU_LIST_PARAM *)pvParam;
	pCiMenuParam->ulNumOfItem = (unsigned int)ucNumOfChoice;

	pucData = (unsigned char *)pvParam + sizeof(MMI_MENU_LIST_PARAM);
	pCiMenuParam->ppszItem = (unsigned char **)pucData;
	pucData += sizeof(unsigned char *) * ucNumOfChoice;

	pCiMenuParam->pszTitle = pucData;
	HxSTD_memcpy(pucData, apucText[0], ausTextLen[0]);
	pucData[ausTextLen[0]] = 0;
	pucData += ausTextLen[0] + 1;

	pCiMenuParam->pszSubTitle = pucData;
	HxSTD_memcpy(pucData, apucText[1], ausTextLen[1]);
	pucData[ausTextLen[1]] = 0;
	pucData += ausTextLen[1] + 1;

	pCiMenuParam->pszBottom = pucData;
	HxSTD_memcpy(pucData, apucText[2], ausTextLen[2]);
	pucData[ausTextLen[2]] = 0;
	pucData += ausTextLen[2] + 1;

	for (i=0; i<ucNumOfChoice; i++)
	{
		pCiMenuParam->ppszItem[i] = pucData;
		HxSTD_memcpy(pucData, apucText[i+3], ausTextLen[i+3]);
		pucData[ausTextLen[i+3]] = 0;
		pucData += ausTextLen[i+3] + 1;
	}

	/* temp print... parkbi 020806 */
	#if 0
	{
		int kk, ll;

		HxLOG_Print( "\n########################################\n");
		HxLOG_Print( "***[CIMMI]---> CiMmi_ProcMenuLast() SsNo[%d]!!!\n", usSsNo);

		HxLOG_Print( "***[CIMMI]---> MENU TITLE:\n");
		for(kk=0; kk<ausTextLen[0]; kk++)
			HxLOG_Print( "%c", *(apucText[0] + kk));
		HxLOG_Print( "\n");

		/* test hex print */
		//for(kk=0; kk<ausTextLen[0]; kk++)
			//HxLOG_Print( "%x", *(apucText[0] + kk));
		//HxLOG_Print( "\n");

		HxLOG_Print( "***[CIMMI]---> MENU SUB-TITLE:\n");
		for(kk=0; kk<ausTextLen[1]; kk++)
			HxLOG_Print( "%c", *(apucText[1] + kk));
		HxLOG_Print( "\n");

		HxLOG_Print( "***[CIMMI]---> MENU BOTTOM:\n");
		for(kk=0; kk<ausTextLen[2]; kk++)
			HxLOG_Print( "%c", *(apucText[2] + kk));
		HxLOG_Print( "\n");

		HxLOG_Print( "***[CIMMI]---> MENU CHOICE:\n");
		for(ll=0; ll<ucNumOfChoice; ll++)
		{
			for(kk=0; kk<ausTextLen[ll+3]; kk++)
				HxLOG_Print( "%c", *(apucText[ll+3] + kk));
			HxLOG_Print( "\n");
		}

		HxLOG_Print( "########################################\n");
	}
	#endif

	OxCAS_Free(apucText);
	OxCAS_Free(ausTextLen);

	/******************************************
	  notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : can't find MMI Instance for SsNo[%d] !!!\n", usSsNo);
		OxCAS_Free(pvParam);	// must be freed by client(receiver)
		return -1;
	}

	HxLOG_Info("[CIMMI]---> CiMmi_ProcMenuLast()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo);

	Callback_Reset(g_hCbNotifyMmiReq);
	while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
	{
		(*pfnMmiReq)((MMI_HANDLE)hMmi, MMI_MENU, (void *)pvParam, ulParamSize);
	}

	return ERR_OK;
}

static int CiMmi_ProcListLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	MMI_MENU_LIST_PARAM	*pCiListParam;
	unsigned char		ucNumOfItem;
	void 				*pvParam;
	unsigned long		ulParamSize;
	unsigned char		*pucData;
	unsigned long		ulTextLen;
	unsigned long		ulApduTag;
	unsigned char 		**apucText;
	unsigned short		*ausTextLen;
	unsigned char		ucLenFieldSize;
	int	i;

	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;
	MMI_INSTANCE *hMmi;

	HxLOG_Info("CiMmi_ProcListLast() called...\n");

	pucData = pucApduData;
	if(pucData == NULL || ulApduLen == 0)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcListLast()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	/* get item number */
	ucNumOfItem = *pucData++;
	if (ucNumOfItem == 0xFF)
		ucNumOfItem = 0;

	HxLOG_Info("[CIMMI]---> CiMmi_ProcListLast()---> NumOfItem %d\n", ucNumOfItem);

	apucText = (unsigned char **)OxCAS_Malloc(sizeof(unsigned char *) * (ucNumOfItem+3));
	if (apucText == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcListLast()---> Error : memory not enough for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}
	ausTextLen = (unsigned short *)OxCAS_Malloc(sizeof(unsigned short) * (ucNumOfItem+3));
	if (ausTextLen == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcListLast()---> Error : memory not enough for SsNo[%d] !!!\n", usSsNo);
		OxCAS_Free(apucText);
		return -1;
	}

	// get sum of text length
	ulTextLen = 0;

	for (i=0; i<ucNumOfItem+3; i++)
	{
		// check if tag is CI_TEXT_LAST or not
		ulApduTag = CIRH_GetApduTag(pucData);
		if (ulApduTag != CI_TEXT_LAST)
		{
			HxLOG_Error("[CIMMI]---> CiMmi_ProcListLast()---> Error : fail to find CI_TEXT_LAST for SsNo[%d], Tag[0x%lx] !!!\n", usSsNo, ulApduTag);
			OxCAS_Free(apucText);
			OxCAS_Free(ausTextLen);
			return -1;
		}

		ausTextLen[i] = (unsigned short)CIRH_GetApduDataLength(pucData);
		if (ausTextLen[i] > MAX_CI_STR_LEN)	// text_char(0~40)
		{
			HxLOG_Info("[CIMMI]--->CiMmi_ProcListLast()---> Error : ausTextLen[%d] %d > %d\n", i, ausTextLen[i], MAX_CI_STR_LEN); // 대응이 되는 에러로 일반 Dbg 로 찍는다.
			//send close_mmi() to CAM & Host App.
			// to be added code by jhcho
		}

		ulTextLen += ausTextLen[i] + 1;	// null terminted string
		ucLenFieldSize = 1;

		pucData += (ucLenFieldSize + 3);

		/* get text data */
		if(ausTextLen[i] == 0)
			apucText[i] = NULL;
		else
			apucText[i] = pucData;

		pucData += ausTextLen[i];
	}

	ulParamSize = sizeof(MMI_MENU_LIST_PARAM) + (unsigned long)ucNumOfItem * sizeof(unsigned char *) + ulTextLen;

	pvParam = (void *)OxCAS_Malloc((unsigned int)ulParamSize); // must be freed by client(receiver)
	if (pvParam == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcListLast()---> Error : memory not enough for SsNo[%d] !!!\n", usSsNo);
		OxCAS_Free(apucText);
		OxCAS_Free(ausTextLen);
		return -1;
	}

	pCiListParam = (MMI_MENU_LIST_PARAM *)pvParam;
	pCiListParam->ulNumOfItem = (unsigned int)ucNumOfItem;

	pucData = (unsigned char *)pvParam + sizeof(MMI_MENU_LIST_PARAM);
	pCiListParam->ppszItem = (unsigned char **)pucData;
	pucData += sizeof(unsigned char *) * ucNumOfItem;

	pCiListParam->pszTitle = pucData;
	HxSTD_memcpy(pucData, apucText[0], ausTextLen[0]);
	pucData[ausTextLen[0]] = 0;
	pucData += ausTextLen[0] + 1;

	pCiListParam->pszSubTitle = pucData;
	HxSTD_memcpy(pucData, apucText[1], ausTextLen[1]);
	pucData[ausTextLen[1]] = 0;
	pucData += ausTextLen[1] + 1;

	pCiListParam->pszBottom = pucData;
	HxSTD_memcpy(pucData, apucText[2], ausTextLen[2]);
	pucData[ausTextLen[2]] = 0;
	pucData += ausTextLen[2] + 1;

	for (i=0; i<ucNumOfItem; i++)
	{
		pCiListParam->ppszItem[i] = pucData;
		HxSTD_memcpy(pucData, apucText[i+3], ausTextLen[i+3]);
		pucData[ausTextLen[i+3]] = 0;
		pucData += ausTextLen[i+3] + 1;
	}

	/* temp print... parkbi 020806 */
	#if 0
	{
		int kk, ll;

		HxLOG_Print( "\n########################################\n");
		HxLOG_Print( "***[CIMMI]---> CiMmi_ProcListLast() SsNo[%d]!!!\n", usSsNo);

		HxLOG_Print( "***[CIMMI]---> LIST TITLE:\n");
		for(kk=0; kk<ausTextLen[0]; kk++)
			HxLOG_Print( "%c", *(apucText[0] + kk));
		HxLOG_Print( "\n");

		HxLOG_Print( "***[CIMMI]---> LIST SUB-TITLE:\n");
		for(kk=0; kk<ausTextLen[1]; kk++)
			HxLOG_Print( "%c", *(apucText[1] + kk));
		HxLOG_Print( "\n");

		HxLOG_Print( "***[CIMMI]---> LIST BOTTOM:\n");
		for(kk=0; kk<ausTextLen[2]; kk++)
			HxLOG_Print( "%c", *(apucText[2] + kk));
		HxLOG_Print( "\n");

		HxLOG_Print( "***[CIMMI]---> LIST ITEM:\n");
		for(ll=0; ll<ucNumOfItem; ll++)
		{
			for(kk=0; kk<ausTextLen[ll+3]; kk++)
				HxLOG_Print( "%c", *(apucText[ll+3] + kk));
			HxLOG_Print( "\n");
		}

		HxLOG_Print( "########################################\n");
	}
	#endif

	OxCAS_Free(apucText);
	OxCAS_Free(ausTextLen);

	/******************************************
	  notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_ProcListLast()---> Error : can't find MMI Instance for SsNo[%d] !!!\n", usSsNo);
		OxCAS_Free(pvParam);
		return -1;
	}

	HxLOG_Info("[CIMMI]---> CiMmi_ProcListLast()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo);

	Callback_Reset(g_hCbNotifyMmiReq);
	while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
	{
		(*pfnMmiReq)((MMI_HANDLE)hMmi, MMI_LIST, (void *)pvParam, ulParamSize);
	}

	return ERR_OK;
}

static int CiMmi_AddInst(unsigned short usSsNo)
{
	MMI_INSTANCE * hMmi;
//	CI_SLOT_ID usSlotId;

 	HxLOG_Info("CiMmi_AddInst() called...\n");

	if (s_ucCiMmiCnt >= MAX_CI_MMI_SS)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_AddInst()---> Error : session is full !!!\n");
		return -1;
	}

	hMmi = CiMmi_GetNewInst();
	if (hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_AddInst()---> Error : instance is full !!!\n");
		return -1;
	}

//	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
//	{
//		HxLOG_Error("[CIMMI]---> CiMmi_AddInst()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
//		return -1;
//	}

	hMmi->usSsNo = usSsNo;

	s_ucCiMmiCnt++;

	return ERR_OK;
}

static int CiMmi_DeleteInst(unsigned short usSsNo)
{
	MMI_INSTANCE * hMmi;

 	HxLOG_Info("CiMmi_DeleteInst() called...\n");

	hMmi = CiMmi_GetInst(usSsNo);
	if (hMmi == NULL)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_DeleteInst()---> Error : SsNo[%d] incorrect !!!\n", usSsNo);
		return -1;
	}

	hMmi->usSsNo = 0;

	s_ucCiMmiCnt--;

	return ERR_OK;
}

static MMI_INSTANCE * CiMmi_GetNewInst(void)
{
	int i;

	for (i = 0; i < MAX_CI_MMI_SS; i++)
	{
		if (s_aCiMmi[i].usSsNo == 0)
			return &s_aCiMmi[i];
	}

	return NULL;
}

static MMI_INSTANCE * CiMmi_GetInst(unsigned short usSsNo)
{
	int i;

	for (i = 0; i < MAX_CI_MMI_SS; i++)
	{
		if (s_aCiMmi[i].usSsNo == usSsNo)
		{
			HxLOG_Info("[CIMMI]---> CiMmi_GetInst()---> %d th instance found\n", i);
			return &s_aCiMmi[i];
		}
	}

	return NULL;
}

HERROR CI_CheckMmiInstance(MMI_HANDLE hMmi)
{
	int i;

	for (i = 0; i < MAX_CI_MMI_SS; i++)
	{
		if (&s_aCiMmi[i] == (MMI_INSTANCE *)hMmi)
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

#if 1
MMI_INSTANCE * CI_MmiGetInstBySlotId(CI_SLOT_ID usSlotId) /* 이 함수는 테스트 용도로만 사용된다. 일반적인 용도로 사용하지 말 것!! */
{
	int i;
	CI_SS_HANDLE hSs;

	for (i = 0; i < MAX_CI_MMI_SS; i++)
	{
		if (s_aCiMmi[i].usSsNo)
		{
			hSs = CIRH_GetSs(s_aCiMmi[i].usSsNo);
			if (hSs == NULL)
				continue;
			if (hSs->hTc == NULL)
				continue;
			if (hSs->hTc->ucTcId == 0)
				continue;
			if (hSs->hTc->usSlotId >= svc_cas_DevCiGetNumberOfModule())
				continue;
			if (hSs->hTc->usSlotId == usSlotId)
			{
				HxLOG_Info("[CIMMI]---> CI_MmiGetInstBySlotId()---> %d th instance found\n", i);
				return &s_aCiMmi[i];
			}
		}
	}

	return NULL;
}
#endif

int CiMmi_SendCloseMmi(unsigned short usSsNo)
{
	unsigned char aucMsg[5];

	HxLOG_Info("CiMmi_SendCloseMmi() called...\n");

	aucMsg[0] = (unsigned char)((CI_CLOSE_MMI >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_CLOSE_MMI >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_CLOSE_MMI & 0xff);
	aucMsg[3] = 0x01;
	aucMsg[4] = CI_MMI_CLOSE_IMMEDIATE;

	if (CIRH_SendApdu(usSsNo, aucMsg, 5) != ERR_OK)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_SendCloseMmi()---> Error : fail to send CI_CLOSE_MMI for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIMMI]---> CiMmi_SendCloseMmi()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

/*
menu_answ () {
				menu_answ_tag 		: 24 uimsbf
				length_field() = 1
				choice_ref 			: 8 uimsbf
			}
*/

/* choice number 가 '0' 이면 'cancel' 을 의미한다..
 * 사실 nMmiReq 는 의미가 없다. notify 에서 준 값을 그대로 다시 받을뿐이당..
 * 현재 상태가 메뉴인지 리스트인지 ci extension 입장에서는 확인할 수도 없당..
 */
int CiMmi_SendMenuAnsw(unsigned short usSsNo, MMI_REQ nMmiReq, unsigned char ucChoiceNo)
{
	unsigned char aucMsg[5];

	/* BS : Not Used */
	nMmiReq = 0;

 	HxLOG_Info("CiMmi_SendMenuAnsw() called...\n");

	/* set apdu tag */
	aucMsg[0] = (unsigned char)((CI_MENU_ANSW >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_MENU_ANSW >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_MENU_ANSW & 0xff);

	/* set length field */
	aucMsg[3] = 0x01;

	/* set answ_id */
	aucMsg[4] = ucChoiceNo;

	if (CIRH_SendApdu(usSsNo, aucMsg, 5) != ERR_OK)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_SendMenuAnsw()---> Error : fail to send CI_MENU_ANSW for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIMMI]---> CiMmi_SendMenuAnsw()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

/*
answ () {
			answ_tag 				: 24 uimsbf
			length_field()			: 8
			answ_id 				: 8 uimsbf
			if (answ_id == answer) {
				for (i=0;i<length;i++)
				text_char 			: 8 uimsbf
			}
		}
*/
/* 'pucStr' 에 'NULL' 을 받으면 'cancel' 으로 처리한당.
 * ci extension API reference manual 에 나와 있당....
 */
int CiMmi_SendAnswString(unsigned short usSsNo, unsigned char *pucStr, unsigned short usStrSize)
{
	unsigned char	aucMsg[5+MAX_CI_STR_LEN]; // tag(3) + length_field(1) + answer_id(1) + text_char(0~40) --> max:45
	unsigned char	ucMsgLen;
	int	i = 0;

 	HxLOG_Info("CiMmi_SendAnswString() called...\n");

	/* set apdu tag */
	aucMsg[i++] = (unsigned char)((CI_ANSW >> 16) & 0xff);
	aucMsg[i++] = (unsigned char)((CI_ANSW >> 8) & 0xff);
	aucMsg[i++] = (unsigned char)(CI_ANSW & 0xff);

	if(pucStr == NULL) /* case 'cancel' */
	{
		HxLOG_Info("[CIMMI]---> CiMmi_SendAnswString()---> cancel\n");

		/* set length field */
		aucMsg[i++] = 0x01; /* only answ_id field */

		/* set answ_id */
		aucMsg[i++] = CI_MMI_ANSW_CANCEL;
		ucMsgLen = 5;
	}
	else /* case answer */
	{
		HxLOG_Info("[CIMMI]---> CiMmi_SendAnswString()---> ans : %s\n", pucStr);

#if 1
		if (usStrSize > MAX_CI_STR_LEN)	// text_char(0~40)
		{
			HxLOG_Error("[CIMMI]--->CiMmi_SendAnswString()---> Error : usStrSize %d > %d\n", usStrSize, MAX_CI_STR_LEN);
			usStrSize = MAX_CI_STR_LEN;	// or send close_mmi() to CAM
		}
#else
		if (usStrSize > MAX_CI_STR_LEN)	// text_char(0~40)
		{
			// send close_mmi() to CAM & Host App.
			// to be added code by jhcho
		}
#endif

		/* set length field */
		aucMsg[i++] = (unsigned char)(usStrSize + 1); /* answ_id field + usStrSize chars */

		/* set answ_id */
		aucMsg[i++] = CI_MMI_ANSW_ANSWER;

		/* copy text_char */
		HxSTD_memcpy(&aucMsg[i], pucStr, usStrSize);
//		OxCAS_Free(pucStr);	// must be freed by client(receiver) : 이 경우 number일텐데 통상 alloc된 메모리가 아니라 배열이다. 확인하자... -> OCTO는 alloc된 메모리 사용함 -> static array 사용으로 변경됨

		ucMsgLen = usStrSize + 5;
	}

	if (CIRH_SendApdu(usSsNo, aucMsg, ucMsgLen) != ERR_OK)
	{
		HxLOG_Error("[CIMMI]---> CiMmi_SendAnswString()---> Error : fail to send CI_ANSW for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
	CiMmi_SetUserInputWaitingFlag(0);
#endif

	HxLOG_Info("[CIMMI]---> CiMmi_SendAnswString()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

#ifdef SUPPORT_SMART_CI_MMI_PINCODE

static int CiMmi_SetUserInputWaitingFlag(unsigned long value)
{
	fWaitingUserInputInMMI = value;

	return ERR_OK;
}

int CiMmi_CheckWaitingUserInput(CI_SLOT_ID usSlotId, unsigned long* IsWaiting)
{
	if (fWaitingUserInputInMMI == 0) {
		*IsWaiting = 0;
	} else {
		*IsWaiting = 1;
	}

	return ERR_OK;
}

#endif

