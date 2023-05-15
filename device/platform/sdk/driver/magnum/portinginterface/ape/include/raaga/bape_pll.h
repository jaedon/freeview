/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_pll.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/18/11 10:12p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_pll.h $
 * 
 * Hydra_Software_Devel/3   4/18/11 10:12p gskerl
 * SW7425-364: Added BAPE_Pll_EnableExternalMclk() API to APE, then called
 * it from NEXUS_AudioModule_EnableExternalMclk()
 * 
 * Hydra_Software_Devel/2   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
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

/***************************************************************************
Summary:
Enable an External Mclk Output
***************************************************************************/
BERR_Code BAPE_Pll_EnableExternalMclk(
    BAPE_Handle     handle,
    BAPE_Pll        pll,
    unsigned        mclkIndex,
    BAPE_MclkRate   mclkRate
    );

#endif /* #ifndef BAPE_PLL_H_ */

