/* $Header:   //Mars/source/pvcs/VM/BASE/archives/PDR_LIGHT/mw/cas/ci/ci_res/ci_rm.c-arc   1.4   Aug 21 2004 08:31:52   wskim  $ */
/***************************************************************
* $Workfile:   ci_rm.c  $
* $Modtime:   Aug 21 2004 07:31:22  $
* Auther : Jaehee Cho / hmkim (CI+)
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

#include "cipack_msg.h"

#include "ci.h"
#include "ci_reshandler.h"
#include "pal_ci.h"

/***************************************************************
* definition
***************************************************************/



/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE	g_hCbNotifyCamState;


/***************************************************************
* static variables and structures
***************************************************************/


/***************************************************************
* static function prototypes
***************************************************************/
static int CiRm_SendProfileReply(unsigned short usSsNo);
static int CiRm_SendProfileChanged(unsigned short usSsNo);
static int CiRm_SendProfileEnq(unsigned short usSsNo);


/***************************************************************
* function bodies
***************************************************************/
int CiRm_Init(void)
{
	return ERR_OK;
}

int CiRm_SsOpenedProc(unsigned short usSsNo)
{
	unsigned char ucMsg[4];
	CI_NOTIFY_CAM_STATE_FUNC	pfnNotifyCamState;
	CI_SS_HANDLE	hSs;

	ucMsg[0] = (unsigned char)((CI_PROFILE_ENQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_PROFILE_ENQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_PROFILE_ENQ & 0xff);
	ucMsg[3] = 0x00;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		PrintErr(("[CIRM]---> CiRm_SsOpenedProc()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	/* get notify function list and notify CAM state */
	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIRM]---> CiRm_SsOpenedProc()---> Error : CIRH_GetSs() SsNo[%d] - NULL hSs\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIRM]---> CiRm_SsOpenedProc()---> Error : CIRH_GetSs() SsNo[%d] - NULL hTc\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIRM]---> CiRm_SsOpenedProc()---> Error : CIRH_GetSs() SsNo[%d] - Invalid TcId\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIRM]---> CiRm_SsOpenedProc()---> Error : CIRH_GetSs() SsNo[%d] - Invalid SlotId\n", usSsNo));
		return -1;
	}

	Callback_Reset(g_hCbNotifyCamState);
    while ((pfnNotifyCamState = (CI_NOTIFY_CAM_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamState)) != NULL)
    {
    	(*pfnNotifyCamState)(hSs->hTc->usSlotId, CI_CAM_INITIALIZING);
    }

	PrintDbg(("[CIRM]---> CiRm_SsOpenedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiRm_SsClosedProc(unsigned short usSsNo)
{
	PrintDbg(("[CIRM]---> CiRm_SsClosedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiRm_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_PROFILE_ENQ:
		PrintNotify(("CI_PROFILE_ENQ received\n"));
		nRtnCode = CiRm_SendProfileReply(usSsNo);
		break;

	case CI_PROFILE_REPLY:
		PrintNotify(("CI_PROFILE_REPLY received (len=%ld)\n", ulApduLen));
		#ifdef _CI_DEBUG_
		if ((pucApduData != NULL) && (ulApduLen != 0))
		{
			int i;
			PrintDbg(( "\t===[CIRM]:CI_PROFILE_REPLY[%d]===\n\t", usSsNo));
			for (i=0; i<ulApduLen; i++)
			{
				if (i%4)
					PrintDbg((" 0x"));
				PrintDbg(("%x", pucApduData[i]));
			}
			PrintDbg(( "\n\t=========================\n"));
		}
		else
		{
			PrintDbg(("[CIRM]---> CiRm_ApduProc()---> SsNo[%d], CAM have no Resource...\n", usSsNo));
		}
		#endif
		nRtnCode = CiRm_SendProfileChanged(usSsNo);
		break;

	case CI_PROFILE_CHANGED:
		PrintNotify(("CI_PROFILE_CHANGED received\n"));
		nRtnCode = CiRm_SendProfileEnq(usSsNo);
		break;

	default:
		nRtnCode = -1;
		PrintErr(("[CIRM]---> CiRm_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	PrintDbg(("[CIRM]---> CiRm_ApduProc()---> SsNo[%d], tag[0x%lx]...\n", usSsNo, ulApduTag));

	return nRtnCode;
}

static int CiRm_SendProfileReply(unsigned short usSsNo)
{
	CI_RES_INST *pResInst;
	unsigned char ucMsg[MAX_CI_RES_INST*4+6];
	unsigned short usLen;
	int i;

#ifdef CI_PLUS_ENABLE
// CI+ ��Ʈ���� CI+ credentials �� ��ġ�Ǿ� ���� ���� ��쿣 CC ���ҽ��� �����ϵ��� �Ѵ�.
// �÷ο� �� CIRH_Init()���� �� ó���� �ؾ� ������ ���꿡�� CI+ credentials �ٿ�ε���� Ÿ�̹����� ���� ������ ���⿡�� ó���ϵ��� �Ѵ� (��, CiCc_IsInitialized() ȣ���� ��ȿ������).
// �׷��� CI+ ��Ʈ���� CI+ credentials �� ��ġ�Ǿ� ���� ���� ���� ġ���� ���� �����̱� ������ ���� �̷� ó���� �� �ʿ䰡 ������?
// �ϴ� �̷� ���������� ��Ȳ���� SMiT CI+ ķ�� ��� ��� CI_REQUEST_CICAM_RESET �� ��û�ϹǷ� �ڵ� �����ϵ��� �Ѵ�...
{
	extern int CiCc_IsInitialized(void);
	CI_RES_HANDLE hRes = CIRH_GetRes();

	if (!CiCc_IsInitialized())
	{
		for (i = 0; i < MAX_CI_RES_INST; i++) // for (i = 0; i < s_ucCiResCnt; i++)
		{
			if (hRes[i].uResId.ulId == CI_CC_RES_V1_ID) // if (s_aCiRes[i].uResId.ulId == CI_CC_RES_V1_ID)
				break;
		}
		if (i < MAX_CI_RES_INST) // if (i < s_ucCiResCnt)
		{
			PrintErr(("[CIRM]---> CiRm_SendProfileReply()---> Error : No credentials in this set. CC resource will be removed !!!\n")); //PrintErr(("[CIRH]---> CIRH_Init()---> Error : No credentials in this set. CC resource will be removed !!!\n"));
			CIRH_DeleteRes(&hRes[i]); // CIRH_DeleteRes(&s_aCiRes[i]);
		}
		else
		{
			 // �̹� ���ŵ� ����... nothing to do
		}
	}
}
#endif

	ucMsg[0] = (unsigned char)((CI_PROFILE_REPLY >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_PROFILE_REPLY >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_PROFILE_REPLY & 0xff);

	pResInst = CIRH_GetRes();
	usLen = 4;
	for (i=0; i<MAX_CI_RES_INST; i++)
	{
		if (pResInst[i].ucMaxSs > 0)
		{
			ucMsg[usLen++] = (unsigned char)((pResInst[i].uResId.ulId >> 24) & 0xff);
			ucMsg[usLen++] = (unsigned char)((pResInst[i].uResId.ulId >> 16) & 0xff);
			ucMsg[usLen++] = (unsigned char)((pResInst[i].uResId.ulId >> 8) & 0xff);
			ucMsg[usLen++] = (unsigned char)(pResInst[i].uResId.ulId & 0xff);
		}
	}

	ucMsg[3] = (unsigned char)(usLen - 4);

	if (CIRH_SendApdu(usSsNo, ucMsg, usLen) != ERR_OK)
	{
		PrintErr(("[CIRM]---> CiRm_SendProfileReply()---> Error : send APDU msg. SsNo[%d]\n", usSsNo));
		return -1;
	}

	return ERR_OK;
}

static int CiRm_SendProfileChanged(unsigned short usSsNo)
{
	unsigned char ucMsg[4];

	ucMsg[0] = (unsigned char)((CI_PROFILE_CHANGED >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_PROFILE_CHANGED >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_PROFILE_CHANGED & 0xff);
	ucMsg[3] = 0x0;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		PrintErr(("[CIRM]---> CiRm_SendProfileChanged()---> Error : send APDU msg. SsNo[%d]\n", usSsNo));
		return -1;
	}

	return ERR_OK;
}

static int CiRm_SendProfileEnq(unsigned short usSsNo)
{
	unsigned char ucMsg[4];

	ucMsg[0] = (unsigned char)((CI_PROFILE_ENQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_PROFILE_ENQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_PROFILE_ENQ & 0xff);
	ucMsg[3] = 0x0;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		PrintErr(("[CIRM]---> CiRm_SendProfileEnq()---> Error : send APDU msg. SsNo[%d]\n", usSsNo));
		return -1;
	}

	return ERR_OK;
}

