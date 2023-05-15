/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: video.cpp $
 * $brcm_Revision: 69 $
 * $brcm_Date: 2/14/12 4:26p $
 * $brcm_Revision: 69 $ *
 * Module Description: Wrapped for .info file
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/video.cpp $
 * 
 * 69   2/14/12 4:26p erickson
 * SW7425-2363: simplify analyzeIndex(). brutus only supports NAV index
 * format.
 * 
 * 68   11/15/11 9:00a jgarrett
 * SW7425-1593: Adding additional video codecs to supported settop/brutus
 * formats
 * 
 * 67   3/3/11 9:44a erickson
 * SWDEPRECATED-3869: rename brutus "timeshifting" to be "playback &
 * record"
 *
 * 66   2/25/11 2:12p erickson
 * SW7125-818: use BNAV_GetEntrySize
 *
 * 65   2/15/11 2:46p nickh
 * SW7422-214: Add support to the info files for multiple video PID's and
 * types for SVC/MVC
 *
 * 64   3/22/10 3:13p erickson
 * SWDEPRECATED-3958: don't cache the result of Video::lastIndex() while
 * recording
 *
 * 63   2/16/10 5:41p jtna
 * SW3556-1051: use video decoder timestamp reordering
 *
 * 62   8/14/09 3:38p erickson
 * PR55994: refactor thumbnail
 *
 * 61   7/28/09 11:12a lwhite
 * PR55693: Added DTCP/IP intialization
 *
 * 60   7/13/09 1:49p vsilyaev
 * PR 55989: Handle 'self-indexed' MPEG-2 TS files
 *
 * 59   6/12/09 11:10a jtna
 * PR43001: support TS/TTS record
 *
 * 58   6/3/09 12:01p jtna
 * PR43001: IP streams background recording work
 *
 * 57   6/1/09 7:54p jtna
 * PR43001: added record-related brutus script commands
 *
 * 56   5/29/09 2:39p jtna
 * PR43001: refactor brutus record logic
 *
 * 55   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 * 54   12/9/08 2:59p vsilyaev
 * PR 49704: Write multiple audio tracks to info file
 *
 * 53   11/26/08 6:40p vsilyaev
 * PR 49704: Bypass hardoded video_xpt table for new combinations of
 * container/video codec
 *
 * 52   7/22/08 11:32a jtna
 * PR44493: Add 192byte TS playback support in Brutus
 *
 * 51   7/14/08 12:54p ssood
 * PR42739: fix for getting IP tuning work on both Main & PIP
 *
 * 50   6/26/08 3:30p mphillip
 * PR42901: Fix 3DES key length
 *
 * 49   6/24/08 10:03a erickson
 * PR43222: add thumbnail mpeg_type
 *
 * 48   4/2/08 3:54p mward
 * PR40307: Don't include any thumbnail support with PVR_SUPPORT=n.
 *
 * 47   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 46   3/18/08 4:06p erickson
 * PR40307: added thumbnail feature
 *
 * 45   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 44   9/17/07 12:37p jrubio
 * PR34613: added function to record info file information for Network
 * Record
 *
 * 43   8/26/07 10:55a jrubio
 * PR33904: added needed changes for netrecord
 *
 * 42   8/9/07 5:42p jrubio
 * PR33830: set network record field to 0 in init
 *
 * 41   7/26/07 7:20p erickson
 * PR32352: support audio only streams when calling b_lookup_vidxpt_type
 *
 * 40   7/10/07 12:45p erickson
 * PR32813: allow bstream_mpeg to be set by app, then tell Video to derive
 * just the formatName
 *
 * 39   6/26/07 3:54p rjlewis
 * PR27152: close the open asf file (frees allocated memory).
 *
 * 38   6/22/07 1:49p erickson
 * PR30701: added ASSERT
 *
 * 37   6/8/07 1:38p gmohile
 * PR 25109 : Add MP4 support
 *
 * 36   4/25/07 1:26p gmohile
 * PR 29933: AVI Support
 *
 * 35   3/22/07 1:53p erickson
 * PR28776: better error msg when index can't be opened
 *
 * 34   1/26/07 12:12p erickson
 * PR26727: allow audio-only recordings
 *
 * 33   1/18/07 5:30p mphillip
 * PR20035: Set encryption key size
 *
 * 32   12/11/06 6:04p vle
 * PR 25897: fix bcmplayer errors when having ASF files in Brutus Playback
 * list. Use asfpars to obtain the same information for ASF files.
 *
 * 30   11/30/06 5:50p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 29   9/28/06 6:57p arbisman
 * PR24289: Back-out changes
 *
 * 27   8/10/06 12:31p rjlewis
 * PR16324: vxworks fs too slow for this test.
 *
 * 26   7/28/06 4:01p erickson
 * PR23057: only return last_timestamp if index is valid, otherwise use
 * fixedLength
 *
 * 25   7/26/06 2:15p tokushig
 * PR20685: added DRM support to brutus
 *
 * SanDiego_DRM_ASF/2   7/20/06 3:22p tokushig
 * merge from main branch
 *
 * 24   7/20/06 6:31p mphillip
 * PR19544: Cleanup record/playback in brutus with the new format of
 * encryption.key
 *
 * 23   7/13/06 4:12p ahulse
 * PR16324: use record/playback status of currently recording / playing
 * file if active
 *
 * 22   7/11/06 5:47p mphillip
 * PR19544: Initial commit of bcrypt_ stubs
 *
 * SanDiego_DRM_ASF/1   7/7/06 2:46p tokushig
 * added handling for 'for purchase' drm videos
 *
 * 21   3/15/06 4:04p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 20   1/17/06 11:46a erickson
 * PR18651: allow audio type names (i.e. AC3,MPEG) to be used in video's
 * audtype param
 *
 * 19   12/21/05 11:53p erickson
 * PR18256: added copy constructor
 *
 * 18   10/21/05 3:10p dma
 * PR17733: auto pids for ip playback
 *
 * 17   10/13/05 5:25p erickson
 * PR17541: moved general purpose code to mediatypes.c
 *
 * 16   10/13/05 3:11p erickson
 * PR17541: added more audio and video codecs for 7401 and beyond
 *
 * 14   9/1/05 9:46a erickson
 * PR16819: correctly terminate strtoul search
 *
 * 13   8/24/05 6:11p arbisman
 * PR16678: Add support for WinCE
 *
 * 12   8/24/05 5:13p erickson
 * PR16819: added support for reading multiple audpid's and audtype's
 *
 * 11   8/17/05 5:25p erickson
 * PR16719: rework Video and Media to never return MString as return value
 *
 * 10   8/9/05 1:46p rjlewis
 * PR1660: merged vxworks changes from branch PR15236.
 * PR15236: PR15236: Used David's E hack for indexToTime function.
 * Function was making too many disk accesses
 *
 * 9   8/5/05 11:52a erickson
 * PR15137: initial variable to NULL
 *
 * 8   7/21/05 5:50p bandrews
 * PR16138: Added conditional on PVR_SUPPORT to analyze index and index
 * format private fns
 *
 * 7   7/18/05 3:34p erickson
 * PR16138: instead of reading the version, just assume that AVC means
 * BNAV_AVC_Entry
 *
 * 6   7/18/05 10:42a erickson
 * PR16138: calculate unchanging index information once
 *
 * 5   7/15/05 11:21p erickson
 * PR16324: removed inefficient implementation and replaced with
 * simplistic 30 fps until we can address it
 *
 * 4   7/15/05 10:43p erickson
 * PR16322: add another function to set up Video and Media parameters from
 * a bstream_mpeg structure
 *
 * 3   6/29/05 9:04a jjordan
 * PR16042: Add support for "IP channels"
 *
 * 2   5/19/05 6:53p mphillip
 * PR15267: Basic SetTop API support for decryption of AES and DVB files
 * for playback
 *
 * 1   2/7/05 7:55p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/23   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/22   11/19/04 3:30p erickson
 * PR13256: reworked Format logic along with Media baseclass, and now
 * allowing fixedLength for indexless bitrate measurements
 *
 * Irvine_BSEAVSW_Devel/21   7/26/04 7:16p liangl
 * PR11643: merging brutus vxworks support
 *
 * Irvine_BSEAVSW_Devel/20   5/24/04 9:53a erickson
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/1   6/17/04 12:57p liangl
 * fixed a bug
 *
 *
 * Irvine_BSEAVSW_Devel/19   4/30/04 12:05p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/18   2/3/04 10:14a erickson
 * PR9497: typecast for vxworks system calls
 *
 * Irvine_BSEAVSW_Devel/17   12/9/03 12:10p erickson
 * PR8850: support no PVR_SUPPORT
 *
 * Irvine_BSEAVSW_Devel/16   10/31/03 11:57a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/15   10/29/03 11:29a erickson
 * don't try to read index of the filename is null
 *
 * Irvine_BSEAVSW_Devel/14   10/28/03 11:25a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/13   10/22/03 11:21a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/12   10/15/03 11:26a erickson
 * don't assume player is NULL on failed open
 *
 * Irvine_BSEAVSW_Devel/11   10/14/03 9:12a erickson
 * PR8063: converted Video::indexToTime to use bcmplayer timestamps
 * instead of an format dependent conversion
 * from index length to time.
 *
 * Irvine_BSEAVSW_Devel/10   9/17/03 12:31p erickson
 * converted from original debug interface to magnum debug interface
 *
 * Irvine_BSEAVSW_Devel/9   8/27/03 4:38p erickson
 * initialize all members in constructor
 * fixed read of encryption keys
 *
 * Irvine_BSEAVSW_Devel/8   8/22/03 5:21p erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/7   8/19/03 11:55a erickson
 * conversion to settop api in progress
 *
 ***************************************************************************/
#include "video.h"

#ifndef _WIN32_WCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef PVR_SUPPORT
#include "tsindexer.h"
#include "bcmplayer.h"
#endif
#include <mstringlist.h>
#include "bstd.h"
#include "bkni.h"
#include "brutus_os.h"
BDBG_MODULE(brutus_video);

Video::Video() {
    BDBG_MSG(("Video::Video()"));
    isHits = false;
    bstream_mpeg_init(&mpeg);
    isEncoded = false;
    isForPurchase = false;
    encodingQuality = bencode_quality_basic;
    resumeindex = 0;
    fixedLength = 0;
    timestamp_enabled = false;
    timestamp_override = false;
    stream = NULL;
    record = NULL;
    tuner = NULL;
    playRefCnt = 0;
    index_filesize = -1;
    last_indexquery = -1;
    last_timestamp = -1;
    memset(&index, 0, sizeof(index));
    isIP = false;
    dtcpIpCtx = NULL;
    doAutoPids = false; /* dma: init  */
    isContinuousRecord = false;
    continuousRecordDuration = 60 * 5;
    isNetRecord = false;
    startPlaybackOfRecord = false;
    isBackgroundRecord = false;
    ch = -1;
#ifdef PVR_SUPPORT
    BKNI_Memset(&thumbnail_data, 0, sizeof(thumbnail_data));
#endif
}

Video::Video(Video &video) : Media(video)
{
    BDBG_MSG(("Video::Video(Video &video)"));
    indexfile = video.indexfile;
    isHits = video.isHits;
    mpeg = video.mpeg;
    isEncoded = video.isEncoded;
    isForPurchase = video.isForPurchase;
    encodingQuality = video.encodingQuality;
    resumeindex = video.resumeindex;
    memset(&index, 0, sizeof(index));
    fixedLength = video.fixedLength;
    timestamp_enabled = video.timestamp_enabled;
    stream = video.stream;
    record = video.record;
    tuner = video.tuner;
    playRefCnt = video.playRefCnt;

    isIP = video.isIP;
    socket = video.socket;
    awindow[video.decoderIndex] = video.awindow[video.decoderIndex];
    adecode[video.decoderIndex] = video.adecode[video.decoderIndex];
    doAutoPids = video.doAutoPids;

    index_filesize = video.index_filesize;
    last_indexquery = video.last_indexquery;
    last_timestamp = video.last_timestamp;
    _indexfilePath = video._indexfilePath;
    isContinuousRecord = video.isContinuousRecord;
    continuousRecordDuration = video.continuousRecordDuration;

#ifdef PVR_SUPPORT
    BKNI_Memset(&thumbnail_data, 0, sizeof(thumbnail_data)); /* don't copy thumbnails. they will regenerate if needed. */
#endif
}

Video::~Video()
{
#ifdef PVR_SUPPORT
    destroyThumbnails();
#endif
}


bool Video::readNameValue(const char *name, const char *val)
{
    if (!strcasecmp(name, "indexfile"))
        indexfile = val;
    else if (!strcasecmp(name, "vidpid"))
	{
        int i = 0;
        while (*val) {
            mpeg.video[i++].pid = strtoul(val, (char **)&val, 0);
            if (*val) val++; /* if not terminated, assume it's a single comma */
        }
	}
    else if (!strcasecmp(name, "audpid")) {
        int i = 0;
        while (*val) {
            mpeg.audio[i++].pid = strtoul(val, (char **)&val, 0);
            if (*val) val++; /* if not terminated, assume it's a single comma */
        }
    }
    else if (!strcasecmp(name, "audtype")) {
        int i = 0;
        while (*val) {
            char *nextval;
            mpeg.audio[i++].format = b_lookup_audtype(val, &nextval);
            val = nextval;
            if (*val) val++; /* if not terminated, assume it's a single comma */
        }
    }
    else if (!strcasecmp(name, "pcrpid"))
        mpeg.pcr_pid = MString(val).toInt();
    else if (!strcasecmp(name, "resumeindex"))
        resumeindex = MString(val).toInt();
    else if (!strcasecmp(name, "fixedlength")) /* in seconds */
        fixedLength = MString(val).toInt() * 1000; /* convert to milliseconds */
    else if (!strcasecmp(name, "mpegformat")) {
        isHits = !strcasecmp(val, "HITS");
    }
    else if (!strcasecmp(name, "purchase")) {
        if (!strcasecmp(val, "1"))
            isForPurchase = true;
    }
    else if (!strcasecmp(name, "quality")) {
        isEncoded = true;
        if (!strcasecmp(val, "best"))
            encodingQuality = bencode_quality_best;
        else if (!strcasecmp(val, "high"))
            encodingQuality = bencode_quality_high;
        else if (!strcasecmp(val, "medium"))
            encodingQuality = bencode_quality_medium;
        else if (!strcasecmp(val, "basic"))
            encodingQuality = bencode_quality_basic;
        else {
            // legacy support
            isEncoded = false;
            isHits = !strcasecmp(val, "HITS");
        }
    }
    else if (!strcasecmp(name, "enc_type")) {
        if (!strcasecmp(val, "dvb")) {
            mpeg.encryption.type = bencryption_type_dvb;
            mpeg.encryption.key_length = 128;
        } else if (!strcasecmp(val, "aes")) {
            mpeg.encryption.type = bencryption_type_aes;
            mpeg.encryption.key_length = 128;
        } else if (!strcasecmp(val, "3des")) {
            mpeg.encryption.type = bencryption_type_3des;
            mpeg.encryption.key_length = 128;
        } else if (!strcasecmp(val, "des")) {
            mpeg.encryption.type = bencryption_type_des;
            mpeg.encryption.key_length = 64;
        } else {
            mpeg.encryption.type = bencryption_type_none;
        }
    }
    else if (!strcasecmp(name, "enc_keys")) {
        uint32_t tmp_key[8];
        sscanf(val, "%x,%x,%x,%x",
            &tmp_key[0],
            &tmp_key[1],
            &tmp_key[2],
            &tmp_key[3]);
        memcpy(mpeg.encryption.key, tmp_key, sizeof(tmp_key));
    }
    else if (!strcasecmp(name, "timestamp_enabled")) {
        if (!strcasecmp(val, "1"))
            timestamp_enabled = true;
    }
    else if (!strcasecmp(name, "timestamp_order")) {
        if (!strcasecmp(val, "decode")) {
            mpeg.timestamp_order = bdecode_timestamp_order_decode;
        }
    }
    else
        return Media::readNameValue(name,val);
    return true;
}

int Video::writeNameValues(FILE *f) {
    BDBG_MSG(("Video::writeNameValues(%p)",f));
    if (indexfile)
        fprintf(f, "indexfile=%s\n", (const char *)indexfile);

    char pids[128];
    int pids_pos=0;
    char types[256];
    int types_pos=0;

    for(unsigned i=0;i<sizeof(mpeg.video)/sizeof(mpeg.video[0]);i++) {
        int rc;
        if(i>0 && mpeg.video[i].pid==0) {
            break;
        }
        rc = BKNI_Snprintf(pids+pids_pos, sizeof(pids)-pids_pos, "%s0x%x",i>0?",":"",mpeg.video[i].pid);
        if(rc<0) {break;}
        pids_pos+=rc;
        rc = BKNI_Snprintf(types+types_pos, sizeof(types)-types_pos, "%s0x%x",i>0?",":"",mpeg.video[i].format);
        if(rc<0) {break;}
        types_pos+=rc;
    }
    pids[pids_pos]='\0';
    types[types_pos]='\0';

    fprintf(f, "vidpid=%s\n", pids);
    fprintf(f, "vidtype=%s\n", types);

    fprintf(f, "pcrpid=0x%x\n", mpeg.pcr_pid);

    pids_pos=0;
    types_pos=0;

    for(unsigned i=0;i<sizeof(mpeg.audio)/sizeof(mpeg.audio[0]);i++) {
        int rc;
        if(i>0 && mpeg.audio[i].pid==0) {
            break;
        }
        rc = BKNI_Snprintf(pids+pids_pos, sizeof(pids)-pids_pos, "%s0x%x",i>0?",":"",mpeg.audio[i].pid);
        if(rc<0) {break;}
        pids_pos+=rc;
        rc = BKNI_Snprintf(types+types_pos, sizeof(types)-types_pos, "%s0x%x",i>0?",":"",mpeg.audio[i].format);
        if(rc<0) {break;}
        types_pos+=rc;
    }
    pids[pids_pos]='\0';
    types[types_pos]='\0';

    fprintf(f, "audpid=%s\n", pids);
    fprintf(f, "audtype=%s\n", types);
    if (isHits)
        fprintf(f, "mpegformat=HITS\n");
    if (isEncoded)
        fprintf(f, "quality=%s\n", g_qualityStr[encodingQuality]);
    if (isForPurchase)
        fprintf(f, "purchase=%d\n", isForPurchase);
    if (mpeg.encryption.type != bencryption_type_none) {
        uint32_t tmp_key[8];
        memcpy(tmp_key,mpeg.encryption.key,sizeof(tmp_key));
        fprintf(f, "enc_type=%s\n", g_encryptionStr[mpeg.encryption.type]);
        fprintf(f, "enc_keys=%x,%x,%x,%x\n",
            tmp_key[0],
            tmp_key[1],
            tmp_key[2],
            tmp_key[3]);
    }
    if (resumeindex)
        fprintf(f, "resumeindex=%d\n", resumeindex);
    if (fixedLength)
        fprintf(f, "fixedlength=%d\n", fixedLength/1000);
    if (timestamp_enabled)
        fprintf(f, "timestamp_enabled=%d\n", 1);
    if (mpeg.timestamp_order == bdecode_timestamp_order_decode) { /* don't write this field if display order (default) */
        fprintf(f, "timestamp_order=decode\n");
    }
    return 0;
}

int Video::writeNameValuesNetwork(char *f) {
    int temp =0;
    BDBG_MSG(("Video::writeNameValues(%p)",f));
    if (indexfile)
        sprintf(f, "indexfile=%s\n", (const char *)indexfile);
    f += (strlen(f)-temp);
    temp = strlen(f);

    sprintf(f, "vidpid=0x%x\n", mpeg.video[0].pid);
    f += (strlen(f)-temp);
    temp = strlen(f);
    sprintf(f, "pcrpid=0x%x\n", mpeg.pcr_pid);
    f += (strlen(f)-temp);
    temp = strlen(f);
    sprintf(f, "audpid=0x%x\n", mpeg.audio[0].pid);
    f += (strlen(f)-temp);
    temp = strlen(f);
    sprintf(f, "audtype=0x%x\n", mpeg.audio[0].format);
    f += (strlen(f)-temp);
    temp = strlen(f);
    if (isHits)
        sprintf(f, "mpegformat=HITS\n");
    if (isEncoded)
        sprintf(f, "quality=%s\n", g_qualityStr[encodingQuality]);
    f += (strlen(f)-temp);
    temp = strlen(f);
    if (isForPurchase)
        sprintf(f, "purchase=%d\n", isForPurchase);
    f += (strlen(f)-temp);
    temp = strlen(f);
    if (mpeg.encryption.type != bencryption_type_none) {
        uint32_t tmp_key[8];
        memcpy(tmp_key,mpeg.encryption.key,sizeof(tmp_key));
        sprintf(f, "enc_type=%s\n", g_encryptionStr[mpeg.encryption.type]);
        sprintf(f, "enc_keys=%x,%x,%x,%x\n",
            tmp_key[0],
            tmp_key[1],
            tmp_key[2],
            tmp_key[3]);
    }
    f += (strlen(f)-temp);
    temp = strlen(f);

    if (resumeindex)
        sprintf(f, "resumeindex=%d\n", resumeindex);
    f += (strlen(f)-temp);
    temp = strlen(f);

    if (fixedLength)
        sprintf(f, "fixedlength=%d\n", fixedLength/1000);


    return 0;
}


int Video::deleteFiles() {
    BDBG_MSG(("Video::deleteFiles()"));
    if (indexfile)
        b_delete_file(indexfilePath());
    return Media::deleteFiles();
}

int Video::lastIndex() {
    int result = index.total;
#ifdef PVR_SUPPORT
    analyzeIndex();
    if (indexfile) {
        /* if the file is being recorded, the size changes, otherwise it's fixed */
        if (record || !result) {
            int64_t fileSize;
            if(!b_get_file_size(indexfilePath(), &fileSize)) {
                BDBG_ASSERT(index.sizeof_entry);
                result = fileSize / index.sizeof_entry;
                if (!record) {
                    /* if we're not recording, we can cache the result */
                    index.total = result;
                }
            }
        }
    }
#endif
    return result;
}

/**
As an optimization, compute internal information once.
Don't compute things that may change, like lastIndex for a recording stream.
**/
void Video::analyzeIndex() {
    BDBG_MSG(("Video::analyzeIndex()"));
#ifdef PVR_SUPPORT
    /* don't need to recompute */
    if (index.computed) return;

    /* brutus only supports NAV files */
    if (indexfile != NULL)
        index.format = bindex_format_bcm;
    else
        index.format = bindex_format_none;

    /* determine size of each index entry */
    switch (index.format) {
    case bindex_format_bcm:
        if (mpeg.video[0].format == bvideo_codec_h264) {
            index.sizeof_entry = BNAV_GetEntrySize(BNAV_Version_AVC);
        }
        else {
            index.sizeof_entry = BNAV_GetEntrySize(BNAV_VersionOriginal);
        }
        break;
    default:
        index.sizeof_entry = 1; /* doesn't matter */
        break;
    }

    index.computed = true;
#endif
}

bindex_format Video::indexFormat() {
    BDBG_MSG(("Video::indexFormat()"));
#ifdef PVR_SUPPORT
    analyzeIndex();
    return index.format;
#else
    return bindex_format_none;
#endif
}

int Video::length(bplayback_t playback) {
    bplayback_status status;

    bplayback_get_status( playback, &status);
    if (status.has_index)
        return status.last_timestamp;
    else
        return fixedLength;
}

int Video::length() {
    brecord_status rec_status;
    int last_index;
    int result = -1;

    last_index = lastIndex();

    if( record ) {
        brecord_get_status( record, &rec_status );
        return rec_status.last_timestamp;
    }

    if (last_index)
        result = indexToTime(last_index-1);;
    if (result == -1)
        result = fixedLength;
    return result;
}

#ifdef PVR_SUPPORT
#include "bcmplayer.h"
#endif
int Video::indexToTime(int index) {
    BDBG_MSG(("Video::indexToTime(%d)",index));
#ifndef PVR_SUPPORT
    return -1;
#else
#if 1
/* PR 16324 - this may be a theoretically correct implementation, but it is
far too inefficient. the main intention is to measure time on streams
with varying frame rates. better options include:
1) measure once and saving to the Video structure. the framerate is not
likely to change midstream.
2) add an API to bcmplayer which can be called without instatiating the
entire thing. also, keep the index open. don't open/close.
3) add an API to bplayback.
For now, I'm going to hardcode for 30 fps.
*/
    int64_t fileSize;
    int timestamp = -1;
    FILE *file;

    /* must check the filesize first */
    if (indexfile.isNull())
        return -1;
    /* if indexfile and mediafile point to the same file, it's not really a NAV index */
    if (indexfile == mediafile)
        return -1;
    if (!indexfile || b_get_file_size((char *) indexfilePath(), &fileSize))
        return -1;

    /* check the cache from a previous call to indexToTime */
    if (fileSize == index_filesize &&
        last_indexquery == index)
    {
        return last_timestamp;
    }

    /* Only TS files have bcmplayer-capable indexes */
    if (mpeg.mpeg_type != bstream_mpeg_type_ts) {
        return -1;
    }

    /* we have to read the index using a temporary instance of bcmplayer */
    BNAV_Player_Handle player = NULL;
    BNAV_Player_Settings settings;
    BNAV_Player_Position pos;

    file = fopen(indexfilePath(), "rb");
    if (!file) goto error;

    BNAV_Player_GetDefaultSettings(&settings);
    settings.videoPid = 1; // fake it out
    settings.readCb = (BP_READ_CB)fread;
    settings.tellCb = (BP_TELL_CB)ftell;
    settings.seekCb = (BP_SEEK_CB)fseek;
    settings.filePointer = file;

#ifdef __vxworks
    settings.skipSanityCheck = true; /* sanity check adds delay to startup -- skip to speed it up! */
#endif

    if (BNAV_Player_Open(&player, &settings)) {
        player = NULL; // not guaranteed to be left NULL
        BDBG_ERR(("Unable to open index %s", (const char *)indexfilePath()));
        indexfile = NULL; /* don't open it again */
        goto error;
    }
    if (BNAV_Player_SetCurrentIndex(player, index)) goto error;
    BNAV_Player_GetPosition(player, &pos);
    timestamp = pos.timestamp;
//  BDBG_WRN(("  result: %lld,%d==>%d", fileSize, index, timestamp));

    /* remember these values */
    index_filesize = fileSize;
    last_indexquery = index;
    last_timestamp = timestamp;

error:
    if (player) BNAV_Player_Close(player);
    if (file) fclose(file);
    return timestamp;

#else
    /* hardcoded for 30 fps for efficiency. */
    return index * 1000 / 30;
#endif
#endif
}

const char *Video::indexfilePath()
{
    /* compute once */
    if (!_indexfilePath) {
        addPath(_indexfilePath, indexfile);
    }
    return _indexfilePath;
}

const char *Video::getIndexFileName()
{
    return indexfile;
}

void Video::setPath(const char *path)
{
    _indexfilePath = NULL; /* clear */
    Media::setPath(path);
}

static struct vidtypes *
b_lookup_dynamic_vidxpt_typ(bvideo_codec video_codec, bstream_mpeg_type mpeg_type, char *buf, size_t buf_size)
{
    struct vidtypes *vt;
    vt = b_lookup_vidxpt_type(video_codec, mpeg_type);
    if(vt) {
        return vt;
    }
    if(buf==NULL || buf_size<sizeof(*vt)) {
        return NULL;
    }
    const char *video = b_vidtype_str(video_codec);
    const char *mpeg = b_xpttype_str(mpeg_type);
    if(video_codec==bvideo_codec_unknown || video==NULL || b_lookup_vidtype(video, NULL)!=video_codec) {
        if ( video_codec==bvideo_codec_unknown ) BDBG_ERR(("Unknown codec"));
        if ( video==NULL ) BDBG_ERR(("Unknown codec"));
        if ( b_lookup_vidtype(video, NULL)!=video_codec ) BDBG_ERR(("Lookup failed '%s'", video));
        BDBG_ERR(("Bad codec"));
        return NULL;
    }
    if(mpeg_type==bstream_mpeg_type_unknown|| mpeg==NULL || b_lookup_xpttype(mpeg, NULL)!=mpeg_type) {
        BDBG_ERR(("Bad mpeg type"));
        return NULL;
    }

    vt = (struct vidtypes *)buf;
    buf += sizeof(*vt);
    buf_size -= sizeof(*vt);
    vt->vidtype = video_codec;
    vt->mpeg_type = mpeg_type;
    vt->infofile_name = buf;
    vt->name = buf+1;
    int rc = BKNI_Snprintf(buf,buf_size, "_%s_%s", video, mpeg);
    if(rc<0) {
        return NULL;
    }
    return vt;
}

struct vidtypes *
b_lookup_dynamic_vidxpt_type_by_infofile(const char *aFormatName, char *buf, size_t buf_size)
{
    const char *mpeg;
    struct vidtypes *vt = b_lookup_vidxpt_type_by_infofile(aFormatName);
    if(vt) {
        return vt;
    }
    if(buf==NULL || buf_size<sizeof(*vt)) {
        return NULL;
    }
    if(aFormatName[0]!='_') {
        return NULL;
    }
    aFormatName++;
    vt = (struct vidtypes *)buf;
    buf += sizeof(*vt);
    buf_size -= sizeof(*vt);
    mpeg = strchr(aFormatName,'_');
    if(!mpeg) {
        return NULL;
    }

    char *dst;
    const char *src;
    for(src=aFormatName,dst=buf;src!=mpeg;src++,dst++,buf_size--) {
        if(buf_size==0) {
            return NULL;
        }
       *dst = *src;
    }
    *dst='\0';
    bvideo_codec video_codec = b_lookup_vidtype(buf, NULL);
    buf=dst;
    mpeg++;
    bstream_mpeg_type mpeg_type = b_lookup_xpttype(mpeg, NULL);
    if(video_codec == bvideo_codec_unknown || mpeg == bstream_mpeg_type_unknown) {
        return NULL;
    }
    vt->vidtype = video_codec;
    vt->mpeg_type = mpeg_type;
    vt->infofile_name = aFormatName;
    vt->name = aFormatName;
    return vt;
}

int Video::deriveFormatName()
{
    bvideo_codec video_codec;
    struct vidtypes *vt;
    char buf[128];

    if (mpeg.video[0].pid) {
        video_codec = mpeg.video[0].format;
		if (mpeg.video[1].pid)
        	video_codec = mpeg.video[1].format;
    }
    else if (mpeg.mpeg_type == bstream_mpeg_type_asf) {
        video_codec = bvideo_codec_vc1;
    }
    else {
        video_codec = bvideo_codec_mpeg2;
    }
    vt = b_lookup_dynamic_vidxpt_typ(video_codec, mpeg.mpeg_type, buf, sizeof(buf));
    if (!vt) return -1;

    formatName = vt->infofile_name;
    formatNumber = 0;
    mimeType = "unknown";
    type = Media::eVideo;
    return 0;
}

static int b_set_vidtype(Video *video, struct vidtypes *v)
{
    BDBG_MSG(("b_set_vidtype()"));
    video->formatName = v->infofile_name;
    video->formatNumber = 0;
    video->mimeType = "unknown";
    video->type = Media::eVideo;
    video->mpeg.video[0].format = v->vidtype;
	if (video->mpeg.video[0].format == bvideo_codec_h264_svc || video->mpeg.video[0].format == bvideo_codec_h264_mvc)
    	video->mpeg.video[1].format = v->vidtype;
    video->mpeg.mpeg_type = v->mpeg_type;
    return 0;
}

int Video::set(const bstream_mpeg *ampeg)
{
    char buf[32];
    BDBG_MSG(("Video::set(%p)",ampeg));
    if (ampeg->video[0].pid) {
        struct vidtypes *vt = b_lookup_dynamic_vidxpt_typ(ampeg->video[0].format, ampeg->mpeg_type, buf, sizeof(buf));
        if (vt) {
            b_set_vidtype(this, vt); /* this sets necessary Video and Media fields */
            mpeg = *ampeg; /* and save the whole thing */
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        /* TEMP: if audio only, then fake this out with MPEG video. eventually, this will be fixed when
        Brutus recognizes a vidtype separate from a transport type */
        struct vidtypes *vt = b_lookup_dynamic_vidxpt_typ(ampeg->video[0].format, ampeg->mpeg_type, buf, sizeof(buf));
        b_set_vidtype(this, vt); /* this sets necessary Video and Media fields */
        mpeg = *ampeg; /* and save the whole thing */
    }
}

int Video::setFormat(const char *aFormatName)
{
    char buf[128];
    BDBG_MSG(("Video::setFormat('%s')",aFormatName));
    struct vidtypes *vt = b_lookup_dynamic_vidxpt_type_by_infofile(aFormatName, buf, sizeof(buf));
    if (vt) {
        return b_set_vidtype(this, vt);
    }
    else {
        return Media::setFormat(aFormatName);
    }
}

#ifdef PVR_SUPPORT
int Video::createThumbnails(bthumbnail_manager_t manager)
{
    bthumbnail_stream_settings settings;
    const char *indexfile = indexfilePath();
    int rc;

    /* if there was no manager, then there's nothing to do */
    if (!manager) {
        return -1;
    }

    if (!indexfile) {
        BDBG_ERR(("index file required"));
        return -1;
    }

    thumbnail_data.manager = manager;
    thumbnail_data.data_stdiofile = fopen(mediafilePath(), "r");
    if (!thumbnail_data.data_stdiofile) {
        BDBG_ERR(("unable to open media file %s for thumbnail processing", mediafilePath()));
        goto error;
    }
    thumbnail_data.data_bfile = bfile_stdio_read_attach(thumbnail_data.data_stdiofile);

    thumbnail_data.index_stdiofile = fopen(indexfile, "r");
    if (!thumbnail_data.index_stdiofile) {
        BDBG_ERR(("unable to open index file %s for thumbnail processing", indexfile));
        goto error;
    }
    thumbnail_data.index_bfile = bfile_stdio_read_attach(thumbnail_data.index_stdiofile);

    bthumbnail_stream_get_default_create_settings(&thumbnail_data.create_settings);
    thumbnail_data.create_settings.name = title; /* debug */
    thumbnail_data.create_settings.user_context = this;
    thumbnail_data.stream = bthumbnail_stream_create(thumbnail_data.manager, &thumbnail_data.create_settings);
    if (!thumbnail_data.stream) {
        BDBG_ERR(("unable to create thumbnail stream"));
        goto error;
    }

    bthumbnail_stream_get_settings(thumbnail_data.stream, &settings);
    settings.first_time = 0;
    settings.last_time = length();
    /* these are the same numbers as playbackscreen.cpp. it's not required to be, but it saves some processing time. */
    settings.full_view.number = 5;
    settings.full_view.min_spacing = 10;
    bthumbnail_stream_set_settings(thumbnail_data.stream, &settings);

    return 0;

error:
    destroyThumbnails();
    return -1;
}

void Video::destroyThumbnails()
{
    if (thumbnail_data.stream) {
        bthumbnail_stream_destroy(thumbnail_data.stream);
    }
    if (thumbnail_data.index_bfile) {
        bfile_stdio_read_detach(thumbnail_data.index_bfile);
    }
    if (thumbnail_data.index_stdiofile) {
        fclose(thumbnail_data.index_stdiofile);
    }
    if (thumbnail_data.data_bfile) {
        bfile_stdio_read_detach(thumbnail_data.data_bfile);
    }
    if (thumbnail_data.data_stdiofile) {
        fclose(thumbnail_data.data_stdiofile);
    }
    BKNI_Memset(&thumbnail_data, 0, sizeof(thumbnail_data));
}
#endif
