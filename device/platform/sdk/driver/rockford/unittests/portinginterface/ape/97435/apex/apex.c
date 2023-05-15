/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apex.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 8/30/12 4:14p $
 *
 * Module Description: APE Exerciser Unit Test Program
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/apex/apex.c $
 * 
 * Hydra_Software_Devel/12   8/30/12 4:14p jgarrett
 * SW7425-3629: Converting to cached memory accesses
 * 
 * Hydra_Software_Devel/11   4/2/12 2:21p gskerl
 * SW7425-1178: Reimplemented pass/fail criteria to allow for DSP padding
 * samples at beginning and end of capture.
 * 
 * Hydra_Software_Devel/10   3/29/12 5:42p gskerl
 * SW7425-1178: Wait for Playback buffer to empty instead of complaining
 * if it's not.
 * 
 * Hydra_Software_Devel/9   3/20/12 6:33p gskerl
 * SW7425-1178: Adjusted placement of calls to BMEM_Heap_Validate to make
 * them less time-sensitive.
 * 
 * Hydra_Software_Devel/9   3/20/12 6:29p gskerl
 * SW7425-1178: Adjusted placement of calls to BMEM_Heap_Validate to make
 * them less time-sensitive.
 * 
 * Hydra_Software_Devel/8   3/19/12 2:05p gskerl
 * SW7425-1178: Added calls to BMEM_Heap_Validate in order to assert on
 * heap corruption.
 * 
 * Hydra_Software_Devel/7   10/12/11 1:57p gskerl
 * SW7425-1178: Added I2S output to apex test program
 * 
 * Hydra_Software_Devel/6   9/26/11 2:28p gskerl
 * SW7425-1178: Refactored. Removed wav file playback. Added separate
 * playback and capture events.
 * 
 * Hydra_Software_Devel/5   9/21/11 10:15p gskerl
 * SW7425-1178: Added code to stop complaining after 100 mismatches.
 * 
 * Hydra_Software_Devel/4   9/21/11 9:07p gskerl
 * SW7425-1178: Added code to indicate pass/fail via exit code for
 * Electric Commander.
 * 
 * Hydra_Software_Devel/4   9/21/11 9:06p gskerl
 * SW7425-1178: Added code to indicate pass/fail via exit code for
 * Electric Commander.
 * 
 * Hydra_Software_Devel/3   9/15/11 4:01p gskerl
 * SW7425-1178: Streamlined the periodic print statement under unlocked
 * condition
 * 
 * Hydra_Software_Devel/2   9/2/11 2:47p gskerl
 * SW7425-1178: Added code to verify captured output data against original
 * playback source data.
 * 
 * 
 ***************************************************************************/


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

/* DSP includes */
#include "bdsp.h"
#include "bdsp_raaga.h"

/* APE includes */
#include "bape.h"

#include "bkni_event_group.h"

BDBG_MODULE(apex);


/* #define BAPE_Heap_Validate(x)  BMEM_Heap_Validate(x) */
#define BAPE_Heap_Validate(x)  (BERR_SUCCESS)


/* 1KHz sine wave at 48 KHz (0dBFS 16-bit/sample) */
static int16_t samples[48] =
{
0, 4276, 8480, 12539, 16383, 19947, 23169, 25995, 28377, 30272, 31650, 32486, 32767, 32486, 31650, 30272, 28377, 25995, 23169, 19947, 16383, 12539, 8480, 4276,
0,-4277,-8481,-12540,-16384,-19948,-23170,-25996,-28378,-30273,-31651,-32487,-32767,-32487,-31651,-30273,-28378,-25996,-23170,-19948,-16384,-12540,-8481,-4277
};

enum eCaptureVerifyState
{
    eCaptureVerfiyStateUnlocked,         /* Totally lost... looking for the first sample that matches the reference. */
    eCaptureVerfiyStateLocking,          /* Found at least one match with the reference, looking for more.  */
    eCaptureVerfiyStateLocked,           /* Found enough consecutive matches to decide that we're in sync with the reference. */
    eCaptureVerfiyStateUnlocking         /* Found at least one mismatch while in Locked state... too many will take us to Unlocked. */
};

char *captureVerifyStateName[] =         /* String descriptions for each of the eState values. */
{
    "Unlocked",
    "Locking",
    "Locked",
    "Unlocking"
};

typedef struct CaptureVerifyContext
{
    enum eCaptureVerifyState    state;          /* Unlocked, Locking, Locked, Unlocking  */
    unsigned                    refIndex;       /* Index into reference buffer of last sample. */
    bool                        channel;        /* Which channel of a channel-pair (0, 1) */

    unsigned                    matches;        /* Counts consecutive matches during Locking state. */
    unsigned                    mismatches;     /* Counts consecutive mismatches during Unlocking state. */

    unsigned long               totalMatches;
    unsigned long               totalMismatches;
    unsigned long               totalSamples;   /* Total number of sampled processed */

    unsigned long               samplesSinceLastLock;
    unsigned long               mismatchesSinceLastLock;

    unsigned long               samplesInLongestLockPeriod;
    unsigned long               mismatchesInLongestLockPeriod;
} CaptureVerifyContext;


/*******************************************************************/
/*******************************************************************/
void verifyBuffer(CaptureVerifyContext *pCtx, void *pBuffer, unsigned bufferSize)
{
    uint32_t    *pSamples = (uint32_t *)pBuffer;                /* Convert buffer to array of 32-bit samples */
    unsigned    sampleCount = bufferSize / sizeof *pSamples;    /* Convert bufferSize to number of samples */
    unsigned    sampleIdx;                                      /* Index into pSamples */

    int16_t    *refSamples = samples;                                   /* pointer to 16-bit reference sample array */
    unsigned    refSampleCount = sizeof samples / sizeof samples[0];    /* number of reference sample array elements */

    const int       scaleFactor = 2;
    const unsigned  lockSampleCount   = 10;                      /* Need this many consecutive matches to lock */
    const unsigned  unlockSampleCount = 10;                      /* Need this many consecutive mismatches to unlock */
    const unsigned  totalMismatchLimit = 100;


    /* Entry Point: */


    /* If first time, print a header message. */
    if (pCtx->totalSamples == 0) printf("At sample 0 State: %s  Looking for match...\n", captureVerifyStateName[pCtx->state] );

    /* Loop through each sample in the caller's buffer. */
    for (sampleIdx=0 ; sampleIdx<sampleCount ; sampleIdx++)
    {
        uint32_t  mySample;     /* The current sample... includes channel and PCM value  */
        int16_t   myValue;      /* Value portion of mySample */
        bool      myChannel;    /* Channel portion of my Sample */

        if (pCtx->totalMismatches == totalMismatchLimit)
        {
            printf("Reached mismatch limit of %lu... Giving up!\n", pCtx->totalMismatches);
            pCtx->totalMismatches++;         /* This is so we don't keep printing this message. */
        }

        if (pCtx->totalMismatches >= totalMismatchLimit) break;

        mySample = pSamples[sampleIdx];
        myValue = (int16_t)(mySample >> 16);
        /* myChannel = (mySample & 0x80) ? 0 : 1; */
        myChannel = sampleIdx % 2;

        pCtx->totalSamples++;
        pCtx->samplesSinceLastLock++;

        /* Unlocked State: Look through the reference samples for a match with the current sample. */
        if (pCtx->state == eCaptureVerfiyStateUnlocked)
        {
            if (myValue != 0)            /* Ignore null samples when we're unlocked */
            {
                unsigned  i;
                static unsigned   samplesIdx = 0;
                for (i=0 ; i<refSampleCount ; i++)
                {
                    if (refSamples[samplesIdx]/scaleFactor == myValue)
                    {
                        printf("At sample %lu %s -> %s: Found match at refIndex:%d\n", pCtx->totalSamples, captureVerifyStateName[pCtx->state], captureVerifyStateName[eCaptureVerfiyStateLocking], samplesIdx);
                        pCtx->refIndex = samplesIdx;
                        pCtx->channel = myChannel;
                        pCtx->state = eCaptureVerfiyStateLocking;
                        pCtx->matches = 0;
                        break;
                    }
                    samplesIdx = (samplesIdx + 1) % refSampleCount;
                }
            }
        }
        /* Partially Locked States (Locking, Locked, Unlocking) */
        else 
        {
            bool match = true;

            /* Figure out the next expected sample */
            pCtx->channel = (pCtx->channel+1) % 2;                      /* Toggle channel */
            if (pCtx->channel == 0) pCtx->refIndex++;                   /* Channel wrapped, get index to next sample */
            if (pCtx->refIndex >= refSampleCount) pCtx->refIndex = 0;   /* Check for index wraparound */

            /* Check this sample's value against the reference array. */
            if (myValue != refSamples[pCtx->refIndex] / scaleFactor)
            {
                printf("Value mismatch: expected: %d  got %d\n", refSamples[pCtx->refIndex]/scaleFactor, myValue);
                match = false;
            }
            /* Check this samples channel indicator. */
            if (myChannel != pCtx->channel)
            {
                printf("Channel mismatch: expected: %d  got %d\n", pCtx->channel, myChannel);
                match = false;
            }

            /* Handle Locking state... */
            if (pCtx->state == eCaptureVerfiyStateLocking)
            {
                if (match)
                {
                    pCtx->matches++;
                    if (pCtx->matches > lockSampleCount)
                    {
                        printf("At sample %lu %s -> %s: Found match at refIndex:%d\n", pCtx->totalSamples, captureVerifyStateName[pCtx->state], captureVerifyStateName[eCaptureVerfiyStateLocked], pCtx->refIndex);
                        pCtx->state = eCaptureVerfiyStateLocked;
                        pCtx->mismatchesSinceLastLock = 0;
                        pCtx->samplesSinceLastLock = 0;
                    }
                }
                else
                {    
                    printf("At sample %lu %s -> %s: Found mismatch at refIndex:%d\n",
                            pCtx->totalSamples, captureVerifyStateName[pCtx->state], captureVerifyStateName[eCaptureVerfiyStateUnlocked], pCtx->refIndex);
                    pCtx->state = eCaptureVerfiyStateUnlocked;
                }
            }

            /* Handle Locked state... */
            else if (pCtx->state == eCaptureVerfiyStateLocked)
            {
                if (match)
                {
                    pCtx->totalMatches++;
                }
                else
                {    
                    printf("At sample %lu %s -> %s: Found mismatch at refIndex:%d\n", pCtx->totalSamples, captureVerifyStateName[pCtx->state], captureVerifyStateName[eCaptureVerfiyStateUnlocking], pCtx->refIndex);
                    pCtx->state = eCaptureVerfiyStateUnlocking;
                    pCtx->mismatches = 0;
                }
            }

            /* Handle Unlocking state... */
            else if (pCtx->state == eCaptureVerfiyStateUnlocking)
            {
                if (match)
                {
                    pCtx->totalMatches++;
                    printf("At sample %lu %s -> %s: Found match at refIndex:%d\n",
                            pCtx->totalSamples, captureVerifyStateName[pCtx->state], captureVerifyStateName[eCaptureVerfiyStateLocked], pCtx->refIndex);

                    pCtx->totalMismatches += pCtx->mismatches;  /* Only count mismatches if we're staying locked. */
                    pCtx->mismatchesSinceLastLock += pCtx->mismatches;

                    pCtx->state = eCaptureVerfiyStateLocked;
                }
                else
                {    
                    pCtx->mismatches++;
                    if (pCtx->mismatches > unlockSampleCount)
                    {
                        printf("At sample %lu %s -> %s: Found mismatch at refIndex:%d\n", pCtx->totalSamples, captureVerifyStateName[pCtx->state], captureVerifyStateName[eCaptureVerfiyStateUnlocked], pCtx->refIndex);
                        pCtx->state = eCaptureVerfiyStateUnlocked;
                    }
                }
            }
        }  /* End if: Partially locked states */

        if (pCtx->state == eCaptureVerfiyStateLocked || pCtx->state == eCaptureVerfiyStateUnlocking)
        {
            if (pCtx->samplesSinceLastLock > pCtx->samplesInLongestLockPeriod)
            {
                pCtx->samplesInLongestLockPeriod    = pCtx->samplesSinceLastLock;
                pCtx->mismatchesInLongestLockPeriod = pCtx->mismatchesSinceLastLock;
            }
        }
        if (pCtx->totalSamples % 100000 == 0)
        {
            if (pCtx->state == eCaptureVerfiyStateLocked || pCtx->state == eCaptureVerfiyStateUnlocking)
            {
                if (pCtx->samplesSinceLastLock == 0) pCtx->samplesSinceLastLock++;  /* Prevent divide-by-zero */
    
                printf("Sample %luK  State: %s   Since last lock at %luK samples: total:%luK mismatches:%lu (%lu PPM)\n", 
                       pCtx->totalSamples/1000,                               /* Sample %luK */
                       captureVerifyStateName[pCtx->state],                   /* State:%s */
                       (pCtx->totalSamples - pCtx->samplesSinceLastLock)/1000,/* Since last lock at %luK samples: */
                       pCtx->samplesSinceLastLock/1000,                       /* total:%luK */
                       pCtx->mismatchesSinceLastLock,                         /* mismatches:%lu */
                       ((pCtx->mismatchesSinceLastLock * 1000000) + (pCtx->samplesSinceLastLock/2)) / pCtx->samplesSinceLastLock );
            }
            else
            {
                printf("Sample %luK  State: %s \n", 
                       pCtx->totalSamples/1000,                               /* Sample %luK */
                       captureVerifyStateName[pCtx->state] );                 /* State:%s */
            }
        } 

    } /* end: for each sample in caller's buffer */

    return;
}

/*******************************************************************/
/*******************************************************************/
static void data_callback_isr(void *pParam1, int param2)
{
    /*
    printf("Playback Data callback - channel 0x%08x\n", (unsigned)pParam1);
    */
    pParam1=pParam1;    /*unused*/
    BKNI_SetEvent_isr((BKNI_EventHandle)param2);
}

/*******************************************************************/
/*******************************************************************/
static void output_capture_callback_isr(void *pParam1, int param2)
{
    /*
    printf("Capture Data callback - channel 0x%08x\n", (unsigned)pParam1);
    */
    pParam1=pParam1;    /*unused*/
    BKNI_SetEvent_isr((BKNI_EventHandle)param2);
}

/*******************************************************************/
/*******************************************************************/
int app_main(int argc, char **argv)
{
    int             iErr;
    BSystem_Info    sysInfo;
    BFramework_Info frmInfo;

	unsigned playback_sampleRate 	= 48000;
	unsigned playback_bitsPerSample = 16;
    unsigned playback_sampleSize    = 2;    /* Each playback sample is 16-bit, either left or right. */
	unsigned playback_isStereo	 	= true;
    unsigned playback_duration      = 6;    /* in seconds */
    unsigned capture_sampleSize     = 4;    /* Each capture sample is 32-bit, either left of right. */
    BERR_Code       errCode;

    BDSP_RaagaSettings raagaSettings;
    BDSP_Handle        dsp;

    BAPE_Settings      apeSettings;
    BAPE_Handle        ape;

    BAPE_DacHandle     dac;

    BAPE_OutputCaptureHandle             outputCapture;
    BAPE_OutputCaptureInterruptHandlers  outputCaptureInterrupts;

    BAPE_MixerHandle       dacMixer;
    BAPE_OutputPort        outputPort;

    BAPE_SpdifOutputHandle spdif;

    BAPE_I2sOutputHandle  i2sOut;

    BAPE_MixerHandle       spdifMixer;

    BAPE_MixerSettings          mixerSettings;
    BAPE_MixerAddInputSettings  mixerAddInputSettings;
    BAPE_MixerHandle            dspMixer;

    BAPE_PlaybackHandle             playback;
    BAPE_PlaybackInterruptHandlers  interrupts;
    BAPE_PlaybackStartSettings      playbackSettings;

    BAPE_Connector         connector;
    BAPE_BufferDescriptor  bufferDescriptor;
    BKNI_EventGroupHandle  eventGroup;
    BKNI_EventHandle       captureEvent;
    BKNI_EventHandle       playbackEvent;

    CaptureVerifyContext    vfyCtx;

    size_t samplesToPlay;

    int testCycle;
    int16_t *pBuffer;
    size_t bufferSize;

    if ( argc > 1 ) playback_sampleRate    = atoi(argv[1]); 
    if ( argc > 2 ) playback_bitsPerSample = atoi(argv[2]);
    if ( argc > 3 ) playback_isStereo      = atoi(argv[3]);
	    
    samplesToPlay = playback_sampleRate * playback_duration * 2;    /* samples/second * seconds * 2 channels = samples */

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

    BKNI_Sleep(500);  /* Workaround so that AppFramework has time to get interrupts working. */

    errCode = BDBG_SetModuleLevel("bape_startprint", BDBG_eMsg);  /* Prints audio config when nodes are started */
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BKNI_CreateEventGroup(&eventGroup);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BKNI_CreateEvent(&playbackEvent);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BKNI_AddEventGroup(eventGroup, playbackEvent);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BKNI_CreateEvent(&captureEvent);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BKNI_AddEventGroup(eventGroup, captureEvent);
    if ( errCode ) return BERR_TRACE(errCode);


    printf("Opening BDSP_Raaga...\n");
    BDSP_Raaga_GetDefaultSettings(&raagaSettings);

    errCode = BDSP_Raaga_Open(
        &dsp,
        frmInfo.hChp,
        frmInfo.hReg,
        frmInfo.hMem,
        frmInfo.hInt,
        frmInfo.hTmr,
        &raagaSettings);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Open APE PI */
    printf("Opening APE...\n");
    BAPE_GetDefaultSettings(&apeSettings);

    apeSettings.rampPcmSamples = false;
    errCode = BAPE_Open(&ape, 
                        frmInfo.hChp, 
                        frmInfo.hReg, 
                        frmInfo.hMem, 
                        frmInfo.hInt, 
                        frmInfo.hTmr, 
                        dsp,
                        &apeSettings);

    /* Create the first mixer (for the DAC and OutputCapture outputs) */
    printf("Creating dacMixer...\n");
    errCode = BAPE_Mixer_Create(ape, NULL, &dacMixer); 
    if ( errCode ) return BERR_TRACE(errCode);


    /* Open DAC and connect it to the dacMixer */
    printf("Opening DAC output...\n");
    errCode = BAPE_Dac_Open(ape, 0, NULL, &dac);
    if ( errCode ) return BERR_TRACE(errCode);

    BAPE_Dac_GetOutputPort(dac, &outputPort);

    printf("Adding DAC output to dacMixer...\n");
    errCode = BAPE_Mixer_AddOutput(dacMixer, outputPort);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Open OutputCapture and connect it to the dacMixer */
    printf("Opening OutputCapture...\n");
    errCode = BAPE_OutputCapture_Open(ape, 0, NULL, &outputCapture);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Register the OutputCapture interrupts */
    BAPE_OutputCapture_GetInterruptHandlers(outputCapture, &outputCaptureInterrupts);
    outputCaptureInterrupts.watermark.pCallback_isr = output_capture_callback_isr;
    outputCaptureInterrupts.watermark.pParam1 = outputCapture;
    outputCaptureInterrupts.watermark.param2 = (int)captureEvent;
    errCode = BAPE_OutputCapture_SetInterruptHandlers(outputCapture, &outputCaptureInterrupts);
    if ( errCode ) return BERR_TRACE(errCode);

    BAPE_OutputCapture_GetOutputPort(outputCapture, &outputPort);

    printf("Adding OutputCapture to dacMixer...\n");
    errCode = BAPE_Mixer_AddOutput(dacMixer, outputPort);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Create the second mixer (for SPDIF output) */
    printf("Creating spdifMixer...\n");
    errCode = BAPE_Mixer_Create(ape, NULL, &spdifMixer); 
    if ( errCode ) return BERR_TRACE(errCode);

    /* Open SPDIF and connect it to the spdifMixer */
    printf("Opening SPDIF output...\n");
    errCode = BAPE_SpdifOutput_Open(ape, 0, NULL, &spdif);
    if ( errCode ) return BERR_TRACE(errCode);

    BAPE_SpdifOutput_GetOutputPort(spdif, &outputPort);

    printf("Adding SPDIF output to spdifMixer...\n");
    errCode = BAPE_Mixer_AddOutput(spdifMixer, outputPort);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Open I2S and connect it to the spdifMixer */
    printf("Opening I2S output...\n");
    errCode = BAPE_I2sOutput_Open(ape, 0, NULL, &i2sOut);
    if ( errCode ) return BERR_TRACE(errCode);

    BAPE_I2sOutput_GetOutputPort(i2sOut, &outputPort);

    printf("Adding I2S output to spdifMixer...\n");
    errCode = BAPE_Mixer_AddOutput(spdifMixer, outputPort);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Create a firmware mixer to feed the dacMixer and spdifMixer */
    printf("Creating dspMixer...\n");
    BAPE_Mixer_GetDefaultSettings( &mixerSettings );
    mixerSettings.type = BAPE_MixerType_eDsp;

    errCode = BAPE_Mixer_Create(ape, &mixerSettings, &dspMixer); 
    if ( errCode ) return BERR_TRACE(errCode);

    BAPE_Mixer_GetConnector( dspMixer, &connector );

    BAPE_Mixer_GetDefaultAddInputSettings(&mixerAddInputSettings);
    mixerAddInputSettings.sampleRateMaster = true;

    printf("Adding dspMixer output to dacMixer input...\n");
    errCode = BAPE_Mixer_AddInput(dacMixer, connector, &mixerAddInputSettings);
    if ( errCode ) return BERR_TRACE(errCode);

    printf("Adding dspMixer output to spdifMixer input...\n");
    errCode = BAPE_Mixer_AddInput(spdifMixer, connector, &mixerAddInputSettings);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Open Playback Channel */
    printf("Opening Playback...\n");
    errCode = BAPE_Playback_Open(ape, 0, NULL, &playback);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Register Playback Interrupts */
    BAPE_Playback_GetInterruptHandlers(playback, &interrupts);
    interrupts.watermark.pCallback_isr = data_callback_isr;
    interrupts.watermark.pParam1 = playback;
    interrupts.watermark.param2 = (int)playbackEvent;
    errCode = BAPE_Playback_SetInterruptHandlers(playback, &interrupts);
    if ( errCode ) return BERR_TRACE(errCode);

    /* Add playback as input to each mixer */
    BAPE_Playback_GetConnector(playback, &connector);

    printf("Adding Playback to dspMixer...\n");
    BAPE_Mixer_GetDefaultAddInputSettings(&mixerAddInputSettings);
    mixerAddInputSettings.convertSampleRate = true;

    errCode = BAPE_Mixer_AddInput(dspMixer, connector, NULL);
    if ( errCode ) return BERR_TRACE(errCode);

  /* Setup Playback Stream Parameters */
    BAPE_Playback_GetDefaultStartSettings(&playbackSettings);

    playbackSettings.sampleRate 	= playback_sampleRate;
    playbackSettings.bitsPerSample 	= playback_bitsPerSample;
    playbackSettings.isStereo 		= playback_isStereo;
    playbackSettings.isSigned 		= true;

    /* Check for heap corrpution. */
    BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

    /***************************************************************************** 
     *  Loop through a number of test cycles.  Each cycle will play samples,
     *  capture them back, and compare the captured samples with the originals.
     *  Also, between each iteration, APE will be put into "standby" for a while,
     *  then resumed.
     *****************************************************************************/
    for (testCycle=0 ; testCycle<5 ; testCycle++)
    {

        size_t samplesPlayed=0;
        size_t samplesCaptured=0;
        size_t offset=0;

        /* Check for heap corrpution. */
        BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

        /***************************************************************************** 
         *  Do the APE Standby/Resume between each test cycle.  But don't bother
         *  before the first cycle.
         *****************************************************************************/
        if (testCycle > 0)
        {
            BDBG_LOG(("Calling BAPE_Standby()..."));
            errCode = BAPE_Standby(ape, NULL);
            if ( errCode ) return BERR_TRACE(errCode);

            /* Check for heap corrpution. */
            BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

            BDBG_LOG(("Sleeping..."));
            BKNI_Sleep(2000);

            BDBG_LOG(("Calling BAPE_Resume()..."));
            errCode = BAPE_Resume(ape);
            if ( errCode ) return BERR_TRACE(errCode);

            /* Check for heap corrpution. */
            BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

        }

        /***************************************************************************** 
         *  Start Playback
         *****************************************************************************/
        printf("Starting playback..\n");
        errCode = BAPE_Playback_Start(playback, &playbackSettings);
        if ( errCode ) return BERR_TRACE(errCode);

        BKNI_Memset(&vfyCtx, 0, sizeof vfyCtx);     /* Reset the verify context buffer. */

        /* Check for heap corrpution. */
        BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

        /***************************************************************************** 
         *  Now go into this loop, continuously filling the playback buffer when
         *  there is room, and verifying any captured data that arrives.
         *  We'll exit this loop when we've captured all of the samples that we've
         *  put in.
         *****************************************************************************/
        do
        {
            unsigned i;
            bool     relook  = false;
            BKNI_EventHandle  wakeEvent;
            unsigned  wakeEventCount;
            void *pCached;

            /***************************************************************************** 
             *  If we got the Playback event, then provide playback samples as needed.
             *****************************************************************************/
            if (wakeEvent == playbackEvent && samplesPlayed < samplesToPlay)
            {
                /* Check available playback buffer space */
                errCode = BAPE_Playback_GetBuffer(playback, &bufferDescriptor);
                if ( errCode )
                {
                    printf("Error getting playback buffer\n");
                    return BERR_TRACE(errCode);
                }

                /* We always use interleaved stereo, so take the first buffer pointer */
                (void)BMEM_ConvertAddressToCached(frmInfo.hMem, bufferDescriptor.buffers[0].pBuffer, &pCached);
                pBuffer = pCached;
                bufferSize = bufferDescriptor.bufferSize;
                if ((samplesToPlay - samplesPlayed) * playback_sampleSize < bufferSize)
                {
                    bufferSize = (samplesToPlay - samplesPlayed) * playback_sampleSize;
                }

                if (bufferSize) 
                {
                    relook = true;  /* Need to check again for more buffers */

                    /* Copy samples into buffer */
                    bufferSize /= (playback_sampleSize * 2);            /* bufferSize is now in L+R sample-pairs. */
                    for ( i=0 ; i<bufferSize ; i++,samplesPlayed+=2 )           /* Loop for each sample-pair. */
                    {
                        pBuffer[2*i] = pBuffer[(2*i)+1] = samples[offset] / 2;  /* Add a pair of left and right samples. */
                        offset++;
                        if ( offset >= sizeof samples / sizeof samples[0] )
                        {
                            offset = 0;
                        }
                    }
                    bufferSize *= 4;            /* bufferSize is now back to bytes. */

                    (void)BMEM_FlushCache(frmInfo.hMem, pCached, bufferSize);
                    errCode = BAPE_Playback_CommitData(playback, bufferSize);
                    if ( errCode )
                    {
                        printf("Error committing playback buffer\n");
                        return BERR_TRACE(errCode);
                    }
                }
            }

           /***************************************************************************** 
             *  If we got the Capture event, then analyze the capture data.
             *****************************************************************************/
            if (wakeEvent == captureEvent )
            {

                /***************************************************************************** 
                 *  Check for captured output samples, verify that they're correct.
                 *****************************************************************************/
                errCode = BAPE_OutputCapture_GetBuffer(outputCapture, &bufferDescriptor);
                if ( errCode )
                {
                    printf("Error getting output capture buffer\n");
                    return BERR_TRACE(errCode);
                }

                /* We always use interleaved stereo, so take the first buffer pointer */
                (void)BMEM_ConvertAddressToCached(frmInfo.hMem, bufferDescriptor.buffers[0].pBuffer, &pCached);
                pBuffer = pCached;
                bufferSize = bufferDescriptor.bufferSize;

                if (bufferSize) 
                {
                    (void)BMEM_FlushCache(frmInfo.hMem, pCached, bufferSize);
                    samplesCaptured += bufferSize/capture_sampleSize;
                    relook = true;  /* Need to check again for more buffers */

                    verifyBuffer(&vfyCtx, pBuffer, bufferDescriptor.bufferSize);

                    errCode = BAPE_OutputCapture_ConsumeData(outputCapture, bufferSize);
                    if ( errCode )
                    {
                        printf("Error consuming output capture buffer\n");
                        return BERR_TRACE(errCode);
                    }
                }
            }

            /***************************************************************************** 
             *  If there was nothing to do (relook is false) then just stop and wait
             *  for something to happen.
             *****************************************************************************/
            if (! relook)
            {
                /* Check for heap corrpution. */
                BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

                /* Wait for data callback */
                errCode = BKNI_WaitForGroup(eventGroup, 5000, &wakeEvent, 1, &wakeEventCount);
                if ( BERR_TIMEOUT != errCode && BERR_SUCCESS != errCode)
                {
                    return BERR_TRACE(errCode);
                }
            }
        } while ( samplesPlayed < samplesToPlay || samplesCaptured < (samplesToPlay * 110) / 100 );

        /***************************************************************************** 
         *  Wait for the the playback buffer to empty.
         *****************************************************************************/
        printf("Waiting for Playback buffer to empty\n" );

        {
            unsigned msTotal = 0;  /* Total milliseconds wait so far. */
            unsigned msIncr  = 100;  /* Number of milliseconds between checks. */

            for (;;)
            {
                BAPE_PlaybackStatus status;
                BAPE_Playback_GetStatus(playback, &status);
                if ( status.queuedBytes == 0 )    break;  /* Normal successful loop exit. */

                BKNI_Sleep(msIncr);
                msTotal += msIncr;

                if (msTotal >= 5000)
                {
                    printf("Timed out waiting for for Playback buffer to empty.\n");
                    return BERR_TRACE(BERR_UNKNOWN);
                }
            }
        }

        /* Check for heap corrpution. */
        BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

        /***************************************************************************** 
         *  Now stop the playback.
         *****************************************************************************/
        printf("Stopping playback\n");
        BAPE_Playback_Stop(playback);

        printf("Flushing output capture\n");
        BAPE_OutputCapture_Flush(outputCapture);

        /* Check for heap corrpution. */
        BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

        /***************************************************************************** 
         *  Now check some of the verification results to decide whether we passed
         *  the test or not.
         *****************************************************************************/
        {
            const unsigned long   totalSamplesMin = samplesToPlay;
            const unsigned long   longestLockSamplesMin = (samplesToPlay * 90) / 100;
            const unsigned long   longestLockMismatchesMax = 0;
            bool wePassed;
            bool testFailed = false;

            printf("------------------------------------------------------------------------\n");
            printf("Capture Verification Summary:\n");

            wePassed = (vfyCtx.totalSamples >= totalSamplesMin) ? true : false;
            if (!wePassed) testFailed = true;
            printf("    Total Samples Captured:            %8lu    (Min:%8lu)    %s\n",  vfyCtx.totalSamples, totalSamplesMin, wePassed?"PASS":"***FAIL***");

            wePassed = ((vfyCtx.samplesInLongestLockPeriod) >= longestLockSamplesMin) ? true : false;
            if (!wePassed) testFailed = true;
            printf("    Samples In Longest Lock Period:    %8lu    (Min:%8lu)    %s\n",  vfyCtx.samplesInLongestLockPeriod, longestLockSamplesMin, wePassed?"PASS":"***FAIL***");

            wePassed = (vfyCtx.mismatchesInLongestLockPeriod <= longestLockMismatchesMax) ? true : false;
            if (!wePassed) testFailed = true;
            printf("    Mismatches In Longest Lock Period: %8lu    (Max:%8lu)    %s\n",  vfyCtx.mismatchesInLongestLockPeriod, longestLockMismatchesMax, wePassed?"PASS":"***FAIL***");
            printf("------------------------------------------------------------------------\n");

            if (testFailed) {
                printf(" Test Failed !!!    Aborting.\n");
                return 1;
            }
        }
    }  /* Do for each testCycle */

    /* Check for heap corrpution. */
    BDBG_ASSERT(BAPE_Heap_Validate( frmInfo.hMem ) == BERR_SUCCESS);

    /***************************************************************************** 
     *  All test cycles are done, shut everything down.
     *****************************************************************************/
    BAPE_Mixer_Stop(dspMixer);

    /* Teardown Connections */
    BAPE_Mixer_RemoveAllInputs(dacMixer);
    BAPE_Mixer_RemoveAllOutputs(dacMixer);
    BAPE_Mixer_RemoveAllInputs(spdifMixer);
    BAPE_Mixer_RemoveAllOutputs(spdifMixer);

    /* Close Resource Handles */
    BAPE_Playback_Close(playback);

    BAPE_Mixer_Destroy(dspMixer);

    BAPE_SpdifOutput_Close(spdif);
    BAPE_Mixer_Destroy(spdifMixer);

    BAPE_OutputCapture_Close(outputCapture);
    BAPE_Dac_Close(dac);
    BAPE_Mixer_Destroy(dacMixer);

/* Shutdown system */
    BAPE_Close(ape);
    BDSP_Close( dsp );

    BKNI_RemoveEventGroup(eventGroup, captureEvent);
    BKNI_DestroyEvent(captureEvent);

    BKNI_RemoveEventGroup(eventGroup, playbackEvent);
    BKNI_DestroyEvent(playbackEvent);

    BKNI_DestroyEventGroup(eventGroup);

    BFramework_Uninit(&frmInfo);
    BSystem_Uninit(&sysInfo);

    printf("Done\n");
    return 0;
}

