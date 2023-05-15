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
 * $brcm_Workfile: bcmnexus_ir.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/5 $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/inputdrivers/bcmnexus/core/bcmnexus_ir.c $
 * 
 * DirectFB_1_4_15_Port/5   3/7/12 5:25p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/4   2/7/12 5:19p christ
 * SW7425-2341: DirectFB: BRCM input drivers fail to check for valid
 *  hardware handles.
 *
 * DirectFB_1_4_15_Port/3   11/2/11 2:31p christ
 * SW7420-2121: DirectFB: Refactor input drivers (ir + keypad) to move
 *  Nexus usage into platform layer
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
 * DirectFB_1_4_Port/10   3/17/11 1:41p christ
 * SW7420-1684: DirectFB: Cannot use "Broadcom" (silver) remote.
 *
 * DirectFB_1_4_Port/9   3/16/11 6:45p christ
 * SW7420-1678: DirectFB: make limited doesn't create a working build.
 *
 * DirectFB_1_4_Port/8   3/14/11 4:41p robertwm
 * SW3556-1224: DirectFB: Cannot build IR driver for 3556 platform.
 * SW3556-1225: DirectFB: DirectFB-1.4.1 Phase 3.0 PreRel fails to build.
 * SW35125-128: DirectFB: IR remote protocol default is wrong and causes
 *  compilation error.
 * SW7420-1654: DirectFB: Add run-time IR protocol and keymap selection.
 *
 * DirectFB_1_4_Port/7   3/2/11 12:24p christ
 * SW7420-1558: DirectFB: Support runtime switching of IR codes for using
 *  different remotes.
 *
 * DirectFB_1_4_Port/6   8/19/10 4:28p robertwm
 * SW35230-1060: DirectFB: Add support for building DFB with different IR
 *  keycode tables.
 *
 * DirectFB_1_4_Port/5   8/11/10 4:59p robertwm
 * SW35230-967: DirectFB: Cannot build IR module for 35230.
 *
 * DirectFB_1_4_Port/4   7/26/10 5:31p jackf
 * SW35230-771: Added include for shim_nexus.h.
 *
 * DirectFB_1_4_Port/3   7/7/10 2:13p robertwm
 * SW7405-4604: DirectFB: IR and keypad drivers need to generate single
 *  KEYPRESS and KEYRELEASE events.
 *
 * DirectFB_1_4_Port/2   7/6/10 2:15p robertwm
 * SW7405-4604: DirectFB: IR and keypad drivers need to generate single
 *  KEYPRESS and KEYRELEASE events.
 *
 * DirectFB_1_4_Port/1   2/24/10 10:27a robertwm
 * SW7468-125: DirectFB: Provide ability to separately build IR and Keypad
 *  input drivers.
 *
 * This code implements a DirectFB input driver for the IR receiver.
 * It borrows heavily from "bsettop_user_io.c" and "bsettop_baser_userio.c".
 * It was shaken out using the "One For All" remote; it will need additional work
 * to support other devices.  It will also need additional work to support
 * simultaneous IR devices.
 ***************************************************************************/

#include <config.h>

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
#include <core/system.h>

#include <misc/conf.h>

/*
** Broadcom specific includes.
*/
#include "platform_init.h"

#include "bcmnexus_ir.h"

#include "bcmnexus.h"

#if defined(HUMAX_PLATFORM_BASE)
#include "nexus_types.h"
#include "nexus_ir_input.h"
#include "bstd.h"
#include "bkni.h"
#endif

D_DEBUG_DOMAIN( bcmnexusIrInput, "bcmNexus/IrInput", "Broadcom NEXUS IR Input Event driver" );

DEFINE_MODULE_DIRECTORY( dfb_bcmnexus_ir_keycodes, "inputdrivers/bcmnexus", DFB_BCMNEXUS_IR_KEYCODES_ABI_VERSION );

DFB_INPUT_DRIVER( bcmNexusIrInputDevice )

/*
** type definitions.
*/

/*
** A container for the IR receiver variables.
*/
typedef struct {

    DFB_PlatformNexusHandle     irInput;
    struct timeval              timePrevEvent;  /* for hardware that doesn't implement "repeat" detection */
    CoreInputDevice            *pDfbDevice;     /* to access the DFB event queue */
    DirectThread               *thread;         /* Input IR event handling thread */
    FusionSkirmish              lock;           /* Lock */
    bool                        cancel_thread;  /* set to true to cancel thread */

} BDFB_ir_info;


/*
** Constant definitions.
*/

/*
** If hw events are arriving faster than every "DEFAULT_REPEAT_MILLI_SECS",
** then we'll make the gross assumption that the key is being held down.
** This is used in the event handlers to debounce the keys.
*/

#define DEFAULT_REPEAT_MILLI_SECS   150

/*
** Define maximum number of events that can be consumed in a single callback.
*/
#define NUM_EVENTS 10

/*
** Define the number of initial repeats to skip/discard before issuing
** an IR key event.
*/
#define DEFAULT_IR_SKIP_REPEAT_COUNT 2

/*
** Global data.
*/

static int g_iIrNumBasicCodes;          /* these variables store the size of the IR keycode arrays */
static int g_iIrNumAdvanCodes;

bcmnexus_IR_keycodes irKeycodes;
KeyMapElement *g_pIrBasicCodeMap;
KeyMapElement *g_pIrAdvanCodeMap;

/*
** Broadcom specific routines.
*/

DFB_PlatformIrInputMode BDFB_lookup_ir_protocol(
    const char *name )
{
    DFB_PlatformIrInputMode inputMode;

    D_DEBUG_AT(bcmnexusIrInput, "%s: looking for IR protocol \"%s\"\n", __FUNCTION__, name);

    inputMode = DFB_PlatformLookupIrInputmode(name);

    if (inputMode >= DFB_PlatformIrInputMode_eMax )
    {
        /* should stop recursive lookup if things are bad */
        if ( strcasecmp(name, DIRECTFB_IR_PROTOCOL ) )
        {
            D_WARN("bcmNexus/IrInput: Cannot find IR protocol \"%s\" - defaulting to \"%s\"!\n", name, DIRECTFB_IR_PROTOCOL);
            inputMode = BDFB_lookup_ir_protocol(DIRECTFB_IR_PROTOCOL);
        }
        else
            D_ERROR("bcmNexus/IrInput: Cannot find IR protocol\n");
    }
    else
        D_DEBUG_AT(bcmnexusIrInput, "%s: found IR protocol \"%s\" [%d]\n", __FUNCTION__, name, inputMode);

    return inputMode;
}

static bool BDFB_load_ir_keycodes (
    const char *name,
    bcmnexus_IR_keycodes *pIRKeys)
{
    DirectLink *link;

    D_DEBUG_AT(bcmnexusIrInput, "%s: Looking for IR keycodes module: '%s'\n", __FUNCTION__,name);

    direct_modules_explore_directory( &dfb_bcmnexus_ir_keycodes );

    direct_list_foreach( link, dfb_bcmnexus_ir_keycodes.entries )
    {
        DirectModuleEntry *module = (DirectModuleEntry*)link;
        IrKeycodesMapFuncs *funcs = NULL;

        funcs = (void*)direct_module_ref( module );
        if (!funcs)
            continue;

        if (!name || !strcasecmp( name, module->name ))
        {
            if (pIRKeys->module)
                direct_module_unref( pIRKeys->module );

            pIRKeys->module = module;
            pIRKeys->funcs  = funcs;
        }
        else
            direct_module_unref( module );
    }

    if (pIRKeys->module && pIRKeys->funcs)
    {
        D_DEBUG_AT(bcmnexusIrInput, "%s: Loaded IR keycodes module: '%s'\n", __FUNCTION__,name);
        return true;
    }

    /* Try default value - if we're not already trying the default. */
    if (strcasecmp(DIRECTFB_IR_KEYCODES,name))
        return BDFB_load_ir_keycodes(DIRECTFB_IR_KEYCODES,pIRKeys);

    D_ERROR( "bcmNexus/IrInput: IR keycodes module '%s' not found!\n", name ? name : "" );

    return false;
}


/*
** Routine to map the IR hw codes to DirectFB keycodes.
*/
static void BDFB_lookup_ir_keycodes(
    uint32_t                uiEventCode,
    DFBInputEvent *     pDfbEvent
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

    pDfbEvent->key_id = DIKI_UNKNOWN;

    for( i=0; i < g_iIrNumBasicCodes; i++ )
    {
        if ( uiEventCode == g_pIrBasicCodeMap[ i ].hwEventCode )
        {
            pDfbEvent->key_id = g_pIrBasicCodeMap[ i ].dfbKeySymbol;
            break;
        }
    }

    /*
    ** Advanced mapping, unicode compatible, modified dependent.
    ** If we don't get a "hit" on the hardware key code, we'll return "DIKS_NULL".
    ** Is this the right solution for the real code?
    */

    pDfbEvent->key_symbol = DIKS_NULL;


    for( i=0; i < g_iIrNumAdvanCodes; i++ )
    {
        if ( uiEventCode == g_pIrAdvanCodeMap[ i ].hwEventCode )
        {
            pDfbEvent->key_symbol = g_pIrAdvanCodeMap[ i ].dfbKeySymbol;
            break;
        }
    }

    return;

}       /* end of BDFB_lookup_ir_keycodes */


/*
**  Data ready callback for the IR receiver.
*/
static void BDFB_input_dataReady(void *context, int param)
{
    FusionSkirmish *lock = (FusionSkirmish *)context;

    BSTD_UNUSED(param);

    fusion_skirmish_prevail( lock );
    fusion_skirmish_notify(  lock );
    fusion_skirmish_dismiss( lock );
}       /* end of BDFB_input_dataReady() */

static void *BDFB_IrfraThread(DirectThread *thread, void *arg)
{
    DirectResult                res;
    DFB_PlatformResult          rc;
    DFB_PlatformInputDeviceEvent irEvent[NUM_EVENTS];
    size_t                      numEventsRead;
    bool                        overflow;
    uint32_t                    eventCode, lastEventCode = 0;
    int                         skipRepeatCnt = 0;
    bool                        bKeyRepeated = false;
    DFBInputEvent               dfbEvent;
    CoreInputDevice *           pDfbDevice;
    size_t                      i;
    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;
    unsigned int                timeout = brcm_config->bcmnexus_ir_timeout;

    BSTD_UNUSED(thread);

    BDFB_ir_info  *pInputInfo = (BDFB_ir_info *)arg;

    memset( &dfbEvent, 0, sizeof( dfbEvent ) );

    while ( pInputInfo && pInputInfo->cancel_thread == false)
    {
        /* Wait for Nexus IR Callback notification */
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
                    rc = DFB_PlatformInputDevice_GetEvents(DFB_PlatformInputDevice_eInfraRed, pInputInfo->irInput, irEvent, NUM_EVENTS, &numEventsRead, &overflow);
                    if ( rc != NEXUS_SUCCESS)
                    {
                        D_ERROR("%s: NEXUS_IrInput_GetEvents failed %d\n", __FUNCTION__, rc);
                        continue;
                    }
                    if ( numEventsRead<1)
                    {
                        continue;
                    }
                    else if (overflow || numEventsRead > NUM_EVENTS)
                    {
                        D_ERROR("%s: NEXUS_IrInput_GetEvents under/overflowed events read=%d\n",__FUNCTION__, numEventsRead);
                    }

                    for (i=0; i<numEventsRead; i++)
                    {
                        D_DEBUG_AT(bcmnexusIrInput, "%s: IR event %d (%d,%d)\n", __FUNCTION__, i, irEvent[i].code, irEvent[i].repeat);

                        eventCode = irEvent[i].code;

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
                        if ((bKeyRepeated == false) && (irEvent[i].repeat == true)) {
                            skipRepeatCnt = brcm_config->bcmnexus_ir_repeat_skip ? brcm_config->bcmnexus_ir_repeat_skip : DEFAULT_IR_SKIP_REPEAT_COUNT;
                        }

                        bKeyRepeated = irEvent[i].repeat;

                        dfbEvent.flags = ( DIEF_KEYID | DIEF_KEYSYMBOL | DIEF_KEYCODE | DIEF_TIMESTAMP );
                        dfbEvent.type  = DIET_KEYPRESS;

                        if (i < (numEventsRead-1))
                            dfbEvent.flags |= DIEF_FOLLOW;

                        if ( bKeyRepeated )
                            dfbEvent.flags |= DIEF_REPEAT;

                        BDFB_lookup_ir_keycodes( eventCode, &dfbEvent );

                        /*
                        ** DirectFB wants to know about both the key press and the release.
                        ** We'll stretch the truth here and generate both events even
                        ** though it might only be a key press.
                        **
                        ** There appears to be some support in the hardware to detect the
                        ** key release, i.e. the "lflag" in the "KBD1_STATUS" register.
                        ** This logic is NOT currently wired up in the software, we'll stick with
                        ** our approximation of always sending a key release event.
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
}       /* end of BDFB_IrfraThread() */

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

static void *BDFB_QueueRCUThread(DirectThread *thread, void *arg)
{
    DirectResult                res;
    NEXUS_Error                 rc;
    NEXUS_IrInputEvent          irEvent[NUM_EVENTS];
    size_t                      numEventsRead;
    bool                        overflow;
    uint32_t                    eventCode, lastEventCode = 0;
    int                         skipRepeatCnt = 0;
    bool                        bKeyRepeated = false;
    DFBInputEvent               dfbEvent;
    CoreInputDevice *           pDfbDevice;
    size_t                      i;
    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;
    unsigned int                timeout = brcm_config->bcmnexus_ir_timeout;

    BSTD_UNUSED(thread);

    BDFB_ir_info  *pInputInfo = (BDFB_ir_info *)arg;

	key_t key_id; 
	int msgtype = 2;  /* 1 : front key, 2 : RCU, 3 : RF4CE */
	
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
	        	D_DEBUG_AT(bcmnexusIrInput, "%s %d message wait\n", __FUNCTION__, __LINE__);

				memset((void *)&rcvbuf, 0x0, sizeof(struct dfbmsgbuf));
				if (msgrcv( key_id, (void *)&rcvbuf, sizeof(struct dfbmsgbuf) - sizeof(long), msgtype, 0) == -1) 
				{ 
					perror("msgrcv error"); 
					continue;	 
				}				
				D_DEBUG_AT(bcmnexusIrInput, "%s rcvbuf = [%x][%x][%x]\n", __FUNCTION__, rcvbuf.u.directfb.device, rcvbuf.u.directfb.code, rcvbuf.u.directfb.type);

				eventCode = rcvbuf.u.directfb.code;
				memset( &dfbEvent, 0, sizeof( dfbEvent ) );
				lastEventCode = eventCode;

                /* Setup the timestamp */
                gettimeofday(&dfbEvent.timestamp, NULL);

				if(rcvbuf.u.directfb.type == /*DI_KEY_REPEATED*/1)
				{
					bKeyRepeated = true;
				}
				else
				{
					bKeyRepeated = false;
				}

				dfbEvent.flags = ( DIEF_KEYID | DIEF_KEYSYMBOL | DIEF_KEYCODE | DIEF_TIMESTAMP );
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

				BDFB_lookup_ir_keycodes( eventCode, &dfbEvent );

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
               "BRCM IR driver" );

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
    DFB_PlatformInputDeviceSettings  irInputSettings;
    BDFB_ir_info                    *pInputInfra;

    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;

    BSTD_UNUSED(uiDeviceId);

    /*
    ** Allocate and initialize the device and "global" data.
    */

    if (!BDFB_load_ir_keycodes(brcm_config->bcmnexus_ir_keycodes, &irKeycodes))
    {
        D_ERROR("bcmNexus/IrInput %s failed to load IR keycodes module: %s\n", __FUNCTION__, brcm_config->bcmnexus_ir_keycodes);
        return DFB_INIT;
    }

    g_iIrNumBasicCodes = irKeycodes.funcs->GetBasicKeycodesSize();
    g_iIrNumAdvanCodes = irKeycodes.funcs->GetAdvanKeycodesSize();

    g_pIrBasicCodeMap = D_CALLOC(g_iIrNumBasicCodes, sizeof(KeyMapElement));

    if (g_pIrBasicCodeMap == NULL)
    {
        D_ERROR("bcmNexus/IrInput: %s failed to allocate basic code map memory\n", __FUNCTION__);
        return DFB_INIT;
    }

    g_pIrAdvanCodeMap = D_CALLOC(g_iIrNumAdvanCodes, sizeof(KeyMapElement));

    if (g_pIrAdvanCodeMap == NULL)
    {
        D_ERROR("bcmNexus/IrInput: %s failed to allocate advanced code map memory\n", __FUNCTION__);
        D_FREE(g_pIrBasicCodeMap);
        return DFB_INIT;
    }

    irKeycodes.funcs->GetBasicKeycodes(g_pIrBasicCodeMap);
    irKeycodes.funcs->GetAdvanKeycodes(g_pIrAdvanCodeMap);

    pInputInfra = D_CALLOC( 1, sizeof(BDFB_ir_info) );

    if (pInputInfra == NULL)
    {
        D_ERROR("bcmNexus/IrInput: %s failed to allocate internal structure pInputInfra\n", __FUNCTION__);
        D_FREE(g_pIrBasicCodeMap);
        D_FREE(g_pIrAdvanCodeMap);
        return DFB_INIT;
    }

    pInputInfra->cancel_thread = false;

    /*
    ** We'll use " pDfbDevice " to queue the events within callbacks
    */
    pInputInfra->pDfbDevice  = pDfbDevice;

#if !defined(HUMAX_PLATFORM_BASE)
    DFB_PlatformInputDevice_GetDefaultSettings(DFB_PlatformInputDevice_eInfraRed,&irInputSettings);
    irInputSettings.dataReady.callback = BDFB_input_dataReady;
    irInputSettings.dataReady.context = &pInputInfra->lock;
    irInputSettings.repeatFilterTime = brcm_config->bcmnexus_ir_repeat_time ? brcm_config->bcmnexus_ir_repeat_time : DEFAULT_REPEAT_MILLI_SECS;
    irInputSettings.eventQueueSize = (irInputSettings.eventQueueSize < NUM_EVENTS) ? NUM_EVENTS : irInputSettings.eventQueueSize;
    irInputSettings.device.ir.mode = BDFB_lookup_ir_protocol(brcm_config->bcmnexus_ir_protocol);
    pInputInfra->irInput = DFB_PlatformInputDevice_Open(DFB_PlatformInputDevice_eInfraRed,0, &irInputSettings);

    if (pInputInfra->irInput == NULL)
    {
        D_ERROR("bcmNexus/IrInput: %s failed to initialize hardware driver\n", __FUNCTION__);

        D_FREE(pInputInfra);
        D_FREE(g_pIrBasicCodeMap);
        D_FREE(g_pIrAdvanCodeMap);

        return DFB_INIT;
    }
#endif

    /* Create the locks/semaphores */
    fusion_skirmish_init( &pInputInfra->lock,  "Broadcom BDFB_Infra Semaphore",  dfb_core_world(NULL) );

    /*
    ** Fill in the driver info with the supported features.
    */
    pDfbDeviceInfo->prefered_id = DIDID_REMOTE;
    pDfbDeviceInfo->desc.type   = DIDTF_REMOTE;
    pDfbDeviceInfo->desc.caps   = DICAPS_KEYS;

    *pDriverData = pInputInfra;

    /*
    ** Create threads to handle the input IR and key events.
    */
#if !defined(HUMAX_PLATFORM_BASE)
    pInputInfra->thread = direct_thread_create(DTT_DEFAULT, BDFB_IrfraThread,  pInputInfra, "BDFB_Irfra");
#else
	pInputInfra->thread = direct_thread_create(DTT_DEFAULT, BDFB_QueueRCUThread,  pInputInfra, "BDFB_QueueRCU");
#endif

    return DFB_OK;
}       /* end of driver_open_device() */

static DFBResult driver_get_keymap_entry(
    CoreInputDevice *           pDfbDevice,
    void *                      pDriverData,
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
    BDFB_ir_info *pInputInfra = (BDFB_ir_info *) pDriverData;

    if (g_pIrBasicCodeMap)
        D_FREE(g_pIrBasicCodeMap);

    if (g_pIrAdvanCodeMap)
        D_FREE(g_pIrAdvanCodeMap);

    if ( pInputInfra )
    {
        /* Cancel IR Input thread... */
        pInputInfra->cancel_thread = true;
        fusion_skirmish_prevail( &pInputInfra->lock );
        fusion_skirmish_notify( &pInputInfra->lock );
        fusion_skirmish_dismiss( &pInputInfra->lock );
        direct_thread_join(pInputInfra->thread);
        direct_thread_destroy(pInputInfra->thread);

#if !defined(HUMAX_PLATFORM_BASE)
        /* Close the ir receiver interface */
        if ( pInputInfra->irInput ) DFB_PlatformInputDevice_Close(DFB_PlatformInputDevice_eInfraRed, pInputInfra->irInput );
#endif

        /* Destroy the locks */
        fusion_skirmish_destroy(&pInputInfra->lock);

        /* Free data structure */
        D_FREE( pInputInfra );
    }
    return;
}       /* end of driver_close_device() */

