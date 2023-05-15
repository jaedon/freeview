/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpwr.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/21/12 6:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/include/bpwr.h $
 * 
 * Hydra_Software_Devel/1   9/21/12 6:34p nickh
 * SW7425-3971: Add common include files
 * 
 * Hydra_Software_Devel/2   5/15/09 11:59a erickson
 * PR55154: rework
 *
 * Hydra_Software_Devel/1   7/7/08 4:24p garylin
 * PR44622: initial version, ported from 7325
 *
 ***************************************************************************/
#ifndef BPWR_H__
#define BPWR_H__

#include "bstd_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************ Module Overview ********************************
The Power Management API is a library that allows the shutting down and
subsequent power up of specified hardware blocks.

This API is targeted for Hydra core based chip, and future chips.  This API
exploits all possible power management modes provided by chip capabilities.
It also attempts to hide all the gritty hardware details from the application.

The API operates on a context handle.  There is a main PWR handle which
represents a certain state of the power management software object. This object
may contain information regarding the hardware blocks that are currently
active and RTS settings, if any of inactive blocks that were previously
shut down.

Most of the API functions are used in user mode context.  It is assumed that
there are no critical sections in the a software module whose
corespodning hardware blocks are shutdown. As such any register writes
are allowed without going through critical sections.

In the user mode context, after one has completed the intended setup for a
ppwer management context, one "applies" it to the hardware.  It is only at this time
that the actual configuration (i.e. register writes or queues) of the
hardware occurs.  Modifying a power management context does not affect the
hardware at all until it is applied.  Please notice that the setup calls
between two "applyings" is order independent, and that if there is conflict
found as "applying" all new settings would not be applied.

For more details, please refer to the description of BPWR_Handle.
***************************************************************************/

/***************************************************************************
 * PWR data types
 **************************************************************************/

/***************************************************************************
Summary:
	This enum lists the possible power modes for a given module

Description:
	BPWR_ePowerMode is an enumeration that lists the possible power
	modes for a given PI module. A module's initial mode is undefined and
	is set via BPWR_SetMode or BPWR_SetAllMode.

***************************************************************************/
typedef enum BPWR_ePowerMode
{
	BPWR_Mode_ePowerDown = 0,
	BPWR_Mode_ePowerUp,
	BPWR_Mode_ePowerUndefined
} BPWR_ePowerMode;

/***************************************************************************
Summary:
	This structure describes the settings for the power management.

Description:
	BPWR_Settings is a structure that use to describe the public
	settings for power management.

	This structure is not used in 7405. The code will read from Sun_Top+Ctrl_Strap to figure out
	the memory configuration. The portinginterface only handle MEMC 1 for non-UMA memory
	configuration.

	ulMemc0ClientStartAddr - address of MEMC0's 1st client
	ulMemc0ClientCount     - number of MEMC0 clients
	ulMemc1ClientStartAddr - address of MEMC1's 1st client
	ulMemc1ClientCount     - number of MEMC1 clients
	ulMemc2ClientStartAddr - address of MEMC2's 1st client
	ulMemc2ClientCount     - number of MEMC2 clients

	The default values for all the fields is 0; hence, these must be defined
	by the application. If none of the start addresses are defined, an error
	will be returned.

***************************************************************************/
typedef struct BPWR_Settings
{
	uint32_t ulMemc0StartAddr;
	uint32_t ulMemc0ClientCount;
	uint32_t ulMemc1StartAddr;
	uint32_t ulMemc1ClientCount;
	uint32_t ulMemc2StartAddr;
	uint32_t ulMemc2ClientCount;
} BPWR_Settings;

/***************************************************************************
 * PWR handles
 **************************************************************************/

/***************************************************************************
Summary:
	Main Power Management (PWR) context handle.

Description:
	This is an opaque handle that application created with BPWR_Open.
	BPWR_Handle holds the context of the Power Management module.  There
	should only one BPWR_Handle per chip.

See Also:
	BPWR_Open, BPWR_Close.
***************************************************************************/
typedef struct BPWR_P_Context          *BPWR_Handle;


/***************************************************************************
 * Public API
 **************************************************************************/

/***************************************************************************
 * Function prototype
 **************************************************************************/
/***************************************************************************
Summary:
	This function gets BPWR's inherent default setting structure.

Description:
	BPWR's inherent default setting structure could be queried by this API
	function prior to BPWR_Open, modified and then passed to BPWR_Open.
	This save application tedious work of filling in the configuration
	structure.

Input:
	<None>

Output:
	pDefSettings - A reference to default settings structure.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully get PWR default settings.

See Also:
	BPWR_Open.
**************************************************************************/
BERR_Code BPWR_GetDefaultSettings
	( BPWR_Settings                   *pDefSettings );

/***************************************************************************
Summary:
	This function opens and initializes the power management object.

Description:
	The first thing one need to do is open a PWR (also create a PWR
	main context handle, BPWR_Handle).  This will hold the states
	of different hardware blocks.  From this handle, the user can
	shutdown and/or power up desired hardware blocks. This handle is
	required when we call BPWR_ApplyChanges to update the hardware.
	This handle eventually be closed by calling BVDC_Close.

Input:
	hChip - BCHP handle
	hReg - BREG handle
	pDefSettings - The default setting that user want the BPWR to be
	in. This parameter can be NULL. In this case BPWR's inherent default
	structure will be used. This inherent structure could be queried prior
	to BPWR_Open with BPWR_GetDefaultSettings, modified and passed to
	BPWR_Open.

Output:
	phPwr - a reference to a PWR handle.  Upon successful open this will
	reference to a fresh new PWR handle (context).  If error occur during
	BPWR_Open *phPwr be NULL.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened PWR.

See Also:
	BPWR_Close, BPWR_GetDefaultSettings.
**************************************************************************/
BERR_Code BPWR_Open
	( BPWR_Handle					  *phPwr,
	  BCHP_Handle					   hChip,
	  BREG_Handle					   hRegister,
	  const BPWR_Settings			  *pDefSettings );

/***************************************************************************
Summary:
	This function closes the power management object..

Description:
	Upon terminating power managemment, the application needs to call BPWR_Close
	to close the power management object. After close, PWR is an invalid handle,
	and should not be use with any other PWR functions.  Failure to close will
	result in undefined	consequences. BPWR_Close returns success if hPwr is NULL.

Input:
	hPwr - Power management handle that was previously opened by
	BPWR_Open.

Output:
	hPwr - Power management handle becomes invalid.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully closed PWR.

See Also:
	BPWR_Open.
**************************************************************************/
BERR_Code BPWR_Close
	( BPWR_Handle                      hPwr );

/***************************************************************************
Summary:
	This function "Apply Changes" and update the desired hardware in the
	power management object.

Description:
	After all settings in the user mode context have been made which
	include configuring the the hardware blocks clocks and other parameters,
	BPWR_ApplyChanges will "apply" the "changes" to hardware.  It is
	possible the the context handle is not setup correctly.  In that case an
	error will return, and no hardware will be updated.  The hardware it
	updates will depend on the software modules specified by BPWR_SetMode.
	This function is optimized to only update required registers.

	BPWR_ApplyChanges actually uses register reads and writes.

Input:
	hPwr - PowerManagement handle that was previously opened by
	BPWR_Open, and setup with BPWR calls.

Output:
	<None>

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully updated hardware.

See Also:
	BREG_Handle.
**************************************************************************/
BERR_Code BPWR_ApplyChanges
	( BPWR_Handle                      hPwr );

/***************************************************************************
Summary:
	This function specifies which software module need to be shutdown or
	powered up.

Description:

Input:
	hPwr  - A valid power management handle created ealier.
	module - The software module to be shutdown.
	eMode - BPWR_Mode_ePowerUp, BPWR_Mode_ePowerUp

Output:
	None.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Function succeed

See Also:
	BPWR_ApplyChanges, BPWR_ePowerMode
**************************************************************************/
BERR_Code BPWR_SetMode
	( const BPWR_Handle                hPwr,
	  BSTD_Module                      module,
	  BPWR_ePowerMode                  eMode );

/***************************************************************************
Summary:
	This function gets the mode of a given module

Description:

Input:
	hPwr  - A valid power management handle created ealier.
	module - The software module to be shutdown.

Output:
	peMode - the mode of the module

Returns:
	BERR_NOT_SUPPORTED - Module is not supported.
	BERR_SUCCESS - Function succeed

See Also:
	BPWR_ApplyChanges, BPWR_ePowerMode
**************************************************************************/
BERR_Code BPWR_GetMode
	( const BPWR_Handle 			   hPwr,
	  BSTD_Module					   module,
	  BPWR_ePowerMode				  *peMode );

/***************************************************************************
Summary:
	This function gets the number of modules

Description:

Input:
	hPwr  - A valid power management handle created ealier.

Output:
	pulModuleCount - the number of modules

Returns:
	BERR_SUCCESS - Function succeed. This function cannot fail.

See Also: BPWR_GetModuleList
**************************************************************************/

BERR_Code BPWR_GetModuleCount
	( BPWR_Handle                      hPwr,
	  uint32_t	                      *pulModuleCount );


/***************************************************************************
Summary:
	This function gets the list of available modules

Description:

Input:
	hPwr  - A valid power management handle created ealier.

Output:
	pModuleList - the list of modules
	ulModuleCount - number of entries in pModuleList array

Returns:
	BERR_NOT_SUPPORTED - ulModuleCount not large enough hold all modules for this chip
	BERR_SUCCESS - Function succeed

See Also: BPWR_GetModuleCount, BSTD_Module
**************************************************************************/
BERR_Code BPWR_GetModuleList
	( BPWR_Handle                      hPwr,
	  BSTD_Module                     *pModuleList,
	  unsigned                         ulModuleCount );


/***************************************************************************
Summary:
	This function sets the modes of the modules in a given list

Description:

Input:
	hPwr  - A valid power management handle created ealier.
	pModuleList - the list of modules
	ulModuleCount - the number of modules
	eMode - the power mode

Output:

Returns:
	BERR_NOT_SUPPORTED - Module is not supported.
	BERR_SUCCESS - Function succeed

See Also:
	BPWR_ApplyChanges, BPWR_ePowerMode, BSTD_Module
**************************************************************************/
BERR_Code BPWR_SetModes
	( BPWR_Handle	                  hPwr,
	  BSTD_Module	                 *pModuleList,
	  uint32_t		                  ulModuleCount,
	  BPWR_ePowerMode                 eMode );



#ifdef __cplusplus
}
#endif

#endif /* #ifndef BPWR_H__ */

/* End of File */
