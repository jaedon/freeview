#ifndef __IR_PVR_H__
#define	__IR_PVR_H__

 
/********************************************************************
*   type definition

********************************************************************/


IR_PVRSERVICE_INFO	*CAS_IR_Drv_FindPVRSvcInfoBySCELL(HUINT16 usSCELLHandle);

void	CAS_IR_Drv_GetPlaybackEncryptionMode(Handle_t hIrSvc, HUINT8 *pucEncryption);
HERROR	CAS_IR_PVR_SetupRecord(CAS_CAI_IF_msgPVR *msgPVR);
HERROR	CAS_IR_PVR_SetupPlayback(CAS_CAI_IF_msgPVR *msgPVR);

HERROR	CAS_IR_Drv_CloseRecord(Handle_t hIrSvc);
HERROR	CAS_IR_Drv_ClosePlayback(Handle_t hIrSvc);

void		CAS_IR_DRV_Setup_Record(Handle_t hIrSvc, HUINT8 recordMode, HUINT16 errorIndex);
void 	CAS_IR_DRV_Setup_Playback(Handle_t hIrSvc, HUINT8 recordMode, HUINT16 errorIndex, HUINT32 ulStatus);
void 	CAS_IR_DRV_Start_RecordService(Handle_t hIrSvc);
void 	CAS_IR_DRV_Stop_RecordService(Handle_t hIrSvc);
void 	CAS_IR_DRV_Start_PlaybackService(Handle_t hIrSvc);
void 	CAS_IR_DRV_Stop_PlaybackService(Handle_t hIrSvc);

#endif

/*	end of file	*/


