/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bcmindexer.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 2/27/12 4:13p $
 *
 * Module Description: Broadcom Index Player
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/include/bcmindexer.h $
 * 
 * 9   2/27/12 4:13p erickson
 * SW7425-2495: move BNAV_Entry encode/decode to public API for use with
 * BNAV_Player_ReadIndex
 * 
 * 8   1/3/12 12:33p erickson
 * SW7231-578: add BNAV_Indexer_Status.parsingErrors
 * 
 * 7   10/19/10 12:55p vsilyaev
 * SW7422-14: Added interface for H.264 SVC/MVC indexing
 * 
 * 6   6/22/10 11:13a erickson
 * SW7405-4465: add BNAV_Indexer_Settings.ptsBasedFrameRate for PTS-based
 * offline indexing
 *
 * 5   6/2/10 12:51p erickson
 * SW7400-2789: add BNAV_Indexer_Settings.allowLargeTimeGaps
 *
 * 4   1/7/09 9:51p erickson
 * PR49501: added append settings
 *
 * 3   2/26/08 12:28p katrep
 * PR38429: Added AVS record and playback support
 *
 * 2   12/27/07 3:27p erickson
 * PR37035: changed simulatedFrameRate to unsigned
 *
 * Irvine_BSEAVSW_Devel/35   3/6/06 1:20p erickson
 * PR19853: added VC1 PES support
 *
 * Irvine_BSEAVSW_Devel/34   1/6/06 10:17a erickson
 * PR17108: fix for 64 bit architectures
 *
 * Irvine_BSEAVSW_Devel/33   7/13/05 2:11p erickson
 * PR16138: added AVC NAV version
 *
 * Irvine_BSEAVSW_Devel/32   3/18/05 9:37a erickson
 * PR14451: moved BSCT_Entry to common file, added AVC support and 6 word
 * SCT support
 *
 * Irvine_BSEAVSW_Devel/31   12/14/04 4:54p marcusk
 * PR13560: Updated to default to frameratesimulation to 0.  Command line
 * indexer updated to accept frame rate setting.
 *
 * Irvine_BSEAVSW_Devel/30   4/13/04 4:54p erickson
 * PR10292: added BNAV_Indexer_FeedReverse
 *
 * Irvine_BSEAVSW_Devel/29   12/2/03 12:05a erickson
 * PR8807: updated docs
 *
 * Irvine_BSEAVSW_Devel/28   11/10/03 2:29p erickson
 * PR8563: added transport timestamp support to bcmplayer
 *
 * Irvine_BSEAVSW_Devel/27   6/24/03 11:03a erickson
 * PR7218 - handled partially encrypted streams. Added maxFrameSize and
 * mpegSizeCallback. Had to change the
 * NAV table format (version 2) in order to handle I frames with reference
 * offset of 0. Bcmplayer
 * is backward compatible.
 *
 * Irvine_BSEAVSW_Devel/26   6/19/03 9:04a erickson
 * added BNAV_Indexer_GetPosition (PR7211)
 *
 * Irvine_BSEAVSW_Devel/25   3/31/03 2:42p erickson
 * updated documentation (further updates will come when DocJet is more
 * available)
 *
 * Irvine_BSEAVSW_Devel/24   2/14/03 1:57p erickson
 * New naming convention
 * Fixed looping from previous rework
 * Removed bcmindexer _getParam/_setParam
 * Did not change code shared between tsplayer and bcmplayer
 * Refactored settings, playmode and other structures
 *
 * Irvine_HDDemo_Devel/23   9/26/02 3:4p erickson
 * Changed bcmindexer vchip from 3 bits to 16 bits packed into 12 bits.
 * The index is NOT backward compatible, but the 3 bit version had only
 * been released for 1 day.
 *
 * Irvine_HDDemo_Devel/22   9/10/02 2:2p erickson
 * changed vchip from 16 to 3 bits
 *
 * Irvine_HDDemo_Devel/21   9/10/02 9:5a erickson
 * updated comments
 *
 * Irvine_HDDemo_Devel/20   9/9/02 11:51a erickson
 * added vchip support
 *
 * Irvine_HDDemo_Devel/19   8/12/02 10:21a erickson
 * a couple small changes requested by customer
 *
 * Irvine_HDDemo_Devel\18   4/17/02 4:20p erickson
 * Removed simulateFramerate from allocate()
 * Added setParam and simulateFramerate enum
 *
 * Irvine_HDDemo_Devel\17   4/17/02 3:27p erickson
 * Removed simulatedFramerate from allocate
 *
 * Irvine_HDDemo_Devel\16   4/17/02 1:14p erickson
 * Made sBcmIndexEntry public
 * Added simulatedFramerate because of timestamping
 *
 * Irvine_HDDemo_Devel\15   4/16/02 12:51p erickson
 * Changed initial comments.
 * Added reset function
 * Added version to allocate/reset
 *
 *
 ***************************************************************************/
#ifndef BCMINDEXER_H__
#define BCMINDEXER_H__

#include "bcmplayer_version.h"
#include "bcmsct_index.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* Summary:
* Structure of a Broadcom index entry.
*
* Description:
* It is used internally but is made public to support BNAV_Player_ReadIndex().
**/
typedef struct
{
    uint32_t words[8];    /* Index entry data */
} BNAV_Entry;

/**
* Summary:
* An extension of BNAV_Entry to support AVC indexing.
*
* Description:
* The first 8 words are identical to BNAV_Entry.
**/
typedef struct
{
    uint32_t words[16];    /* Index entry data */
} BNAV_AVC_Entry;

/**
* Summary:
* The size of one BNAV_Entry.
**/
#define BNAV_ENTRY_SIZE     sizeof(BNAV_Entry)  /* in bytes, but should be an even word */

/**
* Summary:
* Opaque handle used to identify one instance of Bcmindexer.
**/
typedef struct BNAV_Indexer_HandleImpl *BNAV_Indexer_Handle;

/**
* Summary:
* Callback function which is used by BNAV_Indexer_Settings.
**/
typedef unsigned long (*BNAV_WRITE_CB)( const void *p_bfr, unsigned long numEntries, unsigned long entrySize, void *fp );

/**
* Summary:
* Optionally used callback for determining the current MPEG file size in order
* to resolve wrapping issues.
*
* Description:
* See BNAV_Indexer_Settings.mpegSizeCallback for details on how and why this is used.
* Should returns 0 on success.
**/
typedef int (*BNAV_MPEG_SIZE_CB) (
    BNAV_Indexer_Handle handle,
    unsigned long *sizeHi,  /* [out] Hi 32 bits of the current MPEG file size */
    unsigned long *sizeLo   /* [out] Lo 32 bits of the current MPEG file size */
    );

/**
Summary:
The type of video being processed.

Description:
These enum values should match bvideo_codec in the Settop API for consistency.
**/
typedef enum BNAV_Indexer_VideoFormat {
    BNAV_Indexer_VideoFormat_MPEG2 = 2,
    BNAV_Indexer_VideoFormat_AVC = 0x1b,
	BNAV_Indexer_VideoFormat_AVC_SVC = 0x1e,
	BNAV_Indexer_VideoFormat_AVC_MVC = 0x1f,
    BNAV_Indexer_VideoFormat_VC1 = 0xea,
    BNAV_Indexer_VideoFormat_AVS =  0x42
} BNAV_Indexer_VideoFormat;

/**
* Summary:
* Settings structure which is passed to BNAV_Indexer_Open.
**/
typedef struct {
    BNAV_WRITE_CB writeCallback;/* Write callback used to output the new index.
                                    You can use fwrite. */
    void *filePointer;          /* File pointer passed to the write callback.
                                    If you use fwrite, this should be FILE*. */
    BSCT_Version sctVersion;    /* Sets the version of the incoming SCT entries. These cannot
                                    be autodetected. */
    BNAV_Version navVersion;        /* Set compatibility version. bcmplayer will auto-detect
                                    the eBcmNavTableVersion when allocated or reset. */
    BNAV_Indexer_VideoFormat videoFormat; /* Defaults to MPEG2 */

    bool ptsBasedFrameRate;         /* If you're using bcmindexer off-line (not during live
                                    recording), this will set the timestamp based on the PTS
                                    in the stream. */
    unsigned simulatedFrameRate;    /* If you're using bcmindexer off-line (not during live
                                    recording), the timestamp needs to set according
                                    to a simulated frame rate. A non-zero simulatedFrameRate enables
                                    this simulation and the value of simulatedFrameRate is used as the rate,
                                    in units of frames per second. Default is 0 (disabled). */
    unsigned long maxFrameSize; /* Any frame larger than this will be assumed to be bad and
                                    discarded. Value is specified in units of bytes.
                                    Default is 5 MB. This addresses the problem of
                                    partially encrypted streams. When a stream becomes
                                    encrypted midway through the record, SCT entries
                                    stop being generated and a huge frame results when the
                                    next SCT entry is fed. When this happens, the huge
                                    frame, all frames up to the next I frame, and
                                    any open GOP B's after the next I frame must be
                                    discarded. */
    BNAV_MPEG_SIZE_CB mpegSizeCallback;
                                /* Optional callback retrieves the size of the MPEG file.
                                    MPEG data is usually recorded in a different thread
                                    than index data, therefore the size is not available.
                                    This makes it available.
                                    The size of MPEG data is needed to determine how many
                                    times the 32 bit SCT frame offset has wrapped when
                                    the MPEG becomes unencrypted after a section of
                                    encryption. During the encrypted section, the 32 bit SCT frame
                                    offset may have wrapped any number of times.
                                    Otherwise there is no way for bcmindexer to know.
                                    Defaults to NULL (no callback). This callback is ignored
                                    for 40 bit SCT frame offset systems because it is assumed
                                    that they will never wrap. */
    int transportTimestampEnabled; /* If true, then transport has prepended a 4 byte
                                    timestamp to every transport packet and this must be
                                    taken into account. */

    /* If these are set, the index is being appended on to another index.
    The offset will be added to every index entry. timestamps will resume from the last timestamp.
    The start of the appended index will be the next random access point. */
    struct {
        unsigned long offsetHi;         /* Hi 32 bits of the MPEG file offset */
        unsigned long offsetLo;         /* Lo 32 bits of the MPEG file offset */
        unsigned long timestamp;        /* Bcmindexer calculated timestamp */
    } append;

    bool allowLargeTimeGaps;       /* By default, any large gap in the recording (e.g. from a disconnected frontend) will cause
                                      the NAV index timestamp to be paused. This means that the gap will not appear in the timestamps on playback.
                                      Set allowLargeTimeGaps to true if you want to disable this feature. Any gap in the recording will
                                      appear in the index timestamps. */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    bool fifoRecord;					/* FIFO record(true) or not(false) */
#endif
} BNAV_Indexer_Settings;

/**
* Summary:
* Must initialize the BNAV_Indexer_Settings before using it and
* passing it to BNAV_Indexer_Open.
**/
void BNAV_Indexer_GetDefaultSettings(
    BNAV_Indexer_Settings *settings     /* [out] */
    );

/**
* Summary:
* Open an instance of Bcmindexer as identified by the handle.
*
* Return values:
* 0 on success
* -1 if the sctVersion or navVersion is not supported.
**/
int BNAV_Indexer_Open(
    BNAV_Indexer_Handle *handle,        /* [out] */
    const BNAV_Indexer_Settings *settings
    );

/**
* Summary:
* Reset Bcmindexer instance to its initial state.
*
* Return values:
* 0 on success
* -1 if the sctVersion or navVersion is not supported.
**/
int BNAV_Indexer_Reset(
    BNAV_Indexer_Handle handle,
    const BNAV_Indexer_Settings *settings
    );

/**
* Summary:
* Free a Bcmindexer instance.
* Description:
* The handle becomes invalid and any further use leads to undefined behavior.
**/
void BNAV_Indexer_Close(
    BNAV_Indexer_Handle handle
    );

/**
* Summary:
* Feed start code table (SCT) data into Bcmindexer.
*
* Description:
* You must send even blocks of size BSCT_Entry or BSCT_SixWord_Entry. All write callbacks
* will happen while BNAV_Indexer_Feed() is executing. This means that there is no
* buffer which gets flushed to the write callback when you call BNAV_Indexer_Close().
*
* Return values:
* -1 on error
* Number of entries processed
**/
long BNAV_Indexer_Feed(
    BNAV_Indexer_Handle handle,
    void *p_bfr,                /* Pointer to start code index data. This could be
                                    either BSCT_Entry's or BSCT_SixWord_Entry's,
                                    depending on the BSCT_Version. */
    long numEntries             /* Number of SCT entries (either 4 or 6 word) pointed
                                    to by p_bfr. */
    );

/**
* Summary:
* Feed PES data into Bcmindexer.
*
* Description:
* No intermediate SCT entries are produced.
* This is only supported for VC1 currently.
*
* Return values:
* -1 on error
* 0 on success
**/
int BNAV_Indexer_FeedPES(
    BNAV_Indexer_Handle handle,
    uint8_t *p_bfr,             /* Pointer to PES data. */
    unsigned size                   /* Number of bytes pointed to by p_bfr. */
    );

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SCRAMBLE_RECORDING)
void BNAV_Indexer_GetSettings(
    BNAV_Indexer_Handle handle,
    BNAV_Indexer_Settings *settings
    );
void BNAV_Indexer_SetNavVersion(
    BNAV_Indexer_Handle handle,
    BNAV_Version navVersion
    );
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
void BNAV_Indexer_SetFifoRecord(
	BNAV_Indexer_Handle handle, 
	bool fifoRecord
	);
#endif

/**
* Summary:
* Feed start code table (SCT) data in reverse order into Bcmindexer.
*
* Description:
* Works like BNAV_Indexer_Feed, except that you feed SCT data in reverse order.
* If p_bfr points to more than one SCT, those SCT's should be in forward order
* and bcmindexer will traverse them in reverse. Subsequent calls to BNAV_Indexer_FeedReverse
* should send data in reverse order.
*
* Return values:
* -1 on error
* Number of entries processed
**/
long BNAV_Indexer_FeedReverse(
    BNAV_Indexer_Handle handle,
    const BSCT_Entry *p_bfr,                /* Pointer to start code index data */
    long numEntries                 /* Number of BSCT_Entry's pointed to by p_bfr. */
    );

/**
* Summary:
* Read whether Bcmindexer believes the MPEG it's indexing is HITS
* or not.
*
* Description:
* HITS means Head-end-in-the-sky and uses progressive refresh instead of
* I frames.
*
* You must feed a "good amount" of index through before you can tell, otherwise it
* defaults to true. Right now, the algorthim looks for one I frame. When it finds
* just one, it switches it to be not HITS. Therefore you need to feed in at least
* one whole GOP to get an I frame.
*
* Return values:
* 0 = not HITS
* 1 = it is HITS
**/
int BNAV_Indexer_IsHits(
    BNAV_Indexer_Handle handle
    );

/**
* Summary:
* Set the current VChip state.
*
* Description:
* Every entry written to disk after this call will contain this vchip state.
*
* Return values:
* 0 = success
* -1 = failure, invalid vchip state
**/
int BNAV_Indexer_SetVChipState(
    BNAV_Indexer_Handle handle,
    unsigned short vchipState
    );

/**
* Summary:
* Get the current VChip state.
*
* Return values:
* vchip state
**/
unsigned short BNAV_Indexer_GetVChipState(
    BNAV_Indexer_Handle handle
    );

/**
* Summary:
* Current position data returned by BNAV_Indexer_GetPosition.
* Description:
* Even though BNAV_Indexer_Position is identical with BNAV_Player_Position,
* they are separate structures because they belong to different API's and
* could diverge in the future.
**/
typedef struct {
    long index;                     /* Index offset in the Broadcom index */
    unsigned long pts;              /* PTS (Presentation Time Stamp) */
    unsigned long offsetHi;         /* Hi 32 bits of the MPEG file offset */
    unsigned long offsetLo;         /* Lo 32 bits of the MPEG file offset */
    unsigned long timestamp;        /* Bcmindexer calculated timestamp */
    unsigned short vchipState;      /* Current vchip state set by BNAV_Indexer_SetVChipState */
} BNAV_Indexer_Position;

/**
* Summary:
* Get current position information for Bcmindexer.
*
* Return values:
* 0 if successful
* -1 if unsuccessful (because no entries have been written yet)
**/
int BNAV_Indexer_GetPosition(
    BNAV_Indexer_Handle handle,
    BNAV_Indexer_Position *position     /* [out] */
    );

/**
* Summary:
* indexer status
**/
typedef struct BNAV_Indexer_Status {
    unsigned parsingErrors; /* number of SCT parsing errors detected by bcmindexer since open or reset.
                               these errors could result from either errors in the stream or errors in the indexing HW/FW/SW. */
} BNAV_Indexer_Status;

/**
* Summary:
* Get status
**/
int BNAV_Indexer_GetStatus(
    BNAV_Indexer_Handle handle,
    BNAV_Indexer_Status *pStatus     /* [out] */
    );

/**
* Convert 12 bit vchip data to 16 bit vchip data.
**/
unsigned short BNAV_unpack_vchip(unsigned short packed_vchip);

/**
* Convert 16 bit vchip data to 12 bit vchip data, checking the 16 bit
* data for bits 6 and 14. If bits 6 and 14 are both 1, the vchip data
* is valid and BNAV_pack_vchip returns 0 and the packed value is
* written to *packed_vchip. Otherwise it returns -1 and *packed_vchip is left unchanged.
*/
int BNAV_pack_vchip(unsigned short unpacked_vchip, unsigned short *packed_vchip);

#ifdef __cplusplus
}
#endif

#endif /* BCMINDEXER_H__ */

