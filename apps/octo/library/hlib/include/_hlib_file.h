
/*************************************************************
 * @file		hxfile.h
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

typedef 	void *		HxFILE_t;


#define HxFILE_ERR_FAIL 	-1
#define	HxFILE_ERR_OK 		0

typedef	enum tagHxFILE_SEEK
{
	eHxFILE_SEEK_SET = 0,
	eHxFILE_SEEK_CUR = 1,
	eHxFILE_SEEK_END = 2
} HxFILE_Seek_e;

/*
	r	   Open text file for reading.	The stream is positioned at the beginning of the file.
	r+	   Open for reading and writing.  The stream is positioned at the beginning of the file.
	w	   Truncate file to zero length or create text file for writing.  The stream is positioned at the beginning  of  the
		   file.
	w+	   Open  for  reading and writing.	The file is created if it does not exist, otherwise it is truncated.  The stream
		   is positioned at the beginning of the file.
	a	   Open for appending (writing at end of file).  The file is created if it does not exist.	The stream is positioned
		   at the end of the file.
	a+	   Open  for reading and appending (writing at end of file).  The file is created if it does not exist.  The initial
		   file position for reading is at the beginning of the file, but output is always appended to the end of the file.
*/
#define HxFILE_MODE_BINARY_READ_ONLY_OR_NULL_RETURL		"rb"
#define HxFILE_MODE_BINARY_WRITE_ONLY_WITH_EMPTY_FILE	"wb"
#define HxFILE_MODE_BINARY_RW_OR_NULL_RETURL			"r+b"
#define HxFILE_MODE_BINARY_RW_WITH_EMPTY_FILE			"w+b"


/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */


//HINT32 HLIB_FILE_Scanf(HxFILE_t hSelf, const HUINT8 *format, ...)
//HINT32 HLIB_FILE_Printf(HxFILE_t hSelf, const HUINT8 *format, void* output)

#define	HLIB_FILE_Scanf	fscanf
#define HLIB_FILE_Printf	fprintf


HINT32				HLIB_FILE_Flush(HxFILE_t hSelf);
HINT32				HLIB_FILE_EOF(HxFILE_t hSelf);

HINT32				HLIB_FILE_GetChar(HxFILE_t hSelf);
HINT32 				HLIB_FILE_PutChar(HxFILE_t hSelf, HUINT8 ch);


HUINT32				HLIB_FILE_Error(HxFILE_t hSelf);

HERROR 				HLIB_FILE_Seek(HxFILE_t hSelf, HOffset_t offset, HxFILE_Seek_e origin, HOffset_t *ret_pos);
HERROR				HLIB_FILE_Tell(HxFILE_t hSelf, HOffset_t *ret_pos);
HERROR				HLIB_FILE_Size(HxFILE_t hSelf, HUINT64 *ret_size);
HERROR				HLIB_FILE_Size32(HxFILE_t hSelf, HUINT32 *ret_size);
HERROR              HLIB_FILE_SizeFromPath(const HUINT8 *path, HUINT64 *ret_size);

HUINT8*				HLIB_FILE_GetString(HxFILE_t hSelf, const HUINT8 *string, HINT32 len);

HxFILE_t 			HLIB_FILE_Open(const HUINT8 *filename, const HUINT8 *mode);
HxFILE_t 			HLIB_FILE_OpenByIndex(const HUINT8 *filename, const HUINT8 *mode);
HINT32	 			HLIB_FILE_Close(HxFILE_t hSelf);

HUINT32				HLIB_FILE_Read(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count);
HUINT32				HLIB_FILE_Write(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count);
HUINT32				HLIB_FILE_WriteWithoutFlush(HxFILE_t hSelf, void *buffer, HUINT32 size, HUINT32 count);


// flag
HERROR				HLIB_FILE_NoBuffering(HxFILE_t hSelf, HBOOL bEnable);

// util
HERROR				HLIB_FILE_Make(const HUINT8 *pName);
HERROR				HLIB_FILE_Delete(const HUINT8 *pName);
HERROR				HLIB_FILE_Move(const HUINT8 *pOldName, const HUINT8 *pNewName);
HERROR				HLIB_FILE_Copy(const HUINT8 *pSrcName, const HUINT8 *pDstName);
HERROR				HLIB_FILE_IsValid(const HCHAR *name);
HERROR				HLIB_FILE_ReadFile(const HUINT8 *pName, void **ppvData, HUINT32 *pulLen);
HERROR				HLIB_FILE_WriteFile(const HUINT8 *pName, void *pvData, HINT32 len);
HERROR				HLIB_FILE_WriteFileWithLock(const HUINT8 *pName, void *pvData, HINT32 len);
HBOOL 				HLIB_FILE_Exist(const HUINT8 *pName);

HERROR				HLIB_FILE_CopyWithLock(const HCHAR *pName1, const HCHAR *pName2);

void				HLIB_FILE_FreeBuffer(void *p);

HERROR				HLIB_FILE_Touch(const HCHAR *pszFileName);
/* *************************************************************************************************** */
#define ___Debug_Definition___________________________________________________________________________
/* *************************************************************************************************** */


#endif	/* _HXFILE_H_ */



