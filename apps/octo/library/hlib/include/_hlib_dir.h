
/*************************************************************
 * @file		hxfile.h
 * @date		2011/04/19
 * @author
 * @brief		HUMAX common library header, related with codepage
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXDIR_H_
#define	_HXDIR_H_



/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */




/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */
typedef struct tagHxDIR		HxDIR_t;

#define	MAX_HXDIR_PATHNAME_LENGTH				512
#define MAX_HXDIR_FILENAME_LENGTH				512
#define MAX_HXDIR_EXTNAME_LENGTH				16

#define HXDIR_INT_STORAGE_ROOT		"/mnt"
#define HXDIR_EXT_STORAGE_ROOT		"/media"



#if defined(CONFIG_PROD_OS_EMULATOR)
typedef struct tagHxDIR_Stat_t {
        int     st_dev;
        int     st_ino;
        int st_mode;
        int      st_nlink;
        int      st_uid;
        int      st_gid;
        int    st_rdev;
        int     st_size;
        int st_atime;
        int st_mtime;
        int st_ctime;
} HxDIR_Stat_t;
#else
typedef  struct stat HxDIR_Stat_t;

#endif
/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */

extern HxDIR_t*	HLIB_DIR_Open(const HCHAR *pszPath);
extern HERROR	HLIB_DIR_Next(HxDIR_t *pSelf);
extern HERROR	HLIB_DIR_Close(HxDIR_t *pSelf);
extern HCHAR*	HLIB_DIR_GetName(HxDIR_t *pSelf);
extern HCHAR*	HLIB_DIR_GetPath(HxDIR_t *pSelf);
extern HBOOL	HLIB_DIR_IsDir(HxDIR_t *pSelf);
extern HBOOL	HLIB_DIR_IsFile(HxDIR_t *pSelf);
extern HERROR	HLIB_DIR_IsValid(const HCHAR *name);
extern HBOOL	HLIB_DIR_IsExist(const HCHAR *pszDirName);
extern HBOOL	HLIB_DIR_IsDirPath(HCHAR *pszDirName);
extern HINT32	HLIB_DIR_Find(HUINT8	*pszPath, HUINT8 *pszFilter, HUINT8 *pszOption,
						HBOOL (* func)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata), void *userdata);
extern HINT32	HLIB_DIR_FindEx(HUINT8	*pszPath, HUINT8 *pszFilter, HUINT8 *pszOption,
							HBOOL (* cbfile)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata),
							HBOOL (* cbstartdir)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata),
							HBOOL (* cbenddir)(HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata),
							void *userdata);
extern HERROR	HLIB_DIR_Make(const HCHAR *pszPath);
extern HERROR	HLIB_DIR_Chmod(const HCHAR *pszPath, HUINT32 ulMode);
extern HERROR	HLIB_DIR_Move(const HCHAR *pszOldPath, const HCHAR *pszNewPath);
extern HERROR	HLIB_DIR_Delete(const HCHAR *pszPath);
extern HERROR	HLIB_DIR_Copy(const HCHAR *pszPath1, const HCHAR *pszPath2);
extern HERROR 	HLIB_DIR_GetTime(HxDIR_t *pSelf, UNIXTIME *pulCreateTime, UNIXTIME *pulAccessTime, UNIXTIME *pulModificationTime);
extern HERROR	HLIB_DIR_GetFileSize(HxDIR_t *pSelf, HUINT64 *pullSize);
extern HERROR	HLIB_DIR_GetTotalSize(const HCHAR *pszPath, HUINT32 *pulSize);
extern HBOOL	HLIB_DIR_IsDirectoryEmpty(const HCHAR *pszDirMame);
extern HUINT64	HLIB_DIR_GetFileUniqueId(HxDIR_t *pSelf, struct stat *pFstat);
extern HBOOL	HLIB_DIR_IsMountedPath (const HCHAR *path);
extern HERROR	HLIB_DIR_GetMountPath(const HCHAR *path, HCHAR *pszMountPath, HINT32 nLen);
extern HERROR	HLIB_DIR_GetMountFstype(const HCHAR *path, HCHAR *pszFsType, HINT32 nLen);
extern HERROR	HLIB_DIR_GetMountOpts(const HCHAR *path, HCHAR *pszOpts, HINT32 nLen);

/* *************************************************************************************************** */
#define ___Debug_Definition___________________________________________________________________________
/* *************************************************************************************************** */


#endif	/* _HXDIR_H_ */



