/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_outputs.cpp $
 * $brcm_Revision: 60 $
 * $brcm_Date: 7/19/11 3:41p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_outputs.cpp $
 * 
 * 60   7/19/11 3:41p jtna
 * SW7344-126: NEXUS_VideoWindow_SetSettings() on a mosaic parent window
 * is no longer supported. Get the basic Brutus 3DTV working again, but
 * some obscure 3D features have been broken on purpose.
 * 
 * 59   4/8/11 4:17p ahulse
 * SW7405-5212: Disable A/R selection when 3D to 2D conversion is active
 * on composite output
 * 
 * 58   4/19/10 3:34p calvinho
 * SW7420-466: Add function setAspectRatio
 * 
 * 57   3/3/10 2:54p nitinb
 * SW7550-211: Merge from branch SW7550-211
 * 
 * SW7550-211/1   2/5/10 5:53p chengli
 * SW7550-211 : disable HAS_OUTPUT_SELECT for 7530 CRB board as there is
 * no component outputs
 * 
 * 56   1/11/10 11:43a randyjew
 * SW7468-6:Disable component on 97468 boards
 * 
 * 56   1/11/10 11:41a randyjew
 * SW7468-6:Disable component on 97468 boards
 * 
 * 55   8/3/09 7:13p mward
 * PR53752: Missing check for if (_dcs).  Second Brutus instance will have
 * _dcs = NULL.
 * 
 * 54   3/18/09 12:31p erickson
 * PR53340: add 1080p 50hz support
 *
 * 53   9/25/08 4:24p jtna
 * PR42825: do not hide color output arrowbar when format arrowbar is not
 * visible
 *
 * 52   6/17/08 8:09a erickson
 * PR42825: info panel might not be currently visible when resizing
 * controls. use visibleSet to determine if control will be visible.
 *
 * 51   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 50   2/26/08 7:33a jrubio
 * PR39363: fix compile warnings
 *
 * 49   1/24/08 5:14p gmohile
 * PR 38637 : Add aspect ratio control for SD path as well
 *
 * 48   10/17/07 9:44a erickson
 * PR36068: _rfChannel->getItem() may not exist
 *
 * 47   8/1/07 7:59p vle
 * PR 33721: Add support for vesa formats to getOptimalDisplay
 *
 * 46   6/12/07 1:52p erickson
 * PR31914: added AUTO_OUTPUT_MODE. UI option is always on.
 *
 * 45   6/8/07 12:15p erickson
 * PR31914: rename to getOptimalDisplay. remove old, bad AUTO_HDMI_MODE
 * code.
 *
 * 44   6/5/07 6:54p jgarrett
 * PR 31764: Fixing VESA setting by name for scripts
 *
 * 43   4/12/07 10:30a ahulse
 * PR29145: Add support for DCS
 *
 * 42   4/9/07 11:46a mward
 * PR28583: Set _mad and _boxdetect NULL to indicate not supported.
 *
 * 41   1/24/07 11:41a ahulse
 * PR25913: set rfmod country code according to 2 letter country code set
 * by user
 *
 * 40   12/7/06 3:32p ahulse
 * PR25913: If 2nd display has RF, show RF ch setting GUI
 *
 * 39   12/6/06 5:05p jgarrett
 * PR 24624: Changing MAutoList to MList
 *
 * 38   12/6/06 10:43a ahulse
 * PR25913: Add INFO_SHOW_ALL_FORMATS to display all output formats
 * supported by settopApi in info panel
 *
 * 37   11/30/06 7:24p jgarrett
 * PR 24698: PR 24624: Using bconfig variables to detect options for DNR,
 * MAD, Box Detect, and video formats at runtime.
 *
 * 36   10/23/06 6:17p mward
 * PR24520: Support 480p output on 7118.  Disable features not available.
 *
 * 35   10/19/06 3:11p erickson
 * PR25022: correctly label SECAM
 *
 * 34   9/20/06 1:19p erickson
 * PR24264: really default 1080p off
 *
 * 33   9/20/06 12:13p erickson
 * PR24264: added 1080p, but default off for now
 *
 * 32   9/13/06 9:50a erickson
 * PR24230: remove 7327 and 7329
 *
 * 31   9/11/06 5:05p rjlewis
 * PR21941: vxworks likes it to be static const
 *
 * 30   9/6/06 2:43p erickson
 * PR21941: added output_format(--) and made -- and ++ relative to current
 * format
 *
 * 29   9/6/06 1:41p erickson
 * PR21941: fix bonehead bug
 *
 * 28   9/6/06 1:22p erickson
 * PR21941: added output_format(++)
 *
 * 27   8/2/06 4:45p erickson
 * PR21941: moved DCR control from Display to Video panel, converted to
 * slider
 *
 * 26   6/20/06 4:42p erickson
 * PR21941: added SECAM for 7401 B0 and beyond
 *
 * 25   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/8   5/16/06 11:33a tokushig
 * merge from main
 *
 * 24   5/4/06 5:20p jgarrett
 * PR 21322: Fixing skin support for auto hdmi checkbox
 *
 * 23   5/2/06 2:46p jgarrett
 * PR 21322: Adding Auto HDMI option
 *
 * 22   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/7   4/14/06 8:04p tokushig
 * moved info screen location to avoid overscan cutoff
 *
 * SanDiego_Brutus_Skin/6   4/13/06 3:16p tokushig
 * merge from main to SanDiego_Brutus_Skin branch
 *
 * SanDiego_Brutus_Skin/5   3/30/06 10:52a tokushig
 * remove prerendering of icon if magnum based.  since we are using the
 * blitter to display pixmaps, any pixmap with alpha values cannot be
 * displayed properly.  legacy platforms will operate as usual
 *
 * SanDiego_Brutus_Skin/4   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 *
 * SanDiego_Brutus_Skin/3   2/15/06 10:50a tokushig
 * add bevel style for mlabel to info skin
 *
 * SanDiego_Brutus_Skin/2   2/14/06 12:49p tokushig
 * allowed buttons to use images from skin
 *
 * SanDiego_Brutus_Skin/1   2/13/06 4:38p tokushig
 * added partial skin ability.  note that this is primarily for cable show
 * demo purposes and will be expanded to be fully skinnable in the
 * future.
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_outputs.cpp $
 * 
 * 60   7/19/11 3:41p jtna
 * SW7344-126: NEXUS_VideoWindow_SetSettings() on a mosaic parent window
 * is no longer supported. Get the basic Brutus 3DTV working again, but
 * some obscure 3D features have been broken on purpose.
 * 
 * 59   4/8/11 4:17p ahulse
 * SW7405-5212: Disable A/R selection when 3D to 2D conversion is active
 * on composite output
 * 
 * 58   4/19/10 3:34p calvinho
 * SW7420-466: Add function setAspectRatio
 * 
 * 57   3/3/10 2:54p nitinb
 * SW7550-211: Merge from branch SW7550-211
 * 
 * SW7550-211/1   2/5/10 5:53p chengli
 * SW7550-211 : disable HAS_OUTPUT_SELECT for 7530 CRB board as there is
 * no component outputs
 * 
 * 56   1/11/10 11:43a randyjew
 * SW7468-6:Disable component on 97468 boards
 * 
 * 56   1/11/10 11:41a randyjew
 * SW7468-6:Disable component on 97468 boards
 * 
 * 55   8/3/09 7:13p mward
 * PR53752: Missing check for if (_dcs).  Second Brutus instance will have
 * _dcs = NULL.
 * 
 * 54   3/18/09 12:31p erickson
 * PR53340: add 1080p 50hz support
 *
 * 53   9/25/08 4:24p jtna
 * PR42825: do not hide color output arrowbar when format arrowbar is not
 * visible
 *
 * 52   6/17/08 8:09a erickson
 * PR42825: info panel might not be currently visible when resizing
 * controls. use visibleSet to determine if control will be visible.
 *
 * 51   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 50   2/26/08 7:33a jrubio
 * PR39363: fix compile warnings
 *
 * 49   1/24/08 5:14p gmohile
 * PR 38637 : Add aspect ratio control for SD path as well
 *
 * 48   10/17/07 9:44a erickson
 * PR36068: _rfChannel->getItem() may not exist
 *
 * 47   8/1/07 7:59p vle
 * PR 33721: Add support for vesa formats to getOptimalDisplay
 *
 * 46   6/12/07 1:52p erickson
 * PR31914: added AUTO_OUTPUT_MODE. UI option is always on.
 *
 * 45   6/8/07 12:15p erickson
 * PR31914: rename to getOptimalDisplay. remove old, bad AUTO_HDMI_MODE
 * code.
 *
 * 44   6/5/07 6:54p jgarrett
 * PR 31764: Fixing VESA setting by name for scripts
 *
 * 43   4/12/07 10:30a ahulse
 * PR29145: Add support for DCS
 *
 * 42   4/9/07 11:46a mward
 * PR28583: Set _mad and _boxdetect NULL to indicate not supported.
 *
 * 41   1/24/07 11:41a ahulse
 * PR25913: set rfmod country code according to 2 letter country code set
 * by user
 *
 * 40   12/7/06 3:32p ahulse
 * PR25913: If 2nd display has RF, show RF ch setting GUI
 *
 * 39   12/6/06 5:05p jgarrett
 * PR 24624: Changing MAutoList to MList
 *
 * 38   12/6/06 10:43a ahulse
 * PR25913: Add INFO_SHOW_ALL_FORMATS to display all output formats
 * supported by settopApi in info panel
 *
 * 37   11/30/06 7:24p jgarrett
 * PR 24698: PR 24624: Using bconfig variables to detect options for DNR,
 * MAD, Box Detect, and video formats at runtime.
 *
 * 36   10/23/06 6:17p mward
 * PR24520: Support 480p output on 7118.  Disable features not available.
 *
 * 35   10/19/06 3:11p erickson
 * PR25022: correctly label SECAM
 *
 * 34   9/20/06 1:19p erickson
 * PR24264: really default 1080p off
 *
 * 33   9/20/06 12:13p erickson
 * PR24264: added 1080p, but default off for now
 *
 * 32   9/13/06 9:50a erickson
 * PR24230: remove 7327 and 7329
 *
 * 31   9/11/06 5:05p rjlewis
 * PR21941: vxworks likes it to be static const
 *
 * 30   9/6/06 2:43p erickson
 * PR21941: added output_format(--) and made -- and ++ relative to current
 * format
 *
 * 29   9/6/06 1:41p erickson
 * PR21941: fix bonehead bug
 *
 * 28   9/6/06 1:22p erickson
 * PR21941: added output_format(++)
 *
 * 27   8/2/06 4:45p erickson
 * PR21941: moved DCR control from Display to Video panel, converted to
 * slider
 *
 * 26   6/20/06 4:42p erickson
 * PR21941: added SECAM for 7401 B0 and beyond
 *
 * 25   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/8   5/16/06 11:33a tokushig
 * merge from main
 *
 * 24   5/4/06 5:20p jgarrett
 * PR 21322: Fixing skin support for auto hdmi checkbox
 *
 * 23   5/2/06 2:46p jgarrett
 * PR 21322: Adding Auto HDMI option
 *
 * 22   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/7   4/14/06 8:04p tokushig
 * moved info screen location to avoid overscan cutoff
 *
 * SanDiego_Brutus_Skin/6   4/13/06 3:16p tokushig
 * merge from main to SanDiego_Brutus_Skin branch
 *
 * 21   4/4/06 9:21p erickson
 * PR17108: rename to DNR
 *
 * 19   12/15/05 10:25a rgreen
 * PR18609,PR12560: Add 640x480p and VGA entries to lookup tables of valid
 * formats.  "ouptut_format(x) script can be used to switch formats
 *
 * 18   12/8/05 2:52p jrubio
 * PR18442: Added Contour Removal to info panel
 *
 * 17   11/11/05 6:17p rgreen
 * PR12560: Add support for computer formats
 * Added 1280x768
 * Added 1280x720 (partial implementation)
 *
 * 16   9/15/05 6:13p erickson
 * PR17108: handle systems with no windows
 *
 * 15   9/8/05 10:01a erickson
 * PR17048: switch individual chip defines to B_HAS_VDC for all 7038 and
 * beyond platforms
 *
 * 14   8/10/05 5:00p erickson
 * PR15137: handle uninit variable in some cases
 *
 * 13   8/5/05 12:20p erickson
 * PR15137: update decode window before updating setOutput
 *
 * 12   8/4/05 5:24p erickson
 * PR15137: added Control event when output format changes
 *
 * 11   7/22/05 7:44p rgreen
 * PR15041: Unsupported HDMI display outputs result in Video Output Error.
 * Fix check of return value from setOuput call
 *
 * 10   7/18/05 2:12p erickson
 * PR14935: if 2nd display, _outputs will be invisible, therefore don't
 * change outputs
 *
 * 9   7/18/05 12:48p erickson
 * PR15053: don't set output mode when in vesa format
 *
 * 8   4/18/05 10:56a erickson
 * PR14593: prerender images for faster draw time on slow platforms
 *
 * 7   3/28/05 4:57p erickson
 * PR14593: add 3560 support
 *
 * 6   3/24/05 1:21p erickson
 * PR14593: removed old platform
 *
 * 5   3/23/05 1:31p erickson
 * PR12769: added nonlinear scale option
 *
 * 4   2/22/05 12:01p erickson
 * PR14180: renamed variable for vxworks warning
 *
 * 3   2/16/05 9:17a erickson
 * PR13387: fixed background
 *
 * 2   2/15/05 1:22p rgreen
 * PR12560: Add Support for various computer formats (1024x768p, 800x600p
 * etc).
 * Can be enabled using OUTPUT_FORMAT variable in brutus.cfg
 *
 * 1   2/7/05 8:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/18   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/17   12/17/04 3:50p bandrews
 * PR13161: support for changing outputs added
 *
 * Irvine_BSEAVSW_Devel/16   12/15/04 6:05p bandrews
 * PR13158: 7318 has 6 dacs
 *
 * Irvine_BSEAVSW_Devel/15   12/15/04 5:27p bandrews
 * PR13158: 7318 has output select
 *
 * Irvine_BSEAVSW_Devel/14   12/6/04 10:16a erickson
 * PR13312: adapted output panel for 7038 2nd display
 *
 * Irvine_BSEAVSW_Devel/13   12/3/04 3:21p erickson
 * PR12388: vesa can have various format_settings and should be handled
 * differently
 *
 * Irvine_BSEAVSW_Devel/12   10/27/04 12:17p erickson
 * PR13096: added MPAA option
 *
 * Irvine_BSEAVSW_Devel/11   10/26/04 5:06p erickson
 * PR13014: svideo/component are don't-cares on 7038, otherwise it messes
 * of HDSD_SIMUL and others
 *
 * Irvine_BSEAVSW_Devel/10   10/15/04 10:51a erickson
 * PR13014: infobar rework
 *
 * Irvine_BSEAVSW_Devel/9   10/11/04 11:30a erickson
 * PR11081: added output select for 7315/7115 formats and made it more
 * data driven
 *
 * Irvine_BSEAVSW_Devel/8   10/8/04 10:12a erickson
 * PR12829: added static function for looking up output format, and made
 * PAL and NTSC more likely to succeed
 *
 * Irvine_BSEAVSW_Devel/7   9/22/04 4:48p erickson
 * PR12751: allow 16x9 checkbox for 480p and 576p
 *
 * Irvine_BSEAVSW_Devel/6   9/17/04 10:37a erickson
 * PR11081: added confirmation for video format change
 *
 * Irvine_BSEAVSW_Devel/5   9/16/04 4:59p erickson
 * PR12682: sleep no longer needed because settop api returns cached
 * values
 *
 * Irvine_BSEAVSW_Devel/4   9/16/04 3:38p erickson
 * PR11081: more work on info panel display
 *
 * Irvine_BSEAVSW_Devel/3   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 *
 * Irvine_BSEAVSW_Devel/2   9/15/04 10:01a erickson
 * PR11081: hide component ui item if no component
 *
 * Irvine_BSEAVSW_Devel/1   9/13/04 1:53p erickson
 * PR11081: added output control to infopanel
 *
 ****************************************************************************/
#include "info_outputs.h"
#include "control.h"
#include "cfgsettings.h"
#include "marrowbar.h"
#include "mapplication.h"
#include "bsettop_save_settings.h"
#include "mmessagebox.h"

BDBG_MODULE(info_outputs);

/* The following defines make up a bitmask, not an enumeration */
#define OUTPUTS_YPRPB       0x0
#define OUTPUTS_RGB         0x1
#define OUTPUTS_COMPONENT   0x2 /* If set, then test YPRPB or RGB */
#define OUTPUTS_SVIDEO      0x4
#define OUTPUTS_COMPOSITE   0x8
#define OUTPUTS_SD_DONT_CARE 0x8000 /* Ignore non-component settings */

#if !defined(BCM732X_FAMILY) && ( !NEXUS_BOARD_7530_CRB )
/* The 732X family has no outputs to configure onscreen. There are
some options in the config file. */
#define HAS_OUTPUT_SELECT
#endif

#ifdef HAS_OUTPUT_SELECT
static struct {
    const char *name;
    int selection;
} OutputsSelection[] =
{
/* should be board based */
#if BCHP_CHIP == 7115 || BCHP_CHIP == 7315
    /* Can't have svideo and component */
    {"SVideo", OUTPUTS_SVIDEO},
    {"Comp RGB", OUTPUTS_COMPONENT|OUTPUTS_RGB},
    {"Comp YPrPb", OUTPUTS_COMPONENT|OUTPUTS_YPRPB},
#elif BCM_BOARD == 97468
     {"RGB", OUTPUTS_RGB|OUTPUTS_SD_DONT_CARE},
    {"YPrPb", OUTPUTS_YPRPB|OUTPUTS_SD_DONT_CARE},
#else /* 7110, 7038, etc. */
    {"RGB", OUTPUTS_COMPONENT|OUTPUTS_RGB|OUTPUTS_SD_DONT_CARE},
    {"YPrPb", OUTPUTS_COMPONENT|OUTPUTS_YPRPB|OUTPUTS_SD_DONT_CARE},
#endif
    {0,0}
};
#endif

#define MAX_NAMES_PER_FORMAT 3
struct VideoFormatDescription
{
        int uiSortOrder;                            /* Order we would like these to appear (0=first) */
        bool visible;                               /* Is the format visible in the UI? */
        bool isHd;                                  /* Is the format HD? */
        bool is16x9;                                /* Is the format 16x9? */
        bool mainDisplayOnly;                       /* Is the format Main display only? */
        bvideo_format format;                       /* Actual bvideo_format enum */
        bvideo_format sdEquivalent;                 /* What's the SD equivalent of this format? */
        char *pszFormatNames[MAX_NAMES_PER_FORMAT]; /* Array of possible names (first is UI name, others optional) */
};

static VideoFormatDescription g_videoFormats[bvideo_format_count] =
{
/* These are in order of bvideo_format from bsettop_types.h                                                        */
/*  #    visible isHd   is16x9 not2nd Format ID                 SD Equivalent              Names                           */
    {0,  true,  false, false, false, bvideo_format_ntsc,       bvideo_format_ntsc,        {(char *)"480i (NTSC)", (char *)"480i", (char *)"NTSC"}},
    {13, true,  false, false, false, bvideo_format_ntsc_japan, bvideo_format_ntsc_japan,  {(char *)"NTSC J", (char *)"NTSC-J", NULL}},
    {14, true,  false, false, false, bvideo_format_pal_m,      bvideo_format_pal_m,       {(char *)"PAL M", (char *)"PAL-M", NULL}},
    {15, true,  false, false, false, bvideo_format_pal_n,      bvideo_format_pal_n,       {(char *)"PAL N", (char *)"PAL-N", NULL}},
    {16, true,  false, false, false, bvideo_format_pal_nc,     bvideo_format_pal_nc,      {(char *)"PAL NC", (char *)"PAL-NC", NULL}},
    {17, false, false, false, false, bvideo_format_pal_b,      bvideo_format_pal_b,       {(char *)"PAL B", (char *)"PAL-B", NULL}},
    {18, false, false, false, false, bvideo_format_pal_b1,     bvideo_format_pal_b1,      {(char *)"PAL B1", (char *)"PAL-B1", NULL}},
    {19, false, false, false, false, bvideo_format_pal_d,      bvideo_format_pal_d,       {(char *)"PAL D", (char *)"PAL-D", NULL}},
    {20, false, false, false, false, bvideo_format_pal_d1,     bvideo_format_pal_d1,      {(char *)"PAL D1", (char *)"PAL-D1", NULL}},
    {4,  true,  false, false, false, bvideo_format_pal,        bvideo_format_pal,         {(char *)"576i (PAL)", (char *)"576i", (char *)"PAL"}},
    {21, false, false, false, false, bvideo_format_pal_h,      bvideo_format_pal_h,       {(char *)"PAL H", (char *)"PAL-H", NULL}},
    {22, false, false, false, false, bvideo_format_pal_k,      bvideo_format_pal_k,       {(char *)"PAL K", (char *)"PAL-K", NULL}},
    {23, false, false, false, false, bvideo_format_pal_i,      bvideo_format_pal_i,       {(char *)"PAL I", (char *)"PAL-I", NULL}},
    {5,  true,  false, false, false, bvideo_format_secam,      bvideo_format_secam,       {(char *)"576i (SECAM)", (char *)"SECAM", NULL}},
    {1,  true,  true,  false, true,  bvideo_format_480p,       bvideo_format_ntsc,        {(char *)"480p", NULL, NULL}},
    {6,  true,  true,  false, true,  bvideo_format_576p,       bvideo_format_pal,         {(char *)"576p", NULL, NULL}},
    {3,  true,  true,  true,  true,  bvideo_format_1080i,      bvideo_format_ntsc,        {(char *)"1080i", NULL, NULL}},
    {8,  true,  true,  true,  true,  bvideo_format_1080i_50hz, bvideo_format_pal,         {(char *)"1080i 50Hz", (char *)"1080i50", NULL}},
    {9,  true,  true,  true,  true,  bvideo_format_1080p,      bvideo_format_ntsc,        {(char *)"1080p 60Hz", (char *)"1080p", NULL}},
    {10, true,  true,  true,  true,  bvideo_format_1080p_24hz, bvideo_format_ntsc,        {(char *)"1080p 24Hz", (char *)"1080p24", NULL}},
    {11, true,  true,  true,  true,  bvideo_format_1080p_25hz, bvideo_format_pal,         {(char *)"1080p 25Hz",(char *) "1080p25", NULL}},
    {12, true,  true,  true,  true,  bvideo_format_1080p_30hz, bvideo_format_ntsc,        {(char *)"1080p 30Hz", (char *)"1080p30", NULL}},
    {12, true,  true,  true,  true,  bvideo_format_1080p_50hz, bvideo_format_ntsc,        {(char *)"1080p 50Hz", (char *)"1080p50", NULL}},
    {25, false, true,  true,  true,  bvideo_format_1250i_50hz, bvideo_format_pal,         {(char *)"1250i 50Hz", (char *)"1250i50", NULL}},
    {2,  true,  true,  true,  true,  bvideo_format_720p,       bvideo_format_ntsc,        {(char *)"720p", NULL, NULL}},
    {7,  true,  true,  true,  true,  bvideo_format_720p_50hz,  bvideo_format_pal,         {(char *)"720p 50Hz", (char *)"720p50", NULL}},
    {24, false, true,  true,  true,  bvideo_format_720p_24hz,  bvideo_format_ntsc,        {(char *)"720p 24Hz", (char *)"720p24", NULL}},
    {26, true,  true,  true,  true,  bvideo_format_vesa,       bvideo_format_ntsc,        {(char *)"VESA 640x480", (char *)"VESA", NULL}}
};

static MList<VideoFormatDescription> g_mainFormatList;
static MList<VideoFormatDescription> g_secondFormatList;

const char *AspectRatios[] = {
    "HD A/R: Zoom",
    "HD A/R: Box",
    "HD A/R: Pan Scan",
    "HD A/R: Full",
    "HD A/R: Nonlinear",
    0
};

const char *SDAspectRatios[] = {
    "SD A/R: Zoom",
    "SD A/R: Box",
    "SD A/R: Pan Scan",
    "SD A/R: Full",
    "SD A/R: Nonlinear",
    0
};
typedef struct
{
    char *name ;
    int width, height ;
    int refresh ;
    bool interlaced ;
} VesaFormatEntry  ;

/* Special vesa modes */
const  VesaFormatEntry gVesaFormatTable[]  =
{
    {(char* )"1366x768p", 1366, 768, 60, false},
    {(char* )"1024x768p", 1024, 768, 60, false},
    {(char* )"800x600p", 800, 600, 60, false},
    {(char* )"640x480p", 640, 480, 60, false},
    {(char* )"vga", 640, 480, 60, false},
    {(char* )"1280x768p", 1280, 768, 60, false},
    {(char* )"1280x720p", 1280, 720, 60, false},
    {0, 0, 0, 0, 0}
} ;

class OutputsPanel : public MLabel, MButtonEventHandler {
public:
    OutputsPanel(Control *control, MWidget *parent, const MRect &rect);

    void apply();
    void read();

    // overrides
    void show();
    void onClick(MButton *button);

protected:
    Control *_control;
    MArrowBar *_format, *_output, *_aspectRatio, *_aspectRatio_SD, *_rfChannel;
    MCheckBox *_mad, *_boxdetect, *_is16x9, *_mpaa, *_autoOutputFormat;
#ifdef DCS_SUPPORT
    MCheckBox *_dcs;
#endif

    /* restack visible components and resize box */
    void pack();
    int _totalpacked;
#define MAX_PACKED 11
    MWidget *_pack[MAX_PACKED]; // must be > than number of packed components

    MList<VideoFormatDescription> *_pFormatList;

    static void buildVideoFormatTable(bool isSecondDisplay, bool showAllFormats);
};

#define BARWIDTH 180
#define BARHEIGHT 35
#define INDENT 5

#define RF_LABEL_SIZE   13  /* max nbr of chars used in "RF" label : "RF 3 (US)" */

static int formatSort(VideoFormatDescription *pFirst, VideoFormatDescription *pSecond);
int isValidCountryCode( char *pTwoLetterCountry );
bvideo_format getVideoFormatForCountry( char *pTwoLetterCountry );

OutputsPanel::OutputsPanel(Control *control, MWidget *parent, const MRect &rect) :
    MLabel(parent, rect)
{
    char *pCountryCode;
    char rfChan[RF_LABEL_SIZE];

    _totalpacked = 0;
    _control = control;

    /* create controls */
    _pack[_totalpacked++] = _format = new MArrowBar(this, MRect(INDENT,0,BARWIDTH,BARHEIGHT));
    _format->addHandler(this);
#ifdef HAS_OUTPUT_SELECT
    _pack[_totalpacked++] = _output = new MArrowBar(this, MRect(INDENT,0,BARWIDTH,BARHEIGHT));
    _output->addHandler(this);
#endif
    if (!_control->secondDisplay()) {
        if ( bconfig->features.deinterlacer )
        {
            _pack[_totalpacked++] = _mad = new MCheckBox(this, MRect(INDENT,0,BARWIDTH,25), "Deinterlacer");
            _mad->addHandler(this);
        }
        else
        {
            _mad = NULL;
        }
        if ( bconfig->features.box_detect )
        {
            _pack[_totalpacked++] = _boxdetect = new MCheckBox(this, MRect(INDENT,0,BARWIDTH,25), "Box Detect");
            _boxdetect->addHandler(this);
        }
        else
        {
            _boxdetect = NULL;
        }
        _pack[_totalpacked++] = _mpaa = new MCheckBox(this, MRect(INDENT,0,BARWIDTH,25), "MPAA");
        _mpaa->addHandler(this);

        /* set AUTO_OUTPUT_FORMAT with the GUI */
        _pack[_totalpacked++] = _autoOutputFormat = new MCheckBox(this, MRect(INDENT,0,BARWIDTH,25), "Auto Format");
        _autoOutputFormat->addHandler(this);

#ifdef DCS_SUPPORT
        _pack[_totalpacked++] = _dcs = new MCheckBox(this, MRect(INDENT,0,BARWIDTH,25), "DCS");
        _dcs->addHandler(this);
#endif

    }
    else {
        _mad = NULL;
        _boxdetect = NULL;
        _mpaa = NULL;
        _autoOutputFormat = NULL;
#ifdef DCS_SUPPORT
        _dcs = NULL;
#endif
    }

    _pack[_totalpacked++] = _is16x9 = new MCheckBox(this, MRect(INDENT,0,BARWIDTH,25), "16x9 SD Display");
    _is16x9->addHandler(this);
    _pack[_totalpacked++] = _aspectRatio = new MArrowBar(this, MRect(INDENT,0,BARWIDTH,BARHEIGHT));
    _aspectRatio->addHandler(this);
    _pack[_totalpacked++] = _aspectRatio_SD = new MArrowBar(this, MRect(INDENT,0,BARWIDTH,BARHEIGHT));
    _aspectRatio_SD->addHandler(this);
    _pack[_totalpacked++] = _rfChannel = new MArrowBar(this, MRect(INDENT,0,150,BARHEIGHT));
    _rfChannel->addHandler(this);

    BDBG_ASSERT(_totalpacked <= MAX_PACKED);

    pack();

    buildVideoFormatTable(_control->secondDisplay(), _control->cfg()->getBool(Config::INFO_SHOW_ALL_FORMATS));

    /* load text */
    if (_control->secondDisplay())
        _pFormatList = &g_secondFormatList;
    else
        _pFormatList = &g_mainFormatList;

    if( (pCountryCode = (char *)_control->cfg()->get(Config::RFMOD_COUNTRY)) != NULL  ) {
        bvideo_format videoFormat;
        int i;
        /* Look up video format of country type */
        videoFormat = getVideoFormatForCountry( pCountryCode );

        /* If this format type is not in list add it */
        MList<VideoFormatDescription> *pList = ( _control->secondDisplay() )?&g_secondFormatList:&g_mainFormatList;
        for( i=0; i < pList->total(); i++ ) {
            /* Does it exist ?? */
            if( pList->get(i)->sdEquivalent == videoFormat ) break;
        }
        if( i >= pList->total() ) {
            for( i=0; i < bvideo_format_count; i++ ) {
                if( videoFormat == g_videoFormats[i].sdEquivalent ) break;
            }
            pList->add(g_videoFormats+i);
            pList->sort(formatSort);

        }
    }

    for ( int i=0; i < _pFormatList->total(); i++ )
    {
        _format->addItem(_pFormatList->get(i)->pszFormatNames[0]);
    }

#ifdef HAS_OUTPUT_SELECT
    for (int i=0;OutputsSelection[i].name;i++) {
        _output->addItem(OutputsSelection[i].name);
    }
#endif

    for (int i=0;AspectRatios[i];i++) {
        _aspectRatio->addItem(AspectRatios[i]);
        _aspectRatio_SD->addItem(SDAspectRatios[i]);
    }


    if( (pCountryCode = (char *)_control->cfg()->get(Config::RFMOD_COUNTRY)) != NULL && isValidCountryCode( pCountryCode) ) {
        sprintf( rfChan, "%s 3 (%s)", "RF", pCountryCode );
        _rfChannel->addItem(rfChan);
        sprintf( rfChan, "%s 4 (%s)", "RF", pCountryCode );
        _rfChannel->addItem(rfChan);
    }
    else {
        _rfChannel->addItem("RF 3 (US)");
        _rfChannel->addItem("RF 4 (US)");
    }

    setBackgroundColor(app()->style()->color(MStyle::BACKGROUND));
    setBevel(3);

    Skin * pSkin = control->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        //get list box attributes from skin
        MString         screenColor;
        MString         imgFocus;
        MString         textColor;
        MString         textColorFocus;
        MString         buttonOn;
        MString         buttonOff;
        MString         buttonDown;
        MString         bevelStyle;
        uint32_t        bevelWidth       = 0;

        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_COLOR,              screenColor);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_IMAGE_FOCUS,        imgFocus);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_TEXT_COLOR,         textColor);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_TEXT_COLOR_FOCUS,   textColorFocus);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BUTTON_IMAGE_ON,    buttonOn);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BUTTON_IMAGE_OFF,   buttonOff);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BUTTON_IMAGE_DOWN,  buttonDown);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BEVEL_WIDTH,        bevelWidth);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BEVEL_STYLE,        bevelStyle);

        //set panel background color if necessary
        if (!screenColor.isEmpty())
        {
            int32_t nCol = pSkin->getColorValue(screenColor.s());

            setBackgroundColor(nCol);
            _format->setBackgroundColor(nCol);
#ifdef HAS_OUTPUT_SELECT
            _output->setBackgroundColor(nCol);
#endif
            if ( _mad )
                _mad->setBackgroundColor(nCol);
            if ( _boxdetect )
                _boxdetect->setBackgroundColor(nCol);
            if ( _mpaa )
                _mpaa->setBackgroundColor(nCol);
            if ( _autoOutputFormat )
                _autoOutputFormat->setBackgroundColor(nCol);
            _is16x9->setBackgroundColor(nCol);
            _aspectRatio->setBackgroundColor(nCol);
        _aspectRatio_SD->setBackgroundColor(nCol);
            _rfChannel->setBackgroundColor(nCol);
        }

        //set focus image if necessary
        if (!imgFocus.isEmpty())
        {
            //setImageFocus(pSkin->getImage(imgFocus.s()));
        }

        //set text color if necessary
        if (!textColor.isEmpty())
        {
            int32_t nCol = pSkin->getColorValue(textColor.s());
            setTextColor(nCol);
            _format->setTextColor(nCol);
#ifdef HAS_OUTPUT_SELECT
            _output->setTextColor(nCol);
#endif
            if ( _mad )
                _mad->setTextColor(nCol);
            if ( _boxdetect )
                _boxdetect->setTextColor(nCol);
            if ( _mpaa )
                _mpaa->setTextColor(nCol);
            if ( _autoOutputFormat )
                _autoOutputFormat->setTextColor(nCol);
            _is16x9->setTextColor(nCol);
            _aspectRatio->setTextColor(nCol);
        _aspectRatio_SD->setTextColor(nCol);
            _rfChannel->setTextColor(nCol);
        }

        //set focus text color if necessary
        if (!textColorFocus.isEmpty())
        {
            int32_t nCol = pSkin->getColorValue(textColorFocus.s());
            setTextColorFocus(nCol);
            _format->setTextColorFocus(nCol);
#ifdef HAS_OUTPUT_SELECT
            _output->setTextColorFocus(nCol);
#endif
            if ( _mad )
                _mad->setTextColorFocus(nCol);
            if ( _boxdetect )
                _boxdetect->setTextColorFocus(nCol);
            if ( _mpaa )
                _mpaa->setTextColorFocus(nCol);
            if ( _autoOutputFormat )
                _autoOutputFormat->setTextColor(nCol);
            _is16x9->setTextColorFocus(nCol);
            _aspectRatio->setTextColorFocus(nCol);
        _aspectRatio_SD->setTextColorFocus(nCol);
            _rfChannel->setTextColorFocus(nCol);
        }

        MImage * imgButtonOn   = NULL;
        MImage * imgButtonOff  = NULL;
        MImage * imgButtonDown = NULL;
        //set button focus image if necessary
        if (!buttonOn.isEmpty())
        {
            imgButtonOn = pSkin->getImage(buttonOn.s());
        }

        //set button unfocus image if necessary
        if (!buttonOff.isEmpty())
        {
            imgButtonOff = pSkin->getImage(buttonOff.s());
        }

        //set button pressed image if necessary
        if (!buttonDown.isEmpty())
        {
            imgButtonDown = pSkin->getImage(buttonDown.s());
        }

        if (imgButtonOn || imgButtonOff || imgButtonDown)
        {
            _format->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
#ifdef HAS_OUTPUT_SELECT
            _output->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
#endif
            _aspectRatio->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
        _aspectRatio_SD->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
            _rfChannel->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
        }

        //set bevel if necessary
        if ((bevelWidth > 0) && (!bevelStyle.isEmpty()))
        {
            MPainter::BevelStyle bevelType = MPainter::bsRaised;
            if (bevelStyle == XML_BEVEL_SUNKEN)
            {
                bevelType = MPainter::bsSunken;
            }
            if (bevelStyle == XML_BEVEL_UPDOWN)
            {
                bevelType = MPainter::bsUpDown;
            }

            setBevel(bevelWidth, bevelType);
        }
    }
}

void OutputsPanel::pack()
{
    int cury = 5;
    for (int i=0;i<_totalpacked;i++) {
        if (_pack[i]->visibleSet()) {
            _pack[i]->moveTo(_pack[i]->x(), cury);
            cury += _pack[i]->height() + 5;
        }
    }
    setGeometry(MRect(x(), y(), width(), cury));
}

void OutputsPanel::onClick(MButton *button)
{
    apply();
}

void OutputsPanel::show()
{
    MLabel::show();
    /* must read after show because we need components to be visible
    in order to pack correctly */
    read();
}

void OutputsPanel::read()
{
    bdisplay_settings settings, display2_settings;
    bdecode_window_settings window_settings;
    bvideo_format_settings format_settings;
    const BrutusDisplay *brutDisplay = _control->display();
    MWidget *focused = app()->focusedWidget();

    _control->getOutput(&settings);
    if (!_control->window()) return;

    bdecode_window_get(_control->window(), &window_settings);
    bdisplay_get_video_format_settings(&settings, &format_settings);

    if (_control->cfg()->get("HIDE_OUTPUT_MODE")) {
        _format->hide();
    }
    else
    {
        int i, total;
        total = _pFormatList->total();
        for ( i=0; i<total; i++)
        {
            if (_pFormatList->get(i)->format == settings.format)
            {
                _format->setCurrent(i);
                break;
            }
        }
    }

#ifdef HAS_OUTPUT_SELECT
    if (!_control->secondDisplay() && settings.format != bvideo_format_vesa) {

        for (int i=0;OutputsSelection[i].name;i++) {
            int output_select = OutputsSelection[i].selection;

            /* Using negations to make boolean comparisons */
            if ((!(output_select & OUTPUTS_SVIDEO) == !settings.svideo &&
                !(output_select & OUTPUTS_COMPONENT) == !settings.component)
                || output_select & OUTPUTS_SD_DONT_CARE)
            {
                if (settings.component) {
                    boutput_component_settings cs;
                    boutput_component_get(settings.component, &cs);
                    if (output_select & OUTPUTS_RGB && cs.type == boutput_component_type_yprpb)
                    {
                        continue;
                    }
                }

                /* We've got a match */
                _output->setCurrent(i);
                if (_format->visible()) {
                    _output->show();
                }
                break;
            }
        }
    }
    else {
        _output->hide();
    }
#endif

    if( brutDisplay->display2 ) bdisplay_get(brutDisplay->display2, &display2_settings);

    if (settings.rf) {
        boutput_rf_settings rs;
        boutput_rf_get(settings.rf, &rs);
        _rfChannel->setCurrent(rs.channel - 3);
        _rfChannel->show();
    }
    else if ( brutDisplay->display2 && display2_settings.rf ) {
        boutput_rf_settings rs;
        boutput_rf_get( display2_settings.rf, &rs);
        _rfChannel->setCurrent(rs.channel - 3);
        _rfChannel->show();
    }
    else {
        _rfChannel->hide();
    }

    /* 4x3 displays include 480i, 480p, 576i, 576p */
    if (format_settings.height > 576) {
        _is16x9->hide();
    }
    else {
        _is16x9->setChecked(settings.aspect_ratio == bdisplay_aspect_ratio_16x9);
        _is16x9->show();
    }

    _aspectRatio->setCurrent((int)settings.content_mode);

    if ( brutDisplay->display2 ) {
        if ( _control->cfg()->getBool(Config::FORCE_COMPOSITE_OUTPUT_AR_FOR_3D_TO_2D) ) {
            if ( brutDisplay->s3dTv.output > Brutus3DTVMode_eOff ) {
                /* Disable a/r ratio selection because 3D to 2D will overide it */
                _aspectRatio->setEnabled(false);    
                _aspectRatio_SD->setEnabled(false);
            }
            else {
                if(  !_aspectRatio->enabled() ) {
                    /* Re-enable a/r ratio selection */
                    _aspectRatio->setEnabled(true);
                    _aspectRatio_SD->setEnabled(true);
                }
            }
        }
      _aspectRatio_SD->setCurrent((int)display2_settings.content_mode);
    }

    if (_mad)
        _mad->setChecked(window_settings.deinterlacer);
    if (_boxdetect)
        _boxdetect->setChecked(window_settings.box_detect);
    if (_mpaa)
        _mpaa->setChecked(settings.mpaa_enabled);
    if (_autoOutputFormat)
        _autoOutputFormat->setChecked(_control->cfg()->getBool(Config::AUTO_OUTPUT_FORMAT));
#ifdef DCS_SUPPORT
    if( _dcs ) {
        _dcs->setChecked( settings.dcs_type - bdcs_type_dcs_off );
        if ( ( settings.format != bvideo_format_ntsc && settings.format != bvideo_format_480p ) ) {
            _dcs->hide();
        }
    }
#endif

    pack();

    /* Set the focus back to the info panel, preferrably to the same control */
    if (focused && focused->parent() == this)
        focused->focus();
}

void OutputsPanel::apply()
{
    bdisplay_settings settings, display2_settings;
    bdecode_window_settings window_settings;
    bdisplay_extended_settings savesettings;
    int result = 0;
    bool confirm_needed = false;
    bvideo_format new_format=bvideo_format_ntsc,old_format=bvideo_format_ntsc;
    const BrutusDisplay *brutDisplay = _control->display();
    char rfChan[RF_LABEL_SIZE];
    char *pRf;

    _control->getOutput(&settings);
    bdisplay_save_settings(&savesettings, &settings);

    if (_format->visible()) {
        new_format = _pFormatList->get(_format->current())->format;
        if (new_format != settings.format) {
            old_format = settings.format;
            settings.format = new_format;
            confirm_needed = true;

            /* TODO: we only have one vesa mode in the list */
            if (settings.format == bvideo_format_vesa) {
                settings.vesa_settings.width = 640;
                settings.vesa_settings.height = 480;
                settings.vesa_settings.refresh_rate = 60;
                settings.vesa_settings.interlaced = false;
            }
        }
    }

#ifdef HAS_OUTPUT_SELECT
    /* If we're dual output or 2nd display, then we can't rearrange the outputs. */
    if (!_control->cfg()->getBool(Config::DUAL_OUTPUT) && _output->visible()) {
        int output_select = OutputsSelection[_output->current()].selection;

        if (output_select & OUTPUTS_COMPONENT) {
            boutput_component_settings cs;

            settings.component = boutput_component_open(B_ID(0));
            boutput_component_get(settings.component, &cs);
            cs.type = output_select & OUTPUTS_RGB ?
                boutput_component_type_rgb :
                boutput_component_type_yprpb;
            boutput_component_set(settings.component, &cs);
        }
        else {
            settings.component = NULL;
        }
        if (output_select & OUTPUTS_SVIDEO) {
            settings.svideo = boutput_svideo_open(B_ID(0));
        }
        else {
            settings.svideo = NULL;
        }
    }
#endif

    if ( brutDisplay->display2 ) bdisplay_get( brutDisplay->display2, &display2_settings );

    if (settings.rf && _rfChannel->getItem()) {
        boutput_rf_settings rs;
        boutput_rf_get(settings.rf, &rs);
        rs.channel = _rfChannel->current() + 3;
        strcpy( rfChan, _rfChannel->getItem() );
        pRf = strchr( rfChan, '(' );
        rs.country[0] = *(pRf+1);
        rs.country[1] = *(pRf+2);

        boutput_rf_set(settings.rf, &rs);
    }
    else if ( brutDisplay->display2 && display2_settings.rf && _rfChannel->getItem()) {
        boutput_rf_settings rs;
        boutput_rf_get(display2_settings.rf, &rs);
        rs.channel = _rfChannel->current() + 3;
        strcpy( rfChan, _rfChannel->getItem() );
        pRf = strchr( rfChan, '(' );
        rs.country[0] = *(pRf+1);
        rs.country[1] = *(pRf+2);
        boutput_rf_set(display2_settings.rf, &rs);
    }

    if (_is16x9->visible()) {
        settings.aspect_ratio = _is16x9->checked() ?
            bdisplay_aspect_ratio_16x9 : bdisplay_aspect_ratio_4x3;
    }
    settings.content_mode = (bdisplay_content_mode)_aspectRatio->current();

    if ( brutDisplay->display2 ){
      display2_settings.content_mode = (bdisplay_content_mode)_aspectRatio_SD->current();
      bdisplay_set(brutDisplay->display2, &display2_settings);
    }

    if (_mpaa)
        settings.mpaa_enabled = _mpaa->checked();

    if ((_mad || _boxdetect) && _control->window()) {
        bdecode_window_get(_control->window(), &window_settings);
        if (_mad)
            window_settings.deinterlacer = _mad->checked();
        if (_boxdetect)
            window_settings.box_detect = _boxdetect->checked();
        if (brutDisplay->s3dTv.output==Brutus3DTVMode_eOff) {
            result = bdecode_window_set(_control->window(), &window_settings);
        }
    }

    if ( _autoOutputFormat && _autoOutputFormat->checked() != _control->cfg()->getBool(Config::AUTO_OUTPUT_FORMAT) )
    {
        _control->cfg()->set(Config::AUTO_OUTPUT_FORMAT, _autoOutputFormat->checked());
        if (_autoOutputFormat->checked())
        {
            bdisplay_settings tmpSettings = settings;

            _control->getOptimalDisplay(&tmpSettings);
            if ( tmpSettings.format != settings.format )
            {
                settings.format = tmpSettings.format;
                confirm_needed = true;

                /* Check settings for vesa formats */
                if (settings.format == bvideo_format_vesa) {
                    settings.vesa_settings = tmpSettings.vesa_settings;
                }
            }
        }
    }

#ifdef DCS_SUPPORT
    settings.dcs_type = bdcs_type_dcs_off;      /* Default off */
    if( _dcs && ( settings.format == bvideo_format_ntsc || settings.format == bvideo_format_480p)  ) {
        /* put something in here */
        _dcs->show();
        if( _dcs->checked() ) {
            settings.dcs_type = bdcs_type_dcs_on;
            BDBG_WRN(("Set DCS ON !"));
        }
    }
    else if (_dcs) {
        if( confirm_needed ) {
            /* need to disable DCS before changing to an unsupported format */
            settings.dcs_type = bdcs_type_dcs_off;
            settings.format = old_format;
            result = !_control->setOutput(&settings);
            settings.format = new_format;
        }
        _dcs->hide();
    }
#endif

    if (!result)
        result = !_control->setOutput(&settings);

    if (result) {
        bdisplay_restore_settings(&settings, &savesettings);
        _control->setOutput(&settings);
        MMessageBox::info(fb(), "Unable to set output settings");
        return;
    }

    if (confirm_needed) {
        if (MMessageBox::show(fb(), "Accept this setting? Settings will revert in 20 seconds.",
                MMessageBox::bYes|MMessageBox::bNo,
                MMessageBox::bNo,
                20 * 1000) == MMessageBox::bNo)
        {
            // restore
            bdisplay_restore_settings(&settings, &savesettings);
            _control->setOutput(&settings);
            MMessageBox::info(fb(), "Restored settings");
            return;
        }
    }
}

/////////////////////////////////////////////////////////////////

#include "cfgsettings.h"

#define PIXMAPPATH (MString(infoBar()->control()->cfg()->get(Config::IMAGES_PATH)) + "/info/")

OutputsInfoBarPlugin::OutputsInfoBarPlugin(InfoBar *parent) :
    InfoBarPlugin(parent, "display_plugin")
{
    setText("Display");

    _panel = new OutputsPanel(infoBar()->control(), topLevelWidget(), MRect(230,25,BARWIDTH+INDENT*2,295));
    _linkedWidget = _panel;
    _panel->hide();
    _icon = new MImage(app(), PIXMAPPATH + "display.png");
    infoBar()->control()->addControlEventHandler(this);
}

OutputsInfoBarPlugin::~OutputsInfoBarPlugin()
{
    infoBar()->control()->removeControlEventHandler(this);
    delete _icon;
    delete _panel;
}

int OutputsInfoBarPlugin::setFormat(int i, bdisplay_settings *settings)
{
    if (i >= 0 && i < totalFormats()) {
        BDBG_WRN(("setFormat %d", i));
        settings->format = g_mainFormatList[i]->format;
        /* TODO */
        if (settings->format == bvideo_format_vesa) {
            settings->vesa_settings.width = 640;
            settings->vesa_settings.height = 480;
            settings->vesa_settings.refresh_rate = 60;
            settings->vesa_settings.interlaced = false;
        }
        return 0;
    }
    return -1;
}

int OutputsInfoBarPlugin::totalFormats()
{
    return g_mainFormatList.total();
}

int OutputsInfoBarPlugin::getFormatIndex(const bdisplay_settings *settings)
{
    int i, total;

    total = totalFormats();

    for ( i=0; i < total; i++)
    {
        if (settings->format == g_mainFormatList[i]->format)
            return i;
    }
    return -1;
}

int OutputsInfoBarPlugin::setFormat(const char *format,
    bdisplay_settings *settings)
{
    int i, j, total;

    total = totalFormats();

    /* Do an exact text match, case insenstive */
    for ( i=0; i<totalFormats(); i++ )
    {
        VideoFormatDescription *pDesc = g_mainFormatList[i];

        for ( j=0; j<MAX_NAMES_PER_FORMAT && pDesc->pszFormatNames[j]; j++ )
        {
            if ( !strcasecmp(pDesc->pszFormatNames[j], format) )
            {
                settings->format = pDesc->format;
                if (settings->format == bvideo_format_vesa) {
                    settings->vesa_settings.width = 640;
                    settings->vesa_settings.height = 480;
                    settings->vesa_settings.refresh_rate = 60;
                    settings->vesa_settings.interlaced = false;
                }
                return 0;
            }
        }
    }

    /* Special vesa modes */
    for (int i=0;gVesaFormatTable[i].name;i++)
        if (! strcasecmp(gVesaFormatTable[i].name, format)) {
            settings->format = bvideo_format_vesa;
            settings->vesa_settings.width = gVesaFormatTable[i].width;
            settings->vesa_settings.height = gVesaFormatTable[i].height;
            settings->vesa_settings.refresh_rate = gVesaFormatTable[i].refresh;
            settings->vesa_settings.interlaced = gVesaFormatTable[i].interlaced;
            return 0;
        }

    BDBG_ERR(("Unrecognized format: '%s'", format));
    return -1;
}

int OutputsInfoBarPlugin::setAspectRatio(const char *format,bool HD, bdisplay_settings *settings)
{
    const char** AR_list; 
    int AR_MAX;
    if(HD){
        AR_list = AspectRatios;
        AR_MAX = (int)(sizeof(AspectRatios)/sizeof(*AspectRatios))-1;
        
    } else {
        AR_list = SDAspectRatios;
        AR_MAX = (int)(sizeof(SDAspectRatios)/sizeof(*SDAspectRatios))-1;    
    }

    for(int i = 0; i < AR_MAX; i++){
        if(! strcasecmp(format, &AR_list[i][strlen("xD A/R: ")]) ){
            BDBG_WRN(("Settings %s Aspect Ratio to %s", (HD?"HD":"SD"), format));
            settings->content_mode = (bdisplay_content_mode)i;
            return 0;
        }
    }
    
    BDBG_ERR(("Unrecognized aspect ratio format: %s", format));
    return -1;
}

void OutputsInfoBarPlugin::onControlEvent(Control *control, Control::Event event)
{
    /* whenever the output format changes, update the ui. this allows non-info panel changes to be
    reflected in the info panel */
    if (event == Control::evSetOutput)
    {
        ((OutputsPanel *)_panel)->read();
    }
}

void OutputsPanel::buildVideoFormatTable(bool isSecondDisplay, bool showAllFormats )
{
    /* To build a format table, we must cross-reference what's possible on this chip via bconfig */
    int i;
    /* Grab correct list */
    MList<VideoFormatDescription> *pList = (isSecondDisplay)?&g_secondFormatList:&g_mainFormatList;

    if ( pList->total() > 0 )
    {
        /* Done already... */
        return;
    }

    for ( i = 0; i < bvideo_format_count; i++ )
    {
        if ( bconfig->features.video_format_is_supported[i] &&  /* If format is supported */
             ( showAllFormats || g_videoFormats[i].visible ) &&                          /* And not hidden... */
             !(isSecondDisplay && g_videoFormats[i].mainDisplayOnly) /* And if displayable on this display */ )
        {
            /* Add to the list */
            pList->add(g_videoFormats+i);
        }
    }

    /* Sort at the end... */
    pList->sort(formatSort);
}

static int formatSort(VideoFormatDescription *pFirst, VideoFormatDescription *pSecond)
{
    /* Sort swaps if the return value is 1 */
    if ( pFirst->uiSortOrder > pSecond->uiSortOrder )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

struct Iso3166 {
    const char *pszFullName;           /* Full name of country according to ISO 3166 */
    const char *pszTwoLetter;          /* Two letter code for country according to ISO 3166 */
    bvideo_format vhf_format;          /* Composite Video format for vhf modulation for given country */
};

/* TODO: Enries with "Guess" as comment need bvideo_format_xx correcting, when true format is known */
Iso3166 countryLookUp[] = {
    { "AFGHANISTAN",    "AF", bvideo_format_pal_d  },
    { "LAND ISLANDS",  "AX", bvideo_format_ntsc   },
    { "ALBANIA",        "AL", bvideo_format_pal_b  },
    { "ALGERIA",        "DZ", bvideo_format_pal_b  },
    { "AMERICAN SAMOA", "AS" , bvideo_format_ntsc  }, /* Guess */
    { "ANDORRA",        "AD", bvideo_format_ntsc   }, /* Guess */
    { "ANGOLA",         "AO", bvideo_format_pal_i  },
    { "ANGUILLA",       "AI", bvideo_format_ntsc   }, /* Guess */
    { "ANTARCTICA",     "AQ", bvideo_format_ntsc   },
    { "ANTIGUA AND BARBUDA", "AG", bvideo_format_ntsc  }, /* Guess */
    { "ARGENTINA",      "AR", bvideo_format_pal_n  },
    { "ARMENIA",        "AM", bvideo_format_ntsc   }, /* Guess */
    { "ARUBA",          "AW", bvideo_format_ntsc   }, /* Guess */
    { "AUSTRALIA",      "AU", bvideo_format_pal_b  },
    { "AUSTRIA",        "AT", bvideo_format_pal_b  },
    { "AZERBAIJAN",     "AZ", bvideo_format_ntsc   }, /* Guess */
    { "BAHAMAS",        "BS", bvideo_format_ntsc   }, /* Guess */
    { "BAHRAIN",        "BH", bvideo_format_pal_b  },
    { "BANGLADESH",     "BD", bvideo_format_pal_b  },
    { "BARBADOS",       "BB", bvideo_format_ntsc   }, /* Guess */
    { "BELARUS",        "BY", bvideo_format_secam  }, /* Guess */
    { "BELGIUM",        "BE", bvideo_format_pal_b  },
    { "BELIZE",         "BZ", bvideo_format_ntsc   }, /* Guess */
    { "BENIN",          "BJ", bvideo_format_ntsc   }, /* Guess */
    { "BERMUDA",        "BM", bvideo_format_pal_m  },
    { "BHUTAN",         "BT", bvideo_format_ntsc   }, /* Guess */
    { "BOLIVIA",        "BO", bvideo_format_ntsc   },
    { "BOSNIA AND HERZEGOVINA", "BA", bvideo_format_ntsc  }, /* Guess */
    { "BOTSWANA",       "BW", bvideo_format_pal_i  },
    { "BOUVET ISLAND",  "BV", bvideo_format_ntsc   }, /* Guess */
    { "BRAZIL",         "BR", bvideo_format_pal_m  },
    { "BRITISH INDIAN OCEAN TERRITORY", "IO", bvideo_format_pal_i  }, /* Guess */
    { "BRUNEI DARUSSALAM", "BN", bvideo_format_pal_b  },
    { "BULGARIA",       "BG", bvideo_format_pal_d  },
    { "BURKINA FASO",   "BF", bvideo_format_secam  },
    { "BURUNDI",        "BI", bvideo_format_secam  },
    { "CAMBODIA",       "KH", bvideo_format_ntsc   }, /* Guess */
    { "CAMEROON",       "CM", bvideo_format_pal_b  },
    { "CANADA",         "CA", bvideo_format_ntsc   },
    { "CAPE VERDE",     "CV", bvideo_format_ntsc   }, /* Guess */
    { "CAYMAN ISLANDS", "KY", bvideo_format_pal_i  }, /* Guess */
    { "CENTRAL AFRICAN REPUBLIC", "CF", bvideo_format_secam  },
    { "CHAD",           "TD", bvideo_format_secam  },
    { "CHILE",          "CL", bvideo_format_ntsc   },
    { "CHINA",          "CN", bvideo_format_pal_d  },
    { "CHRISTMAS ISLAND", "CX", bvideo_format_ntsc  }, /* Guess */
    { "COCOS (KEELING) ISLANDS", "CC", bvideo_format_ntsc  }, /* Guess */
    { "COLOMBIA",       "CO", bvideo_format_ntsc  },
    { "COMOROS",        "KM", bvideo_format_ntsc  }, /* Guess */
    { "CONGO",          "CG", bvideo_format_secam  },
    { "CONGO, THE DEMOCRATIC REPUBLIC OF THE", "CD", bvideo_format_secam  }, /* Guess */
    { "COOK ISLANDS",   "CK", bvideo_format_ntsc   }, /* Guess */
    { "COSTA RICA",     "CR", bvideo_format_ntsc   },
    { "CTE D'IVOIRE",  "CI", bvideo_format_secam  },
    { "CROATIA",        "HR", bvideo_format_secam  }, /* Guess */
    { "CUBA",           "CU", bvideo_format_ntsc   },
    { "CYPRUS",         "CY", bvideo_format_secam  },
    { "CZECH REPUBLIC", "CZ", bvideo_format_secam  },
    { "DENMARK",        "DK", bvideo_format_pal_b  },
    { "DJIBOUTI",       "DJ", bvideo_format_secam  },
    { "DOMINICA",       "DM", bvideo_format_ntsc   }, /* Guess */
    { "DOMINICAN REPUBLIC", "DO", bvideo_format_ntsc  },
    { "ECUADOR",        "EC", bvideo_format_ntsc   },
    { "EGYPT",          "EG", bvideo_format_secam  },
    { "EL SALVADOR",    "SV", bvideo_format_ntsc   }, /* Guess */
    { "EQUATORIAL GUINEA", "GQ", bvideo_format_pal_b  },
    { "ERITREA",        "ER", bvideo_format_ntsc   }, /* Guess */
    { "ESTONIA",        "EE", bvideo_format_secam  }, /* Guess */
    { "ETHIOPIA",       "ET", bvideo_format_pal_b  },
    { "FALKLAND ISLANDS (MALVINAS)", "FK", bvideo_format_pal_i  }, /* Guess */
    { "FAROE ISLANDS",  "FO", bvideo_format_pal_b  },
    { "FIJI",           "FJ", bvideo_format_ntsc   }, /* Guess */
    { "FINLAND",        "FI", bvideo_format_pal_b  },
    { "FRANCE",         "FR", bvideo_format_secam  },
    { "FRENCH GUIANA",  "GF", bvideo_format_secam  }, /* Guess */
    { "FRENCH POLYNESIA", "PF", bvideo_format_secam  }, /* Guess */
    { "FRENCH SOUTHERN TERRITORIES", "TF", bvideo_format_secam  }, /* Guess */
    { "GABON",          "GA", bvideo_format_secam  },
    { "GAMBIA",         "GM", bvideo_format_ntsc  }, /* Guess */
    { "GEORGIA",        "GE", bvideo_format_secam  }, /* Guess */
    { "GERMANY",        "DE", bvideo_format_pal_b  },
    { "GHANA",          "GH", bvideo_format_pal_b  },
    { "GIBRALTAR",      "GI", bvideo_format_pal_b  },
    { "GREECE",         "GR", bvideo_format_secam  },
    { "GREENLAND",      "GL", bvideo_format_pal_b  },
    { "GRENADA",        "GD", bvideo_format_ntsc  },
    { "GUADELOUPE",     "GP", bvideo_format_ntsc  }, /* Guess */
    { "GUAM",           "GU", bvideo_format_ntsc  }, /* Guess */
    { "GUATEMALA",      "GT", bvideo_format_ntsc  },
    { "GUERNSEY",       "GG", bvideo_format_pal_i  },
    { "GUINEA",         "GN", bvideo_format_secam  },
    { "GUINEA-BISSAU",  "GW", bvideo_format_secam  },
    { "GUYANA",         "GY", bvideo_format_ntsc  }, /* Guess */
    { "HAITI",          "HT", bvideo_format_ntsc  },
    { "HEARD ISLAND AND MCDONALD ISLANDS", "HM", bvideo_format_ntsc  }, /* Guess */
    { "HOLY SEE (VATICAN CITY STATE)", "VA", bvideo_format_pal_b  }, /* Guess */
    { "HONDURAS",       "HN", bvideo_format_ntsc  },
    { "HONG KONG",      "HK", bvideo_format_pal_i  },
    { "HUNGARY",        "HU", bvideo_format_secam  },
    { "ICELAND",        "IS", bvideo_format_pal_b  },
    { "INDIA",          "IN", bvideo_format_pal_b  },
    { "INDONESIA",      "ID", bvideo_format_pal_b  },
    { "IRAN, ISLAMIC REPUBLIC OF", "IR", bvideo_format_secam  },
    { "IRAQ",           "IQ", bvideo_format_secam  },
    { "IRELAND",        "IE", bvideo_format_pal_i  },
    { "ISLE OF MAN",    "IM", bvideo_format_pal_i  },
    { "ISRAEL",         "IL", bvideo_format_pal_b  },
    { "ITALY",          "IT", bvideo_format_pal_b  },
    { "JAMAICA",        "JM", bvideo_format_ntsc  },
    { "JAPAN",          "JP", bvideo_format_ntsc_japan  },
    { "JERSEY",         "JE", bvideo_format_pal_i  },
    { "JORDAN",         "JO", bvideo_format_pal_b  },
    { "KAZAKHSTAN",     "KZ", bvideo_format_secam  }, /* Guess */
    { "KENYA",          "KE", bvideo_format_pal_b  },
    { "KIRIBATI",       "KI", bvideo_format_ntsc  }, /* Guess */
    { "KOREA, DEMOCRATIC PEOPLE'S REPUBLIC OF", "KP", bvideo_format_pal_d  },
    { "KOREA, REPUBLIC OF", "KR", bvideo_format_ntsc  },
    { "KUWAIT",         "KW", bvideo_format_pal_b  },
    { "KYRGYZSTAN",     "KG", bvideo_format_secam  }, /* Guess */
    { "LAO PEOPLE'S DEMOCRATIC REPUBLIC", "LA", bvideo_format_ntsc  }, /* Guess */
    { "LATVIA",         "LV", bvideo_format_ntsc  }, /* Guess */
    { "LEBANON",        "LB", bvideo_format_secam  },
    { "LESOTHO",        "LS", bvideo_format_pal_i  },
    { "LIBERIA",        "LR", bvideo_format_pal_b  },
    { "LIBYAN ARAB JAMAHIRIYA", "LY", bvideo_format_pal_b  },
    { "LIECHTENSTEIN",  "LI", bvideo_format_secam  }, /* Guess */
    { "LITHUANIA",      "LT", bvideo_format_secam  }, /* Guess */
    { "LUXEMBOURG",     "LU", bvideo_format_pal_b  },
    { "MACAO",          "MO", bvideo_format_ntsc  }, /* Guess */
    { "MACEDONIA, THE FORMER YUGOSLAV REPUBLIC OF", "MK", bvideo_format_secam  }, /* Guess */
    { "MADAGASCAR",     "MG", bvideo_format_secam  },
    { "MALAWI",         "MW", bvideo_format_pal_b  },
    { "MALAYSIA",       "MY", bvideo_format_pal_b  },
    { "MALDIVES",       "MV", bvideo_format_pal_b  },
    { "MALI",           "ML", bvideo_format_secam  },
    { "MALTA",          "MT", bvideo_format_pal_b  },
    { "MARSHALL ISLANDS", "MH", bvideo_format_ntsc  }, /* Guess */
    { "MARTINIQUE",     "MQ", bvideo_format_ntsc  }, /* Guess */
    { "MAURITANIA",     "MR", bvideo_format_secam  },
    { "MAURITIUS",      "MU", bvideo_format_secam  },
    { "MAYOTTE",        "YT", bvideo_format_ntsc  }, /* Guess */
    { "MEXICO",         "MX", bvideo_format_ntsc  },
    { "MICRONESIA, FEDERATED STATES OF", "FM", bvideo_format_ntsc  }, /* Guess */
    { "MOLDOVA, REPUBLIC OF", "MD", bvideo_format_ntsc  }, /* Guess */
    { "MONACO",         "MC", bvideo_format_secam  },
    { "MONGOLIA",       "MN", bvideo_format_secam  },
    { "MONTENEGRO",     "ME", bvideo_format_ntsc  },
    { "MONTSERRAT",     "MS", bvideo_format_ntsc  },
    { "MOROCCO",        "MA", bvideo_format_secam  },
    { "MOZAMBIQUE",     "MZ", bvideo_format_pal_b  },
    { "MYANMAR",        "MM", bvideo_format_ntsc  },
    { "NAMIBIA",        "NA", bvideo_format_ntsc  }, /* Guess */
    { "NAURU",          "NR", bvideo_format_ntsc  }, /* Guess */
    { "NEPAL",          "NP", bvideo_format_pal_b  }, /* Guess */
    { "NETHERLANDS",    "NL", bvideo_format_pal_b  },
    { "NETHERLANDS ANTILLES", "AN",bvideo_format_pal_b  },
    { "NEW CALEDONIA",  "NC", bvideo_format_pal_b  }, /* Guess */
    { "NEW ZEALAND",    "NZ", bvideo_format_pal_b  },
    { "NICARAGUA",      "NI", bvideo_format_ntsc  },
    { "NIGER",          "NE", bvideo_format_secam  },
    { "NIGERIA",        "NG", bvideo_format_pal_b  },
    { "NIUE",           "NU", bvideo_format_ntsc  }, /* Guess */
    { "NORFOLK ISLAND", "NF", bvideo_format_ntsc  }, /* Guess */
    { "NORTHERN MARIANA ISLANDS", "MP", bvideo_format_ntsc  }, /* Guess */
    { "NORWAY",         "NO", bvideo_format_pal_b  },
    { "OMAN",           "OM", bvideo_format_pal_b  },
    { "PAKISTAN",       "PK", bvideo_format_pal_b  },
    { "PALAU",          "PW", bvideo_format_ntsc  }, /* Guess */
    { "PALESTINIAN TERRITORY, OCCUPIED", "PS", bvideo_format_pal_b  }, /* Guess */
    { "PANAMA",         "PA", bvideo_format_ntsc  },
    { "PAPUA NEW GUINEA", "PG", bvideo_format_pal_b  },
    { "PARAGUAY",       "PY", bvideo_format_pal_n  },
    { "PERU",           "PE", bvideo_format_ntsc  },
    { "PHILIPPINES",    "PH", bvideo_format_ntsc  },
    { "PITCAIRN",       "PN", bvideo_format_ntsc  }, /* Guess */
    { "POLAND",         "PL", bvideo_format_pal_d  },
    { "PORTUGAL",       "PT", bvideo_format_pal_b  },
    { "PUERTO RICO",    "PR", bvideo_format_ntsc  },
    { "QATAR",          "QA", bvideo_format_pal_b  },
    { "RUNION",        "RE", bvideo_format_ntsc  },
    { "ROMANIA",        "RO", bvideo_format_pal_d  },
    { "RUSSIAN FEDERATION", "RU", bvideo_format_secam  },
    { "RWANDA",         "RW", bvideo_format_secam  },
    { "SAINT HELENA",   "SH", bvideo_format_ntsc  }, /* Guess */
    { "SAINT KITTS AND NEVIS", "KN", bvideo_format_ntsc  },
    { "SAINT LUCIA",    "LC", bvideo_format_ntsc  }, /* Guess */
    { "SAINT PIERRE AND MIQUELON", "PM", bvideo_format_ntsc  }, /* Guess */
    { "SAINT VINCENT AND THE GRENADINES", "VC", bvideo_format_ntsc  }, /* Guess */
    { "SAMOA",          "WS", bvideo_format_ntsc  }, /* Guess */
    { "SAN MARINO",     "SM", bvideo_format_ntsc  }, /* Guess */
    { "SAO TOME AND PRINCIPE", "ST", bvideo_format_ntsc  }, /* Guess */
    { "SAUDI ARABIA",   "SA", bvideo_format_secam  },
    { "SENEGAL",        "SN", bvideo_format_secam  },
    { "SERBIA",         "RS", bvideo_format_pal_b  }, /* Guess */
    { "SEYCHELLES",     "SC", bvideo_format_pal_b  }, /* Guess */
    { "SIERRA LEONE",   "SL", bvideo_format_pal_b  },
    { "SINGAPORE",      "SG", bvideo_format_pal_b  },
    { "SLOVAKIA",       "SK", bvideo_format_secam  },
    { "SLOVENIA",       "SI", bvideo_format_secam  }, /* Guess */
    { "SOLOMON ISLANDS","SB", bvideo_format_ntsc  }, /* Guess */
    { "SOMALIA",        "SO", bvideo_format_ntsc  }, /* Guess */
    { "SOUTH AFRICA",   "ZA", bvideo_format_pal_i  },
    { "SOUTH GEORGIA AND THE SOUTH SANDWICH ISLANDS", "GS", bvideo_format_pal_b  }, /* Guess */
    { "SPAIN",          "ES", bvideo_format_pal_b  },
    { "SRI LANKA",      "LK", bvideo_format_pal_b  },
    { "SUDAN",          "SD", bvideo_format_pal_b  },
    { "SURINAME",       "SR", bvideo_format_ntsc  },
    { "SVALBARD AND JAN MAYEN", "SJ", bvideo_format_ntsc  }, /* Guess */
    { "SWAZILAND",      "SZ", bvideo_format_pal_i  }, /* Guess */
    { "SWEDEN",         "SE", bvideo_format_pal_b  },
    { "SWITZERLAND",    "CH", bvideo_format_pal_b  },
    { "SYRIAN ARAB REPUBLIC", "SY", bvideo_format_pal_b },
    { "TAIWAN, PROVINCE OF CHINA", "TW", bvideo_format_ntsc  }, /* Guess */
    { "TAJIKISTAN",     "TJ", bvideo_format_secam  }, /* Guess */
    { "TANZANIA, UNITED REPUBLIC OF", "TZ", bvideo_format_pal_b  },
    { "THAILAND",       "TH", bvideo_format_pal_b  },
    { "TIMOR-LESTE",    "TL", bvideo_format_pal_b  }, /* Guess */
    { "TOGO",           "TG", bvideo_format_secam  },
    { "TOKELAU",        "TK", bvideo_format_pal_b  }, /* Guess */
    { "TONGA",          "TO", bvideo_format_pal_b  }, /* Guess */
    { "TRINIDAD AND TOBAGO", "TT", bvideo_format_pal_b  }, /* Guess */
    { "TUNISIA",        "TN", bvideo_format_pal_b  },
    { "TURKEY",         "TR", bvideo_format_pal_b  },
    { "TURKMENISTAN",   "TM", bvideo_format_pal_b  }, /* Guess */
    { "TURKS AND CAICOS ISLANDS", "TC", bvideo_format_pal_b  }, /* Guess */
    { "TUVALU",         "TV", bvideo_format_pal_b  }, /* Guess */
    { "UGANDA",         "UG", bvideo_format_pal_b  },
    { "UKRAINE",        "UA", bvideo_format_secam  }, /* Guess */
    { "UNITED ARAB EMIRATES", "AE", bvideo_format_pal_b  },
    { "UNITED KINGDOM", "GB", bvideo_format_pal_i  },
    { "UNITED STATES",  "US", bvideo_format_ntsc   },
    { "UNITED STATES MINOR OUTLYING ISLANDS", "UM", bvideo_format_ntsc  },
    { "URUGUAY",        "UY", bvideo_format_pal_n  },
    { "UZBEKISTAN",     "UZ", bvideo_format_secam  }, /* Guess */
    { "VANUATU",        "VU", bvideo_format_ntsc  },  /* Guess */
    { "VENEZUELA",      "VE", bvideo_format_ntsc  },
    { "VIET NAM",       "VN", bvideo_format_secam  },
    { "VIRGIN ISLANDS, BRITISH", "VG", bvideo_format_pal_i  }, /* Guess */
    { "VIRGIN ISLANDS, U.S.", "VI", bvideo_format_ntsc  },
    { "WALLIS AND FUTUNA", "WF", bvideo_format_ntsc  }, /* Guess */
    { "WESTERN SAHARA", "EH", bvideo_format_ntsc  },    /* Guess */
    { "YEMEN",          "YE", bvideo_format_pal_b  },
    { "ZAMBIA",         "ZM", bvideo_format_pal_b  },
    { "ZIMBABWE",       "ZW", bvideo_format_pal_b  }
};


int isValidCountryCode( char *pTwoLetterCountry ) {

    int i,rc=0;

    for( i=0; i < (int)(sizeof(countryLookUp)/sizeof(Iso3166)); i++ ) {
        if( !strcmp( countryLookUp[i].pszTwoLetter, pTwoLetterCountry ) ) {
            /* Found a match ! */
            rc = 1;
            break;
        }
    }
    return rc;
}

bvideo_format getVideoFormatForCountry( char *pTwoLetterCountry ) {

    bvideo_format rc = bvideo_format_ntsc;
    int i;

    for( i=0; i < (int)(sizeof(countryLookUp)/sizeof(Iso3166)); i++ ) {
        if( !strcmp( countryLookUp[i].pszTwoLetter, pTwoLetterCountry ) ) {
            /* Found a match ! */
            rc = countryLookUp[i].vhf_format;
            break;
        }
    }
    return rc;
}
