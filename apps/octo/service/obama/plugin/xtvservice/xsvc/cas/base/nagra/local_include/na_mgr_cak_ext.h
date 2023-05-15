/*
@file				na_mgr_cak_ext.h
@brief				na_mgr_cak_ext.h (Nagra CAK Service)

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef _CA_CAK_EXT_H
#define _CA_CAK_EXT_H

#include "ca_cak.h"


/********************************************************************
  Structure and Type Definition
 ********************************************************************/
typedef enum tagTRightType
{
	ePre_Paid,
	eSubscription
} TRightType;

typedef struct tagNaSmartcardStatus
{
  	TIccSessionId		iccSessionId;
  	TBoolean			changed;
  	TSmartcardState		alarm;
} NaSmartcardStatus_t;

typedef struct tagNaAccessAlarm
{
 	TPid				elementaryStreamPid;
  	TDvbServiceId		serviceId;
  	TTransportSessionId	transportSessionId;
  	TBoolean			changed;
  	TCaAccess			alarm;
  	TIccSessionId		iccSessionId;
} NaAccessAlarm_t;

typedef struct tagNaChipsetInfo
{
	TChar	*pcNuid;
	TChar	*pcChipsetType;
	TChar	*pcChipsetRev;
} NaChipsetInfo_t;

typedef TCaStatus (*TAccessAlarmCallback)(TUnsignedInt8 xNumberOfSmartcardAlarms,
 	 										const NaSmartcardStatus_t *pxSmartcardAlarmArray,
        										TUnsignedInt8 xNumberOfAccesAlarms,
	 									 	const NaAccessAlarm_t *pxAccessAlarmArray);
typedef void (*TSmartcardAlarmCallback)(TUnsignedInt8 ucSlotId, TSmartcardState eScAlarm);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
typedef void (* TCopInfoCallback)(TTransportSessionId xTransportSessionId, TPid xPid, /*TCopyProtectionData*/TUnsignedInt8 xCopyProtectionData[2]);
#endif

/********************************************************************
  Extern Function Declaration
 ********************************************************************/
 // for NA CAK Processing APIs
extern HERROR CA_NA_CakInitialize(void);

extern HERROR CA_NA_CakRegisterAlarmExportation(TAccessAlarmCallback callback);
extern HERROR CA_NA_CakRegisterSystemInfoCallback(TSystemInfoCallback callback);
extern HERROR CA_NA_CakRegisterLocInfoCallback(TLocationInfoCallback callback);
extern HERROR CA_NA_CakRegisterScAlarmCallback(TSmartcardAlarmCallback callback);

extern HERROR CA_NA_CakRequest(TUnsignedInt32 ulReqId);
extern HERROR CA_NA_CakGetSystemInfo(TSystemInfoStatus *xStatus, TIrdInfo **pxIrdInfo,
 											TUnsignedInt8 *xNumberOfSmartcards, TSmartcardInfoItem **pxSmartcardInfoArray);
extern HERROR CA_NA_CakGetChipsetInfo(NaChipsetInfo_t** pxChipsetInfo);
extern HERROR CA_NA_CakGetAccessStatus(TCaAccess *accessStatus);

extern HERROR CA_NA_CakGetLocInfo(TLocationInfoStatus *xStatus, TUnsignedInt8 *xNumberOfSmartcards, TLocationInfoItem **pxLocationInfoArray);
extern HERROR CA_NA_CakCheckPincode(TPinIndex	xPinIndex,   TPinCode xPinCode, TUnsignedInt8 *pucPinCode, TPinCodeStatus *xStatus);
extern HERROR CA_NA_CakChangePincode(TPinIndex xPinIndex, TPinCode xOldPinCode, TPinCode xNewPinCode, TUnsignedInt8 *pucOldPinCode, TUnsignedInt8 *pucNewPinCode, TPinCodeStatus *xStatus);
extern HERROR CA_NA_CakSameCasIdInSc(TUnsignedInt8 ucSlotId, TUnsignedInt16 usCaSysId);
extern HERROR CA_NA_CakGetZipCode(TUnsignedInt8 ucSlotId, TUnsignedInt32 *pulZipCode);


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
extern HERROR CA_NA_GetValidityHdProduct(TRightType eType, TUnsignedInt32 ucDscLen, TUnsignedInt8 *pucDesc, TBoolean *pbExpired, TCaDuration *ulDuration);
extern HERROR CA_NA_GetScExpirationDate(TUnsignedInt8 ucSlotId, TBoolean *pbEndOfTime, TCalendarTime *pstDataTime);
extern HERROR CA_NA_GetScExpirationDuration(TCaDuration *expireDuraion);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
extern HERROR CA_NA_CopRegisterFunc(TCopInfoCallback pfnCopCallback);
extern HERROR CA_NA_CopUnregisterFunc(void);
extern HERROR CA_NA_CopReceivedData(TTransportSessionId xTransportSessionId, /*TCopyProtectionData*/TUnsignedInt8 xCopyProtectionData[2]);
#endif

// for NA CAK - extension
extern HERROR CA_NA_CakGetGmtTimeAsCalenderTime(TCalendarTime *pstCalendar);

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
extern HERROR CA_NA_Cak7_EnablingConsumptionForced(TBoolean bOneShot);
#endif
extern HERROR CA_NA_CakGetProviderId(TUnsignedInt8 ucSlotId, TPpid *pProviderId);
extern HERROR CA_NA_CakGetEcmCaSystemId(TUnsignedInt8 ucSlotId, TUnsignedInt16 *usCaSysId);

extern HERROR CA_NA_CakResetSysInfoTigger(void);
extern HBOOL CA_NA_CakIsOccuredSysInfoTigger(void);
extern HERROR CA_NA_CakResetSmarcardInfoTigger(void);
extern HBOOL CA_NA_CakIsOccuredSmarcardInfoTigger(void);

#endif /* _CA_CAK_EXT_H */

