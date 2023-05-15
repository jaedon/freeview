/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_transcode.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 9/11/08 12:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_transcode.c $
 * 
 * 11   9/11/08 12:39p shyam
 * PR46778 : Add audiooasthru support to brutus
 * 
 * 10   9/10/08 6:53p shyam
 * PR46778 : Port brutus to B0 firmwre rel 2.2
 * 
 * 9   8/12/08 6:58p shyam
 * PR43426 : Fix transcode disable mode
 * 
 * 8   8/12/08 5:09p shyam
 * PR43426 : Add support for I only and debug function
 * 
 * 7   7/30/08 8:13p shyam
 * PR43426 : Add programming of Qp
 * 
 * 6   7/28/08 5:25p shyam
 * PR 45076 : Add transcode support for streams originating on a playback
 * channel
 * 
 * 5   7/8/08 4:20p shyam
 * PR 42338 : Add audio support
 * PR 42636 : Add support for diferernt PCR and Video Pids
 * 
 * 4   7/2/08 1:41p shyam
 * PR 42338 : Add audio support
 * 
 * 3   6/26/08 2:40p shyam
 * PR43426 : fix stubs to enable regular builds
 * 
 * 2   6/25/08 12:54p shyam
 * PR43426 : Add stubs to enable regualr builds
 * 
 * 1   6/24/08 6:23p shyam
 * PR43426 : Add 7043 Transcode support
 *
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(transcode);


#ifdef B_HAS_TRANSCODE

#define B_HAS_TRANSCODE_AUDIO 1

struct btranscode g_transcode[B_MAX_TRANSCODES] ;

/**
Summary:
	Initialize encoder settings for a particular quality level.
**/
void btranscode_settings_init(
    btranscode_settings *settings, /* [out] settings to be initialized */
    bencode_quality quality, /* quality level to use when chosing initial settings */
    bencode_vpp_mode vpp_mode       /* vpp settings to set to*/
    )
{
	BSTD_UNUSED(quality) ;
	BSTD_UNUSED(vpp_mode) ;
	BKNI_Memset( settings, 0, sizeof( *settings) ) ;
	settings->realtime = true;
	settings->program.video[0].format = bvideo_codec_h264 ;
	settings->program.video[0].width = 720 ;
	settings->program.video[0].height= 480 ;
	settings->program.video[0].bitrate = 25000000 ;
	settings->program.video[0].pid = 0x21 ;
	settings->program.video[0].progressive = true ;
	
	settings->program.audio[0].format = baudio_format_ac3;
	settings->program.audio[0].passthrough = true ;
	settings->program.audio[0].samplerate = 44100;
	settings->program.audio[0].bitrate = 100000 ; 
	settings->program.audio[0].pid = 0x24;

	settings->program.pcr_pid = 0x21 ;
	
	settings->stream_type = bstream_mpeg_type_ts ;
	settings->transport_bitrate = 30000000 ; 
}

/*
Summary:
    Open a transcode engine
Description:
*/
btranscode_t btranscode_open(
    bobject_t transcode_id   /* transcode object id */
   )
{
    btranscode_t transcode;
	NEXUS_RemuxSettings remuxSettings ;
	NEXUS_RemuxHandle rmx;

    unsigned index = B_ID_GET_INDEX(transcode_id);
    if (index >= B_MAX_TRANSCODES)
    {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
	transcode = &g_transcode[index];

	NEXUS_Remux_GetDefaultSettings(&remuxSettings) ;
	transcode->rmx[0] = rmx = NEXUS_Remux_Open(0, &remuxSettings);
	if(NULL == rmx)
	{
		goto error ;
	}

	/* End of open */
    return transcode;
error:
    btranscode_close(transcode);
    return NULL;
}

/*
Summary:
    Close a transcode
*/
void btranscode_close(
    btranscode_t transcode /* handle returned by btranscode_open */
    )
{
	if(transcode->rmx[0])
		NEXUS_Remux_Close(transcode->rmx[0]) ;
}

bresult btranscode_p_start_video_transcode(
    btranscode_t transcode, /* handle returned by btranscode_open */
    bstream_t source   /* source for the decode, either analog or digital */
    )
{
	NEXUS_Transcoder_OutputConfiguration portConfig ;
	NEXUS_Transcoder_OutputPortHandle hPortHandle;
	NEXUS_Transcoder_OutputChannel outputPort ;
	NEXUS_Transcoder_OpenVideoTranscodeSettings openSettings ;
	NEXUS_VideoCodec outputVideoCodec ;

	/* Transcoder stuff */
	BKNI_Memset( &portConfig, 0 , sizeof(portConfig) );
	portConfig.transportBitrate = transcode->settings.transport_bitrate ;
	outputPort.outputChNum = 0;
	outputPort.outputType = 
		NEXUS_Transcoder_OutputChannelType_eTxSerialOut ;
	hPortHandle = NEXUS_Transcoder_OpenOutputPort(outputPort, &portConfig);
	BDBG_ERR(("Port handle = %p", hPortHandle));
	if(NULL==hPortHandle)
	{
		return BSETTOP_ERROR(berr_invalid_state) ;
	}
	transcode->hPortHandle = hPortHandle ;

	BKNI_Memset( &openSettings, 0 , sizeof(openSettings) );
	openSettings.bRealTime = transcode->settings.realtime ;
	openSettings.inputType = NEXUS_Transcoder_VideoInputChannelType_eTxSerialIn ;
	openSettings.inputChNum = 1 ;
	openSettings.inputStreamType = NEXUS_TransportType_eTs ;
	BDBG_ERR(("source Stream pid = %08x", source->mpeg.video[0].pid));
	BDBG_ERR(("source Stream pid = %08x", source->mpeg.pcr_pid));
	BDBG_ERR(("source format = %08x", source->mpeg.video[0].format));
	openSettings.inputStreamId = source->mpeg.video[0].pid ;
	openSettings.inputPcrPid =  source->mpeg.pcr_pid ;
	openSettings.inputVideoCodec = 
		b_videocodec2nexus(source->mpeg.video[0].format) ;

	/* Video Encode/Output Settings */
	openSettings.outputVideoCodec = outputVideoCodec = 
		b_videocodec2nexus(transcode->settings.program.video[0].format) ;
	openSettings.outputStreamType = NEXUS_TransportType_eTs ;
	openSettings.outputPort = hPortHandle ;
	if( NEXUS_VideoCodec_eH264 == outputVideoCodec )
	{
		openSettings.codingSettings.avcCoding.h264Level = NEXUS_Transcoder_H264Level_eL4 ;
		openSettings.codingSettings.avcCoding.h264Profile = NEXUS_Transcoder_H264Profile_eMain ;
	}
	else if( NEXUS_VideoCodec_eMpeg2 == outputVideoCodec )
	{
		openSettings.codingSettings.mpegCoding.mpegLevel = NEXUS_Transcoder_MPEGLevel_eMain ;
		openSettings.codingSettings.mpegCoding.mpegProfile = NEXUS_Transcoder_MPEGProfile_eMain ;
	}
	else if( NEXUS_VideoCodec_eMpeg4Part2 == outputVideoCodec )
	{
		openSettings.codingSettings.mpeg4Coding.mpeg4Level = NEXUS_Transcoder_MPEG4Level_e1;
		openSettings.codingSettings.mpeg4Coding.mpeg4Profile = NEXUS_Transcoder_MPEG4Profile_eSimple;
	}
	
	/** Transport Settings */
	openSettings.outputStreamId = transcode->settings.program.video[0].pid ;
	openSettings.transportBitrate = transcode->settings.transport_bitrate; /* Bits per Second */

	openSettings.outWidth = transcode->settings.program.video[0].width ;
	openSettings.outHeight = transcode->settings.program.video[0].height ;

	openSettings.bProgressive = transcode->settings.program.video[0].progressive;

	openSettings.esPeakRateControl = NEXUS_Transcoder_PeakRateControl_eConstantQp;
	openSettings.esPeakBitrate = 14 ;

	if (bsettop_get_config("qpvalue")) {
		openSettings.esPeakBitrate = atoi(bsettop_get_config("qpvalue"));
		BDBG_WRN(("setting Qp Value %d", openSettings.esPeakBitrate));
	}
	/** VBI Settings */
	openSettings.enableVbi = false ;

	transcode->nexus_transcoder = NEXUS_Transcoder_OpenVideoTranscode(&openSettings) ;
	BDBG_ERR(("Open transcoder handle = %p", transcode->nexus_transcoder));
	if(NULL == transcode->nexus_transcoder)
	{
		return BSETTOP_ERROR(berr_invalid_state) ;
	}

	if(bsettop_get_config("ionly"))
	{
		NEXUS_Transcoder_VideoEncodeSettings vidEncSettings ;
		BDBG_ERR(("\n********** Enable I Only *************\n"));
		NEXUS_Transcoder_GetVideoEncodeSettings(transcode->nexus_transcoder, &vidEncSettings );
		vidEncSettings.gopStructure = NEXUS_Transcoder_GOPStructure_eI_Picture ;
		NEXUS_Transcoder_SetVideoEncodeSettings(transcode->nexus_transcoder, &vidEncSettings );
	}


	NEXUS_Transcoder_Start(transcode->nexus_transcoder) ;

	return b_ok ;
}


bresult btranscode_p_start_audio_transcode(
    btranscode_t transcode, /* handle returned by btranscode_open */
    bstream_t source   /* source for the decode, either analog or digital */
    )
{
  	NEXUS_TranscoderHandle audTranscoder ;
	NEXUS_Transcoder_OpenAudioTranscodeSettings openAudSettings ;

	BKNI_Memset( &openAudSettings, 0, sizeof(openAudSettings) ); 
 	openAudSettings.inputChNum = 1 ;
	openAudSettings.inputType = NEXUS_Transcoder_AudioInputChannelType_eTxSerialIn ;
	openAudSettings.inputAudioStreamId = source->mpeg.audio[0].pid ;
	openAudSettings.inputPcrPid = source->mpeg.pcr_pid ;
	openAudSettings.outputPort = transcode->hPortHandle ;
	audTranscoder = NEXUS_Transcoder_OpenAudioTranscode(&openAudSettings) ;
	NEXUS_Transcoder_Start(audTranscoder) ;

	return b_ok ;
}

bresult btranscode_p_enable_remux(
    btranscode_t transcode, /* handle returned by btranscode_open */
    bstream_t source   /* source for the decode, either analog or digital */
    )
{
	NEXUS_Error nrc ;
	NEXUS_PidChannelHandle vidPidChannel, audPidChannel, pcrPidChannel ;

	BDBG_MSG(("%s, Video Pid = %x",
		(NULL != source->producer.band) ? "Parser Band" : "Playback Band",
		source->mpeg.video[0].pid));
	BDBG_ERR(("Audio Pid = %x", source->mpeg.audio[0].pid));
	/* Open Video Pid channel : Since we are getting a bstream_t object 
	  It means the pid channel already exists so we will get an existing handle
	  no need to worry about settings for this Pidchannel Open call */
	vidPidChannel = bstream_p_open_pid(
					source,
					source->mpeg.video[0].pid, 
					bstream_pid_type_video) ;
	nrc = NEXUS_Remux_AddPidChannel(transcode->rmx[0], vidPidChannel) ;
	if(nrc != NEXUS_SUCCESS)
		return BSETTOP_ERROR(nrc) ;

#if B_HAS_TRANSCODE_AUDIO
	audPidChannel = bstream_p_open_pid(
					source,
					source->mpeg.audio[0].pid, 
					bstream_pid_type_audio) ;
	nrc = NEXUS_Remux_AddPidChannel(transcode->rmx[0], audPidChannel) ;
	if(nrc != NEXUS_SUCCESS)
		return BSETTOP_ERROR(nrc) ;
#endif

	if( source->mpeg.pcr_pid != source->mpeg.video[0].pid &&
		source->mpeg.pcr_pid != source->mpeg.audio[0].pid )
	{
		pcrPidChannel = bstream_p_open_pid(
						source,
						source->mpeg.pcr_pid, 
						bstream_pid_type_pcr) ;
		nrc = NEXUS_Remux_AddPidChannel(transcode->rmx[0], pcrPidChannel) ;
		if(nrc != NEXUS_SUCCESS)
			return BSETTOP_ERROR(nrc) ;
	}

	NEXUS_Remux_Start(transcode->rmx[0]) ;
	
	return b_ok ;
}

/*
Summary:
    Start transcoding a stream.
*/
bstream_t btranscode_start(
    btranscode_t transcode, /* handle returned by btranscode_open */
    bstream_t source,   /* source for the transcode, either analog or digital */
    const btranscode_settings *settings /* [out] settings to be initialized */
    )
{
	bresult brc ;
	bband_t band;
	bstream_t dest ;
	bstream_mpeg mpeg;

	BDBG_OBJECT_ASSERT(source, bstream);
	BDBG_ASSERT(settings);

	/* Remux stuff */
	brc = btranscode_p_enable_remux(transcode, source) ;
	if(brc != b_ok)
		return NULL ;

	transcode->settings = *settings ;

	/* Transcoder Stuff */
	brc = btranscode_p_start_video_transcode(transcode, source) ;
	if(brc != b_ok)
		return NULL ;

#if B_HAS_TRANSCODE_AUDIO
	brc = btranscode_p_start_audio_transcode(transcode, source) ;
	if(brc != b_ok)
		return NULL ;
#endif

	/* Assume Transcoder is connected to IB2 for now */
	band = bstreamer_attach( B_ID(102), bstream_mpeg_type_ts);
	if (!band) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = transcode->settings.program.video[0].pid;
	mpeg.video[0].format = transcode->settings.program.video[0].format;
#if B_HAS_TRANSCODE_AUDIO
	mpeg.audio[0].pid = transcode->settings.program.audio[0].pid;
	mpeg.audio[0].format = transcode->settings.program.audio[0].format;
#else
	mpeg.audio[0].pid = 0 ;
	mpeg.audio[0].format = baudio_format_unknown ;
#endif
	mpeg.pcr_pid = transcode->settings.program.pcr_pid;

	dest = bstream_open(band, &mpeg);
	if (!dest) goto error;

	return dest ;
error :
	return NULL ;
}

/*
Summary:
    Stop transcoding a stream.
Description:
    The stream remains valid after transcode is stopped. 
*/
void btranscode_stop(
    btranscode_t transcode /* handle returned by btranscode_open */
    )
{
    BSTD_UNUSED(transcode);
    BSETTOP_ERROR(berr_not_supported);
}


/*
Summary:
    Pause a stream transcode.
*/
bresult btranscode_pause(
    btranscode_t transcode /* handle returned by btranscode_open */
    )
{
    BSTD_UNUSED(transcode);
    return BSETTOP_ERROR(berr_not_supported);
}


/*
Summary:
    Resume a stream transcode.
*/
bresult btranscode_resume(
    btranscode_t transcode /* handle returned by btranscode_open */
    )
{
    BSTD_UNUSED(transcode);
    return BSETTOP_ERROR(berr_not_supported);
}

/*
Summary:
    Get Settings for a stream transcode
Description:
    Get aand Set settings will take care of dyanmic parameter changes 
*/
void btranscode_get(
    btranscode_t transcode, /* handle returned by btranscode_open */
    btranscode_settings *settings
    )
{
	BDBG_ASSERT(transcode);
	*settings = transcode->settings ;
}


/*
Summary:
    Get Settings for a stream transcode
Description:
    Get aand Set settings will take care of dyanmic parameter changes 
*/
bresult btranscode_set(
    btranscode_t transcode, /* handle returned by btranscode_open */
    const btranscode_settings *settings
    )
{
    BSTD_UNUSED(transcode);
    BSTD_UNUSED(settings);
    return BSETTOP_ERROR(berr_not_supported);
}


/*
Summary:
    Get Settings for a stream transcode
Description:
    Get aand Set settings will take care of dyanmic parameter changes 
*/
bresult btranscode_get_status(
    btranscode_t transcode, /* handle returned by btranscode_open */
    btranscode_status *status
    ) 
{
    BSTD_UNUSED(transcode);
    BSTD_UNUSED(status);
    return BSETTOP_ERROR(berr_not_supported);
}

/*
Summary:
    For debugging the transcoder
*/
void btranscode_debug(int loops)
{
	NEXUS_Transcoder_Debug(loops) ;
}

#else

btranscode_t btranscode_open(
    bobject_t transcode_id   /* transcode object id */
   )
{
    BSTD_UNUSED(transcode_id);
    BSETTOP_ERROR(berr_not_supported);
    return NULL ;
}

/**
Summary:
	Initialize transcoder settings for a particular quality level.
**/
void btranscode_settings_init(
    btranscode_settings *settings, /* [out] settings to be initialized */
    bencode_quality quality, /* quality level to use when chosing initial settings */
    bencode_vpp_mode vpp_mode       /* vpp settings to set to*/
    )
{
    BSTD_UNUSED(settings);
    BSTD_UNUSED(quality);
    BSTD_UNUSED(vpp_mode);
    BSETTOP_ERROR(berr_not_supported);
}

/*
Summary:
    Close a transcode
*/
void btranscode_close(
    btranscode_t transcode /* handle returned by btranscode_open */
    )
{
    BSTD_UNUSED(transcode);
    BSETTOP_ERROR(berr_not_supported);
}

/*
Summary:
    Start transcoding a stream.
*/
bstream_t btranscode_start(
    btranscode_t transcode, /* handle returned by btranscode_open */
    bstream_t source,   /* source for the transcode, either analog or digital */
    const btranscode_settings *settings /* [out] settings to be initialized */
    )
{
    BSTD_UNUSED(transcode);
    BSTD_UNUSED(source);
    BSTD_UNUSED(settings);
    BSETTOP_ERROR(berr_not_supported);
    return NULL ;
}

/*
Summary:
    Stop transcoding a stream.
Description:
    The stream remains valid after transcode is stopped. 
*/
void btranscode_stop(
    btranscode_t transcode /* handle returned by btranscode_open */
    )
{
    BSTD_UNUSED(transcode);
    BSETTOP_ERROR(berr_not_supported);
}


/*
Summary:
    Pause a stream transcode.
*/
bresult btranscode_pause(
    btranscode_t transcode /* handle returned by btranscode_open */
    )
{
    BSTD_UNUSED(transcode);
    return BSETTOP_ERROR(berr_not_supported);
}


/*
Summary:
    Resume a stream transcode.
*/
bresult btranscode_resume(
    btranscode_t transcode /* handle returned by btranscode_open */
    )
{
    BSTD_UNUSED(transcode);
    return BSETTOP_ERROR(berr_not_supported);
}


/*
Summary:
    Get Settings for a stream transcode
Description:
    Get aand Set settings will take care of dyanmic parameter changes 
*/
void btranscode_get(
    btranscode_t transcode, /* handle returned by btranscode_open */
    btranscode_settings *settings
    ) 
{
    BSTD_UNUSED(transcode);
    BSTD_UNUSED(settings);
    BSETTOP_ERROR(berr_not_supported);
}


/*
Summary:
    Set Settings for a stream transcode
Description:
    Sets the transcoder settings 
*/
bresult btranscode_set(
    btranscode_t transcode, /* handle returned by btranscode_open */
    const btranscode_settings *settings
    ) 
{
    BSTD_UNUSED(transcode);
    BSTD_UNUSED(settings);
    return BSETTOP_ERROR(berr_not_supported);
}


/*
Summary:
    Get Settings for a stream transcode
Description:
    Get aand Set settings will take care of dyanmic parameter changes 
*/
bresult btranscode_get_status(
    btranscode_t transcode, /* handle returned by btranscode_open */
    btranscode_status *status
    ) 
{
    BSTD_UNUSED(transcode);
    BSTD_UNUSED(status);
    return BSETTOP_ERROR(berr_not_supported);
}

#endif


