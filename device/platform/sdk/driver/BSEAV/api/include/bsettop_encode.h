/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_encode.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 7/16/08 1:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_encode.h $
 * 
 * 4   7/16/08 1:05p vsilyaev
 * PR 43426: Added more setting for 7403
 * 
 * 3   6/25/08 12:01p jgarrett
 * PR43426: PR43426 : Add 7043 Transcode support
 * 
 * PR43426/1   6/24/08 6:24p shyam
 * PR43426 : Add 7043 Transcode support
 * 
 * 2   4/7/06 10:00a ahulse
 * PR20504: Merge Vpp modifications into main branch
 * 
 * Vpp_7042_Demo/3   4/5/06 3:44p ahulse
 * PR20504: bencode_extended_settings_init now calls bencode_settings_init
 * also, need to pass in bencode_t structure to accomadate this
 * 
 * Vpp_7042_Demo/2   4/4/06 1:43p ahulse
 * PR20504: Added bencode_vpp_off mode, pass in quality setting in
 * extended_settings_init so they can be overridden
 * 
 * Vpp_7042_Demo/1   3/29/06 5:52p ahulse
 * PR20504: Enable support for changeing 7042's vpp settings when encoding
 * analog
 * 
 * 1   2/7/05 7:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/9   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/8   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/7   9/30/03 1:13a erickson
 * updated docjet documentation
 * 
 * Irvine_BSEAVSW_Devel/6   9/17/03 5:04p erickson
 * updated documentation for DocJet
 *
 * Irvine_BSEAVSW_Devel/5   8/21/03 4:31p erickson
 * added get for quality
 *
 * Irvine_BSEAVSW_Devel/4   8/12/03 6:50p vsilyaev
 * Updated encoder settings and renamed bencode_get_settings to the
 * bencode_get.
 *
 * Irvine_BSEAVSW_Devel/3   8/7/03 11:06a vsilyaev
 * Reworked encoder API.
 *
 * Irvine_BSEAVSW_Devel/2   7/9/03 5:19p erickson
 * rework for new bband_t model
 *
 * Irvine_BSEAVSW_Devel/1   7/7/03 9:50a vsilyaev
 * Encoder API.
 *
 *
 ***************************************************************************/
#ifndef BSETTOP_ENCODE_H__
#define BSETTOP_ENCODE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*=***********************************
Analog platforms require an MPEG encoder in order to
record PVR.

The encoder API works by receiving an analog stream and returning
a digital stream. The analog stream is still valid and can be
decoded while the digital stream is either decoded or recorded.
**************************************/

/***************************************************************************
Summary:
	Defines for the video GOP structure.
****************************************************************************/
typedef enum bencode_gop_structure {
    bencode_gop_structure_i,
    bencode_gop_structure_ip,
    bencode_gop_structure_ipb,
    bencode_gop_structure_ipbb,
    bencode_gop_structure_progressive_refresh
} bencode_gop_structure;

/*
Summary:
Data structure used to initialize the encoder.
*/
typedef struct bencode_settings {
	bvideo_codec video_format; /* format of digital video in the encoded stream */
	unsigned video_width; /* width in pixels of the encoded MPEG stream  */
	unsigned video_height; /* height in pixels of the encoded MPEG stream  */
	unsigned video_bitrate; /* bitrate (bits per second) of the encoded MPEG video stream */
	uint16_t video_pid; /* pid for the video program */
	bvideo_frame_rate framerate; /* framerate */
	bencode_gop_structure gop_structure; /* GOP structure */
	bool progressive ; /* Progressive or interlaced */


	baudio_format audio_type; /* format of the digital audio in the encoded stream */
	unsigned audio_bitrate; /* bitrate (bits per second) of the encoded audio stream */
	uint16_t audio_pid; /* pid for the audio program in the encoded stream */

	unsigned transport_bitrate ;
        unsigned vpp_settings; /* what to set the vpp to  */
} bencode_settings;


/**
Summary:
	Open an MPEG encoder channel.
Description:
	For a BCM7041, there are two encoder channels, so you can open two encoder objects.
	For a BCM7040, there is one encoder channel, so you can only open one encoder object.
**/
bencode_t bencode_open(
		bobject_t id /* encode object id */
		);

/**
Summary:
	Close an MPEG encoder channel.
**/
void bencode_close(
		bencode_t encode /* encode object */
		);

/*
Summary:
        Enum used to set enhanced encoding quality settings.
Description:
        This makes it easier to set the bencode_settings structure.
**/
typedef enum bencode_vpp_mode {
        bencode_vpp_none,		/* Don't use extended settings 		*/
        bencode_vpp_off,		/* use extended settings, vpp is off	*/
        bencode_vpp_temporal_only,
        bencode_vpp_spatial_only,
        bencode_vpp_impulse_only,
        bencode_vpp_medium,
        bencode_vpp_default
} bencode_vpp_mode;

/*
Summary:
	Enum used to set default encoding quality settings.
Description:
	This makes it easier to set the bencode_settings structure.
**/
typedef enum bencode_quality {
	bencode_quality_best,
	bencode_quality_high,
	bencode_quality_medium,
	bencode_quality_basic
} bencode_quality;

/**
Summary:
        Initialize extended encoder settings - vpp video bitrate etc. Added to demonstrate
	adjustable vpp settings of 7042 encoder.
**/
void bencode_extended_settings_init(
        bencode_settings *settings,     /* [out] settings to be initialized */
	bencode_quality quality,	/* quality level to use when chosing initial settings */
	bencode_t encode,		/* required for possible resource-dependent defaults */
        bencode_vpp_mode vpp_mode       /* vpp settings to set to*/
        );


/**
Summary:
	Initialize encoder settings for a particular quality level.
**/
void bencode_settings_init(
	bencode_settings *settings, /* [out] settings to be initialized */
	bencode_quality quality,	/* quality level to use when chosing initial settings */
	bencode_t encode			/* required for possible resource-dependent defaults */
	);

/**
Summary:
	Start encoding an analog stream.
Description:
	The analog stream remains valid before, during and after encoding.
Return values:
	If successful, it returns a digital stream which can be decoded or recorded.
	Otherwise it returns NULL.
**/
bstream_t bencode_start(
	bencode_t encode, /* encode object */
	bstream_t stream, /* analog stream to encode */
	const bencode_settings *settings /* settings for the encode */
	);

/**
Summary:
	Stop encoding.
Description:
	After bencode_stop, the digital stream is invalid.
	The analog stream remains valid.
**/
void bencode_stop(
	bencode_t encode,	/* encode object */
	bstream_t stream	/* Digital stream returned by bencode_start which is 
							being stopped. */
	);

#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_ENCODE_H__ */

