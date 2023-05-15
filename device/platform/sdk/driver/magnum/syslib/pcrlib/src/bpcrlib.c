/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpcrlib.c $
 * $brcm_Revision: Hydra_Software_Devel/113 $
 * $brcm_Date: 8/2/12 9:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pcrlib/7038/bpcrlib.c $
 * 
 * Hydra_Software_Devel/113   8/2/12 9:32p bandrews
 * SW7405-1416: downgraded most warnings to messages
 * 
 * Hydra_Software_Devel/112   5/15/12 6:55p bselva
 * SW7360-19: Add support for 7360 platform
 * 
 * Hydra_Software_Devel/111   5/2/12 3:43p chengs
 * SW7584-28: Add support for 7584.
 * 
 * Hydra_Software_Devel/110   1/16/12 8:15p bandrews
 * SW3556-1029: still one case where zero STC was loaded
 * 
 * Hydra_Software_Devel/109   11/17/11 6:59p bandrews
 * SW7346-544: add playback flag to check for refresh on invalidate
 * 
 * Hydra_Software_Devel/108   6/27/11 4:40p bandrews
 * SW7425-654: merge to main
 * 
 * Hydra_Software_Devel/SW7425-654/3   6/24/11 12:25a bandrews
 * SW7425-654: add pairing to config
 * 
 * Hydra_Software_Devel/SW7425-654/2   6/24/11 12:16a bandrews
 * SW7425-654: don't refresh stc on invalidate in NRT mode
 * 
 * Hydra_Software_Devel/SW7425-654/1   6/23/11 11:41p bandrews
 * SW7425-654: add non-real-time behavior and setting to pcrlib
 * 
 * Hydra_Software_Devel/107   5/16/11 2:46p bandrews
 * SW7125-944: remove trace call as failure is normal and handled
 * 
 * Hydra_Software_Devel/106   4/15/11 7:21p bandrews
 * SW7425-217: pcrlib needed to have its getPts logic cleaned up a bit;
 * for the real fix to this problem see SW7405-5118
 * 
 * Hydra_Software_Devel/105   4/12/11 4:47p bandrews
 * SW3556-11: fix copy pasta error for secondary audio iface
 * 
 * Hydra_Software_Devel/104   1/19/11 7:18p bandrews
 * SW35230-2125: make CS encompass all calls to isr, not just the first
 * two
 * 
 * Hydra_Software_Devel/103   12/27/10 2:55p xhuang
 * SW7358-29: Add 7358/7552 support
 * 
 * Hydra_Software_Devel/102   12/8/10 2:30p jrubio
 * SW7344-9: add 7344/7346/7231 support
 * 
 * Hydra_Software_Devel/101   11/12/10 5:47p bandrews
 * SW35230-2125: renamed config option to refresh_stc_on_invalidate, fixed
 * warning
 * 
 * Hydra_Software_Devel/100   11/11/10 9:00p bandrews
 * SW35230-2125: allow pcrlib to seed STC from lowest PTS during
 * invalidate
 * 
 * Hydra_Software_Devel/99   5/11/10 9:41p bandrews
 * SW7401-2855: removed warning
 * 
 * Hydra_Software_Devel/98   5/5/10 3:56p bandrews
 * SW7401-2855: condition last dss merge
 * 
 * Hydra_Software_Devel/97   5/4/10 2:31p bandrews
 * SW7401-2855: qualify merged dssification with 7401 chipdef
 * 
 * Hydra_Software_Devel/96   5/3/10 8:44p bandrews
 * SW7401-2855: merge to main, correct tabs/spaces
 * 
 * Hydra_Software_Devel/95   2/8/10 3:57p bandrews
 * SW3556-1029: fixed short-circuit eval causing zero-valued stc to be
 * loaded
 * 
 * Hydra_Software_Devel/95   2/8/10 3:57p bandrews
 * SW3556-1029: fixed short-circuit eval causing zero-valued pcr to be
 * loaded
 * 
 * Hydra_Software_Devel/94   8/20/09 4:28p mward
 * PR55545: Support 7125.
 * 
 * Hydra_Software_Devel/93   8/10/09 6:17p jrubio
 * PR55232: add 7340/7342 support
 * 
 * Hydra_Software_Devel/92   6/9/09 3:49p erickson
 * PR55611: pcr not required for PCROFFSET-based systems
 *
 * Hydra_Software_Devel/91   4/22/09 4:08p erickson
 * PR49940: have audio and video master modes automatically switch to stc
 * master if the necessary audio or video interface is not configured.
 *
 * Hydra_Software_Devel/90   1/31/09 12:54a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/89   12/30/08 10:52a erickson
 * PR49582: add asserts for easier debug
 *
 * Hydra_Software_Devel/88   12/15/08 11:28a erickson
 * PR49489: fix warning
 *
 * Hydra_Software_Devel/87   12/2/08 9:13p bandrews
 * PR49489: in AMM, don't apply an stc offset before reseeding with pts
 *
 * Hydra_Software_Devel/86   11/26/08 6:43p bandrews
 * PR48177: Downgraded to messages for most, warning for some
 *
 * Hydra_Software_Devel/85   11/19/08 7:00p vishk
 * PR 49424: Coverity Defect ID:11607 FORWARD_NULL bpcrlib.c Product=93549
 * PR 48336: Coverity Defect ID:12067 FORWARD_NULL bpcrlib.c
 * Product=97405nexus
 *
 * Hydra_Software_Devel/84   10/22/08 7:48p bandrews
 * PR45126: Fixed primary channel lock state wasn't set; Fixed non-primary
 * channels not receiving updates if primary channel lock state was
 * locked, but pts-stc was out of bounds
 *
 * Hydra_Software_Devel/83   8/6/08 10:03a erickson
 * PR45459: fix release build warnings
 *
 * Hydra_Software_Devel/82   7/23/08 3:46p jgarrett
 * PR 39452: Merging to main branch
 *
 * Hydra_Software_Devel/Nexus_Audio_Descriptors/1   5/15/08 4:13p jgarrett
 * PR 42360: Adding tertiary decoder for audio descriptors
 *
 * Hydra_Software_Devel/81   5/12/08 4:12p erickson
 * PR42628: free BPCRlib_Handle
 *
 * Hydra_Software_Devel/80   4/30/08 10:10a erickson
 * PR42349: cannot call BKNI_Free inside a critical section. PCRlib should
 * already be inactive by the time BPCRlib_Close is called.
 *
 * Hydra_Software_Devel/79   4/23/08 10:07a erickson
 * PR39453: add 3548/3556 support
 *
 * Hydra_Software_Devel/78   3/27/08 5:47p bandrews
 * PR40090: fixed imbalance in display of messages or video pts error
 *
 * Hydra_Software_Devel/77   2/22/08 10:51a erickson
 * PR39730: add memset to getdefault
 *
 * Hydra_Software_Devel/76   2/15/08 9:54p bandrews
 * PR39635: Update to handle non-TSM tsm mode
 *
 * Hydra_Software_Devel/75   12/5/07 3:27p katrep
 * PR37217: Added 7335 support
 *
 * Hydra_Software_Devel/74   12/5/07 10:33a katrep
 * PR37217: Added support for 7335.
 *
 * Hydra_Software_Devel/73   11/2/07 11:14a jrubio
 * PR35015: fix for 7325
 *
 * Hydra_Software_Devel/72   11/1/07 10:20a jrubio
 * PR35015: add 7325 support
 *
 * Hydra_Software_Devel/71   10/17/07 7:21p bandrews
 * PR33892: Fixed reference to AVC struct members to remove 32
 *
 * Hydra_Software_Devel/70   10/17/07 5:45p bandrews
 * PR33892: Fixed by using dropped frame count to determine errors
 * unrecoverable by DM and recover in PCRlib
 *
 * Hydra_Software_Devel/69   10/17/07 4:50p bandrews
 * PR36196: Reload PTS + offset during underflow
 *
 * Hydra_Software_Devel/68   9/19/07 5:52p bandrews
 * PR35035: Fixed memory leak
 *
 * Hydra_Software_Devel/67   9/19/07 4:10p bandrews
 * PR33851: Add decoder flush request when outside sync limit
 *
 * Hydra_Software_Devel/66   9/7/07 10:57a mward
 * PR23982:  compile warning in BDBG_MSG.
 *
 * Hydra_Software_Devel/65   8/27/07 8:07p bandrews
 * PR33851: Audio master mode and prep for adaptive tsm
 *
 * Hydra_Software_Devel/64   8/22/07 3:32p katrep
 * PR34204: Added support for mosaic mode API. Enabled as default only for
 * 7405 And 7400
 *
 * Hydra_Software_Devel/7405_pip_support/1   8/21/07 2:43p katrep
 * PR34204: Added mosaic mode support for 7405.
 *
 * Hydra_Software_Devel/PR32044/2   6/29/07 11:39a jgarrett
 * PR 32044: Removing compiler warning
 *
 * Hydra_Software_Devel/PR32044/1   6/28/07 2:52p jgarrett
 * PR 32044: Removing check for duplicate PCR block for mosaic mode (RAVE
 * only)
 *
 * Hydra_Software_Devel/63   6/29/07 4:33p gmohile
 * PR 32546 : Add audio and video pts offset while testing audio/video stc
 *
 * Hydra_Software_Devel/62   6/28/07 8:02p bandrews
 * PR31796: PCRlib implementation of audio and video master modes
 *
 * Hydra_Software_Devel/61   6/15/07 1:47p erickson
 * PR31535: fixed DBG output
 *
 * Hydra_Software_Devel/60   4/3/07 5:02p bandrews
 * PR8915: Fixed spelling
 *
 * Hydra_Software_Devel/59   3/9/07 3:23p dlwin
 * PR 28126: Fixed a problem with playback of streams where the streams
 * that are muxed aggressively. These aggressive streams have video and
 * audio farther apart.
 *
 * Hydra_Software_Devel/58   12/4/06 1:39p erickson
 * PR26295: only call BXPT_PcrOffset_RegenOffset_isr on request_stc, not
 * pts_error. this requires a new param to UpdateStc.
 *
 * Hydra_Software_Devel/57   11/17/06 11:18a erickson
 * PR25108: add 7403 support
 *
 * Hydra_Software_Devel/56   11/14/06 4:33p bandrews
 * PR25912: Converted to all hex displays to match other places in the
 * codebase
 *
 * Hydra_Software_Devel/55   10/30/06 4:06p bandrews
 * PR24904: Initial 3563 support
 *
 * Hydra_Software_Devel/54   10/23/06 10:12a mward
 * PR21691: For 7118 should use == not >=.
 *
 * Hydra_Software_Devel/53   10/23/06 10:08a mward
 * PR21691:  7118 is a member of 7400 family.
 *
 * Hydra_Software_Devel/52   10/2/06 4:24p vsilyaev
 * PR 23826: Added functions to allow user control STC
 *
 * Hydra_Software_Devel/51   9/1/06 2:58p erickson
 * PR23128: remove B_HAS_IP in .c file. already removed in .h.
 *
 * Hydra_Software_Devel/50   8/25/06 11:15a jjordan
 * PR23128: rework PCR clock handling for 97401 B0
 *
 * Hydra_Software_Devel/49   8/4/06 12:46p erickson
 * PR23098: convert b_stc and b_delta_stc to uint32_t and int32_t. these
 * are standard values for PTS math. also made BPCRlib_StcDiff_isr
 * unconditionally public.
 *
 * Hydra_Software_Devel/48   7/25/06 3:41p jgarrett
 * PR 22630: Removing comments about BPCRlib_HeartBeat.
 *
 * Hydra_Software_Devel/47   7/19/06 2:00p bandrews
 * PR22630: #if 0'd out the unused code for HeartBeat
 *
 * Hydra_Software_Devel/46   7/18/06 7:14p bandrews
 * PR22630: Demoted warning into message
 *
 * Hydra_Software_Devel/45   7/17/06 6:43p bandrews
 * PR21639: Fixed so that pcrlib reports wraparound when wrap occurs
 *
 * Hydra_Software_Devel/44   6/30/06 6:36p bandrews
 * PR19434: Corrected so that everyone receives STC if requestor is video
 * or primary audio, and only secondary audio receives update if
 * secondary audio requests
 *
 * Hydra_Software_Devel/43   6/15/06 5:14p mward
 * PR21691: Add support for 7118 chip 97118 board
 *
 * Hydra_Software_Devel/42   6/5/06 4:28p erickson
 * PR21941: cleaned up minor warnings
 *
 * Hydra_Software_Devel/41   4/7/06 8:36p bandrews
 * PR19434: Hack to ensure only requestor receives STC
 *
 * Hydra_Software_Devel/40   3/28/06 10:05a vsilyaev
 * PR 18642: Demoted BDBG_WRN in PcrOffset function
 *
 * Hydra_Software_Devel/39   3/20/06 6:16p dlwin
 * PR 15200: Fixed a  compiler warning for uninitialized variable.
 *
 * Hydra_Software_Devel/38   3/14/06 3:30p jjordan
 * PR15200: fix build for non-7411 platforms
 *
 * Hydra_Software_Devel/37   3/2/06 11:39a jjordan
 * PR15200: rework STC steering for 97398
 *
 * Hydra_Software_Devel/36   3/1/06 3:26p jjordan
 * PR18981: fix "live playback" and add config option IP_USE_LIVE_PLAYBACK
 *
 * Hydra_Software_Devel/35   2/28/06 1:47p jjordan
 * PR18981: PR18981: prototype "live playback"
 *
 * Hydra_Software_Devel/34   2/10/06 2:08p vsilyaev
 * PR 18642: Added code to use pcr offset
 *
 * Hydra_Software_Devel/33   2/9/06 2:01p vsilyaev
 * PR 18642: Better trust to PCR when updating STC
 *
 * Hydra_Software_Devel/32   2/3/06 5:08p jgarrett
 * PR 19440: Updating build for the 7400
 *
 * Hydra_Software_Devel/31   2/3/06 3:50p vsilyaev
 * PR 18642: Update PCRlibrary to support playback of constant bitrate
 * streams
 *
 * Hydra_Software_Devel/PR18642/3   1/27/06 4:11p vsilyaev
 * PR18642: Use special configuration mode to change offsets for the
 * constant rate streams
 *
 * Hydra_Software_Devel/PR18642/2   1/27/06 2:28p vsilyaev
 * PR18642: Added code to use PCR for STC update
 *
 * Hydra_Software_Devel/PR18642/1   1/19/06 6:15p vsilyaev
 * PR18642: Use PTS offset to control buffer depth in the decoders
 *
 * Hydra_Software_Devel/30   12/20/05 2:12p vsilyaev
 * PR 18642: Added BPCRlib_Channel_LoadStc function to load user defined
 * STC
 *
 * Hydra_Software_Devel/29   12/15/05 4:38p vsilyaev
 * PR 18642: Added support for the user configurable PTS offsets and CDB
 * levels
 *
 * Hydra_Software_Devel/27   10/6/05 4:46p vsilyaev
 * PR 17441: Demoted warnings to messages, it reduces traffic over the
 * serial port
 *
 * Hydra_Software_Devel/26   9/23/05 12:51p erickson
 * PR17108: initial 97401 checkin
 *
 * Hydra_Software_Devel/25   9/6/05 6:45p vsilyaev
 * PR 16012: Added support for 7401.
 *
 * Hydra_Software_Devel/24   9/2/05 6:08p vsilyaev
 * PR15967: Added more debug output.
 *
 * Hydra_Software_Devel/23   8/29/05 6:23p vsilyaev
 * PR 16685,PR15967: Added use of UpdateStc function.
 *
 * Hydra_Software_Devel/22   8/26/05 12:32p erickson
 * PR15967: Added Invalidate function to cover decoder based trickmodes
 *
 * Hydra_Software_Devel/21   8/17/05 7:56p vsilyaev
 * PR 15967: Added BPCRlib_Channel_AudioPtsError_isr function.
 *
 * Hydra_Software_Devel/20   8/10/05 4:14p vsilyaev
 * PR 15967: Removed references to BMVD_.
 *
 * Hydra_Software_Devel/19   8/9/05 2:30p vsilyaev
 * PR 15967: Use both aux transport and decoder to access STC.
 *
 * Hydra_Software_Devel/18   8/5/05 3:07p vsilyaev
 * PR 15967: Fixed compile time warnings
 *
 * Hydra_Software_Devel/17   6/28/05 1:16p vsilyaev
 * PR 15697: Improved interface to update STC.
 *
 * Hydra_Software_Devel/16   6/24/05 3:53p vsilyaev
 * PR 15967: Use audio/video interface to read STC and PTS.
 *
 * Hydra_Software_Devel/15   9/20/04 8:07p vsilyaev
 * PR 11800: Added test for STC/PTS range into the videoPtsError handler.
 * Increased range for valid video PTS's to -0.5+1.0sec.
 *
 * Hydra_Software_Devel/14   5/19/04 11:37a vsilyaev
 * PR 10971: Added support for DSS playback.
 *
 * Hydra_Software_Devel/13   5/12/04 6:55p vsilyaev
 * PR 10971: Added handling of DSS timebase.
 *
 * Hydra_Software_Devel/12   4/22/04 5:15p vsilyaev
 * PR 8915: Check level of video VBV buffer before using audio PTS as STC.
 *
 * Hydra_Software_Devel/11   4/21/04 8:23p vsilyaev
 * PR 8915: Better handling of playback, try to reuse STC if possible.
 *
 * Hydra_Software_Devel/10   4/21/04 1:47p vsilyaev
 * PR 10668: PR 8915: Fixed test for duplicate audio/video decodes.
 *
 * Hydra_Software_Devel/9   4/5/04 10:28a vsilyaev
 * PR 8915: Use new name for PTS field
 *
 * Hydra_Software_Devel/PVR_7038_PreRelease/1   3/24/04 8:09p vsilyaev
 * Use new name for PTS field
 *
 * Hydra_Software_Devel/8   3/4/04 5:03p vsilyaev
 * PR 8915: Added VideoPtsError handler
 *
 * Hydra_Software_Devel/7   3/1/04 7:17p vsilyaev
 * PR 8915: Fixed comparsion function for the second channel.
 *
 * Hydra_Software_Devel/6   2/1/04 6:23p vsilyaev
 * PR 8915: Don't acquire critical section in functions callable from ISR.
 *
 * Hydra_Software_Devel/5   2/1/04 6:01p vsilyaev
 * PR 8915: Added handling of video decode.
 *
 * Hydra_Software_Devel/4   12/10/03 6:34p vsilyaev
 * PR 8915: Use newly avaliable _isr safe XPT_PCR functions.
 *
 * Hydra_Software_Devel/3   12/10/03 12:15p marcusk
 * PR8915: Fixed minor compiler error.
 *
 * Hydra_Software_Devel/2   12/9/03 2:29p vsilyaev
 * PR 8915: PCRLib module for 7038.
 *
 * Hydra_Software_Devel/1   12/3/03 6:38p vsilyaev
 * 7038 PCRlib module
 *
 ***************************************************************************/

#include "bstd.h"
#include "bpcrlib.h"
#include "blst_list.h"
#include "bkni.h"
#include "bpcrlib_mvd.h"
#include "bpcrlib_aud.h"

#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
  #include "barc.h"
  #include "barc_priv.h"
#endif

BDBG_MODULE(pcrlib);

#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */

enum BPCRlib_Decoder_State {
    BPCRlib_Decoder_State_eNotLocked,
    BPCRlib_Decoder_State_eLocked,
    BPCRlib_Decoder_State_eLocking,
    BPCRlib_Decoder_State_eWaitingSTC,
    BPCRlib_Decoder_State_eInvalidated
};

#if (BCHP_CHIP>=7400) || (BCHP_CHIP==7118) || (BCHP_CHIP==3563) || (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || \
    (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==7125) || \
    (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7231) || (BCHP_CHIP==7358) || (BCHP_CHIP==7552) || \
    (BCHP_CHIP==7584) || (BCHP_CHIP==7360)
#define B_PCRLIB_HAS_PCROFFSET  1
#include "bxpt_pcr_offset.h"
#endif

#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
typedef enum OS_control_state {
    OS_control_idle,
    OS_control_waiting_for_pcr
} OS_control_state;
#endif

#define BPCRlib_P_PCR_FIFO  32

typedef struct {
    uint32_t last_pcr;
    bool    last_pcr_valid;
    unsigned index;
    unsigned count;
    int32_t pcr_fifo[BPCRlib_P_PCR_FIFO];
    bool    pcr_offset_valid;
    uint32_t pcr_offset;
}BPCRlib_P_PCROffset;

struct BPCRlib_P_Channel {
    BLST_D_ENTRY(BPCRlib_P_Channel) link;
    BPCRlib_Handle parent;
    BXPT_PCR_Handle pcr;
    BPCRlib_ChannelSettings settings;
    enum BPCRlib_Decoder_State video_state;
    enum BPCRlib_Decoder_State audio_state;
    BPCRlib_Config cfg;
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
    OS_control_state theOS_control_state;
    int OS_bit;
    int error_count;
#endif

    BPCRlib_P_PCROffset pcr_offset;

    struct {
        enum {delayed_stc_eInvalid, delayed_stc_eLocked, delayed_stc_eWaiting} state;
        uint32_t stc; /* STC to be applied to the decoder */
        uint32_t old_stc; /* snapshot of STC from the running counter */
    } delayed_stc;

    /* added to see if we get consecutive pts errors */
    struct
    {
        uint32_t uiLastDroppedFrameCount;
        uint32_t uiLastDecodedFrameCount;
        uint32_t uiConsecutivePtsErrorCount;
    } sVideo;
};

/* threshold to detect discontinuity in the streame */
#define BPCRLIB_DISC_TRESHOLD   ((3003*30*5)/4)

struct BPCRlib_P_Handle {
    BLST_D_HEAD(BPCRlib_P_Handle_list, BPCRlib_P_Channel) list;
};

static void BPCRlib_P_InvalidatePCRCache(BPCRlib_Channel channel);

BERR_Code
BPCRlib_Open(BPCRlib_Handle *pHandle, BCHP_Handle hChip)
{
    BPCRlib_Handle handle;

    BSTD_UNUSED(hChip);

    BDBG_ASSERT(BPCRlib_StcDiff_isr(false, 16349, 4294746617U)==118514);
    BDBG_ASSERT(BPCRlib_StcDiff_isr(false, 4294746617U, 16349)==-118514);
    BDBG_ASSERT(BPCRlib_StcDiff_isr(false, 16349, 0)==8174);
    BDBG_ASSERT(BPCRlib_StcDiff_isr(false, 0, 16349)==-8174);

    handle = BKNI_Malloc(sizeof(*handle));
    if (!handle) {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BLST_D_INIT(&handle->list);
    *pHandle = handle;

    return BERR_SUCCESS;
}

void
BPCRlib_Close(BPCRlib_Handle handle)
{
    BPCRlib_Channel pcr;

    while((pcr=BLST_D_FIRST(&handle->list))!=NULL) {
        BLST_D_REMOVE_HEAD(&handle->list, link);
        BKNI_Free(pcr);
    }
    BKNI_Free(handle);
    return;
}

#if B_PCRLIB_HAS_PCROFFSET
#else
#define B_HAS_STCDECIFACE 1
#endif

static const BPCRlib_Config pcr_cfg = {
    false,
    BAVC_StreamType_eTsMpeg,
    NULL, /* primary audio */
#if !B_HAS_STCDECIFACE
    NULL,
#else
    &BPCRlib_Audio_Aud,
#endif
    NULL, /* video */
#if !B_HAS_STCDECIFACE
    NULL,
#else
    &BPCRlib_Video_Mvd,
#endif
    NULL, /* secondary audio */
#if !B_HAS_STCDECIFACE
    NULL,
#else
    &BPCRlib_Audio_Aud,
#endif
    NULL, /* tertiary audio */
#if !B_HAS_STCDECIFACE
    NULL,
#else
    &BPCRlib_Audio_Aud,
#endif
    NULL, /* aux transport */
    96 * 1024, /* video CDB level */
    ((3003*4)/2), /* video PTS offset, 120ms */
    (3003/2)*30, /* video STC discard threshold, 1 sec */
    4 * 1024,  /* audio CDB level */
    ((3003*2)/2), /* audio PTS offset, 60ms */
    (3003/2)*120, /* audio STC discard threshold, 4 sec */
    (3003/2)*2, /* pcr_offset 60 ms delay */
    (3003/2)*30*10, /* pcr_discard 10 sec delay */
    BPCRlib_Mode_eAutoPts,
    false, /* is_playback_ip */
    BPCRlib_TsmMode_eSTCMaster, /* tsm_mode */
    0, /* sync_limit */
    NULL, /* dec flush event handle */
    8, /* consecutive pts error limit */
    true, /* refresh stc on invalidate */
    false, /* non realtime mode */
    false /* paired with another channel */
};

BERR_Code
BPCRlib_Channel_GetChannelDefSettings(BPCRlib_Handle handle, BPCRlib_ChannelSettings *config)
{
    BSTD_UNUSED(handle);
    BKNI_Memset(config, 0, sizeof(*config));
    return BERR_SUCCESS;
}

BERR_Code
BPCRlib_Channel_Create(BPCRlib_Handle handle, BXPT_PCR_Handle pcr, BPCRlib_Channel *pChannel, const BPCRlib_ChannelSettings *settings )
{
    BPCRlib_Channel channel;
#if !B_PCRLIB_HAS_PCROFFSET
    BPCRlib_Channel i;
#endif

    channel = BKNI_Malloc(sizeof(*channel));
    if (!channel) {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_EnterCriticalSection();
#if !B_PCRLIB_HAS_PCROFFSET
    for(i=BLST_D_FIRST(&handle->list);i;i=BLST_D_NEXT(i,link)) {
        if (i->pcr == pcr) {
            BKNI_LeaveCriticalSection();
            if (channel)
            {
                BKNI_Free(channel);
            }
            BDBG_ERR(("Can't use the same PCR(%#x) channel for two different pcrlib channels", (unsigned)pcr));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
    }
#endif
    channel->settings = *settings;
    channel->pcr = pcr;
    channel->cfg = pcr_cfg;
    channel->parent=handle;
    channel->audio_state = BPCRlib_Decoder_State_eWaitingSTC;
    channel->video_state = BPCRlib_Decoder_State_eWaitingSTC;
    channel->delayed_stc.state = delayed_stc_eInvalid;

    BPCRlib_P_InvalidatePCRCache(channel);
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
    channel->theOS_control_state = OS_control_idle;
    channel->OS_bit = 1;
    channel->error_count = 0;
#endif

    BLST_D_INSERT_HEAD(&handle->list, channel, link);
    BKNI_LeaveCriticalSection();

    *pChannel = channel;

    return BERR_SUCCESS;
}

void
BPCRlib_Channel_Destroy(BPCRlib_Channel channel)
{
    BKNI_EnterCriticalSection();
    BLST_D_REMOVE(&channel->parent->list, channel, link);
    BKNI_LeaveCriticalSection();
    BKNI_Free(channel);
    return;
}

void
BPCRlib_Channel_GetConfig(BPCRlib_Channel channel, BPCRlib_Config *config)
{
    *config = channel->cfg;
    return;
}

BERR_Code
BPCRlib_Channel_SetConfig( BPCRlib_Channel channel, const BPCRlib_Config *config)
{
    BPCRlib_Channel ch;

    if (config == NULL)
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    if (config->audio!=NULL && config->audio == config->secondary_audio)
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    if (config->audio!=NULL && config->audio == config->tertiary_audio)
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    /* coverity[var_compare_op: FALSE] */
    if ((config->audio_iface==NULL && config->audio!=NULL) ||
        (config->video_iface==NULL && config->video!=NULL) ||
        (config->secondary_audio_iface==NULL && config->secondary_audio!=NULL) ||
        (config->tertiary_audio_iface==NULL && config->tertiary_audio!=NULL))
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    /* coverity[var_deref_op: FALSE] */
    if (config->aux_transport==NULL &&
            ((config->audio && config->audio_iface->useAuxTrp) ||
             (config->secondary_audio && config->secondary_audio_iface->useAuxTrp) ||
             (config->tertiary_audio && config->tertiary_audio_iface->useAuxTrp) ||
             (config->video && config->video_iface->useAuxTrp)))
        return BERR_TRACE(BERR_INVALID_PARAMETER);

    BDBG_MSG(("pcr old config %#x: video %#x audio %#x,%#x", channel, channel->cfg.video, channel->cfg.audio, channel->cfg.secondary_audio));
    BDBG_MSG(("pcr new config %#x: video %#x audio %#x,%#x", channel, config->video, config->audio, config->secondary_audio));
    BKNI_EnterCriticalSection();
    for(ch=BLST_D_FIRST(&channel->parent->list);ch;ch=BLST_D_NEXT(ch,link)) {
        if (ch==channel) {
            /* skip a current channel */
            continue;
        }
        BDBG_MSG(("pcr %#x: video %#x audio %#x,%#x", ch, ch->cfg.video, ch->cfg.audio, ch->cfg.secondary_audio));
        if (
               (config->video && ch->cfg.video == config->video)
            || (config->audio && (ch->cfg.audio == config->audio || ch->cfg.secondary_audio == config->audio))
            || (config->secondary_audio && ( ch->cfg.secondary_audio == config->secondary_audio || ch->cfg.audio == config->secondary_audio))
            || (config->tertiary_audio && ( ch->cfg.tertiary_audio == config->tertiary_audio || ch->cfg.audio == config->tertiary_audio))
            ) {
            BKNI_LeaveCriticalSection();
            BDBG_ERR(("Selected audio or video channel currently assigned to other pcrlib channel"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
    }
    if (channel->cfg.playback!= config->playback) {
        BPCRlib_P_InvalidatePCRCache(channel);
    }
    /* when decoders changes they go to unlocked state */
    if (channel->cfg.video != config->video) {
        channel->video_state = BPCRlib_Decoder_State_eWaitingSTC;
        BPCRlib_P_InvalidatePCRCache(channel);
    }
    if (channel->cfg.audio != config->audio) {
        channel->audio_state = BPCRlib_Decoder_State_eWaitingSTC;
        BPCRlib_P_InvalidatePCRCache(channel);
    }
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
    if (channel->cfg.video != config->video || channel->cfg.audio != config->audio) {
        channel->theOS_control_state = OS_control_idle;
        channel->OS_bit = 1;
        channel->error_count = 0;
    }
#endif
    if (channel->cfg.video != config->video || channel->cfg.audio != config->audio || channel->cfg.playback!= config->playback) {
        channel->delayed_stc.state= delayed_stc_eInvalid;
    }
    channel->cfg = *config;
    BKNI_LeaveCriticalSection();
    /* Note, we don't reconfigure audio/video decoders at pcrlib */
    return BERR_SUCCESS;
}

static BPCRlib_Channel
find_by_audio_isr(BPCRlib_Handle handle, void *audio)
{
    BPCRlib_Channel chn;
    BDBG_ASSERT(audio);
    for(chn=BLST_D_FIRST(&handle->list);chn;chn=BLST_D_NEXT(chn,link)) {
        if (chn->cfg.audio==audio || chn->cfg.secondary_audio==audio || chn->cfg.tertiary_audio==audio) {
            /* we exploit fact that audio and secondary audio can't be shared */
            goto found;
        }
    }
    BDBG_WRN(("Unknown audio channel %#x", (unsigned)audio));
found:
    return chn;
}

static BPCRlib_Channel
find_by_video_isr(BPCRlib_Handle handle, void *video)
{
    BPCRlib_Channel chn;

    BDBG_ASSERT(video);
    for(chn=BLST_D_FIRST(&handle->list);chn;chn=BLST_D_NEXT(chn,link)) {
        if (chn->cfg.video==video) {
            goto found;
        }
    }
    BDBG_WRN(("Unknown video channel %#x", (unsigned)video));
found:
    return chn;
}



/*
* this function takes STC in the 45KHz domain (MPEG) or 27MHz domain (DSS)
* as 32 bit unsigned and returns delta in the 22.5KHz domain, 32 bit signed
*/
int32_t
BPCRlib_StcDiff_isr(bool dss, uint32_t stc1, uint32_t stc2)
{
    int32_t delta, modulo;

    /* 1. convert to the 22.5KHz domain */
    if (dss) {
        stc1 /= 1200;
        stc2 /= 1200;
        modulo = (1<<30)/600;
    } else {
        stc1 /= 2;
        stc2 /= 2;
        modulo = (1<<30);
    }

    delta = stc1 - stc2;
    /* 2. Handle wraparond cases */
    if (stc1 < stc2 && delta < -modulo) {
        delta += modulo*2;
    } else if (stc1 > stc2 && delta > modulo) {
        delta -= modulo*2;
    }
    return delta;
}

static bool
b_is_dss_isr(const BPCRlib_Config *cfg)
{
    return cfg->stream == BAVC_StreamType_eDssPes || cfg->stream == BAVC_StreamType_eDssEs;
}

/* 20060717 bandrews - need update for send_stc to match update_stc for requestor behavior */
static BERR_Code
b_really_send_stc_isr(const BPCRlib_Channel chn, uint32_t stc)
{
    BERR_Code rc=BERR_SUCCESS;
    bool dss = b_is_dss_isr(&chn->cfg);
    uint32_t old_stc;

    if (chn->cfg.mode == BPCRlib_Mode_eConstantDelay) {
#if B_PCRLIB_HAS_PCROFFSET
        rc = BXPT_PcrOffset_SetStc_isr(chn->cfg.aux_transport, stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("BXPT_PcrOffset_SetStc_isr returned error %#x, ignored", rc)); }
#else
        rc = BXPT_PCR_SetUserStc_isr(chn->pcr, stc, 0);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("BXPT_PCR_SetUserStc returned error %#x, ignored", rc)); }
#endif
    }
    if (chn->cfg.video && chn->cfg.video_iface->setStc) {
        BDBG_MSG(("updating video:STC %#x", stc));
        rc = chn->cfg.video_iface->getStc(chn->cfg.aux_transport, chn->cfg.video, &old_stc);
        rc = chn->cfg.video_iface->setStc(chn->cfg.video_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, chn->cfg.video, dss,stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("video setStc returned error %#x, ignored", rc)); }
    }
    if (chn->cfg.audio && chn->cfg.audio_iface->setStc) {
        BDBG_MSG(("updating audio: STC %#x", stc));
        rc = chn->cfg.audio_iface->getStc(chn->cfg.aux_transport, chn->cfg.audio, &old_stc);
        rc = chn->cfg.audio_iface->setStc(chn->cfg.audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, chn->cfg.audio, dss, stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio setStc returned error %#x, ignored", rc)); }
    }
    if (chn->cfg.secondary_audio && chn->cfg.secondary_audio_iface->setStc) {
        BDBG_MSG(("updating secondary_audio: STC %#x", stc));
        rc = chn->cfg.secondary_audio_iface->setStc(chn->cfg.secondary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, chn->cfg.secondary_audio, dss, stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("secondary audio setStc returned error %#x, ignored", rc)); }
    }
    if (chn->cfg.tertiary_audio && chn->cfg.tertiary_audio_iface->setStc) {
        BDBG_MSG(("updating tertiary_audio: STC %#x", stc));
        rc = chn->cfg.tertiary_audio_iface->setStc(chn->cfg.tertiary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, chn->cfg.tertiary_audio, dss, stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("tertiary audio setStc returned error %#x, ignored", rc)); }
    }
    return rc;
}

static BERR_Code
b_send_stc_isr(const BPCRlib_Channel chn, uint32_t stc, void * requestor)
{
    BERR_Code rc=BERR_SUCCESS;
    bool dss = b_is_dss_isr(&chn->cfg);

    /*
        20060717 bandrews -
        if video requests -> set video, primary audio, and secondary audio
        if primary audio requests -> set video, primary audio, and secondary audio
        if secondary audio requests -> set secondary audio only
    */
    if (requestor == chn->cfg.video || requestor == chn->cfg.audio)
    {
        rc = b_really_send_stc_isr(chn, stc);
    }
    else /* secondary or tertiary audio */
    {
        if ( requestor == chn->cfg.secondary_audio )
        {
            if (chn->cfg.secondary_audio && chn->cfg.secondary_audio_iface->setStc) {
                BDBG_MSG(("updating secondary_audio: STC %#x", stc));
                rc = chn->cfg.secondary_audio_iface->setStc(chn->cfg.secondary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, chn->cfg.secondary_audio, dss, stc);
                if (rc!=BERR_SUCCESS) { BDBG_ERR(("secondary audio setStc returned error %#x, ignored", rc)); }
            }
        }
        else /* tertiary */
        {
            if (chn->cfg.tertiary_audio && chn->cfg.tertiary_audio_iface->setStc) {
                BDBG_MSG(("updating tertiary_audio: STC %#x", stc));
                rc = chn->cfg.tertiary_audio_iface->setStc(chn->cfg.tertiary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, chn->cfg.tertiary_audio, dss, stc);
                if (rc!=BERR_SUCCESS) { BDBG_ERR(("tertiary audio setStc returned error %#x, ignored", rc)); }
            }
        }
    }

    return rc;
}

static bool
b_read_stc_isr(const BPCRlib_Channel chn, uint32_t *stc)
{
    BERR_Code rc;

    if (chn->cfg.audio && chn->audio_state!=BPCRlib_Decoder_State_eWaitingSTC && chn->cfg.audio_iface->getStc ) {
      rc = chn->cfg.audio_iface->getStc(chn->cfg.aux_transport, chn->cfg.audio, stc);
      if (rc==BERR_SUCCESS) { return true; }
    }
    if (chn->cfg.video && chn->video_state!=BPCRlib_Decoder_State_eWaitingSTC && chn->cfg.video_iface->getStc) {
      rc = chn->cfg.video_iface->getStc(chn->cfg.aux_transport, chn->cfg.video, stc);
      if (rc==BERR_SUCCESS) { return true; }
    }
    return false;
}

#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
/* 7411 regs */
#define TS_DISCONT_CNT  0x00100458
#define TS_PCR_CHANGED  0x00100448
#define TS_TSM_CONFIG   0x00100540

static BPCRlib_Channel
find_by_decoder_isr(BPCRlib_Handle handle, void *decoder)
{
    BPCRlib_Channel chn;
    BDBG_ASSERT(decoder);
    for(chn=BLST_D_FIRST(&handle->list);chn;chn=BLST_D_NEXT(chn,link)) {
        if (chn->cfg.audio==decoder || chn->cfg.secondary_audio==decoder || chn->cfg.tertiary_audio==decoder) {
            /* we exploit fact that audio and secondary audio can't be shared */
            goto found;
        }
        else if (chn->cfg.video==decoder) {
            goto found;
        }
    }
    BDBG_WRN(("Unknown decoder channel %#x", (unsigned)decoder));
found:
    return chn;
}

BERR_Code
BPCRlib_HasPcrDisco(
    BPCRlib_Handle handle, /* [in] PCRlib channel handle */
    void *decoder, /* [in] decoder handle */
    bool *has_disco
    )
{
    BERR_Code rc = BERR_SUCCESS;
    uint32_t val;
    uint32_t reg;
    BARC_ChannelHandle hArcCh;
    BPCRlib_Channel chn = find_by_decoder_isr(handle, decoder);

    if (!chn)
        return BERR_TRACE(BERR_INVALID_PARAMETER);

    hArcCh = (BARC_ChannelHandle)(chn->cfg.video_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr);
    reg = TS_DISCONT_CNT;
    val = BARC_Reg_Read32_isr(hArcCh->hArc, reg);
    if(val != 0) {
        *has_disco = true;
        BARC_Reg_Write32_isr(hArcCh->hArc, reg, 0);
    }
    else {
        *has_disco = false;
    }

    return rc;
}

static BERR_Code
b_os_control_isr(const BPCRlib_Channel chn, bool use_aux_trp)
{
    BERR_Code rc=BERR_SUCCESS;
    uint32_t val;
    uint32_t reg;
    BARC_ChannelHandle  hArcCh = (BARC_ChannelHandle)(use_aux_trp?chn->cfg.aux_transport:chn->pcr);

    /* OS - PCR_Offset Supression control */
    switch(chn->theOS_control_state) {
    case OS_control_idle:
        chn->OS_bit = 1;
        chn->error_count++;
        if(chn->error_count > 10) {
            chn->error_count = 0;
            chn->OS_bit = 0;
            /*  read it to clear the value */
            reg = TS_PCR_CHANGED;
            val = BARC_Reg_Read32_isr(hArcCh->hArc, reg);
            BDBG_MSG(("@@@ TS_PCR_CHANGED: %#x <- %#x (OS_control_idle state)", (unsigned)reg, (unsigned)val));
            chn->theOS_control_state = OS_control_waiting_for_pcr;
        }
        break;
    case OS_control_waiting_for_pcr:
        /* Really we need a "one shot", ie let only one PCR through (the first one encountered
          in the stream after startup/disco event) */
        reg = TS_PCR_CHANGED;
        val = BARC_Reg_Read32_isr(hArcCh->hArc, reg);
        BDBG_MSG(("@@@ TS_PCR_CHANGED: %#x <- %#x (OS_control_waiting_for_pcr state)", (unsigned)reg, (unsigned)val));
        if(val) {
            chn->error_count = 0;
            chn->OS_bit = 1;
            chn->theOS_control_state = OS_control_idle;
        }
        break;
    }
    reg = TS_TSM_CONFIG;
    val = BARC_Reg_Read32_isr(hArcCh->hArc, reg);
    if(chn->OS_bit) {
        val |= 0x00000001;
    }
    else {
        val &= ~0x00000001;
    }
    BDBG_MSG(("TS_TSM_CONFIG: %#x <- %#x\n", (unsigned)reg, (unsigned)val));
    BARC_Reg_Write32_isr(hArcCh->hArc, reg, val);

    return rc;
}

static BERR_Code
b_update_stc_isr(const BPCRlib_Channel chn, bool is_request_stc)
{
    BERR_Code rc=BERR_SUCCESS;

    if(chn->cfg.is_playback_ip) {
    if (chn->cfg.video && chn->cfg.video_iface->updateStc) {
        rc = b_os_control_isr(chn, chn->cfg.video_iface->useAuxTrp);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("b_os_control_isr() returned error %#x, ignored", rc)); }
        rc = chn->cfg.video_iface->updateStc(chn->cfg.video_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("video updateStc returned error %#x, ignored", rc)); }
    }
    else if (chn->cfg.audio && chn->cfg.audio_iface->updateStc) {
        rc = b_os_control_isr(chn, chn->cfg.audio_iface->useAuxTrp);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("b_os_control_isr() returned error %#x, ignored", rc)); }
        rc = chn->cfg.audio_iface->updateStc(chn->cfg.audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
    }
    if (chn->cfg.secondary_audio && chn->cfg.secondary_audio_iface->updateStc) {
        rc = b_os_control_isr(chn, chn->cfg.secondary_audio_iface->useAuxTrp);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("b_os_control_isr() returned error %#x, ignored", rc)); }
        rc = chn->cfg.secondary_audio_iface->updateStc(chn->cfg.secondary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
    }
    if (chn->cfg.tertiary_audio && chn->cfg.tertiary_audio_iface->updateStc) {
        rc = b_os_control_isr(chn, chn->cfg.tertiary_audio_iface->useAuxTrp);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("b_os_control_isr() returned error %#x, ignored", rc)); }
        rc = chn->cfg.tertiary_audio_iface->updateStc(chn->cfg.tertiary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
    }
    }
    else {
        if (chn->cfg.video && chn->cfg.video_iface->updateStc) {
            rc = chn->cfg.video_iface->updateStc(chn->cfg.video_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
            if (rc!=BERR_SUCCESS) { BDBG_ERR(("video updateStc returned error %#x, ignored", rc)); }
        }
        else if (chn->cfg.audio && chn->cfg.audio_iface->updateStc) {
            rc = chn->cfg.audio_iface->updateStc(chn->cfg.audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
            if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
        }
        if (chn->cfg.secondary_audio && chn->cfg.secondary_audio_iface->updateStc) {
            rc = chn->cfg.secondary_audio_iface->updateStc(chn->cfg.secondary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
            if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
        }
        if (chn->cfg.tertiary_audio && chn->cfg.tertiary_audio_iface->updateStc) {
            rc = chn->cfg.tertiary_audio_iface->updateStc(chn->cfg.tertiary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
            if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
        }
    }

    return rc;
}
#else
/* 20060630 bandrews - this is going to get ugly... more hacks */
static BERR_Code
b_really_update_stc_isr(const BPCRlib_Channel chn, bool is_request_stc)
{
    BERR_Code rc=BERR_SUCCESS;

    if (chn->cfg.video && chn->cfg.video_iface->updateStc) {
        rc = chn->cfg.video_iface->updateStc(chn->cfg.video_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("video updateStc returned error %#x, ignored", rc)); }
    }
    if (chn->cfg.audio && chn->cfg.audio_iface->updateStc) {
        rc = chn->cfg.audio_iface->updateStc(chn->cfg.audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
    }
    if (chn->cfg.secondary_audio && chn->cfg.secondary_audio_iface->updateStc) {
        rc = chn->cfg.secondary_audio_iface->updateStc(chn->cfg.secondary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
    }
    if (chn->cfg.tertiary_audio && chn->cfg.tertiary_audio_iface->updateStc) {
        rc = chn->cfg.tertiary_audio_iface->updateStc(chn->cfg.tertiary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
        if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
    }

    return rc;
}
static BERR_Code
b_update_stc_isr(const BPCRlib_Channel chn, void * requestor, bool is_request_stc)
{
    BERR_Code rc=BERR_SUCCESS;

    /*
        20060630 bandrews -
        if video requests -> update video, primary audio, and secondary audio
        if primary audio requests -> update video, primary audio, and secondary audio
        if secondary audio requests -> update secondary audio only
    */
    if (requestor == chn->cfg.video || requestor == chn->cfg.audio)
    {
        rc = b_really_update_stc_isr(chn, is_request_stc);
    }
    else /* secondary or tertiary audio */
    {
        if ( requestor == chn->cfg.secondary_audio )
        {
            if (chn->cfg.secondary_audio && chn->cfg.secondary_audio_iface->updateStc) {
                rc = chn->cfg.secondary_audio_iface->updateStc(chn->cfg.secondary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
                if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
            }
        }
        else /* tertiary */
        {
            if (chn->cfg.tertiary_audio && chn->cfg.tertiary_audio_iface->updateStc) {
                rc = chn->cfg.tertiary_audio_iface->updateStc(chn->cfg.tertiary_audio_iface->useAuxTrp?chn->cfg.aux_transport:chn->pcr, is_request_stc);
                if (rc!=BERR_SUCCESS) { BDBG_ERR(("audio updateStc returned error %#x, ignored", rc)); }
            }
        }
    }

    return rc;
}
#endif

static BERR_Code
b_load_delayed_stc_isr(BPCRlib_Channel chn, void *requestor)
{
    BERR_Code rc=BERR_SUCCESS;
    uint32_t cur_stc;
    uint32_t stc;
    BSTD_UNUSED(requestor);

    switch(chn->delayed_stc.state) {
    case delayed_stc_eInvalid:
        BDBG_WRN(("b_load_delayed_stc_isr: %#lx(%#lx) ignored, STC is not avaliable yet", (unsigned long)chn, (unsigned long)requestor));
        break;
    case delayed_stc_eLocked:
    case delayed_stc_eWaiting:
#if B_PCRLIB_HAS_PCROFFSET
        cur_stc = BXPT_PcrOffset_GetStc_isr(chn->cfg.aux_transport) + BXPT_PcrOffset_GetOffset_isr(chn->cfg.aux_transport);
#else
        rc = BXPT_PCR_GetStc_isr(chn->pcr, &cur_stc, &stc /* not used */ );
#endif
        stc = chn->delayed_stc.stc + (cur_stc - chn->delayed_stc.old_stc);
        BDBG_WRN(("b_load_delayed_stc_isr: %#lx(%#lx) loading STC %#x (%u + %d)", (unsigned long)chn, (unsigned long)requestor, (unsigned)stc, (unsigned)chn->delayed_stc.stc, (int)(cur_stc - chn->delayed_stc.old_stc)));
        rc = b_send_stc_isr(chn, stc, requestor);
        /* update delayed_stc, so following  call to b_load_delayed_stc_isr would not use stale values */
        chn->delayed_stc.stc = stc;
        chn->delayed_stc.old_stc = cur_stc;
        chn->delayed_stc.state = delayed_stc_eLocked;
        break;
    }
    return rc;
}



static bool
BPCRlib_TestAudioStc_isr(BPCRlib_Channel chn, uint32_t new_stc, uint32_t pts)
{
    int32_t delta;
    bool dss = b_is_dss_isr(&chn->cfg);

    delta = BPCRlib_StcDiff_isr(dss, new_stc, pts);

    if (
    /* TODO: does this need dss qualification?  3003 is an MPEG number */
        (delta >= 0 && delta < (2*chn->cfg.audio_pts_offset + (3003 * 5)/(2*2))) ||
        (delta < 0 && delta > -(2*chn->cfg.audio_pts_offset + (3003 * 60 )/(2*2)))
    ) /* video STC could be  0.15 sec ahead or 1.0 sec behind of PTS */
    {
        BDBG_MSG(("TestAudioStc: STC %#x PTS %#x delta %ld (%s)", new_stc, pts, delta, "good"));
        return true;
    } else {
        BDBG_MSG(("TestAudioStc: STC %#x PTS %#x delta %ld (%s)", new_stc, pts, delta, "bad"));
        return false;
    }
}

static bool
BPCRlib_P_GetStcFromPcr_isr(BPCRlib_Channel chn, uint32_t pts, uint32_t *new_stc)
{
    int32_t delta;
    int32_t offset;
    unsigned i;
    bool dss = b_is_dss_isr(&chn->cfg);
    
    if (!chn->pcr_offset.last_pcr_valid) {
        return false;
    }
    if (chn->pcr_offset.count==0) {
        delta = BPCRlib_StcDiff_isr(dss, pts, chn->pcr_offset.last_pcr);
        BDBG_WRN(("[pcr init]%s use PCR:  %#x %#x %d", delta<0?" can't":"", (unsigned)pts, (unsigned)chn->pcr_offset.last_pcr, (int)delta));
        if (delta<0) {
            return false;
        }
#if BCHP_CHIP == 7401
        if (dss)
        {
            *new_stc = chn->pcr_offset.last_pcr - chn->cfg.pcr_offset * 600;
        }
        else
#endif
        {
            *new_stc = chn->pcr_offset.last_pcr - chn->cfg.pcr_offset;
        }
        chn->pcr_offset.last_pcr_valid = false; /* use own PCR not more than one time */
        return true;
    }
    if(chn->pcr_offset.pcr_offset_valid) {
        bool stc_valid;
        uint32_t stc;

        chn->pcr_offset.pcr_offset_valid=false;
        stc_valid = b_read_stc_isr(chn, &stc);
        if (stc_valid) {
            *new_stc = stc + chn->pcr_offset.pcr_offset;
            BDBG_WRN(("use PCR offset %d: PTS %#x PCR %#x old_STC %#x new_STC %#x", (int)chn->pcr_offset.pcr_offset, (unsigned)pts, (unsigned)chn->pcr_offset.last_pcr, (unsigned)stc, (unsigned)*new_stc));
            return true;
        }
    }
    for(offset=0,i=0;i<chn->pcr_offset.count;i++) {
        offset += chn->pcr_offset.pcr_fifo[i];
    }
    offset /= chn->pcr_offset.count;
    BDBG_MSG(("[pcr offset] median offset:  %d", (int)offset));
    if (2*offset > chn->cfg.pcr_offset) {
        offset -= (chn->cfg.pcr_offset/4); /* decresed buffer fullness a little bit */
    }
#if BCHP_CHIP==7401
    if (dss)
    {
        *new_stc = chn->pcr_offset.last_pcr - 2 * offset * 600;
    }
    else
#endif
    {
        *new_stc = chn->pcr_offset.last_pcr - 2 * offset;
    }
    delta = BPCRlib_StcDiff_isr(dss, pts, *new_stc);
    BDBG_WRN(("[pcr offset %d]%s use PCR: PTS %#x PCR %#x STC %#x %d(%d)", 2*offset, (delta*2<chn->cfg.pcr_offset)?""/*can't"*/:"", (unsigned)pts, (unsigned)chn->pcr_offset.last_pcr, (unsigned)*new_stc, (int)delta*2, (int)chn->cfg.pcr_offset));
#if 0
    if (delta*2<chn->cfg.pcr_offset) {
        return false;
    }
#endif
    return true;
}

static bool
BPCRlib_TestVideoStc_isr(BPCRlib_Channel chn, uint32_t new_stc, uint32_t pts)
{
    int32_t delta;
    bool dss = b_is_dss_isr(&chn->cfg);

    delta = BPCRlib_StcDiff_isr(dss, new_stc, pts);

    if (
    /* TODO: does this need dss qualification?  3003 is an MPEG number */
        (delta >= 0 && delta < (chn->cfg.video_pts_offset + (3003 * 5)/(2*2))) ||
        (delta < 0 && delta > -(chn->cfg.video_pts_offset + (3003 * 60)/(2*2)))
    ) /* audio STC could be  0.15 sec ahead or 1.0 sec behind of PTS */
    {
        BDBG_MSG(("TestVideoStc: STC %#x PTS %#x delta %ld (%s)", new_stc, pts, delta, "good"));
        return true;
    } else {
        BDBG_MSG(("TestVideoStc: STC %#x PTS %#x delta %ld (%s)", new_stc, pts, delta, "bad"));
        return false;
    }
}

static bool
BPCRlib_IsAudioLocked_isr(BPCRlib_Channel chn, uint32_t *locked_stc)
{
    BERR_Code rc;
    BAVC_PTSInfo pts;

    if (!chn->cfg.audio) {
        return false;
    }
    BDBG_ASSERT(chn->cfg.audio_iface->getStc);
    rc = chn->cfg.audio_iface->getStc(chn->cfg.aux_transport, chn->cfg.audio, locked_stc);
    if (rc!=BERR_SUCCESS) {
        return false;
    }

    BDBG_ASSERT(chn->cfg.audio_iface->getPts);
    rc = chn->cfg.audio_iface->getPts(chn->cfg.audio, &pts);
    if (rc!=BERR_SUCCESS) {
        return false;
    }
    return BPCRlib_TestAudioStc_isr(chn, *locked_stc, pts.ui32CurrentPTS);
}

static bool
BPCRlib_IsVideoLocked_isr(BPCRlib_Channel chn, uint32_t *locked_stc)
{
    BAVC_PTSInfo pts;
    BERR_Code rc;

    if (!chn->cfg.video) {
        return false;
    }
    BDBG_ASSERT(chn->cfg.video_iface->getPts);
    rc = chn->cfg.video_iface->getPts(chn->cfg.video, &pts);
    if (rc!=BERR_SUCCESS) {
        return false;
    }
    if (pts.ePTSType == BAVC_PTSType_eInterpolatedFromInvalidPTS) {
        return false;
    }
    BDBG_ASSERT(chn->cfg.video_iface->getStc);
    rc = chn->cfg.video_iface->getStc(chn->cfg.aux_transport, chn->cfg.video, locked_stc);
    if (rc!=BERR_SUCCESS) {
        return false;
    }
    return BPCRlib_TestVideoStc_isr(chn, *locked_stc, pts.ui32CurrentPTS);
}

static bool
BPCRlib_IsVideoBufferLevel_isr(BPCRlib_Channel chn)
{
    unsigned cdb_level;
    BERR_Code rc;

    BDBG_ASSERT(chn->cfg.video_iface->getCdbLevel);
    rc = chn->cfg.video_iface->getCdbLevel(chn->cfg.video, &cdb_level);
    if (rc!=BERR_SUCCESS) {
        BDBG_WRN(("Video buffer level request has returned error %d", cdb_level));
        return false;
    }
    BDBG_MSG(("video VBV %u(%d)", cdb_level, chn->cfg.video_cdb_level));
    return cdb_level > 96 * 1024; /* if there is more then 96KBytes in the VBV bufffer, video is just fine with STC */
}

#if 0
static bool
BPCRlib_IsAudioBufferLevel_isr(BPCRlib_Channel chn)
{
    unsigned cdb_level;
    BERR_Code rc;

    rc = chn->cfg.audio_iface->getCdbLevel(chn->cfg.audio, &cdb_level);
    if (rc!=BERR_SUCCESS) {
        BDBG_WRN(("Audio buffer level request has returned error %d", cdb_level));
        return false;
    }
    BDBG_MSG(("audio CDB %u(%d)", cdb_level, chn->cfg.audio_cdb_level));
    return cdb_level > 1 * 1024; /* if there is more then 1KBytes in the CDB bufffer, audio is just fine with STC */
}
#endif

static BPCRlib_TsmMode BPCRlib_Channel_GetTsmMode(BPCRlib_Channel chn)
{
    switch (chn->cfg.tsm_mode) {
    case BPCRlib_TsmMode_eVideoMaster:
        if (!chn->cfg.paired && !chn->cfg.video) {
            return BPCRlib_TsmMode_eSTCMaster;
        }
        break;
    case BPCRlib_TsmMode_eAudioMaster:
        if (!chn->cfg.paired && !chn->cfg.audio && !chn->cfg.secondary_audio && !chn->cfg.tertiary_audio) {
            return BPCRlib_TsmMode_eSTCMaster;
        }
        break;
    default:
        break;
    }
    return chn->cfg.tsm_mode;
}

BERR_Code
BPCRlib_Channel_AudioRequestStc_isr(BPCRlib_Handle handle, void *audio, uint32_t audio_pts)
{
    BPCRlib_Channel chn = find_by_audio_isr(handle, audio);
    BERR_Code rc = BERR_SUCCESS;
    bool dss = false;
    
    if (!chn) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BDBG_MSG(("(%#x) AudioRequestStc(%#x) PTS %#x %s", (unsigned)chn, (unsigned)audio, (unsigned)audio_pts, chn->cfg.playback?"Playback":""));
    dss = b_is_dss_isr(&chn->cfg);
    if (chn->cfg.playback) {
        if (chn->cfg.mode == BPCRlib_Mode_eConstantDelay) {
            return b_load_delayed_stc_isr(chn, audio);
        }

#if 0
        /* invalidation state */
        if (chn->audio_state == BPCRlib_Decoder_State_eInvalidated)
        {
            if (chn->cfg.video)
            {
                BAVC_PTSInfo pts;
                int32_t pts_diff;
                uint32_t new_stc = 0;
                BDBG_ASSERT(chn->cfg.video_iface->getPts);
                rc = chn->cfg.video_iface->getPts(chn->cfg.video, &pts);
                if (rc!=BERR_SUCCESS) {
                    return rc;
                }
                if (pts.ePTSType == BAVC_PTSType_eInterpolatedFromInvalidPTS) {
                    return rc;
                }

                /* master mode exception: take the lower of audio or video PTS at this point
                this only happens when we've been invalidated */
                pts_diff = (int32_t)(audio_pts - pts.ui32CurrentPTS);
                if (pts_diff < 0)
                {
                    new_stc = audio_pts - chn->cfg.audio_pts_offset;
                }
                else
                {
                    new_stc = pts.ui32CurrentPTS - chn->cfg.video_pts_offset;
                }

                BDBG_MSG(("AudioRequestStc(%#x): [invalidated] using lowest PTS %#x as new STC", (unsigned)audio, new_stc));

                chn->video_state = BPCRlib_Decoder_State_eLocked;
                chn->audio_state = BPCRlib_Decoder_State_eLocked;
                rc = b_send_stc_isr(chn, new_stc, audio);
            }
            else /* no video */
            {
                uint32_t new_stc = 0;

                new_stc = audio_pts - chn->cfg.audio_pts_offset;
                BDBG_MSG(("AudioRequestStc(%#x): [invalidated, no video] using audio PTS %#x as new STC", (unsigned)audio, new_stc));
                chn->video_state = BPCRlib_Decoder_State_eLocked;
                chn->audio_state = BPCRlib_Decoder_State_eLocked;
                rc = b_send_stc_isr(chn, new_stc, audio);
            }
        }
        else
#endif
        {
            switch (BPCRlib_Channel_GetTsmMode(chn))
            {
                case BPCRlib_TsmMode_eVideoMaster:

                    if (chn->video_state != BPCRlib_Decoder_State_eWaitingSTC && chn->video_state != BPCRlib_Decoder_State_eInvalidated && chn->cfg.video)
                    {
                        uint32_t new_stc = 0;

                        if (chn->video_state != BPCRlib_Decoder_State_eNotLocked)
                        {
                            if (BPCRlib_IsVideoLocked_isr(chn, &new_stc))
                            {
                                BDBG_MSG(("AudioRequestStc(%#x): Video master mode -> using video STC %#x as new STC", (unsigned)audio, new_stc));
                            }
                            else
                            {
                                BDBG_MSG(("AudioRequestStc(%#x): [video not locked] Video master mode -> awaiting video request", (unsigned)audio));
                                goto no_seed;
                            }
                        }
                        else if (BPCRlib_IsVideoLocked_isr(chn, &new_stc) && BPCRlib_IsVideoBufferLevel_isr(chn))
                        {
                            BDBG_MSG(("AudioRequestStc(%#x): [VBV/Fifo] Video master mode -> using video STC %#x as new STC", (unsigned)audio, new_stc));
                        }
                        else
                        {
                            BDBG_MSG(("AudioRequestStc(%#x): [video not locked] Video master mode -> awaiting video request", (unsigned)audio));
                            goto no_seed;
                        }

                        rc = b_send_stc_isr(chn, new_stc, audio);
                    }
                    else
                    {
                        BDBG_MSG(("AudioRequestStc(%#x): [video not locked] Video master mode -> awaiting video request", (unsigned)audio));
                    }
no_seed:
                    break;

                case BPCRlib_TsmMode_eAudioMaster:

                    if (chn->cfg.secondary_audio == audio || chn->cfg.tertiary_audio == audio)
                    {
                        uint32_t new_stc;
#if BDBG_DEBUG_BUILD
                        const char *channel_name = (chn->cfg.secondary_audio == audio)?"secondary":"tertiary";
#endif

                        if (chn->cfg.audio == NULL)
                        {
                            BDBG_ERR(("(%#x) AudioRequestStc called in audio master mode for %s channel when primary is not set",(unsigned)audio, channel_name));
                            return BERR_TRACE(BERR_INVALID_PARAMETER);
                        }

                        rc = chn->cfg.audio_iface->getStc(chn->cfg.aux_transport, chn->cfg.audio, &new_stc);
                        if (rc != BERR_SUCCESS)
                        {
                            return rc;
                        }

                        /* if primary audio is locked, use its STC, otherwise, wait for primary audio to lock and load its STC */
                        if (chn->audio_state != BPCRlib_Decoder_State_eWaitingSTC && chn->cfg.audio)
                        {
                            if (chn->audio_state != BPCRlib_Decoder_State_eNotLocked)
                            {
                                if (BPCRlib_IsAudioLocked_isr(chn, &new_stc))
                                {
                                    BDBG_MSG(("AudioRequestStc(%#x): Audio master mode -> %s channel, using primary audio STC %#x as new STC", (unsigned)audio, channel_name, new_stc));
                                    rc = b_send_stc_isr(chn, new_stc, audio);
                                }
                            }
                            else if (BPCRlib_IsAudioLocked_isr(chn, &new_stc))
                            {
                                BDBG_MSG(("AudioRequestStc(%#x): [CDB/Fifo] Audio master mode -> %s channel, using primary audio STC %#x as new STC", (unsigned)audio, channel_name, new_stc));
                                rc = b_send_stc_isr(chn, new_stc, audio);
                            }
                        }
                        else
                        {
                            BDBG_MSG(("AudioRequestStc(%#x): Audio master mode -> %s channel request before primary channel request, ignored", (unsigned)audio, channel_name));
                        }
                    }
                    else
                    {
                        uint32_t new_stc = 0;

#if 0
                        if ((signed)audio_pts - (signed)chn->cfg.audio_pts_offset > 0)
                        {
                            new_stc = audio_pts - chn->cfg.audio_pts_offset;
                        }
                        else
#endif
                        {
                            new_stc = audio_pts;
                        }
                        BDBG_MSG(("AudioRequestStc(%#x): Audio master mode -> using audio PTS %#x as new STC %#x", (unsigned)audio, audio_pts, new_stc));
                        BPCRlib_P_InvalidatePCRCache(chn);
                        chn->audio_state = BPCRlib_Decoder_State_eLocked;
                        rc = b_send_stc_isr(chn, new_stc, audio);
                    }

                    break;

                case BPCRlib_TsmMode_eSTCMaster:
                default:

                    if (chn->cfg.secondary_audio == audio || chn->cfg.tertiary_audio == audio)
                    {
                        uint32_t new_stc = 0;
#if BDBG_DEBUG_BUILD
                        const char *channel_name = (chn->cfg.secondary_audio == audio)?"secondary":"tertiary";
#endif

                        if (chn->cfg.audio==NULL)
                        {
                            BDBG_ERR(("(%#x) AudioRequestStc called for %s channel when primary is not set", (unsigned)chn, channel_name));
                            return BERR_TRACE(BERR_INVALID_PARAMETER);
                        }

                        rc = chn->cfg.audio_iface->getStc(chn->cfg.aux_transport, chn->cfg.audio, &new_stc);
                        if (rc!=BERR_SUCCESS)
                        {
                            BDBG_ERR(("AudioRequestStc(%#x): error retrieving STC", (unsigned)audio));
                            return rc;
                        }

                        /* if primary audio is locked, use its STC, otherwise, wait for primary audio to lock and load its STC */
                        if (chn->audio_state != BPCRlib_Decoder_State_eWaitingSTC && chn->cfg.audio)
                        {
                            if (chn->audio_state != BPCRlib_Decoder_State_eNotLocked)
                            {
    /* PR45126 20081022 bandrews - Don't check if primary audio is locked if pcrlib state says it is */
    /*                            if (BPCRlib_IsAudioLocked_isr(chn, &new_stc))*/
                                {
                                    BDBG_MSG(("AudioRequestStc(%#x): %s channel, using primary audio STC %#x as new STC", (unsigned)audio, channel_name, new_stc));
                                    rc = b_send_stc_isr(chn, new_stc, audio);
                                }
#if 0
                                else
                                {
                                    BDBG_MSG(("AudioRequestStc(%#x): secondary audio channel, primary audio state not eNotLocked, but IsAudioLocked returned false", (unsigned)audio));
                                }
#endif
                            }
                            else if (BPCRlib_IsAudioLocked_isr(chn, &new_stc))
                            {
                                BDBG_MSG(("AudioRequestStc(%#x): [CDB/Fifo] %s channel, using primary audio STC %#x as new STC", (unsigned)audio, channel_name, new_stc));
                                b_send_stc_isr(chn, new_stc, audio);
                            }
                            else
                            {
                                BDBG_MSG(("AudioRequestStc(%#x): secondary audio channel, primary audio state eNotLocked and IsAudioLocked returned false", (unsigned)audio));
                            }
                        }
                        else
                        {
                            BDBG_MSG(("AudioRequestStc(%#x): %s channel request before primary channel request, ignored", (unsigned)audio, channel_name));
                        }
                    }
                    else
                    {
                        uint32_t new_stc = 0;

                        if(b_read_stc_isr(chn, &new_stc) && BPCRlib_TestAudioStc_isr(chn, new_stc, audio_pts))
                        {
                            BDBG_MSG(("AudioRequestStc(%#x): primary audio reusing old STC %#x as new STC", (unsigned)audio, new_stc));
                            goto set_stc;
                        }
                        if (chn->video_state != BPCRlib_Decoder_State_eWaitingSTC && chn->cfg.video)
                        {
                            if (chn->video_state != BPCRlib_Decoder_State_eNotLocked)
                            {
                                if (BPCRlib_IsVideoLocked_isr(chn, &new_stc))
                                {
                                    BDBG_MSG(("AudioRequestStc(%#x): primary audio Using video STC %#x as new STC", (unsigned)audio, new_stc));
                                    goto set_stc;
                                }
                            }
                            else if (BPCRlib_IsVideoBufferLevel_isr(chn) && BPCRlib_IsVideoLocked_isr(chn, &new_stc))
                            {
                                BDBG_MSG(("AudioRequestStc(%#x): primary audio [VBV/Fifo] Using video STC %#x as new STC", (unsigned)audio, new_stc));
                                goto set_stc;
                            }
                        }
                        if (BPCRlib_P_GetStcFromPcr_isr(chn, audio_pts, &new_stc) && BPCRlib_TestAudioStc_isr(chn, new_stc, audio_pts))
                        {
                            BDBG_MSG(("AudioRequestStc(%#x): primary audio Using PCR %#x as new STC %#x", (unsigned)audio, chn->pcr_offset.last_pcr, new_stc));
                            BPCRlib_P_InvalidatePCRCache(chn);
                            goto set_stc;
                        }
                        if (chn->cfg.non_real_time)
                        {
                            new_stc = audio_pts;
                        }
                        else
                        {
#if BCHP_CHIP == 7401
                            if (dss)
                            {
                                new_stc = audio_pts - chn->cfg.audio_pts_offset * 600;
                            }
                            else
#endif
                            {
                                new_stc = audio_pts - chn->cfg.audio_pts_offset;
                            }
                        }
                        BDBG_MSG(("AudioRequestStc(%#x): primary audio Using audio PTS %#x as new STC %#x", (unsigned)audio, audio_pts, new_stc));
                        BPCRlib_P_InvalidatePCRCache(chn);
    set_stc:
                        chn->audio_state = BPCRlib_Decoder_State_eLocked;
                        rc = b_send_stc_isr(chn, new_stc, audio);
                    }
                    break;
            } /* switch */
        } /* invalidate state */
    }
    else /* live */
    {
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
        return b_update_stc_isr(chn, true);
#else
        return b_update_stc_isr(chn, audio, true);
#endif
    }

    return rc;
}


BERR_Code
BPCRlib_Channel_VideoRequestStc_isr(BPCRlib_Handle handle, void *video, const BAVC_PTSInfo *video_pts)
{
    BPCRlib_Channel chn = find_by_video_isr(handle, video);
    BERR_Code rc = BERR_SUCCESS;
    bool dss = false;

    BDBG_ASSERT(video_pts);
    if (!chn) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BDBG_MSG(("(%#x) VideoRequestStc(%#x) PTS %#x(%d) %s", (unsigned)chn, (unsigned)video, (unsigned)video_pts->ui32CurrentPTS, (int)video_pts->ePTSType, chn->cfg.playback?"Playback":""));
    dss = b_is_dss_isr(&chn->cfg);
    if (chn->cfg.playback) 
    {
        uint32_t new_stc = 0;

        chn->sVideo.uiConsecutivePtsErrorCount = 0;
        chn->sVideo.uiLastDecodedFrameCount = 0;
        chn->sVideo.uiLastDroppedFrameCount = 0;

        if (chn->cfg.mode == BPCRlib_Mode_eConstantDelay) {
            return b_load_delayed_stc_isr(chn, video);
        }

#if 0
        /* invalidation state */
        if (chn->video_state == BPCRlib_Decoder_State_eInvalidated)
        {
            if (chn->cfg.audio)
            {
                BAVC_PTSInfo pts;
                int32_t pts_diff;
                uint32_t new_stc = 0;
                BDBG_ASSERT(chn->cfg.audio_iface->getPts);
                rc = chn->cfg.audio_iface->getPts(chn->cfg.audio, &pts);
                if (rc!=BERR_SUCCESS) {
                    return rc;
                }
                if (pts.ePTSType == BAVC_PTSType_eInterpolatedFromInvalidPTS) {
                    return rc;
                }

                /* master mode exception: take the lower of audio or video PTS at this point
                this only happens when we've been invalidated */
                pts_diff = (int32_t)(video_pts->ui32CurrentPTS - pts.ui32CurrentPTS);
                if (pts_diff < 0)
                {
                    new_stc = video_pts->ui32CurrentPTS - chn->cfg.video_pts_offset;
                }
                else
                {
                    new_stc = pts.ui32CurrentPTS - chn->cfg.audio_pts_offset;
                }

                chn->video_state = BPCRlib_Decoder_State_eLocked;
                chn->audio_state = BPCRlib_Decoder_State_eLocked;

                BDBG_MSG(("VideoRequestStc(%#x): [invalidated] using lowest PTS %#x as new STC", (unsigned)video, new_stc));
                rc = b_send_stc_isr(chn, new_stc, video);
            }
            else /* no audio */
            {
                uint32_t new_stc = 0;

                new_stc = video_pts->ui32CurrentPTS - chn->cfg.video_pts_offset;
                BDBG_MSG(("VideoRequestStc(%#x): [invalidated, no audio] using video PTS %#x as new STC", (unsigned)video, new_stc));
                chn->video_state = BPCRlib_Decoder_State_eLocked;
                chn->audio_state = BPCRlib_Decoder_State_eLocked;
                rc = b_send_stc_isr(chn, new_stc, video);
            }
        }
        else
#endif
        {
            switch (BPCRlib_Channel_GetTsmMode(chn))
            {
                case BPCRlib_TsmMode_eAudioMaster:

                    if (BPCRlib_IsAudioLocked_isr(chn, &new_stc) && chn->audio_state == BPCRlib_Decoder_State_eLocked)
                    {
                        BDBG_MSG(("VideoRequestStc(%#x): Audio master mode -> using audio STC %#x as new STC", (unsigned)video, new_stc));
                        chn->video_state = BPCRlib_Decoder_State_eLocked;
                        rc = b_send_stc_isr(chn, new_stc, video);
                    }
                    else
                    {
                        BDBG_MSG(("VideoRequestStc(%#x): [audio not locked] Audio master mode -> awaiting audio request", (unsigned)video));
                    }
                    break;

                case BPCRlib_TsmMode_eVideoMaster:

                    if (video_pts->ePTSType == BAVC_PTSType_eInterpolatedFromInvalidPTS) {
                        BDBG_MSG(("VideoRequestStc(%#x): Video master mode -> interpolated PTS ignored, STC not updated", (unsigned)video));
                        chn->video_state = BPCRlib_Decoder_State_eWaitingSTC;
                        return BERR_SUCCESS;
                    }

                    if (chn->cfg.non_real_time)
                    {
                        new_stc = video_pts->ui32CurrentPTS;
                    }
                    else
                    {
#if BCHP_CHIP == 7401
                        if (dss)
                        {
                            new_stc = video_pts->ui32CurrentPTS - chn->cfg.video_pts_offset * 600;
                        }
                        else
#endif
                        {
                            new_stc = video_pts->ui32CurrentPTS - chn->cfg.video_pts_offset;
                        }
                    }
                    BDBG_MSG(("VideoRequestStc(%#x): Video master mode -> using video PTS %#x as new STC", (unsigned)video, new_stc));
                    BPCRlib_P_InvalidatePCRCache(chn);

                    chn->video_state = BPCRlib_Decoder_State_eLocked;
                    rc = b_send_stc_isr(chn, new_stc, video);
                    break;

                case BPCRlib_TsmMode_eSTCMaster:
                default:

                    if (BPCRlib_IsAudioLocked_isr(chn, &new_stc) && chn->audio_state==BPCRlib_Decoder_State_eLocked) {
                        int32_t delta;

                        delta = BPCRlib_StcDiff_isr(dss, new_stc, video_pts->ui32CurrentPTS);
                        /* TODO: 3003 is an MPEG number, needs dssification? */
                        /* if audio is more 2 seconds behind video, use video PTS */
                        if (delta< (3003 * 2)/(2))
                        {
                            goto set_video_stc;
                        }
                        BDBG_MSG(("VideoRequestStc(%#x): Using audio STC %#x as new STC", (unsigned)video, new_stc));
                        goto set_stc;
                    }

                    if (video_pts->ePTSType == BAVC_PTSType_eInterpolatedFromInvalidPTS) {
                        if (chn->cfg.audio) {
                            rc = chn->cfg.audio_iface->getStc(chn->cfg.aux_transport, chn->cfg.audio, &new_stc);
                            if (rc==BERR_SUCCESS) {
                                BDBG_MSG(("VideoRequestStc(%#x): Interpolated PTS ignored, using audio STC %#x", (unsigned)video, new_stc));
                                goto set_stc;
                            }
                        }
                        BDBG_MSG(("VideoRequestStc(%#x): Interpolated PTS ignored, STC not updated", (unsigned)video));
                        chn->video_state = BPCRlib_Decoder_State_eWaitingSTC;
                        return BERR_SUCCESS;
                    }
                    if(b_read_stc_isr(chn, &new_stc) && BPCRlib_TestVideoStc_isr(chn, new_stc, video_pts->ui32CurrentPTS)) {
                        BDBG_MSG(("VideoRequestStc(%#x): reusing old STC %#x as new STC", (unsigned)video, new_stc));
                        goto set_stc;
                    }
                    if (BPCRlib_P_GetStcFromPcr_isr(chn, video_pts->ui32CurrentPTS, &new_stc)
                            /* && BPCRlib_TestVideoStc_isr(chn, new_stc, video_pts->ui32CurrentPTS)*/
                        ) {
                        BDBG_MSG(("VideoRequestStc(%#x): Using PCR %#x as new STC %#x", (unsigned)video, chn->pcr_offset.last_pcr, new_stc));
                        BPCRlib_P_InvalidatePCRCache(chn);
                        goto set_stc;
                    }
    set_video_stc:
                    if (chn->cfg.non_real_time)
                    {
                        new_stc = video_pts->ui32CurrentPTS;
                    }
                    else
                    {
#if BCHP_CHIP == 7401
                        if (dss)
                        {
                            new_stc = video_pts->ui32CurrentPTS - chn->cfg.video_pts_offset * 600;
                        }
                        else
#endif
                        {
                            new_stc = video_pts->ui32CurrentPTS - chn->cfg.video_pts_offset;
                        }
                    }
                    BDBG_MSG(("VideoRequestStc(%#x): Using video PTS %#x as new STC", (unsigned)video, new_stc));
                    BPCRlib_P_InvalidatePCRCache(chn);
    set_stc:
                    chn->video_state = BPCRlib_Decoder_State_eLocked;
                    rc = b_send_stc_isr(chn, new_stc, video);
                    break;

            } /* switch */
        } /* invalidation state */
    }
    else
    {
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
        return b_update_stc_isr(chn, true);
#else
        return b_update_stc_isr(chn, video, true);
#endif
    }
    return rc;
}

BERR_Code
BPCRlib_Channel_VideoPtsError_isr( BPCRlib_Handle handle, void *video, const BAVC_PTSInfo *video_pts, uint32_t video_stc)
{
    BPCRlib_Channel chn = find_by_video_isr(handle, video);
    BERR_Code rc = BERR_SUCCESS;
    unsigned cdb_level;
    int pts_offset;
    bool dss = false;

    BDBG_ASSERT(video_pts);

    if (!chn) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    dss = b_is_dss_isr(&chn->cfg);
    BDBG_MSG(("(%#x) VideoPtsError(%#x) PTS %#x(%d) STC %#x %d %s %u", (unsigned)chn, (unsigned)video, (unsigned)video_pts->ui32CurrentPTS, (int)video_pts->ePTSType, (unsigned)video_stc,  2*BPCRlib_StcDiff_isr(dss, video_stc, video_pts->ui32CurrentPTS), chn->cfg.playback?"Playback":"", chn->cfg.video_iface->getCdbLevel?chn->cfg.video_iface->getCdbLevel(chn->cfg.video, &cdb_level),cdb_level:0));
    if (chn->cfg.playback) {
        uint32_t new_stc;

        if (chn->cfg.mode == BPCRlib_Mode_eConstantDelay) {
            if(chn->delayed_stc.state==delayed_stc_eWaiting) {
                return b_load_delayed_stc_isr(chn, video);
            } else {
                return BERR_SUCCESS;
            }
        }

        if (video_pts->ePTSType == BAVC_PTSType_eInterpolatedFromInvalidPTS) {
            BDBG_MSG(("Interpolated PTS, ignored"));
            return BERR_SUCCESS;
        }

#if 0
        if (chn->video_state == BPCRlib_Decoder_State_eWaitingSTC)
        {
            chn->video_state = BPCRlib_Decoder_State_eInvalidated;
            BDBG_MSG(("Invalidated -> converting PTS error to request STC"));
            /* we've been invalidated, and we need to treat this pts error as request stc */
            return BPCRlib_Channel_VideoRequestStc_isr(chn->parent, video, video_pts);
        }
#endif

        switch (BPCRlib_Channel_GetTsmMode(chn))
        {
            case BPCRlib_TsmMode_eOutputMaster:
            case BPCRlib_TsmMode_eAudioMaster:

                return rc;
                break;

            case BPCRlib_TsmMode_eVideoMaster:

                pts_offset = chn->cfg.video_pts_offset;
                if (chn->cfg.video_iface->getCdbLevel && chn->cfg.video_iface->getCdbLevel(chn->cfg.video, &cdb_level)==BERR_SUCCESS && cdb_level > chn->cfg.video_cdb_level) {
                    pts_offset = 0;
                }
#if BCHP_CHIP == 7401
                if (dss)
                {
                    new_stc = video_pts->ui32CurrentPTS - pts_offset * 600;
                }
                else
#endif
                {
                    new_stc = video_pts->ui32CurrentPTS - pts_offset;
                }
                BDBG_MSG(("VideoPtsError(%#x): Using video PTS %#x[%d] as new STC %#x (old %#x)", (unsigned)video, (unsigned)video_pts->ui32CurrentPTS, (int)pts_offset, (unsigned)new_stc, (unsigned)video_stc));

                rc = b_send_stc_isr(chn, new_stc, video);
                chn->video_state = BPCRlib_Decoder_State_eLocked;

                break;

            case BPCRlib_TsmMode_eSTCMaster:
            default:

#if 1
                /* 20070920 bandrews - added to detect DM-unrecoverable video error condition */
                if ((signed)video_pts->ui32CurrentPTS - (signed)video_stc > chn->cfg.video_stc_discard)
                {
                    BDBG_MSG(("Video STC/PTS difference outside of discard threshold"));
                    BDBG_MSG(("Last decoded frame count: %d; Current decoded frame count: %d", chn->sVideo.uiLastDecodedFrameCount, video_pts->uiDecodedFrameCount));
                    if (chn->sVideo.uiLastDecodedFrameCount && chn->sVideo.uiLastDecodedFrameCount == video_pts->uiDecodedFrameCount - 1)
                    {
                        BDBG_MSG(("Last dropped frame count: %d; Current dropped frame count: %d", chn->sVideo.uiLastDroppedFrameCount, video_pts->uiDroppedFrameCount));
                        /* no frames have been decoded by this channel between errors */
                        if (chn->sVideo.uiLastDroppedFrameCount < video_pts->uiDroppedFrameCount)
                        {
                            /* frames have been dropped by this channel between errors */
                            chn->sVideo.uiConsecutivePtsErrorCount++;
                            BDBG_MSG(("Consecutive PTS errors detected: %d", chn->sVideo.uiConsecutivePtsErrorCount));
                        }
                    }
                    else
                    {
                        /* set the error count to 1 on the first one after a series of good frames, since we are looking
                        for consecutive errors */
                        chn->sVideo.uiConsecutivePtsErrorCount = 1;
                        BDBG_MSG(("Consecutive PTS errors detected: %d", chn->sVideo.uiConsecutivePtsErrorCount));
                    }

                    /* update the frame counters */
                    chn->sVideo.uiLastDecodedFrameCount = video_pts->uiDecodedFrameCount;
                    chn->sVideo.uiLastDroppedFrameCount = video_pts->uiDroppedFrameCount;

                    if (chn->sVideo.uiConsecutivePtsErrorCount >= chn->cfg.consecutive_pts_error_limit)
                    {
                        BDBG_MSG(("Consecutive PTS error count reached"));
                        /* clear the error counter */
                        chn->sVideo.uiConsecutivePtsErrorCount = 0;
                        /* we have reached a DM-unrecoverable error, reseed STC with video PTS */
                        goto set_stc_from_pts;
                    }
                }
#endif

                /* first test if PTS-STC is inside tracking range */
                if (BPCRlib_TestVideoStc_isr(chn, video_stc, video_pts->ui32CurrentPTS)) {
                    int32_t stc_delta;

                    stc_delta=BPCRlib_StcDiff_isr(dss, video_stc, video_pts->ui32CurrentPTS);
                    if (stc_delta>0 && chn->cfg.video_iface->getCdbLevel) {
                        /* STC is ahead of STC, so decoder would drop a frame */
                        rc = chn->cfg.video_iface->getCdbLevel(chn->cfg.video, &cdb_level);
                        if (rc==BERR_SUCCESS && cdb_level<chn->cfg.video_cdb_level) {
                            /* decoder compressed buffer is too shallow, force STC reload to pause decoder and let it accumulate some data */
                            BDBG_MSG(("video decoder is underflowed %ld,%u:%u", stc_delta, cdb_level, chn->cfg.video_cdb_level));
                            goto set_stc_from_pts;
                        }
                    }
                    BDBG_MSG(("Video is still in range, ignored"));
                    chn->video_state = BPCRlib_Decoder_State_eLocking;
                    return BERR_SUCCESS;
                } 
                else 
                {
                    /* likely discontinuity in the stream, however if audio is just happy with STC, let it go */
                    if (BPCRlib_IsAudioLocked_isr(chn, &new_stc)) 
                    {
                        BDBG_MSG(("Video discontinuity detected, but audio is still locked, ignored"));
                        chn->video_state = BPCRlib_Decoder_State_eNotLocked;
                        return BERR_SUCCESS;
                    }
                    if (BPCRlib_P_GetStcFromPcr_isr(chn, video_pts->ui32CurrentPTS, &new_stc)
                            /* && BPCRlib_TestVideoStc_isr(chn, new_stc, video_pts->ui32CurrentPTS) */
                    ) 
                    {
                        BDBG_MSG(("VideoPtsError(%#x): Using PCR %#x as new STC %#x (old %#x)", (unsigned)video, (unsigned)chn->pcr_offset.last_pcr, (unsigned)new_stc, (unsigned)video_stc));
                        BPCRlib_P_InvalidatePCRCache(chn);
                        goto set_stc;
                    }
                    goto set_stc_from_pts;
                }
set_stc_from_pts:
                pts_offset = chn->cfg.video_pts_offset;
                if (chn->cfg.video_iface->getCdbLevel && chn->cfg.video_iface->getCdbLevel(chn->cfg.video, &cdb_level)==BERR_SUCCESS && cdb_level > chn->cfg.video_cdb_level) 
                {
                    pts_offset = 0;
                }
#if BCHP_CHIP == 7401
                if (dss)
                {
                    new_stc = video_pts->ui32CurrentPTS - pts_offset * 600;
                }
                else
#endif
                {
                    new_stc = video_pts->ui32CurrentPTS - pts_offset;
                }
                BDBG_MSG(("VideoPtsError(%#x): Using video PTS %#x[%d] as new STC %#x (old %#x)", (unsigned)video, (unsigned)video_pts->ui32CurrentPTS, (int)pts_offset, (unsigned)new_stc, (unsigned)video_stc));
set_stc:
                rc = b_send_stc_isr(chn, new_stc, video);
                chn->video_state = BPCRlib_Decoder_State_eLocked;

                break;
        }
    } else {
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
        return b_update_stc_isr(chn, false);
#else
        return b_update_stc_isr(chn, video, false);
#endif
    }
    return rc;
}

#define BPCRLIB_MILLISECONDS_TO_PTS_TICKS(CFG, X) (b_is_dss_isr(CFG) ? (X) * 27000 : (X) * 45)

BERR_Code
BPCRlib_Channel_AudioPtsError_isr(BPCRlib_Handle handle, void *audio, const BAVC_PTSInfo *audio_pts, uint32_t audio_stc)
{
    BPCRlib_Channel chn = find_by_audio_isr(handle, audio);
    BERR_Code rc = BERR_SUCCESS;
    unsigned cdb_level;
    int pts_offset;
    bool dss = false;

    BDBG_ASSERT(audio_pts);

    if (!chn) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    dss = b_is_dss_isr(&chn->cfg);
    BDBG_MSG(("(%#x) AudioPtsError(%#x) PTS %#x(%d) STC %#x %d %s %u", (unsigned)chn, (unsigned)audio, (unsigned)audio_pts->ui32CurrentPTS, (int)audio_pts->ePTSType, (unsigned)audio_stc, 2*BPCRlib_StcDiff_isr(dss, audio_stc, audio_pts->ui32CurrentPTS), chn->cfg.playback?"Playback":"", chn->cfg.audio_iface->getCdbLevel?chn->cfg.audio_iface->getCdbLevel(chn->cfg.audio, &cdb_level),cdb_level:0));
    if (chn->cfg.playback) {
        uint32_t new_stc;
        int32_t stc_delta;

        if (chn->cfg.secondary_audio == audio) {
            BDBG_MSG(("secondary audio ignored"));
            return BERR_SUCCESS;
        }

        if (chn->cfg.tertiary_audio == audio) {
            BDBG_MSG(("tertiary audio ignored"));
            return BERR_SUCCESS;
        }

        if (chn->cfg.mode == BPCRlib_Mode_eConstantDelay) {
            if(chn->delayed_stc.state==delayed_stc_eWaiting) {
                return b_load_delayed_stc_isr(chn, audio);
            } else {
                return BERR_SUCCESS;
            }
        }

        if (audio_pts->ePTSType == BAVC_PTSType_eInterpolatedFromInvalidPTS) {
            BDBG_MSG(("Interpolated PTS ignored"));
            return BERR_SUCCESS;
        }

#if 0
        if (chn->audio_state == BPCRlib_Decoder_State_eWaitingSTC)
        {
            chn->audio_state = BPCRlib_Decoder_State_eInvalidated;
            BDBG_MSG(("Invalidated -> converting PTS error to request STC"));
            /* we've been invalidated, and we need to treat this pts error as request stc */
            return BPCRlib_Channel_AudioRequestStc_isr(chn->parent, audio, audio_pts->ui32CurrentPTS);
        }
#endif

        switch (BPCRlib_Channel_GetTsmMode(chn))
        {
            case BPCRlib_TsmMode_eOutputMaster:
            case BPCRlib_TsmMode_eVideoMaster:

                return rc;
                break;

            case BPCRlib_TsmMode_eAudioMaster:

                pts_offset = chn->cfg.audio_pts_offset;
                stc_delta = (signed)audio_stc - (signed)audio_pts->ui32CurrentPTS;

                if (stc_delta > BPCRLIB_MILLISECONDS_TO_PTS_TICKS(&chn->cfg, (signed)chn->cfg.sync_limit)
                    || stc_delta < - BPCRLIB_MILLISECONDS_TO_PTS_TICKS(&chn->cfg, (signed)chn->cfg.sync_limit)) /* sync limit in PTS ticks*/
                {
                    BDBG_MSG(("Outside of sync limit, requesting audio CDB flush"));
#if BCHP_CHIP == 7401
                    if (dss)
                    {
                        new_stc = audio_pts->ui32CurrentPTS - pts_offset * 600;
                    }
                    else
#endif
                    {
                        new_stc = audio_pts->ui32CurrentPTS - pts_offset;
                    }
                    if (chn->cfg.flush)
                    {
                        BKNI_SetEvent(chn->cfg.flush);
                    }
                }
                else
                {
#if 0
                    if (!BPCRlib_IsAudioBufferLevel_isr(chn))
                    {
                        BDBG_MSG(("Audio CDB underflow, applying pts offset"));
                        new_stc = audio_pts->ui32CurrentPTS - pts_offset;
                    }
                    else
#endif
                    {
                        new_stc = audio_pts->ui32CurrentPTS;
                    }
                }
                BDBG_MSG(("AudioPtsError(%#x): Using audio PTS %#x as new STC %#x (old %#x)", (unsigned)audio, (unsigned)audio_pts->ui32CurrentPTS, (unsigned)new_stc, (unsigned)audio_stc));

                rc = b_send_stc_isr(chn, new_stc, audio);
                chn->audio_state = BPCRlib_Decoder_State_eLocked;
                return BERR_SUCCESS;

                break;

            case BPCRlib_TsmMode_eSTCMaster:
            default:

                stc_delta=BPCRlib_StcDiff_isr(dss, audio_stc, audio_pts->ui32CurrentPTS);
                /* first test if PTS-STC is inside tracking range */
                if ( (stc_delta>0 && stc_delta<chn->cfg.audio_stc_discard) && chn->cfg.audio_iface->getCdbLevel) {
                    /* STC is ahead of STC, so decoder would drop a frame */
                    rc = chn->cfg.audio_iface->getCdbLevel(chn->cfg.audio, &cdb_level);
                    if (rc==BERR_SUCCESS && cdb_level<chn->cfg.audio_cdb_level) {
                        /* decoder compressed buffer is too shallow, force STC reload to pause decoder and let it accumulate some data */
                        BDBG_MSG(("audio decoder is underflowed %ld,%u:%u", stc_delta, cdb_level, chn->cfg.audio_cdb_level));
                        goto set_stc_from_pts;
                    }
                }
                if (BPCRlib_TestAudioStc_isr(chn, audio_stc, audio_pts->ui32CurrentPTS)) {
                    goto ignored;
                }
                /* likely discontinuity in the stream, however if vudei is just happy with STC, let it go */
                if (BPCRlib_IsVideoLocked_isr(chn, &new_stc)) {
                    BDBG_MSG(("Audio discontinuity detected, but video is still locked, ignored"));
                    chn->audio_state = BPCRlib_Decoder_State_eNotLocked;
                    return BERR_SUCCESS;
                }
                if (BPCRlib_P_GetStcFromPcr_isr(chn, audio_pts->ui32CurrentPTS, &new_stc)
                        /* && BPCRlib_TestAudioStc_isr(chn, new_stc, audio_pts->ui32CurrentPTS) */
                    ) {
                    BDBG_MSG(("AudioPtsError(%#x): Using PCR %#x as new STC %#x (old %#x)", (unsigned)audio, (unsigned)chn->pcr_offset.last_pcr, (unsigned)new_stc, (unsigned)audio_stc));
                    BPCRlib_P_InvalidatePCRCache(chn);
                    goto set_stc;
                }
                goto set_stc_from_pts;
set_stc_from_pts:
                pts_offset = chn->cfg.audio_pts_offset;
                if (chn->cfg.audio_iface->getCdbLevel && chn->cfg.audio_iface->getCdbLevel(chn->cfg.audio, &cdb_level)==BERR_SUCCESS && cdb_level > chn->cfg.audio_cdb_level) {
                    pts_offset = 0;
                }
#if BCHP_CHIP == 7401
                if (dss)
                {
                   new_stc = audio_pts->ui32CurrentPTS - pts_offset * 600;
                }
                else
#endif
                {
                   new_stc = audio_pts->ui32CurrentPTS - pts_offset;
                }
                BDBG_MSG(("AudioPtsError(%#x): Using audio PTS %#x[%d] as new STC %#x (old %#x)", (unsigned)audio, (unsigned)audio_pts->ui32CurrentPTS, (int)pts_offset, (unsigned)new_stc, (unsigned)audio_stc));
set_stc:
                rc = b_send_stc_isr(chn, new_stc, audio);
                chn->audio_state = BPCRlib_Decoder_State_eLocked;
                return BERR_SUCCESS;
ignored:
                BDBG_MSG(("Audio is still in range, ignored"));
                chn->audio_state = BPCRlib_Decoder_State_eLocking;
                return BERR_SUCCESS;

                break;
        }
    } 
    else 
    {
#if defined (B_HAS_IP) && defined (BCHP_7411_VER)
        return b_update_stc_isr(chn, false);
#else
        return b_update_stc_isr(chn, audio, false);
#endif
    }
    return rc;

}

static void
BPCRlib_P_InvalidatePCRCache(BPCRlib_Channel chn)
{
    chn->pcr_offset.count = 0;
    chn->pcr_offset.index = 0;
    chn->pcr_offset.last_pcr_valid=false;
    chn->pcr_offset.pcr_offset_valid=false;
    return;
}

bool BPCRlib_P_GetAudioPts_isr(BPCRlib_Channel chn, uint32_t * pts)
{
    bool valid = false;
    BAVC_PTSInfo audio_pts;
    BERR_Code rc = BERR_SUCCESS;
    
    if (chn->cfg.audio)
    {
        BDBG_ASSERT(chn->cfg.audio_iface->getPts);
        rc = chn->cfg.audio_iface->getPts(chn->cfg.audio, &audio_pts);
        if (rc!=BERR_SUCCESS) goto end; /* 20110516 bandrews - do not trace, failure is normal and handled */

        if (audio_pts.ePTSType == BAVC_PTSType_eInterpolatedFromValidPTS 
            || audio_pts.ePTSType == BAVC_PTSType_eCoded)
        {
            BDBG_MSG(("Audio returned valid (%u) PTS of %#x", audio_pts.ePTSType, audio_pts.ui32CurrentPTS));
            *pts = audio_pts.ui32CurrentPTS;
            valid = true;
            goto end;
        }
    }

end:
    return valid;
}

bool BPCRlib_P_GetVideoPts_isr(BPCRlib_Channel chn, uint32_t * pts)
{
    bool valid = false;
    BAVC_PTSInfo video_pts;
    BERR_Code rc = BERR_SUCCESS;
    
    if (chn->cfg.video)
    {
        BDBG_ASSERT(chn->cfg.video_iface->getPts);
        rc = chn->cfg.video_iface->getPts(chn->cfg.video, &video_pts);
        if (rc!=BERR_SUCCESS) {
            BERR_TRACE(rc);
            goto end;
        }

        if (video_pts.ePTSType == BAVC_PTSType_eInterpolatedFromValidPTS 
            || video_pts.ePTSType == BAVC_PTSType_eCoded)
        {
            BDBG_MSG(("Video returned valid (%u) PTS of %#x", video_pts.ePTSType, video_pts.ui32CurrentPTS));
            *pts = video_pts.ui32CurrentPTS;
            valid = true;
            goto end;
        }
    }

end:
    return valid;
}

BERR_Code
BPCRlib_Channel_Invalidate(BPCRlib_Channel chn)
{
    BERR_Code rc = BERR_SUCCESS;
    BDBG_ASSERT(chn);
    BDBG_MSG(("Invalidate: %#x", (unsigned)chn));
    chn->audio_state = BPCRlib_Decoder_State_eWaitingSTC;
    chn->video_state = BPCRlib_Decoder_State_eWaitingSTC;
    chn->delayed_stc.state = delayed_stc_eInvalid;
    BPCRlib_P_InvalidatePCRCache(chn);

    /* handle invalidate call as part of playback resume, try to get lowest
    PTS to seed STC before errors start coming */
    /* 20111117 SW7346-544 bandrews - include playback flag, as some clients are calling this fn in live */
    if (chn->cfg.playback && chn->cfg.refresh_stc_on_invalidate && !chn->cfg.non_real_time)
    {
        uint32_t audio_pts = 0;
        uint32_t video_pts = 0;
        int32_t pts_diff;
        uint32_t new_stc = 0;
        bool audio_valid;
        bool video_valid;

        BKNI_EnterCriticalSection();
        audio_valid = BPCRlib_P_GetAudioPts_isr(chn, &audio_pts);
        video_valid = BPCRlib_P_GetVideoPts_isr(chn, &video_pts);

        if (audio_valid && video_valid)
        {
            pts_diff = (int32_t)(video_pts - audio_pts);
            if (pts_diff < 0)
            {
                new_stc = video_pts - chn->cfg.video_pts_offset;
            }
            else
            {
                new_stc = audio_pts - chn->cfg.audio_pts_offset;
            }

            BDBG_MSG(("Invalidate: using lowest PTS %#x as new STC", new_stc));
        }
        else if (audio_valid)
        {
            new_stc = audio_pts - chn->cfg.audio_pts_offset;
            BDBG_MSG(("Invalidate: [video invalid] using audio PTS %#x as new STC", new_stc));
        }
        else if (video_valid)
        {
            new_stc = video_pts - chn->cfg.video_pts_offset;
            BDBG_MSG(("Invalidate: [audio invalid] using video PTS %#x as new STC", new_stc));
        }
        else
        {
            BDBG_MSG(("Invalidate: [video, audio invalid] no change in STC"));
            BKNI_LeaveCriticalSection();
            goto end;
        }

        chn->video_state = BPCRlib_Decoder_State_eLocked;
        chn->audio_state = BPCRlib_Decoder_State_eLocked;
        rc = b_really_send_stc_isr(chn, new_stc);
        BKNI_LeaveCriticalSection();
    }

end:
    return rc;
}

BERR_Code
BPCRlib_Channel_GetStc(BPCRlib_Channel chn, uint32_t *stc)
{
    BERR_Code rc = BERR_SUCCESS;
    uint32_t stc_low;

    BDBG_ASSERT(chn);

    BKNI_EnterCriticalSection();
#if B_PCRLIB_HAS_PCROFFSET
    /* Systems with PCROFFSET require the aux_transport setting and do not use the pcr setting. */
    *stc = BXPT_PcrOffset_GetStc_isr(chn->cfg.aux_transport) + BXPT_PcrOffset_GetOffset_isr(chn->cfg.aux_transport);
#else
    /* Systems without PCROFFSET require the pcr setting. */
    BDBG_ASSERT(chn->pcr);
    rc = BXPT_PCR_GetStc_isr(chn->pcr, stc, &stc_low /* not used */ );
#endif
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto done;
    }
    if (!chn->cfg.playback || chn->cfg.mode!=BPCRlib_Mode_eConstantDelay) {
        goto done;
    }
    if (chn->delayed_stc.state == delayed_stc_eInvalid) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto done;
    }
    stc_low = chn->delayed_stc.stc + (*stc - chn->delayed_stc.old_stc);
    BDBG_MSG_TRACE(("BPCRlib_Channel_GetStc: %#lx returning STC %#x (%u+%d)", (unsigned long)chn, (unsigned)stc_low, (unsigned)chn->delayed_stc.stc, (int)(*stc - chn->delayed_stc.old_stc)));
    *stc = stc_low;
done:
    BKNI_LeaveCriticalSection();
    return rc;
}


BERR_Code
BPCRlib_Channel_PcrUpdate(BPCRlib_Channel chn, uint32_t pcr)
{
    BERR_Code rc = BERR_SUCCESS;
    uint32_t stc=0;
    bool stc_valid;
    int32_t delta=0;
    bool send_video_stc=false;
    bool send_audio_stc=false;

    if (!chn || !chn->cfg.playback) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if (chn->cfg.mode==BPCRlib_Mode_eConstantDelay) {
        BKNI_EnterCriticalSection();

        switch(chn->delayed_stc.state) {
        case delayed_stc_eLocked:
        case delayed_stc_eWaiting:
#if 0
            rc = BXPT_PCR_GetStc_isr(chn->pcr, &chn->delayed_stc.old_stc, &chn->delayed_stc.stc /* not used */ );
            chn->delayed_stc.stc = pcr;
            BDBG_MSG(("BPCRlib_Channel_PcrUpdate: %#lx saving STC %u (%d)", (unsigned long)chn, (unsigned)pcr, (int)(pcr - chn->delayed_stc.old_stc)));
            chn->delayed_stc.state = delayed_stc_eWaiting; /* don't load STC wait for decoder to ask */
            break;
#endif
        case delayed_stc_eInvalid:
            BDBG_WRN(("BPCRlib_Channel_PcrUpdate: %#lx loading new STC %#x", (unsigned long)chn, (unsigned)pcr));
            rc = b_really_send_stc_isr(chn, pcr);
            chn->delayed_stc.stc = pcr;
            chn->delayed_stc.old_stc = pcr;
            chn->delayed_stc.state = delayed_stc_eLocked;
            break;
        }
        /* update delayed_stc, so following  call to b_load_delayed_stc_isr would not use stale values */
        BKNI_LeaveCriticalSection();
        return rc;
    }


    BKNI_EnterCriticalSection();
    if (chn->cfg.video && chn->video_state==BPCRlib_Decoder_State_eWaitingSTC) {
        send_video_stc = true;
    }
    if (chn->cfg.audio && chn->audio_state==BPCRlib_Decoder_State_eWaitingSTC) {
        send_audio_stc = true;
    }
    if (send_audio_stc && send_video_stc) {
        BDBG_WRN(("%#x preloading stc %#x from pcr %#x", (unsigned)chn, pcr-chn->cfg.pcr_offset, pcr));
        b_send_stc_isr(chn, pcr-chn->cfg.pcr_offset, chn->cfg.video);
        b_send_stc_isr(chn, pcr-chn->cfg.pcr_offset, chn->cfg.audio);
        if (send_audio_stc) {
            chn->audio_state=BPCRlib_Decoder_State_eNotLocked;
        }
        if (send_video_stc) {
            chn->video_state=BPCRlib_Decoder_State_eNotLocked;
        }
    }
    chn->pcr_offset.last_pcr_valid=true;
    chn->pcr_offset.last_pcr=pcr;
    stc_valid = b_read_stc_isr(chn, &stc);
#if 0
    BDBG_MSG(("(%#x) PcrUpdate %u %u %d[%u:%u]", (unsigned)chn, (unsigned)pcr, stc, delta, chn->pcr_offset.index, chn->pcr_offset.count));
#endif
    if (stc_valid) {
        delta = BPCRlib_StcDiff_isr(b_is_dss_isr(&chn->cfg), pcr, stc);
        if (delta <= chn->cfg.pcr_discard/2 && delta >= -chn->cfg.pcr_discard/2) {
            chn->pcr_offset.pcr_fifo[chn->pcr_offset.index] = delta;
            chn->pcr_offset.index++;
            if (chn->pcr_offset.index >= BPCRlib_P_PCR_FIFO) {
                chn->pcr_offset.index = 0;
            }
            if (chn->pcr_offset.count < BPCRlib_P_PCR_FIFO) {
                chn->pcr_offset.count++;
            }
            chn->pcr_offset.pcr_offset_valid = false;
        } else {
            chn->pcr_offset.pcr_offset = pcr-stc;
            chn->pcr_offset.pcr_offset_valid = true;
            BDBG_MSG(("(%#x) PcrOffset %#x %#x (%d/%d/%d)", (unsigned)chn, (unsigned)pcr, stc, (int)delta*2, (int)chn->pcr_offset.pcr_offset,(int)chn->cfg.pcr_discard));
        }
    }
    BKNI_LeaveCriticalSection();

    return rc;
}


