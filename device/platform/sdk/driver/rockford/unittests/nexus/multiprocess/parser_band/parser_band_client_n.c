/******************************************************************************
 *    (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: parser_band_client_n.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/4/11 10:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/parser_band/parser_band_client_n.c $
 * 
 * Hydra_Software_Devel/3   11/4/11 10:25p bandrews
 * SW7231-446: update unit tests
 * 
 * Hydra_Software_Devel/2   10/25/11 4:54p bandrews
 * SW7231-391: unit test update
 * 
 * Hydra_Software_Devel/1   10/11/11 8:53p bandrews
 * SW7231-391: parser band and timebase unit tests
 *****************************************************************************/
#include "nexus_platform_client.h"
#include "nexus_platform.h" 
#include "nexus_parser_band.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(parser_band_client_n);

int main(int argc, char **argv)  {
    NEXUS_ParserBand bands[2];
    int client = 0;
    int total = 0;

    if (argc != 3)
    {
        printf("usage: %s client-index total-clients\n", argv[0]);
        return 1;
    }

    client = atoi(argv[1]);
    total = atoi(argv[2]);

    printf("Starting parser band client %d of %d\n", client, total);

    NEXUS_Platform_Join();

    printf("Client %d acquiring parser band %d...", client, client);
    bands[0] = NEXUS_ParserBand_Open(client);
    BDBG_ASSERT((void *)bands[0] != NULL && (unsigned)bands[0] > (unsigned)NEXUS_ParserBand_eMax);
    printf("PASS\n");

    BKNI_Sleep(2000);

    printf("Client %d attempting to acquire parser band %d...", client, (client + 1) % total);
    BDBG_ASSERT((void *)NEXUS_ParserBand_Open((client + 1) % total) == NULL);
    printf("PASS\n");

    BKNI_Sleep(2000);
        
    printf("Client %d opening next available parser band...", client);
    bands[1] = NEXUS_ParserBand_Open(NEXUS_ANY_ID);
    BDBG_ASSERT((void *)bands[1] != NULL);
    printf("PASS\n");

    printf("Client %d closing acquired parser band %d...", client, client);
    NEXUS_ParserBand_Close(bands[0]);
    printf("PASS\n");

    printf("Client %d closing acquired next available parser band...", client);
    NEXUS_ParserBand_Close(bands[1]);
    printf("PASS\n");

    NEXUS_Platform_Uninit();

    return 0;
}


