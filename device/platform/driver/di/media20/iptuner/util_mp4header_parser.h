/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_mp4header_parser.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: Jeehune Park $
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.09.24
 * File Description:	mp4 header parser
 * Module:
 * Remarks:
 */

/**
 * @defgroup UTIL_MP4HEADER_PARSER
 * @brief mp4 header parser ("moov")
 */

/**
 * @author Jeehune Park (Parkjh4@humaxdigital.com)
 * @date 24 Sep 2011
 */

/**
 * @brief implementation of mp4 header parser
 */


/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

/*@{*/

/**
 * @file util_mp4header_parser.h
 */

#ifndef __UTIL_MP4HEADER_PARSER_H__
#define __UTIL_MP4HEADER_PARSER_H__

#include <stdlib.h>
#include "htype.h"
#include "vkernel.h"

#define UTIL_MP4_BOXDATA_START (0x8)
#define UTIL_MP4_SIZE_VERSION (0x1)
#define UTIL_MP4_SIZE_FLAGS (0x3)
#define UTIL_MP4_BOXDATA_START (0x8)
#define UTIL_MP4_SIZE_VERSION (0x1)
#define UTIL_MP4_SIZE_FLAGS (0x3)
#define UTIL_MP4_SIZE_B64	(0x8)
#define UTIL_MP4_SIZE_B48	(0x6)
#define UTIL_MP4_SIZE_B32	(0x4)
#define UTIL_MP4_SIZE_B24	(0x3)
#define UTIL_MP4_SIZE_B16	(0x2)
#define UTIL_MP4_SIZE_B8	(0x1)
#define UTIL_MP4_SIZE_BRAND (0x5)

typedef enum
{
	UTIL_MP4_OK,
	UTIL_MP4_FAILED,
	UTIL_MP4_BOX_NOT_FOUND,
	UTIL_MP4_BOX_NOT_FOUND_KEEP_GOING
} UTIL_MP4_ERRCODE;

typedef enum
{
    UTIL_MP4_HDL_VIDE,
    UTIL_MP4_HDL_SOUN,
    UTIL_MP4_HDL_HINT,
    UTIL_MP4_HDL_META        
} UTIL_MP4_HDL_t;

typedef enum
{
	UTIL_MP4_CODEC_MP4A,
	UTIL_MP4_CODEC_AVC1
} UTIL_MP4_CODEC_t;

typedef union 
{
	/***********************
	*  b64 for version==1, 32 for 0
	************************/
	HUINT64 b64;
	HUINT32 b32;
} UTIL_MP4_VERSION_t;

typedef struct UTIL_MP4_PARSER_MEM_FILE_t {
	HBOOL bIsOpened;
    HUINT8 *buf;
    size_t buf_len;
    size_t length;
    size_t offset;
} UTIL_MP4_PARSER_MEM_FILE_t;

typedef struct UTIL_MP4_PARSER_AVC1_AVCC
{
	HUINT8	version;
	HUINT8	h264_profile;
	HUINT8	h264_compatible_profile;
	HUINT8	h264_level;
	HUINT8	reserved_6b;
	HUINT8	NAL_length_2b;
	HUINT8	nrSPS;
	HUINT8	nrPPS;
	HUINT8	SPSlen;
	HUINT8	PPSlen;
	UTIL_DLL_t	SPSNAL;
	UTIL_DLL_t	PPSNAL;	
}UTIL_MP4_PARSER_AVC1_AVCC_t;

typedef struct UTIL_MP4_PARSER_AVC1
{
	HUINT8 	reserved_12b[6];
	HUINT16 data_ref_idx;
	HUINT16 enc_version;
	HUINT16 enc_level;
	HUINT32 vendor;
	HUINT32 temp_quality;
	HUINT32 spatial_quality;
	HUINT16 width;
	HUINT16 height;
	HUINT32 h_resol;
	HUINT32 v_resol;
	HUINT32 reserved_32b;
	HUINT16 pre_defined1;
	HUINT8 	name_len;
	HUINT8  compressor_name[32];
	HUINT16 depth;
	HUINT16 pre_defined2;
	UTIL_MP4_PARSER_AVC1_AVCC_t avcc;
}UTIL_MP4_PARSER_AVC1_t;

typedef struct UTIL_MP4_PARSER_MP4A
{
	HUINT8 reserved[6];
	HUINT16 data_reference_index;
	HUINT64 reserved_b64;
	HUINT16	channel_count;
	HINT16	sample_size;
	HINT16 pre_defined;
	HINT16 reserved_b16;
	HUINT16 time_scale;
	HINT16 reserved_b16_2;
}UTIL_MP4_PARSER_MP4A_t;

typedef struct UTIL_MP4_PARSER_STSD_s
{
	UTIL_DLL_t entry;
	UTIL_DLL_t codecInfo;
} UTIL_MP4_PARSER_STSD_t;

typedef struct UTIL_MP4_PARSER_STTS_ENTRY_s
{
    HUINT32 sample_count;
    HUINT32 sample_delta;
} UTIL_MP4_PARSER_STTS_ENTRY_t;

typedef struct UTIL_MP4_PARSER_STTS_s
{
    UTIL_DLL_t sample; /* UTIL_MP4_PARSER_STTS_ENTRY_t */
} UTIL_MP4_PARSER_STTS_t;

typedef struct UTIL_MP4_PARSER_STSS_s
{
    UTIL_DLL_t sample; /* HUINT32 */
} UTIL_MP4_PARSER_STSS_t;

typedef struct UTIL_MP4_PARSER_STSC_ENTRY_s
{
    HUINT32 first_chunk;
    HUINT32 samples_per_chunk;
    HUINT32 sample_description_index;
} UTIL_MP4_PARSER_STSC_ENTRY_s;

typedef struct UTIL_MP4_PARSER_STSC_t
{
    UTIL_DLL_t sample; /* UTIL_MP4_PARSER_STSC_ENTRY_t */
} UTIL_MP4_PARSER_STSC_t;

typedef struct UTIL_MP4_PARSER_STSZ_s
{
    HUINT32 szSample;
    UTIL_DLL_t sample; /* HUINT32 */
} UTIL_MP4_PARSER_STSZ_t;

typedef struct UTIL_MP4_PARSER_STCO_s
{
    UTIL_DLL_t chunk; /* HUINT32 */
} UTIL_MP4_PARSER_STCO_t;

typedef struct UTIL_MP4_PARSER_STBL_s
{
	UTIL_MP4_PARSER_STSD_t stsd;
	UTIL_MP4_PARSER_STTS_t stts;
	UTIL_MP4_PARSER_STSS_t stss;
	UTIL_MP4_PARSER_STSC_t stsc;
	UTIL_MP4_PARSER_STSZ_t stsz;
	UTIL_MP4_PARSER_STCO_t stco;
} UTIL_MP4_PARSER_STBL_t;

typedef struct UTIL_MP4_PARSER_NMHD_s
{
	HINT8 reserved;
} UTIL_MP4_PARSER_NMHD_t;

typedef struct UTIL_MP4_PARSER_HMHD_s
{
	HINT16	maxPDUsize;
	HINT16	avgPDUsize;
	HINT32	maxbitrate;
	HINT32	reserved;
} UTIL_MP4_PARSER_HMHD_t;

typedef struct UTIL_MP4_PARSER_SMHD_s
{
	HINT16	balance;
	HINT16	reserved;
} UTIL_MP4_PARSER_SMHD_t;

typedef struct UTIL_MP4_PARSER_VMHD_s
{
	HUINT16	graphicmode;
	HINT16	opcolor[3];
} UTIL_MP4_PARSER_VMHD_t;

typedef struct UTIL_MP4_PARSER_MINF_s
{
	UTIL_MP4_PARSER_VMHD_t vmhd;
	UTIL_MP4_PARSER_SMHD_t smhd;
	UTIL_MP4_PARSER_HMHD_t hmhd;
	UTIL_MP4_PARSER_NMHD_t nmhd;
	UTIL_MP4_PARSER_STBL_t stbl;
	HBOOL isVMHD;
	HBOOL isSMHD;
	HBOOL isHMHD;
	HBOOL isNMHD;
} UTIL_MP4_PARSER_MINF_t;

typedef struct UTIL_MP4_PARSER_HDLR_s
{
	HUINT32	pre_defined;
	UTIL_MP4_HDL_t	handler_type;
	HUINT32	reserved[3];
	HINT8	*name;
}	UTIL_MP4_PARSER_HDLR_t;

typedef struct UTIL_MP4_PARSER_MDHD_s
{
	UTIL_MP4_VERSION_t creation_time;		/* UTC second */
	UTIL_MP4_VERSION_t modification_time;	/* UTC second */
	HINT32 timescale;
	UTIL_MP4_VERSION_t duration;
	HUINT8 language0;
	HUINT8 language1;
	HUINT8 language2;
	HUINT16	pre_defined;
} UTIL_MP4_PARSER_MDHD_t;

typedef struct UTIL_MP4_PARSER_MDIA_s
{
	UTIL_MP4_PARSER_MDHD_t mdhd;
	UTIL_MP4_PARSER_HDLR_t hdlr;
	UTIL_MP4_PARSER_MINF_t minf;
	HBOOL	isHDLR;
} UTIL_MP4_PARSER_MDIA_t;

typedef struct UTIL_MP4_PARSER_TREF_s
{
	/* Will not parse */
	HINT8 reserved;
} UTIL_MP4_PARSER_TREF_t;

typedef struct UTIL_MP4_PARSER_TKHD_s
{
	UTIL_MP4_VERSION_t creation_time;		/* UTC second */
	UTIL_MP4_VERSION_t modification_time;	/* UTC second */
	HUINT32 track_ID;
	HINT32 reserved;
	UTIL_MP4_VERSION_t duration;
	HUINT32	reserved_2[2];
	HINT16	layer;
	HINT16  alternate_group;
	HINT16	volume;
	HUINT16	reserved_3;
	HINT32	matrix[9];
	HINT32	width;
	HINT32	height;
} UTIL_MP4_PARSER_TKHD_t;

typedef struct UTIL_MP4_PARSER_TRAK_s
{
	UTIL_MP4_PARSER_TKHD_t tkhd;
	UTIL_MP4_PARSER_TREF_t tref;	/* don't parse this */
	UTIL_MP4_PARSER_MDIA_t mdia;
	HBOOL isTREF;					/* existence of tref box */
	
} UTIL_MP4_PARSER_TRAK_t;

typedef struct UTIL_MP4_PARSER_MVHD_s
{
	/* version is 0 or 1 */
	UTIL_MP4_VERSION_t creation_time;		/* UTC second */
	UTIL_MP4_VERSION_t modification_time;	/* UTC second */
	HINT32 timescale;
	UTIL_MP4_VERSION_t duration;
	HINT32	rate;							/* 16:16 as fixed point - eg 1.0 (0x00010000) is normal playback speed */
	HINT16	volume;							/* 8:8 " - eg 1.0(0x0100) is full volume */
	HINT16  reserved;
    HINT32	reserved_array[2];					
	HINT32	matrix[9];						/* transformation matrix for the video */
	HINT32	pre_defined[6];
	HUINT32	next_track_ID;	
} UTIL_MP4_PARSER_MVHD_t;

typedef UTIL_DLL_t MP4_TRAK_Set;

typedef struct UTIL_MP4_PARSER_TREX_s
{
	HUINT32 trackID;
	HUINT32 default_sample_description_index;
	HUINT32 default_sample_duration;
	HUINT32 default_sample_size;
	HUINT32 default_sample_flags;
} UTIL_MP4_PARSER_TREX_t;

typedef struct UTIL_MP4_PARSER_MVEX_s
{
	UTIL_DLL_t trex;
} UTIL_MP4_PARSER_MVEX_t;

typedef struct UTIL_MP4_PARSER_MOOV_s
{
	UTIL_MP4_PARSER_MVHD_t mvhd;
	UTIL_MP4_PARSER_MVEX_t mvex;
	MP4_TRAK_Set trak;
} UTIL_MP4_PARSER_MOOV_t;

typedef struct UTIL_MP4_PARSER_FTYP_s
{
    HUINT32 major_brand;
    HUINT32 minor_version;
    UTIL_DLL_t compatible_brand;
} UTIL_MP4_PARSER_FTYP_t;

typedef struct UTIL_MP4_PARSER_s
{
    UTIL_MP4_PARSER_MOOV_t moov;
    UTIL_MP4_PARSER_FTYP_t ftyp;
    void* fd;           
} UTIL_MP4_PARSER_t;

typedef struct UTIL_MP4_BOX_INFO_s
{
	HUINT32 nStartOffset;
	HUINT32 nBoxSize;
	HBOOL isValid;
} UTIL_MP4_BOX_INFO_t;

typedef HUINT32 UTIL_MP4_LE_VALUE ;

UTIL_MP4_ERRCODE UTIL_MP4_PARSER_Parse(UTIL_MP4_PARSER_t* pMP4, HINT8* pBuf, HINT32 size);
UTIL_MP4_ERRCODE UTIL_MP4_PARSER_Release(UTIL_MP4_PARSER_t* pMP4);

#endif
