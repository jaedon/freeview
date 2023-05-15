/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_context.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 11/26/12 1:13p $
 *
 * Module Description: Host DSP Context Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/bdsp_context.h $
 * 
 * Hydra_Software_Devel/4   11/26/12 1:13p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/3   1/27/12 1:02p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/2   5/16/11 10:06p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/1   4/6/11 2:16p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/4   1/20/11 8:13a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/3   1/13/11 5:58a gautamk
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

#ifndef BDSP_CONTEXT_H_
#define BDSP_CONTEXT_H_

#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bimg.h"
#include "bdsp.h"

/***************************************************************************
Summary:
DSP Audio Context Settings
***************************************************************************/
typedef struct BDSP_ContextCreateAudioSettings
{
    unsigned maxNumAudioDatasyncDownloadBuffer;
    unsigned maxNumAudioPassthruDownloadBuffer;
    unsigned maxNumAudioDecodeDownloadBuffer;
    unsigned maxNumAudioProcessingDownloadBuffer;    
    unsigned maxNumAudioEncodeDownloadBuffer;    
} BDSP_ContextCreateAudioSettings;

/***************************************************************************
Summary:
DSP Video Context Settings
***************************************************************************/
typedef struct BDSP_ContextCreateVideoSettings
{
    unsigned maxNumVideoDecodeDownloadBuffer;
} BDSP_ContextCreateVideoSettings;

/***************************************************************************
Summary:
DSP Video Encode Context Settings
***************************************************************************/
typedef struct BDSP_ContextCreateVideoEncodeSettings
{
    unsigned maxNumVideoEncodeDownloadBuffer;
} BDSP_ContextCreateVideoEncodeSettings;

typedef struct BDSP_ContextCreateScmSettings
{
    unsigned maxNumScmDecodeDownloadBuffer;
} BDSP_ContextCreateScmSettings;
typedef struct BDSP_ContextCreateSettings
{
    unsigned maxTasks;
    BDSP_ContextType    contextType;
    unsigned maxBranch;
    unsigned maxStagePerBranch;
    BDSP_ContextCreateAudioSettings audioDownloadBuffer;
    BDSP_ContextCreateVideoSettings videoDownloadBuffer;    
	BDSP_ContextCreateVideoEncodeSettings	videoEncodeDownloadBuffer;
	BDSP_ContextCreateScmSettings	scmDownloadBuffer;
} BDSP_ContextCreateSettings;

/***************************************************************************
Summary:
Get Default DSP Context Settings
***************************************************************************/
void BDSP_Context_GetDefaultCreateSettings(
    BDSP_Handle dsp,
    BDSP_ContextType contextType,            
    BDSP_ContextCreateSettings *pSettings     /* [out] */
    );
    
/***************************************************************************
Summary:
Create a DSP context
***************************************************************************/
BERR_Code BDSP_Context_Create(
    BDSP_Handle dsp,
    const BDSP_ContextCreateSettings *pSettings,
    BDSP_ContextHandle *pContext    /* [out] */
    );

/***************************************************************************
Summary:
Destroy a DSP context
***************************************************************************/
void BDSP_Context_Destroy(
    BDSP_ContextHandle context
    );

/***************************************************************************
Summary:
Context Level Interrupts
***************************************************************************/
typedef struct BDSP_ContextInterruptHandlers
{
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;        
    } watchdog;
} BDSP_ContextInterruptHandlers;

/***************************************************************************
Summary:
Get Currently Registered Interrupt Handlers
***************************************************************************/
void BDSP_Context_GetInterruptHandlers(
    BDSP_ContextHandle context,
    BDSP_ContextInterruptHandlers *pInterrupts     /* [out] */
    );

/***************************************************************************
Summary:
Set Interrupt Handlers 
 
Description: 
To disable any unwanted interrupt, pass NULL for its callback routine
***************************************************************************/
BERR_Code BDSP_Context_SetInterruptHandlers(
    BDSP_ContextHandle context,
    const BDSP_ContextInterruptHandlers *pInterrupts
    );

/***************************************************************************
Summary:
Process a watchdog interrupt for this context
***************************************************************************/
BERR_Code BDSP_Context_ProcessWatchdogInterrupt(
    BDSP_ContextHandle context
    );


#endif

