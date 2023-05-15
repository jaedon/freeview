#ifndef	__IPC_MSG_H__
#define	__IPC_MSG_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
int CSO_IPCMSG_SetToMsg_DrmInfo(CSO_DrmInfo_t *pInfo, void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_DrmInfo(CSO_DrmInfo_t *pInfo, void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_ProbeInfo(CSO_ProbeInfo_t *probeInfo, void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_ProbeInfo(CSO_ProbeInfo_t *probeInfo, void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_Settings(CSO_Settings_t *settings, void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_Settings(CSO_Settings_t *settings, void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_OpenParams(char *url, CSO_PlayMode_e playmode ,void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_StartParam(int nDeviceId,void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_StoppedPosParam(long long int llStoppedPos,void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_BufferInfo(CSO_BufferInfo_t *pInfo ,void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_BufferInfo(CSO_BufferInfo_t *pInfo ,void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_ResourceMap(CSO_RESOURCE_MAP_t *resourceMap ,void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_ResourceMap(CSO_RESOURCE_MAP_t *resourceMap ,void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_ControlParams(CSO_CTRL_e eCtrl, CSO_CTRL_PARAM_t *pParams ,void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_OpenParams(char **url, CSO_PlayMode_e *playmode ,void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_StartParam(int *pnDeviceId,void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_StoppedPosParam(long long int *pllStoppedPos,void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_ControlParams(CSO_CTRL_e *peCtrl, CSO_CTRL_PARAM_t *pParams ,void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_ControlResult(CSO_CTRL_e eCtrl, CSO_CTRL_PARAM_t *pParams, void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_ControlResult(CSO_CTRL_e eCtrl, CSO_CTRL_PARAM_t *pParams, void *msg, int msglen);
int CSO_IPCMSG_SetToMsg_OutputControl(CSO_OutputControl_t *pstOutputControl, void *msg, int msglen);
int CSO_IPCMSG_GetFromMsg_OutputControl(CSO_OutputControl_t *pstOutputControl, void *msg, int msglen);
void CSO_IPCMSG_MemFree_DrmInfo(CSO_DrmInfo_t drmInfo);
void CSO_IPCMSG_MemFree_Settings(CSO_Settings_t settings);
void CSO_IPCMSG_MemFree_OutputControl(CSO_OutputControl_t stOutputControl);
void CSO_IPCMSG_MemFree_BufferInfo(CSO_BufferInfo_t * pInfo);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* __IPC_MSG_H__ */

