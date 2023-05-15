/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_7550ib.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 11:11a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/include/7550ib/btnr_7550ib.h $
 * 
 * Hydra_Software_Devel/1   9/7/12 11:11a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/1   9/7/12 10:15a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/21   12/6/10 10:19a farshidf
 * SW7550-628: ping's chnages
 * 
 * Hydra_Software_Devel/SW7550-628/1   12/3/10 4:50p pinglong
 * SW7550-628: AGC sometimes is out of range after tuning to an ISDB-T
 * channel
 * 
 * Hydra_Software_Devel/20   10/15/10 11:22a farshidf
 * SW7550-592: add the Enable DPM
 * 
 * Hydra_Software_Devel/SW7550-592/2   10/14/10 1:59p farshidf
 * SW7550-592: change the name
 * 
 * Hydra_Software_Devel/SW7550-592/1   10/12/10 6:09p farshidf
 * SW7550-592: create a seperate call for DPM
 * 
 * Hydra_Software_Devel/18   9/29/10 10:42a farshidf
 * SW7550-587: merge to main
 * 
 * Hydra_Software_Devel/SW7550-587/1   9/29/10 6:20p pinglong
 * SW7550-587: support setting LNA Boost/Tilt on and off
 * 
 * Hydra_Software_Devel/17   5/19/10 5:59p farshidf
 * SW7550-400: add the ofdm signal strength and dpm power level
 * 
 * Hydra_Software_Devel/16   5/18/10 12:01p farshidf
 * SW7550-440: merge to main branch
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/SW7550-440/1   5/18/10 11:47a farshidf
 * SW7550-440: add the dpm callback
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/3   5/14/10 4:40p farshidf
 * SW7550-440: add the installIsrcallback
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/2   4/29/10 6:51p farshidf
 * SW7550-38: add the new cable mode for tuner
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/1   4/29/10 5:16p cbrooks
 * Added miniTune
 * 
 * Hydra_Software_Devel/15   4/27/10 1:37p farshidf
 * SW7550-38: merge from branch 9 of TNR
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/3   4/24/10 1:38p cbrooks
 * Added support for MiniTune and Tuner_AGC_Status
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/2   4/23/10 5:17p cbrooks
 * Cleaned up lots of tuner code
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/1   2/25/10 12:03p farshidf
 * SW7550-38: add the get RFbypass
 * 
 * Hydra_Software_Devel/12   1/11/10 4:26p farshidf
 * SW7550-38: upodate the Tuner get status
 * 
 * Hydra_Software_Devel/11   1/8/10 5:09p farshidf
 * SW7550-38: change unint32 to int32
 * 
 * Hydra_Software_Devel/10   1/8/10 5:03p farshidf
 * SW7550-38: add the BTNR_7550_GetTunerStatus to ge the tuner Rfagcvalue
 * 
 * Hydra_Software_Devel/9   1/6/10 10:44a farshidf
 * SW7550-38: change the function name from BTNR_7550_RFByPassMode to
 * BTNR_7550_SetRFByPassMode
 * 
 * Hydra_Software_Devel/8   1/5/10 12:47p farshidf
 * SW7550-38: add the RF bypass and daisy chain
 * 
 * Hydra_Software_Devel/7   12/28/09 2:46p farshidf
 * SW7550-38: changes for BBS through Nexus
 * 
 * Hydra_Software_Devel/6   12/22/09 2:07p farshidf
 * SW7550-38: add hack code to enable the Tuner status in Nexus
 * 
 * Hydra_Software_Devel/5   11/17/09 8:15p farshidf
 * SW7550-38: update the return
 * 
 * Hydra_Software_Devel/4   11/17/09 8:01p farshidf
 * SW7550-38: update the file with latest nexus code
 * 
 * Hydra_Software_Devel/2   11/12/09 6:57p farshidf
 * SW7550-40: update the tuner files
 * 
 * Hydra_Software_Devel/1   11/5/09 10:23a farshidf
 * SW7550-38: add the 7550 files
 * 
 ***************************************************************************/
#ifndef BTNR_7550IB_H__
#define BTNR_7550IB_H__

#include "bchp.h"
#include "breg_i2c.h"
#include "btnr.h"
#include "btmr.h"
#include "bmem.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	BREG_I2C_Handle hI2cTHD;
	BREG_I2C_Handle hI2cADS;
    unsigned short i2cAddr;             /* 7bit I2C address of Bcm7550 */
	BTMR_Handle hTmr;
	BMEM_Heap_Handle hHeap;
} BTNR_7550_Settings;

/***************************************************************************
Summary:
    This function returns the default settings for Bcm7550 Tuner module.

Description:
    This function is responsible for returns the default setting for 
    BTNR module. The returning default setting should be when
    opening the device.

Returns:
    TODO:

See Also:
    BTNR_7550_Open()

****************************************************************************/
BERR_Code BTNR_7550_GetDefaultSettings(
    BTNR_7550_Settings *pDefSettings  /* [out] Returns default setting */
    );


/***************************************************************************
Summary:
    This function opens Bcm7550 Tuner module.

Description:
    This function is responsible for opening Bcm7550 BTNR module. When BTNR is
    opened, it will create a module handle and configure the module based
    on the default settings. Once the device is opened, it must be closed
    before it can be opened again.

Returns:
    TODO:

See Also:
    BTNR_Close(), BTNR_7550_GetDefaultSettings()

****************************************************************************/
BERR_Code BTNR_7550_Open(
    BTNR_Handle *phDev,                 /* [out] Returns handle */
	BREG_Handle hRegister,					/* Register handle */
    BTNR_7550_Settings *pSettings
    );

/***************************************************************************
Summary:
	7550 Tuner Functions Options 
****************************************************************************/
typedef enum BTNR_7550_TunerAcquisitionType
{
  BTNR_7550_TunerAcquisitionType_eInitTune=1,
  BTNR_7550_TunerAcquisitionType_eTune=2,
  BTNR_7550_TunerAcquisitionType_eMiniTune=3
} BTNR_7550_TunerAcquisitionType;

/***************************************************************************
Summary:
	7550 Tuner Bandwidth Options 
****************************************************************************/
typedef enum BTNR_7550_Bandwidth
{
  BTNR_7550_Bandwidth_e8MHz=1,
  BTNR_7550_Bandwidth_e7MHz=2,
  BTNR_7550_Bandwidth_e6MHz=3,
  BTNR_7550_Bandwidth_e5MHz=4
} BTNR_7550_Bandwidth;

/***************************************************************************
Summary:
	7550 Tuner Standard Options 
****************************************************************************/
typedef enum BTNR_7550_Standard
{
  BTNR_7550_Standard_eDVBT=1,
  BTNR_7550_Standard_eISDBT=2,
  BTNR_7550_Standard_eQAM=3
} BTNR_7550_Standard;

/***************************************************************************
Summary:
	7550 Tuner Setup Options 
****************************************************************************/
typedef enum BTNR_7550_Setup
{
  BTNR_7550_Setup_eCable=1,
  BTNR_7550_Setup_eTerrestrial=2
} BTNR_7550_Setup;

/***************************************************************************
Summary:
	7550 Tuner Tune Settings 
****************************************************************************/
typedef struct BTNR_7550_TuneSettings
{
	BTNR_7550_Bandwidth   Bandwidth;
	BTNR_7550_Standard    AcquisitionMode;
	uint32_t			  Freq;
	BTNR_7550_Setup       TunerSetup;
} BTNR_7550_TuneSettings;

/***************************************************************************
Summary:
	Get Default 7550 Tuner Tune Settings 
****************************************************************************/
BERR_Code BTNR_7550_GetDefaultTuneSettings(
    BTNR_7550_TuneSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Tune the 7550 tuner.
 
Description:
    This function should be used instead of
    BTNR_SetRfFrequency() when Bandwidth and AcquisitionModes are
    different from the default settings (8MHz DVB-T).
 
See Also: 
    BTNR_7550_Tune
****************************************************************************/
BERR_Code BTNR_7550_Tune(
    BTNR_Handle hTnr,
    const BTNR_7550_TuneSettings *pSettings
    );

/***************************************************************************
Summary:
    Process Interrupt Event.
 
Description:

See Also: 
    BTNR_7550_Tune
****************************************************************************/

BERR_Code BTNR_7550_ProcessInterruptEvent(BTNR_Handle h);


/***************************************************************************
Summary:
    Get Interrupt Event Handle
 
Description:

See Also: 
    BTNR_7550_Tune
****************************************************************************/
BERR_Code BTNR_7550_GetInterruptEventHandle(
	BTNR_Handle h, 
	BKNI_EventHandle* hEvent);

/***************************************************************************
Summary:
    Enable/Disable RF by pass mode
 
Description: Enable/Disable RF by pass mode
mode = True means enabled the RF by pass
mode = false means diable RF by pass

See Also: 
    BTNR_7550_RFByPassMode
****************************************************************************/
BERR_Code BTNR_7550_SetRFByPassMode(
    BTNR_Handle hTnr,
    bool mode);



/***************************************************************************
Summary:
    This function opens tells if the 75xx device's RF bypass is enabled/disabled.
 
Description: Enable/Disable RF by pass mode
mode = True means enabled the RF by pass
mode = false means diable RF by pass

See Also: 
    BTNR_7550_RFByPassMode
****************************************************************************/
BERR_Code BTNR_7550_GetRFByPassMode(
    BTNR_Handle hTnr,
    bool *mode);

/***************************************************************************
Summary:
    7550 Tuner Status
****************************************************************************/
typedef struct BTNR_7550_TunerStatus
{
    int32_t rfAgcVal;   /* RF AGC value in 1/256 dB */
	int32_t ifAgcVal;   /* IF AGC value in 1/256 dB */  
    int32_t lnaAgcVal;   /* LNA AGC value in 1/256 dB */
	int32_t	dpmLvel;		/* dpm dvbm level 1/256 */
	int32_t	signalStrength;		/* ofdm signal strength  level 1/256 */
	int32_t    agcStatus;
} BTNR_7550_TunerStatus;

/***************************************************************************
Summary:
    BTNR_7550_GetTunerStatus
****************************************************************************/
BERR_Code BTNR_7550_GetTunerStatus(
    BTNR_Handle hTnr,
    BTNR_7550_TunerStatus *pStatus
    );

/***************************************************************************
Summary:
    BTNR_7550_GetDPM
****************************************************************************/
BERR_Code BTNR_7550_EnableDPM(
    BTNR_Handle hTnr
    );
/***************************************************************************
Summary:
    Data for the ADS interrupt callback
****************************************************************************/
typedef struct BTNR_7550_AdsInterruptData
{
    bool bResetDpm;
    uint8_t iOutdivMs;
} BTNR_7550_AdsInterruptData;


/***************************************************************************
Summary:
    Callback to pass tuning parameters to the demodulator
****************************************************************************/
typedef void (*BTNR_7550_AdsInterruptCallback)(void *pParam1, int param2, const BTNR_7550_AdsInterruptData *pData);


/***************************************************************************
Summary:
    Callback info
****************************************************************************/
typedef struct BTNR_P_CallbackInfo
{
    BTNR_7550_AdsInterruptCallback func;
    void *pParm1;
	int Parm2;
} BTNR_P_CallbackInfo;

/***************************************************************************
Summary:
    BTNR_7550_InstallAdsInterruptCallback
****************************************************************************/
BERR_Code BTNR_7550_InstallAdsInterruptCallback(
	BTNR_Handle			            hDev,
	BTNR_7550_AdsInterruptCallback  fCallBack_isr,
    void                            *pParam1,
    int					            param2
    );

/***************************************************************************
Summary:
    BTNR_7550_RemoveAdsInterruptCallback
****************************************************************************/
BERR_Code BTNR_7550_RemoveAdsInterruptCallback(
    BTNR_Handle handle  /* [in] BTNR handle */
    );

/***************************************************************************
Summary:
	BTNR_7550_SetLNABoost
****************************************************************************/
BERR_Code BTNR_7550_SetLNABoost(
    BTNR_Handle hTnr,
    bool on
    );

/***************************************************************************
Summary:
	BTNR_7550_SetLNATilt
****************************************************************************/
BERR_Code BTNR_7550_SetLNATilt(
    BTNR_Handle hTnr,
    bool on
    );

#ifdef __cplusplus
}
#endif
 
#endif

