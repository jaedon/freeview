/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_dt.c-arc   1.0   17 Dec 2004 10:32:28   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_dt.c  $
* $Modtime:   13 Dec 2004 09:27:30  $
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
#include <timeconv.h>
#include "ci.h"
#include "cipack_msg.h"
#include "ci_reshandler.h"
#include "ci_dt.h"


/***************************************************************
* definition
***************************************************************/



/***************************************************************
* static variables and structures
***************************************************************/
static CI_DT_INSTANCE	s_aCiDt[MAX_CI_DT_SS];
static unsigned char	s_ucCiDtCnt;
static unsigned long	s_ulCiTickFreq;


/***************************************************************
* static function prototypes
***************************************************************/
static CI_DT_INSTANCE * CiDt_GetInst(unsigned short usSsNo);
static CI_DT_INSTANCE * CiDt_AddInst(unsigned short usSsNo);
static int CiDt_DeleteInst(unsigned short usSsNo);
static int CiDt_SendDateTime(unsigned short usSsNo, unsigned short usMjd, unsigned char ucHour, unsigned char ucMin, unsigned char ucSec);
static int CiDt_StopSendingDateTime(CI_DT_INSTANCE *pDtInst);
static int CiDt_GetCurrentMjdTime(unsigned short *pusMjd, unsigned char *pucHour, unsigned char *pucMin, unsigned char *pucSec);

/***************************************************************
* function bodies
***************************************************************/
int CiDt_Init(void)
{
	VK_memset(s_aCiDt, 0, sizeof(CI_DT_INSTANCE) * MAX_CI_DT_SS);

	s_ucCiDtCnt = 0;
	s_ulCiTickFreq = VK_TIMER_GetTickFreq();

	return ERR_OK;
}

int CiDt_SsOpenedProc(unsigned short usSsNo)
{
	if (CiDt_AddInst(usSsNo) == NULL)
		return -1;

	PrintDbg(("[CIDT]---> CiDt_SsOpenedProc() SsNo[%d]\n", usSsNo));

	return ERR_OK;
}

int CiDt_SsClosedProc(unsigned short usSsNo)
{
#if 0
	CI_DT_INSTANCE * hDt;

	hDt = CiDt_GetInst(usSsNo);
	if (hDt == NULL)
	{
		PrintErr(("[CIDT]---> CiDt_SsClosedProc()---> Error : unknown usSsNo[%d] !!!\n", usSsNo));
		return -1;
	}
#endif

	if (CiDt_DeleteInst(usSsNo) != 0)
	{
		PrintErr(("[CIDT]---> CiDt_SsClosedProc()---> Error : delete instance.SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIDT]---> CiDt_SsClosedProc()---> SsNo[%d]\n", usSsNo));

	return ERR_OK;
}

int CiDt_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;
	CI_DT_INSTANCE *pDtInst;
	unsigned short	usMjd;
	unsigned char	ucHour;
	unsigned char	ucMin;
	unsigned char	ucSec;

	switch (ulApduTag)
	{
	case CI_DATE_TIME_ENQ:
		pDtInst = CiDt_GetInst(usSsNo);
		if (pDtInst == NULL)
		{
			nRtnCode = -1;
			PrintErr(("[CIDT]---> CiDt_ApduProc()---> Error : unknown usSsNo[%d] !!!\n", usSsNo));
			break;
		}

		if (pDtInst->ucInterval)
		{
			if (CiDt_StopSendingDateTime(pDtInst) < 0)
			{
				nRtnCode = -1;
				break;
			}
		}

		if (CiDt_GetCurrentMjdTime(&usMjd, &ucHour, &ucMin, &ucSec) != ERR_OK)
		{
			usMjd = 0;
			ucHour = 0;
			ucMin = 0;
			ucSec = 0;
		}

		/* 일단 현재 시간 한번 보내고.. 매 interval 마다 보냄 */
		if (CiDt_SendDateTime(usSsNo, usMjd, ucHour, ucMin, ucSec) < 0)
		{
			nRtnCode = -1;
			break;
		}

		if (*pucApduData && ulApduLen) /* response_interval */
		{
			pDtInst->ulPrevTick = VK_TIMER_GetSystemTick();
			pDtInst->ucElapsedSec = 0;
			pDtInst->ucInterval = pucApduData[0];
		}
		break;

	default:
		nRtnCode = -1;
		PrintErr(("[CIDT]---> CiDt_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	PrintDbg(("[CIDT]---> CiDt_ApduProc()---> SsNo[%d], tag[0x%lx]\n", usSsNo, ulApduTag));

	return nRtnCode;
}

static CI_DT_INSTANCE * CiDt_GetInst(unsigned short usSsNo)
{
	int i;

	for (i=0; i<MAX_CI_DT_SS; i++)
	{
		if (s_aCiDt[i].usSsNo == usSsNo)
		{
			return &s_aCiDt[i];
		}
	}

	return NULL;
}

static CI_DT_INSTANCE * CiDt_AddInst(unsigned short usSsNo)
{
	CI_DT_INSTANCE * hDt;

	/* free 인 instance(session No. 가 0인 instance) 찾자.... */
	hDt = CiDt_GetInst(0);
	if(hDt == NULL)
	{
		PrintErr(("[CIDT]---> CiDt_AddInst()---> Error : DT sessions are full !!!\n"));
		return NULL;
	}

	hDt->usSsNo = usSsNo;

	s_ucCiDtCnt++;

	return hDt;
}

static int CiDt_DeleteInst(unsigned short usSsNo)
{
	CI_DT_INSTANCE * hDt;

	hDt = CiDt_GetInst(usSsNo);
	if (hDt == NULL)
	{
		PrintErr(("[CIDT]---> CiDt_DeleteInst()---> Error : unknown usSsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	VK_memset(hDt, 0, sizeof(CI_DT_INSTANCE));

	s_ucCiDtCnt--;

	return ERR_OK;
}


static int CiDt_SendDateTime(unsigned short usSsNo, unsigned short usMjd,
							unsigned char ucHour, unsigned char ucMin, unsigned char ucSec)
{
	unsigned char ucMsg[9];

	ucMsg[0] = (unsigned char)((CI_DATE_TIME_REPLY >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_DATE_TIME_REPLY >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_DATE_TIME_REPLY & 0xff);
	ucMsg[3] = 5;  /* UTC_time only, local_offset not used!!!! */

	ucMsg[4] = (unsigned char)((usMjd >> 8) & 0xff);
	ucMsg[5] = (unsigned char)(usMjd & 0xff);

	ucMsg[6] = ucHour % 10;
	ucMsg[6] |= (ucHour / 10) << 4;

	ucMsg[7] = ucMin % 10;
	ucMsg[7] |= (ucMin / 10) << 4;

	ucMsg[8] = ucSec % 10;
	ucMsg[8] |= (ucSec / 10) << 4;

	if (CIRH_SendApdu(usSsNo, ucMsg, 9) != ERR_OK)
	{
		PrintErr(("[CIDT]---> CiDt_SendDateTime()---> Error : fail to send apdu msg : SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

//	PrintDbg(("[CIDT][%d][%d:0x%02x:0x%02x:0x%02x]\n", usSsNo, usMjd, ucMsg[6], ucMsg[7], ucMsg[8]));
//	PrintDbg(("[CIDT]---> CiDt_SendDateTime():SsNo[%d][%d:0x%02x:0x%02x:0x%02x]\n", usSsNo, usMjd, ucBcdHour, ucBcdMin, ucBcdSec));

	return ERR_OK;
}

void CiDt_TimerProc(void)
{
	int	i;
	unsigned short	usMjd;
	unsigned char		ucHour;
	unsigned char		ucMin;
	unsigned char		ucSec;
	unsigned long		ulTickNow;
	unsigned long		ulTickDiff;
	unsigned long		ulElapsedSec;
	unsigned char		ucCiDtSsCnt;

	if (CiDt_GetCurrentMjdTime(&usMjd, &ucHour, &ucMin, &ucSec) != ERR_OK)
		return;

	ucCiDtSsCnt = 0;
	for (i=0; i<MAX_CI_DT_SS; i++)
	{
		if (s_aCiDt[i].usSsNo == 0)
			continue;

		ucCiDtSsCnt++;
		if (s_aCiDt[i].ucInterval != 0)
		{
			ulTickNow = VK_TIMER_GetSystemTick();
//VK_Print("$%d\n", ulTickNow);
			ulTickDiff = ulTickNow - s_aCiDt[i].ulPrevTick;
			ulElapsedSec = ulTickDiff / s_ulCiTickFreq;
			if (ulElapsedSec > 0)
			{
				s_aCiDt[i].ulPrevTick += (s_ulCiTickFreq * ulElapsedSec);
				s_aCiDt[i].ucElapsedSec += (unsigned char)ulElapsedSec;
				if (s_aCiDt[i].ucElapsedSec >= s_aCiDt[i].ucInterval)
				{
					s_aCiDt[i].ucElapsedSec -= s_aCiDt[i].ucInterval;
					CiDt_SendDateTime(s_aCiDt[i].usSsNo, usMjd, ucHour, ucMin, ucSec);
				}
			}
		}

		if (ucCiDtSsCnt == s_ucCiDtCnt)
			break;
	}
}

static int CiDt_StopSendingDateTime(CI_DT_INSTANCE *pDtInst)
{
	if(pDtInst == NULL)
	{
		PrintErr(("[CIDT]---> CiDt_StopSendingDateTime()---> Error : pDtInst is NULL !!!\n"));
		return -1;
	}

	pDtInst->ucInterval = 0;
	pDtInst->ucElapsedSec = 0;
	pDtInst->ulPrevTick = 0;

	return ERR_OK;
}

static int CiDt_GetCurrentMjdTime(unsigned short *pusMjd, unsigned char *pucHour,
							unsigned char *pucMin, unsigned char *pucSec)
{
	unsigned long	ulCurTime;	// UNIXTIME
	HUINT16			usMjd;		// not BCD
	TIME_T			stTime;

	if (VK_CLOCK_GetTime(&ulCurTime))
		return -1;
	if (UTIL_TCONV_ConvertUnixTimeToMJD((UNIXTIME)ulCurTime, &usMjd))
		return -1;
	if (UTIL_TCONV_ConvertUnixTimeToTime((UNIXTIME)ulCurTime, &stTime))
		return -1;

#ifdef CI_PLUS_TEST_Technology_from_SmarDTV /* SmarDTV CI+ 테스트 킷 스트림이 함께 제공된 인증서 날짜 정보와 맞지 않아 테스트용으로 추가한 코드임 */
{
	DATE_T stDate;
	stDate.usYear = 2008;
	stDate.ucMonth = 12;
	stDate.ucDay = 25;
	UTIL_TCONV_ConvertYMDToMJD(&stDate, &usMjd); // or UTIL_TCONV_ConvertDateToMJD(&stDate, &usMjd);
}
#endif

	*pusMjd = usMjd;
	*pucHour = stTime.ucHour;
	*pucMin = stTime.ucMinute;
	*pucSec = stTime.ucSecond;

	return ERR_OK;
}

