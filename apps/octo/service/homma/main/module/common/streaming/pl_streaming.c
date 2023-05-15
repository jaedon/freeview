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

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
#define DEFAULT_USER_AGENT "unknown"

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC PLStreaming_InfoType_e _plstreaming_Info_GetType( const HCHAR *pCommand )
{
	PLStreaming_InfoType_e	eInfoType = PLStreaming_InfoType_Unknown;

	if(NULL != pCommand)
	{
		if(0 == HLIB_STD_StrCaseCmp(pCommand, "DESCRIBE"))
		{
			eInfoType = PLStreaming_InfoType_Describe;
		}
		else if(0 == HLIB_STD_StrCaseCmp(pCommand, "OPTIONS"))
		{
			eInfoType = PLStreaming_InfoType_Options;
		}
	}

	return eInfoType;
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
HOM_Result_e PL_Streaming_Start(const Handle_t hSetupInfo, Handle_t *ret_phStreamingId)
{
	Handle_t				hStreamingId = HANDLE_NULL;
	PLStreaming_SetupInfo_t *pstSetupInfo = (PLStreaming_SetupInfo_t *)hSetupInfo;
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	PL_Streaming_Context_SetupDump(pstSetupInfo);

	hStreamingId = PL_Streaming_Context_New(pstSetupInfo);
	if(HANDLE_NULL == hStreamingId || 0 == hStreamingId)
	{
		HxLOG_Error("Resource_Notfound!\r\n");
		return eHOM_Result_Error_Resource_Notfound;
	}

	eHomResult = PL_Streaming_Context_ResConnect(hStreamingId);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_Streaming_Context_ResConnect error! hStreamingId(0x%x)\r\n",hStreamingId);
		PL_Streaming_Context_Delete(hStreamingId);
		return eHOM_Result_Error_Conflict;
	}

	*ret_phStreamingId = hStreamingId;

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
void 	PL_Streaming_End(const Handle_t hStreamingId)
{
	ENTER_FUNCTION;

	PL_Streaming_Context_Delete(hStreamingId);

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
HOM_Result_e	 PL_Streaming_CreateStartInfo(		const DxRsvStreamType_e	eStreamType,
													const PLStreaming_OutType_e	eOutType,
													const HCHAR *pszDeviceName,
													const HCHAR *pszUserAgent,
													const HCHAR *pszDeviceID,
													const HCHAR *pszStreamingPath,
													const HCHAR *pszIPAddress,
													int socketfd[2],
													PLStreaming_StartInfo_t *ret_pstStartInfo)
{
	PLStreaming_StartInfo_t *pstStartInfo = ret_pstStartInfo;

	ENTER_FUNCTION;

	pstStartInfo->eStreamType		= eStreamType;
	pstStartInfo->eOutType			= (ePLStreaming_OutType_Ringbuf | eOutType);
	pstStartInfo->pszDeviceName		= (NULL == pszDeviceName	) ? NULL : HLIB_STD_StrDup(pszDeviceName);
	pstStartInfo->pszUserAgent		= (NULL == pszUserAgent		) ? HLIB_STD_StrDup(DEFAULT_USER_AGENT) : HLIB_STD_StrDup(pszUserAgent); // 2016/5/4 mhkang: if user agent is not used, homma must use default user agent("unknown"). related issue: http://alm.humaxdigital.com/cb/issue/278898#comment-350849
	pstStartInfo->pszDeviceID		= (NULL == pszDeviceID		) ? NULL : HLIB_STD_StrDup(pszDeviceID);
	pstStartInfo->pszStreamingPath	= (NULL == pszStreamingPath	) ? NULL : HLIB_STD_StrDup(pszStreamingPath);
	pstStartInfo->pszIPAddress		= (NULL == pszIPAddress		) ? NULL : HLIB_STD_StrDup(pszIPAddress);
	if (socketfd != NULL)
	{ // for sat>ip
		pstStartInfo->serverfd          = socketfd[0];
		pstStartInfo->clientfd          = socketfd[1];
	}
	else
	{
		pstStartInfo->serverfd          = -1;
		pstStartInfo->clientfd          = -1;
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
void	 PL_Streaming_DestroyStartInfo(	PLStreaming_StartInfo_t *pstStartInfo )
{
	ENTER_FUNCTION;

	if(pstStartInfo->pszDeviceName)		HLIB_STD_MemFree(pstStartInfo->pszDeviceName);
	if(pstStartInfo->pszUserAgent)		HLIB_STD_MemFree(pstStartInfo->pszUserAgent);
	if(pstStartInfo->pszDeviceID)		HLIB_STD_MemFree(pstStartInfo->pszDeviceID);
	if(pstStartInfo->pszStreamingPath)	HLIB_STD_MemFree(pstStartInfo->pszStreamingPath);
	if(pstStartInfo->pszIPAddress)		HLIB_STD_MemFree(pstStartInfo->pszIPAddress);

	HxSTD_memset(pstStartInfo, 0, sizeof(PLStreaming_StartInfo_t));

	LEAVE_FUNCTION;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_CreateSetupHandle(const PLStreaming_StartInfo_t *pstStartInfo, Handle_t *ret_phSetupInfo)
{
	HINT32					nRemainingRes = 0;
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;
	PLStreaming_SetupInfo_t stSetupInfo;

	ENTER_FUNCTION;

	/* resouce check */
	nRemainingRes = PL_Streaming_Resource_GetRemaining();
	if(0 >= nRemainingRes)
	{
		HxLOG_Error("nRemainingRes(%d) Resource full!!\r\n", nRemainingRes);
		return eHOM_Result_Error_Resource_Full;
	}

	/* setup info setting */
	eHomResult = PL_Streaming_Context_CreateSetupInfo(pstStartInfo->eStreamType, pstStartInfo, &stSetupInfo);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("eHomResult(%x), Setup fail!\r\n", eHomResult);
		return eHomResult;
	}

	*ret_phSetupInfo = (Handle_t)HLIB_STD_MemDup(&stSetupInfo, sizeof(PLStreaming_SetupInfo_t));

	LEAVE_FUNCTION;

	return eHomResult;
}

void PL_Streaming_DestroySetupHandle(Handle_t hSetupInfo)
{
	PLStreaming_SetupInfo_t *pstSetupInfo = NULL;

	ENTER_FUNCTION;

	if(HANDLE_NULL != hSetupInfo && 0 != hSetupInfo)
	{
		pstSetupInfo = (PLStreaming_SetupInfo_t *)hSetupInfo;

		PL_Streaming_Context_DeleteSetupInfo(pstSetupInfo);
		HLIB_STD_MemFree(pstSetupInfo);
	}

	LEAVE_FUNCTION;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_M3U8_Get ( const Handle_t hStreamingId, const HCHAR *pRequestPath, HINT32 *ret_pM3U8BufSize, HUINT8 **ret_ppM3U8Buf)
{
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	eHomResult =  PL_Streaming_Context_GetM3U8(hStreamingId, pRequestPath, ret_pM3U8BufSize, ret_ppM3U8Buf);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("eHomResult(%d, %s)!\r\n", eHomResult, PL_Streaming_Debug_GetHomResultStr(eHomResult));
		return eHomResult;
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
HOM_Result_e PL_Streaming_M3U8_Release (const Handle_t hStreamingId, const HCHAR *pRequestPath, HCHAR *pM3U8Buf)
{
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	eHomResult =  PL_Streaming_Context_ReleaseM3U8(hStreamingId, pRequestPath, pM3U8Buf);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("eHomResult(%d, %s)!\r\n", eHomResult, PL_Streaming_Debug_GetHomResultStr(eHomResult));
		return eHomResult;
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
HOM_Result_e PL_Streaming_TS_Get ( const Handle_t hStreamingId, const HCHAR *pRequestPath, HINT32 *ret_pTSBufSize, HUINT8 **ret_ppTSBuf)
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	HxLOG_Debug("hStreamingId = %d, ret_pTSBufSize = 0x%x, ret_ppTSBuf = 0x%x\n", hStreamingId, ret_pTSBufSize, ret_ppTSBuf);

	if(NULL == ret_pTSBufSize && NULL == ret_ppTSBuf)
	{
		eHomResult =  PL_Streaming_Context_ExecCmd(hStreamingId, pRequestPath);
	}
	else
	{
		eHomResult =  PL_Streaming_Context_GetTS(hStreamingId, pRequestPath, ret_pTSBufSize, ret_ppTSBuf);
	}
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("eHomResult(%d, %s)!\r\n", eHomResult, PL_Streaming_Debug_GetHomResultStr(eHomResult));
		return eHomResult;
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
HOM_Result_e PL_Streaming_TS_Release (const Handle_t hStreamingId, const HCHAR *pRequestPath, HCHAR *pTSBuf )
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	if(NULL == pTSBuf)
	{
		LEAVE_FUNCTION;
		return eHomResult;
	}

	eHomResult =  PL_Streaming_Context_ReleaseTS(hStreamingId, pRequestPath, pTSBuf);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("eHomResult(%d, %s)!\r\n", eHomResult, PL_Streaming_Debug_GetHomResultStr(eHomResult));
		return eHomResult;
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
HOM_Result_e PL_Streaming_Info_Get ( const Handle_t hStreamingId, const HCHAR *pCommand, const HCHAR *pRequestPath, HINT32 *ret_pInfoBufSize, HCHAR **ret_ppInfoBuf )
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;
	PLStreaming_InfoType_e	eInfoType = PLStreaming_InfoType_Unknown;

	ENTER_FUNCTION;

	HxLOG_Info(" Tick(%d)\r\n", HLIB_STD_GetSystemTick());

	// TODO: "OPTIONS" 처리

	eInfoType = _plstreaming_Info_GetType(pCommand);
	switch(eInfoType)
	{
	case PLStreaming_InfoType_Describe:
		break;

	default:
		HxLOG_Error("can't get info - pCommand = %s\r\n", pCommand);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	eHomResult =  PL_Streaming_Context_GetInfo(hStreamingId, eInfoType, pRequestPath, ret_pInfoBufSize, ret_ppInfoBuf);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("eHomResult(%d, %s)!\r\n", eHomResult, PL_Streaming_Debug_GetHomResultStr(eHomResult));
		return eHomResult;
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
HOM_Result_e PL_Streaming_Info_Release ( const Handle_t hStreamingId, const HCHAR *pCommand, const HCHAR *pRequestPath, HCHAR *pInfoBuf )
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;
	PLStreaming_InfoType_e	eInfoType = PLStreaming_InfoType_Unknown;

	ENTER_FUNCTION;

	eInfoType = _plstreaming_Info_GetType(pCommand);
	switch(eInfoType)
	{
	case PLStreaming_InfoType_Describe:
	case PLStreaming_InfoType_Options:
		break;

	default:
		HxLOG_Error("can't get info - pCommand = %s\r\n", pCommand);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	eHomResult =  PL_Streaming_Context_ReleaseInfo(hStreamingId, eInfoType, pRequestPath, pInfoBuf);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("eHomResult(%d, %s)!\r\n", eHomResult, PL_Streaming_Debug_GetHomResultStr(eHomResult));
		return eHomResult;
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
HOM_Result_e PL_Streaming_RegisterStatus( const cbPLStreamingStatus cbStatus)
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	eHomResult =  PL_Streaming_Context_RegisterStatus(cbStatus);

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
HOM_Result_e PL_Streaming_FindByStartInfo(const PLStreaming_StartInfo_t *pstStartInfo, Handle_t *ret_phStreamingId)
{
	Handle_t				hStreamingId = HANDLE_NULL;
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	if(NULL == pstStartInfo || NULL == pstStartInfo->pszStreamingPath)
	{
		HxLOG_Error("invalid param: pstStartInfo(0x%x)\r\n", pstStartInfo);
		return eHOM_Result_Error;
	}

	hStreamingId =  PL_Streaming_Context_FindByStartInfo(pstStartInfo);
	if(HANDLE_NULL == hStreamingId || 0 == hStreamingId)
	{
		HxLOG_Debug("no same info\r\n");
		return eHomResult;
	}

	*ret_phStreamingId = hStreamingId;

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
HBOOL	 PL_Streaming_IsUseStreamType(const DxRsvStreamType_e	eStreamType)
{
	HBOOL bIsUseStreamType = FALSE;

	ENTER_FUNCTION;

	bIsUseStreamType =	PL_Streaming_Context_IsUseStreamType(eStreamType);

	LEAVE_FUNCTION;

	return bIsUseStreamType;
}

/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/

#ifdef CONFIG_DEBUG

STATIC HINT32   _plcmd_streaming_SetPosition(void *szArgList)
{
	HCHAR   		*aucArg=NULL;
	HUINT32			ulPosition = 0;
	Handle_t 		hStreamingId = HANDLE_NULL;
	HCHAR			szReqPath[200] = {0, };

	/* 1st argument */
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%d", (char *)&hStreamingId) == 0 )   	return HxCMD_ERR;

	/* 2nd argument */
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%d", (char *)&ulPosition) == 0 )   	return HxCMD_ERR;

	HLIB_CMD_Printf("hStreamingId = %d, ulPosition = %d\r\n", hStreamingId, ulPosition);
	HxSTD_snprintf(szReqPath, 200, "http://192.168.50.11:9000/web/transcoding/148.m3u8?playPosition=%d", ulPosition);
	PL_Streaming_Context_ExecCmd(hStreamingId, szReqPath);

	return HxCMD_OK;
}

#endif


/**
 *
 *
 *
 * @param
 * @return
 */
void	HOMMA_CMDRegister_Streaming(HCHAR *hCmdHandle)
{

#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord( hCmdHandle, _plcmd_streaming_SetPosition,	"setpos", 	"streamingId position", "set play position");
#endif

}



