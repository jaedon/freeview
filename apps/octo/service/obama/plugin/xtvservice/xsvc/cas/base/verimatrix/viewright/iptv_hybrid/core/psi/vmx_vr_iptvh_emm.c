/* (c) 2011-2015 Humax Co., Ltd. 
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
	@file     vmx_vr_iptvh_emm.c
	@brief    vmx_vr_iptvh_emm.c (Verimatrix IPTV Hybrid EMM)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "_xsvc_cas_vmx_iptv_main.h"
#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_os.h"
#include "vmx_vr_iptvh_api.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define VMXIPTV_EMM_QUEUE_SIZE			10	// PT에서 10~15개 정도로 recommend 함.
#define VMXIPTV_EMM_QUEUE_NONE			0xFF

#define GetEmmQueueSlot(x)		(VmxIptv_EmmRaw_t *)(&s_stVmxIptvEmmQueue.astEmm[x])

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

typedef struct {
	HUINT8 				*pucRawData;
	Vmx_Emm_t			*pEmm;
} VmxIptv_EmmRaw_t;

typedef struct {
	HUINT8				ucQueueCnt;
	HUINT8				ucFirstIdx;							// 첫번째 slot
	HUINT8				ucLastIdx;							// 마지막 slot
	HUINT8				ucNextDelIdx;						// 다음에 지울 slot
	VmxIptv_EmmRaw_t			astEmm[VMXIPTV_EMM_QUEUE_SIZE];
	HUINT8				aucNextIdx[VMXIPTV_EMM_QUEUE_SIZE];		// 다음 slot을 가리킨다.
} VmxIPtv_EmmQueue_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG 		 s_ulVmxIptvEmmQSemId;
VmxIPtv_EmmQueue_t 			s_stVmxIptvEmmQueue;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*	
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/
STATIC HERROR iptvh_emm_QueueInit(void);
STATIC HERROR iptvh_emm_QueuePut(Vmx_Emm_t *pEmm, HUINT8 *pucRawData, HBOOL *pbMsg);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEF_EMM_FILTERING__________________________________________________________________________________
STATIC HERROR iptvh_emm_QueueInit(void)
{
	register int i;

	HxLOG_Print("[Enter]\r\n");

	for (i = 0; i < VMXIPTV_EMM_QUEUE_SIZE; i++)
	{
		s_stVmxIptvEmmQueue.astEmm[i].pucRawData 	= NULL;
		s_stVmxIptvEmmQueue.astEmm[i].pEmm 			= NULL;
		s_stVmxIptvEmmQueue.aucNextIdx[i]			= VMXIPTV_EMM_QUEUE_NONE;
	}

	s_stVmxIptvEmmQueue.ucQueueCnt			= 0;
	s_stVmxIptvEmmQueue.ucFirstIdx			= VMXIPTV_EMM_QUEUE_NONE;
	s_stVmxIptvEmmQueue.ucLastIdx			= VMXIPTV_EMM_QUEUE_NONE;
	s_stVmxIptvEmmQueue.ucNextDelIdx			= VMXIPTV_EMM_QUEUE_SIZE/2;		// 가운데 있는 것부터 지운다.

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC HERROR iptvh_emm_QueuePut(Vmx_Emm_t *pEmm, HUINT8 *pucRawData, HBOOL *pbMsg)
{
	register int 		i;
	VmxIptv_EmmRaw_t 	*pEmmRaw;
	HUINT8			*pucTmp = NULL;

//	HxLOG_Print("[Enter]\r\n");

	if(pbMsg == NULL)
	{
		return ERR_FAIL;
	}

	*pbMsg = TRUE;

	VMX_SEM_Get(s_ulVmxIptvEmmQSemId);
	if (s_stVmxIptvEmmQueue.ucQueueCnt  < VMXIPTV_EMM_QUEUE_SIZE)
	{
		// 빈 슬롯을 찾는다.

		// last idx부터 시작한다.
		i = s_stVmxIptvEmmQueue.ucLastIdx;
		do
		{
			// last idx의 다음 것부터 시작되고, last idx = 0xff라도 아래 식에서 정상 값으로 돌아온다.
			// loop를 돌고나면 1씩 증가하면서 slot을 확인해본다.
			i = (i + 1) % VMXIPTV_EMM_QUEUE_SIZE;

			pEmmRaw = GetEmmQueueSlot(i);
		} while (pEmmRaw->pEmm);

		if (s_stVmxIptvEmmQueue.ucQueueCnt == 0)
		{
			// queue가 비어있었다면 first idx도 기록해줘야지
			s_stVmxIptvEmmQueue.ucFirstIdx = i;
		}
		else
		{
			// 아니라면 last slot의 next idx 값을 update해주자
			s_stVmxIptvEmmQueue.aucNextIdx[s_stVmxIptvEmmQueue.ucLastIdx] = i;
		}
		s_stVmxIptvEmmQueue.aucNextIdx[i] = VMXIPTV_EMM_QUEUE_NONE;
		s_stVmxIptvEmmQueue.ucLastIdx = i;


		s_stVmxIptvEmmQueue.ucQueueCnt++;
	}
	else
	{
		// 빈슬롯이 없으면 하나 지우고 대신 넣는다.
		pEmmRaw = GetEmmQueueSlot(s_stVmxIptvEmmQueue.ucNextDelIdx);
		pucTmp = pEmmRaw->pucRawData;

		// 다음 지울 위치를 정해놓는다.	ex. 5->8->1->4->7->(0)->3->6->9->2->5...
		s_stVmxIptvEmmQueue.ucNextDelIdx = (s_stVmxIptvEmmQueue.ucNextDelIdx + 3) % VMXIPTV_EMM_QUEUE_SIZE;
		if (s_stVmxIptvEmmQueue.ucNextDelIdx == 0)
		{
			// idx = 0은 바로 다음에 꺼낼 것이니까 건드리지 말자.
			s_stVmxIptvEmmQueue.ucNextDelIdx = (s_stVmxIptvEmmQueue.ucNextDelIdx + 3);
		}

		*pbMsg = FALSE;
	}

	pEmmRaw->pucRawData 	= pucRawData;
	pEmmRaw->pEmm 		= pEmm;

	if (pucTmp)
	{
		VMX_MEM_Free(pucTmp);
	}

	VMX_SEM_Release(s_ulVmxIptvEmmQSemId);

//	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC HUINT8 *iptvh_emm_QueueGet(void)
{
	VmxIptv_EmmRaw_t 	*pEmmFirst;

//	HxLOG_Print("[Enter]\r\n");

	#if 0
	if (pnScSlot == NULL)
	{
		HxLOG_Error("null ptr\r\n");
		return NULL;
	}
	#endif

	VMX_SEM_Get(s_ulVmxIptvEmmQSemId);

	if (s_stVmxIptvEmmQueue.ucQueueCnt == 0 || s_stVmxIptvEmmQueue.ucFirstIdx == VMXIPTV_EMM_QUEUE_NONE)
	{
		// 남아 있는 node가 없음
//		HxLOG_Print("EMM QUEUE EMPTY\r\n");
		VMX_SEM_Release(s_ulVmxIptvEmmQSemId);
		return NULL;
	}

	// 첫번째 slot을 찾아요
	pEmmFirst = GetEmmQueueSlot(s_stVmxIptvEmmQueue.ucFirstIdx);

	s_stVmxIptvEmmQueue.ucFirstIdx = s_stVmxIptvEmmQueue.aucNextIdx[s_stVmxIptvEmmQueue.ucFirstIdx];
	s_stVmxIptvEmmQueue.ucQueueCnt--;

	// queue가 비었다면 last idx도 고쳐주자
	if (s_stVmxIptvEmmQueue.ucQueueCnt == 0)
		s_stVmxIptvEmmQueue.ucLastIdx = VMXIPTV_EMM_QUEUE_NONE;

	#if 0
	if (pEmmFirst->pEmm == NULL)
		*pnScSlot = HANDLE_NULL;
	else
		*pnScSlot = pEmmFirst->pEmm->nScSlot;
	#endif

	pEmmFirst->pEmm = NULL;

	VMX_SEM_Release(s_ulVmxIptvEmmQSemId);

//	HxLOG_Print("[Exit]\r\n");

	return pEmmFirst->pucRawData;
}

STATIC HERROR iptvh_emm_QueueClear(void)
{
//	HINT32 	nScSlot;
	HUINT8	*pucData;

	HxLOG_Print("[Enter]\r\n");

	while (1)
	{
//		pucData = CXEMM_QueueGet(&nScSlot);
		pucData = iptvh_emm_QueueGet();
		if (pucData == NULL)
			break;

		VMX_MEM_Free(pucData);
	}

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC void iptvh_emm_SendEmmMsgToVm(HUINT32 ulLen)
{
	HUINT8	*pucData = NULL;
	HUINT32 tspacketNum = 0;

//	HxLOG_Print("[Enter]\n");

	pucData = vmx_vr_iptvh_emm_QueueGet();

	if (pucData)
	{
		vmx_vr_iptvh_api_SetEmm(&pucData, &tspacketNum); // VMX_TODO : TS로 전달되게 수정되어져야한다.
#ifdef CONFIG_DEBUG		
		HxLOG_Print("[%s:%d] ulLen(%d), tspacketNum(%d)\n", __FUNCTION__, __LINE__, ulLen, tspacketNum);
		xsvc_vmx_main_PrintDump(VMX_DBG_VR_IPTVH_MGR, "EMMData", pucData, ulLen);
#endif
	}

	VMX_MEM_Free(pucData);

//	HxLOG_Print("[Exit]\n");
}

STATIC HERROR iptvh_emm_Init(void)
{
	HINT32 	nResult = 0;

	nResult = VMX_SEM_Create("VmxIptvCEmmQSem", &s_ulVmxIptvEmmQSemId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	return iptvh_emm_QueueInit();
}

void vmx_vr_iptvh_emm_SendEmmMsgToVm(HUINT32 ulLen)
{
	return iptvh_emm_SendEmmMsgToVm(ulLen);
}

HUINT8 *vmx_vr_iptvh_emm_QueueGet(void)
{
//	return iptvh_emm_QueueGet(pnScSlot);
	return iptvh_emm_QueueGet();
}

HERROR vmx_vr_iptvh_emm_QueueClear(void)
{
	return iptvh_emm_QueueClear();
}

HERROR vmx_vr_iptvh_emm_OnReceived(Vmx_Emm_t *pEmm, HUINT8 *pucRawData, HBOOL *pbMsg)
{
	return iptvh_emm_QueuePut(pEmm, pucRawData, pbMsg);
}

HERROR vmx_vr_iptvh_emm_Init(void)
{
	return iptvh_emm_Init();
}
