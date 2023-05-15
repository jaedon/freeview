
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */

#include <rxlib.h>



/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */
#undef	RxLOG_DOMAIN
#define	RxLOG_DOMAIN	"RxDIR"

#define	LOG				RxLOG_Print
#define	ERR_LOG			RxLOG_Error


#if defined(OS_LINUX)
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#if defined(OS_WIN32)
#include <windows.h>
#endif

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

#if defined(OS_LINUX)
/* OS_LINUX */

typedef DIR *					RxDIR_OS_t;
typedef struct dirent *			RxDIR_Dirent_t;
typedef struct stat				RxDIR_Stat_t;

#elif defined(OS_WIN32)
/* OS_WIN32 */

typedef HANDLE					RxDIR_OS_t;
typedef WIN32_FIND_DATA			RxDIR_Dirent_t;
typedef HINT32					RxDIR_Stat_t;
#endif



typedef enum tagRxDIR_FindOpt
{
	eRxFindOpt_None		= 0x0000,
	eRxFindOpt_Retrieve	= 0x0001,
	eRxFindOpt_Hidden		= 0x0002,
	eRxFindOpt_ReadOnly 	= 0x0004,
	eRxFindOpt_System 	= 0x0008,


	eRxFindOpt_Start_Directory 	= 0x0100,
	eRxFindOpt_End_Directory		= 0x0200,


	eRxFindOpt_ALL		= 0xFFFF
} RxDIR_FindOpt_e;


typedef struct tagRxDIR_Find
{
	HUINT8 			*pszFilter;
	HBOOL			bIsEnd;
	RxDIR_FindOpt_e	eRxFindOpt;

	HBOOL 			(* cbFile)(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata);
	HBOOL 			(* cbStartDir)(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata);
	HBOOL 			(* cbEndDir)(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata);

	void 			*pUserdata;
} RxDIR_Find_t;


typedef struct tagRxDIR_item
{
	RxDIR_Dirent_t	pDirent;
	RxDIR_Stat_t	stStat;

	UNIXTIME		ulCreateTime;
	UNIXTIME		ulAccessTime;
	UNIXTIME		ulModificationTime;

	HUINT32			ulHiSize;
	HUINT32			ulLowSize;

	HCHAR			*pszName;
} RxDIR_Item_t;

typedef struct tagRxDIR
{
	RxDIR_OS_t		pOSDir;
	HUINT8			*pszPath;

	RxDIR_Item_t	stCurItem;
} RxDIR;






/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___OS_Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
STATIC HERROR	_rxdir_os_Open(RxDIR *pDir);
STATIC HERROR	_rxdir_os_Next(RxDIR *pDir);
STATIC HERROR	_rxdir_os_Stat(RxDIR *pDir);
STATIC HERROR	_rxdir_os_Close(RxDIR *pDir);



#if defined(OS_LINUX)
/* OS_LINUX */


STATIC HERROR	_rxdir_os_Open(RxDIR *pDir)
{
	pDir->pOSDir = opendir(pDir->pszPath);
	if(pDir->pOSDir == NULL)
	{
		return ERR_FAIL;
	}

	pDir->stCurItem.pDirent = readdir(pDir->pOSDir);
	if(pDir->stCurItem.pDirent == NULL)
	{
		return ERR_FAIL;
	}

	return _rxdir_os_Stat(pDir);
}

STATIC HERROR	_rxdir_os_Next(RxDIR *pDir)
{
	pDir->stCurItem.pDirent = readdir(pDir->pOSDir);
	if(pDir->stCurItem.pDirent == NULL)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR	_rxdir_os_Close(RxDIR *pDir)
{
	closedir(pDir->pOSDir);
	return ERR_OK;
}

STATIC HERROR	_rxdir_os_Stat(RxDIR *pDir)
{
	HUINT8			szDstPath[MAX_HXDIR_FILENAME_LENGTH];

	pDir->stCurItem.pszName =  pDir->stCurItem.pDirent->d_name;
	RxSTD_snprintf(szDstPath, MAX_HXDIR_FILENAME_LENGTH, "%s/%s", pDir->pszPath, pDir->stCurItem.pszName);

	lstat(szDstPath, &pDir->stCurItem.stStat);

	pDir->stCurItem.ulCreateTime		= pDir->stCurItem.stStat.st_ctime;
	pDir->stCurItem.ulAccessTime		= pDir->stCurItem.stStat.st_atime;
	pDir->stCurItem.ulModificationTime	= pDir->stCurItem.stStat.st_mtime;
	pDir->stCurItem.ulHiSize			= pDir->stCurItem.stStat.st_size >> 32;
	pDir->stCurItem.ulLowSize			= pDir->stCurItem.stStat.st_size & 0xFFFFFFFF;

	return ERR_OK;
}


/*
#include <sys/stat.h>
S_ISREG() , S_ISDIR() , S_CHR() , S_BLK() , S_FIFO() , S_LNK() , S_SSOCK()


#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif
*/

STATIC	HBOOL	_rxdir_os_IsFile(RxDIR *pDir)
{
	return S_ISREG(pDir->stCurItem.stStat.st_mode);
}

STATIC	HBOOL	_rxdir_os_IsDir(RxDIR *pDir)
{
	return S_ISDIR(pDir->stCurItem.stStat.st_mode);
}

/*
define	의미			8진코드
----------------------------------
S_IRUSR	user-read		0400
S_IWUSR	user-write		0200
S_IXUSR	user-execute	0100
----------------------------------
S_IRGRP	group-read		0040
S_IWGRP	group-write	0020
S_IXGRP	group-execute	0010
----------------------------------
S_IROTH	other-read		0004
S_IWOTH	other-write		0002
S_IXOTH	other-execute	0001
*/

STATIC		HERROR	_rxdir_os_Access(const HUINT8 *name)
{
	HINT32 nRet = 0;

	nRet = access(name, F_OK);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC		HERROR	_rxdir_os_MakeDIR(const HUINT8 *name)
{
	HINT32 nRet = 0;

	nRet = mkdir(name, 755);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


#elif defined(OS_WIN32)
/* OS_WIN32 */



STATIC HERROR	_rxdir_os_Open(RxDIR *pDir)
{
	HUINT8 szPath[MAX_HXDIR_FILENAME_LENGTH] = {0, };

	RxSTD_snprintf(szPath, MAX_HXDIR_FILENAME_LENGTH, "%s/*.*", pDir->pszPath);

	pDir->pOSDir = FindFirstFile(szPath, &pDir->stCurItem.pDirent);
	if(pDir->pOSDir == (HANDLE)0xffffffff)
	{
		return ERR_FAIL;
	}

	return _rxdir_os_Stat(pDir);
}

STATIC HERROR	_rxdir_os_Next(RxDIR *pDir)
{
	HBOOL bISNext = FALSE;

	bISNext = FindNextFile(pDir->pOSDir, &pDir->stCurItem.pDirent);
	if(bISNext == FALSE)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


STATIC HERROR	_rxdir_os_Close(RxDIR *pDir)
{
	FindClose(pDir->pOSDir);
	return ERR_OK;
}


STATIC HERROR	_rxdir_os_Stat(RxDIR *pDir)
{
	pDir->stCurItem.pszName		= pDir->stCurItem.pDirent.cFileName;
	pDir->stCurItem.stStat 		= pDir->stCurItem.pDirent.dwFileAttributes;

	pDir->stCurItem.ulCreateTime		= pDir->stCurItem.pDirent.ftCreationTime.dwLowDateTime;
	pDir->stCurItem.ulAccessTime		= pDir->stCurItem.pDirent.ftLastAccessTime.dwLowDateTime;
	pDir->stCurItem.ulModificationTime	= pDir->stCurItem.pDirent.ftLastWriteTime.dwLowDateTime;

	pDir->stCurItem.ulHiSize			= pDir->stCurItem.pDirent.nFileSizeHigh;
	pDir->stCurItem.ulLowSize			= pDir->stCurItem.pDirent.nFileSizeLow;

	return ERR_OK;
}

/*
#include <window.h> or #include <winnt.h>

#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002
#define FILE_SHARE_DELETE               0x00000004

#define FILE_ATTRIBUTE_READONLY             0x00000001
#define FILE_ATTRIBUTE_HIDDEN               0x00000002
#define FILE_ATTRIBUTE_SYSTEM               0x00000004
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020
#define FILE_ATTRIBUTE_DEVICE               0x00000040
#define FILE_ATTRIBUTE_NORMAL               0x00000080
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800
#define FILE_ATTRIBUTE_OFFLINE              0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000
#define FILE_ATTRIBUTE_VIRTUAL              0x00010000
*/

STATIC	HBOOL	_rxdir_os_IsFile(RxDIR *pDir)
{
	return ((pDir->stCurItem.stStat & FILE_ATTRIBUTE_DIRECTORY) > 0) ? FALSE : TRUE;
}

STATIC	HBOOL	_rxdir_os_IsDir(RxDIR *pDir)
{
	return ((pDir->stCurItem.stStat & FILE_ATTRIBUTE_DIRECTORY) > 0) ? TRUE : FALSE;
}


STATIC		HERROR	_rxdir_os_Access(const HUINT8 *name)
{
	HINT32 nRet = 0;

	nRet = access(name);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC		HERROR	_rxdir_os_MakeDIR(const HUINT8 *name)
{
	HINT32 nRet = 0;

	nRet = mkdir(name);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#else
#error ??

#endif


/* *************************************************************************************************** */
#define ___COMMON_Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */


STATIC		RxDIR_FindOpt_e		_rxdir_GetFindOptByStr(HUINT8 *pszOption)
{
	RxDIR_FindOpt_e	eRxFindOpt = eRxFindOpt_None;

	if(pszOption == NULL || *pszOption == 0)
	{
		return eRxFindOpt_None;
	}

	while(*pszOption)
	{
		switch(*pszOption)
		{
		case 'R':		eRxFindOpt |= eRxFindOpt_Retrieve;			break;
		case 'H':		eRxFindOpt |= eRxFindOpt_Hidden;			break;
		case 'S':		eRxFindOpt |= eRxFindOpt_System;			break;
		case 'O':		eRxFindOpt |= eRxFindOpt_Hidden;			break;
		case 'A':		eRxFindOpt |= eRxFindOpt_ALL;				break;
		case 'D':		eRxFindOpt |= (eRxFindOpt_Start_Directory | eRxFindOpt_End_Directory);			break;
		case 'T':		eRxFindOpt |= eRxFindOpt_Start_Directory;	break;
		case 'N':		eRxFindOpt |= eRxFindOpt_End_Directory;		break;

		default:
			break;
		}

		pszOption++;
	}

	return eRxFindOpt;
}

STATIC			HINT32	_rxdir_Find_GetOnlyFilename(HUINT8 *pszStartFilename, HUINT8 *pszEndFilename)
{
	return 0;
}

STATIC			HBOOL	_rxdir_Find_IsMatch(HUINT8 *pszName, HUINT8 *pszFilter, RxDIR_FindOpt_e	eRxFindOpt)
{
	HUINT8	*pszFileExt = NULL;
	HUINT8	*pszFilterExt = NULL;
	HUINT8	*pszWildChar = NULL;
	HINT32	nFilenameLen = 0;


	RxLOG_Assert(pszName);

	// filter 가 null이면 *.* 과 동일하게 동작
	if(pszFilter == NULL)
	{
		return TRUE;
	}

	pszFileExt 		= strrchr(pszName, '.');
	pszFilterExt	= strrchr(pszFilter, '.');

	/* ext name */
	if(pszFilterExt == NULL)
	{
		;
	}
	else if(pszFilterExt[1] == '*')
	{
		;
	}
	else if(pszFileExt == NULL)
	{
		return FALSE;
	}
	else if(RxSTD_StrNCaseCmp(pszFilterExt, pszFileExt, strlen(pszFilterExt)) == 0)
	{
		;
	}
	else
	{
		return FALSE;
	}

	/* file name */
	pszWildChar = strchr(pszFilter, '*');

	if(pszFilterExt)
	{
		nFilenameLen = pszFilterExt-pszFilter;
	}
	else
	{
		nFilenameLen = strlen(pszFilter);
	}

	if(pszFilter[0] == '*')
	{
		;
	}
	else if(RxSTD_StrNCaseCmp(pszName, pszFilter, nFilenameLen) == 0)
	{
		;
	}
	else
	{
		return FALSE;
	}




	return TRUE;
}

STATIC			HBOOL	_rxdir_IgnoreName(HUINT8 *pszDirName, HBOOL bFind)
{
	if(pszDirName[0] != '.')
	{
		return FALSE;
	}
	else if(pszDirName[1] == 0)
	{
		return TRUE;
	}
	else if(pszDirName[1] == '.' && pszDirName[2] == 0)
	{
		return TRUE;
	}
#if defined(CONFIG_DEBUG)
	else if(bFind == TRUE && strcmp(pszDirName, ".svn") == 0)
	{
		return TRUE;
	}
#endif

	return FALSE;
}

STATIC			HINT32	_rxdir_Find(RxDIR_Find_t *pstFind, HUINT8	*pszPath)
{
	RxDIR_t		*pDir = NULL;
	HINT32		nFilterFile = 0;
	HBOOL		bIsMatch = FALSE;
	HUINT8		*pszName = NULL;
	HUINT8		szFullPath[MAX_HXDIR_FILENAME_LENGTH] = {0,};

	pDir = RxDIR_Open(pszPath);
	if(pDir == NULL)
	{
		return 0;
	}

	do
	{
		pszName = RxDIR_GetName(pDir);
		if(_rxdir_IgnoreName(pszName, TRUE) == TRUE)
		{
			continue;
		}

		bIsMatch = FALSE;
		if(RxDIR_IsDir(pDir))
		{
			if((pstFind->eRxFindOpt & eRxFindOpt_Start_Directory))
			{
				bIsMatch = _rxdir_Find_IsMatch(pszName, pstFind->pszFilter, pstFind->eRxFindOpt);
				if(bIsMatch == TRUE)
				{
					nFilterFile++;
					pstFind->bIsEnd = pstFind->cbStartDir(pDir, RxDIR_GetPath(pDir), RxDIR_GetName(pDir), TRUE, pstFind->pUserdata);
				}
			}

			if((pstFind->eRxFindOpt & eRxFindOpt_Retrieve) == 0)
			{
				continue;
			}
			RxSTD_snprintf(szFullPath, MAX_HXDIR_FILENAME_LENGTH, "%s/%s", RxDIR_GetPath(pDir), pszName);
			nFilterFile += _rxdir_Find(pstFind, szFullPath);

			if(pstFind->cbEndDir && (pstFind->eRxFindOpt & eRxFindOpt_End_Directory))
			{
				pstFind->bIsEnd = pstFind->cbEndDir(pDir, RxDIR_GetPath(pDir), RxDIR_GetName(pDir), TRUE, pstFind->pUserdata);
			}

		}
		else if(RxDIR_IsFile(pDir))
		{
			bIsMatch = _rxdir_Find_IsMatch(pszName, pstFind->pszFilter, pstFind->eRxFindOpt);
			if(bIsMatch == TRUE)
			{
				nFilterFile++;
				pstFind->bIsEnd = pstFind->cbFile(pDir, RxDIR_GetPath(pDir), RxDIR_GetName(pDir), FALSE, pstFind->pUserdata);
			}
 		}
		else
		{
			/* skip file */
		}

		if(pstFind->bIsEnd	== TRUE)
		{
			break;
		}

	} while(RxDIR_Next(pDir) == ERR_OK);

	RxDIR_Close(pDir);

	return nFilterFile;
}

STATIC	HERROR	_rxdir_MakeDIR_P(HUINT8	*pszPath)
{
	HUINT8	*p = pszPath;
	HERROR	hError = ERR_OK;

	while(p = strchr(p + 1, '/'))
	{
		*p = 0;

		hError = _rxdir_os_Access(pszPath);
		if (hError == ERR_OK)
		{
			*p = '/';
			continue;
		}

		hError = _rxdir_os_MakeDIR(pszPath);
		if(hError != ERR_OK)
		{
			ERR_LOG("[RxDIR_Make] _rxdir_os_MakeDIR Error!!\n");
			return ERR_FAIL;
		}
		*p = '/';
	}

	return _rxdir_os_MakeDIR(pszPath);
}

STATIC HERROR	_rxdir_Delete(const HUINT8	*pszPath)
{
	RxDIR_t 	*pDir = NULL;
	HINT32		nRet = ERR_FAIL;
	HUINT8		*pszName = NULL;
	HUINT8		szFullPath[MAX_HXDIR_FILENAME_LENGTH] = {0,};

	pDir = RxDIR_Open((HUINT8 *)pszPath);
	if(pDir == NULL)
	{
		return nRet;
	}

	do
	{
		pszName = RxDIR_GetName(pDir);
		if(_rxdir_IgnoreName(pszName, FALSE) == TRUE)
		{
			continue;
		}

		RxSTD_snprintf(szFullPath, MAX_HXDIR_FILENAME_LENGTH, "%s/%s", RxDIR_GetPath(pDir), pszName);

		if(RxDIR_IsDir(pDir))
		{
			_rxdir_Delete(szFullPath);
			nRet = rmdir(szFullPath);
		}
		else
		{
			nRet = RxFile_Delete(szFullPath);
		}
		if(nRet < 0)
		{
			nRet = ERR_FAIL;
			goto FUNC_OUT;
		}
	} while(RxDIR_Next(pDir) == ERR_OK);

FUNC_OUT:
	RxDIR_Close(pDir);

	return ERR_OK;
}

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

RxDIR_t			*RxDIR_Open(HUINT8	*pszPath)
{
	RxDIR 	*pNew = NULL;
	HERROR	hError= ERR_OK;

	RxLOG_Assert(pszPath);

	pNew = (RxDIR *)RxSTD_Alloc(sizeof(RxDIR));
	RxLOG_Assert(pNew);
	RxSTD_MemSet(pNew, 0, sizeof(RxDIR));

	pNew->pszPath = pszPath;
	hError = _rxdir_os_Open(pNew);
	if(hError != ERR_OK)
	{
		RxSTD_Free(pNew);
		ERR_LOG("%s Directory open error!!\n", pszPath);
		return NULL;
	}

	return (RxDIR_t *)pNew;
}

HERROR			RxDIR_Next(RxDIR_t *pSelf)
{
	HERROR	hError = ERR_OK;

	RxLOG_Assert(pSelf);

	hError = _rxdir_os_Next((RxDIR *)pSelf);
	if(hError != ERR_OK)
	{
		return ERR_FAIL;
	}

	hError = _rxdir_os_Stat((RxDIR *)pSelf);

	return hError;
}

HERROR			RxDIR_Close(RxDIR_t *pSelf)
{
	HERROR	hError = ERR_OK;

	RxLOG_Assert(pSelf);

	hError = _rxdir_os_Close((RxDIR *)pSelf);
	RxSTD_Free(pSelf);

	return hError;
}

HUINT8			*RxDIR_GetName(RxDIR_t *pSelf)
{
	RxDIR	*pDir =  (RxDIR *)pSelf;
	return pDir->stCurItem.pszName;
}

HUINT8			*RxDIR_GetPath(RxDIR_t *pSelf)
{
	RxDIR	*pDir =  (RxDIR *)pSelf;
	return pDir->pszPath;
}

HBOOL			RxDIR_IsDir(RxDIR_t *pSelf)
{
	return _rxdir_os_IsDir((RxDIR *)pSelf);
}

HBOOL			RxDIR_IsFile(RxDIR_t *pSelf)
{
	return _rxdir_os_IsFile((RxDIR *)pSelf);
}

HINT32			RxDIR_Find(HUINT8	*pszPath, HUINT8 *pszFilter, HUINT8 *pszOption,
								HBOOL (* func)(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata), void *userdata)
{
	return RxDIR_FindEx(pszPath, pszFilter, pszOption, func, func, NULL, userdata);
}

HINT32			RxDIR_FindEx(HUINT8	*pszPath,
								HUINT8 *pszFilter,
								HUINT8 *pszOption,
								HBOOL (* cbfile)(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata),
								HBOOL (* cbstartdir)(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata),
								HBOOL (* cbenddir)(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata),
								void *userdata)
{
	RxDIR_Find_t stFind;

	LOG("[%s:%d] ====> \npszPath(%s)\npszFilter(%s)\npszOption(%s)\ncbfile(0x%x) cbfile(0x%x) cbstartdir(0x%x) cbenddir(0x%x)",
			__FUNCTION__, __LINE__, pszPath, pszFilter, pszFilter, pszOption, cbfile, cbstartdir, cbenddir, userdata);

	RxSTD_MemSet(&stFind, 0, sizeof(RxDIR_Find_t));

	stFind.pszFilter	= pszFilter;
	stFind.bIsEnd		= FALSE;
	stFind.eRxFindOpt 	= _rxdir_GetFindOptByStr(pszOption);

	stFind.cbFile		= cbfile;
	stFind.cbStartDir	= cbstartdir;
	stFind.cbEndDir		= cbenddir;
	stFind.pUserdata	= userdata;

	return _rxdir_Find(&stFind, pszPath);
}

HERROR			RxDIR_Make(const HUINT8	*pszPath)
{
	HUINT8	*pDup = NULL;
	HERROR	hError = ERR_OK;

	RxLOG_Assert(pszPath);

	hError = _rxdir_os_MakeDIR(pszPath);
#if 0
	if(hError != ERR_OK)
	{
		pDup = HLIB_STD_StrDup(pszPath);
		hError = _rxdir_MakeDIR_P(pDup);
		RxSTD_Free(pDup);
	}
#endif
	return hError;
}

HERROR			RxDIR_Move(const HUINT8	*pszOldPath, const HUINT8	*pszNewPath)
{
	HINT32	nRet = 0;
	HERROR	hError = ERR_OK;

	RxLOG_Assert(pszOldPath);
	RxLOG_Assert(pszNewPath);

	hError = _rxdir_os_Access(pszOldPath);
	if(hError != ERR_OK)
	{
		return ERR_FAIL;
	}

	nRet = rename(pszOldPath, pszNewPath);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR			RxDIR_Delete(const HUINT8	*pszPath)
{
	HINT32	nRet = 0;
	HERROR	hError = ERR_OK;

	hError = _rxdir_Delete(pszPath);
	if(hError == ERR_OK)
	{
		nRet = rmdir(pszPath);
	}

	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR			RxDIR_Copy(const HUINT8	*pszPath1, const HUINT8	*pszPath2)
{
	// TODO:: 만들어야 함.
	RxLOG_Assert(0);

	return ERR_OK;
}

RxDIR_t			*RxDIR_Dup(RxDIR_t *pDir)
{
	#if 0
	RxDIR 	*pNew = NULL;
	HERROR	hError= ERR_OK;

	RxLOG_Assert(pDir);

	pNew = (RxDIR *)HLIB_STD_MemAlloc(sizeof(RxDIR));
	RxLOG_Assert(pNew);
	RxSTD_MemSet(pNew, 0, sizeof(RxDIR));

	pNew->pszPath = ((RxDIR*)pDir)->pszPath;
	hError = _rxdir_os_Open(pNew);
	if(hError != ERR_OK)
	{
		RxSTD_Free(pNew);
		ERR_LOG("%s Directory open error!!\n", pNew->pszPath);
		return NULL;
	}

	return (RxDIR_t *)pNew;
	#endif

	RxDIR 	*pNew = NULL;
	HERROR	hError= ERR_OK;

	RxLOG_Assert(pDir);

	pNew = (RxDIR *)RxSTD_Alloc(sizeof(RxDIR));
	RxLOG_Assert(pNew);
	RxSTD_MemCopy(pNew, pDir, sizeof(RxDIR));

	return (RxDIR_t *)pNew;
}

HERROR 			RxDIR_GetTime(RxDIR_t *pSelf, UNIXTIME *pulCreateTime, UNIXTIME *pulAccessTime, UNIXTIME *pulModificationTime)
{
	RxDIR		*pDir = NULL;

	pDir = (RxDIR *)pSelf;
	if(pDir == NULL)
	{
		return ERR_FAIL;
	}

	if(pulCreateTime)
	{
		*pulCreateTime = pDir->stCurItem.ulCreateTime;
	}

	if(pulAccessTime)
	{
		*pulAccessTime = pDir->stCurItem.ulAccessTime;
	}

	if(pulModificationTime)
	{
		*pulModificationTime = pDir->stCurItem.ulModificationTime;
	}

	return ERR_OK;
}

HERROR 			RxDIR_GetFileSize(RxDIR_t *pSelf, HUINT32 *pulHighFileSize, HUINT32 *pulLowFileSize)
{
	RxDIR		*pDir = NULL;

	pDir = (RxDIR *)pSelf;

	if(pDir == NULL)
	{
		return ERR_FAIL;
	}

	if(pulHighFileSize&& pulLowFileSize)
	{
		*pulHighFileSize = pDir->stCurItem.ulHiSize;
		*pulLowFileSize = pDir->stCurItem.ulLowSize;

	}

	return ERR_OK;
}

