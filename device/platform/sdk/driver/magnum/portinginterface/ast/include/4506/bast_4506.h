/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4506.h $
 * $brcm_Revision: Hydra_Software_Devel/49 $
 * $brcm_Date: 5/11/12 10:33a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/4506/bast_4506.h $
 * 
 * Hydra_Software_Devel/49   5/11/12 10:33a enavarro
 * SWSATFE-128: fixed compiler error under Visual C++
 * 
 * Hydra_Software_Devel/48   12/2/11 5:26p ronchan
 * SWSATFE-163: add length for pretx_vhold and posttx_vhold
 * 
 * Hydra_Software_Devel/47   12/2/11 5:19p ronchan
 * SWSATFE-163: add diseqc pretx_vhold and posttx_vhold config
 * 
 * Hydra_Software_Devel/46   9/12/11 2:39p enavarro
 * SWSATFE-146: added BAST_4506_GetNumTuners()
 * 
 * Hydra_Software_Devel/45   9/8/11 3:06p enavarro
 * SWSATFE-146: moved functions related to VCO avoidance to bast_4506.c
 * and declare them in bast_4506.h so they can be used by the app
 * 
 * Hydra_Software_Devel/44   8/22/11 5:28p ronchan
 * SWSATFE-65: add diseqc_bit_threshold config parameter
 * 
 * Hydra_Software_Devel/43   9/30/10 5:56p ronchan
 * SWSATFE-80: updated DISEQC_CTL2 bit definitions
 * 
 * Hydra_Software_Devel/42   9/29/10 4:10p enavarro
 * SWSATFE-80: added missing bit definitions for XPORT_CTL config
 * parameter
 * 
 * Hydra_Software_Devel/41   9/24/10 2:06p enavarro
 * SWSATFE-81: added lock_history config param
 * 
 * Hydra_Software_Devel/40   4/25/10 11:34a enavarro
 * SWSATFE-52: added splitter_mode_ctl configuration parameter
 * 
 * Hydra_Software_Devel/39   2/2/10 9:46a enavarro
 * SWSATFE-31: added BCM4506_BCM3445_CTL_DISABLE_NOTCH
 * 
 * Hydra_Software_Devel/38   8/27/09 2:54p enavarro
 * HW4506-77: added BCM4506_BCM3445_CTL_METER_MODE bit
 * 
 * Hydra_Software_Devel/37   7/13/09 10:27a enavarro
 * PR 56761: updated BCM3445_CTL bit definition
 * 
 * Hydra_Software_Devel/36   6/10/09 2:43p enavarro
 * PR 54801: added configuration parameters for overriding PLC bandwidths
 * during acquisition and tracking; removed unused configuration
 * parameters
 * 
 * Hydra_Software_Devel/35   6/3/09 9:33a enavarro
 * PR 54801: added ldpc_alt_trk_plc_bw and ldpc_alt_trk_plc_damp config
 * parameters
 * 
 * Hydra_Software_Devel/34   5/5/09 3:41p ronchan
 * PR 54845: Added BAST_4506_WriteHostRegister() and
 * BAST_4506_ReadHostRegister()
 * 
 * Hydra_Software_Devel/33   1/29/09 3:56p enavarro
 * PR 51717: removed max_sweep config parameter
 * 
 * Hydra_Software_Devel/32   1/8/09 10:32a ronchan
 * PR 32395: added comments for BAST_FskChannelConfig
 * 
 * Hydra_Software_Devel/31   12/2/08 11:07a ronchan
 * PR 49859: added bit definition for disabling rx-only mode
 * 
 * Hydra_Software_Devel/30   10/30/08 3:50p enavarro
 * PR 32395: added CNR_THRESH1 config parameter
 * 
 * Hydra_Software_Devel/29   10/30/08 11:35a enavarro
 * PR 32395: added TIMING_LOOP_LOCK config parameter
 * 
 * Hydra_Software_Devel/28   10/23/08 2:35p enavarro
 * PR 32395: added bit definitions for BCM4506_CONFIG_DTV_SCAN_CODE_RATES,
 * BCM4506_CONFIG_DVB_SCAN_CODE_RATES, BCM4506_CONFIG_TURBO_SCAN_MODES,
 * and BCM4506_CONFIG_BLIND_SCAN_MODES configuration parameters
 * 
 * Hydra_Software_Devel/27   10/20/08 2:38p enavarro
 * PR 32395: renamed BLIND_SCAN_SYM_RATE_MIN/BLIND_SCAN_SYM_RATE_MAX to
 * PEAK_SCAN_SYM_RATE_MIN/PEAK_SCAN_SYM_RATE_MAX to be consistent with
 * 73xx
 * 
 * Hydra_Software_Devel/26   10/16/08 11:57a enavarro
 * PR 32395: added bypass OPLL bit in XPORT_CTL config parameter
 * 
 * Hydra_Software_Devel/25   10/15/08 9:47a ronchan
 * PR 32395: fixed BCM3445_CTL config parameter
 * 
 * Hydra_Software_Devel/24   10/14/08 5:13p ronchan
 * PR 32395: added BCM3445_CTL config parameter
 * 
 * Hydra_Software_Devel/23   10/13/08 9:29a ronchan
 * PR 32395: added fsk channel config enumeration
 * 
 * Hydra_Software_Devel/22   9/15/08 9:21a enavarro
 * PR 32395: removed BCM3445_CTL configuration parameter; added
 * STUFF_BYTES config param
 * 
 * Hydra_Software_Devel/21   8/15/08 9:52a ronchan
 * PR 32395: added diseqc_tone_threshold config parameter
 * 
 * Hydra_Software_Devel/20   8/1/08 10:35a enavarro
 * PR 32395: added turbo_ctl config parameter
 * 
 * Hydra_Software_Devel/19   7/21/08 3:29p enavarro
 * PR 32395: removed SNR_THRESHOLD1 and SNR_THRESHOLD2 config parameters
 * 
 * Hydra_Software_Devel/18   7/16/08 1:38p enavarro
 * PR 32395: added DFT_SIZE, DFT_RANGE_START, and DFT_RANGE_END config
 * parameters
 * 
 * Hydra_Software_Devel/17   5/30/08 10:12a enavarro
 * PR 32395: added BCM4506_CONFIG_RT_STATUS_INDICATORS config parameter;
 * removed BCM4506_CONFIG_RAIN_FADE_MAX_SNR config parameter
 * 
 * Hydra_Software_Devel/16   5/27/08 3:46p enavarro
 * PR 32395: added acq_time config parameter
 * 
 * Hydra_Software_Devel/15   4/28/08 3:20p enavarro
 * PR 32395: added some turbo configuration parameters
 * 
 * Hydra_Software_Devel/14   1/15/08 1:48p ronchan
 * PR 32395: added config parameters ftm_tx_power and ftm_ch_select
 * 
 * Hydra_Software_Devel/13   12/11/07 12:24p enavarro
 * PR 32395: removed BAST_4506_3445_LnaStatus struct (use BAST_LnaStatus
 * struct in bast.h instead)
 * 
 * Hydra_Software_Devel/12   10/30/07 11:27a enavarro
 * PR 32395: added network_spec config parameter
 * 
 * Hydra_Software_Devel/11   10/2/07 3:18p enavarro
 * PR 32395: added blind scan config params
 * 
 * Hydra_Software_Devel/10   9/21/07 11:04a enavarro
 * PR 32395: updated config params
 * 
 * Hydra_Software_Devel/9   9/3/07 1:30p enavarro
 * PR 32395: added diseqc_pretx_delay config parameter
 * 
 * Hydra_Software_Devel/8   8/28/07 3:27p enavarro
 * PR 32395: added agc_ctl config parameter
 * 
 * Hydra_Software_Devel/7   8/24/07 1:52p enavarro
 * PR 32395: added tune_dac_* config parameters
 * 
 * Hydra_Software_Devel/6   8/21/07 11:22a enavarro
 * PR 32395: added BCM4506_CONFIG_TUNE_MIX_VCREF
 * 
 * Hydra_Software_Devel/5   8/21/07 10:39a enavarro
 * PR 32395: added BCM4506_CONFIG_TUNE_MIX_DELAY
 * 
 * Hydra_Software_Devel/4   7/30/07 3:06p enavarro
 * PR 33579: Added BAST_GetLnaStatus()
 * 
 * Hydra_Software_Devel/3   7/9/07 4:57p enavarro
 * PR 32395: added xport_clock_adjust config parameter
 * 
 * Hydra_Software_Devel/2   6/22/07 10:54a enavarro
 * PR 32395: fixed compile errors
 * 
 * Hydra_Software_Devel/1   6/21/07 3:39p enavarro
 * PR 32395: initial version
 *
 ***************************************************************************/
 
#ifndef _BAST_4506_H__
#define _BAST_4506_H__               

#ifdef __cplusplus
extern "C" {
#endif

#include "bast.h"
#include "breg_i2c.h"


/******************************************************************************
Summary:
   configuration parameter IDs
Description:
   These are the configuration parameters that can be accessed via 
   BAST_ReadConfig() and BAST_WriteConfig().
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BCM4506_CONFIG_VERSION               0x0000
#define BCM4506_CONFIG_XTAL_FREQ             0x0001
#define BCM4506_CONFIG_FLAGS                 0x0005
#define BCM4506_CONFIG_BCM3445_ADDRESS       0x0006
#define BCM4506_CONFIG_BCM3445_CTL           0x0007
#define BCM4506_CONFIG_FSK_MODE              0x0008
#define BCM4506_CONFIG_TUNER_CTL             0x0009
#define BCM4506_CONFIG_TUNER_CUTOFF          0x000B
#define BCM4506_CONFIG_XPORT_CTL             0x000D
#define BCM4506_CONFIG_LDPC_CTL              0x0011
#define BCM4506_CONFIG_LDPC_ALT_ACQ_PLC_BW   0x0012
#define BCM4506_CONFIG_LOCK_INDICATOR_PIN    0x001A
#define BCM4506_CONFIG_DISEQC_CTL1           0x001C
#define BCM4506_CONFIG_DISEQC_CTL2           0x001E
#define BCM4506_CONFIG_RRTO_USEC             0x0020
#define BCM4506_CONFIG_TONE_AMPLITUDE        0x0028
#define BCM4506_CONFIG_VBOT_AMPLITUDE        0x0029
#define BCM4506_CONFIG_VTOP_AMPLITUDE        0x002A
#define BCM4506_CONFIG_DISEQC_PRETX_DELAY    0x002B
#define BCM4506_CONFIG_TUNE_MIX_DELAY        0x002C
#define BCM4506_CONFIG_TUNE_MIX_VCREF        0x002D
#define BCM4506_CONFIG_TUNE_DAC_DIV_RATIO    0x002E
#define BCM4506_CONFIG_AGC_CTL               0x002F
#define BCM4506_CONFIG_TUNE_DAC_QP2_CNT      0x0030
#define BCM4506_CONFIG_TUNE_DAC_VCREF        0x0031
#define BCM4506_CONFIG_FTM_CORR_CTL          0x0034
#define BCM4506_CONFIG_FTM_CORR_THRES        0x0038
#define BCM4506_CONFIG_FTM_CORR_PEAK_QUAL    0x003C
#define BCM4506_CONFIG_SEARCH_RANGE          0x0040
#define BCM4506_CONFIG_DVB_FINAL_STPLC_LBR   0x0048
#define BCM4506_CONFIG_DVB_FINAL_STPLC       0x004C
#define BCM4506_CONFIG_DTV_FINAL_STPLC       0x0050
#define BCM4506_CONFIG_DCII_FINAL_STPLC      0x0054
#define BCM4506_CONFIG_DTV_1_FINAL_STFLLC    0x0058
#define BCM4506_CONFIG_DTV_1_FINAL_STFLIC    0x005C
#define BCM4506_CONFIG_DTV_2_FINAL_STFLLC    0x0060
#define BCM4506_CONFIG_DTV_2_FINAL_STFLIC    0x0064
#define BCM4506_CONFIG_DVB_1_FINAL_STFLLC    0x0068
#define BCM4506_CONFIG_DVB_1_FINAL_STFLIC    0x006C
#define BCM4506_CONFIG_DVB_2_FINAL_STFLLC    0x0070
#define BCM4506_CONFIG_DVB_2_FINAL_STFLIC    0x0074
#define BCM4506_CONFIG_DCII_FINAL_STFLLC     0x0078
#define BCM4506_CONFIG_DCII_FINAL_STFLIC     0x007C
#define BCM4506_CONFIG_LDPC_SCRAMBLING_SEQ   0x0081
#define BCM4506_CONFIG_FREQ_DRIFT_THRESHOLD  0x00A1
#define BCM4506_CONFIG_RAIN_FADE_THRESHOLD   0x00A5
#define BCM4506_CONFIG_RAIN_FADE_WINDOW      0x00A6
#define BCM4506_CONFIG_ERROR_COUNT_WINDOW    0x00A8
#define BCM4506_CONFIG_XPORT_CLOCK_ADJUST    0x00AA
#define BCM4506_CONFIG_RT_STATUS_INDICATORS  0x00AC
#define BCM4506_CONFIG_DFT_SIZE              0x00AE
#define BCM4506_CONFIG_TURBO_CTL             0x00AF
#define BCM4506_CONFIG_STUFF_BYTES           0x00B0
#define BCM4506_CONFIG_TIMING_LOOP_LOCK      0x00B3
#define BCM4506_CONFIG_LDPC_ALT_TRK_PLC_DAMP 0x00B5
#define BCM4506_CONFIG_DISEQC_TONE_THRESHOLD 0x00B7
#define BCM4506_CONFIG_LDPC_ALT_ACQ_PLC_DAMP 0x00C0
#define BCM4506_CONFIG_SPLITTER_MODE_CTL     0x00C2
#define BCM4506_CONFIG_LOCK_HISTORY          0x00C3
#define BCM4506_CONFIG_DISEQC_PRETX_VHOLD    0x00C4
#define BCM4506_CONFIG_DISEQC_POSTTX_VHOLD   0x00C5
#define BCM4506_CONFIG_BLIND_SCAN_MODES      0x00C6
#define BCM4506_CONFIG_PEAK_SCAN_SYM_RATE_MIN 0x00C7
#define BCM4506_CONFIG_PEAK_SCAN_SYM_RATE_MAX 0x00CB
#define BCM4506_CONFIG_NETWORK_SPEC          0x00CF
#define BCM4506_CONFIG_FTM_TX_POWER          0x00D0
#define BCM4506_CONFIG_FTM_CH_SELECT         0x00D1
#define BCM4506_CONFIG_DTV_SCAN_CODE_RATES   0x00D2
#define BCM4506_CONFIG_DVB_SCAN_CODE_RATES   0x00D4
#define BCM4506_CONFIG_TURBO_SCAN_MODES      0x00D6
#define BCM4506_CONFIG_ACQ_TIME              0x00DE
#define BCM4506_CONFIG_DFT_RANGE_START       0x00E6
#define BCM4506_CONFIG_DFT_RANGE_END         0x00E8
#define BCM4506_CONFIG_CNR_THRESH1           0x00F2
#define BCM4506_CONFIG_LDPC_ALT_TRK_PLC_BW   0x00F6
#define BCM4506_CONFIG_DISEQC_BIT_THRESHOLD  0x00FE

#define BCM4506_CONFIG_LEN_VERSION                  1 
#define BCM4506_CONFIG_LEN_XTAL_FREQ                4
#define BCM4506_CONFIG_LEN_FLAGS                    1 
#define BCM4506_CONFIG_LEN_BCM3445_ADDRESS          1 
#define BCM4506_CONFIG_LEN_BCM3445_CTL              1
#define BCM4506_CONFIG_LEN_FSK_MODE                 1 
#define BCM4506_CONFIG_LEN_TUNER_CTL                1 
#define BCM4506_CONFIG_LEN_TUNER_CUTOFF             1 
#define BCM4506_CONFIG_LEN_XPORT_CTL                2 
#define BCM4506_CONFIG_LEN_LDPC_CTL                 1 
#define BCM4506_CONFIG_LEN_LDPC_ALT_ACQ_PLC_BW      4
#define BCM4506_CONFIG_LEN_LOCK_INDICATOR_PIN       1 
#define BCM4506_CONFIG_LEN_DISEQC_CTL1              1 
#define BCM4506_CONFIG_LEN_DISEQC_CTL2              1 
#define BCM4506_CONFIG_LEN_RRTO_USEC                4 
#define BCM4506_CONFIG_LEN_TONE_AMPLITUDE           1 
#define BCM4506_CONFIG_LEN_VBOT_AMPLITUDE           1 
#define BCM4506_CONFIG_LEN_VTOP_AMPLITUDE           1 
#define BCM4506_CONFIG_LEN_DISEQC_PRETX_DELAY       1 
#define BCM4506_CONFIG_LEN_TUNE_MIX_DELAY           1
#define BCM4506_CONFIG_LEN_TUNE_MIX_VCREF           1
#define BCM4506_CONFIG_LEN_TUNE_DAC_DIV_RATIO       1
#define BCM4506_CONFIG_LEN_AGC_CTL                  1
#define BCM4506_CONFIG_LEN_TUNE_DAC_QP2_CNT         1
#define BCM4506_CONFIG_LEN_TUNE_DAC_VCREF           1
#define BCM4506_CONFIG_LEN_FTM_CORR_CTL             4 
#define BCM4506_CONFIG_LEN_FTM_CORR_THRES           4 
#define BCM4506_CONFIG_LEN_FTM_CORR_PEAK_QUAL       4 
#define BCM4506_CONFIG_LEN_SEARCH_RANGE             4 
#define BCM4506_CONFIG_LEN_DVB_FINAL_STPLC_LBR      4 
#define BCM4506_CONFIG_LEN_DVB_FINAL_STPLC          4 
#define BCM4506_CONFIG_LEN_DTV_FINAL_STPLC          4 
#define BCM4506_CONFIG_LEN_DCII_FINAL_STPLC         4 
#define BCM4506_CONFIG_LEN_DTV_1_FINAL_STFLLC       4 
#define BCM4506_CONFIG_LEN_DTV_1_FINAL_STFLIC       4 
#define BCM4506_CONFIG_LEN_DTV_2_FINAL_STFLLC       4 
#define BCM4506_CONFIG_LEN_DTV_2_FINAL_STFLIC       4 
#define BCM4506_CONFIG_LEN_DVB_1_FINAL_STFLLC       4 
#define BCM4506_CONFIG_LEN_DVB_1_FINAL_STFLIC       4 
#define BCM4506_CONFIG_LEN_DVB_2_FINAL_STFLLC       4 
#define BCM4506_CONFIG_LEN_DVB_2_FINAL_STFLIC       4 
#define BCM4506_CONFIG_LEN_DCII_FINAL_STFLLC        4 
#define BCM4506_CONFIG_LEN_DCII_FINAL_STFLIC        4 
#define BCM4506_CONFIG_LEN_LDPC_SCRAMBLING_SEQ     16
#define BCM4506_CONFIG_LEN_FREQ_DRIFT_THRESHOLD     4 
#define BCM4506_CONFIG_LEN_RAIN_FADE_THRESHOLD      1 
#define BCM4506_CONFIG_LEN_RAIN_FADE_WINDOW         2 
#define BCM4506_CONFIG_LEN_ERROR_COUNT_WINDOW       2 
#define BCM4506_CONFIG_LEN_XPORT_CLOCK_ADJUST       1
#define BCM4506_CONFIG_LEN_RT_STATUS_INDICATORS     1 
#define BCM4506_CONFIG_LEN_DFT_SIZE                 1
#define BCM4506_CONFIG_LEN_TURBO_CTL                1
#define BCM4506_CONFIG_LEN_STUFF_BYTES              1
#define BCM4506_CONFIG_LEN_TIMING_LOOP_LOCK         1
#define BCM4506_CONFIG_LEN_LDPC_ALT_TRK_PLC_DAMP    1
#define BCM4506_CONFIG_LEN_DISEQC_TONE_THRESHOLD    1
#define BCM4506_CONFIG_LEN_LDPC_ALT_ACQ_PLC_DAMP    1
#define BCM4506_CONFIG_LEN_SPLITTER_MODE_CTL        1
#define BCM4506_CONFIG_LEN_LOCK_HISTORY             1
#define BCM4506_CONFIG_LEN_DISEQC_PRETX_VHOLD       1
#define BCM4506_CONFIG_LEN_DISEQC_POSTTX_VHOLD      1
#define BCM4506_CONFIG_LEN_BLIND_SCAN_MODES         1
#define BCM4506_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MIN   4
#define BCM4506_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MAX   4
#define BCM4506_CONFIG_LEN_NETWORK_SPEC             1
#define BCM4506_CONFIG_LEN_FTM_TX_POWER             1
#define BCM4506_CONFIG_LEN_FTM_CH_SELECT            1
#define BCM4506_CONFIG_LEN_DTV_SCAN_CODE_RATES      1
#define BCM4506_CONFIG_LEN_DVB_SCAN_CODE_RATES      1
#define BCM4506_CONFIG_LEN_TURBO_SCAN_MODES         2
#define BCM4506_CONFIG_LEN_ACQ_TIME                 4
#define BCM4506_CONFIG_LEN_DFT_RANGE_START          2
#define BCM4506_CONFIG_LEN_DFT_RANGE_END            2
#define BCM4506_CONFIG_LEN_CNR_THRESH1              2
#define BCM4506_CONFIG_LEN_LDPC_ALT_TRK_PLC_BW      4
#define BCM4506_CONFIG_LEN_DISEQC_BIT_THRESHOLD     2


/******************************************************************************
Summary:
   xport_ctl configuration parameter bit definitions
Description:
   These are bit definitions for xport_ctl configuration parameter.
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BCM4506_XPORT_CTL_BYPASS_OPLL 0x0001 /* 1=bypass OPLL */
#define BCM4506_XPORT_CTL_ERRINV      0x0080 /* PS_ERR: 0=active high, 1=active low */
#define BCM4506_XPORT_CTL_SYNCINV     0x0040 /* PS_SYNC: 0=active high, 1=active low */
#define BCM4506_XPORT_CTL_VLDINV      0x0020 /* PS_VALID: 0=active high, 1=active low */
#define BCM4506_XPORT_CTL_CLKSUP      0x0010 /* PS_CLK: 0=runs continuously, 1=suppress when PS_VALID not active */
#define BCM4506_XPORT_CTL_CLKINV      0x0008 /* PS_CLK: 0=active high, 1=active low */
#define BCM4506_XPORT_CTL_SERIAL      0x0002 /* output format: 0=parallel, 1=serial */
#define BCM4506_XPORT_CTL_XBERT       0x0100 /* 0=normal, 1=XBERT */
#define BCM4506_XPORT_CTL_TEI         0x0200 /* transport error indicator: 0=off, 1=on */
#define BCM4506_XPORT_CTL_DELAY       0x0400 /* 0=normal, 1=delay PS_ERR, PS_VALID, PS_DATA relative to PS_CLK */
#define BCM4506_XPORT_CTL_BCH_CHECK   0x0800
#define BCM4506_XPORT_CTL_CRC8_CHECK  0x1000
#define BCM4506_XPORT_CTL_SYNC1       0x2000 /* 0=no effect, 1=PS_SYNC valid for 1 bit in serial mode */
#define BCM4506_XPORT_CTL_HEAD4       0x4000 /* duration of PS_VALID/PS_SYNC: 0=1 byte, 1=4 bytes */
#define BCM4506_XPORT_CTL_DELH        0x8000 /* 0=no effect, 1=delete MPEG header */


/******************************************************************************
Summary:
   diseqc_ctl2 configuration parameter bit definitions
Description:
   These are bit definitions for diseqc_ctl2 configuration parameter.
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BCM4506_DISEQC_CTL2_ENVELOPE     0x01 /* 0=tone mode, 1=envelope mode */
#define BCM4506_DISEQC_CTL2_TONE_ALIGN   0x02 /* tone alignment: 0=off, 1=on */
#define BCM4506_DISEQC_CTL2_DISABLE_RRTO 0x04 /* 0=RRTO enabled, 1=RRTO disabled */
#define BCM4506_DISEQC_CTL2_TB_ENABLE    0x08 /* tone burst: 0=disabled, 1=enabled */
#define BCM4506_DISEQC_CTL2_TB_B                0x10 /* applies if tone burst is enabled: 0=tone A, 1=tone B */
#define BCM4506_DISEQC_CTL2_EXP_REPLY_DISABLE   0x20 /* 0=first byte bit 1 of command indicates reply bytes expected, 1=don't expect reply bytes (over-rides bit 7) */
#define BCM4506_DISEQC_CTL2_DISABLE_RX_ONLY     0x40 /* rx only mode (send null command), 0=enabled, 1=disabled */
#define BCM4506_DISEQC_CTL2_OVERRIDE_REPLY_BIT  0x80 /* 0=framing byte bit 1 determines reply, 1=override framing byte and always expect reply (if not disabled by bit 5) */


/******************************************************************************
Summary:
   BCM4506_CONFIG_BCM3445_CTL configuration parameter bit definitions
Description:
   BCM4506_CONFIG_BCM3445_CTL configuration parameter bit definitions
See Also:
   BAST_ConfigBcm3445(), BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BCM4506_BCM3445_CTL_LOW_POWER_MODE 0x01
#define BCM4506_BCM3445_CTL_AGC_TOP_LOW    0x00
#define BCM4506_BCM3445_CTL_AGC_TOP_MID    0x02
#define BCM4506_BCM3445_CTL_AGC_TOP_HI     0x04
#define BCM4506_BCM3445_CTL_AGC_TOP        0x06
#define BCM4506_BCM3445_CTL_4507_DISABLE_BBAND_FILTER_CFG 0x08
#define BCM4506_BCM3445_CTL_METER_MODE     0x10
#define BCM4506_BCM3445_CTL_DISABLE_NOTCH  0x20


/******************************************************************************
Summary:
   configuration parameter bit definitions for BCM4506_CONFIG_DVB_SCAN_CODE_RATES
   and BCM4506_CONFIG_DTV_SCAN_CODE_RATES
Description:
   bit definition for BCM4506_CONFIG_DVB_SCAN_CODE_RATES and 
   BCM4506_CONFIG_DTV_SCAN_CODE_RATES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4506_LEGACY_SCAN_1_2  0x01
#define BAST_4506_LEGACY_SCAN_2_3  0x02
#define BAST_4506_LEGACY_SCAN_3_4  0x04
#define BAST_4506_LEGACY_SCAN_5_6  0x08
#define BAST_4506_LEGACY_SCAN_6_7  0x10
#define BAST_4506_LEGACY_SCAN_7_8  0x20


/******************************************************************************
Summary:
   configuration parameter bit definitions for BCM4506_CONFIG_TURBO_SCAN_MODES
Description:
   bit definition for BCM4506_CONFIG_TURBO_SCAN_MODES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4506_TURBO_QPSK_SCAN_1_2  0x0001
#define BAST_4506_TURBO_QPSK_SCAN_2_3  0x0002
#define BAST_4506_TURBO_QPSK_SCAN_3_4  0x0004
#define BAST_4506_TURBO_QPSK_SCAN_5_6  0x0008
#define BAST_4506_TURBO_QPSK_SCAN_7_8  0x0010
#define BAST_4506_TURBO_8PSK_SCAN_2_3  0x0020
#define BAST_4506_TURBO_8PSK_SCAN_3_4  0x0040
#define BAST_4506_TURBO_8PSK_SCAN_4_5  0x0080
#define BAST_4506_TURBO_8PSK_SCAN_5_6  0x0100
#define BAST_4506_TURBO_8PSK_SCAN_8_9  0x0200
#define BAST_4506_TURBO_SCAN_MASK      0x03FF


/******************************************************************************
Summary:
   BCM4506_CONFIG_BLIND_SCAN_MODES configuration parameter bit definitions
Description:
   BCM4506_CONFIG_BLIND_SCAN_MODES configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4506_BLIND_SCAN_MODES_DVB    0x01
#define BAST_4506_BLIND_SCAN_MODES_TURBO  0x02
#define BAST_4506_BLIND_SCAN_MODES_LDPC   0x04
#define BAST_4506_BLIND_SCAN_MODES_DTV    0x08
#define BAST_4506_BLIND_SCAN_MODES_DCII   0x10
#define BAST_4506_BLIND_SCAN_MODES_MASK   0x1F


/******************************************************************************
Summary:
   FSK channel select configuration used by BCM4506_CONFIG_FTM_CH_SELECT
Description:
   This enum selects the FSK channel configuration within the chip's PHY core.
See Also:
   BAST_ResetFtm(), BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
typedef enum BAST_FskChannelConfig
{
   BAST_FskChannelConfig_eCh0Tx_Ch0Rx = 0,
   BAST_FskChannelConfig_eCh0Tx_Ch1Rx,
   BAST_FskChannelConfig_eCh1Tx_Ch0Rx, /* not supported */
   BAST_FskChannelConfig_eCh1Tx_Ch1Rx
} BAST_FskChannelConfig;


/***************************************************************************
Summary:
   This function returns the default settings for 4506 module.
Description:
   This function returns the default settings for 4506 module.
Returns:
   BERR_Code
See Also:
   BAST_Open()
****************************************************************************/
BERR_Code BAST_4506_GetDefaultSettings(
   BAST_Settings *pSettings   /* [out] Default settings */
);


/***************************************************************************
Summary:
   This function returns the default settings for 4506 channel device.
Description:
   This function returns the default settings for 4506 channel device.
Returns:
   BERR_Code
See Also:
   BAST_Open()
****************************************************************************/
BERR_Code BAST_4506_GetChannelDefaultSettings(
   BAST_Handle h, 
   uint32_t chnNo, 
   BAST_ChannelSettings *pChnDefSettings
);


BERR_Code BAST_4506_WriteHostRegister(BAST_Handle h, uint8_t address, uint8_t *data);
BERR_Code BAST_4506_ReadHostRegister(BAST_Handle h, uint8_t address, uint8_t *data);

#ifdef BAST_4506_INCLUDE_VCO_AVOIDANCE
BERR_Code BAST_4506_GetTunerActualVco(BAST_ChannelHandle h, uint32_t *pVco);
BERR_Code BAST_4506_GetTunerExpectedVco(BAST_ChannelHandle h, uint32_t freq, uint32_t *pVco, uint32_t *pDiv);
uint32_t BAST_4506_GetClosestXtalHarmonic(uint32_t vco);
BERR_Code BAST_4506_GetNumTuners(BAST_Handle h, uint8_t *nTuners);
#endif


#ifdef __cplusplus
}
#endif

#endif /* BAST_4506_H__ */

