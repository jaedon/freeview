/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_smartcard.c $
 * $brcm_Revision: 22 $
 * $brcm_Date: 5/24/12 5:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: X:/win_views/97552/BSEAV/api/src/nexus/bsettop_smartcard.c $
 * 
 * 22   5/24/12 5:11p vishk
 * SW7358-217, SW7425-3060: Code clean up to remove all references to
 * insertCard and removeCard.
 * 
 * 21   5/24/12 12:27p vishk
 * SW7358-217, SW7425-3060: Remove insertCard and removeCard and add
 * cardCallback.
 * 
 * 20   3/29/12 10:58a jessem
 * SW7425-2789: Fixed coverity issue 37109.
 *
 * 19   1/25/12 10:01a mward
 * SW7405-5594:  'NEXUS_SmartcardStatus' has no member named 'err'
 *
 * 18   4/21/11 11:04a jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 *
 * 17   8/24/10 3:47p VISHK
 * SW7125-598: Need to add the IFSI param interface in
 * NEXUS_Smartcard_SetSettings
 *
 * 16   8/16/10 5:01p VISHK
 * SW7405-4023: SMARTCARD: Support card deactivation
 *
 * 15   8/4/10 12:17p VISHK
 * SW7125-478: NEXUS_Smartcard_SetParamsFromAtr() is not completed
 *
 * 14   12/8/08 3:56p vishk
 * PR 49706: second smartcard module not working
 *
 * 13   10/10/08 1:19p tmontoya
 * PR47512: bsmartcard_read() returns Nexus error codes...
 *
 * 12   9/11/08 2:20p vishk
 * PR 45615: ISO smartcard channel settings not correct and not
 * configurable.
 * PR 29851: Smartcard functions need to allow more parameter changes
 *
 * 11   8/22/08 1:54p vishk
 * PR 45615: ISO smartcard channel settings not correct and not
 * configurable.
 * PR 29851: Smartcard functions need to allow more parameter changes
 *
 * 10   8/22/08 11:39a vishk
 * PR 45615: ISO smartcard channel settings not correct and not
 * configurable.
 * PR 29851: Smartcard functions need to allow more parameter changes
 *
 * 9   8/19/08 2:33p vishk
 * PR 45615: ISO smartcard channel settings not correct and not
 * configurable.
 * PR 29851: Smartcard functions need to allow more parameter changes
 *
 * 8   8/19/08 1:59p vishk
 * PR 45615: ISO smartcard channel settings not correct and not
 * configurable.
 * PR 29851: Smartcard functions need to allow more parameter changes
 *
 * 7   7/25/08 7:12p vishk
 * PR 45044: bsmartcard_close() Seg-Faults using the handle returned by
 * bsmartcard_open()
 *
 * 6   6/18/08 10:06p erickson
 * PR36068: fix warning
 *
 * 5   5/29/08 8:48a erickson
 * PR39453: 3548/3556 updates
 *
 * 4   5/20/08 4:27p erickson
 * PR40034: support platforms with no smartcard
 *
 * 3   5/16/08 7:22p vishk
 * PR 40034: Develop Nexus Smartcard Interface/Module.
 * PR 40016: bsettop_smartcard: Develop SettopAPI-to-Nexus shim layer
 *
 * 2   3/25/08 10:13a jgarrett
 * PR 40857: Adding stubs for thunk layer
 *
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(smartcard);

struct bsmartcard {
    BDBG_OBJECT(bsmartcard)
#if NEXUS_HAS_SMARTCARD
    NEXUS_SmartcardHandle nSmartcard;
#endif
    bsmartcard_settings_t settings;
    BKNI_EventHandle cardEvent;
    b_event_id_t cardEventId;
};

#if NEXUS_HAS_SMARTCARD
struct bsmartcard cliChannel[NEXUS_NUM_SMARTCARD_CHANNELS];
#endif

void bsmartcard_settings_init(
    bsmartcard_settings_t *settings
)
{
    /* TODO: Real Impl */
    BDBG_ASSERT(NULL != settings);
    BKNI_Memset(settings, 0, sizeof(*settings));

    settings->protocol = bsmartcard_protocol_t0;
    settings->standard = bsmartcard_standard_nds;
    settings->ffactor = 1;
    settings->dfactor = 1;
    settings->ext_clock_divisor = 1;
    settings->tx_retries = 4;
    settings->rx_retries = 4;
    settings->work_wait_time.value = 9600;
    settings->block_wait_time.value = 971;
    settings->extra_guard_time.value= 2;
    settings->block_guard_time.value = 22;
    settings->time_out.value= 5000;
    settings->time_out.unit= bsmartcard_timer_unit_msec;
    settings->sc_pres_dbinfo.db_width = 7;
    settings->sc_pres_dbinfo.sc_pres_mode = bsmartcard_sc_presmode_mask;

    settings->callback_context = NULL;
    settings->card_callback = NULL;
}

bresult bsmartcard_init(void)
{
#if NEXUS_HAS_SMARTCARD
    BKNI_Memset(cliChannel, 0, sizeof(cliChannel));
#endif
    return b_ok;
}

void bsmartcard_get_settings(
    bsmartcard_t smartcard,
    bsmartcard_settings_t *settings
)
{
#if NEXUS_HAS_SMARTCARD
    settings = &smartcard->settings;
#else
    BSTD_UNUSED(smartcard);
    BSTD_UNUSED(settings);
    BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult bsmartcard_set_settings(
    bsmartcard_t smartcard,
    const bsmartcard_settings_t *settings
)
{
#if NEXUS_HAS_SMARTCARD

    NEXUS_Error rc;
    NEXUS_SmartcardSettings nsettings;

    NEXUS_Smartcard_GetSettings(smartcard->nSmartcard, &nsettings);

    switch (settings->standard) {
    case bsmartcard_standard_nds:
        nsettings.standard = NEXUS_SmartcardStandard_eNds;
        break;
    case bsmartcard_standard_iso:
        nsettings.standard = NEXUS_SmartcardStandard_eIso;
        break;
    case bsmartcard_standard_emv2000:
        nsettings.standard = NEXUS_SmartcardStandard_eEmv2000;
        break;
    case bsmartcard_standard_irdeto:
        nsettings.standard = NEXUS_SmartcardStandard_eIrdeto;
        break;
    default:
        nsettings.standard = NEXUS_SmartcardStandard_eUnknown;
        break;
    }

    BDBG_CASSERT(NEXUS_SmartcardProtocol_eT14 == (NEXUS_SmartcardProtocol)bsmartcard_standard_irdeto);
    nsettings.protocol = settings->protocol;

    nsettings.fFactor = settings->ffactor;
    nsettings.dFactor = settings->dfactor;
    nsettings.extClockDivisor = settings->ext_clock_divisor;
    nsettings.txRetries = settings->tx_retries;
    nsettings.rxRetries = settings->rx_retries;
    nsettings.currentIFSD = settings->current_ifsd;

    BDBG_CASSERT(NEXUS_TimerUnit_eMilliSec == (NEXUS_TimerUnit)bsmartcard_timer_unit_msec);
    nsettings.workWaitTime.value = settings->work_wait_time.value;
    nsettings.workWaitTime.unit = settings->work_wait_time.unit;
    nsettings.blockWaitTime.value = settings->block_wait_time.value;
    nsettings.blockWaitTime.unit = settings->block_wait_time.unit;
    nsettings.extraGuardTime.value = settings->extra_guard_time.value;
    nsettings.extraGuardTime.unit = settings->extra_guard_time.unit;
    nsettings.blockGuardTime.value = settings->block_guard_time.value;
    nsettings.blockGuardTime.unit = settings->block_guard_time.unit;
    nsettings.characterWaitTime = settings->character_wait_time;

    BDBG_CASSERT(NEXUS_EdcEncode_eCrc == (NEXUS_EdcEncode)bsmartcard_edc_encode_crc);
    nsettings.edcSetting.edcEncode = settings->edc_setting.edc_encode;
    nsettings.edcSetting.isEnabled = settings->edc_setting.is_enabled;
    nsettings.timeOut.value = settings->time_out.value;
    nsettings.timeOut.unit = settings->time_out.unit;
    nsettings.autoDeactiveReq = settings->auto_deactive_req;
    nsettings.nullFilter = settings->null_filter;

    BDBG_CASSERT(NEXUS_ScPresMode_eMask == (NEXUS_ScPresMode)bsmartcard_sc_presmode_mask);
    nsettings.scPresDbInfo.scPresMode = settings->sc_pres_dbinfo.sc_pres_mode;
    nsettings.scPresDbInfo.isEnabled = settings->sc_pres_dbinfo.is_enabled;
    nsettings.scPresDbInfo.dbWidth = settings->sc_pres_dbinfo.db_width;

    BDBG_CASSERT(NEXUS_ResetCardAction_eReceiveAndDecode == (NEXUS_ResetCardAction)bsmartcard_reset_card_receive_decode);
    nsettings.resetCardAction = settings->reset_card_action;
    nsettings.blockWaitTimeExt.value = settings->block_wait_time_ext.value;
    nsettings.blockWaitTimeExt.unit = settings->block_wait_time_ext.unit;
    nsettings.isPresHigh = settings->is_pres_high;

    smartcard->settings = *settings;

    rc = NEXUS_Smartcard_SetSettings(smartcard->nSmartcard, &nsettings);
    if (rc) return BERR_TRACE(rc);

    return b_ok;
#else
	BSTD_UNUSED(smartcard);
    BSTD_UNUSED(settings);
    return BSETTOP_ERROR(berr_not_supported);
#endif

}


#if NEXUS_HAS_SMARTCARD
static void bsmartcard_p_callback(void *context)
{
    bsmartcard_t smartcard = context;
    if (smartcard->settings.card_callback) {
        b_unlock();
        smartcard->settings.card_callback(smartcard->settings.callback_context);
        b_lock();
    }
}

void bsmartcard_p_event(void *context, int param)
{
	BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}
#endif

bsmartcard_t bsmartcard_open(
    bobject_t smartcard_id, /* smartcard object id */
    const bsmartcard_settings_t *settings /* smartcard settings */
)
{
#if NEXUS_HAS_SMARTCARD
	BERR_Code rc = BERR_SUCCESS;
    unsigned int index;
    NEXUS_SmartcardSettings smartcardSettings;

    BDBG_ASSERT(settings);

    index = B_ID_GET_INDEX(smartcard_id);

    if (index >= NEXUS_NUM_SMARTCARD_CHANNELS) {
        BSETTOP_ERROR(berr_invalid_parameter);
        return NULL;
    }

    if (cliChannel[index].nSmartcard) {
        /* already open */
        return NULL;
    }

    NEXUS_Smartcard_GetDefaultSettings(&smartcardSettings);

    cliChannel[index].settings.card_callback = settings->card_callback;
    cliChannel[index].settings.callback_context = settings->callback_context;


    switch (settings->standard) {
    case bsmartcard_standard_nds:
        smartcardSettings.standard = NEXUS_SmartcardStandard_eNds;
        break;
    case bsmartcard_standard_iso:
        smartcardSettings.standard = NEXUS_SmartcardStandard_eIso;
        break;
    case bsmartcard_standard_irdeto:
        smartcardSettings.standard = NEXUS_SmartcardStandard_eEmv2000;
        break;
    default:
        smartcardSettings.standard = NEXUS_SmartcardStandard_eUnknown;
        break;
    }

    smartcardSettings.protocol = settings->protocol;
    smartcardSettings.fFactor = settings->ffactor;
    smartcardSettings.dFactor = settings->dfactor;
    smartcardSettings.extClockDivisor = settings->ext_clock_divisor;
    smartcardSettings.txRetries = settings->tx_retries;
    smartcardSettings.rxRetries = settings->rx_retries;
    smartcardSettings.workWaitTime.unit= settings->work_wait_time.unit;
    smartcardSettings.workWaitTime.value= settings->work_wait_time.value;
    smartcardSettings.blockWaitTime.unit= settings->block_wait_time.unit;
    smartcardSettings.blockWaitTime.value= settings->block_wait_time.value;
    smartcardSettings.extraGuardTime.unit= settings->extra_guard_time.unit;
    smartcardSettings.extraGuardTime.value= settings->extra_guard_time.value;
    smartcardSettings.blockGuardTime.unit= settings->block_guard_time.unit;
    smartcardSettings.blockGuardTime.value= settings->block_guard_time.value;
    smartcardSettings.characterWaitTime = settings->character_wait_time;
    smartcardSettings.edcSetting.edcEncode = settings->edc_setting.edc_encode;
    smartcardSettings.edcSetting.isEnabled = settings->edc_setting.is_enabled;
    smartcardSettings.timeOut.unit = settings->time_out.unit;
    smartcardSettings.timeOut.value = settings->time_out.value;
    smartcardSettings.autoDeactiveReq = settings->auto_deactive_req;
    smartcardSettings.nullFilter = settings->null_filter;
    smartcardSettings.scPresDbInfo.dbWidth = settings->sc_pres_dbinfo.db_width;
    smartcardSettings.scPresDbInfo.isEnabled = settings->sc_pres_dbinfo.is_enabled;
    smartcardSettings.scPresDbInfo.scPresMode = settings->sc_pres_dbinfo.sc_pres_mode;
    smartcardSettings.resetCardAction = settings->reset_card_action;
    smartcardSettings.blockWaitTimeExt.unit = settings->block_wait_time_ext.unit;
    smartcardSettings.blockWaitTimeExt.value = settings->block_wait_time_ext.value;
    smartcardSettings.isPresHigh = settings->is_pres_high;
    smartcardSettings.currentIFSD = settings->current_ifsd;

    rc = BKNI_CreateEvent(&cliChannel[index].cardEvent);
	if (rc != BERR_SUCCESS)
	{
		rc = BERR_TRACE(rc);
		goto error;
	}
    cliChannel[index].cardEventId = b_event_register(cliChannel[index].cardEvent, bsmartcard_p_callback, &cliChannel[index]);
    if (!cliChannel[index].cardEventId) {
        goto error;
    }

    smartcardSettings.cardCallback.callback = bsmartcard_p_event;
    smartcardSettings.cardCallback.context = cliChannel[index].cardEvent;

    cliChannel[index].nSmartcard = NEXUS_Smartcard_Open(index, &smartcardSettings);
    if (!cliChannel[index].nSmartcard) {
        return NULL;
    }

    return &cliChannel[index];

error:
    if(cliChannel[index].cardEvent)
        BKNI_DestroyEvent(cliChannel[index].cardEvent);

    return NULL;
#else
    BSTD_UNUSED(smartcard_id);
    BSTD_UNUSED(settings);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
#endif
}

void bsmartcard_close(
    bsmartcard_t smartcard /* handle returned by bsmartcard_open */
)
{
#if NEXUS_HAS_SMARTCARD
    BDBG_MSG(("assert(smartcard)"));
    BDBG_ASSERT(smartcard);

    b_event_unregister(smartcard->cardEventId);

    if(smartcard->cardEvent)
        BKNI_DestroyEvent(smartcard->cardEvent);

    BDBG_MSG(("bsmartcard_close"));
    NEXUS_Smartcard_Close(smartcard->nSmartcard);
    smartcard->nSmartcard=NULL;
#else
    BSTD_UNUSED(smartcard);
    BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult bsmartcard_read(
    bsmartcard_t smartcard, /* handle returned by bsmartcard_open */
    void *data,             /* [out,size_is(length)] memory to read into */
    size_t length,          /* maximum number of bytes to read */
    size_t *length_read     /* [out] amount of data read into memory */
)
{
#if NEXUS_HAS_SMARTCARD
    NEXUS_Error rc;

    rc = NEXUS_Smartcard_Read(smartcard->nSmartcard, data, length, length_read);
    if (rc) BSETTOP_ERROR(berr_external_error);

    return b_ok;
#else
    BSTD_UNUSED(smartcard);
    BSTD_UNUSED(data);
    BSTD_UNUSED(length);
    BSTD_UNUSED(length_read);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult bsmartcard_write(
    bsmartcard_t smartcard, /* handle returned by bsmartcard_open */
    const void *data,       /* [size_is(length)] memory to write from */
    size_t length,          /* maximum number of bytes to write */
    size_t *length_written  /* [out] amount of data written */
)
{
#if NEXUS_HAS_SMARTCARD
    NEXUS_Error rc;

    rc = NEXUS_Smartcard_Write(smartcard->nSmartcard, data, length, length_written);
    if (rc) BSETTOP_ERROR(berr_external_error);

    return b_ok;
#else
    BSTD_UNUSED(smartcard);
    BSTD_UNUSED(data);
    BSTD_UNUSED(length);
    BSTD_UNUSED(length_written);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult bsmartcard_get_status(
    bsmartcard_t smartcard,
    bsmartcard_status *status       /* [out] Fills in the status information. */
)
{
#if NEXUS_HAS_SMARTCARD
    NEXUS_Error rc;
    NEXUS_SmartcardStatus nStatus;


    rc = NEXUS_Smartcard_GetStatus(smartcard->nSmartcard, &nStatus);
    if (rc) BSETTOP_ERROR(berr_external_error);

    status->card_present = nStatus.cardPresent;
    status->err = bsmartcard_no_error;
    status->state = nStatus.state;

    return b_ok;
#else
    BSTD_UNUSED(smartcard);
    BSTD_UNUSED(status);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult bsmartcard_reset_card(
    bsmartcard_t smartcard, /* handle returned by bsmartcard_open */
    void *data,             /* [out,size_is(length)] pointer to memory that can be read into */
    size_t length,          /* maximum number of bytes pointed to by data */
    size_t *length_read     /* [out] length of data read into the data field. */
)
{
#if NEXUS_HAS_SMARTCARD
    NEXUS_Error rc;
    rc = NEXUS_Smartcard_ResetCard(smartcard->nSmartcard, data, length, length_read);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    return b_ok;
#else
    BSTD_UNUSED(smartcard);
    BSTD_UNUSED(data);
    BSTD_UNUSED(length);
    BSTD_UNUSED(length_read);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult bsmartcard_reset(
    bsmartcard_t smartcard, /* handle returned by bsmartcard_open */
    bool warm_reset /* true for a warm reset, false for a cold reset */
)
{
#if NEXUS_HAS_SMARTCARD
    NEXUS_Error rc;
    rc = NEXUS_Smartcard_Reset(smartcard->nSmartcard, warm_reset);
    if (rc) BSETTOP_ERROR(berr_external_error);

    return b_ok;
#else
    BSTD_UNUSED(smartcard);
    BSTD_UNUSED(warm_reset);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult bsmartcard_detect_card( /* name changed: was bsmartcard_detectCardPres */
        bsmartcard_t smartcard
)
{
#if NEXUS_HAS_SMARTCARD
    NEXUS_Error rc;
    rc = NEXUS_Smartcard_DetectCard( smartcard->nSmartcard);
    if (rc) BSETTOP_ERROR(berr_external_error);

    return b_ok;
#else
    BSTD_UNUSED(smartcard);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

void bsmartcard_shutdown(void)
{
    return;
}

bresult bsmartcard_deactivate (
    bsmartcard_t smartcard
)
{
 #if NEXUS_HAS_SMARTCARD
    NEXUS_Error rc;
    rc = NEXUS_Smartcard_Deactivate( smartcard->nSmartcard);
    if (rc) BSETTOP_ERROR(berr_external_error);

    return b_ok;
#else
    BSTD_UNUSED(smartcard);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}


