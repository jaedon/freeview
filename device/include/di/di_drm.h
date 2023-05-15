#ifndef __DI_DRM_H__
#define __DI_DRM_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"

#include "di_err.h"
#include "di_media20.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define DI_DRM_MAX_HTTP_CUSTOMHEADER   8	/**< Default Value of Max HTTP CustomHeader */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum {
	DI_DRM_TYPE_NONE,
	DI_DRM_TYPE_PLAYREADY,
	DI_DRM_TYPE_WMDRM,
	DI_DRM_TYPE_VIEWRIGHTWEB,
	DI_DRM_TYPE_MARLIN,
	DI_DRM_TYPE_PLAYREADY_25_BRCM,
	DI_DRM_TYPE_MAX
} DRM_Type;

/* for Marlin(OIPF) */
typedef void (*pfnDRM_OIPF_onDRMMEssageResult)(HUINT8 *msgID, HUINT8 *resultMsg, HUINT32 resultCode);
typedef void (*pfnDRM_OIPF_onDRMSystemStautsChange)(HUINT8 *DRMSystemID);
typedef void (*pfnDRM_OIPF_onDRMRightsError)(HUINT32 errorState, HUINT8 *ContentID, HUINT8 *DRMSystemID, HUINT8 *rightsIssuerURL);

/* for  Playready */
typedef void (*pfnDRM_PlayReady_KeyRetrvCallback) (void/*DRM_CIPHER_CONTEXT *cipher*/);
typedef void (*pfnDRM_PlayReady_ErrorEventCallback) (void *result);

/* for  Verimatrix */
typedef void (*pfnDRM_ViewRightWeb_SetKeyRetrievalStatusCallback)(void *userData, const char *url, unsigned long errCode);
typedef void (*pfnDRM_ViewRightWeb_KeyRetrvCallback) (const void *pUrl, void *pKey, HINT32 *puKeySize, DI_ERR_CODE *errCode);
typedef void (*pfnDRM_ViewRightWeb_ErrorEventCallback) (void *result);

typedef struct DI_DRM_HttpCustomHeader_s
{
	HUINT8 *pName;
	HUINT8 *pValue;
} DI_DRM_HttpCustomHeader_t;

typedef struct PLAYREADY {
	pfnDRM_PlayReady_KeyRetrvCallback KeyRetrvCallback;
	pfnDRM_PlayReady_ErrorEventCallback ErrorEventCallback;

	HUINT8	*pucWrmHeader;
	HINT32	nWrmHeaderLen;
	void* pstPlaypump;
	/* 필요한 내용 추가 필요함 */
	HINT8 *pPrivateKeyPath;			/* for https playready licence request.*/
	HINT8 *pCertificatePath;

	HUINT32						unHttpCustomHeaders;
	DI_DRM_HttpCustomHeader_t	tHttpCustomHeader[DI_DRM_MAX_HTTP_CUSTOMHEADER];
	
	HCHAR	*licenseURL;
} Playready_t;

typedef struct VIEWRIGHTWEB {
	pfnDRM_ViewRightWeb_SetKeyRetrievalStatusCallback  SetKeyRetrievalStatusCallback;
	pfnDRM_ViewRightWeb_KeyRetrvCallback KeyRetrvCallback;
	pfnDRM_ViewRightWeb_ErrorEventCallback ErrorEventCallback;

	HUINT8		szVCASBootAddr[128];

	/* 필요한 내용 추가 필요함 */
} ViewRightWeb_t;

#define MAX_MARLIN_MESSAGE_FILTERS 3

typedef struct MARLIN {
    pfnDRM_OIPF_onDRMMEssageResult onDRMMEssageResult;
    pfnDRM_OIPF_onDRMSystemStautsChange onDRMSystemStautsChange;
	pfnDRM_OIPF_onDRMRightsError onDRMRightsError;

	struct _MessageFilter 
	{
		HBOOL	bUsed;
		HUINT32 nPid;
		void	*pHMessage;
	}msg_t[MAX_MARLIN_MESSAGE_FILTERS];

	HBOOL bPAT_Enable;
	HBOOL bPMT_Enable;

	HUINT32 videoPID_Index;
	HUINT32 audioPID_Index;

	union { /* Marlin  message(xml) 에 대한 정의 필요 - xml 파일을 직접 내려줄 건지 해당 구조체를 만들어서 처리할 것인지 고려되어야 함. */
		HUINT8 *MarlinMessage;
		HUINT8 *MessageURL;
		HUINT8 *AccessURL;
    } msg;

	/* 필요한 내용 추가 필요함 */
} Marlin_t;

#if 0
typedef struct PLAYREADY_OUTPUTCONTROL {
	HUINT32 compressed_digital_audio_level;
	HUINT32 uncompressed_digital_audio_level;
	HUINT32 compressed_digital_video_level;
	HUINT32 uncompressed_digital_video_level;
	HUINT32 anlaog_video_level;
	HUINT8 anlaog_video_extension_guid[16];
} Playready_OutputControl_t;
#else
typedef struct PLAYREADY_MINIMUM_OUTPUT_PROTECTION_LEVELS
{
    HUINT16 wCompressedDigitalVideo;
    HUINT16 wUncompressedDigitalVideo;
    HUINT16 wAnalogVideo;
    HUINT16 wCompressedDigitalAudio;
    HUINT16 wUncompressedDigitalAudio;    
} PlayReady_Minimum_OutputControl_t;

typedef struct PLAYREADY_GUID {
    HUINT32 Data1;
    HUINT16 Data2;
    HUINT16 Data3;
    HUINT8 Data4[8];
} PlayReady_GUID;

typedef struct PLAYREADY_OPL_OUTPUT_IDS
{
    HUINT16 cIds;
    PlayReady_GUID *rgIds;
} PlayReady_OPL_OUTPUT_IDS;

typedef struct PlayReady_OUTPUT_PROTECTION_EX
{
    HUINT32 dwVersion;
    PlayReady_GUID guidId;
    HUINT32 dwConfigData;
} PlayReady_OUTPUT_PROTECTION_EX;

typedef PlayReady_OUTPUT_PROTECTION_EX PlayReady_VIDEO_OUTPUT_PROTECTION_EX;
typedef PlayReady_OUTPUT_PROTECTION_EX PlayReady_AUDIO_OUTPUT_PROTECTION_EX;

typedef struct PLAYREADY_VIDEO_OUTPUT_PROTECTION_IDS_EX
{
    HUINT32 dwVersion;
    HUINT16 cEntries;
    PlayReady_VIDEO_OUTPUT_PROTECTION_EX *rgVop;
} PlayReady_VIDEO_OUTPUT_PROTECTION_IDS_EX;

typedef struct PLAYREADY_AUDIO_OUTPUT_PROTECTION_IDS_EX
{
    HUINT32 dwVersion;
    HUINT16 cEntries;
    PlayReady_AUDIO_OUTPUT_PROTECTION_EX *rgAop;
} PlayReady_AUDIO_OUTPUT_PROTECTION_IDS_EX;

typedef struct PLAYREADY_OUTPUTCONTROL {
	HUINT32 dwVersion;
	PlayReady_Minimum_OutputControl_t minOPL;
	PlayReady_OPL_OUTPUT_IDS oplIdReserved;
	PlayReady_VIDEO_OUTPUT_PROTECTION_IDS_EX vopi;
	PlayReady_AUDIO_OUTPUT_PROTECTION_IDS_EX aopi;
} Playready_OutputControl_t;
#endif

typedef struct VIEWRIGHTWEB_OUTPUTCONTROL {
	void *temp;
} ViewRightWeb_OutputControl_t;

typedef struct BASICCCI {
	HUINT32 EPN;
	HUINT32 CCI;
	HUINT32 ICT; /* ImageConstraintToken*/
	HUINT32 DOT; /* DigitalOnlyToken */
	HUINT32 APS;
} BasicCCI_t;

typedef struct MARLIN_OUTPUTCONTROL {
	BasicCCI_t cci;
} Marlin_OutputControl_t;

typedef struct OUTPUTCONTTOL {
	DRM_Type drm_type;

	union {
		Playready_OutputControl_t playready;
		ViewRightWeb_OutputControl_t viewrightweb;
		Marlin_OutputControl_t marlin;
	} data;	
} OutputControl_t;

typedef void (*DI_DRM_OutputControlCallback_t)(SESSION_HANDLE media_session_handle, OutputControl_t *outputcontrol);
typedef void (*DI_DRM_LicenseDataCallback_t)(SESSION_HANDLE media_session_handle, HUINT8 *pParam1, HUINT32 unParam2);

typedef struct DRM_INFO {
    DRM_Type drm_type;
	DI_DRM_OutputControlCallback_t OutputControl;
	DI_DRM_LicenseDataCallback_t cbSetLicenseData;
	HUINT8* pucLicenseData;
	union {
		Playready_t playready;
		ViewRightWeb_t viewrightweb;
		Marlin_t marlin;
	} drm;
} DRM_Info_t;

typedef struct PLAYREADY_KEY_OFFER {
	void *temp;/*DRM_CIPHER_CONTEXT *cipher*/
} Playready_key_offer_t;

typedef struct VIEWRIGHTWEB_KEY_OFFER {
	void *pUrl;
} ViewRightWeb_key_offer_t;

typedef enum {
	DI_DRM_MARLIN_SETUP,
	DI_DRM_MARLIN_RELEASE,
	DI_DRM_MARLIN_SETDATA,
	DI_DRM_MARLIN_SETSPEED
}DI_DRM_MARLIN_CMD_KEY_e;


typedef struct MARLIN_KEY_OFFER {
	DI_DRM_MARLIN_CMD_KEY_e cmd;
	HINT32	speed;
	HINT8	*buffer;
	HINT32	message_length;

	void *pstPlayback;
	HUINT32 video_stream_id;
	HUINT32 audio_stream_id;

	void **videoPidChannel;
	void **audioPidChannel;

	void *stream;

	void *temp;
} Marlin_key_offer_t;

typedef struct PLAYREADY_KEY_RESULT {
	void *aucEncryptionKey;
} Playready_key_result_t;

typedef struct VIEWRIGHTWEB_KEY_RESULT {
	void *pKey;
	HINT32 *puKeySize;
} ViewRightWeb_key_result_t;

typedef struct MARLIN_KEY_RESULT {
	HUINT32 ecm_pid;
	void *temp;
} Marlin_key_result_t;


typedef struct DRM_KEY_INFO {
	union {
		Playready_key_offer_t playready;
		ViewRightWeb_key_offer_t viewrightweb;
		Marlin_key_offer_t marlin;
	} offer;
	
	union {
		Playready_key_result_t playready;
		ViewRightWeb_key_result_t viewrightweb;
		Marlin_key_result_t marlin;
	} result;
} DRM_Key_Info_t;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* DRM_Info_t에 Session을 생성한다. */
DI_ERR_CODE DI_DRM_CreateSession(SESSION_HANDLE *media_session_handle);

/* DRM_Info_t에 현재 Session의 Info를 가져온다. */
DI_ERR_CODE DI_DRM_GetInfo(SESSION_HANDLE media_session_handle, DRM_Info_t *drm_info);

/* DRM_Info_t에 필요한 Type, Header값 등을 정의 하고 Callback Function 을 install 한다. */
DI_ERR_CODE DI_DRM_ResigterCallback(SESSION_HANDLE media_session_handle, DRM_Info_t *drm_info);

/* DRM의 License를 외부로 부터 획득 한다. */
DI_ERR_CODE DI_DRM_GetLicense(SESSION_HANDLE media_session_handle);
DI_ERR_CODE DI_DRM_GetKey(SESSION_HANDLE media_session_handle, DRM_Key_Info_t *drm_key_info);
DI_ERR_CODE DI_DRM_GetDRMStatus(SESSION_HANDLE media_session_handle);

/* DRM_Info_t에 Session을 제거한다. */
DI_ERR_CODE DI_DRM_DestroySession(SESSION_HANDLE media_session_handle);
DI_ERR_CODE DI_DRM_RegisterOutputControlCallback(SESSION_HANDLE media_session_handle, DI_DRM_OutputControlCallback_t OutputControlCallback);
DI_ERR_CODE DI_DRM_UnregisterOutputControlCallback(SESSION_HANDLE media_session_handle);
DI_ERR_CODE DI_DRM_RegisterLicenseDataCallback(SESSION_HANDLE media_session_handle, DI_DRM_LicenseDataCallback_t callback);
DI_ERR_CODE DI_DRM_UnregisterLicenseDataCallback(SESSION_HANDLE media_session_handle);
DI_ERR_CODE DI_DRM_SetLicenseData(SESSION_HANDLE media_session_handle, HUINT8* pucLicenseData);

/* 각 DRM의 Specific한 Function */
/* Playready 2.5 BRCM : "SecureClock" 기능 동작, 해당 동작을 하지 않을 경우 License의 Setting에 따라서 실패(Bind Fail)가 발생 한다. */
DI_ERR_CODE DI_DRM_SecureClock(SESSION_HANDLE media_session_handle);

/* Playready 2.5 BRCM :  BRCM의 TEE와 SVP(SecureVideoPath)의 환경에서 Decrypt 동작*/
DI_ERR_CODE DI_DRM_DoDecrypt(SESSION_HANDLE media_session_handle, HUINT8 *pBuff, HINT32 nBuffSize, HUINT8 *pIv, HINT32 nIvSize);

/* Playready 2.5 BRCM : Data Copy to Secure Buffer*/
DI_ERR_CODE DI_DRM_PushPumpBuffer(SESSION_HANDLE media_session_handle, void *pvDst, void *pvSrc, HUINT32 unSize, HBOOL bflush);

void* DI_DRM_DataMemAlloc(HUINT32 unSize);
DI_ERR_CODE DI_DRM_DataMemFree(void *pvBuffer);

DI_ERR_CODE DI_DRM_GetStoredLicense(SESSION_HANDLE media_session_handle);


#endif /* !__DI_DRM_H__ */


