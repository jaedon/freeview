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
#include "apetest_common.h"

BDBG_MODULE(ape_test);

/* 1KHz sine wave at 48 KHz (0dBFS 16-bit/sample) */
static int16_t samples[48] =
{
0, 4276, 8480, 12539, 16383, 19947, 23169, 25995, 28377, 30272, 31650, 32486, 32767, 32486, 31650, 30272, 28377, 25995, 23169, 19947, 16383, 12539, 8480, 4276,
0,-4277,-8481,-12540,-16384,-19948,-23170,-25996,-28378,-30273,-31651,-32487,-32767,-32487,-31651,-30273,-28378,-25996,-23170,-19948,-16384,-12540,-8481,-4277
};

#define SWAP32( a )  do{a=((a&0xFF)<<24|(a&0xFF00)<<8|(a&0xFF0000)>>8|(a&0xFF000000)>>24);}while(0)
#define SWAP16( a )  do{a=((a&0xFF)<<8|(a&0xFF00)>>8);}while(0)

typedef struct wave_header
{
    unsigned long riff;         /* 'RIFF' */
    unsigned long riffCSize;    /* size in bytes of file - 8 */
    unsigned long wave;         /* 'WAVE' */
    unsigned long fmt;          /* 'fmt ' */
    unsigned long headerLen;    /* header length (should be 16 for PCM) */
    unsigned short format;      /* 1 - pcm */
    unsigned short channels;    /* 1 - mono, 2 - sterio */
    unsigned long samplesSec;   /* samples / second */
    unsigned long bytesSec;     /* bytes / second */
    unsigned short chbits;      /* channels * bits/sample /8 */
    unsigned short bps;         /* bits per sample (8 or 16) */
                                /* Extensible format */
    unsigned short cbSize;      /* 2 Size of the extension (0 or 22)  */
    unsigned short wValidBitsPerSample; /* 2 Number of valid bits  */
    unsigned short dwChannelMask; /* 4 Speaker position mask  */
    unsigned char SubFormat[16];  /* SubFormat */

    unsigned long dataSig;      /* 'data' */
    unsigned long dataLen;      /* length of data */
}wave_header;

/*******************************************************************/
/*******************************************************************/
static int  extract_wave_header( FILE *pFile, wave_header *pWh)
{
        /* read in the wave file header */
        fread(&pWh->riff,4,1,pFile);
        fread(&pWh->riffCSize,4,1,pFile);
        fread(&pWh->wave,4,1,pFile);
        fread(&pWh->fmt,4,1,pFile);
        fread(&pWh->headerLen,4,1,pFile);
        fread(&pWh->format,2,1,pFile);
        fread(&pWh->channels,2,1,pFile);
        fread(&pWh->samplesSec,4,1,pFile);
        fread(&pWh->bytesSec,4,1,pFile);
        fread(&pWh->chbits,2,1,pFile);
        fread(&pWh->bps,2,1,pFile);

    #if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
        SWAP32(pWh->riff);
        SWAP32(pWh->riffCSize);
        SWAP32(pWh->wave);
        SWAP32(pWh->fmt);
        SWAP32(pWh->headerLen);
        SWAP16(pWh->format);
        SWAP16(pWh->channels);
        SWAP32(pWh->samplesSec);
        SWAP32(pWh->bytesSec);
        SWAP16(pWh->chbits);
        SWAP16(pWh->bps);
    #endif  

        /* Verify that this is a PCM WAV file. */
        if (pWh->riff != 0x46464952) {      
            fprintf(stderr, "RAW data file detected.\n");
            fseek(pFile, 0, SEEK_SET);
        }
        else
        {
            if (pWh->wave != 0x45564157) {
                fprintf(stderr, "Not a WAV file.");
                return 1;
            }

            if (pWh->headerLen == 40 && pWh->format == 0xfffe) { /* WAVE_FORMAT_EXTENSIBLE */
                fread(&pWh->cbSize,2,1,pFile);              /* 2 Size of the extension (0 or 22)  */
                fread(&pWh->wValidBitsPerSample,2,1,pFile);     /* 2 Number of valid bits  */
                fread(&pWh->dwChannelMask,4,1,pFile);       /* 4 Speaker position mask  */
                fread(&pWh->SubFormat,16,1,pFile);              /* SubFormat GUID */
                #if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
                SWAP16(pWh->cbSize);
                SWAP16(pWh->wValidBitsPerSample);
                SWAP32(pWh->dwChannelMask);
                #endif  
            }
            else if (pWh->headerLen == 18 && pWh->format == 1) { /* oddball WAVE format */
                fread(&pWh->cbSize,2,1,pFile);              /* 2 Size of the extension (0 or 22) ?*/
            }
            else if (pWh->headerLen != 16 && pWh->format != 1) {
                fprintf(stderr, "Not PCM data in WAV file. headerLen = %lu, Format 0x%x\n",pWh->headerLen,pWh->format);
            }

            for (;;) {
                if (fread(&pWh->dataSig,4,1,pFile) != 1)
                    break;
                if (fread(&pWh->dataLen,4,1,pFile) != 1)
                    break;
        #if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
                SWAP32(pWh->dataSig);
                SWAP32(pWh->dataLen);
        #endif
                /* looking for 'data' chunk */
                if (pWh->dataSig == 0x61746164) {
                    break;
                }
                else if (fseek(pFile, pWh->dataLen, SEEK_CUR))
                {
                    break;
                }
            }
            if (pWh->dataSig != 0x61746164) {       
                fprintf(stderr, "No 'data' chunk found in WAV file.\n");
                return 1;
            }
            if (pWh->channels > 2) {        
                fprintf(stderr, "Invalid number of channels (%u) specified.\n",pWh->channels);
                return 1;
            }
        }

        return 0;
}

/*******************************************************************/
/*******************************************************************/
static void data_callback_isr(void *pParam1, int param2)
{
    /*
    printf("Data callback - channel 0x%08x\n", (unsigned)pParam1);
    */
    pParam1=pParam1;    /*unused*/
    BKNI_SetEvent_isr((BKNI_EventHandle)param2);
}

/*******************************************************************/
/*******************************************************************/
int app_main(int argc, char **argv)
{
    int             iErr;

	unsigned playback_sampleRate 	= 48000;
	unsigned playback_bitsPerSample = 16;
	unsigned playback_isStereo	 	= true;

	BSystem_Info    sysInfo;
    BFramework_Info frmInfo;
    BERR_Code       errCode;
    BAPE_Settings   apeSettings;
    BAPE_Handle            ape;
    BAPE_PlaybackHandle    playback;
    BAPE_DacHandle         dac;
    BAPE_MixerHandle       dacMixer;
    BAPE_SpdifOutputHandle spdif;
    BAPE_MixerHandle       spdifMixer;
    BAPE_Connector         connector;
    BAPE_OutputPort        outputPort;
    BAPE_BufferDescriptor  bufferDescriptor;
    BKNI_EventHandle       event;
    BAPE_PlaybackInterruptHandlers interrupts;
    BAPE_PlaybackStartSettings settings;
    size_t bytesToPlay = 48000*4*20;    /* 48 kHz, 4 bytes/sample, 20 seconds */
    size_t bytesPlayed=0;
    size_t offset=0;
    int16_t *pBuffer;
    void *pCached;
    size_t bufferSize;
    FILE *pFile = NULL;

    APETEST_HDMITX_Settings     hdmitxSettings;
    BAPE_MaiOutputHandle        maiOutput;
    BAPE_MixerHandle            maiMixer;

    if (argc > 1) 
    {
        pFile = fopen(argv[1], "rb");
        if ( NULL == pFile )
        {
            fprintf(stderr, "Unable to open '%s' for reading\n", argv[1]);
            return -1;
        }
    }
    
	if ( argc > 2 )	 playback_sampleRate 	= atoi(argv[2]); 
	if ( argc > 3 )  playback_bitsPerSample = atoi(argv[3]);
	if ( argc > 4 )  playback_isStereo 		= atoi(argv[4]);

	    
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
    APETEST_InitModuleDebug();

    BKNI_CreateEvent(&event);
    
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
    errCode = BAPE_Playback_Open(ape, 0, NULL, &playback);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    /* Open DAC and mixer */
    errCode = BAPE_Dac_Open(ape, 0, NULL, &dac);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = BAPE_Mixer_Create(ape, NULL, &dacMixer); 
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Open SPDIF and mixer */
    errCode = BAPE_SpdifOutput_Open(ape, 0, NULL, &spdif);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = BAPE_Mixer_Create(ape, NULL, &spdifMixer); 
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Add playback as input to each mixer */
    BAPE_Playback_GetConnector(playback, &connector);
    errCode = BAPE_Mixer_AddInput(dacMixer, connector, false);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = BAPE_Mixer_AddInput(spdifMixer, connector, false);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Link DAC to mixer1 */
    BAPE_Dac_GetOutputPort(dac, &outputPort);
    errCode = BAPE_Mixer_AddOutput(dacMixer, outputPort);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Link SPDIF to mixer2 */
    BAPE_SpdifOutput_GetOutputPort(spdif, &outputPort);
    errCode = BAPE_Mixer_AddOutput(spdifMixer, outputPort);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Open MAI and mixer */
    errCode = BAPE_MaiOutput_Open(ape, 0, NULL, &maiOutput);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Init HDMITX */
    APETEST_HDMITX_GetDefaultSettings(&hdmitxSettings);
    hdmitxSettings.channelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e2Channels;
    hdmitxSettings.speakerAllocation = 0x3;
    errCode = APETEST_HDMITX_Init(&frmInfo, &hdmitxSettings, maiOutput);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Link MAI to mixer3 */
    errCode = BAPE_Mixer_Create(ape, NULL, &maiMixer);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = BAPE_Mixer_AddInput(maiMixer, connector, false);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_MaiOutput_GetOutputPort(maiOutput, &outputPort);
    errCode = BAPE_Mixer_AddOutput(maiMixer, outputPort);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Register Playback Interrupts */
    BAPE_Playback_GetInterruptHandlers(playback, &interrupts);
    interrupts.watermark.pCallback_isr = data_callback_isr;
    interrupts.watermark.pParam1 = playback;
    interrupts.watermark.param2 = (int)event;
    errCode = BAPE_Playback_SetInterruptHandlers(playback, &interrupts);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Setup Playback Stream Parameters */
    BAPE_Playback_GetDefaultStartSettings(&settings);

	settings.sampleRate 	= playback_sampleRate;
	settings.bitsPerSample 	= playback_bitsPerSample;
	settings.isStereo 		= playback_isStereo;
	settings.isSigned 		= true;

    /* If we have a wav file, get the sample rate from it */
    if ( pFile )
    {
        wave_header wh;

        errCode = extract_wave_header( pFile, &wh);

        settings.sampleRate = wh.samplesSec;
        settings.bitsPerSample = wh.bps;
        settings.isStereo = (wh.channels>1);
        settings.isSigned = (wh.bps != 8);    /* 8-bit .wav files are unsigned */
    }

    /* Start Playback */
    errCode = BAPE_Playback_Start(playback, &settings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( pFile )
    {
        printf("Starting playback of file '%s'\n", argv[1]);
    }
    else
    {
        printf("Starting playback for 20 seconds.\n");
    }

    do
    {
        unsigned i;

        /* Check available buffer space */
        errCode = BAPE_Playback_GetBuffer(playback, &bufferDescriptor);
        if ( errCode )
        {
            printf("Error getting playback buffer\n");
            break;
        }

        /* We always use interleaved stereo, so take the first buffer pointer */
        (void)BMEM_ConvertAddressToCached(frmInfo.hMem, bufferDescriptor.buffers[0].pBuffer, &pCached);
        pBuffer = pCached;
        bufferSize = bufferDescriptor.bufferSize;

        if (bufferSize) 
        {
            if ( pFile ) 
            {
                bufferSize = fread(pBuffer, 1, bufferSize, pFile);
                if ( 0 == bufferSize )
                {
                    break;
                }
            }
            else 
            {
                /* Copy samples into buffer */
                bufferSize /= 4;
                for ( i=0; i<bufferSize; i++,bytesPlayed+=4 )
                {
                    pBuffer[2*i] = pBuffer[(2*i)+1] = samples[offset];
                    offset++;
                    if ( offset >= 48 )
                    {
                        offset = 0;
                    }
                }
                bufferSize *= 4;
            }

            (void)BMEM_FlushCache(frmInfo.hMem, pCached, bufferSize);
            errCode = BAPE_Playback_CommitData(playback, bufferSize);
            if ( errCode )
            {
                printf("Error committing playback buffer\n");
                break;
            }
        }
        else 
        {
            /* Wait for data callback */
            errCode = BKNI_WaitForEvent(event, 5000);
        }
    } while ( BERR_SUCCESS == errCode && bytesPlayed < bytesToPlay );

    printf("Waiting for buffer to empty\n");
    for ( ;; )
    {
        BAPE_PlaybackStatus status;
        BAPE_Playback_GetStatus(playback, &status);
        if ( status.queuedBytes > 0 )
        {
            BKNI_Sleep(100);
        }
        else
        {
            break;
        }
    }

    printf("Stopping playback\n");
    BAPE_Playback_Stop(playback);

    /* Teardown Connections */
    BAPE_Mixer_RemoveAllInputs(dacMixer);
    BAPE_Mixer_RemoveAllOutputs(dacMixer);
    BAPE_Mixer_RemoveAllInputs(spdifMixer);
    BAPE_Mixer_RemoveAllOutputs(spdifMixer);
    /* Close Resource Handles */
    BAPE_Mixer_Destroy(dacMixer);
    BAPE_Mixer_Destroy(spdifMixer);
    BAPE_Mixer_Destroy(maiMixer);
    BAPE_Dac_Close(dac);
    BAPE_SpdifOutput_Close(spdif);
    APETEST_HDMITX_Uninit();
    BAPE_MaiOutput_Close(maiOutput);
    BAPE_Playback_Close(playback);

    /* Shutdown system */
    BAPE_Close(ape);

    BFramework_Uninit(&frmInfo);
    BSystem_Uninit(&sysInfo);

    printf("Done\n");
    return 0;
}

