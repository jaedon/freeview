/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: video.h $
 * $brcm_Revision: 36 $
 * $brcm_Date: 3/3/11 9:45a $
 *
 * Module Description: Wrapped for .info file
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/video.h $
 * 
 * 36   3/3/11 9:45a erickson
 * SWDEPRECATED-3869: rename brutus "timeshifting" to be "playback &
 * record"
 *
 * 35   8/14/09 3:38p erickson
 * PR55994: refactor thumbnail
 *
 * 34   7/28/09 11:10a lwhite
 * PR55693: Added DTCP/IP intialization
 *
 * 33   6/12/09 11:11a jtna
 * PR43001: support TS/TTS record
 *
 * 32   6/3/09 12:01p jtna
 * PR43001: IP streams background recording work
 *
 * 31   6/1/09 7:55p jtna
 * PR43001: added record-related brutus script commands
 *
 * 30   5/29/09 2:39p jtna
 * PR43001: refactor brutus record logic
 *
 * 29   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 * 28   7/14/08 12:54p ssood
 * PR42739: fix for getting IP tuning work on both Main & PIP
 *
 * 27   4/2/08 3:54p mward
 * PR40307: Don't include any thumbnail support with PVR_SUPPORT=n.
 *
 * 26   3/18/08 4:06p erickson
 * PR40307: added thumbnail feature
 *
 * 25   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 24   9/17/07 12:37p jrubio
 * PR34613: added function to record info file information for Network
 * Record
 *
 * 23   8/26/07 10:55a jrubio
 * PR33904: added needed changes for netrecord
 *
 * 22   8/9/07 5:36p jrubio
 * PR33830:added needed fields for Network record
 *
 * 21   7/10/07 12:45p erickson
 * PR32813: allow bstream_mpeg to be set by app, then tell Video to derive
 * just the formatName
 *
 * 20   6/11/07 1:04p jjordan
 * PR32034: Add support for TTS streams in IP channels
 *
 * 19   11/30/06 5:50p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 18   9/28/06 6:57p arbisman
 * PR24289: Back-out changes
 *
 * 16   7/26/06 2:15p tokushig
 * PR20685: added DRM support to brutus
 *
 * SanDiego_DRM_ASF/2   7/20/06 3:22p tokushig
 * merge from main branch
 *
 * 15   7/13/06 4:18p ahulse
 * PR16324: use record/playback status of currently recording / playing
 * file if active
 *
 * SanDiego_DRM_ASF/1   7/7/06 2:47p tokushig
 * added handling for 'for purchase' drm videos
 *
 * 14   4/7/06 10:52a ahulse
 * PR20504: Merge Vpp Modifications into main branch
 *
 * Vpp_7042_Demo/1   3/29/06 5:51p ahulse
 * PR20504: Enable support for changeing 7042's vpp settings when encoding
 * analog
 *
 * 13   3/15/06 4:04p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 12   12/21/05 11:53p erickson
 * PR18256: added copy constructor
 *
 * 11   10/21/05 3:05p dma
 * PR17733: auto pids for IP playback
 *
 * 10   10/13/05 5:25p erickson
 * PR17541: moved general purpose code to mediatypes.c
 *
 * 9   8/17/05 5:25p erickson
 * PR16719: rework Video and Media to never return MString as return value
 *
 * 8   7/18/05 3:34p erickson
 * PR16138: removed unused and now invalid code
 *
 * 7   7/18/05 10:42a erickson
 * PR16138: calculate unchanging index information once
 *
 * 6   7/15/05 10:43p erickson
 * PR16322: add another function to set up Video and Media parameters from
 * a bstream_mpeg structure
 *
 * 5   6/29/05 9:04a jjordan
 * PR16042: Add support for "IP channels"
 *
 * 4   5/19/05 6:53p mphillip
 * PR15267: Basic SetTop API support for decryption of AES and DVB files
 * for playback
 *
 * 3   4/27/05 1:15p erickson
 * PR15075: use int64_t for 64 bit file offset instead of assuming off_t
 * is 64 bits (it's not on vxworks)
 *
 * 2   2/14/05 4:08p erickson
 * PR11077: cache the tuner as well as record for timeshifting video
 *
 * 1   2/7/05 7:55p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/11   11/19/04 3:30p erickson
 * PR13256: reworked Format logic along with Media baseclass, and now
 * allowing fixedLength for indexless bitrate measurements
 *
 * Irvine_BSEAVSW_Devel/10   4/30/04 12:05p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/9   10/28/03 11:25a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/8   10/22/03 11:21a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/7   10/14/03 9:12a erickson
 * PR8063: converted Video::indexToTime to use bcmplayer timestamps
 * instead of an format dependent conversion
 * from index length to time.
 *
 * Irvine_BSEAVSW_Devel/6   9/18/03 5:04p erickson
 * reworked settop api, driver and brutus directory structure
 *
 * Irvine_BSEAVSW_Devel/5   8/22/03 5:21p erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/4   8/18/03 4:54p erickson
 * timeshifting pvr now working
 *
 * Irvine_BSEAVSW_Devel/3   8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 *
 * Irvine_BSEAVSW_Devel/2   7/23/03 6:25p erickson
 * allow absolute index/media file paths
 *
 * SanJose_Linux_Devel/16   4/30/03 9:12a erickson
 * audio rework
 *
 * SanJose_Linux_Devel/15   4/9/03 11:22a erickson
 * rework of pvrgui files and class names
 *
 * SanJose_Linux_Devel/14   1/29/03 11:00a erickson
 * added marked flag for new libraryscreen algorithm
 *
 * SanJose_Linux_Devel/13   10/18/02 10:12a erickson
 * supporting multiple directories for BBX demo
 *
 * SanJose_Linux_Devel/12   8/5/02 3:37p erickson
 * added encryption support
 *
 * SanJose_Linux_Devel/11   7/17/02 4:44p erickson
 * analogType must now be based to 7115 record driver
 *
 * SanJose_Linux_Devel\10   6/25/02 11:31a erickson
 * dss support
 *
 * SanJose_Linux_Devel\9   6/17/02 4:29p erickson
 * added currentUsage to improve control.cpp performance
 *
 * SanJose_Linux_Devel\8   5/23/02 12:24p erickson
 * pre 0523 7115 label
 *
 *
 ***************************************************************************/
#ifndef VIDEO_H
#define VIDEO_H

#include "bsettop.h"
#include "media.h"
#include "mediatypes.h"
#ifdef PVR_SUPPORT
#include "bthumbnail.h"
#endif
#include "bfile_stdio.h"
#include <stdio.h>

/**
Video extends Media with Settop API information.

Video contains all the information needed to playback an MPEG and INDEX file.
It can read and write itself to disk, as well as delete itself from disk along with the
MPEG and INDEX file.
**/
class Video : public Media {
public:
    Video();
    Video(Video &video); // copy constructor
    ~Video();

    virtual int setFormat(const char *formatName); /* override of Media */
    int deriveFormatName(); /* set the required formatName based on bstream_mpeg */
    virtual void setPath(const char *path); /* override of Media */
    int set(const bstream_mpeg *mpeg);

    MString indexfile;
    bool isHits;
    bstream_mpeg mpeg;
    bool isEncoded;
    bencode_quality encodingQuality;
    bencode_vpp_mode encodingVpp;
    unsigned long resumeindex;
    bool isForPurchase;

    // returns full path and filename of indexfile
    const char *indexfilePath();

    // returns index filename (not including path)
    const char *getIndexFileName();

    // transient values
    struct {
        bool computed; /* if false, needs to be computed */
        bindex_format format;
        unsigned sizeof_entry;
        long total; /* cache of total number of index entries */
    } index;
    unsigned long fixedLength; /* See length(). If there's no index, you can provide
        a length so that bitrate calculations can still be done. */
    bstream_t stream;
    brecord_t record; /* currently being recorded. */
    btuner_t tuner; /* currently being recorded. */
    int playRefCnt; /* how many PlayEngine's are playing this right now */

    bool isContinuousRecord; /* record into a fixed-size, circular file. this is used for timeshifting. */
    unsigned continuousRecordDuration; /* duration of the buffer in seconds */

    bool startPlaybackOfRecord;
    bool isBackgroundRecord;
    int ch;

    bool isNetRecord;
    char ipv4_addr[16];
    int port;

    bool isIP;
    bplayback_socket_params socket;
    bool timestamp_enabled;
    bool timestamp_override; /* allow user to override TS/TTS settings */
    void *dtcpIpCtx;

    /* dma: added for auto PIDs */
    bdecode_window_t    awindow[2];
    bdecode_t           adecode[2];
    bool                doAutoPids; /* auto PIDs flag. this flag tells playengine to do auto pids or not*/
    unsigned            decoderIndex;

#ifdef PVR_SUPPORT
    // thumbnails
    struct {
        bthumbnail_stream_create_settings create_settings;
        bthumbnail_stream_t stream;
        bthumbnail_manager_t manager;
        bfile_io_read_t index_bfile, data_bfile;
        FILE *index_stdiofile, *data_stdiofile;
    } thumbnail_data;
    void destroyThumbnails();
#endif

    /*
    Delete all files related to this video.
    Overrides Media::deleteFiles.
    */
    int deleteFiles();

    /*
    Returns length of video in milliseconds.
    If there's an index, this will be computed using indexToTime().
    Otherwise, you can set fixedLength, and this will be returned.
    fixedLength defaults to 0.

    Overrides Media::length().
    */
    int length(bplayback_t playback);
    int length();

    /**
    * Determines eIndexFormat based on indexfile name.
    */
    bindex_format indexFormat();

    /**
    * Computes last index from indexfile size.
    */
    int lastIndex();

    /*
    Summary:
    Converts index into milliseconds.
    Description:
    Everytime this function is called, it compares the index filesize and
    last index query to previous values. If either is different or this is the first time, it must
    open the index file, read the specified index, and returns the timestamp.
    Therefore this can be a costly function, especially when you're recording
    and the index filesize is changing all the time.

    When you're not recording, the cache is usually used to serve length(), so
    it's usually not so bad.
    */
    int indexToTime(int index);
#ifdef PVR_SUPPORT
    int createThumbnails(bthumbnail_manager_t manager);
#endif

protected:
    // overrides
    bool readNameValue(const char *name, const char *value);
    int writeNameValues(FILE *f);
    int writeNameValuesNetwork(char *f);
    // private
    void analyzeIndex();

    // cached value
    int64_t index_filesize;
    long last_indexquery, last_timestamp;
    MString _indexfilePath;
};

#endif
