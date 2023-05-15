#include "va_types.h"
#include "va_errors.h"
#include "va_fs.h"
#include <octo_common.h>

#define CAS_DIR_PATH "/var/lib/humaxtv/cas/"

#define FS_MAX_EXT_LEN						5

/* directory masks */
typedef enum
{
	eSVC_FS_DIRMASK_VIDEO		= 0x00000001,
	eSVC_FS_DIRMASK_PLAYLIST		= 0x00000002,
	eSVC_FS_DIRMASK_MOVIE		= 0x00000010,
	eSVC_FS_DIRMASK_MUSIC		= 0x00000020,
	eSVC_FS_DIRMASK_IMAGE		= 0x00000040,

	eSVC_FS_DIRMASK_FILE			= 0x0000FFFF,

	eSVC_FS_DIRMASK_DIR			= 0xFFFF0000,

	eSVC_FS_DIRMASK_ALL			= 0xFFFFFFFF
} xsvcCas_VaDirMask_e;

/** @brief file types than can handle */
typedef enum
{
	ePVR_FS_EXT_VIDEO,
	ePVR_FS_EXT_PLAYLIST,
	ePVR_FS_EXT_MOVIE,
	ePVR_FS_EXT_MUSIC,
	ePVR_FS_EXT_IMAGE,
	ePVR_FS_EXT_MAX
} xsvcCas_VaFsExtType_e;

/** @brief file attribute */
typedef enum
{
	eFS_FileAttr_FILE,
	eFS_FileAttr_DIR,
	eFS_FileAttr_PLAYLIST,
	eFS_FileAttr_VIDEO,
	eFS_FileAttr_MOVIE,
	eFS_FileAttr_MUSIC,
	eFS_FileAttr_IMAGE,
	eFS_FileAttr_MAX
} xsvcCas_VaFsFileAttr_e;

typedef struct
{
	HUINT8				file_name[FS_MAX_FILE_NAME_LEN + 1];
	HUINT8				attrib;				// SVC_FS_FILETYPE_T 사용
	HUINT64				size;
	UNIXTIME			access_time;
	UNIXTIME			modify_time;
	UNIXTIME			create_time;
} xsvcCas_VaFsFileStat_t;

typedef struct
{
	HUINT32					num_files;
	xsvcCas_VaFsFileStat_t	*file_stat;
} xsvcCas_VaFsDirStat_t;

typedef struct
{
	HUINT8	ucExtType;
	HUINT8	ucExtLen;
	HUINT8	*szExtName;
} xsvcCas_VaFsPvrExt_t;

STATIC xsvcCas_VaFsPvrExt_t	s_astVaFs_MediaExtList[] =
{
	{ ePVR_FS_EXT_VIDEO, 		3, 	".TS" },
	{ ePVR_FS_EXT_PLAYLIST, 	4,  ".HPL" },
	{ ePVR_FS_EXT_MOVIE, 		4,  ".AVI" },
	{ ePVR_FS_EXT_MOVIE, 		5,  ".DIVX" },
	{ ePVR_FS_EXT_MOVIE, 		4,  ".SMI" },
	{ ePVR_FS_EXT_MOVIE, 		4,  ".SSA" },
	{ ePVR_FS_EXT_MUSIC, 		4,  ".MP3" },
	{ ePVR_FS_EXT_MUSIC, 		4,  ".MP2" },
	{ ePVR_FS_EXT_MUSIC, 		4,  ".MP1" },
	{ ePVR_FS_EXT_IMAGE, 		4, 	".JPG" },
	{ ePVR_FS_EXT_IMAGE, 		4, 	".JPE" },
	{ ePVR_FS_EXT_IMAGE, 		5, 	".JPEG" },
	{ ePVR_FS_EXT_IMAGE, 		5, 	".JFIF" },
};

STATIC HUINT8		s_ulVaFs_NumMediaExt = sizeof(s_astVaFs_MediaExtList) / sizeof(s_astVaFs_MediaExtList[0]);

typedef int (*compFuncPointer)(const void *, const void *);

static int pal_fs_selector(const struct dirent *dirent, const char *filterstr, unsigned int filtertype)
{
	if (0 == strcmp(dirent->d_name, "lost+found"))
		return 0;

	/* FIXME : How shall I do on dot or dotdot hidden directories */
#if 0
	if ('.' == dirent->d_name[0] && (filtertype != PAL_FS_INFO_ALLLIST))
		return 0;
#endif

	if ((NULL == filterstr) ||
	    (filterstr && NULL != strstr(dirent->d_name, filterstr)))
		return 1;
	else
		return 0;
}


static int pal_fs_Dirscan(const char *dirname, struct dirent ***namelist,
	    const char *filterstr, unsigned int filtertype, int(*compar)(const void *, const void *))
{
	DIR *dp = NULL;
	struct dirent *dirent, **direntp = NULL;
	size_t entrysize;
	int pos = 0;
	int ulDirentSize = 0;

	if (dp = opendir(dirname), NULL == dp)
		return -1;

	while ((dirent = readdir(dp)) != NULL)
	{
		if (pal_fs_selector(dirent, filterstr, filtertype)) {
			if (pos >= ulDirentSize)
			{
				struct dirent **new_direntp = NULL;

				new_direntp = OxFS_Malloc ((pos + 1024) * sizeof(struct dirent *));
				if (NULL == new_direntp)
				{
					goto err_free_list;
				}

				if (NULL != direntp)
				{
					HxSTD_memcpy (new_direntp, direntp, pos * sizeof(struct dirent *));
					OxFS_Free (direntp);
				}

				direntp = new_direntp;
				ulDirentSize = pos + 1024;
			}

			entrysize = offsetof(struct dirent, d_name) + strlen(dirent->d_name) + 1;
			if (direntp[pos]= OxFS_Malloc(entrysize),
			    NULL == direntp[pos])
				goto err_free_list;

			memcpy(direntp[pos], dirent, entrysize);
			pos++;
		}
	}

	closedir(dp);

	if (compar && direntp)
		qsort(direntp, (size_t)pos, sizeof(struct dirent *), compar);

	*namelist = direntp;
	return pos;

err_free_list:
	closedir(dp);
	if (direntp) {
		while (pos > 0)
			OxFS_Free(direntp[--pos]);
		OxFS_Free(direntp);
	}
	*namelist = NULL;
	return -1;
}

static int pal_fs_Dirfree(int pos, struct dirent **namelist)
{
	if (namelist) {
		while (pos > 0)
			OxFS_Free(namelist[--pos]);
		OxFS_Free(namelist);
	}
	return 0;
}

int pal_fs_GetFileInfo(const char *pathname, HUINT32 type, xsvcCas_FileStat_t *filestatp)
{
	struct stat st;

	if (-1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	switch (st.st_mode & S_IFMT) {
	case S_IFREG:
		filestatp->fileType = PAL_FS_FILE_TYPE_FILE;
		break;
	case S_IFDIR:
		filestatp->fileType = PAL_FS_FILE_TYPE_DIR;
		break;
	case S_IFIFO:
		filestatp->fileType = PAL_FS_FILE_TYPE_FIFO;
		break;
	case S_IFLNK:
		filestatp->fileType = PAL_FS_FILE_TYPE_LINK;
		break;
	default:
		filestatp->fileType = PAL_FS_FILE_TYPE_UNKNOWN;
		break;
	}

	/*
	 * when the named file is symbolic link,
	 * return size about the symbolic link,
	 * not the file referenced by the symbolic link.
	 */
	filestatp->ulFileSize = st.st_size;

	filestatp->aTime = st.st_atime;		/* time of last access */
	filestatp->mTime = st.st_mtime;		/* time of last modification */
	filestatp->cTime = st.st_ctime;		/* time of last change */

	/* FIXME PAL_FS_INFO_XXX and FM_FS_LISTTYPE_XXX are not value
	 * for bitwise operation */
	if (PAL_FS_INFO_TIME == type) {
		struct timeval tv;
		struct timezone tz;
		int h, m, c;
		gettimeofday(&tv, &tz);
		h = tz.tz_minuteswest / 60;
		m = ((tz.tz_minuteswest / 15) % 4) * 15;
		c = ((60 * 60) * h) + (60 * m);

		filestatp->aTime = st.st_atime + c;
		filestatp->mTime = st.st_mtime + c;
		filestatp->cTime = st.st_ctime + c;
	}

	return ERR_OK;
}

int va_fs_op_GetDirInfo(const char *dirname, HUINT8 *filterstr, HUINT32 filtertype,PAL_FS_DirStat_t *dirstatp)
{
	xsvcCas_FileStat_t *filestat, *p;
	struct dirent **direntp;
	char fullpath[PATH_MAX];
	int n;
	int i;

	if (n = pal_fs_Dirscan(dirname, &direntp, (char *)filterstr, filtertype, (compFuncPointer)alphasort), n < 0)
		return ERR_FAIL;

	if (0 == n) {
		dirstatp->fileNumber = 0;
		dirstatp->s_astfilestats = NULL;
		return ERR_OK;
	}

	if (filestat = OxFS_Malloc(sizeof(*filestat) * n), NULL == filestat) {
		pal_fs_Dirfree(n, direntp);
		return PAL_FS_ERR_OUTOFMEMORY;
	}

	for (i = 0, p = filestat; i < n; i++, p++) {
		strncpy((char *)p->aucFileName, direntp[i]->d_name, NAME_MAX-1);
		snprintf(fullpath, PATH_MAX, "%s/%s", dirname, direntp[i]->d_name);
		pal_fs_GetFileInfo(fullpath, filtertype, p);
	}

	dirstatp->fileNumber = n;
	dirstatp->s_astfilestats = filestat;
	pal_fs_Dirfree(n, direntp);
	return ERR_OK;
}

int va_fs_op_GetDirInfoFree(PAL_FS_DirStat_t *dirstatp)
{
	if (dirstatp && dirstatp->s_astfilestats)
		OxFS_Free(dirstatp->s_astfilestats);
	return ERR_OK;
}

INLINE STATIC HERROR va_fs_ConvertMwDirMaskToPalDirMask (xsvcCas_VaDirMask_e eMwDirMask, HUINT32 *pulPalDirMask)
{
	if (pulPalDirMask == NULL)					{ return ERR_FAIL; }

	*pulPalDirMask = (HUINT32)eMwDirMask;
	return ERR_OK;
}

STATIC HBOOL va_fs_IsMatchedFile (HUINT32 ulPalFileMask, PAL_FS_FileStat_t *file_stat)
{
	HBOOL		bIsMatched = FALSE;
	HUINT8		ucIdx, ucExtLen = 0, aucExtName[FS_MAX_EXT_LEN];

	switch (ulPalFileMask)
	{
		case PAL_FS_DIRINFO_ALL:
			if ((file_stat->fileType == PAL_FS_FILE_TYPE_FILE) || (file_stat->fileType == PAL_FS_FILE_TYPE_DIR))
			{
				bIsMatched = TRUE;
			}
			break;

		case PAL_FS_DIRINFO_FILE:
			if (file_stat->fileType == PAL_FS_FILE_TYPE_FILE)
			{
				bIsMatched = TRUE;
			}
			break;

		case PAL_FS_DIRINFO_DIR:
			if (file_stat->fileType == PAL_FS_FILE_TYPE_DIR)
			{
				bIsMatched = TRUE;
			}
			break;

		default:
		{
			if ((ulPalFileMask & PAL_FS_DIRINFO_DIR) && (file_stat->fileType == PAL_FS_FILE_TYPE_DIR))
			{
				bIsMatched = TRUE;
			}
			else if (file_stat->fileType == PAL_FS_FILE_TYPE_FILE)
			{
				memset(aucExtName, 0, FS_MAX_EXT_LEN);
				local_mwcfs_GetExtNameFromFileName(file_stat->aucFileName, aucExtName, &ucExtLen);
				if (ucExtLen != 0)
				{
					if (!bIsMatched && (ulPalFileMask & PAL_FS_DIRINFO_VIDEO))
					{
						for (ucIdx = 0; ucIdx < s_ulVaFs_NumMediaExt; ucIdx++)
						{
							if ((s_astVaFs_MediaExtList[ucIdx].ucExtType == ePVR_FS_EXT_VIDEO) && (ucExtLen == s_astVaFs_MediaExtList[ucIdx].ucExtLen))
							{
								if (!MWC_UTIL_DvbStrncmp(aucExtName, s_astVaFs_MediaExtList[ucIdx].szExtName, ucExtLen))
								{
									bIsMatched = TRUE;
									break;
								}
							}
						}
					}

					if (!bIsMatched && (ulPalFileMask & PAL_FS_DIRINFO_PLAYLIST))
					{
						for (ucIdx = 0; ucIdx < s_ulVaFs_NumMediaExt; ucIdx++)
						{
							if ((s_astVaFs_MediaExtList[ucIdx].ucExtType == ePVR_FS_EXT_PLAYLIST) && (ucExtLen == s_astVaFs_MediaExtList[ucIdx].ucExtLen))
							{
								if (!MWC_UTIL_DvbStrncmp(aucExtName, s_astVaFs_MediaExtList[ucIdx].szExtName, ucExtLen))
								{
									bIsMatched = TRUE;
									break;
								}
							}
						}
					}

					if (!bIsMatched && (ulPalFileMask & PAL_FS_DIRINFO_MOVIE))
					{
						for (ucIdx = 0; ucIdx < s_ulVaFs_NumMediaExt; ucIdx++)
						{
							if ((s_astVaFs_MediaExtList[ucIdx].ucExtType == ePVR_FS_EXT_MOVIE) && (ucExtLen == s_astVaFs_MediaExtList[ucIdx].ucExtLen))
							{
								if (!MWC_UTIL_DvbStrncmp(aucExtName, s_astVaFs_MediaExtList[ucIdx].szExtName, ucExtLen))
								{
									bIsMatched = TRUE;
									break;
								}
							}
						}
					}

					if (!bIsMatched && (ulPalFileMask & PAL_FS_DIRINFO_MUSIC))
					{
						for (ucIdx = 0; ucIdx < s_ulVaFs_NumMediaExt; ucIdx++)
						{
							if ((s_astVaFs_MediaExtList[ucIdx].ucExtType == ePVR_FS_EXT_MUSIC) && (ucExtLen == s_astVaFs_MediaExtList[ucIdx].ucExtLen))
							{
								if (!MWC_UTIL_DvbStrncmp(aucExtName, s_astVaFs_MediaExtList[ucIdx].szExtName, ucExtLen))
								{
									bIsMatched = TRUE;
									break;
								}
							}
						}
					}

					if (!bIsMatched && (ulPalFileMask & PAL_FS_DIRINFO_IMAGE))
					{
						for (ucIdx = 0; ucIdx < s_ulVaFs_NumMediaExt; ucIdx++)
						{
							if ((s_astVaFs_MediaExtList[ucIdx].ucExtType == ePVR_FS_EXT_IMAGE) && (ucExtLen == s_astVaFs_MediaExtList[ucIdx].ucExtLen))
							{
								if (!MWC_UTIL_DvbStrncmp(aucExtName, s_astVaFs_MediaExtList[ucIdx].szExtName, ucExtLen))
								{
									bIsMatched = TRUE;
									break;
								}
							}
						}
					}
				}
			}
			break;
		}
	}

	return bIsMatched;
}

STATIC HERROR va_fs_GetDirInfo (xsvcCas_VaDirMask_e eMwDirMask, HUINT8 *dir_name, HUINT8 *filter_name,
							HUINT8 list_type, xsvcCas_VaFsDirStat_t *out_dir_stat)
{
	HUINT32		lsType;
	PAL_FS_DirStat_t	stFsDirStat;
	xsvcCas_VaFsDirStat_t	*pstFmDirStat = out_dir_stat;
	HUINT32				ulIdx, ulNumOfFile = 0, ulFilterKind=0;
	HUINT32				dir_mask;
	HUINT8				ucFindPlaylist = 0;
	HERROR				err = ERR_OK;

	HxLOG_Info("get dir info : %x %s %s %d\n", eMwDirMask, dir_name, filter_name, list_type);

	va_fs_ConvertMwDirMaskToPalDirMask (eMwDirMask, &dir_mask);

	memset(&stFsDirStat, 0, sizeof(PAL_FS_DirStat_t));

	lsType = (HUINT32)list_type;
	err = va_fs_op_GetDirInfo((char *)dir_name, filter_name, lsType, &stFsDirStat);
	if (err != ERR_OK)
	{
		HxLOG_Error("fail to get directory info : %s (0x%x)\n", dir_name, err);
		return err;
	}

	pstFmDirStat->num_files = 0;

	if (stFsDirStat.fileNumber > 0)
	{
		for (ulIdx = 0; ulIdx < stFsDirStat.fileNumber; ulIdx++)
		{
			if (va_fs_IsMatchedFile(dir_mask, &stFsDirStat.s_astfilestats[ulIdx]))
			{
				ulNumOfFile++;
			}
		}

		if (ulNumOfFile)
		{
			pstFmDirStat->file_stat = (xsvcCas_VaFsFileStat_t *)OxMW_Malloc(ulNumOfFile * sizeof(xsvcCas_VaFsFileStat_t));
			if (pstFmDirStat->file_stat != NULL)
			{
				pstFmDirStat->num_files = ulNumOfFile;
				ulNumOfFile = 0;
				// DIR -> PLAYLIST -> FILE 순으로 정렬이 될수 있도록 한다.
				if (dir_mask & PAL_FS_DIRINFO_DIR)
				{
					ulFilterKind = PAL_FS_DIRINFO_DIR;
					for (ulIdx = 0; ulIdx < stFsDirStat.fileNumber; ulIdx++)
					{
						if (va_fs_IsMatchedFile(ulFilterKind, &stFsDirStat.s_astfilestats[ulIdx]))
						{
							HxSTD_StrNCpy (pstFmDirStat->file_stat[ulNumOfFile].file_name, stFsDirStat.s_astfilestats[ulIdx].aucFileName, FS_MAX_FILE_NAME_LEN);
							pstFmDirStat->file_stat[ulNumOfFile].file_name[FS_MAX_FILE_NAME_LEN] = '\0';
							pstFmDirStat->file_stat[ulNumOfFile].attrib	= eFS_FileAttr_DIR;
							pstFmDirStat->file_stat[ulNumOfFile].size = stFsDirStat.s_astfilestats[ulIdx].ulFileSize;
							ulNumOfFile++;
						}
					}
				}

				// 일반 파일들을 읽으려는 경우
				if ((dir_mask == PAL_FS_DIRINFO_DIR) || (dir_mask == PAL_FS_DIRINFO_FILE) || (dir_mask == PAL_FS_DIRINFO_ALL))
				{
					HxLOG_Info("normal reading...\n");
				}
				else
				{
					// Medialist 에서 사용하는 경우로, Playlist 파일이 존재하는지 체크
					if (dir_mask & PAL_FS_DIRINFO_PLAYLIST)
					{
						ulFilterKind = PAL_FS_DIRINFO_PLAYLIST;
						for (ulIdx = 0; ulIdx < stFsDirStat.fileNumber; ulIdx++)
						{
							if (va_fs_IsMatchedFile(ulFilterKind, &stFsDirStat.s_astfilestats[ulIdx]))
							{
								HxSTD_StrNCpy (pstFmDirStat->file_stat[ulNumOfFile].file_name, stFsDirStat.s_astfilestats[ulIdx].aucFileName, FS_MAX_FILE_NAME_LEN);
								pstFmDirStat->file_stat[ulNumOfFile].file_name[FS_MAX_FILE_NAME_LEN] = '\0';
								pstFmDirStat->file_stat[ulNumOfFile].attrib = eFS_FileAttr_PLAYLIST;
								pstFmDirStat->file_stat[ulNumOfFile].size = stFsDirStat.s_astfilestats[ulIdx].ulFileSize;
								ulNumOfFile++;
								ucFindPlaylist	= 1;
							}
						}
					}
				}

				// Playlist 가 존재하는 경우
				if (ucFindPlaylist)
				{
					ulFilterKind = dir_mask & ~(PAL_FS_DIRINFO_DIR | PAL_FS_DIRINFO_PLAYLIST);
				}
				else
				{
					ulFilterKind = dir_mask & ~PAL_FS_DIRINFO_DIR;
				}

				// 찾을 Type 이 남은 경우 ( Playlist 파일(*.hpl)을  제외한 파일들이 해당함. )
				// PAL_FS_DIRINFO_FILE, PAL_FS_DIRINFO_VIDEO, PAL_FS_DIRINFO_MOVIE, PAL_FS_DIRINFO_MUSIC, PAL_FS_DIRINFO_IMAGE
				if (ulFilterKind && (ulFilterKind != PAL_FS_DIRINFO_PLAYLIST))
				{
					for (ulIdx = 0; ulIdx < stFsDirStat.fileNumber; ulIdx++)
					{
						if (va_fs_IsMatchedFile(ulFilterKind, &stFsDirStat.s_astfilestats[ulIdx]))
						{
							HxSTD_StrNCpy (pstFmDirStat->file_stat[ulNumOfFile].file_name, stFsDirStat.s_astfilestats[ulIdx].aucFileName, FS_MAX_FILE_NAME_LEN);
							pstFmDirStat->file_stat[ulNumOfFile].file_name[FS_MAX_FILE_NAME_LEN] = '\0';
							pstFmDirStat->file_stat[ulNumOfFile].attrib = eFS_FileAttr_FILE;
							pstFmDirStat->file_stat[ulNumOfFile].size = stFsDirStat.s_astfilestats[ulIdx].ulFileSize;
							pstFmDirStat->file_stat[ulNumOfFile].access_time = stFsDirStat.s_astfilestats[ulIdx].aTime;
							pstFmDirStat->file_stat[ulNumOfFile].create_time = stFsDirStat.s_astfilestats[ulIdx].cTime;
							pstFmDirStat->file_stat[ulNumOfFile].modify_time = stFsDirStat.s_astfilestats[ulIdx].mTime;
							ulNumOfFile++;
						}
					}
				}
			}
		}

		if (stFsDirStat.s_astfilestats)
		{
			// File System에서 VK_MEM_Alloc으로 할당받은 녀석이다.
			va_fs_op_GetDirInfoFree(&stFsDirStat);
		}

		pstFmDirStat->num_files = ulNumOfFile;

	}

	HxLOG_Info("done to get directory info.\n");

	return err;
}


DWORD VA_FS_Open(const char *pszFileName , tVA_FS_Mode eOpeningMode)
{
	HINT32			nErrCode;
	HUINT32	hFileHandle;
	HUINT32 	flags	;

	HxFILE_t		hFile;

	switch(eOpeningMode)
	{
		case eFILE_READ:
			hFile = HLIB_FILE_Open(pszFileName, "r");

			break;

		case eFILE_WRITE:
			if (HLIB_FILE_Exist(pszFileName) == TRUE)
			{
				hFile = HLIB_FILE_Open(pszFileName, "r+");
			}
			else
			{
				hFile = HLIB_FILE_Open(pszFileName, "w+");
			}
			break;

		case eFILE_PURGE:
			hFile = HLIB_FILE_Open(pszFileName, "w+");
			break;

		default:
			return kVA_ILLEGAL_HANDLE;
			break;
	}

	if (hFile == NULL)
	{
		HxLOG_Info("error occurred in VA_FS_Open(%d).\n", (unsigned int)nErrCode);
		return kVA_ILLEGAL_HANDLE;
	}

	return (DWORD)hFile;
}

INT VA_FS_Close(DWORD dwFileHandle)
{
	HINT32			nErrCode;
	nErrCode = HLIB_FILE_Close((HxFILE_t)dwFileHandle);
	if(nErrCode)
	{
		return kVA_ERROR;
	}
	return kVA_OK;
}

INT VA_FS_Read (DWORD dwFileHandle, UINT32 *pNbReadBytes, BYTE *pBuffer)
{
	if (*pNbReadBytes == 0)
	{
		return kVA_OK;
	}

	HUINT32	ulReadSize;

	ulReadSize = HLIB_FILE_Read((HxFILE_t)dwFileHandle, (void *)pBuffer, 1, *pNbReadBytes)
	*pNbReadBytes = ulReadSize;

	if (ulReadSize <=0 && ulReadSize <= *pNbReadBytes)
	{
		return kVA_ERROR;
	}

	return kVA_OK;
}

INT VA_FS_Write (DWORD dwFileHandle, UINT32 *pNbWriteBytes, BYTE * pBuffer)
{
	if (*pNbWriteBytes == 0)
	{
		return kVA_OK;
	}

	HUINT32 ulWriteSize;

	ulWriteSize = HLIB_FILE_Write((HxFILE_t)dwFileHandle, (void *)pBuffer, 1, *pNbWriteBytes)
	*pNbWriteBytes = ulWriteSize;

	if (ulWriteSize <=0 && ulWriteSize <= *pNbWriteBytes)
	{
		return kVA_ERROR;
	}

	return kVA_OK;
}
INT VA_FS_Seek (DWORD dwFileHandle,
                tVA_FS_SeekOrigin eOrigin, INT32 iOffset,
                UINT32 *pNewPositionFromBegin)
{
	HINT32		nErrCode;
	HxFILE_Seek_e eMode;

	if(eOrigin == eSEEK_FROM_BEGIN)
	{
		eMode = eHxFILE_SEEK_SET;
	}
	else if (eOrigin == eSEEK_FROM_CURRENT)
	{
		eMode = eHxFILE_SEEK_CUR;
	}
	else if (eOrigin == eSEEK_FROM_END)
	{
		eMode = eHxFILE_SEEK_END;
	}
	else
	{
		eMode = eHxFILE_SEEK_SET;
	}

	nErrCode = 	HLIB_FILE_Seek((HxFILE_t)dwFileHandle, iOffset, eMode, (HOffset_t *)pNewPositionFromBegin);
	if(nErrCode)
	{
		return kVA_ERROR;
	}

	return kVA_OK;
}

INT VA_FS_GetSize (const char *pszFileName, UINT32 *pFileSize)
{
	HINT32		nErrCode;
	HxFILE_t		hFile;
	HUINT32		ulFileSize = 0;

	hFile = HLIB_FILE_Open(pszFileName, "r");
	if (hFile == NULL)
	{
		return kVA_ERROR;
	}

	nErrCode = HLIB_FILE_Size32(hFile, &ulFileSize);
	if (nErrCode)
	{
		HLIB_FILE_Close(hFile);
		return kVA_ERROR;
	}

	if (pFileSize)
	{
		*pFileSize = ulFileSize;
	}

	HLIB_FILE_Close(hFile);

	return kVA_OK;
}

INT VA_FS_Remove (const char * pszFileName)
{
	HINT32		nErrCode;

	nErrCode = HLIB_FILE_Delete(pszFileName);
	if(nErrCode)
	{
		return kVA_ERROR;
	}

	return kVA_OK;
}

UINT32 VA_FS_GetFilesCount(void)
{
	INT 		i = 0;
	HINT32		nErrCode;
	const char pathname[] =" ";
	static char *s_acnoFilterExt[] = {NULL, NULL}; // no filter
/*
	TODO: hjlee3
	Viaccess 폴더를 두어야 하나 ??
	/var/lib/humaxtv/cas/
*/
	HxDIR_t		hDir;

	hDir = HLIB_DIR_Open(CAS_DIR_PATH);
	if (hDir == NULL)
	{
		return kVA_ERROR;
	}

	do {
		if (HLIB_DIR_IsFile(hDir) == TRUE)
		{
			i++;
		}
	} while (HLIB_DIR_Next(hDir) == ERR_OK);
	HLIB_DIR_Close(hDir);

	return i;
}

INT VA_FS_GetFiles(UINT32 *pFilesCount, tVA_FS_FileInfo aFiles[])
{
	HINT32		i, uinidx=*pFilesCount;
	HERROR		err;

	xsvcCas_VaFsDirStat_t		stDirStat;
	PAL_FS_FileStat_t		 stFileStat;

/*
	TODO: hjlee3
	이 파일의 parameter를 보면 aFiles.szName 이 포함하는 pFileConut를 넘기는 것으로 보인다.
	aFiles[] 을 보면 여러개가  오는 것으로 보인다.
	그럼 여기에 파일 이름을 넣어 주는 것인가?

*/
	memset(&stFileStat, 0, sizeof(PAL_FS_FileStat_t));
	memset(&stDirStat, 0, sizeof(xsvcCas_VaFsDirStat_t));

	if(HLIB_DIR_IsExist(CAS_DIR_PATH) != TRUE)
	{
		return kVA_ERROR;
	}

	err = va_fs_GetDirInfo(PAL_FS_DIRINFO_FILE, CAS_DIR_PATH, NULL, PAL_FS_INFO_ALLLIST, &stDirStat);
	if(err)
	{
		return kVA_ERROR;
	}

	for(i=0 ; (i < stDirStat.num_files) && (i < uinidx) ; i++)
	{
		HxSTD_memcpy(aFiles[i].szName, stDirStat.file_stat[i].file_name, kVA_FS_MAX_FILE_NAME_SIZE);
	}

	*pFilesCount=i;

	return kVA_OK;
}

