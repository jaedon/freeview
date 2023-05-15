/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/**
	@file     cxcore_ecm.c
	@brief    cxcore_ecm.c (CONAX MAIN / ECM)

	Description: ecm table 관리 등등 \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <util.h>
#include <cxmgr_action.h>

#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_ecm.h"
#include "../local_include/cxcore_stream.h"
#include "../local_include/cxcore_session.h"

#if defined(CONFIG_MW_CAS_CONAX_CPM)
#include "../local_include/cxcore_cpm.h"
#endif
#include "../local_include/cx_dmx.h"

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines  	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

typedef enum
{
	eCxEcm_Init,
	eCxEcm_Running,
	eCxEcm_Stopped
} CxEcmState_t;

// information about ecm filter and corresponding CW
typedef struct
{
	HUINT16			usEcmPid;
	HUINT16			usRefCnt;
	Handle_t		hSession;
	CxEcmState_t	eState;
	HUINT16			usUniqueId;		// filter 거는데 필요
	HUINT32			ulFilterId;		// filter 거는데 필요
	HUINT8			aucFilter[CX_ECM_FILTER_LEN];
	HUINT8			aucMask[CX_ECM_FILTER_LEN];
	HUINT8			aucEvenCW[CX_CW_LENGTH];
	HUINT8			aucOddCW[CX_CW_LENGTH];
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	HUINT8			aucCWPK[CX_CPM_KEY_VALUE_BYTES];
	HUINT8* 		pucCWPK;
#endif
#if defined(CONFIG_MW_MM_PVR)
	HUINT8			bQueryMode;// pvr rec시 maturity rate값을 구할 때 사용...
	HUINT8			ucProgMrValue;
#endif
} CxEcm_t;

typedef struct _CxEcmNode_t
{
	HUINT8* 			pucRawData;
	CxEcm_t* 			pEcm;
	struct _CxEcmNode_t* 							pNextNode;
} CxEcmNode_t;

/*******************************************************************/
/********************      Static Variables 	********************/
/*******************************************************************/
STATIC HULONG	s_ulCxEcmSemId, s_ulCxEcmQSemId;

// ecm queue
STATIC CxEcmNode_t	s_stCxEcmQueue;

// ecm table
STATIC CxEcm_t		s_astCxEcm[CX_MAX_NUM_OF_ECM];

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/
STATIC HERROR local_cxecm_StartFilter(Handle_t hEcm, CxEcmMode_t eMode);
STATIC HERROR local_cxecm_StopFilter(Handle_t hEcm);
STATIC HERROR local_cxecm_OnEcmReceived(HUINT32 ulFilterId, HUINT8* pucRawData);

// ecm queue
STATIC HERROR local_cxecm_QueueInit(void);
STATIC HERROR local_cxecm_QueuePut(CxEcm_t* pEcm, HUINT8* pucRawData);
STATIC HUINT8* local_cxecm_QueueGet(Handle_t* pSessionHandle, Handle_t* pEcmHandle);
STATIC HERROR local_cxecm_QueueClear(void);


#define INIT_________________________________________________

HERROR CXECM_InitCore(void)
{
	int		i;
	HINT32	nResult			= 0;

	ENTER_FUNCTION;

	nResult = CX_SEM_Create("CxCEcmSem", &s_ulCxEcmSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	nResult = CX_SEM_Create("CxCEcmQSem", &s_ulCxEcmQSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memset(s_astCxEcm, 0, sizeof(CxEcm_t) * CX_MAX_NUM_OF_ECM);

	for (i = 0; i < CX_MAX_NUM_OF_ECM; i++)
	{
		s_astCxEcm[i].usEcmPid = PID_NULL;
		s_astCxEcm[i].usRefCnt = 0;
		s_astCxEcm[i].hSession = HANDLE_NULL;
		s_astCxEcm[i].eState = eCxEcm_Init;
		s_astCxEcm[i].ulFilterId = 0;
		s_astCxEcm[i].usUniqueId = 0;
		CX_memset(s_astCxEcm[i].aucEvenCW, 0, CX_ECM_FILTER_LEN);
		CX_memset(s_astCxEcm[i].aucOddCW, 0, CX_ECM_FILTER_LEN);

#if defined(CONFIG_MW_MM_PVR)
		s_astCxEcm[i].bQueryMode = FALSE;
		s_astCxEcm[i].ucProgMrValue = 0;
#endif
	}

	// init ecm queue
	local_cxecm_QueueInit();

	CXDMX_RegisterEcmCb(local_cxecm_OnEcmReceived);

	LEAVE_FUNCTION;
	return ERR_OK;
}

Handle_t CXECM_GetStbHandle(Handle_t hEcm)
{
	CxEcm_t* 				pEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return HANDLE_NULL;
	}

	pEcm = (CxEcm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return HANDLE_NULL;
	}

	return CXSESSION_GetStbHandle(pEcm->hSession);
}

Handle_t CXECM_GetHandleBySession(Handle_t hSession)
{
	int	i;

	for (i = 0 ; i < CX_MAX_NUM_OF_ECM ; i++)
	{
		if (s_astCxEcm[i].hSession == hSession)
		{
			break;
		}
	}

	if (i == CX_MAX_NUM_OF_ECM)
	{
		return HANDLE_NULL;
	}

	return (Handle_t)&s_astCxEcm[i];
}

#if defined(CONFIG_MW_MM_PVR)
HERROR CXECM_SetAccessStatus(Handle_t hEcm, HUINT8 ucProgMr)
{
	CxEcm_t* 				pEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return HANDLE_NULL;
	}

	pEcm = (CxEcm_t *)hEcm;

	pEcm->ucProgMrValue = ucProgMr;

	HxLOG_Print("mr(%d) stbHandle(0x%X) \n", ucProgMr, CXSESSION_GetStbHandle(pEcm->hSession));

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXECM_GetAccessStatus(Handle_t hEcm, HUINT8* pucProgMr)
{
	CxEcm_t* 				pEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pEcm = (CxEcm_t *)hEcm;

	*pucProgMr = pEcm->ucProgMrValue;

	HxLOG_Print("mr(%d) stbHandle(0x%X) \n", *pucProgMr, CXSESSION_GetStbHandle(pEcm->hSession));
	LEAVE_FUNCTION;
	return ERR_OK;
}
#endif // CONFIG_MW_MM_PVR

#define ECM_TABLE__________________________________________________

STATIC HERROR local_cxecm_StartFilter(Handle_t hEcm, CxEcmMode_t eMode)
{
	HERROR		nErr			= ERR_FAIL;
	CxEcm_t* 	pEcm;
	Handle_t	hStbHandle		= HANDLE_NULL;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pEcm = (CxEcm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	//CX_SEM_Get(s_ulCxEcmSemId);

	if (pEcm->usEcmPid == PID_NULL)
	{
		//CX_SEM_Release(s_ulCxEcmSemId);
		HxLOG_Error("null pid\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pEcm->eState == eCxEcm_Running)
	{
		//CX_SEM_Release(s_ulCxEcmSemId);
		HxLOG_Print("already running\r\n");
		LEAVE_FUNCTION;
		return ERR_OK;
	}


	CX_memset(pEcm->aucFilter, 0, CX_ECM_FILTER_LEN);
	CX_memset(pEcm->aucMask, 0, CX_ECM_FILTER_LEN);

	if (eMode == eCxEcm_Both)
	{
		pEcm->aucFilter[0] = 0x80;
		pEcm->aucMask[0] = 0xFE;
	}
	else
	{
		pEcm->aucFilter[0] = (HUINT8)eMode;
		pEcm->aucMask[0] = 0xFF;
	}

	hStbHandle = CXSESSION_GetStbHandle(pEcm->hSession);
	if (hStbHandle == HANDLE_NULL)
	{
		LEAVE_FUNCTION;
		return nErr;
	}

	nErr = CXDMX_RetrieveEcmSection(hStbHandle, pEcm->usEcmPid, pEcm->aucFilter, pEcm->aucMask, &pEcm->ulFilterId);
	if (nErr == ERR_OK)
	{
		pEcm->eState = eCxEcm_Running;
	}

	//	CX_SEM_Release(s_ulCxEcmSemId);

	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxecm_StopFilter(Handle_t hEcm)
{
	CxEcm_t* 				pEcm;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pEcm = (CxEcm_t *)hEcm;
	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	//	CX_SEM_Get(s_ulCxEcmSemId);

	if (pEcm->usEcmPid == PID_NULL)
	{
		//		CX_SEM_Release(s_ulCxEcmSemId);
		HxLOG_Error("null pid\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pEcm->eState != eCxEcm_Running)
	{
		//		CX_SEM_Release(s_ulCxEcmSemId);
		HxLOG_Print("not running\r\n");
		LEAVE_FUNCTION;
		return ERR_OK;
	}

	pEcm->eState = eCxEcm_Stopped;

	//	CX_SEM_Release(s_ulCxEcmSemId);

	LEAVE_FUNCTION;
	return CXDMX_StopEcmSection(pEcm->ulFilterId);
}

// pucRawData는 여기서 copy해서 쓸거이니, 준곳에서 free하시길.
//## copy한 놈은 queue에서 관리하자.
STATIC HERROR local_cxecm_OnEcmReceived(HUINT32 ulFilterId, HUINT8* pucRawData)
{
	CxEcmMode_t		eMode;
	CxEcm_t* 		pEcm			= NULL;
	register int	i;
	HUINT8* 		pucRawEcm;
	HUINT16			usLen;

	ENTER_FUNCTION;

	if (pucRawData == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxEcmSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ECM; i++)
	{
		if (s_astCxEcm[i].ulFilterId == ulFilterId && s_astCxEcm[i].eState == eCxEcm_Running)
		{
			pEcm = &s_astCxEcm[i];
			break;
		}
	}

	if (pEcm == NULL)
	{
		CX_SEM_Release(s_ulCxEcmSemId);
		HxLOG_Error("null ecm\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	usLen = get12bit(pucRawData + 1) + 3;
	pucRawEcm = (HUINT8 *)CX_MEM_Alloc(usLen);
	if (pucRawEcm == NULL)
	{
		CX_SEM_Release(s_ulCxEcmSemId);
		HxLOG_Error("alloc fail\r\n");
		return ERR_FAIL;
	}

	CX_memcpy(pucRawEcm, pucRawData, usLen);

	// filter toggle해서 다시 건다 (0x80 <-> 0x81)
	if (*pucRawData == eCxEcm_Even)
	{
		eMode = eCxEcm_Odd;
	}
	else
	{
		eMode = eCxEcm_Even;
	}

#if 0
{
	int x;

	DDI_UART_Print("ECM>");

	for (x = 0; x < usLen; x++)
		DDI_UART_Print("%02X ", pucRawData[x]);

	DDI_UART_Print("<END>\n");
}
#endif

	//	CXECM_RestartFilter((Handle_t)pEcm, eMode);
	//	CXECM_StopFilter((Handle_t)pEcm);

	//## ecm queue에다가 넣고 mgr task에 msg 보내준다.
	local_cxecm_QueuePut(pEcm, pucRawEcm);
	CXCORE_SendMsg(eCxMsg_Ecm, (Handle_t)pEcm, eMode, (HUINT32)0);
	CX_SEM_Release(s_ulCxEcmSemId);

	CXECM_RestartFilter((Handle_t)pEcm, eMode);

	LEAVE_FUNCTION;
	return ERR_OK;
}


HERROR CXECM_StartFilter(Handle_t hEcm, CxEcmMode_t eMode)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxEcmSemId);

	nErr = local_cxecm_StartFilter(hEcm, eMode);

	CX_SEM_Release(s_ulCxEcmSemId);

	LEAVE_FUNCTION;
	return nErr;
}

HERROR CXECM_StopFilter(Handle_t hEcm)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxEcmSemId);

	nErr = local_cxecm_StopFilter(hEcm);

	CX_SEM_Release(s_ulCxEcmSemId);

	return  nErr;
}

HERROR CXECM_RestartFilter(Handle_t hEcm, CxEcmMode_t eMode)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxEcmSemId);

	local_cxecm_StopFilter(hEcm);
	nErr = local_cxecm_StartFilter(hEcm, eMode);

	CX_SEM_Release(s_ulCxEcmSemId);

	LEAVE_FUNCTION;
	return nErr;
}

Handle_t CXECM_Add(Handle_t hSession, HUINT16 usEcmPid)
{
	int		i;
	CxEcm_t* 				pEcm			= NULL;

	ENTER_FUNCTION;

	// 이미 사용되고 있는 ecm pid 인지 확인
	CX_SEM_Get(s_ulCxEcmSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ECM; i++)
	{
		if (s_astCxEcm[i].usEcmPid == usEcmPid
#if defined(CONFIG_MW_MM_PVR)
			&& (CXSESSION_GetStbActionId(hSession) == CXSESSION_GetStbActionId(s_astCxEcm[i].hSession))	//
#endif
		   )
		{
			// 이미 있다면 ecm table의 index를 return 한다
			s_astCxEcm[i].usRefCnt++;
			pEcm = &s_astCxEcm[i];
			break;
		}
	}

	if (pEcm == NULL)
	{
		// 사용되지 않고 있다면 ecm pid 새로 등록
		for (i = 0; i < CX_MAX_NUM_OF_ECM; i++)
		{
			if (s_astCxEcm[i].usEcmPid == PID_NULL)
			{
				s_astCxEcm[i].usEcmPid = usEcmPid;
				s_astCxEcm[i].usRefCnt = 1;
				s_astCxEcm[i].hSession = hSession;
				s_astCxEcm[i].ulFilterId = 0;
#if defined(CONFIG_MW_MM_PVR)
				s_astCxEcm[i].bQueryMode = FALSE;
				s_astCxEcm[i].ucProgMrValue = 0;
#endif
				pEcm = &s_astCxEcm[i];
				break;
			}
		}
	}

	CX_SEM_Release(s_ulCxEcmSemId);

	// pEcm == null인 경우, 빈 slot이 없어서 등록할 수 없음
	LEAVE_FUNCTION;
	return (Handle_t)pEcm;
}

// 특정 ecm 을 삭제한다. 단, 다른 stream과 연결되어 있다면 지우지 않는다
HERROR CXECM_Delete(Handle_t hEcm)
{
	CxEcm_t* 				pEcm;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pEcm = (CxEcm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxEcmSemId);

	// 유효한 ecm 인지 확인
	if (pEcm->usEcmPid != PID_NULL)
	{
		// reference한 곳이 1개 이하 이면 삭제
		if (pEcm->usRefCnt <= 1)
		{
			//## 다른 곳에서는 안쓰고 있으니 pEcm을 지워 버립시다
			// filter running 중이면 중단
			// table에서 제거

			if (pEcm->eState == eCxEcm_Running)
			{
				// filter 멈춘다
				local_cxecm_StopFilter((Handle_t)pEcm);
			}

			pEcm->usEcmPid = PID_NULL;
			pEcm->usRefCnt = 0;
			pEcm->hSession = HANDLE_NULL;
			pEcm->ulFilterId = 0;
#if defined(CONFIG_MW_MM_PVR)
			pEcm->bQueryMode = FALSE;
			pEcm->ucProgMrValue = 0;
#endif

			CX_memset(pEcm->aucEvenCW, 0, CX_ECM_FILTER_LEN);
			CX_memset(pEcm->aucOddCW, 0, CX_ECM_FILTER_LEN);
		}
		else
		{
			pEcm->usRefCnt--;
			//## stream과의 연결을 생각해봅시다.
			if (CXSTREAM_FindStreamBySessionAndEcm(pEcm->hSession, (Handle_t)pEcm) != ERR_OK)
			{
				// 같은 ecm을 쓰는 stream 중에 같은 session이 없다면, 다른 session을 찾아서 바꿔주어야지요
				pEcm->hSession = CXSTREAM_GetFirstSessionByEcm((Handle_t)pEcm);
			}
		}
	}
	else
	{
		HxLOG_Error("null ecm pid\r\n");
	}

	CX_SEM_Release(s_ulCxEcmSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// return ECM PID
HUINT16 CXECM_GetPid(Handle_t hEcm)
{
	CxEcm_t* 				pEcm;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return PID_NULL;
	}

	pEcm = (CxEcm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return PID_NULL;
	}

	LEAVE_FUNCTION;
	return pEcm->usEcmPid;
}

HERROR CXECM_SetCW(Handle_t hEcm, HUINT8* pucOddCW, HUINT32 ulOddLen, HUINT8* pucEvenCW, HUINT32 ulEvenLen)
{
	CxEcm_t* 				pEcm;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pEcm = (CxEcm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (ulOddLen > 0)
	{
		CX_memcpy(pEcm->aucOddCW, pucOddCW, CX_ECM_FILTER_LEN);
	}

	if (ulEvenLen > 0)
	{
		CX_memcpy(pEcm->aucEvenCW, pucEvenCW, CX_ECM_FILTER_LEN);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXECM_GetCW(Handle_t hEcm, HUINT8* pucOddCW, HUINT8* pucEvenCW)
{
	CxEcm_t* 				pEcm;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pEcm = (CxEcm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memcpy(pucEvenCW, pEcm->aucEvenCW, CX_ECM_FILTER_LEN);
	CX_memcpy(pucOddCW, pEcm->aucOddCW, CX_ECM_FILTER_LEN);

	LEAVE_FUNCTION;
	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXECM_SetCWPK(Handle_t hEcm, HUINT8* pucCWPK)
{
	CxEcm_t* 				pEcm;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pucCWPK == NULL)
	{
		return ERR_FAIL;
	}

	pEcm = (CxEcm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memcpy(pEcm->aucCWPK, pucCWPK, CX_CPM_KEY_VALUE_BYTES);
	pEcm->pucCWPK = pEcm->aucCWPK;

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXECM_GetCWPK(Handle_t hEcm, HUINT8* pucCWPK)
{
	CxEcm_t* 				pEcm;

	ENTER_FUNCTION;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pEcm = (CxEcm_t *)hEcm;

	if (pEcm == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pEcm->pucCWPK == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memcpy(pucCWPK, pEcm->pucCWPK, CX_CPM_KEY_VALUE_BYTES);

	LEAVE_FUNCTION;
	return ERR_OK;
}
#endif

#define ECM_QUEUE_____________________________________________

STATIC HERROR local_cxecm_QueueInit(void)
{
	ENTER_FUNCTION;

	s_stCxEcmQueue.pEcm = NULL;			// 의미 없음
	s_stCxEcmQueue.pucRawData = NULL;			// 의미 없음
	s_stCxEcmQueue.pNextNode = NULL;		// 이 pointer는 항상 first node를 가리키고 있어야지요,

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxecm_QueuePut(CxEcm_t* pEcm, HUINT8* pucRawData)
{
	CxEcmNode_t* 					pNode, * pNewNode;

	ENTER_FUNCTION;

	//## task 시작전에는 튕기고 싶은데...
	//if (s_bCxCoreTaskRun == FALSE)

	CX_SEM_Get(s_ulCxEcmQSemId);

	// 새로은 node 생성
	pNewNode = CX_MEM_Alloc(sizeof(CxEcmNode_t));
	if (pNewNode == NULL)
	{
		CX_SEM_Release(s_ulCxEcmQSemId);
		HxLOG_Error("alloc fail\r\n");
		return ERR_FAIL;
	}

	pNewNode->pEcm = pEcm;
	pNewNode->pucRawData = pucRawData;
	pNewNode->pNextNode = NULL;

	// 가장 마지막 node를 찾는다
	pNode = &s_stCxEcmQueue;

	while (pNode->pNextNode != NULL)
	{
		pNode = pNode->pNextNode;
	}

	pNode->pNextNode = pNewNode;

	CX_SEM_Release(s_ulCxEcmQSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// return value is the pointer to the ECM raw data
STATIC HUINT8* local_cxecm_QueueGet(Handle_t* pSessionHandle, Handle_t* pEcmHandle)
{
	CxEcmNode_t* 					pFirstNode;
	HUINT8* 	pucData			= NULL;

	ENTER_FUNCTION;

	if (pSessionHandle == NULL || pEcmHandle == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return NULL;
	}

	CX_SEM_Get(s_ulCxEcmQSemId);

	pFirstNode = s_stCxEcmQueue.pNextNode;
	if (pFirstNode == NULL)
	{
		// 남아 있는 node가 없음
		CX_SEM_Release(s_ulCxEcmQSemId);
		HxLOG_Info("ECM QUEUE EMPTY\r\n");
		return NULL;
	}

	// 첫번째 node의 값을 pNode에 복사해준다

	if (pFirstNode->pEcm == NULL)
	{
		*pSessionHandle = HANDLE_NULL;
	}
	else
	{
		*pSessionHandle = pFirstNode->pEcm->hSession;
	}

	*pEcmHandle = (Handle_t)pFirstNode->pEcm;
	pucData = pFirstNode->pucRawData;

	s_stCxEcmQueue.pNextNode = pFirstNode->pNextNode;

	CX_MEM_Free(pFirstNode);

	CX_SEM_Release(s_ulCxEcmQSemId);

	LEAVE_FUNCTION;
	return pucData;
}

STATIC HERROR local_cxecm_QueueClear(void)
{
	Handle_t	hEcm, hSession;
	HUINT8* 	pucData;

	ENTER_FUNCTION;

	while (1)
	{
		pucData = CXECM_QueueGet(&hSession, &hEcm);
		if (pucData == NULL)
		{
			break;
		}

		CX_MEM_Free(pucData);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

HBOOL CXECM_IsQueueEmpty(void)
{
	CxEcmNode_t* 					pFirstNode;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxEcmQSemId);

	pFirstNode = s_stCxEcmQueue.pNextNode;

	CX_SEM_Release(s_ulCxEcmQSemId);

	// 남아 있는 node가 없음
	if (pFirstNode == NULL)
	{
		//HxLOG_Info("QUEUE EMPTY\r\n");
		return TRUE;
	}

	LEAVE_FUNCTION;
	return FALSE;
}

// return value is the pointer to the ECM raw data
HUINT8* CXECM_QueueGet(Handle_t* pSessionHandle, Handle_t* pEcmHandle)
{
	return local_cxecm_QueueGet(pSessionHandle, pEcmHandle);
}

HERROR CXECM_QueueClear(void)
{
	return local_cxecm_QueueClear();
}

#if defined(CONFIG_DEBUG)

STATIC HUINT8* local_cxecm_GetEcmState(CxEcmState_t eState)
{
	HUINT8* pszName;

	switch (eState)
	{
		ENUM_TO_STR(eCxEcm_Init)
		ENUM_TO_STR(eCxEcm_Running)
		ENUM_TO_STR(eCxEcm_Stopped)
	default:
		pszName = (HUINT8 *)"Unknown";
		break;
	}

	return pszName;
}

void CMD_CXECM_PrintEcmContext(void)
{
	HUINT32	i;

	for (i = 0; i < CX_MAX_NUM_OF_ECM; i++)
	{
		HxLOG_Warning(HxANSI_COLOR_GREEN("CONAX ECM (%d)\n"), i);

		HxLOG_Warning("\t hSession (%d, 0x%x)\n", CXSESSION_GetSessionIdx(s_astCxEcm[i].hSession), s_astCxEcm[i].hSession);
		HxLOG_Warning("\t usEcmPid (0x%x)\n", s_astCxEcm[i].usEcmPid);
		HxLOG_Warning("\t eState (%s)\n", local_cxecm_GetEcmState(s_astCxEcm[i].eState));

		if(s_astCxEcm[i].eState != eCxEcm_Running)
			continue;

		HxLOG_Warning("\t usRefCnt (%d)\n", s_astCxEcm[i].usRefCnt);
		HxLOG_Warning("\t ulFilterId (0x%x)\n", s_astCxEcm[i].ulFilterId);
		HxLOG_Warning("\t bQueryMode (%d)\n", s_astCxEcm[i].bQueryMode);
		HxLOG_Warning("\t ucProgMrValue (%d)\n", s_astCxEcm[i].ucProgMrValue);
	}
}
#endif

/*********************** End of File ******************************/

