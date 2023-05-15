/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_spec_ukdtt.h
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /OTA			\n

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

#ifndef ___INT_XSVC_SI_UKDTT_H___
#define ___INT_XSVC_SI_UKDTT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_si.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	xSvcSi_UKDTT_SERVICE_REPLACEMENT_TYPE		(0x5)
#define	xSvcSi_UKDTT_MHEG_DATA_BROADCAST_ID		(0x0106)
#define	xSvcSi_UKDTT_MHEG_APPLICATION_CODE1		(0x0101)
#define	xSvcSi_UKDTT_MHEG_APPLICATION_CODE2		(0x0505)

#define	XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_PATH				"/mnt/hd1/ncd/"
#define	XsvcSi_UKDTT_NCD_INFO_LIST_SAVE_FILENAME			"ncd.dat"

#define	XsvcSi_UKDTT_MAX_NCD_INFO_NUM				(75)		/* Spec 내용이 50개로 알고 있지만, Suite의 최대 개수로 조정하자 */

#define	XsvcSi_UKDTT_NCD_SAVE_MAGIC_CODE				(0x4E434444)
#define	XsvcSi_UKDTT_NCD_SAVE_HEADER_LEN				(12)		/* magic code + ncd num + crc */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

extern HERROR			xsvc_si_SetNcdNotifyData_UkDtt(HUINT32 ulASONotifyFlag);
extern HERROR			xsvc_si_ClearNcdNotifyData_UkDtt(void);

#endif // ___INT_XSVC_SI_UKDTT_H___

/* end of file */
