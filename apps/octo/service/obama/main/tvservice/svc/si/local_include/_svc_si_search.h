/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_si_search.h
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

#ifndef ___INT_SVC_SI_SEARCH_H___
#define ___INT_SVC_SI_SEARCH_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include "_svc_si_filtering.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSI_SRCH_NITOPT_NoNit		= 0,
	eSI_SRCH_NITOPT_NitAct,
	eSI_SRCH_NITOPT_NitOth,
	eSI_SRCH_NITOPT_NitOthAll,

	eSI_SRCH_NITOPT_EndOfCase
} svcSi_SrchNitOption_e;

typedef struct
{
	SIxTable_Type_e			 eTableType;
	HUINT32					 ulExtId;
	HUINT32					 ulPid;
	HUINT32					 ulFilterId;
	eFilterTableStatus_t	 eFilterStatus;
} svcSi_FilteringInfo_t;

typedef struct
{
	SvcSi_SrchInfoType_e	 eInfoType;
	HUINT32					 ulItemSize;		// sizeof(STRUCTURE)
	HUINT32					 ulExpandNum;		// Array가 부족할 때마다 늘리는 사이즈
	HUINT32					 ulInfoNum, ulArraySize;
	void					*pvInfoArray;
} svcSi_SrchInfoTable_t;

typedef struct
{
	Handle_t				 hAction;
	SvcSi_SiSpec_t			 unSiSpec;
	SvcSi_SearchOption_t	 stOption;

	HUINT32					 ulDemuxId;

	svcSi_SrchNitOption_e	 eNitOption;
	HUINT16					 usNitNetId;
	HUINT16					 usBatBouquetId;

	HINT32					 nNumOfNetId;
	HUINT16					*pusNitOthNetId;

	svcSi_FilteringType_b	 eFiltering;
	HxList_t				*pstFilterings;							// svcSi_FilteringInfo_t
	HUINT32					 ulSearchResult;

	// Search Results:
	svcSi_SrchInfoTable_t	 astSrchInfoTables[eSI_SRCH_INFOTYPE_MAX];

	SiTxtEncodedType_t		 eTextEncType;							// Name 등의 Text의 Encoding Type
} svcSi_SrchData_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
extern HERROR					svc_si_SrchInitSearchData (svcSi_SrchData_t *pstSrchData, Handle_t hAction, SvcSi_SiSpec_t *punSiSpec, SvcSi_SearchOption_t *pstSrchOption);
extern void						svc_si_SrchFreeSearchData (svcSi_SrchData_t *pstSrchData);
extern HERROR					svc_si_SrchGetFilteringTypes (svcSi_SrchData_t *pstSrchData);
extern svcSi_FilteringInfo_t*	svc_si_SrchFindFilterInfoByFilterId (svcSi_SrchData_t *pstSrchData, HUINT32 ulFilterId);
extern svcSi_FilteringInfo_t*	svc_si_SrchFindFilterInfoByTableTypeAndExtId (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType, HUINT32 ulExtId);
extern HERROR					svc_si_SrchStartFilter (svcSi_SrchData_t *pstSrchData, SIxTable_Type_e eTableType, HUINT32 ulExtId, HUINT32 ulPid);
extern HERROR					svc_si_SrchStartSection (svcSi_SrchData_t *pstSrchData);
extern HERROR 					svc_si_SrchStartNitOthMultiTable (svcSi_SrchData_t *pstSrchData);
extern HERROR					svc_si_SrchStopSection (svcSi_SrchData_t *pstSrchData);
extern HBOOL					svc_si_SrchCheckAllSiTableReceived (svcSi_SrchData_t *pstSrchData);

extern HERROR					svc_si_SrchInitSearchResultTable (svcSi_SrchData_t *pstSrchData);
extern HERROR 					svc_si_SrchFreeSearchResult (svcSi_SrchData_t *pstSrchData);
extern HERROR					svc_si_SrchAddSearchResultItem (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType, void *pvItem, HUINT32 *pulItemIndex);
extern HERROR					svc_si_SrchRemoveSearchResultItem (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType, void *pvItem);
extern HERROR					svc_si_SrchClearSearchResult (svcSi_SrchData_t *pstSrchData, SvcSi_SrchInfoType_e eSrchInfoType);
extern HERROR					svc_si_SrchCountSearchedResult (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 *pulInfoNum);
extern HERROR					svc_si_SrchGetSearchedResults (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 ulMaxArrayNum, HUINT32 *pulInfoNum, void *pvInfoArray);


#endif	// ___INT_SVC_SI_SEARCH_H___
/* end of file */

