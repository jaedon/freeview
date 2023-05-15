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
 * $brcm_Workfile: multiple_index_only_records.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 3/20/12 11:10a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/multiple_index_only_records.c $
 * 
 * Hydra_Software_Devel/7   3/20/12 11:10a mward
 * SW7435-56:  Fix pumpdata( , ,0).
 * 
 * Hydra_Software_Devel/6   10/18/11 2:20p gmullen
 * SW7425-1383: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1383/1   10/14/11 4:13p gmullen
 * SW7425-1383: Updated allPass usages to use new API
 * 
 * Hydra_Software_Devel/5   8/10/10 3:37p randyjew
 * SW7208-105: Fix compile warning for implicit declaration for time
 * 
 * Hydra_Software_Devel/4   6/1/09 12:51p erickson
 * PR42679: limit runtime of test
 *
 * Hydra_Software_Devel/3   12/18/08 2:47a erickson
 * PR50347: update app
 *
 * Hydra_Software_Devel/2   12/17/08 12:02a erickson
 * PR50347: update
 *
 * Hydra_Software_Devel/1   12/12/08 5:10p erickson
 * PR50347: added test app
 *
 * Hydra_Software_Devel/1   12/1/08 12:48p erickson
 * PR49676: added allpass_record_from_playback.c test
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_playpump.h"
#include "nexus_record.h"
#include "nexus_file.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#define NUM_RECORDS 3
unsigned short pid[NUM_RECORDS] = {0x11, 0x21, 0x41};
unsigned current_pos[NUM_RECORDS];
bool done[NUM_RECORDS];

int pumpdata(FILE *file, NEXUS_PlaypumpHandle playpump, unsigned count)
{
	if (count) {
		while (--count) {
			void *buffer;
			size_t buffer_size;
			int n;
			NEXUS_Error rc;
	
			if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
				break;
			if (buffer_size == 0) {
				BKNI_Sleep(1); /* simplistic flow control */
				continue;
			}
	
	#define MAX_READ (188*1024)
			if (buffer_size > MAX_READ)
				buffer_size = MAX_READ;
	
			/* only feed whole packets so we can remove the playback pid channel from record any time */
			buffer_size -= buffer_size%188;
	
			n = fread(buffer, 1, buffer_size, file);
			if (n <= 0) {
				return 1; /* done */
			}
	
			rc = NEXUS_Playpump_ReadComplete(playpump, 0, n);
			BDBG_ASSERT(!rc);
		}
	}
    return 0; /* not done */
}

int main(int argc, char **argv) {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PidChannelHandle allPassPidChannel;
    FILE *file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaypumpOpenPidChannelSettings pidCfg;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_FileRecordHandle recordFile[NUM_RECORDS];
    NEXUS_RecpumpHandle dummyRecpump, recpump[NUM_RECORDS];
    NEXUS_RecordPidChannelSettings recordPidCfg;
    NEXUS_RecordSettings recordCfg;
    NEXUS_RecordHandle record[NUM_RECORDS];
    NEXUS_Error rc;
    const char *fname = "videos/mummy_MI_5element_q64.mpg";
    NEXUS_RecpumpOpenSettings recpumpOpenSettings;
    unsigned i;
    unsigned totaldone = 0;

    srand((unsigned)time(NULL));

    if (argc > 1) fname = argv[1];

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    /* Must use playpump, not playback. The data feed must be stopped on a transport packet boundary in order
    to stop and restart playback with no change. */
    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);

    file = fopen(fname, "r+");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }
    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.transportType = NEXUS_TransportType_eTs;
    /* in order for the index to be valid, we must index every packet in the stream. even if you think you know all the pids,
    there could be an error in the stream resulting in a bogus packet. the safest approach is to do all pass playback and record */
    playpumpSettings.allPass = true;
    playpumpSettings.acceptNullPackets = true;
    playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eNone;
    /* don't set maxDataRate. we want to go all out 108 Mbps */
    rc = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidCfg);
    NEXUS_Playpump_GetAllPassPidChannelIndex(playpump, (unsigned *) &pidCfg.pidSettings.pidChannelIndex );
    allPassPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0 /* don't care */, &pidCfg);

    /* the dummyRecpump is like a group master that allocates a CDB which will be shared among all the index-only recpumps.
    the CDB is required because the RAVE HW must write out to memory without overflow.
    however, because we aren't capturing the CDB data, we can have multiple sessions overwrite each other in the same memory buffer.
    this reduces overall system memory allocation. */
    NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
    dummyRecpump = NEXUS_Recpump_Open(0, &recpumpOpenSettings);

    /* for purposes of this test, start all the records at once */
    for (i=0;i<NUM_RECORDS;i++) {
        char fname[64];

        /* open index-only recpump which uses a shared CDB buffer */
        NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
        recpumpOpenSettings.dummyRecpump = dummyRecpump;
        recpump[i] = NEXUS_Recpump_Open(i+1, &recpumpOpenSettings);

        record[i] = NEXUS_Record_Create();
        NEXUS_Record_GetSettings(record[i], &recordCfg);
        recordCfg.recpump = recpump[i];
        rc = NEXUS_Record_SetSettings(record[i], &recordCfg);
        BDBG_ASSERT(!rc);

        /* only open an index file. all stream data will be discarded. */
        sprintf(fname, "videos/stream%d.nav", i);
        recordFile[i] = NEXUS_FileRecord_OpenPosix(NULL, fname);

        /* we must set the index pid channel before Start so that Record knows how to index. */
        NEXUS_Record_GetDefaultPidChannelSettings(&recordPidCfg);
        recordPidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        recordPidCfg.recpumpSettings.pidTypeSettings.video.index = true;
        recordPidCfg.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
        recordPidCfg.recpumpSettings.pidTypeSettings.video.pid = pid[i]; /* indexing by pid, not pid channel, is required for allPass */
        rc = NEXUS_Record_AddPidChannel(record[i], allPassPidChannel, &recordPidCfg);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Record_Start(record[i], recordFile[i]);
        BDBG_ASSERT(!rc);

        /* now that we've started, we can remove the pid channel until it's needed */
        NEXUS_Record_RemoveAllPidChannels(record[i]);
    }

    /* randomly iterate through the record sessions until each has indexed the entire file.
    each playpump feed is done in random sized chunks.
    the key point is that the Recpump & Record must remain open until the entire file is fed through, but the feeding of that file
    can be interrupted.
    if the Recpump & Record must be stopped before the entire index is complete, the index must be discarded.
    */
    while (totaldone < NUM_RECORDS) {
        unsigned i = rand() % NUM_RECORDS; /* pick a file at random */
        unsigned packets = rand() % 3000; /* feed in a random number of chunks to be indexed */

        if (done[i]) continue;

        /* seek to wherever we left off */
        fseek(file, current_pos[i], SEEK_SET);

        rc = NEXUS_Record_AddPidChannel(record[i], allPassPidChannel, NULL);
        BDBG_ASSERT(!rc);

        printf("feeding %d chunks to record %d...\n", packets, i);
        rc = NEXUS_Playpump_Start(playpump);
        BDBG_ASSERT(!rc);

        if (pumpdata(file, playpump, packets)) {
            done[i] = true;
            totaldone++;
        }

        /* poll to learn when playpump fifo is empty. we can't lose any data or the index will be invalid. */
        while (1) {
            NEXUS_PlaypumpStatus pstatus;
            NEXUS_Playpump_GetStatus(playpump, &pstatus);
            if (pstatus.fifoDepth == 0) break;
            BKNI_Sleep(1);
        }

        NEXUS_Record_RemoveAllPidChannels(record[i]);
        NEXUS_Playpump_Stop(playpump);

        /* remember where we left off so we can continue */
        current_pos[i] = ftell(file);

        if (current_pos[i] > 500*1000000) {
            done[i] = true;
            totaldone++;
        }
    }

    /* shutdown */
    for (i=0;i<NUM_RECORDS;i++) {
        NEXUS_Record_Stop(record[i]);
        NEXUS_Record_RemoveAllPidChannels(record[i]);
        NEXUS_FileRecord_Close(recordFile[i]);
        NEXUS_Record_Destroy(record[i]);
        NEXUS_Recpump_Close(recpump[i]);
    }
    NEXUS_Recpump_Close(dummyRecpump);
    fclose(file);
    NEXUS_Playpump_ClosePidChannel(playpump, allPassPidChannel);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Platform_Uninit();

    return 0;
}
