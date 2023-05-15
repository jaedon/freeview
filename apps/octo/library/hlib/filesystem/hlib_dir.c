
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */

#include <hlib.h>



/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */
#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxDIR"

#define	LOG				HxLOG_Print
#define	ERR_LOG			HxLOG_Error

#include <errno.h>

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
typedef enum tagHLIB_DIR_FindOpt
{
	eFindOpt_None		= 0x0000,
	eFindOpt_Retrieve	= 0x0001,
	eFindOpt_Hidden		= 0x0002,
	eFindOpt_ReadOnly 	= 0x0004,
	eFindOpt_System 	= 0x0008,


	eFindOpt_Start_Directory 	= 0x0100,
	eFindOpt_End_Directory		= 0x0200,


	eFindOpt_ALL		= 0xFFFF
} HLIB_DIR_FindOpt_e;


typedef struct tagHLIB_DIR_Find
{
	HUINT8 			*pszFilter;
	HBOOL			bIsEnd;
	HLIB_DIR_FindOpt_e	eFindOpt;

	HBOOL 			(* cbFile)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata);
	HBOOL 			(* cbStartDir)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata);
	HBOOL 			(* cbEndDir)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata);

	void 			*pUserdata;
} HLIB_DIR_Find_t;

#if	defined(OS_LINUX)
typedef struct tagHLIB_DIR_item
{
	struct dirent	*pstDirent;
	struct stat		stStat;

	UNIXTIME		ulCreateTime;
	UNIXTIME		ulAccessTime;
	UNIXTIME		ulModificationTime;

	HUINT64			ullSize;
	HUINT64			ullBlocks;

	HCHAR			szName[MAX_HXDIR_FILENAME_LENGTH];
} HLIB_DIR_Item_t;

struct	tagHxDIR
{
	DIR		*pstOSDir;
	HCHAR	szPath[MAX_HXDIR_PATHNAME_LENGTH];

	HCHAR			szTempName[MAX_HXDIR_PATHNAME_LENGTH + MAX_HXDIR_FILENAME_LENGTH];
	HLIB_DIR_Item_t	stItem;
};
#elif	defined(OS_WIN32)
typedef struct tagHLIB_DIR_item
{
	WIN32_FIND_DATA	pstDirent;
	HINT32			stStat;

	UNIXTIME		ulCreateTime;
	UNIXTIME		ulAccessTime;
	UNIXTIME		ulModificationTime;

	HUINT32			ulHiSize;
	HUINT32			ulLowSize;
	HUINT64			ullSize;
	HUINT64			ullBlocks;

	HCHAR			szName[MAX_HXDIR_FILENAME_LENGTH];
} HLIB_DIR_Item_t;

struct	tagHxDIR
{
	HANDLE	pstOSDir;
	HCHAR	szPath[MAX_HXDIR_PATHNAME_LENGTH];

	HCHAR			szTempName[MAX_HXDIR_PATHNAME_LENGTH + MAX_HXDIR_FILENAME_LENGTH];
	HLIB_DIR_Item_t	stItem;
};
#endif

#define ISSLASH(_c) ('/' == (_c))
#define DOT_OR_DOTDOT(_basename)	\
	((_basename[0] == '.') &&		\
	 (_basename[1] == '\0' || (_basename[1] == '.' && _basename[2] == '\0')))


/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___OS_Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
STATIC HERROR	hlib_dir_Open(HxDIR_t *pstDir);
STATIC HERROR	hlib_dir_Next(HxDIR_t *pstDir);
STATIC HERROR	hlib_dir_Stat(HxDIR_t *pstDir);
STATIC HERROR	hlib_dir_Close(HxDIR_t *pstDir);



#if defined(OS_LINUX)
/* OS_LINUX */
typedef DIR OS_DIR_t;

STATIC HERROR	hlib_dir_Open(HxDIR_t *pstDir)
{
	pstDir->pstOSDir = opendir(pstDir->szPath);
	if (pstDir->pstOSDir == NULL)
	{
		HxLOG_Error("open dir failed : [%s]\n", pstDir->szPath);
		return ERR_FAIL;
	}

	pstDir->stItem.pstDirent = readdir(pstDir->pstOSDir);
	if (pstDir->stItem.pstDirent == NULL)
	{
		HxLOG_Error("read dir failed! [%s]\n", pstDir->szPath);
		closedir(pstDir->pstOSDir);

		return ERR_FAIL;
	}

	return hlib_dir_Stat(pstDir);
}

STATIC HERROR	hlib_dir_Next(HxDIR_t *pstDir)
{
	pstDir->stItem.pstDirent = readdir(pstDir->pstOSDir);
	if (pstDir->stItem.pstDirent == NULL)
		return ERR_FAIL;

	return ERR_OK;
}

STATIC HERROR	hlib_dir_Close(HxDIR_t *pstDir)
{
	closedir(pstDir->pstOSDir);

	return ERR_OK;
}

STATIC HERROR	hlib_dir_Stat(HxDIR_t *pstDir)
{
	HLIB_STD_StrNCpySafe(pstDir->stItem.szName, pstDir->stItem.pstDirent->d_name, MAX_HXDIR_FILENAME_LENGTH);
	HxSTD_snprintf(pstDir->szTempName, MAX_HXDIR_PATHNAME_LENGTH + MAX_HXDIR_FILENAME_LENGTH, "%s/%s", pstDir->szPath, pstDir->stItem.szName);

	if (lstat(pstDir->szTempName, &pstDir->stItem.stStat) == 0)
	{
		pstDir->stItem.ulCreateTime			= pstDir->stItem.stStat.st_ctime;
		pstDir->stItem.ulAccessTime			= pstDir->stItem.stStat.st_atime;
		pstDir->stItem.ulModificationTime	= pstDir->stItem.stStat.st_mtime;
		pstDir->stItem.ullSize				= pstDir->stItem.stStat.st_size;
		pstDir->stItem.ullBlocks			= pstDir->stItem.stStat.st_blocks;
	}
	else
	{
		HxLOG_Critical("lstat error [%s][%s]\n", pstDir->szTempName, strerror(errno));
		pstDir->stItem.ulCreateTime			= 0;
		pstDir->stItem.ulAccessTime			= 0;
		pstDir->stItem.ulModificationTime	= 0;
		pstDir->stItem.ullSize				= 0;
		pstDir->stItem.ullBlocks			= 0;
	}

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

STATIC	HBOOL	hlib_dir_IsFile(HxDIR_t *pstDir)
{
	return S_ISREG(pstDir->stItem.stStat.st_mode);
}

STATIC	HBOOL	hlib_dir_IsDir(HxDIR_t *pstDir)
{
	return S_ISDIR(pstDir->stItem.stStat.st_mode);
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

STATIC HERROR	hlib_dir_Access(const HCHAR *name)
{
	if (access(name, F_OK) < 0)
		return ERR_FAIL;

	return ERR_OK;
}

STATIC		HERROR	hlib_dir_MakeDIR(const HUINT8 *name)
{
	HINT32 nRet = 0;

	/* 반드시 0755로 설정해야 한다. 그렇지 않으면 bit mask값이 틀려서 엉뚱한 permission이 적용된다. by Moon */
	nRet = mkdir(name, 0755);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}
STATIC		HERROR	hlib_dir_Chmod(const HUINT8 *name, HUINT32 ulMode)
{
	HINT32 nRet = 0;

	nRet = chmod(name, ulMode);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#elif defined(OS_WIN32)
/* OS_WIN32 */
typedef HANDLE	OS_DIR_t;


STATIC HERROR	hlib_dir_Open(HxDIR_t *pDir)
{
	HCHAR szPath[MAX_HXDIR_FILENAME_LENGTH] = {0, };

	snprintf(szPath, MAX_HXDIR_FILENAME_LENGTH, "%s/*.*", pDir->szPath);

	pDir->pstOSDir = FindFirstFile(szPath, &pDir->stItem.pstDirent);
	if(pDir->pstOSDir == (HANDLE)0xffffffff)
	{
		return ERR_FAIL;
	}

	return hlib_dir_Stat(pDir);
}

STATIC HERROR	hlib_dir_Next(HxDIR_t *pDir)
{
	HBOOL bISNext = FALSE;

	bISNext = FindNextFile(pDir->pstOSDir, &pDir->stItem.pstDirent);
	if(bISNext == FALSE)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


STATIC HERROR	hlib_dir_Close(HxDIR_t *pDir)
{
	FindClose(pDir->pstOSDir);
	return ERR_OK;
}


STATIC HERROR	hlib_dir_Stat(HxDIR_t *pDir)
{
	strcpy(pDir->stItem.szName, pDir->stItem.pstDirent.cFileName);
	pDir->stItem.stStat 			= pDir->stItem.pstDirent.dwFileAttributes;

	pDir->stItem.ulCreateTime		= pDir->stItem.pstDirent.ftCreationTime.dwLowDateTime;
	pDir->stItem.ulAccessTime		= pDir->stItem.pstDirent.ftLastAccessTime.dwLowDateTime;
	pDir->stItem.ulModificationTime	= pDir->stItem.pstDirent.ftLastWriteTime.dwLowDateTime;

	pDir->stItem.ulHiSize			= pDir->stItem.pstDirent.nFileSizeHigh;
	pDir->stItem.ulLowSize			= pDir->stItem.pstDirent.nFileSizeLow;

	pDir->stItem.ullSize			= (HUINT64)(pDir->stItem.ulHiSize<<32 | pDir->stItem.ulLowSize);
	pDir->stItem.ullBlocks			= (HUINT64)(pDir->stItem.ulHiSize<<32 | pDir->stItem.ulLowSize);

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

STATIC	HBOOL	hlib_dir_IsFile(HxDIR_t *pDir)
{
	return ((pDir->stItem.stStat & FILE_ATTRIBUTE_DIRECTORY) > 0) ? FALSE : TRUE;
}

STATIC	HBOOL	hlib_dir_IsDir(HxDIR_t *pDir)
{
	return ((pDir->stItem.stStat & FILE_ATTRIBUTE_DIRECTORY) > 0) ? TRUE : FALSE;
}


STATIC		HERROR	hlib_dir_Access(const HUINT8 *name)
{
#if !defined(CONFIG_TEMP_RUN_EMULATOR)
	HINT32 nRet = 0;
#endif

#if !defined(CONFIG_TEMP_RUN_EMULATOR)
	nRet = access(name);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

STATIC		HERROR	hlib_dir_MakeDIR(const HUINT8 *name)
{
	HINT32 nRet = 0;

	nRet = mkdir(name);
	if(nRet < 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC		HERROR	hlib_dir_Chmod(const HUINT8 *name, HUINT32 ulMode)
{
	// not implemented function...

	return ERR_OK;
}

#else
#error ??

#endif


/* *************************************************************************************************** */
#define ___COMMON_Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */


STATIC		HLIB_DIR_FindOpt_e		hlib_dir_GetFindOptByStr(HUINT8 *pszOption)
{
	HLIB_DIR_FindOpt_e	eFindOpt = eFindOpt_None;

	if(pszOption == NULL || *pszOption == 0)
	{
		return eFindOpt_None;
	}

	while(*pszOption)
	{
		switch(*pszOption)
		{
		case 'R':		eFindOpt |= eFindOpt_Retrieve;			break;
		case 'H':		eFindOpt |= eFindOpt_Hidden;			break;
		case 'S':		eFindOpt |= eFindOpt_System;			break;
		case 'O':		eFindOpt |= eFindOpt_Hidden;			break;
		case 'A':		eFindOpt |= eFindOpt_ALL;				break;
		case 'D':		eFindOpt |= (eFindOpt_Start_Directory | eFindOpt_End_Directory);			break;
		case 'T':		eFindOpt |= eFindOpt_Start_Directory;	break;
		case 'N':		eFindOpt |= eFindOpt_End_Directory;		break;

		default:
			break;
		}

		pszOption++;
	}

	return eFindOpt;
}
/*
STATIC			HINT32	hlib_dir_Find_GetOnlyFilename(HUINT8 *pszStartFilename, HUINT8 *pszEndFilename)
{
	return 0;
}
*/
STATIC			HBOOL	hlib_dir_Find_IsMatch(HUINT8 *pszName, HUINT8 *pszFilter, HLIB_DIR_FindOpt_e	eFindOpt)
{
	HCHAR	*pszFileExt = NULL;
	HCHAR	*pszFilterExt = NULL;
	//HUINT8	*pszWildChar = NULL;
	HINT32	nFilenameLen = 0;


	HxLOG_Assert(pszName);

	// filter 가 null이면 *.* 과 동일하게 동작
	if(pszFilter == NULL)
	{
		return TRUE;
	}

	pszFileExt 		= strrchr((char*)pszName, '.');
	pszFilterExt	= strrchr((char*)pszFilter, '.');

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
	else if(strncasecmp(pszFilterExt, pszFileExt, strlen(pszFilterExt)) == 0)
	{
		;
	}
	else
	{
		return FALSE;
	}

	/* file name */
	//pszWildChar = strchr(pszFilter, '*');

	if(pszFilterExt)
	{
		nFilenameLen = (HUINT8*)pszFilterExt-pszFilter;
	}
	else
	{
		nFilenameLen = strlen((char*)pszFilter);
	}

	if(pszFilter[0] == '*')
	{
		;
	}
	else if(strncasecmp((char*)pszName, (char*)pszFilter, nFilenameLen) == 0)
	{
		;
	}
	else
	{
		return FALSE;
	}




	return TRUE;
}

STATIC			HBOOL	hlib_dir_IgnoreName(HUINT8 *pszDirName, HBOOL bFind)
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

	return FALSE;
}

STATIC			HINT32	hlib_dir_Find(HLIB_DIR_Find_t *pstFind, HUINT8	*pszPath)
{
	HxDIR_t		*pDir = NULL;
	HINT32		nFilterFile = 0;
	HBOOL		bIsMatch = FALSE;
	HUINT8		*pszName = NULL;
	HUINT8		szFullPath[MAX_HXDIR_FILENAME_LENGTH] = {0,};

	pDir = HLIB_DIR_Open((const HCHAR*)pszPath);
	if(pDir == NULL)
	{
		return 0;
	}

	do
	{
		pszName = (HUINT8*)HLIB_DIR_GetName(pDir);
		if(hlib_dir_IgnoreName(pszName, TRUE) == TRUE)
		{
			continue;
		}

		bIsMatch = FALSE;
		if(HLIB_DIR_IsDir(pDir))
		{
			if((pstFind->eFindOpt & eFindOpt_Start_Directory))
			{
				bIsMatch = hlib_dir_Find_IsMatch(pszName, pstFind->pszFilter, pstFind->eFindOpt);
				if(bIsMatch == TRUE)
				{
					nFilterFile++;
					pstFind->bIsEnd = pstFind->cbStartDir(pDir, HLIB_DIR_GetPath(pDir), HLIB_DIR_GetName(pDir), TRUE, pstFind->pUserdata);
				}
			}

			if((pstFind->eFindOpt & eFindOpt_Retrieve) == 0)
			{
				continue;
			}
			snprintf((char*)szFullPath, MAX_HXDIR_FILENAME_LENGTH, "%s/%s", HLIB_DIR_GetPath(pDir), pszName);
			nFilterFile += hlib_dir_Find(pstFind, szFullPath);

			if(pstFind->cbEndDir && (pstFind->eFindOpt & eFindOpt_End_Directory))
			{
				pstFind->bIsEnd = pstFind->cbEndDir(pDir, HLIB_DIR_GetPath(pDir), HLIB_DIR_GetName(pDir), TRUE, pstFind->pUserdata);
			}

		}
		else if(HLIB_DIR_IsFile(pDir))
		{
			bIsMatch = hlib_dir_Find_IsMatch(pszName, pstFind->pszFilter, pstFind->eFindOpt);
			if(bIsMatch == TRUE)
			{
				nFilterFile++;
				pstFind->bIsEnd = pstFind->cbFile(pDir, HLIB_DIR_GetPath(pDir), HLIB_DIR_GetName(pDir), FALSE, pstFind->pUserdata);
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

	} while(HLIB_DIR_Next(pDir) == ERR_OK);

	HLIB_DIR_Close(pDir);

	return nFilterFile;
}

STATIC	HERROR	hlib_dir_MakeDIR_P(HCHAR *pszPath)
{
	HCHAR	*p = pszPath;

	while((p = strchr(p + 1, '/')) != NULL)
	{
		*p = 0;

		if (hlib_dir_Access((HUINT8*)pszPath) == ERR_OK)
		{
			*p = '/';
			continue;
		}

		if (hlib_dir_MakeDIR((HUINT8*)pszPath) != ERR_OK)
		{
			HxLOG_Error("makedir failed! [%s]\n", pszPath);
			return ERR_FAIL;
		}

		*p = '/';
	}

	return hlib_dir_MakeDIR((HUINT8*)pszPath);
}

STATIC HERROR	hlib_dir_Delete(const HCHAR *pszPath)
{
	HxDIR_t 	*pDir = NULL;
	HINT32		nRet = ERR_FAIL;
	HCHAR		*pszName = NULL;

	pDir = HLIB_DIR_Open(pszPath);
	if(pDir == NULL)
	{
		return nRet;
	}

	do
	{
		pszName = HLIB_DIR_GetName(pDir);
		if(hlib_dir_IgnoreName((HUINT8*)pszName, FALSE) == TRUE)
		{
			continue;
		}

		HxSTD_snprintf(pDir->szTempName, MAX_HXDIR_FILENAME_LENGTH + MAX_HXDIR_PATHNAME_LENGTH, "%s/%s", HLIB_DIR_GetPath(pDir), pszName);
		if(HLIB_DIR_IsDir(pDir))
		{
			hlib_dir_Delete(pDir->szTempName);
			nRet = rmdir(pDir->szTempName);
		}
		else
		{
			nRet = HLIB_FILE_Delete((HUINT8*)pDir->szTempName);
		}
		if(nRet < 0)
		{
			nRet = ERR_FAIL;
			goto FUNC_OUT;
		}
	} while(HLIB_DIR_Next(pDir) == ERR_OK);

FUNC_OUT:
	HLIB_DIR_Close(pDir);

	return ERR_OK;
}


STATIC HUINT32	hlib_dir_GetTotalFileSize(const HCHAR *pszPath)
{
	HxDIR_t 	*pDir = NULL;
	HUINT8		*pszName = NULL;
	HCHAR		szFullPath[MAX_HXDIR_FILENAME_LENGTH] = {0,};
	HUINT32		ulTotal = 0;

	pDir = HLIB_DIR_Open(pszPath);
	HxLOG_Print("Path name : %s\n", pszPath);
	if(pDir == NULL)
	{
		return 0;
	}

	do
	{
		pszName = (HUINT8*)HLIB_DIR_GetName(pDir);
		if(hlib_dir_IgnoreName(pszName, TRUE) == TRUE)
		{
			continue;
		}

		if(HLIB_DIR_IsDir(pDir) == TRUE)
		{
			snprintf(szFullPath, MAX_HXDIR_FILENAME_LENGTH, "%s/%s", HLIB_DIR_GetPath(pDir), pszName);
			ulTotal += hlib_dir_GetTotalFileSize(szFullPath);
		}
		else if(HLIB_DIR_IsFile(pDir) == TRUE)
		{
			HxLOG_Print("\tFile name : %s/%s : %u\n", pszPath, pszName, (HUINT32)pDir->stItem.ullBlocks);
			ulTotal += (HUINT32)pDir->stItem.ullBlocks;
		}
		else
		{
			/* skip file */
		}
	} while(HLIB_DIR_Next(pDir) == ERR_OK);

	HLIB_DIR_Close(pDir);

	HxLOG_Print("Total : %u\n", ulTotal);
	return ulTotal;
}

static char *hlib_dir_getFullPath(const char *s1, const char *s2)
{
	char *fullpath, *p;
	size_t len;

	len = HxSTD_StrLen(s1) + HxSTD_StrLen(s2) + 2;

	if ( (fullpath = (char *)HLIB_STD_MemAlloc(len)) == NULL )
	{
		HxLOG_Error("[%s,%d] failed to alloc memory.\n" , __FUNCTION__ , __LINE__);
		return NULL;
	}

	HxSTD_StrNCpy(fullpath, s1, HxSTD_StrLen(s1));

	p = fullpath + HxSTD_StrLen(s1) - 1;
	while ( ISSLASH(*p) )
		p--;
	*++p = '/';
	*++p = '\0';

	HxSTD_StrCpy(p, s2);

	return fullpath;
}


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

HxDIR_t*	HLIB_DIR_Open(const HCHAR	*pszPath)
{
	HxDIR_t		*pstNew = NULL;

	HxLOG_Assert(pszPath);

	pstNew = (HxDIR_t*)HLIB_MEM_Calloc(sizeof(HxDIR_t));
	HxLOG_Assert(pstNew);

	HLIB_STD_StrNCpySafe(pstNew->szPath, pszPath, MAX_HXDIR_PATHNAME_LENGTH);
	if (hlib_dir_Open(pstNew) == ERR_OK)
		return pstNew;

	HLIB_MEM_Free(pstNew);
	HxLOG_Error("fail to open directory (%s)\n", pszPath);

	return NULL;
}

HERROR		HLIB_DIR_Next(HxDIR_t *pSelf)
{
	HxLOG_Assert(pSelf);

	if (hlib_dir_Next(pSelf) != ERR_OK)
		return ERR_FAIL;

	return hlib_dir_Stat(pSelf);
}

HERROR		HLIB_DIR_Close(HxDIR_t *pSelf)
{
	HERROR	hError = ERR_OK;

	HxLOG_Assert(pSelf);

	hError = hlib_dir_Close(pSelf);
	HLIB_MEM_Free(pSelf);

	return hError;
}

HCHAR*		HLIB_DIR_GetName(HxDIR_t *pSelf)
{
	return pSelf->stItem.szName;
}

HCHAR*		HLIB_DIR_GetPath(HxDIR_t *pSelf)
{
	return pSelf->szPath;
}

HBOOL		HLIB_DIR_IsDir(HxDIR_t *pSelf)
{
	return hlib_dir_IsDir(pSelf);
}

HBOOL		HLIB_DIR_IsFile(HxDIR_t *pSelf)
{
	return hlib_dir_IsFile(pSelf);
}

HBOOL		HLIB_DIR_IsExist(const HCHAR *pszDirName)
{
#ifdef	OS_LINUX
	if (access(pszDirName, F_OK) == 0)
		return TRUE;

	return FALSE;
#else
	return TRUE;
#endif
}

HERROR	HLIB_DIR_IsValid(const HCHAR *name)
{
#ifdef	OS_LINUX
	struct stat		st;

	if (lstat(name, &st) == 0)
	{
		return ERR_OK;
	}
	return ERR_FAIL;
#else
	LOG("[%s:%d] Unimplemented function\r\n", __FUNCTION__, __LINE__);
	return ERR_OK;
#endif
}

HBOOL		HLIB_DIR_IsDirPath(HCHAR *pszDirName)
{
#ifdef	OS_LINUX
	struct stat		st;

	if (lstat(pszDirName, &st) == 0)
	{
		if (st.st_mode & S_IFDIR)
			return TRUE;
	}

	return FALSE;
#else
	LOG("[%s:%d] Unimplemented function\r\n", __FUNCTION__, __LINE__);
	return ERR_OK;
#endif
}

HINT32		HLIB_DIR_Find(HUINT8	*pszPath, HUINT8 *pszFilter, HUINT8 *pszOption,
						HBOOL (* func)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata), void *userdata)
{
	return HLIB_DIR_FindEx(pszPath, pszFilter, pszOption, func, func, NULL, userdata);
}

HINT32		HLIB_DIR_FindEx(HUINT8	*pszPath,
								HUINT8 *pszFilter,
								HUINT8 *pszOption,
								HBOOL (* cbfile)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata),
								HBOOL (* cbstartdir)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata),
								HBOOL (* cbenddir)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata),
								void *userdata)
{
	HLIB_DIR_Find_t stFind;

	LOG("[%s:%d] ====> \npszPath(%s)\npszFilter(%s)\npszOption(%s)\ncbfile(0x%x) cbfile(0x%x) cbstartdir(0x%x) cbenddir(0x%x)",
			__FUNCTION__, __LINE__, pszPath, pszFilter, pszFilter, pszOption, cbfile, cbstartdir, cbenddir, userdata);

	HxSTD_MemSet(&stFind, 0, sizeof(HLIB_DIR_Find_t));

	stFind.pszFilter	= pszFilter;
	stFind.bIsEnd		= FALSE;
	stFind.eFindOpt 	= hlib_dir_GetFindOptByStr(pszOption);

	stFind.cbFile		= cbfile;
	stFind.cbStartDir	= cbstartdir;
	stFind.cbEndDir		= cbenddir;
	stFind.pUserdata	= userdata;

	return hlib_dir_Find(&stFind, pszPath);
}

HERROR		HLIB_DIR_Make(const HCHAR *pszPath)
{
	HCHAR	*pszDup = NULL;
	HERROR	err;

	HxLOG_Assert(pszPath);

	if (hlib_dir_MakeDIR((HUINT8*)pszPath) == ERR_OK)
		return ERR_OK;

	pszDup = HLIB_MEM_StrDup_CB(pszPath);
	err = hlib_dir_MakeDIR_P(pszDup);
	HLIB_MEM_Free(pszDup);

	return err;
}

// ulMode : 0755, 0777 등 8진수로 값을 넣어줘야한다.
HERROR	HLIB_DIR_Chmod(const HCHAR *pszPath, HUINT32 ulMode)
{
	HxLOG_Assert(pszPath);

	if (hlib_dir_Chmod((HUINT8*)pszPath, ulMode) == ERR_OK)
		return ERR_OK;

	return ERR_FAIL;
}

HERROR		HLIB_DIR_Move(const HCHAR *pszOldPath, const HCHAR *pszNewPath)
{
	HxLOG_Assert(pszOldPath && pszNewPath);

	if (hlib_dir_Access((HUINT8*)pszOldPath) != ERR_OK)
		return ERR_FAIL;

	if (rename(pszOldPath, pszNewPath) < 0)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR		HLIB_DIR_Delete(const HCHAR *pszPath)
{
	if (hlib_dir_Delete(pszPath) == ERR_OK)
	{
		if (rmdir(pszPath) < 0)
			return ERR_FAIL;
	}

	return ERR_OK;
}

// pszPath1 : src
// pszPath2 : dst
HERROR		HLIB_DIR_Copy(const HCHAR *pszPath1, const HCHAR *pszPath2)
{
#ifdef	OS_LINUX
	struct stat fs, dst_st;
	char *fullpath = NULL, *dstpath = NULL;

	if ( pszPath1 == NULL || pszPath2 == NULL )
	{
		HxLOG_Error("NULL parameter came in.\n" );
		return ERR_FAIL;
	}

	if ( stat(pszPath1, &fs) == -1 )
	{
		HxLOG_Error("failed to get information about %s.\n" , pszPath1);
		return ERR_FAIL;
	}

	if ( S_ISDIR(fs.st_mode) )
	{
		OS_DIR_t *dirp;
		struct dirent *dp;

		if ( (dirp = opendir(pszPath1)) == NULL )
		{
			HxLOG_Error("failed to open directory about %s.\n" , pszPath1);
			return ERR_FAIL;
		}

		while ( (dp = readdir(dirp)) != NULL )
		{
			if ( DOT_OR_DOTDOT(dp->d_name) )
				continue;

			if ( (fullpath = hlib_dir_getFullPath(pszPath1, dp->d_name)) == NULL )
				continue;

			if ( (dstpath = hlib_dir_getFullPath(pszPath2, dp->d_name)) == NULL )
			{
				HLIB_STD_MemFree(fullpath);
				fullpath = NULL;
				continue;
			}

			if ( stat(fullpath, &dst_st) == 0 )
			{
				if ( S_ISDIR(dst_st.st_mode) )
				{
					HLIB_DIR_Make(dstpath);

					if ( HLIB_DIR_Copy(fullpath, dstpath) != TRUE )	// recursive copy
					{
						HxLOG_Error("error occurred in subdirectory.\n");
						HLIB_STD_MemFree(dstpath);
						HLIB_STD_MemFree(fullpath);
						closedir(dirp);
						return ERR_FAIL;
					}
				}
				else
				{
					if ( HLIB_FILE_Copy((HUINT8*)fullpath, (HUINT8*)dstpath) != ERR_OK )
					{
						HxLOG_Error("failed to copy a file(%s).\n", fullpath);
					}
				}
			}

			HLIB_STD_MemFree(dstpath);
			dstpath = NULL;
			HLIB_STD_MemFree(fullpath);
			fullpath = NULL;
		}

		closedir(dirp);
	}

	return ERR_OK;
#else
	LOG("[%s:%d] Unimplemented function\r\n", __FUNCTION__, __LINE__);
	return ERR_OK;
#endif
}

HERROR 		HLIB_DIR_GetTime(HxDIR_t *pSelf, UNIXTIME *pulCreateTime, UNIXTIME *pulAccessTime, UNIXTIME *pulModificationTime)
{
	if(pSelf == NULL)
		return ERR_FAIL;

	if(pulCreateTime)
		*pulCreateTime = pSelf->stItem.ulCreateTime;

	if(pulAccessTime)
		*pulAccessTime = pSelf->stItem.ulAccessTime;

	if(pulModificationTime)
		*pulModificationTime = pSelf->stItem.ulModificationTime;

	return ERR_OK;
}

HERROR		HLIB_DIR_GetFileSize(HxDIR_t *pSelf, HUINT64 *pullSize)
{
	if(pSelf == NULL)
		return ERR_FAIL;

	if (pullSize)
		*pullSize = pSelf->stItem.ullSize;

	return ERR_OK;
}

HERROR		HLIB_DIR_GetTotalSize(const HCHAR *pszPath, HUINT32 *pulSize)
{
	if(pszPath == NULL)
		return ERR_FAIL;

	HxLOG_Print("[HLIB_DIR_GetTotalSize]\n");

	if (pulSize)
	{
		*pulSize = hlib_dir_GetTotalFileSize(pszPath);
	}

	return ERR_OK;
}

HBOOL		HLIB_DIR_IsDirectoryEmpty(const HCHAR *pszPath)
{
#if defined(OS_WIN32)
	// TODO :
	return TRUE;
#else
	int n = 0;
	OS_DIR_t	*dir;


	dir = opendir(pszPath);

	if (dir == NULL) //Not a directory or doesn't exist
		return 1;

	while (readdir(dir) != NULL)
	{
		if(++n > 2)
		break;
	}

	closedir(dir);

	if (n <= 2) //Directory Empty
		return TRUE;
	else
		return FALSE;
#endif
}

/*
// pSelf or pFstat 둘 중 하나만 인자로 넣어주어도 동작하도록 구현함.
// This API supports both types of HxDIR_t and struct stat.
// Please use argument pSelf or pFstat.
*/
HUINT64		HLIB_DIR_GetFileUniqueId(HxDIR_t *pSelf, struct stat *pFstat)
{
	HUINT64 ullUid = 0;

#if defined(OS_LINUX)
	struct stat stStat;

	if (pSelf)
	{
		stStat.st_dev = pSelf->stItem.stStat.st_dev;
		stStat.st_ino = pSelf->stItem.stStat.st_ino;
	}
	else if (pFstat)
	{
		stStat.st_dev = pFstat->st_dev;
		stStat.st_ino = pFstat->st_ino;
	}
	else
	{
		HxLOG_Error("Both pSelf and pFstat is NULL\n");
		return 0;
	}

	// inode value size : 32bit
	if (sizeof(stStat.st_ino) <= 4)
	{
		ullUid = (((HUINT64)stStat.st_dev << 32) & 0xffffffff00000000) | (stStat.st_ino);
		return ullUid;
	}
	else	// 64bit
	{
		HUINT8 ucData[8];
		HUINT32 ulCrc32 = 0;

		// example
		// st_ino : 0x000000001f2f3f4f
		// ucData : 4f 3f 2f 1f 00 00 00 00 (little endian)
		HxSTD_memcpy(ucData, &stStat.st_ino, 8);

		ulCrc32 = HLIB_MATH_GetCrc32((void *)ucData, 8);
		ullUid = (((HUINT64)stStat.st_dev << 32) & 0xffffffff00000000) | ulCrc32;


		HxLOG_Debug("st_dev : 0x%x, st_ino : 0x%llx\n", stStat.st_dev, stStat.st_ino);
		HxLOG_Debug("Input: %x %x %x %x %x %x %x %x\n", ucData[0], ucData[1], ucData[2], ucData[3], ucData[4], ucData[5],
																	ucData[6], ucData[7]);
		HxLOG_Debug("UID(CRC64) : 0x%llx\n", ullUid);

		return ullUid;
	}

#else
	if (pSelf)			{		ullUid = (HUINT64)pSelf;	}
	else if (pFstat)	{		ullUid = (HUINT64)pFstat;	}
	else
	{
		HxLOG_Error("Both pSelf and pFstat is NULL\n");
		ullUid = 0;
	}

	return ullUid;
#endif

	return 0;
}

HBOOL		HLIB_DIR_IsMountedPath (const HCHAR *path)
{
#ifdef	OS_LINUX
	FILE *fp;

	fp = fopen("/proc/mounts", "r");
	if (fp)
	{
		struct stat st;
		HCHAR	line[512];
		HCHAR	devname[81];
		HCHAR	mountdir[81];
		HCHAR	fstype[13];
		HCHAR	mntopts[81];
		HBOOL	bIsHdd = FALSE;

		// IsMountedPath의 인자값으로 넘어오는 path가 UBI 영역일 수도, HDD 일 수도..
		// path가 HDD 경로 인 경우 BLOCK Device 인지 Check 해줘야 함.
		if(HLIB_STD_StrStartWith(path, "/mnt"))
		{
			bIsHdd = TRUE;
		}

		if (bIsHdd == TRUE)
		{
			while (fgets(line, 511, fp))
			{
				sscanf(line, "%80s%80s%12s%80s", devname, mountdir, fstype, mntopts);

				if ( (HxSTD_StrCmp(mountdir, "/") != 0) && (HLIB_STD_StrStartWith(path, mountdir) == TRUE) &&  (0 == stat(devname, &st)) && S_ISBLK(st.st_mode) )
				{
					fclose(fp);
					return TRUE;
				}
			}
		}
		else
		{
			while (fgets(line, 511, fp))
			{
				sscanf(line, "%80s%80s%12s%80s", devname, mountdir, fstype, mntopts);
				if ( (HxSTD_StrCmp(mountdir, "/") != 0) && (HLIB_STD_StrStartWith(path, mountdir) == TRUE))
				{
					fclose(fp);
					return TRUE;
				}
			}
		}
		fclose(fp);
	}
	return FALSE;
#else
	(void)path;
	return TRUE;
#endif
}


HERROR hlib_dir_GetMountInfo(const HCHAR *path, HCHAR *pszMountPath, HCHAR *pszDevName, HCHAR *pszFsType,HCHAR *pszMntOpts, HINT32 nLen)
{
#ifdef	OS_LINUX
	struct stat st;
	HCHAR line[256];
	HCHAR devname[81];
	HCHAR mountdir[81];
	HCHAR fstype[13];
	HCHAR mntopts[81];
	FILE *fp;

	if (fp = fopen("/proc/mounts", "r"), NULL == fp) {
		HxLOG_Error("cannot read \"/proc/mounts\"\n");
		return ERR_FAIL;
	}
	while (fgets(line, 255, fp))
	{
		sscanf(line, "%80s%80s%12s%80s", devname, mountdir, fstype, mntopts);
		if ( (HxSTD_StrCmp(mountdir, "/") != 0) && (HLIB_STD_StrStartWith(path, mountdir) == TRUE) &&  (0 == stat(devname, &st)) && S_ISBLK(st.st_mode) )
		{
			if(pszMountPath != NULL)				HLIB_STD_StrNCpySafe(pszMountPath, mountdir, nLen);
			if(pszDevName != NULL)					HLIB_STD_StrNCpySafe(pszDevName, devname, nLen);
			if(pszFsType != NULL)					HLIB_STD_StrNCpySafe(pszFsType, fstype, nLen);
			if(pszMntOpts != NULL)					HLIB_STD_StrNCpySafe(pszMntOpts, mntopts, nLen);

			fclose(fp);
			return ERR_OK;
		}
	}
	fclose(fp);
#endif
	return ERR_FAIL;
}


HERROR	HLIB_DIR_GetMountPath(const HCHAR *path, HCHAR *pszMountPath, HINT32 nLen)
{
	return hlib_dir_GetMountInfo(path,pszMountPath,NULL,NULL,NULL,nLen);
}


HERROR	HLIB_DIR_GetMountFstype(const HCHAR *path, HCHAR *pszFsType, HINT32 nLen)
{
	return hlib_dir_GetMountInfo(path,NULL,NULL,pszFsType,NULL, nLen);
}

HERROR	HLIB_DIR_GetMountOpts(const HCHAR *path, HCHAR *pszOpts, HINT32 nLen)
{
	return hlib_dir_GetMountInfo(path, NULL, NULL, NULL, pszOpts, nLen);
}

