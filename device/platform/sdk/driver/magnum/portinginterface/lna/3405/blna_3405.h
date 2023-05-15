/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: blna_3405.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/24/06 2:01p $
 *
 * Module Description:
 * This module is the driver for BCM3405 LNA.
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/lna/3405/blna_3405.h $
 * 
 * Hydra_Software_Devel/3   10/24/06 2:01p dlwin
 * PR24087: Support BCM3431 lna in reference design.
 * 
 * Hydra_Software_Devel/Refsw_Devel_3560/1   9/6/06 9:21p jerryy
 * PR24087: Support BCM3431 lna in reference design.
 * 
 ***************************************************************************/
/*= Module Overview *********************************************************
<verbatim>

Overview
The Low Noise Amplifier (LNA) is responsible for providing input signal
conditioning for cable television signals in the 50MHz to 860MHz band.
Bcm3405 provides RF AGC functions, which spans 30 dB, can be used to
accommodate input signal strength variations from -15 dBmV to +15 dBmV.
Bcm3405 is optimized to interface to Bcm3418 tuner.  The Bcm3405 can drive up
to three cable tuners, as well as providing bypass and out of band outputs.

Design
  The design for BLNA PI API is broken into two parts.

o Part 1 (open/close/configuration):

    These APIs are used for opening and closing BLNA device.
    When a device is opened, the device can be configured.

o Part 2 (get/set):

    These APIs are used for getting and setting runtime attributes of BLNA.



Usage
The usage of BLNA involves the following:

   * Configure/Open of BLNA

      * Configure BLNA device for the target system
      * Open BLNA device

   * Program device

      * Program LNA


Interrupt Requirements
None


Sample Code:
//
// NOTE: The following sample code does not do any error checking.
//
// In this example, the target H/W board is configured for an
// Out-of-Band Downstream DVS 178 application, with a Crystal Frequency of 24.00 MHz
// and IF Frequency of 43.75 MHz.
//
static BLNA_Handle lnaDev;
static BCHP_Handle hChip3250;
static BREG_I2C_Handle hBcm7038I2C;


main( void )
{
	BLNA_3405_Settings lna3405DevSettings;


	// Initialize hChip3250, hReg3250, hInt3250, and hBcm7038I2C . . .

	// *************************************************************************
	// Start of H/W board specific configuration for BLNA
	// *************************************************************************
	// Configure Bcm3405 Device
	BLNA_3405_GetDefaultSettings( &lna3405DevSettings, NULL );
	// Now initialize Bcm3405 specific data, Bcm3405 needs to know the
	//   I2C address of Bcm3405 device, addr=0x64 (7-bit address)
	lna3405DevSettings.i2cAddr = 0x64;
	lna3405DevSettings.gainMode = BLNA_GainMode_eAGC;
	// Open BLNA Device								 
	BLNA_3405_Open( &lnaDev, NULL, hBcm7038I2C, &lna3405DevSettings );
	// *************************************************************************
	// End of H/W board specific configuration for BLNA
	// *************************************************************************

	// Set to InBand port 0 to analog mode
	BLNA_SetInBandMode( lnaDev, 0, BLNA_InBandMode_eAnalog );
	
	// Enable AGC mode, with 8 dB gain, with a window of -0.6 dB to +0.6 dB
	BLNA_EnableAutomaticGainControl( lnaDev, 40, 3 ); 

	// Disable AGC mode and enable manual gain, with 10 dB gain
	BLNA_EnableManualGainControl( lnaDev, 50 ); 

}

</verbatim>
***************************************************************************/


#ifndef BLNA_3405_H__
#define BLNA_3405_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	BLNA_3405_MX_INBAND_PORTS		(3)
#define	BLNA_3405_MX_OUTOFBAND_PORTS		(2)
#define	BLNA_3405_MX_EXTDRV				(2)


/***************************************************************************
Summary:
	Required default settings structure for LNA module.

Description:
	The default setting structure defines the default configuration of
	LNA when the device is opened.

See Also:
	BLNA_Open()

****************************************************************************/
#define BLNA_3405_SETTINGS_I2CADDR					(0x64)
#define BLNA_3405_SETTINGS_INBANDPORTS				(BLNA_InBandMode_eDigital)
#define BLNA_3405_SETTINGS_OUTOFBANDPORTS			(BLNA_OutOfBandMode_eNormal)
#define	BLNA_3405_SETTINGS_GAINMODE					(BLNA_GainMode_eAGC)
#define BLNA_3405_SETTINGS_AGCOUTPUTLEVEL			(35)
#define BLNA_3405_SETTINGS_AGCDELTALEVEL				(3)
#define BLNA_3405_SETTINGS_MANUALGAINVAL				(40)
typedef struct BLNA_3405_Settings
{
	unsigned short i2cAddr;				/* 7bit I2C address of Bcm3405 */
	BLNA_GainMode gainMode;				/* See BLNA_GainMode for description */
	BLNA_InBandMode inBandPorts[BLNA_3405_MX_INBAND_PORTS];		/* InBand port mode */
	BLNA_OutOfBandMode outOfBandPorts[BLNA_3405_MX_OUTOFBAND_PORTS];	/* OutOfBand port mode */
	unsigned int agcOutputLevel;		/* AGC Output Level, in 2/10 dB */
	int agcDeltaVal;					/* AGC Delta Value, in 2/10 dB */
	unsigned int manualGainVal;			/* If Manual Gain is enabled, manual gain value, in 2/10 dB */
} BLNA_3405_Settings;

/***************************************************************************
Summary:
	This function opens LNA module.

Description:
	This function is responsible for opening BLNA module. When BLNA is
	opened, it will create a module handle and configure the module based
	on the default settings. Once the device is opened, it must be closed
	before it can be opened again.

Returns:
	TODO:

See Also:
	BLNA_Close(), BLNA_GetDefaultSettings()

****************************************************************************/
BERR_Code BLNA_3405_Open(
	BLNA_Handle *phLna,					/* [out] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BREG_I2C_Handle hI2cReg,			/* [in] I2C Register handle */
	const BLNA_3405_Settings *pDefSettings	/* [in] Default settings */
	);


/***************************************************************************
Summary:
	This function returns the default settings for LNA module.

Description:
	This function is responsible for returns the default setting for 
	BLNA module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BLNA_Open()

****************************************************************************/
BERR_Code BLNA_3405_GetDefaultSettings(
	BLNA_3405_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);



#ifdef __cplusplus
}
#endif
 
#endif



