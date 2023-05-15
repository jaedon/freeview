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
	@file	   \n
	@brief	   \n

	Description:   \n
	Module: MW/CAS/irdeto/ir_uc/include \n

	Copyright (c) 2009 Jeong-il
	All rights reserved. \n
*/

#ifndef	__IRUC_API_H__
#define	__IRUC_API_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "UniversalClient_Common_API.h"
#include "UniversalClient_API.h"
#include "iruc_drv.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	/* DVB services. */
	SERVICE_DVB_DESCRAMBLE        = 1,
	SERVICE_DVB_EMM               = 2,
	/* IPTV services. */
	SERVICE_IPTV_REGISTRATION_EMM = 3,
	SERVICE_IPTV_DESCRAMBLE_VOD   = 4,
	/* CypherCast services. */
	SERVICE_CC_DESCRAMBLER        = 5,
	SERVICE_CC_OPERATOR           = 6,
	/* PVR services. */
	SERVICE_PVR_RECORD            = 7,
	SERVICE_PVR_PLAYBACK          = 8,
	SERVICE_PVR_DRM               = 9,
	/* IPDC services. */
	SERVICE_IPDC_DESCRAMBLE       = 10,
	SERVICE_IPDC_EMM              = 11,
	/* DMB services. */
	SERVICE_DMB_DESCRAMBLE        = 12,
	SERVICE_DMB_EMM               = 13,
	/* CMMB services. */
	SERVICE_CMMB_DESCRAMBLE       = 14,
	SERVICE_CMMB_EMM              = 15,
	/* FLO services. */
	SERVICE_FLO_DESCRAMBLE        = 16,
	SERVICE_FLO_EMM               = 17
} IR_UC_SVC;

typedef enum
{
	eIRUC_STATUS_Info_ECM	=	0,
	eIRUC_STATUS_Info_CCA,
	eIRUC_STATUS_Info_SOFTCELL,
	eIRUC_STATUS_Info_MAX,

} IRUC_SVC_STATUS_INFO_e;

typedef struct IRUC_STREAM_STATUS_s
{
	HUINT8 		aucStatusString[UC_CONNECTION_STATUS_STRING_SIZE];
	HUINT16		EcmEmmPid;
	HUINT8		ucComponentCnt;
	HUINT16		EsPidList[IRUC_SVC_MAX_PID];
} IRUC_STREAM_STATUS;

typedef struct
{
	HBOOL bValid;
	HBOOL bEcmType;
	HBOOL bCCA;		/* CCA와 SmartCard를 구별하기 위해 ... */

	uc_service_handle hUC_SVC;
	uc_service_handle hUC_internalSVC;
	HUINT8 aucSVCStatusString[UC_CONNECTION_STATUS_STRING_SIZE];
	Handle_t	hAction;

/* 	TODO: hjlee3, Add*/
	HUINT32		eSvcType;
	HUINT8		ucInstanceCount;
	HUINT32		eMonitorSvcType;
	uc_char 	aucMonitormessage[UC_MONITOR_MSG_LENGTH];
	HUINT8		ucStreamCnt;
	IRUC_STREAM_STATUS stStreamStatus[IRUC_SVC_MAX_PID];
	HBOOL 		bMonitoringValid;
}IRUC_SERVICE_STATUS;

typedef struct
{
	HUINT32	id;
	HUINT8	ucSectorNumber;
	HBOOL	bEntitled;
	HUINT8	aucProductID[UC_SIZE_PRODUCT_ID];
	HUINT32	ulStartDate;
	HUINT32	ulDuration;
	HUINT32 usProductType;
}IRUC_PRODUCT_STATUS;

typedef struct
{
	HUINT32	id;
	HUINT8	ucSectorNumber;
	HUINT8	ucRegion;
	HUINT8	ucSubRegion;
}IRUC_REGION_STATUS;

// ewlee 20100205 - client status
#define	IRUC_DEVICE_CSSN_LEN			(4)

/* UCCA version up이 되면서 define값이 변경됨. 호환성을 위해 아래와 같이 추가함. */
#ifndef UC_MAX_SMARTCARD_SERIAL_NUMBER_LENTH
#define UC_MAX_SMARTCARD_SERIAL_NUMBER_LENTH	UC_MAX_SMARTCARD_SERIAL_NUMBER_LENGTH
#endif

typedef struct
{
	HUINT8		ucTitle[UC_BYTES_GENERIC_SIZE];
	HUINT8		ucVersion[UC_BYTES_GENERIC_SIZE];
	HUINT8		ucCSSN[IRUC_DEVICE_CSSN_LEN];
	HUINT8		ucSerialNumber[UC_BYTES_GENERIC_SIZE];
	HUINT16		nLockID;
	HUINT32		nCASystemID;
	HUINT8		ucSecureType;	// If the value is 0, the secure tpye is "Secure Chipset". If the value is 1, the secure type is "Security ID".
	HUINT8		ucNationality[UC_BUFFER_GENERIC_SIZE]; /* Nationality from UniversalClient_GetNationality() */
	HUINT32		nEcmCount;
	HUINT32		nEmmCount;

//	HUINT8 		ucSecureClent[UC_SECURE_STATUS_LENGTH]; 		/* TODO ; hjlee3  */
	HUINT8 		ucSecureCoreVersion[UC_BYTES_GENERIC_SIZE]; 		/* TODO ; hjlee3  */

	/* smartcard info */
	HUINT8		ucSoftcellVersion[UC_BYTES_GENERIC_SIZE];
	HUINT8		ucBuildInfo[UC_BYTES_GENERIC_SIZE];
	HUINT8		ucSmartCardStatus[UC_STATUS_LENGTH];
	HUINT8		ucScserialNumber[UC_MAX_SMARTCARD_SERIAL_NUMBER_LENTH];
	HUINT16		nsmartcardType; 	/* Smartcard type. */
	HUINT8 		ucmajorVersion;  	/* Major version */
	HUINT8 		ucminorVersion; 	/* Minor version */
	HUINT8		ucSmartCardNationality[UC_BUFFER_GENERIC_SIZE]; /* Nationality from UniversalClient_GetSmartcardNationality() */
	HUINT32 	nSmartCardCASystemID; 					/* CA system ID in Smartcard from UniversalClient_GetSmartcardCASystemID()  */
	/* smartcard extend info */
	HUINT8 		ucbuild; 			/* Build infomation. */
	HUINT8 		ucvariant; 			/* Variant */
	HUINT16 	npatchLevel; 		/* Patch level */
	HUINT16 	nownerId;			/* Owner id. */
	HUINT8		ucTmsData[UC_BYTES_TMS_SIZE];
	HUINT8		ucCapabilities[UC_BYTES_GENERIC_SIZE];

	/* UC extend info */


	HUINT8 		ucClientID[UC_BUFFER_GENERIC_SIZE]; 			/* Client ID */
	HUINT8 		ucSecureCoreLoadStatus[IRUC_SECURE_CORE_LENGTH]; /* SecureCore status*/
	HUINT8 		packagesDownloadProgressInfo[IRUC_SECURE_CORE_LENGTH]; 		/* CCA package download progress bar  */
	HUINT8		ucCCABuildInfo[UC_BYTES_GENERIC_SIZE];
}IRUC_CLIENT_STATUS;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HINT32 		CAS_IRUC_API_SVC_GetStatusService(HUINT8 *pucTotalUcSvcCnt, IRUC_SERVICE_STATUS **ppSvcStatus_shallbefree );
HINT32 		CAS_IRUC_API_GetSecureID(HUINT8 *pSecureID);
HINT32 		CAS_IRUC_API_GetStatusProduct(HUINT32 *pulProductCnt, IRUC_PRODUCT_STATUS **ppstProductstatus_shallbeFree  );
HINT32 		CAS_IRUC_API_GetStatusClient(IRUC_CLIENT_STATUS *pClientStatus, HUINT32 *tmsDataLen);
HINT32 		xsvc_cas_IrUcGetStatusRegion(HUINT32 *pulRegionCnt, IRUC_REGION_STATUS **ppstRegionstatus_shallbeFree  );
HINT32		xsvc_cas_IrUcGetStatusClient(IRUC_CLIENT_STATUS *pClientStatus, HUINT32 *tmsDataLen);
HINT32		xsvc_cas_IrUcGetClientID(HUINT8 *poutClientID, HUINT32 uiLength);
HUINT32		xsvc_cas_iruc_API_CopySerialNum(HUINT8 ucSlotID, HUINT8 *pData);
HINT32		xsvc_cas_IrUcGetStatusService(HUINT8 *pucTotalUcSvcCnt, IRUC_SERVICE_STATUS **ppSvcStatus_shallbefree );
HINT32 		xsvc_cas_IrUcGetEachStatusService(HUINT32 eSvcType, HUINT16 usHandle, IRUC_SERVICE_STATUS **ppEachSvcStatus_shallbefree, IRUC_SVC_STATUS_INFO_e eGetType);
void 		xsvc_cas_IrUcInitStreamStatus(void);
void 		xsvc_cas_IrUcFreeStreamStatus(void);
HINT32 		xsvc_cas_IrUcGetStatusProduct(HUINT32 *pulProductCnt, IRUC_PRODUCT_STATUS **ppstProductstatus_shallbeFree );
HINT32		xsvc_cas_IrUcStopMonitoring(void);
HINT32		xsvc_cas_IrUcStartMonitoring(HUINT16 ushSCELL, HUINT8 ucType);
HINT32 		xsvc_cas_IrUcSetConfigService(uc_service_handle  serviceHandle, HUINT32  TLVLength,  HUINT8 *pTLV);
HIR_ERR 		xsvc_cas_IrUcSvcStatusGetInformationOfEachService(HUINT32 eSvcType, HUINT16 usHandle, IRUC_SERVICE_STATUS *stSvcStatusEach, IRUC_SVC_STATUS_INFO_e eGetType);
#endif /* !__IRUC_API_H__ */

