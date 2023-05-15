/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2.h $
 * $brcm_Revision: Hydra_Software_Devel/38 $
 * $brcm_Date: 10/15/12 4:06p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/include/g2/bast_g2.h $
 * 
 * Hydra_Software_Devel/38   10/15/12 4:06p enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/37   9/6/12 6:19p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/36   5/13/11 9:56a enavarro
 * SWSATFE-91: added BAST_73XX_LDPC_CTL_DISABLE_FEC_REACQ bit
 * 
 * Hydra_Software_Devel/35   11/30/10 10:43a ckpark
 * SWSATFE-91: added reacqCtl to channel handle struct
 * 
 * Hydra_Software_Devel/34   7/1/10 9:39a ronchan
 * SWSATFE-65: add diseqc bit detection threshold parameter
 * 
 * Hydra_Software_Devel/33   9/1/09 4:35p enavarro
 * HW4506-60: added new configuration parameters for PLC override
 * 
 * Hydra_Software_Devel/32   8/27/09 5:00p enavarro
 * HW4506-78: add code for DFT frequency scan
 * 
 * Hydra_Software_Devel/31   7/1/09 7:11p ronchan
 * PR 56509: added new bit BCM73XX_DISEQC_CTL_LNBPU_TXEN to DISEQC_CTL
 * 
 * Hydra_Software_Devel/30   6/9/09 6:36p ronchan
 * PR 55876: added 3stage_agc_top config parameter
 * 
 * Hydra_Software_Devel/29   4/20/09 2:02p enavarro
 * PR 37769: added counters for all interrupts
 * 
 * Hydra_Software_Devel/28   3/2/09 5:28p ronchan
 * PR 52712: added network_spec config parameter
 * 
 * Hydra_Software_Devel/27   1/12/09 3:39p enavarro
 * PR 50993: added TURBO_CTL configuration parameter
 * 
 * Hydra_Software_Devel/26   12/22/08 2:31p ronchan
 * PR 50635: added configuration parameter for external tuner AGC
 * thresholds
 * 
 * Hydra_Software_Devel/25   10/31/08 3:04p enavarro
 * PR 37769: added CNR_THRESH1 config parameter
 * 
 * Hydra_Software_Devel/24   10/14/08 10:09a ronchan
 * PR 37769: modified bcm3445_ctl config parameter bit definitions
 * 
 * Hydra_Software_Devel/23   10/13/08 5:02p ronchan
 * PR 37769: added BCM3445_CTL config parameter and bit defs
 * 
 * Hydra_Software_Devel/22   10/1/08 3:50p enavarro
 * PR 37769: added STUFF_BYTES config parameter
 * 
 * Hydra_Software_Devel/21   9/22/08 2:43p ronchan
 * PR 37881: added fsk channel select enumeration
 * 
 * Hydra_Software_Devel/20   8/25/08 4:34p enavarro
 * PR 37769: added DFT_RANGE_START, DFT_RANGE_END, and DFT_SIZE config
 * parameters
 * 
 * Hydra_Software_Devel/18   8/15/08 3:38p ronchan
 * PR 37769: added diseqc_tone_threshold config parameter
 * 
 * Hydra_Software_Devel/17   7/21/08 4:50p enavarro
 * PR 37769: removed snr_threshold1 and snr_threshold2 config parameters
 * 
 * Hydra_Software_Devel/16   7/8/08 3:53p ronchan
 * PR 37881: added config parameters for tx power & channel select
 * 
 * Hydra_Software_Devel/15   6/20/08 11:09a enavarro
 * PR 37769: added rain_fade_window config parameter
 * 
 * Hydra_Software_Devel/14   6/19/08 2:31p enavarro
 * PR 37769: added support for status indicators
 * 
 * Hydra_Software_Devel/13   6/10/08 5:13p enavarro
 * PR 37769: added acq_time config parameter
 * 
 * Hydra_Software_Devel/12   6/9/08 2:22p enavarro
 * PR 37769: updated BAST_73XX_TURBO_SCAN_MASK definition
 * 
 * Hydra_Software_Devel/11   6/2/08 3:43p enavarro
 * PR 37769: reordered bits in blindScanModes config parameter
 * 
 * Hydra_Software_Devel/10   5/30/08 3:39p enavarro
 * PR 37769: renamed BCM73XX_CONFIG_BLIND_SCAN_SYM_RATE_MIN/MAX to
 * BCM73XX_CONFIG_PEAK_SCAN_SYM_RATE_MIN/MAX
 * 
 * Hydra_Software_Devel/9   5/15/08 4:27p enavarro
 * PR 37881: added VCO_SEPARATION config parameter for splitter mode
 * 
 * Hydra_Software_Devel/8   5/9/08 6:00p enavarro
 * PR 37769: added BAST_73XX_TUNER_CTL_DISABLE_FDDFS_SWITCH bin in
 * TUNER_CTL config parameter
 * 
 * Hydra_Software_Devel/7   5/6/08 4:07p enavarro
 * PR 37769: added BAST_73XX_TURBO_SCAN_MASK macro
 * 
 * Hydra_Software_Devel/6   5/6/08 10:24a enavarro
 * PR 37769: added dtvScanCodeRates, dvbScanCodeRates, and turboScanModes
 * config params
 * 
 * Hydra_Software_Devel/5   4/2/08 11:34a ronchan
 * PR 37881: added vsense threshold parameters
 * 
 * Hydra_Software_Devel/4   3/25/08 4:48p ronchan
 * PR 37769: fixed comment for diseqc_ctl parameter
 * 
 * Hydra_Software_Devel/3   3/12/08 5:25p enavarro
 * PR 37769: added REACQ_DELAY config parameter
 * 
 * Hydra_Software_Devel/2   12/19/07 11:31a enavarro
 * PR 37769: added bit definitions for BCM73XX_CONFIG_BLIND_SCAN_MODES
 * 
 * Hydra_Software_Devel/1   11/21/07 11:19a ronchan
 * PR 32395: initial version
 *
 ***************************************************************************/
#ifndef _BAST_G2_H__
#define _BAST_G2_H__               

#include "bast.h"
#include "breg_i2c.h"


#define BAST_G2_RELEASE_VERSION 1


/******************************************************************************
Summary:
   configuration parameter IDs
Description:
   These are the configuration parameters that can be accessed via 
   BAST_ReadConfig() and BAST_WriteConfig().
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_CONFIG_XTAL_FREQ               0x0000
#define BAST_G2_CONFIG_BCM3445_MI2C_CHANNEL    0x0001
#define BAST_G2_CONFIG_BCM3445_ADDRESS         0x0002
#define BAST_G2_CONFIG_EXT_TUNER_IF_OFFSET     0x0003
#define BAST_G2_CONFIG_TUNER_CTL               0x0004
#define BAST_G2_CONFIG_TUNER_CUTOFF            0x0005
#define BAST_G2_CONFIG_XPORT_CTL               0x0006
#define BAST_G2_CONFIG_LDPC_CTL                0x0007
#define BAST_G2_CONFIG_DISEQC_CTL              0x0008
#define BAST_G2_CONFIG_ACQ_TIME                0x0009
#define BAST_G2_CONFIG_RRTO_USEC               0x000A
#define BAST_G2_CONFIG_DISEQC_PRETX_DELAY      0x000B
#define BAST_G2_CONFIG_SEARCH_RANGE            0x000C
#define BAST_G2_CONFIG_LDPC_SCRAMBLING_SEQ     0x000D
#define BAST_G2_CONFIG_XPORT_CLOCK_ADJUST      0x000E
#define BAST_G2_CONFIG_BLIND_SCAN_MODES        0x000F
#define BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MIN  0x0010
#define BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MAX  0x0011
#define BAST_G2_CONFIG_REACQ_DELAY             0x0012
#define BAST_G2_CONFIG_VSENSE_THRES_HI         0x0013
#define BAST_G2_CONFIG_VSENSE_THRES_LO         0x0014
#define BAST_G2_CONFIG_DTV_SCAN_CODE_RATES     0x0015
#define BAST_G2_CONFIG_DVB_SCAN_CODE_RATES     0x0016
#define BAST_G2_CONFIG_TURBO_SCAN_MODES        0x0017
#define BAST_G2_CONFIG_VCO_SEPARATION          0x0018
#define BAST_G2_CONFIG_RT_STATUS_INDICATORS    0x0019
#define BAST_G2_CONFIG_FREQ_DRIFT_THRESHOLD    0x001A
#define BAST_G2_CONFIG_RAIN_FADE_THRESHOLD     0x001B
#define BAST_G2_CONFIG_REACQ_CTL               0x001C
#define BAST_G2_CONFIG_RAIN_FADE_WINDOW        0x001E
#define BAST_G2_CONFIG_FTM_TX_POWER            0x001F
#define BAST_G2_CONFIG_FTM_CH_SELECT           0x0020
#define BAST_G2_CONFIG_DISEQC_TONE_THRESHOLD   0x0021
#define BAST_G2_CONFIG_DFT_RANGE_START         0x0022
#define BAST_G2_CONFIG_DFT_RANGE_END           0x0023
#define BAST_G2_CONFIG_DFT_SIZE                0x0024
#define BAST_G2_CONFIG_STUFF_BYTES             0x0025
#define BAST_G2_CONFIG_BCM3445_CTL             0x0026
#define BAST_G2_CONFIG_CNR_THRESH1             0x0027
#define BAST_G2_CONFIG_EXT_TUNER_RF_AGC_TOP    0x0028
#define BAST_G2_CONFIG_EXT_TUNER_IF_AGC_TOP    0x0029
#define BAST_G2_CONFIG_TURBO_CTL               0x002A
#define BAST_G2_CONFIG_NETWORK_SPEC            0x002B
#define BAST_G2_CONFIG_3STAGE_AGC_TOP          0x002C
#define BAST_G2_CONFIG_DFT_FREQ_ESTIMATE       0x002D
#define BAST_G2_CONFIG_DFT_FREQ_ESTIMATE_STATUS 0x002E
#define BAST_G2_CONFIG_ALT_PLC_ACQ_BW          0x002F
#define BAST_G2_CONFIG_ALT_PLC_ACQ_DAMP        0x0030
#define BAST_G2_CONFIG_ALT_PLC_TRK_BW          0x0031
#define BAST_G2_CONFIG_ALT_PLC_TRK_DAMP        0x0032
#define BAST_G2_CONFIG_DISEQC_BIT_THRESHOLD    0x0033

/* put config param lengths here... */
#define BAST_G2_CONFIG_LEN_XTAL_FREQ               4
#define BAST_G2_CONFIG_LEN_BCM3445_MI2C_CHANNEL    1
#define BAST_G2_CONFIG_LEN_BCM3445_ADDRESS         1
#define BAST_G2_CONFIG_LEN_EXT_TUNER_IF_OFFSET     4
#define BAST_G2_CONFIG_LEN_TUNER_CTL               1
#define BAST_G2_CONFIG_LEN_TUNER_CUTOFF            1
#define BAST_G2_CONFIG_LEN_XPORT_CTL               2
#define BAST_G2_CONFIG_LEN_LDPC_CTL                1
#define BAST_G2_CONFIG_LEN_DISEQC_CTL              2
#define BAST_G2_CONFIG_LEN_ACQ_TIME                4
#define BAST_G2_CONFIG_LEN_RRTO_USEC               4
#define BAST_G2_CONFIG_LEN_DISEQC_PRETX_DELAY      1
#define BAST_G2_CONFIG_LEN_SEARCH_RANGE            4
#define BAST_G2_CONFIG_LEN_LDPC_SCRAMBLING_SEQ     16
#define BAST_G2_CONFIG_LEN_XPORT_CLOCK_ADJUST      1
#define BAST_G2_CONFIG_LEN_BLIND_SCAN_MODES        1
#define BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MIN  4
#define BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MAX  4
#define BAST_G2_CONFIG_LEN_REACQ_DELAY             4
#define BAST_G2_CONFIG_LEN_VSENSE_THRES_HI         1
#define BAST_G2_CONFIG_LEN_VSENSE_THRES_LO         1
#define BAST_G2_CONFIG_LEN_DTV_SCAN_CODE_RATES     1
#define BAST_G2_CONFIG_LEN_DVB_SCAN_CODE_RATES     1
#define BAST_G2_CONFIG_LEN_TURBO_SCAN_MODES        2
#define BAST_G2_CONFIG_LEN_VCO_SEPARATION          4
#define BAST_G2_CONFIG_LEN_RT_STATUS_INDICATORS    1
#define BAST_G2_CONFIG_LEN_FREQ_DRIFT_THRESHOLD    4
#define BAST_G2_CONFIG_LEN_RAIN_FADE_THRESHOLD     4
#define BAST_G2_CONFIG_LEN_REACQ_CTL               1
#define BAST_G2_CONFIG_LEN_RAIN_FADE_WINDOW        4
#define BAST_G2_CONFIG_LEN_FTM_TX_POWER            1
#define BAST_G2_CONFIG_LEN_FTM_CH_SELECT           1
#define BAST_G2_CONFIG_LEN_DISEQC_TONE_THRESHOLD   1
#define BAST_G2_CONFIG_LEN_DFT_RANGE_START         2
#define BAST_G2_CONFIG_LEN_DFT_RANGE_END           2
#define BAST_G2_CONFIG_LEN_DFT_SIZE                1
#define BAST_G2_CONFIG_LEN_STUFF_BYTES             1
#define BAST_G2_CONFIG_LEN_BCM3445_CTL             1
#define BAST_G2_CONFIG_LEN_CNR_THRESH1             2
#define BAST_G2_CONFIG_LEN_EXT_TUNER_RF_AGC_TOP    2
#define BAST_G2_CONFIG_LEN_EXT_TUNER_IF_AGC_TOP    2
#define BAST_G2_CONFIG_LEN_TURBO_CTL               1
#define BAST_G2_CONFIG_LEN_NETWORK_SPEC            1
#define BAST_G2_CONFIG_LEN_3STAGE_AGC_TOP          2
#define BAST_G2_CONFIG_LEN_DFT_FREQ_ESTIMATE       4
#define BAST_G2_CONFIG_LEN_DFT_FREQ_ESTIMATE_STATUS 1
#define BAST_G2_CONFIG_LEN_ALT_PLC_ACQ_BW          4
#define BAST_G2_CONFIG_LEN_ALT_PLC_ACQ_DAMP        1
#define BAST_G2_CONFIG_LEN_ALT_PLC_TRK_BW          4
#define BAST_G2_CONFIG_LEN_ALT_PLC_TRK_DAMP        1
#define BAST_G2_CONFIG_LEN_DISEQC_BIT_THRESHOLD    2


/******************************************************************************
Summary:
   BAST_G2_CONFIG_DISEQC_CTL configuration parameter bit definitions
Description:
   BAST_G2_CONFIG_DISEQC_CTL configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_DISEQC_CTL_ENVELOPE             0x0001 /* 0=tone mode, 1=envelope mode on TXOUT pin */
#define BAST_G2_DISEQC_CTL_TONE_ALIGN_ENABLE    0x0002 /* tone alignment: 0=off, 1=on */
#define BAST_G2_DISEQC_CTL_RRTO_DISABLE         0x0004 /* 0=RRTO enabled, 1=RRTO disabled */
#define BAST_G2_DISEQC_CTL_TONEBURST_ENABLE     0x0008 /* tone burst: 0=disabled, 1=enabled */
#define BAST_G2_DISEQC_CTL_TONEBURST_B          0x0010 /* applies if tone burst is enabled: 0=tone A, 1=tone B */
#define BAST_G2_DISEQC_CTL_EXP_REPLY_DISABLE    0x0020 /* 0=first byte bit 1 of command indicates reply bytes expected, 1=don't expect reply bytes */
#define BAST_G2_DISEQC_CTL_LNBPU_TXEN           0x0040 /* 0=LNBPU not used , 1= LNBPU on TXEN pin */
#define BAST_G2_DISEQC_CTL_IMPLEMENTED          0x007F


/******************************************************************************
Summary:
   FSK channel select configuration
Description:
   This enum selects the FSK channel configuration within the chip's PHY core.
See Also:
   BAST_ResetFtm(),  BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
typedef enum BAST_FskChannelConfig
{
   BAST_FskChannelConfig_eCh0Tx_Ch0Rx = 0,
   BAST_FskChannelConfig_eCh0Tx_Ch1Rx,
   BAST_FskChannelConfig_eCh1Tx_Ch0Rx,
   BAST_FskChannelConfig_eCh1Tx_Ch1Rx
} BAST_FskChannelConfig;


/******************************************************************************
Summary:
   BAST_G2_CONFIG_XPORT_CTL configuration parameter bit definitions
Description:
   BAST_G2_CONFIG_XPORT_CTL configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_XPORT_CTL_0          0x0001
#define BAST_G2_XPORT_CTL_SERIAL     0x0002
#define BAST_G2_XPORT_CTL_2          0x0004
#define BAST_G2_XPORT_CTL_CLKINV     0x0008
#define BAST_G2_XPORT_CTL_CLKSUP     0x0010
#define BAST_G2_XPORT_CTL_VLDINV     0x0020
#define BAST_G2_XPORT_CTL_SYNCINV    0x0040
#define BAST_G2_XPORT_CTL_ERRINV     0x0080
#define BAST_G2_XPORT_CTL_XBERT      0x0100
#define BAST_G2_XPORT_CTL_TEI        0x0200
#define BAST_G2_XPORT_CTL_DELAY      0x0400
#define BAST_G2_XPORT_CTL_BCH_CHECK  0x0800
#define BAST_G2_XPORT_CTL_CRC8_CHECK 0x1000
#define BAST_G2_XPORT_CTL_SYNC1      0x2000
#define BAST_G2_XPORT_CTL_HEAD4      0x4000
#define BAST_G2_XPORT_CTL_DELH       0x8000


/******************************************************************************
Summary:
   BAST_G2_CONFIG_TUNER_CTL configuration parameter bit definitions
Description:
   BAST_G2_CONFIG_TUNER_CTL configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_TUNER_CTL_DISABLE_RETRIES          0x01
#define BAST_G2_TUNER_CTL_DISABLE_LO_LOOP_TRACKING 0x02
#define BAST_G2_TUNER_CTL_DISABLE_FS_SMART_TUNING  0x04
#define BAST_G2_TUNER_CTL_SET_FILTER_MANUAL        0x08
#define BAST_G2_TUNER_CTL_DISABLE_LO_DIV_2         0x10
#define BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE     0x20
#define BAST_G2_TUNER_CTL_DISABLE_RETUNE           0x40
#define BAST_G2_TUNER_CTL_DISABLE_FDDFS_SWITCHING  0x80


/******************************************************************************
Summary:
   BAST_G2_CONFIG_LDPC_CTL configuration parameter bit definitions
Description:
   BAST_G2_CONFIG_LDPC_CTL configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_LDPC_CTL_DISABLE_AUTO_PILOT_PLL    0x01
#define BAST_G2_LDPC_CTL_AWGN_PLC                  0x02
#define BAST_G2_LDPC_CTL_USE_ALT_ACQ_PLC           0x04
#define BAST_G2_LDPC_CTL_USE_ALT_TRK_PLC           0x08
#define BAST_G2_LDPC_CTL_DISABLE_FEC_REACQ         0x10


/******************************************************************************
Summary:
   BAST_G2_CONFIG_BCM3445_CTL configuration parameter bit definitions
Description:
   BAST_G2_CONFIG_BCM3445_CTL configuration parameter bit definitions
See Also:
   BAST_ConfigBcm3445(), BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_BCM3445_CTL_LOW_POWER_MODE 0x01
#define BAST_G2_BCM3445_CTL_AGC_TOP_LOW    0x00
#define BAST_G2_BCM3445_CTL_AGC_TOP_MID    0x02
#define BAST_G2_BCM3445_CTL_AGC_TOP_HI     0x04
#define BAST_G2_BCM3445_CTL_AGC_TOP        0x06


/******************************************************************************
Summary:
   BAST_G2_CONFIG_BLIND_SCAN_MODES configuration parameter bit definitions
Description:
   BAST_G2_CONFIG_BLIND_SCAN_MODES configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_BLIND_SCAN_MODES_DVB    0x01
#define BAST_G2_BLIND_SCAN_MODES_TURBO  0x02
#define BAST_G2_BLIND_SCAN_MODES_LDPC   0x04
#define BAST_G2_BLIND_SCAN_MODES_DTV    0x08
#define BAST_G2_BLIND_SCAN_MODES_DCII   0x10
#define BAST_G2_BLIND_SCAN_MODES_MASK   0x1F


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_G2_CONFIG_DVB_SCAN_CODE_RATES
   and BAST_G2_CONFIG_DTV_SCAN_CODE_RATES
Description:
   bit definition for BAST_G2_CONFIG_DVB_SCAN_CODE_RATES and 
   BAST_G2_CONFIG_DTV_SCAN_CODE_RATES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_LEGACY_SCAN_1_2  0x01
#define BAST_G2_LEGACY_SCAN_2_3  0x02
#define BAST_G2_LEGACY_SCAN_3_4  0x04
#define BAST_G2_LEGACY_SCAN_5_6  0x08
#define BAST_G2_LEGACY_SCAN_6_7  0x10
#define BAST_G2_LEGACY_SCAN_7_8  0x20


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_G2_CONFIG_TURBO_SCAN_MODES
Description:
   bit definition for BAST_G2_CONFIG_TURBO_SCAN_MODES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_TURBO_QPSK_SCAN_1_2  0x0001
#define BAST_G2_TURBO_QPSK_SCAN_2_3  0x0002
#define BAST_G2_TURBO_QPSK_SCAN_3_4  0x0004
#define BAST_G2_TURBO_QPSK_SCAN_5_6  0x0008
#define BAST_G2_TURBO_QPSK_SCAN_7_8  0x0010
#define BAST_G2_TURBO_8PSK_SCAN_2_3  0x0020
#define BAST_G2_TURBO_8PSK_SCAN_3_4  0x0040
#define BAST_G2_TURBO_8PSK_SCAN_4_5  0x0080
#define BAST_G2_TURBO_8PSK_SCAN_5_6  0x0100
#define BAST_G2_TURBO_8PSK_SCAN_8_9  0x0200
#define BAST_G2_TURBO_SCAN_MASK      0x03FF


/******************************************************************************
Summary:
   real time status indicator bits
Description:
   bit definition for real time status indicators
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_STATUS_EVENT_FREQ_DRIFT 0x01
#define BAST_STATUS_EVENT_RAIN_FADE  0x02
#define BAST_STATUS_EVENT_THRESHOLD1 0x04
#define BAST_STATUS_EVENT_THRESHOLD2 0x08


/******************************************************************************
Summary:
   DFT size used in tone detection
Description:
   valid values for BAST_G2_CONFIG_DFT_SIZE configuration parameter
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_DFT_SIZE_256  0x00  /* 256 pt */
#define BAST_G2_DFT_SIZE_512  0x01  /* 512 pt */
#define BAST_G2_DFT_SIZE_1024 0x02  /* 1024 pt */
#define BAST_G2_DFT_SIZE_2048 0x03  /* 2048 pt */


/******************************************************************************
Summary:
   Turbo acquisition options
Description:
   bit definition for BAST_G2_CONFIG_TURBO_CTL
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_TURBO_CTL_DISABLE_FEC_REACQ  0x01  /* dont reacquire on FEC lost lock */


/******************************************************************************
Summary:
   reacquisition control options
Description:
   bit definition for BAST_G2_CONFIG_REACQ_CTL
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_G2_REACQ_CTL_FREQ_DRIFT 0x01  /* don't lock if carrier drifts beyond search_range */


/***************************************************************************
Summary:
   This function returns the default settings for g2 module.
Description:
   This function returns the default settings for g2 module.
Returns:
   BERR_Code
See Also:
   BAST_Open()
****************************************************************************/
BERR_Code BAST_g2_GetDefaultSettings(
   BAST_Settings *pSettings   /* [out] Default settings */
);


/***************************************************************************
Summary:
   This function returns the default settings for g2 channel device.
Description:
   This function returns the default settings for g2 channel device.
Returns:
   BERR_Code
See Also:
   BAST_Open()
****************************************************************************/
BERR_Code BAST_g2_GetChannelDefaultSettings(
   BAST_Handle h, 
   uint32_t chnNo, 
   BAST_ChannelSettings *pChnDefSettings
);


/* sds channel interrupt enumeration */
typedef enum BAST_Sds_IntID{
   BAST_Sds_IntID_eLock = 0,
   BAST_Sds_IntID_eNotLock,
   BAST_Sds_IntID_eBaudTimer,
   BAST_Sds_IntID_eBerTimer,
   BAST_Sds_IntID_eGenTimer1,
   BAST_Sds_IntID_eGenTimer2,
   BAST_Sds_IntID_eXtalTimer,
   BAST_Sds_IntID_eIfAgcLeThreshRise,
   BAST_Sds_IntID_eIfAgcLeThreshFall,
   BAST_Sds_IntID_eRfAgcLeThreshRise,
   BAST_Sds_IntID_eRfAgcLeThreshFall,
   BAST_Sds_IntID_eRfAgcGtMaxChange,
   BAST_Sds_IntID_eDiseqcVoltageGtHiThresh,
   BAST_Sds_IntID_eDiseqcVoltageLtLoThresh,
   BAST_Sds_IntID_eDiseqcDone,
   BAST_Sds_IntID_eLdpcHp,
   BAST_Sds_IntID_eMi2c,
   BAST_Sds_IntID_eTurboLock,
   BAST_Sds_IntID_eTurboNotLock,
   BAST_Sds_IntID_eAfecLock,
   BAST_Sds_IntID_eAfecNotLock,
   BAST_Sds_MaxIntID
} BAST_Sds_IntID;


/***************************************************************************
Summary:
   This function resets all interrupt counters.
Description:
   This function resets all interrupt counters.
Returns:
   BERR_Code
See Also:
   BAST_Open()
****************************************************************************/
void BAST_g2_ResetInterruptCounters(
   BAST_ChannelHandle h   /* [in] BAST channel handle */
);


/***************************************************************************
Summary:
   This function returns the current interrupt counter for the specified interrupt.
Description:
   This function returns the current interrupt counter for the specified interrupt.
Returns:
   BERR_Code
See Also:
   BAST_Open()
****************************************************************************/
BERR_Code BAST_g2_GetInterruptCount(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   BAST_Sds_IntID     idx, /* [in] specifies which interrupt count to return */
   uint32_t           *pCount
);

#ifdef __cplusplus
}
#endif

#endif /* BAST_G2_H__ */

