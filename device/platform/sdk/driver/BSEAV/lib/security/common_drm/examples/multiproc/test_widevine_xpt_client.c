/******************************************************************************
 *    (c)2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: test_widevine_xpt_client.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/16/12 4:41p $
 *
 * Module Description:
 *
 * Revision History:
 *		20120424 created
 *
 * $brcm_Log: /BSEAV/lib/security/common_drm/examples/multiproc/test_widevine_xpt_client.c $
 * 
 * 1   5/16/12 4:41p cdisc
 * SW7425-3067: adding multiprocess test app
 * 
 *****************************************************************************/
/* Nexus multiproc client example app: playback and decode CA protected stream. */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_hdmi_output.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_security.h"
#include <string.h> /* for memcpy */

#include "refsw_session_simple_client.h"
#include "client_auth.h"

#include "drm_widevine.h"
#include "drm_init.h"

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(test_widevine_xpt_client);

/* N.B.: I think I can(/will) live without the next - -d option shall handle it ?!*/
#define TEST_PLAT 
/* or don't, i.e. TEST_CARD - remove the prev #define */

#ifdef TEST_PLAT
#define PLAT_DRM_BIN_FILE_NAME "WIDEVINE_SETTOP_KEYBOX_00000000.bin"
#endif

static void
print_usage(void)
{
    printf(
    "usage: nexus test_widevine_xpt_client [--help|-h] { 1 | 2 | 3 | \\ \n"
    "             -f file_name [-d [<DRM_file_name>]] \\ \n"
    "			  -c <videocodec>,<audiocodec> \\ \n"
    "             -a <aud_pid> -v <vid_pid>} \n\n"
    "options:\n"
    "  --help|-h      print this help screen\n\n"
    "  1-3 - plays first, second, or third of:\n");
    /* Yeah, C90 compiler limits... have to deal with several printf-s:-() */
    printf(
    "    'outfile.mpg' (encrypted: NEXUS_VideoCodec_eH264, VIDEO_PID 0x120,\n"
    "                   AUDIO_PID 0x131, WIDEVINE_SETTOP_KEYBOX_00000000.bin),\n"
    "    'cnnticker.mpg' (clear: NEXUS_VideoCodec_eMpeg2,\n"
    "                     VIDEO_PID 0x21, AUDIO_PID 0x22),\n"
    "    'spiderman_aes.ts' (clear: NEXUS_VideoCodec_eMpeg2,\n"
    "                        VIDEO_PID 0x11, AUDIO_PID 0x14),\n");
    printf(
    "  -f file_name - other than the listed above file (relative or absolute name)\n"
    "     in such case some or all of the following arguments shall be provided:\n"
    "  -V, -A - video and audio codecs (e.g. h264, mpeg2, ... - video;\n"
    "                                   mpeg, ac3, mp3, ... audio)\n"
    "  -v, -a - video PID, audio PID (note the lowercase for PID-s!)\n"
    "  -d - keybox file name, optional: if file name omitted - using keys from flash,\n"
    "       when <file_name> is in clear, -d shall be omitted as well\n\n");
    printf(
    "watch and report any suspicious activity, or otherwise enjoy...\n\n"
    "Note: when no command line arguments are given, default is '1'\n;"
    "    -f <file_name>, -d, etc. override the preset (in 1-3) ones\n");
}

typedef struct namevalue_t {
    const char *name;
    int namelen;
    int value;
} NameValue;

static NEXUS_VideoCodec
GetVideoCodec(char* codecName)
{
	static const NameValue g_videoCodecStrs[] = {
		{"avc", sizeof("avc") - 1, NEXUS_VideoCodec_eH264},
		{"avs", sizeof("avs") - 1, NEXUS_VideoCodec_eAvs},
		{"divx311", sizeof("divx311") - 1, NEXUS_VideoCodec_eDivx311},
		{"divx3", sizeof("divx3") - 1, NEXUS_VideoCodec_eDivx311},
		{"divx", sizeof("divx") - 1, NEXUS_VideoCodec_eMpeg4Part2},
		{"h263", sizeof("h263") - 1, NEXUS_VideoCodec_eH263},
		{"h264", sizeof("h264") - 1, NEXUS_VideoCodec_eH264},
		{"mjpeg", sizeof("mjpeg") - 1, NEXUS_VideoCodec_eMotionJpeg},
		{"mpeg4", sizeof("mpeg4") - 1, NEXUS_VideoCodec_eMpeg4Part2},
		{"mpeg2", sizeof("mpeg2") - 1, NEXUS_VideoCodec_eMpeg2},
		{"mpeg1", sizeof("mpeg1") - 1, NEXUS_VideoCodec_eMpeg1},
		{"mpeg", sizeof("mpeg") - 1, NEXUS_VideoCodec_eMpeg2},
		{"mvc", sizeof("mvc") - 1, NEXUS_VideoCodec_eH264_Mvc},
		{"rv40", sizeof("rv40") - 1, NEXUS_VideoCodec_eRv40},
		{"spark", sizeof("spark") - 1, NEXUS_VideoCodec_eSpark},
		{"svc", sizeof("svc") - 1, NEXUS_VideoCodec_eH264_Svc},
		{"vc1sm", sizeof("vc1sm") - 1, NEXUS_VideoCodec_eVc1SimpleMain},
		{"vc1", sizeof("vc1") - 1, NEXUS_VideoCodec_eVc1},
		{"vp6", sizeof("vp6") - 1, NEXUS_VideoCodec_eVp6},
		{"vp7", sizeof("vp7") - 1, NEXUS_VideoCodec_eVp7},
		{"vp8", sizeof("vp8") - 1, NEXUS_VideoCodec_eVp8},
		{NULL, 0, 0}
	};
	unsigned int i;
	for (i = 0 ; i < sizeof(g_videoCodecStrs) - 1
		&& !strncasecmp(codecName, g_videoCodecStrs[i].name,
						           g_videoCodecStrs[i].namelen);
		++i) ;

	/* if not found 'i' should be the last (NULL) index, hence should return 0 */
	return g_videoCodecStrs[i].value;
}

static NEXUS_AudioCodec
GetAudioCodec(char* codecName)
{
	static const NameValue g_audioCodecStrs[] = {
		{"aacplus", sizeof("aacplus") - 1, NEXUS_AudioCodec_eAacPlus},
		{"aac", sizeof("aac") - 1, NEXUS_AudioCodec_eAac},
		{"ac3plus", sizeof("ac3plus") - 1, NEXUS_AudioCodec_eAc3Plus},
		{"ac3", sizeof("ac3") - 1, NEXUS_AudioCodec_eAc3},
		{"adpcm", sizeof("adpcm") - 1, NEXUS_AudioCodec_eAdpcm},
		{"amr", sizeof("amr") - 1, NEXUS_AudioCodec_eAmr},
		{"cook", sizeof("cook") - 1, NEXUS_AudioCodec_eCook},
		{"dra", sizeof("dra") - 1, NEXUS_AudioCodec_eDra},
		{"dtshd", sizeof("dtshd") - 1, NEXUS_AudioCodec_eDtsHd},
		{"dts_legacy", sizeof("dts_legacy") - 1, NEXUS_AudioCodec_eDtsLegacy},
		{"dts", sizeof("dts") - 1, NEXUS_AudioCodec_eDts},
		{"flac", sizeof("flac") - 1, NEXUS_AudioCodec_eFlac},
		{"g711", sizeof("g711") - 1, NEXUS_AudioCodec_eG711},
		{"g726", sizeof("g726") - 1, NEXUS_AudioCodec_eG726},
		{"g729", sizeof("g729") - 1, NEXUS_AudioCodec_eG729},
		{"g723.1", sizeof("g723.1") - 1, NEXUS_AudioCodec_eG723_1},
		{"lpcm_1394", sizeof("lpcm_1394") - 1, NEXUS_AudioCodec_eLpcm1394},
		{"mpeg", sizeof("mpeg") - 1, NEXUS_AudioCodec_eMpeg},
		{"mp3", sizeof("mp3") - 1, NEXUS_AudioCodec_eMp3},
		{"pcm", sizeof("pcm") - 1, NEXUS_AudioCodec_ePcmWav},
		{"vorbis", sizeof("vorbis") - 1, NEXUS_AudioCodec_eVorbis},
		{"wmapro", sizeof("wmapro") - 1, NEXUS_AudioCodec_eWmaPro},
		{"wmastd", sizeof("wmastd") - 1, NEXUS_AudioCodec_eWmaStd},
		{"wmats", sizeof("wmats") - 1, NEXUS_AudioCodec_eWmaStdTs},
		{"wma", sizeof("wma") - 1, NEXUS_AudioCodec_eWmaStd},
		{NULL, 0, 0}
	};
	unsigned int i;
	for (i = 0 ; i < sizeof(g_audioCodecStrs) - 1
		&& !strncasecmp(codecName, g_audioCodecStrs[i].name,
				                   g_audioCodecStrs[i].namelen);
		++i);

	/* if not found 'i' should be the last (NULL) index, hence should return 0 */
	return g_audioCodecStrs[i].value;
}

typedef struct PlayParameters_t {
	const char* filename;
	const char*	ndxFilename;
    NEXUS_VideoCodec videoCodec;
    unsigned short videoPid;
    NEXUS_AudioCodec audioCodec;
    unsigned short audioPid;
    unsigned maxWidth, maxHeight;
    bool encrypted;
	const char* drmFilename;
  /* piggybacking this struct to pass params arround; most (NEXUS-specific)
      * of them ptr-s, so the nexus-defined functions could still use simple
      * local vars, as usual/before, instead of these (weirdish :-)) struct members */
	unsigned timeout;	/* most likeliy 0 ?!*/
	NEXUS_FilePlayHandle* file;
	NEXUS_PlaybackHandle* playback;
	NEXUS_SimpleVideoDecoderHandle* videoDecoder;
	NEXUS_SimpleAudioDecoderHandle* audioDecoder;
	NEXUS_PidChannelHandle* videoPidChannel;
	NEXUS_PidChannelHandle* audioPidChannel;
	NEXUS_StcChannelHandle* stcChannel;
	NEXUS_SimpleVideoDecoderStartSettings* videoProgram;
	NEXUS_SimpleAudioDecoderStartSettings* audioProgram;
} PlayParameters;

static void
ParseArguments(int argc, char** argv, PlayParameters* params)
{
	extern char *optarg;
	extern int optind, opterr, optopt;

	static struct {
		const char *filename, *indexfilename, *drmFilename;
		bool encrypted; /* not 100% sure, for some (aka CARD) cases, where drmFilename could be NULL?? */
		NEXUS_VideoCodec videoCodec;
		unsigned short videoPid;
		NEXUS_AudioCodec audioCodec;
		unsigned short audioPid;
		unsigned maxWidth, maxHeight;
	} files_and_info[] = {
		{"videos/outfile.mpg", NULL, PLAT_DRM_BIN_FILE_NAME, true,
			NEXUS_VideoCodec_eH264, 0x120, NEXUS_AudioCodec_eAc3, 0x131, 0, 0},
		{"videos/cnnticker.mpg", "videos/cnnticker.nav", NULL, false,
			NEXUS_VideoCodec_eMpeg2, 0x21, NEXUS_AudioCodec_eMpeg, 0x22, 720, 480},
		{"videos/spider_cc.mpg", "videos/spider_cc.nav", NULL, false,
			NEXUS_VideoCodec_eMpeg2, 0x11, NEXUS_AudioCodec_eAc3,  0x14, 720, 480},
	};

	int n = 0, opt;
	BKNI_Memset(params, 0, sizeof(PlayParameters));

	/* ... and then check for alternative input */
	while ((opt = getopt(argc, argv, "f:d:A:V:a:v:h")) != -1) {
		switch (opt) {
			case 'f':
				params->filename = optarg;
			break;
			case 'd':
				params->drmFilename = optarg;
				params->encrypted = true;
				break;
			case 'a':	/* something like 0x120,0x130 or possibly 288,304 (i.e. decimal) */
			case 'v':	/* something like 0x120,0x130 or possibly 288,304 (i.e. decimal) */
			{
				unsigned short* dest = 'a' == opt ? &params->audioPid :  &params->videoPid;
				char *format = 'x' == optarg[0] || 'x' == optarg[1]
				            || 'X' == optarg[0] || 'X' == optarg[1] ? "%x" : "%d";
				sscanf(optarg, format, dest);
				break;
			}
			case 'A':	/* aac, ac3, mp3, mpeg, ... */
				params->audioCodec = GetAudioCodec(optarg);
				break;

			case 'V':	/* avc, h264, mpeg2, mpeg4, ... */
				params->videoCodec = GetVideoCodec(optarg);
				break;

			default: /* '?' */
				print_usage();
				exit(EXIT_FAILURE);
		}
	}

	/* command line argument (1-3) has been given - set the PlayParameters members properly
	 * ... that is the ones which has not been set through cmd line options
	 * 			(i.e. these latest have higher priority!)
	 */
	if (argc > optind) {
		n = atoi(argv[optind]);
		if (n > (int)(sizeof(files_and_info)/sizeof(files_and_info[0]))) {
			printf("Entered a wrong number, see below: \n\n");
			print_usage();
			exit(EXIT_FAILURE);
		}
		n = n > 0 ? n - 1 : 0;
	}

	params->filename = params->filename ? params->filename :
						files_and_info[n].filename;
	params->ndxFilename = params->ndxFilename ? params->ndxFilename :
						files_and_info[n].indexfilename;
	params->videoCodec = params->videoCodec ? params->videoCodec :
						files_and_info[n].videoCodec;
	params->videoPid = params->videoPid ? params->videoPid :
						files_and_info[n].videoPid;
	params->audioCodec = params->audioCodec ? params->audioCodec :
						files_and_info[n].audioCodec;
	params->audioPid = params->audioPid ? params->audioPid :
						files_and_info[n].audioPid;
	params->maxWidth = params->maxWidth? params->maxWidth :
						files_and_info[n].maxWidth;
	params->maxHeight = params->maxHeight ? params->maxHeight :
						files_and_info[n].maxHeight;
	params->drmFilename = params->drmFilename ? params->drmFilename :
						files_and_info[n].drmFilename;
	params->encrypted = params->encrypted ? params->encrypted :
						files_and_info[n].encrypted;
	printf("Currently selected: %s, %s, %s,\n"
		   "                    video codec %d, video pid %d, audio codec %d, audio pid %d,\n"
		   "                    max width %d, max height %d.\n",
		   params->filename, params->ndxFilename, params->drmFilename,
		   params->videoCodec, params->videoPid, params->audioCodec, params->audioPid,
		   params->maxWidth, params->maxHeight);

}

static void
AuthenticateClient(void);

static NEXUS_PlaypumpHandle
OpenPlaypump(void);

static void
SetPlaybackSettings(NEXUS_PlaybackHandle playback,
		            NEXUS_PlaypumpHandle playpump,
					NEXUS_StcChannelHandle stcChannel);

static NEXUS_Error
OpenPidChannels(PlayParameters*);

static void
SetProgramStartSettings(PlayParameters*);
static void
DoPlayback(PlayParameters*);

static void
SetDRMWidevine(PlayParameters*);

static void
StartDRMWidevine(PlayParameters*);

static void
ResetDRMWidevine(void);

int main(int argc, char* argv[])
{
	NEXUS_Error rc;
	NEXUS_FilePlayHandle file;
	NEXUS_PlaybackHandle playback;
	NEXUS_PlaypumpHandle playpump;
	PlayParameters playParams;

    if (argc > 1 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
    	print_usage();
        return 0;
    }

    /* === verbatim - see below === */
	ParseArguments(argc, argv, &playParams); /* exits if wrong args */

    /* === verbatim ...  === */
    AuthenticateClient(); /* exits if not authenticated! */

    /* === verb.  === */
    playpump = OpenPlaypump(); /* this also explores the client configuration
     	 	 	 	 	 	 	and 'heap with driver-side mapping'(?!) */
    BDBG_ASSERT(playpump); /* better error handling */

    /* === CreatePlayback()  === */
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback); /* better error handling ?? */

	/* --- housekeeping --- */
    playParams.playback = &playback;

    /* === OpenFile-s()  === */
    file = NEXUS_FilePlay_OpenPosix(playParams.filename,
    								playParams.ndxFilename);

    if (file) { /* as elsewhere: reducing any var scope, stc ones in this case... */
    	NEXUS_SimpleVideoDecoderHandle videoDecoder;
        NEXUS_StcChannelHandle stcChannel;
        NEXUS_StcChannelSettings stcSettings;

        /* === GetStcChannelSettings() === */
        NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);

        stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */

        /* === OpenStcChannel() === */
        stcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
        BDBG_ASSERT(stcChannel); /* better error handling ?? */

		/* --- more housekeeping --- */
        playParams.file = &file;
        playParams.stcChannel = &stcChannel;

        /* === verb. === */
        SetPlaybackSettings(playback, playpump, stcChannel);

        /* Bring up video, audio decoders and outputs */
        /* === AcquireVideoAndAudioDecoders() === */
       	videoDecoder = NEXUS_SimpleVideoDecoder_Acquire(NEXUS_ANY_ID);
       	if (videoDecoder) {
       	    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel = NULL;

            /* === AcquireVideoAndAudioDecoders() - cont. === */
       		NEXUS_SimpleAudioDecoderHandle audioDecoder =
       				NEXUS_SimpleAudioDecoder_Acquire(NEXUS_ANY_ID);
			/* for this example, audio is optional */
       		/* ... at least until I hear otherwise?! */
			if (!audioDecoder)
				BDBG_WRN(("audio decoder not available"));

			/* Could this be confusing?? All extra-'parameters' are pointers...
			 * Sure if the answer gets close to 'y' - consider re{factoring|naming} !*/
			/* --- more housekeeping --- */
			playParams.videoDecoder = &videoDecoder;
			playParams.audioDecoder = &audioDecoder;
			playParams.videoPidChannel = &videoPidChannel;
			playParams.audioPidChannel = &audioPidChannel;

			/* === verb. === */
			rc = OpenPidChannels(&playParams);
			if (NEXUS_SUCCESS == rc)
			{
			    NEXUS_SimpleVideoDecoderStartSettings videoProgram;
			    NEXUS_SimpleAudioDecoderStartSettings audioProgram;
			    playParams.videoProgram = &videoProgram;
			    playParams.audioProgram = &audioProgram;

			    /* --- verb. --- */
				SetProgramStartSettings(&playParams);

			    /* --- verb. --- */
				DoPlayback(&playParams); /* until required; start/stop inside */

			}

			/* === ClosePidChannels() === */
		    NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
		    if (audioPidChannel) {
		        NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);
		    }

			/* === ReleaseDecoders() === */
			if (audioDecoder)
				NEXUS_SimpleAudioDecoder_Release(audioDecoder);
			NEXUS_SimpleVideoDecoder_Release(videoDecoder);
       	}
       	else {
			BDBG_ERR(("video decoder not available"));
			rc = NEXUS_NOT_AVAILABLE;
		}

		/* === CloseStcChannel() === */
		NEXUS_StcChannel_Close(stcChannel);

		/* === CloseFile-s() === */
		NEXUS_FilePlay_Close(file);
    }

    /* === FinalCleanup() === */
    if (playback) NEXUS_Playback_Destroy(playback);
    if (playpump) NEXUS_Playpump_Close(playpump);
    NEXUS_Platform_Uninit();
    DismissClient();
    return rc;
}

static NEXUS_PlaypumpHandle
OpenPlaypump(void)
{ /* ... just a modest attempt to reduce the scope of as many variables as possible ... */
    NEXUS_PlaypumpHandle playpump;
	NEXUS_ClientConfiguration platformConfig;
    NEXUS_PlaypumpOpenSettings playpumpOpenSettings;
	NEXUS_Platform_GetClientConfiguration(&platformConfig);
    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpOpenSettings);

    /* must use heap with driver-side mapping */
    playpumpOpenSettings.heap = platformConfig.heap[1];
    playpump = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpOpenSettings);
    BDBG_ASSERT(playpump);
    return playpump;
}

static void
SetPlaybackSettings(NEXUS_PlaybackHandle playback, NEXUS_PlaypumpHandle playpump,
					NEXUS_StcChannelHandle stcChannel)
{ /* ... just a modest attempt to reduce the scope of as many variables as possible ... */
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    /* At the moment 'hard'; think however for a future version, where it may be set dynamically*/
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = true;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);
}

static NEXUS_Error
OpenPidChannels(PlayParameters* playparams)
{
	NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = playparams->videoCodec; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true
    												  /*(playparams->ndxFilename != NULL)*/
    												   ;
    playbackPidSettings.pidTypeSettings.video.simpleDecoder = *playparams->videoDecoder;
    *playparams->videoPidChannel = NEXUS_Playback_OpenPidChannel(*playparams->playback,
    															  playparams->videoPid,
    															 &playbackPidSettings);

    rc = *playparams->videoPidChannel ? rc : 33 ; /* FIXME with appropriate error */
    if (*playparams->audioDecoder) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidSettings.pidTypeSettings.audio.simpleDecoder = *playparams->audioDecoder;
        *playparams->audioPidChannel = NEXUS_Playback_OpenPidChannel(*playparams->playback,
        															  playparams->audioPid,
        															 &playbackPidSettings);

        								/* FIXME with appropriate error or possibly warning*/
        rc = *playparams->audioPidChannel ? rc : 303 ;
    }
	return rc;
}

static void
SetProgramStartSettings(PlayParameters* params)
{
	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	   the audio outputs. */
	NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(params->videoProgram);
	params->videoProgram->settings.codec = params->videoCodec;
	params->videoProgram->settings.pidChannel = *params->videoPidChannel;
	params->videoProgram->settings.stcChannel = *params->stcChannel;
    if (params->maxWidth && params->maxHeight) {
    	params->videoProgram->maxWidth = params->maxWidth;
    	params->videoProgram->maxHeight = params->maxHeight;
    }

	NEXUS_SimpleAudioDecoder_GetDefaultStartSettings(params->audioProgram);
	params->audioProgram->primary.codec = params->audioCodec;
	params->audioProgram->primary.pidChannel = *params->audioPidChannel;
	params->audioProgram->primary.stcChannel = *params->stcChannel;
}

static void
DoPlayback(PlayParameters* param)
{
   SetDRMWidevine(param);
   StartDRMWidevine(param);
   do {
        /* Start decoders */
    	NEXUS_Error rc = NEXUS_SimpleVideoDecoder_Start(*param->videoDecoder, param->videoProgram);
        BDBG_ASSERT(!rc);
        if (*param->audioDecoder) {
            NEXUS_SimpleAudioDecoder_Start(*param->audioDecoder, param->audioProgram);
            /* decode may fail if audio codec not supported */
        }

/*        StartDRMWidevine(param); Quick check if the order StartDRM - SimpleDecoderStart matters */
        /* Start playback */
        rc = NEXUS_Playback_Start(*param->playback, *param->file, NULL);
        BDBG_ASSERT(!rc);


        if ( false /* || param->trick ... in the near future ... */ ) {
            NEXUS_PlaybackTrickModeSettings trick;
            printf("press ENTER to test trick modes\n");
            getchar();
            printf("pause\n");
            NEXUS_Playback_Pause(*param->playback);
            getchar();
            printf("1x rewind\n");
            NEXUS_Playback_GetDefaultTrickModeSettings(&trick);
            trick.rate = -NEXUS_NORMAL_PLAY_SPEED;
            NEXUS_Playback_TrickMode(*param->playback, &trick);
            getchar();
            printf("2x fast-forward\n");
            NEXUS_Playback_GetDefaultTrickModeSettings(&trick);
            trick.rate = 2*NEXUS_NORMAL_PLAY_SPEED;
            NEXUS_Playback_TrickMode(*param->playback, &trick);
            getchar();
        }

        if (!param->timeout) {
            printf("Press ENTER to stop\n");
            getchar();
        }
        else {
            /* auto close */
            BKNI_Sleep(param->timeout*1000);
        }

        /* Bring down system ... is the start and stop order relevant? */
        /*	... stoping in reverse order, unless counter-indicative?! */
        NEXUS_Playback_Stop(*param->playback);
        if (*param->audioDecoder) {
            NEXUS_SimpleAudioDecoder_Stop(*param->audioDecoder);
        }
        NEXUS_SimpleVideoDecoder_Stop(*param->videoDecoder);
    } while ( false /* || params->loop ... in some near future?! */ );
    ResetDRMWidevine();
}

static void
SetDRMWidevine(PlayParameters* param)
{
	/* TODO - error handling */
	DrmRC drm_rc;

	DRM_Widevine_Init_t inputWvInitStruct;
	/* Technically no need to zero out the next, being so simple (2 members)
	BKNI_Memset(&inputWvInitStruct, 0, sizeof(DRM_Widevine_Init_t));
	... but watch out if/when it becomes more complex! */

	inputWvInitStruct.mode = DRM_WidevineDecryptMode_eTs_Iptv;
								/* no const_cast<> :-( ... */
	inputWvInitStruct.key_file = (char*)param->drmFilename;

	drm_rc = DRM_Widevine_Initialize(inputWvInitStruct);
	BDBG_MSG(("DRM_Widevine_Initialize returned %d", drm_rc));

}

static void
StartDRMWidevine(PlayParameters* param)
{
	BDBG_MSG(("Playing encrypted - %s", (param->encrypted ? "yes" : "no")));
	if (param->encrypted) {

		/* TODO - error handling */
		DrmRC drm_rc;
		NEXUS_PidChannelStatus pidStatus;

		/***
		static uint8_t ecm_even[32] = {0xe6, 0x56, 0x07, 0xde, 0x61, 0xf2, 0x05, 0x2c,
									   0x71, 0x40, 0xc5, 0x0a, 0xe1, 0x56, 0x8f, 0x5f,
									   0x95, 0x4c, 0x20, 0xda, 0x4c, 0x7e, 0xa9, 0x2f,
									   0xe7, 0x98, 0xa5, 0xde, 0xd2, 0x5b, 0x53, 0x4b};
		***/

		static uint8_t ecm_odd[32] = {0x18, 0x99, 0x35, 0x79, 0x07, 0x83, 0x40, 0x42,
									  0x2f, 0x6e, 0xe3, 0x9e, 0xf8, 0x3b, 0x88, 0xca,
									  0x28, 0x20, 0x31, 0x26, 0x03, 0x73, 0x49, 0x2a,
									  0xe5, 0x0c, 0x08, 0xbe, 0xa8, 0xf8, 0x5f, 0xe9 };

		static uint8_t id[16] = { 0 };

		static uint8_t common_key[16] = {0x31, 0x9f, 0xf5, 0xa5, 0xb2, 0x74, 0x6d, 0x9b,
										 0x74, 0xa6, 0x34, 0x3c, 0xaa, 0xf0, 0x1e, 0xf1};

										/* not exactly sure why 'ecm_odd' in both cw ?! */
		drm_rc = DRM_Widevine_LoadCwFixed(common_key, ecm_odd, id, DrmSecurityKeyType_eEven);
		BDBG_MSG(("DRM_Widevine_LoadCwFixed /even returned %d", drm_rc));
		BDBG_MSG(("\tCW ID: 0x%02x 0x%02x 0x%02x 0x%02x\n\n", id[0], id[1], id[2], id[3]));

		drm_rc = DRM_Widevine_LoadCwFixed(common_key, ecm_odd, id, DrmSecurityKeyType_eOdd);
		BDBG_MSG(("DRM_Widevine_LoadCwFixed /odd returned %d", drm_rc));
		BDBG_MSG(("\tCW ID: 0x%02x 0x%02x 0x%02x 0x%02x\n\n", id[0], id[1], id[2], id[3]));

		/* Darn, a bit blurry/easy to get confused here... thought I need a pid, ended up
		 * pid channel index was the needed one! Wouldn't be there any simpler way ???  */
		NEXUS_PidChannel_GetStatus(param->videoProgram->settings.pidChannel, &pidStatus);
		DRM_Widevine_AttachPidChannel(pidStatus.pidChannelIndex);
		BDBG_MSG(("DRM_Widevine_AttachPidChannel(%d) /audio returned %d",
				pidStatus.pidChannelIndex, drm_rc));

		NEXUS_PidChannel_GetStatus (param->audioProgram->primary.pidChannel, &pidStatus);
		DRM_Widevine_AttachPidChannel(pidStatus.pidChannelIndex);
		BDBG_MSG(("DRM_Widevine_AttachPidChannel(%d) /video returned %d",
				pidStatus.pidChannelIndex, drm_rc));

	}
}

static void
ResetDRMWidevine(void)
{
	DRM_Widevine_Finalize();
}
