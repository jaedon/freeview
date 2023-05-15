/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2_priv_ldpc.c $
 * $brcm_Revision: Hydra_Software_Devel/158 $
 * $brcm_Date: 9/6/12 6:15p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2_priv_ldpc.c $
 * 
 * Hydra_Software_Devel/158   9/6/12 6:15p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/157   10/18/11 2:24p ckpark
 * SWSATFE-152: acquistion time is improved for under sample mode
 * 
 * Hydra_Software_Devel/156   10/5/11 9:46a enavarro
 * SWSATFE-150: spur canceller not supported in 73xx AST
 * 
 * Hydra_Software_Devel/155   5/13/11 9:56a enavarro
 * SWSATFE-91: added BAST_73XX_LDPC_CTL_DISABLE_FEC_REACQ bit
 * 
 * Hydra_Software_Devel/154   3/31/11 10:51a ckpark
 * SWSATFE-109: PLC BW is adjusted for LDPC 8PSK 3/5 (Default Mode)
 * 
 * Hydra_Software_Devel/153   3/29/11 11:24a ckpark
 * SWSATFE-109: PLC BW is adjusted for LDPC 8PSK 2/3, 3/4 (default mode)
 * to improve CCI performance
 * 
 * Hydra_Software_Devel/152   11/30/10 9:50a ckpark
 * SWSATFE-91: reacquire if bForceReacq is set
 * 
 * Hydra_Software_Devel/151   2/16/10 4:29p enavarro
 * SWSATFE-37: added QPSK 1/2 no pilot optimization for ETSI phase noise
 * 
 * Hydra_Software_Devel/150   2/16/10 4:14p enavarro
 * SWSATFE-1: adjusted turbo tracking PLC BW for low symbol rates (< 7.5
 * Msps)
 * 
 * Hydra_Software_Devel/149   2/4/10 9:01a ckpark
 * SWSATFE-33: For LDPC QPSK 1/2, DAFE average is made by 16 frames to
 * improve freq. estimation
 * 
 * Hydra_Software_Devel/148   2/2/10 11:29a ckpark
 * SWSATFE-32: dont enable PGA hysteresis
 * 
 * Hydra_Software_Devel/147   11/11/09 10:30a ckpark
 * SWSATFE-12: retune at every other reacquisition
 * 
 * Hydra_Software_Devel/146   11/6/09 5:13p ckpark
 * SWSATFE-11: added agc converge time before dft freq estimate; disable
 * 3-stage agc during dft freq estimate
 * 
 * Hydra_Software_Devel/145   11/3/09 2:07p enavarro
 * SWSATFE-1: removed BAST_USE_OLD_PLC option; updated PLC tables
 * 
 * Hydra_Software_Devel/144   10/28/09 4:36p enavarro
 * SWSATFE-1: updated PLC tables
 * 
 * Hydra_Software_Devel/143   10/26/09 11:32a ckpark
 * SWSATFE-8: Disable phase coherency check when mod code search. After
 * find mod. code phase coherecncy check is enabled.
 * The number for frames for DAFE calculation is reduced to 4 from 16
 * frames for LDPC non-pilot scan mode.
 * 
 * Hydra_Software_Devel/142   10/22/09 3:47p enavarro
 * SWSATFE-1: adjusted PLC tables
 * 
 * Hydra_Software_Devel/141   10/20/09 11:36a ckpark
 * SWSATFE-1: adjusted trk euro pilot off PLC for > 15Mbaud; disabled
 * phase consistency check for pilot off
 * 
 * Hydra_Software_Devel/140   10/12/09 4:28p enavarro
 * SWSATFE-1: adjusted b0 acq euro pilot off PLC tables
 * 
 * Hydra_Software_Devel/139   10/6/09 10:17a enavarro
 * SWSATFE-1: adjusted PLC tables
 * 
 * Hydra_Software_Devel/138   10/2/09 3:38p enavarro
 * SWSATFE-1: adjusted PLC tables
 * 
 * Hydra_Software_Devel/137   10/1/09 3:58p enavarro
 * SWSATFE-1: adjusted PLC values
 * 
 * Hydra_Software_Devel/136   9/24/09 10:59a enavarro
 * HW4506-60: updated PLC tables
 * 
 * Hydra_Software_Devel/135   9/22/09 9:10a enavarro
 * HW4506-60: have separate scaling for turbo tracking PLC bandwidths;
 * updated PLC tables
 * 
 * Hydra_Software_Devel/134   9/17/09 4:24p enavarro
 * HW4506-79: updated BCH_MPCFG0; removed syncd processing because 73xx
 * already has HW that corrects syncd mismatch
 * 
 * Hydra_Software_Devel/133   9/16/09 3:37p enavarro
 * HW4506-60: updated PLC tables
 * 
 * Hydra_Software_Devel/132   9/15/09 3:06p enavarro
 * SW7325-573: fixed opll programming bug in undersample mode for BCM734x
 * chips
 * 
 * Hydra_Software_Devel/131   9/10/09 9:28a enavarro
 * SW7325-594: moved functions used for fixed point arithmetic to common
 * location
 * 
 * Hydra_Software_Devel/130   9/9/09 9:53a enavarro
 * HW4506-71: added turbo no_sync counter to improve turbo scan
 * acquisition time
 * 
 * Hydra_Software_Devel/129   9/8/09 4:00p enavarro
 * HW4506-67: use new plc programming for turbo modes
 * 
 * Hydra_Software_Devel/128   9/8/09 2:29p enavarro
 * HW4506-79: added BAST_73xx_P_LdpcSetupNotLockedMonitor() and
 * BAST_73xx_P_LdpcMonitorNotLocked()
 * 
 * Hydra_Software_Devel/127   9/4/09 10:06a enavarro
 * SW7325-573: fixed OPLL programming for undersample mode
 * 
 * Hydra_Software_Devel/126   9/2/09 3:08p enavarro
 * HW4506-60: reworked PLC programming
 * 
 * Hydra_Software_Devel/125   8/27/09 5:07p enavarro
 * HW4506-79: changes to improve acquisition time
 * 
 * Hydra_Software_Devel/124   8/10/09 4:26p ronchan
 * PR 55235: changes for turbo registers for 7340 and 7342
 * 
 * Hydra_Software_Devel/123   8/10/09 10:57a ronchan
 * PR 55235: add support for 7340 and 7342
 * 
 * Hydra_Software_Devel/122   7/7/09 10:38a enavarro
 * PR 56135: changed HP17 timeout for turbo scan
 * 
 * Hydra_Software_Devel/121   7/6/09 5:22p ronchan
 * PR 56602: reacquire if OPLL fails to converge
 * 
 * Hydra_Software_Devel/120   7/2/09 8:58a enavarro
 * PR 37881: removed '//' style comments
 * 
 * Hydra_Software_Devel/119   6/30/09 5:23p ckpark
 * PR 54801: EURO mode non-pilot 8PSK 3/5, 2/3, 3/4 PLC is set for above
 * 20Mbaud
 * 
 * Hydra_Software_Devel/118   6/26/09 11:06a ckpark
 * PR 56263: added future support for DCO offset in LDPC acquisition
 * (currently compiled out)
 * 
 * Hydra_Software_Devel/117   6/23/09 4:05p enavarro
 * PR 56135: updated BAST_73xx_P_ProcessHpState17() for turbo scan; apply
 * 10KHz offset to tuner
 * 
 * Hydra_Software_Devel/116   6/22/09 9:32a enavarro
 * PR 56135: make common function for setting ONFN register
 * 
 * Hydra_Software_Devel/115   6/18/09 2:49p enavarro
 * PR 56135: call BAST_73xx_P_BlindReacquire() when reacquiring in blind
 * scan mode; made improvements to turbo scan acquisition time
 * 
 * Hydra_Software_Devel/114   6/12/09 3:50p ckpark
 * PR 54801: fixed logic error in LPPC Non-pilot 8PSK 3/5,2/3,3/4 final
 * PLC setting
 * 
 * Hydra_Software_Devel/113   6/10/09 4:09p ckpark
 * PR 54801: LDPC 8PSK 3/5, 2/3, 3/4 non-pilot mode and above 20Mbaud PLC
 * is optimized for AWGN
 * 
 * Hydra_Software_Devel/112   6/9/09 4:09p ckpark
 * PR 54801: fixed plc setting for ldpc scan
 * 
 * Hydra_Software_Devel/111   6/9/09 2:37p ronchan
 * PR 55860: power down AFEC for turbo mode
 * 
 * Hydra_Software_Devel/110   5/28/09 4:16p ckpark
 * PR 54801: enable new implementation of BAST_73xx_P_LdpcSetPlc()
 * 
 * Hydra_Software_Devel/109   5/26/09 5:15p ckpark
 * PR 51107: enable PLI/FLI freq leak
 * 
 * Hydra_Software_Devel/108   5/26/09 3:49p enavarro
 * PR 55047: set tracking tuner LPF bw before transferring freq offset to
 * LO
 * 
 * Hydra_Software_Devel/107   5/22/09 3:18p enavarro
 * PR 55043: scale lockFilterTime for low symbol rate; fixed FNRM, FNRMR,
 * and FFNORM programming for low symbol rates; updated RLMAX, SIMAX, and
 * MISC1 for LDPC scan mode; added alternate implementation for
 * BAST_73xx_P_LdpcSetPlc(); added BAST_73xx_P_LdpcSetFinalPlcBw()
 * 
 * Hydra_Software_Devel/106   4/21/09 9:12a enavarro
 * PR 54434: fixed ldpc_scan for non-zero scrambling seq idx
 * 
 * Hydra_Software_Devel/105   4/20/09 2:15p enavarro
 * PR 37769: added counters for all interrupts
 * 
 * Hydra_Software_Devel/104   4/16/09 10:53a enavarro
 * PR 51107: disable LO loop tracking
 * 
 * Hydra_Software_Devel/103   4/15/09 10:38a enavarro
 * PR 51107: initialize freqTransferInt
 * 
 * Hydra_Software_Devel/102   4/13/09 10:59a ronchan
 * PR 54006: use DisableChannelInterrupts_isr() for ISR context
 * 
 * Hydra_Software_Devel/101   3/26/09 9:22a enavarro
 * PR 53608: removed unneccessary delay in
 * BAST_73xx_P_LdpcSetHp17Timeout() and BAST_73xx_P_LdpcHp17Timeout()
 * 
 * Hydra_Software_Devel/100   3/12/09 4:26p enavarro
 * PR 51665: update to latest tuner mixpll retry algorithm
 * 
 * Hydra_Software_Devel/99   3/2/09 3:46p enavarro
 * PR 51665: updated tuneMixStatus bits
 * 
 * Hydra_Software_Devel/98   2/26/09 2:24p enavarro
 * PR 51665: implemented revised algorithm for tuner mixpll locking
 * 
 * Hydra_Software_Devel/97   2/18/09 2:59p ckpark
 * PR 51986: PLC is adjusted for LDPC 8PSK 3/5, 2/3,3/4 to improve CCI
 * performance
 * 
 * Hydra_Software_Devel/96   1/30/09 9:38a ronchan
 * PR 51629: added support for 7336
 * 
 * Hydra_Software_Devel/95   1/13/09 11:13a enavarro
 * PR 50993: update turbo block counters in
 * BAST_73xx_P_LdpcUpdateBlockCounters()
 * 
 * Hydra_Software_Devel/94   1/8/09 4:19p enavarro
 * PR 50930: updated LDPC lock filtering; PR 50925: reworked acquisition
 * time calculation
 * 
 * Hydra_Software_Devel/93   1/6/09 11:30a ronchan
 * PR 50834: improved turbo scan time
 * 
 * Hydra_Software_Devel/92   12/19/08 9:27a enavarro
 * PR 49928: implemented new method for transferring carrier offset to
 * tuner LO PLL; enabled phase consistency check (HPCTRL7.3) for no pilot
 * and pilot pll modes; set 100 msec timeout for LDPC lock after timing
 * lock; ensure HP state 0 processing is done before HP state 15
 * 
 * Hydra_Software_Devel/91   11/7/08 5:24p enavarro
 * PR 37769: reworked ldpc lock interrupt filtering
 * 
 * Hydra_Software_Devel/90   11/5/08 11:06a enavarro
 * PR 37769: revert back to old value for SDS_FFCNT
 * 
 * Hydra_Software_Devel/89   10/31/08 3:24p enavarro
 * PR 37769: removed TONE_SEARCH_MODE code; changed FFCNT setting; changes
 * to BAST_73xx_P_LdpcEnableLoTracking() per changes in 4506
 * 
 * Hydra_Software_Devel/88   10/20/08 11:56a ronchan
 * PR 37769: fixed compile warning
 * 
 * Hydra_Software_Devel/87   10/15/08 4:48p enavarro
 * PR 37769: added lock interrupt filtering
 * 
 * Hydra_Software_Devel/86   10/15/08 3:43p enavarro
 * PR 37769: updated BAST_73xx_P_LdpcNotLock_isr() to not call
 * BAST_73xx_P_GetLockStatus()
 * 
 * Hydra_Software_Devel/85   10/14/08 5:23p ckpark
 * PR 37769: changed register settings to be consistent with 4506
 * 
 * Hydra_Software_Devel/84   10/14/08 3:13p enavarro
 * PR 37769: 73xx-B0 register tweaks from Tommy
 * 
 * Hydra_Software_Devel/83   10/8/08 10:50a enavarro
 * PR 37769: changed OVRDSEL setting for B0
 * 
 * Hydra_Software_Devel/82   10/6/08 10:34a ronchan
 * PR 37769: omit turbo script when excluding turbo
 * 
 * Hydra_Software_Devel/81   10/3/08 10:11a enavarro
 * PR 37769: implemented EnableSpurCanceller API function
 * 
 * Hydra_Software_Devel/80   10/1/08 2:20p enavarro
 * PR 37769: disable notch filter setting from previous acquisition; clear
 * error counters during acquisition; keep bch and mpeg errors in sync
 * 
 * Hydra_Software_Devel/79   9/17/08 10:58a ckpark
 * PR 37769: transfer freq offset to tuner for entire freq range
 * 
 * Hydra_Software_Devel/78   8/22/08 2:02p enavarro
 * PR 37769: check if AFEC is powered before accessing AFEC registers
 * 
 * Hydra_Software_Devel/77   8/21/08 5:22p enavarro
 * PR 37769: dont increment acqCount in LdpcReacquire()
 * 
 * Hydra_Software_Devel/76   8/21/08 4:20p enavarro
 * PR 37769: fixed reacquisition bug when bert is enabled
 * 
 * Hydra_Software_Devel/75   8/20/08 5:01p ckpark
 * PR 37881: LDPC 8PSK 5/6 non-pilot, 20Mbaud PLC is changed
 * 
 * Hydra_Software_Devel/74   8/19/08 1:37p ckpark
 * PR 37769: LDPC PLC is adjusted for Pilot off and 20Mbaud cases.
 * 
 * Hydra_Software_Devel/73   8/15/08 3:40p ckpark
 * PR 37881: PLC value is adjuested for Non-pilot modes, ckp
 * 
 * Hydra_Software_Devel/72   8/15/08 9:46a ckpark
 * PR 37881: LDPC 8PSK 8/9, 9/10 PLC setting is changed, ckp
 * 
 * Hydra_Software_Devel/71   8/14/08 4:00p enavarro
 * PR 37769: accumulate LDPC block counters
 * 
 * Hydra_Software_Devel/70   8/6/08 9:44a ronchan
 * PR 37881: fixed compiler warnings
 * 
 * Hydra_Software_Devel/69   8/5/08 4:46p ronchan
 * PR 37881: removed '//' style comments
 * 
 * Hydra_Software_Devel/68   8/5/08 3:48p ckpark
 * PR 37769: LDPC_VLCI for 8psk 8/9, 9/10 is restored to correct value
 * 
 * Hydra_Software_Devel/67   8/4/08 10:51a enavarro
 * PR 37769: fixed HPCTRL6 setting for pilot loopless mode
 * 
 * Hydra_Software_Devel/66   8/1/08 5:43p ckpark
 * PR 37769: 7325B0 PLC BW is optmized for new forward & backward tracking
 * loop, ckp
 * 
 * Hydra_Software_Devel/65   7/16/08 4:30p enavarro
 * PR 37769: tweaked CLPDCTL for turbo; moved setting of FLLC/FLIC
 * 
 * Hydra_Software_Devel/64   7/10/08 10:40a ronchan
 * PR 37769: don't call ResetStatusIndicators() if status events excluded
 * 
 * Hydra_Software_Devel/63   7/3/08 9:56a enavarro
 * PR 37769: fixed BAST_73xx_P_LdpcConfig() for turbo mode
 * 
 * Hydra_Software_Devel/62   7/1/08 2:33p enavarro
 * PR 37769: removed unused code; do 3-stage AGC throughout acquisition
 * 
 * Hydra_Software_Devel/61   7/1/08 12:13p enavarro
 * PR 37769: removed unnecessary write to CGPDWN; change HPCTRL6 for pilot
 * loopless mode
 * 
 * Hydra_Software_Devel/60   6/27/08 10:47a ronchan
 * PR 37881: extended B0 support to include 7335
 * 
 * Hydra_Software_Devel/59   6/26/08 4:34p enavarro
 * PR 37881: moved ldpcScanState bit definitions to bast_73xx_priv.h
 * 
 * Hydra_Software_Devel/58   6/26/08 4:20p ckpark
 * PR 37769: 7325-B0 changes
 * 
 * Hydra_Software_Devel/57   6/24/08 2:24p ronchan
 * PR 37769: added option to disable forward/backward loop
 * 
 * Hydra_Software_Devel/56   6/20/08 10:47a enavarro
 * PR 37769: fixed compile errors
 * 
 * Hydra_Software_Devel/55   6/19/08 4:05p enavarro
 * PR 37769: added support for status events
 * 
 * Hydra_Software_Devel/54   6/16/08 4:27p enavarro
 * PR 37881: fixed bug in reacquiring with ldpc scan
 * 
 * Hydra_Software_Devel/53   6/16/08 2:48p enavarro
 * PR 37769: adjusted carrier BW; updated turbo acquisition
 * 
 * Hydra_Software_Devel/52   6/10/08 5:15p enavarro
 * PR 37769: added support for measuring acq time
 * 
 * Hydra_Software_Devel/51   6/9/08 2:54p enavarro
 * PR 37769: updated turbo acquisition per changes in 4506
 * 
 * Hydra_Software_Devel/50   6/6/08 4:48p ckpark
 * PR 37769: fixed bug in LdpcSetOpll()
 * 
 * Hydra_Software_Devel/49   6/5/08 4:32p enavarro
 * PR 37769: changed LdpcHp17Timeout(); back out previous change to
 * disable LO tracking mode
 * 
 * Hydra_Software_Devel/48   6/3/08 4:19p enavarro
 * PR 37769: compile out FRF stepping; changes to disable LO tracking
 * mode; latest turbo changes from 4506
 * 
 * Hydra_Software_Devel/47   6/3/08 2:24p ckpark
 * PR 37769: changes for 7325-B0
 * 
 * Hydra_Software_Devel/46   5/27/08 9:22a ckpark
 * PR 37769: dont reset LDPC/BCH if BERT enabled and LDPC locked and MPEG
 * not locked
 * 
 * Hydra_Software_Devel/45   5/23/08 5:37p jrubio
 * PR37769: fix typo
 * 
 * Hydra_Software_Devel/44   5/23/08 4:00p ckpark
 * PR 37769: changes for BCM7325-B0
 * 
 * Hydra_Software_Devel/43   5/23/08 11:23a ronchan
 * PR 37769: added option to compile out turbo
 * 
 * Hydra_Software_Devel/41   5/14/08 2:23p enavarro
 * PR 37769: add support for LDPC pilot scan
 * 
 * Hydra_Software_Devel/40   5/13/08 2:01p enavarro
 * PR 37769: turn off pilot pll for LDPC 8PSK 5/6
 * 
 * Hydra_Software_Devel/39   5/9/08 6:05p enavarro
 * PR 37769: changes from Tommy; added support for splitter mode
 * 
 * Hydra_Software_Devel/38   5/6/08 4:06p enavarro
 * PR 37769: renamed some functions to start with Ldpc*; tweaks to turbo
 * acquisition; added BAST_73xx_P_LdpcSetHp17Timeout(); dont transfer
 * carrier error to LO when LO tracking is disabled
 * 
 * Hydra_Software_Devel/37   5/6/08 11:03a enavarro
 * PR 37769: fixed bug where wrong carrier offset was transferred to the
 * tuner in BAST_73xx_P_ProcessHpState17()
 * 
 * Hydra_Software_Devel/36   4/16/08 11:32a enavarro
 * PR 37769: changes for turbo acquisition
 * 
 * Hydra_Software_Devel/35   4/15/08 8:52a enavarro
 * PR 37769: fixed bug in LDPC scan
 * 
 * Hydra_Software_Devel/34   4/7/08 5:24p enavarro
 * PR 37769: clean up debug code; reacquire immediately if ldpc not locked
 * and mpeg not locked
 * 
 * Hydra_Software_Devel/33   3/28/08 11:07a ckpark
 * PR 37769: LDPC acquisition is optimized (Tommy), PLC BW for QPSK 2/3,
 * QPSK 3/5(no-pilot) is optimized (Chan).
 * 
 * Hydra_Software_Devel/32   3/18/08 2:46p enavarro
 * PR 37769: set AFEC_RST when LDPC locked and MPEG not locked
 * 
 * Hydra_Software_Devel/31   3/14/08 2:23p enavarro
 * PR 37769: increased final baud loop BW
 * 
 * Hydra_Software_Devel/30   3/14/08 11:04a enavarro
 * PR 37769: clean up reacquisition logic; use mpeg lock irq instead of
 * ldpc lock irq
 * 
 * Hydra_Software_Devel/29   3/13/08 10:27a enavarro
 * PR 37769: some register changes in turbo acquisition; added timeout for
 * reaching HP state 17; added BAST_73xx_P_LdpcGenerateQpskPdTable()
 * 
 * Hydra_Software_Devel/28   2/19/08 6:29p jrubio
 * PR38507: fix compile false warnings
 * 
 * Hydra_Software_Devel/27   2/8/08 2:19p enavarro
 * PR 37769: removed commented out code
 * 
 * Hydra_Software_Devel/26   2/6/08 4:49p enavarro
 * PR 37769: removed writes to SDS_IRQ in script; turbo checkout
 * 
 * Hydra_Software_Devel/25   2/6/08 10:01a enavarro
 * PR 37769: added turbo lock/not locked callbacks
 * 
 * Hydra_Software_Devel/24   2/4/08 4:15p enavarro
 * PR 37769: renamed BAST_73xx_P_LdpcSetDigctl7() to
 * BAST_73xx_P_TunerSetDigctl7()
 * 
 * Hydra_Software_Devel/23   1/31/08 2:14p enavarro
 * PR 37769: changed scaling of FRF in LO tracking mode; back out mpeg
 * lock timeout changes
 * 
 * Hydra_Software_Devel/22   1/30/08 2:21p enavarro
 * PR 37769: changed scaling of FFNORM; step FRF only when LO tracking
 * enabled
 * 
 * Hydra_Software_Devel/21   1/23/08 3:45p enavarro
 * PR 37769: set DIGCTL7[6:4] based on ANACTL10; fixed LO tracking enable
 * in ldpc scan mode
 * 
 * Hydra_Software_Devel/20   1/22/08 11:26a enavarro
 * PR 37769: changed timeout for ldpc lock after HP state 17 to 500000
 * baud clocks
 * 
 * Hydra_Software_Devel/19   1/21/08 3:40p agin
 * PR38452: abs() function undefined in linux kernel mode.
 * 
 * Hydra_Software_Devel/18   1/18/08 4:35p enavarro
 * PR 37769: fixed auto reacquisition for ldpc scan mode
 * 
 * Hydra_Software_Devel/17   1/18/08 11:36a enavarro
 * PR 37769: changes to auto reacquisition
 * 
 * Hydra_Software_Devel/16   1/17/08 4:50p enavarro
 * PR 37769: fix potential overflow problem in iteration calculation
 * 
 * Hydra_Software_Devel/15   1/14/08 10:24a enavarro
 * PR 37769: changed DIGCTL7 and HPCTRL7 programming
 * 
 * Hydra_Software_Devel/14   1/4/08 2:05p enavarro
 * PR 37769: reset LDPC/BCH datapath if ldpc locked but mpeg not locked
 * 
 * Hydra_Software_Devel/13   12/18/07 11:50a enavarro
 * PR 37769: use BINT callback ISRs
 * 
 * Hydra_Software_Devel/12   12/14/07 5:11p ckpark
 * PR 37769: acquisition carrier bw increased
 * 
 * Hydra_Software_Devel/11   12/14/07 1:35p enavarro
 * PR 37769: fixed ldpc_scan mode
 * 
 * Hydra_Software_Devel/10   12/14/07 9:56a enavarro
 * PR 37769: acquisition tweaks
 * 
 * Hydra_Software_Devel/9   12/13/07 3:57p ckpark
 * PR 33769: reworked HP state processing
 * 
 * Hydra_Software_Devel/8   12/13/07 12:05p enavarro
 * PR 36679: reworked ldpc lock/not lock interrupt processing
 * 
 * Hydra_Software_Devel/7   12/12/07 2:33p enavarro
 * PR 37769: added FRF stepping
 * 
 * Hydra_Software_Devel/6   12/5/07 4:35p agin
 * PR37869: Integrate SDS into 97325 reference software.
 * 
 * Hydra_Software_Devel/5   12/4/07 6:17p ronchan
 * PR 37769: fixed compiler warnings
 * 
 * Hydra_Software_Devel/4   12/4/07 4:40p ronchan
 * PR 37769: turbo implementation
 * 
 * Hydra_Software_Devel/3   11/30/07 2:40p ronchan
 * PR 37769: rollback previous ldpc script changes
 * 
 * Hydra_Software_Devel/2   11/29/07 4:19p ronchan
 * PR 37769: incorporate changes from 4506
 * 
 * Hydra_Software_Devel/1   11/21/07 11:29a ronchan
 * PR 37769: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2_priv.h"

/* #define BAST_LDPC_DEBUG */
BDBG_MODULE(bast_g2_priv_ldpc);


/* this data was generated on 03-28-2011 11:10:50 */		
/* table data range 9, 255 --> OK */		
		
#define BAST_PLC_NUM_SYMRATES 6		
#define BAST_PLC_DAMP_SCALE 8		
		
/* acq_b0_default_piloton */		
static uint8_t BAST_bw_acq_b0_default_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	69	,
	52	,
	33	,
	37	,
	47	,
	69	
};		
		
static uint8_t BAST_damp_acq_b0_default_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	72	,
	70	,
	50	,
	48	,
	28	,
	28	
};		
		
static uint8_t BAST_bw_acq_b0_default_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	46	,
	36	,
	45	,
	45	,
	70	
};		
		
static uint8_t BAST_damp_acq_b0_default_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	62	,
	64	,
	48	,
	46	,
	28	,
	28	
};		
		
static uint8_t BAST_bw_acq_b0_default_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	88	,
	49	,
	36	,
	51	,
	60	,
	72	
};		
		
static uint8_t BAST_damp_acq_b0_default_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	72	,
	68	,
	56	,
	56	,
	36	,
	36	
};		
		
static uint8_t BAST_bw_acq_b0_default_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	85	,
	55	,
	32	,
	42	,
	64	,
	80	
};		
		
static uint8_t BAST_damp_acq_b0_default_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	50	,
	42	,
	40	,
	37	,
	28	,
	28	
};		
		
/* acq_b0_default_pilotoff */		
static uint8_t BAST_bw_acq_b0_default_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	111	,
	71	,
	69	,
	93	,
	97	,
	103	
};		
		
static uint8_t BAST_damp_acq_b0_default_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	82	,
	72	,
	66	,
	60	,
	54	,
	46	
};		
		
static uint8_t BAST_bw_acq_b0_default_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	137	,
	72	,
	78	,
	102	,
	107	,
	103	
};		
		
static uint8_t BAST_damp_acq_b0_default_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	98	,
	86	,
	72	,
	60	,
	54	,
	46	
};		
		
static uint8_t BAST_bw_acq_b0_default_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	135	,
	95	,
	75	,
	88	,
	102	,
	109	
};		
		
static uint8_t BAST_damp_acq_b0_default_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	118	,
	84	,
	86	,
	76	,
	64	,
	56	
};		
		
static uint8_t BAST_bw_acq_b0_default_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	105	,
	111	,
	135	,
	141	,
	136	,
	133	
};		
		
static uint8_t BAST_damp_acq_b0_default_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	98	,
	86	,
	96	,
	78	,
	64	,
	56	
};		
		
/* acq_b0_euro_piloton */		
static uint8_t BAST_bw_acq_b0_euro_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	121	,
	158	,
	188	,
	96	,
	107	,
	109	
};		
		
static uint8_t BAST_damp_acq_b0_euro_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	84	,
	72	,
	70	,
	68	,
	52	,
	48	
};		
		
static uint8_t BAST_bw_acq_b0_euro_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	131	,
	166	,
	153	,
	138	,
	155	,
	150	
};		
		
static uint8_t BAST_damp_acq_b0_euro_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	66	,
	72	,
	66	,
	64	,
	56	,
	48	
};		
		
static uint8_t BAST_bw_acq_b0_euro_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	108	,
	117	,
	136	,
	189	,
	184	,
	156	
};		
		
static uint8_t BAST_damp_acq_b0_euro_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	60	,
	60	,
	60	,
	56	,
	48	
};		
		
static uint8_t BAST_bw_acq_b0_euro_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	102	,
	80	,
	90	,
	84	,
	83	,
	109	
};		
		
static uint8_t BAST_damp_acq_b0_euro_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	90	,
	82	,
	76	,
	64	,
	56	,
	48	
};		
		
/* acq_b0_euro_pilotoff */		
static uint8_t BAST_bw_acq_b0_euro_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	242	,
	185	,
	108	,
	177	,
	175	,
	172	
};		
		
static uint8_t BAST_damp_acq_b0_euro_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	112	,
	112	,
	116	,
	98	,
	76	,
	64	
};		
		
static uint8_t BAST_bw_acq_b0_euro_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	191	,
	154	,
	114	,
	162	,
	175	,
	164	
};		
		
static uint8_t BAST_damp_acq_b0_euro_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	136	,
	120	,
	104	,
	68	,
	64	,
	56	
};		
		
static uint8_t BAST_bw_acq_b0_euro_pilotoff_8PSK_low[] = {		
	255	,
	255	,
	255	,
	254	,
	252	,
	250	
};		
		
static uint8_t BAST_damp_acq_b0_euro_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	160	,
	184	,
	120	,
	84	,
	72	,
	64	
};		
		
static uint8_t BAST_bw_acq_b0_euro_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	223	,
	185	,
	218	,
	162	,
	160	,
	156	
};		
		
static uint8_t BAST_damp_acq_b0_euro_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	160	,
	128	,
	106	,
	92	,
	76	,
	64	
};		
		
/* trk_b0_default_piloton */		
static uint8_t BAST_bw_trk_b0_default_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	45	,
	49	,
	32	,
	42	,
	40	,
	49	
};		
		
static uint8_t BAST_damp_trk_b0_default_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	144	,
	144	,
	104	,
	88	,
	52	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_default_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	47	,
	73	,
	37	,
	48	,
	42	,
	51	
};		
		
static uint8_t BAST_damp_trk_b0_default_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	104	,
	96	,
	88	,
	80	,
	52	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_default_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	26	,
	9	,
	25	,
	32	,
	39	,
	54	
};		
		
static uint8_t BAST_damp_trk_b0_default_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	59	,
	59	,
	52	,
	48	,
	36	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_default_piloton_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	19	,
	9	,
	28	,
	42	,
	41	,
	57	
};		
		
static uint8_t BAST_damp_trk_b0_default_piloton_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	224	,
	176	,
	90	,
	48	,
	28	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_default_piloton_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	44	,
	36	,
	35	,
	42	,
	43	,
	60	
};		
		
static uint8_t BAST_damp_trk_b0_default_piloton_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	82	,
	82	,
	82	,
	82	,
	50	,
	32	
};		
		
static uint8_t BAST_bw_trk_b0_default_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	54	,
	47	,
	48	,
	29	,
	42	,
	63	
};		
		
static uint8_t BAST_damp_trk_b0_default_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	80	,
	80	,
	36	,
	32	,
	28	
};		
		
/* trk_b0_euro_piloton */		
static uint8_t BAST_bw_trk_b0_euro_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	233	,
	234	,
	208	,
	139	,
	133	,
	100	
};		
		
static uint8_t BAST_damp_trk_b0_euro_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	96	,
	84	,
	62	,
	62	,
	54	,
	40	
};		
		
static uint8_t BAST_bw_trk_b0_euro_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	232	,
	232	,
	190	,
	148	,
	143	,
	114	
};		
		
static uint8_t BAST_damp_trk_b0_euro_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	92	,
	78	,
	66	,
	80	,
	60	,
	48	
};		
		
static uint8_t BAST_bw_trk_b0_euro_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	158	,
	183	,
	123	,
	152	,
	143	,
	114	
};		
		
static uint8_t BAST_damp_trk_b0_euro_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	72	,
	64	,
	64	,
	56	,
	48	
};		
		
static uint8_t BAST_bw_trk_b0_euro_piloton_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	146	,
	146	,
	190	,
	239	,
	210	,
	186	
};		
		
static uint8_t BAST_damp_trk_b0_euro_piloton_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	88	,
	76	,
	72	,
	61	,
	56	,
	48	
};		
		
static uint8_t BAST_bw_trk_b0_euro_piloton_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	144	,
	164	,
	190	,
	187	,
	167	,
	150	
};		
		
static uint8_t BAST_damp_trk_b0_euro_piloton_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	80	,
	64	,
	63	,
	56	,
	48	
};		
		
static uint8_t BAST_bw_trk_b0_euro_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	134	,
	124	,
	121	,
	145	,
	133	,
	114	
};		
		
static uint8_t BAST_damp_trk_b0_euro_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	92	,
	64	,
	60	,
	54	,
	48	
};		
		
/* trk_b0_awgn_piloton */		
static uint8_t BAST_bw_trk_b0_awgn_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	56	,
	47	,
	28	,
	31	,
	35	,
	49	
};		
		
static uint8_t BAST_damp_trk_b0_awgn_piloton_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	72	,
	70	,
	50	,
	48	,
	28	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_awgn_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	65	,
	42	,
	30	,
	37	,
	37	,
	51	
};		
		
static uint8_t BAST_damp_trk_b0_awgn_piloton_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	62	,
	64	,
	48	,
	46	,
	28	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_awgn_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	71	,
	45	,
	31	,
	38	,
	39	,
	54	
};		
		
static uint8_t BAST_damp_trk_b0_awgn_piloton_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	60	,
	60	,
	48	,
	47	,
	28	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_awgn_piloton_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	77	,
	47	,
	31	,
	39	,
	41	,
	57	
};		
		
static uint8_t BAST_damp_trk_b0_awgn_piloton_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	60	,
	60	,
	48	,
	48	,
	28	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_awgn_piloton_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	70	,
	45	,
	32	,
	37	,
	43	,
	60	
};		
		
static uint8_t BAST_damp_trk_b0_awgn_piloton_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	50	,
	42	,
	44	,
	44	,
	28	,
	28	
};		
		
static uint8_t BAST_bw_trk_b0_awgn_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	90	,
	44	,
	31	,
	35	,
	45	,
	63	
};		
		
static uint8_t BAST_damp_trk_b0_awgn_piloton_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	50	,
	42	,
	40	,
	37	,
	28	,
	28	
};		
		
/* trk_a1_default_pilotoff */		
static uint8_t BAST_bw_trk_a1_default_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	67	,
	51	,
	45	,
	46	,
	53	,
	79	
};		
		
static uint8_t BAST_damp_trk_a1_default_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	144	,
	128	,
	116	,
	96	,
	80	,
	64	
};		
		
static uint8_t BAST_bw_trk_a1_default_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	64	,
	55	,
	56	,
	77	,
	95	,
	114	
};		
		
static uint8_t BAST_damp_trk_a1_default_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	112	,
	80	,
	72	,
	64	,
	48	,
	40	
};		
		
static uint8_t BAST_bw_trk_a1_default_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	154	,
	142	,
	141	,
	140	,
	138	,
	140	
};		
		
static uint8_t BAST_damp_trk_a1_default_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	130	,
	118	,
	98	,
	84	,
	72	,
	64	
};		
		
static uint8_t BAST_bw_trk_a1_default_pilotoff_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	198	,
	184	,
	177	,
	177	,
	176	,
	179	
};		
		
static uint8_t BAST_damp_trk_a1_default_pilotoff_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	174	,
	136	,
	122	,
	108	,
	88	,
	72	
};		
		
static uint8_t BAST_bw_trk_a1_default_pilotoff_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	151	,
	161	,
	165	,
	165	,
	162	,
	164	
};		
		
static uint8_t BAST_damp_trk_a1_default_pilotoff_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	166	,
	120	,
	112	,
	100	,
	84	,
	72	
};		
		
static uint8_t BAST_bw_trk_a1_default_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	107	,
	104	,
	100	,
	100	,
	100	,
	114	
};		
		
static uint8_t BAST_damp_trk_a1_default_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	162	,
	142	,
	128	,
	112	,
	96	,
	80	
};		
		
/* trk_a1_euro_pilotoff */		
static uint8_t BAST_bw_trk_a1_euro_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	201	,
	182	,
	108	,
	107	,
	106	,
	106	
};		
		
static uint8_t BAST_damp_trk_a1_euro_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	144	,
	120	,
	80	,
	72	,
	68	,
	64	
};		
		
static uint8_t BAST_bw_trk_a1_euro_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	181	,
	219	,
	132	,
	131	,
	129	,
	129	
};		
		
static uint8_t BAST_damp_trk_a1_euro_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	160	,
	96	,
	80	,
	68	,
	64	,
	56	
};		
		
static uint8_t BAST_bw_trk_a1_euro_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	174	,
	109	,
	160	,
	159	,
	157	,
	157	
};		
		
static uint8_t BAST_damp_trk_a1_euro_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	128	,
	104	,
	116	,
	84	,
	72	,
	64	
};		
		
static uint8_t BAST_bw_trk_a1_euro_pilotoff_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	230	,
	219	,
	255	,
	255	,
	252	,
	251	
};		
		
static uint8_t BAST_damp_trk_a1_euro_pilotoff_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	248	,
	120	,
	132	,
	116	,
	96	,
	80	
};		
		
static uint8_t BAST_bw_trk_a1_euro_pilotoff_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	255	,
	250	,
	208	,
	206	,
	204	,
	204	
};		
		
static uint8_t BAST_damp_trk_a1_euro_pilotoff_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	224	,
	246	,
	150	,
	132	,
	104	,
	88	
};		
		
static uint8_t BAST_bw_trk_a1_euro_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	248	,
	255	,
	225	,
	224	,
	221	,
	221	
};		
		
static uint8_t BAST_damp_trk_a1_euro_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	176	,
	184	,
	124	,
	92	,
	76	,
	64	
};		
		
/* trk_a1_awgn_pilotoff */		
static uint8_t BAST_bw_trk_a1_awgn_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	85	,
	71	,
	43	,
	46	,
	53	,
	79	
};		
		
static uint8_t BAST_damp_trk_a1_awgn_pilotoff_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	76	,
	64	,
	67	,
	40	,
	40	
};		
		
static uint8_t BAST_bw_trk_a1_awgn_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	99	,
	72	,
	46	,
	52	,
	56	,
	77	
};		
		
static uint8_t BAST_damp_trk_a1_awgn_pilotoff_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	82	,
	77	,
	59	,
	58	,
	40	,
	40	
};		
		
static uint8_t BAST_bw_trk_a1_awgn_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	55	,
	55	,
	32	,
	45	,
	59	,
	81	
};		
		
static uint8_t BAST_damp_trk_a1_awgn_pilotoff_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	40	,
	80	,
	59	,
	40	,
	40	
};		
		
static uint8_t BAST_bw_trk_a1_awgn_pilotoff_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	113	,
	74	,
	45	,
	49	,
	61	,
	86	
};		
		
static uint8_t BAST_damp_trk_a1_awgn_pilotoff_8PSK_3_4[BAST_PLC_NUM_SYMRATES] = {		
	80	,
	72	,
	63	,
	61	,
	40	,
	40	
};		
		
static uint8_t BAST_bw_trk_a1_awgn_pilotoff_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	116	,
	78	,
	44	,
	57	,
	64	,
	90	
};		
		
static uint8_t BAST_damp_trk_a1_awgn_pilotoff_8PSK_5_6[BAST_PLC_NUM_SYMRATES] = {		
	72	,
	72	,
	59	,
	64	,
	40	,
	40	
};		
		
static uint8_t BAST_bw_trk_a1_awgn_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	131	,
	76	,
	40	,
	55	,
	67	,
	94	
};		
		
static uint8_t BAST_damp_trk_a1_awgn_pilotoff_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	72	,
	72	,
	56	,
	51	,
	40	,
	40	
};		
		
static uint8_t BAST_bw_trk_turbo_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	145	,
	179	,
	133	,
	100	,
	100	,
	114	
};		
		
static uint8_t BAST_damp_trk_turbo_QPSK_low[BAST_PLC_NUM_SYMRATES] = {		
	72	,
	64	,
	38	,
	36	,
	36	,
	36	
};		
		
static uint8_t BAST_bw_trk_turbo_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	145	,
	161	,
	147	,
	150	,
	130	,
	114	
};		
		
static uint8_t BAST_damp_trk_turbo_QPSK_high[BAST_PLC_NUM_SYMRATES] = {		
	64	,
	58	,
	48	,
	40	,
	34	,
	34	
};		
		
static uint8_t BAST_bw_trk_turbo_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	253	,
	225	,
	160	,
	157	,
	210	,
	214	
};		
		
static uint8_t BAST_damp_trk_turbo_8PSK_low[BAST_PLC_NUM_SYMRATES] = {		
	70	,
	64	,
	34	,
	28	,
	25	,
	20	
};		
		
static uint8_t BAST_bw_trk_turbo_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	250	,
	250	,
	240	,
	236	,
	220	,
	214	
};		
		
static uint8_t BAST_damp_trk_turbo_8PSK_high[BAST_PLC_NUM_SYMRATES] = {		
	48	,
	40	,
	40	,
	32	,
	24	,
	19	
};		
		
static unsigned long BAST_plc_symbol_rate[BAST_PLC_NUM_SYMRATES] = {		
	30000000	,
	15000000	,
	7500000	,
	3750000	,
	1875000	,
	1000000	
};		
		
static uint16_t BAST_plc_trk_bw_scale[BAST_PLC_NUM_SYMRATES] = {		
	298	,
	274	,
	231	,
	155	,
	105	,
	70	
};		
		
static uint16_t BAST_plc_acq_bw_scale[BAST_PLC_NUM_SYMRATES] = {		
	314	,
	325	,
	255	,
	167	,
	103	,
	64	
};		
#if 0		
static uint16_t BAST_plc_acq_bw_min[BAST_PLC_NUM_SYMRATES] = {		
	21775	,
	15080	,
	8100	,
	6240	,
	4600	,
	4420	
};		
		
static uint16_t BAST_plc_trk_bw_min[BAST_PLC_NUM_SYMRATES] = {		
	5800	,
	2330	,
	5750	,
	4500	,
	3700	,
	3400	
};		
#endif		
static uint16_t BAST_plc_turbo_trk_bw_scale[BAST_PLC_NUM_SYMRATES] = {		
	76	,
	56	,
	15	,
	14	,
	10	,
	7	
};		
		
/* end of generated data */		
									
			
#define BAST_LDPC_INIT_LOCK_FILTER_TIME 2000
#define BAST_LDPC_LOCK_FILTER_TIME 10000 
#define BAST_LDPC_LOCK_FILTER_TIME_MAX 80000 
#define BAST_LDPC_LOCK_FILTER_INCREMENT 20000
/* #define BAST_DEBUG_PLC */

/* local functions */
BERR_Code BAST_g2_P_LdpcAcquire1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcAcquire2(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcAcquire3(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcAcquire4(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcAcquire5(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcAcquire7(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcGetModcod(BAST_ChannelHandle h);
void BAST_g2_P_LdpcModeConfig(BAST_ChannelHandle h, uint32_t reg, const uint32_t *pVal);
void BAST_g2_P_LdpcSetQpsk(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetHd8psk(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetGoldCodeSeq(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetSftdly(BAST_ChannelHandle h);
bool BAST_g2_P_LdpcIs8psk(BAST_ChannelHandle h);
bool BAST_g2_P_LdpcIsPilotOn(BAST_ChannelHandle h);
bool BAST_g2_P_LdpcIsPilotPll(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetSistep(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetHpctrl(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetBlen(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetHpmode(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetHpParams(BAST_ChannelHandle h);
void BAST_g2_P_LdpcConfig(BAST_ChannelHandle h, uint32_t reg, const uint32_t *pVal);
void BAST_g2_P_LdpcGenerate8pskPdTable(BAST_ChannelHandle h);
void BAST_g2_P_LdpcGenerateQpskPdTable(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetPilotctl(BAST_ChannelHandle h);
void BAST_g2_P_LdpcEnableHpStateChangeIsr(BAST_ChannelHandle h, bool bEnable);
void BAST_g2_P_LdpcSetConfig2(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetMpcfg1(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetOpll(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetPsl(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcGetModcod1(BAST_ChannelHandle h);
static uint8_t BAST_g2_P_LdpcRmd(uint32_t enc_msg);
void BAST_g2_P_ProcessHpState0(BAST_ChannelHandle h);
void BAST_g2_P_ProcessHpState15(BAST_ChannelHandle h);
void BAST_g2_P_ProcessHpState16(BAST_ChannelHandle h);
void BAST_g2_P_ProcessHpState17(BAST_ChannelHandle h);
void BAST_g2_P_LdpcFailedAcquisition(BAST_ChannelHandle h);
void BAST_g2_P_LdpcGetTunerFs(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcHp17Timeout(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcEnableLockIsr(BAST_ChannelHandle h);
void BAST_g2_P_LdpcSetHp17Timeout(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcMonitor(BAST_ChannelHandle h);
void BAST_g2_P_LdpcTransferCarrierToTuner(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcTransferCarrierToTuner1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcMonitorNotLocked(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_LdpcSetupNotLockedMonitor(BAST_ChannelHandle h);
void BAST_g2_P_LdpcLookupPlcBwDamp(uint8_t *pBwTable, uint16_t *pBwScaleTable, uint8_t *pDampTable, int32_t i, uint32_t *pBw, uint32_t *pDamp);
BERR_Code BAST_g2_P_LdpcAcquire1a(BAST_ChannelHandle h);

static const uint32_t BAST_ldpc_eqmode[2] = {0x00000010, 0x0000003A};  
static const uint32_t BAST_ldpc_f0b[2] = {0x10000000, 0x20000000}; 
static const uint32_t BAST_ldpc_plctl[2] = {0x059F2316, 0x059F230E}; 
static const uint32_t BAST_ldpc_phdiv2[2] = {0x18780000, 0x2EF80000}; 
static const uint32_t BAST_ldpc_ffcoef0[2] = {0x11111111, 0x1A1A1A19}; 
static const uint32_t BAST_ldpc_ffcoef1[2] = {0x1010100F, 0x18171513}; 
static const uint32_t BAST_ldpc_ffcoef2[2] = {0x0F0E0D0D, 0x110F0D0A}; 
static const uint32_t BAST_ldpc_ffcoef3[2] = {0x0C0B0A09, 0x07040000}; 
static const uint32_t BAST_ldpc_ffcoef4[2] = {0x08070604, 0x00000000}; 
static const uint32_t BAST_ldpc_ffcoef5[2] = {0x03010000, 0x00000000}; 

#ifndef BAST_EXCLUDE_TURBO
static const uint32_t BAST_turbo_eqmode[2] = {0x00000000, 0x0000000A};
static const uint32_t BAST_turbo_eqffe1[2] = {0x00000031, 0x00000029};
static const uint32_t BAST_turbo_eqblnd[2] = {0x00000044, 0x00000044};
static const uint32_t BAST_turbo_clpdctl[2] = {0x0000001A, 0x00000012};
static const uint32_t BAST_turbo_cloon[2] = {0x00000000, 0x00000000};
static const uint32_t BAST_turbo_vlctl1[2] = {0x00000005, 0x00000045};
static const uint32_t BAST_turbo_vlctl3[2] = {0x00000004, 0x00000005};
static const uint32_t BAST_turbo_hpctrl4[2] = {0x00000055, 0x00000057};
static const uint32_t BAST_turbo_qpsk[2] = {0x01000000, 0x01000000};
static const uint32_t BAST_turbo_plc[2] = {0x050F0410, 0x050F0410};
static const uint32_t BAST_turbo_cormsk[2] = {0x00000000, 0x00000008};
static const uint32_t BAST_turbo_trnlen[2] = {0x0000007F, 0x0000003F};
static const uint32_t BAST_turbo_blen[2] = {0x00288F00, 0x00284F00};
static const uint32_t BAST_turbo_hpbp[2] = {0x001E8B00, 0x001E4B00};
static const uint32_t BAST_turbo_ofnseq[2] = {0x00000000, 0x80000000};
static const uint32_t BAST_turbo_tzsy[2] = {0x0007070F, 0x0002070F};
#endif

static const uint32_t BAST_ldpc_plhdrcfg[] =
{
   0x00000024,  /* LDPC QPSK 1/2 */
   0x00000025,  /* LDPC QPSK 3/5 */
   0x00000026,  /* LDPC QPSK 2/3 */
   0x00000027,  /* LDPC QPSK 3/4 */
   0x00000028,  /* LDPC QPSK 4/5 */
   0x00000029,  /* LDPC QPSK 5/6 */
   0x0000002a,  /* LDPC QPSK 8/9 */
   0x0000002b,  /* LDPC QPSK 9/10 */
   0x0000002c,  /* LDPC 8PSK 3/5 */
   0x0000002d,  /* LDPC 8PSK 2/3 */
   0x0000002e,  /* LDPC 8PSK 3/4 */
   0x0000002f,  /* LDPC 8PSK 5/6 */
   0x00000030,  /* LDPC 8PSK 8/9 */
   0x00000031   /* LDPC 8PSK 9/10 */
};

static const uint32_t BAST_ldpc_config_1[] =
{
   0x00050103,  /* LDPC QPSK 1/2 */
   0x00070104,  /* LDPC QPSK 3/5 */
   0x00080105,  /* LDPC QPSK 2/3 */
   0x00080106,  /* LDPC QPSK 3/4 */
   0x00080107,  /* LDPC QPSK 4/5 */
   0x00080108,  /* LDPC QPSK 5/6 */
   0x00080109,  /* LDPC QPSK 8/9 */
   0x0008010a,  /* LDPC QPSK 9/10 */
   0x00080204,  /* LDPC 8PSK 3/5 */
   0x00080205,  /* LDPC 8PSK 2/3 */
   0x00080206,  /* LDPC 8PSK 3/4 */
   0x00080208,  /* LDPC 8PSK 5/6 */
   0x00090209,  /* LDPC 8PSK 8/9 */
   0x0009020a   /* LDPC 8PSK 9/10 */
};

static const uint32_t BAST_ldpc_bch_deccfg0[] =
{
   0x7e907dd0,  /* LDPC QPSK 1/2 */
   0x97e09720,  /* LDPC QPSK 3/5 */
   0xa8c0a820,  /* LDPC QPSK 2/3 */
   0xbdd8bd18,  /* LDPC QPSK 3/4 */
   0xca80c9c0,  /* LDPC QPSK 4/5 */
   0xd2f0d250,  /* LDPC QPSK 5/6 */
   0xe100e080,  /* LDPC QPSK 8/9 */
   0xe3d0e350,  /* LDPC QPSK 9/10 */
   0x97e09720,  /* LDPC 8PSK 3/5 */
   0xa8c0a820,  /* LDPC 8PSK 2/3 */
   0xbdd8bd18,  /* LDPC 8PSK 3/4 */
   0xd2f0d250,  /* LDPC 8PSK 5/6 */
   0xe100e080,  /* LDPC 8PSK 8/9 */
   0xe3d0e350   /* LDPC 8PSK 9/10 */
};

static const uint32_t BAST_ldpc_bch_deccfg1[] =
{
   0x0C5A0001,  /* LDPC QPSK 1/2 */
   0x0C6C0001,  /* LDPC QPSK 3/5 */
   0x0A780001,  /* LDPC QPSK 2/3 */
   0x0C870001,  /* LDPC QPSK 3/4 */
   0x0C900001,  /* LDPC QPSK 4/5 */
   0x0A960001,  /* LDPC QPSK 5/6 */
   0x08A00001,  /* LDPC QPSK 8/9 */
   0x08A20001,  /* LDPC QPSK 9/10 */
   0x0C6C0001,  /* LDPC 8PSK 3/5 */
   0x0A780001,  /* LDPC 8PSK 2/3 */
   0x0C870001,  /* LDPC 8PSK 3/4 */
   0x0A960001,  /* LDPC 8PSK 5/6 */
   0x08A00001,  /* LDPC 8PSK 8/9 */
   0x08A20001   /* LDPC 8PSK 9/10 */
};

static const uint32_t BAST_ldpc_vlci[] =
{
   0x2c4d0000,  /* LDPC QPSK 1/2 */
   0x391d0000,  /* LDPC QPSK 3/5 */
   0x3fd10000,  /* LDPC QPSK 2/3 */
   0x3cb40000,  /* LDPC QPSK 3/4 */
   0x3ab50000,  /* LDPC QPSK 4/5 */
   0x395f0000,  /* LDPC QPSK 5/6 */
   0x371f0000,  /* LDPC QPSK 8/9 */
   0x36b60000,  /* LDPC QPSK 9/10 */
   0x27f80000,  /* LDPC 8PSK 3/5 */
   0x267c0000,  /* LDPC 8PSK 2/3 */
   0x2f800000,  /* LDPC 8PSK 3/4    */
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
   0x2d000000,  /* LDPC 8PSK 5/6 */
#else
   0x32e80000,  /* LDPC 8PSK 5/6 */
#endif 
   0x26e80000,  /* LDPC 8PSK 8/9 */
   0x26b80000   /* LDPC 8PSK 9/10 */
};

static const uint32_t BAST_ldpc_snrht[] =
{
   0x03c03235,  /* LDPC QPSK 1/2 */
   0x02e95977,  /* LDPC QPSK 3/5 */
   0x026bf466,  /* LDPC QPSK 2/3 */
   0x01ec7290,  /* LDPC QPSK 3/4 */
   0x01a3240c,  /* LDPC QPSK 4/5 */
   0x01758f45,  /* LDPC QPSK 5/6 */
   0x0128ba9e,  /* LDPC QPSK 8/9 */
   0x011b5fbc,  /* LDPC QPSK 9/10 */
   0x0299deea,  /* LDPC 8PSK 3/5 */
   0x0210eb81,  /* LDPC 8PSK 2/3 */
   0x01881801,  /* LDPC 8PSK 3/4 */
   0x011594c5,  /* LDPC 8PSK 5/6 */
   0x00c916fa,  /* LDPC 8PSK 8/9 */
   0x00bbaafa   /* LDPC 8PSK 9/10 */
};

static const uint32_t BAST_ldpc_snrlt[] =
{
   0x2581f613,  /* LDPC QPSK 1/2 */
   0x2581f613,  /* LDPC QPSK 3/5 */
   0x18378c00,  /* LDPC QPSK 2/3 */
   0x133c79a2,  /* LDPC QPSK 3/4 */
   0x105f6879,  /* LDPC QPSK 4/5 */
   0x0e9798ae,  /* LDPC QPSK 5/6 */
   0x0b974a29,  /* LDPC QPSK 8/9 */
   0x0b11bd5a,  /* LDPC QPSK 9/10 */
   0x1a02b525,  /* LDPC 8PSK 3/5 */
   0x14a9330f,  /* LDPC 8PSK 2/3 */
   0x0f50f00e,  /* LDPC 8PSK 3/4 */
   0x0ad7cfb3,  /* LDPC 8PSK 5/6 */
   0x07dae5c5,  /* LDPC 8PSK 8/9 */
   0x0754adc5   /* LDPC 8PSK 9/10 */
};


static const uint32_t BAST_turbo_titr[] =
{
   0x007F2824,  /* turbo qpsk 1/2 */
   0x00D4C828,  /* turbo qpsk 2/3 */
   0x00BF4830,  /* turbo qpsk 3/4 */
   0x00D4C836,  /* turbo qpsk 5/6 */
   0x00DF483A,  /* turbo qpsk 7/8 */
   0x00FF8024,  /* turbo 8psk 2/3 */
   0x01118028,  /* turbo 8psk 3/4 */
   0x01190030,  /* turbo 8psk 4/5 */
   0x01328036,  /* turbo 8psk 5/6 */
   0x013F803A,  /* turbo 8psk 8/9 */
};

#define abs(x) ((x)<0?-(x):(x))

/******************************************************************************
 BAST_g2_P_LdpcAcquire() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;

   hChn->bForceReacq = false;
   
   if (hChn->acqParams.mode == BAST_Mode_eLdpc_scan)
   {
      hChn->ldpcScanState = BAST_LDPC_SCAN_ON;
      hChn->actualMode = BAST_Mode_eUnknown;
   }
   else
   {
      hChn->ldpcScanState = BAST_LDPC_SCAN_OFF;
      hChn->actualMode = hChn->acqParams.mode;
   }

#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
   if (((hChn->ldpcCtl & BAST_G2_LDPC_CTL_DISABLE_AUTO_PILOT_PLL) == 0) &&
       ((hChn->ldpcScanState & BAST_LDPC_SCAN_ON) == 0))
   {
      if ((hChn->actualMode == BAST_Mode_eLdpc_Qpsk_1_2) ||
          (hChn->actualMode == BAST_Mode_eLdpc_Qpsk_3_5) ||
          ((hChn->actualMode >= BAST_Mode_eLdpc_8psk_3_5) && (hChn->actualMode <= BAST_Mode_eLdpc_8psk_5_6)))
      {
         /* turn off pilot pll */
         hChn->acqParams.acq_ctl &= ~BAST_ACQSETTINGS_LDPC_PILOT_PLL;
      }
      else
      {
         /* turn on pilot pll */
         hChn->acqParams.acq_ctl |= BAST_ACQSETTINGS_LDPC_PILOT_PLL;
      }
   }
#else
   /* disable pilot pll for 7325/7335 B0 */
   hChn->acqParams.acq_ctl &= ~BAST_ACQSETTINGS_LDPC_PILOT_PLL;
#endif
   
   if (hChn->firstTimeLock == 0)
   {
      /* BDBG_MSG(("initializing lockFilterTime")); */
      hChn->lockFilterTime = BAST_LDPC_INIT_LOCK_FILTER_TIME;   
      if (hChn->acqParams.symbolRate < 10000000)
      {
         /* scale lockFilterTime for symbol rate */
         BAST_MultU32U32(hChn->lockFilterTime, 10000000, &P_hi, &P_lo);
         BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &Q_lo);         
         hChn->lockFilterTime = Q_lo;
      }      
   }
   hChn->funct_state = 0;
   return BAST_g2_P_LdpcAcquire1(h);
}


/******************************************************************************
 BAST_g2_P_LdpcAcquire1()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val, val2, filtctl, data0, data1, data2;

   static const uint32_t script_ldpc_0[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_FIRQSTS5, 0x00003C00), /* disable 3-stage agc interrupts */
      BAST_SCRIPT_WRITE(BCHP_SDS_OIFCTL2, 0x08),
      BAST_SCRIPT_WRITE(BCHP_SDS_PILOTCTL, 0x00), 
      BAST_SCRIPT_WRITE(BCHP_SDS_OVRDSEL, 0xFFFF0000),
   
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL1, 0x50),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL2, 0x01),      
      BAST_SCRIPT_AND_OR(BCHP_SDS_SPLL_PWRDN, 0xFFFFFFE7, 0x08), /* power up afec, power down tfec */
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL1, 0x01),      /* reset HP state machine */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL5, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW, 0x0E0E0000),
      BAST_SCRIPT_AND_OR(BCHP_SDS_AGICTL, 0xFFFFFF28, 0x22),
      BAST_SCRIPT_AND_OR(BCHP_SDS_AGTCTL, 0xFFFFFF28, 0x02),
      BAST_SCRIPT_WRITE(BCHP_SDS_AII, 0x00000000),      /* set IF to min */
      BAST_SCRIPT_WRITE(BCHP_SDS_AIT, 0xFFFFFFF0),      /* set RF to max */
      BAST_SCRIPT_WRITE(BCHP_SDS_NTCH_CTRL, 0x00000F30), /* disable any notch filter that was on from the previous acquisition */
      /* BAST_SCRIPT_WRITE(BCHP_SDS_CGPDWN, 0x00), */

      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL1, 0x00),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL5, 0x00),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL2, 0x00),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL6, 0x00),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL4, 0x02),
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW, 0x0E0E0000),
      BAST_SCRIPT_WRITE(BCHP_SDS_AGI, 0x00010000),
      BAST_SCRIPT_WRITE(BCHP_SDS_AGT, 0xFFFF0000),
      BAST_SCRIPT_AND(BCHP_SDS_AGICTL, 0x28),
      BAST_SCRIPT_AND(BCHP_SDS_AGTCTL, 0x28),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_ldpc_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_AFEC_LDPC_CONFIG_0, 0x02000002),   /* reset LDPC FEC */
      BAST_SCRIPT_WRITE(BCHP_AFEC_LDPC_CONFIG_0, 0x00000002),
      BAST_SCRIPT_WRITE(BCHP_SDS_IFMAX, 0x00),      /* software workaround to reset HP */
      BAST_SCRIPT_WRITE(BCHP_SDS_PSMAX, 0x00ff0000),
      BAST_SCRIPT_WRITE(BCHP_SDS_BLEN, 0x00010000),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL1, 0x03),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_ldpc_2[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_SNRCTL, 0x03),     /* more average, ckp */
      BAST_SCRIPT_WRITE(BCHP_SDS_ADC, 0x7e7e0000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSTGCTL, 0x02),
      BAST_SCRIPT_WRITE(BCHP_SDS_ADCPCTL, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_IQPHS, 0x10),
      BAST_SCRIPT_WRITE(BCHP_SDS_IQAMP, 0x20),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL15, 0xFF), /* open up DCO bw in case of re-acq with no tune */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL16, 0xFF), /* make sure it doesn't interact with high AGC bw */
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_ldpc_3[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL7, 0x24), /* always acquire with tuner not in LO tracking mode*/
      BAST_SCRIPT_WRITE(BCHP_SDS_MIXCTL, 0x03),    /* front mixer control */
      BAST_SCRIPT_WRITE(BCHP_SDS_CLCTL, 0x0F),     /* carrier loop is controlled by HP */
      BAST_SCRIPT_WRITE(BCHP_SDS_CLOON, 0x88),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLQCFD, 0x00),    /* ??? pilot/non-pilot, QPSK/8PSK, full-full/soft decision PD */
      BAST_SCRIPT_WRITE(BCHP_SDS_CLMISC, 0x07),    /* ??? these setting are good for pilot mode only, need to */
      BAST_SCRIPT_WRITE(BCHP_SDS_CLMISC2, 0x60),   /* ??? add programmability for other modes */
      BAST_SCRIPT_WRITE(BCHP_SDS_CLSTS, 0x00),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_ldpc_4[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL1, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLTD, 0x20000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLI, 0x00000000),   /* reset front loop integrator */
      BAST_SCRIPT_WRITE(BCHP_SDS_FLPA, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLTD, 0x20000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLI, 0x00000000),   /* reset back loop integrator */
      BAST_SCRIPT_WRITE(BCHP_SDS_PLPA, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_BRI, 0x00000000),   /* reset timing loop integrator */
      BAST_SCRIPT_WRITE(BCHP_SDS_CLFFCTL, 0x02),
#if 0      
      BAST_SCRIPT_WRITE(BCHP_SDS_FLLC, 0x20000100),  /* ??? set up loop bandwidth for front carrier loop */
      BAST_SCRIPT_WRITE(BCHP_SDS_FLIC, 0x40000200),  /* ??? this is not needed for pilot mode */
      BAST_SCRIPT_WRITE(BCHP_SDS_FLLC1, 0x01000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLIC1, 0x01000000),
#endif      
      BAST_SCRIPT_WRITE(BCHP_SDS_BLPCTL1, 0x07),          /* freeze baud loop */
      BAST_SCRIPT_WRITE(BCHP_SDS_BLPCTL2, 0x01),          /* was 0x03 */
      BAST_SCRIPT_WRITE(BCHP_SDS_BRLC, 0x00099C10),
      BAST_SCRIPT_WRITE(BCHP_SDS_BRIC, 0x0000033C),
      BAST_SCRIPT_WRITE(BCHP_SDS_PLTD, 0x28000000),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_ldpc_5[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW, 0x0B0B0000),  /* setting up AGC loop, narrowed, ckp */
      BAST_SCRIPT_WRITE(BCHP_SDS_EQFFE2, 0x06),		
      BAST_SCRIPT_WRITE(BCHP_SDS_EQMU, 0x03),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQMISC, 0x14),   /* set PLHEADER auto save */
      BAST_SCRIPT_WRITE(BCHP_SDS_EQBLND, 0x04),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQCFAD, 0x4C),   /* initialization of EQ tap, org. 0x46 */
      BAST_SCRIPT_WRITE(BCHP_SDS_FERR, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_CLCTL, 0x0F),   /* carrier loop is controlled by HP */
      
#if ((BCHP_CHIP!=7325) && (BCHP_CHIP!=7335)) || (BCHP_VER >= BCHP_VER_B0)
      BAST_SCRIPT_WRITE(BCHP_SDS_CLFBCTL, 0x12),
#endif      
      
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_ldpc_6[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_CLPDCTL, 0xDA),            
      BAST_SCRIPT_WRITE(BCHP_SDS_FLLC, 0x20000100),  /* ??? set up loop bandwidth for front carrier loop */
      BAST_SCRIPT_WRITE(BCHP_SDS_FLIC, 0x40000200),  /* ??? this is not needed for pilot mode */
      BAST_SCRIPT_WRITE(BCHP_SDS_FLLC1, 0x01000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLIC1, 0x01000000),       
      BAST_SCRIPT_EXIT
   };
   
#ifndef BAST_EXCLUDE_TURBO
   static const uint32_t script_ldpc_7[] = /* turbo only */
   {         
      BAST_SCRIPT_WRITE(BCHP_SDS_FLLC, 0x01000100),  
      BAST_SCRIPT_WRITE(BCHP_SDS_FLIC, 0x01000000), 
      BAST_SCRIPT_WRITE(BCHP_SDS_FLLC1, 0x01000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_FLIC1, 0x01000000),  
      BAST_SCRIPT_WRITE(BCHP_SDS_CLMISC2, 0x62),  
      BAST_SCRIPT_WRITE(BCHP_SDS_F0B, 0x19000000),       
      BAST_SCRIPT_EXIT
   };   
#endif

   if ((hChn->bExternalTuner == false) && hChn->funct_state)
      BAST_g2_P_3StageAgc_isr(h, 0);     
            
   switch (hChn->funct_state)
   {
      case 0:
#ifdef BAST_LDPC_DEBUG      
         BDBG_MSG(("starting LDPC acquisition"));
#endif         
         hChn->bFrfTransferred = false;
         hChn->frfStep = 0;         
         BAST_g2_P_ProcessScript(h, script_ldpc_0);
         BAST_g2_P_SetAthr(h);
         
         if (hChn->bExternalTuner == false)
         {
            BAST_g2_P_3StageAgc_isr(h, 0);          
            BAST_g2_P_Enable3StageAgc(h, true);          
         }
            
         if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
            BAST_g2_P_ProcessScript(h, script_ldpc_1);

         BAST_g2_P_SetEqffe3(h);
         BAST_g2_P_SetNyquistAlpha(h);

         BAST_g2_P_ProcessScript(h, script_ldpc_2);
         hChn->funct_state = 1;
         return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, BAST_g2_P_LdpcAcquire1); 

      case 1:
         BAST_g2_P_AndRegister(h, BCHP_SDS_AGICTL, 0xFFFFFF28);
         BAST_g2_P_AndRegister(h, BCHP_SDS_AGTCTL, 0xFFFFFF28);
         hChn->funct_state = 2;
         return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, BAST_g2_P_LdpcAcquire1); 

      case 2:
         val = 0x05;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_AGFCTL, &val); /* reset AGF */
         val = 0x0A0A0000;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_AGF, &val);
         hChn->funct_state = 3;
         return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, BAST_g2_P_LdpcAcquire1); 

      case 3:
         val = 0x00;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_AGFCTL, &val); /* enable AGF */
         BAST_g2_P_LdpcGetTunerFs(h);
         BAST_g2_P_TunerSetDigctl7(h, 0x04);
         BAST_g2_P_ProcessScript(h, script_ldpc_3);
         hChn->funct_state = 4;
         return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, BAST_g2_P_LdpcAcquire1); 

      case 4:
         BAST_g2_P_ProcessScript(h, script_ldpc_4);     
         
         if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_TUNER_TEST_MODE)
            return BAST_g2_P_TunerTestMode(h);
         hChn->funct_state = 5;  /* drop down to state 5 */

      case 5:         
         if (BAST_g2_P_LdpcIsPilotLoopless(h))
         {
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
            val = 0xC3;
#else
            val = 0xC0;
#endif
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL6, &val);
            val = 0x39;
         }
         else
         {
            val = 0xC0;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL6, &val);         
            val = 0x31;
         }
         BAST_g2_P_WriteRegister(h, BCHP_SDS_EQFFE1, &val); /* enable AGF */
         BAST_g2_P_SetEqffe3(h);
         BAST_g2_P_ProcessScript(h, script_ldpc_5);

         if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
         {
            /* LDPC */
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_EQMODE, BAST_ldpc_eqmode);
            
            BAST_g2_P_ProcessScript(h, script_ldpc_6);        
            
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_F0B, BAST_ldpc_f0b);
            val = 0x15;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_VLCTL1, &val); 
            val = 0x09;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_VLCTL2, &val);
            val = 0x04;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_VLCTL3, &val); 
            BAST_g2_P_LdpcSetQpsk(h);
            BAST_g2_P_LdpcSetHd8psk(h);
            BAST_g2_P_LdpcSetGoldCodeSeq(h);
            val = 0x00000100;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_XTAP1, &val); 
            val = 0x00805000;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_XTAP2, &val); 
            val = 0x14;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_PLDCTL, &val); 
            BAST_g2_P_LdpcSetSftdly(h);
         }
#ifndef BAST_EXCLUDE_TURBO
         else
         {
            /* TURBO */
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_SDS_SPLL_PWRDN, 0xFFFFFFE7, 0x10);   /* power down afec, power up tfec */
            val = 0;
            BAST_g2_P_WriteRegister(h, BCHP_TFEC_TTUR, &val); 
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_EQMODE, BAST_turbo_eqmode);
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_EQFFE1, BAST_turbo_eqffe1);
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_EQBLND, BAST_turbo_eqblnd);
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_CLPDCTL, BAST_turbo_clpdctl);
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_CLOON, BAST_turbo_cloon);
            
            BAST_g2_P_ProcessScript(h, script_ldpc_7);
            
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_VLCTL1, BAST_turbo_vlctl1);
            val = 0x07;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_VLCTL2, &val); 
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_VLCTL3, BAST_turbo_vlctl3);
            val = 0x76303000;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_VCOS, &val); 
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_QPSK, BAST_turbo_qpsk);
            val = 0x014E01D9;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HD8PSK1, &val); 
            val = 0x008B00C4;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HD8PSK2, &val); 
            val = 0x00;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_PLDCTL, &val);             
         }
#endif    
         
         BAST_g2_P_ReadRegister(h, BCHP_SDS_BFOS, &val);
         val = val << 1;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_FNRM, &val);
         BAST_g2_P_WriteRegister(h, BCHP_SDS_FNRMR, &val);
         val2 = val >> 3;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_FFNORM, &val2);

         BAST_g2_P_ReadRegister(h, BCHP_SDS_FILTCTL, &filtctl);
         filtctl &= 0x9F;
         if (filtctl == 0x9F)  /* no decimation filter */
         {
            data0 = 0; /* FNRM lshift */
            data1 = 1; /* FNRMR lshift */
            data2 = 1; /* FFNORM lshift */
         }
         else if (filtctl == 0x9E)   /* 1 decimation filter */
         {
            data0 = 1; /* FNRM lshift */
            data1 = 2; /* FNRMR lshift */
            data2 = 2; /* FFNORM lshift */
         }
         else if (filtctl == 0x9C)   /* 2 decimation filters */
         {
            data0 = 2; /* FNRM lshift */
            data1 = 3; /* FNRMR lshift */
            data2 = 3; /* FFNORM lshift */
         }
         else if (filtctl == 0x98)   /* 3 decimation filters */
         {
            data0 = 3; /* FNRM lshift */
            data1 = 4; /* FNRMR lshift */
            data2 = 4; /* FFNORM lshift */
         }   
         else if (filtctl == 0x90)   /* 4 decimation filters */
         {
            data0 = 4; /* FNRM lshift */
            data1 = 5; /* FNRMR lshift */
            data2 = 5; /* FFNORM lshift */
         }  
         else if (filtctl == 0x80)   /* 5 decimation filters */
         {
            data0 = 6; /* FNRM lshift */
            data1 = 5; /* FNRMR lshift */
            data2 = 5; /* FFNORM lshift */
         }          
         else                           /* 6 decimation filters */
         {
            data0 = 7; /* FNRM lshift */
            data1 = 6; /* FNRMR lshift */
            data2 = 6; /* FFNORM lshift */
         }

         val = val2 << data0;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_FNRM, &val);

         val = val2 << data1;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_FNRMR, &val);

         val = val2 << data2;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_FFNORM, &val);

         val = 0x1F;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_CLCTL, &val);

         if (BAST_g2_P_TunerMixpllLostLock(h))
         {
#ifdef BAST_LDPC_DEBUG         
            BDBG_MSG(("tuner mix pll not locked"));
#endif      
            hChn->tuneMixStatus |= BAST_TUNE_MIX_NEXT_RETRY;                      
            return BAST_g2_P_LdpcReacquire(h);
         }

         hChn->funct_state = 0xFF;
         if (((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == 0) ||
             ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON) ||
             ((hChn->acqParams.mode == BAST_Mode_eTurbo_scan) && (hChn->turboScanState & BAST_TURBO_SCAN_HP_LOCKED)))
         {
            BAST_g2_P_3StageAgc_isr(h, 0);
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 2000, BAST_g2_P_LdpcAcquire1a);
         }      

         return BAST_g2_P_LdpcAcquire2(h);

      default:
         BDBG_ERR(("invalid state"));
         BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
         break;
   }
  
   return retCode;
}


/******************************************************************************
 BAST_g2_P_LdpcAcquire1a()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire1a(BAST_ChannelHandle h)
{
   BAST_g2_P_Enable3StageAgc(h, false); /* disable 3stage during dft freq estimate */
   BAST_g2_P_3StageAgc_isr(h, 0);
   return BAST_g2_P_DoDftFreqEstimate(h, BAST_g2_P_LdpcAcquire2);   
}


/******************************************************************************
 BAST_g2_P_LdpcAcquire2()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   static const uint32_t script_ldpc_6[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL7, 0x24),  /* always acquire with tuner not in LO tracking mode */
      BAST_SCRIPT_WRITE(BCHP_SDS_MIXCTL, 0x03),     /* front mixer control */
      BAST_SCRIPT_WRITE(BCHP_SDS_CLCTL, 0x0F),      /* carrier loop is controlled by HP */
      BAST_SCRIPT_WRITE(BCHP_SDS_BRLC, 0x004CE080),  /* open more after acquire time lock */
      BAST_SCRIPT_WRITE(BCHP_SDS_BRIC, 0x00033CF7),
      BAST_SCRIPT_EXIT
   };

   BAST_g2_P_Enable3StageAgc(h, true); /* re-enable 3stage agc after dft freq estimate */
   BAST_g2_P_ProcessScript(h, script_ldpc_6); 

   hChn->funct_state = 0;
   return BAST_g2_P_LdpcAcquire3(h);
}


/******************************************************************************
 BAST_g2_P_LdpcAcquire3()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire3(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   static const uint32_t script_ldpc_7[] =
   {
      /* BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL8, 0x0b), */
      BAST_SCRIPT_WRITE(BCHP_SDS_HPPDWN, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_WSMAX, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_ONFN, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_BLSCCP, 0xA2),  /* leave min # of times fec_phase is stable as 2 */
      BAST_SCRIPT_WRITE(BCHP_SDS_CORMSK, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_TRNLEN, 0x59),
      BAST_SCRIPT_WRITE(BCHP_SDS_DPTH, 0x00100000),
      BAST_SCRIPT_WRITE(BCHP_SDS_TMTH, 0x00000800),
      BAST_SCRIPT_WRITE(BCHP_SDS_MGTH, 0x000f0000),
      BAST_SCRIPT_WRITE(BCHP_SDS_TNRM, 0x000000ff),
      BAST_SCRIPT_WRITE(BCHP_SDS_OFNSEQ, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_MISC2, 0x09010a03),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPBP, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_MSMAX, 0x02010000),  /* magnitude search, not used */
      BAST_SCRIPT_WRITE(BCHP_SDS_PVMAX, 0x02010000),  /* peak verify, not used */
      BAST_SCRIPT_WRITE(BCHP_SDS_SITHRSH, 0x0af01510), /* get back to this due to SpInv acq. under no noise */
      BAST_SCRIPT_EXIT
   };
   
#ifndef BAST_EXCLUDE_TURBO
   static const uint32_t script_turbo_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL5, 0x28),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL6, 0xC0),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL7, 0x48),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL8, 0x03),
      BAST_SCRIPT_WRITE(BCHP_SDS_TFAVG, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_WSMAX, 0x01),
      BAST_SCRIPT_WRITE(BCHP_SDS_IFMAX, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_FMAX, 0x01),      
      BAST_SCRIPT_WRITE(BCHP_SDS_FMMAX, 0x04),
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_turbo_2[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_DPTH, 0x00080000),
      BAST_SCRIPT_WRITE(BCHP_SDS_TMTH, 0x00000300),
      BAST_SCRIPT_WRITE(BCHP_SDS_MGTH, 0x000F0000),
      BAST_SCRIPT_WRITE(BCHP_SDS_TNRM, 0x000000FF),
#if 0      
      BAST_SCRIPT_WRITE(BCHP_SDS_FNRM, 0x01200000),
      BAST_SCRIPT_WRITE(BCHP_SDS_FNRMR, 0x01200000),
#endif      
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_turbo_3[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_MSMAX, 0x03020000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PVMAX, 0x03020000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PSMAX, 0x06020000),      
      BAST_SCRIPT_WRITE(BCHP_SDS_TCMAX, 0x05030410),
      BAST_SCRIPT_WRITE(BCHP_SDS_TLMAX, 0x0501000A),
      BAST_SCRIPT_WRITE(BCHP_SDS_RLMAX, 0x20100000),
      BAST_SCRIPT_WRITE(BCHP_SDS_SFTDLY, 0x00010000),
      BAST_SCRIPT_WRITE(BCHP_SDS_PILOTCTL, 0),      
      BAST_SCRIPT_WRITE(BCHP_SDS_PLHDRCFG, 0x00000001),
      BAST_SCRIPT_WRITE(BCHP_SDS_OVRDSEL, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_HPCTRL1, 0x03),         /* enable HP */
      BAST_SCRIPT_WRITE(BCHP_SDS_OPLL, 0x0000000F),
      BAST_SCRIPT_WRITE(BCHP_SDS_OPLL2, 0x00000001),
      BAST_SCRIPT_EXIT
   };
#endif

   while (hChn->funct_state != 0xFF)
   {
      switch (hChn->funct_state)
      {
         case 0:
            if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
            {
               /* LDPC */
               val = 0x00;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_IFMAX, &val);
               val = 0x00FF0000;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_PSMAX, &val);
               val = 0x00010000;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_BLEN, &val);
               val = 0x03;
               
               BAST_g2_P_LdpcSetPlc(h, true);                            
               
               BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL1, &val);
               hChn->funct_state = 1;
               return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 200, BAST_g2_P_LdpcAcquire3); 
            }
#ifndef BAST_EXCLUDE_TURBO
            else
            {
               /* TURBO */
               BAST_g2_P_LdpcSetPlc(h, true);              
              
               val = 0x08;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_HPMODE, &val);
               val = 0x41;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL1, &val);
               val = 0x00;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL2, &val);
               val = 0x82;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL3, &val);
               BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_HPCTRL4, BAST_turbo_hpctrl4);
               
               BAST_g2_P_ProcessScript(h, script_turbo_1);
               
               BAST_g2_P_TurboSetOnfn(h);
               
               val = 0x40;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_BLSCCP, &val);
               BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_CORMSK, BAST_turbo_cormsk);
               BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_TRNLEN, BAST_turbo_trnlen);
               
               BAST_g2_P_ProcessScript(h, script_turbo_2);
               
               BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_BLEN, BAST_turbo_blen);
               BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_OFNSEQ, BAST_turbo_ofnseq);
               val = 0x00040004;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_MISC1, &val);
               val = 0x09010a03;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_MISC2, &val);
               BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_HPBP, BAST_turbo_hpbp);
               
               BAST_g2_P_ProcessScript(h, script_turbo_3);
               
               hChn->funct_state = 2;
            }
#endif
            break;

         case 1:
            val = 0x06020000;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_SIMAX, &val);                
            val = 0x00010001;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_MISC1, &val);  
            
            BAST_g2_P_LdpcSetHpParams(h);

            val = 0x01;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL1, &val);

            BAST_g2_P_LdpcSetHpmode(h);
            BAST_g2_P_LdpcSetBlen(h);

            val = 0x00;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL2, &val);

            val = 0x82;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL3, &val);

            BAST_g2_P_LdpcSetHpctrl(h);
            BAST_g2_P_ProcessScript(h, script_ldpc_7);
            BAST_g2_P_LdpcSetSistep(h);

            val = 0x2ef82e20;  /* phase estimate divider */
            BAST_g2_P_WriteRegister(h, BCHP_SDS_PHDIV1, &val);

            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_PHDIV2, BAST_ldpc_phdiv2);

            val = 0x020F0000; /* fine frequency set up (pilot mode only) */
            BAST_g2_P_WriteRegister(h, BCHP_SDS_FFCNT, &val);

            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_FFCOEF0, BAST_ldpc_ffcoef0);  /* needed for pilot mode only */
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_FFCOEF1, BAST_ldpc_ffcoef1);
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_FFCOEF2, BAST_ldpc_ffcoef2);
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_FFCOEF3, BAST_ldpc_ffcoef3);
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_FFCOEF4, BAST_ldpc_ffcoef4);
            BAST_g2_P_LdpcModeConfig(h, BCHP_SDS_FFCOEF5, BAST_ldpc_ffcoef5);

            val = 0;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_FFCOEF6, &val);

            BAST_g2_P_LdpcConfig(h, BCHP_SDS_PLHDRCFG, BAST_ldpc_plhdrcfg); /* physical layer scrambler set up */

            BAST_g2_P_LdpcSetPilotctl(h);

            val = 0x03;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL1, &val);  /* enable HP */

            hChn->funct_state = 2;
            break;

         case 2:
            if (hChn->bFrfTransferred)
            {
               val = 0;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_FMAX, &val);
               BAST_g2_P_WriteRegister(h, BCHP_SDS_FMMAX, &val);
            }
            
            BAST_g2_P_TunerSetDigctl7(h, 0x04);

            if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))  /* LDPC mode */
            {
              if ((BAST_g2_P_LdpcIsPilotOn(h) == false) || BAST_g2_P_LdpcIsPilotPll(h))
              {
                 if (BAST_g2_P_LdpcIs8psk(h))
                    BAST_g2_P_LdpcGenerate8pskPdTable(h);
                 else
                    BAST_g2_P_LdpcGenerateQpskPdTable(h);
              }

              if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
              {
                 /* clear and enable HP reacquire interrupt for modcod */
                 BAST_g2_P_OrRegister(h, BCHP_SDS_IRQ, 0x02000200);
              }
              else
                 goto set_hp17_timeout;
            }
            else
            {
              /* set timeout for reaching HP state 17 */
              set_hp17_timeout:
              BAST_g2_P_LdpcSetHp17Timeout(h);                
            }

            hChn->funct_state = 0xFF;
            break;
      }
   }

   hChn->prev_state = 0xFF;
   hChn->count1 = 1;
   hChn->count2 = 0; /* count2=prev hp states visited */
    
#ifdef BAST_LDPC_DEBUG      
   BDBG_MSG(("enabling HP state change interrupt"));
#endif   

   BAST_g2_P_LdpcEnableHpStateChangeIsr(h, true);
   return BERR_SUCCESS; 
}


/******************************************************************************
 BAST_g2_P_LdpcSetHp17Timeout()
******************************************************************************/
void BAST_g2_P_LdpcSetHp17Timeout(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, timeout;
   
   if (hChn->actualMode <= BAST_Mode_eLdpc_Qpsk_2_3)
      timeout = 200000;
   else if (hChn->actualMode <= BAST_Mode_eLdpc_Qpsk_9_10)
      timeout = 180000;
   else if (hChn->actualMode <= BAST_Mode_eLdpc_8psk_3_4)
      timeout = 160000;
   else if (hChn->actualMode <= BAST_Mode_eLdpc_8psk_9_10)
      timeout = 140000;
   else if (BAST_MODE_IS_TURBO(hChn->actualMode))
   {
      if ((hChn->acqParams.mode == BAST_Mode_eTurbo_scan) && (hChn->acqCount < 5))
         timeout = 80000;
      else
         timeout = 200000;
   }
   else
      timeout = 500000;

   BAST_MultU32U32(timeout, 20000000, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &P_hi, &timeout);
   BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, timeout, BAST_g2_P_LdpcHp17Timeout);
}

                
/******************************************************************************
 BAST_g2_P_LdpcModeConfig()
******************************************************************************/
void BAST_g2_P_LdpcModeConfig(BAST_ChannelHandle h, uint32_t reg, const uint32_t *pVal)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, idx = 0;

   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      /* ldpc */
      if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
      {
         if ((hChn->ldpcScanState & BAST_LDPC_SCAN_QPSK) == 0)
            goto mode_8psk;
      }
      else if (BAST_MODE_IS_LDPC_8PSK(hChn->actualMode))
      {
         mode_8psk:
         idx = 1;
      }
   }
   else if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      idx = 1;

   val = pVal[idx];
   BAST_g2_P_WriteRegister(h, reg, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcGetTunerFs()
******************************************************************************/
void BAST_g2_P_LdpcGetTunerFs(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, val;

   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL14, &val);
   BAST_MultU32U32(hChn->tunerFddfs, ((val & 1) ? 16 : 12), &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 128, &Q_hi, &(hChn->tunerFs));
}


/******************************************************************************
 BAST_g2_P_LdpcSetQpsk()
******************************************************************************/
void BAST_g2_P_LdpcSetQpsk(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, i;

   static const uint32_t sds_qpsk_table[] =
   {
      0x016A0000, /* LDPC 8PSK 3/5 */
      0x016A0000, /* LDPC 8PSK 2/3 */
      0x01200000, /* LDPC 8PSK 3/4 */
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
      0x016A0000, /* LDPC 8PSK 5/6 */
#else
      0x01400000, /* LDPC 8PSK 5/6 */
#endif
      0x016A0000, /* LDPC 8PSK 8/9 */
      0x016A0000  /* LDPC 8PSK 9/10 */
   };

   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
   {
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_QPSK)
         goto ldpc_set_qpsk_1;
      else
      {
         /* assume 8PSK 3/5 in scan mode */
         i = 0;
         goto ldpc_set_qpsk_0;
      }
   }
   else if (BAST_MODE_IS_LDPC_8PSK(hChn->actualMode))
   {
      /* LDPC 8PSK */
      i = hChn->actualMode - BAST_Mode_eLdpc_8psk_3_5;

      ldpc_set_qpsk_0:
      val = sds_qpsk_table[i];
   }
   else
   {
      /* LDPC QPSK */
      ldpc_set_qpsk_1:
      val = 0x01000000;
   }
   BAST_g2_P_WriteRegister(h, BCHP_SDS_QPSK, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcSetHd8psk()
******************************************************************************/
void BAST_g2_P_LdpcSetHd8psk(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, val1, qpsk;

   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
   {
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_QPSK)
         goto ldpc_set_hd8psk_1;
      else
         goto ldpc_set_hd8psk_0;
   }
   else if (BAST_MODE_IS_LDPC_8PSK(hChn->actualMode))
   {
      /* LDPC 8PSK */
      ldpc_set_hd8psk_0:
      BAST_g2_P_ReadRegister(h, BCHP_SDS_QPSK, &qpsk);
      qpsk = (qpsk >> 16) & 0x03FF;
      val = (qpsk * 473) / 362;  /* 0x16A=362 */
      val &= 0x3FFF;
      BAST_g2_P_ReadRegister(h, BCHP_SDS_HD8PSK1, &val1);
      val1 &= 0x0000FFFF;
      val1 |= (val << 16);
      BAST_g2_P_WriteRegister(h, BCHP_SDS_HD8PSK1, &val1);

      val = (qpsk * 196) / 362;  /* 0xC4=196 */
      val &= 0x3FFF;
      BAST_g2_P_ReadRegister(h, BCHP_SDS_HD8PSK2, &val1);
      val1 &= 0x0000FFFF;
      val1 |= (val << 16);
      BAST_g2_P_WriteRegister(h, BCHP_SDS_HD8PSK2, &val1);
   }
   else
   {
      /* LDPC QPSK */
      ldpc_set_hd8psk_1:
      val = 0x014E01D9;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_HD8PSK1, &val);
      val = 0x008B00C4;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_HD8PSK2, &val);
   }
}


/******************************************************************************
 BAST_g2_P_LdpcSetSetSftdly()
******************************************************************************/
void BAST_g2_P_LdpcSetSftdly(BAST_ChannelHandle h) 
{
   uint32_t val, vlctl3;

   if (BAST_g2_P_LdpcIs8psk(h))
      val = 0x00001635; /* 8PSK */
   else
      val = 0x0000161A; /* QPSK */

   if ((BAST_g2_P_LdpcIsPilotOn(h) == false) || (BAST_g2_P_LdpcIsPilotPll(h)))
   {
      if (BAST_g2_P_LdpcIs8psk(h))
      {
         vlctl3 = 0x06;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_VLCTL3, &vlctl3);
      }
      val |= 0x00020000;
   }
#if ((BCHP_CHIP!=7325) && (BCHP_CHIP!=7335)) || (BCHP_VER >= BCHP_VER_B0)
   else   /* forward/backward loop */
   {
      /* pilot only */
      if (BAST_g2_P_LdpcIs8psk(h))
      {
         vlctl3 = 0x06;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_VLCTL3, &vlctl3);
      }
   }
#endif   
      
   BAST_g2_P_WriteRegister(h, BCHP_SDS_SFTDLY, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcSetGoldCodeSeq()
******************************************************************************/
void BAST_g2_P_LdpcSetGoldCodeSeq(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   val = (hChn->ldpcScramblingSeq[0] << 24) | (hChn->ldpcScramblingSeq[1] << 16) | 
         (hChn->ldpcScramblingSeq[2] << 8) | hChn->ldpcScramblingSeq[3];
   BAST_g2_P_WriteRegister(h, BCHP_SDS_XSEED, &val);
   val = (hChn->ldpcScramblingSeq[4] << 24) | (hChn->ldpcScramblingSeq[5] << 16) | 
         (hChn->ldpcScramblingSeq[6] << 8) | hChn->ldpcScramblingSeq[7];
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PLHDRSCR1, &val);
   val = (hChn->ldpcScramblingSeq[8] << 24) | (hChn->ldpcScramblingSeq[9] << 16) | 
         (hChn->ldpcScramblingSeq[10] << 8) | hChn->ldpcScramblingSeq[11];
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PLHDRSCR2, &val);
   val = (hChn->ldpcScramblingSeq[12] << 24) | (hChn->ldpcScramblingSeq[13] << 16) | 
         (hChn->ldpcScramblingSeq[14] << 8) | hChn->ldpcScramblingSeq[15];
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PLHDRSCR3, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcIs8psk()
******************************************************************************/
bool BAST_g2_P_LdpcIs8psk(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
   {
      /* ldpc scan in progress */
      if ((hChn->ldpcScanState & BAST_LDPC_SCAN_QPSK) == 0)
         return true;
   }
   else if (BAST_MODE_IS_LDPC(hChn->actualMode))
   {
      if (BAST_MODE_IS_LDPC_8PSK(hChn->actualMode))
         return true;
   }
   else if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      return true;
   return false;
}


/******************************************************************************
 BAST_g2_P_LdpcIsPilotOn()
******************************************************************************/
bool BAST_g2_P_LdpcIsPilotOn(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if (hChn->ldpcScanState & BAST_LDPC_SCAN_MASK)
   {
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_PILOT)
         return true;
   }
   else if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT)
      return true;
   return false;
}


/******************************************************************************
 BAST_g2_P_LdpcIsPilotLoopless()
******************************************************************************/
bool BAST_g2_P_LdpcIsPilotLoopless(BAST_ChannelHandle h)
{
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if (BAST_g2_P_LdpcIsPilotOn(h))
   {        
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_MASK)
      {       
         if (hChn->ldpcScanState & BAST_LDPC_SCAN_FOUND)
         {
            if ((hChn->actualMode == BAST_Mode_eLdpc_Qpsk_1_2) ||
                (hChn->actualMode == BAST_Mode_eLdpc_Qpsk_3_5) ||
                ((hChn->actualMode >= BAST_Mode_eLdpc_8psk_3_5) && (hChn->actualMode < BAST_Mode_eLdpc_8psk_5_6)))
            {
               return true;
            }
         }
         else
            return false; /* assume pilot loop while scanning */
      }
      else if ((hChn->acqParams.acq_ctl & (BAST_ACQSETTINGS_LDPC_PILOT | BAST_ACQSETTINGS_LDPC_PILOT_PLL)) == BAST_ACQSETTINGS_LDPC_PILOT)
         return true;      
   }
   return false;
#else
   return BAST_g2_P_LdpcIsPilotOn(h);
#endif
}


/******************************************************************************
 BAST_g2_P_LdpcIsPilotPll()
******************************************************************************/
bool BAST_g2_P_LdpcIsPilotPll(BAST_ChannelHandle h)
{
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   if (BAST_g2_P_LdpcIsPilotOn(h))
   {  
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_MASK)
      {
         if (hChn->ldpcScanState & BAST_LDPC_SCAN_FOUND)
         {
            if (!((hChn->actualMode == BAST_Mode_eLdpc_Qpsk_1_2) ||
                (hChn->actualMode == BAST_Mode_eLdpc_Qpsk_3_5) ||
                ((hChn->actualMode >= BAST_Mode_eLdpc_8psk_3_5) && (hChn->actualMode < BAST_Mode_eLdpc_8psk_5_6))))
               return true;
         }
         else
            return true; /* assume pilot loop while scanning */
      }
      else if ((hChn->acqParams.acq_ctl & (BAST_ACQSETTINGS_LDPC_PILOT | BAST_ACQSETTINGS_LDPC_PILOT_PLL)) == 
               (BAST_ACQSETTINGS_LDPC_PILOT | BAST_ACQSETTINGS_LDPC_PILOT_PLL))
         return true;
   } 
   return false;
#else
   BSTD_UNUSED(h);
   return false;
#endif
}


/******************************************************************************
 BAST_g2_P_LdpcSetPilotctl()
******************************************************************************/
void BAST_g2_P_LdpcSetPilotctl(BAST_ChannelHandle h)
{
   uint32_t data1, data2, val;

   if (BAST_g2_P_LdpcIs8psk(h))
   {
      if (BAST_g2_P_LdpcIsPilotOn(h))
      {   
         /* 8psk with pilot */
         data1 = 0x0e;
         if (BAST_g2_P_LdpcIsPilotLoopless(h))
         {
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
            val = 0x0D;
            data2 = 0x1b;
#else
            val = 0x0F;
            data2 = 0x13;
#endif
         }
         else
         {         
            val = 0x2D;
            data2 = 0x00;            
         }
      }
      else
      {     
         /* 8psk with no pilot */
         BAST_g2_P_AndRegister(h, BCHP_SDS_PLHDRCFG, 0xFFFFFFDF);

         val = 0x0C;
         data1 = 0x00;
         data2 = 0x00;
      }
   }
   else
   {
      if (BAST_g2_P_LdpcIsPilotOn(h))
      {
         /* qpsk with pilot */
         data1 = 0x16;
         if (BAST_g2_P_LdpcIsPilotLoopless(h))
         {
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
            val = 0x1D;
            data2 = 0x1B;
#else
            val = 0x1F;
            data2 = 0x13;
#endif           
         }
         else
         {                      
            val = 0x3D;
            data2 = 0x00;           
         }
      }
      else
      {     
         /* qpsk with no pilot */
         BAST_g2_P_AndRegister(h, BCHP_SDS_PLHDRCFG, 0xFFFFFFDF);

         val = 0x1C;
         data1 = 0x00;
         data2 = 0x00;
      }
   }
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PILOTCTL, &val);   /* write pilot_ctl */

   val = 0x059f2300 | (data1 & 0xFF);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PLCTL, &val);

   val = 0x0dc00000 | (data2 << 16);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_OVRDSEL, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcSetSistep()
******************************************************************************/
void BAST_g2_P_LdpcSetSistep(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;

   BAST_MultU32U32(hChn->acqParams.symbolRate, 1073741824, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo);
   Q_lo &= 0xFFFFFF00;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_SISTEP, &Q_lo);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_DELIF, &Q_lo);
}


/******************************************************************************
 BAST_g2_P_LdpcSetHpctrl()
******************************************************************************/
void BAST_g2_P_LdpcSetHpctrl(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, hpctrl5, hpctrl4;

   if (BAST_g2_P_LdpcIsPilotLoopless(h))
   {
      val = 0x4A;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL7, &val);

      /* pilot no pll */
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
      hpctrl4 = 0x51;  
#else
      if (BAST_g2_P_LdpcIs8psk(h))
         hpctrl4 = 0x5D;
      else
         hpctrl4 = 0x51;
#endif     
      
      hpctrl5 = 0x28;
   }
   else
   {
      /* no pilot or pilot pll */     
      if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
         val = 0xC8;
      else
         val = 0xC0;
       
      BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL7, &val);

      if (BAST_g2_P_LdpcIs8psk(h))
      {
         hpctrl4 = 0x1D;
         hpctrl5 = 0x29;
      }
      else
      {
         hpctrl5 = 0x28;
         hpctrl4 = 0x15;
      }
   }
   BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL4, &hpctrl4);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_HPCTRL5, &hpctrl5);
}


/******************************************************************************
 BAST_g2_P_LdpcSetBlen()
******************************************************************************/
void BAST_g2_P_LdpcSetBlen(BAST_ChannelHandle h)
{
   uint32_t val;

   if (BAST_g2_P_LdpcIs8psk(h))
   {
      if (BAST_g2_P_LdpcIsPilotOn(h))
         val = 22194 - 1;
      else
         val = 21690 - 1;
   }
   else
   {
      if (BAST_g2_P_LdpcIsPilotOn(h))
         val = 33282 - 1;
      else
         val = 32490 - 1;
   }
   val = val << 8;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_BLEN, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcSetHpmode()
******************************************************************************/
void BAST_g2_P_LdpcSetHpmode(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
   {
      /* enable MODCOD search */
      val = 0x79; 
   }
   else
      val = 0x09;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_HPMODE, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcSetHpParams()
******************************************************************************/
void BAST_g2_P_LdpcSetHpParams(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   BAST_Mode mode;

   val = 0x00;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IFMAX, &val);

   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
      mode = BAST_Mode_eLdpc_Qpsk_1_2; /* in scan mode */
   else
      mode = hChn->actualMode;

   if (mode >= BAST_Mode_eLdpc_Qpsk_2_3)   /* high rate QPSK and 8PSK */
      val = 0x08040000;
   else
     val = 0x06020000;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PSMAX, &val);

   val = 0x20100000;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_RLMAX, &val); 
   val = 0x05030410;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_TCMAX, &val);
   val = 0x0503000a;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_TLMAX, &val);   
      
   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
      val = 0x03;
   else
      val = 0x00;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_TFAVG, &val);  
   
   BAST_g2_P_ReadRegister(h, BCHP_SDS_CGCTRL, &val); 
   if ((val & 0x02) && ((BAST_MODE_IS_LDPC_8PSK(mode)) || (mode <= BAST_Mode_eLdpc_Qpsk_3_5)))
      val = 0x0E; /* undersample mode and (8psk or qpsk_1/2 or qpsk_3/5) */
   else
      val = 0x01;      
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FMAX, &val);    

   val = 0x04;
#if 0   
   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
   {
      if (BAST_g2_P_LdpcIsPilotOn(h) == false)
         val = 0x10;
   }
#endif  
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FMMAX, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcGenerateQpskPdTable()
******************************************************************************/
void BAST_g2_P_LdpcGenerateQpskPdTable(BAST_ChannelHandle h)
{
   uint32_t val, i_idx, q_idx, idx;

   static const uint8_t BAST_pd_qpsk_i[] =
   {
      0x00,
      0x26,
      0x4A,
      0x64,
      0x78,
      0x86,
      0x90,
      0x94,
      0x98,
      0x9A,
      0x9C,
      0x9C,
      0x9E
   };

   static const uint8_t BAST_pd_qpsk_q[] =
   {
      0x00,
      0x13,
      0x25,
      0x32,
      0x3C,
      0x43,
      0x48,
      0x4A,
      0x4C,
      0x4D,
      0x4E,
      0x4E,
      0x4F
   };

   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_LUPA, &val);

   for (i_idx = 0; i_idx < 32; i_idx++)
   {
      if (i_idx < 12)
         idx = i_idx;
      else
         idx = 12;
      val = (uint32_t)BAST_pd_qpsk_i[idx] << 16;
      
      for (q_idx = 0; q_idx < 32; q_idx++)
      {
         if (q_idx < 12)
            idx = q_idx;
         else
            idx = 12;
         val |= ((uint32_t)BAST_pd_qpsk_q[idx] << 8);
         BAST_g2_P_WriteRegister(h, BCHP_SDS_LUPD, &val);
      }
   }

   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_LUPA, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcGenerate8pskPdTable()
******************************************************************************/
void BAST_g2_P_LdpcGenerate8pskPdTable(BAST_ChannelHandle h)
{
   uint32_t val, i, j, n = 0;
   uint16_t s;
   uint8_t x;

   static const uint16_t BAST_pd_8psk[] =
   {
      0x0000,0x0077,0x00AD,0x00C2,0x00CB,0x00CF,0x00D1,0x00D2,
      0x00D3,0x00D3,0x00D3,0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,
      0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,
      0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,0x00D4,
      0x7700,0x6969,0x51A1,0x3FBB,0x34C7,0x2DCD,0x2AD0,0x28D2,
      0x27D3,0x27D3,0x26D3,0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,
      0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,
      0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,0x26D4,
      0xAD00,0xA151,0x8888,0x6EA8,0x5CBB,0x50C6,0x49CC,0x45D0,
      0x42D2,0x41D3,0x41D3,0x40D3,0x40D3,0x40D4,0x40D4,0x40D4,
      0x40D4,0x40D4,0x40D4,0x40D4,0x40D4,0x40D4,0x40D4,0x40D4,
      0x40D4,0x40D4,0x40D4,0x40D4,0x40D4,0x40D4,0x40D4,0x40D4,
      0xC200,0xBB3F,0xA86E,0x9090,0x7AA9,0x69BA,0x5DC5,0x56CC,
      0x52CF,0x50D1,0x4FD2,0x4ED3,0x4ED3,0x4ED3,0x4ED4,0x4ED4,
      0x4ED4,0x4ED4,0x4ED4,0x4ED4,0x4ED4,0x4ED4,0x4ED4,0x4ED4,
      0x4ED4,0x4ED4,0x4ED4,0x4ED4,0x4ED4,0x4ED4,0x4ED4,0x4ED4,
      0xCB00,0xC734,0xBB5C,0xA97A,0x9494,0x7FA9,0x6EBA,0x63C5,
      0x5CCC,0x58CF,0x56D1,0x55D2,0x54D3,0x54D3,0x54D3,0x54D4,
      0x54D4,0x54D4,0x54D4,0x54D4,0x54D4,0x54D4,0x54D4,0x54D4,
      0x54D4,0x54D4,0x54D4,0x54D4,0x54D4,0x54D4,0x54D4,0x54D4,
      0xCF00,0xCD2D,0xC650,0xBA69,0xA97F,0x9595,0x81A9,0x70BA,
      0x65C5,0x5ECB,0x5ACF,0x58D1,0x57D2,0x57D3,0x56D3,0x56D3,
      0x56D4,0x56D4,0x56D4,0x56D4,0x56D4,0x56D4,0x56D4,0x56D4,
      0x56D4,0x56D4,0x56D4,0x56D4,0x56D4,0x56D4,0x56D4,0x56D4,
      0xD100,0xD02A,0xCC49,0xC55D,0xBA6E,0xA981,0x9595,0x81A9,
      0x71B9,0x66C5,0x5FCB,0x5BCF,0x59D1,0x58D2,0x58D3,0x57D3,
      0x57D3,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,
      0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,
      0xD200,0xD228,0xD045,0xCC56,0xC563,0xBA70,0xA981,0x9696,
      0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,
      0x58D3,0x58D3,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,
      0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,0x57D4,
      0xD300,0xD327,0xD242,0xCF52,0xCC5C,0xC565,0xB971,0xA982,
      0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,
      0x58D3,0x58D3,0x58D3,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD300,0xD327,0xD341,0xD150,0xCF58,0xCB5E,0xC566,0xB972,
      0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,
      0x59D2,0x58D3,0x58D3,0x58D3,0x58D4,0x58D4,0x58D4,0x58D4,
      0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD300,0xD326,0xD341,0xD24F,0xD156,0xCF5A,0xCB5F,0xC567,
      0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,
      0x5AD1,0x59D2,0x58D3,0x58D3,0x58D3,0x58D4,0x58D4,0x58D4,
      0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD340,0xD34E,0xD255,0xD158,0xCF5B,0xCB60,
      0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,
      0x5CCF,0x5AD1,0x59D2,0x58D3,0x58D3,0x58D3,0x58D4,0x58D4,
      0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD340,0xD34E,0xD354,0xD257,0xD159,0xCF5C,
      0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,
      0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,0x58D3,0x58D3,0x58D4,
      0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD440,0xD34E,0xD354,0xD357,0xD258,0xD15A,
      0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,
      0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,0x58D3,0x58D3,
      0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD440,0xD44E,0xD354,0xD356,0xD358,0xD259,
      0xD15A,0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,
      0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,0x58D3,
      0x58D3,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD356,0xD357,0xD358,
      0xD259,0xD15A,0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,
      0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,
      0x58D3,0x58D3,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD357,0xD358,
      0xD358,0xD259,0xD15A,0xCF5C,0xCB60,0xC567,0xB972,0xA982,
      0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,
      0x58D3,0x58D3,0x58D3,0x58D4,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD358,
      0xD358,0xD358,0xD259,0xD15A,0xCF5C,0xCB60,0xC567,0xB972,
      0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,
      0x59D2,0x58D3,0x58D3,0x58D3,0x58D4,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD358,0xD358,0xD358,0xD259,0xD15A,0xCF5C,0xCB60,0xC567,
      0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,
      0x5AD1,0x59D2,0x58D3,0x58D3,0x58D3,0x58D4,0x58D4,0x58D4,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD358,0xD358,0xD358,0xD259,0xD15A,0xCF5C,0xCB60,
      0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,
      0x5CCF,0x5AD1,0x59D2,0x58D3,0x58D3,0x58D3,0x58D4,0x58D4,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD358,0xD358,0xD358,0xD259,0xD15A,0xCF5C,
      0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,
      0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,0x58D3,0x58D3,0x58D4,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD358,0xD358,0xD358,0xD259,0xD15A,
      0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,
      0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,0x58D3,0x58D3,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD358,0xD358,0xD358,0xD259,
      0xD15A,0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,
      0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,0x58D3,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD358,0xD358,0xD358,
      0xD259,0xD15A,0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,
      0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,0x58D3,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD358,0xD358,
      0xD358,0xD259,0xD15A,0xCF5C,0xCB60,0xC567,0xB972,0xA982,
      0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,0x59D2,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD358,
      0xD358,0xD358,0xD259,0xD15A,0xCF5C,0xCB60,0xC567,0xB972,
      0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,0x5AD1,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,
      0xD358,0xD358,0xD358,0xD259,0xD15A,0xCF5C,0xCB60,0xC567,
      0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,0x5CCF,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,
      0xD458,0xD358,0xD358,0xD358,0xD259,0xD15A,0xCF5C,0xCB60,
      0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,0x60CB,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,
      0xD458,0xD458,0xD358,0xD358,0xD358,0xD259,0xD15A,0xCF5C,
      0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,0x67C5,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,
      0xD458,0xD458,0xD458,0xD358,0xD358,0xD358,0xD259,0xD15A,
      0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,0x72B9,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,
      0xD458,0xD458,0xD458,0xD458,0xD358,0xD358,0xD358,0xD259,
      0xD15A,0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,0x82A9,
      0xD400,0xD426,0xD440,0xD44E,0xD454,0xD456,0xD457,0xD457,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,0xD458,
      0xD458,0xD458,0xD458,0xD458,0xD458,0xD358,0xD358,0xD358,
      0xD259,0xD15A,0xCF5C,0xCB60,0xC567,0xB972,0xA982,0x9696,
   };

   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_LUPA, &val);

   for (i = 0; i < 32; i++)
   {
      for (j = 0; j < 32; j++)
      {
         s = BAST_pd_8psk[n++];
         x = (s >> 8) & 0xFF;
         if (x & 0x80)
            x = (x << 1) | 0x01;
         else
            x = (x << 1);
         val = x << 16;
         if (x & 1)
         {
            val |= 0x01000000;
            val &= 0xFFFE0000;
         }

         x = (s & 0xFF);
         val |= (x << 8);
         BAST_g2_P_WriteRegister(h, BCHP_SDS_LUPD, &val);
      }
   }

   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_LUPA, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcEnableHpStateChangeIsr()
******************************************************************************/
void BAST_g2_P_LdpcEnableHpStateChangeIsr(BAST_ChannelHandle h, bool bEnable)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t irq_stat;

   BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQ, &irq_stat);
   irq_stat &= 0xFFFF0000; /* retain interrupt enable bits */
   irq_stat |= 0x00000400; /* clear HP state change interrupt */
   
   if (bEnable)
      irq_stat |= 0x04000000;
   else
      irq_stat &= ~0x04000000;
      
   /* clear and enable HP state change interrupt */
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQ, &irq_stat);

   if (bEnable)   
      BINT_EnableCallback_isr(hChn->hHpCb);
   else 
      BINT_DisableCallback_isr(hChn->hHpCb);
}


/******************************************************************************
 BAST_g2_P_LdpcFailedAcquisition()
******************************************************************************/
void BAST_g2_P_LdpcFailedAcquisition(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

#ifdef BAST_LDPC_DEBUG      
   BDBG_MSG(("failed to acquire"));
#endif   
   hChn->acqState = BAST_AcqState_eIdle;    
   BAST_g2_P_IndicateNotLocked(h);   
   BAST_g2_P_DisableChannelInterrupts_isr(h, false, false);   
}


/******************************************************************************
 BAST_g2_P_LdpcAcquire4() - called when HP state is 17 (acquire_ldpc_3)
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire4(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t delay;
      
   BAST_g2_P_StartBert(h);
   if ((BAST_MODE_IS_TURBO(hChn->acqParams.mode)) && (hChn->acqParams.symbolRate < 10000000))
      delay = 60000; /* TURBO */
   else
      delay = 2000; /* LDPC */
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, delay, BAST_g2_P_LdpcAcquire5); 
}


/******************************************************************************
 BAST_g2_P_LdpcAcquire5()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire5(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
#ifndef BAST_EXCLUDE_TURBO
   static const uint32_t script_turbo_4[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_OPLL, 0x000000FF),
      BAST_SCRIPT_WRITE(BCHP_SDS_OPLL2, 0x00000001),
      BAST_SCRIPT_WRITE(BCHP_SDS_OIFCTL3, 0x10),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0x88),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0x80),
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_turbo_5[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TFEC_TCIL, 0x00009FCC),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TRSD, 0x00004FCC),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0xC0),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TZPK, 0x03B58F34),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFMT, 0x00028008),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TSYN, 0x0103FEFE),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TPAK, 0x0009BB47),
      BAST_SCRIPT_EXIT
   };
#endif

   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      /* reset the LDPC */
      val = 0x00000001;
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_RST, &val);
      val = 0x00000000;
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_RST, &val);
      
      val = 0x00000002;
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_LDPC_CONFIG_0, &val);

      BAST_g2_P_LdpcConfig(h, BCHP_AFEC_LDPC_CONFIG_1, BAST_ldpc_config_1);
      BAST_g2_P_LdpcSetConfig2(h);

      val = 0;
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_RST, &val);
      val = 0x030F0E0F;
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_MPLCK, &val);

      BAST_g2_P_LdpcConfig(h, BCHP_AFEC_BCH_DECCFG0, BAST_ldpc_bch_deccfg0);
      BAST_g2_P_LdpcConfig(h, BCHP_AFEC_BCH_DECCFG1, BAST_ldpc_bch_deccfg1);

      BAST_g2_P_LdpcSetMpcfg1(h);

      val = 0x02;
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_RST, &val);
      val = 0x00;
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_RST, &val);

      val = 0xBC47705E; /* orig: 0xBC47105E; */
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_MPCFG0, &val);
      val = 0x05000502;
      BAST_g2_P_WriteRegister(h, BCHP_AFEC_LDPC_CONFIG_0, &val);
      
      BAST_g2_P_LdpcSetOpll(h);
      
#if ((BCHP_CHIP!=7325) && (BCHP_CHIP!=7335)) || (BCHP_VER >= BCHP_VER_B0)
      if (BAST_g2_P_LdpcIsPilotOn(h))
      {
         val = 0x92;
         BAST_g2_P_WriteRegister(h, BCHP_SDS_CLFBCTL, &val);
      }
#endif      
      

   }
#ifndef BAST_EXCLUDE_TURBO
   else
   {
      /* start Turbo here */
      BAST_g2_P_ProcessScript(h, script_turbo_4);
      
      BAST_g2_P_LdpcConfig(h, BCHP_TFEC_TITR, BAST_turbo_titr);
      
      BAST_g2_P_ProcessScript(h, script_turbo_5);
      
      BAST_g2_P_TurboSetTtur(h);
      
      BAST_g2_P_LdpcModeConfig(h, BCHP_TFEC_TZSY, BAST_turbo_tzsy);
      val = 0x40;
      BAST_g2_P_WriteRegister(h, BCHP_TFEC_TFECTL, &val);
#if (BCHP_CHIP==7340) || (BCHP_CHIP==7342)
      /* TFEC_TNBLK */
      /* TFEC_TCBLK */
      /* TFEC_0_TBBLK */
      /* TFEC_TCSYM */
#else
      val = 0x04D2162E;
      BAST_g2_P_WriteRegister(h, BCHP_TFEC_TERR, &val);
      val = 0x00592014;
      BAST_g2_P_WriteRegister(h, BCHP_TFEC_TCOR, &val);
#endif
      
      BAST_g2_P_TurboSetTssq(h);
      
      val = 0x60;
      BAST_g2_P_WriteRegister(h, BCHP_TFEC_TFECTL, &val);
      
      BAST_g2_P_TurboSetOpll(h);
   }
#endif

   if (BAST_g2_P_SetOpll(h) != BERR_SUCCESS)
      return BAST_g2_P_LdpcReacquire(h);

   hChn->initFreqOffset = BAST_g2_P_GetCarrierError(h);
 
   hChn->funct_state = 0;
   hChn->lockIsrFlag = 0xFF;   
   hChn->freqTransferInt = 0;   
   hChn->acqState = BAST_AcqState_eWaitForLock;   
   
   return BAST_g2_P_LdpcEnableLockIsr(h);
}

  
/******************************************************************************
 BAST_g2_P_LdpcAcquire6() - start lock timeout
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire6(BAST_ChannelHandle h)
{
   BERR_Code retCode;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t delay;
   
#ifdef BAST_LDPC_DEBUG         
   BDBG_MSG(("setting timer for lock"));
#endif   
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
      delay = 3000000; /* LDPC */
   else
      delay = 2000000; /* TURBO */
      
   hChn->noSyncCount = 0;         
   if ((hChn->acqParams.mode == BAST_Mode_eTurbo_scan) ||
       (BAST_MODE_IS_LDPC(hChn->actualMode) && BAST_g2_P_LdpcIsPilotOn(h)))
   {
      retCode = BAST_g2_P_LdpcSetupNotLockedMonitor(h);
      if (retCode != BERR_SUCCESS)
      {
         BDBG_WRN(("unable to set timer for no_lock monitoring"));
      }
   }   
   
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, delay, BAST_g2_P_LdpcAcquire7);
}


/******************************************************************************
 BAST_g2_P_LdpcAcquire7() - ldpc lock timer expired
******************************************************************************/
BERR_Code BAST_g2_P_LdpcAcquire7(BAST_ChannelHandle h)
{ 
#ifdef BAST_LDPC_DEBUG         
   BDBG_MSG(("lock timer expired"));
#endif
   BAST_g2_P_IndicateNotLocked(h);
   return BAST_g2_P_LdpcReacquire(h); 
}


/******************************************************************************
 BAST_g2_P_LdpcSetupNotLockedMonitor()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcSetupNotLockedMonitor(BAST_ChannelHandle h)
{ 
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t delay, P_hi, P_lo, Q_hi;
   
   delay = 1000;
   if (hChn->acqParams.symbolRate < 15000000)
   {
      BAST_MultU32U32(delay, 15000000, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &delay);      
   }
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBer, delay, BAST_g2_P_LdpcMonitorNotLocked);
}


/******************************************************************************
 BAST_g2_P_LdpcMonitorNotLocked()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcMonitorNotLocked(BAST_ChannelHandle h)
{
   static uint8_t BAST_noSyncCount_threshold[] =
   {
      50, /* qpsk 1/2 */
      35, /* qpsk 2/3 */
      34, /* qpsk 3/4 */
      31, /* qpsk 5/6 */
      30, /* qpsk 7/8 */
      19, /* 8psk 2/3 */
      18, /* 8psk 3/4 */
      19, /* 8psk 4/5 */
      18, /* 8psk 5/6 */
      18  /* 8psk 8/9 */
   };
   
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t irqsts3;
   int32_t frf, frf4, new_frf, div;
   bool bSetTimer = false;
   
   if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
   {
      /* maintain noSyncCount count */
      BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS3, &irqsts3);
      if (irqsts3 & 0x10)
      {
         hChn->noSyncCount++;
         if (hChn->noSyncCount > BAST_noSyncCount_threshold[hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2])
            retCode = BAST_g2_P_LdpcAcquire7(h);
         else
            bSetTimer = true;
      }
      else
         hChn->noSyncCount = 0;
   }
   else if (BAST_MODE_IS_LDPC(hChn->actualMode) && BAST_g2_P_LdpcIsPilotOn(h))
   {
      BAST_g2_P_ReadRegister(h, BCHP_SDS_FRF, (uint32_t*)&frf);
      BAST_g2_P_ReadRegister(h, BCHP_SDS_FRF4, (uint32_t*)&frf4);
      if (hChn->actualMode == BAST_Mode_eLdpc_Qpsk_1_2)
         div = 16;
      else
         div = 4;
      new_frf = frf + (frf4/div);
      BAST_g2_P_WriteRegister(h, BCHP_SDS_FRF, (uint32_t*)&new_frf);
      /* BDBG_MSG(("frf=0x%X, frf4=0x%X, div=%d, new_frf=0x%X", frf, frf4, div, new_frf)); */
      bSetTimer = true;
   }
   
   if (bSetTimer)
      retCode = BAST_g2_P_LdpcSetupNotLockedMonitor(h);
      
   return retCode;
}


/******************************************************************************
 BAST_g2_P_LdpcEnableLockIsr()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcEnableLockIsr(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      BINT_ClearCallback_isr(hChn->hLdpcLockCb); 
      BINT_ClearCallback_isr(hChn->hLdpcNotLockCb); 
      BINT_EnableCallback_isr(hChn->hLdpcLockCb); 
      BINT_EnableCallback_isr(hChn->hLdpcNotLockCb); 
   }
#ifndef BAST_EXCLUDE_TURBO
   else
   {
      BINT_ClearCallback_isr(hChn->hTurboLockCb); 
      BINT_ClearCallback_isr(hChn->hTurboNotLockCb); 
      BINT_EnableCallback_isr(hChn->hTurboLockCb); 
      BINT_EnableCallback_isr(hChn->hTurboNotLockCb);    
   }      
#endif

   return BAST_g2_P_LdpcAcquire6(h);
}


/******************************************************************************
 BAST_g2_P_LdpcLockStable()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcLockStable(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eBer); 
   
   if (hChn->acqTimeState == 1)
      BAST_g2_P_UpdateAcquisitionTime(h);   
           
   BAST_g2_P_IndicateLocked(h);
      
   hChn->acqState = BAST_AcqState_eMonitor;      
 
   if (hChn->firstTimeLock == 0)
   {
      hChn->firstTimeLock = 1;
      BAST_g2_P_ResetErrorCounters(h); 
   }
   
/* BDBG_MSG(("BAST_g2_P_LdpcLockStable(): lockFilterTime=%d, acqTime=%d", hChn->lockFilterTime, hChn->acqTime)); */
   hChn->lockFilterTime = BAST_LDPC_LOCK_FILTER_TIME; /* reset lock indication filter time */

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_LdpcLock_isr()
******************************************************************************/
void BAST_g2_P_LdpcLock_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   uint32_t val;

   BSTD_UNUSED(param);
   
   BAST_g2_P_IncrementInterruptCounter(h, BAST_Sds_IntID_eAfecLock);
   BDBG_MSG(("BAST_g2_P_LdpcLock_isr"));

   if (hChn->lockIsrFlag == 1)
   {
#ifdef BAST_LDPC_DEBUG            
      BDBG_MSG(("ignoring lock isr"));
#endif      
      return;
   }
   hChn->lockIsrFlag = 1;
   
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eBaud);  
   
   if (hChn->acqState == BAST_AcqState_eWaitForLock)
   {      
      /* ldpc locked for the first time */   
      hChn->acqState = BAST_AcqState_eMonitor;
      
      BAST_g2_P_LdpcSetPlc(h, false);   
            
      BAST_g2_P_LdpcSetFinalBaudLoopBw(h);     
      BAST_g2_P_LdpcSetPsl(h);
      
      if (BAST_g2_P_LdpcIs8psk(h))
         val = 0x44;  /* 8psk */
      else
         val = 0x66;  /* qpsk */
      BAST_g2_P_WriteRegister(h, BCHP_SDS_EQMU, &val);    
      
      BAST_g2_P_ResyncBert(h);
   }

   BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eGen1, hChn->lockFilterTime, BAST_g2_P_LdpcLockStable);      
   BAST_g2_P_LdpcMonitor(h);
}


/******************************************************************************
 BAST_g2_P_LdpcNotLock_isr()
******************************************************************************/
void BAST_g2_P_LdpcNotLock_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   uint32_t val;
   BSTD_UNUSED(param);
   
   BAST_g2_P_IncrementInterruptCounter(h, BAST_Sds_IntID_eAfecNotLock);   
   BAST_g2_P_LdpcUpdateBlockCounters(h);
   
   if (hChn->lockIsrFlag == 0)
   {
#ifdef BAST_LDPC_DEBUG            
      BDBG_MSG(("ignoring not lock isr"));
#endif      
      return;
   }
   hChn->lockIsrFlag = 0;
   
   BDBG_MSG(("BAST_g2_P_LdpcNotLock_isr"));
   
   if (hChn->firstTimeLock)
   {
      hChn->lockFilterTime = hChn->lockFilterTime + BAST_LDPC_LOCK_FILTER_INCREMENT;
      if (hChn->lockFilterTime > BAST_LDPC_LOCK_FILTER_TIME_MAX)
         hChn->lockFilterTime = BAST_LDPC_LOCK_FILTER_TIME_MAX;
#ifdef BAST_LDPC_DEBUG
   BDBG_MSG(("lockFilterTime = %d", hChn->lockFilterTime));
#endif         
   }
      
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eGen1);
     
   BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &val);
   if (((val >> 24) & 0x1F) != 17)
      goto reacquire;

   BAST_g2_P_ReadRegister(h, BCHP_AFEC_LDPC_STATUS, &val);
   if ((val & 0x00010000) || (hChn->acqState == BAST_AcqState_eWaitForLock))
   {
#ifdef BAST_LDPC_DEBUG    
      BDBG_MSG(("ldpc_locked=%d, waiting for mpeg lock", (val & 0x00010000) ? 1 : 0));   
#endif      
      BAST_g2_P_IndicateNotLocked((BAST_ChannelHandle)h);   
      BAST_g2_P_LdpcAcquire6(h);
   }  
   else if ((hChn->ldpcCtl & BAST_G2_LDPC_CTL_DISABLE_FEC_REACQ) == 0)
   {
      reacquire:
#ifdef BAST_LDPC_DEBUG               
      BDBG_MSG(("reacquiring because LDPC not locked"));
#endif      
      BAST_g2_P_IndicateNotLocked((BAST_ChannelHandle)h);
      BAST_g2_P_DisableChannelInterrupts_isr(h, false, false);
      BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, ((BAST_g2_P_Handle *)(h->pDevice->pImpl))->reacqDelay, BAST_g2_P_LdpcReacquire);
   }
}


/******************************************************************************
 BAST_g2_P_ProcessHpState0()
******************************************************************************/
void BAST_g2_P_ProcessHpState0(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   uint32_t val;
   
   if ((hChn->prev_state == 4) || (hChn->prev_state == 5)) /* reset HP if coming from TIM_START */
      BAST_g2_P_OrRegister(h, BCHP_SDS_HPCTRL1, 0x01);

   /* always acquire with tuner not in LO tracking mode*/
   BAST_g2_P_TunerSetDigctl7(h, 0x04);

   val = 0x03;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_MIXCTL, &val);

   val = 0x0F;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_CLCTL, &val);

   hChn->count2 = 1;   
}


/******************************************************************************
 BAST_g2_P_ProcessHpState15()
******************************************************************************/
void BAST_g2_P_ProcessHpState15(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   uint32_t val, data0; 

   static const uint32_t script_ldpc_8[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_ABW, 0x03030000),   /* final AGC BW */
#if 0      
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL5, 0x01), /* 0x09 causes AGC loop to oscillate slighly */
#endif      
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL15, 0xFB),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL16, 0xEE),
      BAST_SCRIPT_EXIT
   };

   if (!(hChn->count2 & 1))
      BAST_g2_P_ProcessHpState0(h);
   hChn->count2 |= 2;

   BAST_g2_P_ReadRegister(h, BCHP_SDS_EQBLND, &val);
   data0 = (val & 0x000000FF) & ~0x04; /* disable CMA */
   val &= 0xFFFFFF00;
   val |= data0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_EQBLND, &val);

   val = 0x06;   /* unfreeze main tap */
   BAST_g2_P_WriteRegister(h, BCHP_SDS_EQFFE2, &val);
   val = 0x02;   /* unfreeze other taps */
   BAST_g2_P_WriteRegister(h, BCHP_SDS_EQFFE2, &val);

   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))    /* LDPC mode */
   {
      /* setting up VLC */
      BAST_g2_P_LdpcConfig(h, BCHP_SDS_VLCI, BAST_ldpc_vlci);
      BAST_g2_P_LdpcConfig(h, BCHP_SDS_VLCQ, BAST_ldpc_vlci);
   }
   else
   {
      /* setting up VLC */
      val = 0x20000000;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_VLCI, &val);
      BAST_g2_P_WriteRegister(h, BCHP_SDS_VLCQ, &val);
   }

   /* setting up BERT */
   BAST_g2_P_ProcessScript(h, script_ldpc_8);
   
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))    /* LDPC mode */
   {   
      BAST_g2_P_LdpcConfig(h, BCHP_SDS_SNRHT, BAST_ldpc_snrht);
      BAST_g2_P_LdpcConfig(h, BCHP_SDS_SNRLT, BAST_ldpc_snrlt);
   }

   val = 0x01546732;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_BEM1, &val);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_BEM2, &val);

   BAST_g2_P_InitBert(h);

   val = 0x0FFF05FF;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_BEIT, &val);
   val = 0x88;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_SNRCTL, &val);
   val = 0x0B;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_SNRCTL, &val);
}


/******************************************************************************
 BAST_g2_P_ProcessHpState16()
******************************************************************************/
void BAST_g2_P_ProcessHpState16(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;

   if (!(hChn->count2 & 2))
      BAST_g2_P_ProcessHpState15(h);
   hChn->count2 |= 4;  
   
   if (hChn->bFrfTransferred)
      BAST_g2_P_WriteRegister(h, BCHP_SDS_FRF, (uint32_t*)&(hChn->frfStep));
}


/******************************************************************************
 BAST_g2_P_LdpcTransferCarrierToTuner()
******************************************************************************/
void BAST_g2_P_LdpcTransferCarrierToTuner(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   BERR_Code retCode;
   int32_t carr_error;
   uint32_t val;
#ifdef BAST_ENABLE_LDPC_DCO_SEPARATION   
   int32_t frfStep; 
   uint32_t Q_hi, Q_lo, P_hi, P_lo;
   bool bSpinv = false;   
#endif   

   carr_error = BAST_g2_P_GetCarrierError(h);
  
   BAST_g2_P_FreezeLoops(h);

   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FRF, &val);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FLI, &val);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_PLI, &val);

   /* retune to get optimal popcap settings */     
   hChn->tunerIfStep = carr_error;
   
#ifdef BAST_ENABLE_LDPC_DCO_SEPARATION  
   hChn->tunerOffset = BAST_DCO_OFFSET; /* put in 10KHz offset in MB2 */
   
   /* put 10KHz offset in FRF to compensate MB2 offset */
   BAST_g2_P_ReadRegister(h, BCHP_SDS_HPCTRL1, &val);
   if (val & 0x02)
   {
      /* HP is enabled */
      BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &val);
      if (val & 0x00020000)
         bSpinv = true;
   }
      
   BAST_MultU32U32(BAST_DCO_OFFSET, 16777216, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo);
   if (bSpinv)
      frfStep = (int32_t)-Q_lo;
   else
      frfStep = (int32_t)Q_lo;
   hChn->frfStep = frfStep * 256;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_FRF, (uint32_t*)&(hChn->frfStep));
#else
   hChn->tunerOffset = 0;   
#endif   
   
#ifdef BAST_LDPC_DEBUG               
   BDBG_MSG(("setting tunerIfStep to %d Hz", hChn->tunerIfStep));  
#endif       
   retCode = BAST_g2_P_TunerQuickTune(h, BAST_g2_P_LdpcTransferCarrierToTuner1);
   if (retCode != BERR_SUCCESS)
   {
      BDBG_ERR(("BAST_g2_P_TunerQuickTune() error 0x%X", retCode));
	}
}


/******************************************************************************
 BAST_g2_P_LdpcTransferCarrierToTuner1()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcTransferCarrierToTuner1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   
   hChn->tuneMixStatus |= BAST_TUNE_MIX_RESET_IF_PREVIOUS_SETTING_FAILS;

   BAST_g2_P_UnfreezeLoops(h);   
   hChn->bFrfTransferred = true;
   hChn->funct_state = 0;
   return BAST_g2_P_LdpcAcquire3(h);
}


/******************************************************************************
 BAST_g2_P_ProcessHpState17()
******************************************************************************/
void BAST_g2_P_ProcessHpState17(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   uint32_t val;
   BAST_g2_FUNCT funct_ptr;   

   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eBaud);
   BAST_g2_P_LdpcEnableHpStateChangeIsr(h, false);        
   
   if (!(hChn->count2 & 4))
      BAST_g2_P_ProcessHpState16(h);
   hChn->count2 |= 8;
   
   if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
   {
      hChn->turboScanState |= (BAST_TURBO_SCAN_HP_INIT | BAST_TURBO_SCAN_HP_LOCKED);
      if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
         hChn->turboScanState |= BAST_TURBO_SCAN_8PSK_HP_LOCKED;
   }
   else if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
   {
      BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 33000, BAST_g2_P_LdpcGetModcod); 
      return;
   }
   
   if (BAST_g2_P_CarrierOffsetOutOfRange(h))
   {
#ifdef BAST_LDPC_DEBUG            
      BDBG_MSG(("carrier freq out of range"));
#endif      
      BAST_g2_P_LdpcReacquire(h);
      return;
   }
    
   if ((hChn->bFrfTransferred == false) && (hChn->bExternalTuner == false))
   {
      BAST_g2_P_LdpcTransferCarrierToTuner(h);
      return;
   }
              
   /* wait for loops to be stable */
   hChn->funct_state = 0;  
   funct_ptr = BAST_g2_P_LdpcAcquire4;          

   val = hChn->trackingAnactl4;
   BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val); 


   BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 2000, funct_ptr); 
}
 
   
/******************************************************************************
 BAST_g2_P_LdpcHp17Timeout()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcHp17Timeout(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   
#ifdef BAST_LDPC_DEBUG
   BDBG_MSG(("HP state 17 timeout"));
#endif
   BAST_g2_P_DisableChannelInterrupts_isr(h, false, false);  
   
   if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
   {
      if ((hChn->turboScanState & BAST_TURBO_SCAN_HP_INIT) == 0)
      {
         hChn->turboScanState = BAST_TURBO_SCAN_HP_INIT;
         if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
            hChn->turboScanState |= BAST_TURBO_SCAN_8PSK_FAILED;
      }
   }
   return BAST_g2_P_LdpcReacquire(h);
}


/******************************************************************************
 BAST_g2_P_LdpcHp_isr()
******************************************************************************/
void BAST_g2_P_LdpcHp_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   uint32_t state, stat, irqstat;
   uint8_t val8;

   BSTD_UNUSED(param);
   BAST_g2_P_IncrementInterruptCounter(h, BAST_Sds_IntID_eLdpcHp);
   
#ifdef BAST_LDPC_DEBUG            
   BDBG_MSG(("BAST_g2_P_LdpcHp_isr()"));
#endif   
   
   if (hChn->bExternalTuner == false)
      BAST_g2_P_3StageAgc_isr(h, 0); 
   
   BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &stat);   
   state = (stat >> 24) & 0x1F;
   
   process_hp_state:   
#ifdef BAST_LDPC_DEBUG            
   BDBG_MSG(("HP state %d", state));
#endif   

   BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQ, &irqstat);
   if ((irqstat & 0x02000200) == 0x02000200)
   {
      /* HP reacquire interrupt */
      irqstat &= 0xFDFF0000;
      irqstat |= 0x00000200;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQ, &irqstat);
      
      val8 = (hChn->ldpcScanState + 1) & 0x03;
      hChn->ldpcScanState &= BAST_LDPC_SCAN_MASK;
      hChn->ldpcScanState |= val8;
#ifdef BAST_LDPC_DEBUG               
      BDBG_MSG(("HP reacquire: scan mode %d", val8));
#endif      
      
      if (val8 == 0)
      {
         BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, ((BAST_g2_P_Handle *)(h->pDevice->pImpl))->reacqDelay, BAST_g2_P_LdpcReacquire);
      }
      else
         BAST_g2_P_LdpcReacquire(h);
      return;
   }
   
   switch (state)
   {
      case 0: /* peak search */
         BAST_g2_P_ProcessHpState0(h);
         break;      
    
      case 15:
         BAST_g2_P_ProcessHpState15(h);
         break;

      case 16:
         BAST_g2_P_ProcessHpState16(h);
         break;

      case 17:
         BAST_g2_P_ProcessHpState17(h);
         break;

      default:
         break;
   }

   hChn->prev_state = state;
   if (state == 17)
      return;

   BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &stat);
   state = (stat >> 24) & 0x1F;
   if (state != hChn->prev_state)
   {
      /* BDBG_MSG(("HP state changed")); */
      goto process_hp_state;
   }
   
   BAST_g2_P_LdpcEnableHpStateChangeIsr(h, true);
}


/******************************************************************************
 BAST_g2_P_LdpcSetConfig2()
******************************************************************************/
void BAST_g2_P_LdpcSetConfig2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t config2, idx, block_length, val, extra_cycles, P_hi, P_lo, Q_hi, Q_lo;

   static uint32_t BAST_block_length[] =   /* must by multiplied by 6 */
   {
      105, /* 1/2 */
      132, /* 3/5 */
      100, /* 2/3 */
      105, /* 3/4 */
      108, /* 4/5 */
      110, /* 5/6 */
      90, /* 8/9 */
      90  /* 9/10 */
   };

   static uint32_t BAST_extra_cycles[] = 
   {
      27, /* 1/2 */
      35, /* 3/5 */
      35, /* 2/3 */
      34, /* 3/4 */
      41, /* 4/5 */
      47, /* 5/6 */
      43, /* 8/9 */
      46  /* 9/10 */
   };

   static uint32_t BAST_ldpc_config_2[] =
   {
      0x00fa00cd, /* QPSK LDPC+BCH 1/2 */
      0x00c7008c, /* QPSK LDPC+BCH 3/5 */
      0x01060083, /* QPSK LDPC+BCH 2/3 */
      0x00fa00a3, /* QPSK LDPC+BCH 3/4 */
      0x00f100bc, /* QPSK LDPC+BCH 4/5 */
      0x00ed00d4, /* QPSK LDPC+BCH 5/6 */
      0x0120010d, /* QPSK LDPC+BCH 8/9 */
      0x0120011b, /* QPSK LDPC+BCH 9/10 */
      0x008400e7, /* 8PSK LDPC+BCH 3/5 */
      0x00ae0122, /* 8PSK LDPC+BCH 2/3 */
      0x00a5018b, /* 8PSK LDPC+BCH 3/4 */
      0x009c0210, /* 8PSK LDPC+BCH 5/6 */
      0x00be0253, /* 8PSK LDPC+BCH 8/9 */
      0x00be0277  /* 8PSK LDPC+BCH 9/10 */
   };

   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
      return;

   BAST_g2_P_GetFecFreq(h);
   if (BAST_g2_P_LdpcIs8psk(h))
   {
      val = 10800; /* 8PSK frame_size/2 */
      if (hChn->actualMode <= BAST_Mode_eLdpc_8psk_3_4)
         idx = 1;
      else
         idx = 2;
      idx += (hChn->actualMode - BAST_Mode_eLdpc_8psk_3_5); 
   }
   else
   {
      val = 16200; /* QPSK frame size/2 */
      idx = hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2; 
   }

   block_length = BAST_block_length[idx] * 12;
   extra_cycles = BAST_extra_cycles[idx];
   BAST_MultU32U32(hChn->fecFreq, val << 1, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &Q_lo);
   val = Q_lo - (block_length << 1);
   val /= (block_length + extra_cycles);
   if (val > 150)
      val = 150;

   idx = hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2;
   config2 = (val << 16) | (BAST_ldpc_config_2[idx] & 0x0000FFFF);
   BAST_g2_P_WriteRegister(h, BCHP_AFEC_LDPC_CONFIG_2, &config2);
}


/******************************************************************************
 BAST_g2_P_LdpcSetMpcfg1()
******************************************************************************/
void BAST_g2_P_LdpcSetMpcfg1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   if (hChn->xportCtl & BAST_G2_XPORT_CTL_BCH_CHECK)
      val = 0x20;
   else
      val = 0;
   if (hChn->xportCtl & BAST_G2_XPORT_CTL_CRC8_CHECK)
      val |= 0x10;
   if (hChn->xportCtl & BAST_G2_XPORT_CTL_TEI)
      val |= 0x08;
   BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_MPCFG1, &val);
}


/******************************************************************************
 BAST_g2_P_LdpcSetOpll()
******************************************************************************/
void BAST_g2_P_LdpcSetOpll(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val1, val2, idx, data0, number_of_bits, number_of_symbols, opll, opll2, cgctrl;
   
   static const uint32_t BAST_ldpc_number_of_bits[] =
   {
      32128, /* 1/2 */
      38608, /* 3/5 */
      42960, /* 2/3 */
      48328, /* 3/4 */
      51568, /* 4/5 */
      53760, /* 5/6 */
      57392, /* 8/9 */
      58112  /* 9/10 */
   };

   if (BAST_g2_P_LdpcIs8psk(h))
   {
      val1 = 21600;
      if (hChn->actualMode <= BAST_Mode_eLdpc_8psk_3_4)
         data0 = 1;
      else
         data0 = 2;
      val2 = 504; /* 14*36 */
      idx = hChn->actualMode - BAST_Mode_eLdpc_8psk_3_5 + data0;
   }
   else
   {
      val1 = 32400;
      val2 = 792; /* 22*36 */
      idx = hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2;
   }
   
   BAST_g2_P_ReadRegister(h, BCHP_SDS_CGCTRL, &cgctrl);   
   if (cgctrl & 0x02)
      number_of_bits = 2 * BAST_ldpc_number_of_bits[idx];
   else
      number_of_bits = BAST_ldpc_number_of_bits[idx];     
        
   if (BAST_g2_P_LdpcIsPilotOn(h))
      val1 += val2;
   number_of_symbols = val1 + 90;

   opll = (number_of_bits >> 3);
   val1 = number_of_symbols >> 1;
   
   if ((val1 < opll) && (cgctrl & 0x02))
   {
      number_of_bits = BAST_ldpc_number_of_bits[idx];
      opll = (number_of_bits >> 3);
      val1 = number_of_symbols;
#if (BCHP_CHIP==7325) || (BCHP_CHIP==7335)         
      BAST_g2_P_OrRegister(h, BCHP_SDS_OIFCTL3, 0x20);   
#else
      BAST_g2_P_OrRegister(h, BCHP_SDS_OIFCTL3, 0x100);  
#endif      
   }
   else
      BAST_g2_P_AndRegister(h, BCHP_SDS_OIFCTL3, ~0x320);     
     
   opll2 = val1 - opll;
   
   BAST_g2_P_WriteRegister(h, BCHP_SDS_OPLL, &opll);   
   BAST_g2_P_WriteRegister(h, BCHP_SDS_OPLL2, &opll2);
}


/******************************************************************************
 BAST_g2_P_LdpcConfig()
******************************************************************************/
void BAST_g2_P_LdpcConfig(BAST_ChannelHandle h, uint32_t reg, const uint32_t *pVal)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BAST_Mode mode;
   uint32_t val, i;

   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      i = hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2;
      goto lookup_register;
   }
   else if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
   {
      /* we're in peak_search */
      if ((reg < BCHP_AFEC_LDPC_CONFIG_0) || (reg > BCHP_AFEC_TM_4))
      {
         /* register is not in AFEC core */
         if (reg == BCHP_SDS_PLHDRCFG)
         {
            if (BAST_g2_P_LdpcIsPilotOn(h))
               val = 0x20;
            else
               val = 0x00;
            if (BAST_g2_P_LdpcIs8psk(h))
               val |= 13; /* pick any 8psk mode */
            else
               val |= 7;  /* pick any qpsk mode */
            BAST_g2_P_WriteRegister(h, reg, &val);
         }
         else 
         {
            if (BAST_g2_P_LdpcIs8psk(h))
               mode = BAST_Mode_eLdpc_8psk_2_3; /* pick any 8psk mode */
            else
               mode = BAST_Mode_eLdpc_Qpsk_3_4; /* pick any qpsk mode */
            goto set_ldpc_idx;
         }
      }
   }
   else
   {
      mode = hChn->actualMode;

      set_ldpc_idx:
      i = mode - BAST_Mode_eLdpc_Qpsk_1_2;
/* BDBG_MSG(("BAST_g2_P_LdpcConfig(): mode=%d, i=%d", mode, i)); */

      lookup_register:
      val = pVal[i];
      BAST_g2_P_WriteRegister(h, reg, &val);
   }
}


/******************************************************************************
 BAST_g2_P_LdpcReacquire()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcReacquire(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val;

   BDBG_MSG(("BAST_g2_P_LdpcReacquire"));
   
   BAST_g2_P_IndicateNotLocked(h);   
   BAST_g2_P_DisableChannelInterrupts_isr(h, false, false);
   BAST_g2_P_LdpcUpdateBlockCounters(h);
#ifndef BAST_EXCLUDE_STATUS_EVENTS
   BAST_g2_P_ResetStatusIndicators(h);
#endif
        
   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
      goto reacquire;
   else
   {
      if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_REACQ_DISABLE) == 0)
      {
         if (hChn->maxReacq && (hChn->acqCount > hChn->maxReacq))
            goto give_up;

         reacquire:
         /* widen filter if using internal tuner */
         if ((hChn->bExternalTuner == false) && (hChn->tunerCutoffWide))
         {
            val = hChn->trackingAnactl4 + (hChn->tunerCutoffWide << 2);
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val);
         }
                
         if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
            return BAST_g2_P_TurboAcquire(h);
            
         if (((hChn->ldpcScanState & 0x03) == 0) && (hChn->blindScanStatus == BAST_BlindScanStatus_eAcquire))
            return BAST_g2_P_BlindReacquire(h);      
        
         if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT_SCAN)
            hChn->acqParams.acq_ctl ^= BAST_ACQSETTINGS_LDPC_PILOT;
            
         hChn->funct_state = 0;
         if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
         {
            if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MODE_MASK) == 0)
               goto LdpcReacquire_1;

            goto acquire_without_retune;
         }
         else
         {
            if (hChn->blindScanStatus != BAST_BlindScanStatus_eAcquire)
               hChn->acqCount++;          

            LdpcReacquire_1:  
            if (hChn->acqCount & 1)
            {
               acquire_without_retune:
               retCode = BAST_g2_P_LdpcAcquire1(h);
            }
            else
            {
               hChn->tunerIfStep = 0;
               hChn->acqState = BAST_AcqState_eTuning;  
               hChn->acqParams.carrierFreqOffset = 0;           
               retCode = BAST_g2_P_TunerSetFreq(h, BAST_g2_P_Acquire);
            }
         }
      }
      else
      {
         give_up:
         BAST_g2_P_LdpcFailedAcquisition(h);
      }
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_P_LdpcSetFinalBaudLoopBw()
******************************************************************************/
void BAST_g2_P_LdpcSetFinalBaudLoopBw(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t lval1, lval2;

   if (hChn->acqParams.symbolRate >= 12000000)
   {
      lval1 = 0x00267000;  
      lval2 = 0x000033C0; 
   }
   else if (hChn->acqParams.symbolRate >= 4000000)
   {
      lval1 = 0x00140000;
      lval2 = 0x00000190;
   }
   else if (hChn->acqParams.symbolRate >= 2000000)
   {
      lval1 = 0x00418000;
      lval2 = 0x00000431;
   }
   else
   {
      lval1 = 0x00B10000;
      lval2 = 0x00000D99;
   }

   BAST_g2_P_WriteRegister(h, BCHP_SDS_BRLC, &lval1);
   BAST_g2_P_WriteRegister(h, BCHP_SDS_BRIC, &lval2);
}


/******************************************************************************
 BAST_g2_P_LdpcSetPsl()
******************************************************************************/
void BAST_g2_P_LdpcSetPsl(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t psl_ctl, val, iter, x, gain, beta;

   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_MASK) == BAST_LDPC_SCAN_ON)
   {
      psl_ctl = 0x00000050;
   }
   else
   {
      /* limit max iterations to 150 */
      BAST_g2_P_ReadRegister(h, BCHP_AFEC_LDPC_CONFIG_2, &val);
      iter = (val >> 16) & 0x07FF;
      if (iter > 150)  
      {
         iter = 150;
         val = (val & 0x0000FFFF) | (iter << 16);
         BAST_g2_P_WriteRegister(h, BCHP_AFEC_LDPC_CONFIG_2, &val);
      }

      /* set PSL threshold to 68% max iterations for 8psk 3/5
         set PSL threshold to 56% max iterations for all other code rates */
      if (hChn->actualMode == BAST_Mode_eLdpc_8psk_3_5)
         x = 68;
      else if (hChn->actualMode == BAST_Mode_eLdpc_8psk_3_4)
         x = 75;
      else
         x = 56;
      iter = (((iter * x * 2) / 100) + 1) >> 1;
      iter = iter << 2;
      
      /* set gain and beta */
      if (hChn->acqParams.symbolRate < 5000000)
      {
         gain = 0x0B; /* gain */
         beta = 0x0A; /* beta */
      }
      else
      {
         gain = 0x09; /* gain */
         beta = 0x07; /* beta */
      }

      psl_ctl = ((beta << 4) & 0xF0) | 0x05;
      psl_ctl |= ((gain & 0x0F) | (((iter & 0x0003) << 6) & 0xC0)) << 8;
      psl_ctl |= ((iter >> 2) & 0xFF) << 16;
      psl_ctl |= ((iter >> 10) & 0x07) << 24;
   }
   BAST_g2_P_WriteRegister(h, BCHP_AFEC_LDPC_PSL_CTL, &psl_ctl);
}


/******************************************************************************
 BAST_g2_P_LdpcRmd()
******************************************************************************/
static uint8_t BAST_g2_P_LdpcRmd(uint32_t enc_msg)
{
   uint32_t mask;
   int i, j, y[32], sum[16], diff[16], Y, I;
   uint8_t dec_msg;

   for (i = 0, mask = 0x80000000; mask; mask = mask >> 1)
      y[i++] = ((enc_msg & mask) ? -1 : 1);

   for (i = 0; i < 5; i++)
   {
      for (j = 0; j < 32; j += 2)
      {
         sum[j >> 1] = y[j] + y[j+1];
         diff[j >> 1] = y[j] - y[j+1];
      }
      for (j = 0; j < 16; j++)
      {
         y[j] = sum[j];
         y[j+16] = diff[j];
      }
   }

   for (i = Y = I = 0; i < 32; i++)
   {
      if (abs(y[i]) > abs(Y))
      {
         I = i;
         Y = y[i];
      }
   }

   I |= (Y <= 0 ? 0x20 : 0);
   dec_msg = 0;
   if (I & 1) dec_msg |= 32;
   if (I & 2) dec_msg |= 16;
   if (I & 4) dec_msg |= 8;
   if (I & 8) dec_msg |= 4;
   if (I & 16) dec_msg |= 2;
   if (I & 32) dec_msg |= 1;
   
   return dec_msg;
}


/******************************************************************************
 BAST_g2_P_LdpcGetModcod()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcGetModcod(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   hChn->funct_state = 0;
   hChn->count1 = 0;
   return BAST_g2_P_LdpcGetModcod1(h);
}


/******************************************************************************
 BAST_g2_P_LdpcGetModcod1()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcGetModcod1(BAST_ChannelHandle h)
{
   #define descramble_hi (0x719D83C9)
   #define descramble_lo (0x53422DFA)

   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, rmd_in, L4, i, j, lval1, mask, d_hi, d_lo;
   uint8_t mode;
   int8_t I, Q;

   while (hChn->funct_state != 0xFF)
   {
      switch (hChn->funct_state)
      {
         case 0: /* wait for symbols to be ready to read */
            BAST_g2_P_ReadRegister(h, BCHP_SDS_HDRD, &val);
            if ((val & 0x0B00) != 0x0300)
            {
               if (hChn->count1++ < 35)
                  return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 1000, BAST_g2_P_LdpcGetModcod1); 
               
               /* modcod search failed */
#ifdef BAST_LDPC_DEBUG                        
               BDBG_MSG(("modcod search failed"));
#endif               
               return BAST_g2_P_LdpcReacquire(h);
            }
            hChn->funct_state = 1;
            break;
             
          case 1:
            BAST_g2_P_ReadRegister(h, BCHP_SDS_PLHDRSCR2, &val); 
            d_hi = descramble_hi ^ val;
            BAST_g2_P_ReadRegister(h, BCHP_SDS_PLHDRSCR1, &val); 
            d_lo = descramble_lo ^ val;                
            
            /* skip 26-bit SOF */
            val = 0x1A000000;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_HDRA, &val);
            
            /* read/slice/descramble into 64-bits, then put odd bits into rmd_in */
            L4 = 0x80000000;
            rmd_in = 0;
            for (i = 2; i; i--)
            {
               lval1 = 0;
               mask = 0x80000000;
               for (j = 32; j; j--, mask = mask >> 1)
               {
                  BAST_g2_P_ReadRegister(h, BCHP_SDS_HDRD, &val); /* [31:24] = I, [23:16] = Q */
/* BDBG_MSG(("HDRD=0x%08X", val)); */
                  I = (int8_t)(val >> 24);
                  Q = (int8_t)((val >> 16) & 0xFF);
                  if (j & 0x01)
                  {
                     /* even */
                     if (I > Q) 
                        lval1 |= mask;
                  }
                  else
                  {
                     /* odd */
                     if (I < -Q) 
                        lval1 |= mask;
                  }
               }

               if (i == 2)
                  lval1 ^= d_hi;
               else
                  lval1 ^= d_lo;

               for (mask = 0x80000000; mask; mask = mask >> 2)
               {
                  rmd_in |= ((mask & lval1) ? L4 : 0);
                  L4 = L4 >> 1;
               }
            }
            
            mode = BAST_g2_P_LdpcRmd(rmd_in);
            mode = mode >> 1;

            if ((mode >= 4) && (mode <= 17))
            {
               hChn->ldpcScanState |= BAST_LDPC_SCAN_FOUND;
               hChn->actualMode = BAST_Mode_eLdpc_Qpsk_1_2 + mode - 4;
               hChn->funct_state = 0xFF;
#ifdef BAST_LDPC_DEBUG                        
               BDBG_MSG(("modcod decoded: modcod=%d, BAST_Mode=%d", mode, hChn->actualMode)); 
#endif                            
            }
            else
            {
#ifdef BAST_LDPC_DEBUG                     
               BDBG_MSG(("bad modcod decoded (%d)", mode)); 
#endif               
               return BAST_g2_P_LdpcReacquire(h);
            }
            break;
      }
   }
   
   /* reacquire with the known code rate */
   hChn->funct_state = 5;
   return BAST_g2_P_LdpcAcquire1(h);
}


/******************************************************************************
 BAST_g2_P_LdpcMonitor()
******************************************************************************/
BERR_Code BAST_g2_P_LdpcMonitor(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if (hChn->bExternalTuner == false)
      BAST_g2_P_3StageAgc_isr(h, 0);         
   
   BAST_g2_P_TransferFreqOffset(h);
   if (hChn->bForceReacq)
      return BAST_g2_P_LdpcReacquire(h);
      
#ifndef BAST_EXCLUDE_STATUS_EVENTS
   BAST_g2_P_CheckStatusIndicators(h);
#endif

   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100000, BAST_g2_P_LdpcMonitor);
}


/******************************************************************************
 BAST_g2_P_LdpcUpdateBlockCounters()
******************************************************************************/
void BAST_g2_P_LdpcUpdateBlockCounters(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t corr, bad, total, val = 0;
   
   if (BAST_g2_P_IsLdpcOn(h))
   {
	   BAST_g2_P_ReadRegister(h, BCHP_AFEC_BCH_DECCBLK, &corr);
	   BAST_g2_P_ReadRegister(h, BCHP_AFEC_BCH_DECBBLK, &bad);
	   BAST_g2_P_ReadRegister(h, BCHP_AFEC_BCH_DECNBLK, &total);

	   BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_DECCBLK, &val);
	   BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_DECBBLK, &val);
	   BAST_g2_P_WriteRegister(h, BCHP_AFEC_BCH_DECNBLK, &val);
	   
	   hChn->ldpcBadBlocks += bad;
	   hChn->ldpcCorrBlocks += corr;
	   hChn->ldpcTotalBlocks += total;
   }
#ifndef BAST_EXCLUDE_TURBO
   else if (BAST_g2_P_IsTurboOn(h))
   {
      BAST_g2_P_TurboUpdateErrorCounters(h);
   }
#endif   
   BAST_g2_P_UpdateErrorCounters(h);   
}


/******************************************************************************
 BAST_g2_P_LdpcSetPlc()
******************************************************************************/
void BAST_g2_P_LdpcSetPlc(BAST_ChannelHandle h, bool bAcq)
{
   static uint8_t *BAST_acq_default_piloton[] =
   {
      BAST_bw_acq_b0_default_piloton_QPSK_low,
      BAST_damp_acq_b0_default_piloton_QPSK_low,
      BAST_bw_acq_b0_default_piloton_QPSK_high,
      BAST_damp_acq_b0_default_piloton_QPSK_high,
      BAST_bw_acq_b0_default_piloton_8PSK_low,
      BAST_damp_acq_b0_default_piloton_8PSK_low,
      BAST_bw_acq_b0_default_piloton_8PSK_high,
      BAST_damp_acq_b0_default_piloton_8PSK_high
   };
   
   static uint8_t *BAST_acq_euro_piloton[] =
   {
      BAST_bw_acq_b0_euro_piloton_QPSK_low,
      BAST_damp_acq_b0_euro_piloton_QPSK_low,
      BAST_bw_acq_b0_euro_piloton_QPSK_high,
      BAST_damp_acq_b0_euro_piloton_QPSK_high,
      BAST_bw_acq_b0_euro_piloton_8PSK_low,
      BAST_damp_acq_b0_euro_piloton_8PSK_low,
      BAST_bw_acq_b0_euro_piloton_8PSK_high,
      BAST_damp_acq_b0_euro_piloton_8PSK_high
   };
   
   static uint8_t *BAST_acq_default_pilotoff[] =
   {
      BAST_bw_acq_b0_default_pilotoff_QPSK_low,
      BAST_damp_acq_b0_default_pilotoff_QPSK_low,
      BAST_bw_acq_b0_default_pilotoff_QPSK_high,
      BAST_damp_acq_b0_default_pilotoff_QPSK_high,
      BAST_bw_acq_b0_default_pilotoff_8PSK_low,
      BAST_damp_acq_b0_default_pilotoff_8PSK_low,
      BAST_bw_acq_b0_default_pilotoff_8PSK_high,
      BAST_damp_acq_b0_default_pilotoff_8PSK_high   
   };
   
   static uint8_t *BAST_acq_euro_pilotoff[] =
   {
      BAST_bw_acq_b0_euro_pilotoff_QPSK_low,
      BAST_damp_acq_b0_euro_pilotoff_QPSK_low,
      BAST_bw_acq_b0_euro_pilotoff_QPSK_high,
      BAST_damp_acq_b0_euro_pilotoff_QPSK_high,
      BAST_bw_acq_b0_euro_pilotoff_8PSK_low,
      BAST_damp_acq_b0_euro_pilotoff_8PSK_low,
      BAST_bw_acq_b0_euro_pilotoff_8PSK_high,
      BAST_damp_acq_b0_euro_pilotoff_8PSK_high   
   };
   
   static uint8_t *BAST_trk_default_piloton[] =
   {
      BAST_bw_trk_b0_default_piloton_QPSK_low,
      BAST_damp_trk_b0_default_piloton_QPSK_low,
      BAST_bw_trk_b0_default_piloton_QPSK_high,
      BAST_damp_trk_b0_default_piloton_QPSK_high,
      BAST_bw_trk_b0_default_piloton_8PSK_low,
      BAST_damp_trk_b0_default_piloton_8PSK_low,
      BAST_bw_trk_b0_default_piloton_8PSK_3_4,
      BAST_damp_trk_b0_default_piloton_8PSK_3_4,
      BAST_bw_trk_b0_default_piloton_8PSK_5_6,
      BAST_damp_trk_b0_default_piloton_8PSK_5_6,
      BAST_bw_trk_b0_default_piloton_8PSK_high,
      BAST_damp_trk_b0_default_piloton_8PSK_high
   };
   
   static uint8_t *BAST_trk_euro_piloton[] =
   {
      BAST_bw_trk_b0_euro_piloton_QPSK_low,
      BAST_damp_trk_b0_euro_piloton_QPSK_low,
      BAST_bw_trk_b0_euro_piloton_QPSK_high,
      BAST_damp_trk_b0_euro_piloton_QPSK_high,
      BAST_bw_trk_b0_euro_piloton_8PSK_low,
      BAST_damp_trk_b0_euro_piloton_8PSK_low,
      BAST_bw_trk_b0_euro_piloton_8PSK_3_4,
      BAST_damp_trk_b0_euro_piloton_8PSK_3_4,
      BAST_bw_trk_b0_euro_piloton_8PSK_5_6,
      BAST_damp_trk_b0_euro_piloton_8PSK_5_6,
      BAST_bw_trk_b0_euro_piloton_8PSK_high,
      BAST_damp_trk_b0_euro_piloton_8PSK_high
   };
   
   static uint8_t *BAST_trk_awgn_piloton[] =
   {
      BAST_bw_trk_b0_awgn_piloton_QPSK_low,
      BAST_damp_trk_b0_awgn_piloton_QPSK_low,
      BAST_bw_trk_b0_awgn_piloton_QPSK_high,
      BAST_damp_trk_b0_awgn_piloton_QPSK_high,
      BAST_bw_trk_b0_awgn_piloton_8PSK_low,
      BAST_damp_trk_b0_awgn_piloton_8PSK_low,
      BAST_bw_trk_b0_awgn_piloton_8PSK_3_4,
      BAST_damp_trk_b0_awgn_piloton_8PSK_3_4,
      BAST_bw_trk_b0_awgn_piloton_8PSK_5_6,
      BAST_damp_trk_b0_awgn_piloton_8PSK_5_6,
      BAST_bw_trk_b0_awgn_piloton_8PSK_high,
      BAST_damp_trk_b0_awgn_piloton_8PSK_high
   };
   
   static uint8_t *BAST_trk_default_pilotoff[] =
   {
      BAST_bw_trk_a1_default_pilotoff_QPSK_low,
      BAST_damp_trk_a1_default_pilotoff_QPSK_low,
      BAST_bw_trk_a1_default_pilotoff_QPSK_high,
      BAST_damp_trk_a1_default_pilotoff_QPSK_high,
      BAST_bw_trk_a1_default_pilotoff_8PSK_low,
      BAST_damp_trk_a1_default_pilotoff_8PSK_low,
      BAST_bw_trk_a1_default_pilotoff_8PSK_3_4,
      BAST_damp_trk_a1_default_pilotoff_8PSK_3_4,
      BAST_bw_trk_a1_default_pilotoff_8PSK_5_6,
      BAST_damp_trk_a1_default_pilotoff_8PSK_5_6,
      BAST_bw_trk_a1_default_pilotoff_8PSK_high,
      BAST_damp_trk_a1_default_pilotoff_8PSK_high
   };
   
   static uint8_t *BAST_trk_euro_pilotoff[] =
   {
      BAST_bw_trk_a1_euro_pilotoff_QPSK_low,
      BAST_damp_trk_a1_euro_pilotoff_QPSK_low,
      BAST_bw_trk_a1_euro_pilotoff_QPSK_high,
      BAST_damp_trk_a1_euro_pilotoff_QPSK_high,
      BAST_bw_trk_a1_euro_pilotoff_8PSK_low,
      BAST_damp_trk_a1_euro_pilotoff_8PSK_low,
      BAST_bw_trk_a1_euro_pilotoff_8PSK_3_4,
      BAST_damp_trk_a1_euro_pilotoff_8PSK_3_4,
      BAST_bw_trk_a1_euro_pilotoff_8PSK_5_6,
      BAST_damp_trk_a1_euro_pilotoff_8PSK_5_6,
      BAST_bw_trk_a1_euro_pilotoff_8PSK_high,
      BAST_damp_trk_a1_euro_pilotoff_8PSK_high
   };
   
   static uint8_t *BAST_trk_awgn_pilotoff[] =
   {
      BAST_bw_trk_a1_awgn_pilotoff_QPSK_low,
      BAST_damp_trk_a1_awgn_pilotoff_QPSK_low,
      BAST_bw_trk_a1_awgn_pilotoff_QPSK_high,
      BAST_damp_trk_a1_awgn_pilotoff_QPSK_high,
      BAST_bw_trk_a1_awgn_pilotoff_8PSK_low,
      BAST_damp_trk_a1_awgn_pilotoff_8PSK_low,
      BAST_bw_trk_a1_awgn_pilotoff_8PSK_3_4,
      BAST_damp_trk_a1_awgn_pilotoff_8PSK_3_4,
      BAST_bw_trk_a1_awgn_pilotoff_8PSK_5_6,
      BAST_damp_trk_a1_awgn_pilotoff_8PSK_5_6,
      BAST_bw_trk_a1_awgn_pilotoff_8PSK_high,
      BAST_damp_trk_a1_awgn_pilotoff_8PSK_high
   };
   
   static uint8_t *BAST_trk_turbo[] =
   {
      BAST_bw_trk_turbo_QPSK_low,
      BAST_damp_trk_turbo_QPSK_low,
      BAST_bw_trk_turbo_QPSK_high,
      BAST_damp_trk_turbo_QPSK_high,
      BAST_bw_trk_turbo_8PSK_low,
      BAST_damp_trk_turbo_8PSK_low,
      BAST_bw_trk_turbo_8PSK_high,
      BAST_damp_trk_turbo_8PSK_high
   };
   
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, bw1, bw2, damp1, damp2, bw, damp, P_hi, P_lo, Q_hi, Q_lo;
   uint16_t *pBwScaleTable;
   uint8_t **pTableSet, *pBwTable, *pDampTable;
   bool bTurbo;
   int32_t offset, i = -1;
   
   bTurbo = BAST_MODE_IS_TURBO(hChn->actualMode);
   
   if (bAcq)
   {
      /* acquisition */
      pBwScaleTable = BAST_plc_acq_bw_scale;
      if (hChn->ldpcCtl & BAST_G2_LDPC_CTL_USE_ALT_ACQ_PLC)
      {
         bw = hChn->altPlcAcqBw;
         damp = (hChn->altPlcAcqDamp * 4) / BAST_PLC_DAMP_SCALE;
         goto set_plc;
      }
      else if (BAST_g2_P_LdpcIsPilotOn(h) || bTurbo)
      {
         /* acquisition, LDPC pilot on */                
         if (h->pDevice->settings.networkSpec == BAST_NetworkSpec_eEuro)
         {
#ifdef BAST_DEBUG_PLC         
            BDBG_MSG(("using BAST_acq_euro_piloton"));
#endif            
            pTableSet = BAST_acq_euro_piloton;
         }
         else
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_acq_default_piloton"));
#endif            
            pTableSet = BAST_acq_default_piloton; /* turbo uses this table */
         }
      }
      else
      {
         /* acquisition, LDPC no pilot */
         if (h->pDevice->settings.networkSpec == BAST_NetworkSpec_eEuro)
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_acq_euro_pilotoff"));
#endif            
            pTableSet = BAST_acq_euro_pilotoff;
         }
         else
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_acq_default_pilotoff"));  
#endif            
            pTableSet = BAST_acq_default_pilotoff;     
         }            
      }
      
      if (hChn->actualMode == BAST_Mode_eUnknown)
         offset = 4;
      else if (bTurbo)
      {
         if (hChn->actualMode >= BAST_Mode_eTurbo_8psk_4_5)
            offset = 6;
         else if (hChn->actualMode >= BAST_Mode_eTurbo_8psk_2_3)
            offset = 4;
         else if (hChn->actualMode >= BAST_Mode_eTurbo_Qpsk_3_4)
            offset = 2;
         else
            offset = 0;
      }
      else if (hChn->actualMode >= BAST_Mode_eLdpc_8psk_8_9)
         offset = 6;
      else if (hChn->actualMode >= BAST_Mode_eLdpc_8psk_3_5)
         offset = 4;
      else if (hChn->actualMode >= BAST_Mode_eLdpc_Qpsk_3_4)
         offset = 2;
      else 
         offset = 0;
   }
   else if (hChn->ldpcCtl & BAST_G2_LDPC_CTL_USE_ALT_TRK_PLC)
   {
      bw = hChn->altPlcTrkBw;
      damp = (hChn->altPlcTrkDamp * 4) / BAST_PLC_DAMP_SCALE;
      goto set_plc;
   }
   else
   {
      /* tracking */  
      pBwScaleTable = BAST_plc_trk_bw_scale;
      if (bTurbo)
      {
         /* turbo */
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_trk_turbo")); 
#endif            
         pBwScaleTable = BAST_plc_turbo_trk_bw_scale;
         pTableSet = BAST_trk_turbo;
      }
      else if (BAST_g2_P_LdpcIsPilotOn(h))
      {
         /* tracking, LDPC pilot on */
         if (hChn->ldpcCtl & BAST_G2_LDPC_CTL_AWGN_PLC)
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_trk_awgn_piloton")); 
#endif            
            pTableSet = BAST_trk_awgn_piloton;
         }
         else if (h->pDevice->settings.networkSpec == BAST_NetworkSpec_eEuro)
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_trk_euro_piloton"));
#endif            
            pTableSet = BAST_trk_euro_piloton;
         }
         else
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_trk_default_piloton"));
#endif
            if ((hChn->actualMode == BAST_Mode_eLdpc_8psk_3_5) && 
                (hChn->acqParams.symbolRate >= 20000000))
            {
               /* originally, bw=0, damp=0 --> PLC=0x00000100 */
               val = 0x00010001; /* improved QEF and no degradation on CCI */
               BAST_g2_P_WriteRegister(h, BCHP_SDS_PLC, &val);
               BAST_g2_P_WriteRegister(h, BCHP_SDS_PLC1, &val);
#if ((BCHP_CHIP!=7325) && (BCHP_CHIP!=7335)) || (BCHP_VER >= BCHP_VER_B0)
               BAST_g2_P_WriteRegister(h, BCHP_SDS_FBLC, &val);
#endif             
               return;
            }
            
            pTableSet = BAST_trk_default_piloton;
         }
      }
      else
      {
         /* tracking, LDPC no pilot */        
         if (hChn->ldpcCtl & BAST_G2_LDPC_CTL_AWGN_PLC)
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_trk_awgn_pilotoff"));
#endif            
            pTableSet = BAST_trk_awgn_pilotoff;
         }
         else if (h->pDevice->settings.networkSpec == BAST_NetworkSpec_eEuro)
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_trk_euro_pilotoff"));
#endif            
            pTableSet = BAST_trk_euro_pilotoff;
         }
         else
         {
#ifdef BAST_DEBUG_PLC            
            BDBG_MSG(("using BAST_trk_default_pilotoff"));
#endif            
            pTableSet = BAST_trk_default_pilotoff;  

#if 0
            /* special case for QPSK 1/2 no pilot optimized for ETSI phase noise */
            if (hChn->actualMode == BAST_Mode_eLdpc_Qpsk_1_2)
            {
               val = 0x0415060F;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_PLC, &val);
               BAST_g2_P_WriteRegister(h, BCHP_SDS_PLC1, &val);    
#if ((BCHP_CHIP!=7325) && (BCHP_CHIP!=7335)) || (BCHP_VER >= BCHP_VER_B0)
               BAST_g2_P_WriteRegister(h, BCHP_SDS_FBLC, &val);
#endif
               val = 0x30;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_EQMODE, &val);          
            }
#endif            
         }            
      }
      
      if (bTurbo)
      {
         if (hChn->actualMode >= BAST_Mode_eTurbo_8psk_4_5)
            offset = 6;
         else if (hChn->actualMode >= BAST_Mode_eTurbo_8psk_2_3)
            offset = 4;
         else if (hChn->actualMode >= BAST_Mode_eTurbo_Qpsk_3_4)
            offset = 2;
         else
            offset = 0;      
      }
      else if (hChn->actualMode >= BAST_Mode_eLdpc_8psk_8_9)
         offset = 10;
      else if (hChn->actualMode == BAST_Mode_eLdpc_8psk_5_6)
         offset = 8;
      else if (hChn->actualMode == BAST_Mode_eLdpc_8psk_3_4)
         offset = 6;
      else if (hChn->actualMode >= BAST_Mode_eLdpc_8psk_3_5)
         offset = 4;
      else if (hChn->actualMode >= BAST_Mode_eLdpc_Qpsk_3_4)
         offset = 2;
      else 
         offset = 0;  
   }
   
   pBwTable = pTableSet[offset];
   pDampTable = pTableSet[offset+1];
      
   for (i = 0; i < BAST_PLC_NUM_SYMRATES; i++)
   {
      if (hChn->acqParams.symbolRate >= BAST_plc_symbol_rate[i])
         break;
   }
   
   if (i == 0)
      goto dont_do_interpolation;
      
   if (i >= BAST_PLC_NUM_SYMRATES)
   {
      i = BAST_PLC_NUM_SYMRATES - 1;
      
      dont_do_interpolation:
      BAST_g2_P_LdpcLookupPlcBwDamp(pBwTable, pBwScaleTable, pDampTable, i, &bw, &damp);
   }
   else
   {
      /* do linear interpolation between i and i-1 */
      BAST_g2_P_LdpcLookupPlcBwDamp(pBwTable, pBwScaleTable, pDampTable, i, &bw1, &damp1);
      BAST_g2_P_LdpcLookupPlcBwDamp(pBwTable, pBwScaleTable, pDampTable, i-1, &bw2, &damp2);
      
      if (bw2 >= bw1)
         val = bw2 - bw1;
      else
         val = bw1 - bw2;
      BAST_MultU32U32(hChn->acqParams.symbolRate - BAST_plc_symbol_rate[i], val, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, BAST_plc_symbol_rate[i-1] - BAST_plc_symbol_rate[i], &Q_hi, &Q_lo);
      if (bw2 >= bw1)
         bw = Q_lo + bw1;
      else
         bw = bw1 - Q_lo;
      
      if (damp2 >= damp1)
         val = damp2 - damp1;
      else
         val = damp1 - damp2;
      BAST_MultU32U32(hChn->acqParams.symbolRate - BAST_plc_symbol_rate[i], val, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, BAST_plc_symbol_rate[i-1] - BAST_plc_symbol_rate[i], &Q_hi, &Q_lo);
      if (damp2 >= damp1)
         damp = Q_lo + damp1;  
      else
         damp = damp1 - Q_lo;      
   }
   
   set_plc:
#ifdef BAST_DEBUG_PLC      
   BDBG_MSG(("PLC: bAcq=%d,bw=%d,damp(x4)=%d", bAcq, bw, damp));
#endif   
   BAST_g2_P_SetBackCarrierBw(h, bw, damp);
}


/******************************************************************************
 BAST_g2_P_LdpcLookupPlcBwDamp()
******************************************************************************/
void BAST_g2_P_LdpcLookupPlcBwDamp(uint8_t *pBwTable, uint16_t *pBwScaleTable, uint8_t *pDampTable, int32_t i, uint32_t *pBw, uint32_t *pDamp)
{   
   *pBw = (uint32_t)(pBwTable[i] * pBwScaleTable[i]);
   *pDamp = (uint32_t)((pDampTable[i] * 8) / BAST_PLC_DAMP_SCALE); 
   *pDamp = *pDamp + 1;
   *pDamp = *pDamp >> 1;
}



