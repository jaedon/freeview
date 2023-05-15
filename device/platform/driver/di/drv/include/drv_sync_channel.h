#ifndef __DRV_SYNC_CHANNEL_H__
#define __DRV_SYNC_CHANNEL_H__

#include "drv_sync_common.h"

DRV_Error DRV_SYNC_SetSyncChannel(DRV_SYNC_AV eSyncHandle, NEXUS_AudioDecoderConnectorType eAudConType);
DRV_Error DRV_SYNC_UnsetSyncChannel(DRV_SYNC_AV eSyncHandle);
DRV_Error DRV_SYNC_CHANNEL_Init(DRV_SYNC_DECODER_s hdecoder);
DRV_Error DRV_SYNC_SetAudioDecoderHandle(DRV_SYNC_AV eSyncHandle, NEXUS_AudioDecoderHandle* hAudDecoder);

#endif

