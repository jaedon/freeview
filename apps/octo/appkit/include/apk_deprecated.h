#ifndef	__APK_DEPRECATED_H__
#define	__APK_DEPRECATED_H__
typedef	void*			APK_HANDLE;

typedef	void	(*CBObjectHandler)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

#if 0
typedef enum
{
	eTargetStorageManual = 0,
	eTargetStorageIntHdd = 1,
	eTargetStorageIntSubHdd = 2,
	eTargetStorageExtHdd = 3
} APKE_TargetStorage_t;

typedef enum
{
	ePANEL_UPPER	= 1,
	ePANEL_MIDDLE	= 0,
	ePANEL_LOWER	= -1
} APKE_PANEL_Priority_e;

typedef enum {
	ePanel_ITEM_STRING,
	ePanel_ITEM_LED,
	ePanel_ITEM_ALL,
} APKE_ITEM_TYPE_e;

typedef enum
{
	eMS_MAX_STORAGE_NUM				= 9,		// eMW_FS_DEV_MAX
	eMS_MAX_PARTITION_NUM			= 12,		// MW_FS_MAX_PARTITION
	eMS_MAX_DEV_NAME				= 16,
	eMS_MAX_FS_VOLUME_LABEL_SIZE	= 17,
	eMS_MAX_FS_MOUNT_PATH_LEN		= 32,
	eMS_MAX_UUID_LEN				= 37,
	eMS_MAX_STORAGEID_LEN			= 37,
} APKE_MAX_Size_e;

typedef struct
{
	unsigned short			usSessionId;
	unsigned char				ucAppDomainIdLen;
	unsigned char *			pucAppDomainId;	// in case of CI+ : <applicationDomainIndentifier>[;<option1>;<option2>;¡¦;<option#>], <applicationDomainIndentifier> = CI_AMMI_APP_DOMAIN_ID
	unsigned char				ucInitialObjectLen;
	unsigned char *			pucInitialObject;
} sCiAmmiTypeReqStart_t;

// CI_AMMI_TYPE_FILE_ACK
typedef struct
{
	unsigned short			usSessionId;
	unsigned char				ucFileOK;	// 1 ÀÌ¾î¾ß À¯È¿, 0 ÀÌ¸é ¹«È¿.
	unsigned int			enReqType;
//	eMhegIb_ReqType_t		enReqType;
	unsigned long				ulBytesLen;	// if enReqType is CI_AMMI_REQUEST_TYPE_FILE, "1 + file name length (x) + 4 + file data length (y)" in case of using S&T MHEG engine
	unsigned char *			pucBytes;	// if enReqType is CI_AMMI_REQUEST_TYPE_FILE, "1 byte for x + x bytes for file name + 4 bytes for y + y bytes for file data" in case of using S&T MHEG engine
} sCiAmmiTypeFileAck_t;

// CI_AMMI_TYPE_REQ_APP_ABORT
typedef struct
{
	unsigned short				usSessionId;
	unsigned int				enAbortReqCode;
//	eMhegIb_AbortReqCode_t		enAbortReqCode;
} sCiAmmiTypeReqAppAbort_t;

// CI_AMMI_TYPE_APP_ABORT_ACK
typedef struct
{
	unsigned short				usSessionId;
//	unsigned char   				ucAbortAckCode;		// CI+ ½ºÆå 1.2, TS 101 699 1.1.1, D-BOOK 5.0 ¸ðµÎ ºÐ¸íÈ÷ ¸í½ÃµÇ¾î ÀÖÁö ¾Ê¾Æ ÀÏ´Ü ¸·¾ÆµÒ
} sCiAmmiTypeAppAbortAck_t;

//CI_AMMI_CONTENT
typedef union
{
	sCiAmmiTypeReqStart_t			typeReqStart;
	sCiAmmiTypeFileAck_t				typeFileAck;
	sCiAmmiTypeReqAppAbort_t			typeReqAppAbort;
	sCiAmmiTypeAppAbortAck_t			typeAppAbortAck;
} uCiAmmiContent_t;

typedef enum
{
	eCiAmmiMsgType_reqStart,
	eCiAmmiMsgType_FileAck,
	eCiAmmiMsgType_ReqAppAbort,
	eCiAmmiMsgType_AppAbortAck,
	eCiAmmiMsgType_MAX
} eCiAmmiMsgType_t;

//CI_AMMI_REQUEST_START_ACK_CODE
typedef enum
{
	eCiAmmiRequestStartAckCode_OK 			= 0x01,
	eCiAmmiRequestStartAckCode_WRONG_API	= 0x02,	// Application domain not supported
	eCiAmmiRequestStartAckCode_API_BUSY		= 0x03	// Application domain supported but not currently available
} eCiAmmiRequestStartAckCode_t; // TS 101 699 ½ºÆå

/* Start of CAS_BC */
#define APKD_CAS_BC_MAX_RIGHTS_ERROR_URL			256
#define APKD_CAS_BC_MANUFACTURERID_MAX_LEN		5
#define APKD_CAS_BC_CARD_ID_MAX_LEN				21
#define APKD_CAS_BC_GROUP_ID_MAX_NUM				7

#define APKD_CAS_BC_NUM_OF_KIND_OF_GROUP_ID		2
#define APKD_CAS_BC_BCAS_GROUPID_PROPERTY		"bcasgroupid"
#define APKD_CAS_BC_CCAS_GROUPID_PROPERTY		"ccasgroupid"

#define APKD_CAS_BC_AUTO_DISPLAY_MSG_MAX_LEN		800
#define APKD_CAS_BC_ALTERNATIVE_MSG_MAX_LEN		320
#define APKD_CAS_BC_SENDDRMMSG_MSG_MAX_LEN		300
#define APKD_CAS_BC_DRMMSGRESULT_MSG_MAX_LEN		300
typedef enum
{
	eCasBcCtrlType_None				= 0,
	eCasBcCtrlType_Drm,
	eCasBcCtrlType_CasCardStatus,
	eCasBcCtrlType_BsAutoMsg,
	eCasBcCtrlType_CaLinkMsg,
	eCasBcCtrlType_DrmAgentMsg,
	eCasBcCtrlType_EndOfCase
} APKE_CasBcCtrlType_e;

typedef enum
{
	eCasBcEvent_None,						//	None
	eCasBcEvent_CasOK,
	eCasBcEvent_CasFail,
	eCasBcEvent_ShowBsAutoMsg,
	eCasBcEvent_HideBsAutoMsg,
	eCasBcEvent_ClearBsAutoMsg,
	eCasBcEvent_ShowCaLinkMsg,
	eCasBcEvent_End,						//	end

} APKE_CasBcCtrl_Event_e;

typedef enum
{
	eCasBcState_None,						//	None
	eCasBcState_CasOK,
	eCasBcState_CasFail,
	eCasBcState_End,						//	end

} APKE_CasBcCtrl_State_e;

typedef enum
{
	eCasBcRights_NoLicense,
	eCasBcRights_InvalidLicense,
	eCasBcRights_ValidLicense,
	eCasBcRights_End,
}APKE_CasBcCtrl_RightsState_e;


typedef	enum
{
	eCAS_BC_DoNothing

	// set
	,eCAS_BC_SetCmdStart							// !!!Do not Use this enumerator!!!
	,eCAS_BC_SetCmdEnd							// !!!Do not Use this enumerator!!!

	// get
	,eCAS_BC_GetCmdStart
	,eCAS_BC_GetCmdAccessCode
	,eCAS_BC_GetCmdCasStatus		// body - none
	,eCAS_BC_GetCmdCardStatus		// body - none
	,eCAS_BC_GetCmdBsAutoMsg		// body - none
	,eCAS_BC_GetCmdStopBsAutoMsg
	,eCAS_BC_GetCmdCaLinkMsg
	,eCAS_BC_GetCmdSendDrmMsg
	,eCAS_BC_GetCmdEnd		// body - none
} APKE_CAS_BC_Cmd_e;

typedef enum
{
	BCAS_CASYSTEMID_START = 0x0003,
	BCAS_CASYSTEMID_CCAS_3 = BCAS_CASYSTEMID_START,		/* Hitachi */
	BCAS_CASYSTEMID_CCAS_4 = 0x0004,		/* Pioneer */
	BCAS_CASYSTEMID_BCAS = 0x0005,		/* B-CAS */
	BCAS_CASYSTEMID_CCAS = 0x0006,		/* Panasonic */
	BCAS_CASYSTEMID_BSCONTROL = 0x0007,
	BCAS_CASYSTEMID_END = BCAS_CASYSTEMID_BSCONTROL,
	BCAS_CASYSTEMID_INVALID = 0xFFFF
} APKE_CAS_BC_CaSystemId_t;

typedef enum
{
	BCAS_CARDINFO_TYPE_BCAS_CARDID,
	BCAS_CARDINFO_TYPE_BCAS_DISCERN,
	BCAS_CARDINFO_TYPE_BCAS_GROUPID,
	BCAS_CARDINFO_TYPE_CCAS_CARDID,
	BCAS_CARDINFO_TYPE_CCAS_DISCERN,
	BCAS_CARDINFO_TYPE_CCAS_GROUPID,
} APKE_CAS_BC_CardInfoType_t;
typedef enum
{
	BCAS_AUTO_DISPLAY_CAN_DELETE		= 0x00,
	BCAS_AUTO_DISPLAY_CANNOT_DELETE	= 0x01,
	BCAS_AUTO_DISPLAY_CLEAR			= 0x02,
	BCAS_AUTO_DISPLAY_END,
} APKE_CAS_BC_AutoDisplayType_e;

typedef enum
{
	CAS_BC_AUTODISPLAY_X_LEFT		= 0x04,
	CAS_BC_AUTODISPLAY_X_CENTER	= 0x02,
	CAS_BC_AUTODISPLAY_X_RIGHT	= 0x01,
	CAS_BC_AUTODISPLAY_X_END		= 0,
} APKE_CAS_BC_AutoDisplayPositionX_t;

typedef enum
{
	CAS_BC_AUTODISPLAY_Y_HIGH	= 0x04,
	CAS_BC_AUTODISPLAY_Y_CENTER	= 0x02,
	CAS_BC_AUTODISPLAY_Y_LOW		= 0x01,
	CAS_BC_AUTODISPLAY_Y_END		= 0,
} APKE_CAS_BC_AutoDisplayPositionY_t;

typedef enum{
	eCAS_BC_DrmMsgCmd_Start
	,eCAS_BC_DrmMsgCmd_GetPpvViewRight
	,eCAS_BC_DrmMsgCmd_GetPpvStatus
	,eCAS_BC_DrmMsgCmd_BuyPpv
	,eCAS_BC_DrmMsgCmd_GetRetryOverStatus
	,eCAS_BC_DrmMsgCmd_End
} APKE_CAS_BC_DrmMsgCmd_e;

typedef	struct
{
	APKE_CasBcCtrl_Event_e	eLastReceivedEvent;
	APKE_CasBcCtrl_State_e	eCtrlState;
	APKE_CasBcCtrl_RightsState_e eRightsState;
	HUINT32				ulError;
	HUINT32				ulCaSystemId;
	HCHAR				szRightsIssuerURL[APKD_CAS_BC_MAX_RIGHTS_ERROR_URL];
}APKS_CASBC_CasStatus_t;

typedef	struct
{
	HBOOL				bBCasOk;
	HBOOL				bCCasOk;
	HCHAR				szBCasManufacturerId[APKD_CAS_BC_MANUFACTURERID_MAX_LEN];
	HCHAR				szCCasManufacturerId[APKD_CAS_BC_MANUFACTURERID_MAX_LEN];
	HCHAR				szBCasCardId[APKD_CAS_BC_CARD_ID_MAX_LEN];
	HCHAR				szCCasCardId[APKD_CAS_BC_CARD_ID_MAX_LEN];
	HUINT32				ulBCasGroupIdNum;
	HUINT32				ulCCasGroupIdNum;
	HCHAR				aszBCasGroupId[APKD_CAS_BC_GROUP_ID_MAX_NUM][APKD_CAS_BC_CARD_ID_MAX_LEN];
	HCHAR				aszCCasGroupId[APKD_CAS_BC_GROUP_ID_MAX_NUM][APKD_CAS_BC_CARD_ID_MAX_LEN];
}APKS_CASBC_CardStatus_t;

typedef	struct
{
	APKE_CasBcCtrl_Event_e				eLastEvent;
	HUINT32							nMsgId;
	APKE_CAS_BC_AutoDisplayType_e		msgType;
	APKE_CAS_BC_AutoDisplayPositionX_t	positionX;
	APKE_CAS_BC_AutoDisplayPositionY_t	positionY;
	HUINT32							msgLength;
	HCHAR							szBsAutoMsg[APKD_CAS_BC_AUTO_DISPLAY_MSG_MAX_LEN];
}APKS_CASBC_BSAutoMsgInfo_t;

typedef	struct
{
	HUINT32						nReqStopMsgId;
	HINT32						nReqResponse;
}APKS_CASBC_BSAutoStopMsgInfo_t;

typedef 	struct
{
	APKS_CASBC_BSAutoMsgInfo_t stBsAutoMsgInfo;
	APKS_CASBC_BSAutoStopMsgInfo_t stBsAutoMsgStopInfo;
}APKS_CASBC_BSAutoMsg_t;

typedef	struct
{
	APKE_CasBcCtrl_Event_e	eLastEvent;
	HUINT32 					tsId;
	HUINT32 					networkId;
	HUINT32 					serviceId;
	HUINT32 					msgLength;
	HUINT8   					szCaLinkMsg[APKD_CAS_BC_ALTERNATIVE_MSG_MAX_LEN];
}APKS_CASBC_CaLinkMsg_t;

typedef	struct{
	HUINT32 			msgId;
	HCHAR			szMsg[APKD_CAS_BC_SENDDRMMSG_MSG_MAX_LEN];
	HUINT32			caSystemId;
}APKS_CASBC_SendDrmMsgData_t;

typedef	struct{
	HUINT32 					 msgId;
	APKE_CAS_BC_DrmMsgCmd_e cmd;
	HUINT32					 resultCode;
	HCHAR					 szResultMsg[APKD_CAS_BC_DRMMSGRESULT_MSG_MAX_LEN];
}APKS_CASBC_DrmMsgResultData_t;

typedef	struct
{
	APKS_CASBC_SendDrmMsgData_t stSendDrmMsgData;
	APKS_CASBC_DrmMsgResultData_t stDrmMsgResultData;
}APKS_CASBC_DrmAgentMsg_t;

typedef	struct
{
	HUINT32				ulError;
	HUINT32				ulErrorState;
	HINT8				*pszContentId;
	HINT8				*pszCaSystemId;
	HINT8				*pszRightsIssuerURL;
} APKS_CASBC_DrmData_t;

typedef	struct
{
	HUINT32							nMsgId;
	APKE_CAS_BC_AutoDisplayType_e		msgType;
} APKS_CASBC_BsAutoMsgData_t;

// MHEG
typedef enum
{
	eMhegApkEvt_None,
	eMhegApkEvt_FlipWindow,
	eMhegApkEvt_LaunchApp,
	eMhegApkEvt_MhegAppState,
	eMhegApkEvt_End
}	APKE_MhegCtrl_Evt_e;

typedef enum
{
	eMhegApkEvt_MhegAppState_Stopped,
	eMhegApkEvt_MhegAppState_Loading,
	eMhegApkEvt_MhegAppState_Started,
	eMhegApkEvt_MhegAppState_Unloading
}	APKE_MhegCtrl_AppState_e;

// SQC Scenario japan
typedef enum{
	eHokaidoSaporo = 10,				// 0		hokaido saporo
	eHokaidoHakodate,					// 1		hokaido hakodate
	eHokaidoAsahigawa,				// 2		hokaido asahigawa
	eHokaidoObihiro,					// 3 		hokaido obihiro
	eHokaidoKusiro,						// 4		hokaido kusiro
	eHokadioKitami,						// 5		hokaido kitami
	eHokadioMuroran,					// 6		hokaido muroran
	eMiyagi,									// 9		miyagi
	eAkida,									// 10		akida
	eYamagata,							// 11	yamagata
	eIwate,									// 8		iwate
	eHukusima,							// 12	hukusima
	eAomori,								// 7		aomori
	eTokyo,									// 18	tokyo
	eKanakawa,							// 19	kanakawa
	eGunma,								// 15	gunma
	eIbaraki,								// 13	ibaraki
	eChiba,									// 17	chiba
	eTochigi,								// 14	tochigi
	eSaitama,								// 16	saitama
	eNagano,								// 25	nagano
	eNiigata,								// 20	niigata
	eYamanashi,							// 24	yamanashi
	eAichi,									// 28	aichi
	eiShikawa,								// 22	ishikawa
	eShizuoka,								// 27	shizuoka
	eHukui,									// 23	hukui
	eToyama,								// 21	toyama
	eMie,										// 29	mie
	eGihu,									// 26	gihu
	eOsaka,									// 32	osaka
	eKyoto,									// 31	kyoto
	eHyogo,									// 33	hyogo
	eWakayama,							// 35	wakayama
	eNara,									// 34	nara
	eShiga,									// 30	shiga
	eHiroshima,							// 39	hiroshima
	eOkayama,								// 38	okayama
	eShimane,								// 37	shimane
	eTottori,									// 36	tottori
	eYamaguchi,							// 40	yamaguchi
	eEhime,									// 43	ehime
	eKagawa,								// 42	kagawa
	eDokushima,							// 41	dokushima
	eKochi,									// 44	kochi
	eHukuoka,								// 45	hukuoka
	eKumamoto,							// 48	kumamoto
	eNagasaki,								// 47	nagasaki
	eKagoshima,							// 51	kagoshima
	eMiyazaki,								// 50	miyazaki
	eOita,									// 49	oita
	eSaga,									// 46	saga
	eOkinawa,								// 52	okinawa
	eTokyoEtcShima = 151,			// 53	tokyo_etc-shima		=> 151
	eKagoshimaEtcShima = 186,		// 54	kagoshima_etc-shima		=> 186

}APKE_JLABS_LocalArea_t;

typedef enum{
	eSubtitleMode_NotUse,
	eSubtitleMode_FirstLanguage,
	eSubtitleMode_SecondLanguage,
	eSubtitleMode_MAX,
}APKE_JLABS_SubtitleMode_t;

typedef enum{
	eSuperImposeMode_NotUse,
	eSuperImposeMode_FirstLanguage,
	eSuperImposeMode_SecondLanguage,
	eSuperImposeMode_MAX
}APKE_JLABS_SuperImposeMode_t;

typedef enum{
	eSetupLaunchMode_Normal,
	eSetupLaunchMode_Wizard,
	eSetupLaunchMode_Installer,
	eSetupLaunchMode_End,
}APKE_JLABS_SetupLaunchMode_t;

typedef enum{
	eDisplayMode3D_Only3DContent = 0,		/* ÄÁÅÙÃ÷¸¸ 3d ·Î 				*/
	eDisplayMode3D_Return2D =1,			/* 3d ÄÁÅÙÃ÷ Á¾·á ÈÄ 2d·Î º¹±Í 	*/
	eDsiplayMode3D_Only2D = 2,				/* 3dµµ 2d ¿µ»óÀ¸·Î Ç¥½Ã 			*/
}APKE_JLABS_DisplayMode_3D_t;

typedef enum{
	ePinUnlockOption_PowrOff,
	ePinUnlockOption_ChChange,
	ePinUnlockOption_End,
}APKE_JLABS_PinUnlockExpirationOption_t;

typedef enum{
	eAmPermissionLevel_0,
	eAmPermissionLevel_1,
	eAmPermissionLevel_2,
	eAmPermissionLevel_3,
	eAmPermissionLevel_END,
}APKE_JLABS_AMPermissionLevel_t;

typedef enum{
	eTuningRange_TV = 0,
	eTuningRange_Radio_Data = 1,
	eTuningRange_All = 2,
}APKE_JLABS_TuningRange_t;

typedef enum{
	eTimebarDispMode_Normal = 0,
	eTimebarDispMode_Mini = 1,
	eTimebarDispMode_Off = 2,
}APKE_JLABS_TimebarDispMode_t;

typedef enum {
	eChMaskResult_None = 0,
	eChMaskResult_Start = 1,
	eChMaskResult_StartOk = 2,
	eChMaskResult_StartFail = 3,
}APKE_JLABS_ChannelMaskResult_t;

typedef enum{
	eStandbyMode_None = 0,
	eStandbyMode_PowerON = 1,
	eStandbyMode_PowerOFF = 2,
}APKE_JLABS_StandbyMode_t;

typedef enum {
	eUploadViewlogDataResult_None = 0,
	eUploadViewlogDataResult_OK = 1,
	eUploadViewlogDataResult_FAIL = 2,
}APKE_JLABS_UploadViewlogDataResult_t;

typedef enum {
	eCancelViewlogDataResult_None = 0,
	eCancelViewlogDataResult_OK = 1,
	eCancelViewlogDataResult_FAIL = 2,
}APKE_JLABS_CancelViewlogDataResult_t;

typedef enum{
	eSysCtrl_Event_ChannelMaskDone = 0,
	eSysCtrl_Event_UserReqCallFail = 2,
	eSysCtrl_Event_UserReqCallOK = 3,
	eSysCtrl_Event_UserReqCallCanceled = 4,
	eSysCtrl_Event_InsertSetupInfoOK = 5,
	eSysCtrl_Event_InsertSetupInfoFAIL = 6,

	eSysCtrl_Event_None,
}APKE_JLABS_SysCtrlJapanEvent_t;

typedef enum{
	eSWDownloadState_None = 0,
	eSWDownloadState_DownloadNotYet = 1,
	eSWDownloadState_DownloadComplete = 2,
}APKE_JLABS_SWDownloadState_t;

typedef enum {
	eStartSettingInfoResult_None = 0,
	eStartSettingInfoResult_OK = 1,
	eStartSettingInfoResult_FAIL = 2,
}APKE_JLABS_StartSettingInfoResult_t;

typedef enum {
	eInsertSetupInfoResult_None = 0,
	eInsertSetupInfoResult_OK = 1,
	eInsertSetupInfoResult_FAIL = 2,
}APKE_JLABS_InsertSetupInfoResult_t;

#endif

#endif
