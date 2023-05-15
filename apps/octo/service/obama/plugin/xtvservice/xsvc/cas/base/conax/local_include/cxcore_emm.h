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
@file				cxcore_emm.h
@brief			cxcore_emm.h

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_EMM_H__
#define	__CXCORE_EMM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/

#define CX_MAX_EMM_ADDR			8
#define CX_EMM_ADDR_LEN			(1+7)	// 맨앞에다가는 0x82를 넣을 것이다.
#define	CX_EMM_MAX_GET_HANDLE	5		// 3번(CX_SC_MAX_RETRY_SEND_CMD 참조) 이상 될 수 없으니 여유를 생각해서...

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

typedef enum
{
	eCxEmm_Init,
	eCxEmm_Running,
	eCxEmm_Stopped
} CxEmmState_t;

typedef struct
{
	CxEmmState_t	eState;
	HUINT16			usUniqueId;		// filter 거는데 필요
	HUINT32			ulFilterId;		// filter 거는데 필요
	HUINT8			aucFilter[CX_EMM_ADDR_LEN];
	HUINT8			aucMask[CX_EMM_ADDR_LEN];
} CxEmmAddr_t;

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HERROR CXEMM_InitCore(void);
HERROR CXEMM_UpdateCat(Handle_t hSession, HUINT16 usUniqueTsId, HUINT8* pucCat);

HERROR CXEMM_RestartFilterByTsId(Handle_t hSession, HUINT16 usUniqueTsId);

HERROR CXEMM_Add(Handle_t hEmm, HUINT16 usEmmPid, HUINT16 usAddrCnt, CxEmmAddr_t* pstAddr);
HERROR CXEMM_Delete(HINT32 nScSlot);
HERROR CXEMM_DeleteByTsId(HUINT16 usUniqueTsId);

HUINT8* CXEMM_QueueGet(HINT32* pnScSlot);
HERROR CXEMM_QueueClear(void);

#if defined(CONFIG_DEBUG)
void CMD_CXEMM_PrintEmmContext(void);
#endif
#endif /* !__CXCORE_EMM_H__ */
