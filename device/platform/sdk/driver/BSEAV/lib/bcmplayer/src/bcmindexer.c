/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bcmindexer.c $
 * $brcm_Revision: 56 $
 * $brcm_Date: 8/17/12 5:18p $
 *
 * Module Description: Converts startcode index to bcmplayer index
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/src/bcmindexer.c $
 * 
 * 56   8/17/12 5:18p erickson
 * SW7346-987: don't skip first SCT of full packet data
 * 
 * 55   4/6/12 11:34a erickson
 * SW7425-2365: print whole SCT in correct order
 * 
 * 54   3/26/12 3:50p erickson
 * SW7425-2365: only discard single dangling field, then proceed with
 * indexing
 * 
 * 53   3/5/12 3:33p erickson
 * SW7425-2404: fix warning
 * 
 * 52   3/5/12 2:39p erickson
 * SW7425-2404: improve ptsBasedFrameRate algorithm for SW indexing
 * 
 * 51   2/27/12 4:13p erickson
 * SW7425-2495: move BNAV_Entry encode/decode to public API for use with
 * BNAV_Player_ReadIndex
 * 
 * 50   2/24/12 1:18p erickson
 * SW7425-2365: fix warning
 * 
 * 49   2/24/12 10:55a erickson
 * SW7425-2365: discard dangling fields in MPEG streams
 *
 * 48   2/13/12 4:23p vsilyaev
 * SW7425-1089: Added support for new ITB format and 16(15) byte
 * extraction
 *
 * 47   1/5/12 12:46p erickson
 * SW7231-578: remove incorrect parsingErrors++
 *
 * 46   1/3/12 12:33p erickson
 * SW7231-578: add BNAV_Indexer_Status.parsingErrors
 *
 * 45   1/3/12 11:46a erickson
 * SW7231-235: change BDBG_MSG_TRACE to print all SCT data
 *
 * 44   8/31/11 1:21p vsilyaev
 * SW7405-5362: For TPIT SCT entries use proper field for the payload
 * offset
 *
 * 43   8/25/11 12:04p vsilyaev
 * SW7405-5362: Fixed typo that was causing drop of last SCT entry in
 * series
 *
 * 42   7/1/11 10:16a vsilyaev
 * SW7405-5362: Updated handling of full packet SCT entries
 *
 * 41   5/6/11 3:22p vsilyaev
 * SW3548-3157: Use monotonic clock for the index timestamps
 *
 * 40   4/19/11 3:38p vsilyaev
 * SW7422-14: Return proper status from the SVC indexer
 *
 * 39   3/10/11 2:24p vsilyaev
 * SW7422-14: Treat nal 0x14 as part of the frame payload
 *
 * 38   12/15/10 12:37p erickson
 * SW7405-4249: fix BNAV_Version_TimestampOnly mode. refactor "skip:"
 * label into a more regular for loop.
 *
 * 37   11/2/10 6:10p vsilyaev
 * SW7422-14: Added initial support for SVC/MVC indexing
 *
 * 36   10/19/10 12:56p vsilyaev
 * SW7422-14: Added interface for H.264 SVC/MVC indexing
 *
 * 35   10/7/10 10:54a vsilyaev
 * SW7422-14: Fixed typo
 *
 * 34   10/6/10 6:39p vsilyaev
 * SW7422-14: Updated Win32 support
 *
 * 33   9/2/10 3:22p mward
 * SW7125-610:  Compiler warning about unused i with big-endian build.
 *
 * 32   6/22/10 11:13a erickson
 * SW7405-4465: add BNAV_Indexer_Settings.ptsBasedFrameRate for PTS-based
 * offline indexing
 *
 * 31   6/21/10 4:31p erickson
 * SW7405-4249: added BNAV_Version_TimestampOnly option for indexing
 * audio-only or scrambled streams
 *
 * 30   6/4/10 5:37p erickson
 * SW7400-2788: provide a minimal safe increment so that timestamps can be
 * unique and guaranteed increasing
 *
 * 29   6/2/10 12:51p erickson
 * SW7400-2789: add BNAV_Indexer_Settings.allowLargeTimeGaps
 *
 * 28   5/13/10 9:51a erickson
 * SW7405-4105: add full packet parsing support. enabled >10 ITB's per TS
 * packet.
 *
 * 27   4/30/10 11:51a erickson
 * SW7405-4105: fix parsing of AVC payload bytes [5] and [6]. add debug.
 * remove support for non-RAVE SCT format.
 *
 * 26   3/16/10 2:17p erickson
 * CDSTRMANA-294: support indexing of field-encoded MPEG streams
 *
 * 25   2/25/10 10:42a erickson
 * SW7400-2685: fix bcmindexer's handling of adjacent I frames
 *
 * 24   2/12/10 2:47p erickson
 * SW7400-2685: add BDBG_ERR for unknown picture_coding_type
 *
 * 23   11/11/09 4:39p erickson
 * SWDEPRECATED-3668: make global into a const
 *
 * 22   4/17/09 7:06a erickson
 * PR54150: check gettimeofday return code
 *
 * 21   2/25/09 5:04p erickson
 * PR52471: make global data const
 *
 * 20   1/12/09 4:10p erickson
 * PR49501: downgrade ERR to MSG
 *
 * 19   1/7/09 11:09p erickson
 * PR49501: add append settings. removed BCM7030 support to make 40 bit
 * offset support easier.
 *
 * 18   10/22/08 1:06p vishk
 * PR 48074: Coverity Defect ID:12257 DEADCODE bcmindexer.c
 * Product=97401linux
 *
 * 17   7/16/08 11:19a erickson
 * PR44853: clean up -Wstrict-prototypes warning
 *
 * 16   4/2/08 9:32a erickson
 * PR41171: fix misspelling
 *
 * 15   4/2/08 9:23a erickson
 * PR41165: assign p_curBfr before using it
 *
 * 14   2/26/08 12:28p katrep
 * PR38429: Added AVS record and playback support
 *
 * 13   2/5/08 3:52p gmohile
 * PR 38979 : Add Random Access indicator in index file
 *
 * 12   1/3/08 6:21p erickson
 * PR36068: switch B_HAS_RAVE to opt-out. all new chips have RAVE. if you
 * are using a legacy chip with latest bcmplayer, please add your
 * BCHP_CHIP.
 *
 * 11   12/27/07 3:27p erickson
 * PR37035: if there's a gap in the recording, reset the timestamp base so
 * that it is skipped
 *
 * 10   12/17/07 1:44p katrep
 * PR37217: Added 7335 support
 *
 * 9   10/10/07 5:50p vishk
 * PR 35272: SettopAPI-Coverity (CID 327): DEADCODE,
 *
 * 8   10/5/07 11:06a vishk
 * PR 35272: SettopAPI-Coverity (CID 327): DEADCODE,
 *
 * 6   9/11/07 4:29p arbisman
 * PR33338: Udate 7400 WinCE 5.0 branch
 *
 * 5   3/28/07 11:48p erickson
 * PR27189: changed SEI indexing logic. previously, we including trailing
 * SEI's with SPS's and PPS's. with this code, we are including leading
 * SEI's with SPS's, PPS's and pictures. this is needed for trick modes
 * on streams that use SEI recovery points.
 *
 * 4   2/7/07 11:07p katrep
 * PR27183: Added debug code to vlidate SCT generated by HW.
 *
 * 3   2/2/07 3:06p erickson
 * PR26328: added #ifndef B_IS_SOFTINDEXER
 *
 * 2   11/27/06 1:59p erickson
 * PR25109: added 7403/7405 support
 *
 * Irvine_BSEAVSW_Devel/81   10/20/06 10:58a erickson
 * PR21742: print error if framesize == 0
 *
 * Irvine_BSEAVSW_Devel/80   10/5/06 3:34p mward
 * PR21671: 7118 B_HAS_RAVE
 *
 * Irvine_BSEAVSW_Devel/79   6/30/06 2:52p erickson
 * PR21941: fix debug msg
 *
 * Irvine_BSEAVSW_Devel/78   6/23/06 8:46a erickson
 * PR21941: downgrade open GOP WRN to MSG
 *
 * Irvine_BSEAVSW_Devel/77   5/11/06 3:31p mphillip
 * PR20797: Replace currentTimestamp implementation to avoid an overflow
 * resulting in bogus values on trimming
 *
 * Irvine_BSEAVSW_Devel/76   4/6/06 11:35a jrubio
 * PR18463: Removed >>1 for PTS on RAVE.
 *
 * Irvine_BSEAVSW_Devel/75   3/31/06 4:24p erickson
 * PR20569: parse PTS from SCT for RAVE platforms
 *
 * Irvine_BSEAVSW_Devel/74   3/6/06 1:20p erickson
 * PR19853: added VC1 PES support
 *
 * Irvine_BSEAVSW_Devel/73   1/6/06 4:45p erickson
 * PR17108: cleaned up DEBUG=n warnings
 *
 * Irvine_BSEAVSW_Devel/72   8/25/05 12:36p erickson
 * PR16678: merged WinCE changes
 *
 * Irvine_BSE_Release/PROD_WinCE50/2   8/19/05 4:15p arbisman
 * PR16678: Add support for WinCE
 *
 * Irvine_BSEAVSW_Devel/71   8/19/05 3:59p erickson
 * PR16208: terminate the PPS and SPS after the SEI, which allows the SEI
 * to be sent without indexing it separately
 *
 * Irvine_BSEAVSW_Devel/70   8/8/05 3:58p erickson
 * PR16138: handle all PES start codes (not just E0), and avoid bad logic
 * for swapping B's and P's
 *
 * Irvine_BSEAVSW_Devel/69   8/3/05 12:07p erickson
 * PR16138: change definition of B frame in NAV table for AVC streams. It
 * is now "non-I frame which is not a reference picture." See comment in
 * code.
 *
 * Irvine_BSEAVSW_Devel/68   7/15/05 8:25p erickson
 * PR16138: fixed AVC rfo and sps_id, added debug
 *
 * Irvine_BSEAVSW_Devel/67   7/13/05 2:10p erickson
 * PR16138: added new NAV version for AVC support, PPS and SPS support
 * added, refactored player to better support this with less code
 * duplication
 *
 * Irvine_BSEAVSW_Devel/66   7/8/05 10:11a erickson
 * PR16155: remove debug
 *
 * Irvine_BSEAVSW_Devel/65   7/8/05 9:57a erickson
 * PR16155: changed b_vlc_decode calling convention
 *
 * Irvine_BSEAVSW_Devel/64   7/7/05 1:18p erickson
 * PR16138: added support for non-IDR I frame for AVC, and capture (but
 * not use of) SPS and PPS.
 *
 * Irvine_BSEAVSW_Devel/63   3/22/05 5:01p erickson
 * PR14451: fixed 6word, non-avc
 *
 * Irvine_BSEAVSW_Devel/62   3/21/05 3:43p erickson
 * PR14451: moved vlc decode algorithm to separate file
 *
 * Irvine_BSEAVSW_Devel/61   3/21/05 12:25p erickson
 * PR14451: fixed FeedAVC for more than 1 SCT at a time
 *
 * Irvine_BSEAVSW_Devel/60   3/18/05 2:13p erickson
 * PR14451: working impl of AVC trick modes
 *
 * Irvine_BSEAVSW_Devel/59   3/18/05 10:01a erickson
 * PR14451: added first pass AVC indexing support
 *
 * Irvine_BSEAVSW_Devel/58   12/14/04 4:54p marcusk
 * PR13560: Updated to default to frameratesimulation to 0.  Command line
 * indexer updated to accept frame rate setting.
 *
 * Irvine_BSEAVSW_Devel/57   9/21/04 4:06p erickson
 * PR12728: cleaned up pedantic warnings
 *
 * Irvine_BSEAVSW_Devel/56   5/10/04 11:33a erickson
 * PR10065: fixed compiler warnings (most were unsigned/signed mismatches)
 *
 * Irvine_BSEAVSW_Devel/55   4/30/04 12:57p erickson
 * PR10909: default to magnum kernelinterface (debug or release) but
 * support legacy if specifically requested
 *
 * Irvine_BSEAVSW_Devel/54   4/13/04 4:59p erickson
 * PR10292: added BNAV_Indexer_FeedReverse and refactored completeFrame to
 * put common code in BNAV_Indexer_StampEntry
 *
 * Irvine_BSEAVSW_Devel/53   11/11/03 5:03p erickson
 * PR8563: wasn't indexing for timestamps correctly. Now that I am, brcm
 * trick modes are working.
 *
 * Irvine_BSEAVSW_Devel/52   11/10/03 2:29p erickson
 * PR8563: added transport timestamp support to bcmplayer
 *
 * Irvine_BSEAVSW_Devel/51   9/17/03 12:11p erickson
 * support both original and magnum debug interfaces
 * replaced kernel interface calls with ansi calls
 *
 * Irvine_BSEAVSW_Devel/50   8/14/03 10:35a erickson
 * removed printf
 *
 * Irvine_BSEAVSW_Devel/49   7/21/03 2:32p erickson
 * added support for B7's (sequence end codes)
 *
 * Irvine_BSEAVSW_Devel/48   6/24/03 3:11p erickson
 * don't have MSG's by default
 *
 * Irvine_BSEAVSW_Devel/47   6/24/03 11:03a erickson
 * PR7218 - handled partially encrypted streams. Added maxFrameSize and
 * mpegSizeCallback. Had to change the
 * NAV table format (version 2) in order to handle I frames with reference
 * offset of 0. Bcmplayer
 * is backward compatible.
 *
 * Irvine_BSEAVSW_Devel/46   6/19/03 9:04a erickson
 * added BNAV_Indexer_GetPosition (PR7211)
 *
 * Irvine_BSEAVSW_Devel/45   3/11/03 3:59p erickson
 * fixed debug interface usage
 *
 * Irvine_BSEAVSW_Devel/44   2/14/03 2:40p erickson
 * removed \n's from debug msg
 *
 * Irvine_BSEAVSW_Devel/43   2/14/03 1:57p erickson
 * New naming convention
 * Fixed looping from previous rework
 * Removed bcmindexer _getParam/_setParam
 * Did not change code shared between tsplayer and bcmplayer
 * Refactored settings, playmode and other structures
 *
 * Irvine_BSEAVSW_Devel/42   2/10/03 5:42p erickson
 * updated tests and utils for new bcmplayer
 *
 * Irvine_HDDemo_Devel/41   9/26/02 3:4p erickson
 * Changed bcmindexer vchip from 3 bits to 16 bits packed into 12 bits.
 * The index is NOT backward compatible, but the 3 bit version had only
 * been released for 1 day.
 *
 * Irvine_HDDemo_Devel/40   9/10/02 10:38a erickson
 * converted vchip from 16 bits in [7] to 3 bits in [5]
 *
 ***************************************************************************/
#ifndef USE_LEGACY_KERNAL_INTERFACE
/* Magnum debug interface */
#include "bstd.h"
#include "bkni.h"
BDBG_MODULE(bcmindexer);
#else
/* Original debug interface */
#define BRCM_DBG_MODULE_NAME "bcmindexer"
#include "brcm_dbg.h"
#define BDBG_ERR(X) BRCM_DBG_ERR(X)
#define BDBG_WRN(X) BRCM_DBG_WRN(X)
#define BDBG_MSG(X) BRCM_DBG_MSG(X)
#define BDBG_DEBUG_BUILD 1
#endif

#if 0
/* easy way to turn on debug */
#include <stdio.h>
#undef BDBG_MSG
#define BDBG_MSG(X) do{printf X; printf("\n");}while(0)
#undef BDBG_WRN
#define BDBG_WRN(X) do{printf X; printf("\n");}while(0)
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(BDBG_DEBUG_BUILD)
/*for debugging purpose: will be deleted after testing unified version.*/
int iHmxPvrDbgLevel=0;
#define HDBG_PrintLog(LEVEL, STR) do{ if((iHmxPvrDbgLevel%2)?(iHmxPvrDbgLevel==LEVEL):(iHmxPvrDbgLevel>=LEVEL)) printf STR;}while(0)
#else
#define HDBG_PrintLog(LEVEL, STR)
#endif

#define BDBG_MSG_TRACE(X) BDBG_MSG(X)

#include "bcmindexer.h"
#include "bcmindexerpriv.h"
#include "mpeg2types.h"
#include "avstypes.h"
#include "bvlc.h"
#include <string.h>
#include <stdlib.h>
#ifdef LINUX
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#elif defined __vxworks
#include <time.h>
#elif defined _WIN32_WCE
#include <Windows.h>
#include <mmsystem.h>
#elif defined _WIN32
#include <Windows.h>
typedef __int64 off_t;
#endif

/* If we offset two words into an SCT6 entry, it looks just like an SCT4 entry,
ignoring the extra payload bytes */
#define CONVERT_TO_SCT4(P_SCT6_ENTRY) \
    ((BSCT_Entry*)((char*)P_SCT6_ENTRY+(sizeof(BSCT_SixWord_Entry)-sizeof(BSCT_Entry))))

/* enable this to add start code validation
   debug messages at the compile time */
/* #define VALIDATE_SC_NAV */
#ifdef VALIDATE_SC_NAV
#define VALIDATE_MPEG_SC(handle,p_curBfr,sc) \
        if(((p_curBfr->startCodeBytes&0xff) > 0xbc)||!(sc==SC_ANY_NON_PTS||sc==SC_PTS||sc==SC_PES||sc==SC_SEQUENCE \
            ||sc==SC_PICTURE||sc==SC_FIRST_SLICE||sc==SC_EXTENSION||sc==SC_SEQ_END||sc==SC_GOP||sc==SC_PES_END \
            ||sc==PC_I_FRAME||sc==PC_P_FRAME||sc==PC_B_FRAME||sc==PC_ANY_FRAME))\
        {\
          BDBG_WRN(("Invalid SCT Entry: %02x pkt offset 0x%02lx%08lx,sc offset 0x%02x",sc, \
          handle->parsingErrors++; \
          (p_curBfr->recordByteCountHi>>24)&0xff,p_curBfr->recordByteCount,p_curBfr->startCodeBytes & 0xff));\
        }

#define VALIDATE_AVC_SC(handle,p_curSct4,sc)                   \
        if((p_curSct4->startCodeBytes&0xff)>0xbc)       \
        {                                               \
          BDBG_WRN(("Invalid SCT Entry: %02x pkt offset 0x%02lx%08lx,offset 0x%02x", sc, \
          handle->parsingErrors++; \
          (p_curSct4->recordByteCountHi>>24) & 0xFF, p_curSct4->recordByteCount,p_curSct4->startCodeBytes & 0xff));\
        }

#define VALIDATE_AVS_SC(handle,p_curBfr,sc) \
        if(((p_curBfr->startCodeBytes&0xff) > 0xbc)||!(sc==SC_AVS_ANY_NON_PTS||sc==SC_AVS_PTS||sc==SC_AVS_PES||sc==SC_AVS_SEQUENCE \
            ||sc==SC_AVS_PICTURE_I ||sc==SC_AVS_PICTURE_PB ||sc==SC_AVS_EXTENSION||sc==SC_AVS_SEQ_END||sc==SC_AVS_PES_END \
            ||sc==PC_AVS_P_FRAME||sc==PC_AVS_B_FRAME || sc==SC_AVS_FIRST_SLICE  ))\
        {\
          BDBG_WRN(("Invalid AVS SCT Entry: %02x pkt offset 0x%02lx%08lx,sc offset 0x%02x",sc, \
          handle->parsingErrors++; \
          (p_curBfr->recordByteCountHi>>24)&0xff,p_curBfr->recordByteCount,p_curBfr->startCodeBytes & 0xff));\
        }

#define VALIDATE_MPEG_NAV(handle, frameType)\
        if(!(frameType == eSCTypeIFrame || frameType == eSCTypePFrame ||\
           frameType == eSCTypeBFrame || eSCTypeBFrame == eSCTypeRPFrame))\
        {\
        }
#else

#define VALIDATE_MPEG_SC(handle, sc_offset,sc)
#define VALIDATE_AVC_SC(handle, sc_offset,sc)
#define VALIDATE_AVS_SC(handle, p_curBfr,sc)
#define VALIDATE_MPEG_NAV(handle, frameType)

#endif


/*---------------------------------------------------------------
- PRIVATE FUNCTIONS
---------------------------------------------------------------*/
static unsigned long BNAV_Indexer_returnPts(const BSCT_Entry *p_sct );
static int BNAV_Indexer_completeFrame( BNAV_Indexer_Handle handle );
static unsigned long BNAV_Indexer_subtractByteCounts( unsigned long largerNum, unsigned long smallerNum );
static unsigned long BNAV_Indexer_getScByteOffsetLo( BNAV_Indexer_Handle handle, const BSCT_Entry *p_entry );
static unsigned long BNAV_Indexer_getScByteOffsetHi( BNAV_Indexer_Handle handle, const BSCT_Entry *p_entry );
static long BNAV_Indexer_FeedAVC(BNAV_Indexer_Handle handle, const void *p_bfr, long numEntries);
static long BNAV_Indexer_FeedAVS(BNAV_Indexer_Handle handle, void *p_bfr, long numEntries);
static long BNAV_Indexer_FeedMPEG2(BNAV_Indexer_Handle handle, void *p_bfr, long numEntries);
static int BNAV_Indexer_Flush(BNAV_Indexer_Handle handle);
static int BNAV_Indexer_completeTimestampFrame(BNAV_Indexer_Handle handle);
static long BNAV_Indexer_P_FilterFullPacket(BNAV_Indexer_Handle handle, const BSCT_SixWord_Entry *sct, long numEntries, long (*feed)(BNAV_Indexer_Handle , const void *, long ));

/**
* currentTimestamp returns the # of milliseconds from some fixed point in the past.
* I currently have a LINUX version. The generic version is only 1 second accurate,
* which might be sufficient if the requirement is to make +/- 30 second jumps.
**/
static unsigned int currentTimestamp(void) {
#if defined LINUX
    struct timespec now;
    if(clock_gettime(CLOCK_MONOTONIC, &now)!=0) {
        return 0;
    } else {
        return now.tv_sec * 1000 + now.tv_nsec / 1000000;
    }
#elif defined __vxworks
    return time(NULL) * 1000;
#elif defined _WIN32_WCE
    return GetTickCount();
#elif defined _WIN32
    return GetTickCount();
#endif
}

void BNAV_Indexer_GetDefaultSettings(BNAV_Indexer_Settings *settings)
{
    memset(settings, 0, sizeof(*settings));
    settings->simulatedFrameRate = 0;
    settings->sctVersion = BSCT_Version40bitOffset;
    settings->videoFormat = BNAV_Indexer_VideoFormat_MPEG2;
    settings->navVersion = BNAV_VersionLatest;
    settings->maxFrameSize = 5 * 1024 * 1024; /* We've seen 500K HD I frames. This is 10x larger. */
}

/******************************************************************************
* INPUTS:   cb = pointer to a function that stores table entries (same format as fwrite)
*           fp = general pointer that is passed in as param 3 into cb function
*           pid = pid of the transport stream used for table generation
* OUTPUTS:  None.
* RETURNS:  pointer to sBcmIndexer structure
* FUNCTION: This function allocates and initializes an indexer structure.
******************************************************************************/
int BNAV_Indexer_Open(BNAV_Indexer_Handle *handle, const BNAV_Indexer_Settings *settings)
{
    *handle = (BNAV_Indexer_Handle)malloc( sizeof( struct BNAV_Indexer_HandleImpl) );
    if (BNAV_Indexer_Reset(*handle, settings)) {
        free(*handle);
        return -1;
    }
    return 0;
}

int BNAV_Indexer_Reset(BNAV_Indexer_Handle handle, const BNAV_Indexer_Settings *settings)
{
    int result = 0;

    if (!settings->writeCallback ||
        !settings->filePointer)
    {
        BDBG_ERR(("Missing required settings"));
        return -1;
    }

    if (settings->videoFormat == BNAV_Indexer_VideoFormat_AVC &&
        settings->navVersion != BNAV_Version_AVC)
    {
        BDBG_ERR(("You need to select an AVC BNAV_Version for bcmindexer."));
        return -1;
    }
    else if(
            (settings->navVersion == BNAV_Version_AVC_Extended &&
             !(settings->videoFormat == BNAV_Indexer_VideoFormat_AVC_SVC || settings->videoFormat == BNAV_Indexer_VideoFormat_AVC_MVC))
         || ((settings->navVersion != BNAV_Version_AVC_Extended) &&
              (settings->videoFormat == BNAV_Indexer_VideoFormat_AVC_SVC || settings->videoFormat == BNAV_Indexer_VideoFormat_AVC_MVC))
        ) {
        BDBG_ERR(("You need to select an AVC_Extended BNAV_Version for bcmindexer."));
        return -1;
    }
    else if (settings->videoFormat != BNAV_Indexer_VideoFormat_AVC &&
        settings->navVersion == BNAV_Version_AVC)
    {
        BDBG_ERR(("You need to select an MPEG2 BNAV_Version for bcmindexer."));
        return -1;
    }
    else if (settings->videoFormat != BNAV_Indexer_VideoFormat_AVS &&
        settings->navVersion == BNAV_Version_AVS)
    {
        BDBG_ERR(("You need to select an MPEG2 BNAV_Version for bcmindexer."));
        return -1;
    }
    else if (settings->navVersion >= BNAV_VersionUnknown) {
        BDBG_ERR(("Unsupported BNAV_Version"));
        return -1;
    }

    memset(handle, 0, sizeof(*handle));
    BNAV_Indexer_FeedAVC_Init(handle);

    handle->settings = *settings;

    if (!settings->simulatedFrameRate && !settings->ptsBasedFrameRate) {
        /* real, not simulated */
        handle->lasttime = handle->starttime = currentTimestamp();
    }

    BNAV_set_frameType(&(handle->curEntry), eSCTypeUnknown);
    BNAV_set_frameType(&(handle->avcEntry), eSCTypeUnknown);

    BNAV_set_frameOffsetLo(&(handle->curEntry), handle->settings.append.offsetLo);
    BNAV_set_frameOffsetHi(&(handle->curEntry), handle->settings.append.offsetHi);
    BNAV_set_frameOffsetLo(&(handle->avcEntry), handle->settings.append.offsetLo);
    BNAV_set_frameOffsetHi(&(handle->avcEntry), handle->settings.append.offsetHi);

    handle->isHits = 1;  /* Assume HITS until we're proven wrong */
    handle->avc.current_sps = handle->avc.current_pps = -1;

    return result;
}

/******************************************************************************
* INPUTS:   handle = pointer to a previously allocated sBcmIndexer structure
* OUTPUTS:  None.
* RETURNS:  None.
* FUNCTION: This function frees any memory used by an indexer structure.
******************************************************************************/
void BNAV_Indexer_Close(BNAV_Indexer_Handle handle)
{
    free((void *)handle);
}

static void BNAV_Indexer_P_AddAppend(BNAV_Indexer_Handle handle, unsigned long *offsetHi, unsigned long *offset)
{
    if (handle->settings.append.offsetHi || handle->settings.append.offsetLo) {
        off_t value = ((off_t)*offsetHi << 32) + *offset;

        /* add the append amount */
        value += ((off_t)handle->settings.append.offsetHi << 32) + handle->settings.append.offsetLo;

        *offset = value & 0xFFFFFFFF;
        *offsetHi = value >> 32;
    }
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SCRAMBLE_RECORDING)
long BNAV_Indexer_FeedScd(BNAV_Indexer_Handle handle, void *p_bfr, long numEntries)
#else
long BNAV_Indexer_Feed(BNAV_Indexer_Handle handle, void *p_bfr, long numEntries)
#endif
{
	HDBG_PrintLog(3,("%s(%d)\n", __func__, __LINE__));
    switch(handle->settings.videoFormat) {
    case BNAV_Indexer_VideoFormat_AVC_MVC:
        return BNAV_Indexer_FeedAVC_MVC(handle, p_bfr, numEntries);
    case BNAV_Indexer_VideoFormat_AVC_SVC:
        return BNAV_Indexer_FeedAVC_SVC(handle, p_bfr, numEntries);
    case BNAV_Indexer_VideoFormat_AVC:
        return BNAV_Indexer_P_FilterFullPacket(handle, p_bfr, numEntries, BNAV_Indexer_FeedAVC);
    case BNAV_Indexer_VideoFormat_AVS:
        return BNAV_Indexer_FeedAVS(handle, p_bfr, numEntries);
    case BNAV_Indexer_VideoFormat_MPEG2:
        return BNAV_Indexer_FeedMPEG2(handle, p_bfr, numEntries);
    default:
        BDBG_ERR(("not supported videoFormat: %u", handle->settings.videoFormat));
        return -1;
    }
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SCRAMBLE_RECORDING)
#define TPIT_ENTRY_TYPE 0x01
#define TICK_PER_MS				108 
#define TIMESTAMP_WRAP_VALUE	(0xffffffff/TICK_PER_MS)

static long BNAV_Indexer_FeedTpit(BNAV_Indexer_Handle handle, void *p_bfr, long numEntries)
{
	int numBytes, i;
	unsigned int navEntrySize;
	void *alloc_buff = NULL;
	unsigned long *p_inbfr;
	unsigned long  frameSize;
#if 0
	unsigned long cur_ms;
	unsigned long last_wrap_ms;
	unsigned long diff;
#endif
	off_t recCount;
	off_t lastRecCount;

	BNAV_Entry *navEntry;
	BNAV_Entry *lastNaventry = &handle->lastEntryWritten;

	HDBG_PrintLog(3,("%s(%d)\n", __func__, __LINE__));
	navEntrySize = (handle->settings.navVersion==BNAV_Version_TPIT_AVC) ? sizeof(BNAV_AVC_Entry) : sizeof(BNAV_Entry);
	alloc_buff = malloc(numEntries * navEntrySize);
	if(alloc_buff == NULL)
	{
		BDBG_ERR(("Can't alloc BNAV_Entry buffer"));
		return -1;
	}
	/* clear buffer */
	memset(alloc_buff, 0, numEntries * navEntrySize);

	navEntry = (BNAV_Entry *)alloc_buff;
	p_inbfr = (unsigned long *)p_bfr;
	lastRecCount = ((off_t)BNAV_get_frameOffsetHi(lastNaventry)<<32) + (off_t)BNAV_get_frameOffsetLo(lastNaventry) + (off_t)BNAV_get_frameSize(lastNaventry);

	for(i=0; i<numEntries; i++)
	{
		/* Make BNAV_Entry
		 record count low : Entry Word 3
		 record count hi	 : Entry Word 2  [31:24] */
		recCount = ((off_t)((p_inbfr[2]>>24)&0x0FF)<<32) + (off_t)(p_inbfr[3]);

		if((recCount == (off_t)0) || (recCount < lastRecCount))
		{
			i --;
			numEntries --;
			p_inbfr += sizeof(BSCT_SixWord_Entry)/sizeof(unsigned long);
			continue;
		}

		frameSize = labs((unsigned long)(recCount&0xFFFFFFFF) - (unsigned long)(lastRecCount&0xFFFFFFFF));
		BNAV_set_frameOffsetLo(navEntry, (unsigned long)(lastRecCount&0xFFFFFFFF));
		BNAV_set_frameOffsetHi(navEntry, (unsigned long)((lastRecCount>>32)&0xFFFFFFFF));
		BNAV_set_frameSize(navEntry, frameSize);
/*
 *	20091004 :: POOH :: the tpit timestamp use the same value of scd timestamp
 */
#if 0
		/* Set Timestamp */
		cur_ms	= p_inbfr[4]/TICK_PER_MS;
		last_wrap_ms = handle->lastTimeStamp%TIMESTAMP_WRAP_VALUE;

		diff = (cur_ms >= last_wrap_ms) ? cur_ms - last_wrap_ms : TIMESTAMP_WRAP_VALUE - last_wrap_ms + cur_ms;
		handle->lastTimeStamp += diff;
		BNAV_set_timestamp(navEntry, handle->lastTimeStamp);
#else
		BNAV_Indexer_StampEntry(handle, navEntry);
#endif
		/* Set PTS */
		BNAV_set_framePts(navEntry, (p_inbfr[5]));

		BNAV_set_packed_vchip(navEntry, handle->vchip);
		BNAV_set_version(navEntry, (handle->settings.navVersion==BNAV_Version_TPIT_AVC)?BNAV_Version_AVC:BNAV_VersionLatest);
		BNAV_set_frameType(navEntry, eSCTypeIFrame);
		/* BNAV_set_fifoRecord_flag(navEntry, handle->settings.fifoRecord); for playback of delayed record file */	


		/* Backup Offset */
		lastRecCount = recCount;

		navEntry = (BNAV_Entry *)((unsigned char*)navEntry + navEntrySize);
		p_inbfr += sizeof(BSCT_SixWord_Entry)/sizeof(unsigned long);
	}

	if(numEntries)
	{
		/* write using callback */
		numBytes = (*handle->settings.writeCallback)(alloc_buff, numEntries, navEntrySize, handle->settings.filePointer);
		if (numBytes != (numEntries*(int)navEntrySize))
		{
			free(alloc_buff);
			BDBG_ERR(("Unable to write index entry."));
			return -1;
		}

		handle->totalEntriesWritten += numEntries;
		memcpy(lastNaventry, (void *)((unsigned int)navEntry-navEntrySize), sizeof(BNAV_Entry));
		memcpy(&handle->curEntry, (void *)((unsigned int)navEntry-navEntrySize), sizeof(BNAV_Entry));
	}

	free(alloc_buff);

	return numEntries;
}

long BNAV_Indexer_Feed(BNAV_Indexer_Handle handle, void *p_bfr, long numEntries)
{
	unsigned long *p_inbfr;
	unsigned long *pEnrtyBuffer = NULL;
	unsigned long *pEnrty;
	long i, realnumEntries, returnVal;

	HDBG_PrintLog(5,("(%s:%ld): Index Arrived (numEntries:%d)\n", __func__, __LINE__, numEntries));
	if(handle->settings.sctVersion == BSCT_Version6wordEntry)
	{
		pEnrtyBuffer = (unsigned long *)malloc(numEntries*sizeof(BSCT_SixWord_Entry));
		if(pEnrtyBuffer== NULL)
		{
			BDBG_ERR(("Can't alloc entry buffer!"));
			return -1;
		}

		realnumEntries = 0;
		p_inbfr = (unsigned long *)p_bfr;
		pEnrty = pEnrtyBuffer;
		for(i=0; i<numEntries; i++)
		{
			if(handle->settings.navVersion == BNAV_Version_TPIT || handle->settings.navVersion == BNAV_Version_TPIT_AVC)
			{
				if(TPIT_ENTRY_TYPE == ((*p_inbfr >> 24) & 0xff)) /* Get the type from Enrty */
				{
					HDBG_PrintLog(7,("[%d out of %d] TPIT\n", i, numEntries));
					memcpy(pEnrty, p_inbfr, sizeof(BSCT_SixWord_Entry));
					realnumEntries++;
					pEnrty += sizeof(BSCT_SixWord_Entry)/sizeof(unsigned long);
				}
			}
			else
			{
				if(TPIT_ENTRY_TYPE != ((*p_inbfr >> 24) & 0xff)) /* Get the type from Enrty */
				{
					HDBG_PrintLog(7,("[%d out of %d] SCD\n", i, numEntries));
					memcpy(pEnrty, p_inbfr, sizeof(BSCT_SixWord_Entry));
					realnumEntries++;
					pEnrty += sizeof(BSCT_SixWord_Entry)/sizeof(unsigned long);
				}
			}
			p_inbfr += sizeof(BSCT_SixWord_Entry)/sizeof(unsigned long);
		}

		if(handle->settings.navVersion == BNAV_Version_TPIT || handle->settings.navVersion == BNAV_Version_TPIT_AVC)
		{
			returnVal = BNAV_Indexer_FeedTpit(handle, pEnrtyBuffer, realnumEntries);
		}
		else
		{
			returnVal = BNAV_Indexer_FeedScd(handle, pEnrtyBuffer, realnumEntries);
		}
		free((void *)pEnrtyBuffer);
	}
	else
	{
		returnVal = BNAV_Indexer_FeedScd(handle, p_bfr, numEntries);
	}

	/* return written NAV entry number */
	return returnVal;
}

void BNAV_Indexer_GetSettings(BNAV_Indexer_Handle handle, BNAV_Indexer_Settings *settings)
{
	*settings = handle->settings;
}

void BNAV_Indexer_SetNavVersion(BNAV_Indexer_Handle handle, BNAV_Version navVersion)
{
	handle->settings.navVersion = navVersion;
}
#endif /*HUMAX_PLATFORM_BASE*/


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
void BNAV_Indexer_SetFifoRecord(BNAV_Indexer_Handle handle, bool fifoRecord)
{
	handle->settings.fifoRecord = fifoRecord;
}
#else
void BNAV_Indexer_SetFifoRecord(BNAV_Indexer_Handle handle, bool fifoRecord)
{
	BSTD_UNUSED(handle);
	BSTD_UNUSED(fifoRecord);
}
#endif

static int BNAV_Indexer_ProcessMPEG2SCT(BNAV_Indexer_Handle handle, const BSCT_Entry *p_curBfr)
{
    BNAV_Entry *navEntry = &handle->curEntry;
    int sc = returnStartCode(p_curBfr->startCodeBytes); /* parse once */
    unsigned long offset = BNAV_Indexer_getScByteOffsetLo(handle, p_curBfr);
    unsigned long offsetHi = BNAV_Indexer_getScByteOffsetHi(handle, p_curBfr);

    BNAV_Indexer_P_AddAppend(handle, &offsetHi, &offset);

    /* detect invalid start code and offsets */
    VALIDATE_MPEG_SC(handle, p_curBfr,sc);

    BDBG_MSG_TRACE(("SCT Entry: %02x %08x %08x %08x %08x", sc, p_curBfr->startCodeBytes, p_curBfr->recordByteCount, p_curBfr->recordByteCountHi, p_curBfr->flags));

    if (handle->settings.navVersion == BNAV_Version_TimestampOnly) {
        switch (sc) {
        /* BNAV_Version_TimestampOnly does not require PTS, but it will index it if found */
        case SC_PTS:
            handle->next_pts = BNAV_Indexer_returnPts(p_curBfr);
            break;
        default:
            /* BNAV_Version_TimestampOnly requires at least one non-PTS startcode so it can get
            the file offset. Without the file offset, the timestamp has no use. */
            {
                unsigned currenttime = currentTimestamp();
                unsigned framesize = BNAV_Indexer_subtractByteCounts( offset, handle->picStart);

                /* don't write out NAV's more than 1 per 30 msec or that are too small. */
                if (currenttime >= handle->lasttime + 30 && framesize > 1000) {
                    BNAV_set_frameSize(navEntry, framesize);
                    BNAV_Indexer_completeTimestampFrame( handle );

                    /* record the start of the next frame */
                    BNAV_set_framePts(navEntry, handle->next_pts);
                    BNAV_set_frameOffsetLo(navEntry, offset);
                    BNAV_set_frameOffsetHi(navEntry, offsetHi);
                    handle->picStart = offset;
                }
            }
            break;
        }
        return 0;
    }

    if (handle->seqHdrFlag) {
        if (sc != SC_PES && sc != SC_PTS && sc != SC_EXTENSION) {
            handle->seqHdrFlag = 0;
            handle->seqHdrSize = BNAV_Indexer_subtractByteCounts(offset, handle->seqHdrStartOffset);
        }
    }

    switch(sc)
    {
    case SC_FIRST_SLICE:
        /* I-slice check here */
        if (returnIntraSliceBit(p_curBfr->startCodeBytes) && handle->isHits) {
            BNAV_set_frameType(navEntry, eSCTypeRPFrame);
            handle->isISlice = 1;
        }
        break;

    case SC_PES:
        break;

    case SC_PTS:
        handle->next_pts = BNAV_Indexer_returnPts(p_curBfr);
        break;

    case SC_SEQUENCE:
        handle->seqHdrStartOffset = offset;
        handle->seqHdrFlag = 1; /* set on next qualifying sct */

        /* complete any pending frame */
        handle->picEnd = handle->seqHdrStartOffset;
        handle->fieldEncoded.count = 0;
        BNAV_Indexer_completeFrame( handle );
        break;

    case SC_SEQ_END:   /* TODO: Change me to any non-slice */
        /* complete any pending frame */
        handle->picEnd = offset;
        handle->fieldEncoded.count = 0;
        BNAV_Indexer_completeFrame( handle );
        break;

    case SC_EXTENSION:
        {
        unsigned char payload0;
        payload0 = (p_curBfr->startCodeBytes >> 16) & 0xFF;
        if ((payload0 & 0xF0) == 0x80) { /* extension_start_code_identifier == picture coding extension ID */
            unsigned char field = (p_curBfr->recordByteCountHi >> 16) & 0x03;

            if (field == 0x1 || field == 0x2) {
                handle->fieldEncoded.count++; /* field */
                BDBG_MSG(("field encoded %#x, %d", field, handle->fieldEncoded.count));
            }
            else {
                handle->fieldEncoded.count = 0; /* frame */
            }

            if (handle->fieldEncoded.count == 2 && field == handle->fieldEncoded.lastField && handle->fieldEncoded.skippedPicture) {
                int rc;
                BDBG_MSG(("repeat field means previous skipped SC_PICTURE should have been start of new picture"));
                handle->fieldEncoded.count = 0;
                BNAV_set_frameType(navEntry, eSCTypeUnknown); /* just skip the dangling field */
                rc = BNAV_Indexer_ProcessMPEG2SCT(handle, &handle->fieldEncoded.skippedPictureSct);
                if (rc) return BERR_TRACE(rc);
                handle->fieldEncoded.count = 1;
            }

            handle->fieldEncoded.lastField = field;
        }
        }

        break;

    case SC_PICTURE:
        {
        unsigned picture_coding_type;
        unsigned temporal_reference;
        bool skip_complete = false;

        temporal_reference = (p_curBfr->startCodeBytes >> 14) & 0x3FF;
        picture_coding_type = returnPictureCode(p_curBfr->startCodeBytes);

        if (handle->fieldEncoded.count == 1) {
            /* this picture could be a field pair. evaluate what we can now. evaluate the rest when the next 0xB5 arrives.
            dangling field means temporal_reference changed, or because it's not a B/B or I/I,I/P,P/P pair. 
            see SW7425-2365 for an overview of the decision logic. */
            if (temporal_reference == handle->fieldEncoded.temporal_reference &&
                ((picture_coding_type == PC_B_FRAME) == (handle->fieldEncoded.picture_coding_type == PC_B_FRAME))) {
                /* we've only processed one field of a field encoded picture. skip this one,
                but save the information in case it turns out to be a new picture. */
                handle->fieldEncoded.skippedPicture = true;
                handle->fieldEncoded.skippedPictureSct = *p_curBfr;
                break;
            }
            /* skip the previous field as dangling, but continue indexing this picture. */
            BDBG_MSG(("discard dangling field: %d=%d, %d=%d", temporal_reference, handle->fieldEncoded.temporal_reference, picture_coding_type, handle->fieldEncoded.picture_coding_type));
            handle->fieldEncoded.count = 0;
            skip_complete = true;
        }
        handle->fieldEncoded.skippedPicture = false;
        handle->fieldEncoded.temporal_reference = temporal_reference;
        handle->fieldEncoded.picture_coding_type = picture_coding_type;

        if (!skip_complete) {
            /* complete any pending frame */
            handle->picEnd = offset;
            handle->fieldEncoded.count = 0;
            handle->fieldEncoded.lastField = 0;
            BNAV_Indexer_completeFrame( handle );
        }

        /* start a new frame */
        handle->picStart = offset;

        BNAV_set_frameOffsetLo(navEntry, offset);
        BNAV_set_frameOffsetHi(navEntry, offsetHi);

        switch(picture_coding_type)
        {
        case PC_I_FRAME:
            handle->isISlice = 1;
            handle->isHits = 0;
            BNAV_set_frameType(navEntry, eSCTypeIFrame);
            if (handle->prev_pc == PC_I_FRAME) {
                handle->prev_I_rfo = 0;
            }
            break;
        case PC_P_FRAME:
            handle->prev_I_rfo = 0;
            /* First P after first I allows open GOP B's to be saved */
            if (handle->hitFirstISlice) {
                handle->allowOpenGopBs = 1;
            }
            BNAV_set_frameType(navEntry, eSCTypePFrame);
            break;
        case PC_B_FRAME:
            BNAV_set_frameType(navEntry, eSCTypeBFrame);
            break;
        default:
            handle->parsingErrors++;
            BDBG_ERR(("unknown picture_coding_type 0x%x", picture_coding_type));
            break;
        }
        handle->prev_pc = picture_coding_type;

        /* make sequence header offset relative to current frame rather than */
        /* relative to reference frame to allow removal of b-frames */
        BNAV_set_seqHdrSize(navEntry, (unsigned short)handle->seqHdrSize);
        BNAV_set_seqHdrStartOffset(navEntry,
                    BNAV_Indexer_subtractByteCounts(handle->picStart, handle->seqHdrStartOffset));

        /* Sets the refFrameOffset after adding the prev_I_rfo to the rfo.
        prev_I_rfo will be non-zero ONLY for open gop B's, which are B's that come
        after an I but before a P. */
        BNAV_set_refFrameOffset(navEntry, handle->rfo + handle->prev_I_rfo);

        if (handle->settings.navVersion >= BNAV_VersionOpenGopBs) {
            if (picture_coding_type == PC_I_FRAME) {
                handle->prev_I_rfo = handle->rfo;
            }
        }

        if (handle->hitFirstISlice) {
            handle->rfo++;
        }

        BNAV_set_framePts(navEntry, handle->next_pts);
        }
        break;
    default:
        break;
    }
    return 0;
}

static long BNAV_Indexer_FeedMPEG2(BNAV_Indexer_Handle handle, void *p_bfr, long numEntries)
{
    long i;
    const BSCT_Entry *p_curBfr;
    int inc;

    switch (handle->settings.sctVersion) {
    case BSCT_Version32bitOffset:
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BSCT_Version40bitOffset:
        /* convert 32 bit byte count to 40 bits for venom2 */
        p_curBfr = p_bfr;
        inc = sizeof(BSCT_Entry);
        break;
    case BSCT_Version6wordEntry:
        /* Offseting into BSCT_SixWordEntry gives the same
        as BSCT_Entry. We don't care about the first two words. */
        p_curBfr = CONVERT_TO_SCT4(p_bfr);
        inc = sizeof(BSCT_SixWord_Entry);
        break;
    default:
        return -1;
    }

    for(i=0; i<numEntries; ++i, p_curBfr = (const BSCT_Entry*)((char*)p_curBfr + inc)) {
        (void)BNAV_Indexer_ProcessMPEG2SCT(handle, p_curBfr);
    }

    return i;
}

long BNAV_Indexer_FeedReverse(BNAV_Indexer_Handle handle, const BSCT_Entry *p_bfr, long numEntries)
{
    int i,j;

    if (handle->settings.sctVersion == BSCT_Version32bitOffset) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    for(i=numEntries-1; i>=0; --i)
    {
        const BSCT_Entry *curSct = &p_bfr[i];
        BNAV_Entry *navEntry;

        int sc = returnStartCode(curSct->startCodeBytes); /* parse once */

        BDBG_MSG(("SCT Entry: %08x %08x %08x %08x", curSct->startCodeBytes, curSct->recordByteCount, curSct->recordByteCountHi, curSct->flags));

        /* Set navEntry to the current BNAV_Entry */
        if (handle->reverse.total_entries)
            navEntry = &handle->reverse.entry[handle->reverse.total_entries-1];
        else
            navEntry = NULL;

        switch(sc)
        {
#if 0
/* No HITS support for OTF PVR. Is this a requirement? */
        case SC_FIRST_SLICE:
            /* I-slice check here */
            if (returnIntraSliceBit(curSct->startCodeBytes) && handle->isHits) {
                BNAV_set_frameType(navEntry, eSCTypeRPFrame);
                handle->isISlice = 1;
            }
            break;
        case SC_PES:
        case SC_SEQ_END:
            break;
#endif
        case SC_PTS:
            {
            unsigned long pts = BNAV_Indexer_returnPts(curSct);

            /* Set PTS for all entries that don't have one. */
            for (j=handle->reverse.total_entries-1;j>=0;j--) {
                BNAV_Entry *entry = &handle->reverse.entry[j];

                if (BNAV_get_framePts(entry))
                    break;
                BNAV_set_framePts(entry, pts);
            }
            }
            break;

        case SC_SEQUENCE:
            handle->seqHdrStartOffset = BNAV_Indexer_getScByteOffsetLo( handle, curSct );
            handle->seqHdrSize = handle->picEnd - handle->seqHdrStartOffset;

            /* Go through every entry and set seqhdr offset if not set already */
            for (j=handle->reverse.total_entries-1;j>=0;j--) {
                BNAV_Entry *entry = &handle->reverse.entry[j];

                if (BNAV_get_seqHdrStartOffset(entry))
                    break;

                BNAV_set_seqHdrSize(entry, (unsigned short)handle->seqHdrSize);
                BNAV_set_seqHdrStartOffset(entry,
                    BNAV_Indexer_subtractByteCounts(
                        BNAV_get_frameOffsetLo(entry), handle->seqHdrStartOffset));
            }

            BNAV_Indexer_Flush(handle);
            break;

        case SC_PICTURE:
            {
            unsigned frameSize, picture_coding_type;
            if (!handle->picEnd) {
                /* If we don't know where this frame ends, we have to skip it. */
                break;
            }

            /* Select a new BNAV_Entry */
            if (handle->reverse.total_entries == MAX_GOP_SIZE) {
                BDBG_ERR(("MAX_GOP_SIZE exceeded. Bad data resulting."));
                handle->parsingErrors++;
                return -1;
            }
            navEntry = &handle->reverse.entry[handle->reverse.total_entries++];

            handle->picStart = BNAV_Indexer_getScByteOffsetLo( handle, curSct );
            frameSize = BNAV_Indexer_subtractByteCounts(handle->picEnd, handle->picStart);
            if (frameSize > handle->settings.maxFrameSize) {
                BDBG_WRN(("Giant frame (%d bytes) detected and rejected: %d", frameSize, handle->totalEntriesWritten));
                handle->parsingErrors++;
                /* Throw away this entry */
                handle->reverse.total_entries--;
                continue;
            }

            BNAV_set_frameOffsetLo(navEntry, handle->picStart);
            BNAV_set_frameOffsetHi(navEntry, 0); /* BNAV_Indexer_getScByteOffsetHi(handle, curSct)); */

            BNAV_set_frameSize(navEntry, frameSize);
            BNAV_Indexer_StampEntry(handle, navEntry);

            /* Set this to 0 so that we'll set it when we hit a seqhdr */
            BNAV_set_seqHdrStartOffset(navEntry, 0);
            BNAV_set_refFrameOffset(navEntry, 0);

            picture_coding_type = returnPictureCode(curSct->startCodeBytes);

            switch (picture_coding_type)
            {
            case PC_I_FRAME:
                BNAV_set_frameType(navEntry, eSCTypeIFrame);
                break;
            case PC_P_FRAME:
                BNAV_set_frameType(navEntry, eSCTypePFrame);
                break;
            case PC_B_FRAME:
                BNAV_set_frameType(navEntry, eSCTypeBFrame);
                break;
            default:
                BDBG_ERR(("unknown picture_coding_type 0x%x", picture_coding_type));
                handle->parsingErrors++;
                break;
            }


            /* When we hit an I frame, we can set the refoffset for the GOP */
            if (picture_coding_type == PC_I_FRAME) {
                int refoff = 0; /* Offset from I frame */
                int newgop = 1; /* Set at every I frame */
                int foundFirstP = 0; /* This is the marker for open GOP B's */
                for (j=handle->reverse.total_entries-1;j>=0;j--) {
                    BNAV_Entry *entry = &handle->reverse.entry[j];
                    int done = 0;

                    switch (BNAV_get_frameType(entry)) {
                    case eSCTypeBFrame:
                        /* If this is an open GOP B for the previous frame, skip it. */
                        if (newgop && !foundFirstP) {
                            refoff++;
                            continue;
                        }
                        break;
                    case eSCTypePFrame:
                        /* If we have a new GOP but we've already found a P, then we're
                        done setting the open GOP B's for the current reference frame.
                        We're done. */
                        /* coverity[dead_error_condition] */
                        if (newgop && foundFirstP) {
                            /* coverity[dead_error_begin] */
                            done = 1;
                            break;
                        }

                        /* Once we find a P, we're into the middle of the GOP so don't
                        skip open GOP B's. */
                        newgop = 0;
                        foundFirstP = 1;
                        break;
                    case eSCTypeIFrame:
                        newgop = 1;
                        break;
                    default:
                        BDBG_ERR(("Only GOP-based streams supported."));
                        handle->parsingErrors++;
                        return -1;
                    }

                    /* coverity[dead_error_condition] */
                    if (done)
                        /* coverity[dead_error_line] */
                        break;

                    BNAV_set_refFrameOffset(entry, refoff);
                    refoff++;
                }

                /* And now we can flush */
                BNAV_Indexer_Flush(handle);
            }
            }
            break;

        default:
            break;
        }

        if (sc != SC_PES && sc != SC_PTS && sc != SC_EXTENSION && sc != SC_FIRST_SLICE)
        {
            handle->picEnd = BNAV_Indexer_getScByteOffsetLo( handle, curSct);
            BDBG_MSG(("Set picEnd %lx, %d", handle->picEnd,sc));
        }
    }

    return numEntries;
}

#define BNAV_LARGE_TIME_GAP 3000 /* 3000 msec = 3 seconds */

/**
Set timestamp and vchip stamps for an entry based on current bcmindexer and system state.
**/
void BNAV_Indexer_StampEntry(BNAV_Indexer_Handle handle, BNAV_Entry *entry)
{
    if (handle->settings.simulatedFrameRate) {
        /* calc a timestamp based on a simulated frame rate. actual system time is irrevelevant. */
        BNAV_set_timestamp(entry, handle->starttime++ * (1000/handle->settings.simulatedFrameRate));
    }
    else if (handle->settings.ptsBasedFrameRate) {
        int frameType = BNAV_get_frameType(entry);
        unsigned timestamp;

        /* 
        this algo converts from decode-order PTS to guaranteed-increasing timestamp. 
        it identifies PTS discontinuities. continuous PTS's are converted to timestamps.
        any pictures without new PTS's have interpolated timestamps.
        B frames are always interpolated. */

#define B_PTS_DISCARD_THRESHOLD (10*45000)
        if (frameType != eSCTypeBFrame && 
            (handle->next_pts < handle->ptsBasedFrameRate.lastPts || 
             handle->next_pts > handle->ptsBasedFrameRate.lastPts + B_PTS_DISCARD_THRESHOLD)) 
        {
            /* get across the pts discontinuity. don't change the rate and don't accumulate frameCount. just advance the lastPts.
            allow for infrequently coded PTS's, but bound it to 10 seconds. */
            handle->ptsBasedFrameRate.startPts = handle->next_pts;
            if (handle->lasttime) {
                if (!handle->ptsBasedFrameRate.rate) {
                    handle->ptsBasedFrameRate.rate = 30; /* initial rate. rate must always be non-zero to ensure timestamps are always increasing. */
                }
                timestamp = handle->lasttime + handle->ptsBasedFrameRate.rate;
            }
            else {
                timestamp = 0;
            }
            handle->ptsBasedFrameRate.startTimestamp = timestamp;
            handle->ptsBasedFrameRate.frameCount = 0;
            handle->ptsBasedFrameRate.lastPts = handle->next_pts;
            
            /* leave handle->ptsBasedFrameRate.rate alone */
        }
        else if (frameType != eSCTypeBFrame && handle->next_pts > handle->ptsBasedFrameRate.lastPts) {
            /* set the timestamp based on the pts difference. track it exactly. */
            unsigned span = (handle->next_pts - handle->ptsBasedFrameRate.startPts) / 45;
            
            timestamp = handle->ptsBasedFrameRate.startTimestamp + span;
            if (timestamp <= handle->lasttime) { 
                timestamp = handle->lasttime + 1;
            }
            handle->ptsBasedFrameRate.frameCount++;
            
            /* set rate */
            BDBG_ASSERT(handle->ptsBasedFrameRate.frameCount);
            handle->ptsBasedFrameRate.rate = (timestamp - handle->ptsBasedFrameRate.startTimestamp) / handle->ptsBasedFrameRate.frameCount;
            handle->ptsBasedFrameRate.lastPts = handle->next_pts;
        }
        else {
            unsigned span;
            /* interpolate the PTS based on rate since startPts */
            handle->ptsBasedFrameRate.frameCount++;
            span = handle->ptsBasedFrameRate.frameCount * handle->ptsBasedFrameRate.rate;
            
            timestamp = handle->ptsBasedFrameRate.startTimestamp + span;
            if (timestamp <= handle->lasttime) { 
                timestamp = handle->lasttime + 1;
            }
        }
        
        BDBG_ASSERT((!timestamp && !handle->lasttime) || timestamp > handle->lasttime); /* must be increasing */
        BNAV_set_timestamp(entry, timestamp);
        handle->lasttime = timestamp;
    }
    else {
        unsigned currenttime = currentTimestamp();

        /* provide a minimal safe increment so that timestamps can be unique and guaranteed increasing. */
        if (currenttime <= (unsigned)handle->lasttime) {
            currenttime = handle->lasttime + 15; /* 15 msec increment is less than 60 fps (16.6 msec), so we shouldn't get ahead of real timestamps */
        }

        /* Any large jump in timestamps can only be caused by the source disappearing because of loss of signal, etc.
        The timestamps should skip over this gap. */
        if (!handle->settings.allowLargeTimeGaps && currenttime - handle->lasttime > BNAV_LARGE_TIME_GAP) {
            handle->starttime += (currenttime - handle->lasttime) - 30; /* turn any large gap into a 30 millisecond gap by adjusting the starttime */
        }
        BNAV_set_timestamp(entry, currenttime - handle->starttime + handle->settings.append.timestamp);
        handle->lasttime = currenttime;
    }
    BNAV_set_packed_vchip(entry, handle->vchip);
    BNAV_set_version(entry, handle->settings.navVersion);
}

/* For FeedReverse only. Write out BNAV_Entry's which have a seqhdr and refoffset. */
static int BNAV_Indexer_Flush(BNAV_Indexer_Handle handle)
{
    int i;
    BDBG_MSG(("flush total %d", handle->reverse.total_entries));
    for (i=0;i<handle->reverse.total_entries; i++) {
        BNAV_Entry *entry = &handle->reverse.entry[i];

        BDBG_MSG(("  %ld, %d, %d",
            BNAV_get_seqHdrStartOffset(entry),
            BNAV_get_refFrameOffset(entry),
            BNAV_get_frameType(entry)
            ));

        if (BNAV_get_seqHdrStartOffset(entry) && BNAV_get_refFrameOffset(entry))
        {
           /* write using callback */
            int numBytes = (*handle->settings.writeCallback)(entry, 1,
                sizeof( BNAV_Entry ), handle->settings.filePointer);
            if (numBytes != sizeof( BNAV_Entry )) {
                BDBG_ERR(("Unable to write index entry."));
                return -1;
            }
            handle->totalEntriesWritten++;
            handle->lastEntryWritten = *entry;
        }
        else {
            /* When we hit the first entry we can't send, break out */
            break;
        }
    }

    /* Shift forward the entry array based on how many were sent */
    handle->reverse.total_entries -= i;
    memmove(handle->reverse.entry, &handle->reverse.entry[i],
        handle->reverse.total_entries * sizeof(BNAV_Entry));

    return 0;
}

static int BNAV_P_CheckEntry(BNAV_Indexer_Handle handle, BNAV_Entry *navEntry)
{
    unsigned long frameSize = BNAV_get_frameSize(navEntry);
    int frameType = BNAV_get_frameType(navEntry);

    if (frameSize > handle->settings.maxFrameSize) {
        BDBG_WRN(("Giant frame (%ld bytes) detected and rejected: %d", frameSize, handle->totalEntriesWritten));
        handle->parsingErrors++;
        /* discard everything until next I frame */
        handle->hitFirstISlice = 0;
        handle->rfo = 0;
        handle->isISlice = 0;
        handle->prev_I_rfo = 0;
        handle->prev_pc = 0;
        handle->allowOpenGopBs = 0; /* including open GOP B's after that first I */
        goto fail;
    }
    else if (frameSize == 0) {
        BDBG_ERR(("Invalid frame size 0 rejected, probably corrupt index at %d", handle->totalEntriesWritten));
        handle->parsingErrors++;
        goto fail;
    }
    else if (BNAV_get_seqHdrSize(navEntry) == 0) {
        BDBG_MSG(("Discarding picture with no sequence header"));
        goto fail;
    }

    /* Skip dangling open GOP B's */
    if (frameType == eSCTypeBFrame && handle->hitFirstISlice && !handle->allowOpenGopBs &&
        handle->settings.navVersion != BNAV_Version_AVC &&
        handle->settings.navVersion != BNAV_Version_VC1_PES &&
        handle->settings.navVersion != BNAV_Version_AVS)
    {
        BDBG_MSG(("Discarding dangling open GOP B: %d", handle->totalEntriesWritten));
        handle->rfo--;
        goto fail;
    }

    /* success */
    return 0;

fail:
    BNAV_set_frameType(navEntry, eSCTypeUnknown);
    return -1;
}


/* Write a MPEG2 or AVC entry back to the write callback. */
int BNAV_Indexer_completeFrameAux(BNAV_Indexer_Handle handle, void *data, int size)
{
    BNAV_Entry *navEntry = (BNAV_Entry *)data;
    unsigned long frameSize = BNAV_Indexer_subtractByteCounts( handle->picEnd, BNAV_get_frameOffsetLo(navEntry));
    int frameType = BNAV_get_frameType(navEntry);

    BNAV_set_frameSize(navEntry,frameSize);

    /* perform sanity check before writing */
    if (BNAV_P_CheckEntry(handle, navEntry))
    {
        return -1;
    }


    if (handle->isISlice)
        handle->hitFirstISlice = 1;

    if (frameType != eSCTypeUnknown && handle->hitFirstISlice)
    {
        int numBytes;
        BNAV_Indexer_StampEntry(handle, navEntry);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
		BNAV_set_fifoRecord_flag(navEntry, handle->settings.fifoRecord); /* for playback of delayed record file */	
#endif

        BDBG_MSG(("Output: %08x %08x %08x %08x %08x %08x (%s)", navEntry->words[0], navEntry->words[1],
            navEntry->words[2], navEntry->words[3], navEntry->words[4], navEntry->words[5],
            BNAV_frameTypeStr[frameType]));

        /* write using callback */
        numBytes = (*handle->settings.writeCallback)(navEntry, 1, size, handle->settings.filePointer);
        if (numBytes != size) {
            BDBG_ERR(("Unable to write index entry."));
            return -1;
        }
        handle->totalEntriesWritten++;
        handle->lastEntryWritten = *navEntry;
    }
    /**
    * Clear the frametype, but don't clear the rest of the information. If
    * in the future the rest of the information must be cleared, watchout so that
    * BNAV_Indexer_GetPosition isn't broken.
    **/
    BNAV_set_frameType(navEntry, eSCTypeUnknown);

    if(handle->isISlice)
    {
        handle->rfo = 1;
        handle->isISlice = 0;
    }
    handle->picEnd = 0;

    return 0;
}

/* Write completed MPEG2 entry to write callback. */
static int BNAV_Indexer_completeFrame(BNAV_Indexer_Handle handle)
{
    return BNAV_Indexer_completeFrameAux(handle, &handle->curEntry, sizeof(handle->curEntry));
}
/* Write completed AVC entry to write callback. */
static int BNAV_Indexer_completeAVCFrame(BNAV_Indexer_Handle handle)
{
    BNAV_AVC_Entry *avcEntry = &handle->avcEntry;

	HDBG_PrintLog(3,("%s(%d)\n", __func__, __LINE__));	
    /* We're going to change the definition of P and B frame for AVC. For MPEG2, a B picture has two
    properties: it uses bidirectional prediction (forward and backward in display order, always backward in decode order)
    and it is not predicted against. The player only uses the second property. It can drop a B frame.
    For AVC, a B picture is has the first property but not the second. At this point, it's unclear if the player
    can use the B directional property for any trick mode (especially if not stored at a slice level).
    Therefore, for the NAV table AVC indexes, the definition of "B frame" will be "non-I frame which is not
    a referenced picture." This means B frames will have the second property - they are discardable. */
    if (BNAV_get_frameType(avcEntry) == eSCTypeBFrame) {
        if (handle->avc.is_reference_picture) {
            BDBG_MSG(("We have a reference B frame"));
            BNAV_set_frameType(avcEntry, eSCTypePFrame);
        }
    }
    else if (BNAV_get_frameType(avcEntry) == eSCTypePFrame) {
        if (!handle->avc.is_reference_picture) {
            BDBG_MSG(("We have a non-reference P frame"));
            BNAV_set_frameType(avcEntry, eSCTypeBFrame);
        }
    }

    return BNAV_Indexer_completeFrameAux(handle, &handle->avcEntry, sizeof(handle->avcEntry));
}

static int BNAV_Indexer_completeTimestampFrame(BNAV_Indexer_Handle handle)
{
    BNAV_Entry *navEntry = &handle->curEntry;
    unsigned size = sizeof(handle->curEntry);
    int numBytes;

    BNAV_Indexer_StampEntry(handle, navEntry);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    BNAV_set_fifoRecord_flag(navEntry, handle->settings.fifoRecord);
#endif	

    BDBG_MSG(("Output: %08x %08x %08x %08x %08x %08x", navEntry->words[0], navEntry->words[1],
        navEntry->words[2], navEntry->words[3], navEntry->words[4], navEntry->words[5]));

    /* write using callback */
    numBytes = (*handle->settings.writeCallback)(navEntry, 1, size, handle->settings.filePointer);
    if (numBytes != (int)size) {
        BDBG_ERR(("Unable to write index entry."));
        return -1;
    }
    handle->totalEntriesWritten++;
    handle->lastEntryWritten = *navEntry;

    return 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ INPUTS:   p_sct = pointer to sct entry
+ OUTPUTS:  None.
+ RETURNS:  pts value (bits [32:1]
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned long BNAV_Indexer_returnPts(const BSCT_Entry *p_sct )
{
    return p_sct->recordByteCountHi;
}

/******************************************************************************
* INPUTS:   p_entry = pointer to a start code entry
* OUTPUTS:  None.
* RETURNS:  Lower 32 bits of the start code byte offset.
* FUNCTION: This function returns the lower 32 bits of of the number obtained
*           by adding the byte offset of the packet to the offset within the
*           packet of the start code.
******************************************************************************/
unsigned long BNAV_Indexer_getScByteOffsetLo(BNAV_Indexer_Handle handle, const BSCT_Entry *p_entry)
{
    unsigned long byteOffset;

    byteOffset = p_entry->recordByteCount + (p_entry->startCodeBytes & 0xff);
    if (handle->settings.transportTimestampEnabled)
        byteOffset += 4;

    return byteOffset;
}

/******************************************************************************
* INPUTS:   p_entry = pointer to a start code entry
* OUTPUTS:  None.
* RETURNS:  most significant 8 bites of the 40 bit offset, shifted into lower 8 bits
* FUNCTION: This function takes into account the start code offset and possible timestamp and increments the Hi once
*
******************************************************************************/
unsigned long BNAV_Indexer_getScByteOffsetHi(BNAV_Indexer_Handle handle, const BSCT_Entry *p_entry )
{
    unsigned long byteOffset;

    /* Offset into transport packet may cause 32 bit overflow. */
    byteOffset = p_entry->recordByteCount + (p_entry->startCodeBytes & 0xff);
    if (handle->settings.transportTimestampEnabled)
        byteOffset += 4;
    if (byteOffset < p_entry->recordByteCount)
    {
        return (p_entry->recordByteCountHi>>24) + 1;
    }
    else
    {
        return p_entry->recordByteCountHi>>24;
    }
}

/******************************************************************************
* INPUTS:   largerNum - Number to subtract from.
*           smallerNum - Number to be subtracted.
* OUTPUTS:  None.
* RETURNS:  Difference between 2 numbers.
* FUNCTION: This function subtracts largerNum from smallerNum.  It assumes that
*           largerNum is larger and if not, it must have wrapped.
*           This function is used for calculating the difference between 64-bit
*           byte counts using only 32-bit numbers and assuming that the
*           difference between the two number can be contained in a 32-bit
*           unsigned integer.
******************************************************************************/
unsigned long BNAV_Indexer_subtractByteCounts( unsigned long largerNum, unsigned long smallerNum )
{
    if (largerNum >= smallerNum)
    {
        return largerNum - smallerNum;
    }
    else
    {
        return (0 - smallerNum) + largerNum;
    }
}

const char * const BNAV_frameTypeStr[eSCTypeUnknown+1] = {
    "unused", /* eSCTypeSeqHdr*/
    "I",      /* eSCTypeIFrame    I-frame */
    "P",      /* eSCTypePFrame    P-frame */
    "B",      /* eSCTypeBFrame    B-frame */
    "unused", /* eSCTypeGOPHdr    GOP header */
    "RP",     /* eSCTypeRPFrame   Reference picture frame */
    "Unk"    /* eSCTypeUnknown   Unknown or "don't care" frame type */
};

int BNAV_Indexer_IsHits(BNAV_Indexer_Handle handle) {
    return handle->isHits;
}

int BNAV_Indexer_SetVChipState(BNAV_Indexer_Handle handle, unsigned short vchipState)
{
    return BNAV_pack_vchip(vchipState, &handle->vchip);
}

unsigned short BNAV_Indexer_GetVChipState(BNAV_Indexer_Handle handle)
{
    return BNAV_unpack_vchip(handle->vchip);
}

unsigned short BNAV_unpack_vchip(unsigned short packed_vchip)
{
    /* unpack from 12 bits */
    packed_vchip = ((packed_vchip << 2) & 0x3f00) | (packed_vchip & 0x003f);
    /* reset the 1 bits & return */
    return packed_vchip | 0x4040;
}

int BNAV_pack_vchip(unsigned short unpacked_vchip, unsigned short *packed_vchip)
{
    /* test the 1 bit in each byte */
    if ((unpacked_vchip & 0x4040) != 0x4040) {
        BDBG_ERR(("Invalid vchip value: 0x%04x", unpacked_vchip));
        return -1;
    }
    /* pack into 12 bits */
    *packed_vchip = ((unpacked_vchip & 0x3f00)>>2) | (unpacked_vchip & 0x003f);
    return 0;
}

int BNAV_Indexer_GetPosition(BNAV_Indexer_Handle handle, BNAV_Indexer_Position *position)
{
    if (!handle->totalEntriesWritten)
        return -1;
    else {
        BNAV_Entry *p_entry = &handle->lastEntryWritten;
        switch(handle->settings.videoFormat) {
        case BNAV_Indexer_VideoFormat_AVC_MVC:
        case BNAV_Indexer_VideoFormat_AVC_SVC:
            return BNAV_Indexer_FeedAVC_GetPosition(handle, position);
        default:
            position->index = handle->totalEntriesWritten-1;
            position->pts = BNAV_get_framePts(p_entry);
            position->offsetHi = BNAV_get_frameOffsetHi(p_entry);
            position->offsetLo = BNAV_get_frameOffsetLo(p_entry);
            position->timestamp = BNAV_get_timestamp(p_entry);
            position->vchipState = BNAV_unpack_vchip(BNAV_get_packed_vchip(p_entry));
            return 0;
        }
    }
}

/* low level bit manipulation */
/*
Summary:
Returns bit 'b' from word 'w',

Example:
    B_GET_BIT(0xDE,1)==0
    B_GET_BIT(0xDE,7)!=0
*/
#define B_GET_BIT(w,b)  ((w)&(1<<(b)))


/*
Summary:
Returns bits 'e'..'b' from word 'w',

Example:
    B_GET_BITS(0xDE,7,4)==0x0D
    B_GET_BITS(0xDE,3,0)=0x0E
*/
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

/*
BNAV_Indexer_P_ProcessFullPacketForFeedAvc takes a TS packet and generates SCT entries, just like the SCD hardware.
It must generate 3 SCT types: RAI, start code, PTS.
*/
static long BNAV_Indexer_P_ProcessFullPacket(BNAV_Indexer_Handle handle, long (*feed)(BNAV_Indexer_Handle , const void *, long ))
{
    unsigned offset = 4;
    bool error=false;
    uint8_t *pkt = handle->fullPacket + 4;
    BSCT_SixWord_Entry sct;
    uint32_t scode;
    long rc;

    BDBG_MSG_TRACE(("BNAV_Indexer_P_ProcessFullPacketForFeedAvc"));
#if 0
    {
        unsigned i;
        for (i=0;i<BCMINDEXER_TS_PACKET_SIZE;i++) {
            BKNI_Printf("%02x ", handle->fullPacket[i]);
            if((i+1)%16 == 0) {
                BKNI_Printf("\n");
            }
        }
        BKNI_Printf("\n");
    }
#endif
    BDBG_MSG_TRACE(("adaptation_field_control:%u", handle->adaptFieldControl));
    switch (handle->adaptFieldControl) {
    case 0x01:
        break;
    case 0x03:
        {
            unsigned adaptation_field_length = pkt[0];
            BDBG_MSG_TRACE(("adaptation_field_length:%u", pkt[0]));
            adaptation_field_length ++;
            offset += adaptation_field_length;
            pkt += adaptation_field_length;
            if(offset>=BCMINDEXER_TS_PACKET_SIZE-8) {
                goto done;
            }
        }
#if 0
        {
            unsigned i;
            for (i=0;i<BCMINDEXER_TS_PACKET_SIZE-offset;i++) {
                BKNI_Printf("%02x ", pkt[i]);
                if((i+1)%16 == 0) {
                    BKNI_Printf("\n");
                }
            }
            BKNI_Printf("\n");
        }
#endif
        break;
    default:
        goto done;
    }

    if(pkt[0]==0x00 && pkt[1]==0x00 && pkt[2]==0x01 && (pkt[3]&0xF0) == 0xE0) { /* PES video header */
        if (B_GET_BITS(pkt[7], 7, 6) & 2) {
            uint32_t pts =
             /* pts_32_30 */((uint32_t)B_GET_BITS(pkt[9], 3, 1)<<29) |
             /* pts_29_22 */((uint32_t)pkt[10]<<21) |
             /* pts_21_15 */((uint32_t)B_GET_BITS(pkt[11], 7, 1)<<14) |
             /* pts_14_7 */((uint32_t)pkt[12]<<6) |
             /* pts_6_0 */B_GET_BITS(pkt[13], 7, 2);
            BKNI_Memset(&sct, 0, sizeof(sct));
            sct.startCodeBytes = 0xFE << 24;
            sct.recordByteCountHi = pts;
            sct.recordByteCount = handle->lastPacketOffset.offset; /* lower 32 bits of packet offset */
            BDBG_MSG_TRACE(("  sending PTS SCT %u", pts));
            rc = feed(handle, &sct, 1);
            if(rc<0) { error=true;}
        }
        /* skip through PES start code */
        offset += 4;
        pkt += 4;
    }
    /* Find start codes and build SCT packets.
    The loop test is "<=" so that we can process any remainder. */
    for (scode=0xFF;offset<=BCMINDEXER_TS_PACKET_SIZE-8;offset++) { /* we need to capture 8 bytes of payload, so we should stop 8 bytes prior to the end of packet */
        scode = (scode<<8) | *pkt;
        pkt++;
        if((scode>>8) != 0x01) {
            continue;
        }
        BKNI_Memset(&sct, 0, sizeof(sct));
        BDBG_MSG_TRACE(("found scode at %u: %02x %02x %02x %02x %02x %02x %02x %02x %02x",  offset-3, scode&0xFF, pkt[0], pkt[1], pkt[2], pkt[3], pkt[4], pkt[5], pkt[6], pkt[7]));
        /* now we can finish out previous SCT. note that payload[] may extend past the next start code. this is how
        the SCD HW works as well. */
        sct.startCodeBytes = (scode&0xFF) << 24;
        scode = 0xFF;
        sct.startCodeBytes |= pkt[0] << 16;
        sct.startCodeBytes |= pkt[1] << 8;
        sct.recordByteCountHi |= pkt[2] << 16;
        sct.recordByteCountHi |= pkt[3] << 8;
        sct.recordByteCountHi |= pkt[4];
        sct.flags |= pkt[5] << 16;
        sct.flags |= pkt[6] << 8;
        sct.flags |= pkt[7];

        /* set offset */
        sct.startCodeBytes |= offset - 3; /* byte offset from the start of the packet to the start code */
        sct.recordByteCount = handle->lastPacketOffset.offset; /* lower 32 bits of packet offset */
        sct.recordByteCountHi |= handle->lastPacketOffset.offsetHi << 24; /* upper 8 bits of packet offset */

        BDBG_MSG_TRACE(("  sending SC SCT"));
        rc = feed(handle, &sct, 1);
        if(rc<0) { error=true;}
    }
done:
    return error?-1:0;
}

int
BNAV_Indexer_P_AddFullPacketPayload(BNAV_Indexer_Handle handle, const BSCT_SixWord_Entry *next)
{
    unsigned offset = handle->fullPacketCount;
    uint8_t *packet;

    if(offset<4 || offset>BCMINDEXER_TS_PACKET_SIZE-23) {
        BDBG_WRN(("out of order full packet SCT (%u:%u)", offset, BCMINDEXER_TS_PACKET_SIZE-23));
        handle->parsingErrors++;
        return -1;
    }
    packet = handle->fullPacket+offset;
    packet[0] = B_GET_BITS(next->word0, 23, 16);
    packet[1] = B_GET_BITS(next->word0, 15, 8);
    packet[2] = B_GET_BITS(next->word0, 7, 0);

    packet[3] = B_GET_BITS(next->word1, 31, 24);
    packet[4] = B_GET_BITS(next->word1, 23, 16);
    packet[5] = B_GET_BITS(next->word1, 15, 8);
    packet[6] = B_GET_BITS(next->word1, 7, 0);

    packet[7] = B_GET_BITS(next->startCodeBytes, 31, 24);
    packet[8] = B_GET_BITS(next->startCodeBytes, 23, 16);
    packet[9] = B_GET_BITS(next->startCodeBytes, 15, 8);
    packet[10] = B_GET_BITS(next->startCodeBytes, 7, 0);

    packet[11] = B_GET_BITS(next->recordByteCount, 31, 24);
    packet[12] = B_GET_BITS(next->recordByteCount, 23, 16);
    packet[13] = B_GET_BITS(next->recordByteCount, 15, 8);
    packet[14] = B_GET_BITS(next->recordByteCount, 7, 0);

    packet[15] = B_GET_BITS(next->recordByteCountHi, 31, 24);
    packet[16] = B_GET_BITS(next->recordByteCountHi, 23, 16);
    packet[17] = B_GET_BITS(next->recordByteCountHi, 15, 8);
    packet[18] = B_GET_BITS(next->recordByteCountHi, 7, 0);

    packet[19] = B_GET_BITS(next->flags, 31, 24);
    packet[20] = B_GET_BITS(next->flags, 23, 16);
    packet[21] = B_GET_BITS(next->flags, 15, 8);
    packet[22] = B_GET_BITS(next->flags, 7, 0);
    offset+= 23;
    handle->fullPacketCount = offset;
    return 0;
}

static long
BNAV_Indexer_P_FilterFullPacket(BNAV_Indexer_Handle handle, const BSCT_SixWord_Entry *sct, long numEntries, long (*feed)(BNAV_Indexer_Handle , const void *, long ))
{
    long i;
    long rc;
    bool error=false;
    const BSCT_SixWord_Entry *next;

    /* filter and process  sequence of full packet SCT entries */
	HDBG_PrintLog(3,("%s(%d)\n", __func__, __LINE__));	
    BDBG_MSG_TRACE(("processing %u entries from %#lx", numEntries, (unsigned long)sct));
    for(next=sct,i=0;i<numEntries; i++,next++) {
        unsigned type = next->word0>>24;
        /* BDBG_MSG_TRACE(("%u:type %#x", i, type)); */
        if(type==0x80) {
            BDBG_MSG_TRACE(("%u:Full Packet offset %u", i, handle->fullPacketCount));
            if(next!=sct) {
                BDBG_MSG_TRACE(("%u:sending %u middle entries from %#lx", i, next-sct, (unsigned long)sct));
                rc = feed(handle, sct, next-sct);
                if(rc<0) {return rc;}
                sct = next;
            }
            sct++;
            if(BNAV_Indexer_P_AddFullPacketPayload(handle, next)!=0) {
                continue;
            }
            if(handle->fullPacketCount == BCMINDEXER_TS_PACKET_SIZE) {
                handle->fullPacketCount = 0;
                rc = BNAV_Indexer_P_ProcessFullPacket(handle, feed);
                if(rc!=0) {error=true;}
            }
        } else {
            if(type==0x01 && B_GET_BIT(next->startCodeBytes, 22)) {
                BDBG_MSG_TRACE(("SCT PAYLOAD_TO_FOLLOW"));
                handle->lastPacketOffset.offset = next->recordByteCount;
                handle->lastPacketOffset.offsetHi = next->startCodeBytes >> 24;

                handle->fullPacketCount = 4;
                handle->fullPacket[0] = 0;
                handle->fullPacket[1] = 0;
                handle->fullPacket[2] = 0;
                handle->fullPacket[3] = 0;
                handle->adaptFieldControl = B_GET_BITS(next->word0, 5, 4);
            } else if(type==0) {
                /* do nothing, SC entries could be between TPIT and RAW payload */
            } else {
                handle->fullPacketCount = 0;
            }
        }
    }
    if(next >= sct+1) {
        HDBG_PrintLog(3,("sending %u last entries from %#lx\n",next-sct, (unsigned long)sct));
        BDBG_MSG_TRACE(("sending %u last entries from %#lx", next-sct, (unsigned long)sct));
        rc = feed(handle, sct, next-sct);
        if(rc<0) {return rc;}
    }
    return error?-1:i;
}


static long
BNAV_Indexer_FeedAVC(BNAV_Indexer_Handle handle, const void *p_bfr, long numEntries)
{
    long i;
    const BSCT_SixWord_Entry *p_curBfr;
    BNAV_AVC_Entry *avcEntry = &handle->avcEntry;

    if (handle->settings.sctVersion != BSCT_Version6wordEntry) {
        BDBG_ERR(("Must use 6 word SCT's for AVC content"));
        return -1;
    }
	HDBG_PrintLog(3,("%s(%d)\n", __func__, __LINE__));	
    /* no AVC hits */
    handle->isHits = 0;

    p_curBfr = (const BSCT_SixWord_Entry *)p_bfr;

    for(i=0; i<numEntries; i++, p_curBfr++)
    {
        unsigned char sc = returnStartCode(p_curBfr->startCodeBytes); /* parse once */
        unsigned long offset, offsetHi;
        unsigned nal_unit_type;
        unsigned nal_ref_idc;
#define TOTAL_PAYLOAD 8
        unsigned char payload[TOTAL_PAYLOAD];
        unsigned index = 0, bit = 7;
        BSCT_Entry *p_curSct4 = CONVERT_TO_SCT4(p_curBfr);
        unsigned type = p_curBfr->word0>>24;

        BDBG_MSG(("SCT %08x %08x %08x %08x %08x %08x",
            ((uint32_t*)p_curBfr)[0],
            ((uint32_t*)p_curBfr)[1],
            ((uint32_t*)p_curBfr)[2],
            ((uint32_t*)p_curBfr)[3],
            ((uint32_t*)p_curBfr)[4],
            ((uint32_t*)p_curBfr)[5]));

        /* Check for "full packet" entry. See bcmindexerpriv.h for comment. */

        /* Check for TPIT Entry to extract RAI. discard all other TPIT entries. */
        if (type == 1)
        {
            if ((p_curBfr->startCodeBytes & 0x20)>>5)
            {
                handle->random_access_indicator = true;
            }
            continue;
        }


        /* Grab the PTS */
        if (sc == 0xfe) {
            handle->next_pts = BNAV_Indexer_returnPts(p_curSct4);
            BDBG_MSG_TRACE(("PTS %08lx", handle->next_pts));
            continue;
        }
        if (sc & 0x80) {
            /* if forbidden_zero_bit is set, this is not an AVC start code */
            continue;
        }

        offset = BNAV_Indexer_getScByteOffsetLo( handle, p_curSct4 );
        offsetHi = BNAV_Indexer_getScByteOffsetHi(handle, p_curSct4);

        BNAV_Indexer_P_AddAppend(handle, &offsetHi, &offset);

        VALIDATE_AVC_SC(handle, p_curSct4,sc);

        BDBG_MSG(("sc %02x at %#lx", sc, offset));
        nal_ref_idc = (sc >> 5) & 0x3;
        nal_unit_type = sc & 0x1F;

        /* extract 8 bytes of payload from BSCT_SixWord_Entry fields. see RDB for documentation on this. */
        payload[0] = (p_curBfr->startCodeBytes >> 16) & 0xFF;
        payload[1] = (p_curBfr->startCodeBytes >> 8) & 0xFF;
        payload[2] = (p_curBfr->recordByteCountHi >> 16) & 0xFF;
        payload[3] = (p_curBfr->recordByteCountHi >> 8) & 0xFF;
        payload[4] = p_curBfr->recordByteCountHi & 0xFF;
        payload[5] = (p_curBfr->flags >> 16) & 0xFF;
        payload[6] = (p_curBfr->flags >> 8) & 0xFF;
        payload[7] = p_curBfr->flags & 0xFF;

        BDBG_MSG(("payload %02x%02x%02x%02x%02x%02x%02x%02x",
            payload[0],payload[1],payload[2],payload[3],payload[4],payload[5],payload[6],payload[7]));

        /* complete pending PPS or SPS because we've hit the next NAL */
        if (handle->avc.current_pps >= 0) {
            /* complete the PPS */
            int id = handle->avc.current_pps;
            handle->avc.pps[id].size = BNAV_Indexer_subtractByteCounts(
                offset, handle->avc.pps[id].offset);
            handle->avc.current_pps = -1;
        }
        else if (handle->avc.current_sps >= 0) {
            /* complete the SPS */
            int id = handle->avc.current_sps;
            handle->avc.sps[id].size = BNAV_Indexer_subtractByteCounts(
                offset, handle->avc.sps[id].offset);
            handle->avc.current_sps = -1;
        }

        /* We must call BNAV_Indexer_completeAVCFrame UNLESS we have IDR/non-IDR slice
        with first_mb_in_slice != 0. So handle all the "other" cases in one spot. */
        if (nal_unit_type != 1 && nal_unit_type != 5 && nal_unit_type != 14 ) {
            handle->picEnd = offset;
            if (BNAV_get_frameType(avcEntry) == eSCTypeIFrame)
                handle->prev_I_rfo = handle->rfo;
            BNAV_Indexer_completeAVCFrame( handle );
        }

        /* if SEI and preceding was not an SEI, remember this offset for start point of next PPS, SPS or picture */
        if (nal_unit_type == 6 && !handle->avc.current_sei_valid) {
            handle->avc.current_sei_valid = true;
            handle->avc.current_sei = offset;
        }

        switch (nal_unit_type) {
        case 1: /* non-IDR slice */
        case 5: /* IDR slice */
            {
            unsigned first_mb_in_slice, slice_type, pic_parameter_set_id;
            unsigned sps_id;

            /* vlc decode the payload */
            first_mb_in_slice = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);
            slice_type = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);
            pic_parameter_set_id = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);

            /* check the pps. anything above this is an invalid AVC stream. */
            if (pic_parameter_set_id >= TOTAL_PPS_ID) {
                BDBG_ERR(("pic_parameter_set_id %d", pic_parameter_set_id));
                handle->parsingErrors++;
                return -1;
            }
            /* check the sps right away. if we use a PPS without receiving it's SPS first, we should discard. */
            sps_id = handle->avc.pps[pic_parameter_set_id].sps_id;
            if (sps_id >= TOTAL_SPS_ID) {
                BDBG_ERR(("seq_parameter_set_id %d from pps %d", sps_id, pic_parameter_set_id));
                handle->parsingErrors++;
                return -1;
            }

            BDBG_MSG(("%s slice: mb=%-4d type=%-2d pps=%-3d offset=%#lx",
                (nal_unit_type==1)?"non-IDR":"IDR", first_mb_in_slice, slice_type, pic_parameter_set_id, offset));

            if (first_mb_in_slice == 0) {
                /* we have the beginning of a new frame. first, complete the previous frame. */
                handle->picEnd = offset;
                if (BNAV_get_frameType(avcEntry) == eSCTypeIFrame)
                    handle->prev_I_rfo = handle->rfo;
                BNAV_Indexer_completeAVCFrame( handle );

                /* start the next frame */
                handle->picStart = handle->avc.current_sei_valid?handle->avc.current_sei:offset; /* start with preceding SEI's if present */

                BNAV_set_frameOffsetLo(avcEntry, handle->picStart);
                BNAV_set_frameOffsetHi(avcEntry, offsetHi);

                /* default to I frame until P or B slices indicate differently */
                BNAV_set_frameType(avcEntry, eSCTypeIFrame);
                handle->isISlice = 1;
                handle->avc.is_reference_picture = 0;

                /* Set PPS and SPS */
                BNAV_set_seqHdrStartOffset(avcEntry,
                    BNAV_Indexer_subtractByteCounts(handle->picStart, handle->avc.pps[pic_parameter_set_id].offset));
                BNAV_set_seqHdrSize(avcEntry,
                    handle->avc.pps[pic_parameter_set_id].size);
                BNAV_set_SPS_Offset(avcEntry,
                    BNAV_Indexer_subtractByteCounts(handle->picStart, handle->avc.sps[sps_id].offset));
                BNAV_set_SPS_Size(avcEntry,
                    handle->avc.sps[sps_id].size);

                BDBG_MSG(("sps %d: %#lx, %ld", sps_id, BNAV_get_SPS_Offset(avcEntry), BNAV_get_SPS_Size(avcEntry)));

                /* check for P slice, which means it's not a B frame */
                if (slice_type == 0 || slice_type == 5) {
                    handle->prev_I_rfo = 0;
                    /* First P after first I allows open GOP B's to be saved */
                    if (handle->hitFirstISlice)
                        handle->allowOpenGopBs = 1;
                }

                /* Sets the refFrameOffset after adding the prev_I_rfo to the rfo.
                prev_I_rfo will be non-zero ONLY for open gop B's, which are B's that come
                after an I but before a P. */
                BDBG_MSG(("BNAV_set_refFrameOffset %d, %d", handle->rfo, handle->prev_I_rfo));
                BNAV_set_refFrameOffset(avcEntry, handle->rfo + handle->prev_I_rfo);

                if (handle->hitFirstISlice) {
                    handle->rfo++;
                }

                BNAV_set_framePts(avcEntry, handle->next_pts);

                BNAV_set_RandomAccessIndicator(avcEntry, handle->random_access_indicator);
                handle->random_access_indicator = false;
            }

            if (nal_unit_type == 5 && (slice_type !=2 && slice_type != 7)) {
                BDBG_ERR(("IDR frame with non-I slices"));
                handle->parsingErrors++;
                return -1;
            }

            /* test if the slice is a referenced by another slice */
            if (nal_ref_idc) {
                handle->avc.is_reference_picture = 1;
            }

            /* test every slice to determine frame type */
            switch (slice_type) {
            case 2:
            case 7:
                /* we've already defaulted to I frame */
                break;
            case 0:
            case 5:
                /* if we ever get one P slice, then it's either a P or B frame, cannot be I */
                if (BNAV_get_frameType(avcEntry) == eSCTypeIFrame) {
                    handle->isISlice = 0;
                    BNAV_set_frameType(avcEntry, eSCTypePFrame);
                }
                break;
            case 1:
            case 6:
                /* if we ever get one B slice, it's a B frame */
                handle->isISlice = 0;
                BNAV_set_frameType(avcEntry, eSCTypeBFrame);
                break;
            default:
                BDBG_ERR(("unsupported slice_type %d", slice_type));
                handle->parsingErrors++;
                break;
            }
            }
            break;
        case 7: /* sequence parameter set */
            {
            unsigned profile_idc, constraint_flags, level_idc, seq_parameter_set_id;

            /* parse and vlc decode the payload */
            profile_idc = payload[0];
            constraint_flags = payload[1];
            level_idc = payload[2];
            index = 3;
            seq_parameter_set_id = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);
            if (seq_parameter_set_id >= TOTAL_SPS_ID) {
                BDBG_ERR(("corrupt seq_parameter_set_id %u", seq_parameter_set_id));
                handle->parsingErrors++;
                return -1;
            }
            handle->avc.sps[seq_parameter_set_id].offset = handle->avc.current_sei_valid?handle->avc.current_sei:offset; /* start with preceding SEI's if present */
            handle->avc.current_sps = seq_parameter_set_id;

            BDBG_MSG(("SeqParamSet: profile_idc=%u flags=0x%x level_idc=%d SPS=%d offset=%#lx",
                profile_idc, constraint_flags, level_idc, seq_parameter_set_id, offset));
            }
            break;
        case 8: /* picture parameter set */
            {
            unsigned pic_parameter_set_id, seq_parameter_set_id;

            /* vlc decode payload */
            pic_parameter_set_id = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);
            if (pic_parameter_set_id >= TOTAL_PPS_ID) {
                BDBG_ERR(("corrupt pic_parameter_set_id %u", pic_parameter_set_id));
                handle->parsingErrors++;
                return -1;
            }
            seq_parameter_set_id = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);
            if (seq_parameter_set_id >= TOTAL_SPS_ID) {
                BDBG_ERR(("corrupt seq_parameter_set_id %u", seq_parameter_set_id));
                handle->parsingErrors++;
                return -1;
            }

            handle->avc.pps[pic_parameter_set_id].offset = handle->avc.current_sei_valid?handle->avc.current_sei:offset; /* start with preceding SEI's if present */
            handle->avc.pps[pic_parameter_set_id].sps_id = seq_parameter_set_id;
            handle->avc.current_pps = pic_parameter_set_id;

            BDBG_MSG(("PicParamSet: PPS=%d, SPS=%d offset=%#lx",
                pic_parameter_set_id, seq_parameter_set_id, offset));
            }
            break;

#if 0
/* This code is optional because we are detecting the picture type by examining each slice. */
        case 9: /* access unit delimiter */
            {
#if BDBG_DEBUG_BUILD
            static const char *primary_pic_type_str[] = {
                "I",
                "I,P",
                "I,P,B",
                "SI",
                "SI,SP",
                "I,SI",
                "I,SI,P,SP",
                "I,SI,P,SP,B"
            };
#endif
            int primary_pic_type;
            primary_pic_type = (payload[0] >> 5) & 0x3;

            BDBG_MSG(("AUD %d (%s)", primary_pic_type, primary_pic_type_str[primary_pic_type]));
            }
            break;
#endif
        }

        /* after any non-SEI, the current_sei is no longer valid */
        if (nal_unit_type != 6) {
            handle->avc.current_sei_valid = false;
        }
    }

    return i;
}

static long
BNAV_Indexer_FeedAVS(BNAV_Indexer_Handle handle, void *p_bfr, long numEntries)
{
    long i;
    const BSCT_Entry *p_curBfr=NULL;
    const BSCT_SixWord_Entry *p_cur6WordBfr=NULL;
    BNAV_Entry *navEntry = &handle->curEntry;

    if (handle->settings.sctVersion != BSCT_Version6wordEntry)
    {
        BDBG_ERR(("Must use 6 word SCT's for AVC content"));
        return -1;
    }
    p_cur6WordBfr = (const BSCT_SixWord_Entry *)p_bfr;

    for (i=0; i<numEntries; i++,p_cur6WordBfr++)
    {
        int sc;
        unsigned long offset, offsetHi;

        p_curBfr = CONVERT_TO_SCT4(p_cur6WordBfr);

        /* Check for TPIT Entry to extract RAI. discard all other TPIT entries. */
        if ((p_cur6WordBfr->word0>>24) == 1)
        {
            if ((p_curBfr->startCodeBytes & 0x20)>>5)
            {
                handle->random_access_indicator = true;
            }
            continue;
        }

        sc = returnStartCode(p_curBfr->startCodeBytes); /* parse once */
        offset = BNAV_Indexer_getScByteOffsetLo(handle, p_curBfr);
        offsetHi = BNAV_Indexer_getScByteOffsetHi(handle, p_curBfr);

        BNAV_Indexer_P_AddAppend(handle, &offsetHi, &offset);

        /* detect invalid start code and offsets */
        VALIDATE_AVS_SC(handle, p_curBfr,sc);

        BDBG_MSG(("AVS SCT Entry: %02x 0x%02x%08x", sc,
                  (p_curBfr->recordByteCountHi>>24) & 0xFF, p_curBfr->recordByteCount));
        if (handle->seqHdrFlag)
        {
            if (sc != SC_AVS_PES && sc != SC_AVS_PTS && sc != SC_AVS_EXTENSION && sc != SC_AVS_USER_DATA)
            {
                handle->seqHdrFlag = 0;
                handle->seqHdrSize = BNAV_Indexer_subtractByteCounts(offset, handle->seqHdrStartOffset);
            }
        }

        switch (sc)
        {
        case SC_AVS_FIRST_SLICE:
            break;

        case SC_AVS_PES:
            break;

        case SC_AVS_PTS:
            handle->next_pts = BNAV_Indexer_returnPts(p_curBfr);
            break;

        case SC_AVS_SEQUENCE:
            handle->seqHdrStartOffset = offset;
            handle->seqHdrFlag = 1; /* set on next qualifying sct */

            /* new complete any pending frame */
            handle->picEnd = handle->seqHdrStartOffset;
            BNAV_Indexer_completeFrame( handle );
            break;

        case SC_AVS_SEQ_END:   /* TODO: Change me to any non-slice */
            /* complete any pending frame */
            handle->picEnd = offset;
            BNAV_Indexer_completeFrame( handle );
            break;

        case SC_AVS_PICTURE_I:
        case SC_AVS_PICTURE_PB:
            /* complete any pending frame */
            handle->picEnd = offset;
            BNAV_Indexer_completeFrame( handle );

            /* start a new frame */
            handle->picStart = offset;

            BNAV_set_frameOffsetLo(navEntry, offset);
            BNAV_set_frameOffsetHi(navEntry, offsetHi);

            if (sc == SC_AVS_PICTURE_I)
            {
                handle->isISlice = 1;   /* indicated I picture */
                handle->isHits = 0;     /* for AVS this should always be 0 */
                BNAV_set_frameType(navEntry, eSCTypeIFrame);
            }
            else
            {
                switch (returnAvsPictureCode(p_curBfr->recordByteCountHi))
                {
                case PC_AVS_P_FRAME:
                    handle->prev_I_rfo = 0;
                    /* First P after first I allows open GOP B's to be saved */
                    /* if (handle->hitFirstISlice)
                        handle->allowOpenGopBs = 1; */
                    BNAV_set_frameType(navEntry, eSCTypePFrame);
                    break;
                case PC_B_FRAME:
                    BNAV_set_frameType(navEntry, eSCTypeBFrame);
                    break;
                }
            }

            /* make sequence header offset relative to current frame rather than */
            /* relative to reference frame to allow removal of b-frames */
            BNAV_set_seqHdrSize(navEntry, (unsigned short)handle->seqHdrSize);
            BNAV_set_seqHdrStartOffset(navEntry,
                                       BNAV_Indexer_subtractByteCounts(handle->picStart, handle->seqHdrStartOffset));

            /* Sets the refFrameOffset after adding the prev_I_rfo to the rfo.
            prev_I_rfo will be non-zero ONLY for open gop B's, which are B's that come
            after an I but before a P. */
            BNAV_set_refFrameOffset(navEntry, handle->rfo + handle->prev_I_rfo);

            if (handle->settings.navVersion >= BNAV_VersionOpenGopBs)
            {
                if (sc == SC_AVS_PICTURE_I)
                    handle->prev_I_rfo = handle->rfo;
            }

            if (handle->hitFirstISlice) {
                handle->rfo++;
            }

            BNAV_set_framePts(navEntry, handle->next_pts);
            break;
        default:
            break;

        }

    }

    return i;
}

int BNAV_Indexer_FeedPES(BNAV_Indexer_Handle handle, uint8_t *p_bfr, unsigned size)
{
    if (handle->settings.navVersion == BNAV_Version_VC1_PES) {
        return BNAV_P_FeedPES_VC1(handle, p_bfr, size);
    }

    BDBG_ERR(("FeedPES is only supported for BNAV_Version_VC1_PES"));
    return -1;
}

int BNAV_Indexer_GetStatus( BNAV_Indexer_Handle handle, BNAV_Indexer_Status *pStatus )
{
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->parsingErrors = handle->parsingErrors;
    return 0;
}

