/***************************************************************
* $Workfile:   ci_ai.c  $
* $Modtime:   13 Dec 2004 09:27:30  $
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

#include "ci.h"
#include "cipack_msg.h"
#include "ci_reshandler.h"
#include "ci_ai.h"
#include "ci_hwtest.h"
#include "pal_ci.h"
#ifdef CI_PLUS_ENABLE
#include "ci_plus.h" // for CI_IsCiPlusCam() and CI_GetDataRate()
#endif


/***************************************************************
* definition
***************************************************************/



/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE g_hCbNotifyAppInfo;
extern CALLBACK_HANDLE g_hCbNotifyCamState;


/***************************************************************
* static variables and structures
***************************************************************/
static CI_APP_INFO s_CiAppInfo[PAL_MAX_CISLOT_NUMBER];
static unsigned short s_usCiAiSsNo[PAL_MAX_CISLOT_NUMBER];


/***************************************************************
* static function prototypes
***************************************************************/
static int CiAi_AddAppInfo(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiAi_DeleteAppInfo(unsigned short usSsNo);
#ifdef CI_PLUS_ENABLE
static int CiAi_CicamReset(unsigned short usSsNo);
#endif


/***************************************************************
* function bodies
***************************************************************/
int CiAi_Init(void)
{
	VK_memset(s_CiAppInfo, 0, sizeof(CI_APP_INFO) * PAL_MAX_CISLOT_NUMBER);

	return ERR_OK;
}

int CiAi_SsOpenedProc(unsigned short usSsNo)
{
	unsigned char ucMsg[4];
	CI_SS_HANDLE hSs;

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_SsOpenedProc()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_SsOpenedProc()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIAI]---> CiAi_SsOpenedProc()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIAI]---> CiAi_SsOpenedProc()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	s_usCiAiSsNo[hSs->hTc->usSlotId] = usSsNo;

	ucMsg[0] = (unsigned char)((CI_APPL_INFO_ENQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_APPL_INFO_ENQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_APPL_INFO_ENQ & 0xff);
	ucMsg[3] = 0x00;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		PrintErr(("[CIAI]---> CiAi_SsOpenedProc()---> Error : send CI_APPL_INFO_ENQ. SsNo[%d]\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIAI]---> CiAi_SsOpenedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiAi_SsClosedProc(unsigned short usSsNo)
{
	CI_SS_HANDLE hSs;

	if (CiAi_DeleteAppInfo(usSsNo) < 0)
	{
		PrintErr(("[CIAI]---> CiAi_SsClosedProc()---> Error : delete app info. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_SsClosedProc()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_SsClosedProc()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIAI]---> CiAi_SsClosedProc()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIAI]---> CiAi_SsClosedProc()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	s_usCiAiSsNo[hSs->hTc->usSlotId] = 0;

#if 0
	if (PAL_CI_CheckModule(hSs->hTc->usSlotId) == PAL_CI_INSERTED)
	{
		PrintDbg(("[CIAI]---> CiAi_SsClosedProc()---> PAL_CI_ResetModule(%d)...\n", hSs->hTc->usSlotId));
		if (PAL_CI_ResetModule(hSs->hTc->usSlotId) != ERR_OK)
		{
			PrintErr(("[CIAI]---> CiAi_SsClosedProc()---> Error : fail to reset slot %d !!!\n", hSs->hTc->usSlotId));
			return -1;
		}
	}
#endif

	PrintDbg(("[CIAI]---> CiAi_SsClosedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiAi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_APPL_INFO_REPLY:
		PrintNotify(("CI_APPL_INFO_REPLY received\n"));
		#ifdef CI_WORKAROUND_FOR_DOUGLAS
		{
			extern void CiAdapt_DouglasWorkAroundTimerCallbackCancel(void);
			CiAdapt_DouglasWorkAroundTimerCallbackCancel();
		}
		#endif
		nRtnCode = CiAi_AddAppInfo(usSsNo, pucApduData, ulApduLen);
		break;

#ifdef CI_PLUS_ENABLE
	case CI_REQUEST_CICAM_RESET:
		PrintNotify(("CI_REQUEST_CICAM_RESET received\n"));
		nRtnCode = CiAi_CicamReset(usSsNo);
		break;
#endif

	default:
		nRtnCode = -1;
		PrintErr(("[CIAI]---> CiAi_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	PrintDbg(("[CIAI]---> CiAi_ApduProc()---> SsNo[%d], tag[0x%lx]...\n", usSsNo, ulApduTag));

	return nRtnCode;
}

int CiAi_EnterMenu(unsigned short usSsNo)
{
	unsigned char ucMsg[4];

	ucMsg[0] = (unsigned char)((CI_ENTER_MENU >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_ENTER_MENU >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_ENTER_MENU & 0xff);
	ucMsg[3] = 0x00;

	if (CIRH_SendApdu(usSsNo, ucMsg, 4) != ERR_OK)
	{
		PrintErr(("[CIAI]---> CiAi_EnterMenu()---> Error : send CI_ENTER_MENU. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIAI]---> CiAi_EnterMenu() SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

CI_APP_INFO * CiAi_GetAppInfo(CI_SLOT_ID usSlotId)
{
	if (usSlotId >= PAL_CI_GetNumberOfModule())
		return NULL;

	return &s_CiAppInfo[usSlotId];
}

unsigned short CiAi_GetSsNo(CI_SLOT_ID usSlotId)
{
	if (usSlotId >= PAL_CI_GetNumberOfModule())
		return 0;

	return s_usCiAiSsNo[usSlotId];
}

static int CiAi_AddAppInfo(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_SS_HANDLE hSs;
	unsigned char ucStrLen;
	CI_NOTIFY_APP_INFO_FUNC		pfnNotifyAppInfo;
	CI_NOTIFY_CAM_STATE_FUNC	pfnNotifyCamState;

	if (pucApduData == NULL || ulApduLen < 6)
	{
		PrintErr(("[CIAI]---> CiAi_AddAppInfo()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_AddAppInfo()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_AddAppInfo()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIAI]---> CiAi_AddAppInfo()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIAI]---> CiAi_AddAppInfo()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	s_CiAppInfo[hSs->hTc->usSlotId].nType = pucApduData[0];
	s_CiAppInfo[hSs->hTc->usSlotId].usManufacturer = ((unsigned short)pucApduData[1] << 8) & 0xff00;
	s_CiAppInfo[hSs->hTc->usSlotId].usManufacturer |= ((unsigned short)pucApduData[2] & 0x00ff);
	s_CiAppInfo[hSs->hTc->usSlotId].usManufacturerCode = ((unsigned short)pucApduData[3] << 8) & 0xff00;
	s_CiAppInfo[hSs->hTc->usSlotId].usManufacturerCode |= ((unsigned short)pucApduData[4] & 0x00ff);
	s_CiAppInfo[hSs->hTc->usSlotId].ucIsInitialized = 1;
	ucStrLen = pucApduData[5];
	if (ucStrLen > MAX_CI_STR_LEN)
	{
		PrintErr(("[CIAI]---> CiAi_AddAppInfo()---> Error : ucStrLen %d > %d\n", ucStrLen, MAX_CI_STR_LEN));
		ucStrLen = MAX_CI_STR_LEN;
	}
	VK_memset(s_CiAppInfo[hSs->hTc->usSlotId].szText, 0, MAX_CI_STR_LEN+1);
	VK_memcpy(s_CiAppInfo[hSs->hTc->usSlotId].szText, &pucApduData[6], ucStrLen);

#ifdef _CI_DEBUG_
{
	int	i;
	VK_Print("\t===CI_APP_INFO[%d]===\n", hSs->hTc->usSlotId);
	VK_Print("\tType[%d], Manu[0x%x, 0x%x]\n",
		s_CiAppInfo[hSs->hTc->usSlotId].nType,
		s_CiAppInfo[hSs->hTc->usSlotId].usManufacturer,
		s_CiAppInfo[hSs->hTc->usSlotId].usManufacturerCode);
	VK_Print("\t");
	for (i=0; i<MAX_CI_STR_LEN; i++)
	{
		if (s_CiAppInfo[hSs->hTc->usSlotId].szText[i] == 0x00)
			break;

		VK_Print("%c", s_CiAppInfo[hSs->hTc->usSlotId].szText[i]);
	}
	VK_Print("\n\t===============\n");
}
#endif

	/******************************************
	 notify app info is changed
	 ******************************************/

	Callback_Reset(g_hCbNotifyAppInfo);
    while ((pfnNotifyAppInfo = (CI_NOTIFY_APP_INFO_FUNC)Callback_GetNextFunc(g_hCbNotifyAppInfo)) != NULL)
    {
    	(*pfnNotifyAppInfo)(hSs->hTc->usSlotId, &s_CiAppInfo[hSs->hTc->usSlotId]);
    }

    /******************************************
	 notify CAM state
	 ******************************************/

	Callback_Reset(g_hCbNotifyCamState);
    while ((pfnNotifyCamState = (CI_NOTIFY_CAM_STATE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamState)) != NULL)
    {
    	(*pfnNotifyCamState)(hSs->hTc->usSlotId, CI_CAM_INITIALIZED);
    }

	/* �Ʒ� �ڵ�� CI_AI_RES_V3_ID�� �����ϴ� ķ�� ���ؼ��� ���Ǿ�� �Ѵ�. but...
	   DT ���ҽ��� �����ϴ� ��� ����ķ�� ��� data rate�� ķ�� ������ RM -> AI -> CAS ���ķ� DT ������ ������ �ϴµ� ������ �ʴ´�.
	   ���������� MMI ���ǵ� ������ �ʴ´�.
	   ������ �̳��� ķ�� AI ���ҽ� ������ ȣ��Ʈ�� ������ �״�� �÷��شٴ� ���̴�.
	   ���� AI �� ������ �����ϴ� ķ���� ķ�� �ִ� ������ �Ǵ��� �� ���� ����.
	   ķ���� ���Ǹ� ���ϴ� �κ��̴�. (ķ�� ���� ���� �ڵ带 �����ϵ簡...)
	   �̿� ���� ������ �ϴ� ���Ƶ�.
	   -> SMiT CI+ ķ�� ��� ȣ��Ʈ�� CI_APPL_INFO_REPLY �� ������ ���Ŀ� �� ������ ��ٸ��� ���� �ణ�� ������ ������ ���δ�!
	   -> CI+ ���� 1.2 "CI Plus Compatibility Identification"�� �̿��ϵ��� CI_IsCiPlusCam() �Լ��� �����ϸ� �Ʒ� �ڵ带 �̿��� �� ���� ���̴�!
	   -> ���� �Ϸ��ϰ� SMiT & Neotion CI+ ķ�� ���� ���� Ȯ�� �Ϸ���.
	   -> but... ȣ��Ʈ �ʿ��� 96 Mbps �� ������ ���� ����ϵ��� ����. �������� data rate info �� ������ �ǹ̰� �����Ƿ� �ϴ� ���Ƶ�. */

#if 0 // #ifdef CI_PLUS_ENABLE
    /******************************************
	 send data rate info
	 ******************************************/

	/* CI+ ���� 1.2 : If data_rate_info APDU is not sent by the host then the maximum data rate supported by the host is 72Mbit/s. */
	if (CI_IsCiPlusCam(hSs->hTc->usSlotId) /* #define USE_CIS_INFORMATION Ȱ���� ���� ��ȿ�Կ� ������ �� */
		&& CIRH_GetSsFromRes(hSs->hTc->usSlotId, CI_AI_RES_V3_ID) != NULL)
	{
		unsigned char ucMsg[5];

		ucMsg[0] = (unsigned char)((CI_DATA_RATE_INFO >> 16) & 0xff);
		ucMsg[1] = (unsigned char)((CI_DATA_RATE_INFO >> 8) & 0xff);
		ucMsg[2] = (unsigned char)(CI_DATA_RATE_INFO & 0xff);
		ucMsg[3] = 0x01;
		ucMsg[4] = CI_GetDataRate();

		if (CIRH_SendApdu(usSsNo, ucMsg, 5) < 0)
		{
			PrintErr(("[CIAI]---> CiAi_AddAppInfo()---> Error : send CI_DATA_RATE_INFO. SsNo[%d] !!!\n", usSsNo));
			return -1;
		}

		PrintNotify(("CI_APPL_INFO_REPLY received -> CI+ with AIv3 -> send CI_DATA_RATE_INFO OK\n"));
	}
#endif

	CI_HWTEST_AddAppInfo();
	PrintDbg(("[CIAI]---> CiAi_AddAppInfo()---> success...\n"));

	return ERR_OK;
}

static int CiAi_DeleteAppInfo(unsigned short usSsNo)
{
	CI_SS_HANDLE hSs;

	CI_HWTEST_DeleteAppInfo();
	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_DeleteAppInfo()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_DeleteAppInfo()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIAI]---> CiAi_DeleteAppInfo()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIAI]---> CiAi_DeleteAppInfo()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	VK_memset(&s_CiAppInfo[hSs->hTc->usSlotId], 0, sizeof(CI_APP_INFO));

	/******************************************
	 notify app info is changed
	 ******************************************/

	PrintDbg(("[CIAI]---> CiAi_DeleteAppInfo()---> success...\n"));

	return ERR_OK;
}

#ifdef CI_PLUS_ENABLE
static int CiAi_CicamReset(unsigned short usSsNo)
{
	CI_SS_HANDLE hSs;

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_CicamReset()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIAI]---> CiAi_CicamReset()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIAI]---> CiAi_CicamReset()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIAI]---> CiAi_CicamReset()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}
	if (PAL_CI_ResetModule(hSs->hTc->usSlotId) != ERR_OK)
	{
		PrintErr(("[CIAI]---> CiAi_CicamReset()---> Error : PAL_CI_ResetModule(%d) !!!\n", hSs->hTc->usSlotId));
		return -1;
	}

	PrintDbg(("[CIAI]---> CiAi_CicamReset()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}
#endif

