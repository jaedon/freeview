/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_si_swup.h
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

#ifndef ___INT_SVC_SI_OTA_CHECK_H___
#define ___INT_SVC_SI_OTA_CHECK_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_si.h>
#include <_svc_si_search.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

enum
{
	eLINKAGE_REF_NIT		= 0x04,
	eLINKAGE_SSU_SCAN		= 0x0A,
	eLINKAGE_SSU 			= 0x09,

	eLINKAGE_ALL			= 0xFF,
};

#define DATA_BROADCAST_ID_SSU					0x000A

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	Handle_t				 hAction;
	SvcSi_SiSpec_t			 unSiSpec;
	SvcSi_SwupCheckOption_t	 stSwupOption;
	SvcSi_LiveSwupOption_t	 stLiveSwupOption;

	HUINT32					 ulDemuxId;

	// IRDETO, HUMAX
	HUINT16					 usPmtSvcId;

	svcSi_FilteringType_b	 eFiltering;
	HxList_t				*pstFilterings;							// svcSi_FilteringInfo_t

	SiTxtEncodedType_t		 eTextEncType;							// Name 등의 Text의 Encoding Type

	HBOOL				bLiveSwupCheck;
} svcSi_SwupCheck_Base_t;

// OUI Structure :
typedef struct
{
	HUINT32		 ulOui;
	HUINT8		 ucUpdateType;
	HBOOL		 bUpdateVerFlag;
	HUINT8		 ucUpdateVer;
	HUINT32		 ulSelectorLen;
	HUINT8		*pucSelectSelectorPnt;
} svcSi_SwupCheck_SsuInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/

extern HERROR 				svc_si_SwupCheckInitData(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, Handle_t hAction, SvcSi_SiSpec_t *punSiSpec, void *pvSwupCheckOption, HBOOL bLiveSwupCheck);
extern HERROR 				svc_si_SwupCheckStartFilter(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, SIxTable_Type_e eTableType, HUINT32 ulExtId, HUINT32 ulPid);
extern HERROR 				svc_si_SwupCheckStopSection(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, SIxTable_Type_e eSiTableType, HUINT32 ulExtId);
extern svcSi_FilteringInfo_t 	*svc_si_SwupCheckFindFilterInfoByFilterId(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, HUINT32 ulFilterId);
extern HBOOL 					svc_si_SwupCheckCheckAllSiTableReceived(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData);

extern HERROR 				svc_si_SwupCheckSetCheckParam(SvcSi_SwupCheckSpec_t *pstSwupSpec, SvcSi_SwupFlag_t *pstSwupCheckParam);
extern HERROR 				svc_si_SwupCheckGetData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupSpec, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 				svc_si_SwupCheckFreeData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupCheckSpec, HUINT32 ulDataType, void *pvDataArray);

extern HERROR 				svc_si_SwupCheckGetSsuInfoFromSsuLinkDes (SIxLinkDes_t *pstLinkDes, HUINT32 *pulSelectorNum, svcSi_SwupCheck_SsuInfo_t **ppstSelector);
extern HERROR 				svc_si_SwupCheckGetSsuInfoFromDataBroadcastIdDes (SIxDataBroadcastIdDes_t *pstDbcIdDes, HUINT32 *pulSelectorNum, svcSi_SwupCheck_SsuInfo_t **ppstSelector);
extern HERROR 				svc_si_SwupCheckFreeSsuInfo (svcSi_SwupCheck_SsuInfo_t *pstSelector);
extern HERROR 				svc_si_SwupCheckConvertSwupSiDataTypeToVersionType (SvcSi_SwupImageType_e eSwupImageType, VersionType_t *peVerType);

extern HERROR 				svc_si_SwupCheckIsSsuStreamType(HUINT8 ucStreamType);
extern HERROR 				svc_si_SwupCheckFindProperOuiDataFromLinkDes(SIxLinkDes_t *pstLinkDes);
extern HERROR 				svc_si_SwupCheckFindLinkage(svcSi_SwupCheck_Base_t *pstSwupCheckBaseData, SIxTable_Type_e eSiType, HUINT8 ucLinkType, HUINT32 *pulNumLink, DbSvc_TripleId_t **pastTripleIdArray);

#endif	// ___INT_SVC_SI_OTA_CHECK_H___
/* end of file */

