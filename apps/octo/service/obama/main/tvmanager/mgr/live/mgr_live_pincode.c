/**
	@file     mgr_live_pincode.c
	@brief    WTV, TSR Playback, Chase Playback ���� Ǭ ���� �ִ� event ������ ����ϴ� application.

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <linkedlist.h>

#include <mgr_action.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define		MAX_EVENT_NUMBER		30


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct {
	Handle_t		hSvc;
	HBOOL		bServiceLockPassed;
	HUINT8		ucEventNum;
	HINT32		nEvtId[MAX_EVENT_NUMBER];
	HUINT8		ucPRate[MAX_EVENT_NUMBER];
}Pincode_Release_Event_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/





/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC Pincode_Release_Event_t 		s_pReleaseEvtPinList[NUM_REC_ACTION+NUM_TSRREC_ACTION];

/*******************************************************************/
/***************** Static Function Prototypes  *********************/
/*******************************************************************/

#define _______________GLOBAL_FUNCTION___________________

STATIC void mgr_live_PinvcodeDeleteNoRecSvc(void)
{
	HUINT32 					ulIdx, ulRecActId;
	MgrRec_RecInfo_t	stRecInfo;
	HERROR					err;
	HBOOL					bRecordingFound;

	for(ulIdx = 0; ulIdx < (NUM_REC_ACTION+NUM_TSRREC_ACTION); ulIdx++)
	{
		if(s_pReleaseEvtPinList[ulIdx].hSvc != 0)
		{ /* ��ϵ� service handle�� ã�Ƽ� ... */

			bRecordingFound = FALSE;
			for(ulRecActId = eActionId_REC_FIRST; ulRecActId <= eActionId_REC_LAST; ulRecActId++)
			{ /* Recording�� �ɸ� �������� �˾ƺ��� ... */
				err = MGR_RECORDER_GetRecordingSvcInfo(ulRecActId, &stRecInfo);
				if( (err == ERR_OK) && (stRecInfo.hMasterSvc == s_pReleaseEvtPinList[ulIdx].hSvc) )
				{
					bRecordingFound = TRUE;
					break;
				}
			}

			if(bRecordingFound == FALSE)
			{ /* Recording�� �� �ɸ� service handle�� ������. */
				HxLOG_Info("[mgr_live_PinvcodeDeleteNoRecSvc] **Dalgling SVC found. Delete this service from pincode history** hSvc=0x%x\n",
												s_pReleaseEvtPinList[ulIdx].hSvc);
				s_pReleaseEvtPinList[ulIdx].hSvc = 0;
				s_pReleaseEvtPinList[ulIdx].ucEventNum = 0;
			}
		}
	}
}

/* WatchTV, Tsr Playback, Chase Playback ���߿� Pincode �� Ǯ��, �� ������ eventid �� parental rate�� ����Ʈ�� �����Ѵ�.
	View action�� recording action�� instance�� ����ϹǷ� hSvc �� interface �Ѵ�. */
HERROR	MGR_LIVE_PincodeSetRatingPassedEventId(Handle_t hSvc, HINT32 nEvtId, HUINT8 ucPRate)
{
	HUINT32 	ulIdx, ulEvtIdx, ulIdx2;

	mgr_live_PinvcodeDeleteNoRecSvc(); /* ���� �� û���ϴ� ���� �ƴ϶� ������ �� �Ź� û���ϰ� ���� �Ѵ� ... */

	/* �̹� ��ϵ� service handle �˻� */
	for(ulIdx = 0; ulIdx < (NUM_REC_ACTION+NUM_TSRREC_ACTION); ulIdx++)
	{
		if(s_pReleaseEvtPinList[ulIdx].hSvc == hSvc)
		{
			for(ulEvtIdx = 0; (ulEvtIdx < s_pReleaseEvtPinList[ulIdx].ucEventNum) && (ulEvtIdx < MAX_EVENT_NUMBER); ulEvtIdx++)
			{
				if(s_pReleaseEvtPinList[ulIdx].nEvtId[ulEvtIdx] == nEvtId)
				{ /* �̹� ��ϵ� event ID �߰�. rating value�� update�ϰ� ����. */
					s_pReleaseEvtPinList[ulIdx].ucPRate[ulEvtIdx] = ucPRate;

					HxLOG_Info("[MGR_LIVE_PincodeSetRatingPassedEventId] **Already EVENT FOUND** hSvc=0x%x bServiceLockPassed=%d nEvtId=%d ucEventNum=%d\n",
										hSvc,
										s_pReleaseEvtPinList[ulIdx].bServiceLockPassed,
										s_pReleaseEvtPinList[ulIdx].nEvtId[ulEvtIdx],
										s_pReleaseEvtPinList[ulIdx].ucEventNum);
					return ERR_OK;
				}
			}

			/* ���ο� event ID ���. */
			if(s_pReleaseEvtPinList[ulIdx].ucEventNum >= MAX_EVENT_NUMBER)
			{ /* wrap around */
				s_pReleaseEvtPinList[ulIdx].ucEventNum = 0;
			}
			s_pReleaseEvtPinList[ulIdx].nEvtId[s_pReleaseEvtPinList[ulIdx].ucEventNum] = nEvtId;
			s_pReleaseEvtPinList[ulIdx].ucPRate[s_pReleaseEvtPinList[ulIdx].ucEventNum] = ucPRate;
			s_pReleaseEvtPinList[ulIdx].ucEventNum++;

			for(ulIdx2 = 0; ulIdx2 < s_pReleaseEvtPinList[ulIdx].ucEventNum; ulIdx2++)
			{
				HxLOG_Info("[MGR_LIVE_PincodeSetRatingPassedEventId] **Registered EVENT ID** hSvc=0x%x bServiceLockPassed=%d nEvtId=%d ucEventNum=%d\n",
								hSvc,
								s_pReleaseEvtPinList[ulIdx].bServiceLockPassed,
								s_pReleaseEvtPinList[ulIdx].nEvtId[ulIdx2],
								s_pReleaseEvtPinList[ulIdx].ucEventNum);
			}
			return ERR_OK;
		}
	}

	/* ���ο� service handle ��� */
	for(ulIdx = 0; ulIdx < (NUM_REC_ACTION+NUM_TSRREC_ACTION); ulIdx++)
	{
		if(s_pReleaseEvtPinList[ulIdx].hSvc == 0)
		{
			s_pReleaseEvtPinList[ulIdx].hSvc = hSvc;
			s_pReleaseEvtPinList[ulIdx].bServiceLockPassed = FALSE;
			s_pReleaseEvtPinList[ulIdx].nEvtId[0] = nEvtId;
			s_pReleaseEvtPinList[ulIdx].ucPRate[0] = ucPRate;
			s_pReleaseEvtPinList[ulIdx].ucEventNum = 1;

			HxLOG_Info("[MGR_LIVE_PincodeSetRatingPassedEventId] **NEW SVC** hSvc=0x%x bServiceLockPassed=%d nEvtId=%d ucEventNum=%d\n",
								hSvc,
								s_pReleaseEvtPinList[ulIdx].bServiceLockPassed,
								s_pReleaseEvtPinList[ulIdx].nEvtId[0],
								s_pReleaseEvtPinList[ulIdx].ucEventNum);

			return ERR_OK;
		}
	}

	HxLOG_Print("\n");

	return ERR_FAIL;
}

// WatchTV, Tsr Playback, Chase Playback ���߿� ���ο� event �� ������, �� event �� ������ ���� �ִ��� Ȯ���Ѵ�.
HERROR	MGR_LIVE_PincodeIsEventIdRatingPassed(Handle_t hSvc, HINT32 nEvtId, HUINT8 *ucPRate, HBOOL *bRatingPassed)
{
	HUINT32		ulIdx, ulEvtIdx;

	mgr_live_PinvcodeDeleteNoRecSvc(); /* ���� �� û���ϴ� ���� �ƴ϶� ������ �� �Ź� û���ϰ� ���� �Ѵ� ... */

	if(!bRatingPassed || !ucPRate)
	{
		return ERR_FAIL;
	}

	*bRatingPassed = FALSE;
	*ucPRate = FALSE;

	for(ulIdx = 0; ulIdx < (NUM_REC_ACTION+NUM_TSRREC_ACTION); ulIdx++)
	{
		if(s_pReleaseEvtPinList[ulIdx].hSvc == hSvc)
		{
			if(s_pReleaseEvtPinList[ulIdx].ucEventNum == 0)
			{
				HxLOG_Info("[MGR_LIVE_PincodeIsEventIdRatingPassed] **SVC FOUND but NO event registrated** hSvc=0x%x nEvtId=%d\n", hSvc, nEvtId);
				return ERR_OK;
			}
			else
			{
				for(ulEvtIdx = 0; (ulEvtIdx < s_pReleaseEvtPinList[ulIdx].ucEventNum) && (ulEvtIdx < MAX_EVENT_NUMBER); ulEvtIdx++)
				{
					if(s_pReleaseEvtPinList[ulIdx].nEvtId[ulEvtIdx] == nEvtId)
					{
						HxLOG_Info("[MGR_LIVE_PincodeIsEventIdRatingPassed] **EVENT FOUND** hSvc=0x%x %dth EvtId=%d PRate=%x\n",
												hSvc,
												ulEvtIdx,
												s_pReleaseEvtPinList[ulIdx].nEvtId[ulEvtIdx],
												s_pReleaseEvtPinList[ulIdx].ucPRate[ulEvtIdx]);

						*bRatingPassed = TRUE; // PC ������ �� ����.
						*ucPRate = s_pReleaseEvtPinList[ulIdx].ucPRate[ulEvtIdx];

						return ERR_OK;
					}
					else
					{
						HxLOG_Info("[MGR_LIVE_PincodeIsEventIdRatingPassed] **SVC FOUND but NOT registered EVENT** [idx=%d evtId=%d] hSvc=0x%x nEvtId=%d\n", ulEvtIdx, s_pReleaseEvtPinList[ulIdx].nEvtId[ulEvtIdx], hSvc, nEvtId);
					}
				}
			}
		}
	}

	HxLOG_Info("[MGR_LIVE_PincodeIsEventIdRatingPassed] **SVC NOT FOUND** hSvc=0x%x nEvtId=%d\n", hSvc, nEvtId);
	return ERR_OK;
}

HERROR	MGR_LIVE_PincodeDeleteAllServiceHandle(void)
{
	HUINT32					ulIdx;

	HxLOG_Info("[MGR_LIVE_PincodeDeleteAllServiceHandle]\n");

	for(ulIdx = 0; ulIdx < (NUM_REC_ACTION+NUM_TSRREC_ACTION); ulIdx++)
	{
		s_pReleaseEvtPinList[ulIdx].hSvc = 0;
		s_pReleaseEvtPinList[ulIdx].ucEventNum = 0;
	}

	return ERR_OK;
}
/*********************** End of File ******************************/

