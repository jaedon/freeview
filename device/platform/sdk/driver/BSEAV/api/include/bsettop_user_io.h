/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_user_io.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/27/11 11:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_user_io.h $
 * 
 * 4   9/27/11 11:55a erickson
 * SW7231-370: extend buser_input_event to pass 48 bits of code
 * information with a separate repeat flag.
 * 
 * 3   3/4/09 7:47p jgarrett
 * PR 52269: Adding XMP support
 * 
 * Trinity_Xmp_Support/1   2/24/09 11:21a prasadv
 * Add XMP support into user io. code cleanup.
 * 
 * 2   3/25/08 10:12a jgarrett
 * PR 40857: Removing private function
 * 
 * 1   2/7/05 7:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/16   1/5/05 3:06p ywu
 * PR 13748: Create an IR keystrokes automation utility
 * bsettop_keystroke.c/h
 * 
 * Irvine_BSEAVSW_Devel/15   7/6/04 3:53p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/14   6/29/04 11:28a erickson
 * PR11135: updated deprecation notices for settop api
 *
 * Irvine_BSEAVSW_Devel/13   1/15/04 4:40p vsilyaev
 * PR 8850: Fixed type in the argument name.
 *
 * Irvine_BSEAVSW_Devel/Irvine_BSEAVSW_Devel_7038/2   12/17/03 4:21p erickson
 * PR8850: clarify which Sejin IR keyboard is supported
 *
 ***************************************************************************/
#ifndef BSETTOP_USER_IO_H__
#define BSETTOP_USER_IO_H__


#ifdef __cplusplus
extern "C"
{
#endif

/*=*********************
Control user input and output, including:

o LED's, including 7-segment LED's
o Remote controls
o Front panel
o IR keyboards

The key codes are passed through unmodified in a 32-bit structure.
***********************/

/* User Input API */

/*
Summary:
    Open a user input object for receiving IR remote and keypad input.
Description:
    For now, the following id's are used:
    0 - remote a
    1 - remote b
    2 - 56 MHz Sejin IR Keyboard
    3 - keypad
*/
buser_input_t buser_input_open(
                     bobject_t user_input_id /* user input object id */
                    );

/*
Summary:
    Close a user input handle.
Description:
    Releases all resources associated with the user input object
*/
void buser_input_close(
        buser_input_t ui /* user input object */
    );

/**
Summary:
Settings to control the user input interface.
**/
typedef struct buser_input_settings {
    bsettop_callback data_ready_callback; /* The callback is called whenever
            data becomes available. It is allowed for the user to call buser_input_get_event
            in the callback. You may receive more than one callback before
            calling buser_input_get_event. */
    void *callback_context; /* User defined context which is passed into the
            above callback. */
} buser_input_settings;

/**
Summary:
Get the current settings from a buser_input_t handle.
Description:
This can be called after buser_input_open to obtain the initial settings.
**/
void buser_input_get_settings(
    buser_input_t ui,
    buser_input_settings *settings);

/**
Summary:
Set new settings for a buser_input_t handle.
**/
void buser_input_set_settings(
    buser_input_t ui,
    const buser_input_settings *settings);

/*
Summary:
Representation of a user event.
Description:
The actual protocol for each input device is different. But they all
fit into the same storage space.
*/
typedef struct buser_input_event {
    /* deprecated */
    uint32_t code;      /* lower 31 bits of event information, 32nd bit is set to 1 as  a repeat flag. */
    
    /* new, recommended code */
    uint32_t low_code;  /* lower 32 bits of event information without the repeat flag. */
    uint32_t high_code; /* upper 32 bits of event information. */
    bool repeat;        /* this event is a repeat code. repeat == code&0x80000000. */
} buser_input_event;


/*
Summary:
Read events from a user input device.

Description:
Because this function does not return a void* to raw data, but an array of structures,
it is not called buser_input_read.
*/
bresult buser_input_get_event(
    buser_input_t ui, /* user input object */
    buser_input_event *event, /* [out,size_is(nevents)] event from the user */
    unsigned nevents, /* number of entries in the event array */
    unsigned *result_nevents /* [out] number of entries read */
    );

/* End of user input API  */

/* User Output API */

/*
Summary:
    Open a user output object for setting LEDS.
*/
buser_output_t buser_output_open(
    bobject_t user_output_id /* user output object id */
    );

/*
Summary:
    Close a user output object.
Description:
   Releases all resources associated with the user output object
*/
void buser_output_close(
    buser_output_t uo /* user output object */
    );

/*
Summary:
    Set or clear an LED on the front panel.
*/
bresult buser_output_set_led(
    buser_output_t ui, /* user output object */
    unsigned led, /* LED to control. The meaning of this number varies
                    for each platform and can change meaning depending on
                    how the outside box is labelled and the color of
                    the LED. */
    bool on         /* If true, the light is on. Otherwise the light is off. */
    );

/*
Summary:
    Write a message to the seven-segment LED digital display.
*/
bresult buser_output_display_message(
    buser_output_t ui, /* user output object */
    const char *message /* Message to be written. The number of characters
                            that can be displayed depends on the platform.
                            The message is always left-aligned and truncated
                            if needed. */
    );

/* End of user output API */

/* The following Public functions and structures have been added for XMP Support */
/**
Summary:
Settings to control the XMP2.
**/
typedef struct buser_input_xmp_settings{
    bsettop_callback xmp_data_ready_callback; /* The callback is called whenever
            data becomes available. */
    void *callback_context; /* User defined context which is passed into the above callback. */
    uint32_t xmp1_owner; /* owner code for the XMP1 protocol.*/
    uint32_t xmp1_registry; /* 32 bit registry code for the XMP1 protocol.*/
    uint32_t xmp2_owner; /* owner code for the XMP2 protocol */
    uint32_t xmp2_tag; /* tag for the XMP2 protocol */
    uint32_t xmp2_remote_registry; /* 32 bit remote registry code for the XMP2 protocol */
    uint32_t xmp2_transceiver_registry; /* 32 bit transciever registry code for the XMP2 protocol */
    bool xmp2remote; /* This indicates whether XMP2 operation is used or not. */
}buser_input_xmp_settings;

/**
Summary:
Status of an XMP2 device. 
*status and previous_ack are always 8 bit* 
**/
typedef struct buser_input_xmp_status{
    uint8_t status; /* 8 bit ack/nak from the remote->STB and viceversa */
    uint32_t response;/* 32 bit data received from the Remote */
    uint8_t previous_ack; /* 8 bit to hold the previous ack */
    unsigned data_counter; /* counter to serve no: of bytes received */
    bool xmp_response_event_registered; /* bool for setting up various events */
}buser_input_xmp_status;

/**
Summary:
Set new XMP2 settings for a buser_input_xmp_t handle.
**/

void buser_input_set_xmp_settings(
    buser_input_t ui,
    const buser_input_xmp_settings *settings);

/**
Summary:
Get the current XMP2 settings from a buser_input_xmp_t handle.
Description:
This can be called after buser_xmp_input_open to obtain the initial settings.
**/

void buser_input_get_xmp_settings(
    buser_input_t ui,
    buser_input_xmp_settings *settings);

/** Summary
    This function is called by the application for sending the bytes from the STB to the remote.
    Input: Remote Handle, Pointer to input buffer, input size, 
    Output: Pointer to output buffer, Output size
    Return: success or external error
    input_buffer always holds 1 byte data.
 **/
bresult buser_input_send_xmp(
    buser_input_t remote, 
    const uint8_t *input_buffer, /* [out,size_is(length)] */
    size_t length 
    );

/** Summary
*  This function is called by the application for receiving the
*  bytes from the remote.
*  Input: Remote Handle, Output: Returns the output data 1 at a
*  time
*  length is Max output buffer size
*  output_size is max number of output bytes received
*  data_packet is the 32 bit data received from the remote
*  output_data always holds 1 byte data
* 
**/

bresult buser_input_receive_xmp(
    buser_input_t remote, 
    uint8_t *output_data, /* [out,size_is(length)] */
    size_t length,
    unsigned *output_size,
    uint32_t *data_packet
    );

/* End of XMP Support */


#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_USER_IO_H__ */

