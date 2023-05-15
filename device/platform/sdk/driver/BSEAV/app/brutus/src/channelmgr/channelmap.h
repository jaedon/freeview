/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: channelmap.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 4/12/10 7:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/channelmap.h $
 * 
 * 23   4/12/10 7:33p nitinb
 * SW7550-339: Enable "channel edit" functionality in Brutus for OFDM
 * input
 * 
 * 22   5/22/09 2:15p erickson
 * PR49686: brutus channels.txt should support list of audio/video pids
 * per program
 *
 * 21   2/1/08 4:59p katrep
 * PR39180: Added support for ofdm/DVB-T THD tuner
 *
 * 20   1/28/08 11:40a jtna
 * PR29193: Rework channeleditscreen behavior
 *
 * 19   6/11/07 1:04p jjordan
 * PR32034: Add support for TTS streams in IP channels
 *
 * 18   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 17   11/30/06 6:15p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 16   10/4/06 4:01p vsilyaev
 * PR 23826: Added RTSP support for AVC/AAC decoder over RTP
 *
 * 15   9/28/06 6:58p arbisman
 * PR24289: Back-out changes
 *
 * 13   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 *
 * 12   10/21/05 3:06p dma
 * PR17733: auto channel/pids scan for IP playback
 *
 * 11   8/24/05 6:11p arbisman
 * PR16678: Add support for WinCE
 *
 * 10   7/27/05 11:43a erickson
 * PR16296: remove restriction of BSETTOP_MAX_PROGRAMS on the number of
 * programs on a channel. This was an artificial restriction based on the
 * POD demo implementation for storing PMT's.
 *
 * 9   6/29/05 9:05a jjordan
 * PR16042: Add support for "IP channels"
 *
 * 8   4/19/05 3:46p erickson
 * PR14701: make digital subchannel support standard for all platforms
 *
 * 7   4/9/05 12:21a erickson
 * PR14701: added SUBCHANNEL_SUPPORT, which involves allowing a channel to
 * tune but not lock, then allowing scan to happen per-channel later
 *
 * 6   4/7/05 5:14p erickson
 * PR14701: added subchannel support
 *
 * 5   4/4/05 1:30p erickson
 * PR13823: removed B_HAS_XXXX frontend #defines. use bconfig for runtime
 * check if really needed.
 *
 * 4   3/29/05 10:34p erickson
 * PR14593: fixed Edit Channels for 3560
 *
 * 3   3/28/05 4:58p erickson
 * PR11759: added multiple linein support
 *
 * 2   3/24/05 10:44a erickson
 * PR14593: 93560 merge
 *
 * Refsw_93560_Bringup/1   3/23/05 10:32p vsilyaev
 * Fixed for 3560
 *
 * 1   2/7/05 7:58p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/43   12/14/04 7:26p bandrews
 * PR13158: 7318 doesn't have SDS but is a 73XX family member
 *
 * Irvine_BSEAVSW_Devel/42   11/19/04 3:29p erickson
 * PR13256: added general purpose print function
 *
 * Irvine_BSEAVSW_Devel/41   11/9/04 7:07p vsilyaev
 * PR 13097: Added support for video stream type.
 *
 * Irvine_BSEAVSW_Devel/40   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 *
 * Irvine_BSEAVSW_Devel/39   8/18/04 11:31a erickson
 * PR10832: updated default QAM symbol rate code
 *
 * Irvine_BSEAVSW_Devel/38   8/17/04 4:41p erickson
 * PR11770: removed brutus inputband support. added streamer as a
 * registered frontend resource, with a mapping between index number and
 * settop api id.
 *
 * Irvine_BSEAVSW_Devel/37   8/9/04 3:48p erickson
 * PR12104: extended SDS to handle DSS_PES and DSS_ES, along with STREAMER
 * and INPUTBAND
 *
 * Irvine_BSEAVSW_Devel/36   7/21/04 4:15p erickson
 * PR11682: improved brutus status msg
 *
 * Irvine_BSEAVSW_Devel/35   7/21/04 3:17p erickson
 * PR11682: added ChannelMapEntry constructor
 *
 * Irvine_BSEAVSW_Devel/34   7/21/04 12:10p erickson
 * PR11682: adapted to tspsimgr changes to better support PMT capture
 * without hack
 *
 * Irvine_BSEAVSW_Devel/33   7/21/04 10:09a erickson
 * PR11682: move channels.txt support out of ChannelMap and into
 * PSIChannelScan
 *
 * Irvine_BSEAVSW_Devel/32   7/20/04 4:55p erickson
 * PR11682: restoring PMT_BUFFER_T until POD is released
 *
 * Irvine_BSEAVSW_Devel/31   7/20/04 11:56a erickson
 * PR11682: need PMT per program, not per entry (currently hardcoded to 10
 * as a hack)
 *
 * Irvine_BSEAVSW_Devel/30   7/19/04 11:45a erickson
 * PR11682: give actual field names
 *
 * Irvine_BSEAVSW_Devel/29   7/19/04 11:34a erickson
 * PR11682: temp definition for PMT_SUPPORT_T
 *
 * Irvine_BSEAVSW_Devel/28   7/15/04 5:17p erickson
 * PR11682: added PodChannelScan support
 *
 * Irvine_BSEAVSW_Devel/27   5/28/04 8:45a erickson
 * PR11243: amodified STREAMER and STREAMER_DSS to support streamer
 * indexes, not just INPUTBAND synonym
 *
 * Irvine_BSEAVSW_Devel/26   5/14/04 11:31a erickson
 * PR10971: set mpeg_type based on ChannelEntry
 *
 * Irvine_BSEAVSW_Devel/25   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/24   3/4/04 1:39p erickson
 * PR9465: completed refactoring of channelmgr by moving channel string
 * generation into Brutus
 *
 * Irvine_BSEAVSW_Devel/23   2/13/04 5:34p erickson
 * PR9465: updated comments
 *
 * Irvine_BSEAVSW_Devel/22   2/13/04 4:19p erickson
 * PR9465: added VSB support to ui
 *
 * Irvine_BSEAVSW_Devel/21   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 *
 * Irvine_BSEAVSW_Devel/20   2/12/04 7:34p erickson
 * PR9465: reworked ChannelManager to support multiple tuners per
 * downstream
 *
 * Irvine_BSEAVSW_Devel/19   2/10/04 4:06p erickson
 * PR8850: 7038 frontend support
 *
 * Irvine_BSEAVSW_Devel/18   12/16/03 12:25a erickson
 * PR8731: POD ui work
 *
 * Irvine_BSEAVSW_Devel/17   11/19/03 4:57p erickson
 * PR8583: resolved some naming issues, especially bvideo_format
 *
 * Irvine_BSEAVSW_Devel/16   10/31/03 11:58a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/15   10/29/03 10:31a erickson
 * added QAM1024
 * support
 *
 * Irvine_BSEAVSW_Devel/14   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/13   10/22/03 11:23a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/12   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 *
 * Irvine_BSEAVSW_Devel/11   9/24/03 2:26p erickson
 * needed bogus TM_LINEINPUT for 7315
 *
 * Irvine_BSEAVSW_Devel/10   9/24/03 12:03p erickson
 * added comment header
 *
 *************************************************************/
#ifndef CHANNELMAP_H__
#define CHANNELMAP_H__

#include <mlist.h>
#include <tspsimgr.h> // PROGRAM_INFO_T
#include <bstd.h>
#include <bkni_multi.h> // BKNI_MutexHandle

#define IS_DSS(MPEG_TYPE) \
    ((MPEG_TYPE) == bstream_mpeg_type_dss_es || \
     (MPEG_TYPE) == bstream_mpeg_type_dss_pes)

/* flatten out settop api's frontend types */
typedef enum {
    btuner_sds_stream,
    btuner_qam_stream,
    btuner_streamer_stream, /* This is a streamer index. Settop API may set FPGA and other
                                board settings to enable it. */
    btuner_analog_stream,
    btuner_vsb_stream,
    btuner_ip_stream,
    btuner_ofdm_stream,     /* DTB-T stream */
#if B_HAS_RTSP
    btuner_rtsp_stream,
#endif
    btuner_total_stream_types /* not a valid stream type */
} btuner_stream_type;

/* channel map entry sub-type: distinguishes the source/creator of channel map */
typedef enum {
        bchannel_entry_static,  /* entry is statically added via channel map file */
        bchannel_entry_SAP=1,   /* entry is dynamically added via IP SAP Protocol */
        bchannel_entry_last
} bchannel_entry_sub_type;

/**
Summary:
The ChannelMap manages datastructures for a channel map and channel list data structures.
See module overview of ChannelManager for a complete overview.

The channel map is stored as a list of ChannelMapEntry objects.
The channel list is stored as a list of Channel objects.

The term "program" refers to Channel relative to its ChannelMapEntry. After scanning, a
ChannelMapEntry can be 0 or more programs. Each program is a Channel. All the programs
put together make up the channel list.

ChannelMap is stateless (no current channel, therefore no up/down).
It does not reference any PSI scanning code and no scanning logic. See ChannelScan for
details on scanning channels.
**/
class ChannelMap {
friend class ChannelScan;
public:
    ChannelMap();

    virtual ~ChannelMap();

    /**
    Copy constructor
    **/
//TODO: ChannelMap(const ChannelMap &map);

    /**
    Summary:
    Overwrite this ChannelMap with the given channel map.

    Description:
    The safe way to edit a channel map is to make a copy of it, modify it,
    and then if you want the changes, copy it back.
    **/
    void replace(const ChannelMap &map);

    /**
    Summary:
    One entry in the channel map.
    **/
    struct ChannelMapEntry {
        void setType(btuner_stream_type type);
        void setSubType(bchannel_entry_sub_type subType);
        btuner_stream_type getType() const {return _type;}
        bchannel_entry_sub_type getSubType() const {return _subType;}

        int disabled;   /* Used to fill entries on the channel map */
                        /* disabled == 1 indicates the entry is free */
                        /* disabled == 2 indicates that the entry has been deleted and should not be written to channels.txt */
        float freq;
        bool pod;
                /* Stores the Ch ID as known by the creator of this entry. E.g. in IPSTB,*/
                /* SAP dynamically adds/removes IP channels. The idea is to provide the */
                /* creator to also delete this entry using this ID. Chnum can't be used */
                /* for lookup as it can change due to user re-arranging channel map. */
                int sourceChNum;

        /* Use the type variable to determine which member of the union to use. */
        union {
            btuner_qam_params qam;
            btuner_vsb_params vsb;
            btuner_ofdm_params ofdm;
            struct {
                btuner_sds_params params;
                bstream_mpeg_type mpeg_type;
            } sds;
            struct {
                btuner_analog_params params;
                int linein; /* -1 is rf, else Settop API linein index */
            } analog;
            struct {
                unsigned index; /* the streamer index (which isn't necessarily the
                                Settop API id. See ChannelManager::addStreamer). */
                bstream_mpeg_type mpeg_type;
            } streamer;
            struct {
                bplayback_socket_params params;
                bool    hasProgramPidsLine; /* dma: true: has "PROGRAM PIDS" line in channels.txt */
                bool    gotAutoPids;        /* dma: true: auto pids worked, false: not */
                bool timestamp_enabled;
            } ip;
#if B_HAS_RTSP
            struct {
                char url[256];
            }rtsp;
#endif
        };

        /**
        Constructor
        **/
        ChannelMapEntry();

        /**
        Summary:
        Returns standard string for type based on btuner_stream_type.
        **/
        const char *typeStr() const;
        bstream_mpeg_type mpegType() const;

    protected:
        btuner_stream_type _type;
        bchannel_entry_sub_type _subType;
    };

    /** dma
     Summary:
     For IP auto PIDs. The method sets the gotAutoPids flag indicating auto pids worked or not.
        true: auto pids worked, don't need to scan for pids the next time.
        false: auto pids failed.
     **/
     void setGotAutoPids(int channel, bool ok);

    /**
    Summary:
    Total number of entries in the channel map.
    **/
    int total() const {return _entries.total();}

    /**
    Summary:
    Get a ChannelMapEntry. Returns true for success and
        copies contents of the channel map entry.
    **/
        bool getEntry(
                int entryIndex,
                ChannelMapEntry *entry // [out]: copy of matching entry's contents
                ) const;

        /**
        this function is no longer supported as channel entry pointer
        is no longer returned outside of the channel map object.
    int entryIndex(ChannelMapEntry *entry) const;
    **/

    /**
    Summary:
    Add a ChannelMapEntry to the end of the list.
    Description:
    ChannelMap now owns the memory and will free it.
    **/
    void add(ChannelMapEntry *entry);

    /**
    Summary:
    Insert a ChannelMapEntry into the list.
    Description:
    ChannelMap now owns the memory and will free it.
    **/
    void insert(int entryIndex, ChannelMapEntry *entry);

    /**
    Summary:
    Mark an entry as free (disabled ==1) for later reuse
    **/
    bool deleteEntry(int entryIndex);

    /**
    Summary:
    Mark an entry as free (disabled ==1) for later reuse.
    Description:
    Instead of using the entry Index (i.e. ch num), it uses
        ch num assigned by the creator of this entry (e.g by
        the IPSTB SAP module).
    **/
    bool deleteEntry(bchannel_entry_sub_type subType, int sourceChNum);

    /**
    Summary:
    Update the entry at the given index
    **/
    bool updateEntry(int entryIndex, ChannelMapEntry *entry);

    /**
    Summary:
    Move a ChannelMapEntry to another position in the channel map.
    **/
    bool rearrange(int entryIndex, int inc);

    /**
    Summary:
    Clear the channel map and channel list.
    **/
    void clear();

    /**
    Summary:
    Return the number of programs for a given entry.
    Description:
    A digital entry can have 0 or more programs.
    An analog entry always has 1 program.
    **/
    int totalPrograms(int entryIndex) const;

    /**
    Summary:
    Get the program info for a program. If returns true, then program
        is filled in with the program info of entryIndex.programIndex entry.
    **/
    bool getProgram(
                int entryIndex,
                int programIndex,
                PROGRAM_INFO_T *program         /* [Out] */
                );

    /**
    Summary:
    **/
    static void printProgram(const PROGRAM_INFO_T *program);

    /**
    * Read and write channel map (and optionally programs) to and from disk.
    **/
    enum {
        eChannelMapEntryInfoOnly,
        eChannelMapEntryAndProgramInfo
    };

    /**
    Summary:
    Determine the next channel in the channel list.
    **/
    int findNextChannel(
        int channel,  /* channel is 1-based. There is no channel 0. */
        int dir /* 1, 0 or -1 */
        ) const;

    /**
    Summary:
    Return the total number of channels in the channel list.
    **/
    int totalChannels() const {return _totalChannels;}

    /**
    Summary:
    One entry in the channel list. chnum contains both main & sub
        channel numbers. 0 return value indicates valid
        channelMapEntry. If program info is already scanned for the
        this channel, validProgramInfo is set to true and info is
        filled in the program structure.
    **/
    struct Channel {
        int chnum;              /* [in] */
        ChannelMapEntry entry;  /* [out] ChannelMapEntry for this Channel */
        bool validProgramInfo;  /* [out] indicates if program structure below is valid */
        PROGRAM_INFO_T program; /* [out] program info for this channel */

        Channel();
    };

    /**
    Summary:
    Get channel information from the channel list.
    **/
    int getChannel(
        int channel, /* Channel is 1-based. There is no channel 0. */
        Channel *Cchannel /* [out] */
        ) const;

    bool isDigital(int channel) const;
    bool isLineIn(int channel) const;
    const void *getPMT(int channel) const;

    /**
    Summary:
    Set a PROGRAM_INFO_T structure from simple variables.
    **/
    static void setProgram(PROGRAM_INFO_T *program,
        unsigned short pcrpid, unsigned short vidpid, unsigned short audpid,
        unsigned short audtype, unsigned short vidtype);
    static void setProgram(PROGRAM_INFO_T *program, const bstream_mpeg *mpeg);


    /**
    Summary:
    Print the channel list to stdio
    **/
    void printChannels() const;

    /**
    Summary:
    Returns true if this entry is not going to be scanned.
    Description:
    The "PROGRAM PIDS" syntax allows you to set the pids for a channel entry
    and bypass scanning.
    **/
    bool isNoScanChannelMapEntry(int entryIndex) const;

    /**
    Summary:
    Add a program to a ChannelMapEntry.
    Description:
    Called by ChannelScan and its descendents.
    **/
    bool addProgram(int entryIndex,
        const PROGRAM_INFO_T *program,
        const void *pmt = NULL,
        bool scanned = true /* if true, then clear will clear the programs.
            otherwise the programs will not be cleared when scanning. */
        );

    /**
    Summary:
    Clear all programs from a ChannelMapEntry.
    Description:
    Called by ChannelScan and its descendents.
    **/
    bool clearPrograms(int entryIndex = -1);

protected:
    BKNI_MutexHandle _mutex;

    /* The Wrapper is a ChannelMapEntry along with
    the programs that were scanned.
    */
    struct Program {
        PROGRAM_INFO_T info;
        const void *pmt;
/*
See podchannelscan.cpp for the only place where the pmt is set. There is a memory leak here, but POD demo
support is low priority.
        Program() {pmt=NULL;}
        ~Program() {if (pmt) delete pmt;}
*/
    };
    struct Wrapper {
        ChannelMapEntry entry;

        MAutoList<Program> programs;
        bool noScan;

        Wrapper(const ChannelMapEntry *e) {
            entry = *e; noScan = false;
        }
    };
    MAutoList<Wrapper> _entries;
    int _totalChannels;

    int channels(const Wrapper *sw) const;
};

/**
* Conversion of frontend enums to strings.
**/
const char *btuner_sds_str(btuner_sds_mode mode, bstream_mpeg_type mpeg_type);
const char *btuner_qam_str(btuner_qam_mode mode);
const char *btuner_vsb_str(btuner_vsb_mode mode);
const char *btuner_ofdm_str();

#define btuner_qam_annex_str(ANNEX) \
    ((ANNEX) == btuner_qam_annex_a ? "AnnexA":"AnnexB")
#define btuner_ofdm_mode_str(MODE) \
    ((MODE) == btuner_ofdm_mode_isdbt? "isdbt":"dvbt")
#define btuner_analog_format_str(FORMAT) \
    ((FORMAT) == bvideo_format_ntsc ? "NTSC":"PAL")
    
const char *btuner_analog_str(const ChannelMap::ChannelMapEntry *entry);

const char *bstream_mpeg_type_str(bstream_mpeg_type mpeg_type);

int b_get_default_qam_symrate(btuner_qam_annex annex, btuner_qam_mode qam_mode);

int b_print_channel_description(char *ptr, int length,
    ChannelMap::ChannelMapEntry *entry);

int b_print_chnum(char *ptr, int length,
    const ChannelMap::Channel *channel);

/* The number of b_lineinput_str depends on platform */
extern const char *b_lineinput_str[];

#define MAINCH(ch) ((ch)<0?(ch):((ch)&0xFF))
#define SUBCH(ch) ((ch)<0?0:((ch) >> 16))
#define MAKESUBCH(mainch, subch) (((subch)<<16) | ((mainch)&0xFF))

#endif // CHANNELMAP_H__

