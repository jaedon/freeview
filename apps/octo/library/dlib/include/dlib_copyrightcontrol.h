/* $Header: $ */
#ifndef DLIB_COPYRIGHTCONTROL_H__
#define DLIB_COPYRIGHTCONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif



#define ___________________NAGRASES___________________

typedef enum
{
	eDxCopyrightControl_SesTrickModeCtrl_NoRestriction = 0x0,
	eDxCopyrightControl_SesTrickModeCtrl_NoSkip_FF2x_Visible = 0x1,
	eDxCopyrightControl_SesTrickModeCtrl_NoSkip_FF4x_Visible = 0x2,
	eDxCopyrightControl_SesTrickModeCtrl_NoSkip_FF8x_Visible = 0x3,
	eDxCopyrightControl_SesTrickModeCtrl_NoSkip_Jump_FF = 0x7
} eDxCopyrightControl_SesTrickModeCtrl_e;

typedef enum
{
	eDxCopyrightControl_SesComponentCtrl_NoRestriction = 0x0,
	eDxCopyrightControl_SesComponentCtrl_DownScale = 0x1,
	eDxCopyrightControl_SesComponentCtrl_DisableOutput = 0x2
} eDxCopyrightControl_SesComponentCtrl_e;

typedef enum
{
	eDxCopyrightControl_SesStorageRight_NoRestriction = 0x0,
	eDxCopyrightControl_SesStorageRight_NoMore90Min = 0x1,
	eDxCopyrightControl_SesStorageRight_NoRecording = 0x2,
	eDxCopyrightControl_SesStorageRight_ExpireSameDay = 0x3,
	eDxCopyrightControl_SesStorageRight_Expire3Day = 0x4,
	eDxCopyrightControl_SesStorageRight_Expire7Day = 0x5,
	eDxCopyrightControl_SesStorageRight_Expire60Day = 0x6
} eDxCopyrightControl_SesStorageRight_e;

// 향별 DRM Info
typedef struct
{
	HUINT8					ucControlByteA;
	HUINT8					ucControlByteB;
} DxCopyrightControl_NagraSes_t;


#define ___________________SES___________________

// 향별 DRM Info
typedef struct
{
	HUINT8					ucControlByteA;
	HUINT8					ucControlByteB;
} DxCopyrightControl_Ses_t;


#define ___________________NORDIG___________________


typedef enum
{
	eDxCopyrightControl_NordigContentProtectionLevel_ShallBeOff = 0,	// Protection MUST NOT be used.
	eDxCopyrightControl_NordigContentProtectionLevel_AsUserSet = 1,		// Protection is applied as user set (MENU)
											// Default value if no signalling.
	eDxCopyrightControl_NordigContentProtectionLevel_MandatoryForHd = 2,	// For HD, protection must be used.
											// For SD, protection is applied as user set.
	eDxCopyrightControl_NordigContentProtectionLevel_Mandatory = 3,		// Protection must be used.
} eDxCopyrightControl_NordigContentProtectionLevel_e;



typedef struct
{
	eDxCopyrightControl_NordigContentProtectionLevel_e	eContentProtectionLevel;
	HBOOL  bHD;		/* if current video is HD */
} DxCopyrightControl_Nordig_t;



#define ___________________CIPLUS___________________

// CI+ DRM 정보를 Metafile에 저장 여부를 확인 하기위하여 사용하는 enum
typedef enum
{
	eDxCopyrightControl_CiPlusDrmWriteInfo_None	= 0,
	eDxCopyrightControl_CiPlusDrmWriteInfo_Writing,
	eDxCopyrightControl_CiPlusDrmWriteInfo_Done
} eDxCopyrightControl_CiPlusDrmWriteInfo_e;

typedef enum
{
	eDxCopyrightControl_CiPlusCcRecInfo_RecordOk 		= 0,
	eDxCopyrightControl_CiPlusCcRecInfo_RecordWait,			// CC resource ver2 Specification 에서 다루는 Recording Start Protocol에 의한 recording content를 나타 냄
	eDxCopyrightControl_CiPlusCcRecInfo_RecordStart,
	eDxCopyrightControl_CiPlusCcRecInfo_RecordError,
//	eDxCopyrightControl_CiPlusCcRecInfo_RecordStop,

	eDxCopyrightControl_CiPlusCcRecInfo_Max 		= 7
} eDxCopyrightControl_CiPlusCcRecInfo_e;

typedef struct
{
	eDxCopyrightControl_CiPlusCcRecInfo_e			eCiPlusCcRecInfo;
} DxCopyrightControl_CiPlusCcRecCtrl_t;

typedef struct
{
	HUINT16						usProgramNum;
	HUINT8						aucLicenseData[1024];
	HUINT16						usLicenseLength;
	HUINT8						aucBindedCamId[8];
} DxCopyrightControl_CiPlusLicenseInfo_t;

typedef struct
{
	UNIXTIME					utTimeStamp;
	HUINT16						usProgramNum;
	HUINT8						ucAgeLimit;		// 나이 제한 (HUINT8 이 적당하지 않다면 바꿔도 됨!)
	HUINT8						aucPrivateData[15];
	HUINT8						aucBindedCamId[8];
} DxCopyrightControl_CiPlusParentalCtrlInfo_t;

typedef struct
{
	unsigned char 				ucMode;			// see CI_CC_URI_MODE_
	unsigned char 				ucAps;			// see CI_CC_URI_APS_
	unsigned char 				ucEmi;			// see CI_CC_URI_EMI_
	unsigned char 				ucIct;			// see CI_CC_URI_ICT_
	unsigned char 				ucRct;			// see CI_CC_URI_RCT_
	unsigned char 				ucDot;			// see CI_CC_URI_DOT_
	unsigned long 				ulMinutesForRL;		// 0 means unlimited retention
} DxCopyrightControl_CiPlusUriInfo_t;

// CI+ 동작에 있어 DRM 성격을 가지고 있는 동작에 대하여 각 동작마다 type을 달리하여 처리
typedef enum
{
	eDxCopyrightControl_CiPlusDrmType_Uri		= 0,
	eDxCopyrightControl_CiPlusDrmType_RecCtrl,
	eDxCopyrightControl_CiPlusDrmType_License,
	eDxCopyrightControl_CiPlusDrmType_ParentalCtrl,

	// 추가적인 처리가 필요한 경우 계속 추가 (MAX로 7개 까지 사용할 수 있음. 더 사용해야 할 경우 code 변경 필요)
	// Max 7개 사용하는 이유는 이전 CI MGR에서 DRM 정보를 올릴때 4byte에 모든 정보를 우겨넣기 위해서였음. 변경된 CI MGR에서는 제한 없이 만들 수 있다.
	// 이전 CI MGR ver.1 부분은 local_mwcutil_ConvertCiPlusDrmInfoToDrmMsgArg() 함수를 참고하면 왜 그래야만 했는지 알 수 있음.

	eDxCopyrightControl_CiPlusDrmType_Max		= 7
} eDxCopyrightControl_CiPlusDrmType_e;

typedef struct
{
	eDxCopyrightControl_CiPlusDrmType_e 		eCiPlusDrmType;
	HUINT8					ucCiPlusDrmInfoVer;

	union{
		DxCopyrightControl_CiPlusUriInfo_t				stCiPlusUriInfo;
		DxCopyrightControl_CiPlusCcRecCtrl_t				stCiPlusRecCtrl;
		DxCopyrightControl_CiPlusLicenseInfo_t			stCiPlusLicenseInfo;
		DxCopyrightControl_CiPlusParentalCtrlInfo_t	stCiPlusParentalCtrlInfo;
	} ciplusDrmInfo;
} DxCopyrightControl_CiPlus_t;



#define ___________________IRDETO___________________


typedef enum
{
	eDxCopyrightControl_IrdetoCopyControlType_Macrovision	= 0x01,	/* Macrovision/Digital/Anolog끼리는 동시 처리 가능. */
	eDxCopyrightControl_IrdetoCopyControlType_Digital    	= 0x02,	/* Macrovision/Digital/Anolog끼리는 동시 처리 가능. */
	eDxCopyrightControl_IrdetoCopyControlType_Analog     	= 0x04,	/* Macrovision/Digital/Anolog끼리는 동시 처리 가능. */
	eDxCopyrightControl_IrdetoCopyControlType_Uri	   		= 0x08,	/* Macrovision/Digital/Anolog/PvrDrm들과 동시에 처리하지 말자. */
	eDxCopyrightControl_IrdetoCopyControlType_PvrDrm		= 0x10	/* Macrovision/Digital/Anolog/Uri들과 동시에 처리하지 말자. */
}eDxCopyrightControl_IrdetoCopyControlType_e;

typedef enum
{
	eDxCopyrightControl_IrdetoCopyControlCCI_Free,
	eDxCopyrightControl_IrdetoCopyControlCCI_None,
	eDxCopyrightControl_IrdetoCopyControlCCI_Once,
	eDxCopyrightControl_IrdetoCopyControlCCI_Never
} eDxCopyrightControl_IrdetoCopyControlCCI_e;

typedef enum
{
	eDxCopyrightControl_IrdetoPvrCopyControl_Freely = 0,
	eDxCopyrightControl_IrdetoPvrCopyControl_NoFurtherCopy,
	eDxCopyrightControl_IrdetoPvrCopyControl_CopyOnce,
	eDxCopyrightControl_IrdetoPvrCopyControl_CopyNever
} eDxCopyrightControl_IrdetoPvrCopyControl_e;

typedef struct
{
	HUINT32				ulCRID;
	eDxCopyrightControl_IrdetoPvrCopyControl_e	eCopyControl;
	HUINT32				ulPlaybackStartTime;
	HUINT32				ulPlaybackDuration;
	HUINT8				ucNrOfPlayback;
}DxCopyrightControl_IrdetoDrmRightsRecord_t;

typedef struct
{
	HUINT16				usTypeMask;
	HUINT8				ucMacrovisionMode;
	eDxCopyrightControl_IrdetoCopyControlCCI_e	eDigitalCCi;
	eDxCopyrightControl_IrdetoCopyControlCCI_e	eAnalogCCi;
	/* PVR DRM관련 Data가 필요할 경우 추가. */

	HUINT32				ulCRID;
	HUINT32				ulRightsRecordId;
	HUINT32				ulSequenceRecordId;

	DxCopyrightControl_IrdetoDrmRightsRecord_t	stIrPvrDrmInfo;
}DxCopyrightControl_Irdeto_t;



#define ___________________VIACCESS___________________



typedef struct
{
	HBOOL	bRecordForbidden;
	HUINT16	bTsrForbidden;

// va_ui_adapt.h의 VA_UI_RECORD_INFO 참조
	HUINT32 ulPlaybackDuration;
	HUINT32 ulMaxNumberOfPlayback;
	HUINT8 szExpireDate[11];	// 3+3+4+1 = 11

// va_ui_adapt.h의 VA_UI_PLAYBACK_INFO 참조
	HUINT32 ulPlaybackCounter;
//	HUINT8 szRecordDate[20];	// 3+3+4+3+3+3+1 = 20
	HUINT8 szFirstPlaybackDate[20];	// 3+3+4+3+3+3+1 = 20
}DxCopyrightControl_Viaccess_t;



#define ___________________CONAX___________________

typedef enum
{
	eDxCopyrightControl_ConaxApsCopyCtrl_Off = 0,		// 0b00
	eDxCopyrightControl_ConaxApsCopyCtrl_BurstOff,		// 0b01, AGC process on, aplit burst off
	eDxCopyrightControl_ConaxApsCopyCtrl_Burst2Line,		// 0b10, AGC process on, 2 line split burst on
	eDxCopyrightControl_ConaxApsCopyCtrl_Burst4Line		// 0b11, AGC process on, 4 line split burst on
}eDxCopyrightControl_ConaxApsCopyCtrl_e;			// APS = Analogue Protection System

typedef enum
{
	eDxCopyrightControl_ConaxEmiCopyCtrl_Freely = 0,		// 0b00
	eDxCopyrightControl_ConaxEmiCopyCtrl_NoMore,		// 0b01, copy no more - no further copying is permitted
	eDxCopyrightControl_ConaxEmiCopyCtrl_Once,			// 0b10, one generation copy is permitted
	eDxCopyrightControl_ConaxEmiCopyCtrl_Never			// 0b11, Copying is prohibited
}eDxCopyrightControl_ConaxEmiCopyCtrl_e;			// EMI = Encruption Mode Indicator

typedef enum
{
	eDxCopyrightControl_ConaxICT_Off = 0,		// 0b0, no image constraint asserted
	eDxCopyrightControl_ConaxICT_On			// 0b1, Image constraint required (<520K, e.g. 960x540)
}eDxCopyrightControl_ConaxICT_e;				// ICT = Image Constrained Trigger

typedef enum
{
	eDxCopyrightControl_ConaxRedistributionCtrl_Off = 0,		// 0b0, no redistribution control asserted
	eDxCopyrightControl_ConaxRedistributionCtrl_On			// 0b1, Redistribution control asserted
}eDxCopyrightControl_ConaxRedistributionCtrl_e;


typedef enum
{
	// depends on EMI
	// ----- EMI == [ Copy Freely ] -----
	eDxCopyrightControl_ConaxRetentionLimit_CopyFree_NoLimit = 0,		// 0bxxx, no retention limit

	// ----- EMI == [ Copy Never ] or [ Copy No More ] -----
	eDxCopyrightControl_ConaxRetentionLimit_NoLimit,			// 0b000, no retention limit
	eDxCopyrightControl_ConaxRetentionLimit_1Week,			// 0b001, retention limit of 1 week allies
	eDxCopyrightControl_ConaxRetentionLimit_2Days,			// 0b010
	eDxCopyrightControl_ConaxRetentionLimit_1Day,			// 0b011
	eDxCopyrightControl_ConaxRetentionLimit_12Hours,			// 0b100
	eDxCopyrightControl_ConaxRetentionLimit_6Hours,			// 0b101
	eDxCopyrightControl_ConaxRetentionLimit_3Hours,			// 0b110
	eDxCopyrightControl_ConaxRetentionLimit_90Min,			// 0b111

	// ----- EMI == [ Copy Once ] -----
	eDxCopyrightControl_ConaxRetentionLimit_NoExpLimit,		// 0b000, no expiry limit
	eDxCopyrightControl_ConaxRetentionLimit_ExpDays_360,	// 0b001, Content expiry after 360 days applies
	eDxCopyrightControl_ConaxRetentionLimit_ExpDays_90,		// 0b010
	eDxCopyrightControl_ConaxRetentionLimit_ExpDays_30,		// 0b011
	eDxCopyrightControl_ConaxRetentionLimit_ExpDays_14,		// 0b100
	eDxCopyrightControl_ConaxRetentionLimit_ExpDays_7,		// 0b101
	eDxCopyrightControl_ConaxRetentionLimit_ExpDays_2,		// 0b110
	eDxCopyrightControl_ConaxRetentionLimit_ExpDays_1		// 0b111
}eDxCopyrightControl_ConaxRetentionLimit_e;

typedef enum
{
	eDxCopyrightControl_ConaxTrickPlayCtrl_NoRestriction = 0,		// 0b000,
	eDxCopyrightControl_ConaxTrickPlayCtrl_FF2X,			// 0b001, no jumping, fast foward limited to maximum 2x speed with visible content
	eDxCopyrightControl_ConaxTrickPlayCtrl_FF4X,			// 0b010, no jumping, fast foward limited to maximum 4x speed with visible content
	eDxCopyrightControl_ConaxTrickPlayCtrl_FF8X,			// 0b011, no jumping, fast foward limited to maximum 8x speed with visible content
	eDxCopyrightControl_ConaxTrickPlayCtrl_Reserved1,
	eDxCopyrightControl_ConaxTrickPlayCtrl_Reserved2,
	eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpFF,			// 0b110, no jumping or fast foward allowed
	eDxCopyrightControl_ConaxTrickPlayCtrl_NoJumpPauseFF		// 0b111 ,no pause, jumping or fast foward [note] contents shall not be stored in any time-shift buffer
} eDxCopyrightControl_ConaxTrickPlayCtrl_e;

// The maturity_rating value specifies any maturity rating information for the exported content.
typedef enum
{
	eDxCopyrightControl_ConaxMaturityRating_Off = 0,		// 0b000, no maturity level
	eDxCopyrightControl_ConaxMaturityRating_G,			// 0b001, Conax G: 0 year
	eDxCopyrightControl_ConaxMaturityRating_PG,			// 0b010, 10 year
	eDxCopyrightControl_ConaxMaturityRating_A,			// 0b011, 18 year
	eDxCopyrightControl_ConaxMaturityRating_X,			// 0b100, 10 year, erotic content
	eDxCopyrightControl_ConaxMaturityRating_XXX			// 0b101, 18 year, erotic content, PIN required
}eDxCopyrightControl_ConaxMaturityRating_e;				// MR = Maturity Rating

typedef enum
{
	eCxDisableAnalOutput_Off = 0,	// 0b0, analogue output allowed
	eCxDisableAnalOutput_On		// 0b1, analogue output forbidden
}eDxCopyrightControl_ConaxDisableAnalogOutput_e;		// disable analogue output

typedef struct
{
	eDxCopyrightControl_ConaxApsCopyCtrl_e		eApsCopyCtrl;
	eDxCopyrightControl_ConaxEmiCopyCtrl_e		eEmiCopyCtrl;
	eDxCopyrightControl_ConaxICT_e				eICT;
	eDxCopyrightControl_ConaxRedistributionCtrl_e	eRedistributionCtrl;
	eDxCopyrightControl_ConaxRetentionLimit_e		eRetentionLimit;
	eDxCopyrightControl_ConaxTrickPlayCtrl_e		eTrickPlayCtrl;
	eDxCopyrightControl_ConaxMaturityRating_e				eMR;
	eDxCopyrightControl_ConaxDisableAnalogOutput_e	eDisableAnalOutput;
}DxCopyrightControl_ConaxDrmInfo_t;



#define ___________________NAGRA___________________

typedef enum
{
	eDxCopyrightControl_NagraDrmType_CopyControl,
	eDxCopyrightControl_NagraDrmType_Uri,
	eDxCopyrightControl_NagraDrmType_Max
}eDxCopyrightControl_NagraDrmType_e;

typedef struct
{
	// TODO:
	HINT32 dummy;
}DxCopyrightControl_NagraCopyControl_t;

typedef struct
{
	HUINT8	ucProtocol;
	HUINT8	ucAps;
	HUINT8	ucEmi;
	HUINT8	ucIct;
	HUINT8	ucRct;
	HUINT8	ucRL;
	HUINT8	aucReserved[5];
}DxCopyrightControl_NagraUri_t;

typedef struct
{
	eDxCopyrightControl_NagraDrmType_e eType;

	union {
		DxCopyrightControl_NagraCopyControl_t	stCopyControl;
		DxCopyrightControl_NagraUri_t			stUri;
	} info;
}DxCopyrightControl_Nagra_t;

#define ___________________VERIMATRIX___________________

typedef enum
{
	eDxCopyrightControl_VerimatrixApsCopyCtrl_Off = 0,			// 0b00
	eDxCopyrightControl_VerimatrixApsCopyCtrl_BurstOff,		// 0b01, AGC process on, aplit burst off
	eDxCopyrightControl_VerimatrixApsCopyCtrl_Burst2Line,		// 0b10, AGC process on, 2 line split burst on
	eDxCopyrightControl_VerimatrixApsCopyCtrl_Burst4Line		// 0b11, AGC process on, 4 line split burst on
}eDxCopyrightControl_VerimatrixApsCopyCtrl_e;					// APS = Analogue Protection System

typedef enum
{
	eDxCopyrightControl_VerimatrixxEmiCopyCtrl_Freely = 0,		// 0b00
	eDxCopyrightControl_VerimatrixEmiCopyCtrl_NoMore,		// 0b01, copy no more - no further copying is permitted
	eDxCopyrightControl_VerimatrixEmiCopyCtrl_Once,			// 0b10, one generation copy is permitted
	eDxCopyrightControl_VerimatrixEmiCopyCtrl_Never			// 0b11, Copying is prohibited
}eDxCopyrightControl_VerimatrixEmiCopyCtrl_e;					// EMI = Encruption Mode Indicator

typedef enum
{
	eDxCopyrightControl_VerimatrixType_Iptv = 0,
	eDxCopyrightControl_VerimatrixType_Dvb,
	eDxCopyrightControl_VerimatrixType_Max
}eDxCopyrightControl_VerimatrixType_e;

typedef struct
{
	eDxCopyrightControl_VerimatrixType_e				usVmxType;

	/* 추후 DVB ONLY에 따라서 아래가 변경되어야함*/
	HBOOL				bHdcp;
	HBOOL				bDC; /* Dwight-Cavendish */
	eDxCopyrightControl_VerimatrixApsCopyCtrl_e eApsCopyCtrl;
	eDxCopyrightControl_VerimatrixEmiCopyCtrl_e eEmiCopyCtrl;

	/* PVR DRM관련 Data가 필요할 경우 추가. */
}DxCopyrightControl_Verimatrix_t;


#define ___________________COMMON___________________



typedef enum
{
	eDxCopyrightControl_RestrictLevel_None = 0,
	eDxCopyrightControl_RestrictLevel_Normal,
	eDxCopyrightControl_RestrictLevel_Max

} DxCopyrightControl_RestrictLevel_e;



typedef enum
{
	eDxCopyrightControl_DrmCcType_None 		= 0,

	eDxCopyrightControl_DrmCcType_Max
} DxCopyrightControl_DrmCcType_e;

typedef enum
{
	eDxCopyrightControl_CasCcType_None 		= 0,

	eDxCopyrightControl_CasCcType_NagraSes 		= 1,
	eDxCopyrightControl_CasCcType_CiPlus 			= 2,				/* ci plus 일반 */
	eDxCopyrightControl_CasCcType_Nagra 			= 3,				/* op가 아닌 nagra 일반 cop 처리 시 */
	eDxCopyrightControl_CasCcType_Irdeto			= 4,				/* op가 아닌 Irdeto Optional, OP 요구시 사용 */
	eDxCopyrightControl_CasCcType_Viaccess		= 5,
	eDxCopyrightControl_CasCcType_Verimatrix		= 6,
	eDxCopyrightControl_CasCcType_Conax		= 7,
	eDxCopyrightControl_CasCcType_Max
} DxCopyrightControl_CasCcType_e;

typedef enum
{
	eDxCopyrightControl_SiCcType_None 		= 0,

	eDxCopyrightControl_SiCcType_Ses 		= 1,
	eDxCopyrightControl_SiCcType_Nordig		= 2,

	eDxCopyrightControl_SiCcType_Max
} DxCopyrightControl_SiCcType_e;


typedef struct
{
	DxCopyrightControl_DrmCcType_e	 			eDrmCcType;

	// playready, VMX 등 drm soultion 관련.
	union
	{
		HUINT8					aucReserved[60];  // 정보 저장이 필요한 모듈과의 호환을 위해서 해당 자료구조의	최대값(60 byte)을 지정함
	} info;

} DxCopyrightControl_DrmCcInfo_t;

typedef struct
{
	DxCopyrightControl_CasCcType_e	 			eCasCcType;

	union
	{
		HUINT8									aucReserved[60];  // 정보 저장이 필요한 모듈과의 호환을 위해서 해당 자료구조의	최대값(60 byte)을 지정함

		DxCopyrightControl_NagraSes_t			stNagraDrmInfo_Ses;
		DxCopyrightControl_CiPlus_t 			stCiPlusDrmInfo;
		DxCopyrightControl_Irdeto_t 			stIrdetoDrmInfo;
		DxCopyrightControl_Viaccess_t			stViaccessDrmInfo;
		DxCopyrightControl_Nagra_t				stNagraDrmInfo;
		DxCopyrightControl_Verimatrix_t			stVmxDrmInfo;
		DxCopyrightControl_ConaxDrmInfo_t		stConaxDrmInfo;
		// 향이 추가될 때마다 위에 추가한다.
	} info;

} DxCopyrightControl_CasCcInfo_t;

typedef struct
{
	DxCopyrightControl_SiCcType_e				eSiCcType;

	union
	{
		HUINT8					aucReserved[60];  // 정보 저장이 필요한 모듈과의 호환을 위해서 해당 자료구조의	최대값(60 byte)을 지정함

		DxCopyrightControl_Ses_t				stSesDrmInfo;
		DxCopyrightControl_Nordig_t 			stNordigDrmInfo;
		// 향이 추가될 때마다 위에 추가한다.
	} info;

} DxCopyrightControl_SiCcInfo_t;


typedef struct
{
	HBOOL		bDrmCc;
	DxCopyrightControl_DrmCcInfo_t		stDrmCcInfo;

	HBOOL		bCasCc;
	DxCopyrightControl_CasCcInfo_t 		stCasCcInfo;

	HBOOL		bSiCc;
	DxCopyrightControl_SiCcInfo_t 		stSiCcInfo;

} DxCopyrightControl_t;




typedef union
{
	eDxCopyrightControl_SesTrickModeCtrl_e		eTrickRestrictMode_Ses;
	eDxCopyrightControl_ConaxTrickPlayCtrl_e	eTrickRestrictMode_Conax;

} DxCopyrightControl_TrickRestrictMode_u;



#ifdef __cplusplus
}
#endif

#endif // DLIB_COPYRIGHTCONTROL_H__


