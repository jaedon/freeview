/******************************************************************************
 *    (c)2011 Broadcom Corporation
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
 * $brcm_Workfile: test_multi_thread_partial_sha1.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 4/19/12 9:58a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/common_drm/examples/common/test_multi_thread_partial_sha1.c $
 * 
 * 1   4/19/12 9:58a celysee
 * SW7425-2894: Support for DTCP-IP with robustness rules
 * 
 * dtv_v3.0/1   6/15/11 5:31p cdisc
 * SWDTV-7493:[ see Broadcom Issue Tracking JIRA for more info ].
 *  General_Comments: This checkin adds support for Openssl functionality
 *  and multithreading
 * File_Specific_Comments:
 * Users should see drm_common_swcrypto.h for new API definitions.
 * Reviewed_By:
 * QA_Concerns:
 * What_Else_Affected: N/A
 * Unit_Testing_Performed: Netflix/WV work on CARD and Vizio 35230.
 * Review_Comments: N/A
 * 
 * dev_dtv_v3.0_SWDTV-7493/1   6/15/11 11:39a cdisc
 * SWDTV-7493: adding openssl and multithread support
 * 
 *****************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <linux/unistd.h>
#include <pthread.h>

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

static void test_partial_sha1(void);
static void test_partial_sha1_loop(void);
static void test_sha1(void);
static void test_sha1_loop(void);

/* Global error variable counter */
static uint32_t error_count = 0;


/******************************************************************************/
int main(int argc, char *argv[])
{   
    int i = 0;
    const int NUM_THREADS = 6;
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    printf("\n\tMain - Starting test app (%s) ^^^^^^^^^^^^^^^^^^^^^^^\n", argv[1]);

    DRM_KeyRegion_SetCustDrmFilePath( argv[1]);

    DRM_Init_Module();

    printf("\tMain - Opened BCRYPT ^^^^^^^^^^^^^^^^^^^^^^^\n");

    for(i=0;i<NUM_THREADS;i++)
    {
   		if(i==0||i==2||i==4){
   			pthread_create(&thread[i], NULL, test_partial_sha1_loop, NULL);
   		}
   		else{
   			pthread_create(&thread[i], NULL, test_sha1_loop, NULL);
   		}

   		test_partial_sha1();

    /*printf("\n\n\t%s - Number of test cases that failed = '%u'\n", __FUNCTION__, error_count);
    printf("\t%s - Number of test cases that failed = '%u'\n", __FUNCTION__, error_count);
    printf("\t%s - Exiting\n", __FUNCTION__);*/

    /*pthread_join(thread[i], NULL);*/
    }

    for(i=0;i<NUM_THREADS;i++)
    {
    	pthread_join(thread[i], NULL);
    }
    return 0;
}


void test_partial_sha1_loop()
{
	int i = 0;
	for(i = 0; i<10;i++)
	{
		test_partial_sha1();
	}
	return;
}

void test_sha1_loop()
{
	int i = 0;
	for(i = 0; i<10;i++)
	{
		test_sha1();
	}
	return;
}


void test_partial_sha1()
{
    int i = 0;
    int curr_data_size = 0;
    uint8_t pShaBuf[32] = {0};
    DrmCommon_PartialSha1Type partialSha1Type;
    uint32_t context = 0;

    uint8_t *pCurrData = NULL;
    uint8_t pBuf[32] = {0xa8, 0xea, 0xc9, 0x81, 0x9b, 0xf0, 0xec, 0xcc,
    					0xa9, 0x04, 0xc4, 0x7f, 0xb1, 0x8c, 0x22, 0xa4,
    					0x1a, 0xaf, 0x6c, 0x05, 0xe8, 0x9e, 0xdb, 0xcf,
    					0x76, 0x5b, 0x21, 0xbd, 0x20, 0xf6, 0x19, 0x38};

    uint8_t pCompareSha1Hash[32] = {0x2e, 0x43, 0x7b, 0x40, 0x28, 0x8f, 0x1d, 0x89,
    							0x7e, 0xdd, 0xc8, 0x61, 0x32, 0x19, 0x9e, 0xce,
    							0x3b, 0x9e, 0xa0, 0xcc, 0x00, 0x00, 0x00, 0x00,
    							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	printf("\n\t%s - Partial SHA-1 test ^^^^^ tid = (0x%08x)\n", __FUNCTION__, pthread_self());

	for(i=0; i<3; i++)
	{
		switch(i)
		{
			case 0:
				/*printf("\t%s - Partial SHA-1 Init\n", __FUNCTION__);*/
				pCurrData = pBuf;
				curr_data_size = 8;
				partialSha1Type = DrmCommon_PartialSha1Type_Init;
			break;

			case 1:
				/*printf("\t%s - Partial SHA-1 Update\n", __FUNCTION__);*/
				pCurrData = &pBuf[8];
				curr_data_size = 16;
				partialSha1Type = DrmCommon_PartialSha1Type_Update;
			break;

			case 2:
				/*printf("\t%s - Partial SHA-1 Finalize\n", __FUNCTION__);*/
				pCurrData = &pBuf[24];
				curr_data_size = 8;
				partialSha1Type = DrmCommon_PartialSha1Type_Finalize;
			break;

		}
		DRM_Common_SwPartialSha1(pBuf, pShaBuf, 32, partialSha1Type, &context);
	}

	/*printf("\t%s - Calculated Full SHA-1 :\n\t\t", __FUNCTION__);
	for(i = 0; i < 20; i++)
	{
	   printf("0x%02x ", pShaBuf[i]);
	}*/

	if(memcmp(pCompareSha1Hash, pShaBuf, 20) == 0)
	{
	   printf("\n\tMain - SHA-1 Test PASSED!!\n\n");
	}
	else
	{
	   printf("\n\tMain - SHA-1 Test FAILED!!\n\n");
	   error_count++;
	   exit(0);
	}
	return;
}


void test_sha1()
{
    int i = 0;
    uint8_t pShaBuf[32] = {0};

    uint8_t pBuf[32] = {0xa8, 0xea, 0xc9, 0x81, 0x9b, 0xf0, 0xec, 0xcc,
    					0xa9, 0x04, 0xc4, 0x7f, 0xb1, 0x8c, 0x22, 0xa4,
    					0x1a, 0xaf, 0x6c, 0x05, 0xe8, 0x9e, 0xdb, 0xcf,
    					0x76, 0x5b, 0x21, 0xbd, 0x20, 0xf6, 0x19, 0x38};

    uint8_t pCompareSha1Hash[32] = {0x2e, 0x43, 0x7b, 0x40, 0x28, 0x8f, 0x1d, 0x89,
    							0x7e, 0xdd, 0xc8, 0x61, 0x32, 0x19, 0x9e, 0xce,
    							0x3b, 0x9e, 0xa0, 0xcc, 0x00, 0x00, 0x00, 0x00,
    							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	printf("\n\t%s - Entered SHA-1 test ^^^^^^^^^^^^^^^^^^^^^^^\n", __FUNCTION__);
	DRM_Common_SwSha1(pBuf, pShaBuf, 32);

	printf("\tMain - Calculated SHA-1 :\n\t\t");
	for(i = 0; i < 20; i++)
	{
	   printf("0x%02x ", pShaBuf[i]);
	}

	if(memcmp(pCompareSha1Hash, pShaBuf, 20) == 0)
	{
	   printf("\n\tMain - SHA-1 Test PASSED!!\n\n");
	}
	else
	{
	   printf("\n\tMain - SHA-1 Test FAILED!!\n\n");
	   exit(0);
	}
	return;
}
