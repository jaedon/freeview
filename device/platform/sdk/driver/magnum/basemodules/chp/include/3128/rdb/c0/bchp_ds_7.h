/***************************************************************************
 *     Copyright (c) 1999-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_ds_7.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/6/11 11:40a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct  6 10:25:09 2011
 *                 MD5 Checksum         8f37dfc69866893136bc4603c1bedf8c
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3128/rdb/c0/bchp_ds_7.h $
 * 
 * Hydra_Software_Devel/1   10/6/11 11:40a farshidf
 * SW3128-1: add C0 RDB files
 *
 ***************************************************************************/

#ifndef BCHP_DS_7_H__
#define BCHP_DS_7_H__

/***************************************************************************
 *DS_7 - Downstream Receiver 7 Registers
 ***************************************************************************/
#define BCHP_DS_7_GBL                            0x000ba000 /* Global Core Control Register */
#define BCHP_DS_7_PD                             0x000ba010 /* Global Power Down Register */
#define BCHP_DS_7_IRQSTS1                        0x000ba040 /* Interrupt Status Register 1 */
#define BCHP_DS_7_IRQSET1                        0x000ba044 /* Set Interrupt Status Register 1 */
#define BCHP_DS_7_IRQCLR1                        0x000ba048 /* Clear Interrupt Status Register 1 */
#define BCHP_DS_7_IRQMSK1                        0x000ba04c /* Interrupt Mask Register 1 */
#define BCHP_DS_7_IRQMSET1                       0x000ba050 /* Set Interrupt Mask Register 1 */
#define BCHP_DS_7_IRQMCLR1                       0x000ba054 /* Clear Interrupt Mask Register 1 */
#define BCHP_DS_7_IRQSTS2                        0x000ba058 /* Interrupt Status Register 2 */
#define BCHP_DS_7_IRQSET2                        0x000ba05c /* Set Interrupt Status Register 2 */
#define BCHP_DS_7_IRQCLR2                        0x000ba060 /* Clear Interrupt Status Register 2 */
#define BCHP_DS_7_IRQMSK2                        0x000ba064 /* Interrupt Mask Register 2 */
#define BCHP_DS_7_IRQMSET2                       0x000ba068 /* Set Interrupt Mask Register 2 */
#define BCHP_DS_7_IRQMCLR2                       0x000ba06c /* Clear Interrupt Mask Register 2 */
#define BCHP_DS_7_STAT                           0x000ba0fc /* Receiver Status Register */
#define BCHP_DS_7_RST                            0x000ba100 /* Global Reset Register */
#define BCHP_DS_7_FRZ                            0x000ba104 /* Global Freeze Register */
#define BCHP_DS_7_BURST_FRZ                      0x000ba10c /* Global Burst noise detector Freeze  Register */
#define BCHP_DS_7_CLK                            0x000ba184 /* Clock Generation Control Register */
#define BCHP_DS_7_NCOU                           0x000ba190 /* NCO Instantaneous Value (Upper) */
#define BCHP_DS_7_NCOL                           0x000ba194 /* NCO Instantaneous Value (Lower) */
#define BCHP_DS_7_FECIN_NCON                     0x000ba198 /* FEC Clock Counter Numerator Value */
#define BCHP_DS_7_FECIN_NCODL                    0x000ba19c /* FEC Clock Counter Delta Value */
#define BCHP_DS_7_FECOUT_NCON                    0x000ba1a0 /* OI Clock Rate Numerator */
#define BCHP_DS_7_FECOUT_NCODL                   0x000ba1a4 /* OI Clock Rate Delta */
#define BCHP_DS_7_US_FCW_DWELL                   0x000ba1a8 /* Upstream Frequency Control Word Dwell-count register */
#define BCHP_DS_7_SGCG                           0x000ba1ac /* Clockgen Signature Analyzer */
#define BCHP_DS_7_ICB_CTL                        0x000ba200 /* Internal Configuration Bus Control and Status */
#define BCHP_DS_7_MBOX_CSR_P                     0x000ba204 /* Mail Box Command and Status for processor */
#define BCHP_DS_7_MBOX_DATA_P                    0x000ba208 /* Mail Box Data for processor */
#define BCHP_DS_7_HI_TST                         0x000ba214 /* Test configuration for down steam core's bus interface */
#define BCHP_DS_7_MBOX_CSR_S                     0x000ba218 /* Mail Box Command and Status for serial test interface. */
#define BCHP_DS_7_MBOX_DATA_S                    0x000ba21c /* Mail Box Data for serial test interface */
#define BCHP_DS_7_BR                             0x000ba300 /* Baud Receiver Control Register */
#define BCHP_DS_7_AGCB                           0x000ba34c /* Digital AGCB Control Register */
#define BCHP_DS_7_AGCBI                          0x000ba350 /* Digital AGCB Gain Integrator Value */
#define BCHP_DS_7_AGCBLI                         0x000ba354 /* Digital AGCB Power Leaky Integrator Value */
#define BCHP_DS_7_SGBR                           0x000ba358 /* Baud Receiver Signature Analyzer */
#define BCHP_DS_7_QMLPS                          0x000ba400 /* QAM Loop Control */
#define BCHP_DS_7_CFL                            0x000ba410 /* Carrier Frequency Loop Control Register */
#define BCHP_DS_7_CFLC                           0x000ba414 /* Carrier Frequency Loop Coefficient Control Register */
#define BCHP_DS_7_CFLI                           0x000ba418 /* Carrier Frequency Loop Integrator Value */
#define BCHP_DS_7_CFLSP                          0x000ba41c /* Carrier Frequency Loop Sweep Control Register */
#define BCHP_DS_7_CFLFOS                         0x000ba480 /* Carrier Frequency Loop Frequency Offset Control Register */
#define BCHP_DS_7_CFLFO                          0x000ba488 /* Carrier Frequency Loop Filter Output Value */
#define BCHP_DS_7_TL                             0x000ba494 /* Timing Loop Control Register */
#define BCHP_DS_7_TLC                            0x000ba498 /* Timing Loop Coefficient Control Register */
#define BCHP_DS_7_TLI                            0x000ba49c /* Timing Loop Integrator Value */
#define BCHP_DS_7_TLSWP                          0x000ba4a0 /* Timing Loop Sweep Control Value */
#define BCHP_DS_7_TLTHRS                         0x000ba4a4 /* Timing Loop Integrator Threshold Register */
#define BCHP_DS_7_TLFOS                          0x000ba4a8 /* Timing Loop Phase Offset Control Register */
#define BCHP_DS_7_TLFO                           0x000ba4ac /* Timing Loop Filter Output Value */
#define BCHP_DS_7_TLAGC                          0x000ba504 /* Timing Loop AGC Control Register */
#define BCHP_DS_7_TLAGCI                         0x000ba508 /* Timing Loop AGC Integrator Value */
#define BCHP_DS_7_TLAGCL                         0x000ba50c /* Timing Loop AGC Leaky Integrator Value */
#define BCHP_DS_7_PERF                           0x000ba510 /* Performance Monitoring Control/Status Register */
#define BCHP_DS_7_TLDHT                          0x000ba514 /* Timing Lock Detector High Threshold Control Register */
#define BCHP_DS_7_TLDLT                          0x000ba518 /* Timing Lock Detector Low Threshold Control Register */
#define BCHP_DS_7_TLDA                           0x000ba51c /* Timing Lock Detector Accumulator Value */
#define BCHP_DS_7_TLDC                           0x000ba520 /* Timing Lock Detector Maximum Count Control Register */
#define BCHP_DS_7_TLDCI                          0x000ba524 /* Timing Lock Detector Counter Value */
#define BCHP_DS_7_US_IFC                         0x000ba530 /* Upstream/Downstream interface control register */
#define BCHP_DS_7_US_FCW_HI                      0x000ba534 /* Upper-part of the Upstream Frequency Control Word register */
#define BCHP_DS_7_US_FCW_LO                      0x000ba538 /* Lower-part of the Upstream Frequency Control Word register */
#define BCHP_DS_7_US_TL_OFFSET                   0x000ba53c /* Upstream Timing Offset register */
#define BCHP_DS_7_US_DSBCLK                      0x000ba540 /* Upstream baud clock register */
#define BCHP_DS_7_FEC                            0x000ba600 /* FEC Control / Status Register */
#define BCHP_DS_7_FECU                           0x000ba610 /* FEC Initialization Register (Upper) */
#define BCHP_DS_7_FECM                           0x000ba614 /* FEC Initialization Register (Middle) */
#define BCHP_DS_7_FECL                           0x000ba618 /* FEC Initialization Register (Lower) */
#define BCHP_DS_7_SGFEC                          0x000ba620 /* FEC Signature Analyzer */
#define BCHP_DS_7_OI_VCO                         0x000ba640 /* OI VCO Control */
#define BCHP_DS_7_OI_CTL                         0x000ba680 /* OI Control */
#define BCHP_DS_7_OI_OUT                         0x000ba684 /* OI PS Output Control */
#define BCHP_DS_7_OI_ERR                         0x000ba69c /* OI Frame Error Count */
#define BCHP_DS_7_OI_SG                          0x000ba6a0 /* Output Interface Signature Analyzer (Test) */
#define BCHP_DS_7_OI_BER_CTL                     0x000ba6a4 /* OI BER Estimation Control Register */
#define BCHP_DS_7_OI_BER                         0x000ba6a8 /* OI BER Estimation Error Counter Value */
#define BCHP_DS_7_BER                            0x000ba700 /* Pre-FEC BER Estimation Control Register */
#define BCHP_DS_7_BERI                           0x000ba704 /* Pre-FEC BER Estimation Error Counter Value */
#define BCHP_DS_7_CERC1                          0x000ba710 /* FEC RS Corrected Bit Counter */
#define BCHP_DS_7_UERC1                          0x000ba714 /* FEC Uncorrectable RS-Block Counter */
#define BCHP_DS_7_NBERC1                         0x000ba718 /* FEC Clean RS-Block Counter */
#define BCHP_DS_7_CBERC1                         0x000ba71c /* FEC Corrected RS-Block Counter */
#define BCHP_DS_7_BMPG1                          0x000ba720 /* FEC Bad MPEG-Packet Counter */
#define BCHP_DS_7_CERC2                          0x000ba724 /* FEC RS Corrected Bit Counter */
#define BCHP_DS_7_UERC2                          0x000ba728 /* FEC Uncorrectable RS-Block Counter */
#define BCHP_DS_7_NBERC2                         0x000ba72c /* FEC Clean RS-Block Counter */
#define BCHP_DS_7_CBERC2                         0x000ba730 /* FEC Corrected RS-Block Counter */
#define BCHP_DS_7_BMPG2                          0x000ba734 /* FEC Bad MPEG-Packet Counter */
#define BCHP_DS_7_TPFEC                          0x000ba738 /* Testport Control Register for FEC */
#define BCHP_DS_7_EUSEDC1                        0x000ba73c /* FEC Erasure used RS-Block Counter */
#define BCHP_DS_7_EDISCARDC1                     0x000ba740 /* FEC Erasure discarded RS-Block Counter */
#define BCHP_DS_7_EUSEDC2                        0x000ba744 /* FEC Erasure used RS-Block Counter */
#define BCHP_DS_7_EDISCARDC2                     0x000ba748 /* FEC Erasure discarded RS-Block Counter */
#define BCHP_DS_7_FECMON_CTL                     0x000ba74c /* FEC Monitor Control */
#define BCHP_DS_7_FECMON_LOCK_LIMITS             0x000ba750 /* FEC Monitor Lock Limits */
#define BCHP_DS_7_FECMON_UNLOCK_LIMITS           0x000ba754 /* FEC Monitor BUnlock Limits */
#define BCHP_DS_7_FECMON_FALSE_MPEG_DETECT_CTL1  0x000ba758 /* FEC Monitor False MPEG Detect Control Parameters 1 */
#define BCHP_DS_7_FECMON_FALSE_MPEG_DETECT_CTL2  0x000ba75c /* FEC Monitor False MPEG Detect Control Parameters 2 */
#define BCHP_DS_7_FECMON_FALSE_MPEG_DETECT_STATUS 0x000ba760 /* FEC Monitor False MPEG Detect Status */
#define BCHP_DS_7_FECMON_INTERVAL_ALERT_THRESHOLD 0x000ba764 /* FEC Monitor Error Count Alert Thresholds */
#define BCHP_DS_7_FECMON_INTERVAL_ERROR_VALUES   0x000ba768 /* FEC Monitor Error Counts within the last trigger interval */
#define BCHP_DS_7_FSCNT1                         0x000ba790 /* Sample Rate Counter 1 */
#define BCHP_DS_7_FSCNT2                         0x000ba794 /* Sample Rate Counter 2 */
#define BCHP_DS_7_FBCNT1                         0x000ba7c0 /* Baud Rate Counter 1 */
#define BCHP_DS_7_FBCNT2                         0x000ba7c4 /* Baud Rate Counter 2 */
#define BCHP_DS_7_SPARE                          0x000ba7fc /* Reserved for Future Expansion */
#define BCHP_DS_7_BND                            0x000bab00 /* BND Control Register */
#define BCHP_DS_7_BND_THR                        0x000bab04 /* BND threshold value Register */
#define BCHP_DS_7_BND_FRZ                        0x000bab08 /* BND Freeze Control Register */
#define BCHP_DS_7_BND_THRFRZ                     0x000bab0c /* BND threshold value Register */
#define BCHP_DS_7_EQ_CTL                         0x000bac00 /* Equalizer Control Register */
#define BCHP_DS_7_EQ_CWC                         0x000bac04 /* CWC Control Register */
#define BCHP_DS_7_EQ_CWC_FSBAUD                  0x000bac08 /* CWC BAUD SAMPLE RATE */
#define BCHP_DS_7_EQ_CWC_FSCARR                  0x000bac0c /* CWC CARRIER SAMPLE RATE */
#define BCHP_DS_7_EQ_FFE                         0x000bac10 /* FFE Control Register */
#define BCHP_DS_7_EQ_DFE                         0x000bac14 /* DFE Control Register */
#define BCHP_DS_7_EQ_CMA                         0x000bac18 /* Equalizer CMA Modulus Control Register */
#define BCHP_DS_7_EQ_RCA                         0x000bac1c /* Equalizer RCA Gain Control Register */
#define BCHP_DS_7_EQ_FMTHR                       0x000bac20 /* Equalizer Main Tap Threshold Control Register */
#define BCHP_DS_7_EQ_LEAK                        0x000bac24 /* Equalizer Leakage Control Register */
#define BCHP_DS_7_EQ_SOFT                        0x000bac28 /* Equalizer Soft Decision Value */
#define BCHP_DS_7_EQ_SGEQ                        0x000bac2c /* Equalizer Signature Analyzer */
#define BCHP_DS_7_EQ_CPL                         0x000bac30 /* Carrier Phase Loop Control */
#define BCHP_DS_7_EQ_CPLC                        0x000bac34 /* Carrier Phase Loop Coefficients */
#define BCHP_DS_7_EQ_CPLSWP                      0x000bac38 /* Carrier Phase Loop Sweep */
#define BCHP_DS_7_EQ_CPLI                        0x000bac3c /* Carrier Phase Loop Integrator */
#define BCHP_DS_7_EQ_CPLPA                       0x000bac40 /* Carrier Phase Loop Phase Accumulator */
#define BCHP_DS_7_EQ_CPLFO                       0x000bac44 /* Carrier Phase Loop Filter Output */
#define BCHP_DS_7_EQ_SNR                         0x000bac48 /* Slicer SNR */
#define BCHP_DS_7_EQ_SNRHT                       0x000bac4c /* Slicer SNR High Threshold */
#define BCHP_DS_7_EQ_SNRLT                       0x000bac50 /* Slicer SNR Low Threshold */
#define BCHP_DS_7_EQ_CLD                         0x000bac54 /* Carrier Lock Detector */
#define BCHP_DS_7_EQ_CLDHT                       0x000bac58 /* Carrier Lock Detector High Threshold */
#define BCHP_DS_7_EQ_CLDLT                       0x000bac5c /* Carrier Lock Detector Low Threshold */
#define BCHP_DS_7_EQ_CLDA                        0x000bac60 /* Carrier Lock Detector Accumulator */
#define BCHP_DS_7_EQ_CLDC                        0x000bac64 /* Carrier Lock Detector Maximum Count Control */
#define BCHP_DS_7_EQ_CLDCI                       0x000bac68 /* Carrier Lock Detector Counter */
#define BCHP_DS_7_EQ_CWC_LEAK                    0x000bac6c /* CWC LEAK Control Register */
#define BCHP_DS_7_EQ_CWC_FIN1                    0x000bac70 /* CWC 1st tap non-baud-related frequency in Hz */
#define BCHP_DS_7_EQ_CWC_FIN2                    0x000bac74 /* CWC 2nd tap non-baud-related frequency in Hz */
#define BCHP_DS_7_EQ_CWC_FIN3                    0x000bac78 /* CWC 3rd tap non-baud-related frequency in Hz */
#define BCHP_DS_7_EQ_CWC_FIN4                    0x000bac7c /* CWC 4th tap non-baud-related frequency in Hz */
#define BCHP_DS_7_EQ_CWC_FOFS1                   0x000bac80 /* CWC 1st tap baud-related frequency offset control word */
#define BCHP_DS_7_EQ_CWC_FOFS2                   0x000bac84 /* CWC 2nd tap baud-related frequency offset control word */
#define BCHP_DS_7_EQ_CWC_FOFS3                   0x000bac88 /* CWC 3rd tap baud-related frequency offset control word */
#define BCHP_DS_7_EQ_CWC_FOFS4                   0x000bac8c /* CWC 4th tap baud-related frequency offset control word */
#define BCHP_DS_7_EQ_CWC_LFC1                    0x000bac90 /* CWC 1st tap phase/freq loop filter control */
#define BCHP_DS_7_EQ_CWC_LFC2                    0x000bac94 /* CWC 2nd tap phase/freq loop filter control */
#define BCHP_DS_7_EQ_CWC_LFC3                    0x000bac98 /* CWC 3rd tap phase/freq loop filter control */
#define BCHP_DS_7_EQ_CWC_LFC4                    0x000bac9c /* CWC 4th tap phase/freq loop filter control */
#define BCHP_DS_7_EQ_AGC                         0x000baca4 /* Equalizer Hum-AGC Loop Control */
#define BCHP_DS_7_EQ_AGCC                        0x000baca8 /* Equalizer Hum-AGC Loop Coefficients */
#define BCHP_DS_7_EQ_AGCI                        0x000bacac /* Equalizer Hum-AGC Loop Filter Integrator */
#define BCHP_DS_7_EQ_AGCPA                       0x000bacb0 /* Equalizer Hum-AGC Loop Gain Accumulator */
#define BCHP_DS_7_EQ_AGCFO                       0x000bacb4 /* Equalizer Hum-AGC Loop Filter Output */
#define BCHP_DS_7_EQ_FN                          0x000bacb8 /* Equalizer Hum-AGC FN Modulus Control Register */
#define BCHP_DS_7_EQ_POWER                       0x000bacbc /* Equalizer Input Power Estimator Register */
#define BCHP_DS_7_EQ_FFEU0                       0x000bad00 /* FFE Coefficient Register 0 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL0                       0x000bad04 /* FFE Coefficient Register 0 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU1                       0x000bad08 /* FFE Coefficient Register 1 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL1                       0x000bad0c /* FFE Coefficient Register 1 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU2                       0x000bad10 /* FFE Coefficient Register 2 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL2                       0x000bad14 /* FFE Coefficient Register 2 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU3                       0x000bad18 /* FFE Coefficient Register 3 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL3                       0x000bad1c /* FFE Coefficient Register 3 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU4                       0x000bad20 /* FFE Coefficient Register 4 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL4                       0x000bad24 /* FFE Coefficient Register 4 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU5                       0x000bad28 /* FFE Coefficient Register 5 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL5                       0x000bad2c /* FFE Coefficient Register 5 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU6                       0x000bad30 /* FFE Coefficient Register 6 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL6                       0x000bad34 /* FFE Coefficient Register 6 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU7                       0x000bad38 /* FFE Coefficient Register 7 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL7                       0x000bad3c /* FFE Coefficient Register 7 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU8                       0x000bad40 /* FFE Coefficient Register 8 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL8                       0x000bad44 /* FFE Coefficient Register 8 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU9                       0x000bad48 /* FFE Coefficient Register 9 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL9                       0x000bad4c /* FFE Coefficient Register 9 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU10                      0x000bad50 /* FFE Coefficient Register 10 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL10                      0x000bad54 /* FFE Coefficient Register 10 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU11                      0x000bad58 /* FFE Coefficient Register 11 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL11                      0x000bad5c /* FFE Coefficient Register 11 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU12                      0x000bad60 /* FFE Coefficient Register 12 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL12                      0x000bad64 /* FFE Coefficient Register 12 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU13                      0x000bad68 /* FFE Coefficient Register 13 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL13                      0x000bad6c /* FFE Coefficient Register 13 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU14                      0x000bad70 /* FFE Coefficient Register 14 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL14                      0x000bad74 /* FFE Coefficient Register 14 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU15                      0x000bad78 /* FFE Coefficient Register 15 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL15                      0x000bad7c /* FFE Coefficient Register 15 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU16                      0x000bad80 /* FFE Coefficient Register 16 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL16                      0x000bad84 /* FFE Coefficient Register 16 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU17                      0x000bad88 /* FFE Coefficient Register 17 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL17                      0x000bad8c /* FFE Coefficient Register 17 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU18                      0x000bad90 /* FFE Coefficient Register 18 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL18                      0x000bad94 /* FFE Coefficient Register 18 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU19                      0x000bad98 /* FFE Coefficient Register 19 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL19                      0x000bad9c /* FFE Coefficient Register 19 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU20                      0x000bada0 /* FFE Coefficient Register 20 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL20                      0x000bada4 /* FFE Coefficient Register 20 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU21                      0x000bada8 /* FFE Coefficient Register 21 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL21                      0x000badac /* FFE Coefficient Register 21 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU22                      0x000badb0 /* FFE Coefficient Register 22 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL22                      0x000badb4 /* FFE Coefficient Register 22 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU23                      0x000badb8 /* FFE Coefficient Register 23 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL23                      0x000badbc /* FFE Coefficient Register 23 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU24                      0x000badc0 /* FFE Coefficient Register 24 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL24                      0x000badc4 /* FFE Coefficient Register 24 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU25                      0x000badc8 /* FFE Coefficient Register 25 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL25                      0x000badcc /* FFE Coefficient Register 25 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU26                      0x000badd0 /* FFE Coefficient Register 26 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL26                      0x000badd4 /* FFE Coefficient Register 26 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU27                      0x000badd8 /* FFE Coefficient Register 27 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL27                      0x000baddc /* FFE Coefficient Register 27 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU28                      0x000bade0 /* FFE Coefficient Register 28 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL28                      0x000bade4 /* FFE Coefficient Register 28 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU29                      0x000bade8 /* FFE Coefficient Register 29 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL29                      0x000badec /* FFE Coefficient Register 29 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU30                      0x000badf0 /* FFE Coefficient Register 30 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL30                      0x000badf4 /* FFE Coefficient Register 30 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU31                      0x000badf8 /* FFE Coefficient Register 31 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL31                      0x000badfc /* FFE Coefficient Register 31 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU32                      0x000bae00 /* FFE Coefficient Register 32 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL32                      0x000bae04 /* FFE Coefficient Register 32 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU33                      0x000bae08 /* FFE Coefficient Register 33 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL33                      0x000bae0c /* FFE Coefficient Register 33 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU34                      0x000bae10 /* FFE Coefficient Register 34 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL34                      0x000bae14 /* FFE Coefficient Register 34 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_FFEU35                      0x000bae18 /* FFE Coefficient Register 35 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_FFEL35                      0x000bae1c /* FFE Coefficient Register 35 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU0                       0x000bae20 /* DFE Coefficient Register 0 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL0                       0x000bae24 /* DFE Coefficient Register 0 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU1                       0x000bae28 /* DFE Coefficient Register 1 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL1                       0x000bae2c /* DFE Coefficient Register 1 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU2                       0x000bae30 /* DFE Coefficient Register 2 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL2                       0x000bae34 /* DFE Coefficient Register 2 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU3                       0x000bae38 /* DFE Coefficient Register 3 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL3                       0x000bae3c /* DFE Coefficient Register 3 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU4                       0x000bae40 /* DFE Coefficient Register 4 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL4                       0x000bae44 /* DFE Coefficient Register 4 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU5                       0x000bae48 /* DFE Coefficient Register 5 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL5                       0x000bae4c /* DFE Coefficient Register 5 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU6                       0x000bae50 /* DFE Coefficient Register 6 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL6                       0x000bae54 /* DFE Coefficient Register 6 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU7                       0x000bae58 /* DFE Coefficient Register 7 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL7                       0x000bae5c /* DFE Coefficient Register 7 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU8                       0x000bae60 /* DFE Coefficient Register 8 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL8                       0x000bae64 /* DFE Coefficient Register 8 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU9                       0x000bae68 /* DFE Coefficient Register 9 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL9                       0x000bae6c /* DFE Coefficient Register 9 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU10                      0x000bae70 /* DFE Coefficient Register 10 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL10                      0x000bae74 /* DFE Coefficient Register 10 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU11                      0x000bae78 /* DFE Coefficient Register 11 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL11                      0x000bae7c /* DFE Coefficient Register 11 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU12                      0x000bae80 /* DFE Coefficient Register 12 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL12                      0x000bae84 /* DFE Coefficient Register 12 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU13                      0x000bae88 /* DFE Coefficient Register 13 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL13                      0x000bae8c /* DFE Coefficient Register 13 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU14                      0x000bae90 /* DFE Coefficient Register 14 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL14                      0x000bae94 /* DFE Coefficient Register 14 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU15                      0x000bae98 /* DFE Coefficient Register 15 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL15                      0x000bae9c /* DFE Coefficient Register 15 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU16                      0x000baea0 /* DFE Coefficient Register 16 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL16                      0x000baea4 /* DFE Coefficient Register 16 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU17                      0x000baea8 /* DFE Coefficient Register 17 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL17                      0x000baeac /* DFE Coefficient Register 17 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU18                      0x000baeb0 /* DFE Coefficient Register 18 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL18                      0x000baeb4 /* DFE Coefficient Register 18 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU19                      0x000baeb8 /* DFE Coefficient Register 19 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL19                      0x000baebc /* DFE Coefficient Register 19 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU20                      0x000baec0 /* DFE Coefficient Register 20 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL20                      0x000baec4 /* DFE Coefficient Register 20 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU21                      0x000baec8 /* DFE Coefficient Register 21 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL21                      0x000baecc /* DFE Coefficient Register 21 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU22                      0x000baed0 /* DFE Coefficient Register 22 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL22                      0x000baed4 /* DFE Coefficient Register 22 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU23                      0x000baed8 /* DFE Coefficient Register 23 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL23                      0x000baedc /* DFE Coefficient Register 23 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU24                      0x000baee0 /* DFE Coefficient Register 24 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL24                      0x000baee4 /* DFE Coefficient Register 24 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU25                      0x000baee8 /* DFE Coefficient Register 25 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL25                      0x000baeec /* DFE Coefficient Register 25 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU26                      0x000baef0 /* DFE Coefficient Register 26 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL26                      0x000baef4 /* DFE Coefficient Register 26 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU27                      0x000baef8 /* DFE Coefficient Register 27 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL27                      0x000baefc /* DFE Coefficient Register 27 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU28                      0x000baf00 /* DFE Coefficient Register 28 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL28                      0x000baf04 /* DFE Coefficient Register 28 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU29                      0x000baf08 /* DFE Coefficient Register 29 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL29                      0x000baf0c /* DFE Coefficient Register 29 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU30                      0x000baf10 /* DFE Coefficient Register 30 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL30                      0x000baf14 /* DFE Coefficient Register 30 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU31                      0x000baf18 /* DFE Coefficient Register 31 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL31                      0x000baf1c /* DFE Coefficient Register 31 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU32                      0x000baf20 /* DFE Coefficient Register 32 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL32                      0x000baf24 /* DFE Coefficient Register 32 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU33                      0x000baf28 /* DFE Coefficient Register 33 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL33                      0x000baf2c /* DFE Coefficient Register 33 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU34                      0x000baf30 /* DFE Coefficient Register 34 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL34                      0x000baf34 /* DFE Coefficient Register 34 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_DFEU35                      0x000baf38 /* DFE Coefficient Register 35 (Upper 16 bits I/Q) */
#define BCHP_DS_7_EQ_DFEL35                      0x000baf3c /* DFE Coefficient Register 35 (Lower 8 bits I/Q) */
#define BCHP_DS_7_EQ_CWC_INT1                    0x000bb060 /* CWC Tap 1 Phase/Frequency Loop Integrator Register */
#define BCHP_DS_7_EQ_CWC_INT2                    0x000bb064 /* CWC Tap 2 Phase/Frequency Loop Integrator Register */
#define BCHP_DS_7_EQ_CWC_INT3                    0x000bb068 /* CWC Tap 3 Phase/Frequency Loop Integrator Register */
#define BCHP_DS_7_EQ_CWC_INT4                    0x000bb06c /* CWC Tap 4 Phase/Frequency Loop Integrator Register */
#define BCHP_DS_7_FFT_CTL                        0x000bb070 /* FFT Control Register */
#define BCHP_DS_7_FFT_PDETW                      0x000bb074 /* FFT Peak Detection Window register */
#define BCHP_DS_7_FFT_VAL                        0x000bb078 /* FFT Max Peak Value Register */
#define BCHP_DS_7_FFT_BIN                        0x000bb07c /* FFT BIN Register */
#define BCHP_DS_7_FFT_GAIN_STS                   0x000bb080 /* FFT GAIN Register */
#define BCHP_DS_7_FM_START_ADD                   0x000bb084 /* FFT Memory Start Address Register */
#define BCHP_DS_7_FM_CURR_ADD                    0x000bb088 /* FFT Memory current Address Register */
#define BCHP_DS_7_FM_RW_DATA                     0x000bb08c /* FFT Memory Read/Write Register */
#define BCHP_DS_7_EQ_MAIN                        0x000bb090 /* FFE MAIN Coefficient Register (Upper 16 bits I/Q) */

#endif /* #ifndef BCHP_DS_7_H__ */

/* End of File */