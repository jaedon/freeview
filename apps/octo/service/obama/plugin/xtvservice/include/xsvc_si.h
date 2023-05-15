/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si.h
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XSVC_SI_H__
#define	__XSVC_SI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
// MW
/*******************************************************************/
//#include <pal_pipe.h>
#include <svc_si.h>

//#include <svc_resources.h>

/*******************************************************************/
// DB
/*******************************************************************/
//#include <db_svc.h>
#include <db_param.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	XEVT_SI_START		= eSEVT_SI_END,

// CONFIG_OP_FREESAT_GEN2_ADDFILTER >>>
	/* - Description : Freesat T_EIT_CC PID 변경 알림
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Service Handle
		   p2 : PID
		   p3 : TBD */
	EVT_SI_T_EITCC_PID_CHANGED,

	/* - Description : Freesat T_EIT_PFEXT PID 변경 알림
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Service Handle
		   p2 : PID
		   p3 : TBD */
	EVT_SI_T_EITPFEXT_PID_CHANGED,

	/* - Description : Freesat T_EIT_SCEXT PID 변경 알림
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : Service Handle
		   p2 : PID
		   p3 : TBD */
	EVT_SI_T_EITSCEXT_PID_CHANGED,
// CONFIG_OP_FREESAT_GEN2_ADDFILTER <<<
};

// Module Action Kind
typedef enum
{
	eXSVCSI_ACT_SPEC_None = eSVCSI_ACT_SPEC_None,

	eSVCSI_ACT_SPEC_EuSat,
	eSVCSI_ACT_SPEC_EuCab,
	eSVCSI_ACT_SPEC_EuTer,
	eSVCSI_ACT_SPEC_Freesat,
	eSVCSI_ACT_SPEC_MeSat,
	eSVCSI_ACT_SPEC_Ses,
	eSVCSI_ACT_SPEC_Ertelecom,
	eSVCSI_ACT_SPEC_Nordig,
	eSVCSI_ACT_SPEC_Tdc,		// YouSee
	eSVCSI_ACT_SPEC_AusDtt,		// 호주향 지상파
	eSVCSI_ACT_SPEC_UkDtt,		// UK 지상파
	eSVCSI_ACT_SPEC_Ziggo,		// 네들란드 케이블
	eSVCSI_ACT_SPEC_ItaDtt,		// Italy 지상파
	eSVCSI_ACT_SPEC_Orf,
	eSVCSI_ACT_SPEC_Mbc,		// Middle-East, MBC
	eSVCSI_ACT_SPEC_AmxCenam,	// Central America, AMX

	eSVCSI_ACT_SPEC_Astra,		// SWUP Only Spec:
	eSVCSI_ACT_SPEC_Irdeto,		// SWUP Only Spec: maybe...
	eSVCSI_ACT_SPEC_DvbSsu,		// SWUP Only Spec:
	eSVCSI_ACT_SPEC_Humax,		// SWUP Only Spec: maybe...
	eSVCSI_ACT_SPEC_DTT,		// SWUP Only Spec: maybe...

	// 필요한 Action은 위에 추가하세요...
	eSVCSI_ACT_SPEC_MAX
} XsvcSi_ActionSpec_e;


// SWUP Related
typedef enum
{
	// ASTRA SWUP
	eSI_SWUP_MODE_ASTRA_HOME_TP,		// Home TP 일 경우
	eSI_SWUP_MODE_ASTRA_REFNIT_TP,		// Reference NIT TP
	eSI_SWUP_MODE_ASTRA_SSUBAT_TP,		// SSUBAT TP 일 경우
	eSI_SWUP_MODE_ASTRA_SSUSVC_TP,		// SSUSVC TP 일 경우

	// DVB-SSU
	eSI_SWUP_MODE_DVBSSU_HOME_TP,		// Home TP 일 경우
	eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP,	// SSU Scan Linkage가 가리킨 TP
	eSI_SWUP_MODE_DVBSSU_SSUSVC_TP,		// SSU Service TP

	// EU-DTT SWUP
	eSI_SWUP_MODE_EUDTT_HOME_TP,		// Home TP 일 경우
	eSI_SWUP_MODE_EUDTT_SSUSCAN_TP,		// SSU Scan Linkage가 가리킨 TP
	eSI_SWUP_MODE_EUDTT_SSUSVC_TP,		// SSU Service TP

	// SkyD(KDG) SWUP
	eSI_SWUP_MODE_SKYD_HOME_TP,			// Home TP 일 경우
	eSI_SWUP_MODE_SKYD_SSUSCAN_TP,		// SSU Scan Linkage가 가리킨 TP
	eSI_SWUP_MODE_SKYD_SSUBAT_TP,		// SSU BAT TP
	eSI_SWUP_MODE_SKYD_SSUSVC_TP,		// SSU Service TP

	// SES SWUP
	eSI_SWUP_MODE_SES_HOME_TP,			// Home TP 일 경우
	eSI_SWUP_MODE_SES_REFNIT_TP,		// Reference NIT TP
	eSI_SWUP_MODE_SES_SSUBAT_TP,		// SSUBAT TP 일 경우
	eSI_SWUP_MODE_SES_SSUSVC_TP,		// SSUSVC TP 일 경우
	eSI_SWUP_MODE_SES_BURST_TP, 		//Busrt Upate TP 일 경우.

	// IRDETO SWUP
	eSI_SWUP_MODE_IRDETO_HOME_TP,		// Home TP 일 경우
	eSI_SWUP_MODE_IRDETO_SSUSCAN_TP,	// SSU Scan Linkage가 가리킨 TP
	eSI_SWUP_MODE_IRDETO_SSUSVC_TP,		// SSU Service TP

	// HUMAX SWUP
	eSI_SWUP_MODE_HUMAX_HOME_TP,		// HOME TP 일 경우
	eSI_SWUP_MODE_HUMAX_SSUSVC_TP,		// SSUSVC TP 일 경우
	eSI_SWUP_MODE_HUMAX_SVC_TP,

	// FREESAT SWUP
	eSI_SWUP_MODE_FSATSSU_HOME_TP,		// Home TP 일 경우
	eSI_SWUP_MODE_FSATSSU_SSUSCAN_TP,	// SSU Scan Linkage가 가리킨 TP
	eSI_SWUP_MODE_FSATSSU_SSUSVC_TP,	// SSU Service TP

	// UPC
	eSI_SWUP_MODE_UPC_HOME_TP,			// Home TP 일 경우
	eSI_SWUP_MODE_UPC_SSUSCAN_TP,		// SSU Scan Linkage가 가리킨 TP
	eSI_SWUP_MODE_UPC_SSUSVC_TP,		// SSU Service TP

	// KDG
	eSI_SWUP_MODE_KDG_HOME_TP,			// 30
	eSI_SWUP_MODE_KDG_SSUSCAN_TP,		// SSU Scan Linkage가 가리킨 TP
	eSI_SWUP_MODE_KDG_SSUBAT_TP,		// SSU BAT TP (SSU Link )
	eSI_SWUP_MODE_KDG_SSUSVC_TP,		// SSU Service TP

	eSI_SWUP_MODE_MAX

} XsvcSi_SwupMode_e;

/** SWUP Data Type : MW_SI_GetOtaData 를 통해서 SI Data를 가져 올 경우 필요한 종류를 지정하는 enum. */
typedef enum
{
	// ASTRA SWUP
	eSI_SWUP_DATA_DELIVERY_TSINFO,		// NIT 에 DeliverySystem Des를 통해 오는 SvcTsInfo (On/TsId + TuningInfo)
	eSI_SWUP_DATA_REFNIT_TRIPLEID,		// NIT 에 존재하는 Reference NIT LINKAGE Triple ID -> to REF-NIT
	eSI_SWUP_DATA_SSU_SCAN_TRIPLEID,	// NIT 에 존재하는 SSU SCAN LINKAGE Triple ID -> to SSU-BAT
	eSI_SWUP_DATA_SSU_LINK_TRIPLEID,	// BAT 에 존재하는 SSU LINKAGE Triple ID -> to SSU-SVC
	eSI_SWUP_DATA_SSU_SIGNAL,			// PMT 에 존재하는 SWUP Signalling Information

	eSI_SWUP_DATA_SSU_UPDATE_INFO,
	eSI_SWUP_DATA_SSU_SCHEDULE_INFO,
	eSI_SWUP_DATA_SSU_MESSAGE_INFO,

	// SES SWUP
	eSI_SWUP_DATA_SSU_BURST_INFO,
	eSI_SWUP_DATA_SESLCN_TRIPLEID,		// APS LCN 을 가리키는 Triple ID
	eSI_SWUP_DATA_NETWORK_NETINFO,		// Network Information
	eSI_SWUP_DATA_ASTAR_PORTAL_TRIPLEID,	// Astra portal 의 tirple id  정보.
	eSI_SWUP_DATA_ASTAR_REGIONAL_PORTAL_TRIPLEID,		//Astra Region portal 의 tirple id 정보

	//Humax SWUP
	eSI_SWUP_DATA_HUMAX_SVC,
	eSI_SWUP_DATA_HUMAX_DESC,

	// UPC SWUP
	eSI_SWUP_DATA_UPC_SIGNAL,			// PMT 에 존재하는 Signalling Information

	// Irdeto SWUP						// BAT 에 존재하는 IRDETO DOWNLOAD CONTROL BYTE정보
	eSI_SWUP_DATA_IRDETO_CTRL_BYTE,

	eSI_SWUP_DATA_MAX
} XsvcSi_SwupDataType_e;

#if 0
typedef enum tag_XSVC_SI_MODULE_Tag
{
	eSI_MOD_TAG_None			= 0x00000000,

	eSI_MOD_TAG_EuSat			= 0x00000001,
	eSI_MOD_TAG_EuCab			= 0x00000002,
	eSI_MOD_TAG_EuTer			= 0x00000003,
	eSI_MOD_TAG_AusTer			= 0x00000004,
	eSI_MOD_TAG_FraTer			= 0x00000005,
	eSI_MOD_TAG_NldTer			= 0x00000006,
	eSI_MOD_TAG_UkTer			= 0x00000007,
	eSI_MOD_TAG_NordigSat		= 0x00000008,
	eSI_MOD_TAG_NordigCab		= 0x00000009,
	eSI_MOD_TAG_NordigTer		= 0x0000000a,
	eSI_MOD_TAG_ItalyTer		= 0x0000000b,

// Common Module은 위로 추가
	eSI_MOD_TAG_Aps				= 0x00010001,
	eSI_MOD_TAG_CanalReady		= 0x00010002,
	eSI_MOD_TAG_Digiturk		= 0x00010003,
	eSI_MOD_TAG_JapanSat		= 0x00010004,
	eSI_MOD_TAG_JapanCab		= 0x00010005,
	eSI_MOD_TAG_JapanTer		= 0x00010006,
	eSI_MOD_TAG_Kdg				= 0x00010007,
	eSI_MOD_TAG_MeSat			= 0x00010008,
	eSI_MOD_TAG_MosTelecom		= 0x00010009,
	eSI_MOD_TAG_NtvPlus			= 0x0001000a,
	eSI_MOD_TAG_Rcs				= 0x0001000b,
	eSI_MOD_TAG_SkyD			= 0x0001000c,
	eSI_MOD_TAG_SpainTdt		= 0x0001000d,
	eSI_MOD_TAG_Tivu			= 0x0001000e,
	eSI_MOD_TAG_TrueVision		= 0x0001000f,
	eSI_MOD_TAG_Tvt				= 0x00010010,
	eSI_MOD_TAG_UkFsat			= 0x00010011,
	eSI_MOD_TAG_Upc				= 0x00010012,
	eSI_MOD_TAG_Ziggo			= 0x00010013,

// Operator Module은 위로 추가
	eSI_MOD_TAG_MAX,
	eSI_MOD_TAG_USER			= 0xFFFFFFFD
} XsvcSi_Module_Tag_e;
#endif

#if 0
typedef struct tagCopyProtection_Info
{
	HUINT8	ucCopyControlType;
	HUINT8	ucDigitalRecording;
	HUINT8	ucAnalogRecording;
	HUINT8	ucCopyRestrictionMode;
	HUINT8	ucImageContraintToken;
	HUINT8	ucEncryptionMode;
	HUINT8	ucRetentionMode;
	HUINT8	ucRetentionState;
} CopyProtection_Info_t;
#endif

typedef enum
{
	eFreesatSiMode_G1 = 0,
	eFreesatSiMode_G2 = 1
} XsvcSi_SiModeFsat_e;

#if !defined(CONFIG_OTA_IRDETO_LOADER_V5)
typedef struct
{
	HUINT8				ucManufacturerCode;
	HUINT8				ucHardwareCode;
	HUINT8				ucLoadSequenceNumber;
	HUINT8				ucVariantHi;
	HUINT8				ucVariantLo;
} XsvcSi_SwupCheckParam_Irdeto_t;

typedef struct
{
	HUINT8				ucManufacturerCode;
	HUINT8				ucHardwareCode;
	HUINT8				ucLoadSeqNumber;
	HUINT8				ucTableId;
	HxDATETIME_Time_t	stDownloadTime;
	HUINT16 			usPid;
} XsvcSi_SwupSignal_Irdeto_t;
#else
typedef struct
{
	HUINT16				usManufacturerId;
	HUINT16				usHardwareVersion;
	HUINT16				usVariant;
	HUINT16				usSubVariant;
	HUINT16				usDownloadSequenceNumber;
	HUINT8				ucTiggerControlByte;
	HUINT8				ucDownloadMode;
} XsvcSi_SwupCheckParam_Irdeto_t;

typedef struct
{
	HUINT16				usManufacturerId;
	HUINT16				usHardwareVersion;
	HUINT16				usVariant;
	HUINT16				usSubVariant;
	HUINT16				usDownloadSequenceNumber;
	HUINT16 			usPid;
} XsvcSi_SwupSignal_Irdeto_t;
#endif

typedef struct
{
	HBOOL		bSwupSvc;
	HUINT16		usSvcId;
	HUINT16		usTsId;
	HUINT16		usOnId;
	HUINT16		usPid;
	HUINT32		ulHardwareVersion;
	HUINT32		ulSoftwareVersion;
	HUINT8		ucForceFlag;
} XsvcSi_SwupSignal_Humax_t;

/*********************************
	UK DTT  구조체.
**********************************/
#define	XsvcSi_UKDTT_MAX_NCD_MESSAGE_LEN				(512)		/* 실제 message 와 date 정보를 포함하기에 512로 늘리자 */
#define	XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM				(4)			/* Suite 참조하여 1로 만들자 */

typedef struct
{
	HUINT8	iso639[4];
	HUINT8	ucText[XsvcSi_UKDTT_MAX_NCD_MESSAGE_LEN];
} XsvcSi_NcdInfoMsg_UkDtt_t;

typedef struct
{
	HBOOL		bValid;				/* Don't Change */
	HUINT8		ucNetChangeId;
	HUINT8		ucNetChangeVer;
	UNIXTIME		ulStartUnixtime;
	HUINT32		ulDuration;
	HUINT8		ucChangeType;
	HUINT16		usDemodCellId;
	XsvcSi_NcdInfoMsg_UkDtt_t	message[XsvcSi_UKDTT_MAX_NCD_MESSAGE_NUM];

	HUINT8			ucInvariantTsPresent;
	HUINT16			usInvariantTsId;
	HUINT16			usInvariantOnId;

	HUINT8		ucReserved[10];
} XsvcSi_NcdInfoData_UkDtt_t ;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#endif	/*	!__XSVC_SI_H__	*/
