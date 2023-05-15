

#ifndef	OAPI_TYPES_H__
#define OAPI_TYPES_H__
#include "hlib.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/* Start === related to META SVC */
#define	DxDESC_LEN							256
#define	DxDESC_TEXT_LEN						(512 + 4)
#define	DxCHICON_URL_LEN_					64

#define	DxEVENT_LINKAGE						8

/* Start === related to META MAIL */
#define 	DxMAIL_MAX_TEXT_SIZE				2048
#define 	DxMAIL_MAX_TITLE_SIZE				256
#define 	DxMAIL_MAX_DATE_SIZE			20

/* Start === related to META PPVHISTORY */
#define 	DxPPVHISTORY_MAX_EVENTNAME_SIZE	256

/* Start === related to META DES */
#define	DxDES_CA_CONTRACT_INFO_COMP_NUM_MAX		12 		/* 유효한 ECM PID는 12 ES in 1 service - TR B14 제1분책 제3편 2.1.2.5 ES수의 제한, Hagaren은 15 -  stCaContractInfo */
#define	DxDES_CA_CONTRACT_VERIFICATION_INFO_MAX	256		/* Hagaren svc_cas에서 사용하는 size */
#define	DxDES_CA_CONTRACT_INFO_FEE_NAME_MAX		21		/* 유료 방송 요금명, max 20 bytes - hagaren stCaContractInfo */
#define	DxDES_DIGITAL_COPY_CONTROL_DES_NUM_MAX	16		/* Max 16 ES in 1 service - TR B14 제1분책 제3편 2.1.2.5 ES수의 제한  */
#define	DxDES_CA_CONTRACT_INFO_DES_NUM_MAX		3		/* 3 CAS - Pioneer, Panasonic, Hitachi JCL */

/*******************************************************************/
/********************     Video  					*************************/
/*******************************************************************/
#define _________enum_video__________________________________________________________


/*******************************************************************/
/********************     Audio  					*************************/
/*******************************************************************/
#define _________enum_Audeo___________________________________________________________________

/*******************************************************************/
/********************    channel 					*************************/
/*******************************************************************/
#define __________channel___________________________________________________________


/*******************************************************************/
/********************    service					*************************/
/*******************************************************************/
#define _________enum_service_________________________________________________________


/*******************************************************************/
/********************    PVR			*************************/
/*******************************************************************/
#define _________enum_PVR_________________________________________________________
#define DXPVR_MAX_ES_STRING_LEN				81 	/* video : 16, audio : 33, data : 80 + 1 */
#define DxPVR_MAX_DATA_ES_NUM				12	/* TR-B14 제3편 2.1.2.5 ES수의 제한 */
typedef struct
{
	HUINT16					usPid;
	HUINT8					ucComponentTag;
	HUINT8					uDataEntryComponentExistFlag;		/* EIT에 pid에 대한 descriptor 정보가 있을 경우 TRUE */
	HUINT8					uAssociatedContentsFlag;			/* Data 방송이 현재 프로그램에 대한 방송 유무 */
	HUINT8					szLangCode[DxPVR_LANG_ISO_639_LEN+1];	/* For caption and super */
	HUINT8					aucTextChar[DXPVR_MAX_ES_STRING_LEN];		/* For data(BML) */
} DxMETA_PvrDataInfo_t;

typedef struct
{
	HUINT32					ulItemNum;
	DxMETA_PvrDataInfo_t	astDataInfo[DxPVR_MAX_DATA_ES_NUM];
} DxMETA_PvrDataInfoList_t;


#define _____end_freesat_service______________________________________________

/*******************************************************************/
/********************    Struct				*************************/
/*******************************************************************/
#define _________STRUCT_________________________________________________________

typedef struct
{
	HINT32 nId;
	HUINT16 networkId;
	HUINT16 tsId;
	HUINT16 serviceId;
	HUINT32 lcn;
	HUINT32 startTime;
	HUINT32 durationTime;
	HUINT8 mainGenre;
	HUINT8 parentalRating;
	HUINT32 ppvFee;
	HBOOL isRecord;
	HUINT32 purchaseTime;
	HUINT32 caSystemId;
	HUINT8 svcName[DxNAME_LEN];
	HUINT8 eventName[DxPPVHISTORY_MAX_EVENTNAME_SIZE];
	HUINT32 				__CRC__;  //CHECK CRC
} DxPPVHistory_t;


typedef struct
{
	HUINT32 			ulId;
	HINT32			eMailType;
	HBOOL			bIsOpen;
	HBOOL			bIsLocked;
	HINT32			nNetworkType;
	HUINT32 			ulBroadcasterGroupId;
	HUINT32 			ulMessageId;
	HUINT8			ucSendTime[DxMAIL_MAX_DATE_SIZE];
	HUINT8			ucMailTitle[DxMAIL_MAX_TITLE_SIZE];
	HUINT8			ucMailText[DxMAIL_MAX_TEXT_SIZE];

	HUINT32 				__CRC__;  //CHECK CRC
} DxMail_t;

typedef struct
{
	HUINT8	ucComponentTag;
	HUINT8	ucDigitalRecordingControlData;
	HUINT8	ucMaximumBitRateFlag;
	HUINT8 	ucCopyControlType;
	HUINT8	ucAPSControlData;
	HUINT8	ucMaximumBitRate;
} DxDes_DigitalCopyControlDesComponent_t;

typedef struct
{
	HUINT8	ucDigitalRecordingControlData;
	HUINT8	ucMaximumBitRateFlag;
	HUINT8	ucComponentControlFlag;
	HUINT8	ucCopyControlType;
	HUINT8	ucAPSControlData;
	HUINT8	ucMaximumBitRate;
	HUINT8	ucNumComponents;
	DxDes_DigitalCopyControlDesComponent_t	astDigitalCopyControlDes[DxDES_DIGITAL_COPY_CONTROL_DES_NUM_MAX];	/* Max 16 ES in 1 service - TR B14 제1분책 제3편 2.1.2.5 ES수의 제한  */
} DxDes_DigitalCopyControlDes_t;

typedef struct
{
	HUINT16	usCASystemId;
	HUINT8	ucCAUnitId;
	HUINT8	ucNumOfComponent;
	HUINT8	aucComponentTag[DxDES_CA_CONTRACT_INFO_COMP_NUM_MAX];	/* 유효한 ECM PID는 12 ES in 1 service - TR B14 제1분책 제3편 2.1.2.5 ES수의 제한, Hagaren은 15 -  stCaContractInfo */
	HUINT8	ucContractVerificationInfoLength;
	HUINT8	aucContractVerificationInfo[DxDES_CA_CONTRACT_VERIFICATION_INFO_MAX];	/* Hagaren svc_cas에서 사용하는 size */
	HUINT8	ucFeeNameLength;
	HUINT8	aucFeeName[DxDES_CA_CONTRACT_INFO_FEE_NAME_MAX];	/* 유료 방송 요금명, max 20 bytes - hagaren stCaContractInfo */
} DxDes_CaContractInfoDes_t;

typedef struct
{
	HUINT16	usDesId;
	HUINT16 usNetworkId;
	HUINT16	usTsId;
	HUINT16	usSvcId;
	HUINT8	ucFreeCAMode;
	/* Digital Copy Control Descriptor */
	HBOOL	bDigitalCopyControlDes;
	DxDes_DigitalCopyControlDes_t	astDigitalCopyControlDes[1];

	/* CA Contract Information Descriptor */
	HUINT8	ucNumCaContractInfoDes;
	DxDes_CaContractInfoDes_t astCaContractInfoDes[DxDES_CA_CONTRACT_INFO_DES_NUM_MAX];	/* 3 CAS - Pioneer, Panasonic, Hitachi JCL */

	HUINT32 __CRC__;  //CHECK CRC
} DxDes_t;



/* DxPbEventInfo_t에 내용을 추가하면 DxPbEventInfo_ExcepPointers_t에도 동일하게 반영 필요 */
typedef struct
{
	/* For programme class */
	HUINT32		ulSvcUid;				/* for channel class. PB중 channel class가 의미 있나? */
	HINT32		nEventId;				/* for programmeID. PB중 programmeID가 의미 있을까? */
	HUINT8		ucParentalRating;
	HUINT32     ulStartTime;
	HUINT32     ulDuration;
	HBOOL       bSubtitles;
	HBOOL		bIsHD;					/* isHD */
	HUINT8		ucContentNibble;		/* genre = contentNibble<<8 | userNibble */
	HUINT8		ucUserNibble;
	HUINT8		ucDigitalCopyControl;	/* eDigitalOutput */
	HUINT8		ucAnalogCopyControl;	/* eAnalogOutput */
	HUINT32		ulLongDescriptionLen;	/* To verify long description length */

	/* For jlabsDataContentComponent class */
	DxMETA_PvrDataInfoList_t	stCaptionList;
	DxMETA_PvrDataInfoList_t	stSuperimposeList;
	DxMETA_PvrDataInfoList_t	stDataList;

	HUINT32 	__CRC__;  //CHECK CRC

	/* Pointer로 선언한 값은 __CRC__ 아래에 위치 하도록 한다. */
	HCHAR		*name;
	HCHAR		*description;
	HCHAR		*longDescription;
} DxPbEventInfo_t;

typedef struct
{
	/* For programme class */
	HUINT32		ulSvcUid;				/* for channel class. PB중 channel class가 의미 있나? */
	HINT32		nEventId;				/* for programmeID. PB중 programmeID가 의미 있을까? */
	HUINT8		ucParentalRating;
	HUINT32     ulStartTime;
	HUINT32     ulDuration;
	HBOOL       bSubtitles;
	HBOOL		bIsHD;					/* isHD */
	HUINT8		ucContentNibble;		/* genre = contentNibble<<8 | userNibble */
	HUINT8		ucUserNibble;
	HUINT8		ucDigitalCopyControl;	/* eDigitalOutput */
	HUINT8		ucAnalogCopyControl;	/* eAnalogOutput */
	HUINT32		ulLongDescriptionLen;	/* To verify long description length */

	/* For jlabsDataContentComponent class */
	DxMETA_PvrDataInfoList_t	stCaptionList;
	DxMETA_PvrDataInfoList_t	stSuperimposeList;
	DxMETA_PvrDataInfoList_t	stDataList;

	HUINT32 	__CRC__;  //CHECK CRC
} DxPbEventInfo_ExcepPointers_t;

#define _______API_FUNCTIONS______________________________________________

HUINT32		DxServiceGetCRC(void);
HUINT32 	DxTransponderGetCRC(void);
HUINT32 	DxProviderGetCRC(void);
HUINT32 	DxGroupGetCRC(void);
HUINT32 	DxBouquetGetCRC(void);
HUINT32 	DxNetworkGetCRC(void);
HUINT32 	DxMailGetCRC(void);
HUINT32 	DxDesGetCRC(void);
HUINT32 	DxPPVHistoryGetCRC(void);

DxEvent_t *				DxEvent_Decode (HxJSONBIN_t decoder, DxEvent_t *type, HxJSON_t json);

void					DxRecData_FreeBin(DxRecListData_t *p);
DxRecListData_t*		DxRecData_DecodeBin (DxRecListData_t *type, HxJSON_t json);
const HCHAR *			DxRecData_EncodeBin (HxJSONBIN_t enc, const DxRecListData_t *recData);
HERROR		            DxRecData_Decode (DxRecListData_t *Rec, HxJSON_t json);
HCHAR *			        DxRecData_Encode (const DxRecListData_t *Rec);

void					DxSchedule_FreeBin(DxSCHEDULE_t *schedule);
HCHAR*					DxSchedule_StrDup(const HCHAR *str);
DxSCHEDULE_t *			DxSchedule_DecodeBin (DxSCHEDULE_t *shcedule, HxJSON_t json);
const HCHAR *			DxSchedule_EncodeBin (HxJSONBIN_t enc, const DxSCHEDULE_t *shcedule);
HERROR  			    DxSchedule_Decode (DxSCHEDULE_t *shcedule, HxJSON_t json);
HCHAR *			        DxSchedule_Encode (const DxSCHEDULE_t *shcedule);


DxService_t *			DxService_DecodeBin (DxService_t *type, HxJSON_t json);
const HCHAR *			DxService_EncodeBin (HxJSONBIN_t enc,const DxService_t *type);
DxService_t *	        DxService_Decode(DxService_t *service, HxJSON_t json);
HCHAR*                  DxService_Encode(const DxService_t *pvData);

DxTransponder_t *		DxTransponder_DecodeBin (DxTransponder_t *type, HxJSON_t json);
const HCHAR *			DxTransponder_EncodeBin (HxJSONBIN_t enc, const DxTransponder_t *type);
DxTransponder_t *	    DxTransponder_Decode (DxTransponder_t *transponder, HxJSON_t json);
HCHAR *			        DxTransponder_Encode (const DxTransponder_t *transponder);

DxProvider_t *			DxProvider_DecodeBin (DxProvider_t *type, HxJSON_t json);
const HCHAR *			DxProvider_EncodeBin (HxJSONBIN_t enc, const DxProvider_t *type);
DxProvider_t *			DxProvider_Decode (DxProvider_t *provider, HxJSON_t json);
HCHAR *			        DxProvider_Encode (const DxProvider_t *provider);

DxGroup_t *				DxGroup_DecodeBin (DxGroup_t *type, HxJSON_t json);
const HCHAR *			DxGroup_EncodeBin (HxJSONBIN_t enc, const DxGroup_t *type);
DxGroup_t *				DxGroup_Decode (DxGroup_t *group, HxJSON_t json);
HCHAR *			        DxGroup_Encode (const DxGroup_t *group);

DxBouquet_t *			DxBouquet_DecodeBin (DxBouquet_t *type, HxJSON_t json);
const HCHAR *			DxBouquet_EncodeBin (HxJSONBIN_t enc, const DxBouquet_t *type);
DxBouquet_t *			DxBouquet_Decode(DxBouquet_t *bouquet, HxJSON_t json);
HCHAR *			        DxBouquet_Encode(const DxBouquet_t *bouquet);

DxNetwork_t *			DxNetwork_DecodeBin (DxNetwork_t *type, HxJSON_t json);
const HCHAR *			DxNetwork_EncodeBin (HxJSONBIN_t enc, const DxNetwork_t *type);
DxNetwork_t *			DxNetwork_Decode (DxNetwork_t *network, HxJSON_t json);
HCHAR *			        DxNetwork_Encode (const DxNetwork_t *network);

DxMail_t *				DxMail_DecodeBin (DxMail_t *type, HxJSON_t json);
const HCHAR *			DxMail_EncodeBin (HxJSONBIN_t enc, const DxMail_t *type);
DxMail_t *				DxMail_Decode(DxMail_t *mail, HxJSON_t json);
HCHAR *					DxMail_Encode(const DxMail_t *mail);

DxPPVHistory_t *  		DxPPVHistory_DecodeBin (DxPPVHistory_t *type, HxJSON_t json);
const HCHAR *			DxPPVHistory_EncodeBin (HxJSONBIN_t enc, const DxPPVHistory_t *type);
DxPPVHistory_t *		DxPPVHistory_Decode(DxPPVHistory_t *ppvhistory, HxJSON_t json);
HCHAR *					DxPPVHistory_Encode(const DxPPVHistory_t *ppvhistory);

HCHAR*					DxXXX_EncodeFile(HCHAR *objectName,  void * data,HINT32 size);
void*					DxXXX_DecodeFile(HCHAR *objectName,  void * data ,HINT32 *size);
void					DxMail_Print (FILE *fp, const DxMail_t *t);
void					DxPPVHistory_Print (FILE *fp, const DxPPVHistory_t *t);

DxPbEventInfo_t *		DxPbEventInfo_DecodeBin (DxPbEventInfo_t *data, HxJSON_t json);
const HCHAR * 			DxPbEventInfo_EncodeBin (HxJSONBIN_t enc, const DxPbEventInfo_t *pbEventInfo);

DxDes_t *				DxDes_DecodeBin (DxDes_t *type, HxJSON_t json);
const HCHAR *			DxDes_EncodeBin (HxJSONBIN_t enc, const DxDes_t *type);
void					DxDes_Print (FILE *fp, const DxDes_t *t);

#ifdef	__cplusplus
}
#endif


#endif /* OAPI_TYPES_H__ */
