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
 * Module Description: Broadcom DirectFB Input Event handler. Module is based on lunux input driver
 *                     implementation. Handles keyboard and pointer input devices.
 *
 *

*****************************************************************************/

#include "keyboard_pointer.h"


D_DEBUG_DOMAIN( bcmnexusKeyboardPointerInput, "KeyboardPointer/bcmNexus", "Keyboard Pointer input driver" );

DFB_INPUT_DRIVER( bcmKeyboardPointerInputDevice )

#define MAX_INPUT_EVENTS                10
#define NUM_INPUT_DEVICES_SUPPORTED     2 /*Total input devices supported by this driver*/
#define KEYBOARD_DEVICE                 0 /*input device number corresponding to /dev/eventsX*/
#define POINTER_DEVICE                  1 /*input device number corresponding to /dev/eventsX*/
#define INPUT_EVENT_TIMEOUT             200


/*  Data ready callback for the keyboard and pointer events*/
static void BDFB_Input_DataReady_CB(void *context, int param)
{
    FusionSkirmish *lock = (FusionSkirmish *)context;
    BSTD_UNUSED(param);

    fusion_skirmish_prevail( lock );
    fusion_skirmish_notify(  lock );
    fusion_skirmish_dismiss( lock );
}

/*
 * Input thread reading from device.
 * Generates events on incoming data.
 */

static void*
BDFB_KeyboardPointerEventThread( DirectThread *thread, void *driver_data )
{
    BDFBInputDeviceData            *data = (BDFBInputDeviceData*) driver_data;
    unsigned int                   i;
    size_t                         numEventsRead;
    bool                           overflow;
    DFB_PlatformInputDeviceEvent   inputEvent[MAX_INPUT_EVENTS];
    DFB_PlatformResult             rc;
    DirectResult                   status= 0;;
    struct input_event             levt[MAX_INPUT_EVENTS];/*Linux input event*/
    DFB_PlatformInputDeviceEvent   prevPointerEvent;
    DFB_PlatformInputDeviceEvent   prevKeyEvent;


    D_DEBUG_AT( bcmnexusKeyboardPointerInput, "%s()", __FUNCTION__ );

    /*Init prevEvent*/
    prevPointerEvent.x = 0;
    prevPointerEvent.y = 0;
    prevPointerEvent.button = 0;
    prevKeyEvent.code = 0xFFFFFFFF;
    prevKeyEvent.down = 0;

    while(!data->cancel_thread)
    {
        DFBInputEvent devt = { .type = DIET_UNKNOWN };
        /* Wait for Nexus NIR Callback notification */
        fusion_skirmish_prevail( &data->lock );
        status = fusion_skirmish_wait( &data->lock, INPUT_EVENT_TIMEOUT );
        fusion_skirmish_dismiss( &data->lock );

        direct_thread_testcancel( thread );

        /* timeout? */
        if (status == DR_TIMEOUT)
        {
            continue;
        }
        /*Process Kyboard Event*/
        if(data->keyboardInput)
        {
            rc = DFB_PlatformInputDevice_GetEvents(DFB_PlatformInputDevice_eKeyboard, data->keyboardInput, inputEvent, MAX_INPUT_EVENTS, &numEventsRead, &overflow);
            if(rc != NEXUS_SUCCESS)
            {
                D_ERROR("%s: DFB_PlatformInputDevice_GetEvents failed %d", __FUNCTION__, rc);
                continue;
            }
            if(numEventsRead < 1)
            {
                continue;
            }

            D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s(): Keyboard numEventsRead: %d ", __FUNCTION__,  numEventsRead);

            for(i=0; i<numEventsRead ; i++)
            {
                levt[i].type = EV_KEY;
                levt[i].code = inputEvent[i].code;
                D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s(): Keyboard code: 0x%x down: %d ", __FUNCTION__, inputEvent[i].code, inputEvent[i].down);
                if((prevKeyEvent.code == inputEvent[i].code ) && (prevKeyEvent.down == inputEvent[i].down))
                {
                    D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s(): Keyboard repeat event ", __FUNCTION__);
                    levt[i].value = 2;
                }
                else
                {
                    levt[i].value = inputEvent[i].down ?1:0;
                }

                prevKeyEvent.code = inputEvent[i].code;
                prevKeyEvent.down = inputEvent[i].down;
            }
          
        }
        else if(data->pointerInput)
        {
            rc = DFB_PlatformInputDevice_GetEvents(DFB_PlatformInputDevice_ePointer, data->pointerInput, inputEvent, MAX_INPUT_EVENTS, &numEventsRead, &overflow);

            if(rc != NEXUS_SUCCESS)
            {
                D_ERROR("%s: DFB_PlatformInputDevice_GetEvents failed %d", __FUNCTION__, rc);
                continue;
            }

            if(numEventsRead<1)
            {
                continue;
            }

            D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s(): prevPointerEvent.button:%d inputEvent[0].button:%d ", __FUNCTION__, prevPointerEvent.button,inputEvent[0].button );

            if(prevPointerEvent.button != inputEvent[0].button)
            {
                static int prevBtnCode;
                levt[0].value = inputEvent[0].button?1:0; /*mouse button press/release*/
                levt[0].type = EV_KEY;

                switch(inputEvent[0].button)
                {
                    case 1:
                        levt[0].code = BTN_LEFT;
                        break;
                    case 2:
                        levt[0].code = BTN_RIGHT;
                        break;
                    case 4:
                        levt[0].code = BTN_MIDDLE;
                        break;
                    default:
                    {
                        /*Release Code*/
                        if(0 == inputEvent[0].button)
                        {
                            levt[0].code = prevBtnCode;
                        }
                    }
                        break;
                }

                prevBtnCode = levt[0].code;
                prevPointerEvent.button = inputEvent[0].button;
            }
            else
            {
                for(i=0; i<numEventsRead ; i++)
                {
                    levt[i].type = EV_ABS;

                    if(prevPointerEvent.x != inputEvent[i].x) {levt[i].code = ABS_X; /*REL_X;*/ levt[i].value = inputEvent[i].x;}

                    if(prevPointerEvent.y != inputEvent[i].y) {levt[i].code = ABS_Y; /*REL_Y;*/ levt[i].value = inputEvent[i].y;}

                    prevPointerEvent.x = inputEvent[i].x;
                    prevPointerEvent.y = inputEvent[i].y;

                }
            }
        }

        direct_thread_testcancel(thread);
        for(i=0; i<numEventsRead ; i++)
        {
            DFBInputEvent temp = { .type = DIET_UNKNOWN };

            if(!translate_event( &levt[i], &temp ))
            {
                printf(" Continue \n");
                continue;
            }

            /* Flush previous event with DIEF_FOLLOW? */
            if(devt.type != DIET_UNKNOWN)
            {
                flush_xy( data, false );

                /* Signal immediately following event. */
                devt.flags |= DIEF_FOLLOW;

                dfb_input_dispatch( data->device, &devt );

                devt.type  = DIET_UNKNOWN;
                devt.flags = DIEF_NONE;
            }

            devt = temp;

            if(D_FLAGS_IS_SET( devt.flags, DIEF_AXISREL ) && devt.type == DIET_AXISMOTION &&
               dfb_config->mouse_motion_compression)
            {
                switch (devt.axis) 
                {
                    case DIAI_X:
                        data->dx += devt.axisrel;
                        continue;

                    case DIAI_Y:
                        data->dy += devt.axisrel;
                        continue;

                    default:
                      break;
                }
            }
            /* Event is dispatched in next round of loop. */
        }

        /* Flush last event without DIEF_FOLLOW. */
        if(devt.type != DIET_UNKNOWN)
        {
            flush_xy( data, false );
            dfb_input_dispatch( data->device, &devt );
        }
        else
           flush_xy( data, true );
    }

    if (status <= 0)
        D_PERROR ("linux_input thread died");

        return NULL;
}

static int
driver_get_available( void )
{
    /*Number of Input Devices handled by this driver: 1.Keyboard 2.Pointer*/
    return NUM_INPUT_DEVICES_SUPPORTED;
}

static void
driver_get_info( InputDriverInfo *info )
{
    if(info)
    {
        /* fill driver info structure */
        snprintf( info->name,
               DFB_INPUT_DRIVER_INFO_NAME_LENGTH, "Broadcom Keyboard & Mouse Driver" );

        snprintf( info->vendor,
               DFB_INPUT_DRIVER_INFO_VENDOR_LENGTH, "Broadcom" );

        info->version.major = 0;
        info->version.minor = 1;
    }

    return;
}

static DFBResult
driver_open_device( CoreInputDevice     *device,
                       unsigned int         number,
                       InputDeviceInfo      *info,
                       void                 **driver_data )
{
    BDFBInputDeviceData           *data;
    DFB_PlatformInputDeviceSettings  inputDeviceSettings;

    D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s() >>  Keyboard Pointer", __FUNCTION__);

    if((!device) || (!info) || (!driver_data))
    {
        D_ERROR("KeyboardPointer/bcmNexus: %s Invalid Parameter", __FUNCTION__);
        return DFB_INIT;
    }

    /* Allocate and fill private data */
    data = D_CALLOC( 1, sizeof(BDFBInputDeviceData) );
    if(!data) {
        return D_OOM();
    }

    data->has_keys = true;
    data->cancel_thread = false;

    /*Device number has one to one correspondence to /dev/eventsX*/
    if(KEYBOARD_DEVICE == number)
    {
        /* Decide which primary input device to be. */
        info->desc.type |= DIDTF_KEYBOARD;
        info->prefered_id = DIDID_KEYBOARD;

        /* Create the locks/semaphores */
        fusion_skirmish_init( &data->lock,  "Broadcom Keyboard Event Semaphore",  dfb_core_world(NULL) );

        inputDeviceSettings.dataReady.callback = BDFB_Input_DataReady_CB;
        inputDeviceSettings.dataReady.context = &data->lock;
        inputDeviceSettings.dataReady.param = EVENT_KEY;
        data->pointerInput = NULL;
        data->keyboardInput = DFB_PlatformInputDevice_Open(DFB_PlatformInputDevice_eKeyboard,0, &inputDeviceSettings);

        if (data->keyboardInput == NULL)
        {
            D_ERROR("KeyboardPointer/bcmNexus: %s failed to initialize keyboard driver", __FUNCTION__);

            D_FREE(data);

            return DFB_INIT;
        }

        D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s() Keyboard Device Handle:%p ", __FUNCTION__, data->keyboardInput );

        /* Start input thread */
        data->thread = direct_thread_create( DTT_INPUT, BDFB_KeyboardPointerEventThread, data, "Keyboard Pointer Input" );
    }
    else if(POINTER_DEVICE == number)
    {
        /* Decide which primary input device to be. */
        info->desc.type |= DIDTF_MOUSE;
        info->prefered_id = DIDID_MOUSE;

        /* Create the locks/semaphores */
        fusion_skirmish_init( &data->lock,  "Broadcom Pointer Event Semaphore",  dfb_core_world(NULL) );

        inputDeviceSettings.dataReady.callback = BDFB_Input_DataReady_CB;
        inputDeviceSettings.dataReady.context = &data->lock;
        inputDeviceSettings.dataReady.param = EVENT_ABS;
        data->keyboardInput = NULL;

        data->pointerInput = DFB_PlatformInputDevice_Open(DFB_PlatformInputDevice_ePointer,0, &inputDeviceSettings);

        if(data->pointerInput == NULL)
        {
            D_ERROR("KeyboardPointer/bcmNexus: %s failed to initialize pointer driver", __FUNCTION__);

            D_FREE(data);

            return DFB_INIT;
        }

        D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s() Pointer Device Handle: %p ", __FUNCTION__, data->pointerInput );

        /* start input thread */
        data->thread = direct_thread_create( DTT_INPUT, BDFB_KeyboardPointerEventThread, data, "Ponter Input" );
    }
    else
    {
        D_ERROR("KeyboardPointer/bcmNexus: %s Unsupported device number", __FUNCTION__);
        D_FREE(data);
        return DFB_INIT;
    }

    data->device = device;

    /* set private data pointer */
    *driver_data = data;

    D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s() << Keyboard Pointer", __FUNCTION__);

    return DFB_OK;
}


static void
driver_close_device( void *driver_data )
{
    BDFBInputDeviceData *data = (BDFBInputDeviceData*) driver_data;;

    D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s() >> Keyboard Pointer Device ", __FUNCTION__);
    if(data)
    {
        if(data->keyboardInput)
        {
            /* Close the keyboard interface */
            if (data->keyboardInput) DFB_PlatformInputDevice_Close(DFB_PlatformInputDevice_eKeyboard, data->keyboardInput );
        }

        if(data->pointerInput)
        {
            /* Close the pointer interface */
            DFB_PlatformInputDevice_Close(DFB_PlatformInputDevice_ePointer, data->pointerInput );
        }

        /* stop input thread */
        data->cancel_thread = true;

        direct_thread_join( data->thread );
        direct_thread_destroy( data->thread );
             
        fusion_skirmish_prevail( &data->lock );
        fusion_skirmish_notify( &data->lock );
        fusion_skirmish_dismiss( &data->lock );

        fusion_skirmish_destroy(&data->lock);
        D_FREE( data );
    }
    else
    {
        D_ERROR("KeyboardPointer/bcmNexus: %s Invalid Parameter", __FUNCTION__);
    }

    D_DEBUG_AT(bcmnexusKeyboardPointerInput, "%s() << Keyboard Pointer Device ", __FUNCTION__);
    return;
}

