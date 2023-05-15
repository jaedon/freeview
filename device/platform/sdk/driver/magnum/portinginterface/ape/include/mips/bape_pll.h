/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_pll.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 12/8/09 11:10a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7408/bape_pll.h $
 * 
 * Hydra_Software_Devel/1   12/8/09 11:10a jgarrett
 * SW7408-17: Adding initial APE support
 * 
 ***************************************************************************/

#ifndef BAPE_PLL_H_
#define BAPE_PLL_H_

/***************************************************************************
Summary:
PLL Settings
***************************************************************************/
typedef struct BAPE_PllSettings
{
    bool freeRun;   /* If true, use an internal 27 MHz clock source instead of a VCXO rate manager */
    unsigned vcxo;  /* Which VCXO Rate Manager will drive this PLL */
} BAPE_PllSettings;

/***************************************************************************
Summary:
Get PLL Settings
***************************************************************************/
void BAPE_Pll_GetSettings(
    BAPE_Handle handle,
    BAPE_Pll pll,
    BAPE_PllSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
Set PLL Settings
***************************************************************************/
BERR_Code BAPE_Pll_SetSettings(
    BAPE_Handle handle,
    BAPE_Pll pll,
    const BAPE_PllSettings *pSettings
    );

#endif /* #ifndef BAPE_PLL_H_ */

