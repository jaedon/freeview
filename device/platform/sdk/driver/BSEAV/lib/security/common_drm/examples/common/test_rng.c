/***************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: test_rng.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 4/19/12 9:58a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/common_drm/examples/common/test_rng.c $
 * 
 * 1   4/19/12 9:58a celysee
 * SW7425-2894: Support for DTCP-IP with robustness rules
 * 
 * 1   10/1/10 2:18p cdisc
 * SW35230-1564: adding test app for RNG in DRM common
 * 
 * 2   9/13/10 6:57p cdisc
 * SW35230-1342: fixing example to use DRM wrapper for setting extern DMA
 *  handle
 * 
 * 1   9/13/10 5:49p cdisc
 * SW35230-1343: adding secure store example
 * 
 * 1   8/23/10 1:57p cdisc
 * SW35230-1125: adding drm test app
 *
 ***************************************************************************/

/**
 * @brief Defines the API for Netflix DRM module.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "nexus_platform.h"
#include "nexus_memory.h"

#include "bstd.h"
#include "bdbg.h" 
#include "bkni.h"
#include "drm_init.h"
#include "drm_common.h"
#include "drm_common_swcrypto_types.h"
#include "drm_common_swcrypto.h"
#include "drm_key_region.h"


/******************************************************************************/

int main(int argc, char *argv[])
{   
    uint8_t pBuf[360] = {0x00};
    int numBytes = 0;
    int i = 0;

    if(argc != 2){
        printf("Please specify number of bytes to generate (max is 360):\n");
        printf("%s 8 \n", argv[0]);
        return -1;
     }

   printf("Converting '%s' to integer\n", argv[1]);
   numBytes = atoi(argv[1]);
   if(numBytes < 0)
   {
	   printf("Main - Error converting argument '%s' to integer, exiting\n", argv[1]);
	   exit(0);
   }
   printf("\nMain - Generating '%u' bytes\n", numBytes);

   if(DRM_Common_GenerateRandomNumber(numBytes, pBuf) != Drm_Success)
   {
	   printf("Main - Error calling 'DRM_Common_GenerateRandomNumber'");
   }
   else
   {
	   for(i = 0; i < numBytes; i++)
	   {
		   printf("%02x ", pBuf[i]);
		   if(((i+1) % 16) == 0)printf("\n");
	   }
   }
   printf("\nExiting\n");
   return 0;
}


