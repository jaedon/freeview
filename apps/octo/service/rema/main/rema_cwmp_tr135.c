/**************************************************************
 *	@file		trport_tr135.c
 *	appkit for cwmp Module
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/05/14
 *	@author			humax
 *	@brief			trport for cwmp
 **************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <wait.h>
#include <hlib.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>

#include "rema_int.h"
#include "rema_cwmp.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#if 0
#define HxLog_Print printf
#define HxLog_Debug printf
#define HxLog_Message printf
#define HxLog_info printf
#endif

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC REMA_OUTPUT_VIDEO_Notifier_t	s_fAppNotifier;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ______________________STATIC_FUNCTION_TRP_GET______________________

STATIC void tr135_output_video_onStatusChanged(void)
{
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(s_fAppNotifier != NULL)
	{
		HxLOG_Warning("[APK EMIT]VIDEO s_fAppNotifier [%p]\n", s_fAppNotifier);
	}
}

STATIC HERROR	tr135_scenario_GetStrValue(const HCHAR *pszKey, HCHAR *pszValue, HINT32 nMax)
{
	return HAPI_GetRegistryStr(pszKey, pszValue, nMax);
}



static HERROR get_Service_STBServiceNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//2 항상 1

	*pszOutput  = 1;

	REM_LEAVE(hErr);
}
static HERROR get_Capability_MaxActiveAVStreams(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
//	APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_MaxActiveAVStreams);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capability_MaxActiveAVPlayers(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
//	APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_MaxActiveAVPlayers);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_PVR_MaxIOStreams(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "3");
	#else

//	APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_PVR_MaxIOStreams);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_PVR_MaxRecordingStreams(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "2");
	#else

//	APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_PVR_MaxRecordingStreams);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_PVR_MaxPlaybackStreams(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "1");
	#else

//	APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_PVR_MaxPlaybackStreams);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_AudioDecoder_AudioStandards(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "MPEG1-Part3-Layer2");
	#else

//	APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_AudioDecoder_AudioStandards);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP2P2_AudioStandards(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "MPEG2-Part3?Layer2");
	#else

//	APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_AudioStandards);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP2P2_ProfileLevelNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

//TRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 0;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif
	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP2P2_ProfileLevel_M_Profile(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
//INT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "MP");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG2Part2_ProfileLevel_M_Profile, nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP2P2_ProfileLevel_M_Level(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
//	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "HL");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG2Part2_ProfileLevel_M_Level, nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP2P2_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "81920");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG2Part2_ProfileLevel_M_MaximumDecodingCapability, nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P2_AudioStandards(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else

//	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_AudioStandards);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P2_ProfileLevelNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_ProfileLevelNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 0;
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);

	//rema_DM_MultiInstance_SetEntry(pszOutput,eType);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P2_ProfileLevel_M_Profile(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part2_ProfileLevel_M_Profile, nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P2_ProfileLevel_M_Level(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part2_ProfileLevel_M_Level,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P2_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part2_ProfileLevel_M_MaximumDecodingCapability,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P10_AudioStandards(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "MPEG4-AAC-HE-v2");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_AudioStandards);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P10_ProfileLevelNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_ProfileLevelNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 0;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);

	//rema_DM_MultiInstance_SetEntry(pszOutput,eType);
	#endif
	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P10_ProfileLevel_M_Profile(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "HIGH");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M_Profile,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P10_ProfileLevel_M_Level(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "4");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M_Level,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_MP4P10_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "25000");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_ProfileLevelNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M_MaximumDecodingCapability,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_SMPTEVC1_AudioStandards(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_SMPTEVC1_ProfileLevelNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	*pszOutput = 0;

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Profile(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Level(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VDec_SMPTEVC1_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "0",2);

	REM_LEAVE(hErr);
}

static HERROR get_Capa_VDec_VideoStandards(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "MPEG2-Part2");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoDecoder_VideoStandards);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_AOutput_AudioFormats(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "HDMI");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_AudioOutput_AudioFormats);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VOutput_CompositeVideoStandards(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "NTSC-J");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoOutput_CompositeVideoStandards);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VOutput_VideoFormats(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "HDMI");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoOutput_VideoFormats);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VOutput_Macrovision(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "7.1.L1");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoOutput_Macrovision);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VOutput_HDCP(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "1.4");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoOutput_HDCP);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_VOutput_DisplayFormats(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "16:9");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_VideoOutput_DisplayFormats);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_HDMI_SupportedResolutions(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "1920x1080p/30Hz");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_HDMI_SupportedResolutions);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_HDMI_CECSupport(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "true");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_HDMI_CECSupport);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_HDMI_HDMI3D(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "true");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_HDMI_HDMI3D);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_CA_CASystems(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "B-CAS");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_CA_CASystems);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_DRM_DRMSystems(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Playready");
	#else

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_Capa_DRM_DRMSystems);

	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Capa_ServiceMonitoring_MaxActiveMainStreams(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "0",2);

	REM_LEAVE(hErr);
}
static HERROR get_Capa_ServiceMonitoring_MinSampleInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "0",2);

	REM_LEAVE(hErr);
}
static HERROR get_Capa_ServiceMonitoring_MaxReportSamples(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "0",2);

	REM_LEAVE(hErr);
}
static HERROR get_Capa_ServiceMonitoring_HighLevelMetricNames(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	REM_LEAVE(hErr);
}
static HERROR get_Comp_FrontEndNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 4;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Comp_AudioDecoderNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_AudioDecoderNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 2;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Comp_VideoDecoderNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 2;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Comp_AudioOutputNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 1;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Comp_VideoOutputNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 1;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Comp_CANumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_CANumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 1;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Comp_HDMINumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);

	REM_LEAVE(hErr);
}
static HERROR get_Comp_SPDIFNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_SPDIFNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 1;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif

	REM_LEAVE(hErr);
}
static HERROR get_Comp_FrontEnd_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Enabled");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_Status,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "DOCSIS1");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_Name,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Frequency(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_Frequency,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Constellation(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_Constellation,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_SNR(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_SNR,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_BER(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_BER,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Power(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_Power,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Locked(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "false");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Modulation_Locked,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_ISDBC_Service_CurrentLogicalChannel(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_ISDBC_Service_CurrentLogicalChannel,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Initializing");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_Status,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_IPAddress(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_IPAddress,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_MACAddress(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_MACAddress,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Frequency(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_Frequency,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Constellation(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_Constellation,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_download_BER(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_BER,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_download_SNR(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_SNR,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Power(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_download_Power,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Frequency(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_upload_Frequency,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_ChannelId(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_upload_ChannelId,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_BandWidth(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_upload_BandWidth,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Power(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_FrontEnd_M_X_JCOM_DOCSIS_upload_Power,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_PVR_StorageNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_Comp_PVR_StorageNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	*pszOutput = 0;
	#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);
	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_PVR_Storage_M_Reference(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Device.Services.StorageService.1");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_PVR_StorageNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_PVR_Storage_M_Reference,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_AudioDecoder_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Enabled");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioDecoderNumberOfEntries,&nArg2);


	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_AudioDecoder_M_Status,nArg1,nArg2);


	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_AudioDecoder_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "AudioDecoder");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioDecoderNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_AudioDecoder_M_Name,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_AudioDecoder_M_AudioStandard(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioDecoderNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_AudioDecoder_M_AudioStandard,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoDecoder_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Enabled");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoDecoder_M_Status,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoDecoder_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "VideoDecoder");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoDecoder_M_Name,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoDecoder_M_MPEG2Part2(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoDecoder_M_MPEG2Part2,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoDecoder_M_MPEG4Part2(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoDecoder_M_MPEG4Part2,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoDecoder_M_MPEG4Part10(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoDecoder_M_MPEG4Part10,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoDecoder_M_SMPTEVC1(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	REM_LEAVE(hErr);

}

static HERROR get_Comp_VideoDecoder_M_ContentAspectRatio(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoDecoder_M_ContentAspectRatio,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_AudioOutput_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_AudioOutput_M_Status,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_AudioOutput_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "AudioOutput");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_AudioOutput_M_Name,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_AudioOutput_M_AudioFormat(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_AudioOutput_M_AudioFormat,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_AudioOutput_M_AudioLevel(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "100");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_AudioOutput_M_AudioLevel,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_AudioOutput_M_CancelMute(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "false");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_AudioOutput_M_CancelMute,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_SPDIF_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Enabled");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_SPDIFNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_SPDIF_M_Status,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_SPDIF_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "SPDIFOutput");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_SPDIFNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_SPDIF_M_Name,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_SPDIF_M_ForcePCM(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR *data;
	REM_ENTER;

	HxSTD_StrCpy(pszOutput, "false"); //dummy
	/*
	DxAudioDigitalOut_e eSpdifOut = eDxAUDIO_DIGITALOUT_UNKNOWN;
	APKS_Pair_t		_DigitalOutList[] =
	{
		{	eDxAUDIO_DIGITALOUT_DOWNMIX,	"PCM"				},
		{	eDxAUDIO_DIGITALOUT_MULTICHANNEL, 	"Multi Channel"		},
		{	eDxAUDIO_DIGITALOUT_BYPASS_AAC, "AAC"				},
		{	eDxAUDIO_DIGITALOUT_BYPASS_AC3, "Bypass AC3"			},
		{	eDxAUDIO_DIGITALOUT_UNKNOWN, 		NULL			}
	};

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	eSpdifOut = APK_UDB_GetValue((const HUINT8 *)USERDB_AUDIO_SPDIF_AUDIO_OUT);
	data = APK_UTIL_GetStringFromValue(_DigitalOutList, eSpdifOut);

	if((data == NULL) || ((HxSTD_StrCmp(data, "PCM") != 0) && (HxSTD_StrCmp(data, "AAC") != 0)))
	{
		RemLOG_Print("APK_UTIL_GetStringFromValue return %s\n",(data?data:"NULL"));
		REM_LEAVE(hErr);
	}

	if(HxSTD_StrCmp(data, "PCM") == 0)
	{
		HxSTD_StrCpy(pszOutput, "true");
	}
	else
	{
		HxSTD_StrCpy(pszOutput, "false");
	}
#endif
*/

	REM_LEAVE(hErr);

}
static HERROR get_Comp_SPDIF_M_Passthrough(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "false");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_SPDIFNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_SPDIF_M_Passthrough,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_SPDIF_M_AudioDelay(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "0");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_SPDIFNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_SPDIF_M_AudioDelay,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoOutput_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "Enabled");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoOutput_M_Status,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoOutput_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "VideoOutput");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoOutput_M_Name,nArg1,nArg2);
#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoOutput_M_CompositeVideoStandard(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoOutput_M_CompositeVideoStandard,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoOutput_M_VideoFormat(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1

	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoOutput_M_VideoFormat,nArg1,nArg2);

	#endif


	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoOutput_M_AspectRatioBehaviour(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "None");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoOutput_M_AspectRatioBehaviour,nArg1,nArg2);

	#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoOutput_M_DisplayFormat(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
/*
	HINT32	nArg1 = 0, nArg2 = 0;
	APKE_AspectRatio_e	eTVAspectRatio;
	HCHAR	*data = NULL;
	APKS_Pair_t 	astAspectRatioPairList[] =
	{
		{	eAspectRatio_4x3,		"4:3"		},
		{	eAspectRatio_16x9,		"16:9"		},
		{	eAspectRatio_14x9,		"14:9"		},
		{	eAspectRatio_Unknown,	"Unknown"	},
		{	eAspectRatio_Unknown,	NULL		}
	};

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	#if 1
	HxSTD_StrCpy(pszOutput, "16:9");
	#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);

	////APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoOutput_M_DisplayFormat,nArg1,nArg2);
	eTVAspectRatio = (APKE_AspectRatio_e)APK_UDB_GetValue((HUINT8 *)USERDB_VIDEO_SCREEN_RATIO);

	RemLOG_Print(" eTVAspectRatio:0x%X\n",eTVAspectRatio);

	data = APK_UTIL_GetStringFromValue(astAspectRatioPairList, eTVAspectRatio);
	if(data == NULL || strcmp(data,"Unknown") == 0)
	{
		RemLOG_Print("APK_UTIL_GetStringFromValue return %s\n",(data?data:"NULL"));
		REM_LEAVE(hErr);
	}
	HxSTD_StrNCpy(pszOutput, data, SIZE_16);
	#endif
*/
	HxSTD_StrCpy(pszOutput, "16:9");//dummy

	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoOutput_M_Macrovision(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoOutput_M_Macrovision,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_VideoOutput_M_HDCP(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_VideoOutput_M_HDCP,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Enabled");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_Status,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "HDMI");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_Name,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_ResolutionMode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Auto");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_ResolutionMode,nArg1,nArg2);
#endif

	RemLOG_Print( "\n[%s,%d] pszOutput : %s \n" , __FUNCTION__, __LINE__, pszOutput );
	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_ResolutionValue(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_ResolutionValue,nArg1,nArg2);
#endif

	RemLOG_Print( "\n[%s,%d] pszOutput : %s \n" , __FUNCTION__, __LINE__, pszOutput );

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_JCOM_CECEnabled(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	HxSTD_StrCpy(pszOutput, "false");
	/*
	APKE_HdmiCecControl_t eHdmiCecControl = eHdmiCecControlDisable;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_SQCSCENARIO_GetHdmiCecMode((APKE_HdmiCecControl_t *)&eHdmiCecControl);
	HxSTD_PrintToStrN(pszOutput, SIZE_16, "%s", (eHdmiCecControl==eHdmiCecControlEnable) ? "true" : "false");
#endif

	RemLOG_Print( "\n[%s,%d] pszOutput : %s \n" , __FUNCTION__, __LINE__, pszOutput );
*/
	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_JCOM_3DOutput(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "true");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_X_JCOM_3DOutput,nArg1,nArg2);
#endif

	RemLOG_Print( "\n[%s,%d] pszOutput : %s \n" , __FUNCTION__, __LINE__, pszOutput );
	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "None");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_Status,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	//HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "HDMIOutput");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_Name,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_EEDID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_EEDID,nArg1,nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_SupportedResolutions(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_SupportedResolutions,nArg1,nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_PreferredResolution(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_PreferredResolution,nArg1,nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_VideoLatency(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_VideoLatency,nArg1,nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_CECSupport(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_CECSupport,nArg1,nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_AutoLipSyncSupport(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_AutoLipSyncSupport,nArg1,nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_Comp_HDMI_M_DisplayDevice_HDMI3DPresent(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_HDMI_M_DisplayDevice_HDMI3DPresent,nArg1,nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_Comp_CA_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Enable");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_CANumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_CA_M_Status,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_CA_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "B-CAS");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_CANumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_CA_M_Name,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_Comp_CA_M_SmartCardReader(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Device.SmartCardReader.1");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_CANumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_Comp_CA_M_SmartCardReader,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_ActiveAVStreams(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_AVS_ActiveAVStreams);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_AVStreamNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 3;
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);

	//rema_DM_MultiInstance_SetEntry(pszOutput,eType);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_AVStream_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Disabled");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVS_AVStream_M_Status,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_AVStream_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVS_AVStream_M_Name,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_AVStream_M_PVRState(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Disabled");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVS_AVStream_M_PVRState,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_AVStream_M_FrontEnd(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVS_AVStream_M_FrontEnd,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_AVStream_M_AudioDecoder(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVS_AVStream_M_AudioDecoder,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_AVStream_M_VideoDecoder(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVS_AVStream_M_VideoDecoder,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_AVStreams_AVStream_M_CA(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVS_AVStream_M_CA,nArg1,nArg2);
#endif

	REM_LEAVE(hErr);

}

//Number of AV players currently active (i.e. with Status != Disabled, which means the AVPlayer is processing an audio or AV stream).

//Todo : If there is no PIP like 4 screens, it has only main view
static HERROR get_AVPlayers_ActiveAVPlayers(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrCpy(pszOutput, "1");

	REM_LEAVE(hErr);
}

//Number of AVPlayer instances.
static HERROR get_AVPlayers_AVPlayerNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
	*pszOutput = 1;

	REM_LEAVE(hErr);
}

//PreferredAudioLanguage
//Human-readable string, as specified in [RFC3066], describing the audio language that SHOULD be used by the STB when receiving a multi-language audio multiplex.
//An empty string indicates that there is no preferred audio language.
static HERROR get_AVPlayers_PreferredAudioLanguage(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 0//def 1
	HxSTD_StrCpy(pszOutput, "en");
#else
	tr135_scenario_GetStrValue(OAPI_SCENARIO_AUDIOLANGUAGE0, pszOutput, 4);
	RemLOG_Print("%s lan(%s)\n",__func__, pszOutput);
#endif
	REM_LEAVE(hErr);
}

/*
Human-readable string, as specified in [RFC3066], describing the subtitling language that SHOULD be used by the STB when receiving multi-language subtitling.
A value of None indicates that no subtitling is required.
An empty string indicates that there is no preferred subtitling language.

*/
static HERROR get_AVPlayers_PreferredSubtitlingLanguage(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 0
	HxSTD_StrCpy(pszOutput, "None");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_AVP_PreferredSubtitlingLanguage);
	tr135_scenario_GetStrValue(OAPI_SCENARIO_SUBTITLELANGUAGE0, pszOutput, 4);
	RemLOG_Print("%s lan(%s)\n",__func__, pszOutput);
#endif
	REM_LEAVE(hErr);
}

/*
Aspect ratio behaviour preference for display on monitor.
If non-empty, the string MUST be one of the .Components.VideoOutput.{i}. AspectRatioBehaviour values.
*/
static HERROR get_AVPlayers_PreferredBehaviour(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	OxOutputVideo_Cache_t	stStatus;
	DxAspectRatio_e			eTvAspectRatio;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 0
	HxSTD_StrCpy(pszOutput, "16:9");
#else
	hErr = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		eTvAspectRatio = stStatus.eTvAspectRatio;

		switch(eTvAspectRatio){
			case eDxASPECTRATIO_UNKNOWN:
				HxSTD_StrCpy(pszOutput, "Unknown");
				break;
			case eDxASPECTRATIO_4X3:
				HxSTD_StrCpy(pszOutput, "4:3");
				break;
			case eDxASPECTRATIO_16X9:
				HxSTD_StrCpy(pszOutput, "16:9");
				break;
			case eDxASPECTRATIO_14X9:
				HxSTD_StrCpy(pszOutput, "14:9");
				break;
			case eDxASPECTRATIO_1X1:
				HxSTD_StrCpy(pszOutput, "1:1");
				break;
		}
	}
	else
	{
		HxSTD_StrCpy(pszOutput, "Unknown");
	}
#endif
	REM_LEAVE(hErr);

}

/*
When set to true, the master PIN code for the STB is set to 0000.
Setting it to false has no effect. The value is not saved in device state and is always false when read.
It is not recommended that this be set to true without the user's permission. No data on the device such as PVR recordings are to be cleared as a result of resetting the PIN code.
*/
static HERROR get_AVPlayers_ResetPINCode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_AVP_ResetPINCode);
#endif
	RemLOG_Print( "\n[%s,%d] pszOutput : %s \n" , __FUNCTION__, __LINE__, pszOutput );

	REM_LEAVE(hErr);

}
static HERROR get_AVPlayers_JCOM_PreferredSuperimposeLanguage(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "ja");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_AVP_X_JCOM_PreferredSuperimposeLanguage);
#endif
	RemLOG_Print( "\n[%s,%d] pszOutput : %s \n" , __FUNCTION__, __LINE__, pszOutput );

	REM_LEAVE(hErr);

}
static HERROR get_AVPlayers_JCOM_Zoom(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_AVP_X_JCOM_Zoom);
#endif
	REM_LEAVE(hErr);

}

//Enables or disables this AV player.
static HERROR get_AVPlayers_AVPlayer_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	OxOutputVideo_Cache_t	stStatus;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 0
	HxSTD_StrCpy(pszOutput, "Disabled");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);
	RemLOG_Print("%s(%d Line) nArg1(%d) , nArg2(%d) \r\n",__func__,__LINE__,nArg1,nArg2);

	//ksjung :
	hErr = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		if(stStatus.bAvEnable == TRUE){
			HxSTD_StrCpy(pszOutput, "true");
		}else{
			HxSTD_StrCpy(pszOutput, "false");
		}
	}
#endif
	REM_LEAVE(hErr);

}

/*
The status of this AV player. Enumeration of:
Disabled
Enabled
Error (OPTIONAL)
If an AV player is disabled then the values of other AV player parameters are not significant.

The Error value MAY be used by the CPE to indicate a locally defined error condition.
*/
static HERROR get_AVPlayers_AVPlayer_M_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	OxOutputVideo_Cache_t	stStatus;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 0
	HxSTD_StrCpy(pszOutput, "Disabled");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);
	RemLOG_Print("%s(%d Line) nArg1(%d) , nArg2(%d) \r\n",__func__,__LINE__,nArg1,nArg2);

	//ksjung :
	hErr = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		if(stStatus.bAvEnable == TRUE){
			HxSTD_StrCpy(pszOutput, "Enabled");
		}else{
			HxSTD_StrCpy(pszOutput, "Disabled");
		}
	}else{
		HxSTD_StrCpy(pszOutput, "Error");
	}

#endif
	REM_LEAVE(hErr);

}

/*
A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to label this instance for future reference.
If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4] and described in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:
Its value MUST NOT be empty.
Its value MUST start with a letter.
If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
The CPE MUST NOT change the parameter value.
*/
static HERROR get_AVPlayers_AVPlayer_M_Alias(HCHAR *pszOutput){
		HERROR hErr = ERR_OK;
		HINT32	nArg1 = 0, nArg2 = 0;

		RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

		CHECK_NULL(pszOutput);

#if 1
		HxSTD_StrCpy(pszOutput, "cpe-AVPlayer");//ksjung : check scenario using this property
#else
		rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
		rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);

		//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVP_AVPlayer_M_Status,nArg1,nArg2);
#endif
		REM_LEAVE(hErr);

}

//Human-readable name associated with this AV player.
static HERROR get_AVPlayers_AVPlayer_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "AVPlayer1");//ksjung : check scenario using this property
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVP_AVPlayer_M_Name,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}

//Human-readable string, as specified in [RFC3066], describing the audio language being currently processed by the AV player.
static HERROR get_AVPlayers_AVPlayer_M_AudioLanguage(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
#if 0
	HxSTD_StrCpy(pszOutput, "en");
#else
//	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
//	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVP_AVPlayer_M_AudioLanguage,nArg1,nArg2);
	hErr = tr135_scenario_GetStrValue(OAPI_SCENARIO_AUDIOLANGUAGE0, pszOutput, 4);
#endif
	REM_LEAVE(hErr);

}
/*
The status of subtitling for this AV player. Enumeration of:
Disabled
Enabled
Error (OPTIONAL)
Subtitles are assumed to be shown only on the main screen.
The Error value MAY be used by the CPE to indicate a locally defined error condition.
*/
static HERROR get_AVPlayers_AVPlayer_M_SubtitlingStatus(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR	buf[4];
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	CHECK_NULL(pszOutput);

#if 0
	HxSTD_StrCpy(pszOutput, "Disabled");
#else
	//rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	//rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVP_AVPlayer_M_SubtitlingStatus,nArg1,nArg2);
	hErr = tr135_scenario_GetStrValue(OAPI_SCENARIO_SUBTITLELANGUAGE0, buf, 4);

	if(hErr==ERR_OK){
		HxSTD_StrCpy(pszOutput, "Enabled");
	}else{
		HxSTD_StrCpy(pszOutput, "Disabled");
	}

#endif
	REM_LEAVE(hErr);
}
static HERROR get_AVPlayers_AVPlayer_M_SubtitlingLanguage(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	CHECK_NULL(pszOutput);
#if 0
	HxSTD_StrCpy(pszOutput, "en");
#else
	hErr = tr135_scenario_GetStrValue(OAPI_SCENARIO_SUBTITLELANGUAGE0, pszOutput, 4);

#endif
	REM_LEAVE(hErr);
}

/*
Comma-separated list (maximum list length 1024) of strings. Each list item MUST be the path name of a row in the Components.AudioOutput table.
If the referenced object is deleted, the corresponding item MUST be removed from the list.
Audio output object instances associated with this AV player, or an empty string if not associated with any audio output object instances. For example:
.Components.AudioOutput.2
*/

//Rocky has only one HDMI Output.
static HERROR get_AVPlayers_AVPlayer_M_AudioOutputs(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Components.AudioOutput.1");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVP_AVPlayer_M_AudioOutputs,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}

/*
Comma-separated list (maximum list length 1024) of strings. Each list item MUST be the path name of a row in the Components.VideoOutput table.
If the referenced object is deleted, the corresponding item MUST be removed from the list.
Video output object instances associated with this AV player, or an empty string if not associated with any video output object instances. For example:
.Components.VideoOutput.2
*/
static HERROR get_AVPlayers_AVPlayer_M_VideoOutputs(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Components.VideoOutput.1");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVP_AVPlayer_M_VideoOutputs,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}

/*
The value MUST be the path name of a row in the AVStreams.AVStream table.
If the referenced object is deleted, the parameter value MUST be set to an empty string.
AV stream object instance associated with this AV player that is being sent to the main screen, or an empty string if not associated with an AV stream object. For example:
.AVStreams.AVStream.2
*/
static HERROR get_AVPlayers_AVPlayer_M_MainStream(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);
		rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);
#if 1
	HxSTD_StrCpy(pszOutput, "AVStreams.AVStream.1");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVP_AVPlayer_M_MainStream,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}

/*
Comma-separated list (maximum list length 1024) of strings.
Each list item MUST be the path name of a row in the AVStreams.AVStream table.
If the referenced object is deleted, the corresponding item MUST be removed from the list.
AV stream object instances associated with this AV player that are being displayed as Picture In Picture (PIP), or an empty string if not associated with any PIP AV stream objects. For example:
.AVStreams.AVStream.2
.AVStreams.AVStream.2,.AVStreams.AVStream.3
*/
static HERROR get_AVPlayers_AVPlayer_M_PIPStreams(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "\0");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_AVP_AVPlayer_M_PIPStreams,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PostalCode( HCHAR *pszPostcode)
{
	HERROR hErr = ERR_OK;
#if 0
	HUINT32 ulLen = 8;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pszPostcode, 0, sizeof(HCHAR)*ulLen);
	APK_SQCSCENARIOJAPAN_GetPostcode(pszPostcode, ulLen);
	pszPostcode[ulLen-1] ='\0';
#else
	HxSTD_MemSet(pszPostcode, 0, sizeof(HCHAR)*1);
	pszPostcode[0] ='\0';
#endif
	RemLOG_Print("pszPostcode(%s)\r\n",pszPostcode);
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_TuningRange(HCHAR *pTuninRange)
{
	HERROR hErr = ERR_OK;
	/*
	HUINT32 tuningRange;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pTuninRange, 0, sizeof(HCHAR)*SIZE_32);

	tuningRange = APK_UDB_GetValue((HUINT8 *)USERDB_JLABS_TUNING_RANGE);
	if(tuningRange == 0)
		HxSTD_StrCpy(pTuninRange, "TV");
	else if(tuningRange == 1)
		HxSTD_StrCpy(pTuninRange, "Radio and Data");
	else
		HxSTD_StrCpy(pTuninRange, "All Media");

	pTuninRange[SIZE_32-1] = '\0';

	RemLOG_Print("pTuninRange(%s)\r\n",pTuninRange);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_URL_Navi( HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
	/*
	HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetNaviURL(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszUrl[ulLen-1] = '\0';

	RemLOG_Print("pszUrl(%d)\r\n",pszUrl);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_URL_InteracTV(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
	/*
	HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetInteracTVURL(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszUrl[ulLen-1] = '\0';

	RemLOG_Print("pszUrl(%d)\r\n",pszUrl);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_URL_VOD(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
	/*
	HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetVodHomeURL(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszUrl[ulLen-1] = '\0';

	RemLOG_Print("pszUrl(%d)\r\n",pszUrl);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_URL_AppDLS(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
	/*
	HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetAppDLSUrl(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszUrl[ulLen-1] = '\0';

	RemLOG_Print("pszUrl(%d)\r\n",pszUrl);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_URL_recommendServer(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
	/*
	HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetRecommendServerUrl(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszUrl[ulLen-1] = '\0';

	RemLOG_Print("pszUrl(%d)\r\n",pszUrl);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_URL_OpenIDServer(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
	/*
	HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetOpenIDServerUrl(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszUrl[ulLen-1] = '\0';

	RemLOG_Print("pszUrl(%d)\r\n",pszUrl);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_URL_EPGServer(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;

	/*
	HUINT32 ulLen = 256;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_MemSet(pszUrl, 0, sizeof(HCHAR)*ulLen);
	hErr = APK_SQCSCENARIOJAPAN_GetEPGServerUrl(pszUrl, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	pszUrl[ulLen-1] = '\0';

	RemLOG_Print("pszUrl(%d)\r\n",pszUrl);
*/
	REM_LEAVE(hErr);

}
#if 0
static HERROR get_JCOM_Config_WebBrowserURLInput(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_StrCpy(pszOutput, "Not Supported");

	REM_LEAVE(hErr);

}
#endif
static HERROR get_JCOM_Config_AutoFirmwareUpdate(HCHAR *pszAutoDownEnable)
{
	HERROR hErr = ERR_OK;
	/*

	HBOOL bAutoSwDown = FALSE;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	APK_SQCSCENARIOJAPAN_GetAutoSWDownloadEnable(&bAutoSwDown);
	HxSTD_PrintToStrN(pszAutoDownEnable,SIZE_16, "%s",bAutoSwDown?"true":"false");

	RemLOG_Print("bAutoSwDown(%d)\r\n",bAutoSwDown);
*/
	REM_LEAVE(hErr);

}

static HERROR get_JCOM_Config_ParentalControl(HCHAR *pszRating)
{
	HERROR hErr = ERR_OK;
/*
	HINT32 ratingValue = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIO_GetMaturityRating(&ratingValue) == ERR_OK)
	{
		if (ratingValue == 0)
		{
			ratingValue = 20; // JCOM 운용사양에 "메뉴에서 무제한을 선택하면 21세로 내려 온다". => JCOM 요구사항 21세를 20세로 표기.
		}
		else if( ratingValue < 0 )
		{
			ratingValue = -1; // Oipf spec상 없는 값은 -1 로 처리한다.
		}
		else
		{
			ratingValue += 3; // JCOM 운용사양에 "메뉴에서 17세를 선택하면 14세로 내려 온다".
		}
		HxSTD_PrintToStrN(pszRating,SIZE_16, "%d", ratingValue);
	}

	RemLOG_Print("ratingValue(%d)\r\n",ratingValue);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PINCodeExpiration(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
/*
	HINT32 Option;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	APK_SQCSCENARIOJAPAN_GetPinUnlockExpireOption((APKE_JLABS_PinUnlockExpirationOption_t*)&Option);

	if(Option == 0)
		HxSTD_StrNCpy(pszOutput, "Power Off", (SIZE_16-1));
	else
		HxSTD_StrNCpy(pszOutput, "Channel Change", (SIZE_16-1));

	RemLOG_Print("PinCodeExpiration(%s)\r\n",pszOutput);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_StandbyMode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	/*

	APKE_JLABS_StandbyMode_t eStandbyMode = eStandbyMode_None;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_GetStandbyMode(&eStandbyMode) != ERR_OK)
	{
		RemLOG_Print("Getting the standbyMode is failed..\n");
		eStandbyMode = eStandbyMode_PowerOFF;
	}

	if(eStandbyMode == eStandbyMode_PowerON)
		HxSTD_StrNCpy(pszOutput, "Normal", (SIZE_16-1));
	else
		HxSTD_StrNCpy(pszOutput, "Eco", (SIZE_16-1));

	RemLOG_Print("eStandbyMode: %s(%d)\r\n",pszOutput,eStandbyMode);
*/
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PowerStatus(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_PowerStatus);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ClientEquipmentNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eTR135A1_DSSTB_1_JCOM_ClientEquipmentNumberOfEntries);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_RemoteEquipmentNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eTR135A1_DSSTB_1_JCOM_RemoteEquipmentNumberOfEntries);

	//rema_DM_MultiInstance_SetEntry(pszOutput,eType);

	REM_LEAVE(hErr);
}
static HERROR get_JCOM_Config_HomeNetwork_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "true");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_HomeNetwork_Enable);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ChannelMasking_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Invalid");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ChannelMasking_Status);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ChannelMasking_ForcedOperation(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HERROR bChMaskForcedOperation = FALSE;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "false");
#else
	APK_SQCSCENARIOJAPAN_GetChannelMaskForcedOperation(&bChMaskForcedOperation);
	HxSTD_PrintToStrN(pszOutput, SIZE_16,"%s",bChMaskForcedOperation?"true":"false");
#endif

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ChannelMasking_Enable(HCHAR *pbChMaskEnable)
{
	HERROR hErr = ERR_OK;
#if 0
	HBOOL bChMasking = FALSE;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	APK_SQCSCENARIOJAPAN_GetChannelMaskEnable(&bChMasking);
	HxSTD_PrintToStrN(pbChMaskEnable,SIZE_16, "%s",bChMasking?"true":"false");
	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR get_JCOM_Config_Internet_ConnectionType(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "DOCSIS",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_Internet_ConnectionTypeUserChange(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HUINT32	value = TRUE;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

//	value = APK_UDB_GetValue( (const HUINT8 *)USERDB_INET_CONNECTION_TYPE_USER_CHANGE );
	if (value == TRUE)
	{
		HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	}

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_Internet_HTTPProxy_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_Internet_HTTPProxy_IPAddress(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_Internet_HTTPProxy_PortNumber(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	pszOutput[0]='\0';

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_Internet_HTTPProxy_UserChange(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_MediaServer_Names(HCHAR *pszFriendlyName)
{
	// 이 함수는 Pioneer 사양에 맞춘 property 를 위한 함수임.
	// Pioneer 는 여러개의 name 을 지원하는 듯함. 우리는 한개만 돌려 줌.
	HERROR hErr = ERR_OK;
#if 0
	HCHAR	szName[UDB_TEXT_LEN] = {0,};
	HUINT32 ulLen = UDB_TEXT_LEN;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	hErr = APK_DLNADMS_GetDmsDbFriendlyName(szName, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	HxSTD_StrNCpy(pszFriendlyName,szName,UDB_TEXT_LEN);

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif

}
static HERROR get_JCOM_Config_MediaServer_Name(HCHAR *pszFriendlyName)
{
	HERROR hErr = ERR_OK;
#if 0
	HCHAR	szName[UDB_TEXT_LEN] = {0,};
	HUINT32 ulLen = UDB_TEXT_LEN;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	hErr = APK_DLNADMS_GetDmsDbFriendlyName(szName, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	HxSTD_StrNCpy(pszFriendlyName,szName,UDB_TEXT_LEN);

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif

}
static HERROR get_JCOM_Config_MediaServer_Clients(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 0//def TEMP_DEFAULT
	HxSTD_StrCpy(pszOutput, "");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_MediaServer_Clients);
#endif

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PurchasedPPV_TotalPriceOfThisMonth(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_PurchasedPPV_TotalPriceOfThisMonth);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PurchasedPPV_TotalPriceOfPreviousMonth(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_PurchasedPPV_TotalPriceOfPreviousMonth);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PurchasedPPV_HistoryNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
#if 0
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_JCOM_PurchasedPPV_HistoryNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);

	//rema_DM_MultiInstance_SetEntry(pszOutput,eType);
#endif

	REM_LEAVE(hErr);
#else
	*pszOutput = 0;
	REM_LEAVE(hErr);
#endif


}
static HERROR get_JCOM_Config_PurchasedPPV_History_M_IsdbId(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_PurchasedPPV_HistoryNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_IsdbId,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PurchasedPPV_History_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_PurchasedPPV_HistoryNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_Name,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PurchasedPPV_History_M_Date(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0001-01-01T00:00:00Z");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_PurchasedPPV_HistoryNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_Date,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_PurchasedPPV_History_M_Price(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_PurchasedPPV_HistoryNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_Price,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_ServiceListDatabaseNumberOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	eTRXDMInstancetype_e eType = eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 0;
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Entry(pszOutput,eType);

	//rema_DM_MultiInstance_SetEntry(pszOutput,eType);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Start(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Start);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Status(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "Disabled");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Status);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Progress(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Progress);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_AreaCode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "23");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_AreaCode);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Frequency_DTT(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_DTT);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Frequency_BS(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_BS);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Frequency_CS1(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CS1);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Frequency_CS2(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CS2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Frequency_CATV(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CATV);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_Install_Frequency_JC_HITS(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_JC_HITS);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_ServiceListDatabase_M_NetworkId(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_NetworkId,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_ServiceListDatabase_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_Name,nArg1,nArg2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_ServiceListDatabase_M_LCNOfEntries(HINT32 *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	*pszOutput = 1;
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2_Entry(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LCNOfEntries, nArg1, nArg2);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_LCN(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0, nArg3 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LCNOfEntries,&nArg3);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D3(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_LCN, nArg1, nArg2, nArg3);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0, nArg3 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LCNOfEntries,&nArg3);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D3(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Name, nArg1, nArg2, nArg3);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_IsdbId(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0, nArg3 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LCNOfEntries,&nArg3);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D3(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_IsdbId, nArg1, nArg2, nArg3);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Frequency(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0, nArg3 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries,&nArg2);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LCNOfEntries,&nArg3);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D3(pszOutput,eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Frequency, nArg1, nArg2, nArg3);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_USB_HDD_PictureQuarity(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_USB_HDD_PictureQuarity);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_USB_HDD_Selected(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_String(pszOutput,eTR135A1_DSSTB_1_JCOM_USB_HDD_Selected);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_USB_HDD_AutoChapter(HCHAR *pszOutput)
{
	HERROR						hErr = ERR_OK;
//	APKE_JLABS_PvrAutoChapter_t mode;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1
	HxSTD_StrCpy(pszOutput, "0");
#else
	APK_SQCSCENARIOJAPAN_GetPvrAutoChapter(&mode);
	HxSTD_snprintf(pszOutput, 15, "%d", mode);
#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_LAN_HDD_ServerName(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szHandle[257] = {0,};
	static char strTemp[257];
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else
	HxSTD_MemSet(strTemp, 0x0, sizeof(strTemp));
	hErr = APK_SQCSCENARIOJAPAN_GetDefaultDMSDeviceHandle(strTemp, 257);
	if( hErr != ERR_OK)
	{
		pszOutput[0]='\0';
		REM_LEAVE(hErr);
	}
	if(HxSTD_StrLen(strTemp) != 0)
	{
		HxSTD_StrNCpy(szHandle,strTemp,(SIZE_128-1));

		HxLog_Debug("szHandle(%s)\n",szHandle);

		//APK_3RDPARTY_CWMP_TR135_Get_LAN_HDD(pszOutput,eTR135A1_DSSTB_1_JCOM_LAN_HDD_ServerName,szHandle);
	}
	else
	{
		pszOutput[0]='\0';
	}

#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_LAN_HDD_RecordingPlace(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HCHAR szHandle[257] = {0,};
	static char strTemp[257];
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

#if 1

#else

	pszOutput[0]='\0';

	hErr = APK_SQCSCENARIOJAPAN_GetDefaultDMSDeviceHandle(strTemp, 257);
	if( hErr != ERR_OK)
	{
		pszOutput[0]='\0';
		REM_LEAVE(hErr);
	}
	if(HxSTD_StrLen(strTemp) != 0)
	{
		HxSTD_StrNCpy(szHandle,strTemp,(SIZE_128-1));

		RemLOG_Print("szHandle(%s)\n",szHandle);

		hErr = APK_DLNADMP_GetRecordDestinations((const char*)&szHandle);
		if( hErr != ERR_OK)
		{
			pszOutput[0]='\0';
			REM_LEAVE(hErr);
		}

		//APK_3RDPARTY_CWMP_TR135_Get_LAN_HDD(pszOutput,eTR135A1_DSSTB_1_JCOM_LAN_HDD_RecordingPlace,szHandle);
	}
	else
	{
		RemLOG_Print("Fail to get Default DMS Device Handle(%s)\n",strTemp);
		pszOutput[0]='\0';
	}

#endif
	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_HomeServer_Enable(HCHAR *pszOutput)
{
#if 0
	HERROR	hErr = ERR_OK;
	HBOOL	bEnabled = FALSE;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	APK_DLNADMS_GetDmsDbEnabled(&bEnabled);
	if (bEnabled == TRUE)
	{
		HxSTD_StrNCpy(pszOutput, "true",(SIZE_16-1));
	}
	else
	{
		HxSTD_StrNCpy(pszOutput, "false",(SIZE_16-1));
	}

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif

}
static HERROR get_JCOM_Config_HomeServer_Name(HCHAR *pszFriendlyName)
{
	// Media server 의 name 과 동일한 property
	HERROR hErr = ERR_OK;
#if 0
	HCHAR	szName[UDB_TEXT_LEN] = {0,};
	HUINT32 ulLen = UDB_TEXT_LEN;

	hErr = APK_DLNADMS_GetDmsDbFriendlyName(szName, ulLen);
	if (ERR_OK != hErr)
	{
		REM_LEAVE(hErr);
	}
	HxSTD_StrNCpy(pszFriendlyName,szName,UDB_TEXT_LEN);

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif

}
static HERROR get_JCOM_Config_ClientEquipment_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ClientEquipmentNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_ClientEquipment_M_Enable, nArg1, nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_ClientEquipment_M_MACAddress(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_ClientEquipmentNumberOfEntries,&nArg2);

	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_ClientEquipment_M_MACAddress, nArg1, nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_RemoteEquipment_M_Enable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_RemoteEquipmentNumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_RemoteEquipment_M_Enable, nArg1, nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_RemoteEquipment_M_Name(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_RemoteEquipmentNumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_RemoteEquipment_M_Name, nArg1, nArg2);

	REM_LEAVE(hErr);

}
static HERROR get_JCOM_Config_RemoteEquipment_M_Nickname(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 nArg1 = 0, nArg2 = 0;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	CHECK_NULL(pszOutput);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_JCOM_RemoteEquipmentNumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Get_CommonDM_Multi_D2(pszOutput,eTR135A1_DSSTB_1_JCOM_RemoteEquipment_M_Nickname, nArg1, nArg2);

	REM_LEAVE(hErr);

}

#define ______________________STATIC_FUNCTION_TRP_SET______________________

static HERROR set_JCOM_Config_PostalCode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if 0
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_SetPostcode(pszOutput) != ERR_OK)
	{
		RemLOG_Print("Get the postcode is failed..\n");
	}

	RemLOG_Print("pszOutput(%s)\r\n",pszOutput);


	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_JCOM_Config_TuningRange(HCHAR *pszRange)
{
	HINT32 mode = 0;
	HERROR hErr = ERR_OK;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(HxSTD_StrCmp(pszRange, "TV")  == 0)
		mode = 0;
	else if(HxSTD_StrCmp(pszRange,  "Radio and Data")  == 0)
		mode = 1;
	else
		mode = 2;

//	APK_UDB_SetValue((HUINT8 *)USERDB_JLABS_TUNING_RANGE, mode);

	RemLOG_Print("mode(%d)\r\n",mode);

	REM_LEAVE(hErr);

}
static HERROR set_JCOM_Config_URL_Navi(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
#if 0
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_SetNaviURL(pszUrl) != ERR_OK)
	{
		RemLOG_Print("Setting the  naviUrl is failed..\n");
	}

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_JCOM_Config_URL_InteracTV(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
#if 0
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_SetInteracTVURL(pszUrl) != ERR_OK)
	{
		RemLOG_Print("Setting the  InteracTVURL is failed..\n");
	}

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_JCOM_Config_URL_VOD(HCHAR *pszUrl)
{
	HERROR hErr = ERR_OK;
#if 0
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_SetVodHomeURL(pszUrl) != ERR_OK)
	{
		RemLOG_Print("Setting the  VodHomeUrl is failed..\n");
	}

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_JCOM_Config_URL_AppDLS(HCHAR *pszUrl)
{
#if 0
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_SetAppDLSUrl(pszUrl) != ERR_OK)
	{
		RemLOG_Print("Setting the  AppDLS Url is failed..\n");
	}

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_JCOM_Config_URL_recommendServer(HCHAR *pszUrl)
{
#if 0

	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_SetRecommendServerUrl(pszUrl) != ERR_OK)
	{
		RemLOG_Print("Setting the  recommendServerUrl is failed..\n");
	}

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif
}
static HERROR set_JCOM_Config_URL_OpenIDServer(HCHAR *pszUrl)
{
#if 0
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_SetOpenIDServerUrl(pszUrl) != ERR_OK)
	{
		RemLOG_Print("Setting the  OpenIDServerUrl is failed..\n");
	}

	REM_LEAVE(hErr);
#else
		REM_LEAVE(ERR_FAIL);
#endif

}
static HERROR set_JCOM_Config_URL_EPGServer(HCHAR *pszUrl)
{
#if 0
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(APK_SQCSCENARIOJAPAN_SetEPGServerUrl(pszUrl) != ERR_OK)
	{
		RemLOG_Print("Setting the  EPGServerUrl is failed..\n");
	}

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif

}

static HERROR set_JCOM_Config_AutoFirmwareUpdate(HCHAR *pszAutoDownEnable)
{
	HERROR hErr = ERR_OK;
#if 0
	HBOOL bAutoDownEnable = FALSE;

	if(HxSTD_StrCmp(pszAutoDownEnable, "true") == 0)
		bAutoDownEnable = TRUE;
	else
		bAutoDownEnable = FALSE;

	APK_SQCSCENARIOJAPAN_SetAutoSWDownloadEnable(bAutoDownEnable);

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif

}
static HERROR set_JCOM_Config_ParentalControl(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
#if 0
	HINT32 ratingValue = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	ratingValue = PARAM_ATOI(pszInput);

	if (ratingValue == 21)
	{
		ratingValue = 0; // JCOM 운용사양에 "메뉴에서 무제한을 선택하면 21세로 내려 온다".
	}
	else
	{
		ratingValue -= 3; // JCOM 운용사양에 "메뉴에서 17세를 선택하면 14세로 내려 온다".
	}
	APK_SQCSCENARIO_SetMaturityRating((int)ratingValue);

	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif


}
static HERROR set_JCOM_Config_PINCodeExpiration(HCHAR *pszPin)
{
	HERROR hErr = ERR_OK;
/*
	HINT32 Option = ePinUnlockOption_PowrOff;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(HxSTD_StrCmp(pszPin, "Power Off")  == 0)
		Option = 0;
	else if(HxSTD_StrCmp(pszPin,  "Channel Change")  == 0)
		Option = 1;

	APK_SQCSCENARIOJAPAN_SetPinUnlockExpireOption((APKE_JLABS_PinUnlockExpirationOption_t)Option);

	RemLOG_Print("Option(%d)\r\n",Option);
*/
	REM_LEAVE(hErr);

}
static HERROR set_JCOM_Config_StandbyMode(HCHAR *pszStandbyMode)
{
	HERROR hErr = ERR_OK;
/*
	APKE_JLABS_StandbyMode_t eStandbyMode = eStandbyMode_None;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(HxSTD_StrCmp(pszStandbyMode, "Normal")  == 0)
		eStandbyMode = eStandbyMode_PowerON;
	else
		eStandbyMode = eStandbyMode_PowerOFF;

	RemLOG_Print("eStandbyMode: %s(%d)\r\n",pszStandbyMode,eStandbyMode);

	APK_SQCSCENARIOJAPAN_SetStandbyMode(eStandbyMode);
*/
	REM_LEAVE(hErr);

}

static HERROR set_JCOM_Config_PowerStatus(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_PowerStatus);

	REM_LEAVE(hErr);

}

static HERROR set_JCOM_Config_ChannelMasking_ForcedOperation(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	/*
	HBOOL	bChMaskForcedOperation = FALSE;
	HUINT32	ulChMaskPattern = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	hErr = APK_SQCSCENARIOJAPAN_GetChannelMaskPattern(&ulChMaskPattern);

	RemLOG_Print("ulChMaskPattern %d, hErr %d\n", ulChMaskPattern,hErr);
	//pattern이 unavailable인 경우에만 설정 가능
	if((hErr != ERR_OK) || (ulChMaskPattern != 0))
	{
		RemLOG_Print("ForcedOperation can be set when status is unavailable!! \n");
		REM_LEAVE(hErr);
	}

	if(HxSTD_StrCmp(pszInput, "true") == 0)
		bChMaskForcedOperation = TRUE;
	else
		bChMaskForcedOperation = FALSE;

	hErr = APK_SQCSCENARIOJAPAN_SetChannelMaskForcedOperation(bChMaskForcedOperation);
*/

	REM_LEAVE(hErr);

}
static HERROR set_JCOM_Config_ChannelMasking_Enable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	/*
	HBOOL	bChMaskEnable = FALSE;
	HBOOL	bChMaskForcedOperation = FALSE;
	HUINT32	ulChMaskPattern = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	hErr |= APK_SQCSCENARIOJAPAN_GetChannelMaskPattern(&ulChMaskPattern);
	hErr |= APK_SQCSCENARIOJAPAN_GetChannelMaskForcedOperation(&bChMaskForcedOperation);
	if(hErr != ERR_OK)
	{
		RemLOG_Print("APK_SQCSCENARIOJAPAN_GetChannelMaskPattern or  APK_SQCSCENARIOJAPAN_GetChannelMaskForcedOperation returned err\n");
		REM_LEAVE(hErr);
	}
	RemLOG_Print("ulChMaskPattern %d, bChMaskForcedOperation %d, pstInfo->nDMDataTypeBool  %d, hErr %d\n", ulChMaskPattern,bChMaskForcedOperation, hErr);

	//pattern이 ailable 또는unavailable이면서 forcedOperation이 ture 인 경우에만 설정 가능
	if(((ulChMaskPattern == 1) || (ulChMaskPattern == 2)) //available
	||((ulChMaskPattern == 0) && ( bChMaskForcedOperation == TRUE))) //unavailable & forcedOperation

	{
		if(HxSTD_StrCmp(pszInput, "true") == 0)
			bChMaskEnable = TRUE;
		else
			bChMaskEnable = FALSE;

		hErr = APK_SQCSCENARIOJAPAN_SetChannelMaskEnable(bChMaskEnable);
	}
	else
	{
		hErr = ERR_FAIL;
	}
*/

	REM_LEAVE(hErr);

}
static HERROR set_JCOM_Config_MediaServer_Name(HCHAR *pszFriendlyName)
{
	HERROR hErr = ERR_OK;
#if 0
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	APK_DLNADMS_SetFriendlyName(pszFriendlyName);
	REM_LEAVE(hErr);
#else
	REM_LEAVE(ERR_FAIL);
#endif



}
static HERROR set_Comp_AudioOutput_M_AudioLevel(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Set_D2(pszInput,nArg1,nArg2,eTR135A1_DSSTB_1_Comp_AudioOutput_M_AudioLevel);

	REM_LEAVE(hErr);

}
static HERROR set_Comp_AudioOutput_M_CancelMute(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Set_D2(pszInput,nArg1,nArg2,eTR135A1_DSSTB_1_Comp_AudioOutput_M_CancelMute);

	REM_LEAVE(hErr);
}
static HERROR set_Comp_SPDIF_M_ForcePCM(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	/*
	DxAudioDigitalOut_e eSpdifOut = eDxAUDIO_DIGITALOUT_UNKNOWN;
	APKS_Pair_t             _DigitalOutList[] =
	{
		{       eDxAUDIO_DIGITALOUT_DOWNMIX,    "PCM"                           },
		{       eDxAUDIO_DIGITALOUT_MULTICHANNEL,       "Multi Channel"         },
		{       eDxAUDIO_DIGITALOUT_BYPASS_AAC, "Bypass AAC"                    },
		{       eDxAUDIO_DIGITALOUT_BYPASS_AC3, "Bypass AC3"                    },
		{       eDxAUDIO_DIGITALOUT_UNKNOWN,            NULL                    }
	};

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if(HxSTD_StrCmp(pszInput, "true") == 0)
	{
		eSpdifOut = APK_UTIL_GetValueFromString(_DigitalOutList, "PCM");
	}
	else
	{
		eSpdifOut = APK_UTIL_GetValueFromString(_DigitalOutList, "Bypass AAC");
	}

	if (APK_SQCAUDIO_SetSpdifAudioOut(eSpdifOut) != ERR_OK)
	{
		RemLOG_Print("Setting SpdifAudioOut is failed.\n");
		REM_LEAVE(hErr);
	}

	APK_UDB_SetValue((const HUINT8 *)USERDB_AUDIO_SPDIF_AUDIO_OUT, (HUINT32)eSpdifOut);
*/
	REM_LEAVE(hErr);
}
static HERROR set_Comp_VideoOutput_M_VideoFormat(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Set_D2(pszInput,nArg1,nArg2,eTR135A1_DSSTB_1_Comp_VideoOutput_M_VideoFormat);

	REM_LEAVE(hErr);
}
static HERROR set_Comp_VideoOutput_M_DisplayFormat(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	HINT32	nArg1 = 0, nArg2 = 0;
	/*
	APKE_AspectRatio_e	eTVAspectRatio;
	APKS_Pair_t 	astAspectRatioPairList[] =
	{
		{	eAspectRatio_4x3,		"4:3"		},
		{	eAspectRatio_16x9,		"16:9"		},
		{	eAspectRatio_14x9,		"14:9"		},
		{	eAspectRatio_Unknown,	"Unknown"	},
		{	eAspectRatio_Unknown,	NULL		}
	};
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries,&nArg2);
	////APK_3RDPARTY_CWMP_TR135_Set_D2(pszInput,nArg1,nArg2,eTR135A1_DSSTB_1_Comp_VideoOutput_M_DisplayFormat);

	eTVAspectRatio = APK_UTIL_GetValueFromString(astAspectRatioPairList, pszInput);

	RemLOG_Print(" pszInput:%s eTVAspectRatio:0x%X\n",pszInput,eTVAspectRatio);

	if(eTVAspectRatio == eAspectRatio_Unknown)
	{
		RemLOG_Print("eTVAspectRatio eAspectRatio_Unknown\n");
		REM_LEAVE(hErr);
	}
	hErr = APK_SQCOUTPUT_SetTvAspectRatio(eTVAspectRatio);
	if (ERR_OK != hErr)
	{
		RemLOG_Print("APK_SQCOUTPUT_SetTvAspectRatio ret Error:0x%X.\n",hErr);
		REM_LEAVE(hErr);
	}

	APK_UDB_SetValue((const HUINT8 *)USERDB_VIDEO_SCREEN_RATIO, eTVAspectRatio);
*/
	REM_LEAVE(hErr);
}
static HERROR set_Comp_HDMI_M_ResolutionMode(HCHAR *pszInput)
{
	HERROR 				hErr = ERR_OK;
	/*
	HINT32				nArg1 = 0, nArg2 = 0;
	APKE_ResolutionSelect_e	eResolutionSelect =eRESSELECT_UNKNOWN;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print(" \n\n #### [%s,%d] value : %s #### \n\n " , __FUNCTION__, __LINE__, pszInput );

	if( HxSTD_StrCmp( pszInput, "Auto"  ))
	{
		eResolutionSelect = eRESSELECT_EDID_MAX;
	}
	else if( HxSTD_StrCmp( pszInput, "Manual"  ))
	{
		eResolutionSelect = eRESSELECT_1920x1080I;
	}
	else
	{
		RemLOG_Print("%s(%d Line)invalid string : %s \r\n",__func__,__LINE__, pszInput);
		REM_LEAVE(ERR_FAIL);
	}

	APK_UDB_SetValue((const HUINT8 *)USERDB_VIDEO_RESOLUTION, eResolutionSelect);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Set_D2(pszInput,nArg1,nArg2,eTR135A1_DSSTB_1_Comp_HDMI_M_ResolutionMode);
*/
	REM_LEAVE(hErr);
}
static HERROR set_Comp_HDMI_M_ResolutionValue(HCHAR *pszInput)
{
	HERROR 				hErr = ERR_OK;
	/*
	HINT32				nArg1 = 0, nArg2 = 0;
	APKE_ResolutionSelect_e	eResolutionSelect =eRESSELECT_UNKNOWN;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print(" \n\n #### [%s,%d] value : %s #### \n\n " , __FUNCTION__, __LINE__, pszInput );

	if( HxSTD_StrCmp( pszInput, "720(1440)x480i/60Hz" ))
	{
		eResolutionSelect = eRESSELECT_720x480I;
	}
	else if( HxSTD_StrCmp( pszInput, "720x480p/60Hz"  ))
	{
		eResolutionSelect = eRESSELECT_720x480P;
	}
	else if( HxSTD_StrCmp( pszInput, "1280x720p/60Hz"  ))
	{
		eResolutionSelect = eRESSELECT_1080x720P;
	}
	else if( HxSTD_StrCmp( pszInput, "1920x1080i/60Hz"  ))
	{
		eResolutionSelect = eRESSELECT_1920x1080P;
	}
	else if( HxSTD_StrCmp( pszInput, "1920x1080p/60Hz"  ))
	{
		eResolutionSelect = eRESSELECT_1920x1080I;
	}
	else
	{
		RemLOG_Print("%s(%d Line)invalid string : %s \r\n",__func__,__LINE__, pszInput);
		REM_LEAVE(ERR_FAIL);
	}

	APK_UDB_SetValue((const HUINT8 *)USERDB_VIDEO_RESOLUTION, eResolutionSelect);

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Set_D2(pszInput,nArg1,nArg2,eTR135A1_DSSTB_1_Comp_HDMI_M_ResolutionValue);
*/
	REM_LEAVE(hErr);
}
static HERROR set_Comp_HDMI_M_JCOM_CECEnabled(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	/*
	APKE_HdmiCecControl_t eHdmiCecControl = eHdmiCecControlDisable;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print(" \n\n #### [%s,%d] value : %s #### \n\n " , __FUNCTION__, __LINE__, pszInput );

	if(HxSTD_StrCmp(pszInput, "true") == 0)
		eHdmiCecControl = eHdmiCecControlEnable;
	else
		eHdmiCecControl = eHdmiCecControlDisable;

	APK_SQCSCENARIO_SetHdmiCecMode(eHdmiCecControl);
*/
	REM_LEAVE(hErr);
}
static HERROR set_Comp_HDMI_M_JCOM_3DOutput(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	/*
	HINT32	nArg1 = 0, nArg2 = 0;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print(" \n\n #### [%s,%d] value : %s #### \n\n " , __FUNCTION__, __LINE__, pszInput );

	rema_DM_MultiInstance_GetEntryCount(eTR135A1_Device_Services_STBServiceNumberOfEntries,&nArg1);
	rema_DM_MultiInstance_GetEntryCount(eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries,&nArg2);
	//APK_3RDPARTY_CWMP_TR135_Set_D2(pszInput,nArg1,nArg2,eTR135A1_DSSTB_1_Comp_HDMI_M_X_JCOM_3DOutput);
*/
	REM_LEAVE(hErr);
}
static HERROR set_AVPlayers_PreferredAudioLanguage(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	/*
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	if (APK_SQCSCENARIO_SetAudioLanguage(pszInput, 0) != ERR_OK)
	{
		RemLOG_Print("%s set AudioLanguage fail \n",__func__);
	}
	////APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_AVP_PreferredAudioLanguage);
*/
	REM_LEAVE(hErr);
}
static HERROR set_AVPlayers_PreferredSubtitlingLanguage(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_AVP_PreferredSubtitlingLanguage);

	REM_LEAVE(hErr);
}
static HERROR set_AVPlayers_PreferredBehaviour(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	/*
	APKE_AspectRatio_e	eTVAspectRatio;

	if(HxSTD_StrCmp(pszInput, "4:3") == 0)
	{
		eTVAspectRatio = eAspectRatio_4x3;
	}
	else if(HxSTD_StrCmp(pszInput, "16:9") == 0)
	{
		eTVAspectRatio = eAspectRatio_16x9;
	}
	else
	{
		RemLOG_Print("[%s:%d] The Input Value is not supported Screen Aspect Ratio\n", __FUNCTION__, __LINE__);
		eTVAspectRatio = eAspectRatio_16x9;
	}

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	APK_UDB_SetValue((const HUINT8 *)USERDB_VIDEO_SCREEN_RATIO, eTVAspectRatio);
	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_AVP_PreferredBehaviour);

	REM_LEAVE(hErr);
	RemLOG_Print("Not supported\r\n");
	*/
	return ERR_OK;
}
static HERROR set_AVPlayers_ResetPINCode(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print(" \n\n #### [%s,%d] value : %s #### \n\n " , __FUNCTION__, __LINE__, pszInput );

    //구현시 APK_SCENARIO_PINCODE_Reset 이용

	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_AVP_ResetPINCode);

	REM_LEAVE(hErr);
}
static HERROR set_AVPlayers_JCOM_PreferredSuperimposeLanguage(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
	RemLOG_Print(" \n\n #### [%s,%d] value : %s #### \n\n " , __FUNCTION__, __LINE__, pszInput );

	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_AVP_X_JCOM_PreferredSuperimposeLanguage);

	REM_LEAVE(hErr);
}
static HERROR set_AVPlayers_JCOM_Zoom(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_AVP_X_JCOM_Zoom);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_Internet_ConnectionType(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	RemLOG_Print("Don't support!\r\n");

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_Internet_ConnectionTypeUserChange(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);
/*
	// 정확히 어디에 사용할지 모른다.
	// 하지만 set 한 값을 get 해주기 위해 udb 에 일단 저장한다.
	// 그리고 의미가 대충 network setting 을 사용자에게 허락 할 것인가? 아닌가?
	// 인것 같은데 이 처리는 OPL 밑 각 process 단에서 user setting 이 내려 올때
	// 이것을 보고 처리 할 수 있을 것 같다.
	if(HxSTD_StrNCmp(pszInput, "true", 4) == 0)
	{
		APK_UDB_SetValue( (const HUINT8 *)USERDB_INET_CONNECTION_TYPE_USER_CHANGE, (HUINT32)TRUE );
	}
	else
	{
		APK_UDB_SetValue( (const HUINT8 *)USERDB_INET_CONNECTION_TYPE_USER_CHANGE, (HUINT32)FALSE );
	}
*/
	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_Internet_HTTPProxy_Enable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	RemLOG_Print("Not Supported!\r\n");

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_Internet_HTTPProxy_IPAddress(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	RemLOG_Print("Not Supported!\r\n");

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_Internet_HTTPProxy_PortNumber(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	RemLOG_Print("Not Supported!\r\n");

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_Internet_HTTPProxy_UserChange(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	RemLOG_Print("Not Supported!\r\n");

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_ISDBC_Install_Start(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Start);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_ISDBC_Install_AreaCode(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_AreaCode);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_ISDBC_Install_Frequency_DTT(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_DTT);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_ISDBC_Install_Frequency_BS(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_BS);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_ISDBC_Install_Frequency_CS1(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CS1);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_ISDBC_Install_Frequency_CS2(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CS2);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_ISDBC_Install_Frequency_CATV(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_CATV);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_ISDBC_Install_Frequency_JC_HITS(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_ISDBC_Install_Frequency_JC_HITS);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_USB_HDD_PictureQuarity(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_USB_HDD_PictureQuarity);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_USB_HDD_Selected(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);


	//APK_3RDPARTY_CWMP_TR135_Set(pszInput,eTR135A1_DSSTB_1_JCOM_USB_HDD_Selected);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_USB_HDD_AutoChapter(HCHAR *pszInput)
{
	HERROR		hErr = ERR_OK;
	HUINT32		mode;

	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	mode = HLIB_STD_StrToUINT32(pszInput);
//	APK_SQCSCENARIOJAPAN_SetPvrAutoChapter(mode);

	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_HomeServer_Enable(HCHAR *pszInput)
{
	HCHAR szFriendlyName[128];
	HERROR hErr = ERR_OK;
/*
	if(HxSTD_StrNCmp(pszInput, "true", 4) == 0)
	{
		HxSTD_MemSet(szFriendlyName, 0, 128);
		hErr = APK_DLNADMS_GetDmsDbFriendlyName(szFriendlyName, 128);
		APK_DLNADMS_Start(szFriendlyName);
	}
	else
	{
		APK_DLNADMS_Stop();
	}
*/
	REM_LEAVE(hErr);
}
static HERROR set_JCOM_Config_HomeServer_Name(HCHAR *pszFriendlyName)
{
	HERROR hErr = ERR_OK;
	/*
	RemLOG_Print("%s(%d Line)\r\n",__func__,__LINE__);

	APK_DLNADMS_SetFriendlyName(pszFriendlyName);
*/
	REM_LEAVE(hErr);
}





#define ______________________PUBLIC_FUNCTION_TRP_GET______________________
HERROR TRP135_Get_Service_STBServiceNumberOfEntries(HCHAR *pszOutput)
{
	return get_Service_STBServiceNumberOfEntries(pszOutput);
}

HERROR TRP135_Get_Capa_PVR_MaxIOStreams(HINT32 *pnNum)
{
	return get_Capa_PVR_MaxIOStreams(pnNum);
}
HERROR TRP135_Get_Capa_PVR_MaxRecordingStreams(HCHAR *pszOutput)
{
	return get_Capa_PVR_MaxRecordingStreams(pszOutput);
}
HERROR TRP135_Get_Capa_PVR_MaxPlaybackStreams(HCHAR *pszOutput)
{
	return get_Capa_PVR_MaxPlaybackStreams(pszOutput);
}
HERROR TRP135_Get_Capa_PVR_MaxTimeDelay(HCHAR *pszOutput)
{
	HxSTD_StrCpy(pszOutput,"0");
	return ERR_OK;
}

HERROR TRP135_Get_Capa_AudioDecoder_AudioStandards(HCHAR *pszOutput)
{
	return get_Capa_AudioDecoder_AudioStandards(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_Profile(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP2P2_ProfileLevel_M_Profile(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_Level(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP2P2_ProfileLevel_M_Level(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP2P2_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP2P2_ProfileLevel_M_MaximumDecodingCapability(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP2P2_AudioStandards(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP2P2_AudioStandards(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP2P2_ProfileLevelNumberOfEntries(HINT32 *pszOutput)
{
	return get_Capa_VDec_MP2P2_ProfileLevelNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_Profile(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP4P2_ProfileLevel_M_Profile(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_Level(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP4P2_ProfileLevel_M_Level(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P2_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP4P2_ProfileLevel_M_MaximumDecodingCapability(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P2_AudioStandards(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP4P2_AudioStandards(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P2_ProfileLevelNumberOfEntries(HINT32 *pszOutput)
{
	return get_Capa_VDec_MP4P2_ProfileLevelNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_Profile(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP4P10_ProfileLevel_M_Profile(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_Level(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP4P10_ProfileLevel_M_Level(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P10_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP4P10_ProfileLevel_M_MaximumDecodingCapability(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P10_AudioStandards(HCHAR *pszOutput)
{
	return get_Capa_VDec_MP4P10_AudioStandards(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_MP4P10_ProfileLevelNumberOfEntries(HINT32 *pszOutput)
{
	return get_Capa_VDec_MP4P10_ProfileLevelNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Profile(HCHAR *pszOutput)
{
	return get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Profile(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Level(HCHAR *pszOutput)
{
	return get_Capa_VDec_SMPTEVC1_ProfileLevel_M_Level(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevel_M_MaximumDecodingCapability(HCHAR *pszOutput)
{
	return get_Capa_VDec_SMPTEVC1_ProfileLevel_M_MaximumDecodingCapability(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_SMPTEVC1_AudioStandards(HCHAR *pszOutput)
{
	return get_Capa_VDec_SMPTEVC1_AudioStandards(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_SMPTEVC1_ProfileLevelNumberOfEntries(HINT32 *pszOutput)
{
	return get_Capa_VDec_SMPTEVC1_ProfileLevelNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Capa_VDec_VideoStandards(HCHAR *pszOutput)
{
	return get_Capa_VDec_VideoStandards(pszOutput);
}
HERROR TRP135_Get_Capa_AOutput_AudioFormats(HCHAR *pszOutput)
{
	return get_Capa_AOutput_AudioFormats(pszOutput);
}
HERROR TRP135_Get_Capa_VOutput_CompositeVideoStandards(HCHAR *pszOutput)
{
	return get_Capa_VOutput_CompositeVideoStandards(pszOutput);
}
HERROR TRP135_Get_Capa_VOutput_VideoFormats(HCHAR *pszOutput)
{
	return get_Capa_VOutput_VideoFormats(pszOutput);
}
HERROR TRP135_Get_Capa_VOutput_Macrovision(HCHAR *pszOutput)
{
	return get_Capa_VOutput_Macrovision(pszOutput);
}
HERROR TRP135_Get_Capa_VOutput_HDCP(HCHAR *pszOutput)
{
	return get_Capa_VOutput_HDCP(pszOutput);
}
HERROR TRP135_Get_Capa_VOutput_DisplayFormats(HCHAR *pszOutput)
{
	return get_Capa_VOutput_DisplayFormats(pszOutput);
}
HERROR TRP135_Get_Capa_HDMI_SupportedResolutions(HCHAR *pszOutput)
{
	return get_Capa_HDMI_SupportedResolutions(pszOutput);
}
HERROR TRP135_Get_Capa_HDMI_CECSupport(HCHAR *pszOutput)
{
	return get_Capa_HDMI_CECSupport(pszOutput);
}
HERROR TRP135_Get_Capa_HDMI_HDMI3D(HCHAR *pszOutput)
{
	return get_Capa_HDMI_HDMI3D(pszOutput);
}
HERROR TRP135_Get_Capa_CA_CASystems(HCHAR *pszOutput)
{
	return get_Capa_CA_CASystems(pszOutput);
}
HERROR TRP135_Get_Capa_DRM_DRMSystems(HCHAR *pszOutput)
{
	return get_Capa_DRM_DRMSystems(pszOutput);
}
HERROR TRP135_Get_Capa_ServiceMonitoring_MaxActiveMainStreams(HCHAR *pszOutput)
{
	return get_Capa_ServiceMonitoring_MaxActiveMainStreams(pszOutput);
}
HERROR TRP135_Get_Capa_ServiceMonitoring_MinSampleInterval(HCHAR *pszOutput)
{
	return get_Capa_ServiceMonitoring_MinSampleInterval(pszOutput);
}
HERROR TRP135_Get_Capa_ServiceMonitoring_MaxReportSamples(HCHAR *pszOutput)
{
	return get_Capa_ServiceMonitoring_MaxReportSamples(pszOutput);
}
HERROR TRP135_Get_Capa_ServiceMonitoring_HighLevelMetricNames(HCHAR *pszOutput)
{
	return get_Capa_ServiceMonitoring_HighLevelMetricNames(pszOutput);
}

HERROR TRP135_Get_Capa_MaxActiveAVStreams(HCHAR *pnNum)
{
	return get_Capability_MaxActiveAVStreams(pnNum);
}
HERROR TRP135_Get_Capa_MaxActiveAVPlayers(HINT32 *pnNum)
{
	return get_Capability_MaxActiveAVPlayers(pnNum);
}
HERROR TRP135_Get_Comp_FrontEndNumberOfEntries(HINT32 *pszOutput)
{
	return get_Comp_FrontEndNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Comp_AudioDecoderNumberOfEntries(HINT32 *pszOutput)
{
	return get_Comp_AudioDecoderNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Comp_VideoDecoderNumberOfEntries(HINT32 *pszOutput)
{
	return get_Comp_VideoDecoderNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Comp_AudioOutputNumberOfEntries(HINT32 *pszOutput)
{
	return get_Comp_AudioOutputNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutputNumberOfEntries(HINT32 *pszOutput)
{
	return get_Comp_VideoOutputNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Comp_CANumberOfEntries(HINT32 *pszOutput)
{
	return get_Comp_CANumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Comp_HDMINumberOfEntries(HINT32 *pszOutput)
{
	return get_Comp_HDMINumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Comp_SPDIFNumberOfEntries(HINT32 *pszOutput)
{
	return get_Comp_SPDIFNumberOfEntries(pszOutput);
}

#define _MULTI_

HERROR TRP135_Get_Comp_FrontEnd_M_Status(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_Status(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_Name(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_Name(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Frequency(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Frequency(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Constellation(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Constellation(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_SNR(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_SNR(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_BER(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_BER(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_CBER(HCHAR *pszOutput)
{
	//Not Supported

	HxSTD_StrCpy(pszOutput, "0");
	return ERR_OK;
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Power(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Power(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Locked(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_ISDBC_Modulation_Locked(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_ISDBC_Service_CurrentLogicalChannel(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_ISDBC_Service_CurrentLogicalChannel(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_Status(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_Status(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_IPAddress(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_IPAddress(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_MACAddress(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_MACAddress(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Frequency(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Frequency(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Constellation(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Constellation(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_BER(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_download_BER(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_SNR(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_download_SNR(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Power(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_download_Power(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Frequency(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Frequency(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_ChannelId(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_ChannelId(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_BandWidth(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_BandWidth(pszOutput);
}
HERROR TRP135_Get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Power(HCHAR *pszOutput)
{
	return	get_Comp_FrontEnd_M_JCOM_DOCSIS_upload_Power(pszOutput);
}
HERROR TRP135_Get_Comp_PVR_StorageNumberOfEntries(HINT32 *pszOutput)
{
	return	get_Comp_PVR_StorageNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_Comp_PVR_Storage_M_Reference(HCHAR *pszOutput)
{
	return	get_Comp_PVR_Storage_M_Reference(pszOutput);
}
HERROR TRP135_Get_Comp_AudioDecoder_M_Status(HCHAR *pszOutput)
{
	return	get_Comp_AudioDecoder_M_Status(pszOutput);
}
HERROR TRP135_Get_Comp_AudioDecoder_M_Name(HCHAR *pszOutput)
{
	return	get_Comp_AudioDecoder_M_Name(pszOutput);
}
HERROR TRP135_Get_Comp_AudioDecoder_M_AudioStandard(HCHAR *pszOutput)
{
	return	get_Comp_AudioDecoder_M_AudioStandard(pszOutput);
}
HERROR TRP135_Get_Comp_VideoDecoder_M_Status(HCHAR *pszOutput)
{
	return	get_Comp_VideoDecoder_M_Status(pszOutput);
}
HERROR TRP135_Get_Comp_VideoDecoder_M_Name(HCHAR *pszOutput)
{
	return	get_Comp_VideoDecoder_M_Name(pszOutput);
}
HERROR TRP135_Get_Comp_VideoDecoder_M_MPEG2Part2(HCHAR *pszOutput)
{
	return	get_Comp_VideoDecoder_M_MPEG2Part2(pszOutput);
}
HERROR TRP135_Get_Comp_VideoDecoder_M_MPEG4Part2(HCHAR *pszOutput)
{
	return	get_Comp_VideoDecoder_M_MPEG4Part2(pszOutput);
}
HERROR TRP135_Get_Comp_VideoDecoder_M_MPEG4Part10(HCHAR *pszOutput)
{
	return	get_Comp_VideoDecoder_M_MPEG4Part10(pszOutput);
}
HERROR TRP135_Get_Comp_VideoDecoder_M_SMPTEVC1(HCHAR *pszOutput)
{
	return	get_Comp_VideoDecoder_M_SMPTEVC1(pszOutput);
}
HERROR TRP135_Get_Comp_VideoDecoder_M_ContentAspectRatio(HCHAR *pszOutput)
{
	return	get_Comp_VideoDecoder_M_ContentAspectRatio(pszOutput);
}
HERROR TRP135_Get_Comp_AudioOutput_M_Status(HCHAR *pszOutput)
{
	return	get_Comp_AudioOutput_M_Status(pszOutput);
}
HERROR TRP135_Get_Comp_AudioOutput_M_Name(HCHAR *pszOutput)
{
	return	get_Comp_AudioOutput_M_Name(pszOutput);
}
HERROR TRP135_Get_Comp_AudioOutput_M_AudioFormat(HCHAR *pszOutput)
{
	return	get_Comp_AudioOutput_M_AudioFormat(pszOutput);
}
HERROR TRP135_Get_Comp_AudioOutput_M_AudioLevel(HCHAR *pszOutput)
{
	return	get_Comp_AudioOutput_M_AudioLevel(pszOutput);
}
HERROR TRP135_Get_Comp_AudioOutput_M_CancelMute(HCHAR *pszOutput)
{
	return	get_Comp_AudioOutput_M_CancelMute(pszOutput);
}
HERROR TRP135_Get_Comp_SPDIF_M_Status(HCHAR *pszOutput)
{
	return	get_Comp_SPDIF_M_Status(pszOutput);
}
HERROR TRP135_Get_Comp_SPDIF_M_Name(HCHAR *pszOutput)
{
	return	get_Comp_SPDIF_M_Name(pszOutput);
}
HERROR TRP135_Get_Comp_SPDIF_M_ForcePCM(HCHAR *pszOutput)
{
	return	get_Comp_SPDIF_M_ForcePCM(pszOutput);
}
HERROR TRP135_Get_Comp_SPDIF_M_Passthrough(HCHAR *pszOutput)
{
	return	get_Comp_SPDIF_M_Passthrough(pszOutput);
}
HERROR TRP135_Get_Comp_SPDIF_M_AudioDelay(HCHAR *pszOutput)
{
	return	get_Comp_SPDIF_M_AudioDelay(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutput_M_Status(HCHAR *pszOutput)
{
	return	get_Comp_VideoOutput_M_Status(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutput_M_Name(HCHAR *pszOutput)
{
	return	get_Comp_VideoOutput_M_Name(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutput_M_CompositeVideoStandard(HCHAR *pszOutput)
{
	return	get_Comp_VideoOutput_M_CompositeVideoStandard(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutput_M_VideoFormat(HCHAR *pszOutput)
{
	return	get_Comp_VideoOutput_M_VideoFormat(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutput_M_AspectRatioBehaviour(HCHAR *pszOutput)
{
	return	get_Comp_VideoOutput_M_AspectRatioBehaviour(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutput_M_DisplayFormat(HCHAR *pszOutput)
{
	return	get_Comp_VideoOutput_M_DisplayFormat(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutput_M_Macrovision(HCHAR *pszOutput)
{
	return	get_Comp_VideoOutput_M_Macrovision(pszOutput);
}
HERROR TRP135_Get_Comp_VideoOutput_M_HDCP(HCHAR *pszOutput)
{
	return	get_Comp_VideoOutput_M_HDCP(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_Status(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_Status(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_Name(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_Name(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_ResolutionMode(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_ResolutionMode(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_ResolutionValue(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_ResolutionValue(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_JCOM_CECEnabled(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_JCOM_CECEnabled(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_JCOM_3DOutput(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_JCOM_3DOutput(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_Status(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_Status(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_Name(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_Name(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_EEDID(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_EEDID(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_SupportedResolutions(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_SupportedResolutions(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_PreferredResolution(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_PreferredResolution(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_VideoLatency(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_VideoLatency(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_CECSupport(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_CECSupport(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_AutoLipSyncSupport(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_AutoLipSyncSupport(pszOutput);
}
HERROR TRP135_Get_Comp_HDMI_M_DisplayDevice_HDMI3DPresent(HCHAR *pszOutput)
{
	return	get_Comp_HDMI_M_DisplayDevice_HDMI3DPresent(pszOutput);
}
HERROR TRP135_Get_Comp_CA_M_Status(HCHAR *pszOutput)
{
	return	get_Comp_CA_M_Status(pszOutput);
}
HERROR TRP135_Get_Comp_CA_M_Name(HCHAR *pszOutput)
{
	return	get_Comp_CA_M_Name(pszOutput);
}
HERROR TRP135_Get_Comp_CA_M_SmartCardReader(HCHAR *pszOutput)
{
	return	get_Comp_CA_M_SmartCardReader(pszOutput);
}
HERROR TRP135_Get_AVStreams_ActiveAVStreams(HCHAR *pszOutput)
{
	return	get_AVStreams_ActiveAVStreams(pszOutput);
}
HERROR TRP135_Get_AVStreams_AVStreamNumberOfEntries(HINT32 *pszOutput)
{
	return	get_AVStreams_AVStreamNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_AVStreams_AVStream_M_Status(HCHAR *pszOutput)
{
	return	get_AVStreams_AVStream_M_Status(pszOutput);
}
HERROR TRP135_Get_AVStreams_AVStream_M_Name(HCHAR *pszOutput)
{
	return	get_AVStreams_AVStream_M_Name(pszOutput);
}
HERROR TRP135_Get_AVStreams_AVStream_M_PVRState(HCHAR *pszOutput)
{
	return	get_AVStreams_AVStream_M_PVRState(pszOutput);
}
HERROR TRP135_Get_AVStreams_AVStream_M_FrontEnd(HCHAR *pszOutput)
{
	return	get_AVStreams_AVStream_M_FrontEnd(pszOutput);
}
HERROR TRP135_Get_AVStreams_AVStream_M_AudioDecoder(HCHAR *pszOutput)
{
	return	get_AVStreams_AVStream_M_AudioDecoder(pszOutput);
}
HERROR TRP135_Get_AVStreams_AVStream_M_VideoDecoder(HCHAR *pszOutput)
{
	return	get_AVStreams_AVStream_M_VideoDecoder(pszOutput);
}
HERROR TRP135_Get_AVStreams_AVStream_M_CA(HCHAR *pszOutput)
{
	return	get_AVStreams_AVStream_M_CA(pszOutput);
}
HERROR TRP135_Get_AVPlayers_ActiveAVPlayers(HCHAR *pszOutput)
{
	return	get_AVPlayers_ActiveAVPlayers(pszOutput);
}
HERROR TRP135_Get_AVPlayers_AVPlayerNumberOfEntries(HINT32 *pszOutput)
{
	return	get_AVPlayers_AVPlayerNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_AVPlayers_PreferredAudioLanguage(HCHAR *pszOutput)
{
	return	get_AVPlayers_PreferredAudioLanguage(pszOutput);
}
HERROR TRP135_Get_AVPlayers_PreferredSubtitlingLanguage(HCHAR *pszOutput)
{
	return	get_AVPlayers_PreferredSubtitlingLanguage(pszOutput);
}
HERROR TRP135_Get_AVPlayers_PreferredBehaviour(HCHAR *pszOutput)
{
	return	get_AVPlayers_PreferredBehaviour(pszOutput);
}
HERROR TRP135_Get_AVPlayers_ResetPINCode(HCHAR *pszOutput)
{
	return	get_AVPlayers_ResetPINCode(pszOutput);
}
HERROR TRP135_Get_AVPlayers_JCOM_PreferredSuperimposeLanguage(HCHAR *pszOutput)
{
	return	get_AVPlayers_JCOM_PreferredSuperimposeLanguage(pszOutput);
}
HERROR TRP135_Get_AVPlayers_JCOM_Zoom(HCHAR *pszOutput)
{
	return	get_AVPlayers_JCOM_Zoom(pszOutput);
}

HERROR TRP135_Get_AVPlayers_AVPlayer_M_Enable(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_Enable(pszOutput);
}


HERROR TRP135_Get_AVPlayers_AVPlayer_M_Status(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_Status(pszOutput);
}

HERROR TRP135_Get_AVPlayers_AVPlayer_M_Alias(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_Alias(pszOutput);
}


HERROR TRP135_Get_AVPlayers_AVPlayer_M_Name(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_Name(pszOutput);
}
HERROR TRP135_Get_AVPlayers_AVPlayer_M_AudioLanguage(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_AudioLanguage(pszOutput);
}
HERROR TRP135_Get_AVPlayers_AVPlayer_M_SubtitlingStatus(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_SubtitlingStatus(pszOutput);
}
HERROR TRP135_Get_AVPlayers_AVPlayer_M_SubtitlingLanguage(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_SubtitlingLanguage(pszOutput);
}
HERROR TRP135_Get_AVPlayers_AVPlayer_M_AudioOutputs(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_AudioOutputs(pszOutput);
}
HERROR TRP135_Get_AVPlayers_AVPlayer_M_VideoOutputs(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_VideoOutputs(pszOutput);
}
HERROR TRP135_Get_AVPlayers_AVPlayer_M_MainStream(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_MainStream(pszOutput);
}
HERROR TRP135_Get_AVPlayers_AVPlayer_M_PIPStreams(HCHAR *pszOutput)
{
	return	get_AVPlayers_AVPlayer_M_PIPStreams(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PostalCode(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PostalCode(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_TuningRange(HCHAR *pszOutput)
{
	return	get_JCOM_Config_TuningRange(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_URL_Navi(HCHAR *pszOutput)
{
	return	get_JCOM_Config_URL_Navi(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_URL_InteracTV(HCHAR *pszOutput)
{
	return	get_JCOM_Config_URL_InteracTV(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_URL_VOD(HCHAR *pszOutput)
{
	return	get_JCOM_Config_URL_VOD(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_URL_AppDLS(HCHAR *pszOutput)
{
	return	get_JCOM_Config_URL_AppDLS(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_URL_recommendServer(HCHAR *pszOutput)
{
	return	get_JCOM_Config_URL_recommendServer(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_URL_OpenIDServer(HCHAR *pszOutput)
{
	return	get_JCOM_Config_URL_OpenIDServer(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_URL_EPGServer(HCHAR *pszOutput)
{
	return	get_JCOM_Config_URL_EPGServer(pszOutput);
}
#if 0
HERROR TRP135_Get_JCOM_Config_WebBrowserURLInput(HCHAR *pszOutput)
{
	return	get_JCOM_Config_WebBrowserURLInput(pszOutput);
}

#endif
HERROR TRP135_Get_JCOM_Config_AutoFirmwareUpdate(HCHAR *pbAutoDownEnable)
{
	return	get_JCOM_Config_AutoFirmwareUpdate(pbAutoDownEnable);
}
HERROR TRP135_Get_JCOM_Config_ParentalControl(HCHAR *pnOutput)
{
	return	get_JCOM_Config_ParentalControl(pnOutput);
}
HERROR TRP135_Get_JCOM_Config_PINCodeExpiration(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PINCodeExpiration(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_StandbyMode(HCHAR *pszOutput)
{
	return	get_JCOM_Config_StandbyMode(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PowerStatus(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PowerStatus(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ClientEquipmentNumberOfEntries(HINT32 *pszOutput)
{
	return	get_JCOM_Config_ClientEquipmentNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_RemoteEquipmentNumberOfEntries(HINT32 *pszOutput)
{
	return	get_JCOM_Config_RemoteEquipmentNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_HomeNetwork_Enable(HCHAR *pszOutput)
{
	return	get_JCOM_Config_HomeNetwork_Enable(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ChannelMasking_Status(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ChannelMasking_Status(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ChannelMasking_ForcedOperation(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ChannelMasking_ForcedOperation(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ChannelMasking_Enable(HCHAR *pbOutput)
{
	return	get_JCOM_Config_ChannelMasking_Enable(pbOutput);
}
HERROR TRP135_Get_JCOM_Config_Internet_ConnectionType(HCHAR *pszOutput)
{
	return	get_JCOM_Config_Internet_ConnectionType(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_Internet_ConnectionTypeUserChange(HCHAR *pszOutput)
{
	return	get_JCOM_Config_Internet_ConnectionTypeUserChange(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_Internet_HTTPProxy_Enable(HCHAR *pszOutput)
{
	return	get_JCOM_Config_Internet_HTTPProxy_Enable(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_Internet_HTTPProxy_IPAddress(HCHAR *pszOutput)
{
	return	get_JCOM_Config_Internet_HTTPProxy_IPAddress(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_Internet_HTTPProxy_PortNumber(HCHAR *pszOutput)
{
	return	get_JCOM_Config_Internet_HTTPProxy_PortNumber(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_Internet_HTTPProxy_UserChange(HCHAR *pszOutput)
{
	return	get_JCOM_Config_Internet_HTTPProxy_UserChange(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_MediaServer_Names(HCHAR *pszOutput)
{
	return	get_JCOM_Config_MediaServer_Names(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_MediaServer_Name(HCHAR *pszOutput)
{
	return	get_JCOM_Config_MediaServer_Name(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_MediaServer_Clients(HCHAR *pszOutput)
{
	return	get_JCOM_Config_MediaServer_Clients(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PurchasedPPV_TotalPriceOfThisMonth(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PurchasedPPV_TotalPriceOfThisMonth(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PurchasedPPV_TotalPriceOfPreviousMonth(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PurchasedPPV_TotalPriceOfPreviousMonth(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PurchasedPPV_HistoryNumberOfEntries(HINT32 *pszOutput)
{
	return	get_JCOM_Config_PurchasedPPV_HistoryNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PurchasedPPV_History_M_IsdbId(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PurchasedPPV_History_M_IsdbId(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Name(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PurchasedPPV_History_M_Name(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Date(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PurchasedPPV_History_M_Date(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_PurchasedPPV_History_M_Price(HCHAR *pszOutput)
{
	return	get_JCOM_Config_PurchasedPPV_History_M_Price(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabaseNumberOfEntries(HINT32 *pszOutput)
{
	return	get_JCOM_Config_ISDBC_ServiceListDatabaseNumberOfEntries(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Start(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Start(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Status(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Status(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Progress(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Progress(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_AreaCode(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_AreaCode(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_DTT(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Frequency_DTT(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_BS(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Frequency_BS(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CS1(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Frequency_CS1(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CS2(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Frequency_CS2(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_CATV(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Frequency_CATV(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_Install_Frequency_JC_HITS(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_Install_Frequency_JC_HITS(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_NetworkId(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_ServiceListDatabase_M_NetworkId(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_Name(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_ServiceListDatabase_M_Name(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LCNOfEntries(HINT32 *pszOutput)
{
	return	get_JCOM_Config_ISDBC_ServiceListDatabase_M_LCNOfEntries(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_LCN(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_LCN(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Name(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Name(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_IsdbId(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_IsdbId(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Frequency(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Frequency(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_USB_HDD_PictureQuarity(HCHAR *pszOutput)
{
	return	get_JCOM_Config_USB_HDD_PictureQuarity(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_USB_HDD_Selected(HCHAR *pszOutput)
{
	return	get_JCOM_Config_USB_HDD_Selected(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_USB_HDD_AutoChapter(HCHAR *pszOutput)
{
	return	get_JCOM_Config_USB_HDD_AutoChapter(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_LAN_HDD_ServerName(HCHAR *pszOutput)
{
	return	get_JCOM_Config_LAN_HDD_ServerName(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_LAN_HDD_RecordingPlace(HCHAR *pszOutput)
{
	return	get_JCOM_Config_LAN_HDD_RecordingPlace(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_HomeServer_Enable(HCHAR *pszOutput)
{
	return	get_JCOM_Config_HomeServer_Enable(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_HomeServer_Name(HCHAR *pszOutput)
{
	return	get_JCOM_Config_HomeServer_Name(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ClientEquipment_M_Enable(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ClientEquipment_M_Enable(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_ClientEquipment_M_MACAddress(HCHAR *pszOutput)
{
	return	get_JCOM_Config_ClientEquipment_M_MACAddress(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_RemoteEquipment_M_Enable(HCHAR *pszOutput)
{
	return	get_JCOM_Config_RemoteEquipment_M_Enable(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_RemoteEquipment_M_Name(HCHAR *pszOutput)
{
	return	get_JCOM_Config_RemoteEquipment_M_Name(pszOutput);
}
HERROR TRP135_Get_JCOM_Config_RemoteEquipment_M_Nickname(HCHAR *pszOutput)
{
	return	get_JCOM_Config_RemoteEquipment_M_Nickname(pszOutput);
}






#define ______________________PUBLIC_FUNCTION_TRP_SET______________________

HERROR TRP135_Set_JCOM_Config_PostalCode(HCHAR *pszOutput)
{
	return	set_JCOM_Config_PostalCode(pszOutput);
}
HERROR TRP135_Set_JCOM_Config_TuningRange(HCHAR *pszRange)
{
	return	set_JCOM_Config_TuningRange(pszRange);
}
HERROR TRP135_Set_JCOM_Config_URL_Navi(HCHAR *pszInput)
{
	return set_JCOM_Config_URL_Navi(pszInput);
}
HERROR TRP135_Set_JCOM_Config_URL_InteracTV(HCHAR *pszInput)
{
	return set_JCOM_Config_URL_InteracTV(pszInput);
}
HERROR TRP135_Set_JCOM_Config_URL_VOD(HCHAR *pszInput)
{
	return set_JCOM_Config_URL_VOD(pszInput);
}
HERROR TRP135_Set_JCOM_Config_URL_AppDLS(HCHAR *pszInput)
{
	return set_JCOM_Config_URL_AppDLS(pszInput);
}
HERROR TRP135_Set_JCOM_Config_URL_recommendServer(HCHAR *pszInput)
{
	return set_JCOM_Config_URL_recommendServer(pszInput);
}
HERROR TRP135_Set_JCOM_Config_URL_OpenIDServer(HCHAR *pszInput)
{
	return	set_JCOM_Config_URL_OpenIDServer(pszInput);
}
HERROR TRP135_Set_JCOM_Config_URL_EPGServer(HCHAR *pszInput)
{
	return	set_JCOM_Config_URL_EPGServer(pszInput);
}
HERROR TRP135_Set_JCOM_Config_AutoFirmwareUpdate(HCHAR *pszAutoDownEnable)
{
	return	set_JCOM_Config_AutoFirmwareUpdate(pszAutoDownEnable);
}
HERROR TRP135_Set_JCOM_Config_ParentalControl(HCHAR *ratingValue)
{
	return	set_JCOM_Config_ParentalControl(ratingValue);
}
HERROR TRP135_Set_JCOM_Config_PINCodeExpiration(HCHAR *pszPin)
{
	return	set_JCOM_Config_PINCodeExpiration(pszPin);
}
HERROR TRP135_Set_JCOM_Config_StandbyMode(HCHAR *pszStandbyMode)
{
	return	set_JCOM_Config_StandbyMode(pszStandbyMode);
}
HERROR TRP135_Set_JCOM_Config_PowerStatus(HCHAR *pszInput)
{
	//RemLOG_Print("What's this option? (%s/%d)\n",__func__,__LINE__);
	return 	set_JCOM_Config_PowerStatus(pszInput);
}

HERROR TRP135_Set_JCOM_Config_ChannelMasking_ForcedOperation(HCHAR *pszOutput)
{
	return	set_JCOM_Config_ChannelMasking_ForcedOperation(pszOutput);
}
HERROR TRP135_Set_JCOM_Config_ChannelMasking_Enable(HCHAR *pszChMaskEnable)
{
	return	set_JCOM_Config_ChannelMasking_Enable(pszChMaskEnable);
}
HERROR TRP135_Set_JCOM_Config_MediaServer_Name(HCHAR *pszFriendlyName)
{
	return	set_JCOM_Config_MediaServer_Name(pszFriendlyName);
}
HERROR TRP135_Set_Comp_AudioOutput_M_AudioLevel(HCHAR *pszInput)
{
	return	set_Comp_AudioOutput_M_AudioLevel(pszInput);
}
HERROR TRP135_Set_Comp_AudioOutput_M_CancelMute(HCHAR *pszInput)
{
	return	set_Comp_AudioOutput_M_CancelMute(pszInput);
}
HERROR TRP135_Set_Comp_SPDIF_M_ForcePCM(HCHAR *pszInput)
{
	return	set_Comp_SPDIF_M_ForcePCM(pszInput);
}
HERROR TRP135_Set_Comp_VideoOutput_M_VideoFormat(HCHAR *pszInput)
{
	return	set_Comp_VideoOutput_M_VideoFormat(pszInput);
}
HERROR TRP135_Set_Comp_VideoOutput_M_DisplayFormat(HCHAR *pszInput)
{
	return	set_Comp_VideoOutput_M_DisplayFormat(pszInput);
}
HERROR TRP135_Set_Comp_HDMI_M_ResolutionMode(HCHAR *pszInput)
{
	return	set_Comp_HDMI_M_ResolutionMode(pszInput);
}
HERROR TRP135_Set_Comp_HDMI_M_ResolutionValue(HCHAR *pszInput)
{
	return	set_Comp_HDMI_M_ResolutionValue(pszInput);
}
HERROR TRP135_Set_Comp_HDMI_M_JCOM_CECEnabled(HCHAR *pszInput)
{
	return	set_Comp_HDMI_M_JCOM_CECEnabled(pszInput);
}
HERROR TRP135_Set_Comp_HDMI_M_JCOM_3DOutput(HCHAR *pszInput)
{
	return	set_Comp_HDMI_M_JCOM_3DOutput(pszInput);
}
HERROR TRP135_Set_AVPlayers_PreferredAudioLanguage(HCHAR *pszInput)
{
	return	set_AVPlayers_PreferredAudioLanguage(pszInput);
}
HERROR TRP135_Set_AVPlayers_PreferredSubtitlingLanguage(HCHAR *pszInput)
{
	return	set_AVPlayers_PreferredSubtitlingLanguage(pszInput);
}
HERROR TRP135_Set_AVPlayers_PreferredBehaviour(HCHAR *pszInput)
{
	return	set_AVPlayers_PreferredBehaviour(pszInput);
}
HERROR TRP135_Set_AVPlayers_ResetPINCode(HCHAR *pszInput)
{
	return	set_AVPlayers_ResetPINCode(pszInput);
}
HERROR TRP135_Set_AVPlayers_JCOM_PreferredSuperimposeLanguage(HCHAR *pszInput)
{
	return	set_AVPlayers_JCOM_PreferredSuperimposeLanguage(pszInput);
}
HERROR TRP135_Set_AVPlayers_JCOM_Zoom(HCHAR *pszInput)
{
	return	set_AVPlayers_JCOM_Zoom(pszInput);
}
HERROR TRP135_Set_JCOM_Config_Internet_ConnectionType(HCHAR *pszInput)
{
	return	set_JCOM_Config_Internet_ConnectionType(pszInput);
}
HERROR TRP135_Set_JCOM_Config_Internet_ConnectionTypeUserChange(HCHAR *pszInput)
{
	return	set_JCOM_Config_Internet_ConnectionTypeUserChange(pszInput);
}
HERROR TRP135_Set_JCOM_Config_Internet_HTTPProxy_Enable(HCHAR *pszInput)
{
	return	set_JCOM_Config_Internet_HTTPProxy_Enable(pszInput);
}
HERROR TRP135_Set_JCOM_Config_Internet_HTTPProxy_IPAddress(HCHAR *pszInput)
{
	return	set_JCOM_Config_Internet_HTTPProxy_IPAddress(pszInput);
}
HERROR TRP135_Set_JCOM_Config_Internet_HTTPProxy_PortNumber(HCHAR *pszInput)
{
	return	set_JCOM_Config_Internet_HTTPProxy_PortNumber(pszInput);
}
HERROR TRP135_Set_JCOM_Config_Internet_HTTPProxy_UserChange(HCHAR *pszInput)
{
	return	set_JCOM_Config_Internet_HTTPProxy_UserChange(pszInput);
}
HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Start(HCHAR *pszInput)
{
	return	set_JCOM_Config_ISDBC_Install_Start(pszInput);
}
HERROR TRP135_Set_JCOM_Config_ISDBC_Install_AreaCode(HCHAR *pszInput)
{
	return	set_JCOM_Config_ISDBC_Install_AreaCode(pszInput);
}
HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_DTT(HCHAR *pszInput)
{
	return	set_JCOM_Config_ISDBC_Install_Frequency_DTT(pszInput);
}
HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_BS(HCHAR *pszInput)
{
	return	set_JCOM_Config_ISDBC_Install_Frequency_BS(pszInput);
}
HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CS1(HCHAR *pszInput)
{
	return	set_JCOM_Config_ISDBC_Install_Frequency_CS1(pszInput);
}
HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CS2(HCHAR *pszInput)
{
	return	set_JCOM_Config_ISDBC_Install_Frequency_CS2(pszInput);
}
HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_CATV(HCHAR *pszInput)
{
	return	set_JCOM_Config_ISDBC_Install_Frequency_CATV(pszInput);
}
HERROR TRP135_Set_JCOM_Config_ISDBC_Install_Frequency_JC_HITS(HCHAR *pszInput)
{
	return	set_JCOM_Config_ISDBC_Install_Frequency_JC_HITS(pszInput);
}
HERROR TRP135_Set_JCOM_Config_USB_HDD_PictureQuarity(HCHAR *pszInput)
{
	return	set_JCOM_Config_USB_HDD_PictureQuarity(pszInput);
}
HERROR TRP135_Set_JCOM_Config_USB_HDD_Selected(HCHAR *pszInput)
{
	return	set_JCOM_Config_USB_HDD_Selected(pszInput);
}
HERROR TRP135_Set_JCOM_Config_USB_HDD_AutoChapter(HCHAR *pszInput)
{
	return	set_JCOM_Config_USB_HDD_AutoChapter(pszInput);
}
HERROR TRP135_Set_JCOM_Config_HomeServer_Enable(HCHAR *pszInput)
{
	return	set_JCOM_Config_HomeServer_Enable(pszInput);
}
HERROR TRP135_Set_JCOM_Config_HomeServer_Name(HCHAR *pszInput)
{
	return	set_JCOM_Config_HomeServer_Name(pszInput);
}

HERROR TR135_module_Init(){
   REM_FUNC_ENTER;

	OAPI_OUTPUT_VIDEO_Init(tr135_output_video_onStatusChanged);
	OAPI_SCENARIO_Init();

	REM_FUNC_LEAVE;

	return ERR_OK;
}
HERROR TR135_module_DeInit(){
   REM_FUNC_ENTER;

	REM_FUNC_LEAVE;

	return ERR_OK;
}







