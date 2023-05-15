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
#include "cb_list.h"

#include "ci.h"
#include "cipack_msg.h"
#include "ci_reshandler.h"
#include "ci_mmi.h"
#include "pal_ci.h"

/***************************************************************
* definition
***************************************************************/



/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE		g_hCbNotifyMmiReq;

//2009.02.06. BSPARK, Mmi Open State�� Ȯ���ϱ� ���� --> �̻��� �ڵ屺��. ���� �ʿ��մϴ�!
HBOOL g_bMmiOpened = FALSE;


/***************************************************************
* static variables and structures
***************************************************************/
static const unsigned char s_CiCharTable[] =
{
	" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
};

static MMI_INSTANCE	s_aCiMmi[MAX_CI_MMI_SS];
static unsigned char	s_ucCiMmiCnt;
#ifdef SUPPORT_SMART_CI_MMI_PINCODE
static unsigned long fWaitingUserInputInMMI = 0;
#endif


/***************************************************************
* static function prototypes
***************************************************************/
static int CiMmi_ProcMmiClose(unsigned short usSsNo,
				unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcDisplayControl(unsigned short usSsNo,
				unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcTextLast(unsigned short usSsNo,
				unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcEnq(unsigned short usSsNo,
				unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcMenuLast(unsigned short usSsNo,
				unsigned char *pucApduData, unsigned long ulApduLen);
static int CiMmi_ProcListLast(unsigned short usSsNo,
				unsigned char *pucApduData, unsigned long ulApduLen);

static int CiMmi_AddInst(unsigned short usSsNo);
static int CiMmi_DeleteInst(unsigned short usSsNo);
static MMI_INSTANCE * CiMmi_GetNewInst(void);
static MMI_INSTANCE * CiMmi_GetInst(unsigned short usSsNo);


/***************************************************************
* function bodies
***************************************************************/
int CiMmi_Init(void)
{
	VK_memset(s_aCiMmi, 0, sizeof(MMI_INSTANCE) * MAX_CI_MMI_SS);

	s_ucCiMmiCnt = 0;

	return ERR_OK;
}

int CiMmi_SsOpenedProc(unsigned short usSsNo)
{
	MMI_INSTANCE *hMmi;
	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;

	PrintDbg(("CiMmi_SsOpenedProc() called...\n"));

	if (CiMmi_AddInst(usSsNo) < 0)
	{
		PrintErr(("[CIMMI]---> CiMmi_SsOpenedProc()---> Error : can't add MMI Instance : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	/******************************************
	 notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_SsOpenedProc()---> Error : can't find MMI Instance : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIMMI]---> CiMmi_SsOpenedProc()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo));

	/* get notify function list and notify mmi */
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
		PrintErr(("[CIMMI]---> CiMmi_SsClosedProc()---> Error : can't find MMI Instance : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIMMI]---> CiMmi_SsClosedProc()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo));

	Callback_Reset(g_hCbNotifyMmiReq);
    while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
    {
    	pfnMmiReq((MMI_HANDLE)hMmi, MMI_CLOSE, NULL, 0);
    }
	g_bMmiOpened = FALSE;

	if (CiMmi_DeleteInst(usSsNo) < 0)
	{
		PrintErr(("[CIMMI]---> CiMmi_SsClosedProc()---> CiMmi_DeleteInst() fail. SsNo[%d]\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIMMI]---> CiMmi_SsClosedProc()---> success SsNo[%d]\n", usSsNo));

	return ERR_OK;
}

int CiMmi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	PrintDbg(("CiMmi_ApduProc() called...\n"));

	switch (ulApduTag)
	{
	/* used in both mode(Low-Level/High-Level) */
	case CI_CLOSE_MMI:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_CLOSE_MMI tag...\n"));
		nRtnCode = CiMmi_ProcMmiClose(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_DISPLAY_CONTROL:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_DISPLAY_CONTROL tag...\n"));
		nRtnCode = CiMmi_ProcDisplayControl(usSsNo, pucApduData, ulApduLen);
		break;
	/* High-Level MMI objects */
	case CI_TEXT_LAST:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_TEXT_LAST tag...\n"));
		nRtnCode = CiMmi_ProcTextLast(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_TEXT_MORE:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_TEXT_MORE tag...\n"));
		break;
	case CI_ENQ:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_ENQ tag...\n"));
		nRtnCode = CiMmi_ProcEnq(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_MENU_LAST:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_MENU_LAST tag...\n"));
		nRtnCode = CiMmi_ProcMenuLast(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_MENU_MORE:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_MENU_MORE tag...\n"));
		break;
	case CI_LIST_LAST:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_LIST_LAST tag...\n"));
		nRtnCode = CiMmi_ProcListLast(usSsNo, pucApduData, ulApduLen);
		break;
	case CI_LIST_MORE:
		PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> Received CI_LIST_MORE tag...\n"));
		break;
	/* Low-Level MMI Keypad object not supported, out of consideration */
	/* Low-Level MMI Display object not supported */
	case CI_SUBTITLE_SEG_LAST:			// host <-- app, ���� �������� ����.
	case CI_SUBTITLE_SEG_MORE:			// host --> app, ���� �������� ����.
	case CI_DISPLAY_MSG:				// host <-- app, ���� �������� ����.
	case CI_SCENE_END_MARK	:			// host <-- app, ���� �������� ����.
	case CI_SCENE_DONE:					// host <-- app, ���� �������� ����.
	case CI_SCENE_CONTROL:				// host --> app, ���� �������� ����.
	case CI_SUBTITLE_DOWNLOAD_LAST:		// host <-- app, ���� �������� ����.
	case CI_SUBTITLE_DOWNLOAD_MORE:		// host --> app, ���� �������� ����.
	case CI_FLUSH_DOWNLOAD:				// host <-- app, ���� �������� ����.
	case CI_DOWNLOAD_REPLY:				// host <-- app, ���� �������� ����.
		nRtnCode = -1;
		PrintErr(("[CIMMI]---> CiMmi_ApduProc()---> Error : not support Apdu Tag[0x%lx] !!!\n", ulApduTag));
		break;
	default:
		nRtnCode = -1;
		PrintErr(("[CIMMI]---> CiMmi_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	PrintDbg(("[CIMMI]---> CiMmi_ApduProc()---> return code[%d], SsNo[%d], tag[0x%lx]\n", nRtnCode, usSsNo, ulApduTag));

	return nRtnCode;
}

int CiMmi_SendCloseMmi(unsigned short usSsNo)
{
	unsigned char aucMsg[5];

	PrintDbg(("CiMmi_SendCloseMmi() called...\n"));

	aucMsg[0] = (unsigned char)((CI_CLOSE_MMI >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_CLOSE_MMI >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_CLOSE_MMI & 0xff);
	aucMsg[3] = 0x01;
	aucMsg[4] = CI_MMI_CLOSE_IMMEDIATE;

	if (CIRH_SendApdu(usSsNo, aucMsg, 5) != ERR_OK)
	{
		PrintErr(("[CIMMI]---> CiMmi_SendCloseMmi()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIMMI]---> CiMmi_SendCloseMmi()---> SsNo[%d]\n", usSsNo));

	return ERR_OK;
}

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
int CiMmi_CheckWaitingUserInput(CI_SLOT_ID usSlotId, unsigned long* IsWaiting)
{
#if 0
	MMI_INSTANCE *hMmi;
	unsigned short usSsNo;
	/******************************************
	 notify mmi
	 ******************************************/
	usSsNo = CiMmi_GetSsNo(usSlotId);
	hMmi = CiMmi_GetInst(usSsNo);

	if(hMmi == NULL)
	{
		return -1;
	}
#endif

	if (fWaitingUserInputInMMI == 0) {
		*IsWaiting = 0;
	} else {
		*IsWaiting = 1;
	}

	return ERR_OK;
}

static int CiMmi_SetUserInputWaitingFlag(unsigned long value)
{
	fWaitingUserInputInMMI = value;

	return ERR_OK;
}
#endif

static int CiMmi_ProcMmiClose(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char ucDelaySec;
	MMI_INSTANCE *hMmi;
	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;

	PrintDbg(("CiMmi_ProcMmiClose() called...\n"));

	if(pucApduData == NULL || ulApduLen < 1)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMmiClose()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMmiClose()---> Error : can't find MMI Instance : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	if (CIRH_ReqCloseSs(usSsNo) < 0)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMmiClose(),CIRH_ReqCloseSs()---> Error : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	if (pucApduData[0] == CI_MMI_CLOSE_IMMEDIATE)
	{
		PrintDbg(("[CIMMI]---> CiMmi_ProcMmiClose()---> CI_MMI_CLOSE_IMMEDIATE\n"));
		ucDelaySec = 0;
	}
	else if (pucApduData[0] == CI_MMI_CLOSE_DELAY)
	{
		PrintDbg(("[CIMMI]---> CiMmi_ProcMmiClose()---> CI_MMI_CLOSE_DELAY\n"));
		ucDelaySec = pucApduData[1];
		PrintDbg(("[CIMMI]---> CiMmi_ProcMmiClose()---> delay %d sec\n", ucDelaySec));
	}
	else
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMmiClose()---> Error : Unknown cmd_id[0x%x] !!!\n", pucApduData[0]));
		return -1;
	}

	/******************************************
	 notify mmi
	 ******************************************/
	PrintDbg(("[CIMMI]---> CiMmi_ProcMmiClose()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo));

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

	PrintDbg(("CiMmi_ProcDisplayControl() called...\n"));

	if(pucApduData == NULL || ulApduLen < 1)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
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
				PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl(CI_MMI_MODE_ACK)---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
				return -1;
			}
			PrintDbg(("[CIMMI]---> CiMmi_ProcDisplayControl()---> SET_MMI_MODE SsNo[%d]\n", usSsNo));
		}
		else
		{
			aucMsg[i++] = 0x01;
			aucMsg[i++] = CI_UNKNOWN_MMI_MODE;
			if (CIRH_SendApdu(usSsNo, aucMsg, i) != ERR_OK)
			{
				PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl(CI_UNKNOWN_MMI_MODE)---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
				return -1;
			}
			PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl()---> Error : unknown mmi mode : SsNo[%d] !!!\n", usSsNo));
			return -1;
		}
		break;

	case GET_DISPLAY_CHAR_TABLE_LIST:
		ulLen = sizeof(s_CiCharTable) + 1;
		CIRH_ConvertApduLengthField(ulLen, &aucMsg[i], &ucLenFieldSize);
		i += ucLenFieldSize;
		aucMsg[i++] = CI_LIST_DISPLAY_CHAR_TABLES;
		VK_memcpy(&aucMsg[i], s_CiCharTable, sizeof(s_CiCharTable)); // VK_memcpy(&aucMsg[i++], s_CiCharTable, sizeof(s_CiCharTable));
		if (CIRH_SendApdu(usSsNo, aucMsg, i + sizeof(s_CiCharTable)) != ERR_OK)
		{
			PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl(CI_LIST_DISPLAY_CHAR_TABLES)---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
			return -1;
		}
		PrintDbg(("[CIMMI]---> CiMmi_ProcDisplayControl()---> GET_DISPLAY_CHAR_TABLE_LIST SsNo[%d]\n", usSsNo));
		break;

	case GET_INPUT_CHAR_TABLE_LIST:
		ulLen = sizeof(s_CiCharTable) + 1;
		CIRH_ConvertApduLengthField(ulLen, &aucMsg[i], &ucLenFieldSize);
		i += ucLenFieldSize;
		aucMsg[i++] = CI_LIST_INPUT_CHAR_TABLES;
		VK_memcpy(&aucMsg[i], s_CiCharTable, sizeof(s_CiCharTable)); // VK_memcpy(&aucMsg[i++], s_CiCharTable, sizeof(s_CiCharTable));
		if (CIRH_SendApdu(usSsNo, aucMsg, i + sizeof(s_CiCharTable)) != ERR_OK)
		{
			PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl(CI_LIST_INPUT_CHAR_TABLES)---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
			return -1;
		}
		PrintDbg(("[CIMMI]---> CiMmi_ProcDisplayControl()---> GET_INPUT_CHAR_TABLE_LIST SsNo[%d]\n", usSsNo));
		break;

	case GET_OVERLAY_GRAPHICS_CHARACTERISTICS:
	case GET_FULL_SCREEN_GRAPHICS_CHARACTERISTICS:
		aucMsg[i++] = 0x01;
		aucMsg[i++] = CI_UNKNOWN_DISPLAY_CONTROL_CMD;
		if (CIRH_SendApdu(usSsNo, aucMsg, i) != ERR_OK)
		{
			PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl(CI_UNKNOWN_DISPLAY_CONTROL_CMD)---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
			return -1;
		}
		PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl()---> Error : not support display control cmd : SsNo[%d] !!!\n", usSsNo));
		return -1;

	default:
		PrintErr(("[CIMMI]---> CiMmi_ProcDisplayControl()---> Error : unknown display control cmd : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	return ERR_OK;
}


static int CiMmi_ProcTextLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char *pucText = NULL;
	CI_NOTIFY_MMI_REQ_FUNC pfnMmiReq;
	MMI_INSTANCE *hMmi = NULL;

	PrintDbg(("CiMmi_ProcTextLast() called...\n"));

	if(pucApduData == NULL || ulApduLen == 0)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcTextLast()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	if (ulApduLen > MAX_CI_STR_LEN)	// text_char(0~40)
	{
		PrintErr(("[CIMMI]--->CiMmi_ProcTextLast()---> Error : ulApduLen %ld > %d\n", ulApduLen, MAX_CI_STR_LEN));
		ulApduLen = MAX_CI_STR_LEN;	// or send close_mmi() to CAM
	}

	pucText = (unsigned char *)CA_MEM_Alloc(ulApduLen);	// be freed by receiver(client)
	if (pucText == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcTextLast()---> Error : memory not enough : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	VK_MEM_Memcpy(pucText, pucApduData, ulApduLen);

	/******************************************
	 notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcTextLast()---> Error : can't find MMI Instance : SsNo[%d] !!!\n", usSsNo));
		CA_MEM_Free(pucText);
		return -1;
	}

	PrintDbg(("[CIMMI]---> CiMmi_ProcTextLast()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo));

	Callback_Reset(g_hCbNotifyMmiReq);
    while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
    {
    	(*pfnMmiReq)((MMI_HANDLE)hMmi, MMI_TEXT, (void *)pucText, ulApduLen);
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

 	PrintDbg(("CiMmi_ProcEnq() called...\n"));

	if(pucApduData == NULL || ulApduLen < 2)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcEnq()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
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

	PrintDbg(("[CIMMI]---> CiMmi_ProcEnq()---> ulParamSize[%ld]\n", ulParamSize));

	pvParam = (void *)CA_MEM_Alloc((unsigned int)ulParamSize); // be freed by receiver(client)
	if (pvParam == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcEnq()---> Error : memory not enough : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	pCiEnqParam = (MMI_ENQ_PARAM *)pvParam;
	pCiEnqParam->ucBlindOnOff = pucApduData[0] & 0x01;
	pCiEnqParam->ucAnswerLen = pucApduData[1];
	pCiEnqParam->pszPrompt = (unsigned char *)pvParam + sizeof(MMI_ENQ_PARAM);
	VK_memcpy(pCiEnqParam->pszPrompt, &pucApduData[2], ulApduLen - 2);
	((unsigned char *)pvParam)[ulParamSize -1] = 0x00;


	/******************************************
	 notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcEnq()---> Error : can't find MMI Instance : SsNo[%d] !!!\n", usSsNo));
		CA_MEM_Free(pvParam);
		return -1;
	}

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
	CiMmi_SetUserInputWaitingFlag(1);
#endif

	PrintDbg(("[CIMMI]---> CiMmi_ProcEnq()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo));

	Callback_Reset(g_hCbNotifyMmiReq);
    while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
    {
    	(*pfnMmiReq)((MMI_HANDLE)hMmi, MMI_ENQ, (void *)pvParam, ulParamSize);
    }

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
/* 'pucStr' �� 'NULL' �� ������ 'cancel' ���� ó���Ѵ�.
 * ci extension API reference manual �� ���� �ִ�....
 */
int CiMmi_SendAnswString(unsigned short usSsNo, unsigned char *pucStr, unsigned short usStrSize)
{
	unsigned char	aucMsg[5+MAX_CI_STR_LEN]; // tag(3) + length_field(1) + answer_id(1) + text_char(0~40) --> max:45
	unsigned char	ucMsgLen;
	int	i = 0;

 	PrintDbg(("CiMmi_SendAnswString() called...\n"));

	/* set apdu tag */
	aucMsg[i++] = (unsigned char)((CI_ANSW >> 16) & 0xff);
	aucMsg[i++] = (unsigned char)((CI_ANSW >> 8) & 0xff);
	aucMsg[i++] = (unsigned char)(CI_ANSW & 0xff);

	if(pucStr == NULL) /* case 'cancel' */
	{
		PrintDbg(("[CIMMI]---> CiMmi_SendAnswString()---> cancel\n"));

		/* set length field */
		aucMsg[i++] = 0x01; /* only answ_id field */

		/* set answ_id */
		aucMsg[i++] = CI_MMI_ANSW_CANCEL;
		ucMsgLen = 5;
	}
	else /* case answer */
	{
		PrintDbg(("[CIMMI]---> CiMmi_SendAnswString()---> ans : %s\n", pucStr));

#if 1
		if (usStrSize > MAX_CI_STR_LEN)	// text_char(0~40)
		{
			PrintErr(("[CIMMI]--->CiMmi_SendAnswString()---> Error : usStrSize %d > %d\n", usStrSize, MAX_CI_STR_LEN));
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
		VK_memcpy(&aucMsg[i], pucStr, usStrSize);
//		CA_MEM_Free(pucStr);	// must be freed by client(receiver) : �� ��� number���ٵ� ��� alloc�� �޸𸮰� �ƴ϶� �迭�̴�. Ȯ������... -> OCTO�� alloc�� �޸� ����� -> static array ������� �����

		ucMsgLen = usStrSize + 5;
	}

	if (CIRH_SendApdu(usSsNo, aucMsg, ucMsgLen) != ERR_OK)
	{
		PrintErr(("[CIMMI]---> CiMmi_SendAnswString()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
	CiMmi_SetUserInputWaitingFlag(0);
#endif

	PrintDbg(("[CIMMI]---> CiMmi_SendAnswString()---> SsNo[%d]\n", usSsNo));

	return ERR_OK;
}

/*
menu_answ () {
				menu_answ_tag 		: 24 uimsbf
				length_field() = 1
				choice_ref 			: 8 uimsbf
			}
*/

/* choice number �� '0' �̸� 'cancel' �� �ǹ��Ѵ�..
 * ��� nMmiReq �� �ǹ̰� ����. notify ���� �� ���� �״�� �ٽ� �������̴�..
 * ���� ���°� �޴����� ����Ʈ���� ci extension ���忡���� Ȯ���� ���� ����..
 */
int CiMmi_SendMenuAnsw(unsigned short usSsNo, MMI_REQ nMmiReq, unsigned char ucChoiceNo)
{
	unsigned char aucMsg[5];

	/* BS : Not Used */
	nMmiReq = 0;
    if ( nMmiReq == 0 )
    {
    }

 	PrintDbg(("CiMmi_SendMenuAnsw() called...\n"));

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
		PrintErr(("[CIMMI]---> CiMmi_SendMenuAnsw()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIMMI]---> CiMmi_SendMenuAnsw()---> SsNo[%d]\n", usSsNo));

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

 	PrintDbg(("CiMmi_ProcMenuLast() called...\n"));

	pucData = pucApduData;
	if(pucData == NULL || ulApduLen == 0)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	/* get item number */
	ucNumOfChoice = *pucData++;
	if (ucNumOfChoice == 0xFF)
		ucNumOfChoice = 0;

	PrintDbg(("[CIMMI]---> CiMmi_ProcMenuLast()---> NumOfChoice %d\n", ucNumOfChoice));

	apucText = (unsigned char **)CA_MEM_Alloc(sizeof(unsigned char *) * (ucNumOfChoice+3));
	if (apucText == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : memory not enough : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	/* BS: Menu List�� ���޵Ǵ� String �� ������ Size ���� */
	ausTextLen = (unsigned short *)CA_MEM_Alloc(sizeof(unsigned short) * (ucNumOfChoice+3));
	if (ausTextLen == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : memory not enough : SsNo[%d] !!!\n", usSsNo));
		CA_MEM_Free(apucText);
		return -1;
	}

 	PrintDbg(("[CIMMI]---> CiMmi_ProcMenuLast()---> ucNumOfChoice %d\n", ucNumOfChoice));

	// get sum of text length
	ulTextLen = 0;
	for (i=0; i<ucNumOfChoice+3; i++)
	{
		// check if tag is CI_TEXT_LAST or not
		ulApduTag = CIRH_GetApduTag(pucData);
		if (ulApduTag != CI_TEXT_LAST)
		{
			PrintErr(("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : fail to find CI_TEXT_LAST : SsNo[%d], Tag[0x%lx] !!!\n", usSsNo, ulApduTag));
			CA_MEM_Free(apucText);
			CA_MEM_Free(ausTextLen);
			return -1;
		}

		ausTextLen[i] = (unsigned short)CIRH_GetApduDataLength(pucData);
		if (ausTextLen[i] > MAX_CI_STR_LEN)	// text_char(0~40)
		{
			PrintDbg(("[CIMMI]--->CiMmi_ProcMenuLast()---> Error : ausTextLen[%d] %d > %d\n", i, ausTextLen[i], MAX_CI_STR_LEN)); // ������ �Ǵ� ������ �Ϲ� Dbg �� ��´�.
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

	pvParam = (void *)CA_MEM_Alloc((unsigned int)ulParamSize);
	if (pvParam == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : memory not enough : SsNo[%d] !!!\n", usSsNo));
		CA_MEM_Free(apucText);
		CA_MEM_Free(ausTextLen);
		return -1;
	}

	pCiMenuParam = (MMI_MENU_LIST_PARAM *)pvParam;
	pCiMenuParam->ucNumOfItem = ucNumOfChoice;

	pucData = (unsigned char *)pvParam + sizeof(MMI_MENU_LIST_PARAM);
	pCiMenuParam->ppszItem = (unsigned char **)pucData;
	pucData += sizeof(unsigned char *) * ucNumOfChoice;

	pCiMenuParam->pszTitle = pucData;
	VK_memcpy(pucData, apucText[0], ausTextLen[0]);
	pucData[ausTextLen[0]] = 0;
	pucData += ausTextLen[0] + 1;

	pCiMenuParam->pszSubTitle = pucData;
	VK_memcpy(pucData, apucText[1], ausTextLen[1]);
	pucData[ausTextLen[1]] = 0;
	pucData += ausTextLen[1] + 1;

	pCiMenuParam->pszBottom = pucData;
	VK_memcpy(pucData, apucText[2], ausTextLen[2]);
	pucData[ausTextLen[2]] = 0;
	pucData += ausTextLen[2] + 1;

	for (i=0; i<ucNumOfChoice; i++)
	{
		pCiMenuParam->ppszItem[i] = pucData;
		VK_memcpy(pucData, apucText[i+3], ausTextLen[i+3]);
		pucData[ausTextLen[i+3]] = 0;
		pucData += ausTextLen[i+3] + 1;
	}

	/* temp print... parkbi 020806 */
	#if 0
	{
		int kk, ll;

		VK_Print( "\n########################################\n");
		VK_Print( "***[CIMMI]---> CiMmi_ProcMenuLast() SsNo[%d]!!!\n", usSsNo);

		VK_Print( "***[CIMMI]---> MENU TITLE:\n");
		for(kk=0; kk<ausTextLen[0]; kk++)
			VK_Print( "%c", *(apucText[0] + kk));
		VK_Print( "\n");

		/* test hex print */
		//for(kk=0; kk<ausTextLen[0]; kk++)
			//VK_Print( "%x", *(apucText[0] + kk));
		//VK_Print( "\n");

		VK_Print( "***[CIMMI]---> MENU SUB-TITLE:\n");
		for(kk=0; kk<ausTextLen[1]; kk++)
			VK_Print( "%c", *(apucText[1] + kk));
		VK_Print( "\n");

		VK_Print( "***[CIMMI]---> MENU BOTTOM:\n");
		for(kk=0; kk<ausTextLen[2]; kk++)
			VK_Print( "%c", *(apucText[2] + kk));
		VK_Print( "\n");

		VK_Print( "***[CIMMI]---> MENU CHOICE:\n");
		for(ll=0; ll<ucNumOfChoice; ll++)
		{
			for(kk=0; kk<ausTextLen[ll+3]; kk++)
				VK_Print( "%c", *(apucText[ll+3] + kk));
			VK_Print( "\n");
		}

		VK_Print( "########################################\n");
	}
	#endif

	CA_MEM_Free(apucText);
	CA_MEM_Free(ausTextLen);

	/******************************************
	 notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcMenuLast()---> Error : can't find MMI Instance : SsNo[%d] !!!\n", usSsNo));
	    CA_MEM_Free(pvParam);	// must be freed by client(receiver)
		return -1;
	}

	PrintDbg(("[CIMMI]---> CiMmi_ProcMenuLast()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo));

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

	PrintDbg(("CiMmi_ProcListLast() called...\n"));

	pucData = pucApduData;
	if(pucData == NULL || ulApduLen == 0)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcListLast()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	/* get item number */
	ucNumOfItem = *pucData++;
	if (ucNumOfItem == 0xFF)
		ucNumOfItem = 0;

	PrintDbg(("[CIMMI]---> CiMmi_ProcListLast()---> NumOfItem %d !!!\n", ucNumOfItem));

	apucText = (unsigned char **)CA_MEM_Alloc(sizeof(unsigned char *) * (ucNumOfItem+3));
	if (apucText == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcListLast()---> Error : memory not enough : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}
	ausTextLen = (unsigned short *)CA_MEM_Alloc(sizeof(unsigned short) * (ucNumOfItem+3));
	if (ausTextLen == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcListLast()---> Error : memory not enough : SsNo[%d] !!!\n", usSsNo));
		CA_MEM_Free(apucText);
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
			PrintErr(("[CIMMI]---> CiMmi_ProcListLast()---> Error : fail to find CI_TEXT_LAST : SsNo[%d], Tag[0x%lx] !!!\n", usSsNo, ulApduTag));
			CA_MEM_Free(apucText);
			CA_MEM_Free(ausTextLen);
			return -1;
		}

		ausTextLen[i] = (unsigned short)CIRH_GetApduDataLength(pucData);
		if (ausTextLen[i] > MAX_CI_STR_LEN)	// text_char(0~40)
		{
			PrintDbg(("[CIMMI]--->CiMmi_ProcListLast()---> Error : ausTextLen[%d] %d > %d\n", i, ausTextLen[i], MAX_CI_STR_LEN)); // ������ �Ǵ� ������ �Ϲ� Dbg �� ��´�.
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

	pvParam = (void *)CA_MEM_Alloc((unsigned int)ulParamSize); // must be freed by client(receiver)
	if (pvParam == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcListLast()---> Error : memory not enough : SsNo[%d] !!!\n", usSsNo));
		CA_MEM_Free(apucText);
		CA_MEM_Free(ausTextLen);
		return -1;
	}

	pCiListParam = (MMI_MENU_LIST_PARAM *)pvParam;
	pCiListParam->ucNumOfItem = ucNumOfItem;

	pucData = (unsigned char *)pvParam + sizeof(MMI_MENU_LIST_PARAM);
	pCiListParam->ppszItem = (unsigned char **)pucData;
	pucData += sizeof(unsigned char *) * ucNumOfItem;

	pCiListParam->pszTitle = pucData;
	VK_memcpy(pucData, apucText[0], ausTextLen[0]);
	pucData[ausTextLen[0]] = 0;
	pucData += ausTextLen[0] + 1;

	pCiListParam->pszSubTitle = pucData;
	VK_memcpy(pucData, apucText[1], ausTextLen[1]);
	pucData[ausTextLen[1]] = 0;
	pucData += ausTextLen[1] + 1;

	pCiListParam->pszBottom = pucData;
	VK_memcpy(pucData, apucText[2], ausTextLen[2]);
	pucData[ausTextLen[2]] = 0;
	pucData += ausTextLen[2] + 1;

	for (i=0; i<ucNumOfItem; i++)
	{
		pCiListParam->ppszItem[i] = pucData;
		VK_memcpy(pucData, apucText[i+3], ausTextLen[i+3]);
		pucData[ausTextLen[i+3]] = 0;
		pucData += ausTextLen[i+3] + 1;
	}

	/* temp print... parkbi 020806 */
	#if 0
	{
		int kk, ll;

		VK_Print( "\n########################################\n");
		VK_Print( "***[CIMMI]---> CiMmi_ProcListLast() SsNo[%d]!!!\n", usSsNo);

		VK_Print( "***[CIMMI]---> LIST TITLE:\n");
		for(kk=0; kk<ausTextLen[0]; kk++)
			VK_Print( "%c", *(apucText[0] + kk));
		VK_Print( "\n");

		VK_Print( "***[CIMMI]---> LIST SUB-TITLE:\n");
		for(kk=0; kk<ausTextLen[1]; kk++)
			VK_Print( "%c", *(apucText[1] + kk));
		VK_Print( "\n");

		VK_Print( "***[CIMMI]---> LIST BOTTOM:\n");
		for(kk=0; kk<ausTextLen[2]; kk++)
			VK_Print( "%c", *(apucText[2] + kk));
		VK_Print( "\n");

		VK_Print( "***[CIMMI]---> LIST ITEM:\n");
		for(ll=0; ll<ucNumOfItem; ll++)
		{
			for(kk=0; kk<ausTextLen[ll+3]; kk++)
				VK_Print( "%c", *(apucText[ll+3] + kk));
			VK_Print( "\n");
		}

		VK_Print( "########################################\n");
	}
	#endif

	CA_MEM_Free(apucText);
	CA_MEM_Free(ausTextLen);

	/******************************************
	 notify mmi
	 ******************************************/
	hMmi = CiMmi_GetInst(usSsNo);
	if(hMmi == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_ProcListLast()---> Error : can't find MMI Instance : SsNo[%d] !!!\n", usSsNo));
	    CA_MEM_Free(pvParam);
		return -1;
	}

	PrintDbg(("[CIMMI]---> CiMmi_ProcListLast()---> hMmi[%p] SsNo[%d]\n", hMmi, hMmi->usSsNo));

	Callback_Reset(g_hCbNotifyMmiReq);
    while ((pfnMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyMmiReq)) != NULL)
    {
    	(*pfnMmiReq)((MMI_HANDLE)hMmi, MMI_LIST, (void *)pvParam, ulParamSize);
    }

	return ERR_OK;
}



static int CiMmi_AddInst(unsigned short usSsNo)
{
	MMI_INSTANCE		*hMmi;
	CI_SS_HANDLE		hSs;

 	PrintDbg(("CiMmi_AddInst() called...\n"));

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_AddInst()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_AddInst()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIMMI]---> CiMmi_AddInst()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIMMI]---> CiMmi_AddInst()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	if (s_ucCiMmiCnt >= MAX_CI_MMI_SS)
	{
		PrintErr(("[CIMMI]---> CiMmi_AddInst()---> Error : session is full !!!\n"));
		return -1;
	}

	hMmi = CiMmi_GetNewInst();
	if (hMmi == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_AddInst()---> Error : instance is full !!!\n"));
		return -1;
	}

	hMmi->usSsNo = usSsNo;

	s_ucCiMmiCnt++;

	return ERR_OK;
}

static int CiMmi_DeleteInst(unsigned short usSsNo)
{
	MMI_INSTANCE		*hMmi;

 	PrintDbg(("CiMmi_DeleteInst() called...\n"));

	hMmi = CiMmi_GetInst(usSsNo);
	if (hMmi == NULL)
	{
		PrintErr(("[CIMMI]---> CiMmi_DeleteInst()---> Error : SsNo[%d] incorrect !!!\n", usSsNo));
		return -1;
	}

	hMmi->usSsNo = 0;

	s_ucCiMmiCnt--;

	return ERR_OK;
}


static MMI_INSTANCE * CiMmi_GetNewInst(void)
{
	int	i;

	for (i=0; i<MAX_CI_MMI_SS; i++)
	{
		if (s_aCiMmi[i].usSsNo == 0)
			return &s_aCiMmi[i];
	}

	return NULL;
}


static MMI_INSTANCE * CiMmi_GetInst(unsigned short usSsNo)
{
	int i;

	for(i=0; i<MAX_CI_MMI_SS; i++)
	{
		if(s_aCiMmi[i].usSsNo == usSsNo)
		{
			PrintDbg(("[CIMMI]---> CiMmi_GetInst()---> %d th instance found\n", i));
			return &s_aCiMmi[i];
		}
	}

	return NULL;
}


#if 1
MMI_INSTANCE * CI_MmiGetInstBySlotId(CI_SLOT_ID usSlotId) /* �� �Լ��� �׽�Ʈ �뵵�θ� ���ȴ�. �Ϲ����� �뵵�� ������� �� ��!! */
{
	int i;
	CI_SS_HANDLE hSs;

	for(i=0; i<MAX_CI_MMI_SS; i++)
	{
		if(s_aCiMmi[i].usSsNo)
		{
			hSs = CIRH_GetSs(s_aCiMmi[i].usSsNo);
			if (hSs == NULL)
				continue;
			if (hSs->hTc == NULL)
				continue;
			if (hSs->hTc->ucTcId == 0)
				continue;
			if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
				continue;
			if (hSs->hTc->usSlotId == usSlotId)
			{
				PrintDbg(("[CIMMI]---> CI_MmiGetInstBySlotId()---> %d th instance found\n", i));
				return &s_aCiMmi[i];
			}
		}
	}

	return NULL;
}
#endif
