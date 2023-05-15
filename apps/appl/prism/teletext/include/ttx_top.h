
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __TTX_TOP_H__
#define __TTX_TOP_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <apk.h>
//#include <octo_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define	TOP_AIT_TITLE_LEN			12


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

enum tagTTX_TOP_Error
{
	ERR_TOP_OK		= ERR_OK,
	ERR_TOP_FAIL,

	ERR_TOP_MALLOC_FAIL,
	ERR_TOP_NO_BTT,
	ERR_TOP_INVALID_ARGUMENT,
	ERR_TOP_UPDATED,

	ERR_TOP_LAST
};

typedef enum
{
	eTTX_TOP_UNKNOWN,

	eTTX_TOP_BTT,
	eTTX_TOP_MPT,
	eTTX_TOP_AIT,
	eTTX_TOP_MPT_EX,

	eTTX_TOP_LAST
} TTX_TOP_Type_t;


typedef struct
{
	HBOOL	bEnabled;
	HUINT16	usPageNum;
	HUINT16	usSubcode;
	HUINT8	aucTitle[TOP_AIT_TITLE_LEN];
} TTX_TOP_Link_t;


typedef struct
{
	TTX_TOP_Type_t	type;
	HBOOL			bUpdated;
	HUINT16			usPageNum;
	HUINT16			usSubcode;
	HUINT8			*apData[23];
} TTX_TOP_Table_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

Handle_t		TTX_TOP_Create 	(void);

void			TTX_TOP_Destroy (
							Handle_t 		hTOP
							);

TTX_TOP_Type_t	TTX_TOP_GetType (
							Handle_t 		hTOP, 
							const HUINT16	usPageNum
							);

HUINT32			TTX_TOP_GetCurVersion (
							Handle_t		hTOP
							);

HERROR 			TTX_TOP_Receive (
							Handle_t 		hTOP, 
							TTX_TOP_Table_t	*pTable
							);

HERROR 			TTX_TOP_GetTOPLink (
							Handle_t 		hTOP, 
							const HUINT16	usPageNum, 
							TTX_TOP_Link_t	aLink[]
							);

HERROR 			TTX_TOP_GetTraceLink (
							Handle_t 		hTOP, 
							const HUINT16 	usPageNum, 
							HUINT16			ausCycle[]
							);

HERROR			TTX_TOP_IsBlockOrGroup (
							Handle_t		hTOP,
							const HUINT16	usPageNum,
							const HUINT16	usSubcode
							);

HERROR 			TTX_TOP_IsPageExist (
							Handle_t 		hTOP, 
							const HUINT16 	usPageNum, 
							const HUINT16 	usSubcode
							);

#endif // __TTX_TOP_H__

