/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_hifidac.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 7/21/08 2:23p $
*
* Module Description:
*   Module name: HIFIDAC
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the top level FMM abstraction, which are exposed to the 
*   application developer.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_hifidac.h $
* 
* Hydra_Software_Devel/1   7/21/08 2:23p speter
* PR 44984: [3548,3556,7405] Adding files from base which have new
* Architecture specific changes and cannot be merged back in base
* 
* Hydra_Software_Devel/4   2/5/07 12:01p rjain
* PR 27477: Change BRAP_OP_DacSetVolume/BRAP_OP_DacGetVolume to
* BRAP_OP_SetDacVolume/BRAP_OP_GetDacVolume according to the Magnum
* naming convention BRAP_VerbNoun()
* 
* Hydra_Software_Devel/3   2/2/07 3:48p rjain
* PR 27477: Make BRAP_OP_P_DacSetVolume() and BRAP_OP_P_DacGetVolume()
* public
* 
* Hydra_Software_Devel/1   9/13/05 7:43p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/2   8/31/05 6:40p bselva
* PR 16148: Adding DAC support
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   8/1/05 4:48p bmishra
* PR 16148: Initial version of the file
* 
* 
***************************************************************************/


#ifndef _BRAP_HIFIDAC_H_
#define _BRAP_HIFIDAC_H_

#ifdef __cplusplus
extern "C" {
#endif



/***************************************************************************
Summary:
	Set the new volume level for the selected HiFiDAC.

Description:
	This function sets audio volume for HiFi DAC
	HiFi DAC volume control is linear in hardware ranging from 0
	min to 1FFFF max volume.  This PI has done a mapping
	from this linear range to 1/100 of DB.
	This function gets values in 1/100 of DB from 0 max to 10200 1/100 DB min,
	and for all values above 10200 the volume will be set to 0 linear
	or -102 DB.  Note: For fractions in DB a linear interpolation is used.
	PI maintains this volume upon channel change.

Returns:
	BERR_SUCCESS 

See Also:
	BRAP_OP_GetDacVolume()
	BRAP_MIXER_SetOutputVolume()

Note: 
    The function BRAP_MIXER_SetOutputVolume() can also be used to set the DAC 
    volume. It is a generic function which can be used for any output port. It 
    works by scaling the data inside the Raptor Audio core at the mixer level.
    BRAP_OP_SetDacVolume() is an extra level of volume control specifically for 
    the DACs. It does volume control by specifically programming the DAC 
    registers.
    
****************************************************************************/
BERR_Code
BRAP_OP_SetDacVolume (
    BRAP_Handle     hRap,			/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,	/* [in] Output Type */
    uint32_t uiVolume				/* [in] volume attenuation in 1/100 dB*/
);

/***************************************************************************
Summary:
	Retrieves the current volume level at the HifiDAC

Returns:
	BERR_SUCCESS 

See Also:
	BRAP_OP_SetDacVolume
	BRAP_MIXER_GetOutputVolume()	
****************************************************************************/
BERR_Code
BRAP_OP_GetDacVolume (
    BRAP_Handle     hRap,			/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,	/* [in] Output Type */
    uint32_t *puiVolume			/* [out] volume attenuation in 1/100 dB*/
);


#ifdef __cplusplus
}
#endif


#endif /* !_BRAP_HIFIDAC_H_ */

