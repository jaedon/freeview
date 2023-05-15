/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
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
 * $brcm_Workfile: tune_oob.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 10/8/12 1:08p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/examples/frontend/tune_oob.c $
 * 
 * 8   10/8/12 1:08p erickson
 * SW7425-3782: rework mtsif
 * 
 * 7   8/7/12 5:37p mphillip
 * SW7231-924: Merge NEXUS_Frontend_Acquire changes to main
 * 
 * cng_unittest/2   8/3/12 5:08p cng
 * SW7231-924: Add clean up for exiting
 * 
 * cng_unittest/1   7/30/12 5:00p cng
 * SW7231-924: Use NEXUS_Frontend_Acquire
 * 
 * 6   6/6/12 7:01p jtna
 * SW7425-2752: fix OOB data routing
 * 
 * 5   5/10/12 7:32p jtna
 * SW7425-2752: update nexus examples to use
 *  NEXUS_ParserBandSourceType_eMtsif for MTSIF-based frontends
 * 
 * 4   1/30/12 6:41p rjlewis
 * SW7552-147: fixed input band.
 * 
 * 3   12/19/11 3:13p erickson
 * SW7425-1991: Corrupted OOB data
 * 
 * 2   2/25/10 11:38a mphillip
 * SW7550-277: Guard record use for platforms lacking record
 * 
 * 1   2/26/09 3:02p vishk
 * PR 46436: AOB PI, Add support for BCM3117 and BCM3114.
 * 
 *****************************************************************************/
/* Example to tune a OOB channel using nexus */

#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#include "nexus_recpump.h"
#endif
#include "nexus_pid_channel.h"
#include "bdbg.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(tune_oob);

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendOutOfBandStatus oobStatus;

    BSTD_UNUSED(param);

    fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

    NEXUS_Frontend_GetOutOfBandStatus(frontend, &oobStatus);
    fprintf(stderr, "OOB Lock callback, frontend 0x%08x - lock status %d, %d\n", (unsigned)frontend, 
        oobStatus.isFecLocked, oobStatus.isQamLocked);
}

int main(int argc, char **argv)
{
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend = NULL;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendAcquireSettings settings;
    unsigned symbolrate, loops = 2;
    NEXUS_FrontendOutOfBandSettings oobSettings;
    NEXUS_Error rc;
    NEXUS_FrontendOutOfBandStatus oobStatus;
    NEXUS_FrontendOutOfBandMode mode = NEXUS_FrontendOutOfBandMode_eDvs167Qpsk;
    NEXUS_PidChannelHandle allpassPidChannel;
    NEXUS_PidChannelSettings pidChannelSettings;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
#if NEXUS_HAS_RECORD
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_RecordSettings recordCfg;
    NEXUS_RecordPidChannelSettings pidCfg;
    NEXUS_FileRecordHandle file;
    const char *fname = "/mnt/hd/videos/oobstream.ts";
#endif

    /* default freq & qam mode */
    unsigned freq = 357;

    /* allow cmdline freq & qam mode for simple test */
    if (argc > 1) {
        mode = atoi(argv[1]);
    }
    if (argc > 2) {
        symbolrate = atoi(argv[2]);
    }

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
    settings.capabilities.outOfBand = true;
    frontend = NEXUS_Frontend_Acquire(&settings);
    if (!frontend) {
        fprintf(stderr, "Unable to find OOB-capable frontend\n");
        return -1;
    }

    
    NEXUS_Frontend_GetDefaultOutOfBandSettings(&oobSettings);
    switch (mode) {
    case NEXUS_FrontendOutOfBandMode_eDvs167Qpsk:
    case NEXUS_FrontendOutOfBandMode_ePod_Dvs167Qpsk:
        freq = 130000000; /*symbolrate = 772000*/
        break;
    case NEXUS_FrontendOutOfBandMode_eDvs178Qpsk:
    case NEXUS_FrontendOutOfBandMode_ePod_Dvs178Qpsk:
        freq = 75250000; /* symbolrate = 1024000 */
        break;
    default:
        fprintf(stderr, "Unsupported modulation type.\n");
        return 0;
    }
    oobSettings.mode = mode;
    oobSettings.symbolRate = symbolrate;
    oobSettings.frequency = freq;
    oobSettings.lockCallback.callback = lock_callback;
    oobSettings.lockCallback.context = frontend;

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);
    /* Map a parser band to the demod's input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    if (userParams.isMtsif) {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(frontend); /* NEXUS_Frontend_TuneXyz() will connect this frontend to this parser band */
    }
    else {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    }
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    parserBandSettings.allPass = true;
    parserBandSettings.acceptNullPackets = true;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    rc = NEXUS_Frontend_TuneOutOfBand(frontend, &oobSettings);
    BDBG_ASSERT(!rc); 
    
    while (loops--) {
        printf("Press enter to get status. \n");
        getchar();
        rc = NEXUS_Frontend_GetOutOfBandStatus(frontend, &oobStatus);
        BDBG_ASSERT(!rc);       
    }

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.pidChannelIndex = NEXUS_ParserBand_e0;
    allpassPidChannel = NEXUS_PidChannel_Open(parserBand, 0, &pidChannelSettings);

#if NEXUS_HAS_RECORD
    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordCfg);
    recordCfg.recpump = recpump;
    NEXUS_Record_SetSettings(record, &recordCfg);
    
    file = NEXUS_FileRecord_OpenPosix(fname,  NULL);
    
    NEXUS_Record_GetDefaultPidChannelSettings(&pidCfg);
    pidCfg.recpumpSettings.pidType = NEXUS_PidType_eUnknown;

    /* Do not generate an index */
    NEXUS_Record_AddPidChannel(record,allpassPidChannel,NULL);

    NEXUS_Record_Start(record, file);
    printf("press ENTER to stop record\n");
    getchar();
    
    NEXUS_Record_Stop(record);
    NEXUS_FileRecord_Close(file);
#endif

    printf("Press enter to stop. \n");
    getchar();

    /* Shutdown */
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
    NEXUS_PidChannel_Close(allpassPidChannel);
    NEXUS_Frontend_Release(frontend);
    NEXUS_Platform_Uninit();
    return 0;
}

