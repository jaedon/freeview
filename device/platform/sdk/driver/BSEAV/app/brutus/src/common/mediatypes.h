/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mediatypes.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 11/26/08 6:40p $
 * $brcm_Revision: 5 $ *
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/mediatypes.h $
 * 
 * 5   11/26/08 6:40p vsilyaev
 * PR 49704: Bypass hardoded video_xpt table for new combinations of
 * container/video codec
 * 
 * 4   3/21/07 11:57a erickson
 * PR26201: fix up decode info page
 * 
 * 3   2/7/07 12:28p erickson
 * PR22543: improve Brutus Playback UI for audio-only streams
 * 
 * 2   1/17/06 11:47a erickson
 * PR18651: allow audio type names (i.e. AC3,MPEG) to be used in video's
 * audtype param
 * 
 * 1   10/13/05 5:24p erickson
 * PR17541: moved general purpose C code
 * 
 **************************************************************************/
#ifndef MEDIATYPES_H__
#define MEDIATYPES_H__

#include "bsettop.h"

#ifdef __cplusplus
extern "C" {
#endif
	
/*=***********************
Common functions and data arrays for using Settop API video and audio data types.
This converts to/from standard string representations.
**************************/
 
extern const char *g_qualityStr[4];		/* index: bencode_quality */
extern const char *g_mpegFormatStr[3];	/* index: bplayback_mpeg_format (defined in brutus, not Settop API) */
extern const char *g_encryptionStr[5];	/* index: bencryption_type */

/* return string based on baudio_format */
const char *b_audtype_str(baudio_format audtype);

/* return string based on bstream_mpeg_type */
const char *b_xpttype_str(bstream_mpeg_type xpttype);

/* return string based on bvideo_codec */
const char *b_vidtype_str(bvideo_codec vidtype);

/* return bvideo_codec from string. will convert from text or hex value. */
bvideo_codec b_lookup_vidtype(const char *text, char **endptr);

/* returns baudio_format. will convert from text or hex value. */
baudio_format b_lookup_audtype(const char *text, char **endptr);

/* returns bstream_mpeg_type . will convert from text or hex value. */
bstream_mpeg_type b_lookup_xpttype(const char *text, char **endptr);

struct vidtypes {
	bvideo_codec vidtype; /* Settop API value */
	bstream_mpeg_type mpeg_type; /* Settop API value */
	const char *name; /* for the UI */
	const char *infofile_name; /* for the info file */
};

/* Lookup information about the video codec/transport type pair */
struct vidtypes *b_lookup_vidxpt_type(bvideo_codec format, bstream_mpeg_type mpeg_type);
struct vidtypes *b_lookup_vidxpt_type_by_infofile(const char *infofile_name);

/* Get "name" for video codec/transport type pair */
const char *b_vidxpt_str(bvideo_codec format, bstream_mpeg_type mpeg_type);


#ifdef __cplusplus
}
#endif

#endif /* MEDIATYPES_H__ */
