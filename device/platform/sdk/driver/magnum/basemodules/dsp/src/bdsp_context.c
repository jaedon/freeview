/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_context.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/6/11 2:16p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/common/bdsp_context.c $
 * 
 * Hydra_Software_Devel/1   4/6/11 2:16p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
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
 * Hydra_Software_Devel/1   12/15/10 2:00p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/

#include "bdsp.h"
#include "bdsp_priv.h"

BDBG_MODULE(bdsp_context);

void BDSP_Context_GetDefaultCreateSettings(
    BDSP_Handle dsp,
    BDSP_ContextType contextType,            
    BDSP_ContextCreateSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(dsp, BDSP_Device);

    BDBG_ASSERT(contextType < BDSP_ContextType_eMax);
    
    BDBG_ASSERT(NULL != pSettings);
    if ( dsp->getDefaultContextSettings ) 
    {
        dsp->getDefaultContextSettings(dsp->pDeviceHandle,contextType, pSettings);
    }
    else
    {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    }
}

BERR_Code BDSP_Context_Create(
    BDSP_Handle dsp,
    const BDSP_ContextCreateSettings *pSettings,
    BDSP_ContextHandle *pContext    /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(dsp, BDSP_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pContext);

    if ( dsp->createContext ) 
    {
        return dsp->createContext(dsp->pDeviceHandle, pSettings, pContext);
    }
    else
    {
        *pContext = NULL;
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void BDSP_Context_Destroy(
    BDSP_ContextHandle context
    )
{
    BDBG_OBJECT_ASSERT(context, BDSP_Context);
    BDBG_ASSERT(NULL != context->destroy);
    context->destroy(context->pContextHandle);
}

void BDSP_Context_GetInterruptHandlers(
    BDSP_ContextHandle context,
    BDSP_ContextInterruptHandlers *pInterrupts     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(context, BDSP_Context);
    BDBG_ASSERT(NULL != pInterrupts);
    if ( context->getInterruptHandlers ) 
    {
        context->getInterruptHandlers(context->pContextHandle, pInterrupts);
    }
}

BERR_Code BDSP_Context_SetInterruptHandlers(
    BDSP_ContextHandle context,
    const BDSP_ContextInterruptHandlers *pInterrupts
    )
{
    BDBG_OBJECT_ASSERT(context, BDSP_Context);
    BDBG_ASSERT(NULL != pInterrupts);
    if ( context->setInterruptHandlers ) 
    {
        return context->setInterruptHandlers(context->pContextHandle, pInterrupts);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_Context_ProcessWatchdogInterrupt(
    BDSP_ContextHandle context
    )
{
    BDBG_OBJECT_ASSERT(context, BDSP_Context);
    if ( context->processWatchdogInterrupt ) 
    {
        return context->processWatchdogInterrupt(context->pContextHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

