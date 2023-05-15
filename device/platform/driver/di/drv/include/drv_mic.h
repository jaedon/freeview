

/*******************************************************************/
//	* @author	   Junho Park
//	* @file 		   drv_mic.h
/*******************************************************************/

#ifndef __DRV_MIC_H__
#define __DRV_MIC_H__

/*******************************************************************/
// Header Files													     */
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

#include "nexus_audio_playback.h"


/*******************************************************************/
// Extern variables													     */
/*******************************************************************/

/*******************************************************************/
// Macro Definition													     */
/*******************************************************************/

/*******************************************************************/
// typedef														     */
/*******************************************************************/

//webcam type define
typedef enum
{
	MIC_TYPE_DUMMY = 0,
	MIC_TYPE_OSS,
	MIC_TYPE_MAX
} MIC_TYPE_e;


/*******************************************************************/
// global function prototype											     */
/*******************************************************************/


DRV_Error DRV_MIC_Init(void);
DRV_Error DRV_MIC_CreatePlayStreamTask(void);
DRV_Error DRV_MIC_StopPlayStreamTask(void);

DRV_Error DRV_MIC_InitDevice(HUINT32 busnum);
DRV_Error DRV_MIC_OpenDevice(HUINT32 busnum);
DRV_Error DRV_MIC_CloseDevice(HUINT32 busnum);
DRV_Error DRV_MIC_SetDeviceStatus(HUINT32 busnum);
DRV_Error DRV_MIC_GetPlaybackStatus(HUINT32 busnum);
DRV_Error DRV_MIC_StartStream(HUINT32 busnum);
DRV_Error DRV_MIC_StopStream(HUINT32 busnum);
DRV_Error DRV_MIC_SetVolume(HUINT32 ulMicVolume);
DRV_Error DRV_MIC_PlaybackStart(void);

void data_callback(void *pParam1, int param2);
DRV_Error DRV_MIC_SetMixerVolume(HUINT32 ulMicMixerVolume);
#endif

