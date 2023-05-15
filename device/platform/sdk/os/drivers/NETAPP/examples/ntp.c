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
 * $brcm_Workfile: ntp.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 12/2/11 3:04p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/examples/ntp.c $
 * 
 * 1   12/2/11 3:04p steven
 * SW7425-1404: Fixing NTP for STB
 *****************************************************************************/
/* NetApp Test Application */
#include <stdio.h>
#include <sys/time.h>
#include "netapp.h"
#include <unistd.h>

#define CHECK(api)                          \
{                                           \
    if (api != NETAPP_SUCCESS)              \
    {                                       \
        printf("%s() line %d failed\n", __FUNCTION__, __LINE__);       \
        goto err_out;                       \
    }                                       \
}

/* Main Routine */
int main(int argc, char* argv[])
{
    NETAPP_HANDLE   hNetApp = NULL;
    struct timeval  tv;
    struct timezone tz;

    printf("######################################################\n");
    printf("           NetApp  %s Test\n", argv[0]);
    printf("#####################################################\n");


    CHECK(NetAppOpen(&hNetApp, NULL, NULL, NULL));

    gettimeofday(&tv, &tz);
    printf("Time=%d\n", (int)tv.tv_sec);

    CHECK(NetAppNtpSetDate(hNetApp, 2000));

    while (1)
    {
        gettimeofday(&tv, &tz);
        printf("Time=%d\n", (int)tv.tv_sec);
        sleep(1);
    }


err_out:
    printf("Exiting now!\n");
    if (hNetApp != NULL)
    {
        NetAppClose(hNetApp);
    }
    return 0;
}

