/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4538.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/12/12 9:18a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/include/4538/bast_4538.h $
 * 
 * Hydra_Software_Devel/4   10/12/12 9:18a enavarro
 * SWSATFE-174: added BAST_4538_WriteGpo()
 * 
 * Hydra_Software_Devel/3   7/23/12 1:52p enavarro
 * SWSATFE-174: ongoing development
 * 
 * Hydra_Software_Devel/2   7/13/12 3:11p enavarro
 * SWSATFE-174: updated config params
 * 
 * Hydra_Software_Devel/1   7/11/12 4:36p enavarro
 * SWSATFE-174: initial check-in
 *
 ***************************************************************************/
#ifndef _BAST_4538_H__
#define _BAST_4538_H__               

#ifdef __cplusplus
extern "C" {
#endif


#include "bast.h"
#include "breg_i2c.h"


/******************************************************************************
Summary:
   configuration parameter addresses
Description:
   These are the configuration parameters that can be accessed via 
   BAST_ReadConfig() and BAST_WriteConfig().
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_NETWORK_SPEC              0x8000
#define BAST_4538_CONFIG_LEN_NETWORK_SPEC          1
#define BAST_4538_CONFIG_SEARCH_RANGE              0x8001
#define BAST_4538_CONFIG_LEN_SEARCH_RANGE          4
#define BAST_4538_CONFIG_XPORT_CTL                 0x8002
#define BAST_4538_CONFIG_LEN_XPORT_CTL             4
#define BAST_4538_CONFIG_MTSIF_CTL                 0x8003 /* MTSIF control */
#define BAST_4538_CONFIG_LEN_MTSIF_CTL             4  
#define BAST_4538_CONFIG_DEBUG_LEVEL               0x8004 /* firmware debug output control */   
#define BAST_4538_CONFIG_LEN_DEBUG_LEVEL           1
#define BAST_4538_CONFIG_DEBUG_MODULE              0x8005
#define BAST_4538_CONFIG_LEN_DEBUG_MODULE          1
#define BAST_4538_CONFIG_HAB_MAX_TIME              0x8006
#define BAST_4538_CONFIG_LEN_HAB_MAX_TIME          4
#define BAST_4538_CONFIG_HAB_MAX_TIME_CMD          0x8007
#define BAST_4538_CONFIG_LEN_HAB_MAX_TIME_CMD      1
#define BAST_4538_CONFIG_ADC_SELECT                0x8008
#define BAST_4538_CONFIG_LEN_ADC_SELECT            1
#define BAST_4538_CONFIG_XTAL_FREQ                 0x0000 /* READ ONLY: crystal freq in Hz */
#define BAST_4538_CONFIG_LEN_XTAL_FREQ             4        
#define BAST_4538_CONFIG_MISC_CTL                  0x0001 /* miscellaneous acquisition settings, see BAST_4538_MISC_CTL_* macros */
#define BAST_4538_CONFIG_LEN_MISC_CTL              1
#define BAST_4538_CONFIG_RAIN_FADE_THRESHOLD       0x0002 /* SNR drop threshold for the rain fade indicator status in units of 1/8 dB SNR */
#define BAST_4538_CONFIG_LEN_RAIN_FADE_THRESHOLD   1
#define BAST_4538_CONFIG_RAIN_FADE_WINDOW          0x0003 /* time window for rain fade indicator status in units of 100 msecs */
#define BAST_4538_CONFIG_LEN_RAIN_FADE_WINDOW      4
#define BAST_4538_CONFIG_FREQ_DRIFT_THRESHOLD      0x0004 /* carrier offset threshold in Hz for frequency drift indicator status */
#define BAST_4538_CONFIG_LEN_FREQ_DRIFT_THRESHOLD  4
#define BAST_4538_CONFIG_STUFF_BYTES               0x0005 /* Number of null bytes to insert in each frame */
#define BAST_4538_CONFIG_LEN_STUFF_BYTES           1
#define BAST_4538_CONFIG_ACQ_TIME                  0x0006 /* acquisition time in usecs */
#define BAST_4538_CONFIG_LEN_ACQ_TIME              4
#define BAST_4538_CONFIG_TUNER_CTL                 0x0008 /* tuner control settings, see BAST_4538_CONFIG_TUNER_CTL_* macros */
#define BAST_4538_CONFIG_LEN_TUNER_CTL             1
#define BAST_4538_CONFIG_REACQ_CTL                 0x0009 /* reacquisition options, see BAST_4538_CONFIG_REACQ_CTL_* macros */
#define BAST_4538_CONFIG_LEN_REACQ_CTL             1
#define BAST_4538_CONFIG_PLC_CTL                   0x000A /* PLC settings */
#define BAST_4538_CONFIG_LEN_PLC_CTL               1
#define BAST_4538_CONFIG_LDPC_CTL                  0x000B /* AFEC settings, see BAST_4538_CONFIG_LDPC_CTL_* macros */
#define BAST_4538_CONFIG_LEN_LDPC_CTL              1
#define BAST_4538_CONFIG_TURBO_CTL                 0x000C /* TFEC settings */
#define BAST_4538_CONFIG_LEN_TURBO_CTL             1
#define BAST_4538_CONFIG_PLC_ALT_ACQ_BW            0x000D /* Alternate acquisition PLC bandwidth in units of Hz.  This bandwidth applies when PLC_CTL[2]=1. */
#define BAST_4538_CONFIG_LEN_PLC_ALT_ACQ_BW        4
#define BAST_4538_CONFIG_PLC_ALT_ACQ_DAMP          0x000E /* Alternate acquisition PLC damping factor scaled 2^3 (e.g. damping factor of 1.0 is 0x08).  This damping factor only applies when PLC_CTL[2]=1. */
#define BAST_4538_CONFIG_LEN_PLC_ALT_ACQ_DAMP      1
#define BAST_4538_CONFIG_PLC_ALT_TRK_BW            0x000F /* Alternate tracking PLC bandwidth in units of Hz.  This bandwidth applies when PLC_CTL[3]=1. */
#define BAST_4538_CONFIG_LEN_PLC_ALT_TRK_BW        4
#define BAST_4538_CONFIG_PLC_ALT_TRK_DAMP          0x0010 /* Alternate tracking PLC damping factor scaled 2^3 (e.g. damping factor of 1.0 is 0x08).  This damping factor only applies when PLC_CTL[3]=1. */
#define BAST_4538_CONFIG_LEN_PLC_ALT_TRK_DAMP      1
#define BAST_4538_CONFIG_BLIND_SCAN_MODES          0x0011 /* Indicates which modes will be considered in the blind scan, see BAST_4538_CONFIG_BLIND_SCAN_MODES_* macros */
#define BAST_4538_CONFIG_LEN_BLIND_SCAN_MODES      1
#define BAST_4538_CONFIG_DTV_SCAN_CODE_RATES       0x0012 /* Selects the DTV code rates that are considered in the scan, see BAST_4538_CONFIG_DTV_SCAN_CODE_RATES_* macros */
#define BAST_4538_CONFIG_LEN_DTV_SCAN_CODE_RATES   1
#define BAST_4538_CONFIG_DVB_SCAN_CODE_RATES       0x0013 /* Selects the DVB-S code rates that are considered in the scan, see BAST_4538_CONFIG_DVB_SCAN_CODE_RATES_* macros */
#define BAST_4538_CONFIG_LEN_DVB_SCAN_CODE_RATES   1
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES      0x0014 /* Selects the DCII code rates that are considered in the scan, see BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_* macros */
#define BAST_4538_CONFIG_LEN_DCII_SCAN_CODE_RATES  1
#define BAST_4538_CONFIG_TURBO_SCAN_MODES          0x0015 /* Selects the Turbo modes that are considered in the scan, see BAST_4538_CONFIG_TURBO_SCAN_MODES_* macros */
#define BAST_4538_CONFIG_LEN_TURBO_SCAN_MODES      2
#define BAST_4538_CONFIG_LDPC_SCAN_MODES           0x0016 /* Selects the LDPC modes that are considered in the scan, see BAST_4538_CONFIG_LDPC_SCAN_MODES_* macros */
#define BAST_4538_CONFIG_LEN_LDPC_SCAN_MODES       2
#define BAST_4538_CONFIG_TUNER_FILTER_OVERRIDE     0x0017 /* channel bandwidth filter */
#define BAST_4538_CONFIG_LEN_TUNER_FILTER_OVERRIDE 1
#define BAST_4538_CONFIG_FTM_TX_POWER              0x0018
#define BAST_4538_CONFIG_LEN_FTM_TX_POWER          1
#define BAST_4538_CONFIG_FTM_CH_SELECT             0x0019
#define BAST_4538_CONFIG_LEN_FTM_CH_SELECT         1
#define BAST_4538_CONFIG_FREQ_ESTIMATE_STATUS      0x001A /* READ ONLY: status of DFT freq estimate */
#define BAST_4538_CONFIG_LEN_FREQ_ESTIMATE_STATUS  1
#define BAST_4538_CONFIG_IF_STEP_SAVE              0x001B /* coarse IF step determined by DFT freq estimate */
#define BAST_4538_CONFIG_LEN_IF_STEP_SAVE          4
#define BAST_4538_CONFIG_STATUS_INDICATOR          0x001C /* real time status indicators, see BAST_4538_CONFIG_STATUS_INDICATOR_* macros */
#define BAST_4538_CONFIG_LEN_STATUS_INDICATOR      1
#define BAST_4538_CONFIG_DFT_RANGE_START           0x001D /* Starting bin for DFT engine. */
#define BAST_4538_CONFIG_LEN_DFT_RANGE_START       2
#define BAST_4538_CONFIG_DFT_RANGE_END             0x001E /* Ending bin for DFT engine. */
#define BAST_4538_CONFIG_LEN_DFT_RANGE_END         2
#define BAST_4538_CONFIG_DEBUG1                    0x0023 /* debug 1 */
#define BAST_4538_CONFIG_LEN_DEBUG1                4
#define BAST_4538_CONFIG_DEBUG2                    0x0024 /* debug 2 */
#define BAST_4538_CONFIG_LEN_DEBUG2                4
#define BAST_4538_CONFIG_ACM_MAX_MODE              0x0025 /* highest BAST_Mode in ACM transmission */
#define BAST_4538_CONFIG_LEN_ACM_MAX_MODE          1
#define BAST_4538_CONFIG_REACQ_CAUSE               0x0026 /* cause of reacquisition */
#define BAST_4538_CONFIG_LEN_REACQ_CAUSE           4


/******************************************************************************
Summary:
   BAST_4538_CONFIG_MISC_CTL configuration parameter bit definitions
Description:
   BAST_4538_CONFIG_MISC_CTL configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_MISC_CTL_VERIFY_TIMING_LOOP 0x02 /* 1=verify timing loop lock */


/******************************************************************************
Summary:
   BAST_4538_CONFIG_TUNER_CTL configuration parameter bit definitions
Description:
   BAST_4538_CONFIG_TUNER_CTL configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_TUNER_CTL_SET_FILTER_MANUAL        0x08 /* 1=tuner LPF will be set to the value specified by TUNER_FILTER_OVERRIDE config parameter */
#define BAST_4538_CONFIG_TUNER_CTL_BYPASS_DFT_FREQ_EST      0x10 /* 1=bypass dft freq estimate */
#define BAST_4538_CONFIG_TUNER_CTL_PRESERVE_COMMANDED_FREQ  0x20 /* 1=tuner LO will be set to the commanded tuner freq */


/******************************************************************************
Summary:
   BAST_4538_CONFIG_BLIND_SCAN_MODES configuration parameter bit definitions
Description:
   BAST_4538_CONFIG_BLIND_SCAN_MODES configuration parameter bit definitions
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_BLIND_SCAN_MODES_DVB    0x01
#define BAST_4538_CONFIG_BLIND_SCAN_MODES_TURBO  0x02
#define BAST_4538_CONFIG_BLIND_SCAN_MODES_LDPC   0x04
#define BAST_4538_CONFIG_BLIND_SCAN_MODES_DTV    0x08
#define BAST_4538_CONFIG_BLIND_SCAN_MODES_DCII   0x10
#define BAST_4538_CONFIG_BLIND_SCAN_MODES_MASK   0x1F


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_DTV_SCAN_CODE_RATES
Description:
   bit definition for BAST_4538_CONFIG_DTV_SCAN_CODE_RATES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_DTV_SCAN_CODE_RATES_1_2  0x01 /* DTV 1/2 */
#define BAST_4538_CONFIG_DTV_SCAN_CODE_RATES_2_3  0x02 /* DTV 2/3 */
#define BAST_4538_CONFIG_DTV_SCAN_CODE_RATES_6_7  0x04 /* DTV 6/7 */
#define BAST_4538_CONFIG_DTV_SCAN_CODE_RATES_ALL  0x07 /* search all DTV modes */


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_DVB_SCAN_CODE_RATES
Description:
   bit definition for BAST_4538_CONFIG_DVB_SCAN_CODE_RATES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_DVB_SCAN_CODE_RATES_1_2  0x01 /* DVB 1/2 */
#define BAST_4538_CONFIG_DVB_SCAN_CODE_RATES_2_3  0x02 /* DVB 2/3 */
#define BAST_4538_CONFIG_DVB_SCAN_CODE_RATES_3_4  0x04 /* DVB 3/4 */
#define BAST_4538_CONFIG_DVB_SCAN_CODE_RATES_5_6  0x08 /* DVB 5/6 */
#define BAST_4538_CONFIG_DVB_SCAN_CODE_RATES_7_8  0x10 /* DVB 7/8 */
#define BAST_4538_CONFIG_DVB_SCAN_CODE_RATES_ALL  0x1F /* search all DVB-S modes */


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_DCII_SCAN_CODE_RATES
Description:
   bit definition for BAST_4538_CONFIG_DCII_SCAN_CODE_RATES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_1_2  0x01 /* DCII 1/2 */
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_2_3  0x02 /* DCII 2/3 */
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_3_4  0x04 /* DCII 3/4 */
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_5_6  0x08 /* DCII 5/6 */
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_7_8  0x10 /* DCII 7/8 */
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_5_11 0x20 /* DCII 5/11 */
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_3_5  0x40 /* DCII 3/5 */
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_4_5  0x80 /* DCII 4/5 */
#define BAST_4538_CONFIG_DCII_SCAN_CODE_RATES_ALL  0xFF /* search all DCII modes */


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_TURBO_SCAN_MODES
Description:
   bit definition for BAST_4538_CONFIG_TURBO_SCAN_MODES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_QPSK_1_2  0x0001
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_QPSK_2_3  0x0002
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_QPSK_3_4  0x0004
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_QPSK_5_6  0x0008
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_QPSK_7_8  0x0010
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_8PSK_2_3  0x0020
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_8PSK_3_4  0x0040
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_8PSK_4_5  0x0080
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_8PSK_5_6  0x0100
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_8PSK_8_9  0x0200
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_MASK      0x03FF
#define BAST_4538_CONFIG_TURBO_SCAN_MODES_ALL       0x03FF


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_LDPC_SCAN_MODES
Description:
   bit definition for BAST_4538_CONFIG_LDPC_SCAN_MODES
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_QPSK_1_2  0x0001
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_QPSK_3_5  0x0002
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_QPSK_2_3  0x0004
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_QPSK_3_4  0x0008
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_QPSK_4_5  0x0010
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_QPSK_5_6  0x0020
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_QPSK_8_9  0x0040
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_QPSK_9_10 0x0080
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_8PSK_3_5  0x0100
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_8PSK_2_3  0x0200
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_8PSK_3_4  0x0400
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_8PSK_5_6  0x0800
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_8PSK_8_9  0x1000
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_8PSK_9_10 0x2000
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_MASK      0x3FFF
#define BAST_4538_CONFIG_LDPC_SCAN_MODES_ALL       0x3FFF


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_STATUS_INDICATOR
Description:
   bit definition for BAST_4538_CONFIG_STATUS_INDICATOR
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_STATUS_INDICATOR_RAIN_FADE  0x01
#define BAST_4538_CONFIG_STATUS_INDICATOR_FREQ_DRIFT 0x02
#define BAST_4538_CONFIG_STATUS_INDICATOR_THRESHOLD2 0x04
#define BAST_4538_CONFIG_STATUS_INDICATOR_THRESHOLD1 0x08


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_PLC_CTL
Description:
   bit definition for BAST_4538_CONFIG_PLC_CTL
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_PLC_CTL_AWGN         0x02 /* 1=use PLC tracking bandwidth optimized for AWGN */
#define BAST_4538_CONFIG_PLC_CTL_OVERRIDE_ACQ 0x04 /* 1=acquisition PLC specified by PLC_ALT_ACQ_BW/PLC_ALT_ACQ_DAMP config parameters */
#define BAST_4538_CONFIG_PLC_CTL_OVERRIDE_TRK 0x08 /* 1=tracking PLC specified by PLC_ALT_TRK_BW/PLC_ALT_TRK_DAMP config parameters */


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_FREQ_ESTIMATE_STATUS
Description:
   bit definition for BAST_4538_CONFIG_FREQ_ESTIMATE_STATUS
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_FREQ_ESTIMATE_STATUS_START  0x01 /* DFT freq estimate started */
#define BAST_4538_CONFIG_FREQ_ESTIMATE_STATUS_COARSE 0x02 /* DFT freq estimate coarse search started */
#define BAST_4538_CONFIG_FREQ_ESTIMATE_STATUS_FINE   0x04 /* DFT freq estimate fine search started */
#define BAST_4538_CONFIG_FREQ_ESTIMATE_STATUS_OQPSK  0x08 /* DFT freq estimate for OQPSK */
#define BAST_4538_CONFIG_FREQ_ESTIMATE_STATUS_DONE   0x80 /* DFT freq estimate finished */


/******************************************************************************
Summary:
   configuration parameter bit definitions for BAST_4538_CONFIG_REACQ_CTL
Description:
   bit definition for BAST_4538_CONFIG_REACQ_CTL
See Also:
   BAST_ReadConfig(), BAST_WriteConfig()
******************************************************************************/
#define BAST_4538_CONFIG_REACQ_CTL_RETUNE       0x01 /* retune on reacquire */
#define BAST_4538_CONFIG_REACQ_CTL_FORCE        0x02 /* force reacquire now */
#define BAST_4538_CONFIG_REACQ_CTL_FORCE_RETUNE 0x04 /* force retune on next reacquire */
#define BAST_4538_CONFIG_REACQ_CTL_FREQ_DRIFT   0x08 /* dont keep lock if carrier freq drifts outside search_range */


/******************************************************************************
Summary:
   Structure containing AVS status
Description:
   This structure contains AVS status.
See Also:
   BAST_GetAvsStatus()
******************************************************************************/
typedef struct BAST_AvsStatus
{
   bool         bEnabled;
   int32_t      temperature;  /* temperature in 1/10 degrees Celsius */
   uint32_t     voltage_1p1_0; /* in mV */
} BAST_AvsStatus;


/***************************************************************************
Summary:
   This function returns the default settings for 4538 module.
Description:
   This function returns the default settings for 4538 module.
Returns:
   BERR_Code
See Also:
   BAST_Open()
****************************************************************************/
BERR_Code BAST_4538_GetDefaultSettings(
   BAST_Settings *pSettings   /* [out] Default settings */
);


/***************************************************************************
Summary:
   This function returns the default settings for 4538 channel device.
Description:
   This function returns the default settings for 4538 channel device.
Returns:
   BERR_Code
See Also:
   BAST_Open()
****************************************************************************/
BERR_Code BAST_4538_GetChannelDefaultSettings(
   BAST_Handle h, 
   uint32_t chnNo, 
   BAST_ChannelSettings *pChnDefSettings
);


/***************************************************************************
Summary:
   This function prints a message from the UART. 
Description:
   This function transmits the given null-terminated string to the UART at
   115200 8-N-1.
Returns:
   BERR_Code
See Also:
   none
****************************************************************************/
BERR_Code BAST_4538_PrintUart(BAST_Handle h, char *pBuf);


/***************************************************************************
Summary:
   This function enables/disables AVS.
Description:
   This function enables/disables AVS.
Returns:
   BERR_Code
See Also:
   none
****************************************************************************/
BERR_Code BAST_4538_EnableAvs(BAST_Handle h, bool bEnable);


/***************************************************************************
Summary:
   This function returns PVT status information.
Description:
   This function returns PVT status information.
Returns:
   BERR_Code
See Also:
   none
****************************************************************************/
BERR_Code BAST_4538_GetAvsStatus(BAST_Handle h, BAST_AvsStatus *pStatus);


/***************************************************************************
Summary:
   This function performs a MI2C write transaction using the BSC controller.
Description:
   This function performs a MI2C write transaction using the BSC controller.
Returns:
   BERR_Code
See Also:
   none
****************************************************************************/
BERR_Code BAST_4538_WriteBsc(
   BAST_Handle h,        /* [in] BAST device handle */
   uint8_t channel,      /* [in] bsc channel select */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *i2c_buf,     /* [in] specifies the data to transmit */
   uint8_t n             /* [in] number of bytes to transmit after the i2c slave address */
);


/***************************************************************************
Summary:
   This function performs a MI2C read transaction using the BSC controller.
Description:
   This function performs a MI2C read transaction using the BSC controller.
Returns:
   BERR_Code
See Also:
   none
****************************************************************************/
BERR_Code BAST_4538_ReadBsc(
   BAST_Handle h,        /* [in] BAST device handle */
   uint8_t channel,      /* [in] bsc channel select */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *out_buf,     /* [in] specifies the data to transmit before the i2c restart condition */
   uint8_t out_n,        /* [in] number of bytes to transmit before the i2c restart condition not including the i2c slave address */
   uint8_t *in_buf,      /* [out] holds the data read */
   uint8_t in_n          /* [in] number of bytes to read after the i2c restart condition not including the i2c slave address */
);


/***************************************************************************
Summary:
   This function sets the state of 1 or more GPO pins.
Description:
   This function sets the state of 1 or more GPO pins.
Returns:
   BERR_Code
See Also:
   none
****************************************************************************/
BERR_Code BAST_4538_WriteGpo(
   BAST_Handle h,        /* [in] BAST device handle */
   uint32_t mask,        /* [in] specifies which GPO pins to write */
   uint32_t data         /* [in] specifies the state of the GPO pins to be written */
);


#ifdef __cplusplus
}
#endif

#endif /* BAST_4538_H__ */

