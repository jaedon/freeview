/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_equalizer_priv.h $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 10/26/10 7:06p $
*
* Module Description:
*   Module name: Hardware Equalizer
*   This file contains the definitions and prototypes for the hardware Equalizer
*   abstraction.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_equalizer_priv.h $
* 
* Hydra_Software_Devel/5   10/26/10 7:06p sgadara
* SW3556-1207: [3548,3556] Implement isr version of Equalizer
* coefficients programming
* 
* Hydra_Software_Devel/4   2/8/10 4:19p sgadara
* SW3548-2739-2686: [3548,3556] Modifying the Equalizer implementation
* design to support PEQ, and GEQ+Tone Control.
* --> After Mixer 2 SRCs will be connected back to back if Eq is added
* 
* Hydra_Software_Devel/3   10/10/08 10:02a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   9/23/08 4:57p speter
* PR 45444: [3548,3556]  Changing the implementation of hardware
* equalizer to reflect the latest changes in the equalizer algorithm
* delivered by Arch team
* 
* Hydra_Software_Devel/2   8/25/08 2:14p speter
* PR 45444: [3548,3556] Implementing the hardware equalizer
* 
* Hydra_Software_Devel/1   8/21/08 4:39p speter
* PR 45444: [3548,3556] Adding the hardware equalizer support.
* Implementing the algorithm provided by the architecture team. This is
* the initial version and not complete
* 
* 
* 
***************************************************************************/

#ifndef _BRAP_EQUALIZER_PRIV_H__ /*{{{*/
#define _BRAP_EQUALIZER_PRIV_H__

/*{{{ Defines */

#define BRAP_P_MAX_GEQ_FILTERS 5

typedef struct BRAP_EQ_P_FilterCoeffs
{
    int32_t b0[BRAP_P_MAX_IIR_FILTERS_IN_SRC];
    int32_t b1[BRAP_P_MAX_IIR_FILTERS_IN_SRC];
    int32_t b2[BRAP_P_MAX_IIR_FILTERS_IN_SRC];
    int32_t a1[BRAP_P_MAX_IIR_FILTERS_IN_SRC];
    int32_t a2[BRAP_P_MAX_IIR_FILTERS_IN_SRC];
}BRAP_EQ_P_FilterCoeffs;

BERR_Code BRAP_P_ApplyCoefficients (
    BRAP_EqualizerHandle hEqualizer
    );

BERR_Code BRAP_P_ApplyCoefficients_isr(
    BRAP_EqualizerHandle hEqualizer
    );
#endif /*}}} #ifndef _BRAP_EQUALIZER_PRIV_H__ */
	
/* End of File */
	

