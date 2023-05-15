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
#include "_hlib_macro.h"

#include "_hlib_id3.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define FLAC_SYNC_STRING "fLaC"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum {
	flac_metadata_type_streaminfo = 0,
	flac_metadata_type_padding,
	flac_metadata_type_application,
	flac_metadata_type_seektable,
	flac_metadata_type_vorbis_comment, //4
	flac_metadata_type_cuesheet,
	flac_metadata_type_picture, // 6
	flac_metadata_type_undefined,
} flac_metadata_type_e;

typedef enum {
	flac_vorbis_data_title = 0,
	flac_vorbis_data_album,
	flac_vorbis_data_album_artist,
	flac_vorbis_data_artist,
	flac_vorbis_data_genre,
	flac_vorbis_data_date,

} flac_vorbis_data_type_e;
typedef struct {
	HCHAR field[32];
	HINT32 fieldType;
} flac_entry_data_t;

typedef struct {
	HBOOL bInit;
	FILE *pFileHandle;

	HINT32 offset;
	HINT32 meta_offset;
	HINT32 pic_offset;
	HINT32 vorbis_offset;

} flac_context_t;

typedef struct {
	HINT32 length;
	HUINT8 *pData;
} flac_comment_entry_t;

typedef struct {

	flac_comment_entry_t vendor_str;
	HINT32 comment_num;
	flac_comment_entry_t *pComments;

} flac_vorbis_comment_t;

/* see id3 v2.4 APIC tag) */
typedef enum {
	flac_picture_type_other = 0,
	flac_picture_type_file_icon_stadard = 1,
	flac_picture_type_file_icon = 2,
	flac_picture_type_front_cover = 3,
	flac_picture_type_back_cover = 4,
	flac_picture_type_leaflet_page = 5,
	flac_picture_type_media = 6,
	flac_picture_type_lead_artist = 7,
	flac_picture_type_artist = 8,
	flac_picture_type_conductor = 9,
	flac_picture_type_band = 10,
	flac_picture_type_composer = 11,
	flac_picture_type_lyricist = 12,
	flac_picture_type_recording_ = 13,
	flac_picture_type_during_recording = 14,
	flac_picture_type_during_performance = 15,
	flac_picture_type_video_screen_capture = 16,
	flac_picture_type_fish = 17,
	flac_picture_type_illustraton = 18,
	flac_picture_type_band_logotype = 19,
	flac_picture_type_publisher_logotype = 20,
	flac_picture_type_undefined
} flac_picture_type_e;

typedef struct {
	flac_picture_type_e type;

	HCHAR	*mime_type;
	HCHAR	*description;
	HUINT32 width;
	HUINT32 height;
	HUINT32 depth;
	HUINT32 colors;
	HUINT32 data_len;

	HUINT8 *data;
} flac_picture_t;

typedef struct {
	HBOOL	last_flag;
	flac_metadata_type_e type;
	HINT32	data_len;
} flac_block_header_t;

typedef struct {
	flac_block_header_t header;
	void * data;
} flac_metadata_block_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static const flac_entry_data_t VORBIS_Commnts[] =
{
	{ "TITLE=", 		flac_vorbis_data_title 		},
	{ "ALBUM=", 		flac_vorbis_data_album		},
	{ "ALBUM ARTIST=",	flac_vorbis_data_album_artist	},
	{ "ALBUMARTIST=",	flac_vorbis_data_album_artist	},
	{ "ARTIST=", 		flac_vorbis_data_artist 	},
	{ "GENRE=", 		flac_vorbis_data_genre 		},
	{ "DATE=", 			flac_vorbis_data_date 		},
};

/*******************************************************************/
/********************      extern functions     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


/*******************************************************************************************
 * Static
 ******************************************************************************************/
static HBOOL	hlib_flac_isflac(FILE *fp)
{
	HUINT8 readbuf[4] = { 0 };
	HINT32 readcnt = 0;
	HINT32 i = 0;

	errno =0;
	readcnt = fread( readbuf, 1, sizeof(readbuf), fp );
	if(errno)
		return FALSE;
	if(sizeof(readbuf) != readcnt)
		return FALSE;

	if(0 == HxSTD_MemCmp(readbuf, "ID3", 3))
	{
		unsigned tag_length = 0;
		HxLOG_Debug("flac detected id3\n");
		/* Skip ID3 Tag */

		/* Skip to the tag length */
		if(fseek(fp, 2, SEEK_CUR) < 0)
			return FALSE;

		for(i = 0; i < 4; i++) {
			if(fread(readbuf, 1, 1, fp) < 1 || readbuf[0] & 0x80)
				return FALSE;
			tag_length <<= 7;
			tag_length |= (readbuf[0] & 0x7F);
		}

		/* Skip  tag data */
		if(fseek(fp, tag_length, SEEK_CUR) < 0)
			return FALSE;

		errno = 0;
		readcnt = fread(readbuf, 1, 4, fp);
		if(errno)
			return FALSE;
		else if(readcnt != 4)
			return FALSE;
	}

	if(0 == HxSTD_MemCmp(FLAC_SYNC_STRING, readbuf, sizeof(readbuf)))
	{
		return TRUE;
	}

	return FALSE;
}

static HERROR	hlib_flac_init(const HCHAR *pszFileName, flac_context_t *pstFlacCtx)
{
	pstFlacCtx->pFileHandle = fopen( pszFileName, "r");
	if ( NULL == pstFlacCtx->pFileHandle )
		return ERR_FAIL;

	if ( !hlib_flac_isflac( pstFlacCtx->pFileHandle) )
		return ERR_FAIL;

	pstFlacCtx->bInit = TRUE;
	return ERR_OK;
}

static HERROR	hlib_flac_release_vorbis_comment( flac_vorbis_comment_t *pstVorbis )
{
	if ( NULL == pstVorbis )
		return ERR_FAIL;
	if ( pstVorbis->comment_num && NULL != pstVorbis->pComments )
	{
		HINT32 i = 0;
		for( i = 0; i < pstVorbis->comment_num; ++i )
		{
			if ( pstVorbis->pComments[i].pData && pstVorbis->pComments[i].pData )
			{
				HLIB_STD_MemFree( pstVorbis->pComments[i].pData );
				pstVorbis->pComments[i].pData = NULL;
			}
		}

		HLIB_STD_MemFree( pstVorbis->pComments );
		pstVorbis->pComments = NULL;
	}
	return ERR_OK;
}

static HERROR	hlib_flac_release_picture( flac_picture_t *pstPicture )
{
	if ( NULL == pstPicture )
		return ERR_FAIL;

	if ( pstPicture->mime_type )
	{
		HLIB_MEM_Free(pstPicture->mime_type);
		pstPicture->mime_type = NULL;
	}
	if ( pstPicture->description )
	{
		HLIB_MEM_Free(pstPicture->description);
		pstPicture->description = NULL;
	}
	if ( pstPicture->data )
	{
		HLIB_MEM_Free(pstPicture->data);
		pstPicture->data = NULL;
	}

	return ERR_OK;
}

static HERROR	hlib_flac_release_metadata(flac_metadata_block_t *pstMetadata)
{
	if ( NULL == pstMetadata )
		return ERR_FAIL;

	switch ( pstMetadata->header.type )
	{
		case flac_metadata_type_vorbis_comment :
			hlib_flac_release_vorbis_comment( (flac_vorbis_comment_t *)pstMetadata->data );
			break;
		case flac_metadata_type_picture :
			hlib_flac_release_picture( (flac_picture_t *)pstMetadata->data );
			break;
		default :
			break;
	}

	if ( pstMetadata->data )
	{
		HLIB_STD_MemFree(pstMetadata->data);
		pstMetadata->data = NULL;
	}
	return ERR_OK;
}

static HERROR	hlib_flac_deinit(flac_context_t *pstFlacCtx)
{
	if ( pstFlacCtx->pFileHandle )
	{
		fclose( pstFlacCtx->pFileHandle );
		pstFlacCtx->pFileHandle = NULL;
	}

	return ERR_OK;
}

HUINT32  hlib_flac_little_endian32(HUINT8 *bitData, HUINT32 bytes)
{
	HUINT32 ret = 0;
	HINT32 i;
	bitData += bytes;

	for(i = 0; i < bytes; i++)
		ret = (ret << 8) | (HUINT32)(*--bitData);

	return ret;
}

static HUINT32	 hlib_flac_unpack32(HUINT8 *bitData, HINT32 bytes)
{
	HUINT32  ret = 0;
	HINT32 i = 0;

	for(i = 0; i < bytes; i++)
	{
		ret = (ret << 8) | (HUINT32)(*bitData++);
	}
	return ret;
}

static HERROR	hlib_flac_convert_id3MusicInfo( flac_vorbis_comment_t *pstVorbis, HxID3_MusicInfo_t * pstInfo )
{
	HINT32 i = 0, j = 0;
	HCHAR *pValue = NULL;
	HCHAR *pEqulPos = NULL;
	HINT32 vorbis_num = 0;
	HINT32 fieldlen = 0;
	flac_entry_data_t *pEntryData = NULL;
	if ( NULL == pstVorbis || NULL == pstInfo )
		return ERR_FAIL;

	vorbis_num = sizeof(VORBIS_Commnts) / sizeof(flac_entry_data_t);
	for ( i = 0; i < pstVorbis->comment_num; ++i )
	{
		for ( j = 0; j < vorbis_num ; ++j )
		{
			pEntryData = (flac_entry_data_t *)&VORBIS_Commnts[j];

			pEqulPos = HxSTD_StrChar((HCHAR *)pstVorbis->pComments[i].pData, '=');
			if ( NULL == pEqulPos )
				continue;

			fieldlen = pEqulPos - (HCHAR *)pstVorbis->pComments[i].pData;

			if ( NULL != HLIB_STD_StrCaseStr((HCHAR *)pstVorbis->pComments[i].pData, pEntryData->field) )
			{
				if ( fieldlen > HxSTD_StrLen(pEntryData->field) + 4)
					continue;

				pValue = (HCHAR *)pEqulPos + 1;
				switch ( pEntryData->fieldType )
				{

					case flac_vorbis_data_title :
						HxSTD_StrNCpy( pstInfo->szTitle, pValue, sizeof(pstInfo->szTitle)-1 );
						break;
					case flac_vorbis_data_album :
						HxSTD_StrNCpy( pstInfo->szAlbum, pValue, sizeof(pstInfo->szAlbum)-1 );
						break;
					case flac_vorbis_data_artist :
						HxSTD_StrNCpy( pstInfo->szArtist, pValue, sizeof(pstInfo->szArtist)-1 );
						break;
					case flac_vorbis_data_genre :
						HxSTD_StrNCpy( pstInfo->szGenre, pValue, sizeof(pstInfo->szGenre)-1 );
						break;
					case flac_vorbis_data_date :
						HxSTD_StrNCpy( pstInfo->szYear, pValue, sizeof(pstInfo->szYear)-1 );
						break;
					default :
						break;
				}
			}
		}
	}

	return ERR_OK;
}

static HERROR	hlib_flac_convert_id3Picture( flac_picture_t *pstPicture, HxID3_Picture_t * pstMusicPic )
{
	if ( pstPicture->data_len && pstPicture->data)
	{
		pstMusicPic->ucPictureType = (HUINT8)pstPicture->type;
		pstMusicPic->ulPictureSize =  pstPicture->data_len;
		pstMusicPic->pucData =  (HUINT8 *)HLIB_STD_MemDup( pstPicture->data, pstPicture->data_len);
	}

	return ERR_OK;
}

static HERROR flac_picture_cstring(FILE *fp, HUINT8 **data, HUINT32 *length)
{
	HUINT8 buffer[4] = { 0 };
	HINT32 datalen = 0;

	HUINT32 length_len = sizeof(buffer); /* convert to bytes */

	if( 1 != fread(buffer, length_len, 1, fp) )
		return ERR_FAIL;
	datalen = hlib_flac_unpack32(buffer, length_len);

	if(0 != *data)
		free(*data);

	if(0 == (*data = HLIB_STD_MemCalloc(datalen + 1)))
		return ERR_FAIL;

	if(datalen > 0)
	{
		if( 1 != fread(*data, datalen, 1, fp) )
			return ERR_FAIL;
	}

	(*data)[datalen] = '\0';
	if ( length )
		*length = datalen ;

	return ERR_OK;
}

static HERROR hlib_flac_parse_picture(FILE *fp, flac_picture_t *pstPicture)
{
	HUINT8 buffer[4] = { 0 };
	HUINT32 len = 0;;

	len = sizeof(buffer);

	if( 1 != fread(buffer, len, 1, fp) )
		return ERR_FAIL;
	pstPicture->type = (flac_picture_type_e)hlib_flac_unpack32(buffer, len);

	if( ERR_OK != flac_picture_cstring(fp, (HUINT8**)(&(pstPicture->mime_type)), NULL) )
		return ERR_FAIL;

	if( ERR_OK != flac_picture_cstring(fp, (HUINT8**)&(pstPicture->description), NULL) )
		return ERR_FAIL;

	if( 1 != fread(buffer, len, 1, fp) )
		return ERR_FAIL;
	pstPicture->width = hlib_flac_unpack32(buffer, len);

	if( 1 != fread(buffer, len, 1, fp) )
		return ERR_FAIL;
	pstPicture->height = hlib_flac_unpack32(buffer, len);

	if( 1 != fread(buffer, len, 1, fp) )
		return ERR_FAIL;
	pstPicture->depth = hlib_flac_unpack32(buffer, len);

	if( 1 != fread(buffer, len, 1, fp) )
		return ERR_FAIL;
	pstPicture->colors = hlib_flac_unpack32(buffer, len);

	if( ERR_OK != flac_picture_cstring(fp, &(pstPicture->data), &(pstPicture->data_len)) )
		return ERR_FAIL;

	return ERR_OK;
}

static HERROR	hlib_flac_parse_vorbis_comment_entry(FILE *fp, flac_comment_entry_t *pstEntry)
{
	HUINT8 buf[4] = { 0 };
	HINT32 pstEntry_length_len = sizeof(buf);
	if ( NULL == pstEntry )
		return ERR_FAIL;

	if( 1 != fread(buf, pstEntry_length_len, 1, fp) )
		return ERR_FAIL;;
	pstEntry->length = hlib_flac_little_endian32(buf, pstEntry_length_len);

	if(0 != pstEntry->pData)
		HLIB_STD_MemFree(pstEntry->pData);

	if(pstEntry->length == 0) {
		pstEntry->pData = NULL;
		return ERR_OK;
	}

	pstEntry->pData = HLIB_STD_MemAlloc(pstEntry->length + 1);
	if(NULL == pstEntry->pData)
		return ERR_FAIL;

	if( 1 != fread(pstEntry->pData, pstEntry->length, 1, fp) )
		return ERR_FAIL;

	pstEntry->pData[pstEntry->length] = '\0';
	return ERR_OK;
}

static HERROR	hlib_flac_parse_vorbis_comment(FILE *fp, flac_vorbis_comment_t *pstVorbis)
{
	HINT32  i = 0;
	HUINT8 buf[4] = { 0 };
	HINT32 num_comments_len = sizeof(buf);

	if ( NULL == pstVorbis )
		return ERR_FAIL;

	if (ERR_OK != hlib_flac_parse_vorbis_comment_entry(fp, &(pstVorbis->vendor_str)))
		return ERR_FAIL;

	if ( 1 != fread(buf, num_comments_len, 1, fp))
		return ERR_FAIL;
	pstVorbis->comment_num = hlib_flac_little_endian32(buf, num_comments_len);

	HxLOG_Debug("[hlib] flac vorbis_comment num : %d.\n", pstVorbis->comment_num);
	if(pstVorbis->comment_num == 0) {
		pstVorbis->pComments = NULL;
	}
	else
	{
		pstVorbis->pComments = HLIB_STD_MemCalloc(pstVorbis->comment_num * sizeof(flac_comment_entry_t));
	}

	if ( NULL == pstVorbis->pComments )
		return ERR_FAIL;

	for(i = 0; i < pstVorbis->comment_num; ++i)
	{
		if( ERR_OK != hlib_flac_parse_vorbis_comment_entry(fp, &pstVorbis->pComments[i]))
			return ERR_FAIL;

		HxLOG_Debug("[hlib] vorbis_comment[%d] : %s.\n", i, pstVorbis->pComments[i].pData );
	}
	return ERR_OK;
}

static HERROR	hlib_flac_metadata_block_read_header(FILE *fp, flac_block_header_t *pstBlockHeader)
{
	HUINT8 raw_header[4] = { 0 }; // 32bit
	if(fread(raw_header, 1, sizeof(raw_header), fp) != sizeof(raw_header))
		return ERR_FAIL;
	pstBlockHeader->last_flag = 0x80 &raw_header[0] ? TRUE : FALSE;
	pstBlockHeader->type = (flac_metadata_type_e)(0x7F &raw_header[0]);
	pstBlockHeader->data_len = hlib_flac_unpack32(raw_header + 1, 3);
	return ERR_OK;
}

static HERROR	hlib_flac_metadata_block_read_data(FILE *fp, flac_block_header_t *pstHeader, void **ppData)
{
	switch ( pstHeader->type )
	{
		case flac_metadata_type_picture :
			{
				flac_picture_t *pPicture = (flac_picture_t *)HLIB_STD_MemCalloc(sizeof(flac_picture_t));
				hlib_flac_parse_picture( fp, pPicture );
				*ppData = (void *)pPicture;
			}
			break;
		case flac_metadata_type_vorbis_comment :
			{
				flac_vorbis_comment_t *pVorbis = (flac_vorbis_comment_t *)HLIB_STD_MemCalloc(sizeof(flac_vorbis_comment_t));
				hlib_flac_parse_vorbis_comment( fp, pVorbis );
				*ppData = (void *)pVorbis;
			}
			break;
		default :
			break;
	}
	return ERR_OK;
}

HERROR	hlib_flac_getMetaData(flac_context_t *pstFlacCtx, flac_metadata_block_t *pstMetadata, flac_metadata_type_e eMetaType)
{
	HBOOL bFind = FALSE;
	HERROR ret = ERR_FAIL;

	do
	{
		ret = hlib_flac_metadata_block_read_header(pstFlacCtx->pFileHandle, &pstMetadata->header);
		if ( ERR_OK != ret )
			return ERR_FAIL;


		if ( eMetaType == pstMetadata->header.type )
		{
			bFind = TRUE;
			break;
		}
		else if ( TRUE == pstMetadata->header.last_flag )
			break;
	} while ( -1 != fseek( pstFlacCtx->pFileHandle, pstMetadata->header.data_len, SEEK_CUR ) );

	if ( bFind )
	{
		pstFlacCtx->offset = ftell(pstFlacCtx->pFileHandle);
		if ( pstMetadata->header.data_len )
		{
			HxLOG_Debug("[hlib] pvData size : %d!!!!\n", pstMetadata->header.data_len);
			pstMetadata->data = NULL;
			hlib_flac_metadata_block_read_data( pstFlacCtx->pFileHandle, &pstMetadata->header, &pstMetadata->data );
		}
		return ERR_OK;
	}
	return ERR_FAIL;
}


/*******************************************************************************************
 * Interfaces
 ******************************************************************************************/

HERROR	HLIB_ID3_GetFlacPicture(const HCHAR *pszFileName, HxID3_Picture_t *pstInfo)
{
	HERROR ret = ERR_FAIL;

	flac_context_t stFlacCtx;
	flac_metadata_block_t flac_meta;

	HxSTD_MemSet( &stFlacCtx, 0x00, sizeof(flac_context_t) );
	HxSTD_MemSet( &flac_meta, 0x00, sizeof(flac_metadata_block_t) );

	ret = hlib_flac_init( pszFileName, &stFlacCtx );
	if ( ERR_OK != ret )
	{
		HxLOG_Error("flac init()... Failed!!!!\n");
		hlib_flac_deinit( &stFlacCtx );
		return ERR_FAIL;
	}
	ret = hlib_flac_getMetaData( &stFlacCtx, &flac_meta, flac_metadata_type_picture );
	if ( ERR_OK != ret )
	{
		HxLOG_Error("flac info() getMetadata/picture... Failed !!!!\n");
		hlib_flac_deinit( &stFlacCtx );
		return ERR_FAIL;
	}

	hlib_flac_convert_id3Picture( (flac_picture_t *)flac_meta.data, pstInfo );

	hlib_flac_release_metadata( &flac_meta );
	hlib_flac_deinit( &stFlacCtx );
	return ERR_OK;
}

HERROR	HLIB_ID3_GetFlacMusicInfo(const HCHAR *pszFileName, HxID3_MusicInfo_t *pstInfo)
{
	HERROR ret = ERR_FAIL;

	flac_context_t stFlacCtx;
	flac_metadata_block_t flac_meta;

	HxSTD_MemSet( &stFlacCtx, 0x00, sizeof(flac_context_t) );
	HxSTD_MemSet( &flac_meta, 0x00, sizeof(flac_metadata_block_t) );

	ret = hlib_flac_init( pszFileName, &stFlacCtx );
	if ( ERR_OK != ret )
	{
		HxLOG_Error("flac init()... Failed!!!!\n");
		hlib_flac_deinit( &stFlacCtx );
		return ERR_FAIL;
	}
	ret = hlib_flac_getMetaData( &stFlacCtx, &flac_meta, flac_metadata_type_vorbis_comment );
	if ( ERR_OK != ret )
	{
		HxLOG_Error("flac info() - getMetadata/vorbis_comment... Failed !!!!\n");
		hlib_flac_deinit( &stFlacCtx );
		return ERR_FAIL;
	}

	hlib_flac_convert_id3MusicInfo( (flac_vorbis_comment_t *)flac_meta.data, pstInfo );

	hlib_flac_release_metadata( &flac_meta );
	hlib_flac_deinit( &stFlacCtx );
	return ERR_OK;
}

/*********************** End of File ******************************/

