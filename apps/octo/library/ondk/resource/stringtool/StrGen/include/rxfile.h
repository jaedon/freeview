
/*************************************************************
 * @file		rxfile.h
 * @date		2011/04/19
 * @author
 * @brief		HUMAX common library header, related with codepage
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXFILE_H_
#define	_HXFILE_H_


/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */

typedef 	void *		RxFile_t;


#define RxFILE_ERR_FAIL 	-1
#define	RxFILE_ERR_OK 		0

typedef	enum tagRxFILE_SEEK
{
	eRxFILE_SEEK_SET = 0,
	eRxFILE_SEEK_CUR = 1,
	eRxFILE_SEEK_END = 2
} RxFILE_Seek_e;


/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */


//HINT32 MHFile_Scanf(MHFile_t hSelf, const HUINT8 *format, ...)
//HINT32 MHFile_Printf(MHFile_t hSelf, const HUINT8 *format, void* output)

#define	RxFile_Scanf(hSelf, format, ...)	fscanf(hSelf, format, __VA_ARGS__)
#define RxFile_Printf(hSelf, format, ...)	fprintf(hSelf, format, __VA_ARGS__)


HINT32				RxFile_Flush(RxFile_t hSelf);
HINT32				RxFile_EOF(RxFile_t hSelf);

HINT32				RxFile_GetChar(RxFile_t hSelf);
HINT32 				RxFile_PutChar(RxFile_t hSelf, HUINT8 ch);


HUINT32				RxFile_Error(RxFile_t hSelf);

HERROR				RxFile_Size(RxFile_t hSelf, HUINT32 *pHi, HUINT32 *pLo);
HERROR 				RxFile_Seek(RxFile_t hSelf, HUINT32 hi_offset, HUINT32 lo_offset, RxFILE_Seek_e origin, HUINT32 *pHi, HUINT32 *pLo);
HERROR				RxFile_Tell(RxFile_t hSelf, HUINT32 *pHi, HUINT32 *pLo);


HUINT8*				RxFile_GetString(RxFile_t hSelf, HUINT8 *string, HINT32 len);

RxFile_t 			RxFile_Open(const HUINT8 *filename, const HUINT8 *mode);
HINT32	 			RxFile_Close(RxFile_t hSelf);

HUINT32				RxFile_Read(RxFile_t hSelf, void *buffer, HUINT32 size, HUINT32 count);
HUINT32				RxFile_Write(RxFile_t hSelf, void *buffer, HUINT32 size, HUINT32 count);


// flag
HERROR				RxFile_NoBuffering(RxFile_t hSelf, HBOOL bEnable);

// util
HERROR				RxFile_Make(const HUINT8 *pName);
HERROR				RxFile_Delete(const HUINT8 *pName);
HERROR				RxFile_Move(const HUINT8 *pOldName, const HUINT8 *pNewName);
HERROR				RxFile_Copy(const HUINT8 *pName1, const HUINT8 *pName2);


/* *************************************************************************************************** */
#define ___Debug_Definition___________________________________________________________________________
/* *************************************************************************************************** */

#if defined(CONFIG_DEBUG)


#endif

#endif	/* _HXFILE_H_ */



