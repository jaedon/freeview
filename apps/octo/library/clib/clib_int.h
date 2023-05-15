/**************************************************************
*	http://www.humaxdigital.com
*	@title			contents categorize library
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/
#ifndef __CLIB_INT_H__
#define __CLIB_INT_H__

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************************************/
#define _________CLIB_INT_Include__________________________________________________________
/**************************************************************************************************/
#include <clib.h>

/**************************************************************************************************/
#define _________CLIB_INT_MACRO____________________________________________________________
/**************************************************************************************************/
#define	CLIB_NAME_LEN		128
#define	CLIB_MAGIC_CODE		0xA5A55A5A
/**************************************************************************************************/
#define _________CLIB_INT_TYPEDEF__________________________________________________________
/**************************************************************************************************/
struct _CList_t
{
	HUINT32	ulMagicCode;
	HUINT32	ulCrc;
	HUINT32	ulCount;
	HUINT8	data[4];
};

struct _CListAttr
{
	HUINT32		ulContentsType;
	HUINT32		ulOperatorType;
	HUINT32		ulExt;

	HUINT8		aucData[4];
};

/**************************************************************************************************/
#define _________CLIB_INT_VALUE____________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_INT_FUNCTION_________________________________________________________
/**************************************************************************************************/
extern HUINT32	CLIB_PORT_StartSession(HxVector_t *pstSvcList, HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);
extern void		CLIB_PORT_EndSession(HUINT32 ulSessionId);
extern HBOOL	CLIB_PORT_FilterIn(HUINT32 ulSessionId, DxService_t *pstSvcInfo);
extern HINT32	CLIB_PORT_GetServiceNumber(HUINT32 ulSessionId, DxService_t *pstSvcInfo, HINT32 idx);

//	Default PORT
extern HBOOL		CLIB_PORT_FilterInDefault(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberDefault(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionDefault(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// MIddle East
extern HBOOL		CLIB_PORT_FilterInMiddleEast(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberMiddleEast(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionMiddleEast(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// Ertelecom
extern HBOOL		CLIB_PORT_FilterInErtelecom(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberErtelecom(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionErtelecom(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// SES
extern HBOOL		CLIB_PORT_FilterInSes(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberSes(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionSes(HxVector_t *pstSvcList, HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// AusDtt
extern HBOOL		CLIB_PORT_FilterInAusDtt(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberAusDtt(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionAusDtt(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

//UkDtt
extern HBOOL		CLIB_PORT_FilterInUkDtt(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberUkDtt(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t* CLIB_PORT_InitSessionUkDtt(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// ziggo
extern CListAttr_t*	CLIB_PORT_InitSessionZiggo(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);
extern HBOOL		CLIB_PORT_FilterInZiggo(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberZiggo(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);

//ItaDtt
extern HBOOL		CLIB_PORT_FilterInItaDtt(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberItaDtt(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t* CLIB_PORT_InitSessionItaDtt(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// Orf
extern HBOOL		CLIB_PORT_FilterInOrf(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberOrf(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionOrf(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// Telenor
extern HBOOL		CLIB_PORT_FilterInTelenor(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberTelenor(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionTelenor(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// TDC
extern HBOOL		CLIB_PORT_FilterInTDC(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberTDC(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionTDC(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// MBC
extern HBOOL		CLIB_PORT_FilterInMBC(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberMBC(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionMBC(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

// AMX
extern HBOOL		CLIB_PORT_FilterInAMX(CListAttr_t *pstAttr, DxService_t *pstSvcInfo);
extern HINT32		CLIB_PORT_GetServiceNumberAMX(CListAttr_t *pstAttr, DxService_t *pstSvcInfo, HINT32 idx);
extern CListAttr_t*	CLIB_PORT_InitSessionAMX(HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt);

#ifdef	__cplusplus
}
#endif

#endif //end of __CLIB_INT_H__


