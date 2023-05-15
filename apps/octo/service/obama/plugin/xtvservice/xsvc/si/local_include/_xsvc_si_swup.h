/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_si_swup.h
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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

#ifndef ___XSVC_SI_OTA_H___
#define ___XSVC_SI_OTA_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_si.h>
#include <_svc_si_swup.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SSU_BOUQUET_ID					0xFF00

#if 0
// =====     ASTRA OTA   ===== //
// Bouquet IDs
#define OTA_ASTRA_BOUQUET_ID					0xFF00

// Linkage Descriptor Types
#define OTA_ASTRA_REF_NIT_LINKAGE				0x04
#define OTA_ASTRA_SSU_SCAN_LINKAGE				0x0A
#define OTA_ASTRA_SSU_LINKAGE					0x09

#define DATA_BROADCAST_ID_SSU					0x000A
#endif

// =====	 UK DTT OTA	 ===== //
#define MIS_TAG 								0xE0  // manufacturer_information_tag. see D-BOOK, page 24-4, Table 24-4
#define SYSTEMID_TAG							0x80	// System ID descriptor
#define HUMAX_DOWNLOAD_ID						0x000378  // IEEE OUI ID for Humax. 24-bit value..

#define DATA_BROADCAST_ID_UK					0x0111

// =====	 FOR UNDER 16 BYTE_FILTER	 ===== //
#define MAX_RETRY_GET_SECTION					10


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
extern HUINT32			xsvc_si_SwupCheckCvtVersionTypeToPalParam(VersionType_t);

#if defined(CONFIG_OTA_HUMAX)
extern SvcSi_ActFunc	xsvc_si_SwupCheckGetActionFunc_Humax (void);
extern void*			xsvc_si_SwupCheckAllocData_Humax (void);
extern void				xsvc_si_SwupCheckFreeData_Humax (void *pvData);
extern HERROR 			xsvc_si_SwupCheckGetSwupCheckData_Humax(Handle_t hAction, XsvcSi_SwupDataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 			xsvc_si_SwupCheckFreeSwupCheckData_Humax (Handle_t hAction, XsvcSi_SwupDataType_e eDataType, void *pvDataArray);
#endif

#if defined(CONFIG_OTA_IRDETO)
extern SvcSi_ActFunc	xsvc_si_SwupCheckGetActionFunc_Irdeto (void);
extern void*			xsvc_si_SwupCheckAllocData_Irdeto (void);
extern void				xsvc_si_SwupCheckFreeData_Irdeto (void *pvData);
extern HERROR 			xsvc_si_SwupCheckFlag_Irdeto (Handle_t hAction, HUINT32 *pulSwupCheckFlag);
extern HERROR 			xsvc_si_SwupCheckSetSwupCheckParam_Irdeto (SvcSi_SwupFlag_t *pstSwupCheckParam);
extern HERROR 			xsvc_si_SwupCheckGetSwupCheckData_Irdeto(Handle_t hAction, XsvcSi_SwupDataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 			xsvc_si_SwupCheckFreeSwupCheckData_Irdeto (Handle_t hAction, XsvcSi_SwupDataType_e eDataType, void *pvDataArray);
#if defined(CONFIG_OTA_IRDETO_LOADER_V5)
extern HERROR 			xsvc_si_SwupCheckMakeUntExtIdByOui_Irdeto(HUINT16	*pusExtId);
#endif
#endif

#if defined(CONFIG_OTA_SES)
extern SvcSi_ActFunc 	xsvc_si_SwupCheckGetActionFunc_Ses (void);
extern void *			xsvc_si_SwupCheckAllocData_Ses (void);
extern void 			xsvc_si_SwupCheckFreeData_Ses (void *pvData);
extern HERROR 			xsvc_si_SwupCheckGetSwupCheckData_Ses(Handle_t hAction, XsvcSi_SwupDataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 			xsvc_si_SwupCheckFreeSwupCheckData_Ses (Handle_t hAction, XsvcSi_SwupDataType_e eDataType, void *pvDataArray);
extern HERROR 			xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Ses (SIxLinkDes_t *pstLinkDes);
#endif

#if defined(CONFIG_OTA_ASTRA)
extern SvcSi_ActFunc 	xsvc_si_SwupCheckGetActionFunc_Astra (void);
extern void *			xsvc_si_SwupCheckAllocData_Astra (void);
extern void 			xsvc_si_SwupCheckFreeData_Astra (void *pvData);
extern HERROR 			xsvc_si_SwupCheckGetSwupCheckData_Astra(Handle_t hAction, XsvcSi_SwupDataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 			xsvc_si_SwupCheckFreeSwupCheckData_Astra (Handle_t hAction, XsvcSi_SwupDataType_e eDataType, void *pvDataArray);
extern HERROR 			xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Astra (SIxLinkDes_t *pstLinkDes);
#endif


extern HERROR			xsvc_si_SwupCheckGetSwupDownType(HUINT8 *pucDownType);
extern HERROR			xsvc_si_SwupCheckSetSwupDownType(HUINT8 ucDownType);
extern void				xsvc_si_SwupCheckMsgEvtNitTable(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg);
extern HERROR			xsvc_si_SwupCheckGetFilteringTypes(xsvcSi_SwupData_t *pstSwupCheck);
extern HERROR			xsvc_si_SwupCheckMakeUntExtIdByOui(HUINT16 *pusUntExtId);


#if defined(CONFIG_OTA_DVBSSU)
extern SvcSi_ActFunc 	xsvc_si_SwupCheckGetActionFunc_Dvbssu (void);
extern void *			xsvc_si_SwupCheckAllocData_Dvbssu (void);
extern void 			xsvc_si_SwupCheckFreeData_Dvbssu (void *pvData);
extern HERROR 			xsvc_si_SwupCheckGetSwupCheckData_Dvbssu(Handle_t hAction, XsvcSi_SwupDataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 			xsvc_si_SwupCheckFreeSwupCheckData_Dvbssu (Handle_t hAction, XsvcSi_SwupDataType_e eDataType, void *pvDataArray);
extern HERROR 			xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Dvbssu(SIxLinkDes_t *pstLinkDes);
extern HERROR			xsvc_si_SwupCheckGetFilteringTypes_Dvbssu(xsvcSi_SwupData_t *pstLiveSwupCheck);
extern void 			xsvc_si_SwupCheckMsgEvtNitTable_Dvbssu (xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg);
extern HERROR 			xsvc_si_SwupCheckMakeUntExtIdByOui_Dvbssu(HUINT16	*pusExtId);


#if defined(CONFIG_OTA_LIVE_ENABLE)
// OTA Live
extern SvcSi_ActFunc 	xsvc_si_LiveSwupCheckGetActionFunc_Dvbssu (void);
extern void*			xsvc_si_LiveSwupCheckAllocData_Dvbssu (void);
extern void 			xsvc_si_LiveSwupCheckFreeData_Dvbssu (void *pvData);
#endif

#if defined(CONFIG_OP_ERTELECOM)
extern HERROR			xsvc_si_SwupCheckGetSwupDownType_Ertelecom(HUINT8 *pucDownType);
extern HERROR			xsvc_si_SwupCheckSetSwupDownType_Ertelecom(HUINT8 ucDownType);
extern void			xsvc_si_SwupCheckMsgEvtNitTable_Ertelecom(xsvcSi_SwupData_t *pstLiveSwupCheck, SvcSi_Msg_t *pstMsg);
extern HERROR			xsvc_si_SwupCheckGetFilteringType_Ertelecom(xsvcSi_SwupData_t *pstLiveSwupCheck);
extern HERROR 			xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_Ertelecom(SIxLinkDes_t *pstLinkDes);
#endif
#if defined(CONFIG_OP_UK_DTT)
extern HERROR 		xsvc_si_SwupCheckGetFilteringTypes_UkDtt(xsvcSi_SwupData_t *pstSwupCheck);
extern void 			xsvc_si_SwupCheckMsgEvtNitTable_UkDtt(xsvcSi_SwupData_t *pstSwupCheck, SvcSi_Msg_t *pstMsg);
extern HERROR 		xsvc_si_SwupCheckMakeUntExtIdByOui_UkDtt(HUINT16	*pusExtId);
#endif
#endif


#if defined(CONFIG_OTA_DTT_EU)
extern SvcSi_ActFunc 	xsvc_si_SwupCheckGetActionFunc_EuDtt (void);
extern void *			xsvc_si_SwupCheckAllocData_EuDtt (void);
extern void 			xsvc_si_SwupCheckFreeData_EuDtt (void *pvData);
extern HERROR 			xsvc_si_SwupCheckGetSwupCheckData_EuDtt(Handle_t hAction, XsvcSi_SwupDataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
extern HERROR 			xsvc_si_SwupCheckFreeSwupCheckData_EuDtt (Handle_t hAction, XsvcSi_SwupDataType_e eDataType, void *pvDataArray);
extern HERROR 			xsvc_si_SwupCheckFindProperOuiDataFromLinkDes_EuDtt(SIxLinkDes_t *pstLinkDes);
#endif

#endif // ___XSVC_SI_OTA_H___

/* end of file */
