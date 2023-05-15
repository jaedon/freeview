/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_tuner.c $
 * $brcm_Revision: Hydra_Software_Devel/129 $
 * $brcm_Date: 2/6/13 11:22a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_tuner.c $
 * 
 * Hydra_Software_Devel/129   2/6/13 11:22a ronchan
 * SWSATFE-126: add dynamic cap search after linear cap scan
 * 
 * Hydra_Software_Devel/128   12/18/12 9:44a ronchan
 * SWSATFE-126: fixed dynamic cap search lo step down condition when
 * linear search is enabled
 * 
 * Hydra_Software_Devel/127   11/14/12 10:50a ronchan
 * SW7346-426: removed support for input1 to tuner0 routing for B0
 * 
 * Hydra_Software_Devel/126   10/24/12 1:40p ckpark
 * SWSATFE-222: tuner LPF is optimized for low symbol rates
 * 
 * Hydra_Software_Devel/125   8/30/12 2:20p ronchan
 * SWSATFE-207: added config parameters for tuner agc window length,
 * amplitude threshold, and loop coefficient
 * 
 * Hydra_Software_Devel/124   8/10/12 3:43p ronchan
 * SWSATFE-75: disable both lna and pga in bypass mode
 * 
 * Hydra_Software_Devel/123   8/6/12 2:28p ronchan
 * SWSATFE-207: revert lna mode control to be channel-specific
 * 
 * Hydra_Software_Devel/122   7/30/12 11:39a ronchan
 * SWSATFE-75: disable lna but not pga in bypass mode
 * 
 * Hydra_Software_Devel/121   7/19/12 2:18p ronchan
 * SWSATFE-207: daisy gain controlled by tuner0
 * 
 * Hydra_Software_Devel/120   7/18/12 3:24p ronchan
 * SWSATFE-207: added daisy gain config param, reworked agc thresh config
 * param, lna mode controlled by tuner0
 * 
 * Hydra_Software_Devel/119   7/17/12 4:30p ronchan
 * SW7360-13: dco workaround only applies to 7344/7346 A0
 * 
 * Hydra_Software_Devel/118   7/5/12 11:41a ronchan
 * SWSATFE-205: enter InitChannels in isr context
 * 
 * Hydra_Software_Devel/117   7/3/12 11:24a ronchan
 * SWSATFE-205: enter tuner state machine in isr context
 * 
 * Hydra_Software_Devel/116   6/29/12 11:34a ronchan
 * SW7360-13: extend tuning range down to 250MHz
 * 
 * Hydra_Software_Devel/115   4/10/12 3:16p ronchan
 * SWSATFE-75: avoid disabling pga in lna bypass mode
 * 
 * Hydra_Software_Devel/114   3/22/12 2:06p enavarro
 * SWSATFE-184: added tuner_rfagc_thresh config parameter
 * 
 * Hydra_Software_Devel/113   3/21/12 10:55a ckpark
 * SWSATFE-182: reset clkmux and ADC in tuner_init script
 * 
 * Hydra_Software_Devel/112   3/14/12 8:48a ronchan
 * SWSATFE-139: use BAST_KVCO_LO_THRESH in condition to increase kvco_cntl
 * in kvco calibration
 * 
 * Hydra_Software_Devel/111   2/13/12 11:34a ckpark
 * SWSATFE-140: for manual filter setting, use calculated filter settings
 * for dft then switch to manual setting during tracking
 * 
 * Hydra_Software_Devel/110   11/7/11 4:36p ronchan
 * SWSATFE-140: initialize ref pll if controlled by current channel
 * 
 * Hydra_Software_Devel/109   10/28/11 2:02p ronchan
 * SW7346-524: fixed coverity defect for unchecked return values
 * 
 * Hydra_Software_Devel/108   10/10/11 11:31a ronchan
 * SW7346-426: removed daisy output and tuner1 path for B0
 * 
 * Hydra_Software_Devel/107   10/7/11 10:57a enavarro
 * SWSATFE-150: fixed bug in BAST_g3_P_TunerGetActualLOFreq()
 * 
 * Hydra_Software_Devel/106   10/7/11 10:35a enavarro
 * SWSATFE-150: added BAST_g3_P_TunerGetActualLOFreq()
 * 
 * Hydra_Software_Devel/105   10/5/11 5:35p ronchan
 * SWSATFE-75: do not program Vregctrl in MXFGA_R02 for A1
 * 
 * Hydra_Software_Devel/104   9/29/11 10:03a ronchan
 * SWSATFE-139: select higher kvco_cntl if kvco calibration oscillating
 * 
 * Hydra_Software_Devel/103   9/20/11 6:30p ronchan
 * SWSATFE-139: shift down kvco range, check for maxed out Vc, add popcap
 * voting in Vc search
 * 
 * Hydra_Software_Devel/102   9/20/11 2:12p ronchan
 * SWSATFE-139: shifted cap boundaries of kvco_cntl table lookup
 * 
 * Hydra_Software_Devel/101   9/19/11 3:00p ronchan
 * SWSATFE-139: added safety counter for kvco calibration
 * 
 * Hydra_Software_Devel/100   9/16/11 9:58a ronchan
 * SWSATFE-139: add configurable ambient temp for Vc adjustment
 * 
 * Hydra_Software_Devel/99   9/15/11 2:07p ronchan
 * SWSATFE-139: added temperature adjustment for Vc thresholds
 * 
 * Hydra_Software_Devel/98   9/13/11 5:37p ronchan
 * SWSATFE-139: implement kvco calibration during initialization
 * 
 * Hydra_Software_Devel/97   8/25/11 10:41a ronchan
 * SWSATFE-139: fixed compile warnings
 * 
 * Hydra_Software_Devel/96   8/25/11 10:02a ronchan
 * SWSATFE-139: do Vcref temperature calibration on 7346 for now
 * 
 * Hydra_Software_Devel/95   8/24/11 3:53p ronchan
 * SWSATFE-139: add call to read avs temperature
 * 
 * Hydra_Software_Devel/94   8/12/11 9:37a ronchan
 * SWSATFE-139: binary search up to second LSB if linear search on
 * 
 * Hydra_Software_Devel/93   8/10/11 11:37a ronchan
 * SWSATFE-139: implement linear cap search
 * 
 * Hydra_Software_Devel/92   8/9/11 10:58a ronchan
 * SW7346-380: fix coverity defect 32716
 * 
 * Hydra_Software_Devel/91   8/2/11 5:42p ronchan
 * SW7346-390: fix coverity defect 32701
 * 
 * Hydra_Software_Devel/90   8/2/11 11:55a ronchan
 * SW7346-383: fixed coverity defect 32703
 * 
 * Hydra_Software_Devel/89   7/29/11 1:57p ronchan
 * SW7344-117: increase delay after refpll initialization
 * 
 * Hydra_Software_Devel/88   7/22/11 10:59a enavarro
 * SWSATFE-75: enable LNA AGC in BAST_g3_P_TunerInit()
 * 
 * Hydra_Software_Devel/87   7/18/11 1:50p ronchan
 * SWSATFE-75: re-enabled DWA mode
 * 
 * Hydra_Software_Devel/86   7/14/11 4:30p ronchan
 * SWSATFE-75: fixed compile warning
 * 
 * Hydra_Software_Devel/85   7/14/11 4:25p ronchan
 * SWSATFE-75: disable dwa mode for dco, lower dco bandwidth
 * 
 * Hydra_Software_Devel/84   7/11/11 11:50a ckpark
 * SWSATFE-75: DCO unfreeze timer is set to 20msec
 * 
 * Hydra_Software_Devel/83   7/8/11 6:07p ronchan
 * SWSATFE-75: fixed compile warning
 * 
 * Hydra_Software_Devel/82   7/8/11 10:15a ronchan
 * SWSATFE-75: added timer select parameter for running dco loop
 * 
 * Hydra_Software_Devel/81   7/7/11 10:44a ronchan
 * SWSATFE-75: added runtime parameter for dco loop
 * 
 * Hydra_Software_Devel/80   7/6/11 6:01p ronchan
 * SWSATFE-75: toggle tuner interface reset in reset sequence
 * 
 * Hydra_Software_Devel/79   7/6/11 11:22a ronchan
 * SWSATFE-75: run and freeze dco in dwa mode for 7358
 * 
 * Hydra_Software_Devel/78   7/6/11 10:43a ronchan
 * SWSATFE-75: implemented DCO serial interface reset sequence
 * 
 * Hydra_Software_Devel/77   6/27/11 3:05p ronchan
 * SWSATFE-126: removed hw auto tune count debug
 * 
 * Hydra_Software_Devel/76   6/27/11 3:04p ronchan
 * SWSATFE-126: added error message for dco indirect timeout
 * 
 * Hydra_Software_Devel/75   6/27/11 11:43a ronchan
 * SWSATFE-126: try hw auto-tune first before popcap search
 * 
 * Hydra_Software_Devel/74   6/22/11 5:29p ronchan
 * SWSATFE-126: removed popcap debugs
 * 
 * Hydra_Software_Devel/73   6/22/11 5:25p ronchan
 * SWSATFE-126: check for mixer pll status toggling, check status when
 * doing hw auto-tune
 * 
 * Hydra_Software_Devel/72   6/21/11 8:14p ronchan
 * SWSATFE-126: step up lo parameters if minimum cap
 * 
 * Hydra_Software_Devel/71   6/21/11 12:10p ronchan
 * SWSATFE-126: implemented dynamic cap search algorithm, dco done bit
 * workaround
 * 
 * Hydra_Software_Devel/70   6/18/11 3:22p ronchan
 * SWSATFE-126: additional adjustments lo freq ranges per request
 * 
 * Hydra_Software_Devel/69   6/13/11 11:20a ronchan
 * SWSATFE-126: adjusted lo freq ranges per tuner team request
 * 
 * Hydra_Software_Devel/68   5/11/11 3:23p ronchan
 * SWSATFE-75: program Vregctrl in MXFGA_R02 for B0
 * 
 * Hydra_Software_Devel/67   4/27/11 9:56a ckpark
 * SWSATFE-75: fixed build warning about not used variable
 * 
 * Hydra_Software_Devel/66   4/25/11 5:19p ckpark
 * SWSATFE-75: LPF is narrowed when FLIP stepping, DFT debug message
 * changed to ERR.
 * 
 * Hydra_Software_Devel/65   4/23/11 3:06p ronchan
 * SWSATFE-75: enable DCOs for 7358
 * 
 * Hydra_Software_Devel/64   4/22/11 3:16p ckpark
 * SWSATFE-75: commented out DEBUG_TEST_TUNER
 * 
 * Hydra_Software_Devel/63   4/14/11 5:45p ronchan
 * SWSATFE-75: fix compile warning for indirect access
 * 
 * Hydra_Software_Devel/62   4/7/11 11:46a ronchan
 * SWSATFE-111: update thresholds for LNA and BB AGC
 * 
 * Hydra_Software_Devel/61   4/5/11 2:42p ckpark
 * SWSATFE-75: replaced "//" comments
 * 
 * Hydra_Software_Devel/60   4/5/11 10:52a ckpark
 * SWSATFE-102: normalized tuner calibration routine is adjusted
 * 
 * Hydra_Software_Devel/59   4/3/11 1:44p ronchan
 * SWSATFE-102: implemented normalized tuner calibration
 * 
 * Hydra_Software_Devel/58   3/30/11 11:23a ronchan
 * SWSATFE-75: fix compile warning
 * 
 * Hydra_Software_Devel/57   3/8/11 2:16p ronchan
 * SWSATFE-75: check dependencies before powering down RFFE
 * 
 * Hydra_Software_Devel/56   3/4/11 5:16p ronchan
 * SWSATFE-75: bypass RFFE powerdown
 * 
 * Hydra_Software_Devel/55   2/25/11 1:09p ronchan
 * SWSATFE-75: bypass daisy programming during initialization
 * 
 * Hydra_Software_Devel/54   2/25/11 11:42a ronchan
 * SWSATFE-75: remove option for 1080MHz refpll
 * 
 * Hydra_Software_Devel/53   2/25/11 11:37a ronchan
 * SWSATFE-75: implemented tuner power management
 * 
 * Hydra_Software_Devel/52   2/17/11 10:04a ronchan
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/51   2/15/11 11:01a ronchan
 * SWSATFE-75: call ConfigAgc() after tuning
 * 
 * Hydra_Software_Devel/50   2/10/11 5:19p ronchan
 * SWSATFE-75: max bias current for RFPGA, adjusted predco gain
 * 
 * Hydra_Software_Devel/49   2/10/11 2:19p ronchan
 * SWSATFE-75: power down daisy input and output as default
 * 
 * Hydra_Software_Devel/48   2/10/11 2:00p ckpark
 * SWSATFE-102: LPF cut-off calculation is adjusted
 * 
 * Hydra_Software_Devel/47   2/9/11 7:55p ckpark
 * SWSATFE-102: caldac powerup moved to TUNER_PWRUP_R01
 * 
 * Hydra_Software_Devel/46   2/4/11 11:12a ckpark
 * SWSATFE-102: LPF cut-off calculation is adjusted
 * 
 * Hydra_Software_Devel/45   2/3/11 3:38p ckpark
 * SWSATFE-75: fixed bug when setting LPF
 * 
 * Hydra_Software_Devel/44   1/21/11 10:19a ronchan
 * SWSATFE-75: moved ddfs reset to after refpll lock
 * 
 * Hydra_Software_Devel/43   1/20/11 4:48p ronchan
 * SWSATFE-75: fixed ddfs reset before programming fcw
 * 
 * Hydra_Software_Devel/42   1/18/11 3:06p ronchan
 * SWSATFE-75: additional debugs when tuner not locked
 * 
 * Hydra_Software_Devel/41   12/29/10 12:08p enavarro
 * SWSATFE-75: added BAST_DEBUG_TUNER() macro
 * 
 * Hydra_Software_Devel/40   12/28/10 4:05p enavarro
 * SWSATFE-75: widen lpf for signal detect mode
 * 
 * Hydra_Software_Devel/39   12/27/10 11:50a enavarro
 * SWSATFE-75: added log trace for tune done
 * 
 * Hydra_Software_Devel/38   12/23/10 4:13p enavarro
 * SWSATFE-75: added test option for 1080MHz refpll
 * 
 * Hydra_Software_Devel/37   12/22/10 11:23a enavarro
 * SWSATFE-75: compile BAST_g3_P_TunerToggleDcoClock() only for A0; fixed
 * complier warnings
 * 
 * Hydra_Software_Devel/36   12/21/10 4:40p enavarro
 * SWSATFE-75: increased delay values in mixpll auto tune
 * 
 * Hydra_Software_Devel/35   12/21/10 3:02p enavarro
 * SWSATFE-75: added mixpll tuning with SM; removed calculation involving
 * float
 * 
 * Hydra_Software_Devel/34   12/16/10 2:46p ronchan
 * SWSATFE-75: fixed comment for cap_cntl programming
 * 
 * Hydra_Software_Devel/33   12/15/10 7:20p ronchan
 * SWSATFE-75: no need to program cap_cntl bit 0 on LO_R03
 * 
 * Hydra_Software_Devel/32   12/15/10 6:52p enavarro
 * SWSATFE-75: set AFE data format to 2's complement
 * 
 * Hydra_Software_Devel/31   12/15/10 5:19p ronchan
 * SWSATFE-75: bypass DCO freeze since failure 2nd time around
 * 
 * Hydra_Software_Devel/30   12/15/10 5:10p ronchan
 * SWSATFE-75: optimize DCO clocking workaround
 * 
 * Hydra_Software_Devel/29   12/15/10 4:02p ronchan
 * SWSATFE-75: fixed error in lodiv calculations
 * 
 * Hydra_Software_Devel/28   12/13/10 10:07p ronchan
 * SWSATFE-75: round fcw calculation
 * 
 * Hydra_Software_Devel/27   12/13/10 3:49p ronchan
 * SWSATFE-75: fixed refpll powerdown bits
 * 
 * Hydra_Software_Devel/26   12/12/10 4:09p ronchan
 * SWSATFE-75: widen filter for flif stepping, turn on testport for debug
 * 
 * Hydra_Software_Devel/25   12/11/10 5:45p ronchan
 * SWSATFE-75: fixed refpll init and capcntl format
 * 
 * Hydra_Software_Devel/24   12/10/10 5:05p ronchan
 * SWSATFE-95: power up crossbar
 * 
 * Hydra_Software_Devel/23   12/10/10 9:13a enavarro
 * SWSATFE-75: replaced "//" comments
 * 
 * Hydra_Software_Devel/22   12/9/10 5:24p ronchan
 * SWSATFE-95: added functions to control tuner LNA
 * 
 * Hydra_Software_Devel/21   10/13/10 2:47p ronchan
 * SWSATFE-75: fixed bug in DCO convergence routine
 * 
 * Hydra_Software_Devel/20   10/12/10 3:32p enavarro
 * SWSATFE-75: added debug message for emulation
 * 
 * Hydra_Software_Devel/19   10/12/10 9:53a ronchan
 * SWSATFE-75: return success for tuner power up for initialization
 * 
 * Hydra_Software_Devel/18   10/11/10 11:35a ronchan
 * SWSATFE-75: added abs macro
 * 
 * Hydra_Software_Devel/17   10/11/10 10:51a ronchan
 * SWSATFE-75: use eBaudUsec instead of eBaud timer
 * 
 * Hydra_Software_Devel/16   10/8/10 5:17p ronchan
 * SWSATFE-75: implemented quick tune and dco convergence
 * 
 * Hydra_Software_Devel/15   10/6/10 4:45p ronchan
 * SWSATFE-75: fixed point calculations in TunerSetFilter
 * 
 * Hydra_Software_Devel/14   10/6/10 11:27a ronchan
 * SWSATFE-75: implemented tuner filter calibration algorithm
 * 
 * Hydra_Software_Devel/13   10/4/10 5:22p ronchan
 * SWSATFE-75: reworked tuner init, added support functions for filter
 * calibration
 * 
 * Hydra_Software_Devel/12   9/29/10 5:36p ronchan
 * SWSATFE-75: implemented FGA and LPF filter codeword lookup
 * 
 * Hydra_Software_Devel/11   9/28/10 6:35p ronchan
 * SWSATFE-75: implemented capacitor binary scan
 * 
 * Hydra_Software_Devel/10   9/27/10 5:28p ronchan
 * SWSATFE-75: implemented TunerSetLoParams() function
 * 
 * Hydra_Software_Devel/9   9/27/10 3:48p ronchan
 * SWSATFE-75: implemented tuner init and indirect register access
 * 
 * Hydra_Software_Devel/8   9/22/10 5:19p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/7   9/21/10 10:32a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/6   9/17/10 5:28p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/5   9/15/10 4:36p enavarro
 * SWSATFE-75: added tuner indirect register access functions
 * 
 * Hydra_Software_Devel/4   9/15/10 10:30a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/3   9/14/10 2:29p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/2   9/8/10 10:34a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   8/31/10 1:55p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

BDBG_MODULE(bast_g3_priv_tuner);


#define BAST_DEBUG_TUNER(x) /* x */
#define BAST_DEBUG_CAP_SEARCH(x) /* x */
#define BAST_DEBUG_KVCO_CAL(x) /* x */

#define BAST_TUNER_LO_TABLE_SIZE 8
#define BAST_TUNER_CAP_CNTL_TABLE_SIZE 5
#define BAST_TUNER_KVCO_CAL_TABLE_SIZE 8

/* #define BAST_TUNER_BYPASS_AGC */
/* #define BAST_TUNER_BYPASS_IFAGC */
/* #define BAST_TUNER_BYPASS_CALIBRATE_LPF  */
/* #define BAST_ENABLE_HW_AUTO_TUNE */
#define BAST_NORMALIZED_FILTER_CAL

#define abs(x) ((x)<0?-(x):(x))

/* local routines */
BERR_Code BAST_g3_P_TunerInit1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerVcSearch(BAST_ChannelHandle h, uint32_t *Vc);
BERR_Code BAST_g3_P_TunerCalibrateKvco(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerCalibrateKvco1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerGetFddfs(BAST_ChannelHandle h, uint32_t *Fddfs);
BERR_Code BAST_g3_P_TunerSetLoParams(BAST_ChannelHandle h, int8_t step);
BERR_Code BAST_g3_P_TunerUpdateActualTunerFreq(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerGetLoDivider(BAST_ChannelHandle h, uint32_t *tunerLoDivider);
BERR_Code BAST_g3_P_TunerSetFcw(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerAutoTune(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerBinaryCapScan(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerLinearCapScan(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerSetCapCntl(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerSetCapCntlLoopParams(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerSetFreq1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerGetCutoff(BAST_ChannelHandle h, uint8_t *lpfCal);
BERR_Code BAST_g3_P_TunerWaitForDcoConverge(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerSetFreq2(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerCalFilter(BAST_ChannelHandle h, uint8_t cutoff);
BERR_Code BAST_g3_P_TunerSetFgaLpf(BAST_ChannelHandle h, BAST_TunerCalType calType, uint8_t bwCode);
BERR_Code BAST_g3_P_TunerCalFgaLpf(BAST_ChannelHandle h, BAST_TunerCalType calType, uint8_t cutoff, uint8_t *calBw);
BERR_Code BAST_g3_P_TunerSetTestTone(BAST_ChannelHandle h, uint8_t mhz);
BERR_Code BAST_g3_P_TunerGetAvgPower(BAST_ChannelHandle h, uint32_t *avg);
BERR_Code BAST_g3_P_TunerQuickTune1(BAST_ChannelHandle h);

#if ((BCHP_CHIP == 7344) || (BCHP_CHIP == 7346)) && (BCHP_VER == BCHP_VER_A0)
BERR_Code BAST_g3_P_TunerToggleDcoClock(BAST_ChannelHandle h);
#endif


static const uint32_t BAST_Tuner_IndirectRegBase[6] = 
{
   BCHP_SDS_TUNER_RFAGC_R02,
   BCHP_SDS_TUNER_BBAGC_R02,
   BCHP_SDS_TUNER_PREDCOI_R01,
   BCHP_SDS_TUNER_PREDCOQ_R01,
   BCHP_SDS_TUNER_POSTDCOI_R01,
   BCHP_SDS_TUNER_POSTDCOQ_R01
};

static const uint8_t tuner_fga_bw[16] = 
{
   60, 57, 54, 52, 49, 47, 45, 43,
   41, 39, 37, 35, 33, 31, 29, 27
};

static const uint8_t tuner_lpf_bw[36] = 
{
   3, 4, 6, 7, 8, 10, 11, 12, 13,
   14, 15, 17, 18, 19, 21, 22, 23, 24,
   26, 28, 29, 30, 32, 33, 35, 36, 37,
   38, 40, 42, 43, 44, 46, 47, 48, 49
};


uint16_t tuner_kvco_cal_capcntl_table[BAST_TUNER_KVCO_CAL_TABLE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t tuner_kvco_cal_kvcocntl_table[BAST_TUNER_KVCO_CAL_TABLE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};


/* shared private functions */
/******************************************************************************
 BAST_g3_P_TunerInit() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TunerInit(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   
   static const uint32_t script_tuner_init[] =
   {
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_RESET_R01, 0x00000001),       /* serial interface reset */
      BAST_SCRIPT_AND(BCHP_SDS_TUNER_RESET_R01, ~0x00000001),     /* release serial reset */
      BAST_SCRIPT_AND(BCHP_SDS_TUNER_PREDCO_R01, ~0x00000200),    /* rst_n to preDCO block */
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_PREDCO_R01, 0x00000200),      /* release preDCO rst_n */
      BAST_SCRIPT_AND(BCHP_SDS_TUNER_POSTDCO_R01, ~0x00000200),   /* rst_n to postDCO block */
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_POSTDCO_R01, 0x00000200),     /* release postDCO rst_n */
      BAST_SCRIPT_OR(BCHP_SDS_GR_BRIDGE_SW_INIT_0, 0x00000004),   /* assert tuner IF reset */
      BAST_SCRIPT_AND(BCHP_SDS_GR_BRIDGE_SW_INIT_0, ~0x00000004), /* deassert tuner IF reset */      

      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_LPF_R01, 0x00002768),      /* set LPF 30MHz */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_MXFGA_R01, 0x3CAAAAAA),    /* set FGA 30MHz */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_IFPGA_R01, 0x00000088),    /* enable DCO feedback, IFpga current 100uA */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_RFPGA_R01, 0x0000003C),    /* 1GHz bw, max bias current */
      
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_LNA_R01, 0xC4963FFE),      /* improved LNA performance */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_LNA_R02, 0x00007A63),      /* enable LNA mode */
      
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_PREDCO_R01, 0x00212A28),   /* 50MHz predco clk div, reset clk div, 1.0 filter gain */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_POSTDCO_R01, 0x00212A24),  /* 50MHz postdco clk div, reset clk div, 0.33 filter gain */
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_PREDCO_R01, 0x00008000),      /* release clk div reset */
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_POSTDCO_R01, 0x00008000),     /* release clk div reset */
      
      BAST_SCRIPT_AND(BCHP_SDS_TUNER_ADC_R02, ~0x00420000),       /* clk not div by 2, data format binary */
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_ADC_R03, 0x0000000E),         /* use external clk, power down D2S */
            
      BAST_SCRIPT_AND(BCHP_SDS_TUNER_ADC_R04, ~0xC0000000),       /* toggle clkmux and ADC resets */      
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_ADC_R04, 0x40000000),    
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_ADC_R04, 0x80000000),  
      
      BAST_SCRIPT_EXIT
   };
   
   /* initialize tuner */
   retCode = BAST_g3_P_ProcessScript(h, script_tuner_init);
   if (retCode != BERR_SUCCESS)
      return retCode;
   
   hChn->postTuneFunct = nextFunct;
   hChn->count1 = 0;
   hChn->funct_state = 0;
   
   return BAST_g3_P_TunerInit1(h);
}


/******************************************************************************
 BAST_g3_P_TunerPowerUp()
******************************************************************************/
BERR_Code BAST_g3_P_TunerPowerUp(BAST_ChannelHandle h)
{
   BAST_ChannelHandle hChn0 = (BAST_ChannelHandle)(h->pDevice->pChannels[0]);
   
   /* power up shared components */
   BAST_g3_P_OrRegister(hChn0, BCHP_SDS_TUNER_PWRUP_COMMON_R01, 0x0000000F);
   
   /* power up tuner sub-blocks */
   BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_PWRUP_R01, 0x007D7FDF);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerPowerDown()
******************************************************************************/
BERR_Code BAST_g3_P_TunerPowerDown(BAST_ChannelHandle h)
{
   BAST_ChannelHandle hChn0 = (BAST_ChannelHandle)(h->pDevice->pChannels[0]);
   BAST_g3_P_ChannelHandle *hChn0Impl = (BAST_g3_P_ChannelHandle *)(hChn0->pImpl);
   BAST_TunerLnaOutputConfig inputChannel;
   bool bAllChannelsPoweredDown = true;
   BAST_ChannelHandle hChannel;
   uint8_t i;
   
   /* power down tuner sub-blocks */
   BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PWRUP_R01, ~0x003FFFFF);
   
   if (h->channel == 0)
      inputChannel = BAST_TunerLnaOutputConfig_eIn0;
   else
      inputChannel = BAST_TunerLnaOutputConfig_eIn1;
   
   /* check dependency on crossbar input */
   if ((hChn0Impl->tunerLnaSettings.out0 != inputChannel) && 
      (hChn0Impl->tunerLnaSettings.out1 != inputChannel) &&
      (hChn0Impl->tunerLnaSettings.daisy != inputChannel))
   {
      /* power down RFFE if unused */
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PWRUP_R01, ~0x00400000);
   }
   
   /* check if all channels powered down */
   for (i = 0; i < h->pDevice->totalChannels; i++)
   {
      hChannel = (BAST_ChannelHandle)(h->pDevice->pChannels[i]);
      if ((((BAST_g3_P_ChannelHandle *)(hChannel->pImpl))->coresPoweredDown & BAST_CORE_SDS) == 0)
         bAllChannelsPoweredDown = false;
   }
   
   /* power down shared components if all channels powered down */
   if (bAllChannelsPoweredDown)
   {
      /* global power down bandgap, postdiv, refpll */
      BAST_g3_P_AndRegister(hChn0, BCHP_SDS_TUNER_PWRUP_COMMON_R01, ~0x0000000F);
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerQuickTune()
******************************************************************************/
BERR_Code BAST_g3_P_TunerQuickTune(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

#ifdef BAST_TUNE_MIX_DEBUG
   BDBG_MSG(("in BAST_g3_P_TunerQuickTune()"));
#endif

   hChn->postTuneFunct = nextFunct;
   
   hChn->funct_state = 0;
   return BAST_g3_P_TunerQuickTune1(h);
}


/******************************************************************************
 BAST_g3_P_TunerSetFreq()
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetFreq(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_ChannelHandle hChn0 = (BAST_ChannelHandle)(h->pDevice->pChannels[0]);
   BAST_g3_P_ChannelHandle *hChn0Impl = (BAST_g3_P_ChannelHandle *)(hChn0->pImpl);
   uint32_t val;
   
   hChn->postTuneFunct = nextFunct;
   
   /* set acq state to tuning */
   if (hChn->acqState == BAST_AcqState_eIdle)
      hChn->acqState = BAST_AcqState_eTuning;
   
   /* select LNA mode */
   if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_LNA_BYPASS)
   {
      BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_LNA_R01, 0x08000000);      /* LNA bypass mode */
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_LNA_R02, ~0x00000440);    /* disable both LNA and PGA */
   }
   else
   {
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_LNA_R01, ~0x08000000);    /* unbypass LNA */
      if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_LNA_PGA_MODE)
         BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LNA_R02, ~0x0000440, 0x00000401);  /* enable PGA, disable LNA */
      else
         BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LNA_R02, ~0x0000440, 0x00000041);  /* enable LNA, disable PGA */
   }
   
   /* configure daisy output gain, note daisy controlled by tuner0 */
   BAST_g3_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_TUNER_DAISY_R01, ~0x000C000, hChn0Impl->tunerDaisyGain << 14);
   
   /* remember full tune freq */
   BAST_g3_P_TunerUpdateActualTunerFreq(h);
   hChn->fullTuneFreq = hChn->actualTunerFreq;
   
   /* set lo parameters */
   BAST_g3_P_TunerSetLoParams(h, 0);
   
   /* program FCW */
   BAST_g3_P_TunerGetLoDivider(h, &(hChn->tunerLoDivider));
   BAST_g3_P_TunerSetFcw(h);
   
   BAST_g3_P_ToggleBit(h, BCHP_SDS_TUNER_LO_R03, 0x00003000);  /* reset div23, cml */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_TUNER_LO_R02, 0x80000000);  /* reset IQ gen */
   
   /* set IQ gen filter bandwidth */
   if (hChn->actualTunerFreq > 2700000000UL)
      val = 0x1800;
   else if (hChn->actualTunerFreq > 2300000000UL)
      val = 0x0800;
   else
      val = 0;
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R04, ~0x00003800, val);
   
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R03, ~0xF8000000, 0xC8000000);  /* comparator Vc threshold */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R02, ~0x0000F800, 0x00002800);  /* QP bias control */
   
   /* start capacitor scan */
   hChn->funct_state = 0;
   hChn->bCalibrateKvco = false;
   hChn->bOverrideKvco = false;
   return BAST_g3_P_TunerBinaryCapScan(h);
}


/******************************************************************************
 BAST_g3_P_TunerSetFilter()
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetFilter(BAST_ChannelHandle h, bool bTracking)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#if 0   
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl; 
#endif   
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
   uint8_t tunerLpfWide = 0;
   
   if (bTracking)
   {
      /* do not calibrate when tracking, use pre-cal values */
      if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_SET_FILTER_MANUAL)
      {
         BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eFga, hChn->tunerFgaCalManual);
         BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eLpf, hChn->tunerLpfCalManual);      
      }
      else
      {
         BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eFga, hChn->tunerFgaCal);
         BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eLpf, hChn->tunerLpfCal);
      }
   }
   else
   {
      if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_TUNER_TEST_MODE) == 0)
      {
         /* set LPF wide setting to 10% of nyquist */
         if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_NYQUIST_20)
            tunerLpfWide = 20;   /* 0.02 * 10^3 */
         else
            tunerLpfWide = 35;   /* 0.035 * 10^3 */
         
         BAST_MultU32U32(tunerLpfWide * 2, hChn->acqParams.symbolRate, &P_hi, &P_lo);
         BAST_DivU64U32(P_hi, P_lo, 1000000000, &Q_hi, &Q_lo);
      
#if 0      
         /* for FLIF stepping frequency estimate */
         Q_lo += (2 * hDev->searchRange) / 1000000;     
         tunerLpfWide = (uint8_t)(Q_lo + 1) >> 1;  /* round */
#else
         tunerLpfWide = (uint8_t)Q_lo;
#endif         
         
         BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eFga, hChn->tunerFgaCal);
         BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eLpf, hChn->tunerLpfCal + tunerLpfWide);
      }
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerGetLockStatus()
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetLockStatus(BAST_ChannelHandle h, bool *bRefPllLocked, bool *bMixPllLocked)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_ChannelHandle hChnRefPll = (BAST_ChannelHandle)(h->pDevice->pChannels[hChn->tunerRefPllChannel]);
   uint32_t val;
   uint8_t i;
   
   /* check ref pll status from appropriate tuner */
   BAST_g3_P_ReadRegister(hChnRefPll, BCHP_SDS_TUNER_PLLSTAT_R01, &val);
   if (val & 0x2)
      *bRefPllLocked = true;
   else
      *bRefPllLocked = false;
   
   /* check mix pll status */
   *bMixPllLocked = true;
   for (i = 0; i < 50; i++)
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_PLLSTAT_R01, &val);
      if ((val & 0x8) == 0)
      {
         *bMixPllLocked = false;
         break;
      }
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerIndirectWrite()
******************************************************************************/
BERR_Code BAST_g3_P_TunerIndirectWrite(BAST_ChannelHandle h, BAST_TunerIndirectRegGroup type, uint8_t addr, uint32_t val)
{
   uint32_t regBase, status;
   uint16_t count = 0;
   
   if (type > BAST_TunerIndirectRegGroup_ePostDcoQ)
      return BERR_INVALID_PARAMETER;
   
   regBase = BAST_Tuner_IndirectRegBase[type];
   
   if (type <= BAST_TunerIndirectRegGroup_eBbagc)
   {
      /* write data */
      BAST_g3_P_WriteRegister(h, regBase + 0x4, &val);
      
      /* set write address for AGC indirect, write enable */
      BAST_g3_P_ReadModifyWriteRegister(h, regBase, ~0x018000FF, (0x00800000 | addr)); /* write enable */
      BAST_g3_P_AndRegister(h, regBase, ~0x01800000); /* disable write */
   }
   else
   {
      /* write data for DCO indirect */
      BAST_g3_P_WriteRegister(h, regBase + 0x8, &val);
      
      /* write enable, set write address */
      BAST_g3_P_ReadModifyWriteRegister(h, regBase, ~0x00000307, 0x00000200 | (addr & 0x7));
      
      /* wait for write done */
      do {
         BAST_g3_P_ReadRegister(h, regBase + 0x4, &status);
         count++;
      }
      while (((status & 0x2) == 0) && (count <= 999));
      
      if (count > 999)
      {
         BDBG_MSG(("DCO indirect write timeout @ 0x%08X", regBase + 0x4));
      }
      
      /* disable write */
      BAST_g3_P_AndRegister(h, regBase, ~0x00000300);
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerIndirectRead()
******************************************************************************/
BERR_Code BAST_g3_P_TunerIndirectRead(BAST_ChannelHandle h, BAST_TunerIndirectRegGroup type, uint8_t addr, uint32_t *pVal)
{
   uint32_t regBase, status;
   uint16_t count = 0;
   
   if (type > BAST_TunerIndirectRegGroup_ePostDcoQ)
      return BERR_INVALID_PARAMETER;
   
   regBase = BAST_Tuner_IndirectRegBase[type];
   
   if (type <= BAST_TunerIndirectRegGroup_eBbagc)
   {
      /* set read address for AGC indirect, read enable */
      BAST_g3_P_ReadModifyWriteRegister(h, regBase, ~0x0180FF00, (0x01000000 | (addr << 8)));
      
      /* read data */
      BAST_g3_P_ReadRegister(h, regBase + 0x8, pVal);
      
      /* disable read */
      BAST_g3_P_AndRegister(h, regBase, ~0x01800000);
   }
   else
   {
      /* read enable, set read address for DCO indirect */
      BAST_g3_P_ReadModifyWriteRegister(h, regBase, ~0x00000307, 0x00000100 | (addr & 0x7));
      
      /* wait for read done */
      do {
         BAST_g3_P_ReadRegister(h, regBase + 0x4, &status);
         count++;
      }
      while (((status & 0x1) == 0) && (count <= 999));
      
      if (count > 999)
      {
         BDBG_MSG(("DCO indirect read timeout @ 0x%08X", regBase + 0x4));
      }
      
      /* read data */
      BAST_g3_P_ReadRegister(h, regBase + 0xC, pVal);
      
      /* disable read */
      BAST_g3_P_AndRegister(h, regBase, ~0x00000300);
   }
   
   return BERR_SUCCESS;
}


/* local functions */
/******************************************************************************
 BAST_g3_P_TunerInit1() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TunerInit1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   bool bRefPllLocked, bMixPllLocked;
   BERR_Code retCode;
   uint32_t val;
   
   static const uint32_t script_ref_pll_init[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_REFPLL_R01, 0x2D3E02B0),   /* div_fb N=22, turn on hysterisis in VCO level shifter, loop filter resistance, set charge pump current, set loop filter dac to ground */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_REFPLL_R02, 0x3FF01232),   /* div_ref M=1, reset lock detector, reset div_ref, reset div_fb, resets loop filter voltage to 0V */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_REFPLL_R03, 0x02060206),   /* program post-divider 3 to 6 */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_REFPLL_R04, 0x00000FFF),   /* enable clock outputs, reset post dividers */
      
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_REFPLL_R02, 0x3FF003A2),   /* disable lock detector reset, release div_ref reset, release div_fb reset, disconnect loop filter from loop-filter dac */
      BAST_SCRIPT_AND_OR(BCHP_SDS_TUNER_REFPLL_R01, ~0x00006000, 0x00002000), /* sets loop filter dac to 2k to minimize leakage */
      BAST_SCRIPT_EXIT
   };
   
   while (1)
   {
      /* BDBG_MSG(("BAST_g3_P_TunerInit1(): funct_state=%d", hChn->funct_state)); */
      switch (hChn->funct_state)
      {
         case 0:
            /* initialize ref pll if controlled by current channel */
            if (hChn->tunerRefPllChannel == h->channel)
            {
               BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_ref_pll_init));
            }
            hChn->funct_state = 1;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 500, BAST_g3_P_TunerInit1);
            
         case 1:
            /* release post dividers reset */
            if (hChn->tunerRefPllChannel == h->channel)
            {
               BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_REFPLL_R04, ~0x0000000F);
            }
            
            /* check refpll status */
            BAST_g3_P_TunerGetLockStatus(h, &bRefPllLocked, &bMixPllLocked);
            if (bRefPllLocked)
               hChn->funct_state = 2;
            else if (hChn->count1 < 50)
            {
               /* retry refpll init */
               BDBG_WRN(("BAST_g3_P_TunerInit1(): refpll NOT locked"));   
               hChn->count1++;
               hChn->funct_state = 0;
            }
            else
            {
               /* return error after max retries */
               BERR_TRACE(retCode = BAST_ERR_TUNER_FAIL);
               BDBG_ERR(("unable to lock refpll!"));
               goto done;
            }
            break;
            
         case 2:
            /* ref pll locked */
            BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PWRUP_R01, ~0x00000080);     /* reset ddfs */
            BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_PWRUP_R01, 0x00000080);
            
            BAST_CHK_RETCODE(BAST_g3_P_TunerGetFddfs(h, &(hChn->tunerFddfs)));   /* get Fddfs frequency */
            
            /* reset and enable LNA AGC */
            val = ((hChn->tunerAgcAmpThresh & 0x001F) << 13) | 0x00000050;    /* set amplitude threshold, reset */
            BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_RFAGC_R01, &val);
            BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_RFAGC_R01, ~0x00000010);  /* release reset */
            
            /* initialize LNA AGC settings */
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x0, 0x0000401A);   /* invert k, bypass sign function and gray stepper, bb_rf_out_sel=10 for LNA AGC */
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x1, hChn->tunerAgcThreshold & 0x0000FFFF);    /* PD threshold for LNA AGC */
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x2, hChn->tunerAgcWinLength & 0x0000FFFF);    /* PD window size for LNA AGC */
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x4, hChn->tunerAgcLoopCoeff & 0x001F);        /* loop coeff value */
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x5, 0x80000000);   /* loop integrator init to midpoint */
         
         #if ((BCHP_CHIP != 7344) && (BCHP_CHIP != 7346)) || (BCHP_VER > BCHP_VER_A0)
            /* initialize DCO lock detectors */
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x0, 0x07334010); /* 0.050 threshold, 2^-16 leakage, enable DSM */
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x0, 0x07334010);
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x0, 0x07334010);
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x0, 0x07334010);

            /* M/N = 768/1024 */
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x1, 0x00010001);
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x1, 0x00010001);
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x1, 0x00010001);
            BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x1, 0x00010001);
         #endif
            
            if (h->channel == 0)
            {
               /* calibrate kvco for ch0 only */
               hChn->count2 = 0;
               return BAST_g3_P_TunerCalibrateKvco(h);
            }
            
            /* continue with next function */
            if (hChn->postTuneFunct != NULL)
               return hChn->postTuneFunct(h);
            else
               return BERR_SUCCESS;
            
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;
      }
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerVcSearch() - binary search for VcRef scaled by 4
******************************************************************************/
BERR_Code BAST_g3_P_TunerVcSearch(BAST_ChannelHandle h, uint32_t *Vc)
{
   uint32_t i, val;
   uint8_t mask, vcRef, popcount;
   
   /* initial VcRef */
   vcRef = mask = 0x10;
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R03, ~0xF8000000, 0x80000000);
   
   do {
      /* delay before reading popcap */
      for (i = 0; i < 999; i++);
      
      popcount = 0;
      for (i = 0; i < 5; i++)
      {
         BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_PLLSTAT_R01, &val);
         /* printf(">> popcap=%x\n", val & 0x04); */
         if (val & 0x04)
            popcount++;
      }
      
      if (popcount >= 3)
      {
         /* printf(" %02X |", vcRef); */
         mask >>= 1;
         vcRef |= mask;
         /* printf(" %02X = %02X\n", mask, vcRef); */
      }
      else
      {
         /* printf(" %02X & ~%02X", vcRef, mask); */
         vcRef &= ~mask;
         mask >>= 1;
         vcRef |= mask;
         /* printf(" | %02X = %02X\n", mask, vcRef); */
      }
      
      /* update VcRef */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R03, ~0xF8000000, vcRef << 27);
   }
   while (mask > 0);
   
   /* Vc = 250mV + VcREF[4:0] * 31.8mV scaled by 5*/
   *Vc = 1250 + (vcRef & 0x1F) * 159;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerCalibrateKvco() - Kvco calibration to generate LUT - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TunerCalibrateKvco(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   if (hChn->count2 >= 8)
   {
   #if 0
      BDBG_MSG(("cap_cntl|kvco_cntl"));
      for (hChn->count2 = 0; hChn->count2 < 8; hChn->count2++)
      {
         BDBG_MSG(("%d|%03X", tuner_kvco_cal_capcntl_table[hChn->count2], tuner_kvco_cal_kvcocntl_table[hChn->count2]));
      }
   #endif
      
      /* done kvco calibration */
      if (hChn->postTuneFunct != NULL)
         return hChn->postTuneFunct(h);
      else
         return BERR_SUCCESS;
   }
   
   /* set lo frequency and parameters */
   hChn->maxCount1 = 0;    /* reset safety counter every vco step */
   hChn->tunerVcoFreqMhz = 5800 + 400 * hChn->count2;
   hChn->tunerFreq = (hChn->tunerVcoFreqMhz >> 2) * 1000000;   /* F_lo = F_vco / 4 */
   BAST_g3_P_TunerSetLoParams(h, 0);
   
   /* program FCW */
   BAST_g3_P_TunerGetLoDivider(h, &(hChn->tunerLoDivider));
   BAST_g3_P_TunerSetFcw(h);
   BAST_DEBUG_KVCO_CAL(BDBG_MSG(("[%d] vco=%d lo=%u", hChn->count2, hChn->tunerVcoFreqMhz, hChn->actualTunerFreq)));
   
   BAST_g3_P_ToggleBit(h, BCHP_SDS_TUNER_LO_R03, 0x00003000);  /* reset div23, cml */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_TUNER_LO_R02, 0x80000000);  /* reset IQ gen */
   
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R03, ~0xF8000000, 0xC8000000);  /* comparator Vc threshold */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R02, ~0x0000F800, 0x00002800);  /* QP bias control */
   
   hChn->funct_state = 0;
   hChn->bCalibrateKvco = true;  /* continue to TunerCalibrateKvco1 after cap search */
   hChn->bOverrideKvco = false;  /* use default kvco values from table 3 */
   return BAST_g3_P_TunerBinaryCapScan(h);
}


/******************************************************************************
 BAST_g3_P_TunerCalibrateKvco1() - Kvco calibration to generate LUT - ISR context
******************************************************************************/
#define BAST_KVCO_LO_THRESH 130
#define BAST_KVCO_HI_THRESH 180
BERR_Code BAST_g3_P_TunerCalibrateKvco1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t Kvco, vc1, vc2, val;
   
   BAST_g3_P_TunerVcSearch(h, &vc1);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LODDFS_R02, &val);
   BAST_DEBUG_KVCO_CAL(BDBG_MSG(("fcw0=%08X -> vc0=%d", val, vc1)));
   
   /* calculate lower vco = (f_vco - 8) / 4 */
   hChn->tunerFreq = ((hChn->tunerVcoFreqMhz - 8) >> 2) * 1000000;
   BAST_g3_P_TunerSetFcw(h);
   BAST_g3_P_TunerVcSearch(h, &vc1);
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LODDFS_R02, &val);
   BAST_DEBUG_KVCO_CAL(BDBG_MSG(("fcw1=%08X -> vc1=%d", val, vc1)));
   
   /* calculate upper vco = (f_vco + 16) / 4 */
   hChn->tunerFreq = ((hChn->tunerVcoFreqMhz + 16) >> 2) * 1000000;
   BAST_g3_P_TunerSetFcw(h);
   BAST_g3_P_TunerVcSearch(h, &vc2);
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LODDFS_R02, &val);
   BAST_DEBUG_KVCO_CAL(BDBG_MSG(("fcw2=%08X -> vc2=%d", val, vc2)));
   
   if ((vc1 >= 6179) || (vc2 >= 6179) || (vc1 == vc2))
   {
      /* increment kvco_cntl and exit if Vc maxed out */
      hChn->tunerKvcoCntl++;
      Kvco = BAST_KVCO_HI_THRESH - 1;
      BAST_DEBUG_KVCO_CAL(BDBG_MSG(("Vc maxed")));
   }
   else
   {
      /* calculate Kvco = df_vco / dV = 24 / (vc2 - vc1), note vc1 and vc2 scaled by 5 */
      Kvco = 240000 / (vc2 - vc1);
      Kvco = (Kvco + 1) >> 1; /* round Kvco */
      BAST_DEBUG_KVCO_CAL(BDBG_MSG(("Kvco=%d", Kvco)));
   }
   
   /* proceed to next vco step if kvco within range, or kvco_cntl maxed out */
   if (((Kvco > BAST_KVCO_LO_THRESH) && (Kvco < BAST_KVCO_HI_THRESH)) || (hChn->tunerKvcoCntl >= 0x7))
   {
      /* save cap_cntl and calibrated kvco_cntl */
      tuner_kvco_cal_capcntl_table[hChn->count2] = hChn->tunerCapCntl;
      tuner_kvco_cal_kvcocntl_table[hChn->count2] = hChn->tunerKvcoCntl;
      hChn->count2++;
      
      return BAST_g3_P_TunerCalibrateKvco(h);
   }
   else if (hChn->maxCount1 > 5)
   {
      /* exceeded Kvco calibration safety count, select higher kvco_cntl */
      BDBG_WRN(("Kvco calibration oscillating..."));
      hChn->tunerKvcoCntl = hChn->tunerKvcoCntl_high;
      
      if (hChn->count2 > 0)
      {
         /* check if kvco_cntl greater than previous entry */
         if (hChn->tunerKvcoCntl > tuner_kvco_cal_kvcocntl_table[hChn->count2 - 1])
            hChn->tunerKvcoCntl = tuner_kvco_cal_kvcocntl_table[hChn->count2 - 1];
      }
      
      /* save cap_cntl and calibrated kvco_cntl */
      tuner_kvco_cal_capcntl_table[hChn->count2] = hChn->tunerCapCntl;
      tuner_kvco_cal_kvcocntl_table[hChn->count2] = hChn->tunerKvcoCntl;
      hChn->count2++;
      
      return BAST_g3_P_TunerCalibrateKvco(h);
   }
   else
   {
      hChn->maxCount1++;   /* increment safety counter */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LO_R02, &val);
      hChn->tunerKvcoCntl = (val >> 16) & 0x7;
      
      if ((Kvco < BAST_KVCO_LO_THRESH) && (hChn->tunerKvcoCntl < 0x7))
      {
         hChn->tunerKvcoCntl_low = hChn->tunerKvcoCntl;
         hChn->tunerKvcoCntl++;  /* increase kvco */
         BAST_DEBUG_KVCO_CAL(BDBG_MSG(("kvcoCntl++=%d", hChn->tunerKvcoCntl)));
      }
      else if (hChn->tunerKvcoCntl > 0)
      {
         hChn->tunerKvcoCntl_high = hChn->tunerKvcoCntl;
         hChn->tunerKvcoCntl--;  /* decrease kvco */
         BAST_DEBUG_KVCO_CAL(BDBG_MSG(("kvcoCntl--=%d", hChn->tunerKvcoCntl)));
      }
      
      /* set lo frequency and parameters */
      hChn->tunerFreq = (hChn->tunerVcoFreqMhz >> 2) * 1000000;   /* F_lo = F_vco / 4 */
      BAST_g3_P_TunerSetLoParams(h, 0);
      
      /* program FCW */
      BAST_g3_P_TunerGetLoDivider(h, &(hChn->tunerLoDivider));
      BAST_g3_P_TunerSetFcw(h);
      
      BAST_g3_P_ToggleBit(h, BCHP_SDS_TUNER_LO_R03, 0x00003000);  /* reset div23, cml */
      BAST_g3_P_ToggleBit(h, BCHP_SDS_TUNER_LO_R02, 0x80000000);  /* reset IQ gen */
      
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R03, ~0xF8000000, 0xC8000000);  /* comparator Vc threshold */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R02, ~0x0000F800, 0x00002800);  /* QP bias control */
      
      /* tune LO using new kvco_cntl */
      hChn->funct_state = 0;
      hChn->bCalibrateKvco = true;  /* resume at TunerCalibrateKvco1 after cap search */
      hChn->bOverrideKvco = true;   /* override kvco values */
      return BAST_g3_P_TunerBinaryCapScan(h);
   }
}


/******************************************************************************
 BAST_g3_P_TunerGetFddfs()
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetFddfs(BAST_ChannelHandle h, uint32_t *Fddfs)
{
   BAST_ChannelHandle hChn0 = (BAST_ChannelHandle)(h->pDevice->pChannels[0]);
   uint32_t val;

   /* read feedback div from TUNER0 */
   BAST_g3_P_ReadRegister(hChn0, BCHP_SDS_TUNER_REFPLL_R01, &val);
   *Fddfs = BAST_G3_XTAL_FREQ * (val >> 25);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerSetLoParams() - this function programs div23_sel, IQGEN_seldivn, 
                         mixsel, IsetSR, QsetSR, and fb_divn
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetLoParams(BAST_ChannelHandle h, int8_t step)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   uint8_t idx;
   
   static const uint32_t tuner_lo_freq_table[BAST_TUNER_LO_TABLE_SIZE] =
   {
      2867000000UL,
      2151000000UL,
      1434000000UL,
      1076000000UL,
      717000000UL,
      539000000UL,
      359000000UL,
      239000000UL
   };

   /* div23_sel = TUNER_LO_R03[14] --> bit 6 */
   /* IQGEN_seldivn = TUNER_LO_R04[15:14] --> bits 3:2 */
   /* fb_divn = TUNER_LO_R03[22:15] --> bits 14:7 */
   static const uint16_t tuner_lo_table_0[BAST_TUNER_LO_TABLE_SIZE] =
   {
      0x0C00,
      0x0840,
      0x0C04,
      0x0844,
      0x0C0C,
      0x084C,
      0x0C08,
      0x0848
   };


   /* mixsel = TUNER_MXFGA_R02[3:2] --> bits 3:2 */
   /* IsetSR = TUNER_MXFGA_R02[17:6] --> bits 17:6 */
   /* QsetSR = TUNER_MXFGA_R02[29:18] --> bits 29:18 */
   static const uint32_t tuner_lo_table_1[BAST_TUNER_LO_TABLE_SIZE] =
   {
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000
   };
   
   /* verify lo parameter step size */
   if (abs(step) > 1)
      return BERR_INVALID_PARAMETER;
   
   /* lookup freq range */
   BAST_g3_P_TunerUpdateActualTunerFreq(h);
   for (idx = 0; idx < BAST_TUNER_LO_TABLE_SIZE - 1; idx++)
   {
      if (hChn->actualTunerFreq >= tuner_lo_freq_table[idx])
         break;
   }
   
   /* step lo parameters if required */
   if (((step > 0) && (idx > 0)) || ((step < 0) && (idx < BAST_TUNER_LO_TABLE_SIZE - 1)))
      idx -= step;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LO_R03, &val);
   val &= ~0x007FC000; /* clear bits 22:14 */
   val |= (tuner_lo_table_0[idx] & 0x0040) << 8;   /* div23_sel */
   val |= (tuner_lo_table_0[idx] & 0x7F00) << 8;   /* fb_divn, bits 7:1 */
   val |= (tuner_lo_table_0[idx] & 0x0080) << 8;   /* fb_divn, bit 0 */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_LO_R03, &val);
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LO_R04, &val);
   val &= ~0x0000C000; /* clear bits 15:14 */
   val |= ((tuner_lo_table_0[idx] << 12) & 0xC000);   /* IQGEN_seldivn */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_LO_R04, &val);
   
   /* clear and set bits 29:6 and bits 3:2 */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_MXFGA_R02, ~0x3FFFFFCC, tuner_lo_table_1[idx]);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerUpdateActualTunerFreq()
******************************************************************************/
BERR_Code BAST_g3_P_TunerUpdateActualTunerFreq(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->actualTunerFreq = hChn->tunerFreq + hChn->tunerIfStep;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerGetLoDivider() - calculates N/M ratio, tunerLoDivider scaled 2^6
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetLoDivider(BAST_ChannelHandle h, uint32_t *tunerLoDivider)
{
   uint32_t val;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LO_R03, &val);  /* fb_divn = TUNER_LO_R03[22:15] */
   *tunerLoDivider = (val & 0x007F8000) >> 9;               /* N = fb_divn, scaled 2^6 */
   
   /* check mixer selection */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_MXFGA_R02, &val);
   val = (val & 0x0C) >> 2;
   if (val == 0x1)
      *tunerLoDivider /= 6;
   else if (val == 0x2)
      *tunerLoDivider /= 8;
   else if (val == 0x3)
      *tunerLoDivider /= 12;
   else
   {
      /* divide by LOTREE divider */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LO_R04, &val);
      val = (val & 0xC000) >> 14;
      if (val == 2)
         val = 8;    /* M = 8 when IQGEN_seldiv = 2*/
      else
         val += 1;   /* M = IQGEN_seldivn + 1 when IQGEN_seldiv = 0, 1, 3 */
      *tunerLoDivider /= val;  
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerSetFcw() - computes and programs frequency control word
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetFcw(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
   
   BAST_g3_P_TunerUpdateActualTunerFreq(h);
   
   /* FCW = (Fc / Fddfs) * (M / N) * 2^32 where Fddfs = 1188MHz */
   BAST_MultU32U32(hChn->actualTunerFreq, 2147483648UL, &P_hi, &P_lo); /* Fc * 2^31 */
   BAST_DivU64U32(P_hi, P_lo, hChn->tunerFddfs >> 8, &Q_hi, &Q_lo);  /* (Fc * 2^31) / (Fddfs / 2^8) */
   BAST_DivU64U32(Q_hi, Q_lo, hChn->tunerLoDivider, &Q_hi, &Q_lo);   /* (Fc / Fddfs) * 2^39) * (M / N) / 2^6 */
   
   /* round FCW value */
   Q_lo = (Q_lo + 1) >> 1;
   
   /* program fcw[36:5] */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_LODDFS_R02, &Q_lo);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerAutoTune() - hw state machine autotune
******************************************************************************/
BERR_Code BAST_g3_P_TunerAutoTune(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t val;
   
   BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_FILCAL_DDFS_CTL, ~0x00020000); /* enable FILCAL DAC clock */

   while (1)
   {
      switch (hChn->funct_state)
      {
         case 0:
            /* enable auto-tuner - hold reset, disable auto-tune, LO parameters by SM */
            BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R05, ~0x80000400, 0x40000000);
            BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_LO_R05, 0x80000000);    /* release reset */
            
            hChn->funct_state = 1;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100, BAST_g3_P_TunerAutoTune);
            
         case 1:
            /* enable auto-tune state machine */
            BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_LO_R05, 0x00000400);
            
            hChn->funct_state = 2;
            hChn->count1 = 0;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 500, BAST_g3_P_TunerAutoTune);
         
         case 2:
            BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LO_R07, &val);
            if ((val & 0x40) || (hChn->count1 > 3))
            {
               /* disable auto-tune state machine when done */
               BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_LO_R05, ~0x00000400);
               BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_FILCAL_DDFS_CTL, 0x00020000); /* turn off FILCAL DAC clock */
               return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100, BAST_g3_P_TunerSetFreq1);
            }
            
            /* continue to wait for state machine */
            hChn->count1++;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 500, BAST_g3_P_TunerAutoTune);
            
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;
      }
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerBinaryCapScan() - binary capacitor scan - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TunerBinaryCapScan(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t val;
   
   while (1)
   {
      switch (hChn->funct_state)
      {
         case 0:
            /* initialize cap and counter */
            hChn->tunerCapCntl = 0x100;
            hChn->count1 = 8;
            
            hChn->funct_state = 1;
            break;
            
         case 1:
            hChn->tunerCapMask = 1 << hChn->count1;  /* cap_cntl mask */
            
            hChn->tunerCapCntl |= hChn->tunerCapMask;
            BAST_CHK_RETCODE(BAST_g3_P_TunerSetCapCntl(h));
            BAST_CHK_RETCODE(BAST_g3_P_TunerSetCapCntlLoopParams(h));
            BAST_DEBUG_CAP_SEARCH(BDBG_MSG(("bin%d: cap=%x", hChn->count1, hChn->tunerCapCntl)));
            
            /* delay before reading popcap */
            hChn->funct_state = 2;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 40, BAST_g3_P_TunerBinaryCapScan);
            
         case 2:
            /* read popcap */
            BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_PLLSTAT_R01, &val);
            if (val & 0x04)
               hChn->tunerCapCntl &= ~hChn->tunerCapMask;  /* pop cap_cntl <i> = 0 */
            else
               hChn->tunerCapCntl |= hChn->tunerCapMask;   /* add cap_cntl <i> = 1 */
            BAST_CHK_RETCODE(BAST_g3_P_TunerSetCapCntl(h));
            
            /* binary search up to second LSB if linear search on */
            if (hChn->count1 <= (uint8_t)(hChn->tunerLsRange ? 1 : 0))
            {
               /* finished binary scan */
               if (hChn->tunerCapCntl == 0x1FF)
               {
                  /* step down lo parameters if cap maxed out */
                  BDBG_MSG(("maxcap: stepdown loparams"));
                  BAST_g3_P_TunerSetLoParams(h, -1);
                  
                  /* reprogram FCW */
                  BAST_g3_P_TunerGetLoDivider(h, &(hChn->tunerLoDivider));
                  BAST_g3_P_TunerSetFcw(h);
                  
                  /* redo cap search */
                  hChn->funct_state = 0;
               }
               else if (hChn->tunerCapCntl == 0)
               {
                  /* step up lo parameters if minimum cap */
                  BDBG_MSG(("mincap: stepup loparams"));
                  BAST_g3_P_TunerSetLoParams(h, 1);
                  
                  /* reprogram FCW */
                  BAST_g3_P_TunerGetLoDivider(h, &(hChn->tunerLoDivider));
                  BAST_g3_P_TunerSetFcw(h);
                  
                  /* redo cap search */
                  hChn->funct_state = 0;
               }
               else
               {
                  /* cap search complete */
                  BAST_CHK_RETCODE(BAST_g3_P_TunerSetCapCntlLoopParams(h));
                  if (hChn->tunerLsRange)
                  {
                     hChn->funct_state = 0;
                     return BAST_g3_P_TunerLinearCapScan(h);
                  }
                  
                  BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R02, ~0x0000F800, 0x00002800);  /* QP bias control */
                  
                  if (hChn->bCalibrateKvco)
                     return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 40, BAST_g3_P_TunerCalibrateKvco1);
                  else
                     return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 40, BAST_g3_P_TunerSetFreq1);
               }
            }
            else
            {
               /* continue cap search */
               hChn->count1--;
               hChn->funct_state = 1;
            }
            break;
            
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;
      }
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerLinearCapScan() - linear capacitor scan - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TunerLinearCapScan(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t val;
#if (BCHP_CHIP == 7346)
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   BCHP_AvsData avsData;
   int32_t vcAdjust;
#endif
   
   /* return if search range zero */
   if (hChn->tunerLsRange == 0)
      return BERR_SUCCESS;
   
   while (1)
   {
      switch (hChn->funct_state)
      {
         case 0:
            hChn->tunerCapCntl -= hChn->tunerLsRange;
            hChn->count1 = 0;
            
         #if (BCHP_CHIP == 7346)
            if ((hDev->hChip) && (hChn->tempAmbient > 0))
            {
               BCHP_GetAvsData(hDev->hChip, &avsData);
               /* BDBG_MSG(("temp=%d.%d", avsData.temperature / 1000, avsData.temperature % 1000)); */
               
               /* Vc adjustment factor is 0.075 * (T_curr - T_ambient) */
               vcAdjust = 3 * ((int32_t)(avsData.temperature) - hChn->tempAmbient * 1000) / 40;
               
               /* Vcref_low = 19 + 0.075*(T_curr - T_ambient) scaled 2^20*/
               /* BDBG_MSG(("19000 + (%d) = %d", vcAdjust, 19000 + vcAdjust)); */
               hChn->tunerVcRefLow = (19000 + vcAdjust) / 500;
               hChn->tunerVcRefLow = (hChn->tunerVcRefLow + 1) >> 1;
               
               /* Vcref_high = 25 + 0.075*(T_curr - T_ambient) */
               hChn->tunerVcRefHigh = (25000 + vcAdjust) / 500;
               hChn->tunerVcRefHigh = (hChn->tunerVcRefHigh + 1) >> 1;
               /* BDBG_MSG(("VcLow=%d, VcHigh=%d", hChn->tunerVcRefLow, hChn->tunerVcRefHigh)); */
            }
         #endif
            
            hChn->funct_state = 1;
            break;
         
         case 1:
            BAST_CHK_RETCODE(BAST_g3_P_TunerSetCapCntl(h));
            BAST_CHK_RETCODE(BAST_g3_P_TunerSetCapCntlLoopParams(h));
            BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R03, ~0xF8000000, hChn->tunerVcRefLow << 27);  /* comparator Vc threshold low */
            BAST_DEBUG_CAP_SEARCH(BDBG_MSG(("ls%d: cap=%x", hChn->count1, hChn->tunerCapCntl)));
            
            /* delay before reading popcap */
            hChn->funct_state = 2;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 40, BAST_g3_P_TunerLinearCapScan);
         
         case 2:
            /* read popcap */
            BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_PLLSTAT_R01, &val);
            if (val & 0x04)
            {
               BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R03, ~0xF8000000, hChn->tunerVcRefHigh << 27);  /* comparator Vc threshold high */
               hChn->funct_state = 3;
               return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 40, BAST_g3_P_TunerLinearCapScan);
            }
            
            hChn->funct_state = 4;
            break;
         
         case 3:
            /* read popcap */
            BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_PLLSTAT_R01, &val);
            if (val & 0x04)
               hChn->funct_state = 4;
            else
               hChn->funct_state = 5;
            break;
         
         case 4:
            if ((hChn->count1 < (uint8_t)(2 * hChn->tunerLsRange)) && (hChn->tunerCapCntl < 0x1FF))
            {
               /* add cap, continue linear search */
               hChn->tunerCapCntl++;
               hChn->count1++;
               hChn->funct_state = 1;
            }
            else
               hChn->funct_state = 5;
            break;
         
         case 5:
            /* linear search complete */
            if (hChn->tunerCapCntl == 0x1FF)
            {
               /* step down lo parameters if cap maxed out */
               BDBG_MSG(("maxcap: stepdown loparams"));
               BAST_g3_P_TunerSetLoParams(h, -1);
               
               /* reprogram FCW */
               BAST_g3_P_TunerGetLoDivider(h, &(hChn->tunerLoDivider));
               BAST_g3_P_TunerSetFcw(h);
               
               /* redo cap search */
               hChn->funct_state = 0;
               return BAST_g3_P_TunerBinaryCapScan(h);
            }
            else if (hChn->tunerCapCntl == 0)
            {
               /* step up lo parameters if minimum cap */
               BDBG_MSG(("mincap: stepup loparams"));
               BAST_g3_P_TunerSetLoParams(h, 1);
               
               /* reprogram FCW */
               BAST_g3_P_TunerGetLoDivider(h, &(hChn->tunerLoDivider));
               BAST_g3_P_TunerSetFcw(h);
               
               /* redo cap search */
               hChn->funct_state = 0;
               return BAST_g3_P_TunerBinaryCapScan(h);
            }
            else
            {
            BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R02, ~0x0000F800, 0x00002800);  /* QP bias control */
            
            if (hChn->bCalibrateKvco)
               return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 40, BAST_g3_P_TunerCalibrateKvco1);
            else
               return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 40, BAST_g3_P_TunerSetFreq1);
            }
         
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;
      }
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerSetCapCntl() - this function programs cap_cntl,  i_cntl
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetCapCntl(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint16_t cap_cntl = hChn->tunerCapCntl;
   
   /* VCO cap setting - 9 bits from cap_cntl[8:0] */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R02, ~0x0FF80000, (cap_cntl << 19) & 0x0FF80000);
   
   /* same as VCO cap setting - msb 7 bits for I_cntl[6:0]  = cap_cntl[8:2] */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LO_R03, ~0x000001FC, cap_cntl & 0x1FC);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerSetCapCntlLoopParams() - this function programs wben_lf, nbr_lf, 
                                    QPbiasCNT2, Kvco_cntl
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetCapCntlLoopParams(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   uint16_t sval;
   uint8_t idx;
   
   static const uint16_t tuner_cap_cntl_table[BAST_TUNER_CAP_CNTL_TABLE_SIZE] = {
      122,
      160,
      228,
      310,
      512
   };

   /* wben_lf = TUNER_LO_R01[28] --> bit 12 */
   /* nbr_lf = TUNER_LO_R01[27:26] --> bits 14:15 */
   /* QPbiasCNT2 = TUNER_LO_R02[10:6] --> bits 10:6 */
   /* Kvco_cntl = TUNER_LO_R02[18:16] --> bits 2:0 */
   static const uint16_t tuner_cap_cntl_loop_params_table[BAST_TUNER_CAP_CNTL_TABLE_SIZE] = {
      0xC141,
      0x4241,
      0xC142,
      0x4243,
      0x4245,
   };
   
   if (hChn->bCalibrateKvco)
   {
      /* lookup capcntl range for calibration */
      for (idx = 0; idx < BAST_TUNER_CAP_CNTL_TABLE_SIZE - 1; idx++)
      {
         if (hChn->tunerCapCntl <= tuner_cap_cntl_table[idx])
            break;
      }
      
      sval = tuner_cap_cntl_loop_params_table[idx];
      
      /* use Kvco_cntl from table if not overriding */
      if (!hChn->bOverrideKvco)
         hChn->tunerKvcoCntl = sval & 0x0007;
   }
   else
   {
      /* use Kvco_cntl from calibrated table during regular tuning */
      for (idx = 1; idx < BAST_TUNER_KVCO_CAL_TABLE_SIZE - 1; idx++)
      {
         /* interval boundaries at CAP[1..6] + 1 */
         if (hChn->tunerCapCntl > tuner_kvco_cal_capcntl_table[idx])
            break;
      }
      
      sval = 0x4240;    /* nbr_lf = b'01, wben_lf=0, QPbiasCNT2=b'01001 */
      hChn->tunerKvcoCntl = tuner_kvco_cal_kvcocntl_table[idx-1];
      /* BDBG_MSG(("cap=%d: kvco_cntl=%d\n", hChn->tunerCapCntl, tuner_kvco_cal_kvcocntl_table[idx])); */
   }

   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LO_R01, &val);
   val &= ~0x1C000000;
   val |= (sval & 0x1000) << 16;    /* wben_lf */
   val |= (sval & 0xC000) << 12;    /* nbr_lf */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_LO_R01, &val); 
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LO_R02, &val);
   val &= ~0x000707C0;
   val |= (hChn->tunerKvcoCntl & 0x7) << 16; /* Kvco_cntl */
   val |= (sval & 0x07C0);                   /* QPbiasCNT2 */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_LO_R02, &val);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerSetFreq1()
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetFreq1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   bool bRefPllLocked = false, bMixPllLocked = false;
   BERR_Code retCode;
   uint32_t val;
   
   BAST_CHK_RETCODE(BAST_g3_P_TunerGetLockStatus(h, &bRefPllLocked, &bMixPllLocked));
   if (!bRefPllLocked)
   {
      BDBG_ERR(("refpll NOT locked"));
   }
   if (!bMixPllLocked)
   {
      BDBG_WRN(("mixpll NOT locked"));
      
      /* disable hw state machine */
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_LO_R05, ~0xC0000400);
      
      /* retry cap search */
      hChn->funct_state = 0;
      hChn->bCalibrateKvco = false;
      hChn->bOverrideKvco = false;
      return BAST_g3_P_TunerBinaryCapScan(h);
   }
   
#if 0
   /* TBD turn on testport for debug */
   BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_IFPGA_R01, 0x20);
   BAST_DEBUG_TUNER(BDBG_MSG(("mixPLL LOCKED!")));
#endif
   
   if (hChn->acqState == BAST_AcqState_eTuning)
   {
      BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eTuneMixPllLock));
      
      /* SDS DCO init */
      val = 0x00030B00;    /* unfreeze SDS DCO */
      BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_DCOCTL, &val);
      BAST_g3_P_AndRegister(h, BCHP_SDS_FE_DCOCTL, ~0x00000300);  /* release DCO resets */
      val = 0x00000000;    /* clear SDS DCO integrator */
      BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_DCOI, &val);
      
   #if ((BCHP_CHIP != 7344) && (BCHP_CHIP != 7346)) || (BCHP_VER > BCHP_VER_A0)
      /* clear pre/post DCO integrators */
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x4, 0x0);
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x4, 0x0);
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x4, 0x0);
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x4, 0x0);
   #endif
      
      /* reset and enable BB AGC */
      val = ((hChn->tunerAgcAmpThresh & 0x1F00) << 5) | 0x00000050;     /* set amplitude threshold, reset */
      BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_BBAGC_R01, &val);
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_BBAGC_R01, ~0x00000010);  /* release reset */
      
      /* initialize BB AGC settings */
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x0, 0x00004019);   /* invert k, bypass sign function and gray stepper, bb_rf_out_sel=01 for BB AGC */
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x1, hChn->tunerAgcThreshold & 0xFFFF0000);    /* PD threshold for BB AGC */
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x2, hChn->tunerAgcWinLength >> 16);           /* PD window size for BB AGC */
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x4, hChn->tunerAgcLoopCoeff >> 8);            /* loop coeff value */
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x5, 0x80000000);   /* loop integrator init to midpoint */
      
      /* calculate tracking bw */
      BAST_g3_P_TunerGetCutoff(h, &(hChn->tunerLpfToCalibrate));
      
      /* wait for DCO convergence */
      hChn->count1 = 0;
      hChn->funct_state = 0;
      return BAST_g3_P_TunerWaitForDcoConverge(h);
   }
   
   if (hChn->postTuneFunct != NULL)
      return hChn->postTuneFunct(h);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerGetCutoff() - compute required tracking bandwidth in MHz
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetCutoff(BAST_ChannelHandle h, uint8_t *lpfCal)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t Fb, val, P_hi, P_lo, Q_hi, Q_lo;
   
   Fb = hChn->acqParams.symbolRate;
   if (Fb >= 6000000)
   {
      /*( (nyquist + 1)/2+ 8% )Fb per Steve: (0.35+1.0)/2+0.08= 0.755, (0.2+1.0)/2+0.08=0.68 */
      /* since we are not going to do final freq transfer, we have to include the fine dft step size in the filter, i.e. 5% of Fb */
      if (((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_NYQUIST_20) == 0) && (h->pDevice->settings.networkSpec != BAST_NetworkSpec_eEuro))
         val = 65;   /* narrow to 80% and add dft_step size, 0.755*0.80+0.05= 0.65 scaled by 10^2 */
      else
         val = 59;   /* narrow to 80% and add dft_step size, 0.68*0.80+0.05 =0.59scaled by 10^2  */ 
       
      BAST_MultU32U32(Fb, val * 2, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, 100000000, &Q_hi, &Q_lo);
      Q_lo = (Q_lo + 1) >> 1;          /* round */
      *lpfCal = (uint8_t)(Q_lo + 1);   /* add 1 MHz */
   }
   else if (Fb <= 1500000)
      *lpfCal = 0x01;
   else if (Fb <= 2500000)
      *lpfCal = 0x02;
   else if (Fb <= 3000000)
      *lpfCal = 0x03;
   else if (Fb <= 4000000)
      *lpfCal = 0x04;
   else
      *lpfCal = 0x05; /* 4MHz < Fb < 6MHz */
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerWaitForDcoConverge() - check and wait for dco lock detect
******************************************************************************/
BERR_Code BAST_g3_P_TunerWaitForDcoConverge(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t val;
   
#if ((BCHP_CHIP == 7344) || (BCHP_CHIP == 7346)) && (BCHP_VER == BCHP_VER_A0)
   /* TBD skip DCO converge since DCO indirect access failure */
   hChn->funct_state = 2;
#endif
   
   while (1)
   {
      switch (hChn->funct_state)
      {
         case 0:
            /* check lock bit for all DCOs */
            BAST_CHK_RETCODE(BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x6, &val));
            if ((val & 0x1) == 0)
            {
               hChn->funct_state = 1;
               break;
            }
            BAST_CHK_RETCODE(BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x6, &val));
            if ((val & 0x1) == 0)
            {
               hChn->funct_state = 1;
               break;
            }
            BAST_CHK_RETCODE(BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x6, &val));
            if ((val & 0x1) == 0)
            {
               hChn->funct_state = 1;
               break;
            }
            BAST_CHK_RETCODE(BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x6, &val));
            if ((val & 0x1) == 0)
            {
               hChn->funct_state = 1;
               break;
            }
            
            /* proceed if all DCOs locked */
            hChn->funct_state = 2;
            break;
            
         case 1:
         #ifdef EMU
            /* fast track dco wait for emulation */
            hChn->count1 += 50;
         #endif
            if (hChn->count1++ < 100)
            {
               /* wait for convergence */
               hChn->funct_state = 0;
               return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100, BAST_g3_P_TunerWaitForDcoConverge);
            }
            
            /* proceed after retries exceeded */
            hChn->funct_state = 2;
            break;
            
         case 2:
            /* proceed to filter calibration */
            return BAST_g3_P_TunerSetFreq2(h);
            
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;
      }
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerSetFreq2()
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetFreq2(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
#if defined(BAST_TUNER_BYPASS_IFAGC) || (BCHP_CHIP == 7358)
   uint32_t val;
#endif
   
   BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eDcoConverge));
   
   if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_SET_FILTER_MANUAL)
   {
      /* set manual filter setting after DFT, so save the calibrated values and apply them when switching to tracking LPF */
      BAST_g3_P_TunerCalFilter(h, hChn->tunerCutoff);
      hChn->tunerFgaCalManual = hChn->tunerFgaCal;
      hChn->tunerLpfCalManual = hChn->tunerLpfCal;
   }   
   BAST_g3_P_TunerCalFilter(h, hChn->tunerLpfToCalibrate);
   BAST_g3_P_TunerSetFilter(h, false);   /* set lpf wide for acq */
   
   BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eTunerLpfCalDone));
   
#ifdef BAST_TUNER_BYPASS_AGC
   /* bypass LNA AGC loop for bringup */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_RFAGC_R02, ~0x007E0000, 0x00200000);  /* LNA AGC bypass gain value - binary offset 50% gain */
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_eRfagc, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x0, (val | 0x200));
      
   /* bypass BB AGC loop for bringup */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_BBAGC_R02, ~0x007E0000, 0x00200000);  /* BB AGC bypass gain value - binary offset 50% gain */
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_eBbagc, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x0, (val | 0x200));
#endif
   
   /* reset and enable IF AGC */
   BAST_CHK_RETCODE(BAST_g3_P_ConfigAgc(h));
   
#ifdef BAST_TUNER_BYPASS_IFAGC
   val = 0x00041E1E;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_AGC_AGCCTL, &val);   /* freeze IF AGC */
   val = 0xA0000000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_AGC_AII, &val);
#endif

#if ((BCHP_CHIP == 7344) || (BCHP_CHIP == 7346)) && (BCHP_VER == BCHP_VER_A0)
   /* toggle dco clocks as workaround */
   BAST_g3_P_TunerToggleDcoClock(h);
#endif

#if (BCHP_CHIP == 7358)
   /* change DEM mode to DWA */
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x0, &val);
   val &= ~0x00000700;
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x0, (val | 0x100));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x0, &val);
   val &= ~0x00000700;
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x0, (val | 0x100));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x0, &val);
   val &= ~0x00000700;
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x0, (val | 0x100));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x0, &val);
   val &= ~0x00000700;
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x0, (val | 0x100));

#if 0   
   /* run and freeze dco loops */
   BAST_g3_P_TunerRunDco(h, BAST_TimerSelect_eGen2, 20); /* unfreeze DCO 20msec */
#endif
#endif

   BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eTuneDone));   
   if (hChn->postTuneFunct != NULL)
      retCode = hChn->postTuneFunct(h);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerCalFilter() - calibrate and set LPF and FGA bandwidths
******************************************************************************/
BERR_Code BAST_g3_P_TunerCalFilter(BAST_ChannelHandle h, uint8_t cutoff)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t mxfga;
   
   static const uint32_t script_tuner_calfilter_start[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_FE_ADCPCTL, 0x000F0001),   /* reset ADC data capture FIFO */
      BAST_SCRIPT_AND(BCHP_SDS_FE_ADCPCTL, ~0x00000001),    /* clear ADC data capture FIFO reset */
      BAST_SCRIPT_WRITE(BCHP_SDS_AGC_AGCCTL, 0x00041E1E),   /* freeze IF AGC */
      BAST_SCRIPT_WRITE(BCHP_SDS_AGC_AII, 0x80000000),      /* set IF AGC integrator for appropriate DDFS tone level for LPF cal */
      
   #if (BCHP_VER >= BCHP_VER_B0)
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_MXFGA_R02, 0xC0000052),       /* hold LO phase shifters in reset, open I/Q mix switches */
   #else
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_MXFGA_R02, 0xC0000012),       /* hold LO phase shifters in reset, open I/Q mix switches */
   #endif
      BAST_SCRIPT_OR(BCHP_SDS_TUNER_PWRUP_R01, 0x00000020),          /* power up cal DAC */
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_FILCAL_DDFS_CTL, 0x00100000), /* power up cal DAC clk, offset binary, no DDFS output shift */
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_tuner_calfilter_end[] =
   {
      BAST_SCRIPT_AND(BCHP_SDS_TUNER_LPF_R01, ~0x00000001),          /* unbypass LPF */
      BAST_SCRIPT_AND(BCHP_SDS_TUNER_PWRUP_R01, ~0x00000020),        /* power down cal DAC */
   #if (BCHP_VER >= BCHP_VER_B0)
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_MXFGA_R02, 0xBF03F052),       /* release phase shifter reset, 12 phase Q, 12 phase I */
   #else
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_MXFGA_R02, 0xBF03F012),       /* release phase shifter reset, 12 phase Q, 12 phase I */
   #endif
      BAST_SCRIPT_WRITE(BCHP_SDS_TUNER_FILCAL_DDFS_CTL, 0x00020000), /* power down cal DAC clk */
      BAST_SCRIPT_EXIT
   };
   
   /* trim filter bandwidth */
   /* if (cutoff < 5) cutoff = 5; */ /* do not limit, narrow BW for low symbol rates */
   if (cutoff > 40) cutoff = 40;
   
   /* save mxfga_r02 value */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_MXFGA_R02, &mxfga);
   
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_tuner_calfilter_start));
   
#ifdef BAST_TUNER_BYPASS_CALIBRATE_LPF
   /* manually set FGA and LPF bw as function of nominal desired cutoff freq for open loop */
   hChn->tunerFgaCal = (cutoff <= 25) ? tuner_fga_bw[0]:tuner_fga_bw[cutoff - 25];
   hChn->tunerLpfCal = tuner_lpf_bw[cutoff - 5];
#else
   /* calibrate FGA first with LPF bypassed */
   if (cutoff <= 24)
      hChn->tunerFgaCal = tuner_fga_bw[0]; /* use fixed nominal value if cutoff below 24 MHz */
   else
      BAST_g3_P_TunerCalFgaLpf(h, BAST_TunerCalType_eFga, cutoff, &(hChn->tunerFgaCal));

   /* calibrate LPF */
   BAST_g3_P_TunerCalFgaLpf(h, BAST_TunerCalType_eLpf, cutoff, &(hChn->tunerLpfCal));
   
   /* LPF cal failsafe to revert to nominal if calibrated value differs too much */
   hChn->tunerCtl &= ~BAST_G3_CONFIG_TUNER_CTL_LPF_FAILSAFE_STATUS;
   
   /* each LSB of SDS_TUNER_LPF_R01[11:6] is ~1MHz */
   if (abs((int32_t)(hChn->tunerLpfCal - cutoff)) > 3)
   {
      hChn->tunerLpfCal = tuner_lpf_bw[cutoff - 5];   /* reject calibrated value if too far off nominal, use nominal values */
      hChn->tunerFgaCal = (cutoff <= 25) ? tuner_fga_bw[0]:tuner_fga_bw[cutoff - 25];
      BAST_DEBUG_TUNER(BDBG_MSG(("FAILSAFE! lpf=%d fga=%d\n", hChn->tunerLpfCal, hChn->tunerFgaCal)));
      
      hChn->tunerCtl |= BAST_G3_CONFIG_TUNER_CTL_LPF_FAILSAFE_STATUS;
   }
#endif
   BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eFga, hChn->tunerFgaCal);
   BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eLpf, hChn->tunerLpfCal);
   
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_tuner_calfilter_end));
   
   /* restore mxfga_r02 value after calibration */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_MXFGA_R02, &mxfga);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerSetFgaLpf() - set FGA or LPF bandwidths
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetFgaLpf(BAST_ChannelHandle h, BAST_TunerCalType calType, uint8_t bwCode)
{
   if (calType == BAST_TunerCalType_eFga)
   {
      /* set FGA pole freq control */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_MXFGA_R01, ~0x7F000000, (bwCode << 24) & 0x7F000000);
   }
   else
   {
      /* set lpf bandwidth tuning code */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_LPF_R01, ~0x00000FC0, (bwCode << 6) & 0x0FC0);
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerCalFgaLpf() - calibrate FGA or LPF bandwidths
******************************************************************************/
BERR_Code BAST_g3_P_TunerCalFgaLpf(BAST_ChannelHandle h, BAST_TunerCalType calType, uint8_t cutoff, uint8_t *calBw)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#ifdef BAST_NORMALIZED_FILTER_CAL
   uint32_t p_tone_half, p_tone_cutoff, p_lower, p_upper;
#else
   uint32_t p_avg, p_6db, p_min_diff, p_diff;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
#endif
   uint8_t target, i;
   int8_t step = 1;
   
   if (calType == BAST_TunerCalType_eFga)
   {
      /* bypass LPF if calibrating FGA */
      BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_LPF_R01, 0x00000001);
      BAST_g3_P_TunerSetFgaLpf(h, BAST_TunerCalType_eLpf, 0x3F);  /* set LPF to max if calibrating FGA */
      target = (cutoff <= 25) ? tuner_fga_bw[0]:tuner_fga_bw[cutoff - 25];  /* lookup nominal value for FGA */
   }
   else
   {
      /* unbypass LPF if calibrating LPF */
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_LPF_R01, ~0x00000001);
      
      /* set filter to max setting for wide cutoff */
      if (cutoff >= 63)
      {
         *calBw = 0x3F;
         return BERR_SUCCESS;
      }
      
      /* comment: when cutoff is below 7Mhz (12Mbaud) , then sometimes p_half_avg is smaller than p_cutoff_avg. */
      /* bypass calibration for low symbol rates and return nominal */
      if (hChn->acqParams.symbolRate < 6000000)
      {
         if (hChn->acqParams.symbolRate < 3000000)
            *calBw = 1;
         else if (hChn->acqParams.symbolRate < 4000000)
            *calBw = 2;
         else
         {
        /* *calBw = tuner_lpf_bw[cutoff - 5];  */ /* lookup nominal value for LPF =calBw=3 --> 5Mhz */
            *calBw = 3; /* 2Mhz looks enough */
         }
         return BERR_SUCCESS;
      }
      target = cutoff;   /* approx 1 MHz/LSB for LPF */
   }

#ifdef BAST_NORMALIZED_FILTER_CAL
   /* set test tone to 1/2 of target cutoff freq and measure power of target filter bandwidth */
   BAST_g3_P_TunerSetFgaLpf(h, calType, target);
   BAST_g3_P_TunerSetTestTone(h, cutoff >> 1);
   BAST_CHK_RETCODE(BAST_g3_P_TunerGetAvgPower(h, &p_tone_half));
   
   /* stop calibration when p_tone_cutoff is between upper and lower threshold */
   p_upper = p_tone_half * hChn->tunerFilCalUpperThresh / 100;   /* threshold is defined in acq.c, 0~100%*/
   p_lower = p_tone_half * hChn->tunerFilCalLowerThresh / 100;   /* threshold is defined in acq.c, 0~100% */
  
   /* set test tone to target cutoff freq and measure power */
   BAST_g3_P_TunerSetTestTone(h, cutoff);
   BAST_CHK_RETCODE(BAST_g3_P_TunerGetAvgPower(h, &p_tone_cutoff));
   
   BAST_DEBUG_TUNER(BDBG_MSG(("normalized filter cal: target=%d", cutoff)));
   BAST_DEBUG_TUNER(BDBG_MSG(("(p_tone_half,p_tone_cutoff)=(%d, %d)", p_tone_half, p_tone_cutoff)));
   BAST_DEBUG_TUNER(BDBG_MSG(("(p_upper,p_lower)=(%d, %d)", p_upper, p_lower)));
   
   /* init calibration parameters */
   i = target;         /* start sweep from target */
   *calBw = target;   /* init calibrated value to target */
   
   if (p_tone_cutoff < p_lower)
   {
      BAST_DEBUG_TUNER(BDBG_MSG(("sweep up: p_tone_cutoff < p_lower")));
      step = 1;  /* sweep up for LPF if power below lower threshold */
   }
   else if (p_tone_cutoff > p_upper)
   {
      BAST_DEBUG_TUNER(BDBG_MSG(("sweep down: p_tone_cutoff > p_upper")));
      step = -1;   /* sweep down for LPF if power above upper threshold */
   }
      
   /* reverse sweep for FGA */
   if (calType == BAST_TunerCalType_eFga)
      step = -step;
   
   while ((p_tone_cutoff < p_lower) || (p_tone_cutoff > p_upper))
   {
      i += step;
      if ((i < 1) || (i > (calType ? 0x3F:0x7F)))
         break;   /* exit if out of bounds */
      
      /* set bw to sweep value and remeasure half tone power */
      BAST_g3_P_TunerSetFgaLpf(h, calType, i);
      BAST_g3_P_TunerSetTestTone(h, cutoff >> 1);
      BAST_CHK_RETCODE(BAST_g3_P_TunerGetAvgPower(h, &p_tone_half));
      
      /* calculate new thresholds */
      p_upper = p_tone_half * hChn->tunerFilCalUpperThresh / 100;   /* threshold is defined in acq.c, 0~100%*/
      p_lower = p_tone_half * hChn->tunerFilCalLowerThresh / 100;   /* threshold is defined in acq.c, 0~100% */
      
      /* set test tone to target cutoff freq and measure power */
      BAST_g3_P_TunerSetTestTone(h, cutoff);
      BAST_CHK_RETCODE(BAST_g3_P_TunerGetAvgPower(h, &p_tone_cutoff));
      
      BAST_DEBUG_TUNER(BDBG_MSG(("%d: power(half=%d,cutoff=%d)=(%d,%d) [upper,lower]=(%d,%d)", i, cutoff>>1, cutoff, p_tone_half, p_tone_cutoff, p_upper, p_lower)));
      *calBw = i;
   }
#else
   /* set test tone to 3/4 of target cutoff freq */
   BAST_g3_P_TunerSetTestTone(h, (cutoff * 3) >> 2);
   
   /* set max filter bandwidth and measure power */
   BAST_g3_P_TunerSetFgaLpf(h, calType, caltype ? 0x3F:0x00);
   BAST_CHK_RETCODE(BAST_g3_P_TunerGetAvgPower(h, &p_avg));
   BAST_MultU32U32(p_avg, 1073741824UL, &P_hi, &P_lo);     /* p_avg scaled 2^30 */
   BAST_DivU64U32(P_hi, P_lo, 2137321597UL, &Q_hi, &Q_lo); /* 3.981071706 * 2^29 */
   p_6db = (Q_lo + 1) >> 1;  /* p_6db = p_avg / 10^0.6 */
   
   /* set test tone to cutoff */
   BAST_g3_P_TunerSetTestTone(h, cutoff);
   
   /* init calibration parameters */
   p_min_diff = 0x7FFFFFFF;
   i = target;         /* start sweep from target */
   *calBw = target;   /* init calibrated value to target */
   
   /* set filter to target and measure power */
   BAST_g3_P_TunerSetFgaLpf(h, calType, target);
   BAST_CHK_RETCODE(BAST_g3_P_TunerGetAvgPower(h, &p_avg));
   
   if (p_avg < p_6db)
   {
      step = -1;  /* sweep down for FGA if power is below corner */
      p_min_diff = p_6db - p_avg;
   }
   else
   {
      step = 1;   /* sweep up for FGA if power is above corner */
      p_min_diff = p_avg - p_6db;
   }
      
   /* reverse sweep for LPF */
   if (calType == BAST_TunerCalType_eLpf)
      step = -step;
   
   /* sweep for optimal bandwidth code */
   while (1)
   {
      i += step;
      if ((i < 1) || (i > (calType ? 0x3F:0x7F)))
         break;   /* exit if out of bounds */
      
      /* set bw to sweep value and remeasure power */
      BAST_g3_P_TunerSetFgaLpf(h, calType, i);
      BAST_g3_P_TunerGetAvgPower(h, &p_avg);
      
      /* calculate new difference between power and p_6db */
      p_diff = abs((int32_t)(p_avg - p_6db));
      if (p_diff < p_min_diff)
      {
         p_min_diff = p_diff;
         *calBw = i;
      }
      if (p_avg > p_6db)
      {
         if ((calType == BAST_TunerCalType_eFga) && (step == -1))
            break;   /* exit if power is more than target and bandwidth increasing */
         else if ((calType == BAST_TunerCalType_eLpf) && (step == 1))
            break;   /* exit if power is more than target and bandwidth increasing  */
      }
      else if (p_avg < p_6db)
         break;   /* exit if power is less than target */
   }
#endif

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerSetTestTone() - generate test tone used for LPF calibration
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetTestTone(BAST_ChannelHandle h, uint8_t mhz)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
   
   BAST_MultU32U32(mhz * 65536, 1000000, &P_hi, &P_lo);        /* F_test * 1000000Hz * 2^16 */
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo); /* (F_test * 1000000Hz * 2^16) / F_sample */
   
   /* set filter cal FCW */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_FILCAL_DDFS_CTL, ~0x0000FFFF, Q_lo & 0xFFFF);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerGetAvgPower() - this function averages 128 samples of AGCLI
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetAvgPower(BAST_ChannelHandle h, uint32_t *avg)
{
   uint32_t val, acc = 0;
   uint8_t i;
   
   for (i = 128; i > 0; i--)
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_AGC_AGCLI, &val);  /* read AGC power estimate */
      acc += (val >> 12);
   }
   acc = (acc + 32) >> 7;  /* round and average power over 128 samples */
   *avg = acc;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerQuickTune1()
******************************************************************************/
BERR_Code BAST_g3_P_TunerQuickTune1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   bool bRefPllLocked, bMixPllLocked;
   BERR_Code retCode;
   
   while (1)
   {
      switch (hChn->funct_state)
      {
         case 0:
            BAST_g3_P_TunerUpdateActualTunerFreq(h);
            
            /* quick tune if change in freq is less than "thermal drift margin" = 2.5MHz(TBD) */
            if (abs((int32_t)(hChn->fullTuneFreq - hChn->actualTunerFreq)) < 2500000)
            {
               BAST_g3_P_TunerSetFcw(h);
               hChn->funct_state = 1;
               return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 30, BAST_g3_P_TunerQuickTune1);
            }
            else
            {
               /* do full tune if margin exceeded */
               hChn->funct_state = 2;
            }
            break;
            
         case 1:
            /* check mixer lock status after quick tune */
            BAST_g3_P_TunerGetLockStatus(h, &bRefPllLocked, &bMixPllLocked);
            if (!bMixPllLocked)
            {
               /* try full tune if quick tune didn't lock */
               hChn->funct_state = 2;
            }
            else
            {
               /* call post tune function if quick tune locked */
               if (hChn->postTuneFunct != NULL)
                  return hChn->postTuneFunct(h);
               else
                  return BERR_SUCCESS;
            }
            break;
            
         case 2:
            /* full tune - doesn't return */
            return BAST_g3_P_TunerSetFreq(h, hChn->postTuneFunct);
            
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;
      }
   }
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TunerConfigLna() - this function configures internal tuner LNA
******************************************************************************/
BERR_Code BAST_g3_P_TunerConfigLna(BAST_Handle h, BAST_TunerLnaSettings *pSettings)
{
   BAST_ChannelHandle hChn0 = (BAST_ChannelHandle)(h->pChannels[0]);
   BAST_g3_P_ChannelHandle *hChn0Impl = (BAST_g3_P_ChannelHandle *)(hChn0->pImpl);
   uint8_t sw_ctl = 0;
   uint32_t daisy_ctl;
   
   BDBG_ASSERT(pSettings);
   
   /* daisy cannot loop back to itself */
   if (pSettings->daisy == BAST_TunerLnaOutputConfig_eDaisy)
   {
      hChn0Impl->tunerLnaStatus = BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
   
   /* daisy input and daisy output cannot be enabled simultaneously */
   if ((pSettings->out0 == BAST_TunerLnaOutputConfig_eDaisy) || (pSettings->out1 == BAST_TunerLnaOutputConfig_eDaisy))
   {
      if (pSettings->daisy != BAST_TunerLnaOutputConfig_eOff)
      {
         hChn0Impl->tunerLnaStatus = BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }
   
   /* power up crossbar */
   BAST_g3_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_TUNER_PWRUP_COMMON_R01, ~0x0, 0x10);
   
   /* internal LNA crossbar always controlled from TUNER0 */
   BAST_g3_P_ReadRegister(hChn0, BCHP_SDS_TUNER_DAISY_R01, &daisy_ctl);
#if (BCHP_VER >= BCHP_VER_B0)
   daisy_ctl |= 0x00000001;   /* power down daisy output as default */
#else
   daisy_ctl |= 0x00001800;   /* power down daisy input output as default */
#endif
   hChn0Impl->tunerLnaSettings = *pSettings;
   hChn0Impl->tunerLnaStatus = 0;
   
   /* configure out0 path */
   switch (pSettings->out0)
   {
      case BAST_TunerLnaOutputConfig_eOff:
         break;
      case BAST_TunerLnaOutputConfig_eIn0:
         sw_ctl |= 0x01;   /* turn on input 0 to output 0 */
         break;
      case BAST_TunerLnaOutputConfig_eIn1:
      #if (BCHP_VER >= BCHP_VER_B0)
         /* input 1 to output 0 routing removed in B0 */
         hChn0Impl->tunerLnaStatus |= BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      #else
         sw_ctl |= 0x08;   /* turn on input 1 to output 0 */
      #endif
         break;
      case BAST_TunerLnaOutputConfig_eDaisy:
      #if (BCHP_VER >= BCHP_VER_B0)
         /* daisy input removed in B0 */
         hChn0Impl->tunerLnaStatus |= BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      #else
         sw_ctl |= 0x40;   /* turn on daisy input to output 0 */
         daisy_ctl &= ~0x00000800;  /* power up daisy input */
      #endif
         break;
      default:
         hChn0Impl->tunerLnaStatus |= BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
   }
   
   /* configure out1 path */
   switch (pSettings->out1)
   {
      case BAST_TunerLnaOutputConfig_eOff:
         break;
      case BAST_TunerLnaOutputConfig_eIn0:
         sw_ctl |= 0x02;   /* turn on input 0 to output 1 */
         break;
      case BAST_TunerLnaOutputConfig_eIn1:
         sw_ctl |= 0x10;   /* turn on input 1 to output 1 */
         break;
      case BAST_TunerLnaOutputConfig_eDaisy:
      #if (BCHP_VER >= BCHP_VER_B0)
         /* daisy input removed in B0 */
         hChn0Impl->tunerLnaStatus |= BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      #else
         sw_ctl |= 0x80;   /* turn on daisy input to output 1 */
         daisy_ctl &= ~0x00000800;  /* power up daisy input */
      #endif
         break;
      default:
         hChn0Impl->tunerLnaStatus |= BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
   }
   
   /* configure daisy path */
   switch (pSettings->daisy)
   {
      case BAST_TunerLnaOutputConfig_eOff:
         break;
      case BAST_TunerLnaOutputConfig_eIn0:
         sw_ctl |= 0x04;   /* turn on input 0 to daisy output */
      #if (BCHP_VER >= BCHP_VER_B0)
         daisy_ctl &= ~0x00000001;  /* power up daisy output */
      #else
         daisy_ctl &= ~0x00001000;  /* power up daisy output */
      #endif
         break;
      case BAST_TunerLnaOutputConfig_eIn1:
      #if (BCHP_VER >= BCHP_VER_B0)
         /* input 1 to daisy routing removed in B0 */
         hChn0Impl->tunerLnaStatus |= BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      #else
         sw_ctl |= 0x20;   /* turn on input 1 to daisy output */
         daisy_ctl &= ~0x00001000;  /* power up daisy output */
      #endif
         break;
      default:
         hChn0Impl->tunerLnaStatus |= BAST_TUNER_LNA_STATUS_ERR_INVALID_CFG;
   }
   
   /* add crossbar settings */
   daisy_ctl &= ~0x00001FE;
   daisy_ctl |= (sw_ctl << 1);
   BAST_g3_P_WriteRegister(hChn0, BCHP_SDS_TUNER_DAISY_R01, &daisy_ctl);
   
   hChn0Impl->tunerLnaStatus |= BAST_TUNER_LNA_STATUS_INITIALIZED;
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetTunerLnaStatus() - this function gets status of internal tuner LNA
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetLnaStatus(BAST_ChannelHandle h, BAST_TunerLnaStatus *pStatus)
{
   /* BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl; */
   BAST_ChannelHandle hChn0 = (BAST_ChannelHandle)(h->pDevice->pChannels[0]);
   BAST_g3_P_ChannelHandle *hChn0Impl = (BAST_g3_P_ChannelHandle *)(hChn0->pImpl);
   uint32_t val;
   
   pStatus->status = hChn0Impl->tunerLnaStatus;
   pStatus->outCfg = (h->channel == 0) ? hChn0Impl->tunerLnaSettings.out0 : hChn0Impl->tunerLnaSettings.out1;
   
   /* read LNA AGC */
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_eRfagc, 0x5, &val);
   pStatus->lnaAgc = val;
   
   /* read BB AGC */
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_eBbagc, 0x5, &val);
   pStatus->bbAgc = val;
   
   return BERR_SUCCESS;
}


#if ((BCHP_CHIP == 7344) || (BCHP_CHIP == 7346)) && (BCHP_VER == BCHP_VER_A0)
/******************************************************************************
 BAST_g3_P_TunerToggleDcoClock() - emulate DCO clock
******************************************************************************/
BERR_Code BAST_g3_P_TunerToggleDcoClock(BAST_ChannelHandle h)
{
   uint32_t i = 0;
#if 0   
   uint32_t val, addr, status;
#endif

#if 0
   /* settings to bring out worst case AGC - use to optimize DCO clocking time below... */
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x0, 0x0000421A);      /* freeze RFAGC */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_RFAGC_R02, ~0x007E0000, 0x00000000);  /* min gain agc_code_bypass = 0 */
   
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x0, 0x0020423D);      /* freeze BBAGC */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_BBAGC_R02, ~0x007E0000, 0x007E0000);  /* max gain agc_code_bypass = 0x3F */

   val = 0x00041E1E;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_AGC_AGCCTL, &val);   /* freeze IFAGC */
   val = 0xD8000000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_AGC_AII, &val);      /* IFAGC max gain */
#endif

   BAST_DEBUG_TUNER(BDBG_MSG(("toggle DCOs...")));
   
   /* toggle DCO power up once */
   BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_PWRUP_R01, 0x00000018);
   BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PWRUP_R01, ~0x00000018);
   BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_PWRUP_R01, 0x00000018);
   
   /* toggle clk div select to force glitches */
#if !defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD) /* TODO_ILEE */
   while (i++ < 9999)
   {
      BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_PREDCO_R01, 0x00010000);
      BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_POSTDCO_R01, 0x00010000);
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PREDCO_R01, ~0x00010000);
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_POSTDCO_R01, ~0x00010000);
   }
#endif
   
#if 0
   /* freeze preDCOi loop */
   printf("freeze preDCOi\n");
   val = 0x07334018; /* freeze DCO loop */
   addr = 0;   /* write enable, set write address zero */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_PREDCOI_R01 + 0x8, &val);
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_PREDCOI_R01, ~0x00000307, 0x00000200 | (addr & 0x7));
   do {
      /* wait for write done */
      BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_PREDCO_R01, 0x00010000);
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PREDCO_R01, ~0x00010000);
      BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_PREDCOI_R01 + 0x4, &status);
   }
   while ((status & 0x2) == 0);
   BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PREDCOI_R01, ~0x00000300); /* disable write */
   
   /* freeze preDCOq loop */
   printf("freeze preDCOq\n");
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_PREDCOQ_R01 + 0x8, &val);
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_PREDCOQ_R01, ~0x00000307, 0x00000200 | (addr & 0x7));
   do {
      /* wait for write done */
      BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_PREDCO_R01, 0x00010000);
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PREDCO_R01, ~0x00010000);
      BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_PREDCOQ_R01 + 0x4, &status);
   }
   while ((status & 0x2) == 0);
   BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_PREDCOQ_R01, ~0x00000300); /* disable write */
   
   /* freeze postDCOi loop */
   printf("freeze postDCOi\n");
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_POSTDCOI_R01 + 0x8, &val);
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_POSTDCOI_R01, ~0x00000307, 0x00000200 | (addr & 0x7));
   do {
      /* wait for write done */
      BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_POSTDCO_R01, 0x00010000);
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_POSTDCO_R01, ~0x00010000);
      BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_POSTDCOI_R01 + 0x4, &status);
   }
   while ((status & 0x2) == 0);
   BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_POSTDCOI_R01, ~0x00000300); /* disable write */
   
   /* freeze postDCOq loop */
   printf("freeze postDCOq\n");
   BAST_g3_P_WriteRegister(h, BCHP_SDS_TUNER_POSTDCOQ_R01 + 0x8, &val);
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_POSTDCOQ_R01, ~0x00000307, 0x00000200 | (addr & 0x7));
   do {
      /* wait for write done */
      BAST_g3_P_OrRegister(h, BCHP_SDS_TUNER_POSTDCO_R01, 0x00010000);
      BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_POSTDCO_R01, ~0x00010000);
      BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_POSTDCOQ_R01 + 0x4, &status);
   }
   while ((status & 0x2) == 0);
   BAST_g3_P_AndRegister(h, BCHP_SDS_TUNER_POSTDCOQ_R01, ~0x00000300); /* disable write */
#endif
   
   return BERR_SUCCESS;
}
#endif


#if 0
/******************************************************************************
 BAST_g3_P_TunerTestDcoIndirects()
******************************************************************************/
BERR_Code BAST_g3_P_TunerTestDcoIndirects(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t i, val;
   
   for (i = 1; i < 9; i++)
   {
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0, i << 28 | i << 20 | i << 12 |i << 4);
      BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0, &val);
      printf("predco_i0=%08X\n", val);
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0, i << 28 | i << 20 | i << 12 |i << 4);
      BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0, &val);
      printf("predco_q0=%08X\n", val);
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0, i << 28 | i << 20 | i << 12 |i << 4);
      BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0, &val);
      printf("postdco_i0=%08X\n", val);
      BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0, i << 28 | i << 20 | i << 12 |i << 4);
      BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0, &val);
      printf("postdco_q0=%08X\n", val);
   }
   
   return BERR_SUCCESS;
}
#endif


/******************************************************************************
 BAST_g3_P_TunerRunDco()
******************************************************************************/
BERR_Code BAST_g3_P_TunerRunDco(BAST_ChannelHandle h, BAST_TimerSelect t, uint16_t runtime)
{
   uint32_t val;
   
   /* unfreeze all DCO loops */
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x0, (val & ~0x08));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x0, (val & ~0x08));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x0, (val & ~0x08));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x0, (val & ~0x08));
   
   /* TBD run DCO for a while.  Don't know how long it has to run. Need to figure this out from experiment */
   return BAST_g3_P_EnableTimer(h, t, runtime, BAST_g3_P_TunerRunDco1);
}


/******************************************************************************
 BAST_g3_P_TunerRunDco1()
******************************************************************************/
BERR_Code BAST_g3_P_TunerRunDco1(BAST_ChannelHandle h)
{
   uint32_t val;
   
   /* freeze all DCO loops */
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoI, 0x0, (val | 0x08));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePreDcoQ, 0x0, (val | 0x08));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoI, 0x0, (val | 0x08));
   BAST_g3_P_TunerIndirectRead(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x0, &val);
   BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_ePostDcoQ, 0x0, (val | 0x08));
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerGetActualLOFreq()
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetActualLOFreq(BAST_ChannelHandle h, uint32_t *pFreq)
{
   uint32_t Fddfs, L6, fcw, Q_hi, Q_lo, P_hi, P_lo, i;

   BAST_g3_P_TunerGetFddfs(h, &Fddfs);
   BAST_g3_P_TunerGetLoDivider(h, &L6);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_TUNER_LODDFS_R02, &fcw);
   BAST_MultU32U32(fcw, Fddfs, &P_hi, &P_lo);
   for (i = 1; i < 32; i++)
   {
      BAST_DivU64U32(P_hi, P_lo, (1<<i), &Q_hi, &Q_lo);
      if ((Q_hi == 0) && ((Q_lo & 0x80000000) == 0))
         break;      
   }
   BAST_MultU32U32(Q_lo, L6, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 64, &Q_hi, &Q_lo);
   BAST_DivU64U32(Q_hi, Q_lo, 1<<(32-i), &Q_hi, pFreq);
   return BERR_SUCCESS;
}

