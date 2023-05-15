/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_pll.c $
 * $brcm_Revision: Hydra_Software_Devel/26 $
 * $brcm_Date: 9/27/12 10:18a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_pll.c $
 * 
 * Hydra_Software_Devel/26   9/27/12 10:18a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/1   9/26/12 10:56a mpeteri
 * SW7445-19: Add support for PLL Macros.  Switch to 128 fs base
 * 
 * Hydra_Software_Devel/25   6/12/12 4:47p jgarrett
 * SW7425-3220: Resolving coverity deadcode errors
 * 
 * Hydra_Software_Devel/24   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/23   1/27/12 4:50p jgarrett
 * SW7429-55: Updating MCLK of all connected outputs when a new mixer is
 * attached
 * 
 * Hydra_Software_Devel/22   12/1/11 3:15p jgarrett
 * SW7429-18: Refactoring PLL values for 256fs to match macros
 * 
 * Hydra_Software_Devel/21   11/30/11 7:15p jgarrett
 * SW7429-18: Dividing PLL factors for 216MHz chips
 * 
 * Hydra_Software_Devel/20   11/14/11 3:41p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/3   10/26/11 12:44p jgarrett
 * SW7429-18: Merging latest changes from main branch
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/25/11 11:15a jgarrett
 * SW7429-18: Adding PLL support for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/19   10/24/11 2:32p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/18   8/29/11 3:07p jgarrett
 * SWDTV-8444: Clearing LOAD_ENx for DTV PLL's after setting new values
 * 
 * Hydra_Software_Devel/17   7/14/11 4:33p gskerl
 * SWDTV-7838: Added additional defines for the
 * BCHP_##Register##_##Field##_MASK variant of some register fields
 * 
 * Hydra_Software_Devel/16   7/8/11 6:37p jgarrett
 * SWDTV-6305: Adding missing register write for AUDIO_MODE_SEL
 * 
 * Hydra_Software_Devel/15   7/8/11 4:26p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/14   7/7/11 4:35p jgarrett
 * SWDTV-6305: Setting MODE_SEL for 352xx PLL implementations to user
 * configurable
 * 
 * Hydra_Software_Devel/13   6/16/11 3:05p gskerl
 * SW7425-321: Renamed BAPE_PllStatus to BAPE_AudioPll, pllStatus to
 * audioPll, eliminated unused baseAddress and fsChannel fields, added
 * populating of baseSampleRate and freqCh1 fields.
 * 
 * Hydra_Software_Devel/12   5/24/11 4:40p gskerl
 * SW7425-321: Added support for mixers with different base sample rates
 * sharing the same PLL
 * 
 * Hydra_Software_Devel/11   4/20/11 7:01p gskerl
 * SW7425-384: Refactored BAPE_P_SetFsTiming_isr() to improve PLLCLKSEL
 * logic and to add support for multiple DACS
 * 
 * Hydra_Software_Devel/10   4/18/11 11:23p gskerl
 * SW7425-364: Fixed Coverity CID 399 (dead code)
 * 
 * Hydra_Software_Devel/9   4/18/11 10:13p gskerl
 * SW7425-364: Added BAPE_Pll_EnableExternalMclk() API to APE, then called
 * it from NEXUS_AudioModule_EnableExternalMclk()
 * 
 * Hydra_Software_Devel/8   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/7   4/6/11 1:23a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/6/11 11:15a jgarrett
 * SW35330-35: Adding 35233
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 7:13p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/6   3/24/11 7:54p gskerl
 * SW7422-146: Improved audio reference clock selection logic to handle
 * RDB differences for the 7231
 * 
 * Hydra_Software_Devel/5   3/22/11 3:00p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/4   3/3/11 8:09p jgarrett
 * SW7422-146: Fixing DDP passthrough sample rate issue
 * 
 * Hydra_Software_Devel/3   2/10/11 5:44p gskerl
 * SW7422-146: Refactored PLL code to support faster mclk rate of 256Fs
 * 
 * Hydra_Software_Devel/2   12/17/10 3:58p jgarrett
 * SW7422-146: Nexus APE integration on 7422
 * 
 * Hydra_Software_Devel/1   12/16/10 4:05p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"

#if BCHP_CLKGEN_REG_START
#include "bchp_clkgen.h"
#endif

BDBG_MODULE(bape_pll);

typedef struct {
    unsigned baseFs; 
    long freqCh1; 
    long ndivInt; 
    int MdivCh0;
    int MdivCh1;
    int MdivCh2;
} BAPE_PllDescriptor;

static BERR_Code BAPE_P_ProgramPll_isr(BAPE_Handle handle, BAPE_Pll pll, BAPE_PllDescriptor *pDescriptor);

#define BAPE_P_USE_PLL_MACROS       1

#if BCHP_AUD_FMM_PLL0_REG_START

/* Code below is for non-DTV chips that use the standard PLL macros */

#include "bchp_aud_fmm_pll0.h"
#if BAPE_CHIP_MAX_PLLS > 1
#include "bchp_aud_fmm_pll1.h"
#define BAPE_PLL_STRIDE (BCHP_AUD_FMM_PLL1_MACRO-BCHP_AUD_FMM_PLL0_MACRO)
#if BAPE_CHIP_MAX_PLLS > 2
#include "bchp_aud_fmm_pll2.h"
#endif
#else
#define BAPE_PLL_STRIDE 0
#endif

#if BAPE_P_USE_PLL_MACROS
static void BAPE_Pll_UpdateMacro_isr(BAPE_Handle handle, BAPE_Pll pll, uint32_t baseRate)
{
    uint32_t regAddr, regVal;
    
    /* AUD_FMM_PLL1_MACRO.MACRO_SELECT = User */
    regAddr = BCHP_AUD_FMM_PLL0_MACRO + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_MACRO, MACRO_SELECT);
    switch ( baseRate )
    {
        case  32000:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_PLL0_MACRO, MACRO_SELECT,  Mult_of_32000);
            break;
        case  44100:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_PLL0_MACRO, MACRO_SELECT,  Mult_of_44100);
            break;
        case  48000:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_PLL0_MACRO, MACRO_SELECT,  Mult_of_48000);
            break;
        case  96000:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_PLL0_MACRO, MACRO_SELECT,  Mult_of_96000);
            break;
        case 192000:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_192000);
            break;
        default:
            BDBG_ERR(("%s : Invalid baseRate %u", __FUNCTION__, baseRate));
            return;
            break;
    }
    BREG_Write32(handle->regHandle, regAddr, regVal);
}
#else /* BAPE_P_USE_PLL_MACROS */
static void BAPE_Pll_UpdateDividers_isr(BAPE_Handle handle, BAPE_Pll pll, uint32_t ndivInt, uint32_t MdivCh0, uint32_t MdivCh1, uint32_t MdivCh2)
{
    uint32_t regAddr, regVal;
    
    /* AUD_FMM_PLL1_MACRO.MACRO_SELECT = User */
    regAddr = BCHP_AUD_FMM_PLL0_MACRO + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_MACRO, MACRO_SELECT);
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_PLL0_MACRO, MACRO_SELECT, User);
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_CONTROL_0.USER_UPDATE_DIVIDERS = 0 */
    regAddr = BCHP_AUD_FMM_PLL0_CONTROL_0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_CONTROL_0, USER_UPDATE_DIVIDERS);
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_USER_NDIV.NDIV_INT = ndivInt */
    regAddr = BCHP_AUD_FMM_PLL0_USER_NDIV + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_NDIV, NDIV_INT);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_NDIV, NDIV_INT, ndivInt);
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_CONTROL_0.USER_UPDATE_DIVIDERS = 1 */
    regAddr = BCHP_AUD_FMM_PLL0_CONTROL_0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_CONTROL_0, USER_UPDATE_DIVIDERS);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_CONTROL_0, USER_UPDATE_DIVIDERS, 1 );
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch0.MDIV = MdivCh0 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch0, MDIV);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch0, MDIV, MdivCh0);
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch0.LOAD_EN = 1 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch0, LOAD_EN, 1 );
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch0.LOAD_EN = 0 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch0, LOAD_EN);
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch1.MDIV = MdivCh1 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch1 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch1, MDIV);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch1, MDIV, MdivCh1);
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch1.LOAD_EN = 1 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch1 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch1, LOAD_EN);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch1, LOAD_EN, 1 );
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch1.LOAD_EN = 0 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch1 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch1, LOAD_EN);
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch2.MDIV = MdivCh2 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch2 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch2, MDIV);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch2, MDIV, MdivCh2);
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch2.LOAD_EN = 1 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch2 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch2, LOAD_EN);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch2, LOAD_EN, 1 );
    BREG_Write32(handle->regHandle, regAddr, regVal);
    
    /* AUD_FMM_PLL0_USER_MDIV_Ch2.LOAD_EN = 0 */
    regAddr = BCHP_AUD_FMM_PLL0_USER_MDIV_Ch2 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch2, LOAD_EN);
    BREG_Write32(handle->regHandle, regAddr, regVal);
}
#endif /* BAPE_P_USE_PLL_MACROS */

#else

#include "bchp_aud_fmm_iop_pll_0.h"

#ifdef BCHP_AUD_FMM_IOP_PLL_1_REG_START
#include "bchp_aud_fmm_iop_pll_1.h"
#define BAPE_PLL_STRIDE (BCHP_AUD_FMM_IOP_PLL_1_REG_START-BCHP_AUD_FMM_IOP_PLL_0_REG_START)
#else
#define BAPE_PLL_STRIDE 0
#endif

#if BAPE_P_USE_PLL_MACROS
static void BAPE_Pll_UpdateMacro_isr(BAPE_Handle handle, BAPE_Pll pll, uint32_t baseRate)
{
    uint32_t regAddr, regVal;
    
    /* AUD_FMM_PLL1_MACRO.MACRO_SELECT = User */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_MACRO + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_MACRO, MACRO_SELECT);
    switch ( baseRate )
    {
        case  32000:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_PLL_0_MACRO, MACRO_SELECT,  Mult_of_32000);
            break;
        case  44100:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_PLL_0_MACRO, MACRO_SELECT,  Mult_of_44100);
            break;
        case  48000:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_PLL_0_MACRO, MACRO_SELECT,  Mult_of_48000);
            break;
        case  96000:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_PLL_0_MACRO, MACRO_SELECT,  Mult_of_96000);
            break;
        case 192000:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_PLL_0_MACRO, MACRO_SELECT, Mult_of_192000);
            break;
        default:
            BDBG_ERR(("%s : Invalid baseRate %u", __FUNCTION__, baseRate));
            return;
            break;
    }
    BREG_Write32(handle->regHandle, regAddr, regVal);
}
#else /* BAPE_P_USE_PLL_MACROS */
static void BAPE_Pll_UpdateDividers_isr(BAPE_Handle handle, BAPE_Pll pll, uint32_t ndivInt, uint32_t MdivCh0, uint32_t MdivCh1, uint32_t MdivCh2)
{
    uint32_t regAddr, regVal;
    
    /* AUD_FMM_PLL1_MACRO.MACRO_SELECT = User */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_MACRO + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_MACRO, MACRO_SELECT);
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_PLL_0_MACRO, MACRO_SELECT, User);
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_CONTROL_0.USER_UPDATE_DIVIDERS = 0 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_CONTROL_0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_CONTROL_0, USER_UPDATE_DIVIDERS);
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_USER_NDIV.NDIV_INT = ndivInt */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_NDIV + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_NDIV, NDIV_INT);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_PLL_0_USER_NDIV, NDIV_INT, ndivInt);
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_CONTROL_0.USER_UPDATE_DIVIDERS = 1 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_CONTROL_0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_CONTROL_0, USER_UPDATE_DIVIDERS);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_PLL_0_CONTROL_0, USER_UPDATE_DIVIDERS, 1 );
    BREG_Write32(handle->regHandle, regAddr, regVal);


    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0.MDIV = MdivCh0 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0, MDIV);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0, MDIV, MdivCh0);
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0.LOAD_EN = 1 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0, LOAD_EN, 1 );
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0.LOAD_EN = 0 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0, LOAD_EN);
    BREG_Write32(handle->regHandle, regAddr, regVal);


    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1.MDIV = MdivCh1 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1, MDIV);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1, MDIV, MdivCh1);
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1.LOAD_EN = 1 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1, LOAD_EN);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1, LOAD_EN, 1 );
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1.LOAD_EN = 0 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1, LOAD_EN);
    BREG_Write32(handle->regHandle, regAddr, regVal);


    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2.MDIV = MdivCh2 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2, MDIV);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2, MDIV, MdivCh2);
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2.LOAD_EN = 1 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2, LOAD_EN);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2, LOAD_EN, 1 );
    BREG_Write32(handle->regHandle, regAddr, regVal);

    /* AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2.LOAD_EN = 0 */
    regAddr = BCHP_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2 + (BAPE_PLL_STRIDE * pll);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2, LOAD_EN);
    BREG_Write32(handle->regHandle, regAddr, regVal);
}
#endif /* BAPE_P_USE_PLL_MACROS */

#endif

static BERR_Code BAPE_P_ProgramPll_isr(BAPE_Handle handle, BAPE_Pll pll, BAPE_PllDescriptor *pDescriptor)
{

#if BAPE_P_USE_PLL_MACROS
    BAPE_Pll_UpdateMacro_isr(handle, pll, pDescriptor->baseFs);
#else
    BAPE_Pll_UpdateDividers_isr(handle, pll, pDescriptor->ndivInt, pDescriptor->MdivCh0, pDescriptor->MdivCh1, pDescriptor->MdivCh2);
#endif

    return BERR_SUCCESS;
}

/* Common code */
void BAPE_Pll_GetSettings(
    BAPE_Handle handle,
    BAPE_Pll pll,
    BAPE_PllSettings *pSettings /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(pll < BAPE_CHIP_MAX_PLLS);
    *pSettings = handle->audioPlls[pll].settings;
}

BERR_Code BAPE_Pll_SetSettings(
    BAPE_Handle handle,
    BAPE_Pll pll,
    const BAPE_PllSettings *pSettings
    )
{
    uint32_t regVal, regAddr, data;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(pll < BAPE_CHIP_MAX_PLLS);
    BDBG_ASSERT(NULL != pSettings);
    handle->audioPlls[pll].settings = *pSettings;

#ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT
    if ( pSettings->freeRun )
    {
        #ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Fixed
            data = BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Fixed;
        #else
            BDBG_ERR(("Can't use fixed reference clock for audio"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        #endif
    }
    else
    {
        /* VCXO source */
        switch (pSettings->vcxo)
        {
        #ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo0
        case 0:
            data = BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo0;
            break;
        #endif

        #ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo1
        case 1:
            data = BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo1;
            break;
        #endif

        #ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo2
        case 2:
            data = BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo2;
            break;
        #endif

        #ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo3
        case 3:
            data = BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo3;
            break;
        #endif

        default:
            BDBG_ERR(("Invalid or unsupported audio VCXO: %u", pSettings->vcxo));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
    }

    BKNI_EnterCriticalSection();
    regAddr = BCHP_CLKGEN_INTERNAL_MUX_SELECT;
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    switch ( pll )
    {
    case BAPE_Pll_e0:
        regVal &= ~BCHP_MASK(CLKGEN_INTERNAL_MUX_SELECT, PLLAUDIO0_REFERENCE_CLOCK);
        regVal |=  BCHP_FIELD_DATA(CLKGEN_INTERNAL_MUX_SELECT, PLLAUDIO0_REFERENCE_CLOCK, data);
        break;
#ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO1_REFERENCE_CLOCK_SHIFT
    case BAPE_Pll_e1:
        regVal &= ~BCHP_MASK(CLKGEN_INTERNAL_MUX_SELECT, PLLAUDIO1_REFERENCE_CLOCK);
        regVal |=  BCHP_FIELD_DATA(CLKGEN_INTERNAL_MUX_SELECT, PLLAUDIO1_REFERENCE_CLOCK, data);
        break;
#endif
#ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO2_REFERENCE_CLOCK_SHIFT
    case BAPE_Pll_e2:
        regVal &= ~BCHP_MASK(CLKGEN_INTERNAL_MUX_SELECT, PLLAUDIO2_REFERENCE_CLOCK);
        regVal |=  BCHP_FIELD_DATA(CLKGEN_INTERNAL_MUX_SELECT, PLLAUDIO2_REFERENCE_CLOCK, data);
        break;
#endif
    default:
        break;
    }
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);
    BKNI_LeaveCriticalSection();
#endif

    return BERR_SUCCESS;
}

void BAPE_P_AttachMixerToPll(BAPE_MixerHandle mixer, BAPE_Pll pll)
{
    BDBG_OBJECT_ASSERT(mixer, BAPE_Mixer);
    BDBG_ASSERT(pll < BAPE_CHIP_MAX_PLLS);
    BLST_S_INSERT_HEAD(&mixer->deviceHandle->audioPlls[pll].mixerList, mixer, pllNode);
    /* Update MCLK source for attached outputs */
    BKNI_EnterCriticalSection();
    BAPE_P_UpdatePll_isr(mixer->deviceHandle, pll);
    BKNI_LeaveCriticalSection();
}

void BAPE_P_DetachMixerFromPll(BAPE_MixerHandle mixer, BAPE_Pll pll)
{
    BDBG_OBJECT_ASSERT(mixer, BAPE_Mixer);
    BDBG_ASSERT(pll < BAPE_CHIP_MAX_PLLS);
    BLST_S_REMOVE(&mixer->deviceHandle->audioPlls[pll].mixerList, mixer, BAPE_Mixer, pllNode);
}

static BERR_Code BAPE_P_GetPllBaseSampleRate_isr(unsigned sampleRate, unsigned *pBaseRate)
{
    switch ( sampleRate )
    {
    case 32000:    /* 32K Sample rate */
    case 64000:      /* 64K Sample rate */
    case 128000:     /* 128K Sample rate */
        *pBaseRate = 32000;
        return BERR_SUCCESS;
    case 44100:    /* 44.1K Sample rate */
    case 88200:    /* 88.2K Sample rate */
    case 176400:   /* 176.4K Sample rate */
        *pBaseRate = 44100;
        return BERR_SUCCESS;
    case 48000:      /* 48K Sample rate */
    case 96000:      /* 96K Sample rate */
    case 192000:     /* 192K Sample rate */
        *pBaseRate = 48000;
        return BERR_SUCCESS;
    default:
        BDBG_ERR(("Invalid sampling rate %u", sampleRate));
        *pBaseRate = 0;
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
}

static BERR_Code BAPE_P_SetPllFreq_isr( BAPE_Handle handle, BAPE_Pll pll, unsigned baseRate )
{
    int i;

    /* 40 nm values clock values - note that these are only used when programming
       the clocks manually using "BAPE_Pll_UpdateDividers_isr".  When using 
       "BAPE_Pll_UpdateMacro_isr", the hardware calculates and programs internally */
    BAPE_PllDescriptor pllInfo[] = 
    {
        #if BAPE_BASE_PLL_TO_FS_RATIO == 128    /* Run PLL Ch0 at 128 BaseFS */
            {  32000,               4096000,         64,          180,          180,       90 },
            {  44100,               5644800,         49,          100,          100,       50 }, 
            {  48000,               6144000,         64,          120,          120,       60 }, 
    
        #elif BAPE_BASE_PLL_TO_FS_RATIO == 256  /* Run PLL Ch0 at 256 BaseFS */
            {  32000,               8192000,         64,           90,           90,       45 },
            {  44100,              11289600,         49,           50,           50,       25 }, 
            {  48000,              12288000,         64,           60,           60,       30 }, 
    
        #elif BAPE_BASE_PLL_TO_FS_RATIO == 512  /* Run PLL Ch0 at 512 BaseFS */
            {  32000,              16384000,        128,           90,           90,       45 },
            {  44100,              22579200,         98,           50,           50,       25 }, 
            {  48000,              24576000,        128,           60,           60,       30 }
        #else
            #error "BAPE_BASE_PLL_TO_FS_RATIO is invalid or not defined"
        #endif
    };
    int numElems = sizeof pllInfo/sizeof pllInfo[0];

    for ( i=0 ; i<numElems ; i++  )
    {
        if ( pllInfo[i].baseFs ==  baseRate )
        {
            break;
        }
    }

    if ( i >= numElems )
    {
        BDBG_ERR(("Invalid sampling rate %u", baseRate));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BDBG_ASSERT(pllInfo[i].baseFs ==  baseRate);

    BDBG_MSG(("Setting PLL %u frequency to %u Hz (%u * BaseFs)", pll,  pllInfo[i].freqCh1, BAPE_BASE_PLL_TO_FS_RATIO));

    BAPE_P_ProgramPll_isr(handle, pll, &(pllInfo[i]));
    handle->audioPlls[pll].baseSampleRate   = baseRate;
    handle->audioPlls[pll].freqCh1          = pllInfo[i].freqCh1;

    return BERR_SUCCESS;
}

BERR_Code BAPE_P_UpdatePll_isr(BAPE_Handle handle, BAPE_Pll pll)
{
    unsigned baseRate = 0;
    unsigned idleRate = 0;
    BAPE_Mixer *pMixer;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(pll < BAPE_CHIP_MAX_PLLS);

    /* Walk through each mixer and make sure we have no conflicts */
    for ( pMixer = BLST_S_FIRST(&handle->audioPlls[pll].mixerList);
          pMixer != NULL;
          pMixer = BLST_S_NEXT(pMixer, pllNode) )
    {
        unsigned mixerRate;

        if ( BAPE_Mixer_P_GetOutputSampleRate(pMixer) == 0 )
        {
            continue;
        }

        errCode = BAPE_P_GetPllBaseSampleRate_isr(BAPE_Mixer_P_GetOutputSampleRate(pMixer), &mixerRate);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        if ( pMixer->running )
        {
            if ( baseRate == 0 )
            {
                baseRate = mixerRate;
            }
            else if ( baseRate != mixerRate )
            {
                BDBG_WRN(("Sample rate conflict on PLL %d.  One mixer requests %u another requests %u", pll, baseRate, mixerRate));
            }
        }
        else if ( idleRate == 0 )
        {
            idleRate = mixerRate;
        }
    }

    if ( baseRate == 0 )
    {
        baseRate = idleRate;
    }

    if ( baseRate != 0 )
    {
        BDBG_MSG(("Updating PLL %u for base sample rate of %u Hz", pll, baseRate));

        errCode = BAPE_P_SetPllFreq_isr( handle, pll, baseRate );
        if ( errCode )  return BERR_TRACE(errCode);

        /* For each output, set it's mclk appropriately */
        for ( pMixer = BLST_S_FIRST(&handle->audioPlls[pll].mixerList);
              pMixer != NULL;
              pMixer = BLST_S_NEXT(pMixer, pllNode) )
        {
            BAPE_OutputPort output;
            unsigned rateNum = BAPE_Mixer_P_GetOutputSampleRate(pMixer);
            unsigned pllChan;
            BAPE_MclkSource mclkSource;

            BDBG_ASSERT( BAPE_MCLKSOURCE_IS_PLL(pMixer->mclkSource));

            if ( BAPE_Mixer_P_GetOutputSampleRate(pMixer) == 0 )
            {
                /* Skip this mixer if it doesn't have a sample rate yet */
                continue;
            }

            switch ( pll )
            {
            default:
            case BAPE_Pll_e0:
                mclkSource = BAPE_MclkSource_ePll0;
                break;
            case BAPE_Pll_e1:
                mclkSource = BAPE_MclkSource_ePll1;
                break;
            case BAPE_Pll_e2:
                mclkSource = BAPE_MclkSource_ePll2;
                break;
            }
            BDBG_ASSERT( mclkSource == pMixer->mclkSource);

            pllChan =  rateNum  <= 48000 ? 0 :        /* Channel 0 runs at 32 KHz, 44.1 KHz, or 48 KHz    */
                       rateNum  <= 96000 ? 1 :        /* Channel 1 runs at 64 KHz, 88.2 KHz, or 96 KHz    */
                                           2 ;        /* Channel 2 runs at 128 KHz, 176.4 KHz, or 192 KHz */
                                                             
            for ( output = BLST_S_FIRST(&pMixer->outputList);
                  output != NULL;
                  output = BLST_S_NEXT(output, node) )
            {
                if ( output->setMclk_isr )
                {
                    BDBG_MSG(("Setting output mclk for '%s' to source:%s channel:%u ratio:%u",
                               output->pName, BAPE_Mixer_P_MclkSourceToText(mclkSource), pllChan, BAPE_BASE_PLL_TO_FS_RATIO));
                    output->setMclk_isr(output, mclkSource, pllChan, BAPE_BASE_PLL_TO_FS_RATIO);
                }
            }

            if ( pMixer->fs != BAPE_FS_INVALID )
            {
                BDBG_MSG(("Setting FS mclk for FS %u to source:%s ratio:%u",
                           pMixer->fs, BAPE_Mixer_P_MclkSourceToText(mclkSource), BAPE_BASE_PLL_TO_FS_RATIO));
                BAPE_P_SetFsTiming_isr(handle, pMixer->fs, mclkSource, pllChan, BAPE_BASE_PLL_TO_FS_RATIO);
            }
        }
    }
    else
    {
        BDBG_MSG(("Not updating PLL %u rate (unknown)", pll));
    }

    return BERR_SUCCESS;
}



BERR_Code BAPE_Pll_EnableExternalMclk(
    BAPE_Handle     handle,
    BAPE_Pll        pll,
    unsigned        mclkIndex,
    BAPE_MclkRate   mclkRate
    )
{
    unsigned            pllChannel;
    unsigned            pll0ToBaseFsRatio;   
    unsigned            requestedPllToBaseFsRatio;
    uint32_t            pllclksel;
    uint32_t            regAddr;
    uint32_t            regVal;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(pll < BAPE_CHIP_MAX_PLLS);
    BDBG_ASSERT(mclkIndex < BAPE_CHIP_MAX_EXT_MCLKS);

    pll0ToBaseFsRatio = BAPE_BASE_PLL_TO_FS_RATIO;

    switch (mclkRate)
    {
    case BAPE_MclkRate_e128Fs:
        requestedPllToBaseFsRatio = 128;
        break;

    case BAPE_MclkRate_e256Fs:
        requestedPllToBaseFsRatio = 256;
        break;

    case BAPE_MclkRate_e384Fs:
        requestedPllToBaseFsRatio = 384;
        break;

    case BAPE_MclkRate_e512Fs:
        requestedPllToBaseFsRatio = 512;
        break;

    default:
        BDBG_ERR(("Requested mclkRate is invalid"));
        return(BERR_TRACE(BERR_INVALID_PARAMETER));
    }
    
    if (requestedPllToBaseFsRatio == pll0ToBaseFsRatio)
    {
        pllChannel = 0;
    }
    else if (requestedPllToBaseFsRatio == 2 * pll0ToBaseFsRatio)
    {
        pllChannel = 1;
    }
#if BAPE_BASE_PLL_TO_FS_RATIO < 256
    else if (requestedPllToBaseFsRatio == 4 * pll0ToBaseFsRatio)
    {
        pllChannel = 2;
    }
#endif
    else
    {
        BDBG_ERR(("Requested mclkRate:%dFs is invalid", requestedPllToBaseFsRatio));
        if ( requestedPllToBaseFsRatio < pll0ToBaseFsRatio )
        {
            BDBG_ERR(("Current minimum MCLK rate is %dFs... you may need to reduce BAPE_BASE_PLL_TO_FS_RATIO", pll0ToBaseFsRatio ));
        }
        return(BERR_TRACE(BERR_INVALID_PARAMETER));
    }
            
#if defined BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE
    switch ( pll )
    {
    /* PLL Timing */
    #if BAPE_CHIP_MAX_PLLS > 0
    case BAPE_Pll_e0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL0_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 1
    case BAPE_Pll_e1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL1_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 2
    case BAPE_Pll_e2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL2_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 3
    case BAPE_Pll_e3:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL3_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 4
        #error "Need to add support for more PLLs"
    #endif
    
    /* Should never get here */
    default:
        BDBG_ERR(("PLL is invalid"));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return(BERR_INVALID_PARAMETER);
    }

    /* Read the register. */
    regAddr = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + ((BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_ELEMENT_SIZE * mclkIndex)/8);
    regVal = BREG_Read32(handle->regHandle, regAddr);

    /* Clear the field that we're going to fill in. */
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, PLLCLKSEL));

    /* Fill in the PLLCLKSEL field. */
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, PLLCLKSEL, pllclksel);

    BREG_Write32(handle->regHandle, regAddr, regVal);

#elif defined BCHP_AUD_FMM_IOP_MISC_MCLK_CFG_i_ARRAY_BASE
    BSTD_UNUSED(regVal);
    BSTD_UNUSED(pllclksel);
    {
        BAPE_Reg_P_FieldList regFieldList;

        regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_IOP_MISC_MCLK_CFG_i, mclkIndex);
        BAPE_Reg_P_InitFieldList(handle, &regFieldList);
        switch ( pll ) 
        {
    #if BAPE_CHIP_MAX_PLLS > 0
    case BAPE_Pll_e0:
        if ( 0 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL0_ch1);
        }
        else if ( 1 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL0_ch2);
        }
        #if BAPE_BASE_PLL_TO_FS_RATIO < 256
        else if ( 2 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL0_ch3);
        }
        #endif        
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 1
    case BAPE_Pll_e1:
        if ( 0 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL1_ch1);
        }
        else if ( 1 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL1_ch2);
        }
        #if BAPE_BASE_PLL_TO_FS_RATIO < 256
        else if ( 2 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL1_ch3);
        }
        #endif        
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 2
    case BAPE_Pll_e2:
        if ( 0 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL2_ch1);
        }
        else if ( 1 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL2_ch2);
        }
        #if BAPE_BASE_PLL_TO_FS_RATIO < 256
        else if ( 2 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL2_ch3);
        }
        #endif        
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 3
    case BAPE_Pll_e3:
        if ( 0 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL3_ch1);
        }
        else if ( 1 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL3_ch2);
        }
        #if BAPE_BASE_PLL_TO_FS_RATIO < 256
        else if ( 2 == pllChannel )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_MISC_MCLK_CFG_i, PLLCLKSEL, PLL3_ch3);
        }
        #endif        
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 4
        #error "Need to add support for more PLLs"
    #endif
        /* Should never get here */
        default:
            BDBG_ERR(("PLL is invalid"));
            BDBG_ASSERT(false);     /* something went wrong somewhere! */
            return(BERR_INVALID_PARAMETER);
        }
        BAPE_Reg_P_ApplyFieldList(&regFieldList, regAddr);
    }
#endif

    return(BERR_SUCCESS);
    
}

BERR_Code BAPE_Pll_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    pllIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each pll, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( pllIndex=0 ; pllIndex<BAPE_CHIP_MAX_PLLS ; pllIndex++ )
    {
        BAPE_AudioPll *pPll = &bapeHandle->audioPlls[pllIndex];

        /* Now apply changes for the settings struct. */
        errCode = BAPE_Pll_SetSettings(bapeHandle, pllIndex, &pPll->settings );
        if ( errCode ) return BERR_TRACE(errCode);

        /* Now restore the dynamic stuff from the values saved in the device struct. */
        if (pPll->baseSampleRate != 0)
        {
            BKNI_EnterCriticalSection();
                errCode = BAPE_P_SetPllFreq_isr( bapeHandle, pllIndex, pPll->baseSampleRate );
            BKNI_LeaveCriticalSection();
            if ( errCode ) return BERR_TRACE(errCode);
        }
    }
    return errCode;
}


