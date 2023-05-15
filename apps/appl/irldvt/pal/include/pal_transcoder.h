/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_transcoder.h.
	@brief

	Description:  									\n
	Module: PAL/TRANSCODER					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef	___PAL_TRANSC_H___
#define	___PAL_TRANSC_H___

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define MAX_TRANS_RAWSECTION_LEN	4096
#define MAX_FILENAME_LEN	255
#define MAX_CRYPTO_KEY_LEN		16

typedef void (*PalTransc_RecMemCallback_t)(HUINT32 ulDeviceId, HUINT32 ulUniqueId,const void *pData, HUINT32 ulLength);
typedef void (*PalTransc_ProbeEventCallback_t)(HUINT32 ulEvent, HUINT32 ulDeviceId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
#if defined(SUPPORT_IDX_RECMEM)
typedef void (*PalTransc_IdxRecMemCallback_t)(HUINT32 ulDeviceId, HUINT32 ulUniqueId, unsigned long long ul64Length);
#endif
//--using profile
#define TRANSCODE_DEVICE_NAME_MAX_LEN	48

typedef HINT32	PalTransc_ProfileHandle_t;
typedef struct _PalTransc_Profile	PalTransc_Profile_t;


/*******************************************************************/
/********************      Enum     ********************/
/*******************************************************************/
typedef enum
{
	ePAL_TRANSC_PROBE_EVT_SEQUENCE_HEADER_INFO,
	ePAL_TRANSC_PROBE_EVT_PROBE_FAIL,
	ePAL_TRANSC_PROBE_EVT_PROBE_SUCCESS,

	/*end*/
	ePAL_TRANSC_PROBE_EVT_MAX,

} PalTransc_ProbeEvent_t;


typedef enum
{
	ePAL_TRANSC_SRCTYPE_NONE,
	ePAL_TRANSC_SRCTYPE_LIVE,
	ePAL_TRANSC_SRCTYPE_PVR,

	/*end*/
	ePAL_TRANSC_SRCTYPE_MAX

} PalTransc_SourceType_e;

typedef enum
{
	ePAL_TRANSC_ENCODEMODE_NONE,

	ePAL_TRANSC_ENCODEMODE_VIDEO,
	ePAL_TRANSC_ENCODEMODE_AUDIO,
	ePAL_TRANSC_ENCODEMODE_AV,

	/*end*/
	ePAL_TRANSC_ENCODEMODE_MAX

} PalTransc_EncodeMode_e;

typedef enum
{
	ePAL_TRANSC_PSITABLE_PAT,
	ePAL_TRANSC_PSITABLE_PMT,
	ePAL_TRANSC_PSITABLE_EIT,
	ePAL_TRANSC_PSITABLE_SIT,

	/*end*/
	ePAL_TRANSC_PSITABLE_MAX

} PalTransc_PSITable_e;

typedef enum
{
	ePAL_TRANSC_CONTAINERTYPE_NONE,
	ePAL_TRANSC_CONTAINERTYPE_TS,
	ePAL_TRANSC_CONTAINERTYPE_MP4,
	ePAL_TRANSC_CONTAINERTYPE_ASF,

	/*end*/
	ePAL_TRANSC_CONTAINERTYPE_MAX

} PalTransc_ContainerType_e;

typedef enum
{
	ePAL_TRANSC_OUTPUTTYPE_NONE,

	ePAL_TRANSC_OUTPUTTYPE_FILE,
	ePAL_TRANSC_OUTPUTTYPE_BUFFER,

	/*end*/
	ePAL_TRANSC_OUTPUTTYPE_MAX

} PalTransc_OutputType_e;

typedef enum
{
	ePAL_TRANSC_CRYPTOTYPE_NONE,

	ePAL_TRANSC_CRYPTOTYPE_3DES,
	ePAL_TRANSC_CRYPTOTYPE_AES,
	ePAL_TRANSC_CRYPTOTYPE_CURRENT,
	ePAL_TRANSC_CRYPTOTYPE_3DES_SECURED_KEY,
	ePAL_TRANSC_CRYPTOTYPE_AES_SECURED_KEY,

} PalTransc_CryptoType_t;


typedef enum
{
	PAL_KindOfClientDevice_Unknown 		= 0x00000000,

	/*
		ios
	*/
	PAL_KindOfClientDevice_IOS			= 0x00010000,
	PAL_KindOfClientDevice_IOS_HUMAX				= (PAL_KindOfClientDevice_IOS | 0x0001),
	PAL_KindOfClientDevice_IOS_IPAD1 				= (PAL_KindOfClientDevice_IOS | 0x0002),
	PAL_KindOfClientDevice_IOS_IPAD2 				= (PAL_KindOfClientDevice_IOS | 0x0003),
	PAL_KindOfClientDevice_IOS_IPAD3 				= (PAL_KindOfClientDevice_IOS | 0x0004),
	PAL_KindOfClientDevice_IOS_IPADMINI  	 		= (PAL_KindOfClientDevice_IOS | 0x0005),
	PAL_KindOfClientDevice_IOS_IPHONE3				= (PAL_KindOfClientDevice_IOS | 0x0006),
	PAL_KindOfClientDevice_IOS_IPHONE4				= (PAL_KindOfClientDevice_IOS | 0x0007),
	PAL_KindOfClientDevice_IOS_IPHONE5				= (PAL_KindOfClientDevice_IOS | 0x0008),

	PAL_KindOfClientDevice_IOS_ETC					= (PAL_KindOfClientDevice_IOS | 0xFFFF),

	/*
		android
	*/
	PAL_KindOfClientDevice_Android		= 0x00020000,
	PAL_KindOfClientDevice_Android_GALTAB7			= (PAL_KindOfClientDevice_Android | 0x0001),
	PAL_KindOfClientDevice_Android_GALTAB10			= (PAL_KindOfClientDevice_Android | 0x0002),
	PAL_KindOfClientDevice_Android_GALNOTE10		= (PAL_KindOfClientDevice_Android | 0x0003),
	PAL_KindOfClientDevice_Android_MOTOROLA_XOOM 	= (PAL_KindOfClientDevice_Android | 0x0004),
	PAL_KindOfClientDevice_Android_GAL2				= (PAL_KindOfClientDevice_Android | 0x0100),
	PAL_KindOfClientDevice_Android_GAL3				= (PAL_KindOfClientDevice_Android | 0x0101),
	PAL_KindOfClientDevice_Android_GALNOTE			= (PAL_KindOfClientDevice_Android | 0x0102),
	PAL_KindOfClientDevice_Android_GALNOTE2			= (PAL_KindOfClientDevice_Android | 0x0103),
	PAL_KindOfClientDevice_Android_VPLAYER			= (PAL_KindOfClientDevice_Android | 0x0104),
	PAL_KindOfClientDevice_Android_OTT				= (PAL_KindOfClientDevice_Android | 0x1000),

	PAL_KindOfClientDevice_Android_ETC				= (PAL_KindOfClientDevice_Android | 0xFFFF),

	/*
		window phone
	*/
	PAL_KindOfClientDevice_WindowPhone		= 0x00040000,

	PAL_KindOfClientDevice_WindowPhone_ETC			= (PAL_KindOfClientDevice_WindowPhone | 0xFFFF),


	/*
		window
	*/
	PAL_KindOfClientDevice_Window			= 0x00080000,
	PAL_KindOfClientDevice_Window_VLC				= (PAL_KindOfClientDevice_Window | 0x0001),
	PAL_KindOfClientDevice_Window_PotPlayer			= (PAL_KindOfClientDevice_Window | 0x0002),
	PAL_KindOfClientDevice_Window_ETC				= (PAL_KindOfClientDevice_Window | 0xFFFF),


	/*
		linux
	*/
	PAL_KindOfClientDevice_Linux			= 0x00100000,

	PAL_KindOfClientDevice_Linux_ETC				= (PAL_KindOfClientDevice_Linux | 0xFFFF),


	/*
		mask
	*/
	PAL_KindOfClientDevice_OSMask			= 0xFFFF0000,
	PAL_KindOfClientDevice_DeviceMask		= 0x0000FFFF,

	PAL_KindOfClientDevice_MAX				= 0xFFFFFFFF
} PalTransc_KindOfClientDevice_e;

typedef enum
{
	ePAL_TRANSC_PROBEMASK_WIDTH			=	(0x00000001),
	ePAL_TRANSC_PROBEMASK_HEIGHT			=	(0x00000001<<1),
	//ePAL_TRANSC_PROBEMASK_DISPLAYWIDTH	=	(0x00000001<<2),	//N/A
	//ePAL_TRANSC_PROBEMASK_DISPLAYHEIGHT	=	(0x00000001<<3),	//N/A
	ePAL_TRANSC_PROBEMASK_ASPECTRATIO		=	(0x00000001<<4),
	//ePAL_TRANSC_PROBEMASK_SAMASPECTRATIOX	=	(0x00000001<<5), // demand on Aspectratio
	//ePAL_TRANSC_PROBEMASK_SAMASPECTRATIOY	=	(0x00000001<<6), // demand on Aspectratio
	ePAL_TRANSC_PROBEMASK_FRAMERATE		=	(0x00000001<<7),
	ePAL_TRANSC_PROBEMASK_VIDEOFORMAT		=	(0x00000001<<8),
	ePAL_TRANSC_PROBEMASK_PROGRESSIVE		=	(0x00000001<<9),
	//ePAL_TRANSC_PROBEMASK_COLORPRIMARIE	=	(0x00000001<<10),	//N/A
	ePAL_TRANSC_PROBEMASK_CODECLEVEL		=	(0x00000001<<11),
	ePAL_TRANSC_PROBEMASK_CODECPROFILE	=	(0x00000001<<12)

	//EOF

} PalTransc_ProbeMask_b;

/*******************************************************************/
/********************      Structs   *************************/
/*******************************************************************/
typedef struct
{
	PalTransc_CryptoType_t eEncType;
	HUINT8 ucKey[MAX_CRYPTO_KEY_LEN];
	HUINT8 ucIv[MAX_CRYPTO_KEY_LEN];
	HUINT8 ucProtectionKey[MAX_CRYPTO_KEY_LEN];
} PalTransc_Crypto_t;

typedef struct _PalTransc_ProfileItem
{
	HINT32			nIndex;	/* profile에서 create하면서 부여 됨. */

	DxVideoCodec_e	eVideoCodec;
	DxAudioCodec_e	eAudioCodec;

	HINT32 			nAudioBitrateKbps;
	HINT32			nVideoBitrateKbps;
	HINT32			nHdVideoBitrateKbps;	// Live가 HD이면, 다른 Bitrate으로 필요로 하면 사용
	HINT32			nFrameRate;
	HINT32			nScreenWidth;
	HINT32			nScreenHeight;

	HBOOL			bInterlaceMode;

	DxAspectRatio_e				eVideoAspectRatio;

	/*video-codec extension*/
	DxVideoEncodeProfile_e	eVideoCodecProfile;
	DxVideoEncodeLevel_e	eVideoCodecLevel;

	/*need probe-info from live*/
	HBOOL	bUsingLiveAudioCodec;	//(true : bypass, false : using eAudioCodec)
	HBOOL	bUsingLiveAspectRatio;	//(true : bypass, false : using eVideoAspectRatio)

} PalTransc_ProfileItem_t;

typedef struct
{
	/*status*/
	PalTransc_SourceType_e	eTransSourceType;
	PalTransc_EncodeMode_e	eTransEncodeMode;
	PalTransc_OutputType_e		eTransOutputType;

	/*resource id*/
	HUINT32	ulRecDeviceId;
	HUINT32	ulDeviceId;
	HUINT32	ulDemuxId;

	/*src*/
	DxVideoCodec_e	eInputVideoFormat;
	DxAudioCodec_e	eInputAudioFormat;
	HUINT16	usVideoPid;
	HUINT16	usAudioPid;
	HUINT16	usPcrPid;
	HUINT32	ulPidBitmask;

	/*profile*/
	PalTransc_ProfileItem_t	stProfile;

	/*when output-type is file*/
	HBOOL	bTimestamp;
	HBOOL	bPcrTsPacketInsert;
	PalTransc_Crypto_t	stCrypto;

	HINT8	acuFilename[MAX_FILENAME_LEN];
	//N/A

} PalTransc_Setup_t;

typedef struct
{
	HUINT16 usFlag;
	HUINT16 usNum_sec;

	/*req*/
	HINT32	ulSize;
	HUINT8	aucRawSectionBuffer[MAX_TRANS_RAWSECTION_LEN];
}PalTransc_PSIData_t;

typedef struct
{
	HUINT16	usPid;
	HUINT32 ulIndexId;

	PalTransc_PSITable_e	eTransPSITableType;

	HBOOL bContinue;
	HUINT16 usPeriodTime;

	PalTransc_PSIData_t	stPSIData;

} PalTransc_PSI_t;

typedef struct
{
	HUINT32	ulGroupId;
	HUINT32	ulDeviceId;
	DxVideoCodec_e					eVideoFormat;
	DxAudioCodec_e					eAudioFormat;

	PalTransc_ContainerType_e	eTranscoderMuxType;

} PalTransc_Capability_t;

typedef struct
{
	HUINT16	usPcrPid;
	HUINT16	usVideoPid;
	HUINT16	usMainAudioPid;
}PalTransc_PidInfo_t;

/*******************************************************************/
/********************      Public Functions     **************************/
/*******************************************************************/

/*initalize*/
extern HERROR PAL_TRANSC_Init(void);
extern HERROR PAL_TRANSC_RegisterHandleRecMem( HUINT32 ulRecDeviceId,  PalTransc_RecMemCallback_t pfnTranscoderDataReceive);
extern HERROR PAL_TRANSC_UnRegisterHandleRecMem( HUINT32 ulRecDeviceId,  PalTransc_RecMemCallback_t pfnTranscoderDataReceive);
#if defined(SUPPORT_IDX_RECMEM)
extern HERROR PAL_TRANSC_RegisterHandleIdxRecMem( HUINT32 ulRecDeviceId,  PalTransc_IdxRecMemCallback_t pfnTranscoderDataReceive);
extern HERROR PAL_TRANSC_UnRegisterHandleIdxRecMem( HUINT32 ulRecDeviceId,  PalTransc_IdxRecMemCallback_t pfnTranscoderDataReceive);
#endif
/*getter*/
extern HERROR PAL_TRANSC_GetNumOfDevice(HUINT32 *pulNumOfDevice);
extern HERROR PAL_TRANSC_GetNumOfDeviceInPlay(HUINT32 *pulNumOfDevice);
extern HERROR PAL_TRANSC_GetCapability(HUINT32 ulDeviceId, PalTransc_Capability_t *pstCapability);

/*setup*/
extern HERROR PAL_TRANSC_Setup( HUINT32 ulDeviceId, PalTransc_Setup_t *pstSetting);
extern HERROR PAL_TRANSC_ChangeProfile( HUINT32 ulDeviceId, PalTransc_ProfileItem_t *pstProfile);

/*psi*/
extern HERROR PAL_TRANSC_GetPSI( HUINT32 ulDeviceId, HUINT16 usPid, PalTransc_PSIData_t  *pstPSIData);
extern HERROR PAL_TRANSC_SetPSI( HUINT32 ulDeviceId, PalTransc_PSI_t *pstPSI);
extern HERROR PAL_TRANSC_ResetPSI( HUINT32 ulDeviceId, HUINT16 usPid);

/*run*/
extern HERROR PAL_TRANSC_Start( HUINT32 ulDeviceId , HUINT32 *pulUniqueId);
extern HERROR PAL_TRANSC_Stop( HUINT32 ulDeviceId );
extern HERROR PAL_TRANSC_Restart( HUINT32 ulDeviceId );
extern HERROR PAL_TRANSC_ProbeStart( HUINT32 ulDeviceId);
extern HERROR PAL_TRANSC_ProbeStop( HUINT32 ulDeviceId);
extern HERROR PAL_TRANSC_ProbeApply( HUINT32 ulDeviceId, HUINT32	ulFlags);
extern HERROR PAL_TRANSC_ProbeFail( HUINT32 ulDeviceId);
extern HERROR PAL_TRANSC_RegisterHandleProbeEvent( HUINT32 ulDeviceId, PalTransc_ProbeEventCallback_t pfHandleProbeEvent);

#if defined(CONFIG_DEBUG)
extern HERROR PAL_TRANSC_RegisterCommand(void);
#endif


/*******************************************************************/
/********************      Debug     *******************************/
/*******************************************************************/

#endif	//___PAL_TRANSC_H___
