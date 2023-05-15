/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_output_capture.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 5/16/12 11:45a $
 *
 * Module Description: Audio Output Capture Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_output_capture.h $
 * 
 * Hydra_Software_Devel/5   5/16/12 11:45a jgarrett
 * SW7425-3071: Adding programmable heaps
 * 
 * Hydra_Software_Devel/4   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/3   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 * Hydra_Software_Devel/2   12/14/10 2:39p jgarrett
 * SW7422-146: Renaming MixerOutput to OutputPort
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BAPE_OUTPUT_CAPTURE_H_
#define BAPE_OUTPUT_CAPTURE_H_

/***************************************************************************
Summary:
Output Capture Handle
***************************************************************************/
typedef struct BAPE_OutputCapture *BAPE_OutputCaptureHandle;

/***************************************************************************
Summary:
Output Capture Open Settings
***************************************************************************/
typedef struct BAPE_OutputCaptureOpenSettings
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
                                       When an amuont >= this threshold is available, 
                                       an interrupt will be raised. This value should
                                       be a multiple of 256. */    
} BAPE_OutputCaptureOpenSettings;

/***************************************************************************
Summary:
Get Default Output Capture Open Settings
***************************************************************************/
void BAPE_OutputCapture_GetDefaultOpenSettings(
    BAPE_OutputCaptureOpenSettings *pSettings       /* [out] */
    );

/***************************************************************************
Summary:
Open a capture output
***************************************************************************/
BERR_Code BAPE_OutputCapture_Open(
    BAPE_Handle hApe,
    unsigned index,
    const BAPE_OutputCaptureOpenSettings *pSettings,
    BAPE_OutputCaptureHandle *pHandle             /* [out] */
    );

/***************************************************************************
Summary:
Close a capture output
***************************************************************************/
void BAPE_OutputCapture_Close(
    BAPE_OutputCaptureHandle hOutputCapture
    );

/***************************************************************************
Summary:
OutputCapture Settings
***************************************************************************/
typedef struct BAPE_OutputCaptureSettings
{
    unsigned bitsPerSample;                 /* Currently, only 32-bit PCM capture is supported.  */
    bool interleaveData;                    /* If true, PCM data for a stereo pair will be interleaved
                                               into a single buffer.  False will use a separate buffer
                                               for each channel. */
} BAPE_OutputCaptureSettings;

/***************************************************************************
Summary:
Get OutputCapture Settings
***************************************************************************/
void BAPE_OutputCapture_GetSettings(
    BAPE_OutputCaptureHandle hOutputCapture,
    BAPE_OutputCaptureSettings *pSettings       /* [out] */
    );

/***************************************************************************
Summary:
Set OutputCapture Settings
***************************************************************************/
BERR_Code BAPE_OutputCapture_SetSettings(
    BAPE_OutputCaptureHandle hOutputCapture,
    const BAPE_OutputCaptureSettings *pSettings
    );

/***************************************************************************
Summary:
Flush OutputCapture Buffer
***************************************************************************/
void BAPE_OutputCapture_Flush(
    BAPE_OutputCaptureHandle hOutputCapture
    );

/***************************************************************************
Summary:
Flush OutputCapture Buffer from interrupt context
***************************************************************************/
void BAPE_OutputCapture_Flush_isr(
    BAPE_OutputCaptureHandle hOutputCapture
    );

/***************************************************************************
Summary:
Get Capture Buffer 
 
Description: 
This routine will return the next contiguous buffer address and size.  If 
interrupts are enabled, the caller should call this routine and commit data 
until the size returned from this function is zero. 
***************************************************************************/
BERR_Code BAPE_OutputCapture_GetBuffer(
    BAPE_OutputCaptureHandle hOutputCapture,
    BAPE_BufferDescriptor *pBuffers      /* [out] */
    );

/***************************************************************************
Summary:
Mark data as consumed from the OutputCapture hardware
***************************************************************************/
BERR_Code BAPE_OutputCapture_ConsumeData(
    BAPE_OutputCaptureHandle hOutputCapture,
    unsigned numBytes                   /* Number of bytes read from the buffer */
    );

/***************************************************************************
Summary:
Get Audio Output Connector
***************************************************************************/
void BAPE_OutputCapture_GetOutputPort(
    BAPE_OutputCaptureHandle hOutputCapture,
    BAPE_OutputPort *pConnector
    );

/***************************************************************************
Summary:
Audio Output Capture Interrupts
***************************************************************************/
typedef struct BAPE_OutputCaptureInterruptHandlers
{
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } watermark;
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } overflow;
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, unsigned sampleRate);
        void *pParam1;
        int param2;
    } sampleRate;
} BAPE_OutputCaptureInterruptHandlers;

/***************************************************************************
Summary:
Get Audio Output Capture Interrupts
***************************************************************************/
void BAPE_OutputCapture_GetInterruptHandlers(
    BAPE_OutputCaptureHandle hOutputCapture,
    BAPE_OutputCaptureInterruptHandlers *pInterrupts    /* [out] */
    );

/***************************************************************************
Summary:
Set Audio Output Capture Interrupts
***************************************************************************/
BERR_Code BAPE_OutputCapture_SetInterruptHandlers(
    BAPE_OutputCaptureHandle hOutputCapture,
    const BAPE_OutputCaptureInterruptHandlers *pInterrupts
    );

#endif /* #ifndef BAPE_OUTPUT_CAPTURE_H_ */

