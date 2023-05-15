/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_user_io.c $
 * $brcm_Revision: 26 $
 * $brcm_Date: 3/15/12 12:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_user_io.c $
 * 
 * 26   3/15/12 12:29p erickson
 * SW7425-1747: remove old code
 * 
 * 25   1/12/12 4:19p jrubio
 * SW7346-400: disregard repeat buttons for NEC remote using LSB
 * 
 * 24   1/12/12 10:16a jrubio
 * SW7346-400: add silver remote
 * 
 * 23   9/27/11 11:55a erickson
 * SW7231-370: extend buser_input_event to pass 48 bits of code
 * information with a separate repeat flag.
 * 
 * 22   4/27/11 4:54p jrubio
 * SW7346-158: fail gracefully with write led calls
 * 
 * 21   2/10/11 6:32p randyjew
 * SW7344-26: Disable Call to NEXUS_Led_WriteString for SPI LED
 * 
 * 20   8/17/10 6:34p mward
 * SW7125-395:  buser_input_open() clean up the event it set up in case of
 * failure.
 * 
 * 19   4/28/10 7:07p mward
 * SW7125-33: Initially clear LEDs on  BCM97019, and disable
 * buser_output_display_message() on that platform.
 * 
 * 18   8/19/09 11:51a jrubio
 * PR55232: disable LED's 7340/7342
 * 
 * 17   3/25/09 2:39p katrep
 * PR53577: New 97466 board uses IR1 on GPIP 23
 * 
 * 16   3/9/09 7:20p mward
 * PR52468: Setup ui->event before opening input device, otherwise, an
 * event from device may occur before it is ready.
 * 
 * 15   3/4/09 7:47p jgarrett
 * PR 52269: Adding XMP support
 * 
 * 14   2/4/09 10:23a jrubio
 * PR51629: add 7336 support
 * 
 * Trinity_Xmp_Support/4   2/24/09 10:55a prasadv
 * complete XMP support now in bsettop_user_io.
 * 
 * Trinity_Xmp_Support/3   2/18/09 3:52p prasadv
 * Added dummy functions for XMP. cleaned up the code.
 * 
 * Trinity_Xmp_Support/2   2/6/09 5:08p prasadv
 * Separate Macros for XMP1 and XMP2. removed redundancy.
 * 
 * Trinity_Xmp_Support/1   1/28/09 8:02p prasadv
 * Adding XMP support. Maintaining under trinity support.
 * 
 * 13   6/11/08 5:57p jgarrett
 * PR 43489: Removing LED write errors for 3548/3556
 * 
 * 12   5/23/08 11:41a jrubio
 * PR42353: add 7335 support
 * 
 * 11   4/11/08 11:17a erickson
 * PR36068: fix non-uhf warnings
 *
 * 10   4/3/08 5:42p jgarrett
 * PR 41312: Setting callback events
 *
 * 9   3/27/08 5:31p jrubio
 * PR40919: add uhf support for both DirecTV and Echo
 *
 * 8   3/19/08 1:23p erickson
 * PR36068: impl UhfInput mapping
 *
 * 7   3/19/08 1:15p erickson
 * PR36068: implement Keypad
 *
 * 6   3/11/08 2:09p erickson
 * PR36068: eliminate Led_WriteString error for 3563
 *
 * 5   2/26/08 1:54p erickson
 * PR36068: fix additional IR inputs, impl user_output
 *
 * 4   12/12/07 9:06a erickson
 * PR36068: wrap with UHF/KEYPAD_SUPPORT macros
 *
 * 3   12/5/07 4:09p erickson
 * PR36068: add ir_input
 *
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 ***************************************************************************/
#include "bsettop_impl.h"
#include "nexus_ir_input.h"
#include "nexus_led.h"
#if NEXUS_HAS_UHF_INPUT
#include "nexus_uhf_input.h"
#endif
#if NEXUS_HAS_KEYPAD
#include "nexus_keypad.h"
#endif

#if defined(XMP1_SUPPORT) || defined(XMP2_SUPPORT)
static buser_input_xmp_t buser_xmp_input_open(void);
static void buser_xmp_input_close(buser_input_xmp_t ui_xmp);

static void buser_xmp_GetDefaultStatus(buser_input_xmp_status *pStatus);
static void buser_xmp_GetDefaultSettings(buser_input_xmp_settings *pSettings);
#endif /* Functions common to XMP1 and XMP2 Operation */

#ifdef XMP2_SUPPORT
static void bxmp_p_callback(void *context, int param);
static void xmp_event_callback(void *pParam1);
static bresult is_xmp2_operation(buser_input_xmp_t ui_xmp);
#endif /* Functions only for XMP2 Operation */

BDBG_MODULE(user_io);

struct buser_input {
    NEXUS_IrInputHandle nIrInput;
#if NEXUS_HAS_UHF_INPUT
    NEXUS_UhfInputHandle nUhfInput;
    uint8_t index;
    bool used;
#endif
#if NEXUS_HAS_KEYPAD
    NEXUS_KeypadHandle nKeypad;
#endif
    buser_input_settings settings;
    B_EventHandle event;
    b_event_id_t eventId;
#if defined(XMP1_SUPPORT) || defined(XMP2_SUPPORT)
    void *xmp_private;          /* For storing the XMP remote handle */
    bool xmp_remote;            /* XMP remote used or not */
#endif
};

#if NEXUS_HAS_UHF_INPUT
buser_input_t g_uhfInputs[NEXUS_NUM_UHF_INPUTS];
#endif

struct buser_output {
    NEXUS_LedHandle led;
};

static void buser_input_p_callback_handler(void *context);
#if NEXUS_HAS_UHF_INPUT
static void buser_uhf_close(buser_input_t ui);
#endif

static bresult buser_input_p_setup_event(buser_input_t ui)
{
    BDBG_ASSERT(NULL != ui);
    ui->event = B_Event_Create(NULL);
    if ( NULL == ui->event )
    {
        return BSETTOP_ERROR(berr_external_error);
    }
    ui->eventId = b_event_register(ui->event, buser_input_p_callback_handler, ui);
    if ( NULL == ui->eventId )
    {
        B_Event_Destroy(ui->event);
        ui->event = NULL;
        return BSETTOP_ERROR(berr_external_error);
    }
    return b_ok;
}

static void buser_input_p_cleanup_event(buser_input_t ui)
{
    BDBG_ASSERT(NULL != ui);
    if ( NULL != ui->eventId )
    {
        b_event_unregister(ui->eventId);
        ui->eventId = NULL;
    }
    if ( NULL != ui->event )
    {
        B_Event_Destroy(ui->event);
        ui->event = NULL;
    }
}

static void buser_input_p_callback(void *context, int param)
{
    buser_input_t ui = (buser_input_t)context;
#ifdef XMP2_SUPPORT
    NEXUS_Error rc;
    bresult xmp2_remote = berr_not_supported;
    NEXUS_IrInputEvent irEvent;
    bool overflow;
    unsigned num;
    buser_input_event *data;
    if(ui->xmp_remote){
        xmp2_remote = is_xmp2_operation(ui->xmp_private);/* If xmp2 remote needs to be used */
        if(xmp2_remote == b_ok){
            if (ui->nIrInput) {
                    rc = NEXUS_IrInput_GetEvents(ui->nIrInput, &irEvent, 1, &num, &overflow);
                    if (rc){
                        BDBG_ERR(("Unable to Get Events from IR"));
                    }
                    data->code = irEvent.code;
                    bsettop_p_userio_xmp(ui->xmp_private, &(data->code));
            }
        }
    }
#endif
    BSTD_UNUSED(param);
    B_Event_Set(ui->event);
}

static void buser_input_p_callback_handler(void *context)
{
    buser_input_t ui = (buser_input_t)context;
    if (ui->settings.data_ready_callback) {
        b_unlock();
        (*ui->settings.data_ready_callback)(ui->settings.callback_context);
        b_lock();
    }
}


#if NEXUS_HAS_UHF_INPUT
static buser_input_t buser_uhf_open(unsigned index)
{
     NEXUS_UhfInputSettings uhfSettings;
     NEXUS_Error rc = 0;
     int i =0;
     bool error = true;
     bresult result;

     NEXUS_UhfInput_GetDefaultSettings(&uhfSettings);
     uhfSettings.dataReady.callback = buser_input_p_callback;

     /* Look for Free UHF input */
     for(i=0;i<NEXUS_NUM_UHF_INPUTS;i++)
     {
         if( !g_uhfInputs[i]->used)
         {
             uhfSettings.dataReady.context = g_uhfInputs[i];
             g_uhfInputs[i]->used= true;
             error = false;
             break;
         }
     }

     if(error)
     {
         BDBG_ERR((" No more UHF channels to allocate"));
         rc = BERR_TRACE(NEXUS_UNKNOWN);
         return NULL;
     }

    /* Pick the Mode of Operation for this Channel */
    if (index == 5)
     uhfSettings.channel = NEXUS_UhfInputMode_eChannel9;
    else
        uhfSettings.channel = NEXUS_UhfInputMode_eChannel1;

    result = buser_input_p_setup_event(g_uhfInputs[i]);
    if ( result )
    {
        BSETTOP_ERROR(rc);
        return NULL;
    }

    g_uhfInputs[i]->nUhfInput = NEXUS_UhfInput_Open(g_uhfInputs[i]->index, &uhfSettings);
    if (!g_uhfInputs[i]->nUhfInput) rc = BERR_TRACE(NEXUS_UNKNOWN);


    return g_uhfInputs[i];

}

static void buser_uhf_close(buser_input_t ui)
{
    buser_input_p_cleanup_event(ui);

    if(ui->used)
        NEXUS_UhfInput_Close(ui->nUhfInput);

   ui->used=false;
   ui->nUhfInput = NULL;

}
#endif

void buserio_p_init()
{

#if NEXUS_HAS_UHF_INPUT
    int i;
    for(i =0; i<NEXUS_NUM_UHF_INPUTS; i++)
    {
        g_uhfInputs[i] = BKNI_Malloc(sizeof(*g_uhfInputs[i]));
        BKNI_Memset(g_uhfInputs[i], 0, sizeof(*g_uhfInputs[i]));
        g_uhfInputs[i]->index = i;
    }
#endif

}

void buserio_p_uninit()
{

#if NEXUS_HAS_UHF_INPUT
    int i;
    for(i =0; i<NEXUS_NUM_UHF_INPUTS; i++)
    {
        if(g_uhfInputs[i]->used)
            buser_uhf_close(g_uhfInputs[i]);

        BKNI_Free(g_uhfInputs[i]);
        g_uhfInputs[i] = NULL;
    }
#endif

}


buser_input_t buser_input_open(bobject_t user_input_id)
{
    unsigned index = B_ID_GET_INDEX(user_input_id);
    buser_input_t ui = NULL;
    NEXUS_Error rc = 0;
    NEXUS_IrInputSettings irInputSettings;


#if NEXUS_HAS_UHF_INPUT
    if(index == 5 || index ==6)
         return buser_uhf_open(index);
#endif

    ui = BKNI_Malloc(sizeof(*ui));
    BKNI_Memset(ui, 0, sizeof(*ui));
#if defined(XMP1_SUPPORT) || defined(XMP2_SUPPORT)
    ui->xmp_remote = false; 
#endif
    NEXUS_IrInput_GetDefaultSettings(&irInputSettings);
    irInputSettings.dataReady.callback = buser_input_p_callback;
    irInputSettings.dataReady.context = ui;
#if BCM_BOARD == 97466
    irInputSettings.channel_number=1;
#endif
    rc = buser_input_p_setup_event(ui);
    if ( rc )
    {
        BSETTOP_ERROR(rc);
        return NULL;
    }

    /* This index mapping is carried over from previous settop api impl's. */
    switch (index) {
    case 0:
        irInputSettings.mode = NEXUS_IrInputMode_eRemoteA;
        ui->nIrInput = NEXUS_IrInput_Open(0, &irInputSettings);
        if (!ui->nIrInput) rc = BERR_TRACE(NEXUS_UNKNOWN);
        break;
    case 1:
        irInputSettings.mode = NEXUS_IrInputMode_eRemoteB;
        ui->nIrInput = NEXUS_IrInput_Open(1, &irInputSettings);
        if (!ui->nIrInput) rc = BERR_TRACE(NEXUS_UNKNOWN);
        break;
    case 2:
        irInputSettings.mode = NEXUS_IrInputMode_eSejin56KhzKbd;
        ui->nIrInput = NEXUS_IrInput_Open(2, &irInputSettings);
        if (!ui->nIrInput) rc = BERR_TRACE(NEXUS_UNKNOWN);
        break;
#if NEXUS_HAS_KEYPAD
    case 3:
        {
            NEXUS_KeypadSettings keypadSettings;
            NEXUS_Keypad_GetDefaultSettings(&keypadSettings);
            keypadSettings.dataReady.callback = buser_input_p_callback;
            keypadSettings.dataReady.context = ui;
            ui->nKeypad = NEXUS_Keypad_Open(0, &keypadSettings);
            if (!ui->nKeypad) rc = BERR_TRACE(NEXUS_UNKNOWN);
        }
        break;
#endif
    case 4:
        irInputSettings.mode = NEXUS_IrInputMode_eCirEchoStar;
        ui->nIrInput = NEXUS_IrInput_Open(0, &irInputSettings);
        if (!ui->nIrInput) rc = BERR_TRACE(NEXUS_UNKNOWN);
        break;
    case 7: 
#if defined(XMP1_SUPPORT) || defined(XMP2_SUPPORT)
        ui->xmp_remote = true;
        ui->xmp_private = buser_xmp_input_open(); 
#endif
        irInputSettings.mode = NEXUS_IrInputMode_eCirXmp;
        ui->nIrInput = NEXUS_IrInput_Open(0, &irInputSettings);
        if (!ui->nIrInput) rc = BERR_TRACE(NEXUS_UNKNOWN);
        break;
    case 8:
        irInputSettings.mode = NEXUS_IrInputMode_eCirNec;
        ui->nIrInput = NEXUS_IrInput_Open(0, &irInputSettings);
        if (!ui->nIrInput) rc = BERR_TRACE(NEXUS_UNKNOWN);
        break;
  default:
        break;
    }
    if (rc) {
		buser_input_p_cleanup_event(ui);
        BKNI_Free(ui);
        return NULL;
    }

    return ui;
}

void buser_input_close(buser_input_t ui)
{
    if (ui->nIrInput) {
        NEXUS_IrInput_Close(ui->nIrInput);
    }
#if NEXUS_HAS_KEYPAD
    if (ui->nKeypad) {
        NEXUS_Keypad_Close(ui->nKeypad);
    }
#endif
#if NEXUS_HAS_UHF_INPUT
    if (ui->nUhfInput) {
        buser_uhf_close(ui);
        return;
    }
#endif
#if defined(XMP1_SUPPORT) || defined(XMP2_SUPPORT)
    if(ui->xmp_remote) {
        buser_xmp_input_close(ui->xmp_private);
    }
#endif
    buser_input_p_cleanup_event(ui);

     BKNI_Free(ui);
}


void buser_input_get_settings(buser_input_t ui, buser_input_settings *settings)
{
    *settings = ui->settings;
}

void buser_input_set_settings(buser_input_t ui, const buser_input_settings *settings)
{
    ui->settings = *settings;
}

bresult buser_input_get_event(buser_input_t ui, buser_input_event *event, unsigned nevents, unsigned *result_nevents)
{
    NEXUS_IrInputSettings irInputSettings;

    *result_nevents = 0;
    
    if (ui->nIrInput) {
        while (nevents) {
            NEXUS_Error rc;
            NEXUS_IrInputEvent irEvent;
            bool overflow;
            unsigned num;

            rc = NEXUS_IrInput_GetEvents(ui->nIrInput, &irEvent, 1, &num, &overflow);
            if (rc) return BERR_TRACE(rc);

            if (!num) break;
            BDBG_ASSERT(num == 1);
#ifdef XMP1_SUPPORT
            if(ui->xmp_remote)
                event[*result_nevents].code = irEvent.code | (irEvent.repeat?0x00FF0000:0);
            else
#endif
            NEXUS_IrInput_GetSettings(ui->nIrInput, &irInputSettings);
            if(irInputSettings.mode == NEXUS_IrInputMode_eCirNec)
                event[*result_nevents].code = irEvent.code | (irEvent.repeat?0x00000001:0);
            else
                event[*result_nevents].code = irEvent.code | (irEvent.repeat?0x80000000:0);
            
            event[*result_nevents].low_code = irEvent.code;
            event[*result_nevents].high_code = irEvent.codeHigh;
            event[*result_nevents].repeat = irEvent.repeat;

            (*result_nevents)++;
            nevents--;
        }
    }
#if NEXUS_HAS_KEYPAD
    else if (ui->nKeypad) {
        while (nevents) {
            NEXUS_Error rc;
            NEXUS_KeypadEvent keypadEvent;
            bool overflow;
            unsigned num;

            rc = NEXUS_Keypad_GetEvents(ui->nKeypad, &keypadEvent, 1, &num, &overflow);
            if (rc) return BERR_TRACE(rc);

            if (!num) break;
            BDBG_ASSERT(num == 1);

            event[*result_nevents].code = keypadEvent.code | (keypadEvent.repeat?0x80000000:0);
            (*result_nevents)++;
            nevents--;
        }
    }
#endif
#if NEXUS_HAS_UHF_INPUT
    else if (ui->nUhfInput) {
        while (nevents) {
            NEXUS_Error rc;
            NEXUS_UhfInputEvent uhfInputEvent;
            bool overflow;
            unsigned num;

            rc = NEXUS_UhfInput_GetEvents(ui->nUhfInput, &uhfInputEvent, 1, &num, &overflow);
            if (rc) return BERR_TRACE(rc);

            if (!num) break;
            BDBG_ASSERT(num == 1);

            event[*result_nevents].code = uhfInputEvent.code | (uhfInputEvent.repeat?0x80000000:0);
            (*result_nevents)++;
            nevents--;
        }
    }
#endif

    return 0;
}

buser_output_t buser_output_open(bobject_t user_output_id)
{
    buser_output_t uo = BKNI_Malloc(sizeof(*uo));
    uo->led = NEXUS_Led_Open(B_ID_GET_INDEX(user_output_id), NULL);
	#if NEXUS_PLATFORM==97019
    (void)NEXUS_Led_WriteString(uo->led, "    ", 0);
	#endif
    return uo;
}

void buser_output_close(buser_output_t uo)
{
    NEXUS_Led_Close(uo->led);
    BKNI_Free(uo);
}

bresult buser_output_set_led(buser_output_t uo, unsigned led, bool on)
{
    return NEXUS_Led_SetLedState(uo->led, led, on);
}

bresult buser_output_display_message(buser_output_t uo, const char *message)
{
#if BCHP_CHIP == 7335  || BCHP_CHIP ==7336 || BCHP_CHIP == 7325 || \
    BCHP_CHIP==7342 || BCHP_CHIP==7340 || BCHP_CHIP==7344 || NEXUS_PLATFORM==97019 || BCHP_CHIP==7346
    /* allow it to fail silently. SPI LED does not support this function */
    BSTD_UNUSED(uo);
    BSTD_UNUSED(message);
    return 0;
#else
    return NEXUS_Led_WriteString(uo->led, message, 0);
#endif
}

#if defined(XMP1_SUPPORT) || defined(XMP2_SUPPORT)
/**  Summary
    This function gets default settings for the structure. 
 **/
static void buser_xmp_GetDefaultSettings(buser_input_xmp_settings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->callback_context = NULL;
    pSettings->xmp_data_ready_callback = NULL;
    pSettings->xmp1_owner = 0;
    pSettings->xmp1_registry = 0;
    pSettings->xmp2_owner = 0;
    pSettings->xmp2_remote_registry = 0;
    pSettings->xmp2_tag = 0;
    pSettings->xmp2_transceiver_registry = 0;
    pSettings->xmp2remote = false;
}

/**  Summary
    This function gets default status for the structure. 
 **/
static void buser_xmp_GetDefaultStatus(buser_input_xmp_status *pStatus)
{
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->data_counter = 0;
    pStatus->previous_ack = 0;
    pStatus->response = 0;
    pStatus->status = 0;
    pStatus->xmp_response_event_registered = false;
}

/*
Summary:
    Open a user input object for receiving IR remote.
*/
static buser_input_xmp_t buser_xmp_input_open(void)  {
    buser_input_xmp_t ui_xmp=NULL;

    ui_xmp = BKNI_Malloc(sizeof(*ui_xmp));
    BKNI_Memset(ui_xmp, 0, sizeof(*ui_xmp));

    buser_xmp_GetDefaultSettings(&ui_xmp->xmp_settings);
    buser_xmp_GetDefaultStatus(&ui_xmp->xmp_status);

#ifdef XMP2_SUPPORT
        NEXUS_IrBlasterSettings pSettings;
        NEXUS_IrBlaster_GetDefaultSettings(&pSettings);
        pSettings.mode = NEXUS_IrBlasterMode_eXmp2;
        pSettings.transmitComplete.callback = bxmp_p_callback;
        pSettings.transmitComplete.context = ui_xmp;
        ui_xmp->irBlaster = NEXUS_IrBlaster_Open(0, &pSettings); 

        ui_xmp->xmp_response_event = B_Event_Create(NULL);
        ui_xmp->xmp_status_event = B_Event_Create(NULL);
        ui_xmp->irb_event = B_Event_Create(NULL);
#endif
    return ui_xmp;
}


/**
Summary:
Get the current XMP2 settings from a buser_input_xmp_t handle.
Description:
This can be called after buser_xmp_input_open to obtain the initial settings.
**/

void buser_input_get_xmp_settings(
    buser_input_t ui, 
    buser_input_xmp_settings *settings)
{
    buser_input_xmp_t ui_xmp;
    ui_xmp = ui->xmp_private;
    *settings = ui_xmp->xmp_settings;
}

/**
Summary:
Set new XMP2 settings for a buser_input_xmp_t handle.
**/
void buser_input_set_xmp_settings(
    buser_input_t ui,
    const buser_input_xmp_settings *settings)
{
    buser_input_xmp_t ui_xmp;
    ui_xmp = ui->xmp_private;
    ui_xmp->xmp_settings = *settings;
#ifdef XMP2_SUPPORT
    if (settings->xmp_data_ready_callback) {
        if (ui_xmp->xmp_status.xmp_response_event_registered) {
            b_event_unregister(ui_xmp->xmp_response_event_id);
        }
        ui_xmp->xmp_response_event_id = b_event_register(ui_xmp->xmp_response_event,
            (b_event_callback_t)xmp_event_callback, ui_xmp);
        ui_xmp->xmp_status.xmp_response_event_registered = true;
    }
#endif
}

/*
Summary:
    Close a user xmp input handle.
Description:
    Releases all resources associated with the user xmp input object
*/
static void buser_xmp_input_close(buser_input_xmp_t ui_xmp)  {
#ifdef XMP2_SUPPORT
    if (ui_xmp->xmp_status.xmp_response_event_registered) {
        b_event_unregister(ui_xmp->xmp_response_event_id);
        ui_xmp->xmp_status.xmp_response_event_registered = false;
    }
    B_Event_Destroy(ui_xmp->xmp_response_event);
    ui_xmp->xmp_response_event = NULL;
    B_Event_Destroy(ui_xmp->xmp_status_event);
    ui_xmp->xmp_status_event = NULL;
    B_Event_Destroy(ui_xmp->irb_event);
    ui_xmp->irb_event = NULL;
#endif
    BKNI_Free(ui_xmp);
}
#else 
void buser_input_get_xmp_settings(
    buser_input_t ui, 
    buser_input_xmp_settings *settings)
{
    BSTD_UNUSED(ui);
    BSTD_UNUSED(settings);
}

void buser_input_set_xmp_settings(
    buser_input_t ui,
    const buser_input_xmp_settings *settings)
{
    BSTD_UNUSED(ui);
    BSTD_UNUSED(settings);
}
#endif /* XMP1 and XMP2 Operation common */


#ifdef XMP2_SUPPORT
/**  Summary
    Callback registered when the response from the remote is got 
**/
static void xmp_event_callback(void *pParam1)
{
    buser_input_xmp_t ui_xmp = (buser_input_xmp_t)pParam1;
    b_unlock();
    if (ui_xmp->xmp_settings.xmp_data_ready_callback) {
        (*ui_xmp->xmp_settings.xmp_data_ready_callback)(ui_xmp->xmp_settings.callback_context);
    }
    b_lock();
}

/**  Summary
*   For setting the irb event when data is sent by the ir
*   blaster
**/

static void bxmp_p_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    buser_input_xmp_t ui_xmp = (buser_input_xmp_t)context;
    B_Event_Set(ui_xmp->irb_event);
}

/*
Summary:
    Check if XMP2 Operation is used or not.
*/

static bresult is_xmp2_operation(buser_input_xmp_t ui_xmp)
{
    if(ui_xmp->xmp_settings.xmp2remote)
        return b_ok;
    else
        return berr_not_supported;
}

/* Summary
   wrapper for receiving XMP data from the remote.

   This functions receives the XMP data from the remote.
   The data can be register packet, data packet, response packet 
   This is a 4 byte data. The ISR sets an event and this function wakes up.
   Input: remote handle, pointer to output buffer
   Output: Returns the four bytes of Output data at a time.
   *output_data is always 8 bit*

 */
bresult buser_input_receive_xmp(buser_input_t remote_ui,  uint8_t *output_data, 
                                size_t length, unsigned *output_size, uint32_t *data_packet_output)
{
    buser_input_xmp_t remote;
    remote = remote_ui->xmp_private;
    return buser_input_p_receive_xmp(remote, output_data, length, output_size, data_packet_output);
}


/*  Summary
    wrapper for sending XMP data to the remote.
    This function is called by the application for sending the bytes from the STB to the remote.
    Input: Remote Handle, Pointer to input buffer, input size, 
    Return: success or external error
 */

bresult buser_input_send_xmp(buser_input_t remote_ui, const uint8_t *input_buffer, size_t length) 
{
    buser_input_xmp_t remote;
    remote = remote_ui->xmp_private;
    return buser_input_p_send_xmp(remote, input_buffer, length);
}
#else

bresult buser_input_receive_xmp(buser_input_t remote_ui,  uint8_t *output_data, 
                                size_t length, unsigned *output_size, uint32_t *data_packet_output)
{
    BSTD_UNUSED(remote_ui);
    BSTD_UNUSED(output_data);
    BSTD_UNUSED(length);
    BSTD_UNUSED(output_size);
    BSTD_UNUSED(data_packet_output);
    return berr_not_supported;
}


bresult buser_input_send_xmp(buser_input_t remote_ui, const uint8_t *input_buffer, size_t length) 
{
    BSTD_UNUSED(remote_ui);
    BSTD_UNUSED(input_buffer);
    BSTD_UNUSED(length);
    return berr_not_supported;
}

#endif /* XMP2 operation specific */
