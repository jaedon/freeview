/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpwm.h $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 6/7/11 1:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwm/7038/bpwm.h $
 * 
 * Hydra_Software_Devel/22   6/7/11 1:16p nickh
 * SW7425-685: Increase PWM channels for 7425
 * 
 * Hydra_Software_Devel/21   3/14/11 2:33p jhaberf
 * SW35330-13: replaced 35330 support with 35233 support
 * 
 * Hydra_Software_Devel/20   2/23/11 3:21p nickh
 * SW7422-286: Increase PWM channels to match HW
 * 
 * Hydra_Software_Devel/19   11/5/10 7:47p jhaberf
 * SW35125-1: Added 35125 DTV chip support
 * 
 * Hydra_Software_Devel/18   5/17/10 3:00p brianlee
 * SW35230-239: Added new APIs to select open drain/totem pole output.
 * Also fixed a previous bug with setting totem pole output.
 * 
 * Hydra_Software_Devel/17   4/26/10 11:20a brianlee
 * SW35230-195: Added support for multiple PWM cores.  Also added new APIs
 * to set/get output polarity.
 * 
 * Hydra_Software_Devel/16   2/10/10 3:10p farshidf
 * SW3548-2779: add  BPWM_GetOnInterval_isr
 * 
 * Hydra_Software_Devel/15   11/24/09 5:39p brianlee
 * SW35230-23: Added new APIs for 35230 (stubs).
 * 
 * Hydra_Software_Devel/14   8/4/09 1:27p erickson
 * PR57259: add BPWM_RampOnInterval and related settings
 *
 * Hydra_Software_Devel/13   12/3/08 10:16a kaushikb
 * PR49867: Adding support for 7420
 *
 * Hydra_Software_Devel/12   8/27/08 1:34p farshidf
 * PR45894: BPWM_SetOnInterval_isr which is safe ro call under isr since
 * we just write to the register the value passed
 *
 * Hydra_Software_Devel/10   1/22/07 1:45p farshidf
 * PR27084: remove the warning
 *
 * Hydra_Software_Devel/9   1/15/07 6:02p farshidf
 * PR27084: change FreqModeType to BPWM_FreqModeType
 *
 * Hydra_Software_Devel/8   1/15/07 2:32p farshidf
 * PR27084: add example of how to use the new APIs
 *
 * Hydra_Software_Devel/7   1/15/07 11:10a farshidf
 * PR27084: add PWM Freq  and contant modes. Add also the On and Period
 * intervals functions
 *
 * Hydra_Software_Devel/6   10/20/03 6:04p brianlee
 * Added #define for number of PWM channels.
 *
 * Hydra_Software_Devel/5   10/1/03 11:41a brianlee
 * Fixed sample code.
 *
 * Hydra_Software_Devel/4   9/25/03 12:13p brianlee
 * Fixed tags for Midas service generator.
 *
 * Hydra_Software_Devel/3   9/17/03 9:53a brianlee
 * Fixed sample code.  Add error code.
 *
 * Hydra_Software_Devel/1   7/24/03 5:28p brianlee
 * Initial version.
 *
 ***************************************************************************/


/*= Module Overview *********************************************************
<verbatim>

Overview
The PWM PI module controls the PWM core within the BCM7038.
There are two independent PWM generators, each with programmable
control word (duty cycle).

Design
The design for BPWM PI API is broken into two parts.
  Part 1 (open/close/configuration):
	These APIs are used for opening and closing BPWM device/device channel.
  Part 2 (setting control word):
	These APIs are used to program the control word.
  Part 3 (start/stop PWM generation):
	These APIs are used to start and stop PWM signal generation.

Usage
The usage of BPWM involves the following:
   * Configure/Open of BPWM
	  * Configure BPWM device for the target system
	  * Open BPWM device
	  * Configure BPWM device channel for the target system
	  * Open BPWM device channel
   * Set the Frequency mode
	  * Program the desired frequency mode : Variable or constant mode
   * Set control word
	  * Program the desired duty cycle through control word
   * Set the On and Period interval in case of constant frequency mode
	  * Program the desired duty cycle with the on and period intervals
   * Controlling PWM generation
	  * Starting and stopping PWM signal generation.

sample code
void main( void )
{
	BPWM_Handle       hPwm;
	BPWM_ChannelHandle   hPwmChan;
	BREG_Handle       hReg;
	BCHP_Handle       hChip;
	BPWM_ChannelSettings defChnSettings;
	unsigned int ChanNo;

	// Do other initialization, i.e. for BREG, BCHP, etc.

	BPWM_Open( &hPwm, hChip, hReg, (BPWM_Settings *)NULL );

	ChanNo = 0; // example for channel 0
	BPWM_GetChannelDefaultSettings( hPwm, ChanNo, &defChnSettings );

	// Make any changes required from the default values
	defChnSettings.openDrainb = false;
	defChnSettings.BPWM_FreqModeType= Variable_Freq_Mode;

	BPWM_OpenChannel( hPwm, &hPwmChan, ChanNo, &defChnSettings );

	// in case you want to change the frequncy mode to contant mode to be able to control the duty cycle
	BPWM_SetFreqMode (hPwmChan, Constant_Freq_Mode);

	// 50% duty cycle
	BPWM_SetControlWord (hPwmChan, 0x8000);

   // set the On interval to 0x40 and the period interval to 0x80, that will keep the duty cycle at 50
   // you can change the duty cycle by chaaging the ratio of On and period intervals in case of contant frequency mode
   // in case of  variable frequency mode the On and period interval values will not have any impact.
   BPWM_SetOnAndPeriodInterval (hPwmChan, 0x40, 0x80);

	// Now start pwm generation
	BPWM_Start (hPwmChan);

	// Stop pwm generation
	BPWM_Stop (hPwmChan);

}
</verbatim>
***************************************************************************/


#ifndef BPWM_H__
#define BPWM_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	Number of PWM channels supported

Description:

See Also:

****************************************************************************/
#if ((BCHP_CHIP==35230) || (BCHP_CHIP==35125) || (BCHP_CHIP==35233))
#define MAX_PWM_CHANNELS			10
#elif ((BCHP_CHIP==7420) || (BCHP_CHIP==7422) || (BCHP_CHIP==7425))
#define MAX_PWM_CHANNELS			4
#else
#define MAX_PWM_CHANNELS			2
#endif

/***************************************************************************
Summary:
	Error Codes specific to BPWM

Description:

See Also:

****************************************************************************/
#define BPWM_ERR_NOTAVAIL_CHN_NO		BERR_MAKE_CODE(BERR_PWM_ID, 0)

/***************************************************************************
Summary:
	The handles for pwm module.

Description:
	Since BPWM is a device channel, it has main device handle as well
	as a device channel handle.

See Also:
	BPWM_Open(), BPWM_OpenChannel()

****************************************************************************/
typedef struct BPWM_P_Handle				*BPWM_Handle;
typedef struct BPWM_P_ChannelHandle			*BPWM_ChannelHandle;

/***************************************************************************
Summary:
	Required default settings structure for PWM module.

Description:
	The default setting structure defines the default configure of
	PWM when the device is opened.  Since BPWM is a device
	channel, it also has default settings for a device channel.
	Currently there are no parameters for device setting.

See Also:
	BPWM_Open(), BPWM_OpenChannel()

****************************************************************************/
typedef struct BPWM_Settings
{
	BINT_Handle hInterrupt; /* only required if using BPWM_RampOnInterval */
} BPWM_Settings;

typedef enum BPWM_FreqModeType
{
	Variable_Freq_Mode = 0,
	Constant_Freq_Mode = 1
}BPWM_FreqModeType;

/***************************************************************************
Summary:
	Enumeration for PWM open drain configuration

Description:
	This enumeration defines open drain config vs. totem pole output

See Also:
	None.

****************************************************************************/
typedef enum BPWM_OpenDrainOutput
{
	PwmOutOpenDrain = 0,
	PwmOutTotemPole
} BPWM_OpenDrainOutput;

typedef struct BPWM_ChannelSettings
{
	bool openDrainb;					/* open drain enable */
	BPWM_FreqModeType  FreqMode;                /* Variable Freq mode or Constant Freq mode */

	unsigned maxChangesPerInterrupt; /* maximum number of register writes per interrupt when using BPWM_RampOnInterval */
	BINT_Id interruptId; /* interrupt ID which BPWM_RampOnInterval will use to ramp */
	BINT_Id secondaryInterruptId; /* additional interrupt ID which BPWM_RampOnInterval will use to ramp */
} BPWM_ChannelSettings;

/***************************************************************************
Summary:
	This function opens PWM module.

Description:
	This function is responsible for opening BPWM module. When BPWM is
	opened, it will create a module handle and configure the module based
	on the default settings. Once the device is opened, it must be closed
	before it can be opened again.

Returns:
	TODO:

See Also:
	BPWM_Close(), BPWM_OpenChannel(), BPWM_CloseChannel(),
	BPWM_GetDefaultSettings()

****************************************************************************/
BERR_Code BPWM_Open(
	BPWM_Handle *pPWM,					/* [out] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BREG_Handle hRegister,				/* [in] Register handle */
	const BPWM_Settings *pDefSettings	/* [in] Default settings */
	);

/***************************************************************************
Summary:
	This function closes PWM module.

Description:
	This function is responsible for closing BPWM module. Closing BPWM
	will free main BPWM handle. It is required that all opened
	BPWM channels must be closed before calling this function. If this
	is not done, the results will be unpredicable.

Returns:
	TODO:

See Also:
	BPWM_Open(), BPWM_CloseChannel()

****************************************************************************/
BERR_Code BPWM_Close(
	BPWM_Handle hDev					/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function returns the default settings for PWM module.

Description:
	This function is responsible for returns the default setting for
	BPWM module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BPWM_Open()

****************************************************************************/
BERR_Code BPWM_GetDefaultSettings(
	BPWM_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);

/***************************************************************************
Summary:
	This function returns the total number of channels supported by
	PWM module.

Description:
	This function is responsible for getting total number of channels
	supported by BPWM module, since BPWM device is implemented as a
	device channel.

Returns:
	TODO:

See Also:
	BPWM_OpenChannel(), BPWM_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BPWM_GetTotalChannels(
	BPWM_Handle hDev,					/* [in] Device handle */
	unsigned int *totalChannels			/* [out] Returns total number downstream channels supported */
	);

/***************************************************************************
Summary:
	This function gets default setting for a PWM module channel.

Description:
	This function is responsible for returning the default setting for
	channel of BPWM. The return default setting is used when opening
	a channel.

Returns:
	TODO:

See Also:
	BPWM_OpenChannel()

****************************************************************************/
BERR_Code BPWM_GetChannelDefaultSettings(
	BPWM_Handle hDev,					/* [in] Device handle */
	unsigned int channelNo,				/* [in] Channel number to default setting for */
	BPWM_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
	);

/***************************************************************************
Summary:
	This function opens PWM module channel.

Description:
	This function is responsible for opening BPWM module channel. When a
	BPWM channel is	opened, it will create a module channel handle and
	configure the module based on the channel default settings. Once a
	channel is opened, it must be closed before it can be opened again.

Returns:
	TODO:

See Also:
	BPWM_CloseChannel(), BPWM_GetChannelDefaultSettings()

****************************************************************************/
BERR_Code BPWM_OpenChannel(
	BPWM_Handle hDev,					/* [in] Device handle */
	BPWM_ChannelHandle *phChn,			/* [out] Returns channel handle */
	unsigned int channelNo,				/* [in] Channel number to open */
	const BPWM_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
	);

/***************************************************************************
Summary:
	This function closes PWM module channel.

Description:
	This function is responsible for closing BPWM module channel. Closing
	BPWM channel it will free BPWM channel handle. It is required that all
	opened BPWM channels must be closed before closing BPWM.

Returns:
	TODO:

See Also:
	BPWM_OpenChannel(), BPWM_CloseChannel()

****************************************************************************/
BERR_Code BPWM_CloseChannel(
	BPWM_ChannelHandle hChn				/* [in] Device channel handle */
	);

/***************************************************************************
Summary:
	This function gets PWM module device handle based on
	the device channel handle.

Description:
	This function is responsible returning BPWM module handle based on the
	BPWM module channel.

Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BPWM_GetDevice(
	BPWM_ChannelHandle hChn,			/* [in] Device channel handle */
	BPWM_Handle *pPWM					/* [out] Returns Device handle */
	);

/***************************************************************************
Summary:
	This function sets the control word

Description:
	This function is used to program the desired control word before
	PWM generation.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_SetControlWord(
	BPWM_ChannelHandle	hChn,			/* [in] Device channel handle */
	uint16_t	 		cWord			/* [in] control word to program */
	);

/***************************************************************************
Summary:
	This function gets the control word

Description:
	This function is used to get the control word for a PWM channel.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_GetControlWord(
	BPWM_ChannelHandle	hChn,			/* [in] Device channel handle */
	uint16_t	 		*cWord			/* [out] control word to program */
	);


/***************************************************************************
Summary:
	This function sets the Freq Model

Description:
	This function is used to program the desired Freq Mode before
	PWM generation.

Returns:
	TODO:

See Also:

***************************************************************************/
BERR_Code BPWM_SetFreqMode(
	BPWM_ChannelHandle	hChn,		/* Device channel handle */
	BPWM_FreqModeType	 Frequeny_Mode		/* Frequency mode  to program */
	);


/***************************************************************************
Summary:
	This function gets the Freq Model

Description:
	This function is used to read the  Freq Mode

Returns:
	TODO:

See Also:

***************************************************************************/
BERR_Code BPWM_GetFreqMode(
	BPWM_ChannelHandle	hChn,			       /* Device channel handle */
	BPWM_FreqModeType	 		*Frequeny_Mode		/* Frequency mode  read */
	);

/***************************************************************************
Summary:
	This function sets the On Interval

Description:
	This function is used to program the desired On Interval before
	PWM generation.

Returns:
	TODO:

See Also:
****************************************************************************/
BERR_Code BPWM_SetOnInterval(
	BPWM_ChannelHandle	hChn,			/* [in] Device channel handle */
	uint16_t	 		OnInterval			/* [out] control word to program */
	);

#define BPWM_SetOnInterval_isr BPWM_SetOnInterval

/***************************************************************************
Summary:
	This function sets the On Interval by making incremental changes (ie. a ramp function) at interrupt time

Description:
	This function is used to program the desired On Interval before
	PWM generation.

	You must set BPWM_Settings.hInterrupt and BPWM_ChannelSettings.interruptId to use this function.
	Set BPWM_ChannelSettings.maxChangesPerInterrupt to control the number of changes per interrupt.
****************************************************************************/
BERR_Code BPWM_RampOnInterval(
	BPWM_ChannelHandle  hChn,           /* Device channel handle */
	uint16_t OnInterval
	);

/***************************************************************************
Summary:
	isr variation of BPWM_RampOnInterval.
****************************************************************************/
BERR_Code BPWM_RampOnInterval_isr(
	BPWM_ChannelHandle  hChn,           /* Device channel handle */
	uint16_t OnInterval
	);

/***************************************************************************
Summary:
	This function gets the On Interval

Description:
	This function is used to get  the On Interval

Returns:
	TODO:

See Also:



****************************************************************************/
BERR_Code BPWM_GetOnInterval(
	BPWM_ChannelHandle	hChn,			/* [in] Device channel handle */
	uint16_t	 		*OnInterval			/* [out] On interval to read */
	);

/***************************************************************************
Summary:
	This function sets the Period Interval

Description:
	This function is used to program the desired Period Interval before
	PWM generation.

Returns:
	TODO:

See Also:



****************************************************************************/
BERR_Code BPWM_SetPeriodInterval(
	BPWM_ChannelHandle	hChn,			/* Device channel handle */
	uint16_t	 		PeriodInterval			/* Period Interval  to program */
	);

/***************************************************************************
Summary:
	This function gets the Period Interval

Description:
	This function is used to get  the period Interval

Returns:
	TODO:

See Also:



****************************************************************************/
BERR_Code BPWM_GetPeriodInterval(
	BPWM_ChannelHandle	hChn,			/* [in] Device channel handle */
	uint16_t	 		*PeriodInterval		/* [out] Period interval  to read */
	);

#define BPWM_GetPeriodInterval_isr BPWM_GetPeriodInterval

/***************************************************************************
Summary:
	This function sets the On and Period Interval

Description:
	This function is used to program the desired On and Period Interval before
	PWM generation.

Returns:
	TODO:

See Also:



****************************************************************************/
BERR_Code BPWM_SetOnAndPeriodInterval(
	BPWM_ChannelHandle	hChn,			        /* Device channel handle */
	uint16_t	 		     OnInterval,		        /* OnInterval  to program */
	uint16_t	 		     PeriodInterval		 /* Period Interval  to program */
	);

/***************************************************************************
Summary:
	This function gets the On and Period Interval

Description:
	This function is used to get the  On and Period Interval

Returns:
	TODO:

See Also:



****************************************************************************/
BERR_Code BPWM_GetOnAndPeriodInterval(
	BPWM_ChannelHandle	hChn,			/* Device channel handle */
	uint16_t	 		*OnInterval,			/* OnInterval  to program */
	uint16_t	 		*PeriodInterval		/* Period Interval  to program */
	);

/***************************************************************************
Summary:
	This function starts PWM generation

Description:
	This function is used to start the PWM generation.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_Start(
	BPWM_ChannelHandle hChn			/* [in] Device channel handle */
	);

/***************************************************************************
Summary:
	This function stops PWM generation

Description:
	This function is used to stop the PWM generation.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_Stop(
	BPWM_ChannelHandle hChn			/* [in] Device channel handle */
	);


/***************************************************************************
Summary:
	This function selects open drain vs. totem pole output

Description:
	This function is used to select open drain vs. totem pole output

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_SetOpenDrainConfig(
	BPWM_ChannelHandle		hChn,			/* [in] Device channel handle */
	BPWM_OpenDrainOutput	type			/* [in] open drain/totem pole select */
	);

/***************************************************************************
Summary:
	This function gets the open drain configuration

Description:
	This function is used to get the open drain vs. totem pole output selection type.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_GetOpenDrainConfig(
	BPWM_ChannelHandle		hChn,			/* [in] Device channel handle */
	BPWM_OpenDrainOutput	*type			/* [out] open drain/totem pole select */
	);


#if ((BCHP_CHIP==35230) || (BCHP_CHIP==35125) || (BCHP_CHIP==35233))
/***************************************************************************
Summary:
	Enumeration for PWM output polarity

Description:
	This enumeration defines output polarity selection

See Also:
	None.

****************************************************************************/
typedef enum BPWM_OutPolaritySel
{
	OutPolarityHigh = 0,
	OutPolarityLow = 1
} BPWM_OutPolaritySel;

/***************************************************************************
Summary:
	Enumeration for PWM Sync Edge Select

Description:
	This enumeration defines sync signal edge selector

See Also:
	None.

****************************************************************************/
typedef enum BPWM_SyncEdgeSel
{
	SyncEdgeNeg = 0,
	SyncEdgePos = 1
} BPWM_SyncEdgeSel;

/***************************************************************************
Summary:
	Enumeration for PWM double buffer mode control

Description:
	This enumeration defines PWM double buffer mode control: disable, period, 
	or sync.

See Also:
	None.

****************************************************************************/
typedef enum BPWM_DblBufMode
{
	DblBufMode_Disabled = 0,
	DblBufMode_Reserved = 1,
	DblBufMode_Period   = 2,
	DblBufMode_Sync     = 3
} BPWM_DblBufMode;

/***************************************************************************
Summary:
	Enumeration for PWM two-input sync

Description:
	This enumeration defines two-input sync type if sync is enabled.

See Also:
	None.

****************************************************************************/
typedef enum BPWM_TwoInputSyncType
{
	TwoInputSync_Vsync = 0,
	TwoInputSync_Vsync2
} BPWM_TwoInputSyncType;

/***************************************************************************
Summary:
	This function sets the control word in register set 1

Description:
	This function is used to program the desired control word in register
	set 1 for two-input sync.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_SetControlWordSet1
(
	BPWM_ChannelHandle  hChn,           /* [in] Device channel handle */
	uint16_t            cWord           /* [in] control word to program for register set 1 */
);

/***************************************************************************
Summary:
	This function gets the control word in register set 1

Description:
	This function is used to get the value of the control word in 
	register set 1 for two-input sync.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_GetControlWordSet1
(
	BPWM_ChannelHandle  hChn,           /* [in] Device channel handle */
	uint16_t            *cWord          /* [out] control word to program for register set 1 */
);

/***************************************************************************
Summary:
	This function sets the On and Period values in register set 1

Description:
	This function is used to program the on interval and cycle period in 
	register set 1 for two-input sync.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_SetOnAndPeriodIntervalSet1
(
	BPWM_ChannelHandle  hChn,           	/* [in] Device channel handle */
	uint16_t            OnInterval,         /* [in] OnInterval  to program */
	uint16_t            PeriodInterval      /* [in] Period Interval  to program */
);

/***************************************************************************
Summary:
	This function gets the On and Period values in register set 1

Description:
	This function is used to get the values of on interval and cycle period in 
	register set 1 for two-input sync.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_GetOnAndPeriodIntervalSet1
(
	BPWM_ChannelHandle  hChn,           	/* [in] Device channel handle */
	uint16_t            *OnInterval,        /* [out] OnInterval  to program */
	uint16_t            *PeriodInterval     /* [out] Period Interval  to program */
);

/***************************************************************************
Summary:
	This function enables or disables sync mode

Description:
	This function is used to enable or disable PWM sync mode.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_EnableSyncMode
(
	BPWM_ChannelHandle	hChn,				/* [in] Device channel handle */
	bool				enable				/* [in] sync enable */
);

/***************************************************************************
Summary:
	This function sets double buffer mode

Description:
	This function is used to set the PWM double buffer mode control.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_SetDblBufMode
(
	BPWM_ChannelHandle	hChn,				/* [in] Device channel handle */
	BPWM_DblBufMode		dblBufMode			/* [in] double buffer mode type */
);

/***************************************************************************
Summary:
	This function gets double buffer mode

Description:
	This function is used to get the PWM double buffer mode control.

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_GetDblBufMode
(
	BPWM_ChannelHandle	hChn,				/* [in] Device channel handle */
	BPWM_DblBufMode		*dblBufMode			/* [out] double buffer mode type */
);

/***************************************************************************
Summary:
	This function sets the sync edge type

Description:
	This function is used to select the sync signal edge

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_SetSyncEdge
(
	BPWM_ChannelHandle	hChn,				/* [in] Device channel handle */
	BPWM_SyncEdgeSel	SyncEdgeSel			/* [in] edge type */
);

/***************************************************************************
Summary:
	This function gets the sync edge type

Description:
	This function is used to get the sync signal edge select type

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_GetSyncEdge
(
	BPWM_ChannelHandle	hChn,				/* [in] Device channel handle */
	BPWM_SyncEdgeSel	*SyncEdgeSel		/* [out] edge type */
);

/***************************************************************************
Summary:
	This function sets the two-input sync type

Description:
	This function is used to select the two-input sync type

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_SetTwoInputSync
(
	BPWM_ChannelHandle		hChn,			/* [in] Device channel handle */
	BPWM_TwoInputSyncType	type			/* [in] two-input sync type */
);

/***************************************************************************
Summary:
	This function gets the two-input sync type

Description:
	This function is used to get the two-input sync type

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_GetTwoInputSync
(
	BPWM_ChannelHandle		hChn,			/* [in] Device channel handle */
	BPWM_TwoInputSyncType	*type			/* [out] two-input sync type */
);

/***************************************************************************
Summary:
	This function sets the output polarity

Description:
	This function is used to set the polarity of the PWM output

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_SetOutputPolarity
(
	BPWM_ChannelHandle	hChn,				/* [in] Device channel handle */
	BPWM_OutPolaritySel	OutPolSel			/* [in] output polarity */
);

/***************************************************************************
Summary:
	This function gets the output polarity

Description:
	This function is used to get the value of the polarity of the PWM output

Returns:
	TODO:

See Also:


****************************************************************************/
BERR_Code BPWM_GetOutputPolarity
(
	BPWM_ChannelHandle	hChn,				/* [in] Device channel handle */
	BPWM_OutPolaritySel	*OutPolSel			/* [out] output polarity */
);



#endif

#ifdef __cplusplus
}
#endif

#endif



