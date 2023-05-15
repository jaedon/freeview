/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playback.c $
 * $brcm_Revision: Hydra_Software_Devel/24 $
 * $brcm_Date: 10/11/12 12:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/playback.c $
 * 
 * Hydra_Software_Devel/24   10/11/12 12:09p syang
 * SW7346-572: add long-time-lock for vdc_test
 * 
 * Hydra_Software_Devel/23   8/17/12 4:07p vanessah
 * SW7425-2501: further cleanup xport file
 * 
 * Hydra_Software_Devel/22   8/2/12 4:50p tdo
 * SW7425-3563: Fix bkni error when previously fail to open playback file
 *
 * Hydra_Software_Devel/21   7/26/12 10:37a tdo
 * SW7425-3563: Memory leak in vdctest/playback.c
 *
 * Hydra_Software_Devel/20   5/23/12 11:32a vanessah
 * SW7360-23: add 7360 a0 support
 *
 * Hydra_Software_Devel/19   5/3/12 1:32p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/18   12/6/11 3:57p tdo
 * SW7435-9: Add support for 7435
 *
 * Hydra_Software_Devel/17   10/12/11 11:02a pntruong
 * SW7429-16: Initial port.
 *
 * Hydra_Software_Devel/16   10/3/11 3:06p darnstein
 * SW7400-3059: add an error message for failure to open playback file.
 *
 * Hydra_Software_Devel/15   6/9/11 4:12p vanessah
 * SW7425-32: Add support to large file.
 *
 * Hydra_Software_Devel/14   4/27/11 4:16p tdo
 * SW7231-133, SW7422-417: Add support for multiple channel decode within
 * a decoder with streamer input and playback  Add option for users to
 * manually force SVD to only decode AVC, this is needed to do multi
 * channel decode with SVD due to memory allocation issue.  Cleanup old
 * code.
 *
 * Hydra_Software_Devel/13   4/22/11 5:41p tdo
 * SW7425-366: Add support for playback of more than 1 file to support
 * dual playback and decode.
 *
 * Hydra_Software_Devel/12   1/18/11 2:01p yuxiaz
 * SW7552-17: Merge from Branch for 7552 support.
 *
 * Hydra_Software_Devel/SW7552-17/1   12/20/10 7:31p xhuang
 * SW7552-17: add 7552 support
 *
 * Hydra_Software_Devel/11   1/6/11 4:26p tdo
 * SW7231-23: Remove include bxpt_priv.h from playback.[ch]
 *
 * Hydra_Software_Devel/10   1/4/11 4:59p tdo
 * SW7422-125: Problem with video playback, playback doesn't wrap
 *
 * Hydra_Software_Devel/9   12/10/10 2:02p hongtaoz
 * SW7425-32: use EMU utility to access between file and device memory;
 * tried to separate xpt muxer PID/rave channels from playback decoder's
 * channels;
 *
 * Hydra_Software_Devel/8   12/8/10 4:52p hongtaoz
 * SW7425-32: brng up playback test on emulation;
 *
 * Hydra_Software_Devel/7   12/2/10 3:08p katrep
 * SW7231-16:add more support for 7231
 *
 * Hydra_Software_Devel/6   11/18/10 2:38p darnstein
 * SW7231-16: port to 7344 and 7346.
 *
 * Hydra_Software_Devel/5   11/1/10 2:03p yuxiaz
 * SW7358-17: Added 7358 support to vdc_test.
 *
 * Hydra_Software_Devel/4   10/28/10 4:35p vanessah
 * SW7425-32:  vdc_test compile error
 *
 * Hydra_Software_Devel/3   10/26/10 6:40p hongtaoz
 * SW7425-32: added XPT compile support for 7425;
 *
 * Hydra_Software_Devel/2   12/7/09 5:32p tdo
 * SW7408-23: Remove direct TV stuff from playback
 *
 * Hydra_Software_Devel/1   12/4/09 3:55p tdo
 * SW7408-23: Add playback feature for digital source
 *
 ***************************************************************************/
#define _GNU_SOURCE
#include <unistd.h>
#include "playback.h"
#if defined(EMULATION) /* page client memory to prevent fread failure */
#include "pagelib.h"
#endif

BDBG_MODULE(vdc_test);

static Playback_Vars s_aPlaybackVars[MAX_PLAYBACKS];
static const Playback_Settings s_stPlaybackDefaultSettings =
{
    "",
    BXPT_PB_SYNC_MPEG_BLIND,
    0,
    false,
    false
};


static void Playback_DescDoneIsr
(
    void                       *Parm1,
    int                         Parm2)
{
    Playback_Handle hPlayback = Parm1;
    BSTD_UNUSED(Parm2);
    BKNI_SetEvent(hPlayback->DescriptorDoneEvent);
}

static void Playback_FeedData
(
    Playback_Handle             hPlayback
)
{
    BXPT_Playback_CreateDesc(hPlayback->hXpt,
                             hPlayback->Descriptor[hPlayback->iBufferNext],
                             hPlayback->Buffer[hPlayback->iBufferNext],
                             hPlayback->uiBufferSize[hPlayback->iBufferNext],
                             true,
                             false,
                            (BXPT_PvrDescriptor * const)NULL);

    BXPT_Playback_AddDescriptors(hPlayback->hPb,
                                 hPlayback->Descriptor[hPlayback->iBufferNext],
                                 hPlayback->Descriptor[hPlayback->iBufferNext]);

    hPlayback->iBufferNext = (hPlayback->iBufferNext + 1) % NUM_PB_BUFFERS;
}

static void Playback_FeedDataTask
(
    BTST_TaskHandle             Task,
    void                       *Parm)
{
    int i;
    unsigned int uiBytesRead;
    BXPT_Playback_ChannelStatus PbStatus;
    BXPT_PvrDescriptor *pCurrentDescriptor;
    bool bDoneInner, bDoneOuter;
    Playback_Handle hPlayback = Parm;

    BSTD_UNUSED(Task);

    while(1)
    {
        BKNI_WaitForEvent(hPlayback->PbStartEvent, BKNI_INFINITE);

        for(i = 0; i < NUM_PB_BUFFERS; i++)
        {
            if(hPlayback->bRunning == false)
            {
                break;
            }

            BTST_P_AcquireLongTimeLock(&hPlayback->stFileLock);
            if(hPlayback->InFilePtr != NULL)
            {
                #if defined(EMULATION) /* page client memory to prevent fread failure */
                uiBytesRead = BEMU_Client_Fread(hPlayback->Buffer[hPlayback->iBufferNext], PB_BUFFER_LEN, hPlayback->InFilePtr);;
                #else
                uiBytesRead = fread(hPlayback->Buffer[hPlayback->iBufferNext], sizeof(char), PB_BUFFER_LEN, hPlayback->InFilePtr);
                #endif
                BDBG_MSG(("playback task read %d bytes from file to %p for buf[%d]",
                    uiBytesRead, hPlayback->Buffer[hPlayback->iBufferNext], hPlayback->iBufferNext));
            }
            else
            {
                uiBytesRead = 0;
            }

            BTST_P_ReleaseLongTimeLock(&hPlayback->stFileLock);
            if(uiBytesRead == 0)
            {
                BDBG_ERR(("Zero byte read from pb file. Exit pb task!"));
                break;
            }
            hPlayback->uiBufferSize[hPlayback->iBufferNext] = uiBytesRead;
            Playback_FeedData(hPlayback);
        }

        BKNI_SetEvent(hPlayback->PbPrimedEvent);
        bDoneOuter = false;

        while(!bDoneOuter)
        {
            if(hPlayback->bRunning == false)
            {
                break;
            }

            if(BKNI_WaitForEvent(hPlayback->DescriptorDoneEvent, BKNI_INFINITE) != BERR_SUCCESS) continue;

            bDoneInner = false;

            while(!bDoneInner)
            {
                #if defined(EMULATION) /* prepare client memory to prevent fread failure */
                BKNI_Sleep(10000); /* emulator may be slow; sleep a while before the next loop */
                #endif
                if(hPlayback->bRunning == false)
                {
                    break;
                }

                BXPT_Playback_GetCurrentDescriptorAddress(hPlayback->hPb, &pCurrentDescriptor);

                if(hPlayback->Descriptor[hPlayback->iBufferNext] != pCurrentDescriptor)
                {
                    /*if(uiBytesRead != 0)*/
                    {
                        BTST_P_AcquireLongTimeLock(&hPlayback->stFileLock);
                        if(hPlayback->InFilePtr != NULL)
                        {
                            #if defined(EMULATION) /* prepare client memory to prevent fread failure */
                            uiBytesRead = BEMU_Client_Fread(hPlayback->Buffer[hPlayback->iBufferNext], PB_BUFFER_LEN, hPlayback->InFilePtr);;
                            #else
                            uiBytesRead = fread(hPlayback->Buffer[hPlayback->iBufferNext], sizeof(char), PB_BUFFER_LEN, hPlayback->InFilePtr);
                            #endif
                            BDBG_MSG(("playback task read %d bytes from file to %p for buf[%d]",
                                uiBytesRead, hPlayback->Buffer[hPlayback->iBufferNext], hPlayback->iBufferNext));
                        }
                        else
                        {
                            uiBytesRead = 0;
                        }
                        BTST_P_ReleaseLongTimeLock(&hPlayback->stFileLock);
                    }

                    if(uiBytesRead == 0)
                    {
                        /* If not repeating, go back and wait for another start event */
                        if(hPlayback->bRepeat == false)
                        {
                            bDoneInner = true;
                            bDoneOuter = true;
                            break;
                        }

                        /* Otherwise, rewind to the beginning and try again. */
                        BTST_P_AcquireLongTimeLock(&hPlayback->stFileLock);
                        if(hPlayback->InFilePtr != NULL)
                        {
                            fseek(hPlayback->InFilePtr, 0L, SEEK_SET);
                        }
                        BTST_P_ReleaseLongTimeLock(&hPlayback->stFileLock);
                        continue;
                    }

                    hPlayback->uiBufferSize[hPlayback->iBufferNext] = uiBytesRead;

                    Playback_FeedData(hPlayback);
                }
                else
                {
                    bDoneInner = true;
                }
            }
        }

        i = 0;
        bDoneOuter = false;
        BXPT_Playback_GetChannelStatus(hPlayback->hPb, &PbStatus);

        /* Signal the calling task that playback has finished. */
        while(!PbStatus.Finished)
        {
            /*BKNI_Sleep(50);*/
            BXPT_Playback_GetChannelStatus(hPlayback->hPb, &PbStatus);
        }

        /* Hack: Let's wait 10th of a second to make sure
        * RAVE is finished with the last descriptor
        * before signaling PbDoneEvent */
        /*BKNI_Sleep(100);*/
        BKNI_SetEvent(hPlayback->PbDoneEvent);
        if(hPlayback->bRunning == false)
        {
            break;
        }
    }
}

static BERR_Code Playback_GetPlaybackDefaultSettings
(
    Playback_Handle             hPlayback,
    Playback_Settings          *pPlaybackDefaultSettings
)
{
    BDBG_ASSERT(hPlayback);
    BDBG_ASSERT(pPlaybackDefaultSettings);

    BKNI_Memcpy((void *)pPlaybackDefaultSettings,
                (void *)&s_stPlaybackDefaultSettings,
                sizeof(Playback_Settings));

    return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
 ***************************************************************************/
Playback_Handle Init_Playback
(
    BXPT_Handle                 hXpt,
    BMEM_Handle                 hMem,
    BINT_Handle                 hInt,
    unsigned int                uiPbChannelNum
)
{
    Playback_Handle hPlayback = &s_aPlaybackVars[uiPbChannelNum];

    hPlayback->hXpt = hXpt;
    hPlayback->hMem = hMem;
    hPlayback->hInt = hInt;
    hPlayback->uiChannelNum = uiPbChannelNum;
    hPlayback->bRunning = false;
    hPlayback->bRepeat = false;
    hPlayback->bInit = false;
    hPlayback->iBufferNext = 0;

    /* Create Mutex */
    BTST_P_InitLongTimeLock(&hPlayback->stFileLock, 10*60);
    return(hPlayback);
}

/***************************************************************************
 ***************************************************************************/
void Open_PlaybackChan
(
    BXPT_Handle                 hXpt,
    Playback_Handle             hPlayback,
    unsigned int                uiPbChannelNum
)
{
    int i;
    BXPT_Playback_ChannelSettings xptPbSettings;

    /* Allocate buffers and descriptors */
    for(i = 0; i < NUM_PB_BUFFERS; i++)
    {
        hPlayback->Buffer[i] = (unsigned char *)BMEM_AllocAligned(hPlayback->hMem, PB_BUFFER_LEN, 12, 0);
        BDBG_ASSERT(hPlayback->Buffer[i]);

        hPlayback->Descriptor[i] = (BXPT_PvrDescriptor *)BMEM_AllocAligned(hPlayback->hMem, sizeof(BXPT_PvrDescriptor), 4, 0);
        BDBG_ASSERT(hPlayback->Descriptor[i]);
    }

    /* Create events */
    BKNI_CreateEvent(&hPlayback->PbStartEvent);
    BKNI_CreateEvent(&hPlayback->PbPrimedEvent);
    BKNI_CreateEvent(&hPlayback->PbDoneEvent);
    BKNI_CreateEvent(&hPlayback->DescriptorDoneEvent);

    /* Open playback channel */
    BXPT_Playback_GetChannelDefaultSettings(hXpt,
                                            uiPbChannelNum,
                                            &xptPbSettings);
    /* PR 51821: Set the AlwaysResumeFromLastDescriptor = true 
     * to avoid warnings from XPT.*/
#ifndef BXPT_NUM_FILTER_BANKS
     xptPbSettings.AlwaysResumeFromLastDescriptor = true;
#endif

    BXPT_Playback_OpenChannel(hXpt,
                              &hPlayback->hPb,
                              uiPbChannelNum,
                              &xptPbSettings);

    /* Create Callback */
    BINT_CreateCallback(&hPlayback->DescDoneIntCb,
                        hPlayback->hInt,
                        BXPT_Playback_GetIntId(
                            hPlayback->hPb,
                            BXPT_PbInt_eDone),
                        Playback_DescDoneIsr,
                        (void *) hPlayback,
                        0);

    BINT_EnableCallback(hPlayback->DescDoneIntCb);

	hPlayback->bInit = true;
	return;
}
/***************************************************************************
 ***************************************************************************/
void Close_PlaybackChan
(
    Playback_Handle             hPlayback
)
{
    int i;

	if(hPlayback->bInit == false)
		return;

    /* Destroy events */
    /* Free buffers and descriptors */
    for(i = 0; i < NUM_PB_BUFFERS; i++)
    {
        BMEM_Free(hPlayback->hMem, hPlayback->Buffer[i]);
        BMEM_Free(hPlayback->hMem, hPlayback->Descriptor[i]);
    }

    /* Close Playback channel */
    BXPT_Playback_CloseChannel(hPlayback->hPb);

    /* Destroy Callback */
    BINT_DisableCallback(hPlayback->DescDoneIntCb);
    BINT_DestroyCallback(hPlayback->DescDoneIntCb);

    /* Destroy events */
    BKNI_DestroyEvent(hPlayback->PbDoneEvent);
    BKNI_DestroyEvent(hPlayback->DescriptorDoneEvent);
    BKNI_DestroyEvent(hPlayback->PbStartEvent);
    BKNI_DestroyEvent(hPlayback->PbPrimedEvent);

	hPlayback->bInit = false;
	return;
}

static BERR_Code Playback_Play
(
    Playback_Handle             hPlayback,
    Playback_Settings          *pPlaySettings)
{
    BERR_Code rc = BERR_SUCCESS;
    BXPT_Playback_ChannelSettings xptPbChannelSettings;
    BXPT_Playback_PacketizeConfig pkt_cfg;

    BTST_P_AcquireLongTimeLock(&hPlayback->stFileLock);
    hPlayback->fd = 0;
    hPlayback->InFilePtr = 0;

    hPlayback->fd = open64(pPlaySettings->szFileName, O_DIRECT | O_RDONLY);
    if(!hPlayback->fd)
    {
        BTST_P_ReleaseLongTimeLock(&hPlayback->stFileLock);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    hPlayback->InFilePtr = fdopen(hPlayback->fd, "rb");
    if(!hPlayback->InFilePtr)
    {
        /* We're probably accessing a network file, so we can't use
        * direct access */
        close(hPlayback->fd);
        hPlayback->fd = 0;
        hPlayback->InFilePtr = fopen(pPlaySettings->szFileName, "rb");
        if(!hPlayback->InFilePtr)
        {
			BDBG_ERR(("failed to open file %s for playback",
				pPlaySettings->szFileName));
			BTST_P_ReleaseLongTimeLock(&hPlayback->stFileLock);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        BDBG_MSG(("opened network file %s for playback",
			pPlaySettings->szFileName));
    }
    BTST_P_ReleaseLongTimeLock(&hPlayback->stFileLock);

    /* Create Feeder Task */
    BTST_CreateTask(&hPlayback->hPbTask,
                    Playback_FeedDataTask,
                    (void *) hPlayback);

    hPlayback->bRunning = true;
    hPlayback->bRepeat = pPlaySettings->bRepeat;
    BXPT_Playback_GetChannelSettings(hPlayback->hPb, &xptPbChannelSettings);
    xptPbChannelSettings.SyncMode = pPlaySettings->uiSyncMode;

    if(pPlaySettings->bIsES == true)
    {
        xptPbChannelSettings.SyncMode = BXPT_PB_SYNC_MPEG_BLIND;
        xptPbChannelSettings.PacketLength = 0xB8;
    }
    else if(pPlaySettings->uiSyncMode == BXPT_PB_SYNC_MPEG_BLIND)
    {
        xptPbChannelSettings.PacketLength = 188;
    }

    BXPT_Playback_SetChannelSettings(hPlayback->hPb, &xptPbChannelSettings);

    /* Fix packetizing for ES streams */
    if(pPlaySettings->bIsES == true)
    {
        BKNI_Memset(&pkt_cfg, 0, sizeof(pkt_cfg));

        /* Setup packetizer */
        pkt_cfg.Pid = 0x100;
        pkt_cfg.ChannelNum = pPlaySettings->uiChannelNum;
        pkt_cfg.PacketizerMode = BXPT_Playback_PacketizerMode_Es;
        pkt_cfg.FilterConfig.StreamId = 0x00;
        BXPT_Playback_PacketizeStream(hPlayback->hPb, 0, &pkt_cfg, true);
    }

    /* Start the channel */
    BXPT_Playback_StartChannel(hPlayback->hPb);
    BKNI_SetEvent(hPlayback->PbStartEvent);
    BKNI_WaitForEvent(hPlayback->PbPrimedEvent, BKNI_INFINITE);

    return BERR_TRACE(rc);
}

/* Playback_DoPlay() will start playing the stream. At end of stream, the */
/* stream is repeated */
BERR_Code Playback_DoStartPlay
(
    Playback_Handle             hPlayback,
    char                       *FileName,
    unsigned int                SyncMode,
    bool                        isES
)
{
    BERR_Code rc = BERR_SUCCESS;
    Playback_Settings stPlaybackSettings;

    rc = Playback_GetPlaybackDefaultSettings(hPlayback, &stPlaybackSettings);
    if(rc != BERR_SUCCESS)
    {
        return BERR_TRACE(rc);
    }

    strcpy(stPlaybackSettings.szFileName, FileName);
    stPlaybackSettings.uiSyncMode = SyncMode;
    stPlaybackSettings.bIsES = isES;
    stPlaybackSettings.bRepeat = true;

    rc = Playback_Play(hPlayback, &stPlaybackSettings);
    if(rc != BERR_SUCCESS)
    {
        return BERR_TRACE(rc);
    }

    return BERR_TRACE(rc);
}

/* Playback_DoPlay() will stop playing a previously started stream. */
BERR_Code Playback_DoStopPlay
(
    Playback_Handle             hPlayback
)
{
    BERR_Code rc = BERR_SUCCESS;

    if(hPlayback->bRunning == false)
        return rc;

    hPlayback->bRunning = false;
    BKNI_WaitForEvent(hPlayback->PbDoneEvent, BKNI_INFINITE);

    /* Destroy Feeder Task */
    BTST_DestroyTask(hPlayback->hPbTask);

    BXPT_Playback_StopChannel(hPlayback->hPb);
    BTST_P_AcquireLongTimeLock(&hPlayback->stFileLock);
    if(hPlayback->InFilePtr)
    {
        fclose(hPlayback->InFilePtr);
        hPlayback->InFilePtr = NULL;
    }
    if(hPlayback->fd)
    {
        close(hPlayback->fd);
        hPlayback->fd = 0;
    }
    BTST_P_ReleaseLongTimeLock(&hPlayback->stFileLock);

    return BERR_TRACE(rc);
}

void Playback_Shutdown
(
    Playback_Handle             hPlayback
)
{
    /* Destroy Mutex */
    BTST_P_UninitLongTimeLock(&hPlayback->stFileLock);

#if 0
    /* Destroy events */
    /* These events cannot be destroyed do to issues
      described in PR 37013  */
    BKNI_DestroyEvent(hPlayback->PbStartEvent);
    BKNI_DestroyEvent(hPlayback->PbPrimedEvent);
    BKNI_DestroyEvent(hPlayback->PbDoneEvent);
    BKNI_DestroyEvent(hPlayback->DescriptorDoneEvent);
#endif
}

/* end of file */
