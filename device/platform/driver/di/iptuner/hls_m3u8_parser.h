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
A media file��s sequence number is not required to appear in its URI.
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

/// @name Parsing�� ���� Attributes Name String Define
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

/// @name Parsing�� ���� ��� �� Define
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
 * HLS �� PLAY TYPE
 */
typedef enum
{
	M3U8_PLAYTYPE_NONE, /**< NONE */
	M3U8_PLAYTYPE_EVENT, /**< EVENT */
	M3U8_PLAYTYPE_VOD /**< VOD */
} m3u8PlayType_e;

/**
 * HLS Stream �� AV TYPE
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
 * HLS �� Encryption Algorithm.
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
 * HLS �� Encryption Padding Algorithm.
 */
typedef enum
{
	M3U8_ENCPADDING_UNKNOWN, /**< UNKNOWN Method */
	M3U8_ENCPADDING_PKCS7 /**< PKCS#7 */
} m3u8EncPadding_e;

/**
 * Double Linked List ������ �� Element ���� ����
 * \See UTIL_DLList
 */
typedef UTIL_DLL_t m3u8SegmentSet_t; /**< Segment Structure Set */
typedef UTIL_DLL_t m3u8KeySet_t; /**< Key Structure Set */
typedef UTIL_DLL_t m3u8ProgramSet_t; /**< Program Structure Set */
typedef UTIL_DLL_t m3u8StreamSet_t; /**< Stream Structure Set */
typedef UTIL_DLL_t m3u8PlaylistSet_t; /**< Playlist Structure Set */
typedef UTIL_DLLNode_t m3u8Node_t; /**< m3u8 �� Structure Ž���� ���� Node Structure redifine */

/**
 * Key �� ���� struct �� �� Key �� ���� ��� ������ �����Ѵ�.
 */
typedef struct m3u8Key_s
{
	HINT8* key_uri; /**< Key�� Uri */
	m3u8Method_e eMethod; /**< Key�� Method(Algorithm) */
	m3u8EncPadding_e eEncPadding; /**< Padding Method(Algorithm) */
	HUINT32 uUsedCount;	/* �ش� key�� ������� segment�� count */
	HUINT8* pIV; /**< Initialize Vector Value */
} m3u8Key_t;

/**
 * Segment �� ���� struct �� �� Segment �� ���� ��� ������ �����Ѵ�.
 */
typedef struct m3u8Segment_s
{
	HUINT64 ullNum; /**< ���׸�Ʈ�� Sequence Number */
	HBOOL	bDiscontinuity; /**< �� Segment ���� Discontinuity ���� */
	HBOOL	bLastSegment; /**< ������ Segment (ENDLIST) */
	HBOOL	bResetSequence;
	HLONG 	datetime; /**< Program DateTime */
	HFLOAT32 duration_of_segment; 	/**< ���׸�Ʈ�� duration */
	HFLOAT32 starttime_of_segment;   /**< ���׸�Ʈ�� Start Time */
	m3u8Key_t *pKey; 				/**< key url. Full URL �� �ƴ� ���, M3U8 �� base_url �� �����Ͽ� ���ȴ�.*/
	HINT8* segment_url;			/**< segment url. Full URL �� �ƴ� ���, M3U8 �� base_url �� �����Ͽ� ���ȴ�.*/
	HINT8* backup_url;			/**< backup segment url. Full URL �� �ƴ� ���, M3U8 �� base_url �� �����Ͽ� ���ȴ�. (���� ��� ����) */
	HBOOL	bChangedTs;		/* �ϳ��� m3u8���Ͽ� ���� ������ TS �� �ִ� ��� */
	HBOOL bByteRangeSupport;		/**< ByteRange Support ���� */
	HUINT64 ullByteRangeLength;	/**< byte ������ ��� ���̸� 10 ������ ǥ�� */
	HUINT64 ullByteRangeOffset;	/**< �ش� ���׸�Ʈ�� offset �� */
	HUINT64 ullByteRangeIndex;		/**< ���� url �� ���� ���׸�Ʈ�� �����ϴ� index */
} m3u8Segment_t;

/**
 * Stream �� ���� struct �� �� Stream �� ���� ��� ������ �����Ѵ�.
 * Stream�� Bandwidth�� ���еȴ�.
 */
typedef struct m3u8Stream_s
{
	HBOOL	bEncrypted; /**< Stream�� Encryption ���� */
	HBOOL bAllowCache; /**< Allow Cache */
	m3u8PlayType_e ePlayType; /**< PlayType */
	HUINT32 unVersion; /**< Version */
	HBOOL	bIFrameOnly; /**< I-Frame Only Playlist */
	HBOOL	bEndList; /**< EDNLIST�� �����ϴ� Stream���� ���� */
//	HUINT32 unStreamDuration; /**< Stream�� Duration */
	HUINT32 unTargetDuration; /**< Stream�� Target Duration */
	HUINT64 ullSequenceNum; /**< Stream�� First Segment��  Sequencial Number */
	HUINT64 ullLastSequenceNum; /**< Stream�� Last updated Sequencial Number(from m3u8 EXT-X-MEDIA-SEQUENCE) */
	HUINT32 unBandwidth; /**< Stream�� Bandwidth */
	m3u8MediaType_e eMediaType; /**< Stream�� Audio/Video Type */
	HLS_CODEC_e eCodec;
	m3u8KeySet_t keySet; /**< Stream�� ���Ǵ� Key Set */
	m3u8SegmentSet_t segmentSet; /**< Stream���� ��ü Segment Set */
} m3u8Stream_t;

/**
 * Program �� ���� struct �� �� Program �� ���� ��� ������ �����Ѵ�.
 */
typedef struct m3u8Program_s
{
	HINT32 nProgramId; /**< Prograrm Id */
	m3u8StreamSet_t streamSet; /**< Program�� ���ϴ� Stream�� Set */
} m3u8Program_t;

/**
 * Multiple M3U8 Parsing �� ���� M3U8 File �ϳ��� ���� structure
 */
typedef struct m3u8Playlist_s
{
	HBOOL bMasterPlaylist;
	HINT32 nProgramId;  /**< Prograrm Id */
	HINT8 *pUrl;  /**< File�� Url */
	HINT8 *pBaseUrl;  /**< File�� Base Url */
	HUINT32 unDuration;  /**< �ϳ��� Stream ��ü Duration ������ ���� ���� */
	HUINT64 ullSequenceNum; /**< SequenceNum */
	HBOOL bUpdated;
	HUINT32 unSegmentNum; /**< Segment Number (0 ~ ) */
	m3u8Stream_t *pStream; /**< m3u8 file�� ���� Stream�� ���� */
	/** Private */
	m3u8Key_t* pCurKey; /**< Stream ���� Segment Parsing �� �ش� Segment�� ����ؾ��� ������ Key */
} m3u8Playlist_t;

/**
 * M3U8 ��ü Struct ����ü�� ��� Segment �� Key �� ���� ������ �����Ѵ�.
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
 *	\brief M3U8 Parser �� Version�� Return �Ѵ�.
 *	\return Parser �� Version
 *	\see
 */
HUINT32 HLS_M3U8_GetParserVersion(void);

/*!
 *	\brief M3U8 Parser Handle ���� �� Parsing
 *	\param pM3U8Buf : M3U8 Data �� �ε��� ����
 *	\param unBufSize : ������ ũ��
 *	\param url : M3U8 File�� Url
 *	\return Parsing �� M3U8 Handle
 *	\see M3U8 Parser Handle ���� �� Parsing�� ���������� HLS_M3U8_Append �� ���
 */
m3u8_t* HLS_M3U8_Create(HINT8* pM3U8Buf, HUINT32 unBufSize, HINT8* url, HINT8* pHostUrl);

/*!
 *	\brief M3U8 Parser Handle ����
 *	\param m3u8 : M3U8 Handle
 *	\return ErrorCode
 *	\see
 */
m3u8Err_e HLS_M3U8_Destroy(m3u8_t *m3u8);

/*!
 *	\brief ������ M3U8 Handle�� M3U8 Data�� Parsing �� �߰�.
 *	\param m3u8 : M3U8 Handle
 *	\param pPlaylist : Parsing�� �ΰ������� �ʿ��� m3u8 ����
 *	\param pM3U8Buf :  �߰��� M3U8 Data
 *	\param unBufSize :  �߰��� M3U8 Data �� ũ��
 *	\return ErrorCode
 *	\see M3U8�� �������� ���Ϸ� ������ ��쿡 �ݺ������� ȣ��
 */
m3u8Err_e HLS_M3U8_Update(m3u8_t* m3u8, m3u8Playlist_t* pPlaylist, HINT8* pM3U8Buf, HUINT32 unBufSize);

/*!
 *	\brief StreamNode ����
 *	\param pStreamNode : Stream Node
 *	\return ErrorCode
 *	\see Stream�� ��� �Ұ����� ��� ���� / ������ �ش� Stream Node�� StreamList���� ����� ���¿����Ѵ�.
 */
m3u8Err_e HLS_M3U8_DiscardStream(m3u8Node_t *pStreamNode);

/*!
 *	\brief �ش� Segment ���� ���� Segment ���� ���� (fo Live Update)
 *	\param pStreamNode : Stream Node
 *	\param ullNum : Target Fragment Number
 *	\return ErrorCode
 *	\see Stream�� ��� �Ұ����� ��� ���� / ������ �ش� Stream Node�� StreamList���� ����� ���¿����Ѵ�.
 */
m3u8Err_e HLS_M3U8_DiscardSegments(m3u8Stream_t *pStream, HUINT64 ullNum);

/*!
 *	\brief keyset list���� keyUri�� ���� key�� ����
 *	\param pStreamNode : Stream Node
 *	\param pKeyUri : ������ key uri
 *	\return ErrorCode
 *	\see Stream�� ��� �Ұ����� ��� ���� / ������ �ش� Stream Node�� StreamList���� ����� ���¿����Ѵ�.
 */
m3u8Err_e HLS_M3U8_DiscardKey(m3u8Stream_t *pStream, HINT8* pKeyUri);

/*!
 *	\brief �ش� Segment ���� ���� Segment ���� Key �����Ͽ� ����  (fo Live Update)
 *	\param pStreamNode : Stream Node
 *	\param ullNum : Target Fragment Number
 *	\return ErrorCode
 *	\see Stream�� ��� �Ұ����� ��� ���� / ������ �ش� Stream Node�� StreamList���� ����� ���¿����Ѵ�.
 */
m3u8Err_e HLS_M3U8_DiscardSegmentsAndKeys(m3u8Stream_t *pStream, HUINT64 ullNum);

m3u8Err_e HLS_M3U8_DiscardSegmentsAndKeysUntilBaseSegment(m3u8Stream_t *pStream, m3u8Segment_t *pBaseSegment);

void HLS_M3U8_Print(m3u8_t *m3u8);

/*!
 *	\brief stream list�� END tag ��/���� live���� �ƴ��� �Ǵ��Ѵ�.
 *	\param pStreamNode : Stream Node
 *	\return BOOL
 */
HBOOL HLS_M3U8_IsLiveStream(m3u8Stream_t *pStream);

/*!
 *	\brief �� segment������ ��� ���� �ð��� ���̸� ���Ѵ�.
 *	\param pStreamNode : Stream Node
 *	\ ullCurSegmentNum : ���� segment�� sequence number
 *	\ ullLastSequenceNum : ���������� segment�� sequence number
 *	\return HFLOAT32
 */
HFLOAT32 HLS_M3U8_GetTimeGapBetweenCurAndLive(m3u8Stream_t* pStream, HUINT64 ullCurSegmentNum, HUINT64 ullLastSequenceNum);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __HLS_M3U8_PARSER_H__ */

/*@}*/
