/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name?» ?û½?´Ï´Ù.
	@brief	  file_name & simple comment.

	Description: File¿¡ ´ëÇÑ ¼³¸í?» ?û½?´Ï´Ù.		\n
	Module: Layer¿Í ¸ðµâ ¸í?» ?û½?´Ï´Ù.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__HX_ID3_H__
#define	__HX_ID3_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	HxID3_FIELD_SIZE_LONG			128
#define	HxID3_FIELD_SIZE_SHORT			64

#define HxID3_IS_SINGLE_BYTE_ENC(enc)    ((enc) == eHXID3_ENC_ISO8859_1 || (enc) == eHXID3_ENC_UTF8)
#define HxID3_IS_DOUBLE_BYTE_ENC(enc)    ((enc) == eHXID3_ENC_UTF16 || (enc) == eHXID3_ENC_UTF16BE)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eHXID3_TAG_ARTIST,
	eHXID3_TAG_ALBUM,
	eHXID3_TAG_TITLE,
	eHXID3_TAG_YEAR,
	eHXID3_TAG_GENRE,
	eHXID3_TAG_PUBLISHER,

	eHXID3_TAG_UNKNOWN
} HXID3_TAG_e;

typedef struct _HxID3_t	HxID3_t;

typedef struct
{
	HBOOL	bIsID3V2;

	HUINT8	ucPictureType;
	HUINT32	ulPictureSize;
	HUINT8	*pucData;
} HxID3_Picture_t;

typedef struct
{
	HINT32		nVersion;
	HINT32		nBitRate;
	HINT32		nSampleRate;
	HUINT32		ulDuration;
	HCHAR		szArtist[HxID3_FIELD_SIZE_LONG];
	HCHAR		szAlbum[HxID3_FIELD_SIZE_LONG];
	HCHAR		szTitle[HxID3_FIELD_SIZE_LONG];
	HCHAR		szYear[HxID3_FIELD_SIZE_SHORT];
	HCHAR		szGenre[HxID3_FIELD_SIZE_SHORT];
	HCHAR		szPublisher[HxID3_FIELD_SIZE_LONG];
} HxID3_MusicInfo_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR	HLIB_ID3_GetID3Picture(const HCHAR *pszFileName, HxID3_Picture_t *pstInfo);
extern HERROR	HLIB_ID3_GetID3MusicInfo(const HCHAR *pszFileName, HxID3_MusicInfo_t *pstInfo);
extern void	HLIB_ID3_MemFree(void *p);

extern HERROR	HLIB_ID3_GetFlacPicture(const HCHAR *pszFileName, HxID3_Picture_t *pstInfo);
extern HERROR	HLIB_ID3_GetFlacMusicInfo(const HCHAR *pszFileName, HxID3_MusicInfo_t *pstInfo);

#endif /* !__PALXML_H__ */
