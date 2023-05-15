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
 * $brcm_Workfile: parser_band_prot_client.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 11/4/11 10:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/parser_band/parser_band_prot_client.c $
 * 
 * Hydra_Software_Devel/2   11/4/11 10:25p bandrews
 * SW7231-446: update unit tests
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

BDBG_MODULE(parser_band_prot_client);

int main(int argc, char **argv)  {
    NEXUS_Error rc;
    NEXUS_ParserBand band;
    NEXUS_ParserBandSettings settings;

    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    printf("Starting parser band protected client\n");

    NEXUS_Platform_Join();

    printf("Client using parser band 0...");
    NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e0, &settings);
    settings.allPass = true;
    rc = NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e0, &settings); /* this should fail, protected */
    BDBG_ASSERT(rc);
    printf("PASS\n");

    printf("Client opening parser band 0..."); /* this should fail, server used unprotected */
    BDBG_ASSERT((void *)NEXUS_ParserBand_Open(0) == NULL);
    printf("PASS\n");

    printf("Client opening parser band 1...");
    band = NEXUS_ParserBand_Open(1); /* this should succeed */
    BDBG_ASSERT((unsigned)band > (unsigned)NEXUS_ParserBand_eMax);
    printf("PASS\n");
    printf("Client closing parser band 1...");
    NEXUS_ParserBand_Close(band);
    printf("PASS\n");

    printf("Client opening next available parser band (1)...");
    band = NEXUS_ParserBand_Open(NEXUS_ANY_ID);
    BDBG_ASSERT((void *)band != NULL);
    printf("PASS\n");

    printf("Client opening parser band 1..."); /* should fail */
    BDBG_ASSERT((void *)NEXUS_ParserBand_Open(1) == NULL);
    printf("PASS\n");

    printf("Client using acquired parser band (1)...");
    NEXUS_ParserBand_GetSettings(band, &settings);
    settings.allPass = true;
    rc = NEXUS_ParserBand_SetSettings(band, &settings);
    BDBG_ASSERT(!rc);
    printf("PASS\n");

    printf("Client closing acquired parser band (1)...");
    NEXUS_ParserBand_Close(band);
    printf("PASS\n");

    printf("Client using closed parser band (1)...");
    NEXUS_ParserBand_GetSettings(band, &settings);
    settings.allPass = true;
    rc = NEXUS_ParserBand_SetSettings(band, &settings);
    BDBG_ASSERT(rc);
    printf("PASS\n");

    printf("Client using NULL parser band...");
    NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)NULL, &settings);
    settings.allPass = true;
    rc = NEXUS_ParserBand_SetSettings((NEXUS_ParserBand)NULL, &settings);
    BDBG_ASSERT(rc);
    printf("PASS\n");

    printf("Client closing closed parser band...");
    NEXUS_ParserBand_Close(band);
    printf("PASS\n");

    printf("Client closing NULL parser band...");
    NEXUS_ParserBand_Close((NEXUS_ParserBand)NULL);
    printf("PASS\n");

    printf("Client opening next available parser band (1)...");
    band = NEXUS_ParserBand_Open(NEXUS_ANY_ID);
    BDBG_ASSERT((void *)band != NULL);
    printf("PASS\n");

    printf("Client opening parser band 1..."); /* should fail */
    BDBG_ASSERT((void *)NEXUS_ParserBand_Open(1) == NULL);
    printf("PASS\n");

    printf("Client closing acquired parser band (1)...");
    NEXUS_ParserBand_Close(band);
    printf("PASS\n");

    NEXUS_Platform_Uninit();

    return 0;
}

