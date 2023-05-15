/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: recordengine.cpp $
 * $brcm_Revision: 22 $
 * $brcm_Date: 9/8/11 9:40a $
 *
 * Module Description: C++ wrapper around playback.c and record.c
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/recordengine.cpp $
 * 
 * 22   9/8/11 9:40a erickson
 * SW7358-113: add AVS indexing support
 * 
 * 21   5/13/11 2:21p erickson
 * SW7550-715: only index MPEG and AVC
 * 
 * 20   8/30/10 11:52a erickson
 * SW7550-551: clear record overflow
 * 
 * 19   6/1/09 7:54p jtna
 * PR43001: added record-related brutus script commands
 * 
 * 18   5/11/09 4:40p ssood
 * PR54955: Record TTS streams over HTTP: set flag in socket param to
 * indicate a TTS stream
 * 
 * 17   3/6/08 2:19p jrubio
 * PR36248: add bstream_mpeg to network open call
 * 
 * PR37038/1   12/11/07 2:51p erickson
 * PR37038: convert continuous record to use beginning_of_stream callbacks
 *
 * 15   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 14   9/10/07 4:15p ahulse
 * PR28979: Have control poll recordengine for record error condition
 *
 * 13   9/6/07 10:49a jrubio
 * PR33904: fix typo
 *
 *
 * 11   8/26/07 10:54a jrubio
 * PR33904: added needed changes for netrecord
 *
 * 10   8/9/07 5:41p jrubio
 * PR33830:adding Network Record changes
 *
 * 9   6/22/07 1:50p erickson
 * PR30701: clear _video on stop
 *
 * 8   2/2/07 9:52a erickson
 * PR26798: call brecpump_get_open_params so CDB/ITB sizes can be reported
 * to debug interface
 *
 * 7   12/19/06 10:28a erickson
 * PR26543: removing private include files. no harm seen. must have been a
 * temp hack.
 *
 * 6   4/14/06 1:59p jgarrett
 * PR 20139: Updating include path
 *
 * 5   4/14/06 12:24p mphillip
 * PR20797: Scale file sizes based on record interval
 *
 * 4   3/23/06 5:20p mphillip
 * PR19786: Fixes for HDSD_SIMUL and DUAL_OUTPUT modes
 *
 * 3   3/23/06 3:10p mphillip
 * PR19786: Fix a segfault, increase the default size to accomodate 5
 * minutes of history
 *
 * 2   3/15/06 4:04p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 1   2/7/05 7:54p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/22   2/2/05 10:09a erickson
 * PR14006: call file close
 *
 * Irvine_BSEAVSW_Devel/21   1/31/05 9:53a erickson
 * PR13351: must close recpump too
 *
 * Irvine_BSEAVSW_Devel/20   11/23/04 8:32p vsilyaev
 * PR 13351: Refactoring pvr  API.
 *
 * Irvine_BSEAVSW_Devel/19   11/19/04 3:31p erickson
 * PR13199: added special return code for bad record file i/o
 *
 * Irvine_BSEAVSW_Devel/18   8/20/04 11:30a erickson
 * PR12349: handle overflow more gracefully
 *
 * Irvine_BSEAVSW_Devel/17   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/16   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/15   10/31/03 11:58a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/14   10/28/03 11:25a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/13   10/15/03 11:26a erickson
 * changed isHits to getStatus
 *
 * Irvine_BSEAVSW_Devel/12   10/9/03 2:34p erickson
 * fixed uclibc support
 *
 * Irvine_BSEAVSW_Devel/11   9/17/03 12:32p erickson
 * converted from original debug interface to magnum debug interface
 *
 * Irvine_BSEAVSW_Devel/10   9/5/03 3:27p erickson
 * adapted to new settop api pvr api
 *
 * Irvine_BSEAVSW_Devel/9   8/29/03 3:14p erickson
 * fixed file_open using MString or NULL
 *
 * Irvine_BSEAVSW_Devel/8   8/27/03 4:39p erickson
 * support no index record
 *
 * Irvine_BSEAVSW_Devel/7   8/22/03 5:21p erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/6   8/21/03 9:43a erickson
 * venom2 settop api work
 *
 ***************************************************************************/
#include "recordengine.h"
#include "cfgsettings.h"
#include "bsettop.h"
#include "bstd.h"
#include "bsettop_fileio_fifo.h"

BDBG_MODULE(brutus_recordengine);

void RecordEngine::error_callback(void *context, brecord_error reason)
{
    ((RecordEngine*)context)->_error = reason;
    ((RecordEngine*)context)->_hasError = true; /* must be last */
}

void RecordEngine::overflow_callback(void *context, bool *stop)
{
    *stop = false; /* keep going, but the GUI will pop up a box */
    ((RecordEngine*)context)->_overflow = true;
    ((RecordEngine*)context)->_hasError = true; /* must be last */
}

RecordEngine::RecordEngine(const char *aname, int index)
{
    brecpump_open_params recpump_open_params;
    _name = aname;
    _recording = false;
    _hasError = false;
    _modalDisplayed = false;
    _index = index;
    _video = NULL;
    _fifo_file = NULL;
    unsigned long defaultEncryptionKey[4] = DEFAULT_ENCRYPTION_KEY;
    
    setEncryptionKey(defaultEncryptionKey);

    brecpump_get_open_params(B_ID(index), &recpump_open_params);
    BDBG_MSG(("record %d: data %d K, index %d K",
        index,
        recpump_open_params.data.buffer_size/1024,
        recpump_open_params.scode.buffer_size/1024));
    _recpump = brecpump_open(B_ID(index), &recpump_open_params);

    _record = brecord_open();
    if (!_record || !_recpump)
        BDBG_ERR(("record_init for %s, index %d", name(), index));
}

RecordEngine::~RecordEngine() {
    if (_record)
        brecord_close(_record);
    if (_recpump)
        brecpump_close(_recpump);
}

int RecordEngine::start(Video *video)
{
    if (!_record)
        return -1;

    if (isActive()) {
        if (stop()) return -1;
    }

    _hasError = false;
    _video = video;

    if (_video->mpeg.encryption.type != bencryption_type_none) {
        memcpy(video->mpeg.encryption.key, _key, sizeof(_key));
    }
    
    /* we only index MPEG, AVC and AVS */
    switch (_video->mpeg.video[0].format) {
    case bvideo_codec_mpeg2:
    case bvideo_codec_h264:
    case bvideo_codec_avs:
        break;
    default:
        video->indexfile = NULL;
        break;        
    }

    if (_video->isContinuousRecord) {
        bfile_out_fifo_t ff = NULL;

        ff = bfile_fifo_out_create(video->mediafilePath(),
            video->indexFormat()?(const char *)video->indexfilePath():NULL);

        if (ff) {
/* alignment macro taken from bsettop_fileio_fifo.c to align the data file for both direct I/O and 188 byte packets */
#define B_PVRDATA_ALIGN  ((188/4)*4096)
            bfile_out_settings settings;
            bfile_fifo_out_get(ff,&settings);
            if (_video->continuousRecordDuration < 5) {
                BDBG_ERR(("Extremely short continuousRecordDuration of %d seconds", _video->continuousRecordDuration));
            }

            /* see api/include/bsettop_fileio_fifo.h for rationale for these values */
            settings.interval = _video->continuousRecordDuration;
            settings.data.soft = ((25 * 1048576)/8) * settings.interval; /* Treat a 25 mbps stream as the worst-case scenario, and extrapolate the file size
                (stream bandwidth, converted to bytes, multiplied by time) */
            settings.data.hard = settings.data.soft * 2;
            settings.data.soft = B_PVRDATA_ALIGN*(settings.data.soft/B_PVRDATA_ALIGN);
            settings.data.hard = B_PVRDATA_ALIGN*(settings.data.hard/B_PVRDATA_ALIGN);
            settings.index.soft = settings.interval * 60 * 64 * 2; /* assuming 60 pictures/second, 64 bytes per NAV entry, plus pad */
            settings.index.hard = settings.index.soft * 2;
            bfile_fifo_out_set(ff,&settings);

            _file = bfile_fifo_out_file(ff);
            _fifo_file = (void *)ff;
        } else
            _file = NULL;
    }
    else if (_video->isNetRecord) {
        /* I must get all the info from brutus config if IP and Port are not set
           then we cannot continue! Only IPV4 is supported right now,
           I must extract the string address from IPSERVER_IPV4 and fill in
           the unint8_t host[4] array of bsocket_params ipv4 struct*/
        bsocket_params params;
        int i=0,start_j=0, end_j=0;
        char * p_begin;
        char * p_end;
        char * temp;

        bsocket_params_init(&params);
        params.addr_type = bsocket_address_type_ipv4;
        p_begin = _video->ipv4_addr;
        params.addr.ipv4.port = _video->port;
        for (i=0;i<4;i++)
        {
            p_end = strstr(p_begin,".");
            if(!p_end)
            {
                if(i==3 && (strlen(p_begin) !=0))
                { /* There is still one last value*/
                    params.addr.ipv4.host[i]= atoi(p_begin);
                    continue;
                }
                break;
            }
            temp = p_end;
            temp++;
            *p_end = 0;
            params.addr.ipv4.host[i]= atoi(p_begin);
            p_begin = temp;
        }
        params.timestamp_enabled = video->timestamp_enabled;
        _file = brecord_file_open_network(video->mediafile,
            video->indexFormat()?(const char *)video->getIndexFileName():NULL, &params, &_video->mpeg);
    }
    else {
        _file = brecord_file_open(video->mediafilePath(),
            video->indexFormat()?(const char *)video->indexfilePath():NULL);
    }
    if (!_file)
        return -5; /* see Control::recordVideo() for final presentation of this */
        
    brecord_params params;
    brecord_params_init(&params, _record);
    params.index_format = video->indexFormat();
    params.callback_context = (void *)this;
    params.error = error_callback;
    params.overflow = overflow_callback;
    params.encryption = video->mpeg.encryption;
    params.timestamp_enabled = video->timestamp_enabled;

    if (!brecord_start(_record, _recpump, video->stream, _file, &params)) {
        _recording = true;
        FIRE(StateChanged);
        return 0;
    }
    return -1;
}

int RecordEngine::stop() {
    if (!isActive())
        return 0;

    int rc = brecord_stop(_record);
    brecord_file_close(_file);
    _file = NULL;
    _fifo_file = NULL;
    _recording = false;
    _video = NULL;
    FIRE(StateChanged);
    return rc;
}

void RecordEngine::setModalState( bool displayed ) {
    _modalDisplayed = displayed;
}

bool RecordEngine::getModalState( void ) {
    return _modalDisplayed;
}

void RecordEngine::getStatus(brecord_status *status) const {
    brecord_get_status(_record, status);
}

void RecordEngine::setEncryptionKey(unsigned long key[4]) {
    memcpy(_key, key, sizeof(_key));
}

FIRE_EV_METHOD(RecordEngine, StateChanged)

bool RecordEngine::getError(brecord_error *error, bool *overflow) const
{
    if (_hasError) {
        *error = _error;
        *overflow = _overflow;
    }
    return _hasError;
}

void RecordEngine::clearOverflowError()
{
	_overflow = false;
	_hasError = false;
}

