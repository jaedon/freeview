/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: mpod.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/18/08 5:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/mpod.c $
 * 
 * 3   6/18/08 5:38p erickson
 * PR43814: fix app
 * 
 * 2   5/21/08 3:08p shyam
 * PR42103 : Fix incomplete passing of params
 * 
 * 1   5/20/08 7:49p shyam
 * PR 42103 : Add transport MPOD support
 * 
 *
******************************************************************************/
/* Nexus example app: mpod exercise example */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"

#include "nexus_mpod.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;

    NEXUS_PlaypumpHandle playpump;

	NEXUS_MpodHandle mpod;
	NEXUS_MpodInputHandle mpodInput, mpodInputPb ;
	NEXUS_MpodOpenSettings mpodOpenSettings ;
	NEXUS_MpodInputSettings mpodInputSettings ;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

	NEXUS_Mpod_GetDefaultOpenSettings(&mpodOpenSettings);
	mpodOpenSettings.mpodMode = NEXUS_MpodMode_eSpod;
	mpodOpenSettings.enableBandAssociation = true;
	mpodOpenSettings.bandType = NEXUS_MpodBandType_eParserBand;
	mpodOpenSettings.band.parserBand = NEXUS_ParserBand_e0 ;
	mpod = NEXUS_Mpod_Open(0, &mpodOpenSettings) ; /* Index always 0 */

	NEXUS_MpodInput_GetDefaultSettings(&mpodInputSettings);
	mpodInputSettings.bandType = NEXUS_MpodBandType_eParserBand ;
	mpodInputSettings.band.parserBand = NEXUS_ParserBand_e0 ;
	mpodInputSettings.allPass = true ;
	mpodInput = NEXUS_MpodInput_Open(mpod, &mpodInputSettings) ;

	printf("Press any key to route playback to Mpod.... ");
    getchar();
    playpump = NEXUS_Playpump_Open(1, NULL);

	NEXUS_MpodInput_GetDefaultSettings(&mpodInputSettings);
	mpodInputSettings.bandType = NEXUS_MpodBandType_ePlaypump ;
	mpodInputSettings.band.playpump = playpump;
	mpodInputPb = NEXUS_MpodInput_Open(mpod, &mpodInputSettings) ;

	printf("Press any key to end program .... ");
    getchar();
	NEXUS_MpodInput_Close(mpodInputPb) ;
	NEXUS_MpodInput_Close(mpodInput) ;
	NEXUS_Mpod_Close(mpod) ;
    return 0;
}
