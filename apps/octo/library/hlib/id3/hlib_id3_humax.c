/**
	@file     hlib_id3_humax.c
	@brief    humax own id3 parser

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
	Current version support ID3V1 and ID3V2 (TextFrame(TXXX) and APIC Frame only)
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

#include "_hlib_stdlib.h"
#include "_hlib_mem.h"
#include "_hlib_sem.h"
#include "_hlib_log.h"
#include "_hlib_codepage.h"
#include "_hlib_macro.h"

#include "_hlib_id3.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
enum
{
	id3_enc_none = -1,
	id3_enc_8859_1,
	id3_enc_utf16,
	id3_enc_utf16be,
	id3_enc_utf8,
	id3_enc_unknown
} id3_enc_e;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
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
} id3_mpeg_t;

//	FRAME - HEADER
typedef struct
{
	HUINT8	aucFrameId[4];
	HINT32	nFrameSize;
	HUINT8	aucFrameFlag[2];
	HUINT8	*pucData;
} id3_frame_t;

//	FRAME - APIC
typedef struct
{
	HCHAR	szMimeType[64];
	HCHAR	szDescription[64];
	HUINT8	ucPictureType;
	HINT32	nSize;
	HUINT8	*pucData;
} id3_apic_t;

//	ID3 - MAIN INFO
typedef struct
{
	HBOOL		bID3V2;
	HINT32		nVersion;
	HINT32		nTagSize;
	HINT32		nTagStartPos;
	HINT32		nMpegPos;

	HINT32		nFileHandle;

	HINT32		nFrameCount;
	id3_frame_t	*pstFrameInfos;

	id3_mpeg_t			stMp3Header;
	HxID3_MusicInfo_t	stMusicInfo;
} id3_info_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ID3_V1_NR_GENRE 148
static const char *ID3_v1_Genre_Desc[ID3_V1_NR_GENRE] =
{
	"Blues",             //0
	"Classic Rock",      //1
	"Country",           //2
	"Dance",             //3
	"Disco",             //4
	"Funk",              //5
	"Grunge",            //6
	"Hip-Hop",           //7
	"Jazz",              //8
	"Metal",             //9
	"New Age",           //10
	"Oldies",            //11
	"Other",             //12
	"Pop",               //13
	"R&B",               //14
	"Rap",               //15
	"Reggae",            //16
	"Rock",              //17
	"Techno",            //18
	"Industrial",        //19
	"Alternative",       //20
	"Ska",               //21
	"Death Metal",       //22
	"Pranks",            //23
	"Soundtrack",        //24
	"Euro-Techno",       //25
	"Ambient",           //26
	"Trip-Hop",          //27
	"Vocal",             //28
	"Jazz+Funk",         //29
	"Fusion",            //30
	"Trance",            //31
	"Classical",         //32
	"Instrumental",      //33
	"Acid",              //34
	"House",             //35
	"Game",              //36
	"Sound Clip",        //37
	"Gospel",            //38
	"Noise",             //39
	"AlternRock",        //40
	"Bass",              //41
	"Soul",              //42
	"Punk",              //43
	"Space",             //44
	"Meditative",        //45
	"Instrumental Pop",  //46
	"Instrumental Rock", //47
	"Ethnic",            //48
	"Gothic",            //49
	"Darkwave",          //50
	"Techno-Industrial", //51
	"Electronic",        //52
	"Pop-Folk",          //53
	"Eurodance",         //54
	"Dream",             //55
	"Southern Rock",     //56
	"Comedy",            //57
	"Cult",              //58
	"Gangsta",           //59
	"Top 40",            //60
	"Christian Rap",     //61
	"Pop/Funk",          //62
	"Jungle",            //63
	"Native American",   //64
	"Cabaret",           //65
	"New Wave",          //66
	"Psychadelic",       //67
	"Rave",              //68
	"Showtunes",         //69
	"Trailer",           //70
	"Lo-Fi",             //71
	"Tribal",            //72
	"Acid Punk",         //73
	"Acid Jazz",         //74
	"Polka",             //75
	"Retro",             //76
	"Musical",           //77
	"Rock & Roll",       //78
	"Hard Rock",         //79
	// following are winamp extentions
	"Folk",                  //80
	"Folk-Rock",             //81
	"National Folk",         //82
	"Swing",                 //83
	"Fast Fusion",           //84
	"Bebob",                 //85
	"Latin",                 //86
	"Revival",               //87
	"Celtic",                //88
	"Bluegrass",             //89
	"Avantgarde",            //90
	"Gothic Rock",           //91
	"Progressive Rock",      //92
	"Psychedelic Rock",      //93
	"Symphonic Rock",        //94
	"Slow Rock",             //95
	"Big Band",              //96
	"Chorus",                //97
	"Easy Listening",        //98
	"Acoustic",              //99
	"Humour",                //100
	"Speech",                //101
	"Chanson",               //102
	"Opera",                 //103
	"Chamber Music",         //104
	"Sonata",                //105
	"Symphony",              //106
	"Booty Bass",            //107
	"Primus",                //108
	"Porn Groove",           //109
	"Satire",                //110
	"Slow Jam",              //111
	"Club",                  //112
	"Tango",                 //113
	"Samba",                 //114
	"Folklore",              //115
	"Ballad",                //116
	"Power Ballad",          //117
	"Rhythmic Soul",         //118
	"Freestyle",             //119
	"Duet",                  //120
	"Punk Rock",             //121
	"Drum Solo",             //122
	"A capella",             //123
	"Euro-House",            //124
	"Dance Hall",            //125
	"Goa",                   //126
	"Drum & Bass",           //127
	"Club-House",            //128
	"Hardcore",              //129
	"Terror",                //130
	"Indie",                 //131
	"Britpop",               //132
	"Negerpunk",             //133
	"Polsk Punk",            //134
	"Beat",                  //135
	"Christian Gangsta Rap", //136
	"Heavy Metal",           //137
	"Black Metal",           //138
	"Crossover",             //139
	"Contemporary Christian",//140
	"Christian Rock ",       //141
	"Merengue",              //142
	"Salsa",                 //143
	"Trash Metal",           //144
	"Anime",                 //145
	"JPop",                  //146
	"Synthpop"               //147
};

static const HINT32 s_anBitrateTable[3][16] = {
	{-1, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, -1},
	{-1, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, -1},
	{-1, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, -1},
};

static const HINT32 s_anSampleRateTable[3][4] = {
	{44100, 48000, 32000, -1},		// MPEG v1
	{22050, 24000, 16000, -1},		// MPEG v2
	{11025, 12000, 8000, -1},		// MPEG v2.5
};

/*******************************************************************/
/********************      extern functions     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
id3_info_t*	hlib_id3_open(const HCHAR *pszFileName)
{
	HINT32	nFileHandle = 0, nRead = 0;
	id3_info_t	*pstInfo = NULL;

	HUINT8	aucBuffer[128];
	HUINT64	ullFileSize;
	HINT32	nReadByte;

	nFileHandle = open(pszFileName, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (nFileHandle < 0)
	{
		HxLOG_Error("fail to open file [%s]\n", pszFileName);

		goto err_rtn;
	}

	pstInfo = (id3_info_t*)HLIB_STD_MemCalloc(sizeof(id3_info_t));
	pstInfo->nFileHandle = nFileHandle;

	//	read mp3 id3 header.
	nRead = read(nFileHandle, aucBuffer, 10);
	if(nRead < 0)
	{
		HxLOG_Error("fail to read file [%s]\n", pszFileName);
	}
	
	if (HxSTD_StrNCmp(aucBuffer, "ID3", 3) == 0)
	{
		//	ID3V2 tag is existed.
		pstInfo->bID3V2 = TRUE;
		pstInfo->nVersion = HxMACRO_SWAP16(HxMACRO_Get16Bit(&aucBuffer[3]));
		pstInfo->nTagSize = (HINT32)(aucBuffer[6] & 0x7F) << 21 |
							(HINT32)(aucBuffer[7] & 0x7F) << 14 |
							(HINT32)(aucBuffer[8] & 0x7F) << 7  |
							(HINT32)(aucBuffer[9] & 0x7F);

		pstInfo->nTagStartPos = 10;
		pstInfo->nMpegPos     = pstInfo->nTagSize + pstInfo->nTagStartPos;
	}
	{
		ullFileSize = lseek(nFileHandle, -128, SEEK_END);
		nReadByte = read(nFileHandle, aucBuffer, 128);
		if (HxSTD_StrNCmp(aucBuffer, "TAG", 3) == 0)
		{
			//	ID3V1 tag is existed
			HLIB_STD_StrNCpySafe(pstInfo->stMusicInfo.szTitle, &aucBuffer[3], 31);
			HLIB_STD_StrNCpySafe(pstInfo->stMusicInfo.szArtist, &aucBuffer[33], 31);
			HLIB_STD_StrNCpySafe(pstInfo->stMusicInfo.szAlbum, &aucBuffer[63], 31);
			HLIB_STD_StrNCpySafe(pstInfo->stMusicInfo.szYear, &aucBuffer[93], 5);
			//HLIB_STD_StrNCpySafe(pstInfo->stMusicInfo.szComment, &aucBuffer[97], 31);
			if (aucBuffer[127] < ID3_V1_NR_GENRE)
				HLIB_STD_StrNCpySafe(pstInfo->stMusicInfo.szGenre, ID3_v1_Genre_Desc[aucBuffer[127]], HxID3_FIELD_SIZE_SHORT);
			else
				HLIB_STD_StrNCpySafe(pstInfo->stMusicInfo.szGenre, "other", HxID3_FIELD_SIZE_SHORT);

			HxLOG_Debug("ID3V1 Tag Title  : [%s]\n", pstInfo->stMusicInfo.szTitle);
			HxLOG_Debug("ID3V1 Tag Artist : [%s]\n", pstInfo->stMusicInfo.szArtist);
			HxLOG_Debug("ID3V1 Tag Album  : [%s]\n", pstInfo->stMusicInfo.szAlbum);
			HxLOG_Debug("ID3V1 Tag Year   : [%s]\n", pstInfo->stMusicInfo.szYear);
			HxLOG_Debug("ID3V1 Tag Genre  : [%s]\n", pstInfo->stMusicInfo.szGenre);
		}
	}

	return pstInfo;

err_rtn:
	if (nFileHandle>0)
		close(nFileHandle);
	if (pstInfo)
		HLIB_STD_MemFree(pstInfo);

	return NULL;
}

HERROR	hlib_id3_close(id3_info_t *pstInfo)
{
	if (pstInfo)
	{
		if (pstInfo->nFileHandle)
			close(pstInfo->nFileHandle);

		HLIB_STD_MemFree(pstInfo);
	}

	return ERR_OK;
}

static HERROR 		hlib_id3_parseMP3Header(id3_mpeg_t  *header, HUINT8 *data)
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
	header->bitrate = s_anBitrateTable[header->version][bitrate_index] * 1024;
	bitrate = s_anBitrateTable[header->version][bitrate_index];
	// Read sample rate
	samplerate_index = (data[2] >> 2) & 0x03;
	header->sample_rate = s_anSampleRateTable[header->version][samplerate_index];
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

static HINT32	hlib_id3_getXingOffset(const id3_mpeg_t* header)
{
	if (header->version == 0) {
		// MPEG1
		return (header->channel_mode == 3 ? 17 : 32);
	} else {
		// MPEG2/2.5
		return (header->channel_mode == 3 ? 9 : 17);
	}
}

static HERROR 		hlib_id3_parseXingHeader(HINT32 nFileHandle, id3_mpeg_t *header)
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

HERROR	hlib_id3_getMpegInfo(id3_info_t *info)
{
	HINT32	nFileHandle;
	HUINT8	aucBuffer[4];
	HUINT64	ullFileSize;
	HINT64	llOffset;

	id3_mpeg_t	*header;

	ullFileSize = lseek(info->nFileHandle, 0, SEEK_END);
	if (lseek(info->nFileHandle, info->nMpegPos, SEEK_SET) < 0)
		return ERR_FAIL;

	if (read(info->nFileHandle, aucBuffer, 4) != 4)
		return ERR_FAIL;

	header = &info->stMp3Header;
	nFileHandle = info->nFileHandle;
	for (;;)
	{
		if (hlib_id3_parseMP3Header(&info->stMp3Header, aucBuffer) == ERR_OK)
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
			llOffset = lseek(nFileHandle, hlib_id3_getXingOffset(header), SEEK_CUR);
			if (llOffset == -1)
				break;

			/* Parse Xing VBR header if any. This sets number_of_frames and data_size. */
			if (hlib_id3_parseXingHeader(nFileHandle, header) == ERR_OK) {
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
			return ERR_OK;
		}

		memmove(&aucBuffer[0], &aucBuffer[1], sizeof(HUINT8)*3);
		if (read(nFileHandle, &aucBuffer[3], 1) != 1)
			break;
	}

	return ERR_FAIL;
}

HERROR	hlib_id3_clearFrame(id3_info_t *info)
{
	HINT32	i;

	if (info->pstFrameInfos)
	{
		for (i = 0; i < info->nFrameCount; i++)
		{
			if (info->pstFrameInfos[i].pucData)
				HLIB_STD_MemFree(info->pstFrameInfos[i].pucData);
		}

		HLIB_STD_MemFree(info->pstFrameInfos);
		info->pstFrameInfos = NULL;
		info->nFrameCount = 0;
	}

	return ERR_OK;
}

id3_frame_t*	hlib_id3_readFrame(id3_info_t *info, HINT32 *pnFrameNum, HINT32 nArgc, ...)
{
	HBOOL		bFound;
	HINT32		i, nFrameSize, nTagSize;
	HUINT8		aucFrameHeader[10];
	HUINT32		ulFrameId;

	id3_frame_t	*pstFrameList;
	va_list		ap;
	HUINT64	ullFileSize;

	if (info->pstFrameInfos)
		hlib_id3_clearFrame(info);

	if (lseek(info->nFileHandle, info->nTagStartPos, SEEK_SET) < 0)
		return NULL;

	//	Get frame id list.
	pstFrameList = (id3_frame_t*)HLIB_STD_MemCalloc(sizeof(id3_frame_t) * nArgc);

	va_start(ap, nArgc);
	for (i = 0; i < nArgc; i++)
		HxSTD_MemCopy(pstFrameList[i].aucFrameId, va_arg(ap, HCHAR *), 4);
	va_end(ap);

	nTagSize = info->nTagSize;

	while (nTagSize > 0)
	{
		if (read(info->nFileHandle, aucFrameHeader, 10) != 10)
		{
			if (pstFrameList)
				HLIB_STD_MemFree(pstFrameList);

			return NULL;
		}

		if (info->nVersion == 4)			//	id3v2.4 : synch safe integer
			nFrameSize = (HINT32)(aucFrameHeader[4] & 0x7F) << 21 |
						 (HINT32)(aucFrameHeader[5] & 0x7F) << 14 |
						 (HINT32)(aucFrameHeader[6] & 0x7F) <<  7 |
						 (HINT32)(aucFrameHeader[7] & 0x7F);
		else
			nFrameSize = HxMACRO_Get32Bit(&aucFrameHeader[4]);

		ulFrameId = *(HUINT32*)aucFrameHeader;
		if (ulFrameId == 0)
		{
			HxLOG_Debug("null frame id found... stop ..\n");
			break;
		}

		bFound = FALSE;
		for (i = 0; i < nArgc; i++)
		{
			if (HxSTD_MemCmp(aucFrameHeader, pstFrameList[i].aucFrameId, 4) == 0)
			{
				bFound = TRUE;
				break;
			}
		}

		if (bFound)
		{
			HxLOG_Debug("frame id [%c%c%c%c] size [%d]\n", aucFrameHeader[0], aucFrameHeader[1], aucFrameHeader[2], aucFrameHeader[3], nFrameSize);
			pstFrameList[i].nFrameSize = nFrameSize;
			pstFrameList[i].aucFrameFlag[0] = aucFrameHeader[8];
			pstFrameList[i].aucFrameFlag[1] = aucFrameHeader[9];
			pstFrameList[i].pucData = (HUINT8*)HLIB_STD_MemAlloc(pstFrameList[i].nFrameSize);
			if (read(info->nFileHandle, pstFrameList[i].pucData, pstFrameList[i].nFrameSize) != pstFrameList[i].nFrameSize)
			{
				HxLOG_Error("frame size is not matched!!!\n");
			}
		} else
		{
			ullFileSize = lseek(info->nFileHandle, nFrameSize, SEEK_CUR);
		}

		nTagSize -= (nFrameSize + 10);
	}

	info->nFrameCount = nArgc;
	info->pstFrameInfos = pstFrameList;

	if (pnFrameNum)
		*pnFrameNum = info->nFrameCount;

	return info->pstFrameInfos;
}

void	hlib_id3_changeEndianUCS2(HUINT16 *pusUCS2)
{
	while (*pusUCS2)
	{
		*pusUCS2 = HxMACRO_SWAP16(*pusUCS2);
		pusUCS2++;
	}
}

HERROR	hlib_id3_fillText(HCHAR *pszDest, HINT32 nLen, HUINT8 enc, HUINT8 *pucData, HINT32 frameSize)
{
	static HUINT16	*s_pusUCS2Str = NULL;
	static HCHAR	*s_pszSingleStr = NULL;

	if (s_pusUCS2Str == NULL)
		s_pusUCS2Str = (HUINT16*)HLIB_STD_MemAlloc(4096);
	if (s_pszSingleStr == NULL)
		s_pszSingleStr = (HCHAR*)HLIB_STD_MemAlloc(4096);

	if (frameSize > 4095)
	{
		HxLOG_Error("too long text frame : [%d]\n", frameSize);
		return ERR_FAIL;
	}

	if (enc == id3_enc_utf16 || enc == id3_enc_utf16be)
	{
		if (pucData[0] == 0xff && pucData[1] == 0xfe)
		{
			enc = id3_enc_utf16;
			pucData += 2;
			frameSize -= 2;
		} else
		if (pucData[0] == 0xfe && pucData[1] == 0xff)
		{
			enc = id3_enc_utf16be;
			pucData += 2;
			frameSize -= 2;
		}
		HxSTD_MemCopy(s_pusUCS2Str, pucData, frameSize);
		s_pusUCS2Str[frameSize / 2] = 0;
	} else
	{
		HxSTD_MemCopy(s_pszSingleStr, pucData, frameSize);
		s_pszSingleStr[frameSize] = 0;
	}

	switch (enc)
	{
		case id3_enc_utf16be:
			hlib_id3_changeEndianUCS2(s_pusUCS2Str);
			//	go through
		case id3_enc_utf16:
			HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8, s_pusUCS2Str, s_pszSingleStr, 4096);
			HLIB_STD_StrUtf8NCpy(pszDest, s_pszSingleStr, nLen);
			break;

		case id3_enc_8859_1:
			HLIB_CP_ConvertCodepageToUCS2(eHxCP_ISO_8859_1, s_pszSingleStr, s_pusUCS2Str, 4096);
			HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8, s_pusUCS2Str, s_pszSingleStr, 4096);
			HLIB_STD_StrUtf8NCpy(pszDest, s_pszSingleStr, nLen);
			break;
		case id3_enc_utf8:
		default:
			HLIB_STD_StrUtf8NCpy(pszDest, s_pszSingleStr, nLen);
			break;
	}

	return ERR_OK;
}

HERROR	hlib_id3_fillTextFrame(id3_info_t *info, const HCHAR *pszFrameId, HCHAR *pszDest, HINT32 nLen)
{
	HINT32	i, frameSize;
	HUINT8	*p;
	HUINT8	enc;

	if (pszFrameId[0] != 'T')
	{
		HxLOG_Error("frame is not Text Field - [%s]\n", pszFrameId);
		return ERR_FAIL;
	}

	for (i = 0; i < info->nFrameCount; i++)
	{
		if (HxSTD_MemCmp(info->pstFrameInfos[i].aucFrameId, pszFrameId, 4) == 0)
		{
			p = info->pstFrameInfos[i].pucData;
			if (p == NULL)
			{
				HxLOG_Warning("null data found. (%s)\n", pszFrameId);
				return ERR_FAIL;
			}

			frameSize = info->pstFrameInfos[i].nFrameSize;

			enc = *p++;		frameSize--;
			hlib_id3_fillText(pszDest, nLen, enc, p, frameSize);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR	hlib_id3_fillAttachedPictureFrame(id3_info_t *info, HINT32 nFrameSize, HUINT8 *pucData, id3_apic_t **pstPicInfo)
{
	HUINT8		enc;
	id3_apic_t	*picInfo;

	picInfo = (id3_apic_t*)HLIB_STD_MemCalloc(sizeof(id3_apic_t));

	enc = *pucData++;		nFrameSize--;

	HLIB_STD_StrNCpySafe(picInfo->szMimeType, pucData, 64);
	pucData += HxSTD_StrLen(picInfo->szMimeType) + 1;
	nFrameSize -= (HxSTD_StrLen(picInfo->szMimeType) + 1);

	picInfo->ucPictureType = *pucData++;		nFrameSize--;

	HLIB_STD_StrNCpySafe(picInfo->szDescription, pucData, 64);
	pucData += HxSTD_StrLen(picInfo->szDescription) + 1;
	nFrameSize -= (HxSTD_StrLen(picInfo->szDescription) + 1);

	picInfo->nSize = nFrameSize;
	picInfo->pucData = (HUINT8*)HLIB_STD_MemDup(pucData, nFrameSize);

	HxLOG_Debug("mime-type : [%s]\n", picInfo->szMimeType);
	HxLOG_Debug("description : [%s]\n", picInfo->szDescription);
	HxLOG_Debug("picture type : [%d]\n", picInfo->ucPictureType);
	HxLOG_Debug("picture size : [%d]\n", picInfo->nSize);

	*pstPicInfo = picInfo;

	return ERR_OK;
}

HERROR	hlib_id3_freeAttachedPictureFrame(id3_apic_t *pstPicInfo)
{
	if (pstPicInfo)
	{
		if (pstPicInfo->pucData)
			HLIB_STD_MemFree(pstPicInfo->pucData);
		HLIB_STD_MemFree(pstPicInfo);
	}

	return ERR_OK;
}

HERROR	hlib_id3_getInfo(const HCHAR *pszFileName, HxID3_MusicInfo_t *pstMusicInfo, HxID3_Picture_t *pstPictureInfo)
{
	id3_info_t	*info;

	info = hlib_id3_open(pszFileName);
	if (info)
	{
		if (info->bID3V2)
		{
			//	parse id3 tag ...
			if (pstMusicInfo)
			{
				hlib_id3_readFrame(info, NULL, 6, "TPE1", "TSOA", "TYER", "TIT2", "TPUB", "TCON");
				hlib_id3_fillTextFrame(info, "TPE1", info->stMusicInfo.szArtist,    HxID3_FIELD_SIZE_LONG);
				hlib_id3_fillTextFrame(info, "TSOA", info->stMusicInfo.szAlbum,     HxID3_FIELD_SIZE_LONG);
				hlib_id3_fillTextFrame(info, "TIT2", info->stMusicInfo.szTitle,     HxID3_FIELD_SIZE_LONG);
				hlib_id3_fillTextFrame(info, "TYER", info->stMusicInfo.szYear,      HxID3_FIELD_SIZE_SHORT);
				hlib_id3_fillTextFrame(info, "TCON", info->stMusicInfo.szGenre,     HxID3_FIELD_SIZE_SHORT);
				hlib_id3_fillTextFrame(info, "TPUB", info->stMusicInfo.szPublisher, HxID3_FIELD_SIZE_LONG);
				hlib_id3_clearFrame(info);
			}

			if (pstPictureInfo)
			{
				HINT32	nSize;
				id3_frame_t	*pstFrameList;

				pstFrameList = hlib_id3_readFrame(info, &nSize, 1, "APIC");
				if ((pstFrameList) && (pstFrameList->pucData))
				{
					id3_apic_t	*pstPicInfo;

					hlib_id3_fillAttachedPictureFrame(info, pstFrameList->nFrameSize, pstFrameList->pucData, &pstPicInfo);
					if (pstPicInfo)
					{
						pstPictureInfo->bIsID3V2 = TRUE;
						pstPictureInfo->ucPictureType = pstPicInfo->ucPictureType;
						pstPictureInfo->ulPictureSize = pstPicInfo->nSize;
						pstPictureInfo->pucData = (HUINT8*)HLIB_STD_MemDup(pstPicInfo->pucData, pstPicInfo->nSize);
						hlib_id3_freeAttachedPictureFrame(pstPicInfo);
					}
				}

				hlib_id3_clearFrame(info);
			}
		}

		if (pstMusicInfo)
		{
			hlib_id3_getMpegInfo(info);
			info->stMusicInfo.nVersion = info->stMp3Header.version;
			info->stMusicInfo.nSampleRate = info->stMp3Header.sample_rate;
			info->stMusicInfo.ulDuration = info->stMp3Header.duration;
			info->stMusicInfo.nBitRate = info->stMp3Header.bitrate;

			HxSTD_MemCopy(pstMusicInfo, &info->stMusicInfo, sizeof(HxID3_MusicInfo_t));
		} 
 
		hlib_id3_close(info);

		return ERR_OK;
	}

	return ERR_FAIL;
}



/*******************************************************************************************
 * Static
 ******************************************************************************************/

/*******************************************************************************************
 * Interfaces
 ******************************************************************************************/
HERROR	HLIB_ID3_GetID3Picture(const HCHAR *pszFileName, HxID3_Picture_t *pstInfo)
{
	return hlib_id3_getInfo(pszFileName, NULL, pstInfo);
}

HERROR	HLIB_ID3_GetID3MusicInfo(const HCHAR *pszFileName, HxID3_MusicInfo_t *pstInfo)
{
	return hlib_id3_getInfo(pszFileName, pstInfo, NULL);
}

void		HLIB_ID3_MemFree(void *p)
{
	HLIB_MEM_Free(p);
}

/*********************** End of File ******************************/

