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

#define INVALID_SATIP_PID           0xFFFF
#define TUNER_STRING_BUF_LEN        1024

/* HD+ TV Guide App 과 Tivizen SAT>IP Client의 경우, 고정된 src=1를 사용하고 있다, 분당에서 사용하려면, src=3으로 강제세팅해야 함, 2014-05-12 */
#ifdef CONFIG_DEBUG
//#define _USE_HD_PLUS_APP_IN_BUNDANG_
#endif

/*
	188 * 1024 * 1 = 192512
	188 * 1024 * 2 = 385024
	188 * 1024 * 3 = 577536
	188 * 1024 * 4 = 770048
    188 * 1024 * 5 = 962560
	188 * 1024 * 6 = 1155072
	188 * 1024 * 7 = 1347584
	188 * 1024 * 8 = 1540096

	188 * 7 * 128 = 168448
*/
#define _USE_NO_WAIT_READ

#ifdef _USE_NO_WAIT_READ
#define _MAX_READ_TS_SIZE	168448
#endif

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/

/*
	ip str		: rtsp://192.168.1.202/?
	antena info : src=1&
	tp info 	: freq=11597&pol=v&msys=dvbs&mtype=qpsk&sr=22000&fec=56&
	svc info 	: pids=0,16,17,92,163,1858

	Signal source src srcID Numerical value between 1 and 255.
	Default value is "1".
*/
typedef enum _plSatipSrcType
{
	eSatipSrcType_None 			= 0,
	eSatipSrcType_LNB_ONLY		= 0x01,

	/* diseqc A~D는 spec에 명시되어 있음. sat/ip v1.2 */
	eSatipSrcType_DISEQC		= 0x00,
	eSatipSrcType_DISEQC_A		= 0x01,
	eSatipSrcType_DISEQC_B		= 0x02,
	eSatipSrcType_DISEQC_C		= 0x03,
	eSatipSrcType_DISEQC_D		= 0x04,

	/* SCD */
	eSatipSrcType_SCD			= 0x00,
	eSatipSrcType_SCD_A			= 0x01,
	eSatipSrcType_SCD_B			= 0x02,

	/* SMATV */
	eSatipSrcType_SMATV			= 0x20,

	/* MONOBLOCK LNB */
	eSatipSrcType_MONOBLOCK_LNB	= 0x30,


	eSatipSrcType_End			= 0xFF
} plSatipSrcType_e;




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
typedef struct _plStreamingModeSatIpMgr
{
	HxSTR_t		*pstM3U8Str;
	HINT32		nChListLastVerNum;
} plStreamingModeSatIpMgr_t;

STATIC	plStreamingModeSatIpMgr_t 		s_pstSatIpMgr;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_HEADER______________________
/**********************************************************************************************************/
STATIC HINT32			_plstreaming_ModeSatIp_SAT_SrcNumToEnum(DxAntInfo_t	*pstAntInfo);
//STATIC plSatipSrcType_e	_plstreaming_ModeSatIp_SAT_EnumToSrcNum(HINT32 nSrcNum, DxAntennaType_e *peAntType, HINT32 *pnInputNum);
STATIC HBOOL 			_plstreaming_ModeSatIp_ReplacePids(PLStreaming_ModeInst_t *pstInst, HINT32 nPIDsCount, HUINT16 usPIDs[]);
STATIC HINT32 			_plstreaming_ModeSatIp_GetSrcNumByTuningParam(DxTuneParam_t	*pstTuningParam);
STATIC HINT32 			_plstreaming_ModeSatIp_GetFreqByTuningParam(DxTuneParam_t	*pstTuningParam);
STATIC HOM_Result_e 	_plstreaming_ModeSatIp_Get_TunerInfo(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData);
STATIC HOM_Result_e		_plstreaming_ModeSatIp_Update_TunerInfo(PLStreaming_ModeInst_t *pstModeInst);
STATIC HBOOL	        _plstreaming_ModeSatIp_UseOnlyDefaultPids(HINT32 nPIDsCount,HUINT16 usPID[]);
STATIC HOM_Result_e     _plstreaming_ModeSatIp_UpdateSchedulePids(PLStreaming_ModeInst_t* pstInstMode);
STATIC HCHAR *			_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_CodeRate_eToString(DxSat_CodeRate_e eFec);
STATIC HCHAR *			_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_Polarization_eToString(DxSat_Polarization_e ePol);
STATIC HCHAR *			_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_TransportSpec_eToString(DxSat_TransportSpec_e eTransSpec);
STATIC HCHAR *			_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_PskModulation_eToString(DxSat_PskModulation_e ePskMod);
STATIC HCHAR *			_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_RollOff_eToString(DxSat_RollOff_e eRoll);
STATIC HCHAR *			_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_Pilot_eToString(DxSat_Pilot_e ePilot);
/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
#define __COMMON_FUNCTION__

STATIC void _plstreaming_ModeSatIp_CreateDumpFile(PLStreaming_ModeInst_t *pstInst, Handle_t hRingbuf, const HCHAR *pRequestPath)
{
#if defined(CONFIG_DEBUG)
	PLStreaming_ModeSatIp_t 	*pSatIp = NULL;

	pSatIp 		= (&pstInst->Mode.stSatIp);


	if(ePLStreaming_OutType_DumpFile & pstInst->stSetupInfo.eOutType)
	{
		if(NULL != pSatIp->pDumpFile)
		{
			HLIB_FILE_Close(pSatIp->pDumpFile);
		}

		if(NULL == pSatIp->pDumpFile)
		{
			HCHAR pszFileName[256] = {0, };

			HxSTD_snprintf(pszFileName, 255, "SATIP_%05d_%05d_%08x.ts", pstInst->stSetupInfo.stTuningParam.sat.tuningInfo.ulFrequency, hRingbuf, HLIB_STD_GetSystemTick());

			pSatIp->pDumpFile = HLIB_FILE_Open(pszFileName, "wb");

			PL_Streaming_Debug_Print("[%s:%d] pSatIp->pDumpFile(0x%x), pszFileName(%s) pRequestPath(%s)\r\n", __FUNCTION__, __LINE__, pSatIp->pDumpFile, pszFileName, pRequestPath);
			HxLOG_Print(HxANSI_COLOR_PURPLE("pSatIp->pDumpFile(0x%x) pszFileName(%s) pRequestPath(%s)\r\n"), pSatIp->pDumpFile, pszFileName, pRequestPath);

#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
			if(NULL == pSatIp->pDumpFile)
			{
				HxLOG_Warning("pSatIp->pDumpFile is NULL\n");

				HxSTD_snprintf(pszFileName, 255, "home1/settop/root/SATIP_%05d_%05d_%08x.ts", pstInst->stSetupInfo.stTuningParam.sat.tuningInfo.ulFrequency, hRingbuf, HLIB_STD_GetSystemTick());
				pSatIp->pDumpFile = HLIB_FILE_Open(pszFileName, "wb");

				HxLOG_Warning("[RETRY] pSatIp->pDumpFile(0x%x) pszFileName(%s)\r\n", pSatIp->pDumpFile, pszFileName);
			}
#endif
		}
	}
#endif
}


STATIC HCHAR *_plstreaming_ModeSatIp_ChangeTokenURL(HCHAR *pszStreamingPath)
{
	HCHAR				*pszTokenPath = NULL;

	#if 0
	pszTokenPath = HxSTD_StrChar(pszStreamingPath, '?');
	if(NULL == pszTokenPath)
	{
		pszTokenPath = pszStreamingPath;
	}
	else
	{
		pszTokenPath++;
	}
	#else
	pszTokenPath = HLIB_STD_StrReplaceChar(pszStreamingPath, pszStreamingPath, '?', '&');
	#endif

	return pszTokenPath;
}

STATIC Handle_t _plstreaming_ModeSatIp_CreateRingbuf(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath)
{
	Handle_t			hRingbuf = HANDLE_NULL;
	HINT32				nMaxSize = 0;
	HxRingBuf_Setup_t	stRingbufSetup;

	HxSTD_memset(&stRingbufSetup, 0, sizeof(HxRingBuf_Setup_t));

	nMaxSize = PL_Streaming_Spec_SATIP_GetBufferSize();
	if(0 >= nMaxSize)
	{
		nMaxSize = (8 * 1024 * 1024);
	}

#if 0 // chunk
	{
		HINT32		nChunkSize = (188 * 4 * 1024);

		stRingbufSetup.eBufType 		= eHxRINGBUF_TYPE_CHUNK;
		stRingbufSetup.ulMaxSize		= nChunkSize * 5;	/* 10개를 가지도록 함. */
		stRingbufSetup.ulChunkSize		= nChunkSize;
		stRingbufSetup.bBlockOverflow	= FALSE;			/* true:overflow시 write block 됨. */
		stRingbufSetup.eRuleAfterRead	= eHxRINGBUF_READRULE_REMAIN;
	}
#else // linear
	{
		stRingbufSetup.eBufType 		= eHxRINGBUF_TYPE_LINEAR;
		stRingbufSetup.ulMaxSize		= nMaxSize;
		stRingbufSetup.ulChunkSize		= 0;
		stRingbufSetup.bBlockOverflow	= FALSE;			/* true:overflow시 write block 됨. */
		stRingbufSetup.eRuleAfterRead	= eHxRINGBUF_READRULE_REMOVE;

	}
#endif
	HxLOG_Debug("stRingbufSetup.eBufType = %d\n", stRingbufSetup.eBufType);
	HxLOG_Debug("stRingbufSetup.ulMaxSize = %d (%d MB)\n", stRingbufSetup.ulMaxSize, stRingbufSetup.ulMaxSize / 1024 / 1024);
	HxLOG_Debug("stRingbufSetup.ulChunkSize = %d (188 * %d)\n", stRingbufSetup.ulChunkSize, stRingbufSetup.ulChunkSize / 188);
	HxLOG_Debug("stRingbufSetup.bBlockOverflow = %d\n", stRingbufSetup.bBlockOverflow);
	HxLOG_Debug("stRingbufSetup.eRuleAfterRead = %d\n", stRingbufSetup.eRuleAfterRead);


	hRingbuf = PL_RINGBUF_Create(&stRingbufSetup);

	HxLOG_Debug("hRingbuf = 0x%x\n", hRingbuf);

    _plstreaming_ModeSatIp_CreateDumpFile(pstModeInst, hRingbuf, pRequestPath);

	return hRingbuf;
}

STATIC HINT32	_plstreaming_ModeSatIp_GetMinReadSize(PLStreaming_ModeInst_t *pstModeInst)
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

STATIC HBOOL _plstreaming_ModeSatIp_WaitLocked(PLStreaming_ModeSatIp_t *pSatIp, HUINT32 ulMaxLoop)
{
	HINT32						nLoopCnt = 0;
	HERROR						hErr = ERR_OK;
	HBOOL						bIsLocked = FALSE;

	while(ulMaxLoop > nLoopCnt)
	{
		hErr = PL_COMMON_IsTunerLocked(pSatIp->pstTS->tunerid, &bIsLocked);
		if((ERR_OK == hErr) && (TRUE == bIsLocked))
		{
			HxLOG_Print(HxANSI_COLOR_PURPLE("Locked!!\n"));
			break;
		}
		else
		{
			VK_TASK_Sleep(50);
			nLoopCnt++;
			HxLOG_Print(HxANSI_COLOR_PURPLE("nLoopCnt(%d)\n"), nLoopCnt);
		}
	}

	return bIsLocked;
}

STATIC HINT32 _plstreaming_ModeSatIp_WaitRingbuf(Handle_t hResHandle, Handle_t hRingbuf, HINT32 nMinReadSize, HINT32 nMaxLoop)
{
	HINT32						nLoopCnt = 0;
	HINT32						nUnreadSize = 0;
	HERROR						hErr = ERR_OK;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
	HxRingBuf_Status_t			stStatus;
	PLStreaming_SetupStatus_e 	eSetupStatus = ePLStreaming_SetupStatus_None;

	while(1)
	{
		HxSTD_MemSet(&stStatus, 0, sizeof(HxRingBuf_Status_t));

		eHomResult = PL_Streaming_Resource_GetStatus(hResHandle, &eSetupStatus);
		if( eHOM_Result_Ok != eHomResult || ePLStreaming_SetupStatus_Conflict == eSetupStatus)
		{
			HxLOG_Error("PL_Streaming_Resource_GetStatus error!!(eHomResult:%d eSetupStatus:%d)\n",eHomResult, eSetupStatus);
			return 0;
		}

		hErr = PL_RINGBUF_GetBufferingStatus(hRingbuf, &stStatus);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("PL_RINGBUF_GetBufferingStatus error!!(hErr=%d)\n", hErr);
			return 0;
		}

		if(stStatus.ulUnreadSize > (nMinReadSize))
		{
			break;
		}
		else if(nMaxLoop <= nLoopCnt)
		{
			HxLOG_Warning("PL_RINGBUF_GetBufferingStatus not data!!(nMaxLoot:%d <= nLoopCnt:%d)\n", nMaxLoop, nLoopCnt);
			return 0;
		}
		else
		{
			HxLOG_Print("nLoopCnt(%d)\n", nLoopCnt);
			VK_TASK_Sleep(50);
		}
		nLoopCnt++;
	}

	/* 188 size로 변경해서 보내 주기 */
	nUnreadSize = stStatus.ulUnreadSize; //(stStatus.ulUnreadSize - (stStatus.ulUnreadSize % 188));

	//PL_Streaming_Debug_Print("[%s:%d] hRingbuf(0x%x) nLoopCnt(%d) nUnreadSize(%d)\r\n", __FUNCTION__, __LINE__, hRingbuf, nLoopCnt, nUnreadSize);

	return nUnreadSize;
}

STATIC HINT32 _plstreaming_ModeSatIp_NoWaitRingbuf(Handle_t hResHandle, Handle_t hRingbuf, HINT32 nMinReadSize)
{
	HINT32						nUnreadSize = 0;
	HERROR						hErr = ERR_OK;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
	HxRingBuf_Status_t			stStatus;
	PLStreaming_SetupStatus_e 	eSetupStatus = ePLStreaming_SetupStatus_None;

	HxSTD_MemSet(&stStatus, 0, sizeof(HxRingBuf_Status_t));

	eHomResult = PL_Streaming_Resource_GetStatus(hResHandle, &eSetupStatus);
	if( eHOM_Result_Ok != eHomResult || ePLStreaming_SetupStatus_Conflict == eSetupStatus)
	{
		HxLOG_Error("PL_Streaming_Resource_GetStatus error!!(eHomResult:%d eSetupStatus:%d)\n",eHomResult, eSetupStatus);
		return 0;
	}

	hErr = PL_RINGBUF_GetBufferingStatus(hRingbuf, &stStatus);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("PL_RINGBUF_GetBufferingStatus error!!(hErr=%d)\n", hErr);
		return 0;
	}

	if(0 == stStatus.ulUnreadSize)
	{
		HxLOG_Print("stStatus.ulUnreadSize is 0\n");
		return 0;
	}

	if(stStatus.ulUnreadSize > nMinReadSize)
	{
#ifdef _USE_NO_WAIT_READ
		// max read size 제한
		if(_MAX_READ_TS_SIZE < stStatus.ulUnreadSize)
		{
			nUnreadSize = _MAX_READ_TS_SIZE;
		}
		else
		{
			nUnreadSize = stStatus.ulUnreadSize;
		}
#else
		nUnreadSize = stStatus.ulUnreadSize;
#endif

		//PL_Streaming_Debug_Print("[%s:%d] hRingbuf(0x%x) nLoopCnt(%d) nUnreadSize(%d)\r\n", __FUNCTION__, __LINE__, hRingbuf, nLoopCnt, nUnreadSize);

		return nUnreadSize;
	}
	else
	{
		HxLOG_Print("stStatus.ulUnreadSize(%d) > nMinReadSize(%d)\n", stStatus.ulUnreadSize, nMinReadSize);
		return 0;
	}
}

STATIC HOM_Result_e _plstreaming_ModeSatIp_ReadRingbuf(PLStreaming_ModeInst_t *pstModeInst, HINT32 nTsIndex, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	HERROR						hErr = ERR_OK;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
	HINT32						ulDataSize = 0;
	HUINT8						*pucData = NULL;
	HUINT32						ulUnreadSize = 0, ulReadSize = 0;
	HINT32						nMinReadSize = 0;
	PLStreaming_ModeSatIp_t 	*pSatIp = NULL;
#ifdef CONFIG_DEBUG
	HUINT32						ulCurTick = 0, ulTimeGap = 0;
	HINT32						nKbps = 0;
#endif

    pSatIp 		= (&pstModeInst->Mode.stSatIp);

	if(TRUE == pSatIp->bIgnoreNext)
	{
		pSatIp->bIgnoreNext = FALSE;
		HxLOG_Print("ignore read (%d)\r\n", nTsIndex);
		return eHOM_Result_Error_Resource_Notfound;
	}

    if (pstModeInst->PidFlag & MASK_PID_NONE)
    {
        HxLOG_Print("if pids=none, send empty rtp\n");
        return eHOM_Result_Error_Resource_Notfound;
    }

	#ifdef _USE_NO_WAIT_READ
	{
        ulUnreadSize = _plstreaming_ModeSatIp_NoWaitRingbuf(pstModeInst->hResHandle, pstModeInst->hRingbuf, nMinReadSize);
		if(0 == ulUnreadSize)
		{
			HxLOG_Print("no data, hRingbuf(0x%x) nTsIndex(%d) status(%s)\r\n",
                pstModeInst->hRingbuf, nTsIndex, PL_Streaming_Debug_GetSetupStatusStr(pstModeInst->eSetupStatus));
			return eHOM_Result_Error_Resource_Notfound;
		}
	}
	#else
	{
		/*
			SATIP에서 RTSP에서 주기적으로 가져간다.
			가져갈 때 error를 return 하고 값을 빈값으로 채워도 문제가 안된다.
			(일정량을 가져가면, rtsp server가 client에 udp로 전달하는 방식)
			WaitRingbuf 값은 rtsp server의 최대 대기 값보다 적게 해야한다.
			(rtsp server는 하나의 명령을 대기 하는 시간은, 10 * 300ms => 3초가 된다.)
			대략 다른 처리까지 합쳐서 최대 2초 내에서 설정해야 한다.

			아래는 5초 까지 waiting: 100 * 50ms
		*/

        ulUnreadSize = _plstreaming_ModeSatIp_WaitRingbuf(pstModeInst->hResHandle, pstModeInst->hRingbuf, nMinReadSize, 100);
		if(0 == ulUnreadSize)
		{
			HxLOG_Warning("_plstreaming_ModeSatIp_WaitRingbuf not data!!, hRingbuf(0x%x) nTsIndex(%d) status(%s)\r\n",
                pstModeInst->hRingbuf, nTsIndex, PL_Streaming_Debug_GetSetupStatusStr(pstInst->eSetupStatus));
			pSatIp->bIgnoreNext = TRUE;
			return eHOM_Result_Error_Resource_Notfound;
		}
	}
	#endif

	if(ulUnreadSize < nMinReadSize)
	{
		HxLOG_Error("ulUnreadSize(%d) <  nMinReadSize(%d)\r\n", ulUnreadSize, nMinReadSize);
		return eHOM_Result_Error;
	}

	ulReadSize = ulUnreadSize;

	pucData = HLIB_STD_MemAlloc(ulReadSize);
	if(NULL == pucData)
	{
		HxLOG_Error("eHOM_Result_Error_OutofMem error!!\r\n");
		return eHOM_Result_Error_OutofMem;
	}

    hErr = PL_RINGBUF_ReadData(pstModeInst->hRingbuf, ulReadSize, pucData, &ulDataSize);
	if(ERR_OK != hErr || 0 == ulDataSize)
	{
		HLIB_STD_MemFree(pucData);

		HxLOG_Error("PL_RINGBUF_ReadData error!!\r\n");
		return eHOM_Result_Error_Resource_Notfound;
	}

#ifdef CONFIG_DEBUG
{
	HUINT32	ulTotalDur = 0;
	HUINT32	ulHour = 0, ulMin = 0, ulSecond = 0;

	/* check kbps of writing to homma */
	ulCurTick = HLIB_STD_GetSystemTick();
	ulTimeGap = ulCurTick - pSatIp->ulLastReadTick;
	if(0 >= ulTimeGap)
	{
		ulTimeGap = 1;
	}
	nKbps = (ulDataSize / ulTimeGap) * 1000 / 1024; /* kbps */

	ulTotalDur = (ulCurTick - pSatIp->ulStartReadTick) / 1000;
	ulHour = ulTotalDur / 3600;
	ulMin = (ulTotalDur - (ulHour * 3600)) / 60;
	ulSecond = ulTotalDur - (ulHour * 3600) - (ulMin * 60);

	HxLOG_Print(HxANSI_COLOR_PURPLE("dur = %02d:%02d:%02d\n\t(4)[DATA_TRACE][HOMMA>READ FROM RINGBUF(0x%x)] ulDataSize(%d, %d kbps), tick = %d, gap = %d\r\n")
									, ulHour, ulMin, ulSecond
                                    , pstModeInst->hRingbuf, ulDataSize, nKbps, ulCurTick, ulTimeGap);
	pSatIp->ulLastReadTick = ulCurTick;
	HxLOG_Message("[SATIP_TIME] dur = %02d:%02d:%02d, (%d, %d kbps), gap = %d\r\n", ulHour, ulMin, ulSecond, ulDataSize, nKbps, ulTimeGap);
}
#endif


	if(eHOM_Result_Ok == eHomResult )
	{
		if(NULL != pSatIp->pDumpFile)
		{
			HLIB_FILE_Write(pSatIp->pDumpFile, pucData, ulDataSize, 1);
		}
	}

	*pulDataSize 	= ulDataSize;
	*ppucData 		= pucData;

	return eHomResult;
}

STATIC void _plstreaming_ModeSatIp_DestroyRingbuf(PLStreaming_ModeInst_t *pstModeInst)
{
	PLStreaming_ModeSatIp_t 	*pSatIp = NULL;

    pSatIp 		= (&pstModeInst->Mode.stSatIp);

    if(HANDLE_NULL != pstModeInst->hRingbuf && 0 != pstModeInst->hRingbuf)
	{
		if (pSatIp->bIsOwner == FALSE)
		{
            PL_RINGBUF_RemoveRingBuf(pstModeInst->hRingbuf);
		}

        PL_RINGBUF_Destroy(pstModeInst->hRingbuf);
        pstModeInst->hRingbuf = HANDLE_NULL;
    }

	if(NULL != pSatIp->pDumpFile)
	{
		HLIB_FILE_Close(pSatIp->pDumpFile);
		pSatIp->pDumpFile = NULL;
	}

}

STATIC void _plstreaming_ModeSatIp_MakeDisplayString(PLStreaming_ModeInst_t *pstModeInst)
{
	HCHAR			szDisplayString[128] = {0, };
	HINT32			nFreq = 0;

    nFreq = _plstreaming_ModeSatIp_GetFreqByTuningParam(&pstModeInst->stSetupInfo.stTuningParam);
    if(NULL != pstModeInst->stSetupInfo.pszIPAddress)
	{
        HxSTD_snprintf(szDisplayString, 127, "SAT>IP (%d:%s)", nFreq, pstModeInst->stSetupInfo.pszIPAddress);
	}
	else
	{
        HxSTD_snprintf(szDisplayString, 127, "SAT>IP (%d:%d)", nFreq, pstModeInst->hRingbuf);
	}


    if(NULL != pstModeInst->stSetupInfo.pszDisplayString)
	{
        HLIB_STD_MemFree(pstModeInst->stSetupInfo.pszDisplayString);
        pstModeInst->stSetupInfo.pszDisplayString = NULL;
	}

    pstModeInst->stSetupInfo.pszDisplayString = HLIB_STD_StrDup(szDisplayString);
}

STATIC void 	_plstreaming_ModeSatIp_DeleteInstance(PLStreaming_ModeInst_t *pstInstMode)
{
	PLStreaming_ModeSatIp_t		*pSatIp = NULL;

    if (pstInstMode == NULL)
    {
        HxLOG_Error("Invalid args(pstInstMode is null)\n");
        return;
    }

    pSatIp 		= (&pstInstMode->Mode.stSatIp);

    if(FALSE == pstInstMode->stSetupInfo.bNotUseResource)
	{
        _plstreaming_ModeSatIp_DestroyRingbuf(pstInstMode);

		if(pSatIp->pstTS)
		{
			HLIB_STD_MemFree(pSatIp->pstTS);
			pSatIp->pstTS = NULL;
		}

		if(pSatIp->pstSvc)
		{
			HLIB_STD_MemFree(pSatIp->pstSvc);
			pSatIp->pstSvc = NULL;
		}

		if(pSatIp->pDumpFile)
		{
			HLIB_FILE_Close(pSatIp->pDumpFile);
			pSatIp->pDumpFile = NULL;
		}

        PL_StreamingKey_Delete(pstInstMode->pstStreamingKey);
        pstInstMode->pstStreamingKey = NULL;
        pstInstMode->isLocked = FALSE;
		pstInstMode->tunerid = -1;
        pstInstMode->eDelayedCmd = PLStreaming_DelayedCmd_None;
		HxSTD_MemSet(pSatIp, 0, sizeof(PLStreaming_ModeSatIp_t));
	}
}


STATIC HUINT32 	_plstreaming_ModeSatIp_Stop(PLStreaming_ModeInst_t *pstModeInst, HBOOL bWaiting)
{
    HUINT32 SamaSlot;

    SamaSlot = PL_Streaming_Resource_Delete(pstModeInst->hResHandle, bWaiting);
    pstModeInst->hResHandle = HANDLE_NULL;
    pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_None;

    return SamaSlot;
}

STATIC HOM_Result_e 	_plstreaming_ModeSatIp_Start(PLStreaming_ModeInst_t *pstModeInst)
{
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;
	HINT32						nMinReadSize = 0;
	PLStreaming_ModeSatIp_t		*pSatIp = NULL;

    pSatIp 		= (&pstModeInst->Mode.stSatIp);

	if (pSatIp == NULL)
	{
		HxLOG_Error("invalid args(pSatIp is null)\n");
		return eHOM_Result_Error_NullPoint;
	}

	if (pSatIp->bIsOwner == FALSE)
	{
        HxLOG_Print("Start NoOwner streaming : hResHandle(0x%x) hRingbuf(0x%x)\n", pstModeInst->hResHandle, pstModeInst->hRingbuf);
		return eHOM_Result_Ok;
	}

    if(HANDLE_NULL == pstModeInst->hResHandle || 0 == pstModeInst->hResHandle)
	{
        pstModeInst->hResHandle = PL_Streaming_Resource_New(pstModeInst->hStreamingId);
	}

    if(HANDLE_NULL == pstModeInst->hResHandle || 0 == pstModeInst->hResHandle)
	{
		HxLOG_Error("resource handle error!!\r\n");
		return eHOM_Result_Error;
	}

    _plstreaming_ModeSatIp_MakeDisplayString(pstModeInst);

    eHomResult = PL_Streaming_Resource_Start(pstModeInst->hResHandle, pstModeInst,0);
	if(eHOM_Result_Ok != eHomResult)
	{
        HxLOG_Error("PL_Streaming_Resource_Start failed(eHomeResult:%d)\n",eHomResult);
		return eHomResult;
	}

    pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_Start;

    nMinReadSize = _plstreaming_ModeSatIp_GetMinReadSize(pstModeInst);

    HxLOG_Print("hResHandle(0x%x) hRingbuf(0x%x) nMinReadSize(%d)\r\n", pstModeInst->hResHandle, pstModeInst->hRingbuf, nMinReadSize);

    eHomResult = _plstreaming_ModeSatIp_Update_TunerInfo(pstModeInst);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Warning("_plstreaming_ModeSatIp_Update_TunerInfo failed(eHomResult:%d)\n", eHomResult);
	}

	return eHOM_Result_Ok;
}

#ifdef SATIP_USE_DEFAULT_PID
STATIC HUINT16 s_stDefaultPIDs[] = {0, 16, 17, 18, 20};
#else
STATIC HUINT16 s_stDefaultPIDs[] = {0};
#endif

STATIC HINT32	_plstreaming_ModeSatIp_GetDefaultPids(HUINT16 usPIDs[], HINT32 nIndex)
{
	HINT32 i = 0;
	HINT32 nMax = sizeof(s_stDefaultPIDs) / sizeof(HUINT16);

	for(i=0; i < nMax; i++)
	{
		usPIDs[nIndex++] = s_stDefaultPIDs[i];
	}

	return nIndex;
}

STATIC HBOOL	_plstreaming_ModeSatIp_IsDefaultPids(HUINT16 usPID)
{
	HINT32 i = 0;
	HINT32 nMax = sizeof(s_stDefaultPIDs) / sizeof(HUINT16);

	for(i=0; i < nMax; i++)
	{
		if(usPID == s_stDefaultPIDs[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HBOOL	_plstreaming_ModeSatIp_UseOnlyDefaultPids(HINT32 nPIDsCount,HUINT16 usPID[])
{
	HINT32 i = 0;
	HINT32 nMax = sizeof(s_stDefaultPIDs) / sizeof(HUINT16);

    if(nPIDsCount != nMax)
    {
        return FALSE;
    }

	for(i=0; i < nMax; i++)
	{
		if(usPID[i] != s_stDefaultPIDs[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}



#define __SATIP_SVC_SAT_INFO__
STATIC HINT32 _plstreaming_ModeSatIp_GetSrcNumByTuningParam(DxTuneParam_t	*pstTuningParam)
{
	HINT32 nSrcNum = -1;

	if(eDxDELIVERY_TYPE_SAT & pstTuningParam->eDeliType)
	{
		nSrcNum = _plstreaming_ModeSatIp_SAT_SrcNumToEnum(&pstTuningParam->sat.antInfo);
	}
	else if(eDxDELIVERY_TYPE_TER & pstTuningParam->eDeliType)
	{
		nSrcNum = 1;
	}
	else if(eDxDELIVERY_TYPE_CAB & pstTuningParam->eDeliType)
	{
		nSrcNum = 1;
	}

	return nSrcNum;
}

STATIC HINT32 _plstreaming_ModeSatIp_GetFreqByTuningParam(DxTuneParam_t	*pstTuningParam)
{
	HINT32 nFreq = -1;

	if(eDxDELIVERY_TYPE_SAT & pstTuningParam->eDeliType)
	{
		nFreq = (HINT32)pstTuningParam->sat.tuningInfo.ulFrequency;
	}
	else if(eDxDELIVERY_TYPE_TER & pstTuningParam->eDeliType)
	{
		nFreq = (HINT32)pstTuningParam->ter.ulFrequency;
	}
	else if(eDxDELIVERY_TYPE_CAB & pstTuningParam->eDeliType)
	{
		nFreq = (HINT32)pstTuningParam->cab.ulFrequency;
	}

	return nFreq;
}

STATIC HINT32	_plstreaming_ModeSatIp_GetSrcNumByToken(PLStreaming_ModeInst_t *pstModeInst, HxTOKEN_Handle_t hToken)
{
	HINT32	nSrcNum = eSatipSrcType_DISEQC_A;

	nSrcNum = HLIB_TOKEN_GetValueInt(hToken, SATIP_SAT_QUERY_SYNTAX_SRC_ID);
	if(0 == nSrcNum)
    {
        /*2014/06/12 mhkang:RTSP Message에 Src 정보가 없을 때, 이전에 RTSP Message에서 사용된 Src값 이용.*/
        nSrcNum = pstModeInst->Mode.stSatIp.nSrc;
        if(0 == nSrcNum)
        {
            HxLOG_Warning("invalid or no src(%d)\n", nSrcNum);
            nSrcNum = eSatipSrcType_DISEQC_A; /* set the default value */
        }
	}

#ifdef _USE_HD_PLUS_APP_IN_BUNDANG_
    pstModeInst->Mode.stSatIp.bSrcConverted = FALSE;
	if(eSatipSrcType_DISEQC_A == nSrcNum)
	{
	 	nSrcNum = eSatipSrcType_DISEQC_C;
        pstModeInst->Mode.stSatIp.bSrcConverted = TRUE;
	}
#endif

	return nSrcNum;
}

STATIC HxList_t *_plstreaming_ModeSatIp_GetSvcListByTuneString(PLStreaming_ModeInst_t *pstModeInst, HxTOKEN_Handle_t hToken, HxList_t *pRootChList)
{
	HxList_t 		*pChList = pRootChList;
	HxList_t 		*pTPList = NULL;
	HINT32			nFreq = 0;
	HINT32			nSrcNum = 0;
	HINT32			nCmpFreq = 0;
	CO_CHANNEL_t	*pstCoCh = NULL;

	HINT32 			nInputSrcNum = 0;

    nSrcNum = _plstreaming_ModeSatIp_GetSrcNumByToken(pstModeInst, hToken);

	nFreq = HLIB_TOKEN_GetValueInt(hToken, SATIP_SAT_QUERY_SYNTAX_FREQ);
	if(0 >= nFreq)
	{
		HxLOG_Error("HLIB_TOKEN_GetValueInt error!, nFreq\r\n");
		return NULL;
	}

	while(pChList)
	{
		pstCoCh = HLIB_LIST_Data(pChList);
		if(NULL == pstCoCh || NULL == pstCoCh->ts)
		{
			pChList = HLIB_LIST_Next(pChList);
			continue;
		}

		nInputSrcNum = _plstreaming_ModeSatIp_GetSrcNumByTuningParam(&pstCoCh->ts->tuningParam);
		nCmpFreq = _plstreaming_ModeSatIp_GetFreqByTuningParam(&pstCoCh->ts->tuningParam);

		if(0 >= nCmpFreq || 0 >= nInputSrcNum)
		{
			pChList = HLIB_LIST_Next(pChList);
			continue;
		}

		if(nInputSrcNum != nSrcNum)
		{
			pChList = HLIB_LIST_Next(pChList);
			continue;
		}

		if((nCmpFreq-5) < nFreq && nFreq < (nCmpFreq+5)) // 8M
		{
			HxLOG_Print("tsid(%d) onid(%d) SVC(%s) SVCUID(%x)\r\n", pstCoCh->ts->tsid,  pstCoCh->ts->onid, pstCoCh->svc->name, pstCoCh->svc->uid);

			pTPList = HLIB_LIST_Append(pTPList, pstCoCh);
		}

		pChList = HLIB_LIST_Next(pChList);
	}

	return pTPList;
}

STATIC void _plstreaming_ModeSatIp_ReleaseTpList(HxList_t *pRootTpList)
{
	HLIB_LIST_RemoveAll(pRootTpList);
}

STATIC HOM_Result_e _plstreaming_ModeSatIp_GetPids(HCHAR *pszPIDs, HINT32 *pnPIDsCount, HUINT16 usPIDs[])
{
	HOM_Result_e	eHomResult = eHOM_Result_Ok;
	HUINT16			usTempPid = 0;
	HINT32			nIndex = 0;
	HINT32			i = 0;
    HCHAR 			*token, *pPtr;


	if(NULL != pszPIDs)
	{

        HxLOG_Debug("pszPIDs : %s\n", pszPIDs);
#if 0 // 2015/2/12 mhkang : pids=
		pNext = HxSTD_StrStr(pszPIDs, "all");
		if(NULL != pNext)
		{
			usPIDs[0] = PL_STREAMING_ALL_PID;
			nIndex++;
            *pnPIDsCount = (nIndex);
			return eHOM_Result_Ok;
		}

        pNext = HxSTD_StrStr(pszPIDs, "none");
        if(NULL != pNext)
        {
            usPIDs[0] = PL_STREAMING_NONE_PID;
            nIndex++;
            *pnPIDsCount = (nIndex);
            return eHOM_Result_Ok;
        }
#endif
	}
#if 0   // 2015/2/11 mhkang: Not used default pids
	nIndex = _plstreaming_ModeSatIp_GetDefaultPids(usPIDs, nIndex);
#endif
	if(NULL != pszPIDs)
	{
		token = strtok_r(pszPIDs, ",", &pPtr);
	    while (token!= NULL)
	    {
			usTempPid = HLIB_STD_StrToINT32(token);

			for(i=0; i < nIndex; i++)
			{
				if(usPIDs[i] == usTempPid)
				{
					break;
				}
			}
			if(nIndex <= i)
			{
				usPIDs[nIndex++] = usTempPid;
			}
	        token = strtok_r(NULL, ",", &pPtr);
	    }
	}

	*pnPIDsCount = (nIndex);

	PL_Streaming_Debug_PrintPids(nIndex, usPIDs, __FUNCTION__, __LINE__);

	return eHomResult;
}

STATIC CO_CHANNEL_t *_plstreaming_ModeSatIp_ChooseCoChannel(HxList_t *pRootTpList, HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	HxList_t 		*pTPList = pRootTpList;
	CO_CHANNEL_t	*pstCoCh = NULL;
	HINT32			i = 0;
	HINT32			nSameCnt = 0;

	while(pTPList)
	{
		pstCoCh = HLIB_LIST_Data(pTPList);
		if(NULL == pstCoCh)
		{
			pTPList = HLIB_LIST_Next(pTPList);
			continue;
		}

		if(NULL == pstCoCh->svc)
		{
			pTPList = HLIB_LIST_Next(pTPList);
			continue;
		}

		HxLOG_Print("SVC(%s) SVCUID(%x)\r\n", pstCoCh->svc->name, pstCoCh->svc->uid);

		for(i=0; i < nPIDsCount; i++)
		{
			if(usPIDs[i] == pstCoCh->svc->pmtPid)
			{
				nSameCnt+=2;
			}
			else if(usPIDs[i] == pstCoCh->svc->videoPid)
			{
				nSameCnt++;
			}
			else if(usPIDs[i] == pstCoCh->svc->audioPid)
			{
				nSameCnt++;
			}
			else if(usPIDs[i] == pstCoCh->svc->pcrPid)
			{
				nSameCnt++;
			}
			else if(usPIDs[i] == pstCoCh->svc->ttxPid)
			{
				nSameCnt++;
			}
		}

		if(2 <= nSameCnt)
		{
			return pstCoCh;
		}

		pTPList = HLIB_LIST_Next(pTPList);
	}

	return NULL;
}

STATIC CO_CHANNEL_t *_plstreaming_ModeSatIp_FindSvcByUID(PLStreaming_ModeInst_t *pstModeInst, HxTOKEN_Handle_t hToken, HxList_t *pRootChList, HUID uUID, HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	HxList_t 		*pChList = NULL;
	CO_CHANNEL_t	*pstCoCh = NULL;

	PLStreaming_ModeSatIp_t 	*pSatIp = NULL;
	PLStreaming_SetupInfo_t 	*pSetupInfo = NULL;

    pSatIp 		= (&pstModeInst->Mode.stSatIp);
    pSetupInfo 	= (&pstModeInst->stSetupInfo);

	HxLOG_Print("uUID("HUID_FMT")\r\n", uUID);

	pChList = pRootChList;
	while(pChList)
	{
		pstCoCh = HLIB_LIST_Data(pChList);
		if(NULL != pstCoCh)
		{
			if(NULL != pstCoCh->svc)
			{
				if(pstCoCh->svc->uid == uUID)
				{
					pSatIp->pstSvc		= HLIB_STD_MemDup(pstCoCh->svc, sizeof(DxService_t));
					pSatIp->pstTS		= HLIB_STD_MemDup(pstCoCh->ts, sizeof(DxTransponder_t));
					HxSTD_memcpy(&pSetupInfo->stTuningParam, &pSatIp->pstTS->tuningParam, sizeof(DxTuneParam_t));


					return pstCoCh;
				}
			}
		}
		pChList = HLIB_LIST_Next(pChList);
	}

	return NULL;
}

STATIC CO_CHANNEL_t *_plstreaming_ModeSatIp_FindSvcByPID(PLStreaming_ModeInst_t *pstModeInst, HxTOKEN_Handle_t hToken, HxList_t *pRootChList, HCHAR	*pszPIDs, HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	HxList_t 		*pTPRootList = NULL;
	CO_CHANNEL_t	*pstCoCh = NULL;
	CO_CHANNEL_t	*pstRootCoCh = NULL;

	PLStreaming_ModeSatIp_t 	*pSatIp = NULL;
	PLStreaming_SetupInfo_t 	*pSetupInfo = NULL;

    pSatIp 		= (&pstModeInst->Mode.stSatIp);
    pSetupInfo 	= (&pstModeInst->stSetupInfo);

	HxLOG_Print("pszPIDs(%s)\r\n", pszPIDs);

	// freq, src에 해당되는 TP의 channel list만 추림
    pTPRootList = _plstreaming_ModeSatIp_GetSvcListByTuneString(pstModeInst, hToken, pRootChList);
	if(NULL == pTPRootList)
	{
		HxLOG_Error("Can not find tp \r\n");
		_plstreaming_ModeSatIp_ReleaseTpList(pTPRootList);
		return NULL;
	}

	// 찾은 채널 중 하나의 tp 정보를 가져옴 (모두 같을 거고)
	if(NULL != pTPRootList)
	{
		pstRootCoCh = HLIB_LIST_Data(pTPRootList);
		if(NULL != pstRootCoCh)
		{
			pSatIp->pstTS		= HLIB_STD_MemDup(pstRootCoCh->ts, sizeof(DxTransponder_t));
			HxSTD_memcpy(&pSetupInfo->stTuningParam, &pSatIp->pstTS->tuningParam, sizeof(DxTuneParam_t));
		}
	}

	// es pid에 매칭되는 채널을 가져와서, pstInst->Mode.stSatIp에 저장
	if(nPIDsCount > 1)
	{
		pstCoCh = _plstreaming_ModeSatIp_ChooseCoChannel(pTPRootList, nPIDsCount, usPIDs);
		if(NULL != pstCoCh)
		{
			pSatIp->pstSvc		= HLIB_STD_MemDup(pstCoCh->svc, sizeof(DxService_t));
		}
	}

	_plstreaming_ModeSatIp_ReleaseTpList(pTPRootList);

	return pstCoCh;
}

STATIC HOM_Result_e _plstreaming_ModeSatIp_MakeAntInfoByString_SAT(PLStreaming_ModeInst_t *pstModeInst, HxTOKEN_Handle_t hToken, DxTransponder_t *pstTS, HxList_t *pRootChList, DxSat_TransportSpec_e eTransSpec, CO_CHANNEL_t	*pCurCoChannel)
{
	HINT32			nQuerySrcNum = 0;
	HINT32			nTpSrcNum = 0;
	HxList_t		*pAntList = NULL;
	HxList_t		*pAntRootList = NULL;
	DxAntInfo_t 	*pAntInfo = NULL;

    nQuerySrcNum = _plstreaming_ModeSatIp_GetSrcNumByToken(pstModeInst, hToken);
    pstModeInst->Mode.stSatIp.nSrc = nQuerySrcNum;

	PL_COMMON_GetAntenaList(&pAntRootList);
	pAntList = pAntRootList;
	while(pAntList)
	{
		pAntInfo = HLIB_LIST_Data(pAntList);
		if(NULL == pAntInfo)
		{
			pAntList = HLIB_LIST_Next(pAntList);
			continue;
		}

        /* 2015/4/21 mhkang: 개선이 필요함.
          현재 문제점:
            - STB LNB 또는 SCD로 채널서치했는데 SAT>IP Client에서 RTSP 요청시 src 가 1이 아니면 에러처리되는 문제.
        */
		nTpSrcNum = _plstreaming_ModeSatIp_SAT_SrcNumToEnum(pAntInfo);
		if(nTpSrcNum != nQuerySrcNum)
		{
			HxLOG_Print("pAnt->Name(%s) nTpSrcNum(%d) nQuerySrcNum(%d)\r\n", pAntInfo->satName, nTpSrcNum, nQuerySrcNum);
			pAntList = HLIB_LIST_Next(pAntList);
			continue;
		}

		HxSTD_MemCopy(&pstTS->tuningParam.sat.antInfo , pAntInfo, sizeof(DxAntInfo_t));
		pstTS->tuningParam.eDeliType			= eDxDELIVERY_TYPE_SAT;
		pstTS->tuningParam.sat.antennaType 		= pAntInfo->antType;
		pstTS->tuningParam.sat.ulTunerGroupId	= (HUINT32)0;
		pstTS->tuningParam.sat.ulRfInputId		= (HUINT32)0;

		HxSTD_memcpy(pstTS->tuningParam.sat.tuningInfo.satType, pAntInfo->satType, DxANTINFO_NAME_LEN);
		pstTS->tuningParam.sat.tuningInfo.antuid = pAntInfo->uid;

		PL_COMMON_ReleseAntenaList(pAntRootList);

		return eHOM_Result_Ok;
	}

	HxLOG_Error(HxANSI_COLOR_RED("#################################################\r\n"));
	HxLOG_Error(HxANSI_COLOR_RED("##############  pAntRootList(NULL) ##############\r\n"));
	HxLOG_Error(HxANSI_COLOR_RED("#################################################\r\n"));

	PL_COMMON_ReleseAntenaList(pAntRootList);
	(void)eTransSpec;

	return eHOM_Result_Error;
}

STATIC HOM_Result_e _plstreaming_ModeSatIp_MakeTPInfoByString_SAT(HxTOKEN_Handle_t hToken, DxTransponder_t 	*pstTS, HxList_t *pRootChList, DxSat_TransportSpec_e eTransSpec, CO_CHANNEL_t	*pCurCoChannel)
{
	HINT32					nSR = 0;			// int
	HINT32					nFreq = 0;			// int

	DxSat_PskModulation_e	ePskModulation 	= eDxSAT_PSK_AUTO;
	DxSat_RollOff_e			eRollOff 		= eDxSAT_ROLL_035;
	DxSat_Pilot_e			ePilot 			= eDxSAT_PILOT_AUTO;
	DxSat_CodeRate_e		eFecCodeRate 	= eDxSAT_CODERATE_AUTO;
	DxSat_Polarization_e	ePolarization 	= eDxSAT_POLAR_AUTO;

	//ex) rtsp://192.168.50.32:554/?src=1&freq=11836&pol=h&msys=dvbs&sr=27500&fec=56&pids=0,300,301,301,302,304

	/* freq */
	{
		/*
			Spec v1.2 - 3.5.11 Query Syntax
				Frequency freq frequency Transponder frequency expressed in MHz as fixed point type or integer value.
					freq=11361.75
					freq=11720


			PANASONIC VIERA TX-58AXW804 request uri:
				- floating point로 들어오고 있음..
				<_plstreaming_ModeSatIp_Str2Info:2156>; pRequestPath (
					rtsp://192.168.50.7:554/?src=1&freq=10729.00&pol=v&ro=0.35&msys=dvbs2&mtype=8psk&plts=off&sr=22000&fec=23
			)
		*/

		HCHAR 		*pszFreq = NULL;

		pszFreq = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_FREQ);
		if(NULL != pszFreq)
		{
			float fValue = 0.00;
			HERROR	hErr = ERR_FAIL;

			hErr = HLIB_TOKEN_Str2Float(pszFreq, &fValue);
			if(ERR_OK == hErr)
			{
				nFreq = (HINT32)fValue;
			}
		}

		//nFreq = HLIB_TOKEN_GetValueInt(hToken, SATIP_SAT_QUERY_SYNTAX_FREQ);
		if(0 >= nFreq)
		{
			//if fail, nFreq == 0
			HxLOG_Error("nFreq error\r\n");
			return eHOM_Result_Error;
		}
	}

	/* sr */
	{
		nSR = HLIB_TOKEN_GetValueInt(hToken, SATIP_SAT_QUERY_SYNTAX_SYMBOL_RATE);
		if(0 >= nSR)
		{
			HxLOG_Error("nSR  error\r\n");
			return eHOM_Result_Error;
		}
	}

	/* fec */
    #if 1 // 2014/9/1 mhkang:  if using eDxSAT_CODERATE_AUTO value,  RTCP APP <fec_inner> field value loss occurs(refer to  http://svn:3000/issues/95522).
	{
		HCHAR					*pszFec = NULL; 	// "12", "23", "34", "56", "78", "89", "35", "45", "910"

		pszFec = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_FEC);
		if(NULL != pszFec)
		{
			switch(pszFec[0])
			{
			case '1':
				eFecCodeRate = eDxSAT_CODERATE_1_2;
				break;

			case '2':
				eFecCodeRate = eDxSAT_CODERATE_2_3;
				break;

			case '3':
				eFecCodeRate = eDxSAT_CODERATE_3_4;
				if('5' == pszFec[1])
					eFecCodeRate = eDxSAT_CODERATE_3_5;
				break;

			case '4':
				eFecCodeRate = eDxSAT_CODERATE_4_5;
				break;

			case '5':
				eFecCodeRate = eDxSAT_CODERATE_5_11;
				if('6' == pszFec[1])
					eFecCodeRate = eDxSAT_CODERATE_5_6;
				break;

			case '7':
				eFecCodeRate = eDxSAT_CODERATE_7_8;
				break;

			case '8':
				eFecCodeRate = eDxSAT_CODERATE_8_9;
				break;

			case '9':
				eFecCodeRate = eDxSAT_CODERATE_9_10;
				break;

			default:
                HxLOG_Warning("invalid fec(%c) value\n", pszFec[0]);

				eFecCodeRate = eDxSAT_CODERATE_AUTO;
                if(NULL != pCurCoChannel && NULL != pCurCoChannel->ts)
				{
					eFecCodeRate = pCurCoChannel->ts->tuningParam.sat.tuningInfo.eFecCodeRate;
				}
				break;
			}

			// TODO: auto
		}
	}
	#else
    eFecCodeRate 	= eDxSAT_CODERATE_AUTO;
	#endif

	/* pol */
	{
		HCHAR 		*pszPol = NULL; 	// "h", "v", "l", "r"

		pszPol = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_POLAR);
		if(NULL != pszPol)
		{
			switch(pszPol[0])
			{
			case 'h':	ePolarization = eDxSAT_POLAR_HOR;		break;
			case 'v':	ePolarization = eDxSAT_POLAR_VER;		break;
			case 'l':	ePolarization = eDxSAT_POLAR_LEFT;		break;
			case 'r':	ePolarization = eDxSAT_POLAR_RIGHT;		break;

			default:
				ePolarization = eDxSAT_POLAR_HOR;
				if(NULL != pCurCoChannel
					&& NULL != pCurCoChannel->ts)
				{
					ePolarization = pCurCoChannel->ts->tuningParam.sat.tuningInfo.ePolarization;
				}
				break;
			}
		}
	}


	// set additional params for dvb-s2
	//if(eDxSAT_TRANS_DVBS2 == eTransSpec)
	{
		/* 8psk */
        #if 1 // 2014/9/1 mhkang:  if using eDxSAT_PSK_AUTO value,  RTCP APP <type> field value loss occurs(refer to http://svn:3000/issues/95523).
		{
			HCHAR					*pszMType = NULL;	// "qpsk", "8psk" (for DVB-S2)

			pszMType = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_MODULATION_TYPE);
			if(NULL != pszMType)
			{
				if(0 == HLIB_STD_StrNCaseCmp(pszMType, "8psk", 4))
				{
					ePskModulation = eDxSAT_PSK_8PSK;
				}
                else if(0 == HLIB_STD_StrNCaseCmp(pszMType, "qpsk", 4))
				{
					ePskModulation = eDxSAT_PSK_QPSK;
				}
                else
                {
                    HxLOG_Warning("invalid mtype(%s) value\n", pszMType);
                    ePskModulation = eDxSAT_PSK_AUTO;
                }
			}
            else
            {
                HxLOG_Warning("not exist mtype field\n");
                ePskModulation = eDxSAT_PSK_AUTO;
            }
		}
		#else
		ePskModulation = eDxSAT_PSK_AUTO;
		#endif


		/* ro */
		{
			HCHAR					*pszRO = NULL;		// "0.35", "0.25", "0.20" (for DVB-S2)

			pszRO = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_ROLL_OFF);
			if(NULL != pszRO)
			{
				if(0 == HLIB_STD_StrNCaseCmp(pszRO, "0.35", 4))
				{
					eRollOff = eDxSAT_ROLL_035;
				}
				else if(0 == HLIB_STD_StrNCaseCmp(pszRO, "0.25", 4))
				{
					eRollOff = eDxSAT_ROLL_025;
				}
				else if(0 == HLIB_STD_StrNCaseCmp(pszRO, "0.20", 4))
				{
					eRollOff = eDxSAT_ROLL_020;
				}
			}
		}

		/* plts */
		{
			HCHAR					*pszPLTS = NULL;	// "on", "off" (for DVB-S2)

			pszPLTS = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_PILOT_TONES);
			if(NULL != pszPLTS)
			{
				if(0 == HLIB_STD_StrNCaseCmp(pszPLTS, "on", 2))
				{
					ePilot = eDxSAT_PILOT_ON;
				}
				else if(0 == HLIB_STD_StrNCaseCmp(pszPLTS, "off", 3))
				{
					ePilot = eDxSAT_PILOT_OFF;
				}
			}
		}

	}

	pstTS->tuningParam.eDeliType = eDxDELIVERY_TYPE_SAT;
	pstTS->tuningParam.sat.tuningInfo.ulFrequency	= nFreq;//11361;
	pstTS->tuningParam.sat.tuningInfo.ulSymbolRate	= nSR;
	pstTS->tuningParam.sat.tuningInfo.eFecCodeRate	= eFecCodeRate;
	pstTS->tuningParam.sat.tuningInfo.ePolarization	= ePolarization;

	pstTS->tuningParam.sat.tuningInfo.eTransSpec	= eTransSpec;
	pstTS->tuningParam.sat.tuningInfo.ePskMod		= ePskModulation;
	pstTS->tuningParam.sat.tuningInfo.ePilot		= ePilot;
	pstTS->tuningParam.sat.tuningInfo.eRollOff		= eRollOff;


	return eHOM_Result_Ok;
}

STATIC HOM_Result_e _plstreaming_ModeSatIp_MakeTuningInfoByString(PLStreaming_ModeInst_t *pstModeInst, HxTOKEN_Handle_t hToken, HxList_t *pRootChList, HCHAR *pszPIDs, HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	PLStreaming_ModeSatIp_t 	*pSatIp = NULL;
	PLStreaming_SetupInfo_t 	*pSetupInfo = NULL;
	HCHAR				*pszMsys = NULL;
	DxTransponder_t 	*pstTS = NULL;
	HOM_Result_e		eHomResult = eHOM_Result_Ok;

    pSatIp 		= (&pstModeInst->Mode.stSatIp);
    pSetupInfo 	= (&pstModeInst->stSetupInfo);

	// service not found 상황, tuning을 위한 param채워줌

	pstTS = (DxTransponder_t *)HLIB_STD_MemCalloc(sizeof(DxTransponder_t));
	if(NULL == pstTS)
	{
		HxLOG_Error("out of mem!!\r\n");
		return eHOM_Result_Error_OutofMem;
	}

	pszMsys = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_MODULATION_SYSTEM);

	if((NULL == pszMsys) || (0 == HLIB_STD_StrNCaseCmp(pszMsys, "dvbs", 4))) // "dvbs" || "dvbs2"
	{
		DxSat_TransportSpec_e	eTransSpec = eDxSAT_TRANS_DVBS;

		if(NULL != pszMsys)
		{
			if(0 == HLIB_STD_StrNCaseCmp(pszMsys, "dvbs2", 5))
			{
				eTransSpec = eDxSAT_TRANS_DVBS2;
			}
		}

		eHomResult = _plstreaming_ModeSatIp_MakeAntInfoByString_SAT(pstModeInst, hToken, pstTS, pRootChList /* 2015/6/14 mhkang: not used*/, eTransSpec, NULL);
		if(eHOM_Result_Ok != eHomResult)
		{
 			HxLOG_Error("_plstreaming_ModeSatIp_MakeAntInfoByString_SAT error !!\r\n");
			goto ERR_RTN;
		}

		eHomResult = _plstreaming_ModeSatIp_MakeTPInfoByString_SAT(hToken, pstTS, pRootChList/* 2015/6/14 mhkang: not used*/, eTransSpec, NULL);
		if(eHOM_Result_Ok != eHomResult)
		{
 			HxLOG_Error("_plstreaming_ModeSatIp_MakeTPInfoByString_SAT error !!\r\n");
			goto ERR_RTN;
		}


		pSatIp->pstTS = pstTS;
		HxSTD_memcpy(&pSetupInfo->stTuningParam, &pSatIp->pstTS->tuningParam, sizeof(DxTuneParam_t));

		// too many prints... Activate it when you need this print.
		//PL_Streaming_Debug_PrintTuningParam(&pSatIp->pstTS->tuningParam, __FUNCTION__, __LINE__);

		return eHOM_Result_Ok;
	}

	if(NULL != pszMsys)
	{
		// TODO: DVB-T. DVB-C 고려
		if((0 == HLIB_STD_StrNCaseCmp(pszMsys, "dvbt", 4)))
		{
			;
		}

		if((0 == HLIB_STD_StrNCaseCmp(pszMsys, "dvbc", 4)))
		{
			;
		}

		HxLOG_Error("not supported delivery type (%s) \r\n", pszMsys);

		eHomResult = eHOM_Result_Error;
		goto ERR_RTN;
	}


ERR_RTN:
	HLIB_STD_MemFree(pstTS);

	return eHomResult;
}


STATIC HOM_Result_e _plstreaming_ModeSatIp_FindSvc(PLStreaming_ModeInst_t *pstModeInst, HxTOKEN_Handle_t hToken, HxList_t *pRootChList, CO_CHANNEL_t **ret_ppstCoCh)
{
	CO_CHANNEL_t	*pstCoCh = NULL;
	HINT32			nPIDsCount = 0;
	HUINT16 		usPIDs[DxRSV_PIDTABLE_COUNT] = {0, };
	HOM_Result_e	eHomResult = eHOM_Result_Ok;
	HCHAR			*pszPIDs = NULL;

	PLStreaming_ModeSatIp_t 	*pSatIp = NULL;

    if(NULL == pstModeInst)
	{
        HxLOG_Error("invalid args(pstModeInst is null)\n");
		return eHOM_Result_Error;
	}

    pSatIp 		= (&pstModeInst->Mode.stSatIp);

	*ret_ppstCoCh = NULL;

	pszPIDs = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_PIDS);
	if( NULL != pszPIDs)
	{
	    HxLOG_Print(HxANSI_COLOR_PURPLE("pszPIDs(%s)\n"), pszPIDs);
        if (HxSTD_StrStr(pszPIDs, "all" ) != NULL) pstModeInst->PidFlag = MASK_PID_ALL;
        if (HxSTD_StrStr(pszPIDs, "none") != NULL) pstModeInst->PidFlag = MASK_PID_NONE;

        if (pstModeInst->PidFlag == 0) // case of pids="1,2,..."
        {
            eHomResult = _plstreaming_ModeSatIp_GetPids(pszPIDs, &nPIDsCount, usPIDs);
            if(eHOM_Result_Ok != eHomResult)
            {
                HxLOG_Error("_plstreaming_ModeSatIp_GetPids error!, usPIDs(%s)\r\n", usPIDs);
                return eHomResult;
            }
        }
        else if (pstModeInst->PidFlag & MASK_PID_NONE)
        {
            nPIDsCount = 1;  // 2015/2/11 mhkang: pids=none 이라도 sama에게 최소 1개 pid를 설정해야 한다. pids=0 로 하자.
            usPIDs[0] = 0;
        }
        else if (pstModeInst->PidFlag & MASK_PID_ALL)
        {
            nPIDsCount = 1;
            usPIDs[0] = PL_STREAMING_ALL_PID;
        }
        else
        {
            nPIDsCount = 0;
            HxLOG_Warning(HxANSI_COLOR_PURPLE("Unknown PidFlag:%d\n"), pstModeInst->PidFlag);
        }

        _plstreaming_ModeSatIp_ReplacePids(pstModeInst, nPIDsCount, usPIDs);
	}
    else
    {
	    HxLOG_Print(HxANSI_COLOR_PURPLE("pszPIDs is NULL (count = %d)\n"), pstModeInst->nPIDsCount);
        if (pstModeInst->nPIDsCount == 0)
        {
            pstModeInst->PidFlag = MASK_PID_NONE;
            nPIDsCount = 1;
            usPIDs[0] = 0;
            _plstreaming_ModeSatIp_ReplacePids(pstModeInst, nPIDsCount, usPIDs);
        }
        else
        {
            /* 2015/4/11 mhkang: http://svn.humaxdigital.com:3000/issues/102625
              RTSP SETUP+SID+TP(pids가 없는 경우) 이전 RTSP Message에서 사용된 PIDS정보를 사용해야 한다.
              do nothing here.
            */
        }
    }

	if(NULL == pstCoCh)
    {
        eHomResult = _plstreaming_ModeSatIp_MakeTuningInfoByString(pstModeInst, hToken, pRootChList, pszPIDs, nPIDsCount, usPIDs);
		if(eHOM_Result_Ok != eHomResult)
		{
			HxLOG_Error("_plstreaming_ModeSatIp_MakeTuningInfoByString error!! \r\n");
		}
		pstCoCh = NULL;	// chnnel info는 비어있다.
	}

	*ret_ppstCoCh = pstCoCh;

	if(eHOM_Result_Ok != eHomResult)
	{
		if(NULL == pSatIp->pstTS)
		{
			HxLOG_Error("eHomResult(%d)\r\n", eHomResult);
			HxLOG_Error("Can not find svc && tp info \r\n");
			eHomResult = eHOM_Result_Error;
		}
		else
		{
			HxLOG_Warning("eHomResult(%d)\r\n", eHomResult);
			HxLOG_Warning("Can not find svc \r\n");
			eHomResult = eHOM_Result_Ok;
		}
	}


	return eHomResult;
}


STATIC HOM_Result_e _plstreaming_ModeSatIp_ParseTPString(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pszStreamingPath, HUID *ret_puUID)
{
	HERROR				hErr = ERR_OK;
	HxList_t			*pRootChList = NULL;
	HxTOKEN_Handle_t	hToken = NULL;
	HOM_Result_e		eHomResult = eHOM_Result_Ok;
	HCHAR				*pszStreamingDupPath = NULL;
	CO_CHANNEL_t 		*pstCoCh = NULL;
	HCHAR				*pszTokenPath = NULL;

	*ret_puUID = 0;

	pszStreamingDupPath = HLIB_STD_StrDup(pszStreamingPath);
	if(NULL == pszStreamingDupPath)
	{
		HxLOG_Error("HLIB_STD_StrDup error!, eHOM_Result_Error_OutofMem \r\n");
		eHomResult = eHOM_Result_Error_OutofMem;
		goto GETUID_FNC_END;
	}

	pszTokenPath = _plstreaming_ModeSatIp_ChangeTokenURL(pszStreamingDupPath);

	hErr = HLIB_TOKEN_Parse(eTokenStrType_URL, pszTokenPath, &hToken);
	if(ERR_OK != hErr)
	{
        HxLOG_Error("HLIB_TOKEN_Parse error! pszTokenPath(%s) (hErr=%d)\n", pszTokenPath, hErr);
		eHomResult = eHOM_Result_Error;
		goto GETUID_FNC_END;
	}

    eHomResult = _plstreaming_ModeSatIp_FindSvc(pstModeInst, hToken, pRootChList, &pstCoCh);
	if(eHOM_Result_Ok != eHomResult)
	{
        HxLOG_Error("_plstreaming_ModeSatIp_FindSvc failed(eHomResult:%d)\n", eHomResult);
	}

	if( NULL != pstCoCh && NULL != pstCoCh->svc)
	{
		*ret_puUID = pstCoCh->svc->uid;
	}


GETUID_FNC_END:


	if(NULL != hToken)
	{
		HLIB_TOKEN_FreeHandle(hToken);
		hToken = NULL;
	}
	if(NULL != pszStreamingDupPath)
	{
		HLIB_STD_MemFree(pszStreamingDupPath);
	}

	return eHomResult;
}

#define __SATIP_SVC_SAT_INFO__

// satip v1.2 spec 에서 참조
STATIC HINT32	_plstreaming_ModeSatIp_SAT_SrcNumToEnum(DxAntInfo_t	*pstAntInfo)
{
    if (pstAntInfo == NULL)  // 2014/10/23 mhkang: http://svn.humaxdigital.com:3000/issues/97849
    {
        HxLOG_Error("Invalid args(pstAntInfo is null)\n");
        return -1;
    }
	switch(pstAntInfo->antType)
	{
		case eDxANT_TYPE_LNB_ONLY:
			return (HINT32)eSatipSrcType_LNB_ONLY;

		case eDxANT_TYPE_DISEQC:
			switch(pstAntInfo->info.diseqc.diseqcInput)
			{
			case eDxDISEQC_INPUT_A:		return (HINT32)eSatipSrcType_DISEQC_A;
			case eDxDISEQC_INPUT_B:		return (HINT32)eSatipSrcType_DISEQC_B;
			case eDxDISEQC_INPUT_C:		return (HINT32)eSatipSrcType_DISEQC_C;
			case eDxDISEQC_INPUT_D:		return (HINT32)eSatipSrcType_DISEQC_D;
			default:
				break;
			}
			break;

		case eDxANT_TYPE_SCD:
			switch(pstAntInfo->info.scd.scdInput)
			{
			case eDxSCD_INPUT_SCD_A:	return (HINT32)eSatipSrcType_SCD_A;
			case eDxSCD_INPUT_SCD_B:	return (HINT32)eSatipSrcType_SCD_B;
			default:
				break;
			}
			break;

		case eDxANT_TYPE_SMATV:
			return (HINT32)eSatipSrcType_SMATV;

		case eDxANT_TYPE_MONOBLOCK_LNB:
			return (HINT32)eSatipSrcType_MONOBLOCK_LNB;
	}

	return (HINT32)eSatipSrcType_None;
}

#if 0
STATIC plSatipSrcType_e	_plstreaming_ModeSatIp_SAT_EnumToSrcNum(HINT32 nSrcNum, DxAntennaType_e *peAntType, HINT32 *pnInputNum)
{
	switch((plSatipSrcType_e)nSrcNum)
	{
		/* LNB */
	case eSatipSrcType_LNB_ONLY:
		*peAntType = eDxANT_TYPE_LNB_ONLY;
		*pnInputNum = nSrcNum;
		break;

		/* DISEQC */
//	case eSatipSrcType_DISEQC_A:		// A는 LNB와 같은 값으로..
	case eSatipSrcType_DISEQC_B:
	case eSatipSrcType_DISEQC_C:
	case eSatipSrcType_DISEQC_D:
		*peAntType = (eDxANT_TYPE_DISEQC-eSatipSrcType_DISEQC);
		*pnInputNum = nSrcNum;
		break;

		/* SCD */
	case eSatipSrcType_SCD:
	case eSatipSrcType_SCD_A:
	case eSatipSrcType_SCD_B:
		*peAntType = eDxANT_TYPE_SCD;
		*pnInputNum = (nSrcNum-eSatipSrcType_SCD);
		break;

		/* SMATV */
	case eSatipSrcType_SMATV:
		*peAntType = eDxANT_TYPE_SMATV;
		*pnInputNum = (nSrcNum-eSatipSrcType_SMATV);
		break;

		/* MONOBLOCK LNB */
	case eSatipSrcType_MONOBLOCK_LNB:
		*peAntType = eDxANT_TYPE_SMATV;
		*pnInputNum = (nSrcNum-eSatipSrcType_MONOBLOCK_LNB);
		break;

	default:
		break;
	}

	return (plSatipSrcType_e)nSrcNum;
}
#endif

STATIC HCHAR *_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxFecToString(DxSat_CodeRate_e eFec)
{
	switch(eFec)
	{
	case	eDxSAT_CODERATE_AUTO:	return "auto";
	case	eDxSAT_CODERATE_NONE:	return "none";
	case	eDxSAT_CODERATE_1_2 :	return "12";
	case	eDxSAT_CODERATE_2_3 :	return "23";
	case	eDxSAT_CODERATE_3_4 :	return "34";
	case	eDxSAT_CODERATE_5_6 :	return "56";
	case	eDxSAT_CODERATE_7_8 :	return "78";

		/* DVBS2 Code Rate */
	case	eDxSAT_CODERATE_3_5 :	return "35";
	case	eDxSAT_CODERATE_4_5 :	return "45";
	case	eDxSAT_CODERATE_5_11:	return "511";
	case	eDxSAT_CODERATE_6_7 :	return "67";
	case	eDxSAT_CODERATE_8_9 :	return "89";
	case	eDxSAT_CODERATE_9_10:	return "910";

	default:
		break;
	}

	return NULL;
}


STATIC DxSat_CodeRate_e _plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_StringToDxFec(HCHAR *pszFecString)
{
	HINT32	i = 0;
	HCHAR	*pName = NULL;

	for(i=0; i <= (HINT32)eDxSAT_CODERATE_9_10; i++)
	{
		pName = _plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxFecToString((DxSat_CodeRate_e)i++);
		if(NULL == pName)
		{
			break;
		}
		if(0 == HxSTD_StrNCmp(pName, pszFecString, HxSTD_StrLen(pName)))
		{
			return (DxSat_CodeRate_e)i;
		}
	}

	return eDxSAT_CODERATE_AUTO;
}

STATIC HCHAR *_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxSatPolarization_eToString(DxSat_Polarization_e ePol)
{
	switch(ePol)
	{
	case	eDxSAT_POLAR_AUTO:		return "auto";
	case	eDxSAT_POLAR_HOR:		return "h";
	case	eDxSAT_POLAR_VER:		return "v";
	case	eDxSAT_POLAR_LEFT:		return "l";
	case	eDxSAT_POLAR_RIGHT:		return "r";

	default:
		break;
	}

	return NULL;
}



STATIC HCHAR *_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxSatRollOffToString(DxSat_RollOff_e eRoll)
{
	switch(eRoll)
	{
	case	eDxSAT_ROLL_020:		return "0.20";
	case	eDxSAT_ROLL_025:		return "0.25";
	case	eDxSAT_ROLL_035:		return "0.35";

	default:
		break;
	}

	return NULL;
}


STATIC	void	_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_TpInfoToString(DxSat_TuningTPInfo_t	*pstTpTuningInfo, HCHAR *pszString, HINT32 nStrBufSize)
{
	nStrBufSize--;
	if(pstTpTuningInfo->eTransSpec == eDxSAT_TRANS_DVBS2)
	{
		HxSTD_snprintf(pszString, nStrBufSize, "&freq=%d&pol=%s&ro=%s&msys=%s&mtype=%s&plts=%s&sr=%d&fec=%s",
												pstTpTuningInfo->ulFrequency,
												_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxSatPolarization_eToString(pstTpTuningInfo->ePolarization),
												/* dvb s2 */_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxSatRollOffToString(pstTpTuningInfo->eRollOff),
												(pstTpTuningInfo->eTransSpec == eDxSAT_TRANS_DVBS) ? "dvbs" : "dvbs2",
												/* dvb s2 */(pstTpTuningInfo->ePskMod == eDxSAT_PSK_8PSK) ? "8psk" : "qpsk",
												/* dvb s2 */(pstTpTuningInfo->ePilot == eDxSAT_PILOT_ON) ? "on" : "off",
												pstTpTuningInfo->ulSymbolRate,
												_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxFecToString(pstTpTuningInfo->eFecCodeRate)
												);
	}
	else
	{
		HxSTD_snprintf(pszString, nStrBufSize, "&freq=%d&pol=%s&msys=%s&sr=%d&fec=%s",
												pstTpTuningInfo->ulFrequency,
												_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxSatPolarization_eToString(pstTpTuningInfo->ePolarization),
												(pstTpTuningInfo->eTransSpec == eDxSAT_TRANS_DVBS) ? "dvbs" : "dvbs2",
												pstTpTuningInfo->ulSymbolRate,
												_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_DxFecToString(pstTpTuningInfo->eFecCodeRate)
												);
	}
}


STATIC HxSTR_t *_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT(const CO_CHANNEL_t	*pstCoCh, HxSTR_t	*pStr)
{
	DxService_t 		*pstSvc	= pstCoCh->svc;
	DxTransponder_t		*pstTS	= pstCoCh->ts;
	HOM_Result_e		eHomResult = eHOM_Result_Ok;
	HCHAR				szStrBuf[512] = {0, };
	HBOOL				bIsAppendPIDs = FALSE;


	//DxSatTuningInfo_t		*pstTuningInfo 	= &pstTS->tuningParam.sat;
	DxSat_TuningTPInfo_t	*pstTpTuningInfo= &pstTS->tuningParam.sat.tuningInfo;
	DxAntInfo_t				*pstAntInfo		= &pstTS->tuningParam.sat.antInfo;
	HINT32					nSrcNum = 0;

	(void)eHomResult;

	//ENTER_FUNCTION;

	nSrcNum = _plstreaming_ModeSatIp_SAT_SrcNumToEnum(pstAntInfo);

	HxLOG_Assert(pStr); // rtsp://192.168.1.202/?src=

	{	/* ant info */
		pStr = HLIB_STR_AppendFormat(pStr, "%d", nSrcNum);
		HxLOG_Assert(pStr);	// rtsp://192.168.1.202/?src=1&
		HxLOG_Info("uri(%s)\r\n", HLIB_STR_GetBuffer(pStr));
	}

	{	/* tp info */
		HxSTD_memset(szStrBuf, 0, sizeof(HCHAR) * 512);

		_plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT_TpInfoToString(pstTpTuningInfo, szStrBuf, 512);
		if(0 != szStrBuf[0])
		{
			pStr = HLIB_STR_Append(pStr, szStrBuf);
			HxLOG_Assert(pStr);	// rtsp://192.168.1.202/?src=1&freq=11597&pol=v&msys=dvbs&mtype=qpsk&sr=22000&fec=56&
			HxLOG_Info("uri(%s)\r\n", HLIB_STR_GetBuffer(pStr));
		}
	}

#if 1 // need test area
	/* svc id */
	pStr = HLIB_STR_AppendFormat(pStr, "&svcuid=%d", pstSvc->uid);
#endif

	{ /* pids */
		HCHAR		szTempBuf[128] = {0, };
		HUINT32		ulPID = 0;

		HxSTD_memset(szStrBuf, 0, sizeof(HCHAR) * 512);

		HxSTD_snprintf(szStrBuf, 256, "&pids=0");
		ulPID = (HUINT32)pstSvc->pmtPid;
		if(ulPID < 0x2000)
		{
			HxSTD_snprintf(szTempBuf, 128, ",%d", ulPID);
			HxSTD_StrNCat(szStrBuf, szTempBuf, 256);
		}

		ulPID = (HUINT32)pstSvc->pcrPid;
		if(ulPID < 0x2000)
		{
			HxSTD_snprintf(szTempBuf, 128, ",%d", ulPID);
			HxSTD_StrNCat(szStrBuf, szTempBuf, 256);
		}

		ulPID = (HUINT32)pstSvc->videoPid;
		if(ulPID < 0x2000)
		{
			HxSTD_snprintf(szTempBuf, 128, ",%d", ulPID);
			HxSTD_StrNCat(szStrBuf, szTempBuf, 256);
		}

		ulPID = (HUINT32)pstSvc->audioPid;
		if(ulPID < 0x2000)
		{
			HxSTD_snprintf(szTempBuf, 128, ",%d", ulPID);
			HxSTD_StrNCat(szStrBuf, szTempBuf, 256);
		}

		ulPID = (HUINT32)pstSvc->ttxPid;
		if(ulPID < 0x2000)
		{
			HxSTD_snprintf(szTempBuf, 128, ",%d", ulPID);
			HxSTD_StrNCat(szStrBuf, szTempBuf, 256);
		}

		//if(0 != szTempBuf[0])
		{
			bIsAppendPIDs = TRUE;
		}
	}

	/* svc info */
	if(TRUE == bIsAppendPIDs)
	{
		pStr = HLIB_STR_Append(pStr, szStrBuf);
		HxLOG_Assert(pStr);
	}

	HxLOG_Info("uri(%s)\r\n", HLIB_STR_GetBuffer(pStr));

	//LEAVE_FUNCTION;

	return pStr;
}


#define __SATIP_SVC_CAB_INFO__
STATIC HxSTR_t *_plstreaming_ModeSatIp_GenM3U8SvcInfo_CAB(const CO_CHANNEL_t	*pstCoCh, HxSTR_t	*pStr)
{
	DxService_t 		*pstSvc	= pstCoCh->svc;
	DxTransponder_t		*pstTS	= pstCoCh->ts;
	HOM_Result_e		eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	/* need add */
	(void)pstSvc;
	(void)pstTS;
	(void)eHomResult;

	LEAVE_FUNCTION;

	return pStr;
}


#define __SATIP_SVC_TER_INFO__
STATIC HxSTR_t * _plstreaming_ModeSatIp_GenM3U8SvcInfo_TER(const CO_CHANNEL_t	*pstCoCh, HxSTR_t	*pStr)
{
	DxService_t 		*pstSvc	= pstCoCh->svc;
	DxTransponder_t		*pstTS	= pstCoCh->ts;
	HOM_Result_e		eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	/* need add */
	(void)pstSvc;
	(void)pstTS;
	(void)eHomResult;

	LEAVE_FUNCTION;

	return pStr;
}

#define __SATIP_SVC_INFO__

STATIC HOM_Result_e _plstreaming_ModeSatIp_GenM3U8SvcInfo(const CO_CHANNEL_t	*pstCoCh, PL_M3U8_t *pstM3U8, const HCHAR * pszIpStr)
{
	HERROR				hErr = ERR_OK;
	HCHAR 				szTempBuf[512] = {0, };
	HOM_Result_e		eHomResult = eHOM_Result_Ok;
	DxService_t 		*pstSvc	= pstCoCh->svc;
	DxTransponder_t		*pstTS	= pstCoCh->ts;
	HxSTR_t				*pStr = NULL;

	//ENTER_FUNCTION;

	if(NULL == pstSvc)
	{
		HxLOG_Print("pstSvc = NULL\r\n");
		return eHOM_Result_Error;
	}

	if(pstSvc->lcn <= 0)
	{
		HxLOG_Print("LCN == 0, name(%s)\r\n", pstSvc->name);
		return eHOM_Result_Ok;
	}

	if(NULL == pstTS)
	{
		HxLOG_Error("pstSvc = NULL, name(%s)\r\n", pstSvc->name);
		return eHOM_Result_Error;
	}

	HxSTD_memset(szTempBuf, 0, sizeof(HCHAR) * 512);
	HxSTD_snprintf(szTempBuf, 512, "%d. %s", pstSvc->lcn, pstSvc->name);

	hErr = PL_M3U8_AppendInf(pstM3U8, 0, szTempBuf);
	if( ERR_OK != hErr)
	{
		HxLOG_Error("PL_M3U8_AppendInf fail, svc name(%s)\r\n", pstSvc->name);
		return eHOM_Result_Error;
	}

	HxSTD_memset(szTempBuf, 0, sizeof(HCHAR) * 512);

	pStr = HLIB_STR_New(pszIpStr);
	if(NULL == pStr)
	{
		HxLOG_Error("HLIB_STR_New fail, svc name(%s), pszIpStr(%s)\r\n", pstSvc->name, pszIpStr);
		return eHOM_Result_Error;
	}

	pStr = HLIB_STR_Append(pStr, "/?src=");
	HxLOG_Assert(pStr);

	if(eDxDELIVERY_TYPE_SAT & pstTS->tuningParam.eDeliType)
	{
		pStr = _plstreaming_ModeSatIp_GenM3U8SvcInfo_SAT(pstCoCh, pStr);
	}
	else if(eDxDELIVERY_TYPE_TER & pstTS->tuningParam.eDeliType)
	{
		pStr = _plstreaming_ModeSatIp_GenM3U8SvcInfo_TER(pstCoCh, pStr);
	}
	else if(eDxDELIVERY_TYPE_CAB & pstTS->tuningParam.eDeliType)
	{
		pStr = _plstreaming_ModeSatIp_GenM3U8SvcInfo_CAB(pstCoCh, pStr);
	}
	else
	{
		HxLOG_Error("pstTS->tuningParam.eDeliType select fail(0x%x)\r\n", pstTS->tuningParam.eDeliType);
		eHomResult = eHOM_Result_Error;
	}

	if(eHOM_Result_Ok != eHomResult)
	{
		HLIB_STR_Delete(pStr);
		HxLOG_Error("PL_M3U8_AppendInf fail, svc name(%s)\r\n", pstSvc->name);
		return eHOM_Result_Error;
	}

	if(HLIB_STR_GetLength(pStr) == 0)
	{
		HLIB_STR_Delete(pStr);
		HxLOG_Error("pStr make fail\r\n", pstSvc->name);
		return eHOM_Result_Error;
	}

	hErr = PL_M3U8_AppendHxSTR(pstM3U8, pStr);
	if( ERR_OK != hErr)
	{
		HLIB_STR_Delete(pStr);
		HxLOG_Error("PL_M3U8_AppendHxSTR fail, svc name(%s)\r\n", pstSvc->name);
		return eHOM_Result_Error;
	}
	HLIB_STR_Delete(pStr);

	//LEAVE_FUNCTION;

	return eHomResult;
}


STATIC HxSTR_t *_plstreaming_ModeSatIp_GenM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath)
{
	HERROR			hErr = ERR_OK;
	HxList_t 		*pChList = NULL;
	HxList_t 		*pRootChList = NULL;
	HxSTR_t			*pM3U8Str = NULL;
	PL_M3U8_t		*pstM3U8 = NULL;
	CO_CHANNEL_t	*pstCoCh = NULL;
	HOM_Result_e	eHomResult = eHOM_Result_Ok;

	pstM3U8 = PL_M3U8_New(PLM3U8_QUEUESIZE_INFINITY);
	if(NULL == pstM3U8)
	{
		HxLOG_Error("PL_M3U8_New\r\n");
		return NULL;
	}

	hErr = PL_COMMON_GetChannelList(&pRootChList);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Error> PL_COMMON_GetChannelList\r\n");
		return NULL;
	}

	pChList = pRootChList;
	while(pChList)
	{
		pstCoCh = HLIB_LIST_Data(pChList);
		if(NULL == pstCoCh)
		{
			pChList = HLIB_LIST_Next(pChList);
			continue;
		}
		if(NULL == pstCoCh->svc || NULL == pstCoCh->ts)
		{
			pChList = HLIB_LIST_Next(pChList);
			continue;
		}

		eHomResult = _plstreaming_ModeSatIp_GenM3U8SvcInfo(pstCoCh, pstM3U8, pRequestPath);
		if(eHOM_Result_Ok != eHomResult)
		{
			HxLOG_Error("_plstreaming_ModeSatIp_GenM3U8SvcInfo fail, svc uid(0x%x)\r\n", pstCoCh->svc->uid);
		}

		pChList = HLIB_LIST_Next(pChList);
	}

	PL_COMMON_ReleaseChannelList(pRootChList);

	PL_M3U8_AppendEndList(pstM3U8);

	pM3U8Str = PL_M3U8_ToBuffer(pstM3U8);
	PL_M3U8_Delete(pstM3U8);


	return pM3U8Str;
}

STATIC HOM_Result_e _plstreaming_ModeSatIp_GetM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HINT32 *pnM3U8Size, HUINT8 **ppucM3U8Buf)
{
	HERROR			hErr = ERR_OK;
	HINT32			nSize = 0;
	HINT32			nLastVersion = 0;
	HUINT8			*pBuffer = NULL;

	STATIC HUINT32	s_ulM3U8SemId = 0;

	plStreamingModeSatIpMgr_t	*pMgr = NULL;

	hErr = PL_COMMON_GetLastVersion(&nLastVersion);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("PL_COMMON_GetLastVersion error!! \r\n");
	}

	if(0 == s_ulM3U8SemId)
	{
		hErr = HxSEMT_Create(&s_ulM3U8SemId, "HommaSatipM3u8", HxSEMT_FIFO);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("HxSEMT_Create fail!! \r\n");
			return eHOM_Result_Error;
		}
	}

	HxSEMT_Get(s_ulM3U8SemId);
	pMgr = &s_pstSatIpMgr;

	if(NULL == pMgr->pstM3U8Str || nLastVersion != pMgr->nChListLastVerNum )
	{
		pMgr->pstM3U8Str = _plstreaming_ModeSatIp_GenM3U8(pstInst, pRequestPath);
		if(NULL != pMgr->pstM3U8Str)
		{
			pMgr->nChListLastVerNum = nLastVersion;
		}
	}

	if(NULL != pMgr->pstM3U8Str)
	{
		nSize			= HLIB_STR_GetLength(pMgr->pstM3U8Str);
		pBuffer 		= HLIB_STR_toUTF8(pMgr->pstM3U8Str);

		*pnM3U8Size 	= nSize;
		*ppucM3U8Buf	= HLIB_STD_StrDup(pBuffer);
	}

	HxSEMT_Release(s_ulM3U8SemId);

	return eHOM_Result_Ok;
}

STATIC HOM_Result_e _plstreaming_ModeSatIp_Str2Info(PLStreaming_ModeInst_t *pstInstMode, const HCHAR *pRequestPath)
{
	Handle_t		hProfileHandle = HANDLE_NULL;
	HUID			uUID = 0;
	HOM_Result_e  	eHomResult = eHOM_Result_Ok;

	PLStreaming_ModeSatIp_t		*pSatIp = NULL;
	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;
	plStreamingInst_t			*pOwnerInst= NULL;

    pSatIp 		= (&pstInstMode->Mode.stSatIp);
    pSetupInfo 	= (&pstInstMode->stSetupInfo);

	HxLOG_Debug("pRequestPath (%s)\r\n", pRequestPath);

	if (pSatIp->bIsOwner == TRUE)  // SAT>IP Owner
	{
        eHomResult = _plstreaming_ModeSatIp_ParseTPString(pstInstMode, pRequestPath, &uUID);
		if(eHOM_Result_Ok != eHomResult)
		{
		    HxLOG_Error("_plstreaming_ModeSatIp_ParseTPString Fail (eHomResult:%d)\n", eHomResult);
			return eHOM_Result_Error;
		}

		if(0 != uUID)
		{
			pSetupInfo->uUID = uUID;
		}

		if(NULL != pSetupInfo->pstMediaProfile)
		{
			PL_MediaProfile_GetHandle(pSetupInfo->pstMediaProfile, &hProfileHandle);
		}

        if(NULL != pstInstMode->pstStreamingKey)
		{
            PL_StreamingKey_Delete(pstInstMode->pstStreamingKey);
		}
        pstInstMode->pstStreamingKey	=  PL_StreamingKey_New(pSetupInfo->pszPrefix, uUID, hProfileHandle, pSetupInfo->pszDeviceID);
        if(NULL == pstInstMode->pstStreamingKey)
		{
			HxLOG_Error(" PL_StreamingKey_New error!! \r\n");
			return eHOM_Result_Error;
		}

        if(HANDLE_NULL == pstInstMode->hRingbuf || 0 == pstInstMode->hRingbuf)
		{
            pstInstMode->hRingbuf = _plstreaming_ModeSatIp_CreateRingbuf(pstInstMode, pRequestPath);
            if(HANDLE_NULL == pstInstMode->hRingbuf || 0 == pstInstMode->hRingbuf)
			{
				HxLOG_Error("_plstreaming_ModeSatIp_CreateRingbuf Fail \r\n");
				return eHOM_Result_Error;
			}
		}
		else
		{
            PL_RINGBUF_ClearBuffer(pstInstMode->hRingbuf);
		}

        if (pstInstMode->Mode.stSatIp.pUdsObj == NULL) // 2015/3/28 mhkang: 기존에 만든 UDS Manager가 있다면 다시 생성하면 안된다(좀비 thread 늘어남. http://svn.humaxdigital.com:3000/issues/102113)
        {
            pstInstMode->Mode.stSatIp.pUdsObj = UDS_CreateManager( pstInstMode->stSetupInfo.serverfd, pstInstMode->hRingbuf);
        }

#ifdef CONFIG_DEBUG
		pSatIp->ulStartReadTick = HLIB_STD_GetSystemTick();
#endif
	}
	else // SAT>IP No-owner
	{
        HxLOG_Info("SAT>IP No-onwer\n");

        /* 2015/3/27 mhkang: owner의 UDS Manager에 non-owner의 socketfd 등록하기.
        owner streaming 종료될 때 non-owner streaming 먼저 종료처리한 후 owner streaming 종료처리되어야 한다.
        왜? non-owner의 pUdsObj를 owner의 pUdsObj 포인터를 그대로 사용중이니까.
        만약 이게 안된다면 pUdsObj를 ReferenceCount 관리되어 RefCnt==0 일때 삭제되도록 해야 한다.
        */
        pOwnerInst = PL_Streaming_Context_FindStreamingInstanceByStreamId(TRUE, pSatIp->nStreamId);
        if (pOwnerInst == NULL)
        {
            HxLOG_Error("Not Found owner stream inst(streamID:%d)\n", pSatIp->nStreamId);
            return eHOM_Result_Error;
        }

        pstInstMode->Mode.stSatIp.pUdsObj = pOwnerInst->pstModeInst->Mode.stSatIp.pUdsObj;
        UDS_AddSocket(pstInstMode->Mode.stSatIp.pUdsObj, pstInstMode->stSetupInfo.serverfd );
	}

	return eHOM_Result_Ok;
}

STATIC HBOOL _plstreaming_ModeSatIp_IsAllPIDs(HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	HINT32 i=0;

	for(i=0; i < nPIDsCount; i++)
	{
		if(PL_STREAMING_ALL_PID == usPIDs[i])
		{
			return TRUE;
		}
	}
	return FALSE;
}

STATIC HBOOL _plstreaming_ModeSatIp_ReplacePids(PLStreaming_ModeInst_t *pstInst, HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	HBOOL	bIsAllPids = FALSE;
	HBOOL	bIsModPids = FALSE;
	HINT32	i = 0, j = 0;

	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;

	pSetupInfo 	= (&pstInst->stSetupInfo);

	bIsAllPids = _plstreaming_ModeSatIp_IsAllPIDs(nPIDsCount, usPIDs);
	if(TRUE == bIsAllPids)
	{
		pSetupInfo->eStreamType = DxRSVSTREAM_TYPE_SATIP_LIVE_ALLPID;
	}

	if(pstInst->nPIDsCount >= nPIDsCount)
	{
		for(i=0; i < pstInst->nPIDsCount; i++)
		{
			for(j=0; j < nPIDsCount; j++)
			{
				if(pstInst->usPIDs[i] == usPIDs[i])
				{
					break;
				}
			}

			if(nPIDsCount <= j)
			{
				bIsModPids = TRUE;
				break;
			}
		}

		if(FALSE == bIsModPids)
		{
			HxLOG_Print("no change\n");
			return FALSE;
		}
	}

	pstInst->nPIDsCount	= nPIDsCount;
	HxSTD_memcpy(pstInst->usPIDs, usPIDs, sizeof(HUINT16) * DxRSV_PIDTABLE_COUNT);

	PL_Streaming_Debug_PrintPids(pstInst->nPIDsCount, pstInst->usPIDs, __FUNCTION__, __LINE__);


	return TRUE;
}


STATIC HBOOL _plstreaming_ModeSatIp_UpdateAddPids(PLStreaming_ModeInst_t *pstInst, HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	HINT32	i=0, j=0;
	HBOOL	bIsAdd = FALSE;
	HBOOL	bIsAllPids = FALSE;

	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;

	pSetupInfo 	= (&pstInst->stSetupInfo);

	bIsAllPids = _plstreaming_ModeSatIp_IsAllPIDs(nPIDsCount, usPIDs);
	if(TRUE == bIsAllPids)
	{
		pSetupInfo->eStreamType = DxRSVSTREAM_TYPE_SATIP_LIVE_ALLPID;
	}

	HxLOG_Info("nPIDsCount(%d)  pstInst->nPIDsCount(%d)\r\n", nPIDsCount,  pstInst->nPIDsCount);

	for(i=0; i < nPIDsCount; i++)
	{
		for(j=0; j < pstInst->nPIDsCount; j++)
		{
			if(pstInst->usPIDs[j] == usPIDs[i])
			{
				break;
			}
		}
		if(j >= pstInst->nPIDsCount)
		{
			HxLOG_Info("add pid (%d)\r\n", usPIDs[i]);
			pstInst->usPIDs[pstInst->nPIDsCount] = usPIDs[i];
			pstInst->nPIDsCount++;
			bIsAdd = TRUE;
		}
	}

	PL_Streaming_Debug_PrintPids(pstInst->nPIDsCount, pstInst->usPIDs, __FUNCTION__, __LINE__);

	return bIsAdd;
}

STATIC HBOOL _plstreaming_ModeSatIp_RemoveAllPids(PLStreaming_ModeInst_t *pstInst)
{
    int i;

    pstInst->nPIDsCount = 0;
    for (i = 0 ; i < pstInst->nPIDsCount ; i++)
    {
        pstInst->usPIDs[i] = INVALID_SATIP_PID;
    }

    return TRUE;
}

STATIC HBOOL _plstreaming_ModeSatIp_UpdateDelPids(PLStreaming_ModeInst_t *pstInst, HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	HINT32	i=0, j=0;
	HINT32  nPidCntTemp = 0, nDelCnt = 0;
	HBOOL	bIsDel = FALSE;
	HBOOL	bIsAllPids = FALSE;

	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;

	pSetupInfo 	= (&pstInst->stSetupInfo);

	bIsAllPids = _plstreaming_ModeSatIp_IsAllPIDs(nPIDsCount, usPIDs);
	if(TRUE == bIsAllPids)
	{
		pSetupInfo->eStreamType = DxRSVSTREAM_TYPE_SATIP_LIVE_ALLPID;
	}

	HxLOG_Info("nPIDsCount(%d/%d) \r\n", nPIDsCount, pstInst->nPIDsCount);

	// default pids가 아닌 경우에, invalid 처리
	for(i=0; i < nPIDsCount; i++)
	{
		for(j=0; j < pstInst->nPIDsCount; j++)
		{
			if(pstInst->usPIDs[j] == usPIDs[i])
			{
				HxLOG_Info("del pid (%d)\r\n", usPIDs[i]);
				pstInst->usPIDs[j] = INVALID_SATIP_PID;
				bIsDel = TRUE;
				nDelCnt++;
				break;
			}
		}
	}

	HxLOG_Info("nDelCnt(%d) \r\n", nDelCnt);

	// 하나라도 지워졌으면,
	if(TRUE == bIsDel)
	{
		nPidCntTemp = pstInst->nPIDsCount;
		for(i=0; i < nPidCntTemp; i++)
		{
			if(INVALID_SATIP_PID == pstInst->usPIDs[i])
			{
				for(j=i+1; j < nPidCntTemp; j++)
				{
					if( pstInst->usPIDs[j] != INVALID_SATIP_PID)
					{
						pstInst->usPIDs[i] = pstInst->usPIDs[j];
						pstInst->usPIDs[j] = INVALID_SATIP_PID;
						break;
					}
				}
			}
		}
		}

	pstInst->nPIDsCount -= nDelCnt;

	// too many prints... Activate it when you need this print.
	//PL_Streaming_Debug_PrintPids(pstInst->nPIDsCount, pstInst->usPIDs, __FUNCTION__, __LINE__);

	return bIsDel;
}

STATIC HBOOL _plstreaming_ModeSatIp_UpdateModPids(PLStreaming_ModeInst_t *pstInst, HINT32 nPIDsCount, HUINT16 usPIDs[])
{
	HINT32	i=0;

	HxLOG_Info("nPIDsCount(%d)  pSatIp->nPIDsCount(%d)\r\n", nPIDsCount,  pstInst->nPIDsCount);

	for(i=0; i < nPIDsCount; i++)
	{
		pstInst->usPIDs[i] = usPIDs[i];
	}

	pstInst->nPIDsCount = nPIDsCount;

	PL_Streaming_Debug_PrintPids(pstInst->nPIDsCount, pstInst->usPIDs, __FUNCTION__, __LINE__);

	return (nPIDsCount>0)?TRUE:FALSE;
}


STATIC HBOOL _plstreaming_ModeSatIp_CheckTPorAntInfoChange(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath)
{
	HERROR				hErr = ERR_OK;
	HCHAR				*pszFreq = NULL;
	HBOOL				bIsChangeTP = FALSE;
	HCHAR 				*pszStreamingDupPath = NULL;
	HCHAR				*pszTokenPath = NULL;
	HINT32				nSrcNum = 0;
	HxTOKEN_Handle_t	hToken = NULL;

	PLStreaming_ModeSatIp_t 	*pSatIp = NULL;

    pSatIp		= (&pstModeInst->Mode.stSatIp);

    if(HANDLE_NULL == pstModeInst->hResHandle || 0 == pstModeInst->hResHandle)
	{
		HxLOG_Print("HANDLE_NULL == pstInst->hResHandle\r\n");
		return TRUE;
	}

	if(NULL == pSatIp->pstTS)
	{
		HxLOG_Print("NULL == pSatIp->pstTS\r\n");
		return TRUE;
	}

	pszStreamingDupPath = HLIB_STD_StrDup(pRequestPath);
	if(NULL == pszStreamingDupPath)
	{
		HxLOG_Error("HLIB_STD_StrDup error!\r\n");
		return FALSE;
	}

	pszTokenPath = _plstreaming_ModeSatIp_ChangeTokenURL(pszStreamingDupPath);

	hErr = HLIB_TOKEN_Parse(eTokenStrType_URL, pszTokenPath, &hToken);
	if(ERR_OK != hErr)
	{
		HxLOG_Print("pszTokenPath = %s, hErr = %d \r\n", pszTokenPath, hErr);
		HLIB_STD_MemFree(pszStreamingDupPath);
		HxLOG_Error("HLIB_TOKEN_Parse error!\r\n");
		return FALSE;
	}

	bIsChangeTP = FALSE;

	if(FALSE == bIsChangeTP)	// check src
	{
        nSrcNum = _plstreaming_ModeSatIp_GetSrcNumByToken(pstModeInst, hToken);

        if (pSatIp->nSrc != nSrcNum)
        {
            pSatIp->nSrc = nSrcNum;
            bIsChangeTP = TRUE;
		}
	}

	if(FALSE == bIsChangeTP)	// check freq
	{
		pszFreq = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_FREQ);
		if(NULL != pszFreq)
		{
			HINT32		nFreq = 0;
			HINT32		nInstanceFreq = 0;

			nFreq = HLIB_STD_StrToINT32(pszFreq);
			if( 0 < nFreq)
			{
				nInstanceFreq = _plstreaming_ModeSatIp_GetFreqByTuningParam(&pSatIp->pstTS->tuningParam);
				if(nFreq != nInstanceFreq)
				{
					bIsChangeTP = TRUE;
				}
			}
		}
	}

	if(FALSE == bIsChangeTP)	// check polar
	{
		HCHAR* pszParam = NULL;

		pszParam = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_POLAR);
		if(NULL != pszParam)
		{
			bIsChangeTP = (strcmp(pszParam, _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_Polarization_eToString(pSatIp->pstTS->tuningParam.sat.tuningInfo.ePolarization)) != 0);
		}
	}

	if(FALSE == bIsChangeTP)	// check ro (roll_off)
	{
		HCHAR* pszParam = NULL;

		pszParam = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_ROLL_OFF);
		if(NULL != pszParam)
		{
			bIsChangeTP = (strcmp(pszParam, _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_RollOff_eToString(pSatIp->pstTS->tuningParam.sat.tuningInfo.eRollOff)) != 0);
		}
	}

	if(FALSE == bIsChangeTP)	// check msys
	{
		HCHAR* pszParam = NULL;

		pszParam = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_MODULATION_SYSTEM);
		if(NULL != pszParam)
		{
			bIsChangeTP = (strcmp(pszParam, _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_TransportSpec_eToString(pSatIp->pstTS->tuningParam.sat.tuningInfo.eTransSpec)) != 0);
		}
	}

	if(FALSE == bIsChangeTP)	// check mtype
	{
		HCHAR* pszParam = NULL;

		pszParam = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_MODULATION_TYPE);
		if(NULL != pszParam)
		{
			bIsChangeTP = (strcmp(pszParam, _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_PskModulation_eToString(pSatIp->pstTS->tuningParam.sat.tuningInfo.ePskMod)) != 0);
		}
	}

	if(FALSE == bIsChangeTP)	// check plts
	{
		HCHAR* pszParam = NULL;

		pszParam = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_PILOT_TONES);
		if(NULL != pszParam)
		{
			bIsChangeTP = (strcmp(pszParam, _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_Pilot_eToString(pSatIp->pstTS->tuningParam.sat.tuningInfo.ePilot)) != 0);
		}
	}

	if(FALSE == bIsChangeTP)	// check sr
	{
		HCHAR* pszParam = NULL;
		int sr;

		pszParam = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_SYMBOL_RATE);
		if(NULL != pszParam)
		{
			sr = atoi(pszParam);
			bIsChangeTP = (sr != pSatIp->pstTS->tuningParam.sat.tuningInfo.ulSymbolRate);
		}
	}

	if(FALSE == bIsChangeTP)	// check fec
	{
		HCHAR* pszParam = NULL;

		pszParam = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_FEC);
		if(NULL != pszParam)
		{
			bIsChangeTP = (strcmp(pszParam, _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_CodeRate_eToString(pSatIp->pstTS->tuningParam.sat.tuningInfo.eFecCodeRate)) != 0);
		}
	}

	HLIB_STD_MemFree(pszStreamingDupPath);

	if(NULL != hToken)
	{
		HLIB_TOKEN_FreeHandle(hToken);
	}

	return (bIsChangeTP);
}


STATIC HBOOL _plstreaming_ModeSatIp_CheckPidChange(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath)
{
	HERROR				hErr = ERR_OK;
	HBOOL				bIsAddPID = FALSE;
	HBOOL				bIsDelPID = FALSE;
	HBOOL				bIsModPID = FALSE;
	HINT32				nPIDsCount = 0;
	HUINT16 			usPIDs[DxRSV_PIDTABLE_COUNT] = {0, };
	HCHAR				*pszPIDs = NULL;
	HCHAR 				*pszStreamingDupPath = NULL;
	HOM_Result_e		eHomResult = eHOM_Result_Ok;
	HxTOKEN_Handle_t	hToken = NULL;
	HCHAR				*pszTokenPath = NULL;
    HCHAR               LastPidFlag;

	HxLOG_Debug("pRequestPath = %s \r\n", pRequestPath);

	pszStreamingDupPath = HLIB_STD_StrDup(pRequestPath);
	if(NULL == pszStreamingDupPath)
	{
		HxLOG_Error("HLIB_STD_StrDup error!\r\n");
		return FALSE;
	}

	pszTokenPath = _plstreaming_ModeSatIp_ChangeTokenURL(pszStreamingDupPath);

	hErr = HLIB_TOKEN_Parse(eTokenStrType_URL, pszTokenPath, &hToken);
	if(ERR_OK != hErr)
	{
		HxLOG_Debug("pszTokenPath = %s, hErr = %d \r\n", pszTokenPath, hErr);
		HLIB_STD_MemFree(pszStreamingDupPath);
		HxLOG_Error("HLIB_TOKEN_Parse error!\r\n");
		return FALSE;
	}

    // backup last PidFlag
    LastPidFlag = pstModeInst->PidFlag;
    pstModeInst->PidFlag = 0;

    if (LastPidFlag & MASK_PID_NONE) // 이전 request에서 pids=none이었으면 pid=0를 제거한다.
    {
        usPIDs[0] = 0;
        _plstreaming_ModeSatIp_UpdateDelPids(pstModeInst, 1, usPIDs);
    }

	pszPIDs = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_PIDS);
	HxLOG_Debug("pszPIDs(%s)\r\n", pszPIDs);
	if( NULL != pszPIDs)
	{
        if (HxSTD_StrStr(pszPIDs, "all" ) != NULL) pstModeInst->PidFlag = MASK_PID_ALL;
        if (HxSTD_StrStr(pszPIDs, "none") != NULL) pstModeInst->PidFlag = MASK_PID_NONE;

        if (pstModeInst->PidFlag & MASK_PID_NONE)
        {
            bIsDelPID = _plstreaming_ModeSatIp_RemoveAllPids(pstModeInst); // current pids=none 이면 이전 pids는 다 삭제한다.
        }

        if (pstModeInst->PidFlag & MASK_PID_ALL)
        {
            bIsAddPID = TRUE;

            usPIDs[0] = PL_STREAMING_ALL_PID;

            bIsAddPID = _plstreaming_ModeSatIp_UpdateAddPids(pstModeInst, 1, usPIDs);

        }

        if (pstModeInst->PidFlag == 0)
        {
            eHomResult = _plstreaming_ModeSatIp_GetPids(pszPIDs, &nPIDsCount, usPIDs);
            if(eHOM_Result_Ok == eHomResult)
            {
                HxLOG_Debug("replace pid\n" );
                bIsModPID = _plstreaming_ModeSatIp_ReplacePids(pstModeInst, nPIDsCount, usPIDs);
            }
        }
	}

	pszPIDs = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_ADDPIDS);
	if(NULL != pszPIDs)
	{
        HxLOG_Debug("addpids (%s)\n", pszPIDs);
		eHomResult = _plstreaming_ModeSatIp_GetPids(pszPIDs, &nPIDsCount, usPIDs);
		if(eHOM_Result_Ok == eHomResult)
		{
            HxLOG_Debug("update pid\n" );
            bIsAddPID = _plstreaming_ModeSatIp_UpdateAddPids(pstModeInst, nPIDsCount, usPIDs);
		}
	}
	pszPIDs = HLIB_TOKEN_GetValueString(hToken, SATIP_SAT_QUERY_SYNTAX_DELPIDS);
	if(NULL != pszPIDs)
	{
        HxLOG_Debug("delpids (%s)\n", pszPIDs);
		eHomResult = _plstreaming_ModeSatIp_GetPids(pszPIDs, &nPIDsCount, usPIDs);
		if(eHOM_Result_Ok == eHomResult)
		{
            HxLOG_Debug("update pid\n" );
            bIsDelPID = _plstreaming_ModeSatIp_UpdateDelPids(pstModeInst, nPIDsCount, usPIDs);
		}
	}

    if (pstModeInst->PidFlag & MASK_PID_NONE || pstModeInst->nPIDsCount == 0) // 현재가 pids=none이었으면 pid=0를 추가.
    {
        usPIDs[0] = 0;
        bIsAddPID = _plstreaming_ModeSatIp_UpdateAddPids(pstModeInst, 1, usPIDs);
        pstModeInst->PidFlag |= MASK_PID_NONE; // pids연산후 남은 pid가 없는 경우 pids=none과 동일하게 처리한다.
    }


	HLIB_STD_MemFree(pszStreamingDupPath);
	if(NULL != hToken)
	{
		HLIB_TOKEN_FreeHandle(hToken);
	}

    if (bIsAddPID == FALSE && bIsDelPID == FALSE && bIsModPID == FALSE) // pids정보가 변경되지 않았으면 PidFlag 정보를 rollback한다.
    {
        pstModeInst->PidFlag = LastPidFlag;
    }

	return (bIsAddPID || bIsDelPID || bIsModPID);
}

#define __SATIP_EXT_FUNCTION__

/*
	SDP syntax
		- 3.5.7 Listing available media streams (DESCRIBE) in SAT>IP Protocol Spec. v1.2

	Session Level:
	s=SatIPServer:1 <frontends>
	Media level:
	a=control:stream=<streamID>
	a=fmtp:33 ver=<major>.<minor>;src=<srcID>;tuner=<feID>,<level>,<lock>,<quality>,<frequency>,<pol
	arisation>,<system>,<type>,<pilots>,<roll_off>,<symbol_rate>,<fec_inner>;pids=<pid0>,…,<pidn>
*/

STATIC HCHAR *_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_CodeRate_eToString(DxSat_CodeRate_e eFec)
{
	switch(eFec)
	{
	case	eDxSAT_CODERATE_1_2 :	return "12";
	case	eDxSAT_CODERATE_2_3 :	return "23";
	case	eDxSAT_CODERATE_3_4 :	return "34";
	case	eDxSAT_CODERATE_5_6 :	return "56";
	case	eDxSAT_CODERATE_7_8 :	return "78";

	/* DVBS2 Code Rate */
	case	eDxSAT_CODERATE_3_5 :	return "35";
	case	eDxSAT_CODERATE_4_5 :	return "45";
	case	eDxSAT_CODERATE_5_11:	return "511";
	case	eDxSAT_CODERATE_6_7 :	return "67";
	case	eDxSAT_CODERATE_8_9 :	return "89";
	case	eDxSAT_CODERATE_9_10:	return "910";

	default:						return "";
	}
}

STATIC HCHAR *_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_Polarization_eToString(DxSat_Polarization_e ePol)
{
	switch(ePol)
	{
	case	eDxSAT_POLAR_HOR:		return "h";
	case	eDxSAT_POLAR_VER:		return "v";
	case	eDxSAT_POLAR_LEFT:		return "l";
	case	eDxSAT_POLAR_RIGHT:		return "r";

	default:						return "";
	}
}

STATIC HCHAR *_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_TransportSpec_eToString(DxSat_TransportSpec_e eTransSpec)
{
	switch(eTransSpec)
	{
	case	eDxSAT_TRANS_DVBS:		return "dvbs";
	case	eDxSAT_TRANS_DVBS2:		return "dvbs2";

	default:						return "";
	}
}

STATIC HCHAR *_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_PskModulation_eToString(DxSat_PskModulation_e ePskMod)
{
	switch(ePskMod)
	{
	case	eDxSAT_PSK_QPSK:		return "qpsk";
	case	eDxSAT_PSK_8PSK:		return "8psk";

	default:						return "";
	}
}

STATIC HCHAR *_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_RollOff_eToString(DxSat_RollOff_e eRoll)
{
	switch(eRoll)
	{
	case	eDxSAT_ROLL_020:		return "0.20";
	case	eDxSAT_ROLL_025:		return "0.25";
	case	eDxSAT_ROLL_035:		return "0.35";

	default:						return "";
	}
}

STATIC HCHAR *_plstreaming_ModeSatIp_GenSDP_SAT_DxSat_Pilot_eToString(DxSat_Pilot_e ePilot)
{
	switch(ePilot)
	{
	case	eDxSAT_PILOT_OFF:		return "off";
	case	eDxSAT_PILOT_ON:		return "on";

	default:						return "";
	}
}

/*
	tuner=<feID>,<level>,<lock>,<quality>,<frequency>,<polarisation>,<system>,<type>,<pilots>,<roll_off>,<symbol_rate>,<fec_inner>;
*/
STATIC	HERROR _plstreaming_ModeSatIp_GenSDP_SAT_TunerString(PLStreaming_ModeInst_t *pstModeInst, HCHAR *pszString, HINT32 nStrBufSize)
{
	HERROR		hErr = ERR_FAIL;
	HINT32		nLevel = 0;		/* 0~255 */
	HINT32		nQuality = 0; 	/* 0~15 */
	HBOOL		bIsLocked = FALSE;
	HINT32		freq, sr;
	HCHAR		*pol, *msys, *mtype, *plts, *ro, *fec;
	DxTransponder_t *pSatIpTp;

	DxSat_TuningTPInfo_t	*pstTpTuningInfo;

	if (NULL == pstModeInst)
	{
		HxLOG_Error("Invalid args(pstModeInst is NULL)\n");
		return ERR_FAIL;
	}

	pSatIpTp 		= pstModeInst->Mode.stSatIp.pstTS;
	if(NULL == pSatIpTp)
	{
		HxLOG_Error("pSatIpTp is NULL\n");
		return ERR_FAIL;
	}

	if(NULL == pszString || 0 == nStrBufSize)
	{
		HxLOG_Error("nStrBufSize = %d, pszString = 0x%x\r\n", nStrBufSize, (HUINT32)pszString);
		return ERR_FAIL;
	}

	/* 2015/7/7 mhkang:
		rtsp setup 직후에 tunerid가 바로 할당되지 않을 수 있다.
		이때는 -1 값인데  PL_COMMON_IsTunerLocked 호출하면 crash 된다. 이때는  locking 안된 것으로 처리한다.
	*/
	if (pstModeInst->tunerid == -1)
	{
		nLevel		= 0;
		bIsLocked	= FALSE;
		nQuality	= 0;
	}
	else
	{
		hErr = PL_COMMON_GetSatIpSignalLevel  (pstModeInst->tunerid, &nLevel);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Error> PL_COMMON_GetSatIpSignalLevel()\n");
		}

		hErr = PL_COMMON_IsTunerLocked        (pstModeInst->tunerid, &bIsLocked);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Error> PL_COMMON_IsTunerLocked()\n");
		}

		hErr = PL_COMMON_GetSatIpSignalQuality(pstModeInst->tunerid, &nQuality);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Error> PL_COMMON_GetSatIpSignalQuality\n");
		}
	}

	pstTpTuningInfo = &pSatIpTp->tuningParam.sat.tuningInfo;

	// auto에 대해서 satip spec에서 정의하지 않았으므로 DESCRIBE에서는 처리 안 해 줌

	freq  = pstTpTuningInfo->ulFrequency;
	pol   = _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_Polarization_eToString(pstTpTuningInfo->ePolarization);
	msys  = _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_TransportSpec_eToString(pstTpTuningInfo->eTransSpec);
	mtype = _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_PskModulation_eToString(pstTpTuningInfo->ePskMod);
	plts  = _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_Pilot_eToString(pstTpTuningInfo->ePilot);
	ro    = _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_RollOff_eToString(pstTpTuningInfo->eRollOff);
	sr    = pstTpTuningInfo->ulSymbolRate;
	fec   = _plstreaming_ModeSatIp_GenSDP_SAT_DxSat_CodeRate_eToString(pstTpTuningInfo->eFecCodeRate);

	HxSTD_snprintf(pszString, nStrBufSize,
					"tuner=%d,%d,%d,%d"
					",%d,%s,%s,%s,%s,%s,%d,%s;"
					, (pstModeInst->tunerid + 1), nLevel, bIsLocked ? 1 : 0, nQuality
					, freq, pol, msys, mtype, plts, ro, sr, fec
					);

	return ERR_OK;
}

/*
	pids=<pid0>,…,<pidn>
*/
STATIC	HERROR _plstreaming_ModeSatIp_GenSDP_SAT_PidsString(HINT32 nPidCnt, HUINT16 pusPids[], HCHAR *pszString, HINT32 nStrBufSize)
{
	HxSTR_t		*pStr = NULL;
	HINT32		i = 0;
	HINT32		nBytes = 0;
	HUINT8		*pucStr = NULL;

	if(NULL == pszString || 0 == nStrBufSize)
	{
		HxLOG_Error("nStrBufSize = %d, pszString = 0x%x\r\n", nStrBufSize, (HUINT32)pszString);
		return ERR_FAIL;
	}

	if(NULL == pusPids || 0 == nPidCnt)
	{
		HxSTD_snprintf(pszString, nStrBufSize, "pids=none");
		return ERR_OK;
	}

	pStr = HLIB_STR_New("pids=");
	for(i = 0; i < nPidCnt; i++)
	{
		if((nPidCnt - i) > 1)
		{
			pStr = HLIB_STR_AppendFormat(pStr, "%d,", pusPids[i]);
		}
		else
		{
			pStr = HLIB_STR_AppendFormat(pStr, "%d", pusPids[i]);
		}
	}

	nBytes = HLIB_STR_GetNumOfBytes(pStr);
	if(nBytes >= nStrBufSize || nBytes < 5)
	{
		HLIB_STR_Delete(pStr);
		HxLOG_Error("nBytes(%d) > nStrBufSize(%d)\r\n", nBytes, nStrBufSize);
		return ERR_FAIL;
	}

	pucStr = HLIB_STR_GetBuffer(pStr);
	HxSTD_memcpy(pszString, pucStr, nBytes);
	pszString[nBytes] = '\0';

	HLIB_STR_Delete(pStr);

	return ERR_OK;
}

STATIC HOM_Result_e 	_plstreaming_ModeSatIp_Get_TunerInfo(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData)
{
	HERROR						hErr = ERR_FAIL;
	HCHAR						*pszTuner = NULL;
	HCHAR						*pszPids = NULL;
	HCHAR						*pucData = NULL;
	PLStreaming_ModeSatIp_t		*pSatIp = NULL;

	ENTER_FUNCTION;

	if(NULL == pstModeInst /*  || NULL == pRequestPath */)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) pRequestPath(%s)\r\n", pstModeInst, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	pSatIp 		= (&pstModeInst->Mode.stSatIp);

	if(pulDataSize == NULL || ppucData == NULL) // 2013/12/09 mhkang: CID 151451 (#2 of 2): Dereference after null check (FORWARD_NULL)
	{
		HxLOG_Error("Invalid_Arguments pulDatasize(%p), ppucData(%p)\n", pulDataSize, ppucData);
		return eHOM_Result_Error_NullPoint;
	}

	HxLOG_Debug("eSetupStatus(%d)  pRequestPath(%s)\r\n", pstModeInst->eSetupStatus, pRequestPath);


	if(NULL == pSatIp->pstTS)
	{
		HxLOG_Error("pSatIp->pstTS is NULL (pRequestPath:%s)\r\n", pRequestPath);
		LEAVE_FUNCTION;
		return eHOM_Result_Error;
	}

	/*
		pSetupInfo->pszStreamingPath를 사용하게 되면, DVB Viewer로 서치할 때, SETUP PLAY PLAY PLAY .... TEARDOWN 와 같이 명령이 오기 때문에
		SETUP은 한번만 오게 됨. 따라서 처음 streaming path를 계속 사용하므로, 그 값을 사용하면 안 됨.

		data ex) src=1;tuner=1,0,0,0,10730,v,dvbs2,8psk,on,0.35,22000,23;pids=0
	*/

	pszTuner = HLIB_STD_MemAlloc(TUNER_STRING_BUF_LEN);
	pszPids = HLIB_STD_MemAlloc(TUNER_STRING_BUF_LEN);

	hErr = _plstreaming_ModeSatIp_GenSDP_SAT_TunerString(pstModeInst, pszTuner, TUNER_STRING_BUF_LEN);
	if(ERR_OK != hErr)
	{
		HLIB_STD_MemFree(pszTuner);
		HLIB_STD_MemFree(pszPids);
		LEAVE_FUNCTION;
		return eHOM_Result_Error;
	}

	if (pstModeInst->PidFlag == 0)
    {
		hErr = _plstreaming_ModeSatIp_GenSDP_SAT_PidsString(pstModeInst->nPIDsCount, pstModeInst->usPIDs, pszPids, TUNER_STRING_BUF_LEN);
        if(ERR_OK != hErr)
        {
            HLIB_STD_MemFree(pszTuner);
            HLIB_STD_MemFree(pszPids);
            LEAVE_FUNCTION;
            return eHOM_Result_Error;
        }
    }
    else
    {
		if (pstModeInst->PidFlag & MASK_PID_ALL) HxSTD_snprintf(pszPids, TUNER_STRING_BUF_LEN, "pids=all");
		if (pstModeInst->PidFlag & MASK_PID_NONE) HxSTD_snprintf(pszPids, TUNER_STRING_BUF_LEN, "pids=none");

    }

	pucData = HLIB_STD_FmtString(NULL, 0,
				"src=%d;"	/* src */
				"%s"		/* tuner */
				"%s"		/* pids */
                , pSatIp->nSrc, pszTuner, pszPids
				);

	if(NULL == pucData)
	{
		*pulDataSize = 0;
	}
	else
	{
		*pulDataSize = HxSTD_StrLen(pucData);
	}
	*ppucData = pucData;

	HLIB_STD_MemFree(pszTuner);
	HLIB_STD_MemFree(pszPids);

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}

STATIC HOM_Result_e 	_plstreaming_ModeSatIp_GetInfo_Describe(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData)
{
	PLStreaming_ModeSatIp_t		*pSatIp = NULL;
	plStreamingInst_t			*pOwnerInst= NULL;
	PLStreaming_ModeInst_t		*pstOwnerModeInst = NULL;

	ENTER_FUNCTION;

	if(NULL == pstModeInst /*  || NULL == pRequestPath */)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) pRequestPath(%s)\r\n", pstModeInst, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	pSatIp 		= (&pstModeInst->Mode.stSatIp);

    if(pulDataSize == NULL || ppucData == NULL) // 2013/12/09 mhkang: CID 151451 (#2 of 2): Dereference after null check (FORWARD_NULL)
	{
        HxLOG_Error("Invalid_Arguments pulDatasize(%p), ppucData(%p)\n", pulDataSize, ppucData);
        return eHOM_Result_Error_NullPoint;
	}

	if (pSatIp->bIsOwner == FALSE)
	{
		// find owner inst
		pOwnerInst = PL_Streaming_Context_FindStreamingInstanceByStreamId(TRUE, pSatIp->nStreamId);
		if (pOwnerInst == NULL)
		{
			HxLOG_Error("Not Found owner(streamID:%d)\n", pSatIp->nStreamId);
			return eHOM_Result_Error_NotExist;
		}

		pstOwnerModeInst = pOwnerInst->pstModeInst;
		if (pstOwnerModeInst == NULL)
		{
			HxLOG_Error("Owner's ModeInst is null(streamID:%d)\n", pSatIp->nStreamId);
			return eHOM_Result_Error_NullPoint;
		}

		if (pstOwnerModeInst->tunerInfo != NULL)
		{
			*ppucData	= HLIB_STD_StrDup(pstOwnerModeInst->tunerInfo);
			*pulDataSize = pstOwnerModeInst->tunerInfo_length;
		}
		else
		{
			*ppucData	= NULL;
			*pulDataSize = 0;
		}
	}
	else
	{
		if (pstModeInst->tunerInfo != NULL)
		{
			*ppucData	= HLIB_STD_StrDup(pstModeInst->tunerInfo);
			*pulDataSize = pstModeInst->tunerInfo_length;
		}
		else
		{
			*ppucData	= NULL;
			*pulDataSize = 0;
		}
	}



	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


STATIC HOM_Result_e 	_plstreaming_ModeSatIp_GetInfo_Options(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData)
{
	(void)pstInst;
	(void)pRequestPath;
	(void)pulDataSize;
	(void)ppucData;
	return eHOM_Result_Ok;
}


STATIC HOM_Result_e _plstreaming_ModeSatIp_UpdateSchedulePids(PLStreaming_ModeInst_t* pstInstMode)
{
    HOM_Result_e eHomResult = eHOM_Result_Ok;

	/* 2015/6/13 mhkang: http://svn.humaxdigital.com:3000/issues/104038#note-16
	   pids변경시마다  RingBuf Clear하면 안될듯함.
    Handle_t    hRingBuf = HANDLE_NULL;

    _plstreaming_ModeSatIp_DestroyRingbuf(pstInstMode);

	hRingBuf = _plstreaming_ModeSatIp_CreateRingbuf(pstInstMode, NULL );
    if(HANDLE_NULL == hRingBuf || 0 == hRingBuf)
    {
        _plstreaming_ModeSatIp_DeleteInstance(pstInstMode);
        HxLOG_Error("_plstreaming_ModeSatIp_CreateRingbuf(hRingBuf:0x%x) error\r\n", hRingBuf);
        return eHOM_Result_Error_CreateFail;
    }

    pstInstMode->hRingbuf = hRingBuf;
	*/

	eHomResult = PL_Streaming_Resource_UpdateSchedulePids(pstInstMode, pstInstMode->nPIDsCount, pstInstMode->usPIDs, pstInstMode->hRingbuf);
    if(eHOM_Result_Ok != eHomResult)
    {
        _plstreaming_ModeSatIp_DeleteInstance(pstInstMode);
        HxLOG_Error("PL_Streaming_Resource_UpdateSchedulePids error!\n");
        return eHomResult;
    }

    return eHomResult;
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
HOM_Result_e PL_Streaming_ModeSatIp_Init(void)
{
	HOM_Result_e	eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	HxSTD_memset(&s_pstSatIpMgr, 0, sizeof(plStreamingModeSatIpMgr_t));

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
void PL_Streaming_ModeSatIp_DeInit(void)
{
	ENTER_FUNCTION;

	if(NULL != s_pstSatIpMgr.pstM3U8Str)
	{
		HLIB_STR_Delete(s_pstSatIpMgr.pstM3U8Str);
		s_pstSatIpMgr.pstM3U8Str = NULL;
	}

	HxSTD_memset(&s_pstSatIpMgr, 0, sizeof(plStreamingModeSatIpMgr_t));

	LEAVE_FUNCTION;
}


/**
 *
 *
 *
 * @param
 * @return
 */
PLStreaming_ModeInst_t *PL_Streaming_ModeSatIp_SetupInstance(PLStreaming_ModeInst_t *pstModeInst)
{
	PLStreaming_ModeSatIp_t		*pSatIp = NULL;
	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;
	plStreamingInst_t			*pOwnerInst= NULL;
	int nStreamId;

	ENTER_FUNCTION;

	if(NULL == pstModeInst)
	{
		HxLOG_Error("Invalid_Arguments pstSetup(0x%x)\r\n", pstModeInst);
		return NULL;
	}
	pSatIp 		= (&pstModeInst->Mode.stSatIp);
	pSetupInfo 	= (&pstModeInst->stSetupInfo);

	pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_None;


	if (pstModeInst->stSetupInfo.pszDeviceName == NULL)
	{
		HxLOG_Error("pstInst->stSetupInfo.pszDeviceName is null");
		goto LOOP_END;
	}

	// 2014/6/12 mhkang:fp_ExStart 호출시 넘어온 DeviceName 파라미터를 SAT>IP에서 streamid로 사용한다.
	nStreamId = atoi(pSetupInfo->pszDeviceName);


	// 2014/6/24 mhkang: find owner's StreamigInst.
	pOwnerInst = PL_Streaming_Context_FindStreamingInstanceByStreamId(TRUE, nStreamId);

	// 2014/6/13 mhkang: streamID로 streaming중인 녀석이 없으면 반드시 owner이다.
	if (pOwnerInst == NULL)
	{
		pSatIp->bIsOwner = TRUE;
		pSatIp->nStreamId = nStreamId;

		HxLOG_Info("SAT>IP Owner Streaming(streamid:%d)!!(%p)\n", nStreamId, PL_Streaming_Context_FindStreamingInstanceByStreamId(FALSE, nStreamId));
	}
	else
	{
		if (pOwnerInst->pstModeInst == NULL)
		{
			HxLOG_Error("pOwnerInst->pstModeInst is null\n");
			goto LOOP_END;
		}

		if (pOwnerInst->pstModeInst->Mode.stSatIp.pstTS == NULL)
		{
			HxLOG_Error("pOwnerInst->pstModeInst->Mode.stSatIp.pstTS(streamID:%d)\n", nStreamId);
			goto LOOP_END;
		}

		// Owner's 정보를 대부분 Copy해 온다.
		pSatIp->bIsOwner				= FALSE;
		pSatIp->nStreamId				= pOwnerInst->pstModeInst->Mode.stSatIp.nStreamId;
		pSatIp->pstTS					= HLIB_STD_MemDup(pOwnerInst->pstModeInst->Mode.stSatIp.pstTS, sizeof(DxTransponder_t));

		pstModeInst->hResHandle			= HANDLE_NULL; //2014/6/13 mhkang: SAT>IP No-owner는 SAMA Resource를 사용하지 않는다.
		pstModeInst->nBitrate			= pOwnerInst->pstModeInst->nBitrate;
		pstModeInst->pstMediaProfileItem= NULL;

		pstModeInst->pstStreamingKey;
		pstModeInst->stSetupInfo;
		pstModeInst->usPIDs;
		pstModeInst->hStreamingId;
		pstModeInst->isChangedTP;
		pstModeInst->Mode;

		_plstreaming_ModeSatIp_ReplacePids(pstModeInst, pOwnerInst->pstModeInst->nPIDsCount,pOwnerInst->pstModeInst->usPIDs);

		HxLOG_Info("SAT>IP No-Onwer Streaming!!\n");
	}


LOOP_END:

	LEAVE_FUNCTION;

	return pstModeInst;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_ModeSatIp_DeleteInstance(PLStreaming_ModeInst_t *pstModeInst)
{
	ENTER_FUNCTION;

    if(NULL == pstModeInst)
	{
        HxLOG_Error("Invalid_Arguments pstInst(0x%x) \r\n", pstModeInst);
		return ;
	}

    if (pstModeInst->Mode.stSatIp.bIsOwner == TRUE) // Owner
    {
        if (UDS_DestroyManager(pstModeInst->Mode.stSatIp.pUdsObj) == false)
        {
            HxLOG_Error("UDS_DestroyManager(pUdsObj:%p) failed\n", pstModeInst->Mode.stSatIp.pUdsObj);
        }

        // SAT>IP Streaming이 끝날 때 socketpair로 생성한 socket handles를 모두 closes한다.
        if (pstModeInst->stSetupInfo.serverfd != -1)
        {
            close(pstModeInst->stSetupInfo.serverfd);
            pstModeInst->stSetupInfo.serverfd = -1;
        }

        if (pstModeInst->stSetupInfo.clientfd != -1)
        {
            close(pstModeInst->stSetupInfo.clientfd);
            pstModeInst->stSetupInfo.clientfd = -1;
        }
    }
    else    // Non-owner
    {
        if (UDS_RemoveSocket(pstModeInst->Mode.stSatIp.pUdsObj, pstModeInst->stSetupInfo.serverfd) == false)
        {
            HxLOG_Error("UDS_RemoveSocket(pUdsObj:%p, socket:%d) failed\n", pstModeInst->Mode.stSatIp.pUdsObj, pstModeInst->stSetupInfo.serverfd);
        }
        // SAT>IP Streaming이 끝날 때 socketpair로 생성한 socket handles를 모두 closes한다.
        if (pstModeInst->stSetupInfo.serverfd != -1)
        {
            close(pstModeInst->stSetupInfo.serverfd);
            pstModeInst->stSetupInfo.serverfd = -1;
        }

        if (pstModeInst->stSetupInfo.clientfd != -1)
        {
            close(pstModeInst->stSetupInfo.clientfd);
            pstModeInst->stSetupInfo.clientfd = -1;
        }
    }

    pstModeInst->Mode.stSatIp.pUdsObj = NULL;

    _plstreaming_ModeSatIp_DeleteInstance(pstModeInst);

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
HOM_Result_e PL_Streaming_ModeSatIp_ResConnect (PLStreaming_ModeInst_t *pstModeInst)
{
	HOM_Result_e  	eHomResult = eHOM_Result_Ok;

	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;

	ENTER_FUNCTION;

    if(NULL == pstModeInst)
	{
        HxLOG_Error("Invalid_Arguments pstSetup(0x%x)\r\n", pstModeInst);
		return eHOM_Result_Error;
	}

    pSetupInfo 	= (&pstModeInst->stSetupInfo);

	if(TRUE == pSetupInfo->bNotUseResource)
	{
		LEAVE_FUNCTION;
		return eHOM_Result_Ok;
	}
	(void)eHomResult;

    eHomResult = _plstreaming_ModeSatIp_Str2Info(pstModeInst, pSetupInfo->pszStreamingPath);
	if(eHOM_Result_Ok != eHomResult)
    {
        HxLOG_Error("_plstreaming_ModeSatIp_Str2Info(pstInst:%p, streamingPath:%s) failed (eHomResult:%d)\n", pstModeInst, pSetupInfo->pszStreamingPath, eHomResult);
        goto ErrBlock;
	}

    eHomResult = _plstreaming_ModeSatIp_Start(pstModeInst);
	if(eHOM_Result_Ok != eHomResult)
	{
        HxLOG_Error("_plstreaming_ModeSatIp_Start(pstInst:%p) failed (eHomResult:%d)\n", pstModeInst, eHomResult);
        goto ErrBlock;
	}

	LEAVE_FUNCTION;

	return eHomResult;

ErrBlock:

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
HOM_Result_e PL_Streaming_ModeSatIp_ResConflict (PLStreaming_ModeInst_t *pstModeInst, HINT32 eNotifyType, HINT32 nRequestPid, HCHAR *fromProcessName)
{
	HUINT32				ulPid = 0;
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;
	SAPI_NotifyType_e	eSapiNotifyType = (SAPI_NotifyType_e)eNotifyType;

	ENTER_FUNCTION;

	switch(eSapiNotifyType)
	{
	case eSAPI_NOTIFY_TYPE_ConflictScheduleAdded:
	case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
		ulPid = HLIB_STD_GetPID();
        HxLOG_Info("eSAPI_NOTIFY_TYPE_ScheduleRemoved(isChangedTP:%d myPid:%d ReqPid:%d Process:%s)\n", pstModeInst->isChangedTP, ulPid ,nRequestPid, fromProcessName);
#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
        if (pstModeInst->isChangedTP == FALSE && nRequestPid != eSAPI_UNIQUE_PROCESS_Indicator_Homma)
#else
        if (pstModeInst->isChangedTP == FALSE && ulPid != nRequestPid) //  RTSP PLAY streamID+TP 형태가 아니고 homma 자신이 아닐 때
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
HOM_Result_e 	PL_Streaming_ModeSatIp_GetM3U8(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	HINT32						ulM3U8BufSize = 0;
	HUINT8						*pM3U8Buf = NULL;
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

    if(NULL == pstModeInst ||  NULL == ppucData || NULL == pRequestPath)
	{
        HxLOG_Error("Invalid_Arguments pstModeInst(0x%x) ppucData(0x%x) pRequestPath(%s)\r\n", pstModeInst, ppucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Print("Request Streaming Path (%s)\r\n", pRequestPath);

    eHomResult = _plstreaming_ModeSatIp_GetM3U8(pstModeInst, pRequestPath, &ulM3U8BufSize, &pM3U8Buf);
	if(eHOM_Result_Ok != eHomResult)
	{
        HxLOG_Error("_plstreaming_ModeSatIp_GetM3U8 error!! pstModeInst(0x%x) ppucData(0x%x) pRequestPath(%s)\r\n", pstModeInst, ppucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	*ppucData		= pM3U8Buf;
	*pulDataSize	= ulM3U8BufSize;

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
HOM_Result_e 	PL_Streaming_ModeSatIp_ReleaseM3U8(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath, HUINT8 *pucData)
{
	ENTER_FUNCTION;

    if(NULL == pucData || NULL == pstModeInst || NULL == pRequestPath)
	{
        HxLOG_Error("Invalid_Arguments pstModeInst(0x%x) pucData(0x%x) pRequestPath(%s)\r\n", pstModeInst, pucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Print("Request Streaming Path (%s)\r\n", pRequestPath);

	HLIB_STD_MemFree(pucData);

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


STATIC HBOOL _plstreaming_ModeSatIp_IsPsiSiPid(HUINT16 usPid)
{
#define _SATIP_PAT_PID		0x0000	// 0
#define _SATIP_CAT_PID		0x0001	// 1
#define _SATIP_TSDT_PID		0x0002	// 2
#define _SATIP_NIT_PID		0x0010	// 16
#define _SATIP_SDTBAT_PID	0x0011	// 17
#define _SATIP_EIT_PID		0x0012	// 18
#define _SATIP_RST_PID		0x0013	// 19
#define _SATIP_TOTTDT_PID	0x0014	// 20

	switch(usPid)
	{
	case _SATIP_PAT_PID:
	case _SATIP_CAT_PID:
	case _SATIP_TSDT_PID:
	case _SATIP_NIT_PID:
	case _SATIP_SDTBAT_PID:
	case _SATIP_EIT_PID:
	case _SATIP_RST_PID:
	case _SATIP_TOTTDT_PID:
		return TRUE;
	default:
		return FALSE;
	}
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeSatIp_GetTS(PLStreaming_ModeInst_t *pstInst,  const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
#if 0
	HINT32 						nTsIndex  = 0;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
	PLStreaming_ModeSatIp_t		*pSatIp = NULL;
	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;
#endif
	ENTER_FUNCTION;

	if(NULL == ppucData || NULL == pulDataSize || NULL == pstInst)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) ppucData(0x%x) pRequestPath(%s) pulDataSize(0x%x)\r\n", pstInst, ppucData, pRequestPath, pulDataSize);
		return eHOM_Result_Error_Invalid_Arguments;
	}

    HxLOG_Error("this function is deprecated!\n");
    return eHOM_Result_Error;

    /* 2015/3/25 mhkang:
       아래 이슈를 처리하면서 TS를 기존 RingBuf 메모리포인터로 넘겨주던 것을 UDS Socket을 이용하여 live555 모듈에게 직접 전달하도록 수정되었음.
       SAT>IP Streaming할때는 더 이상 PL_Streaming_ModeSatIp_GetTS 함수가 호출되지 않음. #if 0 처리함.
       .http://svn.humaxdigital.com:3000/issues/101413
    */
#if 0

	pSatIp 		= (&pstInst->Mode.stSatIp);
	pSetupInfo 	= (&pstInst->stSetupInfo);

    if (pstInst->isLocked == FALSE)
    {
        VK_TASK_Sleep(1);
        HxLOG_Warning("Signal is not locked\n");
        return eHOM_Result_Error_NoSignalLock;
    }

    if (pstInst->PidFlag & MASK_PID_NONE)
    {
        VK_TASK_Sleep(1);
        HxLOG_Warning("pids=none\n");
        return eHOM_Result_Error_NoSignalLock;
    }

	//////////////////////////////////////////////////////////////////////////////
	// 처음 5초간 waiting
	#if 1
	{
		HINT32	i = 0;
		HBOOL	bIsSiPid = FALSE;
		HINT32	nPidsCnt = pstInst->nPIDsCount;

		if(DxRSV_PIDTABLE_COUNT < nPidsCnt)
		{
			HxLOG_Error("invalid pids count = %d\n", nPidsCnt);
			nPidsCnt = DxRSV_PIDTABLE_COUNT;
		}

		for(i = 0; i < nPidsCnt; i++)
		{
			bIsSiPid = _plstreaming_ModeSatIp_IsPsiSiPid(pstInst->usPIDs[i]);
			if(FALSE == bIsSiPid)
				break;
		}

		if((TRUE == bIsSiPid) || (1 == nPidsCnt))
		{
			// 모두 PSI/SI PID이면, 5초 waiting 할 필요 없음
			pSatIp->bFirstGet = TRUE;

			// SI Pid 모드니까, 이 때는 바로 Read 하면 됨
		}
		else
		{
			HUINT32		ulCurTick = 0;

			HxLOG_Print(HxANSI_COLOR_PURPLE("bIsSiPid = %d, nPidsCnt = %d, pSatIp->bFirstGet = %d\n"), bIsSiPid, nPidsCnt, pSatIp->bFirstGet);

			// 맨 처음 Get TS는 Skip할 수 밖에 없음... bFirstGet Flag 세팅만
			if(FALSE == pSatIp->bFirstGet)
			{
				pSatIp->ulFirstGetTick = HLIB_STD_GetSystemTick();
				pSatIp->bFirstGet = TRUE;
				HxLOG_Message("[SATIP_TIME] get first ts\n");
			}

			ulCurTick = HLIB_STD_GetSystemTick();
            if(SATIP_BUFFERING_TIME > (ulCurTick - pSatIp->ulFirstGetTick))
			{
				HxLOG_Print("skip first 5 seconds (%d ms)\n", (ulCurTick - pSatIp->ulFirstGetTick));
				LEAVE_FUNCTION;
				return eHOM_Result_Error;
			}
		}
	}
	#endif
	//////////////////////////////////////////////////////////////////////////////

	eHomResult = _plstreaming_ModeSatIp_ReadRingbuf(pstInst, nTsIndex, pulDataSize, ppucData);
	if(eHOM_Result_Ok != eHomResult)
	{
        HxLOG_Error("_plstreaming_ModeSatIp_ReadRingbuf read fail pRequestPath(%s)\r\n", pRequestPath);
		return eHOM_Result_Error;
	}

	LEAVE_FUNCTION;

	return eHomResult;
#endif
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeSatIp_ReleaseTS(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData)
{
	ENTER_FUNCTION;

	if(NULL == pucData || NULL == pstInst)
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
 * @brief  RTSP PLAY rtrp://ip:port/Stream=#?src=1&freq=10833... 형태로 받았을 때, 호출된다.
 * 첫 RTSP SETUP시 sama에게 Schedule이 Add된 상태이다. RTSP PLAY with TP 처리시 conflict 발생하여 Schedule이 Remove되더라도 SAT>IP Streaming 종료처리하면 안된다.
 * 아래 시나리오시 중간에 있는 P가 실패하더라도 계속 진행할 수 있게 한다.(DVBViewer채널서치 문제 처리한 것임)
 * => S(TP1):streamID > P(streamID, add PID ) > P(streamID, TP2) > P(streamID, add PID) > P(streamId, TP2) .... > T(streamID)
 * @param pstInst Streaming Instance
 * @param pRequestPath 요청한 URL
 * @retval eHOM_Result_Ok 성공(정상적으로 Schedule 갱신됨)
 * @retval eHOM_Result_Error_Conflict Conflict에러가 발생하였다(Schedule Add/Update 하지 못함)
 * @retval Others  에러
 *
 */
HOM_Result_e 	PL_Streaming_ModeSatIp_ExecCmd(PLStreaming_ModeInst_t *pstInstMode,  const HCHAR *pRequestPath)
{
	HBOOL						bIsChangePids = FALSE;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
	PLStreaming_ModeSatIp_t		*pSatIp = NULL;
	HINT32						nStreamId;
    HUINT32                     SamaSlot;
    void* pUdsObj;

	ENTER_FUNCTION;

    if(NULL == pstInstMode   || NULL == pRequestPath)
	{
        HxLOG_Error("Invalid_Arguments pstInst(0x%x) pRequestPath(%s)\r\n", pstInstMode, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

    pSatIp 		= (&pstInstMode->Mode.stSatIp);

    // 2014/12/14 mhkang: only owner can modify sat>ip stream.
    if (pSatIp->bIsOwner == TRUE)
    {
        pstInstMode->isChangedTP = _plstreaming_ModeSatIp_CheckTPorAntInfoChange(pstInstMode, pRequestPath);
        if(TRUE == pstInstMode->isChangedTP)
        {
            HxLOG_Debug("TP Changed! \n");

            UDS_SetSignalLock(pSatIp->pUdsObj, FALSE);  // make unlock
            UDS_SetRingBuf( pSatIp->pUdsObj, HANDLE_NULL);

			SamaSlot = _plstreaming_ModeSatIp_Stop(pstInstMode, FALSE); // 2015/2/15 mhkang: bWaiting = FALSE 으로 사용하여 즉시 빠져나오도록 한다.
            if (SamaSlot == SAMASLOT_INITVAL)
            {
                HxLOG_Error(HxANSI_COLOR_RED("_plstreaming_ModeSatIp_Stop failed\n"));
            }

            nStreamId = pSatIp->nStreamId;
            pUdsObj = pSatIp->pUdsObj;

            _plstreaming_ModeSatIp_DeleteInstance(pstInstMode);

			pSatIp->bIsOwner  = TRUE;
            pSatIp->nStreamId = nStreamId;
			pSatIp->pUdsObj   = pUdsObj;

            eHomResult = _plstreaming_ModeSatIp_Str2Info(pstInstMode, pRequestPath);
            if(eHOM_Result_Ok != eHomResult)
            {
                _plstreaming_ModeSatIp_DeleteInstance(pstInstMode);
                HxLOG_Error("_plstreaming_ModeSatIp_Str2Info error!\r\n");
                return eHOM_Result_Error_CreateFail;
            }

            if (SamaSlot != SAMASLOT_INITVAL)
            {
                /* 2015/2/15 mhkang:
                _plstreaming_ModeSatIp_Stop(pstIst, TRUE/FALSE) TRUE를 사용하는 경우 SamaSchedule 이 삭제되는데 기다리는 시간이 0 ~ 100 msec 소요된다.
                따라서 이 함수에서 Waiting 하지 않고 바로 빠져나오도록 한다(bWaiting = FALSE).
                _plstreaming_ModeSatIp_Str2Info 함수 안에서 APK API 호출하는 부분에서 STB 동작상황에 따라서 100 ~ 200 msec 소요되기 때문에
                _plstreaming_ModeSatIp_Str2Info 함수 호출되는 동안 Sama Schedule이 삭제될 때까지 충분한 시간이 되어 RTSP 응답 시간을 줄일 수 있다.
                */
                PL_Streaming_Resource_WaitForCancelSchedule(SamaSlot);
            }

            UDS_ClearSocketBuffer(pstInstMode->stSetupInfo.clientfd); // 2015/3/31 mhkang: clear udp socket buffer of receiver(live555). refer to http://svn:3000/issues/98338
            UDS_SetRingBuf( pSatIp->pUdsObj, pstInstMode->hRingbuf);
            UDS_SetSendingFlag( pSatIp->pUdsObj, pstInstMode->stSetupInfo.serverfd, (pstInstMode->PidFlag & MASK_PID_NONE) == 0);

            eHomResult = _plstreaming_ModeSatIp_Start(pstInstMode);
            if(eHOM_Result_Ok != eHomResult)
            {
                // 2014/6/17 mhknag: 실패하더라도 empty RTP 보내기 때문에  ModeSatIp_Inst를 삭제할 필요도 없고, 404 Not Found 에러처리 할 필요도 없다.
                // RTCP APP packet 또는 DESCRIBE 응답 SDP 안에 TP 정보에 따라 엘가토에서 채널서치 개수에 영향을 끼치는 부분임.
                //_plstreaming_ModeSatIp_DeleteInstance(pstInst);
                HxLOG_Warning("_plstreaming_ModeSatIp_Start error(eHomResult:%d)\n", eHomResult);
            }

            LEAVE_FUNCTION;

            return eHOM_Result_Ok;
        }

        bIsChangePids = _plstreaming_ModeSatIp_CheckPidChange(pstInstMode, pRequestPath);
        if(TRUE == bIsChangePids)
        {
            if (pstInstMode->isLocked == TRUE) // 2014/12/22 mhkang: locking된 상태에서는 pid 변경 요청이 오면 ringBuf를 재생성한 후에 Update PID 한다.
            {
	            HxLOG_Print(HxANSI_COLOR_PURPLE("PID Changed! pstInstMode->isLocked == TRUE \n"));

				/* 2015/6/13 mhkang: http://svn.humaxdigital.com:3000/issues/104038#note-16
				   pids변경시마다  RingBuf Clear하면 안될듯함.
				 UDS_SetRingBuf( pSatIp->pUdsObj, HANDLE_NULL);
				*/
                 eHomResult = _plstreaming_ModeSatIp_UpdateSchedulePids(pstInstMode);
                 if(eHOM_Result_Ok != eHomResult)
                 {
                     HxLOG_Error("_plstreaming_ModeSatIp_UpdateSchedulePids error!(eHomResult:%d)\n", eHomResult);
                     return eHomResult;
                 }
                 else
                 {
					 /* 2015/6/13 mhkang: http://svn.humaxdigital.com:3000/issues/104038#note-16
						pids변경시마다  RingBuf Clear하면 안될듯함.
					 UDS_ClearSocketBuffer(pstInstMode->stSetupInfo.clientfd); // 2015/3/31 mhkang: clear udp socket buffer of receiver(live555). refer to http://svn:3000/issues/98338
					 */
                     UDS_SetRingBuf(pSatIp->pUdsObj, pstInstMode->hRingbuf);
                 }
            }
            else
            {
	            HxLOG_Print(HxANSI_COLOR_PURPLE("PID Changed! pstInstMode->isLocked == FALSE (PLStreaming_DelayedCmd_Play)\n"));
                /* 2014/12/22 mhknag: locking 되지 않았으면 PL_Streaming_ModeSatIp_Task thread에서 locking 확인된 후에서 Update Pid 한다.
                */
                pstInstMode->eDelayedCmd = PLStreaming_DelayedCmd_Play;
            }
			UDS_SetSendingFlag( pSatIp->pUdsObj, pstInstMode->stSetupInfo.serverfd, (pstInstMode->PidFlag & MASK_PID_NONE) == 0);

            // 2014/6/27 mhkang: 변경된 PID에 맞게 TP정보를 즉시 업데이트함.
           	HxLOG_Print(HxANSI_COLOR_PURPLE("call _plstreaming_ModeSatIp_Update_TunerInfo\n"));

            eHomResult = _plstreaming_ModeSatIp_Update_TunerInfo(pstInstMode);
            if(eHOM_Result_Ok != eHomResult)
            {
                HxLOG_Warning("_plstreaming_ModeSatIp_Update_TunerInfo failed(eHomResult:%d)\n", eHomResult);
            }

            LEAVE_FUNCTION;

            return eHOM_Result_Ok;
        }

        /* 2015/3/25 mhkang:
            client가 그냥 주기적으로 RTSP PLAY+SID+no param 형태로 보내는 client가 존재한다면 그때마다 buffering하게되면 화면이 끊기게 될 것이다.
            따라서 streaming 중인지 아닌지 판단하여 streaming중이 아닐때만 buffering할 수 있도록 해야 할 것 같다.
        */
        UDS_SetRingBuf(pSatIp->pUdsObj, pstInstMode->hRingbuf); // for buffering, non-owner는 ringbuf 바뀌지 않는다.


    }
    else    // non-owner
    {
        // do nothing!
    }

    // 2015/3/27 none owner는 PIDS ALL인지 NONE인지 알수 없다. 어떻게 하지?
    UDS_SetSendingFlag( pSatIp->pUdsObj, pstInstMode->stSetupInfo.serverfd, (pstInstMode->PidFlag & MASK_PID_NONE) == 0);

    (void)pstInstMode;
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
HOM_Result_e 	PL_Streaming_ModeSatIp_GetInfo(PLStreaming_ModeInst_t *pstModeInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData)
{

	switch(eInfoType)
	{
	case PLStreaming_InfoType_Describe:
        return _plstreaming_ModeSatIp_GetInfo_Describe(pstModeInst, pRequestPath, pulDataSize, ppucData);

	case PLStreaming_InfoType_Options:
        return _plstreaming_ModeSatIp_GetInfo_Options(pstModeInst, pRequestPath,pulDataSize, ppucData);

	default:
		break;
	}

	HxLOG_Error("eInfoType(%d) error!\r\n", eInfoType);

	return eHOM_Result_Error;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeSatIp_ReleaseInfo(PLStreaming_ModeInst_t *pstModeInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HCHAR *pucData)
{
	ENTER_FUNCTION;

    if(NULL == pstModeInst || NULL == pucData /* || NULL == pRequestPath */)
	{
        HxLOG_Error("Invalid_Arguments pstModeInst(0x%x) pucData(0x%x) eInfoType(%d) pRequestPath(%s)\r\n", pstModeInst, pucData, eInfoType, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Debug("Request pCommand(%d) pUrl(%s)\r\n", eInfoType, pRequestPath);


	switch(eInfoType)
	{
	case PLStreaming_InfoType_Describe:
		HLIB_STD_FreeFmtString(pucData);
		break;

	// TODO: "OPTIONS" 처리
	case PLStreaming_InfoType_Options:
	default:
		break;
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
HOM_Result_e 	PL_Streaming_ModeSatIp_StartInfoToSetupInfo(const HCHAR *pszPrefix, const PLStreaming_StartInfo_t *pstStartInfo, PLStreaming_SetupInfo_t *ret_pstSetupInfo)
{
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;
	PLStreaming_SetupInfo_t		*pstSetupInfo = ret_pstSetupInfo;

	ENTER_FUNCTION;

	if(NULL == pstStartInfo || NULL == pstSetupInfo)
	{
		HxLOG_Error("Invalid_Arguments pstStartInfo(0x%x) pstSetupInfo(0x%x) \r\n", pstStartInfo, pstSetupInfo);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	if(NULL == pstStartInfo->pszStreamingPath)
	{
		HxLOG_Print("get m3u8?\r\n");
		return eHOM_Result_Ok;
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
HOM_Result_e 	PL_Streaming_ModeSatIp_GetProfileItemHandle(PLStreaming_ModeInst_t *pstModeInst, Handle_t *ret_phProfileItem)
{
//	Handle_t					hProfileHandle = HANDLE_NULL;
//	HOM_Result_e  				eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

    if(NULL == pstModeInst || NULL == ret_phProfileItem)
	{
        HxLOG_Error("Invalid_Arguments pstInst(0x%x) ret_phProfileItem(%s)\r\n", pstModeInst, ret_phProfileItem);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	*ret_phProfileItem = HANDLE_NULL;


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
HOM_Result_e PL_Streaming_ModeSatIp_GetPIDs(PLStreaming_ModeInst_t *pstModeInst, HINT32 *ret_pnPIDsCount, HUINT16 *ret_arrusPIDs)
{
	ENTER_FUNCTION;

    if(NULL == pstModeInst || NULL == ret_pnPIDsCount || NULL == ret_arrusPIDs)
	{
        HxLOG_Error("Invalid_Arguments pstModeInst(0x%x) ret_pnPIDsCount(0x%x) ret_arrusPIDs(%s)\r\n", pstModeInst, ret_pnPIDsCount, ret_arrusPIDs);
		return eHOM_Result_Error_Invalid_Arguments;
	}

    *ret_pnPIDsCount = pstModeInst->nPIDsCount;
    HxSTD_memcpy(ret_arrusPIDs, pstModeInst->usPIDs, sizeof(HUINT16) * DxRSV_PIDTABLE_COUNT);

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


STATIC HOM_Result_e _plstreaming_ModeSatIp_Update_TunerInfo(PLStreaming_ModeInst_t *pstModeInst)
{
	HCHAR *tunerInfo = NULL;
	HUINT32 tunerInfo_length = 0;
	HOM_Result_e eHomResult = eHOM_Result_Ok;

	if (pstModeInst->Mode.stSatIp.bIsOwner == TRUE)
	{
		eHomResult =_plstreaming_ModeSatIp_Get_TunerInfo(pstModeInst, NULL, &tunerInfo_length, &tunerInfo);
		if (eHomResult != eHOM_Result_Ok)
		{
			HxLOG_Error("_plstreaming_ModeSatIp_GetInfo_Describe failed(eHomResult:%d)\n", eHomResult);
			goto ErrBlock;
		}

		if (tunerInfo != NULL)
		{
			HxLOG_Print("Tuner:%s\n", tunerInfo);

			if (pstModeInst->tunerInfo != NULL)
			{
				HLIB_STD_MemFree(pstModeInst->tunerInfo);
			}
			pstModeInst->tunerInfo		  = tunerInfo;
			pstModeInst->tunerInfo_length = tunerInfo_length;

		}
		else
		{
			HxLOG_Print("tuner info is null\n");

			if (pstModeInst->tunerInfo != NULL)
			{
				HLIB_STD_MemFree(pstModeInst->tunerInfo);
			}

			pstModeInst->tunerInfo		  = NULL;
			pstModeInst->tunerInfo_length = 0;

		}
	}
	else
	{
		// 2014/6/26 mhakang:
		// Non-owner는 Owner에서 가져가도록 한다.
	}

	return eHOM_Result_Ok;

ErrBlock:
	return eHomResult;
}

HBOOL _plstreaming_ModeSatIp_CheckTime_UpdateTunerInfo(PLStreaming_ModeInst_t *pstModeInst)
{
    UNIXTIME			utCur = 0;

    if (pstModeInst == NULL)
    {
        HxLOG_Error("Invalid args(pstInst is null)\n");
        return FALSE;
    }


    utCur =  HLIB_STD_GetSystemTime();  // second

    if (utCur - pstModeInst->utUpdateTunerInfo >= 5) // 1 초마다 TP 정보 갱신
    {
        pstModeInst->utUpdateTunerInfo = utCur;
        return TRUE;
    }

    return FALSE;
}

/**
* @date 2014/6/25
* @author mhkang
* @brief 이 함수는 Resource Task Thread에서 10 msec 주기로 호출되는 함수이다.
* Background로 해야할 작업이 있을때 이 함수에서 추가한다.
* 오랫동안 sem lock하는 코드는 작성하지 않아야 한다.
*
* @param
* @return
*/
HOM_Result_e PL_Streaming_ModeSatIp_Task(plStreamingInst_t *pstInst)
{
	HOM_Result_e eHomResult = eHOM_Result_Ok;

	/* 2014/6/26 mhkang: Update SAMA schedule
	수정하기 전에는 Get TS콜백이 호출될 때마다 SAMA 스케줄을 업데이트하였었는데,
	RTSP SETUP 성공후 Client가 RTSP OPTIONS만 주기적으로 보내는 경우 SAMA 스케줄을 업데이트하지 않아 RTSP Session이 종료되는 경우가 발생한다.
	=> Background로 SAMA 스케줄을 업데이트하도록 수정함.
	*/

	if (pstInst == NULL)
	{
		HxLOG_Error("Invalid args(pstInst is null)\n");
		eHomResult = eHOM_Result_Error_NullPoint;
		goto ErrBlock;
	}

	if (pstInst->pstModeInst == NULL)
	{
		HxLOG_Error("Invalid args((pstInst->pstModeInst is null)\n");
		eHomResult = eHOM_Result_Error_NullPoint;
		goto ErrBlock;
	}

	if (pstInst->pstModeInst->hResHandle != HANDLE_NULL)
	{
        PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle, PL_STREAMING_SCHEDULE_SATIP_DURATION, 40);
	}
	else
	{
		// No-owner has hResHandle(HANDLE_NULL).
		// do nothing
	}

    // owner에만 적용해야 하나?
    if (pstInst->pstModeInst->Mode.stSatIp.bIsOwner == TRUE)
	{
        if (pstInst->pstModeInst->isLocked == FALSE)
        {
			if (pstInst->pstModeInst->tunerid >= 0)
			{
				HxLOG_Print(HxANSI_COLOR_PURPLE("call PL_COMMON_IsTunerLocked\n"));
				PL_COMMON_IsTunerLocked(pstInst->pstModeInst->tunerid, &pstInst->pstModeInst->isLocked);
			}

			if (pstInst->pstModeInst->isLocked == TRUE)
			{
				HxLOG_Print("##############################################\n");
				HxLOG_Print("###           (%d)Signal Locked!!!         ###\n", pstInst->pstModeInst->tunerid);
				HxLOG_Print("##############################################\n");

				UDS_SetSignalLock(pstInst->pstModeInst->Mode.stSatIp.pUdsObj, TRUE);

				// 2014/12/22 mhkang: Locking 되고나서 TP정보를 즉시 갱신해야 한다(채널서치 개수에 영향을 줌)
				HxLOG_Print(HxANSI_COLOR_PURPLE("call _plstreaming_ModeSatIp_Update_TunerInfo\n"));
				eHomResult = _plstreaming_ModeSatIp_Update_TunerInfo(pstInst->pstModeInst);
				if(eHOM_Result_Ok != eHomResult)
				{
					HxLOG_Error("_plstreaming_ModeSatIp_Update_TunerInfo error! (eHomResult:%d)\n", eHomResult);
					goto ErrBlock;
				}
			}
        }

        if (pstInst->pstModeInst->isLocked == TRUE && pstInst->pstModeInst->eDelayedCmd == PLStreaming_DelayedCmd_Play)
        {
            eHomResult = _plstreaming_ModeSatIp_UpdateSchedulePids(pstInst->pstModeInst);
            if(eHOM_Result_Ok != eHomResult)
            {
                HxLOG_Error("_plstreaming_ModeSatIp_UpdateSchedulePids error! (eHomResult:%d)\n", eHomResult);
                goto ErrBlock;
            }

			HxLOG_Print(HxANSI_COLOR_PURPLE("call _plstreaming_ModeSatIp_Update_TunerInfo\n"));
            eHomResult = _plstreaming_ModeSatIp_Update_TunerInfo(pstInst->pstModeInst);
            if(eHOM_Result_Ok != eHomResult)
            {
                HxLOG_Error("_plstreaming_ModeSatIp_Update_TunerInfo error! (eHomResult:%d)\n", eHomResult);
                goto ErrBlock;
            }

            pstInst->pstModeInst->eDelayedCmd = PLStreaming_DelayedCmd_None;
        }

        if (_plstreaming_ModeSatIp_CheckTime_UpdateTunerInfo(pstInst->pstModeInst) == TRUE)
        {
            /* 2014/6/26 mhkang:
            DESCRIBE의 SDP 또는 RTCP APP 패킷을 보낼때 STB의 TP 정보를 보내는데,
            SAT>IP Streaming중에 직접 TP 정보를 갖고 오는 함수를 호출하면 delay가 생기는데 이것때문에 RTP Packet전송에 delay가 생겨서 재생화면이 깨질 수 있다.
            Background로 미리 TP정보를 갖고 오도록 수정한다.
            */
			HxLOG_Print(HxANSI_COLOR_PURPLE("call _plstreaming_ModeSatIp_Update_TunerInfo\n"));
            eHomResult = _plstreaming_ModeSatIp_Update_TunerInfo(pstInst->pstModeInst);
            if (eHomResult != eHOM_Result_Ok)
            {
                HxLOG_Error("_plstreaming_ModeSatIp_Update_TunerInfo failed(eHomResult:%d)\n", eHomResult);
                goto ErrBlock;
            }
        }
    }
    else // non-owner
    {
        // do nothing.
    }

	return eHomResult;

ErrBlock:
	return eHomResult;
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
void 	PL_Streaming_ModeSatIp_Dump(PLStreaming_ModeInst_t *pstModeInst)
{
    if(NULL == pstModeInst)
	{
        HxLOG_Error("Invalid_Arguments pstModeInst(0x%x) \r\n", pstModeInst);
		return ;
	}
	HxLOG_Print("pstInst->pstModeInst->(&pstInst->Mode.stSatIp)\n");
}


