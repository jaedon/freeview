/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   hls_m3u8_parser.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: Yang Hyun Uk $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2011.07.20
 * File Description:	M3U8 URL Parser
 * Module:
 * Remarks:
 */

/**
 * @ingroup M3U8_Parser
 */

/**
 * @author Hyunuk Yang(huyang@humaxdigital.com), YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 22 July 2011
 */

/**
 * @note
 * Copyright (C) 2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

/*@{*/

/**
 * @file hls_m3u8_parser.h
 */

#ifndef __HLS_M3U8_PARSER_H__
#define __HLS_M3U8_PARSER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <unistd.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include "util_dllist.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */

#define M3U8_CONST_MAX_TAGNAME 64

/// @name Boolean Define
//@{
#define TRUE 1
#define FALSE 0
//@}


/// @name M3U8 Tag Name String Define
//@{
/**
 * M3U8 TAG
 */
#define M3U8_TAG_EXTM3U	"#EXTM3U"

/**
 * EXTINF is a record marker that describes the media file identified by
the URI that follows it. Each media file URI MUST be preceded by an
EXTINF tag. Its format is: <br><br>
#EXTINF:[duration],[title] <br><br>
"duration" is an integer or floating-point number that specifies the
duration of the media file in seconds. Integer durations SHOULD be
rounded to the nearest integer. Durations MUST be integers if the
protocol version of the Playlist file less than 3. The remainder of
the line following the comma is the title of the media file, which is
an optional human-readable informative title of the media segment.
 */
#define M3U8_TAG_EXTINF	"#EXTINF"

/**
 * The EXT-X-TARGETDURATION tag specifies the maximum media file
duration. The EXTINF duration of each media file in the Playlist
file MUST be less than or equal to the target duration. This tag
MUST appear once in the Playlist file. Its format is:<br><br>
#EXT-X-TARGETDURATION:[s]<br><br>
where s is an integer indicating the target duration in seconds.
 */
#define M3U8_TAG_EXT_X_TARGETDURATION "#EXT-X-TARGETDURATION"

/**
 * Each media file URI in a Playlist has a unique integer sequence
number. The sequence number of a URI is equal to the sequence number
of the URI that preceded it plus one. The EXT-X-MEDIA-SEQUENCE tag
indicates the sequence number of the first URI that appears in a
Playlist file. Its format is:<br><br>
#EXT-X-MEDIA-SEQUENCE:[number]<br><br>
A Playlist file MUST NOT contain more than one EXT-X-MEDIA-SEQUENCE
tag. If the Playlist file does not contain an EXT-X-MEDIA-SEQUENCE
tag then the sequence number of the first URI in the playlist SHALL
be considered to be 0. <br><br>
A media file’s sequence number is not required to appear in its URI.
 */
#define M3U8_TAG_EXT_X_MEDIA_SEQUENCE	"#EXT-X-MEDIA-SEQUENCE"

/**
 * Media files MAY be encrypted. The EXT-X-KEY tag provides information
necessary to decrypt media files that follow it. Its format is: <br><br>
#EXT-X-KEY:METHOD=[method][,URI="URI"][,IV=<IV>]<br><br>
The METHOD attribute specifies the encryption method. Two encryption
methods are defined: NONE and AES-128. <br><br>
An encryption method of NONE means that media files are not
encrypted. If the encryption method is NONE, the URI and the IV
attributes MUST NOT be present.<br><br>
If the Playlist file does not contain an EXT-X-KEY tag then media
files are not encrypted.
 */
#define M3U8_TAG_EXT_X_KEY			"#EXT-X-KEY"

/**
 * The EXT-X-BYTERANGE tag indicates that a media segment is a sub-range
of the resource identified by its media URI. It applies only to the
next media URI that follows it in the Playlist. Its format is:
#EXT-X-BYTERANGE:<n>[@o]
where n is a decimal-integer indicating the length of the sub-range
in bytes. If present, o is a decimal-integer indicating the start of
the sub-range, as a byte offset from the beginning of the resource.
If o is not present, the sub-range begins at the next byte following
the sub-range of the previous media segment.
If o is not present, a previous media segment MUST appear in the
Playlist file and MUST be a sub-range of the same media resource.
A media URI with no EXT-X-BYTERANGE tag applied to it specifies a
media segment that consists of the entire resource.
The EXT-X-BYTERANGE tag appeared in version 4 of the protocol.
 */
#define M3U8_TAG_EXT_X_BYTERANGE	"#EXT-X-BYTERANGE"

/**
 * The EXT-X-PLAYLIST-TYPE tag provides mutability information about the
Playlist file. It applies to the entire Playlist file. It is
optional. Its format is:
#EXT-X-PLAYLIST-TYPE:<EVENT|VOD>
 */
#define M3U8_TAG_EXT_X_PLAYLIST_TYPE	"#EXT-X-PLAYLIST-TYPE"

/**
 * The EXT-X-MEDIA tag is used to relate Playlists that contain
alternative renditions of the same content. For example, three EXTX-
MEDIA tags can be used to identify audio-only Playlists that
contain English, French and Spanish renditions of the same
presentation. Or two EXT-X-MEDIA tags can be used to identify videoonly
Playlists that show two different camera angles.
The EXT-X-MEDIA tag stands alone, in that it does not apply to a
particular URI in the Playlist. Its format is:
#EXT-X-MEDIA:<attribute-list>
 */
#define M3U8_TAG_EXT_X_MEDIA	"#EXT-X-MEDIA"

/**
 * The EXT-X-I-FRAMES-ONLY tag indicates that each media segment in the
Playlist describes a single I-frame. I-frames (or Intra frames) are
encoded video frames whose encoding does not depend on any other
frame.
The EXT-X-I-FRAMES-ONLY tag applies to the entire Playlist. Its
format is:
#EXT-X-I-FRAMES-ONLY
 */
#define M3U8_TAG_EXT_X_I_FRAMES_ONLY	"#EXT-X-I-FRAMES-ONLY"

/**
 * The EXT-X-I-FRAME-STREAM-INF tag identifies a Playlist file
containing the I-frames of a multimedia presentation. It stands
alone, in that it does not apply to a particular URI in the Playlist.
Its format is:
#EXT-X-I-FRAME-STREAM-INF:<attribute-list>
 */
#define M3U8_TAG_EXT_X_I_FRAME_STREAM_INF	"#EXT-X-I-FRAME-STREAM-INF"

/**
 * The EXT-X-PROGRAM-DATE-TIME tag associates the beginning of the next
media file with an absolute date and/or time. The date/time <br><br>
representation is ISO/IEC 8601:2004 [ISO_8601] and SHOULD indicate a
time zone. For example: <br><br>
#EXT-X-PROGRAM-DATE-TIME:[YYYY-MM-DDThh:mm:ssZ]
 */
#define M3U8_TAG_EXT_X_PROGRAM_DATE_TIME	"#EXT-X-PROGRAM-DATE-TIME"

/**
 * The EXT-X-ALLOW-CACHE tag indicates whether the client MAY or MUST
NOT cache downloaded media files for later replay. It MAY occur
anywhere in the Playlist file; it MUST NOT occur more than once. The
EXT-X-ALLOW-CACHE tag applies to all segments in the playlist. Its
format is: <br><br>
#EXT-X-ALLOW-CACHE:[YES|NO]
 */
#define M3U8_TAG_EXT_X_ALLOW_CACHE			"#EXT-X-ALLOW-CACHE"

/**
 * The EXT-X-ENDLIST tag indicates that no more media files will be
added to the Playlist file. It MAY occur anywhere in the Playlist
file; it MUST NOT occur more than once. Its format is:<br><br>
#EXT-X-ENDLIST
 */
#define M3U8_TAG_EXT_X_ENDLIST				"#EXT-X-ENDLIST"

/**
 * The EXT-X-STREAM-INF tag indicates that the next URI in the Playlist
file identifies another Playlist file. Its format is:<br><br>
#EXT-X-STREAM-INF:[attribute=value][,attribute=value]*<br>
[URI] <br><br>
An attribute MUST NOT occur more than once in the same EXT-X-STREAMINF
tag. The following attributes are defined:<br><br>
BANDWIDTH=[n]<br><br>
where n is an integer number of bits per second. It MUST be an upper
bound of the overall bitrate of each media file, calculated to
include container overhead, that appears or will appear in the
Playlist.<br><br>
PROGRAM-ID=[i]<br><br>
where i is an integer that uniquely identifies a particular
presentation within the scope of the Playlist file.
A Playlist file MAY contain multiple EXT-X-STREAM-INF tags with the
same PROGRAM-ID to identify different encodings of the same
presentation. These variant playlists MAY contain additional EXT-XSTREAM-
INF tags.<br><br>
CODECS="[format][,format]*"<br><br>
where each format specifies a media sample type that is present in a
media file in the Playlist file.
Valid format identifiers are those in the ISO File Format Name Space
defined by RFC 4281 [RFC4281].<br><br>
RESOLUTION=[N]x[M]<br><br>
where N is the approximate encoded horizontal resolution of video
within the stream, expressed as a number of pixels, and M is the
approximate encoded vertical resolution. N and M are integers.
 */
#define M3U8_TAG_EXT_X_STREAM_INF			"#EXT-X-STREAM-INF"

/**
 * The EXT-X-DISCONTINUITY tag indicates an encoding discontinuity
between the media file that follows it and the one that preceded it.
The set of characteristics that MAY change is:<br><br>
o file format <br>
o number and type of tracks<br>
o encoding parameters<br>
o encoding sequence<br>
o timestamp sequence<br><br>
Its format is:<br><br>
#EXT-X-DISCONTINUITY
 */
#define M3U8_TAG_EXT_X_DISCONTINUITY		"#EXT-X-DISCONTINUITY"

/**
 * The EXT-X-VERSION tag indicates the compatibility version of the
Playlist file. The Playlist file, its associated media, and its
server MUST comply with all provisions of the most-recent version of
this document describing the protocol version indicated by the tag
value.<br>
Its format is:<br><br>
#EXT-X-VERSION:[n]<br><br>
where n is an integer indicating the protocol version.<br>
A Playlist file MUST NOT contain more than one EXT-X-VERSION tag. A
Playlist file that does not contain an EXT-X-VERSION tag MUST comply
with version 1 of this protocol.
 */
#define M3U8_TAG_EXT_X_VERSION		"#EXT-X-VERSION"
//@}

#define M3U8_ABSOLUTE_PATH			'/'

/// @name Parsing을 위한 Attributes Name String Define
//@{
#define M3U8_ATTR_PROGRAM_ID		"PROGRAM-ID"
#define M3U8_ATTR_BANDWIDTH			"BANDWIDTH"
#define M3U8_ATTR_CODECS			"CODECS"
#define M3U8_ATTR_RESOLUTION		"RESOLUTION"
#define M3U8_ATTR_VIDEO				"VIDEO"
#define M3U8_ATTR_AUDIO				"AUDIO"
#define M3U8_ATTR_TYPE				"TYPE"
#define M3U8_ATTR_GROUP_ID			"GROUP-ID"
#define M3U8_ATTR_LANGUAGE			"LANGUAGE"
#define M3U8_ATTR_NAME				"NAME"
#define M3U8_ATTR_DEFAULT			"DEFAULT"
#define M3U8_ATTR_AUTOSELECT		"AUTOSELECT"
#define M3U8_ATTR_STARTOFFSET		"@"
#define M3U8_ATTR_METHOD			"METHOD"
#define M3U8_ATTR_URI				"URI"
#define M3U8_ATTR_IV				"IV"
#define M3U8_STRING_EVENT			"EVENT"
#define M3U8_STRING_VOD				"VOD"
#define M3U8_STRING_YES				"YES"
#define M3U8_STRING_NO				"NO"
#define M3U8_STRING_AESCBC_128		"AES-128"
#define M3U8_STRING_AES_128			M3U8_STRING_AESCBC_128
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
#define M3U8_STRING_NONE			"NONE"
#endif
//@}

/// @name Parsing을 위한 상수 값 Define
//@{
#define M3U8_CONST_VALUE_BUFFSIZE 	(256)
#define M3U8_CONST_DEFAULT_PROGRAM_ID 	(-1)
#define M3U8_CONST_DUMMY_PROGRAM_ID		(-2)
//@}

/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/**
 * Parser ErrorCode
 */
typedef enum
{
	M3U8_ERR_OK,
	M3U8_ERR_FAIL
} m3u8Err_e;

/**
 * HLS 의 PLAY TYPE
 */
typedef enum
{
	M3U8_PLAYTYPE_NONE, /**< NONE */
	M3U8_PLAYTYPE_EVENT, /**< EVENT */
	M3U8_PLAYTYPE_VOD /**< VOD */
} m3u8PlayType_e;

/**
 * HLS Stream 의 AV TYPE
 */
typedef enum
{
	M3U8_MEDIA_AV, /**< A/V */
	M3U8_MEDIA_VIDEO, /**< VIDEO */
	M3U8_MEDIA_AUDIO /**< AUDIO */
} m3u8MediaType_e;

/**
 * HLS codec definition
 */
typedef enum
{
	HLS_CODEC_NONE 		= 0x00000000,
	HLS_AUDIO_CODEC_AC3 	= 0x00000001,
	HLS_AUDIO_CODEC_ALAC 	= 0x00000002,
	HLS_AUDIO_CODEC_DRA1 	= 0x00000004,
	HLS_AUDIO_CODEC_DTSC 	= 0x00000008,
	HLS_AUDIO_CODEC_DTSE 	= 0x00000010,
	HLS_AUDIO_CODEC_DTSH 	= 0x00000020,
	HLS_AUDIO_CODEC_DTSL 	= 0x00000040,
	HLS_AUDIO_CODEC_EC3 	= 0x00000080,
	HLS_AUDIO_CODEC_ENCA 	= 0x00000100,
	HLS_AUDIO_CODEC_G719 	= 0x00000200,
	HLS_AUDIO_CODEC_G726 	= 0x00000400,
	HLS_AUDIO_CODEC_M4AE 	= 0x00000800,
	HLS_AUDIO_CODEC_MLPA 	= 0x00001000,
	HLS_AUDIO_CODEC_AAC 	= 0x00002000,
	HLS_AUDIO_CODEC_AAC_HE	= 0x00004000,
	HLS_AUDIO_CODEC_AAC_HE2= 0x00008000,
	HLS_AUDIO_CODEC_MP4A 	= 0x00010000,
//	HLS_AUDIO_CODEC_UNCOMP = 0x00008000,
//	HLS_AUDIO_CODEC_SAMR 	= 0x00010000,
	HLS_AUDIO_CODEC_SAWB 	= 0x00020000,
	HLS_AUDIO_CODEC_SAWP 	= 0x00040000,
	HLS_AUDIO_CODEC_SEVC 	= 0x00080000,
	HLS_AUDIO_CODEC_SQCP 	= 0x00100000,
	HLS_AUDIO_CODEC_SSMV 	= 0x00200000,
	HLS_AUDIO_CODEC_UNCOMP_16bAudio = 0x00400000,
	HLS_AUDIO_CODECS		= 0x004FFFFF,		/**< Audio codec filter */
	HLS_VIDEO_CODEC_MP4V	= 0x00800000,
	HLS_VIDEO_CODEC_AVC1 	= 0x01000000,
	//HLS_VIDEO_CODEC_AVC2 = 0x02000000,
	HLS_VIDEO_CODEC_AVC3 	= 0x02000000,
	HLS_VIDEO_CODEC_HEVC 	= 0x04000000,
	//HLS_VIDEO_CODEC_AVCP 	= 0x04000000,
	HLS_VIDEO_CODEC_DRAC	= 0x08000000,
	HLS_VIDEO_CODEC_ENCV	= 0x10000000,
	HLS_VIDEO_CODEC_S263	= 0x20000000,
	HLS_VIDEO_CODEC_VC1	= 0x40000000,
	//HLS_VIDEO_CODEC_MJP2,
	//HLS_VIDEO_CODEC_MVC1,
	//HLS_VIDEO_CODEC_MVC2,
	//HLS_VIDEO_CODEC_SVC,
	HLS_VIDEO_CODECS		= 0x4F800000,		/**< Video codec filter */
	//HLS_TEXT_CODEC_ENCT	= 0x80000000,
	//HLS_TEXT_CODECS		= 0x80000000,
}HLS_CODEC_e;

/**
 * HLS 의 Encryption Algorithm.
 */
typedef enum
{
	M3U8_METHOD_UNKNOWN, /**< UNKNOWN Method */
	M3U8_METHOD_AESCBC128 /**< AES-CBC 128 */
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
	,M3U8_METHOD_NONE /**< not encrypted */
#endif
} m3u8Method_e;

/**
 * HLS 의 Encryption Padding Algorithm.
 */
typedef enum
{
	M3U8_ENCPADDING_UNKNOWN, /**< UNKNOWN Method */
	M3U8_ENCPADDING_PKCS7 /**< PKCS#7 */
} m3u8EncPadding_e;

/**
 * Double Linked List 구조로 각 Element 들을 구성
 * \See UTIL_DLList
 */
typedef UTIL_DLL_t m3u8SegmentSet_t; /**< Segment Structure Set */
typedef UTIL_DLL_t m3u8KeySet_t; /**< Key Structure Set */
typedef UTIL_DLL_t m3u8ProgramSet_t; /**< Program Structure Set */
typedef UTIL_DLL_t m3u8StreamSet_t; /**< Stream Structure Set */
typedef UTIL_DLL_t m3u8PlaylistSet_t; /**< Playlist Structure Set */
typedef UTIL_DLLNode_t m3u8Node_t; /**< m3u8 의 Structure 탐색을 위한 Node Structure redifine */

/**
 * Key 에 대한 struct 로 한 Key 에 대한 모든 정보를 저장한다.
 */
typedef struct m3u8Key_s
{
	HINT8* key_uri; /**< Key의 Uri */
	m3u8Method_e eMethod; /**< Key의 Method(Algorithm) */
	m3u8EncPadding_e eEncPadding; /**< Padding Method(Algorithm) */
	HUINT32 uUsedCount;	/* 해당 key를 사용중인 segment의 count */
	HUINT8* pIV; /**< Initialize Vector Value */
} m3u8Key_t;

/**
 * Segment 에 대한 struct 로 한 Segment 에 대한 모든 정보를 저장한다.
 */
typedef struct m3u8Segment_s
{
	HUINT64 ullNum; /**< 세그먼트의 Sequence Number */
	HBOOL	bDiscontinuity; /**< 현 Segment 부터 Discontinuity 적용 */
	HBOOL	bLastSegment; /**< 마지막 Segment (ENDLIST) */
	HBOOL	bResetSequence;
	HLONG 	datetime; /**< Program DateTime */
	HFLOAT32 duration_of_segment; 	/**< 세그먼트의 duration */
	HFLOAT32 starttime_of_segment;   /**< 세그먼트의 Start Time */
	m3u8Key_t *pKey; 				/**< key url. Full URL 이 아닐 경우, M3U8 의 base_url 과 조합하여 사용된다.*/
	HINT8* segment_url;			/**< segment url. Full URL 이 아닐 경우, M3U8 의 base_url 과 조합하여 사용된다.*/
	HINT8* backup_url;			/**< backup segment url. Full URL 이 아닐 경우, M3U8 의 base_url 과 조합하여 사용된다. (현재 사용 안함) */
	HBOOL	bChangedTs;		/* 하나의 m3u8파일에 여러 종류의 TS 가 있는 경우 */
	HBOOL bByteRangeSupport;		/**< ByteRange Support 여부 */
	HUINT64 ullByteRangeLength;	/**< byte 단위로 재생 길이를 10 진수로 표현 */
	HUINT64 ullByteRangeOffset;	/**< 해당 세그먼트의 offset 값 */
	HUINT64 ullByteRangeIndex;		/**< 같은 url 을 가진 세그먼트를 구분하는 index */
} m3u8Segment_t;

/**
 * Stream 에 대한 struct 로 한 Stream 에 대한 모든 정보를 저장한다.
 * Stream은 Bandwidth로 구분된다.
 */
typedef struct m3u8Stream_s
{
	HBOOL	bEncrypted; /**< Stream이 Encryption 여부 */
	HBOOL bAllowCache; /**< Allow Cache */
	m3u8PlayType_e ePlayType; /**< PlayType */
	HUINT32 unVersion; /**< Version */
	HBOOL	bIFrameOnly; /**< I-Frame Only Playlist */
	HBOOL	bEndList; /**< EDNLIST를 포함하는 Stream인지 여부 */
//	HUINT32 unStreamDuration; /**< Stream의 Duration */
	HUINT32 unTargetDuration; /**< Stream의 Target Duration */
	HUINT64 ullSequenceNum; /**< Stream의 First Segment의  Sequencial Number */
	HUINT64 ullLastSequenceNum; /**< Stream의 Last updated Sequencial Number(from m3u8 EXT-X-MEDIA-SEQUENCE) */
	HUINT32 unBandwidth; /**< Stream의 Bandwidth */
	m3u8MediaType_e eMediaType; /**< Stream의 Audio/Video Type */
	HLS_CODEC_e eCodec;
	m3u8KeySet_t keySet; /**< Stream에 사용되는 Key Set */
	m3u8SegmentSet_t segmentSet; /**< Stream내의 전체 Segment Set */
} m3u8Stream_t;

/**
 * Program 에 대한 struct 로 한 Program 에 대한 모든 정보를 저장한다.
 */
typedef struct m3u8Program_s
{
	HINT32 nProgramId; /**< Prograrm Id */
	m3u8StreamSet_t streamSet; /**< Program에 속하는 Stream의 Set */
} m3u8Program_t;

/**
 * Multiple M3U8 Parsing 을 위한 M3U8 File 하나에 대한 structure
 */
typedef struct m3u8Playlist_s
{
	HBOOL bMasterPlaylist;
	HINT32 nProgramId;  /**< Prograrm Id */
	HINT8 *pUrl;  /**< File의 Url */
	HINT8 *pBaseUrl;  /**< File의 Base Url */
	HUINT32 unDuration;  /**< 하나의 Stream 전체 Duration 측정을 위한 변수 */
	HUINT64 ullSequenceNum; /**< SequenceNum */
	HBOOL bUpdated;
	HUINT32 unSegmentNum; /**< Segment Number (0 ~ ) */
	m3u8Stream_t *pStream; /**< m3u8 file에 속한 Stream의 정보 */
	/** Private */
	m3u8Key_t* pCurKey; /**< Stream 내의 Segment Parsing 중 해당 Segment가 사용해야할 현재의 Key */
} m3u8Playlist_t;

/**
 * M3U8 전체 Struct 구조체로 모든 Segment 와 Key 에 대한 정보를 저장한다.
 */
typedef struct m3u8_s
{
	HINT8 *pTopUrl; /**< m3u8 access url */
	HBOOL bAllowCache; /**< Allow Cache */
	m3u8PlayType_e ePlayType; /**< PlayType */
	HUINT32 unVersion; /**< Version */

	m3u8ProgramSet_t programSet; /**< program set */
	m3u8PlaylistSet_t playlistSet; /**< playlist set(m3u8file) for parsing. */

// __SUPPORT_ABSOLUTE_PATH__
	int8_t * pHostUrl;
} m3u8_t;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*!
 *	\brief M3U8 Parser 의 Version을 Return 한다.
 *	\return Parser 의 Version
 *	\see
 */
HUINT32 HLS_M3U8_GetParserVersion(void);

/*!
 *	\brief M3U8 Parser Handle 생성 및 Parsing
 *	\param pM3U8Buf : M3U8 Data 를 로드한 버퍼
 *	\param unBufSize : 버퍼의 크기
 *	\param url : M3U8 File의 Url
 *	\return Parsing 된 M3U8 Handle
 *	\see M3U8 Parser Handle 생성 후 Parsing은 내부적으로 HLS_M3U8_Append 를 사용
 */
m3u8_t* HLS_M3U8_Create(HINT8* pM3U8Buf, HUINT32 unBufSize, HINT8* url, HINT8* pHostUrl);

/*!
 *	\brief M3U8 Parser Handle 제거
 *	\param m3u8 : M3U8 Handle
 *	\return ErrorCode
 *	\see
 */
m3u8Err_e HLS_M3U8_Destroy(m3u8_t *m3u8);

/*!
 *	\brief 생성된 M3U8 Handle에 M3U8 Data를 Parsing 후 추가.
 *	\param m3u8 : M3U8 Handle
 *	\param pPlaylist : Parsing에 부가적으로 필요한 m3u8 정보
 *	\param pM3U8Buf :  추가될 M3U8 Data
 *	\param unBufSize :  추가될 M3U8 Data 의 크기
 *	\return ErrorCode
 *	\see M3U8이 여러개의 파일로 구성된 경우에 반복적으로 호출
 */
m3u8Err_e HLS_M3U8_Update(m3u8_t* m3u8, m3u8Playlist_t* pPlaylist, HINT8* pM3U8Buf, HUINT32 unBufSize);

/*!
 *	\brief StreamNode 제거
 *	\param pStreamNode : Stream Node
 *	\return ErrorCode
 *	\see Stream이 사용 불가능할 경우 제거 / 이전에 해당 Stream Node는 StreamList에서 추출된 상태여야한다.
 */
m3u8Err_e HLS_M3U8_DiscardStream(m3u8Node_t *pStreamNode);

/*!
 *	\brief 해당 Segment 부터 시작 Segment 까지 제거 (fo Live Update)
 *	\param pStreamNode : Stream Node
 *	\param ullNum : Target Fragment Number
 *	\return ErrorCode
 *	\see Stream이 사용 불가능할 경우 제거 / 이전에 해당 Stream Node는 StreamList에서 추출된 상태여야한다.
 */
m3u8Err_e HLS_M3U8_DiscardSegments(m3u8Stream_t *pStream, HUINT64 ullNum);

/*!
 *	\brief keyset list에서 keyUri와 같은 key를 제거
 *	\param pStreamNode : Stream Node
 *	\param pKeyUri : 삭제할 key uri
 *	\return ErrorCode
 *	\see Stream이 사용 불가능할 경우 제거 / 이전에 해당 Stream Node는 StreamList에서 추출된 상태여야한다.
 */
m3u8Err_e HLS_M3U8_DiscardKey(m3u8Stream_t *pStream, HINT8* pKeyUri);

/*!
 *	\brief 해당 Segment 부터 시작 Segment 까지 Key 포함하여 제거  (fo Live Update)
 *	\param pStreamNode : Stream Node
 *	\param ullNum : Target Fragment Number
 *	\return ErrorCode
 *	\see Stream이 사용 불가능할 경우 제거 / 이전에 해당 Stream Node는 StreamList에서 추출된 상태여야한다.
 */
m3u8Err_e HLS_M3U8_DiscardSegmentsAndKeys(m3u8Stream_t *pStream, HUINT64 ullNum);

m3u8Err_e HLS_M3U8_DiscardSegmentsAndKeysUntilBaseSegment(m3u8Stream_t *pStream, m3u8Segment_t *pBaseSegment);

void HLS_M3U8_Print(m3u8_t *m3u8);

/*!
 *	\brief stream list의 END tag 유/무로 live인지 아닌지 판단한다.
 *	\param pStreamNode : Stream Node
 *	\return BOOL
 */
HBOOL HLS_M3U8_IsLiveStream(m3u8Stream_t *pStream);

/*!
 *	\brief 두 segment사이의 재생 시작 시간의 차이를 구한다.
 *	\param pStreamNode : Stream Node
 *	\ ullCurSegmentNum : 현재 segment의 sequence number
 *	\ ullLastSequenceNum : 마지막으로 segment의 sequence number
 *	\return HFLOAT32
 */
HFLOAT32 HLS_M3U8_GetTimeGapBetweenCurAndLive(m3u8Stream_t* pStream, HUINT64 ullCurSegmentNum, HUINT64 ullLastSequenceNum);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __HLS_M3U8_PARSER_H__ */

/*@}*/
