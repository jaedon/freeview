/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_playback.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 5/16/12 11:45a $
 *
 * Module Description: Audio Playback Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_playback.h $
 * 
 * Hydra_Software_Devel/6   5/16/12 11:45a jgarrett
 * SW7425-3071: Adding programmable heaps
 * 
 * Hydra_Software_Devel/5   2/8/12 11:52a jgarrett
 * SW7346-672: Adding support for changing the playback sample rate on the
 * fly
 * 
 * Hydra_Software_Devel/4   5/12/11 4:39p piyushg
 * SW7425-401: Added PCM playback support for non-interleaved and
 * multichannel data
 * 
 * Hydra_Software_Devel/3   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/2   12/17/10 3:58p jgarrett
 * SW7422-146: Nexus APE integration on 7422
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BAPE_PLAYBACK_H_
#define BAPE_PLAYBACK_H_

/***************************************************************************
Summary:
Playback Handle
***************************************************************************/
typedef struct BAPE_Playback *BAPE_PlaybackHandle;

/***************************************************************************
Summary:
Playback Open settings
***************************************************************************/
typedef struct BAPE_PlaybackOpenSettings
{
    BMEM_Heap_Handle heap;          /* Heap used for allocating buffers.  If not set, the heap
                                       provided in BAPE_Open will be used. */
    unsigned numBuffers;            /* Number of buffers to allocate.  For mono or interleaved stereo,
                                       one buffer is required.  For non-interleaved 7.1 data 8 buffers
                                       are required.  For interleaved 7.1 data, 4 buffers are required. */
    unsigned bufferSize;            /* Buffer size in bytes, must be a multiple of 256.  If more than one
                                       buffer is allocated, all buffers will be this size.  If this size
                                       is 0, buffers will be allocated from the internal buffer pool. */
    unsigned watermarkThreshold;    /* FIFO interrupt threshold in bytes.  
                                       When an amuont <= this threshold is available, 
                                       an interrupt will be raised. This value should
                                       be a multiple of 256. */    
} BAPE_PlaybackOpenSettings;

/***************************************************************************
Summary:
Get Default Playback Open Settings
***************************************************************************/
void BAPE_Playback_GetDefaultOpenSettings(
    BAPE_PlaybackOpenSettings *pSettings
    );

/***************************************************************************
Summary:
Open a playback channel
***************************************************************************/
BERR_Code BAPE_Playback_Open(
    BAPE_Handle hApe,
    unsigned index,
    const BAPE_PlaybackOpenSettings *pSettings,
    BAPE_PlaybackHandle *pHandle                    /* [out] */
    );

/***************************************************************************
Summary:
Close a playback channel
***************************************************************************/
void BAPE_Playback_Close(
    BAPE_PlaybackHandle hPlayback
    );

/***************************************************************************
Summary:
Audio playback settings
***************************************************************************/
typedef struct BAPE_PlaybackSettings
{
    BAPE_MultichannelFormat multichannelFormat;     /* Controls whether the playback channel outputs 2.0, 5.1, or 7.1 data.
                                                       This is not changeable on the fly. */
    bool compressedData;                            /* If true, compressed data will be fed in IEC61937 format.  
                                                       This is not changeable on the fly.  Compressed data should be
                                                       fed as 16-bit stereo interleaved data. */
    unsigned sampleRate;                            /* Sample Rate in Hz.  This is only used if BAPE_PlaybackStartSettings.sampleRate is 
                                                       set to 0 and allows changing of the sampleRate on the fly to permit pitch-shifting, 
                                                       or similar effects. */
} BAPE_PlaybackSettings;

/***************************************************************************
Summary:
Get current audio playback settings 
***************************************************************************/
void BAPE_Playback_GetSettings(
    BAPE_PlaybackHandle hPlayback,
    BAPE_PlaybackSettings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
Set current audio playback settings 
***************************************************************************/
BERR_Code BAPE_Playback_SetSettings(
    BAPE_PlaybackHandle hPlayback,
    const BAPE_PlaybackSettings *pSettings
    );

/***************************************************************************
Summary:
Playback Start Settings
***************************************************************************/
typedef struct BAPE_PlaybackStartSettings
{
    unsigned sampleRate;                    /* Sample Rate in Hz.  If set to 0, the value from BAPE_PlaybackSettings.sampleRate will be used instead. */
    unsigned bitsPerSample;                 /* Supports 8, 16, or 32 */
    bool isStereo;                          /* If true, data will be treated as stereo.  If false, data will be treated as mono. */
    bool isSigned;                          /* If true, data will be treated as signed.  If false, data will be treated as unsigned. */
    bool reverseEndian;                     /* If true, data will be endian-swapped in hardware.  Otherwise, data should match host endian mode. */
    bool loopEnabled;                       /* If true, data will loop continuously without requiring host intervention. */
    bool interleaved;                       /* If true, data for a channel pair is interleaved into a single buffer */
    unsigned startThreshold;                /* Size in bytes that must be committed to the hardware before data will flow into the mixer.  
                                               Must be a multiple of 256. */    
} BAPE_PlaybackStartSettings;

/***************************************************************************
Summary:
Get Default Playback Start Settings
***************************************************************************/
void BAPE_Playback_GetDefaultStartSettings(
    BAPE_PlaybackStartSettings *pSettings       /* [out] */
    );

/***************************************************************************
Summary:
Start Playback
***************************************************************************/
BERR_Code BAPE_Playback_Start(
    BAPE_PlaybackHandle hPlayback,
    const BAPE_PlaybackStartSettings *pSettings
    );

/***************************************************************************
Summary:
Stop Playback
***************************************************************************/
void BAPE_Playback_Stop(
    BAPE_PlaybackHandle hPlayback
    );

/***************************************************************************
Summary:
Flush Playback Buffer
***************************************************************************/
void BAPE_Playback_Flush(
    BAPE_PlaybackHandle hPlayback
    );

/***************************************************************************
Summary:
Get Playback Buffer 
 
Description: 
This routine will return the next contiguous buffer address and size.  If 
interrupts are enabled, the caller should call this routine and commit data 
until the size returned from this function is zero. 
***************************************************************************/
BERR_Code BAPE_Playback_GetBuffer(
    BAPE_PlaybackHandle hPlayback,
    BAPE_BufferDescriptor *pBuffers      /* [out] */
    );

/***************************************************************************
Summary:
Commit Data to the playback hardware
***************************************************************************/
BERR_Code BAPE_Playback_CommitData(
    BAPE_PlaybackHandle hPlayback,
    unsigned numBytes                   /* Number of bytes written into the buffer */
    );

/***************************************************************************
Summary:
Playback Status
***************************************************************************/
typedef struct BAPE_PlaybackStatus
{
    unsigned queuedBytes;
    unsigned fifoSize;
} BAPE_PlaybackStatus;

/***************************************************************************
Summary:
Get Playback Status
***************************************************************************/
void BAPE_Playback_GetStatus(
    BAPE_PlaybackHandle hPlayback,
    BAPE_PlaybackStatus *pStatus    /* [out] */
    );

/***************************************************************************
Summary:
Get Audio Source Connector for output data
***************************************************************************/
void BAPE_Playback_GetConnector(
    BAPE_PlaybackHandle hPlayback,
    BAPE_Connector *pConnector /* [out] */
    );

/***************************************************************************
Summary:
Playback Interrupt Handlers
***************************************************************************/
typedef struct BAPE_PlaybackInterruptHandlers
{
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } watermark;
} BAPE_PlaybackInterruptHandlers;

/***************************************************************************
Summary:
Get Playback Interrupt Handlers
***************************************************************************/
void BAPE_Playback_GetInterruptHandlers(
    BAPE_PlaybackHandle hPlayback,
    BAPE_PlaybackInterruptHandlers *pInterrupts     /* [out] */
    );

/***************************************************************************
Summary:
Set Playback Interrupt Handlers
***************************************************************************/
BERR_Code BAPE_Playback_SetInterruptHandlers(
    BAPE_PlaybackHandle hPlayback,
    const BAPE_PlaybackInterruptHandlers *pInterrupts
    );

#endif /* #ifndef BAPE_PLAYBACK_H_ */

