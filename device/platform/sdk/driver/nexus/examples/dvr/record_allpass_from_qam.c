/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: record_allpass.c $
 * $brcm_Revision: SMS_93380_DEVEL_SC/3 $
 * $brcm_Date: 9/28/09 1:53p $
 *
 * Module Description: Bring up 93380sms platform
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/dvr/record_allpass.c $
 * 
 * SMS_93380_DEVEL_SC/3   9/28/09 1:53p garetht
 * SW7420-349: fix string issue.
 * 
 * SMS_93380_DEVEL_SC/2   9/25/09 12:00p garetht
 * SW7420-349: SMS bring-up
 * 
 * SMS_93380_DEVEL_SC/1   9/15/09 11:32a garetht
 * SW7420-349: Modified test program for 93380
 * 
 *
 *****************************************************************************/
/* Basic bring-up of a streaming media server */

#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_record.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NEXUS_Error NxsResult;
#define FGprintf(Color, ...) printf("\033[1;%dm", Color);printf(__VA_ARGS__);printf("\033[0m\n");
#define FBprintf(Color, Back, ...) printf("\033[1;%d;%dm", Color, Back);printf(__VA_ARGS__);printf("\033[0m\n");
#define TRACENXS(x) NxsResult=x; if(NxsResult==NEXUS_SUCCESS) {FGprintf(42, "[0x%08x] %s - {%s:%d}", NxsResult, #x, __FUNCTION__, __LINE__);}else {FGprintf(41, "[0x%08x] %s - {%s:%d}", NxsResult,  #x, __FUNCTION__, __LINE__);}
#define TRACENXSR(x, y) x=y;FGprintf(35, "[0x%08lx] %s - {%s:%d}", (long unsigned int)x, #y, __FUNCTION__, __LINE__);

#define NO_INDEX_FILE

typedef struct
{
    NEXUS_FileRecordHandle file;
    NEXUS_RecordHandle record;
    NEXUS_RecpumpHandle recpump;
    NEXUS_PidChannelHandle pid;   
} B_REC_DESC;

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus qamStatus;

    BSTD_UNUSED(param);

    fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

    TRACENXS(NEXUS_Frontend_GetQamStatus(frontend, &qamStatus));
    fprintf(stderr, "QAM Lock callback, frontend 0x%08x - lock status %d, %d\n", (unsigned)frontend,
            qamStatus.fecLock, qamStatus.receiverLock);
}

static void fe_status(void *context)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus qamStatus;

    TRACENXS(NEXUS_Frontend_GetQamStatus(frontend, &qamStatus));
    printf("FE %p:\n", (void *)frontend);
    printf("\t Receiver Lock: %s\n", qamStatus.receiverLock ? "yes" : "no");
    printf("\t FEC Lock: %s\n", qamStatus.fecLock ? "yes" : "no");
    printf("\t Ouput PLL Lock: %s\n", qamStatus.opllLock ? "yes" : "no");
    printf("\t Symbol Rate: %u\n", qamStatus.symbolRate);
    printf("\t Symbol Rate Error: %d\n", qamStatus.symbolRateError);
    printf("\t SNR: %u\n", qamStatus.snrEstimate);
    printf("\t Re-acquisition count: %u\n", qamStatus.reacquireCount);
    printf("\t Good RS count: %u\n", qamStatus.goodRsBlockCount);
    printf("\t ber raw count: %u\n", qamStatus.berRawCount);
}

B_REC_DESC openRecording(char *filename, char *indexfilename, uint32_t freq, NEXUS_FrontendQamMode qamMode, uint32_t tunerNum)
{
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_PlatformConfiguration platformConfig;

    NEXUS_FileRecordHandle file;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidCfg0;
    NEXUS_PidChannelHandle pidCh0;
    NEXUS_RecordPidChannelSettings pidCfg;
    NEXUS_RecordSettings recordCfg;
    B_REC_DESC recordDesc;

printf("**********************************************\n");
printf("FILENAME: %s \n", filename);
printf("INDEX FILENAME: %s \n", indexfilename);
printf("FREQ: %d \n", freq);
printf("QAM MODE: %d \n", (int)qamMode);
printf("TUNER NUMBER: %u \n", tunerNum);
printf("**********************************************\n");


    NEXUS_Platform_GetConfiguration(&platformConfig);
    frontend = platformConfig.frontend[tunerNum];
    NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
    qamSettings.frequency = freq;
    qamSettings.mode = qamMode;
    qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
    qamSettings.lockCallback.callback = lock_callback;
    qamSettings.lockCallback.context = frontend;

printf("**********************************************\n");
printf("FRONTEND HANDLE: %0x \n", (unsigned int)frontend);
printf("**********************************************\n");

    TRACENXS(NEXUS_Frontend_GetUserParameters(frontend, &userParams));

    TRACENXS(NEXUS_Frontend_TuneQam(frontend, &qamSettings));
    
    parserBand = NEXUS_ParserBand_e0 + tunerNum;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    parserBandSettings.allPass=true;
    parserBandSettings.acceptNullPackets=false;
    TRACENXS(NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings));

    NEXUS_PidChannel_GetDefaultSettings(&pidCfg0) ;
    pidCfg0.pidChannelIndex = parserBand;  /* for allpass record, pidChannelIndex = parserBand number */
    /* The pid number is redundant here. */
    TRACENXSR(pidCh0, NEXUS_PidChannel_Open(parserBand, 0x0, &pidCfg0));

    recpump = NEXUS_Recpump_Open(tunerNum, NULL);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordCfg);
    recordCfg.recpump = recpump;
    TRACENXS(NEXUS_Record_SetSettings(record, &recordCfg));

    TRACENXSR(file,NEXUS_FileRecord_OpenPosix(filename, indexfilename));

    NEXUS_Record_GetDefaultPidChannelSettings(&pidCfg);
    pidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidCfg.recpumpSettings.pidTypeSettings.video.index = true;
    pidCfg.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2; /* video codec is necessary to generate correct index */
    /* generate index using pid number instead of pid chanel number */
    pidCfg.recpumpSettings.pidTypeSettings.video.pid = 0x11;

#if 1
    /* Do not generate an index */
    TRACENXS(NEXUS_Record_AddPidChannel(record,pidCh0,NULL));
#else
    /* Generate index using a vid value */
    NEXUS_Record_AddPidChannel(record,pidCh0,&pidCfg);
#endif

    TRACENXS(NEXUS_Record_Start(record, file));
    recordDesc.record = record;
    recordDesc.file = file;
    recordDesc.recpump = recpump;
    recordDesc.pid=pidCh0;
    return recordDesc;
}


void closeRecording(B_REC_DESC *channel)
{
    NEXUS_Record_Stop(channel->record);
    NEXUS_Record_RemoveAllPidChannels(channel->record);
    NEXUS_PidChannel_Close(channel->pid);
    NEXUS_FileRecord_Close(channel->file);
    NEXUS_Record_Destroy(channel->record);
    NEXUS_Recpump_Close(channel->recpump);
}


#define BUFSIZE 50

unsigned getSelection(unsigned min, unsigned max)
{
    char inBuf[BUFSIZE];
    unsigned item;

    while(1)
    {
        fgets(inBuf, BUFSIZE, stdin);
        if(!sscanf(inBuf, "%u", &item) || (item > max) || (!(item >= min))) 
            printf("invalid value! Try Again >");
        else
            break;
    }

    printf("\n");
    return item;
}


int main(void)
{
    unsigned item, channel, freq, mod, i;
    bool exit = false;
    char recName[100], idxName[100];
    unsigned streamCnt[8], count;
    B_REC_DESC currentRecs[8];
    char *filenameStem = "/mnt/hd/";
    unsigned stemSize;
    NEXUS_PlatformConfiguration platformConfig;
    
    stemSize = strlen(filenameStem);

    memset(streamCnt, 0, sizeof(streamCnt)); /* current recording number on a record channel */
    memset(currentRecs, 0, sizeof(currentRecs)); /* record handles for each record channel */

    strcpy(recName, filenameStem);
    strcpy(idxName, filenameStem);
    strcpy(recName + stemSize, "T_0_STRM_0000.mpg");
    strcpy(idxName + stemSize, "T_0_STRM_0000.nav");


    TRACENXS(NEXUS_Platform_Init(NULL));
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    while(!exit)
    {
        printf("\nMENU\n\n");
        printf("1/ Open recording on tuner\n");
        printf("2/ Close recording on tuner\n");
        printf("3/ Stats\n");
        printf("4/ Quit\n> ");
        item = getSelection(1, 4);

        switch(item){
            
            case 1: /* OPEN_RECORD */
            
                printf("Which recording channel do you wish to open [0-7]? ");
                channel = getSelection(0, 7);
                
                if(currentRecs[channel].record)
                {
                    printf("Must first close recording on channel %d\n", channel);
                    continue;
                }

                printf("Select frequency in Hz [54 - 1000000000] ");
                freq = getSelection(54, 1000000000);                

                printf("1/ QAM64\n");
                printf("2/ QAM256\n");
                printf("Select Modulation >");
                mod = getSelection(1, 2);

                count = ++(streamCnt[channel]);

                /* modify filename */
                recName[2 + stemSize] = idxName[2 + stemSize] = '0' + channel;
                recName[12 + stemSize] = idxName[12 + stemSize] = (count % 10) + '0';
                count /= 10;
                recName[11 + stemSize] = idxName[11 + stemSize] = (count % 10) + '0';
                count /= 10;                
                recName[10 + stemSize] = idxName[10 + stemSize] = (count % 10) + '0';
                count /= 10;
                recName[9 + stemSize] = idxName[9 + stemSize] = (count % 10) + '0';
                count /= 10;

                printf("Preparing to record file %s\n\n", recName);
               
                currentRecs[channel] = openRecording(recName, idxName, freq,
                                                     (mod == 1) ? NEXUS_FrontendQamMode_e64 : NEXUS_FrontendQamMode_e256, channel);
                break;
                
            case 2: /* CLOSE RECORD */
            
                printf("which channel do you wish to close [0-7]? >");
                channel = getSelection(0, 7);
                if(currentRecs[channel].record)
                {
                    closeRecording(&(currentRecs[channel]));
                    currentRecs[channel].record = NULL;
                }
                else
                    printf("No recording to close on channel %d\n", channel);
                
                break;
            case 3:
                for(i=0; i < 8; i++)
                {
                    if(currentRecs[i].record)
                    {
                        char systemCommand[107];
                        sprintf(systemCommand, "du -h %s\n", recName);
                        printf("=========Tuner %d=========\n", i);
                        fe_status((void *)platformConfig.frontend[i]);
                        system(systemCommand);
                        printf("=======================\n");
                    }
                }
                break;
            case 4:
            default:
                for(i=0; i < 8; i++)
                {
                    if(currentRecs[i].record)
                    {
                        closeRecording(&(currentRecs[i]));
                        currentRecs[i].record = NULL;
                    }
        }
                exit = true;
                break;
    }
    }
    
    NEXUS_Platform_Uninit();
    return 0;
}

