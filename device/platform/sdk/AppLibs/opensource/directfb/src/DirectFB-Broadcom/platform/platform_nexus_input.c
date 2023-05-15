/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: platform_nexus_input.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/1 $
 * $brcm_Date: 11/2/11 2:21p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_nexus_input.c $
 * 
 * DirectFB_1_4_15_Port/1   11/2/11 2:21p christ
 * SW7420-2121: DirectFB: Refactor input drivers (ir + keypad) to move
 *  Nexus usage into platform layer
 * 
 *****************************************************************************/

#include "platform_init.h"

#if NEXUS_HAS_IR_INPUT
#include "nexus_ir_input.h"
#include "bcmnexus_ir_convert.h"
#endif

#if NEXUS_HAS_KEYPAD
#include "nexus_keypad.h"
#endif

BDBG_MODULE(platform_nexus_input);

DFB_PlatformResult DFB_PlatformInputDevice_GetDefaultSettings(
    DFB_PlatformInputDeviceType      device,
    DFB_PlatformInputDeviceSettings *pSettings )
{

    BDBG_MSG(("%s() >> ",__FUNCTION__));

    switch (device)
    {

        case DFB_PlatformInputDevice_eInfraRed:
        {
#if NEXUS_HAS_IR_INPUT
            NEXUS_IrInputSettings nexusSettings;

            BDBG_MSG(("%s(%d)",__FUNCTION__,__LINE__));
            NEXUS_IrInput_GetDefaultSettings(&nexusSettings);

            pSettings->device.ir.mode = DFB_Platform_P_NexustoDFBIrModeTable[nexusSettings.mode];
            pSettings->device.ir.channel_number = nexusSettings.channel_number;

            pSettings->repeatFilterTime = nexusSettings.repeatFilterTime;
            pSettings->eventQueueSize = nexusSettings.eventQueueSize;
            pSettings->dataReady.callback = nexusSettings.dataReady.callback;
            pSettings->dataReady.context = nexusSettings.dataReady.context;
#endif
            break;
        }

        case DFB_PlatformInputDevice_eKeypad:
        {
#if NEXUS_HAS_KEYPAD
            NEXUS_KeypadSettings nexusSettings;

            BDBG_MSG(("%s(%d)",__FUNCTION__,__LINE__));
            NEXUS_Keypad_GetDefaultSettings(&nexusSettings);

            pSettings->device.keypad.prescale = nexusSettings.prescale;
            pSettings->device.keypad.debounce = nexusSettings.debounce;

            pSettings->repeatFilterTime = nexusSettings.repeatFilterTime;
            pSettings->eventQueueSize = nexusSettings.eventQueueSize;
            pSettings->dataReady.callback = nexusSettings.dataReady.callback;
            pSettings->dataReady.context = nexusSettings.dataReady.context;
#endif
            break;
        }

        default:
        {
            BDBG_MSG(("%s(%d)",__FUNCTION__,__LINE__));
            return DFB_PLATFORM_IDNOTFOUND;
        }
    }

    BDBG_MSG(("%s() <<",__FUNCTION__,__LINE__));
    return DFB_PLATFORM_OK;

}


DFB_PlatformNexusHandle DFB_PlatformInputDevice_Open(
    DFB_PlatformInputDeviceType              device,
    unsigned int                             index,
    const DFB_PlatformInputDeviceSettings   *pSettings)
{

    BDBG_MSG(("%s() >> ",__FUNCTION__));

    switch (device)
    {

        case DFB_PlatformInputDevice_eInfraRed:
        {
#if NEXUS_HAS_IR_INPUT
            NEXUS_IrInputSettings nexusSettings;

            NEXUS_IrInput_GetDefaultSettings(&nexusSettings);

            nexusSettings.repeatFilterTime = pSettings->repeatFilterTime;
            nexusSettings.eventQueueSize = pSettings->eventQueueSize;
            nexusSettings.dataReady.callback = pSettings->dataReady.callback;
            nexusSettings.dataReady.context = pSettings->dataReady.context;
            nexusSettings.mode = DFB_Platform_P_DFBtoNexusIrModeTable[pSettings->device.ir.mode];
            nexusSettings.channel_number = pSettings->device.ir.channel_number;

            BDBG_MSG(("%s(%d)",__FUNCTION__,__LINE__));
            return NEXUS_IrInput_Open(index,&nexusSettings);
#else
            return NULL;
#endif
        }

        case DFB_PlatformInputDevice_eKeypad:
        {
#if NEXUS_HAS_KEYPAD
            NEXUS_KeypadSettings nexusSettings;

            NEXUS_Keypad_GetDefaultSettings(&nexusSettings);

            nexusSettings.repeatFilterTime = pSettings->repeatFilterTime;
            nexusSettings.eventQueueSize = pSettings->eventQueueSize;
            nexusSettings.dataReady.callback = pSettings->dataReady.callback;
            nexusSettings.dataReady.context = pSettings->dataReady.context;

            nexusSettings.prescale = pSettings->device.keypad.prescale;
            nexusSettings.debounce = pSettings->device.keypad.debounce;

            BDBG_MSG(("%s(%d)",__FUNCTION__,__LINE__));
            return NEXUS_Keypad_Open(index,&nexusSettings);
#else
            return NULL;
#endif
        }

        default:
        {
            BDBG_MSG(("%s(%d)",__FUNCTION__,__LINE__));
            return NULL;
        }
    }


}

DFB_PlatformResult DFB_PlatformInputDevice_Close(
    DFB_PlatformInputDeviceType device,
    DFB_PlatformNexusHandle     handle )
{

    BDBG_MSG(("%s() >>",__FUNCTION__));

    switch (device)
    {

        case DFB_PlatformInputDevice_eInfraRed:
        {
#if NEXUS_HAS_IR_INPUT
            BDBG_MSG(("%s(%d)",__FUNCTION__,__LINE__));
            NEXUS_IrInput_Close(handle);
#endif
            break;
        }

        case DFB_PlatformInputDevice_eKeypad:
        {
#if NEXUS_HAS_KEYPAD
            BDBG_MSG(("%s(%d)",__FUNCTION__,__LINE__));
            NEXUS_Keypad_Close(handle);
#endif
            break;
        }

        default:
            return DFB_PLATFORM_FAILURE;
    }

    BDBG_MSG(("%s() <<",__FUNCTION__));

    return DFB_PLATFORM_OK;

}

DFB_PlatformResult DFB_PlatformInputDevice_GetEvents(
    DFB_PlatformInputDeviceType   device,
    DFB_PlatformNexusHandle       handle,
    DFB_PlatformInputDeviceEvent *pEvents,
    unsigned int                  numEvents,
    unsigned int                 *pNumEventsRead,
    bool                         *pOverflow )
{

    switch (device)
    {

        case DFB_PlatformInputDevice_eInfraRed:
        {
#if NEXUS_HAS_IR_INPUT
            NEXUS_Error res;
            NEXUS_IrInputEvent nexusEvents[10];

            res = NEXUS_IrInput_GetEvents(handle,nexusEvents,numEvents,pNumEventsRead,pOverflow);

            if (res == NEXUS_SUCCESS)
            {
                if (numEvents)
                {
                    unsigned int i;
                    for (i = 0; i < numEvents; ++i )
                    {
                        pEvents[i].repeat   = nexusEvents[i].repeat;
                        pEvents[i].code     = nexusEvents[i].code;
                        pEvents[i].codeHigh = nexusEvents[i].codeHigh;
                        pEvents[i].interval = nexusEvents[i].interval;
                    }
                }
            }
            else
                return DFB_PLATFORM_FAILURE;
#endif
            break;
        }

        case DFB_PlatformInputDevice_eKeypad:
        {
#if NEXUS_HAS_KEYPAD
            NEXUS_Error res;
            NEXUS_KeypadEvent nexusEvents[10];

            res = NEXUS_Keypad_GetEvents(handle,nexusEvents,numEvents,pNumEventsRead,pOverflow);

            if (res == NEXUS_SUCCESS)
            {
                if (numEvents)
                {
                    unsigned int i;
                    for (i = 0; i < numEvents; ++i )
                    {
                        pEvents[i].repeat   = nexusEvents[i].repeat;
                        pEvents[i].code     = nexusEvents[i].code;                    }
                }
            }
            else
                return DFB_PLATFORM_FAILURE;
#endif
            break;
        }

        default:
            return DFB_PLATFORM_FAILURE;
    }

    return DFB_PLATFORM_OK;
}
