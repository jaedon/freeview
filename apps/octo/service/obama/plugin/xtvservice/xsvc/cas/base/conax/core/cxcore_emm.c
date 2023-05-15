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
	@file     cxcore_emm.c
	@brief    cxcore_emm.c (CONAX MAIN / ECM)

	Description: cat 및 emm table 관리 등등 \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <util.h>
#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_emm.h"
#include "../local_include/cxcore_sc.h"
#include "../local_include/cxcore_session.h"

#include "../local_include/cx_dmx.h"

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines  	   *************************/
/*******************************************************************/

#define IsValidEmmAddr(x)			(x[0])

// emm queue
//#define CX_EMM_QUEUE_SIZE			(10*CX_MAX_NUM_OF_EMM)		// 처리할 ts 마다 10弱낼오?emm 처리 가능하도록
#define CX_EMM_QUEUE_SIZE			10	// PT에서 10~15개 정도로 recommend 함.

#define CX_EMM_QUEUE_NONE			0xFF
#define GetEmmQueueSlot(x)		(CxEmmRaw_t *)(&s_stCxEmmQueue.astEmm[x])


/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

// information about cat & emm filter
typedef struct
{
	HUINT16		usUniqueTsId;		// ts id로 구분해야 할 것 같네요.
	Handle_t	hSession;			// filter 걸려니까 필요한 것 같음 (octo의 경우 filter걸때 action handle을 써야 해서..)
	HINT32		nScSlot;
	HUINT16		usEmmPid;
	HUINT16		usEmmAddrCnt;
	CxEmmAddr_t* pstEmmAddr;
	HUINT8*		pucCat;
} CxEmm_t;

typedef struct
{
	HUINT8* pucRawData;
	CxEmm_t* 				pEmm;
} CxEmmRaw_t;

typedef struct
{
	HUINT8		ucQueueCnt;
	HUINT8		ucNextDelIdx;						// 다음에 지울 slot
	CxEmmRaw_t	astEmm[CX_EMM_QUEUE_SIZE];
} CxEmmQueue_t;

/*******************************************************************/
/********************      Static Variables 	********************/
/******************************************************************/
STATIC HULONG	s_ulCxEmmSemId, s_ulCxEmmQSemId;

// emm queue
STATIC CxEmmQueue_t	s_stCxEmmQueue;

// emm table
STATIC CxEmm_t		s_astCxEmm[CX_MAX_NUM_OF_EMM];

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/
STATIC HERROR local_cxemm_RestartFilter(CxEmm_t *pEmm);
STATIC HERROR local_cxemm_OnEmmReceived(HUINT32 ulFilterId, HUINT8* pucRawData);

// emm queue
STATIC HERROR local_cxemm_QueueInit(void);
STATIC HERROR local_cxemm_QueuePut(CxEmm_t* pEmm, HUINT8* pucRawData, HBOOL* pbMsg);


#define INIT_________________________________________________

HERROR CXEMM_InitCore(void)
{
	int		i;
	HINT32	nResult			= 0;

	ENTER_FUNCTION;

	nResult = CX_SEM_Create("CxCEmmSem", &s_ulCxEmmSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	nResult = CX_SEM_Create("CxCEmmQSem", &s_ulCxEmmQSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	CX_memset(s_astCxEmm, 0, sizeof(CxEmm_t) * CX_MAX_NUM_OF_EMM);

	for (i = 0; i < CX_MAX_NUM_OF_EMM; i++)
	{
		s_astCxEmm[i].hSession = HANDLE_NULL;
		s_astCxEmm[i].usUniqueTsId = 0;
		s_astCxEmm[i].nScSlot = CX_SC_NONE;
		s_astCxEmm[i].pucCat = NULL;
		s_astCxEmm[i].usEmmPid = PID_NULL;
		s_astCxEmm[i].usEmmAddrCnt = 0;
		s_astCxEmm[i].pstEmmAddr = NULL;
	}

	// init ecm queue
	local_cxemm_QueueInit();
	CXDMX_RegisterEmmCb(local_cxemm_OnEmmReceived);
	LEAVE_FUNCTION;
	return ERR_OK;
}

#define CAT_____________________________________________

static HBOOL local_cxemm_IsUpdateCat(CxEmm_t *pstEmm, const HUINT8* pucCat)
{
	HUINT16	usOldCat;
	HUINT16 usNewCat;

	if(pstEmm->pucCat == NULL)
		return TRUE;

	usOldCat = get12bit(pstEmm->pucCat + 1) + 3;
	usNewCat = get12bit(pucCat + 1) + 3;

	if(usOldCat != usNewCat)
		return TRUE;

	if (CX_memcmp(pstEmm->pucCat, pucCat, usNewCat) != 0)
		return TRUE;

	return FALSE;
}

// pucCat는 여기서 copy하고 그냥 돌려줄거니, 처음 만든곳에서 다시 free하시용
HERROR CXEMM_UpdateCat(Handle_t hSession, HUINT16 usUniqueTsId, HUINT8* pucCat)
{
	int		i;
	HINT32	nScSlot;
	HBOOL	bNotFound = TRUE;
	HUINT16	usCatLen;

	ENTER_FUNCTION;

	if(pucCat == NULL)
	{
		HxLOG_Error("pucCat NULL\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxEmmSemId);

	nScSlot = CXSESSION_GetScSlot(hSession);
	if(nScSlot == CX_SC_NONE)
	{
		HxLOG_Error("no smartcard\n");
		CX_SEM_Release(s_ulCxEmmSemId);
		return ERR_FAIL;
	}

	// unique ts id를 우선 비교, 같은 것이 있으면 cat 비교후 처리. 같은게 없으면 새로 등록
	for (i = 0; i < CX_MAX_NUM_OF_EMM; i++)
	{
		// ts id
		if (s_astCxEmm[i].usUniqueTsId == usUniqueTsId)
		{
			// 기존 cat와 비교해서 같으면 무시
			if (local_cxemm_IsUpdateCat(&s_astCxEmm[i], pucCat) == FALSE)
			{
				HxLOG_Print("same CAT: EMM filter restart\r\n");
				s_astCxEmm[i].hSession = hSession;
				local_cxemm_RestartFilter(&s_astCxEmm[i]);
				CX_SEM_Release(s_ulCxEmmSemId);
				return ERR_OK;
			}
			bNotFound = FALSE;
			break;
		}
	}

	if (bNotFound)
	{
		// 같은 ts가 없으면 새로 등록한다.
		for (i = 0; i < CX_MAX_NUM_OF_EMM; i++)
		{
			if (s_astCxEmm[i].usUniqueTsId == 0)
			{
				s_astCxEmm[i].nScSlot = nScSlot;
				s_astCxEmm[i].usUniqueTsId = usUniqueTsId;
				bNotFound = FALSE;
				break;
			}
		}

		if (bNotFound)
		{
			// 빈곳도 없다면 에러
			HxLOG_Error("no slot available\r\n");
			CX_SEM_Release(s_ulCxEmmSemId);
			return ERR_FAIL;
		}
	}

	usCatLen = get12bit(pucCat + 1) + 3;

	if (s_astCxEmm[i].pucCat != NULL)
	{
		CX_MEM_Free(s_astCxEmm[i].pucCat);
	}

	s_astCxEmm[i].pucCat = CX_MEM_Alloc(usCatLen);
	if(s_astCxEmm[i].pucCat == NULL)
	{
		CX_SEM_Release(s_ulCxEmmSemId);
		return ERR_OK;
	}

	s_astCxEmm[i].hSession = hSession;
	CX_memcpy(s_astCxEmm[i].pucCat, pucCat, usCatLen);

	CX_SEM_Release(s_ulCxEmmSemId);

	// semaphore 꼬이니까 sem release하고 호출해야됨
	CXSC_SendCat(nScSlot, (Handle_t)&s_astCxEmm[i], s_astCxEmm[i].pucCat);
	LEAVE_FUNCTION;
	return ERR_OK;
}

#define EMM_TABLE__________________________________________________

STATIC HERROR local_cxemm_StartFilter(CxEmm_t *pEmm)
{
	register int	i;
	HERROR			nErr			= ERR_OK;
	Handle_t		hStbHandle		= HANDLE_NULL;

	ENTER_FUNCTION;

	if (pEmm->usEmmPid == PID_NULL)
	{
		//		CX_SEM_Release(s_ulCxEmmSemId);
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < pEmm->usEmmAddrCnt; i++)
	{
		if (pEmm->pstEmmAddr[i].eState != eCxEmm_Running)
		{
			// aucFilter = 0x82 + address, aucMask = all 0xFF.. cxcore_sc에서 이런 형태로 setting해서 CXEMM_Add()로 호출됨
			HxLOG_Print("[EMM:%02X %02X %02X %02X %02X %02X %02X %02X]\n", pEmm->pstEmmAddr[i].aucFilter[0]		//
																	, pEmm->pstEmmAddr[i].aucFilter[1]		//
																	, pEmm->pstEmmAddr[i].aucFilter[2]		//
																	, pEmm->pstEmmAddr[i].aucFilter[3]		//
																	, pEmm->pstEmmAddr[i].aucFilter[4]		//
																	, pEmm->pstEmmAddr[i].aucFilter[5]		//
																	, pEmm->pstEmmAddr[i].aucFilter[6]		//
																	, pEmm->pstEmmAddr[i].aucFilter[7]);	//
			hStbHandle = CXSESSION_GetStbHandle(pEmm->hSession);
			HxLOG_Print("hStbHandle (0x%x)\n", hStbHandle);
			nErr = CXDMX_RetrieveEmmSection(hStbHandle, pEmm->usEmmPid, pEmm->pstEmmAddr[i].aucFilter, pEmm->pstEmmAddr[i].aucMask, &pEmm->pstEmmAddr[i].ulFilterId);
			if (nErr == ERR_OK)
			{
				pEmm->pstEmmAddr[i].eState = eCxEmm_Running;
			}
		}
	}

	//	CX_SEM_Release(s_ulCxEmmSemId);
	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxemm_StopFilter(CxEmm_t *pEmm)
{
	register int	i;
	HERROR			nErr			= ERR_OK;

	ENTER_FUNCTION;

	if (pEmm->usEmmPid == PID_NULL)
	{
		//		CX_SEM_Release(s_ulCxEmmSemId);
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < pEmm->usEmmAddrCnt; i++)
	{
		if (pEmm->pstEmmAddr[i].eState == eCxEmm_Running)
		{
			pEmm->pstEmmAddr[i].eState = eCxEmm_Stopped;
			nErr = CXDMX_StopEmmSection(pEmm->pstEmmAddr[i].ulFilterId);
		}
	}

	//	CX_SEM_Release(s_ulCxEmmSemId);
	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxemm_RestartFilter(CxEmm_t *pEmm)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	local_cxemm_StopFilter(pEmm);
	nErr = local_cxemm_StartFilter(pEmm);

	LEAVE_FUNCTION;

	return nErr;
}

STATIC HERROR local_cxemm_Add(Handle_t hEmm, HUINT16 usEmmPid, HUINT16 usAddrCnt, CxEmmAddr_t* pstAddr)
{
	CxEmm_t *pEmm;
	HERROR	nErr;

	ENTER_FUNCTION;

	pEmm = (CxEmm_t *)hEmm;

	if (pstAddr == NULL)
	{
		HxLOG_Error("null addr\r\n");
		return ERR_FAIL;
	}

	if (hEmm == HANDLE_NULL)
	{
		CX_MEM_Free(pstAddr);
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	if (pEmm == NULL)
	{
		CX_MEM_Free(pstAddr);
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	// 이미 있는가 확인하여 있다면 내리고
	if (pEmm->usEmmAddrCnt)
	{
		local_cxemm_StopFilter(pEmm);

		// clear data
		pEmm->usEmmAddrCnt = 0;
		if (pEmm->pstEmmAddr)
		{
			CX_MEM_Free(pEmm->pstEmmAddr);
		}
	}

	pEmm->usEmmPid = usEmmPid;

	//새로운 필터들 등록
	pEmm->usEmmAddrCnt = usAddrCnt;
	pEmm->pstEmmAddr = pstAddr;

	nErr = local_cxemm_StartFilter(pEmm);

	LEAVE_FUNCTION;
	return nErr;
}

STATIC HERROR local_cxemm_Delete(CxEmm_t *pEmm)
{
	ENTER_FUNCTION;

	// CaDescEmm이 왔을 때 기존에 있던 거는 다 filter stop
	local_cxemm_StopFilter(pEmm);

	// clear data
	pEmm->usEmmAddrCnt = 0;
	if (pEmm->pstEmmAddr)
	{
		CX_MEM_Free(pEmm->pstEmmAddr);
		pEmm->pstEmmAddr = NULL;
	}

	pEmm->usUniqueTsId = 0;
	pEmm->hSession = HANDLE_NULL;
	if (pEmm->pucCat)
	{
		CX_MEM_Free(pEmm->pucCat);
		pEmm->pucCat = NULL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

// pucRawData는 여기서 copy해서 쓸거니, 만든곳에서 free하셔
//## copy한 놈은 queue에서 관리하자.
STATIC HERROR local_cxemm_OnEmmReceived(HUINT32 ulFilterId, HUINT8* pucRawData)
{
	CxEmm_t* 		pEmm			= NULL;
	register int	i, j;
	HUINT8* 		pucRawEmm;
	HUINT16			usLen;
	HERROR			nErr			= ERR_OK;
	HBOOL			bMsg			= TRUE;

	ENTER_FUNCTION;

	if (pucRawData == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxEmmSemId);

	for (i = 0; i < CX_MAX_NUM_OF_EMM; i++)
	{
		for (j = 0; j < s_astCxEmm[i].usEmmAddrCnt; j++)
		{
			if (s_astCxEmm[i].pstEmmAddr[j].ulFilterId == ulFilterId && s_astCxEmm[i].pstEmmAddr[j].eState == eCxEmm_Running)
			{
				pEmm = &s_astCxEmm[i];
				break;
			}
		}
	}

	if (pEmm == NULL)
	{
		CX_SEM_Release(s_ulCxEmmSemId);
		HxLOG_Error("null emm\r\n");
		return ERR_FAIL;
	}

	usLen = get12bit(pucRawData + 1) + 3;
	pucRawEmm = (HUINT8 *)CX_MEM_Alloc(usLen);
	if (pucRawEmm == NULL)
	{
		CX_SEM_Release(s_ulCxEmmSemId);
		HxLOG_Error("alloc fail\r\n");
		return ERR_FAIL;
	}

	CX_memcpy(pucRawEmm, pucRawData, usLen);

	//## emm queue에다가 넣고 mgr task에 msg 보내준다.
	nErr = local_cxemm_QueuePut(pEmm, pucRawEmm, &bMsg);

	// emm buffer size 이상 emm event를 발생할 필요 없습니다...(Queue full의 원인...)
	if (nErr == ERR_OK && bMsg == TRUE)
	{
		CXCORE_SendMsg(eCxMsg_Emm, HANDLE_NULL, (HUINT32)0, (HUINT32)0);
	}

	CX_SEM_Release(s_ulCxEmmSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}


HERROR CXEMM_RestartFilterByTsId(Handle_t hSession, HUINT16 usUniqueTsId)
{
	HERROR	nErr	= ERR_OK;
	int		i;

	CX_SEM_Get(s_ulCxEmmSemId);

	for (i = 0 ; i < CX_MAX_NUM_OF_EMM ; i++)
	{
		if (s_astCxEmm[i].usUniqueTsId == usUniqueTsId)
		{
			s_astCxEmm[i].hSession = hSession;	// filter를 걸기 위한 action값을 위해 업데이트.
			nErr = local_cxemm_RestartFilter(&s_astCxEmm[i]);
			break;
		}
	}

	CX_SEM_Release(s_ulCxEmmSemId);

	return nErr;
}

HERROR CXEMM_Add(Handle_t hEmm, HUINT16 usEmmPid, HUINT16 usAddrCnt, CxEmmAddr_t* pstAddr)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxEmmSemId);

	nErr = local_cxemm_Add(hEmm, usEmmPid, usAddrCnt, pstAddr);

	CX_SEM_Release(s_ulCxEmmSemId);

	LEAVE_FUNCTION;
	return nErr;
}

HERROR CXEMM_Delete(HINT32 nScSlot)
{
	register int	i;
	HERROR			nErr			= ERR_OK;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxEmmSemId);

	for (i = 0; i < CX_MAX_NUM_OF_EMM; i++)
	{
		if (s_astCxEmm[i].nScSlot == nScSlot)
		{
			nErr = local_cxemm_Delete(&s_astCxEmm[i]);
		}
	}

	CX_SEM_Release(s_ulCxEmmSemId);
	LEAVE_FUNCTION;
	return nErr;
}

HERROR CXEMM_DeleteByTsId(HUINT16 usUniqueTsId)
{
	register int	i;
	HERROR			nErr			= ERR_OK;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxEmmSemId);

	for (i = 0; i < CX_MAX_NUM_OF_EMM; i++)
	{
		if (s_astCxEmm[i].usUniqueTsId == usUniqueTsId)
		{
			nErr = local_cxemm_Delete(&s_astCxEmm[i]);
		}
	}

	CX_SEM_Release(s_ulCxEmmSemId);
	LEAVE_FUNCTION;
	return nErr;
}

#if 0 // from HDPVR-1000C : filter 개수 제한때문에 만든거 같음..
HUINT32 CX_SCI_SetPriorityEmmAddress(CX_SCI_CA_DESC_EMM		*pImage)
{
	HUINT8					tmpAddrData[MAX_ADDR_LEN];
	HINT32					i, j;
	HINT32					nAddr;

	if (pImage == NULL)
		return 1;

	nAddr = pImage->nEmmAddrCnt;
	for (i = 0; i < nAddr; i++)
	{
		if (pImage->pEmmAddr[i].AddrData[6] == 0xE8 || pImage->pEmmAddr[i].AddrData[6] == 0xE9)
			continue;

		for (j = i+1; j < nAddr; j++)
		{
			if (pImage->pEmmAddr[j].AddrData[6] == 0xE8 || pImage->pEmmAddr[j].AddrData[6] == 0xE9)
			{
				memcpy(tmpAddrData, pImage->pEmmAddr[i].AddrData, MAX_ADDR_LEN);
				memcpy(pImage->pEmmAddr[i].AddrData, pImage->pEmmAddr[j].AddrData, MAX_ADDR_LEN);
				memcpy(pImage->pEmmAddr[j].AddrData, tmpAddrData, MAX_ADDR_LEN);
				break;
			}
		}
	}
	return ERR_OK;
}
#endif

#define EMM_QUEUE_____________________________________________

// s_stCxEmmQueue.astEmm[x].pEmm == NULL이면 빈슬롯.
STATIC HERROR local_cxemm_QueueInit(void)
{
	register int	i;

	ENTER_FUNCTION;

	for (i = 0; i < CX_EMM_QUEUE_SIZE; i++)
	{
		s_stCxEmmQueue.astEmm[i].pucRawData = NULL;
		s_stCxEmmQueue.astEmm[i].pEmm = NULL;
	}

	s_stCxEmmQueue.ucQueueCnt = 0;
	s_stCxEmmQueue.ucNextDelIdx = CX_EMM_QUEUE_SIZE / 2;		// 가운데 있는 것부터 지운다.

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxemm_QueuePut(CxEmm_t* pEmm, HUINT8* pucRawData, HBOOL* pbMsg)
{
	register int	i;
	CxEmmRaw_t* 	pEmmRaw, * pEmmNext;
	HUINT8* 		pucTmp			= NULL;

	ENTER_FUNCTION;

	if (pbMsg == NULL)
	{
		return ERR_FAIL;
	}

	*pbMsg = TRUE;

	CX_SEM_Get(s_ulCxEmmQSemId);

	if (s_stCxEmmQueue.ucQueueCnt < CX_EMM_QUEUE_SIZE)
	{
		// 빈 슬롯을 찾는다.
		pEmmRaw = GetEmmQueueSlot(s_stCxEmmQueue.ucQueueCnt);
		s_stCxEmmQueue.ucQueueCnt++;
	}
	else
	{
		// 빈슬롯이 없으면 하나 지우고 대신 넣는다.
		pEmmRaw = GetEmmQueueSlot(s_stCxEmmQueue.ucNextDelIdx);
		pucTmp = pEmmRaw->pucRawData;

		// 지운 슬롯 이후의 데이터를 앞으로 당긴다.
		for (i = s_stCxEmmQueue.ucNextDelIdx ; i < CX_EMM_QUEUE_SIZE - 1 ; i++)
		{
			pEmmRaw = GetEmmQueueSlot(i);
			pEmmNext = GetEmmQueueSlot(i + 1);

			pEmmRaw->pucRawData = pEmmNext->pucRawData;
			pEmmRaw->pEmm = pEmmNext->pEmm;
		}

		// 마지막 슬롯을 얻어온다.
		pEmmRaw = GetEmmQueueSlot(CX_EMM_QUEUE_SIZE - 1);

		// 다음 지울 위치를 정해놓는다.	ex. 5->8->1->4->7->(0)->3->6->9->2->5...
		s_stCxEmmQueue.ucNextDelIdx = (s_stCxEmmQueue.ucNextDelIdx + 3) % CX_EMM_QUEUE_SIZE;
		if (s_stCxEmmQueue.ucNextDelIdx == 0)
		{
			// idx = 0은 바로 다음에 꺼낼 것이니까 건드리지 말자.
			s_stCxEmmQueue.ucNextDelIdx = (s_stCxEmmQueue.ucNextDelIdx + 3);
		}

		*pbMsg = FALSE;
	}

	pEmmRaw->pucRawData = pucRawData;
	pEmmRaw->pEmm = pEmm;

	if (pucTmp)
	{
		CX_MEM_Free(pucTmp);
	}

	CX_SEM_Release(s_ulCxEmmQSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

// return value is the pointer to the EMM raw data
STATIC HUINT8* local_cxemm_QueueGet(HINT32* pnScSlot)
{
	CxEmmRaw_t* pEmmFirst, * pEmmNext;
	HUINT8* 	pucRawData;
	int			i;

	ENTER_FUNCTION;

	if (pnScSlot == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return NULL;
	}

	CX_SEM_Get(s_ulCxEmmQSemId);

	if (s_stCxEmmQueue.ucQueueCnt == 0)
	{
		// 남아 있는 node가 없음
		HxLOG_Print("EMM QUEUE EMPTY\r\n");
		CX_SEM_Release(s_ulCxEmmQSemId);
		return NULL;
	}

	// 첫번째 slot을 찾아요
	pEmmFirst = GetEmmQueueSlot(0);

	s_stCxEmmQueue.ucQueueCnt--;

	if (pEmmFirst->pEmm == NULL)
	{
		*pnScSlot = HANDLE_NULL;
	}
	else
	{
		*pnScSlot = pEmmFirst->pEmm->nScSlot;
	}

	pucRawData = pEmmFirst->pucRawData;

	// Queue에서 꺼냈으니 앞으로 한칸씩...
	for (i = 0 ; i < s_stCxEmmQueue.ucQueueCnt && i < (CX_EMM_QUEUE_SIZE - 1) ; i++)
	{
		pEmmFirst = GetEmmQueueSlot(i);
		pEmmNext = GetEmmQueueSlot(i + 1);

		pEmmFirst->pucRawData = pEmmNext->pucRawData;
		pEmmFirst->pEmm = pEmmNext->pEmm;
	}

	CX_SEM_Release(s_ulCxEmmQSemId);
	LEAVE_FUNCTION;
	return pucRawData;
}

STATIC HERROR local_cxemm_QueueClear(void)
{
	HINT32	nScSlot;
	HUINT8* pucData;

	ENTER_FUNCTION;

	while (1)
	{
		pucData = CXEMM_QueueGet(&nScSlot);
		if (pucData == NULL)
		{
			break;
		}

		CX_MEM_Free(pucData);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

// return value is the pointer to the EMM raw data
HUINT8* CXEMM_QueueGet(HINT32* pnScSlot)
{
	return local_cxemm_QueueGet(pnScSlot);
}

HERROR CXEMM_QueueClear(void)
{
	return local_cxemm_QueueClear();
}

#if defined(CONFIG_DEBUG)
void CMD_CXEMM_PrintEmmContext(void)
{
	HUINT32	i;

	for (i = 0 ; i < CX_MAX_NUM_OF_EMM ; i++)
	{
		HxLOG_Warning(HxANSI_COLOR_GREEN("CONAX EMM (%d)\n"), i);

		HxLOG_Warning("\t hSession (%d, 0x%x)\n", CXSESSION_GetSessionIdx(s_astCxEmm[i].hSession), s_astCxEmm[i].hSession);
		HxLOG_Warning("\t usUniqueTsId (%d)\n", s_astCxEmm[i].usUniqueTsId);

		if(s_astCxEmm[i].usUniqueTsId == 0)
			continue;

		HxLOG_Warning("\t nScSlot (%d)\n", s_astCxEmm[i].nScSlot);
		HxLOG_Warning("\t usEmmPid (0x%x)\n", s_astCxEmm[i].usEmmPid);
		HxLOG_Warning("\t usEmmAddrCnt (%d)\n", s_astCxEmm[i].usEmmAddrCnt);
	}
}
#endif
/*********************** End of File ******************************/

