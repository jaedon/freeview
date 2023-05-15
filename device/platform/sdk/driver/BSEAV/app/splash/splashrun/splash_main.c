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
* $brcm_Revision: 7 $
* $brcm_Date: 9/10/12 4:40p $
*
* Module Description:
*   This file is meant for unit testing of RAP PI for 7401. This file
*   contains the implementation of Functions and Data Structures to test
*   different features of RAP PI.
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/splash/splashrun/splash_main.c $
* 
* 7   9/10/12 4:40p jessem
* SW7425-3872: Removed hMem1 use with 7425.
* 
* 6   4/18/12 12:41p jessem
* SW7425-2828: Corrected method on obtaining heap.
*
* 5   4/10/12 2:16p jessem
* SW7425-2828: Used NEXUS_Platform_GetFrameBufferHeap to determine
* correct heap to use.
*
* 4   3/19/12 1:39p jessem
* SW7425-2653: Changed MEMC1 to use heap2 for 7425.
*
* 3   8/8/11 11:53p nickh
* SW7425-878: Add 7425 support
*
* 2   9/21/09 5:32p nickh
* SW7420-351: Expand functions to provide heap for MEMC1 required by 7420
*
* 1   4/8/09 4:18p shyam
* PR52386 : Build system now based on nexus
*
* 2   4/8/09 12:52p shyam
* PR52386 : Port splash to nexus Base Build system
*
* 1   9/29/08 11:34a erickson
* PR46184: added splash app
*
* 1   5/14/07 7:15p shyam
* PR 30741 : Add reference support for generic portable splash
*
***************************************************************************/
/* #define debug_only */
/* Linux stuff */
#include <stdio.h>          /* for printf */
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
#include "bkni.h"

#include "bmem.h"
#include "bint.h"

#include "nexus_platform.h"
#ifndef debug_only
#include "priv/nexus_core.h"
#endif
#include "nexus_types.h"


BDBG_MODULE(splashrun);

int splash_script_run(BREG_Handle hReg, BMEM_Handle hMem0, BMEM_Handle hMem1);

int main(void)
{

    NEXUS_Error rc;
    NEXUS_PlatformSettings platformSettings;
    BMEM_Heap_Handle hMem0 = NULL, hMem1 = NULL;
	NEXUS_MemoryStatus stat;
    int i;

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

#ifdef debug_only
    splash_script_run(NULL,NULL,NULL);
#else
    splash_script_run(g_pCoreHandles->reg, hMem0, hMem1);
#endif

    NEXUS_Platform_Uninit();
    return 0;

}

void APP_BREG_Write32(BREG_Handle RegHandle, uint32_t reg, uint32_t data)
{
	BSTD_UNUSED(RegHandle) ;
	BSTD_UNUSED(reg) ;
	BSTD_UNUSED(data) ;
}
/* End of File */
