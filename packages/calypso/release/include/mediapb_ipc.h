#ifndef __MEDIAPB_IPC_H__
#define __MEDIAPB_IPC_H__

#include "cso.h"
#include "cso_def.h"

int MEDIAPB_IPC_CSO_BufferInfo(uint32_t ulSessionId, CSO_BufferInfo_t *pInfo);
void MEDIAPB_IPC_CSO_IPCMSG_MemFree_BufferInfo(CSO_BufferInfo_t *pInfo);
int MEDIAPB_IPC_CSO_Close(uint32_t ulSessionId);
int MEDIAPB_IPC_CSO_Restart(uint32_t ulSessionId);
int MEDIAPB_IPC_CSO_Control(uint32_t ulSessionId, CSO_CTRL_e eCtrl, CSO_CTRL_PARAM_t *pParam);
int MEDIAPB_IPC_CSO_GetDefaultSettings(CSO_Settings_t *settings);
int MEDIAPB_IPC_CSO_GetSettings(uint32_t ulSessionId,CSO_Settings_t *settings);
void MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(CSO_Settings_t settings);
int MEDIAPB_IPC_CSO_Init(CSO_Settings_t *settings);
int MEDIAPB_IPC_CSO_Open(uint8_t *pUrl, CSO_PlayMode_e eMode, unsigned int *puSessionId);
int MEDIAPB_IPC_CSO_Probe( uint32_t ulSessionId);
int MEDIAPB_IPC_CSO_DrmInfo(unsigned int ulSessionId, CSO_DrmInfo_t *drminfo);
int MEDIAPB_IPC_CSO_ProbeInfo(unsigned int ulSessionId, CSO_ProbeInfo_t *probeinfo);
int MEDIAPB_IPC_CSO_SetDefaultSettings(CSO_Settings_t *settings);
int MEDIAPB_IPC_CSO_SetResourceMap(void* pMap);
int MEDIAPB_IPC_CSO_SetSettings(uint32_t ulSessionId, CSO_Settings_t *settings);
int MEDIAPB_IPC_CSO_Start(uint32_t ulSessionId, uint32_t ulDeviceId);
/* Calypso will try to seek to given position at the end of stop if the value is equal to or greater than 0. 
   If you are sure there are no more start for this session after stops */
int MEDIAPB_IPC_CSO_Stop(uint32_t ulSessionId, int64_t llStoppedPos);
int MEDIAPB_IPC_CSO_Reset(uint32_t ulSessionId);
int MEDIAPB_IPC_Create(int port);
int MEDIAPB_IPC_RegisterCallback(int type, unsigned int ulSessionId, void *arg);
int MEDIAPB_IPC_CSO_GetOutputControl(uint32_t ulSessionId, CSO_OutputControl_t *pstOutputControl);

#endif /* __MEDIAPB_IPC_H__*/
