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
 * $brcm_Workfile: active_standby.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 10/12/12 11:47a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/standby/active_standby.c $
 * 
 * 9   10/12/12 11:47a mward
 * SW7435-423:  Declare cpu_divisor unused.
 * 
 * 8   10/1/12 1:25p gmohile
 * SW7425-4012 : kill dhcp task before exiting app
 * 
 * 7   2/16/12 5:34p randyjew
 * SW7344-104:7346 No memc1.
 * 
 * 6   2/9/12 11:23a gmohile
 * SW7425-104: Add tranasport wakeup, Fix eth power down
 * 
 * 5   1/10/12 6:07p randyjew
 * SW7344-104:Update for 7344, no sata or memc1.
 * 
 * 4   7/25/11 2:35p nickh
 * SW7420-2006: Include renamed pmlib-263x files
 * 
 * 3   7/13/11 11:01a gmohile
 * SW7408-210 : update active standby app
 * 
 * 2   3/14/11 2:21p gmohile
 * SW7408-210 : Additional command line options
 * 
 * 1   3/10/11 5:40p gmohile
 * SW7408-210 : Add active standby app
 * 
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nexus_platform.h"
#include "nexus_platform_standby.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include "pmlib-263x.h"

#if NEXUS_HAS_RECORD
NEXUS_FileRecordHandle file;
NEXUS_RecpumpHandle recpump;
NEXUS_RecordHandle record;
NEXUS_PidChannelHandle pidChannel[2];
#endif

void *brcm_pm_ctx;

void start_record(void)
{
#if NEXUS_HAS_RECORD
    NEXUS_InputBand inputBand;   
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_RecordPidChannelSettings pidSettings;
    NEXUS_RecordSettings recordSettings;
    
    const char *fname = "videos/stream.mpg";
    const char *index = "videos/stream.nav";

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    recpump = NEXUS_Recpump_Open(0, NULL);

    record = NEXUS_Record_Create();

    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    NEXUS_Record_SetSettings(record, &recordSettings);

    file = NEXUS_FileRecord_OpenPosix(fname,  index);

    /* configure the video pid for indexing */
    NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    pidSettings.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[0], &pidSettings);

    /* the audio pid requires no special configuration */
    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, 0x34, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[1], NULL);

    NEXUS_Record_Start(record, file);
#endif
}

void stop_record(void)
{
#if NEXUS_HAS_RECORD
    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_PidChannel_Close(pidChannel[0]);
    NEXUS_PidChannel_Close(pidChannel[1]);
    NEXUS_FileRecord_Close(file);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
#endif
}


int set_pmlib_state(bool usb_status,    /* 1=on, 0=off */
		    bool enet_status,	/* 1=on, 0=off */
		    bool moca_status,	/* 1=on, 0=off */
		    bool sata_status,	/* 1=on, 0=off */
		    bool memc1_status,	/* 1=on, 0=off */
		    bool tp1_status,	/* 1=on, 0=off */
		    int cpu_divisor,	/* 1, 2, 4, or 8 */
		    int ddr_timeout	/* 0=no PM, >0 = timeout for self-refresh */
		    )
{
    int rc;
    struct brcm_pm_state pmlib_state;   
	BSTD_UNUSED(cpu_divisor);

    rc = brcm_pm_get_status(brcm_pm_ctx, &pmlib_state);
    if(rc) {printf("Cant get PM status\n"); /* return -1; */}
            
    pmlib_state.usb_status = usb_status;
#if BCHP_CHIP==7344
    pmlib_state.sata_status = BRCM_PM_UNDEF;
    pmlib_state.memc1_status = BRCM_PM_UNDEF;
#elif BCHP_CHIP==7346
	pmlib_state.sata_status = sata_status;
    pmlib_state.memc1_status = BRCM_PM_UNDEF;
#else
    pmlib_state.sata_status = sata_status;
    pmlib_state.memc1_status = memc1_status;
#endif
    pmlib_state.tp1_status = tp1_status;
    pmlib_state.ddr_timeout = ddr_timeout;

    rc = brcm_pm_set_status(brcm_pm_ctx, &pmlib_state);
    if(rc) {printf("Cant set PM status\n"); /* return -1; */}

    
    if(enet_status) {
	system("ifup eth0");
    } else {
	system("ifdown eth0");
    }

    if(moca_status) {
	system("mocactl start");
	system("ifup eth1");		
    } else {
	system("ifdown eth1");
	system("mocactl stop");
    }

    return 0;
}

int main(int argc, char **argv) 
{
    NEXUS_PlatformStandbySettings nexusStandbySettings;
    bool record=false;  
    unsigned cpu_divisor=1;
    unsigned ddr_timeout=0;
    bool usb=true;
    bool enet=true;
    bool moca=true;
    bool sata=true;
    bool memc1=true;
    bool tp1=true;
    int loop=1;
    int duration=5;
    
    int curarg = 0;
    int rc;

    while (++curarg < argc) {
        if (!strcmp(argv[curarg], "--help")) {
            printf(
            "Usage: nexus active_standby [OPTIONS]\n"
            "\n"
            "-r            Start a record in the background.\n"
            "-cpu SCALE    Set CPU clock divisor. 1, 2, 4 or 8\n"
	    "-ddr RATE     Set DDR self refresh rate.\n"
	    "-usb          Power down usb\n"
	    "-enet         Power down enet\n"
	    "-moca         Power down moca\n"
	    "-sata         Power down sata\n"
	    "-memc1        Power down memc1\n"
	    "-tp1          Power down tp1\n"
	    "-loop N       Loop N times. Default no loop. -1 for infinite loop\n"
	    "-duration N   Standby for N seconds. Default 5 seconds. -1 for manual control\n"
            );
            return 0;
        }
	else if (!strcmp(argv[curarg], "-r")) {
            record = true;
        }
        else if (!strcmp(argv[curarg], "-cpu") && curarg+1 < argc) {
            cpu_divisor = atoi(argv[++curarg]);
        }
	else if (!strcmp(argv[curarg], "-ddr") && curarg+1 < argc) {
            ddr_timeout = atoi(argv[++curarg]);
        }
        else if (!strcmp(argv[curarg], "-usb")) {
            usb = false;
        }
	else if (!strcmp(argv[curarg], "-enet")) {
            enet = false;
        }
	else if (!strcmp(argv[curarg], "-moca")) {
            moca = false;
        }
	else if (!strcmp(argv[curarg], "-sata")) {
            sata = false;
        }
	else if (!strcmp(argv[curarg], "-memc1")) {
            memc1 = false;
        }
	else if (!strcmp(argv[curarg], "-tp1")) {
            tp1 = false;
        }
	else if (!strcmp(argv[curarg], "-loop")) {
            loop = atoi(argv[++curarg]);
        }
	else if (!strcmp(argv[curarg], "-duration")) {
            duration = atoi(argv[++curarg]);
        }
    }

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);  

    brcm_pm_ctx = brcm_pm_init();
    
    rc = set_pmlib_state(true, true, true, true, true, true, 1, 0); 
    if(rc) return -1;
    
    if(record) {
	start_record();
    }

    while(1) {
	
	if(loop>0) {
	    loop--;
	} else if(loop==0) {
	    break;
	}

	if(duration==-1) {
	    printf("press ENTER for Active Standby\n");
	    getchar();
	} else {
	    BKNI_Sleep(duration*1000);
	}
	printf("Entering Active standby\n");

	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eActive;    
	rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
	BDBG_ASSERT(!rc);
    
	rc = set_pmlib_state(usb, enet, moca, sata, memc1, tp1, cpu_divisor, ddr_timeout); 
	if(rc) return -1;        

	if(duration==-1) {
	    printf("press ENTER to Resume from Standby\n");
	    getchar();
	}else {
	    BKNI_Sleep(duration*1000);
	}
	printf("Resuming from Standby\n");
    
	rc = set_pmlib_state(true, true, true, true, true, true, 1, 0); 
	if(rc) return -1;    
    
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eOn;    
	rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
	BDBG_ASSERT(!rc);
    }
        
    if(record) {
	stop_record();
    }

    NEXUS_Platform_Uninit();

    system("if [ -e /tmp/udhcpc.eth0.pid ]; then kill `cat /tmp/udhcpc.eth0.pid`; fi");
    system("if [ -e /tmp/udhcpc.eth1.pid ]; then kill `cat /tmp/udhcpc.eth1.pid`; fi");

    return 0;
}
