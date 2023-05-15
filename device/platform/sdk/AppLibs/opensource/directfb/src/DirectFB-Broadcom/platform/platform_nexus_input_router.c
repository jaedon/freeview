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
 * $brcm_Workfile: platform_nexus_input_router.c $
 *
 * Module Description:DirectFB NIR platform driver. This driver is an interface between 
                      Nexus NIR API and DirectFB input driver.
 *
 *

*****************************************************************************/

#include "platform_init.h"
#include "nexus_input_client.h"
#include <stdlib.h>


BDBG_MODULE(platform_input_router);

#define NUM_INPUT_DEVICES           6           /*Max input devices supported.*/
#define MAX_DFB_NSC_CLIENTS         5           /*Max dfbClientID. 0 to 4: Single dfb client (0) and 1 to 4 for multi DFB clients,*/

#define MAX_NIR_INPUT_EVENTS        10          /*Max NIR events that can be read at a time*/

DFB_PlatformNexusHandle DFB_PlatformInputDevice_Open(
    DFB_PlatformInputDeviceType              device,
    unsigned int                             index,
    const DFB_PlatformInputDeviceSettings   *pSettings )
{
    NEXUS_Error rc;
    char *pClientId;
    NEXUS_InputClientSettings nirClientSettings;
    NEXUS_InputClientHandle inputRouterClientHandle = NULL;
    int dfbClientID = 0;
    /*Fixed Client IDs as per dfb_clientid and device type*/
    int inputRouterClientID[MAX_DFB_NSC_CLIENTS][NUM_INPUT_DEVICES]  = {{0,1,2,3,4,5},
                                                                        {6,7,8,9,10,11},
                                                                        {12,13,14,15,16,17},
                                                                        {18,19,20,21,22,23},
                                                                        {24,25,26,27,28,29},
                                                                       };

    BSTD_UNUSED(index);

    BDBG_MSG(("%s() >> ",__FUNCTION__));

    if(!pSettings)
    {
        BDBG_ERR(("%s(%d): Invalid Parameter!", __FUNCTION__, __LINE__));
        return NULL;
    }

    if ((pClientId = getenv("dfb_clientid")) != NULL)
    {
        dfbClientID = strtoul(pClientId, NULL, 10);
    }

    /*Bounds check*/
    if (((int)dfbClientID < MAX_DFB_NSC_CLIENTS) && ((int)device < NUM_INPUT_DEVICES))
    {
        inputRouterClientHandle = NEXUS_InputClient_Acquire(inputRouterClientID[dfbClientID][device]);
        if(!inputRouterClientHandle)
        {
            BDBG_ERR(("%s(): Cannot acquire NIR Client for clientID %d [%d %d]!", __FUNCTION__, inputRouterClientID[dfbClientID][device], dfbClientID, device));
            return NULL;
        }
    }
    else
    {
        BDBG_ERR(("%s(): NIR Client ID not available %d %d!", __FUNCTION__, dfbClientID, device));
        return NULL;
    }

    NEXUS_InputClient_GetSettings(inputRouterClientHandle, &nirClientSettings);

    /*Copy Client info*/
    nirClientSettings.codeAvailable.callback = pSettings->dataReady.callback;
    nirClientSettings.codeAvailable.context  = pSettings->dataReady.context;
    nirClientSettings.codeAvailable.param    = pSettings->dataReady.param;

    switch (device)
    {
        case DFB_PlatformInputDevice_eInfraRed:
        {
            /*Choose event(s) to receive from NIR server*/
            nirClientSettings.filterMask = (1 << NEXUS_InputRouterDevice_eIrInput);

            break;
        }

        case DFB_PlatformInputDevice_eKeypad:
        {
            /*Choose event(s) to receive from NIR server*/
            nirClientSettings.filterMask = (1 << NEXUS_InputRouterDevice_eKeypad);

            break;
        }

        case DFB_PlatformInputDevice_eKeyboard:
        {
            /*Choose event(s) to receive from NIR server*/
            nirClientSettings.filterMask =  (1 <<NEXUS_InputRouterDevice_eKeyboard);

            break;
        }

        case DFB_PlatformInputDevice_ePointer:
        {
            /*Choose event(s) to receive from NIR server*/
            nirClientSettings.filterMask = (1 << NEXUS_InputRouterDevice_ePointer);

            break;
        }

        default:
        {
            BDBG_ERR(("%s(%d) Unknown or Unsupported Input Device",__FUNCTION__,__LINE__));
            /*Release resources*/
            NEXUS_InputClient_Release(inputRouterClientHandle);
            return NULL;
        }
    }
#if 0
    if((0 == dfbClientID) || 
      ((1==dfbClientID) && (DFB_PlatformInputDevice_eKeyboard == device)) ||
      ((2==dfbClientID) && (DFB_PlatformInputDevice_ePointer== device))   ||
      ((3==dfbClientID) && (DFB_PlatformInputDevice_eInfraRed == device)) ||
      (4 == dfbClientID))
#endif
    /*Set NIR Client Settings*/
    rc = NEXUS_InputClient_SetSettings(inputRouterClientHandle, &nirClientSettings);

    if(NEXUS_SUCCESS != rc)
    {
        BDBG_ERR(("%s(): Cannot set NIR client settings %d ", __FUNCTION__, rc));
        /*Release resources*/
        NEXUS_InputClient_Release(inputRouterClientHandle);
        return NULL;
    }

    BDBG_MSG(("%s() << ",__FUNCTION__));

    return (DFB_PlatformNexusHandle)inputRouterClientHandle;

}

DFB_PlatformResult DFB_PlatformInputDevice_GetDefaultSettings(
    DFB_PlatformInputDeviceType      device,
    DFB_PlatformInputDeviceSettings *pSettings
    )
{
    BSTD_UNUSED(device);
    BSTD_UNUSED(pSettings);

    BDBG_MSG(("%s() >> ",__FUNCTION__));

    memset(pSettings, 0, sizeof(DFB_PlatformInputDeviceSettings));
    
    BDBG_MSG(("%s() <<",__FUNCTION__,__LINE__));

    return DFB_PLATFORM_OK;

}

DFB_PlatformResult DFB_PlatformInputRouterClientDevice_SetSettings(
    DFB_PlatformNexusHandle client,
    DFB_PlatformInputDeviceType      device,
    DFB_PlatformInputDeviceSettings *pSettings 
    )
{
    BSTD_UNUSED(client);
    BSTD_UNUSED(device);
    BSTD_UNUSED(pSettings);

    BDBG_MSG(("%s() >> ",__FUNCTION__));

    /*Input Router Settings implemented in Master Nexus App*/

    BDBG_MSG(("%s() <<",__FUNCTION__,__LINE__));
    return DFB_PLATFORM_UNSUPPORTED;

}

DFB_PlatformResult DFB_PlatformInputDevice_GetEvents(
    DFB_PlatformInputDeviceType   device,
    DFB_PlatformNexusHandle       handle,
    DFB_PlatformInputDeviceEvent *pEvents,
    unsigned int                  numEvents,
    unsigned int                 *pNumEventsRead,
    bool                         *pOverflow
    )
{
    NEXUS_InputRouterCode nexusNIREvents[MAX_NIR_INPUT_EVENTS];
    unsigned numEventsReceived;
    NEXUS_Error rc;
    unsigned int index;

    BDBG_MSG(("%s() >> ",__FUNCTION__));

    if((!handle) || (!pEvents) || (!pOverflow) || (!pNumEventsRead))
    {
        BDBG_ERR(("%s(%d): Invalid Parameter!", __FUNCTION__, __LINE__));
        return DFB_PLATFORM_INVARG;
    }

    if (numEvents <= 0)
    {
        *pNumEventsRead = 0;
        return DFB_PLATFORM_OK;
    }
    else if (numEvents > MAX_NIR_INPUT_EVENTS)
    {
        /*Read up to Max events at a time*/
        numEvents = MAX_NIR_INPUT_EVENTS;
    }

    rc = NEXUS_InputClient_GetCodes(handle, nexusNIREvents, numEvents, &numEventsReceived);
    if(NEXUS_SUCCESS != rc)
    {
        BDBG_ERR(("%s: Cannot get NIR codes from server ", __FUNCTION__));
        return DFB_PLATFORM_FAILURE;
    }

    if (numEventsReceived == 0)
    {
        *pNumEventsRead = 0;
        return DFB_PLATFORM_OK;
    }

    switch (device)
    {
        case DFB_PlatformInputDevice_eInfraRed:
        {
            /*Check received event is from expected input device */
            if (nexusNIREvents[0].deviceType != NEXUS_InputRouterDevice_eIrInput)
            {
                *pNumEventsRead = 0;
                BDBG_ERR(("%s(): NIR Device Type mismatch! Received:%d Expected:%d", __FUNCTION__, nexusNIREvents[0].deviceType, device));
                return DFB_PLATFORM_FAILURE;
            }

            for (index = 0; (index< numEvents) && (index<numEventsReceived); ++index)
            {
                pEvents[index].repeat   = nexusNIREvents[index].data.irInput.repeat;
                pEvents[index].code     = nexusNIREvents[index].data.irInput.code;
            }

            *pNumEventsRead = index;

            /*Overflow can be detected in only Master NIR process.*/
            *pOverflow = 0;

            break;
        }

        case DFB_PlatformInputDevice_eKeypad:
        {
            /*Check whether the received event is from expected input device */
            if (nexusNIREvents[0].deviceType != NEXUS_InputRouterDevice_eKeypad)
            {
                *pNumEventsRead = 0;
                BDBG_ERR(("%s(): NIR Device Type mismatch! Received:%d Expected:%d", __FUNCTION__, nexusNIREvents[0].deviceType, device));
                return DFB_PLATFORM_FAILURE;
            }

            for (index = 0; (index < numEvents) && (index<numEventsReceived); ++index )
            {
                pEvents[index].repeat   = nexusNIREvents[index].data.keypad.repeat;
                pEvents[index].code     = nexusNIREvents[index].data.keypad.code;
            }

            *pNumEventsRead = index;

            /*Overflow can be detected in only Master NIR process.*/
            *pOverflow = 0;

            break;
        }

        case DFB_PlatformInputDevice_eKeyboard:
        {
            /*Check whether the received event is from expected input device */
            if (nexusNIREvents[0].deviceType != NEXUS_InputRouterDevice_eKeyboard)
            {
                *pNumEventsRead = 0;
                BDBG_ERR(("%s(): NIR Device Type mismatch! Received:%d Expected:%d", __FUNCTION__, nexusNIREvents[0].deviceType, device));
                return DFB_PLATFORM_FAILURE;
            }

            for (index = 0; (index < numEvents) && (index<numEventsReceived); ++index )
            {
                pEvents[index].down   = nexusNIREvents[index].data.keyboard.down;
                pEvents[index].code     = nexusNIREvents[index].data.keyboard.code;
            }

            *pNumEventsRead = index;

            /*Overflow can be detected in only Master NIR process.*/
            *pOverflow = 0;

            break;
        }

        case DFB_PlatformInputDevice_ePointer:
        {
            /*Check whether the received event is from expected input device */
            if (nexusNIREvents[0].deviceType != NEXUS_InputRouterDevice_ePointer)
            {
                    *pNumEventsRead = 0;
                    BDBG_ERR(("%s(): NIR Device Type mismatch! Received:%d Expected:%d", __FUNCTION__, nexusNIREvents[0].deviceType, device));
                    return DFB_PLATFORM_FAILURE;
            }

            for (index = 0; (index < numEvents) && (index<numEventsReceived); ++index )
            {
                pEvents[index].x      = nexusNIREvents[index].data.pointer.x;
                pEvents[index].y      = nexusNIREvents[index].data.pointer.y;
                pEvents[index].button = nexusNIREvents[index].data.pointer.button;
            }

            *pNumEventsRead = index;

           /*Overflow can be detected in only Master NIR process.*/
           *pOverflow = 0;

            break;
        }

        default:
        {
            BDBG_WRN(("%s(%d) Device type not supported",__FUNCTION__,__LINE__));
            *pNumEventsRead = 0;
            *pOverflow = 0;
            return DFB_PLATFORM_IDNOTFOUND;
        }
    }

    BDBG_MSG(("%s() <<",__FUNCTION__,__LINE__));

    return DFB_PLATFORM_OK;

}

DFB_PlatformResult DFB_PlatformInputDevice_Close(
    
    DFB_PlatformInputDeviceType device,
    DFB_PlatformNexusHandle     client
    )
{
    BSTD_UNUSED(device);
    BDBG_MSG(("%s() >>",__FUNCTION__));

    if (!client)
    {
        BDBG_ERR(("DFB_PlatformInputDevice_Close(): Invalid NIR Client handle !"));
        return DFB_PLATFORM_INVARG;
    }

    NEXUS_InputClient_Release(client);

    BDBG_MSG(("%s() <<",__FUNCTION__));

    return DFB_PLATFORM_OK;
}

