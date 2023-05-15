/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop.h $
 * $brcm_Revision: 108 $
 * $brcm_Date: 2/23/12 6:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop.h $
 * 
 * 108   2/23/12 6:53p jtna
 * SW7425-2473: add bsettop_uninit_frontend()
 * 
 * 107   5/23/11 3:37p jgarrett
 * SW7346-213: Adding btuner_untune
 * 
 * 106   3/1/10 10:42a jgarrett
 * SW7550-272: Bump Version
 * 
 * 105   1/27/10 10:02a jgarrett
 * SW7550-213: Version Increment
 * 
 * 104   11/19/09 8:02p jgarrett
 * SW7405-3357: Adding audio output routing for decode and pcm playback
 * prior to start
 * 
 * 103   11/10/09 3:43p jgarrett
 * SW7420-166: Adding graphics heap index
 * 
 * 102   10/5/09 11:06a jgarrett
 * SW7325-612: Fixing bitmasks for certain blending operations
 * 
 * 101   9/29/09 11:48a jgarrett
 * SW7405-3100: Adding display underflows and display drops to video
 * decode status
 * 
 * 100   9/14/09 3:05p jgarrett
 * SW7405-3011: Splitting sync control into precision and basic
 * 
 * 99   9/10/09 5:17p jgarrett
 * SW7405-3011: Adding sync options to bsettop_init_settings
 * 
 * 98   9/4/09 11:44a jgarrett
 * SWDEPRECATED-3910: Exposing audio and video FIFO sizes
 * 
 * 97   8/10/09 1:41p jgarrett
 * PR 51648: Adding option to set band gap settings in API
 * 
 * 96   7/2/09 10:18a jgarrett
 * PR 56558: Adding init settings routines
 *
 * 95   6/30/09 10:47a jgarrett
 * PR 52051: Increment version
 *
 * 94   5/15/09 3:22p jgarrett
 * PR 52687: Incrementing version
 *
 * 93   5/6/09 4:12p jgarrett
 * PR 50291: Adding wide GA mode
 *
 * SanDiego_Mot_DSR/PR52051/4   6/23/09 6:44p mward
 * PR 52051: Update.
 *
 * 94   5/15/09 3:22p jgarrett
 * PR 52687: Incrementing version
 *
 * 93   5/6/09 4:12p jgarrett
 * PR 50291: Adding wide GA mode

 * SanDiego_Mot_DSR/PR52051/3   4/10/09 3:45p mward
 * PR52051:  Bumping version.
 *
 * SanDiego_Mot_DSR/PR52051/2   3/27/09 3:25p mward
 * PR52051:  Branch for testing dialog normalization switch with new RAP
 * release.
 *
 * 92   3/18/09 12:26p jgarrett
 * PR 53340: Adding 1080p50
 *
 * 91   3/13/09 3:13p jgarrett
 * PR 49827: Merge to main branch
 *
 * 90   3/4/09 7:47p jgarrett
 * PR 52269: Adding XMP support
 *
 * 89   3/2/09 11:04a jgarrett
 * PR 52599: Adding 97205
 *
 * 88   2/20/09 1:40p jgarrett
 * PR 52355: Adding ANR
 *
 * Trinity_Xmp_Support/9   2/24/09 11:02a prasadv
 * Code clean up. moved XMP support to user io
 *
 * Trinity_Xmp_Support/8   2/6/09 3:33p prasadv
 * Removed the #ifdef for xmp header file.
 *
 * Trinity_Xmp_Support/7   1/28/09 7:48p prasadv
 * Updating to the latest
 *
 * PR49827/4   3/12/09 10:24a mward
 * PR49827: bumping version.
 *
 * PR49827/3   2/26/09 10:28a mward
 * PR52490: Bumping version
 *
 * PR49827/2   2/19/09 11:31a mward
 * PR49827: Bumping version
 *
 * PR49827/1   1/19/09 3:19p mward
 * PR49827: Bumping version
 *
 * 87   12/18/08 6:37p jgarrett
 * PR 49983: Bumping version
 *
 * 86   11/14/08 5:33p jgarrett
 * PR 48985: Bumping version
 *
 * 85   11/6/08 10:30a jgarrett
 * PR 48700: Revising static library link order
 *
 * 84   10/31/08 9:41a jgarrett
 * PR 47230: Bumping version number
 *
 * 83   9/17/08 4:16p jgarrett
 * PR 47061: Version increment
 *
 * Trinity_Xmp_Support/6   11/11/08 4:38p prasadv
 * 97456 5.1 release as per version 82
 *
 * 82   9/8/08 10:43a vsilyaev
 * PR 45615: Bumped revision number
 *
 * 81   8/14/08 12:09p jgarrett
 * PR 32047: Bumping version
 *
 * 80   7/23/08 3:19p jgarrett
 * PR 43426: Bumping version
 *
 * 79   7/21/08 3:55p jgarrett
 * PR 42739: Bumping version
 *
 * 78   7/21/08 2:36p jgarrett
 * PR 45013: Incrementing version
 *
 * Trinity_Xmp_Support/5   10/29/08 6:13p prasadv
 * for 97118 6.0 release based on version 77
 *
 * 77   7/14/08 1:24p jgarrett
 * PR 36285: Bumping BSETTOP_VERSION
 *
 * 76   6/25/08 12:01p jgarrett
 * PR43426: PR43426 : Add 7043 Transcode support
 *
 * PR43426/1   6/24/08 6:25p shyam
 * PR43426 : Add 7043 Transcode support
 *
 * Trinity_Xmp_Support/4   7/21/08 11:04a prasadv
 * 97458 6.0 Take2 Release based on v74
 *
 * Trinity_Xmp_Support/3   6/27/08 9:47p prasadv
 * 97456 5.0 release based on version 75
 *
 * 75   5/28/08 2:54p vsilyaev
 * PR 42365: Bumped API version
 *
 * 74   4/11/08 3:49p vsilyaev
 * PR 40987:  Bumping version number
 *
 * 73   3/31/08 6:26p jgarrett
 * PR 40987: Bumping version number
 *
 * 72   3/25/08 10:13a jgarrett
 * PR 40857: Incrementing version
 *
 * 71   3/12/08 11:20a jgarrett
 * PR 36248: Incrementing version
 *
 * 70   3/3/08 9:57a jgarrett
 * PR 36787: Bumping version
 *
 * 69   2/21/08 1:45p vsilyaev
 * PR 36787: Bumped version number
 *
 * Trinity_Xmp_Support/2   6/26/08 10:58a prasadv
 * for 97118 5.2 release based on version 64
 *
 * Trinity_Xmp_Support/1   6/24/08 8:34p prasadv
 * for XMP Support under trinity branch
 *
 * 68   1/31/08 5:17p jgarrett
 * PR 39180: Adding ofdm tuning
 *
 * 67   1/21/08 11:02a jgarrett
 * PR 38591: Incrementing version number
 *
 * 66   1/9/08 11:22a jgarrett
 * PR 36809: Incrementing version
 *
 * 65   12/21/07 11:44a vsilyaev
 * PR 38269: Bumped version number
 *
 * 64   12/12/07 11:11a vsilyaev
 * PR 36971: Bumped version number
 *
 * 63   11/26/07 5:24p vsilyaev
 * PR 35854: Added memory management interface
 *
 * 62   11/9/07 2:19p jgarrett
 * PR 36308: Bumping version
 *
 * 61   10/25/07 11:26a jgarrett
 * PR 28004: Bumping version number
 *
 * 60   10/25/07 9:20a jgarrett
 * PR 28004: Bumping version number
 *
 * 59   9/14/07 3:30p jgarrett
 * PR30689: PR30689: Add second CVBS output support in one display on
 * 7400.
 *
 * PR30689/1   9/14/07 11:12a mward
 * PR30689: Add second CVBS output support in one display on 7400.
 *
 * 58   9/12/07 10:13a jgarrett
 * PR 34578: Bumping BSETTOP_VERSION
 *
 * 57   8/31/07 5:00p jgarrett
 * PR 32044: Updating BSETTOP_VERSION
 *
 * 56   8/13/07 5:25p jgarrett
 * PR 25900: Bumping version number
 *
 * 55   7/24/07 4:03p jgarrett
 * PR 25900: Bumping version number
 *
 * 54   7/11/07 6:48p jgarrett
 * PR 20139: Bumping version number
 *
 * 53   7/3/07 9:08a erickson
 * PR25109: bumped BSETTOP_VERSION for 97401 Phase 6.0
 *
 * 52   6/28/07 2:18p vsilyaev
 * PR 32275: Fixed order of include files (all SettopApi include shall be
 * included in the bsettop.h and bsettop_impl.h)
 *
 * 51   6/14/08 10:11a jgarrett
 * PR 32123: Adding bsettop_compat.h
 *
 * 50   5/24/07 10:36a gmohile
 * PR 25109: increment version for samsung release
 *
 * 49   5/7/07 1:32p erickson
 * PR25109: bumped BSETTOP_VERSION for 97401 Phase 5.0
 *
 * 48   4/30/07 10:21a gmohile
 * PR 25109: increment version number for release
 *
 * 47   4/24/07 12:07p jgarrett
 * PR 20139: Bumping version number for release
 *
 * 46   3/23/07 8:01a jgarrett
 * PR 25900: Bumping version for release
 *
 * 45   3/9/07 12:18a erickson
 * PR24374: bumped BSETTOP_VERSION for 97401 Phase 4.0
 *
 * 44   2/12/07 5:02p jgarrett
 * PR 20139: Bumping version number for update
 *
 * 43   2/8/07 3:45p jgarrett
 * PR 27751: Bumping revision number for PVR encryption flag
 *
 * 42   1/30/07 2:17p vsilyaev
 * PR 19687: Added power management functions
 *
 * 41   1/22/07 9:46a gmohile
 * PR 25109: increment BSETTOP_VERSION for 7403
 *
 * 40   12/21/06 4:26p vsilyaev
 * PR26258: add cablecard support in Settop API kernel mode
 *
 * PROD_TRINITY_REFSW_Devel/1   12/19/06 3:31p haisongw
 * PR26258: add cablecard support in Settop API kernel mode
 *
 * 38   12/18/06 9:24a erickson
 * PR24374: increment BSETTOP_VERSION for 97401 phase 3.5 (2nd time)
 *
 * 37   12/13/06 4:27p erickson
 * PR24374: increment BSETTOP_VERSION for 97401 phase 3.5
 *
 * 36   12/7/06 5:33p mward
 * PR23262: update 97118 build (Phase 1.0 release)
 *
 * 35   11/28/06 7:48p dlwin
 * PR 25582: Bump rev. for release.
 *
 * 34   11/7/06 9:35a erickson
 * PR24374: increased BSETTOP_VERSION for 97401 Phase 3.0
 *
 * 33   10/24/06 1:05p mward
 * PR21671: Increment for 97118 release.
 *
 * 32   10/18/06 4:09p jgarrett
 * PR 20139: Updating BSETTOP_VERSION for release
 *
 * 31   10/10/06 3:11p dlwin
 * PR24374: 97401 phase 2.5 release, take 2
 *
 * 30   9/21/06 5:31p erickson
 * PR24374: 97401 phase 2.5 release
 *
 * 29   9/18/06 4:54p dlwin
 * PR 24218: bump rev. for release.
 *
 * 28   8/23/06 9:21a jgarrett
 * PR 20139: Bumping version number for release
 *
 * 27   7/31/06 11:11a dlwin
 * PR 23090: Update for 97398 Phase 7.0.
 *
 * 26   7/19/06 6:29p vsilyaev
 * PR 20308: Bumped release number
 *
 * 25   7/11/06 5:56p mphillip
 * PR19544: Bumping the version number once again, due to API changes
 *
 * 24   6/22/06 2:36p jgarrett
 * PR 20139: Updating version number for release
 *
 * 23   6/8/06 5:45p mphillip
 * PR20908: API changes mean bsettop.h needs bumping
 *
 * 22   5/30/06 12:05p jgarrett
 * PR 20139: Incrementing version for release.
 *
 * 21   5/15/06 2:23p vsilyaev
 * PR 20308: Bumped version number
 *
 * 20   4/25/06 3:25p vsilyaev
 * PR 20308: Bumped version number
 *
 * 19   4/18/06 4:10p jgarrett
 * PR 20139: Updating version number for release
 *
 * 18   3/15/06 9:07a erickson
 * PR17108: bump version for release
 *
 * 17   2/28/06 10:06a mphillip
 * PR19927: Version bump for release
 *
 * 16   1/10/06 10:20a erickson
 * PR17108: bump BSETTOP_VERSION
 *
 * 15   12/19/05 12:20p vsilyaev
 * PR 17883: Bumped version number
 *
 * 14   12/2/05 2:29p vsilyaev
 * PR 17883: Bumped version number
 *
 * 13   10/31/05 6:07p vsilyaev
 * PR 17883: Bumped version number
 *
 * 12   10/5/06 4:22p vsilyaev
 * PR16538,PR12828: Bumped version
 *
 * 11   8/25/05 10:54a erickson
 * PR16819: inc VERSION
 *
 * 10   8/1/05 12:23p erickson
 * PR16300: incremented BSETTOP_VERSION for bplaypump changes
 *
 * 9   7/28/05 2:45p mphillip
 * PR15145: Time for a bump
 *
 * 8   6/15/05 11:25a mphillip
 * PR15896: Remove two const declarations (and add a "this structure
 * should be treated as read-only" comment) to get bconfig_copy to work
 * under 2.7
 *
 * 7   6/9/05 3:24p erickson
 * PR15513: all pass record is now audio pid 0 or 0x1fff. large warnings
 * now printed.
 *
 * 6   6/3/05 12:06p vsilyaev
 * PR 15740: Fix-up for version log.
 *
 * 4   5/20/05 11:20a mphillip
 * PR15267: Public API change means bumping the settop api version
 *
 * 3   5/13/05 12:18p vsilyaev
 * PR 15253:  Bumped version number
 *
 * 2   5/2/05 12:09p erickson
 * PR15145: inc BSETTOP_VERSION to make bsettop.h the latest
 *
 * 1   2/7/05 7:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/28   11/23/04 6:53p vsilyaev
 * PR 13351: Bumped version number to reflect changes in the PVR API.
 *
 * Irvine_BSEAVSW_Devel/27   11/4/04 6:54p vsilyaev
 * PR 13097: Bumped version number (ABI change in the size of mpeg_params
 * structure).
 *
 * Irvine_BSEAVSW_Devel/26   10/25/04 3:28p vsilyaev
 * PR 12665: Merge from OTFPVR branch
 *
 * Irvine_BSEAVSW_Devel/BESOTFPVR/1   10/8/04 7:50p vsilyaev
 * Include rec/play pump before pvr.
 *
 * Irvine_BSEAVSW_Devel/25   6/10/04 8:08a erickson
 * PR11135: brecpump implemented and brecord converted to use recpump
 *
 * Irvine_BSEAVSW_Devel/24   6/8/04 7:40a erickson
 * PR11135: added iniital playpump impl for 97038
 *
 * Irvine_BSEAVSW_Devel/23   5/3/04 11:21a vsilyaev
 * PR 10927: Bumped minore version number (changed layout of bconfig).
 *
 * Irvine_BSEAVSW_Devel/22   1/20/04 9:23a vsilyaev
 * PR 9383: Bumped version number.
 *
 * Irvine_BSEAVSW_Devel/Irvine_BSEAVSW_Devel_7038/1   12/10/03 6:33p vsilyaev
 * Added bsettop_uninit function.
 *
 * Irvine_BSEAVSW_Devel/20   11/14/03 2:10p erickson
 * PR8612: added vbi support to settop api
 *
 * Irvine_BSEAVSW_Devel/19   10/31/03 11:58a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/18   10/28/03 11:26a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/17   10/8/03 11:46a erickson
 * added initial implementation for smartcard
 *
 * Irvine_BSEAVSW_Devel/16   9/30/03 1:13a erickson
 * updated docjet documentation
 *
 * Irvine_BSEAVSW_Devel/15   9/25/03 1:28p erickson
 * added cpu-based encryption to settop api and util
 *
 * Irvine_BSEAVSW_Devel/14   9/24/03 11:51a erickson
 * initial pcm play implementation
 *
 * Irvine_BSEAVSW_Devel/13   9/18/03 5:05p erickson
 * reworked settop api, driver and brutus directory structure
 *
 * Irvine_BSEAVSW_Devel/12   9/17/03 5:04p erickson
 * updated documentation for DocJet
 *
 * Irvine_BSEAVSW_Devel/11   8/13/03 11:37a erickson
 * added more examples
 *
 * Irvine_BSEAVSW_Devel/10   8/1/03 5:34p vsilyaev
 * Added extern C { } brackets.
 *
 * Irvine_BSEAVSW_Devel/9   7/30/03 7:16p vsilyaev
 * added bsettop_config.h header.
 *
 * Irvine_BSEAVSW_Devel/8   7/30/03 2:35p vsilyaev
 * Rearanged headers into the alphabetical order.
 *
 * Irvine_BSEAVSW_Devel/7   7/25/03 8:13p vsilyaev
 * Removed declarations into the bsettop_types.h.
 *
 * Irvine_BSEAVSW_Devel/6   7/25/03 4:22p erickson
 * in the middle of rework, does not compile
 *
 * Irvine_BSEAVSW_Devel/5   7/9/03 5:19p erickson
 * rework for new bband_t model
 *
 * Irvine_BSEAVSW_Devel/4   7/8/03 3:19p erickson
 * added graphics api
 *
 * Irvine_BSEAVSW_Devel/3   6/20/03 5:56p vsilyaev
 * Added PVR.
 *
 * Irvine_BSEAVSW_Devel/2   6/16/03 4:42p erickson
 * api work
 *
 * Irvine_BSEAVSW_Devel/1   6/13/03 3:09p vsilyaev
 * SetTop reference API.
 *
 *
 ***************************************************************************/

#ifndef BSETTOP_H__
#define BSETTOP_H__

#include "bsettop_types.h"

#include "bsettop_compat.h"
#include "bsettop_config.h"
#include "bsettop_decode.h"
#include "bsettop_display.h"
#include "bsettop_encode.h"
#include "bsettop_transcode.h"
#include "bsettop_graphics.h"
#include "bsettop_message.h"
#include "bsettop_playpump.h"
#include "bsettop_recpump.h"
#include "bsettop_pvr.h"
#include "bsettop_tuner.h"
#include "bsettop_user_io.h"
#include "bsettop_pcm.h"
#include "bsettop_cipher.h"
#include "bsettop_smartcard.h"
#include "bsettop_vbi.h"
#include "bsettop_cablecard.h"
#include "bsettop_crypto.h"
#include "bsettop_mem.h"

/*=********************************
Universal include file for the Settop API.

In order to call any function from the Settop API from your application,
you should include this and only this file.
***********************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*
Summary:
	Use the BSETTOP_VERSION macro and ignore this type.
*/
typedef unsigned bsettop_version;

/*
Summary:
	Version macro which must be passed to bsettop_init().
*/
#define BSETTOP_VERSION ((bsettop_version)0x0080)

/*
Summary:
	Initializes the settop API.

Description:
	Required before any other call. If you don't call it, some key functions will fail;
	others will succeed but will result in undefined behavior.

	Use the BSETTOP_VERSION macro to pass the correct version. This verifies
	at run-time that the correct shared library is being used.
*/
bresult bsettop_init(bsettop_version version);

/*
Summary:
	Releases resources

Description:
	This function is used to release resources allocated during bsettop_init.
	User should call this function before exit from an application.
*/
void bsettop_uninit(void);

/**
Summary:
Describes the power management state of the Settop API.
Description:
Each enum may have separate dependencies. They are documented per enum.
**/
typedef struct bsettop_power_state {
    bool av_outputs; /* Set false to power down all audio/video outputs. This includes digital and analog. This can be done
    	while decodes are in progress. However, no change in decode (stopping, restart, etc.) is permitted while
    	in this mode. */
    /* TODO: ethernet, SATA core and HDD, decoders, etc. */
} bsettop_power_state;

/**
Summary:
Get the current power management state.
**/
void bsettop_get_power_state(bsettop_power_state *state);

/**
Summary:
Set the current power management state.
Description:
Use bsettop_get_power_state to get an initialized bsettop_power_state structure.
**/
bresult bsettop_set_power_state(const bsettop_power_state *state);

/**
Summary:
Max number of video dacs on a chip
**/
#define BSETTOP_MAX_VIDEO_DACS (7)

typedef enum bsettop_lipsync_mode
{
    bsettop_lipsync_mode_precision,     /* Enables basic + subframe audio adjustments based on video feedback (default) */
    bsettop_lipsync_mode_basic,         /* Enables tsm + Video and audio path delay matching */
    bsettop_lipsync_mode_tsm,           /* Enables timestamp management */
    bsettop_lipsync_mode_disabled       /* Disables all lipsync */
} bsettop_lipsync_mode;

/**
Summary:
Settings used to configure bsettop_init()
**/
typedef struct bsettop_init_settings
{
    bool open_frontend; /* if false, user can call bsettop_init_frontend after bsettop_init [default=true] */

    bsettop_lipsync_mode lipsync_mode;  /* If true, precision lipsync is enabled. [default=precision] */
    bool sync_path_swap;                /* If true, the video sync paths are swapped [default=false] */

    unsigned video_fifo_size;           /* Video FIFO size in bytes */
    unsigned audio_fifo_size;           /* Audio FIFO size in bytes */

    uint32_t video_dac_band_gap[BSETTOP_MAX_VIDEO_DACS];   /* Adjustment to the video TDAC and QDAC bandgap setting.
                                                              The default value is correct for most chipsets. However, there are
                                                              some production runs that require an adjustment for correct amplitude,
                                                              depends on the particular fab line that manufactured the chip. */
} bsettop_init_settings;

/**
Summary:
Get current initialization settings
**/
void bsettop_get_init_settings(
    bsettop_init_settings *settings /* [out] */
    );

/**
Summary:
Apply initialization settings before calling bsettop_init()

Description:
This function has no effect after bsettop_init() is called.
**/
int bsettop_set_init_settings(
    const bsettop_init_settings *settings
    );

/**
Summary:
Initialize the frontend after bsettop_init if bsettop_init_settings.open_frontend = false;
**/
int bsettop_init_frontend(void);

/**
Summary:
Uninitialize the frontend; 
**/
void bsettop_uninit_frontend(void);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_H__ */
