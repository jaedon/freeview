/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_input_capture.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 5/4/12 6:03p $
 *
 * Module Description: Audio Input Capture Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_input_capture.h $
 * 
 * Hydra_Software_Devel/7   5/4/12 6:03p gskerl
 * SW7425-2075: Added prototype for BAPE_InputCapture_GetStatus() and
 * defined BAPE_InputCaptureStatus struct
 * 
 * Hydra_Software_Devel/6   2/9/12 11:21a jgarrett
 * SW7425-2074: Enabling input halt and revising MAI shutown for
 * multichannel PCM
 * 
 * Hydra_Software_Devel/5   3/1/11 3:25p jgarrett
 * SW7422-146: Moving sample rate to input port from input capture
 * 
 * Hydra_Software_Devel/4   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/3   1/5/11 11:32a jgarrett
 * SW7422-146: Moving sample rate to input capture
 * 
 * Hydra_Software_Devel/2   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BAPE_INPUT_CAPTURE_H_
#define BAPE_INPUT_CAPTURE_H_

/***************************************************************************
Summary:
InputCapture Handle
***************************************************************************/
typedef struct BAPE_InputCapture *BAPE_InputCaptureHandle;

/***************************************************************************
Summary:
InputCapture open settings
***************************************************************************/
typedef struct BAPE_InputCaptureOpenSettings
{
    bool useLargeRingBuffers;   /* If true, large ringbuffers will be used, 
                                   otherwise standard buffers will be used. */
} BAPE_InputCaptureOpenSettings;

/***************************************************************************
Summary:
Get Default InputCapture Settings
***************************************************************************/
void BAPE_InputCapture_GetDefaultOpenSettings(
    BAPE_InputCaptureOpenSettings *pSettings
    );

/***************************************************************************
Summary:
Open an input capture channel
***************************************************************************/
BERR_Code BAPE_InputCapture_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_InputCaptureOpenSettings *pSettings,
    BAPE_InputCaptureHandle *pHandle                    /* [out] */
    );

/***************************************************************************
Summary:
Close an input capture channel
***************************************************************************/
void BAPE_InputCapture_Close(
    BAPE_InputCaptureHandle handle
    );

/***************************************************************************
Summary:
InputCapture Start Settings
***************************************************************************/
typedef struct BAPE_InputCaptureStartSettings
{
    BAPE_InputPort input;
} BAPE_InputCaptureStartSettings;

/***************************************************************************
Summary:
Get Default InputCapture Start Settings
***************************************************************************/
void BAPE_InputCapture_GetDefaultStartSettings(
    BAPE_InputCaptureStartSettings *pSettings       /* [out] */
    );

/***************************************************************************
Summary:
Start InputCapture
***************************************************************************/
BERR_Code BAPE_InputCapture_Start(
    BAPE_InputCaptureHandle handle,
    const BAPE_InputCaptureStartSettings *pSettings
    );

/***************************************************************************
Summary:
Stop InputCapture
***************************************************************************/
void BAPE_InputCapture_Stop(
    BAPE_InputCaptureHandle handle
    );

/***************************************************************************
Summary:
Get Audio Source Connector for output data
***************************************************************************/
void BAPE_InputCapture_GetConnector(
    BAPE_InputCaptureHandle handle,
    BAPE_Connector *pConnector /* [out] */
    );

/***************************************************************************
Summary:
Input Capture Interrupt Handlers
***************************************************************************/
typedef struct BAPE_InputCaptureInterruptHandlers
{
    /* This interrupt fires when the active input has changed to an incompatible format and been halted.
       The application must call BAPE_InputCapture_Stop() and BAPE_InputCapture_Start() to resume processing. */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } inputHalted;
} BAPE_InputCaptureInterruptHandlers;

/***************************************************************************
Summary:
Get Currently Registered Interrupt Handlers
***************************************************************************/
void BAPE_InputCapture_GetInterruptHandlers(
    BAPE_InputCaptureHandle handle,
    BAPE_InputCaptureInterruptHandlers *pInterrupts     /* [out] */
    );

/***************************************************************************
Summary:
Set Interrupt Handlers 
 
Description: 
To disable any unwanted interrupt, pass NULL for its callback routine
***************************************************************************/
BERR_Code BAPE_InputCapture_SetInterruptHandlers(
    BAPE_InputCaptureHandle handle,
    const BAPE_InputCaptureInterruptHandlers *pInterrupts
    );

/***************************************************************************
Summary:
Audio Decoder Status
***************************************************************************/
typedef struct BAPE_InputCaptureStatus
{
    bool running;
    bool halted;
} BAPE_InputCaptureStatus;

/***************************************************************************
Summary:
Get Audio InputCapture Status
***************************************************************************/
void BAPE_InputCapture_GetStatus(
    BAPE_InputCaptureHandle handle,
    BAPE_InputCaptureStatus *pStatus     /* [out] */
    );

#endif /* #ifndef BAPE_INPUT_CAPTURE_H_ */

