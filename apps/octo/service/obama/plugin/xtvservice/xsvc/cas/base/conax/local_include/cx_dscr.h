/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cx_dscr.h
	@brief  	 Descrambler Adaptation Layer

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
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
* ⓒ 2011-2012 Humax Co., Ltd.
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
#ifndef __CX_DSCR_H__
#define __CX_DSCR_H__
/*******************************************************************/
/********************     Include files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************    Types and structures   ********************/
/*******************************************************************/
#define VALID_PID(x)						(x > 0 && x < 0x2000)
#define INVALID_PID(x)						!VALID_PID(x)

#define	CX_DSCR_VALUE_BYTES					32	// cxcore_cpm.h CX_CPM_KEY_VALUE_BYTES과 같은 값


/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
void CXDSCR_Init(void);
void CXDSCR_Exit(void);
HUINT32 CXDSCR_Open(Handle_t hEcm, HUINT32 ulCxStreamIdx, HUINT16 usPid, Handle_t hAction);
HERROR CXDSCR_Close(HUINT32 ulStbDscrHandle, HUINT16 usPid);
HERROR CXDSCR_SetKeys(HUINT32 ulStbDscrHandle, HUINT16 usOddKeyLength, const HUINT8* pucOddKey, HUINT16 usEvenKeyLength, const HUINT8* pucEvenKey, void* pUserData);
void CXDSCR_ChangePid(HUINT32 ulCxStreamIdx, HUINT16 usPid);
#if defined (CONFIG_MW_CAS_CONAX_CPM)
HERROR CXDSCR_ClearCWPK(HUINT32 ulDscrHandle);
#endif

#endif	/* !__CX_DSCR_H__ */
