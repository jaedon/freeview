/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2_priv_qpsk.c $
 * $brcm_Revision: Hydra_Software_Devel/128 $
 * $brcm_Date: 10/10/12 4:23p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2_priv_qpsk.c $
 * 
 * Hydra_Software_Devel/128   10/10/12 4:23p enavarro
 * SWSATFE-218: implement manual spectral inversion scan on external tuner
 * 
 * Hydra_Software_Devel/127   9/6/12 7:22p ronchan
 * SWSATFE-214: fix compile errors
 * 
 * Hydra_Software_Devel/126   9/6/12 6:17p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/125   9/4/12 5:04p ckpark
 * SWSATFE-215: increased checkLockInterval for external tuner
 * 
 * Hydra_Software_Devel/124   5/22/12 3:01p ckpark
 * SWSATFE-191: adjusted bw and delay values for VerifyLock()
 * 
 * Hydra_Software_Devel/123   5/21/12 4:38p ckpark
 * SWSATFE-191: limit maxSweep to 1 in VerifyLock(); scale down fli sum in
 * VerifyLock() to prevent overflow; don't relock viterbi in
 * QpskLockViterbi() if VerifyLock() has already been done
 * 
 * Hydra_Software_Devel/122   5/21/12 1:36p ckpark
 * SWSATFE-191: fixed initial direction of lock points in VerifyLock();
 * OQPSK changes
 * 
 * Hydra_Software_Devel/121   5/10/12 3:04p ckpark
 * SWSATFE-191: retune on reacquire for oqpsk
 * 
 * Hydra_Software_Devel/120   5/10/12 2:33p ckpark
 * SWSATFE-191: rollback change in BAST_73xx_P_QpskReacquire()
 * 
 * Hydra_Software_Devel/119   5/10/12 10:07a ckpark
 * SWSATFE-191: oqpsk changes for NTCH_CTRL; limit maxSweep to 2 in
 * VerifyLock(); increase searchRangeFli in VerifyLock to
 * searchRange+Fb/16; dcii changes in VerifyLock(); check for lock before
 * reacquiring
 * 
 * Hydra_Software_Devel/118   10/5/11 9:46a enavarro
 * SWSATFE-150: spur canceller not supported in 73xx AST
 * 
 * Hydra_Software_Devel/117   11/30/10 9:51a ckpark
 * SWSATFE-91: reacquire if bForceReacq is set
 * 
 * Hydra_Software_Devel/116   10/15/10 10:14a ckpark
 * SWSATFE-85: Network tuner 20Mbaud, acquisition BW is adjusted
 * 
 * Hydra_Software_Devel/115   5/28/10 5:38p ronchan
 * SW7335-732: removed unnecessary conditions
 * 
 * Hydra_Software_Devel/114   5/27/10 6:01p ronchan
 * SW7335-732: fixed coverity warning
 * 
 * Hydra_Software_Devel/113   2/2/10 11:29a ckpark
 * SWSATFE-32: dont enable PGA hysteresis
 * 
 * Hydra_Software_Devel/112   1/20/10 11:14a ckpark
 * SWSATFE-28: Viterbi lock time is increased from 25000 to 35000 to
 * improve slow acquisition under clean channel
 * 
 * Hydra_Software_Devel/111   1/5/10 5:22p enavarro
 * SW7335-639: got DCII scan to work
 * 
 * Hydra_Software_Devel/110   12/15/09 10:07a ckpark
 * SWSATFE-21: Adjusted carrier delay (below 4M) and carrier_acq_bw and
 * trk_bw (below 2M) for AMC tuner
 * 
 * Hydra_Software_Devel/109   12/10/09 3:37p ckpark
 * SWSATFE-21: adjusted carrier delay, acq/tracking bw in
 * BAST_73xx_P_QpskVerifyLock
 * 
 * Hydra_Software_Devel/108   12/8/09 10:40a enavarro
 * SWSATFE-21: set AGC delay to 4 msecs
 * 
 * Hydra_Software_Devel/107   12/8/09 10:04a enavarro
 * SWSATFE-21: add delay after enabling AGC for external tuner
 * 
 * Hydra_Software_Devel/106   12/8/09 9:17a ckpark
 * SWSATFE-9: remove DCO reset for external tuner
 * 
 * Hydra_Software_Devel/105   12/4/09 4:29p enavarro
 * SWSATFE-20: fixed bug where reacquisition timer was not enabled on
 * NotLock interrupt; set BAST_LOCK_INDICATION_FILTER_TIME_MAX to 1e6
 * usecs
 * 
 * Hydra_Software_Devel/104   11/23/09 3:16p ckpark
 * SWSATFE-17: For Default mode, DVB-S high symbol rates (>30Mbaud) has
 * EURO mode PLC setting.
 * 
 * Hydra_Software_Devel/103   11/11/09 10:21a ckpark
 * SWSATFE-12: retune every other reacquisition
 * 
 * Hydra_Software_Devel/102   11/6/09 4:49p ckpark
 * SWSATFE-11: added AGC convergence delay after setting acquisition AGC
 * bandwidth; changed acquisition AGC bandwidth to be same as in LDPC;
 * dont do 3-stage AGC processing during DFT freq estimate
 * 
 * Hydra_Software_Devel/101   11/3/09 3:39p ckpark
 * SWSATFE-9: reset DCO for external tuner at start of acquisition;
 * increase carrierDelay for low baud rate in
 * BAST_73xx_P_QpskLockViterbi()
 * 
 * Hydra_Software_Devel/100   10/30/09 9:49a enavarro
 * SWSATFE-9: set DCOCTL1 to 0x00 for external tuner
 * 
 * Hydra_Software_Devel/99   10/1/09 9:55a enavarro
 * SWSATFE-5: adjusted carrierDelay in BAST_73xx_P_QpskLockViterbi() for
 * low baud rates
 * 
 * Hydra_Software_Devel/98   9/30/09 10:26a ckpark
 * HW4506-79: Legacy carrier delay *=2 for 5M ~10M
 * 
 * Hydra_Software_Devel/97   9/14/09 11:00a enavarro
 * HW4506-79: put back VerifyLock algorithm because it is needed for
 * external tuner
 * 
 * Hydra_Software_Devel/96   9/10/09 9:28a enavarro
 * SW7325-594: moved functions used for fixed point arithmetic to common
 * location
 * 
 * Hydra_Software_Devel/95   8/27/09 5:06p enavarro
 * HW4506-79: changed code to use DFT freq scan instead of
 * AcquireTimeLock; reworked VerifyLock for faster acquisition time
 * 
 * Hydra_Software_Devel/94   8/10/09 11:00a ronchan
 * PR 55235: add support for 7340 and 7342
 * 
 * Hydra_Software_Devel/93   7/6/09 5:22p ronchan
 * PR 56602: reacquire if OPLL fails to converge
 * 
 * Hydra_Software_Devel/92   7/6/09 9:41a enavarro
 * PR 56602: reset OIF before programming OPLL
 * 
 * Hydra_Software_Devel/91   6/26/09 11:07a ckpark
 * PR 56263: put in FLI compensation for MB2 DCO offset
 * 
 * Hydra_Software_Devel/90   6/23/09 3:53p ckpark
 * PR 56263: apply 10KHz offset in tuner for DCO avoidance
 * 
 * Hydra_Software_Devel/89   6/22/09 5:37p ckpark
 * PR 56140: dont transfer freq offset to LO for external tuner
 * 
 * Hydra_Software_Devel/88   6/19/09 3:08p enavarro
 * PR 56165: fixed compiler warning
 * 
 * Hydra_Software_Devel/87   6/19/09 2:28p enavarro
 * PR 56165: use BAST_73xx_P_SetFlifOffset() instead of writing to FLIF
 * register directly
 * 
 * Hydra_Software_Devel/86   6/18/09 2:48p enavarro
 * PR 56135: call BAST_73xx_P_BlindReacquire() when reacquiring in blind
 * scan mode
 * 
 * Hydra_Software_Devel/85   6/9/09 2:43p ckpark
 * PR 54801: adjusted PLC
 * 
 * Hydra_Software_Devel/84   5/26/09 3:45p enavarro
 * PR 55047: set tracking tuner LPF bw before transferring freq offset to
 * LO
 * 
 * Hydra_Software_Devel/83   4/20/09 2:05p enavarro
 * PR 37769: added counters for all interrupts
 * 
 * Hydra_Software_Devel/82   4/16/09 10:53a enavarro
 * PR 51107: disable LO loop tracking
 * 
 * Hydra_Software_Devel/81   4/15/09 10:38a enavarro
 * PR 51107: initialize freqTransferInt
 * 
 * Hydra_Software_Devel/80   4/13/09 10:59a ronchan
 * PR 54006: use DisableChannelInterrupts_isr() for ISR context
 * 
 * Hydra_Software_Devel/79   3/20/09 4:26p enavarro
 * PR 52954: set verify_lock search range to hDev->searchRange if
 * acquire_time_lock not performed
 * 
 * Hydra_Software_Devel/78   3/12/09 4:25p enavarro
 * PR 51665: update to latest tuner mixpll retry algorithm
 * 
 * Hydra_Software_Devel/77   3/2/09 3:46p enavarro
 * PR 51665: updated tuneMixStatus bits
 * 
 * Hydra_Software_Devel/76   2/26/09 2:24p enavarro
 * PR 51665: implemented revised algorithm for tuner mixpll locking
 * 
 * Hydra_Software_Devel/75   2/18/09 3:00p ckpark
 * PR 52255: DTV 6/7 final PLC is narrowed to improve QEF
 * 
 * Hydra_Software_Devel/74   2/12/09 2:10p enavarro
 * PR 50888: fixed DVB 1/2 long acquisition time caused by wrong PLC
 * bandwidth in EURO mode
 * 
 * Hydra_Software_Devel/73   1/29/09 6:35p ronchan
 * PR 51629: added support for 7336
 * 
 * Hydra_Software_Devel/72   1/29/09 6:33p ronchan
 * PR 51203: do 3-stage AGC periodically only for internal tuners
 * 
 * Hydra_Software_Devel/70   1/19/09 4:06p ckpark
 * PR 51203: do 3-stage AGC periodically
 * 
 * Hydra_Software_Devel/69   1/12/09 10:54a enavarro
 * PR 49705: propagated changes from BCM4506
 * 
 * Hydra_Software_Devel/68   1/8/09 4:12p enavarro
 * PR 50925: reworked acquisition time calculation
 * 
 * Hydra_Software_Devel/67   1/7/09 4:07p enavarro
 * PR 50899: fixed bug in DVB scan mode where OPLL didn't get reprogrammed
 * after locking to a different code rate
 * 
 * Hydra_Software_Devel/66   12/22/08 2:34p ronchan
 * PR 50635: override default external tuner AGC thresholds
 * 
 * Hydra_Software_Devel/65   11/17/08 10:32a ckpark
 * PR 37769: fix Legacy 30Mbaud (acquire time lock is bypassed) acq beyond
 * 5Mhz  serach range in Verify Lock,
 * 
 * Hydra_Software_Devel/64   10/31/08 3:23p enavarro
 * PR 37769: removed TONE_SEARCH_MODE code
 * 
 * Hydra_Software_Devel/63   10/24/08 11:06a ronchan
 * PR 37769: fixed compile warning
 * 
 * Hydra_Software_Devel/62   10/23/08 10:41a enavarro
 * PR 37881: initialize hChn->actualMode before calling
 * BAST_73xx_P_QpskSetOpll()
 * 
 * Hydra_Software_Devel/61   10/23/08 9:51a enavarro
 * PR 37769: fix BAST_73xx_P_QpskSetOpll() for DigicipherII
 * 
 * Hydra_Software_Devel/60   10/3/08 10:11a enavarro
 * PR 37769: implemented EnableSpurCanceller API function
 * 
 * Hydra_Software_Devel/59   10/1/08 3:51p enavarro
 * PR 37769: added STUFF_BYTES config parameter
 * 
 * Hydra_Software_Devel/58   10/1/08 2:23p enavarro
 * PR 37769: disable notch filter settings from previous acquisition;
 * added BAST_73xx_P_QpskUpdateErrorCounters(); dont log any rs errors
 * during acquisition
 * 
 * Hydra_Software_Devel/57   9/17/08 10:59a ckpark
 * PR 37769: dont enable LO tracking for < 950MHz
 * 
 * Hydra_Software_Devel/56   8/14/08 4:00p enavarro
 * PR 37769: set DCOCTL1=0x20 for external tuner
 * 
 * Hydra_Software_Devel/55   8/5/08 3:45p enavarro
 * PR 37769: reset HP on start of acquisition
 * 
 * Hydra_Software_Devel/54   7/21/08 4:49p enavarro
 * PR 37769: call BAST_73xx_P_GetActualMode() before going into monitor
 * state
 * 
 * Hydra_Software_Devel/53   7/14/08 3:54p enavarro
 * PR 37769: fixed bug in disabling frequency detector
 * 
 * Hydra_Software_Devel/52   7/9/08 4:52p ronchan
 * PR 37769: don't call ResetStatusIndicators() if status events excluded
 * 
 * Hydra_Software_Devel/51   7/7/08 3:15p ckpark
 * PR 37769: changed dco bw in legacy mode to match ldpc mode, do NOT set
 * dcoctl1 as 0x20 (using 3rd DCO, which is for ADC DC canceller) and
 * restore to 0x31.
 * 
 * Hydra_Software_Devel/50   7/3/08 10:33a ckpark
 * PR 37769: initialize SDS_CLFBCTL for 7325-B0
 * 
 * Hydra_Software_Devel/49   7/3/08 9:54a enavarro
 * PR 37769: reworked BAST_73xx_P_QpskAcquire0()
 * 
 * Hydra_Software_Devel/48   7/1/08 12:11p enavarro
 * PR 37769: initialize HPCTRL6; removed unnecessary write to CGPDWN; do
 * 3-stage AGC throughout acquisition; adjusted baud loop damping factor
 * for 17-23 Msym/s; adjusted tracking carrier BW for DTV modes
 * 
 * Hydra_Software_Devel/47   6/26/08 3:36p enavarro
 * PR 37881: set FLIF for splitter mode
 * 
 * Hydra_Software_Devel/46   6/20/08 11:11a enavarro
 * PR 37769: added BAST_73xx_ResetStatusIndicators()
 * 
 * Hydra_Software_Devel/45   6/19/08 2:34p enavarro
 * PR 37769: added support for status events
 * 
 * Hydra_Software_Devel/44   6/17/08 11:22a enavarro
 * PR 37769: increase carrierDelay in VerifyLock for warm start
 * 
 * Hydra_Software_Devel/43   6/16/08 2:47p enavarro
 * PR 37769: changes to VerifyLock() for warm start acquisition
 * 
 * Hydra_Software_Devel/42   6/13/08 5:19p enavarro
 * PR 43276: call BAST_73xx_P_TransferFreqOffset() every 100 msecs in
 * monitor state; adjusted tracking BW
 * 
 * Hydra_Software_Devel/41   6/10/08 5:16p enavarro
 * PR 37769: added support for measuring acq time
 * 
 * Hydra_Software_Devel/40   6/3/08 3:37p enavarro
 * PR 37769: updated final carrier BW
 * 
 * Hydra_Software_Devel/39   6/2/08 2:15p enavarro
 * PR 37769: split up BAST_73xx_P_QpskAcquire0() because part of it is
 * going to be used for peak scan
 * 
 * Hydra_Software_Devel/38   5/23/08 11:19a ronchan
 * PR 37769: added option to compile out splitter mode
 * 
 * Hydra_Software_Devel/36   5/9/08 6:04p enavarro
 * PR 37769: added support for splitter mode; put tuner filter on wide
 * setting on reacquire
 * 
 * Hydra_Software_Devel/35   5/6/08 10:29a enavarro
 * PR 37769: do acquire_time_lock for <= 30MBaud legacy mode; added
 * maxSweep parameter in verify_lock; added
 * BAST_73xx_P_QpskCheckCodeRateScan()
 * 
 * Hydra_Software_Devel/34   4/9/08 2:18p enavarro
 * PR 37769: return BERR_Code in BAST_73xx_P_QpskDelayReacquire()
 * 
 * Hydra_Software_Devel/33   4/7/08 3:26p ckpark
 * PR 37769: Legacy acquisition BW is adjusted
 * 
 * Hydra_Software_Devel/32   3/28/08 11:06a ckpark
 * PR 37769: PLC BW for 5M, DVB gets back to org. setting (Chan)
 * 
 * Hydra_Software_Devel/31   3/17/08 2:10p enavarro
 * PR 37769: tweaked lock interrupt filtering
 * 
 * Hydra_Software_Devel/30   3/12/08 5:21p enavarro
 * PR 37769: retune with FLI offset for optimal popcap setting; added
 * reacqDelay before reacquiring
 * 
 * Hydra_Software_Devel/29   3/12/08 1:41p enavarro
 * PR 37769: fixed issue with not getting lock interrupt because it was
 * cleared before enabling the irq
 * 
 * Hydra_Software_Devel/28   2/19/08 6:39p jrubio
 * PR38507: fix compile false warnings
 * 
 * Hydra_Software_Devel/27   2/12/08 9:52a ckpark
 * PR 37769: changed carrierDelay in VerifyLock(); changed tracking PLC
 * setting
 * 
 * Hydra_Software_Devel/26   2/4/08 5:06p ckpark
 * PR 37769: use baud clock for acquisition delays
 * 
 * Hydra_Software_Devel/25   2/4/08 4:12p enavarro
 * PR 37769: do carrier offset checking after LO tracking is enabled;
 * updated BAST_73xx_P_QpskEnableLoTracking()
 * 
 * Hydra_Software_Devel/24   1/31/08 2:16p enavarro
 * PR 37769: changed FLI computation in BAST_73xx_P_QpskEnableLoTracking()
 * 
 * Hydra_Software_Devel/23   1/25/08 11:06a enavarro
 * PR 37769: use baud clock timer in BAST_73xx_P_QpskSpinvScan1()
 * 
 * Hydra_Software_Devel/22   1/24/08 4:05p ckpark
 * PR 37769: lowered tracking DCO BW; decreased carrierDelay values in
 * VerifyLock()
 * 
 * Hydra_Software_Devel/21   1/24/08 11:14a enavarro
 * PR 37769: fixed BAST_73xx_P_QpskReacquire() for external tuner
 * 
 * Hydra_Software_Devel/20   1/22/08 3:36p enavarro
 * PR 37769: fixed SDS_FLI calculation in
 * BAST_73xx_P_QpskEnableLoTracking()
 * 
 * Hydra_Software_Devel/19   1/22/08 11:27a enavarro
 * PR 37769: fixed bug in computing SDS_FLI in
 * BAST_73xx_P_QpskEnableLoTracking()
 * 
 * Hydra_Software_Devel/18   1/18/08 11:37a enavarro
 * PR 37769: changes to auto reacquisition
 * 
 * Hydra_Software_Devel/17   1/15/08 3:45p enavarro
 * PR 37769: change SDS_AGI and SDS_AGT for ext tuner
 * 
 * Hydra_Software_Devel/16   1/14/08 5:37p enavarro
 * PR 37769: changes for ext tuner acquisition
 * 
 * Hydra_Software_Devel/15   1/14/08 11:46a enavarro
 * PR 37769: enable lo tracking after QpskNarrowBw()
 * 
 * Hydra_Software_Devel/14   12/19/07 11:33a enavarro
 * PR 37769: added support for blind scan
 * 
 * Hydra_Software_Devel/13   12/18/07 11:50a enavarro
 * PR 37769: use BINT callback ISRs
 * 
 * Hydra_Software_Devel/12   12/17/07 11:27a enavarro
 * PR 37769: tweaked lock event filtering
 * 
 * Hydra_Software_Devel/11   12/14/07 2:36p ckpark
 * PR 37769: legacy qpsk acquisition tweaks
 * 
 * Hydra_Software_Devel/10   12/13/07 4:21p ckpark
 * PR 37769: lab checkout
 * 
 * Hydra_Software_Devel/9   12/13/07 11:03a enavarro
 * PR 37769: clear lock interrupts prior to enabling the callback
 * 
 * Hydra_Software_Devel/8   12/12/07 5:59p enavarro
 * PR 37769: tweaked lock interrupt filtering
 * 
 * Hydra_Software_Devel/7   12/12/07 3:56p ckpark
 * PR 37769: tweak legacy qpsk acquisition
 * 
 * Hydra_Software_Devel/6   12/10/07 2:57p ronchan
 * PR 33769: refined lock indication filtering
 * 
 * Hydra_Software_Devel/5   12/10/07 9:42a ronchan
 * PR 37769: added lock indication filtering
 * 
 * Hydra_Software_Devel/4   12/5/07 4:48p agin
 * PR37869: Integrate SDS into 97325 reference software.
 * 
 * Hydra_Software_Devel/3   12/5/07 11:09a ckpark
 * PR 33769: fixed bug manual scan of DTV 1/2 in DTV scan mode; removed
 * BAST_73xx_P_QpskSetLockInterrupts()
 * 
 * Hydra_Software_Devel/2   12/5/07 9:01a ckpark
 * PR 37769: improved DTV 1/2 acquisition time in DTV scan mode
 * 
 * Hydra_Software_Devel/1   11/21/07 11:31a ronchan
 * PR 32395: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2_priv.h"

BDBG_MODULE(bast_g2_priv_qpsk);
/* #define VERIFY_LOCK_DEBUG */
/* #define BAST_DISABLE_INTERRUPT_FILTERING */

/* bit definitions for dtvScanState */
#define BAST_G2_DTV_SCAN_ON  0x80
#define BAST_G2_DTV_SCAN_1_2 0x01
#define BAST_G2_DTV_SCAN_OFF 0x00

#define BAST_LOCK_INDICATION_FILTER_TIME 5000 
#define BAST_LOCK_INDICATION_FILTER_TIME_MAX 1500000 
#define BAST_LOCK_INDICATION_INCREMENT 5000
#define BAST_LOCK_INDICATION_RAMP_INIT -2

/* local functions */
void BAST_g2_P_QpskInitializeLoopParameters(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetEqmode(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetMode(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetOqpsk1(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetOqpsk2(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetClctl2(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetClctl3(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetFinalBlBw(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetFinalFlBw(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskAcquire0a(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskAcquire1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskAcquire2(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskAcquire3(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskAcquire4(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskDelayReacquire(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskLockViterbi(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskVerifyLock(BAST_ChannelHandle h);
bool BAST_g2_P_CarrierOffsetOutOfRange(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskSpinvScan(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskSpinvScan1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskNarrowBw(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskNarrowBw1(BAST_ChannelHandle h);
void BAST_g2_P_QpskSetOpll(BAST_ChannelHandle h);
bool BAST_g2_P_QpskCheckCodeRateScan(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskMonitor(BAST_ChannelHandle h);
BAST_Mode BAST_g2_P_QpskGetActualMode(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskTransferFreqOffset(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskTransferFreqOffset0(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskConvergeAgc(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskReacquire(BAST_ChannelHandle h);


/******************************************************************************
 BAST_g2_P_QpskAcquire() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_QpskAcquire(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   hChn->bForceReacq = false;
   
   if (hChn->acqParams.mode == BAST_Mode_eDss_scan)
      hChn->dtvScanState = BAST_G2_DTV_SCAN_ON;
   else
      hChn->dtvScanState = BAST_G2_DTV_SCAN_OFF; 
   hChn->actualMode = hChn->acqParams.mode;

   hChn->spinvState &= BAST_SPINV_STATE_INV;   
   if (hChn->bExternalTuner)
   {
      if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SPINV_IQ_SCAN) == BAST_ACQSETTINGS_SPINV_IQ_SCAN)
         hChn->spinvState |= BAST_SPINV_STATE_SCAN_ENABLED;
      BAST_g2_P_SetSymbolRate(h);
   }     
             
   BAST_g2_P_QpskAcquire0(h);
   
   if (hChn->bExternalTuner == false)
   {
      hChn->funct_state = 10;
      hChn->passFunct = BAST_g2_P_QpskAcquire0a;
      return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 200, BAST_g2_P_QpskConvergeAgc);
   }
   else
   {
      /* enable AGC */
      val = 0x08;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_AGICTL, &val);		
      BAST_g2_P_WriteRegister(h, BCHP_SDS_AGTCTL, &val);  
      return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 4000, BAST_g2_P_QpskAcquire0a);
   }
}


/******************************************************************************
 BAST_g2_P_QpskConvergeAgc()
******************************************************************************/
BERR_Code BAST_g2_P_QpskConvergeAgc(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   BAST_g2_P_3StageAgc_isr(h, 0);
   hChn->funct_state--;
   if (hChn->funct_state)
      return  BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 200, BAST_g2_P_QpskConvergeAgc);
   else
      return hChn->passFunct(h);
}


/******************************************************************************
 BAST_g2_P_QpskAcquire0()
******************************************************************************/
BERR_Code BAST_g2_P_QpskAcquire0(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   static const uint32_t script_qpsk_0[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_FIRQSTS5, 0x00003C00), /* disable 3-stage agc interrupts */
#if ((BCHP_CHIP!=7325) && (BCHP_CHIP!=7335)) || (BCHP_VER >= BCHP_VER_B0)
      BAST_SCRIPT_WRITE(BCHP_SDS_CLFBCTL, 0x12),
#endif
      BAST_SCRIPT_OR(BCHP_SDS_SPLL_PWRDN, 0x18), /* powerdown tfec and afec */
      BAST_SCRIPT_WRITE(BCHP_SDS_OIFCTL2, 0x08),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL1, 0x51),         /* reset and disable HP */
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL2, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL6, 0x80),      
      BAST_SCRIPT_WRITE(BCHP_SDS_PILOTCTL, 0x00), 
      BAST_SCRIPT_WRITE(BCHP_SDS_OVRDSEL, 0xFFFF0000),   /* enable micro override of HP */
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_VCOS, 0x00004000),
      BAST_SCRIPT_WRITE(BCHP_SDS_SNRCTL, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLOON, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_VTCTL2, 0x40),
      BAST_SCRIPT_WRITE(BCHP_SDS_VTCTL2, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_FECTL, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_FECTL, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_MISCTL, 0x08),
      BAST_SCRIPT_WRITE(BCHP_SDS_ADC, 0x7e7e0000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSTGCTL, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_ADCPCTL, 0x01),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_2[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_IQPHS, 0x10),
      BAST_SCRIPT_WRITE(BCHP_SDS_IQAMP, 0x20),           
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_7[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW, 0x07070000),
      BAST_SCRIPT_AND_OR(BCHP_SDS_AGICTL, 0x28, 0x22),
      BAST_SCRIPT_AND_OR(BCHP_SDS_AGTCTL, 0x28, 0x02),
      BAST_SCRIPT_WRITE(BCHP_SDS_AII, 0x00000000),   /* set IF to min */
      BAST_SCRIPT_WRITE(BCHP_SDS_AIT, 0xFFFFFFF0),   /* set RF to max */
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW, 0x0E0E0000),
      BAST_SCRIPT_WRITE(BCHP_SDS_AGI, 0x00010000),
      BAST_SCRIPT_WRITE(BCHP_SDS_AGT, 0xFFFF0000),
      BAST_SCRIPT_AND(BCHP_SDS_AGICTL, 0x28),
      BAST_SCRIPT_AND(BCHP_SDS_AGTCTL, 0x28),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL1, 0x00),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL5, 0x00),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL2, 0x00),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL6, 0x00),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL4, 0x02),
      BAST_SCRIPT_EXIT   
   };
   
   static const uint32_t script_qpsk_8[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL15, 0xFF), 
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL16, 0xFF), 
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL3, 0x00), /* enable tuner post-DCO */
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW,0x0B0B0000),  /* same as LDPC ABW */
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_qpsk_10[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_AGTCTL, 0x0A), 
      BAST_SCRIPT_WRITE(BCHP_SDS_AGICTL, 0x0A), 
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW, 0x07070000), 
      BAST_SCRIPT_WRITE(BCHP_SDS_AGI, 0x00000000), 
      BAST_SCRIPT_WRITE(BCHP_SDS_AGT, 0xFFFF0000), 
      BAST_SCRIPT_WRITE(BCHP_SDS_AII, 0x00000000), 
      BAST_SCRIPT_WRITE(BCHP_SDS_AIT, 0x80000000), 
      BAST_SCRIPT_EXIT
   };

   BAST_g2_P_ProcessScript(h, script_qpsk_0);
   
   /* disable any notches from previous acquisition */   
   val = (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK) ? 0x00000F70 : 0x00000F30;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_NTCH_CTRL, &val); 
               
   if (hChn->bExternalTuner == false)
      BAST_g2_P_ProcessScript(h, script_qpsk_7);
   else
   {
      BAST_g2_P_ProcessScript(h, script_qpsk_10);
      
      /* override RF and IF AGC thresholds */
      val = hChn->extTunerRfAgcTop << 16;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_AGT, &val);
      val = hChn->extTunerIfAgcTop << 16;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_AGI, &val);
   }
   
   BAST_g2_P_SetAthr(h);
   if (hChn->bExternalTuner == false)
      BAST_g2_P_Enable3StageAgc(h, true);
   BAST_g2_P_ProcessScript(h, script_qpsk_1);
   BAST_g2_P_SetNyquistAlpha(h);
   BAST_g2_P_ProcessScript(h, script_qpsk_2);

   if (hChn->bExternalTuner == false)
      BAST_g2_P_ProcessScript(h, script_qpsk_8);
  
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_QpskAcquire0a()
******************************************************************************/
BERR_Code BAST_g2_P_QpskAcquire0a(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
#if 0   
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
#endif   
   
   static const uint32_t script_qpsk_3[] =
   {
      /* BAST_SCRIPT_WRITE(BCHP_SDS_CGPDWN, 0x08), */
      BAST_SCRIPT_WRITE(BCHP_SDS_AGFCTL, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_AGF, 0x04300000),
      BAST_SCRIPT_WRITE(BCHP_SDS_AGFCTL, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_VLCI, 0x08000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_VLCQ, 0x08000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_MIXCTL, 0x01),
      BAST_SCRIPT_EXIT
   };
              
   static const uint32_t script_qpsk_9[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_CLPDCTL, 0x10),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLQCFD, 0x8A),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLMISC, 0x03),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLMISC2, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLSTS, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLFFCTL, 0x02),     /* bypass fine mixer */
      BAST_SCRIPT_WRITE(BCHP_SDS_PLDCTL, 0x0a),      /* bypass rescrambler/descrambler */
      BAST_SCRIPT_WRITE(BCHP_SDS_PLC, 0x06080518),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLTD, 0x20000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLI, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLPA, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLTD, 0x20000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLI, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLPA, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_BRI, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_BLPCTL2, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_QPSK, 0x01000000),  /* need to set here again, in case switch from LDPC 8PSK to Legacy, ckp */
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_qpsk_4[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_EQFFE1, 0x51),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQFFE2, 0x02),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQMU, 0x22),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_5[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_EQMISC, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQBLND, 0x04),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQCFAD, 0x46),     /* per Tommy 1/8/2009, was 0x4C */
      BAST_SCRIPT_WRITE(BCHP_SDS_F0B, 0x16000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_BLPCTL1, 0x07),
      BAST_SCRIPT_WRITE(BCHP_SDS_VLCTL1, 0x85),
      BAST_SCRIPT_WRITE(BCHP_SDS_VLCTL2, 0xF1),
      BAST_SCRIPT_WRITE(BCHP_SDS_VLCTL3, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_FERR, 0x00000000),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_6[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_BEIT, 0x00FF005F),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLQCFD, 0x8A),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLTD, 0x25000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLTD, 0x25000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_BRI, 0x00000000),
      BAST_SCRIPT_EXIT
   };
   
   BAST_g2_P_QpskInitializeLoopParameters(h);

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_TUNER_TEST_MODE)
      return BAST_g2_P_TunerTestMode(h);

   BAST_g2_P_ProcessScript(h, script_qpsk_3);
   
   if (hChn->bExternalTuner)
   {
#if 0   
      val = 0x03; /* reset DCO per Steve */
      BAST_g2_P_WriteRegister(h, BCHP_SDS_DCOCTL2, &val);
#endif      
      val = 0x53;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_CLCTL, &val);
   }
   else
   {
      val = 0x00;
      BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL7, &val);
      val = 0x73;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_CLCTL, &val);
   }
   
   BAST_g2_P_ProcessScript(h, script_qpsk_9);
   
   BAST_g2_P_SetEqffe3(h);
   BAST_g2_P_ProcessScript(h, script_qpsk_4);
   BAST_g2_P_QpskSetEqmode(h);
   BAST_g2_P_ProcessScript(h, script_qpsk_5);
   BAST_g2_P_QpskSetMode(h);
   BAST_g2_P_InitBert(h);
   BAST_g2_P_ProcessScript(h, script_qpsk_6);
   BAST_g2_P_StartBert(h);
   BAST_g2_P_QpskSetOqpsk1(h);
   BAST_g2_P_QpskSetClctl2(h);
   BAST_g2_P_QpskSetFinalBlBw(h);
   BAST_g2_P_QpskSetFinalFlBw(h);

   if (hChn->bExternalTuner)
   {  
      BAST_g2_P_SetFlifOffset(h, hChn->extTunerIfOffset);   
      return BAST_g2_P_QpskAcquire1(h);
   }
   else
   {
      BAST_g2_P_ReadRegister(h, BCHP_SDS_CLCTL, &(hChn->clctl));
      val = 0x1F;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_CLCTL, &val);
      
      if (BAST_g2_P_TunerMixpllLostLock(h))
      {
         hChn->tuneMixStatus |= BAST_TUNE_MIX_NEXT_RETRY;                      
         return BAST_g2_P_QpskReacquire(h);
      }

      BAST_g2_P_Enable3StageAgc(h, false);       
      return BAST_g2_P_DoDftFreqEstimate(h, BAST_g2_P_QpskAcquire1);    
   }
}


/******************************************************************************
 BAST_g2_P_QpskAcquire1a()
******************************************************************************/
BERR_Code BAST_g2_P_QpskAcquire1a(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   BAST_g2_P_Enable3StageAgc(h, true); /* re-enable 3stage agc after dft freq estimate */
       
#ifndef BAST_EXCLUDE_SPLITTER_MODE
   if (hChn->tunerCtl & BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE)
      hChn->clctl &= ~0x20;
#endif
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLCTL, &(hChn->clctl));
   
   val = 0x70;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLMISC2, &val);     

   hChn->funct_state = 0;
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 20000, BAST_g2_P_QpskLockViterbi);
}


/******************************************************************************
 BAST_g2_P_QpskAcquire1()
******************************************************************************/
BERR_Code BAST_g2_P_QpskAcquire1(BAST_ChannelHandle h)
{
   BAST_g2_P_Handle *hDev = h->pDevice->pImpl;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi;

#ifndef BAST_EXCLUDE_SPLITTER_MODE   
   if (hChn->tunerCtl & BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE)   
      BAST_g2_P_SetFlifOffset(h, hChn->splitterModeAdj);    
#endif   
   
   if ((hChn->bExternalTuner == false) && ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK) == 0))
   {      
      /* internal tuner */
      return BAST_g2_P_QpskAcquire1a(h);

   }
   else
   {
      /* external tuner */
      hChn->checkLockInterval = 80000; /* 40000; */
      hChn->initCarrierDelay =  500000;
      
      /* reset the eq */
      BAST_g2_P_OrRegister(h, BCHP_SDS_EQFFE1, 0x01);

      BAST_g2_P_SetCarrierBw(h, hChn->carrierAcqBw, hChn->carrierAcqDamp);
      BAST_g2_P_SetBaudBw(h, hChn->baudBw, hChn->baudDamp);

      hChn->sweep = 0;
      hChn->sum0 = 0;  /* sum0 = fli_sum */
      hChn->bVitLocked = false;

      if (hChn->acqParams.symbolRate > 19000000)
         hChn->symbolRateVerifyLock = 20000000;
      else
         hChn->symbolRateVerifyLock = hChn->acqParams.symbolRate;

#if 0         
      if (hChn->acqCount < 5)
         hChn->maxSweep = 3;
      else
         hChn->maxSweep = 6;
#else
      hChn->maxSweep = 1;         
#endif         
         
      if (hChn->acqParams.carrierFreqOffset)
      {
         /* warm start */
         hChn->searchRangeFli = 0;
         hChn->maxSweep = 1;
      }
      else
         hChn->searchRangeFli = hDev->searchRange + (hChn->acqParams.symbolRate >> 4);
      BAST_MultU32U32(hChn->symbolRateVerifyLock, hChn->acqParams.symbolRate >= 20000000 ? 858993459 : 536870912, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, (uint32_t *)&(hChn->stepFreq));

   #ifdef VERIFY_LOCK_DEBUG
      BDBG_MSG(("VerifyLock: searchRangeFli=%d", hChn->searchRangeFli));
   #endif

      if (hChn->spinvState & BAST_SPINV_STATE_SCAN_ENABLED)
         hChn->spinvState &= (BAST_SPINV_STATE_SCAN_ENABLED | BAST_SPINV_STATE_INV); /* clear previous state */
      
      hChn->funct_state = 0;
      return BAST_g2_P_QpskVerifyLock(h);      
   }
}
  

/******************************************************************************
 BAST_g2_P_QpskVerifyLock()
******************************************************************************/
BERR_Code BAST_g2_P_QpskVerifyLock(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo, val, bw, damp, vtctl1;
   int32_t offset, fli, i;
   BERR_Code retCode = BERR_SUCCESS;
   
   static uint32_t carrier_delay_dvb_dtv[6] = /* scaled 2^11 */  
   {  
   
      4253, /* 2.8 */ /* was 3780,2.6 */
      4453, /* 2.8 */ /* was 4253, */
      4726, /* 3.0 */
      5780, /* 3.2 */
      6253, /* 3.6 */
      7726  /* 4.0 */  
   };

   static uint32_t carrier_delay_dcii[6] = /* scaled 2^11 */
   {
      5253, /* 2.8 */ /* was 3780,2.6 */
      4453, /* 2.8 */ /* was 4253, */
      4726, /* 3.0 */
      5780, /* 3.2 */
      6253, /* 3.6 */
      7726  /* 4.0 */ 
   };

   while (1)
   {
      /* BDBG_MSG(("VL state %d", hChn->funct_state)); */
      if (hChn->bExternalTuner == false)
         BAST_g2_P_3StageAgc_isr(h, 0);             
      
      switch (hChn->funct_state)
      {
         case 0:
            if (BAST_MODE_IS_DCII(hChn->acqParams.mode))
               hChn->carrierDelay = carrier_delay_dcii[hChn->sweep] * 10;
            else
            {
               hChn->carrierDelay = carrier_delay_dvb_dtv[hChn->sweep];
               if (hChn->symbolRateVerifyLock >= 15000000)
                  hChn->carrierDelay *= 4;
               else if (hChn->symbolRateVerifyLock < 4000000)
                  hChn->carrierDelay *= 6; /* was 8 */
               else
                  hChn->carrierDelay *= 6;
            }
          
            if (hChn->acqParams.carrierFreqOffset)
               hChn->carrierDelay *= 2; /* give extra delay for warm start */
         
            BAST_MultU32U32(hChn->carrierDelay, hChn->initCarrierDelay, &P_hi, &P_lo);
            BAST_DivU64U32(P_hi, P_lo, 20480, &Q_hi, &(hChn->carrierDelay));
            hChn->lockPoint = 0;
            hChn->funct_state = 1;
            
            /* reset the eq */
            BAST_g2_P_OrRegister(h, BCHP_SDS_EQFFE1, 0x01);

            val = 0x16000000;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_F0B, &val);   
            break;

         case 1:         
            if ((hChn->sweep == 0) && (hChn->lockPoint == 1))
            {
#ifdef VERIFY_LOCK_DEBUG             
               BDBG_MSG(("FLI sum = %d (0x%08X)", hChn->sum0, hChn->sum0));
#endif               
               if (hChn->sum0 < 0)
                  hChn->stepFreq = -(hChn->stepFreq);
            }
            val = (hChn->lockPoint + 1) >> 1;
            offset = val * hChn->stepFreq;
            if (hChn->lockPoint & 1)
               offset = -offset;

#ifdef VERIFY_LOCK_DEBUG 
BAST_MultU32U32(offset < 0 ? -offset : offset, hChn->sampleFreq >> 2, &P_hi, &P_lo);
BAST_DivU64U32(P_hi, P_lo, 2147483648UL, &Q_hi, &Q_lo);
            BDBG_MSG(("VerifyLock: Sweep=%d, LockPoint=%d, step_freq=%d, offset=%d", hChn->sweep, hChn->lockPoint, hChn->stepFreq, ((offset < 0) ? ((int32_t)-Q_lo) : Q_lo))); 
#endif
            /* check if we have gone beyond search range */
            BAST_MultU32U32(hChn->symbolRateVerifyLock, val * 625, &P_hi, &P_lo);
            BAST_DivU64U32(P_hi, P_lo, 10000, &Q_hi, &Q_lo);          
            if (Q_lo > hChn->searchRangeFli)
               hChn->funct_state = 5;
            else
            {
               hChn->fli = offset;

               bw = hChn->carrierAcqBw;
               damp = hChn->carrierAcqDamp;
               BAST_g2_P_SetCarrierBw(h, bw, damp);

               /* set FLI */
               BAST_g2_P_WriteRegister(h, BCHP_SDS_FLI, (uint32_t *)&offset);

               hChn->count1 = 0;
               hChn->count2 = 0;
               hChn->maxCount1 = hChn->carrierDelay / hChn->checkLockInterval; 
               if (hChn->maxCount1 == 0)
                  hChn->maxCount1 = 1;

               hChn->funct_state = 2;
            }
            break;

         case 2: /* qpsk_verify_lock_narrow_bw_1 */
            if (hChn->lockPoint == 0)
            {
               BAST_g2_P_ReadRegister(h, BCHP_SDS_FLI,(uint32_t *) &fli);
               hChn->sum0 += ((hChn->fli - fli)/32);
            }

            if (hChn->count2 <= 10)
            {
               val = (hChn->count1 * 10) / hChn->maxCount1;
               if (val > hChn->count2)
               {
                  hChn->count2 = val;
                  bw = hChn->carrierAcqBw - (((hChn->carrierAcqBw - hChn->carrierTrkBw) * val) / 10);                  
                  BAST_g2_P_SetCarrierBw(h, bw, hChn->carrierAcqDamp);
               }
            }

            hChn->count1++;
            if (hChn->count1 < hChn->maxCount1)
               return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, hChn->checkLockInterval, BAST_g2_P_QpskVerifyLock); 

            /* narrow to tracking bw */
            BAST_g2_P_SetCarrierBw(h, hChn->carrierTrkBw, hChn->carrierTrkDamp);
            hChn->funct_state = 8;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 90000, BAST_g2_P_QpskVerifyLock); /* need this to prevent lock->unlock->lock */
            
         case 8:
            hChn->bVitLocked = true;

            if (BAST_MODE_IS_DCII(hChn->acqParams.mode))
               hChn->maxCount1 = 200;
            else
               hChn->maxCount1 = 20;
            
            hChn->count2 = (hChn->carrierDelay >> 1) / hChn->checkLockInterval; /* hChn->count2 = (hChn->carrierDelay >> (hChn->acqParams.mode == BAST_Mode_eDvb_scan ? 1 : 2)) / hChn->checkLockInterval; */
            if (hChn->count2 == 0)
               hChn->count2 = 1;
            hChn->funct_state = 3;
            break;

         case 3: /* qpsk_check_lock_while_delay_1 */
            hChn->count1 = 0;
            for (i = 0; i < 20; i++)
            {
               val = 0x1B;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQSTS4, &val);
               val = 0;
               BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS4, &val);
               if ((val & 0x1B) != 0x09)
                  break; /* not yet locked */
               else
                  hChn->count1++;
            }
            if (hChn->count1 >= 20)
               hChn->funct_state = 6; /* vit locked */
            else
            {
               hChn->count2--;
               if (hChn->count2)
                  return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, hChn->checkLockInterval, BAST_g2_P_QpskVerifyLock); 
               else 
                  hChn->funct_state = 4; /* vit not locked */
            }
            break;

         case 4: /* qpsk_verify_lock_4 */
            hChn->bVitLocked = false;

            BAST_g2_P_OrRegister(h, BCHP_SDS_EQFFE1, 0x01);

            val = 0x16000000;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_F0B, &val);

            hChn->lockPoint++;
            hChn->funct_state = 1;
            break;

         case 5:
            if (hChn->spinvState & BAST_SPINV_STATE_SCAN_ENABLED)
            {
               BAST_g2_P_ReadRegister(h, BCHP_SDS_VTCTL1, &vtctl1);
               vtctl1 &= ~0x60;
               if (hChn->spinvState & BAST_SPINV_STATE_INV)
               {
#ifdef VERIFY_LOCK_DEBUG                  
                  BDBG_MSG(("VerifyLock: try spinv=0"));
#endif
                  vtctl1 |= 0x00;
               }
               else
               {
#ifdef VERIFY_LOCK_DEBUG                  
                  BDBG_MSG(("VerifyLock: try spinv=1"));    
#endif
                  vtctl1 |= 0x20;
               }
               hChn->spinvState ^= BAST_SPINV_STATE_INV;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL1, &vtctl1);
               
               if ((hChn->spinvState & BAST_SPINV_STATE_PASS) == 0)
               {
                  hChn->spinvState |= BAST_SPINV_STATE_PASS;
                  goto retry_this_sweep;
               }
               else
               {
                  hChn->spinvState &= ~BAST_SPINV_STATE_PASS;
               }
            }
            
            if (hChn->dtvScanState & BAST_G2_DTV_SCAN_ON)
            {
               hChn->dtvScanState ^= BAST_G2_DTV_SCAN_1_2;
               BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_VTCTL1, 0xFFFFFFE0, 
                  ((hChn->dtvScanState & BAST_G2_DTV_SCAN_1_2) ? 0 : 0x11));

               BAST_g2_P_ReadRegister(h, BCHP_SDS_VTCTL2, &val);
               val |= 0x40;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL2, &val);
               val &= ~0x40;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL2, &val);

               if (hChn->dtvScanState & BAST_G2_DTV_SCAN_1_2)
               {
                  retry_this_sweep:
                  hChn->funct_state = 0;
                  break;
               }
            }
            hChn->sweep++;
            if (hChn->sweep < hChn->maxSweep)
               hChn->funct_state = 0;
            else
               hChn->funct_state = 6;
            break;

         case 6: /* qpsk_verify_lock_6 */
            if (hChn->bVitLocked == false)
            {
               BDBG_MSG(("Vit could not lock"));
               return BAST_g2_P_QpskDelayReacquire(h);
            }
            
#ifdef VERIFY_LOCK_DEBUG 
            BDBG_MSG(("VerifyLock: Vit locked")); 
#endif
            
            hChn->actualMode = BAST_g2_P_QpskGetActualMode(h);

            BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &val);
            val = (val >> 24) & 0x0F;
            switch (val)
            {
               case 0:
               case 8:
               case 9:
                  val = 0x4000;
                  break;
               case 1:
               case 10:
               case 11:
                  val = 0x4800;
                  break;
               case 2:
               case 12:
               case 13:
                  val = 0x5000;
                  break;
               case 3:
               case 14:
                  val = 0x6000;
                  break;
               case 4:
                  val = 0x6800;
                  break;
               case 5:
               case 15:
                  val = 0x7800;
                  break;
            }
            BAST_g2_P_WriteRegister(h, BCHP_SDS_VCOS, &val);
                       
            if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK) && (hChn->bExternalTuner == false))
               return BAST_g2_P_QpskAcquire1a(h);
            else              
               return BAST_g2_P_QpskSpinvScan(h);           
               
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;         
      }
   }
   return retCode;
}

  
/******************************************************************************
 BAST_g2_P_QpskLockViterbi()
******************************************************************************/
BERR_Code BAST_g2_P_QpskLockViterbi(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, bw, damp, vst_save;
   int32_t i, j;
   BERR_Code retCode = BERR_SUCCESS;   

   while (1)
   {
      /* BDBG_MSG(("VL state %d", hChn->funct_state)); */
      if (hChn->bExternalTuner == false)
         BAST_g2_P_3StageAgc_isr(h, 0);             
      
      switch (hChn->funct_state)
      {
         case 0:     
            if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK) && (hChn->bExternalTuner == false))  
            {
               /* viterbi has already been locked in BAST_g2_P_QpskVerifyLock() */
               hChn->bVitLocked = true;
               hChn->funct_state = 6;
               break;
            }
           
            hChn->initCarrierDelay = (hChn->acqParams.mode == BAST_Mode_eDcii_scan) ? 900000 : 350000;
            hChn->checkLockInterval = 20000;
            
            if (hChn->acqParams.symbolRate < 5000000)
               hChn->carrierDelay = hChn->initCarrierDelay*2;
            else if (hChn->acqParams.symbolRate < 10000000)
               hChn->carrierDelay = hChn->initCarrierDelay;
            else
               hChn->carrierDelay = hChn->initCarrierDelay >> 1;                   
                     
/* BDBG_MSG(("initCarrierDelay=%d, carrierDelay=%d, checkLockInterval=%d", hChn->initCarrierDelay, hChn->carrierDelay, hChn->checkLockInterval));  */
            
            /* reset the eq */
            BAST_g2_P_OrRegister(h, BCHP_SDS_EQFFE1, 0x01);
            
            BAST_g2_P_SetCarrierBw(h, hChn->carrierAcqBw, hChn->carrierAcqDamp);
            BAST_g2_P_SetBaudBw(h, hChn->baudBw, hChn->baudDamp);

            hChn->bVitLocked = false;
                                       	
            hChn->funct_state = 1;
            break;

         case 1: 
            bw = hChn->carrierAcqBw;
            damp = hChn->carrierAcqDamp;
            BAST_g2_P_SetCarrierBw(h, bw, damp);

            hChn->count1 = 0;
            hChn->count2 = 0;
            hChn->maxCount1 = (2 * hChn->carrierDelay) / hChn->checkLockInterval; 
            if (hChn->maxCount1 == 0)
               hChn->maxCount1 = 1;

            hChn->funct_state = 2;
            break;

         case 2: /* qpsk_verify_lock_narrow_bw_1 */
            bw = hChn->carrierAcqBw - ((hChn->count1 * (hChn->carrierAcqBw - hChn->carrierTrkBw)) / hChn->maxCount1);
            BAST_g2_P_SetCarrierBw(h, bw, hChn->carrierAcqDamp);
            hChn->count1++;
            if (hChn->count1 < hChn->maxCount1)
               return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, hChn->checkLockInterval, BAST_g2_P_QpskLockViterbi); 

            /* narrow to tracking bw */
            BAST_g2_P_SetCarrierBw(h, hChn->carrierTrkBw, hChn->carrierTrkDamp);
            
            hChn->funct_state = 8;
            break;
            
         case 8:
            hChn->bVitLocked = true;
            hChn->count1 = (hChn->carrierDelay * 2) / hChn->checkLockInterval;
            if (hChn->count1 < 1)
               hChn->count1 = 1;
            hChn->funct_state = 3;
            break;

         case 3: /* qpsk_check_lock_while_delay_1 */
            if (hChn->count1 > 0)
            {
               /* clear sticky status bits */
               val = 0xFF;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQSTS4, &val);
               
               hChn->funct_state = 7;
               return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, hChn->checkLockInterval, BAST_g2_P_QpskLockViterbi);
            }
            hChn->funct_state = 4; /* vit not locked */
            break;
            
         case 7: 
            BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS4, &val);
            if ((val & 0x18) == 0x08)
               hChn->funct_state = 6; /* vit locked */
            else
            {
               hChn->count1--;
               hChn->funct_state = 3;
            }
            break;

         case 4: /* qpsk_verify_lock_4 */
            hChn->bVitLocked = false;
            if (hChn->dtvScanState & BAST_G2_DTV_SCAN_ON)
            {
               hChn->dtvScanState ^= BAST_G2_DTV_SCAN_1_2;
               BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_VTCTL1, 0xFFFFFFE0, 
                  ((hChn->dtvScanState & BAST_G2_DTV_SCAN_1_2) ? 0 : 0x11));

               BAST_g2_P_ReadRegister(h, BCHP_SDS_VTCTL2, &val);
               val |= 0x40;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL2, &val);
               val &= ~0x40;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL2, &val);

               if (hChn->dtvScanState & BAST_G2_DTV_SCAN_1_2)
               {
                  hChn->funct_state = 1;
                  break;
               }
            }
            hChn->funct_state = 6;
            break;

         case 6: /* qpsk_verify_lock_6 */
            if (hChn->bVitLocked == false)
            {
               BDBG_MSG(("* Vit could not lock"));
               return BAST_g2_P_QpskReacquire(h);
            }
            
            if ((hChn->acqParams.mode == BAST_Mode_eDss_scan) ||
                (hChn->acqParams.mode == BAST_Mode_eDvb_scan) || 
                (hChn->acqParams.mode == BAST_Mode_eDcii_scan))
            {
               /* check VST stability */
               vst_save = 0xFF;
               for (i = 0; i < 11; i++)
               {
                  for (j = 50; j > 0; j--)
                  {
                     BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &val);
                     val = (val >> 24) & 0x0F;
                     if (val != vst_save)
                        break;
                  }
                  vst_save = val;
                  if (j == 0)
                     break;
               }
            }

            hChn->actualMode = BAST_g2_P_QpskGetActualMode(h);

            BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &val);
            val = (val >> 24) & 0x0F;
            switch (val)
            {
               case 0:
               case 8:
               case 9:
                  val = 0x4000;
                  break;
               case 1:
               case 10:
               case 11:
                  val = 0x4800;
                  break;
               case 2:
               case 12:
               case 13:
                  val = 0x5000;
                  break;
               case 3:
               case 14:
                  val = 0x6000;
                  break;
               case 4:
                  val = 0x6800;
                  break;
               case 5:
               case 15:
                  val = 0x7800;
                  break;
            }
            BAST_g2_P_WriteRegister(h, BCHP_SDS_VCOS, &val);
            
            return BAST_g2_P_QpskSpinvScan(h);
               
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;         
      }
   }
   return retCode;
}


/******************************************************************************
 BAST_g2_P_QpskInitializeLoopParameters()
******************************************************************************/
void BAST_g2_P_QpskInitializeLoopParameters(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t Fb, carrier_acq_bw, carrier_acq_damp, carrier_trk_bw, carrier_trk_damp, baud_loop_bw, baud_loop_damp, cmin, bmin;
   uint32_t P_hi, P_lo, Q_hi;

   /* initialize baud/carrier loop parameters based on symbol rate */
   Fb = hChn->acqParams.symbolRate;
   if (Fb >= 27000000)
   {
      /* Fb >= 27M */
      if (hChn->bExternalTuner)
      {
         carrier_acq_bw = 6554; 
         carrier_acq_damp = 10;
         carrier_trk_bw = 655;
         carrier_trk_damp = 4;      
      }
      else
      {
         carrier_acq_bw = 2588; 
         carrier_acq_damp = 12;
         carrier_trk_bw = 327;
         carrier_trk_damp = 8;
      }
      baud_loop_bw = 5243;
      baud_loop_damp = 4;
      cmin = 66;
      bmin = 66;
   }
   else if (Fb >= 23000000)
   {
      /* 27M > Fb >= 23M */
      if (hChn->bExternalTuner)
      {
         carrier_acq_bw = 7078;
         carrier_acq_damp = 10;
         carrier_trk_bw = 655;
         carrier_trk_damp = 42;      
      }
      else
      {      
         carrier_acq_bw = 2588;
         carrier_acq_damp = 12;
         carrier_trk_bw = 327;
         carrier_trk_damp = 8;
      }
      baud_loop_bw = 5243;
      baud_loop_damp = 4;
      cmin = 66;
      bmin = 66;
   }
   else if (Fb >= 17000000)		
   {
      /* 23M > Fb >= 17M */  
      if (hChn->bExternalTuner)
      {
         carrier_acq_bw = 9078;
         carrier_acq_damp = 10;
         carrier_trk_bw = 1655;
         carrier_trk_damp = 10; 
      }
      else
      {      
         carrier_acq_bw = 2588;
         carrier_acq_damp = 12;
         carrier_trk_bw = 327;
         carrier_trk_damp = 8;
      }
      baud_loop_bw = 5243;
      baud_loop_damp = 8;
      cmin = 66;
      bmin = 66;  
   }
   else if (Fb >= 12000000)
   {
      /* 17M > Fb >= 12M */
      if (hChn->bExternalTuner)
      {
         carrier_acq_bw = 7864;
         carrier_acq_damp = 18;
         carrier_trk_bw = 1442;
         carrier_trk_damp = 4;      
      }
      else
      {
         carrier_acq_bw = 2078; /*1078 */
         carrier_acq_damp = 6;
         carrier_trk_bw = 455; /*255*/
         carrier_trk_damp = 5;   
      }
      baud_loop_bw = 2243;
      baud_loop_damp = 4;
      cmin = 66;
      bmin = 87;
   }
   else if (Fb >= 4000000)
   {
      /* 4M <= Fb < 12M */
      if (hChn->bExternalTuner)
      {
         carrier_acq_bw = 7864;
         carrier_acq_damp = 4;
         carrier_trk_bw = 367;
         carrier_trk_damp = 4;      
      }
      else
      {      
         carrier_acq_bw = 2588;
         carrier_acq_damp = 12;
         carrier_trk_bw = 327;
         carrier_trk_damp = 8;
      }
      baud_loop_bw = 3680; 
      baud_loop_damp = 4;
      cmin = 132;   
      bmin = 160;
   }
   else if (Fb >= 2000000)
   {
      /* 2M <= Fb < 4M */
      if (hChn->bExternalTuner)
      {
         carrier_acq_bw = 5505;
         carrier_acq_damp = 4;
         carrier_trk_bw = 367; 
         carrier_trk_damp = 4;   
      }
      else
      {      
         carrier_acq_bw = 2588;
         carrier_acq_damp = 12;
         carrier_trk_bw = 327; 
         carrier_trk_damp = 8;
      }
      baud_loop_bw = 2359;
      baud_loop_damp = 4;
      cmin = 132;
      bmin = 262;
   }
   else
   {
      /* Fb < 2M */
      if (hChn->bExternalTuner)
      {
         carrier_acq_bw = 5505; 
         carrier_acq_damp = 4;
         carrier_trk_bw = 289;
         carrier_trk_damp = 4;      
      }      
      else
      {      
         carrier_acq_bw = 3882; /* was 2588 */ 
         carrier_acq_damp = 12;
         carrier_trk_bw = 130; /* was 163 */
         carrier_trk_damp = 8;
      }
      baud_loop_bw = 2359;
      baud_loop_damp = 12;
      cmin = 118;
      bmin = 472;
   }

   BAST_MultU32U32(carrier_acq_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 262144, &Q_hi, &carrier_acq_bw);
   BAST_MultU32U32(carrier_trk_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 262144, &Q_hi, &carrier_trk_bw);
   BAST_MultU32U32(baud_loop_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 262144, &Q_hi, &baud_loop_bw);
   BAST_MultU32U32(cmin, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 262144, &Q_hi, &cmin);
   BAST_MultU32U32(bmin, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 262144, &Q_hi, &bmin);
   
   hChn->carrierAcqBw = carrier_acq_bw;
   hChn->carrierAcqDamp = carrier_acq_damp;
   hChn->carrierTrkBw = carrier_trk_bw;
   hChn->carrierTrkDamp = carrier_trk_damp;
   hChn->baudBw = baud_loop_bw;
   hChn->baudDamp = baud_loop_damp;
   hChn->cmin = cmin;
   hChn->bmin = bmin;
}


/******************************************************************************
 BAST_g2_P_QpskSetEqmode()
******************************************************************************/
void BAST_g2_P_QpskSetEqmode(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   if (hChn->acqParams.symbolRate <= (hChn->sampleFreq >> 2))
      val = 0;
   else
      val = 0x20;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_EQMODE, &val);
}


/******************************************************************************
 BAST_g2_P_QpskSetMode()
******************************************************************************/
void BAST_g2_P_QpskSetMode(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t vtctl2, fmod, vtctl1;

   static uint32_t script_dcii[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_V7, 0x08800000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V6, 0x08660000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V5, 0x08000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V4, 0x07000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V3, 0x06000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V2, 0x05000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V1, 0x04500000),  
      BAST_SCRIPT_WRITE(BCHP_SDS_V0, 0x04000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_VINT, 0x25800000),
      BAST_SCRIPT_WRITE(BCHP_SDS_FRS, 0x0a0bfffe),
      BAST_SCRIPT_WRITE(BCHP_SDS_FSYN, 0x02000805),
      BAST_SCRIPT_EXIT
   };

   static uint32_t script_dvb_dtv[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_V7, 0x00e60000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V6, 0x00e60000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V5, 0x09000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V4, 0x08800000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V3, 0x08660000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V2, 0x07800000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V1, 0x06000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_V0, 0x04800000),
      BAST_SCRIPT_WRITE(BCHP_SDS_VINT, 0x27e70000),
      BAST_SCRIPT_WRITE(BCHP_SDS_FRS, 0x020b2523),
      BAST_SCRIPT_WRITE(BCHP_SDS_FSYN, 0x02000805),
      BAST_SCRIPT_EXIT
   };

   if (BAST_MODE_IS_DCII(hChn->acqParams.mode))
   {
      /* DCII */
      BAST_g2_P_ProcessScript(h, script_dcii);
   }
   else
   {
      /* DVB or DTV */
      BAST_g2_P_ProcessScript(h, script_dvb_dtv);
   }

   switch (hChn->acqParams.mode)
   {
      case BAST_Mode_eDvb_scan:
         vtctl1 = 0xF0;
         break;
      case BAST_Mode_eDss_6_7:
         vtctl1 = 0xE4;
         break;
      case BAST_Mode_eDss_2_3:
      case BAST_Mode_eDvb_2_3:
         vtctl1 = 0xE1;
         break;
      case BAST_Mode_eDss_1_2:
      case BAST_Mode_eDvb_1_2:
         vtctl1 = 0xE0;
         break;
      case BAST_Mode_eDvb_3_4:
         vtctl1 = 0xE2;
         break;
      case BAST_Mode_eDvb_5_6:
         vtctl1 = 0xE3;
         break;
      case BAST_Mode_eDvb_7_8:
         vtctl1 = 0xE5;
         break;
      case BAST_Mode_eDss_scan:
         vtctl1 = 0xF1;
         break;
      case BAST_Mode_eDcii_scan:
         vtctl1 = 0x12;
         break;
      case BAST_Mode_eDcii_5_11:
         vtctl1 = 0x08;
         break;
      case BAST_Mode_eDcii_1_2:
         vtctl1 = 0x09;
         break;
      case BAST_Mode_eDcii_3_5:
         vtctl1 = 0x0A;
         break;
      case BAST_Mode_eDcii_2_3:
         vtctl1 = 0x0B;
         break;
      case BAST_Mode_eDcii_3_4:
         vtctl1 = 0x0C;
         break;
      case BAST_Mode_eDcii_4_5:
         vtctl1 = 0x0D;
         break;
      case BAST_Mode_eDcii_5_6:
         vtctl1 = 0x0E;
         break;
      case BAST_Mode_eDcii_7_8:
         vtctl1 = 0x0F;
         break;
      default:
         BDBG_ASSERT(0); /* should never get here */
   }

   vtctl2 = 0x00;  
   if (BAST_MODE_IS_DCII(hChn->acqParams.mode))
   {
      /* DCII */
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT)
      {
         vtctl2 = 0x04;
         if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT_Q)
         {
            vtctl1 |= 0xA0; /* split Q */
            vtctl1 &= 0xBF;
         }
         else
         {
            vtctl1 |= 0x80; /* split I */
            vtctl1 &= 0x9F;
         }
      }
      else
         vtctl1 |= 0xE0;  /* combine */
   }

   if ((BAST_MODE_IS_DTV(hChn->acqParams.mode)) ||
       (BAST_MODE_IS_DVB(hChn->acqParams.mode)) ||
       ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT) == 0))
   {
      vtctl1 &= 0x9F;
      if (hChn->spinvState & BAST_SPINV_STATE_SCAN_ENABLED)
      {
         if (hChn->spinvState & BAST_SPINV_STATE_INV)
         {
#ifdef VERIFY_LOCK_DEBUG
            BDBG_MSG(("trying spinv=1"));
#endif
            vtctl1 |= 0x20;
         }
         else
         {
#ifdef VERIFY_LOCK_DEBUG
            BDBG_MSG(("trying spinv=0"));     
#endif
            vtctl1 |= 0x00;
         }
      }
      else
         vtctl1 |= ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SPINV_IQ_SCAN) >> 8);
   }
   BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL1, &vtctl1);

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK)
   {
      BAST_g2_P_OrRegister(h, BCHP_SDS_EQMISC, 0x02);
      BAST_g2_P_OrRegister(h, BCHP_SDS_CLMISC, 0x10);
      vtctl2 |= 0x02;
   }

   BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL2, &vtctl2);

   /* reset viterbi decoder */
   vtctl2 |= 0x40;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL2, &vtctl2);
   vtctl2 &= 0xBF;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_VTCTL2, &vtctl2);

   /* STFMOD */
   if (BAST_MODE_IS_DVB(hChn->acqParams.mode))
   {
      /* DVB */
      fmod = 0x00005200;
   }
   else if (BAST_MODE_IS_DTV(hChn->acqParams.mode))
   {
      /* DTV */
      fmod = 0x00005600;
   }
   else
   {
      /* DCII */
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT)
      {
         if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT_Q)
            fmod = 0x00000A00;
         else
            fmod = 0x00004A00;
      }
      else
         fmod = 0x00005A00; /* combine */
   }
   fmod |= (hChn->stuffBytes & 0x1F);   
   fmod |= ((hChn->xportCtl & BAST_G2_XPORT_CTL_TEI) ? 0x02 : 0x00) << 8;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FMOD, &fmod);
}


/******************************************************************************
 BAST_g2_P_QpskSetOqpsk1()
******************************************************************************/
void BAST_g2_P_QpskSetOqpsk1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK)
   {
      val = 0x00;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_CLPDCTL, &val);

      val = 0xB1;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_CLMISC, &val);

      val = 0x0F;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_BLPCTL1, &val);
   }
}


/******************************************************************************
 BAST_g2_P_QpskSetOqpsk2()
******************************************************************************/
void BAST_g2_P_QpskSetOqpsk2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val1, val2;
  
   val1 = (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK) ? 0xB1 : 0xA1;
   val2 = 0x10;

   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLMISC, &val1);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLPDCTL, &val2);
}


/******************************************************************************
 BAST_g2_P_QpskSetClctl2()
******************************************************************************/
void BAST_g2_P_QpskSetClctl2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   BAST_g2_P_ReadRegister(h, BCHP_SDS_CLCTL, &val);
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK)
      val = 0x5C; /* (val & 0x70) | 0x0C; */
   else
      val = (val & 0x30) | 0x0F;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLCTL, &val);
}


/******************************************************************************
 BAST_g2_P_QpskSetClctl3()
******************************************************************************/
void BAST_g2_P_QpskSetClctl3(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   BAST_g2_P_ReadRegister(h, BCHP_SDS_CLCTL, &val);
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK)
      val &= 0xFFFFFF70;
   else
      val &= 0xFFFFFF30;
   val |= 0x0C;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLCTL, &val);
}


/******************************************************************************
 BAST_g2_P_QpskSetFinalFlBw()
******************************************************************************/
void BAST_g2_P_QpskSetFinalFlBw(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t vst, fllc, flic;

   BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &vst);
   vst = (vst >> 24) & 0x0F;

   if (BAST_MODE_IS_DVB(hChn->acqParams.mode))
   {
      /* DVB */
      if (vst <= 1)
      {
         /* code rate 1/2 or 2/3 */
         fllc = 0x01000100;
         flic = 0x01000000;
      }
      else
      {
         fllc = 0x01000100;
         flic = 0x01000000;
      }
   }
   else if (BAST_MODE_IS_DTV(hChn->acqParams.mode))
   {
      /* DTV */
      if (vst <= 1)
      {
         /* code rate 1/2 or 2/3 */
         fllc = 0x02880300;
         flic = 0x029B0200;
      }
      else
      {
         fllc = 0x04F20200;
         flic = 0x01B20200;
      }
   }
   else
   {
      /* DCII */
      fllc = 0x02000200;
      flic = 0x01000300;
   }
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FLLC, &fllc);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FLIC, &flic);
}


/******************************************************************************
 BAST_g2_P_QpskSetFinalBlBw()
******************************************************************************/
void BAST_g2_P_QpskSetFinalBlBw(BAST_ChannelHandle h)
{
   BAST_P_Handle *hDev = h->pDevice;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, vst;

   BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &vst); 
   vst &= 0x0F000000;

   if (hDev->settings.networkSpec == BAST_NetworkSpec_eEuro)
   {
      BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &val); 
      if (vst == 0)
      {
         /* mode is DTV 1/2 or DVB 1/2 */
         val = 0x050F0418;
      }
      else
         val = 0x061E050B;
   }
   else if (BAST_MODE_IS_DVB(hChn->acqParams.mode))
   {
      if ((hChn->acqParams.symbolRate <= 5000000) && (vst != 0))
         val = 0x0410050C;  /* 0x050F0418;  */     /*  No good val = 0x0410050C;  val = 0x063C051; val = 0x051E0421*/
      else if ((hChn->acqParams.symbolRate > 30000000) && (vst != 0))
      	 val = 0x061E050B;
      else
         val = 0x050F0418;      /* only this value works */
   }
   else if (BAST_MODE_IS_DTV(hChn->acqParams.mode))
      val = 0x050F0410; /* 0x050F0418; */	/* DTV 6/7 marginal improvement */
   else
      val = 0x030F041F;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PLC, &val);
}


/******************************************************************************
 BAST_g2_P_QpskCheckCodeRateScan()
******************************************************************************/
bool BAST_g2_P_QpskCheckCodeRateScan(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint8_t mode_mask, search_mask;

   if (hChn->acqParams.mode == BAST_Mode_eDss_scan)
   {
      search_mask = hChn->dtvScanCodeRates;
      mode_mask = hChn->actualMode - BAST_Mode_eDss_1_2;
      if (mode_mask == 2)
         mode_mask = 4;
      goto check_scan;
   }
   else if (hChn->acqParams.mode == BAST_Mode_eDvb_scan)
   {
      search_mask = hChn->dvbScanCodeRates;
      mode_mask = hChn->actualMode - BAST_Mode_eDvb_1_2;
      
      if (mode_mask == 4)
         mode_mask = 5;
         
      check_scan:
      mode_mask = 1 << mode_mask;
  
      if ((mode_mask & search_mask) == 0)  
         return false;
   }
   return true;
}


/******************************************************************************
 BAST_g2_P_QpskSpinvScan()
******************************************************************************/
BERR_Code BAST_g2_P_QpskSpinvScan(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BAST_Mode mode = hChn->actualMode;
   
   if (BAST_g2_P_QpskCheckCodeRateScan(h) == false)
   {
      BDBG_MSG(("BAST_Mode 0x%X not allowed", mode));
      return BAST_g2_P_QpskDelayReacquire(h);
   }
   
   if ((mode != BAST_Mode_eDvb_5_6) && (mode != BAST_Mode_eDss_6_7))
      return BAST_g2_P_QpskNarrowBw(h);
   
   /*if ((mode == BAST_Mode_eDss_6_7) || (mode == BAST_Mode_eDss_scan) ||
       (mode == BAST_Mode_eDvb_5_6) || (mode == BAST_Mode_eDvb_scan) ||
       (mode == BAST_Mode_eDcii_scan)) */
   {
      /* do spinv scan */
      BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_FSYN, 0xFFFF0000, 0x0802);
   }

   hChn->funct_state = 0;
   hChn->count1 = 0;
   return BAST_g2_P_QpskSpinvScan1(h);
}


/******************************************************************************
 BAST_g2_P_QpskSpinvScan1()
******************************************************************************/
BERR_Code BAST_g2_P_QpskSpinvScan1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   while (1)
   {
      switch (hChn->funct_state)
      {
         case 0:
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_VTCTL1, 0xFFFFFF9F, (hChn->count1 % 3) << 5);
            hChn->funct_state = 1;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 30000, BAST_g2_P_QpskSpinvScan1); 

         case 1:
            val = 0x1B;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQSTS4, &val);
            hChn->funct_state = 2;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 40000, BAST_g2_P_QpskSpinvScan1); 

         case 2:
            BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS4, &val);
            val &= 0x1B;
            if (val == 0x09)
               goto done;
            else
            {
               hChn->count1++;
               if (hChn->count1 < 9)
                  hChn->funct_state = 0;
               else
               {
                  BAST_g2_P_AndRegister(h, BCHP_SDS_VTCTL1, 0xFFFFFF9F);
                  goto done;
               }
            }
            break;
      }
   }

   done:
   return BAST_g2_P_QpskNarrowBw(h);
}


/******************************************************************************
 BAST_g2_P_QpskNarrowBw()
******************************************************************************/
BERR_Code BAST_g2_P_QpskNarrowBw(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   val = 0x04003020;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FSYN, &val);

   hChn->count1 = 0;
   hChn->count2 = 4;

   return BAST_g2_P_QpskNarrowBw1(h); 
}


/******************************************************************************
 BAST_g2_P_QpskTransferFreqOffset()
******************************************************************************/
BERR_Code BAST_g2_P_QpskTransferFreqOffset(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, Q_hi, Q_lo, P_hi, P_lo, fli_step;  
   int32_t carrier_error, div;

   if ((hChn->bExternalTuner) || (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK))
   {
      /* skip freq transfer */
      return BAST_g2_P_QpskAcquire2(h);
   }
   
   carrier_error = BAST_g2_P_GetCarrierError(h);
   
   BAST_g2_P_FreezeLoops(h);
   
   /* clear FLI and PLI */
   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FLI, (uint32_t *)&val);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PLI, (uint32_t *)&val);
     
   /* set tracking tuner LPF bw */
   val = hChn->trackingAnactl4;
   BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val); 
   
   /* retune to get optimal popcap settings */  
   /* put in 10KHz offset in MB2 */
   hChn->tunerOffset = BAST_DCO_OFFSET;
   hChn->tunerIfStep = carrier_error;
   
   /* put 15KHz offset in FLI to compensate MB2 offset */
   BAST_g2_P_ReadRegister(h, BCHP_SDS_AFECTL1, &val);
   if (val & 0x02)
      div = 16;
   else
      div = 8;
   fli_step = BAST_DCO_OFFSET * div;      
   BAST_MultU32U32(fli_step, 1073741824, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FLI, &Q_lo);
     
BDBG_MSG(("transfer freq offset %d", carrier_error));     
   return BAST_g2_P_TunerQuickTune(h, BAST_g2_P_QpskTransferFreqOffset0);
}


/******************************************************************************
 BAST_g2_P_QpskTransferFreqOffset0()
******************************************************************************/
BERR_Code BAST_g2_P_QpskTransferFreqOffset0(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   hChn->tuneMixStatus |= BAST_TUNE_MIX_RESET_IF_PREVIOUS_SETTING_FAILS;

   BAST_g2_P_UnfreezeLoops(h);
   
   /* wait */
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 20000, BAST_g2_P_QpskAcquire2); 
}


/******************************************************************************
 BAST_g2_P_QpskNarrowBw1()
******************************************************************************/
BERR_Code BAST_g2_P_QpskNarrowBw1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t cdelta, bdelta, bw, damp;

   if (hChn->count1 >= hChn->count2)
      return BAST_g2_P_QpskTransferFreqOffset(h);
   else
   {
      cdelta = (hChn->carrierTrkBw - hChn->cmin) >> 2;
      bw = hChn->carrierTrkBw - cdelta * (hChn->count1 + 1);
      damp = hChn->carrierTrkDamp + ((hChn->count1 + 1) << 2);
      BAST_g2_P_SetCarrierBw(h, bw, damp);

      hChn->count1++;

      bdelta = (hChn->baudBw - hChn->bmin) >> 2;
      bw = hChn->baudBw - hChn->count1 * bdelta;
      BAST_g2_P_SetBaudBw(h, bw, hChn->baudDamp);
            
      return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK) ? 100000 : 1000, BAST_g2_P_QpskNarrowBw1); 
   }
}


/******************************************************************************
 BAST_g2_P_QpskAcquire2()
******************************************************************************/
BERR_Code BAST_g2_P_QpskAcquire2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   static const uint32_t script_qpsk_0[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_FSYN, 0x04003020),
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW, 0x03030000),   
#if 0      
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL5, 0x01),
#endif 
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL15, 0xFB), 
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL16, 0xEE),  
 

      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_EQBLND, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLTD, 0x16000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLTD, 0x16000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLQCFD, 0x20),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_2[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_VLCTL1, 0x04),         /* Freeze VLC */
      BAST_SCRIPT_WRITE(BCHP_SDS_VLCTL2, 0x07),
      BAST_SCRIPT_WRITE(BCHP_SDS_VLCTL3, 0x04),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLTD, 0x28000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLQCFD, 0x4A),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_3[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_EQFFE1, 0x71),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQFFE2, 0x02),      
      BAST_SCRIPT_WRITE(BCHP_SDS_EQMU, 0x44),
#if 0      
      BAST_SCRIPT_WRITE(BCHP_SDS_DCOCTL1, 0x31),
      BAST_SCRIPT_WRITE(BCHP_SDS_DCOCTL2, 0x09),
#endif      
      BAST_SCRIPT_WRITE(BCHP_SDS_SNRCTL, 0x03),
      BAST_SCRIPT_EXIT      
   };
   
   if (hChn->bExternalTuner == false)
      BAST_g2_P_ProcessScript(h, script_qpsk_0);
   else
   {
      val = 0x03030000;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_ABW, &val);
   }
   
   BAST_g2_P_ProcessScript(h, script_qpsk_1);

   BAST_g2_P_QpskSetFinalBlBw(h);
   BAST_g2_P_ProcessScript(h, script_qpsk_2);
   BAST_g2_P_QpskSetOqpsk2(h);
   BAST_g2_P_QpskSetClctl3(h);

   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PLI, &val);
   val = 0x71;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLMISC2, &val);

   BAST_g2_P_QpskSetFinalFlBw(h);
   BAST_g2_P_SetEqffe3(h);

   BAST_g2_P_ProcessScript(h, script_qpsk_3);
#if 0   
   if (hChn->bExternalTuner)
   {
      val = 0x00; /* 0x20; */
      BAST_g2_P_WriteRegister(h, BCHP_SDS_DCOCTL1, &val);
   }
#endif

   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 1000 * 20, BAST_g2_P_QpskAcquire3); 
}


/******************************************************************************
 BAST_g2_P_QpskSetOpll()
******************************************************************************/
void BAST_g2_P_QpskSetOpll(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, fmod, stuff, rs_mode, data0, data2, data3, data6, data7, data4;
   uint32_t lval1, lval2, gcf, P_hi, P_lo, Q_hi, Q_lo, lval3;

   BAST_g2_P_ReadRegister(h, BCHP_SDS_FMOD, &fmod);
   rs_mode = (fmod & 0x0C00);
   stuff = fmod & 0x1F;

   if ((rs_mode == 0x0800) || (rs_mode == 0x0000))
   {
      /* DVB and DCII */
      data2 = 188;
      data3 = 204;
   }
   else
   {
      /* DTV */
      data2 = 130;
      data3 = 147;
   }

   if (fmod & 0x00008000)
      data4 = 16; /* RS decoder disabled */
   else
      data4 = 0;

   lval1 = data2 * (data2 + data4 + stuff);
   lval2 = data3 * data2;

   if (data3 != 147)
      data3 = 188;

   /* BDBG_MSG(("BAST_g2_P_QpskSetOpll(): actualMode=%d", hChn->actualMode)); */
   switch (hChn->actualMode)
   {
      case BAST_Mode_eDvb_1_2:
      case BAST_Mode_eDss_1_2:
      case BAST_Mode_eDcii_1_2:
         data6 = 1;
         data7 = 2;
         break;

      case BAST_Mode_eDvb_2_3:
      case BAST_Mode_eDss_2_3:
      case BAST_Mode_eDcii_2_3:
         data6 = 2;
         data7 = 3;
         break;

      case BAST_Mode_eDvb_3_4:
      case BAST_Mode_eDcii_3_4:
         data6 = 3;
         data7 = 4;
         break;

      case BAST_Mode_eDvb_5_6:
      case BAST_Mode_eDcii_5_6:
         data6 = 5;
         data7 = 6;
         break;

      case BAST_Mode_eDss_6_7:
         data6 = 6;
         data7 = 7;
         break;

      case BAST_Mode_eDvb_7_8:
      case BAST_Mode_eDcii_7_8:
         data6 = 7;
         data7 = 8;
         break;

      case BAST_Mode_eDcii_5_11:
         data6 = 5;
         data7 = 11;
         break;

      case BAST_Mode_eDcii_3_5:
         data6 = 3;
         data7 = 5;
         break;

      case BAST_Mode_eDcii_4_5:
         data6 = 4;
         data7 = 5;
         break;

      default:
         data6 = data7 = 1;
         break;
   }

   lval1 *= (data6 << 1);  /* lval1 = m */

   BAST_g2_P_ReadRegister(h, BCHP_SDS_CGCTRL, &val);
   if (val & 0x02)
      data0 = 2;
   else
      data0 = 4;
   lval2 *= (data7 * data0);  /* lval2 = n */

   /* are we in split mode? */
   BAST_g2_P_ReadRegister(h, BCHP_SDS_VTCTL2, &val);
   if (val & 0x04)
      lval2 = lval2 << 1;

   BAST_MultU32U32(hChn->acqParams.symbolRate, data0 * lval1, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, lval2, &Q_hi, &Q_lo);

   if (hChn->xportCtl & BAST_G2_XPORT_CTL_DELH)
   {
      if (hChn->xportCtl & BAST_G2_XPORT_CTL_HEAD4)
         data0 = 4;
      else
         data0 = 1;
      BAST_MultU32U32(Q_lo, data3-data0, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, data3, &Q_hi, &Q_lo);
   }

   gcf = BAST_g2_P_GCF(lval1, lval2);
   lval1 /= gcf;
   lval2 /= gcf;

   BAST_g2_P_OrRegister(h, BCHP_SDS_OIFCTL3, 0x2);   
   BAST_g2_P_AndRegister(h, BCHP_SDS_OIFCTL3, ~0x2);   
   BAST_g2_P_WriteRegister(h, BCHP_SDS_OPLL, &lval1);
   lval3 = lval2 - lval1;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_OPLL2, &lval3);

#if 0  /* TBD... */
   if (xport_clock_adjust[which_rcvr])
   {
      fval1 = ((float)lval1 / (float)lval2) * (1.0 + (float)xport_clock_adjust[which_rcvr] / 2550.0);
      *((unsigned long idata *)&isb_byte3) = (unsigned long)((fval1 * (float)lval3) / (1.0 - fval1));
      isb_write_address(SDS_OPLL);

      BAST_g2_P_WriteRegister(h, BCHP_SDS_OPLL2, &lval3);
   }
#endif
}


/******************************************************************************
 BAST_g2_P_QpskAcquire3()
******************************************************************************/
BERR_Code BAST_g2_P_QpskAcquire3(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   /* reset output interface */
   BAST_g2_P_OrRegister(h, BCHP_SDS_PSCTL, 0x01);
   BAST_g2_P_AndRegister(h, BCHP_SDS_PSCTL, ~0x01);
   
   hChn->actualMode = BAST_g2_P_GetActualMode(h);  
  
   BAST_g2_P_QpskSetOpll(h);
   if (BAST_g2_P_SetOpll(h) != BERR_SUCCESS)
      return BAST_g2_P_QpskReacquire(h);
   
   val = (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_RS_DISABLE) ? 0x08 : 0x00;
   val |= 0x01;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FECTL, &val);
   val &= ~0x01;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FECTL, &val);

   if (BAST_g2_P_CarrierOffsetOutOfRange(h))
   {
      BDBG_MSG(("carrier offset out of range"));
      return BAST_g2_P_QpskDelayReacquire(h);
   }
   hChn->initFreqOffset = BAST_g2_P_GetCarrierError(h);   
   
   if (BAST_MODE_IS_DTV(hChn->acqParams.mode))
   {
      val = 0x69;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_EQMU, &val);      
   }
   
   hChn->acqState = BAST_AcqState_eWaitForLock;
   hChn->freqTransferInt = 0;
   hChn->lockFilterTime = BAST_LOCK_INDICATION_FILTER_TIME;
   hChn->lockFilterRamp = BAST_LOCK_INDICATION_RAMP_INIT;
        
   hChn->lockIsrFlag = 0xFF;
   BAST_g2_P_QpskNotLock_isr((void*)h, 0);        
   BINT_EnableCallback_isr(hChn->hQpskLockCb);
   BINT_EnableCallback_isr(hChn->hQpskNotLockCb);      
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_QpskAcquire4()
******************************************************************************/
BERR_Code BAST_g2_P_QpskAcquire4(BAST_ChannelHandle h)
{
   uint32_t val;

   /* BDBG_MSG(("freq detector disabled")); */
   val = 0x0A;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLQCFD, &val);
                  
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100000, BAST_g2_P_QpskMonitor);     
}
 
 
/******************************************************************************
 BAST_g2_P_QpskMonitor()
******************************************************************************/
BERR_Code BAST_g2_P_QpskMonitor(BAST_ChannelHandle h)
{  
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   if (hChn->bExternalTuner == false)
      BAST_g2_P_3StageAgc_isr(h, 0);         

   BAST_g2_P_TransferFreqOffset(h);
   if (hChn->bForceReacq)
      return BAST_g2_P_QpskReacquire(h);

#ifndef BAST_EXCLUDE_STATUS_EVENTS   
   BAST_g2_P_CheckStatusIndicators(h);
#endif   

   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100000, BAST_g2_P_QpskMonitor);   
}


/******************************************************************************
 BAST_g2_P_QpskLockStable()
******************************************************************************/
BERR_Code BAST_g2_P_QpskLockStable(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eBer);  
      
   BDBG_MSG(("BAST_g2_P_QpskLockStable()"));      
   if (hChn->acqTimeState != 2)
      BAST_g2_P_UpdateAcquisitionTime(h);      
 
   BAST_g2_P_IndicateLocked(h);
   hChn->acqState = BAST_AcqState_eMonitor;      
   hChn->lockFilterTime = BAST_LOCK_INDICATION_FILTER_TIME; /* reset lock indication filter time */
   hChn->lockFilterRamp = BAST_LOCK_INDICATION_RAMP_INIT;
   hChn->lockIsrFlag = 0xFF;
   if (hChn->firstTimeLock == 0)
   {
      hChn->firstTimeLock = 1;
      BAST_g2_P_ResetErrorCounters(h);
   }

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_QpskLock_isr() - callback routine for not_locked->locked transition
******************************************************************************/
void BAST_g2_P_QpskLock_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   BAST_Mode mode;
   uint32_t val;
   BSTD_UNUSED(param);
   
   BAST_g2_P_IncrementInterruptCounter(h, BAST_Sds_IntID_eLock);
   
   BDBG_MSG(("BAST_g2_P_QpskLock_isr()"));
   if (hChn->bExternalTuner == false)
      BAST_g2_P_3StageAgc_isr(h, 0);
   
   if (BAST_g2_P_QpskCheckCodeRateScan(h) == false)
   {
      BDBG_ERR(("BAST_Mode 0x%X not allowed", hChn->actualMode));
      BAST_g2_P_QpskReacquire(h);
   }
   
   val = 0xFF;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQSTS2, &val);
   BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS2, &val);
   if ((val & 0x08) == 0)
   {
      BDBG_MSG(("actually not locked in QpskLock_isr(), calling not lock isr"));
      BAST_g2_P_QpskNotLock_isr(p, 0);
      return;
   }
  
   hChn->lockIsrFlag = 1;
      
   mode = BAST_g2_P_QpskGetActualMode(h);
   if (mode != hChn->actualMode)
   {
      BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &val);   
      BDBG_MSG(("different code rate detected (curr_mode=%d, expected_mode=%d, VST=0x%X", mode, hChn->actualMode, val));
      BAST_g2_P_QpskReacquire(h);
      return;
   }
      
   if (hChn->acqState == BAST_AcqState_eMonitor)
   {      
      hChn->lockFilterTime += ((hChn->lockFilterRamp++ > 0 ? hChn->lockFilterRamp : 1) * BAST_LOCK_INDICATION_INCREMENT);
      if (hChn->lockFilterTime > BAST_LOCK_INDICATION_FILTER_TIME_MAX)
         hChn->lockFilterTime = BAST_LOCK_INDICATION_FILTER_TIME_MAX;
   }
   
   /* BDBG_MSG(("enabling lock stable timer for %d usecs", hChn->lockFilterTime)); */
   BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eGen1, hChn->lockFilterTime, BAST_g2_P_QpskLockStable);
   BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 30000 * 20, BAST_g2_P_QpskAcquire4);
}


/******************************************************************************
 BAST_g2_P_QpskGetActualMode() - determine actual code rate found during scan mode
******************************************************************************/
BAST_Mode BAST_g2_P_QpskGetActualMode(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BAST_Mode actualMode = hChn->acqParams.mode;
   uint32_t val;
   
   if ((actualMode != BAST_Mode_eDvb_scan) &&
       (actualMode != BAST_Mode_eDss_scan) &&
       (actualMode != BAST_Mode_eDcii_scan))
      return hChn->actualMode;
       
   BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &val);
   val = (val >> 24) & 0x0F;
   
   switch (val)
   {
      case 0:
      case 9:
         if (actualMode == BAST_Mode_eDvb_scan)
            actualMode = BAST_Mode_eDvb_1_2;
         else if (actualMode == BAST_Mode_eDss_scan)
            actualMode = BAST_Mode_eDss_1_2;
         else if (actualMode == BAST_Mode_eDcii_scan)
            actualMode = BAST_Mode_eDcii_1_2;
         break;
      case 8:
         if (actualMode == BAST_Mode_eDcii_scan)      
            actualMode = BAST_Mode_eDcii_5_11;
         break;
      case 1:
      case 11:
         if (actualMode == BAST_Mode_eDvb_scan)
            actualMode = BAST_Mode_eDvb_2_3;
         else if (actualMode == BAST_Mode_eDss_scan)
            actualMode = BAST_Mode_eDss_2_3;
         else if (actualMode == BAST_Mode_eDcii_scan)
            actualMode = BAST_Mode_eDcii_2_3;
         break;
      case 10:
         if (actualMode == BAST_Mode_eDcii_scan)      
            actualMode = BAST_Mode_eDcii_3_5;
         break;      
      case 2:
      case 12:
         if (actualMode == BAST_Mode_eDvb_scan)
            actualMode = BAST_Mode_eDvb_3_4;
         else if (actualMode == BAST_Mode_eDcii_scan)
            actualMode = BAST_Mode_eDcii_3_4;
         break;
      case 13:
         if (actualMode == BAST_Mode_eDcii_scan)      
            actualMode = BAST_Mode_eDcii_4_5;
         break;           
      case 3:
      case 14:
         if (actualMode == BAST_Mode_eDvb_scan)
            actualMode = BAST_Mode_eDvb_5_6;
         else if (actualMode == BAST_Mode_eDcii_scan)
            actualMode = BAST_Mode_eDcii_5_6;
         break;
      case 4:
         if (actualMode == BAST_Mode_eDss_scan)
            actualMode = BAST_Mode_eDss_6_7;
         break;
      case 5:
      case 15:
         if (actualMode == BAST_Mode_eDvb_scan)
            actualMode = BAST_Mode_eDvb_7_8;
         else if (actualMode == BAST_Mode_eDcii_scan)
            actualMode = BAST_Mode_eDcii_7_8;
         break;
      default:
         BDBG_ERR(("BAST_g2_P_QpskGetActualMode(): invalid mode"));
         break;
   }
   
   return actualMode;
}


/******************************************************************************
 BAST_g2_P_QpskNotLock_isr() - callback routine for locked->not_locked transition
******************************************************************************/
void BAST_g2_P_QpskNotLock_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;   
   uint32_t val;
   BSTD_UNUSED(param);
     
   BDBG_MSG(("BAST_g2_P_QpskNotLock_isr()"));

   BAST_g2_P_IncrementInterruptCounter(h, BAST_Sds_IntID_eNotLock);
   
   if (hChn->lockIsrFlag == 0xFF)
      goto process_not_locked;
 
   if (hChn->bExternalTuner == false)
      BAST_g2_P_3StageAgc_isr(h, 0);
     
   val = 0xFF;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQSTS2, &val);
   BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS2, &val);
   if (val & 0x08)
   {
      BDBG_MSG(("actually locked in QpskNotLock_isr(), calling lock isr"));
      BAST_g2_P_QpskLock_isr(p, 0);
      return;
   }
   
   if (hChn->lockIsrFlag == 0)
   {
      BDBG_MSG(("ignoring not lock isr"));
      return;
   }
   
   process_not_locked:
   hChn->lockIsrFlag = 0;  
   BAST_g2_P_IndicateNotLocked(h);
   
   val = 0x4A;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLQCFD, &val);
   
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eGen1);
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eBaud);
      
   BAST_g2_P_ReadRegister(h, BCHP_SDS_BCKTMR, &val);
   if ((val & 0x04) == 0)
   {
      BDBG_MSG(("enable reacquisition timer for %d usecs", ((BAST_g2_P_Handle *)(h->pDevice->pImpl))->reacqDelay));
      BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBer, 
                              ((BAST_g2_P_Handle *)(h->pDevice->pImpl))->reacqDelay, 
                              BAST_g2_P_QpskReacquire);
   }
}


/******************************************************************************
 BAST_g2_P_QpskDelayReacquire()
******************************************************************************/
BERR_Code BAST_g2_P_QpskDelayReacquire(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   if ((hChn->blindScanStatus == BAST_BlindScanStatus_eAcquire) || (hChn->acqCount < 5))
      return BAST_g2_P_QpskReacquire(h);
      
   BAST_g2_P_IndicateNotLocked(h);   
   BAST_g2_P_DisableChannelInterrupts_isr(h, false, false);  
   
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 
      ((BAST_g2_P_Handle *)(h->pDevice->pImpl))->reacqDelay, BAST_g2_P_QpskReacquire);
}


/******************************************************************************
 BAST_g2_P_QpskReacquire()
******************************************************************************/
BERR_Code BAST_g2_P_QpskReacquire(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   BERR_Code retCode = BERR_SUCCESS;

   BDBG_MSG(("BAST_g2_P_QpskReacquire"));

#if 0
   if ((hChn->acqState == BAST_AcqState_eWaitForLock) || (hChn->acqState == BAST_AcqState_eMonitor))
   {
      /* are we really not locked? */
      val = 0xFF;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQSTS2, &val);
      BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS2, &val);
      if ((val & 0xc) == 0x8)
      {
         BDBG_MSG(("BAST_g2_P_QpskReacquire(): actually locked in QpskNotLock_isr(), calling lock isr"));
         BAST_g2_P_QpskLock_isr((void*)h, 0);
         return BERR_SUCCESS;
      }
   } 
#endif   
   
   BAST_g2_P_IndicateNotLocked(h);
   BAST_g2_P_DisableChannelInterrupts_isr(h, false, false);
#ifndef BAST_EXCLUDE_STATUS_EVENTS   
   BAST_g2_P_ResetStatusIndicators(h);
#endif
    
   if ((hChn->bExternalTuner == false) && (hChn->tunerCutoffWide))
   {
      val = hChn->trackingAnactl4 + (hChn->tunerCutoffWide << 2);   
      BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val);
   }
   
   if (hChn->blindScanStatus == BAST_BlindScanStatus_eAcquire)
      return BAST_g2_P_BlindReacquire(h);
      
   hChn->acqCount++;

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_REACQ_DISABLE)
   {
      /* give up */
      give_up:
      hChn->acqState = BAST_AcqState_eIdle;
      BDBG_MSG(("failed to acquire"));
      return retCode; 
   }

   if (hChn->maxReacq && (hChn->acqCount > hChn->maxReacq))
      goto give_up;

   if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK) && (hChn->bExternalTuner == false))
      goto retune;
      
   /* retune and acquire again */
   hChn->tunerIfStep = 0;
   if ((hChn->bExternalTuner == false) && ((hChn->tunerCtl & BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE) == 0))
   {
      if (hChn->acqCount & 1)
         goto acquire_without_retune;
         
      retune:
      hChn->acqState = BAST_AcqState_eTuning;    
      retCode = BAST_g2_P_TunerSetFreq(h, BAST_g2_P_Acquire);
   }
   else 
   {
      acquire_without_retune:
      retCode = BAST_g2_P_QpskAcquire(h);
   }
   return retCode;
}


/******************************************************************************
 BAST_g2_P_QpskUpdateErrorCounters()
******************************************************************************/
void BAST_g2_P_QpskUpdateErrorCounters(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   BAST_g2_P_ReadRegister(h, BCHP_SDS_FERR, &val);
   hChn->rsCorr += ((val >> 16) & 0xFFFF);
   hChn->rsUncorr += (val & 0xFFFF);

   /* get pre-Viterbi error count */
   BAST_g2_P_OrRegister(h, BCHP_SDS_VTCTL2, 0x80); /* take snapshot of VREC&VRCV and clear them */
   BAST_g2_P_ReadRegister(h, BCHP_SDS_VREC, &val);
   hChn->preVitErrors += val;
   
   BAST_g2_P_UpdateErrorCounters(h);
}
