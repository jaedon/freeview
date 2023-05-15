/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_mixer.h $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 6/15/12 4:25p $
 *
 * Module Description: Audio Mixer Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_mixer.h $
 * 
 * Hydra_Software_Devel/14   6/15/12 4:25p jgarrett
 * SW7425-3252: Allowing mixer balance to be set on the fly for
 * certification
 * 
 * Hydra_Software_Devel/13   6/15/12 1:07p jgarrett
 * SW7425-3252: Adding multiStreamBalance setting per dolby specification
 * 
 * Hydra_Software_Devel/12   3/7/12 4:35p jgarrett
 * SW7435-24: Adding DSP index for decode and dsp mixer
 * 
 * Hydra_Software_Devel/11   12/2/11 3:18p jgarrett
 * SW7346-578: Adding ability to change some mixer settings without
 * destroying and re-creating the mixer
 * 
 * Hydra_Software_Devel/10   8/25/11 1:48p gskerl
 * SW7425-1119: Removed obsolete prototype for
 * BAPE_Mixer_GetLoopbackInputPort
 * 
 * Hydra_Software_Devel/9   8/17/11 3:37p gskerl
 * SW7425-1119: Added prototypes for BAPE_Mixer_Start() and
 * BAPE_Mixer_Stop()
 * 
 * Hydra_Software_Devel/8   7/8/11 4:24p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/7   6/3/11 6:41p jgarrett
 * SW7425-406: Adding BAPE_Mixer_GetConnector routines
 * 
 * Hydra_Software_Devel/6   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/5   1/10/11 5:19p jgarrett
 * SW7422-146: Adding ramp step APIs
 * 
 * Hydra_Software_Devel/4   12/17/10 3:58p jgarrett
 * SW7422-146: Nexus APE integration on 7422
 * 
 * Hydra_Software_Devel/3   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 * Hydra_Software_Devel/2   12/14/10 2:39p jgarrett
 * SW7422-146: Renaming MixerOutput to OutputPort
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BAPE_MIXER_H_
#define BAPE_MIXER_H_

#include "bavc.h"

/***************************************************************************
Summary:
Mixer Handle
***************************************************************************/
typedef struct BAPE_Mixer *BAPE_MixerHandle;


/***************************************************************************
Summary:
Mixer Types
***************************************************************************/
typedef enum BAPE_MixerType
{
    BAPE_MixerType_eStandard,   /* Mixing and sample rate conversion are permitted */
    BAPE_MixerType_eDsp,        /* Mix using the DSP/Firmware and not the FMM */
    BAPE_MixerType_eMax
} BAPE_MixerType;

/***************************************************************************
Summary:
Mixer Settings
***************************************************************************/
typedef struct BAPE_MixerSettings
{
    BAPE_MixerType type;                /* Type of mixer.  This can only be set while creating 
                                           a mixer and not changed afterward. */

    unsigned mixerSampleRate;           /* Output rate of this mixer if it is fixed.  
                                           Default = 0, which will slave to the master input
                                           sample rate */ 

    unsigned defaultSampleRate;         /* If there is no master input or the
                                           master input is not started, the 
                                           sample rate specified here will be 
                                           applied to all outputs.  Ignored if mixerSampleRate
                                           has been set. */

    unsigned dspIndex;                  /* Index of the DSP you would like to use.  Default = 0. 
                                           Applies to DSP mixers only. */

    BAPE_Pll outputPll;                 /* Which output PLL will be used with this mixer.  
                                           It's usage depends on the type of outputs 
                                           connected to the mixer.  If the mixer has 
                                           only DAC outputs, then outputPll is not used.
                                           Otherwise, if the mixer has any I2S or Spdif 
                                           outputs, then outputPll specifies the PLL to 
                                           be used as the mixer's timing source.  
                                           Otherwise (no DACS, I2S, or Spdif outputs), 
                                           outputPll specifies the PLL to use when:
                                           1) there is not another mixer with matching inputs 
                                           and settings that has a DAC output whose 
                                           clock can be used for the timing source,
                                           and 2) a valid outputNco has not been 
                                           specified.

                                           Strongly recommended for I2S and SPDIF outputs.  
                                           Independent PLLs should be used if their 
                                           input clock (VCXO) is different or if their 
                                           sample rate is not an integer multiple of 
                                           other rates driven by the PLL. This cannot 
                                           be changed while inputs are running.

                                           Setting this field to BAPE_Pll_eMax will 
                                           prevent this mixer from using a PLL as a 
                                           timing source.   */

    BAPE_Nco outputNco;                 /* Which output NCO will be used with this mixer.
                                           It's usage depends on the type of outputs 
                                           connected to the mixer.  If the mixer has 
                                           only DAC outputs, then outputNco is not used.
                                           Otherwise, if the mixer has any I2S or Spdif 
                                           outputs, then outputNco specifies the NCO to 
                                           be used as the mixer's timing source unless 
                                           outputPll specifies a valid PLL.  Otherwise 
                                           (no DACS, I2S, or Spdif outputs), then 
                                           outputNco specifies the NCO to use for the 
                                           timing source.

                                           Recommended for use when a mixer only has MAI, 
                                           DummyOutput, and/or OutputCapture outputs
                                           (if the current chip has an audio NCO).

                                           Independent NCOs should be used if their 
                                           input clock (timebase) is different.  This
                                           cannot be changed while inputs are running.

                                           This field's default value is BAPE_Nco_eMax,
                                           which prevents the mixer from using an 
                                           NCO for a timing source.  */

    BAVC_Timebase outputTimebase;       /* What timebase will be used to drive 
                                           any DAC outputs connected to this mixer.  
                                           Timebase input to the VCXO Rate Manager 
                                           must be programmed externally by the 
                                           application.  This cannot be changed while 
                                           inputs are running. */

    unsigned outputDelay;               /* Delay for connected outputs (in ms).  Currently, applies to decoder
                                           inputs only. */

    int multiStreamBalance;             /* If this is a DSP mixer and you are mixing multi-stream content, this
                                           value controls the relative volume between the main and associated
                                           audio content.  0 means no preference.  Positive values up to 32 
                                           favor associated audio in dB steps.  32 will mute main audio.  Negative
                                           values down to -32 favor main audio in dB steps.  -32 will mute the
                                           associated audio. */
} BAPE_MixerSettings;

/***************************************************************************
Summary:
Get Default Mixer Settings
***************************************************************************/
void BAPE_Mixer_GetDefaultSettings(
    BAPE_MixerSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
Open a mixer
***************************************************************************/
BERR_Code BAPE_Mixer_Create(
    BAPE_Handle deviceHandle,
    const BAPE_MixerSettings *pSettings,
    BAPE_MixerHandle *pHandle               /* [out] */
    );

/***************************************************************************
Summary:
Close a mixer
***************************************************************************/
void BAPE_Mixer_Destroy(
    BAPE_MixerHandle handle
    );

/***************************************************************************
Summary:
Get Current Mixer Settings
***************************************************************************/
void BAPE_Mixer_GetSettings(
    BAPE_MixerHandle hMixer,
    BAPE_MixerSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
Set Mixer Settings 
 
Description: 
This call can only be made while all inputs to a mixer are stopped and 
the mixer itself is stopped (meaning BAPE_Mixer_Start has not been called). 
Attempts to change on the fly will return an error and the new settings will 
not be saved.  The only exception is changing the mixer balance settings 
for DSP mixers. 
 
See Also: 
BAPE_Mixer_GetSettings 
***************************************************************************/
BERR_Code BAPE_Mixer_SetSettings(
    BAPE_MixerHandle hMixer,
    const BAPE_MixerSettings *pSettings
    );

/***************************************************************************
Summary:
Start a mixer 

Description:
This call is optional.  By default, mixers will automatically start when
The first input starts, but if you want to explicitly enable the mixer
earlier call this function prior to starting any inputs. 
***************************************************************************/
BERR_Code BAPE_Mixer_Start(
    BAPE_MixerHandle handle
    );

/***************************************************************************
Summary:
Stop a mixer

Description:
This call is required only if you call BAPE_Mixer_Start().  By default, 
mixers will automatically stop when the last input stops, but if you
have explicitly started the mixer via BAPE_Mixer_Start() you must call
this routine to stop it after all inputs have stopped.
***************************************************************************/
void BAPE_Mixer_Stop(
    BAPE_MixerHandle handle
    );

/***************************************************************************
Summary:
Get a data path connector to cascade a mixer to other audio components
***************************************************************************/
void BAPE_Mixer_GetConnector(
    BAPE_MixerHandle handle,
    BAPE_Connector *pConnector /* [out] */
    );

/***************************************************************************
Summary:
Mixer Input Settings
***************************************************************************/
typedef struct BAPE_MixerAddInputSettings
{
    bool sampleRateMaster;      /* If true, this will be the master input for sample rate purposes */
    bool convertSampleRate;     /* For DSP mixers, this input must be sample-rate converted to 
                                   match the mixed sample rate.  Ignored on other mixer types.  */
} BAPE_MixerAddInputSettings;

/***************************************************************************
Summary:
Get Default Mixer Input Settings
***************************************************************************/
void BAPE_Mixer_GetDefaultAddInputSettings(
    BAPE_MixerAddInputSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
Add Mixer Input 
 
Description: 
Inputs to a mixer can not be modified while any previously connected 
inputs are running. 
***************************************************************************/
BERR_Code BAPE_Mixer_AddInput(
    BAPE_MixerHandle handle,
    BAPE_Connector input,
    const BAPE_MixerAddInputSettings *pSettings     /* Optional, pass NULL for default settings */
    );

/***************************************************************************
Summary:
Remove Mixer Input 
 
Description: 
Inputs to a mixer can not be modified while any previously connected 
inputs are running. 
***************************************************************************/
BERR_Code BAPE_Mixer_RemoveInput(
    BAPE_MixerHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove All Mixer Inputs
 
Description: 
Inputs to a mixer can not be modified while any previously connected 
inputs are running. 
***************************************************************************/
BERR_Code BAPE_Mixer_RemoveAllInputs(
    BAPE_MixerHandle handle
    );

/***************************************************************************
Summary:
Add Mixer Output
 
Description: 
Outputs from a mixer can not be modified while any previously connected 
inputs are running.
***************************************************************************/
BERR_Code BAPE_Mixer_AddOutput(
    BAPE_MixerHandle handle,
    BAPE_OutputPort output
    );

/***************************************************************************
Summary:
Remove Mixer Output
 
Description: 
Outputs from a mixer can not be modified while any previously connected 
inputs are running.
***************************************************************************/
BERR_Code BAPE_Mixer_RemoveOutput(
    BAPE_MixerHandle handle,
    BAPE_OutputPort output
    );

/***************************************************************************
Summary:
Remove All Mixer Outputs
 
Description: 
Outputs from a mixer can not be modified while any previously connected 
inputs are running. 
***************************************************************************/
BERR_Code BAPE_Mixer_RemoveAllOutputs(
    BAPE_MixerHandle handle
    );

/***************************************************************************
Summary:
Input Volume Settings
***************************************************************************/
typedef struct BAPE_MixerInputVolume
{
    int32_t coefficients[BAPE_Channel_eMax][BAPE_Channel_eMax];     /* Entries in this table reflect scaling from the input channel to the output channel.
                                                                       The first index is the input channel and the second index is the output channel.
                                                                       Default is to have BAPE_VOLUME_NORMAL for each [n][n] coefficient and 
                                                                       BAPE_VOLUME_MIN for all others.  This maps input channels to the same output channel
                                                                       with no scaling.  You can achieve effects such as a mono mix with these coefficients
                                                                       if desired by setting [Left][Left] to BAPE_VOLUME_NORMAL/2 and [Left][Right] to 
                                                                       BAPE_VOLUME_NORMAL/2, etc.  Mixing is only permitted between channels in the 
                                                                       same channel pair, so for example you can blend left and right, but not left and 
                                                                       center.  Values are specified in 5.23 2' complement integers.  These settings 
                                                                       are ignored for compressed inputs. */
    uint32_t coefficientRamp;     /* The 23 LSB of this field is the fractional part and the bit[26:23] MSB is the integer part. 
                                     e.g. 0x800000 = 1, 0x400000 = 0.5 */
    bool muted;                   /* Mute input data if true */
} BAPE_MixerInputVolume;

/***************************************************************************
Summary:
Get Input Volume Settings
***************************************************************************/
BERR_Code BAPE_Mixer_GetInputVolume(
    BAPE_MixerHandle mixer,
    BAPE_Connector input,
    BAPE_MixerInputVolume *pVolume      /* [out] */
    );

/***************************************************************************
Summary:
Set Input Volume Settings 
 
Description: 
Because inputs can be connected to more than one mixer, input volume is not 
persistent after an input is removed from a mixer. 
***************************************************************************/
BERR_Code BAPE_Mixer_SetInputVolume(
    BAPE_MixerHandle mixer,
    BAPE_Connector input,
    const BAPE_MixerInputVolume *pVolume
    );

/***************************************************************************
Summary:
Output Volume Settings
***************************************************************************/
typedef struct BAPE_OutputVolume
{
    uint32_t volume[BAPE_Channel_eMax];         /* Output volume scaling per output channel.  Default is BAPE_VOLUME_NORMAL for all channels.  
                                                   Ignored for compressed data.  Values are specified in 5.23 integers, so 0x800000 corresponds
                                                   to unity (BAPE_VOLUME_NORMAL). */
    bool muted;                                 /* Mute all output channels if true. */
} BAPE_OutputVolume;

/***************************************************************************
Summary:
Get Output Volume Settings
***************************************************************************/
BERR_Code BAPE_GetOutputVolume(
    BAPE_OutputPort output,
    BAPE_OutputVolume *pVolume      /* [out] */
    );

/***************************************************************************
Summary:
Set Output Volume Settings 
 
Description: 
Output volume settings are persistent and will remain set even when 
outputs are added/removed from mixers. 
***************************************************************************/
BERR_Code BAPE_SetOutputVolume(
    BAPE_OutputPort output,
    const BAPE_OutputVolume *pVolume
    );
   
/***************************************************************************
Summary:
Output Delay Status
***************************************************************************/
typedef struct BAPE_OutputDelayStatus
{
    unsigned pathDelay;         /* Path delay of all inputs to this output (in ms) */
    unsigned additionalDelay;   /* Delay added in BAPE_MixerSettings (in ms) */
} BAPE_OutputDelayStatus;

/***************************************************************************
Summary:
Get Output Delay Status for lipsync purposes
***************************************************************************/
void BAPE_GetOutputDelayStatus(
    BAPE_OutputPort output,
    BAPE_OutputDelayStatus *pStatus     /* [out] */
    );
      
/***************************************************************************
Summary:
Get Output Volume Ramp Step
***************************************************************************/
void BAPE_GetOutputVolumeRampStep(
    BAPE_Handle deviceHandle,
    uint32_t *pRampStep                 /* All mixers output volume is changed by this amount
                                           every Fs while ramping.  Specified in 4.23 format. 
                                           Ignored for compressed data. */
    );
   
/***************************************************************************
Summary:
Set Output Volume Ramp Step
***************************************************************************/
BERR_Code BAPE_SetOutputVolumeRampStep(
    BAPE_Handle deviceHandle,
    uint32_t rampStep                   /* All mixers output volume is changed by this amount
                                           every Fs while ramping.  Specified in 4.23 format. 
                                           Ignored for compressed data. */
    );
   
/***************************************************************************
Summary:
Get Sample Rate Converter Volume Ramp Step
***************************************************************************/
void BAPE_GetSampleRateConverterRampStep(
    BAPE_Handle deviceHandle,
    uint32_t *pRampStep                 /* All sample rate converters volume is changed by this amount
                                           every Fs while ramping.  Specified in 4.23 format. 
                                           Ignored for compressed data. */
    );
   
/***************************************************************************
Summary:
Set Sample Rate Converter Volume Ramp Step
***************************************************************************/
BERR_Code BAPE_SetSampleRateConverterRampStep(
    BAPE_Handle deviceHandle,
    uint32_t rampStep                   /* All sample rate converters volume is changed by this amount
                                           every Fs while ramping.  Specified in 4.23 format. 
                                           Ignored for compressed data. */
    );

#endif /* #ifndef BAPE_MIXER_H_ */

