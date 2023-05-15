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

#include "bcmnexus_rf4ce.h"

#include "bcmnexus.h"

#include "nexus_types.h"
#include "bstd.h"
#include "bkni.h"


D_DEBUG_DOMAIN( bcmnexusRf4ceInput, "bcmNexus/Rf4ceInput", "Broadcom NEXUS RF4CE Input Event driver" );
DFB_INPUT_DRIVER( bcmNexusRf4ceInputDevice )

/*
** type definitions.
*/

/*
** A container for the RF4CE receiver variables.
*/
typedef struct {

    DFB_PlatformNexusHandle     rf4ceInput;
    struct timeval              timePrevEvent;  /* for hardware that doesn't implement "repeat" detection */
    CoreInputDevice            *pDfbDevice;     /* to access the DFB event queue */
    DirectThread               *thread;         /* Input RF4CE event handling thread */
    FusionSkirmish              lock;           /* Lock */
    bool                        cancel_thread;  /* set to true to cancel thread */

} BDFB_rf4ce_info;


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
** an RF4CE key event.
*/
#define DEFAULT_RF4CE_SKIP_REPEAT_COUNT 2

// from bcmnexus_ir_keycodes_remotea.h
static const KeyMapElement Rf4ceBasicCodeMap [] =
{
    { DIKI_0,       0x0010 },   /* should these be the DIKI_KP_xxx constants? */
    { DIKI_1,       0x0011 },
    { DIKI_2,       0x0012 },
    { DIKI_3,       0x0013 },
    { DIKI_4,       0x0014 },
    { DIKI_5,       0x0015 },
    { DIKI_6,       0x0016 },
    { DIKI_7,       0x0017 },
    { DIKI_8,       0x0018 },
    { DIKI_9,       0x0019 },
    { DIKI_LEFT,    0x0044 },
    { DIKI_RIGHT,   0x0045 },
    { DIKI_DOWN,    0x0043 },
    { DIKI_UP,      0x0042 },
    { DIKI_ENTER,   0x0048 },        /* the select key on the remote */
    { DIKI_ESCAPE,  0x0047 }         /* the exit key on the remote */
};

static const KeyMapElement Rf4ceAdvanCodeMap [] =
{
    { DIKS_0,               0x0010 }, 
    { DIKS_1,               0x0011 }, 
    { DIKS_2,               0x0012 }, 
    { DIKS_3,               0x0013 }, 
    { DIKS_4,               0x0014 }, 
    { DIKS_5,               0x0015 }, 
    { DIKS_6,               0x0016 }, 
    { DIKS_7,               0x0017 }, 
    { DIKS_8,               0x0018 }, 
    { DIKS_9,               0x0019 }, 
    { DIKS_CURSOR_LEFT,     0x0044 }, 
    { DIKS_CURSOR_RIGHT,    0x0045 }, 
    { DIKS_CURSOR_DOWN,     0x0043 }, 
    { DIKS_CURSOR_UP,       0x0042 }, 
    { DIKS_CHANNEL_UP,      0x0050 }, 
    { DIKS_CHANNEL_DOWN,    0x0051 }, 
    { DIKS_EXIT,            0x0047 }, 
    { DIKS_ENTER,           0x0048 }, 
    { DIKS_MENU,            0x0032 }, 
    { DIKS_EPG,             0x0031 }, 
	{ DIKS_POWER,           0x0001 }, 
	{ DIKS_INFO,            0x0030 }, 
	{ DIKS_MUTE,            0x0062 }, 
	{ DIKS_AUDIO,           0x0038 }, 
	{ DIKS_LAST,            0x0052 }, 
	{ DIKS_REWIND,          0x0086 }, 
	{ DIKS_FASTFORWARD,     0x0085 }, 
	{ DIKS_PLAY,            0x0081 }, 
	{ DIKS_RECORD,          0x0084 }, 
	{ DIKS_PAUSE,           0x0082 }, 
	{ DIKS_STOP,            0x0083 }, 
	{ DIKS_PREVIOUS,        0x0087 }, 
	{ DIKS_NEXT,            0x0088 }, 
    { DIKS_FAVORITES,       0x0034 }, /*DI_KEY_REMOCON_FAVORITE  */
    { DIKS_VOLUME_UP,       0x0060 },
	{ DIKS_VOLUME_DOWN,     0x0061 },
	{ DIKS_AUX,             0x006a },
    { DIKS_TV,              0x0053 }, 
    { DIKS_SCREEN,          0x0037 }, 
	{ DIKS_ZOOM,            0x0068 }, 
	{ DIKS_TEXT,            0x0070 }, 
	{ DIKS_INTERNET,        0x00aa },
	{ DIKS_OPTION,          0x003b }, 
    { DIKS_SUBTITLE,        0x0039 },
    { DIKS_RED,             0x0020 },
    { DIKS_GREEN,           0x0021 },
    { DIKS_YELLOW,          0x0022 },
    { DIKS_BLUE,            0x0023 },
    { DIKS_PLAYER,          0x003d },
{ DIKS_SETUP,           0x0036 }, /*DI_KEY_REMOCON_SETUP */
    { DIKS_HELP,            0x003f }, /*DI_KEY_REMOCON_HELP */   
    { DIKS_PAGE_UP,         0x0040 }, /*DI_KEY_REMOCON_PAGEUP */	 
    { DIKS_PAGE_DOWN,       0x0041 }, /*DI_KEY_REMOCON_PAGEDOWN */   		
    { DIKS_BACK,            0x0046 }, /*DI_KEY_REMOCON_BACK */		
    { DIKS_DIGITS,          0x0057 }, /* DI_KEY_REMOCON_CH_EDIT */	  
    { DIKS_FREEZE,          0x0067 }, /*DI_KEY_REMOCON_FREEZE */		
    { DIKS_PLAYPAUSE,       0x0080 }, /*DI_KEY_REMOCON_PLAYPAUSE */			

    { DIKS_HOME,            0x008e }, /*DI_KEY_REMOCON_HOME */ 			
    { DIKS_PIP,             0x00b1 }, /*DI_KEY_REMOCON_PIPDISP */ 			
    { DIKS_MOVE,            0x00b2 }, /*DI_KEY_REMOCON_PIPMOVE */ 			
    { DIKS_SWAP,            0x00b3 }, /*DI_KEY_REMOCON_PIPSWAP */ 			
    { DIKS_MHP,             0x00c1 }, /*DI_KEY_REMOCON_MHP */			
    { DIKS_LANGUAGE,        0x00c2 }, /*DI_KEY_REMOCON_LANGUAGE */				
    { DIKS_SHIFT,           0x00ca }, /*DI_KEY_REMOCON_SHIFT */					
    { DIKS_CLEAR,           0x00cb }, /*DI_KEY_REMOCON_CLEAR */						
    { DIKS_PVR,             0x00d0 }, /*DI_KEY_REMOCON_MODE_PVR */						
    { DIKS_TV2,             0x00d1 }, /*DI_KEY_REMOCON_MODE_TV */						
    { DIKS_DVD,             0x00d2 }, /*DI_KEY_REMOCON_MODE_DVD */						
    { DIKS_PROGRAM,			    0x1006 }, /*DI_KEY_REMOCON_DETAIL_INFO */
    { DIKS_CUSTOM0,         0x008b },
    { DIKS_CUSTOM1,         0x008c },
    { DIKS_CUSTOM2,         0x008d }, 
    { DIKS_CUSTOM3,         0x003a }, 
    { DIKS_CUSTOM4,         0x0035 }, 
    { DIKS_CUSTOM5,         0x00c6 },
    { DIKS_CUSTOM6,         0x00c8 },
    { DIKS_CUSTOM7,         0xCCD7 },	/* Custom key for MSG_MULTICUSTOMCODE_INPUT */
    { DIKS_CUSTOM8,         0xCCD8 },	/* Custom key for MSG_MULTICUSTOMCODE_SUCCESS */
    { DIKS_CUSTOM9,         0xCCD9 },	/* Custom key for MSG_MULTICUSTOMCODE_CANCEL */
    { DIKS_CUSTOM10,        0x001a },	/* DI_KEY_REMOCON_11 */
    { DIKS_CUSTOM11,        0x001b },	/* DI_KEY_REMOCON_12 */    
    { DIKS_CUSTOM12,        0x0033 },	/* DI_KEY_REMOCON_QMENU */    
    { DIKS_CUSTOM13,        0x003c },	/* DI_KEY_REMOCON_SCHEDULE */	  
    { DIKS_CUSTOM14,        0x0054 },	/* DI_KEY_REMOCON_COMBO */	  
    { DIKS_CUSTOM15,        0x0055 },	/* DI_KEY_REMOCON_FREESAT */	  
    { DIKS_CUSTOM16,        0x0056 },	/* DI_KEY_REMOCON_ONE_TOUCH */	  
    { DIKS_CUSTOM18,        0x0063 },	/* DI_KEY_REMOCON_EXT_AV */	  
    { DIKS_CUSTOM19,        0x0064 },	/* DI_KEY_REMOCON_EXT_AV_PLUS */	  
    { DIKS_CUSTOM20,        0x0065 },	/* DI_KEY_REMOCON_EXT_AV_MINUS */	  
    { DIKS_CUSTOM21,        0x0066 },	/* DI_KEY_REMOCON_AVMODE */	
    { DIKS_CUSTOM22,        0x0069 },	/* DI_KEY_REMOCON_3D */ 	
    { DIKS_CUSTOM23,        0x003e },	/* DI_KEY_REMOCON_IP_BROWSER */ 	
    { DIKS_CUSTOM24,        0x0071 },	/* DI_KEY_REMOCON_TELETEXT_OPTION */ 		
    { DIKS_CUSTOM25,        0x0072 },	/* DI_KEY_REMOCON_TTX_HOLD */ 		
    { DIKS_CUSTOM26,        0x0073 },	/* DI_KEY_REMOCON_TTX_REVEAL */ 		
    { DIKS_CUSTOM27,        0x0074 },	/* DI_KEY_REMOCON_TTX_DOUBLE */ 		
    { DIKS_CUSTOM28,        0x0075 },	/* DI_KEY_REMOCON_TTX_CANCEL */ 		
    { DIKS_CUSTOM29,        0x0076 },	/* DI_KEY_REMOCON_TTX_INDEX */ 		
    { DIKS_CUSTOM30,        0x0077 },	/* DI_KEY_REMOCON_TTX_SUBPAGE */ 		
    { DIKS_CUSTOM31,        0x0078 },	/* DI_KEY_REMOCON_TTX_UPDATE */ 		
    { DIKS_CUSTOM32,        0x0079 },	/* DI_KEY_REMOCON_TTX_MIX */ 		
    { DIKS_CUSTOM34,        0x007a },	/* DI_KEY_REMOCON_HIDDEN4MP */ 	

    { DIKS_CUSTOM36,        0x0090 },	/* DI_KEY_REMOCON_QVIEW */		
    { DIKS_CUSTOM37,        0x0091 },	/* DI_KEY_REMOCON_ENERGY_GREEN */		
    { DIKS_CUSTOM38,        0x0092 },	/* DI_KEY_REMOCON_PSM */		
    { DIKS_CUSTOM39,        0x0093 },	/* DI_KEY_REMOCON_SSM */		
    { DIKS_CUSTOM40,        0x0094 },	/* DI_KEY_REMOCON_AD */		
    { DIKS_CUSTOM41,        0x00a0 },	/* DI_KEY_REMOCON_FRONT_LOCK_OFF */		
    { DIKS_CUSTOM42,        0x00a1 },	/* DI_KEY_REMOCON_HOTELMENU */		
    { DIKS_CUSTOM43,        0x00a2 },	/* DI_KEY_REMOCON_SERVICEMODE */		
    { DIKS_CUSTOM44,        0x00a3 },	/* DI_KEY_REMOCON_GAME */		
    { DIKS_CUSTOM45,        0x00a4 },	/* DI_KEY_REMOCON_VOD */		
    { DIKS_CUSTOM46,        0x00a5 },	/* DI_KEY_REMOCON_VODPART_FORMAT */		
    { DIKS_CUSTOM47,        0x00b0 },	/* DI_KEY_REMOCON_PIP_CHLIST */ 	
    { DIKS_CUSTOM48,        0x00c0 },	/* DI_KEY_REMOCON_MYEPG */ 	
    { DIKS_CUSTOM49,        0x00c3 },	/* DI_KEY_REMOCON_ALERT */	
    { DIKS_CUSTOM50,        0x00c4 },	/* DI_KEY_REMOCON_RECALL */	
    { DIKS_CUSTOM51,        0x00c5 },	/* DI_KEY_REMOCON_SYSTEM */ 
    { DIKS_CUSTOM52,        0x00c7 },	/* DI_KEY_REMOCON_TIVUEPG */ 
    { DIKS_CUSTOM53,        0x00c9 },	/* DI_KEY_REMOCON_FUNCTION */ 
    { DIKS_CUSTOM54,        0x00cc },	/* DI_KEY_REMOCON_PROMOTION */	
    { DIKS_CUSTOM55,        0x00d3 },	/* DI_KEY_REMOCON_MODE_AUDIO */	
    { DIKS_CUSTOM56,        0x1000 },	/* DI_KEY_REMOCON_DT */	
    { DIKS_CUSTOM57,        0x1001 },	/* DI_KEY_REMOCON_BS */ 
    { DIKS_CUSTOM58,        0x1002 },	/* DI_KEY_REMOCON_CATV */ 
    { DIKS_CUSTOM59,        0x1003 },	/* DI_KEY_REMOCON_NET */ 
    { DIKS_CUSTOM60,        0x1004 },	/* DI_KEY_REMOCON_VID_AUD */ 
    { DIKS_CUSTOM61,        0x1005 },	/* DI_KEY_REMOCON_DATA */ 
    { DIKS_CUSTOM63,        0x1100 },	/* DI_KEY_REMOCON_DIGITURK_ARCH */ 
    { DIKS_CUSTOM64,        0x1200 },	/* DI_KEY_REMOCON_OTTV_ONDEMAND */ 
    { DIKS_CUSTOM65,        0x0089 }, /*DI_KEY_REMOCON_REPLAY */ 		
    { DIKS_CUSTOM66,        0x008a }	/* DI_KEY_REMOCON_SKIP */		
};

/*
** Global data.
*/

static int g_iRf4ceNumBasicCodes;          /* these variables store the size of the IR keycode arrays */
static int g_iRf4ceNumAdvanCodes;

/*
** Broadcom specific routines.
*/


/*
** Routine to map the RF4CE hw codes to DirectFB keycodes.
*/
static void BDFB_lookup_rf4ce_keycodes(
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

    for( i=0; i < g_iRf4ceNumBasicCodes; i++ )
    {
        if ( uiEventCode == Rf4ceBasicCodeMap[ i ].hwEventCode )
        {
            pDfbEvent->key_id = Rf4ceBasicCodeMap[ i ].dfbKeySymbol;
            break;
        }
    }

    /*
    ** Advanced mapping, unicode compatible, modified dependent.
    ** If we don't get a "hit" on the hardware key code, we'll return "DIKS_NULL".
    ** Is this the right solution for the real code?
    */

    pDfbEvent->key_symbol = DIKS_NULL;


    for( i=0; i < g_iRf4ceNumAdvanCodes; i++ )
    {
        if ( uiEventCode == Rf4ceAdvanCodeMap[ i ].hwEventCode )
        {
            pDfbEvent->key_symbol = Rf4ceAdvanCodeMap[ i ].dfbKeySymbol;
            break;
        }
    }

    return;

}       /* end of BDFB_lookup_rf4ce_keycodes */


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

static void *BDFB_QueueRF4CEThread(DirectThread *thread, void *arg)
{
    uint32_t                    eventCode, lastEventCode = 0;
    bool                        bKeyRepeated = false;
    DFBInputEvent               dfbEvent;
    CoreInputDevice *           pDfbDevice;
    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
	int 						displacement = 0;
	
    BSTD_UNUSED(thread);

    BDFB_rf4ce_info  *pInputInfo = (BDFB_rf4ce_info *)arg;

	key_t key_id; 
	int msgtype = 3;  /* 1 : front key, 2 : RCU, 3 : RF4CE */
	
	key_id = msgget((key_t)24680, IPC_CREAT|0666); 
	if (key_id == -1) 
	{ 
		perror("msgget error"); 
		return NULL; 
	}

	memset( &dfbEvent, 0, sizeof( dfbEvent ) );

    while (pInputInfo && pInputInfo->cancel_thread == false)
    {
    	if (pInputInfo->cancel_thread == false )
        {
        	pDfbDevice = (CoreInputDevice *)( pInputInfo->pDfbDevice );

            if ( pDfbDevice )
            {
	        	D_DEBUG_AT(bcmnexusRf4ceInput, "%s %d message wait\n", __FUNCTION__, __LINE__);
				memset((void *)&rcvbuf, 0x0, sizeof(struct dfbmsgbuf));
				if (msgrcv( key_id, (void *)&rcvbuf, sizeof(struct dfbmsgbuf) - sizeof(long), msgtype, 0) == -1) 
				{ 
					perror("msgrcv error"); 
					continue;	 
				}				

				D_DEBUG_AT(bcmnexusRf4ceInput, "%s rcvbuf = [%x][%x][%x]\n", __FUNCTION__, rcvbuf.u.directfbrf4ce.device, rcvbuf.u.directfbrf4ce.code, rcvbuf.u.directfbrf4ce.type);

				eventCode = rcvbuf.u.directfbrf4ce.code;
				memset( &dfbEvent, 0, sizeof( dfbEvent ) );
				lastEventCode = eventCode;

                /* Setup the timestamp */
                gettimeofday(&dfbEvent.timestamp, NULL);
				if(rcvbuf.u.directfbrf4ce.type == 3 /*DI_RF4CE_REL_MOTION*/ || rcvbuf.u.directfbrf4ce.type == 4 /*DI_RF4CE_ABS_MOTION*/)
				{
					displacement = 0;
					dfbEvent.type  = DIET_AXISMOTION;
					if(rcvbuf.u.directfbrf4ce.xdisplacement != 0)
					{
						dfbEvent.axis = DIAI_X;
						displacement = rcvbuf.u.directfbrf4ce.xdisplacement;
					}
					else if (rcvbuf.u.directfbrf4ce.ydisplacement != 0)
					{
						dfbEvent.axis = DIAI_Y;
						displacement = rcvbuf.u.directfbrf4ce.ydisplacement;
					}
					else if(rcvbuf.u.directfbrf4ce.zdisplacement != 0)
					{
						dfbEvent.axis = DIAI_Z;
						displacement = rcvbuf.u.directfbrf4ce.zdisplacement;
					}

					if(rcvbuf.u.directfbrf4ce.type == 3 /*DI_RF4CE_REL_MOTION*/)
					{
						dfbEvent.flags = (DIEF_AXISREL | DIEF_TIMESTAMP);
						dfbEvent.axisrel = displacement;
					}
					else
					{
						dfbEvent.flags = (DIEF_AXISABS | DIEF_TIMESTAMP);
						dfbEvent.axisabs = displacement;
					}
				}
				else
				{
					if(rcvbuf.u.directfbrf4ce.type == /*DI_RF4CE_REPEATED*/1)
					{
						bKeyRepeated = true;
					}
					dfbEvent.flags = ( DIEF_KEYID | DIEF_KEYSYMBOL | DIEF_KEYCODE | DIEF_TIMESTAMP );
					if(rcvbuf.u.directfbrf4ce.type == /*DI_RF4CE_PRESSED*/0 || rcvbuf.u.directfbrf4ce.type == /*DI_RF4CE_REPEATED*/1)
					{
						dfbEvent.type  = DIET_KEYPRESS;
					}
					else
					{
						dfbEvent.type  = DIET_KEYRELEASE;
					}
					if ( bKeyRepeated )
						dfbEvent.flags |= DIEF_REPEAT;

					BDFB_lookup_rf4ce_keycodes( eventCode, &dfbEvent );
				}
				dfb_input_dispatch( pDfbDevice, &dfbEvent );
	        }
    	}
    }
	
    return NULL;
}       /* end of BDFB_QueueRF4CEThread() */

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
               "BRCM RF4CE driver" );

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
    BDFB_rf4ce_info                    *pInputRf4ce;
    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;

    BSTD_UNUSED(uiDeviceId);

    /*
    ** Allocate and initialize the device and "global" data.
    */

    g_iRf4ceNumBasicCodes = ( sizeof( Rf4ceBasicCodeMap ) / sizeof ( KeyMapElement ) );
    g_iRf4ceNumAdvanCodes = ( sizeof( Rf4ceAdvanCodeMap ) / sizeof ( KeyMapElement ) );

    pInputRf4ce = D_CALLOC( 1, sizeof(BDFB_rf4ce_info) );

    if (pInputRf4ce == NULL)
    {
        D_ERROR("bcmNexus/Rf4ceInput: %s failed to allocate internal structure pInputRf4ce\n", __FUNCTION__);
        return DFB_INIT;
    }

    pInputRf4ce->cancel_thread = false;

    /*
    ** We'll use " pDfbDevice " to queue the events within callbacks
    */
    pInputRf4ce->pDfbDevice  = pDfbDevice;

    /* Create the locks/semaphores */
    fusion_skirmish_init( &pInputRf4ce->lock,  "Broadcom BDFB_Infra Semaphore",  dfb_core_world(NULL) );

    /*
    ** Fill in the driver info with the supported features.
    */
    pDfbDeviceInfo->prefered_id = DIDID_REMOTE+2;
    pDfbDeviceInfo->desc.type   = DIDTF_REMOTE;
    pDfbDeviceInfo->desc.caps   = DICAPS_KEYS | DICAPS_AXES;

    *pDriverData = pInputRf4ce;

    /*
    ** Create threads to handle the input RF4CE and key events.
    */
	pInputRf4ce->thread = direct_thread_create(DTT_DEFAULT, BDFB_QueueRF4CEThread,  pInputRf4ce, "BDFB_QueueRF4CE");

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
    BDFB_rf4ce_info *pInputRf4ce = (BDFB_rf4ce_info *) pDriverData;

    if ( pInputRf4ce )
    {
        /* Cancel RF4CE Input thread... */
        pInputRf4ce->cancel_thread = true;
        fusion_skirmish_prevail( &pInputRf4ce->lock );
        fusion_skirmish_notify( &pInputRf4ce->lock );
        fusion_skirmish_dismiss( &pInputRf4ce->lock );
        direct_thread_join(pInputRf4ce->thread);
        direct_thread_destroy(pInputRf4ce->thread);

        /* Destroy the locks */
        fusion_skirmish_destroy(&pInputRf4ce->lock);

        /* Free data structure */
        D_FREE( pInputRf4ce );
    }
    return;
}       /* end of driver_close_device() */

