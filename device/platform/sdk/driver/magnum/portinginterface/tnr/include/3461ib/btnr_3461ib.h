/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3461.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/19/11 12:33p $
 *
 * Module Description:
 *
 * Revision History:  $
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3461/btnr_3461.h $
 * 
 * Hydra_Software_Devel/3   4/19/11 12:33p atanugul
 * SW3461-3: Implement BTNR_SetSettings(), Powermode for 3461 TNR and
 * update BHAB_3461_P_DecodeInterrupt()
 * 
 * Hydra_Software_Devel/2   3/17/11 1:14p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip
 * 
 * Hydra_Software_Devel/1   3/16/11 3:25p atanugul
 * SW3461-3: Add support for BCM3461
 * 
 ***************************************************************************/
#ifndef BTNR_3461_H__
#define BTNR_3461_H__

#include "bchp.h"
#include "breg_i2c.h"
#include "bhab.h"
#include "btnr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned short i2cAddr;             /* 7bit I2C address of Bcm3461 */
    unsigned int channelNo;                 /* Channel number to tune to */
} BTNR_3461_Settings;

/***************************************************************************
Summary:
    This function returns the default settings for Bcm3461 Tuner module.

Description:
    This function is responsible for returns the default setting for 
    BTNR module. The returning default setting should be when
    opening the device.

Returns:
    TODO:

See Also:
    BTNR_3461_Open()

****************************************************************************/
BERR_Code BTNR_3461_GetDefaultSettings(
    BTNR_3461_Settings *pDefSettings  /* [out] Returns default setting */
    );


/***************************************************************************
Summary:
    This function opens Bcm3461 Tuner module.

Description:
    This function is responsible for opening Bcm3461 BTNR module. When BTNR is
    opened, it will create a module handle and configure the module based
    on the default settings. Once the device is opened, it must be closed
    before it can be opened again.

Returns:
    TODO:

See Also:
    BTNR_Close(), BTNR_3461_GetDefaultSettings()

****************************************************************************/
BERR_Code BTNR_3461_Open(
    BTNR_Handle *phDev,                 /* [out] Returns handle */
    BTNR_3461_Settings *pSettings,
    BHAB_Handle hHab
    );

#ifdef __cplusplus
}
#endif
 
#endif



