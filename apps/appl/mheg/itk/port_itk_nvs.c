
/**
	@file	  port_itk_nvs.c														\n
	@brief	  itk file for Nvs porting layer							\n

	Description: ITK에서 Nvs Porting 에 사용되는 기능을 정의한 Body파일	\n
	Module: mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n

	[Description]

*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include "port_itk_main.h"
#include "port_itk_nvs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ITK_NVS_DIR		"/var/lib/humaxtv_user"
#define ITK_NVS_SEARCH_FILE_WHEN_GET_TIME

/*
	Factory reset할때나 Search할때 아래 pre-define을 활성화 시키면 모든 FS를 삭제한다.
	2010.02.22. NVM을 지우면 이전에 가지고 있던 FS iterator를 engine이 계속 사용하려 해서 문제가 된다.
	FSA는 삭제하면 engine이 새로 생성하는 시도를 한다. 그래서 NVM은 삭제하지 않고 FSA만 삭제한다.
*/
//#define REMOVE_ALL_NVS_FS_WHEN_FACTORY_RESET

#define ITK_NVS_FS_NVM_BASE_PATH	ITK_NVS_DIR"/nvm"		//"/usr/data"
#define ITK_NVS_FS_FSA_BASE_PATH	ITK_NVS_DIR"/fsa"		//"/usr/data"
#define ITK_NVS_FS_RAM_BASE_PATH	ITK_NVS_DIR"/ram"		//"/usr/data"
#define ITK_NVS_FS_PST_BASE_PATH	ITK_NVS_DIR"/pst"		//"/usr/data"

#define ITK_NVS_FS_METAFILE_NAME	"meta.dat"

#define ITK_NVS_FSA_MAGIC_NUMBER		0x78564936

#define ITK_NVS_PATH_MAX				(256)//(1024)
#define ITK_NVS_NAME_LEN				(256)
#define ITK_NVS_FULLPATH_MAX			(1024)
#define ITK_NVS_FS_NAME_MAX			(32)
#define ITK_NVS_METAFILE_HEADER_SIZE	(32)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/**************************************
	NVS FILE
***************************************/
typedef struct ITK_NvsFileHandle_t
{
	char entryName[ITK_NVS_NAME_LEN];
	size_t dataSize;
	size_t usedBytes;
	void * caller;

	struct ITK_NvsFileHandle_t	*pPrev;
	struct ITK_NvsFileHandle_t	*pNext;
} ITK_NvsFileHandle_t;

typedef struct ITK_NvsFileIterator_t
{
	itk_nvs_file_t vtbl;

	struct ITK_NvsFileHandle_t *pItem;
} ITK_NvsFileIterator_t;

/***************************************
	NVS FS
***************************************/
typedef struct ITK_NvsFsHandle_t
{
	itk_nvs_fs_type_t	fsType;
	char fsName[ITK_NVS_PATH_MAX];
	size_t minimumCapacity;
	itk_uint8_t priority;
	size_t metadataSize;
	size_t freeSpace;		/* META DATA를 위한 header size는 제외한 free space */
	size_t totaldataSize;	/* META DATA를 포함한 모든 data size */
	void * caller;

	struct ITK_NvsFsHandle_t	*pPrev;
	struct ITK_NvsFsHandle_t	*pNext;

	HUINT32 ulNvsFileNum;

	struct ITK_NvsFileHandle_t	*pHead;
	struct ITK_NvsFileHandle_t	*pTail;
} ITK_NvsFsHandle_t;

typedef struct ITK_NvsFsIterator_t
{
	itk_nvs_fs_t vtbl;

	struct ITK_NvsFsHandle_t *pItem;
} ITK_NvsFsIterator_t;

/***************************************
	NVS
***************************************/
typedef struct ITK_NvsHandle_t
{
	HUINT32 ulNvsFsNum;

	struct ITK_NvsFsHandle_t	*pHead;
	struct ITK_NvsFsHandle_t	*pTail;
} ITK_NvsHandle_t;

typedef struct ITK_NvsIterator_t
{
	itk_nvs_t vtbl;

	struct ITK_NvsHandle_t *pItem;
} ITK_NvsIterator_t;

typedef struct tagMhegNvsInfo
{
	redkey_profile_t	itkProfile;
	itk_nvs_t			*pObject;
} MHEG_NVS_INFO_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static MHEG_NVS_INFO_t	s_mhegNvsInfo;

/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/
static inline char * mheg_itk_port_nvsAllocDir(const char * p1, const char * p2);
static inline void mheg_itk_port_nvsFreeDir(char * p);
#if 0		// not used
static HBOOL local_itk_NVS_RmDir(const char * dir);
#endif
static HBOOL mheg_itk_port_nvsMkDir(const char * dir);
static HBOOL mheg_itk_port_nvsIsDir(const char * dir);



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static HBOOL mheg_itk_port_nvsCheckValidFsType(itk_nvs_fs_type_t fsType)
{
	HBOOL	bValid = FALSE;
	redkey_profile_t eItkProfile = s_mhegNvsInfo.itkProfile;

	switch (fsType)
	{
		case ITK_NVS_FS_NVM:
			if ( (eItkProfile == REDKEY_PROFILE_UK_FSAT)
				|| (eItkProfile == REDKEY_PROFILE_FREESATG2)
				|| (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
				|| (eItkProfile == REDKEY_PROFILE_SOUTH_AFRICA) )
			{
				bValid = TRUE;
			}
			break;
		case ITK_NVS_FS_FSA:
			if ( (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
				|| (eItkProfile == REDKEY_PROFILE_SOUTH_AFRICA) )
			{
				bValid = TRUE;
			}
			break;
		case ITK_NVS_FS_RAM:
			if ( (eItkProfile == REDKEY_PROFILE_UK_HD)
				|| (eItkProfile == REDKEY_PROFILE_INTERNAL5)
				|| (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
				|| (eItkProfile == REDKEY_PROFILE_NZ_HD)
				|| (eItkProfile == REDKEY_PROFILE_THAI) )
			{
				bValid = TRUE;
			}
			break;
		case ITK_NVS_FS_PST:
			if ( (eItkProfile == REDKEY_PROFILE_UK_HD)
				|| (eItkProfile == REDKEY_PROFILE_INTERNAL5)
				|| (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
				|| (eItkProfile == REDKEY_PROFILE_NZ_HD)
				|| (eItkProfile == REDKEY_PROFILE_THAI) )
			{
				bValid = TRUE;
			}
			break;
		default:
			HxLOG_Error(" NOT IMPLEMEMTED!!! fsType(%d)\n", fsType);
	}

	HxLOG_Debug("fsType is valid (%s)\n", (bValid == TRUE) ? "TRUE" : "FALSE");

	return bValid;
}

static HBOOL mheg_itk_port_nvsGetFullPath(itk_nvs_fs_type_t fsType, char const *fsName, char const *entryName, char *szFullpath, int nBufLen)
{
	if ((fsName == NULL) || (szFullpath == NULL))
	{
		HxLOG_Error(" buffer is NULL!!!\n");
		return FALSE;
	}

	switch (fsType)
	{
		case ITK_NVS_FS_NVM:
			snprintf(szFullpath, nBufLen, "%s", ITK_NVS_FS_NVM_BASE_PATH);
			break;
		case ITK_NVS_FS_FSA:
			snprintf(szFullpath, nBufLen, "%s", ITK_NVS_FS_FSA_BASE_PATH);
			break;
		case ITK_NVS_FS_RAM:
			snprintf(szFullpath, nBufLen, "%s", ITK_NVS_FS_RAM_BASE_PATH);
			break;
		case ITK_NVS_FS_PST:
			snprintf(szFullpath, nBufLen, "%s", ITK_NVS_FS_PST_BASE_PATH);
			break;
		default:
			HxLOG_Error(" NOT IMPLEMEMTED!!! fsType(%d)\n", fsType);
			return FALSE;
	}

	/*
		FS type에 대해 FS name은 unique하다
		따라서 FS name을 folder로 구분후 해당 folder에 FS file을 저장하는 구조로 간다.
	*/
	if (fsName[0] == '/')
	{
		snprintf(szFullpath, nBufLen, "%s%s", szFullpath, fsName);
	}
	else
	{
		snprintf(szFullpath, nBufLen, "%s/%s", szFullpath, fsName);
	}

	if (entryName)
	{
		if (entryName[0] == '/')
		{
			snprintf(szFullpath, nBufLen, "%s%s", szFullpath, entryName);
		}
		else
		{
			snprintf(szFullpath, nBufLen, "%s/%s", szFullpath, entryName);
		}
	}

	return TRUE;
}

static HBOOL mheg_itk_port_nvsCheckDir(char *file)
{
	int nSeperatorPos = 0, i;
	char *dir;

	if (file == NULL)
	{
		HxLOG_Error(" pointer is null!\n");
		return FALSE;
	}

	for (i = 0; i < ITK_NVS_FULLPATH_MAX; i++)
	{
		if (file[i] == '/')
			nSeperatorPos = i;
		if (file[i] == '\0')
			break;
	}

	dir = HLIB_STD_MemAlloc(nSeperatorPos + 1);
	if (dir == NULL)
	{
		HxLOG_Error(" Not enough memory!\n");
		return FALSE;
	}
	memset(dir, 0, nSeperatorPos + 1);
	strncpy(dir, file, nSeperatorPos);

	if (!mheg_itk_port_nvsIsDir(dir))
	{
		mheg_itk_port_nvsMkDir(dir); // make this directory
		HxLOG_Debug(" Make DIR(%s) str size(%d)\n", dir, nSeperatorPos);
	}

	HLIB_STD_MemFree(dir);

	return TRUE;
}

static HBOOL mheg_itk_port_nvsDataSize
(
	char const * file,
	size_t * data_size
)
{
	int fd/*, sz = 0*/;
	struct stat fs;

	if( file == NULL
		|| data_size == NULL )
	{
		HxLOG_Error(" invalid parameters !\n");
		return FALSE;
	}

	fd = open(file, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if( fd < 0 )
	{
		HxLOG_Error(" open error !\n");
		return FALSE;
	}

	if( fstat(fd, &fs) < 0 )
	{
		close(fd);
		HxLOG_Error(" fstat error !\n");
		return FALSE;
	}

	close(fd);

	if( fs.st_size <= ITK_NVS_METADATA_LENGTH )
	{
		HxLOG_Error(" invalid fs.st_size !\n");
		return FALSE;
	}

	*data_size = fs.st_size - ITK_NVS_METADATA_LENGTH;

	HxLOG_Debug(" get data_size in file(%s) !\n", file);

	return TRUE;
}

static HBOOL mheg_itk_port_nvsRead
(
	char const * file,
	itk_uint8_t * metadata,
	size_t partially,
	size_t data_size,
	itk_uint8_t * data
)
{
	int fd, sz = 0;
	struct stat fs;

	if( file == NULL
		|| ( metadata == NULL && data == NULL ) )
	{
		HxLOG_Error(" invalid parameters !\n");
		return FALSE;
	}

	fd = open(file, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if( fd < 0 )
	{
		HxLOG_Error(" open error !\n");
		return FALSE;
	}

	if( fstat(fd, &fs) < 0 )
	{
		close(fd);
		HxLOG_Error(" fstat error !\n");
		return FALSE;
	}

	if( fs.st_size == 0
		|| fs.st_size < ITK_NVS_METADATA_LENGTH )
	{
		close(fd);
		HxLOG_Error(" invalid fs.st_size !\n");
		return FALSE;
	}

	if( metadata )
	{
		int n ;
		n = (int)read(fd, metadata, ITK_NVS_METADATA_LENGTH);
		if(n<=0)
		{
			close(fd);
			return FALSE;
		}
	}

	if( data )
	{
		if( data_size )
		{
			partially += ITK_NVS_METADATA_LENGTH;

			sz = fs.st_size - partially;
			if( sz < (int)data_size )
			{
				if (lseek(fd, partially, SEEK_SET) < 0) {
					HxLOG_Error(" lseek error !\n");
					close(fd);
					return FALSE;
				}

				// prevent 155352
				if(read(fd, data, sz) != sz)
				{
					HxLOG_Error(" read error !\n");
					close(fd);
					return FALSE;
				}
			}
			else
			{
				if (lseek(fd, partially, SEEK_SET) < 0) {
					HxLOG_Error(" lseek error !\n");
					close(fd);
					return FALSE;
				}

				if (read(fd, data, data_size) != (ssize_t)data_size) {
					HxLOG_Error(" lseek error !\n");
					close(fd);
					return FALSE;
				}
			}
		}
	}

	close(fd);

	//HxLOG_Debug(" read data from file(%s) !\n", file);

	return TRUE;
}

static HBOOL mheg_itk_port_nvsWrite
(
	char const * file,
	itk_uint8_t const * metadata,
	size_t partially,
	size_t data_size,
	itk_uint8_t const * data
)
{
	int fd;

	if( file == NULL
		/*|| ( metadata == NULL && data == NULL )*/ )
	{
		HxLOG_Error(" invalid parameters !\n");
		return FALSE;
	}

	fd = open(file, O_WRONLY | O_CREAT | /*O_TRUNC |*/ O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if( fd < 0 )
	{
		HxLOG_Error(" open error ! filename(%s)\n", file);
		return FALSE;
	}

	if( metadata )
	{
		//lseek(fd, 0, SEEK_SET);
		write(fd, metadata, ITK_NVS_METADATA_LENGTH);
	}

	if( data )
	{
		if( data_size )
		{
			partially += ITK_NVS_METADATA_LENGTH;

			if (lseek(fd, partially, SEEK_SET) < 0) {
				HxLOG_Error(" lseek error !\n");
				close(fd);
				return FALSE;
			}

			write(fd, data, data_size);
		}
	}

	close(fd);

	HxLOG_Debug(" write data into file(%s) !\n", file);

	return TRUE;
}

static HBOOL mheg_itk_port_nvsErase
(
	const char * file
)
{
	return (0 == remove(file)) ? TRUE : FALSE;
}

static HBOOL mheg_itk_port_nvsIsReg
(
	const char * reg
)
{
	struct stat fs;
	return (0 == stat(reg, &fs) && S_ISREG(fs.st_mode)) ? TRUE : FALSE;
}

static inline char * mheg_itk_port_nvsAllocDir
(
	const char * p1,
	const char * p2
)
{
	const size_t l1 = strlen(p1);
	const size_t l2 = strlen(p2);
	char * path, * p;

	if (path = p = HLIB_STD_MemAlloc(l1 + l2 + 2),
		NULL == p)
		return NULL;

	strncpy(p, p1, l1);


	for (p = p + l1 - 1; '/' == (*p); p--)
	{
		if(path == p) break;
	}
	*++p = '/';
	*++p = '\0';

	strncpy(p, p2, l2);
	p[l2] = '\0';

	return path;
}

static inline void mheg_itk_port_nvsFreeDir
(
	char * p
)
{
	if (p)	HLIB_STD_MemFree(p);
}

#if 0	// not used
static HBOOL local_itk_NVS_RmDir
(
	const char * dir
)
{
	char * path;
	struct stat fs;

	if (-1 == stat(dir, &fs))
		return FALSE;

	if (S_ISDIR(fs.st_mode))
	{
		DIR * dirp;
		struct dirent * dp;
		HBOOL ok = FALSE;

		if (dirp = opendir(dir),
			NULL == dirp)
			return FALSE;

		while ((dp = readdir(dirp)) != NULL)
		{
			if ((dp->d_name[0] == '.')
				&& (dp->d_name[1] == '\0' || (dp->d_name[1] == '.' && dp->d_name[2] == '\0')))
				continue;

			if (path = mheg_itk_port_nvsAllocDir(dir, dp->d_name),
			    NULL == path)
				continue;

			if (ok = local_itk_NVS_RmDir(path),
				TRUE != ok)
				remove(path);

			mheg_itk_port_nvsFreeDir(path);
		}

		closedir(dirp);
	}

	if (-1 == remove(dir))
		return FALSE;

	return TRUE;
}
#endif

static HBOOL mheg_itk_port_nvsMkDir
(
	const char * dir
)
{
	char *odir, *p;

	odir = p = HLIB_STD_StrDup(dir);
	do
	{
		p = strchr(p + 1, '/');
		if (p)
			*p = '\0';

		if (access(odir, F_OK) == -1)
		{
			if (mkdir(odir, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
			{
				HLIB_STD_MemFree(odir);
				return FALSE;
			}
		}

		if (p)
			*p = '/';
	} while (p);

	HLIB_STD_MemFree(odir);

	return TRUE;
}

static HBOOL mheg_itk_port_nvsIsDir
(
	const char * dir
)
{
	struct stat fs;

	return (0 == stat(dir, &fs) && S_ISDIR(fs.st_mode)) ? TRUE : FALSE;
}

static void mheg_itk_port_nvsFileAppend(ITK_NvsFileHandle_t *pItem,
	ITK_NvsFileHandle_t **ppHead, ITK_NvsFileHandle_t **ppTail)
{
	if ((*ppHead) == NULL)
	{
		(*ppHead) = pItem;
		pItem->pPrev = NULL;
	}
	else
	{
		(*ppTail)->pNext = pItem;
		pItem->pPrev = (*ppTail);
	}
	(*ppTail) = pItem;
	pItem->pNext = NULL;

	HxLOG_Debug(C_CODE_F_WHITE_B_BLUE" APPENDED! pItem(%p) pItem prev(%p) pItem next(%p) pHead(%p) pHead next(%p) pTail(%p) pTail prev(%p) pTail next(%p)"C_CODE_RESET"\n",
			pItem, pItem->pPrev, pItem->pNext, *ppHead, (*ppHead)->pNext, (*ppTail), (*ppTail)->pPrev, (*ppTail)->pNext);
}

static void mheg_itk_port_nvsFileRemove(ITK_NvsFileHandle_t *pItem,
	ITK_NvsFileHandle_t **ppHead, ITK_NvsFileHandle_t **ppTail)
{
	HxLOG_Debug(C_CODE_F_WHITE_B_RED" REMOVED! entryName(%s) pItem(%p)"C_CODE_RESET"\n", pItem->entryName, pItem);

	if (pItem->pPrev == NULL)
	{
		(*ppHead) = pItem->pNext;
	}
	else
	{
		pItem->pPrev->pNext = pItem->pNext;
	}
	if (pItem->pNext == NULL)
	{
		(*ppTail) = pItem->pPrev;
	}
	else
	{
		pItem->pNext->pPrev = pItem->pPrev;
	}
}

static ITK_NvsFileHandle_t *mheg_itk_port_nvmFileHandle(const char *name,
	ITK_NvsFileHandle_t *pHead, ITK_NvsFileHandle_t *pTail)
{
	ITK_NvsFileHandle_t *pItem = NULL;

	(void)(pTail);

	pItem = pHead;

	while (pItem != NULL)
	{
		if (strcmp(pItem->entryName, name) == 0)
		{
			break;
		}

		pItem = pItem->pNext;
	}

	return pItem;
}

static void mheg_itk_port_nvsFsAppend(ITK_NvsFsHandle_t *pItem,
	ITK_NvsFsHandle_t **ppHead, ITK_NvsFsHandle_t **ppTail)
{
	if ((*ppHead) == NULL)
	{
		(*ppHead) = pItem;
		pItem->pPrev = NULL;
	}
	else
	{
		(*ppTail)->pNext = pItem;
		pItem->pPrev = (*ppTail);
	}
	(*ppTail) = pItem;
	pItem->pNext = NULL;

	HxLOG_Debug(C_CODE_F_WHITE_B_PURPLE" APPENDED! fsType(%d) faName(%s)"C_CODE_RESET"\n", pItem->fsType, pItem->fsName);
	HxLOG_Debug(C_CODE_F_WHITE_B_PURPLE" pItem(%p) pItem prev(%p) pItem next(%p) pHead(%p) pHead next(%p) pTail(%p) pTail prev(%p) pTail next(%p)"C_CODE_RESET"\n",
			pItem, pItem->pPrev, pItem->pNext, *ppHead, (*ppHead)->pNext, (*ppTail), (*ppTail)->pPrev, (*ppTail)->pNext);
}

static void mheg_itk_port_nvsFsRemove(ITK_NvsFsHandle_t *pItem,
	ITK_NvsFsHandle_t **ppHead, ITK_NvsFsHandle_t **ppTail)
{
	HxLOG_Debug(C_CODE_F_WHITE_B_RED" REMOVED! fsType(%d) faName(%s) pItem(%p)"C_CODE_RESET"\n", pItem->fsType, pItem->fsName, pItem);

	if (pItem->pPrev == NULL)
	{
		(*ppHead) = pItem->pNext;
	}
	else
	{
		pItem->pPrev->pNext = pItem->pNext;
	}
	if (pItem->pNext == NULL)
	{
		(*ppTail) = pItem->pPrev;
	}
	else
	{
		pItem->pNext->pPrev = pItem->pPrev;
	}
}

static ITK_NvsFsHandle_t *mheg_itk_port_nvsFsHandle(itk_nvs_fs_type_t fsType, const char *fsName,	ITK_NvsFsHandle_t *pHead, ITK_NvsFsHandle_t *pTail)
{
	ITK_NvsFsHandle_t *pItem = NULL;

	(void)(pTail);

	pItem = pHead;

	while (pItem != NULL)
	{
//		HxLOG_Debug(" fsType(%d) faName(%s)\n", pItem->fsType, pItem->fsName);
//		HxLOG_Debug(" pItem(%p) pItem->prev(%p) pItem->next(%p) pHead(%p) pTail(%p)\n", pItem, pItem->pPrev, pItem->pNext, pHead, pTail);

		if (pItem->fsType == fsType
			&& strcmp(pItem->fsName, fsName) == 0)
		{
			break;
		}

		pItem = pItem->pNext;
	}

	return pItem;
}

static ITK_NvsFileHandle_t *mheg_itk_port_nvmFileDelete(ITK_NvsFileHandle_t *pFile, HBOOL bFileOnly)
{
	ITK_NvsFileHandle_t *pRet = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;
	char file[ITK_NVS_PATH_MAX];
	HBOOL bErr;

	if (pFile == NULL)
	{
		HxLOG_Error(" pFile is NULL !\n");
		return NULL;
	}

	pFs = (ITK_NvsFsHandle_t *)pFile->caller;
	if (pFs == NULL)
	{
		HxLOG_Error(" caller is NULL !\n");
		return NULL;
	}

	pRet = pFile->pNext; // The iterator shall operate on the next file.

	if ( !bFileOnly )
	{
		bErr = mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, pFile->entryName, file, ITK_NVS_PATH_MAX);
		/* 예외 상황 처리. 이런지경에 오면 안된다. 이미 해당 포인터는 garbage이기 때문이다 */
		if (bErr == FALSE)
		{
			return (ITK_NvsFileHandle_t *)NULL;
		}
		mheg_itk_port_nvsErase(file);
	}

	HxLOG_Debug(C_CODE_F_WHITE_B_RED" REMOVED! file(%s)"C_CODE_RESET"\n", file);

	/* Remove the file pointer. */
	mheg_itk_port_nvsFileRemove(pFile, &pFs->pHead, &pFs->pTail);

	pFs->freeSpace += pFile->dataSize;
	pFs->totaldataSize -= (pFile->dataSize + ITK_NVS_METADATA_LENGTH);
	pFs->ulNvsFileNum--;

	HLIB_STD_MemFree(pFile);
	pFile = NULL;

	return pRet;
}

typedef struct ITK_NVS_LIST_t
{
	char *pItem;	// dir

	struct ITK_NVS_LIST_t *pPrev;
	struct ITK_NVS_LIST_t *pNext;
} ITK_NVS_LIST_t;

ITK_NVS_LIST_t	*pNvsDirListHead = NULL, *pNvsDirListTail = NULL;

static ITK_NVS_LIST_t *mheg_itk_port_nvsItemPush(char *pItem)
{
	ITK_NVS_LIST_t *pNewNode = NULL;
	ITK_NVS_LIST_t *pHead = pNvsDirListHead, *pTail = pNvsDirListTail;

	pNewNode = HLIB_STD_MemAlloc(sizeof(ITK_NVS_LIST_t));
	if (pNewNode == NULL)
	{
		HxLOG_Error(" out of memory : pNewNode is NULL !\n");
		return NULL;
	}

	pNewNode->pItem = pItem;

	if (pHead == NULL)
	{
		pHead = pNewNode;
		pNewNode->pPrev = NULL;
		pNvsDirListHead = pHead;
	}
	else
	{
		pTail->pNext = pNewNode;
		pNewNode->pPrev = pTail;
	}
	pTail = pNewNode;
	pNewNode->pNext = NULL;

	pNvsDirListTail = pTail;

	return pNewNode;
}

static char *mheg_itk_port_nvsItemPop(char **pItem)
{
	ITK_NVS_LIST_t *pHead = pNvsDirListHead, *pTail = pNvsDirListTail;

	if ((pHead == NULL) || (pTail == NULL))
	{
		return NULL;
	}

	*pItem = pTail->pItem;

	/* 1 node(head = tail) */
	if (pTail->pPrev == NULL)
	{
		HLIB_STD_MemFree(pTail);
		pNvsDirListHead = NULL;
		pNvsDirListTail = NULL;
	}
	else
	{
		pTail->pPrev->pNext = NULL;
		pNvsDirListTail = pTail->pPrev;
		HLIB_STD_MemFree(pTail);
	}

	return *pItem;
}

static HBOOL mheg_itk_port_nvmFileSearchAll(char *pBasedir, const char *pDir, ITK_NvsFsHandle_t *pFs)
{
	DIR *dirp = NULL;
	struct dirent *dp = NULL;
	char dir[ITK_NVS_FULLPATH_MAX];
	int entrysize = 0;

	if (pBasedir == NULL)
	{
		HxLOG_Error(" DIR is NULL !\n");
		return FALSE;
	}

	if (pFs == NULL)
	{
		HxLOG_Error(" pFs is NULL !\n");
		return FALSE;
	}

	if (pDir == NULL)
		snprintf(dir, ITK_NVS_FULLPATH_MAX, "%s", pBasedir);
	else
		snprintf(dir, ITK_NVS_FULLPATH_MAX, "%s%s", pBasedir, pDir);

	dirp = opendir(dir);

	if (dirp == NULL)
	{
		HxLOG_Debug(" dir open fail(%s)!\n", dir);
		return FALSE;
	}

	while ((dp = readdir(dirp)) != NULL)
	{
		if ((strcmp(dp->d_name,".") == 0) || (strcmp(dp->d_name,"..") == 0))
		{
			continue;
		}

		if (pDir != NULL)
			snprintf(dir, ITK_NVS_FULLPATH_MAX, "%s%s/%s", pBasedir, pDir, dp->d_name);
		else
			snprintf(dir, ITK_NVS_FULLPATH_MAX, "%s/%s", pBasedir, dp->d_name);
//printf("\t\tdir(%s) pBasedir(%s) pDir(%s) strlen d_name(%d)\n", dir, pBasedir, pDir, strlen(dp->d_name);

		if (mheg_itk_port_nvsIsDir(dir))
		{
			char *pname = NULL;

			entrysize += strlen(dp->d_name) + 1 /* slash */ + 1 /* null */;
			pname = HLIB_STD_MemAlloc(entrysize);
			if (pname == NULL)
			{
				closedir(dirp);

				HxLOG_Error(" out of memory : pFile is NULL !\n");
				return FALSE;
			}
			memset(pname, 0, entrysize);
			if (pDir != NULL)
				snprintf(pname, entrysize, "%s/%s", pDir, dp->d_name);
			else
				snprintf(pname, entrysize, "/%s", dp->d_name);

//			printf("\t\tPUSH dir(%s)\n", pname);

			/* free는 Pop호출후 */
			mheg_itk_port_nvsItemPush(pname);
		}
		else if (mheg_itk_port_nvsIsReg(dir))
		{
			ITK_NvsFileHandle_t *pFile = NULL;

			/* meta.dat 검색 제외 */
			if (strcmp(dp->d_name, ITK_NVS_FS_METAFILE_NAME) == 0)
			{
				continue;
			}

			pFile = (ITK_NvsFileHandle_t *)HLIB_STD_MemAlloc(sizeof(*pFile));
			if (pFile == NULL)
			{
				closedir(dirp);

				HxLOG_Error(" out of memory : pFile is NULL !\n");
				return FALSE;
			}

			if (!mheg_itk_port_nvsDataSize(dir, &pFile->dataSize))
			{
				HLIB_STD_MemFree(pFile);

				closedir(dirp);

				HxLOG_Error(" mheg_itk_port_nvsDataSize error !\n");
				return FALSE;
			}

			if (pDir != NULL)
			{
				snprintf(pFile->entryName, ITK_NVS_NAME_LEN, "%s/%s", pDir, dp->d_name);
			}
			else
			{
				/*
					rjlee. work-around
					fsa는 entryname의 선두가 '/' 이다
				*/
				if (pFs->fsType == ITK_NVS_FS_FSA)
					snprintf(pFile->entryName, ITK_NVS_NAME_LEN, "/%s", dp->d_name);
				else
					snprintf(pFile->entryName, ITK_NVS_NAME_LEN, "%s", dp->d_name);
			}

			//HxLOG_Debug("[ENTRY NAME] dir(%s)\n", pFile->entryName);

			pFile->usedBytes = pFile->dataSize;
			pFile->caller = (void *)pFs;
			pFile->pPrev = pFile->pNext = NULL;

			/* Append the file pointer. */
			mheg_itk_port_nvsFileAppend(pFile, &pFs->pHead, &pFs->pTail);

			pFs->freeSpace -= pFile->dataSize;
			pFs->totaldataSize += (pFile->dataSize + ITK_NVS_METADATA_LENGTH);
			pFs->ulNvsFileNum++;
		}
	}

	closedir(dirp);

	return TRUE;
}

static HBOOL mheg_itk_port_nvmFileOpen(ITK_NvsFsHandle_t *pFs)
{
	char *popDir = NULL;
	char basedir[ITK_NVS_PATH_MAX];

	if (pFs == NULL)
	{
		HxLOG_Error(" pFs is NULL !\n");
		return FALSE;
	}

	mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, NULL, basedir, ITK_NVS_PATH_MAX);

	do
	{
		mheg_itk_port_nvmFileSearchAll(basedir, popDir, pFs);

		if (popDir != NULL)
			HLIB_STD_MemFree(popDir);

	} while(mheg_itk_port_nvsItemPop(&popDir) != NULL);

	return TRUE;
}

/* sub folder까지 지우는 기능은 없다 */
static HBOOL mheg_itk_port_nvmFileDeleteAll(char *pszPath)
{
	DIR *dirp = NULL;
	struct dirent *dp = NULL;
	char dir[ITK_NVS_FULLPATH_MAX];

	if (pszPath == NULL)
	{
		HxLOG_Error(" Path is NULL !\n");
		return FALSE;
	}

	dirp = opendir(pszPath);
	if (dirp == NULL)
	{
		HxLOG_Error(" opendir error(%s) !\n", dir);
		return FALSE;
	}

	do
	{
		if ((dp = readdir(dirp)) != NULL)
		{
			if (strcmp(dp->d_name,".") == 0
				|| strcmp(dp->d_name,"..") == 0)
				continue;

			/* meta.dat 검색 제외 */
			if (strcmp(dp->d_name, ITK_NVS_FS_METAFILE_NAME) == 0)
			{
				continue;
			}

			snprintf(dir, ITK_NVS_FULLPATH_MAX, "%s/%s", dir, dp->d_name);

			HxLOG_Debug(" read file '%s' !\n", dp->d_name);

			if (mheg_itk_port_nvsIsReg(dir))
			{
				mheg_itk_port_nvsErase(dir);
			}
		}
	} while (dp != NULL);

	closedir(dirp);

	HxLOG_Debug(" deleted all files in dir '%s' !\n", pszPath);

	return TRUE;
}

static void mheg_itk_port_nvmFileClose(ITK_NvsFsHandle_t *pFs, HBOOL bDeleteFile)
{
	ITK_NvsFileHandle_t *pFile = NULL, *pDel = NULL;
	char file[ITK_NVS_PATH_MAX];

	if (pFs == NULL)
	{
		HxLOG_Error(" pFs is NULL !\n");
		return;
	}

	pFile = pFs->pHead;

	while (pFile != NULL)
	{
		pDel = pFile;
		pFile = pDel->pNext;

		if (bDeleteFile == TRUE)
		{
			mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, pDel->entryName, file, ITK_NVS_PATH_MAX);
			//HxLOG_Debug(" => delete(%s)\n", file);
			mheg_itk_port_nvsErase(file);
		}

		/* Remove the file pointer. */
		mheg_itk_port_nvsFileRemove(pDel, &pFs->pHead, &pFs->pTail);

		pFs->freeSpace += pDel->dataSize;
		pFs->totaldataSize -= (pDel->dataSize + ITK_NVS_METADATA_LENGTH);
		pFs->ulNvsFileNum--;

		HLIB_STD_MemFree(pDel);
		//pDel = NULL;
	}

	pFs->pHead = NULL;
	pFs->pTail = NULL;

	HxLOG_Error(" close dir '%s' ! freeSpace(%d) NvsFileNum(%d) [%p]\n", pFs->fsName, pFs->freeSpace, pFs->ulNvsFileNum, pFs);

	return;
}

void mheg_itk_port_nvmFileRelease(struct itk_nvs_file_t * thiz)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return;
	}

	// If thiz points to a partially written file, this call shall discard the file data
	// and the file shall be removed from the file system.
	pFile = piFile->pItem;

	HxLOG_Debug(" File Iterator(%p)\n", thiz);

	if (pFile != NULL
		&& pFile->usedBytes != pFile->dataSize)
	{
		mheg_itk_port_nvmFileDelete(pFile, FALSE);
	}

	HLIB_STD_MemFree(piFile);
	//piFile = NULL;

	return;
}

itk_present_t mheg_itk_port_nvmFileNext(struct itk_nvs_file_t * thiz)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITK_NOT_PRESENT;
	}

	pFile = piFile->pItem;
	if (pFile != NULL)
	{
		// If the iterator was pointing to a partially written file, that file shall be discarded
		// and the iterator shall move to next file.
		if (pFile->usedBytes != pFile->dataSize)
		{
			HxLOG_Debug(" name(%s) itk_nvs_file_t(%p)\n", pFile->entryName == NULL ? "NULL" : pFile->entryName, thiz);
			piFile->pItem = mheg_itk_port_nvmFileDelete(pFile, FALSE);
		}
		else
		{
			piFile->pItem = pFile->pNext;
		}
	}

	HxLOG_Debug(" itk_nvs_file_t(%p) next file handle(%p)\n", thiz, piFile->pItem);

	return piFile->pItem ? ITK_PRESENT : ITK_NOT_PRESENT;
}

char const *mheg_itk_port_nvmFileName(struct itk_nvs_file_t * thiz)
{
	ITK_NvsFileIterator_t *piFile = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile != NULL
		&& piFile->pItem != NULL)
	{
		return piFile->pItem->entryName;
	}

	return NULL;
}

size_t mheg_itk_port_nvmFileDataSize(struct itk_nvs_file_t * thiz)
{
	ITK_NvsFileIterator_t *piFile = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile != NULL
		&& piFile->pItem != NULL)
	{
		return piFile->pItem->dataSize;
	}

	return 0;
}

itk_errcode_t mheg_itk_port_nvmFileRead(struct itk_nvs_file_t * thiz, itk_uint8_t * buffer)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;
	//ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITKE_FAIL;
	}

	pFile = piFile->pItem;
	if (pFile == NULL)
	{
		HxLOG_Error(" thiz is EOL !\n");
		return ITKE_FAIL;
	}

	pFs = (ITK_NvsFsHandle_t *)pFile->caller;
	if (pFs == NULL)
	{
		HxLOG_Error(" caller is NULL !\n");
		return ITKE_FAIL;
	}

	if ( buffer )
	{
		// The file data cannot be read until all of the file data has been written.
		if ( pFile->dataSize == pFile->usedBytes )
		{
			char file[ITK_NVS_FULLPATH_MAX];

			mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, pFile->entryName, file, ITK_NVS_PATH_MAX);
			//snprintf(file, ITK_NVS_FULLPATH_MAX, "%s%s", file, pFile->entryName);

			mheg_itk_port_nvsRead(file, NULL, 0, pFile->dataSize, buffer); // TODO : how to check the return value

			HxLOG_Debug(" ok (file:%s, buffer:%p) ! itk_nvs_file_t[%p]\n", file, buffer, thiz);
			return ITKE_OK;
		}
	}

	HxLOG_Debug(" invalid parameters !\n");

	return ITKE_FAIL;
}

itk_errcode_t mheg_itk_port_nvmFileWrite(struct itk_nvs_file_t * thiz, size_t data_size, void const * buffer)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;
	//ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITKE_FAIL;
	}

	pFile = piFile->pItem;
	if (pFile == NULL)
	{
		HxLOG_Error(" thiz is EOL !\n");
		return ITKE_FAIL;
	}

	pFs = (ITK_NvsFsHandle_t *)pFile->caller;
	if (pFs == NULL)
	{
		HxLOG_Error(" caller is NULL !\n");
		return ITKE_FAIL;
	}

	if( buffer )
	{
		if( data_size )
		{
			char file[ITK_NVS_FULLPATH_MAX];

			mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, pFile->entryName, file, ITK_NVS_PATH_MAX);
			//snprintf(file, ITK_NVS_FULLPATH_MAX, "%s%s", file, pFile->entryName);

			mheg_itk_port_nvsWrite(file, NULL, pFile->usedBytes, data_size, buffer);

			pFile->usedBytes += data_size; // The files can be written in whole or in part.

			HxLOG_Debug(" ok (file:%s, data_size:%d, buffer:%p) ! [%p]\n", file, data_size, buffer, thiz);
			return ITKE_OK;
		}
	}

	HxLOG_Error(" invalid parameters !\n");

	return ITKE_FAIL;
}

itk_errcode_t mheg_itk_port_nvmFileReadMetaData(struct itk_nvs_file_t * thiz, itk_uint8_t * metadata)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;
	//ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITKE_FAIL;
	}

	pFile = piFile->pItem;
	if (pFile == NULL)
	{
		HxLOG_Error(" thiz is EOL !\n");
		return ITKE_FAIL;
	}

	pFs = (ITK_NvsFsHandle_t *)pFile->caller;
	if (pFs == NULL)
	{
		HxLOG_Error(" caller is NULL !\n");
		return ITKE_FAIL;
	}

	if ( metadata )
	{
		char file[ITK_NVS_FULLPATH_MAX];

		mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, pFile->entryName, file, ITK_NVS_PATH_MAX);
		//snprintf(file, ITK_NVS_FULLPATH_MAX, "%s%s", file, pFile->entryName);

		mheg_itk_port_nvsRead(file, metadata, 0, 0, NULL);

		HxLOG_Debug(" ok (file:%s, metadata:%p) ! [%p]\n", file, metadata, thiz);
		return ITKE_OK;
	}

	HxLOG_Error(" invalid parameters !\n");

	return ITKE_FAIL;
}

itk_errcode_t mheg_itk_port_nvmFileWriteMetaData(struct itk_nvs_file_t * thiz, itk_uint8_t const * metadata)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;
	//ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITKE_FAIL;
	}

	pFile = piFile->pItem;
	if (pFile == NULL)
	{
		HxLOG_Error(" thiz is EOL !\n");
		return ITKE_FAIL;
	}

	pFs = (ITK_NvsFsHandle_t *)pFile->caller;
	if (pFs == NULL)
	{
		HxLOG_Error(" caller is NULL !\n");
		return ITKE_FAIL;
	}

	if ( metadata )
	{
		char file[ITK_NVS_FULLPATH_MAX];

		mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, pFile->entryName, file, ITK_NVS_PATH_MAX);
		//snprintf(file, ITK_NVS_FULLPATH_MAX, "%s%s", file, pFile->entryName);

		mheg_itk_port_nvsWrite(file, metadata, 0, 0, NULL);

		HxLOG_Debug(" ok (file:%s, metadata:%p) ! [%p]\n", file, metadata, thiz);
		return ITKE_OK;
	}

	HxLOG_Error(" invalid parameters !\n");

	return ITKE_FAIL;
}

itk_present_t mheg_itk_port_nvmFileErase(struct itk_nvs_file_t * thiz)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;

	piFile = (ITK_NvsFileIterator_t *)thiz;
	if (piFile == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITK_NOT_PRESENT;
	}

	pFile = piFile->pItem;
	if (pFile != NULL)
	{
		HxLOG_Debug(" name(%s) itk_nvs_file_t(%p)\n", pFile->entryName == NULL ? "NULL" : pFile->entryName, thiz);
		piFile->pItem = mheg_itk_port_nvmFileDelete(pFile, FALSE);
	}

	return piFile->pItem ? ITK_PRESENT : ITK_NOT_PRESENT;
}

static HBOOL mheg_itk_port_nvsFsGetMetaFile
(
	char const * fullPath,
	size_t * minimum_capacity,
	itk_uint8_t * priority,
	size_t * metadata_size,
	size_t rsize,
	itk_uint8_t * metadata
)
{
	int fd = -1;
	HUINT8	szFileName[ITK_NVS_FULLPATH_MAX];
	HUINT8	aucHeader[ITK_NVS_METAFILE_HEADER_SIZE], *pucHeader;
	HUINT32	ulMagicNum, ulCapacity, ulPriority, ulMetaSize;
	int	n;
	
	if( fullPath == NULL )
	{
		HxLOG_Error(" invalid parameters !\n");
		return FALSE;
	}

	snprintf(szFileName, ITK_NVS_FULLPATH_MAX, "%s/%s", fullPath, ITK_NVS_FS_METAFILE_NAME);

	HxLOG_Debug(" get info into metafile(%s)\n", szFileName);

	fd = open(szFileName, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if( fd < 0 )
	{
		HxLOG_Error(" open error !\n");
		return FALSE;
	}

	n = (int)read(fd, aucHeader, ITK_NVS_METAFILE_HEADER_SIZE);
	if( n <= 0 )
	{
		close(fd);
		return FALSE;
	}
	
	pucHeader = aucHeader;

	pucHeader += 4;
	memcpy(&ulMagicNum, pucHeader, sizeof(HUINT32));

	if (ulMagicNum != ITK_NVS_FSA_MAGIC_NUMBER)
	{
		close(fd);
		HxLOG_Error(" invalid meta file(%s) magic_num(0x%x)\n", szFileName, ulMagicNum);
		return FALSE;
	}

	pucHeader += 4;
	memcpy (&ulCapacity, pucHeader, sizeof(HUINT32));
	if (minimum_capacity)
		*minimum_capacity = ulCapacity;

	pucHeader += 4;
	memcpy (&ulPriority, pucHeader, sizeof(HUINT32));
	if (priority)
		*priority = ulPriority;

	pucHeader += 4;
	memcpy (&ulMetaSize, pucHeader, sizeof(HUINT32));
	if (metadata_size)
		*metadata_size = ulMetaSize;

	if( metadata )
	{
		if( ulMetaSize && rsize )
		{
			if( ulMetaSize < rsize )
			{
				// prevent 155350
				if(read(fd, metadata, ulMetaSize) != ulMetaSize)
				{
					HxLOG_Error(" read error !\n");
					close(fd);
					return FALSE;
				}
			}
			else
			{
				if (read(fd, metadata, rsize) != (ssize_t)rsize) {
					HxLOG_Error(" lseek error !\n");
					close(fd);
					return FALSE;
				}
			}
		}
	}

	close(fd);

	return TRUE;
}

static HBOOL mheg_itk_port_nvsFsSetMetaFile
(
	char const * fullPath,
	size_t minimum_capacity,
	itk_uint8_t priority,
	size_t metadata_size,
	size_t wsize,
	itk_uint8_t const * metadata
)
{
	int fd = -1;
	char		szFileName[ITK_NVS_FULLPATH_MAX];
	HUINT8	aucHeader[ITK_NVS_METAFILE_HEADER_SIZE], *pucHeader;
	HUINT32	ulMagicNum, ulCapacity, ulPriority, ulMetaSize;

	if( fullPath == NULL )
	{
		HxLOG_Error(" invalid parameters !\n");
		return FALSE;
	}

	snprintf(szFileName, ITK_NVS_FULLPATH_MAX, "%s/%s", fullPath, ITK_NVS_FS_METAFILE_NAME);

	HxLOG_Debug(" set info into metafile(%s) metadata_size(%d) wsize(%d) metadata(%p)!\n", szFileName, metadata_size, wsize, metadata);

	fd = open(szFileName, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);

	if( fd < 0 )
	{
		HxLOG_Error(" open error ! fullPath(%s) szFileName(%s) fd(0x%x)\n", fullPath, szFileName, fd);
		return FALSE;
	}

	pucHeader = aucHeader;
	memset (pucHeader, 0, ITK_NVS_METAFILE_HEADER_SIZE);

	pucHeader += 4;
	ulMagicNum = ITK_NVS_FSA_MAGIC_NUMBER;
	memcpy(pucHeader, &ulMagicNum, sizeof(HUINT32));

	pucHeader += 4;
	ulCapacity = minimum_capacity;
	memcpy (pucHeader, &ulCapacity, sizeof(HUINT32));

	pucHeader += 4;
	ulPriority = priority;
	memcpy (pucHeader, &ulPriority, sizeof(HUINT32));

	pucHeader += 4;
	ulMetaSize = metadata_size;
	memcpy (pucHeader, &ulMetaSize, sizeof(HUINT32));

// rjlee. 2010.02.07.
#if 1
	// The relative priority of the file system, 0 being lowest and 255 highest.
	if(minimum_capacity > 0)
#else
	if( minimum_capacity && priority )
#endif
	{
		write(fd, aucHeader, ITK_NVS_METAFILE_HEADER_SIZE);
	}

	if( metadata )
	{
		if( wsize )
		{
			if (lseek(fd, ITK_NVS_METAFILE_HEADER_SIZE, SEEK_SET) < 0) {
				HxLOG_Error(" lseek error !\n");
				close(fd);
				return FALSE;
			}

			write(fd, metadata, wsize);
		}
	}

	close(fd);

	return TRUE;
}

static ITK_NvsFsHandle_t *mheg_itk_port_nvsFsDelete(ITK_NvsFsHandle_t *pFs,
	HBOOL bFsOnly)
{
	ITK_NvsFsHandle_t *pRet = NULL;
	ITK_NvsHandle_t *pNvs = NULL;

	if (pFs == NULL)
	{
		HxLOG_Error(" pFs is NULL !\n");
		return NULL;
	}

	pNvs = (ITK_NvsHandle_t *)pFs->caller;
	if (pNvs == NULL)
	{
		HxLOG_Error(" caller is NULL !\n");
		return NULL;
	}

	pRet = pFs->pNext; // The iterator shall operate on the next available file system.

	if ( !bFsOnly )
	{
		char dir[ITK_NVS_FULLPATH_MAX];

		{
			ITK_NvsFileHandle_t *pFile = pFs->pHead;

			while (pFile != NULL)
			{
				HxLOG_Debug(" name(%s)\n", pFile->entryName == NULL ? "NULL" : pFile->entryName);
				pFile = mheg_itk_port_nvmFileDelete(pFile, FALSE);
			}
		}

		mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, NULL, dir, ITK_NVS_PATH_MAX);

		mheg_itk_port_nvsErase(dir);
	}

	/* Remove the fs pointer. */
	mheg_itk_port_nvsFsRemove(pFs, &pNvs->pHead, &pNvs->pTail);

	pNvs->ulNvsFsNum--;

	HLIB_STD_MemFree(pFs);
	//pFs = NULL;

	return pRet;
}

static HBOOL mheg_itk_port_nvsFsOpen(ITK_NvsHandle_t *pNvs, itk_nvs_fs_type_t fsType)
{
	DIR *dirp = NULL;
	struct dirent *dp = NULL;
	char *pBasepath = NULL;

	if (pNvs == NULL)
	{
		HxLOG_Error(" pNvs is NULL !\n");
		return FALSE;
	}

	HxLOG_Debug(" >>> fsType(%d) [%p]\n", fsType, pNvs);

	switch(fsType)
	{
		case ITK_NVS_FS_FSA:
			pBasepath = ITK_NVS_FS_FSA_BASE_PATH;
			break;
		case ITK_NVS_FS_NVM:
			pBasepath = ITK_NVS_FS_NVM_BASE_PATH;
			break;
		case ITK_NVS_FS_RAM:
			pBasepath = ITK_NVS_FS_RAM_BASE_PATH;
			break;
		case ITK_NVS_FS_PST:
			pBasepath = ITK_NVS_FS_PST_BASE_PATH;
			break;
		default:
			HxLOG_Error(" Not supported fs type!\n");
			return FALSE;
	}

	if ((dirp = opendir(pBasepath)) == NULL)
	{
		HxLOG_Error(" opendir error !\n");
		return FALSE;
	}

	do {
		if ((dp = readdir(dirp)) != NULL)
		{
			char dir[ITK_NVS_FULLPATH_MAX];

			if (strcmp(dp->d_name,".") == 0
				|| strcmp(dp->d_name,"..") == 0)
				continue;

			snprintf(dir, ITK_NVS_FULLPATH_MAX, "%s/%s", pBasepath, dp->d_name);

			HxLOG_Debug(" read dir '%s' ! [%p]\n", dp->d_name, pNvs);

			/* NVM root에 있는 폴더들은 모두 FS이다(fsType이 동일한 FS도 존재 가능) */
			if (mheg_itk_port_nvsIsDir(dir))
			{
				size_t		minimumCapacity, metadataSize;
				itk_uint8_t	priority;

				if ( mheg_itk_port_nvsFsGetMetaFile(dir, &minimumCapacity, &priority, &metadataSize, 0, NULL) == TRUE )
				{
					//redkey_profile_t eItkProfile = s_mhegNvsInfo.itkProfile;		// not used
					ITK_NvsFsHandle_t *pFs = NULL;

					pFs = (ITK_NvsFsHandle_t *)HLIB_STD_MemAlloc(sizeof(*pFs));
					if (pFs == NULL)
					{
						closedir(dirp);

						HxLOG_Error(" out of memory : pFs is NULL !\n");
						return FALSE;
					}

					pFs->fsType = fsType;
					strncpy(pFs->fsName, dp->d_name, ITK_NVS_PATH_MAX - 1);
					pFs->fsName[ITK_NVS_PATH_MAX - 1] = '\0';

					pFs->minimumCapacity = minimumCapacity;
					pFs->priority = priority;
					pFs->metadataSize = metadataSize;
					pFs->freeSpace = pFs->minimumCapacity;
					pFs->caller = (void *)pNvs;
					pFs->pPrev = pFs->pNext = NULL;
					pFs->ulNvsFileNum = 0;
					pFs->pHead = pFs->pTail = NULL;

					/* 해당 FS 폴더 안의 모든 파일을 검색 */
					if (mheg_itk_port_nvmFileOpen(pFs) != TRUE)
					{
						mheg_itk_port_nvmFileClose(pFs, FALSE);

						//if (pFs != NULL)
						//{
							HLIB_STD_MemFree(pFs);
							//pFs = NULL;
						//}

						closedir(dirp);

						HxLOG_Error(" mheg_itk_port_nvmFileOpen error !\n");
						return FALSE;
					}

					/* Append the fs pointer. */
					mheg_itk_port_nvsFsAppend(pFs, &pNvs->pHead, &pNvs->pTail);

					pNvs->ulNvsFsNum++;
				}
			}
		}
	} while (dp != NULL);

	closedir(dirp);

	HxLOG_Debug(" open dir '%s' ! [%p]\n",pBasepath, pNvs);

	return TRUE;
}

static void mheg_itk_port_nvsFsClose(ITK_NvsHandle_t *pNvs, itk_nvs_fs_type_t fsType, Mheg_Nvs_Fs_Type_t MhegFsType)
{
	ITK_NvsFsHandle_t *pFs = NULL, *pDel = NULL;
	char szDelPath[ITK_NVS_PATH_MAX];

	if (pNvs == NULL)
	{
		HxLOG_Error(" pNvs is NULL !\n");
		return;
	}

	pFs = pNvs->pHead;

	while (pFs != NULL)
	{
		pDel = pFs;
		pFs = pDel->pNext;

		if (fsType == pDel->fsType)
		{
#if defined(REMOVE_ALL_NVS_FS_WHEN_FACTORY_RESET)
			/* do nothing */
#else
			/*
				NVM이외의 모든 File Handle삭제. FS Handle은 삭제하지 않는다
				단, factory reset은 제외. factory reset일때 조차 NVM handle을 지우지 않는다면, meta data에 의해 기존 생성된 NVM size를 절대 변경할수 없는 문제가 생긴다
			*/
			if ((pDel->fsType == ITK_NVS_FS_NVM) && !(MhegFsType & eMHEG_NVS_FACTORY_RESET))
			{
				mheg_itk_port_nvmFileClose(pDel, TRUE);

				/* 자료구조이외에 존재할지 모르는 파일들을 모두 찾아서 지운다 */
				mheg_itk_port_nvsGetFullPath(pDel->fsType, pDel->fsName, NULL, szDelPath, ITK_NVS_PATH_MAX);
				mheg_itk_port_nvmFileDeleteAll(szDelPath);
			}
			else
#endif
			{
				mheg_itk_port_nvmFileClose(pDel, FALSE);

				/* Remove the fs pointer. */
				mheg_itk_port_nvsFsRemove(pDel, &pNvs->pHead, &pNvs->pTail);

				pNvs->ulNvsFsNum--;

				HLIB_STD_MemFree(pDel);
				//pDel = NULL;
			}

			HxLOG_Debug(" close dir '%s' ! NvsFsNum(%d) nvs_handle[%p]\n", ITK_NVS_FS_NVM_BASE_PATH, pNvs->ulNvsFsNum, pNvs);
		}
	}

	if (pNvs->ulNvsFsNum == 0)
	{
		pNvs->pHead = pNvs->pTail = NULL;
	}
}

void mheg_itk_port_nvsFsRelease(struct itk_nvs_fs_t * thiz)
{
	ITK_NvsFsIterator_t *piFs = NULL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return;
	}

	HxLOG_Debug(" FS Iterator(%p)\n", piFs);

	HLIB_STD_MemFree(piFs);
	//piFs = NULL;

	return;
}

itk_present_t mheg_itk_port_nvsFsNext(struct itk_nvs_fs_t * thiz)
{
	ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITK_NOT_PRESENT;
	}

	HxLOG_Debug(" itk_nvs_fs_t(%p)\n", thiz);

	pFs = piFs->pItem;
	if (pFs != NULL)
	{
		piFs->pItem = pFs->pNext;
	}

	return piFs->pItem ? ITK_PRESENT : ITK_NOT_PRESENT;
}

itk_nvs_file_t *mheg_itk_port_nvsFsGetNvsFile(struct itk_nvs_fs_t * thiz, const char * name)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;
	ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;
	HBOOL	bValidFsType = FALSE;

	redkey_profile_t eItkProfile = s_mhegNvsInfo.itkProfile;

	HxLOG_Trace();

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return (itk_nvs_file_t *)NULL;
	}

	pFs = piFs->pItem;
	if (pFs == NULL)
	{
		HxLOG_Error(" pFs is NULL ! name(%s)\n", name);
		return (itk_nvs_file_t *)NULL;
	}
	else
	{
		HxLOG_Debug(C_CODE_F_WHITE_B_PURPLE" fsname (%s) "C_CODE_RESET"\n", pFs->fsName);
	}

	bValidFsType = mheg_itk_port_nvsCheckValidFsType(pFs->fsType);
	if (bValidFsType == FALSE)
	{
		HxLOG_Error(" Invalid FS Type(%d)\n", pFs->fsType);
		return (itk_nvs_file_t *)NULL;
	}

	if (name == NULL)
	{
		pFile = pFs->pHead;
		if (pFile == NULL)
		{
			HxLOG_Debug(C_CODE_F_BLACK_B_GREEN" There is no file handle. Empty FS!!! name(%s) thiz(%p)"C_CODE_RESET"\n", name, thiz);
			return (itk_nvs_file_t *)NULL;
		}
	}
	else
	{
		pFile = mheg_itk_port_nvmFileHandle(name, pFs->pHead, pFs->pTail);
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN" File name(%s) thiz(%p)"C_CODE_RESET"\n", name, thiz);

	if (pFile == NULL)
	{
#if defined(ITK_NVS_SEARCH_FILE_WHEN_GET_TIME)
		char	file[ITK_NVS_FULLPATH_MAX];

		if (mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, name, file, ITK_NVS_PATH_MAX) == TRUE)
		{
			if (mheg_itk_port_nvsIsReg(file) == TRUE)
			{
				pFile = (ITK_NvsFileHandle_t *)HLIB_STD_MemAlloc(sizeof(*pFile));
				if (pFile == NULL)
				{
					HxLOG_Error(" out of memory : pFile is NULL !\n");
					return (itk_nvs_file_t *)NULL;
				}

				if (!mheg_itk_port_nvsDataSize(file, &pFile->dataSize))
				{
					HLIB_STD_MemFree(pFile);
					HxLOG_Error(" mheg_itk_port_nvsDataSize error !\n");
					return (itk_nvs_file_t *)NULL;
				}
				else
				{
					strncpy(pFile->entryName, name, ITK_NVS_NAME_LEN - 1);
					pFile->entryName[ITK_NVS_NAME_LEN - 1] = '\0';

					pFile->usedBytes = pFile->dataSize;
					pFile->caller = (void *)pFs;
					pFile->pPrev = pFile->pNext = NULL;
				}

				if (name != NULL)
				{
					/* Append the file pointer. */
					mheg_itk_port_nvsFileAppend(pFile, &pFs->pHead, &pFs->pTail);

					pFs->freeSpace -= pFile->dataSize;
					pFs->totaldataSize += (pFile->dataSize + ITK_NVS_METADATA_LENGTH);
					pFs->ulNvsFileNum++;
				}
			}
			else
			{
				HxLOG_Debug("Not support file!!\n");
				return (itk_nvs_file_t *)NULL;
			}
		}
		else
#endif
		{
			HxLOG_Debug(" not exists : pFile is NULL ! name(%s) fsType(%d), fsName(%s) thiz(%p)\n", name, pFs->fsType, pFs->fsName, thiz);
			return (itk_nvs_file_t *)NULL;
		}
	}

	piFile = (ITK_NvsFileIterator_t *)HLIB_STD_MemAlloc(sizeof(*piFile));
	if (piFile == NULL)
	{
		HxLOG_Error(" out of memory : piFile is NULL !\n");
		return (itk_nvs_file_t *)NULL;
	}
	//memset(piFile, 0, sizeof(*piFile);

	piFile->vtbl.release		= mheg_itk_port_nvmFileRelease;
	piFile->vtbl.next			= mheg_itk_port_nvmFileNext;
	piFile->vtbl.name			= mheg_itk_port_nvmFileName;
	piFile->vtbl.dataSize		= mheg_itk_port_nvmFileDataSize;
	piFile->vtbl.read			= mheg_itk_port_nvmFileRead;
	piFile->vtbl.write			= mheg_itk_port_nvmFileWrite;
	piFile->vtbl.readMetadata	= mheg_itk_port_nvmFileReadMetaData;
	piFile->vtbl.writeMetadata	= mheg_itk_port_nvmFileWriteMetaData;
	piFile->vtbl.erase			= mheg_itk_port_nvmFileErase;

	if ( (eItkProfile != REDKEY_PROFILE_UK_FSAT)
		&& (eItkProfile != REDKEY_PROFILE_FREESATG2) )
	{
		piFile->vtbl.readMetadata	= NULL;
		piFile->vtbl.writeMetadata	= NULL;
	}

	piFile->pItem = pFile;

	HxLOG_Debug(" returned file_t(%p) (name:%s) (fsname:%s)! itk_nvs_fs_t[%p]\n", piFile, piFile->vtbl.name, pFs->fsName, thiz);

	return (itk_nvs_file_t *)piFile;
}

itk_nvs_file_t *mheg_itk_port_nvsFsNewNvsFile(struct itk_nvs_fs_t * thiz,
                            char const * entryName, itk_uint8_t const * metadata,
                            size_t dataSize)
{
	ITK_NvsFileIterator_t *piFile = NULL;
	ITK_NvsFileHandle_t *pFile = NULL;
	ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;
	char file[ITK_NVS_FULLPATH_MAX];

	redkey_profile_t eItkProfile = s_mhegNvsInfo.itkProfile;

	HxLOG_Trace();

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return (itk_nvs_file_t *)NULL;
	}

	if (entryName == NULL)
	{
		HxLOG_Error(" invalid parameters !\n");
		return (itk_nvs_file_t *)NULL;
	}

	pFs = piFs->pItem;
	if (pFs == NULL)
	{
		HxLOG_Error(" pFs is NULL !\n"); // never happen
		return (itk_nvs_file_t *)NULL;
	}

	pFile = mheg_itk_port_nvmFileHandle(entryName, pFs->pHead, pFs->pTail);
	if (pFile != NULL)
	{
		HxLOG_Error(" already exists (entryName:%s, pFile:%p) !\n", entryName, pFile);
		return (itk_nvs_file_t *)NULL;
	}

#if 1
	/*
		2010.03.31. free space 체크후 파일 생성
		METADATA_LENGTH는 host가 cover하는것으로 한다. Test suite에 64K max인 FS에 64K를 시도하는데 meta size까지 포함하면 절대 생성할수 없기 때문이다
	*/
	if (pFs->freeSpace < dataSize)
	{
		HxLOG_Error(C_CODE_F_WHITE_B_PURPLE" Maximum size is over! Max(%d) free(%d) totaldataSize(%d) dataSize(%d)"C_CODE_RESET"\n", pFs->minimumCapacity, pFs->freeSpace, pFs->totaldataSize, dataSize);
		return (itk_nvs_file_t *)NULL;
	}
#endif

	piFile = (ITK_NvsFileIterator_t *)HLIB_STD_MemAlloc(sizeof(*piFile));
	if (piFile == NULL)
	{
		HxLOG_Error(" out of memory : piFile is NULL !\n");
		return (itk_nvs_file_t *)NULL;
	}
	//memset(piFile, 0, sizeof(*piFile));

	pFile = (ITK_NvsFileHandle_t *)HLIB_STD_MemAlloc(sizeof(*pFile));
	if (pFile == NULL)
	{
		//if (piFile != NULL)
		//{
			HLIB_STD_MemFree(piFile);
			//piFile = NULL;
		//}

		HxLOG_Error(" out of memory : pFile is NULL !\n");
		return (itk_nvs_file_t *)NULL;
	}
	//memset(pFile, 0, sizeof(*pFile));

	strncpy(pFile->entryName, entryName, ITK_NVS_NAME_LEN - 1);
	pFile->entryName[ITK_NVS_NAME_LEN - 1] = '\0';

	pFile->dataSize = dataSize;
	pFile->usedBytes = 0;
	pFile->caller = (void *)pFs;
	pFile->pPrev = pFile->pNext = NULL;

	piFile->vtbl.release		= mheg_itk_port_nvmFileRelease;
	piFile->vtbl.next			= mheg_itk_port_nvmFileNext;
	piFile->vtbl.name			= mheg_itk_port_nvmFileName;
	piFile->vtbl.dataSize		= mheg_itk_port_nvmFileDataSize;
	piFile->vtbl.read			= mheg_itk_port_nvmFileRead;
	piFile->vtbl.write			= mheg_itk_port_nvmFileWrite;
	piFile->vtbl.readMetadata	= mheg_itk_port_nvmFileReadMetaData;
	piFile->vtbl.writeMetadata	= mheg_itk_port_nvmFileWriteMetaData;
	piFile->vtbl.erase			= mheg_itk_port_nvmFileErase;

	if ( (eItkProfile != REDKEY_PROFILE_UK_FSAT)
		&& (eItkProfile != REDKEY_PROFILE_FREESATG2) )
	{
		piFile->vtbl.readMetadata	= NULL;
		piFile->vtbl.writeMetadata	= NULL;
	}

	piFile->pItem = pFile;

	mheg_itk_port_nvsGetFullPath(pFs->fsType, pFs->fsName, pFile->entryName, file, ITK_NVS_PATH_MAX);

	mheg_itk_port_nvsCheckDir(file);

	/* Write the new file. */
	if (mheg_itk_port_nvsWrite(file, metadata, 0, 0, NULL) != TRUE)
	{
		HLIB_STD_MemFree(pFile);
		HLIB_STD_MemFree(piFile);

		HxLOG_Error(" cannot create a new file (entryName:%s, metadata:%p, dataSize:%d) ! [%p]\n", entryName, metadata, dataSize, thiz);
		return (itk_nvs_file_t *)NULL;
	}

	/* Append the file pointer. */
	mheg_itk_port_nvsFileAppend(pFile, &pFs->pHead, &pFs->pTail);

	pFs->freeSpace -= pFile->dataSize;
	pFs->totaldataSize += (pFile->dataSize + ITK_NVS_METADATA_LENGTH);
	pFs->ulNvsFileNum++;

#ifdef CONFIG_DEBUG
	if (metadata)
		HxLOG_Debug(C_CODE_F_BLACK_B_YELLOW" create a new file (fsName(%s) entryName:%s, metadata:%p, dataSize:%d) ! file_handle[%p] itk_nvs_file_t[%p] itk_nvs_fs_t[%p]"C_CODE_RESET"\n", pFs->fsName, entryName, metadata, dataSize, pFile, piFile, thiz);
	else
		HxLOG_Debug(C_CODE_F_BLACK_B_YELLOW" create a new file (fsName(%s) entryName:%s, dataSize:%d) ! file_handle[%p] itk_nvs_file_t[%p] itk_nvs_fs_t[%p]"C_CODE_RESET"\n", pFs->fsName, entryName, dataSize, pFile, piFile, thiz);
#endif

	return (itk_nvs_file_t *)piFile;
}

itk_nvs_fs_type_t mheg_itk_port_nvsFsType(struct itk_nvs_fs_t * thiz)
{
	ITK_NvsFsIterator_t *piFs = NULL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs != NULL
		&& piFs->pItem != NULL)
	{
		return piFs->pItem->fsType;
	}

	return ITK_NVS_FS_NVM;
}

const char *mheg_itk_port_nvsFsName(struct itk_nvs_fs_t * thiz)
{
	ITK_NvsFsIterator_t *piFs = NULL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs != NULL
		&& piFs->pItem != NULL)
	{
		return piFs->pItem->fsName;
	}

	return NULL;
}

size_t mheg_itk_port_nvsFsFreeSpace(struct itk_nvs_fs_t * thiz)
{
	ITK_NvsFsIterator_t *piFs = NULL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs != NULL
		&& piFs->pItem != NULL)
	{
		HxLOG_Debug(C_CODE_F_WHITE_B_PURPLE" fsName(%s) freeSpace(%d) thiz(%p)"C_CODE_RESET"\n", piFs->pItem->fsName, piFs->pItem->freeSpace, thiz);
		return piFs->pItem->freeSpace;
	}

	return 0;
}

size_t mheg_itk_port_nvsFsSizeMetaData(struct itk_nvs_fs_t * thiz)
{
	ITK_NvsFsIterator_t *piFs = NULL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs != NULL
		&& piFs->pItem != NULL)
	{
		return piFs->pItem->metadataSize;
	}

	return 0;
}

itk_errcode_t mheg_itk_port_nvsFsReadMetaData(struct itk_nvs_fs_t * thiz,
        itk_uint8_t *buffer, size_t bufferLen)
{
	ITK_NvsFsIterator_t *piFs = NULL;

	if (buffer == NULL || bufferLen == 0)
		return ITKE_FAIL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs != NULL
		&& piFs->pItem != NULL
		&& piFs->pItem->metadataSize > 0)
	{
		char dir[ITK_NVS_PATH_MAX];

		memset(dir, 0, ITK_NVS_PATH_MAX);
		// error check???
		if (mheg_itk_port_nvsGetFullPath(piFs->pItem->fsType, piFs->pItem->fsName, NULL, dir, ITK_NVS_PATH_MAX) == TRUE)
		{
			if (mheg_itk_port_nvsFsGetMetaFile(dir,	NULL, NULL, NULL, bufferLen, buffer) == TRUE)
			{
				return ITKE_OK;
			}
		}
	}

	return ITKE_FAIL;
}

itk_errcode_t mheg_itk_port_nvsFsWriteMetaData(struct itk_nvs_fs_t * thiz,
    itk_uint8_t const * metadata)
{
	ITK_NvsFsIterator_t *piFs = NULL;

	if (metadata == NULL)
		return ITKE_FAIL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs != NULL
		&& piFs->pItem != NULL
		&& piFs->pItem->metadataSize > 0)
	{
		char dir[ITK_NVS_PATH_MAX];

		memset(dir, 0, ITK_NVS_PATH_MAX);
		// error check???
		if (mheg_itk_port_nvsGetFullPath(piFs->pItem->fsType, piFs->pItem->fsName, NULL, dir, ITK_NVS_PATH_MAX) == TRUE)
		{
			if (mheg_itk_port_nvsFsSetMetaFile(dir, 0, 0, 0, piFs->pItem->metadataSize, metadata) == TRUE)
			{
				return ITKE_OK;
			}
		}
	}

	return ITKE_FAIL;
}

itk_present_t mheg_itk_port_nvsFsErase(struct itk_nvs_fs_t * thiz)
{
	ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITK_NOT_PRESENT;
	}

	pFs = piFs->pItem;
	if (pFs != NULL)
	{
		piFs->pItem = mheg_itk_port_nvsFsDelete(pFs, FALSE);
	}

	return piFs->pItem ? ITK_PRESENT : ITK_NOT_PRESENT;
}

itk_present_t mheg_itk_port_nvsFsEnabled(struct itk_nvs_fs_t * thiz)
{
	ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;
	HBOOL	bEnable = FALSE;

	piFs = (ITK_NvsFsIterator_t *)thiz;
	if (piFs == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return ITK_NOT_PRESENT;
	}

	pFs = piFs->pItem;
	if (pFs != NULL)
	{
		redkey_profile_t eItkProfile = s_mhegNvsInfo.itkProfile;

		if ( (eItkProfile == REDKEY_PROFILE_UK_HD)
		|| (eItkProfile == REDKEY_PROFILE_INTERNAL5)
		|| (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
		|| (eItkProfile == REDKEY_PROFILE_NZ_SD)
		|| (eItkProfile == REDKEY_PROFILE_THAI) )
		{
			bEnable = TRUE;
		}
	}

	return (bEnable == TRUE)? ITK_PRESENT : ITK_NOT_PRESENT;
}
/*******************************************************************************************************/

itk_nvs_fs_t *mheg_itk_port_nvsFsGetNvs(struct itk_nvs_t * thiz, itk_nvs_fs_type_t fsType, const char *fsName)
{
	ITK_NvsFsIterator_t *piFs = NULL;
	ITK_NvsFsHandle_t *pFs = NULL;
	ITK_NvsIterator_t *piNvs = NULL;
	ITK_NvsHandle_t *pNvs = NULL;
	redkey_profile_t eItkProfile = s_mhegNvsInfo.itkProfile;

	HxLOG_Debug(C_CODE_F_BLACK_B_CYAN" fsType:%d, fsName:%s ! [%p]"C_CODE_RESET"\n", fsType, fsName, thiz);

	if (fsType != ITK_NVS_FS_DVR && fsType != ITK_NVS_FS_NVM && fsType != ITK_NVS_FS_FSA && fsType != ITK_NVS_FS_RAM && fsType != ITK_NVS_FS_PST)
	{
		HxLOG_Error(" invalid parameters !\n");
		return (itk_nvs_fs_t *)NULL;
	}

	piNvs = (ITK_NvsIterator_t *)thiz;
	if (piNvs == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return (itk_nvs_fs_t *)NULL;
	}

	pNvs = piNvs->pItem;
	if (pNvs == NULL)
	{
		HxLOG_Error(" pNvs is NULL !\n");
		return (itk_nvs_fs_t *)NULL;
	}

	if (fsName == NULL)
	{
		pFs = pNvs->pHead;
		if (pFs == NULL)
		{
			HxLOG_Debug(C_CODE_F_BLACK_B_GREEN" There is no FS handle. Empty FS!!! fsName(%s) thiz(%p)"C_CODE_RESET"\n", fsName, thiz);
			return (itk_nvs_fs_t *)NULL;
		}
	}
	else
	{
		pFs = mheg_itk_port_nvsFsHandle(fsType, fsName, pNvs->pHead, pNvs->pTail);

		if ( ( eItkProfile == REDKEY_PROFILE_AUSTRALIA )
			 || (eItkProfile == REDKEY_PROFILE_SOUTH_AFRICA) )
		{
			if (pFs == NULL)
			{
				/*
					FSA의 경우 name에 /가 같이 오는 경우가 있다.
					newNvsFs에 의해 생성된 FS인 경우 문제가 없으나, reboot후 file search해서 생성된 FS인 경우 fsName에 /가 있었던건지 아닌지 구분할 방법이 없다.
					따라서 engine이 getNvsFs를 할때 판단해서 이름을 바꾸는 work-around를 적용한다
				*/
				if (*fsName == '/')
				{
					pFs = mheg_itk_port_nvsFsHandle(fsType, &fsName[1], pNvs->pHead, pNvs->pTail);
					if (pFs != NULL)
					{
						strncpy(pFs->fsName, fsName, ITK_NVS_PATH_MAX - 1);
						pFs->fsName[ITK_NVS_PATH_MAX - 1] = '\0';
					}
				}
			}
		}

		if (pFs == NULL)
		{
			if ( (eItkProfile != REDKEY_PROFILE_AUSTRALIA)
				 && (eItkProfile != REDKEY_PROFILE_NZ_HD)
				 && (eItkProfile != REDKEY_PROFILE_UK_HD)
				 && (eItkProfile != REDKEY_PROFILE_SOUTH_AFRICA)
				 && (eItkProfile != REDKEY_PROFILE_THAI)
				 && (eItkProfile != REDKEY_PROFILE_UK_FSAT)
				 && (eItkProfile != REDKEY_PROFILE_FREESATG2))
			{
				HxLOG_Debug(" not exists (fsType:%d, fsName:%s) !\n", fsType, fsName);
				return (itk_nvs_fs_t *)NULL;
			}
			else
			{
#if defined(ITK_NVS_SEARCH_FILE_WHEN_GET_TIME)
				size_t		minimumCapacity, metadataSize;
				itk_uint8_t	priority;
				char		dir[ITK_NVS_PATH_MAX];
				/*
					자료구조에서 검색을 못했다면 해당 경로에 file이 존재하는지 확인
					만약 존재한다면 자료구조에도 넣자
				*/
				memset(dir, 0, ITK_NVS_PATH_MAX);
				// error check???
				if (mheg_itk_port_nvsGetFullPath(fsType, fsName, NULL, dir, ITK_NVS_PATH_MAX) == TRUE)
				{
					if ( mheg_itk_port_nvsFsGetMetaFile(dir, &minimumCapacity, &priority, &metadataSize, 0, NULL) == TRUE )
					{
						HxLOG_Debug(" fsName(%s) Exists\n", fsName);

						pFs = (ITK_NvsFsHandle_t *)HLIB_STD_MemAlloc(sizeof(*pFs));
						if (pFs == NULL)
						{
							HxLOG_Error(" out of memory : pFs is NULL !\n");
							return (itk_nvs_fs_t *)NULL;
						}
						else
						{
							pFs->fsType = fsType;
							strncpy(pFs->fsName, fsName, ITK_NVS_PATH_MAX - 1);
							pFs->fsName[ITK_NVS_PATH_MAX - 1] = '\0';
							pFs->minimumCapacity = minimumCapacity;
							pFs->priority = priority;
							pFs->metadataSize = metadataSize;
							pFs->freeSpace = pFs->minimumCapacity;
							pFs->caller = (void *)pNvs;
							pFs->pPrev = pFs->pNext = NULL;
							pFs->ulNvsFileNum = 0;
							pFs->pHead = pFs->pTail = NULL;

							/* FS Handle append는 iterator생성 이후에 하자 */
							/* Append the fs pointer. */
							mheg_itk_port_nvsFsAppend(pFs, &pNvs->pHead, &pNvs->pTail);
							pNvs->ulNvsFsNum++;
						}
					}
					else
					{
						HxLOG_Debug(" not exists (fsType:%d, fsName:%s) !\n", fsType, fsName);
						return (itk_nvs_fs_t *)NULL;
					}
				}
				else
#endif
				{
					HxLOG_Debug(" not exists (fsType:%d, fsName:%s) !\n", fsType, fsName);
					return (itk_nvs_fs_t *)NULL;
				}
			}
		}
	}

	switch (fsType)
	{
		/*
			HD-FOX_T의 경우 Zapper모델로 FSA는 일단 NVM과 동일하게 간다
		*/
		case ITK_NVS_FS_FSA:
		case ITK_NVS_FS_NVM:
		case ITK_NVS_FS_PST:
		{
			piFs = (ITK_NvsFsIterator_t *)HLIB_STD_MemAlloc(sizeof(*piFs));
			if (piFs == NULL)
			{
				if (pFs)
					HLIB_STD_MemFree(pFs);

				HxLOG_Error(" out of memory : piFs is NULL !\n");
				return (itk_nvs_fs_t *)NULL;
			}
			//memset(piFs, 0, sizeof(*piFs));

			piFs->vtbl.release			= mheg_itk_port_nvsFsRelease;
			piFs->vtbl.next			= mheg_itk_port_nvsFsNext;
			piFs->vtbl.getNvsFile		= mheg_itk_port_nvsFsGetNvsFile;
			piFs->vtbl.newNvsFile		= mheg_itk_port_nvsFsNewNvsFile;
			piFs->vtbl.type			= mheg_itk_port_nvsFsType;
			piFs->vtbl.name			= mheg_itk_port_nvsFsName;
			piFs->vtbl.freeSpace		= mheg_itk_port_nvsFsFreeSpace;
			piFs->vtbl.metadataSize	= mheg_itk_port_nvsFsSizeMetaData;
			piFs->vtbl.readMetadata	= mheg_itk_port_nvsFsReadMetaData;
			piFs->vtbl.writeMetadata	= mheg_itk_port_nvsFsWriteMetaData;
			piFs->vtbl.erase			= mheg_itk_port_nvsFsErase;
			piFs->vtbl.enabled			= mheg_itk_port_nvsFsEnabled;

			if ( (eItkProfile != REDKEY_PROFILE_AUSTRALIA)
				 && (eItkProfile != REDKEY_PROFILE_UK_HD)
				 && (eItkProfile != REDKEY_PROFILE_NZ_HD)
				 && (eItkProfile != REDKEY_PROFILE_SOUTH_AFRICA)
				 && (eItkProfile != REDKEY_PROFILE_THAI))
			{
				piFs->vtbl.metadataSize		= NULL;
				piFs->vtbl.readMetadata		= NULL;
				piFs->vtbl.writeMetadata	= NULL;
			}

			if ( (eItkProfile != REDKEY_PROFILE_UK_HD)
				&& (eItkProfile != REDKEY_PROFILE_INTERNAL5)
				&& (eItkProfile != REDKEY_PROFILE_AUSTRALIA)
				&& (eItkProfile != REDKEY_PROFILE_NZ_HD)
				&& (eItkProfile != REDKEY_PROFILE_THAI) )
			{
				piFs->vtbl.enabled			= NULL;
			}

			piFs->pItem = pFs;
		}
		break;

		default:
		{
			HxLOG_Error(" not supported file system !\n");
		}	return (itk_nvs_fs_t *)NULL;
	}

	HxLOG_Debug(" nvs_fs_t[%p]\n", piFs);

	return (itk_nvs_fs_t *)piFs;
}

itk_nvs_fs_t *mheg_itk_port_nvsFsNewNvs(struct itk_nvs_t * thiz, itk_nvs_fs_type_t fsType,
		const char *fsName, size_t minimumCapacity, itk_uint8_t priority,
		itk_uint8_t const * metadata, size_t metadataSize)
{
	ITK_NvsFsIterator_t	*piFs = NULL;
	ITK_NvsFsHandle_t	*pFs = NULL;
	ITK_NvsIterator_t	*piNvs = NULL;
	ITK_NvsHandle_t		*pNvs = NULL;
	redkey_profile_t 	eItkProfile = s_mhegNvsInfo.itkProfile;

	HxLOG_Debug(C_CODE_F_BLACK_B_YELLOW" fsType:%d, fsName:%s, minimumCapacity:%d, priority:%d, metadata:%p, metadataSize:%d [%p]"C_CODE_RESET"\n",
		fsType, fsName, minimumCapacity, priority, metadata, metadataSize, thiz);

	if ((fsType != ITK_NVS_FS_DVR && fsType != ITK_NVS_FS_NVM && fsType != ITK_NVS_FS_FSA && fsType != ITK_NVS_FS_RAM && fsType != ITK_NVS_FS_PST)
		|| fsName == NULL)
	{
		HxLOG_Error(" invalid parameters !\n");
		return (itk_nvs_fs_t *)NULL;
	}

	piNvs = (ITK_NvsIterator_t *)thiz;
	if (piNvs == NULL)
	{
		HxLOG_Error(" thiz is NULL !\n");
		return (itk_nvs_fs_t *)NULL;
	}

	pNvs = piNvs->pItem;
	if (pNvs == NULL)
	{
		HxLOG_Error(" pNvs is NULL !\n");
		return (itk_nvs_fs_t *)NULL;
	}

	/*
		getNvsFs이후 없다면 newNvsFs이므로 file search하지 말고 자료구조에서 검색하자
	*/
	pFs = mheg_itk_port_nvsFsHandle(fsType, fsName, pNvs->pHead, pNvs->pTail);
	if (pFs != NULL)
	{
		HxLOG_Error(" already exists (fsType:%d, fsName:%s, pFs:%p) !\n", fsType, fsName, pFs);
		return (itk_nvs_fs_t *)NULL;
	}

	switch (fsType)
	{
		/*
			HD-FOX_T의 경우 Zapper모델로 FSA는 일단 NVM과 동일하게 간다
		*/
		case ITK_NVS_FS_FSA:
		case ITK_NVS_FS_NVM:
		case ITK_NVS_FS_PST:
		{
			char dir[ITK_NVS_PATH_MAX];

			piFs = (ITK_NvsFsIterator_t *)HLIB_STD_MemAlloc(sizeof(*piFs));
			if (piFs == NULL)
			{
				HxLOG_Error(" out of memory : piFs is NULL !\n");
				return (itk_nvs_fs_t *)NULL;
			}

			pFs = (ITK_NvsFsHandle_t *)HLIB_STD_MemAlloc(sizeof(*pFs));
			if (pFs == NULL)
			{
				HLIB_STD_MemFree(piFs);

				HxLOG_Error(" out of memory : pFs is NULL !\n");
				return (itk_nvs_fs_t *)NULL;
			}

			pFs->fsType = fsType;
			strncpy(pFs->fsName, fsName, ITK_NVS_PATH_MAX - 1);
			pFs->fsName[ITK_NVS_PATH_MAX - 1] = '\0';

			pFs->minimumCapacity = minimumCapacity;
			pFs->priority = priority;
			pFs->metadataSize = metadataSize;
			pFs->freeSpace = pFs->minimumCapacity;
			pFs->caller = (void *)pNvs;
			pFs->pPrev = pFs->pNext = NULL;
			pFs->ulNvsFileNum = 0;
			pFs->pHead = pFs->pTail = NULL;
			pFs->totaldataSize = 0;

			piFs->vtbl.release			= mheg_itk_port_nvsFsRelease;
			piFs->vtbl.next				= mheg_itk_port_nvsFsNext;
			piFs->vtbl.getNvsFile		= mheg_itk_port_nvsFsGetNvsFile;
			piFs->vtbl.newNvsFile		= mheg_itk_port_nvsFsNewNvsFile;
			piFs->vtbl.type				= mheg_itk_port_nvsFsType;
			piFs->vtbl.name				= mheg_itk_port_nvsFsName;
			piFs->vtbl.freeSpace		= mheg_itk_port_nvsFsFreeSpace;
			piFs->vtbl.metadataSize		= mheg_itk_port_nvsFsSizeMetaData;
			piFs->vtbl.readMetadata		= mheg_itk_port_nvsFsReadMetaData;
			piFs->vtbl.writeMetadata	= mheg_itk_port_nvsFsWriteMetaData;
			piFs->vtbl.erase			= mheg_itk_port_nvsFsErase;
			piFs->vtbl.enabled			= mheg_itk_port_nvsFsEnabled;
			piFs->pItem = pFs;

			if ( (eItkProfile != REDKEY_PROFILE_AUSTRALIA)
				 && (eItkProfile != REDKEY_PROFILE_UK_HD)
				 && (eItkProfile != REDKEY_PROFILE_NZ_HD)
				 && (eItkProfile != REDKEY_PROFILE_SOUTH_AFRICA)
				 && (eItkProfile != REDKEY_PROFILE_THAI))
			{
				piFs->vtbl.metadataSize		= NULL;
				piFs->vtbl.readMetadata		= NULL;
				piFs->vtbl.writeMetadata	= NULL;
			}

			if ( (eItkProfile != REDKEY_PROFILE_UK_HD)
				&& (eItkProfile != REDKEY_PROFILE_INTERNAL5)
				&& (eItkProfile != REDKEY_PROFILE_AUSTRALIA)
				&& (eItkProfile != REDKEY_PROFILE_NZ_HD)
				&& (eItkProfile != REDKEY_PROFILE_THAI) )
			{
				piFs->vtbl.enabled			= NULL;
			}

			memset(dir, 0, ITK_NVS_PATH_MAX);
			// error check???
			mheg_itk_port_nvsGetFullPath(fsType, fsName, NULL, dir, ITK_NVS_PATH_MAX);

			/* Make the new fs. */
			if (!mheg_itk_port_nvsIsDir(dir))
			{
				mheg_itk_port_nvsMkDir(dir); // make this directory
			}

			if ( (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
				 || (eItkProfile == REDKEY_PROFILE_UK_HD)
				 || (eItkProfile == REDKEY_PROFILE_NZ_HD)
				 || (eItkProfile == REDKEY_PROFILE_SOUTH_AFRICA)
				 || (eItkProfile == REDKEY_PROFILE_THAI)
				 || (eItkProfile == REDKEY_PROFILE_UK_FSAT)
				 || (eItkProfile == REDKEY_PROFILE_FREESATG2))
			{
				if (mheg_itk_port_nvsFsSetMetaFile(dir,	minimumCapacity, priority, metadataSize, metadataSize, metadata) != TRUE)
				{
					HLIB_STD_MemFree(pFs);
					HLIB_STD_MemFree(piFs);

					HxLOG_Error(" error !!\n");
					return (itk_nvs_fs_t *)NULL;
				}
			}

			/* Append the fs pointer. */
			mheg_itk_port_nvsFsAppend(pFs, &pNvs->pHead, &pNvs->pTail);
			//HxLOG_Debug(" FINAL! pHead(%p) pTail(%p)\n", pNvs->pHead, pNvs->pTail));

		}
		break;

		default:
		{
			HxLOG_Error(" not supported file system !\n");
		}	return (itk_nvs_fs_t *)NULL;
	}

	pNvs->ulNvsFsNum++;

	HxLOG_Debug(" nvs_handle[%p] itk_nvs_fs_t[%p] itk_nvs_t[%p]\n", pFs, piFs, thiz);

	return (itk_nvs_fs_t *)piFs;
}

itk_nvs_t *mheg_itk_port_nvsGetObject(void)
{
	ITK_NvsIterator_t	*piNvs = NULL;
	ITK_NvsHandle_t	*pNvs = NULL;

	piNvs = HLIB_STD_MemAlloc(sizeof(ITK_NvsIterator_t));
	if (piNvs == NULL)
	{
		HxLOG_Error("The NVS Iterator object cannot create because not enough memory!!\n");
		return (itk_nvs_t*)NULL;
	}

	pNvs = HLIB_STD_MemAlloc(sizeof(ITK_NvsHandle_t));
	if (pNvs == NULL)
	{
		HLIB_STD_MemFree(piNvs);

		HxLOG_Error("The NVS Handle object cannot create because not enough memory!!\n");
		return (itk_nvs_t*)NULL;
	}

	pNvs->ulNvsFsNum = 0;
	pNvs->pHead = pNvs->pTail = NULL;

	piNvs->vtbl.getNvsFs = mheg_itk_port_nvsFsGetNvs;
	piNvs->vtbl.newNvsFs = mheg_itk_port_nvsFsNewNvs;
	piNvs->pItem = pNvs;

	return (itk_nvs_t*)piNvs;
}

void mheg_itk_port_nvsGetFileObject(itk_nvs_t *Nvs)
{
	ITK_NvsIterator_t	*piNvs = (ITK_NvsIterator_t*)Nvs;
	ITK_NvsHandle_t	*pNvs = NULL;

	redkey_profile_t eItkProfile = s_mhegNvsInfo.itkProfile;

	if (piNvs == NULL)
	{
		HxLOG_Error(" NVS Iterator is NULL!!\n");
		return;
	}

	pNvs = piNvs->pItem;

// search all directory
	if ( (eItkProfile == REDKEY_PROFILE_UK_FSAT)
		|| (eItkProfile == REDKEY_PROFILE_FREESATG2)
		|| (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
		|| (eItkProfile == REDKEY_PROFILE_SOUTH_AFRICA) )
	{
		if (mheg_itk_port_nvsFsOpen(pNvs, ITK_NVS_FS_NVM) != TRUE)
		{
			mheg_itk_port_nvsFsClose(pNvs, ITK_NVS_FS_NVM, eMHEG_NVS_FACTORY_RESET);

			//		HLIB_STD_MemFree(pNvs);
			//		HLIB_STD_MemFree(piNvs);

			HxLOG_Error(" NVM Open error !\n");
			//		return (itk_nvs_t *)NULL;
		}
	}

	if ( (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
		|| (eItkProfile == REDKEY_PROFILE_SOUTH_AFRICA) )
	{
		if (mheg_itk_port_nvsFsOpen(pNvs, ITK_NVS_FS_FSA) != TRUE)
		{
			mheg_itk_port_nvsFsClose(pNvs, ITK_NVS_FS_FSA, eMHEG_NVS_FACTORY_RESET);

			//		HLIB_STD_MemFree(pNvs);
			//		HLIB_STD_MemFree(piNvs);

			HxLOG_Error(" FSA Open error !\n");
			//		return (itk_nvs_t *)NULL;
		}
	}

	if ( (eItkProfile == REDKEY_PROFILE_UK_HD)
		|| (eItkProfile == REDKEY_PROFILE_INTERNAL5)
		|| (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
		|| (eItkProfile == REDKEY_PROFILE_NZ_HD)
		|| (eItkProfile == REDKEY_PROFILE_THAI) )
	{
		if (mheg_itk_port_nvsFsOpen(pNvs, ITK_NVS_FS_PST) != TRUE)
		{
			mheg_itk_port_nvsFsClose(pNvs, ITK_NVS_FS_PST, eMHEG_NVS_FACTORY_RESET);

			//		HLIB_STD_MemFree(pNvs);
			//		HLIB_STD_MemFree(piNvs);

			HxLOG_Error(" PST Open error !\n");
			//		return (itk_nvs_t *)NULL;
		}
	}
}

extern char **environ;
void mheg_itk_port_nvsRemoveFs(Mheg_Nvs_Fs_Type_t MhegFsType)
{
	/* rjlee. work-around */
#if 1
	char szCmd[50];
	pid_t pid;
	int status;
	int nBufLen = sizeof(szCmd);

#if defined(REMOVE_ALL_NVS_FS_WHEN_FACTORY_RESET)
	strncpy(szCmd, "rm -rf " ITK_NVS_DIR "/*", nBufLen - 1);
#else
	strncpy(szCmd, "rm -rf "ITK_NVS_FS_FSA_BASE_PATH"/*", nBufLen - 1);
#endif
	szCmd[nBufLen - 1] = '\0';

	pid = vfork();
	if (pid == 0)
	{
		char *argv[4];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = szCmd;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);

		exit(0);
	}

	do
	{
		if (waitpid(pid, &status, 0) == -1)
		{
			HxLOG_Error("pid == -1, errno = %d\n", errno);
			if (errno != EINTR)
				return;
		}
		else
		{
			return;
		}
	} while(1);

#endif
}

#define _____PUBLIC_APIs________________________________________________________________
/* external variables  and functions */

void PORT_ITK_Nvs_ResetFs(Mheg_Nvs_Fs_Type_t MhegFsType)
{
	ITK_NvsIterator_t	*piNvs = NULL;
	ITK_NvsHandle_t	*pNvs = NULL;

	HxLOG_Debug(C_CODE_F_WHITE_B_BLUE" RESET FS!!(MhegFsType:0x%x)"C_CODE_RESET"\n", MhegFsType);

	if ((MhegFsType & eMHEG_NVS_FS_DVR) ||
		(MhegFsType & eMHEG_NVS_FS_NVM) ||
		(MhegFsType & eMHEG_NVS_FS_FSA) ||
		(MhegFsType & eMHEG_NVS_FS_RAM) ||
		(MhegFsType & eMHEG_NVS_FS_PST) ||
		(MhegFsType & eMHEG_NVS_FACTORY_RESET))
	{
		/* correct type */
	}
	else
	{
		HxLOG_Error(" Invalid FS type!!!\n");
		return;
	}

	piNvs = (ITK_NvsIterator_t*)s_mhegNvsInfo.pObject;
	if (piNvs == NULL)
	{
		HxLOG_Error(" Nvs object is NULL !\n");
		return;
	}

	pNvs = piNvs->pItem;
	if (pNvs == NULL)
	{
		HxLOG_Error(" pNvs is NULL !\n");
		return;
	}

	if (MhegFsType == eMHEG_NVS_FACTORY_RESET)
	{
		MhegFsType |= (eMHEG_NVS_FS_DVR | eMHEG_NVS_FS_NVM | eMHEG_NVS_FS_FSA | eMHEG_NVS_FS_RAM | eMHEG_NVS_FS_PST);
	}

	/* FS는 많지 않으므로 여러번 수행해도 큰 부담이 없다 */
	if (MhegFsType & eMHEG_NVS_FS_DVR)
	{
		mheg_itk_port_nvsFsClose(pNvs, ITK_NVS_FS_DVR, MhegFsType);
	}
	if (MhegFsType & eMHEG_NVS_FS_NVM)
	{
		mheg_itk_port_nvsFsClose(pNvs, ITK_NVS_FS_NVM, MhegFsType);
	}
	if (MhegFsType & eMHEG_NVS_FS_FSA)
	{
		mheg_itk_port_nvsFsClose(pNvs, ITK_NVS_FS_FSA, MhegFsType);
	}
	if (MhegFsType & eMHEG_NVS_FS_RAM)
	{
		mheg_itk_port_nvsFsClose(pNvs, ITK_NVS_FS_RAM, MhegFsType);
	}
	if (MhegFsType & eMHEG_NVS_FS_PST)
	{
		mheg_itk_port_nvsFsClose(pNvs, ITK_NVS_FS_PST, MhegFsType);
	}

#if 0
	pNvs->ulNvsFsNum = 0;
	pNvs->pHead = pNvs->pTail = NULL;
#endif

	piNvs->vtbl.getNvsFs = mheg_itk_port_nvsFsGetNvs;
	piNvs->vtbl.newNvsFs = mheg_itk_port_nvsFsNewNvs;
	piNvs->pItem = pNvs;

	/*
		delete folder
		이곳에서 하나씩 지워도 자료구조에 없는 놈들은 지워지지 않는 문제가 있다.
		따라서 자료구조만 모두 clear하고 file은 폴더와 통째로 한방에 날리는것이 좋다
	*/
#if 0
	local_itk_NVS_RmDir(ITK_NVS_FS_NVM_BASE_PATH);
	local_itk_NVS_RmDir(ITK_NVS_FS_FSA_BASE_PATH);
#endif

	mheg_itk_port_nvsRemoveFs(MhegFsType);
}

void PORT_ITK_Nvs_RegisterObject(void)
{
	ITK_registerManager(ITK_NVS, s_mhegNvsInfo.pObject);
}

void PORT_ITK_Nvs_CreateObject(redkey_profile_t eItkProfile)
{
	s_mhegNvsInfo.itkProfile = eItkProfile;
	s_mhegNvsInfo.pObject = mheg_itk_port_nvsGetObject();

	mheg_itk_port_nvsGetFileObject(s_mhegNvsInfo.pObject);

	if (s_mhegNvsInfo.pObject)
	{
		ITK_registerManager(ITK_NVS, s_mhegNvsInfo.pObject);
	}
	HxLOG_Debug(" nvs_fs_t(%p)\n", s_mhegNvsInfo.pObject);
}


