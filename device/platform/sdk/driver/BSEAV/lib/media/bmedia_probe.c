/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe.c $
 * $brcm_Revision: 124 $
 * $brcm_Date: 10/14/12 7:02a $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe.c $
 * 
 * 124   10/14/12 7:02a jgarrett
 * SW7425-3955: Fixing standalone unit test builds
 * 
 * 123   10/11/12 12:04p vsilyaev
 * SW7425-3955: Return proper tracks for BD audio
 * 
 * 122   6/4/12 5:52p jgarrett
 * SW7425-3065: Adding BD audio codecs to PSI parser
 * 
 * 122   6/4/12 5:51p jgarrett
 * SW7425-3065: Adding BD audio codecs to PSI parser
 * 
 * 121   5/16/12 12:15p vsilyaev
 * SW7405-4929: Properly handle shutdown for results of MPEG-2 PES probe
 * 
 * 120   5/4/12 3:54p vsilyaev
 * SW7425-2994: Updated mkv support to reject tracks with unsupported
 * compression, except the 'auxilary' tracks
 * 
 * 119   4/20/12 2:27p vsilyaev
 * SW7405-4929: Added intentification/parsing of subtitle streams
 * 
 * 118   3/16/12 4:06p vsilyaev
 * SW7231-662: Added AIFF support
 * 
 * 117   2/21/12 5:12p vsilyaev
 * SW7425-2378: Support for Monkey Audio (APE) files
 * 
 * 116   2/13/12 1:04p vsilyaev
 * SW7425-2298: Added progressive_sequence from the MPEG-2 sequence
 * extension
 * 
 * 115   1/30/12 3:08p vsilyaev
 * SW7425-2298: Added profile and level extraction for MPEG-2 ES streams
 * 
 * 114   1/20/12 1:00p vsilyaev
 * SW7425-2178: Added MJPEG video codec
 * 
 * 113   12/29/11 3:12p vsilyaev
 * SW7231-500: Merged IEEE-1394 LPCM support
 * 
 * SW7231-500/1   12/29/11 1:11p jgarrett
 * SW7231-500: Adding IEEE-1394 LPCM support
 * 
 * 112   11/11/11 5:24p vsilyaev
 * SW7425-1752: Added FLAC format support
 * 
 * 111   8/25/11 6:21p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 * 110   8/15/11 3:12p vsilyaev
 * SW7425-938: Added mapping for SH263 and VP8 video codecs
 * 
 * 109   7/13/11 6:05p vsilyaev
 * FWAVD-138: Better handle results from snprintf function
 * 
 * 108   7/6/11 6:40p vsilyaev
 * FWAVD-138: Added support for VP8 video
 * 
 * 107   7/1/11 5:43p vsilyaev
 * SWDTV-7732: Added vorbis support
 * 
 * 106   7/1/11 12:59p vsilyaev
 * SWDTV-7732: Added webm DocType
 * 
 * 105   6/20/11 6:38p vsilyaev
 * SWBLURAY-26273: Communicate MP4 DRM information to application
 * 
 * 104   5/3/11 5:02p vsilyaev
 * SWBLURAY-22800: Refactored support of H.264 in FLV
 * 
 * 103   5/3/11 11:53a vsilyaev
 * SWBLURAY-22800: Merged H.264 in FLV to mainline
 * 
 * dev_bdvd_v3.0_SWBLURAY-22800/1   4/26/11 4:46p ialauder
 * Added function to calculate the timestamp of FLV files
 * 
 * 102   4/11/11 3:12p vsilyaev
 * SWDTV-6571: Added parsing of deviceConformanceTemplate
 * 
 * 101   3/9/11 2:54p jtna
 * SW7125-618: Coverity Defect ID:25821 BAD_FREE (back-out previous fix)
 * 
 * 100   3/2/11 12:20p vsilyaev
 * SWBLURAY-24399: Merged DTS-CD support
 * 
 * fix_bdvd_v3.0_SWBLURAY-24399/2   2/18/11 3:22p ialauder
 * Fix issue with handling mismatching track types.
 * 
 * fix_bdvd_v3.0_SWBLURAY-24399/1   2/18/11 1:41p ialauder
 * Add case for DTS-CD and fix an issue if the PMT track is listed as
 * other.
 * 
 * 99   12/30/10 1:16p vsilyaev
 * SWBLURAY-23899: Merged support to control min size of parsed data.
 * 
 * fix_bdvd_v3.0_issue19100/fix_bdvd_issue20106/dev_bdvd_issue21863/dev_bdvd_issue22221/fix_bdvd_b3pvr_SWBLURAY-23899/1   12/21/10 12:08p ialauder
 * Allow the min probe amount to be configurable.
 * 
 * 98   12/14/10 1:31p gmohile
 * SW7422-134 : Add SVC/MVC probe
 * 
 * 97   12/10/10 1:53p gmohile
 * SW35230-2517 : Initial version for AVS probe
 * 
 * 96   11/11/10 12:48p jhaberf
 * SW35230-2165: fixed build warning
 * 
 * 95   11/4/10 11:58a vsilyaev
 * SW35230-2036: Clarified conditions when bmpeg2ts_psi_probe_track could
 * be used
 * 
 * 94   11/4/10 11:30a vsilyaev
 * SW35230-2096: Added extraction of the language information
 * 
 * 93   11/4/10 10:37a vsilyaev
 * SW35230-2093: Added parsing of language information
 * 
 * 92   11/2/10 12:00p vsilyaev
 * SW7342-265: Fixed typo
 * 
 * 91   10/27/10 2:35p vsilyaev
 * SW35230-1952: Added support for MS ADPCM audio
 * 
 * 90   9/28/10 11:03a vsilyaev
 * SW7405-4904: Added code to recongnize 'PR' files
 * 
 * 89   9/3/10 10:39a calvinho
 * SW7125-618: Fix Coverity Defect ID 25821 BAD_FREE, marked as
 * intentional
 * 
 * 88   8/27/10 5:28p vsilyaev
 * SWBLURAY-21085: Improved detection time duration of MPEG-2 TS  streams
 * 
 * 87   8/25/10 3:33p vsilyaev
 * SW3548-3068: Added stereoscopic fields
 * 
 * 86   7/14/10 6:10p vsilyaev
 * SW3556-1131: Added basic support for CDXA format
 * 
 * 85   6/21/10 12:50p vsilyaev
 * SW3548-2975: Added code to detect offset for the next EBML object (next
 * volume)
 * 
 * 84   4/28/10 11:49a vsilyaev
 * SWBLURAY-20104: Decreased size of the segment and increased number of
 * segments
 * 
 * 83   4/19/10 12:13p vsilyaev
 * SW3548-2890: Print DRM flag for encrypted ASF tracks
 * 
 * 82   3/19/10 5:56p vsilyaev
 * SW7405-3773: Added detection of fragmented MP4 files
 * 
 * 81   3/9/10 9:55a vsilyaev
 * SW3548-2813:Parse only header of sample_size objects
 * 
 * 80   3/3/10 11:26a vsilyaev
 * SW7405-3959: Use 32-bit field for video bitrate
 * 
 * 79   2/22/10 1:15p vsilyaev
 * SW3556-1051: Added function to initialize video track information
 * 
 * 78   2/11/10 7:25p vsilyaev
 * SW3556-1051: Added enum to specify timestamp order in the stream
 * 
 * 77   2/2/10 3:51p vsilyaev
 * SW7468-69: Added RMFF support
 * 
 * 76   1/15/10 2:21p vsilyaev
 * SW7405-3779: Return frame rate for the MPEG-2 video streams
 * 
 * 75   12/22/09 6:31p vsilyaev
 * SW7405-3661: Improved algorithm to detect stream duration
 * 
 * fix_bdvd_v3.0_issue17996/1   12/22/09 7:24p gooslin
 * Expand check for stream discontinuity. If discontinuity is detected,
 * ensure the calculated bitrate falls within an expected range before
 * using it for calculations
 * 
 * 74   12/18/09 12:58p vsilyaev
 * SW3556-946: Added fields from the ASF FileObject
 * 
 * 73   12/1/09 3:45p katrep
 * SW7405-3457:Add DRA proble support  in TS stream
 * 
 * 72   11/23/09 4:11p vsilyaev
 * SW7405-3368: Added parsing of MKV attachments
 * 
 * 71   11/5/09 2:25p gmohile
 * SW7405-3348 : Extract mkv codec id string
 * 
 * 70   10/30/09 1:48p gmohile
 * SW7405-3209 : Fix probe to return stream as self index if pcr pid and
 * video pid are different
 * 
 * 69   10/27/09 11:19a gmohile
 * SW7405-3269 : Add support for 192 byte TS packet
 * 
 * 68   10/1/09 12:08p vsilyaev
 * SW7405-3121: Merge changes for handling not interleaved AVI streams
 * 
 * fix_bdvd_v1.5_pr15387/2   10/1/09 2:08p gooslin
 * Move max_av_byte_delta to the proper probe struct
 * 
 * fix_bdvd_v1.5_pr15387/1   9/29/09 5:27p gooslin
 * Add index parse flag
 * 
 * 67   9/1/09 4:06p vsilyaev
 * SW3548-2442: Added support for AMR audio
 * 
 * 66   8/26/09 10:22a gmohile
 * SWDEPRECATED-3878 : Report index type as self index for vob/pes
 * 
 * 65   7/31/09 10:35a vsilyaev
 * PR 50725: When measured proper duration of the stream, re-evaluate
 * bitate based on the filesize and duration
 * 
 * 64   7/17/09 10:06a vsilyaev
 * PR 56734: Separated public and private interfaces on parsing mpeg audio
 * frames
 * 
 * 63   7/16/09 12:45p vsilyaev
 * PR 56734: Merged probing of VBR MP3 streams
 * 
 * 62   7/15/09 7:35p vsilyaev
 * PR 55653: Added WAV format
 * 
 * 61   7/13/09 1:42p vsilyaev
 * PR 55989: Mark proper MPEG-2 TS files as self-indexable
 * 
 * fix_bdvd_v1.5_pr15251/2   7/10/09 5:15p gooslin
 * Move mpeg frame parsing to use an independent structure
 * 
 * fix_bdvd_v1.5_pr15251/1   7/9/09 3:37p gooslin
 * Add support for getting MP3 VBR information from the Xing/VBRI header
 * 
 * 60   6/9/09 7:32p vsilyaev
 * PR 54342: Added code to probe index in the media files
 * 
 * 59   6/3/09 7:55p vsilyaev
 * PR 55417: Updated code for new semantics of the bfile_buffer_read
 * 
 * 58   6/3/09 7:35p vsilyaev
 * PR 55617: Added code to extract Pixel Aspect Ration
 * 
 * 57   3/16/09 4:35p vsilyaev
 * PR 48852: Don't duplicate streams that we weren't able to detect
 * 
 * 56   2/12/09 12:44p gmohile
 * PR 52048 : fix discontinuous timestamp calculation
 * 
 * 55   2/3/09 3:10p vsilyaev
 * PR 46190: Added PCM types in AVI probe
 * 
 * 54   1/22/09 3:51p gmohile
 * PR 51373 : Probe bitrate for mpeg video es streams
 * 
 * 53   1/20/09 4:20p gmohile
 * PR 50725 : Seperate mpeg1 pts parsing
 * 
 * 52   1/14/09 12:31p gmohile
 * PR 50725 : Add duration detection for mpeg1 streams
 * 
 * 51   1/13/09 3:45p gmohile
 * PR 50725 : Add duration probe for PES and VOB streams
 * 
 * 50   12/16/08 11:11a vsilyaev
 * PR 50148: Fixed feature leak
 * 
 * 49   12/16/08 10:28a vsilyaev
 * PR 50148: Improved parsing of H.264 streams
 * 
 * 48   12/8/08 4:13p vsilyaev
 * PR 47650: Added code to parse ID3 meta-data
 * 
 * 47   12/4/08 5:46p vsilyaev
 * PR 49899: Detect all brands of Mpeg4Part12 containers, and pass
 * compability test to application
 * 
 * 46   12/2/08 10:30a vsilyaev
 * PR 49834: Addec code to read bsid and print extra codec information
 * 
 * 45   12/1/08 5:31p vsilyaev
 * PR 48852: Don't duplicated PCR and other pids
 * 
 * 44   11/25/08 10:12a vsilyaev
 * PR 49654: Estimate bitrate based on duration and file size
 * 
 * 43   11/25/08 10:11a vsilyaev
 * PR 49654: Estimate bitrate based on duration and file size
 * 
 * 42   11/25/08 1:12a vsilyaev
 * PR 49646: Added code to parse and print H264 profile/level information
 * 
 * 41   11/20/08 3:03p vsilyaev
 * PR 49522: Fixed use of AVI types
 * 
 * 40   11/20/08 12:47p gmohile
 * PR 48852 : Return non matching ES information in TS streams as extra
 * programs
 * 
 * 39   11/20/08 12:33p vsilyaev
 * PR 49522: Added frame rate for AVI streams
 * 
 * 38   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 37   11/12/08 3:45p katrep
 * PR48182: Fixed compiler warning
 * 
 * 36   11/12/08 12:37a vsilyaev
 * PR 48182: Improved robustness of bitrate/duration detection
 * 
 * 35   11/11/08 9:49p vsilyaev
 * PR 48182: Added code to detect stream duration, using parsed timestamps
 * 
 * 34   11/11/08 6:07p vsilyaev
 * PR 47926: Handle ES files with wrong extension
 * 
 * 33   11/11/08 10:25a vsilyaev
 * PR 47650: Added bitrate/duration estimation for audio ES files
 * 
 * 32   10/28/08 9:13a vsilyaev
 * PR 45520: Fixed warnings
 * 
 * 29   10/20/08 11:05a vsilyaev
 * PR 32813: Fixed typo
 * 
 * 28   10/15/08 7:18p vsilyaev
 * PR 47926: Added probe mode to handle files with wrong extension
 * 
 * 27   10/15/08 12:31p vsilyaev
 * PR 32813: CSP188253 Added auxilary deep parsing for MPEG-2 TS streams
 * parsed as
 * 
 * 26   10/8/08 12:18p vsilyaev
 * PR 47280: Added support for another way of embedding AAC audio into MKV
 * files
 * 
 * 25   10/2/08 10:35a vsilyaev
 * PR 32813: Print number of programs and number of tracks
 * 
 * 24   9/18/08 3:27p vsilyaev
 * PR 47105: Added support for layered buffer cache
 * 
 * 23   9/17/08 3:51p vsilyaev
 * PR 46888: Added bitrate and duration detection for MPEG-2 TS streams
 * 
 * 22   7/21/08 4:32p katrep
 * PR44143: Set the LOAS as default configuration for AAC Plus audio
 * format.
 * 
 * 21   7/21/08 1:18p vsilyaev
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/1   7/18/08 4:52p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * 20   7/11/08 4:41p erickson
 * PR31188: fix for non-FLV builds
 * 
 * 19   7/11/08 4:01p vsilyaev
 * PR 31188: Added support for FLV probe
 * 
 * 18   3/4/08 12:22p vsilyaev
 * PR 39818: Added MKV stream probes
 * 
 * 17   10/16/07 10:16a jtna
 * PR35952: Coverity Defect ID:3996 OVERRUN_STATIC
 * 
 * 16   10/15/07 4:14p jtna
 * PR35952: Coverity Defect ID:3996 OVERRUN_STATIC, Intentional: add
 * Coverity comments
 * 
 * 15   7/25/07 6:28p vsilyaev
 * PR 32813: Added code to handle partially matches ES streams
 * 
 * 14   7/25/07 1:51p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 13   7/24/07 7:21p vsilyaev
 * PR 32813: Added MPEG-2 PES probe
 * 
 * 12   7/23/07 11:36a vsilyaev
 * PR 32813: Added MPEG-2 TS payload probe
 * 
 * 11   7/21/07 9:53p vsilyaev
 * PR 32813: Added MPEG-1 System probe
 * 
 * 10   7/20/07 11:57p vsilyaev
 * PR 32813: Added MPEG video probe
 * 
 * 9   7/20/07 4:35p vsilyaev
 * PR 32813: Set track id for elementary streams
 * 
 * 8   7/20/07 1:36p vsilyaev
 * PR 32813: Added support for ES files
 * 
 * 7   7/19/07 12:17p vsilyaev
 * PR 32813: Renamed bmedia_track to bmedia_track_type
 * 
 * 6   7/17/07 4:31p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 5   7/16/07 8:32a erickson
 * PR32813: Fixed warning
 * 
 * 4   7/10/07 5:54p vsilyaev
 * PR 32813: Fixed warnings
 * 
 * 3   7/10/07 5:22p vsilyaev
 * PR 32813: Added stream duration field
 * 
 * 2   7/10/07 12:42p erickson
 * PR32813: fix issues while integrating with Brutus
 * 
 * 1   7/9/07 4:05p vsilyaev
 * PR 32813: Stream format probe module
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bmedia_probe_impl.h"
#include "bkni.h"
#include "balloc.h"
#if B_HAS_ASF
#include "basf_probe.h"
#endif
#if B_HAS_AVI
#include "bavi_probe.h"
#endif
#if B_HAS_FLV
#include "bflv_probe.h"
#endif
#if B_HAS_RMFF
#include "brmff_probe.h"
#endif
#include "bmkv_probe.h"
#include "bmp4_probe.h"
#include "bmp3_probe.h"
#include "bwav_probe.h"
#include "bogg_probe.h"
#include "bflac_probe.h"
#include "bmedia_cdxa.h"
#include "bmpeg2ts_psi_probe.h"
#include "bmpeg2ts_probe.h"
#include "bmpeg2pes_probe.h"
#include "bmedia_probe_es.h"
#include "bmpeg1_probe.h"
#include "bmpeg_audio_util.h"
#include "bavc_video_probe.h"
#include "bavs_video_probe.h"
#include "bmpeg_video_probe.h"
#include "bac3_probe.h"
#include "bape_probe.h"
#include "baiff_probe.h"
#include "bmedia_util.h"

BDBG_MODULE(bmedia_probe);
#define BDBG_MSG_TRACE(x)   /*BDBG_MSG(x)*/

BDBG_OBJECT_ID(bmedia_probe_t);


static const bmedia_probe_format_desc *b_media_probe_formats[] = {
#if B_HAS_ASF
    &basf_probe,
#endif
#if B_HAS_AVI
    &bavi_probe,
#endif
#if B_HAS_FLV
    &bflv_probe,
#endif
#if B_HAS_RMFF
    &brmff_probe,
#endif
    &baiff_probe,
    &bape_probe,
    &bmkv_probe,
    &bmp4_probe,
    &bmp3_probe,
    &bwav_probe,
    &bogg_probe,
    &bflac_probe,
    &bcdxa_probe,
    &bmpeg2ts_psi_probe, 
    &bmpeg2ts_probe,
    &bmpeg1_probe,
    &bmpeg2pes_probe,
    &bmpeg2ts192_psi_probe, 
    &bmpeg2ts192_probe
};

#define B_MEDIA_N_PROBES ((sizeof(b_media_probe_formats)/sizeof(*b_media_probe_formats)))

#define B_MEDIA_PROBE_N_SEGS    128
#define B_MEDIA_PROBE_MAX_EXPECTED_BITRATE (5000*1024*8) /* 2000 Kbps */
#define B_MEDIA_PROBE_MIN_EXPECTED_BITRATE (30*1024*8) /* 50 Kbps */
#define B_MEDIA_PROBE_N_DURATION_SEGMENTS (10)
#define B_MEDIA_PROBE_N_BITRATE_RETRIES (6)
#define B_MEDIA_PROBE_MIN_SEGMENT_LENGTH (100000)
#define B_MEDIA_PROBE_MIN_BITRATE_THRESOLD (0.95)
#define B_MEDIA_PROBE_MAX_BITRATE_THRESOLD (1.05)

struct bmedia_probe {
    BDBG_OBJECT(bmedia_probe_t)
    batom_factory_t factory;
    batom_pipe_t pipe;
    bmedia_probe_es_t es_probe;
    bmedia_probe_base_t probes[B_MEDIA_N_PROBES];
};

bmedia_probe_t 
bmedia_probe_create(void)
{
    bmedia_probe_t  probe;
    unsigned i;

    probe = BKNI_Malloc(sizeof(*probe));
    if(!probe) {
        BDBG_ERR(("bmedia_probe_create: can't allocate %u bytes", sizeof(*probe)));
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(probe, bmedia_probe_t);
    probe->factory = batom_factory_create(bkni_alloc, 16);
    if(!probe->factory) {
        goto err_factory;
    }
    probe->pipe = batom_pipe_create(probe->factory);
    if(!probe->pipe) {
        goto err_pipe;
    }
    probe->es_probe = NULL;
    /* clear all probes */
    for(i=0;i<B_MEDIA_N_PROBES;i++) {
        probe->probes[i]=NULL;
    }
    BDBG_MSG_TRACE(("bmedia_probe_create<: %#lx", (unsigned long)probe));
    return probe;
err_pipe:
    batom_factory_destroy(probe->factory);
err_factory:
    BKNI_Free(probe);
err_alloc:
    return NULL;
}

void 
bmedia_probe_destroy(bmedia_probe_t probe)
{
    unsigned i;
    BDBG_OBJECT_ASSERT(probe, bmedia_probe_t);
    for(i=0;i<B_MEDIA_N_PROBES;i++) {
        if(probe->probes[i]) {
            b_media_probe_formats[i]->destroy(probe->probes[i]);
        }
    }
    if(probe->es_probe) {
        bmedia_probe_es_destroy(probe->es_probe);
    }
    batom_pipe_destroy(probe->pipe);
    batom_factory_destroy(probe->factory);
    BDBG_OBJECT_DESTROY(probe, bmedia_probe_t);
    BKNI_Free(probe);
    return;
}


static size_t 
b_strlen(const char *s)
{
    size_t off;
    for(off=0;s[off]!='\0';off++) { }
    return off;
}

static const char *
b_strrchr(const char *s, int c)
{
    size_t off;

    for(off=b_strlen(s);off>0;off--) {
        if(s[off]==c) {
            return s+off;
        }
    }
    if(s[0]==c) {
        return s;
    }
    return NULL;
}

static int 
b_strcasecmp(const char *s1, const char *s2)
{
    size_t off;
    for(off=0;;off++) {
        char c1 = s1[off];
        char c2 = s2[off];
        int diff;
        if(c1==c2) {
            if(c1==0) {
                return 0;
            }
            continue;
        }
        if(c1 >= 'a' && c1 <= 'z') {
            c1 &= ~0x20;
        }
        if(c2 >= 'a' && c2 <= 'z') {
            c2 &= ~0x20;
        }
        diff = c1 - c2;
        if(diff!=0) {
            return diff;
        }
    }
}

bool 
bmedia_probe_match_ext(const bmedia_probe_file_ext *ext_list, const char *ext)
{
    unsigned i;
    if(ext_list == NULL || ext == NULL || ext_list[0][0]=='\0' || ext[0]=='\0') {
        return true;
    }
    for(i=0;ext_list[i][0]!='\0';i++) {
        if(b_strcasecmp(ext_list[i], ext)==0) {
            BDBG_MSG(("bmedia_probe_match_ext: ext:%s matched:%s", ext, ext_list[i]));
            return true;
        }
    }
    return false;
}


static unsigned 
b_media_probe_filter_ext(bool *valid_probes, const char *ext)
{
    unsigned i,nvalid;
    BDBG_MSG_TRACE(("b_media_probe_filter_ext: ext:%s", ext?ext:"NONE"));
    for(nvalid=0,i=0;i<B_MEDIA_N_PROBES;i++) {
        if(!valid_probes[i]) {
            continue;
        }
        valid_probes[i] = bmedia_probe_match_ext(b_media_probe_formats[i]->ext_list, ext);
        if(valid_probes[i]) {
            nvalid++;
        }
    }
    BDBG_MSG(("b_media_probe_filter_ext: nvalid:%u", nvalid));
    return nvalid;
}

static unsigned
b_media_probe_filter_header(bmedia_probe_t probe, bool *valid_probes, bfile_buffer_t buf, const bmedia_probe_parser_config *config)
{
    size_t probe_size;
    unsigned i,nvalid=0;
    batom_t atom;
    batom_cursor cursor;
    bfile_buffer_result result;

    BSTD_UNUSED(probe);

    for(i=0,probe_size=4; i<B_MEDIA_N_PROBES; i++) {
        if( valid_probes[i] && probe_size<b_media_probe_formats[i]->header_size ) {
            probe_size=b_media_probe_formats[i]->header_size;
        }
    }
    atom = bfile_buffer_read(buf, config->parse_offset+0, probe_size, &result);
    if(atom==NULL) {
        goto err_probe;
    }
    for(i=0; i<B_MEDIA_N_PROBES; i++) {
        if(valid_probes[i]) {
            batom_cursor_from_atom(&cursor, atom);
            if(b_media_probe_formats[i]->header_match(&cursor)) {
                nvalid++;
                continue;
            }
            valid_probes[i] = false;
        }
    }
    batom_release(atom);
done:
    BDBG_MSG(("b_media_probe_filter_header: nvalid:%u", nvalid));
    return nvalid;

err_probe:
    for(i=0; i<B_MEDIA_N_PROBES; i++) {
        valid_probes[i]=false;
    }
    goto done;
}

static unsigned
b_media_probe_filter_type(bool *valid_probes, bstream_mpeg_type type)
{
    unsigned i;
    unsigned nvalid;
    for(nvalid=0,i=0; i<B_MEDIA_N_PROBES; i++) {
        if(valid_probes[i]) {
            if(b_media_probe_formats[i]->type == type) {
                nvalid++;
                continue;
            }
            valid_probes[i] = false;
        }
    }
    BDBG_MSG(("b_media_probe_filter_type: nvalid:%u", nvalid));
    return nvalid;
}

static int b_media_strlen(const char *s) {
    const char *t=s;
	while (*t) { t++; }
	return t-s;
}

typedef struct b_snprintf_buf {
    size_t left;
    size_t printed;
    char *buf;
} b_snprintf_buf;


static void 
b_snprintf_buf_advance(b_snprintf_buf *b, int rc)
{
    BDBG_ASSERT(b->left>=1);
    if(rc>0) {
        b->printed += rc;
        if((unsigned)rc < b->left) {
            b->left -= rc;
            b->buf += rc;
        } else {
            b->buf += b->left-1;
            b->left = 1;
        }
    }
}

int
bmedia_stream_to_string(const bmedia_probe_stream *stream, char *dest_buf, size_t size)
{
    const char *format;
    const bmedia_probe_track *track;
    unsigned last_program;
    unsigned hours;
    unsigned mins;
    unsigned secs;
    char format_extra[128];
    char fps[16];
    const char *index;
    b_snprintf_buf buf;

    BDBG_ASSERT(dest_buf);
    if(size<1) {
        return -1;
    }

    buf.left = size;
    buf.buf = dest_buf;
    buf.printed = 0;
    if(!stream) {
        return BKNI_Snprintf(dest_buf, size, "unknown");
    }
    fps[0]='\0';
    switch(stream->type) {
    default:
    case bstream_mpeg_type_unknown: format="unknown"; break;
    case bstream_mpeg_type_es: 
        if(stream->probe_id!=BMP3_PROBE_ID) {
            format="ES";
        } else {
            const bmp3_probe_stream *mp3 = (bmp3_probe_stream *)stream;
            const char *id3;
            switch(mp3->id3) {
            default:
            case bmp3_id3_none: id3=""; break;
            case bmp3_id3v1: id3="-ID3V1"; break;
            case bmp3_id3v2_2: id3="-ID3V2_2"; break;
            case bmp3_id3v2_3: id3="-ID3V2_3"; break;
            case bmp3_id3v2_4: id3="-ID3V2_4"; break;
            }
            BKNI_Snprintf(format_extra, sizeof(format_extra), "MP3%s{%s%s%s%s%s}", id3, mp3->title.str, mp3->title.len>0?" ; ":"", mp3->artist.str, mp3->artist.len>0?" ; ":"", mp3->album.str);
            format = format_extra;
        }
        break;
    case bstream_mpeg_type_bes: format="BES";break;
    case bstream_mpeg_type_pes: format="PES";break;
    case bstream_mpeg_type_ts: format="MPEG2-TS";break;
    case bstream_mpeg_type_dss_es: format="DSS-ES";break;
    case bstream_mpeg_type_dss_pes: format="DSS-PES";break;
    case bstream_mpeg_type_vob: format="DVD VOB";break;
#if B_HAS_ASF
    case bstream_mpeg_type_asf:  
        {
            const basf_probe_stream *asf= (basf_probe_stream *)stream;
            char deviceConformanceTemplate[BASF_DEVICE_CONFORMANCE_TEMPLATE_BUFFER+1];
            unsigned i;
            unsigned j;
            *deviceConformanceTemplate = '\0';
            for(j=i=0;i<asf->deviceConformanceTemplateCount;i++) {
                for(;j<BASF_DEVICE_CONFORMANCE_TEMPLATE_BUFFER;j++) {
                    deviceConformanceTemplate[j] = (char )asf->deviceConformanceTemplates[j];
                    if(deviceConformanceTemplate[j]==0) {
                        break;
                    }
                }
                deviceConformanceTemplate[j] = ' ';
                deviceConformanceTemplate[j+1] = '\0';
                j++;
            }
            BKNI_Snprintf(format_extra, sizeof(format_extra), "ASF{%s%s%s%s%s%s}", asf->seekableFlag?"seekable ":"", asf->broadcastFlag?"broadcast ":"", asf->liveFlag?"live":"", asf->stereoscopic_info.stereoscopic?"3D ":"", asf->encryption_type!=basf_encryption_none?"DRM ":"", deviceConformanceTemplate);
            format=format_extra;
            break;
        }
#endif
#if B_HAS_AVI
    case bstream_mpeg_type_avi: 
        format="AVI";
        BKNI_Snprintf(fps, sizeof(fps), " %u.%03uFPS ", ((bavi_probe_stream *)stream)->video_framerate/1000, ((bavi_probe_stream *)stream)->video_framerate%1000);
        break;
#endif
    case bstream_mpeg_type_mpeg1: format="MPEG1";break;
    case bstream_mpeg_type_cdxa: format="CDXA";break;
    case bstream_mpeg_type_mp4: 
        {
            const bmp4_probe_stream *mp4_stream = (bmp4_probe_stream *)stream;
            BKNI_Snprintf(format_extra, sizeof(format_extra), "MP4 [%s%s%s]", mp4_stream->ftyp, mp4_stream->compatible?"":",not compatible", mp4_stream->fragmented?",fragmented":"");
            format=format_extra;
        }
        break;
    case bstream_mpeg_type_mkv: 
        {
            const bmkv_probe_stream *mkv_stream = (bmkv_probe_stream *)stream;
            BKNI_Snprintf(format_extra, sizeof(format_extra), "MKV [%s]", mkv_stream->docType, mkv_stream->next_volume.next_volume_offset_valid?":multivolume":"");
            format=format_extra;
        }
        break;
    case bstream_mpeg_type_flv: format="FLV";break;
    case bstream_mpeg_type_wav: format="WAV";break;
    case bstream_mpeg_type_ogg: format="OGG";break;
    case bstream_mpeg_type_flac: format="FLAC";break;
    case bstream_mpeg_type_rmff: format="RMFF";break;
    case bstream_mpeg_type_ape: format="APE";break;
    case bstream_mpeg_type_aiff: format="AIFF";break;
    }
    switch(stream->index) {
    default:
    case bmedia_probe_index_unknown: index = ""; break;
    case bmedia_probe_index_required: 
    case bmedia_probe_index_available: index= "INDEX:"; break;
    case bmedia_probe_index_missing: index= "NO-INDEX:"; break;
    case bmedia_probe_index_self: index="SELF-INDEX:"; break; 
    }
    secs = stream->duration/1000;
    mins = secs/60;
    hours = mins/60;
    mins = mins%60;
    secs = secs%60;
    b_snprintf_buf_advance(&buf, BKNI_Snprintf(buf.buf, buf.left, "%s[%s%u:%u] %u:%02u.%02u %s%uKbps [ ", format, index, stream->ntracks, stream->nprograms, hours, mins, secs, fps, stream->max_bitrate/1000));
    for(last_program=0,track=BLST_SQ_FIRST(&stream->tracks); track; track=BLST_SQ_NEXT(track,link)) {
        const char *language, *codec_id;
#if B_HAS_ASF
        char ascii_language[6];
#endif
        char codec_specific[48];
        size_t codec_specific_offset = 0;

        codec_specific[0]='\0';
        codec_specific[1]='\0';
        if(last_program!=track->program && track!=BLST_SQ_FIRST(&stream->tracks)) {
            b_snprintf_buf_advance(&buf,BKNI_Snprintf(buf.buf, buf.left, "] [ ", format, stream->duration/1000, stream->max_bitrate/1024));
        }
        last_program=track->program;
        switch(track->type) {
        case bmedia_track_type_audio:
            switch(track->info.audio.codec) {
            default:
            case baudio_format_unknown: format="unknown";break;
            case baudio_format_mpeg: format="MPEG1/2 Layer1/2";break;
            case baudio_format_mp3: format="MPEG1/2 Layer3";break;
            case baudio_format_aac: format="AAC";break;
            case baudio_format_aac_plus: format="AAC+";break;
            /* case baudio_format_aac_plus_loas: format="AAC+ LOAS";break; */
            case baudio_format_aac_plus_adts: format="AAC+ ADTS";break; 
            case baudio_format_ac3: format="AC3"; break;
            case baudio_format_ac3_plus: format="AC3+";break;
            case baudio_format_dts: format="DTS";break;
            case baudio_format_lpcm_hddvd: format="LPCM HD-DVD";break;
            case baudio_format_lpcm_bluray: format="LPCM BluRay";break;
            case baudio_format_dts_hd: format="DTS HD";break;
            case baudio_format_wma_std: format="WMA STD";break;
            case baudio_format_wma_pro: format="WMA PRO";break;
            case baudio_format_lpcm_dvd: format="LPCM DVD";break;
            case baudio_format_pcm: format="PCM";break;
            case baudio_format_amr: format="AMR";break;
            case baudio_format_vorbis: format="VORBIS";break;
            case baudio_format_flac: format="FLAC";break;
            case baudio_format_dra: format="DRA";break;
            case baudio_format_cook: format="COOK";break;
            case baudio_format_g726: format="G726";break;
            case baudio_format_adpcm: format="ADPCM";break;
            case baudio_format_dvi_adpcm: format="DVI ADPCM";break;
            case baudio_format_dts_cd: format="DTS-CD";break;
            case baudio_format_lpcm_1394: format="LPCM-1394";break;
            case baudio_format_ape: format="APE";break;
            case baudio_format_mlp: format="MLP";break;
            }

            switch(track->info.audio.codec) {
            default:
                break;
            case baudio_format_ac3: 
            case baudio_format_ac3_plus: 
                if(((bmedia_probe_ac3_audio*)&track->info.audio.codec_specific)->valid) {
                    codec_specific[0]=' ';
                switch(((bmedia_probe_ac3_audio*)&track->info.audio.codec_specific)->codec) {
                    case baudio_format_ac3: 
                        BKNI_Snprintf(codec_specific+1, sizeof(codec_specific)-1,"AC3");
                        break;
                    case baudio_format_ac3_plus: 
                        BKNI_Snprintf(codec_specific+1, sizeof(codec_specific)-1,"AC3+");
                        break;
                    default:
                        BKNI_Snprintf(codec_specific+1, sizeof(codec_specific)-1,"Unknown");
                        break;
                    }
                }
                break;
            }

            b_snprintf_buf_advance(&buf,BKNI_Snprintf(buf.buf, buf.left,"0x%02x:audio(%s %u:%u:%u:%u%s) ", track->number, format, (unsigned)track->info.audio.sample_rate, (unsigned)track->info.audio.channel_count, (unsigned)track->info.audio.sample_size, (unsigned)track->info.audio.bitrate, codec_specific));
            break;
        case bmedia_track_type_video:
            switch(track->info.video.codec) {
            default:
            case bvideo_codec_unknown: format="unknown";break;
            case bvideo_codec_mpeg1: format="MPEG1";break;
            case bvideo_codec_mpeg2: format="MPEG2";break;
            case bvideo_codec_mpeg4_part2: format="MPEG4 Part2";break;
            case bvideo_codec_h263: format="H.263";break;
            case bvideo_codec_h264: format="H.264"; break;
            case bvideo_codec_h264_svc: format="H.264 SVC"; break;
            case bvideo_codec_h264_mvc: format="H.264 MVC"; break;
            case bvideo_codec_vc1: format="VC-1 AP";break;
            case bvideo_codec_vc1_sm: format="VC-1 SP/MP";break;
            case bvideo_codec_divx_311: format="DivX 3.11";break;
            case bvideo_codec_avs: format="AVS";break;
            case bvideo_codec_vp6: format="VP6";break;
            case bvideo_codec_spark: format="Spark/H.263";break;
            case bvideo_codec_rv40: format="RV40";break;
            case bvideo_codec_vp8: format="VP8";break;
            case bvideo_codec_mjpeg: format="MJPEG";break;
            }

            switch(track->info.video.codec) {
            default: break;
            case bvideo_codec_h264:
            case bvideo_codec_h264_svc:
            case bvideo_codec_h264_mvc:
                if(((bmedia_probe_h264_video *)&track->info.video.codec_specific)->sps.valid) {
                    codec_specific[0]=' ';
                    b_h264_video_sps_to_string(&((bmedia_probe_h264_video *)&track->info.video.codec_specific)->sps, codec_specific+1, sizeof(codec_specific)-1);
                }
                break;
            case bvideo_codec_mpeg1:
            case bvideo_codec_mpeg2:
                {
                    const bmedia_probe_mpeg_video *mpeg_video = (bmedia_probe_mpeg_video*)&track->info.video.codec_specific;
                    const char *profile;
                    const char *level;
                    switch(mpeg_video->profile) {
                    case 0x5: profile = "Simple";break;
                    case 0x4: profile = "Main";break;
                    case 0x3: profile = "SNR Scalable";break;
                    case 0x2: profile = "Spatially Scalable";break;
                    case 0x1: profile = "High";break;
                    default: profile = "";break;
                    }
                    switch(mpeg_video->level) {
                    case 0x0A: level = "Low";break;
                    case 0x08: level = "Main";break;
                    case 0x06: level = "High 1440";break;
                    case 0x04: level = "High";break;
                    default: level = "";break;
                    }
                    BKNI_Snprintf(codec_specific, sizeof(codec_specific), " %u.%03uFPS(%s,%s%s)", mpeg_video->framerate/1000, mpeg_video->framerate, profile, level, mpeg_video->progressive_sequence?",progressive":"");
                }
                break;
            case bvideo_codec_avs:
                BKNI_Snprintf(codec_specific, sizeof(codec_specific), " %u.%03uFPS", ((bmedia_probe_avs_video*)&track->info.video.codec_specific)->framerate/1000, ((bmedia_probe_avs_video*)&track->info.video.codec_specific)->framerate%1000);
                break;
            }
            codec_specific_offset = b_media_strlen(codec_specific);
            switch(stream->type) {
#if B_HAS_ASF
            case bstream_mpeg_type_asf:
                {
                    const basf_probe_track *asf_track = (basf_probe_track *)track;
                    if(asf_track->aspectRatioValid && track->info.video.height) {
                        double ar = (asf_track->aspectRatio.x * (float)track->info.video.width) / (asf_track->aspectRatio.y * (float)track->info.video.height);
                        const char *sar;
                        switch((int)(ar*100)) {
                        case 177: sar = "16:9"; break;
                        case 133: sar = "4:3"; break;
                        case 150: sar = "3:2"; break;
                        case 160: sar = "16:10"; break;
                        default: sar = NULL; break;
                        }
                        if(sar) {
                            BKNI_Snprintf(codec_specific+codec_specific_offset, sizeof(codec_specific)-codec_specific_offset, " %s",  sar);
                        } else {
                            BKNI_Snprintf(codec_specific+codec_specific_offset, sizeof(codec_specific)-codec_specific_offset, " (AR %.2f)", ar);
                        }
                        codec_specific_offset = b_media_strlen(codec_specific);
                    }
                    if(asf_track->encryptedContentFlag) {
                        BKNI_Snprintf(codec_specific+codec_specific_offset, sizeof(codec_specific)-codec_specific_offset, " DRM");
                        codec_specific_offset = b_media_strlen(codec_specific);
                    }
                }
                break;
#endif
#if B_HAS_RMFF
            case bstream_mpeg_type_rmff:
                BKNI_Snprintf(codec_specific+codec_specific_offset, sizeof(codec_specific)-codec_specific_offset, " %u.%03uFPS ", ((brmff_probe_track*)track)->info.video.framerate/1000, ((brmff_probe_track*)track)->info.video.framerate%1000);
                break;
#endif
            case bstream_mpeg_type_mp4:
                {
                    const bmp4_probe_track *mp4_track = (bmp4_probe_track *)track;
                    if(mp4_track->duration) {
                        unsigned framerate = ((1000*1000)*(uint64_t)mp4_track->sample_count)/mp4_track->duration;
                        BKNI_Snprintf(codec_specific+codec_specific_offset, sizeof(codec_specific)-codec_specific_offset, " %u.%03uFPS %s", framerate/1000, framerate%1000, mp4_track->encrypted?"DRM":"");
                    }
                }
            default:
                break;
            }

            b_snprintf_buf_advance(&buf, BKNI_Snprintf(buf.buf, buf.left, "0x%02x:video(%s%s %uKbps %ux%u%s) ", track->number, format, codec_specific, track->info.video.bitrate, track->info.video.width, track->info.video.height, track->info.video.timestamp_order==bmedia_timestamp_order_decode?" DTS":""));
            break;
        case bmedia_track_type_pcr:
            b_snprintf_buf_advance(&buf,BKNI_Snprintf(buf.buf, buf.left,"0x%02x:PCR ", track->number));
            break;
        case bmedia_track_type_other:
            b_snprintf_buf_advance(&buf, BKNI_Snprintf(buf.buf, buf.left, "0x%02x:unknown ", track->number));
            break;
        }

        switch(stream->type) {
        case bstream_mpeg_type_mkv: 
            {
                const bmkv_probe_track *mkv_track = (bmkv_probe_track*)track;
                language = mkv_track->language;
                codec_id = mkv_track->codec_id;
                if(mkv_track->unsupported) {
                    b_snprintf_buf_advance(&buf, BKNI_Snprintf(buf.buf, buf.left, "UNSUPPORTED ", language));
                }
            }
            break;
        case bstream_mpeg_type_mp4: 
            language = ((bmp4_probe_track*)track)->language;
            codec_id = NULL;
            break;
#if B_HAS_AVI
        case bstream_mpeg_type_avi: 
            language = ((bavi_probe_track*)track)->language;
            codec_id = NULL;
            break;
#endif
#if B_HAS_ASF
        case bstream_mpeg_type_asf: 
            {
                uint16_t *asf_language = ((basf_probe_track*)track)->language;
                codec_id = NULL;
                language = NULL;
                if(asf_language) {
                    unsigned i;
                    BKNI_Memset(ascii_language, 0, sizeof(ascii_language));
                    for(i=0;i<sizeof(ascii_language)-1;i++) {
                        ascii_language[i] = (unsigned char)asf_language[i];
                        if(asf_language[i]==0) {
                            break;
                        }
                    }
                    language = ascii_language;
                }
            }
            break;
#endif
        default:
            language = NULL;
            codec_id = NULL;
            break;
        }
        if(language && *language!='\0') {
            b_snprintf_buf_advance(&buf, BKNI_Snprintf(buf.buf, buf.left, "<%s> ", language));
        }
        if(codec_id && *codec_id!='\0') {
            b_snprintf_buf_advance(&buf, BKNI_Snprintf(buf.buf, buf.left, "<%s> ", codec_id));
        }
    }

    b_snprintf_buf_advance(&buf, BKNI_Snprintf(buf.buf, buf.left, "]", format));
    return buf.printed;
}

static const bmedia_probe_stream *
b_media_probe_feed_es(bmedia_probe_t probe, const char *ext, bfile_buffer_t buf)
{
    off_t off;
    bmedia_probe_track *track = NULL;
    bmedia_probe_stream *stream = NULL;

    if(!probe->es_probe) {
        probe->es_probe = bmedia_probe_es_create(probe->factory);
        if(!probe->es_probe) {
            return NULL;
        }
    }
    bmedia_probe_es_filter_ext(probe->es_probe, ext);
    for(off=0;;) {
        bfile_buffer_result result;
        unsigned nactive;
        const size_t read_len = BMEDIA_PROBE_FEED_SIZE;
        batom_t atom = bfile_buffer_read(buf, off, read_len, &result);
        BDBG_MSG(("b_media_probe_feed_es: %#lx off:%u read_len:%u result:%u atom:%#x(%u)", (unsigned long)probe, (unsigned)off, read_len, result, atom, atom?batom_len(atom):0));
        if(atom==NULL) {
            track = bmedia_probe_es_last(probe->es_probe);
            break;
        } 
        BDBG_ASSERT(atom);
        off+=batom_len(atom);
        track = bmedia_probe_es_feed(probe->es_probe, atom, &nactive);
        if(track || nactive==0) {
            break;
        }
    }
    bmedia_probe_es_reset(probe->es_probe);
    if(track) {
        stream = BKNI_Malloc(sizeof(*stream));
        if(stream) {
            track->number = 1; /* assign track number */ 
            stream->probe_id = B_MEDIA_N_PROBES;
            bmedia_probe_stream_init(stream, bstream_mpeg_type_es);
            if(track->type == bmedia_track_type_audio) {
                if(stream->max_bitrate==0) {
                    stream->max_bitrate = track->info.audio.bitrate*1000;
                }
            } else if (track->type == bmedia_track_type_video) {
                stream->max_bitrate = track->info.video.bitrate*1000;
            }
            bmedia_probe_add_track(stream, track);
        }
    }
    return stream;
}

static void
b_media_probe_parse_aux(bmedia_probe_t probe, bmedia_probe_stream *stream, const bmedia_probe_format_desc *media_probe, bfile_buffer_t buf, bmedia_probe_parser_config  *parser_config)
{
	bmedia_probe_stream *stream_aux;
	unsigned i;

	for(i=0;i<B_MEDIA_N_PROBES ;i++) {
		if(b_media_probe_formats[i] == media_probe) {
			bmedia_probe_track *track_aux, *track;
			struct bmedia_probe_track_list extra_tracks;	

			BLST_SQ_INIT(&extra_tracks);

			if(!probe->probes[i]) {
				probe->probes[i]= b_media_probe_formats[i]->create(probe->factory);
				if(!probe->probes[i]) {
					BDBG_WRN(("b_media_probe_parse_aux: %#lx can't create probe for stream type %u", (unsigned long)probe, b_media_probe_formats[i]->type));
					break;
				}
			}
			stream_aux = (bmedia_probe_stream *)media_probe->parse(probe->probes[i], buf, probe->pipe, parser_config);
			if(stream_aux && stream_aux->type == stream->type) {
				if(stream->duration==0) {
					stream->duration = stream_aux->duration;
				}
				if(stream->max_bitrate==0) {
					stream->max_bitrate = stream_aux->max_bitrate;
				}
				
				while(NULL!=(track_aux=BLST_SQ_FIRST(&stream_aux->tracks))) {
					bool match_found=false;
					
					BLST_SQ_REMOVE_HEAD(&stream_aux->tracks, link);				

					for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
                        bool matched = track->number == track_aux->number;
                        bool mpeg2ts_matched = stream->type == bstream_mpeg_type_ts && B_MEDIA_MPEG2TS_TRACKNO_PID(track->number) == B_MEDIA_MPEG2TS_TRACKNO_PID(track_aux->number);
						if(matched || mpeg2ts_matched) {
                            /* We have seen sometimes that the PMT returns the type as bmedia_track_type_other for some streams.
                                Since the PIDS match we should be able to use the track_aux info */
							if(track->type == track_aux->type || track->type == bmedia_track_type_other) {
								bool found_info = false;
                                track->type = track_aux->type;
								switch(track->type) {
								case bmedia_track_type_video:
								  	if(track->info.video.codec != track_aux->info.video.codec) {
                                        BDBG_WRN(("b_media_probe_parse_aux: video codecs did not match use track_aux (%d:%d)",track->info.video.codec, track_aux->info.video.codec));
                                    }                                    
                                    track->info.video = track_aux->info.video;
                                    match_found=true;
                                    found_info=true;								  	
								  	break;
								case bmedia_track_type_audio:
                                    if(!matched && mpeg2ts_matched) {
                                        if(track->info.audio.codec != track_aux->info.audio.codec) {
                                            break;
                                        }
                                    }
                                    if(!(track->info.audio.codec == baudio_format_ac3_plus && track_aux->info.audio.codec == baudio_format_ac3)) {
                                        if(matched) {
                                            if(track->info.audio.codec != track_aux->info.audio.codec) {
                                                BDBG_WRN(("b_media_probe_parse_aux: audio codecs did not match use track_aux (%d:%d)",track->info.audio.codec, track_aux->info.audio.codec));
                                            }
                                            track->info.audio = track_aux->info.audio;
                                        }
                                    }
                                    match_found=true;
                                    found_info=true;								  	
								  	break;
								case bmedia_track_type_pcr:
									/* fall through */
								default: 
								  	match_found=true;
								  	break;
								}
								if(found_info) {
									break;
								}
							} else if (track_aux->type == bmedia_track_type_other) { /* other track matches any type */
                                match_found=true;
                            }
						} 
					}
					
					if(!match_found){
						BLST_SQ_INSERT_HEAD(&extra_tracks, track_aux, link);
					} else {
						BKNI_Free(track_aux);
					}
				}
				
				/* copy extra tracks */
				while(NULL!=(track_aux=BLST_SQ_FIRST(&extra_tracks))) {
					BLST_SQ_REMOVE_HEAD(&extra_tracks,link);  
					track_aux->program = BMEDIA_PROBE_INVALID_PROGRAM;
					BLST_SQ_INSERT_TAIL(&stream->tracks, track_aux, link);
				}

				b_media_probe_formats[i]->stream_free(probe->probes[i], (bmedia_probe_stream *)stream_aux);
			}
			break;
		}
	}
	return;
}

#define B_MEDIA_PROBE_TIMESTAMP_STEP    (4*1024*1024)

static int
b_media_probe_get_timestamp(bmedia_probe_t probe, bfile_buffer_t buf, bmedia_timestamp_parser_t timestamp_parser, off_t offset, bmedia_timestamp *timestamp)
{
    bfile_buffer_result result;
    const size_t read_len = BMEDIA_PROBE_FEED_SIZE;
    int rc=-1;
    size_t parsed;
    batom_t atom;

    BSTD_UNUSED(probe);

    timestamp_parser->methods->seek(timestamp_parser, offset);
    timestamp->offset = 0;
    timestamp->timestamp = 0;
    for(parsed=0;parsed<B_MEDIA_PROBE_TIMESTAMP_STEP;) {
        batom_cursor cursor;
        atom = bfile_buffer_read(buf, offset+parsed, read_len, &result);
        BDBG_MSG(("b_media_probe_get_timestamp: %#lx off:%u(%u) read_len:%u result:%u atom:%#x(%u)", (unsigned long)probe, (unsigned)(offset+parsed), parsed, read_len, result, atom, atom?batom_len(atom):0));
        if(!atom) {
            break;
        }
        parsed += batom_len(atom);
        batom_cursor_from_atom(&cursor, atom);
        rc = timestamp_parser->methods->parse(timestamp_parser, &cursor, timestamp);
        BDBG_MSG(("b_media_probe_get_timestamp: %#lx rc=%d offset:%u timestamp:%u", (unsigned long)probe, rc, (unsigned)timestamp->offset, (unsigned)timestamp->timestamp));
        batom_release(atom);
        if(rc!=0) {
            if(rc==1) {
                rc=0; /* succcess */
            }
            break;
        }
        rc=-1;
    }
    return rc;
}
/* Take n samples (B_MEDIA_PROBE_N_DURATION_SEGMENTS) from the segment and average out the average bitrate */
static unsigned
b_media_probe_calculate_segment_bitrate(bmedia_probe_t probe, bfile_buffer_t buf, bmedia_timestamp_parser_t timestamp_parser, const bmedia_probe_config *config, off_t off_first, off_t off_last)
{       
    off_t skip;
    off_t off = off_first;
    bmedia_timestamp first, last;
    unsigned bitrate = 0;
    int count = 0;
    int i, rc;
    unsigned sum_bitrate = 0;

    if (off_first >= off_last) {
        return 0;
    }
    
    skip = (off_last - off_first) / (config->duration_config.max_bitrate_n_segments + 1);
   
    rc = b_media_probe_get_timestamp(probe, buf, timestamp_parser, off, &first);
    if(rc == 0 ) { 

        off += skip;
        for(i = 0; (i < config->duration_config.max_bitrate_n_segments) && (off < off_last); i++)
        {           

            rc = b_media_probe_get_timestamp(probe, buf, timestamp_parser, off, &last);
            if(rc!=0) { break;}
            
            /* If the there is a discontinuity (begining.pts > end.pts) ignore that value */
            if(off != off_first && first.timestamp < last.timestamp && first.offset < last.offset)
            {
                sum_bitrate += (8*45000*(last.offset - first.offset))/(last.timestamp - first.timestamp);
                count++;
            }
            first = last;        

            off += skip;
        }
        if (count != 0) {
            bitrate = sum_bitrate / count;
        }
    }
    return bitrate;
}

/* Check the two halves of a section to see if the birate of the halves is similar*/
static unsigned
b_media_probe_calculate_section_bitrate(bmedia_probe_t probe, bfile_buffer_t buf, bmedia_timestamp_parser_t timestamp_parser, const bmedia_probe_config *config, off_t off_first, off_t off_last, int depth)
{
    int rc;
    int bitrate_first_half=0, bitrate_second_half=0, bitrate = 0, bitrate_difference = 0, bitrate_max = 0;
    unsigned duration;
    bmedia_timestamp first, last, middle;

    if (off_first >= off_last) {
        return 0;
    }
       
    /* Try binary search n times (max_bitrate_probe_depth) or until the section is smaller than B_MEDIA_PROBE_MIN_SEGMENT_LENGTH */
    /* If either of those are true then try to calculate the average bitrate of the entire section based on segments */
    if (depth < config->duration_config.max_bitrate_probe_depth && ((off_last - off_first) > config->duration_config.min_bitrate_segment_size)) {
        rc = b_media_probe_get_timestamp(probe, buf, timestamp_parser, off_first, &first);
        if(rc!=0) {return 0;}

        rc = b_media_probe_get_timestamp(probe, buf, timestamp_parser, (off_first+off_last)/2, &middle);
        if(rc!=0) {return 0;}

        rc = b_media_probe_get_timestamp(probe, buf, timestamp_parser, off_last, &last);
        if(rc!=0) {return 0;}
        
        /* if the offsets are not sequential then there may have been an error */
        if (first.offset >= middle.offset || middle.offset >= last.offset ||
            first.offset >= last.offset)
        {
            return 0;
        }

        if(middle.timestamp > first.timestamp) {
            duration = (middle.timestamp - first.timestamp);
            bitrate_first_half = (8*45000*(middle.offset - first.offset))/duration;
        }
        if(last.timestamp > middle.timestamp) {
            duration = (last.timestamp - middle.timestamp);
            bitrate_second_half = (8*45000*(last.offset - middle.offset))/duration;
        }

        bitrate_difference = bitrate_first_half > bitrate_second_half ? (bitrate_first_half - bitrate_second_half) : (bitrate_second_half - bitrate_first_half);            
        bitrate_max = bitrate_first_half > bitrate_second_half ? ((bitrate_second_half * config->duration_config.max_bitrate_percentage_difference) /100) : ((bitrate_first_half * config->duration_config.max_bitrate_percentage_difference)/100);
        /* If either half bitrate is 0 or if the bitrates are not within a partiuclar range try again */
        if (bitrate_first_half == 0 || bitrate_second_half == 0 ||
            (bitrate_difference > bitrate_max))
        {
            bitrate_first_half = b_media_probe_calculate_section_bitrate(probe, buf, timestamp_parser, config, off_first, (off_last + off_first) / 2, depth + 1);
            bitrate_second_half = b_media_probe_calculate_section_bitrate(probe, buf, timestamp_parser, config, (off_last + off_first) / 2, off_last, depth + 1);
            
            if (bitrate_first_half == 0 && bitrate_second_half != 0 )
            {
                bitrate_first_half = bitrate_second_half;
            }

            if (bitrate_first_half != 0 && bitrate_second_half == 0)
            {
               bitrate_second_half = bitrate_first_half;
            }
        }
        bitrate = (bitrate_first_half + bitrate_second_half) / 2;
    }
    else
    {
        bitrate =  b_media_probe_calculate_segment_bitrate(probe, buf, timestamp_parser, config, off_first, off_last);
    }
    return bitrate;

}



static unsigned
b_media_probe_acquire_duration(bmedia_probe_t probe, bfile_io_read_t fd, bfile_buffer_t buf, bmedia_timestamp_parser_t timestamp_parser, const bmedia_probe_config *config)
{
    bmedia_timestamp first, last, middle;
    int rc;
    int bitrate_max = 0, bitrate_difference = 0;
    off_t off_first, off_last;
    unsigned duration, duration_first_half, duration_second_half;
    unsigned step_back;

    BDBG_OBJECT_ASSERT(probe, bmedia_probe_t);
    BDBG_ASSERT(timestamp_parser);
    BDBG_ASSERT(buf);
    BDBG_ASSERT(fd);

    rc = fd->bounds(fd, &off_first, &off_last);
    if(rc!=0) { goto error;}

    rc = b_media_probe_get_timestamp(probe, buf, timestamp_parser, off_first, &first);
    if(rc!=0) { goto error;}

    rc = b_media_probe_get_timestamp(probe, buf, timestamp_parser, (off_first+off_last)/2, &middle);
    if(rc!=0) { goto error;}

    for(step_back=BMEDIA_PROBE_FEED_SIZE;step_back<B_MEDIA_PROBE_TIMESTAMP_STEP;step_back*=2) {
        bool last_try;

        if(off_first+step_back<=off_last) {
            off_last = off_last - step_back;
            last_try = false;
        } else {
            off_last = off_first+ 3*((off_last - off_first)/4);
            last_try = true;
        }
        rc = b_media_probe_get_timestamp(probe, buf, timestamp_parser, off_last, &last);
        if(rc==0) { 
            break;
        }
        last = middle;
    }
    BDBG_MSG(("b_media_probe_acquire_duration: first:%u,%u middle:%u,%u last:%u,%u", (unsigned)first.offset, (unsigned)first.timestamp, (unsigned)middle.offset, (unsigned)middle.timestamp, (unsigned)last.offset, (unsigned)last.timestamp));

    if(!(first.offset < last.offset)) {
        goto error;
    }
    duration = last.timestamp - first.timestamp;
    duration_first_half = duration;
    duration_second_half = duration;
    if(middle.offset > first.offset)  {
        duration_first_half = middle.timestamp - first.timestamp;
    }
    if(last.offset > first.offset) {
        duration_second_half = last.timestamp - middle.timestamp;
    }
    BDBG_MSG(("b_media_probe_acquire_duration: %#lx duration %u:%u:%u", (unsigned long)probe, duration, duration_first_half, duration_second_half));
    if( 9*(duration/16) < duration_first_half || 9*(duration/16) < duration_second_half || 
        7*(duration/16) > duration_first_half || 7*(duration/16) > duration_second_half ) {
        /* get lowest bitrate */
        unsigned bitrate_first_half=0, bitrate_second_half=0, bitrate;

        if(middle.timestamp > first.timestamp) {
            duration = (middle.timestamp - first.timestamp);
            bitrate_first_half = (8*45000*(middle.offset - first.offset))/duration;
        }
        if(last.timestamp > middle.timestamp) {
            duration = (last.timestamp - middle.timestamp);
            bitrate_second_half = (8*45000*(last.offset - middle.offset))/duration;
        }

        /* For discontinous files it may be necessary to break down the file to smaller sections to ensure duration */
        if (config->duration_config.max_bitrate_n_segments != 0 && config->duration_config.max_bitrate_probe_depth != 0)
        {
            bitrate_difference = bitrate_first_half > bitrate_second_half ? (bitrate_first_half - bitrate_second_half) : (bitrate_second_half - bitrate_first_half);            
            bitrate_max = bitrate_first_half > bitrate_second_half ? ((bitrate_second_half * config->duration_config.max_bitrate_percentage_difference) /100) : ((bitrate_first_half * config->duration_config.max_bitrate_percentage_difference)/100);
            /* If either half bitrate is 0 or if the bitrates are not within a partiuclar range try again */
            if (bitrate_first_half == 0 || bitrate_second_half == 0 ||
                (bitrate_difference > bitrate_max))
            {
                bitrate_first_half = b_media_probe_calculate_section_bitrate(probe, buf, timestamp_parser, config, off_first, (off_last + off_first) / 2, 1);
                bitrate_second_half = b_media_probe_calculate_section_bitrate(probe, buf, timestamp_parser, config, (off_last + off_first) / 2, off_last, 1);
            }
        }

        if ((bitrate_first_half > B_MEDIA_PROBE_MAX_EXPECTED_BITRATE || bitrate_first_half < B_MEDIA_PROBE_MIN_EXPECTED_BITRATE) &&
            (bitrate_second_half <= B_MEDIA_PROBE_MAX_EXPECTED_BITRATE && bitrate_second_half >= B_MEDIA_PROBE_MIN_EXPECTED_BITRATE)) {
            bitrate = bitrate_second_half;
        } else if ((bitrate_second_half > B_MEDIA_PROBE_MAX_EXPECTED_BITRATE || bitrate_second_half < B_MEDIA_PROBE_MIN_EXPECTED_BITRATE) &&
            (bitrate_first_half <= B_MEDIA_PROBE_MAX_EXPECTED_BITRATE && bitrate_first_half >= B_MEDIA_PROBE_MIN_EXPECTED_BITRATE)) {
            bitrate = bitrate_first_half;
        } else if((bitrate_first_half < bitrate_second_half && bitrate_first_half>0) || bitrate_second_half==0) {
            bitrate = bitrate_first_half;
        } else {
            bitrate = bitrate_second_half;
        }
        if(bitrate>0) {
            duration = ((1000*8)*(off_last - off_first))/bitrate;
        } else {
            duration = 0;
        }
        BDBG_MSG(("b_media_probe_acquire_duration: %#lx detected discontinuous stream, bitrate %u:%u, duration:%u", (unsigned long)probe, bitrate_first_half, bitrate_second_half, duration));
    } else {
        duration /= 45;
        BDBG_MSG(("b_media_probe_acquire_duration: %#lx continuous stream, duration:%u", (unsigned long)probe, duration));
    }
    return duration;

error:
    return 0;
}

static const bmedia_probe_stream *
b_media_probe_parse_step(bmedia_probe_t probe, bfile_io_read_t fd, bfile_buffer_t buf, const bmedia_probe_config *config, const char *ext)
{
    unsigned i;
    bool valid_probes[B_MEDIA_N_PROBES];
    const bmedia_probe_stream *stream=NULL;
    const bmedia_probe_stream *stream_only=NULL;
    bmedia_probe_parser_config parser_config;

    parser_config.parse_offset = config->probe_offset;
    parser_config.probe_index = config->probe_index;
    parser_config.parse_index = config->parse_index;
    parser_config.stream_specific = config->stream_specific;
    parser_config.min_parse_request = config->min_probe_request;
    for(i=0;i<B_MEDIA_N_PROBES;i++) {
        valid_probes[i] = true;
    }

    if(config->type == bstream_mpeg_type_unknown || b_media_probe_filter_type(valid_probes, config->type)>0) {
        if(b_media_probe_filter_ext(valid_probes, ext)) {
            if(b_media_probe_filter_header(probe, valid_probes, buf, &parser_config)>0) {
                for(i=0;i<B_MEDIA_N_PROBES;i++) {
                    if(!valid_probes[i]) {
                        continue;
                    }
                    if(probe->probes[i]==NULL) { /* create probe on demand */
                        probe->probes[i]= b_media_probe_formats[i]->create(probe->factory);
                        if(!probe->probes[i]) {
                            BDBG_WRN(("bmedia_probe_parse: %#lx can't create probe for stream type %u", (unsigned long)probe, b_media_probe_formats[i]->type));
                            continue;
                        }
                    }
                    batom_pipe_flush(probe->pipe);
                    BDBG_MSG(("bmedia_probe_parse: %#lx probing type %u", (unsigned long)probe, b_media_probe_formats[i]->type));
                    stream = b_media_probe_formats[i]->parse(probe->probes[i], buf, probe->pipe, &parser_config);
                    if(stream) {
                        ((bmedia_probe_stream *)stream)->probe_id = i;

                        if(config->probe_duration) {
                            const bmedia_probe_track *track;
                            bmedia_timestamp_parser_t timestamp_parser={0};
                            switch(stream->type) {
                            case bstream_mpeg_type_ts:
                                for(track=BLST_SQ_FIRST(&stream->tracks); track; track=BLST_SQ_NEXT(track, link)) {
                                    if(track->type == bmedia_track_type_pcr) {
                                        timestamp_parser = bmpeg2ts_pcr_parser_create(track->number, ((bmpeg2ts_probe_stream*)stream)->pkt_len);
                                        break;
                                    }
                                }
                                break;
                            case bstream_mpeg_type_pes:
                            case bstream_mpeg_type_vob:
                                for(track=BLST_SQ_FIRST(&stream->tracks); track; track=BLST_SQ_NEXT(track, link)) {
                                    if(track->type == bmedia_track_type_video) {
                                        timestamp_parser = bmpeg2pes_pts_parser_create(track->number, 0);
                                        break;
                                    }
                                }
                                break;
                            case bstream_mpeg_type_mpeg1:
                                for(track=BLST_SQ_FIRST(&stream->tracks); track; track=BLST_SQ_NEXT(track, link)) {
                                    if(track->type == bmedia_track_type_video) {
                                        timestamp_parser = bmpeg1_pts_parser_create(track->number, 0);
                                        break;
                                    }
                                }
                                break;
                            default:
                                timestamp_parser = NULL;
                                break;
                            }
                            if(timestamp_parser) {
                                unsigned duration = b_media_probe_acquire_duration(probe, fd, buf, timestamp_parser, config);
                                if(duration>0) {
                                    ((bmedia_probe_stream *)stream)->duration = duration;
                                    ((bmedia_probe_stream *)stream)->max_bitrate = 0; /* clear max_bitrate so it would be re-evaluated based on the stream size and duration */
                                }
                                timestamp_parser->methods->destroy(timestamp_parser);
                            }
                        }
                        if(config->probe_es || config->probe_payload) {
                            if(b_media_probe_formats[i]==&bmpeg2ts_psi_probe) {
                                b_media_probe_parse_aux(probe, (bmedia_probe_stream *)stream, &bmpeg2ts_probe, buf, &parser_config);
                            } else if (b_media_probe_formats[i]==&bmpeg2ts192_psi_probe) {
                                b_media_probe_parse_aux(probe, (bmedia_probe_stream *)stream, &bmpeg2ts192_probe, buf, &parser_config);
                            }
                        }
                        /* Check induced MPEG-2 TS index */
                        if(stream->type == bstream_mpeg_type_ts) {
                            const bmedia_probe_track *track;                          
                            for(track=BLST_SQ_FIRST(&stream->tracks); track; track=BLST_SQ_NEXT(track, link)) {
                                if(track->type == bmedia_track_type_pcr) {
                                    ((bmedia_probe_stream *)stream)->index = bmedia_probe_index_self;
                                    break;
                                }
                            }                            
                        }
			            if(stream->type == bstream_mpeg_type_pes || stream->type == bstream_mpeg_type_vob){
			                const bmedia_probe_track *track;		
			                for(track=BLST_SQ_FIRST(&stream->tracks); track; track=BLST_SQ_NEXT(track, link)) {
			                    if(track->type == bmedia_track_type_video) {
				                    ((bmedia_probe_stream *)stream)->index = bmedia_probe_index_self;
				                    break;
                                }
                            }
                        }
                        if(stream_only) {
                            bmedia_probe_stream_free(probe, stream_only);
                            stream_only = NULL;
                        }
                        if(BLST_SQ_FIRST(&stream->tracks)) {
                            break;
                        }
                        stream_only = stream;
                    }
                }
            }
        }
    }
    if(!stream) {
        stream = stream_only;
    }
    return stream;
}




const bmedia_probe_stream *
bmedia_probe_parse(bmedia_probe_t probe, bfile_io_read_t fd, const bmedia_probe_config *config)
{
    bfile_buffer_cfg buffer_cfg;
    bfile_buffer_t buf;
    const bmedia_probe_stream *stream=NULL;
    const char *ext = NULL;
    void *file_buffer=NULL;


    BDBG_OBJECT_ASSERT(probe, bmedia_probe_t);
    BDBG_ASSERT(fd);
    BDBG_ASSERT(config);
    BDBG_MSG_TRACE(("bmedia_probe_parse>: %#lx fd:%#lx config:%s:%u", (unsigned long)probe, (unsigned long)fd, config->file_name?config->file_name:"", (unsigned)config->type));

    if(!config->buffer) {
        file_buffer = BKNI_Malloc(B_MEDIA_PROBE_N_SEGS * BMEDIA_PROBE_FEED_SIZE);
        if(!file_buffer) {
            goto err_file_buf;
        }
        bfile_buffer_default_cfg(&buffer_cfg);
        buffer_cfg.buf_len = B_MEDIA_PROBE_N_SEGS * BMEDIA_PROBE_FEED_SIZE;
        buffer_cfg.buf = file_buffer;
        buffer_cfg.fd = fd;
        buffer_cfg.nsegs = B_MEDIA_PROBE_N_SEGS;
        buf = bfile_buffer_create(probe->factory, &buffer_cfg);
        if(!buf) {
            BDBG_ERR(("bmedia_probe_parse: %#lx can't create buffer", (unsigned long)probe));
            goto err_buf;
        }
    } else {
        buf = config->buffer;
    }

    if(config->file_name) {
        ext = b_strrchr(config->file_name, '.');
        if(ext) {
            ext++;
        }
        BDBG_MSG(("bmedia_probe_parse: file_name:%s ext:%s", config->file_name, ext?ext:"NONE"));
    }

    stream = b_media_probe_parse_step(probe, fd, buf, config, ext);
    if(!stream && ext && config->probe_all_formats) {
        stream = b_media_probe_parse_step(probe, fd, buf, config, NULL);
    }

    fd->seek(fd, 0, SEEK_SET);

    batom_pipe_flush(probe->pipe);
    if(!stream && config->probe_es) {
        stream = b_media_probe_feed_es(probe, ext, buf);
        if(!stream && ext && config->probe_all_formats) {
            stream = b_media_probe_feed_es(probe, NULL, buf);
        }
    }
    if(stream) {
        if(stream->duration == 0 || stream->max_bitrate==0) {
            off_t first, last;
            int rc = fd->bounds(fd, &first, &last);
            if(rc==0) {
                if(stream->max_bitrate != 0) {
                    ((bmedia_probe_stream *)stream)->duration = ((1000*8)*(last - first))/stream->max_bitrate;
                } else if(stream->duration !=0) {
                    ((bmedia_probe_stream *)stream)->max_bitrate = ((1000*8)*(last - first))/stream->duration;
                }
            }
        }
        if(stream->probe_id < B_MEDIA_N_PROBES) {
            if(b_media_probe_formats[stream->probe_id]==&bmp3_probe) {
                ((bmedia_probe_stream *)stream)->probe_id = BMP3_PROBE_ID;
            } else if(b_media_probe_formats[stream->probe_id]==&bmpeg2ts_psi_probe) {
                ((bmedia_probe_stream *)stream)->probe_id = BMPEG2TS_PSI_PROBE_ID;
            } else if(b_media_probe_formats[stream->probe_id]==&bmpeg2ts192_psi_probe) {
                ((bmedia_probe_stream *)stream)->probe_id = BMPEG2TS192_PSI_PROBE_ID;
            } else if(b_media_probe_formats[stream->probe_id]==&bmpeg2pes_probe) {
                ((bmedia_probe_stream *)stream)->probe_id = BMPEG2PES_PROBE_ID;
            }
        }
    }


    if(!config->buffer) {
        bfile_buffer_destroy(buf);
    }
    if(file_buffer) {
        BKNI_Free(file_buffer);
    }
#if BDBG_DEBUG_BUILD
    if(stream) {
        char buf[128];

        bmedia_stream_to_string(stream, buf, sizeof(buf));
        BDBG_MSG(("bmedia_probe_stream: %s", buf));
    }
#endif
    return stream;
        
err_buf:
    BKNI_Free(file_buffer);
err_file_buf:
    return NULL;
}

void 
bmedia_probe_default_cfg(bmedia_probe_config *config)
{
    BKNI_Memset(config, 0, sizeof(*config));
    config->file_name = NULL;
    config->type = bstream_mpeg_type_unknown;
    config->probe_es = true;
    config->probe_payload = true;
    config->probe_all_formats = true;
    config->probe_duration = true;
    config->probe_index = true;
    config->parse_index = false;
    config->buffer = NULL;
    config->probe_offset = 0;
    config->min_probe_request = 512*1024;
    config->stream_specific.mkv.probe_next_volume = false;
    config->stream_specific.mkv.probe_attachments = false;

    config->duration_config.max_bitrate_n_segments = 0; 
    config->duration_config.max_bitrate_percentage_difference = 0;
    config->duration_config.max_bitrate_probe_depth = 0;
    config->duration_config.min_bitrate_segment_size = 0;
    return;
}

void 
bmedia_probe_stream_free(bmedia_probe_t probe, const bmedia_probe_stream *stream)
{
    const bmedia_probe_format_desc *probe_desc;
    unsigned  probe_id; 
    BDBG_OBJECT_ASSERT(probe, bmedia_probe_t);
    BDBG_ASSERT(stream);
    switch(stream->probe_id) {
    case BMP3_PROBE_ID:
        probe_desc = &bmp3_probe;
        probe_id = B_MEDIA_N_PROBES;
        break;
    case BMPEG2TS_PSI_PROBE_ID:
        probe_desc = &bmpeg2ts_psi_probe;
        probe_id = B_MEDIA_N_PROBES;
        break;
    case BMPEG2TS192_PSI_PROBE_ID:
        probe_desc = &bmpeg2ts192_psi_probe;
        probe_id = B_MEDIA_N_PROBES;
        break;
    case BMPEG2PES_PROBE_ID:
        probe_desc = &bmpeg2pes_probe;
        probe_id = B_MEDIA_N_PROBES;
        break;
    default:
        BDBG_ASSERT(stream->probe_id <= B_MEDIA_N_PROBES);
        probe_desc = NULL;
        probe_id = stream->probe_id;
        break;
    }
    if(probe_desc) {
        for(probe_id=0;probe_id<B_MEDIA_N_PROBES;probe_id++) {
            if(b_media_probe_formats[probe_id] == probe_desc) {
                break;
            }
        }
    }
    if(probe_id <= B_MEDIA_N_PROBES) {  /* workaround for Coverity not understanding BDBG_ASSERT() */
        if(probe_id!=B_MEDIA_N_PROBES) {
            BDBG_ASSERT(probe->probes[probe_id]);
            b_media_probe_formats[probe_id]->stream_free(probe->probes[probe_id], (bmedia_probe_stream *)stream);
        } else {
            /* ES stream */
            bmedia_probe_basic_stream_free(NULL, (bmedia_probe_stream *)stream);
        }
    }
    return;
}

void 
bmedia_probe_stream_init(bmedia_probe_stream *stream, bstream_mpeg_type type)
{
    BLST_SQ_INIT(&stream->tracks);
    stream->type = type;
    stream->max_bitrate = 0;
    stream->duration = 0;
    stream->nprograms = 0;
    stream->ntracks = 0;
    stream->index = bmedia_probe_index_unknown;
    return;
}


void 
bmedia_probe_basic_stream_free(bmedia_probe_base_t probe, bmedia_probe_stream *stream)
{
    bmedia_probe_track *track;
    BSTD_UNUSED(probe);
    BDBG_ASSERT(stream);

    while(NULL!=(track=BLST_SQ_FIRST(&stream->tracks))) {
        BLST_SQ_REMOVE_HEAD(&stream->tracks, link);
        BKNI_Free(track);
    }
    BKNI_Free(stream);
    return;
}

void 
bmedia_probe_track_init(bmedia_probe_track *track)
{
    BKNI_Memset(track, 0, sizeof(*track));
    track->type = bmedia_track_type_other;
    return;
}

void 
bmedia_probe_add_track(bmedia_probe_stream *stream, bmedia_probe_track *new_track)
{
    bmedia_probe_track *prev, *track;
    bool new_program;

    BDBG_ASSERT(stream);
    BDBG_ASSERT(new_track);

    for(track=BLST_SQ_FIRST(&stream->tracks); track; track=BLST_SQ_NEXT(track, link)) {
        if(track->number == new_track->number && track->type == new_track->type && track->program==new_track->program) {
            BDBG_WRN(("bmedia_probe_add_track: %#lx %#lx duplicate %u:%u:%u", (unsigned long)stream, (unsigned long)new_track, new_track->number, new_track->program, new_track->program));
            BKNI_Free(new_track);
            return;
        }
    }
    for(new_program=true, prev=NULL, track=BLST_SQ_FIRST(&stream->tracks); track; track=BLST_SQ_NEXT(track, link)) {
        if(track->program > new_track->program) {
            break;
        }
        new_program = track->program!=new_track->program;
        prev = track;
    }
    if(prev==NULL) {
        BLST_SQ_INSERT_HEAD(&stream->tracks, new_track, link);
    } else {
        BLST_SQ_INSERT_AFTER(&stream->tracks, prev, new_track, link);
    }
    stream->ntracks++;
    if(new_program) {
        stream->nprograms++;
    }
    return;
}

void 
bmedia_probe_video_init(bmedia_probe_video *video)
{
    BKNI_Memset(video, 0, sizeof(*video));
    video->codec = bvideo_codec_unknown;
    video->timestamp_order = bmedia_timestamp_order_display;
    video->height = 0;
    video->width = 0;
    video->bitrate = 0;
    return;
}


