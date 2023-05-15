#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include "bstd_defs.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_media20.h"
#include "di_audio.h"
#include "di_video.h"
#include "di_demux.h"
#include "nexus_base_os.h"
#include "cmd_hwtest_parser_av_decode.h"

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgList); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgList); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgList); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgList);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgList)

#define AssertDI(arg1, arg2) if (arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define DEFAULT_TUNER 0
#define DEFAULT_PLAY_DEMUX 8
#define DEFAULT_REC_DEMUX 2
#define SECOND_REC_DEMUX 3
#define HDD_TEST_REC1_DEV_ID 1
#define HDD_TEST_REC2_DEV_ID 2
#define DEFAULT_PLAYER 0
#define DEFAULT_VIDEO 0
#define DEFAULT_AUDIO 0
#define MAX_REC_NUM 3
#define MAX_PID_NUM 3

static void *g_pHSession = NULL;
static HBOOL g_bIsTuned = FALSE;
static HBOOL g_bThreadRunning = FALSE;
static unsigned long g_ulThreadID = 0;

//static DI_IPTUNER_Index_e g_nTunerIndex = DI_IPTUNER_MAX_NUM;;
static DI_MEDIA_ChannelEventCallback_t s_ChannelEventCallbacks;

typedef struct CONF_MULTICAST_LIST
{
	const char *url;
	DI_MEDIA_ChannelTunerType_e eTunerType;
	unsigned int videopid;
    unsigned int videotype;
    unsigned int audiopid;
    unsigned int audiotype;
    unsigned int pcrpid;
} CONF_MULTICAST_LIST;

static const CONF_MULTICAST_LIST multicast_test_list[] = {
        { "udp://238.100.100.101:1234", DI_MEDIA_CHANNEL_TUNER_TYPE_AV, 0x21, DI_VIDEO_FORMAT_H264, 0x22, DI_AUDFORMAT_MPEG, 0x21 },
        { "udp://238.100.100.102:2345", DI_MEDIA_CHANNEL_TUNER_TYPE_AV, 0x31, DI_VIDEO_FORMAT_H264, 0x32, DI_AUDFORMAT_MPEG, 0x31 },
#if 0
        { "udp://238.100.100.103:3456", DI_MEDIA_CHANNEL_TUNER_TYPE_AV, 0x21, DI_VIDEO_FORMAT_H264, 0x22, DI_AUDFORMAT_MPEG, 0x21 },
        // Insert multicast entries here
        // ...
        // ...
#endif
        { NULL, DI_MEDIA_CHANNEL_TUNER_TYPE_AV, 0x0, DI_VIDEO_FORMAT_H264, 0x0, DI_AUDFORMAT_MPEG, 0x0 }
};


// typedef void (*DI_MEDIA_ChannelTuningEventCallback)
//              (HUINT32 ulTunerId, DI_MEDIA_ChannelTuningEvent_e eTuningEvent);
static void _local_ChannelTuningCallback(HUINT32 ulHandleID, DI_MEDIA_ChannelTuningEvent_e eTuningEvent)
{
	CMD_Printf("CMD>> =====================================\n");
	CMD_Printf("CMD>> = Channel Tuning Callback           =\n");
	CMD_Printf("CMD>> =====================================\n");
	CMD_Printf("CMD>> = Tuner ID:%d, Tuning Event:%d\n", ulHandleID, eTuningEvent);
	CMD_Printf("CMD>> =====================================\n");
}

// typedef void (*DI_MEDIA_ChannelNotifyEventCallback)
//              (HUINT32 ulTunerId, DI_MEDIA_ChannelNotifyEvent_e eNotifyEvent); // <--
static void _local_ChannelNotifyCallback(HUINT32 ulHandleID, DI_MEDIA_ChannelNotifyEvent_e eNotifyEvent)
{
	CMD_Printf("CMD>> =====================================\n");
	CMD_Printf("CMD>> = Channel Notify Callback           =\n");
	CMD_Printf("CMD>> =====================================\n");
	CMD_Printf("CMD>> = Tuner ID:%d, Notify Event:%d\n", ulHandleID, eNotifyEvent);
	CMD_Printf("CMD>> =====================================\n");
}

static int
HwTest_Multicast_PlayStart(HUINT8 *pURI,
					DI_VIDEO_FORMAT nVidFmt, DI_AUDIO_FORMAT nAudFmt,
					HUINT16 usVideoPid, HUINT16 usAudioPid, HUINT16 usPcrPid)
{
	DI_ERR_CODE eDiRet = DI_ERR_OK;
	void *pHSession;
	Pid_Info pidInfo;
	DI_MEDIA_ChannelTunerType_e eTunerType;

	CMD_Printf("CMD>> [HwTest_Multicast_PlayStart]++\n\r");

	if (pURI == NULL)
	{
		CMD_Printf("CMD>> Error>> Invalid NULL URL!\n\r");
		return CMD_ERR;
	}

	// Prepare playback/playpump
	{
		CMD_Printf("CMD>> URL : %s\n",pURI);

		CMD_Printf("CMD>> --> DI_MEDIA_CreateSession()\n");
		eDiRet = DI_MEDIA_CreateSession((HINT8 *)pURI, &pHSession);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant create multicast session!\r\n");
			goto func_error;
		}
		g_pHSession = pHSession;
		CMD_Printf("CMD>> --> (sess:%d, sess:%d)\n", g_pHSession, pHSession);

		eTunerType = DI_MEDIA_CHANNEL_TUNER_TYPE_AV;

		/*
		 * DI_MEDIA_SetSessionProperties
		 * DI_MEDIA_RegisterSessionCallback
		 */
		s_ChannelEventCallbacks.cbTuningEvent = _local_ChannelTuningCallback; // DI_MEDIA_ChannelTuningEventCallback
		s_ChannelEventCallbacks.cbNotifyEvent = _local_ChannelNotifyCallback; // DI_MEDIA_ChannelNotifyEventCallback

		CMD_Printf("CMD>> --> DI_MEDIA_RegisterChannelEventCallback()\n");
		eDiRet = DI_MEDIA_RegisterChannelEventCallback(pHSession,
		                                               eTunerType,
		                                               (DI_MEDIA_ChannelEventCallback_t *)&s_ChannelEventCallbacks);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant register multicast channel event!\r\n");
			goto func_error;
		}
	}

	CMD_Printf("CMD>> --> DI_DEMUX_SetPath()\n");
	eDiRet = DI_DEMUX_SetPath(DRV_DEMUX_GetFirstPlaybackParserIndex(),
				DI_DEMUX_INPUT_PLAYER,
				DEFAULT_PLAYER,
				DI_DEMUX_OUTPUT_DECODER, 0, -1);
	if (eDiRet != DI_ERR_OK)
	{
		CMD_Printf("CMD>> Error> in DI_DEMUX_SetPath\n\r");
		return CMD_ERR;
	}

	// Start playback/playpump
	{
		CMD_Printf("CMD>> --> DI_MEDIA_OpenSession()\n");
		eDiRet = DI_MEDIA_OpenSession(pHSession);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant open multicast session!\r\n");
			goto func_error;
		}

		g_bIsTuned = TRUE;
	}

	pidInfo.pidBitmask = PID_BITMASK_VIDEO | PID_BITMASK_AUDIO_MAIN | PID_BITMASK_PCR;
	pidInfo.usVideoPid = usVideoPid;
	pidInfo.usAudioMainPid = usAudioPid;
	pidInfo.usPCRPid = usPcrPid;

	CMD_Printf("CMD>> --> DI_DEMUX_StartPID()\n");
	eDiRet = DI_DEMUX_StartPID(DRV_DEMUX_GetFirstPlaybackParserIndex(), &pidInfo);
	if (eDiRet != DI_ERR_OK)
	{
		CMD_Printf("CMD>> Error> in DI_DEMUX_StartPID\n\r");
		return CMD_ERR;
	}

	CMD_Printf("CMD>> --> DI_AUDIO_Start(nAudFmt:%d)\n", nAudFmt);
	eDiRet = DI_AUDIO_Start(DEFAULT_AUDIO, nAudFmt);
	if (eDiRet != DI_ERR_OK)
	{
		CMD_Printf("CMD>> Error> in DI_AUDIO_Start\n\r");
		return CMD_ERR;
	}

	CMD_Printf("CMD>> --> DI_VIDEO_Start(nVidFmt:%d)\n", nVidFmt);
	eDiRet = DI_VIDEO_Start(DEFAULT_VIDEO, nVidFmt);
	if (eDiRet != DI_ERR_OK)
	{
		CMD_Printf("CMD>> Error> in DI_VIDEO_Start\n\r");
		return CMD_ERR;
	}

	CMD_Printf("CMD>> --> DI_VIDEO_Show()\n");
	eDiRet = DI_VIDEO_Show(DEFAULT_VIDEO, TRUE);
	if (eDiRet != DI_ERR_OK)
	{
		CMD_Printf("CMD>> Error> in DI_VIDEO_Show\n\r");
		return CMD_ERR;
	}

	CMD_Printf("CMD>> [HwTest_Multicast_PlayStart]--\n\r");
	return CMD_OK;

func_error:
	CMD_Printf("CMD>> Error> HwTest_Multicast_PlayStart failed...\n\r");
	return CMD_OK;
}

static int
HwTest_Multicast_PlayStop(int deviceID)
{
	DI_ERR_CODE eDiRet = DI_ERR_OK;
	void *pHSession = g_pHSession;

	CMD_Printf("CMD>> [HwTest_Multicast_PlayStop]++\n\r");

	if (g_bIsTuned == FALSE)
	{
		CMD_Printf("CMD>> ERROR> Invalid multicast status(%d)\r\n", g_bIsTuned);
		return 0;
	}

	if (pHSession == NULL)
	{
		CMD_Printf("CMD>> ERROR> Invalid multicast session()\r\n");
		return 0;
	}

	{
		CMD_Printf("CMD>> --> DI_VIDEO_Stop(devID:%d)\n", deviceID);
		DI_VIDEO_Stop(deviceID);


		CMD_Printf("CMD>> --> DI_AUDIO_Stop(devID:%d)\n", deviceID);
		DI_AUDIO_Stop(deviceID);


		CMD_Printf("CMD>> --> DI_DEMUX_StopPID(devID:%d)\n", deviceID);
		DI_DEMUX_StopPID(deviceID, PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR);


		CMD_Printf("CMD>> --> DI_DEMUX_ReleasePath(devID:%d)\n", deviceID);
		DI_DEMUX_ReleasePath(deviceID);
	}

	CMD_Printf("CMD>> --> DI_MEDIA_CloseSession(sess:%d)\n", pHSession);
	eDiRet = DI_MEDIA_CloseSession(pHSession);
	if (eDiRet != DI_ERR_OK)
	{
		CMD_Printf("CMD>> ERROR> Cant close multicast session!\r\n");
		goto func_error;
	}

	CMD_Printf("CMD>> --> DI_MEDIA_UnregisterChannelEventCallback(sess:%d)\n", pHSession);
	eDiRet = DI_MEDIA_UnregisterChannelEventCallback(pHSession);
	if (eDiRet != DI_ERR_OK)
	{
		CMD_Printf("CMD>> ERROR> Cant unregister multicast channel event!\r\n");
		goto func_error;
	}


	CMD_Printf("CMD>> --> DI_MEDIA_DestroySession(sess:%d)\n", pHSession);
	eDiRet = DI_MEDIA_DestroySession(pHSession);
	if (eDiRet != DI_ERR_OK)
	{
		CMD_Printf("CMD>> ERROR> Cant destroy multicast session!\r\n");
		goto func_error;
	}

	g_bIsTuned = FALSE;

	CMD_Printf("CMD>> [HwTest_Multicast_PlayStop]--\n\r");
	return CMD_OK;

func_error:
	CMD_Printf("CMD>> Error> HwTest_Multicast_PlayStop failed...\n\r");
	return CMD_OK;
}

static void HwTest_Multicast_ZappingTestThread(void *pvParam)
{
	int iReturn;
	unsigned int idx = 0;
	unsigned int loopCount = 0;
	DI_MEDIA_ChannelTunerType_e eTunerType;
	unsigned int videopid = 0xFFFF;
	unsigned int videotype = 0;
	unsigned int audiopid = 0xFFFF;
	unsigned int audiotype = 0;
	unsigned int pcrpid = 0xFFFF;

	BSTD_UNUSED(pvParam);

	loopCount = 0;
	while( g_bThreadRunning )
	{
		if (g_bThreadRunning == 0)
		{
			VK_TASK_Sleep(100);
			break;
		}

		if (loopCount%10 == 0)
		{
			if (g_bIsTuned == TRUE)
			{
				CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStop()\r\n");
				HwTest_Multicast_PlayStop(0);
				CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStop()\r\n");
				g_bIsTuned = FALSE;
				g_pHSession = NULL;
			}

			if (idx == 0)
			{
				idx = 1;
			}
			else
			{
				idx = 0;
			}

			eTunerType = multicast_test_list[idx].eTunerType;
			videopid   = multicast_test_list[idx].videopid;
			videotype  = multicast_test_list[idx].videotype;
			audiopid   = multicast_test_list[idx].audiopid;
			audiotype  = multicast_test_list[idx].audiotype;
			pcrpid     = multicast_test_list[idx].pcrpid;

			CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStart()\r\n");
			iReturn = HwTest_Multicast_PlayStart((HUINT8 *)multicast_test_list[idx].url,
									videotype,
									audiotype,
									(HUINT16)videopid,
									(HUINT16)audiopid,
									(HUINT16)pcrpid);
			CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStart()\r\n");
			if (iReturn != CMD_OK)
			{
				CMD_Printf("CMD>> %s(%d)\n", __FUNCTION__, __LINE__);
				; // return 0;
			}
			g_bIsTuned = TRUE;
		}

		VK_TASK_Sleep(1000);
		loopCount++;
	}

	if (g_bIsTuned == TRUE)
	{
		CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStop()\r\n");
		HwTest_Multicast_PlayStop(0);
		CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStop()\r\n");
		g_bIsTuned = FALSE;
		g_pHSession = NULL;
	}

	CMD_Printf("CMD>> === HwTest_Multicast_ZappingTestThread :: END ===\r\n");
	return;
}

unsigned long HwTest_Multicast_TaskCreate(const char* name, void (*func)(void *), void *args, int stackSize, int priority)
{
	NEXUS_ThreadSettings settingsThread;
	NEXUS_ThreadHandle	neThread;

	NEXUS_Thread_GetDefaultSettings(&settingsThread);
	if (priority >= 0) settingsThread.priority = priority;
	if (stackSize > 0) settingsThread.stackSize = stackSize;

	neThread = NEXUS_Thread_Create(name, func, args, &settingsThread);
	if (neThread == NULL)
		CMD_Printf("CMD>> Task Creation Fail !!!\n");

	return (unsigned long)neThread;
}

void HwTest_Multicast_TaskDestroy(unsigned long neThread)
{
	NEXUS_Thread_Destroy((NEXUS_ThreadHandle)neThread);
}

// multicast tune <uri>
// <uri> : udp://224.1.1.1:1234 or rtp://224.1.1.1:1234
// multicast release
int CMD_HwTest_Parser_MULTICAST(void *szArgList)
{
	HUINT8  *pucURI = NULL;
	HUINT8  *pucStreamPath = NULL;
	HUINT8  *pucStreamIndex = NULL;
	HUINT32 eDiRet = 0;
	int iReturn;
	HBOOL   bUseURL = FALSE;
	HUINT32 unDiVideoDecoderType = 0;
	HBOOL   bIPTuner = FALSE;
	HUINT32 nCount = 0;
	HUINT32 uStreamIndex = 0;
	HUINT8  *pOption = NULL;
	void *pHSession;
	DI_MEDIA_ChannelTunerType_e eTunerType;
	unsigned int videopid = 0xFFFF;
	unsigned int videotype = 0;
	unsigned int audiopid = 0xFFFF;
	unsigned int audiotype = 0;
	unsigned int pcrpid = 0xFFFF;

	GET_ARGS;

	unDiVideoDecoderType = DI_VIDEO_DECODER_MAIN;

	/*
	 * DI> multicast play 0
	 * DI> multicast play 1
	 * DI> ...
	 * DI> multicast test
	 * DI> multicast stop
	 */
#if 0
	// Following [tune], [tuneindex], [release] cmd test is for multicast tuning test.
	// So, these can't play multicast videos, ...
	// This tuning test can be confirmed di_media(multicast) internal debugging mode
	// through the streaming recording(file writing)
	if CMD_IS("reset")
	{
FREE_HEAP_MEMORY:
		//if ( g_nTunerIndex !=  DI_IPTUNER_MAX_NUM )
		//{
		//	DI_IPTUNER_Close(g_nTunerIndex);
		//	g_nTunerIndex = DI_IPTUNER_MAX_NUM;
		//}
		if (NULL != pucURI)
		{
        	DD_MEM_Free(pucURI);
			pucURI = NULL;
	    }
        /* else - do nothing */

	    //if (NULL != pstMediaInfo)
        //{
        //	DD_MEM_Free(pstMediaInfo);
	    //    pstMediaInfo = NULL;
        //}
		g_bIsTuned = FALSE;
	}
	else if CMD_IS("tune") // URL based
	{
		/*
		 * url have multicast address range
		 *   - 224.1.2.3:1234
		 */
		if (g_bIsTuned == TRUE)
		{
			CMD_Printf("CMD>> Command : media is already started(tune:%d) \r\n",
						g_bIsTuned);
			return 0;
		}

		bUseURL = FALSE;
		pucStreamPath = HWTEST_PARAM;
		if (pucStreamPath == NULL)
		{
			CMD_Printf("CMD>> Command : multicast tune <uri> <option> \r\n");
			return 0;
		}
		pOption = HWTEST_PARAM1;

		if ((NULL == VK_strstr((char *)pucStreamPath, "udp://"))
			&& (NULL == VK_strstr((char *)pucStreamPath, "rtp://")))
		{
			CMD_Printf("CMD>> ERROR> Unsupported multicast URL(%s)!\r\n", pucStreamPath);
			goto FREE_HEAP_MEMORY;
		}

		eTunerType = DI_MEDIA_CHANNEL_TUNER_TYPE_AV;

		CMD_Printf("CMD>> URL : %s\n",pucStreamPath);
		eDiRet = DI_MEDIA_CreateSession((HINT8 *)pucStreamPath, &pHSession);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant create multicast session!\r\n");
			goto FREE_HEAP_MEMORY;
		}

		/*
		 * DI_MEDIA_SetSessionProperties
		 * DI_MEDIA_RegisterSessionCallback
		 */
		s_ChannelEventCallbacks.cbTuningEvent = _local_ChannelTuningCallback; // DI_MEDIA_ChannelTuningEventCallback
		s_ChannelEventCallbacks.cbNotifyEvent = _local_ChannelNotifyCallback; // DI_MEDIA_ChannelNotifyEventCallback

		eDiRet = DI_MEDIA_RegisterChannelEventCallback(pHSession,
		                                               eTunerType,
		                                               (DI_MEDIA_ChannelEventCallback_t *)&s_ChannelEventCallbacks);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant register multicast channel event!\r\n");
			goto FREE_HEAP_MEMORY;
		}

		eDiRet = DI_MEDIA_OpenSession(pHSession);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant open multicast session!\r\n");
			goto FREE_HEAP_MEMORY;
		}

		g_bIsTuned = TRUE;
	}
	else if CMD_IS("tuneindex") // index base
	{
		/*
		 * url have multicast address range
		 *   - 224.1.2.3:1234
		 */
		if (g_bIsTuned == TRUE)
		{
			CMD_Printf("CMD>> Command : media is already started(tune:%d) \r\n",
						g_bIsTuned);
			return 0;
		}

		bUseURL = FALSE;
		pucStreamPath = HWTEST_PARAM;
		if (pucStreamPath == NULL)
		{
			CMD_Printf("CMD>> Command : multicast tune <uri> <option> \r\n");
			return 0;
		}

		pucStreamIndex = HWTEST_PARAM;
		if (pucStreamIndex == NULL)
		{
			CMD_Printf("CMD>> Command : multicast play <idx> \r\n");
			return 0;
		}
		uStreamIndex = VK_atoi(pucStreamIndex);

		eTunerType = DI_MEDIA_CHANNEL_TUNER_TYPE_AV;
		iReturn = CMD_ERR;
		switch(uStreamIndex) {
		case 0:
			eDiRet = DI_MEDIA_CreateSession((HINT8 *)"rtp://238.100.100.101:1234", &pHSession);
			break;
		case 1:
			eDiRet = DI_MEDIA_CreateSession((HINT8 *)"udp://238.100.100.102:2345", &pHSession);
			break;
		default:
			CMD_Printf("CMD>> Error> Command : multicast tuneindex <idx> \r\n");
			return 0;
		}

		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant create multicast session!\r\n");
			goto FREE_HEAP_MEMORY;
		}

		/*
		 * DI_MEDIA_SetSessionProperties
		 * DI_MEDIA_RegisterSessionCallback
		 */
		s_ChannelEventCallbacks.cbTuningEvent = _local_ChannelTuningCallback; // DI_MEDIA_ChannelTuningEventCallback
		s_ChannelEventCallbacks.cbNotifyEvent = _local_ChannelNotifyCallback; // DI_MEDIA_ChannelNotifyEventCallback

		eDiRet = DI_MEDIA_RegisterChannelEventCallback(pHSession,
		                                               eTunerType,
		                                               (DI_MEDIA_ChannelEventCallback_t *)&s_ChannelEventCallbacks);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant register multicast channel event!\r\n");
			goto FREE_HEAP_MEMORY;
		}

		eDiRet = DI_MEDIA_OpenSession(pHSession);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant open multicast session!\r\n");
			goto FREE_HEAP_MEMORY;
		}

		g_bIsTuned = TRUE;
	}
	else if CMD_IS("release")
	{
		if (g_bIsTuned == FALSE)
		{
			CMD_Printf("CMD>> ERROR> Invalid multicast status(%d)\r\n", pucStreamPath);
			return 0;
		}

		eDiRet = DI_MEDIA_CloseSession(pHSession);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant close multicast session!\r\n");
			//goto FREE_HEAP_MEMORY;
		}

		eDiRet = DI_MEDIA_UnregisterChannelEventCallback(pHSession);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant register multicast channel event!\r\n");
			//goto FREE_HEAP_MEMORY;
		}

		eDiRet = DI_MEDIA_DestroySession(pHSession);
		if (eDiRet != DI_ERR_OK)
		{
			CMD_Printf("CMD>> ERROR> Cant destroy multicast session!\r\n");
			//goto FREE_HEAP_MEMORY;
		}

		g_bIsTuned = FALSE;
	}
	else
#endif
	/*
	 * DI> multicast play 1
	 */
	if CMD_IS("play")
	{
		/*
		 * url have multicast address range
		 *   - 224.1.2.3:1234
		 */
		if (g_bIsTuned == TRUE)
		{
			CMD_Printf("CMD>> Command : media is already started(tune:%d) \r\n",
						g_bIsTuned);
			return 0;
		}

		pucStreamIndex = HWTEST_PARAM;
		if (pucStreamIndex == NULL)
		{
			CMD_Printf("CMD>> Command : multicast play <idx> \r\n");
			return 0;
		}
		pOption = HWTEST_PARAM1;
		uStreamIndex = VK_atoi(pucStreamIndex);

		iReturn = CMD_ERR;
		switch(uStreamIndex) {
		case 0:
			// _hdforum.mpg with UDP
			videopid = 0x21;
    		videotype = DI_VIDEO_FORMAT_H264;
    		audiopid = 0x24;
    		audiotype = DI_AUDFORMAT_MPEG;
    		pcrpid = 0x21;
    		CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStart()\r\n");
			iReturn = HwTest_Multicast_PlayStart((HUINT8 *)"udp://238.100.100.101:1234",
									videotype,
									audiotype,
									(HUINT16)videopid,
									(HUINT16)audiopid,
									(HUINT16)pcrpid);
			CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStart()\r\n");
			break;
		case 1:
			// _spiderman with UDP
			videopid = 0x31;
			videotype = DI_VIDEO_FORMAT_H264;
			audiopid = 0x32;
			audiotype = DI_AUDFORMAT_MPEG;
			pcrpid = 0x31;
			CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStart()\r\n");
			iReturn = HwTest_Multicast_PlayStart("udp://238.100.100.102:2345",
									videotype,
									audiotype,
									(HUINT16)videopid,
									(HUINT16)audiopid,
									(HUINT16)pcrpid);
			CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStart()\r\n");
			break;
		case 2:
			// _hdforum.mpg with RTP
			videopid = 0x21;
			videotype = DI_VIDEO_FORMAT_H264;
			audiopid = 0x24;
			audiotype = DI_AUDFORMAT_MPEG;
			pcrpid = 0x21;
			CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStart()\r\n");
			iReturn = HwTest_Multicast_PlayStart((HUINT8 *)"rtp://238.100.100.103:2345",
									videotype,
									audiotype,
									(HUINT16)videopid,
									(HUINT16)audiopid,
									(HUINT16)pcrpid);
			CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStart()\r\n");
			break;
		case 3:
			// _spiderman with RTP
			videopid = 0x31;
			videotype = DI_VIDEO_FORMAT_H264;
			audiopid = 0x32;
			audiotype = DI_AUDFORMAT_MPEG;
			pcrpid = 0x31;
			CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStart()\r\n");
			iReturn = HwTest_Multicast_PlayStart("rtp://238.100.100.104:4567",
									videotype,
									audiotype,
									(HUINT16)videopid,
									(HUINT16)audiopid,
									(HUINT16)pcrpid);
			CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStart()\r\n");
			break;
		// Insert more multicast test entries
		// ...
		// ...
		// ...
		default:
			CMD_Printf("CMD>> Error> Unsupported index number:%d\r\n", uStreamIndex);
			CMD_Printf("CMD>> Error> Command : multicast play <idx> \r\n");
			return 0;
		}

		if (iReturn != CMD_OK)
		{
			CMD_Printf("CMD>> %s(%d)\n", __FUNCTION__, __LINE__);
            return 0;
		}
		g_bIsTuned = TRUE;
	}
	/*
	 * DI> multicast test
	 */
	else if CMD_IS("test")
	{
		if (g_ulThreadID != 0)
		{
			CMD_Printf("CMD>> MULTICAST Test Start Fail:: already be started !!!\r\n");
		}
		DI_VIDEO_Show(0, 1);
		g_ulThreadID  = HwTest_Multicast_TaskCreate("MULTICAST",
						HwTest_Multicast_ZappingTestThread,
						NULL,
						64*1024,
						USER_PRIORITY12);
		g_bThreadRunning = 1;
		CMD_Printf("CMD>> MULTICAST Test Start!!!\r\n");
	}
	/*
	 * DI> multicast stop
	 */
	else if CMD_IS("stop")
	{
		if (g_ulThreadID == 0)
		{
			CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStop()\r\n");
			HwTest_Multicast_PlayStop(0);
			CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStop()\r\n");
			g_bIsTuned = FALSE;
			g_pHSession = NULL;
		}
		else
		{
			g_bThreadRunning = 0;
			CMD_Printf("CMD>> --> HwTest_Multicast_TaskDestroy()\r\n");
			HwTest_Multicast_TaskDestroy(g_ulThreadID);
			g_ulThreadID = 0;
			CMD_Printf("CMD>> MULTICAST Test END!!!\r\n");

			if (g_bIsTuned == TRUE)
			{
				CMD_Printf("CMD>> --> +HwTest_Multicast_PlayStop()\r\n");
				HwTest_Multicast_PlayStop(0);
				CMD_Printf("CMD>> --> -HwTest_Multicast_PlayStop()\r\n");
				g_bIsTuned = FALSE;
				g_pHSession = NULL;
			}
		}
	}
	else
	{
		CMD_Printf("CMD>> Command : Error! Not accepted(cmd:%d, p0:%s, p1:%d, p2:%s)\r\n", szCmd, szParam, szParam1, szParam2);
		CMD_Printf("CMD>> Command : multicast play 0\r\n");
		CMD_Printf("CMD>> Command : multicast play 1\r\n");
		CMD_Printf("CMD>> Command : multicast test\r\n");
		CMD_Printf("CMD>> Command : multicast stop\r\n");
		return 0;
	}
	return iResult;
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

