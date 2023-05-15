/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test_manual.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 7/16/12 4:19p $
 *
 * Module Description:
 *  All the manual mode of testing will be part of this module.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/raaga_test_manual.c $
 * 
 * Hydra_Software_Devel/10   7/16/12 4:19p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/9   3/19/12 5:52p ramanatm
 * SW7425-2378: [7425] Add support for Monkey Audio Codec (MAC)- Add the
 * index in raaga_test as well
 * 
 * Hydra_Software_Devel/8   2/3/12 2:49p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/7   1/25/12 6:20p gprasad
 * SW7360-2: Fixing issue with streamer playback in raaga test app
 * 
 * Hydra_Software_Devel/6   1/5/12 12:47p gprasad
 * FWRAA-370: Adding changes to configure HIFIDAC RM based on sample rate
 * interrupt
 * 
 * Hydra_Software_Devel/5   11/9/11 4:22p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/4   8/1/11 5:34p gprasad
 * FWRAA-327: Add support for G.711/G.726/G.729 codecs
 * 
 * Hydra_Software_Devel/3   5/18/11 9:55p srajapur
 * SW7425-572: [7425] Adding VP6 support in raaga test app
 * 
 * Hydra_Software_Devel/2   5/16/11 10:10p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/1   1/14/11 3:25p gdata
 * SW7422-191:[7422] Adding the raaga test application
 * 
 ****************************************************************************/ 

#include "raaga_test.h"

BDBG_MODULE(raaga_test);

char * algoString [] = {
    "BDSP_AudioType_eMpeg",
    "BDSP_AudioType_eAac/AacAdts",
    "BDSP_AudioType_eAacLoas",
    "BDSP_AudioType_eAacSbrAdts",
    "BDSP_AudioType_eAacSbr/SbrLoas",
    "BDSP_AudioType_eAc3",
    "BDSP_AudioType_eAc3Plus",
    "BDSP_AudioType_eLpcmBd",
    "BDSP_AudioType_eLpcmHdDvd",
    "BDSP_AudioType_eDtshd",
    "BDSP_AudioType_eLpcmDvd",
    "BDSP_AudioType_eWmaStd",
    "BDSP_AudioType_eMlp",
    "BDSP_AudioType_ePcm",
    "BDSP_AudioType_eDtsLbr",
    "BDSP_AudioType_eDdp7_1",
    "BDSP_AudioType_eMpegMc",       
    "BDSP_AudioType_eWmaPro",
    "BDSP_AudioType_eLpcmDvdA",
    "BDSP_AudioType_eDtsBroadcast",
    "BDSP_AudioType_ePcmWav",         
    "BDSP_AudioType_eAmr",
    "BDSP_AudioType_eDra",
    "BDSP_AudioType_eRealAudioLbr",  
    "BDSP_AudioType_eAdpcm",
    "BDSP_AudioType_eG711G726",
    "BDSP_AudioType_eG729",
    "BDSP_AudioType_eVorbis",
    "BDSP_AudioType_eG723.1",
    "BDSP_AudioType_eFlac",
	"BDSP_AudioType_eMac",
    "BDSP_AudioType_eAmrwb",

};

char * videoAlgoString [] = {
    "BDSP_VideoType_eRealVideo9",
    "BDSP_VideoType_eVP6"
};



void BRAAGA_TEST_ManualTestMenu (
    BRAAGA_TEST_Handle    hRaagaTest
    )
{
	int			j=0;
	unsigned 	i=0,InputData=0;
	
	printf("How many streams you want to add\t");
	scanf("%ud",&InputData);
	hRaagaTest->uiNumStreams = InputData;
	
	printf("hRaagaTest->uiNumStream=%d\n",hRaagaTest->uiNumStreams);
    
    if(hRaagaTest->bAudEnable == true)
    {
#if RAAGA_TEST_ENABLE_TRANSPORT    
        printf ("\n\nEnter the audio Stream Information for Stream No: %d\n",i);
        printf ("--------------------------------------------------\n\n");

		do
			{
			printf ("Enter the audio stream type TS(0),ES(3) etc:\t");
			scanf("%ud",&InputData);
			}while(InputData != 0 && InputData != 3);
		hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eStreamType =(BAVC_StreamType)InputData;

		if(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eStreamType == BAVC_StreamType_eTsMpeg)
		{
	        printf ("Enter PCR PID (in hex) : ");
	        scanf  ("%x",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].uiPcrPid));
		
	        printf ("Enter Audio PID (in hex) : ");
	        scanf  ("%x",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].uiAudioPid));
		}
#endif       

#if RAAGA_TEST_STATIC_CDB_ITB
		if(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eStreamType == BAVC_StreamType_eEs)
		{

	        printf ("Enter audio the CDB Filename : ");
	        scanf  ("%s",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].aCdbFileName[0]));

	        printf ("Enter audio the ITB Filename : ");
	        scanf  ("%s",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].aItbFileName[0]));        
		}
#endif
        printf ("Enter Audio Algorithm : \n");
		printf("eStreamType=%d uiPcrPid=%d uiAudioPid=%d Algorithm=%d\n",
			hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eStreamType,
			hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].uiPcrPid,
			hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].uiAudioPid,
			hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eAlgoType);
        for (j=0; j<BDSP_AudioType_eMax; j++)
        {
            printf ("(%d) => %s\n",j,algoString[j]);
        }
        scanf  ("%ud",&InputData);
		hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eAlgoType = (BDSP_AudioType) InputData;
    		hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].bUsed = true;
        
#if RAAGA_TEST_ENABLE_TRANSPORT    
	if(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eStreamType == BAVC_StreamType_eTsMpeg)
	{
        if (hRaagaTest->bFilePb == true)
        {
			printf("Enter audio stream path\n");
			scanf("%s",hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].aStreamName);
		}
        else
        {
            sprintf (hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].aStreamName, " ");
        }
	}
#endif
        i++;
    }
        
    if(hRaagaTest->bVidEnable == true)        
    {
#if RAAGA_TEST_ENABLE_TRANSPORT    
        printf ("\n\nEnter the video Stream Information for Stream No: %d\n",i);
        printf ("--------------------------------------------------\n\n");

        do
		{
    		printf ("Enter the audio stream type TS(0),ES(3) etc:\t");
			scanf("%ud",&InputData);
		}while(InputData != 0 && InputData != 3);
		hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eStreamType =(BAVC_StreamType)InputData;

		if(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eStreamType == BAVC_StreamType_eTsMpeg)
		{
	        printf ("Enter PCR PID (in hex) : ");
	        scanf  ("%x",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].uiPcrPid));
	
	        printf ("Enter Video PID (in hex) : ");
	        scanf  ("%x",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].uiAudioPid));
        }
#endif       

#if RAAGA_TEST_STATIC_CDB_ITB
		if(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eStreamType == BAVC_StreamType_eEs)
		{
	        printf ("Enter the video CDB Filename : ");
	        scanf  ("%s",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].aCdbFileName[0]));
	
	        printf ("Enter the video ITB Filename : ");
	        scanf  ("%s",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].aItbFileName[0]));        
        }    
#endif        
        printf ("Enter Video Algorithm : \n");
		printf("eStreamType=%d uiPcrPid=%d uiAudioPid=%d Algorithm=%d\n",
			hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eStreamType,
			hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].uiPcrPid,
			hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].uiAudioPid,
			hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eVAlgoType);
        for (j=0; j<BDSP_VideoType_eMax; j++)
        {
            printf ("(%d) => %s\n",j,videoAlgoString[j]);
        }
        scanf  ("%ud",&InputData);
		hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eVAlgoType = (BDSP_VideoType) InputData;
		hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].bUsed = true;        

#if RAAGA_TEST_ENABLE_TRANSPORT    
	    if(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eStreamType == BAVC_StreamType_eTsMpeg)
		{
			printf("Enter video stream path\n");
			scanf("%s",hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].aStreamName);
	    }
#endif
    }        
	
/*	strcpy(hRaagaTest->sRaagaStrParams[0].aStreamName, "/mnt/nfs/gdata/difmus1_ac3_11_14.ts"); */
}
