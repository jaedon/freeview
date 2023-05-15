/**************************************************************************************/
/**
 * @file oplavoutput.h
 *
 * AV Output Interfaces
 *
**************************************************************************************
**/

#ifndef __OPLAVOUTPUT_H__
#define __OPLAVOUTPUT_H__

#define OPLAVOUTPUT_MAX_NAME_LEN		64

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/
typedef enum {
	OPLAVOUTPUT_TYPE_NONE		= 0,
	OPLAVOUTPUT_TYPE_VIDEO,
	OPLAVOUTPUT_TYPE_AUDIO,
	OPLAVOUTPUT_TYPE_BOTH
} eOplAVOutputType;

typedef enum {
	OPLAVOUTPUT_VIDEO_MODE_NORMAL		= 0x00000001,
	OPLAVOUTPUT_VIDEO_MODE_STRETC		= 0x00000002,
	OPLAVOUTPUT_VIDEO_MODE_ZOOM			= 0x00000004
} eOplAVOutputVideoMode;

typedef enum {
	OPLAVOUTPUT_AUDIO_MODE_NONE			= 0x00000001,
	OPLAVOUTPUT_AUDIO_MODE_AC3			= 0x00000002,
	OPLAVOUTPUT_AUDIO_MODE_UNCOMPRESSED	= 0x00000004
} eOplAVOutputAudioMode;

typedef enum {
	OPLAVOUTPUT_AUDIO_RANGE_NORMAL		= 0x00000001,
	OPLAVOUTPUT_AUDIO_RANGE_NARROW		= 0x00000002,
	OPLAVOUTPUT_AUDIO_RANGE_WIDE		= 0x00000004
} eOplAVOutputAudioRange;

typedef enum {
	OPLAVOUTPUT_HDVIDEO_FORMAT_480I		= 0x00000001,
	OPLAVOUTPUT_HDVIDEO_FORMAT_480P		= 0x00000002,
	OPLAVOUTPUT_HDVIDEO_FORMAT_576I		= 0x00000004,
	OPLAVOUTPUT_HDVIDEO_FORMAT_576P		= 0x00000008,
	OPLAVOUTPUT_HDVIDEO_FORMAT_720P		= 0x00000010,
	OPLAVOUTPUT_HDVIDEO_FORMAT_1080I	= 0x00000011,
	OPLAVOUTPUT_HDVIDEO_FORMAT_1080P	= 0x00000012
} eOplAVOutputHDVideoFormat;

typedef enum {
	OPLAVOUTPUT_TV_ASPECT_RATIO_UNKNOWN	= 0x00000001,
	OPLAVOUTPUT_TV_ASPECT_RATIO_4_3		= 0x00000002,
	OPLAVOUTPUT_TV_ASPECT_RATIO_16_9	= 0x00000004
} eOplAVOutputAspectRatio;

/******************************************************************
	Typedef
******************************************************************/
typedef struct _OplAVOutput_t {
	char		szName[OPLAVOUTPUT_MAX_NAME_LEN];
	eOplAVOutputType	eType;
	BPL_BOOL		bEnabled;
	BPL_BOOL		bSubtitleEnabled;
	eOplAVOutputVideoMode		eVideoMode;
	eOplAVOutputAudioMode		eAudioMode;
	eOplAVOutputAudioRange		eAudioRange;
	eOplAVOutputHDVideoFormat	eVideoFormat;
	eOplAVOutputAspectRatio		eAspectRatio;
	unsigned int	supportedVideoModesMask;
	unsigned int	supportedAudioModesMask;
	unsigned int	supportedAudioRangesMask;
	unsigned int	supportedVideoFormatsMask;
	unsigned int	supportedAspectRatiosMask;
} OplAVOutput_t;

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus OPL_HMXOutput_GetNumOfDev(unsigned int *pnNumOfDev);
BPLStatus OPL_HMXOutput_IsSupportScart(unsigned int ulDevIdx, BPL_BOOL *pIsExist);
BPLStatus OPL_HMXOutput_IsSupportTvScart(unsigned int ulDevIdx, BPL_BOOL *pIsExist);
BPLStatus OPL_HMXOutput_IsSupportVcrScart(unsigned int ulDevIdx, BPL_BOOL *pIsExist);
BPLStatus OPL_HMXOutput_IsSupportComposite(unsigned int ulDevIdx, BPL_BOOL *pIsExist);
BPLStatus OPL_HMXOutput_IsSupportComponent(unsigned int ulDevIdx, BPL_BOOL *pIsExist);
BPLStatus OPL_HMXOutput_IsSupportRF(unsigned int ulDevIdx, BPL_BOOL *pIsExist);
BPLStatus OPL_HMXOutput_IsSupportHdmi(unsigned int ulDevIdx, BPL_BOOL *pIsExist);
BPLStatus OPL_HMXOutput_IsSupportSpdif(unsigned int ulDevIdx, BPL_BOOL *pIsExist);
BPLStatus OPL_HMXOutput_GetCurTVScartFormat(unsigned int ulDevIdx, char *pszTvScartFormat);
BPLStatus OPL_HMXOutput_SetCurTVScartFormat(unsigned int ulDevIdx, const char *pszTvScartFormat);
BPLStatus OPL_HMXOutput_GetSupportedTVScartFormat(unsigned int ulDevIdx, int *count, char ***pszTvScartFormat);
BPLStatus OPL_HMXOutput_GetCurVcrScartFormat(unsigned int ulDevIdx, char *pszVcrScartFormat);
BPLStatus OPL_HMXOutput_SetCurVcrScartFormat(unsigned int ulDevIdx, const char *pszVcrScartFormat);
BPLStatus OPL_HMXOutput_GetSupportedVCRScartFormat(unsigned int ulDevIdx, int *count, char ***pszVcrScartFormat);
BPLStatus OPL_HMXOutput_GetResolutionSelect(unsigned int ulDevIdx, char *pszResolutionSelect, int nMaxStrlen);
BPLStatus OPL_HMXOutput_SetResolutionSelect(unsigned int ulDevIdx, const char *pszResolution);
BPLStatus OPL_HMXOutput_GetSupportedResolutions(unsigned int ulDevIdx, int *count, char ***pszResolutions);
BPLStatus OPL_HMXOutput_GetTvAspectRatio(unsigned int ulDevIdx, char *pszTvAspectRatio);
BPLStatus OPL_HMXOutput_SetTvAspectRatio(unsigned int ulDevIdx, const char *pszTvAspectRatio);
BPLStatus OPL_HMXOutput_GetDisplayFormat(unsigned int ulDevIdx, char *pszDisplayFormat);
BPLStatus OPL_HMXOutput_SetDisplayFormat(unsigned int ulDevIdx, const char *pszDisplayFormat);
BPLStatus OPL_HMXOutput_GetWss2HdmiMode(unsigned int ulDevIdx, char *pszWss2HdmiMode);
BPLStatus OPL_HMXOutput_GetVideoStandard(unsigned int ulDevIdx, char *pszVideoStandard);
BPLStatus OPL_HMXOutput_GetAvAddtionalSignal(unsigned int ulDevIdx, char *pszAvAddtionalSignal);
BPLStatus OPL_HMXOutput_IsAudioDescriptionEnabled(unsigned int ulDevIdx, BPL_BOOL *pEnabled);
BPLStatus OPL_HMXOutput_IsSubtitleEnabled(unsigned int ulDevIdx, BPL_BOOL *pEnabled);
BPLStatus OPL_HMXOutput_IsHardOfHearingEnabled(unsigned int ulDevIdx, BPL_BOOL *pEnabled);
BPLStatus OPL_HMXOutput_SetAudioDescriptionEnabled(unsigned int ulDevIdx, BPL_BOOL enabled);
BPLStatus OPL_HMXOutput_SetSubtitleEnabled(unsigned int ulDevIdx, BPL_BOOL enabled);
BPLStatus OPL_HMXOutput_SetHardOfHearingEnabled(unsigned int ulDevIdx, BPL_BOOL enabled);

BPLStatus OPL_HMXAudio_GetNumOfDev(unsigned int *pnNumOfDev);
BPLStatus OPL_HMXAudio_GetName(unsigned int ulDevIdx, char* szName);
BPLStatus OPL_HMXAudio_GetVolumeMax(unsigned int ulDevIdx, int *pVolumeMax);
BPLStatus OPL_HMXAudio_GetVolumeMin(unsigned int ulDevIdx, int *pVolumeMin);
BPLStatus OPL_HMXAudio_GetLipSyncDelay(unsigned int ulDevIdx, int *pnLipSyncDelay);
BPLStatus OPL_HMXAudio_SetLipSyncDelay(unsigned int ulDevIdx, int nLipSyncDelay);

BPLStatus OPL_HMXAudio_GetSoundMode(unsigned int ulDevIdx, char* szSoundMode);
BPLStatus OPL_HMXAudio_SetSoundMode(unsigned int ulDevIdx, const char* szSoundMode);
BPLStatus OPL_HMXAudio_GetDualMono(unsigned int ulDevIdx, int *pnSoundMode);
BPLStatus OPL_HMXAudio_SetDualMono(unsigned int ulDevIdx,int nSoundMode);

BPLStatus OPL_HMXAudio_GetSpdifAudioOutputMode(unsigned int ulDevIdx, char* szSpdifOutput);
BPLStatus OPL_HMXAudio_SetSpdifAudioOutputMode(unsigned int ulDevIdx,const char* szSpdifOutput);
BPLStatus OPL_HMXAudio_GetHdmiAudioOutputMode(unsigned int ulDevIdx, char* szHdmiOutput);
BPLStatus OPL_HMXAudio_SetHdmiAudioOutputMode(unsigned int ulDevIdx,const char* szHdmiOutput);
BPLStatus OPL_HMXAudio_GetDigitalAudioOutputMode(unsigned int ulDevIdx, char* szDigitalOutput);
BPLStatus OPL_HMXAudio_SetDigitalAudioOutputMode(unsigned int ulDevIdx,const char* szDigitalOutput);

BPLStatus OPL_HMXAudio_GetDigitalAudioOutputCodec(unsigned int ulDevIdx, char* szCodec);
BPLStatus OPL_HMXAudio_SetDigitalAudioOutputCodec(unsigned int ulDevIdx,const char* szCodec);

#ifdef __cplusplus
};

class OplHmxOutput {
private:
	unsigned int		ulDevIdx;

public:
	OplHmxOutput(unsigned int ulDevIdx)
	{
		ulDevIdx = ulDevIdx;
	}
	virtual ~OplHmxOutput()
	{
	}
	static bool getNumOfDev(unsigned int *pnNumOfDev)
	{
		return OPL_HMXOutput_GetNumOfDev(pnNumOfDev);
	}

	// related with existance.
	static bool IsSupportScart(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSupportScart(ulDevIdx, &IsExist); return IsExist; }
	static bool IsSupportTvScart(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSupportTvScart(ulDevIdx, &IsExist); return IsExist; }
	static bool IsSupportVcrScart(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSupportVcrScart(ulDevIdx, &IsExist); return IsExist; }
	static bool IsSupportComposite(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSupportComposite(ulDevIdx, &IsExist); return IsExist; }
	static bool IsSupportComponent(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSupportComponent(ulDevIdx, &IsExist); return IsExist; }
	static bool IsSupportRF(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSupportRF(ulDevIdx, &IsExist); return IsExist; }
	static bool IsSupportHdmi(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSupportHdmi(ulDevIdx, &IsExist); return IsExist; }
	static bool IsSupportSpdif(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSupportSpdif(ulDevIdx, &IsExist); return IsExist; }

	// related with SCART
	static bool getCurTvScartFormat(unsigned int ulDevIdx, char *pszTvScartFormat)
	{ return (OPL_HMXOutput_GetCurTVScartFormat(ulDevIdx, pszTvScartFormat) == BPL_STATUS_OK); }
	static bool setCurTvScartFormat(unsigned int ulDevIdx, const char *pszTvScartFormat)
	{ return (OPL_HMXOutput_SetCurTVScartFormat(ulDevIdx, pszTvScartFormat) == BPL_STATUS_OK); }
	static bool getSupportedTvScartFormat(unsigned int ulDevIdx, int *count, char ***pszSupportedScartFormat)
	{ return (OPL_HMXOutput_GetSupportedTVScartFormat(ulDevIdx, count, pszSupportedScartFormat) == BPL_STATUS_OK); }
	static bool getCurVcrScartFormat(unsigned int ulDevIdx, char *pszVcrScartFormat)
	{ return (OPL_HMXOutput_GetCurVcrScartFormat(ulDevIdx, pszVcrScartFormat) == BPL_STATUS_OK); }
	static bool setCurVcrScartFormat(unsigned int ulDevIdx, const char *pszVcrScartFormat)
	{ return (OPL_HMXOutput_SetCurVcrScartFormat(ulDevIdx, pszVcrScartFormat) == BPL_STATUS_OK); }
	static bool getSupportedVcrScartFormat(unsigned int ulDevIdx, int *count, char ***pszSupportedScartFormat)
	{ return (OPL_HMXOutput_GetSupportedVCRScartFormat(ulDevIdx, count, pszSupportedScartFormat) == BPL_STATUS_OK); }

	// related with resolution.
	static bool getCurResolution(unsigned int ulDevIdx, char *pszResolution, int nMaxStrlen)
	{ return (OPL_HMXOutput_GetResolutionSelect(ulDevIdx, pszResolution, nMaxStrlen) == BPL_STATUS_OK); }
	static bool setCurResolution(unsigned int ulDevIdx, const char *pszResolution)
	{ return (OPL_HMXOutput_SetResolutionSelect(ulDevIdx, pszResolution) == BPL_STATUS_OK); }
	static bool getSupportedResolutions(unsigned int ulDevIdx, int *count, char ***pszSupportedResolutions)
	{ return (OPL_HMXOutput_GetSupportedResolutions(ulDevIdx, count, pszSupportedResolutions) == BPL_STATUS_OK); }

	// related with TV Aspect Ratio
	static bool getTvAspectRatio(unsigned int ulDevIdx, char *pszTvAspectRatio)
	{ return (OPL_HMXOutput_GetTvAspectRatio(ulDevIdx, pszTvAspectRatio) == BPL_STATUS_OK); }
	static bool setTvAspectRatio(unsigned int ulDevIdx, const char *pszTvAspectRatio)
	{ return (OPL_HMXOutput_SetTvAspectRatio(ulDevIdx, pszTvAspectRatio) == BPL_STATUS_OK); }

	// related with Display Format
	static bool getDisplayFormat(unsigned int ulDevIdx, char *pszDisplayFormat)
	{ return (OPL_HMXOutput_GetDisplayFormat(ulDevIdx, pszDisplayFormat) == BPL_STATUS_OK); }
	static bool setDisplayFormat(unsigned int ulDevIdx, const char *pszDisplayFormat)
	{ return (OPL_HMXOutput_SetDisplayFormat(ulDevIdx, pszDisplayFormat) == BPL_STATUS_OK); }

	// related with Addtional things...
	static bool getWss2HdmiMode(unsigned int ulDevIdx, char *pszWss2HdmiMode)
	{ return (OPL_HMXOutput_GetWss2HdmiMode(ulDevIdx, pszWss2HdmiMode) == BPL_STATUS_OK); }
	static bool getVideoStandard(unsigned int ulDevIdx, char *pszVideoStandard)
	{ return (OPL_HMXOutput_GetVideoStandard(ulDevIdx, pszVideoStandard) == BPL_STATUS_OK); }
	static bool getAvAddtionalSignal(unsigned int ulDevIdx, char *pszAvAddtionalSignal)
	{ return (OPL_HMXOutput_GetAvAddtionalSignal(ulDevIdx, pszAvAddtionalSignal) == BPL_STATUS_OK); }

	static bool IsAudioDescriptionEnabled(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsAudioDescriptionEnabled(ulDevIdx, &IsExist); return IsExist; }
	static bool IsSubtitleEnabled(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsSubtitleEnabled(ulDevIdx, &IsExist); return IsExist; }
	static bool IsHardOfHearingEnabled(unsigned int ulDevIdx)
	{ BPL_BOOL IsExist; OPL_HMXOutput_IsHardOfHearingEnabled(ulDevIdx, &IsExist); return IsExist; }

	static bool setAudioDescriptionEnabled(unsigned int ulDevIdx, bool enabled)
	{ return (OPL_HMXOutput_SetAudioDescriptionEnabled(ulDevIdx, enabled) == BPL_STATUS_OK); }
	static bool setSubtitleEnabled(unsigned int ulDevIdx, bool enabled)
	{ return (OPL_HMXOutput_SetSubtitleEnabled(ulDevIdx, enabled) == BPL_STATUS_OK); }
	static bool setHardOfHearingEnabled(unsigned int ulDevIdx, bool enabled)
	{ return (OPL_HMXOutput_SetHardOfHearingEnabled(ulDevIdx, enabled) == BPL_STATUS_OK); }
};

class OplHmxAudio {
private:
	unsigned int		ulDevIdx;

public:
	OplHmxAudio(unsigned int ulDevIdx)
	{
		ulDevIdx = ulDevIdx;
	}
	virtual ~OplHmxAudio()
	{
	}
	static bool getNumOfDev(unsigned int *pnNumOfDev)
	{
		return OPL_HMXAudio_GetNumOfDev(pnNumOfDev);
	}
	static bool getName(unsigned int ulDevIdx,char *szName)
	{ return (OPL_HMXAudio_GetName(ulDevIdx, szName) == BPL_STATUS_OK); }

	//Volume
	static bool getVolumeMax(unsigned int ulDevIdx, int *pVolumeMax)
	{ return (OPL_HMXAudio_GetVolumeMax(ulDevIdx, pVolumeMax) == BPL_STATUS_OK); }
	static bool getVolumeMin(unsigned int ulDevIdx, int *pVolumeMin)
	{ return (OPL_HMXAudio_GetVolumeMin(ulDevIdx, pVolumeMin) == BPL_STATUS_OK); }

	// LipSync
	static bool getLipSyncDelay(unsigned int ulDevIdx, int *pnLipSyncDelay)
	{ return (OPL_HMXAudio_GetLipSyncDelay(ulDevIdx, pnLipSyncDelay) == BPL_STATUS_OK); }
	static bool setLipSyncDelay(unsigned int ulDevIdx, int nLipSyncDelay)
	{ return (OPL_HMXAudio_SetLipSyncDelay(ulDevIdx, nLipSyncDelay) == BPL_STATUS_OK); }

	// Sound Mode
	static bool getSoundMode(unsigned int ulDevIdx, char *szSoundMode)
	{ return (OPL_HMXAudio_GetSoundMode(ulDevIdx, szSoundMode) == BPL_STATUS_OK);}
	static bool setSoundMode(unsigned int ulDevIdx, const char *szSoundMode)
	{ return (OPL_HMXAudio_SetSoundMode(ulDevIdx, szSoundMode) == BPL_STATUS_OK);}


	// Dual Mono
	static bool getDualMono(unsigned int ulDevIdx, int *pnSoundMode)
	{ return (OPL_HMXAudio_GetDualMono(ulDevIdx, pnSoundMode) == BPL_STATUS_OK);}
	static bool setDualMono(unsigned int ulDevIdx, int nSoundMode)
	{ return (OPL_HMXAudio_SetDualMono(ulDevIdx, nSoundMode) == BPL_STATUS_OK);}

	// Digital Output Mode
	static bool getSpdifAudioOutputMode(unsigned int ulDevIdx, char *szSpdifOutputMode)
	{ return (OPL_HMXAudio_GetSpdifAudioOutputMode(ulDevIdx, szSpdifOutputMode) == BPL_STATUS_OK); }
	static bool setSpdifAudioOutputMode(unsigned int ulDevIdx, const char *szSpdifOutputMode)
	{ return (OPL_HMXAudio_SetSpdifAudioOutputMode(ulDevIdx, szSpdifOutputMode) == BPL_STATUS_OK); }
	static bool getHdmiAudioOutputMode(unsigned int ulDevIdx, char *szHdmiOutputMode)
	{ return (OPL_HMXAudio_GetHdmiAudioOutputMode(ulDevIdx, szHdmiOutputMode) == BPL_STATUS_OK); }
	static bool setHdmiAudioOutputMode(unsigned int ulDevIdx, const char *szHdmiOutputMode)
	{ return (OPL_HMXAudio_SetHdmiAudioOutputMode(ulDevIdx, szHdmiOutputMode) == BPL_STATUS_OK); }

	static bool getDigitalOutputMode(unsigned int ulDevIdx, char *szDigitalOutputMode)
	{ return (OPL_HMXAudio_GetDigitalAudioOutputMode(ulDevIdx, szDigitalOutputMode) == BPL_STATUS_OK); }
	static bool setDigitalOutputMode(unsigned int ulDevIdx, const char *szDigitalOutputMode)
	{ return (OPL_HMXAudio_SetDigitalAudioOutputMode(ulDevIdx, szDigitalOutputMode) == BPL_STATUS_OK); }

	// Digital Output Codec
	static bool getDigitalOutputCodec(unsigned int ulDevIdx, char *szDigitalOutputCodec)
	{  return (OPL_HMXAudio_GetDigitalAudioOutputCodec(ulDevIdx, szDigitalOutputCodec) == BPL_STATUS_OK); }
	static bool setDigitalOutputCodec(unsigned int ulDevIdx, const char *szDigitalOutputCodec)
	{ return (OPL_HMXAudio_SetDigitalAudioOutputCodec(ulDevIdx, szDigitalOutputCodec) == BPL_STATUS_OK); }

	// Transcoding Mode.
	static bool getTranscodingMode(unsigned int ulDevIdx, char *szTranscodingMode)
	{ return true; /*return (OPL_HMXAudio_SetLipSyncDelay(ulDevIdx, nLipSyncDelay) == BPL_STATUS_OK); */}
	static bool setTranscodingMode(unsigned int ulDevIdx, const char *szTranscodingMode)
	{ return true; /*return (OPL_HMXAudio_SetLipSyncDelay(ulDevIdx, nLipSyncDelay) == BPL_STATUS_OK); */}

	// Transcoding flag
	static bool IsTranscodingEnabled(unsigned int ulDevIdx)
	{ return true; /*return (OPL_HMXAudio_SetLipSyncDelay(ulDevIdx, nLipSyncDelay) == BPL_STATUS_OK); */}

	// Supported things..
	static bool getSupportedSoundOutputModes(unsigned int ulDevIdx, int *count, char ***pszSupportedSoundOutputModes)
	{ return true;/*return (OPL_HMXOutput_GetSupportedResolutions(ulDevIdx, count, pszSupportedResolutions) == BPL_STATUS_OK);*/ }
	static bool getSupportedDigitalOutputCodecs(unsigned int ulDevIdx, int *count, char ***pszSupportedDigitalOutputCodecs)
	{ return true;/*return (OPL_HMXOutput_GetSupportedResolutions(ulDevIdx, count, pszSupportedResolutions) == BPL_STATUS_OK);*/ }
	static bool getSupportedDigitalOutputModes(unsigned int ulDevIdx, int *count, char ***pszSupportedDigitalOutputModes)
	{ return true;/*return (OPL_HMXOutput_GetSupportedResolutions(ulDevIdx, count, pszSupportedResolutions) == BPL_STATUS_OK);*/ }
	static bool getSupportedTranscodingModes(unsigned int ulDevIdx, int *count, char ***pszSupportedTranscodingModes)
	{ return true;/*return (OPL_HMXOutput_GetSupportedResolutions(ulDevIdx, count, pszSupportedResolutions) == BPL_STATUS_OK);*/ }

};

#endif

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#ifdef __cplusplus
};

/******************************************************************
	class OplAVOutput
******************************************************************/
typedef OplAVOutput_t	OplAVOutput;

#endif	/* __cplusplus */

#endif	/* __OPLAVOUTPUT_H__ */



