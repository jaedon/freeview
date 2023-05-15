#ifndef __MW_TRANSCODER__
#define __MW_TRANSCODER__

#define MAX_TRANS_FILENAME_LEN	256
#define MAX_TRANS_RAWSECTION_LEN	4096

typedef enum
{
	eSVC_TRANSCODER_SRCTYPE_None,

	eSVC_TRANSCODER_SRCTYPE_Live,
	eSVC_TRANSCODER_SRCTYPE_Tsr,
	eSVC_TRANSCODER_SRCTYPE_Pvr,
	eSVC_TRANSCODER_SRCTYPE_Media,
	eSVC_TRANSCODER_SRCTYPE_Ip,

	/*end*/
	eSVC_TRANSCODER_SRCTYPE_MAX

} SvcTranscoder_SourceType_e;

typedef enum
{
	eSVC_TRANSCODER_ENCODEMODE_None,

	eSVC_TRANSCODER_ENCODEMODE_AV,
	eSVC_TRANSCODER_ENCODEMODE_AUDIO,

	/*end*/
	eSVC_TRANSCODER_ENCODEMODE_MAX

} SvcTranscoder_EncodeMode_e;


typedef enum
{
	eSVC_TRANSCODER_OUTPUTTYPE_None,

	eSVC_TRANSCODER_OUTPUTTYPE_FILE,
	eSVC_TRANSCODER_OUTPUTTYPE_BUFFER,

	/*end*/
	eSVC_TRANSCODER_OUTPUTTYPE_MAX

} SvcTranscoder_OutputType_e;


enum
{
	eSEVT_TRANSCODER_NONE	= eSEVT_TRANSCODER_START,

	eSEVT_TRANSCODER_STARTED,
	eSEVT_TRANSCODER_START_FAILED,

	eSEVT_TRANSCODER_PROBE_START,
	eSEVT_TRANSCODER_PROBE_FAILED,

	eSEVT_TRANSCODER_STOPED,
	eSEVT_TRANSCODER_STOP_FAILED,

	eSEVT_TRANSCODER_END

}; /* Message to GWM */

typedef struct
{
	HUINT16	usVideoPid;
	HUINT16	usAudioPid;
	HUINT16	usPcrPid;

} SvcTranscoder_AvPid_t;

typedef struct
{
	DxVideoCodec_e	eVideoCodec;
	DxAudioCodec_e	eAudioCodec;
	HUINT8			ucVideoStreamType;	// for PMT
	HUINT8			ucAudioStreamType;	// for PMT

	HINT32 			nAudioBitrate_kbps;
	HINT32			nVideoBitrate_kbps;
	HINT32			nHdVideoBitrate_kbps;

	HINT32			nScreenWidth;
	HINT32			nScreenHeight;
	HINT32			nFrameRate;

	HBOOL			bIsInteraceMode;

	DxAspectRatio_e	eVideoAspectRatio;

	/*video-codec extension*/
	DxVideoEncodeProfile_e eVideoCodecProfile;
	DxVideoEncodeLevel_e eVideoCodecLevel;

	/*need probe-info from live*/
	HBOOL			bUsingLiveAudioCodec;	//(true : bypass, false : using eAudioCodec)
	HBOOL			bUsingLiveAspectRatio;	//(true : bypass, false : using eVideoAspectRatio)

} SvcTranscoder_Profile_t;


typedef struct
{
	Handle_t hParentAction; //transcoder을 사용하는 최 상위 Mgr의 actionHandle, for event trigger

	/**
	  * TRUE 경우 : PalTransc_ 사용
	  * FALSE경우 : PAL_Rec 사용
	**/
	HBOOL	bTranscode;	// N/A
	HBOOL	bPcrTsPacketInsert;	// profile-file 로 이동 필요함

	/*status*/
	SvcTranscoder_EncodeMode_e	eTransEncodeMode;
	SvcTranscoder_OutputType_e		eTransOutputType;
	SvcTranscoder_SourceType_e	eTransSourceType;

	/*resource id*/
	HUINT32 ulRecDeviceId;
	HUINT32 ulDeviceId;
	HUINT32 ulDemuxId;

	/*src*/
	SvcTranscoder_AvPid_t	stAvPid;
	DxVideoCodec_e	eVideoCodec;
	DxAudioCodec_e	eAudioCodec;

	/**/
	SvcTranscoder_Profile_t stProfile;

	/*
		DES/3DES Encryption
	*/
	HBOOL		 bEncryption;

	/*profile list idx*/
	HINT32 nTranscodeProfileId;	// -1 : none

	/*when output-type is file*/
	HINT8	acuFilename[MAX_TRANS_FILENAME_LEN];		// File 일 경우
	Handle_t		hRingBuf;							// Ring Buffer (MEM)일 경우

} SvcTranscoder_Settings_t;

typedef struct
{
	HUINT16			usPid;
	SiTableType_e	eSiType;

	/*raw section*/
	HUINT16	usFlag;
	HBOOL	bContinue;
	HUINT16 usPeriodTime;
	HINT32	ulRawSectionSize;
	HUINT8	aucRawSectionBuffer[MAX_TRANS_RAWSECTION_LEN];

} SvcTranscoder_PSITable_t;

extern HERROR	SVC_TRANSCODER_Init(PostMsgToMgrCb_t fnSvcTranscoderPostMsgCb);

extern HERROR	SVC_TRANSCODER_Open(Handle_t hAction, SvcTranscoder_Settings_t *pSettings, ApiSyncMode_t eApiSyncMode);
extern HERROR	SVC_TRANSCODER_StartService(Handle_t hAction, ApiSyncMode_t eApiSyncMode);
extern HERROR	SVC_TRANSCODER_StopService(Handle_t hAction, ApiSyncMode_t eApiSyncMode);

extern HERROR	SVC_TRANSCODER_ChangePid(Handle_t hAction, SvcTranscoder_AvPid_t *pPids, ApiSyncMode_t eApiSyncMode);
extern HERROR	SVC_TRANSCODER_ChangeProfile(Handle_t hAction, SvcTranscoder_Profile_t *pProfile, ApiSyncMode_t eApiSyncMode);
extern HERROR	SVC_TRANSCODER_SetPSITable(Handle_t hAction, SvcTranscoder_PSITable_t *pTable, ApiSyncMode_t eApiSyncMode);

/*PIS table 정보를 Transcoder Start 이후에 설정하는 경우 시작전 PSI 의 PID를 아래 함수로 등록 함.*/
extern HERROR	SVC_TRANSCODER_RegisterPSITable(Handle_t hAction, SvcTranscoder_PSITable_t *pTable, ApiSyncMode_t eApiSyncMode);

#endif
