#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<string.h>
#endif

#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "cmd_hwtest_interface.h"
#include "cmd_hwtest_parser_demo.h"

#include "di_media20.h"

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define UNUSED(x) (void)(x)


//#define	_DEBUG_
//#define	_DEBUG_ENTER_EXIT_

#ifdef	_DEBUG_
#define	PrintData	DI_UART_Print
#define	PrintError	DI_UART_Print
#else
#define	PrintData	while (0) ((int (*)(char *, ...)) 0)
#define	PrintError	DI_UART_Print
#endif

#ifdef	_DEBUG_ENTER_EXIT_
#define	PrintEnter()	DI_UART_Print("(+)%s\n\r", __FUNCTION__)
#define	PrintExit()		DI_UART_Print("(-)%s\n\r", __FUNCTION__)
#else
#define	PrintEnter()
#define	PrintExit()
#endif

SESSION_HANDLE g_hMPlay_Session;
HUINT32 g_MPlay_MediaId;
DI_MEDIA_EventCallback_t s_tMediaEventCallback;



void P_CMD_MediaCallbackFn(HUINT32 ulDeviceId, HUINT32 ulEvent, HUINT32 Value)
{
	if(s_tMediaEventCallback != NULL)
	{
		s_tMediaEventCallback(ulDeviceId,ulEvent,Value);
	}

}

void P_CMD_MEDIA_RegisterCallback(DI_MEDIA_EventCallback_t MediaEventCallback)
{

	s_tMediaEventCallback = MediaEventCallback;

}

void P_CMD_MEDIA_Stop(void)
{
	DI_ERR_CODE dResult;

	dResult = DI_MEDIA_StopSession(g_hMPlay_Session, g_MPlay_MediaId);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_StopSession failed!\n");
		return ;
	}
	CMD_Printf("Stop Decoders....\n");

	dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_AUDIO_Stop failed!\n");
	}

	dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_VIDEO_Stop failed!\n");
	}

	dResult = DI_MEDIA_UnregisterMediaEventCallback(g_hMPlay_Session, g_MPlay_MediaId);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_UnegisterMediaEventCallback failed!\n");
		return ;
	}

	dResult = DI_MEDIA_ReleaseSession(g_hMPlay_Session, g_MPlay_MediaId);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_ReleaseSession failed!\n");
		return ;
	}

	dResult = DI_MEDIA_CloseSession(g_hMPlay_Session);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_CloseSession failed!\n");
		return;
	}

	dResult = DI_MEDIA_DestroySession(g_hMPlay_Session);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_DestroySession failed!\n");
		return ;
	}

	return ;
}

void P_CMD_MEDIA_Play(unsigned char *ucStr)
{
	DI_ERR_CODE dResult;
	DI_MEDIA_INFO_t ProbeInfo;

	CMD_Printf("pUrl [[%s]] \n",ucStr);

	//create
	dResult = DI_MEDIA_CreateSession(ucStr, &(g_hMPlay_Session));
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_CreateSession failed!\n");
		return ;
	}
	CMD_Printf("created\n");
	//probe

	dResult = DI_MEDIA_ProbeSession(g_hMPlay_Session);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_ProbeSession failed!\n");
		return ;
	}

	CMD_Printf("probed\n");

	//startdecoder
	dResult = DI_MEDIA_GetProbeInfo(g_hMPlay_Session, &(ProbeInfo));
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_GetProbeInfo failed!\n");
		return ;
	}

	//opensession
	dResult = DI_MEDIA_OpenSession(g_hMPlay_Session);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_OpenSession failed!\n");
		return ;
	}
	CMD_Printf("opened\n");

	//setupsession
	dResult = DI_MEDIA_SetupSession(g_hMPlay_Session, &(g_MPlay_MediaId));
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_SetupSession failed!\n");
		return ;
	}
	CMD_Printf("Setup\n");

	//mediaeventcb
	dResult = DI_MEDIA_RegisterMediaEventCallback(g_hMPlay_Session, g_MPlay_MediaId, (DI_MEDIA_EventCallback_t)P_CMD_MediaCallbackFn);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_RegisterMediaEventCallback failed!\n");
		return ;
	}

	dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
		return ;
	}

	if(ProbeInfo.ucTotalAudioTrack != 0)
	{
		dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, ProbeInfo.audio_codec);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_Start failed!\n");
			return ;
		}
	}
	else
	{
		CMD_Printf("No audio track\n");
	}

	if(ProbeInfo.ucTotalVideoTrack != 0)
	{
		dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, ProbeInfo.video_codec);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_VIDEO_Start failed!\n");
			return ;
		}
	}
	else
	{
		CMD_Printf("No Video track\n");
	}
	CMD_Printf("Start decorders\n");

	//startsession
	dResult = DI_MEDIA_StartSession(g_hMPlay_Session, g_MPlay_MediaId, FALSE);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_MEDIA_StartSession failed!\n");
		return ;
	}
	return;
}


int CMD_HwTest_Parser_Demo(void *szArgs)
{
	GET_ARGS;

	UNUSED(szParam);
	UNUSED(szParam1);
	UNUSED(szParam2);
	UNUSED(iResult);

	if CMD_IS("media")
	{
		if (PARAM_IS("start"))
		{
			P_CMD_MEDIA_Play(HWTEST_PARAM1);

		}
		else if (PARAM_IS("stop"))
		{
			P_CMD_MEDIA_Stop();
		}
	}
	else if CMD_IS("bbb")
	{

	}
    return 0;
}


#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef HWTEST_PARAM2
#undef GET_ARGS
#undef PARAM_IS
#undef PARAM1_IS
#undef PARAM2_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC

