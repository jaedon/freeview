
#ifndef __IR_EVT_HEADER__
#define __IR_EVT_HEADER__


typedef enum
{
	/* declaralation for 32 bit wised operation */
	IREVTBW_SVC_INIT				= 0,
	IREVTBW_SVC_OPENED			= 1,
	IREVTBW_SVC_DEFINED			= (IREVTBW_SVC_OPENED <<1), // val 2
	IREVTBW_SVC_SDT_RECEIVED	= (IREVTBW_SVC_DEFINED << 1), // val 4
	IREVTBW_SVC_GLOBAL_STATUS	= (IREVTBW_SVC_SDT_RECEIVED << 1), // val 8

	// TODO: After integration of sdt processing, ADD "IREVTBW_SVC_SDT_RECEIVED"
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
	IREVTBW_SVC_TRIGGER 			= ( IREVTBW_SVC_GLOBAL_STATUS ),
#elif (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	IREVTBW_SVC_TRIGGER 			= ( IREVTBW_SVC_DEFINED | IREVTBW_SVC_GLOBAL_STATUS ),
#else
	IREVTBW_SVC_TRIGGER 			= (	IREVTBW_SVC_OPENED | IREVTBW_SVC_DEFINED | IREVTBW_SVC_GLOBAL_STATUS ),
#endif
	IREVTBW_SVC_CAS_OK 			= (IREVTBW_SVC_TRIGGER << 1),
	IREVTBW_SVC_CAS_FAIL			= (IREVTBW_SVC_CAS_OK << 1),
	IREVTBW_SVC_CLOSED			= (IREVTBW_SVC_CAS_FAIL <<1),


	IREVTBW_SVC_MAX, // = 0x80000000 /*to prevent problem of expanding*/

}IREVT_SVC_STATE;

#if 0
typedef enum
{
	IREVT_NONE =0 ,
	IREVT_SVC, //it's only regarding descramble service and releated banner message.
	IREVT_IRD, //for mail, recovery code and etc,. without banner message.
// TODO: mail,
	IREVT_MAX,
}IREVT_Class
#endif

//Structure for command GRESET, SSVCID
typedef struct
{
	HUINT16 usBouquetId; //it used only for recovery type 1, Reset Channel order.
	HUINT16 usNetworkId;
	HUINT16 usTsId;
	HUINT16 usSvcId;
}RecoveryTuneParam;

typedef struct
{
	HUINT16 usLength;
	HUINT8  *pucBillData;
}VirtualBillData;

typedef struct
{
	HUINT8  pucPhoneNo[16];
	HUINT32 ulIpAddress;
	HUINT16 usPort;
	HUINT16 usReportDate;
	HUINT16 usReportTime;
	HUINT8  pucUsername[10];
	HUINT8  pucPassword[20];
}DataMiningParameters;

typedef struct
{
	HUINT8  pucPhoneNo[16];
	HUINT32 ulIpAddress;
	HUINT16 usPort;
}IPPVCallbackInformation;

typedef struct
{
	HUINT8  ucStatus;
	HUINT16 usReportDate;
	HUINT16 usReportTime;
}IPPVSchedule;

typedef struct
{
	HUINT8  pucPhoneNo[16];
	HUINT32 ulIpAddress;
	HUINT16 usPort;
	HUINT8  pucUsername[20];
	HUINT8  pucPassword[20];
	HUINT8	ucReportType;
}DecoderStatusTrigger;

typedef struct
{
	HUINT8 ucApplication;
	HUINT8 ucSystem;
}DownloadType;

typedef struct
{
	HUINT8  ucDMStatus;
	HUINT8  ucThershold;
	HUINT16 usDelay;
}DataMiningStatus;

typedef struct
{
	HUINT16  usXPos;
	HUINT16  usYPos;
	HUINT8 	 ucDuration;
}DisplayCardNumber;

typedef struct
{
	HUINT16	usChannelKey;
	HUINT32	ulTimeStart;
	HUINT16	usFrequency;
	HUINT32 ulExpiration;
	HUINT32 ulTimeBefore;
	HUINT32	ulTimeAfter;
	HUINT8  pucServiceName[30];
	HUINT8  pucEventName[60];
}RemoteTagging;

typedef enum
{
	eTagging_Freq_Once = 0x01,
	eTagging_Freq_Daily,
	eTagging_Freq_Weekly,
	eTagging_Freq_Weekends,
	eTagging_Freq_Weekdays
}IR_REMOTE_TAGGING_FREQ;

typedef struct
{
	HUINT16	usNetworkId;
	HUINT16	usTsId;
	HUINT16	usServiceId;
	HUINT32	ulTimeStart;
	HINT32	nEventId;
	HUINT16	usDuration;
	HUINT32 ulTimeBefore;
	HUINT32	ulTimeAfter;
	HUINT8  pucServiceName[30];
	HUINT8  pucEventName[60];
#if 0 // 현재 Old version만 사용 Phase 2에서 지원 ?(2010.09.24)
	HUINT8	ucSeriesFlag;
	HUINT16	usFrequency;
	HUINT32 ulExpiration;
	HUINT8	ucCancelFlag;
#endif
}RemoteRecording;

typedef enum
{
	eRecording_Freq_DontCare = 0,
	eRecording_Freq_Once,
	eRecording_Freq_Daily,
	eRecording_Freq_Weekly,
	eRecording_Freq_Weekends,
	eRecording_Freq_Weekdays
}IR_REMOTE_RECORDING_FREQ;

typedef struct
{
	HUINT8  pucProductName[14];
	HUINT8	ucPersistentDelete;
}DeletePushVodEvnet;

typedef struct
{
	HUINT8	ucClear3PAFlash;
	HUINT8	ucClear3PAEEPROM;
}Clear3PAData;

typedef struct
{
	HUINT16	usRow;
	HUINT16	usColumn;
	HUINT8	ucFlashing;
	HUINT8	ucBackType;
	HUINT8	ucBackColor;
	HUINT8	ucFontColor;
	HUINT8	ucDuration;
	HUINT8	ucTextLen;
	HUINT8  pucText[256];
}ColourDisplayData;

typedef struct
{
	HUINT8  pucPhoneNo[16];
	HxCountry_e	eCountryId;
}PhoneNumber;

typedef struct
{
	HUINT8	ucEnableOtherMode;
	HUINT8	pucChipID[8];
	HUINT8	ucReserved1;
	HUINT8	ucReserved2;
}OtherMode;

typedef struct
{
	HUINT16	usRow;
	HUINT16	usColumn;
	HUINT8	ucFlashing;
	HUINT8	pucSMSCode[8];
	HUINT8	ucDuration;
	HUINT8	ucTextLen;
	HUINT8  pucText[256];
}EasySMSDisplayData;

typedef struct
{
	HUINT8	ucButtonEnable;
	HUINT8	ucButtonPosition;
	HUINT8	ucButtonId;
	HUINT16	usDisplayFrequency;
	HUINT8	ucTestMode;
	HUINT16	usReserved;
}DigiaktifTaggingData;

typedef struct
{
	HUINT8	ucEnableFtaLimit;
	HUINT8	pucChipID[8];
	HUINT8	ucDayLimit;
	HUINT8	ucReserved;
}FTALimit;

//	Application에서 Static Buffer를 요구하는 상태임
//	아래 구조체중 pvExt가 사이즈가 가변적이 되어야 하나
//	Application쪽의 요구에 따라 Static으로 할당 받음.
#define	MAX_IREVT_EXTDATA_LEN			2048

typedef union
{
	//IREVT_CLASS_DECODER_MSG_NORMAL_MAIL
	//IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY
	HUINT8 ucMail_Index;

	//IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE
	HUINT32 ulPinCodeBCDType;

	//IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT
	HUINT8 aucHashedCode[IR_SC_INFO_ID_LENGTH]; //Cautions: it uses 12 char size practically.

	//IREVT_CLASS_DECODER_MSG_FORCE_TUNE
	RecoveryTuneParam stForceTuneParam;

	//IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL
	VirtualBillData stVirtualBillData;

	//IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS
	DataMiningParameters stDMParameters;

	//IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP
	HUINT32 ulCaFeatureBitmap;

	//IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO
	IPPVCallbackInformation stIppvCallbackInfo;

	//IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD
	HUINT8	ucThreshold;

	//IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE
	IPPVSchedule stIppvSchedule;

	//IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS,
	DecoderStatusTrigger stDecorderReportStatus;

	//IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE
	DownloadType stDownloadType;

	//IREVT_CLASS_DECODER_MSG_EPG_DM_SETUP
	DataMiningStatus stDMStatus;

	//IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER
	DisplayCardNumber stDisplayCardNo;

	//IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG:
	RemoteTagging stRemoteTagging;

	//IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD
	RemoteRecording stRemoteRecording;

	//IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT
	DeletePushVodEvnet stDelPushVodEvent;

	//IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA
	Clear3PAData stClear3PaData;

	//IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY
	ColourDisplayData stColourDisplayData;

	//IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO
	PhoneNumber stCSPhoneNo;

	//IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO
	PhoneNumber stPpvPhoneNo;

	//IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES
	HUINT32 ulEpgFeatureBitmap;

	//IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS
	HUINT16 usUserFolderSize;

	//IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE
	OtherMode stSetOtherMode;

	//IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF
	DigiaktifTaggingData stDigiaktifTagging;

	//IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY
	EasySMSDisplayData	stEasySmsDisplayData;

	//IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT
	FTALimit stFtaLimit;

	//IREVT_CLASS_DECODER_MSG_PVR
	HUINT32	ulErrIndex;

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_UPC)
	HUINT8	ucUsageId;
#endif
	// TODO: Fill parameters you need here.

	HUINT8	pucExt[MAX_IREVT_EXTDATA_LEN];
//	void * pvExt; /*Reserved for extra parameters, this item should be free in application if allocated */
}IrEvtExtData;

typedef struct
{
	IREVT_CLASSe eIrEvtClass;

	/* Followings  for displaying a CA system banner message */
	HUINT32 		ulAxiCodeIdx; //only used in svc update. regarding banner msg.

	/* Followings to process one decoder data  */
	IrEvtExtData	ExtData;

}IREVT_Data;

// function declaration
HIR_ERR local_cas_ir_EVT_UpdateSvcStatus(IREVT_SVC_STATE state, Handle_t hAction , HMSG *hMsg);
HIR_ERR local_cas_ir_EVT_UpdateIRDStatus(Handle_t hAction , HMSG *hMsg);
HINT32 CAS_IR_EVT_SendEvent(Handle_t hAction, HMSG *hMsg);
#if defined(CONFIG_DEBUG)
void local_cas_ir_EVT_DEBUG_PrintEvtClass(HUINT16 msg_class);
#endif

#endif //__IR_EVT_HEADER__}

