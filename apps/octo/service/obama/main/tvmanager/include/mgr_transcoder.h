#ifndef	_____MGR_TRANSCODER_H_____
#define	_____MGR_TRANSCODER_H_____

#include <octo_common.h>
#include <svc_si.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define TRANSCODING_FILE_LEN				256

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eMgrTrcd_SrcType_None,

	eMgrTrcd_SrcType_Live,
	eMgrTrcd_SrcType_Playback,

	eMgrTrcd_SrcType_EndOfCase
} MgrTrcd_SrcType_e;

typedef enum
{
	eMgrTrcd_DstType_None,

	eMgrTrcd_DstType_RingBuf,
	eMgrTrcd_DstType_File,

	eMgrTrcd_DstType_EndOfCase
} MgrTrcd_DstType_e;

typedef struct
{
	Handle_t					 hSvcHandle;
} MgrTrcd_StartSrc_Live_t;

typedef struct
{
	HUINT32						 ulContentId;
} MgrTrcd_StartSrc_Pb_t;

typedef struct
{
	Handle_t					 hRingBuf;
} MgrTrcd_StartDst_RingBuf_t;

typedef struct
{
	HCHAR						 szFileUrl[TRANSCODING_FILE_LEN];
} MgrTrcd_StartDst_File_t;

typedef struct
{
	DxVideoCodec_e	eVideoCodec;
	DxAudioCodec_e	eAudioCodec;

	HINT32			nAudioBitrateKbps;
	HINT32			nVideoBitrateKbps;
	HINT32			nHdVideoBitrateKbps;	// Live가 HD이면, 다른 Bitrate으로 필요로 하면 사용
	HINT32			nFrameRate;
	HINT32			nScreenWidth;
	HINT32			nScreenHeight;

	HBOOL			bInterlaceMode;

	DxAspectRatio_e eVideoAspectRatio;

	/*video-codec extension*/
	DxVideoEncodeProfile_e eVideoCodecProfile;
	DxVideoEncodeLevel_e eVideoCodecLevel;

	/*need probe-info from live*/
	HBOOL	bUsingLiveAudioCodec;	//(true : bypass, false : using eAudioCodec)
	HBOOL	bUsingLiveAspectRatio;	//(true : bypass, false : using eVideoAspectRatio)

} MgrTrcd_StreamProfile;

typedef struct
{
	MgrTrcd_SrcType_e		 eSrcType;
	union
	{
		MgrTrcd_StartSrc_Live_t		 stLive;
		MgrTrcd_StartSrc_Pb_t		 stPb;
	} unSrc;

	MgrTrcd_DstType_e	 	 eDstType;
	union
	{
		MgrTrcd_StartDst_RingBuf_t	 stRingBuf;
		MgrTrcd_StartDst_File_t		 stFile;
	} unDst;

	MgrTrcd_StreamProfile	 	stProfile;
	HBOOL						bPcrTsPacketInsert;
} MgrTrcd_Start_t;

/*******************************************************************/
/********************      Functions            ********************/
/*******************************************************************/

HERROR			MGR_TRANSCODER_Start (HUINT32 ulActionId, MgrTrcd_Start_t *pstStartArg);
HERROR			MGR_TRANSCODER_Stop (HUINT32 ulActionId);
HERROR 			MGR_TRANSCODER_GetMakeSectionSiSpec (SvcSi_MakeSectionSpec_t *pstSiSpec);



#endif /* _____MGR_TRANSCODER_H_____ */
