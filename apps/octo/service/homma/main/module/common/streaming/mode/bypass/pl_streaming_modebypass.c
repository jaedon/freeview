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
#include "../../int_streaming.h"

#include <sapi.h>
/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/

typedef enum _plStreaming_M3U8Type_e
{
	plStreaming_M3U8Type_None = 0,

	plStreaming_M3U8Type_Root,

	plStreaming_M3U8Type_Bitrate,

	plStreaming_M3U8Type_Max
} plStreaming_M3U8Type_e;


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
#define __COMMON_FUNCTION__
STATIC HINT32 _plstreaming_ModeByPass_WaitLocked(PLStreaming_ModeInst_t *pstModeIns, HUINT32 ulMaxLoop);


STATIC void _plstreaming_ModeByPass_CreateDumpFile(PLStreaming_ModeInst_t *pstInst, Handle_t hRingbuf, const HCHAR *pRequestPath)
{
#if defined(CONFIG_DEBUG)
	PLStreaming_ModeByPass_t 	*pByPass = NULL;

	pByPass 	= (&pstInst->Mode.stByPass);


	if(ePLStreaming_OutType_DumpFile & pstInst->stSetupInfo.eOutType)
	{
		if(NULL != pByPass->pDumpFile)
		{
			HLIB_FILE_Close(pByPass->pDumpFile);
		}

		if(NULL == pByPass->pDumpFile)
		{
			HCHAR pszFileName[256] = {0, };

			HxSTD_snprintf(pszFileName, 255, "NTLS_%04lld_%05d_%08x.ts", pstInst->stSetupInfo.uUID, hRingbuf, HLIB_STD_GetSystemTick());

			pByPass->pDumpFile = HLIB_FILE_Open(pszFileName, "wb");

			PL_Streaming_Debug_Print("[%s:%d] pszFileName(%s) pRequestPath(%s)\r\n", __FUNCTION__, __LINE__, pszFileName, pRequestPath);
		}
	}
#endif
}

STATIC HOM_Result_e _plstreaming_ModeByPass_GetUID(const PLStreaming_StartInfo_t *pstStartInfo, HUID *ret_puUID)
{
	HCHAR	*pUIDStr = NULL;
	HUID	uUID = 0;

	pUIDStr = HLIB_STD_StrCaseStr(pstStartInfo->pszStreamingPath, PL_STREAMING_ByPass_DEFAULT_CHANNEL_PREFIX);
	if(NULL != pUIDStr)
	{
		uUID = HLIB_STD_StrToINT32(pUIDStr);
		*ret_puUID = uUID;
		return eHOM_Result_Ok;
	}

	*ret_puUID = 0;

	return eHOM_Result_Error;
}

STATIC Handle_t _plstreaming_ModeByPass_CreateRingbuf(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath)
{
	Handle_t			hRingbuf = HANDLE_NULL;
	HINT32				nMaxSize = 0;
	HxRingBuf_Setup_t	stRingbufSetup;

	HxSTD_memset(&stRingbufSetup, 0, sizeof(HxRingBuf_Setup_t));

	nMaxSize = PL_Streaming_Spec_ByPass_GetBufferSize();
	if(0 >= nMaxSize)
	{
		nMaxSize = (188 * 7 * 3000); // about 3.9 MB
	}

	stRingbufSetup.eBufType 		= eHxRINGBUF_TYPE_LINEAR;
	stRingbufSetup.ulMaxSize		= nMaxSize;
	stRingbufSetup.ulChunkSize		= 0;
	stRingbufSetup.bBlockOverflow	= FALSE;			/* true:overflow시 write block 됨. */
	stRingbufSetup.eRuleAfterRead	= eHxRINGBUF_READRULE_REMOVE;

	hRingbuf = PL_RINGBUF_Create(&stRingbufSetup);

	_plstreaming_ModeByPass_CreateDumpFile(pstInst, hRingbuf, pRequestPath);

	return hRingbuf;
}

STATIC HINT32	_plstreaming_ModeByPass_GetMinReadSize(PLStreaming_ModeInst_t *pstInst)
{
	HINT32		nMinReadSize = 0;

#if 0
	{
		HINT32		i = 0;


		for(i=0; i<pstInst->nPIDsCount; i++)
		{
			if(pstInst->usPIDs[i] < 100)
			{
				// section
				nMinReadSize += 0;
			}
			else
			{
				// av
				nMinReadSize += (1024 * 64);
			}
		}
	}
#else
	nMinReadSize = 0;
#endif


	return nMinReadSize;
}

STATIC HINT32 _plstreaming_ModeByPass_WaitRingbuf(Handle_t hResHandle, Handle_t hRingbuf, HINT32 nMinReadSize, HINT32 nMaxLoop)
{
	HINT32						nLoopCnt = 0;
	HINT32						nUnreadSize = 0;
	HERROR						hErr = ERR_OK;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
	HxRingBuf_Status_t			stStatus;
	PLStreaming_SetupStatus_e 	eSetupStatus = ePLStreaming_SetupStatus_None;
	HINT32 nMaxReadSize = 188 * 3000;  // 2014/10/11 mhkang: 최대 전송 크기 564kbyte 제한

	while(1)
	{
		HxSTD_MemSet(&stStatus, 0, sizeof(HxRingBuf_Status_t));

		eHomResult = PL_Streaming_Resource_GetStatus(hResHandle, &eSetupStatus);
		if( eHOM_Result_Ok != eHomResult || ePLStreaming_SetupStatus_Conflict == eSetupStatus)
		{
			HxLOG_Error("PL_Streaming_Resource_GetStatus error!!\r\n");
			return 0;
		}

		hErr = PL_RINGBUF_GetBufferingStatus(hRingbuf, &stStatus);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("PL_RINGBUF_GetBufferingStatus error!!\r\n");
			return 0;
		}

		if(stStatus.ulUnreadSize > (nMinReadSize))
		{
			if (stStatus.ulUnreadSize > nMaxReadSize)
			{
				nUnreadSize = nMaxReadSize; // 2014/10/11 mhkang: 최대크기로 제한(최대크기 제한하지 않았을때 메모리릭 발생. 원인불명. http://svn.humaxdigital.com:3000/issues/97142)
			}
			else
			{
				nUnreadSize = stStatus.ulUnreadSize;
			}
			break;
		}
		else if(nMaxLoop <= nLoopCnt)
		{
			HxLOG_Warning("PL_RINGBUF_GetBufferingStatus not data!!\r\n");
			return 0;
		}
		else
		{
			HxLOG_Debug("nLoopCnt(%d)\r\n", nLoopCnt);
			VK_TASK_Sleep(50);
		}
		nLoopCnt++;
	}

	PL_Streaming_Debug_Print("[%s:%d] hRingbuf(0x%x) nLoopCnt(%d) nUnreadSize(%d)\r\n", __FUNCTION__, __LINE__, hRingbuf, nLoopCnt, nUnreadSize);

	return nUnreadSize;
}

STATIC HOM_Result_e _plstreaming_ModeByPass_ReadRingbuf(PLStreaming_ModeInst_t *pstInst, HINT32 nTsIndex, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	HERROR						hErr = ERR_OK;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
	HINT32						ulDataSize = 0;
	HUINT8						*pucData = NULL;
	HUINT32						ulUnreadSize = 0;
	HINT32						nMinReadSize = 0;
	PLStreaming_ModeByPass_t 	*pByPass = NULL;

	pByPass		= (&pstInst->Mode.stByPass);

	if(0 == ulUnreadSize)
	{
		ulUnreadSize = _plstreaming_ModeByPass_WaitRingbuf(pstInst->hResHandle, pstInst->hRingbuf, nMinReadSize, 10); // 2015/3/6 mhkang: ( (10 x 50 msec) + mxlib:50 msec) x 100  loop를 돌게 된다.
		/*  2015/3/16: mhkang: http://svn.humaxdigital.com:3000/issues/101339
		if(0 == ulUnreadSize)
		{
			HxLOG_Warning("_plstreaming_ModeByPass_WaitRingbuf not data!!, hRingbuf(0x%x) nTsIndex(%d) status(%s)\r\n",
				pstInst->hRingbuf, nTsIndex, PL_Streaming_Debug_GetSetupStatusStr(pstInst->eSetupStatus));
			return eHOM_Result_Error_Resource_Notfound;
		}
		*/
	}

	if (ulUnreadSize > 0)
	{
		pucData = HLIB_STD_MemAlloc(ulUnreadSize);
		if(NULL == pucData)
		{
			HxLOG_Error("eHOM_Result_Error_OutofMem error!!\r\n");
			return eHOM_Result_Error_OutofMem;
		}

		hErr = PL_RINGBUF_ReadData(pstInst->hRingbuf, ulUnreadSize, pucData, &ulDataSize);
		if(ERR_OK != hErr || 0 == ulDataSize)
		{
			HLIB_STD_MemFree(pucData);
			HxLOG_Error("PL_RINGBUF_ReadData error!!\r\n");
			return eHOM_Result_Error_Resource_Notfound;
		}
	}
	else
	{
		// 2015/3/16 mhkang: http://svn.humaxdigital.com:3000/issues/101339
		ulDataSize = 188;

		pucData = HLIB_STD_MemAlloc(ulDataSize);
		HxSTD_MemSet(pucData, 0, ulDataSize);

		// TS NULL Packet
		pucData[0] = 0x47;  // Sync Byte : 0x47
		pucData[1] = 0x1F;  //
		pucData[2] = 0xFF;  // PID = 8191 (0x1ff)
		pucData[3] = 0x20;  // [00] [10]  [0000] : scramblingControl=00 AdaptionFieldControl=10,  ContinuityCount=0000
	}


#ifdef CONFIG_DEBUG
{
	HUINT32		ulCurTick = 0, ulTimeGap = 0;
	HINT32		nKbps = 0;
	HUINT32		ulTotalDur = 0;
	HUINT32		ulHour = 0, ulMin = 0, ulSecond = 0;

	/* check kbps of writing to homma */
	ulCurTick = HLIB_STD_GetSystemTick();
	ulTimeGap = ulCurTick - pByPass->ulLastReadTick;
	if(0 >= ulTimeGap)
	{
		ulTimeGap = 1;
	}
	nKbps = (ulDataSize / ulTimeGap) * 1000 / 1024; /* kbps */

	ulTotalDur = (ulCurTick - pByPass->ulStartReadTick) / 1000;
	ulHour = ulTotalDur / 3600;
	ulMin = (ulTotalDur - (ulHour * 3600)) / 60;
	ulSecond = ulTotalDur - (ulHour * 3600) - (ulMin * 60);

	HxLOG_Debug(HxANSI_COLOR_YELLOW("dur = %02d:%02d:%02d\n\t(4)[DATA_TRACE][HOMMA>READ FROM RINGBUF(0x%x)] ulDataSize(%d, %d kbps), tick = %d, gap = %d\r\n")
									, ulHour, ulMin, ulSecond
									, pstInst->hRingbuf, ulDataSize, nKbps, ulCurTick, ulTimeGap);
	pByPass->ulLastReadTick = ulCurTick;
}
//	HxLOG_Print("nMinReadSize(%d) ulDataSize(%d) ulUnreadSize(%d) nTsIndex(%d) status(%s)\r\n",
//		nMinReadSize, ulDataSize, ulUnreadSize, nTsIndex, PL_Streaming_Debug_GetSetupStatusStr(pstInst->eSetupStatus));
#endif


	if(eHOM_Result_Ok == eHomResult )
	{
		if(NULL != pByPass->pDumpFile)
		{
			HLIB_FILE_Write(pByPass->pDumpFile, pucData, ulDataSize, 1);
		}
	}

	*pulDataSize 	= ulDataSize;
	*ppucData 		= pucData;

	return eHomResult;
}

STATIC void _plstreaming_ModeByPass_DestroyRingbuf(PLStreaming_ModeInst_t *pstInst)
{
	PLStreaming_ModeByPass_t 	*pByPass = NULL;

	pByPass 	= (&pstInst->Mode.stByPass);

	if(HANDLE_NULL != pstInst->hRingbuf && 0 != pstInst->hRingbuf)
	{
		PL_RINGBUF_Destroy(pstInst->hRingbuf);
		pstInst->hRingbuf = HANDLE_NULL;
	}

	if(NULL != pByPass->pDumpFile)
	{
		HLIB_FILE_Close(pByPass->pDumpFile);
		pByPass->pDumpFile = NULL;
	}

}

STATIC void _plstreaming_ModeByPass_MakeDisplayString(PLStreaming_ModeInst_t *pstInst)
{
	HCHAR			szDisplayString[128] = {0, };

	if(NULL != pstInst->stSetupInfo.pszIPAddress)
	{
		HxSTD_snprintf(szDisplayString, 127, "NTLS (%s)", pstInst->stSetupInfo.pszIPAddress);
	}
	else
	{
		HxSTD_snprintf(szDisplayString, 127, "NTLS (%d)", pstInst->hRingbuf);
	}


	if(NULL != pstInst->stSetupInfo.pszDisplayString)
	{
		HLIB_STD_MemFree(pstInst->stSetupInfo.pszDisplayString);
		pstInst->stSetupInfo.pszDisplayString = NULL;
	}

	pstInst->stSetupInfo.pszDisplayString = HLIB_STD_StrDup(szDisplayString);
}

STATIC HOM_Result_e _plstreaming_ModeByPass_DeleteInstance(PLStreaming_ModeInst_t *pstModeInst)
{
	PLStreaming_ModeByPass_t	*pByPass = NULL;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;

	if (pstModeInst == NULL)
	{
		HxLOG_Error("Invalid args(pstModeInst is null)\n");
		return eHOM_Result_Error_Invalid_Arguments;
	}

	pByPass		= (&pstModeInst->Mode.stByPass);

	if(FALSE == pstModeInst->stSetupInfo.bNotUseResource)
	{
		_plstreaming_ModeByPass_DestroyRingbuf(pstModeInst);

		// TODO: 필요한 메모리 해제
		PL_StreamingKey_Delete(pstModeInst->pstStreamingKey);
		pstModeInst->pstStreamingKey = NULL;

		HxSTD_MemSet(pByPass, 0, sizeof(PLStreaming_ModeByPass_t));
	}

	return eHomResult;
}


STATIC void 	_plstreaming_ModeByPass_Stop(PLStreaming_ModeInst_t *pstModeInst)
{
	PL_Streaming_Resource_Delete(pstModeInst->hResHandle, TRUE);
	pstModeInst->hResHandle = HANDLE_NULL;

	pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_None;
}

STATIC HOM_Result_e 	_plstreaming_ModeByPass_Start(PLStreaming_ModeInst_t *pstModeInst)
{
	HOM_Result_e  	eHomResult = eHOM_Result_Ok;
//	HINT32			isLocked;
	HINT32			nMinReadSize = 0;
	HUINT32			ulUnreadSize = 0;

	if(HANDLE_NULL == pstModeInst->hResHandle || 0 == pstModeInst->hResHandle)
	{
		pstModeInst->hResHandle = PL_Streaming_Resource_New(pstModeInst->hStreamingId);
	}

	if(HANDLE_NULL == pstModeInst->hResHandle || 0 == pstModeInst->hResHandle)
	{
		HxLOG_Error("resource handle error!!\r\n");
		return eHOM_Result_Error;
	}

	eHomResult = PL_StreamingContextUtil_MakeTuningParamAndDisplayString(pstModeInst);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("Error> PL_StreamingContextUtil_MakeTuningParamAndDisplayString\r\n");
		return eHomResult;
	}

	eHomResult = PL_Streaming_Resource_Start(pstModeInst->hResHandle, pstModeInst, 0);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("read error hRingbuf(0x%x)\r\n", pstModeInst->hRingbuf);
		return eHomResult;
	}

	pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_Start;

	nMinReadSize = _plstreaming_ModeByPass_GetMinReadSize(pstModeInst);

	HxLOG_Debug("hResHandle(0x%x) hRingbuf(0x%x) nMinReadSize(%d)\r\n", pstModeInst->hResHandle, pstModeInst->hRingbuf, nMinReadSize);



#if 1   // 2015/3/16 mhkang: crash 이슈가 있어서 #if 1 로  임시 변경함(http://svn.humaxdigital.com:3000/issues/101412)
	ulUnreadSize = _plstreaming_ModeByPass_WaitRingbuf(pstModeInst->hResHandle, pstModeInst->hRingbuf, 0, 200);
	if(0 == ulUnreadSize)
	{
		HxLOG_Error("read error _plstreaming_ModeByPass_WaitRingbuf(0x%x)\r\n", pstModeInst->hRingbuf);
		return eHOM_Result_Error_Conflict;
	}
#else
	// 2015/3/10 mhkang: PK_SYSINFO_TUNER_IsLocked 호출시 crash발생함(hdneo)
	isLocked = _plstreaming_ModeByPass_WaitLocked(pstModeInst, 60);  // 2015/3/6 mhkang: 50 msec x value(60) = 3000 msec 안에 signal locking이 되겠지?(log출력 off상태)
	if (isLocked == 0)
	{
		HxLOG_Warning("Current TP is not locked until 3000 msec\n");
		return eHOM_Result_Error_Conflict;
	}
	else if (isLocked == -1) // error
	{
		HxLOG_Error("_plstreaming_ModeByPass_WaitLocked failed\n");
		return eHOM_Result_Error;
	}
#endif
	return eHOM_Result_Ok;
}


/**
  @author mhkang2@humaxdigital.com
  @date 2015/3/6
  @brief obtain the signal locking information.
  @param[IN] pByPass
  @param[IN] ulMaxLoop  check signal locking until ulMaxLoop x 50 msec
  @retval 0 unlocked
  @retval 1 locked
  @retval -1 error
*/
STATIC HINT32 _plstreaming_ModeByPass_WaitLocked(PLStreaming_ModeInst_t *pstModeInst, HUINT32 ulMaxLoop)
{
	HINT32						nLoopCnt = 0;
	HERROR						hErr = ERR_OK;
	HBOOL						bIsLocked = FALSE;

	if (pstModeInst == NULL)
	{
		HxLOG_Error("Invalid args(pByPass is null)\n");
		return -1;
	}


	while(ulMaxLoop > nLoopCnt)
	{
		hErr = PL_COMMON_IsTunerLocked(pstModeInst->tunerid, &bIsLocked);
		if((ERR_OK == hErr) && (TRUE == bIsLocked))
		{
			HxLOG_Print(HxANSI_COLOR_YELLOW("Locked!!\n"));
			break;
		}
		else
		{
			VK_TASK_Sleep(50);
			nLoopCnt++;
			HxLOG_Print(HxANSI_COLOR_YELLOW("nLoopCnt(%d)\n"), nLoopCnt);
		}
	}

	return (bIsLocked == TRUE?1:0);
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_ModeByPass_Init(void)
{
	HOM_Result_e	eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;


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
void PL_Streaming_ModeByPass_DeInit(void)
{
	ENTER_FUNCTION;


	LEAVE_FUNCTION;
}


/**
 *
 *
 *
 * @param
 * @return
 */
PLStreaming_ModeInst_t *PL_Streaming_ModeByPass_SetupInstance(PLStreaming_ModeInst_t *pstInst)
{

	ENTER_FUNCTION;

	if(NULL == pstInst)
	{
		HxLOG_Error("Invalid_Arguments pstSetup(0x%x)\r\n", pstInst);
		return NULL;
	}

	pstInst->eSetupStatus = ePLStreaming_SetupStatus_None;

	LEAVE_FUNCTION;

	return pstInst;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_ModeByPass_DeleteInstance(PLStreaming_ModeInst_t *pstInst)
{
	ENTER_FUNCTION;

	if(NULL == pstInst)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) \r\n", pstInst);
		return ;
	}
	_plstreaming_ModeByPass_DeleteInstance(pstInst);

	LEAVE_FUNCTION;

	return ;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_ModeByPass_ResConnect (PLStreaming_ModeInst_t *pstModeInst)
{
	HOM_Result_e  	eHomResult = eHOM_Result_Ok;
	HUID			uUID = 0;
	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;
	Handle_t		hProfileHandle = HANDLE_NULL;

	ENTER_FUNCTION;

	if(NULL == pstModeInst)
	{
		HxLOG_Error("Invalid_Arguments pstSetup(0x%x)\n", pstModeInst);
		return eHOM_Result_Error;
	}

	pSetupInfo = (&pstModeInst->stSetupInfo);

	if(TRUE == pSetupInfo->bNotUseResource)
	{
		LEAVE_FUNCTION;
		return eHOM_Result_Ok;
	}

	if(NULL != pstModeInst->pstStreamingKey)
	{
		PL_StreamingKey_Delete(pstModeInst->pstStreamingKey);
	}

	// create StreamingKey
	pstModeInst->pstStreamingKey	=  PL_StreamingKey_New(pSetupInfo->pszPrefix, uUID, hProfileHandle, pSetupInfo->pszDeviceID);
	if(NULL == pstModeInst->pstStreamingKey)
	{
		HxLOG_Error(" PL_StreamingKey_New error!!\n");
		return eHOM_Result_Error;
	}

	// create RingBuf
	if(HANDLE_NULL == pstModeInst->hRingbuf || 0 == pstModeInst->hRingbuf)
	{
		pstModeInst->hRingbuf = _plstreaming_ModeByPass_CreateRingbuf(pstModeInst, pSetupInfo->pszStreamingPath);
		if(HANDLE_NULL == pstModeInst->hRingbuf || 0 == pstModeInst->hRingbuf)
		{
			HxLOG_Error("_plstreaming_ModeByPass_CreateRingbuf Fail \n");
			return eHOM_Result_Error;
		}
	}
	else
	{
		PL_RINGBUF_ClearBuffer(pstModeInst->hRingbuf);
	}

	eHomResult = _plstreaming_ModeByPass_Start(pstModeInst);
	if(eHOM_Result_Ok != eHomResult)
	{
		_plstreaming_ModeByPass_DeleteInstance(pstModeInst);
		HxLOG_Error("_plstreaming_ModeByPass_Start failed : pstSetup(0x%x)\r\n", pstModeInst);
		return eHOM_Result_Error_Conflict;
	}

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
HOM_Result_e PL_Streaming_ModeByPass_ResConflict (PLStreaming_ModeInst_t *pstInst, HINT32 eNotifyType, HINT32 nRequestPid, HCHAR *fromProcessName)
{
#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
#else
	HUINT32				ulPid = 0;
#endif
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;
	SAPI_NotifyType_e	eSapiNotifyType = (SAPI_NotifyType_e)eNotifyType;

	ENTER_FUNCTION;

	switch(eSapiNotifyType)
	{
	case eSAPI_NOTIFY_TYPE_ConflictScheduleAdded:
	case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
		if (nRequestPid != eSAPI_UNIQUE_PROCESS_Indicator_Homma)
#else
		ulPid = HLIB_STD_GetPID();
		if (ulPid != nRequestPid) // homma가 아니고,
#endif
		{
			eHomResult = eHOM_Result_Error_Conflict;
		}
		else
		{
			eHomResult = eHOM_Result_Ok;
		}

		break;

	default:
		eHomResult = eHOM_Result_Ok;
		break;
	}

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
HOM_Result_e 	PL_Streaming_ModeByPass_GetM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	/* 2014/1/16 mhkang: no use
	HINT32						ulM3U8BufSize = 0;
	HUINT8						*pM3U8Buf = NULL;
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;
	PLStreaming_ModeByPass_t		*pByPass = NULL;
	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;
	plStreaming_M3U8Type_e		eM3U8Type = plStreaming_M3U8Type_None;

	ENTER_FUNCTION;

	if(NULL == ppucData || NULL == pstInst || NULL == pRequestPath)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) ppucData(0x%x) pRequestPath(%s)\r\n", pstInst, ppucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Print("Request Streaming Path (%s)\r\n", pRequestPath);

	pByPass 		= (&pstInst->Mode.stByPass);
	pSetupInfo 	= (&pstInst->stSetupInfo);

	eM3U8Type = _plstreaming_ModeByPass_GetM3U8Type(pstInst, pRequestPath);
	switch(eM3U8Type)
	{
	case plStreaming_M3U8Type_Root:
		HxLOG_Print("plStreaming_M3U8Type_Root\r\n", __FUNCTION__, __LINE__);
		eHomResult = _plstreaming_ModeByPass_GetRootM3U8(pstInst, pRequestPath, &ulM3U8BufSize, &pM3U8Buf);
		break;

	case plStreaming_M3U8Type_Bitrate:
		HxLOG_Print("plStreaming_M3U8Type_Bitrate\r\n", __FUNCTION__, __LINE__);
		eHomResult = _plstreaming_ModeByPass_GetBitrateM3U8(pstInst, pRequestPath, &ulM3U8BufSize, &pM3U8Buf);
		break;

	default:
		pM3U8Buf = NULL;
		eHomResult = eHOM_Result_Error;
		break;
	}

	if(NULL == pM3U8Buf || eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("plz Check Arguments pstInst(0x%x) ppucData(0x%x) pRequestPath(%s)\r\n", pstInst, ppucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	*ppucData 		= pM3U8Buf;
	*pulDataSize 	= ulM3U8BufSize;

	LEAVE_FUNCTION;
  */
	return eHOM_Result_Ok;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeByPass_ReleaseM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData)
{
	ENTER_FUNCTION;

	if(NULL == pucData || NULL == pstInst || NULL == pRequestPath)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) pucData(0x%x) pRequestPath(%s)\r\n", pstInst, pucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Debug("Request Streaming Path (%s)\r\n", pRequestPath);



	HLIB_STD_MemFree(pucData);

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeByPass_GetTS(PLStreaming_ModeInst_t *pstInst,  const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	HINT32 						nTsIndex  = 0;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	if(NULL == ppucData || NULL == pulDataSize || NULL == pstInst)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) ppucData(0x%x) pRequestPath(%s) pulDataSize(0x%x)\r\n", pstInst, ppucData, pRequestPath, pulDataSize);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	nTsIndex = pstInst->Mode.stByPass.nLastReadChunkId + 1;

	eHomResult = _plstreaming_ModeByPass_ReadRingbuf(pstInst, nTsIndex, pulDataSize, ppucData);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("_plstreaming_ModeByPass_ReadRingbuf read fail pRequestPath(%s)\r\n", pRequestPath);
		return eHOM_Result_Error;
	}
	pstInst->Mode.stByPass.nLastReadChunkId = nTsIndex;

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
HOM_Result_e 	PL_Streaming_ModeByPass_ReleaseTS(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData)
{
	ENTER_FUNCTION;

	if(NULL == pucData || NULL == pstInst /*2014/1/23 mhkang:ByPass시 사용안함 || NULL == pRequestPath */)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) pucData(0x%x) pRequestPath(%s)\r\n", pstInst, pucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Debug("Request Streaming Path (%s)\r\n", pRequestPath);

	HLIB_STD_MemFree(pucData);

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeByPass_ExecCmd(PLStreaming_ModeInst_t *pstInst,  const HCHAR *pRequestPath)
{
	ENTER_FUNCTION;

	(void)pstInst;
	(void)pRequestPath;

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeByPass_GetInfo(PLStreaming_ModeInst_t *pstInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData)
{

	ENTER_FUNCTION;

	(void)pstInst;
	(void)eInfoType;
	(void)pRequestPath;
	(void)pulDataSize;
	(void)ppucData;

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeByPass_ReleaseInfo(PLStreaming_ModeInst_t *pstInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HCHAR *pucData)
{
	ENTER_FUNCTION;

	(void)pstInst;
	(void)eInfoType;
	(void)pRequestPath;
	(void)pucData;

	if(NULL != pucData)
	{
		HLIB_STD_MemFree(pucData);
	}

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeByPass_StartInfoToSetupInfo(const HCHAR *pszPrefix, const PLStreaming_StartInfo_t *pstStartInfo, PLStreaming_SetupInfo_t *ret_pstSetupInfo)
{
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;
	HUID						uUID = 0;
	PLStreaming_SetupInfo_t		*pstSetupInfo = ret_pstSetupInfo;

	ENTER_FUNCTION;

	if(NULL == pstStartInfo || NULL == pstSetupInfo || NULL == pstStartInfo->pszStreamingPath)
	{
		HxLOG_Error("Invalid_Arguments pstStartInfo(0x%x) pstSetupInfo(0x%x) \r\n", pstStartInfo, pstSetupInfo);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	eHomResult = PL_StreamingContextUtil_GetUID(pstStartInfo, &uUID);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_StreamingContextUtil_GetUID Fail \r\n");
		return eHOM_Result_Error_Invalid_Arguments;
	}

	pstSetupInfo->uUID = uUID;

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeByPass_GetProfileItemHandle(PLStreaming_ModeInst_t *pstInst, Handle_t *ret_phProfileItem)
{
	Handle_t					hProfileHandle = HANDLE_NULL;

	ENTER_FUNCTION;

	if(NULL == pstInst || NULL == ret_phProfileItem)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) ret_phProfileItem(%s)\r\n", pstInst, ret_phProfileItem);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	*ret_phProfileItem = hProfileHandle;


	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_ModeByPass_GetPIDs(PLStreaming_ModeInst_t *pstInst, HINT32 *ret_pnPIDsCount, HUINT16 *ret_arrusPIDs)
{
	ENTER_FUNCTION;

	if(NULL == pstInst || NULL == ret_pnPIDsCount || NULL == ret_arrusPIDs)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) ret_pnPIDsCount(0x%x) ret_arrusPIDs(%s)\r\n", pstInst, ret_pnPIDsCount, ret_arrusPIDs);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	*ret_pnPIDsCount = 1;
	ret_arrusPIDs[0] = 0x0000;

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}

/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/

/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_ModeByPass_Dump(PLStreaming_ModeInst_t *pstInst)
{
	if(NULL == pstInst)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) \r\n", pstInst);
		return ;
	}
	HxLOG_Print("pstInst->pstModeInst->(&pstInst->Mode.stByPass)\n");
}


