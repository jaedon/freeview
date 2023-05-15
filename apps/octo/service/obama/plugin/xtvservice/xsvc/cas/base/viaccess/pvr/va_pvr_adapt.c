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

//#include <_svc_cas_mgr_common.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define VA_PVR_MAX_FILE_NAME_LENGTH		PAL_PVR_MAX_FILE_NAME_LENGTH
#define VA_PVR_META_BUFFER_SIZE				1024

#define VA_PVR_TSR_BUFFER_SIZE	(60*60*2/10*5)	// 2�ð� * 10�ʿ� �ѹ� ecm ����, �ִ� 5�� ECM ó�� ����..(�Ϲ������δ� �̷��Ա����� �ȿõ�)
#define VA_PVR_PB_BUFFER_SIZE		(1024*200)

#define VA_PVR_INVALID_VALUE		0xFFFFFFFF

#define VA_PVR_ECM_FILE_OPEN_MAX_RETRY	5

#define EXT_VA_PVR_META_FILE			".vme"
#define EXT_VA_PVR_ECM_FILE				".vec"

/********************************************************************
*	External variables & function prototypes
********************************************************************/

typedef struct tag_VA_ECM_LIST_t {
	HUINT32 ulValidStart;	// timestamp
	HUINT32 ulValidEnd;
	HUINT16 usPid;
	HUINT16 usEcmLength;
	HUINT8 aucEcm[kVA_PVR_ECM_MAX_SIZE];

	struct tag_VA_ECM_LIST_t *pPrev;
	struct tag_VA_ECM_LIST_t *pNext;
	struct tag_VA_ECM_LIST_t *pGroup;
} VA_ECM_LIST_t;

/********************************************************************
*	Static variables
********************************************************************/
static HUINT32 s_ulCurrentTime;

static HUINT8 *s_pucPbFileBuffer;
static HUINT32 s_ulCurEcmIndex;
static HUINT32 s_ulCurEcmValidStart;
static HUINT32 s_ulCurEcmValidEnd;

static VA_ECM_DATA_t *s_pstTsrBuf;
static HUINT32 s_ulTsrBufHead, s_ulTsrBufTail;

static VA_ECM_LIST_t *s_pstEcmListHead, *s_pstEcmListTail;
static VA_ECM_LIST_t *s_pstCurrentEcm;
static HBOOL s_bIsCurEcmSent;

static HUINT32 sVaTsrSemId;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/

HERROR VA_PVR_Adapt_SetRecordInfo(HUINT32 ulAcsId, VA_UI_RECORD_INFO *pstInfo)
{
	Handle_t 			hVaAction;
	SvcMeta_Record_t 	stRecInfo;
	HUINT32			ulContentsId;

	HxLOG_Print("[Enter](%d)\n", ulAcsId);

	hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(ulAcsId);

	if (SVC_META_GetContentIdByhAction(VAMGR_GetActionHandle(hVaAction), &ulContentsId) != ERR_OK)
	{
		HxLOG_Info("fail to get contentid for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	if (SVC_META_GetRecordInfo(ulContentsId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Info("fail to get recordinfo for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

#if 1
//VA_OS_Printf("** VA_PVR_Adapt_SetRecordInfo(%d, %d, %s)\n", pstInfo->ulPlaybackDuration, pstInfo->ulMaxNumberOfPlayback, pstInfo->szExpireDate);
	stRecInfo.stDrmInfo.info.stViaccessDrmInfo.ulPlaybackDuration = pstInfo->ulPlaybackDuration;
	stRecInfo.stDrmInfo.info.stViaccessDrmInfo.ulMaxNumberOfPlayback = pstInfo->ulMaxNumberOfPlayback;
	VA_memcpy(stRecInfo.stDrmInfo.info.stViaccessDrmInfo.szExpireDate, pstInfo->szExpireDate, 11);

	if (SVC_META_SetRecordInfo(ulContentsId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Info("fail to get recordinfo for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

HERROR VA_PVR_Adapt_SetPlaybackInfo(HUINT32 ulAcsId, VA_UI_PLAYBACK_INFO *pstInfo)
{
	Handle_t 			hVaAction;
	SvcMeta_Record_t 	stRecInfo;
	HUINT32			ulContentsId;

	HxLOG_Print("[Enter](%d)\n", ulAcsId);

	hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(ulAcsId);

	if (SVC_META_GetContentIdByhAction(VAMGR_GetActionHandle(hVaAction), &ulContentsId) != ERR_OK)
	{
		HxLOG_Info("fail to get contentid for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	if (SVC_META_GetRecordInfo(ulContentsId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Info("fail to get recordinfo for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

//VA_OS_Printf("** VA_PVR_Adapt_SetPlaybackInfo(%d, %s, %s)\n", pstInfo->ulPlaybackCounter, pstInfo->szRecordDate, pstInfo->szFirstPlaybackDate);
	stRecInfo.stDrmInfo.info.stViaccessDrmInfo.ulPlaybackCounter = pstInfo->ulPlaybackCounter;
	VA_memcpy(stRecInfo.stDrmInfo.info.stViaccessDrmInfo.szFirstPlaybackDate, pstInfo->szFirstPlaybackDate, 20);

	if (SVC_META_SetRecordInfo(ulContentsId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Info("fail to get recordinfo for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR VA_PVR_Adapt_Init()
{
	HERROR nErr;

	s_pstEcmListHead = NULL;
	s_pstEcmListTail = NULL;

	s_ulTsrBufHead = VA_PVR_INVALID_VALUE;
	s_ulTsrBufTail = VA_PVR_INVALID_VALUE;
	s_ulCurEcmIndex = 0;
	s_ulCurEcmValidStart = 0;
	s_ulCurEcmValidEnd = 0;
	s_bIsCurEcmSent = FALSE;

	nErr = VA_SEM_Create(VA_TSR_SEM_NAME, &sVaTsrSemId);
	if (nErr)
	{
		HxLOG_Info("failed to create semaphore (%d)\n", nErr);
		return ERR_FAIL;
	}

	s_pstTsrBuf = (VA_ECM_DATA_t *)VA_MEM_Alloc(VA_PVR_TSR_BUFFER_SIZE * sizeof (VA_ECM_DATA_t));
	if (s_pstTsrBuf == NULL)
		return ERR_FAIL;

	return ERR_OK;
}

static void local_va_pvr_tsr_DeleteEcm(HUINT32 ulTimecode)
{
	VA_SEM_Get(sVaTsrSemId);

	if (s_ulTsrBufTail != s_ulTsrBufHead)
	{
		// 1�� �տ� �ͱ����� ���� �ɷ� ����.
		while (s_pstTsrBuf[s_ulTsrBufTail].ulTimestamp >= ulTimecode - 1 && s_pstTsrBuf[s_ulTsrBufTail].ulTimestamp <= ulTimecode)
		{
			s_ulTsrBufTail = (s_ulTsrBufTail + VA_PVR_TSR_BUFFER_SIZE - 1) % VA_PVR_TSR_BUFFER_SIZE;
		}
	}

	VA_SEM_Release(sVaTsrSemId);
}

// head - (1) - (2) - (3) - ... - tail
// new node �� tail�� ���̴µ�, �Ųٷ� �Ѱǰ�??
static HERROR local_va_pvr_tsr_AddEcm(VA_ECM_CHANNEL_t *pEcmChannel, HUINT32 ulEcmLength, HUINT8 *pucEcm, HUINT32 ulTimestamp)
{
	HUINT32 ulNextNode;

	VA_SEM_Get(sVaTsrSemId);

	if ((s_ulTsrBufTail + 1) % VA_PVR_TSR_BUFFER_SIZE == s_ulTsrBufHead)
	{
		// full - �ǵ� ���� ���쵵�� ����..
		HxLOG_Info("TSR buffer full - delete last one\n");

		s_ulTsrBufHead = (s_ulTsrBufHead + 1) % VA_PVR_TSR_BUFFER_SIZE;
	}

	if (s_ulTsrBufHead == VA_PVR_INVALID_VALUE)
	{
		// empty
		s_ulTsrBufHead = 0;
		s_ulTsrBufTail = 0;
		ulNextNode = 0;
	}
	else
	{
		// ���� �Ͱ� timecode �� pid�� ������ ������� ����..
		//recording pause�� ���� timecode�� ���� ecm�� ��ĥ �� �ִ�
		ulNextNode = (s_ulTsrBufTail + 1) % VA_PVR_TSR_BUFFER_SIZE;
		if (ulTimestamp == s_pstTsrBuf[s_ulTsrBufTail].ulTimestamp &&
			pEcmChannel == s_pstTsrBuf[s_ulTsrBufTail].pEcmChannel)
		{
			// ���� timecode & pid�̴� �����..
			ulNextNode = s_ulTsrBufTail;
		}
		else
		{
			s_ulTsrBufTail = ulNextNode;
		}
	}

	s_pstTsrBuf[ulNextNode].pEcmChannel = pEcmChannel;
	s_pstTsrBuf[ulNextNode].ulEcmLength = ulEcmLength;
	s_pstTsrBuf[ulNextNode].ulTimestamp = ulTimestamp;
	VA_memcpy(s_pstTsrBuf[ulNextNode].aucEcm, pucEcm, ulEcmLength);

	VA_SEM_Release(sVaTsrSemId);
	return ERR_OK;
}

HUINT32 local_va_pvr_tsr_GetValidStart(HUINT32 *pulCurIdx)
{
	HUINT32 ulValidStart, ulValidEnd, ulIndex, ulPrevIndex;

	ulIndex = *pulCurIdx;

	ulValidStart = 0;
	ulValidEnd = s_pstTsrBuf[ulIndex].ulTimestamp;

	// ���ӵ� ��� ECM�� timestamp�� ���� �� �����Ƿ�, �ٸ��� ���ö����� �˻�
	while (ulIndex != s_ulTsrBufHead)
	{
		ulIndex = (ulIndex + VA_PVR_TSR_BUFFER_SIZE - 1) % VA_PVR_TSR_BUFFER_SIZE;
		ulValidStart = s_pstTsrBuf[ulIndex].ulTimestamp;

		if (ulValidStart != ulValidEnd)
			break;
	}

	// �������� ���� ��� �� ���� ecm�� ã�´�.
	while (ulIndex != s_ulTsrBufHead)
	{
		ulPrevIndex = (ulIndex + VA_PVR_TSR_BUFFER_SIZE - 1) % VA_PVR_TSR_BUFFER_SIZE;
		if (s_pstTsrBuf[ulPrevIndex].ulTimestamp != ulValidStart)
			break;

		ulIndex = ulPrevIndex;
	}

	*pulCurIdx = ulIndex;

	return ulValidEnd;
}

HUINT32 local_va_pvr_tsr_GetValidEnd(HUINT32 ulCurIdx)
{
	HUINT32 ulValidStart, ulValidEnd, ulIndex;

	ulIndex = ulCurIdx;

	ulValidStart = s_pstTsrBuf[ulIndex].ulTimestamp;
	ulValidEnd = 0xFFFFFFFF;

	// ���ӵ� ��� ECM�� timestamp�� ���� �� �����Ƿ�, �ٸ��� ���ö����� �˻�
	while (ulIndex != s_ulTsrBufTail)
	{
		ulIndex = (ulIndex + 1) % VA_PVR_TSR_BUFFER_SIZE;
		ulValidEnd = s_pstTsrBuf[ulIndex].ulTimestamp;

		if (ulValidStart != ulValidEnd)
			break;
	}

	return ulValidEnd;
}

HUINT32 local_va_pvr_tsr_GetNextIndex(HUINT32 ulCurIdx)
{
	HUINT32 ulValidStart, ulIndex, ulNextIndex;

	ulIndex = ulCurIdx;

	ulValidStart = s_pstTsrBuf[ulIndex].ulTimestamp;

	// ���ӵ� ��� ECM�� timestamp�� ���� �� �����Ƿ�, �ٸ��� ���ö����� �˻�
	ulNextIndex = ulIndex;

	while (ulNextIndex != s_ulTsrBufTail)
	{
		ulNextIndex = (ulNextIndex + 1) % VA_PVR_TSR_BUFFER_SIZE;
		if (s_pstTsrBuf[ulNextIndex].ulTimestamp != ulValidStart)
		{
			ulIndex = ulNextIndex;
			break;
		}
	}

	return ulIndex;
}

HERROR local_va_pvr_tsr_SendEcm(HUINT32 ulAcsId, HUINT32 ulCurrentTime)
{
	HUINT32 ulNextIndex;
	HUINT32 ulNextEcmValidStart = VA_PVR_INVALID_VALUE;
	HUINT32 ulNextEcmValidEnd = VA_PVR_INVALID_VALUE;
	HBOOL bSendEcm = FALSE;
	HERROR nErr;

	VA_SEM_Get(sVaTsrSemId);

	if (s_ulTsrBufHead == VA_PVR_INVALID_VALUE)
	{
		// tsr buffer�� empty
		HxLOG_Info("TSR buffer empty\n");
		VA_SEM_Release(sVaTsrSemId);
		return ERR_FAIL;
	}
	else
	{
		if (s_ulCurEcmValidStart == 0 && s_ulCurEcmValidEnd == 0)
		{
			// valid ���� ���� ����� ����������.
			s_ulCurEcmValidStart = s_pstTsrBuf[s_ulTsrBufHead].ulTimestamp;
			s_ulCurEcmValidEnd = local_va_pvr_tsr_GetValidEnd(s_ulTsrBufHead);
		}

		if (s_ulCurEcmValidStart > ulCurrentTime)
		{
			// ���� ecm���� ���� ecm�� ��� �Ѵ�

			ulNextIndex = s_ulCurEcmIndex;

			while (1)
			{
				if (ulNextIndex == s_ulTsrBufHead)
				{
					//���� ecm�� �Ǿ��̴�...  ���� �Ⱥ��´ٸ� ecm�� ��������.
					if (ulNextIndex != s_ulCurEcmIndex)
					{
						s_ulCurEcmIndex = ulNextIndex;

						s_ulCurEcmValidStart = s_pstTsrBuf[ulNextIndex].ulTimestamp;
						s_ulCurEcmValidEnd = local_va_pvr_tsr_GetValidEnd(ulNextIndex);
						bSendEcm = TRUE;
					}
					else if (s_bIsCurEcmSent == FALSE)
					{
						bSendEcm = TRUE;
					}
					break;
				}
				else
				{
					ulNextEcmValidEnd = s_pstTsrBuf[ulNextIndex].ulTimestamp;
					ulNextEcmValidStart = local_va_pvr_tsr_GetValidStart(&ulNextIndex);

					if (ulNextEcmValidStart <= ulCurrentTime && ulNextEcmValidEnd > ulCurrentTime)
					{
						// �̰� ������..
						s_ulCurEcmIndex = ulNextIndex;
						s_ulCurEcmValidStart = ulNextEcmValidStart;
						s_ulCurEcmValidEnd = ulNextEcmValidEnd;
						bSendEcm = TRUE;
						break;
					}
				}
			}
		}
		else if (s_ulCurEcmValidEnd <= ulCurrentTime)
		{
			// ���� ecm���� ���� ecm�� ��� �Ѵ�

			ulNextIndex = s_ulCurEcmIndex;

			while (1)
			{
				if (ulNextIndex == s_ulTsrBufTail)
				{
					//���� ecm�� �ǵ��̴�...  ���� �Ⱥ��´ٸ� ecm�� ��������.
					if (ulNextIndex != s_ulCurEcmIndex)
					{
						s_ulCurEcmIndex = ulNextIndex;

						s_ulCurEcmValidStart = s_pstTsrBuf[ulNextIndex].ulTimestamp;
						s_ulCurEcmValidEnd = 0xFFFFFFFF;
						bSendEcm = TRUE;
					}
					else if (s_bIsCurEcmSent == FALSE)
					{
						bSendEcm = TRUE;
					}
					break;
				}
				else
				{
					ulNextIndex = local_va_pvr_tsr_GetNextIndex(ulNextIndex);

					ulNextEcmValidStart = s_pstTsrBuf[ulNextIndex].ulTimestamp;

					ulNextEcmValidEnd = local_va_pvr_tsr_GetValidEnd(ulNextIndex);

					if (ulNextEcmValidStart <= ulCurrentTime && ulNextEcmValidEnd > ulCurrentTime)
					{
						// �̰� ������..
						s_ulCurEcmIndex = ulNextIndex;
						s_ulCurEcmValidStart = ulNextEcmValidStart;
						s_ulCurEcmValidEnd = ulNextEcmValidEnd;
						bSendEcm = TRUE;
						break;
					}
				}
			}
		}
		else
		{
			// ���� ecm �����̰�, ���� �Ⱥ��´ٸ� ecm�� �����Ѵ�.
			if (s_bIsCurEcmSent == FALSE)
			{
				bSendEcm = TRUE;
			}
		}

		if (bSendEcm)
		{
			//## ecm ������..
			nErr = VA_DMX_PutEcmFromPVR(ulAcsId, s_pstTsrBuf[s_ulCurEcmIndex].aucEcm, s_pstTsrBuf[s_ulCurEcmIndex].pEcmChannel->usPid);
			if (nErr == ERR_OK)
				s_bIsCurEcmSent = TRUE;

			// ���� timestamp�� �͵��� �� ����������..
			ulNextIndex = s_ulCurEcmIndex;
			while (ulNextIndex != s_ulTsrBufTail)
			{
				ulNextIndex = (ulNextIndex + 1) % VA_PVR_TSR_BUFFER_SIZE;
				if (s_pstTsrBuf[ulNextIndex].ulTimestamp != s_pstTsrBuf[s_ulCurEcmIndex].ulTimestamp)
					break;

				nErr = VA_DMX_PutEcmFromPVR(ulAcsId, s_pstTsrBuf[ulNextIndex].aucEcm, s_pstTsrBuf[ulNextIndex].pEcmChannel->usPid);
				if (nErr == ERR_OK)
					s_bIsCurEcmSent = TRUE;
			}
		}
	}

	VA_SEM_Release(sVaTsrSemId);
	return ERR_OK;
}

HUINT32 local_va_pvr_pb_DeleteEcm(HUINT32 ulTimecode)
{
	VA_ECM_LIST_t *pTemp, *pGroup, *pGroupNext;

	if (s_pstEcmListTail != NULL)
	{
		// 1�� �� �ͱ����� ���� �ɷ� ����.
		if (s_pstEcmListTail->ulValidStart >= ulTimecode - 1 && s_pstEcmListTail->ulValidStart <= ulTimecode)
		{
			pGroup = s_pstEcmListTail->pGroup;

			// ���� group�� ecm�� ó��
			while (pGroup)
			{
				pGroupNext = pGroup->pGroup;

				VA_MEM_Free(pGroup);

				pGroup = pGroupNext;
			}

			if (s_pstEcmListTail->pPrev)
				s_pstEcmListTail->pPrev->pNext = NULL;

			pTemp = s_pstEcmListTail;
			s_pstEcmListTail= s_pstEcmListTail->pPrev;

			VA_MEM_Free(pTemp);
		}
	}
}

HUINT32 local_va_pvr_pb_AddEcm(HUINT8 *pData)
{
	VA_ECM_LIST_t *pEcmList, *pNext, *pGroupNext;
	HUINT32 ulLen = 0;

	pEcmList = (VA_ECM_LIST_t *)VA_MEM_Alloc(sizeof(VA_ECM_LIST_t));

	if (pEcmList == NULL)
	{
		return 0;
	}

	// time stamp 			: 4
	// ecm pid 			: 2
	// ecm length 			: 2
	// ecm data 			: max  1024

	pEcmList->ulValidStart = Get32bit(pData);
	pEcmList->ulValidEnd = 0xFFFFFFFF;
	pEcmList->usPid =Get16bit(pData + 4);
	pEcmList->usEcmLength = Get16bit(pData + 6);

	if (pEcmList->usPid == 0xFFFF && pEcmList->usEcmLength == 0)
	{
		// �����
		local_va_pvr_pb_DeleteEcm(pEcmList->ulValidStart);
		ulLen = pEcmList->usEcmLength + 8;
		if(pEcmList != NULL)
		{
			VA_MEM_Free(pEcmList);
		}
	}
	else
	{
		VA_memcpy(pEcmList->aucEcm, pData + 8, pEcmList->usEcmLength);

		if (s_pstEcmListTail != NULL && s_pstEcmListTail->ulValidStart == pEcmList->ulValidStart)
		{
			// �ð��� ������ group�� �����.... pid�� �ٸ� ecm���� ���� ó���Ϸ���....
			if (s_pstEcmListTail->usPid == pEcmList->usPid)
			{
				// ���� pid�� ���� ���� �����.. (recording pause�� ���Ͻð��� ���� ecm ���� ���ó��)
				s_pstEcmListTail->usEcmLength = pEcmList->usEcmLength;
				VA_memcpy(s_pstEcmListTail->aucEcm, pEcmList->aucEcm, pEcmList->usEcmLength);

				VA_MEM_Free(pEcmList);

				// �Ʒ� return ��ó�����ؼ�..
				pEcmList = s_pstEcmListTail;
			}
			else
			{
				// pid�� �ٸ��� group���� ó��
				pEcmList->pPrev = NULL;
				pEcmList->pNext = NULL;
				if (s_pstEcmListTail->pGroup)
				{
					pEcmList->pGroup = s_pstEcmListTail->pGroup;
				}
				else
				{
					pEcmList->pGroup = NULL;
				}

				s_pstEcmListTail->pGroup = pEcmList;
			}
		}
		else
		{
			pEcmList->pPrev = s_pstEcmListTail;
			pEcmList->pNext = NULL;
			pEcmList->pGroup = NULL;
			s_pstEcmListTail = pEcmList;

			if (s_pstEcmListHead == NULL)
				s_pstEcmListHead = pEcmList;

			if (pEcmList->pPrev)
			{
				pEcmList->pPrev->pNext = pEcmList;
				pEcmList->pPrev->ulValidEnd = pEcmList->ulValidStart;
			}
		}
		if(pEcmList != NULL)
		{
			ulLen = pEcmList->usEcmLength + 8;
		}
	}

	return ulLen; // length of ecm packet
}

// DMX ��⿡�� ECM filter�� setting�Ǿ��� �� �ش� PID�� �´� ECM�� ������ �ٷ� �޶�� ��û�� �� ����
// �׿� ���� ó���� �Ʒ� �Լ����� ��
HERROR VA_SvcPb_SendEcmByPid(HUINT32 ulAcsId, HUINT16 usPid)
{
	HBOOL bSendEcm = FALSE;
	VA_ECM_LIST_t *pNext, *pGroup;
	HERROR nErr;
	VA_PVR_FILE_t *pFile;

	if (VA_PI_Adapt_IsProgramSwitchOn(VA_PI_Adapt_GetVaActionHandleByAcsId(ulAcsId)) == FALSE)
	{
		HxLOG_Info("not switch on(acs:%x)\n", ulAcsId);
		return ERR_FAIL;
	}

	pFile = VA_PVR_GetFileInfo(ulAcsId);

	if (pFile == NULL)
	{
		HxLOG_Info("pFile = NULL\n");
		return ERR_FAIL;
	}

	VA_SEM_Get(pFile->ulSemId);

	if (s_pstEcmListHead == NULL)
	{
		// ecm buffer�� empty
		HxLOG_Info("REC buffer empty\n");
		VA_SEM_Release(pFile->ulSemId);
		return ERR_FAIL;
	}
	else
	{
		// ������ġ Ȥ�� �� ���� ��ġ���� ���� pid�� �ִ��� ���캻��..
		if (s_bIsCurEcmSent)
			pNext = s_pstCurrentEcm->pNext;
		else
			pNext = s_pstCurrentEcm;

		while (pNext != NULL)
		{
			if (pNext->usPid == usPid)
			{
				s_pstCurrentEcm = pNext;
				bSendEcm = TRUE;
			}
			else
			{
				pGroup = pNext->pGroup;

				while (pGroup != NULL)
				{
					if (pGroup->usPid == usPid)
					{
						s_pstCurrentEcm = pNext;
						bSendEcm = TRUE;
						break;
					}

					pGroup = pGroup->pGroup;
				}
			}

			if (bSendEcm)
				break;

			pNext = pNext->pNext;
		}

		if (bSendEcm)
		{
			//## ecm ������..
			nErr = VA_DMX_PutEcmFromPVR(ulAcsId, s_pstCurrentEcm->aucEcm, s_pstCurrentEcm->usPid);
			if (nErr == ERR_OK)
				s_bIsCurEcmSent = TRUE;

			pNext = s_pstCurrentEcm;

			while (pNext->pGroup)
			{
				pNext = pNext->pGroup;
				nErr = VA_DMX_PutEcmFromPVR(ulAcsId, pNext->aucEcm, pNext->usPid);
				if (nErr == ERR_OK)
					s_bIsCurEcmSent = TRUE;
			}

			// �Ʒ��� ������ ST chip���� �߻��ϴ� ������ �����ϱ� ���� ��.. �ٸ� platform�� �� �ٸ� �� ����..
			// ó������ Ʋ�������� ������ time stamp = 0 �ΰ��� �Ѿ�ô�.. �߰��� ���۵� ��� �� ����� �������.
			// 1�� ������.. sync�� �ɸ� �ȸ��� ��찡 �־��..
			if (pFile->ulTimestamp)
			{
				pFile->nTimeOffset = s_pstCurrentEcm->ulValidStart - pFile->ulTimestamp - 1;

				// 3 �̻� Ʋ������ ���ϰ� ����
				if (pFile->nTimeOffset > 3)
					pFile->nTimeOffset = 3;
			}
		}
	}

	VA_SEM_Release(pFile->ulSemId);
	return ERR_OK;
}

HERROR local_va_pvr_pb_SendEcm(HUINT32 ulAcsId, HUINT32 ulCurrentTime)
{
	HBOOL bSendEcm = FALSE;
	VA_ECM_LIST_t *pNext;
	HERROR nErr;
	VA_PVR_FILE_t *pFile;

	HxLOG_Print("[Enter(%d)]\n", ulAcsId);
	pFile = VA_PVR_GetFileInfo(ulAcsId);

	if (pFile)
	{
		VA_SEM_Get(pFile->ulSemId);
	}

	if (s_pstEcmListHead == NULL)
	{
		// ecm buffer�� empty
		HxLOG_Info("REC buffer empty\n");
		if (pFile)
		{
			VA_SEM_Release(pFile->ulSemId);
		}
		return ERR_FAIL;
	}
	else
	{
		if (s_pstCurrentEcm->ulValidStart > ulCurrentTime)
		{
			// ���� ecm���� ���� ecm�� ��� �Ѵ�

			pNext = s_pstCurrentEcm;

			while (1)
			{
				if (pNext == s_pstEcmListHead)
				{
					//���� ecm�� �Ǿ��̴�...  ���� �Ⱥ��´ٸ� ecm�� ��������.
					if (pNext != s_pstCurrentEcm)
					{
						s_pstCurrentEcm = pNext;
						bSendEcm = TRUE;
					}
					else if (s_bIsCurEcmSent == FALSE)
					{
						bSendEcm = TRUE;
					}

					break;
				}
				else
				{
					pNext = pNext->pPrev;

					if (pNext->ulValidStart <= ulCurrentTime && pNext->ulValidEnd > ulCurrentTime)
					{
						// �̰� ������..
						s_pstCurrentEcm = pNext;
						bSendEcm = TRUE;
						break;
					}
				}
			}
		}
		else if (s_pstCurrentEcm->ulValidEnd <= ulCurrentTime)
		{
			// ���� ecm���� ���� ecm�� ��� �Ѵ�

			pNext = s_pstCurrentEcm;

			while (1)
			{
				if (pNext == s_pstEcmListTail)
				{
					//���� ecm�� �ǵ��̴�...  ���� �Ⱥ��´ٸ� ecm�� ��������.
					if (pNext != s_pstCurrentEcm)
					{
						s_pstCurrentEcm = pNext;
					bSendEcm = TRUE;
					}
					else if (s_bIsCurEcmSent == FALSE)
					{
						bSendEcm = TRUE;
					}

					break;
				}
				else
				{
					pNext = pNext->pNext;

					if (pNext->ulValidStart <= ulCurrentTime && pNext->ulValidEnd > ulCurrentTime)
					{
						// �̰� ������..
					s_pstCurrentEcm = pNext;
						bSendEcm = TRUE;
						break;
					}
				}
			}
		}
		else
		{
		// ���� ecm �����̰�, ���� �Ⱥ��´ٸ� ecm�� �����Ѵ�.
			if (s_bIsCurEcmSent == FALSE)
			{
				bSendEcm = TRUE;
			}
		}

		if (bSendEcm)
		{
			//## ecm ������..
			nErr = VA_DMX_PutEcmFromPVR(ulAcsId, s_pstCurrentEcm->aucEcm, s_pstCurrentEcm->usPid);
			if (nErr == ERR_OK)
			{
				s_bIsCurEcmSent = TRUE;

				HxLOG_Info("[send ECM:acs:%x, time:%d, pid:%x, ecm_len:%d]\n", ulAcsId, s_pstCurrentEcm->ulValidStart, s_pstCurrentEcm->usPid, s_pstCurrentEcm->usEcmLength);
			}
			pNext = s_pstCurrentEcm;

			while (pNext->pGroup)
			{
				pNext = pNext->pGroup;
				nErr = VA_DMX_PutEcmFromPVR(ulAcsId, pNext->aucEcm, pNext->usPid);
				if (nErr == ERR_OK)
				{
					s_bIsCurEcmSent = TRUE;
					HxLOG_Info("[send ECM:acs:%x, time:%d, pid:%x, ecm_len:%d]\n", ulAcsId, pNext->ulValidStart, pNext->usPid, pNext->usEcmLength);
				}
			}
		}
	}

	if (pFile)
	{
		VA_SEM_Release(pFile->ulSemId);
	}
	return ERR_OK;
}

void local_va_pvr_pb_DeleteEcmAll()
{
	VA_ECM_LIST_t *pNext, *pTemp, *pGroup, *pGroupNext;

	pTemp = s_pstEcmListHead;

	s_pstEcmListHead = NULL;
	s_pstEcmListTail = NULL;

	while (pTemp)
	{
		pNext = pTemp->pNext;
		pGroup = pTemp->pGroup;

		VA_MEM_Free(pTemp);
		pTemp = pNext;

		// ���� group�� ecm�� ó��
		while (pGroup)
		{
			pGroupNext = pGroup->pGroup;

			VA_MEM_Free(pGroup);

			pGroup = pGroupNext;
		}
	}
}

HERROR local_va_pvr_pb_ParseEcmFile(HUINT32 ulFileHandle)
{
	int idx = 0, nLen;
	HUINT32 ulRead;

	if (s_pucPbFileBuffer)
	{
		VA_MEM_Free(s_pucPbFileBuffer);
	}

	s_pucPbFileBuffer = (HUINT8 *)VA_MEM_Alloc(VA_PVR_PB_BUFFER_SIZE);

	if (s_pucPbFileBuffer == NULL)
	{
		HxLOG_Info("fail to alloc buffer (size:%d)\n", VA_PVR_PB_BUFFER_SIZE);
		return ERR_FAIL;
	}

	idx = 0;

	do
	{
		ulRead = HLIB_FILE_Read((HxFILE_t)ulFileHandle, s_pucPbFileBuffer + idx, 1, VA_PVR_PB_BUFFER_SIZE - idx);
		if (ulRead <= 0)
		{
			HxLOG_Info("fail to read ecm file\n");
			VA_MEM_Free(s_pucPbFileBuffer);
			s_pucPbFileBuffer = NULL;
			return ERR_FAIL;
		}

		ulRead += idx;
		idx = 0;

		while (idx < ulRead)
		{
			if (ulRead - idx < 8 || ulRead - idx < Get16bit(s_pucPbFileBuffer + idx + 6) + 8)
			{
				// buffer ������ �߷��ִ�. ������ �� �о�;� ��.
				VA_memcpy(s_pucPbFileBuffer, s_pucPbFileBuffer + idx, ulRead - idx);
				idx = ulRead - idx;
				break;
			}

			nLen = local_va_pvr_pb_AddEcm(s_pucPbFileBuffer + idx);
			if (nLen == 0)
			{
				// ���� �׸�����..
				HxLOG_Info("fail to add ecm\n");
				VA_MEM_Free(s_pucPbFileBuffer);
				s_pucPbFileBuffer = NULL;
				return ERR_FAIL;
			}

			idx += nLen;
		}
	}while (ulRead == VA_PVR_PB_BUFFER_SIZE);

	s_pstCurrentEcm = s_pstEcmListHead;
	s_bIsCurEcmSent = FALSE;

	return ERR_OK;
}

HERROR VA_SvcPb_OpenFile(Handle_t hVaAction)
{
	SvcMeta_Record_t 	stRecInfo;
	HCHAR 				szFileName[VA_PVR_MAX_FILE_NAME_LENGTH + 16];
	HUINT32 			ulFileHandle, ulContentsId;
	VA_PVR_FILE_t *pFile;
	HxFILE_t		hFile;

	HxLOG_Print("[Enter]\n");

	if (SVC_META_GetContentIdByhAction(VAMGR_GetActionHandle(hVaAction), &ulContentsId) != ERR_OK)
	{
		HxLOG_Info("fail to get contentid for action(%x)\n", hVaAction);
		return ERR_FAIL;
	}

	pFile = VA_PVR_GetPbFileInfo(hVaAction, ulContentsId, TRUE);
	if (pFile == NULL)
	{
		HxLOG_Info("fail to get file info for action (%x)\n", hVaAction);
		return ERR_FAIL;
	}

	VA_SEM_Get(pFile->ulSemId);

	if (pFile->bIsTimeshift)
	{
		// timeshift�� ���� ��������??
		s_bIsCurEcmSent = FALSE;
	}
	else
	{
		if (SVC_META_GetRecordInfo(ulContentsId, &stRecInfo) != ERR_OK)
		{
			HxLOG_Info("fail to get recordinfo for acs (%x)\n", hVaAction);
			VA_SEM_Release(pFile->ulSemId);
			return ERR_FAIL;
		}

		snprintf(szFileName, VA_PVR_MAX_FILE_NAME_LENGTH + 16, "%s%s%s", stRecInfo.aucPath, stRecInfo.aucFileName, EXT_VA_PVR_ECM_FILE);

		if (pFile->bRecord)
		{
			// ���� ��ȭ ���� ���. �ݾҴٰ� �ٽ� ����..

			if (HLIB_FILE_Close((HxFILE_t)pFile->ulFileHandle) != ERR_OK)
			{
				HxLOG_Info("fail to close file for acs (%x)\n", pFile->ulAcsId);
				VA_SEM_Release(pFile->ulSemId);
				return ERR_FAIL;
			}

			if (HLIB_FILE_Exist(szFileName) == TRUE)
			{
			 	hFile = HLIB_FILE_Open(szFileName, "r+");
			}
			else
			{
				hFile = HLIB_FILE_Open(szFileName, "w+");
			}

			if(hFile == NULL)
			{
				HxLOG_Info(" fail to open file for acs (%d)\n", pFile->ulAcsId);
				VA_SEM_Release(pFile->ulSemId);
				return ERR_FAIL;
			}
			pFile->ulFileHandle = (HUINT32)hFile;

			if (local_va_pvr_pb_ParseEcmFile(pFile->ulFileHandle) != ERR_OK)
			{
				HLIB_FILE_Close((HxFILE_t)pFile->ulFileHandle);
				HxLOG_Info("fail to parse ecm file (%x)\n", pFile->ulAcsId);
				VA_SEM_Release(pFile->ulSemId);
				return ERR_FAIL;
			}
		}
		else
		{
			// �̹� ��ȭ �Ϸ�� ���
			if (HLIB_FILE_Exist(szFileName) == TRUE)
			{
			 	hFile = HLIB_FILE_Open(szFileName, "r+");
			}
			else
			{
				hFile = HLIB_FILE_Open(szFileName, "w+");
			}

			if(hFile == NULL)
			{
				HxLOG_Info("fail to open file for acs (%x)\n", hVaAction);
				VA_SEM_Release(pFile->ulSemId);
				return ERR_FAIL;
			}
			ulFileHandle = (HUINT32)hFile;

			if (local_va_pvr_pb_ParseEcmFile(ulFileHandle) != ERR_OK)
			{
				HLIB_FILE_Close((HxFILE_t)ulFileHandle);
				HxLOG_Info("fail to parse ecm file (%x)\n", hVaAction);
				VA_SEM_Release(pFile->ulSemId);
				return ERR_FAIL;
			}

			if (HLIB_FILE_Close((HxFILE_t)ulFileHandle) != ERR_OK)
			{
				HxLOG_Info("fail to close file for acs (%x)\n", hVaAction);
				VA_SEM_Release(pFile->ulSemId);
				return ERR_FAIL;
			}
			pFile->eStatus = eVaPvrFileStatus_Open;
		}
	}

	pFile->hVaActionPb = hVaAction;

	VA_SEM_Release(pFile->ulSemId);
	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR VA_SvcPb_CloseFile(Handle_t hVaAction)
{
	HUINT32 ulContentsId;
	VA_PVR_FILE_t *pFile;

	HxLOG_Print("[Enter]\n");

	pFile = VA_PVR_GetPbFileInfoByVaActionHandle(hVaAction);
	if (pFile == NULL)
	{
		HxLOG_Info("fail to get file info for action (%x)\n", hVaAction);
		return ERR_FAIL;
	}

	VA_SEM_Get(pFile->ulSemId);

	if (pFile->bIsTimeshift)
	{
		// timeshift�� ���� ��������??
	}
	else
	{
		if (s_pucPbFileBuffer)
		{
			VA_MEM_Free(s_pucPbFileBuffer);
			s_pucPbFileBuffer = NULL;
		}

		local_va_pvr_pb_DeleteEcmAll();
	}

	if (pFile->bRecord== FALSE)
	{
		// ��ȭ�� ���� ��쿡�� �ݴ´�, chase playback�� ��ȭ�ʿ��� close�� ����
		pFile->eStatus = eVaPvrFileStatus_Close;
	}

	pFile->hVaActionPb = HANDLE_NULL;

	VA_SEM_Release(pFile->ulSemId);

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

void VA_SvcPb_ForceSetTimecode(HUINT32 ulAcsId)
{
	Handle_t hVaAction;

	hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(ulAcsId);

	s_bIsCurEcmSent = FALSE;
	VA_PVR_SetTimestamp(hVaAction, s_ulCurrentTime);
}

void VA_SvcPb_TimecodeChange(Handle_t hAction, HUINT32 ulCurrentTime)
{
	Handle_t hVaAction;
	HUINT32 ulAcsId;

HxLOG_Print("[Enter(%d)]\n", ulCurrentTime);

	//  hAction�� octo�� �ڵ��̴� hVaAction���� �ٲ����..
	hVaAction = VAMGR_GetVaHandle(hAction);

	if (hVaAction == VA_INVALID_HANDLE)
	{
		HxLOG_Info("invalid handle(%x,%x)\n", hAction, hVaAction);
		return;
	}


	VA_PVR_SetTimestamp(hVaAction, ulCurrentTime);
HxLOG_Print("[Exit]\n");
}

void VA_SvcPb_SendEcm(Handle_t hVaAction, HUINT32 ulCurrentTime)
{
	HUINT32 ulAcsId;
	HINT32 nAdjustedTime;
	VA_PVR_FILE_t *pFile;

	HxLOG_Print("[Enter(%d)]\n", hVaAction);

	if (VA_PI_Adapt_IsProgramSwitchOn(hVaAction) == FALSE)
	{
		HxLOG_Info("not switch on(%x)\n", hVaAction);
		return;
	}

	ulAcsId = VA_PI_Adapt_GetAcsIdByVaActionHandle(hVaAction);

	if (ulAcsId != VA_INVALID_HANDLE)
	{
		nAdjustedTime = (HINT32)ulCurrentTime;

		// ECM filter �ɸ��� ������ �������� timestamp ����
		pFile = VA_PVR_GetFileInfo(ulAcsId);
		if (pFile)
		{
			nAdjustedTime += pFile->nTimeOffset;
			if (nAdjustedTime < 0)
				nAdjustedTime = 0;

			pFile->ulTimestamp = ulCurrentTime;
		}

		if (VAMGR_GetActType(hVaAction) == eCAS_ActionType_Tsr_Play)
		{
			// tsr�� ���
			local_va_pvr_tsr_SendEcm(ulAcsId, nAdjustedTime);
		}
		else
		{
			// rec�� ���
			local_va_pvr_pb_SendEcm(ulAcsId, nAdjustedTime);
		}

		s_ulCurrentTime = nAdjustedTime;
	}
	HxLOG_Print("[Exit]\n");
}

HUINT32 VA_PVR_Rec_GetTimecode(HUINT32 ulAcsId)
{
	VA_PVR_FILE_t *pFile;

HxLOG_Print("[Enter(%d)]\n", ulAcsId);

	pFile = VA_PVR_GetFileInfo(ulAcsId);
	if (pFile != NULL)
	{
		if (pFile->ulTimestamp)
			return pFile->ulTimestamp;
	}

HxLOG_Print("[Exit]\n");
	return 0;
}

void VA_PVR_Rec_TimecodeChange(Handle_t hAction, HUINT32 ulCurrentTime)
{
	Handle_t hVaAction;
	HUINT32 ulAcsId;
	VA_PVR_FILE_t *pFile;

HxLOG_Print("[Enter(%d)]\n", ulCurrentTime);

	//  hAction�� octo�� �ڵ��̴� hVaAction���� �ٲ����..
	hVaAction = VAMGR_GetVaHandle(hAction);

	ulAcsId = VA_PI_Adapt_GetAcsIdByVaActionHandle(hVaAction);

	if (ulAcsId != VA_INVALID_HANDLE)
	{
		pFile = VA_PVR_GetFileInfo(ulAcsId);
		if (pFile != NULL)
		{
			pFile->ulTimestamp = ulCurrentTime;
		}
	}

HxLOG_Print("[Exit]\n");
}

HERROR VA_PVR_Rec_OpenFile(VA_PVR_FILE_t *pFile, HUINT32 ulRetryCnt)
{
	Handle_t 			hVaAction;
	SvcMeta_Record_t 	stRecInfo;
	HCHAR 				szFileName[VA_PVR_MAX_FILE_NAME_LENGTH + 16];

	HxFILE_t		hFile;

	HxLOG_Print("[Enter](%d,%d,%d)\n", pFile->ulAcsId, pFile->eStatus, ulRetryCnt);

	if (pFile->eStatus != eVaPvrFileStatus_FileOpen)
	{
		HxLOG_Info("wrong file status(%d)\n", pFile->eStatus);
		return ERR_FAIL;
	}
	hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(pFile->ulAcsId);

	if (SVC_META_GetContentIdByhAction(VAMGR_GetActionHandle(hVaAction), &pFile->ulContentsId) != ERR_OK)
	{
		HxLOG_Info("fail to get contentid for acs (%d)\n", pFile->ulAcsId);

		if (ulRetryCnt < VA_PVR_ECM_FILE_OPEN_MAX_RETRY)
		{
			VA_PVR_OpenEcmFileRetry(pFile, ulRetryCnt + 1);
		}
		return ERR_FAIL;
	}

	if (SVC_META_GetRecordInfo(pFile->ulContentsId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Info("fail to get recordinfo for acs (%d)\n", pFile->ulAcsId);
		return ERR_FAIL;
	}

#if 1
	// humax meta file�� scramble flag set�Ѵ�. ���߿� playback�� ���� cas�� enable��Ű����...
	do
	{
//VA_OS_Printf("\n*********** SET u1cScrambled ************\n\n");
		stRecInfo.u1cScrambled = TRUE;

		if (SVC_META_SetRecordInfo(pFile->ulContentsId, &stRecInfo) != ERR_OK)
		{
			HxLOG_Info("fail to get recordinfo for acs (%d)\n", pFile->ulAcsId);
			return ERR_FAIL;
		}

		stRecInfo.u1cScrambled = FALSE;
		SVC_META_GetRecordInfo(pFile->ulContentsId, &stRecInfo);
	} while (stRecInfo.u1cScrambled != TRUE);
#endif

	VA_SEM_Get(pFile->ulSemId);

	if (pFile->bIsTimeshift)
	{
		// timeshift�� ���� ������ ���� ���Դϴ�.
	}
	else
	{
		// rec�� ���, ���� ������ �������ش�.

		snprintf(szFileName, VA_PVR_MAX_FILE_NAME_LENGTH + 16, "%s%s%s", stRecInfo.aucPath, stRecInfo.aucFileName, EXT_VA_PVR_ECM_FILE);

		if (HLIB_FILE_Exist(szFileName) == TRUE)
		{
			hFile = HLIB_FILE_Open(szFileName, "r+");
		}
		else
		{
			hFile = HLIB_FILE_Open(szFileName, "w+");
		}
		if(hFile == NULL)
		{
			HxLOG_Info(" fail to open file for acs (%d)\n", pFile->ulAcsId);
			VA_SEM_Release(pFile->ulSemId);
			return ERR_FAIL;
		}
		pFile->ulFileHandle = (HUINT32)hFile;
	}

	pFile->eStatus = eVaPvrFileStatus_Open;

	VA_SEM_Release(pFile->ulSemId);
	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR VA_PVR_Rec_CloseFile(VA_PVR_FILE_t *pFile)
{
	Handle_t 			hVaAction;
	SvcMeta_Record_t 	stRecInfo;

	HxLOG_Print("[Enter](%d, %d,%d)\n", pFile->bIsTimeshift, pFile->ulAcsId, pFile->eStatus);

	if (pFile == NULL || pFile->eStatus != eVaPvrFileStatus_FileClose)
	{
		HxLOG_Info("pFile is null or wrong status\n");
		return ERR_FAIL;
	}

	VA_SEM_Get(pFile->ulSemId);

	if (pFile->bIsTimeshift)
	{
		// timeshift�� ���, buffer�� Ŭ���� �����ش�.
		VA_SEM_Get(sVaTsrSemId);
		s_ulTsrBufHead = VA_PVR_INVALID_VALUE;
		s_ulTsrBufTail = VA_PVR_INVALID_VALUE;

		s_ulCurEcmIndex = 0;
		s_ulCurEcmValidStart = 0;
		s_ulCurEcmValidEnd = 0;
		s_bIsCurEcmSent = FALSE;
		VA_SEM_Release(sVaTsrSemId);
	}
	else
	{
		// rec�� ���, ���� ������ �������ش�.
		if (HLIB_FILE_Close((HxFILE_t)pFile->ulFileHandle) != ERR_OK)
		{
			HxLOG_Info("fail to close file\n");
			VA_SEM_Release(pFile->ulSemId);
			return ERR_FAIL;
		}
	}

	pFile->eStatus = eVaPvrFileStatus_Close;

	VA_SEM_Release(pFile->ulSemId);
	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR VA_PVR_Rec_DeleteEcm(HUINT32 ulAcsId, HBOOL bBlock)
{
	VA_PVR_FILE_t *pFile;
	HUINT32 ulWritten, ulCurTime, ulEcmTime;
	HUINT8 aucVaEcmWriteBuffer[kVA_PVR_ECM_MAX_SIZE + 64];
	int nOffset;

	HxLOG_Print("[Enter]\n");

	pFile = VA_PVR_GetFileInfo(ulAcsId);

	if (pFile == NULL || pFile->eStatus != eVaPvrFileStatus_Open)
	{
		HxLOG_Info("pFile is null or wrong status (%x, %d)\n", pFile, pFile==NULL?0:pFile->eStatus);
		return ERR_FAIL;
	}

	VA_SEM_Get(pFile->ulSemId);

	pFile->bBlocked = bBlock;

	if (bBlock == TRUE)
	{
		ulEcmTime = pFile->ulTimestamp;


		if (pFile->bIsTimeshift)
		{
			// timeshift�� ���,
			local_va_pvr_tsr_DeleteEcm(ulEcmTime);
		}
		else
		{
			// rec�ΰ��,
			// time stamp 			: 4
			// ecm pid 			: 2
			// ecm length 			: 2
			// ecm data 			: max  1024

			nOffset = 0;

			// time stamp
			Put32bit(aucVaEcmWriteBuffer + nOffset, ulEcmTime);
			nOffset += 4;

			// ecm pid
			Put16bit(aucVaEcmWriteBuffer + nOffset, 0xFFFF);
			nOffset += 2;

			// ecm length
			Put16bit(aucVaEcmWriteBuffer + nOffset, 0);
			nOffset += 2;

			ulWritten = HLIB_FILE_Write((HxFILE_t)pFile->ulFileHandle, aucVaEcmWriteBuffer, 1, nOffset);
			if (ulWritten <= 0)
			{
				HxLOG_Info("fail to write\n");
				VA_SEM_Release(pFile->ulSemId);
				return ERR_FAIL;
			}

			if (nOffset != ulWritten)
			{
				HxLOG_Info("wrong written data (%d,%d)\n", nOffset, ulWritten);
				VA_SEM_Release(pFile->ulSemId);
				return ERR_FAIL;
			}

			// pb�� �ɷ������� (chase play)
			if (pFile->hVaActionPb != HANDLE_NULL)
			{
				local_va_pvr_pb_AddEcm(aucVaEcmWriteBuffer);
			}
		}
	}

	VA_SEM_Release(pFile->ulSemId);
	HxLOG_Print("[Exit]\n");
}

HERROR VA_PVR_Rec_RecordEcm(VA_ECM_CHANNEL_t *pEcmChannel, HUINT32 ulEcmLength, HUINT8 *pucEcm, HUINT32 ulTimestamp)
{
	VA_PVR_FILE_t *pFile;
	HUINT32 ulWritten, ulCurTime, ulEcmTime;
	HUINT8 aucVaEcmWriteBuffer[kVA_PVR_ECM_MAX_SIZE + 64];
	int nOffset;

	HxLOG_Print("[Enter(%d, %02X %02X %02X]\n", ulEcmLength, pucEcm[0], pucEcm[1], pucEcm[2]);

	if (pEcmChannel == NULL)
	{
		HxLOG_Info("pEcmChannel is null\n");
		return ERR_FAIL;
	}

	pFile = pEcmChannel->pFile;

	if (pFile == NULL || pFile->eStatus != eVaPvrFileStatus_Open)
	{
		HxLOG_Info("pFile is null or wrong status (%x, %d)\n", pFile, pFile==NULL?0:pFile->eStatus);
		return ERR_FAIL;
	}

	if (pFile->bBlocked)
	{
		HxLOG_Info("ECM recording is blocked (acs:%d, %x)\n", pFile->ulAcsId, pFile);
		return ERR_OK;
	}

	VA_SEM_Get(pFile->ulSemId);

	ulCurTime = VA_PVR_Rec_GetTimecode(pFile->ulAcsId);

#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
	// driver test������ timestamp���� ����� ���޵��� �ʴ� �� ����.. ���� �ð��� ���
	if (ulCurTime >= ulTimestamp + 1)
#else
	// timestamp���� ����� ���� �ȵȴٸ�,, �Ʒ� ó�� ����.. �Ʒ������� 5���̻� ���̳��� ���� timecode�� ��ü��.
	if (ulCurTime >= ulTimestamp + 5)
#endif
	{
		ulEcmTime = ulCurTime;
	}
	else
	{
		ulEcmTime = ulTimestamp;
	}

	if (pFile->bIsTimeshift)
	{
		// timeshift�� ���,
		local_va_pvr_tsr_AddEcm(pEcmChannel, ulEcmLength, pucEcm, ulEcmTime);
	}
	else
	{
		// rec�ΰ��,

		//## ecm packet ����...
		// time stamp 			: 4
		// ecm pid 			: 2
		// ecm length 			: 2
		// ecm data 			: max  1024

		nOffset = 0;

		// time stamp
		Put32bit(aucVaEcmWriteBuffer + nOffset, ulEcmTime);
		nOffset += 4;

		// ecm pid
		Put16bit(aucVaEcmWriteBuffer + nOffset, pEcmChannel->usPid);
		nOffset += 2;

		if (ulEcmLength % 4)
		{
			ulEcmLength += 4 - (ulEcmLength % 4);
		}

		// ecm length
		Put16bit(aucVaEcmWriteBuffer + nOffset, ulEcmLength);
		nOffset += 2;

		// ecm
		VA_memcpy(aucVaEcmWriteBuffer + nOffset, pucEcm, ulEcmLength);
		nOffset += ulEcmLength;

		ulWritten = HLIB_FILE_Write((HxFILE_t)pFile->ulFileHandle, aucVaEcmWriteBuffer, 1, nOffset);
		if (ulWritten <= 0)
		{
			HxLOG_Info("fail to write\n");
			VA_SEM_Release(pFile->ulSemId);
			return ERR_FAIL;
		}

		if (nOffset != ulWritten)
		{
			HxLOG_Info("wrong written data (%d,%d)\n", nOffset, ulWritten);
			VA_SEM_Release(pFile->ulSemId);
			return ERR_FAIL;
		}

		// pb�� �ɷ������� (chase play)
		if (pFile->hVaActionPb != HANDLE_NULL)
		{
			local_va_pvr_pb_AddEcm(aucVaEcmWriteBuffer);
		}

		HxLOG_Info("[timestamp:%ld, pid:%x, ecm_len:%d, total_len:%d, written:%d]\n", ulEcmTime, pEcmChannel->usPid, ulEcmLength, nOffset, ulWritten);
	}

	VA_SEM_Release(pFile->ulSemId);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

HERROR VA_PVR_Adapt_WriteMetaData(HUINT32 ulAcsId, HUINT32 ulMetaLength, HUINT8 *pMeta)
{
	Handle_t 			hVaAction;
	SvcMeta_Record_t 	stRecInfo;
	HCHAR 				szFileName[VA_PVR_MAX_FILE_NAME_LENGTH + 16];
	HUINT32 			ulWritten, ulFileHandle, ulContentsId;

	HxFILE_t			hFile;

	HxLOG_Print("[Enter]\n");

	if (pMeta == NULL)
	{
		HxLOG_Info("pMetaData is null\n");
		return ERR_FAIL;
	}

	hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(ulAcsId);

	if (SVC_META_GetContentIdByhAction(VAMGR_GetActionHandle(hVaAction), &ulContentsId) != ERR_OK)
	{
		HxLOG_Info("fail to get contentid for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	if (SVC_META_GetRecordInfo(ulContentsId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Info("fail to get recordinfo for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	snprintf(szFileName, VA_PVR_MAX_FILE_NAME_LENGTH + 16, "%s%s%s", stRecInfo.aucPath, stRecInfo.aucFileName, EXT_VA_PVR_META_FILE);

	hFile = HLIB_FILE_Open(szFileName, "w");
	if (hFile == NULL)
	{
		HxLOG_Info(" fail to open file for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	ulWritten = HLIB_FILE_Write(hFile, pMeta, 1, ulMetaLength);
	if (ulWritten <= 0 || ulMetaLength != ulWritten)
	{
		HxLOG_Info("fail to write meta file for acs (%d)\n", ulAcsId);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

HERROR VA_PVR_Adapt_ReadMetaData(HUINT32 ulAcsId, HUINT32 ulMetaLength, HUINT8 *pMeta)
{
	Handle_t 			hVaAction;
	SvcMeta_Record_t 	stRecInfo;
	HCHAR 				szFileName[VA_PVR_MAX_FILE_NAME_LENGTH + 16];
	HUINT8 				aucVaMetaReadBuffer[VA_PVR_META_BUFFER_SIZE];
	HUINT32 			ulRead, ulFileHandle, ulContentsId;

	HxFILE_t		hFile;

	HxLOG_Print("[Enter]\n");

	if (pMeta == NULL)
	{
		HxLOG_Info("pMetaData is null\n");
		return ERR_FAIL;
	}

	if (ulMetaLength > VA_PVR_META_BUFFER_SIZE)
	{
		HxLOG_Info("too small buffer size. required size is (%d)\n", ulMetaLength);
		return ERR_FAIL;
	}

	hVaAction = VA_PI_Adapt_GetVaActionHandleByAcsId(ulAcsId);

	if (SVC_META_GetContentIdByhAction(VAMGR_GetActionHandle(hVaAction), &ulContentsId) != ERR_OK)
	{
		HxLOG_Info("fail to get contentid for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	if (SVC_META_GetRecordInfo(ulContentsId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Info("fail to get recordinfo for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	snprintf(szFileName, VA_PVR_MAX_FILE_NAME_LENGTH + 16, "%s%s%s", stRecInfo.aucPath, stRecInfo.aucFileName, EXT_VA_PVR_META_FILE);

	hFile - HLIB_FILE_Open(szFileName, "r");
	if (hFile == NULL)
	{
		HxLOG_Info(" fail to open file for acs (%d)\n", ulAcsId);
		return ERR_FAIL;
	}

	ulRead = HLIB_FILE_Read(hFile, aucVaMetaReadBuffer, 1, ulMetaLength);
	if (ulRead <= 0 || ulMetaLength != ulRead)
	{
		HxLOG_Info("fail to write meta file for acs (%d)\n", ulAcsId);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);

	VA_memcpy(pMeta, aucVaMetaReadBuffer, ulMetaLength);

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}


