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

#ifndef ___IRUC_DRV_HEADER___
#define ___IRUC_DRV_HEADER___

#include "UniversalClient_Common_API.h"
#include <ir_api.h>

#define IRUC_HUMAX_MAX_SERVICE 	(IR_HUMAX_MAX_SERVICE)
#define IRUC_HUMAX_MAX_TP (IR_HUMAX_MAX_TP)

// TODO: Internal HDD Model의 경우 1LIVE + 1 TSR + 4 REC를 할때는 다시 고민 해야 된다.
#define IRUC_SVC_MAX_PID 	(IR_SVC_MAX_PID)

#define IRUC_INVALID_UC_SVC_CONTEXT	 (0xFFFFFFFE)
#define IRUC_INVALID_PID 		(0x2000)

#define IRUC_INVALID_UC_SVCHANDLE			(0xFFFFFFFF) // it's different from softcell handle type, then 4 bytes sizes' required.
#define IRUC_INVALID_TS_UNIQUE_ID			(0xFFFF)

#define IRUC_EMM_PREFIX			(0x01)
#define IRUC_ECM_PREFIX			(0x02)

#define IRUC_SECURE_CORE_LENGTH			256

#define CLIENTSERIALNUMBER	13
#define HASHCODENUMBER	20
#define CA_MAX_LENGTH_CLIENT_SERIAL_NUMBER 11

#define IRUC_VIEW_ACTION 				(1)
#define IRUC_REC_ACTION 				(2)

#define UC_BYTES_TMS_SIZE				32

typedef struct
{
	HUINT16		usPID;
	HUINT8		ucTypeFromPMT;
	HUINT8		ucTypeFromPlay;
	HUINT16		usEmmorEcmPID;
	HBOOL		ucAllocatedDsc;
}IRUC_PID_INFO;


typedef struct
{
	//It represents SvcContext in Universal Client.
	HUINT32	UcSvcContext; // consist of 4 bytes ( prefix(EMM:1, ECM:2)/00/00/action id )

	Handle_t			hIrSvc; // real identifer in irdeo module.
	Handle_t			hAct; //action handle
	Handle_t			hSvc; //service handle
	SvcCas_ActionType_e hActionType; // action type rec, tsr, live
	HUINT32				ucIsSelected;  // allocate instance
	HUINT16				usTsUniqueId;
	HUINT32				ulDemuxId;

	HUINT8				IsRunning;
	HUINT8				IsMonitoring;

	uc_service_handle	UC_SvcHandle;

	HUINT8				IsSentSvcDefine;

	IRUC_PID_INFO		PidList[IRUC_SVC_MAX_PID];
	HUINT8				ucStreamCount;

	HUINT8				*pPmtRaw;
	HUINT8				IsValidPmt;

	HUINT8 	szEcmStatusMsg[256];
	HUINT8 	szSvcStatusMsg[256];

// hjlee3,
//    convergent_service_monitor_info cloakedCAMonitorInfo;
//    convergent_service_monitor_info softcellMonitorInfo;
	HBOOL				bCCA; // true : CCA
	HUINT8				svcType;
	HUINT32				ulStreamCount;
	HUINT8				ucHashCodevalue[CLIENTSERIALNUMBER];
	uc_convergent_client_service_monitor_status_st	UCmonitorInfo;
	uc_service_stream_status_st		*UCStreamStatusInfo;

	HUINT8 ulattributeCount; 
	uc_tms_attribute_item_info pTmsAttributesItemInfoList[2];

} IRUC_SVC_INFO;

typedef struct
{
	//It represents SvcContext in Universal Client.
	HUINT32	UcSvcContext; // consist of 4 bytes ( prefix(EMM:1, ECM:2)/00/00/action id )
	uc_service_handle	UC_SvcHandle;

	Handle_t	hAction; //for emm filter.
	HUINT8	ucActId;
	HUINT32	ulDemuxId;
	HUINT16	usTsUniqueId;
	HUINT16	onId;
	HUINT8	IsValidCat;
	HUINT8	*pCatRaw;
	HUINT32	nCatSize; //it includes section tb id, & length element item.

	HUINT8	IsMonitoring;
	HUINT8	szSvcStatusMsg[256];
	HUINT8 	szEmmStatusMsg[256];
	HBOOL	ucUsed;

// hjlee3,
//  convergent_service_monitor_info cloakedCAMonitorInfo;
//  convergent_service_monitor_info softcellMonitorInfo;
	HUINT8				svcType;
	HUINT16  			emmPid;
	HUINT32				ulStreamCount;
	uc_service_handle	UC_SoftCellHandle;
	uc_service_stream_status_st	*UCStreamStatusInfo;
	uc_convergent_client_service_monitor_status_st	UCmonitorInfo;

	HUINT8				secureCoreListStatus[IRUC_SECURE_CORE_LENGTH];
	HUINT8				packagesDownloadProgressInfo[IRUC_SECURE_CORE_LENGTH]; //  IRUC_SVC_SECURE_CORE
} IRUC_TP_INFO;


/*******************************************************************/
/********************      Functions     ********************************/
/*******************************************************************/
HUINT8		CAS_IRUC_GetIrSvcInfoIDXbyActHandle(Handle_t hAction);	// ewlee 20100217

HBOOL		CAS_IRUC_CheckDescramblePid(HUINT32 SvcContext, HUINT16 ulPid);
HBOOL 		CAS_IRUC_SetOtherDescramblePid(HUINT32 SvcContext, HUINT16 ulPid, HUINT8 *cComponetCnt, 	HUINT32 *ulDscIndex, HUINT16 *usOtherPidList);
HBOOL 		CAS_IRUC_CheckAllocateDscOtherPid(HUINT32 SvcContext, HUINT16 ulPid);
HBOOL 		CAS_IRUC_ResetOtherDescramblePid(HUINT32 SvcContext, HUINT16 ulPid);
HINT32		CAS_IRUC_GetActHandDmxIDbySvcContext(HUINT32 SvcContext, Handle_t *pActHOut, HUINT32 *pulDmxIdOut);
void 		CAS_IRUC_DRV_Init(void);
void 		CAS_IRUC_Client_Init(void);
HINT32 		CAS_IRUC_API_GetSmartcardStatus(HUINT32 smartcardId,uc_smartcard_status	*stBuffScStatus);
HINT32 		CAS_IRUC_API_GetSmartcardInfo(HUINT32 smartcardId,uc_smartcard_info	*stBuffScInfo);

HIR_ERR	xsvc_cas_IrUcSvcStatusGetNumberOfServices(IR_SVC_STATUS_INFO_MAIN **stSvcStatusMain, HUINT8 *pucNumberOfServices);
#endif //___IRUC_DRV_HEADER___

