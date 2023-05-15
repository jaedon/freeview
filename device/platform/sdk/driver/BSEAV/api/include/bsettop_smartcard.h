/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_smartcard.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 5/24/12 5:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_smartcard.h $
 * 
 * 10   5/24/12 5:09p mphillip
 * SW7358-217, SW7425-3060: Collapse insert/remove callbacks into a single
 * smartcard callback
 * 
 * 9   8/24/10 3:37p mphillip
 * SW7125-598: Add IFSD as a parameter to the settings structure
 * 
 * 8   8/16/10 4:56p mphillip
 * SW7405-4023: Add bsmartcard_deactivate
 * 
 * 7   8/4/10 12:17p vsilyaev
 * SW7125-478: Removed function bsmartcard_set_params_from_atr
 * 
 * 6   8/19/08 1:40p vsilyaev
 * PR 45615: Added ISO smartcard channel settings
 * 
 * 5   9/13/06 1:03p mphillip
 * PR23885: Added a card insertion/removal callback
 * 
 * 4   8/21/06 5:09p mphillip
 * PR23568: Copy/paste error, fixed bsmartcard_protocol to be
 * bsmartcard_standard for the enum values for bsmartcard_standard's
 * enums
 * 
 * 3   8/18/06 3:49p mphillip
 * PR23568: Add warm/cold parameter for bsmartcard_reset, and fix error
 * enum to be bsmartcard_err_
 * 
 * 2   8/17/06 11:38a mphillip
 * PR23568: Renaming/adjusting of the smartcard API.  Functionality is
 * still forthcoming.
 * 
 * 1   2/7/05 7:06p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/11   7/6/04 3:54p erickson
 * PR11771: settop api dataflow redesign
 * 
 * Irvine_BSEAVSW_Devel/10   6/29/04 11:28a erickson
 * PR11135: updated deprecation notices for settop api
 * 
 * Irvine_BSEAVSW_Devel/9   2/12/04 11:08a btan
 * PR 9741: Added settop API for 7038
 * 
 * Irvine_BSEAVSW_Devel/8   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 * 
 * Irvine_BSEAVSW_Devel/7   10/31/03 11:58a erickson
 * settop api changes after internal review
 * 
 * Irvine_BSEAVSW_Devel/6   10/28/03 11:28a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/5   10/10/03 5:33p erickson
 * converted all streaming api's to use size_t instead of unsigned
 * 
 * Irvine_BSEAVSW_Devel/4   10/8/03 2:19p erickson
 * got smartcard working
 *
 * Irvine_BSEAVSW_Devel/3   10/8/03 11:46a erickson
 * added initial implementation for smartcard
 *
 * Irvine_BSEAVSW_Devel/2   10/7/03 5:28p erickson
 * changed comments
 *
 * Irvine_BSEAVSW_Devel/1   10/7/03 5:17p erickson
 * initial smartcard api
 *
 ***************************************************************************/
#ifndef BSETTOP_SMARTCARD_H__
#define BSETTOP_SMARTCARD_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*=*************************
Smartcard (bsmartcard_t) supports a read/write interface to the smartcard along with
reset functions.

Asynchronous I/O is not supported yet.
****************************/

/*
Summary:
	Smartcard handle returned by bsmartcard_open.
*/
typedef struct bsmartcard *bsmartcard_t;

/*
Summary:
	Smartcard protocol enum definition.
*/
typedef enum bsmartcard_protocol {
	bsmartcard_protocol_unknown,
	bsmartcard_protocol_t0,
	bsmartcard_protocol_t1,
	bsmartcard_protocol_t14
} bsmartcard_protocol;

/*
Summary:
	Smartcard standard enum definition.
*/
typedef enum bsmartcard_standard {
	bsmartcard_standard_nds,
	bsmartcard_standard_iso,
	bsmartcard_standard_emv2000,
	bsmartcard_standard_irdeto
} bsmartcard_standard;

/*
Summary:
    This enum is to identify the unit of timer value. 

Description:
See Also:

*/
typedef enum bsmartcard_timer_unit{
    bsmartcard_timer_unit_etu = 0,  /* in Elementary Time Units */
    bsmartcard_timer_unit_clock,      /* in raw clock cycles that smart card receives */
    bsmartcard_timer_unit_msec  /* in milli seconds */
} bsmartcard_timer_unit;

/*
Summary:
    The timer value that application set to or get from the smartcard.

Description:

See Also:

*/
typedef struct bsmartcard_timer_value
{
    unsigned    value;  /* timer value */
    bsmartcard_timer_unit unit; /* units */
} bsmartcard_timer_value;

/*
Summary:
    This enum is to identify error detect code (EDC) encoding.

Description:
    This enumeration defines the supported error detect code (EDC) encoding .

See Also:
*/
typedef enum bsmartcard_edc_encode {
    bsmartcard_edc_encode_lrc= 0,
	bsmartcard_edc_encode_crc
} bsmartcard_edc_encode;

/*
Summary:
    The configuration of EDC setting for T=1 protocol only.

Description:

See Also:
 */
typedef struct bsmartcard_edc_setting
{
    bsmartcard_edc_encode edc_encode;   /* EDC encoding */
    bool    is_enabled;
} bsmartcard_edc_setting;

/* 
Summary:
    This enum is to identify read or write a specific register.

Description:
    This enumeration defines read or write of a smart card register. 

See Also:
 */
typedef enum bsmartcard_sc_presmode {
    bsmartcard_sc_presmode_debounce = 0,   
    bsmartcard_sc_presmode_mask        
}  bsmartcard_sc_presmode;

/* 
Summary:
    The configuration of Smartcard Pres Debounce.

Description:

See Also:

 */
typedef struct bsmartcard_sc_pres_dbinfo
{
    bsmartcard_sc_presmode	sc_pres_mode;   /* EDC encoding */
    bool                is_enabled; /* True if enabled. Otherwise it should be false*/
    uint8_t             db_width; 
			
} bsmartcard_sc_pres_dbinfo;

/* 
Summary:
    This enum is to identify action for BSCD_Channel_ResetCard function.

Description:
This enumeration defines the supported action for  BSCD_Channel_ResetCard function.

See Also:
BSCD_Channel_ResetCard
 */
typedef enum bsmartcard_reset_card_action{
	bsmartcard_reset_card_no_action = 0,
	bsmartcard_reset_card_receive_decode 
} bsmartcard_reset_card_action;

/*
Summary:
	Smartcard settings structure.
Description:
	Smartcard settings structure, used by bsmartcard_open.  
        This allows protocol (T=0, T=1, T=14) and standard (NDS, ISO, EMV2000, IRDETO) to be selected.
*/
typedef struct bsmartcard_settings {
    bsmartcard_protocol protocol;
    bsmartcard_standard standard;
    uint8_t ffactor;
    uint8_t dfactor;
    uint8_t ext_clock_divisor;
    uint8_t tx_retries;
    uint8_t rx_retries;
    bsmartcard_timer_value  work_wait_time;
    bsmartcard_timer_value  block_wait_time;
    bsmartcard_timer_value  extra_guard_time;
    bsmartcard_timer_value  block_guard_time;
    unsigned                character_wait_time;    
    bsmartcard_edc_setting  edc_setting;
    bsmartcard_timer_value  time_out;
    bool                    auto_deactive_req;
    bool                    null_filter;
    bsmartcard_sc_pres_dbinfo       sc_pres_dbinfo;
    bsmartcard_reset_card_action    reset_card_action;
    bsmartcard_timer_value  block_wait_time_ext;
    bool                    is_pres_high;
    void *callback_context;		/* Context pointer, returned in the callback functions */
    bsettop_callback    card_callback; /* Called when a card is inserted or removed. */
    unsigned            current_ifsd; /* This attribute indicates the current IFSD*/
} bsmartcard_settings_t;

/*
Summary:
	Smartcard setting structure initialization.
Description:
	This function initializes the smartcard settings structure to defaults.
*/
void bsmartcard_settings_init(
	bsmartcard_settings_t *settings
);

/*
Summary:
	Open a smart object which corresponds to one smartcard reader.
Description:
*/
bsmartcard_t bsmartcard_open(
	bobject_t smartcard_id, /* smartcard object id */
	const bsmartcard_settings_t *settings /* smartcard settings */
);

/*
Summary:
	Close a smartcard object.
Description:
	After closing, the bsmartcard_t handle is invalid.
*/
void bsmartcard_close(
	bsmartcard_t smartcard /* handle returned by bsmartcard_open */
);

/*
Summary:
	Get the smartcard channel settings structure a smart object which corresponds to one smartcard reader.

Description:
*/
void bsmartcard_get_settings(
    bsmartcard_t smartcard,
    bsmartcard_settings_t *settings
);

/*
Summary:
	Set the smartcard channel settings structure a smart object which corresponds to one smartcard reader.

Description:
*/
bresult bsmartcard_set_settings(
    bsmartcard_t smartcard,
    const bsmartcard_settings_t *settings
);

/*
Summary:
	Read data from the smartcard.

Description:
	If you don't call bsmartcard_set_async, this function will block until it can return
	some data or returns an error.

	If you call bsmartcard_set_async, this function will never block and will return
	either some data, no data or an error.

	If read fails, you should call bsmartcard_get_status in order to determine
	the state of the smartcard interface.
*/
bresult bsmartcard_read(
	bsmartcard_t smartcard,	/* handle returned by bsmartcard_open */
	void *data,				/* [out,size_is(length)] memory to read into */
	size_t length,			/* maximum number of bytes to read */
	size_t *length_read		/* [out] amount of data read into memory */
);

/*
Summary:
	Write data to the smartcard.

Description:
	If you don't call bsmartcard_set_async, this function will block until it can write
	some data or returns an error.

	If you call bsmartcard_set_async, this function will never block and will write
	some data or return an error.

	If write fails, you should call bsmartcard_get_status in order to determine
	the state of the smartcard interface.
*/
bresult bsmartcard_write(
	bsmartcard_t smartcard, /* handle returned by bsmartcard_open */
	const void *data, 		/* [size_is(length)] memory to write from */
	size_t length,			/* maximum number of bytes to write */
	size_t *length_written	/* [out] amount of data written */
);

/* TODO: need async notification of status change using bsettop_event */

/*
Summary:
	Smartcard error status enum.
*/
typedef enum bsmartcard_error {
	bsmartcard_no_error,
	bsmartcard_err_rx_parity,
	bsmartcard_err_tx_parity,
	bsmartcard_err_rx_timeout,
	bsmartcard_err_tx_timeout,
	bsmartcard_err_hardware_failure,
	bsmartcard_err_reset_terminal /* requires a call to bsmartcard_reset() */
} bsmartcard_error;

/*
Summary:
	Smartcard state enum.
Description:
	This represents the current state of the given slot and card.
*/
typedef enum bsmartcard_state {
	bsmartcard_state_unknown = 0, /* Unknown state (perhaps not yet initialized).  Persistent. */
	bsmartcard_state_cold_resetting, /* A cold reset has been requested but is not yet complete.  Transient. */
	bsmartcard_state_warm_resetting, /* A warm reset has been requested but is not yet complete.  Transient. */
	bsmartcard_state_reset_done, /* The slot/card reset has completed.  Persistent. */
	bsmartcard_state_activating, /* The slot/card is currently being activated, but activation is not yet complete.  Transient. */
	bsmartcard_state_receive_decode_atr, /* The ATR is being received or decoded.  Transient. */
	bsmartcard_state_ready, /* The slot/card is initialized and is awaiting sends/receives.  Persistent. */
	bsmartcard_state_transmitting, /* The slot/card is currently transmitting.  Transient. */
	bsmartcard_state_transmitted, /* The slot/card has completed its transmission.  Persistent. */
	bsmartcard_state_receiving, /* The slot/card is currently receiving.  Transient. */
	bsmartcard_state_ignore, /* The slot/card is ignoring events/commands.  Persistent. */
	bsmartcard_state_initialized, /* The slot/card has been initialized, but the ATR has not yet been received.  Persistent. */
	bsmartcard_state_max_state /* A value indicating the total number of possible states.  The state returned from bsmartcard_get_status should never exceed this value. */
} bsmartcard_state;
/*
Summary:
	Status information returned by bsmartcard_get_status.
*/
typedef struct bsmartcard_status {
	bool card_present;
	bsmartcard_error err;
	bsmartcard_state state;
} bsmartcard_status;

/*
Summary:
Get the status of the smartcard interface.
Description:
This function must be called after read or write fails in order to determine the
state of the smartcard interface.
*/
bresult bsmartcard_get_status(
	bsmartcard_t smartcard,
	bsmartcard_status *status		/* [out] Fills in the status information. */
);

/*
Summary:
	Reset a smartcard.
Description:
	Reset the smartcard itself.
*/
bresult bsmartcard_reset_card(
	bsmartcard_t smartcard, /* handle returned by bsmartcard_open */
	void *data,				/* [out,size_is(length)] pointer to memory that can be read into */
	size_t length,			/* maximum number of bytes pointed to by data */
	size_t *length_read		/* [out] length of data read into the data field. */
);

/*
Summary:
	Reset the smartcard interface.
Description:
	Reprogram all the Broadcom smartcard interface, not the card.
	If you want to reset the card, use bsmartcard_reset_card.

	The interface must be reset whenever a card is inserted.
*/
bresult bsmartcard_reset(
	bsmartcard_t smartcard, /* handle returned by bsmartcard_open */
	bool warm_reset /* true for a warm reset, false for a cold reset */
);


/*
Summary:
	Detect the card insertion.
Description:
	The function will be blocked until the card is inserted.
*/
bresult bsmartcard_detect_card( /* name changed: was bsmartcard_detectCardPres */
		bsmartcard_t smartcard
);

/*
Summary:
	Smartcard initialization.
Description:
	The function initializes the smartcard module.
*/
bresult bsmartcard_init(void);

/*
Summary:
	Smartcard shutdown.
Description:
	The function shutdowns the smartcard module.
*/

void bsmartcard_shutdown(void);
/*
Summary:
Deactivate the smartcard. 

Description:
This function is used to deactivate the smartcard. After deactivation, the 
smartcard needs to be reset in order to communicate with it again.
*/
bresult bsmartcard_deactivate (
	bsmartcard_t smartcard
);

#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_SMARTCARD_H__ */
