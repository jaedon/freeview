/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/172 $
 * $brcm_Date: 2/7/13 4:04p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv.h $
 * 
 * Hydra_Software_Devel/172   2/7/13 4:04p ronchan
 * SWSATFE-174: increment release version to 5
 * 
 * Hydra_Software_Devel/171   2/7/13 10:31a ckpark
 * SWSATFE-174: apply BAST_FROF2_WORKAROUND for 4538-Ax only
 * 
 * Hydra_Software_Devel/170   2/6/13 1:52p enavarro
 * SWSATFE-174: removed BAST_g3_P_ResyncBert() since it is never used
 * 
 * Hydra_Software_Devel/169   2/6/13 1:33p ckpark
 * SWSATFE-174: define BAST_FROF2_WORKAROUND for 4538
 * 
 * Hydra_Software_Devel/168   2/6/13 10:14a enavarro
 * SWSATFE-174: removed BAST_g3_P_HpEnableFineFreqEstimate() since it is
 * no longer used
 * 
 * Hydra_Software_Devel/167   2/6/13 9:38a ckpark
 * SWSATFE-174: added variables for supporting fine freq estimation
 * 
 * Hydra_Software_Devel/165   1/4/13 9:40a ckpark
 * SWSATFE-174: re-enable fine freq estimation on 4538
 * 
 * Hydra_Software_Devel/164   12/18/12 5:56p ronchan
 * SWSATFE-174: increment release version to 4
 * 
 * Hydra_Software_Devel/163   12/18/12 4:48p enavarro
 * SWSATFE-174: disable fine freq est on 4538 for now
 * 
 * Hydra_Software_Devel/162   12/18/12 10:37a enavarro
 * SWSATFE-174: added virtual function getLockStatusFunct in channel
 * handle
 * 
 * Hydra_Software_Devel/161   12/13/12 4:31p enavarro
 * SWSATFE-174: added postInitApFunct
 * 
 * Hydra_Software_Devel/160   12/12/12 3:14p enavarro
 * SWSATFE-234: added dynamic power down of SDS_OPLL
 * 
 * Hydra_Software_Devel/159   12/11/12 10:36a ronchan
 * SWSATFE-174: increment release version to 3
 * 
 * Hydra_Software_Devel/158   11/21/12 8:28a enavarro
 * SWSATFE-174: re-enable fine freq estimation on 4538
 * 
 * Hydra_Software_Devel/157   11/19/12 5:53p enavarro
 * SWSATFE-174: disable fine freq estimation for 4538
 * 
 * Hydra_Software_Devel/156   11/8/12 4:19p enavarro
 * SWSATFE-174: changed function signature of
 * BAST_SetAdcSelect()/BAST_GetAdcSelect()
 * 
 * Hydra_Software_Devel/155   11/5/12 5:00p enavarro
 * SWSATFE-174: re-enable fine freq estimate on 4538
 * 
 * Hydra_Software_Devel/154   11/5/12 3:28p ronchan
 * SWSATFE-174: add function for dynamic ldpc power management
 * 
 * Hydra_Software_Devel/153   11/5/12 2:52p ckpark
 * SWSATFE-174: disable fine freq estimation on 4538
 * 
 * Hydra_Software_Devel/152   10/23/12 4:20p enavarro
 * SWSATFE-174: put BFOS programming in separate function for 4538 dft low
 * baud rate workaround
 * 
 * Hydra_Software_Devel/151   10/23/12 9:22a enavarro
 * SWSATFE-174: fix compile error
 * 
 * Hydra_Software_Devel/150   10/22/12 5:08p enavarro
 * SWSATFE-174: dont declare excluded functions
 * 
 * Hydra_Software_Devel/149   10/22/12 4:30p enavarro
 * SWSATFE-174: removed unused handle variables when BAST_HAS_WFE is
 * defined; added BAST_EXCLUDE_ACW build option
 * 
 * Hydra_Software_Devel/148   10/22/12 11:17a enavarro
 * SWSATFE-221: fix dft for low baud rate
 * 
 * Hydra_Software_Devel/147   10/22/12 9:26a ckpark
 * SWSATFE-221: added debug3 config parameter for debugging
 * 
 * Hydra_Software_Devel/146   10/19/12 11:02a ronchan
 * SW3461-274: changes to bfec structure
 * 
 * Hydra_Software_Devel/145   10/18/12 11:17a enavarro
 * SWSATFE-174: enable fine frequency estimation for 4538
 * 
 * Hydra_Software_Devel/144   10/15/12 10:03a enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/143   10/1/12 3:24p enavarro
 * SWSATFE-174: disable notch filters on 4538 for now
 * 
 * Hydra_Software_Devel/142   9/11/12 11:11a enavarro
 * SWSATFE-174: added BAST_EXCLUDE_EXT_TUNER build option; specify
 * BAST_EXCLUDE_STATUS_EVENTS in 4538 to save memory
 * 
 * Hydra_Software_Devel/141   9/7/12 9:51a ronchan
 * SWSATFE-214: fix header files for 7358 and 7360
 * 
 * Hydra_Software_Devel/140   9/7/12 8:20a enavarro
 * SWSATFE-214: restructure AST directory
 * 
 * Hydra_Software_Devel/139   8/30/12 2:18p ronchan
 * SWSATFE-207: added config parameters for tuner agc window length,
 * amplitude threshold, and loop coefficient
 * 
 * Hydra_Software_Devel/138   8/29/12 3:56p ronchan
 * SW7346-1005: change Mi2cDisableInterrupts to isr context
 * 
 * Hydra_Software_Devel/137   8/28/12 2:28p enavarro
 * SWSATFE-140: removed BAST_g3_P_UpdateCWC()
 * 
 * Hydra_Software_Devel/136   8/28/12 2:13p enavarro
 * SWSATFE-174: enable spur canceller for 4517
 * 
 * Hydra_Software_Devel/135   8/28/12 9:57a enavarro
 * SWSATFE-174: disable CWC canceller by default; added changes for 4538
 * 
 * Hydra_Software_Devel/134   8/1/12 10:30a enavarro
 * SWSATFE-174: added BAST_SetAdcSelect() and BAST_GetAdcSelect() to the
 * API for chips with multiple wideband tuners
 * 
 * Hydra_Software_Devel/133   7/18/12 3:22p ronchan
 * SWSATFE-207: added daisy gain config param, renamed agc thresh config
 * param
 * 
 * Hydra_Software_Devel/132   7/13/12 9:41a enavarro
 * SWSATFE-140: added sds revision id to bast handle
 * 
 * Hydra_Software_Devel/131   6/27/12 5:11p enavarro
 * SWSATFE-174: added 4538 support
 * 
 * Hydra_Software_Devel/130   5/15/12 11:31a ronchan
 * SW7360-13: add ast chip header for 7360
 * 
 * Hydra_Software_Devel/129   5/10/12 7:05p ronchan
 * SWSATFE-194: add tuner init done event
 * 
 * Hydra_Software_Devel/128   4/26/12 9:08a enavarro
 * SWSATFE-140: fixed compiler warning
 * 
 * Hydra_Software_Devel/127   4/20/12 2:51p ronchan
 * SWSATFE-75: add diseqc almost empty callback and isr
 * 
 * Hydra_Software_Devel/126   3/22/12 2:05p enavarro
 * SWSATFE-184: added tuner_rfagc_thresh config parameter
 * 
 * Hydra_Software_Devel/125   3/20/12 10:17a enavarro
 * SWSATFE-176: added BAST_g3_P_GetCarrierErrorPli() and
 * BAST_g3_P_GetCarrierErrorFli()
 * 
 * Hydra_Software_Devel/124   3/19/12 1:48p enavarro
 * SWSATFE-140: fixed compiler warning
 * 
 * Hydra_Software_Devel/123   3/16/12 4:20p enavarro
 * SWSATFE-140: added reacqCause to channel handle struct
 * 
 * Hydra_Software_Devel/122   3/15/12 11:28a enavarro
 * SWSATFE-140: fixed acqCount for turbo scan
 * 
 * Hydra_Software_Devel/121   3/12/12 3:29p ronchan
 * SWSATFE-180: add acw to diseqc device
 * 
 * Hydra_Software_Devel/120   2/28/12 3:49p enavarro
 * SWSATFE-177: added ACM mode support
 * 
 * Hydra_Software_Devel/119   2/13/12 11:30a ckpark
 * SWSATFE-140: added tunerFgaCalManual and tunerLpfCalManual to channel
 * handle struct
 * 
 * Hydra_Software_Devel/118   2/9/12 3:12p enavarro
 * SWSATFE-140: reworked acquisition timer
 * 
 * Hydra_Software_Devel/117   2/1/12 3:27p enavarro
 * SWSATFE-140: added input parameter to BAST_g3_P_InitChannels()
 * 
 * Hydra_Software_Devel/116   1/26/12 11:06a enavarro
 * SWSATFE-140: define BAST_DONT_USE_MI2C_IRQ for 4517
 * 
 * Hydra_Software_Devel/115   1/11/12 3:59p ronchan
 * SWSATFE-140: remove exclude ftm compile flag
 * 
 * Hydra_Software_Devel/114   1/3/12 10:06a ckpark
 * SWSATFE-162: added debug1 and debug2 config params
 * 
 * Hydra_Software_Devel/113   12/27/11 3:46p ckpark
 * SWSATFE-166: added function declaration
 * 
 * Hydra_Software_Devel/112   12/27/11 11:07a enavarro
 * SWSATFE-166: added bTurboSpinv to channel handle struct; added function
 * declarations
 * 
 * Hydra_Software_Devel/111   11/30/11 2:44p enavarro
 * SWSATFE-157: declare BAST_g3_P_GetRegisterWriteWaitTime()
 * 
 * Hydra_Software_Devel/110   11/15/11 2:33p ckpark
 * SWSATFE-161: reset BERT once when bert lost lock; fixed BERCTL when
 * BERT not enabled
 * 
 * Hydra_Software_Devel/109   11/11/11 10:59a ronchan
 * SWSATFE-156: add tone state to diseqc device
 * 
 * Hydra_Software_Devel/108   11/7/11 3:06p ronchan
 * SWSATFE-140: add tuner ref pll channel to identify shared ref pll
 * control
 * 
 * Hydra_Software_Devel/107   10/13/11 1:57p enavarro
 * SWSATFE-150: changes to cwc
 * 
 * Hydra_Software_Devel/106   10/10/11 3:09p enavarro
 * SWSATFE-150: added BAST_g3_P_ResetCWC()
 * 
 * Hydra_Software_Devel/105   10/7/11 4:44p enavarro
 * SWSATFE-150: added CWC support
 * 
 * Hydra_Software_Devel/104   10/7/11 10:35a enavarro
 * SWSATFE-150: added BAST_g3_P_TunerGetActualLOFreq()
 * 
 * Hydra_Software_Devel/103   10/6/11 9:17a enavarro
 * SWSATFE-150: reworked spur canceller to use CWC instead of notch
 * filters
 * 
 * Hydra_Software_Devel/102   9/29/11 10:44a ckpark
 * SWSATFE-137: removed BAST_g3_P_UpdateSnore() declaration
 * 
 * Hydra_Software_Devel/101   9/29/11 9:51a ronchan
 * SWSATFE-139: add high and low kvco_cntl
 * 
 * Hydra_Software_Devel/100   9/28/11 2:30p enavarro
 * SWSATFE-86: added bMonitorLock in channel handle struct
 * 
 * Hydra_Software_Devel/99   9/16/11 9:57a ronchan
 * SWSATFE-139: add variable ambient temperature for Vc adjust
 * 
 * Hydra_Software_Devel/98   9/15/11 2:24p ronchan
 * SWSATFE-139: added high and low thresholds for reference Vc
 * 
 * Hydra_Software_Devel/97   9/13/11 5:39p ronchan
 * SWSATFE-139: add support for kvco calibration
 * 
 * Hydra_Software_Devel/96   8/24/11 2:54p enavarro
 * SWSATFE-140: add support for BAST_EXCLUDE_LDPC
 * 
 * Hydra_Software_Devel/95   8/24/11 10:55a enavarro
 * SWSATFE-140: fixed compiler errors when BAST_EXCLUDE_LDPC is defined
 * 
 * Hydra_Software_Devel/94   8/24/11 10:32a ronchan
 * SWSATFE-139: add chip handle in device handle
 * 
 * Hydra_Software_Devel/93   8/18/11 5:13p enavarro
 * SWSATFE-140: add build option to exclude ftm
 * 
 * Hydra_Software_Devel/92   8/15/11 2:29p ckpark
 * SWSATFE-83: disable fine freq estimation for now
 * 
 * Hydra_Software_Devel/91   8/12/11 10:43a enavarro
 * SWSATFE-140: add build option to exclude ldpc code from ast pi
 * 
 * Hydra_Software_Devel/90   8/11/11 2:28p ronchan
 * SWSATFE-139: revert linear search range to unsigned
 * 
 * Hydra_Software_Devel/89   8/11/11 1:55p enavarro
 * SWSATFE-86: add build options to exclude bcm3445 and mi2c support
 * 
 * Hydra_Software_Devel/88   8/10/11 11:06a ronchan
 * SWSATFE-139: changed linear search range to signed
 * 
 * Hydra_Software_Devel/87   8/10/11 10:57a enavarro
 * SWSATFE-86: reworked snr filtering
 * 
 * Hydra_Software_Devel/86   8/10/11 10:06a ronchan
 * SWSATFE-139: added linear cap search range for tuner
 * 
 * Hydra_Software_Devel/85   7/29/11 11:11a guangcai
 * SWSATFE-137: implemented SNORE filtering; enabled SNORE for all g3
 * chips
 * 
 * Hydra_Software_Devel/84   7/27/11 11:34a guangcai
 * SWSATFE-136: added BAST_g3_P_WriteVerifyRegister()
 * 
 * Hydra_Software_Devel/83   7/22/11 2:43p guangcai
 * SWSATFE-127: Fine freq estimation is defined
 * 
 * Hydra_Software_Devel/82   7/15/11 9:59a enavarro
 * SWSATFE-86: added bOpen in BAST_g3_P_Handle struct
 * 
 * Hydra_Software_Devel/81   7/8/11 10:14a ronchan
 * SWSATFE-75: added timer select parameter for running dco loop
 * 
 * Hydra_Software_Devel/80   7/7/11 4:14p ckpark
 * SWSATFE-127: #define'd BAST_ENABLE_FINE_FREQ_ESTIMATION
 * 
 * Hydra_Software_Devel/79   7/7/11 10:44a ronchan
 * SWSATFE-75: added runtime parameter for dco loop
 * 
 * Hydra_Software_Devel/78   7/7/11 10:34a enavarro
 * SWSATFE-127: renamed frof3StableCount to fineFreqStableCount
 * 
 * Hydra_Software_Devel/77   7/6/11 11:18a ronchan
 * SWSATFE-75: added function to run dco loops
 * 
 * Hydra_Software_Devel/76   6/20/11 2:15p enavarro
 * SWSATFE-127: check for FROF3 < 7KHz before enabling fine frequency
 * estimate
 * 
 * Hydra_Software_Devel/75   6/16/11 9:10a enavarro
 * SWSATFE-127: added fine freq offset estimation for DVB-S2
 * 
 * Hydra_Software_Devel/74   5/26/11 4:12p enavarro
 * SWSATFE-86: added workaround for timer irqs not hooked up in 4528-A0
 * 
 * Hydra_Software_Devel/73   4/29/11 2:58p enavarro
 * SWSATFE-86: added support for undersample mode
 * 
 * Hydra_Software_Devel/72   4/11/11 10:07a enavarro
 * SWSATFE-86: added BAST_HAS_LEAP for 4528
 * 
 * Hydra_Software_Devel/71   4/8/11 10:02a enavarro
 * SWSATFE-86: removed BAST_EXCLUDE_SPLITTER_MODE and BAST_EXCLUDE_OLD_LNA
 * 
 * Hydra_Software_Devel/70   4/5/11 10:54a ckpark
 * SWSATFE-102: normalized tuner calibration threshold is declared
 * 
 * Hydra_Software_Devel/69   4/2/11 5:55p enavarro
 * SWSATFE-86: added BAST_g3_P_ConfigChanAgc()
 * 
 * Hydra_Software_Devel/68   4/1/11 9:03a enavarro
 * SWSATFE-86: added BAST_g3_P_GetChanAddress() declaration
 * 
 * Hydra_Software_Devel/67   4/1/11 8:17a enavarro
 * SWSATFE-86: move BAST_DEFAULT_SAMPLE_FREQ definition in chip-specific
 * file
 * 
 * Hydra_Software_Devel/66   4/1/11 7:59a enavarro
 * SWSATFE-86: added BAST_g3_P_SetChanDecFcw() declaration
 * 
 * Hydra_Software_Devel/65   3/31/11 9:37a enavarro
 * SWSATFE-110: store saved modcod
 * 
 * Hydra_Software_Devel/64   3/30/11 11:23a ronchan
 * SWSATFE-75: remove BAST_TunerIndirectRegGroup_eMax
 * 
 * Hydra_Software_Devel/63   3/10/11 3:42p ronchan
 * SWSATFE-75: added functions to power down fskphy
 * 
 * Hydra_Software_Devel/62   3/7/11 8:35a enavarro
 * SWSATFE-86: defined BAST_EXCULDE_SPUR_CANCELLER because notch filter
 * isn't supported yet in g3 code
 * 
 * Hydra_Software_Devel/61   3/4/11 8:11a enavarro
 * SWSATFE-75: add support for BCM4528
 * 
 * Hydra_Software_Devel/60   3/3/11 11:46a enavarro
 * SWSATFE-75: fixed compiler errors when BAST_EXCLUDE_TURBO is defined
 * 
 * Hydra_Software_Devel/59   3/3/11 10:52a enavarro
 * SWSATFE-75: put all diseqc state variables in BAST_g3_P_DiseqcDevice
 * struct and allocate this only if the channel has diseqc capability
 * 
 * Hydra_Software_Devel/58   2/17/11 10:04a ronchan
 * SWSATFE-75: added definition for BAST_g3_P_ConfigAgc()
 * 
 * Hydra_Software_Devel/57   1/27/11 3:42p enavarro
 * SWSATFE-75: removed dftSize config parameter
 * 
 * Hydra_Software_Devel/56   1/24/11 11:48a enavarro
 * SWSATFE-75: added MISC_CTL config param
 * 
 * Hydra_Software_Devel/55   1/24/11 10:12a enavarro
 * SWSATFE-75: add ability to specify ndiv when setting sample freq
 * 
 * Hydra_Software_Devel/54   1/19/11 3:49p ronchan
 * SWSATFE-99: remove old fsk channel select parameter
 * 
 * Hydra_Software_Devel/53   1/19/11 3:20p ronchan
 * SWSATFE-99: added chip-independent fsk channel config api
 * 
 * Hydra_Software_Devel/52   1/11/11 3:23p ronchan
 * SWSATFE-75: added parameters and functions for 3445 support
 * 
 * Hydra_Software_Devel/51   1/4/11 11:06a enavarro
 * SWSATFE-75: added timeout for dft_done
 * 
 * Hydra_Software_Devel/50   12/29/10 12:10p enavarro
 * SWSATFE-75: removed peakScanPower from channel handle struct
 * 
 * Hydra_Software_Devel/49   12/27/10 8:51a enavarro
 * SWSATFE-75: change scaling for rainFadeSnr*
 * 
 * Hydra_Software_Devel/48   12/24/10 3:03p enavarro
 * SWSATFE-75: added bPlcTracking to channel handle struct
 * 
 * Hydra_Software_Devel/47   12/22/10 11:24a enavarro
 * SWSATFE-75: declare BAST_g3_P_TurboDisableSyncInterrupt()
 * 
 * Hydra_Software_Devel/46   12/17/10 10:44a enavarro
 * SWSATFE-75: added dft_current_max_pow in channel handle struct
 * 
 * Hydra_Software_Devel/45   12/14/10 3:55p ronchan
 * SWSATFE-75: added function def for SetPlc()
 * 
 * Hydra_Software_Devel/44   12/11/10 7:46p enavarro
 * SWSATFE-75: added some state variables for dft
 * 
 * Hydra_Software_Devel/43   12/9/10 5:25p ronchan
 * SWSATFE-95: added functions to control tuner LNA
 * 
 * Hydra_Software_Devel/42   12/8/10 2:11p enavarro
 * SWSATFE-75: added turbo sync detect callback
 * 
 * Hydra_Software_Devel/41   11/30/10 2:58p enavarro
 * SWSATFE-75: added reacq_ctl config parameter
 * 
 * Hydra_Software_Devel/40   11/29/10 3:22p enavarro
 * SWSATFE-80: added BAST_GetNetworkSpec() and BAST_SetNetworkSpec() to
 * the API
 * 
 * Hydra_Software_Devel/39   11/24/10 4:48p enavarro
 * SWSATFE-75: added BAST_g3_P_LdpcAcquireInit()
 * 
 * Hydra_Software_Devel/38   11/22/10 3:15p enavarro
 * SWSATFE-75: renamed hHpCb to hHpLockCb; renamed BAST_g3_P_Hp_isr() to
 * BAST_g3_HPLock_isr()
 * 
 * Hydra_Software_Devel/37   10/22/10 2:58p ronchan
 * SWSATFE-75: added definition for DisableChannelInterrupts_isr
 * 
 * Hydra_Software_Devel/36   10/21/10 10:43a ronchan
 * SWSATFE-75: remove unused diseqc parameters
 * 
 * Hydra_Software_Devel/35   10/19/10 11:11a enavarro
 * SWSATFE-75: added comments to some peak scan variables in the channel
 * struct
 * 
 * Hydra_Software_Devel/34   10/18/10 2:53p enavarro
 * SWSATFE-75: added peak scan support
 * 
 * Hydra_Software_Devel/33   10/14/10 5:42p ronchan
 * SWSATFE-75: moved diseqcCtl to diseqcSettings
 * 
 * Hydra_Software_Devel/32   10/11/10 11:34a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/31   10/5/10 3:06p enavarro
 * SWSATFE-75: added pli-to-fli leak
 * 
 * Hydra_Software_Devel/30   10/4/10 2:41p enavarro
 * SWSATFE-75: added function declarations
 * 
 * Hydra_Software_Devel/29   9/30/10 3:13p ronchan
 * SWSATFE-80: store diseqc settings in BAST_DiseqcSettings struct, added
 * API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/28   9/30/10 2:02p enavarro
 * SWSATFE-80: store transport settings in BAST_OutputTransportSettings
 * struct in the channel handle; removed xportCtl channel handle member
 * 
 * Hydra_Software_Devel/27   9/29/10 5:28p ronchan
 * SWSATFE-75: added variables for tuner filter calibration
 * 
 * Hydra_Software_Devel/26   9/29/10 3:49p enavarro
 * SWSATFE-80: added API functions to set/get output transport settings;
 * added bit definitions for xportCtl variable
 * 
 * Hydra_Software_Devel/25   9/29/10 11:56a enavarro
 * SWSATFE-75: added bfos and bTimingLock to channel handle struct
 * 
 * Hydra_Software_Devel/24   9/29/10 10:11a enavarro
 * SWSATFE-75: added BAST_GetSignalDetectStatus() to BAST API; removed
 * unused channel handle members
 * 
 * Hydra_Software_Devel/23   9/28/10 6:37p ronchan
 * SWSATFE-75: added tuner variables
 * 
 * Hydra_Software_Devel/22   9/28/10 10:25a enavarro
 * SWSATFE-75: added comment for BAST_TimerSelect_eBer
 * 
 * Hydra_Software_Devel/21   9/27/10 3:45p ronchan
 * SWSATFE-75: added opll_N and opll_D to channel handle
 * 
 * Hydra_Software_Devel/20   9/24/10 9:51a ronchan
 * SWSATFE-75: add function definition for SetFfeMainTap()
 * 
 * Hydra_Software_Devel/19   9/23/10 6:18p ronchan
 * SWSATFE-75: clarified bw and damp for carrier and baud loop
 * 
 * Hydra_Software_Devel/18   9/22/10 5:37p ronchan
 * SWSATFE-75: added SetBaudBw, SetCarrierBw, Bert functions
 * 
 * Hydra_Software_Devel/17   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/16   9/22/10 10:09a enavarro
 * SWSATFE-75: added BAST_g3_P_IsHpLocked()
 * 
 * Hydra_Software_Devel/15   9/21/10 3:57p enavarro
 * SWSATFE-75: implemented tracking and lock filtering
 * 
 * Hydra_Software_Devel/14   9/21/10 1:31p enavarro
 * SWSATFE-75: added BAST_g3_P_NotchUpdate()
 * 
 * Hydra_Software_Devel/13   9/21/10 11:47a enavarro
 * SWSATFE-75: clean up function declaration
 * 
 * Hydra_Software_Devel/12   9/21/10 10:33a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/11   9/20/10 5:26p ronchan
 * SWSATFE-75: fixed compile errors
 * 
 * Hydra_Software_Devel/10   9/20/10 5:09p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/9   9/20/10 3:22p enavarro
 * SWSATFE-75: added BAST_g3_P_TimerIsRunning() and
 * BAST_g3_P_TimerIsExpired() functions; updated BAST_AcqState
 * 
 * Hydra_Software_Devel/8   9/17/10 5:22p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/7   9/15/10 4:36p enavarro
 * SWSATFE-75: added tuner indirect register access functions
 * 
 * Hydra_Software_Devel/6   9/15/10 10:30a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/5   9/14/10 2:29p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/4   9/8/10 2:39p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/3   9/8/10 10:35a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/2   9/1/10 5:03p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   8/31/10 1:55p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#ifndef _BAST_G3_PRIV_H__
#define _BAST_G3_PRIV_H__

/* exclude flags for code savings */
/*
#define BAST_EXCLUDE_TURBO
#define BAST_EXCLUDE_STATUS_EVENTS
#define BAST_EXCLUDE_POWERDOWN
#define BAST_EXCLUDE_PEAK_SCAN
#define BAST_EXCLUDE_MI2C
#define BAST_EXCLUDE_BCM3445
#define BAST_EXCLUDE_LDPC
#define BAST_EXCLUDE_FTM
*/
#define BAST_EXCLUDE_SPUR_CANCELLER /* compile out spur canceller by default */

#include "bast_g3.h"
#if (BCHP_CHIP==7358)
   #include "bast_7358_priv.h"
#elif (BCHP_CHIP==7346) 
   #include "bast_7346_priv.h"
#elif (BCHP_CHIP==7344)
   #include "bast_7344_priv.h"
#elif (BCHP_CHIP==4528)
   #define BAST_HAS_WFE
   #define BAST_HAS_LEAP
   #define BAST_EXCLUDE_MI2C
   #define BAST_EXCLUDE_BCM3445
   #include "bast_4528_priv.h"
#elif (BCHP_CHIP==4517)
   #define BAST_HAS_LEAP
   #define BAST_EXCLUDE_LDPC
   #define BAST_DONT_USE_MI2C_IRQ   
   #undef BAST_EXCLUDE_SPUR_CANCELLER   
   #include "bast_4517_priv.h"
#elif (BCHP_CHIP==7360)
   #include "bast_7360_priv.h"
#elif (BCHP_CHIP==4538)
   #define BAST_HAS_WFE
   #define BAST_HAS_LEAP
   #define BAST_EXCLUDE_BCM3445
   #define BAST_EXCLUDE_MI2C
   #define BAST_EXCLUDE_STATUS_EVENTS
   #define BAST_EXCLUDE_EXT_TUNER
   #define BAST_EXCLUDE_ACW
   /* disable notch for now... #undef BAST_EXCLUDE_SPUR_CANCELLER */
   #define BAST_ENABLE_FINE_FREQ_ESTIMATION
#if BCHP_VER < BCHP_VER_B0   
   #define BAST_FROF2_WORKAROUND
#endif   
   #include "bast_4538_priv.h"
#else
   #error "unsupported BCHP_CHIP"
#endif


#define BAST_G3_RELEASE_VERSION 5


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
#define BAST_LDPC_SCAN_STATE_QPSK          0x02
#define BAST_LDPC_SCAN_STATE_PILOT         0x01
#define BAST_LDPC_SCAN_STATE_ENABLED       0x80
#define BAST_LDPC_SCAN_STATE_FOUND         0x40
#define BAST_LDPC_SCAN_STATE_MASK          0xC0

/* bit definitions for turboScanState */
#define BAST_TURBO_SCAN_STATE_HP_INIT        0x01
#define BAST_TURBO_SCAN_STATE_HP_LOCKED      0x02
#define BAST_TURBO_SCAN_STATE_8PSK_HP_LOCKED 0x04
#define BAST_TURBO_SCAN_STATE_8PSK_FAILED    0x08
#define BAST_TURBO_SCAN_STATE_FIRST_TIME     0x80

/* bit definitions for signalDetectStatus */
#define BAST_SIGNAL_DETECT_ENABLED     0x80
#define BAST_SIGNAL_DETECT_IN_PROGRESS 0x40
#define BAST_SIGNAL_DETECT_TIMING_LOCK 0x01

/* bit definitions for peakScanStatus */
#define BAST_PEAK_SCAN_STATUS_ENABLED 0x80
#define BAST_PEAK_SCAN_STATUS_DONE    0x40
#define BAST_PEAK_SCAN_STATUS_ERROR   0x20

#define BAST_BCM3445_DEFAULT_ADDRESS 0xD8

typedef BERR_Code (*BAST_g3_FUNCT)(BAST_ChannelHandle);
typedef BERR_Code (*BAST_g3_DEV_FUNCT)(BAST_Handle);
typedef void (*BAST_g3_LOCK_FUNCT)(BAST_ChannelHandle, bool*);

/* timers */
typedef enum BAST_TimerSelect
{
   BAST_TimerSelect_eBaud = 0,
   BAST_TimerSelect_eBaudUsec,
   BAST_TimerSelect_eBer,  /* used for delays between successive register accesses in bclk domain */
   BAST_TimerSelect_eGen1, /* used for reacquisition timeout */
   BAST_TimerSelect_eGen2, /* used for stable_lock timer and lock monitor activities */
   BAST_TimerSelect_eGen3, /* used to measure acquisition time */
   BAST_TimerSelect_eDiseqc1,
   BAST_TimerSelect_eDiseqc2   
} BAST_TimerSelect;

#if (BCHP_CHIP==4528)
#define BAST_TimerSelect_eReacqTimer       BAST_TimerSelect_eBaudUsec
#define BAST_TimerSelect_eStableLockTimer  BAST_TimerSelect_eBaudUsec
#define BAST_TimerSelect_eMonitorLockTimer BAST_TimerSelect_eBaudUsec
#else
#define BAST_TimerSelect_eReacqTimer       BAST_TimerSelect_eGen1
#define BAST_TimerSelect_eStableLockTimer  BAST_TimerSelect_eGen2
#define BAST_TimerSelect_eMonitorLockTimer BAST_TimerSelect_eGen2
#endif

/* acquisition states */
typedef enum BAST_AcqState
{
   BAST_AcqState_eIdle = 0,
   BAST_AcqState_eTuning,
   BAST_AcqState_eAcquiring,
   BAST_AcqState_eWaitForInitialLock,
   BAST_AcqState_eWaitForStableLock,
   BAST_AcqState_eMonitorLock
} BAST_AcqState;

/* tuner indirect register groups */
typedef enum BAST_TunerIndirectRegGroup
{
   BAST_TunerIndirectRegGroup_eRfagc,
   BAST_TunerIndirectRegGroup_eBbagc,
   BAST_TunerIndirectRegGroup_ePreDcoI,
   BAST_TunerIndirectRegGroup_ePreDcoQ,
   BAST_TunerIndirectRegGroup_ePostDcoI,
   BAST_TunerIndirectRegGroup_ePostDcoQ
} BAST_TunerIndirectRegGroup;

/* tuner calibration type */
typedef enum BAST_TunerCalType
{
   BAST_TunerCalType_eFga,
   BAST_TunerCalType_eLpf
} BAST_TunerCalType;

/* reason for reacquisition */
typedef enum BAST_ReacqCause
{
   BAST_ReacqCause_eOK = 0,
   BAST_ReacqCause_eTimingLoopNotLocked, /* timing loop could not be locked */
   BAST_ReacqCause_eFecNotStableLock, /* reacquisition timer expired due to FEC not able to stay in lock */
   BAST_ReacqCause_eInvalidMode,  /* restricted mode */
   BAST_ReacqCause_eCarrierOffsetOutOfRange, /* carrier offset drifted beyond search range */
   BAST_ReacqCause_eInvalidModcod, /* decoded modcod is invalid */
   BAST_ReacqCause_eModcodNotStable, /* modcod not stable */
   BAST_ReacqCause_eTimerError1,  /* BAST_EnableTimer() returned error */
   BAST_ReacqCause_eTimerError2,  /* BAST_EnableTimer() returned error */
   BAST_ReacqCause_eHpLostLock1,  /* HP not locked in frame boundary isr */
   BAST_ReacqCause_eHpLostLock2,  /* HP lost lock while waiting for turbo sync */
   BAST_ReacqCause_eHpLostLock3,  /* HP lost lock during LDPC tracking */
   BAST_ReacqCause_eHpLostLock4,  /* HP lost lock during Turbo tracking */
   BAST_ReacqCause_eHpCouldNotLock, /* HP could not reach state 5 */
   BAST_ReacqCause_eNoAfec, /* LDPC acquisition initiated but chip does not have AFEC */
   BAST_ReacqCause_eNoTfec, /* turbo acquisition initiated but chip does not have TFEC */
   BAST_ReacqCause_eInvalidCondition1, /* actualMode is not LDPC in LDPC function */
   BAST_ReacqCause_eVitNotLock, /* no Viterbi lock in legacy QPSK mode */
   BAST_ReacqCause_eUnableToDetermineActualMode, /* legacy qpsk mode: puncture rate status not stable */
   BAST_ReacqCause_eTurboSyncTimeout, /* no turbo sync seen */
   BAST_ReacqCause_eCodeRateChanged   /* legacy qpsk mode: detected code rate changed */
} BAST_ReacqCause;

/* function pointer for ftm idle funct */
typedef BERR_Code (*BAST_FTM_ISR)(BAST_Handle);


/***************************************************************************
Summary:
   Structure for FTM device of the BAST_g3_P_Handle
Description:
   This is the FTM device component of the BAST_g3_P_Handle.
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
} BAST_g3_P_FtmDevice;


/***************************************************************************
Summary:
   Structure for diseqc device of the BAST_g3_P_ChannelHandle
Description:
   This is the diseqc device component of the BAST_g3_P_ChannelHandle.  This
   struct is allocated only if the channel has diseqc capability.
See Also:
   None.
****************************************************************************/
typedef struct BAST_g3_P_DiseqcDevice
{
   BKNI_EventHandle     hDiseqcEvent;             /* diseqc event handle */
   BKNI_EventHandle     hDiseqcOverVoltageEvent;  /* diseqc over-voltage event handle */
   BKNI_EventHandle     hDiseqcUnderVoltageEvent; /* diseqc under-voltage event handle */
   BINT_CallbackHandle  hDiseqcDoneCb;            /* callback handle for diseqc done interrupt */
   BINT_CallbackHandle  hDiseqcOverVoltageCb;     /* callback handle for diseqc over voltage interrupt */
   BINT_CallbackHandle  hDiseqcUnderVoltageCb;    /* callback handle for diseqc under voltage interrupt */
   BINT_CallbackHandle  hDiseqc1TimerCb;          /* callback handle for diseqc 1 timer interrupt */
   BINT_CallbackHandle  hDiseqc2TimerCb;          /* callback handle for diseqc 2 timer interrupt */
   BINT_CallbackHandle  hDiseqcTxFifoAlmostEmptyCb;   /* callback handle for diseqc tx fifo almost empty interrupt */
   BAST_g3_FUNCT        diseqc1TimerIsr;          /* function for handling diseqc timer 1 interrupt */   
   BAST_g3_FUNCT        diseqc2TimerIsr;          /* function for handling diseqc timer 2 interrupt */ 
   BAST_DiseqcStatus    diseqcStatus;             /* status of last diseqc transaction */
   BAST_DiseqcSettings  dsecSettings;             /* diseqc settings */
   uint8_t              diseqcSendBuffer[128];    /* diseqc transmit buffer */
   uint8_t              diseqcSendLen;            /* diseqc transmit length */
   uint8_t              diseqcSendCount;          /* diseqc transmit count */
#ifndef BAST_EXCLUDE_ACW   
   uint8_t              diseqcAcw;                /* auto-control word */
#endif
   bool                 bDiseqcToneOn;            /* true if diseqc tone is enabled */
} BAST_g3_P_DiseqcDevice;


/***************************************************************************
Summary:
   Structure for chip-specific portion of the BAST handle
Description:
   This is the chip-specific component of the BAST_Handle.
See Also:
   None.
****************************************************************************/
typedef struct BAST_g3_P_Handle
{
   BREG_Handle           hRegister;          /* register handle */
   BINT_Handle           hInterrupt;         /* interrupt handle */
   BCHP_Handle           hChip;              /* chip handle */
   BKNI_EventHandle      hInitDoneEvent;     /* init done event handle */
#ifndef BAST_EXCLUDE_FTM                     
   BKNI_EventHandle      hFtmEvent;          /* FTM event handle */
   BAST_g3_P_FtmDevice   hFtmDev;            /* FTM device handle */   
#endif
   BAST_g3_DEV_FUNCT     postInitApFunct;    /* chip-specific function to call after BAST_InitAp() */
   BAST_FskChannel       fskTxChannel;       /* channel for fsk transmit  */
   BAST_FskChannel       fskRxChannel;       /* channel for fsk receive  */
   uint32_t              xtalFreq;           /* crystal freq in Hz */
   uint32_t              searchRange;        /* tuner search range in Hz */
   bool                  bInit;              /* true when AST PI has completely initialized */
#ifndef BAST_EXCLUDE_FTM                     
   bool                  bFtmLocalReset;     /* distinguish between bast reset or local reset */
#endif   
   bool                  bFtmPoweredDown;    /* indicates if ftm core powered down */
#ifndef BAST_EXCLUDE_BCM3445
   bool                  bBcm3445;           /* TRUE = has bcm3445 LNA */
#endif
   bool                  bOpen;              /* true if AST PI has been openned successfully */
   uint8_t               numInternalTuners;  /* number of internal tuners on this chip */
#ifndef BAST_EXCLUDE_FTM                     
   uint8_t               ftmTxPower;         /* config param for transmit power */
#endif
#ifndef BAST_EXCLUDE_BCM3445
   uint8_t               bcm3445Address;     /* i2c address of bcm3445 */
#endif
   uint8_t               sdsRevId;           /* SDS core revision ID */
} BAST_g3_P_Handle;


/***************************************************************************
Summary:
   Structure for chip-specific portion of the BAST channel handle
Description:
   This is the chip-specific component of the BAST_ChannelHandle.
See Also:
   None.
****************************************************************************/
typedef struct BAST_g3_P_ChannelHandle
{
   BKNI_EventHandle     hLockChangeEvent;    /* change of lock status event handle */
   BKNI_EventHandle     hPeakScanEvent;      /* peak scan event handle */   
#ifndef BAST_EXCLUDE_MI2C   
   BKNI_EventHandle     hMi2cEvent;          /* mi2c event handle */
#endif
#ifndef BAST_EXCLUDE_STATUS_EVENTS
   BKNI_EventHandle     hStatusEvent;        /* status event handle */
#endif
   BKNI_EventHandle     hTunerInitDoneEvent; /* tuner init done event handle */
   BAST_AcqSettings     acqParams;           /* input acquisition parameters */
   BAST_AcqState        acqState;            /* acquisition state */
   BINT_CallbackHandle  hBaudTimerCb;           /* callback handle for baud clock timer interrupt */
   BINT_CallbackHandle  hBerTimerCb;            /* callback handle for BER timer interrupt */
   BINT_CallbackHandle  hGen1TimerCb;           /* callback handle for general timer 1 interrupt */
   BINT_CallbackHandle  hGen2TimerCb;           /* callback handle for general timer 2 interrupt */
   BINT_CallbackHandle  hGen3TimerCb;           /* callback handle for general timer 3 interrupt */
   BINT_CallbackHandle  hQpskLockCb;            /* callback handle for Legacy mode lock interrupt */
   BINT_CallbackHandle  hQpskNotLockCb;         /* callback handle for Legacy mode not lock interrupt */
   BINT_CallbackHandle  hHpLockCb;              /* callback handle for HP state reaching rcvr lock */
   BINT_CallbackHandle  hHpFrameBoundaryCb;     /* callback handle for HP frame boundary */   
#ifndef BAST_EXCLUDE_LDPC   
   BINT_CallbackHandle  hLdpcLockCb;            /* callback handle for LDPC lock interrupt */
   BINT_CallbackHandle  hLdpcNotLockCb;         /* callback handle for LDPC not lock interrupt */
#endif   
#ifndef BAST_EXCLUDE_TURBO   
   BINT_CallbackHandle  hTurboLockCb;           /* callback handle for turbo lock interrupt */
   BINT_CallbackHandle  hTurboNotLockCb;        /* callback handle for turbo not lock interrupt */
   BINT_CallbackHandle  hTurboSyncCb;           /* callback handle for turbo sync detected */
#endif   
#ifndef BAST_EXCLUDE_MI2C
   BINT_CallbackHandle  hMi2cCb;                /* callback handle for MI2C interrupt */
#endif
   BAST_g3_FUNCT        baudTimerIsr;        /* function for handling baud timer interrupt */
   BAST_g3_FUNCT        berTimerIsr;         /* function for handling BER timer interrupt */
   BAST_g3_FUNCT        gen1TimerIsr;        /* function for handling general timer 1 interrupt */
   BAST_g3_FUNCT        gen2TimerIsr;        /* function for handling general timer 2 interrupt */
   BAST_g3_FUNCT        gen3TimerIsr;        /* function for handling general timer 3 interrupt */    
   BAST_g3_FUNCT        passFunct;           /* general function pointer used in acquisition state machine */
   BAST_g3_FUNCT        failFunct;           /* general function pointer used in acquisition state machine */
   BAST_g3_FUNCT        postTuneFunct;       /* function to call after tuning */
   BAST_g3_FUNCT        acqFunct;            /* mode-specific acquisition function pointer */
   BAST_g3_FUNCT        checkModeFunct;      /* virtual function to check valid mode */
   BAST_g3_FUNCT        onLockFunct;         /* mode-specific function to handle lock interrupt */
   BAST_g3_FUNCT        onLostLockFunct;     /* mode-specific function to handle lost lock interrupt */
   BAST_g3_FUNCT        onStableLockFunct;   /* mode-specific function to process stable lock event */
   BAST_g3_FUNCT        onMonitorLockFunct;  /* mode-specific function to do periodic lock monitoring */
   BAST_g3_FUNCT        enableLockInterrupts; /* virtual function to enable lock/lost_lock interrupts */
   BAST_g3_FUNCT        disableLockInterrupts; /* virtual function to disable lock/lost_lock interrupts */
   BAST_g3_LOCK_FUNCT   getLockStatusFunct;  /* virtual function to get lock status */
   BAST_g3_P_DiseqcDevice *diseqc;           /* pointer to the diseqc info for this channel */
   BAST_Mode            actualMode;          /* actual mode found during code rate scan */
#ifndef BAST_EXCLUDE_LDPC
   BAST_Mode            acmMaxMode;          /* maximum mode in ACM */
#endif
   BAST_OutputTransportSettings xportSettings;     /* mpeg transport settings */
#ifndef BAST_HAS_WFE
   BAST_TunerLnaSettings      tunerLnaSettings;    /* crossbar configuration of tuner LNA */
#endif
#ifndef BAST_EXCLUDE_SPUR_CANCELLER
   BAST_SpurCancellerConfig   spurConfig[6];
#endif
#ifndef BAST_EXCLUDE_BCM3445   
   BAST_Mi2cChannel           bcm3445Mi2cChannel;  /* identifies which mi2c channel controls the BCM3445 associated with this channel's tuner */
   BAST_Bcm3445OutputChannel  bcm3445TunerInput;   /* output port of the BCM3445 feeding this channel's tuner */
   BAST_Bcm3445Settings       bcm3445Settings;     /* configuration of BCM3445 controlled on this channel's MI2C */
#endif   
   BAST_ReacqCause      reacqCause;          /* cause for reacquisition */
   uint32_t             vcoRefClock;         /* VCO reference clock frequency in Hz */
   uint32_t             sampleFreq;          /* ADC sampling frequency in Hz */
   uint32_t             fecFreq;             /* FEC (Turbo/LDPC) clock frequency in Hz */
   uint32_t             tunerFreq;           /* requested tuner frequency in Hz */
   uint32_t             actualTunerFreq;     /* actual tuner frequency in Hz */
   uint32_t             tunerIfStepSize;     /* internal tuner IF step size in Hz used in freq scan algorithm */
   uint32_t             outputBitRate;       /* output transport bit rate in bps */
   uint32_t             funct_state;         /* specifies current state in acquisition state machine */
   uint32_t             dft_funct_state;     /* current state in dft carrier search state machine */
   uint32_t             prev_state;          /* previous HP state */
   uint32_t             acqCount;            /* number of reacquisitions */
   uint32_t             carrierAcq1Bw;       /* acquisition carrier loop bandwidth in Hz */
   uint32_t             carrierAcq1Damp;     /* acquisition carrier loop damping factor scaled 2^2 */
   uint32_t             carrierAcq2Bw;       /* viterbi carrier loop bandwidth in Hz */
   uint32_t             carrierAcq2Damp;     /* viterbi carrier loop damping factor scaled 2^2 */
   uint32_t             carrierAcq3Bw;       /* narrow carrier loop bandwidth in Hz, previously cmin */
   uint32_t             carrierAcq3Damp;     /* narrow carrier loop damping factor scaled 2^2 */
   uint32_t             carrierTrkBw;        /* final tracking carrier loop bandwidth in Hz */
   uint32_t             carrierTrkDamp;      /* final tracking carrier loop damping factor scaled 2^2 */
   uint32_t             baudAcqBw;           /* baud loop bandwidth in Hz */
   uint32_t             baudAcqDamp;         /* baud loop damping factor scaled 2^2 */
   uint32_t             baudTrkBw;           /* final baud loop bandwidth in Hz, previous bmin */
   uint32_t             baudTrkDamp;         /* final baud loop damping factor scaled 2^2 */
   uint32_t             mpegErrorCount;      /* accumulated MPEG errors */
   uint32_t             mpegFrameCount;      /* accumulated MPEG frames */
   uint32_t             berErrors;           /* accumulated BER error count */
   uint32_t             debug1;              /* debug config param 1 */
   uint32_t             debug2;              /* debug config param 2 */
   uint32_t             debug3;              /* debug config param 3 */
   uint32_t             rsCorr;              /* accumulated RS correctable errors */
   uint32_t             rsUncorr;            /* accumulated RS uncorrectable errors */
   uint32_t             preVitErrors;        /* accumulated pre-Viterbi error count */
   uint32_t             peakScanSymRateMin;  /* minimum symbol rate in peak scan */
   uint32_t             peakScanSymRateMax;  /* maximum symbol rate in peak scan */
   uint32_t             peakScanOutput;      /* peak scan output status */
   uint32_t             acqTime;             /* acquisition time in usecs */
   uint32_t             irqCount[BAST_g3_MaxIntID]; /* interrupt counter */
   uint32_t             altPlcAcqBw;         /* Acquisition PLC BW override value */
   uint32_t             altPlcTrkBw;         /* Tracking PLC BW override value */   
   uint32_t             dft_count1;          /* general counter used in dft carrier search */
   uint32_t             dft_current_max_pow; /* used in dft */
   uint32_t             count1;              /* general counter used in acquisition */
   uint32_t             count2;              /* general counter used in acquisition */
   uint32_t             maxCount1;           /* general counter used in acquisition */ 
   uint32_t             stableLockTimeout;   /* timeout in usecs for determining stable lock */
   uint32_t             maxStableLockTimeout;/* maximum timeout in usecs for determining stable lock */
   uint32_t             lockFilterIncr;      /* used in lock filtering */
   uint32_t             trace[BAST_TraceEvent_eMax]; /* trace buffer */
   uint32_t             carrierDelay;        /* LockViterbi dwell time */
   uint32_t             checkLockInterval;   /* LockViterbi interval */
   uint32_t             opll_N;              /* calculated OPLL N value */
   uint32_t             opll_D;              /* calculated OPLL D value */
   uint32_t             bfos;                /* baud loop frequency control word */
   uint32_t             peakScanIfStepSize;  /* IF step size used in peak scan */
   uint32_t             peakScanMaxPower;    /* max peak power found during peak scan */
   uint32_t             peakScanSymRateEst;  /* symbol rate where max peak power occurs */
   uint32_t             timeSinceStableLock; /* in units of 100 msecs */
#ifdef BAST_ENABLE_FINE_FREQ_ESTIMATION   
   uint32_t             hpFrameCount;        /* current number of frames */ 
#endif   
#ifndef BAST_EXCLUDE_LDPC   
   uint32_t             ldpcTotalBlocks;     /* accumulated total LDPC block count */
   uint32_t             ldpcCorrBlocks;      /* accumulated LDPC correctable block count */
   uint32_t             ldpcBadBlocks;       /* accumulated LDPC bad block count */
#endif      
#ifndef BAST_EXCLUDE_TURBO   
   uint32_t             turboTotalBlocks;    /* accumulated Turbo block count */
   uint32_t             turboCorrBlocks;     /* accumulated Turbo correctable block count */
   uint32_t             turboBadBlocks;      /* accumulated Turbo bad block count */
#endif      
#ifndef BAST_EXCLUDE_STATUS_EVENTS   
   uint32_t             freqDriftThreshold;  /* carrier freq drift threshold in Hz */
   uint32_t             rainFadeSnrAve;      /* ave snr in 1/8 dB during rain fade window */
   uint32_t             rainFadeSnrMax;      /* max snr in 1/8 dB during rain fade window */
   uint32_t             rainFadeWindow;      /* time window for rain fade indicator in units of 100 msecs */
#endif      
#ifndef BAST_HAS_WFE   
   uint32_t             fullTuneFreq;        /* freq at which full tune was done */
   uint32_t             tunerFddfs;          /* internal tuner DDFS frequency in Hz */
   uint32_t             tunerLoDivider;      /* internal tuner LO divider scaled 2^6 */
   uint32_t             tunerVcRefLow;       /* tuner VcRef low threshold */
   uint32_t             tunerVcRefHigh;      /* tuner VcRef high threshold */
   uint32_t             tunerAgcThreshold;   /* value of BB/LNA AGC clip detector thresholds */
   uint32_t             tunerAgcWinLength;   /* value of BB/LNA AGC window lengths */
#endif   
   int32_t              tunerIfStep;         /* used in freq scan */
   int32_t              tunerIfStepMax;      /* used in freq scan */
   int32_t              dftFreqEstimate;     /* used in freq scan */
   int32_t              initFreqOffset;      /* used in carrier freq drift threshold */
   int32_t              lockFilterRamp;      /* used in lock filtering */
   int32_t              freqTransferInt;     /* used in pli-to-fli leak */
   bool                 bHasDiseqc;          /* true if channel supports diseqc */
   bool                 bHasAfec;            /* true if channel supports DVB-S2 */
   bool                 bHasTfec;            /* true if channel supports Turbo */
   bool                 bVitLocked;          /* true if viterbi locked in legacy acquisition */
   bool                 bLocked;             /* true if demod and fec are locked */
   bool                 bLastLocked;         /* filtered last lock status */
   bool                 bFsNotDefault;       /* true if sample freq has changed */
   bool                 bBlindScan;          /* true if blind scan is enabled */   
   bool                 bReacqTimerExpired;  /* true if reacquisition timer expired */
   bool                 bEverStableLock;     /* true if current transponder was ever locked */
   bool                 bStableLock;         /* true if currently stable locked */
   bool                 bTimingLock;         /* true if timing loop is locked */
   bool                 bPlcTracking;        /* true if tracking plc values are used */
   bool                 bUndersample;        /* true if in undersample mode */
   bool                 bMonitorLock;        /* false if in powered down standby mode */
#ifndef BAST_EXCLUDE_EXT_TUNER         
   bool                 bExternalTuner;      /* true if channel uses external tuner */
#endif
#ifndef BAST_HAS_WFE
   bool                 bHasTunerRefPll;     /* true if channel controls internal tuner REF PLL */
   bool                 bOverrideKvco;       /* override kvco */
   bool                 bCalibrateKvco;      /* calibrating kvco */
#else
   bool                 bOverrideFiltctl;
#endif   
#ifndef BAST_EXCLUDE_SPUR_CANCELLER
   bool                 bCwcActive[6];
#endif   
#ifndef BAST_EXCLUDE_MI2C   
   bool                 bMi2cInProgress;     /* true if MI2C is busy */
#endif
#ifndef BAST_EXCLUDE_STATUS_EVENTS   
   bool                 bStatusInterrupts;   /* true if host wants PI to monitor status indicators */
#endif
#ifdef BAST_ENABLE_FINE_FREQ_ESTIMATION
   bool                 bEnableFineFreq;     /* true if DVB-S2 pilot mode */
#endif   
   uint16_t             dftRangeStart;       /* configuration parameter used in tone detect */
   uint16_t             dftRangeEnd;         /* configuration parameter used in tone detect */
   uint16_t             peakScanDftSize;     /* DFT size used in peak scan */
   uint16_t             peakScanMaxPeakBin;  /* bin where max peak power occurs */   
#ifndef BAST_EXCLUDE_TURBO  
   bool                 bTurboSpinv;         /* spectral inversion setting for HP */
   uint16_t             turboScanModes;      /* specifies which turbo modes to search in scan mode */
   uint16_t             turboScanCurrMode;   /* used in turbo scan mode */
#endif
#ifndef BAST_EXCLUDE_LDPC
   uint16_t             ldpcScanModes;       /* specifies which ldpc modes to search in scan mode */
#endif
#ifndef BAST_HAS_WFE   
   uint16_t             tunerCapCntl;        /* tuner mixer PLL VCO cap setting */
   uint16_t             tunerCapMask;        /* search mask for tuner mixer PLL VCO cap */
   uint16_t             tunerVcoFreqMhz;     /* vco calibration freq in MHz */
   uint16_t             tunerAgcAmpThresh;   /* value of BB/LNA AGC amplitude thresholds */
   uint16_t             tunerAgcLoopCoeff;   /* value of BB/LNA AGC loop coefficients */
#endif   
   uint8_t              dftFreqEstimateStatus; /* used in DFT freq estimate algorithm */
   uint8_t              blindScanCurrMode;   /* used in blind acquisition */
   uint8_t              blindScanModes;      /* config parameter used to specify which modes to scan */
   uint8_t              tunerCtl;            /* TUNER_CTL configuration parameter */
   uint8_t              tunerLpfToCalibrate; /* tuner LPF freq to calibrate */
   uint8_t              tunerCutoff;         /* desired internal tuner cutoff frequency in MHz */
   uint8_t              agcCtl;              /* AGC_CTL configuration parameter */
   uint8_t              dtvScanState;        /* used for manual DTV 1/2 scan in DTV scan mode */
   uint8_t              coresPoweredDown;
   uint8_t              lockIsrFlag;
   uint8_t              dtvScanCodeRates;    /* specifies which DTV code rates to search in scan mode */
   uint8_t              dvbScanCodeRates;    /* specifies which DVB code rates to search in scan mode */
   uint8_t              dciiScanCodeRates;   /* specifies which DCII code rates to search in scan mode */
   uint8_t              stuffBytes;            /* number of null stuff bytes */
   uint8_t              reacqCtl;              /* reacquisition control flags */
   uint8_t              altPlcAcqDamp;         /* Acquisition PLC damping factor override value, scaled 2^3 */
   uint8_t              altPlcTrkDamp;         /* Tracking PLC damping factor override value, scaled 2^3 */
   uint8_t              plcCtl;                /* PLC_CTL configuration parameter */
   uint8_t              signalDetectStatus;    /* status of signal detect mode */
   uint8_t              peakScanStatus;        /* status of peak scan */
   uint8_t              dft_done_timeout;      /* used to timeout dft done */
   uint8_t              miscCtl;                /* MISC_CTL configuration parameter */ 
#ifndef BAST_EXCLUDE_TURBO
   uint8_t              turboCtl;            /* TURBO_CTL configuration parameter */
   uint8_t              turboScanState;      /* used in Turbo scan mode */
#endif  
#ifndef BAST_EXCLUDE_LDPC
   uint8_t              ldpcCtl;             /* LDPC_CTL configuration parameter */
   uint8_t              ldpcScanState;       /* used in LDPC scan mode */
   uint8_t              modcod;              /* saved modcod */   
   uint8_t              ldpcScramblingSeq[16]; /* XSEED+PLHDRCFGx sequence */     
#endif   
#ifndef BAST_EXCLUDE_BCM3445   
   uint8_t              bcm3445Status;         /* see BAST_BCM3445_STATUS_* macros in bast.h */
   uint8_t              bcm3445Ctl;            /* BCM3445_CTL configuration parameter */
#endif   
#ifndef BAST_HAS_WFE 
   uint8_t              tunerLnaStatus;        /* see BAST_TUNER_LNA_STATUS_* macros in bast.h */
   uint8_t              tunerFgaCal;         /* calibrated value for FGA */
   uint8_t              tunerLpfCal;         /* calibrated value for LPF */
   uint8_t              tunerFgaCalManual;
   uint8_t              tunerLpfCalManual;
   uint8_t              tunerFilCalUpperThresh; /* upper threshold for tuner filter calibration as percentage of half tone scaled by 100 */
   uint8_t              tunerFilCalLowerThresh; /* lower threshold for tuner filter calibration as percentage of half tone scaled by 100 */
   uint8_t              tunerLsRange;           /* tuner linear cap search range */
   uint8_t              tunerKvcoCntl;          /* tuner kvco_cntl override */
   uint8_t              tunerKvcoCntl_high;     /* high tuner kvco_cntl */
   uint8_t              tunerKvcoCntl_low;      /* low tuner kvoc_cntl */
   uint8_t              tempAmbient;            /* configurable ambient temperature for tuner Vc adjustment */
   uint8_t              tunerDaisyGain;         /* daisy output gain */
   uint8_t              tunerRefPllChannel;     /* parent channel controlling REF PLL */
#endif
#ifndef BAST_EXCLUDE_STATUS_EVENTS      
   uint8_t              rainFadeThreshold;   /* rain fade threshold in 1/8 dB */ 
   uint8_t              statusEventIndicators;
#endif
} BAST_g3_P_ChannelHandle;


/******************************************************************************
BAST_g3_Ftm_P_InterruptCbTable
Summary: Interrupt Callback Table to describe interrupt Names, ISRs, and Masks 
*******************************************************************************/
typedef struct BAST_g3_Ftm_P_InterruptCbTable
{
	BINT_Id       	   IntrId;
	BINT_CallbackFunc pfCallback;
	int               iParam2; 
	bool              enable;
} BAST_g3_Ftm_P_InterruptCbTable ;


/* implementation of API functions */
BERR_Code BAST_g3_P_Open(BAST_Handle *, BCHP_Handle, void*, BINT_Handle, const BAST_Settings *pDefSettings);
BERR_Code BAST_g3_P_Close(BAST_Handle);
BERR_Code BAST_g3_P_GetTotalChannels(BAST_Handle, uint32_t *);
BERR_Code BAST_g3_P_OpenChannel(BAST_Handle, BAST_ChannelHandle *, uint32_t chnNo, const BAST_ChannelSettings *pChnDefSettings);
BERR_Code BAST_g3_P_CloseChannel(BAST_ChannelHandle);
BERR_Code BAST_g3_P_GetDevice(BAST_ChannelHandle, BAST_Handle *);
BERR_Code BAST_g3_P_InitAp(BAST_Handle, const uint8_t *);
BERR_Code BAST_g3_P_SoftReset(BAST_Handle);
BERR_Code BAST_g3_P_ResetChannel(BAST_ChannelHandle, bool);
#ifndef BAST_HAS_LEAP
BERR_Code BAST_g3_P_GetApVersion(BAST_Handle, uint16_t*, uint8_t*, uint32_t*, uint8_t*, uint8_t*);
BERR_Code BAST_g3_P_GetVersionInfo(BAST_Handle h, BFEC_VersionInfo *pVersion);
#endif
BERR_Code BAST_g3_P_TuneAcquire(BAST_ChannelHandle, const uint32_t, const BAST_AcqSettings *);
BERR_Code BAST_g3_P_GetChannelStatus(BAST_ChannelHandle, BAST_ChannelStatus *);
BERR_Code BAST_g3_P_GetLockStatus(BAST_ChannelHandle, bool *);
BERR_Code BAST_g3_P_ResetStatus(BAST_ChannelHandle);
BERR_Code BAST_g3_P_SetDiseqcTone(BAST_ChannelHandle, bool);
BERR_Code BAST_g3_P_GetDiseqcTone(BAST_ChannelHandle, bool*);
BERR_Code BAST_g3_P_SetDiseqcVoltage(BAST_ChannelHandle, bool);
BERR_Code BAST_g3_P_GetDiseqcVoltage(BAST_ChannelHandle, uint8_t*);
BERR_Code BAST_g3_P_EnableDiseqcLnb(BAST_ChannelHandle, bool);
BERR_Code BAST_g3_P_EnableVsenseInterrupts(BAST_ChannelHandle, bool);
#ifndef BAST_EXCLUDE_ACW
BERR_Code BAST_g3_P_SendACW(BAST_ChannelHandle, uint8_t);
BERR_Code BAST_g3_P_DoneACW(BAST_ChannelHandle);
#endif
BERR_Code BAST_g3_P_SendDiseqcCommand(BAST_ChannelHandle, const uint8_t *pSendBuf, uint8_t sendBufLen);
BERR_Code BAST_g3_P_GetDiseqcStatus(BAST_ChannelHandle, BAST_DiseqcStatus *pStatus);
BERR_Code BAST_g3_P_ResetDiseqc(BAST_ChannelHandle, uint8_t options);
#ifndef BAST_EXCLUDE_MI2C 
BERR_Code BAST_g3_P_WriteMi2c(BAST_ChannelHandle, uint8_t, uint8_t*, uint8_t);
BERR_Code BAST_g3_P_ReadMi2c(BAST_ChannelHandle, uint8_t, uint8_t*, uint8_t, uint8_t *, uint8_t);
#endif
BERR_Code BAST_g3_P_GetSoftDecisionBuf(BAST_ChannelHandle, int16_t*, int16_t*);
BERR_Code BAST_g3_P_FreezeEq(BAST_ChannelHandle, bool);
BERR_Code BAST_g3_P_PowerDown(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_g3_P_PowerUp(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_g3_P_ReadRegister(BAST_ChannelHandle, uint32_t, uint32_t*);
BERR_Code BAST_g3_P_WriteRegister(BAST_ChannelHandle, uint32_t, uint32_t*);
BERR_Code BAST_g3_P_ReadConfig(BAST_ChannelHandle, uint16_t, uint8_t*, uint8_t);
BERR_Code BAST_g3_P_WriteConfig(BAST_ChannelHandle, uint16_t, uint8_t*, uint8_t);
BERR_Code BAST_g3_P_GetLockChangeEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
#ifndef BAST_EXCLUDE_FTM      
BERR_Code BAST_g3_P_GetFtmEventHandle(BAST_Handle, BKNI_EventHandle*);
BERR_Code BAST_g3_P_ResetFtm(BAST_Handle);
BERR_Code BAST_g3_P_ReadFtm(BAST_Handle, uint8_t *pBuf, uint8_t *n);
BERR_Code BAST_g3_P_WriteFtm(BAST_Handle, uint8_t *pBuf, uint8_t n);
BERR_Code BAST_g3_P_PowerDownFtm(BAST_Handle);
BERR_Code BAST_g3_P_PowerUpFtm(BAST_Handle);
#endif
BERR_Code BAST_g3_P_GetDiseqcEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
BERR_Code BAST_g3_P_GetDiseqcVsenseEventHandles(BAST_ChannelHandle, BKNI_EventHandle*, BKNI_EventHandle*);
BERR_Code BAST_g3_P_AbortAcq(BAST_ChannelHandle);
BERR_Code BAST_g3_P_PeakScan(BAST_ChannelHandle h, uint32_t tunerFreq);
BERR_Code BAST_g3_P_GetPeakScanStatus(BAST_ChannelHandle h, BAST_PeakScanStatus*);
BERR_Code BAST_g3_P_GetPeakScanEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
#ifndef BAST_EXCLUDE_STATUS_EVENTS   
BERR_Code BAST_g3_P_EnableStatusInterrupts(BAST_ChannelHandle, bool);
BERR_Code BAST_g3_P_GetStatusEventHandle(BAST_ChannelHandle h, BKNI_EventHandle *pEvent);
#endif
BERR_Code BAST_g3_P_EnableSpurCanceller(BAST_ChannelHandle h, uint8_t n, BAST_SpurCancellerConfig *pConfig);
BERR_Code BAST_g3_P_SetSearchRange(BAST_Handle h, uint32_t searchRange);
BERR_Code BAST_g3_P_GetSearchRange(BAST_Handle h, uint32_t *pSearchRange);
#ifndef BAST_EXCLUDE_LDPC
BERR_Code BAST_g3_P_SetAmcScramblingSeq(BAST_ChannelHandle h, uint32_t xseed, uint32_t plhdrscr1, uint32_t plhdrscr2, uint32_t plhdrscr3);
#endif
BERR_Code BAST_g3_P_SetTunerFilter(BAST_ChannelHandle h, uint32_t cutoffHz);
BERR_Code BAST_g3_P_GetSignalDetectStatus(BAST_ChannelHandle h, BAST_SignalDetectStatus *pStatus);
BERR_Code BAST_g3_P_SetOutputTransportSettings(BAST_ChannelHandle, BAST_OutputTransportSettings*);
BERR_Code BAST_g3_P_GetOutputTransportSettings(BAST_ChannelHandle, BAST_OutputTransportSettings*);
BERR_Code BAST_g3_P_SetDiseqcSettings(BAST_ChannelHandle, BAST_DiseqcSettings*);
BERR_Code BAST_g3_P_GetDiseqcSettings(BAST_ChannelHandle, BAST_DiseqcSettings*);
BERR_Code BAST_g3_P_SetNetworkSpec(BAST_Handle, BAST_NetworkSpec);
BERR_Code BAST_g3_P_GetNetworkSpec(BAST_Handle, BAST_NetworkSpec *);
BERR_Code BAST_g3_P_SetFskChannel(BAST_Handle h, BAST_FskChannel fskTxChannel, BAST_FskChannel fskRxChannel);
BERR_Code BAST_g3_P_GetFskChannel(BAST_Handle h, BAST_FskChannel *fskTxChannel, BAST_FskChannel *fskRxChannel);
BERR_Code BAST_g3_P_SetPeakScanSymbolRateRange(BAST_ChannelHandle, uint32_t, uint32_t);
BERR_Code BAST_g3_P_GetPeakScanSymbolRateRange(BAST_ChannelHandle, uint32_t*, uint32_t*);
#ifdef BAST_HAS_WFE
BERR_Code BAST_g3_P_SetAdcSelect(BAST_ChannelHandle h, uint8_t);
BERR_Code BAST_g3_P_GetAdcSelect(BAST_ChannelHandle h, uint8_t*, uint8_t*);
#else
BERR_Code BAST_g3_P_ReadAgc(BAST_ChannelHandle, BAST_Agc, uint32_t*);
BERR_Code BAST_g3_P_WriteAgc(BAST_ChannelHandle, BAST_Agc, uint32_t*);
BERR_Code BAST_g3_P_FreezeAgc(BAST_ChannelHandle, BAST_Agc, bool);
#endif

/* private functions */

/* bast_<chip>.c */
BERR_Code BAST_g3_P_InitHandle(BAST_Handle h);
BERR_Code BAST_g3_P_InitConfig(BAST_ChannelHandle h);

/* bast_g3_priv.c */
BERR_Code BAST_g3_P_DisableChannelInterrupts_isr(BAST_ChannelHandle h, bool bDisableDiseqc, bool bDisableMi2c);
BERR_Code BAST_g3_P_DisableChannelInterrupts(BAST_ChannelHandle h, bool bDisableDiseqc, bool bDisableMi2c);

/* bast_g3_priv_acq.c */
BERR_Code BAST_g3_P_WriteVerifyRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t *val);
BERR_Code BAST_g3_P_LogTraceBuffer(BAST_ChannelHandle h, BAST_TraceEvent event);
BERR_Code BAST_g3_P_ProcessScript(BAST_ChannelHandle hChn, uint32_t const *pScr);
uint32_t BAST_g3_P_GCF(uint32_t m, uint32_t n);
BERR_Code BAST_g3_P_SetFlifOffset(BAST_ChannelHandle h, int32_t offset);
BERR_Code BAST_g3_P_SetDefaultSampleFreq(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_UpdateErrorCounters(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_GetSymbolRateError(BAST_ChannelHandle h, int32_t *pSymRateError);
BERR_Code BAST_g3_P_GetCarrierError(BAST_ChannelHandle h, int32_t *pCarrierError);
BERR_Code BAST_g3_P_GetActualMode(BAST_ChannelHandle h, BAST_Mode *pActualMode);
BERR_Code BAST_g3_P_ResetErrorCounters(BAST_ChannelHandle h);
void BAST_g3_P_IncrementInterruptCounter(BAST_ChannelHandle h, BAST_g3_IntID idx);
BERR_Code BAST_g3_P_InitChannels(BAST_ChannelHandle h, bool bInitNextChannel);
BERR_Code BAST_g3_P_InitChannelHandle(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_PrepareNewAcquisition(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TuneAcquire_isr(BAST_ChannelHandle h);
void BAST_g3_P_IndicateLocked(BAST_ChannelHandle h);
void BAST_g3_P_IndicateNotLocked(BAST_ChannelHandle h);
void BAST_g3_P_Lock_isr(void *p, int param);
void BAST_g3_P_NotLock_isr(void *p, int param);
BERR_Code BAST_g3_P_InitHandleDefault(BAST_Handle h);
void BAST_g3_P_ToggleBit(BAST_ChannelHandle h, uint32_t reg, uint32_t mask);
BERR_Code BAST_g3_P_SetSampleFreq(BAST_ChannelHandle h, uint32_t ndiv, uint32_t mdiv);
BERR_Code BAST_g3_P_GetSampleFreq(BAST_ChannelHandle h, uint32_t *pSampleFreq);
void BAST_g3_P_ReadModifyWriteRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t and_mask, uint32_t or_mask);
void BAST_g3_P_OrRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t or_mask);
void BAST_g3_P_AndRegister(BAST_ChannelHandle h, uint32_t reg, uint32_t and_mask);
BERR_Code BAST_g3_P_SdsPowerUp(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_SdsPowerDown(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_SetFunctTable(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_StartTracking(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_Reacquire(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_SetBaudBw(BAST_ChannelHandle h, uint32_t bw, uint32_t damp);
BERR_Code BAST_g3_P_SetCarrierBw(BAST_ChannelHandle h, uint32_t bw, uint32_t damp);
BERR_Code BAST_g3_P_ConfigOif(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_InitBert(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_StartBert(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_SetAgcTrackingBw(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_SetFfeMainTap(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_ConfigAgc(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_NonLegacyModeAcquireInit(BAST_ChannelHandle h);
void BAST_g3_P_GetRegisterWriteWaitTime(BAST_ChannelHandle h, uint32_t reg, uint32_t *wait_time);
BERR_Code BAST_g3_P_Acquire1(BAST_ChannelHandle h);
void BAST_g3_P_GetCarrierErrorFli(BAST_ChannelHandle h, int32_t *pCarrierErrorFli);
void BAST_g3_P_GetCarrierErrorPli(BAST_ChannelHandle h, int32_t *pCarrierErrorPli);
BERR_Code BAST_g3_P_SdsDisableOif(BAST_ChannelHandle h);
uint32_t BAST_g3_P_GetNumDecimatingFilters(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_ConfigBl(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_SetBfos(BAST_ChannelHandle h, uint32_t two_pow_hb);
void BAST_g3_P_SdsPowerDownOpll(BAST_ChannelHandle h);
void BAST_g3_P_SdsPowerUpOpll(BAST_ChannelHandle h);
#ifndef BAST_EXCLUDE_STATUS_EVENTS
BERR_Code BAST_g3_P_CheckStatusIndicators(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_ResetStatusIndicators(BAST_ChannelHandle h);
#endif
#ifndef BAST_HAS_WFE
void BAST_g3_P_GetSampleFreqMN(BAST_ChannelHandle h, uint32_t *pN, uint32_t *pM);
#endif

/* bast_g3_priv_cwc.c */
#ifndef BAST_EXCLUDE_SPUR_CANCELLER
BERR_Code BAST_g3_P_ClearSpurCancellerConfig(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_DisableSpurCanceller(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_InitCWC(BAST_ChannelHandle h, BAST_g3_FUNCT funct);
BERR_Code BAST_g3_P_ResetCWC(BAST_ChannelHandle h);
#endif

/* bast_g3_priv_dft.c */
BERR_Code BAST_g3_P_DftSearchCarrier(BAST_ChannelHandle h, BAST_g3_FUNCT funct);
BERR_Code BAST_g3_P_DftPeakScan(BAST_ChannelHandle h);

/* bast_g3_priv_diseqc.c */
BERR_Code BAST_g3_P_DiseqcPowerUp(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_DiseqcPowerDown(BAST_ChannelHandle h);
void BAST_g3_P_DiseqcDone_isr(void *p, int param);
void BAST_g3_P_DiseqcOverVoltage_isr(void *p, int param);
void BAST_g3_P_DiseqcUnderVoltage_isr(void *p, int param);
void BAST_g3_P_DiseqcTxFifoAlmostEmpty_isr(void *p, int param);
void BAST_g3_P_DisableDiseqcInterrupts_isr(BAST_ChannelHandle h);

/* bast_g3_priv_ftm.c */
BERR_Code BAST_g3_P_PowerUpFskphy(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_PowerDownFskphy(BAST_ChannelHandle h);
#ifndef BAST_EXCLUDE_FTM                  
#ifndef BAST_ENABLE_SKIT_FSK
BERR_Code BAST_g3_Ftm_P_InitDevice(BAST_Handle);
BERR_Code BAST_g3_Ftm_P_EnableFtmInterrupts(BAST_Handle h, bool bEnable);
void BAST_g3_Ftm_P_HandleInterrupt_isr(void *p, int param);
#else
#define BAST_g3_Ftm_P_InitDevice(h) BAST_g3_Fsk_P_InitDevice(h)
#define BAST_g3_Ftm_P_EnableFtmInterrupts(h, bEnable) BAST_g3_Fsk_P_EnableFskInterrupts(h, bEnable)
#define BAST_g3_Ftm_P_HandleInterrupt_isr BAST_g3_Fsk_P_HandleInterrupt_isr
void BAST_g3_Fsk_P_HandleInterrupt_isr(void *p, int param);
#endif
#endif

/* bast_g3_priv_hp.c */
void BAST_g3_P_HpLock_isr(void *p, int param);
void BAST_g3_P_HpFrameBoundary_isr(void *p, int param);
BERR_Code BAST_g3_P_HpEnable(BAST_ChannelHandle h, bool bEnable);
BERR_Code BAST_g3_P_HpAcquire(BAST_ChannelHandle h, BAST_g3_FUNCT funct);
BERR_Code BAST_g3_P_OnHpTimeOut(BAST_ChannelHandle h);
bool BAST_g3_P_IsHpLocked(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_HpDisableInterrupts(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_HpIsSpinv(BAST_ChannelHandle h, bool *pSpinv);
BERR_Code BAST_g3_P_HpGetFreqOffsetEstimate(BAST_ChannelHandle h, int32_t *pFreq);

/* bast_g3_priv_ldpc.c */
#ifndef BAST_EXCLUDE_LDPC
BERR_Code BAST_g3_P_LdpcSetFunctTable(BAST_ChannelHandle h);
bool BAST_g3_P_IsLdpcPilotOn(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_LdpcPowerUp(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_LdpcPowerDown(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_GetAfecClock(BAST_ChannelHandle h, uint32_t *pFreq);
bool BAST_g3_P_IsLdpcOn(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_LdpcUpdateBlockCounters(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_LdpcDisableLockInterrupts(BAST_ChannelHandle h);
#if (BCHP_CHIP==4528) || (BCHP_CHIP==4538)
BERR_Code BAST_g3_P_LdpcEnableDynamicPowerShutDown(BAST_ChannelHandle h, bool bEnable);
#endif
#endif

/* bast_g3_priv_lna.c */
#ifndef BAST_EXCLUDE_BCM3445
BERR_Code BAST_g3_P_ConfigBcm3445(BAST_Handle, BAST_Bcm3445Settings*);
BERR_Code BAST_g3_P_MapBcm3445ToTuner(BAST_ChannelHandle, BAST_Mi2cChannel, BAST_Bcm3445OutputChannel);
BERR_Code BAST_g3_P_GetBcm3445Status(BAST_ChannelHandle, BAST_Bcm3445Status*);
#endif

/* bast_g3_priv_mi2c.c */
#ifndef BAST_EXCLUDE_MI2C
void BAST_g3_P_Mi2c_isr(void *p, int param);
BERR_Code BAST_g3_P_Mi2cReset(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_Mi2cDisableInterrupts_isr(BAST_ChannelHandle h);
#endif

/* bast_g3_priv_plc.c */
BERR_Code BAST_g3_P_SetPlc(BAST_ChannelHandle h, uint32_t bw, uint32_t damp);
BERR_Code BAST_g3_P_ConfigPlc(BAST_ChannelHandle h, bool bAcq);

/* bast_g3_priv_qpsk.c */
BERR_Code BAST_g3_P_QpskSetFunctTable(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskUpdateErrorCounters(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskDisableLockInterrupts(BAST_ChannelHandle h);

/* bast_g3_priv_snr.c */
BERR_Code BAST_g3_P_GetSnr(BAST_ChannelHandle h, uint32_t *pSnr);

/* bast_g3_priv_timer.c */
BERR_Code BAST_g3_P_EnableTimer(BAST_ChannelHandle h, BAST_TimerSelect t, uint32_t delay, BAST_g3_FUNCT func);
BERR_Code BAST_g3_P_DisableTimer(BAST_ChannelHandle h, BAST_TimerSelect t);
bool BAST_g3_P_IsTimerRunning(BAST_ChannelHandle h, BAST_TimerSelect t);
bool BAST_g3_P_IsTimerExpired(BAST_ChannelHandle h, BAST_TimerSelect t);
void BAST_g3_P_BaudTimer_isr(void *p, int param);
void BAST_g3_P_BerTimer_isr(void *p, int param);
void BAST_g3_P_Gen1Timer_isr(void *p, int param);
void BAST_g3_P_Gen2Timer_isr(void *p, int param);
void BAST_g3_P_Gen3Timer_isr(void *p, int param);
void BAST_g3_P_Diseqc1Timer_isr(void *p, int param);
void BAST_g3_P_Diseqc2Timer_isr(void *p, int param);

/* bast_g3_priv_tuner.c */
BERR_Code BAST_g3_P_TunerQuickTune(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct);
BERR_Code BAST_g3_P_TunerSetFreq(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct);
BERR_Code BAST_g3_P_TunerInit(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct);
BERR_Code BAST_g3_P_TunerPowerUp(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerPowerDown(BAST_ChannelHandle h);
#ifndef BAST_HAS_WFE
BERR_Code BAST_g3_P_TunerSetFilter(BAST_ChannelHandle h, bool bTracking);
BERR_Code BAST_g3_P_TunerConfigLna(BAST_Handle h, BAST_TunerLnaSettings *pSettings);
BERR_Code BAST_g3_P_TunerGetLnaStatus(BAST_ChannelHandle h, BAST_TunerLnaStatus *pStatus);
BERR_Code BAST_g3_P_TunerGetLockStatus(BAST_ChannelHandle h, bool *bRefPllLocked, bool *bMixPllLocked);
BERR_Code BAST_g3_P_TunerIndirectWrite(BAST_ChannelHandle h, BAST_TunerIndirectRegGroup type, uint8_t addr, uint32_t val);
BERR_Code BAST_g3_P_TunerIndirectRead(BAST_ChannelHandle h, BAST_TunerIndirectRegGroup type, uint8_t addr, uint32_t *pVal);
BERR_Code BAST_g3_P_TunerRunDco(BAST_ChannelHandle h, BAST_TimerSelect t, uint16_t runtime);
BERR_Code BAST_g3_P_TunerRunDco1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TunerGetActualLOFreq(BAST_ChannelHandle h, uint32_t *pFreq);
#endif

/* bast_g3_priv_turbo.c */
#ifndef BAST_EXCLUDE_TURBO
BERR_Code BAST_g3_P_TurboPowerUp(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TurboPowerDown(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TurboUpdateErrorCounters(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_GetTfecClock(BAST_ChannelHandle h, uint32_t *pFreq);
BERR_Code BAST_g3_P_TurboSetFunctTable(BAST_ChannelHandle h);
bool BAST_g3_P_IsTurboOn(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TurboDisableLockInterrupts(BAST_ChannelHandle h);
void BAST_g3_P_TurboSync_isr(void *p, int param);
BERR_Code BAST_g3_P_TurboDisableSyncInterrupt(BAST_ChannelHandle h);
#endif

#ifdef BAST_HAS_WFE
BERR_Code BAST_g3_P_SetChanDecFcw(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_GetChanAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr);
BERR_Code BAST_g3_P_ConfigChanAgc(BAST_ChannelHandle h, bool bTracking);
BERR_Code BAST_g3_P_GetChanAciCoeff(BAST_ChannelHandle h, int16_t **p);
BERR_Code BAST_g3_P_SetChanAciCoeff(BAST_ChannelHandle h, int16_t *pCoeff);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BAST_g3_PRIV_H__ */

