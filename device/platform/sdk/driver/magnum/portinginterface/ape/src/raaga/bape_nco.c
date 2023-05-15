/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_nco.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 9/27/12 10:18a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_nco.c $
 * 
 * Hydra_Software_Devel/7   9/27/12 10:18a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/1   8/17/12 5:49p mpeteri
 * SW7445-19: Add additional PLL and NCO for 7445
 * 
 * Hydra_Software_Devel/6   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/5   1/27/12 4:50p jgarrett
 * SW7429-55: Updating MCLK of all connected outputs when a new mixer is
 * attached
 * 
 * Hydra_Software_Devel/4   11/14/11 3:16p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/26/11 12:44p jgarrett
 * SW7429-18: Merging latest changes from main branch
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/25/11 10:15a jgarrett
 * SW7429-18: Adding NCO support for 7429
 * 
 * Hydra_Software_Devel/3   10/25/11 1:30p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/2   7/11/11 2:29p jgarrett
 * SW7552-72: Removing 35230 compiler warning
 * 
 * Hydra_Software_Devel/1   7/8/11 4:29p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"

#if BCHP_CLKGEN_REG_START
#include "bchp_clkgen.h"
#endif

BDBG_MODULE(bape_nco);

#if BAPE_CHIP_MAX_NCOS > 0     /* If no NCOs on this chip, then skip all of this.  None of these functions are ever called. */

#ifdef BCHP_AUD_FMM_IOP_NCO_0_REG_START
#include "bchp_aud_fmm_iop_nco_0.h"
#ifdef BCHP_AUD_FMM_IOP_NCO_1_REG_START
#include "bchp_aud_fmm_iop_nco_1.h"
#define BAPE_NCO_STRIDE (BCHP_AUD_FMM_IOP_NCO_1_REG_START - BCHP_AUD_FMM_IOP_NCO_0_REG_START)
#else
#define BAPE_NCO_STRIDE 0
#endif
#endif

#ifdef BCHP_AUD_FMM_OP_MCLKGEN_REG_START
#include "bchp_aud_fmm_op_mclkgen.h"
#if BAPE_CHIP_MAX_NCOS > 1
#define BAPE_NCO_STRIDE (BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_1_SAMPLE_INC - BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_SAMPLE_INC)
#else
#define BAPE_NCO_STRIDE 0
#endif
#endif

#if defined BCHP_AUD_FMM_OP_MCLKGEN_REG_START
static void BAPE_Nco_UpdateDividers_isr(BAPE_Handle handle, BAPE_Nco nco, uint32_t sampleInc, uint32_t  numerator, uint32_t denominator, uint32_t phaseInc, BAVC_Timebase outputTimebase)
{
    uint32_t regAddr, regVal;
    
    regAddr = BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_CONTROL + (BAPE_NCO_STRIDE * nco);
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_CONTROL, TIMEBASE);
    switch ( outputTimebase )
    {
    case BAVC_Timebase_e0:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_CONTROL, TIMEBASE, TIMEBASE_0);
        break;
    case BAVC_Timebase_e1:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_CONTROL, TIMEBASE, TIMEBASE_1);
        break;
    case BAVC_Timebase_e2:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_CONTROL, TIMEBASE, TIMEBASE_2);
        break;
    case BAVC_Timebase_e3:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_CONTROL, TIMEBASE, TIMEBASE_3);
        break;
    }
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);

    regAddr = BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_RATE_RATIO + (BAPE_NCO_STRIDE * nco);
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_RATE_RATIO, DENOMINATOR);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_RATE_RATIO, DENOMINATOR, denominator);
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);

    regAddr = BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_SAMPLE_INC + (BAPE_NCO_STRIDE * nco);
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_SAMPLE_INC, SAMPLE_INC) | BCHP_MASK(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_SAMPLE_INC, NUMERATOR));
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_SAMPLE_INC, NUMERATOR, numerator);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_SAMPLE_INC, SAMPLE_INC, sampleInc);
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);

    regAddr = BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_PHASE_INC + (BAPE_NCO_STRIDE * nco);
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_PHASE_INC, PHASE_INC);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_PHASE_INC, PHASE_INC, phaseInc);
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);
}
#elif defined BCHP_AUD_FMM_IOP_NCO_0_REG_START
static void BAPE_Nco_UpdateDividers_isr(BAPE_Handle handle, BAPE_Nco nco, uint32_t sampleInc, uint32_t  numerator, uint32_t denominator, uint32_t phaseInc, BAVC_Timebase outputTimebase)
{
    uint32_t regAddr, regVal;
    
    regAddr = BCHP_AUD_FMM_IOP_NCO_0_MCLK_GEN_0_CONTROL + (BAPE_NCO_STRIDE * nco);
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_CONTROL, TIMEBASE);
    switch ( outputTimebase )
    {
    case BAVC_Timebase_e0:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_CONTROL, TIMEBASE, TIMEBASE_0);
        break;
    case BAVC_Timebase_e1:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_CONTROL, TIMEBASE, TIMEBASE_1);
        break;
    case BAVC_Timebase_e2:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_CONTROL, TIMEBASE, TIMEBASE_2);
        break;
    case BAVC_Timebase_e3:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_CONTROL, TIMEBASE, TIMEBASE_3);
        break;
    }
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);

    regAddr = BCHP_AUD_FMM_IOP_NCO_0_MCLK_GEN_0_RATE_RATIO + (BAPE_NCO_STRIDE * nco);
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_RATE_RATIO, DENOMINATOR);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_RATE_RATIO, DENOMINATOR, denominator);
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);

    regAddr = BCHP_AUD_FMM_IOP_NCO_0_MCLK_GEN_0_SAMPLE_INC + (BAPE_NCO_STRIDE * nco);
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_SAMPLE_INC, SAMPLE_INC) | BCHP_MASK(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_SAMPLE_INC, NUMERATOR));
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_SAMPLE_INC, NUMERATOR, numerator);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_SAMPLE_INC, SAMPLE_INC, sampleInc);
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);

    regAddr = BCHP_AUD_FMM_IOP_NCO_0_MCLK_GEN_0_PHASE_INC + (BAPE_NCO_STRIDE * nco);
    regVal = BREG_Read32_isr(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_PHASE_INC, PHASE_INC);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_NCO_0_MCLK_GEN_0_PHASE_INC, PHASE_INC, phaseInc);
    BREG_Write32_isr(handle->regHandle, regAddr, regVal);
}
#else
#error Unknown NCO register layout
#endif



void BAPE_P_AttachMixerToNco(BAPE_MixerHandle mixer, BAPE_Nco nco)
{
    unsigned ncoIndex = nco - BAPE_Nco_e0;

    BDBG_OBJECT_ASSERT(mixer, BAPE_Mixer);
    BDBG_ASSERT(ncoIndex < BAPE_CHIP_MAX_NCOS);
    BDBG_MSG(("Attaching mixer %p to NCO:%u", mixer, ncoIndex ));
    BLST_S_INSERT_HEAD(&mixer->deviceHandle->audioNcos[ncoIndex].mixerList, mixer, ncoNode);
    /* Update MCLK source for attached outputs */
    BKNI_EnterCriticalSection();
    BAPE_P_UpdateNco_isr(mixer->deviceHandle, nco);
    BKNI_LeaveCriticalSection();
}

void BAPE_P_DetachMixerFromNco(BAPE_MixerHandle mixer, BAPE_Nco nco)
{
    unsigned ncoIndex = nco - BAPE_Nco_e0;

    BDBG_OBJECT_ASSERT(mixer, BAPE_Mixer);
    BDBG_ASSERT(ncoIndex < BAPE_CHIP_MAX_NCOS);
    BDBG_MSG(("Detaching mixer %p from NCO:%u", mixer, ncoIndex ));
    BLST_S_REMOVE(&mixer->deviceHandle->audioNcos[ncoIndex].mixerList, mixer, BAPE_Mixer, ncoNode);
}

static BERR_Code BAPE_P_SetNcoFreq_isr( BAPE_Handle handle, BAPE_Nco nco, unsigned baseRate, unsigned *pOversample, BAVC_Timebase outputTimebase )
{
    int i;

    struct ncoInfo {
            unsigned baseFs; int oversample; long ncoFreq; int sampleInc; long numerator; int denominator; int phaseInc;
    } ncoInfo[] = 
    {       /* Multiples of 32 KHz */
            {  32000,              256,         8192000,          3,            303,           1024,          0x09b583  },
            {  64000,              128,         8192000,          3,            303,           1024,          0x09b583  },
            {  64000,              256,        16384000,          1,           1327,           2048,          0x136B06  },
            { 128000,              128,        16384000,          1,           1327,           2048,          0x136B06  },
            { 128000,              256,        32768000,          0,           3375,           4096,          0x26D60D  },

            /* Multiples of 44.1 KHz */
            {  44100,              256,        11289600,          2,            307,            784,          0x0d6159  },
            {  88200,              128,        11289600,          2,            307,            784,          0x0d6159  },
            {  88200,              256,        22579200,          1,            307,           1568,          0x1AC2B2  },
            { 176400,              128,        22579200,          1,            307,           1568,          0x1AC2B2  },
            { 176400,              256,        45158400,          0,           1875,           3136,          0x358564  },
                                                                                           
            /* Multiples of 48 KHz */
            {  48000,              256,        12288000,          2,            101,            512,          0x0e9045  },
            {  96000,              128,        12288000,          2,            101,            512,          0x0e9045  },
            {  96000,              256,        24576000,          1,            101,           1024,          0x1D208A  },
            { 192000,              128,        24576000,          1,            101,           1024,          0x1D208A  },
            { 192000,              256,        49152000,          0,           1125,           2048,          0x3A4114  },
    };
    int numElems = sizeof ncoInfo/sizeof ncoInfo[0];

    if (pOversample ) *pOversample = 0;     /* set to zero in case of early return */

    /* Search the table above to find the first entry for our baseRate.  There may be several entries for
     * the baseRate, but for now, just use the first one we find.  */
    for ( i=0 ; i<numElems ; i++  )
    {
        if ( ncoInfo[i].baseFs ==  baseRate )
        {
            break;
        }
    }

    if ( i >= numElems )
    {
        BDBG_ERR(("Can't find ncoInfo for sampling rate %u", baseRate));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BDBG_ASSERT(ncoInfo[i].baseFs ==  baseRate);

    BDBG_MSG(("Setting NCO %u frequency to %u Hz (%u * %u)", nco,  ncoInfo[i].ncoFreq, ncoInfo[i].oversample, ncoInfo[i].baseFs));

    BAPE_Nco_UpdateDividers_isr(handle, nco, ncoInfo[i].sampleInc, ncoInfo[i].numerator, ncoInfo[i].denominator, ncoInfo[i].phaseInc, outputTimebase);
    handle->audioNcos[nco].baseSampleRate   = baseRate;
    handle->audioNcos[nco].ncoFreq          = ncoInfo[i].ncoFreq;
    handle->audioNcos[nco].timebase         = outputTimebase;

    if (pOversample ) *pOversample = ncoInfo[i].oversample;   /* pass oversample factor back to caller. */

    return BERR_SUCCESS;
}

BERR_Code BAPE_P_UpdateNco_isr(BAPE_Handle handle, BAPE_Nco nco)
{
    unsigned baseRate = 0;
    unsigned idleRate = 0;
    BAVC_Timebase outputTimebase = (unsigned) -1;
    bool          gotTimebase = false;
    BAPE_Mixer *pMixer;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(nco < BAPE_CHIP_MAX_NCOS);

    /* Walk through each mixer and make sure we have no conflicts */
    for ( pMixer = BLST_S_FIRST(&handle->audioNcos[nco].mixerList);
          pMixer != NULL;
          pMixer = BLST_S_NEXT(pMixer, ncoNode) )
    {
        unsigned mixerRate;

        if ( BAPE_Mixer_P_GetOutputSampleRate(pMixer) == 0 )
        {
            continue;
        }

        if ( ! gotTimebase )
        {
            outputTimebase = pMixer->settings.outputTimebase;
            gotTimebase = true;
        }
        else if (pMixer->settings.outputTimebase != outputTimebase )
        {
            BDBG_WRN(("Timebase conflict on NCO %d.  One mixer requests timebase %u another requests timebase %u", nco, outputTimebase, pMixer->settings.outputTimebase));
        }

        mixerRate = BAPE_Mixer_P_GetOutputSampleRate(pMixer);

        if ( pMixer->running )
        {
            if ( baseRate == 0 )
            {
                baseRate = mixerRate;
            }
            else if ( baseRate != mixerRate )
            {
                BDBG_WRN(("Sample rate conflict on NCO %d.  One mixer requests %u another requests %u", nco, baseRate, mixerRate));
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
        uint32_t oversample;

        BDBG_MSG(("Updating NCO %u for base sample rate of %u Hz", nco, baseRate));
        errCode = BAPE_P_SetNcoFreq_isr( handle, nco, baseRate, &oversample, outputTimebase );
        if ( errCode )
        {
            BDBG_ERR(("Failed to set NCO %u for sample rate of %u Hz", nco, baseRate));
            return BERR_TRACE(errCode);
        }

        /* For each output, set it's mclk appropriately */
        for ( pMixer = BLST_S_FIRST(&handle->audioNcos[nco].mixerList);
              pMixer != NULL;
              pMixer = BLST_S_NEXT(pMixer, ncoNode) )
        {
            BAPE_OutputPort output;
            BAPE_MclkSource mclkSource;

            BDBG_ASSERT( BAPE_MCLKSOURCE_IS_NCO(pMixer->mclkSource));

            if ( BAPE_Mixer_P_GetOutputSampleRate(pMixer) == 0 )
            {
                /* Skip this mixer if it doesn't have a sample rate yet */
                continue;
            }

            switch ( nco )
            {
            default:
            case BAPE_Nco_e0:
                mclkSource = BAPE_MclkSource_eNco0;
                break;
            case BAPE_Nco_e1:
                mclkSource = BAPE_MclkSource_eNco1;
                break;
            case BAPE_Nco_e2:
                mclkSource = BAPE_MclkSource_eNco2;
                break;
            case BAPE_Nco_e3:
                mclkSource = BAPE_MclkSource_eNco3;
                break;
            case BAPE_Nco_e4:
                mclkSource = BAPE_MclkSource_eNco4;
                break;
            }
            BDBG_ASSERT( mclkSource == pMixer->mclkSource);

            for ( output = BLST_S_FIRST(&pMixer->outputList);
                  output != NULL;
                  output = BLST_S_NEXT(output, node) )
            {
                if ( output->setMclk_isr )
                {
                    BDBG_MSG(("Setting output mclk for '%s' to source:%s ratio:%u",
                               output->pName, BAPE_Mixer_P_MclkSourceToText(mclkSource), oversample));
                    output->setMclk_isr(output, mclkSource, 0, oversample);
                }
            }
            if ( pMixer->fs != BAPE_FS_INVALID )
            {
                BDBG_MSG(("Setting FS mclk for FS %u to source:%s ratio:%u",
                           pMixer->fs, BAPE_Mixer_P_MclkSourceToText(mclkSource), oversample));
                BAPE_P_SetFsTiming_isr(handle, pMixer->fs, mclkSource, 0, oversample);
            }
        }
    }
    else
    {
        BDBG_MSG(("Not updating NCO %u rate (unknown)", nco));
    }

    return BERR_SUCCESS;
}


BERR_Code BAPE_Nco_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    ncoIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each nco, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( ncoIndex=0 ; ncoIndex<BAPE_CHIP_MAX_NCOS ; ncoIndex++ )
    {
        BAPE_AudioNco *pNco = &bapeHandle->audioNcos[ncoIndex];

        /* Now apply changes for the settings struct. */
            /* Nothing to do here. NCOs don't have a SetSettings function to call. */

        /* Now restore the dynamic stuff from the values saved in the device struct. */
        if (pNco->baseSampleRate != 0)
        {
            BKNI_EnterCriticalSection();
                errCode = BAPE_P_SetNcoFreq_isr( bapeHandle, ncoIndex, pNco->baseSampleRate, NULL, pNco->timebase );
            BKNI_LeaveCriticalSection();
            if ( errCode ) return BERR_TRACE(errCode);
        }
    }
    return errCode;
}


/***************************************************************************
    Define stub functions for when there are no NCOs. 
***************************************************************************/
#else /* BAPE_CHIP_MAX_NCOS > 0 */
    /* No NCOs, just use stubbed out functions. */

/**************************************************************************/

BERR_Code BAPE_Nco_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BSTD_UNUSED(bapeHandle);
    return BERR_SUCCESS;
}

#endif /* BAPE_CHIP_MAX_NCOS > 0 */



