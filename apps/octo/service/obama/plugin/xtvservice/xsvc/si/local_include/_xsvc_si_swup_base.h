/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_swup_base.h
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /SWUP CHECK	\n

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

#ifndef ___XSVC_SI_SWUP_BASE_H___
#define ___XSVC_SI_SWUP_BASE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HBOOL		 bUseUnt;
	HUINT8		 ucUpdateVer;
	HUINT16		 usSvcId;
	HUINT16		 usPid;
} xsvcSi_SwupPsiInfo_t;

typedef struct
{
	svcSi_SwupCheck_Base_t		 stBase;

	HUINT16						 usPmtSvcId;
	HUINT16						 usPmtPid;

	HUINT16						 usBatBouquetId;
	HUINT16						 usNetIdForNitOth;	// DVB-SSU SWUP시 이 값이 0이 아니면 NIT OTH도 사용한다

	HBOOL						 bFound;
	SvcSi_SwupSignal_t	 		 stSwupInfo;

	HUINT16 					 usTsId;
} xsvcSi_SwupData_t;

/*******************************************************************/
/********************      Static variable     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Functions     ********************/
/*******************************************************************/

#endif // ___XSVC_SI_SWUP_BASE_H___

/* end of file */

