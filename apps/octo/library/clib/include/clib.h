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
#ifndef __CLIB_H__
#define __CLIB_H__

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************************************/
#define _________CLIB_Include__________________________________________________________
/**************************************************************************************************/
#include <hlib.h>

/**************************************************************************************************/
#define _________CLIB_MACRO____________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_TYPEDEF__________________________________________________________
/**************************************************************************************************/
typedef struct _CList_t		CList_t;
typedef struct _CListAttr	CListAttr_t;

typedef struct
{
	HUID	uid;
	HBOOL	bLCNControl;
	HINT32	nLogicalNo;
} CListItem_t;

#define	CLIB_TYPE_TV			0x00000001
#define	CLIB_TYPE_RADIO		0x00000002
#define	CLIB_TYPE_HD			0x00000004
#define	CLIB_TYPE_CAS			0x00000008
#define	CLIB_TYPE_SATELLITE		0x00000010
#define	CLIB_TYPE_ALPABET		0x00000020

#define	CLIB_OP_NULL			0x00000000
#define	CLIB_OP_SES				0x00000001
#define	CLIB_OP_ME				0x00000002
#define	CLIB_OP_AUSDTT			0x00000004
#define	CLIB_OP_ERTELECOM		0x00000008
#define	CLIB_OP_UK_DTT			0x00000010
#define	CLIB_OP_ZIGGO			0x00000020
#define	CLIB_OP_ITA_DTT			0x00000040
#define	CLIB_OP_ORF				0x00000080
#define CLIB_OP_TELENOR         0x00000100
#define CLIB_OP_TDC         	0x00000200
#define CLIB_OP_MBC         	0x00000400
#define CLIB_OP_AMX				0x00000800
#define	CLIB_OP_XXX				0x00000FF

/**************************************************************************************************/
#define _________CLIB_VALUE____________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_FUNCTION_________________________________________________________
/**************************************************************************************************/
extern HxVector_t*	CLIB_GenerateList(HxVector_t *pstSvcList, HxVector_t *pstOrgList, HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt, HBOOL bSort);
extern HINT32		CLIB_GetListSize(CList_t *pstList);
extern CList_t*		CLIB_Serialize(HxVector_t *pstList, HINT32 *pnSize);
extern HxVector_t*	CLIB_DeSerialize(CList_t *pstCList);
extern void			CLIB_RemoveCList(CList_t *pstCList);
extern void			CLIB_RemoveItemList(HxVector_t *pstList);

#ifdef	__cplusplus
}
#endif

#endif //end of __CLIB_H__

