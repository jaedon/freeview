#ifndef __MGR_SAPIP_H__
#define __MGR_SAPIP_H__

#include <mgr_transcoder.h>

enum
{
	eMEVT_SATIP			= eMEVT_SATIP_START,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_SATIP_NOTIFY_START_FAILED,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_SATIP_NOTIFY_STARTED,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_SATIP_NOTIFY_START_STOPPED,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_SATIP_NOTIFY_UPDATE_PIDS,

};

typedef enum
{
	eMgrSatIpFailed_NOSIGNAL			= 0,
	eMgrSatIpFailed_CREATE				= 1,

} MgrSatIp_EndReasonType_e;

typedef struct
{
	DxVideoCodec_e			eVideoCodec;
	DxAudioCodec_e			eAudioCodec;

	HINT32					nAudioBitrateKbps;
	HINT32					nVideoBitrateKbps;
	HINT32					nHdVideoBitrateKbps;
	HINT32					nFrameRate;
	HINT32					nScreenWidth;
	HINT32					nScreenHeight;

	HBOOL					bInterlaceMode;

	DxAspectRatio_e			eVideoAspectRatio;

	/*video-codec extension*/
	DxVideoEncodeProfile_e	eVideoCodecProfile;
	DxVideoEncodeLevel_e	eVideoCodecLevel;

	/*need probe-info from live*/
	HBOOL	bUsingLiveAudioCodec;	/* (true : bypass, false : using eAudioCodec) */
	HBOOL	bUsingLiveAspectRatio;	/* (true : bypass, false : using eVideoAspectRatio) */

} MgrSatIp_TranscoderStreamProfile;

typedef struct
{
	HUINT16 					usVideoPid;
	HUINT16						usAudioPid;
	HUINT16						usPcrPid;
	DxVideoCodec_e				eVideoCodec;
	DxAudioCodec_e				eAudioCodec;

} MgrSatIp_TranscoderStreamSrc;

typedef struct
{
	HUINT16						usTsUniqId;
	HUINT16 					usTsId;
	HUINT16						usSvcId;
	HUINT16						usPmtPid;

} MgrSatIp_TranscoderStreamPSI;

typedef struct
{
	HBOOL								bTranscode;
	HBOOL								bPcrTsPacketInsert;
	MgrSatIp_TranscoderStreamSrc		stSrc;
	MgrSatIp_TranscoderStreamProfile 	stProfile;
	MgrSatIp_TranscoderStreamPSI		stPsi;
	DbSvc_Info_t						stSvcInfo;	/* The current service information */
	DbSvc_TsInfo_t						stTsInfo;		/* The current TS information including delivery type */
	Handle_t							hSvcHandle;
} MgrSatIp_TranscoderStreamArg_t;

typedef struct
{
	/* manager */
	HUINT32							ulRsvSlotId;

	/* input param */
	DxTuneParam_t					stTuneParam;

	HINT32							nPIDsCount;
	HUINT16							usPIDs[DxRSV_PIDTABLE_COUNT];


	/* output param */
	Handle_t						hRingBuf;

	/* SI */
	HBOOL							bPMTValid;

	/* DRM */
	HBOOL							bDrm;

	/* priority */
	DxRsvStream_AllocMode_e			eAllocMode;
	HUINT16							usPriority;

	/* transcoder param */
	MgrSatIp_TranscoderStreamArg_t	stStartArg;
}MgrSatIp_Setup_t;

HERROR			MGR_SATIP_Init (void);
HERROR			MGR_SATIP_Start (MgrSatIp_Setup_t *pstSetup, HUINT32 *pulActionId);
HERROR			MGR_SATIP_GetSatipSiSpec (SvcSi_SatipSpec_t *pstSiSpec);
HERROR			MGR_SATIP_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			MGR_SATIP_ChangePriority (HUINT32 ulActionId, HUINT16 usPriority);
HERROR			MGR_SATIP_ChangeTranscodingProfile (HUINT32 ulActionId, MgrSatIp_TranscoderStreamProfile stProfile);
HERROR			MGR_SATIP_Try (MgrSatIp_Setup_t *pstSetup);
HERROR			MGR_SATIP_Stop (HUINT32 ulActionId);
HBOOL 			MGR_SATIP_GetDrmFlag (HUINT32 ulActionId);

#endif /* __MGR_SAPIP_H__ */
