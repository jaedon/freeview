/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_si_live.h
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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

#ifndef ___INT_SVC_SI_LIVE_H___
#define ___INT_SVC_SI_LIVE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSI_LIVESTATE_Init		= 0,
	eSI_LIVESTATE_Skipped,
	eSI_LIVESTATE_Running,
	eSI_LIVESTATE_Done,

	eSI_LIVESTATE_EndOfCase
} svcSi_LiveState_e;

typedef struct
{
	Handle_t				 hAction;

	Handle_t				 hService;
	Handle_t				 hSupService;
	HBOOL					 bFccSvc;

	HUINT32					 ulDemuxId;

	HBOOL					 bAbortPatPmt;
	HUINT16					 usPmtSvcId;
	HUINT16					 usPmtPid;
	HUINT16					 usEitSvcId;
	HUINT16					 usSdtTsId;
	HUINT16					 usNitNetId;
	HUINT16					 usBatBouquetId;
	HUINT16					 usAitPid;

	// To check PAT whether it comes from the current TP or not:
	HUINT32					 ulPatTimeTick;

	svcSi_LiveState_e		 eState;
	svcSi_FilteringType_b	 eTableFiltering;

	// Filtering 정보를 위에 올리는 판단을 위한 값들.
	HUINT32					 aulTimeoutFilterId[eSIxTABLETYPE_MAX];
	HUINT32					 aulCrc32[eSIxTABLETYPE_MAX];

	SiTxtEncodedType_t		 eTextEncType;						// Name 등의 Text의 Encoding Type
} svcSi_LiveData_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
extern HERROR		svc_si_LiveSetLiveData (svcSi_LiveData_t *pstLiveData, Handle_t hAction, Handle_t hMasterSvc, Handle_t hSuppSvc, HUINT32 ulSuppType);
extern HERROR		svc_si_LiveStartSection (svcSi_LiveData_t *pstLiveData, svcSi_FilteringType_b bFiltering);
extern HERROR		svc_si_LiveStopSection (svcSi_LiveData_t *pstLiveData);

extern HERROR 		svc_si_LiveSetPbData (svcSi_LiveData_t *pstLiveData, Handle_t hAction, HUINT16 usTsId, HUINT16 usSvcId, HUINT16 usPmtPid );


#endif	// ___INT_SVC_SI_LIVE_H___
/* end of file */

