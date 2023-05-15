 /**
 @file				 ca_psi_if_ext.h
 @brief 			 ca_psi_if_ext.h (Nagra PSI SI Service)

 Description:  \n
				 \n
 Module: MW/CAS \n
 Remarks :\n

 Copyright (c) 2008 HUMAX Co., Ltd. \n
 All rights reserved. \n
 */
#ifndef _CA_PSI_SI_EXT_H
#define _CA_PSI_SI_EXT_H

/********************************************************************
  Constant/Macro Definition
 ********************************************************************/
#if defined(CONFIG_MW_CH_SAT_1TUNER)
#define MAX_TUNER_NO						1
#else
#define MAX_TUNER_NO						2
#endif

/*	CAS 관련 definition */
#if defined(CONFIG_MW_MM_PVR)
#define MAX_NO_SESSION						NUM_MAX_ACTION							//pvr model
#else
#define MAX_NO_SESSION						1										//zapper model
#endif

#define MAX_OPEN_SESSION					4										//동시 open 가능한 session 갯수
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) && defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#define MAX_NO_STREAM						6										//max num of stream of each session  <-- broadcom에 MAX_NUMBER_OF_PIDS_PER_TSID 개수에 맞도록 설정 필요
#else
#define MAX_NO_STREAM						16										//max num of stream of each session
#endif
#define MAX_TOTAL_NO_STREAM				(MAX_NO_STREAM * MAX_OPEN_SESSION)		//MAX_NO_STREAM * MAX_OPEN_SESSION

#define MAX_NUM_OF_CA_PID					60
#define MAX_NUM_OF_SERVICE_PID			60

#define NULL_INDEX							0xFFFFFFFF
#define NULL_PID								0x2000

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#define TAG_NASP_CA						0x86
#define TAG_MACD_CA							0x8C			// Merlin Access Criteria Descriptor
#define MAX_NUM_OF_CA_MAC_DESC				5
#endif

#define MAX_EIT_DES_SIZE 					1024
#define MAX_STR_LEN							32


/********************************************************************
  Structure and Type Definition
 ********************************************************************/
typedef enum tagCasType
{
	eCasTypeFree = 0,
	eCasTypeNaScramble,
	eCasTypeOtherScramble,
	eCasType_Max
} CasType_t;

typedef struct PSI_CA_DES_STRUCT
{
	TUnsignedInt16		usCaSysId;
	TUnsignedInt16		usCaPid;
} PSI_CA_DES_t;

typedef struct PSI_CA_CAT_DES_LIST_STRUCT
{
	TUnsignedInt8			ucCaDesNum;
	PSI_CA_DES_t		stCaDesElem[MAX_NUM_OF_CA_PID];
} PSI_CA_CAT_DES_LIST_t;

typedef struct PSI_CA_PMT_ELEM_STRUCT
{
	EsType_t				eEsType;

	TUnsignedInt8			ucStreamType;
	TUnsignedInt8			bIsCurrent;
	TUnsignedInt16		usElemPid;

	TUnsignedInt8			ucaFlag;
	TUnsignedInt8			ucNumOfCADesc;
	PSI_CA_DES_t		stElemCaDesc[MAX_NUM_OF_CA_PID];
} PSI_CAS_PMT_ELEM_t;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
typedef struct PSI_CA_NASC_DESC_STRUCT
{
	TUnsignedInt8			ucTag;
	TUnsignedInt8			ucLength;
	TUnsignedInt8			*pucRawData;
} PSI_CA_NASC_DESC_t;

typedef struct PSI_CA_MAC_DESC_STRUCT
{
	TUnsignedInt8		ucTag;
	TUnsignedInt8		ucLength;
	TUnsignedInt8		*pucRawData;

	TUnsignedInt16	usEcmCaSysId;
} PSI_CA_MAC_DESC_t;										// Merlin Access Criteria Descriptor
#endif

typedef struct PSI_CA_PMT_INFO_STRUCT
{
	TBoolean				bUsed;
	TUnsignedInt16		usPmtLen;

	TUnsignedInt16		usServiceId;
	TUnsignedInt32		ulCrc32;
	TUnsignedInt16		usPcrPid;
	TUnsignedInt8   		ucVerNum;
	TBoolean				bCurNextInd;

	TUnsignedInt8			ucaFlag;
	TUnsignedInt8			ucNumOfCADesc;
	PSI_CA_DES_t		stGlobCaDesc[MAX_NUM_OF_CA_PID];

	TUnsignedInt8			ucNumOfElem;
	PSI_CAS_PMT_ELEM_t	stElemInfo[MAX_NUM_OF_SERVICE_PID];

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	PSI_CA_MAC_DESC_t	stCaMacDesc[MAX_NUM_OF_CA_MAC_DESC];						// Merlin Access Criteria Descriptor, 0-for Ariden, 1-For Merlin
#endif
} PSI_CA_PMT_INFO_t;

#if defined(CONFIG_MW_CAS_NAGRA_RCS)
typedef struct PSI_EVT_PURCHASE_STRUCT
{
	TUnsignedInt8			fPresent;
	TUnsignedInt8			ucEvtName[MAX_STR_LEN];
	TUnsignedInt8			ucSvcName[MAX_STR_LEN];
	TUnsignedInt16		usEvtId;
	TUnsignedInt32		ulDesLength;
	TUnsignedInt8			ucDesInfo[MAX_EIT_DES_SIZE];
	TUnsignedInt8			ucRawEvtNameLength;
	TUnsignedInt8			*pucRawEvtName;
	TUnixDate			ulStartTime;
	TUnixDate			ulDuration;
	TCalendarTime		stCalendarTime;
} PSI_EventPurcahseInfo_t;
#endif

extern HERROR CA_NA_PsiGetLiveAction(Handle_t       *phAction);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
extern HERROR 	CA_NA_PSI_IsHdPlusService(Handle_t hAction);
extern HERROR 	CA_NA_PSI_GetSizeOfMacCaDesc(Handle_t hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucLen);
extern HERROR 	CA_NA_PSI_GetContentOfMacCaDesc(Handle_t hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucDesc);
extern HERROR 	CA_NA_PSI_GetActionsSameService(Handle_t hAction, Handle_t *phActions, TUnsignedInt8 *pucNum);
#endif


#if defined(CONFIG_MW_CAS_NAGRA_RCS)
extern HERROR CA_NA_PSI_siExt_ParseEit(Handle_t hAction, TUnsignedInt8 *pucEitPRaw, TUnsignedInt8 *pucEitFRaw);
extern HERROR CA_NA_PSI_siExt_GetEventInfo(TUnsignedInt8 ucEitType, PSI_EventPurcahseInfo_t *pstEventInfo);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
extern void		NAMGR_CAKSVC_SetParentalControl(Handle_t hAction, TBoolean bUserPinPass);
#endif
#if defined(CONFIG_DEBUG)
void NAMGR_CAKSVC_PrintNagraInstance(void);
#endif
#endif /* _CA_PSI_SI_EXT_H */

