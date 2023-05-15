/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*******************************************************************
	File Description
********************************************************************/
/**
@file				cxcore_nvm.h
@brief			cxcore_nvm.h

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_NVM_H__
#define	__CXCORE_NVM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>
#include "../local_include/cxcore_sc.h"

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HERROR CXNVM_InitCore(void);

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
HUINT32 CXNVM_GetUserTextSlotNum(void);
HERROR CXNVM_ReadUserText(HUINT8 ucSlotIdx, CxUserText_t* pstText);
HERROR CXNVM_UserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt);
HERROR CXNVM_WriteUserText(CxUserText_t* pstText, HBOOL	bStartTimeOmit);
HERROR CXNVM_OverWriteUserText(HUINT8 ucSeqNo, HUINT32 ulCurrentUnixTime);
HERROR CXNVM_RemoveUserText(HUINT8 ucSeqNo);
HERROR CXNVM_CompareUserTextWithoutStartTime(CxUserText_t* pstText);
HERROR CXNVM_IsAlreadyDisplayed(CxUserText_t* pstText);
#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
HERROR CXNVM_FingerprintDisplayed(void);
HERROR CXNVM_ReadFingerprint(CxFingerprint_t* pstFingerprint);
HERROR CXNVM_WriteFingerprint(CxFingerprint_t* pstFingerprint);
HERROR CXNVM_DelayedWriteFingerprint(CxFingerprint_t* pstFingerprint);
HERROR CXNVM_ResetCompareFPTime(void);
#endif

#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXNVM_WriteKeyStore(void* pvData, HUINT32 ulOffset, HUINT32 ulLength);
HERROR CXNVM_ReadKeyStore(void* pvData, HUINT32 ulOffset, HUINT32 ulLength);
#endif

#if defined(CONFIG_MW_MM_PVR)
HERROR CXNVM_WritePvrKeyWithHash(HUINT8* pucKey, HUINT32 ulLength);
HERROR CXNVM_ReadPvrKeyWithHash(HUINT8* pucKey, HUINT32 ulLength);
#endif

#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
HERROR CXNVM_WriteCAPINCode(HUINT8* pucKey, HUINT32 ulLength);
HERROR CXNVM_ReadCAPINCode(HUINT8* pucKey, HUINT32 ulLength);
#endif
void CXNVM_ClearConaxUserData(void);


// API function (which can be invoked from outside of the conax core)
/* move to conax.h
*/
#endif /* !__CXCORE_NVM_H__ */

