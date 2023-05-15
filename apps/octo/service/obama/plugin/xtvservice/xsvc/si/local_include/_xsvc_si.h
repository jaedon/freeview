/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si.h
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

#ifndef ___INT_XSVC_SI_H___
#define ___INT_XSVC_SI_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <xsvc_si.h>
#include <_svc_si_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define UNDEFINED_SERVICE_NAME ((HUINT8 *)"Unknown")
#define UNDEFINED_NETWORK_NAME ((HUINT8 *)"Unknown")
#define UNDEFINED_PROVIDER_NAME ((HUINT8 *)"Unknown")

// 중동향 OP 의 Provider Name
#define ADTV_PROVIDER_NAME		(HUINT8 *)"ADTV"
#define Aljazeera_PROVIDER_NAME	(HUINT8 *)"Aljazeera"
#define CNE_PROVIDER_NAME		(HUINT8 *)"CNE"

// Provider name of Digiturk
#define DIGITURK_PROVIDER_NAME (HUINT8*)"Digital Platform"


#define PRIVATE_DATA_SPECIFIER_UNDEFINED		0x00000000
#define PRIVATE_DATA_SPECIFIER_FOR_UK_DTT		0x0000233A	/* UK_DTT */
#define PRIVATE_DATA_SPECIFIER_FOR_UK_FSAT		0x46534154	/* FSAT */
#define PRIVATE_DATA_SPECIFIER_FOR_ZIGGO		0x00000028	/* ZIGGO */
#define PRIVATE_DATA_SPECIFIER_FOR_NORDIG		0x00000029	/* NORDIG */
#define PRIVATE_DATA_SPECIFIER_FOR_YOUSEE		0x00000031	/* YOUSEE : Value assigned by ETSI in TR101 162. The value for YouSee is 0x00000031 */
#define PRIVATE_DATA_SPECIFIER_FOR_YOUSEE_TEST_STREAM		0xFCFCFCFC


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/** @breaf ICT Section */
typedef struct	tagICT_SECTION_INFO
{
	HUINT8	ucManufacturerCode;
	HUINT8	ucHardwareCode;
	HUINT8	aucSeqNumber[255];
	HUINT8	aucTableId[255];
	HUINT16 ausDownloadTime[255];
} xsvcSi_IctSectionInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
HERROR	xsvc_si_SetSiMode_FreeSat(XsvcSi_SiModeFsat_e eSiMode);
HERROR	xsvc_si_GetSiMode_FreeSat(XsvcSi_SiModeFsat_e *peSiMode);

// Mheg Callback
HERROR xsvc_si_SetActionModuleMheg(void);

HERROR xsvc_si_GetAlternativeAudioPriority_Base(const svcSi_AudioEs_t *pstAudioEs, HBOOL bLangMatched, HINT32 *pnUserPriority, HxLANG_Id_e *peLangId, DxDualMonoSelect_e *peDualMonoSelect);
HERROR xsvc_si_GetAlternativeAudioPriority_UkDtt(const svcSi_AudioEs_t *pstAudioEs, HBOOL bLangMatched, HINT32 *pnUserPriority, HxLANG_Id_e *peLangId, DxDualMonoSelect_e *peDualMonoSelect);


#endif	/* ___INT_XSVC_SI_H___ */
/* end of file */
