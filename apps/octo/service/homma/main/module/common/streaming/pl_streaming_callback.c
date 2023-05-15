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

#include <TransExternApi.h>
#include <transcoderControl.h>

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
#define ENTER_CALLBACK_FUNCTION	HxLOG_Info("(+) Tick(%d)\r\n", HLIB_STD_GetSystemTick())
#define LEAVE_CALLBACK_FUNCTION	HxLOG_Info("(-) Tick(%d)\r\n", HLIB_STD_GetSystemTick())


/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
STATIC PLStreaming_OutType_e s_ePLStreamingOutType = ePLStreaming_OutType_Ringbuf;

#ifdef CONFIG_DEBUG
//#define CHECK_CALLBACK_CALL_TIME
#endif

#ifdef CHECK_CALLBACK_CALL_TIME
#define CHECK_CALL_TIME_GAP							\
{													\
	HUINT32	ulTotalDur = 0;                         \
	HUINT32	ulHour = 0, ulMin = 0, ulSecond = 0;	\
                                                    \
	HUINT32	ulCurTick = 0, ulTimeGap = 0;           \
                                                    \
	/* check kbps of writing to homma */            \
	ulCurTick = HLIB_STD_GetSystemTick();           \
	ulTimeGap = ulCurTick - s_ulCallbackLastTick;   \
	s_ulCallbackLastTick = ulCurTick;               \
                                                    \
	HxLOG_Warning("gap = %d ticks\n", ulTimeGap);   \
}
STATIC HUINT32	s_ulCallbackLastTick;
#else
#define CHECK_CALL_TIME_GAP
#endif

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC DxRsvStreamType_e _plstreaming_Callback_Streaming_DLNATypeToRsvStreamType (const HINT32 requst_spec_type, const HINT32 requst_media_type)
{
	PLRequstSpecType_e	eRequstSpecType = ePLRequstSpecType_Unknown;
	PLRequstMediaType_e	eRequstMediaType = ePLRequstMediaType_Unknown;

	switch((Streaming_SpecType_e)requst_spec_type)
	{
	case eStreamingSpecType_DMS:		eRequstSpecType = ePLRequstSpecType_HLS;	break;
	case eStreamingSpecType_WOON:		eRequstSpecType = ePLRequstSpecType_HLS;	break;
	case eStreamingSpecType_SATIP:		eRequstSpecType = ePLRequstSpecType_SATIP;	break;
    case eStreamingSpecType_NTLS:		eRequstSpecType = ePLRequstSpecType_NTLS;	break;
	case eStreamingSpecType_Unknown:
	default:
		return DxRSVSTREAM_TYPE_UNKNOWN;
	}

	switch((Streaming_MediaType_e)requst_media_type)
	{
	case eStreamingMediaType_PVRTS_AUDIO:	eRequstMediaType = ePLRequstMediaType_PVRTS_A;	break;
	case eStreamingMediaType_PVRTS_VIDEO:	eRequstMediaType = ePLRequstMediaType_PVRTS_AV;	break;
	case eStreamingMediaType_LIVE_AUDIO:	eRequstMediaType = ePLRequstMediaType_LIVE_A;	break;
	case eStreamingMediaType_LIVE_VIDEO:	eRequstMediaType = ePLRequstMediaType_LIVE_AV;	break;
	default:
		return DxRSVSTREAM_TYPE_UNKNOWN;
	}

	switch(eRequstSpecType)
	{
	case ePLRequstSpecType_HLS:
		switch(eRequstMediaType)
		{
		case ePLRequstMediaType_PVRTS_A:	return DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO;
		case ePLRequstMediaType_PVRTS_AV:	return DxRSVSTREAM_TYPE_HLS_FILE_PVR;
		case ePLRequstMediaType_LIVE_A:		return DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO;
		case ePLRequstMediaType_LIVE_AV:	return DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO;
		default:
			break;
		}
		break;

	case ePLRequstSpecType_SATIP:
		switch(eRequstMediaType)
		{
		case ePLRequstMediaType_LIVE_A: 	return DxRSVSTREAM_TYPE_SATIP_LIVE;
		case ePLRequstMediaType_LIVE_AV:	return DxRSVSTREAM_TYPE_SATIP_LIVE;
		default:
			break;
		}
		break;

    case ePLRequstSpecType_NTLS:
        switch(eRequstMediaType)
        {
		case ePLRequstMediaType_PVRTS_A:	return DxRSVSTREAM_TYPE_NTLS_FILE_PVR_AUDIO;
		case ePLRequstMediaType_PVRTS_AV:	return DxRSVSTREAM_TYPE_NTLS_FILE_PVR;
        case ePLRequstMediaType_LIVE_A: 	return DxRSVSTREAM_TYPE_NTLS_LIVE_AUDIO;
        case ePLRequstMediaType_LIVE_AV:	return DxRSVSTREAM_TYPE_NTLS_LIVE_VIDEO;
        default:
            break;
        }
        break;

	default:
		break;
	}

	return DxRSVSTREAM_TYPE_UNKNOWN;
}

/*
- call flow
	* HLS
		Streaming Start->
		Get Root M3U8 ->
		Release Root M3U8 ->
		Get Bitrate M3U8 ->
		Release Bitrate M3U8 ->
		Get TS  ->
		Release TS ->
		Streaming End

	* sat/ip
		- m3u8
		Streaming Start->
		Get Root M3U8 ->
		Release Root M3U8 ->
		Streaming End

		- ts
		Streaming Start->
		Get TS  ->
		Release TS ->
		Streaming End
*/

/**
  @author mhkang2
  @date 2015/3/31
  @brief when streaming starts, this function is called by mxlib.
  @param device_name            SAT>IP Streaming에서는 streamid 값으로 사용된다.
  @param socketfd               if sat>ip streaming type, socketfd[0] is serverfd and  socketfd[1] is clientfd,  if other streaming type, socketfd is null.
  @param[OUT] ret_phStreamingId
*/
STATIC HOM_Result_e _plstreaming_Callback_Streaming_Start (	const HINT32	requst_spec_type,
															const HINT32	requst_media_type,
															const HCHAR 	*device_name,
															const HCHAR 	*user_agent,
															const HCHAR 	*session_id,
															const HCHAR 	*path_name,
															const HCHAR 	*ip,
                                                            int             socketfd[2],
															Handle_t 		*ret_phStreamingId )
{
	HBOOL					bIsUseStreamType = FALSE;
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;
	Handle_t 				hStreaming = HANDLE_NULL;
	Handle_t				hSetupInfo = HANDLE_NULL;
	PLStreaming_StartInfo_t	stStartInfo;

	DxRsvStreamType_e		eStreamType			= _plstreaming_Callback_Streaming_DLNATypeToRsvStreamType(requst_spec_type, requst_media_type);
	const HCHAR				*pszDeviceName		= device_name;
	const HCHAR				*pszDeviceID		= session_id;
	const HCHAR				*pszUserAgent		= user_agent;
	const HCHAR				*pszIPAddress		= ip;
	const HCHAR				*pszStreamingPath	= path_name;
	HUINT16 uRetry = 0;

	if(NULL != ret_phStreamingId)	*ret_phStreamingId = HANDLE_NULL;

	HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
	HxLOG_Print(HxANSI_COLOR_CYAN("###### %s (%s)\n"), __FUNCTION__, session_id);
	HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
	CHECK_CALL_TIME_GAP;

	PL_COMMON_PrintTime(NULL);

    HxLOG_Info("(+) eStreamType(0x%x) pszDeviceName(%s) pszDeviceID(%s) pszUserAgent(%s) pszIPAddress(%s) pszStreamingPath(%s)\n",
          eStreamType, pszDeviceName, pszDeviceID, pszUserAgent, pszIPAddress, pszStreamingPath);	


	ENTER_CALLBACK_FUNCTION;

	if(DxRSVSTREAM_TYPE_UNKNOWN == eStreamType)
	{
		HxLOG_Error("Error> Unknown streamT type! requst_spec_type :%d, requst_media_type:%d \n", requst_spec_type, requst_media_type);
		return eHOM_Result_Error;
	}

	HxSTD_memset(&stStartInfo, 0, sizeof(PLStreaming_StartInfo_t));

	bIsUseStreamType = PL_Streaming_IsUseStreamType(eStreamType);
	if(FALSE == bIsUseStreamType)
	{
		HxLOG_Error("Error> Not init StreamType (%d)\r\n", eStreamType);
		return eHOM_Result_Error_NotInit;
	}

    eHomResult = PL_Streaming_CreateStartInfo(eStreamType, s_ePLStreamingOutType, pszDeviceName, pszUserAgent, pszDeviceID, pszStreamingPath, pszIPAddress, socketfd, &stStartInfo);
	if(	eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("Error> PL_Streaming_CreateStartInfo fail(err:0x%x, device:%s)\r\n", eHomResult, pszDeviceID);
		goto END_FUNCTION;
	}

	/* duplication check */
	eHomResult = PL_Streaming_FindByStartInfo(&stStartInfo, &hStreaming);
	if(	eHOM_Result_Ok == eHomResult )
	{
		if(HANDLE_NULL != hStreaming &&	0 != hStreaming)
		{
			HxLOG_Error("Error> duplication start pszDeviceID(%s)\r\n", pszDeviceID);
			*ret_phStreamingId = hStreaming;
			goto END_FUNCTION;
		}
	}

	/* Retry again for unfreezed sama resources */
func_retry:
	eHomResult = PL_Streaming_CreateSetupHandle(&stStartInfo, &hSetupInfo);
	if(eHOM_Result_Ok != eHomResult || HANDLE_NULL == hSetupInfo || 0 == hSetupInfo)
	{
		HxLOG_Error("Critical/Error> CreateSetupHandle fail(err:0x%x, retry:%d)\r\n", eHomResult, uRetry);
		if ((eHOM_Result_Error_Resource_Full == eHomResult) && (uRetry < 5))
		{
			HLIB_STD_TaskSleep(100);
			uRetry++; goto func_retry;
		}
		goto END_FUNCTION;
	}

	eHomResult = PL_Streaming_Start(hSetupInfo, &hStreaming);
	if(eHOM_Result_Ok != eHomResult || HANDLE_NULL == hStreaming || 0 == hStreaming)
	{
		HxLOG_Error("Critical/Error> Streaming_Start fail(err:0x%x)!\r\n", eHomResult);
		goto END_FUNCTION;
	}
/* 2014/3/27 mhkang: STB채널이 변경(Add/Modify/Delete)되면  EventListener가 호출되어 업데이트되기 때문에 여기서 아래코드가 호출될 필요는 없다.
	PL_COMMON_UpdateChannelList();
*/
	*ret_phStreamingId = hStreaming;
	eHomResult = eHOM_Result_Ok;

END_FUNCTION:
	PL_Streaming_DestroySetupHandle(hSetupInfo);
	PL_Streaming_DestroyStartInfo(&stStartInfo);
	CHECK_CALL_TIME_GAP;

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(%x) eHomResult(%s)\r\n", __FUNCTION__, __LINE__, hStreaming, PL_Streaming_Debug_GetHomResultStr(eHomResult));
	LEAVE_CALLBACK_FUNCTION;

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Callback_Streaming_End (const Handle_t hStreaming)
{
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;

	HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
    HxLOG_Print(HxANSI_COLOR_CYAN("###### %s hStreaming(%x)\n"), __FUNCTION__, hStreaming);
	HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
	CHECK_CALL_TIME_GAP;
	PL_COMMON_PrintTime(NULL);

    PL_Streaming_Debug_Print("[%s:%d] (+) hStreaming(0x%x) \n", __FUNCTION__, __LINE__, hStreaming);

	ENTER_CALLBACK_FUNCTION;
	PL_Streaming_End(hStreaming);
	LEAVE_CALLBACK_FUNCTION;

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(%x) eHomResult(%s)\r\n", __FUNCTION__, __LINE__, hStreaming, PL_Streaming_Debug_GetHomResultStr(eHomResult));

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Callback_M3U8_Get ( const Handle_t hStreaming, const HCHAR *pRequestPath, HINT32 *ret_pM3U8BufSize, HUINT8 **ret_ppM3U8Buf)
{
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;

	HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
    HxLOG_Print(HxANSI_COLOR_CYAN("###### %s hStreaming(%x)\n"), __FUNCTION__, hStreaming);
	HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
	CHECK_CALL_TIME_GAP;
	PL_COMMON_PrintTime(NULL);

    PL_Streaming_Debug_Print("[%s:%d] (+) hStreaming(0x%x) pRequestPath(%s)\n", __FUNCTION__, __LINE__, hStreaming, pRequestPath);

    if (ret_pM3U8BufSize == NULL || ret_ppM3U8Buf == NULL)
    {
        HxLOG_Error("Invalid args(ret_pM3U8BufSize:%p , ret_ppM3U8Buf:%p)\n",ret_pM3U8BufSize , ret_ppM3U8Buf);
        return eHOM_Result_Error_Invalid_Arguments;
    }

	if(NULL != ret_pM3U8BufSize)	*ret_pM3U8BufSize = 0;
	if(NULL != ret_ppM3U8Buf)		*ret_ppM3U8Buf = NULL;

	ENTER_CALLBACK_FUNCTION;
	eHomResult = PL_Streaming_M3U8_Get(hStreaming, pRequestPath, ret_pM3U8BufSize, ret_ppM3U8Buf);
	LEAVE_CALLBACK_FUNCTION;

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(%x) eHomResult(%s)\r\n", __FUNCTION__, __LINE__, hStreaming, PL_Streaming_Debug_GetHomResultStr(eHomResult));

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Callback_M3U8_Release (const Handle_t hStreaming, const HCHAR *pRequestPath, HCHAR *pM3U8Buf)
{
	HOM_Result_e  			eHomResult = eHOM_Result_Ok;

	//HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
	//HxLOG_Print(HxANSI_COLOR_CYAN("###### %s \n"), __FUNCTION__);
	//HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
	CHECK_CALL_TIME_GAP;
	PL_COMMON_PrintTime(NULL);

    PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(0x%x) pRequestPath(%s)\n", __FUNCTION__, __LINE__, hStreaming, pRequestPath);

	ENTER_CALLBACK_FUNCTION;
	eHomResult = PL_Streaming_M3U8_Release(hStreaming, pRequestPath, pM3U8Buf);
	LEAVE_CALLBACK_FUNCTION;

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(%x) eHomResult(%s)\r\n", __FUNCTION__, __LINE__, hStreaming, PL_Streaming_Debug_GetHomResultStr(eHomResult));

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Callback_TS_Get ( const Handle_t hStreaming, const HCHAR *pRequestPath, HINT32 *ret_pTSBufSize, HUINT8 **ret_ppTSBuf)
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	HxLOG_Debug(HxANSI_COLOR_CYAN("################################################################\n"));
	HxLOG_Debug(HxANSI_COLOR_CYAN("###### %s(%s) hStreaming(%x)\n"), __FUNCTION__, (ret_pTSBufSize == NULL && ret_ppTSBuf == NULL)?"CMD":"DAT", hStreaming);
	HxLOG_Debug(HxANSI_COLOR_CYAN("################################################################\n"));
	CHECK_CALL_TIME_GAP;

	PL_Streaming_Debug_Print("[%s:%d] (+) %s Mode - hStreaming(0x%x) pRequestPath(%s)\n",
									__FUNCTION__, __LINE__, ((ret_pTSBufSize == NULL && ret_ppTSBuf == NULL)?"CMD":"DAT"), hStreaming, pRequestPath);

	PL_COMMON_PrintTime(NULL);

	if(NULL != ret_pTSBufSize)	*ret_pTSBufSize = 0;
	if(NULL != ret_ppTSBuf)		*ret_ppTSBuf = NULL;

	ENTER_CALLBACK_FUNCTION;
	eHomResult = PL_Streaming_TS_Get(hStreaming, pRequestPath, ret_pTSBufSize, ret_ppTSBuf);
	LEAVE_CALLBACK_FUNCTION;

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(%x) eHomResult(%s)\r\n", __FUNCTION__, __LINE__, hStreaming, PL_Streaming_Debug_GetHomResultStr(eHomResult));

	CHECK_CALL_TIME_GAP;

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_Callback_TS_Release (const Handle_t hStreaming, const HCHAR *pRequestPath, HCHAR *pTSBuf )
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	//HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
	//HxLOG_Print(HxANSI_COLOR_CYAN("###### %s \n"), __FUNCTION__);
	//HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
	CHECK_CALL_TIME_GAP;

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(0x%x) pRequestPath(%s) pTSBuf(0x%x)\n", __FUNCTION__, __LINE__, hStreaming, pRequestPath, pTSBuf);

	ENTER_CALLBACK_FUNCTION;
	eHomResult = PL_Streaming_TS_Release(hStreaming, pRequestPath, pTSBuf);
	LEAVE_CALLBACK_FUNCTION;

	PL_COMMON_PrintTime(NULL);

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(%x) eHomResult(%s)\r\n", __FUNCTION__, __LINE__, hStreaming, PL_Streaming_Debug_GetHomResultStr(eHomResult));

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_CallBack_Info_Get( const Handle_t hStreaming, const HCHAR *pCommand, const HCHAR *pRequestPath, HINT32 *ret_pInfoBufSize, HCHAR **ret_ppInfoBuf )
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;


	CHECK_CALL_TIME_GAP;

    PL_Streaming_Debug_Print("[%s:%d] (+) hStreaming(0x%x) pCommand(%s) pRequestPath(%s)\n", __FUNCTION__, __LINE__, hStreaming, pCommand, pRequestPath);

    if (ret_ppInfoBuf == NULL || ret_pInfoBufSize == NULL)
    {
        HxLOG_Error("Invalid args(ret_ppInfoBuf:%p, ret_pInfoBufSize:%p)\n", ret_ppInfoBuf, ret_pInfoBufSize);
        return eHOM_Result_Error_Invalid_Arguments;
    }

	if(NULL != ret_pInfoBufSize)	*ret_pInfoBufSize = 0;
	if(NULL != ret_ppInfoBuf)		*ret_ppInfoBuf = NULL;

	ENTER_CALLBACK_FUNCTION;
	eHomResult = PL_Streaming_Info_Get(hStreaming, pCommand, pRequestPath, ret_pInfoBufSize, ret_ppInfoBuf);
	LEAVE_CALLBACK_FUNCTION;

    if (*ret_ppInfoBuf == NULL)
        HxLOG_Error(HxANSI_COLOR_RED("############################### PL_Streaming_Info_Get Failed #################################\n"));

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(%x) eHomResult(%s)\r\n", __FUNCTION__, __LINE__, hStreaming, PL_Streaming_Debug_GetHomResultStr(eHomResult));

	CHECK_CALL_TIME_GAP;

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_CallBack_Info_Release( const Handle_t hStreaming, const HCHAR *pCommand, const HCHAR *pRequestPath, HCHAR *pInfoBuf )
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	CHECK_CALL_TIME_GAP;

    PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(0x%x) pCommand(%s) pRequestPath(%s)\n",
									__FUNCTION__, __LINE__, hStreaming, pCommand, pRequestPath);

	ENTER_CALLBACK_FUNCTION;
	eHomResult = PL_Streaming_Info_Release(hStreaming, pCommand, pRequestPath, pInfoBuf);
	LEAVE_CALLBACK_FUNCTION;

	PL_Streaming_Debug_Print("[%s:%d] (-) hStreaming(%x) eHomResult(%s)\r\n", __FUNCTION__, __LINE__, hStreaming, PL_Streaming_Debug_GetHomResultStr(eHomResult));

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_CallBack_RegisterStatus( const cbPLStreamingStatus cbStatus)
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	HxLOG_Print("(+) cbStatus(%x)\r\n", cbStatus);

	ENTER_CALLBACK_FUNCTION;
	eHomResult = PL_Streaming_RegisterStatus(cbStatus);
	LEAVE_CALLBACK_FUNCTION;

	if (eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("Error> failed to register(%s)\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));
	}
	HxLOG_Print("(-) eHomResult(%s)\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));

	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_CallBack_Init( void )
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	HxLOG_Print("(+)\r\n");

	ENTER_CALLBACK_FUNCTION;

	LEAVE_CALLBACK_FUNCTION;

	HxLOG_Print("(-) eHomResult(%s)\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));

	/*  Init 함수가 OK 를 리턴해야 Init 과정이 Lib 단에서 정상 등록이라고 판단..  */
	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_CallBack_Deinit( void )
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	HxLOG_Print("(+)\r\n");

	ENTER_CALLBACK_FUNCTION;

	LEAVE_CALLBACK_FUNCTION;

	HxLOG_Print("(-) eHomResult(%s)\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));

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
HOM_Result_e PL_Streaming_Callback_Init(void)
{
	TransExApi		stransExApi;
	HULONG			ulHlsTimeOut = 30000; /* 30sec */

	ENTER_FUNCTION;

	HxSTD_memset(&stransExApi, 0, sizeof(TransExApi));

	stransExApi.fp_ExStart					=  (fp_Streaming_Start)_plstreaming_Callback_Streaming_Start;
	stransExApi.fp_ExStop					=  (fp_Streaming_Stop)_plstreaming_Callback_Streaming_End;

	stransExApi.fp_ExM3U8_Get				=  (fp_Streaming_M3U8_Get)_plstreaming_Callback_M3U8_Get;
	stransExApi.fp_ExM3U8_Release			=  (fp_Streaming_M3U8_Release)_plstreaming_Callback_M3U8_Release;

	stransExApi.fp_ExTS_Get					=  (fp_Streaming_TS_Get)_plstreaming_Callback_TS_Get;
	stransExApi.fp_ExTS_Release				=  (fp_Streaming_TS_Release)_plstreaming_Callback_TS_Release;

#if 1
	stransExApi.fp_ExInfo_Get				=  (fp_Streaming_Info_Get)_plstreaming_CallBack_Info_Get;
	stransExApi.fp_ExInfo_Release			=  (fp_Streaming_Info_Release)_plstreaming_CallBack_Info_Release;
#endif

	stransExApi.fp_ExRegisterStateCallback 	=  (fp_Streaming_RegisterStateCallback)_plstreaming_CallBack_RegisterStatus;

	stransExApi.fp_ExInit					=  (fp_Streaming_Init)_plstreaming_CallBack_Init;
	stransExApi.fp_ExDeinit 				=  (fp_Streaming_Deinit)_plstreaming_CallBack_Deinit;

	TransCtrl_Init((void *)&stransExApi, &ulHlsTimeOut );

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
void 	PL_Streaming_Callback_DeInit(void)
{
	ENTER_FUNCTION;

	LEAVE_FUNCTION;

	return ;
}


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/

#if defined(CONFIG_DEBUG)
STATIC void  _plcmd_streaming_Callback_SatipDump_Chlist(void)
{
 	Handle_t 		hStreamId			= HANDLE_NULL;
	HBOOL			bIsSetOutType		= FALSE;
	HOM_Result_e	eHomResult 			= eHOM_Result_Ok;

	if(0 == (s_ePLStreamingOutType & ePLStreaming_OutType_DumpFile))
	{
		bIsSetOutType = TRUE;
		s_ePLStreamingOutType |= ePLStreaming_OutType_DumpFile;
	}

    eHomResult = _plstreaming_Callback_Streaming_Start( eStreamingSpecType_SATIP, eStreamingMediaType_LIVE_VIDEO, NULL, NULL, NULL, NULL, NULL, NULL, &hStreamId);
	if(eHOM_Result_Ok == eHomResult)
	{
		HCHAR			*pszM3U8Path		= "rtsp://192.168.22.153:554";
		HINT32			nM3U8Size			= 0;
		HUINT8			*pM3U8Buf			= NULL;

		eHomResult = _plstreaming_Callback_M3U8_Get(hStreamId, pszM3U8Path, &nM3U8Size, &pM3U8Buf);
		if(eHOM_Result_Ok == eHomResult)
		{
			HxFILE_t		fp = NULL;

			fp = HLIB_FILE_Open("satip_chlist.m3u8", "wb");
			if(fp)
			{
				HLIB_FILE_Write(fp, pM3U8Buf, nM3U8Size, 1);
				HLIB_FILE_Close(fp);
			}

			_plstreaming_Callback_M3U8_Release(hStreamId, pszM3U8Path, pM3U8Buf);
		}
		_plstreaming_Callback_Streaming_End(hStreamId);
	}

	if(TRUE == bIsSetOutType)
	{
		s_ePLStreamingOutType &= ~ePLStreaming_OutType_DumpFile;
	}

}

STATIC HINT32   _plcmd_streaming_Callback_SatipDump(void *szArgList)
{
	HCHAR   		*aucArg=NULL;
	HCHAR			szStr[HxCMD_MAX_WORD_LEN] = {0, };

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&szStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(szStr);

	HLIB_CMD_Printf("szStr start\r\n", szStr);

	if(VK_strcmp(szStr, "CHLIST") == 0)
	{
		_plcmd_streaming_Callback_SatipDump_Chlist();
	}
	else if (VK_strcmp(szStr, "PVR") == 0)
	{

	}

	HLIB_CMD_Printf("szStr end\r\n", szStr);


	return HxCMD_OK;
}


STATIC HINT32   _plcmd_streaming_Callback_filedump(void *szArgList)
{
	if(ePLStreaming_OutType_DumpFile & s_ePLStreamingOutType)
	{
		s_ePLStreamingOutType &= (~ePLStreaming_OutType_DumpFile);
		HLIB_CMD_Printf("ePLStreaming_OutType_DumpFile off!! \r\n");
	}
	else
	{
		s_ePLStreamingOutType |= ePLStreaming_OutType_DumpFile;
		HLIB_CMD_Printf("ePLStreaming_OutType_DumpFile on!! \r\n");
	}


	return HxCMD_OK;
}

STATIC HINT32   _plcmd_streaming_Callback_chlist(void *szArgList)
{
	HCHAR   		*aucArg=NULL;
	HCHAR			szStr[HxCMD_MAX_WORD_LEN] = {0, };
	HINT32			nPrintOption = 0;
	HxList_t 		*pList = NULL;
	HxList_t 		*pRootList = NULL;
	CO_CHANNEL_t	*pstChannel = NULL;
	HERROR			hErr = ERR_FAIL;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&szStr) == 0 )
	{
		//
	}

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &nPrintOption) == 0 )
		{
			//
		}
	}
	HLIB_CMD_Printf("Print Option = %d\r\n", nPrintOption);


	HLIB_STD_StrUpper(szStr);

	HLIB_CMD_Printf("szStr start\r\n", szStr);

	/* PL_COMMON_InitChannelList(); */

	hErr = PL_COMMON_GetChannelList(&pRootList); /* <-- error check */
	if(ERR_OK == hErr)
	{
		pList = pRootList;
		while(pList)
		{
			pstChannel = (CO_CHANNEL_t *)HLIB_LIST_Data(pList);
			if(NULL != pstChannel && NULL != pstChannel->svc && NULL != pstChannel->ts)
			{
				HLIB_CMD_Printf("CH(%3d. %s) id(%d,%d,%d) eDeliType(%d) svc_uid(%d) ts_uid(%d) \n",
					pstChannel->svc->lcn, pstChannel->svc->name,
					pstChannel->ts->onid, pstChannel->ts->tsid, pstChannel->svc->svcid,
					pstChannel->ts->tuningParam.eDeliType,
					pstChannel->svc->uid,
					pstChannel->ts->uid);
				if(nPrintOption > 0)
				{
					HLIB_CMD_Printf("\t\t --> %d(%c) %d \n",
						pstChannel->ts->tuningParam.sat.tuningInfo.ulFrequency,
						(pstChannel->ts->tuningParam.sat.tuningInfo.ePolarization == eDxSAT_POLAR_HOR) ? 'H' : (pstChannel->ts->tuningParam.sat.tuningInfo.ePolarization == eDxSAT_POLAR_VER) ? 'V' : '?',
						pstChannel->ts->tuningParam.sat.tuningInfo.ulSymbolRate);
				}
			}
			pList = HLIB_LIST_Next(pList);
		}

		PL_COMMON_ReleaseChannelList(pRootList);
	}
	else
	{
		HLIB_CMD_Printf("Can't get Channel List\r\n");
	}

	HLIB_CMD_Printf("szStr end\r\n", szStr);


	return HxCMD_OK;
}

STATIC HINT32   _plcmd_streaming_Callback_printmem(void *szArgList)
{

	PL_Streaming_Debug_PrintMem(HLIB_STD_GetPID());

	return HxCMD_OK;
}

#endif

void	HOMMA_CMDRegister_StreamingCallback(const HCHAR *hCmdHandle)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord( hCmdHandle, _plcmd_streaming_Callback_filedump,	"filedump", 	"|", "ts dump");
	HLIB_CMD_RegisterWord( hCmdHandle, _plcmd_streaming_Callback_chlist, 	"chlist", 		"|", "chlist test action");
	HLIB_CMD_RegisterWord( hCmdHandle, _plcmd_streaming_Callback_SatipDump, "dump_satip", 	"chlist", "sat/ip dump");
	HLIB_CMD_RegisterWord( hCmdHandle, _plcmd_streaming_Callback_printmem,	"printmem", 	"homma pid", "printmem getting ts");
#endif
}


