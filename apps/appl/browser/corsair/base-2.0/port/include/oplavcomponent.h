/**************************************************************************************/
/**
 * @file oplavcomponent.h
 *
 * Video Broadcast
 *
 **************************************************************************************
 **/

#ifndef __OPL_AVCOMPONENT_H__
#define __OPL_AVCOMPONENT_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/

#if 0 // 기존 source 아무도 안 사용 함.. 밑에 broadcast 에서 옮긴걸로 대체
typedef enum {
	OPLAVCOMPONENT_TYPE_VIDEO			= 0,
	OPLAVCOMPONENT_TYPE_AUDIO,
	OPLAVCOMPONENT_TYPE_SUBTITLE,
} eOplAVComponentType;

typedef enum {
	OPLAVCOMPONENT_TS_VIDEO_AVC_HD_25		= 0,
	OPLAVCOMPONENT_TS_VIDEO_AVC_SD_25,
	OPLAVCOMPONENT_TS_AUDIO_HEAAC,
	OPLAVCOMPONENT_TS_AUDIO_AC3,
	OPLAVCOMPONENT_MP4_VIDEO_AVC_HD_25,
	OPLAVCOMPONENT_MP4_VIDEO_AVC_SD_25,
	OPLAVCOMPONENT_MP4_AUDIO_HEAAC,
	OPLAVCOMPONENT_MP4_AUDIO_AC3,
	OPLAVCOMPONENT_AUDIO_MPEG1_L3,
	OPLAVCOMPONENT_AUDIO_HEAAC,
	OPLAVCOMPONENT_AUDIO_WMV
} eOplAVComponentEncoding;

typedef enum {
	OPLAVCOMPONENT_ASPECT_RATIO_UNKNOWN	= 0x00000001,
	OPLAVCOMPONENT_ASPECT_RATIO_4_3		= 0x00000002,
	OPLAVCOMPONENT_ASPECT_RATIO_16_9	= 0x00000004
} eOplAVComponentAspectRatio;

/******************************************************************
	Typedef
******************************************************************/
typedef struct {
	eOplAVOutputType			eType;
	eOplAVComponentEncoding		eEncoding;
	eOplAVComponentAspectRatio	eAspectRatio;
	BPL_BOOL		bEncrypted;
	char			szLanguage[8];		/* ISO 639 */
	unsigned int	ulAudioChannels;
	BPL_BOOL		bHearingImpaired;
} OplAVComponent_t;
#endif

/* sw1g-1t sbkim, broadcast에서 사용하는게 일본향 programme에서도 사용해야 해서 avcomponent로 옮긴다.*/

#define OPLAVCOMPONENT_LANG_SIZE				34
#define OPLAVCOMPONENT_TEXT_SIZE					37
#define OPLAVCOMPONENT_COMP_NAME				256
#define OPLAVCOMPONENT_COMP_ROLE				256
#define OPLAVCOMPONENT_MAX_MULTIVIEW			3 /* 3개 */
#define OPLAVCOMPONENT_MULTIVIEW_SIZE			2 /* 숫자 + '\0' */

typedef enum {
	OPLAVCOMPONENT_TYPE_VIDEO					= 0,
	OPLAVCOMPONENT_TYPE_AUDIO					= 1,
	OPLAVCOMPONENT_TYPE_SUBTITLE				= 2,
	OPLAVCOMPONENT_TYPE_SUPERIMPOSE				= 3,
	OPLAVCOMPONENT_TYPE_DATA					= 4,		// ngs 일본향
	OPLAVCOMPONENT_TYPE_MAX
} eOplAVComponentType;

typedef enum
{
	OPLAVCOMPONENT_SUBTYPE_NONE						= 0,

	// Audio
	OPLAVCOMPONENT_SUBTYPE_AUDIO_SINGLEMONO			= 1,
	OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO			= 2,
	OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO				= 3,
	OPLAVCOMPONENT_SUBTYPE_AUDIO_MULTILINGUAL		= 4,
	OPLAVCOMPONENT_SUBTYPE_AUDIO_SURROUND			= 5,
	OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY		 		= 6,

	// Video

	// Subtitle
	OPLAVCOMPONENT_SUBTYPE_SUBTITLE_DVB				= 201,
	OPLAVCOMPONENT_SUBTYPE_SUBTITLE_EBU				= 202,

	OPLAVCOMPONENT_SUBTYPE_MAX
} eOplAVComponentSubType;

typedef enum
{
	OPLAVCOMPONENT_STREAM_TYPE_NONE						= 0,

	// Audio
	OPLAVCOMPONENT_STREAM_TYPE_AUDIO_CODEC_MPEG2		= 0x04,	/* ISO/IEC 13818-3 Audio */
	OPLAVCOMPONENT_STREAM_TYPE_AUDIO_CODEC_AAC			= 0x0f,
	OPLAVCOMPONENT_STREAM_TYPE_AUDIO_CODEC_AAC_PLUS		= 0x11,

} eOplAVComponentStreamType;


/******************************************************************
	Typedef
******************************************************************/
typedef struct
{
	HUINT32							 ulComponentTag;
	HUINT32							 ulPid;
	eOplAVComponentType				 eType;						// OIPF에 맞춤.
	eOplAVComponentSubType			 eSubType;					// Humax Extension용 SubType
	HBOOL							 bHardOfHearing_Impaired;	// Humax Hard of Hearing for subtitle or hearing impaired 
	HFLOAT32						 fAspectRatio;
	HCHAR							 szLanguage[OPLAVCOMPONENT_LANG_SIZE];				// ISO639
	HBOOL							 bAudioDescription;
	HUINT32							 ulAudioChannels;
	HCHAR							 encoding[OPLAVCOMPONENT_TEXT_SIZE];
	char 							 compName[OPLAVCOMPONENT_COMP_NAME];
	char							 compRole[OPLAVCOMPONENT_COMP_ROLE];

	/* For Japan Extended - JLABS */
	HUINT32							 ulComponentType;
	HUINT32							 ulStreamType;
	HCHAR							 szLanguage2[OPLAVCOMPONENT_LANG_SIZE];
	HBOOL							 bAssociatedContentFlag;			// only data component
	HBOOL							 bDataEntryComponentFlag;
	HCHAR							 szText[OPLAVCOMPONENT_TEXT_SIZE];
	HCHAR							 szMultiView[OPLAVCOMPONENT_MAX_MULTIVIEW][OPLAVCOMPONENT_MULTIVIEW_SIZE];
	HINT32							 nMultiViewNum;
	HINT32							 nDigitalCopyControl;
} OplAVComponent_t;


/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};
#endif

#endif

