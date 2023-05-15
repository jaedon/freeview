/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_lcnupdate_ses.h
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIVE			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef ___XSVC_SI_LCNUPDATE_SES_H___
#define ___XSVC_SI_LCNUPDATE_SES_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_si.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SESLCN_SVCLIST_NAME_LEN		36		// Spec 은 31이지만 변환시 붙을 수 있는 Size + /0 해서 36으로
#define SESLCN_BOUQUET_NAME_LEN		36

#define ASTRA_REF_NIT_LINKAGE			0x04
#define ASTRA_SGT_LINKAGE				0x90
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
#define ASTRA_PORTAL_BOUQUET_ID		0x11

#define ASTRA_PORTAL_LINKAGE			0x94
#define ASTRA_REGIONAL_PORTAL_LINKAGE	0x95

#define ASTRA_PORTAL_SVC_NUM			0
#define ASTRA_REGIONAL_PORTAL_SVC_NUM	99
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
extern SvcSi_ActFunc	xsvc_si_LcnUpdateSrchGetSearchActionFunc_Ses (void);
extern void*			xsvc_si_LcnUpdateSrchAllocSearchActionData_Ses (void);
extern void				xsvc_si_LcnUpdateSrchFreeSearchActionData_Ses (void *pvData);

extern HERROR 			xsvc_si_LcnUpdateSrchSetExtSrchData_Ses (Handle_t hAction, SvcSi_ExtSrchOptionType_t eOptionType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
extern HERROR 			xsvc_si_LcnUpdateSrchGetExtSrchData_Ses (Handle_t hAction, SvcSi_ExtSrchDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 			xsvc_si_LcnUpdateSrchFreeExtSrchData_Ses (Handle_t hAction, SvcSi_ExtSrchDataType_t eDataType, void *ppvDataArray);


#endif
/* end of file */
