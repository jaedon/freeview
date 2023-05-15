/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_acq.c $
 * $brcm_Revision: Hydra_Software_Devel/212 $
 * $brcm_Date: 2/7/13 11:26a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_acq.c $
 * 
 * Hydra_Software_Devel/212   2/7/13 11:26a enavarro
 * SWSATFE-174: clear opll bypass bits if xportSettings.bOpllBypass is
 * false
 * 
 * Hydra_Software_Devel/211   2/6/13 1:52p enavarro
 * SWSATFE-174: removed BAST_g3_P_ResyncBert() since it is never used
 * 
 * Hydra_Software_Devel/210   1/24/13 5:05p ckpark
 * SWSATFE-174: set bit 17 CLCTL1; changed leak bandwidth for turbo in
 * BAST_g3_P_LeakPliToFli()
 * 
 * Hydra_Software_Devel/209   12/21/12 10:23a ronchan
 * SWSATFE-174: changed false lock warning to message
 * 
 * Hydra_Software_Devel/208   12/18/12 10:42a enavarro
 * SWSATFE-174: check for false lock in BAST_g3_P_Lock_isr()
 * 
 * Hydra_Software_Devel/207   12/12/12 3:14p enavarro
 * SWSATFE-234: added dynamic power down of SDS_OPLL
 * 
 * Hydra_Software_Devel/206   12/12/12 9:43a enavarro
 * SWSATFE-174: set FE_ADCPCTL[4:3]='00' for 4538.
 * 
 * Hydra_Software_Devel/205   11/20/12 10:04a enavarro
 * SWSATFE-174: fixed bug in BAST_g3_P_IsCarrierOutOfRange() on 4538
 * 
 * Hydra_Software_Devel/204   11/5/12 4:58p enavarro
 * SWSATFE-174: reset DAFE loop in initialization
 * 
 * Hydra_Software_Devel/203   11/5/12 3:25p ronchan
 * SWSATFE-174: change power management for 4538
 * 
 * Hydra_Software_Devel/202   11/2/12 10:59a enavarro
 * SWSATFE-174: added check for NULL handle
 * 
 * Hydra_Software_Devel/201   10/29/12 1:38p ckpark
 * SWSATFE-225: allow some time for mpeg to regain lock if lost lock
 * 
 * Hydra_Software_Devel/200   10/23/12 4:20p enavarro
 * SWSATFE-174: put BFOS programming in separate function for 4538 dft low
 * baud rate workaround
 * 
 * Hydra_Software_Devel/199   10/22/12 4:32p enavarro
 * SWSATFE-174: removed unused functions/variables when BAST_HAS_WFE is
 * defined
 * 
 * Hydra_Software_Devel/198   10/22/12 11:18a enavarro
 * SWSATFE-221: fix dft for low baud rate
 * 
 * Hydra_Software_Devel/197   10/1/12 11:22a enavarro
 * SWSATFE-217: return BAST_ERR_SAMPLE_FREQ if spll not locked; verify
 * sampleFreq is reasonably close after calling
 * BAST_g3_P_SetSampleFreq(); implement retry when BAST_g3_P_SmartTune()
 * fails
 * 
 * Hydra_Software_Devel/196   9/20/12 9:08a enavarro
 * SWSATFE-174: updated CLCTL1 for 4538
 * 
 * Hydra_Software_Devel/195   9/13/12 8:48a enavarro
 * SWSATFE-174: fixed compiler warning when BAST_EXCLUDE_EXT_TUNER is
 * defined
 * 
 * Hydra_Software_Devel/194   9/11/12 11:12a enavarro
 * SWSATFE-174: added BAST_EXCLUDE_EXT_TUNER build option
 * 
 * Hydra_Software_Devel/193   9/10/12 4:03p enavarro
 * SWSATFE-174: retain default setting for bits 19:16 in SDS_CL_CLCTL1
 * 
 * Hydra_Software_Devel/192   8/30/12 2:19p ronchan
 * SWSATFE-207: initialize config parameters for tuner agc window length,
 * amplitude threshold, and loop coefficient
 * 
 * Hydra_Software_Devel/191   8/28/12 2:28p enavarro
 * SWSATFE-140: removed BAST_g3_P_UpdateCWC()
 * 
 * Hydra_Software_Devel/190   8/13/12 10:20a ronchan
 * SWSATFE-213: exit critical section if error condition occurs
 * 
 * Hydra_Software_Devel/189   8/2/12 10:18a ckpark
 * SWSATFE-161: clear bert reset
 * 
 * Hydra_Software_Devel/188   7/26/12 4:48p enavarro
 * SW7346-942: not necessary to call BAST_g3_P_AbortAcq() at end of tuner
 * test mode
 * 
 * Hydra_Software_Devel/187   7/18/12 3:26p ronchan
 * SWSATFE-207: initialize daisy gain config, adjust agc thresh config
 * initial value
 * 
 * Hydra_Software_Devel/186   7/13/12 10:13a enavarro
 * SWSATFE-140: keep pll_ctrl_msb field when programming
 * SDS_OI_OPLL_MDIV_CTRL
 * 
 * Hydra_Software_Devel/185   7/5/12 11:41a ronchan
 * SWSATFE-205: enter InitChannels in isr context instead
 * 
 * Hydra_Software_Devel/184   7/3/12 5:08p enavarro
 * SWSATFE-205: put critical section around call to BAST_g3_P_HpEnable()
 * in BAST_g3_P_PrepareNewAcquisition()
 * 
 * Hydra_Software_Devel/183   6/27/12 3:08p ckpark
 * SWSATFE-204: fixed bug in BAST_g3_P_UpdateErrorCounters()
 * 
 * Hydra_Software_Devel/182   5/23/12 4:57p ckpark
 * SWSATFE-197: added debug message
 * 
 * Hydra_Software_Devel/181   3/22/12 2:05p enavarro
 * SWSATFE-184: initialize tuner_rfagc_thresh config parameter
 * 
 * Hydra_Software_Devel/180   3/21/12 4:25p enavarro
 * SWSATFE-181: use BAST_g3_P_HpGetFreqOffsetEstimate() in
 * BAST_g3_P_GetCarrierError()
 * 
 * Hydra_Software_Devel/179   3/21/12 2:37p enavarro
 * SWSATFE-181: fixed fli/pli freq leak for turbo
 * 
 * Hydra_Software_Devel/178   3/21/12 2:28p enavarro
 * SWSATFE-140: added ability to verify timing loop is locked via config
 * param bit; added option in reacqCtl config param to preserve h/w state
 * when acquisition fails and auto reacq is disabled
 * 
 * Hydra_Software_Devel/177   3/20/12 10:17a enavarro
 * SWSATFE-176: added BAST_g3_P_GetCarrierErrorPli() and
 * BAST_g3_P_GetCarrierErrorFli(); fixed carrier error estimation for
 * turbo mode
 * 
 * Hydra_Software_Devel/176   3/19/12 4:42p ckpark
 * SWSATFE-181:PLI to FLI carrier error leak is introduced for Turbo modes
 * 
 * Hydra_Software_Devel/175   3/19/12 3:36p enavarro
 * SWSATFE-140: updated snr_threshold values for DVB-S and turbo
 * 
 * Hydra_Software_Devel/174   3/16/12 4:20p enavarro
 * SWSATFE-140: added reacqCause to channel handle struct
 * 
 * Hydra_Software_Devel/173   3/15/12 11:28a enavarro
 * SWSATFE-140: fixed acqCount for turbo scan
 * 
 * Hydra_Software_Devel/172   3/5/12 3:03p enavarro
 * SWSATFE-140: changed maxStableLockTimeout
 * 
 * Hydra_Software_Devel/171   2/28/12 3:51p enavarro
 * SWSATFE-177: initialize acmMaxMode; added register settings for ACM
 * 
 * Hydra_Software_Devel/170   2/20/12 10:01a ckpark
 * SWSATFE-140: added delay after configuring the AGC
 * 
 * Hydra_Software_Devel/169   2/9/12 3:12p enavarro
 * SWSATFE-140: reworked acquisition timer
 * 
 * Hydra_Software_Devel/168   2/1/12 3:56p enavarro
 * SWSATFE-140: call BAST_g3_P_InitChannels() from BAST_ResetChannel()
 * 
 * Hydra_Software_Devel/167   1/31/12 2:39p ckpark
 * SWSATFE-140: set TPCTL2 for non-4517; increment timeSinceStableLock
 * after onMonitorLockFunct processing
 * 
 * Hydra_Software_Devel/166   1/27/12 2:24p enavarro
 * SWSATFE-140: set tracking PLC in the onStableLockFunct()
 * 
 * Hydra_Software_Devel/165   1/24/12 4:24p enavarro
 * SWSATFE-140: use default value for TPCTL2
 * 
 * Hydra_Software_Devel/164   1/17/12 4:31p enavarro
 * SWSATFE-162: fixed turbo_scan with spinv
 * 
 * Hydra_Software_Devel/163   1/17/12 11:00a enavarro
 * SWSATFE-86: fixed compiler warnings for turbo-disabled chips
 * 
 * Hydra_Software_Devel/162   1/13/12 10:49a enavarro
 * SWSATFE-140: set default reacqCtl config param
 * 
 * Hydra_Software_Devel/161   1/12/12 3:43p enavarro
 * SWSATFE-162: changed debug message level in
 * BAST_g3_P_IsCarrierOffsetOutOfRange()
 * 
 * Hydra_Software_Devel/160   1/12/12 10:59a enavarro
 * SWSATFE-140: fixed carrier error debug output in
 * BAST_g3_P_IsCarrierOffsetOutOfRange(); include LDPC modes in
 * blindScanModes only if AFEC is available
 * 
 * Hydra_Software_Devel/159   1/11/12 11:04a ckpark
 * SWSATFE-162: fixed carrier error calculation for turbo
 * 
 * Hydra_Software_Devel/158   1/6/12 9:52a ckpark
 * SWSATFE-162: set tracking plc on stable lock
 * 
 * Hydra_Software_Devel/157   1/6/12 9:32a enavarro
 * SWSATFE-140: initialize searchRange
 * 
 * Hydra_Software_Devel/156   1/4/12 10:17a ckpark
 * SWSATFE-162: initialize bTurboSpinv based on acq_ctl settings
 * 
 * Hydra_Software_Devel/155   1/4/12 9:33a ckpark
 * SWSATFE-162: initialized debug1 and debug2
 * 
 * Hydra_Software_Devel/154   12/28/11 9:39a enavarro
 * SWSATFE-166: dynamically turn on/off tfec clock as needed
 * 
 * Hydra_Software_Devel/153   12/27/11 2:27p enavarro
 * SWSATFE-166: indicate not locked on reacquire; reacquire if HP not
 * locked during MonitorLock
 * 
 * Hydra_Software_Devel/152   12/27/11 11:08a enavarro
 * SWSATFE-166: initialize bTurboSpinv at start of acquisition
 * 
 * Hydra_Software_Devel/151   12/16/11 10:51a enavarro
 * SWSATFE-162: keep TFEC clock always on for now
 * 
 * Hydra_Software_Devel/150   12/15/11 4:54p ckpark
 * SWSATFE-166: fixed BAST_g3_P_GetCarrierError() in turbo spectral
 * inversion; set CL_FLLC1 equal to CL_FLLC; set CL_FLIC1 equal to
 * CL_FLIC1
 * 
 * Hydra_Software_Devel/149   12/13/11 3:04p ckpark
 * SWSATFE-164: reinitialize eq taps at start of acquisition
 * 
 * Hydra_Software_Devel/148   12/12/11 4:59p ronchan
 * SW7346-590: fixed initial unlock when acquiring after lock
 * 
 * Hydra_Software_Devel/147   12/7/11 5:07p enavarro
 * SWSATFE-157: fix compiler warning
 * 
 * Hydra_Software_Devel/146   12/7/11 3:43p ckpark
 * SWSATFE-157: adjusted filtctl for low baud rate
 * 
 * Hydra_Software_Devel/145   12/5/11 3:09p ckpark
 * SWSATFE-157: update filtctl
 * 
 * Hydra_Software_Devel/144   11/30/11 2:46p enavarro
 * SWSATFE-157: added BAST_g3_P_GetRegisterWriteWaitTime()
 * 
 * Hydra_Software_Devel/143   11/15/11 2:33p ckpark
 * SWSATFE-161: reset BERT once when bert lost lock; fixed BERCTL when
 * BERT not enabled
 * 
 * Hydra_Software_Devel/142   11/10/11 10:57a guangcai
 * SWSATFE-158: changed filtctl for low baud rates
 * 
 * Hydra_Software_Devel/141   10/17/11 3:36p enavarro
 * SWSATFE-140: fixed 4517 compile error
 * 
 * Hydra_Software_Devel/140   10/17/11 9:08a enavarro
 * SWSATFE-150: fixed compiler warning
 * 
 * Hydra_Software_Devel/139   10/17/11 9:05a enavarro
 * SWSATFE-150: enable spur canceller on stable lock
 * 
 * Hydra_Software_Devel/138   10/13/11 3:01p enavarro
 * SWSATFE-150: updated BAST_g3_P_InitCW()
 * 
 * Hydra_Software_Devel/137   10/10/11 3:10p enavarro
 * SWSATFE-150: added BAST_g3_P_ResetCWC()
 * 
 * Hydra_Software_Devel/136   10/7/11 4:43p enavarro
 * SWSATFE-150: added CWC support
 * 
 * Hydra_Software_Devel/135   10/6/11 9:17a enavarro
 * SWSATFE-150: reworked spur canceller to use CWC instead of notch
 * filters
 * 
 * Hydra_Software_Devel/134   9/29/11 10:43a ckpark
 * SWSATFE-137: reset SNORE 300 msecs after stable lock
 * 
 * Hydra_Software_Devel/133   9/29/11 9:33a ckpark
 * SWSATFE-137: reset SNORE on stable lock
 * 
 * Hydra_Software_Devel/132   9/28/11 4:19p enavarro
 * SWSATFE-148: made changes for TUNER_CTL_PRESERVE_COMMANDED_FREQ
 * 
 * Hydra_Software_Devel/131   9/28/11 2:34p enavarro
 * SWSATFE-86: set default bMonitorLock to true
 * 
 * Hydra_Software_Devel/130   9/28/11 2:30p enavarro
 * SWSATFE-86: added bMonitorLock in channel handle struct
 * 
 * Hydra_Software_Devel/129   9/20/11 4:49p enavarro
 * SWSATFE-86: removed debug msgs
 * 
 * Hydra_Software_Devel/127   9/16/11 9:57a ronchan
 * SWSATFE-139: init ambient temp to zero to disable Vc adjustment
 * 
 * Hydra_Software_Devel/126   9/15/11 2:24p ronchan
 * SWSATFE-139: initialize Vc thresholds
 * 
 * Hydra_Software_Devel/125   9/13/11 5:42p ronchan
 * SWSATFE-139: initialize kvco calibration parameters
 * 
 * Hydra_Software_Devel/124   8/24/11 2:54p enavarro
 * SWSATFE-140: add support for BAST_EXCLUDE_LDPC
 * 
 * Hydra_Software_Devel/123   8/24/11 10:55a enavarro
 * SWSATFE-140: fixed compiler errors when BAST_EXCLUDE_LDPC is defined
 * 
 * Hydra_Software_Devel/122   8/18/11 5:13p enavarro
 * SWSATFE-140: add build option to exclude ftm
 * 
 * Hydra_Software_Devel/121   8/12/11 10:43a enavarro
 * SWSATFE-140: add build option to exclude ldpc code from ast pi
 * 
 * Hydra_Software_Devel/120   8/11/11 2:00p enavarro
 * SWSATFE-86: add build options to exclude bcm3445 and mi2c support
 * 
 * Hydra_Software_Devel/119   8/10/11 11:06a ronchan
 * SWSATFE-139: renamed tuner linear search range
 * 
 * Hydra_Software_Devel/118   8/10/11 10:57a enavarro
 * SWSATFE-86: reworked snr filtering
 * 
 * Hydra_Software_Devel/117   8/10/11 10:06a ronchan
 * SWSATFE-139: initialize tuner linear search range
 * 
 * Hydra_Software_Devel/116   7/29/11 1:18p enavarro
 * SWSATFE-86: fixed compiler warning when building BCM4528
 * 
 * Hydra_Software_Devel/115   7/29/11 11:11a guangcai
 * SWSATFE-137: implemented SNORE filtering; enabled SNORE for all g3
 * chips
 * 
 * Hydra_Software_Devel/114   7/29/11 11:01a ronchan
 * SW7344-117: check for spll lock status
 * 
 * Hydra_Software_Devel/113   7/27/11 11:34a guangcai
 * SWSATFE-136: updates for low baud rate acquisition
 * 
 * Hydra_Software_Devel/112   7/19/11 11:50a enavarro
 * SW7346-331: fixed unchecked return values
 * 
 * Hydra_Software_Devel/111   7/13/11 2:26p enavarro
 * SWSATFE-86: do nothing for BAST_g3_P_SetDefaultSampleFreq() when WFE is
 * present
 * 
 * Hydra_Software_Devel/110   7/8/11 10:14a ronchan
 * SWSATFE-75: added timer select parameter for running dco loop
 * 
 * Hydra_Software_Devel/109   7/7/11 5:14p ronchan
 * SWSATFE-75: disable dco run for now in monitor lock
 * 
 * Hydra_Software_Devel/108   7/7/11 10:44a ronchan
 * SWSATFE-75: added runtime parameter for dco loop
 * 
 * Hydra_Software_Devel/107   7/6/11 11:22a ronchan
 * SWSATFE-75: run and freeze dco periodically for 7358
 * 
 * Hydra_Software_Devel/106   6/6/11 2:20p enavarro
 * SWSATFE-86: updates for undersample mode; removed BAST_g3_P_SetEqffe3()
 * since it is same as BAST_g3_P_SetFfeMainTap()
 * 
 * Hydra_Software_Devel/105   6/3/11 4:27p enavarro
 * SWSATFE-86: fixed bug in oif loop mode
 * 
 * Hydra_Software_Devel/104   6/3/11 10:14a enavarro
 * SW7358-48: fixed compiler error in 7358
 * 
 * Hydra_Software_Devel/103   6/2/11 5:32p enavarro
 * SWSATFE-86: fixed opll programming bug
 * 
 * Hydra_Software_Devel/102   5/12/11 2:35p enavarro
 * SWSATFE-86: support undersample mode
 * 
 * Hydra_Software_Devel/101   5/10/11 3:48p ckpark
 * SWSATFE-118: override FILTCTL for < 3Msps for better DFT performance;
 * widen acquisition baud loop bw
 * 
 * Hydra_Software_Devel/100   5/10/11 2:04p enavarro
 * SWSATFE-118: fixed baud loop bw programming
 * 
 * Hydra_Software_Devel/99   5/5/11 10:21a enavarro
 * SWSATFE-86: bypass destagger and DCO for 4528; updated initial BFOS for
 * 4528
 * 
 * Hydra_Software_Devel/98   5/3/11 3:49p ckpark
 * SWSATFE-75: Commented out debug on.
 * 
 * Hydra_Software_Devel/97   5/3/11 3:31p ckpark
 * SWSATFE-75: DFT non-decimation filter setting is fixed, DFCTL setting
 * is corrected, I/Q phase detector is enabled to timing lock before DFT
 * 
 * Hydra_Software_Devel/96   5/2/11 10:01a enavarro
 * SWSATFE-86: added detection for undersample mode; updated opll
 * calculation
 * 
 * Hydra_Software_Devel/95   4/29/11 3:01p enavarro
 * SWSATFE-86: updated opll programming
 * 
 * Hydra_Software_Devel/94   4/29/11 9:42a enavarro
 * SWSATFE-86: removed SPLL register writes in 4528
 * 
 * Hydra_Software_Devel/93   4/25/11 3:11p enavarro
 * SWSATFE-75: changed SDS_CG_SPLL_MDIV_CTRL and SDS_CG_SPLL_CTRL per
 * Hiroshi
 * 
 * Hydra_Software_Devel/92   4/25/11 2:53p ckpark
 * SWSATFE-75: trial of decimation filter setting is added
 * 
 * Hydra_Software_Devel/91   4/13/11 2:42p ronchan
 * SWSATFE-114: cannot call AbortAcq from isr context
 * 
 * Hydra_Software_Devel/90   4/8/11 10:03a enavarro
 * SWSATFE-86: dont include turbo in blindScanModes if BAST_EXCLUDE_TURBO
 * is defined
 * 
 * Hydra_Software_Devel/89   4/5/11 4:28p ckpark
 * SWSATFE-75: Adjusted acquisition EQ mu for Legacy modes
 * 
 * Hydra_Software_Devel/88   4/5/11 4:07p ckpark
 * SWSATFE-75: Need to unfreeze EQ for Legacy modes when acquisition
 * 
 * Hydra_Software_Devel/87   4/5/11 10:53a ckpark
 * SWSATFE-102: normalized tuner calibration threshold is adjusted
 * 
 * Hydra_Software_Devel/86   4/2/11 6:05p enavarro
 * SWSATFE-86: dont Fs smart tune for WFE; call BAST_g3_P_ConfigChanAgc()
 * to set tracking AGC BW in WFE
 * 
 * Hydra_Software_Devel/85   4/1/11 8:02a enavarro
 * SWSATFE-86: implement set/get sample freq for WFE
 * 
 * Hydra_Software_Devel/84   3/25/11 9:25a enavarro
 * SWSATFE-75: changed BAST_g3_P_ConfigFeBl()
 * 
 * Hydra_Software_Devel/83   3/8/11 2:45p ronchan
 * SWSATFE-75: fixed compile warning
 * 
 * Hydra_Software_Devel/82   3/7/11 10:48a ronchan
 * SWSATFE-75: increased default value of ftm tx power
 * 
 * Hydra_Software_Devel/81   3/7/11 8:32a enavarro
 * SWSATFE-86: fixed compile errors for BAST_EXCLUDE_SPUR_CANCELLER
 * 
 * Hydra_Software_Devel/80   3/4/11 5:16p ronchan
 * SWSATFE-75: bypass SPLL power down for now
 * 
 * Hydra_Software_Devel/79   3/4/11 8:12a enavarro
 * SWSATFE-75: add support for BCM4528
 * 
 * Hydra_Software_Devel/78   3/3/11 11:46a enavarro
 * SWSATFE-75: fixed compiler errors when BAST_EXCLUDE_TURBO is defined
 * 
 * Hydra_Software_Devel/77   3/3/11 10:52a enavarro
 * SWSATFE-75: put all diseqc state variables in BAST_g3_P_DiseqcDevice
 * struct and allocate this only if the channel has diseqc capability
 * 
 * Hydra_Software_Devel/76   2/25/11 1:08p ronchan
 * SWSATFE-75: disable spll channel 0 on power down
 * 
 * Hydra_Software_Devel/75   2/14/11 4:37p ronchan
 * SWSATFE-75: adjusted initial value for ftm tx power
 * 
 * Hydra_Software_Devel/74   2/9/11 3:17p enavarro
 * SWSATFE-75: fixed bug that incorrectly cleared reacqCtl
 * 
 * Hydra_Software_Devel/73   2/3/11 3:39p ckpark
 * SWSATFE-75: set to tracking LPF after DFT freq estimate
 * 
 * Hydra_Software_Devel/72   2/1/11 3:04p enavarro
 * SWSATFE-75: updated reacq_ctl bit definition
 * 
 * Hydra_Software_Devel/71   1/27/11 3:41p enavarro
 * SWSATFE-88: removed dft_size config param
 * 
 * Hydra_Software_Devel/70   1/26/11 4:48p enavarro
 * SWSATFE-75: updated comment
 * 
 * Hydra_Software_Devel/69   1/24/11 11:49a enavarro
 * SWSATFE-75: added option to disable Fs smart tune
 * 
 * Hydra_Software_Devel/68   1/24/11 10:13a enavarro
 * SWSATFE-75: add ability to specify ndiv when setting sample freq;
 * implement smart tune
 * 
 * Hydra_Software_Devel/67   1/19/11 3:49p ronchan
 * SWSATFE-99: initialize modified fsk channel config parameters
 * 
 * Hydra_Software_Devel/66   1/11/11 3:26p ronchan
 * SWSATFE-75: initialize 3445 parameters
 * 
 * Hydra_Software_Devel/65   12/31/10 9:46a enavarro
 * SWSATFE-75: minor change in BAST_g3_P_OnStableLock()
 * 
 * Hydra_Software_Devel/64   12/29/10 12:11p enavarro
 * SWSATFE-75: fixed BAST_g3_P_GetActualMode() in blind scan mode
 * 
 * Hydra_Software_Devel/63   12/28/10 5:20p enavarro
 * SWSATFE-75: fixed bug in blind scan for turbo signal
 * 
 * Hydra_Software_Devel/62   12/28/10 4:07p enavarro
 * SWSATFE-75: check out signal detect function
 * 
 * Hydra_Software_Devel/61   12/27/10 3:38p enavarro
 * SWSATFE-75: fixed BAST_g3_P_GetActualMode() for turbo scan; set
 * reacquisition timeout for turbo mode
 * 
 * Hydra_Software_Devel/60   12/27/10 8:53a enavarro
 * SWSATFE-75: change scaling of rainFadeThreshold
 * 
 * Hydra_Software_Devel/59   12/24/10 3:06p enavarro
 * SWSATFE-75: increase lock stable timeout to 5 msecs for ldpc/turbo; set
 * tracking plc on first lock
 * 
 * Hydra_Software_Devel/58   12/23/10 1:49p enavarro
 * SWSATFE-75: in BAST_g3_P_PrepareNewAcquisition(), abort current
 * acquisition before turning off afec/tfec
 * 
 * Hydra_Software_Devel/57   12/22/10 3:35p enavarro
 * SWSATFE-75: set reacq timeout to300 msecs; fixed
 * BAST_g3_P_GetActualMode() for ldpc scan
 * 
 * Hydra_Software_Devel/56   12/22/10 3:03p enavarro
 * SWSATFE-75: check out BAST_g3_P_GetCarrierError()
 * 
 * Hydra_Software_Devel/55   12/22/10 11:14a enavarro
 * SWSATFE-75: fixed BAST_g3_P_LeakPliToFli(); add call to
 * onMonitorLockFunct in BAST_g3_P_OnMonitorLock()
 * 
 * Hydra_Software_Devel/54   12/22/10 10:21a enavarro
 * SWSATFE-75: set tracking PLC in BAST_g3_P_OnStableLock(); check for
 * forced reacquire after onStableLockFunct()
 * 
 * Hydra_Software_Devel/53   12/21/10 2:12p enavarro
 * SWSATFE-75: initialize SDS_CG_SPLL_CTRL; only write to HPOVERRIDE from
 * HP functions; disable HP in BAST_g3_P_PrepareNewAcquisition()
 * 
 * Hydra_Software_Devel/52   12/18/10 6:44p enavarro
 * SWSATFE-75: fixed calculation error in BAST_g3_P_SetBaudBw(); fixed bug
 * in writing to AGC_ABW
 * 
 * Hydra_Software_Devel/51   12/18/10 3:13p enavarro
 * SWSATFE-75: removed debug code
 * 
 * Hydra_Software_Devel/50   12/17/10 6:07p ckpark
 * SWSATFE-75: disable PSL as default
 * 
 * Hydra_Software_Devel/49   12/17/10 4:59p ckpark
 * SWSATFE-75: updated BAST_g3_P_InitBert() and BAST_g3_P_Acquire1() per
 * TT40G
 * 
 * Hydra_Software_Devel/48   12/17/10 10:43a enavarro
 * SWSATFE-75: put back original acq baud loop bw and plc; return 0 for
 * carrier error for now until we have time to verify it; initialize
 * lockIsrFlag
 * 
 * Hydra_Software_Devel/47   12/15/10 5:07p enavarro
 * SWSATFE-75: removed debug code
 * 
 * Hydra_Software_Devel/46   12/15/10 3:27p enavarro
 * SWSATFE-75: initialize lockIsrFlag
 * 
 * Hydra_Software_Devel/44   12/14/10 3:44p enavarro
 * SWSATFE-75: widen baud loop bw; widen acquisition plc
 * 
 * Hydra_Software_Devel/43   12/13/10 10:13p ronchan
 * SWSATFE-75: tuner requires double Fs
 * 
 * Hydra_Software_Devel/42   12/10/10 3:55p enavarro
 * SWSATFE-75: changed FE_IQCTL per change in RDB
 * 
 * Hydra_Software_Devel/41   12/9/10 9:02a enavarro
 * SWSATFE-75: fixed naming conflict
 * 
 * Hydra_Software_Devel/40   11/30/10 2:59p enavarro
 * SWSATFE-75: force not locked if carrier offset goes beyond search_range
 * and BAST_G3_CONFIG_REACQ_CTL_FREQ_DRIFT bit is set
 * 
 * Hydra_Software_Devel/39   11/29/10 3:25p enavarro
 * SWSATFE-75: updated pfctl value for turbo
 * 
 * Hydra_Software_Devel/38   10/22/10 3:07p ronchan
 * SWSATFE-75: fixed DisableChannelInterrupts call in Reacquire to be ISR
 * context
 * 
 * Hydra_Software_Devel/37   10/20/10 4:32p enavarro
 * SWSATFE-10: added BAST_DEBUG_ACQ() macro
 * 
 * Hydra_Software_Devel/36   10/18/10 2:54p enavarro
 * SWSATFE-75: added peak scan support
 * 
 * Hydra_Software_Devel/35   10/18/10 10:01a enavarro
 * SWSATFE-75: finished implementing BAST_g3_P_ConfigOif()
 * 
 * Hydra_Software_Devel/34   10/15/10 4:19p enavarro
 * SWSATFE-75: initial coding of BAST_g3_P_ConfigOif()
 * 
 * Hydra_Software_Devel/33   10/14/10 5:40p ronchan
 * SWSATFE-75: initialize dsecSettings struct
 * 
 * Hydra_Software_Devel/32   10/14/10 8:49a enavarro
 * SWSATFE-75: initialize equalizer
 * 
 * Hydra_Software_Devel/31   10/12/10 3:33p enavarro
 * SWSATFE-75: fixed debug output in BAST_g3_P_GetSampleFreq()
 * 
 * Hydra_Software_Devel/30   10/12/10 9:54a enavarro
 * SWSATFE-75: add BERR_TRACE to returned errors
 * 
 * Hydra_Software_Devel/29   10/11/10 11:34a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/28   10/5/10 3:06p enavarro
 * SWSATFE-75: added pli-to-fli leak
 * 
 * Hydra_Software_Devel/27   10/4/10 2:40p enavarro
 * SWSATFE-75: updated channel initialization
 * 
 * Hydra_Software_Devel/26   9/30/10 2:03p enavarro
 * SWSATFE-75: store transport settings in BAST_OutputTransportSettings
 * struct in the channel handle; removed xportCtl channel handle member
 * 
 * Hydra_Software_Devel/25   9/29/10 3:50p enavarro
 * SWSATFE-80: removed xport_ctl config parameter
 * 
 * Hydra_Software_Devel/24   9/29/10 11:55a enavarro
 * SWSATFE-75: added bfos and bTimingLock to channel handle struct
 * 
 * Hydra_Software_Devel/23   9/29/10 10:12a enavarro
 * SWSATFE-75: added BAST_g3_P_IsTimingLoopLocked(); initialize
 * signalDetectStatus; removed unused channel handle members
 * 
 * Hydra_Software_Devel/22   9/28/10 2:41p enavarro
 * SWSATFE-75: implemented BAST_g3_P_GetSymbolRateError() and
 * BAST_g3_P_GetCarrierError()
 * 
 * Hydra_Software_Devel/20   9/24/10 3:05p enavarro
 * SWSATFE-75: fixed comment
 * 
 * Hydra_Software_Devel/19   9/23/10 6:20p ronchan
 * SWSATFE-75: added SetAgcTrackingBw() function
 * 
 * Hydra_Software_Devel/18   9/23/10 2:20p ronchan
 * SWSATFE-75: renamed to bast_g3_priv_acq.c
 * 
 * Hydra_Software_Devel/17   9/23/10 2:06p enavarro
 * SWSATFE-75: added BAST_g3_P_SdsDisableOif(); implemented
 * BAST_g3_P_Reacquire(); call BAST_g3_P_CheckStatusIndicators() in
 * BAST_g3_P_OnMonitorLock()
 * 
 * Hydra_Software_Devel/16   9/23/10 10:39a ronchan
 * SWSATFE-75: moved script_qpsk_0 to Acquire1
 * 
 * Hydra_Software_Devel/15   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/14   9/22/10 10:10a enavarro
 * SWSATFE-75: set tracking PLC in OnStableLock()
 * 
 * Hydra_Software_Devel/13   9/21/10 3:57p enavarro
 * SWSATFE-75: implemented tracking and lock filtering
 * 
 * Hydra_Software_Devel/12   9/21/10 11:48a enavarro
 * SWSATFE-75: clean up function declaration
 * 
 * Hydra_Software_Devel/11   9/21/10 10:33a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/10   9/20/10 5:26p ronchan
 * SWSATFE-75: fixed compile errors
 * 
 * Hydra_Software_Devel/9   9/20/10 5:10p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/8   9/17/10 5:27p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/7   9/16/10 3:01p enavarro
 * SWSATFE-75: added agc init; removed delay after dft_freq_estimate; call
 * BAST_g3_P_ConfigPlc() instead of calling SetPlc() with fixed bw
 * 
 * Hydra_Software_Devel/6   9/15/10 1:45p enavarro
 * SWSATFE-75: implemented BAST_g3_P_Acquire()
 * 
 * Hydra_Software_Devel/5   9/15/10 10:30a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/4   9/14/10 2:29p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/3   9/8/10 10:35a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/2   9/1/10 5:02p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   8/31/10 1:57p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

BDBG_MODULE(bast_g3_priv_acq);


#define BAST_DEBUG_ACQ(x) /* x */


/******************************************************************************
 BAST_g3_P_InitHandleDefault()
******************************************************************************/
BERR_Code BAST_g3_P_InitHandleDefault(BAST_Handle h)
{
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)(h->pImpl);

#ifndef BAST_EXCLUDE_FTM
   hDev->bFtmLocalReset = false; /* must initialize this flag independent of ftm init */
   hDev->fskTxChannel = BAST_FskChannel_e0;
   hDev->fskRxChannel = BAST_FskChannel_e0;
   hDev->ftmTxPower = 0xF;
#endif  
   hDev->searchRange = 5000000;
   hDev->bFtmPoweredDown = false;
#ifndef BAST_EXCLUDE_BCM3445
   hDev->bBcm3445 = false;
   hDev->bcm3445Address = BAST_BCM3445_DEFAULT_ADDRESS;
#endif   
   hDev->bInit = false; 
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_InitChannelHandle()
******************************************************************************/
BERR_Code BAST_g3_P_InitChannelHandle(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
#ifndef BAST_EXCLUDE_LDPC   
   int i;
#endif

   hChn->baudTimerIsr = NULL;
   hChn->berTimerIsr = NULL;
   hChn->gen1TimerIsr = NULL;
   hChn->gen2TimerIsr = NULL;
   hChn->gen3TimerIsr = NULL;
   hChn->acqState = BAST_AcqState_eIdle;
#ifndef BAST_EXCLUDE_MI2C   
   hChn->bMi2cInProgress = false;
#endif
   hChn->berErrors = 0;
   hChn->mpegErrorCount = 0;
   hChn->mpegFrameCount = 0;
#ifndef BAST_EXCLUDE_LDPC   
   hChn->ldpcBadBlocks = 0;
   hChn->ldpcCorrBlocks = 0;
   hChn->ldpcTotalBlocks = 0;
   hChn->ldpcCtl = BAST_G3_CONFIG_LDPC_CTL_DISABLE_PSL;   
   hChn->ldpcScanModes = BAST_G3_CONFIG_LDPC_SCAN_MODES_ALL;   
   for (i = 0; i < 16; i++)
      hChn->ldpcScramblingSeq[i] = 0x00;
   hChn->ldpcScramblingSeq[2] = 0x01;   
   hChn->acmMaxMode = BAST_Mode_eLdpc_8psk_9_10;
#endif   
   hChn->rsCorr = 0;
   hChn->rsUncorr = 0;
   hChn->preVitErrors = 0;
#ifndef BAST_EXCLUDE_TURBO   
   hChn->turboBadBlocks = 0;
   hChn->turboCorrBlocks = 0;
   hChn->turboTotalBlocks = 0;
#endif   
   hChn->reacqCtl = BAST_G3_CONFIG_REACQ_CTL_RETUNE | BAST_G3_CONFIG_REACQ_CTL_FREQ_DRIFT;
   hChn->bLastLocked = false;
   hChn->bUndersample = false;
   hChn->timeSinceStableLock = 0;
#ifndef BAST_EXCLUDE_SPUR_CANCELLER
   BAST_g3_P_ClearSpurCancellerConfig(h);
#endif
   if (hChn->bHasDiseqc)
   {
      hChn->diseqc->diseqc1TimerIsr = NULL;
      hChn->diseqc->diseqc2TimerIsr = NULL;
      hChn->diseqc->dsecSettings.bEnvelope = true;
      hChn->diseqc->dsecSettings.bToneAlign = false;
      hChn->diseqc->dsecSettings.bDisableRRTO = false;
      hChn->diseqc->dsecSettings.bEnableToneburst = false;
      hChn->diseqc->dsecSettings.bToneburstB = false;
      hChn->diseqc->dsecSettings.bOverrideFraming = false;
      hChn->diseqc->dsecSettings.bExpectReply = false;
      hChn->diseqc->dsecSettings.bEnableLNBPU = false;
      hChn->diseqc->dsecSettings.bDisableRxOnly = false; 
      hChn->diseqc->dsecSettings.rrtoUsec = 210000;        /* 210000us */
      hChn->diseqc->dsecSettings.bitThreshold = 0x2D3;     /* 723us */
      hChn->diseqc->dsecSettings.toneThreshold = 0x6E;     /* 687.5mV * 0.16 = 110 */
      hChn->diseqc->dsecSettings.preTxDelay = 15;          /* default 15 ms delay */
      hChn->diseqc->dsecSettings.vsenseThresholdHi = 0x30; /* ~22 V */
      hChn->diseqc->dsecSettings.vsenseThresholdLo = 0xD1; /* ~10 V */
      hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eSuccess;
   }
   
   hChn->blindScanModes = BAST_G3_CONFIG_BLIND_SCAN_MODES_DVB;
#ifndef BAST_EXCLUDE_LDPC
   hChn->blindScanModes |= BAST_G3_CONFIG_BLIND_SCAN_MODES_LDPC;
#endif
#ifndef BAST_EXCLUDE_TURBO   
   hChn->blindScanModes |= BAST_G3_CONFIG_BLIND_SCAN_MODES_TURBO;
#endif

#ifndef BAST_EXCLUDE_POWERDOWN
   hChn->coresPoweredDown = BAST_CORE_DISEQC;
#else
   hChn->coresPoweredDown = 0;
#endif
   hChn->dtvScanCodeRates = BAST_G3_CONFIG_DTV_SCAN_CODE_RATES_ALL;
   hChn->dvbScanCodeRates = BAST_G3_CONFIG_DVB_SCAN_CODE_RATES_ALL;
   hChn->dciiScanCodeRates = BAST_G3_CONFIG_DCII_SCAN_CODE_RATES_ALL;
#ifndef BAST_EXCLUDE_TURBO   
   hChn->turboScanCurrMode = 0;
   hChn->turboScanModes = BAST_G3_CONFIG_TURBO_SCAN_MODES_ALL;
   hChn->turboCtl = 0;   
#endif   
   hChn->peakScanSymRateMin = 0;
   hChn->peakScanSymRateMax = 0;   
   hChn->acqTime = 0;
#ifndef BAST_EXCLUDE_STATUS_EVENTS      
   hChn->statusEventIndicators = 0;
   hChn->rainFadeThreshold = 3<<3; /* 3dB*2^3 */
   hChn->freqDriftThreshold = 1500000;
   hChn->bStatusInterrupts = false;
   hChn->rainFadeWindow = 3000; /* 300 secs */
#endif   
   hChn->stuffBytes = 0;
   hChn->plcCtl = 0;
   hChn->bFsNotDefault = true;
   hChn->dftFreqEstimateStatus = 0;
   hChn->tunerCtl = 0;
   hChn->bReacqTimerExpired = false;
   hChn->dftRangeStart = 0x0000;
   hChn->dftRangeEnd = 0x00FF;
   hChn->signalDetectStatus = 0;
#ifndef BAST_EXCLUDE_BCM3445   
   hChn->bcm3445Status = 0;
   hChn->bcm3445Mi2cChannel = BAST_Mi2cChannel_e0;
   hChn->bcm3445TunerInput = BAST_Bcm3445OutputChannel_eNone;
   hChn->bcm3445Ctl = BAST_G3_BCM3445_CTL_AGC_TOP_MID;
#endif   
#ifndef BAST_HAS_WFE  
   hChn->tunerFilCalUpperThresh = 65; /* upper threshold is 65% of half_tone_avg */
   hChn->tunerFilCalLowerThresh = 35; /* lower threshold is 35% of half_tone_avg */
   hChn->tunerLsRange = 2;
   hChn->bOverrideKvco = false;
   hChn->bCalibrateKvco = false;
   hChn->tunerVcoFreqMhz = 0;
   hChn->tunerKvcoCntl = 0;
   hChn->tunerVcRefLow = 19;
   hChn->tunerVcRefHigh = 25;
   hChn->tunerAgcThreshold = 0x4F340040;  /* [31:16] for BB, [15:0] for LNA */
   hChn->tunerAgcWinLength = 0x13231323;  /* [31:16] for BB, [15:0] for LNA */
   hChn->tunerAgcAmpThresh = 0x1010;      /* [12:8] for BB, [4:0] for LNA */
   hChn->tunerAgcLoopCoeff = 0x1414;      /* [12:8] for BB, [4:0] for LNA */
   hChn->tunerDaisyGain = 0x2;   /* default daisy output gain */
   hChn->tempAmbient = 0;
#endif   
   hChn->bMonitorLock = true;  
   hChn->debug1 = 0;
   hChn->debug2 = 0;
   hChn->debug3 = 0;
   hChn->acqParams.symbolRate = 20000000;
   hChn->acqParams.carrierFreqOffset = 0;
   hChn->acqParams.mode = BAST_Mode_eDss_scan;
   hChn->acqParams.acq_ctl = BAST_ACQSETTINGS_DEFAULT;

   BAST_g3_P_InitConfig(h); /* chip-specific configuration */
   
   BAST_g3_ResetInterruptCounters(h);   
#ifndef BAST_EXCLUDE_STATUS_EVENTS
   retCode = BAST_g3_P_ResetStatusIndicators(h);
#endif

   return retCode;
}


/******************************************************************************
 BAST_g3_P_InitNextChannel()
******************************************************************************/
BERR_Code BAST_g3_P_InitNextChannel(BAST_ChannelHandle h)
{  
   BERR_Code retCode = BERR_SUCCESS;
   
   if (h->channel == (h->pDevice->totalChannels - 1))
      goto set_init_done_event;
   else
   {
      if (h->pDevice->pChannels[h->channel + 1] != NULL)
         retCode = BAST_g3_P_InitChannels(h->pDevice->pChannels[h->channel + 1], true);
      else
      {
         set_init_done_event:
      BKNI_SetEvent(((BAST_g3_P_Handle *)(h->pDevice->pImpl))->hInitDoneEvent);
   }
   }

   return retCode;
}


/******************************************************************************
 BAST_g3_P_InitChannels() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_InitChannels(BAST_ChannelHandle h, bool bInitNextChannel)
{
   static const uint32_t script_sds_init[] =
   {
#ifdef BAST_HAS_WFE
      /* BCM4528 */
      BAST_SCRIPT_WRITE(BCHP_SDS_CG_CGDIV01, 0x005D0119), /* 375KHz mi2c clk */
      BAST_SCRIPT_WRITE(BCHP_SDS_INTR2_0_CPU_MASK_SET, 0x3FFFFFFF),
      BAST_SCRIPT_WRITE(BCHP_SDS_INTR2_0_CPU_CLEAR, 0x3FFFFFFF),
      BAST_SCRIPT_AND(BCHP_SDS_MISC_MISCTL, ~0x00000008), /* power down ring oscillators for noise reduction */
      BAST_SCRIPT_WRITE(BCHP_SDS_BL_BFOS, 0x03A0CC00), /* assume Fb=20, Fs=145.125 */
      BAST_SCRIPT_WRITE(BCHP_SDS_MISC_TPDIR, 0xFF4B008F),
      /*BAST_SCRIPT_WRITE(BCHP_SDS_MISC_TPCTL2, 0x8F000000),*/
      BAST_SCRIPT_WRITE(BCHP_SDS_FE_ADCPCTL, 0x000F0001),  /* i_nofifo=1, q_nofifo=1, insel=0, ob_tc=0 */
      BAST_SCRIPT_AND(BCHP_SDS_FE_ADCPCTL, ~0x1), /* clear reset */
      BAST_SCRIPT_WRITE(BCHP_SDS_FE_IQCTL, 0x00008284), /* bypass IQ phase/amplitude imbalance correction */ 
      BAST_SCRIPT_WRITE(BCHP_SDS_FE_DSTGCTL, 0x00000001), /* bypass destagger */
      BAST_SCRIPT_WRITE(BCHP_SDS_FE_DCOCTL, 0x00020311), /* bypass DCO */
#else
      BAST_SCRIPT_WRITE(BCHP_SDS_CG_CGDIV01, 0x005D0119), /* 375KHz mi2c clk */
      BAST_SCRIPT_WRITE(BCHP_SDS_CG_SPLL_CTRL, 0x84400000), /* per Hiroshi */
   #if (BCHP_CHIP==7358)
      BAST_SCRIPT_AND_OR(BCHP_SDS_CG_SPLL_MDIV_CTRL, ~0x000FF800, 0x0003C000), /* pll_ctrl_msb=0x78 */
      BAST_SCRIPT_AND_OR(BCHP_SDS_CG_SPLL_CTRL, ~0xF0000000, 0xA0000000), /* Kp=0xA */  
   #endif      
      BAST_SCRIPT_WRITE(BCHP_SDS_INTR2_0_CPU_MASK_SET, 0x3FFFFFFF),
      BAST_SCRIPT_WRITE(BCHP_SDS_INTR2_0_CPU_CLEAR, 0x3FFFFFFF),
   #if (BCHP_CHIP!=4517)      
      BAST_SCRIPT_WRITE(BCHP_SDS_INTR2_1_CPU_MASK_SET, 0x000000FF),
      BAST_SCRIPT_WRITE(BCHP_SDS_INTR2_1_CPU_CLEAR, 0x000000FF),
      BAST_SCRIPT_AND(BCHP_SDS_MISC_TPCTL2, ~0x70000000),
   #endif      
      BAST_SCRIPT_AND(BCHP_SDS_MISC_MISCTL, ~0x00000008), /* power down ring oscillators for noise reduction */
      BAST_SCRIPT_WRITE(BCHP_SDS_AGC_AGCCTL, 0x00041F1F), /* IF AGC only, freeze/reset RF/IF */
      BAST_SCRIPT_WRITE(BCHP_SDS_BL_BFOS, 0x049CCC00),     
      BAST_SCRIPT_WRITE(BCHP_SDS_MISC_TPDIR, 0xFF4B008F),
      BAST_SCRIPT_OR(BCHP_SDS_FE_ADCPCTL, 0x00000001),  /* reset */
      BAST_SCRIPT_AND(BCHP_SDS_FE_ADCPCTL, 0xFFFFFF00), /* clear reset */
      BAST_SCRIPT_WRITE(BCHP_SDS_FE_IQCTL, 0x00008080), /* changed from TT40G */ 
#endif      
      BAST_SCRIPT_EXIT
   };

#ifndef BAST_EXCLUDE_EXT_TUNER      
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#endif
   BERR_Code retCode;

   BAST_CHK_RETCODE(BAST_g3_P_SetDefaultSampleFreq(h));

   /* initialize clockgen and sds */
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_sds_init));  

   BAST_g3_P_IndicateNotLocked(h);

#ifndef BAST_EXCLUDE_EXT_TUNER      
   if (hChn->bExternalTuner)
   {
      /* external tuner */
      BDBG_ERR(("BAST_g3_P_InitChannels(): external tuner code not implemented"));
      BDBG_ASSERT(0);
   }
#endif

   if ((((BAST_g3_P_Handle*)h->pDevice->pImpl)->bInit == false) && bInitNextChannel)
      retCode = BAST_g3_P_TunerInit(h, BAST_g3_P_InitNextChannel);

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_SetDefaultSampleFreq()
******************************************************************************/
BERR_Code BAST_g3_P_SetDefaultSampleFreq(BAST_ChannelHandle h)
{
   BERR_Code retCode = BERR_SUCCESS;

#ifndef BAST_HAS_WFE   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   

   if (hChn->bFsNotDefault)
   {
      BAST_CHK_RETCODE(BAST_g3_P_SetSampleFreq(h, 82, 6));
        
      if (hChn->sampleFreq == BAST_DEFAULT_SAMPLE_FREQ)
         hChn->bFsNotDefault = false;
      else
      {
         BDBG_ERR(("unable to set default sample freq (%d Hz)", BAST_DEFAULT_SAMPLE_FREQ));
         BERR_TRACE(retCode = BAST_ERR_AP_UNKNOWN);  
      }
   }

   done:
#else
   /* do nothing */
#if 0
   hChn->sampleFreq = BAST_DEFAULT_SAMPLE_FREQ;
   hChn->bFsNotDefault = false;
#endif   
#endif
   
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetLockChangeEventHandle()
******************************************************************************/
BERR_Code BAST_g3_P_GetLockChangeEventHandle(BAST_ChannelHandle h, BKNI_EventHandle *hLockChangeEvent)
{
   BDBG_ENTER(BAST_g3_P_GetLockChangeEventHandle);
   
   *hLockChangeEvent = ((BAST_g3_P_ChannelHandle *)h->pImpl)->hLockChangeEvent;
   
   BDBG_LEAVE(BAST_g3_P_GetLockChangeEventHandle);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_ProcessScript()
******************************************************************************/
BERR_Code BAST_g3_P_ProcessScript(BAST_ChannelHandle hChn, uint32_t const *pScr)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, val, reg, op;

   while (1)
   {
      mb = *pScr++;
      val = *pScr++;
      reg = mb & ~BAST_SCRIPT_OPCODE;
      op = mb & BAST_SCRIPT_OPCODE;
      if (op == BAST_SCRIPT_OPCODE_WRITE)
      {
         BAST_g3_P_WriteRegister(hChn, reg, &val);
      }
      else if (op == BAST_SCRIPT_OPCODE_AND)
      {
         BAST_g3_P_ReadRegister(hChn, reg, &mb);
         mb &= val;
         BAST_g3_P_WriteRegister(hChn, reg, &mb);
      }
      else if (op == BAST_SCRIPT_OPCODE_OR)
      {
         BAST_g3_P_ReadRegister(hChn, reg, &mb);
         mb |= val;
         BAST_g3_P_WriteRegister(hChn, reg, &mb);
      }
      else if (op == BAST_SCRIPT_OPCODE_AND_OR)
      {
         BAST_g3_P_ReadRegister(hChn, reg, &mb);
         mb &= val;
         mb |= *pScr++;
         BAST_g3_P_WriteRegister(hChn, reg, &mb);
      }
      else if (op == BAST_SCRIPT_OPCODE_DEBUG)
      {
         BDBG_MSG(("script debug: %d", val));
      }
      else if (op == BAST_SCRIPT_OPCODE_EXIT)
         break;
      else
      {
         BDBG_ERR(("invalid script operation"));
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
      }
   }

   return retCode;
}


/******************************************************************************
 BAST_g3_P_GCF()
******************************************************************************/
uint32_t BAST_g3_P_GCF(uint32_t m, uint32_t n)
{
   uint32_t L1, L2, L3, L4;

   L1 = m;
   L2 = n;

   while (L2 > 0)
   {
      L3 = L1 / L2;
      L4 = L1 - (L2 * L3);
      L1 = L2;
      L2 = L4;
   }
   return L1;
}


/******************************************************************************
 BAST_g3_P_ReadModifyWriteRegister()
******************************************************************************/
void BAST_g3_P_ReadModifyWriteRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t and_mask, uint32_t or_mask)
{
   uint32_t val;

   BAST_g3_P_ReadRegister(h, reg, &val);
   val = (val & and_mask) | or_mask;
   BAST_g3_P_WriteRegister(h, reg, &val);
}


/******************************************************************************
 BAST_g3_P_OrRegister()
******************************************************************************/
void BAST_g3_P_OrRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t or_mask)
{
   uint32_t val;

   BAST_g3_P_ReadRegister(h, reg, &val);
   val |= or_mask;
   BAST_g3_P_WriteRegister(h, reg, &val);
}


/******************************************************************************
 BAST_g3_P_AndRegister()
******************************************************************************/
void BAST_g3_P_AndRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t and_mask)
{
   uint32_t val;

   BAST_g3_P_ReadRegister(h, reg, &val);
   val &= and_mask;
   BAST_g3_P_WriteRegister(h, reg, &val);
}


/******************************************************************************
 BAST_g3_P_ToggleBit()
******************************************************************************/
void BAST_g3_P_ToggleBit(BAST_ChannelHandle h, uint32_t reg, uint32_t mask)
{
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, reg, &val);
      
   val |= mask;
   BAST_g3_P_WriteRegister(h, reg, &val);
      
   val &= ~mask;
   BAST_g3_P_WriteRegister(h, reg, &val);  
}


/******************************************************************************
 BAST_g3_P_UpdateUndersampleMode()
******************************************************************************/
void BAST_g3_P_UpdateUndersampleMode(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;  
   uint32_t val;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_CG_CGDIV00, &val);

   if (hChn->sampleFreq  < (hChn->acqParams.symbolRate << 2))
   {
      BDBG_MSG(("Undersample Mode enabled"));
      hChn->bUndersample = true;
      val |= 0x01;
   }
   else
   {
      hChn->bUndersample = false;
      val &= ~0x01;
   }
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CG_CGDIV00, &val);   
}


#ifndef BAST_HAS_WFE
/******************************************************************************
 BAST_g3_P_GetSampleFreqMN()
******************************************************************************/
void BAST_g3_P_GetSampleFreqMN(BAST_ChannelHandle h, uint32_t *pN, uint32_t *pM)
{
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CG_SPLL_NPDIV, &val);
   *pN = (val >> 24) & 0xFF;
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CG_SPLL_MDIV_CTRL, &val);
   *pM = val & 0xFF;   
}
#endif


/******************************************************************************
 BAST_g3_P_SetSampleFreq() - 
    for narrowband tuners, Fs = (xtal * ndiv) / (4 * mdiv)
    for wideband tuner, Fs = ndiv
******************************************************************************/
BERR_Code BAST_g3_P_SetSampleFreq(BAST_ChannelHandle h, uint32_t ndiv, uint32_t mdiv)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;  
   BERR_Code retCode = BERR_SUCCESS;
   
#ifndef BAST_HAS_WFE
   uint32_t val;
   uint16_t count = 0;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_CG_SPLL_NPDIV, &val);
   val &= ~0xFF000000;
   val |= (ndiv << 24);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CG_SPLL_NPDIV, &val);
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CG_SPLL_MDIV_CTRL, &val);
   val &= ~0x000000FF;
   val |= (mdiv & 0xFF);
   val |= 0x00000100;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CG_SPLL_MDIV_CTRL, &val);
   val &= ~0x00000100;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CG_SPLL_MDIV_CTRL, &val);
   
   /* check spll lock status */
   do {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_CG_SPLL_STATUS, &val);
      count++;
      if (count > 100)
         BKNI_Delay(1);
   }
   while (((val & 0x1000) == 0) && (count <= 200));

   if ((val & 0x1000) == 0)
   {
      BDBG_ERR(("BAST_g3_P_SetSampleFreq(): SPLL not locked"));     
      return BAST_ERR_SAMPLE_FREQ;
   }
   
   retCode = BAST_g3_P_GetSampleFreq(h, &(hChn->sampleFreq));
   if (retCode == BERR_SUCCESS)
   {
      if (hChn->sampleFreq == BAST_DEFAULT_SAMPLE_FREQ)
         hChn->bFsNotDefault = false;
      else
      {
         BDBG_MSG(("sample freq = %u", hChn->sampleFreq));
         hChn->bFsNotDefault = true;
      }
   }
#else
   /* Fs is not controlled by AST PI */
   BSTD_UNUSED(ndiv);
   BSTD_UNUSED(mdiv);
   hChn->bFsNotDefault = false;
#endif

   BAST_g3_P_UpdateUndersampleMode(h);

   return retCode;
}


#ifndef BAST_HAS_WFE
/******************************************************************************
 BAST_g3_P_GetVcoRefClock() - updates vcoRefClock
******************************************************************************/
BERR_Code BAST_g3_P_GetVcoRefClock(BAST_ChannelHandle h, uint32_t *pVcoRefClock)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl; 
   uint32_t val, ndiv, pdiv, P_hi, P_lo, Q_hi;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CG_SPLL_NPDIV, &val);
   ndiv = (val >> 24) & 0xFF;
   pdiv = val & 0x07;
   if (pdiv == 0)
      pdiv = 8;
   BAST_MultU32U32(hDev->xtalFreq, ndiv, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, pdiv, &Q_hi, pVcoRefClock);
   return BERR_SUCCESS;
}
#endif


/******************************************************************************
 BAST_g3_P_GetSampleFreq()
******************************************************************************/
BERR_Code BAST_g3_P_GetSampleFreq(BAST_ChannelHandle h, uint32_t *pSampleFreq)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#ifndef BAST_HAS_WFE   
   BERR_Code retCode;
   uint32_t val;

   BAST_CHK_RETCODE(BAST_g3_P_GetVcoRefClock(h, &(hChn->vcoRefClock)));

   BAST_g3_P_ReadRegister(h, BCHP_SDS_CG_SPLL_MDIV_CTRL, &val);
   val &= 0x000000FF;
   *pSampleFreq = (hChn->vcoRefClock / val) >> 1;  /* div back by 2 */

   /* BDBG_MSG(("channel %d: VCO_freq=%u, Fs=%u", h->channel, hChn->vcoRefClock, *pSampleFreq)); */
   
   done:
   return retCode;
#else
   *pSampleFreq = hChn->sampleFreq;
   return BERR_SUCCESS;
#endif
}


/******************************************************************************
 BAST_g3_P_SetNyquistAlpha()
******************************************************************************/
BERR_Code BAST_g3_P_SetNyquistAlpha(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t Fs_over_Fb, nvctl, pfctl;

   Fs_over_Fb = (hChn->sampleFreq * 4) / hChn->acqParams.symbolRate;
   if (Fs_over_Fb < 9)
     nvctl = 0x80;
   else if (Fs_over_Fb < 10)
     nvctl = 0x40;
   else
     nvctl = 0;

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_NYQUIST_20)
   {
      /* alpha = 0.20 */
      nvctl |= 0x04; /* nvctl */
      pfctl = 0x01;  /* pfctl */
   }
   else
   {
      /* alpha = 0.35 */
      pfctl = 0x00; /* pfctl */
   }

   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
      pfctl |= 0x02;

   BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_NVCTL, &nvctl);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_PFCTL, &pfctl);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SetBaudBw() - damp is scaled 2^2
******************************************************************************/
BERR_Code BAST_g3_P_SetBaudBw(BAST_ChannelHandle h, uint32_t bw, uint32_t damp)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t lval1, lval2, lval3, P_hi, P_lo;

   /* BDBG_MSG(("BAST_g3_P_SetBaudBw(): bw=%d, damp*4=%d", bw, damp)); */
   BAST_MultU32U32(bw, 1073741824, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &lval2, &lval1);
   lval2 = ((lval1 * damp) + 1) >> 1;
   lval2 &= 0xFFFFFF00;
   BAST_MultU32U32(lval1, bw, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &lval3, &lval1);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BRLC, &lval2);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BRIC, &lval1);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetNumHb() - returns the number of halfbands
******************************************************************************/
uint32_t BAST_g3_P_GetNumHb(uint32_t Fb, uint32_t Fs)
{
   uint32_t hb_number, rx_os_ratio, P_hi, P_lo, Q_hi;

   BAST_MultU32U32(Fs, 65536, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, Fb, &Q_hi, &rx_os_ratio);
   
   if (rx_os_ratio < (8*65536))
      hb_number = 0;
   else if (rx_os_ratio < (16*65536))
      hb_number = 1;
   else if (rx_os_ratio < (32*65536))
      hb_number = 2;
   else if (rx_os_ratio <= (64*65536))
      hb_number = 3;
   else if (rx_os_ratio <= (128*65536))
      hb_number = 4;
   else
      hb_number = 5;
   return hb_number;
}


/******************************************************************************
 BAST_g3_P_SetBfos()
******************************************************************************/
BERR_Code BAST_g3_P_SetBfos(BAST_ChannelHandle h, uint32_t two_pow_hb)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
   
   BAST_MultU32U32(16777216, hChn->sampleFreq, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &Q_lo);
   BAST_DivU64U32(Q_hi, Q_lo, two_pow_hb, &Q_hi, &Q_lo);

   if (hChn->bUndersample)
      Q_lo = Q_lo << 1;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BFOS, &Q_lo);

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_ConfigBl()
******************************************************************************/
BERR_Code BAST_g3_P_ConfigBl(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t Fs_over_Fb, P_hi, P_lo, Q_hi, filtctl;
   uint32_t val2, val3, data0;
   
   /* Fs_over_Fb = 2^16 * Fs/Fb */
   BAST_MultU32U32(65536, hChn->sampleFreq, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &Fs_over_Fb);

   val2 = Fs_over_Fb;
   data0 = 1;
   while ((val2 > 524288) && (data0 < 64))
   {
      data0 = data0 << 1;
      val2 = val2 >> 1;
   }
   val2 = Fs_over_Fb / data0;
   if ((val2 > 262144) && (val2 < 268698))
   {
      data0 = data0 >> 1;
      if (data0 == 0)
         data0 = 1;
   }
   
   if (data0 > 32) /* all 6 HBs are used */
      filtctl = 0x40;
   else
      filtctl = (32 - data0) | 0xC0;
      
   if (h->pDevice->settings.networkSpec == BAST_NetworkSpec_eEuro)   
      val3 = 373555; /* 5.7*2^16 */
   else
      val3 = 416154; /* 6.35*2^16 */
   if (val2 > val3)
      filtctl |= 0x0000; /* quarter band */
   else if (val2 > 279183) /* 4.26*2^16 */
      filtctl |= 0x4000; /* third band */
   else
      filtctl |= 0xC000; /* half band */ /* was 0xC000 */
      
#if 0
   if (filtctl == 0x40D8) /* 3.7 to 5.4 */
   {
      data0 = 4;
      filtctl = 0xC0DC;
   }
   else if (filtctl == 0x00D8) /* 2.9 to 3.6 */
   {
      filtctl = 0x40D8; /* increase from quarterband to thirdband */
   }   
   else if (((filtctl & 0x00FF) == 0xD0) || ((filtctl & 0x00FF) == 0xC0))
   {
      if (hChn->acqParams.symbolRate < 2500000)
      {
         data0 = 8;
         filtctl = 0x00D8;
      }
      else /* 2.5 to 2.8 */
      {
         filtctl = 0x40D8;
         data0 = 8;
      }        
   }
#endif

   BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_FILTCTL, &filtctl);
   
   return BAST_g3_P_SetBfos(h, data0);
}

   
/******************************************************************************
 BAST_g3_P_ConfigFe()
******************************************************************************/
BERR_Code BAST_g3_P_ConfigFe(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t filtctl, val;

   val = 0;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLIF, &val);

   BAST_g3_P_ReadRegister(h, BCHP_SDS_FE_FILTCTL, &filtctl);

   filtctl &= 0xFF;
   if (filtctl == 0x10)
      val = 0x10;
   else if (filtctl == 0x00)
      val = 0x30;
   else
      val = 0;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_DFCTL, &val);
   
   /* AGF setup */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_FE_AGFCTL, 0x00000001); /* reset AGF */
   if (BAST_MODE_IS_LEGACY_QPSK(hChn->acqParams.mode))
      val = 0x04300000;
   else
      val = 0x0A0A0000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_AGF, &val);
   val = 0;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_AGFCTL, &val);
   
   val = 1;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_MIXCTL, &val);   

   /* reset DCO */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_FE_DCOCTL, 0x00000300);   
   
   /* reset IQ */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_FE_IQCTL, 0x00000001);     

   BAST_CHK_RETCODE(BAST_g3_P_SetNyquistAlpha(h));   
   BAST_CHK_RETCODE(BAST_g3_P_SetBaudBw(h, hChn->acqParams.symbolRate / 100, 4));

   val = 0;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BRI, &val);   
   
   /* reset BL */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_BL_BLPCTL, 0x00000100);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_InitBert()
******************************************************************************/
BERR_Code BAST_g3_P_InitBert(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;

#if 0   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_BERT_BERCTL, &val); 
   val &= 0xFFFF00FF;
#endif   
   val = 0xA5;    
   if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_PN_INVERT) == 0)
      val |= 0x0200;
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_PRBS15)
      val |= 0x0100;
   val |= 1; /* hold BERT in reset state */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BERT_BERCTL, &val);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_StartBert()
******************************************************************************/
BERR_Code BAST_g3_P_StartBert(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_BERT_ENABLE)
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_BERT_BERCTL, &val);  
      val &= 0xFFFFFF00;
      
      /* enable BERT */
      if (hChn->xportSettings.bSerial)
         val |= 0xD0; /* serial */
      else
         val |= 0xC0; /* parallel */

      if ((hChn->xportSettings.bXbert) && (BAST_MODE_IS_DTV(hChn->acqParams.mode)))
      {
         val |= 0x08; /* XBERT */
      }

      BAST_g3_P_WriteRegister(h, BCHP_SDS_BERT_BERCTL, &val);  
      val |= 0x01;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_BERT_BERCTL, &val);  
      val |= 0x02;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_BERT_BERCTL, &val);  
      val &= ~1;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_BERT_BERCTL, &val);  
   }
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SetCarrierBw() - damp is scaled 2^2
******************************************************************************/
BERR_Code BAST_g3_P_SetCarrierBw(BAST_ChannelHandle h, uint32_t bw, uint32_t damp)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t lval1, lval2, P_hi, P_lo, x, mb, stfllc;
   static const uint32_t int_scale[7] = {1, 4, 16, 64, 256, 1024, 4096};
   static const uint32_t lin_scale[5] = {1, 4, 16, 64, 256};

   for (x = 0; x < 5; x++)
   {
      lval1 = ((damp * bw) >> 1) * lin_scale[x];
      if (lval1 > hChn->sampleFreq)
         break;
   }
   if (x)
      x--;

   BAST_MultU32U32(damp * bw, 8192 * 256 * lin_scale[x], &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &lval2, &lval1);
   stfllc = (lval1 << 7) + 0x8000;
   stfllc &= 0x3FFF0000;
   stfllc |= ((x & 0x07) << 8);

   /* compute 16-bit integrator coefficient */
   for (x = 0; x < 7; x++)
   {
      BAST_MultU32U32(bw * bw, int_scale[x], &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &lval2, &lval1);
      if (lval1 > hChn->sampleFreq)
         break;
   }
   if (x)
      x--;

   lval2 = ((bw + 5) / 10);
   BAST_MultU32U32(lval2 * int_scale[x] * 8, lval2 * 8192, &P_hi, &P_lo); 
   lval1 = ((hChn->acqParams.symbolRate + 5) / 10);
   BAST_DivU64U32(P_hi, P_lo, lval1, &P_hi, &P_lo); 
   lval1 = ((hChn->sampleFreq + 5) / 10);
   BAST_DivU64U32(P_hi, P_lo, lval1, &P_hi, &P_lo); 
   mb = ((P_lo << 15) + 0x8000);
   mb &= 0xFFFF0000;
   mb |= ((x & 0x0F) << 8);
  
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLLC, &stfllc);  
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLIC, &mb);
   /* BDBG_MSG(("BAST_g3_P_SetCarrierBw(): bw=%d, damp*4=%d, fllc=0x%X, flic=0x%X", bw, damp, stfllc, mb)); */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_ConfigOif()
 input: opll_N, opll_D, outputBitRate
******************************************************************************/
BERR_Code BAST_g3_P_ConfigOif(BAST_ChannelHandle h)
{ 
   /* set OI_OPLL, OI_OPLL2, OI_OIFCTL01, OI_OSUBD, OI_OPLL_NPDIV OI_OPLL_MDIV_CTRL, OI_OIFCTL00 */
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;   
   uint32_t d_factor, compensated_d, compensated_d_n, byt_clk_sel, bit_clk_opll, val, ndiv_int, 
            vco_ref_clock, vco_base, vco_residue, m1div, ndiv_frac, P_hi, P_lo, Q_hi, Q_lo,
            vco_freq;
   
   for (d_factor = 1; d_factor < 8; d_factor = d_factor << 1)
   {
      if ((hChn->opll_N << 1) < (hChn->opll_D * d_factor))
         break;
   }

   compensated_d = (uint32_t)(hChn->opll_D * (uint32_t)d_factor);
   compensated_d_n = compensated_d - hChn->opll_N;

   BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OPLL, &(hChn->opll_N));   
   BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OPLL2, &compensated_d_n);

   switch (d_factor)
   {
      case 1:
         byt_clk_sel = 0x000;
         break;
         
      case 2:
         byt_clk_sel = 0x100;
         break;
         
      case 4:
         byt_clk_sel = 0x200;
         break;
         
      default: /* d_factor==8 */
         byt_clk_sel = 0x300;
         break;
   }
   BAST_g3_P_ReadRegister(h, BCHP_SDS_OI_OIFCTL01, &val);  
   val &= ~0x00001B00; /* clear bits 8, 9, 11, 12 */   
   val |= byt_clk_sel;
   val |= (byt_clk_sel << 3);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OIFCTL01, &val);  
   
   /* val = ceil(4000000 / hChn->outputBitRate); */
   val = ((4000000 / hChn->outputBitRate) + 1) & 0x000000FF;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OSUBD, &val);
   bit_clk_opll = val * hChn->outputBitRate;

   if (hChn->xportSettings.bOpllBypass)
   {
      BAST_g3_P_OrRegister(h, BCHP_SDS_OI_OIFCTL01, 0x14);
   }
   else
   {
      BAST_g3_P_AndRegister(h, BCHP_SDS_OI_OIFCTL01, ~0x14);
      
      /* set OPLL_NPDIV, OPLL_MDIV_CTRL */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_OI_OPLL_NPDIV, &val);
      val &= 0x07;
      if (val == 0)
         val = 8;
      vco_ref_clock = hDev->xtalFreq / val;
      for (ndiv_int = 30; ndiv_int <= 59; ndiv_int++)
      {
         vco_base = ndiv_int * vco_ref_clock;

         /* m1div = ceil(vco_base/bit_clk_opll) */
         m1div = vco_base / bit_clk_opll;         
         if ((vco_base % bit_clk_opll) > 0)
            m1div++;

         if (m1div >= 255)
            continue;
         
         vco_freq = m1div * bit_clk_opll;
         if ((vco_freq < 1000000000UL) || (vco_freq > 1600000000UL))
            continue;
         
         vco_residue = (m1div * bit_clk_opll) - vco_base;
         if (vco_residue >= vco_ref_clock)
            continue;

         BAST_MultU32U32(vco_residue, 1048576 << 1, &P_hi, &P_lo);
         BAST_DivU64U32(P_hi, P_lo, vco_ref_clock, &Q_hi, &Q_lo);
         ndiv_frac = (Q_lo + 1) >> 1;

         BAST_g3_P_ReadRegister(h, BCHP_SDS_OI_OPLL_NPDIV, &val);
         val &= 0x00000007;
         val |= ((ndiv_int << 24) & 0xFF000000);
         val |= ((ndiv_frac << 4) & 0x00FFFFF0);
         BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OPLL_NPDIV, &val);
         
         BAST_g3_P_ReadRegister(h, BCHP_SDS_OI_OPLL_MDIV_CTRL, &val);  
         val &= 0x000FF800; /* save pll_ctrl_msb */
      
         m1div |= BCHP_SDS_OI_OPLL_MDIV_CTRL_channel_load_en_MASK;
         m1div |= val;
         BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OPLL_MDIV_CTRL, &m1div);   
         m1div &= ~BCHP_SDS_OI_OPLL_MDIV_CTRL_channel_load_en_MASK;
         BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OPLL_MDIV_CTRL, &m1div);            
         break;
      }

      /* enable the loop */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_OI_OIFCTL01, &val);  
      val |= BCHP_SDS_OI_OIFCTL01_loop_en_MASK;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OIFCTL01, &val);        
   }

   val = 0x01; /* reset oif */
   if (hChn->xportSettings.bSerial)
      val |= 0x02;
   if (hChn->xportSettings.bClkInv)
      val |= 0x08;   
   if (hChn->xportSettings.bClkSup)
      val |= 0x10;        
   if (hChn->xportSettings.bVldInv)
      val |= 0x20;        
   if (hChn->xportSettings.bSyncInv)
      val |= 0x40;         
   if (hChn->xportSettings.bErrInv)
      val |= 0x80;        
   if (BAST_MODE_IS_TURBO(hChn->actualMode))
      val |= 0x8100;
   else if (BAST_MODE_IS_LDPC(hChn->actualMode))
      val |= 0x8000;
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_PN_INVERT)
      val |= 0x0800;
   if (hChn->xportSettings.bXbert)
      val |= 0x0400;
   if (hChn->xportSettings.bDelHeader)
      val |= 0x800000;   
   if (hChn->xportSettings.bHead4)
      val |= 0x400000;      
   if (hChn->xportSettings.bSync1)
      val |= 0x200000;  
   BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OIFCTL00, &val);
   val &= ~1; /* toggle OIF reset */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_OI_OIFCTL00, &val);

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SetFfeMainTap()
******************************************************************************/
BERR_Code BAST_g3_P_SetFfeMainTap(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQFFECTL, &val); 
   val &= 0xFF00FFFF;
   if ((BAST_MODE_IS_DTV(hChn->acqParams.mode)) || hChn->bUndersample)
      val |= 0x00060000;
   else
      val |= 0x000C0000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_EQFFECTL, &val); 
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_ConfigAgc()
******************************************************************************/
BERR_Code BAST_g3_P_ConfigAgc(BAST_ChannelHandle h)
{
#ifdef BAST_HAS_WFE
   /* don't have to do anything here since AGCs were already configured in BAST_g3_P_TunerSetFreq() */
   BSTD_UNUSED(h);
   return BERR_SUCCESS;
#else
   static const uint32_t script_agc_init[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_AGC_ABW, 0x0A0A0000),
      BAST_SCRIPT_WRITE(BCHP_SDS_AGC_AGCCTL, 0x00041F19),   /* enable IF AGC only, freeze/reset RF/IF */
      BAST_SCRIPT_AND(BCHP_SDS_AGC_AGCCTL, ~0x00000101),    /* clear IF and RF AGC reset */  
      BAST_SCRIPT_EXIT
   };
   
   BERR_Code retCode;
   uint32_t val;
   
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_agc_init));

   if (h->pDevice->settings.networkSpec == BAST_NetworkSpec_eDefault)
      val = 0x18000000;
   else
      val = 0x10000000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_AGC_ATHR, &val);
   
   done:
   return retCode;
#endif   
}


/******************************************************************************
 BAST_g3_P_SetAgcTrackingBw()
******************************************************************************/
BERR_Code BAST_g3_P_SetAgcTrackingBw(BAST_ChannelHandle h)
{
#ifdef BAST_HAS_WFE
      BAST_g3_P_ConfigChanAgc(h, true);
      return BERR_SUCCESS;
#else
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   if (hChn->agcCtl & BAST_G3_CONFIG_AGC_CTL_METER_MODE)
      val = 0x0A0A0000;
   else
      val = 0x03030000;
   
   BAST_g3_P_WriteRegister(h, BCHP_SDS_AGC_ABW, &val);
   return BERR_SUCCESS;
#endif   
}


/******************************************************************************
 BAST_g3_P_CheckTimingLoopStateMachine()
******************************************************************************/
BERR_Code BAST_g3_P_CheckTimingLoopStateMachine(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t val, i;
   int32_t  bfos_step, sum, bri;
   
   while (hChn->funct_state != 0xFFFFFFFF)
   {
      /* BDBG_MSG(("BAST_g3_P_CheckTimingLoop(): state=%d", hChn->funct_state)); */
      switch (hChn->funct_state)
      {
         case 0:
            /* open up timing loop bw */
            BAST_CHK_RETCODE(BAST_g3_P_SetBaudBw(h, hChn->acqParams.symbolRate / 400, 4 * 4));

            bfos_step = hChn->bfos >> 9;
            if (hChn->count1 == 0)
               bfos_step = -bfos_step;

            val = (uint32_t)((int32_t)hChn->bfos + bfos_step);
            BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BFOS, &val);

            /* reset baud loop integrator */
            BAST_g3_P_ToggleBit(h, BCHP_SDS_BL_BLPCTL, 0x00000100);
            val = 0;
            BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BRI, &val);

            /* wait some time */
            hChn->funct_state = 1;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, 300000, BAST_g3_P_CheckTimingLoopStateMachine);  

         case 1:
            /* Narrow baud loop bandwidth */
            BAST_CHK_RETCODE(BAST_g3_P_SetBaudBw(h, hChn->acqParams.symbolRate / 1600, 11 * 4));

            sum = 0; 
            for (i = 32; i; i--)
            {
               BAST_g3_P_ReadRegister(h, BCHP_SDS_BL_BRI, (uint32_t*)&bri);
               sum += (bri / 32);
            }

            if (hChn->count1)
            {
               if (sum > 0)
                  sum = 0;
               else
                  sum = -sum;
            }
            else if (sum < 0)
               sum = 0;

            /* restore original BFOS */
            BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BFOS, &(hChn->bfos));

            val = 0;
            BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BRI, &val);

            if ((hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_PRESERVE_COMMANDED_FREQ) == 0)     
               BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLIF, &val);

            if (sum > 0x20000)
            {
               if (hChn->count1)
               {
                  /* indicate timing loop locked */
                  hChn->bTimingLock = true;
                  hChn->funct_state = 0xFFFFFFFF;
               }
               else
               {
                  hChn->count1 = 1;
                  hChn->funct_state = 0;
               }
            }
            else
               hChn->funct_state = 0xFFFFFFFF;
            break;

         default:
            break;
      }   
   }   
   
   retCode = hChn->passFunct(h);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_CheckTimingLoop() - sets bTimingLock if timing loop is locked
******************************************************************************/
BERR_Code BAST_g3_P_CheckTimingLoop(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->passFunct = nextFunct;
   hChn->bTimingLock = false;
   BAST_g3_P_ReadRegister(h, BCHP_SDS_BL_BFOS, &(hChn->bfos));
   hChn->funct_state = 0;
   hChn->count1 = 0;
   return BAST_g3_P_CheckTimingLoopStateMachine(h);
}


/******************************************************************************
 BAST_g3_P_SignalDetectModeExit()
******************************************************************************/
BERR_Code BAST_g3_P_SignalDetectModeExit(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   if (hChn->bTimingLock)
      hChn->signalDetectStatus |= BAST_SIGNAL_DETECT_TIMING_LOCK;
   hChn->signalDetectStatus &= ~BAST_SIGNAL_DETECT_IN_PROGRESS;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_Acquire2()
******************************************************************************/
BERR_Code BAST_g3_P_Acquire2(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   if (hChn->bTimingLock)
   {
      hChn->count1 = hChn->count2 = 0;
      return hChn->acqFunct(h);
   }

   BDBG_WRN(("timing loop not locked"));
   hChn->reacqCause = BAST_ReacqCause_eTimingLoopNotLocked;
   return BAST_g3_P_Reacquire(h);
}


/******************************************************************************
 BAST_g3_P_Acquire1()
******************************************************************************/
BERR_Code BAST_g3_P_Acquire1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   
   static const uint32_t script_qpsk_0[] =
   {
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL1, ~0x000000FF, 0x00000013),
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL1, ~0x000000FF, 0x00000010),
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL2, ~0x0000FF00, 0x00007000),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_EQMISCCTL, 0x00000400),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_EQCFAD, 0x00000046),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_F0B, 0x16000000),
      BAST_SCRIPT_AND_OR(BCHP_SDS_BL_BLPCTL, ~0x000000FF, 0x00000007),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_VLCTL, 0x0000F185),
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FERR, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_PILOTCTL, 0x0),
      BAST_SCRIPT_EXIT
   };

#ifndef BAST_HAS_WFE
   BAST_CHK_RETCODE(BAST_g3_P_TunerSetFilter(h, true)); /* set tracking LPF */    
#endif

   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_0));
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SIGNAL_DETECT_MODE)
   {
      /* signal detect mode */
      retCode = BAST_g3_P_CheckTimingLoop(h, BAST_g3_P_SignalDetectModeExit);
   }
   else if (hChn->miscCtl & BAST_G3_CONFIG_MISC_CTL_CHECK_TIMING_LOOP)
      retCode = BAST_g3_P_CheckTimingLoop(h, BAST_g3_P_Acquire2);
   else
   {
      hChn->bTimingLock = true;
      retCode = BAST_g3_P_Acquire2(h);
   }
   
   done:
   return retCode;
}
  

/******************************************************************************
 BAST_g3_P_Acquire0()
******************************************************************************/  
BERR_Code BAST_g3_P_Acquire0(BAST_ChannelHandle h)
{ 
   static const uint32_t script_acq_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_CLFBCTL, 0x00000002), /* turn off fwd-bckwd loop */
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_CLDAFECTL, 0x00010001),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_VCOS, 0x00004000),
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNRCTL, 0x01),
      BAST_SCRIPT_AND(BCHP_SDS_CL_CLCTL2, 0x00FFFFFF), /* CLOON=0x00 */
      BAST_SCRIPT_OR(BCHP_SDS_VIT_VTCTL, 0x00004000), /* reset viterbi block */
      BAST_SCRIPT_AND(BCHP_SDS_VIT_VTCTL, 0xFFFF00FF), /* VTCTL2=0x00 */
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FECTL, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FECTL, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_EQFFECTL, 0x22000250), /* need to unfreeze EQ for Legacy, was 0x00000740 */
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_VLCI, 0x08000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_VLCQ, 0x08000000),
#if 1      
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_CLCTL1, 0x0C8A1073), /* orig:0x008A1073 */
#else  
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_CLCTL1, 0x0C881073), /* close loop in the receiver */
#endif      
      BAST_SCRIPT_AND(BCHP_SDS_CL_CLCTL1, ~0x00000003), /* clear carrier loop reset bit */
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_CLCTL2, 0x00000003),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_CLFFCTL, 0x02),  /* bypass fine mixer */
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_PLDCTL, 0x0000800a),   /* bypass rescrambler/descrambler */
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_PLC, 0x06080518),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLTD, 0x20000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLI, 0x00000000),
      /* read only: BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLPA, 0x00000000), */
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_PLTD, 0x20000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_PLI, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_PLPA, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_HDQPSK, 0x01000000),
      BAST_SCRIPT_EXIT
   };
          
   static const uint32_t script_acq_2[] =
   {      
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLLC, 0x01000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLIC, 0x01000000),
      BAST_SCRIPT_EXIT
   };
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t ffe_main_tap, i, val, eqcfad, f0b;   
   BERR_Code retCode;
   
   BAST_CHK_RETCODE(BAST_g3_P_ConfigBl(h));
   BAST_CHK_RETCODE(BAST_g3_P_ConfigFe(h));

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_TUNER_TEST_MODE)
   {
#ifndef BAST_HAS_WFE      
      BAST_CHK_RETCODE(BAST_g3_P_TunerSetFilter(h, true)); /* set tracking LPF */
#endif
      hChn->acqState = BAST_AcqState_eIdle;
      return BERR_SUCCESS; /* exit the acquisition */
   }
 
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_acq_1));
      
   if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
   {
      val = 0;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLFFCTL, &val);
   }
   BAST_CHK_RETCODE(BAST_g3_P_SetFfeMainTap(h));
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQFFECTL, &val);
   ffe_main_tap = (val >> 16) & 0x1F;     
   for (i = 0; i < 24; i++)
   {
      val = 0x00;
      if (i == ffe_main_tap)
         val = 0x16; /*0x18*/

      eqcfad = 0x40 | i;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_EQCFAD, &eqcfad);

      f0b = (val & 0xFF) << 24;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_F0B, &f0b);
   }   
   BAST_g3_P_ToggleBit(h, BCHP_SDS_EQ_EQFFECTL, 1); /* reset the eq */  
   
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_acq_2));
   BAST_CHK_RETCODE(BAST_g3_P_ConfigPlc(h, true)); /* set acquisition plc */  

   if (hChn->peakScanStatus & BAST_PEAK_SCAN_STATUS_ENABLED)
      return BAST_g3_P_DftPeakScan(h);
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_CLCTL1, &val);
   val &= 0xFFFFFF00;
   val |= 0x0000001F;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLCTL1, &val);
   val &= 0xFFFFFF00;
   val |= 0x0000001C;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLCTL1, &val);     

   /* enable I/Q phase detector, enable baud recovery loop */   
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_BL_BLPCTL, ~0x000000FF, 0x00000007);
    
   retCode = BAST_g3_P_DftSearchCarrier(h, BAST_g3_P_Acquire1);

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_Acquire0()
******************************************************************************/  
BERR_Code BAST_g3_P_Acquire(BAST_ChannelHandle h)
{      
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BAST_DEBUG_ACQ(BDBG_MSG(("BAST_g3_P_Acquire()")));

   /* check for bypass acquire option */
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_BYPASS_ACQUIRE)
   {
      hChn->acqState = BAST_AcqState_eIdle;   
      return BERR_SUCCESS;
   }

   hChn->acqState = BAST_AcqState_eAcquiring;
   hChn->reacqCtl &= (BAST_G3_CONFIG_REACQ_CTL_FREQ_DRIFT | BAST_G3_CONFIG_REACQ_CTL_RETUNE);
   hChn->timeSinceStableLock = 0;

   BAST_CHK_RETCODE(BAST_g3_P_GetActualMode(h, &(hChn->actualMode)));   
   BAST_CHK_RETCODE(BAST_g3_P_ConfigAgc(h));
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 7000, BAST_g3_P_Acquire0);  /* AGC delay to get the stable DFT output power at the beginning of DFT, esp. under ACI  */
   
   done:
   return retCode;
}   


/******************************************************************************
 BAST_g3_P_GetActualMode()
******************************************************************************/
BERR_Code BAST_g3_P_GetActualMode(BAST_ChannelHandle h, BAST_Mode *pActualMode)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t fmod, vst, i;
   
   *pActualMode = BAST_Mode_eUnknown;

   if ((hChn->acqState == BAST_AcqState_eIdle) && (hChn->acqParams.mode == BAST_Mode_eUnknown))
      goto done;
      
   if ((hChn->acqParams.mode == BAST_Mode_eDvb_scan) || 
       (hChn->acqParams.mode == BAST_Mode_eDss_scan) ||
       (hChn->acqParams.mode == BAST_Mode_eDcii_scan))
      goto legacy_mode;
      
   if ((hChn->acqState == BAST_AcqState_eTuning) || (hChn->acqState == BAST_AcqState_eAcquiring))
   {
      if ((hChn->acqParams.mode == BAST_Mode_eLdpc_scan) ||
          (hChn->acqParams.mode == BAST_Mode_eTurbo_scan) ||
          (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM) ||
          (hChn->acqParams.mode == BAST_Mode_eBlindScan))
      {
         goto done;
      }
      else
      {
         *pActualMode = hChn->acqParams.mode;
      }
   }
   else if (BAST_MODE_IS_LEGACY_QPSK(hChn->acqParams.mode))
   {
      /* Legacy QPSK */
      legacy_mode:
      BAST_g3_P_ReadRegister(h, BCHP_SDS_FEC_FMOD, &fmod);
      fmod = (fmod >> 8) & 0x0C;
      if (fmod == 0x04)
         *pActualMode = BAST_Mode_eDss_1_2; /* DTV */
      else if (fmod == 0x00)
         *pActualMode = BAST_Mode_eDvb_1_2; /* DVB */
      else
         *pActualMode = BAST_Mode_eDcii_1_2; /* DCII */

      /* determine code rate */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VST, &vst);
      vst = (vst >> 24) & 0x0F;
      switch (vst)
      {
         case 0x00: /* DVB/DTV 1/2 */
         case 0x09: /* DCII 1/2 */
            i = 0;
            break;
         case 0x01: /* DVB/DTV 2/3 */
         case 0x0B: /* DCII 2/3 */
            i = 1;
            break;
         case 0x02: /* DVB 3/4 */
         case 0x0C: /* DCII 3/4 */
         case 0x04: /* DTV 6/7 */
            i = 2;
            break;
         case 0x03: /* DVB 5/6 */
         case 0x0E: /* DCII 5/6 */
            i = 3;
            break;
         case 0x05: /* DVB 7/8 */
         case 0x0F: /* DCII 7/8 */
            i = 4;
            break;
         case 0x08: /* DCII 5/11 */
            i = 5;
            break;
         case 0x0A: /* DCII 3/5 */
            i = 6;
            break;
         case 0x0D: /* DCII 4/5 */
            i = 7;
            break;
         default:
            i = 0xFF;
            break;
      }
      if (i == 0xFF)
         *pActualMode = BAST_Mode_eUnknown;
      else
         *pActualMode += i;
   }
   else if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      /* actual mode is set after modcod in ldpc scan */
      if (hChn->acqParams.mode != BAST_Mode_eLdpc_scan)
         *pActualMode = hChn->acqParams.mode;
      else
         *pActualMode = hChn->actualMode;
   }
   else if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      /* actual mode is set at start of each acquisition in turbo scan */
      if (hChn->acqParams.mode != BAST_Mode_eTurbo_scan)
         *pActualMode = hChn->acqParams.mode;
      else
         *pActualMode = hChn->actualMode;
   } 
   
   done:
   /* BAST_DEBUG_ACQ(BDBG_MSG(("BAST_g3_P_GetActualMode(): actualMode=0x%X", *pActualMode))); */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetSymbolRateError()
******************************************************************************/
BERR_Code BAST_g3_P_GetSymbolRateError(BAST_ChannelHandle h, int32_t *pSymRateError)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t bw, shift, val, P_hi, P_lo, Q_hi, Q_lo;
   int32_t bri, sval;
   
   /* determine actual symbol rate */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_BL_BLPCTL, &val);
   bw = ((val >> 9) & 0x06);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_BL_BRI, (uint32_t*)&bri);
   switch (bw)
   {
      case 0:
         shift = 6; 
         break;
      case 0x02:
         shift = 8;
         break;
      case 0x04:
         shift = 10; 
         break;
      default: /* 0x06 */
         shift = 12; 
         break;
   }

   sval = bri / (int32_t)(1 << shift);

   BAST_g3_P_ReadRegister(h, BCHP_SDS_BL_BFOS, &val);
   sval += val;
   BAST_g3_P_ReadRegister(h, BCHP_SDS_FE_FILTCTL, &val);

   sval *= (32 - (val & 0x1F));
   if ((val & 0x80) == 0) /* all 6 HBs are used */
     sval *= 2;

   BAST_MultU32U32(hChn->sampleFreq, 16777216, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, (uint32_t)sval, &Q_hi, &Q_lo);

   if (hChn->bUndersample)
      Q_lo = Q_lo << 1;
   
   *pSymRateError = (int32_t)Q_lo - (int32_t)hChn->acqParams.symbolRate;

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetCarrierErrorFli()
******************************************************************************/
void BAST_g3_P_GetCarrierErrorFli(BAST_ChannelHandle h, int32_t *pCarrierErrorFli)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   
   int32_t sval;
   uint32_t val, P_hi, P_lo, Q_hi, Q_lo;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_FLI, (uint32_t*)&sval); /* front loop enabled */
   if (sval >= 0)
      val = sval;
   else
      val = (uint32_t)-sval;
   BAST_MultU32U32(val, hChn->sampleFreq, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 1073741824, &Q_hi, &Q_lo);  /* Q=abs(fval1) */
   BAST_DivU64U32(Q_hi, Q_lo, 8, &Q_hi, &Q_lo);
   if (sval >= 0)
      *pCarrierErrorFli = (int32_t)Q_lo;
   else
      *pCarrierErrorFli = (int32_t)-Q_lo;   
   /* BDBG_MSG(("carrier_error(FLI)=%d", *pCarrierErrorFli)); */
}


/******************************************************************************
 BAST_g3_P_GetCarrierErrorPli()
******************************************************************************/
void BAST_g3_P_GetCarrierErrorPli(BAST_ChannelHandle h, int32_t *pCarrierErrorPli)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   
   int32_t sval;
   uint32_t val, P_hi, P_lo, Q_hi, Q_lo;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_PLI, (uint32_t*)&sval);
   if (sval >= 0)
      val = sval;
   else
      val = (uint32_t)-sval;
   BAST_MultU32U32(val, hChn->acqParams.symbolRate, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 1073741824, &Q_hi, &Q_lo);  /* Q=abs(fval1) */
   BAST_DivU64U32(Q_hi, Q_lo, 8, &Q_hi, &Q_lo);
   if (sval >= 0)
      *pCarrierErrorPli = (int32_t)Q_lo;
   else
      *pCarrierErrorPli = (int32_t)-Q_lo;   
   /* BDBG_ERR(("carrier_error(PLI)=%d", *pCarrierErrorPli)); */
}


/******************************************************************************
 BAST_g3_P_GetCarrierError()
******************************************************************************/
BERR_Code BAST_g3_P_GetCarrierError(BAST_ChannelHandle h, int32_t *pCarrierError)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   int32_t carrier_error_pli, carrier_error_fli, carrier_error, sval;
   uint32_t val, P_hi, P_lo, Q_hi, Q_lo, flif;
   bool bHpSpinv, bFrontLoopEnabled, bBackLoopEnabled;
   
   carrier_error = 0;
   carrier_error_pli = 0;

   bHpSpinv = false;  /* bHpSpinv=true if HP is enabled and spinv detected */
   bFrontLoopEnabled = false;
   bBackLoopEnabled = false;
   BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_HPCONTROL, &val);
   if (val & 0x08)   /* HP enabled */
   {
      BAST_g3_P_HpIsSpinv(h, &bHpSpinv);
   }

   /* check FLI and PLI for legacy modes */
   if (BAST_MODE_IS_LEGACY_QPSK(hChn->acqParams.mode))
   {
      /* check if front loop is enabled */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_HPOVERRIDE, &val);
      if (val & 0x01)
      {
         /* front loop enable not controlled by HP */
         BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_CLCTL1, &val);
         bFrontLoopEnabled = (val & 0x10) ? true : false;
      }

      /* read FLI if front loop enabled */
      if (bFrontLoopEnabled)
         BAST_g3_P_GetCarrierErrorFli(h, &carrier_error_fli);
      else
         carrier_error_fli = 0;
      carrier_error = carrier_error_fli;

      /* check if back loop is enabled */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_HPOVERRIDE, &val);
      if (val & 0x10)
      {
         /* back loop enable is not controlled by HP */
         BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_CLCTL2, &val);
         bBackLoopEnabled = (val & 0x1000) ? true : false;
      }

      /* read PLI if back loop enabled */
      if (bBackLoopEnabled)
         BAST_g3_P_GetCarrierErrorPli(h, &carrier_error_pli);
      else
         carrier_error_pli = 0;
      carrier_error += carrier_error_pli;
   }
   else
   {
      BAST_CHK_RETCODE(BAST_g3_P_HpGetFreqOffsetEstimate(h, &carrier_error));     

      if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
      {
         BAST_g3_P_GetCarrierErrorFli(h, &carrier_error_fli);
         BAST_g3_P_GetCarrierErrorPli(h, &carrier_error_pli);
         sval = carrier_error_fli + carrier_error_pli;
         if (bHpSpinv)
            carrier_error -= sval;
         else
            carrier_error += sval;
      }
   }

   /* FLIF * Fs / 2^28 */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_FLIF, &flif);
   if (flif & 0x08000000)
      flif |= 0xF0000000; /* sign extend */
   sval = (int32_t)flif;
   if (sval >= 0)
      val = (uint32_t)sval;
   else
      val = (uint32_t)-sval;
   BAST_MultU32U32(val, hChn->sampleFreq, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 268435456, &Q_hi, &Q_lo);
   if (sval >= 0)
      sval = (int32_t)Q_lo;
   else
      sval = (int32_t)-Q_lo;   
   if (bHpSpinv)
      sval = -sval; /* invert if HP is enabled and spinv detect */         
   carrier_error += sval;
/* BDBG_ERR(("FLIF=%d, carrier_error(FLIF)=%d", flif, sval)); */
/* BDBG_ERR(("tunerIfStep=%d", hChn->tunerIfStep)); */
   carrier_error -= hChn->tunerIfStep;
   carrier_error = -carrier_error; 
   *pCarrierError = carrier_error;

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_SetFlifOffset()
******************************************************************************/
BERR_Code BAST_g3_P_SetFlifOffset(BAST_ChannelHandle h, int32_t offset)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, P_hi, P_lo;
   int32_t flif;

   if (offset >= 0)
      val = offset;
   else
      val = -offset;

   BAST_MultU32U32(val, 268435456, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &P_hi, &P_lo);
   if (offset >= 0)
      flif = -P_lo;
   else
      flif = P_lo;

   val = (uint32_t)flif;
   val &= 0x0FFFFF00;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLIF, (uint32_t *)&val);
   /* BDBG_MSG(("BAST_g3_P_SetFlifOffset(): offset=%d, FLIF=0x%08X", offset, val)); */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_IsCarrierOffsetOutOfRange()
******************************************************************************/
bool BAST_g3_P_IsCarrierOffsetOutOfRange(BAST_ChannelHandle h)
{
#ifndef BAST_EXCLUDE_EXT_TUNER         
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#endif
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   int32_t carrierError;
   uint32_t val, range;
   BERR_Code retCode;
   bool bOutOfRange = false;

#ifndef BAST_EXCLUDE_EXT_TUNER         
   if (hChn->bExternalTuner == false)
#endif      
   {
      BAST_CHK_RETCODE(BAST_g3_P_GetCarrierError(h, &carrierError));
      
      if (carrierError < 0)
         val = -carrierError;
      else
         val = carrierError;

      range = (hDev->searchRange * 6) / 5;
      if (val > range)
      {
         bOutOfRange = true;
         BDBG_WRN(("carrier offset out of range (%d Hz)", carrierError));
      }
   }
   
   done:
   return bOutOfRange;
}


/******************************************************************************
 BAST_g3_P_IndicateLocked()
******************************************************************************/
void BAST_g3_P_IndicateLocked(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   hChn->bLocked = true;
   if (hChn->bLastLocked == false)
   {
      BDBG_MSG(("SDS%d locked", h->channel));
      BKNI_SetEvent(hChn->hLockChangeEvent);
   }

   hChn->bLastLocked = true;
}


/******************************************************************************
 BAST_g3_P_IndicateNotLocked()
******************************************************************************/
void BAST_g3_P_IndicateNotLocked(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   hChn->bLocked = false;
   if (hChn->bLastLocked)
   {
      BDBG_MSG(("SDS%d not locked", h->channel));   
      BKNI_SetEvent(hChn->hLockChangeEvent);
   }
   
   hChn->bLastLocked = false;
}


/******************************************************************************
 BAST_g3_P_TunerTestMode()
******************************************************************************/
BERR_Code BAST_g3_P_TunerTestMode(BAST_ChannelHandle h)
{
   BSTD_UNUSED(h);
   /* any register settings for tuner test mode goes here... */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_BlindScanSetNextMode()
******************************************************************************/
BERR_Code BAST_g3_P_BlindScanSetNextMode(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
  
   /* determine next mode to try */
   next_mode:
   while (hChn->blindScanModes)
   {
      if (hChn->blindScanCurrMode == 0)
         hChn->blindScanCurrMode = 1;
      else   
         hChn->blindScanCurrMode = (hChn->blindScanCurrMode << 1) & BAST_G3_CONFIG_BLIND_SCAN_MODES_MASK;
         
      if (hChn->blindScanCurrMode == 0)
         hChn->acqCount++;
      else if (hChn->blindScanCurrMode & hChn->blindScanModes)
         break;
   }
   
   if (hChn->blindScanCurrMode == 0)
   {
      BDBG_ERR(("BAST_g3_P_BlindScanSetNextMode(): blindScanCurrMode should not be 0"));
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      return retCode;
   }
   
   if (hChn->blindScanCurrMode & BAST_G3_CONFIG_BLIND_SCAN_MODES_DVB)
   {
      BDBG_MSG(("Blind scan: trying DVB scan"));
      hChn->acqParams.mode = BAST_Mode_eDvb_scan;
   }
   else if (hChn->blindScanCurrMode & BAST_G3_CONFIG_BLIND_SCAN_MODES_DTV)
   {
      BDBG_MSG(("Blind scan: trying DTV scan"));   
      hChn->acqParams.mode = BAST_Mode_eDss_scan;
   }
   else if (hChn->blindScanCurrMode & BAST_G3_CONFIG_BLIND_SCAN_MODES_DCII)
   {
      BDBG_MSG(("Blind scan: trying DCII scan"));     
      hChn->acqParams.mode = BAST_Mode_eDcii_scan;
   }
   else if (hChn->blindScanCurrMode & BAST_G3_CONFIG_BLIND_SCAN_MODES_LDPC)
   {
#ifndef BAST_EXCLUDE_LDPC          
      if (hChn->bHasAfec)
      {
         BDBG_MSG(("Blind scan: trying LDPC scan"));    
         hChn->acqParams.mode = BAST_Mode_eLdpc_scan;
      }
      else
#endif         
      {
         /* remove LDPC from blind scan because this channel does not have an AFEC */
         hChn->blindScanModes &= ~BAST_G3_CONFIG_BLIND_SCAN_MODES_LDPC;
         goto next_mode;
      }
   }
   else if (hChn->blindScanCurrMode & BAST_G3_CONFIG_BLIND_SCAN_MODES_TURBO)
   {
      if (hChn->bHasTfec)
      {
         BDBG_MSG(("Blind scan: trying Turbo scan"));    
         hChn->acqParams.mode = BAST_Mode_eTurbo_scan;
      }
      else
      {
         /* remove Turbo from blind scan because this channel does not have a TFEC */
         hChn->blindScanModes &= ~BAST_G3_CONFIG_BLIND_SCAN_MODES_TURBO;
         goto next_mode;
      }
   }
   else
   {
      BDBG_ERR(("BAST_g3_P_BlindScanSetNextMode(): invalid value for blindScanCurrMode (0x%X)", hChn->blindScanCurrMode));
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      return retCode;
   }
   
   retCode = BAST_g3_P_SetFunctTable(h);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_BlindScanInit()
******************************************************************************/
BERR_Code BAST_g3_P_BlindScanInit(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);

   hChn->bBlindScan = true;
   hChn->blindScanCurrMode = 0;
   return BAST_g3_P_BlindScanSetNextMode(h);
}


/******************************************************************************
 BAST_g3_P_BlindReacquire()
******************************************************************************/
BERR_Code BAST_g3_P_BlindReacquire(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->tunerIfStep = 0;
   hChn->acqState = BAST_AcqState_eTuning;  
   hChn->acqParams.carrierFreqOffset = 0;            
   return BAST_g3_P_TunerSetFreq(h, BAST_g3_P_BlindScanSetNextMode);
}


/******************************************************************************
 BAST_g3_P_ResetAcquisitionTimer() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_ResetAcquisitionTimer(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eGen3, 0xFFFFFFFF, NULL);   
   hChn->acqTime = 0;
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetAcquisitionTimerValue()
******************************************************************************/
void BAST_g3_P_GetAcquisitionTimerValue(BAST_ChannelHandle h, uint32_t *pVal)
{
   uint32_t lval1;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_MISC_GENTMR3, &lval1);
   *pVal = (0xFFFFFFFF - lval1);
}


/******************************************************************************
 BAST_g3_P_ClearTraceBuffer()
******************************************************************************/
BERR_Code BAST_g3_P_ClearTraceBuffer(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t i;
   
   for (i = 0; i < BAST_TraceEvent_eMax; i++)
      hChn->trace[i] = 0;
      
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LogTraceBuffer()
******************************************************************************/
BERR_Code BAST_g3_P_LogTraceBuffer(BAST_ChannelHandle h, BAST_TraceEvent event)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   if (event < BAST_TraceEvent_eMax)
   {
      BAST_g3_P_GetAcquisitionTimerValue(h, &(hChn->trace[event]));
      return BERR_SUCCESS;
   }
   else
      return BERR_INVALID_PARAMETER;
}


/******************************************************************************
 BAST_g3_P_IncrementInterruptCounter()
******************************************************************************/
void BAST_g3_P_IncrementInterruptCounter(BAST_ChannelHandle h, BAST_g3_IntID idx)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   hChn->irqCount[idx]++;
}


/******************************************************************************
 BAST_g3_P_UpdateErrorCounters()
******************************************************************************/
BERR_Code BAST_g3_P_UpdateErrorCounters(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t frc, ferc, berc;
   
   /* take snapshot of mpeg frame and error counter */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_OI_OIFCTL01, 0x00400000);

   BAST_g3_P_ReadRegister(h, BCHP_SDS_OI_FRC, &frc);      
   hChn->mpegFrameCount += frc;      
   BAST_g3_P_ReadRegister(h, BCHP_SDS_OI_FERC, &ferc);
   hChn->mpegErrorCount += ferc;
    
   BAST_g3_P_ReadRegister(h, BCHP_SDS_BERT_BERC, &berc);
   hChn->berErrors += berc;  
 
   return BERR_SUCCESS; 
}


/******************************************************************************
 BAST_g3_P_ResetErrorCounters()
******************************************************************************/
BERR_Code BAST_g3_P_ResetErrorCounters(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BAST_CHK_RETCODE(BAST_g3_P_UpdateErrorCounters(h));
   BAST_CHK_RETCODE(BAST_g3_P_QpskUpdateErrorCounters(h));

#ifndef BAST_EXCLUDE_LDPC   
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      BAST_CHK_RETCODE(BAST_g3_P_LdpcUpdateBlockCounters(h));
   }
#endif   
#ifndef BAST_EXCLUDE_TURBO
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      BAST_CHK_RETCODE(BAST_g3_P_TurboUpdateErrorCounters(h));
   }
#endif

   hChn->berErrors = 0;
   hChn->mpegErrorCount = 0;
   hChn->mpegFrameCount = 0;
#ifndef BAST_EXCLUDE_LDPC   
   hChn->ldpcBadBlocks = 0;
   hChn->ldpcCorrBlocks = 0;
   hChn->ldpcTotalBlocks = 0;
#endif   
   hChn->rsCorr = 0;
   hChn->rsUncorr = 0;
   hChn->preVitErrors = 0;
#ifndef BAST_EXCLUDE_TURBO    
   hChn->turboBadBlocks = 0;
   hChn->turboCorrBlocks = 0;
   hChn->turboTotalBlocks = 0;
#endif   
   
   done:
   return retCode;
}


#ifndef BAST_EXCLUDE_STATUS_EVENTS
/******************************************************************************
 BAST_g3_P_CheckStatusIndicators()
******************************************************************************/
BERR_Code BAST_g3_P_CheckStatusIndicators(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   bool bSetEvent = false;   
   int32_t offset;
   uint32_t diff, snr, snr8, val, snrThreshold1, snrThreshold2;
   int idx;
   
   static const uint16_t BAST_SNR_THRESHOLD1[] = /* in units of 1/256 dB */
   {
      822,  /* 0: DVB 1/2 = 3.21 */
      1308, /* 1: DVB 2/3 = 5.109375 */
      1567, /* 2: DVB 3/4 = 6.1211 */
      1817, /* 3: DVB 5/6 = 7.09766 */
      2017, /* 4: DVB 7/8 = 7.8789 */
      15 * 256, /* 5: DTV 1/2 */
      15 * 256, /* 6: DTV 2/3 */
      15 * 256, /* 7: DTV 6/7 */
      15 * 256, /* 8: DCII 1/2 */
      15 * 256, /* 9: DCII 2/3 */
      15 * 256, /* 10: DCII 3/4 */
      15 * 256, /* 11: DCII 5/6 */
      15 * 256, /* 12: DCII 7/8 */
      15 * 256, /* 13: DCII 5/11 */
      15 * 256, /* 14: DCII 3/5 */
      15 * 256, /* 15: DCII 4/5 */
      15 * 256, /* 16: LDPC QPSK 1/2 */
      15 * 256, /* 17: LDPC QPSK 3/5 */
      15 * 256, /* 18: LDPC QPSK 2/3 */
      15 * 256, /* 19: LDPC QPSK 3/4 */
      15 * 256, /* 20: LDPC QPSK 4/5 */
      15 * 256, /* 21: LDPC QPSK 5/6 */
      15 * 256, /* 22: LDPC QPSK 8/9 */
      15 * 256, /* 23: LDPC QPSK 9/10 */      
      15 * 256, /* 24: LDPC 8PSK 3/5 */
      15 * 256, /* 25: LDPC 8PSK 2/3 */
      15 * 256, /* 26: LDPC 8PSK 3/4 */
      15 * 256, /* 27: LDPC 8PSK 5/6 */
      15 * 256, /* 28: LDPC 8PSK 8/9 */
      15 * 256, /* 29: LDPC 8PSK 9/10 */     
      660,  /* 30: Turbo QPSK 1/2 = 2.578125 */  
      1173, /* 31: Turbo QPSK 2/3 = 4.582 */  
      1440, /* 32: Turbo QPSK 3/4 = 5.625 */ 
      1717, /* 33: Turbo QPSK 5/6 = 6.707 */  
      1892, /* 34: Turbo QPSK 7/8 = 7.390625 */  
      2094, /* 35: Turbo 8PSK 2/3 = 8.1797 */  
      2299, /* 36: Turbo 8PSK 3/4 = 8.98047 */  
      2424, /* 37: Turbo 8PSK 4/5 = 9.46875 */  
      2799, /* 38: Turbo 8PSK 5/6 = 10.9336 */  
      2924  /* 39: Turbo 8PSK 8/9 = 11.4219 */  
   };
   
   static const uint16_t BAST_SNR_THRESHOLD2[] = /* in units of 1/256 dB */
   {
      552,  /* 0: DVB 1/2 = 2.15625 */
      1038, /* 1: DVB 2/3 = 4.0547 */
      1308, /* 2: DVB 3/4 = 5.109375 */
      1567, /* 3: DVB 5/6 = 6.1211 */
      1767, /* 4: DVB 7/8 = 6.90234 */
      10 * 256, /* 5: DTV 1/2 */
      10 * 256, /* 6: DTV 2/3 */
      10 * 256, /* 7: DTV 6/7 */
      10 * 256, /* 8: DCII 1/2 */
      10 * 256, /* 9: DCII 2/3 */
      10 * 256, /* 10: DCII 3/4 */
      10 * 256, /* 11: DCII 5/6 */
      10 * 256, /* 12: DCII 7/8 */
      10 * 256, /* 13: DCII 5/11 */
      10 * 256, /* 14: DCII 3/5 */
      10 * 256, /* 15: DCII 4/5 */      
      10 * 256, /* 16: LDPC QPSK 1/2 */
      10 * 256, /* 17: LDPC QPSK 3/5 */
      10 * 256, /* 18: LDPC QPSK 2/3 */
      10 * 256, /* 19: LDPC QPSK 3/4 */
      10 * 256, /* 20: LDPC QPSK 4/5 */
      10 * 256, /* 21: LDPC QPSK 5/6 */
      10 * 256, /* 22: LDPC QPSK 8/9 */
      10 * 256, /* 23: LDPC QPSK 9/10 */      
      10 * 256, /* 24: LDPC 8PSK 3/5 */
      10 * 256, /* 25: LDPC 8PSK 2/3 */
      10 * 256, /* 26: LDPC 8PSK 3/4 */
      10 * 256, /* 27: LDPC 8PSK 5/6 */
      10 * 256, /* 28: LDPC 8PSK 8/9 */
      10 * 256, /* 29: LDPC 8PSK 9/10 */     
      400,  /* 30: Turbo QPSK 1/2 = 1.5625 */  
      903,  /* 31: Turbo QPSK 2/3 = 3.5273 */  
      1173, /* 32: Turbo QPSK 3/4 = 4.582 */ 
      1466, /* 33: Turbo QPSK 5/6 = 5.7266 */  
      1642, /* 34: Turbo QPSK 7/8 = 6.4140625 */  
      1842, /* 35: Turbo 8PSK 2/3 = 7.1953 */  
      2042, /* 36: Turbo 8PSK 3/4 = 7.97656 */  
      2172, /* 37: Turbo 8PSK 4/5 = 8.484375 */  
      2549, /* 38: Turbo 8PSK 5/6 = 9.957 */  
      2674  /* 39: Turbo 8PSK 8/9 = 10.4453 */     
   };
   
   if (hChn->bStatusInterrupts == false)
      return BERR_SUCCESS;
      
   /* check freq drift threshold */ 
   BAST_CHK_RETCODE(BAST_g3_P_GetCarrierError(h, &offset));
      
   if (offset > hChn->initFreqOffset)
      diff = (uint32_t)(offset - hChn->initFreqOffset);
   else
      diff = (uint32_t)(hChn->initFreqOffset - offset);
   if (diff > hChn->freqDriftThreshold)
   {
      if ((hChn->statusEventIndicators & BAST_G3_CONFIG_STATUS_INDICATOR_FREQ_DRIFT) == 0)
      {
         /* BDBG_MSG(("ch %d freq drift detected", h->channel)); */
         hChn->statusEventIndicators |= BAST_G3_CONFIG_STATUS_INDICATOR_FREQ_DRIFT;
         bSetEvent = true;
      }
   }
   else
      hChn->statusEventIndicators &= ~BAST_G3_CONFIG_STATUS_INDICATOR_FREQ_DRIFT;
      
   BAST_CHK_RETCODE(BAST_g3_P_GetSnr(h, &snr)); /* snr is in 1/256 dB */
   
   /* determine threshold 1 and 2 values from mode */
   if (BAST_MODE_IS_DVB(hChn->actualMode))
      idx = hChn->actualMode - BAST_Mode_eDvb_1_2;
   else if (BAST_MODE_IS_DTV(hChn->actualMode))
      idx = 5 + hChn->actualMode - BAST_Mode_eDss_1_2;
   else if (BAST_MODE_IS_DCII(hChn->actualMode))
      idx = 8 + hChn->actualMode - BAST_Mode_eDcii_1_2;
   else if (BAST_MODE_IS_LDPC(hChn->actualMode))
      idx = 16 + (hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2);
   else if (BAST_MODE_IS_TURBO(hChn->actualMode))
      idx = 30 + (hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2);
   else
   {
      snrThreshold1 = snrThreshold2 = 0;
      goto check_snr_threshold;
   }
   
   /* BDBG_MSG(("snrThreshold_idx=%d (actualMode=%d)", idx, hChn->actualMode)); */
   snrThreshold1 = BAST_SNR_THRESHOLD1[idx];
   snrThreshold2 = BAST_SNR_THRESHOLD2[idx];
   
   /* check SNR threshold 1 */
   check_snr_threshold:
   if (snr < snrThreshold1)
   {
      if ((hChn->statusEventIndicators & BAST_G3_CONFIG_STATUS_INDICATOR_THRESHOLD1) == 0)
      {
         /* BDBG_MSG(("ch %d CNR threshold1 detected", h->channel)); */
         hChn->statusEventIndicators |= BAST_G3_CONFIG_STATUS_INDICATOR_THRESHOLD1;
         bSetEvent = true;        
      }
   }
   else
      hChn->statusEventIndicators &= ~BAST_G3_CONFIG_STATUS_INDICATOR_THRESHOLD1;
      
   /* check SNR threshold 2 */
   if (snr < snrThreshold2)
   {
      if ((hChn->statusEventIndicators & BAST_G3_CONFIG_STATUS_INDICATOR_THRESHOLD2) == 0)
      {
         /* BDBG_MSG(("ch %d CNR threshold2 detected", h->channel)); */
         hChn->statusEventIndicators |= BAST_G3_CONFIG_STATUS_INDICATOR_THRESHOLD2;
         bSetEvent = true;        
      }
   }
   else
      hChn->statusEventIndicators &= ~BAST_G3_CONFIG_STATUS_INDICATOR_THRESHOLD2;
      
   /* check for rain fade */
   snr8 = (snr + 128) >> 5; /* snr8=snr in 1/8 dB */
   if (hChn->rainFadeSnrAve == 0)
   {
      hChn->rainFadeSnrAve = snr8;
      hChn->rainFadeSnrMax = snr8;
   }
   else
   {
      /* compute ave snr in window */
      if (snr8 > hChn->rainFadeSnrAve)
         val = snr8 - hChn->rainFadeSnrAve;
      else
         val = hChn->rainFadeSnrAve - snr8;
      val = val / hChn->rainFadeWindow;
      if (snr8 > hChn->rainFadeSnrAve)
         hChn->rainFadeSnrAve += val;
      else
         hChn->rainFadeSnrAve -= val;
         
      if (snr8 > hChn->rainFadeSnrMax)
         hChn->rainFadeSnrMax = snr8; /* set new max */
      else
         hChn->rainFadeSnrMax -= ((hChn->rainFadeSnrMax - hChn->rainFadeSnrAve) / hChn->rainFadeWindow); /* decay max snr */         
         
      if (snr8 < (hChn->rainFadeSnrMax - (uint32_t)(hChn->rainFadeThreshold)))
      {
         if ((hChn->statusEventIndicators & BAST_G3_CONFIG_STATUS_INDICATOR_RAIN_FADE) == 0)
         {
            hChn->statusEventIndicators |= BAST_G3_CONFIG_STATUS_INDICATOR_RAIN_FADE;
            bSetEvent = true;
         }
      }
      else
         hChn->statusEventIndicators &= ~BAST_G3_CONFIG_STATUS_INDICATOR_RAIN_FADE;
   }
   /* BDBG_MSG(("rain_fade: ave=0x%08X, max=0x%08X, curr=0x%08X", hChn->rainFadeSnrAve, hChn->rainFadeSnrMax, snr8)); */
   
   if (bSetEvent)
   {
      /* BDBG_MSG(("set status event")); */
      BKNI_SetEvent(hChn->hStatusEvent);
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_ResetStatusIndicators()
******************************************************************************/
BERR_Code BAST_g3_P_ResetStatusIndicators(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->statusEventIndicators = 0;
   hChn->rainFadeSnrAve = 0;
   hChn->rainFadeSnrMax = 0;
   return BERR_SUCCESS;
}
#endif


/******************************************************************************
 BAST_g3_P_SdsPowerDownOpll() - power down the OPLL core
******************************************************************************/ 
void BAST_g3_P_SdsPowerDownOpll(BAST_ChannelHandle h)
{
   BAST_g3_P_OrRegister(h, BCHP_SDS_OI_OPLL_PWRDN_RST, 0x4);  
}


/******************************************************************************
 BAST_g3_P_SdsPowerUpOpll() - power up the OPLL core
******************************************************************************/ 
void BAST_g3_P_SdsPowerUpOpll(BAST_ChannelHandle h)
{
   BAST_g3_P_AndRegister(h, BCHP_SDS_OI_OPLL_PWRDN_RST, ~0x4);    
}


/******************************************************************************
 BAST_g3_P_SdsDisableOif() - turn off oif 
******************************************************************************/ 
BERR_Code BAST_g3_P_SdsDisableOif(BAST_ChannelHandle h)
{
   BAST_g3_P_OrRegister(h, BCHP_SDS_OI_OIFCTL00, 0x00080001);
   BAST_g3_P_AndRegister(h, BCHP_SDS_OI_OIFCTL00, ~0x1);
   BAST_g3_P_SdsPowerDownOpll(h);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_PrepareNewAcquisition() - non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_PrepareNewAcquisition(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   
   static const uint32_t script_reset_channel[] =
   {
#ifndef BAST_EXCLUDE_TURBO   
#if 1 /* dynamically turn on/off TFEC clock as needed */
      BAST_SCRIPT_OR(BCHP_TFEC_MISC_POST_DIV_CTL, 0x00000004), /* disable tfec clock */
#endif      
#endif  
#ifndef BAST_EXCLUDE_LDPC
      BAST_SCRIPT_OR(BCHP_AFEC_GLOBAL_CLK_CNTRL, 0x00800000), /* disable afec clock */   
#endif      
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FECTL, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FECTL, 0x00),
      BAST_SCRIPT_EXIT
   };
   
   /* reset last locked status */
   hChn->bLastLocked = false;
   
   BAST_CHK_RETCODE(BAST_g3_P_DisableChannelInterrupts(h, false, false));
   BKNI_EnterCriticalSection();
   retCode = BAST_g3_P_HpEnable(h, false);
   BKNI_LeaveCriticalSection();
   if (retCode != BERR_SUCCESS) goto done;
   
   BAST_CHK_RETCODE(BAST_g3_P_AbortAcq(h));
   BAST_CHK_RETCODE(BAST_g3_P_ResetStatus(h));
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_reset_channel));
   
#ifndef BAST_EXCLUDE_STATUS_EVENTS     
   BAST_CHK_RETCODE(BAST_g3_P_ResetStatusIndicators(h));
#endif

   BAST_g3_P_ClearTraceBuffer(h);
   hChn->bBlindScan = false;
   hChn->initFreqOffset = 0;
   hChn->dftFreqEstimateStatus = 0;   
   hChn->bEverStableLock = false;
   hChn->bStableLock = false;
   hChn->signalDetectStatus = 0;
   hChn->peakScanStatus = 0;
   hChn->lockIsrFlag = 0;
#ifndef BAST_EXCLUDE_TURBO    
   hChn->turboScanCurrMode = 0;
#endif  

   done:
   return retCode;
}


#ifndef BAST_HAS_WFE
/******************************************************************************
 BAST_g3_P_SdsPowerUp() - power up the sds core
******************************************************************************/ 
BERR_Code BAST_g3_P_SdsPowerUp(BAST_ChannelHandle h)
{
   BSTD_UNUSED(h);
   /*BAST_g3_P_AndRegister(h, BCHP_SDS_CG_SPLL_MDIV_CTRL, ~0x00000400);*/  /* TBD power up demod */
   /*BAST_g3_P_AndRegister(h, BCHP_SDS_CG_SPLL_PWRDN_RST, ~0x00000004);*/   /* TBD power up spll */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SdsPowerDown() - power down the sds core
******************************************************************************/ 
BERR_Code BAST_g3_P_SdsPowerDown(BAST_ChannelHandle h)
{   
   BERR_Code retCode;
   
   /* turn off oif */
   retCode = BAST_g3_P_SdsDisableOif(h);
   
   /*BAST_g3_P_OrRegister(h, BCHP_SDS_CG_SPLL_MDIV_CTRL, 0x00000400);*/  /* TBD power down demod - affects refPLL */
   /*BAST_g3_P_OrRegister(h, BCHP_SDS_CG_SPLL_PWRDN_RST, 0x00000004);*/  /* TBD power down spll - affects refPLL */
   
   return retCode;   
}
#endif


/******************************************************************************
 BAST_g3_P_SetFunctTable()
******************************************************************************/ 
BERR_Code BAST_g3_P_SetFunctTable(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

#ifndef BAST_EXCLUDE_LDPC   
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      retCode = BAST_g3_P_LdpcSetFunctTable(h);
      goto done;
   }
#endif   
#ifndef BAST_EXCLUDE_TURBO      
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      retCode = BAST_g3_P_TurboSetFunctTable(h);
      goto done;
   }
#endif      
   if (BAST_MODE_IS_LEGACY_QPSK(hChn->acqParams.mode))
      retCode = BAST_g3_P_QpskSetFunctTable(h);
   else
   {
      BDBG_ERR(("BAST_g3_P_SetFunctTable(): invalid mode (%d)", hChn->acqParams.mode));
      BDBG_ASSERT(0);
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_ResetLockFilter()
******************************************************************************/
BERR_Code BAST_g3_P_ResetLockFilter(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi;
   
   hChn->stableLockTimeout = BAST_MODE_IS_LEGACY_QPSK(hChn->actualMode) ? 3000 : 5000;
   hChn->maxStableLockTimeout = 200000;   
   hChn->lockFilterRamp = -2;
   hChn->lockFilterIncr = 5000;
   
   if (hChn->acqParams.symbolRate < 20000000)
   {
      /* scale to symbol rate */
      BAST_MultU32U32(20000000, hChn->stableLockTimeout, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &(hChn->stableLockTimeout));
      BAST_MultU32U32(20000000, hChn->maxStableLockTimeout, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &(hChn->maxStableLockTimeout));      
      BAST_MultU32U32(20000000, hChn->lockFilterIncr, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &(hChn->lockFilterIncr));        
   }
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SmartTune()
******************************************************************************/
BERR_Code BAST_g3_P_SmartTune(BAST_ChannelHandle h)
{
   BERR_Code retCode = BERR_SUCCESS;

#ifndef BAST_HAS_WFE   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;   
   bool bUseDefaultFs = true;
   uint32_t range_low, range_high, x, harmonic, Fs_expected, diff;

   if (hChn->miscCtl & BAST_G3_CONFIG_MISC_CTL_DISABLE_SMART_TUNE)
      return BERR_SUCCESS;
      
   x = hDev->searchRange + ((((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_NYQUIST_20) ? 60 : 68) * hChn->acqParams.symbolRate) / 100);
   
   harmonic = BAST_DEFAULT_SAMPLE_FREQ * 6;
   range_low = harmonic - x;
   range_high = harmonic + x;
   if ((hChn->tunerFreq > range_low) && (hChn->tunerFreq < range_high))
      bUseDefaultFs = false;
   else
   {
      harmonic = BAST_DEFAULT_SAMPLE_FREQ * 8;
      range_low = harmonic - x;
      range_high = harmonic + x;
      if ((hChn->tunerFreq > range_low) && (hChn->tunerFreq < range_high))
         bUseDefaultFs = false;   
   }

   /* BDBG_MSG(("bUseDefaultFs=%d, hChn->bFsNotDefault=%d", bUseDefaultFs, hChn->bFsNotDefault)); */
   if (bUseDefaultFs)
   {
      Fs_expected = BAST_DEFAULT_SAMPLE_FREQ;
      retCode = BAST_g3_P_SetDefaultSampleFreq(h);
   }
   else
   {
      Fs_expected = 189000000UL;
      retCode = BAST_g3_P_SetSampleFreq(h, 84, 6); /* ndiv=84, mdiv=6, Fs=189MHz */ 
   }

   if (Fs_expected <= hChn->sampleFreq)
      diff = hChn->sampleFreq - Fs_expected;
   else
      diff = Fs_expected - hChn->sampleFreq;
   if (diff > 1000)
   {
      BDBG_ERR(("BAST_g3_P_SmartTune: bad Fs (expected=%u, read=%u)", Fs_expected, hChn->sampleFreq));
      retCode = BAST_ERR_SAMPLE_FREQ;     
   }
#else
   /* Fs smart tune doesn't apply to chips with WFE */
   BSTD_UNUSED(h);
#endif

   return retCode;
}


/******************************************************************************
 BAST_g3_P_TuneAcquire_isr() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TuneAcquire_isr(BAST_ChannelHandle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
#ifndef BAST_EXCLUDE_TURBO   
   uint32_t spinv;
#endif

   BAST_g3_P_ResetLockFilter(h);
   BAST_g3_P_ResetAcquisitionTimer(h);
   hChn->reacqCause = BAST_ReacqCause_eOK;
   
#ifndef BAST_EXCLUDE_TURBO   
   hChn->turboScanState = BAST_TURBO_SCAN_STATE_FIRST_TIME;
   spinv = hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SPINV_IQ_SCAN;
   if ((spinv == BAST_ACQSETTINGS_SPINV_Q_INV) || (spinv == BAST_ACQSETTINGS_SPINV_I_INV))
      hChn->bTurboSpinv = true;
   else   
      hChn->bTurboSpinv = false;
#endif

   retCode = BAST_g3_P_SmartTune(h);
   if (retCode != BERR_SUCCESS)
      return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100, BAST_g3_P_TuneAcquire_isr); /* retry */

   BAST_g3_P_UpdateUndersampleMode(h);
#ifndef BAST_EXCLUDE_SPUR_CANCELLER
   BAST_CHK_RETCODE(BAST_g3_P_DisableSpurCanceller(h));
#endif   
   
   if ((hChn->peakScanStatus & BAST_PEAK_SCAN_STATUS_ENABLED) == 0)
   {
      if (BAST_MODE_IS_BLIND_SCAN(hChn->acqParams.mode))
         retCode = BAST_g3_P_BlindScanInit(h);
      else
         retCode = BAST_g3_P_SetFunctTable(h);
      if (retCode != BERR_SUCCESS)
         goto done;
   }

#ifndef BAST_EXCLUDE_EXT_TUNER            
   if (hChn->bExternalTuner)
      retCode = BAST_g3_P_Acquire(h);
   else
#endif      
   {
      hChn->tunerIfStep = 0;
      
      /* check for bypass tune option */
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_BYPASS_TUNE)
         retCode = BAST_g3_P_Acquire(h);
      else
         retCode = BAST_g3_P_TunerSetFreq(h, BAST_g3_P_Acquire);
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_Reacquire() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_Reacquire(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
#ifndef BAST_HAS_WFE   
   bool bRefPllLocked, bMixPllLocked;
#endif
   
   BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eReacquire));
   BAST_DEBUG_ACQ(BDBG_MSG(("BAST_g3_P_Reacquire()")));
   
   hChn->bStableLock = false;   
   BAST_g3_P_IndicateNotLocked(h);
   
   /* disable lock/lost_lock interrupts */
   BAST_CHK_RETCODE(hChn->disableLockInterrupts(h));
   BAST_CHK_RETCODE(BAST_g3_P_DisableChannelInterrupts_isr(h, false, false));     

   if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_REACQ_DISABLE) &&
       (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_PSOF))
      goto label1;
   
   BAST_CHK_RETCODE(BAST_g3_P_SdsDisableOif(h));
   BAST_CHK_RETCODE(BAST_g3_P_HpEnable(h, false));   
#ifndef BAST_EXCLUDE_STATUS_EVENTS   
   BAST_CHK_RETCODE(BAST_g3_P_ResetStatusIndicators(h));
#endif
#ifndef BAST_EXCLUDE_SPUR_CANCELLER
   BAST_CHK_RETCODE(BAST_g3_P_DisableSpurCanceller(h));
#endif   

   label1:
   if (hChn->bBlindScan)
   {
#ifndef BAST_EXCLUDE_TURBO    
      if ((hChn->acqParams.mode != BAST_Mode_eTurbo_scan) || 
          ((hChn->acqParams.mode == BAST_Mode_eTurbo_scan) && (hChn->turboScanCurrMode == 0)))
#endif          
      {
         BAST_CHK_RETCODE(BAST_g3_P_BlindScanSetNextMode(h));
      }
   }
   else
   {
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_REACQ_DISABLE)
      {
         hChn->acqState = BAST_AcqState_eIdle;
         BDBG_MSG(("failed to acquire"));
         return retCode;
      }

      if (hChn->acqParams.mode != BAST_Mode_eTurbo_scan)
         hChn->acqCount++;
      
      if ((hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_RETUNE) && ((hChn->acqCount & 1) == 0))
         hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE_RETUNE;      
   }
   
#ifndef BAST_HAS_WFE            
   if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_BYPASS_TUNE) == 0)
   {
#ifndef BAST_EXCLUDE_EXT_TUNER      
      if (hChn->bExternalTuner == false)
#endif         
      {
         BAST_CHK_RETCODE(BAST_g3_P_TunerSetFilter(h, false)); /* set acquisition LPF */
         BAST_CHK_RETCODE(BAST_g3_P_TunerGetLockStatus(h, &bRefPllLocked, &bMixPllLocked));
         if (!bRefPllLocked || !bMixPllLocked)
            hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE_RETUNE;
      }
   }
#endif         
   
   if (hChn->reacqCtl & (BAST_G3_CONFIG_REACQ_CTL_RETUNE | BAST_G3_CONFIG_REACQ_CTL_FORCE_RETUNE))
   {
      hChn->tunerIfStep = 0;   
      retCode = BAST_g3_P_TunerSetFreq(h, BAST_g3_P_Acquire);
   }
   else
      retCode = BAST_g3_P_Acquire(h);
  
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_OnReacqTimerExpired()
******************************************************************************/
BERR_Code BAST_g3_P_OnReacqTimerExpired(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BAST_DEBUG_ACQ(BDBG_MSG(("BAST_g3_P_OnReacqTimerExpired()")));   
   
   hChn->bReacqTimerExpired = true;   
  
   /* dont reacquire if demod is currently locked */
   if (hChn->acqState == BAST_AcqState_eWaitForStableLock)
      return BERR_SUCCESS;

   hChn->reacqCause = BAST_ReacqCause_eFecNotStableLock;
   return BAST_g3_P_Reacquire(h);
}


/******************************************************************************
 BAST_g3_P_StartReacqTimer()
******************************************************************************/
BERR_Code BAST_g3_P_StartReacqTimer(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   uint32_t reacquisition_timeout, P_hi, P_lo, Q_hi, min_timeout;
     
   /* compute the reacquisition timeout */
   if (BAST_MODE_IS_TURBO(hChn->actualMode))
      min_timeout = 500000;
   else
      min_timeout = 300000;
      
   if (hChn->acqParams.symbolRate >= 20000000)
      reacquisition_timeout = min_timeout;
   else
   {
      BAST_MultU32U32(20000000, min_timeout, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &reacquisition_timeout);
   }
   
   hChn->bReacqTimerExpired = false;   
   BAST_DEBUG_ACQ(BDBG_MSG(("setting reacq timer to %d usecs", reacquisition_timeout)));   
   return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eReacqTimer, reacquisition_timeout, BAST_g3_P_OnReacqTimerExpired);
}


/******************************************************************************
 BAST_g3_P_StartTracking()
******************************************************************************/
BERR_Code BAST_g3_P_StartTracking(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode;
   
   BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eStartTracking));      
   hChn->trace[BAST_TraceEvent_eInitialLock] = 0;
   hChn->trace[BAST_TraceEvent_eStableLock] = 0;
   BAST_DEBUG_ACQ(BDBG_MSG(("BAST_g3_P_StartTracking()")));

   BAST_CHK_RETCODE(hChn->checkModeFunct(h));
      
   if (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_FORCE)
   {
      BDBG_MSG(("BAST_g3_P_StartTracking(): invalid mode"));
      hChn->reacqCause = BAST_ReacqCause_eInvalidMode;
      return BAST_g3_P_Reacquire(h);
   }
   
   BAST_CHK_RETCODE(BAST_g3_P_StartBert(h));

   hChn->acqState = BAST_AcqState_eWaitForInitialLock;

   /* enable lock/lost_lock interrupts */
   hChn->lockIsrFlag = 0;
   BAST_CHK_RETCODE(hChn->enableLockInterrupts(h));
   
   BAST_CHK_RETCODE(BAST_g3_P_StartReacqTimer(h));
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_LeakPliToFli()
******************************************************************************/
BERR_Code BAST_g3_P_LeakPliToFli(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   int32_t pli, fli, slval2, slval3, x;
   uint32_t P_hi, P_lo, val;
   
   if (BAST_MODE_IS_LDPC(hChn->actualMode))
      goto done;
      
   /* turbo and legacy QPSK */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_PLI, (uint32_t*)&pli);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_FLI, (uint32_t*)&fli); 

   if (BAST_MODE_IS_TURBO(hChn->actualMode))
   {
      slval3 = 16384; 
      slval2 = 128;   
   }
   else if (BAST_MODE_IS_DCII(hChn->actualMode))
   {
      slval3 = 272144; 
      slval2 = 512;   
   }
   else
   {
      slval3 = 16384; 
      slval2 = 128;
   }
   
   hChn->freqTransferInt += (pli / slval3);
   slval2 = (pli / slval2) + hChn->freqTransferInt;
        
   if (slval2 < 0)
      val = -slval2;
   else
      val = slval2;
                       
   BAST_MultU32U32(val, hChn->acqParams.symbolRate, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &P_hi, &P_lo);
   if (slval2 >= 0)
      x = (int32_t)P_lo;
   else
      x = (int32_t)-P_lo;
      
   fli += x;

   pli -= slval2;   

   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_PLI, (uint32_t*)&pli);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLI, (uint32_t*)&fli);  
   
   done:
   return retCode;   
}


/******************************************************************************
 BAST_g3_P_OnMonitorLock() - scheduled every 100 msecs
******************************************************************************/
BERR_Code BAST_g3_P_OnMonitorLock(BAST_ChannelHandle h)
{  
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode;

   if (hChn->timeSinceStableLock == 4)
   {
#ifndef BAST_EXCLUDE_SPUR_CANCELLER
      BAST_CHK_RETCODE(BAST_g3_P_ResetCWC(h));
#endif

      /* reset SNORE after 300 msecs */
      BAST_g3_P_AndRegister(h, BCHP_SDS_SNR_SNORECTL, ~0x80); 
      BAST_g3_P_OrRegister(h, BCHP_SDS_SNR_SNORECTL, 0x80);    
   }

   if (hChn->onMonitorLockFunct)
      hChn->onMonitorLockFunct(h);
   if (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_FORCE)
      goto force_reacquire;
      
   if (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_FREQ_DRIFT)
   {
      if (BAST_g3_P_IsCarrierOffsetOutOfRange(h))
      {
         hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE_RETUNE;

         force_reacquire:
         hChn->reacqCause = BAST_ReacqCause_eCarrierOffsetOutOfRange;
         return BAST_g3_P_Reacquire(h);
      }
   }

   BAST_CHK_RETCODE(BAST_g3_P_LeakPliToFli(h));

   /* DCO ramping fix */
   /* TBD */
   
#ifndef BAST_EXCLUDE_STATUS_EVENTS
   BAST_g3_P_CheckStatusIndicators(h);
#endif

   hChn->timeSinceStableLock++;
   if (hChn->bMonitorLock)
      retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eMonitorLockTimer, 100000, BAST_g3_P_OnMonitorLock);
   
   done:
   return retCode;
}



/******************************************************************************
 BAST_g3_P_OnStableLock1()
******************************************************************************/
BERR_Code BAST_g3_P_OnStableLock1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   
   if (hChn->bMonitorLock)
   {
      BAST_CHK_RETCODE(BAST_g3_P_OnMonitorLock(h));
      if (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_FORCE)
         retCode = BAST_g3_P_Reacquire(h);      
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_OnStableLock()
******************************************************************************/
BERR_Code BAST_g3_P_OnStableLock(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode;

   /* reset SNORE */
   BAST_g3_P_AndRegister(h, BCHP_SDS_SNR_SNORECTL, ~0x80); 
   BAST_g3_P_OrRegister(h, BCHP_SDS_SNR_SNORECTL, 0x80); 
      
   BAST_CHK_RETCODE(BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eReacqTimer));   
   BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eStableLock));
   BAST_DEBUG_ACQ(BDBG_MSG(("BAST_g3_P_OnStableLock(): t=%d", hChn->acqTime)));   
   BAST_CHK_RETCODE(BAST_g3_P_ResetLockFilter(h));  
   BAST_CHK_RETCODE(BAST_g3_P_GetCarrierError(h, &(hChn->initFreqOffset)));     

#if 0   
   if (hChn->bPlcTracking == false)
      BAST_g3_P_ConfigPlc(h, false); /* set tracking PLC */   
#endif

   if (hChn->onStableLockFunct)
   {
      BAST_CHK_RETCODE(hChn->onStableLockFunct(h));
      if (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_FORCE)
         return BAST_g3_P_Reacquire(h);
   }

   hChn->acqState = BAST_AcqState_eMonitorLock;
   BAST_g3_P_IndicateLocked(h);      
   hChn->bEverStableLock = true;  
   hChn->bStableLock = true;
   hChn->freqTransferInt = 0;
   hChn->timeSinceStableLock = 0;
   hChn->count1 = 0;

#ifndef BAST_EXCLUDE_SPUR_CANCELLER
   retCode = BAST_g3_P_InitCWC(h, BAST_g3_P_OnStableLock1);
#else
   retCode = BAST_g3_P_OnStableLock1(h);
#endif
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_Lock_isr() - callback routine for not_locked->locked transition
******************************************************************************/
void BAST_g3_P_Lock_isr(void *p, int int_id)
{
   BERR_Code retCode;
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   bool bLocked;
   
   BAST_g3_P_IncrementInterruptCounter(h, int_id);   
   BAST_DEBUG_ACQ(BDBG_MSG(("BAST_g3_P_Lock_isr(%d), lockIsrFlag=%d", int_id, hChn->lockIsrFlag)));

   if (hChn->getLockStatusFunct)
   {
      hChn->getLockStatusFunct(h, &bLocked);
      if (!bLocked)
      {
         BDBG_MSG(("false lock detected"));
         BAST_g3_P_NotLock_isr(p, BAST_g3_IntID_eNotLock);
         return;
      }
   }
   
   if (hChn->lockIsrFlag == 1)
   {       
      BDBG_MSG(("ignoring lock isr"));   
      return;
   }
   hChn->lockIsrFlag = 1; 

   BAST_g3_P_GetAcquisitionTimerValue(h, &(hChn->acqTime));  
   
   if (hChn->trace[BAST_TraceEvent_eInitialLock] == 0)
   {
      retCode = BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eInitialLock);
      if (retCode != BERR_SUCCESS)
      {
         BDBG_WRN(("BAST_g3_P_Lock_isr(): BAST_g3_P_LogTraceBuffer() error 0x%X", retCode));
      }
   }

   if (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_FREQ_DRIFT)
   {      
      if (BAST_g3_P_IsCarrierOffsetOutOfRange(h))
      {
         hChn->reacqCtl |= (BAST_G3_CONFIG_REACQ_CTL_FORCE_RETUNE | BAST_G3_CONFIG_REACQ_CTL_FORCE);
         hChn->reacqCause = BAST_ReacqCause_eCarrierOffsetOutOfRange;
         goto reacquire;
      }
   }
   
   if (hChn->onLockFunct)
      hChn->onLockFunct(h);

   reacquire:
   if (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_FORCE)
      BAST_g3_P_Reacquire(h);
   else
   {     
      hChn->acqState = BAST_AcqState_eWaitForStableLock;  
      BAST_DEBUG_ACQ(BDBG_MSG(("starting stable lock timer (%d usecs)", hChn->stableLockTimeout)));      
      BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eStableLockTimer, hChn->stableLockTimeout, BAST_g3_P_OnStableLock);   
   }
}


/******************************************************************************
 BAST_g3_P_NotLock_isr() - callback routine for locked->not_locked transition
******************************************************************************/
void BAST_g3_P_NotLock_isr(void *p, int int_id)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   bool bStartReacqTimer;
     
   BAST_g3_P_IncrementInterruptCounter(h, int_id);
   BAST_DEBUG_ACQ(BDBG_MSG(("BAST_g3_P_NotLock_isr(%d), lockIsrFlag=%d", int_id, hChn->lockIsrFlag)));

   hChn->acqTime = 0;
   if (hChn->lockIsrFlag == 0)
   {    
      BDBG_MSG(("ignoring not lock isr"));  
      return;
   }
   hChn->lockIsrFlag = 0;
   
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eStableLockTimer);
   
   BAST_g3_P_IndicateNotLocked(h);
   
   if ((hChn->acqState == BAST_AcqState_eMonitorLock) || 
       (BAST_g3_P_IsTimerRunning(h, BAST_TimerSelect_eReacqTimer) == false))
      bStartReacqTimer = true;
   else
      bStartReacqTimer = false;
   
   hChn->acqState = BAST_AcqState_eWaitForInitialLock;
   
   /* extend stable lock timeout */
   if ((hChn->acqCount > 0) || hChn->bEverStableLock)
   {
      hChn->stableLockTimeout += (((hChn->lockFilterRamp++ > 0) ? hChn->lockFilterRamp : 1) * hChn->lockFilterIncr);   
      if (hChn->stableLockTimeout > hChn->maxStableLockTimeout)
         hChn->stableLockTimeout = hChn->maxStableLockTimeout;
   }
   
   if (hChn->bReacqTimerExpired)
   {
      hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
      hChn->reacqCause = BAST_ReacqCause_eFecNotStableLock;
   }
   
   if (hChn->onLostLockFunct)
      hChn->onLostLockFunct(h);
   
   if (hChn->reacqCtl & BAST_G3_CONFIG_REACQ_CTL_FORCE)
   {
      BAST_g3_P_Reacquire(h);
      return;
   }
   
   if (hChn->bStableLock && (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_BERT_ENABLE))
   {
      /* reset the bert if bert lost lock */
      BAST_g3_P_OrRegister(h, BCHP_SDS_BERT_BERCTL, 1);  
      BAST_g3_P_AndRegister(h, BCHP_SDS_BERT_BERCTL, ~1);  /* clear reset */ 
   }
   
   hChn->bStableLock = false;
   
   if (bStartReacqTimer)
      BAST_g3_P_StartReacqTimer(h);
}


/******************************************************************************
 BAST_g3_P_WriteVerifyRegister() - callback routine for locked->not_locked transition
******************************************************************************/
BERR_Code BAST_g3_P_WriteVerifyRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t *val)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t retry, val2;
   
   for (retry = 0; retry < 100; retry++)
   {
      BAST_CHK_RETCODE(BAST_g3_P_WriteRegister(h, reg, val));
      BAST_CHK_RETCODE(BAST_g3_P_ReadRegister(h, reg, &val2));
      if (*val == val2)
         break;
   }
   
   if (retry >= 100)
      retCode = BAST_ERR_IOMB_XFER;
   
   done:
   return retCode;   
}


/******************************************************************************
 BAST_g3_P_SetMixctl()
******************************************************************************/
BERR_Code BAST_g3_P_SetMixctl(BAST_ChannelHandle h)
{
#if 0   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#endif
   uint32_t val;

#if 0   
   if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_PRESERVE_COMMANDED_FREQ)
     val = 0x01;
   else
#endif      
     val = 0x03;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_MIXCTL, &val);     
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_NonLegacyModeAcquireInit()
******************************************************************************/
BERR_Code BAST_g3_P_NonLegacyModeAcquireInit(BAST_ChannelHandle h)
{
   static const uint32_t script_nonlegacy_0[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNRCTL, 0x03),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_PLTD, 0x28000000),
      BAST_SCRIPT_OR(BCHP_SDS_CL_CLCTL1, 0x0000001C),
#ifndef BAST_HAS_WFE
      BAST_SCRIPT_WRITE(BCHP_SDS_AGC_ABW, 0x0B0B0000), 
#endif
      BAST_SCRIPT_AND_OR(BCHP_SDS_EQ_EQFFECTL, 0x00FF00FF, 0x03000600), /* EQMU=0x03,EQFFE2=0x06 */
      BAST_SCRIPT_AND_OR(BCHP_SDS_EQ_EQMISCCTL, 0x000000FF, 0x00140400), /* EQMISC=0x14, EQBLND=0x04 */
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_EQCFAD, 0x4C),   /* initialization of EQ tap */
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FERR, 0x00000000),
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLFBCTL, 0xFFFFFF00, 0x02), /* fwd loop frz, backward loop frz */
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_nonlegacy_2[] =
   {
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL1, 0xFFFF00FF, 0x0000DB00),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLLC, 0x20000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLIC, 0x40000200),
#if 1      
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLLC1, 0x20000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLIC1, 0x40000200),
#else
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLLC1, 0x01000100), /* orig */
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLIC1, 0x01000000), /* orig */
#endif
      BAST_SCRIPT_EXIT
   };

#if 0
   static const uint32_t script_nonlegacy_3[] =
   {
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL1, 0xFFFF00FF, 0x0000DE00),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLLC, 0x1F000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLIC, 0x40000200),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLLC1, 0x20000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLIC1, 0xC0000200),
      BAST_SCRIPT_EXIT
   };
#endif

#if 0
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#endif
   BERR_Code retCode;

   BAST_CHK_RETCODE(BAST_g3_P_SetFfeMainTap(h));
   BAST_CHK_RETCODE(BAST_g3_P_SetMixctl(h));
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_nonlegacy_0));

#if 0
   if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_PRESERVE_COMMANDED_FREQ)
   {
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_nonlegacy_3));
   }
   else
#endif      
   {
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_nonlegacy_2));
   }

   done:
   return retCode;   
}


/******************************************************************************
 BAST_g3_P_GetRegisterWriteWaitTime()
******************************************************************************/
void BAST_g3_P_GetRegisterWriteWaitTime(BAST_ChannelHandle h, uint32_t reg, uint32_t *wait_time)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   *wait_time = 0;
   if (hChn->acqParams.symbolRate < 8000000)
   {
#if 1  
      /* VIT, FEC, CL, and EQ are in the baud clock domain */         
      if (((reg >= BCHP_SDS_VIT_VTCTL) && (reg <= BCHP_SDS_FEC_FRSV)) ||         
         ((reg >= BCHP_SDS_CL_CLCTL1) && (reg <= BCHP_SDS_EQ_MGAINA))) 
#else
      if (((reg >= BCHP_SDS_CL_CLCTL1) && (reg <= BCHP_SDS_FEC_FRSV)) ||
          ((reg >= BCHP_SDS_SNR_SNRCTL) && (reg <= BCHP_SDS_SNR_SNORECTL)))
#endif
      {
        if (hChn->acqParams.symbolRate > 2000000)
          *wait_time = 1;
        else
          *wait_time = 2;
      }
   }
}



/******************************************************************************
 BAST_g3_P_GetRegisterWriteWaitTime()
******************************************************************************/
uint32_t BAST_g3_P_GetNumDecimatingFilters(BAST_ChannelHandle h)
{
   uint32_t filtctl, i, n = 0;
   
   n = 0;
   BAST_g3_P_ReadRegister(h, BCHP_SDS_FE_FILTCTL, &filtctl);
   for (i = 0; i < 5; i++)
   {
      if ((filtctl & (1 << i)) == 0)
         n++;
      else
         break;
   }   
   if ((n == 5) && ((filtctl & 0x80) == 0))
      n++;
   return n;
}

