/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_leap_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 10/18/12 3:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/leap/bads_leap_priv.h $
 * 
 * Hydra_Software_Devel/17   10/18/12 3:43p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/15   9/28/12 2:17p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/14   6/21/12 3:21p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/13   6/18/12 5:15p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS directory
 * structure
 * 
 * Hydra_Software_Devel/12   2/14/12 10:42a atanugul
 * SW3128-119: Update SetAcquireParams API to make the second parameter as
 * const
 * 
 * Hydra_Software_Devel/11   2/8/12 5:14p atanugul
 * SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/10   1/10/12 4:57p atanugul
 * SW3128-107:Enable HAB CRC Check in PI
 * 
 * Hydra_Software_Devel/9   12/8/11 3:13p atanugul
 * SW3128-89: Add PI support for Spectrum Analyzer Data retrieval
 * 
 * Hydra_Software_Devel/8   11/16/11 4:25p atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/7   9/15/11 3:20p atanugul
 * SW3128-50: Add BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/6   8/30/11 11:19a atanugul
 * SW3128-47: Add Support for BCM3123
 * 
 * Hydra_Software_Devel/5   8/2/11 4:47p atanugul
 * SW3128-2: Fix BADS_3128_GetSoftDecision() and  rxSymbolRate in
 * BADS_3128_GetAsyncStatus(); code cleanup
 * 
 * Hydra_Software_Devel/4   5/16/11 2:51p atanugul
 * SW3128-1: Add scan support to 3128
 * 
 * Hydra_Software_Devel/3   4/11/11 6:33p atanugul
 * SW3128-2: Fix Power Status Opcode
 * 
 * Hydra_Software_Devel/2   3/28/11 6:15p atanugul
 * SW3128-2: code cleanup
 * 
 * Hydra_Software_Devel/1   3/4/11 3:15p vishk
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 * board) plug on 7422 board
 * 
 * Hydra_Software_Devel/3   3/2/11 6:08p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/2   2/2/11 12:46p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/1   2/1/11 7:44a atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 ***************************************************************************/
#ifndef BADS_leap_PRIV_H__
#define BADS_leap_PRIV_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "berr_ids.h"
#include "bads.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines raw HAB test mesg hdr (struct) */
#define HAB_MSG_HDR(OPCODE,N,CORE_TYPE) \
    { ((uint8_t)(((uint16_t)(OPCODE)) >> 2)), \
    (((uint8_t)(0x03 & (OPCODE)) << 6) | ((uint8_t)(((N)>>4)  & 0x3F))), \
    ((((uint8_t)(((N)& 0x0F) << 4))) | ((uint8_t)(0x0F & (CORE_TYPE)))) }
    
#define BADS_CORE_TYPE		0x1
#define CORE_TYPE_GLOBAL	0x0

typedef enum BADS_OpCodesDS{
        BADS_eAcquire = 0x10,
        BADS_eAcquireParamsWrite = 0x11,
        BADS_eAcquireParamsRead = 0x91,
        BADS_eAnnexASymbolRateWrite = 0x12,
        BADS_eAnnexASymbolRateRead = 0x92,
        BADS_eScanParamsWrite = 0x13,
        BADS_eScanParamsRead = 0x93,
        BADS_eAcqWordsWrite = 0x14,
        BADS_eAcqWordsRead = 0x94,
        BADS_eResetStatus = 0x15,
        BADS_eRequestAsyncStatus = 0x16,
        BADS_eGetAsyncStatus = 0x96,
        BADS_eGetScanStatus = 0x99,
        BADS_eGetConstellation = 0xA3,
        BADS_eGetVersion = 0xB9,
        BADS_eGetVersionInfo = 0xBA,
        BADS_ePowerCtrlOn = 0x19,
        BADS_ePowerCtrlOff = 0x18,
        BADS_ePowerCtrlRead = 0x98,
        BADS_eReqSpectrumAnalyzerData = 0x122,
        BADS_eGetSpectrumAnalyzerData = 0x322
}BADS_OpCodesDS;

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
    BADS_Leap_Close(), BADS_Leap_OpenChannel(), BADS_Leap_CloseChannel(),
    BADS_Leap_GetDefaultSettings()

****************************************************************************/
BERR_Code BADS_Leap_Open(
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
    BADS_Leap_Open(), BADS_Leap_CloseChannel()

****************************************************************************/
BERR_Code BADS_Leap_Close(
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
    BADS_Leap_Open(), BADS_Leap_Close()

****************************************************************************/
BERR_Code BADS_Leap_Init(
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
    BADS_Leap_Open()

****************************************************************************/
BERR_Code BADS_Leap_GetVersion(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_Version *pVersion              /* [out] Returns version */
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
    BADS_Leap_Open()

****************************************************************************/    
BERR_Code BADS_Leap_GetVersionInfo(
    BADS_Handle hDev,                    /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
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
    BADS_Leap_OpenChannel(), BADS_Leap_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BADS_Leap_GetTotalChannels(
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
    BADS_Leap_CloseChannel(), BADS_Leap_GetChannelDefaultSettings()

****************************************************************************/
BERR_Code BADS_Leap_OpenChannel(
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
    BADS_Leap_OpenChannel(), BADS_Leap_CloseChannel()

****************************************************************************/
BERR_Code BADS_Leap_CloseChannel(
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
BERR_Code BADS_Leap_GetDevice(
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
    BADS_Leap_OpenChannel()

****************************************************************************/
BERR_Code BADS_Leap_GetChannelDefaultSettings(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int channelNo,             /* [in] Channel number to default setting for */
    BADS_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
    );

/***************************************************************************
Summary:
    This function gets the status synchronously of Qam In-Band Downstream module channel.

Description:
    This function is responsible for getting the complete status synchronously for
    a Qam In-Band Downstream module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_Leap_GetStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    );

/***************************************************************************
Summary:
    This function requests the status asynchronously of Qam In-Band Downstream module channel.

Description:
    This function is responsible for requesting the status to be calculated asynchronously for
    a Qam In-Band Downstream module channel. The Qam frontend is responsible to inform 
    the backend when the status is ready either through an interrupt or by any other predetermined
    method.
    
Returns:
    TODO:

See Also: BADS_GetStatus

****************************************************************************/
BERR_Code BADS_Leap_RequestAsyncStatus(
    BADS_ChannelHandle hChn            /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function gets the status asynchronously of Qam In-Band Downstream module channel.

Description:
    This function is responsible for getting the complete status asynchronously for
    a Qam In-Band Downstream module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_Leap_GetAsyncStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    );

/***************************************************************************
Summary:
    This function gets the scan status synchronously for a Qam In-Band 
    Downstream module channel.

Description:
    This function is responsible for getting the scan status synchronously for
    a Qam In-Band Downstream module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_Leap_GetScanStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_ScanStatus *pScanStatus        /* [out] Returns status */
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
    BADS_Leap_GetStatus()

****************************************************************************/
BERR_Code BADS_Leap_GetLockStatus(
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
BERR_Code BADS_Leap_GetSoftDecision(
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
    BADS_Leap_GetLockStatus(), BADS_Leap_GetStatus()

****************************************************************************/
BERR_Code BADS_Leap_InstallCallback(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Callback callbackType,         /* [in] type of Callback */
    BADS_CallbackFunc pCallbackFunc,    /* [in] Pointer to completion callback. */
    void *pParam                        /* [in] Pointer to callback user data. */
    );

    
/***************************************************************************
Summary:
	This function returns the default settings for Qam In-Band Downstream module.

Description:
	This function is responsible for returns the default setting for 
	BADS module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BADS_Leap_Open()

****************************************************************************/
BERR_Code BADS_Leap_GetDefaultAcquireParams(
    BADS_InbandParam *ibParams          /* [out] default Inband Parameters */
    );
    
    
/***************************************************************************
Summary:
    This function sends the acquire parameters for a specific ADS module.

Description:
    This function sends the acquire parameters for a specific ADS module.
    This sets the acquire parameters for a specific acquire.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_Leap_SetAcquireParams(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    const BADS_InbandParam *ibParams          /* [in] Inband Parameters to use */
    );

    
/***************************************************************************
Summary:
    This function retrieves the acquire parameters set for a specific ADS module.

Description:
    This function gets the acquire parameters for a specific ADS module.

Returns:
    TODO:

See Also:

****************************************************************************/ 
BERR_Code BADS_Leap_GetAcquireParams(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    BADS_InbandParam *ibParams          /* [out] Inband Parameters to use */
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
    BADS_Leap_GetLock(), BADS_Leap_GetStatus(), BADS_Leap_GetSoftDecision()

****************************************************************************/
BERR_Code BADS_Leap_Acquire(
    BADS_ChannelHandle hChn,             /* [in] Device channel handle */
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
    BADS_Leap_Acquire()

****************************************************************************/
BERR_Code BADS_Leap_EnablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function disables the power-saver mode.

Description:
    This function is responsible for disabling the downstream receiver
    power-saver mode.  When the BADS is in the power-saver mode, the
    Qam In-Band Downstream receiver is shutdown.

Returns:
    TODO:

See Also:
    BADS_Leap_EnablePowerSaver()
    BADS_Leap_Acquire()

****************************************************************************/
BERR_Code BADS_Leap_DisablePowerSaver(
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
BERR_Code BADS_Leap_ProcessNotification(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    unsigned int event                  /* [in] Event code and event data*/
    );

/***************************************************************************
Summary:
    This function opens configures (enables/disables) the leap device's 
    RF out for daisy chaining.

Description:
    This function opens configures (enables/disables) the leap device's 
    RF out for daisy chaining.
    
Returns:
    TODO:

See Also:   
****************************************************************************/
BERR_Code BADS_Leap_SetDaisyChain(
    BADS_Handle hDev,       /* [in] Device channel handle */
    bool enableDaisyChain   /* [in] Eanble/disable daisy chain. */
    );

/***************************************************************************
Summary:
    This function opens tells if the leap device's RF out daisy chaining is enabled/disabled.

Description:
    This function opens tells if the leap device's RF out daisy chaining is enabled/disabled.
    
Returns:
    TODO:

See Also:   
****************************************************************************/

BERR_Code BADS_Leap_GetDaisyChain(
    BADS_Handle hDev,           /* [in] Device channel handle */
    bool *isEnableDaisyChain    /* [in] Eanble/disable daisy chain. */
    );

/***************************************************************************
Summary:
    This function resets the leap device's FEC bit error and block counters.

Description:
    
Returns:
    TODO:

See Also:   
****************************************************************************/
BERR_Code BADS_Leap_ResetStatus(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function performs an i2c read from the slave attached to the leap's i2c bus.

Description:
    
Returns:
    TODO:

See Also:   
****************************************************************************/
BERR_Code BADS_Leap_ReadSlave(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */
	uint8_t chipAddr,			 /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,			 /* [in] sub addr of the register to read from the slave device */
	uint8_t subAddrLen,			 /* [in] how many bytes is the sub addr? one to four*/
	uint32_t *data,				 /* [out] ptr to the data that we will read from the slave device */
	uint8_t dataLen				 /* [in] how many bytes are we going to read? one to four*/
    );

/***************************************************************************
Summary:
    This function performs an i2c write to the slave attached to the leap's i2c bus.

Description:
    
Returns:
    TODO:

See Also:   
****************************************************************************/
BERR_Code BADS_Leap_WriteSlave(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */
	uint8_t chipAddr,			 /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,			 /* [in] sub addr of the register to read from the slave device */
	uint8_t subAddrLen,			 /* [in] how many bytes is the sub addr? one to four*/
	uint32_t *data,				 /* [in] ptr to the data that we will write to the slave device */
	uint8_t dataLen				 /* [in] how many bytes are we going to write? one to four*/
    );

/***************************************************************************
Summary:
	Set the Scan param

Description:
	
Returns:
	TODO:

See Also:	
****************************************************************************/
BERR_Code BADS_Leap_SetScanParam(
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
BERR_Code BADS_Leap_GetScanParam(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_ChannelScanSettings *pChnScanSettings                 
    );

/******************************************************************************
Summary:
   This function sends request for spectrum analyzer data to the LEAP.
Description:
  
Returns:
   BERR_Code

See Also: BADS_GetSpectrumAnalyzerData   
******************************************************************************/
BERR_Code BADS_Leap_RequestSpectrumAnalyzerData(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
    BADS_SpectrumSettings *pSettings /* [in] spectrum settings */
    );

/******************************************************************************
Summary:
   This function gets spectrum analyzer data from the LEAP.
Description:
  
Returns:
   BERR_Code

See Also: BADS_RequestSpectrumAnalyzerData
******************************************************************************/     
BERR_Code BADS_Leap_GetSpectrumAnalyzerData(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
    BADS_SpectrumData  *pSpectrumData /* [out] spectrum Data*/
    );    
   
#ifdef __cplusplus
}
#endif

#endif



