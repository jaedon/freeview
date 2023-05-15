/***************************************************************************
 *     (c)2005-2011 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: bads_api.h $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 11/16/11 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/35233/bads_api.h $
 * 
 * Hydra_Software_Devel/16   11/16/11 6:04p atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/15   10/10/11 1:51p farshidf
 * SW7552-134: update to match V2_0 of 3461
 * 
 * Hydra_Software_Devel/14   8/16/11 4:24p farshidf
 * SW7552-105: rebase ro 3461_V1_0_RC0 label
 * 
 * Hydra_Software_Devel/13   7/19/11 8:03p farshidf
 * SW7552-60: update teh callback struct
 * 
 * Hydra_Software_Devel/12   7/19/11 7:58p farshidf
 * SW7552-60: add new include
 * 
 * Hydra_Software_Devel/11   7/18/11 3:53p farshidf
 * SWDTV-7869: update the FFT event
 * 
 * Hydra_Software_Devel/10   7/15/11 5:47p farshidf
 * SWDTV-7869: add the 35126Bo changes
 * 
 * Hydra_Software_Devel/9   5/31/11 3:58p farshidf
 * SW7552-36: compile fix
 * 
 * Hydra_Software_Devel/8   5/10/11 11:09p farshidf
 * SWDTV-7035: merge main
 * 
 * Hydra_Software_Devel/SWDTV-7035/1   5/10/11 12:31p farshidf
 * SWDTV-7035: update to latest
 * 
 * Hydra_Software_Devel/7   5/6/11 4:26p farshidf
 * SWDTV-6857: waring fix
 * 
 * Hydra_Software_Devel/6   5/6/11 2:23p farshidf
 * SWDTV-6857: merge main
 * 
 * Hydra_Software_Devel/SWDTV-6857/1   5/6/11 2:03p farshidf
 * SWDTV-6587: add scan structure and calls
 * 
 * Hydra_Software_Devel/5   4/28/11 8:18p farshidf
 * SWDTV-6190: fix the code
 * 
 * Hydra_Software_Devel/4   4/27/11 11:47p farshidf
 * SWDTV-6190: move the L3 interrupt processing inside the PI
 * 
 * Hydra_Software_Devel/3   4/27/11 12:28p farshidf
 * SWDTV-6190: fix the max channel number
 * 
 * Hydra_Software_Devel/2   4/25/11 6:22p farshidf
 * SWDTV-6191: update the ADS callback
 * 
 * Hydra_Software_Devel/1   4/20/11 12:41p farshidf
 * SWDTV-6190: add intial DS files
 * 
 * 32   3/16/11 7:40p farshidf
 * SW3128-1: add an extra timer for status
 * 
 * 31   3/8/11 1:17p farshidf
 * SW3128-1: use one channel for ADS on 3461
 * 
 * 29   2/28/11 5:57p cbrooks
 * sw3128-1:Changed acqwords to internal_params
 * 
 * 28   2/8/11 11:48a farshidf
 * SW3128-1: update the constellation function
 * 
 * 27   2/7/11 5:08p mpovich
 * SW3128-1: Integrate HAB power control and status messages with ADS
 *  driver.  Update HAB driver (misc integ. updates and fixes).
 * 
 * Rom_Devel_3128/4   2/5/11 1:52p mpovich
 * SW3128-1: Rebase to main branch.
 * 
 * 26   2/5/11 1:07p farshidf
 * SW3128-1: add the auto acquire
 * 
 * Rom_Devel_3128/3   2/4/11 11:24a mpovich
 * SW3128-1: Keep power-up/off state of ADS channels.
 * 
 * Rom_Devel_3128/2   2/1/11 10:23a mpovich
 * SW3128-1: Rebase to the latest from the main branch.
 * 
 * 25   1/31/11 7:03p farshidf
 * SW3128-1: add the WFE to ADS callback sample
 * 
 * 24   1/31/11 12:55a mpovich
 * SW3128-1: Add HAB driver changes integrated with ADS and WFE modules.
 * 
 * Rom_Devel_3128/1   1/28/11 4:42p mpovich
 * SW3128-1: Add get/set functions to integrate ADS with HAB commands
 * 
 * 23   1/26/11 4:13p farshidf
 * SW3128-1: clean up
 * 
 * 22   1/26/11 3:28p cbrooks
 * sw3128-1:Cleanup Code
 * 
 * 21   1/25/11 9:29p cbrooks
 * sw3128-1: Cleanup Code
 * 
 * 20   1/21/11 5:19p farshidf
 * SW3128-1: update the names
 * 
 * 19   1/18/11 6:19p farshidf
 * SW3128-1: add channel irq handle
 * 
 * 18   1/17/11 3:01p farshidf
 * SW3128-1: add the ADS irq
 * 
 * 17   12/22/10 2:21p farshidf
 * SW3128-1: update
 * 
 * 16   12/21/10 2:17p farshidf
 * SW3128-1: update the API
 * 
 * 15   12/20/10 4:40p farshidf
 * SW3128-1: update
 * 
 * 14   12/18/10 10:40a farshidf
 * SW3128-1: update
 * 
 * 13   12/17/10 4:17p farshidf
 * SW3128-1: update
 * 
 ***************************************************************************/
#ifndef BADS_API_H__
#define BADS_API_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "bads_def.h"
#include "bads_struct.h"

#ifndef LEAP_BASED_CODE 
#define BREG_WriteField(h, Register, Field, Data) \
  BREG_Write32(h, BCHP_##Register, ((BREG_Read32(h, BCHP_##Register) & \
  ~((uint32_t)BCHP_MASK(Register, Field))) | \
  BCHP_FIELD_DATA(Register, Field, Data)))


#define BREG_ReadField(h, Register, Field) \
  ((BREG_Read32(h, BCHP_##Register) & BCHP_MASK(Register,Field)) >> \
  BCHP_SHIFT(Register,Field))


#define CHK_RETCODE( rc, func )             \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define DEV_MAGIC_ID            ((BERR_ADS_ID<<16) | 0xFACE)
#endif



#define MX_ADS_CHANNELS         (1)


#define BADS_CHSCN_EVENT		7
#define BADS_FCA_EVENT			6
#define BADS_DS0_2_EVENT		1
#define BADS_IQ_CONSTELLATION_VECTORS   30

typedef struct BADS_P_3x7x_Handle               *BADS_3x7x_Handle;
typedef struct BADS_P_3x7x_ChannelHandle        *BADS_3x7x_ChannelHandle;

typedef enum BADS_FecLock
{
	/* Most of these are currently not supported, are here for future use */
    BADS_FecLock_eNoChange = 0,
	BADS_FecLock_eNotLocked = 2,
	BADS_FecLock_eLocked = 3,
  BADS_FecLock_eLast
} BADS_FecLock;

/* Channel power on/off status */
typedef enum 
{
    BADS_eChan_Power_Off = 0,
    BADS_eChan_Power_On  = 1,
    BADS_eChan_Power_Unknown = 0xFF
} BADS_3x7x_Chan_PowerStatus;

/***************************************************************/
/***************************************************************/

/***************************************************************************
* HAB Function call
****************************************************************************/
/***************************************************************************
Summary:
    This function opens Qam In-Band Downstream module.

Description:
    This function is responsible for opening BADS module. When BADS is
    opened, it will create a module handle and configure the module based
    on the default settings. Once the device is opened, it must be closed
    before it can be opened again.

Returns:
    TODO:

See Also:
    BADS_3x7x_Close(), BADS_3x7x_OpenChannel(), BADS_3x7x_CloseChannel(),
    BADS_3x7x_GetDefaultSettings()

****************************************************************************/
BERR_Code BADS_3x7x_Open(
    BADS_Handle *pAds,                  /* [out] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle, Bcm3250 */
    const struct BADS_Settings *pDefSettings    /* [in] Default settings */
    );

/***************************************************************************
Summary:
    This function closes Qam In-Band Downstream module.

Description:
    This function is responsible for closing BADS module. Closing BADS
    will free main BADS handle. It is required that all opened
    BDQS channels must be closed before calling this function. If this
    is not done, the results will be unpredicable.

Returns:
    TODO:

See Also:
    BADS_3x7x_Open(), BADS_3x7x_CloseChannel()

****************************************************************************/
BERR_Code BADS_3x7x_Close(
    BADS_Handle hDev                    /* [in] Device handle */
    );

/***************************************************************************
Summary:
    This function initialize Qam In-Band Downstream module.

Description:
    This function is responsible for initializing BADS module. The initialize
    function may do none or one or more of the following:
    - Download FW into ADS core
    - Startup ADS core
    - Retrieve information from ADS core regarding ADS static configuration
    - etc

Returns:
    TODO:

See Also:
    BADS_3x7x_Open(), BADS_3x7x_Close()

****************************************************************************/
BERR_Code BADS_3x7x_Init(
    BADS_Handle hDev                    /* [in] Device handle */
    );

/***************************************************************************
Summary:
    This function returns the version information.

Description:
    This function is responsible for returning the version information.

Returns:
    TODO:

See Also:
    BADS_3x7x_Open()

****************************************************************************/
BERR_Code BADS_3x7x_GetVersion(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_Version *pVersion              /* [out] Returns version */
    );

/***************************************************************************
Summary:
    This function returns the total number of channels supported by
    Qam In-Band Downstream module.

Description:
    This function is responsible for getting total number of channels
    supported by BADS module, since BADS device is implemented as a
    device channel.

Returns:
    TODO:

See Also:
    BADS_3x7x_OpenChannel(), BADS_3x7x_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BADS_3x7x_GetTotalChannels(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    );

/***************************************************************************
Summary:
    This function opens Qam In-Band Downstream module channel.

Description:
    This function is responsible for opening BADS module channel. When a
    BADS channel is opened, it will create a module channel handle and
    configure the module based on the channel default settings. Once a
    channel is opened, it must be closed before it can be opened again.

Returns:
    TODO:

See Also:
    BADS_3x7x_CloseChannel(), BADS_3x7x_GetChannelDefaultSettings()

****************************************************************************/
BERR_Code BADS_3x7x_OpenChannel(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_ChannelHandle *phChn,          /* [out] Returns channel handle */
    unsigned int channelNo,             /* [in] Channel number to open */
    const struct BADS_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
    );

/***************************************************************************
Summary:
    This function closes Qam In-Band Downstream module channel.

Description:
    This function is responsible for closing BADS module channel. Closing
    BADS channel it will free BADS channel handle. It is required that all
    opened BDQS channels must be closed before closing BADS.

Returns:
    TODO:

See Also:
    BADS_3x7x_OpenChannel(), BADS_3x7x_CloseChannel()

****************************************************************************/
BERR_Code BADS_3x7x_CloseChannel(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function gets Qam In-Band Downstream module device handle based on
    the device channel handle.

Description:
    This function is responsible returning BADS module handle based on the
    BADS module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_3x7x_GetDevice(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Handle *pQds                   /* [out] Returns Device handle */
    );


/***************************************************************************
Summary:
    This function gets default setting for a Qam In-Band Downstream module channel.

Description:
    This function is responsible for returning the default setting for
    channel of BADS. The return default setting is used when opening
    a channel.

Returns:
    TODO:

See Also:
    BADS_3x7x_OpenChannel()

****************************************************************************/
BERR_Code BADS_3x7x_GetChannelDefaultSettings(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int channelNo,             /* [in] Channel number to default setting for */
    BADS_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
    );

/***************************************************************************
Summary:
    This function gets the status of Qam In-Band Downstream module channel.

Description:
    This function is responsible for getting the complete status for
    a Qam In-Band Downstream module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_3x7x_GetStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    );

/***************************************************************************
Summary:
    This function gets the lock status for a Qam In-Band Downstream
    module channel.

Description:
    This function is responsible for getting the lock status
    for a BADS module channel.

Returns:
    TODO:

See Also:
    BADS_3x7x_GetStatus()

****************************************************************************/
BERR_Code BADS_3x7x_GetLockStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_LockStatus *pLockStatus         /* [out] Returns lock status */
    );

/***************************************************************************
Summary:
    This function gets the I and Q values for soft decision of a
    Qam In-Band Downstream module channel.

Description:
    This function is responsible for getting the I and Q values for soft
    decision of a BADS module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_3x7x_GetSoftDecision(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *ival,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    );


/***************************************************************************
Summary:
    This function installs a callback function for Lock State Change event.

Description:
    This function is responsible for installing a callback function for
    Lock State Change event.  The application code should use this function
    to install a callback function, which will be called when the
    Qam In-Band Downstream channel changes lock state.
    A lock state change is defined at switching from Lock-Unlock or Unlock-Lock.
    To determine the current lock state, a call to BADS_GetLockStatus() is
    required. To get more a more detail status, call BADS_GetStatus().

    Note: It is "highly" recommended that the callback function do the minimum
    require to notify the application of this event, such sent a message or
    fire an event.  This callback function may be called from an
    interrupt context.  Please use with caution.

Returns:
    TODO:

See Also:
    BADS_3x7x_GetLockStatus(), BADS_3x7x_GetStatus()

****************************************************************************/
BERR_Code BADS_3x7x_InstallCallback(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Callback callbackType,         /* [in] type of Callback */
    BADS_CallbackFunc pCallbackFunc,    /* [in] Pointer to completion callback. */
    void *pParam                        /* [in] Pointer to callback user data. */
    );

/***************************************************************************
Summary:
    This function tries to acquire downstream lock for the specific
    Qam In-Band Downstream module channel.

Description:
    This function is responsible for trying to acquire downstream lock of
    the input IF signal. Acquiring downstream lock involves configuring
    the H/W to desire configuration, then running a Qam In-Band Downstream
    acquisition script. If this is the first acquisition for the current
    annex mode, then a Qam In-Band Downstream configuration script will be run
    prior to running acquisition script.
    This function will automatically enable the downstream receiver if
    the receiver was in power-saver mode.

Returns:
    TODO:

See Also:
    BADS_3x7x_GetLock(), BADS_3x7x_GetStatus(), BADS_3x7x_GetSoftDecision()

****************************************************************************/
BERR_Code BADS_3x7x_Acquire(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_InbandParam *ibParam           /* [in] Inband Parameters to use */
    );

/***************************************************************************
Summary:
    This function enable the power-saver mode.

Description:
    This function is responsible for enabling the downstream receiver
    power-saver mode.  When the BADS is in the power-saver mode, the
    Qam In-Band Downstream receiver is shutdown.

Returns:
    TODO:

See Also:
    BADS_3x7x_EnablePowerSaver()

****************************************************************************/
BERR_Code BADS_3x7x_EnablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    );


/***************************************************************************
Summary:
    This function disable the power-saver mode.

Description:
    This function is responsible for enabling the downstream receiver
    power-saver mode.  When the BADS is in the power-saver mode, the
    Qam In-Band Downstream receiver is shutdown.

Returns:
    TODO:

See Also:
    BADS_3x7x_DisablePowerSaver()

****************************************************************************/
BERR_Code BADS_3x7x_DisablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function is responsible for processing a notificiation for the specific
    Qam In-Band Downstream module channel.

Description:
    This function needs to called when notification is received.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_3x7x_ProcessNotification(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    unsigned int event                  /* [in] Event code and event data*/
    );

/***************************************************************************
Summary:
	This function opens configures (enables/disables) the 75xx device's 
	RF out for daisy chaining.

Description:
	This function opens configures (enables/disables) the 75xx device's 
	RF out for daisy chaining.
	
Returns:
	TODO:

See Also:	
****************************************************************************/
BERR_Code BADS_3x7x_SetDaisyChain(
	BADS_Handle hDev,		/* [in] Returns handle */
	bool enableDaisyChain	/* [in] Eanble/disable daisy chain. */
	);

/***************************************************************************
Summary:
	This function opens tells if the 75xx device's RF out daisy chaining is enabled/disabled.

Description:
	This function opens tells if the 75xx device's RF out daisy chaining is enabled/disabled.
	
Returns:
	TODO:

See Also:	
****************************************************************************/

BERR_Code BADS_3x7x_GetDaisyChain(
	BADS_Handle hDev,			/* [in] Returns handle */
	bool *isEnableDaisyChain	/* [in] Eanble/disable daisy chain. */
	);

/***************************************************************************
Summary:
	This function resets the 75xx device's FEC bit error and block counters.

Description:
	
Returns:
	TODO:

See Also:	
****************************************************************************/
BERR_Code BADS_3x7x_ResetStatus(
	 BADS_ChannelHandle hDev  			/* [in] Returns handle */
	);


/***************************************************************************
Summary:
	Get the Event handle

Description:
	
Returns:
	TODO:

See Also:	
****************************************************************************/

BERR_Code BADS_3x7x_GetInterruptEventHandle(
   BADS_Handle hDev,            /* [in] BADS handle */
   BKNI_EventHandle *phEvent /* [out] event handle */
);

/***************************************************************************
Summary:
	Process the Interrupt event

Description:
	
Returns:
	TODO:

See Also:	
****************************************************************************/
BERR_Code BADS_3x7x_ProcessInterruptEvent(
    BADS_Handle hDev           /* [in] Returns handle */
    );

/***************************************************************************
Summary:
	Stop the ADS timer

Description:
	
Returns:
	TODO:

See Also:	
****************************************************************************/
BERR_Code BADS_3x7x_Untune(
    BADS_Handle hDev           /* [in] Returns handle */
    );
/***************************************************************************
Summary:
	Set the Scan param

Description:
	
Returns:
	TODO:

See Also:	
****************************************************************************/
BERR_Code BADS_3x7x_SetScanParam(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_ChannelScanSettings *pChnScanSettings                 
    );
/***************************************************************************
Summary:
	Get the Scan param

Description:
	
Returns:
	TODO:

See Also:	
****************************************************************************/
BERR_Code BADS_3x7x_GetScanParam(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_ChannelScanSettings *pChnScanSettings                 
    );

void BADS_3x7x_TimerFunc(void *myParam1, int myParam2);
BERR_Code BADS_3x7x_SetAcquireParams(BADS_3x7x_ChannelHandle hChn, const BADS_Acquire_Params_t* pMesgData);
BERR_Code BADS_3x7x_SetScanParams(BADS_3x7x_ChannelHandle hChn, const BADS_Scan_Params_t* pMesgData);
BERR_Code BADS_3x7x_SetAcqWords(BADS_3x7x_ChannelHandle hChn, const BADS_Internal_Params_t* pMesgData);


/***************************************************************************
Summary:
    Data for the ADS  callback
****************************************************************************/
/*This is a single NON-channelized structure called on demand then sent through the HAB*/
/*Each sub-structure is completely filled by an appropriate function call*/

typedef enum BADS_CallbackMode
{
	/* Most of these are currently not supported, are here for future use */
    BADS_CallbackMode_eSetMode = 0,
	BADS_CallbackMode_eRequestMode = 1,
    BADS_CallbackMode_eLast
} BADS_CallbackMode;


typedef struct BADS_P_AdsCallbackData_s
{
	void*			            hTunerChn;
	uint16_t		          Mode;										/*Callback is to send data on eSetMode, Callback is to get data on eRequestMode*/
	uint32_t              Symbol_Rate;            /*Upper Symbol_Rate currently being scanned on eSetMode: return last value sent on eRequestMode*/
	int32_t		            Freq_Offset;					  /*Front end offset to use on eSetMode: return last value sent on eRequestMode*/
	uint32_t		          RF_Freq;						    /*RF frequency of the tuner on eRequestMode: set to 0 if unknown*/
	int32_t		            Total_Mix_After_ADC;    /*Sum of mixer frequencies after ADC on eRequestMode*/
	int16_t		            PreADC_Gain_x256db ;    /*Gain in db*256 before ADC on eRequestMode: set to 0x8000 if unknown*/
	int16_t		            PostADC_Gain_x256db;    /*Gain in db*256 after ADC on eRequestMode: set to 0x8000 if unknown*/
	int16_t		            External_Gain_x256db;   /*Gain in db*256 external to chip (like external LNA) on eRequestMode: set to 0x8000 if unknown*/
}BADS_P_AdsCallbackData_t;

/***************************************************************/
/***************************************************************/

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/


typedef struct BADS_P_3x7x_Handle
{
	uint32_t			        magicId;                    /* Used to check if structure is corrupt */
    BCHP_Handle					hChip;
	BINT_Handle					hInterrupt;
    BREG_Handle					hRegister;
	unsigned int		        mxChnNo;
	BADS_3x7x_ChannelHandle		hAdsChn[MX_ADS_CHANNELS];
	bool				        isDaisyChain;
	BKNI_EventHandle			hIntEvent;
	bool				        timerStopped;
	BTMR_TimerHandle			hTimer;
	BTMR_TimerHandle			hStatusTimer;
	BADS_FecLock		        LockStatus[MX_ADS_CHANNELS];
} BADS_P_3x7x_Handle;


typedef struct BADS_P_3x7x_ChannelHandle
{
	uint32_t					    magicId;                      /* Used to check if structure is corrupt */
	BREG_Handle						hRegister;
	BADS_ChannelHandle				hChnAds;
	BADS_3x7x_Handle				hAds;
	unsigned int					chnNo;
	bool						    isLock;                       /* current lock status */
	BADS_3x7x_AcqParams_t			*pChnAcqParam;
	BADS_3x7x_ChnLockStatus_t		*pChnLockStatus;
    BADS_3x7x_ChnScanStatus_t     *pChnScanStatus;                  /* channel status obtained post acquire or post scan with BADS_3x7x_GetScanStatus() */
    BADS_3x7x_ChnStatus_t         *pChnStatus;                      /* channel status updated with BADS_3x7x_GetStatus() */

    BADS_3x7x_Chan_PowerStatus		ePowerStaus;                     /* channel power (on/off) status */
#if BCHP_DS_CORE_V_9_1 
	BKNI_EventHandle				hChscnEvent;
	BKNI_EventHandle				hFcaEvent;
#elif ((BCHP_DS_CORE_V_9_2) || (BCHP_DS_CORE_V_9_4))
	BKNI_EventHandle				hFftEvent;
#endif
    BADS_CallbackFunc				pCallback[BADS_Callback_eLast];
	void 							*pCallbackParam[BADS_Callback_eLast];
	BINT_CallbackHandle				hDs2Callback;
} BADS_P_3x7x_ChannelHandle;

#ifdef __cplusplus
}
#endif

#endif

