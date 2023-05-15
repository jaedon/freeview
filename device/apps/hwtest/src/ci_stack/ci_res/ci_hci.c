/***************************************************************
* $Workfile:   ci_hci.c  $
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
#include "ci_hci.h"
#include <cb_list.h>


/***************************************************************
* definition
***************************************************************/



/***************************************************************
* static / extern variables and structures
***************************************************************/
static CI_HCI_REPLACE	stCiHciReplace[MAX_CI_HCI_REPLACE];
static unsigned long	ulReplacementCount = 0;

extern CALLBACK_HANDLE	g_hCbNotifyHostTune;
static CI_NOTIFY_HOST_TUNE_FUNC s_pfnNotifyHostTune;
static CIPLUS_TuneInfo_t s_stTuneInfo;

extern CALLBACK_HANDLE	g_hCbNotifyPidReplace;
static CI_NOTIFY_PID_REPLACE_FUNC s_pfnNotifyPidReplace;
static CIPLUS_PidInfo_t s_stPidInfo;


/***************************************************************
* static function prototypes
***************************************************************/
static int	CiHci_ProcTune(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int	CiHci_ProcReplace(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int	CiHci_ProcClearReplace(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);


/***************************************************************
* function bodies
***************************************************************/
int CiHci_Init(void)
{
	VK_memset(stCiHciReplace, 0x00, sizeof(CI_HCI_REPLACE) * MAX_CI_HCI_REPLACE);

	ulReplacementCount = 0;

	return ERR_OK;
}

int CiHci_SsOpenedProc(unsigned short usSsNo)
{
	PrintDbg(("[CIHCI]---> CiHci_SsOpenedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiHci_SsClosedProc(unsigned short usSsNo)
{
	CI_SS_HANDLE hSs;
	int	i;

	PrintDbg(("[CIHCI]---> CiHci_SsClosedProc()---> SsNo[%d]...\n", usSsNo));

	if (ulReplacementCount == 0)
	{
//		PrintNotify(("[CIHCI]---> CiHci_SsClosedProc()---> no replaced\n"));
		return ERR_OK;
	}

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIHCI]---> CiHci_SsClosedProc()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIHCI]---> CiHci_SsClosedProc()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIHCI]---> CiHci_SsClosedProc()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIHCI]---> CiHci_SsClosedProc()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	for (i = 0; i < MAX_CI_HCI_REPLACE; i++)
	{
		if (stCiHciReplace[i].ulReplacementCount == ulReplacementCount)
			break;
	}

	/* 세션이 닫히면 현재 demux에 걸려있는 component 중에 stCiHciReplace[i].usReplacementPid 인 것을 찾아 stCiHciReplace[i].usReplacedPid 로 다시 걸어라 */

	if (i < MAX_CI_HCI_REPLACE)
	{
		PrintNotify(("[CIHCI]---> CiHci_SsClosedProc()---> PID 0x%x returning to 0x%x ...\n", stCiHciReplace[i].usReplacementPid, stCiHciReplace[i].usReplacedPid));

		s_stPidInfo.eReplaceMode = eCiPlus_Replace_Stop;
		s_stPidInfo.usOriginalPid = stCiHciReplace[i].usReplacedPid;
		s_stPidInfo.usTargetPId = stCiHciReplace[i].usReplacementPid;
		Callback_Reset(g_hCbNotifyPidReplace);
		while ((s_pfnNotifyPidReplace = (CI_NOTIFY_PID_REPLACE_FUNC)Callback_GetNextFunc(g_hCbNotifyPidReplace)) != NULL)
		{
			if ((*s_pfnNotifyPidReplace)(hSs->hTc->usSlotId, &s_stPidInfo) != ERR_OK)
			{
				PrintErr(("[CIHCI]---> CiHci_SsClosedProc()---> Error : SsNo[%d] - PID returning failed !!!\n", usSsNo));
			}
		}
	}

	return ERR_OK;
}

int CiHci_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	PrintDbg(("[CIHCI]---> CiHci_ApduProc()---> SsNo[%d], tag[0x%lx]...\n", usSsNo, ulApduTag));

	switch (ulApduTag)
	{
	case CI_TUNE:
		nRtnCode = CiHci_ProcTune(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_REPLACE:
		nRtnCode = CiHci_ProcReplace(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CLEAR_REPLACE:
		nRtnCode = CiHci_ProcClearReplace(usSsNo, pucApduData, ulApduLen);
		break;

	default:
		nRtnCode = -1;
		PrintErr(("[CIHCI]---> CiHci_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	return nRtnCode;
}

static int	CiHci_ProcTune(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned short	usNetId;
	unsigned short	usOrgNetId;
	unsigned short	usTsId;
	unsigned short	usSvcId;

	CI_SS_HANDLE hSs;

	if (pucApduData == NULL || ulApduLen < 8)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcTune()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcTune()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcTune()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcTune()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIHCI]---> CiHci_ProcTune()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	usNetId = ((unsigned short)pucApduData[0] << 8) & 0xff00;
	usNetId |= ((unsigned short)pucApduData[1] & 0x00ff);
	usOrgNetId = ((unsigned short)pucApduData[2] << 8) & 0xff00;
	usOrgNetId |= ((unsigned short)pucApduData[3] & 0x00ff);
	usTsId = ((unsigned short)pucApduData[4] << 8) & 0xff00;
	usTsId |= ((unsigned short)pucApduData[5] & 0x00ff);
	usSvcId = ((unsigned short)pucApduData[6] << 8) & 0xff00;
	usSvcId |= ((unsigned short)pucApduData[7] & 0x00ff);

	/* find the service to be tuned from DB and change to that service */

	PrintNotify(("[CIHCI]---> CiHci_ProcTune()---> service tunning to NetId 0x%x, OrgNetId 0x%x, TsId 0x%x, SvcId 0x%x ...\n", usNetId, usOrgNetId, usTsId, usSvcId));

	s_stTuneInfo.eTuneMode = eCiPlus_Tune_Start;
	s_stTuneInfo.usNetId = usNetId;
	s_stTuneInfo.usOnId = usOrgNetId;
	s_stTuneInfo.usTsId = usTsId;
	s_stTuneInfo.usSvcId = usSvcId;
	Callback_Reset(g_hCbNotifyHostTune);
	while ((s_pfnNotifyHostTune = (CI_NOTIFY_HOST_TUNE_FUNC)Callback_GetNextFunc(g_hCbNotifyHostTune)) != NULL)
	{
		if ((*s_pfnNotifyHostTune)(hSs->hTc->usSlotId, &s_stTuneInfo) != ERR_OK)
		{
			PrintErr(("[CIHCI]---> CiHci_ProcTune()---> Error : SsNo[%d] - service tunning failed !!!\n", usSsNo));
		}
	}

	return ERR_OK;
}

static int	CiHci_ProcReplace(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char	ucReplacementRef;
	unsigned short	usReplacedPid;
	unsigned short	usReplacementPid;

	CI_SS_HANDLE hSs;
	int	i;

	if (pucApduData == NULL || ulApduLen < 5)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcReplace()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcReplace()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcReplace()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcReplace()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIHCI]---> CiHci_ProcReplace()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	ucReplacementRef = pucApduData[0];
	usReplacedPid = ((unsigned short)pucApduData[1] << 8) & 0x1f00;
	usReplacedPid |= ((unsigned short)pucApduData[2] & 0x00ff);
	usReplacementPid = ((unsigned short)pucApduData[3] << 8) & 0x1f00;
	usReplacementPid |= ((unsigned short)pucApduData[4] & 0x00ff);

	// 이전에 받은 replace object 와 같은 것이 있는지 비교
	for (i = 0; i < MAX_CI_HCI_REPLACE; i++)
	{
		if (stCiHciReplace[i].ucReplacementRef == ucReplacementRef)
			break;
	}

	// 이전에 받은 것이 없다면
	if (i == MAX_CI_HCI_REPLACE)
	{
		for (i = 0; i < MAX_CI_HCI_REPLACE; i++)
		{
			if (stCiHciReplace[i].ucReplacementRef == 0x00)
				break;
		}
		if (i == MAX_CI_HCI_REPLACE)
		{
			PrintErr(("[CIHCI]---> CiHci_ProcReplace()---> Error : MAX_CI_HCI_REPLACE(%d) is too small !!!\n", MAX_CI_HCI_REPLACE));
			return -1;
		}
	}

	stCiHciReplace[i].ucReplacementRef = ucReplacementRef;
	stCiHciReplace[i].usReplacedPid = usReplacedPid;
	stCiHciReplace[i].usReplacementPid = usReplacementPid;
	stCiHciReplace[i].ulReplacementCount = ++ulReplacementCount;

	/* 현재 demux에 걸려있는 component 중에 usReplacedPid 인 것을 찾아 usReplacementPid 로 다시 걸어라 */

	PrintNotify(("[CIHCI]---> CiHci_ProcReplace()---> PID 0x%x replacing to 0x%x ...\n", usReplacedPid, usReplacementPid));

	s_stPidInfo.eReplaceMode = eCiPlus_Replace_Start;
	s_stPidInfo.usOriginalPid = usReplacedPid;
	s_stPidInfo.usTargetPId = usReplacementPid;
	Callback_Reset(g_hCbNotifyPidReplace);
	while ((s_pfnNotifyPidReplace = (CI_NOTIFY_PID_REPLACE_FUNC)Callback_GetNextFunc(g_hCbNotifyPidReplace)) != NULL)
	{
		if ((*s_pfnNotifyPidReplace)(hSs->hTc->usSlotId, &s_stPidInfo) != ERR_OK)
		{
			PrintErr(("[CIHCI]---> CiHci_ProcReplace()---> Error : SsNo[%d] - PID replacing failed !!!\n", usSsNo));
		}
	}

	return ERR_OK;
}

static int	CiHci_ProcClearReplace(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char ucReplacementRef;
	CI_SS_HANDLE hSs;
	int	i;

	if (pucApduData == NULL || ulApduLen < 1)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcClearReplace()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcClearReplace()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcClearReplace()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcClearReplace()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIHCI]---> CiHci_ProcClearReplace()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	ucReplacementRef = pucApduData[0];

	// 이전에 받은 replace object 와 같은 것이 있는지 비교
	for (i = 0; i < MAX_CI_HCI_REPLACE; i++)
	{
		if (stCiHciReplace[i].ucReplacementRef == ucReplacementRef)
			break;
	}

	// 이전에 받은 것이 없다면 -> 뭘 어떡 하라구!
	if (i == MAX_CI_HCI_REPLACE)
	{
		PrintErr(("[CIHCI]---> CiHci_ProcClearReplace()---> Error : ucReplacementRef(0x%02X) not received !!!\n", ucReplacementRef));
		return -1;
	}

	/* 현재 demux에 걸려있는 component 중에 stCiHciReplace[i].usReplacementPid 인 것을 찾아 stCiHciReplace[i].usReplacedPid 로 다시 걸어라 */

	PrintNotify(("[CIHCI]---> CiHci_ProcClearReplace()---> PID 0x%x returning to 0x%x ...\n", stCiHciReplace[i].usReplacementPid, stCiHciReplace[i].usReplacedPid));

	s_stPidInfo.eReplaceMode = eCiPlus_Replace_Cancel;
	s_stPidInfo.usOriginalPid = stCiHciReplace[i].usReplacedPid;
	s_stPidInfo.usTargetPId = stCiHciReplace[i].usReplacementPid;
	Callback_Reset(g_hCbNotifyPidReplace);
	while ((s_pfnNotifyPidReplace = (CI_NOTIFY_PID_REPLACE_FUNC)Callback_GetNextFunc(g_hCbNotifyPidReplace)) != NULL)
	{
		if ((*s_pfnNotifyPidReplace)(hSs->hTc->usSlotId, &s_stPidInfo) != ERR_OK)
		{
			PrintErr(("[CIHCI]---> CiHci_ProcClearReplace()---> Error : SsNo[%d] - PID returning failed !!!\n", usSsNo));
		}
	}

	VK_memset(&stCiHciReplace[i], 0x00, sizeof(CI_HCI_REPLACE));

	return ERR_OK;
}
