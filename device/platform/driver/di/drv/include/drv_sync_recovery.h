#ifndef __DRV_SYNC_RECOVERY_H__
#define __DRV_SYNC_RECOVERY_H__

typedef enum
{
	DRV_SYNC_NULL=0,
	DRV_SYNC_REQ_RESTART

} DRV_SYNC_MSG_TYPE;

/* For Requesting Restart */
typedef struct
{
	unsigned long ulParam0;
	unsigned long ulParam1;

} DRV_SYNC_MSG;

unsigned int DRV_SYNC_GetPcrValue(void);
void DRV_SYNC_SetTsmMode(void);
void DRV_SYNC_SetVideoTsmMode(HBOOL bOnOff);
void DRV_SYNC_SetAudioTsmMode(HBOOL bOnOff);
DRV_Error DRV_SYNC_GetMsgId(unsigned long *ulSyncMsgId);
DRV_Error DRV_SYNC_RECOVERY_Init(DRV_SYNC_DECODER_s hdecoder);
#endif

