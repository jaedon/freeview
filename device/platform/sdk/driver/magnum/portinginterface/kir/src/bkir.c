/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkir.c $
 * $brcm_Revision: Hydra_Software_Devel/132 $
 * $brcm_Date: 4/29/13 4:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/kir/src/bkir.c $
 *
 * Hydra_Software_Devel/132   4/29/13 4:48p agin
 * SW7231-1184:  Unexpected result of NEXUS_IrInput_EnableDataFilter
 * function use.
 * 
 * Hydra_Software_Devel/125   10/8/12 12:41a agin
 * SWNOOS-576:  Fixed compile error for 7550.
 * 
 * Hydra_Software_Devel/124   10/5/12 1:29p agin
 * SWNOOS-576:  Add back PI support for 7550.
 * 
 * Hydra_Software_Devel/123   10/5/12 9:45a agin
 * SW7445-29:  Fixed compile error for 7445.
 * 
 * Hydra_Software_Devel/122   9/24/12 4:57p randyjew
 * SW7563-2: Add 7563 Support
 * 
 * Hydra_Software_Devel/121   9/22/12 5:36p agin
 * SW7445-29:  Fixed compile error for 7435.
 * 
 * Hydra_Software_Devel/120   9/22/12 5:10p agin
 * SW7445-29:  Fixed issue with BKIR_N_CHANNELS=2.
 * 
 * Hydra_Software_Devel/119   9/21/12 6:49p agin
 * SW7445-29:  add kir PI support for 7445.
 * 
 * Hydra_Software_Devel/118   4/28/12 3:33p chengs
 * SW7584-23: Add 7584 support.
 * 
 * Hydra_Software_Devel/117   4/26/12 4:56p bselva
 * SW7360-17: Add support for 7360
 * 
 * Hydra_Software_Devel/116   4/6/12 6:43p jkim
 * SW7425-2109: Fix IR remote anomalies by initializing the command
 * register to the default value when opening channel.
 * 
 * Hydra_Software_Devel/115   2/21/12 6:34p jkim
 * SW7425-2283: Add API to enable/disable filter1
 * 
 * Hydra_Software_Devel/114   11/21/11 6:20p mward
 * SW7435-7: Add 7435.
 * 
 * Hydra_Software_Devel/113   10/14/11 4:28p jkim
 * SW7231-370: Add simultaneout spport for 32/48 bit for NEC
 * 
 * Hydra_Software_Devel/112   9/20/11 2:11p jkim
 * SW7429-10: Add 7429 support
 * 
 * Hydra_Software_Devel/111   9/20/11 1:59p jkim
 * SWBLURAY-27433: Add Toshiba IR support
 * 
 * Hydra_Software_Devel/110   9/20/11 1:34p jkim
 * SWBLURAY-27433: Add Toshiba IR support
 * 
 * Hydra_Software_Devel/109   4/29/11 6:13p jkim
 * SW7405-5300: Add variable length RCMM support. It was tested with 32bit
 * RCMM and 24bit RCMM data.
 * 
 * Hydra_Software_Devel/108   2/8/11 3:18p jkim
 * SW7340-248: 1. Remove redundant code to clear interrupt status bit in
 * BKIR_ReadIsr(). It is already cleared in BKIR_P_HandleInterrupt_Isr()
 * . 2. Guard a call to BKIR_Read_Isr() in BKIR_Read() by calling
 * BKNI_EnterCriticalSection().
 * 
 * Hydra_Software_Devel/107   1/26/11 2:26p etrudeau
 * SWBLURAY-23692: add 7640 support
 * 
 * Hydra_Software_Devel/106   12/27/10 2:19p xhuang
 * SW7358-29: Add 7358/7552 support
 * 
 * Hydra_Software_Devel/105   12/7/10 6:13p jrubio
 * SW7344-9: add 7344/7346 support
 * 
 * Hydra_Software_Devel/104   12/2/10 10:31a katrep
 * SW7231-4:add support for 7231
 * 
 * Hydra_Software_Devel/103   11/2/10 5:04p hongtaoz
 * SW7425-9: added 7425 support;
 * 
 * Hydra_Software_Devel/102   10/13/10 2:53p jkim
 * SW7630-98: Add support for RCA remote
 * 
 * Hydra_Software_Devel/101   10/7/10 4:53p nickh
 * SW7422-74: Add 7422 support
 * 
 * Hydra_Software_Devel/100   7/23/10 10:45a jkim
 * SW7550-471: use BKIR_HAS_DATA_FILTER instead of BCHP_CHIP
 * 
 * Hydra_Software_Devel/99   6/25/10 11:28a jkim
 * SW7550-471:Add support for pattern matching and 48 bit data
 * 
 * Hydra_Software_Devel/98   3/3/10 5:39p jkim
 * HW7420-766: modified CIR parameters to use a new variable length IR
 * decoder from a leading edge to a leading edge.
 * 
 * Hydra_Software_Devel/97   12/13/09 6:41p rpereira
 * SW7550-41: Fixed compilation issues for 7550
 *
 * Hydra_Software_Devel/95   11/6/09 4:18p gmohile
 * SW7408-1 : Add 7408 support
 *
 * Hydra_Software_Devel/94   9/15/09 10:33a rpereira
 * SW7630-45: Adding support for 7630
 *
 * Hydra_Software_Devel/93   9/4/09 12:46p jkim
 * SW7405-340: fine tune r-step parameters based on the spec.
 *
 * Hydra_Software_Devel/92   9/2/09 2:51p jkim
 * SW7325-524: fixed a bug where CIR parameters were only partially
 * written to CIR register.
 *
 * Hydra_Software_Devel/91   9/1/09 9:21p rpereira
 * SW7550-30: Adding 7550 support
 *
 * Hydra_Software_Devel/90   8/24/09 1:49p jkim
 * SW7405-340: Add RC6 Mode 0 support
 *
 * Hydra_Software_Devel/89   8/20/09 4:23p mward
 * PR55545: Support 7125.
 *
 * Hydra_Software_Devel/88   8/10/09 4:41p jrubio
 * PR55232: add 7340/7342 support
 *
 * Hydra_Software_Devel/87   8/3/09 6:42p jkim
 * PR56525: check if preamble A or B is used.
 *
 * Hydra_Software_Devel/86   8/3/09 10:53a jkim
 * PR56525: Fix it per Cisco's request
 *
 * Hydra_Software_Devel/85   7/27/09 7:06p jkim
 * PR56525: add a function to set custom device type
 *
 * Hydra_Software_Devel/84   7/22/09 3:12p jkim
 * PR37963: Add functions to check preamble status
 *
 * Hydra_Software_Devel/83   7/14/09 6:09p jkim
 * PR56525: Add API to get default cir paramters so that it can be
 * modified and copied to cutom cir
 *
 * Hydra_Software_Devel/82   4/17/09 11:18a agin
 * PR54338: Moved BKIR_N_CHANNELS to bkir.h because other files need this.
 *
 * Hydra_Software_Devel/81   4/10/09 5:29p jkim
 * PR37963: modify rstep parameters according to the spec
 *
 * Hydra_Software_Devel/80   4/9/09 5:23p rpereira
 * PR52971: adding 7635 support
 *
 * Hydra_Software_Devel/79   4/9/09 4:27p jkim
 * PR53968: use DEBUG_MSG instead of DEBUG_WRN
 *
 * Hydra_Software_Devel/78   4/8/09 10:01a jkim
 * PR53968: update using the  changes made by Gorden Chen, which was
 * tested on Samsung HR24.
 *
 * Hydra_Software_Devel/77   4/6/09 1:59p jkim
 * PR38349: modify RCMM parameters basedon the latest spec
 *
 * Hydra_Software_Devel/76   3/12/09 6:01p jkim
 * PR37963: create a duplicate XMP-2 CIR parameters used for RCU without
 * supporting ACK. This way is needed for better performance since we do
 * not have to support XMP-2 data and ACK at the same time.
 *
 * Hydra_Software_Devel/75   3/6/09 1:09p jkim
 * PR52510: remove a global variable
 *
 * Hydra_Software_Devel/74   2/17/09 8:28p jkim
 * PR37963: modified xmp-2 parameters to support a new spec using a single
 * cir parameters
 *
 * Hydra_Software_Devel/73   2/9/09 8:45p jkim
 * PR37963: Add XIR parameter for XMP-2 ACK/NAK
 *
 * Hydra_Software_Devel/72   1/31/09 1:10a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/71   12/2/08 4:44p kaushikb
 * PR49867: Adding support for 7420
 *
 * Hydra_Software_Devel/71   12/2/08 4:42p kaushikb
 * PR49867: Adding support for 7420
 *
 * Hydra_Software_Devel/70   9/22/08 4:16p jkim
 * PR37963: add xmp-2 cir param
 *
 * Hydra_Software_Devel/69   8/21/08 9:00a fbasso
 * PR 44541: added support for 7601.
 *
 * Hydra_Software_Devel/68   7/28/08 1:42p jkim
 * PR45065: add R-step remote support
 *
 * Hydra_Software_Devel/67   6/27/08 1:47p brianlee
 * PR44259: Need to program CIR registers 23, 25, 26, 27 for all remote
 * types.
 *
 * Hydra_Software_Devel/66   4/4/08 2:21p farshidf
 * PR39191: add 3548/3556 support
 *
 * Hydra_Software_Devel/65   4/3/08 2:11p brianlee
 * PR23894: Initialize user callback pointer to NULL when opening and
 * closing a channel.
 *
 * Hydra_Software_Devel/64   4/1/08 4:19p brianlee
 * PR23894: Added functions to register user callback with KIR PI.
 *
 * Hydra_Software_Devel/63   3/27/08 1:09p jkim
 * PR37963: Add one byte xmp-2 support
 *
 * Hydra_Software_Devel/62   2/21/08 4:31p brianlee
 * PR18303: Fixed the bkir read function for 7325.
 *
 * Hydra_Software_Devel/61   1/4/08 12:49p erickson
 * PR38536: check for hChnCallback before destroying it
 *
 * Hydra_Software_Devel/60   11/28/07 10:16a farshidf
 * PR36888:Add 7335 support
 *
 * Hydra_Software_Devel/59   11/27/07 6:16p farshidf
 * PR36888: Add support for 7335
 *
 * Hydra_Software_Devel/58   10/14/07 3:53p jkim
 * PR14344: add 7325 support
 *
 * Hydra_Software_Devel/57   10/13/07 11:35a jkim
 * PR14344: Add 7325 support and modify RCMM parameters
 *
 * Hydra_Software_Devel/56   5/21/07 4:05p jkim
 * PR30841: Add 7405 support
 *
 * Hydra_Software_Devel/55   5/2/07 5:39p jkim
 * PR30225: use preamble B as repeat flag
 *
 * Hydra_Software_Devel/54   4/23/07 2:48p jkim
 * PR14344: Modify nec paramters per request by Mahesh Danke and add 7440
 * support when needed
 *
 * Hydra_Software_Devel/53   4/12/07 10:33a jkim
 * PR14344: Fix compiler warning
 *
 * Hydra_Software_Devel/52   4/9/07 4:51p jkim
 * PR28061: Add RCMM support
 *
 * Hydra_Software_Devel/51   2/28/07 11:01a jkim
 * PR14344: Remove unnecessary printf()
 *
 * Hydra_Software_Devel/50   2/16/07 11:35a jkim
 * PR14344: Added 7440 support
 *
 * Hydra_Software_Devel/49   2/2/07 11:22a jkim
 * PR27238: Modify to use the correct IRQ definition
 *
 * Hydra_Software_Devel/48   1/12/07 4:21p jkim
 * PR14344: Add 3563 support
 *
 * Hydra_Software_Devel/47   12/1/06 4:07p jkim
 * PR26238: Remove compiler warning
 *
 * Hydra_Software_Devel/46   11/28/06 11:40a jkim
 * PR25664: Added RC6 remote support. Simultaneous support for two remote
 * should be working but can not be tested using broadcom reference
 * board.
 *
 * Hydra_Software_Devel/45   11/9/06 11:19a jkim
 * PR14344: Added 7403 support
 *
 * Hydra_Software_Devel/44   9/26/06 11:12a agin
 * PR24562: Support 4th IR receiver for 7400.
 *
 * Hydra_Software_Devel/43   9/19/06 5:20p agin
 * PR24339: Resolve compiler warning for DEBUG=n builds for UPG modules.
 *
 * Hydra_Software_Devel/42   8/9/06 11:04a agin
 * PR23362: Add 3563 support.
 *
 * Hydra_Software_Devel/41   7/18/06 2:47p jkim
 * PR14344: fixed the problem related to legacy mode Echostar UHF where
 * unwanted interrupt is happening.
 *
 * Hydra_Software_Devel/40   6/15/06 5:14p mward
 * PR21681:  Add support for 7118 chip 97118.
 *
 * Hydra_Software_Devel/39   3/21/06 3:04p jkim
 * PR20326: Add support for 7438
 *
 * Hydra_Software_Devel/38   2/1/06 11:49a rjlewis
 * PR19044: endian issue when storing by longs.
 *
 * Hydra_Software_Devel/37   1/15/06 9:13a agin
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/36   12/22/05 10:44a agin
 * PR18787: Update I2C, KIR with C3 defines using new version method.
 *
 * Hydra_Software_Devel/35   11/14/05 1:33p jkim
 * PR16923: change the value of "min dead time after fault" to 30 per
 * Linda Lau's request
 *
 * Hydra_Software_Devel/34   10/21/05 2:09p jkim
 * PR14344:write 0x0000007F for UHF remote
 *
 * Hydra_Software_Devel/33   10/11/05 2:57p jkim
 * PR16923: Add legacy support for directtv uhf
 *
 * Hydra_Software_Devel/32   9/30/05 3:26p jkim
 * PR14344: Add UHF legacy support
 *
 * Hydra_Software_Devel/30   8/23/05 5:27p jkim
 * PR14344: adding 7401 support
 *
 * Hydra_Software_Devel/29   5/18/05 12:43a agin
 * PR14720: B2, C1, C2 compilation support.
 *
 * Hydra_Software_Devel/28   5/10/05 2:59p brianlee
 * PR15254: Fixed a warning.
 *
 * Hydra_Software_Devel/27   5/10/05 9:26a brianlee
 * PR15254: Increase tolerance on data bit periods for RF UEI remote.
 * This is needed to make LIST and down arrow keys work.
 *
 * Hydra_Software_Devel/26   5/6/05 3:43p brianlee
 * PR14284: Fixed problem where RF UEI mode does not turn off other
 * protocols correctly.
 *
 * Hydra_Software_Devel/24   4/7/05 10:49a agin
 * PR14698: unused parameters.
 *
 * Hydra_Software_Devel/23   4/4/05 3:14p agin
 * PR14558: Removed printf in the ISR.
 *
 * Hydra_Software_Devel/22   4/4/05 2:40p agin
 * PR14558:  Add pointer to array for BKIR_Read function.
 *
 * Hydra_Software_Devel/21   3/28/05 8:34p vinwang
 * PR14240: Fix Ir1_in not enable because auto select does not work for
 * 93560 board.
 *
 * Hydra_Software_Devel/20   3/21/05 10:28p vsilyaev
 * PR 14413:  Added BERR_TRACE's
 *
 * Hydra_Software_Devel/19   3/17/05 4:12p vsilyaev
 * PR 14413:  Added support for 3560.
 *
 * Hydra_Software_Devel/18   3/3/05 11:48a brianlee
 * PR14284: Fixed compile error.
 *
 * Hydra_Software_Devel/17   3/3/05 10:54a brianlee
 * PR14284: Disable SEJIN and TWIRP for RF UEI.
 *
 * Hydra_Software_Devel/16   2/28/05 2:27p brianlee
 * PR14284: Add support for UEI RF remote.
 *
 * Hydra_Software_Devel/15   12/10/04 11:52a brianlee
 * PR13515: Changed UEI remote spacing tolerance to improve response.
 *
 * Hydra_Software_Devel/14   6/1/04 11:13a brianlee
 * PR11291: Added code to support KIR3.
 *
 * Hydra_Software_Devel/13   5/26/04 8:45a dlwin
 * PR 11189: Merging to Main Branch
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/3   5/25/04 3:14p brianlee
 * PR11209: Update RC5 remote to handle both field values.
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   5/5/04 2:11p brianlee
 * PR10969: Make static structures constant.
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/28/04 2:33p brianlee
 * PR10857: Version for B0.
 *
 * Hydra_Software_Devel/12   3/26/04 4:32p brianlee
 * PR8971: Remove BDBG_ASSERT() for malloc failure.
 *
 * Hydra_Software_Devel/11   3/4/04 5:36p brianlee
 * PR9958: Added BKIR_KirDevice_eCirCustom type and the function
 * BKIR_SetCustomCir() to allow user to set custom KIR settings.
 *
 * Hydra_Software_Devel/10   12/29/03 3:59p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/9   11/4/03 6:52p brianlee
 * Get rid of enter/leave macros.
 *
 * Hydra_Software_Devel/8   10/17/03 10:42a brianlee
 * 1.) Added a function to check for repeat key.
 * 2.) Fixed CIR write function.
 *
 * Hydra_Software_Devel/7   10/2/03 10:40a brianlee
 * Fixed BKIR_Read so that it outputs interrupt device type, not IR device
 * type.
 *
 * Hydra_Software_Devel/6   9/24/03 11:58a brianlee
 * Changed the names of header files.
 *
 * Hydra_Software_Devel/5   9/23/03 10:37a brianlee
 * Added support for Sony variable length packet detection.
 *
 * Hydra_Software_Devel/4   9/19/03 1:45p brianlee
 * Fixed warnings from Midas build.
 *
 * Hydra_Software_Devel/3   9/16/03 6:31p brianlee
 * Writing to IRQ0_IRQEN register does not require coreoffset.
 *
 * Hydra_Software_Devel/2   9/16/03 10:41a brianlee
 * Fixed coreoffset calculation.
 *
 * Hydra_Software_Devel/1   9/15/03 10:29a brianlee
 * Initial version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkir.h"
#include "bkir_priv.h"
#include "bchp_kbd1.h"

#if BKIR_N_CHANNELS > 1
    #include "bchp_kbd2.h"
#if BKIR_N_CHANNELS > 2
    #include "bchp_kbd3.h"
#if BKIR_N_CHANNELS > 3
    #include "bchp_kbd4.h"
#endif
#endif
#endif

#include "bchp_irq0.h"
#include "bchp_int_id_irq0.h" 

#if (BCHP_CHIP==7231) || (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7358) || (BCHP_CHIP==7360) || (BCHP_CHIP==7422) || \
    (BCHP_CHIP==7425) || (BCHP_CHIP==7429) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445) || (BCHP_CHIP==7552) || (BCHP_CHIP==7563) || \
    (BCHP_CHIP==7584)
    #define BKIR_USES_AON
#endif

#ifdef BKIR_USES_AON
    #include "bchp_int_id_irq0_aon.h"
    #include "bchp_irq0_aon.h"
#endif

BDBG_MODULE(bkir);

#define DEV_MAGIC_ID            ((BERR_KIR_ID<<16) | 0xFACE)

#define BKIR_CHK_RETCODE( rc, func )        \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define MAX_KIR_CHANNELS            BKIR_N_CHANNELS

#define KBD_CMD_TWIRP_ENABLE        0x01
#define KBD_CMD_SEJIN_ENABLE        0x02
#define KBD_CMD_REMOTE_A_ENABLE     0x04
#define KBD_CMD_REMOTE_B_ENABLE     0x08
#define KBD_CMD_CIR_ENABLE          0x10

#define KBD_STATUS_DEVICE_MASK      0x1c
#define KBD_STATUS_DEVICE_SHIFTS    2

#ifndef RCMM_VARIABLE_LENGTH_SUPPORT
#define RCMM_VARIABLE_LENGTH_SUPPORT          0
#endif

/*******************************************************************************
*
*   Private Data
*
*******************************************************************************/
static const CIR_Param giParam = {
    270-1,          /* count divisor: divide by 270 for 10us period */
    { {900,0}, {450,0}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {900,0}, {225,0}, {0,0}, {0,0} }, /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[]                              */
    2,              /* number of entries in pb[] */
    1,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    1,              /* if true, pb[] matches a repeat sequence */
    50,             /* pulse tolerance */
    275,            /* T0 */
    225,            /* delta T */
    0,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle */
                    /* true => measure spacing between 2 consecutive edges */
    {50,3},         /* data symbol fix-width pulse period, */
                    /* tolerance = pulse tolerance (above) */
    {0, 0},         /* spacing tolerance value = not applicable, */
                    /* and select code = 12.5% */
    16-1,           /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    0,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    50-1,           /* time-out clock divisor: divide by 50 or .5 ms */
    200,            /* frame time-out = 100 ms */
    22,             /* edge time-out = 11 ms */
    6,              /* minimum dead-time after fault = 3 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};


static const CIR_Param sa_e2050Param = {
    100-1,          /*count divisor: divide by 100 for 3.70us period */
    { {910,0}, {890,0}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} }, /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    1,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    227,            /* pulse tolerance = 0.841 ms */
    450,            /* T0 = 1.667 ms */
    450,            /* delta T = 1.667 ms */
    0,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle */
                    /* true => measure spacing between 2 consecutive edges */
    {227,3},        /* data symbol fix-width pulse period = 0.841ms, */
                    /* tolerance = pulse tolerance (above) */
    {0, 0},         /* spacing tolerance value = not applicable, */
                    /* and select code = 12.5% */
    22-1,           /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    0,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    135-1,          /* time-out clock divisor: divide by 135 or .5ms */
    140,            /* frame time-out = 70 ms */
    9,              /* edge time-out = 4.5 ms */
    9,              /* minimum dead-time after fault = 4.5 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};


static const CIR_Param twirpParam = {
    90-1,           /* count divisor: divide by 90 for 3.33us period */
    { {84,1}, {0,0}, {0,0}, {0,0} },    /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    1,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    84,             /* pulse tolerance = 0.280 ms */
    300,            /* T0 = 1.0000 ms */
    40+(3<<10),     /* delta T = 0.1358 ms (40.75*t) */
    1,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle */
                    /* true => measure spacing between 2 consecutive edges */
    {84,1},         /* data symbol fix-width pulse period = 0.280, */
                    /* tolerance = 25% */
    {20, 3},        /* spacing tolerance value = .5 DeltaT, */
                    /* and select code = value */
    8-1,            /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    4-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    1,              /* variable length data */
    150-1,          /* time-out clock divisor: divide by 150 or .5ms */
    52,             /* frame time-out = 26 ms */
    8,              /* edge time-out = 4 ms */
    8,              /* minimum dead-time after fault = 4 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    972,                /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};


static const CIR_Param sonyParam = {
    72-1,           /* count divisor: divide by 72 for 2.66us period */
    { {900,0}, {0,0}, {0,0}, {0,0} },   /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} }, /* pb[], preamble B pulse sequence */
    1,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    225,            /* pulse tolerance = 0.500 ms */
    450,            /* T0 = 1.200 ms */
    225,            /* delta T = 0.600 ms */
    0,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle */
                    /* true => measure spacing between 2 consecutive edges */
    {225,3},        /* data symbol fix-width pulse period = 0.600ms, */
                    /* tolerance = pulse tolerance (above) */
    {0, 0},         /* spacing tolerance value = not applicable, */
                    /* and select code = 12.5% */
    12-1,           /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    0,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    1,              /* variable length data */
    188-1,          /* time-out clock divisor: divide by 188 or .5ms */
    90,             /* frame time-out = 45 ms */
    7,              /* edge time-out = 3.5 ms */
    3,              /* minimum dead-time after fault = 1.5 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0x308,          /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};


static const CIR_Param recs80Param = {
    226-1,          /* count divisor: divide by 226 for 8.370 us period */
    { {17,2}, {907,0}, {0,0}, {0,0} },      /* pa[], preamble A pulse sequence */
    { {17,2}, {453,0}, {17,2}, {453,0} },   /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[] */
    4,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    17,             /* pulse tolerance = 0.1432 ms */
    605,            /* T0 = 5.060 ms */
    302,            /* delta T = 2.5279 ms */
    0,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle */
                    /* true => measure spacing between 2 consecutive edges */
    {17,3},         /* data symbol fix-width pulse period = 0.142ms, */
                    /* tolerance = pulse tolerance (above) */
    {0, 0},         /* spacing tolerance value = not applicable, */
                    /* and select code = 12.5% */
    10-1,           /* no. of symbols for sequence with preamble A */
    11-1,           /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    0,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    60-1,           /* time-out clock divisor: divide by 60 or .5ms */
    190,            /* frame time-out = 95 ms */
    17,             /* edge time-out = 8.5 ms */
    17,             /* minimum dead-time after fault = 8.5 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};


static const CIR_Param rc5Param = {
    54-1,           /* count divisor: divide by 54 for 2.000 us period */
    { {445,1}, {889,1}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {889,1}, {0,0}, {0,0}, {0,0} },       /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[] */
    1,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    445,            /* pulse tolerance = value not used */
    889,            /* bit Period = 1.778 ms (PS T0) */
    0,              /* not used for bi-phase (PS delta T) */
    0,              /*  - " - (symbol pulse position) */
    0,              /*  - " - (measure spacing for complete cycle) */
    {0, 0},         /*  - " - (data symbol fix-width pulse period) */
    {0, 0},         /* bit period tolerance value = not applicable, */
                    /* and select code = 12.5% */
    12-1,           /* no. of symbols for sequence with preamble A */
    12-1,               /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    1,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    250-1,          /* time-out clock divisor: divide by 250 or .5ms */
    56,             /* frame time-out = 28 ms */
    5,              /* edge time-out = 2.5 ms */
    3,              /* minimum dead-time after fault = 1.5 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const CIR_Param ueiParam = {
    270-1,          /* count divisor */
    { {600,0}, {120,3}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {300,0}, {120,3}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[] */
    2,              /* number of entries in pb[] */
    1,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    26,             /* pulse tolerance = value not used */
    60,             /* T0 */
    60,             /* not used for bi-phase (PS delta T) */
    0,              /*  - " - (symbol pulse position) */
    1,              /*  - " - (measure spacing for complete cycle) */
    {60, 1},        /*  - " - (data symbol fix-width pulse period) */
    {12, 3},            /* bit period tolerance value = not applicable, */
                    /* and select code */
    8-1,            /* no. of symbols for sequence with preamble A */
    8-1,            /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    1,              /* two symbols per cycle */
    1,              /* check stop symbol */
    0,              /* variable length data */
    50-1,           /* time-out clock divisor */
    74,             /* frame time-out */
    14,             /* edge time-out */
    16,             /* minimum dead-time after fault */
    {900, 0},       /* stop symbol pulse or cycle period */
    1100,           /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const CIR_Param RfueiParam = {
    270-1,          /* count divisor */
    { {600,1}, {120,3}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {300,1}, {120,3}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[] */
    2,              /* number of entries in pb[] */
    1,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    26,             /* pulse tolerance = value not used */
    60,             /* T0 */
    60,             /* delta T */
    0,              /* not used: symbol pulse position */
    1,              /* not used: measure spacing for complete cycle */
    {60, 0},        /* not used: data symbol fix-width pulse period */
    {0, 1},         /* bit period tolerance value = not applicable, */
                    /* and select code */
    20-1,           /* no. of symbols for sequence with preamble A */
    20-1,           /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    1,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    10-1,           /* time-out clock divisor */
    940,            /* frame time-out */
    70,             /* edge time-out */
    16,             /* minimum dead-time after fault */
    {60, 0},        /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};


static const CIR_Param echoDishRemoteParam = {
/* Symbol time spec. includes 400us ON pulse. */
    270-1,                      /* count divisor: divide by 135 for 10us period */
    { {40,3}, {658,0}, {0,0}, {0,0} },  /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    2,                          /* number of entries in pa[] */
    0,                          /* number of entries in pb[] */
    0,                          /* measure preamble pulse: */
                                /*   0 => even counts specifies cycle period */
                                /*   1 => even counts specifies off pulse period */
    0,                          /* if true, pb[] matches a repeat sequence */
    40,                         /* pulse tolerance = 400us */
    325,                        /* T0 = 3.25ms */
    907 | 1<<10,                /* DeltaT = -1.1675ms */
    0,                          /* false => fix symbol pulse between edges 0 & 1 */
                                /* true => fix symbol pulse between edges 1 & 2 */
    0,                          /* false => measure spacing for complete cycle */
                            /* true => measure spacing between 2 consecutive edges */
                                /*   (for stop/IWG symbol) */
    {40,3},                     /* data/stop symbol fix-width pulse period, */
                                /*   tolerance = pulse tolerace (above) */
    {0, 0},                     /* spacing tolerance value = not applicable, */
                                /*   and spacing tolerance = 12.5% */
    16-1,                       /* no. of symbols for sequence with preamble A */
    0,                          /* no. of symbols for sequence with preamble B */
    1-1,                        /* no. of data bits per symbol */
    0,                          /* most/!least significant symbol received first */
    0,                          /* left/!right adjust received data */
    0,                          /* bi-phase/!pulse-spacing coded */
    0,                          /* two sub-symbols per cycle */
    1,                          /* have inter-word gap or stop symbol */
    1,                          /* variable length data */
    50-1,                       /* time-out clock divisor: divide by 50 or .5 ms */
    148,                        /* frame time-out = 74 ms */
    16,                         /* edge time-out = 8.0 ms */
    16,                         /* minimum dead-time after fault = 8.0 ms */
    {658,0},                    /* stop/IWG off/cycle period = 6.5ms, 12.5% */
    744,                         /* data symbol time-out */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const CIR_Param echostarUHFParam =
{   /* Manchester */
/* Symbol time spec. includes 400us ON pulse. */
    108-1,                      /*  count unit @ 27MHz = 4us */
    { {500,0}, {250,0}, {500,0}, {250,0}},  /* pa[], preamble A pulse sequence */
    { {500,0}, {500,0}, {250,0}, {250,0}},     /* pb[], preamble B pulse sequence */
    3,                          /* number of entries in pa[] */
    3,                          /* number of entries in pb[] */
    1,                          /* measure preamble pulse: */
    /*   0 => even counts specifies cycle period */
    /*   1 => even counts specifies off pulse period */
    0,                          /* if true, pb[] matches a repeat sequence */
    0,                          /* pulse tolerance = 400us */
    250,                        /* T0 = 1ms */
    0,                          /* DeltaT = 0ms */
    0,                          /* false => fix symbol pulse between edges 0 & 1 */
    /* true => fix symbol pulse between edges 1 & 2 */
    0,                          /* false => measure spacing for complete cycle */
    /* true => measure spacing between 2 consecutive edges */
    /*   (for stop/IWG symbol) */
    {0,0},                      /* data/stop symbol fix-width pulse period, */
    /*   tolerance = pulse tolerace (above) */
    {0,0},                      /* spacing tolerance value = not applicable, */
    /*   and spacing tolerance = 12.5% */
    26-1,                       /* no. of symbols for sequence with preamble A */
    5-1,                        /* no. of symbols for sequence with preamble B */
    1-1,                        /* no. of data bits per symbol */
    1,                          /* most/!least significant symbol received first */
    0,                          /* left/!right adjust received data */
    1,                          /* bi-phase/!pulse-spacing coded */
    0,                          /* two sub-symbols per cycle */
    0,                          /* have inter-word gap or stop symbol */
    0,                          /* variable length data */
    50-1,                       /* time-out unit @ 27MHz = 200us */
    253,                        /* frame time-out = 50.5 ms */
    25,                         /* edge time-out = 5.0 ms */
    2,                          /* minimum dead-time after fault = 3.0 ms */
    {0,0},                      /* stop/IWG off/cycle period = 6.5ms, 12.5% */
    0,                          /* data symbol time-out */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const CIR_Param necParam = {
    270-1,          /* count divisor */
    { {900,0}, {450,0}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {900,0}, {225,0}, {0,0}, {0,0} }, /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[] */
    2,              /* number of entries in pb[] */
    1,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    1,              /* if true, pb[] matches a repeat sequence */
    50,             /* pulse tolerance */
    113,            /* bit Period = 1.778 ms (PS T0) */
    113,            /* delta T */
    0,              /*  - " - (symbol pulse position) */
    0,              /*  - " - (measure spacing for complete cycle) */
    {50, 3},        /*  - " - (data symbol fix-width pulse period) */
    {0, 0},         /* bit period tolerance value = not applicable, */
                    /* and select code = 12.5% */
    48-1,           /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    0,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    1,              /* variable length data */
    50-1,           /* time-out clock divisor */
    255,            /* frame time-out */
    22,             /* edge time-out */
    6,              /* minimum dead-time after fault = 1.5 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    256,              /* data symbol timeout */
    280,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const CIR_Param giSatParam = {
    270-1,          /* count divisor: divide by 270 for 10us period */
    { {500,0}, {200,0}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    1,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    50,             /* pulse tolerance */
    200,            /* T0 */
    200,            /* delta T */
    0,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle */
                    /* true => measure spacing between 2 consecutive edges */
    {100,3},        /* data symbol fix-width pulse period, */
                    /* tolerance = pulse tolerance (above) */
    {0, 0},         /* spacing tolerance value = not applicable, */
                    /* and select code = 12.5% */
    12-1,           /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    0,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    50-1,           /* time-out clock divisor: divide by 50 or .5 ms */
    200,            /* frame time-out = 100 ms */
    22,             /* edge time-out = 11 ms */
    6,              /* minimum dead-time after fault = 3 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const CIR_Param directvUHFParam = {
    270-1,            /* count divisor */
    { {600,0}, {120,3}, {0,0}, {0,0} },    /* pa[], preamble A pulse
sequence */
    { {300,0}, {120,3}, {0,0}, {0,0} },        /* pb[], preamble B pulse
sequence */
    2,                /* number of entries in pa[] */
    2,                /* number of entries in pb[] */
    1,                /* measure preamble pulse: */
                    /*    0 => even counts specifies cycle period */
                    /*    1 => even counts specifies off pulse period */
    0,                /* if true, pb[] matches a repeat sequence */
    26,                /* pulse tolerance = value not used */
    60,                /* T0 */
    60,                /* not used for bi-phase (PS delta T) */
    0,                /*  - " - (symbol pulse position) */
    0,                /*  - " - (measure spacing for complete cycle) */
    {0, 0},        /*  - " - (data symbol fix-width pulse period) */
    {0, 0},            /* bit period tolerance value = not applicable, */
                    /* and select code */
    20-1,            /* no. of symbols for sequence with preamble A */
    20-1,            /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    1,                /* most/!least significant symbol received first */
    0,                /* left/!right adjust received data */
    0,                /* bi-phase/!pulse-spacing coded */
    1,                /* two symbols per cycle */
    0,                /* check stop symbol */
    0,                /* variable length data */
    5-1,            /* time-out clock divisor */
    1880,                /* frame time-out */
    140,                /* edge time-out */
    30,                /* minimum dead-time after fault */
    {600, 0},        /* stop symbol pulse or cycle period */
    0,            /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static CIR_Param rC6Mode0Param = {
/*
 * CIR configuration parameters for RC6 Mode 0
 *
 */
    125-1,      /* count divisor: count period = T/96 @ 27MHz, T=444.44.us */
    { {576,1}, {192,1}, {96,1}, {0,0} },
            /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} },
            /* pb[], preamble B pulse sequence */
    3,      /* number of entries in pa[] */
    0,      /* number of entries in pb[] */
    1,      /* preamble pulse type: */
            /*   0 => even counts specifies cycle period */
            /*   1 => even counts specifies off pulse period */
    0,      /* if true, pb[] matches a repeat sequence */
    0,      /* not used: pulse tolerance */
    192,        /* T0 = 888.88us (2T) */
    0,      /* not used: DeltaT */
    0,      /* not used */
    0,      /* not used */
    {0,0},      /* not used: stop symbol */
    {0,1},      /* bit period tolerance = ~12.5% */
    0,      /* not used: no. of symbols for sequence with preamble A */
    0,      /* not used: no. of symbols for sequence with preamble B */
    1-1,        /* no. of data bits per symbol = 1 */
    1,      /* most/!least significant symbol received first */
    0,      /* left/!right adjust received data */
    1,      /* bi-phase/!pulse-spacing coded */
    0,      /* two sub-symbols per cycle */
    0,      /* have inter-word gap or stop symbol */
    1,      /* variable length data */
    96-1,       /* time-out clock divisor: divide by 96 for 0.444ms period */
    100,        /* frame time-out = 100T */
    8,      /* edge time-out = 8T */
    8,      /* minimum dead-time after fault = 8T */
    {0,0},      /* not used: stop/IWG off/cycle period */
    672,        /* data symbol time-out = 3.11ms = 7T */
    0,      /* not used: repeat timer timeout */
    0,      /* stop parameter unit selector: */
            /*   0: stop has count units */
            /*   1: stop has timout units */
    0,      /* data symbol timer clock tick dataSymTimeout units selector: */
            /*   0: dataSymTimeout has count units */
            /*   1: dataSymTimeout has timout units */
    0,      /* not used for biphase: ignore data symbol timer expiration */
            /*      while waiting for Edge 1: 0:false, 1:true */
    0,      /* enable data symbol time-out expiration flag to lflag */
            /*      status register: 0:false, 1:true */
    0,      /* enable havePreambleAftStop parameter for */
            /*      non-biphase decoding: 0:false, 1:true */
    0,      /* have preamble after stop symbol: 0:false, 1:true */
    1,      /* restrictive decoding enabled: 0:false, 1:true */
    1,      /* RC6 encoded: 0:false, 1:true. Requires biphaseCoded=1 */
    1,      /* don't validate RC mode bits: 0:false, 1:true */
    0,      /* RC6 mode bits (3 bits): 0 for Mode 0, 6 for Mode 6 */
    0,      /* Reserved for future use */
    0,      /* Reserved for future use */
    1,      /* don't validate control field bits: 0:false, 1:true */
    0,      /* RC6M0 control field (8 bits) or RC6M6A customer code bits (16 bits) */
    8-1,        /* number of RC6M0 control field bits: 0..15 for 1..16 bits */
    1       /* RC6 mode bits and control field pass-through control: */
            /*   0: Exclude mode bits and control field from */
            /*      received data. The nccb field determines */
            /*      the size of the control field. */
            /*   1: Exclude mode bits from the received data, but, */
            /*      include control field. */
            /*   2: Not allowed. */
            /*   3: Include both mode bits and customer code in */
            /*      the received data. */
};


static const CIR_Param rC6Mode6AParam = {
/*
 * CIR configuration parameters for RC6 Mode 6A
 *
 */
    125-1,      /* count divisor: count period = T/96 @ 27MHz, T=444.44.us */
    { {576,1}, {192,1}, {96,1}, {0,0} },
                /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} },
                /* pb[], preamble B pulse sequence */
    3,          /* number of entries in pa[] */
    0,          /* number of entries in pb[] */
    1,          /* preamble pulse type: */
                /*   0 => even counts specifies cycle period */
                /*   1 => even counts specifies off pulse period */
    0,          /* if true, pb[] matches a repeat sequence */
    0,          /* not used: pulse tolerance */
    192,        /* T0 = 888.88us (2T) */
    0,          /* not used: DeltaT */
    0,          /* not used */
    0,          /* not used */
    {0,0},      /* not used: stop symbol */
    {0,1},      /* bit period tolerance = ~12.5% */
    0,          /* not used: no. of symbols for sequence with preamble A */
    0,          /* not used: no. of symbols for sequence with preamble B */
    1-1,        /* no. of data bits per symbol = 1 */
    1,          /* most/!least significant symbol received first */
    0,          /* left/!right adjust received data */
    1,          /* bi-phase/!pulse-spacing coded */
    0,          /* two sub-symbols per cycle */
    0,          /* have inter-word gap or stop symbol */
    1,          /* variable length data */
    96-1,       /* time-out clock divisor: divide by 96 for 0.444ms period */
    100,        /* frame time-out = 100T */
    8,          /* edge time-out = 8T */
    8,          /* minimum dead-time after fault = 8T */
    {0,0},      /* not used: stop/IWG off/cycle period */
    672,        /* data symbol time-out = 3.11ms = 7T */
    0,          /* not used: repeat timer timeout */
    0,          /* stop parameter unit selector: */
                /*   0: stop has count units */
                /*   1: stop has timout units */
    0,          /* data symbol timer clock tick dataSymTimeout units selector: */
                /*   0: dataSymTimeout has count units */
                /*   1: dataSymTimeout has timout units */
    0,          /* not used for biphase: ignore data symbol timer expiration */
                /*      while waiting for Edge 1: 0:false, 1:true */
    0,          /* enable data symbol time-out expiration flag to lflag */
                /*      status register: 0:false, 1:true */
    0,          /* enable havePreambleAftStop parameter for */
                /*      non-biphase decoding: 0:false, 1:true */
    0,          /* have preamble after stop symbol: 0:false, 1:true */
    1,          /* restrictive decoding enabled: 0:false, 1:true */
    1,          /* RC6 encoded: 0:false, 1:true. Requires biphaseCoded=1 */
    1,          /* don't validate RC mode bits: 0:false, 1:true */
    6,          /* RC6 mode bits (3 bits): 6 for Mode 6A */
    0,          /* Reserved: don't validate RC6 trailer: 0:false, 1:true */
    0,          /* Reserved: RC6 trailer (1 bit): 0 for Mode 6A */
    1,          /* don't validate customer code bits: 0:false, 1:true */
    0,          /* RC6 customer code bits (16 bits) */
    0,          /* number of RC6 customer code bits: 0..15 for 1..16 bits */
    3           /* RC6 mode bits and customer code pass-through control: */
                /*   0: Exclude mode bits and customer code from */
                /*      received data. The nccb field determines */
                /*      the size of the customer code. */
                /*   1: Exclude mode bits from the received data, but, */
                /*      include customer code. */
                /*   2: Not allowed. */
                /*   3: Include both mode bits and customer code in */
                /*      the received data. */
};

#if RCMM_VARIABLE_LENGTH_SUPPORT != 1
static const CIR_Param s_RCMMParam = {
/*
 * CIR configuration parameters for RCMM
 *
 * T0 = 444.44us
 * deltaT = 166.67us
 * Data 3 symbol OFF pulse width = 944.44us - 166.67us = 777.77us
 * Maximum pulse width = max(416.66, 277.77, 166.66, 777.77)
 *                     = 777.77us
 * Maximum frame duration = 416.66us + 277.77us + 166.66us
 *                          + 16 * 944.44us
 *                        = 15972.22us
 */
    30-1,       /* count divisor: divide by for 1.111us period */
    { {411,2}, {625,3}, {0,0}, {0,0} }, /* {411,2}->457us ONTIME+/- 50%, {625,3}->695us(ON+OFF) and use pulse tolerance of 54 */
                /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} },
                /* pb[], preamble B pulse sequence */
    2,          /* number of entries in pa[] */
    0,          /* number of entries in pb[] */
    0,          /* measure preamble pulse: */
                /*   0 => even counts specifies cycle period, meaning 625 in preamble denote ON+OFF */
                /*   1 => even counts specifies off pulse period */
    0,          /* if true, pb[] matches a repeat sequence */
    54,         /* pulse tolerance = 58us */
    400,        /* T0 = 444.44us */
    150,        /* DeltaT = 166.67ms */
    0,          /* false => fix symbol pulse between edges 0 & 1 */
                /* true => fix symbol pulse between edges 1 & 2 */
    0,          /* false => measure spacing for complete cycle */
                /* true => measure spacing between 2
                                        consecutive edges */
    {150,2},    /* data/stop symbol fix-width pulse period = 166.6us, tolerance = 50% */
    {54, 3},    /* spacing tolerance value = 58us, !!! */
    16-1,       /* no. of symbols for sequence with preamble A */
    0,          /* no. of symbols for sequence with preamble B */
    2-1,        /* no. of data bits per symbol */
    1,          /* most/!least significant symbol received first */
    0,          /* left/!right adjust received data */
    0,          /* bi-phase/!pulse-spacing coded */
    0,          /* two sub-symbols per cycle */
    0,          /* have inter-word gap or stop symbol */
    0,          /* variable length data */
    50-1,       /* time-out clock divisor: divide by 50
                                        or 55.55us */
    328,        /* frame time-out = 18,222us */
    18,         /* edge time-out = 1000us */
    18,         /* minimum dead-time after fault = 1000us */
    {0,0},      /* not used: stop/IWG off/cycle period */
    0,            /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};
#else
static const CIR_Param s_RCMMParam = {
/*
 * CIR configuration parameters for RCMM
 *
 * T0 = 444.44us
 * deltaT = 166.67us
 * Data 3 symbol OFF pulse width = 944.44us - 166.67us = 777.77us
 * Maximum pulse width = max(416.66, 277.77, 166.66, 777.77)
 *                     = 777.77us
 * Maximum frame duration = 416.66us + 277.77us + 166.66us
 *                          + 16 * 944.44us
 *                        = 15972.22us
 */
    30-1,       /* count divisor: divide by for 1.111us period */
    { {411,2}, {625,3}, {0,0}, {0,0} }, /* {411,2}->457us ONTIME+/- 50%, {625,3}->695us(ON+OFF) and use pulse tolerance of 54 */
                /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} },
                /* pb[], preamble B pulse sequence */
    2,          /* number of entries in pa[] */
    0,          /* number of entries in pb[] */
    0,          /* measure preamble pulse: */
                /*   0 => even counts specifies cycle period, meaning 625 in preamble denote ON+OFF */
                /*   1 => even counts specifies off pulse period */
    0,          /* if true, pb[] matches a repeat sequence */
    54,         /* pulse tolerance = 58us */
    400,        /* T0 = 444.44us */
    150,        /* DeltaT = 166.67ms */
    0,          /* false => fix symbol pulse between edges 0 & 1 */
                /* true => fix symbol pulse between edges 1 & 2 */
    0,          /* false => measure spacing for complete cycle */
                /* true => measure spacing between 2
                                        consecutive edges */
    {150,2},    /* data/stop symbol fix-width pulse period = 166.6us, tolerance = 50% */
    {54, 3},    /* spacing tolerance value = 58us, !!! */
    16-1,       /* ot used with variable length data, no. of symbols for sequence with preamble A */
    0,          /* no. of symbols for sequence with preamble B */
    2-1,        /* no. of data bits per symbol */
    1,          /* most/!least significant symbol received first */
    0,          /* left/!right adjust received data */
    0,          /* bi-phase/!pulse-spacing coded */
    0,          /* two sub-symbols per cycle */
    0,          /* have inter-word gap or stop symbol */
    1,          /* variable length data */
    50-1,       /* time-out clock divisor: divide by 50
                                        or 55.55us */
    328,        /* frame time-out = 18,222us */
    18,         /* edge time-out = 1000us */
    18,         /* minimum dead-time after fault = 1000us */
    {0,0},      /* not used: stop/IWG off/cycle period */
    906,            /* data symbol timeout: require > max Data 3 period = 850 + 54 count units (= 944.4us+60us) = 904 units */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};
#endif

static const CIR_Param rStepParam = {
    54-1,           /* count divisor: divide by 54 for 2.000 us period */
    { {165,3}, {0,0}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    1,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    67,             /* pulse tolerance = 67*2=134us; pa ul = 330+134= 464; pall = 330-134=196 */
    325,            /* bit Period = 325*2=650 us (PS T0) */
    0,              /* not used for bi-phase (PS delta T) */
    0,              /*  - " - (symbol pulse position) */
    0,              /*  - " - (measure spacing for complete cycle) */
    {0, 0},         /*  - " - (data symbol fix-width pulse period) */
    {67, 3},        /* bit period tolerance value = 53*2=106us; ul = 650+134=784; ll=650-134=516*/
    17-1,           /* no. of symbols for sequence with preamble A */
    17-1,               /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    1,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    250-1,          /* time-out clock divisor: divide by 250 or .5ms */
    24+1,           /* frame time-out = 12 ms */
    3+1,                /* edge time-out = 1.5 ms */
    3,              /* minimum dead-time after fault = 1.5 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,              /* validate a short pulse */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

#if defined (HUMAX_PLATFORM_BASE)
#define XMP2_NO_ACK
#endif
#ifdef XMP2_NO_ACK
static const CIR_Param xmp2Param = {
    90-1,           /* count divisor: 90 divide by 27MHz for 3.333us period */
    { {70,2}, {0,0}, {0,0}, {0,0} },    /* pa[], 67*3.33=223us, 1=+/125% 2= +/- 50%, 3=use purse tolerance*/
    { {0,0}, {0,0}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    0,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    35,             /* pulse tolerance = 29*3.33 = 166.55us. Only used if tolerance code is 3 */
    291,            /* T0 = 970us (969.98+0*136.71) */
    41,             /* delta T = 136.71us 41*3.333=136.653*/
    0,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle */
                    /* true => measure spacing between 2 consecutive edges */
    {70,2},         /* data symbol fix-width pulse period = 0.280, */
                    /* tolerance 2 = 50% */
    {21, 3},        /* spacing tolerance value = .5 DeltaT, */
                    /* and select code = value */
    8-1,            /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    4-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    150-1,          /* time-out clock divisor: 3.333us*150 = .5ms */
    60,             /* frame time-out = 60 ms */
    8,              /* edge time-out = 4 ms */
    8,              /* minimum dead-time after fault = 4 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,          /* data symbol timeout used with variable length data: 937*3.333us=3123us. After this time, we assume all data are captured */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};
#else
#ifdef XMP2_NEW_LE_TO_LE
/* This is the new XMP2 decoder.
 * This can decode a variable length IR input from leading edge to leading edge.
 * When used, this should decode XMXP-2 data and ack/nak IR input more reliably
 * than using falling edge since there are more jitter on the falling edge of IR input
 */
static const CIR_Param xmp2Param = {
    90-1,           /* count divisor: 90 divide by 27MHz for 3.333us period */
    { {0,0}, {0,0}, {0,0}, {0,0} }, /* pa[], 67*3.33=223us, 1=+/125% 2= +/- 50%, 3=use pulse tolerance*/
    { {0,0}, {0,0}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    0,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    74,             /* pulse tolerance = 74*3.33 = 245us. Only used if tolerance code is 3 */
    291,            /* one cycle of data 0: 970us/3.33=291 */
    41,             /* delta T = 136.71us 41*3.333=136.653*/
    0,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle*/
                    /* true => measure spacing between 2 consecutive edges */
    {116,3},        /* data symbol fix-width pulse period = 116*3.33=385us */
                    /* hilim = 500us+107us+23us=630us lolim = 214us-54us-20us=140 */
                    /* midpoint = (hilim+lolim)/2=385us. 385us/3.33us=116*/
                    /* tolerance 3 = use T0 tolerance value = (385-140)/3.33=74 */
    {21, 3},        /* spacing tolerance value = .5 DeltaT, */
                    /* and select code = value */
    0,              /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    4-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    1,              /* variable length data */
    150-1,          /* time-out clock divisor: 3.333us*150 = .5ms */
    60,             /* frame time-out = 60 ms */
    8,              /* edge time-out = 4 ms */
    8,              /* minimum dead-time after fault = 4 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    1090,           /* data symbol timeout used with variable length data: */
                    /* 1090*3.333us=3629us. After this time, we assume all data are captured */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};
#else
static const CIR_Param xmp2Param = {
    90-1,           /* count divisor: 90 divide by 27MHz for 3.333us period */
    { {70,2}, {0,0}, {0,0}, {0,0} },    /* pa[], 67*3.33=223us, 1=+/125% 2= +/- 50%, 3=use pulse tolerance*/
    { {0,0}, {0,0}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    1,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    74,             /* pulse tolerance = 74*3.33 = 245us. Only used if tolerance code is 3 */
    205,            /* 205*3.33us=683us: T0 = 970us-214us=756us. Reduced it to make it work when on pulse is bigger and off pulse is smaller*/
    41,             /* delta T = 136.71us 41*3.333=136.653*/
    1,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    1,              /* false => measure spacing for complete cycle*/
                    /* true => measure spacing between 2 consecutive edges */
    {116,3},        /* data symbol fix-width pulse period = 397us */
                    /* hilim = 500us+107us+23us=630us lolim = 214us-54us-20us=140 */
                    /* midpoint = (hilim+lolim)/2=385us. 385us/3.33us=116*/
                    /* tolerance 2 = 50% 3 = use T0 tolerance value = 385-140/3.33=74 */
    {21, 3},        /* spacing tolerance value = .5 DeltaT, */
                    /* and select code = value */
    8-1,            /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    4-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    1,              /* variable length data */
    150-1,          /* time-out clock divisor: 3.333us*150 = .5ms */
    60,             /* frame time-out = 60 ms */
    8,              /* edge time-out = 4 ms */
    8,              /* minimum dead-time after fault = 4 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    1090,           /* data symbol timeout used with variable length data: */
                    /* 1090*3.333us=3629us. After this time, we assume all data are captured */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};
#endif /* #ifdef XMP2_NEW_LE_TO_LE*/


#endif /* ifdef XMP2_NO_ACK */

static const CIR_Param xmp2AckParam = {
    90-1,           /* count divisor: 90 divide by 27MHz for 3.333us period */
    { {70,2}, {0,0}, {0,0}, {0,0} },    /* pa[], 67*3.33=223us, 1=+/125% 2= +/- 50%, 3=use purse tolerance*/
    { {0,0}, {0,0}, {0,0}, {0,0} },     /* pb[], preamble B pulse sequence */
    0,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    0,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    35,             /* pulse tolerance = 29*3.33 = 166.55us. Only used if tolerance code is 3 */
    291,            /* T0 = 970us (969.98+0*136.71) */
    41,             /* delta T = 136.71us 41*3.333=136.653*/
    0,              /* false => fix symbol pulse between edges 0 & 1 */
                    /* true => fix symbol pulse between edges 1 & 2 */
    0,              /* false => measure spacing for complete cycle */
                    /* true => measure spacing between 2 consecutive edges */
    {70,2},         /* data symbol fix-width pulse period = 0.280, */
                    /* tolerance 2 = 50% */
    {21, 3},        /* spacing tolerance value = .5 DeltaT, */
                    /* and select code = value */
    2-1,            /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    4-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    150-1,          /* time-out clock divisor: 3.333us*150 = .5ms */
    60,             /* frame time-out = 60 ms */
    8,              /* edge time-out = 4 ms */
    10,             /* minimum dead-time after fault = 5 ms. Make sure only 1 interrupt is received when 4 byte packet is received */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout used with variable length data: 972*3.333us=3239us. After this time, we assume all data are captured */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const CIR_Param rcaParam = {
    270-1,          /* count divisor 270/27Mhz = 10us */
    { {400,0}, {400,0}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    { {0,0}, {0,0}, {0,0}, {0,0} }, /* pb[], preamble B pulse sequence */
    2,              /* number of entries in pa[] */
    0,              /* number of entries in pb[] */
    1,              /* measure preamble pulse: */
                    /*  0 => even counts specifies cycle period */
                    /*  1 => even counts specifies off pulse period */
    0,              /* if true, pb[] matches a repeat sequence */
    50,             /* pulse tolerance */
    150,            /* bit Period = 1.50 ms (PS T0) */
    100,                /* delta T = 1000us */
    0,              /*  - " - (symbol pulse position) */
    0,              /*  - " - (measure spacing for complete cycle) */
    {50, 1},        /*  - " - (data symbol fix-width pulse period) */
    {30, 3},        /* bit period tolerance value = not applicable, */
                    /* and select code = 12.5% */
    24-1,           /* no. of symbols for sequence with preamble A */
    0,              /* no. of symbols for sequence with preamble B */
    1-1,            /* no. of data bits per symbol */
    1,              /* most/!least significant symbol received first */
    0,              /* left/!right adjust received data */
    0,              /* bi-phase/!pulse-spacing coded */
    0,              /* two symbols per cycle */
    0,              /* check stop symbol */
    0,              /* variable length data */
    50-1,           /* time-out clock divisor = 10us*50 = 500us */
    200,            /* frame time-out */
    21,             /* edge time-out = 10ms*/
    13,             /* minimum dead-time after fault = 6 ms */
    {0, 0},         /* stop symbol pulse or cycle period */
    0,              /* data symbol timeout */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const CIR_Param toshibaTC9012Param = {
/*
 * Initial frame has 32 bits of data. Using variable length decoding, Repeat
 * code treated as frame with 1 bit of data.
 */
    270-1,	/* count divisor: divide by 270 for 10us/count */
    { {450,0}, {900,0}, {0,0}, {0,0} }, /* pa[], preamble A pulse sequence */
    {   {0,0},   {0,0}, {0,0}, {0,0} }, /* pb[], preamble B pulse sequence */
    2,		/* number of entries in pa[] */
    0,		/* number of entries in pb[] */
    0,		/* measure preamble pulse: */
		/*	0 => even counts specifies cycle period */
    		/*	1 => even counts specifies off pulse period */
    0,		/* if true, pb[] matches a repeat sequence */
    0,	 	/* pulse tolerance */
    112,	/* T0 = 1.12ms */
    113,	/* delta = 1.13ms */
    0,		/* false => fix symbol pulse between edges 0 & 1 */
		/* true => fix symbol pulse between edges 1 & 2 */
    0,		/* false => measure spacing for complete cycle */
		/* true => measure spacing between 2 consecutive edges */
    {56, 2},	/* data symbol fix-width pulse period and tolerance:
				!!! TODO:adjust		50% */
    {0, 0},	/* spacing tolerance value = not applicable, */
		/* and tolerance select code = 12.5% */
    0,		/* no. of symbols for sequence with preamble A */
    0,		/* no. of symbols for sequence with preamble B */
    1-1,	/* no. of data bits per symbol */
    0,		/* most/!least significant symbol received first */
    0,		/* left/!right adjust received data */
    0,		/* bi-phase/!pulse-spacing coded */
    0,		/* two symbols per cycle */
    0,		/* check stop symbol */
    1,		/* variable length data */
    50-1,	/* time-out clock divisor: 500us per time-out unit */
    200,	/* frame time-out: 100ms */
    16, 	/* edge time-out: 8ms */
    16,		/* minimum dead-time after fault = 8 ms */
    {0, 0}, 	/* stop symbol pulse or cycle period */
    254,	/* data symbol timeout: T1 max + 2 */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BKIR_P_Handle
{
    uint32_t        magicId;                    /* Used to check if structure is corrupt */
    BCHP_Handle     hChip;
    BREG_Handle     hRegister;
    BINT_Handle     hInterrupt;
    unsigned int    maxChnNo;
    BKIR_ChannelHandle hKirChn[MAX_KIR_CHANNELS];
} BKIR_P_Handle;

typedef struct BKIR_P_ChannelHandle
{
    uint32_t            magicId;                    /* Used to check if structure is corrupt */
    BKIR_Handle         hKir;
    uint32_t            chnNo;
    uint32_t            coreOffset;
    BKNI_EventHandle    hChnEvent;
    BINT_CallbackHandle hChnCallback;
    BKIR_KirPort        irPort;                     /* port select setting */
    BKIR_KirPort        irPortSelected;             /* which port was selected (auto mode) */
    bool                intMode;
    bool                repeatFlag;                 /* flag indicating remote A repeat condition */
    bool                isCirMode;
    BKIR_Callback       kirCb;                      /* callback function */
    void                *cbData;                    /* data passed to callback function */
    CIR_Param           customCirParam;
    BKIR_KirDevice      customDevice;               /* device that this custom cir is used for */
                                                    /* this flag will be used for special handling. */
    bool                cir_pa;                     /* preamble A is detected. */
    bool                cir_pb;                     /* preamble B is detected. */
} BKIR_P_ChannelHandle;



/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BKIR_Settings defKirSettings = NULL;

static const BKIR_ChannelSettings defKir0ChnSettings =
{
    BKIR_KirPortAuto,
    true,
    0
};

#if BKIR_N_CHANNELS > 1
static const BKIR_ChannelSettings defKir1ChnSettings =
{
    BKIR_KirPortAuto,
    true,
    0
};

#if BKIR_N_CHANNELS > 2
static const BKIR_ChannelSettings defKir2ChnSettings =
{
    BKIR_KirPortAuto,
    true,
    0
};

#if BKIR_N_CHANNELS > 3
static const BKIR_ChannelSettings defKir3ChnSettings =
{
    BKIR_KirPortAuto,
    true,
    0
};
#endif
#endif
#endif

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BKIR_Open(
    BKIR_Handle *pKir,                  /* [output] Returns handle */
    BCHP_Handle hChip,                  /* Chip handle */
    BREG_Handle hRegister,              /* Register handle */
    BINT_Handle hInterrupt,             /* Interrupt handle */
    const BKIR_Settings *pDefSettings   /* Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BKIR_Handle hDev;
    unsigned int chnIdx;
    BSTD_UNUSED(pDefSettings);

    /* Sanity check on the handles we've been given. */
    BDBG_ASSERT( hChip );
    BDBG_ASSERT( hRegister );
    BDBG_ASSERT( hInterrupt );

    /* Alloc memory from the system heap */
    hDev = (BKIR_Handle) BKNI_Malloc( sizeof( BKIR_P_Handle ) );
    if( hDev == NULL )
    {
        *pKir = NULL;
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BKIR_Open: BKNI_malloc() failed\n"));
        goto done;
    }

    hDev->magicId   = DEV_MAGIC_ID;
    hDev->hChip     = hChip;
    hDev->hRegister = hRegister;
    hDev->hInterrupt = hInterrupt;
    hDev->maxChnNo  = MAX_KIR_CHANNELS;
    for( chnIdx = 0; chnIdx < hDev->maxChnNo; chnIdx++ )
    {
        hDev->hKirChn[chnIdx] = NULL;
    }

    *pKir = hDev;

done:
    return( retCode );
}

BERR_Code BKIR_Close(
    BKIR_Handle hDev                    /* Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BKNI_Free( (void *) hDev );

    return( retCode );
}

BERR_Code BKIR_GetDefaultSettings(
    BKIR_Settings *pDefSettings,        /* [output] Returns default setting */
    BCHP_Handle hChip                   /* Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hChip);

    *pDefSettings = defKirSettings;

    return( retCode );
}

BERR_Code BKIR_GetTotalChannels(
    BKIR_Handle hDev,                   /* Device handle */
    unsigned int *totalChannels         /* [output] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    *totalChannels = hDev->maxChnNo;

    return( retCode );
}

BERR_Code BKIR_GetChannelDefaultSettings(
    BKIR_Handle hDev,                   /* Device handle */
    unsigned int channelNo,             /* Channel number to default setting for */
    BKIR_ChannelSettings *pChnDefSettings /* [output] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

#if !BDBG_DEBUG_BUILD
    BSTD_UNUSED(hDev);
#endif

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    switch (channelNo)
    {
        case 0:
            *pChnDefSettings = defKir0ChnSettings;
            break;

#if BKIR_N_CHANNELS > 1
        case 1:
            *pChnDefSettings = defKir1ChnSettings;
            break;

#if BKIR_N_CHANNELS > 2
        case 2:
            *pChnDefSettings = defKir2ChnSettings;
            break;

#if BKIR_N_CHANNELS > 3
        case 3:
            *pChnDefSettings = defKir3ChnSettings;
            break;
#endif
#endif
#endif

        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;

    }

    return( retCode );
}

BERR_Code BKIR_GetDefaultCirParam (
    BKIR_ChannelHandle  hChn,            /* Device channel handle */
    BKIR_KirDevice      device,          /* device type to enable */
    CIR_Param           *pCustomCirParam /* [output] Returns default setting */
)
{
    BERR_Code           retCode = BERR_SUCCESS;
    const CIR_Param         *pCirParam = NULL;

    BSTD_UNUSED(hChn);
    switch (device)
    {
        case BKIR_KirDevice_eCirGI:
            pCirParam = &giParam;
            break;
        case BKIR_KirDevice_eCirSaE2050:
            pCirParam = &sa_e2050Param;
            break;
        case BKIR_KirDevice_eCirTwirp:
            pCirParam = &twirpParam;
            break;
        case BKIR_KirDevice_eCirSony:
            pCirParam = &sonyParam;
            break;
        case BKIR_KirDevice_eCirRecs80:
            pCirParam = &recs80Param;
            break;
        case BKIR_KirDevice_eCirRc5:
            pCirParam = &rc5Param;
            break;
        case BKIR_KirDevice_eCirUei:
            pCirParam = &ueiParam;
            break;
        case BKIR_KirDevice_eCirRfUei:
            pCirParam = &RfueiParam;
            break;
        case BKIR_KirDevice_eCirEchoStar:
            pCirParam = &echoDishRemoteParam;
            break;
        case BKIR_KirDevice_eCirNec:
            pCirParam = &necParam;
            break;
        case BKIR_KirDevice_eCirGISat:
            pCirParam = &giSatParam;
            break;
        case BKIR_KirDevice_eCirCustom:
            pCirParam = &(hChn->customCirParam);
            break;
        case BKIR_KirDevice_eCirRC6:
            pCirParam = &rC6Mode6AParam;
            break;
        case BKIR_KirDevice_eCirDirectvUhfr:
            pCirParam = &directvUHFParam;
            break;
        case BKIR_KirDevice_eCirEchostarUhfr:
            pCirParam = &echostarUHFParam;
            break;
        case BKIR_KirDevice_eCirRcmmRcu:
            pCirParam = &s_RCMMParam;
            break;
        case BKIR_KirDevice_eCirRstep:
            pCirParam = &rStepParam;
            break;
        case BKIR_KirDevice_eCirXmp2:
            pCirParam = &xmp2Param;
            break;
        case BKIR_KirDevice_eCirXmp2Ack:
            pCirParam = &xmp2AckParam;
            break;
        case BKIR_KirDevice_eCirRC6Mode0:
            pCirParam = &rC6Mode0Param;
            break;
        case BKIR_KirDevice_eCirRca:
            pCirParam = &rcaParam;
            break;
        case BKIR_KirDevice_eCirToshibaTC9012:
            pCirParam = &toshibaTC9012Param;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }
    if (retCode == BERR_SUCCESS)
    {
        BKNI_Memcpy ((void *)pCustomCirParam, (void *)pCirParam, sizeof(CIR_Param));
    }

    return( retCode );
}

BERR_Code BKIR_OpenChannel(
    BKIR_Handle hDev,                   /* Device handle */
    BKIR_ChannelHandle *phChn,          /* [output] Returns channel handle */
    unsigned int channelNo,             /* Channel number to open */
    const BKIR_ChannelSettings *pChnDefSettings /* Channel default setting */
    )
{
    BERR_Code           retCode = BERR_SUCCESS;
    BKIR_ChannelHandle  hChnDev;
    uint32_t            lval;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hChnDev = NULL;

    if( channelNo < hDev->maxChnNo )
    {
        if( hDev->hKirChn[channelNo] == NULL )
        {
            /* Alloc memory from the system heap */
            hChnDev = (BKIR_ChannelHandle) BKNI_Malloc( sizeof( BKIR_P_ChannelHandle ) );
            if( hChnDev == NULL )
            {
                *phChn = NULL;
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BKIR_OpenChannel: BKNI_malloc() failed\n"));
                goto done;
            }

            BKIR_CHK_RETCODE( retCode, BKNI_CreateEvent( &(hChnDev->hChnEvent) ) );
            hChnDev->magicId    = DEV_MAGIC_ID;
            hChnDev->hKir       = hDev;
            hChnDev->chnNo      = channelNo;
            hChnDev->irPort     = pChnDefSettings->irPort;
            hChnDev->isCirMode  = false;
            hChnDev->kirCb      = NULL;
            hChnDev->cbData     = NULL;
            hChnDev->customDevice = pChnDefSettings->customDevice;
            hDev->hKirChn[channelNo] = hChnDev;

            /*
             * Offsets are based off of BSCA
             */
            if (channelNo == 0)
                hChnDev->coreOffset = 0;
#if BKIR_N_CHANNELS > 1
            else if (channelNo == 1)
                hChnDev->coreOffset = BCHP_KBD2_STATUS - BCHP_KBD1_STATUS;
#if BKIR_N_CHANNELS > 2
            else if (channelNo == 2)
                hChnDev->coreOffset = BCHP_KBD3_STATUS - BCHP_KBD1_STATUS;
#if BKIR_N_CHANNELS > 3
            else if (channelNo == 3)
                hChnDev->coreOffset = BCHP_KBD4_STATUS - BCHP_KBD1_STATUS;
#endif
#endif
#endif

            /* initialize the CMD register to the default value. */
            BREG_Write32(hDev->hRegister, hChnDev->coreOffset + BCHP_KBD1_CMD, 0x07);
            
            /*
             * Enable interrupt for this channel
             */
            hChnDev->intMode = pChnDefSettings->intMode;
            if (hChnDev->intMode == true)
            {
                static const BINT_Id IntId[BKIR_N_CHANNELS] =
                {
                    #if defined(BCHP_INT_ID_kbd1)
                        BCHP_INT_ID_kbd1
                    #else
                        BCHP_INT_ID_kbd1_irqen
                    #endif
                    #if BKIR_N_CHANNELS > 1
                        ,BCHP_INT_ID_kbd2_irqen
                    #if BKIR_N_CHANNELS > 2
                        ,BCHP_INT_ID_kbd3_irqen
                    #if BKIR_N_CHANNELS > 3
                        ,BCHP_INT_ID_kbd4_irqen
                    #endif
                    #endif
                    #endif
                };
                /*
                 * Register and enable L2 interrupt.
                 */
                BKIR_CHK_RETCODE( retCode, BINT_CreateCallback(
                    &(hChnDev->hChnCallback), hDev->hInterrupt, IntId[channelNo],
                    BKIR_P_HandleInterrupt_Isr, (void *) hChnDev, 0x00 ) );
                BKIR_CHK_RETCODE( retCode, BINT_EnableCallback( hChnDev->hChnCallback ) );

                BKNI_EnterCriticalSection();

                /*
                 * Enable KIR interrupt in UPG
                 */
                lval = BREG_Read32(hDev->hRegister, BCHP_IRQ0_IRQEN);
                if (channelNo == 0)
                    #if defined(BCHP_IRQ0_AON_IRQEN_kbd1_irqen_MASK)
                        lval |= BCHP_IRQ0_AON_IRQEN_kbd1_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd1_irqen_MASK)
                        lval |= BCHP_IRQ0_IRQEN_kbd1_irqen_MASK;
                    #else
                        lval |= BCHP_IRQ0_IRQEN_kbd1_MASK;
                    #endif
#if BKIR_N_CHANNELS > 1
                else if (channelNo == 1)
                    #if defined(BCHP_IRQ0_AON_IRQEN_kbd2_irqen_MASK)
                        lval |= BCHP_IRQ0_AON_IRQEN_kbd2_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd2_irqen_MASK)
                        lval |= BCHP_IRQ0_IRQEN_kbd2_irqen_MASK;
                    #else
                        lval |= BCHP_IRQ0_IRQEN_kbd2_MASK;
                    #endif
#if BKIR_N_CHANNELS > 2
                else if (channelNo == 2)
                    #if defined(BCHP_IRQ0_AON_IRQEN_kbd3_irqen_MASK)
                        lval |= BCHP_IRQ0_AON_IRQEN_kbd3_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd3_irqen_MASK)
                        lval |= BCHP_IRQ0_IRQEN_kbd3_irqen_MASK;
                    #else
                        lval |= BCHP_IRQ0_IRQEN_kbd3_MASK;
                    #endif
#if BKIR_N_CHANNELS > 3
                else if (channelNo == 3)
                    #if defined(BCHP_IRQ0_IRQEN_kbd4_irqen_MASK)
                        lval |= BCHP_IRQ0_IRQEN_kbd4_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd4_irqen_MASK)
                        lval |= BCHP_IRQ0_IRQEN_kbd4_irqen_MASK;
                    #else
                        lval |= BCHP_IRQ0_IRQEN_kbd4_MASK;
                    #endif
#endif
#endif
#endif
                BREG_Write32( hDev->hRegister, BCHP_IRQ0_IRQEN, lval );

                /*
                 * Enable KIR interrupt in KIR
                 */
                BKIR_P_EnableInt (hChnDev);
                BKNI_LeaveCriticalSection();
            }
            else
            {
                hChnDev->hChnCallback = NULL;

                BKNI_EnterCriticalSection();
                /*
                 * Disable KIR interrupt in UPG
                 */
                lval = BREG_Read32(hDev->hRegister, BCHP_IRQ0_IRQEN);
                if (channelNo == 0)
                    #if defined(BCHP_IRQ0_AON_IRQEN_kbd1_irqen_MASK)
                        lval &= ~BCHP_IRQ0_AON_IRQEN_kbd1_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd1_irqen_MASK)
                        lval &= ~BCHP_IRQ0_IRQEN_kbd1_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd1_MASK)
                        lval &= ~BCHP_IRQ0_IRQEN_kbd1_MASK;
                    #endif
#if BKIR_N_CHANNELS > 1
                else if (channelNo == 1)
                    #if defined(BCHP_IRQ0_AON_IRQEN_kbd2_irqen_MASK)
                        lval &= ~BCHP_IRQ0_AON_IRQEN_kbd2_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd2_irqen_MASK)
                        lval &= ~BCHP_IRQ0_IRQEN_kbd2_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd2_MASK)
                        lval &= ~BCHP_IRQ0_IRQEN_kbd2_MASK;
                    #endif
#if BKIR_N_CHANNELS > 2
                else if (channelNo == 2)
                    #if defined(BCHP_IRQ0_AON_IRQEN_kbd3_irqen_MASK)
                        lval &= ~BCHP_IRQ0_AON_IRQEN_kbd3_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd3_irqen_MASK)
                        lval &= ~BCHP_IRQ0_IRQEN_kbd3_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd3_MASK)
                        lval &= ~BCHP_IRQ0_IRQEN_kbd3_MASK;
                    #endif
#if BKIR_N_CHANNELS > 3
                else if (channelNo == 3)
                    #if defined(BCHP_IRQ0_AON_IRQEN_kbd4_irqen_MASK)
                        lval &= ~BCHP_IRQ0_AON_IRQEN_kbd4_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd4_irqen_MASK)
                        lval &= ~BCHP_IRQ0_IRQEN_kbd4_irqen_MASK;
                    #elif defined(BCHP_IRQ0_IRQEN_kbd4_MASK)
                        lval &= ~BCHP_IRQ0_IRQEN_kbd4_MASK;
                    #endif
#endif
#endif
#endif
                BREG_Write32( hDev->hRegister, BCHP_IRQ0_IRQEN, lval );

                /*
                 * Disable KIR interrupt in KIR
                 */
                BKIR_P_DisableInt (hChnDev);
                BKNI_LeaveCriticalSection();
            }

            *phChn = hChnDev;
        }
        else
        {
            retCode = BERR_TRACE(BKIR_ERR_NOTAVAIL_CHN_NO);
        }
    }
    else
    {
        retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
    }

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hChnDev != NULL )
        {
            BKNI_DestroyEvent( hChnDev->hChnEvent );
            BKNI_Free( hChnDev );
            hDev->hKirChn[channelNo] = NULL;
            *phChn = NULL;
        }
    }
    return( retCode );
}

BERR_Code BKIR_CloseChannel(
    BKIR_ChannelHandle hChn         /* Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BKIR_Handle hDev;
    unsigned int chnNo;


    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;
    /*
     * Disable interrupt for this channel
     */
    BKNI_EnterCriticalSection();
    BKIR_P_DisableInt (hChn);
    BKNI_LeaveCriticalSection();

    if (hChn->hChnCallback) {
        BKIR_CHK_RETCODE( retCode, BINT_DisableCallback( hChn->hChnCallback ) );
        BKIR_CHK_RETCODE( retCode, BINT_DestroyCallback( hChn->hChnCallback ) );
    }
    hChn->kirCb     = NULL;
    hChn->cbData    = NULL;
    BKNI_DestroyEvent( hChn->hChnEvent );
    chnNo = hChn->chnNo;
    BKNI_Free( hChn );
    hDev->hKirChn[chnNo] = NULL;

done:
    return( retCode );
}

BERR_Code BKIR_GetDevice(
    BKIR_ChannelHandle hChn,            /* Device channel handle */
    BKIR_Handle *phDev                  /* [output] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phDev = hChn->hKir;

    return( retCode );
}


BERR_Code BKIR_EnableIrDevice (
    BKIR_ChannelHandle  hChn,       /* Device channel handle */
    BKIR_KirDevice      device          /* device type to enable */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval;
    BKIR_Handle hDev;
    bool isCirDevice = false;
    BKIR_KirDevice      tempDevice;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD);
    switch( device )
    {
        case BKIR_KirDevice_eTwirpKbd:
            lval = lval | KBD_CMD_TWIRP_ENABLE;
            break;

        case BKIR_KirDevice_eSejin38KhzKbd:
            lval = (lval | KBD_CMD_SEJIN_ENABLE) & ~BCHP_KBD1_CMD_alt_table_MASK;
            break;

        case BKIR_KirDevice_eSejin56KhzKbd:
            lval = lval | KBD_CMD_SEJIN_ENABLE | BCHP_KBD1_CMD_alt_table_MASK;
            break;

        case BKIR_KirDevice_eRemoteA:
            lval = lval | KBD_CMD_REMOTE_A_ENABLE;
            break;

        case BKIR_KirDevice_eRemoteB:
            lval = lval | KBD_CMD_REMOTE_B_ENABLE;
            break;

        case BKIR_KirDevice_eCirGI:
        case BKIR_KirDevice_eCirSaE2050:
        case BKIR_KirDevice_eCirTwirp:
        case BKIR_KirDevice_eCirSony:
        case BKIR_KirDevice_eCirRecs80:
        case BKIR_KirDevice_eCirRc5:
        case BKIR_KirDevice_eCirUei:
        case BKIR_KirDevice_eCirRfUei:
        case BKIR_KirDevice_eCirEchoStar:
        case BKIR_KirDevice_eCirNec:
        case BKIR_KirDevice_eCirGISat:
        case BKIR_KirDevice_eCirCustom:
        case BKIR_KirDevice_eCirDirectvUhfr:
        case BKIR_KirDevice_eCirEchostarUhfr:
        case BKIR_KirDevice_eCirRC6:
        case BKIR_KirDevice_eCirRcmmRcu:
        case BKIR_KirDevice_eCirRstep:
        case BKIR_KirDevice_eCirXmp2:
        case BKIR_KirDevice_eCirXmp2Ack:
        case BKIR_KirDevice_eCirRC6Mode0:
        case BKIR_KirDevice_eCirRca:
        case BKIR_KirDevice_eCirToshibaTC9012:
            isCirDevice = true;
            if ((device == BKIR_KirDevice_eCirUei) || (device == BKIR_KirDevice_eCirDirectvUhfr) || (device == BKIR_KirDevice_eCirRfUei))
            {
                /* This flag is used to read repeat flag in ISR.
                 * This flag should apply to all CIR devices.
                 * Until, we test more CIR devices, use it for
                 * UEI device only.
                 */
                hChn->isCirMode = true;
            }
            if (device == BKIR_KirDevice_eCirCustom)
            {
                if ((hChn->customDevice == BKIR_KirDevice_eCirUei) || (hChn->customDevice == BKIR_KirDevice_eCirDirectvUhfr) || (hChn->customDevice == BKIR_KirDevice_eCirRfUei))
                {
                    /* This flag is used to read repeat flag in ISR.
                     * This flag should apply to all CIR devices.
                     * Until, we test more CIR devices, use it for
                     * UEI device only.
                     */
                    hChn->isCirMode = true;
                }
            }
            lval = lval | KBD_CMD_CIR_ENABLE;
            if ((device == BKIR_KirDevice_eCirRfUei) ||
                (device == BKIR_KirDevice_eCirDirectvUhfr) ||
                (device == BKIR_KirDevice_eCirEchostarUhfr) ||
                (device == BKIR_KirDevice_eCirRC6))
                lval &= 0xf0;       /* disable TWIRP, SEJIN, REMOTE A, and REMOTE B */

            if (device == BKIR_KirDevice_eCirCustom)
            {
                if ((hChn->customDevice == BKIR_KirDevice_eCirRfUei) ||
                    (hChn->customDevice == BKIR_KirDevice_eCirDirectvUhfr) ||
                    (hChn->customDevice == BKIR_KirDevice_eCirEchostarUhfr) ||
                    (hChn->customDevice == BKIR_KirDevice_eCirRC6))
                    lval &= 0xf0;       /* disable TWIRP, SEJIN, REMOTE A, and REMOTE B */
            }
            break;

        default:
            retCode = BERR_INVALID_PARAMETER;
            goto done;
    }

    BDBG_MSG(("%s Write32(%x, %x)", __FUNCTION__, hChn->coreOffset + BCHP_KBD1_CMD, lval));
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD, lval);

    /* Set KBD3.FILTER1 to 0x0000007F for UHF */
    tempDevice = device; /* to do special handling */
    if (device == BKIR_KirDevice_eCirCustom)
    {
        /* Use custom device to set special filter */
        tempDevice = hChn->customDevice ;
    }
    switch ( tempDevice )
    {
        case BKIR_KirDevice_eCirDirectvUhfr:
        case BKIR_KirDevice_eCirEchostarUhfr:
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1, 0x0000003F | BCHP_KBD1_FILTER1_filter_en_MASK);
            break;
        case BKIR_KirDevice_eCirUei:
            BDBG_MSG(("%s Write32(%x, %x -> %x)", __FUNCTION__, hChn->coreOffset + BCHP_KBD1_FILTER1, BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1), 0x00000034 | BCHP_KBD1_FILTER1_filter_en_MASK));
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1, 0x00000034 | BCHP_KBD1_FILTER1_filter_en_MASK);
            break;
        case BKIR_KirDevice_eCirXmp2:
        case BKIR_KirDevice_eCirXmp2Ack:
            BDBG_MSG(("%s Write32(%x, %x -> %x)", __FUNCTION__, hChn->coreOffset + BCHP_KBD1_FILTER1, BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1), 0x00000011 | BCHP_KBD1_FILTER1_filter_en_MASK));
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1, 0x00000011 | BCHP_KBD1_FILTER1_filter_en_MASK);
            break;
        default:
            BDBG_MSG(("%s Write32(%x, %x)", __FUNCTION__, hChn->coreOffset + BCHP_KBD1_FILTER1, 0x00000000 & ~BCHP_KBD1_FILTER1_filter_en_MASK));
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1, 0x00000000 & ~BCHP_KBD1_FILTER1_filter_en_MASK);
            break;
   }

    /* Program CIR reg file for CIR devices */
    if (isCirDevice)
    {
        BKIR_P_ConfigCir (hChn, device);
    }

done:

    return( retCode );
}

BERR_Code BKIR_DisableIrDevice (
    BKIR_ChannelHandle  hChn,       /* Device channel handle */
    BKIR_KirDevice      device          /* device type to enable */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval;
    BKIR_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD);

    switch( device )
    {
        case BKIR_KirDevice_eTwirpKbd:
            lval = lval & ~KBD_CMD_TWIRP_ENABLE;
            break;

        case BKIR_KirDevice_eSejin38KhzKbd:
        case BKIR_KirDevice_eSejin56KhzKbd:
            lval = lval & ~KBD_CMD_SEJIN_ENABLE;
            break;

        case BKIR_KirDevice_eRemoteA:
            lval = lval & ~KBD_CMD_REMOTE_A_ENABLE;
            break;

        case BKIR_KirDevice_eRemoteB:
            lval = lval & ~KBD_CMD_REMOTE_B_ENABLE;
            break;

        case BKIR_KirDevice_eCirGI:
        case BKIR_KirDevice_eCirSaE2050:
        case BKIR_KirDevice_eCirTwirp:
        case BKIR_KirDevice_eCirSony:
        case BKIR_KirDevice_eCirRecs80:
        case BKIR_KirDevice_eCirRc5:
        case BKIR_KirDevice_eCirUei:
        case BKIR_KirDevice_eCirRfUei:
        case BKIR_KirDevice_eCirEchoStar:
        case BKIR_KirDevice_eCirNec:
        case BKIR_KirDevice_eCirGISat:
        case BKIR_KirDevice_eCirCustom:
        case BKIR_KirDevice_eCirDirectvUhfr:
        case BKIR_KirDevice_eCirEchostarUhfr:
        case BKIR_KirDevice_eCirRC6:
        case BKIR_KirDevice_eCirRcmmRcu:
        case BKIR_KirDevice_eCirRstep:
        case BKIR_KirDevice_eCirXmp2:
        case BKIR_KirDevice_eCirXmp2Ack:
        case BKIR_KirDevice_eCirRC6Mode0:
        case BKIR_KirDevice_eCirRca:
        case BKIR_KirDevice_eCirToshibaTC9012:
            lval = lval & ~KBD_CMD_CIR_ENABLE;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto done;
    }

    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD, lval);
done:
    return( retCode );
}

BERR_Code BKIR_EnableDataFilter (
    BKIR_ChannelHandle  hChn,       /* Device channel handle */
    uint64_t            pat0,       /* pattern0 to match (only least significant 48bits are used) */
    uint64_t            pat1,       /* pattern1 to match (only least significant 48bits are used) */
    uint64_t            mask0,      /* don't care bits in pattern0 (only least significant 48bits are used) */
    uint64_t            mask1       /* don't care bits in pattern1 (only least significant 48bits are used) */
    )
{
    #ifdef BCHP_KBD1_CMD_data_filtering_MASK
    uint32_t lval;
    BKIR_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

        #ifdef BCHP_KBD1_KBD_MASK2_kbd_mask0_MASK
            /* write 48 bit pattern to pattern and mask bits */
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_PAT0, pat0 & 0xffffffff);
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_PAT1, ((pat1 & 0xffff) << 16) | ((pat0 >> 32) & 0xffff));
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_PAT2, ((pat1 >> 16) & 0xffffffff));
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_MASK0, (mask0 & 0xffffffff));
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_MASK1, ((mask1 & 0xffff) << 16) | ((mask0 >> 32) & 0xffff));
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_MASK2, ((mask1 >> 16) & 0xffffffff));
        #else
            BSTD_UNUSED(pat1);

            /* not all chips support the second filter, so return error if the user implies usage */
            if (~mask1)
                return( BERR_NOT_SUPPORTED );

    /* write pattern to match and mask bits */
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_PAT0, pat0 & 0xffffffff);
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_PAT1, (pat0 >> 32) & 0xffff);
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_MASK0, mask0 & 0xffffffff);
            BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_KBD_MASK1, (mask0 >> 32) & 0xffff);
        #endif

    /* enable data filtering based on pattern */
    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD);
    lval = (lval | BCHP_KBD1_CMD_data_filtering_MASK);
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD, lval);

        return( BERR_SUCCESS );
    #else
        BSTD_UNUSED(hChn);
        BSTD_UNUSED(pat0);
        BSTD_UNUSED(pat1);
        BSTD_UNUSED(mask0);
        BSTD_UNUSED(mask1);
        return( BERR_NOT_SUPPORTED );
    #endif
}

BERR_Code BKIR_DisableDataFilter (
    BKIR_ChannelHandle  hChn        /* Device channel handle */
    )
{
#ifdef BCHP_KBD1_CMD_data_filtering_MASK
    uint32_t lval;
    BKIR_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

    /* disable data filtering */
    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD);
    lval = (lval & ~BCHP_KBD1_CMD_data_filtering_MASK);
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD, lval);

    return( BERR_SUCCESS );
#else
    BSTD_UNUSED(hChn);
    return( BERR_NOT_SUPPORTED );
#endif
}

BERR_Code BKIR_EnableFilter1 (
    BKIR_ChannelHandle  hChn,       	/* Device channel handle */
    unsigned int        filter_width 	/* filter width if smaller than this to be rejected */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval;
    BKIR_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

    /* enable filter1 and write filter width. */
    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1);
    lval &= ~BCHP_KBD1_FILTER1_filter_en_MASK;
    lval &= ~BCHP_KBD1_FILTER1_filter_width_MASK;
    lval = (lval | BCHP_KBD1_FILTER1_filter_en_MASK | filter_width);
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1, lval);

    return( retCode );
}

BERR_Code BKIR_DisableFilter1 (
    BKIR_ChannelHandle  hChn        /* Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval;
    BKIR_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

    /* disable filter1 and clear filter width. */
    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1);
    lval &= ~BCHP_KBD1_FILTER1_filter_en_MASK;
    lval &= ~BCHP_KBD1_FILTER1_filter_width_MASK;
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_FILTER1, lval);

    return( retCode );
}

BERR_Code BKIR_DisableAllIrDevices (
    BKIR_ChannelHandle  hChn        /* Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval;
    BKIR_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD);

    lval &= ~(  KBD_CMD_TWIRP_ENABLE |
            KBD_CMD_SEJIN_ENABLE |
            KBD_CMD_REMOTE_A_ENABLE |
            KBD_CMD_REMOTE_B_ENABLE |
            KBD_CMD_CIR_ENABLE);

    BDBG_WRN(("%s Write32(%x, %x)", __FUNCTION__, hChn->coreOffset + BCHP_KBD1_CMD, lval));
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD, lval);

    return( retCode );
}

BERR_Code BKIR_GetEventHandle(
    BKIR_ChannelHandle hChn,            /* Device channel handle */
    BKNI_EventHandle *phEvent           /* [output] Returns event handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phEvent = hChn->hChnEvent;

    return( retCode );
}



BERR_Code BKIR_IsDataReady (
    BKIR_ChannelHandle  hChn,           /* Device channel handle */
    bool                *dataReady      /* [output] flag to indicate if data is ready */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval;
    BKIR_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

    /*
     * This function should only be called when polling for data ready.
     * If interrupt is enabled, the caller should pend on interrupt event
     */
    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_STATUS);
    *dataReady = (lval & BCHP_KBD1_STATUS_irq_MASK) ? true : false;

    return( retCode );
}


BERR_Code BKIR_Read_Isr(
    BKIR_ChannelHandle      hChn,           /* Device channel handle */
    BKIR_KirInterruptDevice *pDevice,       /* [output] pointer to IR device type that generated the key */
    unsigned char           *data           /* [output] pointer to data received */
    )
{
    BERR_Code       retCode = BERR_SUCCESS;
    uint32_t        lval;
    BKIR_Handle     hDev;
    uint32_t data0;
    uint32_t data1;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hKir;

    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_STATUS);
    *pDevice = (BKIR_KirInterruptDevice)((lval & KBD_STATUS_DEVICE_MASK) >> KBD_STATUS_DEVICE_SHIFTS);

    data0 = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_DATA0);
    data1 = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_DATA1);
    data[0] = data0       & 0xff;
    data[1] = data0 >> 8  & 0xff;
    data[2] = data0 >> 16 & 0xff;
    data[3] = data0 >> 24 & 0xff;
    data[4] = data1       & 0xff;
#if BCHP_KBD1_CMD_data_filtering_MASK
    /* 7550 can store up to 48 bit */
    data[5] = data1 >> 8  & 0xff;
#endif

    return( retCode );
}


BERR_Code BKIR_Read(
    BKIR_ChannelHandle      hChn,           /* Device channel handle */
    BKIR_KirInterruptDevice *pDevice,       /* [output] pointer to IR device type that generated the key */
    unsigned char           *data           /* [output] pointer to data received */
    )
{
    BERR_Code rc;

    BKNI_EnterCriticalSection();
    rc = BKIR_Read_Isr (hChn, pDevice, data);
    BKNI_LeaveCriticalSection();

    return rc;
}

BERR_Code BKIR_IsRepeated(
    BKIR_ChannelHandle      hChn,           /* [in] Device channel handle */
    bool                    *repeatFlag     /* [out] flag to remote A repeat condition */
    )
{
    BERR_Code       retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *repeatFlag = hChn->repeatFlag;

    return( retCode );
}

BERR_Code BKIR_IsPreambleA(
    BKIR_ChannelHandle      hChn,           /* [in] Device channel handle */
    bool                    *preambleFlag   /* [out] flag to remote A repeat condition */
    )
{
    BERR_Code       retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *preambleFlag = hChn->cir_pa;

    return( retCode );
}

BERR_Code BKIR_IsPreambleB(
    BKIR_ChannelHandle      hChn,           /* [in] Device channel handle */
    bool                    *preambleFlag   /* [out] flag to remote A repeat condition */
    )
{
    BERR_Code       retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *preambleFlag = hChn->cir_pb;

    return( retCode );
}

void BKIR_SetCustomDeviceType (
    BKIR_ChannelHandle  hChn,           /* Device channel handle */
    BKIR_KirDevice      customDevice    /* device that this custom cir is used for */
)
{
    hChn->customDevice = customDevice;
}

void BKIR_SetCustomCir (
    BKIR_ChannelHandle  hChn,       /* Device channel handle */
    CIR_Param           *pCirParam
)
{
    BSTD_UNUSED(hChn);
    BKNI_Memcpy ((void *)&(hChn->customCirParam), (void *)pCirParam, sizeof(CIR_Param));
}

void BKIR_RegisterCallback (
    BKIR_ChannelHandle  hChn,       /* Device channel handle */
    BKIR_Callback       callback,   /* Callback function to register */
    void                *pData      /* Data passed to callback function */
)
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hChn->kirCb = callback;
    hChn->cbData = pData;
}

void BKIR_UnregisterCallback (
    BKIR_ChannelHandle  hChn        /* Device channel handle */
)
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hChn->kirCb = NULL;
    hChn->cbData = NULL;
}

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
void BKIR_P_EnableInt(
    BKIR_ChannelHandle  hChn            /* Device channel handle */
    )
{
    uint32_t    lval;
    BKIR_Handle hDev;

    hDev = hChn->hKir;
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_STATUS, 0);  /* clear any previous interrupt */
    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD);
    lval |= BCHP_KBD1_CMD_kbd_irqen_MASK;
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD, lval);

}

void BKIR_P_DisableInt(
    BKIR_ChannelHandle  hChn            /* Device channel handle */
    )
{
    uint32_t    lval;
    BKIR_Handle hDev;

    hDev = hChn->hKir;

    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD);
    lval &= ~BCHP_KBD1_CMD_kbd_irqen_MASK;
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CMD, lval);

}

static void BKIR_P_HandleInterrupt_Isr
(
    void *pParam1,                      /* Device channel handle */
    int parm2                           /* not used */
)
{
    BKIR_ChannelHandle  hChn;
    BKIR_Handle         hDev;
    uint32_t            lval;
    BSTD_UNUSED(parm2);

    hChn = (BKIR_ChannelHandle) pParam1;
    BDBG_ASSERT( hChn );

    hDev = hChn->hKir;
    /* Clear interrupt */
    lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_STATUS);

    /* Check if preamble is detected */
    hChn->cir_pa = false;
    hChn->cir_pb = false;

    /* If we're in Cir Mode then test the 'cir_pb' status for repeat keys. */
    if ( hChn->isCirMode )
    {
        hChn->cir_pa = (lval & BCHP_KBD1_STATUS_cir_pa_MASK) ? true : false;
        hChn->cir_pb = (lval & BCHP_KBD1_STATUS_cir_pb_MASK) ? true : false;
        hChn->repeatFlag = (lval & BCHP_KBD1_STATUS_cir_pb_MASK) ? true : false;
    }
    else
    {
        hChn->cir_pa = (lval & BCHP_KBD1_STATUS_cir_pa_MASK) ? true : false;
        hChn->cir_pb = (lval & BCHP_KBD1_STATUS_cir_pb_MASK) ? true : false;
        hChn->repeatFlag = (lval & BCHP_KBD1_STATUS_rflag_MASK) ? true : false;
    }

    lval &= ~BCHP_KBD1_STATUS_irq_MASK;
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_STATUS, lval);

    /*
     * Call any callback function if installed
     */
    if (hChn->kirCb)
    {
        (*hChn->kirCb)( hChn, hChn->cbData );
    }

    BKNI_SetEvent( hChn->hChnEvent );
    return;
}

void BKIR_P_WriteCirParam (
    BKIR_ChannelHandle  hChn,       /* Device channel handle */
    uint32_t                addr,
    uint32_t                data
)
{
    BKIR_Handle         hDev;
    uint32_t                lval;

    hDev = hChn->hKir;
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CIR_ADDR, addr);
    lval = data& 0xFFF;
    BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_KBD1_CIR_DATA, lval);
}

void BKIR_P_ConfigCir (
    BKIR_ChannelHandle  hChn,       /* Device channel handle */
    BKIR_KirDevice      device      /* device type to enable */
)
{
    uint32_t        ulData;
    uint32_t        uli, ulj;
    const CIR_Param *pCirParam;
    BKIR_Handle     hDev;

    hDev = hChn->hKir;
    switch (device)
    {
        case BKIR_KirDevice_eCirGI:
            pCirParam = &giParam;
            break;
        case BKIR_KirDevice_eCirSaE2050:
            pCirParam = &sa_e2050Param;
            break;
        case BKIR_KirDevice_eCirTwirp:
            pCirParam = &twirpParam;
            break;
        case BKIR_KirDevice_eCirSony:
            pCirParam = &sonyParam;
            break;
        case BKIR_KirDevice_eCirRecs80:
            pCirParam = &recs80Param;
            break;
        case BKIR_KirDevice_eCirRc5:
            pCirParam = &rc5Param;
            break;
        case BKIR_KirDevice_eCirUei:
            pCirParam = &ueiParam;
            break;
        case BKIR_KirDevice_eCirRfUei:
            pCirParam = &RfueiParam;
            break;
        case BKIR_KirDevice_eCirEchoStar:
            pCirParam = &echoDishRemoteParam;
            break;
        case BKIR_KirDevice_eCirNec:
            pCirParam = &necParam;
            break;
        case BKIR_KirDevice_eCirGISat:
            pCirParam = &giSatParam;
            break;
        case BKIR_KirDevice_eCirCustom:
            pCirParam = &(hChn->customCirParam);
            break;
        case BKIR_KirDevice_eCirRC6:
            pCirParam = &rC6Mode6AParam;
            break;
        case BKIR_KirDevice_eCirDirectvUhfr:
            pCirParam = &directvUHFParam;
            break;
        case BKIR_KirDevice_eCirEchostarUhfr:
            pCirParam = &echostarUHFParam;
            break;
        case BKIR_KirDevice_eCirRcmmRcu:
            pCirParam = &s_RCMMParam;
            break;
        case BKIR_KirDevice_eCirRstep:
            pCirParam = &rStepParam;
            break;
        case BKIR_KirDevice_eCirXmp2:
            pCirParam = &xmp2Param;
            break;
        case BKIR_KirDevice_eCirXmp2Ack:
            pCirParam = &xmp2AckParam;
            break;
        case BKIR_KirDevice_eCirRC6Mode0:
            pCirParam = &rC6Mode0Param;
            break;
        case BKIR_KirDevice_eCirRca:
            pCirParam = &rcaParam;
            break;
        case BKIR_KirDevice_eCirToshibaTC9012:
            pCirParam = &toshibaTC9012Param;
            break;
        default:
            return;
    }

    BKIR_P_WriteCirParam (hChn, 0, pCirParam->frameTimeout);

    ulData = (pCirParam->stop.tol << 10) | (pCirParam->stop.val);
    BKIR_P_WriteCirParam (hChn, 1, ulData);

    ulData = (pCirParam->pbCount << 3) | pCirParam->paCount;
    BKIR_P_WriteCirParam (hChn, 2, ulData);

    for (uli=0, ulj=3; uli<4; uli++) {
        ulData = ((pCirParam->pa)[uli].tol << 10) | (pCirParam->pa)[uli].val;
        BKIR_P_WriteCirParam (hChn, ulj++, ulData);
        ulData = ((pCirParam->pb)[uli].tol << 10) | (pCirParam->pb)[uli].val;
        BKIR_P_WriteCirParam (hChn, ulj++, ulData);
    }

    /* 6th bit needed 48 bit support for symbol A is in bit 11 for backward compatibility. */
    ulData = (pCirParam->nSymB << 5) | (pCirParam->nSymA & 0x1F) | ((pCirParam->nSymA >> 5) << 11);
    BKIR_P_WriteCirParam (hChn, 11, ulData);

    ulData = (pCirParam->symPulseWidth.tol << 10) | pCirParam->symPulseWidth.val;
    BKIR_P_WriteCirParam (hChn, 12, ulData);

    ulData = (pCirParam->spacingTol.tol << 10) | pCirParam->spacingTol.val;
    BKIR_P_WriteCirParam (hChn, 13, ulData);

    BKIR_P_WriteCirParam (hChn, 14, pCirParam->t0);
    BKIR_P_WriteCirParam (hChn, 15, pCirParam->delT);
    BKIR_P_WriteCirParam (hChn, 16, pCirParam->countDivisor);
    BKIR_P_WriteCirParam (hChn, 17, pCirParam->pulseTol);

    ulData = (pCirParam->varLenData << 11)
        | (pCirParam->chkStopSym << 10)
        | (pCirParam->twoSymPerCy << 9)
        | (pCirParam->biphaseCoded << 8)
        | (pCirParam->mostSignifSymRecvFirst << 7)
        | (pCirParam->leftAdjustRecvData << 6)
        | (pCirParam->pbRepeat << 5)
        | (pCirParam->measurePreamblePulse << 4)
        | (pCirParam->measureSymPulse << 3)
        | (pCirParam->fixSymPulseLast << 2)
        | (pCirParam->bitsPerSym & 3);
    BKIR_P_WriteCirParam (hChn, 18, ulData);

    BKIR_P_WriteCirParam (hChn, 19, pCirParam->timeoutDivisor);
    BKIR_P_WriteCirParam (hChn, 20, pCirParam->edgeTimeout);
    BKIR_P_WriteCirParam (hChn, 21, pCirParam->faultDeadTime);
    BKIR_P_WriteCirParam (hChn, 22, pCirParam->dataSymTimeout);

    BKIR_P_WriteCirParam(hChn, 23, pCirParam->repeatTimeout);

    ulData = (pCirParam->havePreambleAftStop << 5)
         | (pCirParam->enHavePreambleAftStop << 4)
         | (pCirParam->dataSymTimerExpStatEn << 3)
         | (pCirParam->ignoreDataSymTimerEdge1 << 2)
         | (pCirParam->stopParamUnit << 1)
         | (pCirParam->dataSymClkTickUnit);
    BKIR_P_WriteCirParam(hChn, 25, ulData);

    ulData = (pCirParam->restrictiveDecode << 10)
         | (pCirParam->passModeCustCodePass << 8)
         | (pCirParam->dontValidateMode << 7)
         | (pCirParam->dontValidateTrailer << 6)
         | (pCirParam->dontValidateCustCode << 5)
         | (pCirParam->modeBits << 2)
         | (pCirParam->trailer << 1)
         | (pCirParam->rc6);
    BKIR_P_WriteCirParam(hChn, 26, ulData);

    ulData = (pCirParam->nCustCodeBits << 16)
         | (pCirParam->custCode);
    BKIR_P_WriteCirParam(hChn, 27, ulData);
}

