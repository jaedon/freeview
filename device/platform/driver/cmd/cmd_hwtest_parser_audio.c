#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "nexus_timebase.h"

#include "bstd_defs.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_hdmi.h"
#include "di_video.h"
#include "di_demux.h"
#include "di_audio.h"
#include "di_scart.h"
#include "di_osd.h"
#include "drv_gpio.h"
#include "drv_hdmi.h"
#include "drv_video.h"
#include "drv_audio.h"
#include "drv_osd.h"
#include "cmd_hwtest_parser_av_decode.h"
#if defined(CONFIG_FAST_AUDIO_CHANGE)
#include "di_pvr.h"
#endif

#if defined(CONFIG_DI20) && defined(CONFIG_TRANSCODER)
#include "fcntl.h"
#include "di_transcoder.h"
#include "di_pvr.h"
#define	ENABLE_TEST_TWO_TRANSCODING_ONLY_LIVE 1
#endif
#if defined(CONFIG_IPSTREAM) && defined(CONFIG_IPSTREAM_MULTICAST)
#include "drv_multicast.h"
#endif
#if defined(CONFIG_MIC)
#include "di_mic.h"
#endif
#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3	szParam3
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}


int CMD_HwTest_Parser_Audio(void *szArgs)
{
	GET_ARGS;
	HINT32 		vol;
	HINT32 		output;
	HUINT32 	delay;
	DRV_Error	dResult;
	DI_ERR_CODE diResult;

	iResult=iResult;
	szParam2=szParam2;

	if CMD_IS("beep")
	{
		CMD_Printf("Command : audio beep -> [%s]\n", (char *)szParam);
		if PARAM_IS("on")
		{
			if(DRV_AUDIO_BeepPlay(TRUE) == DRV_OK)
				return 0;
			else
			{
                if ( CMD_GetMsgUI() != NULL )
                {
                    (int *)(CMD_GetMsgUI())("Audio Beep Play failed\n");
                }
				CMD_Printf("DRV_AUDIO_BeepPlay failed!\n");
			}
		}
		else if PARAM_IS("off")
		{
			if(DRV_AUDIO_BeepPlay(FALSE) == DRV_OK)
				return 0;
			else
			{
                if ( CMD_GetMsgUI() != NULL )
                {
                    (int *)(CMD_GetMsgUI())("Audio Beep Stop failed\n");
                }
				CMD_Printf("DRV_AUDIO_BeepPlay failed!\n");
			}
		}
		else
			CMD_Printf("Wrong parameter! [on / off]\n");

	}
	else if CMD_IS("vol")
	{
		/* Volume Control Test */
		CMD_Printf("Command : audio volume [decoder: %s] [volume: %s]\n", (char *)szParam, (char *)szParam1);

		if(szParam == NULL || szParam1 == NULL)
		{
			CMD_Printf("Choose the correct decoder[0~2] and volume value [0~100]\n");
			return 0;
		}

		vol = VK_atoi((char *)szParam1);
		output = VK_atoi((char *)szParam);
		if(output<0 || output > 2)
		{
			CMD_Printf("Out of range, choose a correct decoder [0~2]\n");
			return 0;
		}
#if defined(CONFIG_DI10)
		if(vol >20 || vol <0)
		{
			CMD_Printf("%d is out of volume range! Choose [0~20]\n", vol);
			return 0;
		}
#else
		if(vol >100 || vol <0)
		{
			CMD_Printf("%d is out of volume range! Choose [0~100]\n", vol);
			return 0;
		}
#endif
		if(DI_AUDIO_SetVolume(output, vol) == DI_ERR_OK)
			return 0;
		else
			CMD_Printf("%s failed!\n", __FUNCTION__);
	}
	else if CMD_IS("decvol")
	{
		HUINT32 ulDeviceId;		
		if(READABLE_IN_DEC(HWTEST_PARAM,ulDeviceId) && READABLE_IN_DEC(HWTEST_PARAM1,vol))
		{
			diResult = DI_AUDIO_SetDecoderVolume(ulDeviceId,vol);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("%s failed!\n", __FUNCTION__);
			}
			else
			{
				CMD_Printf("Set Decoder(%d) Volume(%d)\n",ulDeviceId, vol);
				return 0;
			}
		}
		else
		{
			CMD_Printf("wrong command!!\n");
			CMD_Printf("usage : audio decvol [decoder id] [volume value (0~100)]\n");
		}
	}
	else if CMD_IS("mode")
	{
		/* Output Mode Test */
		if PARAM_IS("0")
		{
			CMD_Printf("Command : audio mode [stereo]\n");
			if(DI_AUDIO_SetStereo(DI_AUDIO_DECODER_MAIN, DI_AUDIO_DUAL_Stereo) == DI_ERR_OK)
				return 0;
			else
				CMD_Printf("%s failed!\n", __FUNCTION__);
		}
		else if PARAM_IS("1")
		{
			CMD_Printf("Command : audio mode [mono]\n");
			if(DI_AUDIO_SetStereo(DI_AUDIO_DECODER_MAIN, DI_AUDIO_DUAL_Mix) == DI_ERR_OK)
				return 0;
			else
				CMD_Printf("%s failed!\n", __FUNCTION__);
		}
		else if PARAM_IS("2")
		{
			CMD_Printf("Command : audio mode [left]\n");
			if(DI_AUDIO_SetStereo(DI_AUDIO_DECODER_MAIN, DI_AUDIO_DUAL_LeftOnly) == DI_ERR_OK)
				return 0;
			else
				CMD_Printf("%s failed!\n", __FUNCTION__);
		}
		else if PARAM_IS("3")
		{
			CMD_Printf("Command : audio mode [right]\n");
			if(DI_AUDIO_SetStereo(DI_AUDIO_DECODER_MAIN, DI_AUDIO_DUAL_RightOnly) == DI_ERR_OK)
				return 0;
			else
				CMD_Printf("%s failed!\n", __FUNCTION__);
		}
		else
		{
			CMD_Printf("%s Wrong parameters, audio mode [stereo / mono / left / right]\n", __FUNCTION__);
		}
	}
	else if CMD_IS("mute")
	{
		/* Mute Function Test */
		CMD_Printf("Command : audio mute [output: %s] [mute: %s]\n", (char *)szParam, (char *)szParam1);
		if(szParam == NULL || szParam1 == NULL)
		{
			CMD_Printf("Choose the correct decoder[0~2] and mute option [on/off]\n");
			return 0;
		}

		output = VK_atoi((char *)szParam);
		if(output == 0)
		{
			output = DI_AUDIO_OUTPUT_ALL;
		}
		else if(output == 1)
		{
			output = DI_AUDIO_OUTPUT_DAC;
		}
		else if(output == 2)
		{
			output = DI_AUDIO_OUTPUT_SPDIF;
		}
		else if(output == 4)
		{
			output= DI_AUDIO_OUTPUT_HDMI;
		}
		else if(output == 64)
		{
			output = DI_AUDIO_OUTPUT_SCART_2;
		}
		else
		{
			CMD_Printf("Out of range, choose one output [0: ALL 1:DAC 2: SPDIF 4: HDMI 64: VCR Scart]\n");
			return 0;
		}

		if PARAM1_IS("on")
		{
			if(DI_AUDIO_SetMute(output, TRUE) == DI_ERR_OK)
				return 0;
			else
				CMD_Printf("%s failed!\n", __FUNCTION__);
		}
		else if PARAM1_IS("off")
		{
			if(DI_AUDIO_SetMute(output, FALSE) == DI_ERR_OK)
				return 0;
			else
				CMD_Printf("%s failed!\n", __FUNCTION__);
		}
		else
		{
			CMD_Printf("Wrong parameters, audio mute [on / off]\n");
		}
	}
	else if CMD_IS("info")
	{
		/* Print Audio module Information */
		CMD_Printf("Command : audio info\n");
		if(DRV_AUDIO_PrintInfo() == DRV_OK)
			return 0;
		else
			CMD_Printf("%s failed!\n",__FUNCTION__);
	}

#if 1
	else if CMD_IS("sync")
	{
		CMD_Printf("Command: audio sync\n");
		if PARAM_IS("on")
		{
			dResult = DRV_AUDIO_ChangeSyncMode(DI_AUDIO_DECODER_MAIN, TRUE);
			if(dResult != DRV_OK)
			{
				CMD_Printf("DRV_AUDIO_ChangeSyncMode faild!\n");
			}
		}
		else if PARAM_IS("off")
		{
			dResult = DRV_AUDIO_ChangeSyncMode(DI_AUDIO_DECODER_MAIN, FALSE);
			if(dResult != DRV_OK)
			{
				CMD_Printf("DRV_AUDIO_ChangeSyncMode faild!\n");
			}
		}
		return 0;
	}
	else if CMD_IS("drum")
	{
		FILE* pfile;
		unsigned char* buf;
		buf = (unsigned char *)DD_MEM_Alloc(5486);
		pfile = fopen("drum.mpg", "rb");
		if(pfile == NULL)
	 	{
	 		CMD_Printf("There's no file!\n");
			return 0;
		}
		fread(buf, 1, 5486, pfile);
		diResult = DI_AUDIO_StartMemoryStream(99, DI_AUDFORMAT_MPEG, buf, 5486, 10, DI_AUDIO_OUTPUT_DAC);
		if(diResult != DI_ERR_OK)
		{
			CMD_Printf("DI_AUDIO_StartMemoryStream failed!\n");
		}

		return 0;
	}
	else if CMD_IS("pcm")
	{
		FILE* pfile;
		unsigned char* buf;

		if PARAM_IS("on")
		{
			pfile = fopen("butterfly.wav", "rb");
			if(pfile == NULL)
		 	{
		 		CMD_Printf("There's no file!\n");
				return 0;
			}

			buf = (unsigned char *)DD_MEM_Alloc(44100*4*20);
			fread(buf, 1, 44100*4*20, pfile);
			diResult = DI_AUDIO_StartMemoryStream(123, DI_AUDFORMAT_PCM, buf, 44100*4*20, 1, DI_AUDIO_OUTPUT_DAC);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_StartMemoryStream failed!\n");
			}
            fclose(pfile);
            DD_MEM_Free(buf);

			return 0;
		}
		else if PARAM_IS("off")
		{
			diResult = DI_AUDIO_StopMemoryStream(123);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_StopMemoryStream failed!\n");
			}

			//DD_MEM_Free(buf);

			return 0;
		}
	}
	else if CMD_IS("mp3")
	{
		FILE* pfile = NULL;
		unsigned char* buf = NULL;

		CMD_Printf("Command: audio mp3\n");

		if PARAM_IS("on")
		{
			 pfile = fopen("test.mp3", "rb");
			 if(pfile == NULL)
		 	{
		 		CMD_Printf("There's no file!\n");
				return 0;
		 	}
			buf = (unsigned char *)DD_MEM_Alloc(48000*4*20);
			fread(buf, 1, 48000*4*20, pfile);
			diResult = DI_AUDIO_StartMemoryStream(446, DI_AUDFORMAT_MP3, buf, 48000*4*20, 1, DI_AUDIO_OUTPUT_DAC);
		    DD_MEM_Free(buf);
            fclose(pfile);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_StartMemoryStream failed!\n");
			}
			return 0;
		}
		else if PARAM_IS("off")
		{
			diResult = DI_AUDIO_StopMemoryStream(446);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_StopMemoryStream failed!\n");
			}

			//DD_MEM_Free(buf);

			return 0;
		}
		else if PARAM_IS("pause")
		{
			diResult = DI_AUDIO_PauseMemoryStream(446, TRUE);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_PauseMemoryStream failed!\n");
			}
			return 0;
		}
		else if PARAM_IS("resume")
		{
			diResult = DI_AUDIO_PauseMemoryStream(446, FALSE);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_PauseMemoryStream failed!\n");
			}
			return 0;
		}
	}

#if defined(CONFIG_MIC)
	else if CMD_IS("mic")
	{

		CMD_Printf("Command: audio mic\n");

		if PARAM_IS("start")
		{
			diResult = DI_MIC_Start();
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_MIC_Start failed!\n");
			}
			return 0;
		}
		else if PARAM_IS("stop")
		{
			diResult = DI_MIC_Stop();
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_MIC_Stop failed!\n");
			}
			return 0;
		}
	}
#endif

#endif
	else if CMD_IS("callback")
	{
		DRV_AUDIO_CallbackCheck();
		return 0;
	}
	else if CMD_IS("start")
	{
		if PARAM_IS("main")
		{
		CMD_Printf("Command : audio start\n");
		if(DI_AUDIO_Start(0, DI_AUDFORMAT_MPEG) != DI_ERR_OK)
		{
			CMD_Printf("audio start failed!\n");
			return 0;
		}
	}
		else if PARAM_IS("sub")
		{
			CMD_Printf("Command : audio start\n");
			if(DI_AUDIO_Start(1, DI_AUDFORMAT_MPEG) != DI_ERR_OK)
			{
				CMD_Printf("audio start failed!\n");
				return 0;
			}
		}
	}
	else if CMD_IS("stop")
	{
		CMD_Printf("Command : audio stop\n");
		if PARAM_IS("main")
		{
			if(DI_AUDIO_Stop(0) !=DI_ERR_OK)
			{
				CMD_Printf("audio stop failed!\n");
				return 0;
			}
		}
		else if PARAM_IS("sub")
		{
			if(DI_AUDIO_Stop(1) !=DI_ERR_OK)
			{
				CMD_Printf("audio stop failed!\n");
				return 0;
			}
		}
		else if PARAM_IS("spdif")
		{
			if(DI_AUDIO_Stop(2) !=DI_ERR_OK)
			{
				CMD_Printf("audio stop failed!\n");
				return 0;
			}
		}
		else
			CMD_Printf("Wrong option! [main / spdif]\n");
	}
	else if CMD_IS("delay")
	{
		CMD_Printf("Command : audio delay\n");

		if(szParam == NULL)
		{
			CMD_Printf("Delay amount needed![0~10000]\n");
			return 0;
		}
		delay = VK_atoi((char *)szParam);
		CMD_Printf("Delay: %d\n", delay);

		if(delay <= 100000)
		{
			diResult = DI_AUDIO_DelayOutput(DI_AUDIO_DECODER_MAIN, delay);
			if(diResult == DI_ERR_ERROR)
			{
				CMD_Printf("DI_AUDIO_DelayOutput failed!\n");
				return 0;
			}
		}
		else
			CMD_Printf("Wrong option! [0~100000]\n");
	}
	else if CMD_IS("spdif")
	{
		CMD_Printf("Command : audio dout\n");

		if PARAM_IS("pcm")
		{
#if defined(CONFIG_DI10)
			diResult = DI_AUDIO_SetTranscoding(DI_AUDFORMAT_NONE);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSpdifOutput failed!\n");
				return 0;
			}
#else
			diResult = DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT_SPDIF, FALSE, DI_AUDFORMAT_NONE);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSpdifOutput failed!\n");
				return 0;
			}
#endif
		}
		else if PARAM_IS("bypass")
		{
#if defined(CONFIG_DI10)
			diResult = DI_AUDIO_SetTranscoding(DI_AUDFORMAT_AC3);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_COMPRESSED);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSpdifOutput failed!\n");
				return 0;
			}
#else
			diResult = DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT_SPDIF, FALSE, DI_AUDFORMAT_NONE);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_BYPASS);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSpdifOutput failed!\n");
				return 0;
			}
#endif
		}
		else if PARAM_IS("trans")
		{
#if defined(CONFIG_DI10)
			diResult = DI_AUDIO_SetTranscoding(DI_AUDFORMAT_AC3);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_COMPRESSED);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSpdifOutput failed!\n");
				return 0;
			}
#else
			diResult = DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT_SPDIF, TRUE, DI_AUDFORMAT_AC3);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_TRANSCODING);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSpdifOutput failed!\n");
				return 0;
			}
#endif
		}
		else
		{
			CMD_Printf("Wrong option![pcm / bypass]\n");
			return 0;
		}

	}
	else if CMD_IS("hdmi")
	{
		CMD_Printf("Command : audio dout\n");

		if PARAM_IS("pcm")
		{
#if defined(CONFIG_DI10)
			diResult = DI_AUDIO_SetTranscoding(DI_AUDFORMAT_NONE);
#else
			diResult = DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT_HDMI, FALSE, DI_AUDFORMAT_NONE);
#endif
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetHdmiOutput failed!\n");
				return 0;
			}
		}
		else if PARAM_IS("bypass")
		{
#if defined(CONFIG_DI10)
			diResult = DI_AUDIO_SetTranscoding(DI_AUDFORMAT_NONE);
#else
			diResult = DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT_HDMI, FALSE, DI_AUDFORMAT_NONE);
#endif
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_BYPASS);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetHdmiOutput failed!\n");
				return 0;
			}
		}
		else if PARAM_IS("trans")
		{
#if defined(CONFIG_DI10)
			diResult = DI_AUDIO_SetTranscoding(DI_AUDFORMAT_AC3);
#else
			diResult = DI_AUDIO_SetTranscoding(DI_AUDIO_OUTPUT_HDMI, TRUE, DI_AUDFORMAT_AC3);
#endif
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetTranscoding failed!\n");
				return 0;
			}

			diResult = DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_TRANSCODING);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetHdmiOutput failed!\n");
				return 0;
			}
		}
		else
		{
			CMD_Printf("Wrong option![pcm / bypass]\n");
			return 0;
		}
	}
	else if CMD_IS("scms")
	{
		if PARAM_IS("nocopy")
		{
			diResult = DI_AUDIO_SetSCMS(DI_AUDOUT_COPYRIGHT_NO_COPY);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSCMS failed!\n");
			}
		}
		else if PARAM_IS("onecopy")
		{
			diResult = DI_AUDIO_SetSCMS( DI_AUDOUT_COPYRIGHT_ONE_COPY);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSCMS failed!\n");
			}
		}
		else if PARAM_IS("allcopy")
		{
			diResult = DI_AUDIO_SetSCMS( DI_AUDOUT_COPYRIGHT_NO_RESTRICTION);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_SetSCMS failed!\n");
			}
		}
		#if 0
		else if PARAM_IS("test")
		{
			HBOOL bCopyRight;
			HUINT16 categoryCode;

			if((szParam1 == NULL) || (szParam2 == NULL))
			{
				CMD_Printf("parameter is null!\n");
				return 0;
			}
			bCopyRight = VK_atoi((char *)szParam1);
			categoryCode = VK_atoi((char *)szParam2);
			CMD_Printf("bCopyRight: %d categoryCode: %d\n", bCopyRight, categoryCode);

			dResult = DRV_AUDIO_SetSCMS(bCopyRight, categoryCode);
			if(dResult != DRV_OK)
			{
				CMD_Printf("DRV_AUDIO_SetSCMS failed!\n");
			}
		}
		#endif
		return 0;
	}
  else if CMD_IS("hwmute")
  {
		if PARAM_IS("on")
		{
			if(DRV_AUDIO_HWMUTE(TRUE) == DRV_OK)
				return 0;
			else
			{
                CMD_Printf("error");
			}
		}
        else if PARAM_IS("off")
	    {
			if(DRV_AUDIO_HWMUTE(FALSE) == DRV_OK)
				return 0;
			else
			{
                CMD_Printf("error");
			}
		}
    }
#if defined(CONFIG_DI20)
	else if CMD_IS("flush")
	{
		if PARAM_IS("main")
		{
			dResult = DI_AUDIO_Flush(DI_AUDIO_DECODER_MAIN);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_Flush failed!\n");
			}
		}
        else if PARAM_IS("sub")
	    {
			dResult = DI_AUDIO_Flush(DI_AUDIO_DECODER_SUB);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("DI_AUDIO_Flush failed!\n");
			}
		}

		return 0;
	}
#endif
#if defined(CONFIG_DI20) && defined(CONFIG_TRANSCODER)
    else if CMD_IS("encinfo")
    {
        dResult = DRV_AUDIO_MuxOutputShowStatus(DI_AUDIO_DECODER_MAIN);
        if(dResult != DI_ERR_OK)
        {
            CMD_Printf("DRV_AUDIO_MuxOutput_GetStatus failed!\n");
        }

        return 0;
    }
#endif
#if defined(CONFIG_FAST_AUDIO_CHANGE)
	else if CMD_IS("fpb")
	{
		#define TOTAL_TRACK_COUNT CONFIG_PRIMER_COUNT
		//#define USE_NORMAL_PLAY

		int i = 0, ch = 0;
		int buffer[128];
		HUINT32 ulDemuxId;
		HINT32 ulPreviousAudio, ulPreviousPrimer;
		HINT32 ulCurrentAudio, ulCurrentPrimer;
		HINT32 ulNextAudio, ulNextPrimer;
		HINT32 ulNewAudio, ulNewPrimer;
		DI_PVR_PLAY_SETUP_t playSetup;
		DI_PVR_PLAY_SETUP_t *stSetup;
		PrimerPid_Info primerPidInfo;
		Pid_Info pidInfo;
		DI_AUDIO_PRIMER_t multiTrack;
		HUINT32 decoderCnt;
		DI_AUDIO_CAP_t *p_stAudCapInfo;
		DI_AUDIO_FORMAT eFormat[TOTAL_TRACK_COUNT] = {DI_AUDFORMAT_AAC_PLUS, DI_AUDFORMAT_AAC_PLUS, DI_AUDFORMAT_AAC_PLUS};
		HUINT16 usPid[TOTAL_TRACK_COUNT] = {260, 261, 265};

		DI_VIDEO_Stop(0);
		DI_AUDIO_Stop(0);
		DI_DEMUX_StopPID(0, PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR);
		DI_DEMUX_ReleasePath(0);

		VK_memset(&pidInfo, 0, sizeof(Pid_Info));
		VK_memset(buffer, 0, 128);
		VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
		VK_snprintf(buffer, 127, "/mnt/hd2/AV_20080411_1100\0"); //DTG Testing stream
		VK_memcpy((void *)playSetup.aucFileName, (void *)buffer, VK_strlen((char *)buffer)+1);
		playSetup.bIsRecording = false;
		playSetup.eFileType = eDI_PVR_FileType_Linear;
		playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		playSetup.eStreamType = eDI_PVR_Stream_Video;
		playSetup.ulStartTime = 0;

		stSetup = &playSetup;
		ulDemuxId = DRV_DEMUX_GetFirstPlaybackParserIndex();

		DI_DEMUX_SetPath(ulDemuxId, DI_DEMUX_INPUT_PLAYER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);

#if defined(CONFIG_DI20)
		stSetup->ulDemuxId = ulDemuxId;
#endif
		DI_PVR_PLAY_Setup(0, stSetup);
#if defined(USE_NORMAL_PLAY)
		/* Demux setting */
		ulCurrentAudio = 0;
		pidInfo.pidBitmask = PID_BITMASK_VIDEO | PID_BITMASK_PCR | PID_BITMASK_AUDIO_MAIN;
		pidInfo.usVideoPid = 258;
		pidInfo.usPCRPid = 280;
		pidInfo.usAudioMainPid = usPid[ulCurrentAudio];
		DI_DEMUX_StartPID(ulDemuxId, &pidInfo);

		/* Audio path */
#if defined(CONFIG_DI20)
		DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
		DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
		DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

		/* Start A/V */
		DI_AUDIO_Start(0, eFormat[ulCurrentAudio]);
		DI_VIDEO_Start(0, DI_VIDEO_FORMAT_H264);
		DI_PVR_PLAY_Start(0);
		VK_TASK_Sleep(100);
		DI_VIDEO_Show(0,1);

		ch = '0';
		while(ch != 'z')
		{
			ch = VK_getchar();

			if(ch == 'n')
			{
				DI_AUDIO_Stop(0);
				DI_DEMUX_StopPID(ulDemuxId, PID_BITMASK_AUDIO_MAIN);

				ulCurrentAudio++;
				if(ulCurrentAudio >= TOTAL_TRACK_COUNT)
				{
					ulCurrentAudio = 0;
				}
				pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
				pidInfo.usAudioMainPid = usPid[ulCurrentAudio];
				DI_DEMUX_StartPID(ulDemuxId, &pidInfo);

#if defined(CONFIG_DI20)
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
				DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
				DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);
				DI_AUDIO_Start(0, eFormat[ulCurrentAudio]);
			}
		}
#else
		/* Demux setting */
		/* Video & PCR */
		pidInfo.pidBitmask = PID_BITMASK_VIDEO | PID_BITMASK_PCR;
		pidInfo.usVideoPid = 258;
		pidInfo.usPCRPid = 280;
		DI_DEMUX_StartPID(ulDemuxId, &pidInfo);

		/* Audio primers */
		primerPidInfo.ulPrimerIndex = 0;
		primerPidInfo.ulPidBitmask = PID_BITMASK_AUDIO_PRIMER;
		primerPidInfo.usAudioPid = usPid[0];
		DI_DEMUX_StartPrimerPID(ulDemuxId, &primerPidInfo);

		primerPidInfo.ulPrimerIndex = 1;
		primerPidInfo.ulPidBitmask = PID_BITMASK_AUDIO_PRIMER;
		primerPidInfo.usAudioPid = usPid[1];
		DI_DEMUX_StartPrimerPID(ulDemuxId, &primerPidInfo);

		primerPidInfo.ulPrimerIndex = 2;
		primerPidInfo.ulPidBitmask = PID_BITMASK_AUDIO_PRIMER;
		primerPidInfo.usAudioPid = usPid[2];
		DI_DEMUX_StartPrimerPID(ulDemuxId, &primerPidInfo);


		/* Audio path */
#if defined(CONFIG_DI20)
		DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
#else
		DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_PCM);
#endif
		DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

		/* Audio primer */
		DI_AUDIO_GetCapability(&decoderCnt, &p_stAudCapInfo);

		/* Force test by 3 primer */
		p_stAudCapInfo[DI_AUDIO_DECODER_MAIN].ulPrimerCount = 3;

		for(i=0; i < p_stAudCapInfo[DI_AUDIO_DECODER_MAIN].ulPrimerCount; i++)
		{
			multiTrack.ulPrimerIndex = i;
			multiTrack.eAudioType = DI_AUDIOTYPE_PLAYBACK;
			multiTrack.eFormat = eFormat[i];
			DI_AUDIO_StartPrimer(0, &multiTrack);
		}

		ulCurrentAudio = 1;
		ulCurrentPrimer = 1;
		DI_AUDIO_SelectPrimer(0, ulCurrentPrimer);
		DI_AUDIO_Start(0, eFormat[ulCurrentAudio]);

		DI_VIDEO_Start(0, DI_VIDEO_FORMAT_H264);

		DI_PVR_PLAY_Start(0);

		VK_TASK_Sleep(100);

		DI_VIDEO_Show(0,1);

		ch = '0';
		while(ch != 'z')
		{
			ch = VK_getchar();

			if(ch == 'n')
			{
				/* Stop Current audio */
				DI_AUDIO_SelectPrimer(0, ulCurrentPrimer);
				DI_AUDIO_Stop(0);
				CMD_Printf("Stop Current audio \n");
				CMD_Printf("ulCurrentAudio=%d ulCurrentPrimer=%d \n", ulCurrentAudio, ulCurrentPrimer);


				/* Next audio index info */
				ulNextPrimer = ulCurrentPrimer+1;
				if(ulNextPrimer >= p_stAudCapInfo[DI_AUDIO_DECODER_MAIN].ulPrimerCount)
				{
					ulNextPrimer = 0;
				}

				ulNextAudio = ulCurrentAudio+1;
				if(ulNextAudio >= TOTAL_TRACK_COUNT)
				{
					ulNextAudio = 0;
				}

				/* Start Next audio */
				DI_AUDIO_SelectPrimer(0, ulNextPrimer);
				DI_AUDIO_Start(0, eFormat[ulNextAudio]);

				CMD_Printf("Start Next audio \n");
				CMD_Printf("ulNextAudio=%d ulNextPrimer=%d \n", ulNextAudio, ulNextPrimer);

				/* Previous primer index info */
				ulPreviousPrimer = ulCurrentPrimer-1;
				if(ulPreviousPrimer < 0)
				{
					ulPreviousPrimer = p_stAudCapInfo[DI_AUDIO_DECODER_MAIN].ulPrimerCount-1;
				}

				ulPreviousAudio = ulCurrentAudio-1;
				if(ulPreviousAudio < 0)
				{
					ulPreviousAudio = TOTAL_TRACK_COUNT-1;
				}

				/* Free previous primer */
				multiTrack.ulPrimerIndex = ulPreviousPrimer;
				multiTrack.eAudioType = DI_AUDIOTYPE_PLAYBACK;
				multiTrack.eFormat = eFormat[ulPreviousAudio];
				DI_AUDIO_StopPrimer(0, &multiTrack);

				primerPidInfo.ulPrimerIndex = ulPreviousPrimer;
				primerPidInfo.ulPidBitmask = PID_BITMASK_AUDIO_PRIMER;
				primerPidInfo.usAudioPid = usPid[ulPreviousAudio];
				DI_DEMUX_StopPrimerPID(ulDemuxId, &primerPidInfo);

				CMD_Printf("Free previous primer \n");
				CMD_Printf("ulPreviousAudio=%d ulPreviousPrimer=%d \n", ulPreviousAudio, ulPreviousPrimer);

				/* Start New primer index info */
				ulNewPrimer = ulNextPrimer+1;
				if(ulNewPrimer >= p_stAudCapInfo[DI_AUDIO_DECODER_MAIN].ulPrimerCount)
				{
					ulNewPrimer = 0;
				}

				ulNewAudio = ulNextAudio+1;
				if(ulNewAudio >= TOTAL_TRACK_COUNT)
				{
					ulNewAudio = 0;
				}

				/* Start New primer */
				primerPidInfo.ulPrimerIndex = ulNewPrimer;
				primerPidInfo.ulPidBitmask = PID_BITMASK_AUDIO_PRIMER;
				primerPidInfo.usAudioPid = usPid[ulNewAudio];
				DI_DEMUX_StartPrimerPID(ulDemuxId, &primerPidInfo);

				multiTrack.ulPrimerIndex = ulNewPrimer;
				multiTrack.eAudioType = DI_AUDIOTYPE_PLAYBACK;
				multiTrack.eFormat = eFormat[ulNewAudio];
				DI_AUDIO_StartPrimer(0, &multiTrack);
				CMD_Printf("Allocate new primer \n");
				CMD_Printf("ulNewAudio=%d ulNewPrimer=%d \n", ulNewAudio, ulNewPrimer);

				/* Update current info */
				ulCurrentPrimer = ulNextPrimer;
				ulCurrentAudio = ulNextAudio;

				CMD_Printf("Update current info \n");
				CMD_Printf("ulCurrentAudio=%d ulCurrentPrimer=%d \n", ulCurrentAudio, ulCurrentPrimer);
			}
		}
#endif
	}
#endif

#if defined(TEMP_HRA_SELF_TEST)
	else if CMD_IS("hra")
	{
		bool bDac = FALSE;
		int addr = 0;
		int data = 0;
		HUINT8 ucData =0;
		char* szParm3;
		char input;
		int track_num;

		CMD_Printf("Command: High resolution audio \n");

		if PARAM_IS("on")
		{
#if defined(CONFIG_DI10)
#define AUDIO_VOL_LEVEL_MAX		20
#else
#define AUDIO_VOL_LEVEL_MAX		100
#endif
			DI_VIDEO_Stop(0);
			DI_AUDIO_Stop(0);
			DI_DEMUX_StopPID(0, PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR);
			DI_DEMUX_ReleasePath(0);

			DI_AUDIO_SetSpdifOutput(DI_AUDOUT_SPDIF_DOWNMIX);
			DI_AUDIO_SetHdmiOutput(DI_AUDOUT_HDMI_DOWNMIX);

			DRV_AUDIO_GetHraTrackList();
			DRV_AUDIO_ShowHraTrackList();
			CMD_Printf("Select track index ==> ");
			input = VK_getchar();

			track_num = VK_atoi((char *)&input);
			diResult = DRV_AUDIO_StartHraTrack(track_num);
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DRV_AUDIO_StartHraTrack failed!\n");
			}
			DI_AUDIO_SetVolume(DI_AUDIO_OUTPUT_I2S, AUDIO_VOL_LEVEL_MAX);
			DI_AUDIO_SetMute(DI_AUDIO_OUTPUT_I2S, FALSE);

			return 0;
		}
		else if PARAM_IS("off")
		{
			DI_AUDIO_SetMute(DI_AUDIO_OUTPUT_I2S, TRUE);
			diResult = DRV_AUDIO_StopHraTrack();
			if(diResult != DI_ERR_OK)
			{
				CMD_Printf("DRV_AUDIO_StopHraTrack failed!\n");
			}

			return 0;
		}

		else if PARAM_IS("r")
		{
			DRV_AUDIO_HRA_Print();
			return CMD_OK;
		}
		else if PARAM_IS("dac")
		{
			bDac = TRUE;
		}
		else if PARAM_IS("src")
		{
			bDac = FALSE;
		}
		else
		{
			return CMD_ERR;
		}

		if(READABLE_IN_HEX(HWTEST_PARAM1, addr) && 	READABLE_IN_HEX(HWTEST_PARAM2, data))
		{
			ucData = (HUINT8)data;
			if(bDac)
			{
				CMD_Printf("Writing to DAC \nAddr: %02x, Data: %02x\n", addr, ucData);
				P_AUDIO_HRA_DAC_Write((HUINT8)addr, &ucData, 1);
			}
			else
			{
				CMD_Printf("Writing to Sampling Rate Converter \nAddr: %02x, Data: %02x\n", addr, ucData);
				P_AUDIO_HRA_SRC_Write((HUINT8)addr, &ucData, 1);
			}
		}
	}
#endif
#if defined(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC)
	else if CMD_IS("exthra")
	{
		CMD_Printf("Command: High resolution audio \n");

		if PARAM_IS("on")
		{
			DI_AUDIO_SetHraOutput(TRUE);
		}
		else if PARAM_IS("off")
		{
			DI_AUDIO_SetHraOutput(FALSE);
		}
		else
		{
			CMD_Printf("Command: Wrong External High resolution audio \n");
		}
	}
#endif
#if defined(CONFIG_MIC)
	else if CMD_IS("decv")
	{
		int volume;
		volume = VK_atoi((char *)szParam);
		DI_AUDIO_SetDecoderVolume(0,volume);
		return 0;
	}
	else if CMD_IS("micm")
	{
		int volume;
		volume = VK_atoi((char *)szParam);
		DI_MIC_SetMixerVolume(volume);
		return 0;
	}
	else if CMD_IS("micv")
	{
		unsigned int volume;
		volume = VK_atoi((char *)szParam);
		DI_MIC_SetVolume(volume);
		return 0;
	}
#endif
	else
	{
		CMD_Printf("Wrong command format! [command: beep / vol / mode / mute / info]\n");
	}
	return 0;
}



