/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: baus_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 12/16/08 11:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/aus/3117/baus_priv.h $
 * 
 * Hydra_Software_Devel/1   12/16/08 11:31p vishk
 * PR46622: AUS PI: Add support for BCM311
 *
 ***************************************************************************/

#ifndef BAUS_PRIV_H__
#define BAUS_PRIV_H__

#include "bchp.h"
#include "bhab.h"
#include "baus.h"

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************
Summary:
	The settings for Out-of-Band Upstream module.

Description:

See Also:
	BAUS_Open()

****************************************************************************/
typedef struct
{
	unsigned long xtalFreq; 			/* Crystal Freqency in Hertz */
	bool isTransmitterEnabled;

	BAUS_OperationMode operationMode;
	unsigned long rfFreq; 				/* RF Frequency in Hertz */
	unsigned int powerLevel; 			/* in hundredth of dBmV */
	unsigned int symbolRate; 			/* in baud */
	unsigned premablePatternSz;
} BAUS_P_Settings;

/***************************************************************************
Summary:
	The handle for Out-of-Band Upstream module.

Description:

See Also:
	BAUS_Open()

****************************************************************************/
typedef struct BAUS_P_Handle
{
	uint32_t magicId;					/* Used to check if structure is corrupt */
	BCHP_Handle hChip;
	BREG_Handle hRegister;
	BINT_Handle hInterrupt;
	BHAB_DevId devId;
	BHAB_Handle hHab;
	BAUS_P_Settings settings;
} BAUS_P_Handle;


#ifdef __cplusplus
}
#endif

#endif

