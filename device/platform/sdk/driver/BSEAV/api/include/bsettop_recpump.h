/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_recpump.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 11/9/07 9:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_recpump.h $
 * 
 * 8   11/9/07 9:25a vsilyaev
 * PR 34612: Added packet_size configuration
 * 
 * 7   12/4/06 5:21p jrubio
 * PR25213: PR25927 Add new recpump_immediate_stop that completely stops
 * 
 * 
 * 
 * 
 * PR25213: PR25927 Add new recpump_immediate_stop that completely stops
 * * all recpump from calling client callbacks
 * 
 * 6   12/1/06 4:22p jrubio
 * PR25213: PR25927 Add new recpump_immediate_stop that completely stops
 * all recpump from calling client callbacks
 * 
 * 5   7/12/06 3:18p ahulse
 * PR16324: get playback / record time via call to bplayback_get_status /
 * brecord_get_status
 * 
 * 4   6/6/06 7:19p ahulse
 * PR20917: Add MCARD 3DES decryption, using XPT TS_DMA/PB Engines, to
 * BCM7038 SW
 * 
 * 3   6/9/05 3:24p erickson
 * PR15513: all pass record is now audio pid 0 or 0x1fff. large warnings
 * now printed.
 * 
 * 2   5/4/05 4:59p erickson
 * PR14576: added num_descriptors and extended comments
 * 
 * 1   2/7/05 7:06p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   11/23/04 8:12p vsilyaev
 * PR 13351: Refactoring playback API.
 * 
 * Irvine_BSEAVSW_Devel/5   6/29/04 11:29a erickson
 * PR11135: updated deprecation notices for settop api
 * 
 * Irvine_BSEAVSW_Devel/4   6/24/04 10:27a erickson
 * PR11135: converted to generic bsettop_callback for playpump and recpump
 *
 * Irvine_BSEAVSW_Devel/3   6/17/04 9:37a erickson
 * PR11135: recpump callback enabled
 *
 * Irvine_BSEAVSW_Devel/2   6/10/04 8:08a erickson
 * PR11135: brecpump implemented and brecord converted to use recpump
 *
 * Irvine_BSEAVSW_Devel/1   6/8/04 7:40a erickson
 * PR11135: added iniital playpump impl for 97038
 *
 *******************************************************************************/
#ifndef BSETTOP_RECPUMP_H__
#define BSETTOP_RECPUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=****************************************
brecpump is a low-level PVR api for recording.
It does not perform any file i/o and does not maintain any threads,
but uses asynchronous callbacks to send data out to the application.

The brecord api is built on top of brecpump. It performs file i/o
and threading.
*******************************************/

/**
Summary:
Handle for brecpump api.
**/
typedef struct brecpump *brecpump_t;

/**
Summary:
Parameters that must be passed into brecpump_open.
**/
typedef struct brecpump_open_params {
	struct {
		size_t buffer_size; /* size in bytes of the record buffer */
		unsigned alignment; /* alignment for the block. units of 2^alignment. Therefore a value of 2 will be 
			4 byte aligned, 3 will be 8 byte aligned, etc. only used for data, not scode. */
		unsigned num_descriptors; /* buffer_size/num_descriptors determines the size of the blocks that
			you will receive from recpump. */
		void * buf;	/* pass in ptr to buffer to be used instead of using own */
	} data, scode;
} brecpump_open_params;

/**
Summary:
Returns default configuration parameters for the record pump 
**/
void brecpump_get_open_params(
		bobject_t id, 
		brecpump_open_params *open_params /* [out] */
		);

/**
Summary:
Open a recpump source.
**/
brecpump_t brecpump_open(
		bobject_t id, 
		const brecpump_open_params *open_params /* optional parameters for the record pump */
		);

/**
Summary:
Close the recpump.
Description:
It should already be stopped.
**/
void brecpump_close(brecpump_t recpump);

/**
Summary:
Parameters that must be passed into brecpump_start.
**/
typedef struct brecpump_params {
	bencryption_params encryption; /* PVR encryption parameters to be applied to
								the recorded data. */
	bool timestamp_enabled;	/* If true, then a 4 byte timestamp is prepended to every
								transport packet. bplayback_params.timestamp_enabled
								must match. */
	void *callback_context;	/* Context pointer which is returned in the callback functions. */
	bsettop_callback data_write_callback; /* Callback function that will be called for
								MPEG data. You should call brecpump_data_get_buffer
								but the actual file write and brecpump_data_write_complete
								call should not be made from this callback. */
	bsettop_callback index_write_callback; /* Optional callback function that will be called
								for SCT (start code table) data. You can set this to be
								NULL if you don't want index data produced. */
	bsettop_callback overflow;	/* Called whenever a buffer overflow is detected.
								Recorded data will be corrupt. This is used for system
								debug only. In a production system this should never
								happen, and if it does, you should just keep on recording. */

	size_t packet_size;         /* the size of the blocks that you will receive from recpump would be multiplier of packet_size, please also see 
                                    brecpump_open_params  for buffer_size and num_descriptors */
} brecpump_params;

/**
Summary:
Initialize the recpump parameters to defaults.

Description:
You will need to set at the very least the data_write_callback.
**/
void brecpump_params_init(
	brecpump_params *params, /* [out] */
	brecpump_t recpump
	);

/**
Summary:
Start recording a stream.

Description:
To perform an "all pass" record of an entire transport stream, create a stream with video[0].pid and 
audio[0].pid of 0. This is only supported on magnum-based platforms and has possibly undesired 
effects on all records, so only use this as a test mode.
**/
bresult brecpump_start(
	brecpump_t recpump,
	bstream_t stream,
	const brecpump_params *params
	);

/**
Summary:
Stop the recording and wait for the recording process to finalize.
The recpump handle can be safely closed after brecpump_stop returns.
**/
bresult brecpump_stop(
	brecpump_t recpump
	);


/**
Summary:
 Please call before you call brecpump_stop if you have
stopped abnormally. This will ensure that when you stop
you will not receive any more call backs and data
**/
void brecpump_immediate_stop(
	brecpump_t recpump
	);

/**
Summary:
Get next portion of MPEG data from the record buffer that needs to be consumed from transport.

Description:
This returns the same type of information as the write_callback.
Note that after receiving the callback, if you call brecpump_data_get_buffer again,
there may be more data that becomes available.
**/
bresult brecpump_data_get_buffer(
	brecpump_t recpump,
	const void **buffer, /* [out] memory which contains data to be written */
	size_t *buffer_size /* [out] total amount of data to be written */
	);

/**
Summary:
Get next portion of SCT index data from the record buffer that needs to be consumed.
Description:
This returns the same type of information as the write_callback.
Note that after receiving the callback, if you call brecpump_index_get_buffer again,
there may be more data that becomes available.
**/
bresult brecpump_index_get_buffer(
	brecpump_t recpump,
	const void **buffer, /* [out] memory which contains data to be written */
	size_t *buffer_size /* [out] total amount of data to be written */
	);

/**
Summary:
Report back how much MPEG data was consumed (usually by writing to disk).

Description:
If the write fails, the user has the option of calling brecpump_write_complete
or stopping pvr altogether. There is no way and no point in information brecpump
that a write has failed.
**/
bresult brecpump_data_write_complete(
	brecpump_t recpump,
	size_t amount_written
	);

/**
Summary:
Report back how much index data was consumed (usually by writing to disk).

Description:
If the write fails, the user has the option of calling brecpump_write_complete
or stopping pvr altogether. There is no way and no point in information brecpump
that a write has failed.
**/
bresult brecpump_index_write_complete(
	brecpump_t recpump,
	size_t amount_written
	);

/**
Summary:
Status returned by brecord_get_status.
**/
typedef struct brecpump_status {
	unsigned long mpeg_bytes_recorded;	/* Number of bytes of mpeg stream data recorded */
	unsigned long mpeg_fifo_depth;	/* Current depth in bytes of the mpeg record buffer */
	unsigned long mpeg_fifo_size;	/* Current size in bytes of the mpeg record buffer */
	bool has_index;	/* If true, then you are recording an index and the following fields
						are valid. */
	unsigned long index_bytes_recorded;	/* Number of bytes of index data recorded */
	unsigned long index_fifo_depth;	/* Current depth in bytes of the index record buffer */
	unsigned long index_fifo_size;	/* Current size in bytes of the index record buffer */
	unsigned long last_timestamp;	/* timestamp of last frame recorded. Not used by recpump, */
					/* but included for use in brecord_status -time in mSec */
} brecpump_status;

/**
Summary:
Get current status of the recording.
**/
void brecpump_get_status(
	brecpump_t recpump,
	brecpump_status *status	/* [out] current status information */
	);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_RECPUMP_H__ */
