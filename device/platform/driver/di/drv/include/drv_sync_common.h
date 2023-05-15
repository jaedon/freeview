#ifndef __DRV_SYNC_COMMON_H__
#define __DRV_SYNC_COMMON_H__

typedef enum {
	DRV_SYNC_VID=0,
	DRV_SYNC_AUDMAIN,
	DRV_SYNC_AUDSUB,
	DRV_SYNC_AUDSPDIF
} DRV_SYNC_AV;

typedef struct DRV_SYNC_AV_DECODER 
{
	NEXUS_VideoDecoderHandle hVidDecoder;
	NEXUS_AudioDecoderHandle hAudMainDecoder;
	NEXUS_AudioDecoderHandle hAudSubDecoder;
	NEXUS_AudioDecoderHandle hAudSpdifDecoder;
} DRV_SYNC_DECODER_s;

#define VID_DEC_INSTANCE 0
#define AUD_ASTM_MAIN_INSTANCE 0
#define AUD_ASTM_SPDIF_INSTANCE 1

DRV_Error DRV_SYNC_Init(void);

#endif 
