/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_message.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 2/27/08 5:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_message.h $
 * 
 * 9   2/27/08 5:38p vsilyaev
 * PR 32470: clarified use of bmessage_format_pes
 * 
 * 8   1/18/07 11:36a vsilyaev
 * PR 27186: Differentiate between parser and input bands. AKA, fixing
 * bmessage from playback source
 * 
 * 7   11/20/06 3:12p ahulse
 * PR25212: Add psf_crc_disabled to bmessage_stream_params, so crc
 * checking can be en/disabled on Private short form sections
 * 
 * 6   8/12/05 3:30p erickson
 * PR16659: added bmessage_stream_params.filter_group and reworked
 * bmessage to handle wrapping correctly. now I'm copying a whole message
 * into a intermediate buffer. this greatly simplifies the logic and
 * removes the possibility of overwrite.
 * 
 * 5   7/11/05 3:00p erickson
 * PR15064: added bmessage_stream_params.psi_crc_disabled, which defaults
 * false
 * 
 * 4   7/11/05 2:27p erickson
 * PR16044: updated documentation for bmessage
 * 
 * 3   3/15/05 2:53p erickson
 * PR13415: added notes on bmessage_format types
 * 
 * 2   3/11/05 6:19p erickson
 * PR13415: added support for multiple bmessage_stream_t's on the same
 * band/pid using FilterGroups
 * 
 * 1   2/7/05 7:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/32   8/10/04 4:23p erickson
 * PR12152: added buffer_size
 * 
 * Irvine_BSEAVSW_Devel/31   7/27/04 3:11p erickson
 * PR11771: doc changes
 * 
 * Irvine_BSEAVSW_Devel/30   7/9/04 1:34p erickson
 * PR11771: removed size_is hint because it's not an array
 * 
 * Irvine_BSEAVSW_Devel/29   7/6/04 3:53p erickson
 * PR11771: settop api dataflow redesign
 * 
 * Irvine_BSEAVSW_Devel/28   6/29/04 11:28a erickson
 * PR11135: updated deprecation notices for settop api
 *
 * Irvine_BSEAVSW_Devel/27   5/25/04 11:43a erickson
 * PR11135: clarified usage of bmessage_Read
 *
 * Irvine_BSEAVSW_Devel/26   12/9/03 12:14p erickson
 * PR8892: added a bit more clarification
 *
 * Irvine_BSEAVSW_Devel/25   12/8/03 5:18p marcusk
 * PR 8892: Furthur clarified documentation.
 *
 * Irvine_BSEAVSW_Devel/24   12/8/03 5:06p marcusk
 * PR 8892: Updated documention regarding bmessage_filter usage.
 *
 * Irvine_BSEAVSW_Devel/23   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 *
 * Irvine_BSEAVSW_Devel/22   11/5/03 5:07p erickson
 * PR8560: moved dss code back. this isn't the right solution.
 *
 ***************************************************************************/
#ifndef BSETTOP_MESSAGE_H__
#define BSETTOP_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=****************************
The message api allows you to collect transport messages from a variety of
data sources, including MPEG2 transport streams.

We provide higher level libraries to parse PSI or PSIP message
formats. Please refer to BSEAV/lib/mpeg2_ts_parse and BSEAV/linux/lib/tspsi.
*******************************/

/*
Summary:
	Handle returned by bmessage_open which represents one PID parser channel.
*/
typedef struct bmessage_struct *bmessage_stream_t;

/*
Summary:
Format of data which you're parsing.
Description:
This maps to eSingleChannelRecordType in bcmtranshw.h.
*/
typedef enum bmessage_format {
	bmessage_format_ts,  /* capture all transport packets in the pid, no filtering. transport header are captured. */
	bmessage_format_pes, /* capture all PES packets (i.e. all transport payload) in the pid, no filtering. transport headers are stripped. */
	bmessage_format_psi, /* capture PSI message using filter */
	bmessage_format_dss_mpt, /* capture DSS MPT messages using filter */
	bmessage_format_dss_aux, /* capture DSS AUX messages using filter */
	bmessage_format_dss_regular /* capture DSS packets, no filtering (?) */
} bmessage_format;

/*
Summary:
Open a message stream for a particular format of data.

Description:
Message data does not start being collected until you call bmessage_start.
*/
bmessage_stream_t bmessage_open(
	bmessage_format format
	);

/*
Summary:
PID filtering structure passed to bmessage_open.
Description:
All filtering is done using bit by bit comparisons.

There are two types of masks that are used in conjuntion with the coefs.

inclusion mask (mask) - for all inclusion mask bits that are unmasked
		(set to 0) ALL of the corresponding bits in the
		message must match the bits specified by the coef in order to
				be included. If one bit differs, it will be excluded.

exclusion mask (excl) - for all exclusion mask bits that are unmasked
		(set to 0) at least one of the corresponding bits
		in the message must NOT match the bits specified
		by the coef in order to be included. If all of the bits
				match, it will be excluded.

Results from both comparisons are AND'd together to form the final pass/fail decision.

Exception: Currently for MPEG2 streams, byte 2 (i.e. the message length field)
of the filter and the MPEG2 message data is ignored.
*/
typedef struct bmessage_filter {
	uint8_t mask[16];	/* Bits to be ignored. See Description. */
	uint8_t coef[16];	/* Bits to be included. See Description. */
	uint8_t excl[16];	/* Bits to be excluded. Doesn't apply to Venom2 platforms. See Description. */
} bmessage_filter;

/**
Summary:
Parameters for bmessage_start
**/
typedef struct bmessage_stream_params {
	bband_t band;				/* The input band used to filter. Filtering actually happens on the parser band, but 
									this is mapped internally.
									This setting is mutually exclusive with bmessage_stream_params.stream. */
	bstream_t stream; /* This is the playback stream to filter. It is required that the playback already be routed
					to a parser band, otherwise this function will fail. On RAVE systems, playback always has a
					dedicated parser band. On non-RAVE systems, you must set
						bplayback_params.route_through_parser_band = true.
					This setting is mutually exclusive with bmessage_stream_params.bnd. */
	uint16_t pid;				/* the pid to filter */
	bmessage_filter filter;		/* the filter to apply */
	int buffer_size;			/* In kilobytes. Default is 4K. Ranges between
									1K and 512K in powers of 2. */
	bool filter_group;			/* Defaults to false. 
		You can expand the number of message filters you use by using filter groups.
		A filter group is a set of filters that share the same band and pid. All filters in the group
		share the same pid channel. The downside is that all messages for a pid channel arrive in the
		same buffer, and an additional software filter compare must be used to sort out who is gets the message.
		Also, messages that wrap around the end of the buffer must be copied into an intermediate buffer.
		If a filter group is not enabled, you will get higher throughput from bmessage because there
		will be no copies or software filter comparisons, but you will be limited by the number of pid channels available. */

	/* NOTE: psi_crc_disabled and psf_crc_disabled and can be used in the following  */
	/* combinations to give the following functionality :                            */
	/*                                                                               */
	/* AUTO Mode  : CRC check is determined by the short/long form private section   */
	/*              if short form (section_syntax_indicator=0) no crc is checked     */
	/*              if long form  (section_syntax_indicator=1) crc is needed         */
	/* enable CRC : CRC enabled  for both short and long form private section.       */
	/* disable CRC: CRC disabled for both short and long form private section.       */
	/*                                                                               */
	/* AUTO       : psf_crc_disabled = true , psi_crc_disabled = false               */
	/* enable CRC : psf_crc_disabled = false, psi_crc_disabled = false               */
	/* disable CRC: psf_crc_disabled = true,  psi_crc_disabled = true                */

	bool psi_crc_disabled;		/* disable CRC check of PSI message data */
	bsettop_callback data_ready_callback;	/* Optional callback to notify the application
		when bmessage_stream_get_buffer will return more data to be read. 
		When you get a callback, additional callbacks will be disabled until you
		call bmessage_get_buffer.
		
		If you are using multiple bmessage_streams on the same band & pid,
		then the behavior will change slightly. You may receive callbacks
		and yet get no data when calling bmessage_get_buffer. This still falls
		within specified behavior, and is necessary to make the internal
		algorithm work.
		*/
	bsettop_callback overflow;	/* Optional callback to notify the user when
									an overflow has occured. */
	void *callback_context;		/* User defined context which will be passed to
									the above callback functions. */
	bool psf_crc_disabled;		/* disable Private Short Form CRC check of PSI data */
} bmessage_stream_params;

void bmessage_stream_params_init(
	bmessage_stream_params *params,
	bmessage_stream_t stream);

/*
Summary:
Start filtering a message stream for a particular band and pid.

Description:
You can have multiple message streams started at the same time for the same band.
This technique makes message processing faster. The limit of message streams
depends on hardware.
*/
bresult bmessage_start(
	bmessage_stream_t message_stream,
	const bmessage_stream_params *params
	);

/*
Summary:
Get pointer to data available to be read from the transport message buffer.

Description:
The data in the buffer is raw and unparsed.
Depending on your buffer sizes and the amount of data captured, subsequent reads may
return data that starts in the middle of a message. It's up to the application
to join multiple reads together and parse the data.

The transport hardware will return messages with a trailing pad to ensure
4 byte alignment. You'll need to skip over these bytes in your app, however you
must include these bytes in your count for the bmessage_read_complete.

This function is always non-blocking.
*/
bresult bmessage_get_buffer(
	bmessage_stream_t message_stream, /* message stream handle returned by bmessage_open */
	const void ** p_bfr, 			/* [out] memory to read into */
	size_t *length
	);

/**
Summary:
After processing bmessage_get_buffer, use this function to report back how much
data was consumed.

Description:
You should never call bmessage_read_complete twice in a row without calling
bmessage_get_buffer again.

The amount_consumed can never be greater than the length last returned by
bmessage_get_buffer.
**/
bresult bmessage_read_complete(
	bmessage_stream_t message_stream, /* message stream handle returned by bmessage_open */
	size_t amount_consumed
	);

/*
Summary:
Stop any more data from being filtered.

Description:
You cannot call bmessage_get_buffer after this.
You can call bmessage_start to start again, or bmessage_close to clean up.
*/
void bmessage_stop(
	bmessage_stream_t message_stream /* message stream handle returned by bmessage_open */
	);

/*
Summary:
Close a message stream.

Description:
If the message stream was not already stopped, it will be.
The bmessage_stream_t handle is no longer valid.
*/
void bmessage_close(
	bmessage_stream_t message_stream /* message stream handle returned by bmessage_open */
	);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_MESSAGE_H__ */
