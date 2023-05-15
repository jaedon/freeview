/***************************************************************************
*     (c)2005-2009 Broadcom Corporation
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
* $brcm_Workfile: splash_main.c $
* $brcm_Revision: 11 $
* $brcm_Date: 9/10/12 4:40p $
*
* Module Description:
*   This file is meant for unit testing of RAP PI for 7401. This file
*   contains the implementation of Functions and Data Structures to test
*   different features of RAP PI.
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/splash/splashgen/splash_main.c $
* 
* 11   9/10/12 4:40p jessem
* SW7425-3872: Removed hMem1 use with 7425.
* 
* 10   9/4/12 4:39p jessem
* SW7425-3872: NULLed hMem1 for 7425 to reflect reassignment of GFD0 and
* GFD1 to MEMC0.
*
* 9   4/18/12 2:47p jessem
* SW7425-2828: Corrected method on obtaining heap. Added run-time options
* to set the display format and to disable the 2nd display. Backed-out
* of creating a separate surface for the the 2nd display.
*
* 8   4/9/12 5:04p jessem
* SW7425-2828: Used NEXUS_Platform_GetFrameBufferHeap to determine
* correct heaps to use.
*
* 7   3/16/12 4:50p jessem
* SW7425-2653: Changed MEMC1 heap assignment for 7425.
*
* 6   11/11/11 11:03a jessem
* SW7425-1527: Enclosed label "error" within SPLASH_NOHDM.
*
* 5   8/8/11 11:12p nickh
* SW7425-878: Add 7425 support
*
* 4   4/9/10 3:31p rjain
* SW7550-284: fixing for SPLASH_NOHDM
*
* 3   9/21/09 5:13p nickh
* SW7420-351: Expand functions to provide heap for MEMC1 required by 7420
*
* 2   5/14/09 5:41p shyam
* PR52592 : Add support for ARGB8888 surfaces
*
* 1   4/8/09 4:16p shyam
* PR52386 : Move splash build system to nexus
*
* 3   4/8/09 12:44p shyam
* PR52386 : Port splash to nexus Base Build system
*
* 2   12/24/08 5:03p katrep
* PR50711: Add splash support for 7405/7335/7325 and settop chips
*
* 1   9/29/08 11:34a erickson
* PR46184: added splash app
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bstd.h"
#include "bkni.h"
#include "bmem.h"
#include "bint.h"
#include "bi2c.h"

#include "nexus_platform.h"
#ifndef debug_only
#include "priv/nexus_core.h"
#endif
#include "nexus_types.h"
#include "splash_vdc_rulgen.h"
#include "bsplash_board.h"

extern ModeHandles g_stModeHandles;


#ifdef debug_only
int splash_capture(void* hChp, void* hReg, void* hMem, void* hMem1, void* hInt, void* hRegI2c) ;
#else
int splash_capture(BCHP_Handle hChp, BREG_Handle hReg, BMEM_Heap_Handle hMem0, BMEM_Heap_Handle hMem1, BINT_Handle hInt, BREG_I2C_Handle hRegI2c, BFMT_VideoFmt eDisplayFmt, BFMT_VideoFmt eDisplayFmt2, bool noSd);
#endif

BDBG_MODULE(splashgen);

void print_usage(void)
{
	printf("\nUsage:\n");
	printf("\t -d <format> - For format, specify one of the following.\n");
	printf("\t               Use the number in the parenthesis.\n");
	printf("\t               480p(0), 576p(1), 720p60(2), 720p50(3),\n");
	printf("\t               1080i60(4), 1080i50(5), 1080p60(6), 1080p50(7)\n\n");
	printf("\t               Default is 480p60.\n");
	printf("\n\t -no_sd      - means no 2nd display. 2nd display is enabled by default.\n");
	printf("\t               If 2nd display is desired, the SD format will be based\n");
	printf("\t               on the 1st display format. For example, if the 1st\n");
	printf("\t               display format is 720p60, the 2nd display format\n");
	printf("\t               will be NTSC; if the 1st display format is 1080p50,\n");
	printf("\t               the 2nd display format will be PAL.\n");
	printf("\n\t -h          - prints this message\n");
}

int main(int argc, char *argv[])
{
    NEXUS_Error rc;
    NEXUS_PlatformSettings platformSettings;
    BREG_I2C_Handle i2cRegHandle = NULL;
    BMEM_Heap_Handle hMem0 = NULL, hMem1 = NULL;
	NEXUS_MemoryStatus stat;
    int i;
	uint32_t format;
	bool noSd = false;
    BFMT_VideoFmt eDisplayFmt;
    BFMT_VideoFmt eDisplayFmt2;

#ifndef SPLASH_NOHDM
    BI2C_Handle i2cHandle;
    BI2C_ChannelSettings i2cChanSettings;
    BI2C_ChannelHandle i2cChanHandle;
#endif

    /* Process all the command options */
    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-d"))
        {
            format = strtol(argv[++i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-no_sd"))
        {
            noSd = true;
        }
        else
        {
            print_usage();
            return -1;
        }
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);

    platformSettings.openI2c = false;
    platformSettings.openFrontend = false;
    platformSettings.openFpga=false;
    platformSettings.openOutputs = false;
#if NEXUS_DTV_PLATFORM
    platformSettings.displayModuleSettings.panel.dvoLinkMode = NEXUS_PanelOutputLinkMode_eSingleChannel1;
#endif

    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

	for (i=0; i<NEXUS_MAX_HEAPS; i++)
	{
		rc = NEXUS_Heap_GetStatus(g_pCoreHandles->nexusHeap[i], &stat);
		if (!rc)
		{
			if (stat.memcIndex==0 && stat.memoryType==7)  /* look for MEMC0 */
			{
				hMem0 = NEXUS_Heap_GetMemHandle(g_pCoreHandles->nexusHeap[i]);
			}
#if (BCHP_CHIP == 7420 || BCHP_CHIP == 7435)
			if (stat.memcIndex==1 && stat.memoryType==7) /* look for MEMC1 */
			{
				hMem1 = NEXUS_Heap_GetMemHandle(g_pCoreHandles->nexusHeap[i]);
			}

			if (hMem0 && hMem1) break;
#else
    		hMem1 = NULL;
			if (hMem0) break;
#endif
		}
		else
		{
			BDBG_ERR(("Failed to get heaps."));
			return 1;
		}
	}

#ifndef SPLASH_NOHDM
    rc = BI2C_Open( &i2cHandle, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, NULL);
    if ( rc!=NEXUS_SUCCESS )
    {
        BDBG_ERR(("Failed to open i2c"));
        goto error;
    }
    BI2C_GetChannelDefaultSettings(i2cHandle, B_I2C_CHANNEL_HDMI, &i2cChanSettings);

    i2cChanSettings.clkRate = BI2C_Clk_eClk100Khz;
    rc = BI2C_OpenChannel(i2cHandle, &i2cChanHandle, B_I2C_CHANNEL_HDMI, &i2cChanSettings);
    if ( rc!=NEXUS_SUCCESS )
    {
        BDBG_ERR(("Failed to open i2c channel "));
        goto error;
    }
    rc = BI2C_CreateI2cRegHandle(i2cChanHandle, &i2cRegHandle);
    if ( rc!=NEXUS_SUCCESS )
    {
        BDBG_ERR(("Failed to open reg handle"));goto error;
    }
#endif

#ifdef debug_only
    splash_capture(
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL);
#else

	/* set 1st and 2nd display formats */
	switch (format)
	{
		case 0:
			eDisplayFmt = BFMT_VideoFmt_e480p;
			eDisplayFmt2 = BFMT_VideoFmt_eNTSC;
			break;
		case 1:
			eDisplayFmt = BFMT_VideoFmt_e576p_50Hz;
			eDisplayFmt2 = BFMT_VideoFmt_ePAL_I;
			break;
		case 2:
			eDisplayFmt = BFMT_VideoFmt_e720p;
			eDisplayFmt2 = BFMT_VideoFmt_eNTSC;
			break;
		case 3:
			eDisplayFmt = BFMT_VideoFmt_e720p_50Hz;
			eDisplayFmt2 = BFMT_VideoFmt_ePAL_I;
			break;
		case 4:
			eDisplayFmt = BFMT_VideoFmt_e1080i;
			eDisplayFmt2 = BFMT_VideoFmt_eNTSC;
			break;
		case 5:
			eDisplayFmt = BFMT_VideoFmt_e1080i_50Hz;
			eDisplayFmt2 = BFMT_VideoFmt_ePAL_I;
			break;
		case 6:
			eDisplayFmt = BFMT_VideoFmt_e1080p;
			eDisplayFmt2 = BFMT_VideoFmt_eNTSC;
			break;
		case 7:
			eDisplayFmt = BFMT_VideoFmt_e1080p_50Hz;
			eDisplayFmt2 = BFMT_VideoFmt_ePAL_I;
			break;
		default:
			eDisplayFmt = BFMT_VideoFmt_e480p;
			eDisplayFmt2 = BFMT_VideoFmt_eNTSC;
			break;
	}

    splash_capture(
                  g_pCoreHandles->chp,
                  g_pCoreHandles->reg,
                  hMem0,
                  hMem1,
                  g_pCoreHandles->bint,
                  i2cRegHandle,
                  eDisplayFmt,
                  eDisplayFmt2,
                  noSd);
#endif

#ifndef SPLASH_NOHDM
    error:
    BI2C_CloseI2cRegHandle(i2cRegHandle) ;
    BI2C_CloseChannel(i2cChanHandle) ;
    BI2C_Close(i2cHandle) ;
#endif
    NEXUS_Platform_Uninit();
    return 0;
}

/* End of File */

