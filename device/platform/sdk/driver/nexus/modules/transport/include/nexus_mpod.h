/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_mpod.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 11/7/11 3:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/include/nexus_mpod.h $
 * 
 * 11   11/7/11 3:59p bandrews
 * SW7231-446: merge to main
 * 
 * SW7231-446/1   11/3/11 9:26p bandrews
 * SW7231-391: mpod param verification requires a struct
 * 
 * 10   9/21/10 11:10a mphillip
 * SW7420-1044: Correct location of pre-filtering MPOD variable
 * 
 * 9   9/20/10 6:28p mphillip
 * SW7420-1044: Add support for pre-filtering pids destined for MPOD
 * 
 * 8   10/9/09 10:41a erickson
 * SW7325-605: add NEXUS_MpodOpenSettings.clockNotRunning
 *
 * 7   9/22/09 1:03p erickson
 * SW7325-605: add NEXUS_MpodOpenSettings.outputInvertSync. update
 *  comments.
 *
 * 6   9/23/08 9:16a jgarrett
 * PR 46513: Merging 97459 support to mainline
 *
 * PR44830/2   9/15/08 5:10p anilmm
 * PR44830: Adjust for nexus naming convention
 *
 * PR44830/1   7/28/08 7:31p anilmm
 * PR44830: Add output interface formatter control to
 *  NEXUS_MpodOpenSettings
 *
 * 5   7/7/08 4:44p erickson
 * PR44486: added NEXUS_MpodOpenSettings.pod2ChipMClkMuxSelect
 *
 * 4   6/18/08 1:28p erickson
 * PR43730: fix warning, remove unneeded handle in GetDefaultSettings
 *
 * 3   5/21/08 1:56p shyam
 * PR 42103 : Fix comments
 *
 * 2   5/21/08 11:16a erickson
 * PR42103: comment changes
 *
 * 1   5/20/08 7:47p shyam
 * PR 42103 : Add transport MPOD support
 *
 *****************************************************************************/

#ifndef NEXUS_MPOD_H__
#define NEXUS_MPOD_H__

#include "nexus_types.h"
#include "nexus_playpump.h"
#include "nexus_parser_band.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle which represents an Mpod device.
**/
typedef struct NEXUS_Mpod *NEXUS_MpodHandle;

/**
Summary:
Handle which represents an Mpod input channel.
**/
typedef struct NEXUS_MpodInput *NEXUS_MpodInputHandle;

/**
Summary:
The type of Mpod device interface
**/
typedef enum NEXUS_MpodMode
{
    NEXUS_MpodMode_eSpod, /* SPOD Cable Card interface */
    NEXUS_MpodMode_eMpod, /* MPOD Cable Card interface */
    NEXUS_MpodMode_eMax
} NEXUS_MpodMode;

/**
Summary:
Enum used in NEXUS_MpodOpenSettings to select input source to Mpod.
**/
typedef enum NEXUS_MpodBandType
{
    NEXUS_MpodBandType_eParserBand,
    NEXUS_MpodBandType_ePlaypump,
    NEXUS_MpodBandType_eMax
} NEXUS_MpodBandType;

/**
Summary:
Open settings for an Mpod device interface
**/
typedef struct NEXUS_MpodOpenSettings
{
    NEXUS_MpodMode mpodMode;

    bool enableBandAssociation; /* A virtual Parser band can be associated with a data stream
                                   for data that originates in the S-Pod. The parser band assocated can be a playback or a inputband parser.
                                   determined by bandType field if enabled using enableBandAssociation. */

    NEXUS_MpodBandType bandType; /* Determines the use of band union */
    struct
    {
        NEXUS_ParserBand parserBand;
        NEXUS_PlaypumpHandle playpump;
    } band;                     /* input source used for NEXUS_MpodMode_eSpod */


    bool timestampInsertEnable; /* Replace the timestamp received from the interface with a new internally generated one. */

    /* Output Interface Formatter Control */
    bool byteSync; /* If true, byte sync is enabled */
    bool clockNotRunning; /* If true, the output clock will stop when data is not being output (CLK_NRUN). */
    bool invertClock; /* If true, clock is active low, else active high */
    unsigned clockDelay; /* Delay of output clock in 108MHz cycles (0=none) */
    bool outputInvertSync;

    bool pod2ChipMClkMuxSelect; /* Selects which pin is used for the Pod2Chip MCLK.
                                    false - Use the POD2CHIP_MCLKI input pin (default).
                                    true - Use a GPIO input pin. The pin number and pin mux configuration is chip or board specific. */
} NEXUS_MpodOpenSettings;

/**
Summary:
Enable routing of parser band (for live) or playback to Mpod.

band and bandType are only used in NEXUS_Mpod_Open.
**/
typedef struct NEXUS_MpodInputSettings
{
    NEXUS_MpodBandType bandType; /* determines the use of band union */
    union
    {
        NEXUS_PlaypumpHandle playpump;
        NEXUS_ParserBand parserBand;
    } band;
    bool allPass; /* Used for debug mode */
    bool mpodPreFilter; /* If true, enable filtering on the data stream */
} NEXUS_MpodInputSettings;

/**
Summary:
Get default settings for NEXUS_Mpod_Open
**/
void NEXUS_Mpod_GetDefaultOpenSettings(
    NEXUS_MpodOpenSettings* pOpenSettings /* [out] */
    );

/**
Summary:
Opens the Mpod device
**/
NEXUS_MpodHandle NEXUS_Mpod_Open(
    unsigned int index,
    const NEXUS_MpodOpenSettings *pOpenSettings
    );

/**
Summary:
Closes the Mpod device
**/
void NEXUS_Mpod_Close(
    NEXUS_MpodHandle mpod
    );

/**
Summary:
Gets default Mpod Input settings for use in NEXUS_MpodInput_Open
**/
void NEXUS_MpodInput_GetDefaultSettings(
    NEXUS_MpodInputSettings *pMpodInputSettings /* [out] */
    );

/**
Summary:
Opens an Mpod input channel
**/
NEXUS_MpodInputHandle NEXUS_MpodInput_Open(
    NEXUS_MpodHandle mpod,
    const NEXUS_MpodInputSettings *pMpodInputSettings
    );

/**
Summary:
Closes an Mpod input channel
**/
void NEXUS_MpodInput_Close(
    NEXUS_MpodInputHandle mpod
    );

/**
Summary:
Gets Mpod input channel settings
**/
void NEXUS_MpodInput_GetSettings(
    NEXUS_MpodInputHandle mpodInput,
    NEXUS_MpodInputSettings *pMpodInputSettings /* [out] */
    );

/**
Summary:
Sets Mpod input channel settings
**/
NEXUS_Error NEXUS_MpodInput_SetSettings(
    NEXUS_MpodInputHandle mpodInput,
    const NEXUS_MpodInputSettings *pMpodInputSettings
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_MPOD_H__ */

