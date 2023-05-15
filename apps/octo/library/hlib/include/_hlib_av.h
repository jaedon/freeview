/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__HLIB_AV_H__
#define	__HLIB_AV_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eHxAV_STREAMTYPE_VIDEO,
	eHxAV_STREAMTYPE_AUDIO,
	eHxAV_STREAMTYPE_OTHER,

	eHxAV_STREAMTYPE_UNKNOWN
} HxAV_StreamType_e;

typedef struct
{
	HCHAR		szCodecName[32];
	HINT32		nWidth, nHeight;
	HINT32		nBitRate;
} HxAV_VideoInfo_t;

typedef struct
{
	HCHAR		szCodecName[32];
	HINT32		nBitPerSample;
	HINT32		nSampleRate;
	HINT32		nBitRate;
	HINT32		nChannels;
} HxAV_AudioInfo_t;

typedef struct
{
	//3 Common StreamInfo
	HCHAR		szCodecName[32];		// ( nVideoTrack > 0 ) ? stVideoInfo.szCodecName : stAudioInfo.szCodecName
	HINT32		nStreamType;
	HINT32		nDuration;				
	HINT64		llFrameCount;
	HINT32		nWidth, nHeight;
	HINT32		nBitRate;				// total stream bitrate in bit/s

	//3 Video StreamInfo
	HINT32				nVideoTrack;
	HxAV_VideoInfo_t	stVideoInfo;	// Information of 1st Video Track

	//3 Audio StreamInfo
	HINT32				nAudioTrack;
	HxAV_AudioInfo_t	stAudioInfo;	// Information of 1st Audio Track

} HxAV_SteamInfo_t;

typedef struct
{
	//3 Common StreamInfo
	HINT32		nStreamType;
	HINT32		nDuration;				
	HINT32		nBitRate;				// total stream bitrate in bit/s

	//3 Audio StreamInfo
	HINT32		nAudioTrack;
	HxAV_AudioInfo_t	stAudioInfo;	// Information of 1st Audio Track

} HxAV_AudioSteamInfo_t;

typedef struct
{
	HINT32		nWidth;
	HINT32		nHeight;

	HUINT8		data[4];
} HxAV_ImageInfo_t;

typedef struct  _HxAV_tag		HxAV_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern	HxAV_ImageInfo_t*	HLIB_AV_GetVideoThumbnail(const HCHAR *pszFileName,
												HxAV_SteamInfo_t *pstStreamInfo,
												HUINT32 ulTime,
												HBOOL (*recalc_size)(HINT32 *pnDstWidth, HINT32 *pnDstHeight, HINT32 nSrcWidth, HINT32 nSrcHeight),
												HINT32 (*interrupt_callback)(void*),
												void *interrupt_userdata);

extern HERROR				HLIB_AV_GetVideoStreamInfo(const HCHAR *pszFileName, HxAV_SteamInfo_t *pstStreamInfo, HINT32 (*interrupt_callback)(void*), void *interrupt_userdata);
extern HERROR				HLIB_AV_GetAudioStreamInfo(const HCHAR *pszFileName, HxAV_AudioSteamInfo_t *pstAudioStreamInfo, HINT32 (*interrupt_callback)(void*), void *interrupt_userdata); /* HxAV_SteamInfo_t --> HxAV_AudioSteamInfo_t */

extern void					HLIB_AV_FreeImageInfo(HxAV_ImageInfo_t *pstImageInfo);

#endif /* !__HLIB_AV_H__ */

