
#ifndef __SILIB_RAWPOOL_H__
#define __SILIB_RAWPOOL_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_rawpool.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define SILIB_RAWPOOL_NAME_LEN				31
#define SILIB_RAWPOOL_HANDLE_NULL			((SIxRawPoolHandle_t)NULL)

/*******************************************************************/
/********************      Enumerations         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Data Type         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32				 ulUniqueId;
	HUINT32				 ulTsIdx;
} SIxRawOption_t;

typedef struct
{
	HCHAR			 	 szPoolName[SILIB_RAWPOOL_NAME_LEN + 1];
	SIxSpec_e			 eSpec;
	SIxTextEncoding_e	 eTextEnc;
} SIxRawPoolProperty_t;

typedef HUINT32			 SIxRawPoolHandle_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HUINT8*				SILIB_RAW_DuplicateRaw (HUINT8 *pucRaw);
extern SIxRawPoolHandle_t	SILIB_RAW_CreatePool (SIxRawPoolProperty_t *pstProperty);
extern HERROR				SILIB_RAW_DestroyPool (SIxRawPoolHandle_t hPool);
extern HERROR				SILIB_RAW_ClearPool (SIxRawPoolHandle_t hPool);
extern HERROR				SILIB_RAW_GetProperty (SIxRawPoolHandle_t hPool, SIxRawPoolProperty_t *pstProperty);
extern HERROR				SILIB_RAW_ChangeProperty (SIxRawPoolHandle_t hPool, SIxRawPoolProperty_t *pstProperty);
extern HERROR 				SILIB_RAW_GetIncompleteSectionStatus (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulExtId, HBOOL *pbIncomplete);
extern HERROR 				SILIB_RAW_FinalizeIncompleteSection (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulExtId);
extern HERROR				SILIB_RAW_AddRawSection (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT8 *pucRaw, SIxRawOption_t *pstOption, HBOOL bIncompleteSec);
extern HERROR 				SILIB_RAW_FindSameRawSection (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulExtId, HUINT8 *pucRaw);
extern HERROR				SILIB_RAW_FreeRawSectionByExtId (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId);
extern HERROR				SILIB_RAW_FreeRawSectionByUniqueId (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulUniqueId);
extern HERROR				SILIB_RAW_FreeAllRawSection (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType);
extern SIxTable_PsiTable_t*	SILIB_RAW_Parse (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HUINT32 ulExtId);
extern HUINT32				SILIB_RAW_CountRaw (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId);
extern HERROR				SILIB_RAW_GetRawArrays (SIxRawPoolHandle_t hPool, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulNumRaw);
extern SIxDescriptor_t*		SILIB_RAW_ParseDes (SIxRawPoolHandle_t hPool, const HUINT8 *pucRawData);

#ifdef __cplusplus
}
#endif

#endif // __SILIB_RAWPOOL_H__
