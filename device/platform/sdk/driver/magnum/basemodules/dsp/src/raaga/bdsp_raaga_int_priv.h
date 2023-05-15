/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_int_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/15/11 5:53p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/7425/raaga/api/bdsp_raaga_int_priv.h $
 * 
 * Hydra_Software_Devel/2   6/15/11 5:53p gautamk
 * SW7425-683: [7425] Adding support for Power management in BDSP
 * 
 * Hydra_Software_Devel/1   4/6/11 2:28p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/2   2/26/11 2:00a srajapur
 * SW7422-294 :[7422]  Adding watchdog recovery API support
 * 
 * Hydra_Software_Devel/1   1/13/11 5:32a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 ***************************************************************************/

#ifndef BDSP_RAAGA_INT_PRIV_H_
#define BDSP_RAAGA_INT_PRIV_H_
#include "bdsp_raaga_types.h"
#include "bdsp_raaga_priv.h"


BERR_Code BDSP_Raaga_P_AckInstall(
    void *pDeviceHandle
);
BERR_Code BDSP_Raaga_P_AckUninstall(
    void *pDeviceHandle
);

BERR_Code BDSP_Raaga_P_InterruptInstall (
	void    *pTaskHandle		/* [in] Raptor Channel handle */
);

BERR_Code BDSP_Raaga_P_InterruptUninstall (
    void    *pTaskHandle        /* [in] Raptor Channel handle */
);


BERR_Code BDSP_Raaga_P_ContextInterruptInstall (
    void    *pContextHandle        
);

BERR_Code BDSP_Raaga_P_ContextInterruptUninstall (
    void    *pContextHandle        
);

void BDSP_Raaga_P_GetAudioInterruptHandlers_isr(
    void *pTaskHandle,
    BDSP_AudioInterruptHandlers *pHandlers);

BERR_Code BDSP_Raaga_P_SetAudioInterruptHandlers_isr(
    void *pTaskHandle,
    const BDSP_AudioInterruptHandlers *pHandlers);

BERR_Code BDSP_Raaga_P_SetInterruptHandlers(
    void *pContextHandle,
    const BDSP_ContextInterruptHandlers *pInterrupts);

BERR_Code BDSP_Raaga_P_ProcessWatchdogInterrupt(
    void *pContextHandle);

void BDSP_Raaga_P_GetInterruptHandlers(
    void *pContextHandle, 
    BDSP_ContextInterruptHandlers *pInterrupts);

BERR_Code BDSP_Raaga_P_EnableDspWatchdogTimer (
                       void    *pDeviceHandle,
                       bool                bEnable
                       );


#endif
