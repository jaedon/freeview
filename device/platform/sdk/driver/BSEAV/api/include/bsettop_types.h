/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_types.h $
 * $brcm_Revision: 66 $
 * $brcm_Date: 6/30/11 5:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_types.h $
 * 
 * 66   6/30/11 5:15p erickson
 * SW7346-299: fix NEXUS_VideoFrameRate_e7_493 typo
 * 
 * 65   6/3/11 4:05p mphillip
 * SW7425-417: Update HDMI audio output modes
 * 
 * 64   9/10/09 12:15p vsilyaev
 * SW7335-529: Use bmedia_types for container, video and audio types
 * 
 * 63   9/1/09 4:06p vsilyaev
 * SW3548-2442: Added support for AMR audio
 * 
 * 62   7/30/09 6:46p vsilyaev
 * PR 57220: Added 7.943 and 14.985 frame rates
 * 
 * 61   7/15/09 7:21p vsilyaev
 * PR 55653: Added WAV format
 * 
 * 60   5/11/09 1:38p vsilyaev
 * PR54955: NW record of TTS content: add flag to socket structure to
 * indicate if sending/receiving a TTS stream
 * 
 * PR54955/1   5/11/09 1:18p ssood
 * PR54955: NW record of TTS content: add flag to socket structure to
 * indicate if sending/receiving a TTS stream
 * 
 * 59   3/18/09 12:25p jgarrett
 * PR 53340: Adding 1080p50
 *
 * 58   3/4/09 7:47p jgarrett
 * PR 52269: Adding XMP support
 *
 * 57   2/4/09 10:07a vsilyaev
 * PR 46190: Added PCM audio type
 *
 * Trinity_Xmp_Support/7   2/24/09 11:19a prasadv
 * Add XMP support. Code cleanup.
 *
 * 56   12/9/08 10:27a vsilyaev
 * PR 43467: Added DRA audio
 *
 * dra/1   8/7/08 4:29p gmohile
 * PR 43467 : Added dra support
 *
 * 55   7/21/08 3:46p jgarrett
 * PR 44143: changing default aac-he format
 *
 * 54   7/11/08 4:19p vsilyaev
 * PR 44727: Added softcoded types
 *
 * 53   6/25/08 12:01p jgarrett
 * PR43426: PR43426 : Add 7043 Transcode support
 *
 * PR43426/1   6/24/08 6:24p shyam
 * PR43426 : Add 7043 Transcode support
 *
 * 52   3/6/08 2:01p vsilyaev
 * PR 36248: Pass mpeg parameters into the brecord_file_open_network and
 * bfile_posix_out_network
 *
 * 51   2/21/08 1:43p vsilyaev
 * PR 39818: Added bstream_mpeg_type_mkv type
 *
 * 50   1/21/08 10:47a jgarrett
 * PR 38591: Increasing MAX_VIDEO_CODECS
 *
 * 48   8/22/07 5:29p jgarrett
 * PR 32044: Merging to main branch
 *
 * 7405_pip_support/2   8/21/07 5:43p katrep
 * PR34204: Synced with latest on this branch.
 *
 * 47   8/9/07 5:47p jgarrett
 * PR 33830: Adding network record prototypes
 *
 * 7405_pip_support/1   8/21/07 2:38p katrep
 * PR34204: Added mosaic mode support for 7405.
 *
 * PR32044/1   6/26/07 5:54p jgarrett
 * PR 32044: Adding Mosaic Mode APIs
 *
 * 46   7/6/07 11:50a vsilyaev
 * PR 32813: Added type for unknown/invalid stream format and codec type
 *
 * 45   6/13/07 10:31a vsilyaev
 * PR 32119: Added LPCM DVD audio type
 *
 * 44   6/1/07 4:08p vsilyaev
 * PR 31728: Added baudio_format_aac_plus_laos audio type. Fixed typo
 *
 * 43   6/1/07 4:07p vsilyaev
 * PR 31728: Added baudio_format_aac_plus_laos audio type
 *
 * 42   5/18/07 12:30a vsilyaev
 * PR 31188: Added FLV stream type
 *
 * 41   5/16/07 3:10p vsilyaev
 * PR30807:  Need an API to read HDMI device tree physical address.
 *
 * PR30807/1   5/16/07 1:49p dlwin
 * PR 30807: Need an API to read HDMI device tree physical address.
 *
 * 40   5/1/07 11:22a vsilyaev
 * PR 28631: Added MP4 container format
 *
 * 39   4/23/07 12:20p vsilyaev
 * PR 29948: Added WMA PRO audio codec type
 *
 * 38   3/23/07 11:21a vsilyaev
 * PR 28320: Added type for MPEG1 System stream
 *
 * 37   3/1/07 7:09p vsilyaev
 * PR 25701: Added support for the AVI container
 *
 * 36   1/29/07 5:14p vsilyaev
 * PR 25701: Added DivX video codec
 *
 * 35   1/18/07 11:36a vsilyaev
 * PR 27186: Differentiate between parser and input bands. AKA, fixing
 * bmessage from playback source
 *
 * 34   11/30/06 7:21p jgarrett
 * PR 24264: Adding comment about modifying bvideo_format
 *
 * 33   10/5/06 6:00p jgarrett
 * PR 24743: Adding bvideo_codec_mpeg4_part2
 *
 * 32   9/18/06 12:47p erickson
 * PR24264: add 1080p and other HD formats to Settop API
 *
 * 31   8/23/06 10:20a erickson
 * PR23714: add h263 support to Settop API
 *
 * 30   7/21/06 5:09p vsilyaev
 * PR 21107: Added support for the VC1 SP and MP
 *
 * 29   6/12/06 11:18a erickson
 * PR21646: added bvideo_frame_rate
 *
 * 28   5/30/06 12:04p vsilyaev
 * PR 21882: Removed C++ comment
 *
 * 27   5/30/06 11:44a mphillip
 * PR14698: Fix previous check-in comment (oops)
 *
 * 26   5/30/06 11:43a mphillip
 * PR14698: Changed comment-styles to fix a compile-time warning
 *
 * 25   5/26/06 2:45p dlwin
 * PR 21105: Add support WMA codec.
 *
 * 24   5/25/06 7:40p jgarrett
 * PR 21463: Fixing PAL enum values
 *
 * 23   5/24/06 9:24a erickson
 * PR21463: added full set of pal formats
 *
 * 22   5/2/06 2:45p jgarrett
 * PR 21291: Adding hdmi capabilities and hdmi audio type
 *
 * 21   5/1/06 2:55p erickson
 * PR19661: added 1080p_30hz display support
 *
 * 20   4/7/06 3:50p vsilyaev
 * PR 20680: Added bfile library
 *
 * 19   3/28/06 11:38a vsilyaev
 * PR 20308: Added ASF stream type
 *
 * 17   3/14/06 6:54p dlwin
 * PR 20206: Removed B_HAS_IP use.  Now it always incude
 *
 * 16   2/13/06 2:47p dlwin
 * PR 19628: Add in support DTS HD audio format.
 *
 * 15   1/20/06 8:40a dlwin
 * PR 19169: Corrected spelliing of Blu-Ray and changed Blu-Ray to a value
 * of 0x84.
 *
 * 14   1/19/06 6:03p dlwin
 * PR 19169: Added baudio_format_lpcm_hddvd audio type and changed
 * baudio_format_lpcm to baudio_format_lpcm_blueray.
 *
 * 13   12/29/05 5:04p dlwin
 * PR 18840: Added support for LPCM
 *
 * 12   12/13/05 4:53p erickson
 * PR18439: added bdecode_acquire_capture_buffer et al to return capture
 * buffer to application
 *
 * 11   11/2/05 6:19p vsilyaev
 * PR 16982: split bsettop_impl.h to bsettop_impl.h and bsettop_impl_cfg.h
 *
 * 10   10/13/05 3:12p erickson
 * PR17541: added more audio and video codecs for 7401 and beyond
 *
 * 9   10/12/05 4:05p erickson
 * PR15241: added bvideo_codec_none for analog and other external video
 * sources
 *
 * 8   8/24/05 4:15p erickson
 * PR16678: WIN32_WCE merge
 *
 * PROD_WinCE50/1   8/17/05 6:48p arbisman
 * PR16678: Add support for WinCE
 *
 * 5   7/27/05 3:28p vsilyaev
 * PR 16385: Merge IP record feature
 *
 * PROD_IPSTB_REFSW_Devel/2   7/15/05 4:05p wesleyl
 * PR15072: Add brecord_ip_t definition for IP record
 *
 * PROD_IPSTB_REFSW_Devel/1   7/10/05 2:27p wesleyl
 * PR15072: Add IP playback and IP record typedef
 *
 * 3   6/2/05 3:59p vsilyaev
 * PR 15740: Fixed test for id before converting it to bobject_t .
 *
 * 2   4/27/05 1:12p erickson
 * PR15075: redefine off_t to 64 bits for vxworks
 *
 * 1   2/7/05 7:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/44   12/29/04 7:24p marcusk
 * PR13701: Updated with support for H.264 Stream Type (0xB1)
 *
 * Irvine_BSEAVSW_Devel/43   10/28/04 7:44p vsilyaev
 * PR 13097: Added video coding type.
 *
 * Irvine_BSEAVSW_Devel/42   7/6/04 3:53p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/41   6/29/04 11:28a erickson
 * PR11135: updated deprecation notices for settop api
 *
 * Irvine_BSEAVSW_Devel/40   6/24/04 10:27a erickson
 * PR11135: converted to generic bsettop_callback for playpump and recpump
 *
 * Irvine_BSEAVSW_Devel/39   6/2/04 2:43p erickson
 * PR11204: changed/removed deprecated settop api functions
 *
 * Irvine_BSEAVSW_Devel/37   5/25/04 11:43a erickson
 * PR11135: fixed docs on audio level. It's not db.
 *
 * Irvine_BSEAVSW_Devel/36   4/15/04 12:50p erickson
 * PR9801: modified value of baudio_format_mpeg to match 13818-1
 * definition
 *
 * Irvine_BSEAVSW_Devel/35   4/7/04 1:18a vsilyaev
 * PR 10508: Added linuxkernel target.
 *
 * Irvine_BSEAVSW_Devel/34   3/3/04 4:07p vsilyaev
 * PR 8850: Added note on volume handling for mono devices.
 *
 * Irvine_BSEAVSW_Devel/33   1/27/04 5:57p vsilyaev
 * PR 9497: Added vxworks handling
 *
 * Irvine_BSEAVSW_Devel/32   1/6/04 1:47p vsilyaev
 * PR 8850: Fixed warnings in no OS build.
 *
 * Irvine_BSEAVSW_Devel/31   1/6/04 12:07p vsilyaev
 * PR 8850: Fixed compile warnings for non OS.
 *
 * Irvine_BSEAVSW_Devel/Irvine_BSEAVSW_Devel_7038/1   12/4/03 1:27p erickson
 * PR8850: initial Brutus support on 7038
 *
 * Irvine_BSEAVSW_Devel/29   12/4/03 3:52p erickson
 * PR8734: don't allow non-MPEG audio for mpegaudio 1. Hardcode it to 1st
 * app only.
 *
 * Irvine_BSEAVSW_Devel/28   11/19/03 4:57p erickson
 * PR8583: resolved some naming issues, especially bvideo_format
 *
 * Irvine_BSEAVSW_Devel/27   11/17/03 3:09p erickson
 * PR8612: added tuner to band so that a digital stream can still remember
 * which downstream it came from
 *
 * Irvine_BSEAVSW_Devel/26   11/7/03 5:02p erickson
 * PR8570: use bstd_defs.h in order to support linux, vxworks, etc.
 *
 * Irvine_BSEAVSW_Devel/25   11/6/03 5:12p erickson
 * PR8570: initial vxworks migration
 *
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 5:05p erickson
 * initial vxworks migration
 *
 * Irvine_BSEAVSW_Devel/24   11/4/03 2:42p erickson
 * documentation changes
 *
 * Irvine_BSEAVSW_Devel/23   10/31/03 11:58a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/22   10/29/03 5:04p erickson
 * PR8253: enable spdif volume contorl
 *
 * Irvine_BSEAVSW_Devel/21   10/28/03 11:26a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/20   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/19   9/30/03 1:13a erickson
 * updated docjet documentation
 *
 * Irvine_BSEAVSW_Devel/18   9/24/03 11:51a erickson
 * initial pcm play implementation
 *
 * Irvine_BSEAVSW_Devel/17   9/18/03 8:46a erickson
 * made brecord_session_t private
 *
 * Irvine_BSEAVSW_Devel/16   9/17/03 5:04p erickson
 * updated documentation for DocJet
 *
 * Irvine_BSEAVSW_Devel/15   9/9/03 5:25p erickson
 * needed to expand BID_MAX_INDEX to handle our encoded streamer and
 * playback ids.
 * see bstreamer_attach.
 *
 * Irvine_BSEAVSW_Devel/14   8/18/03 11:02a erickson
 * initial bringup for brutus 7320, still in progress
 *
 * Irvine_BSEAVSW_Devel/13   8/12/03 9:43a erickson
 * changes after pre-review meeting
 *
 * Irvine_BSEAVSW_Devel/12   7/30/03 2:36p vsilyaev
 * Replace bsettop_event_t with bsettop_event_t,
 * Include some external files which defines imported data types.
 *
 * Irvine_BSEAVSW_Devel/11   7/30/03 2:08p erickson
 * removed all return value comments because we're standardized and they
 * add nothing
 *
 * Irvine_BSEAVSW_Devel/10   7/30/03 1:43p erickson
 * added comments to define macro meanings and usage
 * removed no-info comments. we can add them back in as we have meaningful
 * information.
 *
 * Irvine_BSEAVSW_Devel/9   7/29/03 6:03p erickson
 * more rework
 *
 * Irvine_BSEAVSW_Devel/8   7/28/03 4:43p erickson
 * more comment work
 *
 * Irvine_BSEAVSW_Devel/6   7/25/03 8:14p vsilyaev
 * Moved declaration from the bsettop.h
 *
 * Irvine_BSEAVSW_Devel/5   7/25/03 4:22p erickson
 * in the middle of rework, does not compile
 *
 * Irvine_BSEAVSW_Devel/4   7/9/03 5:19p erickson
 * rework for new bband_t model
 *
 * Irvine_BSEAVSW_Devel/3   6/24/03 5:30p vsilyaev
 * Use brecord_file_t and bplayback_file_t instead bfile_in and bfile_out.
 *
 * Irvine_BSEAVSW_Devel/2   6/16/03 4:42p erickson
 * api work
 *
 * Irvine_BSEAVSW_Devel/1   6/13/03 3:09p vsilyaev
 * SetTop reference API.
 *
 *
 ***************************************************************************/
#ifndef BSETTOP_TYPES_H__
#define BSETTOP_TYPES_H__

#include "bstd_defs.h"

#include "bfile_types.h"

#include "bmedia_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=*************************
Collection of handles, structures and other types that are used across
the Settop API.

Types that are only used in one module are defined in that module's
header file.
****************************/

/**
* Summary:
* Private definition using in B_ID macro.
* Description:
* The purpose of BID_MAX_INDEX is to make sure that you only pass an
* integer into B_ID() and it enables B_ID_IS_INDEX() to differentiate
* between pointers and integers. The idea is that a pointer will never have
* a value less than BID_MAX_INDEX and that you will never need an index
* >= BID_MAX_INDEX.
* NOTE: it's not actually MAX as much as TOTAL or FIRST_INVALID_INDEX.
**/
#define BID_MAX_INDEX 200

/*
Summary:
    Create an object_id from integer for selecting resources to open.
Description:
    It increments the id internally so that B_ID(0) is valid but different from
    a NULL bobject_t.
*/
#define B_ID(x) (((x)>=BID_MAX_INDEX||(((int)x)<0)) ? NULL: ((bobject_t)((x)+1)))

/*
Summary:
    Handle to select a particular resource when opening.
Description:
    The reason we use a typedef instead of an integer index is to allow
    for registry-based implementations in the future. For today, B_ID(index) is
    the only supported method of obtaining bobject_t's.
*/
typedef struct bobject_id_impl *bobject_t;

/*
Summary:
    Standard error code for settop api.
Description:
    Every settop api function returns either:
    - For open functions, a handle which is NULL on error.
    - For close, structure init and never-failing get functions have void return type.
    - For all other functions, a bresult which is non-zero (!= b_ok) on error.
*/
typedef enum bresult {
    b_ok=0,
    berr_out_of_memory=1,
    berr_invalid_parameter=2,
    berr_not_supported=3,
    berr_not_available=4,
    berr_busy=5,
    berr_external_error=6,
    berr_invalid_state=7,
	berr_timeout=8
} bresult;


/**
Summary:
Generic callback from the Settop API into user code.
Description:
Callbacks are used for notification to the user.

There are some general rules for the use of callbacks.

1) You should never block or do extensive work in a callback. They should always
execute quickly. If you don't execute quickly, you may impact the performance of
the entire system.

2) You can make calls back into the Settop API, but you should make a limited number
of calls. In general, you should query state and not change state.
The exact calls recommended or allowed depends on the particular callback
and it will be documented with that callback. Any calls back into the Settop API from
a callback introduce re-entrancy, and it's hard to predict what will happen if any
call is made.

The value of context is always user-defined. It is specified when the user specifies the callback
function. The user is responsible to make sure the context pointer allows
the user program to access whatever handles or state data is needed in the callback.
**/
typedef void (*bsettop_callback)(void *context);

/*
Summary:
    Handle returned by btuner_open().
*/
typedef struct btuner *btuner_t;

/*
Summary:
    Handle returned by btuner_linein_open().
*/
typedef struct btuner_linein *btuner_linein_t;

/*
Summary:
    Settop rectangular geometry object. Used by bdisplay and bgraphics.
*/
typedef struct bsettop_rect {
    int x;
    int y;
    unsigned width;
    unsigned height;
} bsettop_rect;

/*
Summary:
    bband_t represents a digital transport input band. It encompasses both the transport
    input band and the transport type ( the type of data current on the input band).

Description:
    A careful distinction should be made between input band and parser band. An input band
    is an external transport input to the chip. This is often hard-coded to certain frontend
    configurations per platform.

    There is a dynamic mapping between input band and parser band. the input band is often mapped
    directly to the parser band of the same number, but this is not required. An input band
    can be mapped to more than one parser band. This is done by making multiple calls to
    bstream_open with the same bband_t.
*/
typedef struct bband *bband_t;


/*
Summary:
    A stream is either a digital or analog program which can be decoded or recorded.
Description:
    Streams are produced by one of the following:
        - tuning linein or rf (btuner_tune_rf or btuner_tune_linein)
        - tuning digital and selecting mpeg params (btuner_tune_qpsk/qam and bstream_set_mpeg)
        - starting playback (bplay_start)
        - direct transport input (bstream_set)
*/
typedef struct bstream *bstream_t;

/*
Summary:
    Handle returned by bplayback_open().
*/
typedef struct bplayback *bplayback_t;

/*
Summary:
    Handle returned by bplayfile_file_open.
Description:
  This is an object to retreive data from the media.
  This object has support for two independent data flows.
  Usually it implemented by read(2) and seek(2) functions
*/
typedef struct bplayback_file *bplayback_file_t;

/*
Summary:
    Handle returned by brecord_open().
*/
typedef struct brecord *brecord_t;

/*
Summary:
    Handle returned by bplayback_ip_open().
*/
typedef struct bplayback_ip *bplayback_ip_t;


/*
Summary:
    Handle returned by brecord_ip_open().
*/
typedef struct brecord_ip *brecord_ip_t;



/*
Summary:
  Handle returned by brecord_file_open().
Description:
  This is an object to store date to the media. Usually it implemented by write(2) function.
  This object has support for two independent data flows.
*/
typedef struct brecord_file *brecord_file_t;

/*
Summary:
    Handle returned by bdecode_open().
*/
typedef struct bdecode *bdecode_t;

/*
Summary:
    Handle returned by bdecode_window_open().
*/
typedef struct bdecode_window *bdecode_window_t;

/*
Summary:
    Handle returned by bdecode_detach_audio().
*/
typedef struct baudio_decode *baudio_decode_t;

/*
Summary:
    Handle returned by bdecode_detach_video().
*/
typedef struct bvideo_decode *bvideo_decode_t;

/*
Summary:
    Handle returned by bdisplay_open().
*/
typedef struct bdisplay *bdisplay_t;

/*
Summary:
    Handle returned by buser_input_open().
*/
typedef struct buser_input *buser_input_t;

/*
Summary:
    Handle returned by buser_output_open().
*/
typedef struct buser_output *buser_output_t;

/*
Summary:
  bencode_t is a handle that refers to an encoder chip.
Description:
  An encoder may have 1 or more channels.
  A system may have 0, 1 or more encoders.
*/
typedef struct bencode *bencode_t;


/*
Summary:
    Handle returned by btransode_open().
*/
typedef struct btranscode *btranscode_t;


/**
Summary:
Handle for the playpump api.
**/
typedef struct bplaypump *bplaypump_t;

/*
Summary:
    Graphics engine handle returned by bgraphics_open.
*/
typedef struct bgraphics *bgraphics_t;

/*
Summary:
    Surface handle returned by bgraphics_create_surface.
Description:
    Each graphics engine can have many surfaces.
*/
typedef struct bsurface *bsurface_t;

/*
Summary:
Maximum baudio_level value.
*/
#define BAUDIO_LEVEL_MAX 100
/*
Summary:
Minimum baudio_level value.
*/
#define BAUDIO_LEVEL_MIN -100

/*
Summary:
    Audio level used in baudio_volume.
Description:
    Max value is BAUDIO_LEVEL_MAX. Min value is BAUDIO_LEVEL_MIN.
*/
typedef int baudio_level;

/*
Summary:
   Data structure for audio volume.
Description:
    Volume is not measured in units of dB. It is simply a range between the min and max
    audio level of the device. Min is always a value very close to mute.
    If the device has attenuation only, max will be no attenuation.
    If the device has amplication, max will be maximum amplification.

    The baudio_level values can range from BAUDIO_LEVEL_MIN (the softest) to
    BAUDIO_LEVEL_MAX (the loudest). The baudio_level is usually converted to the dB
    range of the device by a linear conversion from MIN/MAX to the min/max dB of the device.

    If you want a specific dB setting on a device, you may have to modify the settop api
    implementation.

    The actual audio level which is produced on the output will vary based on the hardware
    and the input source. Your application will have to be tuned if you want to normalize
    the output levels.

    If some device supports only 'mono' type of control, user shall expect the following behavior:
      - API will return structure where left volume is equal to the right volume
      - when API receives structure from the user, only value of the left volume is used, right is ignored
*/
typedef struct baudio_volume {
    baudio_level left;
    baudio_level right;
    bool muted;
} baudio_volume;


/*
Summary:
    Video formats
*/
typedef enum bvideo_format {
/*
Developers -- Please be very careful when modifying ANYTHING in this enum.  There are
              multiple places in the code where the order and number of elements in this
              enum define array structures.  Any changes here must be made in those locations
              as well.  Search for bvideo_format_count to find locations.
*/
    bvideo_format_ntsc,              /* NTSC Interlaced */
    bvideo_format_ntsc_japan,        /* Japan NTSC, no pedestal level */
    bvideo_format_pal_m,             /* PAL Brazil */
    bvideo_format_pal_n,             /* PAL_N */
    bvideo_format_pal_nc,            /* PAL_N, Argentina */
    bvideo_format_pal_b,            /* Australia */
    bvideo_format_pal_b1,           /* Hungary */
    bvideo_format_pal_d,            /* China */
    bvideo_format_pal_d1,           /* Poland */
    bvideo_format_pal_g,            /* Europe. Same as bvideo_format_pal. */
    bvideo_format_pal = bvideo_format_pal_g,     /* PAL Europe */
    bvideo_format_pal_h,            /* Europe */
    bvideo_format_pal_k,            /* Europe */
    bvideo_format_pal_i,            /* U.K. */
    bvideo_format_secam,             /* SECAM III B6 */
    bvideo_format_480p,              /* NTSC Progressive (27Mhz) */
    bvideo_format_576p,              /* HD PAL Progressive 50Hz for Australia */
    bvideo_format_1080i,             /* HD 1080 Interlaced */
    bvideo_format_1080i_50hz,        /* European 50Hz HD 1080 */
    bvideo_format_1080p,             /* HD 1080 Progressive, 60Hz */
    bvideo_format_1080p_24hz,        /* HD 1080 Progressive, 24Hz */
    bvideo_format_1080p_25hz,        /* HD 1080 Progressive, 25Hz */
    bvideo_format_1080p_30hz,        /* HD 1080 Progressive, 30Hz */
    bvideo_format_1080p_50hz,        /* HD 1080 Progressive, 50Hz */
    bvideo_format_1250i_50hz,        /* HD 1250 Interlaced, 50Hz */
    bvideo_format_720p,              /* HD 720 Progressive */
    bvideo_format_720p_50hz,         /* HD 720p 50Hz for Australia */
    bvideo_format_720p_24hz,         /* HD 720p 24Hz */
    bvideo_format_vesa,              /* PC monitor. Requires width, height and refresh rate parameters
                                            in bdisplay_settings. */
    bvideo_format_count              /* Total number of video formats -- must be last */
} bvideo_format;


#define BSETTOP_MAX_VIDEO_CODECS 10      /* PLEASE UPDATE THIS IF YOU ADD VIDEO CODECS!!! */

/*
Summary:
Video frame rate (frames per second)
Description:
This is used for both display and source frame rates.
*/
typedef enum bvideo_frame_rate {
    bvideo_frame_rate_unknown =    0,
    bvideo_frame_rate_7_493   =  749, /* 7.493 */
    bvideo_frame_rate_14_985  = 1498, /* 14.985 */
    bvideo_frame_rate_23_976  = 2397,
    bvideo_frame_rate_24      = 2400,
    bvideo_frame_rate_25      = 2500,
    bvideo_frame_rate_29_97   = 2997,
    bvideo_frame_rate_30      = 3000,
    bvideo_frame_rate_50      = 5000,
    bvideo_frame_rate_59_94   = 5994,
    bvideo_frame_rate_60      = 6000
} bvideo_frame_rate;

/**
Summary:
Test if the audio format is MPEG 1/2 layer 1/2 audio.

Description:
Our decoders handle MPEG 1/2 layer 1/2 using a single data type.
For this reason, we need some single test for all these formats in PSI information.
**/
#define BAUDIO_FORMAT_MPEG(FORMAT) ((FORMAT) == 0 || (FORMAT) == 3 || (FORMAT) == 4)

/*
Summary:
    BTSC program selection
*/
typedef enum boutput_rf_btsc_mode {
    boutput_rf_btsc_mode_mono,
    boutput_rf_btsc_mode_stereo,
    boutput_rf_btsc_mode_sap,
    boutput_rf_btsc_mode_sapmono
} boutput_rf_btsc_mode;

/* Summary:
    This is a enum lists the type of socket addresses that can be used in bsocket_params structure.
   Description:
      This enum will specify the structure that will be used in bsocket_params.addr union.
*/
typedef enum bsocket_address_type {
    bsocket_address_type_ipv4,
    bsocket_address_type_url
} bsocket_address_type;

/*
Summary:
    This is a common structure to pass socket params to any settop api modules that may
    need it.
Description:
    You must specify what bscoket_address_type is contained in the particular bsocket_params
    struct. If socket address type is bsocket_address_type_ipv4 then the ipv4 structure will
    be used. If bsocket_address_type_url is specified then char url[128] will be used.
*/
typedef struct bsocket_params
{
   bsocket_address_type addr_type;
   union
   {
      char url[128];
      struct
      {
         uint8_t host[4];
         uint16_t port;
      } ipv4;
      /* Eventually add ipv6, ... */
   } addr;
   bool timestamp_enabled; /* indicates if this socket is sending or receiving MPEG2 TS packets w/ timestamps */
} bsocket_params;


/*
Summary:
    HDMI Audio Selection
*/
typedef enum boutput_hdmi_audio_mode
{
    boutput_hdmi_audio_mode_pcm,        /* Stereo PCM */
    boutput_hdmi_audio_mode_pcm_2ch=boutput_hdmi_audio_mode_pcm,
    boutput_hdmi_audio_mode_compressed, /* AC3 Compressed */
    boutput_hdmi_audio_mode_pcm_6ch,    /* 6 channel PCM */
    boutput_hdmi_audio_mode_ac3_plus,   /* Dolby Digital Plus (AC3+ or DDP) audio */
    boutput_hdmi_audio_mode_dts,        /* DTS */
    boutput_hdmi_audio_mode_dts_hd,     /* DTS HD, decodes only DTS part of DTS-HD stream */
    boutput_hdmi_audio_mode_dts_legacy, /* DTS HD, legacy mode (14 bit), uses legacy frame-sync */
    boutput_hdmi_audio_mode_count       /* Count of modes */
} boutput_hdmi_audio_mode;

/*
Summary:
    HDMI Rx EDID Info
    This is structure for storing information from HDMI Rx EDID
*/
typedef struct boutput_hdmi_rx_edid_info
{
    /* Please HDMI Specification 1.3a under Section 8.6 for details of Physical Addres.
     * Each node (A..D) represent an attached HDMI device on HDMI device tree. */
    uint8_t phys_addr_A;                    /* Physical Address for HDMI node A */
    uint8_t phys_addr_B;                    /* Physical Address for HDMI node B */
    uint8_t phys_addr_C;                    /* Physical Address for HDMI node C */
    uint8_t phys_addr_D;                    /* Physical Address for HDMI node D */
} boutput_hdmi_rx_edid_info;

#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_TYPES_H__*/

