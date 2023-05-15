/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#include <hlib.h>
#include <rlib.h>
#include <string.h>

#include <vkernel.h>
#include <octo_common.h>

#include <pal_ringbuf.h>

#include "svc_mediaserver.h"

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
#define ENTER_FUNCTION	HxLOG_Info("(+)\r\n")
#define LEAVE_FUNCTION	HxLOG_Debug("(-)\r\n")

#define ENTER_GSEM		VK_SEM_Get (s_ulMediaServerSemId)
#define LEAVE_GSEM		VK_SEM_Release (s_ulMediaServerSemId)


/* pl_streaming_receive.c와 svc_media server 동시에 변경해야 함. */
#define	HOMMA_SERVER_PORT			30991

#define SVC_MEDIASERVER_INST_NUM	16

#define BASIC_RTP_PACKET_UNIT_SIZE	(188*7)

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
typedef struct _SVC_MediaServer_Instance
{
	HBOOL				bExecuting;
	Handle_t			hRingBuf;
	Handle_t			hMsgId;
	HUINT32				ulSlot;
	HINT32				nIndex;
	HINT32				nErrCnt;
	UNIXTIME			utLastUpdate;
	DxRSV_Streaming_t	stRsvStreaming;
} SVC_MediaServer_Instance_t;

typedef struct _SVC_MediaServer_Msg
{
	HxRingBuf_SignalType_e 	eType;
	Handle_t				hRingBuf;
	HxRingBuf_Status_t		stStatus;
} SVC_MediaServer_Msg_t;


STATIC HLONG						s_ulMediaServerSemId = 0;
STATIC HLONG						s_ulMediaServerTaskId = 0;
STATIC HLONG						s_ulMediaServerMsgId = 0;
STATIC SVC_MediaServer_Instance_t	s_astMediaServerInstance[SVC_MEDIASERVER_INST_NUM];
#ifdef CONFIG_DEBUG
STATIC HUINT32						s_aulLastWriteTick[SVC_MEDIASERVER_INST_NUM];
#endif


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC void svc_MediaServer_Task(void *pvArg);

STATIC SVC_MediaServer_Instance_t *svc_MediaServer_GetEmptyInstance (void)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < SVC_MEDIASERVER_INST_NUM; ulIndex++)
	{
		if (TRUE != s_astMediaServerInstance[ulIndex].bExecuting)
		{
			return &(s_astMediaServerInstance[ulIndex]);
		}
	}

	return NULL;
}

STATIC SVC_MediaServer_Instance_t *svc_MediaServer_GetInstanceByRingbuf (Handle_t hRingBuf)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < SVC_MEDIASERVER_INST_NUM; ulIndex++)
	{
		if (TRUE == s_astMediaServerInstance[ulIndex].bExecuting)
		{
			if ( hRingBuf == s_astMediaServerInstance[ulIndex].hRingBuf)
			{
				return &(s_astMediaServerInstance[ulIndex]);
			}
		}
	}

	return NULL;
}

STATIC SVC_MediaServer_Instance_t *svc_MediaServer_GetInstanceBySlot (HINT32 ulSamaSlot)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < SVC_MEDIASERVER_INST_NUM; ulIndex++)
	{
		if (TRUE == s_astMediaServerInstance[ulIndex].bExecuting)
		{
			if ( ulSamaSlot == s_astMediaServerInstance[ulIndex].ulSlot)
			{
				return &(s_astMediaServerInstance[ulIndex]);
			}
		}
	}

	return NULL;
}


STATIC void svc_MediaServer_DeleteInstance (SVC_MediaServer_Instance_t *pstInst)
{
	if(NULL != pstInst)
	{
		HxSTD_memset(pstInst, 0, sizeof(SVC_MediaServer_Instance_t));
	}
}

STATIC HERROR svc_MediaServer_Init(void)
{
	HERROR			hErr = ERR_OK;

	ENTER_FUNCTION;

	hErr = VK_SEM_Create (&s_ulMediaServerSemId, "StreamRes_GSem", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_SEM_Create Semaphore failed!!!\n");
		return ERR_FAIL;
	}

	hErr = VK_TASK_Create (svc_MediaServer_Task, MW_PVR_PLAYER_TASK_PRIORITY, MW_PVR_STACK_SIZE, "MediaServerTask", NULL, &s_ulMediaServerTaskId, 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_TASK_Create error:\n");
		return ERR_FAIL;
	}

	hErr = (HERROR)VK_MSG_Create(OCTO_MSGQ_LEVEL01, sizeof(SVC_MediaServer_Msg_t), "MediaServerMsg", &s_ulMediaServerMsgId, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_MSG_Create error:\n");
		return ERR_FAIL;
	}

	hErr = VK_TASK_Start(s_ulMediaServerTaskId);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_TASK_Start error:\n");
		return ERR_FAIL;
	}

	VK_TASK_Sleep(0);

	LEAVE_FUNCTION;

	return hErr;
}

STATIC void svc_MediaServer_DeInit(void)
{
	if(0 != s_ulMediaServerTaskId)
	{
		VK_TASK_Stop(s_ulMediaServerTaskId);
		VK_TASK_Destroy(s_ulMediaServerTaskId);
	}

	if(0 != s_ulMediaServerSemId)
	{
		VK_SEM_Destroy(s_ulMediaServerSemId);
	}
}


#define __MEDIASERVER_TASK__

STATIC HINT32 svc_MediaServer_CreateSocket(void)
{
	return HLIB_EXTRA_OpenClient("hommastreaming");
}

STATIC void svc_MediaServer_DestroySocket(HINT32 nSock)
{
	(void)HLIB_SOCKET_Close(nSock);
}

STATIC HERROR svc_MediaServer_DataTransfer(HINT32	nSock, HCHAR *pData, HINT32 nDataSize)
{
	HERROR	hErr = ERR_OK;

	if(NULL == pData || 0 == nSock)
	{
		HxLOG_Error("Error> nSock = %d, pData = 0x%x, nDataSize = %d\n", nSock, pData, nDataSize);
		return ERR_FAIL;
	}

	hErr = HLIB_SOCKET_Write(nSock, pData, nDataSize, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error> Fail Socket write\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HUINT8 *svc_MediaServer_GetRingBufData(Handle_t hRingBuf, Handle_t hMsgId, HINT32 nIndex, HUINT32 *pulReadDataSize)
{
	HUINT8 				*pReadData = NULL;
	HUINT8				*pDataSizePos = NULL;
	HUINT32				ulReadDataSize = 0;
	HINT32				nAddPos = 0;
	HERROR				hErr = ERR_OK;
	HINT32				nUnreadSize = 0;
	HxRingBuf_Status_t	stRingbufStatus;

	HxSTD_MemSet(&stRingbufStatus, 0, sizeof(HxRingBuf_Status_t));

	hErr = PAL_RINGBUF_GetBufferingStatus(hRingBuf, &stRingbufStatus);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Error> PAL_RINGBUF_GetBufferingStatus failed[hRingBuf:%d]!\r\n", hRingBuf);
		return NULL;
	}

	if(BASIC_RTP_PACKET_UNIT_SIZE > stRingbufStatus.ulUnreadSize)
	{
		return NULL;
	}

	pReadData = HLIB_MEM_Calloc(stRingbufStatus.ulUnreadSize + 512 /* dummy */);
	if(NULL == pReadData)
	{
		HxLOG_Error("out of mem!!\r\n");
		return NULL;
	}

	HxSTD_MemCopy((pReadData+nAddPos), &hMsgId, 4);
	nAddPos += 4;

	HxSTD_MemCopy((pReadData+nAddPos), &nIndex, 4);
	nAddPos += 4;

	pDataSizePos = pReadData+nAddPos;
	nAddPos += 4;

	nUnreadSize = (stRingbufStatus.ulUnreadSize - (stRingbufStatus.ulUnreadSize % BASIC_RTP_PACKET_UNIT_SIZE));

	hErr = PAL_RINGBUF_ReadData(hRingBuf, nUnreadSize, (pReadData+nAddPos), &ulReadDataSize);
	if(ERR_OK != hErr)
	{
		HxLOG_Warning("Ringbuf check (%d)!!\r\n", hRingBuf);
		HLIB_STD_MemFree(pReadData);
		pReadData = NULL;
	}
	nAddPos += ulReadDataSize;

	HxSTD_MemCopy((pDataSizePos), &ulReadDataSize, 4);

	*pulReadDataSize = (nAddPos);

	HxLOG_Print("[READ OBAMA RINGBUF] nUnreadSize = %d, stRingbufStatus.ulUnreadSize = %d, ulReadDataSize = %d, *pulReadDataSize = %d \r\n",
		nUnreadSize, stRingbufStatus.ulUnreadSize, ulReadDataSize, *pulReadDataSize);

	return pReadData;
}

STATIC void svc_MediaServer_ReleaseRingBufData(HUINT8 *pData)
{
	if(pData)
	{
		HLIB_STD_MemFree(pData);
		pData = NULL;
	}
}


STATIC void svc_MediaServer_Task(void *pvArg)
{
	UNIXTIME 			utCur = 0;
	HERROR				hErr = ERR_OK;
	HINT32				nSock = 0;
	HUINT32				ulLoop = 0;
//	HUINT32				ulFindRingBufLoop = 0;
	Handle_t			hRingbuf = HANDLE_NULL;
	Handle_t			hMsgId = HANDLE_NULL;
	HUINT8 				*pReadData = NULL;
	HINT32				nIndex = 0;
	HUINT32 			ulReadDataSize = 0;
	SVC_MediaServer_Msg_t stMsg;
#ifdef CONFIG_DEBUG
	HUINT32				ulCurTick = 0, ulTimeGap = 0;
	HINT32				nKbps = 0;
#endif

	while(0 == nSock)
	{
		nSock = svc_MediaServer_CreateSocket();
		if(0 == nSock)
		{
			VK_TASK_Sleep(100);
			HxLOG_Error("Fail svc_MediaServer_CreateSocket\n");
		}
	}

	while(1)
	{
		utCur = HLIB_STD_GetSystemTime();
		NOT_USED_PARAM(utCur);

		HxSTD_MemSet(&stMsg, 0, sizeof(SVC_MediaServer_Msg_t));

		hErr = VK_MSG_ReceiveTimeout(s_ulMediaServerMsgId, &stMsg, sizeof(SVC_MediaServer_Msg_t), 500);

		for (ulLoop = 0; ulLoop < SVC_MEDIASERVER_INST_NUM; ulLoop++)
		{
			hRingbuf = HANDLE_NULL;
			pReadData = NULL;
			ulReadDataSize = 0;

			ENTER_GSEM;
			if (TRUE == s_astMediaServerInstance[ulLoop].bExecuting)
			{
				hRingbuf = s_astMediaServerInstance[ulLoop].hRingBuf;
				hMsgId = s_astMediaServerInstance[ulLoop].hMsgId;
				nIndex = s_astMediaServerInstance[ulLoop].nIndex;
			}
			LEAVE_GSEM;

			if(HANDLE_NULL == hRingbuf || 0 == hRingbuf)
			{
				continue;
			}

			pReadData = svc_MediaServer_GetRingBufData(hRingbuf, hMsgId, nIndex, &ulReadDataSize);
			if(NULL == pReadData)
			{
				continue;
			}

			hErr = svc_MediaServer_DataTransfer(nSock, pReadData, ulReadDataSize);

#ifdef CONFIG_DEBUG
			/* check kbps of writing to homma */
			ulCurTick = HLIB_STD_GetSystemTick();
			ulTimeGap = ulCurTick - s_aulLastWriteTick[ulLoop];
			if(0 >= ulTimeGap)
			{
				ulTimeGap = 1;
			}
			nKbps = (ulReadDataSize / ulTimeGap) * 1000 / 1024; /* kbps */

			HxLOG_Print(HxANSI_COLOR_YELLOW("\n\t(2)[DATA_TRACE][OBAMA>SEND(%d)] size(%d) (%d kbps) hMsgId(0x%x), tick = %d, gap = %d\r\n"), hRingbuf, ulReadDataSize, nKbps, hMsgId, ulCurTick, ulTimeGap);
			s_aulLastWriteTick[ulLoop] = ulCurTick;
#endif

			if(ERR_OK == hErr)
			{
				ENTER_GSEM;
				if(hRingbuf == s_astMediaServerInstance[ulLoop].hRingBuf
					&& hMsgId ==s_astMediaServerInstance[ulLoop].hMsgId )
				{
					s_astMediaServerInstance[ulLoop].nIndex++;
				}
				LEAVE_GSEM;
			}
			else
			{
				HxLOG_Error("svc_MediaServer_DataTransfer error\r\n");
				ENTER_GSEM;
				if (TRUE == s_astMediaServerInstance[ulLoop].bExecuting)
				{
					s_astMediaServerInstance[ulLoop].nErrCnt++;
					if(10 < s_astMediaServerInstance[ulLoop].nErrCnt)
					{
						HxLOG_Error("\r\n");
						HxLOG_Error("############################################################!\r\n");
						HxLOG_Error("\r\n");
						HxLOG_Error("############################################################!\r\n");
						HxLOG_Error("delete error or zombie instance!!\r\n");
						HxLOG_Error("-------------------------------info-------------------------\r\n");
						HxLOG_Error("       ulInstIndex  : %d\r\n", ulLoop);
						HxLOG_Error("       ulPacketIndex: %d\r\n", s_astMediaServerInstance[ulLoop].nIndex);
						HxLOG_Error("       ulSlot       : %d\r\n", s_astMediaServerInstance[ulLoop].ulSlot);
						HxLOG_Error("       hRingBuf     : %d\r\n", s_astMediaServerInstance[ulLoop].hRingBuf);
						HxLOG_Error("############################################################!\r\n");
						HxLOG_Error("\r\n");

						svc_MediaServer_DeleteInstance(&s_astMediaServerInstance[ulLoop]);
					}
				}
				LEAVE_GSEM;
			}

			svc_MediaServer_ReleaseRingBufData(pReadData);
		}
	}

	if(nSock != 0)
	{
		svc_MediaServer_DestroySocket(nSock);
	}

	return;
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

void   SVC_MediaServer_RINGBUF_Signal(HxRingBuf_SignalType_e eType, Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus)
{
	SVC_MediaServer_Msg_t stMsg;

	stMsg.eType 	= eType;
	stMsg.hRingBuf 	= hRingBuf;
	if(NULL != pstStatus)
	{
		HxSTD_MemCopy(&stMsg.stStatus, pstStatus, sizeof(HxRingBuf_Status_t));
	}
	else
	{
		HxSTD_MemSet(&stMsg.stStatus, 0, sizeof(HxRingBuf_Status_t));
	}

	switch(eType)
	{
	case eHxRINGBUF_SIG_Write:
		VK_MSG_Send(s_ulMediaServerMsgId, &stMsg, sizeof(SVC_MediaServer_Msg_t));
		break;

	default:
		break;
	}
}

HERROR SVC_MediaServer_Watchdog_Add(Handle_t hRingBuf, DxSchedule_t *pstSchedule)
{
	HERROR 		hErr = ERR_OK;

	SVC_MediaServer_Instance_t	*pstInst = NULL;

	ENTER_FUNCTION;

	if(0 == s_ulMediaServerSemId)
	{
		hErr = svc_MediaServer_Init();
		if(ERR_OK != hErr)
		{
			HxLOG_Error("Error> Init fail\n");
			return ERR_FAIL;
		}
	}

	if(NULL == pstSchedule || HANDLE_NULL == hRingBuf || 0 == hRingBuf)
	{
		HxLOG_Error("Error> Invalid Argument!!, hRingBuf(0x%x) pstSchedule(0x%x)\n", hRingBuf, pstSchedule);
		return ERR_FAIL;
	}

	switch(pstSchedule->eRsvType)
	{
	case DxRSVTYPE_NETSTREAMING_LIVE:
	case DxRSVTYPE_NETSTREAMING_FILE:
		break;

	default:
		HxLOG_Error("Error> check pstSchedule->eRsvType(0x%x)\n", pstSchedule->eRsvType);
		return ERR_FAIL;
	}

	ENTER_GSEM;

	pstInst = svc_MediaServer_GetEmptyInstance();
	if(NULL == pstInst)
	{
		HxLOG_Error("Error> instance count check!!\n");
		LEAVE_GSEM;
		return ERR_FAIL;
	}

	pstInst->bExecuting 	= TRUE;
	pstInst->hRingBuf 		= hRingBuf;
	pstInst->hMsgId			= pstSchedule->uExtInfo.stStrm.ulMsgId;
	pstInst->ulSlot			= pstSchedule->ulSlot;
	pstInst->utLastUpdate	= HLIB_STD_GetSystemTime();
	HxSTD_MemCopy(&pstInst->stRsvStreaming, &pstSchedule->uExtInfo.stStrm, sizeof(DxRSV_Streaming_t));

	LEAVE_GSEM;

	HxLOG_Print("Insert> hRingBuf(%d) pstInst(0x%x)!!\n", hRingBuf, pstInst);


	LEAVE_FUNCTION;

	return ERR_OK;
}


HERROR SVC_MediaServer_Watchdog_UpdateMsgIdByRingbuf(Handle_t hRingBuf, DxSchedule_t *pstSchedule)
{
	SVC_MediaServer_Instance_t *pstInst = NULL;

	ENTER_FUNCTION;

	if(NULL == pstSchedule)
	{
		return ERR_FAIL;
	}

	ENTER_GSEM;

	pstInst = svc_MediaServer_GetInstanceByRingbuf(hRingBuf);
	if(NULL != pstInst)
	{
		pstInst->hMsgId 		= pstSchedule->uExtInfo.stStrm.ulMsgId;
		HxSTD_MemCopy(&pstInst->stRsvStreaming, &pstSchedule->uExtInfo.stStrm, sizeof(DxRSV_Streaming_t));
	}
	LEAVE_GSEM;

	HxLOG_Print("Update>  hRingBuf(%d) pstInst(0x%x)!!\n", hRingBuf, pstInst);

	LEAVE_FUNCTION;

	return ERR_OK;
}


HERROR SVC_MediaServer_Watchdog_RemoveByRingbuf(Handle_t hRingBuf)
{
	SVC_MediaServer_Instance_t *pstInst = NULL;

	ENTER_FUNCTION;

	ENTER_GSEM;

	pstInst = svc_MediaServer_GetInstanceByRingbuf(hRingBuf);
	if(NULL != pstInst)
	{
		svc_MediaServer_DeleteInstance(pstInst);
	}
	LEAVE_GSEM;

	HxLOG_Print("Delete>  hRingBuf(%d) pstInst(0x%x)!!\n", hRingBuf, pstInst);

	LEAVE_FUNCTION;

	return ERR_OK;
}



HERROR SVC_MediaServer_Watchdog_RemoveBySamaSlot(HINT32 nSamaSlot)
{
	SVC_MediaServer_Instance_t *pstInst = NULL;

	ENTER_FUNCTION;

	ENTER_GSEM;

	pstInst = svc_MediaServer_GetInstanceBySlot(nSamaSlot);
	if(NULL != pstInst)
	{
		svc_MediaServer_DeleteInstance(pstInst);
	}
	LEAVE_GSEM;

	HxLOG_Print("Delete>  nSamaSlot(%d) pstInst(0x%x)!!\n", nSamaSlot, pstInst);

	LEAVE_FUNCTION;

	return ERR_OK;
}



/*********************** End of File ******************************/
