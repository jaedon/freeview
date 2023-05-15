/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_vbi.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 3/20/08 5:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_vbi.h $
 * 
 * 6   3/20/08 5:12p vsilyaev
 * PR 40468: Updated description of the bdisplay_vbi_tt_write
 * 
 * PR8365/1   2/15/06 8:29p shyam
 * PR 8365 : Put in CC708 changes for efficiently handling cc708 and demux
 * 608 data and 708 data
 * 
 * 4   2/13/06 3:35p katrep
 * PR18943: added bdisplay_vbi_tt_write and bstream_vbi_tt_read (not
 * implemented)
 * 
 * 3   10/28/05 7:55p rjlewis
 * PR17043: Added ability to disable CGMS and WSS output.
 * Added ability to determine if CGMS & WSS output is enabled.
 * 
 * 2   3/21/05 9:06a erickson
 * PR13306: added cgms and wss set functions
 * 
 * 1   2/7/05 7:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/5   2/3/05 12:59p erickson
 * PR13305: added bdisplay_vbi_cc_write api
 * 
 * Irvine_BSEAVSW_Devel/4   11/16/04 1:01p shyam
 * PR 8365 : Changed for supporting CC708
 * 
 * Irvine_BSEAVSW_Devel/3   7/6/04 3:54p erickson
 * PR11771: settop api dataflow redesign
 * 
 * Irvine_BSEAVSW_Devel/2   6/29/04 11:28a erickson
 * PR11135: updated deprecation notices for settop api
 * 
 * Irvine_BSEAVSW_Devel/1   11/14/03 2:10p erickson
 * PR8612: added vbi support to settop api
 *
 ***************************************************************************/
#ifndef BSETTOP_VBI_H__
#define BSETTOP_VBI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=****************************************
The vbi api supports close captioning, teletext and other formats.
You can decode and capture vbi from an analog or digital stream.
You can route the vbi information out to certain analog displays (e.g. composite and
svideo)
or capture it as data for use by the application.

After getting a new stream (either from btuner_tune_XXX or from bstream_open), all
vbi outputs default to off. You must explicitly turn each output by assigning
display handles to the bstream_vbi_settings structure.

The reason we default vbi to off is that the settop api cannot guess the correct
output in certain configurations. Consider a PIP system. You have two decodes and
only one can output vbi at a time. The application must decide.
*******************************************/

/*
Summary:
Settings structure to control decoding, capture and routing of VBI to displays.
*/
typedef struct bstream_vbi_settings {
	struct {
		bdisplay_t display;	/* if NULL, then disabled */
		unsigned line;	/* only applies to PAL */
	} cc;
	struct {
		bdisplay_t display;	/* if NULL, then disabled */
		unsigned lines[10];
	} tt;
	struct {
		bdisplay_t display;	/* if NULL, then disabled */
	} cgms;
	struct {
		bdisplay_t display;	/* if NULL, then disabled */
	} wss;

	bsettop_callback cc_data_ready_callback; /* Callback when notifies the user
		when data is available in the bstream_vbi_cc_read function. 
		It is permissible to call bstream_vbi_cc_read from inside the callback. */
	void *callback_context;	/* User defined context which is passed to
		the above callback. */
} bstream_vbi_settings;

/*
Summary:
Get current vbi settings.
*/
void bstream_vbi_get_settings(
	bstream_t stream, 					/* analog or digital stream */
	bstream_vbi_settings *settings	/* [out] */
	);

/*
Summary:
Set new vbi settings.
*/
bresult bstream_vbi_set_settings(
	bstream_t stream, 					/* analog or digital stream */
	const bstream_vbi_settings *settings	/* new settings */
	);

/*
Summary:
VBI closed captioning data atom
*/
typedef struct bstream_vbi_cc {
	uint8_t data[2];
	int field;	/* 0 is top, 1 is bottom */
	int cc_type;	/* 0/1 means 608, 2/3 means 708 */
} bstream_vbi_cc;

/*
Summary:
Read vbi closed caption data from a stream.

Description:
This function never blocks. Use the bstream_vbi_settings.cc_data_ready_callback 
callback to know when data is available.
*/
bresult bstream_vbi_cc_read(
	bstream_t stream,  			/* analog or digital stream */
	bstream_vbi_cc *data,		/* [out,size_is(length)] */
	size_t length,				/* # of bstream_vbi_cc's pointed to by data param */
	size_t *length_read			/* [out] actual number of bstream_vbi_cc's read into data buffer */
	);

/*
Summary:
CC708 VBI closed captioning data atom
*/
typedef struct bstream_cc_708 {
	uint8_t cc_type;	/* 0/1 means 608, 2/3 means 708 */
	uint8_t data[2];
} bstream_cc_708;

/*
Summary:
Read CC708 vbi closed caption data from a stream.

Description:
This function never blocks. Use the bstream_vbi_settings.cc_data_ready_callback 
callback to know when data is available.
*/
bresult bstream_vbi_cc708_read(
	bstream_t stream,  			/* analog or digital stream */
	bstream_cc_708 *data,		/* [out,size_is(length)] */
	size_t length,				/* # of bstream_vbi_cc's pointed to by data param */
	size_t *length_read			/* [out] actual number of bstream_vbi_cc's read into data buffer */
	);

/*
Summary:
Write vbi closed caption data for output to the VEC

Description:
This function never blocks. There is no callback to know when space
is available, but in general one data item is consumed every field.
*/
bresult bdisplay_vbi_cc_write(
	bdisplay_t display,			/* display (must be SD format) */
	const bstream_vbi_cc *data,	/* [size_is(length)] */
	size_t length,				/* # of bstream_vbi_cc's pointed to by data param */
	size_t *length_written		/* The actual number of bstream_vbi_cc's writen
									into the display queue. This is used for flow
									control. */
	);

/*
Summary
VBI teletext data
*/
typedef struct bstream_vbi_tt {
	uint8_t framingCode;
	uint8_t data[42];
}bstream_vbi_tt;

/*
Summary:
Read vbi teletext data from a stream.

Description:
This function never blocks. 
*/
bresult bstream_vbi_tt_read(
	bstream_t stream,  			/* analog or digital stream */
	bstream_vbi_tt *data,		/* [out,size_is(length)] */
	size_t length,				/* # of bstream_vbi_cc's pointed to by data param */
	size_t *length_read			/* [out] actual number of bstream_vbi_cc's read into data buffer */
	);

/*
Summary:
Write vbi teletext data for output to the VEC

Description:
This function never blocks. There is no callback to know when space
is available, but in general one data item is consumed every field. But,
if the function returns an error, for flowcontrol, the line is not consumed
and the user should retry.
*/


bresult bdisplay_vbi_tt_write(
	bdisplay_t display,			/* display (must be SD format) */
	const bstream_vbi_tt *data,	/* [size_is(length)] */
	size_t length,				/* # of bstream_vbi_cc's pointed to by data param */
    uint8_t polarity             /* 0 = bottom , 1= top field */
	);

#define BDISPLAY_VBI_INVALID_CGMS (0xffffffff) /* use to disable CGMS output */

/**
Summary:
Set the CGMS value to be encoded into display.

Description:
This only works if the display is SD and has the VBI encoder.
On dual output systems, this is usually the SD-only output.

This requires that bstream_vbi_settings.cgms.display is false. Otherwise
the value will be overwritten by the stream input. Use cgms_value of
BDISPLAY_VBI_INVALID_CGMS to disable CGMS output.
**/
bresult bdisplay_vbi_set_cgms(
	bdisplay_t display,			/* display (must be SD format) */
	uint32_t cgms_value			/* CGMS value to pass out VBI */
	);

#define BDISPLAY_VBI_INVALID_WSS (0xffff) /* use to disable WSS output */

/**
Summary:
Set the WSS value to be encoded into display.

Description:
This only works if the display is SD and has the VBI encoder.
On dual output systems, this is usually the SD-only output.

This requires that bstream_vbi_settings.wss.display is false. Otherwise
the value will be overwritten by the stream input.  Use wss_value of
BDISPLAY_VBI_INVALID_WSS to disable WSS output.
**/
bresult bdisplay_vbi_set_wss(
	bdisplay_t display,			/* display (must be SD format) */
	uint16_t wss_value			/* WSS value to pass out VBI */
	);

/**
Summary:
Determine if the CGMS value is being encoded into display.

Description:
This only works if the display is SD and has the VBI encoder.
On dual output systems, this is usually the SD-only output.

This requires that bstream_vbi_settings.cgms.display is false.
**/
bresult bdisplay_vbi_get_cgms(
	bdisplay_t display,			/* display (must be SD format) */
	bool *enabled
	);

/**
Summary:
Determine if WSS value is being encoded into display.

Description:
This only works if the display is SD and has the VBI encoder.
On dual output systems, this is usually the SD-only output.

This requires that bstream_vbi_settings.wss.display is false.
**/
bresult bdisplay_vbi_get_wss(
	bdisplay_t display,			/* display (must be SD format) */
	bool *enabled
	);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_VBI_H__ */
