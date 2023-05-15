/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_asf.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	MMS ASF Header Parser
 * Module:
 * Remarks:
 */	

/**
 * @ingroup MMS_ASF_CONTROLLER
 */

/**
 * @author Jin Lee(jlee@humaxdigital.com)
 * @date 30 Jul 2012
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
 * @file mms_asf.h
 */

#ifndef _MMS_ASF_H_
#define _MMS_ASF_H_ 

#include "htype.h"

#define MMS_ASF_STREAM_VIDEO	0x0001
#define MMS_ASF_STREAM_AUDIO	0x0002
#define MMS_ASF_STREAM_UNKNOWN  0xffff

#define ASF_GUID_FMT "%8.8x-%4.4x-%4.4x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x"
#define ASF_GUID_PRINT( asf_guid )  \
	(asf_guid).v1,			  \
	(asf_guid).v2,			  \
	(asf_guid).v3,			  \
	(asf_guid).v4[0],(asf_guid).v4[1],(asf_guid).v4[2],(asf_guid).v4[3],	\
	(asf_guid).v4[4],(asf_guid).v4[5],(asf_guid).v4[6],(asf_guid).v4[7]

/**************** ADDED for RTSP ASF ************/
#define	MMS_ASF_GUID_LENGTH 16
#define	MMS_ASF_AUDIO_STREAM  0xf8699e40
#define	MMS_ASF_VIDEO_STREAM  0xbc19efc0
#define	ASF_COMMAND_MEDIA_STREAM  0x59dacfc0
#define	ASF_FILE_TRANSFER_MEDIA_STREAM  0x91bd222c
#define ASF_GUID_FMT_STRING_SIZE        91


typedef struct
{
	int i_cat;	  /* MMS_ASF_STREAM_VIDEO, MMS_ASF_STREAM_AUDIO */
	int i_bitrate;  /* -1 : unknown */
	int i_selected;
} ASF_STREAM_t;

typedef struct
{
	HINT32 audio_id,video_id; /* best stuff. (ids)   */
	HINT32 n_audio, n_video;  /* numbers of streams  */
	HINT32 *audio_streams,*video_streams; /* streams */
}ASF_STREAMS_t;

typedef enum
{
	MMS_ASF_CODEC_SIMPLE,
	MMS_ASF_CODEC_MAIN,
	MMS_ASF_CODEC_ADV
}eMMS_ASF_CODEC;

/** 
 * MMS ASF main handle
 */
typedef struct
{
	HINT64	  i_file_size;
	HINT64	  i_data_packets_count;
	HINT32	  i_min_data_packet_size;
	HINT32	  i_max_data_packet_size;
	HINT64	i_play_duration;  
	HINT64	i_send_duration;
	HINT32	i_max_bitrate;

	HINT32 	i_encrypted;
	eMMS_ASF_CODEC eVidCodec;

	HUINT8	*pRawAsfHeader;			  /* raw header		 */
	HINT32	nRawAsfHeaderLen;		  /* its length		 */

	ASF_STREAM_t stream[128];
	ASF_STREAMS_t *streams;

} ASF_HEADER_t;

/** 
 * MMS ASF GUID
 */
typedef struct guid_s
{
	HUINT32 v1; /* le */
	HUINT16 v2; /* le */
	HUINT16 v3; /* le */
	HUINT8  v4[8];
} ASF_GUID_t;


void  ASF_GenerateGuid ( ASF_GUID_t *p_guid );
void  ASF_ParseHeader ( ASF_HEADER_t *hdr, HUINT8 *p_header, int i_header );
void  ASF_SelectStream  ( ASF_HEADER_t *hdr, int i_bitrate_max, HBOOL b_all, HBOOL b_audio, HBOOL b_video );


/** 
 * REFERENCE ASF-Specification 10.1 Top-level ASF Object GUIDs 
 */
static const ASF_GUID_t asf_object_header_guid =
{
	0x75B22630,		/* V1 Value */
	0x668E,			/* V2 Value */
	0x11CF,
	{ 0xA6,0xD9, 0x00,0xAA,0x00,0x62,0xCE,0x6C }
};

/** 
 * ASF_File_Properties_Object : 10.2	Header Object GUIDs 
 */
static const ASF_GUID_t asf_object_file_properties_guid =
{
	0x8cabdca1,		/* V1 Value */
	0xa947,			/* V2 Value */
	0x11cf,
	{ 0x8e,0xe4, 0x00,0xC0,0x0C,0x20,0x53,0x65 }
};

/** 
 * ASF_Stream_Properties_Object : 10.2	Header Object GUIDs 
 */
static const ASF_GUID_t asf_object_stream_properties_guid =
{
	0xB7DC0791,		/* V1 Value */
	0xA9B7,			/* V2 Value */
	0x11CF,
	{ 0x8E,0xE6, 0x00,0xC0,0x0C,0x20,0x53,0x65 }
};

/** 
 * ASF_Stream_Bitrate_Properties_Object : 10.2	Header Object GUIDs 
 */
static const ASF_GUID_t asf_object_bitrate_properties_guid =
{
	0x7BF875CE,		/* V1 Value */
	0x468D,			/* V2 Value */
	0x11D1,
	{ 0x8D,0x82,0x00,0x60,0x97,0xC9,0xA2,0xB2 }
};

/** 
 * ASF_Bitrate_Mutual_Exclusion_Object : 10.2 Header Object GUIDs 
 */
static const ASF_GUID_t asf_object_bitrate_mutual_exclusion_guid =
{
	0xD6E229DC,		/* V1 Value */
	0x35DA,			/* V2 Value */
	0x11D1,
	{ 0x90,0x34,0x00,0xA0,0xC9,0x03,0x49,0xBE }
};

/** 
 * ASF_Header_Extension_Object : 10.2	Header Object GUIDs 
 */
static const ASF_GUID_t asf_object_header_extension_guid =
{
	0x5FBF03B5,		/* V1 Value */
	0xA92E,			/* V2 Value */
	0x11CF,
	{ 0x8E, 0xE3, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 }
};

/** 
 * ASF_Content_Encryption_Object : 10.2	Header Object GUIDs 
 */
static const ASF_GUID_t asf_object_content_encryption_guid =
{
	0x2211B3FB,		/* V1 Value */
	0xBD23,			/* V2 Value */
	0x11D2,
	{ 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E }
};

/** 
 * ASF_Extended_Content_Encryption_Object : 10.2	Header Object GUIDs 
 */
static const ASF_GUID_t asf_object_extended_content_encryption_guid =
{
	0x298AE614,		/* V1 Value */
	0x2622,			/* V2 Value */
	0x4C17,
	{ 0xB9, 0x35, 0xDA, 0xE0, 0x7E, 0xE9, 0x28, 0x9C }
};

/** 
 * ASF_Digital_Signature_Object : 10.2	Header Object GUIDs 
 */
static const ASF_GUID_t asf_object_digital_signature_guid =
{
	0x2211B3FC,		/* V1 Value */
	0xBD23,			/* V2 Value */
	0x11D2,
	{ 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E }
};

/** 
 * ASF_Extended_Stream_Properties_Object : 10.3	Header Extension Object GUIDs 
 */
static const ASF_GUID_t asf_object_extended_stream_properties_guid =
{
	0x14E6A5CB,		/* V1 Value */
	0xC672,			/* V2 Value */
	0x4332,
	{ 0x83, 0x99, 0xA9, 0x69, 0x52, 0x06, 0x5B, 0x5A }
};

/** 
 * ASF_Audio_Media : 10.4	Stream Properties Object Stream Type GUIDs 
 */
static const ASF_GUID_t asf_object_stream_type_audio =
{
	0xF8699E40,		/* V1 Value */
	0x5B4D,			/* V2 Value */
	0x11CF,
	{ 0xA8,0xFD, 0x00,0x80,0x5F,0x5C,0x44,0x2B }
};

/** 
 * ASF_Video_Media : 10.4	Stream Properties Object Stream Type GUIDs 
 */
static const ASF_GUID_t asf_object_stream_type_video =
{
	0xbc19efc0,		/* V1 Value */
	0x5B4D,			/* V2 Value */
	0x11CF,
	{ 0xA8,0xFD, 0x00,0x80,0x5F,0x5C,0x44,0x2B }
};

/**
 * ASF Object header 
 */
struct __attribute__((packed))  asf_obj_header_t {
	HUINT8 guid[MMS_ASF_GUID_LENGTH];
	HUINT64 size;
}; 


/**
 * MS ASF Header 
 */
struct __attribute__((packed)) asf_header_t {
	struct asf_obj_header_t objh; /* object header	   */
	HUINT32 cno;				 /* number of subchunks */
	HUINT8 v1;				   /* unknown (0x01)	  */
	HUINT8 v2;				   /* unknown (0x02)	  */
};


struct __attribute__((packed)) asf_stream_header_t {
	HUINT8 type[MMS_ASF_GUID_LENGTH];		/* Stream type (audio/video) GUID 16	*/
	HUINT8 concealment[MMS_ASF_GUID_LENGTH]; /* Audio error concealment type GUID 16 */
	HUINT64 unk1;		/* Unknown, maybe reserved ( usually contains 0 ) UINT64 8 */
	HUINT32 type_size;   /* Total size of type-specific data UINT32 4			   */
	HUINT32 stream_size; /* Size of stream-specific data UINT32 4				   */
	HUINT16 stream_no;   /* Stream number UINT16 2								  */
	HUINT32 unk2;		/* Unknown UINT32 4										*/
};

struct asf_streams_t {
	int audio_id,video_id; /* best stuff. (ids)   */
	int n_audio, n_video;  /* numbers of streams  */
	int *audio_streams,*video_streams; /* streams */
};


/*! 
 *	\brief Create ASF header handle
 *	\param void
 *	\return ASF_HEADER_t: header handle
 *	\see 
 */
ASF_HEADER_t *new_pAsfHeaderInfo_t(void);

/*! 
 *	\brief Destroy ASF header handle
 *	\param info: ASF_HEADER_t handle
 *	\return void
 *	\see 
 */
void free_pAsfHeaderInfo_t(ASF_HEADER_t *info);

/*! 
 *	\brief Parse the asf header and get the highest streams which works fine with specified bandwidth(bw).
 *	\param hinfo: returning asf header, bw: required bandwidth, buffer: header buffer, asf_header_size: header size
 *	\return int
 *	\see 
 */
int asf_interpret_header(ASF_HEADER_t *hinfo,const int bw,
			 HUINT8 *buffer,const size_t asf_header_size); /* (const HUINT8 *buffer) */

/*! 
 *	\brief Find GUID
 *	\param buffer: asf header buffer, cur_pos: start position, guid: ASF GUID value, buffer_len: limited buffer size for searching
 *	\return int: return point after (guid + length) if success and (-1) for failure
 *	\see 
 */
int find_asf_guid(const HUINT8 *buffer,int cur_pos,const HUINT8 *guid,int buffer_len);

/*! 
 *	\brief Select audio and video main(fastest) streams matched with specified bandwidth(bw).
 *	\param header: asf header buffer, asf_header_size: asf header size, bw: required bandwidth, streams: returning asf_streams_t
 *	\return int
 *	\see 
 */
int Find_Main_Asf_Streams(HUINT8 *header, int asf_header_size,
				int bw, struct asf_streams_t *streams); /* (const HUINT8 *header) */


#endif /* __MMS_ASF_H__ */



