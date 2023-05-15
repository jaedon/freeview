/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_si_hbbtv.h
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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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

#ifndef ___INT_SVC_SI_HBBTV_H___
#define ___INT_SVC_SI_HBBTV_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_si_filtering.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	SVCSI_HBBTV_APPLICATION_TYPE		0x10	/* Application type�� 0x10�� AIT���� �����Ѵ�. */

#define MAX_DEFERRED_SVC					8
#define MAX_DEFFERED_TAG					8

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32					 ulNumTags;
	HUINT16					 ausAssociationTags[MAX_DEFFERED_TAG];

	HBOOL					 bReceived;

	HUINT16					 usOnId;
	HUINT16					 usTsId;
	HUINT16					 usSvcId;
	HUINT16					 usPmtPid;
	HUINT32					 ulFilterId;
} svcSi_HbbtvDeferred_t;

typedef struct
{
	Handle_t				 hAction;

	HUINT16					 usOnId;
	HUINT16					 usTsId;
	HUINT16					 usSvcId;

	HUINT32					 ulDemuxId;

	HUINT16					 usPmtSvcId;
	HUINT16					 usPmtPid;
	HUINT16					 usAitPid;

	HUINT32					 ulDeferredNum;
	svcSi_HbbtvDeferred_t	 astDeffered[MAX_DEFERRED_SVC];

	// To check PAT whether it comes from the current TP or not:
	HUINT32					 ulPatTimeTick;

	svcSi_FilteringType_b	 eTableFiltering;

	// Filtering ������ ���� �ø��� �Ǵ��� ���� ����.
	HUINT32					 aulTimeoutFilterId[eSIxTABLETYPE_MAX];
	HUINT32					 aulCrc32[eSIxTABLETYPE_MAX];

	SiTxtEncodedType_t		 eTextEncType;						// Name ���� Text�� Encoding Type
} svcSi_HbbtvData_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/
extern HERROR 	svc_si_HbbtvGetAitSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount);
extern HERROR	svc_si_HbbtvSetHbbtvData (svcSi_HbbtvData_t *pstHbbtv, Handle_t hAction, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId);
extern HERROR	svc_si_HbbtvStartSection (svcSi_HbbtvData_t *pstHbbtv, svcSi_FilteringType_b eFiltering);
extern HERROR	svc_si_HbbtvStopSection (svcSi_HbbtvData_t *pstHbbtv);

#endif	// ___INT_SVC_SI_HBBTV_H___

/* end of file */
