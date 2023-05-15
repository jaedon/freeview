/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_fileio_fifo.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 12/7/07 4:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_fileio_fifo.h $
 * 
 * 6   12/7/07 4:24p vsilyaev
 * PR 37037: Improved documentation for fifo file
 *
 * 5   4/12/06 2:16p vsilyaev
 * PR 20680: Promoted bsettop_fileio_fifo API
 *
 * 4   12/23/05 4:10p vsilyaev
 * PR 18183: Renamed API functions
 *
 * 2   11/21/05 3:42p vsilyaev
 * PR 18183: Renamed size function to bounds, so it would remain useful
 * with trimmed files
 *
 * 1   11/17/05 4:22p vsilyaev
 * PR 18183: Added circular(FIFO) file
 *
 ***************************************************************************/
#ifndef BSETTOP_FILEIO_FIFO_H__
#define BSETTOP_FILEIO_FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsettop_types.h"
#include "bfile_io.h"

/*
Overview
bfile_out_fifo is a type of record file used for PVR timeshifting.

bfile_out_fifo works by trimming the beginning of a file so that the overall file size remains bounded.
For efficiency reasons, trimming is done in chunks, not in bytes.
Because of variable bit rate complexities, bounding is not done using a simple size number but with set of time and data limits.

The main API is:

    bfile_fifo_out_create - create a new timeshifting buffer for record
    
    bfile_fifo_out_file - create a brecord_file_t mapped to the timeshifting buffer for use in Settop API record
    
    bfile_fifo_in_open - create a bplayback_file_t mapped to the timeshifting buffer for use in Settop API playback
        based on either a live recording or a static file (i.e. terminated recording).

The actual size of the file on disk is not actually fixed. Most applications require a timeshifting buffer which is bounded by a specified time. 
Because video bit rates vary, the actual amount of data needed to achieve this time will vary.
bfile_out_fifo employs three parameters to determine limits:

    interval - desired accessible size of buffer, measured in units of seconds
        see bfile_out_settings for details

    soft limit - minimum size of sustained buffer (either data or index), measured  in units of bytes
        see bfile_out_limit for details

    hard limit - maximum size of buffer (either data or index), measured  in units of bytes
        see bfile_out_limit for details
        
After the bfile_out_fifo is closed (i.e. the recording is terminated), the file can be reopened as
a static playback file. See bfile_fifo_in_open for details.

If your application wants to convert a time-limited timeshifting buffer into a disk-limited permanent recording,
it can simply update the bfile_out_settings to enormous values. The recording can then continue until it 
fails because of a full disk. Here's an example:

    bfile_out_settings settings;
    bfile_out_get(file, &settings);
    settings.interval = 356 * 24 * 60 * 60; // 1 year
    settings.data.soft = 1024*1024*1024*1024; // 1000 GB
    settings.data.hard = settings.data.soft;
    settings.index.soft = settings.data.soft;
    settings.index.hard = settings.data.soft;
    bfile_out_set(file, &settings);
*/

/*
Summary:
Handle used for timeshifting buffer
*/
typedef struct bfile_out_fifo *bfile_out_fifo_t;

/*
Summary:
Configuration parameters for the timeshifting buffer

Description:
These numbers are used to limit the size of the timeshifting buffer on the disk.

The soft limit is the targetted file size.
A sustained continuous record will not be less than "soft".
You can terminate a record before it reaches the soft limit, but if allow it to continue (i.e. sustained), it will eventually reach the soft limit.

In certain cases (e.g. if the bitrate of the recorded stream suddenly increases) bfile_out_fifo may
exceed the soft limit. However, it will never exceed the hard limit.
Once the hard limit is reached, an overflow error occurs.

Choosing an appropriate hard and soft limit should be as follows:

1. Choose the desired interval based on your application (e.g. 1 hour, 2 hours).

    // 1 hour = 3600 seconds
    bfile_out_settings.interval = 60 * 60;

2. For the data soft limit, multiply the interval by the expected max bitrate for your system:

    // for 20 Mbps
    bfile_out_settings.data.soft = bfile_out_settings.interval * (20*1024*1024/8);

3. For the data hard limit, increase from the soft limit by a margin no less than 50% of the soft limit.

    // 100% margin
    bfile_out_settings.data.hard = bfile_out_settings.data.soft * 2;

4. For the index soft limit, multiply the interval by the max size of a NAV entry (for BNAV_AVC_Entry, it's 64 bytes)
    and the max frame rate of the stream (e.g. 60 fps).

    // 64 byte NAV, 60fps, and some margin
    bfile_out_settings.index.soft = bfile_out_settings.interval * 64 * 60 * 2;

5. For the index hard limit, increase from the soft limit by a margin no less than 200% of the soft limit.
   The index buffer is small, so there's a very small cost to guarantee no index overflow.

    bfile_out_settings.index.hard = bfile_out_settings.index.soft * 2;

See bfile_out_settings for how bfile_out_limit is used for the data and index files.
*/
typedef struct bfile_out_limit {
    off_t soft; /* Targetted file size in bytes. */
    off_t hard; /* Maximum file size in bytes. */
} bfile_out_limit;

/**
Summary:
This value is used to indicate if timestamp or index position is invalid
**/
#define BPVR_INVALID_POSITION ((unsigned long)-1)

/**
Summary:
bpvr_position provides several types of information about a position in the timeshifting buffer

See also: bfile_fifo_out_position
**/
typedef struct bpvr_position {
    off_t mpeg_file_offset;     /*  Byte offset in the data file. (called "mpeg" for legacy reasons. it applies to any codec.) */
    unsigned long index_offset; /*  Offset of the current index entry in the index.
                                    This field is set to BPVR_INVALID_POSITION, if the index is invalid */

    unsigned long timestamp;    /*  Timestamp of the current index entry in the index.
                                    Timestamp is different from PTS. Timestamp is generated
                                    by bcmindexer, it begins at 0 for any recorded stream,
                                    and is guaranteed to be continuous increasing throughout
                                    the stream. The units of the timestamp depend on the
                                    implementation of bcmindexer, but they are defaulted
                                    to milliseconds.
                                    This field is set to BPVR_INVALID_POSITION, if the index is invalid */
} bpvr_position;

/*
Summary:
Configuration parameters for the timeshifting buffer
*/
typedef struct bfile_out_settings {
    unsigned interval;     /* Amount of time accessible to application in the timeshifting buffer, units are seconds.
                              Must set data and index soft/hard limits that coordinate with this interval. See bfile_out_limit. */
    bfile_out_limit data;  /* Limits for the timeshifting data file */
    bfile_out_limit index; /* Limits for the timeshifting index file */
} bfile_out_settings;

/*
Summary:
This function opens a new timeshifting buffer

Description:
The bfile_out_fifo_t can be used for only one record at a time.
The files will be created if they don't already exist and truncated if they do.
The index file is mandatory for timeshifting.
*/

bfile_out_fifo_t bfile_fifo_out_create(
    const char *data_file_name, /* name of data file to create */
    const char *index_file_name /* name of index file to create */
    );
    
/*
Summary:
This function returns position information about the first and last
picture in the timeshifting buffer
*/
bresult bfile_fifo_out_position(
    bfile_out_fifo_t file,  /* Handle created by bfile_fifo_out_create() */
    bpvr_position *first,   /* [out] first accessible picture in the file */
    bpvr_position *last     /* [out] last accessible picture in the file */
    );

/*
Summary:
This function returns a handle to be passed to brecord_start().
*/
brecord_file_t bfile_fifo_out_file(
    bfile_out_fifo_t file /* Handle created by bfile_fifo_out_create() */
    );

/*
Summary:
This function returns a handle to be passed to bplayback_start().

Description:
This function shall be used only for files created by bfile_fifo_out_create().
The bplayback_file_t can be used for only one playback at a time.

All the metadata needed to manage the timeshifting buffer is persisted to disk. This means you can close the 
buffer bfile_out_fifo (i.e. stop recording) and at a later time reopen a bplayback_file_t for non-timeshifting playback.
Pass a NULL to file for non-timeshifting playback.

If bfile_out_fifo_t is currently open, you must not open the data and index file with a NULL as file. 
The metadata is being updated during record and an unlinked playback will get corrupted.
*/
bplayback_file_t bfile_fifo_in_open(
    const char *data_file_name,
    const char *index_file_name,
    bfile_out_fifo_t file /* Live recording which will be timeshifted. 
                             Pass NULL only if this is not a live timeshift. */
    );

/*
Summary:
Get the current configuration of the timeshifting buffer
*/
void
bfile_fifo_out_get(
    bfile_out_fifo_t file,       /* Handle created by bfile_fifo_out_create() */
    bfile_out_settings *settings /* [out] current settings of timeshifting buffer */
    );

/*
Summary:
Set a new configuration for the timeshifting buffer
*/
bresult
bfile_fifo_out_set(
    bfile_out_fifo_t file,             /* Handle created by bfile_fifo_out_create() */
    const bfile_out_settings *settings /* new settings of timeshifting buffer */
    );


struct bfile_io_write_fifo;
struct bfile_io_read_fifo;


/* low level functions */
struct bfile_io_write_fifo *bpvrfifo_write_open(const char *fname, int flags, off_t start);
bfile_io_write_t bpvrfifo_write_file(struct bfile_io_write_fifo *file);
void bpvrfifo_write_close(struct bfile_io_write_fifo *file);
struct bfile_io_read_fifo *bpvrfifo_read_open(const char *fname, int flags, struct bfile_io_write_fifo *writer);
bfile_io_read_t bpvrfifo_read_file(struct bfile_io_read_fifo *file);
void bpvrfifo_read_close(struct bfile_io_read_fifo *file);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_FILEIO_FIFO_H__ */

