/**
	@file     hxid3.c
	@brief   libid3 interface function file

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <dlfcn.h>

#include "_hlib_stdlib.h"
#include "_hlib_mem.h"
#include "_hlib_sem.h"
#include "_hlib_log.h"
#include "_hlib_codepage.h"
#include "_hlib_macro.h"

#include "_hlib_id3.h"

#include <id3.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HXID3_MAGICCODE		0x03B312F0

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	void		*pvID3LibHandle;
	HBOOL		bEnable;
	HUINT32		ulSemId;

	//	Symbol Link for LIBPNG Used function.
	ID3_FrameID	(*ID3Frame_GetID)				(const ID3Frame *frame);
	ID3Field*	(*ID3Frame_GetField)			(const ID3Frame *frame, ID3_FieldID name);
	ID3Frame*	(*ID3Tag_FindFrameWithID)		(const ID3Tag *tag, ID3_FrameID id);
	HBOOL		(*ID3Tag_HasTagType)			(const ID3Tag *tag, ID3_TagType);
	ID3Tag*		(*ID3Tag_New)					(void);
	void		(*ID3Tag_Delete)				(ID3Tag *tag);
	size_t		(*ID3Tag_Link)					(ID3Tag *tag, const char *fileName);
	HUINT32		(*ID3Field_GetINT)				(const ID3Field *field);
	size_t		(*ID3Field_GetASCII)			(const ID3Field *field, char *buffer, size_t maxChars);
	size_t		(*ID3Field_GetUNICODE)			(const ID3Field *field, unicode_t *buffer, size_t maxChars);
	size_t		(*ID3Field_Size)				(const ID3Field *field);
	void		(*ID3Field_GetBINARY)			(const ID3Field *field, uchar *buffer, size_t buffLength);

	//	if needs, add more function.
} id3_init_t;

typedef enum
{
	eHXID3_ENC_NONE     = -1,
	eHXID3_ENC_ISO8859_1,
	eHXID3_ENC_UTF16,
	eHXID3_ENC_UTF16BE,
	eHXID3_ENC_UTF8,
	eHXID3_ENC_MAX
} _HxID3_Enc_e;

typedef struct
{
	HBOOL	bIsID3V2;

	HUINT8	ucEncArtist;
	HCHAR	szArtist[HxID3_FIELD_SIZE_LONG];
	HUINT8	ucEncAlbum;
	HUINT8	szAlbum[HxID3_FIELD_SIZE_LONG];			/**< ID3 album */
	HUINT8	ucEncTitle;
	HUINT8	szTitle[HxID3_FIELD_SIZE_LONG];			/**< ID3 title */
	HUINT8	ucEncYear;
	HUINT8	szYear[HxID3_FIELD_SIZE_SHORT];			/**< ID3 year that released */
	HUINT8	ucEncGenre;
	HUINT8	szGenre[HxID3_FIELD_SIZE_SHORT];		/**< ID3 genre that varies among ID3v1 and 2. NULL if not available */
	HUINT8	ucEncPublisher;
	HUINT8	szPublisher[HxID3_FIELD_SIZE_LONG];		/**< ID3 Publisher */

} _HxID3Info_t;

typedef struct
{
	HINT32 version;			// MPEG version
	HINT32 protection;		//true if the frame is protected by CRC
	HINT32 bitrate;			// bitrate in Kbps
	HINT32 sample_rate;		// sample rate in Hz
	HINT32 padding;			// true if the frame is padded
	HINT32 channel_mode;		// channel mode
	HINT32 header_offset;		// mp3 header offset

	HINT32 frame_size;			// size of the frame in bytes
	HINT32 samples_per_frame;	// number of audio samples in the frame

	// From Xing VBR header for VBR files, From file size for CBR files.
	HINT32 number_of_frames;	// number of frames
	HINT64 data_size;			// data size
	HINT32 vbr_bitrate;		// bitrate in Kbps

	HINT32 duration;

	ID3Tag	*pId3Tag;		// ID3 Tag info
} mp3header_t;

struct _HxID3_t
{
	HUINT32			magiccode;
	mp3header_t		header;
	_HxID3Info_t		id3Info;
};

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static const HINT32 bitrate_table[3][16] = {
	{-1, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, -1},
	{-1, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, -1},
	{-1, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, -1},
};

static const HINT32 samplerate_table[3][4] = {
	{44100, 48000, 32000, -1},		// MPEG v1
	{22050, 24000, 16000, -1},		// MPEG v2
	{11025, 12000, 8000, -1},		// MPEG v2.5
};

/*******************************************************************/
/********************      extern functions     ********************/
/*******************************************************************/
static HERROR	mp3_skipId3v2(id3_init_t *id3, HINT32 nFileHandle);
static HERROR 	mp3_parseHeader(id3_init_t *id3, mp3header_t  *header, HUINT8 *data);
static HINT32	mp3_getXingOffset(id3_init_t *id3, const mp3header_t* header);
static HERROR 	mp3_parseXingHeader(id3_init_t *id3, HINT32 nFileHandle, mp3header_t *header);
static HERROR	mp3_getStreamInfo(id3_init_t *id3, const HCHAR *pszFileName, mp3header_t *header);

static HERROR	mp3_ConvertString(id3_init_t *id3, HINT32 encoding, HUINT8* pencType, HUINT8* pucDst, HUINT8* pucSrc, HINT32 nMaxLen);
static HERROR	mp3_ConvertCodeLayout(id3_init_t *id3, HUINT8 *pucStr, HINT32 nLen);
static HCHAR*	mp3_getId3TextField(id3_init_t *id3, ID3Frame* frame, HINT32 *out_encoding);
static HERROR	mp3_getId3Info(id3_init_t *id3, const HCHAR *pszFileName, ID3Tag *pTag, _HxID3Info_t *outInfo);
static HERROR	mp3_getId3Picture(id3_init_t *id3, const HCHAR *pszFileName, ID3Tag *pTag, HxID3_Picture_t *outPic);

static id3_init_t*	id3_getInstance(void)
{
	static id3_init_t	*s_pstID3Init = NULL;

	if (s_pstID3Init)
	{
		HxSEMT_Get(s_pstID3Init->ulSemId);

		if (s_pstID3Init->bEnable)
			return s_pstID3Init;

		HxSEMT_Release(s_pstID3Init->ulSemId);

		return NULL;
	} else
	{
		id3_init_t	*id3 = HLIB_MEM_Calloc(sizeof(id3_init_t));

		s_pstID3Init = id3;

		HxSEMT_Create(&id3->ulSemId, "id3sem", 0);
		HxSEMT_Get(id3->ulSemId);

		id3->pvID3LibHandle = dlopen("/usr/lib/libid3.so", RTLD_LAZY);
		if (id3->pvID3LibHandle == NULL)
		{
			HxLOG_Critical("[%s](%d) can't find libid3.so in /usr/lib directory!!\n");
			HxLOG_Assert(0);

			return NULL;
		}

		id3->ID3Frame_GetID			= dlsym(id3->pvID3LibHandle, "ID3Frame_GetID");
		id3->ID3Frame_GetField		= dlsym(id3->pvID3LibHandle, "ID3Frame_GetField");
		id3->ID3Tag_FindFrameWithID	= dlsym(id3->pvID3LibHandle, "ID3Tag_FindFrameWithID");
		id3->ID3Tag_HasTagType		= dlsym(id3->pvID3LibHandle, "ID3Tag_HasTagType");
		id3->ID3Tag_New				= dlsym(id3->pvID3LibHandle, "ID3Tag_New");
		id3->ID3Tag_Delete			= dlsym(id3->pvID3LibHandle, "ID3Tag_Delete");
		id3->ID3Tag_Link			= dlsym(id3->pvID3LibHandle, "ID3Tag_Link");
		id3->ID3Field_GetINT		= dlsym(id3->pvID3LibHandle, "ID3Field_GetINT");
		id3->ID3Field_GetASCII		= dlsym(id3->pvID3LibHandle, "ID3Field_GetASCII");
		id3->ID3Field_GetUNICODE	= dlsym(id3->pvID3LibHandle, "ID3Field_GetUNICODE");
		id3->ID3Field_Size			= dlsym(id3->pvID3LibHandle, "ID3Field_Size");
		id3->ID3Field_GetBINARY		= dlsym(id3->pvID3LibHandle, "ID3Field_GetBINARY");

		HxLOG_Assert(id3->ID3Frame_GetID && id3->ID3Frame_GetField && id3->ID3Tag_FindFrameWithID);
		HxLOG_Assert(id3->ID3Tag_HasTagType && id3->ID3Tag_New && id3->ID3Tag_Delete && id3->ID3Tag_Link);
		HxLOG_Assert(id3->ID3Field_GetINT && id3->ID3Field_GetASCII && id3->ID3Field_GetUNICODE);
		HxLOG_Assert(id3->ID3Field_Size && id3->ID3Field_GetBINARY);

		id3->bEnable = TRUE;

		s_pstID3Init = id3;
	}

	return s_pstID3Init;
}

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HxID3_t*	hlib_id3_open(id3_init_t *id3, const HCHAR *pszFileName)
{
	HERROR		ret = 0;
	HxID3_t		*pstInfo = NULL;

	if (pszFileName == NULL)
		return NULL;

	pstInfo = (HxID3_t*)HLIB_MEM_Calloc(sizeof(HxID3_t));
	HxLOG_Assert(pstInfo);
	pstInfo->magiccode = HXID3_MAGICCODE;
	ret = mp3_getStreamInfo(id3, pszFileName, &pstInfo->header);
	if (ret == ERR_OK)
	{
		mp3_getId3Info(id3, pszFileName, pstInfo->header.pId3Tag, &pstInfo->id3Info);

		return pstInfo;
	}

	if (pstInfo)
		HLIB_MEM_Free(pstInfo);

	return NULL;
}

HERROR	hlib_id3_close(id3_init_t *id3, HxID3_t *pstInfo)
{
	if (pstInfo->magiccode == HXID3_MAGICCODE)
	{
		HLIB_MEM_Free(pstInfo);

		return ERR_OK;
	}

	HxLOG_Error("[%s] HxID3 doesn't have valid magiccode!!\n", __FILE_LINE__);

	return ERR_FAIL;
}

const HCHAR*	hlib_id3_getid3tagstring(id3_init_t *id3, HxID3_t *pstInfo, HXID3_TAG_e eTag)
{
	HCHAR	*pszData;

	if (pstInfo == NULL)
		return NULL;

	switch (eTag)
	{
		case eHXID3_TAG_ARTIST:		pszData = pstInfo->id3Info.szArtist;				break;
		case eHXID3_TAG_ALBUM:		pszData = (HCHAR*)pstInfo->id3Info.szAlbum;			break;
		case eHXID3_TAG_TITLE:		pszData = (HCHAR*)pstInfo->id3Info.szTitle;			break;
		case eHXID3_TAG_YEAR :		pszData = (HCHAR*)pstInfo->id3Info.szYear;			break;
		case eHXID3_TAG_GENRE:		pszData = (HCHAR*)pstInfo->id3Info.szGenre;			break;
		case eHXID3_TAG_PUBLISHER: 	pszData = (HCHAR*)pstInfo->id3Info.szPublisher;		break;
		default:
			pszData = NULL;
			break;
	}

	if (HxSTD_StrEmpty(pszData))
		return NULL;

	return (const HCHAR *)pszData;
}

#define	HXID3_CHECKANDSET(id3, a, b, c, d)		do {						\
											const HCHAR *pszTemp = hlib_id3_getid3tagstring(id3, a, b);		\
											if (pszTemp)										\
												HLIB_STD_StrNCpySafe(d, pszTemp, c);			\
										} while(0);

#if	0
HERROR	HxID3_GetMP3Info(const HCHAR *pszFileName, HxMP3Info_t *pstInfo)
{
	HERROR			ret = 0;
	mp3header_t		header = { 0, };

	if (pszFileName == NULL || pstInfo == NULL)
		return ERR_FAIL;

	HxSTD_MemSet(&header, 0x00, sizeof(mp3header_t));

	/* get stream info */
	ret = mp3_getStreamInfo(pszFileName, &header);
	if (ret == ERR_OK)
	{
		// translate stream info
		pstInfo->nAudioVersion = header.version;
		pstInfo->nProtection   = header.protection;
		pstInfo->nBitrate      = header.bitrate;
		pstInfo->nSampleRate   = header.sample_rate;
		pstInfo->nDuration     = header.duration;			// in seconds
	}
	else
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	HxID3_GetID3Info(const HCHAR *pszFileName, _HxID3Info_t *pstInfo)
{
	HERROR		ret;
	mp3header_t	header;

	if (HxSTD_StrEmpty(pszFileName) || pstInfo == NULL)
	{
		HxLOG_Debug("no file to get id3 (%s)\n", pszFileName);

		return ERR_FAIL;
	}

	HxSTD_MemSet(&header, 0x00, sizeof(mp3header_t));

	/* parse its header */
	ret = mp3_getStreamInfo(pszFileName, &header);
	if (ret == ERR_OK)
	{
		/* get ID3 tag info */
		return mp3_getId3Info(pszFileName, header.pId3Tag, pstInfo);
	}

	HxLOG_Debug("no id3 tag in [%s]\n", pszFileName);
	return ERR_FAIL;
}
#endif

/*******************************************************************************************
 * Static
 ******************************************************************************************/
static HERROR	mp3_skipId3v2(id3_init_t *id3, HINT32 nFileHandle)
{
	HUINT8 			buffer[10] = { 0, };
	HUINT32 		id3v2_size = 0;
	HINT64			llOffset = 0;

	HINT32			nReadByte = 0;

	llOffset = lseek(nFileHandle, 0, SEEK_CUR);
	if (llOffset == -1)
		return ERR_FAIL;

	/*
	Loop until we find something other than ID3v2 in case the file
	contains multiple ID3v2 tags.
	*/
	for (;;)
	{
		/* Read ten bytes to examine ID3v2 */
		nReadByte = read(nFileHandle, buffer, 10);
		if (nReadByte != 10)
			break;

		/* Make sure the read stream is ID3v2 header. */
		if (buffer[0] != 'I' || buffer[1] != 'D' || buffer[2] != '3') {
			break;
		}

		/* Obtain tag size. */
		id3v2_size =
			(HUINT32)(buffer[6] & 0x7F) << 21 |
			(HUINT32)(buffer[7] & 0x7F) << 14 |
			(HUINT32)(buffer[8] & 0x7F) << 7  |
			(HUINT32)(buffer[9] & 0x7F);

		/* Seek to the end position. */
		llOffset = lseek(nFileHandle, id3v2_size, SEEK_CUR);
		if (llOffset == -1)
			break;
	}

	if (llOffset >= 0)
		lseek(nFileHandle, llOffset, SEEK_SET);

	return ERR_OK;
}

static HERROR 		mp3_parseHeader(id3_init_t *id3, mp3header_t  *header, HUINT8 *data)
{
	HINT32 bitrate_index = 0, samplerate_index = 0;
	HINT32 bitrate = 0;

	/* Make sure sync header and LayerIII */
	if (data[0] != 0xFF || (data[1] & 0xE0) != 0xE0)
	{
		return -1;
	}

	/* Read MPEG version */
	switch ((data[1] >> 3) & 0x03) {
	case 3:
		header->version = 0;	/* MPEG1 ---> MP3_VERSION_MPEG1 */
		break;
	case 2:
		header->version = 1;	/* MPEG2 ---> MP3_VERSION_MPEG2 */
		break;
	case 0:
		header->version = 2;	/* MPEG2.5 ---> MP3_VERSION_MPEG2_5 */
		break;
	case 1:						/* reserved in specification */
	default:
		return -1;
	}

	// Read bitrate
	bitrate_index = data[2] >> 4;
	header->bitrate = bitrate_table[header->version][bitrate_index] * 1024;
	bitrate = bitrate_table[header->version][bitrate_index];
	// Read sample rate
	samplerate_index = (data[2] >> 2) & 0x03;
	header->sample_rate = samplerate_table[header->version][samplerate_index];
	// Validate bitrate and sample rate
	if (header->bitrate < 0 || header->sample_rate < 0) {
		return ERR_FAIL;
	}
	// Read padding
	header->padding = (data[2] >> 1) & 0x01;
	// Read stereo mode
	header->channel_mode = (data[3] >> 6) & 0x03;

	if (header->version == 0) {
		header->frame_size = 144000 * bitrate / header->sample_rate + header->padding;
		header->samples_per_frame = 1152;

	} else {
		header->frame_size = 72000 * bitrate / header->sample_rate + header->padding;
		header->samples_per_frame = 576;
	}

	header->number_of_frames = 0;	/* Set zero for now. */

	return ERR_OK;
}

static HINT32	mp3_getXingOffset(id3_init_t *id3, const mp3header_t* header)
{
	if (header->version == 0) {
		// MPEG1
		return (header->channel_mode == 3 ? 17 : 32);
	} else {
		// MPEG2/2.5
		return (header->channel_mode == 3 ? 9 : 17);
	}
}

static HERROR 		mp3_parseXingHeader(id3_init_t *id3, HINT32 nFileHandle, mp3header_t *header)
{
	HUINT8 				xing_header[16] = { 0, };	/* 16 bytes is enough. */
	HINT32				nReadByte;

	nReadByte = read(nFileHandle, xing_header, 16);
	if (nReadByte != 16)
		return ERR_FAIL;

	if (memcmp(xing_header, "Xing", 4) != 0) {
		return ERR_FAIL;
	}
	if (xing_header[7] & 0x01) {
		header->number_of_frames =
			(HUINT32)xing_header[ 8] << 24 |
			(HUINT32)xing_header[ 9] << 16 |
			(HUINT32)xing_header[10] << 8  |
			(HUINT32)xing_header[11]
			;
	}
	if (xing_header[7] & 0x02) {
		header->data_size =
			(HUINT32)xing_header[12] << 24 |
			(HUINT32)xing_header[13] << 16 |
			(HUINT32)xing_header[14] << 8  |
			(HUINT32)xing_header[15]
			;
	}

	return ERR_OK;
}

static HERROR	mp3_getStreamInfo(id3_init_t *id3, const HCHAR *pszFileName, mp3header_t *header)
{
#if defined(WIN32)
	//TODO
#else
	HUINT8 				buffer[4] = { 0, };
	HUINT64 			ullFileSize = 0;
	HINT32				nFileHandle;
	HINT32				nReadByte = 0;
	HINT64				llOffset = 0;
	struct stat			fstat;

	if (lstat(pszFileName, &fstat) == -1)
	{
		HxLOG_Error("fail to get stat [%s]\n", pszFileName);
		return ERR_FAIL;
	}

	ullFileSize = fstat.st_size;
	if (nFileHandle = open(pszFileName, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH), nFileHandle < 0)
	{
		HxLOG_Error("fail to open file [%s]\n", pszFileName);
		return ERR_FAIL;
	}

	/* Skip ID3v2 for unsyncronized ID3v2 tag. */
	mp3_skipId3v2(id3, nFileHandle);

	nReadByte = read(nFileHandle, buffer, 4);
	if (nReadByte != 4)
		goto error_get_mp3stream_info;

	for (;;)
	{
		if (mp3_parseHeader(id3, header, buffer) == ERR_OK)
		{
			/* Found a MPEG Audio header... */

			/* Get the offset position where the first frame begins. */
			HUINT64 content_pos = 0;

			content_pos = lseek(nFileHandle, 0, SEEK_CUR);
			if( content_pos > 4 )
				content_pos -= 4;
			else
				content_pos = 0;

			header->header_offset = content_pos;
			HxLOG_Debug("Header found at: %d \n", (HINT32)(content_pos-2));

			/* Initialize data_size and number_of_frames, assuming a CBR stream. */
			header->data_size = (ullFileSize - content_pos);
			header->number_of_frames = header->data_size / header->frame_size;

			/* Discard side info. */
			llOffset = lseek(nFileHandle, mp3_getXingOffset(id3, header), SEEK_CUR);
			if (llOffset == -1)
				goto error_get_mp3stream_info;

			/* Parse Xing VBR header if any. This sets number_of_frames and data_size. */
			if (mp3_parseXingHeader(id3, nFileHandle, header) == ERR_OK) {
				HFLOAT64 duration = (HFLOAT64)header->samples_per_frame * header->number_of_frames / header->sample_rate;
				HFLOAT64 bitrate = header->data_size * 8.0 / duration;
				header->duration = (HINT32)duration;
				header->vbr_bitrate = (HINT32)bitrate;
			} else {
				HFLOAT64 duration = (HFLOAT64)header->samples_per_frame * header->number_of_frames / header->sample_rate;
				header->duration = (HINT32)duration;
			}

			HxLOG_Debug(" version(enumerated) : 0x%x\n", header->version);
			HxLOG_Debug(" protection : %d\n", header->protection);
			HxLOG_Debug(" bitrate : %d\n", header->bitrate);
			HxLOG_Debug(" vbr-bitrate : %d\n", header->vbr_bitrate);
			HxLOG_Debug(" sample_rate : %d\n", header->sample_rate);
			HxLOG_Debug(" channel_mode : %d\n", header->channel_mode);
			HxLOG_Debug(" frame_size : %d\n", header->frame_size);
			HxLOG_Debug(" samples_per_frame : %d\n", header->samples_per_frame);
			HxLOG_Debug(" number_of_frames : %d\n", header->number_of_frames);
			HxLOG_Debug(" data_size : %llu\n", header->data_size);
			HxLOG_Debug(" duration : %d\n", header->duration);

			/* Exit with success. */
			break;
		}

		/* Read next. */
		memmove(&buffer[0], &buffer[1], sizeof(HUINT8)*3);
		nReadByte = read(nFileHandle, &buffer[3], 1);
		if (nReadByte != 1)
			goto error_get_mp3stream_info;
	}

	fsync(nFileHandle);
	close(nFileHandle);

	return 0;

error_get_mp3stream_info:
	fsync(nFileHandle);
	close(nFileHandle);
#endif
	return -1;
}

static void		mp3_changeEndianUCS2(id3_init_t *id3, HUINT16 *pusString)
{
	HUINT16		*pusChar;

	pusChar = pusString;
	while (*pusChar)
	{
		*pusChar = HxMACRO_SWAP16(*pusChar);
		pusChar++;
	}
}

static HERROR	mp3_ConvertString(id3_init_t *id3, HINT32 encoding, HUINT8* pencType, HUINT8* pucDst, HUINT8* pucSrc, HINT32 nMaxLen)
{
	HUINT16	*pusUCS2Str = NULL;
	HCHAR	*pszSingleStr = NULL;

	if (pucDst == NULL || pucSrc == NULL || pencType == NULL)
	{
		HxLOG_Error("Error ----\n");
		return ERR_FAIL;
	}

	pusUCS2Str = (HUINT16*)HLIB_MEM_Calloc(8192);
	pszSingleStr = (HCHAR*)HLIB_MEM_Calloc(4096);
	if ((pusUCS2Str == NULL) || (pszSingleStr == NULL))
		goto err_rtn;

	HxLOG_Debug("encoding : [%d] pucSrc : [%s]\n", encoding, pucSrc);
	if (HxID3_IS_DOUBLE_BYTE_ENC(encoding))
	{
		HxSTD_MemSet(pszSingleStr, 0, 4096);

		*pencType = encoding;
		HxLOG_Debug("[%s] ID3 MULTI BYTE ENCODING Mode\n", __FILE_LINE__);
		if (encoding == eHXID3_ENC_UTF16)
		{
			HLIB_LOG_Dump(pucSrc, 128, 0, 0);
			mp3_changeEndianUCS2(id3, (HUINT16*)pucSrc);
			HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8, (const HUINT16*)pucSrc, (void*)pszSingleStr, 4096);
			HLIB_STD_StrUtf8NCpy((HCHAR*)pucDst, pszSingleStr, nMaxLen - 1);
			pucDst[nMaxLen - 1] = 0;
		} else
		if (encoding == eHXID3_ENC_UTF16BE)
		{
			HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8, (const HUINT16*)pucSrc, (void*)pszSingleStr, 4096);
			HLIB_STD_StrUtf8NCpy((HCHAR*)pucDst, pszSingleStr, nMaxLen - 1);
			pucDst[nMaxLen - 1] = 0;
		}
		else
		{
			HxLOG_Error("[%s] not supported string type --- \n", __FILE_LINE__);
			*pucDst = 0;
		}
	}
	else
	{
		HINT32			ulCpTagLen = 0;
		eHxCP_CodePage_e		eCodePage = eHxCP_ISO_6937;
		HINT32			nCpRet;
		HERROR			hErr;

		hErr = HLIB_CP_CPTagByCodepage((const HUINT8*)pucSrc, &eCodePage, &ulCpTagLen);
		if (hErr != ERR_OK)
		{
			eCodePage = eHxCP_UNICODE_UTF8;
			ulCpTagLen = 0;
		}

		nCpRet = HLIB_CP_ConvertCodepageToUCS2(eCodePage, pucSrc + ulCpTagLen, pusUCS2Str, 4096);
		nCpRet = HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8, (HUINT16*)pusUCS2Str, pszSingleStr, 4096);

		HLIB_STD_StrUtf8NCpy((HCHAR*)pucDst, pszSingleStr, nMaxLen - 1);
		pucDst[nMaxLen - 1] = 0;
	}

err_rtn:
	if (pusUCS2Str)
		HLIB_MEM_Free(pusUCS2Str);
	if (pszSingleStr)
		HLIB_MEM_Free(pszSingleStr);

	return ERR_OK;
}

static HERROR	mp3_ConvertCodeLayout(id3_init_t *id3, HUINT8 *pucStr, HINT32 nLen)
{
#if defined(USE_MP3TAG_RUSSIAN)
	// convert CP1251(russia windows) to iso8859-5
	HINT32 idx = 0;
	for(idx = 0; idx < nLen; idx++)
	{
		if (pucStr[idx] >= 0xB0 && pucStr[idx] <= 0xFF)
			pucStr[idx] -= 0x10;
	}
#endif
	return ERR_OK;
}

void			mp3_swapBytes(id3_init_t *id3, HUINT8 *pData, HINT32 size)
{
	HUINT8		c = 0;
	HINT32					i = 0;

	for (i = 0; i < size; i += 2)
	{
		c = (HUINT8)pData[i];
		pData[i] = pData[i + 1];
		pData[i + 1] = c;
	}
}

HINT32				mp3_getGenreIndex(id3_init_t *id3, HINT8 *str)
{
	HINT32			value = -1;		// -1 genre means it's string data itself

	// If the genre string begins with "(ddd)", where "ddd" is a number, then
	// "ddd" is the genre number---get it
	if (str[0] == '(')				/* genre code is number */
	{
		HINT8 *pCur = &str[1];

		while (isdigit(*pCur))
		{
			pCur++;
		}
		if (*pCur == ')')
		{
			// if the genre number is greater than ID3_NR_OF_V1_GENRES, its invalid.
			value = atoi(&str[1]);
		}
	}

	return value;
}

static HCHAR*	mp3_getId3TextField(id3_init_t *id3, ID3Frame* frame, HINT32 *out_encoding)
{
	static HUINT8 	*readbuf = NULL;	// static buffer, could be reallodcated
	static size_t 	readbuf_size = HxID3_FIELD_SIZE_LONG;
	ID3Field 		*field = NULL;
	HINT32 			encoding = 0;

	if (id3->ID3Frame_GetID(frame) == ID3FID_NOFRAME)
	{
		*out_encoding = ID3TE_ISO8859_1;
		return "[ unknown frame ]";
	}

	field = id3->ID3Frame_GetField(frame, ID3FN_TEXTENC);
	if (field != NULL)
		encoding = id3->ID3Field_GetINT(field);
	else
		encoding = ID3TE_ISO8859_1;

	field = id3->ID3Frame_GetField(frame, ID3FN_TEXT);
	if (field == NULL)
		field = id3->ID3Frame_GetField(frame, ID3FN_URL);
	if (field == NULL)
		return NULL;

	if (readbuf == NULL)
		readbuf = (HUINT8 *)HLIB_MEM_Malloc(readbuf_size);

	HxSTD_MemSet(readbuf, 0, readbuf_size);
	while(1)
	{
		size_t 	read = 0;

		if (ID3TE_IS_SINGLE_BYTE_ENC(encoding))
		{
			read = id3->ID3Field_GetASCII(field, (HINT8*)readbuf, readbuf_size);
			mp3_ConvertCodeLayout(id3, readbuf, read);
		}
		else
		{
		    read = id3->ID3Field_GetUNICODE(field, (unicode_t*)readbuf, readbuf_size / 2);
#if defined(USE_MP3TAG_RUSSIAN)
			readbuf[(read * 2) + 1] = 0x0000;
#else
			readbuf[read + 1] = 0x0000;
#endif
		}

		if (read < readbuf_size)
		{
		    	break;
		}

		/* grow buffer when needed */
		HLIB_MEM_Free(readbuf);
		readbuf_size *= 2;
		readbuf = (HUINT8 *)HLIB_MEM_Malloc(readbuf_size);
		HxSTD_MemSet(readbuf, 0, readbuf_size);
	}

	*out_encoding = encoding;

	return (HCHAR*)readbuf;
}

static HERROR	mp3_getId3Info(id3_init_t *id3, const HCHAR *pszFileName, ID3Tag *pTag, _HxID3Info_t *outInfo)
{
	ID3Frame	*pFrame = NULL;
	HUINT8		*str = NULL;
	HINT32 		size = 0, encoding = 0;
	HINT32 		value = 0;

	if (outInfo == NULL)
	{
		HxLOG_Error("NULL info!\n");

		return ERR_FAIL;
	}

	pTag = id3->ID3Tag_New();

	size = id3->ID3Tag_Link(pTag, pszFileName);
	if (size <= 0)
	{
		HxLOG_Error("fail to link ID3 tag.\n");
		if (pTag)
			id3->ID3Tag_Delete(pTag);

		return ERR_FAIL;
	}

	outInfo->bIsID3V2 = id3->ID3Tag_HasTagType(pTag, ID3TT_ID3V2);

	/* artist */
	pFrame = id3->ID3Tag_FindFrameWithID(pTag, ID3FID_LEADARTIST);
	if (pFrame != NULL)
	{
		str = (HUINT8*)mp3_getId3TextField(id3, pFrame, &encoding);
		if (str != NULL)
		{
			mp3_ConvertString(id3, encoding, &outInfo->ucEncArtist, (HUINT8*)outInfo->szArtist, str, HxID3_FIELD_SIZE_LONG);
		}
	}

	/* album */
	pFrame = id3->ID3Tag_FindFrameWithID(pTag, ID3FID_ALBUM);
	if (pFrame != NULL)
	{
		str = (HUINT8*)mp3_getId3TextField(id3, pFrame, &encoding);
		if (str != NULL)
		{
			mp3_ConvertString(id3, encoding, &outInfo->ucEncAlbum, outInfo->szAlbum, str, HxID3_FIELD_SIZE_LONG);
		}
	}

	/* year */
	pFrame = id3->ID3Tag_FindFrameWithID(pTag, ID3FID_YEAR);
	if (pFrame != NULL)
	{
		str = (HUINT8*)mp3_getId3TextField(id3, pFrame, &encoding);
		if (str != NULL)
		{
			mp3_ConvertString(id3, encoding, &outInfo->ucEncYear, outInfo->szYear, str, HxID3_FIELD_SIZE_SHORT);
		}
	}

	/* title */
	pFrame = id3->ID3Tag_FindFrameWithID(pTag, ID3FID_TITLE);
	if (pFrame != NULL)
	{
		str = (HUINT8*)mp3_getId3TextField(id3, pFrame, &encoding);
		if (str != NULL)
		{
			mp3_ConvertString(id3, encoding, &outInfo->ucEncTitle, outInfo->szTitle, str, HxID3_FIELD_SIZE_LONG);
		}
	}

	/* publisher */
	pFrame = id3->ID3Tag_FindFrameWithID(pTag, ID3FID_PUBLISHER);
	if (pFrame != NULL)
	{
		str = (HUINT8*)mp3_getId3TextField(id3, pFrame, &encoding);
		if (str != NULL)
		{
			mp3_ConvertString(id3, encoding, &outInfo->ucEncPublisher, outInfo->szPublisher, str, HxID3_FIELD_SIZE_LONG);
		}
	}


	/* genre */
	pFrame = id3->ID3Tag_FindFrameWithID(pTag, ID3FID_CONTENTTYPE);
	if (pFrame != NULL)
	{
		if (outInfo->bIsID3V2)
		{	// for ID3v2
			str = (HUINT8*)mp3_getId3TextField(id3, pFrame, &encoding);
			if (str != NULL)
			{
				if (ID3TE_IS_SINGLE_BYTE_ENC(encoding))
				{
					value = mp3_getGenreIndex(id3, (HINT8*)str);

					if (value >= 0 && value < ID3_NR_OF_V1_GENRES)
					{
						char *genreStr = (HINT8 *)ID3_v1_genre_description[value];
						snprintf((char*)outInfo->szGenre, sizeof(outInfo->szGenre), "%s", genreStr);
					}
					else							/* genre is text */
					{
						snprintf((char*)outInfo->szGenre, sizeof(outInfo->szGenre), "%s", str);
					}
				}
				else
				{
					HUINT16 	data[HxID3_FIELD_SIZE_SHORT] = { 0, };
					HUINT8		convStr[HxID3_FIELD_SIZE_SHORT] = { 0, };
					HINT32		usedBytes = 0, utf8len = 0;

					memset(data, 0, HxID3_FIELD_SIZE_SHORT);
					memset(convStr, 0, HxID3_FIELD_SIZE_SHORT);
					if (encoding == eHXID3_ENC_UTF16)
					{
						usedBytes = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UCS2,(const void *)str);
						if( usedBytes < 0 ) usedBytes = 0;
						HxSTD_MemCopy(data, str, usedBytes);
						mp3_swapBytes(id3, (HUINT8 *)data, HxID3_FIELD_SIZE_SHORT);
					}
					else
						HxSTD_MemCopy(data, str, HxID3_FIELD_SIZE_SHORT);

					/* makeup UTF8 genre string with UCS2 data */
					outInfo->ucEncGenre = 0;
					HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8, (const HUINT16 *)data, (void *)convStr, HxID3_FIELD_SIZE_SHORT);
					utf8len = HLIB_CP_NumOfBytes(eHxCP_UNICODE_UTF8,(const void *)convStr);
					if (utf8len > 0)
					{
						value = mp3_getGenreIndex(id3, (HINT8 *)convStr);
						if (value >= 0 && value < ID3_NR_OF_V1_GENRES)
						{
							char *genreStr = (HINT8 *)ID3_v1_genre_description[value];
							snprintf((char*)outInfo->szGenre, sizeof(outInfo->szGenre), "%s", genreStr);
						}
						else							/* genre is text */
						{
							snprintf((char*)outInfo->szGenre, sizeof(outInfo->szGenre), "%s", convStr);
						}
					}
				}
			}
		}
		else
		{	// for ID3v1
			ID3Field *pField = id3->ID3Frame_GetField(pFrame, ID3FN_CONTENTTYPE);
			if (pField != NULL)
			{
				value = id3->ID3Field_GetINT(pField);
				if (value < ID3_NR_OF_V1_GENRES)
				{
					char *genreStr = (HINT8 *)ID3_v1_genre_description[value];
					snprintf((char*)outInfo->szGenre, sizeof(outInfo->szGenre), "%s", genreStr);
				}
			}
		}
	}

	if (pTag)
		id3->ID3Tag_Delete(pTag);

	return ERR_OK;
}

static HERROR	mp3_getId3Picture(id3_init_t *id3, const HCHAR *pszFileName, ID3Tag *pTag, HxID3_Picture_t *outPic)
{
	ID3Frame	*pFrame = NULL;
	ID3Field	*pField = NULL;
	HINT32 		size = 0;

	if (outPic == NULL)
	{
		HxLOG_Error("outPic is NULL\n");

		return ERR_FAIL;
	}

	pTag = id3->ID3Tag_New();

	size = id3->ID3Tag_Link(pTag, pszFileName);
	if (size <= 0)
	{
		HxLOG_Error("fail to link ID3 tag\n");
		if (pTag)
			id3->ID3Tag_Delete(pTag);

		return ERR_FAIL;
	}

	outPic->bIsID3V2 = id3->ID3Tag_HasTagType(pTag, ID3TT_ID3V2);

	/* picture */
	pFrame = id3->ID3Tag_FindFrameWithID(pTag, ID3FID_PICTURE);
	HxLOG_Debug("bIsID3V2 : [%d] pFrame : [%p]\n", outPic->bIsID3V2, pFrame);
	if (pFrame != NULL)
	{
		pField = id3->ID3Frame_GetField(pFrame, ID3FN_DATA);
		if (pField != NULL)
		{
			outPic->ulPictureSize = id3->ID3Field_Size(pField);
			outPic->pucData = (HUINT8 *)HLIB_MEM_Malloc(outPic->ulPictureSize);
			id3->ID3Field_GetBINARY(pField, outPic->pucData, outPic->ulPictureSize);

			if (pTag)
				id3->ID3Tag_Delete(pTag);

			return ERR_OK;
		}
		else
			goto exit_get_id3_picture;
	}
	else
		goto exit_get_id3_picture;


exit_get_id3_picture:
	if (pTag)
		id3->ID3Tag_Delete(pTag);

	return ERR_FAIL;
}

/*******************************************************************************************
 * Interfaces
 ******************************************************************************************/

HERROR	HLIB_ID3_GetID3Picture(const HCHAR *pszFileName, HxID3_Picture_t *pstInfo)
{
	HERROR			ret;
	mp3header_t		header;
	id3_init_t		*id3 = id3_getInstance();

	if (id3 == NULL)
		return ERR_FAIL;

	if (HxSTD_StrEmpty(pszFileName) || pstInfo == NULL)
	{
		HxLOG_Error("input is not valid : [%s]\n", pszFileName);
		HxSEMT_Release(id3->ulSemId);

		return ERR_FAIL;
	}

	HxSTD_MemSet(&header, 0x00, sizeof(mp3header_t));

	/* parse its header */
	ret = mp3_getStreamInfo(id3, pszFileName, &header);
	if (ret == 0)
	{
		/* get ID3 tag info */
		ret = mp3_getId3Picture(id3, pszFileName, header.pId3Tag, pstInfo);

		HxSEMT_Release(id3->ulSemId);
		return ret;
	}

	HxLOG_Debug("no tag [%s]\n", pszFileName);

	HxSEMT_Release(id3->ulSemId);

	return ERR_FAIL;
}

HERROR	HLIB_ID3_GetID3MusicInfo(const HCHAR *pszFileName, HxID3_MusicInfo_t *pstInfo)
{
	HxID3_t		*pstId3Info;
	id3_init_t	*id3 = id3_getInstance();

	if (id3 == NULL)
		return ERR_FAIL;

	pstId3Info = hlib_id3_open(id3, pszFileName);
	if (pstId3Info)
	{
		HxSTD_MemSet(pstInfo, 0, sizeof(HxID3_MusicInfo_t));
		HXID3_CHECKANDSET(id3, pstId3Info, eHXID3_TAG_TITLE,  	HxID3_FIELD_SIZE_LONG,  pstInfo->szTitle);
		HXID3_CHECKANDSET(id3, pstId3Info, eHXID3_TAG_GENRE,  	HxID3_FIELD_SIZE_SHORT, pstInfo->szGenre);
		HXID3_CHECKANDSET(id3, pstId3Info, eHXID3_TAG_ARTIST, 	HxID3_FIELD_SIZE_LONG,  pstInfo->szArtist);
		HXID3_CHECKANDSET(id3, pstId3Info, eHXID3_TAG_ALBUM,  	HxID3_FIELD_SIZE_LONG,  pstInfo->szAlbum);
		HXID3_CHECKANDSET(id3, pstId3Info, eHXID3_TAG_YEAR,   	HxID3_FIELD_SIZE_SHORT, pstInfo->szYear);
		HXID3_CHECKANDSET(id3, pstId3Info, eHXID3_TAG_PUBLISHER,HxID3_FIELD_SIZE_LONG, 	pstInfo->szPublisher);

		pstInfo->ulDuration = pstId3Info->header.duration;
		pstInfo->nVersion	= pstId3Info->header.version;
		pstInfo->nBitRate	= pstId3Info->header.bitrate;
		pstInfo->nSampleRate = pstId3Info->header.sample_rate;

		hlib_id3_close(id3, pstId3Info);

		HxSEMT_Release(id3->ulSemId);
		return ERR_OK;
	}

	HxSEMT_Release(id3->ulSemId);
	return ERR_FAIL;
}

void		HLIB_ID3_MemFree(void *p)
{
	HLIB_MEM_Free(p);
}

/*********************** End of File ******************************/
