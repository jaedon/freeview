/***************************************************************
* $Workfile:   ci_cu.c  $
* $Modtime:
* Auther : hmkim
*
* Desc : CAM Firmware Upgrade (CI+)
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include "ci_cu.h"
#include <cb_list.h>


/***************************************************************
* definition
***************************************************************/



/***************************************************************
* static / extern variables and structures
***************************************************************/
static CI_CU_INSTANCE	s_aCiCu[MAX_CI_CU_SS];
static unsigned char	s_ucCiCuCnt;

extern CALLBACK_HANDLE	g_hCbNotifyCamUpgrade;
static CI_NOTIFY_CAM_UPGRADE_FUNC s_pfnNotifyCamUpgrade;
static CIPLUS_CamUpgradeInfo_t s_stCamUpgradeInfo;


/***************************************************************
* static function prototypes
***************************************************************/
static int CiCu_UpgradeReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCu_UpgradeProgress(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCu_UpgradeComplete(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);

static void CiCu_TimerCallback(unsigned long ulTimerId, void *pvParam);

static int CiCu_AddInst(unsigned short usSsNo);
static int CiCu_DeleteInst(unsigned short usSsNo);
static CI_CU_INSTANCE * CiCu_GetNewInst(void);
static CI_CU_INSTANCE * CiCu_GetInst(unsigned short usSsNo);


/***************************************************************
* function bodies
***************************************************************/
int CiCu_Init(void)
{
	VK_memset(s_aCiCu, 0, sizeof(CI_CU_INSTANCE) * MAX_CI_CU_SS);

	s_ucCiCuCnt = 0;

	return ERR_OK;
}

int CiCu_SsOpenedProc(unsigned short usSsNo)
{
	if (CiCu_AddInst(usSsNo) < 0)
	{
		return -1;
	}

	return ERR_OK;
}

int CiCu_SsClosedProc(unsigned short usSsNo)
{
	if (CiCu_DeleteInst(usSsNo) < 0)
	{
		return -1;
	}

	return ERR_OK;
}

int CiCu_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_CAM_UPGRADE:
//		PrintNotify(("CI_CAM_UPGRADE received\n"));
		nRtnCode = CiCu_UpgradeReply(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CAM_UPGRADE_PROGRESS:
//		PrintNotify(("CI_CAM_UPGRADE_PROGRESS received\n"));
		nRtnCode = CiCu_UpgradeProgress(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CAM_UPGRADE_COMPLETE:
//		PrintNotify(("CI_CAM_UPGRADE_COMPLETE received\n"));
		nRtnCode = CiCu_UpgradeComplete(usSsNo, pucApduData, ulApduLen);
		break;

	default:
		nRtnCode = -1;
		PrintErr(("[CICU]---> CiCu_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	PrintDbg(("[CICU]---> CiCu_ApduProc() SsNo[%d], tag[0x%lx]...\n", usSsNo, ulApduTag));

	return nRtnCode;
}

static int CiCu_UpgradeReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_CU_INSTANCE	*hCu;

	CI_HOST_MODE	enHostMode; 		/* CI_HOST_MODE */
	unsigned char	ucUpgradeType;		/* CI_CU_UPGRADE_TYPE */
	unsigned short	usDownloadTime; 	/* download duration in sec : see CI_CU_UNKNOWN_DOWNLOAD_DURATION */
	unsigned char	ucAnswer;			/* CI_CU_UPGRADE_ANSWER */

	unsigned char ucMsg[5];

	hCu = CiCu_GetInst(usSsNo);
	if (hCu == NULL)
	{
		PrintErr(("[CICU]---> CiCu_UpgradeReply()---> Error : CiCu_GetInst() SsNo[%d] - NULL hCu\n", usSsNo));
		return -1;
	}

	if (pucApduData == NULL || ulApduLen < 3)
	{
		PrintErr(("[CICU]---> CiCu_UpgradeReply()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	enHostMode = CI_GetHostMode();

	ucUpgradeType = pucApduData[0];
	usDownloadTime = ((unsigned short)pucApduData[1] << 8) & 0xff00;
	usDownloadTime |= ((unsigned short)pucApduData[2] & 0x00ff);

	switch (enHostMode)
	{
		case CI_HOST_MODE_NORMAL:
			ucAnswer = CI_CU_UPGRADE_ANSWER_ASK;
			break;
		case CI_HOST_MODE_UNATTENDED:
			if (ucUpgradeType == CI_CU_UPGRADE_TYPE_DELAYED)
				ucAnswer = CI_CU_UPGRADE_ANSWER_NO;
			else
				ucAnswer = CI_CU_UPGRADE_ANSWER_YES;
			break;
		case CI_HOST_MODE_SERVICE:
			ucAnswer = CI_CU_UPGRADE_ANSWER_YES;
			break;
		default:
			PrintErr(("[CICU]---> CiCu_UpgradeReply()---> Error : Unknown Host Mode[0x%x] !!!\n", enHostMode));
			return -1;
	}

	ucMsg[0] = (unsigned char)((CI_CAM_UPGRADE_REPLY >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CAM_UPGRADE_REPLY >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CAM_UPGRADE_REPLY & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = ucAnswer;

	PrintNotify(("[CICU]---> CiCu_UpgradeReply()---> host mode = 0x%x, upgrade type = 0x%x, duration time = 0x%x (%d sec), answer = 0x%x\n",
		enHostMode, ucUpgradeType, usDownloadTime, usDownloadTime, ucAnswer));

	if (CIRH_SendApdu(usSsNo, ucMsg, 5) != ERR_OK)
	{
		PrintErr(("[CICU]---> CiCu_UpgradeReply()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	if (ucAnswer == CI_CU_UPGRADE_ANSWER_YES)
	{
		VK_TIMER_EventAfter(CI_CU_TIMEOUT, CiCu_TimerCallback, (void *)hCu, sizeof(CI_CU_INSTANCE), (unsigned long *)&hCu->ulTimerID);
	}

	return ERR_OK;
}

static int CiCu_UpgradeProgress(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_CU_INSTANCE	*hCu;
	unsigned char ucDownloadProgressInPercentage; /* 진행률(%) */

	hCu = CiCu_GetInst(usSsNo);
	if (hCu == NULL)
	{
		PrintErr(("[CICU]---> CiCu_UpgradeProgress()---> Error : CiCu_GetInst() SsNo[%d] - NULL hCu\n", usSsNo));
		return -1;
	}

	if (pucApduData == NULL || ulApduLen < 1)
	{
		PrintErr(("[CICU]---> CiCu_UpgradeProgress()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	if (hCu->ulTimerID)
	{
		VK_TIMER_Cancel(hCu->ulTimerID);
		hCu->ulTimerID = 0;
	}

	ucDownloadProgressInPercentage = pucApduData[0];

	PrintNotify(("[CICU]---> CiCu_UpgradeProgress()---> %d (%%)\n", ucDownloadProgressInPercentage));

	/* CAM firmware upgrade 진행 상태 처리 (진행률을 UI로 표시하고 user interaction 을 막을 것) */
	s_stCamUpgradeInfo.eCamUpgradeMode = eCiPlus_CamUpgrade_Processing;
	s_stCamUpgradeInfo.ucDownloadProgressInPercentage = ucDownloadProgressInPercentage;
	s_stCamUpgradeInfo.eCamUpgradeResetMode = eCiPlus_CamUpgradeReset_NO_RESET;
	Callback_Reset(g_hCbNotifyCamUpgrade);
	while ((s_pfnNotifyCamUpgrade = (CI_NOTIFY_CAM_UPGRADE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamUpgrade)) != NULL)
	{
		if ((*s_pfnNotifyCamUpgrade)(hCu->usSlotId, &s_stCamUpgradeInfo) != ERR_OK)
		{
			PrintErr(("[CICU]---> CiCu_UpgradeProgress()---> Error : callback failed !!!\n"));
		}
	}

	VK_TIMER_EventAfter(CI_CU_TIMEOUT, CiCu_TimerCallback, (void *)hCu, sizeof(CI_CU_INSTANCE), (unsigned long *)&hCu->ulTimerID);

	return ERR_OK;
}

static int CiCu_UpgradeComplete(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_CU_INSTANCE	*hCu;
	unsigned char ucResetRequestStatus; /* CI_CU_RESET_REQUEST */

	hCu = CiCu_GetInst(usSsNo);
	if (hCu == NULL)
	{
		PrintErr(("[CICU]---> CiCu_UpgradeComplete()---> Error : CiCu_GetInst() SsNo[%d] - NULL hCu\n", usSsNo));
		return -1;
	}

	if (pucApduData == NULL || ulApduLen < 1)
	{
		PrintErr(("[CICU]---> CiCu_UpgradeComplete()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	if (hCu->ulTimerID)
	{
		VK_TIMER_Cancel(hCu->ulTimerID);
		hCu->ulTimerID = 0;
	}

	ucResetRequestStatus = pucApduData[0];

	PrintNotify(("[CICU]---> CiCu_UpgradeComplete()---> 0x%x\n", ucResetRequestStatus));

	/* CAM firmware upgrade 진행 완료 처리 (UI로 표시하고 막혔던 user interaction 을 풀고 상황에 따라 캠 리셋할 것) :
	   업그레이드 완료되었을 때 뿐만이 아니라 캠에서 업그레이드를 취소할 때도 불릴 수 있음에 유의할 것!
       이 경우 ucResetRequestStatus는 CI_CU_RESET_REQUEST_NO_RESET로 온다.
       아래 리셋 코드는 callback 을 통해 처리될 수 있도록 변경하자... */
	s_stCamUpgradeInfo.eCamUpgradeMode = eCiPlus_CamUpgrade_DoneOrCancelled;
	s_stCamUpgradeInfo.ucDownloadProgressInPercentage = 100;
	s_stCamUpgradeInfo.eCamUpgradeResetMode = ucResetRequestStatus;
	Callback_Reset(g_hCbNotifyCamUpgrade);
	while ((s_pfnNotifyCamUpgrade = (CI_NOTIFY_CAM_UPGRADE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamUpgrade)) != NULL)
	{
		if ((*s_pfnNotifyCamUpgrade)(hCu->usSlotId, &s_stCamUpgradeInfo) != ERR_OK)
		{
			PrintErr(("[CICU]---> CiCu_UpgradeComplete()---> Error : callback failed !!!\n"));
		}
	}

	#if 0
	switch (ucResetRequestStatus)
	{
		/* 일단 아래 두 경우 모두 PAL_CI_ResetModule()을 호출하도록 한다... */
		case CI_CU_RESET_REQUEST_PCMCIA:
		case CI_CU_RESET_REQUEST_CAM:
			if (PAL_CI_ResetModule(hCu->usSlotId) != ERR_OK)
				return -1;
			break;

		case CI_CU_RESET_REQUEST_NO_RESET:
			break;

		default:
			PrintErr(("[CICU]---> CiCu_UpgradeComplete()---> Error : Unknown Reset Request[0x%x] !!!\n", ucResetRequestStatus));
			return -1;
	}
	#endif

	return ERR_OK;
}

static void CiCu_TimerCallback(unsigned long ulTimerId, void *pvParam)
{
	CI_CU_INSTANCE	*hCu = (CI_CU_INSTANCE *)pvParam;

	PrintErr(("[CICU]---> CiCu_TimerCallback()---> Error : No response from CAM. CAM will be reset now !!!\n"));

	if (hCu == NULL)
	{
		PrintErr(("[CICU]---> CiCu_TimerCallback()---> Error : hCu is NULL !!!\n"));
		return;
	}

	hCu->ulTimerID = 0;

	/* CAM firmware upgrade 진행 실패 처리 (UI로 표시하고 캠 리셋) : 아래 리셋 코드는 callback 을 통해 처리될 수 있도록 변경하자... */
	s_stCamUpgradeInfo.eCamUpgradeMode = eCiPlus_CamUpgrade_Failed;
	s_stCamUpgradeInfo.ucDownloadProgressInPercentage = 100;
	s_stCamUpgradeInfo.eCamUpgradeResetMode = eCiPlus_CamUpgradeReset_CAM;
	Callback_Reset(g_hCbNotifyCamUpgrade);
	while ((s_pfnNotifyCamUpgrade = (CI_NOTIFY_CAM_UPGRADE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamUpgrade)) != NULL)
	{
		if ((*s_pfnNotifyCamUpgrade)(hCu->usSlotId, &s_stCamUpgradeInfo) != ERR_OK)
		{
			PrintErr(("[CICU]---> CiCu_TimerCallback()---> Error : callback failed !!!\n"));
		}
	}

	#if 0
	if (PAL_CI_ResetModule(hCu->usSlotId) != ERR_OK)
	{
		PrintErr(("[CICU]---> CiCu_TimerCallback()---> Error : SsNo[%d] - reset failed !!!\n", hCu->usSsNo));
		return;
	}
	#endif

	return;
}

static int CiCu_AddInst(unsigned short usSsNo)
{
	CI_CU_INSTANCE	*hCu;
	CI_SS_HANDLE	hSs;

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CICU]---> CiCu_AddInst()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CICU]---> CiCu_AddInst()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CICU]---> CiCu_AddInst()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CICU]---> CiCu_AddInst()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	if (s_ucCiCuCnt >= MAX_CI_CU_SS)
	{
		PrintErr(("[CICU]---> CiCu_AddInst()---> Error : session is full !!!\n"));
		return -1;
	}

	hCu = CiCu_GetNewInst();
	if (hCu == NULL)
	{
		PrintErr(("[CICU]---> CiCu_AddInst()---> Error : instance is full !!!\n"));
		return -1;
	}

	hCu->usSsNo = usSsNo;
	hCu->usSlotId = hSs->hTc->usSlotId;

	s_ucCiCuCnt++;

	return ERR_OK;
}

static int CiCu_DeleteInst(unsigned short usSsNo)
{
	CI_CU_INSTANCE	*hCu;

	hCu = CiCu_GetInst(usSsNo);
	if (hCu == NULL)
	{
		PrintErr(("[CICU]---> CiCu_DeleteInst()---> Error : SsNo[%d] incorrect !!!\n", usSsNo));
		return -1;
	}

	/* CAM firmware upgrade 진행 취소 처리 (유저에 의해 캠이 제거된 상황에 대한 처리) */
	if (s_stCamUpgradeInfo.eCamUpgradeMode == eCiPlus_CamUpgrade_Processing)
	{
		if (hCu->ulTimerID)
		{
			VK_TIMER_Cancel(hCu->ulTimerID);
			hCu->ulTimerID = 0;
		}

		PrintNotify(("[CICU]---> CiCu_DeleteInst()---> CAM F/W upgrade is broken !!!\n"));

		s_stCamUpgradeInfo.eCamUpgradeMode = eCiPlus_CamUpgrade_Broken;
		s_stCamUpgradeInfo.ucDownloadProgressInPercentage = 100;
		s_stCamUpgradeInfo.eCamUpgradeResetMode = eCiPlus_CamUpgradeReset_NO_RESET;
		Callback_Reset(g_hCbNotifyCamUpgrade);
		while ((s_pfnNotifyCamUpgrade = (CI_NOTIFY_CAM_UPGRADE_FUNC)Callback_GetNextFunc(g_hCbNotifyCamUpgrade)) != NULL)
		{
			if ((*s_pfnNotifyCamUpgrade)(hCu->usSlotId, &s_stCamUpgradeInfo) != ERR_OK)
			{
				PrintErr(("[CICU]---> CiCu_DeleteInst()---> Error : callback failed !!!\n"));
			}
		}
	}

	hCu->usSsNo = 0;
	hCu->usSlotId = 0; // INVALID_CI_SLOT_ID

	s_ucCiCuCnt--;

	return ERR_OK;
}

static CI_CU_INSTANCE * CiCu_GetNewInst(void)
{
	int	i;

	for (i=0; i<MAX_CI_CU_SS; i++)
	{
		if (s_aCiCu[i].usSsNo == 0)
			return &s_aCiCu[i];
	}

	return NULL;
}

static CI_CU_INSTANCE * CiCu_GetInst(unsigned short usSsNo)
{
	int i;

	for (i=0; i<MAX_CI_CU_SS; i++)
	{
		if(s_aCiCu[i].usSsNo == usSsNo)
		{
			PrintDbg(("[CICU]---> CiCu_GetInst()---> %d th instance found\n", i));
			return &s_aCiCu[i];
		}
	}

	return NULL;
}

