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
@file			cxcore_cpm.h
@brief			cxcore_cpm.h

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_CPM_H__
#define	__CXCORE_CPM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

#define	CX_CPM_MAX_KEY_SLOT		256//32	//64
#define	CX_CPM_KEY_VALUE_BYTES	32

#define	CX_CPM_INVALID_KEYID	0x0000

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/

typedef enum
{
	eCxCpm_LPK	= 0x02,
	eCxCpm_CWPK	= 0x05,
	eCxCpm_ALL	= 0xFF
} CxCpmKeyType_t;

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/
typedef struct
{
	HUINT16	usId;
	HUINT8	ucType;	// LPK(010) | CWPK (101), @see CxCpmKeyType_t	// ID만...저장 하면 될까??
} CxCpmKeyCurrentInUse_t;

typedef struct
{
	HUINT16	usId;
	HUINT8	ucType;	// LPK(010) | CWPK (101), @see CxCpmKeyType_t
	HUINT8	ucLength;
	HUINT8	aucValue[CX_CPM_KEY_VALUE_BYTES];
} CxCpmKey_t;

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HERROR CXCPM_InitCore(void);
HBOOL CXCPM_IsEncryptedCwMode(void);
HUINT32 CXCPM_CountKey(CxCpmKeyType_t type);
HERROR CXCPM_GetProtocolId(HUINT16* pusId);
HERROR CXCPM_GetKey(HUINT16 usKeyId, CxCpmKey_t* pKey);
HBOOL CXCPM_HasValue(CxCpmKey_t* pKey);
HERROR CXCPM_Process_UPDATE_KEY(void* context, HUINT32 ulChipId, CxCpmKeyType_t type, HUINT16 usKeyId, const HUINT8* pucValue, HUINT8 ucPL);
HERROR CXCPM_Process_REMOVE_KEY(void* context, HUINT32 ulChipId, HUINT16 usKeyId);

HERROR CXCPM_SetTrackingKey(HUINT16 usId, HUINT8 ucType);
HERROR CXCPM_CompareTrackingKey(HUINT16 usId, HUINT8 ucType);
HERROR CXCPM_CompareAndResetTrackingKey(HUINT16 usId, HUINT8 ucType);


#endif /* !__CXCORE_CPM_H__ */

