/**
 * drv_stream_mux.h
*/

#ifndef __DRV_STREAM_MUX_H__
#define __DRV_STREAM_MUX_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"
#include "nexus_pid_channel.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define	MAX_MUX_NUM				2
#define	MAX_NUM_PSI				5
#define	MAX_NUM_PES				5
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct	DRV_STREAM_MUX_Information
{
	HUINT8	index;
	HUINT32 videoCodec;
	HUINT16 videoPid;
	HUINT32 audioCodec;
	HUINT16 audioPid;
	HUINT16 audioSubPid;
	HUINT16 pcrPid;
	HBOOL	NonPcrInsert;
	HBOOL	bIsWatch;
	HUINT32	ulAudCnt;	
	HUINT32 ulDemuxId;
} DRV_STREAM_MUX_INFORMATION;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_STREAM_MUX_Init(HUINT32 ulDeviceId);
DRV_Error DRV_STREAM_MUX_UnInit(HUINT32 ulDeviceId);
DRV_Error DRV_STREAM_MUX_Start(HUINT32 ulDeviceId,DRV_STREAM_MUX_INFORMATION	*pMuxInfo);
DRV_Error DRV_STREAM_MUX_Stop(HUINT32 ulDeviceId);
DRV_Error DRV_STREAM_MUX_Add_SystemData(HUINT32 ulDeviceId,DRV_STREAM_MUX_INFORMATION	*pMuxInfo);

DRV_Error DRV_STREAM_MUX_GetTranscodeVideo(HUINT32 ulDeviceId,NEXUS_PidChannelHandle *ppPidChannelTranscodeVideo);
DRV_Error DRV_STREAM_MUX_GetTranscodeAudio(HUINT32 ulDeviceId,NEXUS_PidChannelHandle *ppPidChannelTranscodeAudio,NEXUS_PidChannelHandle *ppPidChannelTranscodeSubAudio);
DRV_Error DRV_STREAM_MUX_GetTranscodePCR(HUINT32 ulDeviceId,NEXUS_PidChannelHandle *ppPidChannelTranscodePcr);
DRV_Error	DRV_STREAM_MUX_GetTranscodePSI(HUINT32 ulDeviceId,HUINT32 ulIndex,NEXUS_PidChannelHandle *ppPidChannelTranscodePsi);
DRV_Error	DRV_STREAM_MUX_GetUsedPES(HUINT32 ulDeviceId,HUINT32 *pNumber);
DRV_Error	DRV_STREAM_MUX_GetTranscodePES(HUINT32 ulDeviceId,HUINT32 ulIndex,NEXUS_PidChannelHandle *ppPidChannelTranscodePes);
DRV_Error	DRV_STREAM_MUX_CloseTotalTranscodePSI(HUINT32 ulDeviceId);
DRV_Error	DRV_STREAM_MUX_SetPesData(HUINT32 ulDeviceId,HUINT32 ulInputDemuxId,HUINT16 pesPid,HUINT32 *pIndexId );
DRV_Error	DRV_STREAM_MUX_SetPSIData(HUINT32 ulDeviceId,HUINT16 psiPid,HUINT32 psiSize,HUINT8 *psiData,HUINT8 contflag,unsigned long timeOut,HUINT32 *pIndexId );
DRV_Error	DRV_STREAM_MUX_UpdatePSIData(HUINT32 ulDeviceId,HUINT32 ulIndex,HUINT32 psiSize,HUINT8 *pPSIData,HUINT8 startflag);
DRV_Error	DRV_STREAM_MUX_GetUsedPSI(HUINT32 ulDeviceId,HUINT32 *pNumber);
DRV_Error	DRV_STREAM_MUX_GetDuration(HUINT32 ulDeviceId,HUINT32 *ulTime);
#endif /* !__DRV_STREAM_MUX_H__ */

