/***************************************************************
* $Workfile:   ci_hlc.c  $
* $Modtime:
* Auther : hmkim
*
* Desc : Host Language & Country (CI+)
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include "ci_hlc.h"


/***************************************************************
* definition
***************************************************************/



/***************************************************************
* static variables and structures
***************************************************************/
static CI_HLC_INSTANCE	s_aCiHlc[MAX_CI_HLC_SS];
static unsigned char	s_ucCiHlcCnt;


/***************************************************************
* static function prototypes
***************************************************************/
static int CiHlc_SendCountryInfo(unsigned short usSsNo);
static int CiHlc_SendLanguageInfo(unsigned short usSsNo);

static int CiHlc_AddInst(unsigned short usSsNo);
static int CiHlc_DeleteInst(unsigned short usSsNo);
static CI_HLC_INSTANCE * CiHlc_GetNewInst(void);
static CI_HLC_INSTANCE * CiHlc_GetInst(unsigned short usSsNo);


/***************************************************************
* function bodies
***************************************************************/
int CiHlc_Init(void)
{
	VK_memset(s_aCiHlc, 0, sizeof(CI_HLC_INSTANCE) * MAX_CI_HLC_SS);

	s_ucCiHlcCnt = 0;

	return ERR_OK;
}

int CiHlc_SsOpenedProc(unsigned short usSsNo)
{
	int nRtnCode = ERR_OK;

	if (CiHlc_AddInst(usSsNo) < 0)
	{
		return -1;
	}

	nRtnCode |= CiHlc_SendCountryInfo(usSsNo);
	nRtnCode |= CiHlc_SendLanguageInfo(usSsNo);

	return nRtnCode;
}

int CiHlc_SsClosedProc(unsigned short usSsNo)
{
	if (CiHlc_DeleteInst(usSsNo) < 0)
	{
		return -1;
	}

	return ERR_OK;
}

int CiHlc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_HOST_COUNTRY_ENQ:
		PrintNotify(("CI_HOST_COUNTRY_ENQ received\n"));
		nRtnCode = CiHlc_SendCountryInfo(usSsNo);
		break;

	case CI_HOST_LANGUAGE_ENQ:
		PrintNotify(("CI_HOST_LANGUAGE_ENQ received\n"));
		nRtnCode = CiHlc_SendLanguageInfo(usSsNo);
		break;

	default:
		nRtnCode = -1;
		PrintErr(("[CIHLC]---> CiHlc_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	PrintDbg(("[CIHLC]---> CiHlc_ApduProc()---> SsNo[%d], tag[0x%lx]...\n", usSsNo, ulApduTag));

	return nRtnCode;
}

static int CiHlc_SendCountryInfo(unsigned short usSsNo) // 1) 세션이 오픈되었을 때, 2) 캠에서 요청할 때, 3) 메뉴에서 설정이 변경된 직후에 호출이 되어야 한다.
{
	unsigned char ucMsg[7];

	ucMsg[0] = (unsigned char)((CI_HOST_COUNTRY_REPLY >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_HOST_COUNTRY_REPLY >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_HOST_COUNTRY_REPLY & 0xff);
	ucMsg[3] = 0x03;

	CI_GetCountryCode(&ucMsg[4]);

	if (CIRH_SendApdu(usSsNo, ucMsg, 7) != ERR_OK)
	{
		PrintErr(("[CIHLC]---> CiHlc_SendCountryInfo()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIHLC]---> CiHlc_SendCountryInfo()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

static int CiHlc_SendLanguageInfo(unsigned short usSsNo) // 1) 세션이 오픈되었을 때, 2) 캠에서 요청할 때, 3) 메뉴에서 설정이 변경된 직후에 호출이 되어야 한다.
{
	unsigned char ucMsg[7];

	ucMsg[0] = (unsigned char)((CI_HOST_LANGUAGE_REPLY >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_HOST_LANGUAGE_REPLY >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_HOST_LANGUAGE_REPLY & 0xff);
	ucMsg[3] = 0x03;

	CI_GetLanguageCode(&ucMsg[4]);

	if (CIRH_SendApdu(usSsNo, ucMsg, 7) != ERR_OK)
	{
		PrintErr(("[CIHLC]---> CiHlc_SendLanguageInfo()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIHLC]---> CiHlc_SendLanguageInfo()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

static int CiHlc_AddInst(unsigned short usSsNo)
{
	CI_HLC_INSTANCE	*hHlc;
	CI_SS_HANDLE	hSs;

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CIHLC]---> CiHlc_AddInst()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CIHLC]---> CiHlc_AddInst()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CIHLC]---> CiHlc_AddInst()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CIHLC]---> CiHlc_AddInst()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	if (s_ucCiHlcCnt >= MAX_CI_HLC_SS)
	{
		PrintErr(("[CIHLC]---> CiHlc_AddInst()---> Error : session is full !!!\n"));
		return -1;
	}

	hHlc = CiHlc_GetNewInst();
	if (hHlc == NULL)
	{
		PrintErr(("[CIHLC]---> CiHlc_AddInst()---> Error : instance is full !!!\n"));
		return -1;
	}

	hHlc->usSsNo = usSsNo;
	hHlc->usSlotId = hSs->hTc->usSlotId;

	s_ucCiHlcCnt++;

	return ERR_OK;
}

static int CiHlc_DeleteInst(unsigned short usSsNo)
{
	CI_HLC_INSTANCE	*hHlc;

	hHlc = CiHlc_GetInst(usSsNo);
	if (hHlc == NULL)
	{
		PrintErr(("[CIHLC]---> CiHlc_DeleteInst()---> Error : SsNo[%d] incorrect !!!\n", usSsNo));
		return -1;
	}

	VK_memset(hHlc, 0, sizeof(CI_HLC_INSTANCE));

	s_ucCiHlcCnt--;

	return ERR_OK;
}

static CI_HLC_INSTANCE * CiHlc_GetNewInst(void)
{
	int	i;

	for (i = 0; i < MAX_CI_HLC_SS; i++)
	{
		if (s_aCiHlc[i].usSsNo == 0)
			return &s_aCiHlc[i];
	}

	return NULL;
}

static CI_HLC_INSTANCE * CiHlc_GetInst(unsigned short usSsNo)
{
	int i;

	for (i = 0; i < MAX_CI_HLC_SS; i++)
	{
		if(s_aCiHlc[i].usSsNo == usSsNo)
		{
			PrintDbg(("[CIHLC]---> CiHlc_GetInst()---> %d th instance found\n", i));
			return &s_aCiHlc[i];
		}
	}

	return NULL;
}

int CiHlc_NotifyHostCountryInfoChanged(CI_SLOT_ID usSlotId)
{
	int i;

	for (i = 0; i < MAX_CI_HLC_SS; i++)
	{
		if(s_aCiHlc[i].usSsNo)
		{
			if (s_aCiHlc[i].usSlotId == usSlotId)
			{
				return CiHlc_SendCountryInfo(s_aCiHlc[i].usSsNo);
			}
		}
	}

	return ERR_OK; // 세션이 닫힌 상태 (CI+ 캠이 없거나)... 어쨌든 정상 (CIv1 캠은 이 리소스를 지원하지 않음으로 문제되지 않음)
}

int CiHlc_NotifyHostLanguageInfoChanged(CI_SLOT_ID usSlotId)
{
	int i;

	for (i = 0; i < MAX_CI_HLC_SS; i++)
	{
		if(s_aCiHlc[i].usSsNo)
		{
			if (s_aCiHlc[i].usSlotId == usSlotId)
			{
				return CiHlc_SendLanguageInfo(s_aCiHlc[i].usSsNo);
			}
		}
	}

	return ERR_OK; // 세션이 닫힌 상태 (CI+ 캠이 없거나)... 어쨌든 정상 (CIv1 캠은 이 리소스를 지원하지 않음으로 문제되지 않음)
}

