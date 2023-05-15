/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_config.h $
 * $brcm_Revision: 16 $
 * $brcm_Date: 5/2/12 6:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_config.h $
 * 
 * 16   5/2/12 6:46p jgarrett
 * SW7425-2802: Removing RAP_XXX build variables for audio post processing
 * in favor of runtime detection
 * 
 * 15   4/25/12 11:14a sgadara
 * SW7358-272: [7358] Support AVL on Brutus
 * 
 * 14   3/13/09 3:13p jgarrett
 * PR 49827: Merge to main branch
 * 
 * PR49827/3   3/3/09 1:59p mward
 * PR 52355: Adding ANR
 * 
 * 13   2/20/09 1:39p jgarrett
 * PR 52355: Adding ANR
 *
 * PR49827/2   1/13/09 5:11p mward
 * PR50613: Add support for SRS Volume IQ.
 * 
 * PR49827/1   12/1/08 4:37p mward
 * PR49827: Add a Dolby Volume checkbox in Info:Audio.
 * 
 * 12   8/14/08 11:46a jgarrett
 * PR 32047: Adding pcm capture
 * 
 * 11   1/31/08 5:15p jgarrett
 * PR 39180: Adding ofdm tuning
 * 
 * 10   8/22/07 5:29p jgarrett
 * PR 32044: Merging to main branch
 * 
 * 7405_pip_support/1   8/21/07 2:38p katrep
 * PR34204: Added mosaic mode support for 7405.
 * 
 * 9   7/10/07 5:05p vsilyaev
 * PR 32849: Added list of streamer and input band object ID's
 * 
 * PR32044/1   6/26/07 5:55p jgarrett
 * PR 32044: Adding Mosaic Mode APIs
 * 
 * 8   2/23/07 3:41p jgarrett
 * PR 28036: Adding new options for LAB/CAB/Sharpness
 * 
 * 7   2/8/07 3:45p jgarrett
 * PR 27751: Adding runtime detection of PVR encryption support
 * 
 * 6   1/19/07 7:24p vsilyaev
 * PR 26400: Added decoder to handle separate audio decode
 * 
 * 5   11/30/06 7:22p jgarrett
 * PR 24698: PR 24264: Adding features for MAD, BNR/MNR/DCR, and video
 * formats.
 * 
 * 4   9/13/06 3:57p erickson
 * PR22954: add HD display as bconfig feature
 * 
 * 3   6/15/05 11:25a mphillip
 * PR15896: Remove two const declarations (and add a "this structure
 * should be treated as read-only" comment) to get bconfig_copy to work
 * under 2.7
 * 
 * 2   4/4/05 2:58p erickson
 * PR13823: added linein and analog to bconfig, and fixed a few areas
 * 
 * 1   2/7/05 7:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/9   12/10/04 4:28p marcusk
 * PR13348: Added support for record_pes and record_tsdma options.
 * 
 * Irvine_BSEAVSW_Devel/8   5/3/04 5:07p vsilyaev
 * PR 10927: Renamed pcm to pcm_play.
 * 
 * Irvine_BSEAVSW_Devel/7   5/3/04 12:28p vsilyaev
 * PR 10927: Changed type of bconfig to be constant point to constant data
 * (it allows to change data at initialization time).
 * 
 * Irvine_BSEAVSW_Devel/6   5/3/04 11:22a vsilyaev
 * PR 10927: Added resource list.
 * 
 * Irvine_BSEAVSW_Devel/5   11/4/03 2:42p erickson
 * documentation changes
 * 
 * Irvine_BSEAVSW_Devel/4   10/28/03 11:28a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/3   9/30/03 1:13a erickson
 * updated docjet documentation
 * 
 * Irvine_BSEAVSW_Devel/2   9/30/03 12:19a erickson
 * added standard header stuff like cplusplus
 * 
 * Irvine_BSEAVSW_Devel/1   8/11/03 4:27p vsilyaev
 * Settop run-time config.
 *
 ***************************************************************************/
#ifndef BSETTOP_CONFIG_H__
#define BSETTOP_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=**************
The config interface allows the application to get information
about the board, its high-level features, and all chips on the board.

Read the bconfig global variable after calling bsettop_init()
to access all information.

Although most of the structure is const-protected, the current 
toolchain does not appear to allow it all to be protected and still be 
used in bconfig_copy for proxy mode.  The user of this API should note 
that these structures should be treated as read-only.  Modifying 
the values will not actually affect what capabilities are supported 
by the chip or board.
****************/

/*
Summary:
String array datatype used in several bconfig structures.
*/
typedef const char *bstring_list[];

/*
Summary:
A data structure to describe a single chip family on a board.
*/
typedef struct bconfig_family {
    const char *name; /* for example, venom2 */
    const char *revision; /* family revision, for example, B0 */
    char major;  /* major revision 'A', 'B' ... */
    unsigned minor; /* minor revision number */
} bconfig_family;

/*
Summary:
A data structure to list the chip families on a board.
*/
typedef bconfig_family  *bconfig_family_list[10];

/*
Summary:
A data structure to describe a single chip on a board.
*/
typedef struct bconfig_chip {
    unsigned chip_no; /* chip name like 7320, 7020 */
    const char *revision; /* chip revision, like RC0 */
    char major;  /* major revision 'A', 'B' ... */
    unsigned minor; /* minor revision number */
    bconfig_family_list families; /* list of family cores included in the chip */
} bconfig_chip;

/*
Summary:
A data structure to define the list of chips on a board.
*/
typedef bconfig_chip *bconfig_chip_list[10];

/*
Summary:
The set of features which can be supported by a board.
*/
typedef struct bconfig_features {
    bool sds;          /* system might have SDS */
    bool qam;          /* system might have QAM */
    bool vsb;          /* system might have VSB */
    bool ofdm;         /* system might have DVB-T OFDM */
    bool analog;       /* system might have analog audio/video source */
    bool video_hd;     /* system could decode HD video */
    bool display_hd;   /* system could display HD video */
    bool dnr_dcr;      /* system might have DCR support (digital contour removal) */
    bool dnr_bnr;      /* system might have BNR support (block noise removal) */
    bool dnr_mnr;      /* system might have MNR support (mosquito noise removal)*/
    bool anr;          /* system might have ANR support (analog noise reduction)*/
    bool deinterlacer; /* system might have deinterlacing support */
    bool box_detect;   /* system might have letterbox detection support */
    bool pvr_encryption; /* system might have PVR encryption */
    bool sharpness;    /* System might have sharpness control */
    bool cab;          /* System might have color adjustments [Green stretch, Blue Stretch, Auto Flesh Tome] */
    bool lab;          /* System might have luma adjustments [Dynamic Contrast] */
    bool mosaic;       /* System might support mosaic mode */
    bool dolby_volume; /* System might have Dolby volume post processing */
    bool srs_volume;   /* System might have SRS volume IQ post processing */
    bool auto_volume_level;   /* System might have Auto Volume Level post processing */    
    bool dts_transcode; /* System might have DTS audio transcoding */
    bool ac3_transcode; /* System might have AC3 audio transcoding */

    bool video_format_is_supported[bvideo_format_count]; /* Array of supported video formats (index w/bvideo_format values) */
} bconfig_features;


typedef struct bconfig_obj_list {
    const char *type;         /* string representation of the object type */
    unsigned total;           /* total number of objects */
    const bobject_t *objects; /* list (array) of objects avaliable in the system */
} bconfig_obj_list;

/*
Summary:
Data structure to describe list of resources avaliable on a board. 
*/
typedef struct bconfig_board_resources {
    bconfig_obj_list record;       /* list of records */
    bconfig_obj_list record_pes;   /* list of pes records */
    bconfig_obj_list record_tsdma; /* list of tsdma records */
    bconfig_obj_list playback;     /* list of playbacks */
    bconfig_obj_list encode;       /* list of MPEG encoders */
    bconfig_obj_list display;      /* list of displays */
    bconfig_obj_list decode;       /* list of decodes */
    bconfig_obj_list analog;       /* list of analog tuners */
    bconfig_obj_list linein;       /* list of linein ids (see btuner_linein_t) */
    bconfig_obj_list sds;          /* list of SDS capable tuners */
    bconfig_obj_list qam;          /* list of QAM capable tuners */
    bconfig_obj_list vsb;          /* list of VSB capable tuners */
    bconfig_obj_list ofdm;         /* list of DVB-T/OFDM capable tuners */
    bconfig_obj_list pcm_play;     /* list of PCM playbacks */
    bconfig_obj_list decode_still; /* list of video still picture decodes */
    bconfig_obj_list decode_es;    /* list of ES only decoders (used for audio ES decode) */
    bconfig_obj_list streamers;    /* list of streamer inputs */
    bconfig_obj_list input_bands;  /* list of transport input bands */
    bconfig_obj_list decode_mosaic;/* list of Mosaic mode decoders */
    bconfig_obj_list pcm_capture;  /* list of PCM capture channels */
} bconfig_board_resources;

/*
Summary:
Data structure to describe a board, including a list of chips
and features.
*/
typedef struct bconfig_system{
    const char *board;  /* name of the board */
    unsigned board_revision;
    bconfig_chip_list chips; /* list of Broadcom chips on the board */
    bconfig_features features; /* list of features supported by the board */
    bconfig_board_resources resources; /* list of resources provided by the board hardware */ 
} bconfig_system;

/*
Summary:
The access point for all system configuration information.
Description:
It is initialized after calling bsettop_init().
*/
extern const bconfig_system * const bconfig;

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_CONFIG_H__ */

