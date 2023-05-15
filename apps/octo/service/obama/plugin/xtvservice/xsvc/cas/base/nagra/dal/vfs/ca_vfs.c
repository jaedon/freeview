/**************************************************************************************************/
#define ________CA_SEC_Private_Include_____________________________________________________
/**************************************************************************************************/
#include <na_glob.h>
#include <_svc_cas_mgr_common.h>


/**************************************************************************************************/
#define ________CA_SEC_Golbal_Value________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define ________CA_RHP_Private_Define_______________________________________________________
/**************************************************************************************************/
#define	KILOBYTE					(1024)
#define	MEGABYTE					(1024*1024)

#define CA_VFS_MAX_PARTITION_SIZE	(2*MEGABYTE)
#define CA_VFS_MAX_FILE_NAME_LEN	64
#define CA_VFS_MAX_DIR_NAME_LEN		64

#define CA_VFS_ROOT_PATH			CAS_NA_JFFS2_PATH "vfs"

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST) //DALTS 3.31.0에 있는 bug 때문에 임시로 추가. DAL Library가 수정되면 삭제 필요.
//#define	CA_VFS_DALTS_BUG_FIX
#endif

/* pxDirectoryName 이 empty string일 경우 "/"로 간주 하도록 Spec이 update되면 아래 Define 활성화. */
#define	CA_VFS_EMPTY_STRING_AS_ROOT


/**************************************************************************************************/
#define ________CA_SEC_Private_Type_Define_________________________________________________
/**************************************************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define VFSLogTrace(message) 	//(HLIB_CMD_Printf(C_CODE_F_PURPLE"[CA VFS](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define VFSLogError(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED"[CA VFS](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define VFSLogWarning(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED_B_BLACK"[CA VFS](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))


#if defined (HxLOG_Print)
#undef	HxLOG_Print
#define HxLOG_Print(...)	VFSLogTrace((__VA_ARGS__))
#endif

#if defined (HxLOG_Error)
#undef	HxLOG_Error
#define HxLOG_Error(...)	VFSLogError((__VA_ARGS__))
#endif

#if defined (HxLOG_Warning)
#undef	HxLOG_Warning
#define HxLOG_Warning(...)	VFSLogWarning((__VA_ARGS__))
#endif

#endif

typedef struct
{
	HxFILE_t		hHLibFile;
	TVfsOpenFlags	xOpenFlags;
	HCHAR			*pszFileName;
}CaVfsFileHandle_t;


typedef struct
{
	HxDIR_t					*phDir;
	HBOOL					bEndOfRead;
	HUINT32					ulEntryCnt;
	TVfsDirectoryEntry		*pxDirectoryEntry;
}CaVfsDirectoryHandle_t;

typedef struct
{
	HxList_t				*phFileList;
	HxList_t				*phDirList;
	HUINT32					ulMaxSize;
	HUINT32					ulUsedSize;
}CaVfsContext_t;

/**************************************************************************************************/
#define ________CA_SEC_Private_Static_Value_________________________________________________
/**************************************************************************************************/
STATIC CaVfsContext_t s_stCaVfsContext;

/**************************************************************************************************/
#define ________CA_SEC_Private_Static_Prototype______________________________________________
/**************************************************************************************************/
STATIC CaVfsContext_t*	local_na_vfs_GetContext(void);
STATIC HERROR	local_na_vfs_InitContext(void);
STATIC HERROR	local_na_vfs_MakeFilePathName(const HCHAR *pszFileName, HCHAR *pucFullPath, HUINT32 ulBufLen);
STATIC HERROR	local_na_vfs_InitRootPath(void);
STATIC HERROR	local_na_vfs_AddFileNameInHandle(CaVfsFileHandle_t *phFile, const HCHAR *szFileName);
STATIC HERROR	local_na_vfs_FreeFileNameInHandle(CaVfsFileHandle_t *phFile);
STATIC HERROR	local_na_vfs_AppendFileHandleInList(CaVfsFileHandle_t *pstFileHandle);
STATIC HBOOL	local_na_vfs_IsFileHandleExistInList(CaVfsFileHandle_t *pstFileHandle);
STATIC HERROR	local_na_vfs_RemoveFileHandleInList(CaVfsFileHandle_t *pstFileHandle);
STATIC HBOOL	local_na_vfs_CB_FindByFileName(void *pvUserData, void *pvListData);
STATIC HBOOL	local_na_vfs_IsFileHandleExistByNameInList(const HCHAR *pszFileName);
STATIC HERROR	local_na_vfs_AppendDirectoryHandleInList(CaVfsDirectoryHandle_t *pstDirHandle);
STATIC HERROR	local_na_vfs_RemoveDirectoryHandleInList(CaVfsDirectoryHandle_t *pstDirHandle);
STATIC HBOOL	local_na_vfs_IsDirectoryHandleExistInList(CaVfsDirectoryHandle_t *pstDirHandle);
//STATIC HBOOL	local_na_vfs_CB_FindByDirName(void *pvUserData, void *pvListData);
//STATIC HBOOL	local_na_vfs_IsDirHandleExistByNameInList(const HCHAR *pszDirName);
STATIC HERROR	local_na_vfs_CheckDirInFileName(const HCHAR* pszFileName);
STATIC HERROR	local_na_vfs_CheckDirInOldAndNewFile(const HCHAR* pszOldName, const HCHAR* pszNewName, HBOOL *pbDiffDir);
//STATIC HBOOL	local_na_vfs_AreThereOpendDirctories(const HCHAR *pszName);
STATIC HBOOL	local_na_vfs_isDirectoryEmpty(const HCHAR * pszDirMame);
STATIC HERROR	local_na_vfs_RemoveEmptyDirctories(const HCHAR *pszDirName);
STATIC HERROR	local_na_vfs_DirGetEntryCountInDir(const HCHAR *pszDirName, HUINT32 *pulEntCnt);
STATIC HBOOL	local_na_vfs_FileExist(const HCHAR *pszName);
STATIC HxFILE_t	local_na_vfs_FileOpen(const HCHAR *pszFileName, const HCHAR *pszMode);
STATIC HINT32	local_na_vfs_FileClose(HxFILE_t hSelf);
STATIC HUINT32	local_na_vfs_FileRead(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count);
STATIC HUINT32	local_na_vfs_FileWirte(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count);
STATIC HERROR	local_na_vfs_FileMake(const HCHAR *pszName);
STATIC HERROR	local_na_vfs_FileMove(const HCHAR *pszOldName, const HCHAR *pszNewName);
STATIC HERROR	local_na_vfs_FileDelete(const HCHAR *pszName);
STATIC HERROR	local_na_vfs_FileSeek(HxFILE_t hSelf, HOffset_t nOffset, HxFILE_Seek_e eOrigin, HOffset_t *nRetPos);
STATIC HERROR	local_na_vfs_FileTell(HxFILE_t hSelf, HOffset_t *nRetPos);
STATIC HERROR	local_na_vfs_FileSize(HxFILE_t hSelf, HUINT64 *pullRetSize);
STATIC HINT32	local_na_vfs_FileFlush(HxFILE_t hSelf);
STATIC HxDIR_t*	local_na_vfs_DirOpen(const HCHAR	*pszPath);
STATIC HERROR	local_na_vfs_DirClose(HxDIR_t *pSelf);
STATIC HCHAR*	local_na_vfs_DirGetName(HxDIR_t *pSelf);
STATIC HCHAR* 	local_na_vfs_DirGetPath(HxDIR_t *pSelf);
STATIC HBOOL	local_na_vfs_IsDir(HxDIR_t *pSelf);
//STATIC HBOOL	local_na_vfs_IsFile(HxDIR_t *pSelf);
STATIC HBOOL	local_na_vfs_IsDirExist(const HCHAR *pszDirName);
STATIC HERROR 	local_na_vfs_DirMake(const HCHAR *pszPath);
//STATIC HERROR	local_na_vfs_DirGetFileSize(HxDIR_t *pSelf, HUINT64 *pullSize);
STATIC HERROR	local_na_vfs_DirRemove(const HCHAR *pszPath);
STATIC HERROR	local_na_vfs_DirGetTotalSize(const HCHAR *pszPath, HUINT32 *pulSize);


/**************************************************************************************************/
#define ________CA_SEC_Public_Functions_Body_______________________________________________
/**************************************************************************************************/
/**
 *  @brief
 *    This function creates an empty file and opens the current file for reading
 *    and writing. The name of the file is defined by \c pxFileName. It is an
 *    absolute path that may contain directories separated by a '/'. The VFS
 *    driver is expected to create all the intermediate directories if they do
 *    not exist.
 *
 *    The creating mode is defined by \c xCreateMode. If the file is created
 *    with the \c VFS_CREATE_NEW mode and the specified file already exists, the
 *    file is not modified and the function returns \c VFS_ERROR_ACCESS. If the
 *    \c VFS_CREATE_ALWAYS mode is set and the specified file already exists,
 *    the function overwrites the file and clears the existing attributes. The
 *    new file handler is returned in \c pxFileHandle. If the call fails, this
 *    value is set to \c VFS_FILE_HANDLE_INVALID.
 *
 *  @param[out] pxFileHandle
 *    File handle returned by this call.
 *  @param[in]  pxFileName
 *    Absolute filename of the file to be open.
 *  @param[in]  xCreateMode
 *    File creation mode.
 *
 *  @retval VFS_NO_ERROR
 *            Success to create the file.
 *  @retval VFS_ERROR_LOW_MEMORY
 *            The system has no enough free memory to perform the requested
 *            operation.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR_FILESYSTEM_FULL
 *            The physical file system has not enough space left for the
 *            requested operation.
 *  @retval VFS_ERROR_ACCESS
 *            The file already exists and the function is called in the
 *            VFS_CREATE_NEW mode.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsCreateFile (TVfsFileHandle* pxFileHandle, const TChar* pxFileName, TVfsCreateMode xCreateMode)
{
	HxFILE_t			hHLibFile;
	CaVfsFileHandle_t	*phNewFile = NULL;
	HCHAR				aucFullPath[512] = {0,};
	HCHAR				*pucMode = NULL;
	CaVfsContext_t		*pContext = local_na_vfs_GetContext();

	HxLOG_Print("vfsCreateFile is called [%s][%d]\n", pxFileName, xCreateMode);

	if(pxFileHandle == NULL)
	{
		HxLOG_Error("pxFileHandle is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	*pxFileHandle = VFS_FILE_HANDLE_INVALID;

	if(pxFileName == NULL)
	{
		HxLOG_Error("pxFileName is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	if(local_na_vfs_MakeFilePathName(pxFileName, aucFullPath, sizeof(aucFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	/* the  mandatory  treatment  of  automatic  directory  creation/deletion  on  a  VFS implementation supporting the notion of directories.  */
	if(local_na_vfs_CheckDirInFileName((const HCHAR*)aucFullPath) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_CheckDirInFileName].\n");
		return VFS_ERROR;
	}

	/*The  physical  file  system  has  not  enough  space  left	for  the requested operation*/
	if(pContext->ulUsedSize >= pContext->ulMaxSize)
	{
		HxLOG_Print("Used File System size Max[%u] used[%u]\n", pContext->ulMaxSize, pContext->ulUsedSize);
		return VFS_ERROR_FILESYSTEM_FULL;
	}

	/**< Creates a new file. If the file exists, the creation fails. */
	if(xCreateMode == VFS_CREATE_NEW)
	{
		/* The  file  already  exists */
		if(local_na_vfs_FileExist((const HCHAR *)aucFullPath) == TRUE)
		{
			HxLOG_Error("The  file  already  exists [%s]\n", aucFullPath);
			return VFS_ERROR_ACCESS;
		}
		pucMode = "wb";
		/* 'b' is for binary mode. */
		/* w	   :	Truncate file to zero length or create text file for writing.  The stream is positioned at the beginning of the file.*/

	}
	else if(xCreateMode == VFS_CREATE_ALWAYS)
	{
		/* If  the  VFS_CREATE_ALWAYS  mode  is  set  and  the specified file already exists, the function overwrites the file and clears the existing attributes */
		if(local_na_vfs_FileExist((const HCHAR *)aucFullPath) == TRUE)
		{
			pucMode = "w+b"; //1 무조건 w+b로 해도 될 듯 한데 "wb"와 "w+b"로 그냥 구분해 본다.
			/* w+ :	Open for reading and writing.  The file is created if it does not  exist, otherwise it is truncated. The stream is positioned at the beginning of  the file. */
		}
		else
		{
			pucMode = "wb";
			/* w	   :	Truncate file to zero length or create text file for writing.  The stream is positioned at the beginning of the file.*/
		}
	}
	else
	{
		HxLOG_Error("Invalid Create Mode [%d]\n", xCreateMode);
		return VFS_ERROR_BAD_PARAMETER;
	}


	if(local_na_vfs_FileMake((const HCHAR *) aucFullPath) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileMake] [%s]\n", aucFullPath);
		return VFS_ERROR;
	}

	hHLibFile = local_na_vfs_FileOpen((const HCHAR *) aucFullPath, (const HCHAR *)pucMode);
	if(hHLibFile == NULL)
	{
		HxLOG_Error("Error in [local_na_vfs_FileOpen] [%s]\n", aucFullPath);
		return VFS_ERROR;
	}

	phNewFile = (CaVfsFileHandle_t *)OxCAS_Malloc(sizeof(CaVfsFileHandle_t));
	if(phNewFile == NULL)
	{
		HxLOG_Error("Error in [OxCAS_Malloc] NULL\n");
		return VFS_ERROR_LOW_MEMORY;
	}

	phNewFile->hHLibFile = hHLibFile;
	phNewFile->xOpenFlags = VFS_FILE_OPEN_READ_WRITE;

	if(local_na_vfs_AddFileNameInHandle(phNewFile, pxFileName) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_AddFileNameInHandle] NULL\n");
		OxCAS_Free(phNewFile);
		return VFS_ERROR_LOW_MEMORY;
	}

	*pxFileHandle = (TVfsFileHandle)phNewFile;

	/* Register the opend handel to File handle list */
	if(local_na_vfs_AppendFileHandleInList(phNewFile) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_AppendFileHandleInList]\n");
	}

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function opens a file for reading or writing. The name of the file
 *    is defined by \c pxFileName.
 *
 *    The opening mode is defined by the flags \c xOpenFlags. One and only one
 *    of the flags \c VFS_FILE_OPEN_READ_ONLY, \c VFS_FILE_OPEN_WRITE_ONLY or
 *    \c VFS_FILE_OPEN_READ_WRITE must be set. If not, the call fails.
 *
 *    Both \c VFS_OPEN_TRUNCATE and \c VFS_FILE_OPEN_APPEND are optional. Both
 *    are relevant in writing operations, ignored otherwise. If
 *    \c VFS_OPEN_TRUNCATE is set, the current content of the file is discarded
 *    before the call returns. If \c VFS_FILE_OPEN_APPEND is set, the seek
 *    pointer is moved at the end of the file before each write operation. On a
 *    successful open, the seek pointer is set at the beginning of the file.
 *    The new file handler is returned in \c pxFileHandle. If the call fails,
 *    this value is set to \c VFS_FILE_HANDLE_INVALID.
 *
 *  @param[out] pxFileHandle
 *    File handle returned by this call.
 *  @param[in]  pxFileName
 *    Absolute filename of the file to be open.
 *  @param[in]  xOpenFlags
 *    File access type.
 *
 *  @retval VFS_NO_ERROR
 *            Success to open file.
 *  @retval VFS_ERROR_LOW_MEMORY
 *            The system has no enough free memory to perform the requested
 *            operation.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR_NO_SUCH_FILE
 *            The requested file does not exist (opening an inexistent file
 *            for reading).
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsOpenFile(TVfsFileHandle* pxFileHandle,const TChar* pxFileName, TVfsOpenFlags xOpenFlags)
{
	CaVfsContext_t		*pContext = local_na_vfs_GetContext();
	HBOOL 				bTruncate = FALSE, bAppend = FALSE, bExisted = FALSE;
	HxFILE_t			hHlibFile;
	HCHAR				*pucMode = NULL;
	CaVfsFileHandle_t	*phFile = NULL;
	HCHAR				aucFullPath[512] = {0,};

	HxLOG_Print("vfsOpenFile is called [%s][0x%04x]\n", pxFileName, xOpenFlags);

	if(pxFileHandle == NULL)
	{
		HxLOG_Error("pxFileHandle is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	*pxFileHandle = VFS_FILE_HANDLE_INVALID;

	if(pxFileName == NULL)
	{
		HxLOG_Error("pxFileName is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	if(local_na_vfs_MakeFilePathName(pxFileName, aucFullPath, sizeof(aucFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	/* If the error occured, the call must fail and an VFS_FILE_HANDLE_INVALID should be handed back. */
	*pxFileHandle = VFS_FILE_HANDLE_INVALID;

	bExisted = local_na_vfs_FileExist((const HCHAR *)aucFullPath);

	bAppend =  (xOpenFlags & VFS_FILE_OPEN_APPEND)? TRUE:FALSE;
	bTruncate = (xOpenFlags & VFS_FILE_OPEN_TRUNCATE)? TRUE:FALSE;

	if(xOpenFlags & VFS_FILE_OPEN_READ_ONLY)
	{
		/**< The requested file does not exist *(typically if opening an inexistent file for reading)*/
		if(bExisted != TRUE)
		{
			HxLOG_Error("The requested file does not exist\n");
			return VFS_ERROR_NO_SUCH_FILE;
		}
		pucMode = "rb";
		/* r	:	Open text file for reading. The stream is positioned at the beginning of  the file.  */
	}
	else if(xOpenFlags & VFS_FILE_OPEN_WRITE_ONLY)
	{
		/* If VFS_OPEN_TRUNCATE is set, the current content of the file is discarded before the call returns. */
		if(bTruncate)
		{
			pucMode = "wb";
			/* w	   :	Truncate file to zero length or create text file for writing.  The stream is positioned at the beginning of the file.*/
		}
		else if(bAppend)
		{
			pucMode = "ab";
			/* a    :	Open for appending (writing at end of file).  The file is created  if  it does not exist.  The stream is positioned at the end of the file. */
		}
		else
		{
			pucMode = "wb";
		}
	}
	else if(xOpenFlags & VFS_FILE_OPEN_READ_WRITE)
	{
		/**< The requested file does not exist *(typically if opening an inexistent file for reading)*/
		if(bExisted != TRUE)
		{
			HxLOG_Error("The requested file does not exist\n");
			return VFS_ERROR_NO_SUCH_FILE;
		}

		/* If VFS_OPEN_TRUNCATE is set, the current content of the file is discarded before the call returns. */
		if(bTruncate)
		{
			pucMode = "w+b";
			/* w+ :	Open for reading and writing.  The file is created if it does not  exist, otherwise it is truncated. The stream is positioned at the beginning of  the file. */
		}
		else if(bAppend)
		{
			pucMode = "a+b";
			/* a+ :	Open for appending (writing at end of file).  The file is created  if  it does not exist.  The stream is positioned at the end of the file. */

		}
		else
		{
			pucMode = "r+b";
			/* r+ :	Open for reading and writing.  The stream is positioned at the  beginning of the file. */
		}
	}
	else
	{
		HxLOG_Error("Invalid Open Flag [0x%04x]\n", xOpenFlags);
		return VFS_ERROR_BAD_PARAMETER;
	}

	hHlibFile = local_na_vfs_FileOpen((const HCHAR *)aucFullPath, (const HCHAR *)pucMode);
	if(hHlibFile == NULL)
	{
		HxLOG_Error("Error in [local_na_vfs_FileOpen] [%s][%s]\n", aucFullPath, pucMode);
		return VFS_ERROR;
	}

	if(bAppend)
	{
		/*	If the file was opened with the VFS_FILE_OPEN_APPEND mode, the seek pointer of the file is set at the end of the file before each write. */
		if(local_na_vfs_FileSeek(hHlibFile, 0, eHxFILE_SEEK_END, NULL) != ERR_OK)
		{
			HxLOG_Error("Error in [local_na_vfs_FileSeek]\n");
			return VFS_ERROR;
		}
	}

	phFile = (CaVfsFileHandle_t *)OxCAS_Malloc(sizeof(CaVfsFileHandle_t));
	if(phFile == NULL)
	{
		HxLOG_Error("Error in [OxCAS_Malloc] NULL\n");
		return VFS_ERROR_LOW_MEMORY;
	}

	phFile->hHLibFile = hHlibFile;
	phFile->xOpenFlags = xOpenFlags;

	if(local_na_vfs_AddFileNameInHandle(phFile, pxFileName) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_AddFileNameInHandle]\n");
		return VFS_ERROR_LOW_MEMORY;
	}

	HxLOG_Print("[vfsOpenFile] Opened Handle[0x%x]\n", phFile);

	*pxFileHandle = (TVfsFileHandle)phFile;

	/* Register the opend handel to File handle list */
	if(local_na_vfs_AppendFileHandleInList(phFile) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_AppendFileHandleInList]\n");
	}

	/* Update a tatal used size */
	if(local_na_vfs_DirGetTotalSize(CA_VFS_ROOT_PATH, &pContext->ulUsedSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_DirGetTotalSize].\n");
		return ERR_FAIL;
	}

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function closes a previously opened file.
 *
 *  @param[in]  xFileHandle
 *    Handle of the file to close.
 *
 *  @retval VFS_NO_ERROR
 *            Success to close file.
 *  @retval VFS_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsCloseFile(TVfsFileHandle xFileHandle)
{
	CaVfsFileHandle_t	*phFile = NULL;
	CaVfsContext_t		*pContext = local_na_vfs_GetContext();

	HxLOG_Print("vfsCloseFile is called [0x%x]\n", xFileHandle);

	if(xFileHandle == VFS_FILE_HANDLE_INVALID)
	{
		HxLOG_Error("xFileHandle is invalid\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	phFile = (CaVfsFileHandle_t *)xFileHandle;
	if(phFile == NULL)
	{
		HxLOG_Error("hFile is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	/* Have not been opend or already be closed. */
	if(local_na_vfs_IsFileHandleExistInList(phFile) != TRUE)
	{
		HxLOG_Error("The handle has never been opend. [0x%x]\n", phFile);
		return VFS_ERROR_BAD_HANDLE;
	}
	else
	{
		/* Unregister the opend handel from File handle list */
		if(local_na_vfs_RemoveFileHandleInList(phFile) != ERR_OK)
		{
			HxLOG_Error("Error in [local_na_vfs_AppendFileHandleInList] NULL\n");
		}
	}

	local_na_vfs_FileClose(phFile->hHLibFile);

	if(local_na_vfs_FreeFileNameInHandle(phFile) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FreeFileNameInHandle]\n");
		return VFS_ERROR_LOW_MEMORY;
	}

	OxCAS_Free(phFile);


	/* Update a tatal used size */
	if(local_na_vfs_DirGetTotalSize(CA_VFS_ROOT_PATH, &pContext->ulUsedSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_DirGetTotalSize].\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Used File System size [%u]\n", pContext->ulUsedSize);

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function removes an existing file from the file system.
 *
 *    Its content is discarded. The \c pxFileName string is an absolute path
 *    that may contain directories. If those directories are empty, they shall
 *    be removed as well (a directory is considered "empty" if it contains
 *    either no file or only "empty" directories).
 *
 *  @param[in]  pxFileName
 *    Absolute filename of the file to be removed from the file system
 *
 *  @retval VFS_NO_ERROR
 *            Success to remove file.
 *  @retval VFS_ERROR_NO_SUCH_FILE
 *            The requested file does not exist.
 *  @retval VFS_ERROR_ACCESS
 *            The requested operation cannot be performed, as the file is
 *            currently accessed.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsRemoveFile(const TChar* pxFileName)
{
	HCHAR				aucFullPath[512] = {0,};
	CaVfsContext_t		*pContext = local_na_vfs_GetContext();

	HxLOG_Print("vfsRemoveFile is called [%s]\n", pxFileName);

	if(pxFileName == NULL)
	{
		HxLOG_Error("pxFileName is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	/* To access a opened file */
	if(local_na_vfs_IsFileHandleExistByNameInList(pxFileName) == TRUE)
	{
		HxLOG_Error("The file is currently accessed. [%s]\n", pxFileName);
		return VFS_ERROR_ACCESS;
	}

	if(local_na_vfs_MakeFilePathName(pxFileName, aucFullPath, sizeof(aucFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	/**< The requested file does not exist */
	if(local_na_vfs_FileExist((const HCHAR *)aucFullPath) != TRUE)
	{
		HxLOG_Error("The requested file does not exist. [%s]\n", aucFullPath);
		return VFS_ERROR_NO_SUCH_FILE;
	}

	/* The  requested  operation  cannot  be  performed,  as  the  file  is currently accessed. */
	if(local_na_vfs_FileDelete((const HCHAR *)aucFullPath) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileDelete] The file is currently accessed. [%s]\n", aucFullPath);
		return VFS_ERROR_ACCESS;
	}

	/* the  mandatory  treatment  of  automatic  directory  creation/deletion  on  a  VFS implementation supporting the notion of directories.  */
	if(local_na_vfs_RemoveEmptyDirctories((const HCHAR *)aucFullPath) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_RemoveEmptyDirctories][%s]\n", aucFullPath);
		return VFS_ERROR;
	}

	/* Update a tatal used size */
	if(local_na_vfs_DirGetTotalSize(CA_VFS_ROOT_PATH, &pContext->ulUsedSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_DirGetTotalSize].\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Used File System size [%u]\n", pContext->ulUsedSize);

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function renames an existing file. This function is only used to
 *    rename files and not directories.
 *
 *    If a file with the new name already exists, the rename operation fails
 *    and \c VFS_ERROR is returned. If the file path is invalid and a file with
 *    the new name already exists, the rename operation fails and
 *    \c VFS_ERROR_NO_SUCH_FILE is returned.
 *
 *  @param[in]  pxOldName
 *    Old name of the file to rename.
 *  @param[in]  pxNewName
 *    New name of the file to rename.
 *
 *  @retval VFS_NO_ERROR
 *            Success to rename the file.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR_NO_SUCH_FILE
 *            The requested file or directory does not exist.
 *  @retval VFS_ERROR_ACCESS
 *            The object cannot be rename because it is used by the system.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsRenameFile(const TChar* pxOldName, const TChar* pxNewName)
{
	HCHAR	aucOldFullPath[512] = {0,}, aucNewFullPath[512] = {0,};
	HBOOL	bDiffDir = FALSE;

	HxLOG_Print("vfsRenameFile is called Old[%s], New[%s]\n", pxOldName, pxNewName);

	if(pxOldName == NULL || pxNewName == NULL)
	{
		HxLOG_Error("pxOldName or pxNewName is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	if(HxSTD_StrCmp(pxOldName, pxNewName) == 0)
	{
		HxLOG_Print("The names are identical. No need to be renamed.\n");
		return VFS_NO_ERROR;
	}

	if(local_na_vfs_MakeFilePathName(pxOldName, aucOldFullPath, sizeof(aucOldFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	/* The requested file or directory does not exist. */
	if(local_na_vfs_FileExist((const HCHAR *)aucOldFullPath) != TRUE)
	{
		HxLOG_Error("The requested file does not exist. [%s]\n", aucOldFullPath);
		return VFS_ERROR_NO_SUCH_FILE;
	}

	if(local_na_vfs_MakeFilePathName(pxNewName, aucNewFullPath, sizeof(aucNewFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	/* If  a  file  with  the  new  name  already  exists,  the  rename  operation  fails  and  VFS_ERROR  is returned. */
	if(local_na_vfs_FileExist((const HCHAR *)aucNewFullPath) == TRUE)
	{
		HxLOG_Error("The file with the new name already exists.[%s]\n", aucNewFullPath);
		return VFS_ERROR;
	}

	/* To access a opened file */
	if(local_na_vfs_IsFileHandleExistByNameInList(pxOldName) == TRUE)
	{
		HxLOG_Error("The file is currently accessed. [%s]\n", pxOldName);
		return VFS_ERROR_ACCESS;
	}

	/* the  mandatory  treatment  of  automatic  directory  creation/deletion  on  a  VFS implementation supporting the notion of directories.  */
	if(local_na_vfs_CheckDirInOldAndNewFile((const HCHAR*)aucOldFullPath, (const HCHAR*)aucNewFullPath, &bDiffDir) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_CheckDirInOldAndNewFile]\n");
		return VFS_ERROR;
	}

	/*The  object  cannot  be  rename  because  it  is  used  by  the system.*/
	if(local_na_vfs_FileMove((const HCHAR *)aucOldFullPath, (const HCHAR *)aucNewFullPath) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileMove]\n");
		return VFS_ERROR_ACCESS;
	}

	if(bDiffDir == TRUE)
	{
		/* the  mandatory  treatment  of  automatic  directory  creation/deletion  on  a  VFS implementation supporting the notion of directories.  */
		if(local_na_vfs_RemoveEmptyDirctories((const HCHAR *)aucOldFullPath) != ERR_OK)
		{
			HxLOG_Error("Error in [local_na_vfs_RemoveEmptyDirctories][%s]\n", aucOldFullPath);
			return VFS_ERROR;
		}
	}
	HxLOG_Print("[vfsRenameFile] is completed\n");

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function gathers miscellaneous information about an opened file.
 *
 *  @param[in]  xFileHandle
 *    Handle of the file to get information from. The file may have
 *    been opened for reading or writing.
 *  @param[out] pxFileStatistics
 *    Buffer provided by the application where to store the retrieved
 *    information.
 *
 *  @retval VFS_NO_ERROR
 *            Success to get file info.
 *  @retval VFS_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsStatFile(TVfsFileHandle xFileHandle, TVfsFileStatistics* pxFileStatistics)
{

	CaVfsFileHandle_t	*phFile = NULL;
	HUINT64				ullRetSize;
	HOffset_t			nRetPos;

	HxLOG_Print("vfsStatFile is called [0x%x]\n", xFileHandle);

	if(xFileHandle == VFS_FILE_HANDLE_INVALID)
	{
		HxLOG_Error("xFileHandle is invalid\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	if(pxFileStatistics == NULL)
	{
		HxLOG_Error("pxFileStatistics is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	phFile = (CaVfsFileHandle_t *)xFileHandle;
	if(phFile == NULL)
	{
		HxLOG_Error("hFile is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	/* To check if the handle is valid. */
	if(local_na_vfs_IsFileHandleExistInList(phFile) != TRUE)
	{
		HxLOG_Error("The handle has never been opend. [0x%x]\n", phFile);
		return VFS_ERROR_BAD_HANDLE;
	}

	if(local_na_vfs_FileTell(phFile->hHLibFile, &nRetPos) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileTell]\n");
		return VFS_ERROR;
	}

	if(local_na_vfs_FileSize(phFile->hHLibFile, &ullRetSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileSize]\n");
		return VFS_ERROR;
	}

	HxLOG_Print("vfsStatFile length[%u], pos[%u]\n", (HUINT32)ullRetSize, (HUINT32)nRetPos);

	pxFileStatistics->position = (TSize)nRetPos;
	pxFileStatistics->length = (TSize)ullRetSize;

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function opens the directory specified by its path and returns a
 *    handle useable to list the content of this directory.
 *
 *  @param[out] pxDirectoryHandle
 *    Returned directory handle to pass to subsequent calls to
 *    vfsReadDirectory and vfsCloseDirectory.
 *  @param[in]  pxDirectoryName
 *    Path pointing on the directory to open.
 *
 *  @retval VFS_NO_ERROR
 *            Success to open the directory.
 *  @retval VFS_ERROR_LOW_MEMORY
 *            Not enough memory to open the directory.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR_NO_SUCH_FILE
 *            The directory to open is invalid or inexistent.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsOpenDirectory(TVfsDirectoryHandle* pxDirectoryHandle, const TChar* pxDirectoryName)
{
	HxDIR_t 				*phDir;
	HCHAR					aucFullPath[512] = {0,};
	HUINT32					ulEntryCount = 0;
	CaVfsDirectoryHandle_t	*pstDirHandle;

	HxLOG_Print("vfsOpenDirectory is called [%s]\n", pxDirectoryName);

	if(pxDirectoryHandle == NULL)
	{
		HxLOG_Error("pxDirectoryHandle is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	*pxDirectoryHandle = VFS_DIRECTORY_HANDLE_INVALID;

	if(pxDirectoryName == NULL)
	{
		HxLOG_Error("pxDirectoryName is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

#if !defined(CA_VFS_EMPTY_STRING_AS_ROOT)
	if(HxSTD_StrLen(pxDirectoryName) == 0
#if defined(CA_VFS_DALTS_BUG_FIX)
		&& ((pxDirectoryHandle + 1) != pxDirectoryName) /* pxDirectoryName입력이 없을 경우에 대한 처리 추가 */
#endif
		)
	{
		HxLOG_Error("pxDirectoryName is empty\n");
		return VFS_ERROR_BAD_PARAMETER;
	}
#endif

	if(local_na_vfs_MakeFilePathName(pxDirectoryName, aucFullPath, sizeof(aucFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	if(local_na_vfs_IsDirExist((const HCHAR *)aucFullPath) != TRUE)
	{
		HxLOG_Error("The directory to open is invalid or inexistent.\n");
		return VFS_ERROR_NO_SUCH_FILE;
	}

	if(local_na_vfs_DirGetEntryCountInDir((const HCHAR *) aucFullPath, &ulEntryCount) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_DirGetEntryCountInDir] [%s].\n", aucFullPath);
		return VFS_ERROR;
	}

	phDir = local_na_vfs_DirOpen((const HCHAR *)aucFullPath);
	/* The directory to open is invalid or inexistent.*/
	if(phDir == NULL)
	{
		HxLOG_Error("Error in [local_na_vfs_DirOpen] [%s].\n", aucFullPath);
		return VFS_ERROR;
	}

	pstDirHandle = (CaVfsDirectoryHandle_t *)OxCAS_Malloc(sizeof(CaVfsDirectoryHandle_t));
	if(pstDirHandle == NULL)
	{
		HxLOG_Error("Error in [OxCAS_Malloc] NULL\n");
		return VFS_ERROR_LOW_MEMORY;
	}

	pstDirHandle->phDir = phDir;
	pstDirHandle->bEndOfRead = FALSE;
	pstDirHandle->ulEntryCnt = ulEntryCount;

	pstDirHandle->pxDirectoryEntry = (TVfsDirectoryEntry *)OxCAS_Malloc(sizeof(TVfsDirectoryEntry));
	if(pstDirHandle->pxDirectoryEntry == NULL)
	{
		HxLOG_Error("Error in [OxCAS_Malloc] NULL\n");
		OxCAS_Free(pstDirHandle);
		return VFS_ERROR_LOW_MEMORY;
	}

	pstDirHandle->pxDirectoryEntry->pEntryName = (HCHAR *)OxCAS_Malloc(CA_VFS_MAX_DIR_NAME_LEN * 2);
	if(pstDirHandle->pxDirectoryEntry->pEntryName == NULL)
	{
		HxLOG_Error("Error in [OxCAS_Malloc] NULL\n");
		OxCAS_Free(pstDirHandle->pxDirectoryEntry);
		OxCAS_Free(pstDirHandle);
		return VFS_ERROR_LOW_MEMORY;
	}

	HxLOG_Print("vfsOpenDirectory  Opened handle[0x%x], entry count[%d]\n", pstDirHandle, pstDirHandle->ulEntryCnt);

	*pxDirectoryHandle = (TVfsDirectoryHandle)pstDirHandle;

	/* Register the handle to directory handle list. */
	if(local_na_vfs_AppendDirectoryHandleInList(pstDirHandle) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_AppendDirectoryHandleInList]\n");
	}

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function reads one entry of an opened directory.
 *
 *  @param[in]  xDirectoryHandle
 *    Directory handle returned by the \c vfsOpenDirectory function.
 *  @param[out] ppxDirectoryEntry
 *    Structure allocated by the VFS driver and returned to the
 *    caller. This structure contains information about either an
 *    existing file or an existing subdirectory of the opened
 *    directory. It shall be set to \c NULL when the directory has been
 *    completely read. The allocated structure shall remain valid
 *    until \c vfsReadDirectory or \c vfsCloseDirectory is called.
 *
 *  @retval VFS_NO_ERROR
 *            Success to read an entry.
 *  @retval VFS_ERROR_LOW_MEMORY
 *            Not enough memory to allocated the entry structure.
 *  @retval VFS_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsReadDirectory(TVfsDirectoryHandle xDirectoryHandle, TVfsDirectoryEntry** ppxDirectoryEntry)
{
	CaVfsDirectoryHandle_t *pstDirHandle;
	HCHAR					aucFullPath[512] = {0,};
	HBOOL					bDirectory = FALSE, bExist = TRUE;

	HxLOG_Print("vfsReadDirectory is called [0x%x]\n", xDirectoryHandle);

	if(xDirectoryHandle == VFS_DIRECTORY_HANDLE_INVALID)
	{
		HxLOG_Error("xDirectoryHandle is invalid\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	pstDirHandle = (CaVfsDirectoryHandle_t *)xDirectoryHandle;

	if(pstDirHandle == NULL)
	{
		HxLOG_Error("pstDirHandle is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	if(ppxDirectoryEntry == NULL)
	{
		HxLOG_Error("ppxDirectoryEntry is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	/* To access a invalid handle */
	if(local_na_vfs_IsDirectoryHandleExistInList(pstDirHandle) != TRUE)
	{
		HxLOG_Error("The handle has never been opend. [0x%x]\n", pstDirHandle);
		return VFS_ERROR_BAD_HANDLE;
	}

	HxLOG_Print("Read Directory Path : [%s]\n", local_na_vfs_DirGetPath(pstDirHandle->phDir));

	/* A directory disappered after having been opened. */
	if(local_na_vfs_IsDirExist(local_na_vfs_DirGetPath(pstDirHandle->phDir)) != TRUE)
	{
		HxLOG_Error("The directoy is disappear. [0x%x]\n", pstDirHandle);
		*ppxDirectoryEntry = NULL;
		return VFS_ERROR;
	}

	bDirectory = local_na_vfs_IsDir(pstDirHandle->phDir);

	HxSTD_snprintf(aucFullPath, sizeof(aucFullPath), "%s/%s", local_na_vfs_DirGetPath(pstDirHandle->phDir), local_na_vfs_DirGetName(pstDirHandle->phDir));
	if((bDirectory == TRUE) && (local_na_vfs_IsDirExist(aucFullPath) != TRUE))
	{
		HxLOG_Error("The directoy is gone. [0x%s]\n", aucFullPath);
		bExist = FALSE;
	}
	else if((bDirectory == FALSE) && (local_na_vfs_FileExist(aucFullPath) != TRUE))
	{
		HxLOG_Error("The File is gone. [0x%s]\n", aucFullPath);
		bExist = FALSE;
	}

	if(bExist == FALSE)
	{
		while(1)
		{
			/* Directory가  Open된 이후에 파일이나 directory가 삭제된 경우 다음 entry를 찾는다. */
			if(HLIB_DIR_Next(pstDirHandle->phDir) != ERR_OK)
			{
				HxLOG_Print("Not Found Next directory entry. [%d]\n", pstDirHandle->ulEntryCnt);

				if(pstDirHandle->ulEntryCnt == 0)
				{
					HxLOG_Print("No more directory entry.\n");
					pstDirHandle->bEndOfRead = TRUE;
					break;
				}
				else
				{
					pstDirHandle->ulEntryCnt --;
					continue;
				}
			}

			bDirectory = local_na_vfs_IsDir(pstDirHandle->phDir);
			bExist = TRUE;

			HxSTD_snprintf(aucFullPath, sizeof(aucFullPath), "%s/%s", local_na_vfs_DirGetPath(pstDirHandle->phDir), local_na_vfs_DirGetName(pstDirHandle->phDir));

			HxLOG_Error("Next directoy Entry. [0x%s]\n", aucFullPath);

			if((bDirectory == TRUE) && (local_na_vfs_IsDirExist(aucFullPath) != TRUE))
			{
				HxLOG_Error("The directoy is gone. [0x%s]\n", aucFullPath);
				bExist = FALSE;
			}
			else if((bDirectory == FALSE) && (local_na_vfs_FileExist(aucFullPath) != TRUE))
			{
				HxLOG_Error("The File is gone. [0x%s]\n", aucFullPath);
				bExist = FALSE;
			}

			if(bExist == TRUE)
			{
				break;
			}
		}
	}

	if(pstDirHandle->bEndOfRead != FALSE)
	{
		*ppxDirectoryEntry = NULL;
		HxLOG_Print("The directory has been completely read.\n");
		return VFS_NO_ERROR;
	}

	pstDirHandle->pxDirectoryEntry->type = (bDirectory == TRUE)? VFS_DIRECTORY_ENTRY_SUBDIRECTORY : VFS_DIRECTORY_ENTRY_FILE;

	HxSTD_snprintf(pstDirHandle->pxDirectoryEntry->pEntryName, CA_VFS_MAX_DIR_NAME_LEN * 2 - 1, "%s/%s",
			&local_na_vfs_DirGetPath(pstDirHandle->phDir)[HxSTD_StrLen(CA_VFS_ROOT_PATH) + 1], local_na_vfs_DirGetName(pstDirHandle->phDir));

	HxLOG_Print("vfsReadDirectory [%d][%s]\n", pstDirHandle->pxDirectoryEntry->type, pstDirHandle->pxDirectoryEntry->pEntryName);

	/* Because a readdir() already is called in HLIB_DIR_Open(), HLIB_DIR_Nest() is called on the end of the function body */
	if(HLIB_DIR_Next(pstDirHandle->phDir) != ERR_OK)
	{
		HxLOG_Print("No more directory entry.\n");
		pstDirHandle->bEndOfRead = TRUE;
	}

	*ppxDirectoryEntry = pstDirHandle->pxDirectoryEntry;

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function close a directory previously opened with
 *    \c vfsOpenDirectory.
 *
 *  @param[in]  xDirectoryHandle
 *    Directory handle returned by the vfsOpenDirectory function.
 *
 *  @retval VFS_NO_ERROR
 *            Success to close the directory.
 *  @retval VFS_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsCloseDirectory(TVfsDirectoryHandle xDirectoryHandle)
{
	HERROR					hError;
	CaVfsDirectoryHandle_t	*pstDirHandle;

	HxLOG_Print("vfsCloseDirectory is called [0x%x]\n", xDirectoryHandle);

	if(xDirectoryHandle == VFS_DIRECTORY_HANDLE_INVALID)
	{
		HxLOG_Error("xDirectoryHandle is invalid\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	pstDirHandle = (CaVfsDirectoryHandle_t *)xDirectoryHandle;

	if(pstDirHandle == NULL)
	{
		HxLOG_Error("pstDirHandle is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	/* To access a invalid handle */
	if(local_na_vfs_IsDirectoryHandleExistInList(pstDirHandle) != TRUE)
	{
		HxLOG_Error("The handle has never been opend. [0x%x]\n", pstDirHandle);
		return VFS_ERROR_BAD_HANDLE;
	}

	hError = local_na_vfs_DirClose(pstDirHandle->phDir);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[local_na_vfs_DirClose] error\n");
	}

	if(pstDirHandle->pxDirectoryEntry)
	{
		if(pstDirHandle->pxDirectoryEntry->pEntryName)
		{
			OxCAS_Free(pstDirHandle->pxDirectoryEntry->pEntryName);
		}

		OxCAS_Free(pstDirHandle->pxDirectoryEntry);
	}

	if(hError != ERR_OK)
	{
		return VFS_ERROR;
	}

	/* Unregister a directory handle from directory handle list */
	if(local_na_vfs_RemoveDirectoryHandleInList(pstDirHandle) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_RemoveDirectoryHandleInList]\n");
	}

	OxCAS_Free(pstDirHandle);

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function writes data into a previously open file.
 *    The data to write are provided in a buffer, \c pxBuffer. The size of the
 *    buffer is specified by \c xSizeOfBuffer.
 *
 *    The data are written at the current seek pointer position of the file.
 *    Data at the current seek position are overwritten. If the file was opened
 *    with the \c VFS_FILE_OPEN_APPEND mode, the seek pointer of the file is set
 *    at the end of the file before each write. The seek pointer of the file is
 *    set after the last byte written in the file.
 *
 *    If xSizeOfBuffer is \c NULL, \c VFS_NO_ERROR is returned. No data is written
 *    If the entire buffer cannot be written due to file system full, the
 *    function returns \c VFS_ERROR_FILESYSTEM_FULL. No data is written.
 *    If the file has not been opened for writing, \c VFS_ERROR_ACCESS is
 *    returned.
 *
 *  @param[in]  xFileHandle
 *    Handle of the file to write into.
 *  @param[in]  pxBuffer
 *    Data buffer to write into the file.
 *  @param[in]  xSizeOfBuffer
 *    Size in bytes of the buffer containing the data to write.
 *
 *  @retval VFS_NO_ERROR
 *            Success to write file.
 *  @retval VFS_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR_ACCESS
 *            The file was not opened for writing.
 *  @retval VFS_ERROR_FILESYSTEM_FULL
 *            The physical file system has not enough space left for the
 *            requested operation.
 *  @retval VFS_ERROR
 *            Other error.
 *
 *  @remark
 *    -# This function can return asynchronously to increase performances.
 *       From VFS API 1.1.0 Nagravision applications will call the
 *       vfsSynchronizeFile function when flushing is required.
 *
 */
TVfsStatus vfsWriteToFile(TVfsFileHandle  xFileHandle, const TUnsignedInt8* pxBuffer, TSize xSizeOfBuffer)
{
	HBOOL 				bAppend = FALSE;
	HUINT32				ulSize;
	HOffset_t			nRetPos;
	CaVfsFileHandle_t	*phFile = NULL;
	CaVfsContext_t		*pContext = local_na_vfs_GetContext();


	HxLOG_Print("vfsWriteToFile is called Handle[0x%x], buffer[0x%x], size[%d]\n", xFileHandle, pxBuffer, xSizeOfBuffer);

	if(xFileHandle == VFS_FILE_HANDLE_INVALID)
	{
		HxLOG_Error("xFileHandle is invalid\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	if(pxBuffer == NULL)
	{
		HxLOG_Error("pxBuffer is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	/* If xSizeOfBuffer is  NULL, VFS_NO_ERROR  is returned. */
	if(xSizeOfBuffer == 0)
	{
		HxLOG_Error("xSizeOfBuffer is 0\n");
		return VFS_NO_ERROR;
	}

	/*The  physical  file  system  has  not  enough  space  left	for  the requested operation*/
	if(pContext->ulUsedSize >= pContext->ulMaxSize)
	{
		HxLOG_Print("Used File System size Max[%u] used[%u]\n", pContext->ulMaxSize, pContext->ulUsedSize);
		return VFS_ERROR_FILESYSTEM_FULL;
	}

	phFile = (CaVfsFileHandle_t *)xFileHandle;
	if(phFile == NULL)
	{
		HxLOG_Error("hFile is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	/* To access a invalid handle */
	if(local_na_vfs_IsFileHandleExistInList(phFile) != TRUE)
	{
		HxLOG_Error("The handle has never been opend. [0x%x]\n", phFile);
		return VFS_ERROR_BAD_HANDLE;
	}

	if(phFile->xOpenFlags & VFS_FILE_OPEN_READ_ONLY) /*  If  the  file  has  not  been  opened  for  writing,  VFS_ERROR_ACCESS  is returned. */
	{
		HxLOG_Error("The file has not been opened for writing.\n");
		return VFS_ERROR_ACCESS;
	}

	bAppend =  (phFile->xOpenFlags & VFS_FILE_OPEN_APPEND)? TRUE:FALSE;

	if(bAppend)
	{
		/*	If the file was opened with the VFS_FILE_OPEN_APPEND mode, the
			seek pointer of the file is set at the end of the file before each write. */
		if(local_na_vfs_FileSeek(phFile->hHLibFile, 0, eHxFILE_SEEK_END, &nRetPos) != ERR_OK)
		{
			HxLOG_Error("Error in [local_na_vfs_FileSeek]\n");
			return VFS_ERROR;
		}
	}

	ulSize = local_na_vfs_FileWirte(phFile->hHLibFile, (void *)pxBuffer, 1, xSizeOfBuffer);
	pContext->ulUsedSize += ulSize;

	HxLOG_Print("Writed size[%u], Total used Size[%u]\n", ulSize, pContext->ulUsedSize);

	if(ulSize != xSizeOfBuffer)
	{
		HxLOG_Error("The size is different. Req[%u], Result[%u]\n", xSizeOfBuffer, ulSize);
		return VFS_ERROR;
	}

	return VFS_NO_ERROR;
}



/**
 *  @brief
 *    This function flushes pending writings on the specified file handle. Its
 *    implementation shall be synchronous. It flushes also pending metadata
 *    treatments like renaming.
 *
 *  @param[in]  xFileHandle
 *    Handle of the file to flush.
 *
 *  @retval VFS_NO_ERROR
 *            Success to write file.
 *  @retval VFS_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsSynchronizeFile(TVfsFileHandle xFileHandle)
{
	CaVfsFileHandle_t	*phFile = NULL;
	HINT32				nError = 0;

	HxLOG_Print("vfsSynchronizeFile is called Handle[0x%x]\n", xFileHandle);

	if(xFileHandle == VFS_FILE_HANDLE_INVALID)
	{
		HxLOG_Error("xFileHandle is invalid\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	phFile = (CaVfsFileHandle_t *)xFileHandle;
	if(phFile == NULL)
	{
		HxLOG_Error("hFile is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	if(local_na_vfs_IsFileHandleExistInList(phFile) != TRUE)
	{
		HxLOG_Error("The handle has never been opend. [0x%x]\n", phFile);
		return VFS_ERROR_BAD_HANDLE;
	}

	nError = local_na_vfs_FileFlush(phFile->hHLibFile);
	if( nError != 0)
	{
		HxLOG_Error("Error in [local_na_vfs_FileFlush]. [%d]\n", nError);
		return VFS_ERROR;
	}

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function reads data from a previously open file.
 *
 *    The read data are stored in the provided buffer, \c pxBuffer. The size of
 *    this buffer is specified by \c xSizeOfBuffer and the application expects
 *    that the buffer will be filled with data from the file. The number of
 *    bytes actually read from the file is returned by the \c pxSizeRead
 *    pointed value.
 *
 *    If the function returns any other value than \c VFS_NO_ERROR, the data in
 *    \c pxBuffer and \c pxSizeRead are invalid.Data are read from the current
 *    seek pointer position in the file. The seek pointer of the file is set
 *    after the last byte read in the file.
 *
 *    If the buffer cannot be filled due to end of file, the function returns
 *    \c VFS_NO_ERROR and \c pxSizeRead is set to 0.
 *
 *    If \c xSizeOfBuffer is \c NULL, \c VFS_NO_ERROR is returned. No data is
 *    read. The file must have been open for reading. If not,
 *    \c VFS_ERROR_ACCESS is returned.
 *
 *  @param[in]  xFileHandle
 *    Handle of the file to read from.
 *  @param[out]   pxBuffer
 *    Buffer where to store read bytes. The data in the buffer are not
 *    valid if the function returns any other value than \c VFS_NO_ERROR.
 *  @param[in]  xSizeOfBuffer
 *    Size in bytes of the buffer where to store read data.
 *  @param[out] pxSizeRead
 *    Returned count in bytes of the data read from the file. This
 *    value cannot be bigger than the size of the buffer. This value
 *    is not valid if the function returns any other value than
 *    \c VFS_NO_ERROR.
 *
 *  @retval VFS_NO_ERROR
 *            Success to read file.
 *  @retval VFS_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR_ACCESS
 *            The file was not open for reading.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsReadFromFile(TVfsFileHandle  xFileHandle,TUnsignedInt8* pxBuffer, TSize xSizeOfBuffer,TSize* pxSizeRead)
{
	HUINT64				ullRetSize;
	HOffset_t			nOldPos = 0, nNewPos = 0;
	HUINT32				ulSize;
	CaVfsFileHandle_t	*phFile = NULL;

	HxLOG_Print("vfsReadFromFile is called Handle[0x%x], buffer[0x%x], size[%d]\n", xFileHandle, pxBuffer, xSizeOfBuffer);

	*pxSizeRead = 0;

	if(xFileHandle == VFS_FILE_HANDLE_INVALID)
	{
		HxLOG_Error("xFileHandle is invalid\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	if(pxBuffer == NULL)
	{
		HxLOG_Error("pxBuffer is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	/* If xSizeOfBuffer is  NULL, VFS_NO_ERROR is returned. */
	if(xSizeOfBuffer == 0)
	{
		HxLOG_Error("xSizeOfBuffer is 0\n");
		return VFS_NO_ERROR;
	}

	phFile = (CaVfsFileHandle_t *)xFileHandle;
	if(phFile == NULL)
	{
		HxLOG_Error("hFile is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	/* To access a invalid handle */
	if(local_na_vfs_IsFileHandleExistInList(phFile) != TRUE)
	{
		HxLOG_Error("The handle has never been opend. [0x%x]\n", phFile);
		return VFS_ERROR_BAD_HANDLE;
	}

	if(local_na_vfs_FileSize(phFile->hHLibFile, &ullRetSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileSize]\n");
		return VFS_ERROR;
	}

	if(local_na_vfs_FileTell(phFile->hHLibFile, &nOldPos) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileTell]\n");
		return VFS_ERROR;
	}

	HxLOG_Print("The requested File size[%u] Old pos[%d]\n", (HUINT32)ullRetSize, nOldPos);

	if(xSizeOfBuffer > ullRetSize)
	{
		HxLOG_Warning("xSizeOfBuffer is to Big. Req. Size[%u], File Size[%u]\n", xSizeOfBuffer, ullRetSize);
	}

	/*	The file must have been open for reading. If not, VFS_ERROR_ACCESS is returned. */
	if(phFile->xOpenFlags & VFS_FILE_OPEN_WRITE_ONLY)
	{
		HxLOG_Error("The file has not been opened for reading.\n");
		return VFS_ERROR_ACCESS;
	}

	do
	{
		ulSize = local_na_vfs_FileRead(phFile->hHLibFile, pxBuffer, 1, (HUINT32)xSizeOfBuffer);

		if(local_na_vfs_FileTell(phFile->hHLibFile, &nNewPos) != ERR_OK)
		{
			HxLOG_Error("Error in [local_na_vfs_FileTell]\n");
			return VFS_ERROR;
		}

		HxLOG_Print("The Read size[%u] New pos[%d]\n", (HUINT32)(nNewPos - nOldPos), nNewPos);

		if(((HUINT32)(nNewPos - nOldPos) == (HUINT32)xSizeOfBuffer) || (ullRetSize == nNewPos))
		{
			break;
		}

		/*
			work around 적용.
			DAL (VFS) Test에서  Test case 26 Life Time test진행 중 Read operation에서 제대로 된 data를 읽어 오지 못하는 문제가 발생하여
			File close를 하고 다시 open을 하여 처리하도록 함. 이렇게 하니 제대로 읽어 옴.
		*/
		VK_TASK_Sleep(10);

		{
			HCHAR				aucFullPath[512] = {0,};

			HxLOG_Warning("[Warning] Read operation Error, Work around start.\n");

			local_na_vfs_FileClose(phFile->hHLibFile);

			if(local_na_vfs_MakeFilePathName(phFile->pszFileName, aucFullPath, sizeof(aucFullPath)) != ERR_OK)
			{
				HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
				return VFS_ERROR;
			}

			phFile->hHLibFile = local_na_vfs_FileOpen((const HCHAR *)aucFullPath, (const HCHAR *)"r+b");
			if(phFile->hHLibFile == NULL)
			{
				HxLOG_Error("Error in [local_na_vfs_FileOpen] [%s][%s]\n", aucFullPath, "r+b");
				return VFS_ERROR;
			}

			if(local_na_vfs_FileSeek(phFile->hHLibFile, nOldPos, eHxFILE_SEEK_SET, NULL) != ERR_OK)
			{
				HxLOG_Error("Error in [local_na_vfs_FileSeek]\n");
				return VFS_ERROR;
			}
		}
	}while ((HUINT32)(nNewPos - nOldPos) != (HUINT32)xSizeOfBuffer);

	*pxSizeRead = (HUINT32)(nNewPos - nOldPos);

	if((HUINT32)(nNewPos - nOldPos) != (HUINT32)xSizeOfBuffer)
	{
		HxLOG_Warning("The size is different. Req[%u], Result[%u]\n", xSizeOfBuffer, (HUINT32)(nNewPos - nOldPos));
	}

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    This function changes the seek pointer of the file. The next read or
 *    write operation will occur at the requested position.  If the file was
 *    opened in writing mode \c VFS_FILE_OPEN_APPEND, the next write will still
 *    be done at the end of the file.
 *
 *    Giving an absolute offset of 0 will cause the next read or write
 *    operation to act on the very beginning of the file.
 *
 *    Given an absolute offset out of the file limits, the \c vfsSeekInFile()
 *    function returns \c VFS_ERROR_END_OF_FILE. The seek pointer is left
 *    untouched.
 *
 *  @param[in]  xFileHandle
 *    Handle to the file of which the seek pointer shall
 *    be changed to.
 *  @param[in]  xOffset
 *    New absolute or relative offset of the file.
 *  @param[in]  xSeekMode
 *    Defines is the offset is absolute or relative to the
 *    current position.
 *
 *  @retval VFS_NO_ERROR
 *            Success to seek file pointer.
 *  @retval VFS_ERROR_BAD_HANDLE
 *            The handle passed to the function is not valid.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR_END_OF_FILE
 *            The specified offset sets the file position out of the
 *            file limits.
 *  @retval VFS_ERROR
 *            Other error.
 *
 */
TVfsStatus vfsSeekInFile(TVfsFileHandle xFileHandle, TSignedInt32 xOffset, TVfsSeekMode xSeekMode)
{
	CaVfsFileHandle_t	*phFile = NULL;
	HUINT64				ullFileSize = 0;
	HOffset_t			nOffset = 0, nCurPos = 0, nRetPos = 0;
	HxFILE_Seek_e		eOrigin;

	HxLOG_Print("vfsSeekInFile is called Handle[0x%x] offset[%d] mode[%d]\n", xFileHandle, xOffset, xSeekMode);

	if(xFileHandle == VFS_FILE_HANDLE_INVALID)
	{
		HxLOG_Error("xFileHandle is invalid\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	phFile = (CaVfsFileHandle_t *)xFileHandle;
	if(phFile == NULL)
	{
		HxLOG_Error("hFile is null\n");
		return VFS_ERROR_BAD_HANDLE;
	}

	/* To access a invalid handle */
	if(local_na_vfs_IsFileHandleExistInList(phFile) != TRUE)
	{
		HxLOG_Error("The handle has never been opend. [0x%x]\n", phFile);
		return VFS_ERROR_BAD_HANDLE;
	}

	if(local_na_vfs_FileSize(phFile->hHLibFile, &ullFileSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileSize]\n");
		return VFS_ERROR;
	}

	if(local_na_vfs_FileTell(phFile->hHLibFile, &nCurPos) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileTell]\n");
		return VFS_ERROR;
	}

	HxLOG_Print("The requested File size[%u] pos[%d]\n", (HUINT32)ullFileSize, nCurPos);

	nOffset = xOffset;

	if(xSeekMode == VFS_SEEK_ABSOLUTE)
	{
		if((nOffset < 0) || ((nOffset > 0) && (ullFileSize < nOffset)))
		{
			HxLOG_Error("Offset is invalid. file size[%u], offset[%ld]\n", (HUINT32)ullFileSize, nOffset);
			return VFS_ERROR_END_OF_FILE;
		}
		eOrigin = eHxFILE_SEEK_SET;
	}
	else if(xSeekMode == VFS_SEEK_RELATIVE)
	{
		if((nOffset + nCurPos < 0) || ((nOffset > 0) && (ullFileSize < nOffset + nCurPos)))
		{
			HxLOG_Error("Offset is invalid. file size[%u], offset[%ld], cur_pos[%ld]\n", (HUINT32)ullFileSize, nOffset, nCurPos);
			return VFS_ERROR_END_OF_FILE;
		}
		eOrigin = eHxFILE_SEEK_CUR;
	}
	else
	{
		return VFS_ERROR_BAD_PARAMETER;
	}

	if(local_na_vfs_FileSeek(phFile->hHLibFile, nOffset, eOrigin, &nRetPos) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_FileSeek]\n");
		return VFS_ERROR;
	}

	HxLOG_Print("Req [%d], Final pos[%ld]\n", nOffset, nRetPos);

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    Return the total size in bytes of a disk partition used by the VFS.
 *
 *  @param[in]  pxDirectoryName
 *    Pointer to a null-terminated string that specifies a directory
 *    on the disk partition of interest.
 *  @param[out] pxTotalSize
 *    Total number of bytes on the given disk partition.
 *
 *  @retval VFS_NO_ERROR
 *            Success.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR
 *            Failure.
 *
 */
TVfsStatus vfsGetTotalSize(const TChar* pxDirectoryName, TUnsignedInt64* pxTotalSize)
{
	CaVfsContext_t		*pContext = local_na_vfs_GetContext();
	HCHAR				aucFullPath[512] = {0,};
	HUINT32				ulSize = 0;
#if defined(CA_VFS_DALTS_BUG_FIX)
	static HUINT8		ucNum = 0;
#endif

	HxLOG_Print("vfsGetTotalSize is called [%s]\n", pxDirectoryName);

#if defined(CA_VFS_DALTS_BUG_FIX)
	ucNum ++;
#endif

	if(pxDirectoryName == NULL)
	{
		HxLOG_Error("pxDirectoryName is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	if(pxTotalSize == NULL)
	{
		HxLOG_Error("pxTotalSize is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

#if !defined(CA_VFS_EMPTY_STRING_AS_ROOT)
	if(HxSTD_StrLen(pxDirectoryName) == 0
#if defined(CA_VFS_DALTS_BUG_FIX)
		&& ucNum != 5
#endif
		)
	{
		HxLOG_Error("pxDirectoryName is empty\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

#if defined(CA_VFS_DALTS_BUG_FIX)
	if(ucNum >= 6)
	{
		ucNum = 0;
	}
#endif
#endif

	if(local_na_vfs_MakeFilePathName(pxDirectoryName, aucFullPath, sizeof(aucFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	if(local_na_vfs_IsDirExist((const HCHAR *)aucFullPath) != TRUE)
	{
		HxLOG_Error("The directory to open is invalid or inexistent.\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	/* 항상 total size를 계산해야 한다. 그래서 VFS용 root를 이용한다. */
	if(local_na_vfs_DirGetTotalSize(/*aucFullPath*/CA_VFS_ROOT_PATH, &ulSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_DirGetTotalSize].\n");
		return VFS_ERROR;
	}

	HxLOG_Print("Total Size [%u][0x%x]\n", ulSize, ulSize);
	pContext->ulUsedSize = ulSize;

#ifdef UINT64_TYPE
	*pxTotalSize = (HUINT64)ulSize;
#else
	pxTotalSize->leastSignificantBits = ulSize;
	pxTotalSize->mostSignificantBits = 0;
#endif

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    Return the free size in bytes of a disk partition used by the VFS.
 *
 *  @param[in]  pxDirectoryName
 *    Pointer to a null-terminated string that specifies a directory
 *    on the disk partition of interest.
 *  @param[out] pxFreeSize
 *    Total number of free bytes on the disk partition.
 *
 *  @retval VFS_NO_ERROR
 *            Success.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR
 *            Failure.
 *
 */
TVfsStatus vfsGetFreeSize(const TChar* pxDirectoryName, TUnsignedInt64* pxFreeSize)
{
	CaVfsContext_t		*pContext = local_na_vfs_GetContext();
	HUINT32				ulSize = 0;
	HCHAR				aucFullPath[512] = {0,};
#if defined(CA_VFS_DALTS_BUG_FIX)
	static HUINT8		ucNum = 0;
#endif

	HxLOG_Print("vfsGetFreeSize is called [%s]\n", pxDirectoryName);

#if defined(CA_VFS_DALTS_BUG_FIX)
	ucNum ++;
#endif

	if(pxDirectoryName == NULL)
	{
		HxLOG_Error("pxDirectoryName is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	if(pxFreeSize == NULL)
	{
		HxLOG_Error("pxFreeSize is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

#if !defined(CA_VFS_EMPTY_STRING_AS_ROOT)
	if(HxSTD_StrLen(pxDirectoryName) == 0
#if defined(CA_VFS_DALTS_BUG_FIX)
		&& ucNum != 5
#endif
		)
	{
		HxLOG_Error("pxDirectoryName is empty\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

#if defined(CA_VFS_DALTS_BUG_FIX)
	if(ucNum >= 6)
	{
		ucNum = 0;
	}
#endif
#endif

	if(local_na_vfs_MakeFilePathName(pxDirectoryName, aucFullPath, sizeof(aucFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	if(local_na_vfs_IsDirExist((const HCHAR *)aucFullPath) != TRUE)
	{
		HxLOG_Error("The directory to open is invalid or inexistent.\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	/* 항상 total size를 계산해야 한다. 그래서 VFS용 root를 이용한다. */
	if(local_na_vfs_DirGetTotalSize(/*aucFullPath*/CA_VFS_ROOT_PATH, &ulSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_DirGetTotalSize].\n");
		return VFS_ERROR;
	}

	HxLOG_Print("Free Size [%u][0x%x]\n", CA_VFS_MAX_PARTITION_SIZE - ulSize, CA_VFS_MAX_PARTITION_SIZE - ulSize);
	pContext->ulUsedSize = ulSize;

#ifdef UINT64_TYPE
	*pxFreeSize = (HUINT64)(CA_VFS_MAX_PARTITION_SIZE - ulSize);
#else
	pxFreeSize->leastSignificantBits = CA_VFS_MAX_PARTITION_SIZE - ulSize;
	pxFreeSize->mostSignificantBits = 0;
#endif

	return VFS_NO_ERROR;
}


/**
 *  @brief
 *    Return the cluster size in bytes of a disk partition used by the VFS.
 *    A cluster is the smallest amount of space that can be allocated to hold
 *    a file.
 *
 *  @param[in]  pxDirectoryName
 *    Pointer to a null-terminated string that specifies a directory
 *    on the disk partition of interest.
 *  @param[out] pxClusterSize
 *    Cluster size of the disk partition. This value must never be
 *    NULL or equal to 0. If the 'cluster' notion doesn't apply the
 *    cluster size must be set to 1.
 *
 *  @retval VFS_NO_ERROR
 *            Success.
 *  @retval VFS_ERROR_BAD_PARAMETER
 *            Some parameters have an unsupported value.
 *  @retval VFS_ERROR
 *            Failure.
 *
 */
TVfsStatus vfsGetClusterSize(const TChar* pxDirectoryName, TUnsignedInt64* pxClusterSize)
{
	//HxDIR_t				*phDir;
	HCHAR				aucFullPath[512] = {0,};
#if defined(CA_VFS_DALTS_BUG_FIX)
	static HUINT8		ucNum = 0;
#endif

	HxLOG_Print("vfsGetClusterSize is called [%s]\n", pxDirectoryName);

#if defined(CA_VFS_DALTS_BUG_FIX)
	ucNum ++;
#endif

	if(pxDirectoryName == NULL)
	{
		HxLOG_Error("pxDirectoryName is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

	if(pxClusterSize == NULL)
	{
		HxLOG_Error("pxClusterSize is null\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

#if !defined(CA_VFS_EMPTY_STRING_AS_ROOT)
	if(HxSTD_StrLen(pxDirectoryName) == 0
#if defined(CA_VFS_DALTS_BUG_FIX)
		&& ucNum != 5
#endif
		)

	{
		HxLOG_Error("pxDirectoryName is empty\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

#if defined(CA_VFS_DALTS_BUG_FIX)
	if(ucNum >= 6)
	{
		ucNum = 0;
	}
#endif
#endif

	if(local_na_vfs_MakeFilePathName(pxDirectoryName, aucFullPath, sizeof(aucFullPath)) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_MakeFilePathName].\n");
		return VFS_ERROR;
	}

	if(local_na_vfs_IsDirExist((const HCHAR *)aucFullPath) != TRUE)
	{
		HxLOG_Error("The directory to open is invalid or inexistent.\n");
		return VFS_ERROR_BAD_PARAMETER;
	}

#if 0
	phDir = local_na_vfs_DirOpen((const HCHAR *)aucFullPath);
	if(phDir == NULL)/* The directory to open is invalid or inexistent.*/
	{
		HxLOG_Error("Error in [local_na_vfs_DirOpen] [%s].\n", aucFullPath);
		return VFS_ERROR;
	}
#endif

	/* If  the  'cluster'  notion  doesn't  apply  the cluster size must be set to 1. */
#ifdef UINT64_TYPE
	*pxClusterSize = 1;
#else
	pxClusterSize->leastSignificantBits = 1;
	pxClusterSize->mostSignificantBits = 0;
#endif

#if 0
	if(local_na_vfs_DirClose(phDir) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_DirClose].\n");
		return VFS_ERROR;
	}
#endif

	return VFS_NO_ERROR;
}


/**************************************************************************************************/
#define ________CA_SEC_Private_Functions_Body_______________________________________________
/**************************************************************************************************/
STATIC CaVfsContext_t* local_na_vfs_GetContext(void)
{
	return &s_stCaVfsContext;
}

STATIC HERROR local_na_vfs_InitContext(void)
{
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	HxSTD_memset(pContext, 0, sizeof(CaVfsContext_t));

	pContext->ulMaxSize = CA_VFS_MAX_PARTITION_SIZE;

	if(local_na_vfs_DirGetTotalSize(CA_VFS_ROOT_PATH, &pContext->ulUsedSize) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_DirGetTotalSize].\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Used File System size [%u]\n", pContext->ulUsedSize);

	return ERR_OK;
}

STATIC HERROR local_na_vfs_CheckFileNameValid(const HCHAR *pszFileName, HUINT32	ulLen)
{
	HUINT32	i;
	HCHAR	ch;

	for(i = 0; i < ulLen; i ++)
	{
		ch = pszFileName[i];
		if(((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')) || (ch == '.') || (ch == '-') || (ch == '_') || (ch == '/'))
		{
			continue;
		}
		else
		{
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

STATIC HERROR local_na_vfs_MakeFilePathName(const HCHAR *pszFileName, HCHAR *pucFullPath, HUINT32 ulBufLen)
{
	HUINT32	ulLen = 0;

	if(pszFileName == NULL|| pucFullPath == NULL)
	{
		HxLOG_Error("Filename or pucFullPath buffer is NULL.\n");
		return ERR_FAIL;
	}

	ulLen = HxSTD_StrLen(pszFileName);

#if 0
	if(ulLen == 0)
	{
		HxLOG_Error("Filename length is zero.\n");
		return ERR_FAIL;
	}
#endif

	if(ulLen > CA_VFS_MAX_FILE_NAME_LEN)
	{
		HxLOG_Warning("[local_na_vfs_MakeFilePathName] pszFileName is too long.\n");
	}

	if(local_na_vfs_CheckFileNameValid(pszFileName, ulLen) != ERR_OK)
	{
		HxLOG_Error("Error in [local_na_vfs_CheckFileNameValid] [%s]\n", pszFileName);
		return ERR_FAIL;
	}

	if((ulLen + HxSTD_StrLen(CA_VFS_ROOT_PATH)) > ulBufLen - 1)
	{
		HxLOG_Error("Buffer size is too small. buff[%u], req [%u]\n", ulBufLen, (ulLen + HxSTD_StrLen(CA_VFS_ROOT_PATH)));
		return ERR_FAIL;
	}

	HxSTD_snprintf(pucFullPath, ulBufLen, "%s/%s", CA_VFS_ROOT_PATH, (pszFileName[0] == '/')? &pszFileName[1]:pszFileName);

	HxLOG_Print("Full Path Name [%s]\n", pucFullPath);

	return ERR_OK;
}

STATIC HERROR local_na_vfs_InitRootPath(void)
{
	HCHAR	aucFullPath[512] = {0,};

	HxSTD_snprintf(aucFullPath, sizeof(aucFullPath) - 1, "%s", CA_VFS_ROOT_PATH);

	if(local_na_vfs_IsDirExist((const HCHAR *)aucFullPath) != TRUE)
	{
		HxLOG_Debug("Need to Create Root Folder for VFS. [%s]\n", aucFullPath);

		if(local_na_vfs_DirMake((const HCHAR *) aucFullPath) != ERR_OK)
		{
			HxLOG_Error("ERROR In [local_na_vfs_DirMake]. [%s]\n", aucFullPath);
			return ERR_FAIL;
		}

#if	defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if(HLIB_EXTRA_Chown((const HCHAR *) aucFullPath, "obama", "root") != ERR_OK)
		{
			HxLOG_Error("ERROR In [HLIB_EXTRA_Chown]. [%s]\n", aucFullPath);
		}
#endif
	}

	return ERR_OK;
}

STATIC HERROR local_na_vfs_AddFileNameInHandle(CaVfsFileHandle_t *phFile, const HCHAR *szFileName)
{
	HUINT32 ulFileNameLen = HxSTD_StrLen(szFileName);

	phFile->pszFileName = (HCHAR *)OxCAS_Malloc(ulFileNameLen + 1);
	if(phFile->pszFileName == NULL)
	{
		HxLOG_Error("ERROR In [OxCAS_Malloc].\n");
		return ERR_FAIL;
	}

	HxSTD_StrNCpy(phFile->pszFileName, szFileName, ulFileNameLen);
	phFile->pszFileName[ulFileNameLen] = 0;

	return ERR_OK;
}

STATIC HERROR local_na_vfs_FreeFileNameInHandle(CaVfsFileHandle_t *phFile)
{
	if(phFile->pszFileName != NULL)
	{
		OxCAS_Free(phFile->pszFileName);
	}

	return ERR_OK;
}

STATIC HERROR local_na_vfs_AppendFileHandleInList(CaVfsFileHandle_t *pstFileHandle)
{
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	HxLOG_Print("[local_na_vfs_AppendFileHandleInList] [0x%x]\n", pstFileHandle);

	pContext->phFileList = HLIB_LIST_Append(pContext->phFileList, (void *)pstFileHandle);

	if(pContext->phFileList == NULL)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR local_na_vfs_RemoveFileHandleInList(CaVfsFileHandle_t *pstFileHandle)
{
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	HxLOG_Print("[local_na_vfs_RemoveFileHandleInList] [0x%x]\n", pstFileHandle);

	pContext->phFileList = HLIB_LIST_Remove(pContext->phFileList, (void *)pstFileHandle);

	return ERR_OK;
}

STATIC HBOOL local_na_vfs_IsFileHandleExistInList(CaVfsFileHandle_t *pstFileHandle)
{
	HxList_t		*list;
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	list = HLIB_LIST_Find(pContext->phFileList, (void *)pstFileHandle);

	if(list == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

STATIC HBOOL	local_na_vfs_CB_FindByFileName(void *pvUserData, void *pvListData)
{
	CaVfsFileHandle_t 	*pstFileHandle;
	HCHAR				*pszFileName;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstFileHandle = (CaVfsFileHandle_t *)pvListData;
	pszFileName = (HCHAR *)pvUserData;

	return (HxSTD_StrCmp(pstFileHandle->pszFileName, pszFileName) == 0) ? TRUE : FALSE;
}

STATIC HBOOL local_na_vfs_IsFileHandleExistByNameInList(const HCHAR *pszFileName)
{
	HxList_t		*list;
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	list = HLIB_LIST_FindEx(pContext->phFileList, (void *)pszFileName, local_na_vfs_CB_FindByFileName);

	if(list == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

STATIC HERROR local_na_vfs_AppendDirectoryHandleInList(CaVfsDirectoryHandle_t *pstDirHandle)
{
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	HxLOG_Print("[local_na_vfs_AppendFileHandleInList] [0x%x]\n", pstDirHandle);

	pContext->phDirList = HLIB_LIST_Append(pContext->phDirList, (void *)pstDirHandle);

	if(pContext->phDirList == NULL)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR local_na_vfs_RemoveDirectoryHandleInList(CaVfsDirectoryHandle_t *pstDirHandle)
{
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	HxLOG_Print("[local_na_vfs_RemoveDirectoryHandleInList] [0x%x]\n", pstDirHandle);

	pContext->phDirList = HLIB_LIST_Remove(pContext->phDirList, (void *)pstDirHandle);

	return ERR_OK;
}

STATIC HBOOL local_na_vfs_IsDirectoryHandleExistInList(CaVfsDirectoryHandle_t *pstDirHandle)
{
	HxList_t		*list;
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	list = HLIB_LIST_Find(pContext->phDirList, (void *)pstDirHandle);

	if(list == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

#if 0
STATIC HBOOL	local_na_vfs_CB_FindByDirName(void *pvUserData, void *pvListData)
{
	CaVfsDirectoryHandle_t 	*pstDirHandle;
	HUINT32					ulStrLen = 0;
	HCHAR					*pszDirName, aucOrgDirName[512] = {0,};

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstDirHandle = (CaVfsDirectoryHandle_t *)pvListData;
	pszDirName = (HCHAR *)pvUserData;

	HxSTD_StrNCpy(aucOrgDirName, local_na_vfs_DirGetPath(pstDirHandle->phDir), sizeof(aucOrgDirName) - 1);

	ulStrLen = HxSTD_StrLen(local_na_vfs_DirGetPath(pstDirHandle->phDir));
	aucOrgDirName[ulStrLen - 1] = 0; /* Path name 의 마지막에 있는 '/' 제거 */

	HxLOG_Print("Target[%s], In List[%s]\n", pszDirName, aucOrgDirName);

	return (HxSTD_StrCmp(aucOrgDirName, pszDirName) == 0) ? TRUE : FALSE;
}

STATIC HBOOL local_na_vfs_IsDirHandleExistByNameInList(const HCHAR *pszDirName)
{
	HxList_t		*list;
	CaVfsContext_t	*pContext = local_na_vfs_GetContext();

	list = HLIB_LIST_FindEx(pContext->phDirList, (void *)pszDirName, local_na_vfs_CB_FindByDirName);

	if(list == NULL)
	{
		return FALSE;
	}

	return TRUE;
}
#endif

STATIC HERROR local_na_vfs_CheckDirInFileName(const HCHAR* pszFileName)
{

	HCHAR	*p, *pszDup = NULL;
	HERROR	hErr = ERR_OK;

	HxLOG_Assert(pszFileName);

	pszDup = HLIB_STD_StrDup(pszFileName);
	p = pszDup;

	HxLOG_Print("Full name : [%s]\n", p);

	if((pszDup = HxSTD_StrRChar(pszDup, '/')) != NULL)
	{
		*pszDup = 0;

		HxLOG_Print("final depth dir name : [%s]\n", p);

		if(local_na_vfs_IsDirExist((const HCHAR *)p) != TRUE)
		{
			hErr = local_na_vfs_DirMake((const HCHAR *)p);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("ERROR In [local_na_vfs_DirMake]. [%s]\n", p);
			}
		}
		else
		{
			HxLOG_Print("The dir is already existed : [%s]\n", p);
		}
	}

	HLIB_STD_MemFree(p);

	return hErr;
}

STATIC HERROR local_na_vfs_CheckDirInOldAndNewFile(const HCHAR* pszOldName, const HCHAR* pszNewName, HBOOL *pbDiffDir)
{

	HCHAR	*pszOld, *pszOldDup = NULL, *pszNew, *pszNewDup = NULL;
	HERROR	hErr = ERR_OK;

	HxLOG_Assert(pszOldName);
	HxLOG_Assert(pszNewName);

	pszOldDup = HLIB_STD_StrDup(pszOldName);
	pszOld = pszOldDup;

	pszNewDup = HLIB_STD_StrDup(pszNewName);
	pszNew = pszNewDup;

	HxLOG_Print("File name : Old[%s], New[%s]\n", pszOld, pszNew);

	*pbDiffDir = FALSE;

	if((pszOldDup = HxSTD_StrRChar(pszOldDup, '/')) == NULL)
	{
		HxLOG_Error("ERROR In HxSTD_StrRChar(pszOldDup). [%s]\n", pszOld);
		HLIB_STD_MemFree(pszOld);
		HLIB_STD_MemFree(pszNew);
		return ERR_FAIL;
	}
	*pszOldDup = 0;

	if((pszNewDup = HxSTD_StrRChar(pszNewDup, '/')) == NULL)
	{
		HxLOG_Error("ERROR In HxSTD_StrRCharpszNewDup). [%s]\n", pszNew);
		HLIB_STD_MemFree(pszOld);
		HLIB_STD_MemFree(pszNew);
		return ERR_FAIL;
	}
	*pszNewDup = 0;

	HxLOG_Print("final depth dir name : Old[%s], New[%s]\n", pszOld, pszNew);

	if(HxSTD_StrCmp(pszOld, pszNew) != 0)
	{
		if(local_na_vfs_IsDirExist((const HCHAR *)pszNew) != TRUE)
		{
			hErr = local_na_vfs_DirMake((const HCHAR *)pszNew);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("ERROR In [local_na_vfs_DirMake]. [%s]\n", pszOld);
			}
		}
		else
		{
			HxLOG_Print("The dir is already existed : [%s]\n", pszOld);
		}
		*pbDiffDir = TRUE;
	}

	HLIB_STD_MemFree(pszOld);
	HLIB_STD_MemFree(pszNew);

	return hErr;
}

#if 0
STATIC HBOOL local_na_vfs_AreThereOpendDirctories(const HCHAR *pszName)
{
	HCHAR	*p, *pszDup = NULL;
	HBOOL	bOpend = FALSE;

	HxLOG_Assert(pszName);

	pszDup = HLIB_STD_StrDup(pszName);
	p = pszDup;

	HxLOG_Print("Full name : [%s]\n", p);

	while((pszDup = HxSTD_StrRChar(pszDup, '/')) != NULL)
	{
		*pszDup = 0;

		HxLOG_Print("Dir name : [%s]\n", p);

		if(HxSTD_StrCmp(CA_VFS_ROOT_PATH, p) == 0)
		{
			HxLOG_Error("This is Root Path. [%s]\n", p);
			break;
		}

		if(local_na_vfs_IsDirHandleExistByNameInList(p) == TRUE)
		{
			HxLOG_Error("The directory is opened.[%s]\n", p);
			bOpend = TRUE;
			break;
		}

		pszDup = p;
	}

	HLIB_STD_MemFree(p);

	return bOpend;
}
#endif

STATIC HERROR local_na_vfs_RemoveEmptyDirctories(const HCHAR *pszName)
{
	HCHAR	*p, *pszDup = NULL;
	HERROR	hErr = ERR_OK;

	HxLOG_Assert(pszName);

	pszDup = HLIB_STD_StrDup(pszName);
	p = pszDup;

	HxLOG_Print("Full name : [%s]\n", p);

	while((pszDup = HxSTD_StrRChar(pszDup, '/')) != NULL)
	{
		*pszDup = 0;

		HxLOG_Print("Dir name : [%s]\n", p);

		if(HxSTD_StrCmp(CA_VFS_ROOT_PATH, p) == 0)
		{
			HxLOG_Error("This is Root Path. [%s]\n", p);
			break;
		}

		/* Directory 내에 File 도 없고 지우려고 하는  */
		if(local_na_vfs_isDirectoryEmpty(p) == TRUE)
		{
			HxLOG_Print("The directory is Empty.[%s]\n", p);

			if(local_na_vfs_DirRemove((const HCHAR *)p) != ERR_OK)
			{
				HxLOG_Error("ERROR In [local_na_vfs_DirRemove]. [%s]\n", p);
			}

		}
		else
		{
			HxLOG_Print("The directory is not Empty.[%s]\n", p);
			break;
		}
		pszDup = p;
	}

	HLIB_STD_MemFree(p);

	return hErr;

}

STATIC HERROR local_na_vfs_DirGetEntryCountInDir(const HCHAR *pszDirName, HUINT32 *pulEntCnt)
{
	HxDIR_t 	*pDir= NULL;
	HUINT32		ulEntCnt = 0;

	pDir = HLIB_DIR_Open(pszDirName);
	if(pDir)
	{
		do{
			HxLOG_Print("[%d]Entry Name [%s]\n", ulEntCnt, HLIB_DIR_GetName(pDir));

			ulEntCnt ++;

		}while(HLIB_DIR_Next(pDir) == ERR_OK);
		HLIB_DIR_Close(pDir);
	}
	else
	{
		return ERR_FAIL;
	}

	*pulEntCnt = ulEntCnt;
	return ERR_OK;

}

STATIC HBOOL local_na_vfs_isDirectoryEmpty(const HCHAR *pszDirName)
{
	return HLIB_DIR_IsDirectoryEmpty(pszDirName);
}

STATIC HBOOL local_na_vfs_FileExist(const HCHAR *pszName)
{
	return HLIB_FILE_Exist(pszName);
}

STATIC HxFILE_t local_na_vfs_FileOpen(const HCHAR *pszFileName, const HCHAR *pszMode)
{
	return HLIB_FILE_Open(pszFileName, pszMode);
}

STATIC HINT32 local_na_vfs_FileClose(HxFILE_t hSelf)
{
	return HLIB_FILE_Close(hSelf);
}

STATIC HUINT32 local_na_vfs_FileRead(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count)
{
	return HLIB_FILE_Read(hSelf, buffer, size, count);
}

STATIC HUINT32 local_na_vfs_FileWirte(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count)
{

	return HLIB_FILE_WriteWithoutFlush(hSelf, buffer, size, count);
}

STATIC HERROR local_na_vfs_FileMake(const HCHAR *pszName)
{
	HERROR hErr;

	hErr = HLIB_FILE_Make(pszName);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	/* obama만 access 가능하도록 file permission 변경 */
	HLIB_EXTRA_Chmod(pszName, 0600);

	{
		HBOOL bRoot = FALSE;
		if(HLIB_EXTRA_AmIRoot(&bRoot) == ERR_OK)
		{
			if(bRoot == TRUE)
			{
				if(HLIB_EXTRA_Chown(pszName, "obama", "obama") != ERR_OK)
				{
					HxLOG_Error("ERROR In [HLIB_EXTRA_Chown]. [%s]\n", pszName);
				}
			}
		}
	}
#endif

	return hErr;
}

STATIC HERROR local_na_vfs_FileMove(const HCHAR *pszOldName, const HCHAR *pszNewName)
{
	return HLIB_FILE_Move(pszOldName, pszNewName);
}

STATIC HERROR local_na_vfs_FileDelete(const HCHAR *pszName)
{
	return HLIB_FILE_Delete(pszName);
}

STATIC HERROR local_na_vfs_FileSeek(HxFILE_t hSelf, HOffset_t nOffset, HxFILE_Seek_e eOrigin, HOffset_t *nRetPos)
{
	return HLIB_FILE_Seek(hSelf, nOffset, eOrigin, nRetPos);
}

STATIC HERROR local_na_vfs_FileTell(HxFILE_t hSelf, HOffset_t *nRetPos)
{
	return HLIB_FILE_Tell(hSelf, nRetPos);
}

STATIC HERROR local_na_vfs_FileSize(HxFILE_t hSelf, HUINT64 *pullRetSize)
{
	return HLIB_FILE_Size(hSelf, pullRetSize);
}

STATIC HINT32 local_na_vfs_FileFlush(HxFILE_t hSelf)
{
	return HLIB_FILE_Flush(hSelf);
}

STATIC HxDIR_t* local_na_vfs_DirOpen(const HCHAR	*pszPath)
{
	return HLIB_DIR_Open(pszPath);
}

STATIC HERROR local_na_vfs_DirClose(HxDIR_t *pSelf)
{
	return HLIB_DIR_Close(pSelf);
}

STATIC HCHAR* local_na_vfs_DirGetName(HxDIR_t *pSelf)
{
	return HLIB_DIR_GetName(pSelf);
}

STATIC HCHAR* local_na_vfs_DirGetPath(HxDIR_t *pSelf)
{
	return HLIB_DIR_GetPath(pSelf);
}

STATIC HBOOL local_na_vfs_IsDir(HxDIR_t *pSelf)
{
	return HLIB_DIR_IsDir(pSelf);
}

#if 0
STATIC HBOOL local_na_vfs_IsFile(HxDIR_t *pSelf)
{
	return HLIB_DIR_IsFile(pSelf);
}
#endif

STATIC HBOOL local_na_vfs_IsDirExist(const HCHAR *pszDirName)
{
	return HLIB_DIR_IsExist(pszDirName);
}

STATIC HERROR local_na_vfs_DirMake(const HCHAR *pszPath)
{
	HERROR hErr;

	hErr = HLIB_DIR_Make(pszPath);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	/* obama만 access 가능하도록 file permission 변경 */
	HLIB_EXTRA_Chmod(pszPath, 0700);

	{
		HBOOL bRoot = FALSE;
		if(HLIB_EXTRA_AmIRoot(&bRoot) == ERR_OK)
		{
			if(bRoot == TRUE)
			{
				if(HLIB_EXTRA_Chown(pszPath, "obama", "obama") != ERR_OK)
				{
					HxLOG_Error("ERROR In [HLIB_EXTRA_Chown]. [%s]\n", pszPath);
				}
			}
		}
	}
#endif

	return hErr;
}

#if 0
STATIC HERROR local_na_vfs_DirGetFileSize(HxDIR_t *pSelf, HUINT64 *pullSize)
{
	return HLIB_DIR_GetFileSize(pSelf, pullSize);
}
#endif

STATIC HERROR local_na_vfs_DirRemove(const HCHAR *pszPath)
{
	return HLIB_DIR_Delete(pszPath);
}


STATIC HERROR local_na_vfs_DirGetTotalSize(const HCHAR *pszPath, HUINT32 *pulSize)
{
	return HLIB_DIR_GetTotalSize(pszPath, pulSize);
}


/**************************************************************************************************/
#define ________CA_SEC_Public_Functions_Body_______________________________________________
/**************************************************************************************************/
HERROR CAS_NA_VfsInitialize(void)
{
	if(local_na_vfs_InitRootPath() != ERR_OK)
	{
		HxLOG_Error("Error in local_na_vfs_InitRootPath()");
		return ERR_FAIL;
	}

	if(local_na_vfs_InitContext() != ERR_OK)
	{
		HxLOG_Error("Error in local_na_vfs_InitContext()");
		return ERR_FAIL;
	}

	return ERR_OK;
}



