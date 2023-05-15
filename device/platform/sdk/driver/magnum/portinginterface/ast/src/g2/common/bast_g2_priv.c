/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/137 $
 * $brcm_Date: 10/15/12 4:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2_priv.c $
 * 
 * Hydra_Software_Devel/137   10/15/12 4:07p enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/136   9/6/12 7:14p ronchan
 * SWSATFE-214: fix compile errors
 * 
 * Hydra_Software_Devel/135   9/6/12 6:12p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/134   7/9/12 11:54a ronchan
 * SWSATFE-205: enter InitChannels in ISR context
 * 
 * Hydra_Software_Devel/133   3/28/12 9:57a ronchan
 * SWSATFE-185: SPLL0 power down from SDS1 prevents mi2c access to 3445 on
 * SDS0
 * 
 * Hydra_Software_Devel/132   3/9/12 5:09p ronchan
 * SW7335-1402: power up ch0 spll to access dacpll registers
 * 
 * Hydra_Software_Devel/131   3/8/12 10:47a ronchan
 * SWSATFE-99: fixed logic for fsk channel configuration
 * 
 * Hydra_Software_Devel/130   2/4/11 9:59a ronchan
 * SW7340-250: fixed trace for case when BDBG_DEBUG_BUILD=0
 * 
 * Hydra_Software_Devel/129   1/31/11 5:23p ronchan
 * SWSATFE-99: added implementation for BAST_SetFskChannel() and
 * BAST_GetFskChannel()
 * 
 * Hydra_Software_Devel/128   1/27/11 4:17p enavarro
 * SW7346-21: added implementation for BAST_SetPeakScanSymbolRateRange()
 * and BAST_GetPeakScanSymbolRateRange()
 * 
 * Hydra_Software_Devel/127   11/30/10 10:43a ckpark
 * SWSATFE-91: added reacqCtl config parameter
 * 
 * Hydra_Software_Devel/126   11/30/10 9:36a ckpark
 * SWSATFE-80: implemented BAST_SetNetworkSpec() and BAST_GetNetworkSpec()
 * 
 * Hydra_Software_Devel/125   10/6/10 2:53p ckpark
 * SWSATFE-82: fixed bug in agf status in BAST_73xx_P_GetChannelStatus()
 * 
 * Hydra_Software_Devel/124   10/6/10 10:10a enavarro
 * SWSATFE-82: updated input power estimate
 * 
 * Hydra_Software_Devel/123   9/30/10 6:10p ronchan
 * SWSATFE-80: clarified case for reply expected override
 * 
 * Hydra_Software_Devel/122   9/30/10 5:39p ronchan
 * SWSATFE-80: set setting to false if diseqc setting not implemented
 * 
 * Hydra_Software_Devel/121   9/30/10 5:20p ronchan
 * SWSATFE-80: fixed get tone detect threshold in GetDiseqcSettings
 * 
 * Hydra_Software_Devel/120   9/30/10 5:02p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/119   9/30/10 9:08a enavarro
 * SWSATFE-80: added implementation functions for
 * SetOutputTransportSettings() and GetOutputTransportSettings()
 * 
 * Hydra_Software_Devel/118   9/17/10 1:51p enavarro
 * SWSATFE-80: added BAST_SetSearchRange(), BAST_GetSearchRange(),
 * BAST_SetTunerFilter(), and BAST_SetAmcScramblingSeq()
 * 
 * Hydra_Software_Devel/117   7/1/10 3:23p ckpark
 * SWSATFE-66: fixed write config for ALT_PLC_ACQ_DAMP and
 * ALT_TRK_ACQ_DAMP config parameters
 * 
 * Hydra_Software_Devel/116   7/1/10 10:35a ronchan
 * SWSATFE-65: add diseqc bit detection threshold parameter
 * 
 * Hydra_Software_Devel/115   2/4/10 11:12a ronchan
 * SWSATFE-27: remove diseqc spll serialization
 * 
 * Hydra_Software_Devel/114   1/19/10 10:54a ronchan
 * SWSATFE-27: remove redundant calls to set diseqc clock
 * 
 * Hydra_Software_Devel/113   1/18/10 4:23p ronchan
 * SWSATFE-27: reset to default sample freq if aborting acquisition
 * 
 * Hydra_Software_Devel/112   10/26/09 11:05a enavarro
 * SW7342-54: use common function for initializing default BAST_Handle
 * 
 * Hydra_Software_Devel/111   10/21/09 4:06p ronchan
 * SW7342-52: fixed power down issue due to shared spll on 7342
 * 
 * Hydra_Software_Devel/110   9/22/09 5:08p ronchan
 * SW7325-606: power down dac pll for single channel case
 * 
 * Hydra_Software_Devel/109   9/10/09 9:31a enavarro
 * SW7325-593: return raw AIF value in BAST_GetChannelStatus()
 * 
 * Hydra_Software_Devel/108   9/2/09 4:38p ronchan
 * SW7325-585: wait for init done when powering up channel
 * 
 * Hydra_Software_Devel/107   9/1/09 4:36p enavarro
 * HW4506-60: added new configuration parameters for PLC override
 * 
 * Hydra_Software_Devel/106   8/27/09 5:02p enavarro
 * HW4506-78: add code to support DFT frequency scan
 * 
 * Hydra_Software_Devel/105   8/7/09 3:32p ronchan
 * PR 55860: skip AFEC interrupt disable if powered down
 * 
 * Hydra_Software_Devel/104   8/3/09 5:31p ronchan
 * PR 55860: clear interrupt callbacks after disabling callbacks
 * 
 * Hydra_Software_Devel/103   7/30/09 6:56p ronchan
 * PR 57227: prevent LDPC or TURBO acquisitions on external channel
 * 
 * Hydra_Software_Devel/102   7/16/09 5:31p ronchan
 * PR 54581: bypass initialization power up when power management excluded
 * 
 * Hydra_Software_Devel/101   6/18/09 2:47p enavarro
 * PR 56135: initialize turboScanCurrMode
 * 
 * Hydra_Software_Devel/100   6/9/09 6:33p ronchan
 * PR 55876: added 3stage_agc_top config parameter
 * 
 * Hydra_Software_Devel/99   5/26/09 3:35p enavarro
 * PR 53391: return proper tuner LPF setting in BAST_GetChannelStatus()
 * for SET_FILTER_MANUAL mode
 * 
 * Hydra_Software_Devel/98   5/18/09 4:58p ronchan
 * PR 54581: force power up when initializing or resetting
 * 
 * Hydra_Software_Devel/97   5/11/09 2:01p ronchan
 * PR 54984: clean up diseqc voltage events and callbacks
 * 
 * Hydra_Software_Devel/96   4/30/09 9:34a ronchan
 * PR 54581: remove auto-power up when closing channel
 * 
 * Hydra_Software_Devel/95   4/27/09 3:36p ronchan
 * PR 54629: protect tune acquire call if sds powered down
 * 
 * Hydra_Software_Devel/94   4/27/09 2:28p ronchan
 * PR 54581: bypass SWM deregistration if FTM core powered down, destroy
 * FTM callbacks when closing device, null out channel and device handles
 * during close
 * 
 * Hydra_Software_Devel/92   4/20/09 2:03p enavarro
 * PR 37769: added counters for all interrupts
 * 
 * Hydra_Software_Devel/91   4/13/09 3:48p ronchan
 * PR 54006: enter tuning and acquisition in isr context
 * 
 * Hydra_Software_Devel/90   4/13/09 10:56a ronchan
 * PR 54006: created DisableChannelInterrupts_isr() for ISR context
 * 
 * Hydra_Software_Devel/89   3/2/09 6:09p ronchan
 * PR 52712: added network_spec config parameter
 * 
 * Hydra_Software_Devel/88   2/26/09 2:22p enavarro
 * PR 51665: initialize tuneMixStatus in channel handle
 * 
 * Hydra_Software_Devel/87   2/18/09 2:40p ronchan
 * PR 52262: added debug function traces to main api calls
 * 
 * Hydra_Software_Devel/86   2/18/09 10:58a ronchan
 * PR 52251: implemented bypass tune option
 * 
 * Hydra_Software_Devel/85   2/12/09 2:09p enavarro
 * PR 51503: kill transport output before tuning
 * 
 * Hydra_Software_Devel/84   1/22/09 11:38a enavarro
 * PR 50993: call BAST_73xx_P_UpdateErrorCounters() for Turbo mode in
 * BAST_73xx_P_GetChannelStatus()
 * 
 * Hydra_Software_Devel/83   1/19/09 4:05p ckpark
 * PR 51203: added AGF power estimation
 * 
 * Hydra_Software_Devel/82   1/13/09 3:02p ronchan
 * PR 51069: deregister tuners from FTM upon closing device
 * 
 * Hydra_Software_Devel/80   1/12/09 3:42p enavarro
 * PR 50993: added TURBO_CTL configuration parameter
 * 
 * Hydra_Software_Devel/79   1/8/09 4:07p enavarro
 * PR 50925: dont clear acqTime in BAST_AbortAcq()
 * 
 * Hydra_Software_Devel/78   12/22/08 2:33p ronchan
 * PR 50635: added configuration parameter for external tuner AGC
 * thresholds
 * 
 * Hydra_Software_Devel/77   12/3/08 4:20p ronchan
 * PR 49936: avoid spll power down if diseqc is needed
 * 
 * Hydra_Software_Devel/76   11/4/08 3:00p ronchan
 * PR 37881: power down afec and tfec if mi2c controlling 3445
 * 
 * Hydra_Software_Devel/75   10/31/08 3:17p enavarro
 * PR 37769: added CNR_THRESH1 config parameter
 * 
 * Hydra_Software_Devel/74   10/20/08 3:33p ronchan
 * PR 37769: renamed channel reset
 * 
 * Hydra_Software_Devel/73   10/17/08 2:49p ronchan
 * PR 37769: updated channel reset, changed soft reset to call channel
 * reset
 * 
 * Hydra_Software_Devel/72   10/13/08 5:04p ronchan
 * PR 37769: added BCM3445_CTL config parameter
 * 
 * Hydra_Software_Devel/71   10/13/08 3:10p ronchan
 * PR 37769: added channel reset function, fixed GetApVersion() to return
 * proper chip version
 * 
 * Hydra_Software_Devel/70   10/8/08 11:20a ckpark
 * PR 37769: use filtered lock status in BAST_GetLockStatus()
 * 
 * Hydra_Software_Devel/69   10/6/08 10:38a ronchan
 * PR 37769: added option to exclude peak scan
 * 
 * Hydra_Software_Devel/68   10/1/08 3:50p enavarro
 * PR 37769: added STUFF_BYTES config parameter
 * 
 * Hydra_Software_Devel/67   10/1/08 2:31p ronchan
 * PR 37769: decoupled ftm reset from soft reset
 * 
 * Hydra_Software_Devel/66   10/1/08 2:13p enavarro
 * PR 37769: reset error counters at start of BAST_TuneAcquire(); put code
 * for updating error counters in separate functions
 * 
 * Hydra_Software_Devel/65   9/29/08 4:23p ronchan
 * PR 37769: implemented soft reset
 * 
 * Hydra_Software_Devel/64   9/22/08 2:45p ronchan
 * PR 37769: added bounds checking for some config params
 * 
 * Hydra_Software_Devel/63   9/15/08 2:19p ronchan
 * PR 37769: removed output pll power down
 * 
 * Hydra_Software_Devel/62   9/15/08 11:16a ronchan
 * PR 37769: abort acq before powerdown, don't power down spll if mi2c
 * controlling 3445 lna
 * 
 * Hydra_Software_Devel/61   8/27/08 11:36a ronchan
 * PR 37769: fixed compile warning
 * 
 * Hydra_Software_Devel/60   8/25/08 4:35p enavarro
 * PR 37769: added DFT_RANGE_START, DFT_RANGE_END, and DFT_SIZE config
 * parameters
 * 
 * Hydra_Software_Devel/59   8/22/08 3:29p enavarro
 * PR 37769: changed peak scan
 * 
 * Hydra_Software_Devel/58   8/22/08 2:03p enavarro
 * PR 37769: check if AFEC/TFEC is powered before accessing AFEC/TFEC
 * registers
 * 
 * Hydra_Software_Devel/57   8/20/08 10:30a ronchan
 * PR 37769: fixed diseqc power down
 * 
 * Hydra_Software_Devel/56   8/15/08 3:36p ronchan
 * PR 37669: added diseqc_tone_threshold config parameter
 * 
 * Hydra_Software_Devel/55   8/14/08 3:59p enavarro
 * PR 37769: accumulate LDPC block counters
 * 
 * Hydra_Software_Devel/54   8/5/08 4:41p ronchan
 * PR 37881: removed '//' style comments
 * 
 * Hydra_Software_Devel/53   7/21/08 4:49p enavarro
 * PR 37769: removed snr_threshold1 and snr_threshold2 config parameters
 * 
 * Hydra_Software_Devel/52   7/9/08 4:52p ronchan
 * PR 37769: don't call ResetStatusIndicators() if status events excluded
 * 
 * Hydra_Software_Devel/51   7/8/08 3:53p ronchan
 * PR 37881: added config parameters for tx power & channel select
 * 
 * Hydra_Software_Devel/50   7/3/08 9:53a enavarro
 * PR 37769: reworked BAST_73xx_P_QpskAcquire0()
 * 
 * Hydra_Software_Devel/49   6/26/08 4:35p enavarro
 * PR 37881: fixed pilot status in BAST_73xx_P_GetChannelStatus()
 * 
 * Hydra_Software_Devel/48   6/20/08 11:11a enavarro
 * PR 37769: added rain_fade_window config parameter
 * 
 * Hydra_Software_Devel/47   6/19/08 2:32p enavarro
 * PR 37769: added support for status events; added config parameters for
 * status events
 * 
 * Hydra_Software_Devel/46   6/18/08 10:20a ronchan
 * PR 37769: set lna notch filter for internal tuners only
 * 
 * Hydra_Software_Devel/45   6/13/08 5:17p enavarro
 * PR 37769: set acqState to idle on AbortAcq(); factor in
 * carrierFreqOffset in the carrier_error status if warm start acq locked
 * the channel
 * 
 * Hydra_Software_Devel/44   6/10/08 5:20p ronchan
 * PR 37769: initialize vsense callbacks to null if diseqc unsupported
 * 
 * Hydra_Software_Devel/43   6/10/08 5:14p enavarro
 * PR 37769: added acq_time config parameter
 * 
 * Hydra_Software_Devel/42   6/5/08 10:19a enavarro
 * PR 37769: removed floating point operation
 * 
 * Hydra_Software_Devel/41   6/2/08 3:44p enavarro
 * PR 37769: implemented BAST_73xx_P_GetPeakScanStatus()
 * 
 * Hydra_Software_Devel/40   6/2/08 2:14p enavarro
 * PR 37769: added support for peak scan
 * 
 * Hydra_Software_Devel/39   5/30/08 3:42p enavarro
 * PR 37881: added code to create/destroy hStatusEvent and hPeakScanEvent;
 * implemented BAST_73xx_P_GetPeakScanEventHandle() and
 * BAST_73xx_P_GetStatusEventHandle(); added support for writing/reading
 * peak_scan_sym_rate_min/max config parameters
 * 
 * Hydra_Software_Devel/38   5/30/08 10:21a enavarro
 * PR 37769: added stub for BAST_GetStatusEventHandle()
 * 
 * Hydra_Software_Devel/37   5/29/08 5:02p enavarro
 * PR 37769: added stubs for 73xx implementation of BAST_PeakScan(),
 * BAST_GetPeakScanStatus(), BAST_GetPeakScanEventHandle(), and
 * BAST_EnableStatusInterrupts()
 * 
 * Hydra_Software_Devel/36   5/27/08 3:22p ronchan
 * PR 37769: added options to compile out powerdown functions
 * 
 * Hydra_Software_Devel/35   5/23/08 11:26a ronchan
 * PR 37769: added option to compile out turbo
 * 
 * Hydra_Software_Devel/33   5/22/08 3:09p enavarro
 * PR 37881: fixed problem with reading/writing tuner_cutoff config
 * parameter
 * 
 * Hydra_Software_Devel/32   5/15/08 4:46p enavarro
 * PR 37881: added vco_separation config param; add check for splitter
 * mode only applies to 2 internal tuners
 * 
 * Hydra_Software_Devel/31   5/14/08 2:23p enavarro
 * PR 37769: add support for LDPC pilot scan
 * 
 * Hydra_Software_Devel/30   5/13/08 1:57p enavarro
 * PR 37769: determine number of internal tuners in BAST_InitAp()
 * 
 * Hydra_Software_Devel/29   5/6/08 2:17p ronchan
 * PR 37881: removed support for 7335 A0
 * 
 * Hydra_Software_Devel/28   5/6/08 10:25a enavarro
 * PR 37769: added dtvScanCodeRates, dvbScanCodeRates, and turboScanModes
 * config params
 * 
 * Hydra_Software_Devel/27   4/30/08 3:58p enavarro
 * PR 37769: added call to BAST_73xx_P_SetDefaultSampleFreq() in
 * BAST_73xx_P_PowerUp(); initialize acqCount to 0 in BAST_TuneAcquire()
 * 
 * Hydra_Software_Devel/26   4/28/08 3:04p ronchan
 * PR 37881: added vsense callbacks
 * 
 * Hydra_Software_Devel/24   4/16/08 11:27a enavarro
 * PR 37769: fixed BAST_73xx_P_GetChannelStatus() for turbo mode
 * 
 * Hydra_Software_Devel/23   4/8/08 9:48a ronchan
 * PR 37881: power up afec when closing channel, added vsense threshold
 * config parameters
 * 
 * Hydra_Software_Devel/22   3/27/08 10:55a ronchan
 * PR 37769: fixed bug in close channel
 * 
 * Hydra_Software_Devel/21   3/18/08 4:48p enavarro
 * PR 37769: fixed fec_clock status in BAST_73xx_P_GetChannelStatus()
 * 
 * Hydra_Software_Devel/20   3/18/08 2:44p enavarro
 * PR 37769: updated BAST_73xx_P_GetLockStatus()
 * 
 * Hydra_Software_Devel/19   3/13/08 9:45a enavarro
 * PR 37769: added BAST_G2_CONFIG_REACQ_DELAY config parameter
 * 
 * Hydra_Software_Devel/18   3/12/08 5:59p ronchan
 * PR 37881: enabled ftm interrupts for 7335 A1
 * 
 * Hydra_Software_Devel/17   2/26/08 7:20p agin
 * PR39930:  AST Tuner failing to tune under VxWorks.
 * 
 * Hydra_Software_Devel/16   2/19/08 6:05p jrubio
 * PR38507: fix compile warnings
 * 
 * Hydra_Software_Devel/15   2/13/08 3:56p ronchan
 * PR 37769: implemented sds power down / power up
 * 
 * Hydra_Software_Devel/14   2/6/08 9:58a enavarro
 * PR 37769: added callbacks for turbo lock/not locked interrupts
 * 
 * Hydra_Software_Devel/13   1/31/08 2:08p enavarro
 * PR 37769: change BINT_DisableCallback() to BINT_DisableCallback_isr()
 * 
 * Hydra_Software_Devel/12   1/23/08 1:49p enavarro
 * PR 37769: changed BAST_73xx_P_GetSoftDecisionBuf() to return 15  I&Q
 * pairs
 * 
 * Hydra_Software_Devel/11   1/18/08 11:34a enavarro
 * PR 37769: make BAST_73xx_P_TunerSetFreq() return BERR_Code
 * 
 * Hydra_Software_Devel/10   1/7/08 11:15a enavarro
 * PR 37769: implemented read/write/set RF/IF AGC functions; implemented
 * freeze equalizer function
 * 
 * Hydra_Software_Devel/9   12/19/07 2:34p ronchan
 * PR 37769: disabled ftm interrupts for 7335A0 due to hw bug
 * 
 * Hydra_Software_Devel/8   12/19/07 11:33a enavarro
 * PR 37769: added support for blind scan
 * 
 * Hydra_Software_Devel/7   12/18/07 11:48a enavarro
 * PR 37769: added BAST_73xx_P_GetSnrEstimate()
 * 
 * Hydra_Software_Devel/6   12/17/07 4:45p enavarro
 * PR 37769: added pre-vit error counts to legacy qpsk status
 * 
 * Hydra_Software_Devel/5   12/17/07 11:26a enavarro
 * PR 37769: added critical section in BAST_AbortAcq()
 * 
 * Hydra_Software_Devel/4   12/14/07 4:52p ronchan
 * PR 37881: implemented channel interrupt array
 * 
 * Hydra_Software_Devel/3   12/5/07 4:30p agin
 * PR37869: Integrate SDS into 97325 reference software.
 * 
 * Hydra_Software_Devel/2   12/5/07 10:57a ckpark
 * PR 33769: reworked BAST_GetLockStatus()
 * 
 * Hydra_Software_Devel/1   11/21/07 11:25a ronchan
 * PR 32395: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2_priv.h"

BDBG_MODULE(bast_g2_priv);


/* local routines */
#ifndef BAST_EXCLUDE_PEAK_SCAN
BERR_Code BAST_g2_P_PeakScan1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_PeakScan2(BAST_ChannelHandle h);
#endif

extern BAST_g2_Ftm_P_InterruptCbTable BAST_Ftm_Interrupts[];
extern const uint32_t BAST_Sds_ChannelIntrID[BAST_G2_MAX_CHANNELS][BAST_Sds_MaxIntID];

extern const uint32_t script_sds_init[];
extern const uint32_t script_sds_ext_tuner_init[];


/******************************************************************************
 BAST_g2_P_Open()
******************************************************************************/
BERR_Code BAST_g2_P_Open(
   BAST_Handle *h,                   /* [out] BAST handle */
   BCHP_Handle hChip,                /* [in] chip handle */
   void        *pReg,                /* [in] pointer to register or i2c handle */
   BINT_Handle hInterrupt,           /* [in] Interrupt handle */
   const BAST_Settings *pDefSettings /* [in] default settings */
)
{
   BERR_Code retCode;
   BAST_Handle hDev;
   BAST_g2_P_Handle *hG2Dev;
   uint8_t i;
   
   BSTD_UNUSED(hChip);
   BDBG_ENTER(BAST_g2_P_Open);

   /* allocate memory for the handle */
   hDev = (BAST_Handle)BKNI_Malloc(sizeof(BAST_P_Handle));
   BDBG_ASSERT(hDev);
   BKNI_Memset((void*)hDev, 0, sizeof(BAST_P_Handle));
   hG2Dev = (BAST_g2_P_Handle *)BKNI_Malloc(sizeof(BAST_g2_P_Handle));
   BDBG_ASSERT(hG2Dev);
   BKNI_Memset((void*)hG2Dev, 0, sizeof(BAST_g2_P_Handle));
   hDev->pImpl = (void*)hG2Dev;

   hDev->pChannels = (BAST_P_ChannelHandle **)BKNI_Malloc(BAST_G2_MAX_CHANNELS * sizeof(BAST_P_ChannelHandle *));
   BDBG_ASSERT(hDev->pChannels);
   BKNI_Memset((void*)hDev->pChannels, 0, sizeof(BAST_G2_MAX_CHANNELS * sizeof(BAST_P_ChannelHandle *)));

   /* initialize our handle */
   hG2Dev->hRegister = (BREG_Handle)pReg;
   hG2Dev->hInterrupt = hInterrupt;
   BAST_g2_P_InitHandleDefault(hDev); /* intialize g2 default settings for device handle */
   BAST_g2_P_InitHandle(hDev); /* initialize chip-specific device handle settings */

   BKNI_Memcpy((void*)(&(hDev->settings)), (void*)pDefSettings, sizeof(BAST_Settings));
   hDev->totalChannels = BAST_G2_MAX_CHANNELS;
   for (i = 0; i < BAST_G2_MAX_CHANNELS; i++)
      hDev->pChannels[i] = NULL;

   /* initialize ftm device  */
   retCode = BAST_g2_Ftm_P_InitDevice(hDev);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   /* create ftm callbacks */
	for(i = 0; i < 32 ; i++)
	{
      retCode = BINT_CreateCallback(&(hG2Dev->hFtmDev.hCallback[i]), hInterrupt, BAST_Ftm_Interrupts[i].IntrId, BAST_g2_Ftm_P_HandleInterrupt_isr, (void*)hDev, i);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
	}

   /* create events */
   retCode = BKNI_CreateEvent(&(hG2Dev->hFtmEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(hG2Dev->hInitDoneEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   *h = hDev;

   BDBG_LEAVE(BAST_g2_P_Open);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_Close() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_Close(BAST_Handle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g2_P_Handle *hG2Dev;
   uint8_t i;

   BDBG_ASSERT(h);
   BDBG_ENTER(BAST_g2_P_Close);
   
   hG2Dev = (BAST_g2_P_Handle *)(h->pImpl);
   
   /* deregister tuners from FTM if not powered down */
   if (!hG2Dev->bFtmPoweredDown)
      retCode = BAST_g2_P_ResetFtm(h);
   
   /* destroy ftm callbacks */
   BAST_g2_Ftm_P_EnableFtmInterrupts(h, false);
	for(i = 0; i < 32 ; i++)
      BINT_DestroyCallback(hG2Dev->hFtmDev.hCallback[i]);
   
   BKNI_DestroyEvent(((BAST_g2_P_Handle *)(h->pImpl))->hFtmEvent);
   BKNI_DestroyEvent(((BAST_g2_P_Handle *)(h->pImpl))->hInitDoneEvent);
   BKNI_Free((void*)h->pChannels);
   BKNI_Free((void*)h->pImpl);
   BKNI_Free((void*)h);
   h = NULL;

   BDBG_LEAVE(BAST_g2_P_Close);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetTotalChannels()
******************************************************************************/
BERR_Code BAST_g2_P_GetTotalChannels(
   BAST_Handle  h,             /* [in] BAST handle */
   uint32_t     *totalChannels /* [out] number of channels supported */
)
{
   BSTD_UNUSED(h);
   BDBG_ENTER(BAST_g2_P_GetTotalChannels);
   
   *totalChannels = BAST_G2_MAX_CHANNELS;
   
   BDBG_LEAVE(BAST_g2_P_GetTotalChannels);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_OpenChannel()
******************************************************************************/
BERR_Code BAST_g2_P_OpenChannel(
   BAST_Handle                 h,              /* [in] BAST handle */
   BAST_ChannelHandle         *pChannelHandle, /* [out] BAST channel handle */
   uint32_t                   chnNo,           /* [in] channel number */
   const BAST_ChannelSettings *pSettings       /* [in] channel settings */
)
{
   BERR_Code retCode;
   BAST_ChannelSettings cs;
   BAST_P_ChannelHandle *ch;
   BAST_g2_P_ChannelHandle *chG2;
   BAST_g2_P_Handle *pDev = h->pImpl;

   BDBG_ASSERT(chnNo < BAST_G2_MAX_CHANNELS);
   BDBG_ENTER(BAST_g2_P_OpenChannel);

   if (pSettings == NULL)
      BAST_g2_GetChannelDefaultSettings(h, chnNo, &cs);
   else
      cs = *pSettings;

   /* allocate memory for the channel handle */
   ch = (BAST_P_ChannelHandle *)BKNI_Malloc(sizeof(BAST_P_ChannelHandle));
   BDBG_ASSERT(ch);
   BKNI_Memset((void*)ch, 0, sizeof(BAST_P_ChannelHandle));
   BKNI_Memcpy((void*)(&ch->settings), (void*)&cs, sizeof(BAST_ChannelSettings)); 
   ch->channel = (uint8_t)chnNo;
   ch->pDevice = h;
   h->pChannels[chnNo] = ch;

   chG2 = (BAST_g2_P_ChannelHandle *)BKNI_Malloc(sizeof(BAST_g2_P_ChannelHandle));
   BDBG_ASSERT(chG2);
   BKNI_Memset((void*)chG2, 0, sizeof(BAST_g2_P_ChannelHandle));

   /* create callbacks for lock interrupts */
   retCode = BINT_CreateCallback(&(chG2->hQpskLockCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eLock], BAST_g2_P_QpskLock_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG2->hQpskNotLockCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eNotLock], BAST_g2_P_QpskNotLock_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   /* create callbacks for timer interrupts */
   retCode = BINT_CreateCallback(&(chG2->hBaudTimerCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eBaudTimer], BAST_g2_P_BaudTimer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG2->hBerTimerCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eBerTimer], BAST_g2_P_BerTimer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG2->hGen1TimerCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eGenTimer1], BAST_g2_P_Gen1Timer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG2->hGen2TimerCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eGenTimer2], BAST_g2_P_Gen2Timer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG2->hXtalTimerCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eXtalTimer], BAST_g2_P_XtalTimer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   /* create callback for MI2C interrupt */
   retCode = BINT_CreateCallback(&(chG2->hMi2cCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eMi2c], BAST_g2_P_Mi2c_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   retCode = BKNI_CreateEvent(&(chG2->hLockChangeEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG2->hMi2cEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG2->hStatusEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG2->hPeakScanEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   ch->pImpl = (void*)chG2;
   BAST_g2_P_InitChannelHandle(ch);
   
   if (!chG2->bExternalTuner)
   {
      /* create callbacks for agc interrupts */
      retCode = BINT_CreateCallback(&(chG2->h3StageAgc0Cb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eIfAgcLeThreshRise], BAST_g2_P_IfAgcLeThreshRise_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_CreateCallback(&(chG2->h3StageAgc1Cb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eIfAgcLeThreshFall], BAST_g2_P_IfAgcLeThreshFall_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_CreateCallback(&(chG2->h3StageAgc2Cb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eRfAgcLeThreshRise], BAST_g2_P_RfAgcLeThreshRise_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_CreateCallback(&(chG2->h3StageAgc3Cb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eRfAgcLeThreshFall], BAST_g2_P_RfAgcLeThreshFall_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_CreateCallback(&(chG2->h3StageAgc4Cb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eRfAgcGtMaxChange], BAST_g2_P_RfAgcGtMaxChange_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   else
   {
      chG2->h3StageAgc0Cb = NULL;
      chG2->h3StageAgc1Cb = NULL;
      chG2->h3StageAgc2Cb = NULL;
      chG2->h3StageAgc3Cb = NULL;
      chG2->h3StageAgc4Cb = NULL;
   }
   
   if (chG2->bHasDiseqc)
   {
      /* create diseqc done callback */
      retCode = BINT_CreateCallback(&(chG2->hDiseqcDoneCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eDiseqcDone], BAST_g2_P_DiseqcDone_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create diseqc event */
      retCode = BKNI_CreateEvent(&(chG2->hDiseqcEvent));
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create diseqc over voltage callback */
      retCode = BINT_CreateCallback(&(chG2->hDiseqcOverVoltageCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eDiseqcVoltageGtHiThresh], BAST_g2_P_DiseqcOverVoltage_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create over-voltage diseqc event */
      retCode = BKNI_CreateEvent(&(chG2->hDiseqcOverVoltageEvent));
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create diseqc under voltage callback */
      retCode = BINT_CreateCallback(&(chG2->hDiseqcUnderVoltageCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eDiseqcVoltageLtLoThresh], BAST_g2_P_DiseqcUnderVoltage_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create under-voltage diseqc event */
      retCode = BKNI_CreateEvent(&(chG2->hDiseqcUnderVoltageEvent));
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   else
   {
      chG2->hDiseqcDoneCb = NULL;
      chG2->hDiseqcEvent = NULL;
      chG2->hDiseqcOverVoltageCb = NULL;
      chG2->hDiseqcOverVoltageEvent = NULL;
      chG2->hDiseqcUnderVoltageCb = NULL;
      chG2->hDiseqcUnderVoltageEvent = NULL;
   }
      
   /* create HP callback */
   if (chG2->bHasAfec || chG2->bHasTfec)
   {
      retCode = BINT_CreateCallback(&(chG2->hHpCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eLdpcHp], BAST_g2_P_LdpcHp_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }    
   else
      chG2->hHpCb = NULL;      
      
   if (chG2->bHasAfec)
   {
      retCode = BINT_CreateCallback(&(chG2->hLdpcLockCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eAfecLock], BAST_g2_P_LdpcLock_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);   
      retCode = BINT_CreateCallback(&(chG2->hLdpcNotLockCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eAfecNotLock], BAST_g2_P_LdpcNotLock_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);   
   }
   else
   {
      chG2->hLdpcLockCb = NULL;  
      chG2->hLdpcNotLockCb = NULL;
   }
   
#ifndef BAST_EXCLUDE_TURBO
   if (chG2->bHasTfec)
   {
      retCode = BINT_CreateCallback(&(chG2->hTurboLockCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eTurboLock], BAST_g2_P_TurboLock_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);   
      retCode = BINT_CreateCallback(&(chG2->hTurboNotLockCb), pDev->hInterrupt, BAST_Sds_ChannelIntrID[chnNo][BAST_Sds_IntID_eTurboNotLock], BAST_g2_P_TurboNotLock_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS); 
   }
   else
   {
#endif
      chG2->hTurboLockCb = NULL;
      chG2->hTurboNotLockCb = NULL;
#ifndef BAST_EXCLUDE_TURBO
   }
#endif
   
   *pChannelHandle = ch;

   BDBG_LEAVE(BAST_g2_P_OpenChannel);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_CloseChannel()
******************************************************************************/
BERR_Code BAST_g2_P_CloseChannel(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BERR_Code retCode;
   BAST_g2_P_ChannelHandle *chG2;
   
   BDBG_ENTER(BAST_g2_P_CloseChannel);
   
   /* abort acquisition */
   BAST_CHK_RETCODE(BAST_g2_P_AbortAcq(h));
   BAST_g2_P_IndicateNotLocked(h);
   
   /* disable interrupts */
   BAST_CHK_RETCODE(BAST_g2_P_DisableChannelInterrupts(h, true, true));

   chG2 = (BAST_g2_P_ChannelHandle *)(h->pImpl);
   BKNI_DestroyEvent(chG2->hLockChangeEvent);
   BKNI_DestroyEvent(chG2->hMi2cEvent);
   BKNI_DestroyEvent(chG2->hPeakScanEvent);
   BKNI_DestroyEvent(chG2->hStatusEvent);
   
   BINT_DestroyCallback(chG2->hBaudTimerCb);
   BINT_DestroyCallback(chG2->hBerTimerCb);
   BINT_DestroyCallback(chG2->hGen1TimerCb);
   BINT_DestroyCallback(chG2->hGen2TimerCb);
   BINT_DestroyCallback(chG2->hXtalTimerCb);
   
   if (!chG2->bExternalTuner)
   {
      BINT_DestroyCallback(chG2->h3StageAgc0Cb);
      BINT_DestroyCallback(chG2->h3StageAgc1Cb);
      BINT_DestroyCallback(chG2->h3StageAgc2Cb);
      BINT_DestroyCallback(chG2->h3StageAgc3Cb);
      BINT_DestroyCallback(chG2->h3StageAgc4Cb);
   }
   
   BINT_DestroyCallback(chG2->hQpskLockCb);
   BINT_DestroyCallback(chG2->hQpskNotLockCb);
   BINT_DestroyCallback(chG2->hMi2cCb);
   
   if (chG2->hHpCb)
      BINT_DestroyCallback(chG2->hHpCb);
      
   if (chG2->bHasDiseqc)
   {
      BKNI_DestroyEvent(chG2->hDiseqcEvent);   
      BINT_DestroyCallback(chG2->hDiseqcDoneCb);
      BKNI_DestroyEvent(chG2->hDiseqcOverVoltageEvent);
      BINT_DestroyCallback(chG2->hDiseqcOverVoltageCb);
      BKNI_DestroyEvent(chG2->hDiseqcUnderVoltageEvent);
      BINT_DestroyCallback(chG2->hDiseqcUnderVoltageCb);

   }
   if (chG2->bHasAfec)
   {
      BINT_DestroyCallback(chG2->hLdpcLockCb);
      BINT_DestroyCallback(chG2->hLdpcNotLockCb);
   }
#ifndef BAST_EXCLUDE_TURBO
   if (chG2->bHasTfec)
   {
      BINT_DestroyCallback(chG2->hTurboLockCb);
      BINT_DestroyCallback(chG2->hTurboNotLockCb);
   }
#endif

   BKNI_Free((void*)chG2);
   BKNI_Free((void*)h);
   h = NULL;

   done:
   BDBG_LEAVE(BAST_g2_P_CloseChannel);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetDevice()
******************************************************************************/
BERR_Code BAST_g2_P_GetDevice(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Handle *pDev      /* [out] BAST handle */
)
{
   BDBG_ASSERT(h);
   BDBG_ENTER(BAST_g2_P_GetDevice);
   
   *pDev = h->pDevice;
   
   BDBG_LEAVE(BAST_g2_P_GetDevice);
   return BERR_SUCCESS; 
}


/******************************************************************************
 BAST_g2_P_InitAp() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_InitAp(
   BAST_Handle h,            /* [in] BAST handle */
   const uint8_t *pHexImage  /* [in] pointer to microcode image */
)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_ChannelHandle hChn;
   BERR_Code retCode = BERR_SUCCESS;
   int i;

   BSTD_UNUSED(pHexImage);
   BDBG_ENTER(BAST_g2_P_InitAp);
   
   /* disable interrupts */
   BAST_CHK_RETCODE(BAST_g2_P_DisableInterrupts(h));

   BKNI_WaitForEvent(hDev->hInitDoneEvent, 0);
   
   /* reinitialize channel handle state */
   hDev->numInternalTuners = 0;
   for (i = 0; i < h->totalChannels; i++)
   {
      hChn = h->pChannels[i];
      BAST_g2_P_InitChannelHandle(hChn);     
      if (((BAST_g2_P_ChannelHandle *)(hChn->pImpl))->bExternalTuner == false)
         hDev->numInternalTuners++;
   #ifndef BAST_EXCLUDE_POWERDOWN
      BAST_g2_P_PowerUp(hChn, BAST_CORE_ALL);
   #endif
   }
   /* BDBG_MSG(("numInternalTuners=%d", hDev->numInternalTuners)); */

   /* start initialization process */
   BKNI_EnterCriticalSection();
   BAST_g2_P_InitChannels(h->pChannels[0], true);
   BKNI_LeaveCriticalSection();

   /* wait for init done interrupt */
   if (BKNI_WaitForEvent(hDev->hInitDoneEvent, 1000) != BERR_SUCCESS)
   {
      BDBG_ERR(("BAST initialization timeout\n")); 
      BERR_TRACE(retCode = BAST_ERR_AP_NOT_INIT);            
   }

   done:
   BDBG_LEAVE(BAST_g2_P_InitAp);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_SoftReset()
******************************************************************************/
BERR_Code BAST_g2_P_SoftReset(
   BAST_Handle h           /* [in] BAST device handle */
)
{
   BERR_Code retCode = BERR_SUCCESS;
   int i;
   
   BDBG_ASSERT(h);
   BDBG_ENTER(BAST_g2_P_SoftReset);
   
   for (i = 0; i < h->totalChannels; i++)
   {
      retCode = BAST_g2_P_ResetChannel(h->pChannels[i], true);
      if (retCode != BERR_SUCCESS)
         goto done;
   }
   
   /* re-initialize tuners */
   BAST_CHK_RETCODE(BAST_g2_P_InitAp(h, NULL));
   
   done:
   BDBG_LEAVE(BAST_g2_P_SoftReset);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_ResetChannel() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_ResetChannel(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool bForceReset       /* [in] true=force reset, false=abort when busy */
)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   /* check if i2c busy */
   if (hChn->bMi2cInProgress)
   {
      if (bForceReset)
      {
         /* reset the mi2c */
         val = 0x80;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_IICTL2, &val);
         val = 0x00;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_IICTL2, &val);
      }
      else
      {
         BERR_TRACE(retCode = BAST_ERR_MI2C_BUSY);
         return retCode;
      }
   }
   
   /* check if diseqc busy */
   if (hChn->diseqcStatus.status == BAST_DiseqcSendStatus_eBusy)
   {
      if (bForceReset)
      {
         /* reset the diseqc core */
         BAST_g2_P_ResetDiseqc(h, 0);
      }
      else
      {
         BERR_TRACE(retCode = BAST_ERR_DISEQC_BUSY);
         return retCode;
      }
   }
   
   /* abort acquisition */
   BAST_CHK_RETCODE(BAST_g2_P_AbortAcq(h));
   BAST_g2_P_IndicateNotLocked(h);
   
   /* disable channel interrupts */
   retCode = BAST_g2_P_DisableChannelInterrupts(h, true, true);

   /* re-initialize single channel */
   BAST_g2_P_InitChannelHandle(h);
#ifndef BAST_EXCLUDE_POWERDOWN
   BAST_CHK_RETCODE(BAST_g2_P_PowerUp(h, BAST_CORE_ALL)); /* power up all cores */
#else
   BKNI_EnterCriticalSection();
   BAST_g2_P_InitChannels(h, false);
   BKNI_LeaveCriticalSection();
#endif
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetApStatus()
******************************************************************************/
BERR_Code BAST_g2_P_GetApStatus(
   BAST_Handle h,           /* [in] BAST device handle */
   BAST_ApStatus *pStatus   /* [out] AP status */
)
{
   BERR_Code retCode = BERR_NOT_SUPPORTED;
   BSTD_UNUSED(h);
   BSTD_UNUSED(pStatus);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetApVersion()
******************************************************************************/
BERR_Code BAST_g2_P_GetApVersion(
   BAST_Handle h,          /* [in] BAST handle */
   uint16_t    *pChipId,   /* [out] BAST chip ID */
   uint8_t     *pChipVer,  /* [out] chip revision number */
   uint32_t    *pBondId,   /* [out] chip bonding option */
   uint8_t     *pApVer,    /* [out] AP microcode version */
   uint8_t     *pCfgVer    /* [out] host configuration version */
)
{
   BSTD_UNUSED(h);
   BDBG_ENTER(BAST_g2_P_GetApVersion);
   
   *pChipId = BAST_G2_CHIP;
   *pChipVer = 0xA0 + ((BCHP_VER >> 12) & 0xF0) + (BCHP_VER & 0xF);
   *pBondId = 0;
   *pApVer = BAST_G2_BUILD_VERSION;
   *pCfgVer = 0xFF;  /* not applicable */

   BDBG_LEAVE(BAST_g2_P_GetApVersion);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_TuneAcquire() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_TuneAcquire(
   BAST_ChannelHandle h,            /* [in] BAST channel handle */
   const uint32_t     freq,         /* [in] RF tuner frequency in Hz */
   const BAST_AcqSettings *pParams  /* [in] acquisition parameters */
)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pDevice->pImpl);   
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)(h->pImpl);
   
   BDBG_ASSERT(pParams);
   BDBG_ENTER(BAST_g2_P_TuneAcquire);
   
   if (hChn->coresPoweredDown & BAST_CORE_SDS)
      return BAST_ERR_POWERED_DOWN;
   
   if ((pParams->symbolRate < 1000000UL) || (pParams->symbolRate > 45000000UL))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));

   if ((freq < 200000000UL) || (freq > 2200000000UL))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));

   if ((pParams->mode == BAST_Mode_eUnknown) ||
       ((pParams->mode > BAST_Mode_eDcii_4_5) && (pParams->mode < BAST_Mode_eLdpc_Qpsk_1_2)) ||
       (pParams->mode == 0x2F) || (pParams->mode > BAST_Mode_eBlindScan) ||
       ((pParams->mode > BAST_Mode_eTurbo_8psk_8_9) && (pParams->mode < BAST_Mode_eBlindScan)))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));
   
   if ((!hChn->bHasAfec) && (BAST_MODE_IS_LDPC(pParams->mode)))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));
   
   if ((!hChn->bHasTfec) && (BAST_MODE_IS_TURBO(pParams->mode)))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));

   /* disable channel interrupts */
   BAST_g2_P_DisableChannelInterrupts(h, false, false);
   
   if (hChn->bFsNotDefault)
      BAST_g2_P_SetDefaultSampleFreq(h);
   
   BAST_g2_P_KillTransport(h);
   
   BAST_g2_P_IndicateNotLocked(h);
      
   hChn->tunerFreq = freq;
   hChn->acqParams = *pParams;
   hChn->blindScanStatus = BAST_BlindScanStatus_eIdle;
   hChn->initFreqOffset = 0;
   hChn->firstTimeLock = 0;
   hChn->tuneMixStatus = 0;
   hChn->turboScanCurrMode = 0;
   hChn->dftFreqEstimateStatus = 0;   
   BAST_g2_P_ResetStatus(h);
   
   if ((hChn->tunerCtl & BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE) && (hDev->numInternalTuners != 2))
   {
      BDBG_WRN(("cannot enable splitter mode for %d internal tuners!", hDev->numInternalTuners));
      hChn->tunerCtl &= ~BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE;
   }
   
   /* set 3445 notch filter for internal tuners */
   if ((hDev->bBcm3445) && (!hChn->bExternalTuner))
   {
      retCode = BAST_g2_P_LnaSetNotchFilter(h, (freq < 1100000000UL ? true : false));
      if (retCode != BERR_SUCCESS)
         return retCode;
   }

   /* complete tuning and acquisition in ISR context */
   BKNI_EnterCriticalSection();
   retCode = BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 30, BAST_g2_P_TuneAcquire1);
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BAST_g2_P_TuneAcquire);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_TuneAcquire1() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_TuneAcquire1(BAST_ChannelHandle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)(h->pImpl);
   
#ifndef BAST_EXCLUDE_STATUS_EVENTS
   BAST_g2_P_ResetStatusIndicators(h);
#endif
   BAST_g2_P_ResetAcquisitionTimer(h);
   
   if (hChn->bExternalTuner)
   {
      retCode = BAST_g2_P_Acquire(h);
   }
   else
   {
      hChn->acqState = BAST_AcqState_eTuning;
      hChn->tunerIfStep = 0;
      
      /* check for bypass tune option */
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_BYPASS_TUNE)
         retCode = BAST_g2_P_Acquire(h);
      else
         retCode = BAST_g2_P_TunerSetFreq(h, BAST_g2_P_Acquire);
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetChannelStatus()
******************************************************************************/
BERR_Code BAST_g2_P_GetChannelStatus(
   BAST_ChannelHandle h,        /* [in] BAST handle */
   BAST_ChannelStatus *pStatus  /* [out] pointer to status structure */
)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   bool bDacLocked, bMixerLocked;
   uint32_t val, aif, agf, ndfctl;   
   
   BDBG_ENTER(BAST_g2_P_GetChannelStatus);
   
   if (hChn->coresPoweredDown & BAST_CORE_SDS)
      return BAST_ERR_POWERED_DOWN;

   pStatus->mode = BAST_g2_P_GetActualMode(h);

   if (hChn->bExternalTuner)
      pStatus->bTunerLocked = true;
   else
   {
      BAST_g2_P_TunerGetLockStatus(h, &bDacLocked, &bMixerLocked);
      pStatus->bTunerLocked = (bMixerLocked && bDacLocked);
   }

   pStatus->bDemodLocked = false;
   pStatus->bBertLocked = false;
   BAST_g2_P_GetLockStatus(h, &(pStatus->bDemodLocked));
   if (pStatus->bDemodLocked)
   {
      BAST_g2_P_ReadRegister(h,(uint32_t) BCHP_SDS_BEST, &val);
      if (val & 0x02000000)
         pStatus->bBertLocked = true;
   }

   BAST_g2_P_GetSnrEstimate(h, &(pStatus->snrEstimate));
   BAST_g2_P_ReadRegister(h, BCHP_SDS_AII, &(pStatus->IFagc));
   BAST_g2_P_ReadRegister(h, BCHP_SDS_AIT, &(pStatus->RFagc));
   
   BAST_g2_P_ReadRegister(h,(uint32_t) BCHP_SDS_AIF, (uint32_t *)&aif);
   BAST_g2_P_ReadRegister(h,(uint32_t) BCHP_SDS_AGF, (uint32_t *)&agf);
   BAST_g2_P_ReadRegister(h,(uint32_t) BCHP_SDS_NDFCTL, (uint32_t *)&ndfctl);       
   aif &= 0xFFFFFF00;
   ndfctl &= 0x000000C0;
   agf = (agf >> 16) & 0x0000000F;       
   pStatus->agf = aif | ndfctl | agf;

   BAST_g2_P_GetSampleFreq(h);
   pStatus->sample_clock = hChn->sampleFreq;
   pStatus->symbolRate = hChn->acqParams.symbolRate;
   pStatus->carrierOffset = hChn->acqParams.carrierFreqOffset;
   pStatus->reacqCount = hChn->acqCount;
   pStatus->tunerFreq = hChn->tunerFreq;
   pStatus->outputBitrate = hChn->outputBitRate;
   
   if (hChn->tunerCtl & BAST_G2_TUNER_CTL_SET_FILTER_MANUAL)
      pStatus->tunerFilter = hChn->tunerCutoff * 1000000;
   else
      pStatus->tunerFilter = hChn->tunerLpfToCalibrate * 1000000;
      
   pStatus->symbolRateError = BAST_g2_P_GetSymbolRateError(h);
   pStatus->carrierError = BAST_g2_P_GetCarrierError(h);
   if (hChn->acqState == BAST_AcqState_eMonitor)
   {
      if (hChn->acqParams.carrierFreqOffset)
         pStatus->carrierError -= hChn->acqParams.carrierFreqOffset;
   }

   if (BAST_MODE_IS_LEGACY_QPSK(pStatus->mode))
   {
      BAST_g2_P_QpskUpdateErrorCounters(h);
      pStatus->modeStatus.legacy.rsCorrCount = hChn->rsCorr;
      pStatus->modeStatus.legacy.rsUncorrCount = hChn->rsUncorr;
      pStatus->modeStatus.legacy.preVitErrCount = hChn->preVitErrors;

      BAST_g2_P_ReadRegister(h, BCHP_SDS_VST, &val);
      pStatus->modeStatus.legacy.spinv = (BAST_Spinv)((val >> 28) & 0x03);
      pStatus->modeStatus.legacy.phase = (BAST_PhaseRotation)((val >> 17) & 0x03);
   }
   else if (BAST_MODE_IS_LDPC(pStatus->mode))
   {
      BAST_g2_P_LdpcUpdateBlockCounters(h);
      pStatus->modeStatus.ldpc.corrBlocks = hChn->ldpcCorrBlocks;
      pStatus->modeStatus.ldpc.badBlocks = hChn->ldpcBadBlocks;
      pStatus->modeStatus.ldpc.totalBlocks = hChn->ldpcTotalBlocks;

      BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &val);
      pStatus->modeStatus.ldpc.hpstatus = val & 0x7FFFFFFF;
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_ON)
      {
         if (hChn->ldpcScanState & BAST_LDPC_SCAN_PILOT)
            pStatus->modeStatus.ldpc.hpstatus |= 0x80000000;         
      }
      else if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT)
         pStatus->modeStatus.ldpc.hpstatus |= 0x80000000;

      pStatus->modeStatus.ldpc.fec_clock = hChn->fecFreq;
   }
#ifndef BAST_EXCLUDE_TURBO
   else if (BAST_g2_P_IsTurboOn(h))
   {
      BAST_g2_P_TurboUpdateErrorCounters(h);
      BAST_g2_P_UpdateErrorCounters(h);
      
      pStatus->modeStatus.turbo.totalBlocks = hChn->turboTotalBlocks;
      pStatus->modeStatus.turbo.corrBlocks = hChn->turboCorrBlocks;
      pStatus->modeStatus.turbo.badBlocks = hChn->turboBadBlocks;
            
      BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &val);
      pStatus->modeStatus.turbo.hpstatus = val;

      pStatus->modeStatus.turbo.fec_clock = hChn->fecFreq;
   }
#endif
   else
      BAST_g2_P_UpdateErrorCounters(h);
   
   pStatus->mpegErrors = hChn->mpegErrorCount;
   pStatus->mpegCount = hChn->mpegFrameCount;
   pStatus->berErrors = hChn->berErrors;
   
   BDBG_LEAVE(BAST_g2_P_GetChannelStatus);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetLockStatus()
******************************************************************************/
BERR_Code BAST_g2_P_GetLockStatus(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   bool *bLocked          /* [out] lock indicator */
)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BDBG_ENTER(BAST_g2_P_GetLockStatus);

   *bLocked = hChn->bLocked;
   
   BDBG_LEAVE(BAST_g2_P_GetLockStatus);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_ResetStatus()
******************************************************************************/
BERR_Code BAST_g2_P_ResetStatus(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BDBG_ENTER(BAST_g2_P_ResetStatus);

   BAST_g2_P_ResetErrorCounters(h);
   hChn->acqCount = 0;
   
   BDBG_LEAVE(BAST_g2_P_ResetStatus);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_GetSoftDecisionBuf()
******************************************************************************/
BERR_Code BAST_g2_P_GetSoftDecisionBuf(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   int16_t *pI,           /* [out] 15 I-values */
   int16_t *pQ            /* [out] 15 Q-values */
)
{
   uint32_t val, i;
   
   BDBG_ENTER(BAST_g2_P_GetSoftDecisionBuf);

   for (i = 0; i < 15; i++)
   {
      BAST_g2_P_ReadRegister(h, BCHP_SDS_EQSFT, &val);
      pI[i] = (int16_t)((val >> 24) & 0xFF);
      if (pI[i] & 0x80)
         pI[i] -= 0x100;
      pQ[i] = (int16_t)((val >> 16) & 0xFF);
      if (pQ[i] & 0x80)
         pQ[i] -= 0x100;
   }
   
   BDBG_LEAVE(BAST_g2_P_GetSoftDecisionBuf);
   return BERR_SUCCESS;
}


/******************************************************************************
 BERR_Code BAST_g2_P_ReadAgc()
******************************************************************************/
BERR_Code BAST_g2_P_ReadAgc(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   uint32_t *pAgc         /* [out] integrator value */
)
{
   uint32_t reg = ((which_agc == BAST_Agc_eRF) ? BCHP_SDS_AIT : BCHP_SDS_AII);
   BDBG_ENTER(BAST_g2_P_ReadAgc);
   
   BAST_g2_P_ReadRegister(h, reg, pAgc);
   
   BDBG_LEAVE(BAST_g2_P_ReadAgc);
   return BERR_SUCCESS;
}


/******************************************************************************
 BERR_Code BAST_g2_P_WriteAgc()
******************************************************************************/
BERR_Code BAST_g2_P_WriteAgc(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   uint32_t *pAgc         /* [in] integrator value */
)
{
   uint32_t reg = ((which_agc == BAST_Agc_eRF) ? BCHP_SDS_AIT : BCHP_SDS_AII);
   BDBG_ENTER(BAST_g2_P_WriteAgc);
   
   BAST_g2_P_WriteRegister(h, reg, pAgc);
   
   BDBG_LEAVE(BAST_g2_P_WriteAgc);
   return BERR_SUCCESS;
}


/******************************************************************************
 BERR_Code BAST_g2_P_FreezeAgc()
******************************************************************************/
BERR_Code BAST_g2_P_FreezeAgc(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   bool bFreeze           /* [in] true = freeze, false = unfreeze */
)
{
   uint32_t reg = ((which_agc == BAST_Agc_eRF) ? BCHP_SDS_AGTCTL : BCHP_SDS_AGICTL);
   uint32_t val = (bFreeze ? 0x02 : 0);
   
   BDBG_ENTER(BAST_g2_P_FreezeAgc);
   
   BAST_g2_P_ReadModifyWriteRegister(h, reg, 0xFFFFFFFC, val);
   
   BDBG_LEAVE(BAST_g2_P_FreezeAgc);
   return BERR_SUCCESS;
}


/******************************************************************************
 BERR_Code BAST_g2_P_FreezeEq()
******************************************************************************/
BERR_Code BAST_g2_P_FreezeEq(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   bool bFreeze           /* [in] true = freeze, false = unfreeze */
)
{
   uint32_t val = (bFreeze ? 0x07 : 0x02);
   BDBG_ENTER(BAST_g2_P_FreezeEq);
   
   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_EQFFE2, 0xFFFFFFF8, val);
   
   BDBG_LEAVE(BAST_g2_P_FreezeEq);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_PowerDown()
******************************************************************************/
BERR_Code BAST_g2_P_PowerDown(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   uint32_t options       /* [in] see BAST_CORE_* macros */
)
{
#ifndef BAST_EXCLUDE_POWERDOWN
   BERR_Code retCode = BERR_SUCCESS;
   /* BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pDevice->pImpl); */
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)(h->pImpl);
   BAST_ChannelHandle hChn0 = (BAST_ChannelHandle)(h->pDevice->pChannels[0]);
   BAST_ChannelHandle hChn1 = (BAST_ChannelHandle)(h->pDevice->pChannels[1]);
   bool bMi2cUsedForBcm3445 = false;
   uint32_t mb, i;
   
   BDBG_ENTER(BAST_g2_P_PowerDown);
   
   /* scan to see if any other channels using this channel's master i2c */
   for (i = 0; i < h->pDevice->totalChannels; i++)
   {
      hChn = (BAST_g2_P_ChannelHandle *)(h->pDevice->pChannels[i]->pImpl);
      if (hChn->bcm3445Mi2cChannel == h->channel)
         bMi2cUsedForBcm3445 = true;
   }
   hChn = (BAST_g2_P_ChannelHandle *)(h->pImpl);
   BDBG_MSG(("bcm3445 on channel %d, master? %d", hChn->bcm3445Mi2cChannel, bMi2cUsedForBcm3445));
   
   if ((options & BAST_CORE_SDS) && ((hChn->coresPoweredDown & BAST_CORE_SDS) == 0))
   {
      /* power down tuner, sds, and fec in this channel */
      hChn->coresPoweredDown |= BAST_CORE_SDS;
      
      /* abort acquisition before powering down */
      BAST_g2_P_AbortAcq(h);
      BAST_g2_P_IndicateNotLocked(h);
      
      if (!(hChn->bExternalTuner))
      {
         /* tuner power down */
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL2, ~0x00000000, 0x01);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL4, ~0x00000000, 0x03);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL5, ~0x00000008, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL6, ~0x00000004, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL7, ~0x00000000, 0x0B);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL8, ~0x00000000, 0xC0);
         
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL10, ~0x00000000, 0x90);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL13, ~0x00000000, 0x40);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL14, ~0x00000000, 0x10);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL16, ~0x00000000, 0x20);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL17, ~0x00000001, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL18, ~0x00000040, 0x82);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL19, ~0x00000000, 0xC0);
         
         /* power down ch0 dac pll if all internal tuners powered down */
         if ((h->pDevice->totalChannels <= 2) ||
            ((((BAST_g2_P_ChannelHandle *)(hChn0->pImpl))->coresPoweredDown & BAST_CORE_SDS) && 
            (((BAST_g2_P_ChannelHandle *)(hChn1->pImpl))->coresPoweredDown & BAST_CORE_SDS)))
         {
            /* enable and power up sample pll output on channel0 for tuner0 register access */
            if (h->channel > 0)
            {
               BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_SPLL_MODE, ~0x04, 0x00);
               mb = 0x020;
               BAST_g2_P_WriteRegister(hChn0, BCHP_SDS_SPLL_PWRDN, &mb);
            }
            
            BDBG_MSG(("power down dacpll"));
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL9, ~0x00000000, 0x20);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL21, ~0x00000000, 0x80);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL23, ~0x00000000, 0x80);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL24, ~0x00000000, 0xC4);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL25, ~0x00000000, 0x20);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL27, ~0x00000000, 0x20);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL28, ~0x00000000, 0x20);
         }
      }
      
      /* sds power down */
      BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_AFECTL1, 0xFFFFFFFF, 0x01);
      
      /* power down sample pll if mi2c not controlling 3445 and if not used for diseqc */
      if ((!bMi2cUsedForBcm3445) && 
         ((!hChn->bHasDiseqc) || (options & BAST_CORE_DISEQC) || (hChn->coresPoweredDown & BAST_CORE_DISEQC)))
      {
         BDBG_MSG(("power down spll"));
      #if (BCHP_CHIP==7342)
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_PWRDN, 0xFFFFFFFF, 0x1C);
         
         /* disable ch0 sample pll output if all internal tuners powered down */
         if (((((BAST_g2_P_ChannelHandle *)(hChn0->pImpl))->coresPoweredDown & BAST_CORE_ALL) == BAST_CORE_ALL) && 
            ((((BAST_g2_P_ChannelHandle *)(hChn1->pImpl))->coresPoweredDown & BAST_CORE_ALL) == BAST_CORE_ALL))
         {
            mb = 0x11F;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_SPLL_PWRDN, &mb);
            
            BDBG_MSG(("disable spll0"));
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_SPLL_MODE, 0xFFFFFFFF, 0x04);
         }
      #else
         mb = 0x11F;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_SPLL_PWRDN, &mb);
         
         /* disable sample pll output */
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_MODE, 0xFFFFFFFF, 0x04);
      #endif
      }
      else
      {
         BDBG_MSG(("power down afec/tfec"));
         /* only power down afec and tfec if mi2c controlling 3445 */
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_PWRDN, 0xFFFFFFFF, 0x18);
      }
   }

   if (hChn->bHasDiseqc)
   {
      if ((options & BAST_CORE_DISEQC) && ((hChn->coresPoweredDown & BAST_CORE_DISEQC) == 0))
      {
         /* power down diseqc core in this channel */
         BDBG_MSG(("power down diseqc"));
         hChn->coresPoweredDown |= BAST_CORE_DISEQC;
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_DSCTL10, 0xFFFFFFFF, 0x80);
         
         /* power down sample pll if mi2c not controlling 3445 and sds powered down */
         if ((!bMi2cUsedForBcm3445) && (hChn->coresPoweredDown & BAST_CORE_SDS))
         {
            BDBG_MSG(("power down spll"));
         #if (BCHP_CHIP==7342)
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_PWRDN, 0xFFFFFFFF, 0x1C);
            
            /* disable ch0 sample pll output if all internal tuners powered down */
            if (((((BAST_g2_P_ChannelHandle *)(hChn0->pImpl))->coresPoweredDown & BAST_CORE_ALL) == BAST_CORE_ALL) && 
               ((((BAST_g2_P_ChannelHandle *)(hChn1->pImpl))->coresPoweredDown & BAST_CORE_ALL) == BAST_CORE_ALL))
            {
               mb = 0x11F;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_SPLL_PWRDN, &mb);
               
               BDBG_MSG(("disable spll0"));
               BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_SPLL_MODE, 0xFFFFFFFF, 0x04);
            }
         #else
            mb = 0x11F;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_SPLL_PWRDN, &mb);
            
            /* disable sample pll output */
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_MODE, 0xFFFFFFFF, 0x04);
         #endif
         }
      }
   }
   
   BDBG_LEAVE(BAST_g2_P_PowerDown);
   return retCode;
#else
   return BERR_NOT_SUPPORTED;
#endif
}


/******************************************************************************
 BAST_g2_P_PowerUp()
******************************************************************************/
BERR_Code BAST_g2_P_PowerUp(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   uint32_t options       /* [in] see BAST_CORE_* macros */
)
{
#ifndef BAST_EXCLUDE_POWERDOWN
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)(h->pImpl);
   BAST_ChannelHandle hChn0 = (BAST_ChannelHandle)(h->pDevice->pChannels[0]);
   BAST_ChannelHandle hChn1 = (BAST_ChannelHandle)(h->pDevice->pChannels[1]);
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pDevice->pImpl);
   uint32_t mb;
   
   BDBG_ENTER(BAST_g2_P_PowerUp);
   
   if ((options & BAST_CORE_SDS) && (hChn->coresPoweredDown & BAST_CORE_SDS))
   {
      /* power up tuner, sds, and fec in this channel */
      hChn->coresPoweredDown &= ~BAST_CORE_SDS;
      
      /* sds power up */
      BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_AFECTL1, ~0x01, 0x00);
      
   #if (BCHP_CHIP==7342)
      if (!(hChn->bExternalTuner))
      {
         /* power up sample pll for first tuner powered up */
         if ((((BAST_g2_P_ChannelHandle *)(hChn0->pImpl))->coresPoweredDown & BAST_CORE_SDS) ^ 
            (((BAST_g2_P_ChannelHandle *)(hChn1->pImpl))->coresPoweredDown & BAST_CORE_SDS))
         {
            BDBG_MSG(("enable spll0"));
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_SPLL_PWRDN, ~0x11F, 0x20);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_SPLL_MODE, ~0x04, 0x00);
         }
      }
      else
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_MODE, ~0x04, 0x00);
   #else
      /* enable sample pll output */
      BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_MODE, ~0x04, 0x00);
   #endif
      
      BDBG_MSG(("power up spll"));
      mb = 0x020;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_SPLL_PWRDN, &mb);
      
      if (!(hChn->bExternalTuner))
      {
         /* power up dac pll for first tuner powered up */
         if ((h->pDevice->totalChannels <= 2) ||
            ((((BAST_g2_P_ChannelHandle *)(hChn0->pImpl))->coresPoweredDown & BAST_CORE_SDS) ^ 
            (((BAST_g2_P_ChannelHandle *)(hChn1->pImpl))->coresPoweredDown & BAST_CORE_SDS)))
         {
            /* enable and power up sample pll output on channel0 for tuner0 register access */
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_SPLL_MODE, ~0x04, 0x00);
            mb = 0x020;
            BAST_g2_P_WriteRegister(hChn0, BCHP_SDS_SPLL_PWRDN, &mb);
            
            BDBG_MSG(("power up dacpll"));
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL9, ~0x00000020, 0);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL21, ~0x00000080, 0);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL23, ~0x00000080, 0);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL24, ~0x000000C4, 0);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL25, ~0x00000020, 0);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL27, ~0x00000020, 0);
            BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_TUNER_ANACTL28, ~0x00000020, 0);
            BAST_g2_P_ReadRegister(hChn0, BCHP_TUNER_ANACTL9, &mb);
         }
         
         /* tuner power up  */
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL2, ~0x01, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL4, ~0x03, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL5, 0xFFFFFFFF, 0x08);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL6, 0xFFFFFFFF, 0x04);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL7, ~0x0B, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL8, ~0xC0, 0x00);
         
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL10, ~0x90, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL13, ~0x40, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL14, ~0x10, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL16, ~0x20, 0x00);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL17, 0xFFFFFFFF, 0x01);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL18, ~0x82, 0x40);
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL19, ~0xC0, 0x00);
      }
      
      /* re-initialize single channel */
      BKNI_WaitForEvent(hDev->hInitDoneEvent, 0);
      BKNI_EnterCriticalSection();
      BAST_g2_P_InitChannels(h, false);
      BKNI_LeaveCriticalSection();
      
      /* wait for init done interrupt */
      if (BKNI_WaitForEvent(hDev->hInitDoneEvent, 1000) != BERR_SUCCESS)
      {
         BDBG_ERR(("BAST_PowerUp initialization timeout")); 
         BERR_TRACE(retCode = BAST_ERR_AP_NOT_INIT);            
      }
   }
   
   if (hChn->bHasDiseqc)
   {
      if ((options & BAST_CORE_DISEQC) && (hChn->coresPoweredDown & BAST_CORE_DISEQC))
      {
         /* power up diseqc core in this channel */
         BDBG_MSG(("power up diseqc"));
         hChn->coresPoweredDown &= ~BAST_CORE_DISEQC;
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_DSCTL10, ~0x80, 0x00);
         
         /* reset diseqc clock */
         BAST_g2_P_SetDiseqcClock(h);
         
      #if (BCHP_CHIP==7342)
         if (!(hChn->bExternalTuner))
         {
            /* power up sample pll for first diseqc powered up */
            if ((((BAST_g2_P_ChannelHandle *)(hChn0->pImpl))->coresPoweredDown & BAST_CORE_DISEQC) ^ 
               (((BAST_g2_P_ChannelHandle *)(hChn1->pImpl))->coresPoweredDown & BAST_CORE_DISEQC))
            {
               BDBG_MSG(("enable spll0"));
               BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_SPLL_PWRDN, ~0x11F, 0x20);
               BAST_g2_P_ReadModifyWriteRegister(hChn0, BCHP_SDS_SPLL_MODE, ~0x04, 0x00);
            }
         }
         else
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_MODE, ~0x04, 0x00);
      #else
         /* enable sample pll output */
         BDBG_MSG(("enable spll"));
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_MODE, ~0x04, 0x00);
      #endif
         
         /* power up sample pll needed by diseqc */
         BDBG_MSG(("power up spll"));
         BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_PWRDN, ~0x107, 0x020);
      }
   }
   
   BDBG_LEAVE(BAST_g2_P_PowerUp);
   return retCode;
#else
   return BERR_NOT_SUPPORTED;
#endif
}


/******************************************************************************
 BAST_g2_P_ReadConfig()
******************************************************************************/
BERR_Code BAST_g2_P_ReadConfig(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   uint16_t id,            /* [in] configuration parameter ID */
   uint8_t *p,                /* [in] buffer to hold the read data */
   uint8_t len             /* [in] number of bytes to read */
)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pDevice->pImpl);
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   int i;

   if ((p == NULL) ||
       !((len == 1) || (len == 2) || (len == 4) || (len == 16)))
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      goto done;
   }

   switch (id)
   {
      case BAST_G2_CONFIG_ALT_PLC_ACQ_BW:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_ALT_PLC_ACQ_BW);
         p[0] = (uint8_t)((uint32_t)hChn->altPlcAcqBw >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->altPlcAcqBw >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->altPlcAcqBw >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->altPlcAcqBw & 0xFF);           
         break;
         
      case BAST_G2_CONFIG_ALT_PLC_ACQ_DAMP:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_ALT_PLC_ACQ_DAMP);
         *p = hChn->altPlcAcqDamp;         
         break;
              
      case BAST_G2_CONFIG_ALT_PLC_TRK_BW:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_ALT_PLC_TRK_BW);
         p[0] = (uint8_t)((uint32_t)hChn->altPlcTrkBw >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->altPlcTrkBw >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->altPlcTrkBw >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->altPlcTrkBw & 0xFF);           
         break;
         
      case BAST_G2_CONFIG_ALT_PLC_TRK_DAMP:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_ALT_PLC_TRK_DAMP);
         *p = hChn->altPlcTrkDamp;         
         break;
         
      case BAST_G2_CONFIG_DFT_FREQ_ESTIMATE:
         p[0] = (uint8_t)((uint32_t)hChn->dftFreqEstimate >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->dftFreqEstimate >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->dftFreqEstimate >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->dftFreqEstimate & 0xFF);         
         break;
        
      case BAST_G2_CONFIG_DFT_FREQ_ESTIMATE_STATUS:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DFT_FREQ_ESTIMATE_STATUS); 
         *p = hChn->dftFreqEstimateStatus;
         break;
         
      case BAST_G2_CONFIG_CNR_THRESH1:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_CNR_THRESH1);         
         p[0] = (uint8_t)(hChn->currentCnrThreshold1 >> 8);
         p[1] = (uint8_t)(hChn->currentCnrThreshold1 & 0xFF);
         break;
         
      case BAST_G2_CONFIG_STUFF_BYTES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_STUFF_BYTES);   
         *p = hChn->stuffBytes;   
         break;
      
      case BAST_G2_CONFIG_TURBO_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_TURBO_CTL);
         *p = hChn->turboCtl;
         break;
         
      case BAST_G2_CONFIG_DFT_RANGE_START:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DFT_RANGE_START);
         p[0] = (uint8_t)(hDev->dftRangeStart >> 8);
         p[1] = (uint8_t)(hDev->dftRangeStart & 0xFF);
         break;
         
      case BAST_G2_CONFIG_DFT_RANGE_END:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DFT_RANGE_END);
         p[0] = (uint8_t)(hDev->dftRangeEnd >> 8);
         p[1] = (uint8_t)(hDev->dftRangeEnd & 0xFF);
         break;
         
      case BAST_G2_CONFIG_DFT_SIZE:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DFT_SIZE);
         *p = hDev->dftSize;
         break;
         
      case BAST_G2_CONFIG_ACQ_TIME:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_ACQ_TIME);
         p[0] = (uint8_t)(hChn->acqTime >> 24);
         p[1] = (uint8_t)(hChn->acqTime >> 16);
         p[2] = (uint8_t)(hChn->acqTime >> 8);
         p[3] = (uint8_t)(hChn->acqTime & 0xFF);
         break;
         
      case BAST_G2_CONFIG_XTAL_FREQ:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_XTAL_FREQ);
         p[0] = (uint8_t)(hDev->xtalFreq >> 24);
         p[1] = (uint8_t)(hDev->xtalFreq >> 16);
         p[2] = (uint8_t)(hDev->xtalFreq >> 8);
         p[3] = (uint8_t)(hDev->xtalFreq & 0xFF);
         break;
      
      case BAST_G2_CONFIG_VCO_SEPARATION:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_VCO_SEPARATION);
         p[0] = (uint8_t)(hDev->vcoSeparation >> 24);
         p[1] = (uint8_t)(hDev->vcoSeparation >> 16);
         p[2] = (uint8_t)(hDev->vcoSeparation >> 8);
         p[3] = (uint8_t)(hDev->vcoSeparation & 0xFF);
         break;
         
      case BAST_G2_CONFIG_BCM3445_ADDRESS:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_BCM3445_ADDRESS);
         *p = hDev->bcm3445Address;
         break;
      
      case BAST_G2_CONFIG_TUNER_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_TUNER_CTL);
         *p = hChn->tunerCtl;
         break;
      
      case BAST_G2_CONFIG_TUNER_CUTOFF:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_TUNER_CUTOFF);
         *p = hChn->tunerCutoff;
         break;
      
      case BAST_G2_CONFIG_XPORT_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_XPORT_CTL);
         p[0] = (uint8_t)(hChn->xportCtl >> 8);
         p[1] = (uint8_t)(hChn->xportCtl & 0xFF);
         break;
      
      case BAST_G2_CONFIG_LDPC_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_LDPC_CTL);
         *p = hChn->ldpcCtl;
         break;
      
      case BAST_G2_CONFIG_DISEQC_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DISEQC_CTL);
         p[0] = (uint8_t)(hChn->diseqcCtl >> 8);
         p[1] = (uint8_t)(hChn->diseqcCtl & 0xFF);
         break;

      case BAST_G2_CONFIG_RRTO_USEC:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_RRTO_USEC);
         p[0] = (uint8_t)(hChn->diseqcRRTOuSec >> 24);
         p[1] = (uint8_t)(hChn->diseqcRRTOuSec >> 16);
         p[2] = (uint8_t)(hChn->diseqcRRTOuSec >> 8);
         p[3] = (uint8_t)(hChn->diseqcRRTOuSec & 0xFF);
         break;

      case BAST_G2_CONFIG_DISEQC_PRETX_DELAY:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DISEQC_PRETX_DELAY);
         *p = hChn->diseqcPreTxDelay;
         break;
      
      case BAST_G2_CONFIG_SEARCH_RANGE:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_SEARCH_RANGE);
         p[0] = (uint8_t)(hDev->searchRange >> 24);
         p[1] = (uint8_t)(hDev->searchRange >> 16);
         p[2] = (uint8_t)(hDev->searchRange >> 8);
         p[3] = (uint8_t)(hDev->searchRange & 0xFF);
         break;
         
      case BAST_G2_CONFIG_REACQ_DELAY:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_REACQ_DELAY);
         p[0] = (uint8_t)(hDev->reacqDelay >> 24);
         p[1] = (uint8_t)(hDev->reacqDelay >> 16);
         p[2] = (uint8_t)(hDev->reacqDelay >> 8);
         p[3] = (uint8_t)(hDev->reacqDelay & 0xFF);
         break;
         
      case BAST_G2_CONFIG_LDPC_SCRAMBLING_SEQ:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_LDPC_SCRAMBLING_SEQ);
         for (i = 0; i < 16; i++)
            p[i] = hChn->ldpcScramblingSeq[i];
         break;

      case BAST_G2_CONFIG_BCM3445_MI2C_CHANNEL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_BCM3445_MI2C_CHANNEL);
         *p = hDev->bcm3445Mi2cChannel;
         break;

      case BAST_G2_CONFIG_EXT_TUNER_IF_OFFSET:
         BDBG_ASSERT(len = BAST_G2_CONFIG_LEN_EXT_TUNER_IF_OFFSET);
         p[0] = (uint8_t)(hChn->extTunerIfOffset >> 24);
         p[1] = (uint8_t)(hChn->extTunerIfOffset >> 16);
         p[2] = (uint8_t)(hChn->extTunerIfOffset >> 8);
         p[3] = (uint8_t)(hChn->extTunerIfOffset & 0xFF);
         break;
         
      case BAST_G2_CONFIG_BLIND_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_BLIND_SCAN_MODES);
         *p = hChn->blindScanModes;
         break;

      case BAST_G2_CONFIG_VSENSE_THRES_HI:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_VSENSE_THRES_HI);
         *p = hChn->diseqcVsenseThreshHi;
         break;

      case BAST_G2_CONFIG_VSENSE_THRES_LO:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_VSENSE_THRES_LO);
         *p = hChn->diseqcVsenseThreshLo;
         break;

      case BAST_G2_CONFIG_DTV_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DTV_SCAN_CODE_RATES);     
         *p = hChn->dtvScanCodeRates;
         break;
         
      case BAST_G2_CONFIG_DVB_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DVB_SCAN_CODE_RATES);     
         *p = hChn->dvbScanCodeRates;
         break;
         
      case BAST_G2_CONFIG_TURBO_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_TURBO_SCAN_MODES);     
         p[0] = (uint8_t)(hChn->turboScanModes >> 8);
         p[1] = (uint8_t)(hChn->turboScanModes & 0xFF);
         break;
                  
      case BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MIN:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MIN);  
         p[0] = (uint8_t)(hChn->peakScanSymRateMin >> 24);
         p[1] = (uint8_t)(hChn->peakScanSymRateMin >> 16);
         p[2] = (uint8_t)(hChn->peakScanSymRateMin >> 8);
         p[3] = (uint8_t)(hChn->peakScanSymRateMin & 0xFF);
         break;
                  
      case BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MAX:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MAX);  
         p[0] = (uint8_t)(hChn->peakScanSymRateMax >> 24);
         p[1] = (uint8_t)(hChn->peakScanSymRateMax >> 16);
         p[2] = (uint8_t)(hChn->peakScanSymRateMax >> 8);
         p[3] = (uint8_t)(hChn->peakScanSymRateMax & 0xFF);
         break;
         
#ifndef BAST_EXCLUDE_STATUS_EVENTS   
      case BAST_G2_CONFIG_RT_STATUS_INDICATORS:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_RT_STATUS_INDICATORS);
         *p = hChn->statusEventIndicators;
         break;
        
      case BAST_G2_CONFIG_FREQ_DRIFT_THRESHOLD:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_FREQ_DRIFT_THRESHOLD);
         p[0] = (uint8_t)(hChn->freqDriftThreshold >> 24);
         p[1] = (uint8_t)(hChn->freqDriftThreshold >> 16);
         p[2] = (uint8_t)(hChn->freqDriftThreshold >> 8);
         p[3] = (uint8_t)(hChn->freqDriftThreshold & 0xFF);
         break;
         
      case BAST_G2_CONFIG_RAIN_FADE_THRESHOLD:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_RAIN_FADE_THRESHOLD); 
         p[0] = (uint8_t)(hChn->rainFadeThreshold >> 24);
         p[1] = (uint8_t)(hChn->rainFadeThreshold >> 16);
         p[2] = (uint8_t)(hChn->rainFadeThreshold >> 8);
         p[3] = (uint8_t)(hChn->rainFadeThreshold & 0xFF);
         break;
         
      case BAST_G2_CONFIG_RAIN_FADE_WINDOW:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_RAIN_FADE_WINDOW);
         p[0] = (uint8_t)(hChn->rainFadeWindow >> 24);
         p[1] = (uint8_t)(hChn->rainFadeWindow >> 16);
         p[2] = (uint8_t)(hChn->rainFadeWindow >> 8);
         p[3] = (uint8_t)(hChn->rainFadeWindow & 0xFF);
         break;         
#endif         

      case BAST_G2_CONFIG_FTM_TX_POWER:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_FTM_TX_POWER);
         *p = hDev->ftmTxPower;
         break;
      
      case BAST_G2_CONFIG_FTM_CH_SELECT:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_FTM_CH_SELECT);
         *p = hDev->ftmChannelSelect;
         break;
      
      case BAST_G2_CONFIG_DISEQC_TONE_THRESHOLD:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DISEQC_TONE_THRESHOLD);
         *p = hChn->diseqcToneThreshold;
         break;
      
      case BAST_G2_CONFIG_DISEQC_BIT_THRESHOLD:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DISEQC_BIT_THRESHOLD);
         p[0] = (uint8_t)(hChn->diseqcBitThreshold >> 8);
         p[1] = (uint8_t)(hChn->diseqcBitThreshold & 0xFF);
         break;
      
      case BAST_G2_CONFIG_BCM3445_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_BCM3445_CTL);
         *p = hChn->bcm3445Ctl;
         break;
      
      case BAST_G2_CONFIG_EXT_TUNER_RF_AGC_TOP:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_EXT_TUNER_RF_AGC_TOP);
         p[0] = (uint8_t)(hChn->extTunerRfAgcTop >> 8);
         p[1] = (uint8_t)(hChn->extTunerRfAgcTop & 0xFF);
         break;
      
      case BAST_G2_CONFIG_EXT_TUNER_IF_AGC_TOP:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_EXT_TUNER_IF_AGC_TOP);
         p[0] = (uint8_t)(hChn->extTunerIfAgcTop >> 8);
         p[1] = (uint8_t)(hChn->extTunerIfAgcTop & 0xFF);
         break;
         
      case BAST_G2_CONFIG_3STAGE_AGC_TOP:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_3STAGE_AGC_TOP);
         p[0] = (uint8_t)(hChn->intTuner3StageAgcTop >> 8);
         p[1] = (uint8_t)(hChn->intTuner3StageAgcTop & 0xFF);
         break;
      
      case BAST_G2_CONFIG_NETWORK_SPEC:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_NETWORK_SPEC);
         *p = (uint8_t)(h->pDevice->settings.networkSpec);
         break;
      
      case BAST_G2_CONFIG_REACQ_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_REACQ_CTL); 
         *p = hChn->reacqCtl;         
         break;
         
      default:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_P_WriteConfig()
******************************************************************************/
BERR_Code BAST_g2_P_WriteConfig(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   uint16_t id,            /* [in] configuration parameter ID */
   uint8_t *p,             /* [in] data to write */
   uint8_t len             /* [in] number of bytes to write */
)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pDevice->pImpl);
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   int i;

   if ((p == NULL) ||
       !((len == 1) || (len == 2) || (len == 4) || (len == 16)))
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      goto done;
   }

   switch (id)
   {
      case BAST_G2_CONFIG_REACQ_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_REACQ_CTL); 
         hChn->reacqCtl = *p;      
         break;
         
      case BAST_G2_CONFIG_ALT_PLC_ACQ_BW:
         BDBG_ASSERT(len = BAST_G2_CONFIG_LEN_ALT_PLC_ACQ_BW);
         hChn->altPlcAcqBw = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
         
      case BAST_G2_CONFIG_ALT_PLC_ACQ_DAMP:
         BDBG_ASSERT(len = BAST_G2_CONFIG_LEN_ALT_PLC_ACQ_DAMP);
         hChn->altPlcAcqDamp = *p;
         break;
         
      case BAST_G2_CONFIG_ALT_PLC_TRK_BW:
         BDBG_ASSERT(len = BAST_G2_CONFIG_LEN_ALT_PLC_TRK_BW);
         hChn->altPlcTrkBw = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
         
      case BAST_G2_CONFIG_ALT_PLC_TRK_DAMP:
         BDBG_ASSERT(len = BAST_G2_CONFIG_LEN_ALT_PLC_TRK_DAMP);
         hChn->altPlcTrkDamp = *p;
         break;
                  
      case BAST_G2_CONFIG_DFT_FREQ_ESTIMATE:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
        
      case BAST_G2_CONFIG_DFT_FREQ_ESTIMATE_STATUS:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DFT_FREQ_ESTIMATE_STATUS); 
         hChn->dftFreqEstimateStatus = *p;
         break;
         
      case BAST_G2_CONFIG_CNR_THRESH1:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
   
      case BAST_G2_CONFIG_STUFF_BYTES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_STUFF_BYTES);   
         hChn->stuffBytes = *p & 0x1F;
         break;
      
       case BAST_G2_CONFIG_TURBO_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_TURBO_CTL);
         hChn->turboCtl = *p;
         break;
         
      case BAST_G2_CONFIG_DFT_RANGE_START:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DFT_RANGE_START);
         hDev->dftRangeStart = (p[0] << 8) | p[1]; 
         break;
            
      case BAST_G2_CONFIG_DFT_RANGE_END:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DFT_RANGE_END);
         hDev->dftRangeEnd = (p[0] << 8) | p[1]; 
         break;
            
      case BAST_G2_CONFIG_DFT_SIZE:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DFT_SIZE);
         hDev->dftSize = *p;
         break;
                  
      case BAST_G2_CONFIG_BCM3445_MI2C_CHANNEL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_BCM3445_MI2C_CHANNEL);
         if (*p >= BAST_G2_MAX_CHANNELS)
            hDev->bcm3445Mi2cChannel = BAST_G2_MAX_CHANNELS - 1;
         else
            hDev->bcm3445Mi2cChannel = *p;
         break;

      case BAST_G2_CONFIG_XTAL_FREQ:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_XTAL_FREQ);
         hDev->xtalFreq = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
         
      case BAST_G2_CONFIG_VCO_SEPARATION:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_VCO_SEPARATION);
         hDev->vcoSeparation = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;

      case BAST_G2_CONFIG_BCM3445_ADDRESS:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_BCM3445_ADDRESS);
         hDev->bcm3445Address = *p;
         break;

      case BAST_G2_CONFIG_TUNER_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_TUNER_CTL);
         hChn->tunerCtl = *p;
         break;

      case BAST_G2_CONFIG_TUNER_CUTOFF:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_TUNER_CUTOFF);
         hChn->tunerCutoff = *p;
         break;

      case BAST_G2_CONFIG_XPORT_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_XPORT_CTL);
         hChn->xportCtl = (p[0] << 8) | p[1];    
         break;

      case BAST_G2_CONFIG_LDPC_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_LDPC_CTL);
         hChn->ldpcCtl = *p;
         break;

      case BAST_G2_CONFIG_DISEQC_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DISEQC_CTL);
         hChn->diseqcCtl = ((p[0] << 8) | p[1]) & BAST_G2_DISEQC_CTL_IMPLEMENTED;
         break;

      case BAST_G2_CONFIG_RRTO_USEC:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_RRTO_USEC);
         hChn->diseqcRRTOuSec = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         hChn->diseqcRRTOuSec &= 0xFFFFF;
         break;

      case BAST_G2_CONFIG_DISEQC_PRETX_DELAY:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DISEQC_PRETX_DELAY);
         hChn->diseqcPreTxDelay = *p;
         break;

      case BAST_G2_CONFIG_SEARCH_RANGE:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_SEARCH_RANGE);
         hDev->searchRange = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;

      case BAST_G2_CONFIG_REACQ_DELAY:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_REACQ_DELAY);
         hDev->reacqDelay = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
     
      case BAST_G2_CONFIG_LDPC_SCRAMBLING_SEQ:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_LDPC_SCRAMBLING_SEQ);
         for (i = 0; i < 16; i++)
            hChn->ldpcScramblingSeq[i] = p[i];
         break;

      case BAST_G2_CONFIG_EXT_TUNER_IF_OFFSET:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_EXT_TUNER_IF_OFFSET);
         hChn->extTunerIfOffset = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
         
      case BAST_G2_CONFIG_XPORT_CLOCK_ADJUST:
         /* TBD... */
         break;

      case BAST_G2_CONFIG_BLIND_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_BLIND_SCAN_MODES);
         hChn->blindScanModes = *p;
         break;

      case BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MIN:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MIN);      
         hChn->peakScanSymRateMin = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;

      case BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MAX:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MAX);  
         hChn->peakScanSymRateMax = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;

      case BAST_G2_CONFIG_VSENSE_THRES_HI:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_VSENSE_THRES_HI);
         hChn->diseqcVsenseThreshHi = *p;
         break;

      case BAST_G2_CONFIG_VSENSE_THRES_LO:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_VSENSE_THRES_LO);
         hChn->diseqcVsenseThreshLo = *p;
         break;

      case BAST_G2_CONFIG_DTV_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DTV_SCAN_CODE_RATES);
         hChn->dtvScanCodeRates = *p;
         break;
         
      case BAST_G2_CONFIG_DVB_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DVB_SCAN_CODE_RATES);
         hChn->dvbScanCodeRates = *p;
         break;
         
      case BAST_G2_CONFIG_TURBO_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_TURBO_SCAN_MODES);
         hChn->turboScanModes = (p[0] << 8) | p[1];
         break;
            
      case BAST_G2_CONFIG_ACQ_TIME:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_ACQ_TIME);
         /* hChn->acqTime = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]; */
         break;
         
#ifndef BAST_EXCLUDE_STATUS_EVENTS   
      case BAST_G2_CONFIG_RT_STATUS_INDICATORS:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_RT_STATUS_INDICATORS);
         hChn->statusEventIndicators = *p;
         break;
        
      case BAST_G2_CONFIG_FREQ_DRIFT_THRESHOLD:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_FREQ_DRIFT_THRESHOLD);
         hChn->freqDriftThreshold = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
         
      case BAST_G2_CONFIG_RAIN_FADE_THRESHOLD:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_RAIN_FADE_THRESHOLD);
         hChn->rainFadeThreshold = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
         
      case BAST_G2_CONFIG_RAIN_FADE_WINDOW:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_RAIN_FADE_WINDOW);   
         hChn->rainFadeWindow = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
#endif

      case BAST_G2_CONFIG_FTM_TX_POWER:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_FTM_TX_POWER);
         hDev->ftmTxPower = *p;
         break;
      
      case BAST_G2_CONFIG_FTM_CH_SELECT:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_FTM_CH_SELECT);
         if (*p > BAST_FskChannelConfig_eCh1Tx_Ch1Rx)
            hDev->ftmChannelSelect = BAST_FskChannelConfig_eCh0Tx_Ch0Rx;
         else
            hDev->ftmChannelSelect = *p;
         break;
      
      case BAST_G2_CONFIG_DISEQC_TONE_THRESHOLD:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DISEQC_TONE_THRESHOLD);
         hChn->diseqcToneThreshold = *p;
         break;
      
      case BAST_G2_CONFIG_DISEQC_BIT_THRESHOLD:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_DISEQC_BIT_THRESHOLD);
         hChn->diseqcBitThreshold = (p[0] << 8) | p[1];
         hChn->diseqcBitThreshold &= 0xFFF;
         break;
      
      case BAST_G2_CONFIG_BCM3445_CTL:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_BCM3445_CTL);
         hChn->bcm3445Ctl = *p & 0x7;
         break;
      
      case BAST_G2_CONFIG_EXT_TUNER_RF_AGC_TOP:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_EXT_TUNER_RF_AGC_TOP);
         hChn->extTunerRfAgcTop = (p[0] << 8) | p[1];
         break;
      
      case BAST_G2_CONFIG_EXT_TUNER_IF_AGC_TOP:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_EXT_TUNER_IF_AGC_TOP);
         hChn->extTunerIfAgcTop = (p[0] << 8) | p[1];
         break;
      
      case BAST_G2_CONFIG_3STAGE_AGC_TOP:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_3STAGE_AGC_TOP);
         hChn->intTuner3StageAgcTop = (p[0] << 8) | p[1];
         break;
      
      case BAST_G2_CONFIG_NETWORK_SPEC:
         BDBG_ASSERT(len == BAST_G2_CONFIG_LEN_NETWORK_SPEC);
         if (*p <= BAST_NetworkSpec_eEuro)
            h->pDevice->settings.networkSpec = *p;
         break;
      
      default:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_P_DisableInterrupts() - Non-ISR context
******************************************************************************/ 
BERR_Code BAST_g2_P_DisableInterrupts(
   BAST_Handle h   /* [in] BAST handle */
)
{
   int i;
   BAST_ChannelHandle hChn;
   BDBG_ASSERT(h);
   
   BAST_g2_Ftm_P_EnableFtmInterrupts(h, false);
   for (i = 0; i < h->totalChannels; i++)
   {
      hChn = h->pChannels[i];
      if (hChn != NULL)
         BAST_g2_P_DisableChannelInterrupts(hChn, true, true);
   }
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_DisableChannelInterrupts_isr() - ISR context
******************************************************************************/ 
BERR_Code BAST_g2_P_DisableChannelInterrupts_isr(
   BAST_ChannelHandle h,   /* [in] BAST handle */
   bool bDisableDiseqc,    /* [in] true = disable diseqc interrupts also */
   bool bDisableMi2c       /* [in] true = disable mi2c interrupts also */
)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eBaud);
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eBer);
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eGen1);
   /* BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eXtal); */
   
   BAST_g2_P_Enable3StageAgc(h, false);
   
   BINT_DisableCallback_isr(hChn->hQpskLockCb);
   BINT_DisableCallback_isr(hChn->hQpskNotLockCb);
   BINT_ClearCallback_isr(hChn->hQpskLockCb);
   BINT_ClearCallback_isr(hChn->hQpskNotLockCb);
   
   if (hChn->bHasAfec || hChn->bHasTfec)
   {
      BINT_DisableCallback_isr(hChn->hHpCb);
      BINT_ClearCallback_isr(hChn->hHpCb);
   }
   
   if ((hChn->bHasAfec) && BAST_g2_P_IsLdpcOn(h))
   {
      BINT_DisableCallback_isr(hChn->hLdpcLockCb);
      BINT_DisableCallback_isr(hChn->hLdpcNotLockCb);
      BINT_ClearCallback_isr(hChn->hLdpcLockCb);
      BINT_ClearCallback_isr(hChn->hLdpcNotLockCb);
   }
#ifndef BAST_EXCLUDE_TURBO
   if (hChn->bHasTfec)
   {
      BINT_DisableCallback_isr(hChn->hTurboLockCb);
      BINT_DisableCallback_isr(hChn->hTurboNotLockCb);  
      BINT_ClearCallback_isr(hChn->hTurboLockCb);
      BINT_ClearCallback_isr(hChn->hTurboNotLockCb);      
   }
#endif
   if (hChn->bHasDiseqc && bDisableDiseqc)
   {
      BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eGen2);
      BAST_g2_P_DisableDiseqcInterrupts_isr(h);
   }
   
   if (bDisableMi2c)
   {
      BINT_DisableCallback_isr(hChn->hMi2cCb);
      BINT_ClearCallback_isr(hChn->hMi2cCb);
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_DisableChannelInterrupts() - Non-ISR context
******************************************************************************/ 
BERR_Code BAST_g2_P_DisableChannelInterrupts(
   BAST_ChannelHandle h,   /* [in] BAST handle */
   bool bDisableDiseqc,    /* [in] true = disable diseqc interrupts also */
   bool bDisableMi2c       /* [in] true = disable mi2c interrupts also */
)
{
   BERR_Code retCode;
   
   BKNI_EnterCriticalSection();
   retCode = BAST_g2_P_DisableChannelInterrupts_isr(h, bDisableDiseqc, bDisableMi2c);
   BKNI_LeaveCriticalSection();
   
   return retCode;
}


/******************************************************************************
 BAST_g2_P_AbortAcq() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_AbortAcq(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BAST_g2_P_ChannelHandle *hChn;
   BERR_Code retCode;
   
   BDBG_ASSERT(h);
   BDBG_ENTER(BAST_g2_P_AbortAcq);
   
   hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   retCode = BAST_g2_P_DisableChannelInterrupts(h, false, false);

   hChn->acqTimeState = 0;
   /* hChn->acqTime = 0; */
   hChn->acqState = BAST_AcqState_eIdle;
   
   if (hChn->bFsNotDefault)
      BAST_g2_P_SetDefaultSampleFreq(h);
   
   BDBG_LEAVE(BAST_g2_P_AbortAcq);
   return retCode;   
}


/******************************************************************************
 BAST_g2_P_GetPeakScanEventHandle()
******************************************************************************/
BERR_Code BAST_g2_P_GetPeakScanEventHandle(BAST_ChannelHandle h, BKNI_EventHandle *pEvent)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDBG_ENTER(BAST_g2_P_GetPeakScanEventHandle);
   
   *pEvent = ((BAST_g2_P_ChannelHandle *)h->pImpl)->hPeakScanEvent;
   
   BDBG_LEAVE(BAST_g2_P_GetPeakScanEventHandle);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetStatusEventHandle()
******************************************************************************/
BERR_Code BAST_g2_P_GetStatusEventHandle(BAST_ChannelHandle h, BKNI_EventHandle *pEvent)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDBG_ENTER(BAST_g2_P_GetStatusEventHandle);
   
   *pEvent = ((BAST_g2_P_ChannelHandle *)h->pImpl)->hStatusEvent;
   
   BDBG_LEAVE(BAST_g2_P_GetStatusEventHandle);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_PeakScan() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_PeakScan(BAST_ChannelHandle h, uint32_t tunerFreq)
{
#ifndef BAST_EXCLUDE_PEAK_SCAN
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   BDBG_ENTER(BAST_g2_P_PeakScan);

   if (hChn->bExternalTuner)
      return BERR_NOT_SUPPORTED;
      
   BAST_g2_P_IndicateNotLocked(h);
   
   /* BDBG_MSG(("PeakScan() start")); */
   BAST_g2_P_DisableChannelInterrupts(h, false, false);
   
   if ((hChn->peakScanSymRateMin == 0) || (hChn->peakScanSymRateMax == 0))
   {
      hChn->peakScanSymRateMin = hChn->peakScanSymRateMax = 0;
      hChn->acqParams.symbolRate = 20000000;
   }
   else
      hChn->acqParams.symbolRate = hChn->peakScanSymRateMax;
   hChn->acqParams.carrierFreqOffset = 0;
   hChn->acqParams.acq_ctl = BAST_ACQSETTINGS_DEFAULT;
   
   hChn->tunerFreq = tunerFreq;
   hChn->acqCount = 0;
   hChn->blindScanStatus = BAST_BlindScanStatus_ePeakSearch;
   hChn->acqState = BAST_AcqState_ePeakScan;
   hChn->tunerIfStep = 0;

   hChn->peakScanOutput = 0;  
   hChn->peakScanPower = 0;
   
   BDBG_LEAVE(BAST_g2_P_PeakScan);
   return BAST_g2_P_TunerSetFreq(h, BAST_g2_P_PeakScan1);
#else
   return BERR_NOT_SUPPORTED;
#endif
}


#ifndef BAST_EXCLUDE_PEAK_SCAN
/******************************************************************************
 BAST_g2_P_PeakScan1()
******************************************************************************/
BERR_Code BAST_g2_P_PeakScan1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   hChn->funct_state = 0;
   BAST_g2_P_QpskAcquire0(h);
   return BAST_g2_P_PeakScan2(h);
}


/******************************************************************************
 BAST_g2_P_PeakScan2()
******************************************************************************/
BERR_Code BAST_g2_P_PeakScan2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pDevice->pImpl); 
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val, P_hi, P_lo;
   
#if 0
   static const uint32_t script_peak_scan_0[] =
   {
      /* tone detect */
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_RANGE_START, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_RANGE_END, 0x000000FF),  /* 256 */
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_CTRL1, 0x9D), /* FFT size=512 */
      BAST_SCRIPT_WRITE(BCHP_SDS_FILTCTL, 0x40), /* 6 HB */
      BAST_SCRIPT_EXIT
   };
#endif

   static const uint32_t script_peak_scan_1[] =
   {
      /* sym rate scan */
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_RANGE_START, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_RANGE_END, 0x000000FF),  /* +1/8 bandwidth */
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_CTRL1, 0x97), /* 16 x gain, FFT size = 2048 */
      BAST_SCRIPT_EXIT
   };
   
   while (hChn->funct_state != 0xFF)
   {
      switch (hChn->funct_state)
      {
         case 0:
            BAST_g2_P_Enable3StageAgc(h, true);
            BAST_g2_P_3StageAgc_isr((void*)h, 0);
            if (hChn->peakScanSymRateMin == 0)
            {
               val = (uint32_t)hDev->dftRangeStart;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_DFT_RANGE_START, &val);
               
               val = (uint32_t)hDev->dftRangeEnd;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_DFT_RANGE_END, &val);  
               
               val = 0x9C | (hDev->dftSize & 0x03);
               BAST_g2_P_WriteRegister(h, BCHP_SDS_DFT_CTRL1, &val);  
               
               val = 0x40;  
               BAST_g2_P_WriteRegister(h, BCHP_SDS_FILTCTL, &val);  /* 6 HB */                                       
            }
            else
               BAST_g2_P_ProcessScript(h, script_peak_scan_1);
               
            BAST_g2_P_ReadRegister(h, BCHP_SDS_DFT_CTRL1, &val);
            val &= 0x03;
            hChn->count1 = 256 << val; /* count1=dft_size */
            hChn->count2 = ((uint32_t)(hChn->sampleFreq + 64) >> 7); /* count2=step_size=Fs/128 */
            hChn->count3 = hChn->peakScanSymRateMin;  /* count3=current Fb */
            hChn->maxCount1 = 0; /* maxCount1=max_peak_bin */
            hChn->bfos = hChn->peakScanSymRateMin; /* bfos=saved Fb */
            hChn->funct_state = 1;
            break;
            
         case 1:
            BAST_g2_P_3StageAgc_isr((void*)h, 0);   
            
            /* set the DDFS FCW */
            BAST_MultU32U32(hChn->count3, 32768, &P_hi, &P_lo);
            BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &P_hi, &P_lo);
            BAST_g2_P_WriteRegister(h, BCHP_SDS_DFT_DDFS_FCW, &P_lo);
            
            /* start DFT engine */
            val = 1;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_DFT_CTRL0, &val);
            val = 0;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_DFT_CTRL0, &val);
            
            hChn->funct_state = 2;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 500, BAST_g2_P_PeakScan2);
            
         case 2: 
            /* check for DFT done */
            BAST_g2_P_ReadRegister(h, BCHP_SDS_DFT_CTRL0, &val);
            if ((val & 0xE0000000) != 0xC0000000)
               return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 500, BAST_g2_P_PeakScan2);
            
            BAST_g2_P_ReadRegister(h, BCHP_SDS_DFT_PEAK_POW, &val);
            /* BDBG_MSG(("peak scan Fb=%d: pow=%d", hChn->count3, val)); */
            if (val > hChn->peakScanPower)
            {
               hChn->peakScanPower = val;
               BAST_g2_P_ReadRegister(h, BCHP_SDS_DFT_PEAK_BIN, &val);
               hChn->maxCount1 = val;
               hChn->bfos = hChn->count3;  /* bfos=saved Fb */
            }
            hChn->count3 += hChn->count2;
            if (hChn->count3 <= (hChn->peakScanSymRateMax + hChn->count2))
               hChn->funct_state = 1;
            else
               hChn->funct_state = 3;
            break;
            
         case 3:
            if (hChn->peakScanSymRateMax)
            {
               if (hChn->maxCount1 > (hChn->count1 >> 1))
               {
                  BAST_MultU32U32(hChn->count1 - hChn->maxCount1, hChn->sampleFreq, &P_hi, &P_lo);
                  BAST_DivU64U32(P_hi, P_lo, hChn->count1 * 16, &P_hi, &P_lo);                  
                  val = hChn->bfos - P_lo;
               }
               else /* positive freq */
               {
                  BAST_MultU32U32(hChn->maxCount1, hChn->sampleFreq, &P_hi, &P_lo);
                  BAST_DivU64U32(P_hi, P_lo, hChn->count1 * 16, &P_hi, &P_lo);
                  val = hChn->bfos + P_lo;
               }
            }
            else
               val = hChn->maxCount1;
            hChn->peakScanOutput = val;
            hChn->blindScanStatus = BAST_BlindScanStatus_eIdle;
            hChn->acqState = BAST_AcqState_eIdle;
            hChn->funct_state = 0xFF;
            BKNI_SetEvent(hChn->hPeakScanEvent);
            break;

         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);         
            break;
      }
   }
   
   return retCode;
}
#endif


/******************************************************************************
 BAST_g2_P_GetPeakScanStatus()
******************************************************************************/
BERR_Code BAST_g2_P_GetPeakScanStatus(BAST_ChannelHandle h, BAST_PeakScanStatus *pStatus)
{
#ifndef BAST_EXCLUDE_PEAK_SCAN
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   BDBG_ENTER(BAST_g2_P_GetPeakScanStatus);
   
   pStatus->status = (hChn->blindScanStatus == BAST_BlindScanStatus_ePeakSearch ? 1 : 0);
   pStatus->tunerFreq = hChn->tunerFreq;
   pStatus->out = hChn->peakScanOutput;
   pStatus->peakPower = hChn->peakScanPower;
   
   BDBG_LEAVE(BAST_g2_P_GetPeakScanStatus);
   return BERR_SUCCESS;
#else
   return BERR_NOT_SUPPORTED;
#endif
}


/******************************************************************************
 BAST_g2_P_EnableStatusInterrupts()
******************************************************************************/
BERR_Code BAST_g2_P_EnableStatusInterrupts(BAST_ChannelHandle h, bool bEnable)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BDBG_ENTER(BAST_g2_P_EnableStatusInterrupts);
   
   hChn->bStatusInterrupts = bEnable;
   
   BDBG_LEAVE(BAST_g2_P_EnableStatusInterrupts);
   return BERR_SUCCESS;
}



/******************************************************************************
 BAST_g2_ResetInterruptCounters()
******************************************************************************/
void BAST_g2_ResetInterruptCounters(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   int i;
   
   for (i = 0; i < BAST_Sds_MaxIntID; i++)
      hChn->irqCount[i] = 0;
}


/******************************************************************************
 BAST_g2_GetInterruptCount()
******************************************************************************/
BERR_Code BAST_g2_GetInterruptCount(BAST_ChannelHandle h, BAST_Sds_IntID idx, uint32_t *pCount)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if (idx >= BAST_Sds_MaxIntID)
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);      
   }
   else
      *pCount = hChn->irqCount[idx];
   return retCode;
}


/******************************************************************************
 BAST_g2_P_SetSearchRange()
******************************************************************************/
BERR_Code BAST_g2_P_SetSearchRange(BAST_Handle h, uint32_t searchRange)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   
   hDev->searchRange = searchRange;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_GetSearchRange()
******************************************************************************/
BERR_Code BAST_g2_P_GetSearchRange(BAST_Handle h, uint32_t *pSearchRange)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   
   *pSearchRange = hDev->searchRange;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_SetAmcScramblingSeq()
******************************************************************************/
BERR_Code BAST_g2_P_SetAmcScramblingSeq(BAST_ChannelHandle h, uint32_t xseed, uint32_t plhdrscr1, uint32_t plhdrscr2, uint32_t plhdrscr3)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   hChn->ldpcScramblingSeq[0] = (uint8_t)((xseed >> 24) & 0xFF);
   hChn->ldpcScramblingSeq[1] = (uint8_t)((xseed >> 16) & 0xFF);
   hChn->ldpcScramblingSeq[2] = (uint8_t)((xseed >> 8) & 0xFF);
   hChn->ldpcScramblingSeq[3] = (uint8_t)(xseed & 0xFF);
   hChn->ldpcScramblingSeq[4] = (uint8_t)((plhdrscr1 >> 24) & 0xFF);
   hChn->ldpcScramblingSeq[5] = (uint8_t)((plhdrscr1 >> 16) & 0xFF);
   hChn->ldpcScramblingSeq[6] = (uint8_t)((plhdrscr1 >> 8) & 0xFF);
   hChn->ldpcScramblingSeq[7] = (uint8_t)(plhdrscr1 & 0xFF);
   hChn->ldpcScramblingSeq[8] = (uint8_t)((plhdrscr2 >> 24) & 0xFF);
   hChn->ldpcScramblingSeq[9] = (uint8_t)((plhdrscr2 >> 16) & 0xFF);
   hChn->ldpcScramblingSeq[10] = (uint8_t)((plhdrscr2 >> 8) & 0xFF);
   hChn->ldpcScramblingSeq[11] = (uint8_t)(plhdrscr2 & 0xFF);
   hChn->ldpcScramblingSeq[12] = (uint8_t)((plhdrscr3 >> 24) & 0xFF);
   hChn->ldpcScramblingSeq[13] = (uint8_t)((plhdrscr3 >> 16) & 0xFF);
   hChn->ldpcScramblingSeq[14] = (uint8_t)((plhdrscr3 >> 8) & 0xFF);
   hChn->ldpcScramblingSeq[15] = (uint8_t)(plhdrscr3 & 0xFF);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_SetTunerFilter()
******************************************************************************/
BERR_Code BAST_g2_P_SetTunerFilter(BAST_ChannelHandle h, uint32_t cutoffHz)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if (cutoffHz == 0)
      hChn->tunerCtl &= ~BAST_G2_TUNER_CTL_SET_FILTER_MANUAL;      
   else
   {
      hChn->tunerCutoff = (uint8_t)((cutoffHz + 500000) / 1000000); /* round to nearest MHz */
      hChn->tunerCtl |= BAST_G2_TUNER_CTL_SET_FILTER_MANUAL;
   }
 
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_SetOutputTransportSettings()
******************************************************************************/
BERR_Code BAST_g2_P_SetOutputTransportSettings(BAST_ChannelHandle h, BAST_OutputTransportSettings *pSettings)
{
   uint16_t xportCtl = 0;
   uint8_t buf[2];
   
   if (pSettings->bSerial)
      xportCtl |= BAST_G2_XPORT_CTL_SERIAL;
   if (pSettings->bClkInv)
      xportCtl |= BAST_G2_XPORT_CTL_CLKINV;
   if (pSettings->bClkSup)
      xportCtl |= BAST_G2_XPORT_CTL_CLKSUP;
   if (pSettings->bVldInv)
      xportCtl |= BAST_G2_XPORT_CTL_VLDINV;
   if (pSettings->bSyncInv)
      xportCtl |= BAST_G2_XPORT_CTL_SYNCINV;
   if (pSettings->bErrInv)
      xportCtl |= BAST_G2_XPORT_CTL_ERRINV;
   if (pSettings->bXbert)
      xportCtl |= BAST_G2_XPORT_CTL_XBERT;
   if (pSettings->bTei)
      xportCtl |= BAST_G2_XPORT_CTL_TEI;
   if (pSettings->bDelay)
      xportCtl |= BAST_G2_XPORT_CTL_DELAY;
   if (pSettings->bSync1)
      xportCtl |= BAST_G2_XPORT_CTL_SYNC1;
   if (pSettings->bHead4)
      xportCtl |= BAST_G2_XPORT_CTL_HEAD4;
   if (pSettings->bDelHeader)
      xportCtl |= BAST_G2_XPORT_CTL_DELH;        
   if (pSettings->bchMpegErrorMode == BAST_BchMpegErrorMode_eBchOrCrc8)
      xportCtl |= 0;
   else if (pSettings->bchMpegErrorMode == BAST_BchMpegErrorMode_eCrc8)
      xportCtl |= BAST_G2_XPORT_CTL_CRC8_CHECK;
   else if (pSettings->bchMpegErrorMode == BAST_BchMpegErrorMode_eBch)  
      xportCtl |= BAST_G2_XPORT_CTL_BCH_CHECK;
   else 
      xportCtl |= (BAST_G2_XPORT_CTL_CRC8_CHECK | BAST_G2_XPORT_CTL_BCH_CHECK);

   buf[0] = (xportCtl >> 8) & 0xFF;
   buf[1] = (xportCtl & 0xFF);
   return BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_XPORT_CTL, buf, BAST_G2_CONFIG_LEN_XPORT_CTL);
}


/******************************************************************************
 BAST_g2_P_GetOutputTransportSettings()
******************************************************************************/
BERR_Code BAST_g2_P_GetOutputTransportSettings(BAST_ChannelHandle h, BAST_OutputTransportSettings *pSettings)
{
   BERR_Code retCode;
   uint16_t xportCtl;
   uint8_t buf[2];

   retCode = BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_XPORT_CTL, buf, BAST_G2_CONFIG_LEN_XPORT_CTL);
   if (retCode != BERR_SUCCESS)
      return retCode;

   xportCtl = (buf[0] << 8) | buf[1];

   if (xportCtl & BAST_G2_XPORT_CTL_SERIAL)  
      pSettings->bSerial = true;
   else
      pSettings->bSerial = false;
   if (xportCtl & BAST_G2_XPORT_CTL_CLKINV) 
      pSettings->bClkInv = true; 
   else
      pSettings->bClkInv = false;
   if (xportCtl & BAST_G2_XPORT_CTL_CLKSUP)
      pSettings->bClkSup = true;
   else
      pSettings->bClkSup = false;
   if (xportCtl & BAST_G2_XPORT_CTL_VLDINV)
      pSettings->bVldInv = true;
   else
      pSettings->bVldInv = false;
   if (xportCtl & BAST_G2_XPORT_CTL_SYNCINV)
      pSettings->bSyncInv = true;
   else
      pSettings->bSyncInv = false;
   if (xportCtl & BAST_G2_XPORT_CTL_ERRINV)
      pSettings->bErrInv = true;
   else
      pSettings->bErrInv = false;
   if (xportCtl & BAST_G2_XPORT_CTL_XBERT)
      pSettings->bXbert = true;
   else
      pSettings->bXbert = false;
   if (xportCtl & BAST_G2_XPORT_CTL_TEI)
      pSettings->bTei = true;
   else
      pSettings->bTei = false;
   if (xportCtl & BAST_G2_XPORT_CTL_DELAY)
      pSettings->bDelay = true;
   else
      pSettings->bDelay = false;
   if (xportCtl & BAST_G2_XPORT_CTL_SYNC1)
      pSettings->bSync1 = true;
   else
      pSettings->bSync1 = false;
   if (xportCtl & BAST_G2_XPORT_CTL_HEAD4)
      pSettings->bHead4 = true;
   else
      pSettings->bHead4 = false;
   if (xportCtl & BAST_G2_XPORT_CTL_DELH)
      pSettings->bDelHeader = true; 
   else
      pSettings->bDelHeader = false;   
   pSettings->bOpllBypass = false;

   if ((xportCtl & BAST_G2_XPORT_CTL_CRC8_CHECK) &&  ((xportCtl & BAST_G2_XPORT_CTL_BCH_CHECK)==0))
      pSettings->bchMpegErrorMode = BAST_BchMpegErrorMode_eCrc8;
   else if ((xportCtl & BAST_G2_XPORT_CTL_BCH_CHECK) &&  ((xportCtl & BAST_G2_XPORT_CTL_CRC8_CHECK)==0))
      pSettings->bchMpegErrorMode = BAST_BchMpegErrorMode_eBch;
   else if ((xportCtl & BAST_G2_XPORT_CTL_CRC8_CHECK) &&  (xportCtl & BAST_G2_XPORT_CTL_BCH_CHECK))
      pSettings->bchMpegErrorMode = BAST_BchMpegErrorMode_eBchAndCrc8;
   else
      pSettings->bchMpegErrorMode = BAST_BchMpegErrorMode_eBchOrCrc8;
      
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_SetDiseqcSettings()
******************************************************************************/
BERR_Code BAST_g2_P_SetDiseqcSettings(BAST_ChannelHandle h, BAST_DiseqcSettings *pSettings)
{
   BERR_Code retCode;
   uint16_t diseqcCtl = 0;
   uint8_t buf[2], rrto[4];
   
   if (pSettings->bEnvelope)
      diseqcCtl |= BAST_G2_DISEQC_CTL_ENVELOPE;
   if (pSettings->bToneAlign)
      diseqcCtl |= BAST_G2_DISEQC_CTL_TONE_ALIGN_ENABLE;
   if (pSettings->bDisableRRTO)
      diseqcCtl |= BAST_G2_DISEQC_CTL_RRTO_DISABLE;
   if (pSettings->bEnableToneburst)
      diseqcCtl |= BAST_G2_DISEQC_CTL_TONEBURST_ENABLE;
   if (pSettings->bToneburstB)
      diseqcCtl |= BAST_G2_DISEQC_CTL_TONEBURST_B;
   if (pSettings->bOverrideFraming)
   {
      if (pSettings->bExpectReply)
         pSettings->bExpectReply = false;    /* not implemented */
      else
         diseqcCtl |= BAST_G2_DISEQC_CTL_EXP_REPLY_DISABLE;
   }
   if (pSettings->bEnableLNBPU)
      diseqcCtl |= BAST_G2_DISEQC_CTL_LNBPU_TXEN;
   if (pSettings->bDisableRxOnly)
      pSettings->bDisableRxOnly = false;  /* not implemented */
   
   buf[0] = (diseqcCtl >> 8) & 0xFF;
   buf[1] = (diseqcCtl & 0xFF);
   BAST_CHK_RETCODE(BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_DISEQC_CTL, buf, BAST_G2_CONFIG_LEN_DISEQC_CTL));
   
   /* set rrto */
   rrto[0] = (uint8_t)(pSettings->rrtoUsec >> 24);
   rrto[1] = (uint8_t)(pSettings->rrtoUsec >> 16);
   rrto[2] = (uint8_t)(pSettings->rrtoUsec >> 8);
   rrto[3] = (uint8_t)(pSettings->rrtoUsec & 0xFF);
   BAST_CHK_RETCODE(BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_RRTO_USEC, rrto, BAST_G2_CONFIG_LEN_RRTO_USEC));
   
   /* set rx bit threshold */
   buf[0] = (uint8_t)(pSettings->bitThreshold >> 8);
   buf[1] = (uint8_t)(pSettings->bitThreshold & 0xFF);
   BAST_CHK_RETCODE(BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_DISEQC_BIT_THRESHOLD, buf, BAST_G2_CONFIG_LEN_DISEQC_BIT_THRESHOLD));
   
   /* set tone detect threshold */
   BAST_CHK_RETCODE(BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_DISEQC_TONE_THRESHOLD, &(pSettings->toneThreshold), BAST_G2_CONFIG_LEN_DISEQC_TONE_THRESHOLD));
   
   /* set pretx delay */
   BAST_CHK_RETCODE(BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_DISEQC_PRETX_DELAY, &(pSettings->preTxDelay), BAST_G2_CONFIG_LEN_DISEQC_PRETX_DELAY));
   
   /* set vsense thresholds */
   BAST_CHK_RETCODE(BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_VSENSE_THRES_HI, &(pSettings->vsenseThresholdHi), BAST_G2_CONFIG_LEN_VSENSE_THRES_HI));
   BAST_CHK_RETCODE(BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_VSENSE_THRES_LO, &(pSettings->vsenseThresholdLo), BAST_G2_CONFIG_LEN_VSENSE_THRES_LO));
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetDiseqcSettings()
******************************************************************************/
BERR_Code BAST_g2_P_GetDiseqcSettings(BAST_ChannelHandle h, BAST_DiseqcSettings *pSettings)
{
   BERR_Code retCode;
   uint16_t diseqcCtl;
   uint8_t buf[2], rrto[4];
   
   BAST_CHK_RETCODE(BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_DISEQC_CTL, buf, BAST_G2_CONFIG_LEN_DISEQC_CTL));
   diseqcCtl = (buf[0] << 8) | buf[1];

   if (diseqcCtl & BAST_G2_DISEQC_CTL_ENVELOPE)  
      pSettings->bEnvelope = true;
   else
      pSettings->bEnvelope = false;
   if (diseqcCtl & BAST_G2_DISEQC_CTL_TONE_ALIGN_ENABLE) 
      pSettings->bToneAlign = true; 
   else
      pSettings->bToneAlign = false;
   if (diseqcCtl & BAST_G2_DISEQC_CTL_RRTO_DISABLE)
      pSettings->bDisableRRTO = true;
   else
      pSettings->bDisableRRTO = false;
   if (diseqcCtl & BAST_G2_DISEQC_CTL_TONEBURST_ENABLE)
      pSettings->bEnableToneburst = true;
   else
      pSettings->bEnableToneburst = false;
   if (diseqcCtl & BAST_G2_DISEQC_CTL_TONEBURST_B)
      pSettings->bToneburstB = true;
   else
      pSettings->bToneburstB = false;
   if (diseqcCtl & BAST_G2_DISEQC_CTL_EXP_REPLY_DISABLE)
   {
      pSettings->bOverrideFraming = true;
      pSettings->bExpectReply = false;
   }
   else
   {
      pSettings->bOverrideFraming = false;
      pSettings->bExpectReply = false;
   }
   if (diseqcCtl & BAST_G2_DISEQC_CTL_LNBPU_TXEN)
      pSettings->bEnableLNBPU = true;
   else
      pSettings->bEnableLNBPU = false;
   pSettings->bDisableRxOnly = false;  /* not implemented */
   
   /* get rrto */
   BAST_CHK_RETCODE(BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_RRTO_USEC, rrto, BAST_G2_CONFIG_LEN_RRTO_USEC));
   pSettings->rrtoUsec = (rrto[0] << 24) | (rrto[1] << 16) | (rrto[2] << 8) | rrto[3];
   pSettings->rrtoUsec &= 0xFFFFF;
   
   /* get rx bit threshold */
   BAST_CHK_RETCODE(BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_DISEQC_BIT_THRESHOLD, buf, BAST_G2_CONFIG_LEN_DISEQC_BIT_THRESHOLD));
   pSettings->bitThreshold = (buf[0] << 8) | buf[1];
   pSettings->bitThreshold &= 0xFFF;
   
   /* get tone detect threshold */
   BAST_CHK_RETCODE(BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_DISEQC_TONE_THRESHOLD, &(pSettings->toneThreshold), BAST_G2_CONFIG_LEN_DISEQC_TONE_THRESHOLD));
   
   /* get pretx delay */
   BAST_CHK_RETCODE(BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_DISEQC_PRETX_DELAY, &(pSettings->preTxDelay), BAST_G2_CONFIG_LEN_DISEQC_PRETX_DELAY));
   
   /* get vsense thresholds */
   BAST_CHK_RETCODE(BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_VSENSE_THRES_HI, &(pSettings->vsenseThresholdHi), BAST_G2_CONFIG_LEN_VSENSE_THRES_HI));
   BAST_CHK_RETCODE(BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_VSENSE_THRES_LO, &(pSettings->vsenseThresholdLo), BAST_G2_CONFIG_LEN_VSENSE_THRES_LO));
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_P_SetNetworkSpec()
******************************************************************************/
BERR_Code BAST_g2_P_SetNetworkSpec(BAST_Handle h, BAST_NetworkSpec s)
{
   if (s > BAST_NetworkSpec_eEuro)
      return BERR_INVALID_PARAMETER;

   h->settings.networkSpec = s;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_GetNetworkSpec()
******************************************************************************/
BERR_Code BAST_g2_P_GetNetworkSpec(BAST_Handle h, BAST_NetworkSpec *p)
{
   *p = h->settings.networkSpec;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_SetFskChannel()
******************************************************************************/
BERR_Code BAST_g2_P_SetFskChannel(BAST_Handle h, BAST_FskChannel fskTxChannel, BAST_FskChannel fskRxChannel)
{
   uint8_t sb;

   if ((fskTxChannel > BAST_FskChannel_e1) || (fskRxChannel > BAST_FskChannel_e1))
      return BERR_INVALID_PARAMETER;
   
   if ((fskTxChannel == BAST_FskChannel_e0) && (fskRxChannel == BAST_FskChannel_e0))
      sb = BAST_FskChannelConfig_eCh0Tx_Ch0Rx;
   else if ((fskTxChannel == BAST_FskChannel_e0) && (fskRxChannel == BAST_FskChannel_e1))
      sb = BAST_FskChannelConfig_eCh0Tx_Ch1Rx;
   else if ((fskTxChannel == BAST_FskChannel_e1) && (fskRxChannel == BAST_FskChannel_e0))
      sb = BAST_FskChannelConfig_eCh1Tx_Ch0Rx;
   else
      sb = BAST_FskChannelConfig_eCh1Tx_Ch1Rx;
  
   return BAST_WriteConfig(h->pChannels[0], BAST_G2_CONFIG_FTM_CH_SELECT, &sb, BAST_G2_CONFIG_LEN_FTM_CH_SELECT);
}


/******************************************************************************
 BAST_g3_P_GetFskChannel()
******************************************************************************/
BERR_Code BAST_g2_P_GetFskChannel(BAST_Handle h, BAST_FskChannel *fskTxChannel, BAST_FskChannel *fskRxChannel)
{
   BERR_Code retCode;
   uint8_t sb;

   retCode = BAST_ReadConfig(h->pChannels[0], BAST_G2_CONFIG_FTM_CH_SELECT, &sb, BAST_G2_CONFIG_LEN_FTM_CH_SELECT);
   if (retCode == BERR_SUCCESS)
   {
      if (sb == BAST_FskChannelConfig_eCh0Tx_Ch0Rx)
      {
         *fskTxChannel = BAST_FskChannel_e0;
         *fskRxChannel = BAST_FskChannel_e0;
      }
      else if (sb == BAST_FskChannelConfig_eCh0Tx_Ch1Rx)
      {
         *fskTxChannel = BAST_FskChannel_e0;
         *fskRxChannel = BAST_FskChannel_e1;
      }
      else if (sb == BAST_FskChannelConfig_eCh1Tx_Ch0Rx)
      {
         *fskTxChannel = BAST_FskChannel_e1;
         *fskRxChannel = BAST_FskChannel_e0;
      }
      else
      {
         *fskTxChannel = BAST_FskChannel_e1;
         *fskRxChannel = BAST_FskChannel_e1;
      }
   }
   return retCode;
}


/******************************************************************************
 BAST_g2_P_SetPeakScanSymbolRateRange()
******************************************************************************/
BERR_Code BAST_g2_P_SetPeakScanSymbolRateRange(BAST_ChannelHandle h, uint32_t minSymbolRate, uint32_t maxSymbolRate)
{
   BERR_Code retCode; 
   uint8_t buf[4];

   if ((minSymbolRate == 0) || (maxSymbolRate == 0))
   {
      minSymbolRate = 0;
      maxSymbolRate = 0;
   }
   else if ((minSymbolRate < 1000000) || (maxSymbolRate < 1000000) || 
            (minSymbolRate > 45000000) || (maxSymbolRate > 45000000) ||        
            (minSymbolRate > maxSymbolRate))
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
  
   buf[0] = (uint8_t)((minSymbolRate >> 24) & 0xFF);
   buf[1] = (uint8_t)((minSymbolRate >> 16) & 0xFF);
   buf[2] = (uint8_t)((minSymbolRate >> 8) & 0xFF);
   buf[3] = (uint8_t)(minSymbolRate & 0xFF);   
   retCode = BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MIN, buf, BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MIN);
   if (retCode)
      goto done;

   buf[0] = (uint8_t)((maxSymbolRate >> 24) & 0xFF);
   buf[1] = (uint8_t)((maxSymbolRate >> 16) & 0xFF);
   buf[2] = (uint8_t)((maxSymbolRate >> 8) & 0xFF);
   buf[3] = (uint8_t)(maxSymbolRate & 0xFF);   
   retCode = BAST_g2_P_WriteConfig(h, BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MAX, buf, BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MAX);

   done:
   if (retCode != BERR_SUCCESS)
   {
      return BERR_TRACE(retCode);
   }
   return retCode;

}


/******************************************************************************
 BAST_g2_P_GetPeakScanSymbolRateRange()
******************************************************************************/  
BERR_Code BAST_g2_P_GetPeakScanSymbolRateRange(BAST_ChannelHandle h, uint32_t *pMinSymbolRate, uint32_t *pMaxSymbolRate)
{
   BERR_Code retCode; 
   uint8_t buf[4];

   if ((pMinSymbolRate == NULL) || (pMaxSymbolRate == NULL))
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   retCode = BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MIN, buf, BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MIN);
   if (retCode != BERR_SUCCESS)
      goto done;
   *pMinSymbolRate = (uint32_t)((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
   
   retCode = BAST_g2_P_ReadConfig(h, BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MAX, buf, BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MAX);
   if (retCode != BERR_SUCCESS)
      goto done;
   *pMaxSymbolRate = (uint32_t)((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);

   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_P_GetVersionInfo()
******************************************************************************/  
BERR_Code BAST_g2_P_GetVersionInfo(BAST_Handle h, FEC_DeviceVersionInfo *pVersion)
{
   BSTD_UNUSED(h);

   pVersion->majorVersion = BAST_API_VERSION;
   pVersion->minorVersion = BAST_G2_RELEASE_VERSION;
   pVersion->buildId = BAST_G2_BUILD_VERSION;
   pVersion->buildType = 0;
   return BERR_SUCCESS;
}
