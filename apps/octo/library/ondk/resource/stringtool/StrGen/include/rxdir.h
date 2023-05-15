
/*************************************************************
 * @file		rxfile.h
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
typedef void*	RxDIR_t;

#define MAX_HXDIR_FILENAME_LENGTH							512


/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */

RxDIR_t			*RxDIR_Open(HUINT8	*pszPath);
HERROR			RxDIR_Next(RxDIR_t *pSelf);
HERROR			RxDIR_Close(RxDIR_t *pSelf);
HUINT8			*RxDIR_GetName(RxDIR_t *pSelf);
HUINT8			*RxDIR_GetPath(RxDIR_t *pSelf);
HBOOL			RxDIR_IsDir(RxDIR_t *pSelf);
HBOOL			RxDIR_IsFile(RxDIR_t *pSelf);
HINT32			RxDIR_Find(HUINT8	*pszPath, HUINT8 *pszFilter, HUINT8 *pszOption,
					HBOOL (* func)(RxDIR_t *pSelf, HUINT8 *pszPath, HUINT8 *pszName, HBOOL bIsDir, void *userdata),
					void *userdata);


HERROR			RxDIR_Make(const HUINT8	*pszPath);
HERROR			RxDIR_Move(const HUINT8	*pszOldPath, const HUINT8	*pszNewPath);
HERROR			RxDIR_Delete(const HUINT8	*pszPath);
HERROR			RxDIR_Copy(const HUINT8	*pszPath1, const HUINT8	*pszPath2);
HERROR 			RxDIR_GetTime(RxDIR_t *pSelf, UNIXTIME *pulCreateTime, UNIXTIME *pulAccessTime, UNIXTIME *pulModificationTime);
HERROR 			RxDIR_GetFileSize(RxDIR_t *pSelf, HUINT32 *pulHighFileSize, HUINT32 *pulLowFileSize);
RxDIR_t			*RxDIR_Dup(RxDIR_t *pDir);

/* *************************************************************************************************** */
#define ___Debug_Definition___________________________________________________________________________
/* *************************************************************************************************** */

#if defined(CONFIG_DEBUG)


#endif



#endif	/* _HXDIR_H_ */



