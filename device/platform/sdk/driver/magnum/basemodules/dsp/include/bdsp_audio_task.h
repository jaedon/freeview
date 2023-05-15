/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_audio_task.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 2/6/13 6:41p $
 *
 * Module Description: Audio DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/bdsp_audio_task.h $
 * 
 * Hydra_Software_Devel/10   2/6/13 6:41p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/10   2/6/13 6:26p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/3   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/9   11/7/12 6:11p ananthan
 * SW7425-3808: Added BDSP changes to add new Event ID to notify change in
 * dialnorm.
 * 
 * Hydra_Software_Devel/SW7425-4216/2   1/8/13 4:57p gprasad
 * SW7425-4216: Merging mainline changes to rbuf capture branch as of
 * 01/08/2012
 * 
 * Hydra_Software_Devel/SW7425-4216/1   12/4/12 5:06p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/8   9/18/12 6:35p ananthan
 * SW7346-914: Adding support for MPEG Ancillary data parsing.
 * 
 * Hydra_Software_Devel/7   6/19/12 3:15a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/6   4/11/12 7:53p gprasad
 * SW7425-2863: Add an interrupt to notify when the o/p buffer gets full
 * during the encoding process
 * 
 * Hydra_Software_Devel/5   2/15/12 3:02p ashoky
 * SW7425-2308: Adding interface to put audio firmware in audio gap fill
 * mode.
 * 
 * Hydra_Software_Devel/4   7/8/11 4:38p gprasad
 * SW7552-64: Changes to provide stream sample rate along wityh base rate
 * to APE
 * 
 * Hydra_Software_Devel/3   6/9/11 8:11p gautamk
 * SW7422-357 : [7425] Modifying API prtotype for Add/Remove stage input
 * 
 * Hydra_Software_Devel/2   4/29/11 11:42a srajapur
 * SW7422-368 : [7422] BDSP_AudioTask_GetDefaultXxxSettings is Raaga-
 * specific ---------> Moved raaga specific API from common files to
 * raaga files and renamed the API's accordingly.
 * 
 * Hydra_Software_Devel/1   4/6/11 2:15p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/10   2/16/11 5:37a srajapur
 * SW7422-247 : [7422] Adding all audio post processing algorithms.
 * 
 * Hydra_Software_Devel/9   2/5/11 4:08a srajapur
 * SW7422-247 : [7422] Adding Generic passthrul audio processing support.
 * 
 * Hydra_Software_Devel/8   2/3/11 3:11a srajapur
 * SW7422-229 : Adding support for MPEG algorithm. --> Passing the
 * correct modevalue to application.
 * 
 * Hydra_Software_Devel/7   1/19/11 6:13p jgarrett
 * SW7422-146: Enabling TSM
 * 
 * Hydra_Software_Devel/6   1/19/11 7:23a gautamk
 * SW7422-191:Adding API for Default settings.
 * 
 * Hydra_Software_Devel/5   1/18/11 4:57a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/4   1/18/11 4:26a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/3   1/13/11 5:57a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/2   12/15/10 6:47p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 * Hydra_Software_Devel/1   12/14/10 2:17p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BDSP_AUDIO_TASK_H_
#define BDSP_AUDIO_TASK_H_

#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bimg.h"
#include "bdsp_task.h"
#include "bdsp_raaga_fw_settings.h"
#include "bdsp_raaga_fw_status.h"

/***************************************************************************
Summary:
Audio Capture Handle
***************************************************************************/
typedef struct BDSP_AudioCapture *BDSP_AudioCaptureHandle;

/***************************************************************************
Summary:
Audio Capture Create Settings
***************************************************************************/
typedef struct BDSP_AudioCaptureCreateSettings
{
    unsigned maxChannels;       /* Maximum number of channels to capture.  1 = mono/compressed.  2 = stereo.  6 = 5.1.  Default = 2. */
    size_t channelBufferSize;   /* Channel buffer size in bytes.  Default is 1536kB. */
    BMEM_Heap_Handle hHeap;     /* Memory Heap to use for allocating buffers.  If NULL, the default heap will be used. */
} BDSP_AudioCaptureCreateSettings;

/***************************************************************************
Summary:
Settings to add a capture handle to a task
***************************************************************************/
typedef struct BDSP_AudioTaskAddCaptureSettings
{
    /* TODO: These two will become a stage handle in the new cit model */
    unsigned branchId;  /* The Stage ID of the stage whose output will be captured */
    unsigned stageId;   /* The branch ID of the stage whose output will be captured */

    /* TODO: This will be removed in the new cit model, stage handle + output id will become sufficient */
    BDSP_AF_P_DistinctOpType eOpType; /* The distinct output type that needs to be captured. */

    unsigned outputId;  /* The output port number of 'eOpType' that needs to be captured */

    bool updateRead; /* This flag is enabled when there is no consumer for the output buffer
                       and the capture thread is expected to update the buffer read pointers */
} BDSP_AudioTaskAddCaptureSettings;

/***************************************************************************
Summary:
Buffer Descriptor
***************************************************************************/
typedef struct BDSP_BufferDescriptor
{
    bool interleaved;               /* If true, every other channel will have valid pointers,
                                       e.g. L for L/R, Ls for Ls/Rs, etc.  */
    unsigned numBuffers;            /* Number of buffers.  For mono/interleaved stereo this is 1.  For
                                       non-interleaved stereo, it's 2.  For non-interleaved 7.1 it's 8. */
    struct
    {
        void *pBuffer;              /* Buffer base address prior to wraparound */
        void *pWrapBuffer;          /* Buffer address after wraparound (NULL if no wrap has occurred) */
    } buffers[BDSP_AF_P_MAX_CHANNELS];

    unsigned bufferSize;            /* Buffer size before wraparound in bytes */
    unsigned wrapBufferSize;        /* Buffer size after wraparound in bytes */        
} BDSP_BufferDescriptor;

/***************************************************************************
Summary:
	This structure returns information of bit rate change in stream

Notes: 
    This structure will be passed on to application callback function
    on bit rate change interrupt
***************************************************************************/
typedef struct BDSP_AudioBitRateChangeInfo
{
    BDSP_AudioType   eType;   /* audio type */
    uint32_t ui32BitRate;    /* New Bit Rate value*/
					     /* If eType = DTS or DTS-HD and 
					     ui32BitRateIndex = 29,30 or 31
					     ui32BitRate = 0 */
    uint32_t ui32BitRateIndex;    /* This has the Bit rate index
    							as given in the standard. This value
    							is zero for audio type AAC-HE*/
} BDSP_AudioBitRateChangeInfo;

/***************************************************************************
Summary:
Pause an audio DSP task
***************************************************************************/
BERR_Code BDSP_AudioTask_Pause(
    BDSP_TaskHandle task
    );

/***************************************************************************
Summary:
Resume an audio DSP task
***************************************************************************/
BERR_Code BDSP_AudioTask_Resume(
    BDSP_TaskHandle task
    );

/***************************************************************************
Summary:
Advance an audio DSP task
***************************************************************************/
BERR_Code BDSP_AudioTask_Advance(
    BDSP_TaskHandle task,
    unsigned ms
    );

/***************************************************************************
Summary:
Get TSM settings for a particular task stage
***************************************************************************/
BERR_Code BDSP_AudioTask_GetTsmSettings_isr(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_AudioTaskTsmSettings *pTsmSettings         /* [out] */
    );

/***************************************************************************
Summary:
Set TSM settings for a particular task stage
***************************************************************************/
BERR_Code BDSP_AudioTask_SetTsmSettings_isr(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_AudioTaskTsmSettings *pTsmSettings
    );

/***************************************************************************
Summary:
Get TSM status from a particular task stage
***************************************************************************/
BERR_Code BDSP_AudioTask_GetTsmStatus_isr(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_AudioTaskTsmStatus *pTsmStatus         /* [out] */
    );

/***************************************************************************
Summary:
Get Datasync settings for a particular task stage
***************************************************************************/
BERR_Code BDSP_AudioTask_GetDatasyncSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_AudioTaskDatasyncSettings *pTsmSettings         /* [out] */
    );

/***************************************************************************
Summary:
Set Datasync settings for a particular task stage
***************************************************************************/
BERR_Code BDSP_AudioTask_SetDatasyncSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_AudioTaskDatasyncSettings *pTsmSettings
    );

/***************************************************************************
Summary:
Get Datasync status from a particular task stage
***************************************************************************/
BERR_Code BDSP_AudioTask_GetDatasyncStatus(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_AudioTaskDatasyncStatus *pTsmStatus         /* [out] */
    );

#if 0

/***************************************************************************
Summary:
Add input to a stage
***************************************************************************/
BERR_Code BDSP_AudioTask_AddStageInput(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_CIT_P_FwStgSrcDstDetails *pInputInfo
    );

/***************************************************************************
Summary:
Remove input from a stage
***************************************************************************/
BERR_Code BDSP_AudioTask_RemoveStageInput(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_CIT_P_FwStgSrcDstDetails *pInputInfo
    );

/***************************************************************************
Summary:
Add output to a stage
***************************************************************************/
BERR_Code BDSP_AudioTask_AddStageOutput(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_CIT_P_FwStgSrcDstDetails *pOutputInfo
    );

/***************************************************************************
Summary:
Remove output from a stage
***************************************************************************/
BERR_Code BDSP_AudioTask_RemoveStageOutput(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_CIT_P_FwStgSrcDstDetails *pOutputInfo
    );
#endif
/***************************************************************************
Summary: 
Audio Interrupt Handlers for a task 
***************************************************************************/
typedef struct BDSP_AudioInterruptHandlers 
{
    /* Interrupt fires when first PTS is received */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus);
        void *pParam1;
        int param2;
    } firstPts;
    /* Interrupt fires when TSM Fail (PTS Error) occurs */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus);
        void *pParam1;
        int param2;    
    } tsmFail;
    /* Interrupt fires when TSM transitions from fail -> pass in ASTM mode */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus);
        void *pParam1;
        int param2;
    } tsmPass;
    /* Interrupt fires when the decoder receives the first or any new sample rate in the stream */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, unsigned streamSampleRate, unsigned baseSampleRate);
        void *pParam1;
        int param2;
    } sampleRateChange;
    /* Interrupt fires when the decoder achieves frame lock */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } lock;
    /* Interrupt fires when the decoder loses frame lock */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } unlock;
    /* Interrupt fires when the status buffer is valid */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } statusReady;
        /* Interrupt fires when the mode change happens */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2,const uint32_t ui32ModeValue);
        void *pParam1;
        int param2;
    } modeChange;

        /* Interrupt fires when the bitrate change happens */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2,const BDSP_AudioBitRateChangeInfo *bitrateChangeInfo);
        void *pParam1;
        int param2;
    } bitrateChange;
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } cdbItbOverflow;
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } cdbItbUnderflow;
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } startPtsReached;
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } stopPtsReached;
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } unlicensedAlgo;
   struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } rampEnable;    
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } encoderOutputOverflow;    
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } ancillaryData; 
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } dialnormChange; 
        
} BDSP_AudioInterruptHandlers;

/***************************************************************************
Summary: 
Get Current Interrupt Handlers for a task 
***************************************************************************/
void BDSP_AudioTask_GetInterruptHandlers_isr(
    BDSP_TaskHandle task,
    BDSP_AudioInterruptHandlers *pHandlers   /* [out] */
    );

/***************************************************************************
Summary: 
Set Current Interrupt Handlers for a task 
***************************************************************************/
BERR_Code BDSP_AudioTask_SetInterruptHandlers_isr(
    BDSP_TaskHandle task,
    const BDSP_AudioInterruptHandlers *pHandlers
    );


/***************************************************************************
Summary: 
Get default Datasync settings.
***************************************************************************/
BERR_Code BDSP_AudioTask_GetDefaultDatasyncSettings(
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    );

/***************************************************************************
Summary: 
Get default Tsm settings.
***************************************************************************/
BERR_Code BDSP_AudioTask_GetDefaultTsmSettings(
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    );

/***************************************************************************
Summary: 
Command to put an audio task in zero fill mode. Required for NRT xcode case, where incase
of audio gaps, audio stalls video too. This command avoids this deadlock.
***************************************************************************/
BERR_Code BDSP_AudioTask_AudioGapFill(
    BDSP_TaskHandle task
    );

/***************************************************************************
Summary:
Get Default Audio Capture Create Settings
***************************************************************************/
void BDSP_AudioCapture_GetDefaultCreateSettings(
    BDSP_AudioCaptureCreateSettings *pSettings  /* [out] */
    );

/***************************************************************************
Summary:
Create an audio capture context 
 
Description: 
An Audio Capture object is used to capture a copy of the DSP data into host 
memory prior to being consumed by intended consumer (e.g. audio mixer hardware). 
For the data to be copied, a thread must call BDSP_ProcessAudioCapture() on a 
background thread at a frequent interval (e.g. 10ms). 
 
To bind a capture handle to a task and output, you must call 
BDSP_AudioTask_AddCapture before calling BDSP_Task_Start.  After 
BDSP_Task_Stop has been called, BDSP_AudioTask_RemoveCapture should 
also be called. 
***************************************************************************/
BERR_Code BDSP_AudioCapture_Create(
    BDSP_ContextHandle hContext,
    const BDSP_AudioCaptureCreateSettings *pSettings,
    BDSP_AudioCaptureHandle *pCapture   /* [out] */
    );

/***************************************************************************
Summary:
Destroy an audio capture context
***************************************************************************/
void BDSP_AudioCapture_Destroy(
    BDSP_AudioCaptureHandle hCapture
    );

/***************************************************************************
Summary:
Get Default settings to add a capture handle to a task
***************************************************************************/
void BDSP_AudioTask_GetDefaultAddCaptureSettings(
    BDSP_AudioTaskAddCaptureSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
Add an audio capture handle to a task
 
Description: 
An Audio Capture object is used to capture a copy of the DSP data into host 
memory prior to being consumed by intended consumer (e.g. audio mixer hardware). 
For the data to be copied, a thread must call BDSP_ProcessAudioCapture() on a 
background thread at a frequent interval (e.g. 10ms). 
 
To bind a capture handle to a task and output, you must call 
BDSP_AudioTask_AddCapture before calling BDSP_Task_Start.  After 
BDSP_Task_Stop has been called, BDSP_AudioTask_RemoveCapture should 
also be called. 
***************************************************************************/
BERR_Code BDSP_AudioTask_AddCapture(
    BDSP_TaskHandle hTask,
    BDSP_AudioCaptureHandle hCapture,
    const BDSP_AudioTaskAddCaptureSettings *pSettings
    );

/***************************************************************************
Summary:
Remove an audio capture handle from a task
***************************************************************************/
void BDSP_AudioTask_RemoveCapture(
    BDSP_TaskHandle hTask,
    BDSP_AudioCaptureHandle hCapture
    );


/***************************************************************************
Summary:
Get the Buffer Descriptors
***************************************************************************/
BERR_Code BDSP_AudioCapture_GetBuffer(
    BDSP_AudioCaptureHandle hCapture,
    BDSP_BufferDescriptor *pBuffers /* [out] */
    );

/***************************************************************************
Summary:
Consume data from the capture buffers
***************************************************************************/
BERR_Code BDSP_AudioCapture_ConsumeData(
    BDSP_AudioCaptureHandle hCapture,
    unsigned numBytes                   /* Number of bytes read from each buffer */
    );

#endif

