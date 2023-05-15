/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/130 $
 * $brcm_Date: 10/15/12 4:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2_priv.h $
 * 
 * Hydra_Software_Devel/130   10/15/12 4:07p enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/129   10/10/12 4:23p enavarro
 * SWSATFE-218: implement manual spectral inversion scan on external tuner
 * 
 * Hydra_Software_Devel/128   9/6/12 6:13p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/127   11/10/11 4:18p ronchan
 * SWSATFE-156: add diseqc tone state to channel handle
 * 
 * Hydra_Software_Devel/126   10/10/11 9:04a enavarro
 * SWSATFE-150: removed declaration of BAST_73xx_P_EnableSpurCanceller()
 * 
 * Hydra_Software_Devel/125   10/5/11 9:46a enavarro
 * SWSATFE-150: spur canceller not supported in 73xx AST
 * 
 * Hydra_Software_Devel/124   1/31/11 5:22p ronchan
 * SWSATFE-99: added implementation for BAST_SetFskChannel() and
 * BAST_GetFskChannel()
 * 
 * Hydra_Software_Devel/123   1/27/11 4:17p enavarro
 * SW7346-21: added implementation for BAST_SetPeakScanSymbolRateRange()
 * and BAST_GetPeakScanSymbolRateRange()
 * 
 * Hydra_Software_Devel/122   11/30/10 10:42a ckpark
 * SWSATFE-91: added reacqCtl to channel handle struct
 * 
 * Hydra_Software_Devel/121   11/30/10 9:37a ckpark
 * SWSATFE-80: implemented BAST_SetNetworkSpec() and
 * BAST_GetNetworkSpec(); added bForceReacq to channel handle struct
 * 
 * Hydra_Software_Devel/120   9/30/10 5:03p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/119   9/30/10 9:08a enavarro
 * SWSATFE-80: added implementation functions for
 * SetOutputTransportSettings() and GetOutputTransportSettings()
 * 
 * Hydra_Software_Devel/118   9/17/10 1:50p enavarro
 * SWSATFE-80: added BAST_SetSearchRange(), BAST_GetSearchRange(),
 * BAST_SetTunerFilter(), and BAST_SetAmcScramblingSeq()
 * 
 * Hydra_Software_Devel/117   7/1/10 10:36a ronchan
 * SWSATFE-65: add diseqc bit detection threshold parameter
 * 
 * Hydra_Software_Devel/116   2/4/10 11:11a ronchan
 * SWSATFE-27: remove diseqc spll serialization
 * 
 * Hydra_Software_Devel/115   1/21/10 5:06p ronchan
 * SWSATFE-22: added error buffer for ftm
 * 
 * Hydra_Software_Devel/114   11/3/09 2:04p enavarro
 * SWSATFE-1: removed BAST_USE_OLD_PLC option
 * 
 * Hydra_Software_Devel/113   10/27/09 9:50a ronchan
 * SW7342-54: added definition for BAST_73xx_P_InitHandleDefault
 * 
 * Hydra_Software_Devel/112   10/26/09 11:04a enavarro
 * SW7342-54: added ndiv, m1div, m2div, and m3div to BAST_73xx_P_Handle
 * 
 * Hydra_Software_Devel/111   9/24/09 5:06p enavarro
 * SW7340-40: add code to use DACPLL/MIXPLL state machines for BCM734x
 * 
 * Hydra_Software_Devel/110   9/22/09 9:10a enavarro
 * HW4506-60: fix compiler warnings
 * 
 * Hydra_Software_Devel/109   9/14/09 9:22a enavarro
 * HW4506-79: restore VerifyLock algorithm because it is needed for
 * external tuner
 * 
 * Hydra_Software_Devel/108   9/10/09 9:27a enavarro
 * SW7325-594: moved functions used for fixed point arithmetic to common
 * location
 * 
 * Hydra_Software_Devel/107   9/9/09 9:52a enavarro
 * HW4506-71: added noSyncCount to BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/106   9/8/09 3:59p enavarro
 * HW4506-67: use new plc programming for turbo modes
 * 
 * Hydra_Software_Devel/105   9/1/09 4:36p enavarro
 * HW4506-60: added new configuration parameters for PLC override
 * 
 * Hydra_Software_Devel/104   8/27/09 5:01p enavarro
 * HW4506-78: add code for DFT frequency scan; removed AcquireTimeLock
 * code
 * 
 * Hydra_Software_Devel/103   8/20/09 3:41p enavarro
 * PR 57155: added variables in BAST_73xx_P_ChannelHandle struct to
 * support DFT freq estimation
 * 
 * Hydra_Software_Devel/102   8/10/09 10:49a ronchan
 * PR 55235: add support for 7340 and 7342
 * 
 * Hydra_Software_Devel/101   8/3/09 5:32p ronchan
 * PR 55860: renamed DisableDiseqcInterrupts for isr context
 * 
 * Hydra_Software_Devel/100   7/6/09 5:08p ronchan
 * PR 56602: add return code for SetOpll function
 * 
 * Hydra_Software_Devel/99   7/1/09 7:10p ronchan
 * PR 56509: add definition for BAST_EnableDiseqcLnb()
 * 
 * Hydra_Software_Devel/98   6/26/09 11:03a ckpark
 * PR 56263: added BAST_DCO_OFFSET_THRESHOLD; updated BAST_DCO_OFFSET;
 * added frfStep in channel handle struct
 * 
 * Hydra_Software_Devel/97   6/23/09 3:50p ckpark
 * PR 56263: declared BAST_73xx_P_TunerRampMb2(); added tunerOffset to
 * channel handle struct for DCO avoidance
 * 
 * Hydra_Software_Devel/96   6/22/09 9:31a enavarro
 * PR 56135: make common function for setting ONFN register
 * 
 * Hydra_Software_Devel/95   6/19/09 2:25p enavarro
 * PR 56165: have common functions for read/writing FLIF
 * 
 * Hydra_Software_Devel/94   6/18/09 2:46p enavarro
 * PR 56135: improved turbo scan acquisition time
 * 
 * Hydra_Software_Devel/93   6/9/09 6:33p ronchan
 * PR 55876: added 3stage_agc_top config parameter
 * 
 * Hydra_Software_Devel/92   5/26/09 3:33p enavarro
 * PR 53391: added tunerLpfToCalibrate to BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/91   5/22/09 3:08p enavarro
 * PR 54801: declared BAST_73xx_P_SetBackCarrierBw()
 * 
 * Hydra_Software_Devel/90   5/12/09 3:25p ronchan
 * PR 54946: changed voltage type for BAST_GetDiseqcVoltage()
 * 
 * Hydra_Software_Devel/89   4/30/09 9:29a enavarro
 * PR 51107: declared BAST_73xx_P_LdpcIsPilotOn()
 * 
 * Hydra_Software_Devel/88   4/27/09 3:16p ronchan
 * PR 54581: added flag to indicate if FTM core is powered down
 * 
 * Hydra_Software_Devel/87   4/20/09 2:03p enavarro
 * PR 37769: added counters for all interrupts
 * 
 * Hydra_Software_Devel/86   4/15/09 10:31a enavarro
 * PR 51107: updated PLI-to-FLI freq leak algorithm
 * 
 * Hydra_Software_Devel/85   4/13/09 3:49p ronchan
 * PR 54006: added new private function TuneAcquire1()
 * 
 * Hydra_Software_Devel/84   4/13/09 10:55a ronchan
 * PR 54006: created DisableChannelInterrupts_isr() for ISR context
 * 
 * Hydra_Software_Devel/83   3/31/09 10:45a ronchan
 * PR 37881: modified ftm handle for fsk
 * 
 * Hydra_Software_Devel/82   3/20/09 4:24p enavarro
 * PR 52954: removed BAST_MAX_SYMRATE_FOR_ACQ_TIME_LOCK because it is no
 * longer used
 * 
 * Hydra_Software_Devel/81   3/12/09 4:18p enavarro
 * PR 51665: added fullTuneFreq to BAST_73xx_P_ChannelHandle struct;
 * updated tuneMixStatus bit definitions
 * 
 * Hydra_Software_Devel/80   3/2/09 3:44p enavarro
 * PR 51665: updated tuneMixStatus bits
 * 
 * Hydra_Software_Devel/79   2/26/09 2:19p enavarro
 * PR 51665: implemented revised algorithm for tuner mixpll locking
 * 
 * Hydra_Software_Devel/78   2/17/09 4:56p ronchan
 * PR 37769: removed function to determine if ftm is busy
 * 
 * Hydra_Software_Devel/77   2/17/09 3:55p ronchan
 * PR 37881: added fsk option
 * 
 * Hydra_Software_Devel/76   2/12/09 4:29p enavarro
 * PR 51665: added bSetLpf in channel handle struct
 * 
 * Hydra_Software_Devel/75   2/12/09 3:39p ronchan
 * PR 52055: remove poll processing logic
 * 
 * Hydra_Software_Devel/74   2/12/09 3:21p enavarro
 * PR 51665: added variables for tuner gmx adjustment in channel handle
 * struct
 * 
 * Hydra_Software_Devel/73   2/12/09 2:08p enavarro
 * PR 51665: added variables in channel handle struct for DCO convergence
 * check and dynamic tuner bandgap setting
 * 
 * Hydra_Software_Devel/72   1/31/09 1:35a jrubio
 * PR51629: fix 7336 support
 * 
 * Hydra_Software_Devel/71   1/29/09 6:34p ronchan
 * PR 51629: added support for 7336
 * 
 * Hydra_Software_Devel/70   1/12/09 3:38p enavarro
 * PR 50993: added turboCtl to BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/69   1/8/09 4:05p enavarro
 * PR 50925: removed acqTimeInitLock from BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/68   12/22/08 2:33p ronchan
 * PR 50635: added configuration parameter for external tuner AGC
 * thresholds
 * 
 * Hydra_Software_Devel/67   12/19/08 9:19a enavarro
 * PR 49928: added bFrfTransferred to BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/66   12/3/08 3:04p ronchan
 * PR 49936: changed function definition for InitChannels
 * 
 * Hydra_Software_Devel/65   10/31/08 3:16p enavarro
 * PR 37769: removed BAST_73xx_P_ToneSearchMode(); added
 * currentCnrThreshold1 to channel handle
 * 
 * Hydra_Software_Devel/64   10/20/08 3:34p ronchan
 * PR 37769: renamed channel reset
 * 
 * Hydra_Software_Devel/63   10/16/08 2:25p ronchan
 * PR 37769: added channel reset function
 * 
 * Hydra_Software_Devel/62   10/14/08 3:11p enavarro
 * PR 37769: save/restore OVRDSEL register when freezing/unfreezing loops
 * 
 * Hydra_Software_Devel/61   10/13/08 5:11p ronchan
 * PR 37769: added BCM3445_CTL config parameter
 * 
 * Hydra_Software_Devel/60   10/13/08 3:09p ronchan
 * PR 37769: added channel reset function
 * 
 * Hydra_Software_Devel/59   10/6/08 10:38a ronchan
 * PR 37769: added exclude flags for BSL savings
 * 
 * Hydra_Software_Devel/58   10/3/08 10:10a enavarro
 * PR 37769: implemented EnableSpurCanceller API function
 * 
 * Hydra_Software_Devel/57   10/1/08 3:50p enavarro
 * PR 37769: added STUFF_BYTES config parameter
 * 
 * Hydra_Software_Devel/56   10/1/08 2:18p enavarro
 * PR 37769: added tuner gain calibration to AcqState; added tunerGain and
 * firstTimeLock indicator to BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/55   9/29/08 3:06p ronchan
 * PR 37769: added function to determine if ftm is busy
 * 
 * Hydra_Software_Devel/54   9/17/08 9:11a enavarro
 * PR 37769: added functions to provide more flexibility for controlling
 * the BCM3445
 * 
 * Hydra_Software_Devel/53   8/25/08 4:35p enavarro
 * PR 37769: added DFT_RANGE_START, DFT_RANGE_END, and DFT_SIZE config
 * parameters
 * 
 * Hydra_Software_Devel/52   8/22/08 12:11p enavarro
 * PR 37769: added BAST_73xx_P_IsLdpcOn() and BAST_73xx_P_IsTurboOn()
 * 
 * Hydra_Software_Devel/51   8/15/08 3:37p ronchan
 * PR 37769: added diseqc_tone_threshold config parameter
 * 
 * Hydra_Software_Devel/50   8/14/08 4:10p enavarro
 * PR 37769: declare BAST_73xx_P_LdpcUpdateBlockCounters()
 * 
 * Hydra_Software_Devel/49   8/5/08 4:42p ronchan
 * PR 37881: removed '//' style comments
 * 
 * Hydra_Software_Devel/48   7/21/08 4:50p enavarro
 * PR 37769: removed snr_threshold1 and snr_threshold2 config parameters
 * 
 * Hydra_Software_Devel/47   7/8/08 3:55p ronchan
 * PR 37881: added config parameters for tx power & channel select
 * 
 * Hydra_Software_Devel/46   7/3/08 9:54a enavarro
 * PR 37769: reworked BAST_73xx_P_QpskAcquire0()
 * 
 * Hydra_Software_Devel/45   7/1/08 12:06p enavarro
 * PR 37769: added ratio1 and ratio2 to BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/44   6/26/08 4:31p enavarro
 * PR 37881: added bit definitions for ldpcScanState
 * 
 * Hydra_Software_Devel/43   6/26/08 2:04p enavarro
 * PR 37881: removed tunerMb2 from BAST_73xx_P_ChannelHandle struct; added
 * BAST_73xx_P_UpdateSplitterModeAdj()
 * 
 * Hydra_Software_Devel/42   6/20/08 11:10a enavarro
 * PR 37769: added support for rain_fade status events
 * 
 * Hydra_Software_Devel/41   6/19/08 2:33p enavarro
 * PR 37769: added support for status events
 * 
 * Hydra_Software_Devel/40   6/16/08 2:44p enavarro
 * PR 37769: added BAST_73xx_P_TurboSetPlc()
 * 
 * Hydra_Software_Devel/39   6/13/08 5:15p enavarro
 * PR 43276: added BAST_73xx_P_TransferFreqOffset()
 * 
 * Hydra_Software_Devel/38   6/10/08 5:15p enavarro
 * PR 37769: added support for measuring acq time
 * 
 * Hydra_Software_Devel/37   6/2/08 2:14p enavarro
 * PR 37769: added support for peak scan
 * 
 * Hydra_Software_Devel/36   5/30/08 3:40p enavarro
 * PR 37881: added hPeakScanEvent, hStatusEvent, peakScanSymRateMin, and
 * peakScanSymRateMax to BAST_73xx_P_ChannelHandle
 * 
 * Hydra_Software_Devel/35   5/30/08 10:20a enavarro
 * PR 37769: added BAST_GetStatusEventHandle()
 * 
 * Hydra_Software_Devel/34   5/29/08 5:01p enavarro
 * PR 37769: added function prototype for 73xx implementation of
 * BAST_PeakScan(), BAST_GetPeakScanStatus(),
 * BAST_GetPeakScanEventHandle(), and BAST_EnableStatusInterrupts()
 * 
 * Hydra_Software_Devel/33   5/27/08 3:24p ronchan
 * PR 37769: added options to compile out powerdown functions
 * 
 * Hydra_Software_Devel/32   5/23/08 11:38a ronchan
 * PR 37769: added options to compile out turbo, splitter mode
 * 
 * Hydra_Software_Devel/29   5/20/08 10:53a enavarro
 * PR 37881: removed tunerFilterAdj from BAST_73xx_P_ChannelHandle struct
 * because it was no longer needed
 * 
 * Hydra_Software_Devel/28   5/16/08 4:24p enavarro
 * PR 37881: added tunerMb2, tunerVco, and tunerFilterAdj to
 * BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/27   5/15/08 4:29p enavarro
 * PR 37881: added tunerFilterAdj and splitterModeAdj to
 * BAST_73xx_P_ChannelHandle struct; added vcoSeparation to
 * BAST_73xx_P_Handle struct
 * 
 * Hydra_Software_Devel/26   5/13/08 2:00p enavarro
 * PR 37769: added numInternalTuners to BAST_73xx_P_Handle struct
 * 
 * Hydra_Software_Devel/25   5/9/08 6:01p enavarro
 * PR 37769: added tuneDacDivRatio to channel handle struct
 * 
 * Hydra_Software_Devel/24   5/6/08 4:08p enavarro
 * PR 37769: added turboScanCurrMode to BAST_73xx_P_ChannelHandle
 * 
 * Hydra_Software_Devel/23   5/6/08 10:24a enavarro
 * PR 37769: added dtvScanCodeRates, dvbScanCodeRates, and turboScanModes
 * config params; added maxSweep to BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/22   4/30/08 3:57p enavarro
 * PR 37769: added BAST_73xx_P_SetDefaultSampleFreq()
 * 
 * Hydra_Software_Devel/21   4/28/08 2:22p ronchan
 * PR 37881: added vsense functions
 * 
 * Hydra_Software_Devel/20   4/16/08 11:28a enavarro
 * PR 37769: declare some functions that are shared between ldpc and turbo
 * acquisition
 * 
 * Hydra_Software_Devel/19   4/2/08 11:31a ronchan
 * PR 37881: added vsense thresholds, added GetDiseqcVoltage function
 * definition
 * 
 * Hydra_Software_Devel/18   3/12/08 5:19p enavarro
 * PR 37769: added lockIsrFlag in BAST_73xx_P_ChannelHandle struct; added
 * reacqDelay in BAST_73xx_P_Handle
 * 
 * Hydra_Software_Devel/17   3/10/08 4:31p enavarro
 * PR 37769: added BAST_73xx_P_FreezeLoops() and
 * BAST_73xx_P_UnfreezeLoops(); added agictl and agtctl in
 * BAST_73xx_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/16   2/12/08 9:21a ronchan
 * PR 37769: added coresPoweredDown to channel struct
 * 
 * Hydra_Software_Devel/15   2/4/08 4:11p enavarro
 * PR 37769: renamed BAST_73xx_P_LdpcSetDigctl7() to
 * BAST_73xx_P_TunerSetDigctl7()
 * 
 * Hydra_Software_Devel/14   1/18/08 11:33a enavarro
 * PR 37769: make BAST_73xx_P_TunerSetFreq() return BERR_Code
 * 
 * Hydra_Software_Devel/13   12/19/07 11:38a enavarro
 * PR 37769: fixed compiler warning
 * 
 * Hydra_Software_Devel/12   12/19/07 11:32a enavarro
 * PR 37769: added support for blind scan
 * 
 * Hydra_Software_Devel/11   12/18/07 11:54a enavarro
 * PR 37769: added BAST_73xx_P_GetSnrEstimate()
 * 
 * Hydra_Software_Devel/10   12/17/07 11:26a enavarro
 * PR 37769: added lockFilterRamp to ChanelHandle struct
 * 
 * Hydra_Software_Devel/9   12/14/07 1:28p ronchan
 * PR 37769: moved tuner dac div ratio to chip specific headers
 * 
 * Hydra_Software_Devel/8   12/14/07 10:14a ronchan
 * PR 37881: added channel interrupt enumeration
 * 
 * Hydra_Software_Devel/7   12/12/07 3:53p ckpark
 * PR 37769: move lock filter macros in bast_73xx_priv_qpsk.c
 * 
 * Hydra_Software_Devel/6   12/10/07 2:37p ronchan
 * PR 37769: added lock filter time to channel handle
 * 
 * Hydra_Software_Devel/5   12/6/07 11:33a ronchan
 * PR 37769: added support for 7335
 * 
 * Hydra_Software_Devel/4   12/5/07 10:37a ckpark
 * PR 37769: added maxReacq; get lock status from
 * BAST_IndicateLocked/BAST_IndicateNotLocked
 * 
 * Hydra_Software_Devel/3   12/4/07 4:41p ronchan
 * PR 37769: added turbo functions
 * 
 * Hydra_Software_Devel/2   11/30/07 10:14a ronchan
 * PR 37769: function prototype for GetLockChangeEventHandle
 * 
 * Hydra_Software_Devel/1   11/21/07 11:26a ronchan
 * PR 37769: initial version
 *
 ***************************************************************************/
#ifndef _BAST_G2_PRIV_H__
#define _BAST_G2_PRIV_H__

/* #define BAST_DISABLE_TUNER_CAL */

#include "bast_g2.h"
#if BCHP_CHIP==7325
   #include "bast_7325_priv.h"
#elif (BCHP_CHIP==7335) 
   #include "bast_7335_priv.h"
#elif (BCHP_CHIP==7340)
   #include "bast_7340_priv.h"
#elif (BCHP_CHIP==7342)
   #include "bast_7342_priv.h"
#else
   #error "unsupported BCHP_CHIP"
#endif


#define BAST_CHK_RETCODE(x) \
   { if ((retCode = (x)) != BERR_SUCCESS) goto done; }

#define BAST_SCRIPT_OPCODE          0xF0000000
#define BAST_SCRIPT_OPCODE_WRITE    0x00000000
#define BAST_SCRIPT_OPCODE_AND      0x10000000
#define BAST_SCRIPT_OPCODE_OR       0x20000000
#define BAST_SCRIPT_OPCODE_AND_OR   0x30000000
#define BAST_SCRIPT_OPCODE_DEBUG    0x40000000
#define BAST_SCRIPT_OPCODE_EXIT     0xF0000000
#define BAST_SCRIPT_WRITE(reg,val)  (BAST_SCRIPT_OPCODE_WRITE | reg), val
#define BAST_SCRIPT_AND(reg,val)    (BAST_SCRIPT_OPCODE_AND | reg), val
#define BAST_SCRIPT_OR(reg,val)     (BAST_SCRIPT_OPCODE_OR | reg), val
#define BAST_SCRIPT_AND_OR(reg,a,o) (BAST_SCRIPT_OPCODE_AND_OR | reg), a, o
#define BAST_SCRIPT_EXIT            BAST_SCRIPT_OPCODE_EXIT
#define BAST_SCRIPT_DEBUG(val)      BAST_SCRIPT_OPCODE_DEBUG, val

/* bit definitions for ldpcScanState */
#define BAST_LDPC_SCAN_OFF           0x00
#define BAST_LDPC_SCAN_ON            0x80
#define BAST_LDPC_SCAN_FOUND         0x40
#define BAST_LDPC_SCAN_MASK          0xC0
#define BAST_LDPC_SCAN_8PSK_NO_PILOT 0x00
#define BAST_LDPC_SCAN_8PSK_PILOT    0x01
#define BAST_LDPC_SCAN_QPSK_NO_PILOT 0x02
#define BAST_LDPC_SCAN_QPSK_PILOT    0x03
#define BAST_LDPC_SCAN_QPSK          0x02
#define BAST_LDPC_SCAN_PILOT         0x01
#define BAST_LDPC_SCAN_MODE_MASK     0x03

/* bit definitions for turboScanState */
#define BAST_TURBO_SCAN_HP_INIT        0x01
#define BAST_TURBO_SCAN_HP_LOCKED      0x02
#define BAST_TURBO_SCAN_8PSK_HP_LOCKED 0x04
#define BAST_TURBO_SCAN_8PSK_FAILED    0x08

/* bit definitions for tuneMixStatus */
#define BAST_TUNE_MIX_RESUME_RETRY                    0x02
#define BAST_TUNE_MIX_GIVE_UP                         0x04
#define BAST_TUNE_MIX_NEXT_RETRY                      0x08
#define BAST_TUNE_MIX_RESET_IF_PREVIOUS_SETTING_FAILS 0x10
#define BAST_TUNE_MIX_LOCKED                          0x80

#define BAST_DCO_OFFSET 15000 
#define BAST_DCO_OFFSET_THRESHOLD 10000

#define BAST_DEFAULT_SAMPLE_FREQ 135000000

#define BAST_FREQ_ESTIMATE_STATUS_START  0x01
#define BAST_FREQ_ESTIMATE_STATUS_FS     0x02
#define BAST_FREQ_ESTIMATE_STATUS_COARSE 0x04
#define BAST_FREQ_ESTIMATE_STATUS_FINE   0x08
#define BAST_FREQ_ESTIMATE_STATUS_DONE   0x80

#define BAST_SPINV_STATE_SCAN_ENABLED 0x80
#define BAST_SPINV_STATE_PASS         0x01
#define BAST_SPINV_STATE_INV          0x02

#define BAST_EXCLUDE_SPUR_CANCELLER

/* exclude flags for BSL code savings */
/*
#define BAST_EXCLUDE_TURBO
#define BAST_EXCLUDE_STATUS_EVENTS
#define BAST_EXCLUDE_POWERDOWN
#define BAST_EXCLUDE_SPLITTER_MODE
#define BAST_EXCLUDE_OLD_LNA
#define BAST_EXCLUDE_PEAK_SCAN
*/


typedef BERR_Code (*BAST_g2_FUNCT)(BAST_ChannelHandle);

typedef enum BAST_TimerSelect
{
   BAST_TimerSelect_eBaud = 0,
   BAST_TimerSelect_eBaudUsec,
   BAST_TimerSelect_eBer,
   BAST_TimerSelect_eGen1,
   BAST_TimerSelect_eGen2,
   BAST_TimerSelect_eXtal 
} BAST_TimerSelect;

#define BAST_TIMER_DISEQC BAST_TimerSelect_eGen2

typedef enum BAST_AcqState
{
   BAST_AcqState_eIdle = 0,
   BAST_AcqState_eTunerGainCalibration,
   BAST_AcqState_eTuning,
   BAST_AcqState_eAcquiring,
   BAST_AcqState_eWaitForLock,
   BAST_AcqState_ePeakScan,
   BAST_AcqState_eMonitor
} BAST_AcqState;

typedef enum BAST_BlindScanStatus
{
   BAST_BlindScanStatus_eIdle = 0,
   BAST_BlindScanStatus_ePeakSearch,
   BAST_BlindScanStatus_eAcquire
} BAST_BlindScanStatus;


#define BAST_BCM3445_DEFAULT_ADDRESS 0xD8

/* function pointer for ftm idle funct */
typedef BERR_Code (*BAST_FTM_ISR)(BAST_Handle);

/***************************************************************************
Summary:
   Structure for FTM device of the BAST_g2_P_Handle
Description:
   This is the FTM device component of the BAST_g2_P_Handle.
See Also:
   None.
****************************************************************************/
typedef struct
{
#ifndef BAST_ENABLE_SKIT_FSK
   BAST_FTM_ISR         idle_funct;
   BKNI_EventHandle     event;
   BINT_CallbackHandle  hCallback[32];          /* ftm interrupts callback array */
   uint32_t             rid;                    /* receiver id */
   uint32_t             reg_time_left;          /* registration time remaining */
   bool                 bForwardPacket;         /* determine whether to forward rcvd network packet to host */
   uint16_t             options;                /* current option bits */
   uint16_t             req_options;            /* requested option bits */
   uint16_t             rx_bit_mask;
   uint16_t             status;
   uint8_t              tx_buffer[65];
   uint8_t              rx_buffer[65];
   uint8_t              host_buffer[65];
   uint8_t              err_buffer[65];
   uint8_t              buf_status;
   uint8_t              err_status;
   uint8_t              rx_packet_cmd;          /* received command id */
   uint8_t              rx_packet_src_addr;     /* received source address */
   uint8_t              rx_packet_dest_addr;    /* received destination address */
   uint8_t              rx_packet_data_len;     /* received data length */
   uint8_t              rx_packet_header_crc;   /* received header crc */
   uint8_t              sp_packet_src_addr;     /* used to store src address of outgoing sp packet */
   uint8_t              num_offers;
   uint8_t              network_activity;
#else
   BINT_CallbackHandle  hCallback[32];       /* ftm interrupts callback array */
   bool                 bLnbDetected;        /* indicates LNB detection */
   uint16_t             status;
   uint16_t             timecount_max;       /* TC max calculated from N_channel in 0.1ms units */
   uint16_t             slot_length;         /* TS slot length varies from 3000us to 6500us in 500us steps */
   uint16_t             tdma_cycle;          /* (N_Channel + 1) * slot_length / 100 in 0.1ms units */
   uint8_t              tx_buffer[65];
   uint8_t              rx_buffer[65];
   uint8_t              buf_status;
   uint8_t              err_status;
   uint8_t              n_channel;           /*N_Channel is the number of channels in the LNB, used to define number of time slots in system */
   
#endif
} BAST_g2_P_FtmDevice;


/***************************************************************************
Summary:
   Structure for chip-specific portion of the BAST handle
Description:
   This is the chip-specific component of the BAST_Handle.
See Also:
   None.
****************************************************************************/
typedef struct BAST_g2_P_Handle
{
   BREG_Handle           hRegister;          /* register handle */
   BINT_Handle           hInterrupt;         /* interrupt handle */
   BKNI_EventHandle      hFtmEvent;          /* FTM event handle */
   BKNI_EventHandle      hInitDoneEvent;     /* init done event handle */
   BAST_LnaIntConfig     lnaIntConfig;       /* bcm3445 lna configuration */
   BAST_LnaExtConfig     lnaExtConfig;       /* bcm3445 lna configuration */
   BAST_g2_P_FtmDevice hFtmDev;            /* FTM device handle */   
   uint32_t              xtalFreq;           /* crystal freq in Hz */
   uint32_t              searchRange;        /* tuner search range in Hz */
   uint32_t              reacqDelay;         /* reacquisition delay in usecs */
   uint32_t              vcoSeparation;      /* used in splitter mode */
   bool                  bBcm3445;           /* TRUE = has bcm3445 lna */
   bool                  bFtmLocalReset;     /* distinguish between bast reset or local reset */
   bool                  bFtmPoweredDown;    /* indicates if ftm core powered down */
   uint16_t              dftRangeStart;      /* configuration parameter used in tone detect */
   uint16_t              dftRangeEnd;        /* configuration parameter used in tone detect */
   uint8_t               bcm3445Address;     /* i2c address of bcm3445 */
   uint8_t               bcm3445Mi2cChannel; /* specifies which mi2c channel the 3445 is connected to */
   uint8_t               numInternalTuners;  /* number of internal tuners on this chip */
   uint8_t               ftmTxPower;         /* config param for transmit power */
   uint8_t               ftmChannelSelect;   /* config param for channel select */
   uint8_t               dftSize;            /* configuration parameter used in tone detect */ 
   uint8_t               ndiv;               /* spll ndiv */
   uint8_t               m1div;              /* default Fs divider */
   uint8_t               m2div;              /* tfec clock divider */
   uint8_t               m3div;              /* afec clock divider */
} BAST_g2_P_Handle;


/***************************************************************************
Summary:
   Structure for chip-specific portion of the BAST channel handle
Description:
   This is the chip-specific component of the BAST_ChannelHandle.
See Also:
   None.
****************************************************************************/
typedef struct BAST_g2_P_ChannelHandle
{
   BKNI_EventHandle     hLockChangeEvent;    /* change of lock status event handle */
   BKNI_EventHandle     hDiseqcEvent;        /* diseqc event handle */
   BKNI_EventHandle     hDiseqcOverVoltageEvent;   /* diseqc over-voltage event handle */
   BKNI_EventHandle     hDiseqcUnderVoltageEvent;  /* diseqc under-voltage event handle */
   BKNI_EventHandle     hMi2cEvent;          /* mi2c event handle */
   BKNI_EventHandle     hPeakScanEvent;      /* peak scan event handle */
   BKNI_EventHandle     hStatusEvent;        /* status event handle */
   BAST_AcqSettings     acqParams;           /* input acquisition parameters */
   BAST_AcqState        acqState;            /* acquisition state */
   BINT_CallbackHandle  hDiseqcDoneCb;          /* callback handle for diseqc done interrupt */
   BINT_CallbackHandle  hDiseqcOverVoltageCb;   /* callback handle for diseqc over voltage interrupt */
   BINT_CallbackHandle  hDiseqcUnderVoltageCb;  /* callback handle for diseqc under voltage interrupt */
   BINT_CallbackHandle  hBaudTimerCb;           /* callback handle for baud clock timer interrupt */
   BINT_CallbackHandle  hBerTimerCb;            /* callback handle for BER timer interrupt */
   BINT_CallbackHandle  hGen1TimerCb;           /* callback handle for general timer 1 interrupt */
   BINT_CallbackHandle  hGen2TimerCb;           /* callback handle for general timer 2 interrupt */
   BINT_CallbackHandle  hXtalTimerCb;           /* callback handle for crystal timer interrupt */
   BINT_CallbackHandle  h3StageAgc0Cb;          /* callback handle for 3-stage AGC interrupt */
   BINT_CallbackHandle  h3StageAgc1Cb;          /* callback handle for 3-stage AGC interrupt */
   BINT_CallbackHandle  h3StageAgc2Cb;          /* callback handle for 3-stage AGC interrupt */
   BINT_CallbackHandle  h3StageAgc3Cb;          /* callback handle for 3-stage AGC interrupt */
   BINT_CallbackHandle  h3StageAgc4Cb;          /* callback handle for 3-stage AGC interrupt */
   BINT_CallbackHandle  hQpskLockCb;            /* callback handle for Legacy mode lock interrupt */
   BINT_CallbackHandle  hQpskNotLockCb;         /* callback handle for Legacy mode not lock interrupt */
   BINT_CallbackHandle  hLdpcLockCb;            /* callback handle for LDPC lock interrupt */
   BINT_CallbackHandle  hLdpcNotLockCb;         /* callback handle for LDPC not lock interrupt */
   BINT_CallbackHandle  hHpCb;                  /* callback handle for HP interrupt */
   BINT_CallbackHandle  hTurboLockCb;           /* callback handle for turbo lock interrupt */
   BINT_CallbackHandle  hTurboNotLockCb;        /* callback handle for turbo not lock interrupt */
   BINT_CallbackHandle  hMi2cCb;                /* callback handle for MI2C interrupt */
   BAST_g2_FUNCT      baudTimerIsr;        /* function for handling baud timer interrupt */
   BAST_g2_FUNCT      berTimerIsr;         /* function for handling BER timer interrupt */
   BAST_g2_FUNCT      gen1TimerIsr;        /* function for handling general timer 1 interrupt */
   BAST_g2_FUNCT      gen2TimerIsr;        /* function for handling general timer 2 interrupt */
   BAST_g2_FUNCT      xtalTimerIsr;        /* function for handling crystal timer interrupt */
   BAST_g2_FUNCT      passFunct;           /* general function pointer used in acquisition state machine */
   BAST_g2_FUNCT      failFunct;           /* general function pointer used in acquisition state machine */
   BAST_g2_FUNCT      postTuneFunct;       /* function to call after tuning */
   BAST_g2_FUNCT      postTunerInitFunct;  /* function to call after tuner initialization */
   BAST_g2_FUNCT      postTuneMixFunct;    /* function to call after tuner mix pll setting */
   BAST_g2_FUNCT      postSetFddfsFunct;   /* function to call after setting Fddfs */
   BAST_DiseqcStatus    diseqcStatus;        /* status of last diseqc transaction */
   BAST_Mode            actualMode;          /* actual mode found during code rate scan */
   BAST_BlindScanStatus blindScanStatus;     /* blind scan status */
   BAST_Bcm3445Settings bcm3445Settings;     /* configuration of BCM3445 controlled on this channel's MI2C */
   BAST_Mi2cChannel     bcm3445Mi2cChannel;  /* identifies which mi2c channel controls the BCM3445 associated with this channel's tuner */
   BAST_Bcm3445OutputChannel bcm3445TunerInput; /* output port of the BCM3445 feeding this channel's tuner */
   uint32_t             sampleFreq;          /* ADC sampling frequency in Hz */
   uint32_t             fecFreq;             /* FEC (Turbo/LDPC) clock frequency in Hz */
   uint32_t             upClk;               /* uP clock frequency in Hz */
   uint32_t             diseqcClk;           /* diseqc clock frequency in Hz */
   uint32_t             tunerFreq;           /* requested tuner frequency in Hz */
   uint32_t             actualTunerFreq;     /* actual tuner frequency in Hz */
   uint32_t             outputBitRate;       /* output transport bit rate in bps */
   uint32_t             tunerIfStepSize;     /* internal tuner IF step size in Hz used in freq scan algorithm */
   uint32_t             tunerFddfs;          /* internal tuner DDFS frequency in Hz */
   uint32_t             tunerFs;
   uint32_t             tunerLoDivider;      /* internal tuner LO divider scaled 2^1 */
   uint32_t             bfos;                /* saved baud rate control word */
   uint32_t             count1;              /* general counter used in acquisition */
   uint32_t             count2;              /* general counter used in acquisition */
   uint32_t             count3;              /* general counter used in acquisition */
   uint32_t             maxCount1;           /* general counter used in acquisition */  
   uint32_t             funct_state;         /* specifies current state in acquisition state machine */
   uint32_t             prev_state;          /* previous HP state */
   uint32_t             trackingAnactl4;     /* ANACTL4 tracking value */
   uint32_t             acqCount;            /* number of reacquisitions */
   uint32_t             maxReacq;            /* maximum number of reacquisitions */
   uint32_t             carrierAcqBw;        /* acquisition carrier loop bandwidth in Hz */
   uint32_t             carrierAcqDamp;      /* acquisition carrier loop damping factor scaled 2^2 */
   uint32_t             carrierTrkBw;        /* tracking carrier loop bandwidth in Hz */
   uint32_t             carrierTrkDamp;      /* tracking carrier loop damping factor scaled 2^2 */
   uint32_t             baudBw;              /* baud loop bandwidth in Hz */
   uint32_t             baudDamp;            /* baud loop damping factor scaled 2^2 */
   uint32_t             cmin;
   uint32_t             bmin;
   uint32_t             clctl;
   uint32_t             carrierDelay;        /* VerifyLock dwell time */
   uint32_t             mpegErrorCount;      /* accumulated MPEG errors */
   uint32_t             mpegFrameCount;      /* accumulated MPEG frames */
   uint32_t             berErrors;           /* accumulated BER error count */
   uint32_t             ldpcTotalBlocks;     /* accumulated total LDPC block count */
   uint32_t             ldpcCorrBlocks;      /* accumulated LDPC correctable block count */
   uint32_t             ldpcBadBlocks;       /* accumulated LDPC bad block count */
   uint32_t             rsCorr;              /* accumulated RS correctable errors */
   uint32_t             rsUncorr;            /* accumulated RS uncorrectable errors */
   uint32_t             preVitErrors;        /* accumulated pre-Viterbi error count */
   uint32_t             turboTotalBlocks;    /* accumulated Turbo block count */
   uint32_t             turboCorrBlocks;     /* accumulated Turbo correctable block count */
   uint32_t             turboBadBlocks;      /* accumulated Turbo bad block count */
   uint32_t             diseqcRRTOuSec;      /* diseqc receive reply time out */
   uint32_t             p_6db;               /* target power level in internal tuner calibration */
   uint32_t             lockFilterTime;      /* filter time for lock indication */
   uint32_t             agtctl;              /* saved value of AGTCTL */
   uint32_t             agictl;              /* saved value of AGICTL */
   uint32_t             tunerVco;            /* tuner VCO freq in Hz */   
   uint32_t             peakScanSymRateMin;  /* minimum symbol rate in peak scan */
   uint32_t             peakScanSymRateMax;  /* maximum symbol rate in peak scan */
   uint32_t             peakScanOutput;
   uint32_t             peakScanPower;
   uint32_t             acqTime;             /* acquisition time in usecs */
   uint32_t             freqDriftThreshold;  /* carrier freq drift threshold in Hz */
   uint32_t             rainFadeThreshold;   /* rain fade threshold in 1/2^24 dB */
   uint32_t             rainFadeSnrAve;      /* ave snr in 1/2^24 dB during rain fade window */
   uint32_t             rainFadeSnrMax;      /* max snr in 1/2^24 dB during rain fade window */
   uint32_t             rainFadeWindow;      /* time window for rain fade indicator in units of 100 msecs */
   int32_t              ratio1;
   uint32_t             ratio2;
   uint32_t             tunerGain;           /* output of tuner gain calibration, used in power estimation */
   uint32_t             ovrdselSave;         /* saved OVRDSEL register value */
   uint32_t             fullTuneFreq;        /* freq at which full tune was done */
   uint32_t             irqCount[BAST_Sds_MaxIntID]; /* interrupt counter */
   uint32_t             altPlcAcqBw;         /* Acquisition PLC BW override value */
   uint32_t             altPlcTrkBw;         /* Tracking PLC BW override value */   
   uint32_t             initCarrierDelay;
   uint32_t             checkLockInterval;
   uint32_t             symbolRateVerifyLock;/* used in VerifyLock algorithm */
   uint32_t             searchRangeFli;      /* VerifyLock search range */   
   int32_t              lockFilterRamp;      /* used in legacy mode lock status filtering */
   int32_t              extTunerIfOffset;    /* external tuner IF offset in Hz */  
   int32_t              tunerIfStep;         /* used in freq scan */
   int32_t              tunerIfStepMax;      /* used in freq scan */
   int32_t              dftFreqEstimate;     /* used in freq scan */
   int32_t              splitterModeAdj;     /* adjustment to carrier_error estimate in splitter mode */
   int32_t              initFreqOffset;      /* used in carrier freq drift threshold */
   int32_t              carrierErrorPli;     /* PLI component of carrier error */
   int32_t              freqTransferInt;     /* used in PLI-to-FLI freq leak */
   int32_t              tunerOffset;         /* internal tuner offset for dco avoidance */
   int32_t              frfStep;             
   int32_t              stepFreq;            /* used in VerifyLock algorithm */
   int32_t              sum0;                /* used in VerifyLock algorithm */
   int32_t              fli;                 /* used in VerifyLock algorithm */
   bool                 bHasDiseqc;          /* true if channel supports diseqc */
   bool                 bExternalTuner;      /* true if channel uses external tuner */
   bool                 bHasDedicatedPll;    /* true if channel has dedicated sample PLL */
   bool                 bHasTunerDacPll;     /* true if channel has internal tuner DAC PLL */
   bool                 bHasAfec;            /* true if channel supports DVB-S2 */
   bool                 bHasTfec;            /* true if channel supports Turbo */
   bool                 bVitLocked;          /* true if viterbi locked in legacy acquisition */
   bool                 bLocked;             /* true if demod and fec are locked */
   bool                 bMi2cInProgress;     /* true if MI2C is busy */
   bool                 bLastLocked;         /* filtered last lock status */
   bool                 bStatusInterrupts;   /* true if host wants PI to monitor status indicators */
   bool                 bFrfTransferred;     /* true if carrier offset already transferred to tuner LO PLL */
   bool                 bTunerCalFailed;     /* true if tuner LPF calibration failed */
   bool                 bTunerAdjImageFilter;/* true if tuner image filter needs to be adjusted */
   bool                 bSetLpf;             /* true if tuner LPF cal needs to be done after IMF adjustment */
   bool                 bFsNotDefault;       /* true if sample freq has changed */
   bool                 bForceReacq;         /* true if carrier error out of range */
   bool                 bDiseqcToneOn;       /* true if diseqc tone is enabled */
   uint16_t             xportCtl;            /* XPORT_CTL configuration parameter */
   uint16_t             diseqcCtl;           /* DISEQC_CTL configuration parameter */
   uint16_t             turboScanModes;      /* specifies which turbo modes to search in scan mode */
   uint16_t             turboScanCurrMode;   /* used in turbo scan mode */
   uint16_t             currentCnrThreshold1;/* current CNR threshold1 value being used */
   uint16_t             extTunerRfAgcTop;    /* external tuner RF AGC take over point */
   uint16_t             extTunerIfAgcTop;    /* external tuner IF AGC take over point*/
   uint16_t             intTuner3StageAgcTop;/* internal tuner 3-Stage IF AGC take over point*/
   uint16_t             mb0_i;               /* used for DCO convergence check */
   uint16_t             mb0_q;               /* used for DCO convergence check */
   uint16_t             mb1_i;               /* used for DCO convergence check */
   uint16_t             mb1_q;               /* used for DCO convergence check */
   uint16_t             diseqcBitThreshold;    /* threshold for diseqc rx bit detection */
   uint8_t              dftFreqEstimateStatus; /* used in DFT freq estimate algorithm */
   uint8_t              blindScanCurrMode;   /* used in blind acquisition */
   uint8_t              blindScanModes;      /* config parameter used to specify which modes to scan */
   uint8_t              ldpcScanState;       /* used in LDPC scan mode */
   uint8_t              turboScanState;      /* used in Turbo scan mode */
   uint8_t              ldpcCtl;             /* LDPC_CTL configuration parameter */
   uint8_t              tunerCutoff;         /* desired internal tuner cutoff frequency in MHz */
   uint8_t              tunerCutoffWide;     /* tuner filter wide setting during acquisition */
   uint8_t              tunerCtl;            /* TUNER_CTL configuration parameter */
   uint8_t              dtvScanState;        /* used for manual DTV 1/2 scan in DTV scan mode */
   uint8_t              diseqcPreTxDelay;
   uint8_t              diseqcSendLen;
   uint8_t              diseqcVsenseThreshHi; /* specifies high threshold for vsense interrupt */
   uint8_t              diseqcVsenseThreshLo; /* specifies low threshold for vsense interrupt */
   uint8_t              coresPoweredDown;
   uint8_t              lockIsrFlag;
   uint8_t              dtvScanCodeRates;    /* specifies which DTV code rates to search in scan mode */
   uint8_t              dvbScanCodeRates;    /* specifies which DVB code rates to search in scan mode */
   uint8_t              tuneDacDivRatio;     /* specifies internal tuner Fddfs */
   uint8_t              acqTimeState;
   uint8_t              statusEventIndicators;
   uint8_t              bcm3445Status;       /* see BAST_BCM3445_STATUS_* macros in bast.h */
   uint8_t              firstTimeLock;
   uint8_t              stuffBytes;          /* number of null stuff bytes */
   uint8_t              diseqcToneThreshold;   /* threshold for tone detect */
   uint8_t              diseqcSendBuffer[128];
   uint8_t              ldpcScramblingSeq[16]; /* XSEED+PLHDRCFGx sequence */
   uint8_t              bcm3445Ctl;            /* BCM3445_CTL configuration parameter */
   uint8_t              turboCtl;              /* TURBO_CTL configuration parameter */
   uint8_t              tunerBandgap;          /* TUNER_ANACTL9[4:2] setting */
   uint8_t              tunerGmx;              /* tuner image filter R value and corner freq adjust */
   uint8_t              tunerDacIctl;          /* tuner DAC bias control */
   uint8_t              tuneMixStatus;         /* tuner mixpll programming status */
   uint8_t              tuneMixState;
   uint8_t              tuneDacState;
   uint8_t              tuneMixLockStableCount;
   uint8_t              tunerGmxIdx;           /* tuner mixpll retry level */
   uint8_t              tunerLpfToCalibrate;   /* tuner LPF freq to calibrate */
   uint8_t              altPlcAcqDamp;         /* Acquisition PLC damping factor override value, scaled 2^3 */
   uint8_t              altPlcTrkDamp;         /* Tracking PLC damping factor override value, scaled 2^3 */
   uint8_t              noSyncCount;           /* number of times turbo RS sync not detected */
   uint8_t              sweep;                 /* used in legacy mode VerifyLock algorithm */
   uint8_t              maxSweep;              /* used in legacy mode VerifyLock algorithm */
   uint8_t              lockPoint;             /* used in legacy mode VerifyLock algorithm */  
   uint8_t              reacqCtl;              /* reacquisition options */   
   uint8_t              spinvState;            /* refer to BAST_SPINV_STATE_* macros */
} BAST_g2_P_ChannelHandle;


/******************************************************************************
BAST_g2_Ftm_P_InterruptCbTable
Summary: Interrupt Callback Table to describe interrupt Names, ISRs, and Masks 
*******************************************************************************/
typedef struct BAST_g2_Ftm_P_InterruptCbTable
{
	BINT_Id       	   IntrId;
	BINT_CallbackFunc pfCallback;
	int               iParam2; 
	bool              enable;
} BAST_g2_Ftm_P_InterruptCbTable ;


/* private BAST_g2 functions used in legacy, ldpc, and turbo modes */
BERR_Code BAST_g2_P_TuneAcquire1(BAST_ChannelHandle);
BERR_Code BAST_g2_P_DisableInterrupts(BAST_Handle h);
BERR_Code BAST_g2_P_ProcessScript(BAST_ChannelHandle hChn, uint32_t const *pScr);
BERR_Code BAST_g2_P_DisableChannelInterrupts(BAST_ChannelHandle h, bool bDisableDiseqc, bool bDisableMi2c);
BERR_Code BAST_g2_P_DisableChannelInterrupts_isr(BAST_ChannelHandle h, bool bDisableDiseqc, bool bDisableMi2c);
void BAST_g2_P_DiseqcDone_isr(void *p, int param);
void BAST_g2_P_DiseqcOverVoltage_isr(void *p, int param);
void BAST_g2_P_DiseqcUnderVoltage_isr(void *p, int param);
void BAST_g2_P_LdpcHp_isr(void *p, int param);
void BAST_g2_P_Agc_isr(void *p, int param);
void BAST_g2_P_QpskLock_isr(void *p, int param);
void BAST_g2_P_QpskNotLock_isr(void *p, int param);
void BAST_g2_P_LdpcLock_isr(void *p, int param);
void BAST_g2_P_LdpcNotLock_isr(void *p, int param);
#ifndef BAST_EXCLUDE_TURBO
void BAST_g2_P_TurboLock_isr(void *p, int param);
void BAST_g2_P_TurboNotLock_isr(void *p, int param);
#endif
void BAST_g2_P_BaudTimer_isr(void *p, int param);
void BAST_g2_P_BerTimer_isr(void *p, int param);
void BAST_g2_P_Gen1Timer_isr(void *p, int param);
void BAST_g2_P_Gen2Timer_isr(void *p, int param);
void BAST_g2_P_XtalTimer_isr(void *p, int param);
void BAST_g2_P_3StageAgc_isr(void *p, int param);
BERR_Code BAST_g2_P_DisableTimer(BAST_ChannelHandle h, BAST_TimerSelect t);
void BAST_g2_P_Enable3StageAgc(BAST_ChannelHandle h, bool bEnable);
void BAST_g2_P_DisableDiseqcInterrupts_isr(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_EnableTimer(BAST_ChannelHandle h, BAST_TimerSelect t, uint32_t delay, BAST_g2_FUNCT func);
uint32_t BAST_g2_P_GCF(uint32_t m, uint32_t n);
void BAST_g2_P_IndicateLocked(BAST_ChannelHandle h);
void BAST_g2_P_IndicateNotLocked(BAST_ChannelHandle h);
void BAST_g2_P_SetSampleFreq(BAST_ChannelHandle h, uint32_t p1, uint32_t p2, uint32_t ndiv, uint32_t m1div, uint32_t m2div, uint32_t m3div);
void BAST_g2_P_GetSampleFreq(BAST_ChannelHandle h);
void BAST_g2_P_GetFecFreq(BAST_ChannelHandle h);
void BAST_g2_P_GetDiseqcClock(BAST_ChannelHandle h);
void BAST_g2_P_SetDiseqcClock(BAST_ChannelHandle h);
void BAST_g2_P_ReadModifyWriteRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t and_mask, uint32_t or_mask);
void BAST_g2_P_OrRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t or_mask);
void BAST_g2_P_AndRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t and_mask);
void BAST_g2_P_SetNyquistAlpha(BAST_ChannelHandle h);
void BAST_g2_P_SetSymbolRate(BAST_ChannelHandle h);
void BAST_g2_P_InitBert(BAST_ChannelHandle h);
void BAST_g2_P_StartBert(BAST_ChannelHandle h);
void BAST_g2_P_SetCarrierBw(BAST_ChannelHandle h, uint32_t bw, uint32_t damp);
void BAST_g2_P_SetBaudBw(BAST_ChannelHandle h, uint32_t bw, uint32_t damp);
BERR_Code BAST_g2_P_SetOpll(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerQuickTune(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct);
bool BAST_g2_P_LdpcIsPilotLoopless(BAST_ChannelHandle h);
void BAST_g2_P_TunerGetLockStatus(BAST_ChannelHandle h, bool *bDacLocked, bool *bMixerLocked);
BERR_Code BAST_g2_P_TunerSetFreq(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct);
void BAST_g2_P_TunerInit(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct);
void BAST_g2_P_InitConfig(BAST_ChannelHandle h);
void BAST_g2_P_InitChannelHandle(BAST_ChannelHandle h);
void BAST_g2_P_InitChannels(BAST_ChannelHandle h, bool);
BERR_Code BAST_g2_P_Acquire(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_BlindAcquire(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_BlindAcquire1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskAcquire(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcAcquire(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TurboAcquire(BAST_ChannelHandle h);
#ifndef BAST_EXCLUDE_TURBO
BERR_Code BAST_g2_P_TurboSetOpll(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TurboSetTssq(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TurboSetTtur(BAST_ChannelHandle h);
void BAST_g2_P_TurboUpdateErrorCounters(BAST_ChannelHandle h);
#endif
void BAST_g2_P_LdpcSetPlc(BAST_ChannelHandle h, bool bAcq);
void BAST_g2_P_LdpcSetFinalBaudLoopBw(BAST_ChannelHandle h);
void BAST_g2_P_SetAthr(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerTestMode(BAST_ChannelHandle h);
void BAST_g2_P_SetEqffe3(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_DoDftFreqEstimate(BAST_ChannelHandle h, BAST_g2_FUNCT funct);
bool BAST_g2_P_OkToEnableLoTracking(BAST_ChannelHandle h);
#ifndef BAST_ENABLE_SKIT_FSK
BERR_Code BAST_g2_Ftm_P_InitDevice(BAST_Handle);
BERR_Code BAST_g2_Ftm_P_EnableFtmInterrupts(BAST_Handle h, bool bEnable);
void BAST_g2_Ftm_P_HandleInterrupt_isr(void *p, int param);
#else
#define BAST_g2_Ftm_P_InitDevice(h) BAST_g2_Fsk_P_InitDevice(h)
#define BAST_g2_Ftm_P_EnableFtmInterrupts(h, bEnable) BAST_g2_Fsk_P_EnableFskInterrupts(h, bEnable)
#define BAST_g2_Ftm_P_HandleInterrupt_isr BAST_g2_Fsk_P_HandleInterrupt_isr
void BAST_g2_Fsk_P_HandleInterrupt_isr(void *p, int param);
#endif
BAST_Mode BAST_g2_P_GetActualMode(BAST_ChannelHandle h);
int32_t BAST_g2_P_GetSymbolRateError(BAST_ChannelHandle h);
int32_t BAST_g2_P_GetCarrierError(BAST_ChannelHandle h);
bool BAST_g2_P_CarrierOffsetOutOfRange(BAST_ChannelHandle h);
void BAST_g2_P_InitHandle(BAST_Handle h);
void BAST_g2_P_InitHandleDefault(BAST_Handle h);
void BAST_g2_P_Mi2c_isr(void *p, int param);
BERR_Code BAST_g2_P_GetSnrEstimate(BAST_ChannelHandle h, uint32_t *pSnr);
void BAST_g2_P_TunerSetDigctl7(BAST_ChannelHandle h, uint32_t or_mask);
void BAST_g2_P_FreezeLoops(BAST_ChannelHandle h);
void BAST_g2_P_UnfreezeLoops(BAST_ChannelHandle h);
void BAST_g2_P_ResyncBert(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcReacquire(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcAcquire6(BAST_ChannelHandle h);
void BAST_g2_P_SetDefaultSampleFreq(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_QpskAcquire0(BAST_ChannelHandle h);
void BAST_g2_P_ResetAcquisitionTimer(BAST_ChannelHandle h);
void BAST_g2_P_UpdateAcquisitionTime(BAST_ChannelHandle h);
void BAST_g2_P_TransferFreqOffset(BAST_ChannelHandle h);
void BAST_g2_P_LdpcUpdateBlockCounters(BAST_ChannelHandle h);
#ifndef BAST_EXCLUDE_STATUS_EVENTS
void BAST_g2_P_CheckStatusIndicators(BAST_ChannelHandle h);
void BAST_g2_P_ResetStatusIndicators(BAST_ChannelHandle h);
#endif
#ifndef BAST_EXCLUDE_SPLITTER_MODE
void BAST_g2_P_UpdateSplitterModeAdj(BAST_ChannelHandle h);
#endif
bool BAST_g2_P_IsLdpcOn(BAST_ChannelHandle h);
bool BAST_g2_P_IsTurboOn(BAST_ChannelHandle h);
void BAST_g2_P_UpdateErrorCounters(BAST_ChannelHandle h);
void BAST_g2_P_QpskUpdateErrorCounters(BAST_ChannelHandle h);
void BAST_g2_P_ResetErrorCounters(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcLockStable(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_KillTransport(BAST_ChannelHandle h);
bool BAST_g2_P_TunerMixpllLostLock(BAST_ChannelHandle h);
void BAST_g2_P_IncrementInterruptCounter(BAST_ChannelHandle h, BAST_Sds_IntID idx);
void BAST_g2_P_IfAgcLeThreshRise_isr(void *p, int param);
void BAST_g2_P_IfAgcLeThreshFall_isr(void *p, int param);
void BAST_g2_P_RfAgcLeThreshRise_isr(void *p, int param);
void BAST_g2_P_RfAgcLeThreshFall_isr(void *p, int param);
void BAST_g2_P_RfAgcGtMaxChange_isr(void *p, int param);
bool BAST_g2_P_LdpcIsPilotOn(BAST_ChannelHandle h);
void BAST_g2_P_SetBackCarrierBw(BAST_ChannelHandle h, uint32_t bw, uint32_t damp);
BERR_Code BAST_g2_P_BlindReacquire(BAST_ChannelHandle h);
void BAST_g2_P_GetFlifOffset(BAST_ChannelHandle h, int32_t *pOffset);
void BAST_g2_P_SetFlifOffset(BAST_ChannelHandle h, int32_t offset);
void BAST_g2_P_LdpcConfig(BAST_ChannelHandle h, uint32_t reg, const uint32_t *pVal);
void BAST_g2_P_TurboSetOnfn(BAST_ChannelHandle h);
void BAST_g2_P_TunerRampMb2(BAST_ChannelHandle h, bool bPos, uint32_t len);

/* G2 implementation of API functions */
BERR_Code BAST_g2_P_Open(BAST_Handle *, BCHP_Handle, void*, BINT_Handle, const BAST_Settings *pDefSettings);
BERR_Code BAST_g2_P_Close(BAST_Handle);
BERR_Code BAST_g2_P_GetTotalChannels(BAST_Handle, uint32_t *);
BERR_Code BAST_g2_P_OpenChannel(BAST_Handle, BAST_ChannelHandle *, uint32_t chnNo, const BAST_ChannelSettings *pChnDefSettings);
BERR_Code BAST_g2_P_CloseChannel(BAST_ChannelHandle);
BERR_Code BAST_g2_P_GetDevice(BAST_ChannelHandle, BAST_Handle *);
BERR_Code BAST_g2_P_InitAp(BAST_Handle, const uint8_t *);
BERR_Code BAST_g2_P_SoftReset(BAST_Handle);
BERR_Code BAST_g2_P_ResetChannel(BAST_ChannelHandle, bool);
BERR_Code BAST_g2_P_GetApStatus(BAST_Handle, BAST_ApStatus *);
BERR_Code BAST_g2_P_GetApVersion(BAST_Handle, uint16_t*, uint8_t*, uint32_t*, uint8_t*, uint8_t*);
BERR_Code BAST_g2_P_TuneAcquire(BAST_ChannelHandle, const uint32_t, const BAST_AcqSettings *);
BERR_Code BAST_g2_P_GetChannelStatus(BAST_ChannelHandle, BAST_ChannelStatus *);
BERR_Code BAST_g2_P_GetLockStatus(BAST_ChannelHandle, bool *);
BERR_Code BAST_g2_P_ResetStatus(BAST_ChannelHandle);
BERR_Code BAST_g2_P_SetDiseqcTone(BAST_ChannelHandle, bool);
BERR_Code BAST_g2_P_GetDiseqcTone(BAST_ChannelHandle, bool*);
BERR_Code BAST_g2_P_SetDiseqcVoltage(BAST_ChannelHandle, bool);
BERR_Code BAST_g2_P_GetDiseqcVoltage(BAST_ChannelHandle, uint8_t*);
BERR_Code BAST_g2_P_EnableDiseqcLnb(BAST_ChannelHandle, bool);
BERR_Code BAST_g2_P_EnableVsenseInterrupts(BAST_ChannelHandle, bool);
BERR_Code BAST_g2_P_SendACW(BAST_ChannelHandle, uint8_t);
BERR_Code BAST_g2_P_SendDiseqcCommand(BAST_ChannelHandle, const uint8_t *pSendBuf, uint8_t sendBufLen);
BERR_Code BAST_g2_P_GetDiseqcStatus(BAST_ChannelHandle, BAST_DiseqcStatus *pStatus);
BERR_Code BAST_g2_P_ResetDiseqc(BAST_ChannelHandle, uint8_t options);
BERR_Code BAST_g2_P_ResetFtm(BAST_Handle);
BERR_Code BAST_g2_P_ReadFtm(BAST_Handle, uint8_t *pBuf, uint8_t *n);
BERR_Code BAST_g2_P_WriteFtm(BAST_Handle, uint8_t *pBuf, uint8_t n);
BERR_Code BAST_g2_P_PowerDownFtm(BAST_Handle);
BERR_Code BAST_g2_P_PowerUpFtm(BAST_Handle);
BERR_Code BAST_g2_P_WriteMi2c(BAST_ChannelHandle, uint8_t, uint8_t*, uint8_t);
BERR_Code BAST_g2_P_ReadMi2c(BAST_ChannelHandle, uint8_t, uint8_t*, uint8_t, uint8_t *, uint8_t);
BERR_Code BAST_g2_P_GetSoftDecisionBuf(BAST_ChannelHandle, int16_t*, int16_t*);
BERR_Code BAST_g2_P_ReadAgc(BAST_ChannelHandle, BAST_Agc, uint32_t*);
BERR_Code BAST_g2_P_WriteAgc(BAST_ChannelHandle, BAST_Agc, uint32_t*);
BERR_Code BAST_g2_P_FreezeAgc(BAST_ChannelHandle, BAST_Agc, bool);
BERR_Code BAST_g2_P_FreezeEq(BAST_ChannelHandle, bool);
BERR_Code BAST_g2_P_PowerDown(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_g2_P_PowerUp(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_g2_P_ReadRegister(BAST_ChannelHandle, uint32_t, uint32_t*);
BERR_Code BAST_g2_P_WriteRegister(BAST_ChannelHandle, uint32_t, uint32_t*);
BERR_Code BAST_g2_P_ReadConfig(BAST_ChannelHandle, uint16_t, uint8_t*, uint8_t);
BERR_Code BAST_g2_P_WriteConfig(BAST_ChannelHandle, uint16_t, uint8_t*, uint8_t);
BERR_Code BAST_g2_P_GetLockChangeEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
BERR_Code BAST_g2_P_GetFtmEventHandle(BAST_Handle, BKNI_EventHandle*);
BERR_Code BAST_g2_P_GetDiseqcEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
BERR_Code BAST_g2_P_GetDiseqcVsenseEventHandles(BAST_ChannelHandle, BKNI_EventHandle*, BKNI_EventHandle*);
BERR_Code BAST_g2_P_AbortAcq(BAST_ChannelHandle);
BERR_Code BAST_g2_P_ConfigLna(BAST_Handle, BAST_LnaIntConfig int_config, BAST_LnaExtConfig ext_config);
BERR_Code BAST_g2_P_GetLnaStatus(BAST_Handle, BAST_LnaStatus *pStatus);
BERR_Code BAST_g2_P_LnaSetNotchFilter(BAST_ChannelHandle h, bool bEnable);
BERR_Code BAST_g2_P_PeakScan(BAST_ChannelHandle h, uint32_t tunerFreq);
BERR_Code BAST_g2_P_GetPeakScanStatus(BAST_ChannelHandle h, BAST_PeakScanStatus*);
BERR_Code BAST_g2_P_GetPeakScanEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
BERR_Code BAST_g2_P_EnableStatusInterrupts(BAST_ChannelHandle, bool);
BERR_Code BAST_g2_P_GetStatusEventHandle(BAST_ChannelHandle h, BKNI_EventHandle *pEvent);
BERR_Code BAST_g2_P_ConfigBcm3445(BAST_Handle, BAST_Bcm3445Settings*);
BERR_Code BAST_g2_P_MapBcm3445ToTuner(BAST_ChannelHandle, BAST_Mi2cChannel, BAST_Bcm3445OutputChannel);
BERR_Code BAST_g2_P_GetBcm3445Status(BAST_ChannelHandle, BAST_Bcm3445Status*);
BERR_Code BAST_g2_P_SetSearchRange(BAST_Handle, uint32_t);
BERR_Code BAST_g2_P_GetSearchRange(BAST_Handle, uint32_t*);
BERR_Code BAST_g2_P_SetAmcScramblingSeq(BAST_ChannelHandle, uint32_t xseed, uint32_t plhdrscr1, uint32_t plhdrscr2, uint32_t plhdrscr3);
BERR_Code BAST_g2_P_SetTunerFilter(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_g2_P_SetOutputTransportSettings(BAST_ChannelHandle, BAST_OutputTransportSettings*);
BERR_Code BAST_g2_P_GetOutputTransportSettings(BAST_ChannelHandle, BAST_OutputTransportSettings*);
BERR_Code BAST_g2_P_SetDiseqcSettings(BAST_ChannelHandle, BAST_DiseqcSettings*);
BERR_Code BAST_g2_P_GetDiseqcSettings(BAST_ChannelHandle, BAST_DiseqcSettings*);
BERR_Code BAST_g2_P_SetNetworkSpec(BAST_Handle, BAST_NetworkSpec);
BERR_Code BAST_g2_P_GetNetworkSpec(BAST_Handle, BAST_NetworkSpec*);
BERR_Code BAST_g2_P_SetFskChannel(BAST_Handle h, BAST_FskChannel fskTxChannel, BAST_FskChannel fskRxChannel);
BERR_Code BAST_g2_P_GetFskChannel(BAST_Handle h, BAST_FskChannel *fskTxChannel, BAST_FskChannel *fskRxChannel);
BERR_Code BAST_g2_P_SetPeakScanSymbolRateRange(BAST_ChannelHandle h, uint32_t minSymbolRate, uint32_t maxSymbolRate);
BERR_Code BAST_g2_P_GetPeakScanSymbolRateRange(BAST_ChannelHandle h, uint32_t *pMinSymbolRate, uint32_t *pMaxSymbolRate);
BERR_Code BAST_g2_P_GetVersionInfo(BAST_Handle h, FEC_DeviceVersionInfo *p);

#ifdef __cplusplus
}
#endif

#endif /* BAST_G2_PRIV_H__ */

