#ifndef __DRV_ASTM_H__
#define __DRV_ASTM_H__

#include "drv_sync_common.h"

HBOOL DRV_SYNC_GetAstmStarted(void);
HBOOL DRV_SYNC_GetAstmVideoValid(void);
HBOOL DRV_SYNC_GetAstmAudioValid(void);

DRV_Error DRV_SYNC_SetAstmSettings(DRV_SYNC_AV eSyncHandle);
DRV_Error DRV_SYNC_UnsetAstmSettings(DRV_SYNC_AV eSyncHandle);
DRV_Error DRV_SYNC_AstmStop(void);
DRV_Error DRV_SYNC_AstmStart(void);
DRV_Error DRV_SYNC_ASTM_Init(DRV_SYNC_DECODER_s hdecoder);
#endif 

