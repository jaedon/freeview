/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: decode_test.c $
 * $brcm_Revision: 62 $
 * $brcm_Date: 5/10/10 11:19a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/decode/decode_test.c $
 * 
 * 62   5/10/10 11:19a calvinho
 * SW7405-4202: Correct Coverity comment
 * 
 * 61   4/13/10 3:17p calvinho
 * SW7405-4202: Fix Coverity Defect ID:19650 CHECKED_RETURN
 * 
 * 60   7/30/09 12:15p vsilyaev
 * PR 57223: Fixed bad use of pointers
 * 
 * 59   7/1/09 11:34a katrep
 * PR54139: Another coverity issue, stack > 1024
 * 
 * 58   4/14/09 12:20p katrep
 * PR54139: Fixed coverity issues
 * 
 * 56   9/19/08 5:07p vishk
 * PR 47150: Coverity Issues.
 * 
 * 55   3/3/08 12:55p vsilyaev
 * PR 39818: Print difference between pts as signed integer
 * 
 * 54   12/17/07 1:44p katrep
 * PR37217: Added 7335 support
 * 
 * 53   11/21/07 11:50a jtna
 * PR35943: Coverity Defect ID:484 SECURE_CODING - mark as intentional
 * 
 * 52   10/26/07 10:19a rjlewis
 * PR36481: can't use bcrypto init before bsettop_init.
 * 
 * 51   10/18/07 5:56p jtna
 * PR35943: Coverity Defect ID:484 SECURE_CODING
 * 
 * 50   10/18/07 5:53p jtna
 * PR35940: Coverity Defect ID:502 STACK_USE
 * 
 * 48   3/1/07 11:26a gmohile
 * PR 25109: add 7403 support
 * 
 * 47   1/17/07 6:10p ptimariu
 * PR20685: initialized ext_asf_scanning to 0
 * 
 * 46   10/31/06 4:20p ahulse
 * PR25090: bargs need initializing via bargs_init()
 * 
 * 45   10/17/06 2:40p vsilyaev
 * PR 24958: Fixed dropping of data
 * 
 * 44   10/5/06 4:00p vsilyaev
 * PR 24775: Added code that allows to move to the next playback file
 * 
 * 43   10/2/06 6:34p mward
 * PR21671:  Add 7118.
 * 
 * 42   9/14/06 5:49p jgarrett
 * PR 20139: Removing compiler warning.
 * 
 * 41   9/14/06 5:20p jrubio
 * PR24136: You can still pass in an index, but if you don't you will not
 * see the *.ndx message
 * 
 * 40   9/7/06 2:10p vsilyaev
 * PR 23305: Fixed code to record data from the playback output
 * 
 * 39   8/23/06 4:15p ptimariu
 * PR20685: removed unused open_params
 * 
 * 37   8/22/06 4:42p vsilyaev
 * PR 23826: Added option to take playback data from a pipe (stdin)
 * 
 * 36   8/20/06 11:00p ptimariu
 * PR20685: remove lic_extern from decode
 * 
 * 34   8/17/06 3:54p ahulse
 * PR23613: Don't enable encoder when using 656 input
 * 
 * 33   8/8/06 3:32p erickson
 * PR23129: support external analog on 740x
 * 
 * 32   8/2/06 11:42p ptimariu
 * PR20685: merge from drm playback/playpump change
 * 
 * PR20685_MSDRM/7   7/24/06 1:42p ptimariu
 * PR20685: using playback instead of playpump for drm
 * 
 * 31   7/25/06 5:24p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 * 
 * 30   7/21/06 10:28a ptimariu
 * PR20685: fix dual decode drm
 * 
 * 28   7/20/06 5:45p ptimariu
 * PR20685: asf drm integration
 * 
 * 28   7/20/06 5:41p ptimariu
 * PR20685: merge asf drm
 * 
 * PR20685_MSDRM/6   7/20/06 10:34a ptimariu
 * PR20685: initialize drm_t
 * 
 * PR20685_MSDRM/5   7/17/06 3:38p ptimariu
 * PR20685: fix return type for lic_extern default function
 * 
 * PR20685_MSDRM/4   7/12/06 2:31p ptimariu
 * PR20685: added drm callback context
 * 
 * PR20685_MSDRM/3   7/6/06 5:11p ptimariu
 * PR20685: added external grabber, license playcount, times
 * 
 * PR20685_MSDRM/2   7/5/06 2:33p ptimariu
 * PR20685: changed license notification interface
 * 
 * PR20685_MSDRM/1   6/30/06 12:25p ptimariu
 * PR20685: asf drm integration
 *
 * 27   6/9/06 10:56a erickson
 * PR21941: cleaned up/standardized error messages
 * 
 * 26   4/7/06 1:39p erickson
 * PR17108: fix stream[] reference
 * 
 * 25   4/3/06 5:56p vsilyaev
 * PR 20577: Added bitrate monitor
 * 
 * 24   3/31/06 6:58p vsilyaev
 * PR 20577: Added code to show source resolution
 * 
 * 23   2/9/06 2:03p vsilyaev
 * PR 18642:print playback level, if applicable
 * 
 * 22   2/2/06 5:04p vsilyaev
 * PR 19233:Use common function for tune. Added diseqc_tone option
 * 
 * 21   12/15/05 3:55p vsilyaev
 * PR 18642: Added support for Video Over IP sources
 * 
 * 20   11/3/05 4:04p vsilyaev
 * PR 17875: Added support for VC-1
 * 
 * 19   8/17/05 12:18p vsilyaev
 * PR15139: Fixed format for the sscanf.
 * 
 * 18   6/20/05 2:57p vsilyaev
 * PR 15866: Added support of format of the encoded audio.
 * 
 * 17   6/15/05 6:14p vsilyaev
 * PR 14386: Added code to printout difference between PTS and STC.
 * 
 * 15   5/27/05 6:30p vsilyaev
 * PR14397: Refactoring digital VBI support to include 7411
 * 
 * 14   5/26/05 11:18a vsilyaev
 * PR 13873: Removed test OTFPVR code
 * 
 * 13   5/24/05 6:33p vsilyaev
 * PR 13873: Fixed activation of  OTFPVR code
 * 
 * 12   5/24/05 5:36p vsilyaev
 * PR 13873: Merge OTFPVR code
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/6   11/15/04 8:45p vsilyaev
 * Added code to switch to rewind mode.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/5   11/13/04 1:57p vsilyaev
 * Separated otfpvr to backend and frontend  parts.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/4   11/12/04 1:37p vsilyaev
 * Refactored playback interface.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/3   10/21/04 8:10p vsilyaev
 * Merged 7038 changes.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/BESOTFPVR_7038/1   10/18/04 6:27p vsilyaev
 * Added decoding of the OTF PVR stream.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/2   10/11/04 8:54p vsilyaev
 * Changed bespvr code to start  playback routing to pidparser.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/1   10/8/04 7:59p vsilyaev
 * Added support for BESPVR
 * 
 * 11   4/21/05 5:59p vsilyaev
 * PR 14955: Added support for clipping parameters.
 * 
 * 9   4/20/05 3:01p vsilyaev
 * PR 14724: Added handling of timestamp option.
 * 
 * 8   4/11/05 11:56a vsilyaev
 * PR 14696: Removed unused variable.
 * 
 * 7   3/31/05 3:10p vsilyaev
 * PR 14440: Added options to set encryption parameters.
 * 
 * 6   3/3/05 4:39p vsilyaev
 * PR 14305: Open decode before stream, due to weirdness of legacy
 * implementation it helps to not trigger open of decoder driver.
 * 
 * 5   2/24/05 4:00p erickson
 * PR14180: added more flexible go() command for vxworks
 * 
 * 4   2/18/05 3:59p erickson
 * PR14180: added vxworks support
 * 
 * 3   2/14/05 4:33p vsilyaev
 * PR13158: Merged to the main branch
 * 
 * Irvine_BSEAVSW_Devel/48   2/10/05 1:04p erickson
 * PR13823: if decoding hd, be helpful and turn off sd outputs
 * automatically
 * 
 * Irvine_BSEAVSW_Devel/47   12/14/04 6:10p vsilyaev
 * PR13158: Open tuner only when it's needed (helps playback on the
 * platform without tuner).
 * 
 * Irvine_BSEAVSW_Devel/46   12/7/04 4:50p vsilyaev
 * PR 13097: Added support for two decodes.
 * 
 * Irvine_BSEAVSW_Devel/45   11/23/04 8:14p vsilyaev
 * PR 13351: Refactoring playback API.
 * 
 * Irvine_BSEAVSW_Devel/44   11/23/04 4:43p vsilyaev
 * PR 13351: Refactoring playback API.
 * 
 * Irvine_BSEAVSW_Devel/43   11/9/04 12:22p vsilyaev
 * PR 13097: Refactored code to allow launch of multiple decodes.
 * 
 * Irvine_BSEAVSW_Devel/42   10/28/04 7:45p vsilyaev
 * PR 13097: Added video coding type.
 * 
 * Irvine_BSEAVSW_Devel/41   10/25/04 11:52a vsilyaev
 * PR 12576: Added code to use linein id.
 * 
 * Irvine_BSEAVSW_Devel/40   10/13/04 5:42p bandrews
 * PR12998: Implemented
 * 
 * Irvine_BSEAVSW_Devel/39   8/18/04 10:11a vsilyaev
 * PR11425: PR 12027: Added support for content mode and aspect ratio.
 * 
 * Irvine_BSEAVSW_Devel/38   6/2/04 2:43p erickson
 * PR11204: changed/removed deprecated settop api functions
 * 
 * Irvine_BSEAVSW_Devel/37   5/3/04 12:38p vsilyaev
 * PR 10927: Use new bconfig layout.
 * 
 * Irvine_BSEAVSW_Devel/36   4/2/04 4:06p vsilyaev
 * PR 10421: Added VBI support
 * 
 * Irvine_BSEAVSW_Devel/35   4/1/04 1:09p vsilyaev
 * PR10201: PR 8850: Removed RF output, when user sets explicit output
 * configuration.
 * 
 * Irvine_BSEAVSW_Devel/34   3/3/04 5:08p vsilyaev
 * PR 8850: Improved handling of playback files.
 * 
 * Irvine_BSEAVSW_Devel/33   2/18/04 11:06a erickson
 * PR9792: added some debug output
 * 
 * Irvine_BSEAVSW_Devel/32   2/4/04 4:34p vsilyaev
 * PR 9465: Added VSB support.
 * 
 * Irvine_BSEAVSW_Devel/31   1/20/04 4:02p vsilyaev
 * PR 8850: Added handlng of the MPEG stream type.
 * 
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "bsettop.h"
#include "bargs.h"
#include "btune.h"

static struct b_extra_arg {
    const char *host;
    int *port;
    int *play_time;
    struct {
        int port;
        int play_time;
    } storage;
} b_extra_arg = {
    NULL,NULL,NULL,
    {0,0}
};

static const struct bargs bextra_args[] = {
    {"host", bargString, parse_string, "VideoOverIP server", (void *)&b_extra_arg.host, NULL},
    {"port", bargInteger, parse_int, "port for VideoOverIP", (void *)&b_extra_arg.port, &b_extra_arg.storage.port},
    {"play_time", bargInteger, parse_int, "play stream this number of seconds", (void *)&b_extra_arg.play_time, &b_extra_arg.storage.play_time},
    BARGS_TERMINATOR
};


static const bargs *decode_args[] = {
    b_args,
    bqam_args,
    bsds_args,
    bstreamer_args,
    bmpeg_args,
    bdisplay_args,
    bplayback_args,
    banalog_args,
    btuner_args,
    bdecode_args,
    bwindow_args,
    bvolume_args,
    bvsb_args,
    bextra_args,
    NULL
};

static void
usage(const char *name)
{
    fprintf(stderr, "%s: Settop decode application\n", name);
    barg_usage(name, decode_args, "[filename] [indexname]");
    return;
}

static void 
invalid_arg(int arg, const char *argv[])
{
    fprintf(stderr, "Invalid argument %s\n", argv[arg]);
    usage(argv[0]);
    exit(1);
}

#ifdef LINUX
#include <sys/time.h>
struct bplay_pipe {
    bplaypump_t playpump;
    int fd;
    bool eof;
};

static void 
bplay_pipe_run(struct bplay_pipe *pipe)
{
    struct timeval end, now, timeout;
    fd_set fds;
    const int SIZE = 16384;
    uint8_t* buf = 0;
    int rc;
    void *buffer;
    size_t buffer_size;
    size_t off;
    size_t buf_len;

    buf = (uint8_t*) malloc(SIZE * sizeof(*buf));
    if (!buf)
        goto error;

    if (pipe->eof) {
        sleep(1);
        free(buf);
        return;
    }
    rc = gettimeofday(&end, NULL);
    if(rc){
        perror("gettimeofday");
        goto error;
    }
    end.tv_sec++;
    for(;;) {
        FD_ZERO(&fds);
        FD_SET(pipe->fd, &fds);
        rc = gettimeofday(&now, NULL);
        if(rc){
        perror("gettimeofday");
        goto error;
        }
        timeout.tv_sec = end.tv_sec - now.tv_sec;
        timeout.tv_usec = end.tv_usec - now.tv_usec;
        if (timeout.tv_usec < 0) {
            timeout.tv_usec += 1000000;
            timeout.tv_sec--;
        }
        if (timeout.tv_sec<0) {
            break;
        }
        rc = select(pipe->fd+1, &fds, NULL, NULL, &timeout);
        if (rc<0) {
            perror("select");
            goto error;
        } else if (rc==0) {
            break;
        }
        rc = read(pipe->fd, buf, SIZE * sizeof(*buf));
        if (rc<0) {
            perror("select");
            goto error;
        }
        if (rc==0) {
            pipe->eof = true;
            break;
        }
        for(buf_len=rc,off=0;off<buf_len;) {
            if (bplaypump_get_buffer(pipe->playpump, &buffer, &buffer_size)) {
                perror("bplaypump_get_buffer");
                goto error;
            }
            if(buffer_size==0) {
                timeout.tv_sec = 0;
                timeout.tv_usec = 10000;
                rc = select(pipe->fd+1, NULL, NULL, NULL, &timeout);
                if (rc<0) {
                    perror("select");
                    goto error;
                }
                continue; /* have to consume all data what have read */
            }
            if (buffer_size > buf_len-off) {
                buffer_size = buf_len-off;
            }
            memcpy(buffer,buf+off, buffer_size);
            if (bplaypump_read_complete(pipe->playpump, 0, buffer_size)) {
                perror("bplaypump_read_complete");
                goto error;
            }
            off += buffer_size;
        }
    }
    if (buf)
        free(buf);
    return;
error:
    pipe->eof = true;
    if(buf)
        free(buf);
    return;
}
#endif


#define B_N_RESOURCES   2

#if B_HAS_DRM

#include "blst_slist.h"  
#include "drmdecryptor.h" 
#include "drmcore.h" 

/* example callback function for the license state notification
*/
bdrm_err 
bdrm_util_license_callback (bdrm_acq_state* lic_acq_state, bdrm_license_t license,
                            const void* context)
{
    bdrm_license_info lic_info; bdrm_err ret = bdrm_err_ok;
    char* type_msg = NULL; uint8_t count = 0, index = 0; 
    
    BSTD_UNUSED(context);
    if (lic_acq_state == NULL) { return (bdrm_err_fail); }    
    printf("\n\n--->>license acq state changed to lic:(%d) http:(%d)", 
           lic_acq_state->license_state, lic_acq_state->http_state);

    if ((lic_acq_state->license_state == bdrm_lic_acquired) || 
        (lic_acq_state->license_state == bdrm_lic_faillocal)) {
        count = bdrm_license_get_count(license);
        for (index = 0; index < count; index++) { 
            memset(&lic_info, 0, sizeof(bdrm_license_info));
            bdrm_license_get_item(license, index, &lic_info);            
            switch (lic_info.type) {
                case bdrm_lictype_from:         type_msg = "from"; break;
                case bdrm_lictype_until:        type_msg = "until"; break;
                case bdrm_lictype_fromuntil:    type_msg = "from-until"; break;
                case bdrm_lictype_count:        type_msg = "count"; break;
                case bdrm_lictype_unlimited:    type_msg = "unlimited"; break;
                case bdrm_lictype_noright:      type_msg = "noright"; break;
                case bdrm_lictype_unknown:      type_msg = "unknown"; break;
            }
            printf("\n\t stream id=%d type_msg=\"%s\" type=(%d)"
                   "\n\t play_count=(%d) plays=%d \n\t times_count=(%d) times=%s", 
                   lic_info.stream_id, type_msg, lic_info.type, lic_info.plays_count, 
                   lic_info.plays[0], lic_info.times_count, asctime(&lic_info.times[0]));
        }
    } else ret = bdrm_err_fail;
    printf("\n");
    
    return(ret);
}

#endif /* B_HAS_DRM */

struct file_context {
    bool eof;
    bool next;
};

static bplayback_loopmode 
file_end_of_stream(void *context)
{
    struct file_context *file = context;

    file->eof = true;
    return file->next?bplayback_loopmode_pause:bplayback_loopmode_loop;
}

int
main(int argc, const char *argv[])
{
    bresult rc;
    bstream_mpeg mpeg;
    bstream_t stream[B_N_RESOURCES]={NULL};
    bdisplay_t display=NULL;
    bdecode_t decode[B_N_RESOURCES]={NULL};
    bdecode_window_t window[B_N_RESOURCES]={NULL};
    bplayback_file_t file[B_N_RESOURCES]={NULL};
    bplayback_t playback[B_N_RESOURCES]={NULL};
    bplaypump_t playpump[B_N_RESOURCES]={NULL};
    int last_arg;
    const char *fname;
    const char *indxname;
    int tuner_index, display_index, window_index, decode_index, playback_index;
    int channel, cur_arg;
    bool next_channel;
    bool digital;
    btune_state tune_state;
    bool need_settop_init=true;
    bool repeat_play = false;
    struct file_context file_context;
    static const char chn_sep[]="--";
    static const char file_sep[]=":";
 #ifdef LINUX
    struct bplay_pipe pipe = {NULL,-1,false};
#endif
    
#if B_HAS_DRM
    bdrm_t drm[B_N_RESOURCES];
    bdrm_cfg drm_cfg[B_N_RESOURCES];
    bdrm_decryptor_t dcrypt[B_N_RESOURCES];
    void* lc_cbk_ctx = NULL;
#endif /* B_HAS_DRM */

    bplaypump_open_params* params = NULL;
    playpump[0] = NULL;

    bargs_init();

    if (argc<2) {
        usage(argv[0]);
        return 1;
    }
    cur_arg=1;
parse_next_file:
    file_context.next=false;
    file_context.eof=false;
    channel=0;
    do { 
        bool skip = false;

        next_channel = false;
        stream[channel] = NULL; 
        fname=NULL;
        indxname=NULL;
        tuner_index=display_index=window_index=decode_index=playback_index=channel;
        if(!repeat_play) {
            barg_clear(decode_args);
        }
        last_arg = barg_parse(argv, decode_args, cur_arg, argc);
        if (last_arg<0) {
            invalid_arg(-last_arg, argv);
        } else if (last_arg==0) { /* there is should be at least one extra parameter */
            /* do nothing */
        } else {
            if (strcmp(argv[last_arg],chn_sep)==0) {
                next_channel = true;
                last_arg++;
            } else if (strcmp(argv[last_arg],file_sep)==0) {
                file_context.next = true;
                last_arg++;
            } else {
                fname = argv[last_arg++];
                if (argc>last_arg && strcmp(argv[last_arg],chn_sep)!=0 && strcmp(argv[last_arg],file_sep)!=0  ) {
                    indxname = argv[last_arg++];
                    if (!strcmp(indxname, "none"))        
                        indxname  = NULL;
                }

                if (last_arg < argc && strcmp(argv[last_arg],chn_sep)==0) {
                    next_channel = true;
                    last_arg++;
                }  else if (last_arg < argc && strcmp(argv[last_arg],file_sep)==0) {
                    file_context.next = true;
                    last_arg++;
                }
            }
        }

        if (need_settop_init) {
            need_settop_init = false;
            rc = bsettop_init(BSETTOP_VERSION); /* initialize global stuff */
            if (rc!=b_ok) {
                fprintf(stderr, "Initialization error\n");
                return 1;
            }
        }

        bencryption_params_init( &bmpeg_arg.storage.encryption ); 
        bmpeg_arg.storage.encryption.key_length = 64;

        btune_mpeg_init(&mpeg);

        digital = mpeg.video[0].pid || mpeg.audio[0].pid || bmpeg_arg.mpeg_type;

        if (bdecode_arg.skip) {
            skip = *bdecode_arg.skip;
        }
        if (skip) {
            /* do nothing */
        }
        /* open decode. if decode engine not set use the same channel as tuner channel */
        else if (bdecode_arg.channel)
            decode_index = *bdecode_arg.channel;
        else if (btuner_arg.channel)
            decode_index = *btuner_arg.channel;
        else
            decode_index = channel;
        if(!decode[channel]) {
            decode[channel] = bdecode_open(B_ID(decode_index));
        }
        if (!decode[channel]) {
            fprintf(stderr, "Decode %d not avaliable\n", decode_index);
            return 1;
        }

        if (skip) {
            /* do nothing */
        } 
        /**
        * Video Over IP 
        **/
        else if (b_extra_arg.host && digital) {
            bplayback_socket_params *ip_params;
            bplayback_t ip_play;
            bplayback_file_t ip_file;
            bplayback_params play_params;

            ip_play = bplayback_ip_open();
            if (!ip_play) {
                fprintf(stderr, "Can't access video over IP stack\n");
                return 1;
            }

            ip_params = (bplayback_socket_params*)malloc(sizeof(bplayback_socket_params));
            if(!ip_params)
            {
                fprintf(stderr, "Can't allocate memory\n");
                return 1;
            }

            bplayback_socket_params_init(ip_params, ip_play);
            strncpy(ip_params->open_params.ip_addr, b_extra_arg.host, sizeof(ip_params->open_params.ip_addr)-1);
            ip_params->open_params.ip_addr[sizeof(ip_params->open_params.ip_addr)-1]='\0';
            if (b_extra_arg.port) ip_params->open_params.port = *b_extra_arg.port;
            ip_file = bplayback_socket_open(ip_play, ip_params);
            if (!ip_file) {
                fprintf(stderr, "Can't access video over IP server\n");
                free(ip_params);
                return 1;
            }
            free(ip_params);
            bplayback_params_init(&play_params, ip_play);
            playpump[channel] = bplaypump_open(B_ID(playback_index), NULL);
            stream[channel] = bplayback_start(ip_play, playpump[channel], &mpeg, ip_file, &play_params);
            if (!stream[channel]) {
                fprintf(stderr, "Can't start VideoIP playback\n");
                return 1;
            }
        }
#ifdef LINUX
        /* play from a pipe */
        else if (fname && fname[0]=='-' && fname[1]=='\0' && digital) {
            bplaypump_params playpump_params;
            playpump[channel] = bplaypump_open(B_ID(playback_index), params);
            if (!playpump[channel]) {
                fprintf(stderr, "Can't open playpump\n");
                return 1;
            }
            bplaypump_params_init(&playpump_params, playpump[channel]);
            stream[channel] = bplaypump_start(playpump[channel], &mpeg, &playpump_params);
            if (!stream[channel]) {
                fprintf(stderr, "Can't start playback\n");
                return 1;
            }
            pipe.playpump = playpump[channel];
            pipe.fd = 0;
        }
#endif
    /**
        * Playback
        **/
        else if (fname && digital) {
            bplayback_params play_params;

            fprintf(stderr, "Decode from a file\n");

            file[channel] = bplayback_file_open(fname,indxname);
            if (!file[channel]) {
                file[channel] = bplayback_file_open(fname,NULL);
                if (file[channel]) {
                    fprintf(stderr, "Index file not available\n");
                } else {
                    perror(fname);
                    return 1;
                }
            }
            if (bplayback_arg.channel) {
                playback_index = *bplayback_arg.channel;
            } else {
                playback_index = channel;
            }

#if B_HAS_DRM
            drm_cfg[channel].ext_asf_scanning = 0;
            if ((drm[channel] = bdrm_open(fname, &drm_cfg[channel])) != NULL)  { 
                drm_cfg[channel].lic_callback = bdrm_util_license_callback;
                drm_cfg[channel].lic_callback_ctx = lc_cbk_ctx;
                bdrm_init_handlers(drm[channel]);                 
                bdrm_dcrypt_instance(drm[channel], &dcrypt[channel]);                                                        
                memcpy(&mpeg.encryption.key, &dcrypt[channel], sizeof(&dcrypt[channel]));
            }                                                                   
#endif /* B_HAS_DRM */
           
            if(!playpump[channel]) {
                playpump[channel] = bplaypump_open(B_ID(playback_index), params);
            }
            if(!playback[channel]) {
                playback[channel] = bplayback_open();
            }

            if (!playback[channel]) {
                fprintf(stderr, "Can't open playback\n");
                return 1;
            }
            bplayback_params_init(&play_params, playback[channel]);
            if (bmpeg_arg.timestamp) {
                play_params.timestamp_enabled = *bmpeg_arg.timestamp;
                play_params.timestamp_active = *bmpeg_arg.timestamp;
                if (bmpeg_arg.ignore_timestamp) {
                    play_params.timestamp_active = !(*bmpeg_arg.ignore_timestamp);
                }
            }
            play_params.callback_context = &file_context;
            play_params.end_of_stream = file_end_of_stream;
            stream[channel] = bplayback_start(playback[channel], playpump[channel], &mpeg, file[channel], &play_params);
            if (!stream[channel]) {
                fprintf(stderr, "Can't start playback\n");
                return 1;
            }
#if 0
        /* this is an example of how data could be recorded from the playback */
        {
        brecord_file_t file;
        brecord_t record;
        brecord_params params;
        brecpump_t recpump;
        const char *fname="remux.pes";

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
            /* get a record channel, if record channel not set use the same channel as tuner channel */
            recpump = brecpump_open(B_ID(0), NULL);
            if (!recpump) {
                fprintf(stderr, "Recpump not available\n");
                return 1;
            }
            brecord_params_init(&params, record);
            if (brecord_start(record, recpump, stream[channel], file, &params)!=b_ok) {
                fprintf(stderr, "Can't start record\n");
                return 1;
            }
        }
#endif

    } else {
#if BCHP_CHIP == 7401 || BCHP_CHIP == 7400 || BCHP_CHIP == 7118 || BCHP_CHIP == 7403 || BCHP_CHIP == 7405 || BCHP_CHIP == 7335
        stream[channel] = btune_tune(channel, &mpeg, false /* no encoder */, &tune_state);
#else
        if( btuner_arg.channel && *btuner_arg.channel != 7 ) {
            stream[channel] = btune_tune(channel, &mpeg, true, &tune_state);
        }
        else {
            stream[channel] = btune_tune(channel, &mpeg, false /* no encoder when 656 input */ , &tune_state);
        }
#endif
        if (!stream[channel]) {
            return 1;
        }
    }
        if (display==NULL) {
            /* open display, if display not set use the same channel number as tuner */
            if (bdisplay_arg.display) {
                display_index = *bdisplay_arg.display;
            } else {
                display_index = decode_index;
            }
            display = bdisplay_open(B_ID(display_index));
            if (!display) {
                fprintf(stderr, "Display %d not available\n", display_index);
                return 1;
            }

            if (bdisplay_arg.composite || bdisplay_arg.svideo || bdisplay_arg.component || bdisplay_arg.dvi || bdisplay_arg.format || bdisplay_arg.aspect_ratio || bdisplay_arg.content_mode) {
                bdisplay_settings settings;
    
                bdisplay_get(display, &settings);

                if (bdisplay_arg.component || bdisplay_arg.composite || bdisplay_arg.dvi ) {
                    /* remove all existent outputs */
                    settings.component = NULL;
                    settings.composite = NULL;
                    settings.svideo = NULL;
                    settings.dvi = NULL;
                    settings.rf = NULL;
                }
                if (bdisplay_arg.composite) {
                    boutput_composite_t composite=NULL;
                    composite = boutput_composite_open(B_ID(0)); /* open first composite output */
                    if (!composite) {
                        fprintf(stderr, "Composite output not available\n");
                        return 1;
                    }
                    settings.composite = composite;
                }
                if (bdisplay_arg.svideo) {
                    boutput_svideo_t svideo=NULL;
                    svideo = boutput_svideo_open(B_ID(0));
                    if (!svideo) {
                        fprintf(stderr, "S-Video output not available\n");
                        return 1;
                    }
                    settings.svideo = svideo;
                } 
                if (bdisplay_arg.component) {
                    boutput_component_t component=NULL;
                    component = boutput_component_open(B_ID(0));
                    if (!component) {
                        fprintf(stderr, "Component output not available\n");
                        return 1;
                    }
                    settings.component = component;
                }
                if (bdisplay_arg.dvi) {
                    boutput_dvi_t dvi=NULL;
                    boutput_dvi_settings dvi_settings;
                    dvi = boutput_dvi_open(B_ID(0));
                    if (!dvi) {
                        fprintf(stderr, "DVI output not available\n");
                        return 1;
                    }
                    boutput_dvi_get(dvi, &dvi_settings);
                    dvi_settings.hdcp = *bdisplay_arg.dvi;
                    boutput_dvi_set(dvi, &dvi_settings);
                    settings.dvi = dvi;
                }
                if (bdisplay_arg.format) {
                    bvideo_format_settings s;
                    settings.format = *bdisplay_arg.format;
                    
                    bvideo_get_format_settings(settings.format, &s);
                    if (s.height > 576 || !s.interlaced) {
                        /* this is HD */
                        settings.svideo = NULL;
                        settings.composite = NULL;
                        settings.rf = NULL;
                    }
                }
                if (bdisplay_arg.aspect_ratio) {
                    settings.aspect_ratio = *bdisplay_arg.aspect_ratio;
                }
                if (bdisplay_arg.content_mode) {
                    settings.content_mode = *bdisplay_arg.content_mode;
                }
                rc = bdisplay_set(display, &settings);
                if (rc!=b_ok) {
                    fprintf(stderr, "Requested display configuration not supported\n");
                    return 1;
                }
            }   
        }

        if (bwindow_arg.index) {
            window_index = *bwindow_arg.index;
        } else {
            window_index = decode_index;
        }
        if (!window[channel]) {
            window[channel] = bdecode_window_open(B_ID(window_index), display);
        }
        if (!window[channel]) {
            fprintf(stderr, "Decode window %d not available\n", window_index);
            return 1;
        }
    
        if (bvolume_arg.mute || bvolume_arg.volume) {
            baudio_volume vol;

            bdisplay_get_dac_audio_volume(display, &vol);
            if (bvolume_arg.mute) vol.muted = true;
            if (bvolume_arg.volume) vol.right = vol.left = *bvolume_arg.volume;

            bdisplay_set_dac_audio_volume(display, &vol);
        }   

        if (!stream[channel]) {
            fprintf(stderr, "Stream is not available\n");
            return 1;
        }

        /* start vbi if required */
        if (b_arg.vbi && *b_arg.vbi) {
            bstream_vbi_settings vbi_cfg;

            bstream_vbi_get_settings(stream[channel], &vbi_cfg);
            vbi_cfg.cc.display = display;
            bstream_vbi_set_settings(stream[channel], &vbi_cfg);
        }

        /* display program */
        rc = bdecode_start(decode[channel], stream[channel], window[channel]); /* start decode of the stream from the stream using specified decode engine displaying on the specified  window */
        if (rc!=b_ok) {
            fprintf(stderr, "Can't start decode\n");
            return 1;
        }
        if (channel==0) {
            /* start VBI for the first decoded stream */
            bstream_vbi_settings vbisettings;
            bstream_vbi_get_settings(stream[channel], &vbisettings);
            vbisettings.cc.display = display;
            bstream_vbi_set_settings(stream[channel], &vbisettings);
        }

        if (bwindow_arg.window_rect ||
            bwindow_arg.x || bwindow_arg.y || bwindow_arg.width || bwindow_arg.height ||
            bwindow_arg.clip_x || bwindow_arg.clip_y || bwindow_arg.clip_width || bwindow_arg.clip_height
            )
        {
            bdecode_window_settings settings;

            rc = bdecode_window_get(window[channel], &settings);
            if (rc==b_ok) {
                if (bwindow_arg.window_rect) {
                    /* coverity[secure_coding] */ /* coverity[unchecked_value] */
                    sscanf(bwindow_arg.window_rect, "%d,%d,%u,%u",
                    &settings.position.x,
                    &settings.position.y,
                    &settings.position.width,
                    &settings.position.height);
                } else {
                    if (bwindow_arg.x) settings.position.x = *bwindow_arg.x;
                    if (bwindow_arg.y) settings.position.y = *bwindow_arg.y;
                    if (bwindow_arg.width) settings.position.width = *bwindow_arg.width;
                    if (bwindow_arg.height) settings.position.height = *bwindow_arg.height;
                }
                if (bwindow_arg.clip_x) settings.cliprect.x = *bwindow_arg.clip_x;
                if (bwindow_arg.clip_y) settings.cliprect.y = *bwindow_arg.clip_y;
                if (bwindow_arg.clip_width) settings.cliprect.width = *bwindow_arg.clip_width;
                if (bwindow_arg.clip_height) settings.cliprect.height = *bwindow_arg.clip_height;

                printf("decode_window %d,%d,%d,%d[%d,%d,%d,%d]\n",
                    settings.position.x,settings.position.y,settings.position.width,settings.position.height,
                    settings.cliprect.x,settings.cliprect.y,settings.cliprect.width,settings.cliprect.height
                );
                rc = bdecode_window_set(window[channel],  &settings);
            }
            if (rc!=b_ok) {
                fprintf(stderr, "Can't set decode window parameters\n");
            }
        }
        cur_arg = last_arg;
        channel++;
    } while(next_channel);


#ifdef __vxworks
    printf("Decode started\n");
    while (1) {
        BKNI_Sleep(1000);
    }
#else
#if 0
    printf("Decode started\nPress any key to stop decode\n");
    getchar();
#else
    {
        bdecode_status cur,prev;
        unsigned long i;

        bdecode_get_status(decode[0],&prev);
        for(i=1;;i++) {
#ifdef LINUX
            if (pipe.playpump) {
                bplay_pipe_run(&pipe);
            } else
#endif
            sleep(1);
            bdecode_get_status(decode[0],&cur);
            printf("[%lu] video: %ux%u%c pts %lu/%ld/%lu stc %lu/%lu/%lu %d buf %lu/%lu audio: pts %lu/%ld/%lu stc %lu/%lu/%lu %d buf %lu/%lu <%d/%d>",  i,
                    cur.source_width, cur.source_height,cur.interlaced?'i':'p',
                    cur.video_pts, cur.video_pts-prev.video_pts, cur.video_pts/i, 
                    cur.video_stc, cur.video_stc-prev.video_stc, cur.video_stc/i, (int)(cur.video_stc - cur.video_pts),
                    cur.video_fifo_depth, cur.video_fifo_size,
                    cur.audio_pts, cur.audio_pts-prev.audio_pts, cur.audio_pts/i, 
                    cur.audio_stc, cur.audio_stc-prev.audio_stc, cur.audio_stc/i, (int)(cur.audio_stc - cur.audio_pts),
                    cur.audio_fifo_depth, cur.audio_fifo_size,
                    (int32_t)cur.video_pts-(int32_t)cur.audio_pts,(int32_t)prev.video_pts-(int32_t)prev.audio_pts
                    );
            if(playpump[0]) {
                bplaypump_status status;
                if (bplaypump_get_status(playpump[0], &status)==b_ok) {
                    printf(" play %lu/%lu/%lu/%lu", status.fifo_depth, status.fifo_size, (unsigned long)status.bytes_played,(unsigned long)(8*status.bytes_played/i));
                }
            }
            printf("\n");
            prev=cur;
            if ( ((b_extra_arg.play_time && (int)i>*b_extra_arg.play_time) || file_context.eof) && file_context.next) {
                while(channel-->0) {
                    bstream_close(stream[channel]);
                    if(playback[channel]) {
                        bplayback_stop(playback[channel]);
                    }
                }
                repeat_play = true;
                goto parse_next_file;
            }
        }
    }
#endif

    while(channel-->0) {
        bstream_close(stream[channel]);
        bdecode_window_close(window[channel]);
        bdecode_close(decode[channel]);
#if B_HAS_DRM
            bdrm_close(drm[channel]); 
            dcrypt[channel] = NULL; drm[channel] = NULL; 
#endif /* B_HAS_DRM */

        }
    
    bdisplay_close(display);
    bsettop_uninit();
#endif

    return 0;
}

