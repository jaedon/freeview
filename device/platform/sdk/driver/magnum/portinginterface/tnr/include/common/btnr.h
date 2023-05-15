/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr.h $
 * $brcm_Revision: Hydra_Software_Devel/44 $
 * $brcm_Date: 10/18/12 3:41p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/include/common/btnr.h $
 * 
 * Hydra_Software_Devel/44   10/18/12 3:41p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/42   9/28/12 3:06p atanugul
 * SW3128-228: Add support for BTNR_GetVersionInfo()
 * 
 * Hydra_Software_Devel/41   7/30/12 3:14p atanugul
 * SW3461-252: Add PI support for Tuen Gain API
 * 
 * Hydra_Software_Devel/40   4/9/12 2:56p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/39   3/30/12 12:07p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/38   1/26/12 11:51a atanugul
 * SW3461-131: remove BTNR_RfInputMode_eInternalLnaDaisy
 * 
 * Hydra_Software_Devel/37   1/16/12 6:20p atanugul
 * SW3461-131: Add new BTNR_RfInputMode to set the mode to internal LNA
 * and enable daisy out
 * 
 * Hydra_Software_Devel/36   1/10/12 4:56p atanugul
 * SW3128-107:Enable HAB CRC Check in PI
 * 
 * Hydra_Software_Devel/35   12/22/11 3:37p atanugul
 * SW3128-100: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/34   5/27/11 2:17p atanugul
 * SW3461-3: Converted BTNR_LpfBandwidth enum into unsigned integer
 * 
 * Hydra_Software_Devel/33   5/24/11 5:36p atanugul
 * SW3461-3: Modify BTNR_LpfBandwidth, BTNR_Standard according to the
 * naming convention
 * 
 * Hydra_Software_Devel/32   5/24/11 3:29p atanugul
 * SW3461-3: Add BTNR_RfInputMode and agcVal to BTNR_Settings structure
 * and remore BTNR_PowerMode enum
 * 
 * Hydra_Software_Devel/31   5/5/11 4:11p atanugul
 * SW3128-2: Remove BTNR_TunerApplication
 * 
 * Hydra_Software_Devel/30   5/4/11 4:22p atanugul
 * SW7231-32: Reverting back to version 28
 * 
 * Hydra_Software_Devel/29   5/3/11 11:51a vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Undo the changes done since version 26.
 * 
 * Hydra_Software_Devel/28   4/19/11 12:32p atanugul
 * SW3461-3: Implement BTNR_SetSettings(), Powermode for 3461 TNR and
 * update BHAB_3461_P_DecodeInterrupt()
 * 
 * Hydra_Software_Devel/27   4/11/11 5:15p atanugul
 * SW3128-2: Add Get_Settings and SetSettings support to 3128 TNR PI
 * 
 * Hydra_Software_Devel/26   11/8/10 5:13p haisongw
 * SW7125-717: add Docsis scan control support
 * 
 * Hydra_Software_Devel/25   6/22/10 6:14p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/24   11/18/09 10:18a farshidf
 * SW7550-38: add the last changes
 * 
 * Hydra_Software_Devel/23   11/12/09 6:57p farshidf
 * SW7550-40: update the tuner files
 * 
 * Hydra_Software_Devel/22   11/10/09 7:11p farshidf
 * 7550-38: add the tuner code
 * 
 * Hydra_Software_Devel/21   8/21/09 6:20p vishk
 * PR 57484: Bcm31xx: Add Power Management support for TNR
 * 
 * Hydra_Software_Devel/20   9/12/07 12:54p vishk
 * PR 29024: To create PI for BCM3420 tuner.
 * 
 * Hydra_Software_Devel/19   6/7/05 8:35a dlwin
 * PR 14682: Added additional functions for Docsis and general use.  New
 * function 'BTNR_GetInfo()' returns additional informtion for the tuner.
 * 
 * Hydra_Software_Devel/18   4/1/05 5:25p dlwin
 * PR 14682: Added semi-private functions for QamCm to use to AGC
 * readings.  This required for Docsis integration.
 * 
 * Hydra_Software_Devel/17   12/2/03 9:27a dlwin
 * Updated sample code to use NULL instead of hChip3250.
 * 
 * Hydra_Software_Devel/16   10/7/03 8:52a dlwin
 * Removed the need to call a create generic tuner  handle.
 * 
 * Hydra_Software_Devel/15   9/29/03 7:59p dlwin
 * Added first pass of test plan documentation.
 * 
 * Hydra_Software_Devel/14   9/18/03 2:43p dlwin
 * Remvoed any warning from '-Wall' option.
 * 
 * Hydra_Software_Devel/13   9/9/03 8:43a dlwin
 * Updated API function parameter comment for DocJet.
 * 
 * Hydra_Software_Devel/12   8/29/03 7:32a dlwin
 * Updated comments.
 * 
 * Hydra_Software_Devel/11   8/26/03 2:18p dlwin
 * Updated comment and sample code.
 * 
 * Hydra_Software_Devel/10   8/26/03 11:09a dlwin
 * Newly updated API, using a similar design model as BREG_I2C and BREG.
 * 
 * Hydra_Software_Devel/9   8/22/03 9:11a dlwin
 * Updated sample code.
 * 
 * Hydra_Software_Devel/8   8/20/03 6:19p dlwin
 * Updated to be more DocJet happy.
 * 
 * Hydra_Software_Devel/7   8/20/03 4:15p dlwin
 * Update sample code.
 * 
 * Hydra_Software_Devel/6   8/18/03 9:23a dlwin
 * Added interrupt requirement.
 * 
 * Hydra_Software_Devel/5   8/14/03 8:37a dlwin
 * Updated sample code.
 * 
 * Hydra_Software_Devel/4   8/1/03 7:11p dlwin
 * Added much more comments, plus changed BTNR_SetTunerFreq() to
 * BTNR_SetTunerRfFreq() and BTNR_GetTunerFreq() to
 * BTNR_GetTunerRfFreq().
 * 
 * Hydra_Software_Devel/3   6/25/03 4:43p dlwin
 * Updated comments.
 * 
 * Hydra_Software_Devel/2   6/12/03 5:43p dlwin
 * Added more comments.
 * 
 * Hydra_Software_Devel/1   6/3/03 4:03p dlwin
 * Initial version
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
Tuner module represents a generalize representatiion of a tuner.  The 
basic function of a tuner is for downconverting Radio Frequency (RF) signal to
a Intermediate Frequency (IF) signal.  For InBand downstream, the tuner module
is responsible for downconverting RF frequencies of 130MHz-860MHz to an
IF signal.  For most reference design, IF frequency is either 43.75 MHz for
Annex B or 36.125 MHz for Annex A.  Annex A support requires a H/W tuner
with 8MHz SAW filter.  Annex B support requires a H/W tuner with 6MHz SAW filter.
For OutOfBand downstream, the tuner module is responsible for downconverting
RF frequencies of 70MHz-130MHz to an IF signal.  For most reference design, 
IF frequency for OutOfBand is 44MHz.
To support the numerous tuners that may appear on for system, the Tuner module
has been designed to allow for customization by the S/W board designer yet
provide a common interface for controlling the operation of the tuner.  To
accomplish the above goals, the Tuner module API is divided into two parts.
First section (called Custom) deals with customization while the other section
(called General) provides control over the operation of a tuner.  The Custom 
section API consists of Open/Close/Create.  The General section API provides
GetRfFreq/SetRfFreq, 


Design
The design for BQDS PI API is broken into two parts.

o Part 1 (open/close/configuration/create):

    These APIs are used for opening and closing a specific BTNR device.  Also
    included in this section is an API to create a general BTNR device handle.

o Part 2 (get/set):

    These APIs are used for getting and setting runtime attributes of BTNR
    using a general BTNR device handle.


Usage
The usage of BTNR involves the following:

   * Configure/Open/Create of BTNR

      * Configure BTNR device for the target system
      * Open BTNR device

   * Tune device channel

      * Set RF frequency using general BTNR handle


Interrupt Requirements:
None


Sample Code
//
// NOTE: The following sample code does not do any error checking.
//
// In this example, the target H/W board contains two tuners. One is a
// Bcm3418 tuner configuried for Annex B application, with IF Frequency
// of 43.75 MHz.  The other tuner is an Alps TDDE tuner, so used Annex B
// application.
//

#include "btnr_3418.h"
#include "btnr_alpstdde.h"

static BTNR_AlpsTdde_Handle hTnrAlpsTdde;
static BTNR_Handle hTnrDev1;
static BTNR_Handle hTnrDev2;
static BCHP_Handle hChip3250;
static BREG_Handle hReg3250;
static BREG_I2C_Handle hBcm7038I2C;


main( void )
{
    BTNR_3418_Settings tnr3418DevSettings;
    BTNR_AlpsTdde_Settings tnrAlpsTddeDevSettings;


    // Initialize hChip3250, hReg3250, hInt3250, and hBcm7038I2C . . .


    // *************************************************************************
    // Start of H/W board specific configuration for BTNR
    // *************************************************************************
    // Configure first tuner, Bcm3418 Device
    BTNR_3418_GetDefaultSettings( &tnr3418DevSettings, hChip3250 );
    // Now initialize Bcm3418 specific data, Bcm3418 needs to know the
    //   If Freq. to use, 43.75 MHz
    //   I2C address of Bcm3418 device, addr=0x10
    tnr3418DevSettings.ifFreq = 43750000;
    tnr3418DevSettings.i2cAddr = 0x10;
    // Open first tuner, Bcm3418 Device
    BTNR_3418_Open( &hTnrDev1, NULL, hBcm7038I2C, &tnr3418DevSettings );

    // Configure second tuner, Alps TDDE Device
    BTNR_AlpsTdde_GetDefaultSettings( &tnrAlpsTddeDevSettings, hChip3250 );
    // Now initialize Alps TDDE specific data
    tnrAlpsTddeDevSettings.ifFreq = 43750000;
    // Open second tuner, Alps TDDE Device
    BTNR_AlpsTdde_Open( &hTnrDev2, NULL, hBcm7038I2C, &tnrAlpsTddeDevSettings );

    // *************************************************************************
    // End of H/W board specific configuration for BTNR
    // *************************************************************************

    // Set the first tuner to 777,000,000 Hz (777 Mhz) for a digital channel
    // using general BTNR handle.
    BTNR_SetTunerRfFreq( hTnrDev1, 777000000, BTNR_TunerMode_eDigital );
    // Set the first tuner to 600,000,000 Hz (600 Mhz) for a analog channel
    // using general BTNR handle.
    BTNR_SetTunerRfFreq( hTnrDev1, 600000000, BTNR_TunerMode_eAnalog );

    // Set the second tuner to 500,000,000 Hz (500 Mhz) for a digital channel
    // using general BTNR handle.
    BTNR_SetTunerRfFreq( hTnrDev2, 500000000, BTNR_TunerMode_eDigital );
}

</verbatim>
***************************************************************************/

/****************************************************************************
    Test Plan

Purpose
    The purpose of this test script is to test Tuner PI as described in the
    Design Requirements in Twiki.

Requirement
    1 Shall use Device Porting Interface device model, supporting 1 or more device channels 
    2 Shall use Set/Get Porting Interface change model 
    3 Shall provide support for Bcm3418 tuner 
    4 Shall provide support for Out-of-Band Bcm3250 tuner 
    5 Shall support configurable board specific Tuner component per device channel 
    6 Shall provide programmable RF Frequency per device channel 
    7 Shall provide programmable Analog/Digital RF signal per device channel 

Test Setup
    See QDS and QOB Test Plans, which tests this PI.


Test Procedure
    See QDS and QOB Test Plans, which tests this PI.


***************************************************************************/

#ifndef BTNR_H__
#define BTNR_H__

#include "bchp.h"
#include "berr_ids.h"
#include "bfec.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    Error Codes specific to BTNR

Description:

See Also:

****************************************************************************/
#define BTNR_ERR_POWER_DOWN       BERR_MAKE_CODE(BERR_TNR_ID, 0)
#define BTNR_ERR_INVALID_CRC      BERR_MAKE_CODE(BERR_TNR_ID, 1)

/***************************************************************************
Summary:
    The handle for Tuner module

Description:
    An opaque handle for BTNR device.

See Also:
    BTNR_xxx_CreateTunerHandle(), 'xxx' is specific to a tuner, for example
    BTNR_3418_CreateTunerHandle() for Bcm3418 tuner.

****************************************************************************/
typedef struct BTNR_P_Handle                *BTNR_Handle;

/***************************************************************************
Summary:
    Enumeration for Tuner mode

Description:
    This enumeration defines the tuner mode for Qam Upstream.

See Also:
    BTNR_SetTunerRfFreq(), BTNR_GetTunerRfFreq()

****************************************************************************/
typedef enum BTNR_TunerMode
{
    BTNR_TunerMode_eDigital,
    BTNR_TunerMode_eAnalog,
    BTNR_TunerMode_eDocsis,   
    BTNR_TunerMode_eLast
} BTNR_TunerMode;

/***************************************************************************
Summary:
    Enumeration for Tuner Application

Description:
    This field controls the target application.

See Also:
    BTNR_SetSettings(), BTNR_GetSettings()

****************************************************************************/
typedef enum BTNR_TunerApplication
{
  BTNR_TunerApplication_eCable,
  BTNR_TunerApplication_eTerrestrial,
  BTNR_TunerApplication_eLast
} BTNR_TunerApplication;

/***************************************************************************
Summary:
    Enumeration for modulation standard

Description:
    This field controls modulation standard to be received.

See Also:
    BTNR_SetSettings(), BTNR_GetSettings()

****************************************************************************/
typedef enum BTNR_Standard
{
  BTNR_Standard_eDvbt,
  BTNR_Standard_eIsdbt,
  BTNR_Standard_eQam,
  BTNR_Standard_eDvbt2,
  BTNR_Standard_eLast
} BTNR_Standard;

/***************************************************************************
Summary:
    Enumeration for TNR Input Mode

Description:
    This field specifies the Input mode for TNR.

See Also:
    BTNR_SetSettings(), BTNR_GetSettings()

****************************************************************************/
typedef enum BTNR_RfInputMode
{
  BTNR_RfInputMode_eOff, /* Tuner is off. */
  BTNR_RfInputMode_eExternalLna,  /* Tuner Rf input through UHF path. This Rf path does not use internal LNA. */
  BTNR_RfInputMode_eInternalLna, /* Tuner Rf input through VHF path. This Rf path uses internal LNA. */
  BTNR_RfInputMode_eStandardIf,  /* 44 MHz or 36 MHz */
  BTNR_RfInputMode_eLowIf,  /*4 MHz to 5 MHz. */
  BTNR_RfInputMode_eBaseband,
  BTNR_RfInputMode_eLast
} BTNR_RfInputMode;

/***************************************************************************
Summary:
    Enumeration for FFT Size for Spetrum Analyzer Data

Description:
    This enumeration represents the FFT Size for Spetrum Analyzer Data.

See Also:

****************************************************************************/
typedef enum BTNR_FftSize
{
    BTNR_FftSize_e64 = 0x6,
    BTNR_FftSize_e128 = 0x7,
    BTNR_FftSize_e256 = 0x8,
    BTNR_FftSize_e512 = 0x9,
    BTNR_FftSize_e1024 = 0x0a,
    BTNR_FftSize_e2048 = 0x0b,
    BTNR_FftSize_e4096 = 0x0c,
    BBTNR_FftSize_eLast
} BTNR_FftSize;

/***************************************************************************
Summary:
    Callback used for event notification.

Description:
    When this PI wants to notify an application, it will call this callback
    function the callback function is registered.

See Also:

****************************************************************************/
typedef BERR_Code (*BTNR_CallbackFunc)(void *pParam );

/***************************************************************************
Summary:
    Enumeration for Callback types

Description:
    This enumeration defines Callback types.

See Also:

****************************************************************************/
typedef enum BTNR_Callback
{
    BTNR_Callback_eSpectrumDataReady,   /* Callback to notify application that spectrum analyzer data is ready */ 
    BTNR_Callback_eLast                 /* More may be required */
} BTNR_Callback;

/***************************************************************************
Summary:
    Structure for Tuner Information

Description:
    This defines the tuner information.

See Also:
    BTNR_GetInfo()

****************************************************************************/
#define BRCM_TUNER_MAKER_ID             (1)
typedef struct BTNR_TunerInfo
{
    unsigned int tunerMaker;            /* 0=unknown, 1=BRCM Tuner */
    unsigned int tunerId;
    unsigned int tunerMajorVer;
    unsigned int tunerMinorVer;
} BTNR_TunerInfo;

/***************************************************************************
Summary:
    Structure to set tuner into power saver mode.

Description:
    Structure to set tuner into power saver mode.

See Also:
    BTNR_GetInfo()

****************************************************************************/
typedef struct BTNR_PowerSaverSettings
{
    bool enable; /* 1 =  enable power saver, 0 = disable power saver. */
} BTNR_PowerSaverSettings;

/***************************************************************************
Summary:
    Structure for Internal Gain Settings

Description:
    Structure for Internal Gain Settings

See Also:
    BTNR_GetSettings(), BTNR_SetSettings()

****************************************************************************/
typedef struct BTNR_InternalGainSettings
{
    uint32_t frequency; /* frequency */
    int16_t internalGainLoopThrough; /* Internal Gain to Loop Through */
    int16_t internalGainDaisy;  /* Internal Gain to Daisy */
} BTNR_InternalGainSettings;

/***************************************************************************
Summary:
    Structure for External Gain Settings

Description:
    Structure for External Gain Settings

See Also:
    BTNR_GetSettings(), BTNR_SetSettings()

****************************************************************************/
typedef struct BTNR_ExternalGainSettings
{
    int16_t externalGainBypassable; /* External Gain bypassable */
    int16_t externalGainTotal; /* External Gain Total */
} BTNR_ExternalGainSettings;

/***************************************************************************
Summary:
    Structure to set tuner settings.

Description:
    Structure to set tuner settings.

See Also:
    BTNR_GetSettings(), BTNR_SetSettings()

****************************************************************************/
typedef struct BTNR_Settings
{
    BTNR_Standard std;
    BTNR_TunerApplication tnrApplication;
    uint32_t bandwidth; /* low pass filter bandwidth in units of Hz */
    BTNR_RfInputMode rfInputMode; /* TNR RF input mode */
    uint32_t agcVal; /* AGC value */
    BTNR_InternalGainSettings internalGainSettings; /* internal gain settings */
    BTNR_ExternalGainSettings externalGainSettings; /* external gain settings */
} BTNR_Settings;

/***************************************************************************
Summary:
    Structure for Spectrum Analyzer Data

Description:

See Also:
    BTNR_GetSpectrumAnalyzerData()

****************************************************************************/
typedef struct BTNR_SpectrumData
{
    uint32_t *data; /* ptr to the data */
    uint16_t datalength; /* data length in number of words */    
    bool moreData; /* this bit indicates whether there is more data available or not */
} BTNR_SpectrumData;

/***************************************************************************
Summary:
    Spectrum settings for Spectrum Analyzer Data

Description:

See Also:
    BTNR_RequestSpectrumAnalyzerData()

****************************************************************************/
typedef struct BTNR_SpectrumSettings
{
    uint32_t startFreq; /* The start frequency of the spectrum analyzer in Hz */
    uint32_t stopFreq; /* The stop frequency of the spectrum analyzer in Hz */
    BTNR_FftSize fftSize; /*  FFT Size */
    uint8_t reserved; /*  Reserved for future use */
    uint32_t numSamples; /* Total number of 32-bit frequency values to return */
} BTNR_SpectrumSettings;

/***************************************************************************
Summary:
    This function sets the Tuner to the requested frequency.

Description:
    This function is responsible for setting Tuner to a frequency.

    This function will call the tuner specific SetRfFreq function.
    
Returns:
    TODO:

See Also:
    BTNR_GetTunerRfFreq()

****************************************************************************/
BERR_Code BTNR_SetTunerRfFreq(
    BTNR_Handle hDev,                   /* [in] Device handle */
    uint32_t freq,                      /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode            /* [in] Requested tuner mode */
    );

/***************************************************************************
Summary:
    This function gets the current Tuner tuned frequency.

Description:
    This function is responsible for getting the current Tuner tuned frequency.

    This function will call the tuner specific GetRfFreq function.
    
Returns:
    TODO:

See Also:
    BTNR_SetTunerRfFreq()

****************************************************************************/
BERR_Code BTNR_GetTunerRfFreq(
    BTNR_Handle hDev,                   /* [in] Device handle */
    uint32_t *freq,                     /* [out] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode           /* [out] Returns tuner mode */
    );

/***************************************************************************
Summary:
    This function gets the Tuner Information.

Description:
    This function is responsible for getting Tuner Information.
    
Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BTNR_GetInfo(
    BTNR_Handle hDev,                   /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo             /* [out] Tuner information */
    );

/***************************************************************************
Summary:
    This function closes Tuner module.

Description:
    This function is responsible for closing Tuner module.
    
Returns:
    TODO:

See Also:
    BTNR_xxxx_Open()

****************************************************************************/
BERR_Code BTNR_Close(
    BTNR_Handle hDev                    /* [in] Device handle */
    );

/***************************************************************************
Summary:
    This function gets the power-saver mode.

Description:
    This function returns the current power-saver mode.
    
Returns:
    TODO:

See Also:
    BTNR_xxxx_Open()

****************************************************************************/
BERR_Code BTNR_GetPowerSaver(
    BTNR_Handle hDev,                   /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings /* [out] Power saver settings. */
    );

/***************************************************************************
Summary:
    This function sets the power-saver mode.

Description:
    This function is responsible for enabling/disabling the downstream tuner
    power-saver mode.  When the power-saver mode is enabled, the
    Qam In-Band Downstream tuner is shutdown. 
    
Returns:
    TODO:

See Also:
    BTNR_xxxx_Open()

****************************************************************************/
BERR_Code BTNR_SetPowerSaver(
    BTNR_Handle hDev,                   /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings /* [in] Power saver settings. */
    );

/***************************************************************************
Summary:
    This function gets the Tuner's AGC register value.

Description:
    This function is responsible for getting the AGC register value using
    register offset.
    
Returns:
    TODO:

See Also:
    BTNR_xxxx_Open()

****************************************************************************/
BERR_Code BTNR_P_GetTunerAgcRegVal(
    BTNR_Handle hDev,                   /* [in] Device handle */
    uint32_t regOffset,                 /* [in] AGC register offset */
    uint32_t *agcVal                    /* [out] output value */
    );

/***************************************************************************
Summary:
    This function sets the Tuner's AGC register value.

Description:
    This function is responsible for setting the AGC register value using
    register offset.
    
Returns:
    TODO:

See Also:
    BTNR_xxxx_Open()

****************************************************************************/
BERR_Code BTNR_SetTunerAgcRegVal(
    BTNR_Handle hDev,                   /* [in] Device handle */
    uint32_t regOffset,                 /* [in] AGC register offset */
    uint32_t *agcVal                    /* [in] input value */
    );

/***************************************************************************
Summary:
    This function gets TNR settings.

Description:
    This function returns the current TNR Settings.
    
Returns:
    TODO:

See Also:
    BTNR_xxxx_Open()

****************************************************************************/
BERR_Code BTNR_GetSettings(
    BTNR_Handle hDev,           /* [in] Device handle */
    BTNR_Settings *settings     /* [out] TNR settings. */
    );
    
/***************************************************************************
Summary:
    This function sets TNR settings.

Description:
    This function sets the current TNR Settings.
    
Returns:
    TODO:

See Also:
    BTNR_xxxx_Open()

****************************************************************************/
BERR_Code BTNR_SetSettings(
    BTNR_Handle hDev,           /* [in] Device handle */
    BTNR_Settings *settings     /* [in] TNR settings. */
    );
    
/******************************************************************************
Summary:
   This function sends request for spectrum analyzer data to the LEAP.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BTNR_RequestSpectrumAnalyzerData(
    BTNR_Handle hDev,     /* [in] Device handle */ 
    BTNR_SpectrumSettings *pSettings /* [in] spectrum settings */
    );

/******************************************************************************
Summary:
   This function gets spectrum analyzer data from the LEAP.
Description:
  
Returns:
   BERR_Code
******************************************************************************/    
BERR_Code BTNR_GetSpectrumAnalyzerData(
    BTNR_Handle hDev,     /* [in] Device handle */ 
    BTNR_SpectrumData  *pSpectrumData /* [out] spectrum Data*/
    );    
    
/***************************************************************************
Summary:
    This function is responsible for installing a callback function.

Description:
    This function installs a callback function.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BTNR_InstallCallback(
    BTNR_Handle hDev,     /* [in] Device handle */
    BTNR_Callback callbackType, /* [in] Type of callback */
    BTNR_CallbackFunc pCallback_isr, /* [in] Function Ptr to ISR callback */
    void *pParam                 /* [in] Generic parameter send on callback */
    );

/***************************************************************************
Summary:
    This function returns the version information.

Description:
    This function is responsible for returning the core driver version 
    information. It return the majorVersion and minorVersion of the core
    driver.
Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BTNR_GetVersionInfo(
    BTNR_Handle hDev,              /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
    );    
    
#ifdef __cplusplus
}
#endif
 
#endif
