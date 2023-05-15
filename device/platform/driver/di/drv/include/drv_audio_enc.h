/**
 * drv_audio.h
*/

#ifndef __DRV_AUDIO_ENC_H__
#define __DRV_AUDIO_ENC_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "di_audio.h"
#include "drv_audio.h"
#include "drv_err.h"
#include "di_transcoder.h"

#include "nexus_audio_decoder.h"
#include "nexus_hdmi_output.h"
#include "nexus_audio_mux_output.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

typedef struct
{
	DI_TRANSCODER_SOURCE_TYPE eTransType;
	DI_AUDIO_FORMAT nInputType;
	DI_AUDIO_FORMAT nOutputType;

	HUINT32 ulDemuxId;
	HBOOL bEncoding;
	HUINT32 ulMultiAudioCnt;
} DRV_AUDIO_ENC_Set_t;

typedef enum
{
	DRV_AUDIO_ENC_Stereo,
	DRV_AUDIO_ENC_LeftOnly,
	DRV_AUDIO_ENC_RightOnly,
	DRV_AUDIO_ENC_Mix
}DRV_AUDIO_ENC_STEREO_MODE;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error DRV_AUDIO_EncInit(void);
DRV_Error DRV_AUDIO_EncSetStereo(HUINT32 ulDevId, DRV_AUDIO_ENC_STEREO_MODE dualMode);
DRV_Error DRV_AUDIO_EncSettings(HUINT32 ulDevId, DRV_AUDIO_ENC_Set_t *Settings);
DRV_Error DRV_AUDIO_EncUnsettings(HUINT32 ulDevId);
DRV_Error DRV_AUDIO_EncStart(HUINT32 ulDevId, HUINT32 ulDelay);
DRV_Error DRV_AUDIO_EncStop(HUINT32 ulDevId);
DRV_Error DRV_AUDIO_EncRestart(HUINT32 ulDevId,HUINT32 ulDemuxId);
DRV_Error DRV_AUDIO_GetAudioMuxOutput(HUINT32 ulAudioDevId,HUINT32 ulIndex, NEXUS_AudioMuxOutputHandle *muxOutput);
DRV_Error DRV_AUDIO_GetEncDelayStatus(HUINT32 ulAudioDevId, unsigned *Dee);
DRV_Error DRV_AUDIO_MuxOutputShowStatus(HUINT32 ulAudioDevId);

#endif /* !__DRV_AUDIO_ENC_H__ */

