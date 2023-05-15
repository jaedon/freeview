/**************************** Include Files**********************************/ 
#include "framework.h"

/* Linux stuff */
#include <stdio.h>          /* for printf */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

/* APE includes */
#include "bape.h"

#include "apetest_hdmitx.h"

#include "hbr_tone.h"

BDBG_MODULE(ape_test);

/*******************************************************************/
/*******************************************************************/
int app_main(int argc, char **argv)
{
    int             iErr;

	BSystem_Info    sysInfo;
    BFramework_Info frmInfo;
    BERR_Code       errCode;
    BAPE_Settings   apeSettings;
    BAPE_Handle            ape;
    BAPE_PlaybackHandle    playback;
    BAPE_PlaybackOpenSettings playbackOpenSettings;
    BAPE_MaiOutputHandle   mai;
    BAPE_MixerHandle       maiMixer;
    BAPE_Connector         connector;
    BAPE_OutputPort        outputPort;
    BAPE_MixerSettings     mixerSettings;
    BAPE_MixerAddInputSettings mixerAddSettings;
    BAPE_BufferDescriptor  bufferDescriptor;
    BAPE_PlaybackSettings  playbackSettings;
    BAPE_PlaybackStartSettings playbackStartSettings;
    APETEST_HDMITX_Settings hdmitxSettings;
    int16_t *pBuffer;
    void *pCached;
    size_t bufferSize;
	    
    iErr = BSystem_Init( argc, argv, &sysInfo );
    if ( iErr )
    {
        printf( "System init FAILED!\n" );
        return iErr;
    }

    iErr = BFramework_Init( &sysInfo, &frmInfo );
    if ( iErr )
    {
        printf( "Framework init FAILED!\n" );
        return iErr;
    }

#if 0        
    BDBG_SetModuleLevel("BHDM*", BDBG_eMsg);
#endif
    BDBG_SetModuleLevel("bape_startprint", BDBG_eMsg);

    /* Open APE PI */
    BAPE_GetDefaultSettings(&apeSettings);
    errCode = BAPE_Open(&ape, 
                        frmInfo.hChp, 
                        frmInfo.hReg, 
                        frmInfo.hMem, 
                        frmInfo.hInt, 
                        frmInfo.hTmr, 
                        NULL, /* No DSP Required for Playback */
                        &apeSettings);

    /* Open Playback Channel */
    BAPE_Playback_GetDefaultOpenSettings(&playbackOpenSettings);
    playbackOpenSettings.numBuffers = 1;
    playbackOpenSettings.bufferSize = sizeof(g_hbrTone);
    errCode = BAPE_Playback_Open(ape, 0, &playbackOpenSettings, &playback);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    /* Open MAI and mixer */
    errCode = BAPE_MaiOutput_Open(ape, 0, NULL, &mai);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    APETEST_HDMITX_GetDefaultSettings(&hdmitxSettings);
    /* Setup for compressed HBR */
    hdmitxSettings.speakerAllocation = 0xff;
    hdmitxSettings.channelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_eReferToStreamHeader;
    errCode = APETEST_HDMITX_Init(&frmInfo, &hdmitxSettings, mai);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_Mixer_GetDefaultSettings(&mixerSettings);
    mixerSettings.outputNco = BAPE_Nco_eMax;
    errCode = BAPE_Mixer_Create(ape, &mixerSettings, &maiMixer); 
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_Playback_GetConnector(playback, &connector);
    BAPE_Mixer_GetDefaultAddInputSettings(&mixerAddSettings);
    mixerAddSettings.sampleRateMaster = true;
    errCode = BAPE_Mixer_AddInput(maiMixer, connector, &mixerAddSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_MaiOutput_GetOutputPort(mai, &outputPort);
    errCode = BAPE_Mixer_AddOutput(maiMixer, outputPort);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Setup Playback Stream Parameters */
    BAPE_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.compressedData = true;
    BAPE_Playback_SetSettings(playback, &playbackSettings);

    /* Load data into buffer */
    errCode = BAPE_Playback_GetBuffer(playback, &bufferDescriptor);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    (void)BMEM_ConvertAddressToCached(frmInfo.hMem, bufferDescriptor.buffers[0].pBuffer, &pCached);
    pBuffer = pCached;
    bufferSize = bufferDescriptor.bufferSize;
    BKNI_Memcpy(pBuffer, g_hbrTone, bufferSize);
    (void)BMEM_FlushCache(frmInfo.hMem, pCached, bufferSize);
    errCode = BAPE_Playback_CommitData(playback, bufferSize);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Start Playback */
    BAPE_Playback_GetDefaultStartSettings(&playbackStartSettings);
    playbackStartSettings.sampleRate = 192000;      /* HBR */
    playbackStartSettings.bitsPerSample = 16*4;     /* IEC-61937x16 */
    playbackStartSettings.interleaved = true;
    playbackStartSettings.loopEnabled = true;
    errCode = BAPE_Playback_Start(playback, &playbackStartSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    printf("Playback started.  Press Enter to Exit\n");
    getchar();

    printf("Stopping playback\n");
    BAPE_Playback_Stop(playback);

    /* Teardown Connections */
    BAPE_Mixer_RemoveAllInputs(maiMixer);
    BAPE_Mixer_RemoveAllOutputs(maiMixer);
    /* Close Resource Handles */
    BAPE_Mixer_Destroy(maiMixer);
    APETEST_HDMITX_Uninit();
    BAPE_MaiOutput_Close(mai);
    BAPE_Playback_Close(playback);

    /* Shutdown system */
    BAPE_Close(ape);

    BFramework_Uninit(&frmInfo);
    BSystem_Uninit(&sysInfo);

    printf("Done\n");
    return 0;
}

