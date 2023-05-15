/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_i2s_output.c $
* $brcm_Revision: 4 $
* $brcm_Date: 8/1/12 7:20p $
*
* API Description:
*   API name: I2sOutput
*    Specific APIs related to I2S audio outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_mips/nexus_i2s_output.c $
* 
* 4   8/1/12 7:20p bandrews
* SW7420-2085: add appropriate stubs for nexus object stuff
* 
* 3   7/27/12 4:38p jgarrett
* SW7420-2085: Converting from BDBG_OBJECT to NEXUS_OBJECT
* 
* 2   8/18/11 6:12p jgarrett
* SWDTV-6306: Adding I2S Multichannel Stubs
* 
* 1   11/20/09 5:23p jgarrett
* SW7408-17: Adding soft_audio
* 
* 7   3/2/09 5:37p jgarrett
* PR 51468: Renaming globals to match naming conventions
* 
* 6   7/17/08 2:06p jgarrett
* PR 42576: Adding BDBG_OBJECT
* 
* 5   6/18/08 5:13p jgarrett
* PR 43822: Fixing LR clock alignment to default to false
* 
* 4   4/2/08 11:31a erickson
* PR40198: fix DEBUG=n warning
*
* 3   2/20/08 9:49a erickson
* PR39770: fix return code warning
*
* 2   1/21/08 10:08a jgarrett
* PR 38535: Switching I2S output port enum
*
* Nexus_Devel/1   12/6/07 2:17p jgarrett
* PR 37971: Adding I2sOutput
*
***************************************************************************/

#include "nexus_audio_module.h"
#include "priv/nexus_core_audio.h"

BDBG_MODULE(nexus_i2s_output);

typedef struct NEXUS_I2sOutput
{
    NEXUS_OBJECT(NEXUS_I2sOutput);
} NEXUS_I2sOutput;

/***************************************************************************
Summary:
	Get default settings for an I2S output
See Also:

 ***************************************************************************/
void NEXUS_I2sOutput_GetDefaultSettings(
    NEXUS_I2sOutputSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
	Open an I2S Output device
See Also:
    NEXUS_I2sOutput_Close
 ***************************************************************************/
NEXUS_I2sOutputHandle NEXUS_I2sOutput_Open(
    unsigned index,
    const NEXUS_I2sOutputSettings *pSettings
    )
{
    NEXUS_Error errCode;
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_I2sOutput, NEXUS_I2sOutput_Close);

/***************************************************************************
Summary:
	Close an I2S Output device
See Also:
    NEXUS_I2sOutput_Open
 ***************************************************************************/
static void NEXUS_I2sOutput_P_Finalizer(
    NEXUS_I2sOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/***************************************************************************
Summary:
	Get settings for an I2S output
See Also:
	NEXUS_I2sOutput_SetSettings
 ***************************************************************************/
void NEXUS_I2sOutput_GetSettings(
    NEXUS_I2sOutputHandle handle,
    NEXUS_I2sOutputSettings *pSettings  /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
	Set settings for an I2S output
See Also:
	NEXUS_I2sOutput_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_I2sOutput_SetSettings(
    NEXUS_I2sOutputHandle handle,
    const NEXUS_I2sOutputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
    Get the audio connector for an I2S output
See Also:

 ***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sOutput_GetConnector(
    NEXUS_I2sOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

typedef struct NEXUS_I2sMultiOutput
{
    NEXUS_OBJECT(NEXUS_I2sMultiOutput);
} NEXUS_I2sMultiOutput;

/***************************************************************************
Summary:
Get default settings for a Multichannel I2S output
***************************************************************************/
void NEXUS_I2sMultiOutput_GetDefaultSettings(
    NEXUS_I2sMultiOutputSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Open a Multichannel I2S Output device
***************************************************************************/
NEXUS_I2sMultiOutputHandle NEXUS_I2sMultiOutput_Open( /* attr{destructor=NEXUS_I2sMultiOutput_Close}  */
    unsigned index,
    const NEXUS_I2sMultiOutputSettings *pSettings   /* attr{null_allowed=y} */
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}
    
NEXUS_OBJECT_CLASS_MAKE(NEXUS_I2sMultiOutput, NEXUS_I2sMultiOutput_Close);

/***************************************************************************
Summary:
	Close an I2S Output device
See Also:
    NEXUS_I2sOutput_Open
 ***************************************************************************/
static void NEXUS_I2sMultiOutput_P_Finalizer(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Get settings for a Multichannel I2S output
***************************************************************************/
void NEXUS_I2sMultiOutput_GetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_I2sMultiOutputSettings *pSettings  /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Set settings for a Multichannel I2S output
***************************************************************************/
NEXUS_Error NEXUS_I2sMultiOutput_SetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    const NEXUS_I2sMultiOutputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}
    

/***************************************************************************
Summary:
Get the audio connector for a multichannel I2S or the first stereo pair 
if set to stereo mode. 
***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetConnector(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}


/***************************************************************************
Summary:
Get the audio connector for an individual stereo pair on Multichannel I2S. 
Only supported if NEXUS_I2sMultiOutputSettings.mode is set to 
NEXUS_I2sMultiMode_eStereo. 
***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetStereoConnector(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_AudioChannelPair channelPair
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(channelPair);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}
