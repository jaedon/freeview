/*
@file				na_mgr_cak_irdcmd.h
@brief				na_mgr_cak_irdcmd.h (Nagra CAK Service[IRD-COMMAND])

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef _CA_CAK_IRDCMD_H
#define _CA_CAK_IRDCMD_H

#include "ca_cak.h"

#define NA_IRDCMD_MAX_PROFILE				5
#define NA_IRDCMD_MAX_MSG_LEN				50
#define NA_IRDCMD_MAX_PINCODE_LEN			16

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
#define INVALID_MAIL_ID						0xFFFF
#define INAVLID_HANDLE						0xFFFFFFFF

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
#define MAX_MAIL_LENG_PER_SEGMENT			93
#else
#define MAX_MAIL_LENG_PER_SEGMENT			45
#endif
#define MAX_MAIL_SEGMENT_NUM				63
#define MAX_MAIL_LENGTH						(MAX_MAIL_LENG_PER_SEGMENT * MAX_MAIL_SEGMENT_NUM)
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
#define INVALID_POPUP_ID					0xFFFF
#define MAX_POPUP_LENGTH					2835
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
#define MAX_ZONE_ID_LENGTH		6
#define MAX_ACCOUNT_ID_LENGTH		6

#define MAX_COUNTRY_CODE_LENGTH		3
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
#define MAX_WAKING_SLOT						8
#endif

#define EMM_IRD_OPCODE_MASK					(0x3F)
#define EMM_IRD_ZONEID_MASK					(0x80)

/********************************************************************
  Structure and Type Definition
 ********************************************************************/
typedef enum tagNaIrdCmdOpCode
{
	eOpCodeInit 						=	0x00,
	eOpCodeDefault						=	0x01,
	eOpCodeMasterSlaveInit				=	0x01,
	eOpCodeMasterSlaveCancel			=	0x02,
	eOpCodeMasterSlaveSingleShot 		=	0x03,
	eOpCodeCountryCode 				=	0x03,
	eOpCodeDisplayProfile				= 	0x02,
	eOpCodeMessage						= 	0x03,
	eOpCodeMultiscreen						= 	0x04,
	eOpCodeMax							=	0xff
} NA_IRDCMD_OPCODE_t;

typedef enum tagNaIrdCmdId
{
	eComIDResetPincode					=	0x12,
	eComIDMail 							=	0xC0,
	eComIDForceTune						=	0xC1,
	eComIDForceIdentification 			=	0xC2,
	eComIDSetMacrovisionCps				=	0xC4,
	eComIDConfigureStb					=	0xC5,
	eComIDSetNetworkID					=	0xC6,
	eComIDMasterSlave 					=	0xC7,
	eComIDSetPincode					=	0xC8,
	eComIDSetForceStandby				=	0xC9,
	eComIDSetForceDefSetting			=	0xCC,
	eComIDDisplayPopUp					=	0xCF,
	eComIDChangeUsageID					=	0xD3,
	eComIDLogicalDiskFormat				=	0xD5,
	eComIDZoneID						= 	0xD8,
	eComIDEmmWakeUp						= 	0xD9,
	eComIDRebootStb						=	0xDB,
	eComIDApsHdPlus						= 	0x53,
	eComIDUnknown						=	0xff
} NA_IRDCMD_ID_t;

typedef enum tagNaIrdCmdProfileId
{
	eProfileSmartcard = 1,
	eProfileProducts
} NA_IRDCMD_PROFILE_ID_t;

typedef enum tagNaIrdCmdFrequency
{
	eFreqOnce = 0,
	eFreqDaily,
	eFreqAlways
} NA_IRDCMD_FREQUENCY_t;

typedef enum tagNaIrdMsgType
{
	eMsgHotlineNum = 1,
	eMsgCallCharge
} NA_IRDCMD_MSG_TYPE_t;

typedef struct tagNaIrdCmdProfile
{
	TUnsignedInt8	ucProfieType;		// 1 - smartcard, 2 - products
	TUnsignedInt8	ucPeroid[NA_IRDCMD_MAX_PROFILE];
	TUnsignedInt8	ucBeginOfPeroid[NA_IRDCMD_MAX_PROFILE];
	TUnsignedInt8	ucEndOfPeroid[NA_IRDCMD_MAX_PROFILE];
	TUnsignedInt8	ucFrequency[NA_IRDCMD_MAX_PROFILE];
 } NA_IRDCMD_PROFILE_t;

typedef struct tagNaIrdCmdMessage
{
	TUnsignedInt8	ucMsgType;			// 1 - hotline num, 2 - call charge
	TUnsignedInt8	ucMsgContent[NA_IRDCMD_MAX_MSG_LEN];
} NA_IRDCMD_MESSAGE_t;

typedef void (*TIrdCmdCallbackFunc)(NA_IRDCMD_ID_t eCmdId, NA_IRDCMD_OPCODE_t eCmdOpCode, TUnsignedInt32 ulParam1, TUnsignedInt32 ulParam2);


#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
typedef enum
{
	eMailPriortyNormal		= 0,
	eMailPriortyHigh		= 1,
	eMailPriortyEmergency	= 2,
	eMailPriortyInvalid		= 3
} NA_MAIL_PRIORITY;

typedef enum
{
	eMsgStatusUpdating		= 0,
	eMsgStatusCompeleted	= 1
} NA_MAIL_MSG_STATUS;

typedef struct tagNaMailMessage
{
	TUnsignedInt16	nMailId;
	TUnsignedInt8	nPriority;
	TBoolean		bRead;
	TUnsignedInt32	hMsgSave;
	UNIXTIME		nSystime;
	TUnsignedInt32	nSize;
	TUnsignedInt32	nStatus;
	TUnsignedInt8	szText[MAX_MAIL_LENGTH];
} NA_MAIL_MSG;
#endif

typedef enum
{
	POPUP_PERSISTENCE_NORMAL		= 0,
	POPUP_PERSISTENCE_TIMEOUT		= 1,
	POPUP_PERSISTENCE_USER			= 2,
	POPUP_PERSISTENCE_INVALID		= 3
} NA_POPUP_PERSISTENCE;

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
typedef enum
{
	MSG_STATUS_UPDATING				= 0,
	MSG_STATUS_COMPLETED			= 1
} NA_POPUP_MSG_STATUS;

typedef struct tagPopupMessage
{
	TUnsignedInt16	nPopupId;
	TUnsignedInt8	nPersistence;
	UNIXTIME		nSystime;
	TUnsignedInt32	nStatus;
	TUnsignedInt32	nStrLen;
	TUnsignedInt8	szText[MAX_POPUP_LENGTH];
} NA_POPUP_MSG;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
typedef	struct tagEmmZoneIdInfo
{
	TUnsignedInt8	ucReceived;

	TUnsignedInt8 ucZoneId[MAX_ZONE_ID_LENGTH];
	TUnsignedInt32 nIpAddress;
	TUnsignedInt16 usPortNumber;
	TUnsignedInt8 ucAccountId[MAX_ACCOUNT_ID_LENGTH];
} NA_EMM_ZONEID_INFO;

typedef	struct tagEmmCountryCodeInfo
{
	TUnsignedInt8	ucReceived;

	TUnsignedInt8 ucCountryCode[MAX_COUNTRY_CODE_LENGTH];
	TUnsignedInt32 nIpAddress;
	TUnsignedInt16 usPortNumber;
	TUnsignedInt8 ucAccountId[MAX_ACCOUNT_ID_LENGTH];
} NA_EMM_COUNTRYCODE_INFO;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
typedef	struct tagWakingLoopSlot
{
	TUnsignedInt8	ucReserved : 5;
	TUnsignedInt8	ucNetFlag : 1;
	TUnsignedInt8	ucTsFlag : 1;
	TUnsignedInt8	ucSvcFlag : 1;

	TUnsignedInt16	usNetId;
	TUnsignedInt16	usTsId;
	TUnsignedInt16	usSvcId;
	TUnsignedInt16	usWakingDur;
} NA_WAKING_LOOP_SLOT;

typedef	struct tagEmmWakeupInfo
{
	TUnsignedInt8	ucReceived;

	TUnsignedInt8	ucWakingLoopCnt;
	NA_WAKING_LOOP_SLOT stWakingSlot[MAX_WAKING_SLOT];

	TUnsignedInt8	ucReserved : 6;
	TUnsignedInt8	ucWakeupTimeFlag : 1;
	TUnsignedInt8	ucSleepingDurFlag : 1;

	// not used
	TUnsignedInt8	ucWakeupYear;
	TUnsignedInt8	ucWakeupMonth;
	TUnsignedInt8	ucWakeupDay;
	TUnsignedInt8	ucWakeupHour;
	TUnsignedInt8	ucWakeupMin;

	TUnsignedInt16	usSleepingDur;
} NA_EMM_WAKEUP_INFO;
#endif


/********************************************************************
  Extern Function Declaration
 ********************************************************************/

// for NA IRD-COMMAD APIs
extern HERROR CA_NA_IRDCMD_InitIrdVar(void);
extern HERROR CA_NA_IRDCMD_ResetIRDCmdData(void);
extern HERROR CA_NA_IRDCMD_IsValidIRDCmd(TUnsignedInt8 *pxIrdCmd);
extern HERROR CA_NA_IRDCMD_ProcessSequenceNumber(NA_IRDCMD_ID_t nIrdCmdId, HUINT8 *pxIrdCmd);
extern HERROR CA_NA_IRDCMD_GetIRDCommand(TUnsignedInt8 *pxIrdCmd, NA_IRDCMD_ID_t *pIrdCmdId, NA_IRDCMD_OPCODE_t *pOperationCode);
extern HERROR CA_NA_IRDCMD_ProcessIRDCmd(TUnsignedInt8 *pxIrdCmd);
extern HERROR CA_NA_IRDCMD_RegisterCallback(TIrdCmdCallbackFunc pfnCallback);

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
extern HERROR CA_NA_IRDCMD_EMM_GetWakeupTime(UNIXTIME *pulTime);
extern HERROR CA_NA_IRDCMD_EMM_InitWakingTable(void);
extern HERROR CA_NA_IRDCMD_EMM_HasMoreNextWaking(TBoolean *pbNext);
extern HERROR CA_NA_IRDCMD_EMM_GetWakingTuneInfo(TUnsignedInt8 *pucNetworkIdFlag, TUnsignedInt16 *pusNetworkId,
																TUnsignedInt8 *pucTsIdFlag, TUnsignedInt16 *pusTsId,
																TUnsignedInt8 *pucServiceIdFlag, TUnsignedInt16 *pusServiceId);
extern HERROR CA_NA_IRDCMD_EMM_GetWakingTimer(UNIXTIME *pulTime);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
extern HERROR CA_NA_IRDCMD_ZoneId_GetID(TUnsignedInt8 *pZoneId, TUnsignedInt32 unMaxLen);
extern HERROR CA_NA_IRDCMD_CountryCode_GetID(TUnsignedInt8 *pCountryCode, TUnsignedInt32 unMaxLen);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
extern HERROR CA_NA_IRDCMD_MAIL_CheckUnreadMessage(void);
extern HERROR CA_NA_IRDCMD_MAIL_ClearMailMessageHeader(void);
extern HERROR CA_NA_IRDCMD_MAIL_MakeMailMessage(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16	*pnMailId, TBoolean *pfCompleted, TUnsignedInt16 *pnDeletedMailId);
extern HERROR CA_NA_IRDCMD_MAIL_LoadMailMessages(void);
extern HERROR CA_NA_IRDCMD_MAIL_GetMailPriority(TUnsignedInt16 nMailId, NA_MAIL_PRIORITY *pnPriority);
extern HERROR CA_NA_IRDCMD_MAIL_GetMailList(NA_MAIL_MSG ***pppMailList, TUnsignedInt32 *pnCount);
extern HERROR CA_NA_IRDCMD_MAIL_GetMailData(TUnsignedInt16 nMailId, NA_MAIL_MSG **ppMailData);
extern HERROR CA_NA_IRDCMD_MAIL_DeleteMailMessage(TUnsignedInt16 nMailId);
extern HERROR CA_NA_IRDCMD_MAIL_UpdateReadFlag(TUnsignedInt16 nMailId, TBoolean bRead);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
extern HERROR CA_NA_IRDCMD_POPUP_DisplayPopupMessage(void);
extern HERROR CA_NA_IRDCMD_POPUP_DeletePopupMessage(void);
extern HERROR CA_NA_IRDCMD_POPUP_MakePopupMessage(TUnsignedInt8 *pxIrdCmd, TUnsignedInt16 *pnPopupId, TBoolean *pfCompleted);
extern HERROR CA_NA_IRDCMD_POPUP_RemovePopupMessage(void);
#endif
#endif /* _CA_CAK_IRDCMD_H */


