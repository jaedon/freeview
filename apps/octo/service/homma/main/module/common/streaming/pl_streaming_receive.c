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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "int_streaming.h"
#include "int_include/int_pl_streaming_resource.h"

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
/* pl_streaming_receive.c와 svc_media server 동시에 변경해야 함. */
#define	HOMMA_SERVER_PORT		30991

#define RINGBUF_DUMP_FILENAME	"/receive_ringbuf.ts"

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

STATIC HLONG							s_ulPLStreamingRecvSemId = 0;
STATIC HLONG							s_ulPLStreamingRecvTaskId = 0;
#if defined(CONFIG_DEBUG)
STATIC HBOOL							s_bDumpRingbuf = FALSE;
#endif

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/


STATIC HINT32 _plStreaming_Receive_CreateSocket(void)
{
	return HLIB_EXTRA_OpenServer("hommastreaming");
}

STATIC void _plStreaming_Receive_DestroySocket(HINT32 nSock)
{
	(void)HLIB_SOCKET_Close(nSock);
}

STATIC HERROR _plStreaming_Receive_Data(HINT32 nSock, HCHAR *pData, HINT32 nDataSize)
{
	HERROR	hErr = ERR_OK;

	hErr = HLIB_SOCKET_Write(nSock, pData, nDataSize, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail Socket write\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static void		 _plStreaming_Receive_onPacketReceived(void *userData, HINT32 nSocket, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	HUINT8				*pucData = (HUINT8 *)pvData;
	HUINT32				ulWrittenSize = 0;
	Handle_t			hRingBuf = HANDLE_NULL;
	HERROR 				hErr = ERR_OK;
	HINT32				nAddPos = 0;
	HINT32				nIndex = 0;
	HINT32				nDataSize = 0;
    _plStreamResInst_t* pResInst;

	HxSTD_memcpy(&hRingBuf, (pucData+nAddPos), 4);
	nAddPos += 4;

	if(HANDLE_NULL == hRingBuf || 0 == hRingBuf)
	{
        HxLOG_Error("Invalid ringbuf [sock:%d, req:%d, size:%d]\r\n", nSocket, ulRequestId, nSize);
		return;
	}

	HxSTD_memcpy(&nIndex, (pucData+nAddPos), 4);
	nAddPos += 4;

	HxSTD_memcpy(&nDataSize, (pucData+nAddPos), 4);
	nAddPos += 4;

	if(nDataSize != (nSize - nAddPos))
	{
		HxLOG_Error("Error> [hRingBuf:0x%x] Unmatched data size check!!!!!!!!   nDataSize(%d) receive size(%d)\r\n", hRingBuf, nDataSize, (nSize - nAddPos));
		HxLOG_Error("data size check!!!!!!!!   nDataSize(%d) receive size(%d)\r\n", nDataSize, (nSize - nAddPos));
		HxLOG_Error("data size check!!!!!!!!   nDataSize(%d) receive size(%d)\r\n", nDataSize, (nSize - nAddPos));
		HxLOG_Error("data size check!!!!!!!!   nDataSize(%d) receive size(%d)\r\n", nDataSize, (nSize - nAddPos));
		HxLOG_Error("data size check!!!!!!!!   nDataSize(%d) receive size(%d)\r\n", nDataSize, (nSize - nAddPos));
		HxLOG_Error("data size check!!!!!!!!   nDataSize(%d) receive size(%d)\r\n", nDataSize, (nSize - nAddPos));
		HxLOG_Error("data size check!!!!!!!!   nDataSize(%d) receive size(%d)\r\n", nDataSize, (nSize - nAddPos));
	}

	HxLOG_Debug(HxANSI_COLOR_GREEN("\n\t(3)[DATA_TRACE][HOMMA>RECEIVED(0x%x)] nIndex(%d) nSize(%d, %dK) Tick(%d)\r\n"), hRingBuf, nIndex, nDataSize, (nDataSize/1024), HLIB_STD_GetSystemTick());


#ifdef CONFIG_DEBUG
	if(TRUE == s_bDumpRingbuf)
	{	// dump ringbuf
		HxFILE_t	fp = NULL;

		fp = HLIB_FILE_Open(RINGBUF_DUMP_FILENAME, "ab");
		if(fp)
		{
            /* 2014/423 mhkang: 아래처럼하면 cmduartconsole/homma/streaming/dump_ringbuf 명령으로 수신하는 TS를 dump했을 때 TS가 짤린다.
			HLIB_FILE_Write(fp, pucData+DxRSV_STREAMKEY_LEN, (nSize-DxRSV_STREAMKEY_LEN), 1);
            */
            HLIB_FILE_Write(fp, (pucData+nAddPos), (nDataSize), 1);
			HLIB_FILE_Close(fp);
		}
	}
#endif

    pResInst = PL_Streaming_Resource_GetInstanceByRingBuf(hRingBuf);
    if (pResInst == NULL)
    {
        HxLOG_Warning("Cannot find ResourceInstance(hRingBuf:%d)\n",hRingBuf);
        return;
    }

    if (pResInst->bUseRingBuf == TRUE)  // 2015/2/2 mhkang:
    {
        hErr = PL_RINGBUF_WriteData(hRingBuf, (nDataSize), (pucData+nAddPos), &ulWrittenSize);
        if ((ERR_OK != hErr) || (nDataSize != ulWrittenSize))
        {
            HxLOG_Error("PL_RINGBUF_WriteData(hRingBuf:0x%x) failed(err:%d)! [reqid:%d, size:%d, dataSize:%d, WrittenSize:%ld]\n",
                        hRingBuf, hErr, ulRequestId,
                        nSize, nDataSize, ulWrittenSize);
        }
    }
 }

static void		 _plStreaming_Receive_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	switch (eStatus)
	{
	case HxSOCKET_ADD_CLIENT:
		HxLOG_Print("HxSOCKET_ADD_CLIENT\r\n");
		break;

	case HxSOCKET_REMOVE_CLIENT:
		HxLOG_Print("HxSOCKET_REMOVE_CLIENT\r\n");
		break;

	case HxSOCKET_DISCONNECT_CLIENT:
		HxLOG_Print("HxSOCKET_DISCONNECT_CLIENT\r\n");
		break;

	default:
		break;
	}
}


STATIC void _plStreaming_Receive_Task(void *pvArg)
{
	HINT32				nSock = 0;

    HxLOG_Error("[INF] +\n");

	nSock = _plStreaming_Receive_CreateSocket();

	/* 2015/7/22 mhkang:
	nSock 값은 실제 HxSocket_Server_t 또는 HxSocket_t 구조체 포인터가 넘어온다.
	그런데 HINT32 으로 casting되어 넘어오는데 mips계열은 양수로 리턴되는데 arm 계열에서는 음수로 넘어오고 있다.
	음수가 에러는 아니지만  HINT32으로 잘못 casting되어 넘어오고 있다. */
	if(0 == nSock)
	{
		HxLOG_Error(HxANSI_COLOR_GREEN("Error> _plStreaming_Receive_CreateSocket failed(nSock:%d, errno:%d)\n"), nSock, errno);
		return;
	}

	while(1)
	{
		HLIB_SOCKET_Listen(nSock, TRUE, _plStreaming_Receive_onPacketReceived, _plStreaming_Receive_onStatusChanged, NULL);
		VK_TASK_Sleep(1); /* Already socket blocked with SOCKET_FOREVER */
	}

	if(0 == nSock)
	{
		_plStreaming_Receive_DestroySocket(nSock);
	}

    HxLOG_Error("[INF] -\n");
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


#if 0
{
	PL_Streaming_Context_FindByStreamingKey 	=> in : streamingkey,  ret : Streamingid;
	PL_Streaming_Context_GetRingbufHandle		=> in : streaming id,  ret : ringbuf handle

	PL_RINGBUF_WriteData(ringbuf handle, HUINT32 ulDataSize,HUINT8 * pucData,HUINT32 * pulWrittenSize);
}

#endif


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Receive_Init(void)
{
	HOM_Result_e  	eHomResult = eHOM_Result_Ok;
	HERROR			hErr = ERR_OK;

	ENTER_FUNCTION;

	hErr = VK_SEM_Create (&s_ulPLStreamingRecvSemId, "StreamRes_GSem", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_SEM_Create Semaphore failed!!!\n");
		return eHOM_Result_Error_InitFail;
	}

	hErr = VK_TASK_Create (_plStreaming_Receive_Task, VK_TASK_PRIORITY_MW_REF, SIZE_16K, "PLStreamingRecvTask", NULL, &s_ulPLStreamingRecvTaskId, 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_TASK_Create error:\n");
		return eHOM_Result_Error_InitFail;
	}

	hErr = VK_TASK_Start(s_ulPLStreamingRecvTaskId);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("VK_TASK_Start error:\n");
		return eHOM_Result_Error_InitFail;
	}

	VK_TASK_Sleep(0);


	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_Receive_DeInit(void)
{
	ENTER_FUNCTION;

	LEAVE_FUNCTION;

	return ;
}



/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/

#if defined(CONFIG_DEBUG)
STATIC HINT32   _plcmd_streaming_Ringbuf_dump(void *szArgList)
{
	s_bDumpRingbuf = (s_bDumpRingbuf == TRUE) ? FALSE : TRUE;
	if(TRUE == s_bDumpRingbuf)
	{
		HLIB_CMD_Printf("ringbuf dump enabled\n");
		HLIB_FILE_Delete(RINGBUF_DUMP_FILENAME);
	}
	else
	{
		HLIB_CMD_Printf("ringbuf dump disabled\n");
	}

	return HxCMD_OK;
}
#endif

extern void	HOMMA_CMDRegister_StreamingReceive(const HCHAR *hCmdHandle)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord( hCmdHandle, _plcmd_streaming_Ringbuf_dump, "dump_ringbuf", "toggle true/false", "received ringbuf dump");
#endif
}



