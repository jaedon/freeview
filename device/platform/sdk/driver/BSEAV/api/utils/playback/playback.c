/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playback.c $
 * $brcm_Revision: 36 $
 * $brcm_Date: 5/20/10 10:59a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/playback/playback.c $
 * 
 * 36   5/20/10 10:59a mward
 * SW7335-661:  Keep support for BSEAV/lib/divxdrm for 7400, 7403.
 * bdrm_decrypt_init() prototype is different.
 * 
 * 35   4/29/10 6:46p mward
 * SW7125-376: C++ style comments are not allowed in ISO C90.
 * 
 * 34   2/5/10 5:06p mananp
 * SW7335-661:DivX DRM certification for 7335
 * 
 * 33   1/22/10 3:54p mward
 * SW7335-661: Restore changes lost in checkin of version 32.
 * 
 * 32   1/20/10 3:59p mananp
 * SW7335-661:DivX DRM certification for 7335
 * 
 * 31   10/5/09 4:59p erickson
 * SW7405-3130: don't call bplayback_get_trickmode_by_rate for non-MPEG
 * video
 * 
 * 30   7/30/09 12:15p vsilyaev
 * PR 57223: Fixed bad use of pointers
 * 
 * 29   4/14/09 12:34p katrep
 * PR54139: Fixed coverity issue
 * 
 * 28   5/12/08 5:16p erickson
 * PR42365: fix -once option for nexus
 *
 * 27   10/26/07 10:19a rjlewis
 * PR36481: can't use bcrypto init before bsettop_init.
 *
 * 26   10/25/07 2:31p jtna
 * PR35949: Coverity Defect ID:355 FORWARD_NULL
 *
 * 25   9/25/07 2:50p gmohile
 * PR 34640 : Add Divx DRM support
 *
 * 24   9/6/07 4:58p gmohile
 * PR 34640 : Add Divx Drm support
 *
 * 23   2/7/07 1:07p erickson
 * PR25125: add by_rate support so we can use STC trick modes for FP120
 * testing
 *
 * 22   11/17/06 4:50p erickson
 * PR24374: fix help
 *
 * 21   10/31/06 4:24p ahulse
 * PR25090: bargs need initializing via bargs_init()
 *
 * 20   9/7/06 2:10p vsilyaev
 * PR 23305: Fixed code to record data from the playback output
 *
 * 19   7/19/06 11:07a erickson
 * PR20382: don't require indexfile
 *
 * 18   7/11/06 9:53a jrubio
 * PR20382: Add Dual Stream Support, combine audio and video pes stream
 *
 * 16   4/12/06 2:16p vsilyaev
 * PR 20680: Promoted bsettop_fileio_fifo API
 *
 * 15   3/6/06 3:24p erickson
 * PR19785: 19785
 *
 * 14   2/14/06 4:52p vsilyaev
 * PR 8563: Updated timestamp support
 *
 * 13   2/2/06 5:04p vsilyaev
 * PR 19233:Use common function for tune. Added diseqc_tone option
 *
 * 12   1/17/06 11:29a erickson
 * PR17108: remove default pids for ES and PES. This prevents real ES and
 * PES usage.
 *
 * 11   12/23/05 4:10p vsilyaev
 * PR 18183: Renamed API functions
 *
 * 10   11/30/05 3:13p vsilyaev
 * PR 18183: Added FIFO file support
 *
 * 9   8/5/05 3:29p erickson
 * PR15139: clean up pedantic warnings
 *
 * 8   8/4/05 4:31p vsilyaev
 * PR16504: converted playpump_params.band to
 * playpump_params.route_through_parser_band
 *
 * 6   8/1/05 10:42a erickson
 * PR16300: check the right pointer
 *
 * 5   7/8/05 8:02a erickson
 * PR16138: fixed two divide by zero errors
 *
 * 4   5/24/05 5:37p vsilyaev
 * PR 13873: Merge OTFPVR code
 *
 * Irvine_BSEAVSW_Devel/BESOTFPVR/2   11/16/04 6:08p vsilyaev
 * Added support for OTF PVR.
 *
 * 3   4/22/05 2:33p erickson
 * PR14708: added standard go support
 *
 * 2   2/18/05 3:59p erickson
 * PR14180: fixes for vxworks
 *
 * 1   2/7/05 7:38p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/26   2/2/05 10:08a erickson
 * PR14006: allow "none" for indexfile so that we can use scripts with no
 * index playback
 *
 * Irvine_BSEAVSW_Devel/25   1/20/05 3:25p erickson
 * PR13097: fixed avc support in playback and decode
 *
 * Irvine_BSEAVSW_Devel/24   11/23/04 8:14p vsilyaev
 * PR 13351: Refactoring playback API.
 *
 * Irvine_BSEAVSW_Devel/23   9/13/04 4:57p erickson
 * PR12594: added fifo depth reading to status
 *
 * Irvine_BSEAVSW_Devel/22   6/16/04 5:15p erickson
 * PR11135: added uninit
 *
 * Irvine_BSEAVSW_Devel/21   4/16/04 11:43a erickson
 * PR8850: it might not be the mpegfile that can't be opened, so use a
 * more generic message
 *
 * Irvine_BSEAVSW_Devel/20   1/27/04 3:39p erickson
 * PR9496: rate 0 should be normal play. settop api doesn't do this
 * automatically.
 *
 * Irvine_BSEAVSW_Devel/19   1/22/04 2:50p vsilyaev
 * PR 8850: Updated to be compatible with new bargs.
 *
 * Irvine_BSEAVSW_Devel/18   12/19/03 7:41p vsilyaev
 * PR 8850: Use Settop API types.
 *
 * Irvine_BSEAVSW_Devel/17   11/10/03 2:38p erickson
 * PR8563: added transport timestamp support to driver, settop api and
 * bcmplayer
 *
 * Irvine_BSEAVSW_Devel/16   10/29/03 11:03a erickson
 * settop api changes
 * ES support
 *
 * Irvine_BSEAVSW_Devel/15   10/23/03 3:51p erickson
 * added PES default pids
 *
 * Irvine_BSEAVSW_Devel/14   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 *
 ***************************************************************************/

#include <bstd.h>
#include <bkni.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "bsettop.h"
#include "bargs.h"
#include "btune.h"

#include "bsettop_fileio.h"
#include "bsettop_fileio_fifo.h"

#ifdef B_HAS_DIVX_DRM
#include "bdrm_decrypt.h"
#endif

/* make these globals so that stat functions can get them */
bdisplay_t display;
bdecode_t decode;
bdecode_window_t window;

/**
* playback app specific args
**/
struct bplayback_app_arg {
    bool *once;
    bool *otfpvr;
    const char *scriptfile;
    const char *scripttext;
    bool *record;
    bool *fifo;
    struct {
        bool once;
        bool otfpvr;
        bool record;
        bool fifo;
    } storage;
};

struct bplayback_app_arg bplayback_app_arg = { NULL,NULL,NULL,NULL,NULL, NULL, {false, false, false, false} };
const struct bargs bplayback_app_args[] = {
    {"once", bargNone, parse_set, "No playback looping", (void *)&bplayback_app_arg.once, &bplayback_app_arg.storage.once},
    {"otfpvr", bargNone, parse_set, "On The Fly PVR", (void *)&bplayback_app_arg.otfpvr, &bplayback_app_arg.storage.otfpvr},
    {"record", bargNone, parse_set, "start record", (void *)&bplayback_app_arg.record, &bplayback_app_arg.storage.record},
    {"fifo", bargNone, parse_set, "use circular buffer(FIFO) for record", (void *)&bplayback_app_arg.fifo, &bplayback_app_arg.storage.fifo},
/*  {"scrtext", bargString, parse_string, "Script commands", (void *)&bplayback_app_arg.scripttext, NULL }, */
/*  {"scrfile", bargString, parse_string, "Script file", (void *)&bplayback_app_arg.scriptfile, NULL }, */
    BARGS_TERMINATOR
};
/**
* end playback app specific args
**/

static const bargs *playback_args[] = {
    bmpeg_args,
    bdisplay_args,
    bplayback_app_args,
    b_args,
    bstreamer_args,
    bqam_args,
    bvsb_args,
    bsds_args,
    brecord_args,
    NULL
};

/* Must be kept in sync with PlaybackState in playback.h */
static const char *stateStr[] = {
    "eStopped",
    "ePlaying",
    "ePaused",
    "eTrickMode",
    "eFrameAdvance"
};

static struct {
    int trickModeRate;
    int fast;
    bool cameToEnd;
    bool is_mpeg_video;
} state;

static void
usage(const char *name)
{
    fprintf(stderr, "%s: Settop playback application\n", name);
    barg_usage(name, playback_args, "mpegfile [{indexfile|none} [script]]");
    return;
}

static void
invalid_arg(int arg, const char *argv[])
{
    fprintf(stderr, "Invalid argument %s\n", argv[arg]);
    usage(argv[0]);
    exit(1);
}

static void
printMenu(void)
{
    printf(
    "BcmPlayer Menu:\n"
    "?      Print this menu\n"
    "play   Normal play\n"
    "pause  Pause\n");
    printf(
    "sr     Slow Motion Backward (incremental)\n"
    "sf     Slow Motion Forward (incremental)\n"
    "fr     Frame Reverse\n"
    "fa     Frame Advance\n"
    "rew    Rewind (incremental)\n"
    "ff     Fast Forward (incremental)\n");
    printf(
    "st     Print statistics\n"
    "h      Print command history\n"
    "sl X   Sleep X milliseconds\n"
    "g NUM  Goto an index\n"
    "t MODE [MODIFIER SLOWMOTIONRATE DECODER_MODE])   Set trick mode\n"
    "       MODE = Normal,SkipB,IP,SkipP,I,BCM,RATE\n"
    "       DECODER_MODE = all, IP, I\n"
    "wait   Wait for end/beginning of stream\n"
    "q      Quit\n"
    "\n");
}

/* TODO: this could be expanded with bcmplayer.c stats */
static int
printStats(bplayback_t pb)
{
    unsigned long totalIndex;
    bplayback_status status;
    bdecode_status decode_status;

    bplayback_get_status(pb, &status);
    totalIndex = status.index_last_offset + 1;

    printf(
    "Current index: %ld (%ld%%)\n"
    "Total index entries: %ld\n"
    "Mpeg Size: %ld\n"
    "Mpeg Pos:  %ld\n"
    "Total bytes played:  %ld\n"
    "State: %s\n",
    status.position.index_offset, status.position.index_offset  * 100 / totalIndex,
    totalIndex,
    (long)status.mpeg_file_size, (long)status.position.mpeg_file_offset,
    (long)status.bytes_played,
    status.state<(sizeof(stateStr)/sizeof(*stateStr))?stateStr[status.state]:""
    );

    bdecode_get_status(decode, &decode_status);
    printf(
    "Video Fifo: %u%%\n"
    "Audio Fifo: %u%%\n"
    "Playback:   %u%%\n",
    decode_status.video_fifo_size ?
        (unsigned)(decode_status.video_fifo_depth * 100 / decode_status.video_fifo_size) : 0,
    decode_status.audio_fifo_size ?
        (unsigned)(decode_status.audio_fifo_depth * 100 / decode_status.audio_fifo_size) : 0,
    status.fifo_size ?
        (unsigned)(status.fifo_depth * 100 / status.fifo_size) : 0);

    return 0;
}

static int
trickMode(bplayback_t pb, int fast, int dir)
{
    int rc;

    /* reset */
    if (fast != state.fast) {
        state.trickModeRate = 0;
        state.fast = fast;
    }

    /* hop over 1 */
    if (state.trickModeRate == 0 && dir > 0)
        state.trickModeRate++;
    else if (state.trickModeRate == 2 && dir < 0)
        state.trickModeRate--;
    state.trickModeRate += dir;

    if (state.trickModeRate == 0) {
        rc = bplayback_play(pb);
    }
    else {
        bplayback_trickmode_params params;
        int rate;
		/* bplayback_trickmode_params params; */

        if (state.fast)
            rate = BPLAYBACK_NORMALPLAY_SPEED * state.trickModeRate;
        else
            rate = BPLAYBACK_NORMALPLAY_SPEED / state.trickModeRate;

        if (state.is_mpeg_video) {
            /* bplayback_get_trickmode_by_rate is only valid for MPEG video */
            rc = bplayback_get_trickmode_by_rate(pb, rate, &params);
            if (rc) return rc;
        }
        else {
            /* more trick modes are possible, but this gives basic coverage. */
            bplayback_trickmode_params_init(&params);
            params.mode = bplayback_player_mode_by_rate;
            params.mode_modifier = rate;
        }

        rc = bplayback_trickmode(pb, &params);
    }
    return rc;
}

static bplayback_loopmode handleloop(void *context)
{
    BSTD_UNUSED(context);
    state.cameToEnd = true;
    if (bplayback_app_arg.once)
        return bplayback_loopmode_pause;
    else
        return bplayback_loopmode_loop;
}

static int
processCmd(bplayback_t pb, const char *s) {
    const char *end = s+strcspn(s, " \t");
    const char *param = "0\n";
    if (end)
        param = end+strspn(end, " \t");

#define CMD(STR) (!strncmp(s, STR, end-s))
    printf("processCmd '%s'\n", s);
    if (CMD("?") || CMD("help")) {
        printMenu();
    }
    else if (CMD("play")) {
        bplayback_play(pb);
        state.trickModeRate = 0;
    }
    else if (CMD("pause")) {
        bplayback_pause(pb);
    }
    else if (CMD("fr")) {
        bplayback_frameadvance(pb, bplayback_player_mode_brcm, false);
        state.trickModeRate = 0;
    }
    else if (CMD("fa")) {
        bplayback_frameadvance(pb, bplayback_player_mode_brcm, true);
        state.trickModeRate = 0;
    }
    else if (CMD("sr")) {
        trickMode(pb, 0, -1);
    }
    else if (CMD("sf")) {
        trickMode(pb, 0, 1);
    }
    else if (CMD("rew")) {
        trickMode(pb, 1, -1);
    }
    else if (CMD("ff")) {
        trickMode(pb, 1, 1);
    }
    else if (CMD("q")) {
        return 1;
    }
    else if (CMD("wait")) {
        state.cameToEnd = false;
        while (!state.cameToEnd) {
            BKNI_Sleep(100 );
        }
    }
    else if (CMD("g")) {
        bplayback_goto_index(pb, atol(param), SEEK_SET);
    }
    else if (CMD("st")) {
        printStats(pb);
    }
    else if (CMD("sl")) {
        BKNI_Sleep(atoi(param));
    }
    else if (CMD("t")) {
        bplayback_trickmode_params params;
        int rc;
        char *cmd;

        bplayback_trickmode_params_init(&params);

        cmd = strtok((char *)param, " \t");
        if (!cmd) {
            bplayback_play(pb);
            state.trickModeRate = 0;
            return 0;
        }

        if (!strcasecmp(cmd, "SkipB"))
            params.mode = bplayback_player_mode_skip_b;
        else if (!strcasecmp(cmd, "SkipP"))
            params.mode = bplayback_player_mode_skip_p;
        else if (!strcasecmp(cmd, "IP"))
            params.mode = bplayback_player_mode_ip;
        else if (!strcasecmp(cmd, "I"))
            params.mode = bplayback_player_mode_i;
        else if (!strcasecmp(cmd, "BCM"))
            params.mode = bplayback_player_mode_brcm;
        else if (!strcasecmp(cmd, "normal"))
            params.mode = bplayback_player_mode_normal;
        else if (!strcasecmp(cmd, "rate"))
            params.mode = bplayback_player_mode_by_rate;
        else {
            printf("unknown mode: %s\n", cmd);
            return 0;
        }

        cmd = strtok(NULL, " \t");
        if (cmd)
            params.mode_modifier = atoi(cmd);

        cmd = strtok(NULL, " \t");
        if (cmd)
            params.slow_motion_rate = atoi(cmd);

        cmd = strtok(NULL, " \t");
        if (cmd) {
            if (!strcasecmp(cmd, "all"))
                params.decode_mode = bplaypump_decode_mode_all;
            else if (!strcasecmp(cmd, "IP"))
                params.decode_mode = bplaypump_decode_mode_ip;
            else if (!strcasecmp(cmd, "I"))
                params.decode_mode = bplaypump_decode_mode_i;
        }
        rc = bplayback_trickmode(pb, &params);
/*      if (!rc)
            printf("Trick mode: %s\n", trick_getLabel(mode, modeModifier, slowMotion)); */
        state.trickModeRate = 0;
    }
#ifdef USE_READLINE
    else if (CMD("h")) {
        HIST_ENTRY **list;
        register int i;

        list = history_list ();
        if (list)
        {
            for (i = 0; list[i]; i++)
            fprintf (stderr, "%d: %s\r\n", i, list[i]->line);
        }
    }
#endif
    else {
        printf("Unknown command: %s\n", s);
    }
    return 0;
}


static int
processCmdString(bplayback_t pb, char *s) {
    char *cmd;
    char *end = s+strlen(s);
    cmd = strtok(s, ";");
    while (cmd) {
        /* strtok will be reused again inside processCmd,
        and it means it will change strtok global state.
        This is a workaround. */
        char *testend = cmd + strlen(cmd);
        cmd += strspn(cmd, " \t");
        if (processCmd(pb, cmd))
            return 1;
        if (testend == end)
            break;
        cmd = strtok(testend+1, ";");
    }
    return 0;
}

static void
DVRMenu(bplayback_t pb)
{
    char s[256];

    printMenu();
    while (1)
    {
        char *cmd;

#ifdef USE_READLINE
        cmd = readline(">");
        if (cmd) {
            strcpy(s, cmd);
            if (*s)
                add_history(s);
            free(cmd);
        }
#else
        printf(">");
        cmd = fgets(s,sizeof(s)-1,stdin);
        if (cmd==NULL) {
            break;
        }
        cmd = strchr(s, '\n');
        if (cmd) {
            *cmd = '\0';
        }
#endif

        if (processCmdString(pb, s))
            break;
    }
}

int
main(int argc, const char *argv[])
{
    bresult rc;
    bstream_mpeg mpeg,mpeg2;
    bstream_t stream,stream2=NULL;
    bplayback_file_t file=NULL,file2=NULL;
    bplayback_t playback=NULL,playback2=NULL;
    bplaypump_t playpump=NULL,playpump2=NULL;
    int last_arg;
    const char *mpegfile=NULL;
    char *script = NULL;
    bool done=false;
    const char *indexfile=NULL;
    bplayback_params play_params,play_params2;
    int index = 0;
    brecord_t record=NULL;
    bfile_out_fifo_t fifo_file=NULL;
    btune_state tune_state;
#ifdef B_HAS_DIVX_DRM
    bdrm_decrypt_t decrypt;
#endif
    bargs_init();

    last_arg = barg_parse(argv, playback_args, 1, argc);
    if (last_arg<0) {
        invalid_arg(-last_arg, argv);
    } else if (last_arg==0) { /* there is should be at least one extra parameter */
        /* do nothing */
    } else {
        mpegfile = argv[last_arg++];
        if (argc>last_arg) {
            indexfile = argv[last_arg++];
            if (argc>last_arg) {
                script = (char *)argv[last_arg++];
            }
        }
    }

    /* check for at least a file before the lengthy init */
    if (!mpegfile) {
        usage(argv[0]);
        return 1;
    }


    rc = bsettop_init(BSETTOP_VERSION); /* initialize global stuff */
    if (rc!=b_ok) {
        fprintf(stderr, "Initialization error\n");
        return 1;
    }

    bencryption_params_init( &(bmpeg_arg.storage.encryption) );
    bmpeg_arg.storage.encryption.key_length = 64;

    btune_mpeg_init(&mpeg);
    if (bmpeg_arg.dual_stream) {
        /* Will duplicate everything from video to audio */
        fprintf(stdout, "\t\t DUAL PES MODE\n");
        btune_mpeg_init(&mpeg2);
    }

    if (bdisplay_arg.display) {
        index = *bdisplay_arg.display;
    }

    if (indexfile && !strcmp(indexfile, "none")) {
        if (bmpeg_arg.dual_stream) {
            fprintf(stderr,"You must have a 2nd audio pes file\n");
            return 1;
        }
        indexfile  = NULL;
    }else if (!indexfile && bmpeg_arg.dual_stream) {
        fprintf(stderr,"You cannot specify dual pes and none\n ");
        return 1;
    }

#ifdef B_HAS_DIVX_DRM
	#if (BCHP_CHIP==7400) || (BCHP_CHIP==7403)
	decrypt = bdrm_decrypt_init(mpegfile);
	#else
    decrypt = bdrm_decrypt_init(mpegfile, NULL,NULL);
	#endif
#endif

    if (bplayback_app_arg.record && *bplayback_app_arg.record) {
        brecord_file_t rfile;
        brecpump_t recpump;
        brecord_params params;

        /* start record */
        if (!indexfile) {
            usage(argv[0]);
            fprintf(stderr, "record option requires index file\n");
            return 1;
        }
        if (bplayback_app_arg.fifo && *bplayback_app_arg.fifo) {
            fifo_file = bfile_fifo_out_create(mpegfile, indexfile);
            if (!fifo_file) {
                perror(mpegfile);
                return 1;
            }
            rfile = bfile_fifo_out_file(fifo_file);
        } else {
            rfile = brecord_file_open(mpegfile, indexfile);
            if (!file) {
                perror(mpegfile);
                return 1;
            }
        }
        stream = btune_tune(0, &mpeg, true, &tune_state);
        if (!stream) {
            return 1; /*  nothing to record */
        }
        recpump = brecpump_open(B_ID(brecord_arg.channel?*brecord_arg.channel:btuner_arg.channel?*btuner_arg.channel:0), NULL);
        if (!recpump) {
            fprintf(stderr, "Record is not avaliable\n");
            return 1;
        }
        record = brecord_open();
        if (!record) {
            fprintf(stderr, "Record is not avaliable\n");
            return 1;
        }
        brecord_params_init(&params, record);
        if (bmpeg_arg.timestamp) {
            params.timestamp_enabled = *bmpeg_arg.timestamp;
            printf("timestamp %d\n", params.timestamp_enabled );
        }

        if (brecord_start(record, recpump, stream, rfile, &params)!=b_ok) {
            fprintf(stderr, "Can't start record\n");
            return 1;
        }
#ifdef __vxworks
        BKNI_Sleep(2000);
#else
        sleep(2);
#endif
    }

    if (fifo_file) {
        file = bfile_fifo_in_open(mpegfile, indexfile, fifo_file);
    } else if(bmpeg_arg.dual_stream) {
        file = bplayback_file_open(mpegfile, NULL);
        /* Using Index file as audio pes source */
        file2 = bplayback_file_open(indexfile,NULL);
    } else {
        file = bplayback_file_open(mpegfile, indexfile);
    }
    if (!file) {
        perror("Unable to open file(s)");
        return 1;
    } else if(bmpeg_arg.dual_stream && !file2){
        perror("Unable to open file2 for dual pes mode");
        return 1;
    }
#ifdef B_HAS_DIVX_DRM
    memcpy(&mpeg.encryption.key, &decrypt, sizeof(decrypt));
#endif

    playpump = bplaypump_open(B_ID(index), NULL);
    if (!playpump) {
        fprintf(stderr, "Can't use playpump %d\n", index);
        return 1;
    }
    if (bplayback_app_arg.otfpvr && *bplayback_app_arg.otfpvr) {
        playback = botfplay_open(B_ID(index));
    } else {
        playback = bplayback_open();
    }
    if (!playback) {
        fprintf(stderr, "Can't use playback\n");
        return 1;
    }

    if (bmpeg_arg.dual_stream) {
        /* Move to next index? */
        playpump2 = bplaypump_open(B_ID(index+1), NULL);
        if (!playpump2) {
            fprintf(stderr, "Can't use playpump2 (dual pes) %d\n", index);
            return 1;
        }

        playback2 = bplayback_open();

        if (!playback2) {
            fprintf(stderr, "Can't use playback2 (dual pes)\n");
            return 1;
        }

    }

    bplayback_params_init(&play_params, playback);
    play_params.index_format = indexfile?bindex_format_bcm:bindex_format_none;
    play_params.callback_context = playback;
    play_params.end_of_stream = handleloop;
    play_params.beginning_of_stream = handleloop;
    play_params.loopmode.end_of_stream = bplayback_app_arg.once?bplayback_loopmode_pause:bplayback_loopmode_loop;
    play_params.loopmode.beginning_of_stream = bplayback_app_arg.once?bplayback_loopmode_pause:bplayback_loopmode_loop;
    if (bmpeg_arg.timestamp) {
        play_params.timestamp_enabled = *bmpeg_arg.timestamp;
        play_params.timestamp_active = *bmpeg_arg.timestamp;
        if (bmpeg_arg.ignore_timestamp) {
            play_params.timestamp_active = !(*bmpeg_arg.ignore_timestamp);
        }
    }
    if (bplayback_app_arg.otfpvr && *bplayback_app_arg.otfpvr) {
        play_params.route_through_parser_band = true;
    }

    if(bmpeg_arg.dual_stream) {
        bplayback_params_init(&play_params2, playback2);
        play_params.callback_context = playback2;
        play_params.end_of_stream = handleloop;
        play_params.beginning_of_stream = handleloop;
        play_params.loopmode.end_of_stream = bplayback_app_arg.once?bplayback_loopmode_pause:bplayback_loopmode_loop;
        play_params.loopmode.beginning_of_stream = bplayback_app_arg.once?bplayback_loopmode_pause:bplayback_loopmode_loop;
    }

    if (record) {
        stream = bplayback_start_timeshifting(playback, playpump, &mpeg, file, &play_params, record);
    } else if (bmpeg_arg.dual_stream){
        stream = bplayback_start(playback, playpump, &mpeg, file, &play_params);
        stream2 = bplayback_start(playback2, playpump2, &mpeg2, file2, &play_params2);
    } else{
        stream = bplayback_start(playback, playpump, &mpeg, file, &play_params);
    }

    if (!stream) {
        fprintf(stderr, "Can't start playback\n");
        return 1;
    } else if (bmpeg_arg.dual_stream && !stream2 ) {
        fprintf(stderr, "Can't start playback2 pes stream\n");
        return 1;
    }

    decode = bdecode_open(B_ID(index)); /* open first decode engine */
    if (!decode) {
        fprintf(stderr, "Decode is not avaliable");
        return 1;
    }

    display = bdisplay_open(B_ID(index)); /* open first display */
    if (!display) {
        fprintf(stderr, "Display is not avaliable");
        return 1;
    }

    window = bdecode_window_open(B_ID(index), display); /* open first window on the given display */
    if (!window) {
        fprintf(stderr, "Decode window is not avaliable");
        return 1;
    }

    /* display program */
    if (bmpeg_arg.dual_stream) {
        bstream_join(stream, stream2);
        rc = bdecode_start(decode, stream, window);
    } else {
        rc = bdecode_start(decode, stream, window); /* start decode */
    }
    if (rc!=b_ok) {
        fprintf(stderr, "Can't start decode");
        return 1;
    }
#if 0
        /* this is an example of how data could be recorded from the playback */
        {
        brecord_file_t file;
        brecord_t record;
        brecord_params params;
        brecpump_t recpump;
        const char *fname="remux.ts";

            file = brecord_file_open(fname, NULL);
            if (!file) {
                perror(fname);
                return 1;
            }
            record = brecord_open();
            if (!record) {
                fprintf(stderr, "Record not available\n");
                return 1;
            }
            recpump = brecpump_open(B_ID(0), NULL);
            if (!recpump) {
                fprintf(stderr, "Recpump not available\n");
                return 1;
            }
            brecord_params_init(&params, record);
            if (brecord_start(record, recpump, stream, file, &params)!=b_ok) {
                fprintf(stderr, "Can't start record\n");
                return 1;
            }
        }
#endif

    state.is_mpeg_video = (mpeg.video[0].format == bvideo_codec_mpeg2);
    if (script) {
        done = processCmdString(playback, script);
    }


    if (!done) {
        DVRMenu(playback);
    }


    if(record) {
        bplayback_stop_timeshifting(playback);
        brecord_stop(record);
        brecord_close(record);
    }
    bstream_close(stream);
    if(bmpeg_arg.dual_stream)
        bstream_close(stream2);
    bdecode_window_close(window);
    bdecode_close(decode);
    bdisplay_close(display);
    bsettop_uninit();

    return 0;
}

