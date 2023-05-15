/**
	@file     hxpng.c
	@brief   libpng interface function file

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <hlib.h>
#include <assert.h>

#include <dlfcn.h>

#include "_hlib_stdlib.h"
#include "_hlib_mem.h"
#include "_hlib_log.h"

#include "_hlib_exif.h"

#include <libexif/exif-data.h>
#include <libexif/exif-content.h>
#include <libexif/exif-entry.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define hxexif_data_get_entry(exif, d,t)					\
	(((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_0],t) ?			\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_0],t) :			\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_1],t) ?			\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_1],t) :			\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_EXIF],t) ?		\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_EXIF],t) :		\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_GPS],t) ?		\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_GPS],t) :		\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_INTEROPERABILITY],t) ?	\
	 ((exif_init_t*)exif)->fn_exif_content_get_entry(d->ifd[EXIF_IFD_INTEROPERABILITY],t) : NULL)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	void		*pvExifLibHandle;
	HBOOL		bEnable;
	HUINT32		ulSemId;

	//	Symbol Link for LIBPNG Used function.
	ExifData*	(*fn_exif_data_new_from_file)	(const char *path);
	const char*	(*fn_exif_entry_get_value)		(ExifEntry *entry, char *val, unsigned int maxlen);
	void		(*fn_exif_data_unref)			(ExifData *data);
	ExifEntry*	(*fn_exif_content_get_entry)	(ExifContent *content, ExifTag tag);

} exif_init_t;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*******************************************************************************************
 * Static
 ******************************************************************************************/
static exif_init_t*	exif_getInstance(void)
{
	static exif_init_t	*s_pstExifInit = NULL;

	if (s_pstExifInit)
	{
		HxSEMT_Get(s_pstExifInit->ulSemId);
		if (s_pstExifInit->bEnable)
			return s_pstExifInit;
		HxSEMT_Release(s_pstExifInit->ulSemId);

		return NULL;
	} else
	{
		exif_init_t	*exif = HLIB_MEM_Calloc(sizeof(exif_init_t));

		s_pstExifInit = exif;

		HxSEMT_Create(&exif->ulSemId, "exifsem", 0);
		HxSEMT_Get(exif->ulSemId);

		exif->pvExifLibHandle = dlopen("/usr/lib/libexif.so", RTLD_LAZY);
		if (exif->pvExifLibHandle == NULL)
		{
			HxLOG_Critical("[%s](%d) can't find libexif.so in /usr/lib directory!!!\n");
			HxLOG_Assert(0);

			return NULL;
		}
		exif->fn_exif_data_new_from_file		= dlsym(exif->pvExifLibHandle, "exif_data_new_from_file");
		exif->fn_exif_entry_get_value			= dlsym(exif->pvExifLibHandle, "exif_entry_get_value");
		exif->fn_exif_data_unref				= dlsym(exif->pvExifLibHandle, "exif_data_unref");
		exif->fn_exif_content_get_entry			= dlsym(exif->pvExifLibHandle, "exif_content_get_entry");

		exif->bEnable = TRUE;

		s_pstExifInit = exif;
	}

	return s_pstExifInit;
}

#define	RETURN_IF_VOID(expr)		do{if(expr)return;}while(0)
#define	RETURN_IF_PTR(expr, ret)	do{if(expr)return ret;}while(0)

ExifData*	hxexif_open(exif_init_t *exif, const HUINT8 *filename)
{
	RETURN_IF_PTR(exif == NULL || filename == NULL, NULL);

	return (ExifData *)exif->fn_exif_data_new_from_file(filename);
}

void	hxexif_close(exif_init_t *exif, void*	pvExif)
{
	RETURN_IF_VOID(exif == NULL && pvExif == NULL);

	exif->fn_exif_data_unref((ExifData *)pvExif);
}

HUINT8*	hxexif_getinfostring(exif_init_t *exif, void* pvExif, HxEXIF_Entry_e eEntry, HUINT8 *pucBuf)
{
	return pucBuf;
}


int		hxexif_getdimension(exif_init_t *exif, ExifData *pstExif, HINT32 *out_width, HINT32 *out_height)
{
	ExifEntry	*pstEntry1 = NULL, *pstEntry2 = NULL;
	HUINT8		szStr1[64] = { 0, }, szStr2[64] = { 0, };

	RETURN_IF_PTR(exif == NULL || pstExif == NULL, ERR_FAIL);

    pstEntry1 = hxexif_data_get_entry(exif, pstExif, EXIF_TAG_PIXEL_X_DIMENSION);
	if (pstEntry1 != NULL)
	{
		exif->fn_exif_entry_get_value(pstEntry1, szStr1, sizeof(szStr1));
		*out_width = atoi(szStr1);

		pstEntry2 = hxexif_data_get_entry(exif, pstExif, EXIF_TAG_PIXEL_Y_DIMENSION);
		if (pstEntry2 != NULL)
		{
			exif->fn_exif_entry_get_value(pstEntry2, szStr2, sizeof(szStr2));
			*out_height = atoi(szStr2);
		}
		else
		{
	    	HxLOG_Debug("no EXIF entry! (entry=%d) \n", eHxEXIF_Dimensions);
			return ERR_FAIL;
	    }
	}
	else
	{
    	HxLOG_Debug("no EXIF entry! (entry=%d) \n", eHxEXIF_Dimensions);
        return ERR_FAIL;
    }

	return ERR_OK;
}

int		hxexif_getorientation(exif_init_t *exif, ExifData *pstExif, HUINT32 *out_orientation)
{
	ExifEntry	*pstEntry1 = NULL;
	HUINT8		szStr[64] = { 0, };
	int			orientation = 0;

	RETURN_IF_PTR(exif == NULL || pstExif == NULL, 0);

    pstEntry1 = hxexif_data_get_entry(exif, pstExif, EXIF_TAG_ORIENTATION);
	if (pstEntry1 != NULL)
	{
		exif->fn_exif_entry_get_value(pstEntry1, szStr, sizeof(szStr));
		orientation = atoi(szStr);

	}
	else
	{
    	HxLOG_Debug("no EXIF entry! (entry=%d) \n", eHxEXIF_Orientation);
        return 0;
    }

	return orientation;
}

int		hxexif_getmake(exif_init_t *exif, ExifData *pstExif, HCHAR *out_string, HUINT32 ulOutStrLen)
{
	ExifEntry	*pstEntry1 = NULL;

	RETURN_IF_PTR(exif == NULL || pstExif == NULL, 0);

    pstEntry1 = hxexif_data_get_entry(exif, pstExif, EXIF_TAG_MAKE);
	if (pstEntry1 != NULL)
	{
		exif->fn_exif_entry_get_value(pstEntry1, out_string, ulOutStrLen);
	}
	else
	{
    	HxLOG_Debug("no EXIF entry! (entry=%d) \n", eHxEXIF_Orientation);
        return ERR_FAIL;
    }

	return ERR_OK;
}

int		hxexif_getmodel(exif_init_t *exif, ExifData *pstExif, HCHAR *out_string, HUINT32 ulOutStrLen)
{
	ExifEntry	*pstEntry1 = NULL;

	RETURN_IF_PTR(exif == NULL || pstExif == NULL, ERR_FAIL);

    pstEntry1 = hxexif_data_get_entry(exif, pstExif, EXIF_TAG_MODEL);
	if (pstEntry1 != NULL)
	{
		exif->fn_exif_entry_get_value(pstEntry1, out_string, ulOutStrLen);
	}
	else
	{
    	HxLOG_Debug("no EXIF entry! (entry=%d) \n", eHxEXIF_Orientation);
        return ERR_FAIL;
    }

	return ERR_OK;
}

int		hxexif_getphotodate(exif_init_t *exif, ExifData *pstExif, HCHAR *out_string, HUINT32 ulOutStrLen)
{
	ExifEntry	*pstEntry1 = NULL;

	RETURN_IF_PTR(exif == NULL || pstExif == NULL, ERR_FAIL);

    pstEntry1 = hxexif_data_get_entry(exif, pstExif, EXIF_TAG_DATE_TIME);
	if (pstEntry1 != NULL)
	{
		exif->fn_exif_entry_get_value(pstEntry1, out_string, ulOutStrLen);
	}
	else
	{
    	HxLOG_Debug("no EXIF entry! (entry=%d) \n", eHxEXIF_Orientation);
        return ERR_FAIL;
    }

	return ERR_OK;
}

/*******************************************************************************************
 * public
 ******************************************************************************************/

HERROR	HLIB_EXIF_GetFileInfo(const HCHAR *pszFileName, HxEXIF_Info_t *pstInfo)
{
	ExifData	*pvFile;
	exif_init_t		*exif = exif_getInstance();

	if (exif == NULL)
		return ERR_FAIL;

	if (HxSTD_StrEmpty(pszFileName) || pstInfo == NULL)
	{
		HxLOG_Error("source is null [%s]\n", pszFileName);
		HxSEMT_Release(exif->ulSemId);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstInfo, 0, sizeof(HxEXIF_Info_t));
	pvFile = hxexif_open(exif, (HCHAR*)pszFileName);
	if (pvFile)
	{
		hxexif_getdimension(exif, pvFile, &pstInfo->nWidth, &pstInfo->nHeight);
		hxexif_getphotodate(exif, pvFile, pstInfo->szDate, 32);
		hxexif_close(exif, pvFile);

		HxSEMT_Release(exif->ulSemId);

		return ERR_OK;
	}

	HxSEMT_Release(exif->ulSemId);
	return ERR_FAIL;
}

HUINT8*	HLIB_EXIF_GetThumbnail(const HCHAR *pszFileName, HINT32 *pnSize)
{
	ExifData	*pstExif = NULL;
	HUINT8		*pucBuf  = NULL;
	exif_init_t	*exif = exif_getInstance();

	if (exif == NULL)
		return NULL;

	pstExif = hxexif_open(exif, pszFileName);

	if ((pstExif == NULL) || (pnSize == NULL))
	{
		HxLOG_Print("invalid parameter! \n");
		HxSEMT_Release(exif->ulSemId);
		return NULL;
	}

	if ((pstExif->data != NULL) &&
		(pstExif->data[0] == 0xFF) &&
		(pstExif->data[1] == 0xD8))		/* 0xFFD8: JPEG SOI */
	{
		pucBuf = (HUINT8 *)HLIB_MEM_Malloc(pstExif->size);

		if (pucBuf != NULL)
		{
			HxSTD_MemCopy(pucBuf, pstExif->data, pstExif->size);
			*pnSize = pstExif->size;
		}
		else
		{
			HxLOG_Debug("alloc failed! \n");
		}
	}
	else
	{
		HxLOG_Debug("no EXIF thumbnail! \n");
	}

	hxexif_close(exif, pstExif);
	HxSEMT_Release(exif->ulSemId);

	return pucBuf;
}

void	HLIB_EXIF_FreeBuffer(HUINT8 *pucBuffer)
{
	HLIB_MEM_Free(pucBuffer);
}



