/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_encode.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 9/11/08 12:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_encode.c $
 * 
 * 7   9/11/08 12:38p shyam
 * PR46778 : Add support for AAC audio encode from I2S
 * 
 * 6   9/10/08 6:53p shyam
 * PR46778 : Port brutus to B0 firmwre rel 2.2
 * 
 * 5   7/18/08 11:15a jrubio
 * PR44849: fix TRANSCODE isue
 * 
 * 4   7/17/08 4:36p shyam
 * PR43426 : Add 7043 656 Encode support
 * 
 * 3   12/20/07 10:28a erickson
 * PR37590: fix warnings
 * 
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(encode);

#ifdef B_HAS_TRANSCODE
struct bencode g_encode[B_MAX_ENCODES] ;
#endif

bencode_t bencode_open(bobject_t id)
{
#ifdef B_HAS_TRANSCODE
    bencode_t encode ;

    unsigned index = B_ID_GET_INDEX(id);
    if (index >= B_MAX_ENCODES)
    {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }

    encode = &g_encode[index];

    return encode ;
#else
    BSTD_UNUSED(id);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
#endif
}

void bencode_close(bencode_t encode)
{
#ifdef B_HAS_TRANSCODE
    BSTD_UNUSED(encode);
#else
    BSTD_UNUSED(encode);
    BSETTOP_ERROR(berr_not_supported);
#endif
}

void bencode_extended_settings_init(bencode_settings *settings, bencode_quality quality, bencode_t encode, bencode_vpp_mode vpp_mode)
{
#ifdef B_HAS_TRANSCODE
    BSTD_UNUSED(quality) ;
    BSTD_UNUSED(encode) ;
	BSTD_UNUSED(vpp_mode);

    settings->video_pid = 0x21 ;
    settings->video_width = 720 ;
    settings->video_height = 480 ;
    settings->video_bitrate = 30000000 ;
    settings->video_format = bvideo_codec_mpeg2;
    settings->framerate = bvideo_frame_rate_29_97 ;
    settings->gop_structure = bencode_gop_structure_ip ;
    settings->progressive = false ;
    settings->transport_bitrate = 35000000 ;

    settings->audio_pid = 0x24 ;
    settings->audio_bitrate = 100000 ;
    settings->audio_type = baudio_format_aac;

#else
    BSTD_UNUSED(settings);
    BSTD_UNUSED(quality);
    BSTD_UNUSED(encode);
    BSTD_UNUSED(vpp_mode);
    BSETTOP_ERROR(berr_not_supported);
#endif
}

void bencode_settings_init(bencode_settings *settings, bencode_quality quality, bencode_t encode)
{
#ifdef B_HAS_TRANSCODE
    BSTD_UNUSED(quality) ;
    BSTD_UNUSED(encode) ;
    settings->video_pid = 0x21 ;
    settings->video_width = 720 ;
    settings->video_height = 480 ;
    settings->video_bitrate = 30000000 ;
    settings->video_format = bvideo_codec_mpeg2;
    settings->framerate = bvideo_frame_rate_29_97 ;
    settings->gop_structure = bencode_gop_structure_ip ;
    settings->progressive = false ;
    settings->transport_bitrate = 35000000 ;

    settings->audio_pid = 0x24 ;
    settings->audio_bitrate = 100000 ;
    settings->audio_type = baudio_format_aac;

#else
    BSTD_UNUSED(settings);
    BSTD_UNUSED(quality);
    BSTD_UNUSED(encode);
    BSETTOP_ERROR(berr_not_supported);
#endif
}

#ifdef B_HAS_TRANSCODE

static bresult bencode_p_start_video_encode(
    bencode_t encode, /* handle returned by bencode_open */
    bstream_t stream   /* source for the decode, either analog or digital */
    )
{
	NEXUS_Transcoder_OutputConfiguration portConfig ;
	NEXUS_Transcoder_OutputChannel outputPort ;
	NEXUS_Transcoder_OpenVideoEncodeSettings openSettings ;
	NEXUS_VideoCodec outputVideoCodec ;

	/* Transcoder stuff */
	BKNI_Memset( &portConfig, 0 , sizeof(portConfig) );
	portConfig.transportBitrate = encode->settings.transport_bitrate ;
	outputPort.outputChNum = 0;
	outputPort.outputType = 
		NEXUS_Transcoder_OutputChannelType_eTxSerialOut ;
	encode->hPortHandle = NEXUS_Transcoder_OpenOutputPort(outputPort, &portConfig);
	BDBG_ERR(("Port handle = %p", encode->hPortHandle));
	if(NULL==encode->hPortHandle)
	{
		return BSETTOP_ERROR(berr_invalid_state) ;
	}

	BKNI_Memset( &openSettings, 0 , sizeof(openSettings) );
	openSettings.bRealTime = true ;
	openSettings.inputType = NEXUS_Transcoder_VideoInputChannelType_eITUBT656 ;
	openSettings.inputChNum = stream->producer.linein->vdec7043DeviceNo  ;
	openSettings.videoformat = NEXUS_VideoFormat_eNtsc ;
	openSettings.bFmtAutoDetect = true;
	/* Video Encode/Output Settings */
	openSettings.outputVideoCodec = outputVideoCodec = 
		b_videocodec2nexus(encode->settings.video_format) ;
	openSettings.outputStreamType = NEXUS_TransportType_eTs ;
	openSettings.outputPort = encode->hPortHandle ;
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
	openSettings.outputStreamId = encode->settings.video_pid ;
	openSettings.esPeakRateControl = NEXUS_Transcoder_PeakRateControl_eConstantQp ;
	openSettings.esTargetBitrate = encode->settings.video_bitrate; /* Bits per Second */

	openSettings.transportBitrate = encode->settings.transport_bitrate; /* Bits per Second */

	openSettings.outWidth = encode->settings.video_width ;
	openSettings.outHeight = encode->settings.video_height ;

	openSettings.bProgressive = encode->settings.progressive;

	/** VBI Settings */
	openSettings.enableVbi = false ;

	encode->nexus_encoder = NEXUS_Transcoder_OpenVideoEncode(&openSettings) ;
	BDBG_ERR(("Open encoder handle = %p", encode->nexus_encoder));
	if(NULL == encode->nexus_encoder)
	{
		return BSETTOP_ERROR(berr_invalid_state) ;
	}

	NEXUS_Transcoder_Start(encode->nexus_encoder) ;

	return b_ok ;

}

static bresult bencode_p_start_audio_encode(
    bencode_t encode /* handle returned by bencode_open */
)
{
	NEXUS_Error rc ;
	NEXUS_Transcoder_OpenAudioEncodeSettings openSettings ;

	/* Transcoder stuff */
	BKNI_Memset( &openSettings, 0 , sizeof(openSettings) );
	openSettings.bRealTime = true ;
	openSettings.inputType = NEXUS_Transcoder_AudioInputChannelType_eI2S ;
	openSettings.inputChNum = 0 ;
	openSettings.inputSamplingRate = 48000 ;
	
	/* Audio Encode/Output Settings */
	openSettings.audioCodec = b_audiocodec2nexus(encode->settings.audio_type) ;
	openSettings.outSamplingRate = 48000 ;
	openSettings.outputStreamType = NEXUS_TransportType_eTs ;
	openSettings.outputPort = encode->hPortHandle ;
	/* Transport Settings */
	openSettings.outputStreamId = encode->settings.audio_pid ;
	openSettings.transportBitrate = encode->settings.audio_bitrate; /* Bits per Second */

	encode->nexus_audEncoder = NEXUS_Transcoder_OpenAudioEncode(&openSettings) ;
	rc = NEXUS_Transcoder_Start(encode->nexus_audEncoder);
	if(rc)
		return BSETTOP_ERROR(berr_invalid_state) ;
	return b_ok ;
}

#endif
bstream_t bencode_start(bencode_t encode, bstream_t stream, const bencode_settings *settings)
{
#ifdef B_HAS_TRANSCODE
    bresult brc ;
	bband_t band;
	bstream_t dest ;
	bstream_mpeg mpeg;
	
    encode->settings = *settings ;
    brc = bencode_p_start_video_encode(encode, stream);
	if(brc != b_ok) goto error ;

    brc = bencode_p_start_audio_encode(encode);
	if(brc != b_ok) goto error ;

	/* Assume Transcoder is connected to IB2 for now */
	band = bstreamer_attach( B_ID(102), bstream_mpeg_type_ts);
	if (!band) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = encode->settings.video_pid;
	mpeg.video[0].format = encode->settings.video_format;

	mpeg.audio[0].pid = encode->settings.audio_pid;
	mpeg.audio[0].format = encode->settings.audio_type;
	mpeg.pcr_pid = encode->settings.video_pid;

	dest = bstream_open(band, &mpeg);
	if (!dest) goto error;

	return dest ;
error :
	return NULL ;
#else
    BSTD_UNUSED(encode);
    BSTD_UNUSED(stream);
    BSTD_UNUSED(settings);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
#endif
}

void bencode_stop(bencode_t encode, bstream_t stream)
{
#ifdef B_HAS_TRANSCODE
    BSTD_UNUSED(encode);
    BSTD_UNUSED(stream);

#else
    BSTD_UNUSED(encode);
    BSTD_UNUSED(stream);
    BSETTOP_ERROR(berr_not_supported);
#endif
}

