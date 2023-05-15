/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2ts_psi_probe.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 4/20/12 2:27p $
 *
 * Module Description:
 *
 * MPEG-2 TS Parser/Demux library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2ts_psi_probe.h $
 * 
 * 7   4/20/12 2:27p vsilyaev
 * SW7405-4929: Added intentification/parsing of subtitle streams
 * 
 * 6   11/4/10 11:58a vsilyaev
 * SW35230-2036: Clarified conditions when bmpeg2ts_psi_probe_track could
 * be used
 * 
 * 5   8/27/10 5:26p vsilyaev
 * SWBLURAY-19100: Added support for DVB subtitles
 * 
 * 4   7/21/08 1:19p vsilyaev
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/1   7/18/08 4:53p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * 3   7/25/07 1:50p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 2   7/23/07 11:36a vsilyaev
 * PR 32813: Added MPEG-2 TS payload probe
 * 
 * 1   7/17/07 4:18p vsilyaev
 * PR 32813: MPEG-2 TS/PES/PSI parser
 * 
 * 
 *******************************************************************************/
#ifndef __BMPEG2TS_PSI_PROBE_H__
#define __BMPEG2TS_PSI_PROBE_H__


#include "bmedia_probe_impl.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef enum bmpeg2ts_psi_transport_type {
	bmpeg2ts_psi_transport_unknown,
	bmpeg2ts_psi_transport_dvb_subtitles,
	bmpeg2ts_psi_transport_teletext,
    bmpeg2ts_psi_transport_pcr
} bmpeg2ts_psi_transport_type;


#define BMPEG2TS_PSI_PROBE_ID       0x9000
#define BMPEG2TS192_PSI_PROBE_ID    0x9001

typedef struct bmpeg2ts_psi_dvb_subtitle {
    bool valid;
    uint8_t languageCode[4]; /* 3 ascii codes, add one more for "\0" */
    uint8_t type;
    uint8_t compositionPageId;
    uint8_t ancillaryPageId;
} bmpeg2ts_psi_dvb_subtitle;

typedef struct bmpeg2ts_psi_language {
    bool valid;
    uint8_t type;
    uint8_t code[4]; /* 3 ascii codes, add one more for "\0" */
} bmpeg2ts_psi_language;

/* 
 * pointer bmedia_probe_track should  be converted to bmpeg2ts_psi_probe_track only if following conditions met:
 * 1. bmedia_probe_stream.probe_id == BMPEG2TS192_PSI_PROBE_ID || bmedia_probe_stream.probe_id == BMPEG2TS_PSI_PROBE_ID
 * 2. bmedia_probe_track.program != BMEDIA_PROBE_INVALID_PROGRAM
 */
typedef struct bmpeg2ts_psi_probe_track {
    bmedia_probe_track media;
    bmpeg2ts_psi_transport_type transport_type;
    size_t descriptor_size; /* size of the descriptor data, 0 if unavailable */
    const void *descriptor;
    bmpeg2ts_psi_dvb_subtitle dvb_subtitle;
    bmpeg2ts_psi_language language;
} bmpeg2ts_psi_probe_track;

extern const bmedia_probe_format_desc bmpeg2ts_psi_probe;
extern const bmedia_probe_format_desc bmpeg2ts192_psi_probe;

#ifdef __cplusplus
}
#endif


#endif  /* __BMPEG2TS_PSI_PROBE_H__ */

