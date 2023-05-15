/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bcmindexerpriv.h $
 * $brcm_Revision: 24 $
 * $brcm_Date: 3/5/12 3:33p $
 *
 * Module Description: Private types and prototypes for bcmindexer & bcmplayer
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/src/bcmindexerpriv.h $
 * 
 * 24   3/5/12 3:33p erickson
 * SW7425-2404: fix warning
 * 
 * 23   3/5/12 2:39p erickson
 * SW7425-2404: improve ptsBasedFrameRate algorithm for SW indexing
 * 
 * 22   2/27/12 4:13p erickson
 * SW7425-2495: move BNAV_Entry encode/decode to public API for use with
 * BNAV_Player_ReadIndex
 * 
 * 21   2/24/12 10:55a erickson
 * SW7425-2365: discard dangling fields in MPEG streams
 * 
 * 20   2/13/12 4:23p vsilyaev
 * SW7425-1089: Added support for new ITB format and 16(15) byte
 * extraction
 * 
 * 19   1/3/12 12:33p erickson
 * SW7231-578: add BNAV_Indexer_Status.parsingErrors
 * 
 * 18   7/1/11 10:16a vsilyaev
 * SW7405-5362: Updated handling of full packet SCT entries
 * 
 * 17   11/2/10 6:20p vsilyaev
 * SW7422-14: Fixed warnings
 * 
 * 16   11/2/10 6:11p vsilyaev
 * SW7422-14: Added initial support for SVC/MVC indexing
 * 
 * 15   10/19/10 12:56p vsilyaev
 * SW7422-14: Added interface for H.264 SVC/MVC indexing
 * 
 * 14   6/22/10 11:13a erickson
 * SW7405-4465: add BNAV_Indexer_Settings.ptsBasedFrameRate for PTS-based
 * offline indexing
 *
 * 13   5/13/10 9:51a erickson
 * SW7405-4105: add full packet parsing support. enabled >10 ITB's per TS
 * packet.
 *
 * 12   3/16/10 2:17p erickson
 * CDSTRMANA-294: support indexing of field-encoded MPEG streams
 *
 * 11   2/25/10 10:42a erickson
 * SW7400-2685: fix bcmindexer's handling of adjacent I frames
 *
 * 10   11/11/09 4:39p erickson
 * SWDEPRECATED-3668: make global into a const
 *
 * 9   2/25/09 5:04p erickson
 * PR52471: make global data const
 *
 * 8   4/2/08 11:44a gmohile
 * PR 41170 : Fix coverity CID 7943
 *
 * 7   4/2/08 9:32a erickson
 * PR41171: fix misspelling
 *
 * 6   2/5/08 3:52p gmohile
 * PR 38979 : Add Random Access indicator in index file
 *
 * 5   12/27/07 3:27p erickson
 * PR37035: if there's a gap in the recording, reset the timestamp base so
 * that it is skipped
 *
 * 4   10/8/07 7:40p vishk
 * PR 35262: LIB-Converity (CID 421): OVERRUN_STATIC,
 *
 * 3   10/3/07 9:33a vishk
 * PR 35262: LIB-Converity (CID 421): OVERRUN_STATIC,
 *
 * 2   3/28/07 11:48p erickson
 * PR27189: changed SEI indexing logic. previously, we including trailing
 * SEI's with SPS's and PPS's. with this code, we are including leading
 * SEI's with SPS's, PPS's and pictures. this is needed for trick modes
 * on streams that use SEI recovery points.
 *
 * Irvine_BSEAVSW_Devel/16   3/6/06 1:20p erickson
 * PR19853: added VC1 PES support
 *
 * Irvine_BSEAVSW_Devel/15   7/13/05 2:10p erickson
 * PR16138: added new NAV version for AVC support, PPS and SPS support
 * added, refactored player to better support this with less code
 * duplication
 *
 * Irvine_BSEAVSW_Devel/14   9/21/04 4:06p erickson
 * PR12728: cleaned up pedantic warnings
 *
 * Irvine_BSEAVSW_Devel/13   2/14/03 1:57p erickson
 * New naming convention
 * Fixed looping from previous rework
 * Removed bcmindexer _getParam/_setParam
 * Did not change code shared between tsplayer and bcmplayer
 * Refactored settings, playmode and other structures
 *
 * Irvine_HDDemo_Devel/12   9/26/02 3:4p erickson
 * Changed bcmindexer vchip from 3 bits to 16 bits packed into 12 bits.
 * The index is NOT backward compatible, but the 3 bit version had only
 * been released for 1 day.
 *
 * Irvine_HDDemo_Devel/11   9/10/02 10:37a erickson
 * converted vchip from 16 bits in [7] to 3 bits in [5]
 *
 * Irvine_HDDemo_Devel\9   7/2/02 4:52p erickson
 * added some parens for the macros
 *
 * Irvine_HDDemo_Devel\8   4/17/02 1:16p erickson
 * Made sBcmIndexEntry public
 * Added timestamp
 *
 * Irvine_HDDemo_Devel\7   4/16/02 12:58p erickson
 * Changed header
 *
 *
 ***************************************************************************/
#ifndef BCMINDEXERPRIV_H
#define BCMINDEXERPRIV_H

#include "bcmindexer_svc.h"
#include "bcmindexer_nav.h"

/* Convert eSCType to a string */
extern const char * const BNAV_frameTypeStr[eSCTypeUnknown+1];

struct BNAV_Indexer_HandleImpl
{
    BNAV_Indexer_Settings settings;

    char            seqHdrFlag;             /* Flag indicating if MPEG2 sequence header information was found.  */
    char            isISlice;               /* Flag indicating that we found an
                                               I-slice or I-frame */
    char            isHits;                 /* Flag indicating we're dealing with a
                                               HITS-stream */
    char            hitFirstISlice;         /* Flag indicating that we've hit the
                                               first I-slice */
    unsigned long   seqHdrSize;             /* Number of bytes in the sequence
                                               header */
    unsigned long   seqHdrStartOffset;      /* Sequence header offset (in bytes) relative to frame offset */
    unsigned long   picStart;               /* Offset of start of picture */
    unsigned long   picEnd;                 /* Offset of end of picture */
    struct {
        unsigned count;      /* count of fields in a field encoded picture */
        unsigned lastField;
        bool skippedPicture; /* SCT was skipped because it was believed to be a second field in an already indexed frame */
        BSCT_Entry skippedPictureSct; /* valid if skippedPicture is true */
        unsigned temporal_reference;
        unsigned picture_coding_type;
    } fieldEncoded;
    unsigned char   rfo;                    /* Reference frame offset counter */

    BNAV_Entry      curEntry;               /* The current MPEG2 structure that is being populated */
    BNAV_AVC_Entry  avcEntry;               /* The current AVC structure that is being populated. */

    unsigned long   next_pts;               /* pts buffer for next picture frame */
    bool random_access_indicator;

    unsigned starttime;              /* base timestamp for calculating NAV timestamp */
    unsigned lasttime;               /* last timestamp written to NAV entry */
    struct {
        /* pts and timestamp at beginning of this PTS continuity */
        unsigned startPts;
        unsigned startTimestamp;
        
        /* last PTS assigned. used for interpolated PTSs */
        unsigned frameCount; /* frame count since lastPts */
        unsigned lastPts;
        unsigned rate; /* in milliseconds/frame units */
    } ptsBasedFrameRate;

    unsigned        prev_pc;                /* previous picture code */
    int             prev_I_rfo;             /* This contains the rfo for the previous
                                                I frame, until the next P frame is detected.
                                                This value is added to the rfo of the open gop
                                                B frames. */
    unsigned short  vchip;                  /* Current packed vchip state */
    unsigned parsingErrors;

    struct {
#define MAX_GOP_SIZE 50
        BNAV_Entry      entry[MAX_GOP_SIZE];
        int total_entries;
    } reverse;

    /* this information is used to report BNAV_Indexer_GetPosition consistently */
    int             totalEntriesWritten;
    BNAV_Entry      lastEntryWritten;
    int allowOpenGopBs;

    /* define structures for AVC indexing */
#define TOTAL_PPS_ID 256
#define TOTAL_SPS_ID 32
    struct {
        int current_sps, current_pps; /* if -1, then no SPS or PPS being captured */
        int is_reference_picture; /* if true, then this picture has at least one slice that is referenced by another slice.
            this picture is not dropppable. */
        struct {
            unsigned long offset;
            unsigned long size;
            int sps_id;
        } pps[TOTAL_PPS_ID];
        struct {
            unsigned long offset;
            unsigned long size;
        } sps[TOTAL_SPS_ID];
        unsigned long current_sei;
        bool current_sei_valid;
    } avc;

    /* instead of complicated wraparound logic, I buffer a minimum amount before
    processing PES payload. MIN_PES_PAYLOAD includes the startcode byte itself. */
    #define MIN_PES_PAYLOAD 5
    struct {
        unsigned char buf[MIN_PES_PAYLOAD + 2];
        int bufsize;
        int sccount;
        uint64_t offset;

        bool vc1_interlace;
        uint64_t sequence_offset;
        int sequence_size;
        uint64_t entrypoint_offset;
        int entrypoint_size;
    } pes;

    /* bcmindexer "full packet" support allows bcmindexer to process a whole TS packet in the ITB.
    The whole TS packet is needed if the # of ITB's is greater than 10 per TS packet.
    You must #define BXPT_STARTCODE_BUFFER_WORKAROUND in XPT PI to enable this feature in the FW. */
    unsigned fullPacketCount;
    unsigned adaptFieldControl;
#define BCMINDEXER_TS_PACKET_SIZE 188
    uint8_t fullPacket[BCMINDEXER_TS_PACKET_SIZE];
    struct {
        unsigned offset, offsetHi;
    } lastPacketOffset;
    BNAV_Indexer_SVC_Info svc;
};

int BNAV_P_FeedPES_VC1(BNAV_Indexer_Handle handle, uint8_t *p_bfr, unsigned size);

int BNAV_Indexer_completeFrameAux(BNAV_Indexer_Handle handle,
    void *data, /* data points to either a BNAV_Entry or a BNAV_AVC_Entry. */
    int size  /* size of data */
    );

void BNAV_Indexer_StampEntry(BNAV_Indexer_Handle handle, BNAV_Entry *entry);

int BNAV_Indexer_P_AddFullPacketPayload(BNAV_Indexer_Handle handle, const BSCT_SixWord_Entry *next);


#endif
