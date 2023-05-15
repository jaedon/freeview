
#include <hlib.h>
#include <dirent.h>
#include "_dlib_port_fileinfo.h"

typedef	struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxFileInfo_Field_t;

static HBOOL	dlib_fileinfo_getType (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*finfo = (DxFileInfo_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = finfo->eDataType;

	return TRUE;
}

static HBOOL	dlib_fileinfo_getPath (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*finfo = (DxFileInfo_t*)src;
	HCHAR	*pszTempStr, *p;

	HxLOG_Assert(src && size);

	pszTempStr = DLIB_StrDup(finfo->szFileName);
	p = HxSTD_StrRChar(pszTempStr, '/');
	if (p == NULL)
	{
		HxLOG_Warning("File extension is not valid!!! [%s]\n", finfo->szFileName);
		field->u.string = DLIB_StrDup("");
		DLIB_Free(pszTempStr);
	} else
	{
		*p = 0;
		field->u.string = pszTempStr;
	}

	return TRUE;
}

static HBOOL	dlib_fileinfo_getOneDepthPath (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*finfo = (DxFileInfo_t*)src;

	HxLOG_Assert(src && size);

	field->u.string = DLIB_StrDup(finfo->szOnedepth);

	return TRUE;
}

static HBOOL	dlib_fileinfo_getName (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*finfo = (DxFileInfo_t*)src;
	HCHAR	*pszTempStr, *pszFileName;

	HxLOG_Assert(src && size);

	pszTempStr = (HCHAR*)DLIB_Calloc(DLIB_FILEINFO_NAME_LEN);
	pszFileName = HxSTD_StrRChar(finfo->szFileName, '/');
	if (pszFileName)
		HxSTD_StrNCpy(pszTempStr, pszFileName + 1, DLIB_FILEINFO_NAME_LEN - 1);
	else
		HxSTD_StrNCpy(pszTempStr, finfo->szFileName, DLIB_FILEINFO_NAME_LEN - 1);
	field->u.string = pszTempStr;

	return TRUE;
}

static HBOOL	dlib_fileinfo_getSize (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*finfo = (DxFileInfo_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = finfo->size;

	return TRUE;
}

static HBOOL	dlib_fileinfo_getExt (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*finfo = (DxFileInfo_t*)src;

	HxLOG_Assert(src && size);
	field->u.string = DLIB_StrDup(HLIB_PATH_Extension(finfo->szFileName));
	if (field->u.string)
		HLIB_STD_StrLower(field->u.string);

	return TRUE;
}


static HBOOL	dlib_fileinfo_getInterface (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*finfo = (DxFileInfo_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = finfo->eInterface;
	return TRUE;
}

/* eDataType ¸¶´Ù ´Þ¸® °¡Áö´Â Á¤º¸¸¦ ÀÎµ¦½Ì ÇÏ±âÀ§ÇÑ ÇÊµå*/
static HBOOL	dlib_fileinfo_getKeyword1 (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*pstFinfo = (DxFileInfo_t*)src;
	HxLOG_Assert(src && size);

	if (pstFinfo->eDataType == eDxFILETYPE_MUSIC)
	{
		//2 Album
		if(pstFinfo->size != 0)
		{
			HxID3_MusicInfo_t *pstMusicInfo = (HxID3_MusicInfo_t*)pstFinfo->data;
			HCHAR		szAlbumKey[HxID3_FIELD_SIZE_LONG+36] = {0,};

			if (HxSTD_StrLen(pstMusicInfo->szAlbum) > 0)
			{
				HxSTD_snprintf(szAlbumKey, sizeof(szAlbumKey)-1, "Album=%s",pstMusicInfo->szAlbum);
			}
			else
			{
				HxSTD_snprintf(szAlbumKey, sizeof(szAlbumKey)-1, "Album");
			}

			HxLOG_Debug("szAlbumKey : %s, length : %d\n", szAlbumKey, HxSTD_StrLen(szAlbumKey));
			field->u.string = DLIB_StrDup(szAlbumKey);
		}
	}
	else
	{
		// type¿¡ µû¶ó ÇÊ¿äÇÑ keyword Ãß°¡
	}

	return TRUE;

}

/* eDataType ¸¶´Ù ´Þ¸® °¡Áö´Â Á¤º¸¸¦ ÀÎµ¦½Ì ÇÏ±âÀ§ÇÑ ÇÊµå*/
static HBOOL	dlib_fileinfo_getKeyword2 (DxField_t *field, const void *src, HUINT32 size)
{
	const DxFileInfo_t	*pstFinfo = (DxFileInfo_t*)src;
	HxLOG_Assert(src && size);

	if (pstFinfo->eDataType == eDxFILETYPE_MUSIC)
	{
		if(pstFinfo->size != 0)
		{
			//2 Artist
			HxID3_MusicInfo_t *pstMusicInfo = (HxID3_MusicInfo_t*)pstFinfo->data;
			HCHAR		szArtistKey[HxID3_FIELD_SIZE_LONG+36] = {0,};

			if (HxSTD_StrLen(pstMusicInfo->szArtist) > 0 )
			{
				HxSTD_snprintf(szArtistKey, sizeof(szArtistKey)-1, "Artist=%s",pstMusicInfo->szArtist);
			}
			else
			{
				HxSTD_snprintf(szArtistKey, sizeof(szArtistKey)-1, "Artist");
			}
			HxLOG_Debug("szArtistKey : %s, length : %d\n", szArtistKey, HxSTD_StrLen(szArtistKey));
			field->u.string = DLIB_StrDup(szArtistKey);
		}
	}
	else
	{
		// type¿¡ µû¶ó ÇÊ¿äÇÑ keyword Ãß°¡
	}

	return TRUE;
}
/* eDataType ¸¶´Ù ´Þ¸® °¡Áö´Â Á¤º¸¸¦ ÀÎµ¦½Ì ÇÏ±âÀ§ÇÑ ÇÊµå*/
static HBOOL	dlib_fileinfo_getKeyword3 (DxField_t *field, const void *src, HUINT32 size)
{
//	const DxFileInfo_t	*pstFinfo = (DxFileInfo_t*)src;
//	HxLOG_Assert(src && size);

	// type¿¡ µû¶ó ÇÊ¿äÇÑ keyword Ãß°¡

	return TRUE;
}

#if defined(CONFIG_FILEINFO_IFACE_NO_RECORDINGS)
static HERROR	dlib_fileinfo_Encoder (const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HUINT32				 ulDstLength;
	HUINT32				 ulStringLen;
	DxFileInfo_t		*pstSrcFile = (DxFileInfo_t *)src;
	HUINT8				*pucDstBuf = NULL, *pucBuf;

	// Length °è»ê
	ulDstLength  = 0;
	ulDstLength += sizeof(HUINT64) + sizeof(DxStorageInterface_e) + sizeof(DxFileState_t);
	ulDstLength += sizeof(DxFileType_e);

	// szOnedepth Length
	ulDstLength += sizeof(HUINT32);				// szOnedepth length
	ulStringLen = HxSTD_StrLen(pstSrcFile->szOnedepth);
	ulStringLen = (ulStringLen > 0) ? (((ulStringLen - 1) >> 2) + 1) << 2 : 0;
	ulDstLength += ulStringLen;

	// szFileName Length
	ulDstLength += sizeof(HUINT32);				// szFileName length
	ulStringLen = HxSTD_StrLen(pstSrcFile->szFileName);
	ulStringLen = (ulStringLen > 0) ? (((ulStringLen - 1) >> 2) + 1) << 2 : 0;
	ulDstLength += ulStringLen;

	ulDstLength += sizeof(HINT32);				// size
	ulDstLength += pstSrcFile->size;			// data

	pucDstBuf = (HUINT8 *)DLIB_Calloc(ulDstLength);
	if (NULL == pucDstBuf)
	{
		return ERR_FAIL;
	}

	pucBuf = pucDstBuf;

	HxSTD_MemCopy(pucBuf, &(pstSrcFile->uid), sizeof(HUINT64));
	pucBuf += sizeof(HUINT64);

	HxSTD_MemCopy(pucBuf, &(pstSrcFile->eInterface), sizeof(DxStorageInterface_e));
	pucBuf += sizeof(DxStorageInterface_e);

	HxSTD_MemCopy(pucBuf, &(pstSrcFile->st_stat), sizeof(DxFileState_t));
	pucBuf += sizeof(DxFileState_t);

	HxSTD_MemCopy(pucBuf, &(pstSrcFile->eDataType), sizeof(DxFileType_e));
	pucBuf += sizeof(DxFileType_e);

	ulStringLen = HxSTD_StrLen(pstSrcFile->szOnedepth);
	ulStringLen = (ulStringLen > 0) ? (((ulStringLen - 1) >> 2) + 1) << 2 : 0;
	HxSTD_MemCopy(pucBuf, &ulStringLen, sizeof(HUINT32));
	pucBuf += sizeof(HUINT32);

	if (ulStringLen > 0)
	{
		HxSTD_MemCopy(pucBuf, pstSrcFile->szOnedepth, ulStringLen);
		pucBuf += ulStringLen;
	}

	ulStringLen = HxSTD_StrLen(pstSrcFile->szFileName);
	ulStringLen = (ulStringLen > 0) ? (((ulStringLen - 1) >> 2) + 1) << 2 : 0;
	HxSTD_MemCopy(pucBuf, &ulStringLen, sizeof(HUINT32));
	pucBuf += sizeof(HUINT32);

	if (ulStringLen > 0)
	{
		HxSTD_MemCopy(pucBuf, pstSrcFile->szFileName, ulStringLen);
		pucBuf += ulStringLen;
	}

	HxSTD_MemCopy(pucBuf, &(pstSrcFile->size), sizeof(HINT32));
	pucBuf += sizeof(HINT32);

	if (pstSrcFile->size > 0)
	{
		HxSTD_MemCopy(pucBuf, pstSrcFile->data, pstSrcFile->size);
		pucBuf += pstSrcFile->size;
	}

	*dst = (void *)pucDstBuf;
	*dstSize = ulDstLength;

	return ERR_OK;
}

static HERROR	dlib_fileinfo_Decoder (const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HUINT32			 ulDstSize = 0, ulStringLen;
	HUINT8			*pucSrcBuf = (HUINT8 *)src;
	DxFileInfo_t	 stTempFileInfo;
	DxFileInfo_t	*pstDstFileInfo = NULL;

	HxSTD_MemSet(&stTempFileInfo, 0, sizeof(DxFileInfo_t));

	HxSTD_MemCopy(&(stTempFileInfo.uid), pucSrcBuf, sizeof(HUINT64));
	pucSrcBuf += sizeof(HUINT64);

	HxSTD_MemCopy(&(stTempFileInfo.eInterface), pucSrcBuf, sizeof(DxStorageInterface_e));
	pucSrcBuf += sizeof(DxStorageInterface_e);

	HxSTD_MemCopy(&(stTempFileInfo.st_stat), pucSrcBuf, sizeof(DxFileState_t));
	pucSrcBuf += sizeof(DxFileState_t);

	HxSTD_MemCopy(&(stTempFileInfo.eDataType), pucSrcBuf, sizeof(DxFileType_e));
	pucSrcBuf += sizeof(DxFileType_e);

	HxSTD_MemCopy(&ulStringLen, pucSrcBuf, sizeof(HUINT32));
	pucSrcBuf += sizeof(HUINT32);

	if (ulStringLen > 0)
	{
		HxSTD_MemCopy(stTempFileInfo.szOnedepth, pucSrcBuf, ulStringLen);
		pucSrcBuf += ulStringLen;
	}

	HxSTD_MemCopy(&ulStringLen, pucSrcBuf, sizeof(HUINT32));
	pucSrcBuf += sizeof(HUINT32);

	if (ulStringLen > 0)
	{
		HxSTD_MemCopy(stTempFileInfo.szFileName, pucSrcBuf, ulStringLen);
		pucSrcBuf += ulStringLen;
	}

	HxSTD_MemCopy(&(stTempFileInfo.size), pucSrcBuf, sizeof(HINT32));
	pucSrcBuf += sizeof(HINT32);

	ulDstSize = (stTempFileInfo.size > 0) ? sizeof(DxFileInfo_t) + stTempFileInfo.size : sizeof(DxFileInfo_t);
	pstDstFileInfo = (DxFileInfo_t *)DLIB_Malloc(ulDstSize);
	if (NULL == pstDstFileInfo)
	{
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstDstFileInfo, &stTempFileInfo, sizeof(DxFileInfo_t));
	HxSTD_MemCopy(pstDstFileInfo->data, pucSrcBuf, pstDstFileInfo->size);

	*dst = pstDstFileInfo;
	*dstSize = ulDstSize;

	return ERR_OK;
}
#else
static HERROR	dlib_fileinfo_Encoder (const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
#if defined(CONFIG_DIET_DATATYPE)
{
	return ERR_FAIL;
}
#else
{
	return ERR_FAIL;
}
#endif
}

static HERROR	dlib_fileinfo_Decoder (const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
#if defined(CONFIG_DIET_DATATYPE)
{
	return ERR_FAIL;
}
#else
{
	return ERR_FAIL;
}
#endif
}
#endif

static HERROR	dlib_fileinfo_Serialize (const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
#if defined(CONFIG_DIET_DATATYPE)
{
	return ERR_FAIL;
}
#else
{
	return ERR_FAIL;
}
#endif
}

static HERROR	dlib_fileinfo_Deserialize (const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
#if defined(CONFIG_DIET_DATATYPE)
{
	return ERR_FAIL;
}
#else
{
	return ERR_FAIL;
}
#endif
}


static const DxFileInfo_Field_t	s_FILEINFO_FIELDS[] =
{
	  {{eDxTYPE_INTEGER, "type"      , FALSE, {0}}, dlib_fileinfo_getType}
	, {{eDxTYPE_STRING , "ext"       , TRUE,  {0}}, dlib_fileinfo_getExt}

	, {{eDxTYPE_STRING , "path"      , TRUE,  {0}}, dlib_fileinfo_getPath}
	, {{eDxTYPE_STRING , "onedepth"  , FALSE, {0}}, dlib_fileinfo_getOneDepthPath}
	, {{eDxTYPE_STRING , "name"      , FALSE, {0}}, dlib_fileinfo_getName}
	, {{eDxTYPE_INTEGER, "size"      , FALSE, {0}}, dlib_fileinfo_getSize}

	, {{eDxTYPE_INTEGER, "interface" , FALSE, {0}}, dlib_fileinfo_getInterface}

	, {{eDxTYPE_STRING, "keyword1" , TRUE, {0}}, dlib_fileinfo_getKeyword1}
	, {{eDxTYPE_STRING, "keyword2" , TRUE, {0}}, dlib_fileinfo_getKeyword2}
	, {{eDxTYPE_STRING, "keyword3" , TRUE, {0}}, dlib_fileinfo_getKeyword3}
};

const HCHAR *	DLIB_FILEINFO_GetDBFullName (HINT32 dupIndex)
{
	return ":memory:";
}


HINT32			DLIB_FILEINFO_GetFieldCount (void)
{
	return sizeof(s_FILEINFO_FIELDS) / sizeof(s_FILEINFO_FIELDS[0]);
}

DxField_t *		DLIB_FILEINFO_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_FILEINFO_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_FILEINFO_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_FILEINFO_FIELDS[index].getData)
	{
		if (!s_FILEINFO_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_FILEINFO_FIELDS[index].field.name);
			return NULL;
		}
	}

	// Â¸ÃžÂ¸Ã°Â¸Â® Ã‡Ã”Â¼Ã¶Â´Ã‚ HxSTD_MemXXXÂ¸Â¦ Â»Ã§Â¿Ã«Ã‡Ã‘Â´Ã™.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

HERROR			DLIB_FILEINFO_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_fileinfo_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_FILEINFO_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_fileinfo_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_FILEINFO_Serialize (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_fileinfo_Serialize(src, srcSize, dst, dstSize);
}

HERROR			DLIB_FILEINFO_Deserialize (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_fileinfo_Deserialize(src, srcSize, dst, dstSize);
}



HERROR	DLIB_FILEINFO_ConvertStatToDxStat(const HxDIR_Stat_t *fstat, DxFileState_t *pstDxStat)
{
//	struct tm *now;

	if(S_ISREG(fstat->st_mode)) pstDxStat->st_mode 	= eDxISREG;
	if(S_ISDIR(fstat->st_mode)) pstDxStat->st_mode 	= eDxISDIR;
	if(S_ISCHR(fstat->st_mode)) pstDxStat->st_mode 	= eDxISCHR;
	if(S_ISBLK(fstat->st_mode)) pstDxStat->st_mode 	= eDxISBLK;
	if(S_ISFIFO(fstat->st_mode)) pstDxStat->st_mode 	= eDxISFIFO;
	if(S_ISLNK(fstat->st_mode)) pstDxStat->st_mode 	= eDxISLNK;
	if(S_ISSOCK(fstat->st_mode)) pstDxStat->st_mode 	= eDxISSOCK;

	pstDxStat->st_size	= fstat->st_size;

	pstDxStat->ullMTime = (HUINT64)fstat->st_mtime;
	pstDxStat->ullATime = (HUINT64)fstat->st_atime;
	pstDxStat->ullCTime = (HUINT64)fstat->st_ctime;

	return ERR_OK;
}

DxFileState_t *DILB_FILEINFO_Stat_StructConvert(const HCHAR* pszPath)
{
	DxFileState_t *pstDxFileInfo;
//	struct tm *now;
	HxDIR_Stat_t stOrignal;

	if(stat(pszPath,&stOrignal) != 0 ) return NULL;

	pstDxFileInfo = (DxFileState_t*)DLIB_Malloc(sizeof(DxFileState_t));

	if(S_ISREG(stOrignal.st_mode)) pstDxFileInfo->st_mode 	= eDxISREG;
	if(S_ISDIR(stOrignal.st_mode)) pstDxFileInfo->st_mode 	= eDxISDIR;
	if(S_ISCHR(stOrignal.st_mode)) pstDxFileInfo->st_mode 	= eDxISCHR;
	if(S_ISBLK(stOrignal.st_mode)) pstDxFileInfo->st_mode 	= eDxISBLK;
	if(S_ISFIFO(stOrignal.st_mode)) pstDxFileInfo->st_mode 	= eDxISFIFO;
	if(S_ISLNK(stOrignal.st_mode)) pstDxFileInfo->st_mode 	= eDxISLNK;
	if(S_ISSOCK(stOrignal.st_mode)) pstDxFileInfo->st_mode 	= eDxISSOCK;

	pstDxFileInfo->st_size	= stOrignal.st_size;

	pstDxFileInfo->ullMTime = (HUINT64)stOrignal.st_mtime;
	pstDxFileInfo->ullATime = (HUINT64)stOrignal.st_atime;
	pstDxFileInfo->ullCTime = (HUINT64)stOrignal.st_ctime;

	return pstDxFileInfo;
}



void			DLIB_FILEINFO_Print (FILE *fp, const void *t)
{
#if	0
	fprintf(fp, "FILEINFO[%d] id:%d %s\n"
		, t->uid
		, t->id
		, t->name
	);
	DxLIB_PRINT_INT(t->version);

#ifdef	CONFIG_OP_FREESAT
	{
		const DxFreesatGroup_t *freesat = &(t->ex.freesat);

		DxLIB_PRINT_HEX(freesat->ucGroupType);
		DxLIB_PRINT_HEX(freesat->ucNondestructiveTuneFlag);
		DxLIB_PRINT_HEX(freesat->ucReturnChannelAccessFlag);
		DxLIB_PRINT_STR(freesat->szIso639LangCode);
		DxLIB_PRINT_HEX(freesat->ucG2ExtensionFlag);
		DxLIB_PRINT_HEX(freesat->ucDefaultVisableFlag);
		DxLIB_PRINT_HEX(freesat->ucAdultChannelsGroupFlag);
		DxLIB_PRINT_HEX(freesat->ucIpChannelsGroupFlag);
		DxLIB_PRINT_HEX(freesat->ucCaChannelsGroupFlag);
	}
#endif
#endif
}


