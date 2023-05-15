/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bcmnexus_key.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/4 $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/inputdrivers/bcmnexus/core/bcmnexus_key.c $
 * 
 * DirectFB_1_4_15_Port/4   3/7/12 5:25p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/3   2/7/12 5:19p christ
 * SW7425-2341: DirectFB: BRCM input drivers fail to check for valid
 *  hardware handles.
 *
 * DirectFB_1_4_15_Port/2   11/2/11 2:23p christ
 * SW7420-2121: DirectFB: Refactor input drivers (ir + keypad) to move
 *  Nexus usage into platform layer
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 6:09p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/4   7/26/10 5:31p jackf
 * SW35230-771: Added include for shim_nexus.h.
 *
 * DirectFB_1_4_Port/3   7/7/10 2:13p robertwm
 * SW7405-4604: DirectFB: IR and keypad drivers need to generate single
 *  KEYPRESS and KEYRELEASE events.
 *
 * DirectFB_1_4_Port/2   3/12/10 11:56a robertwm
 * SW7405-4022: DirectFB: IR remote fails to work when using
 *  GetInputDevice.
 *
 * DirectFB_1_4_Port/1   2/24/10 10:27a robertwm
 * SW7468-125: DirectFB: Provide ability to separately build IR and Keypad
 *  input drivers.
 *
 * This code implements a DirectFB input driver for the keypad.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <directfb.h>
#include <directfb_keynames.h>

#include <core/coredefs.h>
#include <core/coretypes.h>

#include <core/input.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/messages.h>
#include <direct/thread.h>

#include <core/core.h>
#include <core/input_driver.h>

#include <misc/conf.h>

/*
** Broadcom specific includes.
*/
#include "platform_init.h"

#if defined(HUMAX_PLATFORM_BASE)
#include <core/system.h>
#include "bcmnexus.h"
#include "nexus_types.h"
#include "nexus_led.h"
#include "nexus_keypad.h"
#include "bstd.h"
#include "bkni.h"
#endif

D_DEBUG_DOMAIN( bcmnexusKeyInput, "bcmNexus/KeyInput", "Broadcom NEXUS Keypad Input Event driver" );
DFB_INPUT_DRIVER( bcmnexusKeyInputDevice )

/*
** type definitions.
*/

/*
** For mapping hardware codes to DirectFB key codes.
*/
typedef struct {

    DFBInputDeviceKeySymbol     dfbKeySymbol;
    uint32_t                    hwEventCode;

} KeyMapElement;


/*
** A container for the keypad variables.
*/
typedef struct  {

    DFB_PlatformNexusHandle     keypad;
    struct timeval              timePrevEvent;  /* for hardware that doesn't implement "repeat" detection */
    CoreInputDevice *           pDfbDevice;     /* to access the DFB event queue                                    */
    DirectThread               *thread;         /* Input key event handling thread */
    FusionSkirmish              lock;           /* Lock */
    bool                        cancel_thread;  /* set to true to cancel thread */

} BDFB_kpd_info;

/*
** Constant definitions.
*/

/*
** If hw events are arriving faster than every "DEFAULT_REPEAT_MILLI_SECS",
** then we'll make the gross assumption that the key is being held down.
** This is used in the event handlers to debounce the keys.
*/

#define DEFAULT_REPEAT_MILLI_SECS   150

#define DEFAULT_DEBOUNCE_COUNT      36

/*
** Define maximum number of events that can be consumed in a single callback.
*/
#define NUM_EVENTS 10

/*
** Define the number of initial repeats to skip/discard before issuing
** a keypad key event.
*/
#define DEFAULT_KEYPAD_SKIP_REPEAT_COUNT 1

/*
** Keypad key code maps.
**
** The "hwEventCode"s  in the following table are the values generated by the keypad hardware.
**
** The  "DIKS_xxx" constants are for the basic keyboard mapping (defined in "Directfb_keyboard.h").
** The  "DIKI_xxx" constants are for the advanced keyboard mapping.
*/

#if defined(HUMAX_PLATFORM_BASE)
KeyMapElement KpdBasicCodeMap [] =
{
    { DIKI_LEFT,     0x00004000 }, /*DI_KEY_FRONT_LEFT*/
    { DIKI_RIGHT,    0x00008000 }, /*DI_KEY_FRONT_RIGHT*/
    { DIKI_DOWN,     0x00002000 }, /*DI_KEY_FRONT_DOWN*/
    { DIKI_UP,       0x00001000 }  /*DI_KEY_FRONT_UP*/
};

KeyMapElement KpdAdvanCodeMap [] =
{
    { DIKS_CURSOR_LEFT,     0x00004000 },/*DI_KEY_FRONT_LEFT */
    { DIKS_CURSOR_RIGHT,    0x00008000 },/*DI_KEY_FRONT_RIGHT */
    { DIKS_CURSOR_DOWN,     0x00002000 },/*DI_KEY_FRONT_DOWN */
    { DIKS_CURSOR_UP,       0x00001000 },/*DI_KEY_FRONT_UP */
    { DIKS_CHANNEL_UP,      0x00000008 },/*DI_KEY_FRONT_CH_UP */
    { DIKS_CHANNEL_DOWN,    0x00000004 },/*DI_KEY_FRONT_CH_DOWN */
    { DIKS_VOLUME_UP,       0x00000002 },/*DI_KEY_FRONT_VOL_UP */
    { DIKS_VOLUME_DOWN,     0x00000001 },/*DI_KEY_FRONT_VOL_DOWN */
    { DIKS_POWER,           0x00000040 },/*DI_KEY_FRONT_PWR */	
    { DIKS_MENU,            0x00000010 },/*DI_KEY_FRONT_MENU */
    { DIKS_AUX,             0x00000020 },/*DI_KEY_FRONT_INPUT */
    { DIKS_SELECT,          0x00000080 },/*DI_KEY_FRONT_OK */
    { DIKS_PLAYPAUSE,       0x00000100 },/*DI_KEY_FRONT_DVD_PLAY_PAUSE */
    { DIKS_STOP,            0x00000200 },/*DI_KEY_FRONT_DVD_STOP */
    { DIKS_EJECT,           0x00000400 },/*DI_KEY_FRONT_DVD_OPEN_CLOSE */
    { DIKS_TV,              0x00000800 },/*DI_KEY_FRONT_TV_RADIO */
    { DIKS_EPG,             0x00010000 },/*DI_KEY_FRONT_GUIDE */
    { DIKS_BACK,            0x00020000 },/*DI_KEY_FRONT_BACK */
    { DIKS_INFO,            0x00040000 },/*DI_KEY_FRONT_DETAIL_INFO */
    { DIKS_CUSTOM99,        0x00080000 },/*DI_KEY_FRONT_HIDDEN */
    { DIKS_CUSTOM98,        0x00100000 },/*DI_KEY_FRONT_WPS */
    { DIKS_CUSTOM59,        0x00200000 }/*DI_KEY_FRONT_NETWORK */
	
};

#else
KeyMapElement KpdBasicCodeMap [] =
{
    { DIKI_LEFT,     0xFFEF },
    { DIKI_RIGHT,    0xFFDF },
    { DIKI_DOWN,     0xFFFD },
    { DIKI_UP,       0xFEFF },
    { DIKI_ENTER,    0xEFFF }           /* the select key on the keypad */
};

KeyMapElement KpdAdvanCodeMap [] =
{
    { DIKS_CURSOR_LEFT,     0xFFEF },
    { DIKS_CURSOR_RIGHT,    0xFFDF },
    { DIKS_CURSOR_DOWN,     0xFFFD },
    { DIKS_CURSOR_UP,       0xFEFF },
    { DIKS_SELECT,          0xEFFF },
    { DIKS_CHANNEL_UP,      0xFFFB },
    { DIKS_CHANNEL_DOWN,    0xFFBF },
    { DIKS_VOLUME_UP,       0xFBFF },
    { DIKS_VOLUME_DOWN,     0xBFFF },
    { DIKS_MENU,            0xDFFF },
    { DIKS_INFO,            0xFDFF }             /* the "guide" key on the on the keypad  */
};
#endif


/*
** Global data.
*/

static int g_iKpdNumBasicCodes;
static int g_iKpdNumAdvanCodes;

/*
** Broadcom specific routines.
*/

/*
** Routine to map the keypad hw codes to DirectFB keycodes.
*/
static void BDFB_lookup_kpd_keycodes(
    uint32_t         uiEventCode,
    DFBInputEvent   *pDfbEvent
    )
{
    int i;

    /*
    **  Hardware keycode, i.e. no mapping applied.
    */
    pDfbEvent->key_code = uiEventCode;

    /*
    ** Basic mapping, modifier independent.
    ** If we don't get a "hit" on the hardware key code, we'll return "DIKI_UNKNOWN"
    */

    pDfbEvent->key_id= DIKI_UNKNOWN;

    for( i=0; i < g_iKpdNumBasicCodes; i++ )
    {
        if ( uiEventCode == KpdBasicCodeMap[ i ].hwEventCode )
        {
            pDfbEvent->key_id = KpdBasicCodeMap[ i ].dfbKeySymbol;
            break;
        }
    }

    /*
    ** Advanced mapping, unicode compatible, modified dependent.
    ** If we don't get a "hit" on the hardware key code, we'll return "DIKS_NULL".
    ** Is this the right solution for the real code?
    */

    pDfbEvent->key_symbol = DIKS_NULL;


    for( i=0; i < g_iKpdNumAdvanCodes; i++ )
    {
        if ( uiEventCode == KpdAdvanCodeMap[ i ].hwEventCode )
        {
            pDfbEvent->key_symbol = KpdAdvanCodeMap[ i ].dfbKeySymbol;
            break;
        }
    }

    return;

}       /* end of BDFB_lookup_kpd_keycodes() */

/*
**  Data ready callback for the IR receiver.
*/
static void BDFB_input_dataReady(void *context, int param)
{
    FusionSkirmish *lock = (FusionSkirmish *)context;

    BSTD_UNUSED(param);

    fusion_skirmish_prevail( lock );
    fusion_skirmish_notify( lock );
    fusion_skirmish_dismiss( lock );
}       /* end of BDFB_input_dataReady() */

static void *BDFB_KeypadThread(DirectThread *thread, void *arg)
{
    DirectResult                    res;
    DFB_PlatformResult              rc;
    DFB_PlatformInputDeviceEvent    keypadEvent[NUM_EVENTS];
    size_t                          numEventsRead, i;
    bool                            overflow, bKeyRepeated = false;
    uint32_t                        eventCode, lastEventCode = 0;
    int                             skipRepeatCnt = 0;
    DFBInputEvent                   dfbEvent;
    CoreInputDevice *               pDfbDevice;
#if defined(HUMAX_PLATFORM_BASE)
    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;
    unsigned int                timeout = brcm_config->bcmnexus_key_timeout;
#else
    unsigned int                    timeout = dfb_config->bcmnexus_key_timeout;
#endif

    BSTD_UNUSED(thread);

    BDFB_kpd_info *pInputInfo = (BDFB_kpd_info *)arg;

    memset( &dfbEvent, 0, sizeof( dfbEvent ) );

    while ( pInputInfo && pInputInfo->cancel_thread == false)
    {
        /* Wait for Nexus Keypad Callback notification */
        fusion_skirmish_prevail( &pInputInfo->lock );
        res = fusion_skirmish_wait( &pInputInfo->lock, timeout );
        fusion_skirmish_dismiss( &pInputInfo->lock );

        if ( pInputInfo->cancel_thread == false )
        {
            /*
            ** If there is queue available to receive the data, send it along.
            ** This should always be true for this driver, but will check just to
            ** be certain.
            */
            pDfbDevice = (CoreInputDevice *)( pInputInfo->pDfbDevice );

            if ( pDfbDevice )
            {
                /* If no keycode event received for a given timeout, then generate a KEYRELEASE event */
                if (res == DR_TIMEOUT)
                {
                    if (dfbEvent.type & DIET_KEYPRESS)
                    {
                        dfbEvent.type = DIET_KEYRELEASE;
                        dfb_input_dispatch( pDfbDevice, &dfbEvent );
                    }
                }
                else
                {
                    rc = DFB_PlatformInputDevice_GetEvents(DFB_PlatformInputDevice_eKeypad,pInputInfo->keypad, keypadEvent, NUM_EVENTS, &numEventsRead, &overflow);
                    if ( rc != NEXUS_SUCCESS)
                    {
                        D_ERROR("%s: NEXUS_Keypad_GetEvents failed %d\n", __FUNCTION__, rc);
                        continue;
                    }
                    if ( numEventsRead<1)
                    {
                        continue;
                    }
                    else if ( overflow || numEventsRead > NUM_EVENTS)
                    {
                        D_ERROR("%s: NEXUS_Keypad_GetEvents under/overflowed events read=%d\n",__FUNCTION__, numEventsRead);
                    }

                    for (i=0; i<numEventsRead; i++)
                    {
                        D_DEBUG_AT(bcmnexusKeyInput, "%s: Keypad event %d (%d,%d)\n", __FUNCTION__, i, keypadEvent[i].code, keypadEvent[i].repeat);

                        eventCode = keypadEvent[i].code;

                        /* If we've not received a KEYRELEASE timeout and we are pressing a new key
                           then we need to generate a KEYRELEASE event first. */
                        if (timeout && (eventCode != lastEventCode) && (dfbEvent.type == DIET_KEYPRESS))
                        {
                            dfbEvent.type = DIET_KEYRELEASE;
                            dfb_input_dispatch( pDfbDevice, &dfbEvent );
                        }

                        memset( &dfbEvent, 0, sizeof( dfbEvent ) );

                        lastEventCode = eventCode;

                        /* Setup the timestamp */
                        gettimeofday(&dfbEvent.timestamp, NULL);

                        /* Initialise Skip Repeat Count if first repeat noticed */
                        if ((bKeyRepeated == false) && (keypadEvent[i].repeat == true)) {
#if defined(HUMAX_PLATFORM_BASE) 
                            skipRepeatCnt = brcm_config->bcmnexus_key_repeat_skip ? brcm_config->bcmnexus_key_repeat_skip : DEFAULT_KEYPAD_SKIP_REPEAT_COUNT;
#else
                            skipRepeatCnt = dfb_config->bcmnexus_key_repeat_skip ? dfb_config->bcmnexus_key_repeat_skip : DEFAULT_KEYPAD_SKIP_REPEAT_COUNT;
#endif
                        }

                        bKeyRepeated = keypadEvent[i].repeat;

                        dfbEvent.flags = ( DIEF_KEYID | DIEF_KEYSYMBOL | DIEF_KEYCODE | DIEF_TIMESTAMP );
                        dfbEvent.type  = DIET_KEYPRESS;

                        if (i < (numEventsRead-1))
                            dfbEvent.flags |= DIEF_FOLLOW;

                        if ( bKeyRepeated )
                            dfbEvent.flags |= DIEF_REPEAT;

                        BDFB_lookup_kpd_keycodes( eventCode, &dfbEvent );

                        /*
                        ** DirectFB wants to know about both the key press and the release.
                        ** We'll stretch the truth here and generate both events even
                        ** though it might only be a key press.
                        **
                        ** We will also skip a number of initial repeats to help prevent applications
                        ** having to have the same logic of discarding the first N initial key repeats.
                        */
                        if (!bKeyRepeated || (bKeyRepeated && (skipRepeatCnt == 0)))
                        {
                            dfb_input_dispatch( pDfbDevice, &dfbEvent );

                            if (!timeout) {
                                dfbEvent.type = DIET_KEYRELEASE;
                                dfb_input_dispatch( pDfbDevice, &dfbEvent );
                            }
                        }

                        if (skipRepeatCnt)
                            skipRepeatCnt--;
                    }
                }
            }
        }
    }
    return NULL;
}       /* end of BDFB_KeypadThread() */

#if defined(HUMAX_PLATFORM_BASE)
#include <sys/ipc.h> 
#include <sys/msg.h>

struct dfbmsgbuf 
{ 
    long msgtype;
	union
    {
        struct
        {
		    unsigned int device;
			unsigned int code;
			unsigned int type;
			char mtext[255-24];
        }directfb;

		struct
		{
			unsigned int device;
			unsigned int code;
			unsigned int type;
			int xdisplacement;
			int ydisplacement;
			int zdisplacement;
			char mtext[255-48];
		}directfbrf4ce;

		char mtext[255];
	}u;
};

static struct dfbmsgbuf rcvbuf;

static void *BDFB_QueueKEYThread(DirectThread *thread, void *arg)
{
     DirectResult                res;
	NEXUS_Error					rc;
	NEXUS_KeypadEvent 			keypadEvent[NUM_EVENTS];
	size_t						numEventsRead;
	bool 						overflow;
    uint32_t                    eventCode, lastEventCode = 0;
    int                         skipRepeatCnt = 0;
    bool                        bKeyRepeated = false;
    DFBInputEvent               dfbEvent;
    CoreInputDevice *           pDfbDevice;
    size_t                      i;
#if defined(HUMAX_PLATFORM_BASE)
    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;
    unsigned int                timeout = brcm_config->bcmnexus_key_timeout;
#else
    unsigned int                    timeout = dfb_config->bcmnexus_key_timeout;
#endif

    BSTD_UNUSED(thread);

	BDFB_kpd_info *pInputInfo = (BDFB_kpd_info *)arg;

	key_t key_id; 
	int msgtype = 1;  /* 1 : front key, 2 : RCU, 3 : RF4CE */
	
	key_id = msgget((key_t)24680, IPC_CREAT|0666); 
	if (key_id == -1) 
	{ 
		perror("msgget error"); 
		return; 
	}

	memset( &dfbEvent, 0, sizeof( dfbEvent ) );

    while ( pInputInfo && pInputInfo->cancel_thread == false)
    {
    	if ( pInputInfo->cancel_thread == false )
        {
        	pDfbDevice = (CoreInputDevice *)( pInputInfo->pDfbDevice );

            if ( pDfbDevice )
            {
	        	D_DEBUG_AT(bcmnexusKeyInput, "%s %d message wait\n", __FUNCTION__, __LINE__);

				memset((void *)&rcvbuf, 0x0, sizeof(struct dfbmsgbuf));
				if (msgrcv( key_id, (void *)&rcvbuf, sizeof(struct dfbmsgbuf) - sizeof(long), msgtype, 0) == -1) 
				{ 
					perror("msgrcv error"); 
					continue;	 
				}				
				D_DEBUG_AT(bcmnexusKeyInput, "%s rcvbuf = [%x][%x][%x]\n", __FUNCTION__, rcvbuf.u.directfb.device, rcvbuf.u.directfb.code, rcvbuf.u.directfb.type);
				D_ERROR("%s rcvbuf = [%x][%x][%x]\n", __FUNCTION__, rcvbuf.u.directfb.device, rcvbuf.u.directfb.code, rcvbuf.u.directfb.type);

				eventCode = rcvbuf.u.directfb.code;
				memset( &dfbEvent, 0, sizeof( dfbEvent ) );
				lastEventCode = eventCode;

                /* Setup the timestamp */
                gettimeofday(&dfbEvent.timestamp, NULL);

				if(rcvbuf.u.directfb.type == /*DI_KEY_REPEATED*/1)
				{
					bKeyRepeated = true;
				}
                        	dfbEvent.flags = ( DIEF_KEYID | DIEF_KEYSYMBOL | DIEF_KEYCODE | DIEF_TIMESTAMP | DIEF_BUTTONS );
				if(rcvbuf.u.directfb.type == /*DI_KEY_PRESSED*/0 || rcvbuf.u.directfb.type == /*DI_KEY_REPEATED*/1)
				{
					dfbEvent.type  = DIET_KEYPRESS;
				}
				else
				{
					dfbEvent.type  = DIET_KEYRELEASE;
				}
				if ( bKeyRepeated )
					dfbEvent.flags |= DIEF_REPEAT;

				BDFB_lookup_kpd_keycodes( eventCode, &dfbEvent );

				dfb_input_dispatch( pDfbDevice, &dfbEvent );
	        }
    	}
    }
	
    return NULL;
}       /* end of BDFB_IrfraThread() */
#endif



/*
** DirectFB "standard' input driver interface routines.
*/

static int driver_get_available( void )
{
    /*
    ** Are there any registers or services we should check?
    ** Or should we simply assume that since the driver has been built, we are good to go?
    */
     return 1;
}

static void driver_get_info( InputDriverInfo *pDeviceInfo )
{
    /*
    ** These values are simply a guess.
    */

    snprintf( pDeviceInfo->name,
               DFB_INPUT_DRIVER_INFO_NAME_LENGTH,
               "BRCM keypad driver" );

    snprintf( pDeviceInfo->vendor,
               DFB_INPUT_DRIVER_INFO_VENDOR_LENGTH,
               "Broadcom" );

    pDeviceInfo->version.major = 3;
    pDeviceInfo->version.minor = 1;
}       /* end of driver_get_info() */

static DFBResult driver_open_device(
    CoreInputDevice *   pDfbDevice,
    unsigned int        uiDeviceId,
    InputDeviceInfo *   pDfbDeviceInfo,
    void **             pDriverData
    )
{
    DFB_PlatformInputDeviceSettings   keypadSettings;
    BDFB_kpd_info         *pInputKeypad;

    BSTD_UNUSED(uiDeviceId);

    /*
    ** Allocate and initialize the device and "global" data.
    */
    g_iKpdNumBasicCodes = ( sizeof( KpdBasicCodeMap ) / sizeof ( KeyMapElement ) );
    g_iKpdNumAdvanCodes = ( sizeof( KpdAdvanCodeMap ) / sizeof ( KeyMapElement ) );

    pInputKeypad = D_CALLOC( 1, sizeof(BDFB_kpd_info) );

    if (pInputKeypad == NULL)
    {
        D_ERROR("bcmNexus/KeyInput: %s failed to initialize pInputKeypad\n", __FUNCTION__);
        return DFB_INIT;
    }

    pInputKeypad->cancel_thread = false;

    /*
    ** We'll use " pDfbDevice " to queue the events within callbacks
    */
    pInputKeypad->pDfbDevice = pDfbDevice;

#if !defined(HUMAX_PLATFORM_BASE)
    DFB_PlatformInputDevice_GetDefaultSettings(DFB_PlatformInputDevice_eKeypad,&keypadSettings);
    keypadSettings.dataReady.callback = BDFB_input_dataReady;
    keypadSettings.dataReady.context = &pInputKeypad->lock;
    keypadSettings.repeatFilterTime = dfb_config->bcmnexus_key_repeat_time ? dfb_config->bcmnexus_key_repeat_time : DEFAULT_REPEAT_MILLI_SECS;
    keypadSettings.device.keypad.debounce = dfb_config->bcmnexus_key_debounce ? dfb_config->bcmnexus_key_debounce : DEFAULT_DEBOUNCE_COUNT;
    keypadSettings.eventQueueSize = (keypadSettings.eventQueueSize < NUM_EVENTS) ? NUM_EVENTS : keypadSettings.eventQueueSize;
    pInputKeypad->keypad = DFB_PlatformInputDevice_Open(DFB_PlatformInputDevice_eKeypad,0, &keypadSettings);

    if (pInputKeypad->keypad == NULL)
    {
        D_ERROR("bcmNexus/KeyInput: %s failed to initialize hardware driver\n", __FUNCTION__);
        D_FREE(pInputKeypad);
        return DFB_INIT;
    }
#endif
    /* Create the locks/semaphores */
    fusion_skirmish_init( &pInputKeypad->lock, "Broadcom BDFB_Keypad Semaphore", dfb_core_world(NULL) );

    /*
    ** Fill in the driver info with the supported features.
    */
    pDfbDeviceInfo->prefered_id = DIDID_REMOTE+1;
    pDfbDeviceInfo->desc.type   = DIDTF_KEYBOARD;
    pDfbDeviceInfo->desc.caps   = DICAPS_KEYS;

    *pDriverData = pInputKeypad;

    /*
    ** Create threads to handle the input IR and key events.
    */
#if !defined(HUMAX_PLATFORM_BASE)
        pInputKeypad->thread = direct_thread_create(DTT_DEFAULT, BDFB_KeypadThread, pInputKeypad, "BDFB_Keypad");
#else
	pInputKeypad->thread = direct_thread_create(DTT_DEFAULT, BDFB_QueueKEYThread, pInputKeypad, "BDFB_QueueKEY");
#endif

    return DFB_OK;
}       /* end of driver_open_device() */

static DFBResult driver_get_keymap_entry(
    CoreInputDevice *           pDfbDevice,
    void *                                              pDriverData,
    DFBInputDeviceKeymapEntry * pKeymapEntry
    )
{
        BSTD_UNUSED(pDfbDevice);
        BSTD_UNUSED(pDriverData);
        BSTD_UNUSED(pKeymapEntry);
        return DFB_UNSUPPORTED;
}       /* end of driver_get_keymap_entry() */

static void driver_close_device( void *pDriverData )
{
        BDFB_kpd_info *pInputKeypad = (BDFB_kpd_info *)pDriverData;

    if ( pInputKeypad )
    {
        /* Cancel Keypad Input thread... */
        pInputKeypad->cancel_thread = true;
        fusion_skirmish_prevail( &pInputKeypad->lock );
        fusion_skirmish_notify( &pInputKeypad->lock );
        fusion_skirmish_dismiss( &pInputKeypad->lock );
        direct_thread_join(pInputKeypad->thread);
        direct_thread_destroy(pInputKeypad->thread);

        /* Close the keypad interface */
        if ( pInputKeypad->keypad ) DFB_PlatformInputDevice_Close(DFB_PlatformInputDevice_eKeypad, pInputKeypad->keypad );

        /* Destroy the locks */
        fusion_skirmish_destroy(&pInputKeypad->lock);

        /* Free data structure */
        D_FREE( pInputKeypad );
    }

    return;
}       /* end of driver_close_device() */

