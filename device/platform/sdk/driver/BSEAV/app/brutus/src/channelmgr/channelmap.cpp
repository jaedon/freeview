/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: channelmap.cpp $
 * $brcm_Revision: 42 $
 * $brcm_Date: 1/27/11 11:30a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/channelmap.cpp $
 * 
 * 42   1/27/11 11:30a mward
 * SW7125-726:  Add QAM Annex C.
 * 
 * 41   7/8/09 10:32a erickson
 * PR56373: fix ChannelMap::add so that it doesn't wipe out disabled
 * channels. disabled channels (e.g. INVALID entries in channels.txt) are
 * necessary to allow the user to control the absolute channel numbering.
 *
 * 40   6/23/09 6:16p katrep
 * PR55806: QAM128 support
 *
 * 39   5/22/09 2:15p erickson
 * PR49686: brutus channels.txt should support list of audio/video pids
 * per program
 *
 * 38   5/22/09 1:55p erickson
 * PR49686: fix indenting before attempting new feature
 *
 * 37   11/15/08 7:11a agin
 * PR48985: Support LDPC and Turbo at the same time.
 *
 * 36   3/10/08 4:36p vishk
 * PR 40148: 3510-B2 configuration for 97405 B0 platform. Changed the
 * QAM64 and QAM256 symbol rate to match accurately to that of the
 * standard.
 *
 * 35   2/1/08 4:59p katrep
 * PR39180: Added support for ofdm/DVB-T THD tuner
 *
 * 34   1/28/08 11:42a jtna
 * PR29193: Rework channeleditscreen behavior
 *
 * 33   11/12/07 10:25a ahulse
 * PR29654: Don't delete entries that use "PROGRAM_PIDS" when lose lock on
 * channel
 *
 * 32   6/19/07 3:19p erickson
 * PR32352: convert to bmpeg_type_pairs
 *
 * 31   5/3/07 2:16p mward
 * PR30301: add QAM16/32 support
 *
 * 30   2/2/07 11:37a rjlewis
 * PR27577: vxworks compiler says you can't jump over initializations.
 *
 * 29   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 28   11/30/06 6:15p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 27   11/2/06 11:12a arbisman
 * PR25227: Display IP channel's detais in status vindow
 *
 * 26   10/4/06 4:01p vsilyaev
 * PR 23826: Added RTSP support for AVC/AAC decoder over RTP
 *
 * 25   9/28/06 6:58p arbisman
 * PR24289: Back-out changes
 *
 * 23   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 *
 * 22   5/31/06 4:21p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/1   5/16/06 11:33a tokushig
 * merge from main
 *
 * 21   5/12/06 12:38p erickson
 * PR19790: added QPSK_TURBO
 *
 * 20   5/3/06 9:59a erickson
 * PR19790: added QPSK_LDPC support
 *
 * 18   10/24/05 3:28p dma
 * PR17733: auto pids for IP playback
 *
 * 17   10/21/05 3:05p dma
 * PR17733: auto channel/pids scan for IP playback
 *
 * 16   9/15/05 6:12p erickson
 * PR17108: update for more strict compiler
 *
 * 15   7/27/05 11:43a erickson
 * PR16296: remove restriction of BSETTOP_MAX_PROGRAMS on the number of
 * programs on a channel. This was an artificial restriction based on the
 * POD demo implementation for storing PMT's.
 *
 * 14   6/9/05 4:27p erickson
 * PR15514: when skipping disabled channels, don't skip subchannels if
 * you're already on the main channel
 *
 * 13   5/2/05 12:51p erickson
 * PR15138: detect subchannel even if no programs
 *
 * 12   4/26/05 3:54p erickson
 * PR15055: added HDMI input for 93560
 *
 * 11   4/19/05 3:46p erickson
 * PR14701: make digital subchannel support standard for all platforms
 *
 * 10   4/14/05 10:38a erickson
 * PR14593: print BAND if streamer id >= 100
 *
 * 9   4/12/05 11:48a erickson
 * PR14701: only use subchannel if there's digital programs, and make it 1
 * based
 *
 * 8   4/9/05 12:21a erickson
 * PR14701: added SUBCHANNEL_SUPPORT, which involves allowing a channel to
 * tune but not lock, then allowing scan to happen per-channel later
 *
 * 7   4/7/05 5:14p erickson
 * PR14701: added subchannel support
 *
 * 6   4/4/05 1:30p erickson
 * PR13823: removed B_HAS_XXXX frontend #defines. use bconfig for runtime
 * check if really needed.
 *
 * 5   3/29/05 10:34p erickson
 * PR14593: fixed Edit Channels for 3560
 *
 * 3   3/28/05 4:58p erickson
 * PR11759: added multiple linein support
 *
 * 2   2/24/05 1:21p erickson
 * PR14180: converted os specific to kni
 *
 * 1   2/7/05 7:58p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/58   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/57   1/4/05 9:48a erickson
 * PR13256: removed linefeed from record name
 *
 * Irvine_BSEAVSW_Devel/56   12/14/04 10:41a arbisman
 * PR13439: Implement "snprintf" for vxWorks
 *
 * Irvine_BSEAVSW_Devel/55   11/19/04 3:29p erickson
 * PR13256: added general purpose print function
 *
 * Irvine_BSEAVSW_Devel/54   11/9/04 7:08p vsilyaev
 * PR 13097: Added support for video stream type.
 *
 * Irvine_BSEAVSW_Devel/53   8/18/04 11:31a erickson
 * PR10832: updated default QAM symbol rate code
 *
 * Irvine_BSEAVSW_Devel/52   8/17/04 4:41p erickson
 * PR11770: removed brutus inputband support. added streamer as a
 * registered frontend resource, with a mapping between index number and
 * settop api id.
 *
 * Irvine_BSEAVSW_Devel/51   8/9/04 3:48p erickson
 * PR12104: extended SDS to handle DSS_PES and DSS_ES, along with STREAMER
 * and INPUTBAND
 *
 * Irvine_BSEAVSW_Devel/50   7/22/04 12:43p erickson
 * PR11682: fixed typo
 *
 * Irvine_BSEAVSW_Devel/49   7/22/04 12:40p erickson
 * PR11682: findNextChannel was mixing 1-based and 0-based logic. Brutus
 * covered up the mistake.
 *
 * Irvine_BSEAVSW_Devel/48   7/21/04 4:15p erickson
 * PR11682: improved brutus status msg
 *
 * Irvine_BSEAVSW_Devel/47   7/21/04 3:17p erickson
 * PR11682: added ChannelMapEntry constructor
 *
 * Irvine_BSEAVSW_Devel/46   7/21/04 12:10p erickson
 * PR11682: adapted to tspsimgr changes to better support PMT capture
 * without hack
 *
 * Irvine_BSEAVSW_Devel/45   7/21/04 10:09a erickson
 * PR11682: move channels.txt support out of ChannelMap and into
 * PSIChannelScan
 *
 * Irvine_BSEAVSW_Devel/44   7/20/04 11:56a erickson
 * PR11682: need PMT per program, not per entry (currently hardcoded to 10
 * as a hack)
 *
 * Irvine_BSEAVSW_Devel/43   7/15/04 5:17p erickson
 * PR11682: added PodChannelScan support
 *
 * Irvine_BSEAVSW_Devel/42   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 *
 * Irvine_BSEAVSW_Devel/41   5/28/04 8:45a erickson
 * PR11243: amodified STREAMER and STREAMER_DSS to support streamer
 * indexes, not just INPUTBAND synonym
 *
 * Irvine_BSEAVSW_Devel/40   5/14/04 12:00p erickson
 * PR10971: set right mpegtype for streamer
 *
 * Irvine_BSEAVSW_Devel/39   5/14/04 11:31a erickson
 * PR10971: set mpeg_type based on ChannelEntry
 *
 * Irvine_BSEAVSW_Devel/38   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/37   3/8/04 2:58p erickson
 * PR9883: it's not as pretty, but we can't use global buffers like this
 *
 * Irvine_BSEAVSW_Devel/36   3/4/04 1:39p erickson
 * PR9465: completed refactoring of channelmgr by moving channel string
 * generation into Brutus
 *
 * Irvine_BSEAVSW_Devel/35   3/2/04 5:26p erickson
 * PR9217: improved debug output on channel scan
 *
 * Irvine_BSEAVSW_Devel/34   2/13/04 6:10p erickson
 * PR9465: call init for vsb params
 *
 * Irvine_BSEAVSW_Devel/33   2/13/04 5:34p erickson
 * PR9465: VSB should support DEFAULT
 *
 * Irvine_BSEAVSW_Devel/32   2/13/04 4:19p erickson
 * PR9465: added VSB support to ui
 *
 * Irvine_BSEAVSW_Devel/31   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 *
 * Irvine_BSEAVSW_Devel/30   2/3/04 10:14a erickson
 * PR9497: inital vxworks port
 *
 * Irvine_BSEAVSW_Devel/29   1/21/04 3:59p erickson
 * PR9421: str->streamer.mpeg_type has to be set to bstream_mpeg_type_ts
 * for INPUTBAND entries
 *
 * Irvine_BSEAVSW_Devel/28   1/7/04 11:27a erickson
 * PR8731: Now that we're using a union, we can only initialize one
 * member, not all.
 *
 * Irvine_BSEAVSW_Devel/27   12/19/03 3:49p erickson
 * PR8731: added semicolon after BDBG_MODULE statement
 *
 * Irvine_BSEAVSW_Devel/26   12/17/03 3:44p erickson
 * PR8850: added error message for wrong PROGRAM PIDS line
 *
 * Irvine_BSEAVSW_Devel/25   12/16/03 12:25a erickson
 * PR8731: POD ui work
 *
 * Irvine_BSEAVSW_Devel/24   12/11/03 12:56p erickson
 * PR8868: it was writing NTSC/PAL in the wrong spot
 *
 * Irvine_BSEAVSW_Devel/23   11/25/03 4:03p erickson
 * PR8742: fixed 8PSK and symbol_rate
 *
 * Irvine_BSEAVSW_Devel/22   11/20/03 10:43a erickson
 * PR8583: fixed venom2 for new bvideo_format enum
 *
 *************************************************************/
#include "channelmap.h"
#include "bsettop_namevalue_pairs.h"
#include <stdio.h>
#include <assert.h>
#include <bstd.h>
#include <bkni.h>
#include <string.h>

BDBG_MODULE(channelmap);

ChannelMap::ChannelMap()
{
    BKNI_CreateMutex(&_mutex);
    _totalChannels = 0;
}

ChannelMap::~ChannelMap()
{
    BKNI_DestroyMutex(_mutex);
}

void ChannelMap::clear()
{
    BKNI_AcquireMutex(_mutex);
    _entries.clear();
    _totalChannels = 0;
    BKNI_ReleaseMutex(_mutex);
}

void ChannelMap::setGotAutoPids(int chnum, bool ok)
/* this method set the gotAutoPids flag.
    If ok=true, we've acquired PIDs info automatically.
*/
{
    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[MAINCH(chnum)];
    if (sw) {
        sw->entry.ip.gotAutoPids = ok;
    }
    BKNI_ReleaseMutex(_mutex);
}

int ChannelMap::getChannel(int chnum, Channel *channel) const
{
    int retValue = -1;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[MAINCH(chnum)];
    if (sw) {
        channel->chnum = chnum;
        channel->entry = sw->entry;     //copy
        retValue = 0;   // success as entry is found
        Program *p = sw->programs[SUBCH(chnum)];
        if (p) {
            channel->program = p->info;     //copy
            channel->validProgramInfo = true;
        }
        else {
            channel->validProgramInfo = false;
        }
    }
    BKNI_ReleaseMutex(_mutex);
    return retValue;
}

const void *ChannelMap::getPMT(int chnum) const
{
    const void *retValue = NULL;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[MAINCH(chnum)];
    if (sw) {
        Program *p = sw->programs[SUBCH(chnum)];
        retValue = p?p->pmt:NULL;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

int ChannelMap::findNextChannel(int chnum, int dir) const
{
    int orgchnum = chnum;
    int nextChannel = -1;

    BKNI_AcquireMutex(_mutex);
    do {
        int mainch = MAINCH(chnum);
        int subch = SUBCH(chnum);
        Wrapper *sw;

        while (1) {
            sw = _entries[mainch];
            if (sw) break;

            /* we have to change main channels */
            dir = 0;
            subch = 0;
            if (--mainch < 0)
                goto out;
        }

        subch += dir;

        if (subch < 0) {
            mainch--;
            if (mainch < 0)
                mainch = _entries.total() - 1;
            sw = _entries[mainch];
            subch = sw->programs.total() - 1;
            if (subch < 0)
                subch = 0; /* no subchannels, so take mainch by itself */
        }
        else if (subch >= sw->programs.total()) {
            subch = 0;
            mainch++;
            if (mainch == _entries.total())
                mainch = 0;
        }
        chnum = MAKESUBCH(mainch, subch);

        /* now test the channel */
        /* need to release the lock & reacquire it as getChannel also gets this lock */
        BKNI_ReleaseMutex(_mutex);
        Channel ch;
        if (getChannel(chnum, &ch))
            return -1; // internal error
        BKNI_AcquireMutex(_mutex);

        /* If a channel is disabled, we skip over it when doing up/down operations.
        You can always jump to a disabled channel by keying in the number. Because
        of this, we have to channel up/down on subchannels of a disabled channel
        once we've arrived there. */
        if (!ch.entry.disabled || MAINCH(orgchnum) == MAINCH(chnum)) {
            nextChannel = chnum;
            goto out;
        }
    } while (chnum != orgchnum);

out:
    BKNI_ReleaseMutex(_mutex);
    return chnum;
}

void ChannelMap::insert(int index, ChannelMapEntry *str)
{
    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = new Wrapper(str);
    _totalChannels++;
    _entries.insert(index, sw);
    BKNI_ReleaseMutex(_mutex);
}

void ChannelMap::add(ChannelMapEntry *entry)
{
    /* add() is just a short-cut for inserting at the end */
    insert(-1, entry);
}

bool ChannelMap::deleteEntry(int index)
{
    bool retValue = false;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[index];
    if (sw) {
        /* marked entry as deleted (==2. see comment after definition of .disabled) */
        sw->entry.disabled = 2;
        /* clear all the associated program Infos */
        sw->programs.clear();
        retValue = true;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

bool ChannelMap::deleteEntry(bchannel_entry_sub_type subType, int sourceChNum)
{
    bool retValue = false;
    Wrapper *sw;
    BKNI_AcquireMutex(_mutex);
    MListItr<Wrapper> itr(&_entries);
    for (sw = itr.first(); sw; sw = itr.next()) {
        if (sw->entry.getSubType() == subType &&
            sw->entry.sourceChNum == sourceChNum)
        {
            /* marked entry as deleted (==2. see comment after definition of .disabled) */
            sw->entry.disabled = 2;
            /* clear all the associated program Infos */
            sw->programs.clear();
            retValue = true;
            break;
        }
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

bool ChannelMap::updateEntry(int index, ChannelMapEntry *entry)
{
    bool retValue = false;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[index];
    if (sw) {
        sw->entry = *entry;
        retValue = true;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

bool ChannelMap::rearrange(int index, int inc)
{
    bool retValue = false;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries.remove(index);
    if (sw) {
        _entries.insert(index+inc, sw);
        retValue = true;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

/* returns number of digital programs. if analog, 0. */
int ChannelMap::totalPrograms(int s) const
{
    int retValue;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[s];
    retValue = sw ? sw->programs.total() : -1;
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

/* returns number of channels. for digital, this is the number of programs.
for analog, it's 1. this is only used internally. */
int ChannelMap::channels(const Wrapper *sw) const
{
    if (sw->entry.getType() != btuner_analog_stream)
        return sw->programs.total();
    else
        return 1; // analog means 1
}

bool ChannelMap::getProgram(int s, int index, PROGRAM_INFO_T *program)
{
    bool retValue = false;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[s];
    if (sw) {
        Program *p = sw->programs[index];
        if (p) {
            *program = p->info; //copy
            retValue = true;
        }
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

/* static */
void ChannelMap::printProgram(const PROGRAM_INFO_T *program)
{
    int i;
    printf("Program %d\n", program->program_number);
    printf("  pcr %x\n", program->pcr_pid);
    printf("  ca %x\n", program->ca_pid);
    for (i=0;i<program->num_video_pids;i++)
        printf("  video %d: %x, ca %x\n",
        i, program->video_pids[i].pid, program->video_pids[i].ca_pid);
    for (i=0;i<program->num_audio_pids;i++)
        printf("  audio %d: %x, ca %x\n",
        i, program->audio_pids[i].pid, program->audio_pids[i].ca_pid);
}

bool ChannelMap::addProgram(int s, const PROGRAM_INFO_T *program,
    const void *pmt, bool scanned)
{
    bool retValue = false;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[s];
    if (sw) {
        Program *p = new Program(); // will be deleted by MAutoList
        p->info = *program; // copy it
        p->pmt = pmt; // TODO: this assumes the memory will not be deallocated. see header file for notes.
        sw->programs.add(p);
        sw->noScan = !scanned; // if one program is "not scanned", then the whole entry is noScan
        retValue = true;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

bool ChannelMap::clearPrograms(int entryIndex)
{
    bool retValue = false;

    BKNI_AcquireMutex(_mutex);
    if (entryIndex != -1) {
        /* clear one */
        Wrapper *sw = _entries[entryIndex];
        if (sw && ! sw->noScan ) {
            /* noScan is set if we had a "PROGRAM_PIDS" field, don't clear these entries */
            sw->programs.clear();
            /* dma: clear this flag so that we can to auto pids the next time we
            tune to this IP channel.
            */
            sw->entry.ip.gotAutoPids = false;
            retValue = true;
        }
    }
    else {
        /* clear all */
        for (int i=0;i<total();i++) {
            /* don't clear fixed pid streams */
            BKNI_ReleaseMutex(_mutex); //release & reacquire lock
            if (!isNoScanChannelMapEntry(i))
                clearPrograms(i);
            BKNI_AcquireMutex(_mutex);
        }
        retValue = true;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

/* static */
void ChannelMap::setProgram(PROGRAM_INFO_T *program,
    unsigned short pcrpid, unsigned short vidpid, unsigned short audpid, unsigned short audtype,unsigned short vidtype)
{
    BKNI_Memset(program, 0, sizeof(*program));
    program->pcr_pid = pcrpid;
    program->num_video_pids = 1;
    program->video_pids[0].pid = vidpid;
    program->video_pids[0].streamType = vidtype;
    program->num_audio_pids = 1;
    program->audio_pids[0].pid = audpid;
    program->audio_pids[0].streamType = audtype;
}

void ChannelMap::setProgram(PROGRAM_INFO_T *program, const bstream_mpeg *mpeg)
{
    unsigned i;
    BKNI_Memset(program, 0, sizeof(*program));
    program->pcr_pid = mpeg->pcr_pid;
    for (i=0;mpeg->video[i].pid && i<BSETTOP_MAX_PROGRAMS && i<MAX_PROGRAM_MAP_PIDS;i++) {
        program->num_video_pids++;
        program->video_pids[i].pid = mpeg->video[i].pid;
        program->video_pids[i].streamType = mpeg->video[i].format;
    }
    for (i=0;mpeg->audio[i].pid && i<BSETTOP_MAX_PROGRAMS && i<MAX_PROGRAM_MAP_PIDS;i++) {
        program->num_audio_pids++;
        program->audio_pids[i].pid = mpeg->audio[i].pid;
        program->audio_pids[i].streamType = mpeg->audio[i].format;
    }
}


ChannelMap::ChannelMapEntry::ChannelMapEntry()
{
    disabled = 0;
    freq = 0.0;
    pod = false;
    sourceChNum = -1;

    if (bconfig->features.sds) {
        setType(btuner_sds_stream);
    }
    else if (bconfig->features.qam) {
        setType(btuner_qam_stream);
    }
    else if (bconfig->features.vsb) {
        setType(btuner_vsb_stream);
    }
    else if (bconfig->features.ofdm) {
        setType(btuner_ofdm_stream);
    }
    else {
        setType(btuner_analog_stream);
    }

    /* default: set ch sub type to be static */
    setSubType(bchannel_entry_static);
}

void ChannelMap::ChannelMapEntry::setType(btuner_stream_type type)
{
    _type = type;
    switch (type) {
    case btuner_sds_stream:
        btuner_sds_params_init(&sds.params, NULL);
        sds.mpeg_type = bstream_mpeg_type_ts;
        break;
    case btuner_qam_stream:
        btuner_qam_params_init(&qam, NULL);
        break;
    case btuner_vsb_stream:
        btuner_vsb_params_init(&vsb, NULL);
        break;
    case btuner_ofdm_stream:
        btuner_ofdm_params_init(&ofdm, NULL);
        break;
    case btuner_analog_stream:
        btuner_analog_params_init(&analog.params, NULL);
        analog.linein = -1;
        break;
    case btuner_streamer_stream:
        streamer.index = 0;
        streamer.mpeg_type = bstream_mpeg_type_ts;
        break;
    }
}

void ChannelMap::ChannelMapEntry::setSubType(bchannel_entry_sub_type subType)
{
    _subType = subType;
}

bool ChannelMap::getEntry(int index, ChannelMap::ChannelMapEntry *entry) const
{
    bool retValue = false;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[index];
    if (sw) {
        *entry = sw->entry; //copy entry
        retValue = true;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

#if 0
// we no longer provide direct access to the channel map entries.
int ChannelMap::entryIndex(ChannelMapEntry *entry) const
{
    int retValue = -1;

    BKNI_AcquireMutex(_mutex);
    MListItr<Wrapper> itr(&_entries);
    int i = 0;
    for (Wrapper *sw = itr.first(); sw; sw = itr.next()) {
        if (&sw->entry == entry) {
            retValue = i;
            break;
        }
        i++;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}
#endif

void ChannelMap::printChannels() const
{
    BKNI_AcquireMutex(_mutex);
    int ch = 1;
    MListItr<Wrapper> itr(&_entries);
    for (Wrapper *sw = itr.first(); sw; sw = itr.next()) {
        for (int i=0;i<sw->programs.total();i++) {
            switch (sw->entry.getType()) {
            case btuner_qam_stream:
                printf("%d: qam %f\n", ch++, sw->entry.freq);
                break;
            case btuner_sds_stream:
                printf("%d: sds %f\n", ch++, sw->entry.freq);
                break;
            case btuner_vsb_stream:
                printf("%d: vsb %f\n", ch++, sw->entry.freq);
                break;
            case btuner_ofdm_stream:
                printf("%d: ofdm %f\n", ch++, sw->entry.freq);
                break;
            case btuner_streamer_stream:
                printf("%d: streamer %d\n", ch++, sw->entry.streamer.index);
                break;
            case btuner_analog_stream:
                printf("%d: analog, %s\n", ch++, btuner_analog_str(&sw->entry));
                break;
            default: break;
            }
        }
    }
    BKNI_ReleaseMutex(_mutex);
}

bool ChannelMap::isNoScanChannelMapEntry(int stream) const
{
    bool retValue = false;

    BKNI_AcquireMutex(_mutex);
    Wrapper *sw = _entries[stream];
    if (sw) {
        retValue = sw->noScan;
    }
    BKNI_ReleaseMutex(_mutex);
    return (retValue);
}

const char *btuner_sds_str(btuner_sds_mode mode, bstream_mpeg_type mpeg_type)
{
    switch (mode) {
    case btuner_sds_mode_qpsk_dvb: return "QPSK_DVB";
    case btuner_sds_mode_qpsk_dss:
        if (mpeg_type == bstream_mpeg_type_dss_es)
            return "QPSK_DSS";
        else
            return "QPSK_DSS_PES";
    case btuner_sds_mode_qpsk_turbo: return "QPSK_TURBO";
    case btuner_sds_mode_8psk_turbo: return "8PSK_TURBO";
    case btuner_sds_mode_turbo: return "TURBO";
    case btuner_sds_mode_qpsk_ldpc: return "QPSK_LDPC";
    case btuner_sds_mode_8psk_ldpc: return "8PSK_LDPC";
    case btuner_sds_mode_ldpc: return "LDPC";
    case btuner_sds_mode_qpsk_dcii: return "DCII";
    default: break;
    }
    assert(false);
    return NULL;
}

const char *btuner_qam_str(btuner_qam_mode mode)
{
    switch (mode) {
    case 16: return "QAM16";
    case 32: return "QAM32";
    case 64: return "QAM64";
    case 128: return "QAM128";
    case 256: return "QAM256";
    case 1024: return "QAM1024";
    default: break;
    }
    assert(false);
    return NULL;
}

const char *btuner_vsb_str(btuner_vsb_mode mode)
{
    switch (mode) {
    case 8: return "VSB8";
    case 16: return "VSB16";
    default: break;
    }
    assert(false);
    return NULL;
}

const char *btuner_ofdm_str()
{
    return "OFDM";
}

bstream_mpeg_type ChannelMap::ChannelMapEntry::mpegType() const {
    switch (_type) {
    case btuner_sds_stream:
        return sds.mpeg_type;
    case btuner_streamer_stream:
        return streamer.mpeg_type;
    default:
        return bstream_mpeg_type_ts;
    }
}

const char *ChannelMap::ChannelMapEntry::typeStr() const {
    switch (_type) {
    case btuner_streamer_stream:
        /* Can't use static char buffer because of dual output configuration */
        return "Streamer";
    case btuner_vsb_stream:
        return btuner_vsb_str(vsb.mode);
    case btuner_ofdm_stream:
        return btuner_ofdm_str();
    case btuner_sds_stream:
        return btuner_sds_str(sds.params.mode, sds.mpeg_type);
    case btuner_qam_stream:
        return btuner_qam_str(qam.mode);
    case btuner_analog_stream:
        return btuner_analog_str(this);
    default:
        return NULL;
    }
}

bool ChannelMap::isDigital(int chnum) const {
    ChannelMap::Channel channel;
    return !getChannel(chnum, &channel) &&
        channel.entry.getType() != btuner_analog_stream;
}

bool ChannelMap::isLineIn(int chnum) const {
    ChannelMap::Channel channel;
    return !getChannel(chnum, &channel) &&
        channel.entry.getType() == btuner_analog_stream &&
        channel.entry.analog.linein >= 0;
}

const char *bstream_mpeg_type_str(bstream_mpeg_type mpeg_type)
{
    const char *name;
    if (!b_get_name(bmpeg_type_pairs, mpeg_type, &name))
        return name;
    return "TS";
}

int b_get_default_qam_symrate(btuner_qam_annex qam_annex, btuner_qam_mode qam_mode)
{
    if (qam_annex == btuner_qam_annex_a)
        return 6952000; /* 64 or 256 */
    if (qam_annex == btuner_qam_annex_c)
        return 5274000 ; /* 64 or 256 */
    else
        return (qam_mode == 64)?5056941:5360537;
    /* TODO: QAM1024 default? */
}

int b_print_chnum(char *ptr, int length,
    const ChannelMap::Channel *channel)
{
    if (channel->entry.getType() != btuner_analog_stream &&
        (channel->validProgramInfo || channel->chnum > 65535))
    {
        return BKNI_Snprintf(ptr, length, "Ch %d.%d",
            MAINCH(channel->chnum),SUBCH(channel->chnum)+1);
    }
    return BKNI_Snprintf(ptr, length, "Ch %d", channel->chnum);
}

int b_print_channel_description(char *ptr, int length,
    ChannelMap::ChannelMapEntry *entry)
{
    bool analog = entry->getType() == btuner_analog_stream;
    bool linein = analog && entry->analog.linein >= 0;
    bool streamer = (entry->getType() == btuner_streamer_stream);
    int n = 0;

    if (linein) {
        n += BKNI_Snprintf(ptr, length, "%s",
            entry->typeStr());
    }
    else if (streamer) {
        if (entry->streamer.index >= 100) {
            n += BKNI_Snprintf(ptr, length, "BAND %d (%s)",
                entry->streamer.index - 100,
                bstream_mpeg_type_str(entry->streamer.mpeg_type));
        }
        else {
            n += BKNI_Snprintf(ptr, length, "%s %d (%s)",
                entry->typeStr(),
                entry->streamer.index,
                bstream_mpeg_type_str(entry->streamer.mpeg_type));
        }
    }
    else if(entry->getType() == btuner_ip_stream) {
        if(entry->ip.params.open_params.protocol == bsettop_ip_protocol_rtsp) {
            const char *name = strrchr(entry->ip.params.open_params.url, '/');
            if(name==NULL) {
                name = entry->ip.params.open_params.url;
                }
                n += BKNI_Snprintf(ptr, length, "%s", name);
        }
        else
            n += BKNI_Snprintf(ptr, length, "%s : %d",
                entry->ip.params.open_params.ip_addr,
                entry->ip.params.open_params.port);
    }
#if B_HAS_RTSP
    else if (entry->getType() == btuner_rtsp_stream) {
    const char *name = strrchr(entry->rtsp.url, '/');
    if(name==NULL) {
        name = entry->rtsp.url;
        }
        n += BKNI_Snprintf(ptr, length, "%s", name);
    }
#endif
    else {
        n += BKNI_Snprintf(ptr, length, "%s, %0.2f MHz",
            entry->typeStr(),
            entry->freq);
    }
    return n;
}

/* note: this list is replicated in 'editchannelsscreen.cpp' due to a vxworks compile problem */
const char *b_lineinput_str[] = {
#if BCHP_CHIP == 7038
    "COMPOSITE",
    "SVIDEO",
    "COMPONENT"
#elif BCHP_CHIP == 3560
    "COMPOSITE1",
    "COMPOSITE2",
    "COMPOSITE3",
    "COMPOSITE4",
    "SVIDEO1",
    "SVIDEO2",
    "COMPONENT",
    "HDMI"
#else
    "LINEINPUT"
#endif
};

const char *btuner_analog_str(const ChannelMap::ChannelMapEntry *entry)
{
    if (entry->getType() != btuner_analog_stream)
        return NULL;

    if (entry->analog.linein == -1)
        return "ANALOG";

    return b_lineinput_str[entry->analog.linein];
}

ChannelMap::Channel::Channel()
{
    chnum = -1;
    validProgramInfo = false;
        memset(&entry, sizeof(ChannelMap::ChannelMapEntry), 0);
        memset(&program, sizeof(PROGRAM_INFO_T), 0);
}

