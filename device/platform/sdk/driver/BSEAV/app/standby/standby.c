/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: standby.c $
 * $brcm_Revision: 51 $
 * $brcm_Date: 10/12/12 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/standby/standby.c $
 * 
 * 51   10/12/12 11:50a mward
 * SW7435-423: SW7425-3782:  NEXUS_Frontend_GetConnector().
 * 
 * 50   10/11/12 5:47p gmohile
 * SW7435-327 : Fix gpio wakeup status
 * 
 * 49   10/1/12 1:25p gmohile
 * SW7425-4012 : kill dhcp task before exiting app
 * 
 * 48   9/27/12 3:12p gmohile
 * SW7435-327 : Add gpio wakeup
 * 
 * 47   9/14/12 3:18p gmohile
 * SW7435-327 : Power down moca is moca wol is not used
 * 
 * 46   9/6/12 2:57p gmohile
 * SW7435-327 : Fix SD graphics
 * 
 * 45   8/27/12 1:02p gmohile
 * SW7435-327 : Add graphics test
 * 
 * 44   8/10/12 11:04a gmohile
 * SW7435-292 : Add 7435 A0 support
 * 
 * 43   7/12/12 2:43p gmohile
 * SW7429-155 : Add 7429 support
 * 
 * 42   6/6/12 1:35p gmohile
 * SW7344-308 : Enable S3 warm boot
 * 
 * 41   5/24/12 3:53p gmohile
 * SW7231-794 : Fix moca wol support
 * 
 * 40   5/1/12 2:40p gmohile
 * SW7231-794 : Add WOL support
 * 
 * 39   4/26/12 3:28p gmohile
 * SW7344-308 : Add satellite frontend test
 * 
 * 38   4/10/12 2:11p gmohile
 * SW7425-2289 : Fix cec data received callback
 * 
 * 37   4/10/12 2:03p gmohile
 * SW7425-2289 : Fix cec data received callback
 * 
 * 36   3/6/12 2:24p xhuang
 * SW7552-9: enable S3 cold boot for 7552
 * 
 * 35   3/2/12 3:36p bselva
 * SW7358-169: Added S3 sandby support for 7358 platform.
 * 
 * 34   3/1/12 3:05p gmohile
 * SW7425-2289 : Fix cec wake up support
 * 
 * 33   3/1/12 10:51a gmohile
 * SW7425-2289 : Add cec wake up support
 * 
 * 32   2/24/12 11:03a gmohile
 * SW7231-426 : enable S3 warm boot for 7231
 * 
 * 31   2/9/12 11:23a gmohile
 * SW7425-104: Add tranasport wakeup, Fix eth power down
 * 
 * 30   1/30/12 11:21a gmohile
 * SW7425-1708 : Update standby app for S3 warm boot
 * 
 * 29   1/16/12 6:06p randyjew
 * SW7344-104: Add S3 to 7346/7344
 * 
 * 28   11/9/11 5:43p xhuang
 * SW7552-9: Add for 7552
 * 
 * 27   11/3/11 1:26p bselva
 * SW7358-169: Added the changes required to support 7358 platform.
 * 
 * 26   10/27/11 4:12p gmohile
 * SW7231-426 : Fix uhf wakeup support
 * 
 * 25   10/19/11 1:10p gmohile
 * SW7420-2103 : Add uhf wakeup support
 * 
 * 24   10/12/11 7:37p randyjew
 * SW7231-345: Disable MEMC1 for 7344/7346
 * 
 * 23   10/3/11 12:51p nickh
 * SW7420-2006: Enable MoCA and MEMC1 for 7420
 * 
 * 22   10/3/11 10:15a katrep
 * SW7231-345:enable memc and moca for 7425
 * 
 * 21   9/20/11 5:49p katrep
 * SW7231-345:7231 should not try to turn on MOCA or memc1
 * 
 *****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "standby.h"
#include "pmlib-263x.h"
#include <pthread.h>

BDBG_MODULE(standby);

B_StandbyNexusHandles g_StandbyNexusHandles;
B_DeviceState g_DeviceState;

pthread_t cli_thread, gfx_thread;

static struct {
      void *brcm_pm_ctx;
      pmlib_state_t g_state;
   
#define MAX_MOUNTS 16
      struct {
        char device[256];
        char mountpoint[256];
      } mounts[MAX_MOUNTS];
      unsigned total_mounts;
} g_power_state;


void irCallback(void *pParam, int iParam)
{
    size_t numEvents = 1;
    NEXUS_Error rc = 0;
    bool overflow;
    NEXUS_IrInputHandle irHandle = *(NEXUS_IrInputHandle *)pParam;
    BSTD_UNUSED(iParam);
    while (numEvents && !rc) {
        NEXUS_IrInputEvent irEvent;
	NEXUS_PlatformStandbySettings nexusStandbySettings;
        rc = NEXUS_IrInput_GetEvents(irHandle, &irEvent, 1, &numEvents, &overflow); 
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	if (numEvents && 
	    ((irEvent.code == POWER_STANDBY_IR_CODE) || (irEvent.code == EXIT_IR_CODE) || (irEvent.code == S1_IR_CODE) || (irEvent.code == S2_IR_CODE) || (irEvent.code == S3_IR_CODE)) && 
	    !irEvent.repeat && 
	    (nexusStandbySettings.mode != NEXUS_PlatformStandbyMode_ePassive) && (nexusStandbySettings.mode != NEXUS_PlatformStandbyMode_eDeepSleep)) {
            printf("Received Power Command from IR\n");
	    if(irEvent.code == EXIT_IR_CODE) {
		g_DeviceState.exit_app = true;
	    } else if (irEvent.code == S1_IR_CODE) {
		g_DeviceState.s1_only = true;
	    } else if (irEvent.code == S2_IR_CODE) {
		g_DeviceState.s2_only = true;
	    } else if (irEvent.code == S3_IR_CODE) {
		g_DeviceState.s3_only = true;
	    }
	    BKNI_SetEvent(g_StandbyNexusHandles.event);
	}        
    }
}

#if NEXUS_HAS_UHF_INPUT
void uhfCallback(void *pParam, int iParam)
{
    size_t numEvents = 1;
    NEXUS_Error rc = 0;
    bool overflow;
    NEXUS_UhfInputHandle uhfHandle = *(NEXUS_UhfInputHandle *)pParam;
    BSTD_UNUSED(iParam);
    while (numEvents && !rc) {
        NEXUS_UhfInputEvent uhfEvent;
	NEXUS_PlatformStandbySettings nexusStandbySettings;
        rc = NEXUS_UhfInput_GetEvents(uhfHandle, &uhfEvent, 1, &numEvents, &overflow);
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	if (numEvents && (uhfEvent.code == POWER_STANDBY_UHF_CODE) && !uhfEvent.repeat && 
	    (nexusStandbySettings.mode != NEXUS_PlatformStandbyMode_ePassive)) {
            printf("Received Power Command from UHF\n");
	    BKNI_SetEvent(g_StandbyNexusHandles.event);
	}
        /* if (numEvents) */
/*             printf("uhfCallback: rc: %d, code: %08x, repeat: %s\n", rc, uhfEvent.code, uhfEvent.repeat ? "true" : "false"); */
    }
}
#endif

#if NEXUS_HAS_KEYPAD
void keypadCallback(void *pParam, int iParam)
{
    size_t numEvents = 1;
    NEXUS_Error rc = 0;
    bool overflow;
    NEXUS_KeypadHandle  keypadHandle = *(NEXUS_KeypadHandle *)pParam;
    BSTD_UNUSED(iParam);
    while (numEvents && !rc) {
	NEXUS_KeypadEvent keypadEvent;
	NEXUS_PlatformStandbySettings nexusStandbySettings;
        rc = NEXUS_Keypad_GetEvents(keypadHandle, &keypadEvent,1, &numEvents, &overflow); 
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	/* if (numEvents) { */
/*             printf("Received Power Command from Keypad %#x\n", keypadEvent.code);	     */
/* 	}   */      
    }
}
#endif

void transportWakeupCallback(void *pParam, int iParam)
{
    BSTD_UNUSED(pParam);
    BSTD_UNUSED(iParam);
    printf("Xpt Wakeup Event \n");
    BKNI_SetEvent(g_StandbyNexusHandles.event);
}

#if NEXUS_HAS_CEC
void cecDeviceReady_callback(void *context, int param)
{
    NEXUS_CecStatus status;

    BSTD_UNUSED(param);
    BSTD_UNUSED(context);
    /* BKNI_SetEvent((BKNI_EventHandle)context); */
    NEXUS_Cec_GetStatus(g_StandbyNexusHandles.hCec, &status);

    BDBG_WRN(("BCM%d Logical Address <%d> Acquired", 
        BCHP_CHIP,        
        status.logicalAddress)) ;

    BDBG_WRN(("BCM%d Physical Address: %X.%X.%X.%X",
        BCHP_CHIP,
        (status.physicalAddress[0] & 0xF0) >> 4, 
        (status.physicalAddress[0] & 0x0F),
        (status.physicalAddress[1] & 0xF0) >> 4, 
        (status.physicalAddress[1] & 0x0F))) ;

    if ((status.physicalAddress[0] = 0xFF)
    && (status.physicalAddress[1] = 0xFF))
    {
	    BDBG_WRN(("CEC Device Ready!")) ;
	       g_DeviceState.cecDeviceReady = true ;
    }
}

void cecMsgReceived_callback(void *context, int param)
{
    NEXUS_CecStatus status;
    NEXUS_CecReceivedMessage receivedMessage;
    NEXUS_Error rc ;
    uint8_t i, j ;
    char msgBuffer[3*(NEXUS_CEC_MESSAGE_DATA_SIZE +1)];

    BSTD_UNUSED(param);
    BSTD_UNUSED(context);
    /* BKNI_SetEvent((BKNI_EventHandle)context); */
    NEXUS_Cec_GetStatus(g_StandbyNexusHandles.hCec, &status);

    BDBG_WRN(("Message Received: %s", status.messageReceived ? "Yes" : "No")) ;
		
    rc = NEXUS_Cec_ReceiveMessage(g_StandbyNexusHandles.hCec, &receivedMessage);
    BDBG_ASSERT(!rc);
    
    /* For debugging purposes */
    for (i = 0, j = 0; i <= receivedMessage.data.length ; i++)
    {
        j += BKNI_Snprintf(msgBuffer + j, sizeof(msgBuffer)-j, "%02X ", 
        receivedMessage.data.buffer[i]) ;
    }                
    BDBG_WRN(("CEC Message Length %d Received: %s", 
        receivedMessage.data.length, msgBuffer)) ;

    BDBG_WRN(("Msg Recd Status from Phys/Logical Addrs: %X.%X.%X.%X / %d", 
        (status.physicalAddress[0] & 0xF0) >> 4, (status.physicalAddress[0] & 0x0F),
        (status.physicalAddress[1] & 0xF0) >> 4, (status.physicalAddress[1] & 0x0F),
        status.logicalAddress)) ;

}

void cecMsgTransmitted_callback(void *context, int param)
{
    NEXUS_CecStatus status;
    
    BSTD_UNUSED(param);
    BSTD_UNUSED(context);
    /* BKNI_SetEvent((BKNI_EventHandle)context); */
    NEXUS_Cec_GetStatus(g_StandbyNexusHandles.hCec, &status);

    BDBG_WRN(("Msg Xmit Status for Phys/Logical Addrs: %X.%X.%X.%X / %d", 
        (status.physicalAddress[0] & 0xF0) >> 4, (status.physicalAddress[0] & 0x0F),
        (status.physicalAddress[1] & 0xF0) >> 4, (status.physicalAddress[1] & 0x0F),
        status.logicalAddress)) ;

    BDBG_WRN(("Xmit Msg Acknowledged: %s", 
		status.transmitMessageAcknowledged ? "Yes" : "No")) ;
    BDBG_WRN(("Xmit Msg Pending: %s", 
		status.messageTransmitPending ? "Yes" : "No")) ;
}
#endif

#if NEXUS_HAS_FRONTEND
static void qam_lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus qamStatus;

    BSTD_UNUSED(param);
    
    fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

    NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
    fprintf(stderr, "QAM Lock callback, frontend 0x%08x - lock status %d, %d\n", (unsigned)frontend,
            qamStatus.fecLock, qamStatus.receiverLock);
}

void tune_qam(void)
{
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_FrontendUserParameters userParams;    
    NEXUS_ParserBandSettings parserBandSettings;
    unsigned i;

    NEXUS_Platform_GetConfiguration(&g_StandbyNexusHandles.platformConfig);

    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        NEXUS_FrontendCapabilities capabilities;
        g_StandbyNexusHandles.frontend = g_StandbyNexusHandles.platformConfig.frontend[i];
        if (g_StandbyNexusHandles.frontend) {
            NEXUS_Frontend_GetCapabilities(g_StandbyNexusHandles.frontend, &capabilities);
            /* Does this frontend support qam? */
            if ( capabilities.qam )
            {
                break;
            }
        }
    }

    if (NULL == g_StandbyNexusHandles.frontend )
    {
        fprintf(stderr, "Unable to find QAM-capable frontend\n");       
	return;
    }
    
    NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
    qamSettings.frequency = 765000000;
    qamSettings.mode = NEXUS_FrontendQamMode_e64;
    qamSettings.symbolRate = 5056900;
    qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
    qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e6Mhz;
    qamSettings.lockCallback.callback = qam_lock_callback;
    qamSettings.lockCallback.context = g_StandbyNexusHandles.frontend;    

    NEXUS_Frontend_GetUserParameters(g_StandbyNexusHandles.frontend, &userParams);
    
    NEXUS_ParserBand_GetSettings(g_StandbyNexusHandles.parserBand, &parserBandSettings);
    if (userParams.isMtsif) {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(g_StandbyNexusHandles.frontend); /* NEXUS_Frontend_TuneXyz() will connect this frontend to this parser band */
    }
    else {
	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    }
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(g_StandbyNexusHandles.parserBand, &parserBandSettings);

    NEXUS_Frontend_TuneQam(g_StandbyNexusHandles.frontend, &qamSettings);
}

void untune_qam(void)
{
    NEXUS_Frontend_Untune(g_StandbyNexusHandles.frontend);
}

static void sat_lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendSatelliteStatus status;    

    BSTD_UNUSED(param);

    fprintf(stderr, "Frontend(%p) - lock callback\n", (void*)frontend);

    NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
    fprintf(stderr, "  demodLocked = %d\n", status.demodLocked);    
}

void tune_sat(void)
{
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendDiseqcSettings diseqcSettings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_ParserBandSettings parserBandSettings;
    unsigned i;

    NEXUS_Platform_GetConfiguration(&g_StandbyNexusHandles.platformConfig);

    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        NEXUS_FrontendCapabilities capabilities;
        g_StandbyNexusHandles.frontend = g_StandbyNexusHandles.platformConfig.frontend[i];
        if (g_StandbyNexusHandles.frontend) {
            NEXUS_Frontend_GetCapabilities(g_StandbyNexusHandles.frontend, &capabilities);
            /* Does this frontend support satellite? */
            if ( capabilities.satellite )
            {
                break;
            }
        }
    }

    if (NULL == g_StandbyNexusHandles.frontend )
    {
        fprintf(stderr, "Unable to find satellite-capable frontend\n");
        return;
    }

    NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
    satSettings.frequency = 1119000000;
    satSettings.mode = NEXUS_FrontendSatelliteMode_eDvb;
    satSettings.lockCallback.callback = sat_lock_callback;
    satSettings.lockCallback.context = g_StandbyNexusHandles.frontend;
    NEXUS_Frontend_GetUserParameters(g_StandbyNexusHandles.frontend, &userParams);

    /* Map a parser band to the demod's input band. */    
    NEXUS_ParserBand_GetSettings(g_StandbyNexusHandles.parserBand, &parserBandSettings);
    if (userParams.isMtsif) {
	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(g_StandbyNexusHandles.frontend); /* NEXUS_Frontend_TuneXyz() will connect this frontend to this parser band */
    } else {
	parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    }   
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(g_StandbyNexusHandles.parserBand, &parserBandSettings);

    NEXUS_Frontend_GetDiseqcSettings(g_StandbyNexusHandles.frontend, &diseqcSettings);
    diseqcSettings.toneEnabled = true;
    diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
    NEXUS_Frontend_SetDiseqcSettings(g_StandbyNexusHandles.frontend, &diseqcSettings);
    printf("Set DiseqcSettings\n");

    NEXUS_Frontend_TuneSatellite(g_StandbyNexusHandles.frontend, &satSettings);    
}

void untune_sat(void)
{
    NEXUS_Frontend_Untune(g_StandbyNexusHandles.frontend);
}
#endif

void *graphics_thread(void *context)
{
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_VideoWindowSettings window0Settings, window1Settings;
    NEXUS_SurfaceCreateSettings surfaceHdSettings, surfaceSdSettings;
    NEXUS_Error rc;
    unsigned i;

    BSTD_UNUSED(context);

    NEXUS_VideoWindow_GetSettings(g_StandbyNexusHandles.window0, &window0Settings);   
    NEXUS_VideoWindow_GetSettings(g_StandbyNexusHandles.window1, &window1Settings);
    NEXUS_Surface_GetCreateSettings(g_StandbyNexusHandles.offscreenHD, &surfaceHdSettings);
    NEXUS_Surface_GetCreateSettings(g_StandbyNexusHandles.offscreenSD, &surfaceSdSettings);
    
    for(i=0;;i++) {

	if(g_DeviceState.stop_graphics) {
	    rc = NEXUS_Graphics2D_Checkpoint(g_StandbyNexusHandles.gfx, NULL);
            if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                rc = BKNI_WaitForEvent(g_StandbyNexusHandles.checkpointEvent, BKNI_INFINITE);
            }
	    return NULL;
	}

	NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);   

	fillSettings.surface = g_StandbyNexusHandles.offscreenHD;
	fillSettings.rect.x = rand() % surfaceHdSettings.width;
	fillSettings.rect.y = rand() % surfaceHdSettings.height;
	fillSettings.rect.width = 20;
	fillSettings.rect.height = 20;
	fillSettings.color = (0xFF << 24) | rand();
                   
	while (1) {
	    rc = NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);
	    if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
		BKNI_WaitForEvent(g_StandbyNexusHandles.spaceAvailableEvent, BKNI_INFINITE);            
	    } else {
                break;
            } 
	}    

	fillSettings.surface = g_StandbyNexusHandles.offscreenSD;
	fillSettings.rect.x = rand() % surfaceSdSettings.width;
	fillSettings.rect.y = rand() % surfaceSdSettings.height;
	fillSettings.rect.width = 20;
	fillSettings.rect.height = 20;
	fillSettings.color = (0xFF << 24) | rand();
             
	while (1) {
	    rc = NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);
	    if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
		BKNI_WaitForEvent(g_StandbyNexusHandles.spaceAvailableEvent, BKNI_INFINITE);            
	    } else {
                break;
            } 
	}         
	
	if (i && i%1000==0) {
	    fillSettings.surface = g_StandbyNexusHandles.offscreenHD;
	    fillSettings.rect = window0Settings.position;	    
	    fillSettings.color = 0x00cacaca;
	    
	    while (1) {
		rc = NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);
		if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
		    BKNI_WaitForEvent(g_StandbyNexusHandles.spaceAvailableEvent, BKNI_INFINITE);
		} else {
		    break;
		} 
	    }     

	    fillSettings.surface = g_StandbyNexusHandles.offscreenSD;
	    fillSettings.rect = window1Settings.position;	    
	    fillSettings.color = 0x00cacaca;
	    while (1) {
		rc = NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);
		if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
		    BKNI_WaitForEvent(g_StandbyNexusHandles.spaceAvailableEvent, BKNI_INFINITE);
		} else {
		    break;
		} 
	    }  
		  	    
	    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	    blitSettings.source.surface = g_StandbyNexusHandles.offscreenHD;
	    blitSettings.output.surface = g_StandbyNexusHandles.framebufferHD;
	    blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
	    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
	    while (1) {
		rc = NEXUS_Graphics2D_Blit(g_StandbyNexusHandles.gfx, &blitSettings);	  
		if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
		    BKNI_WaitForEvent(g_StandbyNexusHandles.spaceAvailableEvent, BKNI_INFINITE);
		} else {
		    break;
		} 
	    }  

	    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	    blitSettings.source.surface = g_StandbyNexusHandles.offscreenSD;
	    blitSettings.output.surface = g_StandbyNexusHandles.framebufferSD;
	    blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
	    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
	    while (1) {
		rc = NEXUS_Graphics2D_Blit(g_StandbyNexusHandles.gfx, &blitSettings);	  
		if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
		    BKNI_WaitForEvent(g_StandbyNexusHandles.spaceAvailableEvent, BKNI_INFINITE);
		} else {
		    break;
		} 
	    }  
            
            rc = NEXUS_Graphics2D_Checkpoint(g_StandbyNexusHandles.gfx, NULL);
            if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                rc = BKNI_WaitForEvent(g_StandbyNexusHandles.checkpointEvent, BKNI_INFINITE);
            }	    	    
            BDBG_ASSERT(!rc);   
        }	
    }    
}

void playback_start(void)
{
    if(g_DeviceState.playback_started)
        return;    

    g_StandbyNexusHandles.file = NEXUS_FilePlay_OpenPosix("videos/cnnticker.mpg", NULL);
    if (!g_StandbyNexusHandles.file) {
        fprintf(stderr, "can't open file\n");
	BDBG_ASSERT(g_StandbyNexusHandles.file);
    }

    /* Start playback */
    NEXUS_Playback_Start(g_StandbyNexusHandles.playback, g_StandbyNexusHandles.file, NULL);
    
    g_DeviceState.playback_started = true;       
}

void playback_stop(void)
{
    if(!g_DeviceState.playback_started)
        return;   
    
    /*Stop playback */
    NEXUS_Playback_Stop(g_StandbyNexusHandles.playback);
    
    /* Close File. Required for umount */
    NEXUS_FilePlay_Close(g_StandbyNexusHandles.file);
    
    g_DeviceState.playback_started = false;
}

void decode_start(void)
{
    if(g_DeviceState.decode_started)
        return;  

    /* Start decoders */
    NEXUS_VideoDecoder_Start(g_StandbyNexusHandles.videoDecoder, &g_StandbyNexusHandles.videoProgram);
    NEXUS_AudioDecoder_Start(g_StandbyNexusHandles.audioDecoder, &g_StandbyNexusHandles.audioProgram);
    
    g_DeviceState.decode_started = true;
}

void decode_stop(void)
{
    if(!g_DeviceState.decode_started)
        return;  

    /* Stop decoders */
    NEXUS_VideoDecoder_Stop(g_StandbyNexusHandles.videoDecoder);
    NEXUS_AudioDecoder_Stop(g_StandbyNexusHandles.audioDecoder);
    
    g_DeviceState.decode_started = false;
}

void graphics_start(void)
{
    if(g_DeviceState.graphics_started)
        return;

    g_DeviceState.stop_graphics=false;
    pthread_create(&gfx_thread, NULL, graphics_thread, NULL);    
    
    g_DeviceState.graphics_started = true;
}

void graphics_stop(void)
{
    if(!g_DeviceState.graphics_started)
        return;

    g_DeviceState.stop_graphics=true;
    pthread_join(gfx_thread, NULL);    
    
    g_DeviceState.graphics_started = false;
}

void complete(void *data, int unused)
{    
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

void start_app(void)
{         
    NEXUS_DisplaySettings displaySettings; 
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoFormatInfo videoFormatInfo0, videoFormatInfo1;
    NEXUS_StcChannelSettings stcSettings; 
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_IrInputSettings irSettings; 
#if NEXUS_HAS_UHF_INPUT 
    NEXUS_UhfInputSettings uhfSettings;
#endif    
#if NEXUS_HAS_KEYPAD
    NEXUS_KeypadSettings keypadSettings;
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif
    NEXUS_SurfaceCreateSettings createSettings;        
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DSettings gfxSettings;

    NEXUS_Platform_GetConfiguration(&g_StandbyNexusHandles.platformConfig);    

    BKNI_CreateEvent(&g_StandbyNexusHandles.event);
    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irSettings.dataReady.callback = irCallback;
    irSettings.dataReady.context = &g_StandbyNexusHandles.irHandle;
    g_StandbyNexusHandles.irHandle = NEXUS_IrInput_Open(0, &irSettings);  

#if NEXUS_HAS_UHF_INPUT
    NEXUS_UhfInput_GetDefaultSettings(&uhfSettings);
    uhfSettings.channel = NEXUS_UhfInputMode_eChannel1;
    uhfSettings.dataReady.callback = uhfCallback;
    uhfSettings.dataReady.context = &g_StandbyNexusHandles.uhfHandle;
    g_StandbyNexusHandles.uhfHandle = NEXUS_UhfInput_Open(0, &uhfSettings);
#endif

#if NEXUS_HAS_KEYPAD
    NEXUS_Keypad_GetDefaultSettings(&keypadSettings);
    keypadSettings.dataReady.callback = keypadCallback;
    keypadSettings.dataReady.context = &g_StandbyNexusHandles.keypadHandle;
    g_StandbyNexusHandles.keypadHandle = NEXUS_Keypad_Open(0, &keypadSettings);
#endif    

    BKNI_CreateEvent(&g_StandbyNexusHandles.checkpointEvent);
    BKNI_CreateEvent(&g_StandbyNexusHandles.spaceAvailableEvent);    

    NEXUS_Display_GetDefaultSettings(&displaySettings);  
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    g_StandbyNexusHandles.display0 = NEXUS_Display_Open(0, &displaySettings);
#if SD_DISPLAY
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    g_StandbyNexusHandles.display1 = NEXUS_Display_Open(1, &displaySettings);
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
    if (g_StandbyNexusHandles.platformConfig.outputs.hdmi[0]) {
	NEXUS_Display_AddOutput(g_StandbyNexusHandles.display0, NEXUS_HdmiOutput_GetVideoConnector(g_StandbyNexusHandles.platformConfig.outputs.hdmi[0]));
    }
    rc = NEXUS_HdmiOutput_GetStatus(g_StandbyNexusHandles.platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(g_StandbyNexusHandles.display0, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(g_StandbyNexusHandles.display0, &displaySettings);
	}
    }
#endif  
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if(g_StandbyNexusHandles.platformConfig.outputs.component[0]) {
	NEXUS_Display_AddOutput(g_StandbyNexusHandles.display0, NEXUS_ComponentOutput_GetConnector(g_StandbyNexusHandles.platformConfig.outputs.component[0]));
    }
#endif    

#if SD_DISPLAY
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if(g_StandbyNexusHandles.platformConfig.outputs.composite[0]) {
	NEXUS_Display_AddOutput(g_StandbyNexusHandles.display1, NEXUS_CompositeOutput_GetConnector(g_StandbyNexusHandles.platformConfig.outputs.composite[0]));
    }
#endif
#if NEXUS_NUM_RFM_OUTPUTS
    if (g_StandbyNexusHandles.platformConfig.outputs.rfm[0]) {
        NEXUS_Display_AddOutput(g_StandbyNexusHandles.display1, NEXUS_Rfm_GetVideoConnector(g_StandbyNexusHandles.platformConfig.outputs.rfm[0]));
    }
#endif      
#endif
    
    
    g_StandbyNexusHandles.gfx = NEXUS_Graphics2D_Open(0, NULL);
    NEXUS_Graphics2D_GetSettings(g_StandbyNexusHandles.gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = g_StandbyNexusHandles.checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = g_StandbyNexusHandles.spaceAvailableEvent;
    NEXUS_Graphics2D_SetSettings(g_StandbyNexusHandles.gfx, &gfxSettings);

    NEXUS_Display_GetSettings(g_StandbyNexusHandles.display0, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo0);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = videoFormatInfo0.width;
    createSettings.height = videoFormatInfo0.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    g_StandbyNexusHandles.framebufferHD = NEXUS_Surface_Create(&createSettings);
    g_StandbyNexusHandles.offscreenHD = NEXUS_Surface_Create(&createSettings);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = g_StandbyNexusHandles.framebufferHD;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0xFF0000FF;
    NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);
    fillSettings.surface = g_StandbyNexusHandles.offscreenHD;    
    NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);

#if SD_DISPLAY
    NEXUS_Display_GetSettings(g_StandbyNexusHandles.display1, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo1);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width =  videoFormatInfo1.width;
    createSettings.height = videoFormatInfo1.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    g_StandbyNexusHandles.framebufferSD = NEXUS_Surface_Create(&createSettings);
    g_StandbyNexusHandles.offscreenSD = NEXUS_Surface_Create(&createSettings);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = g_StandbyNexusHandles.framebufferSD;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0xFF0000FF;
    NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);
    fillSettings.surface = g_StandbyNexusHandles.offscreenSD;    
    NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);
#endif

    NEXUS_Display_GetGraphicsSettings(g_StandbyNexusHandles.display0, &graphicsSettings);
    graphicsSettings.enabled = true;    
    NEXUS_Display_SetGraphicsSettings(g_StandbyNexusHandles.display0, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(g_StandbyNexusHandles.display0, g_StandbyNexusHandles.framebufferHD);

#if SD_DISPLAY
    NEXUS_Display_GetGraphicsSettings(g_StandbyNexusHandles.display1, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(g_StandbyNexusHandles.display1, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(g_StandbyNexusHandles.display1, g_StandbyNexusHandles.framebufferSD);
#endif

    g_StandbyNexusHandles.window0 = NEXUS_VideoWindow_Open(g_StandbyNexusHandles.display0, 0);
    NEXUS_VideoWindow_GetSettings(g_StandbyNexusHandles.window0, &windowSettings);
    windowSettings.position.x = videoFormatInfo0.width/4;
    windowSettings.position.y = videoFormatInfo0.height/4;
    windowSettings.position.width = videoFormatInfo0.width/2;
    windowSettings.position.height = videoFormatInfo0.height/2;
    windowSettings.alpha = 0xFF;
    NEXUS_VideoWindow_SetSettings(g_StandbyNexusHandles.window0, &windowSettings);

#if SD_DISPLAY
    g_StandbyNexusHandles.window1 = NEXUS_VideoWindow_Open(g_StandbyNexusHandles.display1, 0);  
    NEXUS_VideoWindow_GetSettings(g_StandbyNexusHandles.window1, &windowSettings);
    windowSettings.position.x = videoFormatInfo1.width/4;
    windowSettings.position.y = videoFormatInfo1.height/4;
    windowSettings.position.width = videoFormatInfo1.width/2;
    windowSettings.position.height = videoFormatInfo1.height/2;
    windowSettings.alpha = 0xFF;
    NEXUS_VideoWindow_SetSettings(g_StandbyNexusHandles.window1, &windowSettings);  
#endif                    


    g_StandbyNexusHandles.playpump = NEXUS_Playpump_Open(0, NULL);    
    g_StandbyNexusHandles.playback = NEXUS_Playback_Create();    
    
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    g_StandbyNexusHandles.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(g_StandbyNexusHandles.playback, &playbackSettings);
    playbackSettings.playpump = g_StandbyNexusHandles.playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = g_StandbyNexusHandles.stcChannel;
    NEXUS_Playback_SetSettings(g_StandbyNexusHandles.playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    g_StandbyNexusHandles.audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(g_StandbyNexusHandles.platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(g_StandbyNexusHandles.audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(g_StandbyNexusHandles.platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(g_StandbyNexusHandles.audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(
    NEXUS_HdmiOutput_GetAudioConnector(g_StandbyNexusHandles.platformConfig.outputs.hdmi[0]),
    NEXUS_AudioDecoder_GetConnector(g_StandbyNexusHandles.audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
         

    /* bring up decoder and connect to display */
    g_StandbyNexusHandles.videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(g_StandbyNexusHandles.window0, NEXUS_VideoDecoder_GetConnector(g_StandbyNexusHandles.videoDecoder));
#if SD_DISPLAY
    NEXUS_VideoWindow_AddInput(g_StandbyNexusHandles.window1, NEXUS_VideoDecoder_GetConnector(g_StandbyNexusHandles.videoDecoder));
#endif   

    NEXUS_VideoWindow_GetSettings(g_StandbyNexusHandles.window0, &windowSettings);
    /* punch through the picture-in-graphics hole */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = g_StandbyNexusHandles.framebufferHD;
    fillSettings.rect = windowSettings.position;    
    fillSettings.color = 0x00cacaca;
    NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);

#if SD_DISPLAY
    NEXUS_VideoWindow_GetSettings(g_StandbyNexusHandles.window1, &windowSettings);
    /* punch through the picture-in-graphics hole */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = g_StandbyNexusHandles.framebufferSD;
    fillSettings.rect = windowSettings.position;
    fillSettings.color = 0x00cacaca;
    NEXUS_Graphics2D_Fill(g_StandbyNexusHandles.gfx, &fillSettings);
#endif

    rc = NEXUS_Graphics2D_Checkpoint(g_StandbyNexusHandles.gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
	rc = BKNI_WaitForEvent(g_StandbyNexusHandles.checkpointEvent, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);

#if NEXUS_HAS_CEC
    {
	NEXUS_CecSettings cecSettings;
	NEXUS_CecStatus cecStatus;
	NEXUS_HdmiOutputStatus status;
	unsigned loops;
	
	g_StandbyNexusHandles.hCec = g_StandbyNexusHandles.platformConfig.outputs.cec[0];
	
	NEXUS_HdmiOutput_GetStatus(g_StandbyNexusHandles.platformConfig.outputs.hdmi[0], &status);

	NEXUS_Cec_GetSettings(g_StandbyNexusHandles.hCec, &cecSettings);
        cecSettings.messageReceivedCallback.callback = cecMsgReceived_callback ;
        cecSettings.messageReceivedCallback.context = g_StandbyNexusHandles.event;
        
        cecSettings.messageTransmittedCallback.callback = cecMsgTransmitted_callback;
        cecSettings.messageTransmittedCallback.context = g_StandbyNexusHandles.event;
        
        cecSettings.logicalAddressAcquiredCallback.callback = cecDeviceReady_callback ;
        cecSettings.logicalAddressAcquiredCallback.context = g_StandbyNexusHandles.event;

        cecSettings.physicalAddress[0]= (status.physicalAddressA << 4)
            | status.physicalAddressB;
        cecSettings.physicalAddress[1]= (status.physicalAddressC << 4)
            | status.physicalAddressD;
	
	NEXUS_Cec_SetSettings(g_StandbyNexusHandles.hCec, &cecSettings);
	
	/* Enable CEC core */
	NEXUS_Cec_GetSettings(g_StandbyNexusHandles.hCec, &cecSettings);
        cecSettings.enabled = true;
        NEXUS_Cec_SetSettings(g_StandbyNexusHandles.hCec, &cecSettings);

	printf("Wait for logical address before starting test...\n");
	for (loops = 0; loops < 20; loops++) {
	    if (g_DeviceState.cecDeviceReady)
		break;
	    BKNI_Sleep(100);
	}
		
	NEXUS_Cec_GetStatus(g_StandbyNexusHandles.hCec, &cecStatus);
	if (cecStatus.logicalAddress == 0xFF)
	{
	    printf("No CEC capable device found on HDMI output\n");
	}
    }
#endif



    /* Open the audio and video pid channels */
    if(g_DeviceState.qam || g_DeviceState.sat) {	
	g_StandbyNexusHandles.parserBand = NEXUS_ParserBand_e0;
	g_StandbyNexusHandles.videoPidChannel = NEXUS_PidChannel_Open(g_StandbyNexusHandles.parserBand, 0x31, NULL);
        g_StandbyNexusHandles.audioPidChannel = NEXUS_PidChannel_Open(g_StandbyNexusHandles.parserBand, 0x34, NULL);

	NEXUS_VideoDecoder_GetDefaultStartSettings(&g_StandbyNexusHandles.videoProgram);
        g_StandbyNexusHandles.videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
        g_StandbyNexusHandles.videoProgram.pidChannel = g_StandbyNexusHandles.videoPidChannel;
        g_StandbyNexusHandles.videoProgram.stcChannel = g_StandbyNexusHandles.stcChannel;
        NEXUS_AudioDecoder_GetDefaultStartSettings(&g_StandbyNexusHandles.audioProgram);
        g_StandbyNexusHandles.audioProgram.codec = NEXUS_AudioCodec_eAc3;
        g_StandbyNexusHandles.audioProgram.pidChannel = g_StandbyNexusHandles.audioPidChannel;
        g_StandbyNexusHandles.audioProgram.stcChannel = g_StandbyNexusHandles.stcChannel;

	NEXUS_StcChannel_GetSettings(g_StandbyNexusHandles.stcChannel, &stcSettings);
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; 
        stcSettings.modeSettings.pcr.pidChannel = g_StandbyNexusHandles.videoPidChannel; 
        NEXUS_StcChannel_SetSettings(g_StandbyNexusHandles.stcChannel, &stcSettings);    
    } else {
	NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
	playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
	playbackPidSettings.pidTypeSettings.video.index = true;
	playbackPidSettings.pidTypeSettings.video.decoder = g_StandbyNexusHandles.videoDecoder;
	g_StandbyNexusHandles.videoPidChannel = NEXUS_Playback_OpenPidChannel(g_StandbyNexusHandles.playback, 0x21, &playbackPidSettings);
	
	NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
	playbackPidSettings.pidTypeSettings.audio.primary = g_StandbyNexusHandles.audioDecoder;
	g_StandbyNexusHandles.audioPidChannel = NEXUS_Playback_OpenPidChannel(g_StandbyNexusHandles.playback, 0x22, &playbackPidSettings);
	
	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	   the audio outputs. */
	NEXUS_VideoDecoder_GetDefaultStartSettings(&g_StandbyNexusHandles.videoProgram);
	g_StandbyNexusHandles.videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
	g_StandbyNexusHandles.videoProgram.pidChannel = g_StandbyNexusHandles.videoPidChannel;
	g_StandbyNexusHandles.videoProgram.stcChannel = g_StandbyNexusHandles.stcChannel;
	NEXUS_AudioDecoder_GetDefaultStartSettings(&g_StandbyNexusHandles.audioProgram);
	g_StandbyNexusHandles.audioProgram.codec = NEXUS_AudioCodec_eMpeg;
	g_StandbyNexusHandles.audioProgram.pidChannel = g_StandbyNexusHandles.audioPidChannel;
	g_StandbyNexusHandles.audioProgram.stcChannel = g_StandbyNexusHandles.stcChannel;
    }
    
    if(g_DeviceState.graphics_enabled)
	graphics_start();
    decode_start();
    
#if NEXUS_HAS_FRONTEND
    if(g_DeviceState.qam)
	tune_qam();
    else if (g_DeviceState.sat)
	tune_sat();
    else
#endif
	playback_start();
}

void stop_app(void)
{
    /* Bring down system */
#if NEXUS_HAS_FRONTEND
    if(g_DeviceState.qam)
	untune_qam();
    else if (g_DeviceState.sat)
	untune_sat();
    else
#endif
	playback_stop();
    decode_stop();
    graphics_stop();

#if NEXUS_HAS_KEYPAD
    NEXUS_Keypad_Close(g_StandbyNexusHandles.keypadHandle);
#endif
#if NEXUS_HAS_UHF_INPUT
    NEXUS_UhfInput_Close(g_StandbyNexusHandles.uhfHandle);
#endif
    NEXUS_IrInput_Close(g_StandbyNexusHandles.irHandle);    
    NEXUS_Playback_Destroy(g_StandbyNexusHandles.playback);
    NEXUS_Playpump_Close(g_StandbyNexusHandles.playpump);
    NEXUS_VideoWindow_RemoveInput(g_StandbyNexusHandles.window0, NEXUS_VideoDecoder_GetConnector(g_StandbyNexusHandles.videoDecoder));
#if SD_DISPLAY
    NEXUS_VideoWindow_RemoveInput(g_StandbyNexusHandles.window1, NEXUS_VideoDecoder_GetConnector(g_StandbyNexusHandles.videoDecoder));
#endif
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(g_StandbyNexusHandles.videoDecoder));
    NEXUS_VideoWindow_Close(g_StandbyNexusHandles.window0);
#if SD_DISPLAY
    NEXUS_VideoWindow_Close(g_StandbyNexusHandles.window1);
#endif
    NEXUS_Display_Close(g_StandbyNexusHandles.display0);
#if SD_DISPLAY
    NEXUS_Display_Close(g_StandbyNexusHandles.display1);  
#endif
    NEXUS_VideoDecoder_Close(g_StandbyNexusHandles.videoDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(g_StandbyNexusHandles.platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(g_StandbyNexusHandles.platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(g_StandbyNexusHandles.audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(g_StandbyNexusHandles.audioDecoder); 
    NEXUS_Surface_Destroy(g_StandbyNexusHandles.offscreenHD);
    NEXUS_Surface_Destroy(g_StandbyNexusHandles.framebufferHD);
#if SD_DISPLAY
    NEXUS_Surface_Destroy(g_StandbyNexusHandles.offscreenSD);
    NEXUS_Surface_Destroy(g_StandbyNexusHandles.framebufferSD);
#endif
    NEXUS_Graphics2D_Close(g_StandbyNexusHandles.gfx);  
    NEXUS_StcChannel_Close(g_StandbyNexusHandles.stcChannel);  
    BKNI_DestroyEvent(g_StandbyNexusHandles.event);
    BKNI_DestroyEvent(g_StandbyNexusHandles.checkpointEvent);
    BKNI_DestroyEvent(g_StandbyNexusHandles.spaceAvailableEvent);
}

int umount_all(void)
{
    FILE *f;

    g_power_state.total_mounts = 0;

    f = fopen("/proc/mounts", "r+");
    if (!f) return -1;
    while (!feof(f) && g_power_state.total_mounts < MAX_MOUNTS) {
        char buf[256];
        char *c1, *c2;
        if (fgets(buf, 256, f)) {
            if (strstr(buf, "/dev/sd") == buf || strstr(buf, " nfs ") || strstr(buf, "/dev/mmcblk") ) {
                c1 = strchr(buf, ' ');
                if (!c1) continue;

                *c1 = 0;
                strcpy(g_power_state.mounts[g_power_state.total_mounts].device, buf);
                c1++;
                c2 = strchr(c1, ' ');
                if (!c2) continue;

                *c2 = 0;
                strcpy(g_power_state.mounts[g_power_state.total_mounts].mountpoint, c1);

                printf("found mount point %d: %s %s\n", g_power_state.total_mounts, g_power_state.mounts[g_power_state.total_mounts].device, g_power_state.mounts[g_power_state.total_mounts].mountpoint);

                snprintf(buf, 256, "umount %s", g_power_state.mounts[g_power_state.total_mounts].mountpoint);
                if (!system(buf)) {
                    g_power_state.total_mounts++;
                }
            }
        }
    }
    fclose(f);
    
    return 0;
}

int wait_for_device( char *device_path, unsigned long timeout_in_ms )
{
    int     rc = 0;

    if (index( device_path, ':') == NULL   ) {  /* If device has ":", then it's an NFS path... skip it. */
        unsigned long   ms;
        unsigned long   ms_interval = 200;
        unsigned long   ms_max = timeout_in_ms;
        int             rc;
        struct stat     statbuf;

        for (ms=0 ; ms<ms_max ; ms+=ms_interval) {
            rc = stat(device_path, &statbuf);
            if (rc == 0) break;
            if (ms == 0) BKNI_Printf("Waiting for device %s to exist...\n", device_path);
            BKNI_Sleep(ms_interval);
        }
        if (ms >= ms_max) {
            BKNI_Printf("Device %s not found after %lu ms\n", device_path, ms );
            return 1;   /* return error code */
        }
        else if (ms > 0) {
            BKNI_Printf("Device %s appeared after %lu ms\n", device_path, ms );
        }
    }

    return rc;
}

int mount_all(void)
{
    unsigned i;

    for (i=0;i<g_power_state.total_mounts;i++) {
        char buf[256];
        int rc;

        rc = wait_for_device( g_power_state.mounts[i].device, 10000 );
        if (rc ) {    /* Device isn't there or not ready... */
            BKNI_Printf("Can't mount device \n", g_power_state.mounts[i].device );
            continue;
        }

        snprintf(buf, 256, "mount %s %s", g_power_state.mounts[i].device, g_power_state.mounts[i].mountpoint);
        printf("%s\n", buf);
        system(buf);
    }
    g_power_state.total_mounts = 0;
    
    return 0;
}

void get_pmlib_state(pmlib_state_t *state)
{
    *state = g_power_state.g_state;
}

int set_pmlib_state(const pmlib_state_t *state)
{
    struct brcm_pm_state pmlib_state;    
    BERR_Code rc;

    rc = brcm_pm_get_status(g_power_state.brcm_pm_ctx, &pmlib_state);
    if(rc) {printf("Cant get PM status\n");}

    pmlib_state.usb_status = state->usb;
#if MEMC1_SUPPORT 
    pmlib_state.memc1_status = state->memc1;
#else
    pmlib_state.memc1_status = BRCM_PM_UNDEF;
#endif
    pmlib_state.tp1_status = state->tp1;
    /* pmlib_state.cpu_divisor = state->cpu?1:8; */
    pmlib_state.ddr_timeout = state->ddr?0:64;
    
    pmlib_state.standby_flags = state->flags;

    /* 2.6.37-2.4 Kernel has some issue while resuming from S2 standby mode. So it requires
     * some delay to added while resuming. Changing the flag to 0x4 makes sure that this
     * delay is added. Needs to be removed once the Kernel fix is available.
     */
#if (BCHP_CHIP == 7358) 	
    pmlib_state.standby_flags |=  0x4;
#endif

    rc = brcm_pm_set_status(g_power_state.brcm_pm_ctx, &pmlib_state);
    if(rc) {printf("Cant set PM status\n");}
    
    if(state->enet) {
	if(!g_power_state.g_state.enet) {
	    system("ifup eth0");  	    	    	    
	}
    } else {
	if(g_power_state.g_state.enet)	    
	    system("ifdown eth0");
    }

    if(state->moca) {
	if(!g_power_state.g_state.moca){
	    system("mocactl start");
	    system("ifup eth1");	    	    
	}
    } else {
	if(g_power_state.g_state.moca){
	    system("ifdown eth1");
	    system("mocactl stop");
	}
    }

    g_power_state.g_state = *state;

    return 0;
}


void *commands_thread(void *context)
{    
    BSTD_UNUSED(context);

    for (g_DeviceState.exit_app=false;!g_DeviceState.exit_app;)
    {
        char buffer[256];
        char *buf;
	
	printf("standby>"); fflush(stdout);
        fgets(buffer, 256, stdin);
        if (feof(stdin)) break;
        buffer[strlen(buffer)-1] = 0; /* chop off \n */

        buf = strtok(buffer, ";");
        if (!buf) continue;

	do {
	    if (!strcmp(buf, "?") || !strcmp(buf, "help")) {
                printf(
                "Commands:\n"
                "  s1 - Enter S1 Standby\n"
                "  s2 - Enter S2 Standby\n"
                "  s3 - Enter S3 Standby\n"                
                "  all - Cycle through all Standby states\n"
                );                
                printf(
                "  q - quit\n"
                );
            } else if (!strcmp(buf, "s1")) {
		g_DeviceState.s1_only = true;
		g_DeviceState.cli = true;
		BKNI_SetEvent(g_StandbyNexusHandles.event);
	    } else if (!strcmp(buf, "s2")) {
		g_DeviceState.s2_only = true;
		g_DeviceState.cli = true;
		BKNI_SetEvent(g_StandbyNexusHandles.event);
	    } else if (!strcmp(buf, "s3")) {
		g_DeviceState.s3_only = true;
		g_DeviceState.cli = true;
		BKNI_SetEvent(g_StandbyNexusHandles.event);
	    } else if (!strcmp(buf, "all")) {		
		BKNI_SetEvent(g_StandbyNexusHandles.event);
	    } else if (!strcmp(buf, "q")) {
                g_DeviceState.exit_app = true;
		BKNI_SetEvent(g_StandbyNexusHandles.event);
                break;
            } else {
                printf("unknown command: '%s' (use '?' for list)\n", buf);
            } 
	} while ((buf = strtok(NULL, ";")));
    }

    return NULL;
}

int main(int argc, char **argv)
{      
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformStandbySettings nexusStandbySettings;
    NEXUS_PlatformStandbyStatus nexusStandbyStatus;
    NEXUS_Error rc;
    NEXUS_IrInputDataFilter irPattern;
    NEXUS_TransportWakeupSettings xptWakeupSettings;    
    NEXUS_InputBand inputBand;

    pmlib_state_t pmlib_state;  

    int curarg = 0;
    unsigned timeout = 5; /* seconds */
    bool _auto=false, ir_wakeup=true, uhf_wakeup=true, xpt_wakeup=true, cec_wakeup=true, gpio_wakeup=true, eth_wol_wakeup=true, moca_wol_wakeup=true, ethoff=false, mocaoff=false, openfe=false;
    int standby_flags=0;

    BKNI_Memset(&g_StandbyNexusHandles, 0, sizeof(g_StandbyNexusHandles));
    BKNI_Memset(&g_DeviceState, 0, sizeof(g_DeviceState));
    g_DeviceState.graphics_enabled = true;

    while (++curarg < argc) {
        if (!strcmp(argv[curarg], "--help")) {
	    printf("Usage: nexus standby [-timeout SECONDS]\n");
	    printf("\n");
	    printf("-timeout SECONDS   sets the standby timeout. 0 means no timeout.\n"  );
	    printf("-ir                1 : enable IR wake up;  0 : disable IR wakeup; default is 1.\n");
	    printf("-uhf               1 : enable UHF wake up; 0 : disable UHF wakeup; default is 1.\n");
	    printf("-xpt               1 : enable XPT wake up; 0 : disable XPT wakeup; default is 1.\n");
	    printf("-cec               1 : enable CEC wake up; 0 : disable CEC wakeup; default is 1.\n");
	    printf("-gpio              1 : enable GPIO wake up; 0 : disable GPIO wakeup; default is 1.\n");
	    printf("-ethwol            1 : enable ETH WOL wake up; 0 : disable ETH WOL wakeup; default is 1.\n");
	    printf("-mocawol           1 : enable MOCA WOL wake up; 0 : disable MOCA WOL wakeup; default is 1.\n");
	    printf("-ethoff            1 : Eth  is powered down in S1; 0 : Eth  is powered up in S1; default is 0.\n");
	    printf("-mocaoff           1 : Moca is powered down in S1; 0 : Moca is powered up in S1; default is 0.\n");
	    printf("-g_StandbyNexusHandles.gfx               1 : enable graphics; 0 : disable graphics; default is 1.\n");
	    printf("-qam               Use qam input. Default is playback.\n");
	    printf("-sat               Use satellite input. Default is playback.\n");
	    printf("-auto              Automatically transition through all power states.\n");		    
	    printf("-flags             Standby Flags. Used for debug. e.g. -flags 0x1\n");
            return 0;
        }
        else if (!strcmp(argv[curarg], "-timeout") && curarg+1 < argc) {
            timeout = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-ir") && curarg+1 < argc) {
            ir_wakeup = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-uhf") && curarg+1 < argc) {
            uhf_wakeup = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-xpt") && curarg+1 < argc) {
            xpt_wakeup = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-cec") && curarg+1 < argc) {
            cec_wakeup = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-gpio") && curarg+1 < argc) {
            gpio_wakeup = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-ethwol") && curarg+1 < argc) {
            eth_wol_wakeup = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-mocawol") && curarg+1 < argc) {
            moca_wol_wakeup = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-ethoff") && curarg+1 < argc) {
            ethoff = atoi(argv[++curarg]);
        } 
	else if (!strcmp(argv[curarg], "-mocaoff") && curarg+1 < argc) {
            mocaoff = atoi(argv[++curarg]);
        } 
	else if(!strcmp(argv[curarg], "-gfx") && curarg+1 < argc){
	    g_DeviceState.graphics_enabled = atoi(argv[++curarg]);
	}
	else if (!strcmp(argv[curarg], "-flags") && curarg+1 < argc) {
            standby_flags = strtol(argv[++curarg], NULL, 0);
        } 
	else if(!strcmp(argv[curarg], "-qam")){
	    g_DeviceState.qam = true;
	}
	else if(!strcmp(argv[curarg], "-sat")){
	    g_DeviceState.sat = true;
	}
	else if(!strcmp(argv[curarg], "-openfe")){
	    openfe = true;
	}	
	else if (!strcmp(argv[curarg], "-auto")) {
            _auto = true;
        } 
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = (g_DeviceState.qam || g_DeviceState.sat || openfe)?true:false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    g_power_state.brcm_pm_ctx = brcm_pm_init();
    g_power_state.g_state.enet = true;
#if MOCA_SUPPORT
    g_power_state.g_state.moca = true;
#else
    g_power_state.g_state.moca = false;
#endif

    get_pmlib_state(&pmlib_state);
    pmlib_state.usb = true;
    pmlib_state.enet = true;
#if MOCA_SUPPORT
    pmlib_state.moca = true;
#else
    pmlib_state.moca = false;
#endif
    pmlib_state.tp1 = true;
    pmlib_state.cpu = true;
    pmlib_state.ddr = true;
#if MEMC1_SUPPORT
    pmlib_state.memc1 = true;
#else
    pmlib_state.memc1 = false;
#endif
    pmlib_state.flags = standby_flags;
    set_pmlib_state(&pmlib_state);

    if(eth_wol_wakeup) {
	/* Enable LAN WOL */
	system("ethtool -s eth0 wol g");
    } else {
	/* Disable LAN WOL */
	system("ethtool -s eth0 wol d");
    }

    if(moca_wol_wakeup) {
	/* Enable MOCA WOL */
	system("mocactl wol --enable");
	system("ethtool -s eth1 wol g");
    } else {
	/* Disable MOCA WOL */
	system("mocactl wol --disable");
	system("ethtool -s eth1 wol d");
	system("mocactl stop");
    }

    start_app();        

    /* rc = pthread_create(&cli_thread, NULL, commands_thread, NULL); */
/*     BDBG_ASSERT(!rc); */

    while(1) {
	rc = BKNI_WaitForEvent(g_StandbyNexusHandles.event, _auto?(timeout*1000):0xFFFFFFFF); 

#if NEXUS_HAS_FRONTEND
	if(g_DeviceState.qam)
	    untune_qam();
	else if(g_DeviceState.sat)
	    untune_sat();
	else
#endif
	    playback_stop();

	decode_stop();
	graphics_stop();


	if(g_DeviceState.exit_app) {
	    break;
	} 
	else if(g_DeviceState.s1_only){
	    goto s1;
	} 
	else if(g_DeviceState.s2_only){
	    goto s2;
	}
#if S3_STANDBY_SUPPORT 
	else if(g_DeviceState.s3_only){
	    goto s3;
	}
#endif


	/**
	 * Enter S1 standby
	 **/
s1:
	printf("Entering S1 mode\n");   	

	NEXUS_IrInput_GetDefaultDataFilter(&irPattern );
	irPattern.patternWord0 = POWER_STANDBY_IR_CODE;
	NEXUS_IrInput_EnableDataFilter(g_StandbyNexusHandles.irHandle, &irPattern);

	NEXUS_TransportWakeup_GetSettings(&xptWakeupSettings);
	BKNI_Memcpy(xptWakeupSettings.filter[0].packet, Filter, sizeof(Filter));
	xptWakeupSettings.inputBand = inputBand;
	xptWakeupSettings.packetLength = sizeof(Filter)/sizeof(Filter[0]);
	xptWakeupSettings.wakeupCallback.callback = transportWakeupCallback;
	xptWakeupSettings.wakeupCallback.context = NULL;
	xptWakeupSettings.enabled = true;
	rc = NEXUS_TransportWakeup_SetSettings(&xptWakeupSettings);
	BDBG_ASSERT(!rc);

	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eActive;    
	rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
	BDBG_ASSERT(!rc);

	get_pmlib_state(&pmlib_state);
	pmlib_state.usb = false;
	pmlib_state.enet = ethoff?false:true;
#if MOCA_SUPPORT
	pmlib_state.moca = mocaoff?false:true;
#else
	pmlib_state.moca = false;
#endif
	pmlib_state.tp1 = false;
	pmlib_state.cpu = false;
	pmlib_state.ddr = false;
	pmlib_state.memc1 = false;
	set_pmlib_state(&pmlib_state);

	rc = BKNI_WaitForEvent(g_StandbyNexusHandles.event, g_DeviceState.s1_only?0xFFFFFFFF:timeout*1000);     

	printf("Resume from S1\n");
	get_pmlib_state(&pmlib_state);
	pmlib_state.usb = true;
	pmlib_state.enet = true;
#if MOCA_SUPPORT
	pmlib_state.moca = true;
#else
	pmlib_state.moca = false;
#endif
	pmlib_state.tp1 = true;
	pmlib_state.cpu = true;
	pmlib_state.ddr = true;
#if MEMC1_SUPPORT
	pmlib_state.memc1 = true;
#else
	pmlib_state.memc1 = false;
#endif
	set_pmlib_state(&pmlib_state);          
    
	g_DeviceState.s1_only = false;

	if(rc == BERR_SUCCESS) {
	    BDBG_WRN(("S1 Resume didnt happen from Timeout"));
	    goto resume;
	}
    
	/**
	 * Enter S2 standby
	 **/   
s2:
	printf("Entering S2 Mode\n");    

	NEXUS_TransportWakeup_GetSettings(&xptWakeupSettings);
	BKNI_Memcpy(xptWakeupSettings.filter[0].packet, Filter, sizeof(Filter));
	xptWakeupSettings.inputBand = inputBand;
	xptWakeupSettings.packetLength = sizeof(Filter)/sizeof(Filter[0]);
	xptWakeupSettings.wakeupCallback.callback = NULL/* transportWakeupCallback */;
	xptWakeupSettings.wakeupCallback.context = NULL;
	xptWakeupSettings.enabled = true;
	rc = NEXUS_TransportWakeup_SetSettings(&xptWakeupSettings);
	BDBG_ASSERT(!rc);

	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_ePassive;
	nexusStandbySettings.wakeupSettings.ir = ir_wakeup;
	nexusStandbySettings.wakeupSettings.uhf = uhf_wakeup;
	nexusStandbySettings.wakeupSettings.transport = xpt_wakeup;
#if NEXUS_HAS_CEC
	nexusStandbySettings.wakeupSettings.cec = cec_wakeup;
#endif	
	nexusStandbySettings.wakeupSettings.gpio = gpio_wakeup;
	nexusStandbySettings.wakeupSettings.timeout = g_DeviceState.s2_only?0:timeout;
	rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
	BDBG_ASSERT(!rc);
    
	umount_all();        		
	
	brcm_pm_suspend(g_power_state.brcm_pm_ctx, BRCM_PM_STANDBY);     

	NEXUS_Platform_GetStandbyStatus(&nexusStandbyStatus);
    
	printf("S2 Wake up Status\n"
	       "Standby Status : \n"
	       "IR      : %d\n"
	       "UHF     : %d\n"
	       "XPT     : %d\n"
	       "CEC     : %d\n"	       	       
	       "GPIO    : %d\n"	  
	       "Timeout : %d\n"	  
	       "\n",
	       nexusStandbyStatus.wakeupStatus.ir, 
	       nexusStandbyStatus.wakeupStatus.uhf,
	       nexusStandbyStatus.wakeupStatus.transport,
	       nexusStandbyStatus.wakeupStatus.cec,
	       nexusStandbyStatus.wakeupStatus.gpio,
	       nexusStandbyStatus.wakeupStatus.timeout);
	

	g_DeviceState.s2_only = false;

	if(!nexusStandbyStatus.wakeupStatus.timeout) {
	    BDBG_WRN(("S2 Resume didnt happen from Timeout"));
	    goto resume;
	}

#if S3_STANDBY_SUPPORT
	/**
	 * Enter S3 standby
	 **/   
s3 :
	printf("Entering S3 Mode\n");  


	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eDeepSleep;  
	nexusStandbySettings.wakeupSettings.ir = ir_wakeup;
#if NEXUS_HAS_CEC
	nexusStandbySettings.wakeupSettings.cec = cec_wakeup;
#endif	
	nexusStandbySettings.wakeupSettings.gpio = gpio_wakeup;
	nexusStandbySettings.wakeupSettings.timeout = _auto?timeout:0;
	rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
	BDBG_ASSERT(!rc);

#if S3_WARM_BOOT    
	printf("S3 Warm Boot\n");
	system("echo mem > /sys/power/state"); 	
#else 
	printf("S3 Cold Boot\n");
	system("echo 1 > /sys/devices/platform/brcmstb/halt_mode");
	system("halt");
	return 0;
#endif
	NEXUS_Platform_GetStandbyStatus(&nexusStandbyStatus);

	printf("S3 Wake up Status\n"
	       "Standby Status : \n"
	       "IR      : %d\n"
	       "UHF     : %d\n"
	       "XPT     : %d\n"
	       "CEC     : %d\n"	  
	       "GPIO    : %d\n"	  
	       "Timeout : %d\n"	       
	       "\n",
	       nexusStandbyStatus.wakeupStatus.ir, 
	       nexusStandbyStatus.wakeupStatus.uhf,
	       nexusStandbyStatus.wakeupStatus.transport,
	       nexusStandbyStatus.wakeupStatus.cec,
	       nexusStandbyStatus.wakeupStatus.gpio,
	       nexusStandbyStatus.wakeupStatus.timeout);
#endif
	g_DeviceState.s3_only = false;
	
resume:        

	mount_all();

	printf("Resuming Normal Mode\n");
		
    
	NEXUS_Platform_GetStandbySettings(&nexusStandbySettings);
	nexusStandbySettings.mode = NEXUS_PlatformStandbyMode_eOn;    
	nexusStandbySettings.openFrontend = (g_DeviceState.qam || g_DeviceState.sat || openfe);
	rc = NEXUS_Platform_SetStandbySettings(&nexusStandbySettings);
	BDBG_ASSERT(!rc);
    

	NEXUS_IrInput_DisableDataFilter(g_StandbyNexusHandles.irHandle);              
	
	if(g_DeviceState.graphics_enabled)
	    graphics_start();
	decode_start();

#if NEXUS_HAS_FRONTEND
	if(g_DeviceState.qam)
	    tune_qam();
	else if(g_DeviceState.sat)
	    tune_sat();
	else
#endif
	    playback_start();

    }       

    stop_app();

    NEXUS_Platform_Uninit();
    
    system("if [ -e /tmp/udhcpc.eth0.pid ]; then kill `cat /tmp/udhcpc.eth0.pid`; fi");
    system("if [ -e /tmp/udhcpc.eth1.pid ]; then kill `cat /tmp/udhcpc.eth1.pid`; fi");
        
    return 0;
}
