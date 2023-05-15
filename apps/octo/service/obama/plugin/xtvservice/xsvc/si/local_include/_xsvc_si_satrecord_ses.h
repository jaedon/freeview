
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_satrecord_ses.h
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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

#ifndef ___XSVC_SI_SATRECORD_SES_H___
#define ___XSVC_SI_SATRECORD_SES_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_si.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ASTRA_RECORDING_LINKAGE			0xA0
#define HDPLUS_RECORDING_LINKAGE			0xA1
#define SES_SATREC_BOUQUET_ID			0x0012
#define DSMCC_STREAM_TYPE				0x0B

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
// OUI Structure :

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
extern SvcSi_ActFunc	xsvc_si_SatRecordGetSearchActionFunc_Ses (void);
extern void*			xsvc_si_SatRecordAllocSearchActionData_Ses (void);
extern void				xsvc_si_SatRecordFreeSearchActionData_Ses (void *pvData);
extern HERROR			xsvc_si_ServiceGetSvcAndVideoTypeFromDvbSvcType_Ses (HUINT32 ulActionId, HUINT32 ulDvbSvcType, DxSvcType_e *peSvcType, DxVideoType_e *peVideoType);
extern HERROR			xsvc_si_SatRecordGetSrchData_Ses (Handle_t hAction, SvcSi_SatRecordDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR			xsvc_si_SatRecordFreeSrchData_Ses (Handle_t hAction, SvcSi_SatRecordDataType_t eDataType, void *pvDataArray);

#endif
/* end of file */
