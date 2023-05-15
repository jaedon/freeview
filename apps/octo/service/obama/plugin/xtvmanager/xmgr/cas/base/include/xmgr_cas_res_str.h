/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  ap_cas_res_str.h
	@brief	  ap_cas_res_str header

	Description: cas string infomation, etc		\n
	Module: 		 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__AP_CAS_RES_STR_H__
#define	__AP_CAS_RES_STR_H__

#define	__XMGR_CAS_INSIDE__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <xmgr_cas_str_enum.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	struct	_tagResourceStruct_t
{
	HUINT32		ulSelectedSubItem;
	HCHAR		*dataFileName;
	void		*dataFileHandle;
	void		*rawData;
} AP_CasResStr_t;



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR		AP_CasResStr_Init(/*HCHAR *resourceDataPath*/void);
HERROR		AP_CasResStr_PrepareResource(HUINT8 *szLang639Code);
HERROR		AP_CasResStr_GetResourceDataById(HUINT32 ulId, HUINT8* pszStr, HUINT32 *pulSize);
HCHAR		*AP_CasResStr_GetStrRsc(HUINT32 ulId);

#undef	__XMGR_CAS_INSIDE__

#endif /* !__AP_CAS_RES_STR_H__ */


