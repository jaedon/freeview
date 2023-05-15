/*
	Viaccess PVR API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.123
*/

/********************************************************************
*	Including Headers
********************************************************************/
#include "va_def.h"

#include "va_pvr.h"

#include "va_pi_api.h"

#include "va_pvr_api.h"
#include "va_pvr_adapt.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define USE_PVR_TASK

#define VA_PVR_MSG_SYSTEM_EXIT			0
#define VA_PVR_MSG_OPEN_FILE				1
#define VA_PVR_MSG_CLOSE_FILE				2
#define VA_PVR_MSG_OPEN_PB_FILE			3
#define VA_PVR_MSG_CLOSE_PB_FILE			4
#define VA_PVR_MSG_RECORD_ECM			5
#define VA_PVR_MSG_WRITE_META			6
#define VA_PVR_MSG_READ_META				7
#define VA_PVR_MSG_SET_RECORD_INFO		8
#define VA_PVR_MSG_SET_PLAYBACK_INFO		9
#define VA_PVR_MSG_INC_PLAYBACK_COUNT	10
#define VA_PVR_MSG_PLAYBACK_TIMESTAMP	11
#define VA_PVR_MSG_AUTO_PIN_ENTER		12
#define VA_PVR_MSG_SEND_ECM_BY_PID		13

#define VA_PVR_MAX_ECM_CHANNEL		(VA_PI_MAX_ACS_INSTANCE * 6)


/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/
static VA_PVR_FILE_t 		s_stVaPvrFile[VA_PI_MAX_ACS_INSTANCE];
static VA_ECM_CHANNEL_t	s_stVaEcmChannel[VA_PVR_MAX_ECM_CHANNEL];

static HUINT32			sVaPvrSemId;
#ifdef USE_PVR_TASK
static HUINT32 			sVaPvrQid;
static HUINT32 			sVaPvrTid;
#endif

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
HBOOL VA_PVR_IsChasePlayback(HUINT32 ulAcsId);

#ifdef USE_PVR_TASK
static void VA_PVR_Task(void * args)
{
	UINT32 aulMsg[4];
	UINT32 ulMsgType;
	HBOOL bWaitMsg = TRUE;
	VA_PVR_FILE_t *pFile;
	VA_ECM_DATA_t *pEcm;

	HxLOG_Print("[Enter]\n");

	while (bWaitMsg)
	{
		if (VA_GetMessage(sVaPvrQid, (void *)&aulMsg) == VK_OK)
		{
			ulMsgType = aulMsg[0];

			HxLOG_Info("MSG Received msg_type(%d)\n", ulMsgType);

			switch (ulMsgType)
			{
				case VA_PVR_MSG_OPEN_FILE:
					pFile = (VA_PVR_FILE_t *)aulMsg[1];
					if (aulMsg[2])
					{
						VK_TASK_Sleep(aulMsg[2] * 10);
					}
					VA_PVR_Rec_OpenFile(pFile, aulMsg[2]);
					break;

				case VA_PVR_MSG_CLOSE_FILE:
					pFile = (VA_PVR_FILE_t *)aulMsg[1];
					VA_PVR_Rec_CloseFile(pFile);
					break;

				case VA_PVR_MSG_OPEN_PB_FILE:
					VA_SvcPb_OpenFile((Handle_t)aulMsg[1]);
					break;

				case VA_PVR_MSG_CLOSE_PB_FILE:
					VA_SvcPb_CloseFile((Handle_t)aulMsg[1]);
					break;

				case VA_PVR_MSG_RECORD_ECM:
					pEcm = (VA_ECM_DATA_t *)aulMsg[1];
					VA_PVR_Rec_RecordEcm(pEcm->pEcmChannel, pEcm->ulEcmLength, pEcm->aucEcm, pEcm->ulTimestamp);
					VA_MEM_Free(pEcm);
					break;

				case VA_PVR_MSG_SET_RECORD_INFO:
				{
					HERROR nErr;
					VA_UI_RECORD_INFO stInfo;
					nErr = VA_UI_Adapt_GetRecordInfo(aulMsg[1], aulMsg[2], &stInfo);

					VA_PVR_Adapt_SetRecordInfo(aulMsg[1], &stInfo);
				}
					break;

				case VA_PVR_MSG_SET_PLAYBACK_INFO:
				{
					HERROR nErr;
					VA_UI_PLAYBACK_INFO stInfo;
					nErr = VA_UI_Adapt_GetPlaybackInfo(aulMsg[1], &stInfo);

					VA_PVR_Adapt_SetPlaybackInfo(aulMsg[1], &stInfo);
				}
					break;

				case VA_PVR_MSG_INC_PLAYBACK_COUNT:
					if (VA_PVR_IsChasePlayback(aulMsg[1]) == FALSE)
					{
						// 이미 녹화 완료된 경우
						VA_UI_Adapt_IncrementPlaybackCount(aulMsg[1]);
						HxLOG_Info("[VA_UI_Adapt_IncrementPlaybackCount(%d)\n", aulMsg[1]);
					}

					break;

				case VA_PVR_MSG_PLAYBACK_TIMESTAMP:
					VA_SvcPb_SendEcm(aulMsg[1], aulMsg[2]);
					break;

				case VA_PVR_MSG_AUTO_PIN_ENTER:
					VA_UI_Adapt_AutoPinForRecord(aulMsg[1], aulMsg[2]);
					break;

				case VA_PVR_MSG_SEND_ECM_BY_PID:
					VA_SvcPb_SendEcmByPid(aulMsg[1], (HUINT16)aulMsg[2]);
					break;

				case VA_PVR_MSG_SYSTEM_EXIT:
					bWaitMsg = FALSE;
					break;

				default:
					HxLOG_Info( "Unknown Msg\n");
					break;
			}
		}
	}

	VA_MSG_Destroy(sVaPvrQid);

	HxLOG_Print("[Exit]\n");
}
#endif

void VA_PVR_SendEcmByPid(HUINT32 ulAcsId, HUINT16 usPid)
{
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter] \n");

#ifdef USE_PVR_TASK
	aulMsg[0] = VA_PVR_MSG_SEND_ECM_BY_PID;
	aulMsg[1] = ulAcsId;
	aulMsg[2] = (HUINT16)usPid;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif

	HxLOG_Print("[Exit] \n");
}

void VA_PVR_AutoPinForRecord(HUINT32 ulAcsId, HUINT32 ulScSlot)
{
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter] \n");

	if (VA_PI_Adapt_IsRecord(ulAcsId))
	{
#ifdef USE_PVR_TASK
		aulMsg[0] = VA_PVR_MSG_AUTO_PIN_ENTER;
		aulMsg[1] = ulAcsId;
		aulMsg[2] = ulScSlot;
		VA_PostMessage(sVaPvrQid, aulMsg);
#endif
	}

	HxLOG_Print("[Exit] \n");
}

HBOOL VA_PVR_IsChasePlayback(HUINT32 ulAcsId)
{
	int i;

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		// file info에 ulAcsId가 새팅되어 있고, hVaActionPb도 세팅되어있으면 chase playback 중인 경우..
		if (s_stVaPvrFile[i].eStatus != eVaPvrFileStatus_Close && s_stVaPvrFile[i].bRecord == TRUE && s_stVaPvrFile[i].hVaActionPb != HANDLE_NULL)
		{
			if (ulAcsId == VA_PI_Adapt_GetAcsIdByVaActionHandle(s_stVaPvrFile[i].hVaActionPb))
				return TRUE;
		}
	}

	return FALSE;
}

void VA_PVR_SetTimestamp(Handle_t hAction, HUINT32 ulCurrentTime)
{
	UINT32 aulMsg[4];

HxLOG_Print("[Enter(%x,%d)] \n", hAction, ulCurrentTime);

#ifdef USE_PVR_TASK
	aulMsg[0] = VA_PVR_MSG_PLAYBACK_TIMESTAMP;
	aulMsg[1] = hAction;
	aulMsg[2] = ulCurrentTime;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif

HxLOG_Print("[Exit] \n");
}

void VA_PVR_IncrementPlaybackCount(HUINT32 ulAcsId)
{
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter] \n");

#ifdef USE_PVR_TASK
	aulMsg[0] = VA_PVR_MSG_INC_PLAYBACK_COUNT;
	aulMsg[1] = ulAcsId;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif

	VA_PVR_SetPlaybackInfo(ulAcsId);

	HxLOG_Print("[Exit] \n");
}

void VA_PVR_SetRecordInfo(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle)
{
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter] \n");

#ifdef USE_PVR_TASK
	aulMsg[0] = VA_PVR_MSG_SET_RECORD_INFO;
	aulMsg[1] = ulAcsId;
	aulMsg[2] = ulStbStreamHandle;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif

	HxLOG_Print("[Exit] \n");
}

void VA_PVR_SetPlaybackInfo(HUINT32 ulAcsId)
{
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter] \n");

#ifdef USE_PVR_TASK
	aulMsg[0] = VA_PVR_MSG_SET_PLAYBACK_INFO;
	aulMsg[1] = ulAcsId;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif

	HxLOG_Print("[Exit] \n");
}

VA_PVR_FILE_t *VA_PVR_GetFileInfo(HUINT32 ulAcsId)
{
	int i;

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		if (s_stVaPvrFile[i].eStatus != eVaPvrFileStatus_Close && s_stVaPvrFile[i].ulAcsId == ulAcsId)
		{
			return &s_stVaPvrFile[i];
		}
	}

	return NULL;
}

VA_PVR_FILE_t *VA_PVR_GetPbFileInfoByVaActionHandle(Handle_t hVaAction)
{
	int i;

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		if (s_stVaPvrFile[i].eStatus == eVaPvrFileStatus_Open && s_stVaPvrFile[i].hVaActionPb == hVaAction)
		{
			return &s_stVaPvrFile[i];
		}
	}

	return NULL;
}

VA_PVR_FILE_t *VA_PVR_GetPbFileInfo(Handle_t hVaAction, HUINT32 ulContentsId, HBOOL bCreate)
{
	int i;

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		if (s_stVaPvrFile[i].eStatus == eVaPvrFileStatus_Open && s_stVaPvrFile[i].ulContentsId == ulContentsId)
		{
			return &s_stVaPvrFile[i];
		}
	}

	if (bCreate)
	{
		for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
		{
			if (s_stVaPvrFile[i].eStatus == eVaPvrFileStatus_Close)
			{
				//## 새로 파일 생성요청
				s_stVaPvrFile[i].ulAcsId = VA_PI_Adapt_GetAcsIdByVaActionHandle(hVaAction);
				s_stVaPvrFile[i].ulTimestamp = 0;
				s_stVaPvrFile[i].eStatus = eVaPvrFileStatus_FileOpen;
				s_stVaPvrFile[i].bIsTimeshift = FALSE;
				s_stVaPvrFile[i].ulContentsId = ulContentsId;
				s_stVaPvrFile[i].bRecord = FALSE;
				return &s_stVaPvrFile[i];
			}
		}
	}

	return NULL;
}

void VA_PVR_OpenEcmFileRetry(VA_PVR_FILE_t *pFile, HUINT32 ulRetryCnt)
{
#ifdef USE_PVR_TASK
	UINT32 aulMsg[4];

	aulMsg[0] = VA_PVR_MSG_OPEN_FILE;
	aulMsg[1] = (HUINT32)pFile;
	aulMsg[2] = ulRetryCnt;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif
}

void VA_PVR_OpenPbFile(Handle_t hVaAction)
{
#ifdef USE_PVR_TASK
	UINT32 aulMsg[4];

	aulMsg[0] = VA_PVR_MSG_OPEN_PB_FILE;
	aulMsg[1] = (HUINT32)hVaAction;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif
}

void VA_PVR_ClosePbFile(Handle_t hVaAction)
{
#ifdef USE_PVR_TASK
	UINT32 aulMsg[4];

	aulMsg[0] = VA_PVR_MSG_CLOSE_PB_FILE;
	aulMsg[1] = (HUINT32)hVaAction;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif
}

void VA_PVR_OpenEcmFile(HUINT32 ulAcsId, HBOOL bIsTimeshift)
{
	int idx;
	VA_PVR_FILE_t *pFile = NULL;
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter](acs:%d,TSR:%d)\n", ulAcsId, bIsTimeshift);

	for (idx = 0; idx < VA_PI_MAX_ACS_INSTANCE; idx++)
	{
		if (s_stVaPvrFile[idx].eStatus == eVaPvrFileStatus_Close)
		{
			//## 새로 파일 생성요청
			pFile = &s_stVaPvrFile[idx];

			pFile->ulAcsId = ulAcsId;
			pFile->ulTimestamp = 0;
			pFile->eStatus = eVaPvrFileStatus_FileOpen;
			pFile->bIsTimeshift = bIsTimeshift;
			pFile->hVaActionPb = HANDLE_NULL;
			pFile->bBlocked = FALSE;
			pFile->nTimeOffset = 0;
			pFile->bRecord = TRUE;

#if defined(USE_PVR_TASK) && !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST) // driver test시에는 msg왔다 갔다하는 사이에 fail 나버림..
			aulMsg[0] = VA_PVR_MSG_OPEN_FILE;
			aulMsg[1] = (HUINT32)pFile;
			aulMsg[2] = 0;
			VA_PostMessage(sVaPvrQid, aulMsg);
#else
			VA_PVR_Rec_OpenFile(pFile, 0);
#endif
			break;
		}
	}
	HxLOG_Print("[Exit]\n");
}

void VA_PVR_CloseEcmFile(HUINT32 ulAcsId)
{
	int idx;
	VA_PVR_FILE_t *pFile = NULL;
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter](acs:%d)\n", ulAcsId);

	for (idx = 0; idx < VA_PI_MAX_ACS_INSTANCE; idx++)
	{
		if (s_stVaPvrFile[idx].ulAcsId == ulAcsId)
		{
			// 파일을 닫는다..
			pFile = &s_stVaPvrFile[idx];
			pFile->eStatus = eVaPvrFileStatus_FileClose;
			pFile->bRecord = FALSE;

#ifdef USE_PVR_TASK
			aulMsg[0] = VA_PVR_MSG_CLOSE_FILE;
			aulMsg[1] = (HUINT32)pFile;
			VA_PostMessage(sVaPvrQid, aulMsg);
#else
			VA_PVR_Rec_CloseFile(pFile);
#endif
			break;
		}
	}
	HxLOG_Print("[Exit]\n");
}

void VA_PVR_Init()
{
	int nErr;
	int i;
	UINT8 szSemName[10];

	HxLOG_Print("[Enter]\n");

	VA_PVR_Adapt_Init();

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		s_stVaPvrFile[i].eStatus = eVaPvrFileStatus_Close;
		s_stVaPvrFile[i].hVaActionPb = HANDLE_NULL;
		s_stVaPvrFile[i].bIsTimeshift = FALSE;
		sprintf(szSemName, "VPS%d", i);
		VA_SEM_Create(szSemName, &s_stVaPvrFile[i].ulSemId);
	}

	for (i = 0; i < VA_PVR_MAX_ECM_CHANNEL; i++)
	{
		s_stVaEcmChannel[i].pFile = NULL;
	}

	nErr = VA_SEM_Create(VA_PVR_SEM_NAME, &sVaPvrSemId);
	if (nErr)
	{
		HxLOG_Info("failed to create semaphore (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

#ifdef USE_PVR_TASK
	VA_CreateMessageQueue(VA_PVR_QUEUE_NAME, &sVaPvrQid);

	nErr = VK_TASK_Create(VA_PVR_Task, VA_PVR_TASK_PRIORITY, VA_PVR_TASK_STACK_SIZE, VA_PVR_TASK_NAME, NULL, &sVaPvrTid, 0);
	if (nErr)
	{
		HxLOG_Info("failed to create task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VK_TASK_Start(sVaPvrTid);
	if (nErr)
	{
		HxLOG_Info("failed to start task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}
#endif

	HxLOG_Print("[Exit]\n");
}

void VA_PVR_Exit()
{
	UINT32 aulMsg[4];

	HxLOG_Print("[Enter] \n");

#ifdef USE_PVR_TASK
	aulMsg[0] = VA_PVR_MSG_SYSTEM_EXIT;
	VA_PostMessage(sVaPvrQid, aulMsg);
#endif

	HxLOG_Print("[Exit] \n");
}


/*
	VA_PVR_OpenEcmChannel
		open one ECM channel corresponding to one ECM PID
	arguments
		dwAcsId
		eRecordType
		wEcmChannelPid
	return
		kVA_ILLEGAL_HANDLE
		dwStbEcmChannelHandle
*/
DWORD VA_PVR_OpenEcmChannel(DWORD dwAcsId, tVA_PVR_RecordType eRecordType, WORD wEcmChannelPid)
{
	VA_PI_AcsInstance *pAcs;
	VA_PVR_FILE_t *pFile = NULL;
	int idx;
	HBOOL bSuccess = FALSE;

	HxLOG_Print("[Enter](%d,%d,%x)\n", dwAcsId, eRecordType, wEcmChannelPid);

	// wrong dwAcsId
	pAcs = VA_PI_Adapt_GetAcsInstById(dwAcsId);
	if (pAcs == NULL)
	{
		HxLOG_Info("dwAcsId is wrong (%d)\n", dwAcsId);
		return kVA_ILLEGAL_HANDLE;
	}

	// eRecordType is not a value from tVA_PVR_RecordType enumeration
	if (eRecordType != ePERSISTENT && eRecordType != eTIMESHIFT)
	{
		HxLOG_Info("eRecordType is wrong (%d)\n", eRecordType);
		return kVA_ILLEGAL_HANDLE;
	}

	// eRecordType is not compatible with ACS operating mode (e.g. eTIMESHIDFT_SCRAMBLED is not compatible with eLIVE_AND_PERSISTENT_RECORD)
	if ((eRecordType == ePERSISTENT && !(pAcs->nAcsMode == ePERSISTENT_RECORD || pAcs->nAcsMode == eLIVE_AND_PERSISTENT_RECORD)) ||
		(eRecordType == eTIMESHIFT && !(pAcs->nAcsMode == eTIMESHIFT_RECORD || pAcs->nAcsMode == eLIVE_AND_TIMESHIFT_RECORD)))
	{
		HxLOG_Info("eRecordType and acs mode is not match (%d,%d)\n", eRecordType, pAcs->nAcsMode);
		return kVA_ILLEGAL_HANDLE;
	}

	// wrong pid
	if (wEcmChannelPid > 0x1FFF)
	{
		HxLOG_Info("wrong wEcmChannelPid(%x)\n", wEcmChannelPid);
		return kVA_ILLEGAL_HANDLE;
	}

	VA_SEM_Get(sVaPvrSemId);

	// 1. 파일 확인

	for (idx = 0; idx < VA_PI_MAX_ACS_INSTANCE; idx++)
	{
		if (s_stVaPvrFile[idx].eStatus == eVaPvrFileStatus_Open && s_stVaPvrFile[idx].ulAcsId == dwAcsId)
		{
			HxLOG_Info("file for acs (%d) is existed\n", dwAcsId);
			pFile = &s_stVaPvrFile[idx];
			break;
		}
	}

	if (pFile == NULL)
	{
		HxLOG_Info("fail to open file\n");
		VA_SEM_Release(sVaPvrSemId);
		return kVA_ILLEGAL_HANDLE;
	}

	// 2. ecm channel 확인

#if 0
	//## 이미 있는 것을 다시 open하면 어찌해야하나? error? 이런경우가 없다면 아래 for는 제거
	for (idx = 0; idx < VA_PVR_MAX_ECM_CHANNEL; idx++)
	{
		if (s_stVaEcmChannel[idx].pFile == pFile && s_stVaEcmChannel[idx].usPid == wEcmChannelPid)
		{
			//## 같은건데 우짜까???
			bSuccess = TRUE;
			break;
		}
	}

	if (bSuccess == FALSE)
	{
		for (idx = 0; idx < VA_PVR_MAX_ECM_CHANNEL; idx++)
		{
			if (s_stVaEcmChannel[idx].pFile == NULL)
			{
				s_stVaEcmChannel[idx].pFile = pFile;
				s_stVaEcmChannel[idx].usPid = wEcmChannelPid;
				bSuccess = TRUE;
				break;
			}
		}
	}

	if (bSuccess == FALSE)
	{
		HxLOG_Info("fail to open ecm channel\n");
		VA_SEM_Release(sVaPvrSemId);
		return kVA_ILLEGAL_HANDLE;
	}
#else
// driver test에서는 같은 pid가 와도 에러 없이 정상 처리하도록 되어있음
	for (idx = 0; idx < VA_PVR_MAX_ECM_CHANNEL; idx++)
	{
		if (s_stVaEcmChannel[idx].pFile == NULL)
		{
			s_stVaEcmChannel[idx].pFile = pFile;
			s_stVaEcmChannel[idx].usPid = wEcmChannelPid;
			break;
		}
	}
#endif

	VA_SEM_Release(sVaPvrSemId);
	HxLOG_Print("[Exit](%d)\n", idx);
	return (DWORD)idx;
}

/*
	VA_PVR_CloseEcmChannel
		close an ECM channel
	arguments
		dwStbEcmChannelHandle: returned by VA_PVR_OpenEcmChannel()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_PVR_CloseEcmChannel(DWORD dwStbEcmChannelHandle)
{
	HxLOG_Print("[Enter](%d)\n", dwStbEcmChannelHandle);

	// dwStbEcmChannelHandle doesn't correspond to a valid opened ECM channel
	// close ecm channel
	VA_SEM_Get(sVaPvrSemId);

	if (dwStbEcmChannelHandle >= VA_PVR_MAX_ECM_CHANNEL || s_stVaEcmChannel[dwStbEcmChannelHandle].pFile == NULL)
	{
		HxLOG_Info("wrong handle(%x)\n", dwStbEcmChannelHandle);
		VA_SEM_Release(sVaPvrSemId);
		return kVA_INVALID_PARAMETER;
	}

	s_stVaEcmChannel[dwStbEcmChannelHandle].pFile = NULL;

	VA_SEM_Release(sVaPvrSemId);
	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_PVR_RecordEcm
		record one ECM associated to a previously opened ECM channel
	arguments
		dwStbEcmChannelHandle: returned by VA_PVR_OpenEcmChannel()
		uiEcmLength
		pEcm: freed by VA
		pUserData
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_BUSY
*/
INT VA_PVR_RecordEcm(DWORD dwStbEcmChannelHandle, UINT32 uiEcmLength, BYTE *pEcm, void *pUserData)
{
	UINT32 aulMsg[4];
	VA_ECM_DATA_t *pEcmData;

	HxLOG_Print("[Enter]\n");

	// uiEcmLength is out of range
	if (uiEcmLength == 0 || uiEcmLength > kVA_PVR_ECM_MAX_SIZE)
	{
		HxLOG_Info("uiEcmLength is wrong (%d)\n", uiEcmLength);
		return kVA_INVALID_PARAMETER;
	}

	if (pEcm == NULL)
	{
		HxLOG_Info("pEcm is null\n");
		return kVA_INVALID_PARAMETER;
	}

	VA_SEM_Get(sVaPvrSemId);

	// dwStbEcmChannelHandle doesn't correspond to a valid opened ECM channel
	if (dwStbEcmChannelHandle >= VA_PVR_MAX_ECM_CHANNEL || s_stVaEcmChannel[dwStbEcmChannelHandle].pFile == NULL)
	{
		HxLOG_Info("wrong handle(%x)\n", dwStbEcmChannelHandle);
		VA_SEM_Release(sVaPvrSemId);
		return kVA_INVALID_PARAMETER;
	}

#if 0
	// STB is already processing a recording on this ECM channel
	if (0)
	{
		return kVA_RESOURCE_BUSY;
	}
#endif

#ifdef USE_PVR_TASK
	pEcmData = (VA_ECM_DATA_t *)VA_MEM_Alloc(sizeof(VA_ECM_DATA_t));
	if (pEcmData == NULL)
	{
		HxLOG_Info("alloc fail\n");
		VA_SEM_Release(sVaPvrSemId);
		return kVA_ERROR;
	}

	pEcmData->pEcmChannel 	= &s_stVaEcmChannel[dwStbEcmChannelHandle];
	pEcmData->ulEcmLength 	= uiEcmLength;
	pEcmData->ulTimestamp 	= (HUINT32)pUserData;
	VA_memcpy(pEcmData->aucEcm, pEcm, uiEcmLength);

	aulMsg[0] = VA_PVR_MSG_RECORD_ECM;
	aulMsg[1] = (HUINT32)pEcmData;
	VA_PostMessage(sVaPvrQid, aulMsg);
#else
	VA_PVR_Rec_RecordEcm(&s_stVaEcmChannel[dwStbEcmChannelHandle], uiEcmLength, pEcm, (HUINT32)pUserData);
#endif

	VA_SEM_Release(sVaPvrSemId);
	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_PVR_WriteMetadata
		write metadata file which contains VA private information
	arguments
		dwAcsId
		uiMetadataLength
		pMetaData: allocated and freed by VA
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_BUSY
*/
INT VA_PVR_WriteMetadata(DWORD dwAcsId, UINT32 uiMetadataLength, BYTE *pMetaData)
{
	VA_PI_AcsInstance *pAcs;

	HxLOG_Print("[Enter]\n");

	// wrong dwAcsId or ACS mode is eLIVE
	pAcs = VA_PI_Adapt_GetAcsInstById(dwAcsId);
	if (pAcs == NULL || pAcs->nAcsMode == eLIVE)
	{
		HxLOG_Info("dwAcsId is wrong (%d) or eLIVE mode\n", dwAcsId);
		return kVA_INVALID_PARAMETER;
	}

	if (uiMetadataLength == 0 || uiMetadataLength > kVA_PVR_METADATA_MAX_SIZE)
	{
		HxLOG_Info("uiMetadataLength is wrong (%d)\n", uiMetadataLength);
		return kVA_INVALID_PARAMETER;
	}

	if (pMetaData == NULL)
	{
		HxLOG_Info("pMetaData is null\n");
		return kVA_INVALID_PARAMETER;
	}

#if 0
	// read/write meda data is already in progress for this ACS instance
	if (0)
	{
		return kVA_RESOURCE_BUSY;
	}
#endif

	VA_SEM_Get(sVaPvrSemId);

	// request to write meta data
	VA_PVR_Adapt_WriteMetaData(dwAcsId, uiMetadataLength, pMetaData);

	VA_SEM_Release(sVaPvrSemId);
	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_PVR_ReadMetadata
		read metadata file which contains VA private information
	arguments
		dwAcsId
		uiMetadataLength
		pMetaData: allocated and freed by VA
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_RESOURCE_BUSY
*/
INT VA_PVR_ReadMetadata(DWORD dwAcsId, UINT32 uiMetadataLength, BYTE *pMetaData)
{
	VA_PI_AcsInstance *pAcs;

	HxLOG_Print("[Enter]\n");

	// wrong dwAcsId or ACS mode is eLIVE
	pAcs = VA_PI_Adapt_GetAcsInstById(dwAcsId);
	if (pAcs == NULL || pAcs->nAcsMode == eLIVE)
	{
		HxLOG_Info("dwAcsId is wrong (%d) or eLIVE mode\n", dwAcsId);
		return kVA_INVALID_PARAMETER;
	}

	if (uiMetadataLength == 0 || uiMetadataLength > kVA_PVR_METADATA_MAX_SIZE)
	{
		HxLOG_Info("uiMetadataLength is wrong (%d)\n", uiMetadataLength);
		return kVA_INVALID_PARAMETER;
	}

	if (pMetaData == NULL)
	{
		HxLOG_Info("pMetaData is null\n");
		return kVA_INVALID_PARAMETER;
	}

#if 0
	// read/write meda data is already in progress for this ACS instance
	if (0)
	{
		return kVA_RESOURCE_BUSY;
	}
#endif

	VA_SEM_Get(sVaPvrSemId);

	// request to read meta data
	VA_PVR_Adapt_ReadMetaData(dwAcsId, uiMetadataLength, pMetaData);

	VA_SEM_Release(sVaPvrSemId);
	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

