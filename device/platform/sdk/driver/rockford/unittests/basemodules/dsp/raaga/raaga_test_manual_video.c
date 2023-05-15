/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test_manual_video.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/14/12 3:38p $
 *
 * Module Description:
 *  All the manual mode of testing will be part of this module.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/raaga_test_manual_video.c $
 * 
 * Hydra_Software_Devel/2   8/14/12 3:38p rshijith
 * FWRAA-478: Improved the manual parameter entry interface
 * 
 * Hydra_Software_Devel/1   8/14/12 3:03p rshijith
 * FWRAA-478: Adding video encoder related files to raaga
 * 
 * Hydra_Software_Devel/H264_enc_devel/2   11/7/11 9:39a ashoky
 * SW7231-344: Adding changes in the interface after discussion.
 * 
 * Hydra_Software_Devel/H264_enc_devel/1   9/20/11 7:08p ashoky
 * SW7231-344: Adding initial code changes for video encoder on raaga as
 * per first design.
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

#include "raaga_test_video.h"

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
    "BDSP_AudioType_eG729"
};

char * videoAlgoString [] = {
    "BDSP_VideoType_eRealVideo9",
    "BDSP_VideoType_eVP6"
};

char * videoEncAlgoString [] = {
    "BDSP_VideoEncode_eH264",
	"BDSP_VideoEncode_eVP8"
};
char * videoEncFrameRateString [] = {
	"BDSP_VF_P_ENCODE_FRAME_RATE_eUNKNOWN",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e23_97",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e24",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e25",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e29_97",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e30",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e50",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e59_94",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e60",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e14_985",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e7_493",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e15",
	"BDSP_VF_P_ENCODE_FRAME_RATE_e10"
};

char * videoEncProfileString [] = {
	
	"H264_PROFILE_eBASELINE",
	"H264_PROFILE_eMAIN",
	"VP8_RfBicubic__LfNormal",
	"VP8_RfBilinear_LfSimple",
	"VP8_RfBilinear_LfNone",
	"VP8_RfNone_____LfNone",
};

char * videoEncGopString [] = {
	
    "GOP_STRUCT_eIonly",
    "GOP_STRUCT_eIP",
	"GOP_STRUCT_eIPB",
    "GOP_STRUCT_eIPBB"
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
	
	printf("NumStreams=%d\n",hRaagaTest->uiNumStreams);
    
    if(hRaagaTest->bAudEnable == true)
    {
#if RAAGA_TEST_ENABLE_TRANSPORT    
        printf ("\n\nEnter the audio Stream Information for Stream No: %d\n",i);
        printf ("--------------------------------------------------\n\n");

        printf ("Enter the audio stream type TS(0),ES(3) etc:\t");
		scanf("%ud",&InputData);
		hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eStreamType =(BAVC_StreamType)InputData;

        printf ("Enter PCR PID (in hex) : ");
        scanf  ("%x",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].uiPcrPid));

        printf ("Enter Audio PID (in hex) : ");
        scanf  ("%x",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].uiAudioPid));
#endif       

#if RAAGA_TEST_STATIC_CDB_ITB
        printf ("Enter audio the CDB Filename : ");
        scanf  ("%s",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].aCdbFileName[0]));

        printf ("Enter audio the ITB Filename : ");
        scanf  ("%s",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].aItbFileName[0]));        
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
		printf("Enter audio stream path\n");
		scanf("%s",hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].aStreamName);
#endif
        i++;
    }
        
    if(hRaagaTest->bVidEnable == true)        
    {
#if RAAGA_TEST_ENABLE_TRANSPORT    
        printf ("\n\nEnter the video Stream Information for Stream No: %d\n",i);
        printf ("--------------------------------------------------\n\n");

        printf ("Enter the video stream type TS(0),ES(3) etc:\t");
		scanf("%ud",&InputData);
		hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eStreamType =(BAVC_StreamType)InputData;

        printf ("Enter PCR PID (in hex) : ");
        scanf  ("%x",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].uiPcrPid));

        printf ("Enter Video PID (in hex) : ");
        scanf  ("%x",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].uiAudioPid));
#endif       

#if RAAGA_TEST_STATIC_CDB_ITB
        printf ("Enter the video CDB Filename : ");
        scanf  ("%s",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].aCdbFileName[0]));

        printf ("Enter the video ITB Filename : ");
        scanf  ("%s",&(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].aItbFileName[0]));        
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
		printf("Enter video stream path\n");
		scanf("%s",hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].aStreamName);
#endif
    }        
	if(hRaagaTest->bVidEncodeEnable == true)
	{	
		printf ("Enter the YUV video Filename : ");
		scanf  ("%s",&(hRaagaTest->sRaagaStrParams[0].aStreamName[0]));

		printf ("Enter picture height : ");
		scanf("%ud", &hRaagaTest->sVidEncodeParams.ui32IpHeight);
		printf("\n");
		printf ("Enter picture width : ");
		scanf("%ud", &hRaagaTest->sVidEncodeParams.ui32IpWidth);
		printf("\n");
        printf ("Enter Video Algorithm : \n");
		
        for (j=0; j < BDSP_VideoEncode_eMax; j++)
        {
            printf ("(%d) => %s\n",j,videoEncAlgoString[j]);
        }
        scanf  ("%ud",&InputData);
		hRaagaTest->sRaagaStrParams[0].eVAlgoType = (BDSP_VideoType) InputData;

		printf("Enter FrameRate : \n");
		for(j=0; j < BDSP_VF_P_EncodeFrameRate_eMax; j++)
		{
			printf ("(%d) => %s\n",j,videoEncFrameRateString[j]);
		}
		scanf  ("%ud",&InputData);
		hRaagaTest->sVidEncodeParams.ui32FrameRate = InputData;
		
		printf("which Profile : \n");

		if(hRaagaTest->sRaagaStrParams[0].eVAlgoType == 0)
		{
			printf ("(%d) => %s\n",BDSP_Raaga_VideoH264Profile_eBaseline,videoEncProfileString[0]);
			printf ("(%d) => %s\n",BDSP_Raaga_VideoH264Profile_eMain,videoEncProfileString[1]);
		}
		else{
			printf ("(0) => %s\n",videoEncProfileString[2]);
			printf ("(1) => %s\n",videoEncProfileString[3]);
			printf ("(2) => %s\n",videoEncProfileString[4]);
			printf ("(3) => %s\n",videoEncProfileString[5]);
		}
		scanf  ("%ud",&InputData);
		hRaagaTest->sVidEncodeParams.ui32Profile = InputData;

		if(hRaagaTest->sRaagaStrParams[0].eVAlgoType == 0)
		{		
			printf("Level : \n");
			printf ("31 => 3.1, 32 => 3.2, 40=> 4.0 \n");
			scanf  ("%ud",&InputData);
		}		
		hRaagaTest->sVidEncodeParams.ui32Level = InputData;
		
		printf("Enter Target Bitrate : \n");
		scanf  ("%ud",&InputData);
		hRaagaTest->sVidEncodeParams.ui32TargetBitRate = InputData;
		
		printf("Intra Period : \n");
		scanf  ("%ud",&InputData);
		hRaagaTest->sVidEncodeParams.ui32IntraPeriod = InputData;
		
		printf("IDR Period : \n");
		scanf  ("%ud",&InputData);
		hRaagaTest->sVidEncodeParams.ui32IdrPeriod = InputData;
		
		printf("Enter GOP structure : \n");
		for(j=0; j < BDSP_Raaga_VideoGopStruct_eMax; j++)
		{
			printf ("(%d) => %s\n",j,videoEncGopString[j]);
		}
		scanf  ("%ud",&InputData);
		hRaagaTest->sVidEncodeParams.ui32GopStruct = InputData;
		printf ("Deblocking Disable(0), Enable (1)\n");
		scanf  ("%ud",&InputData);
		hRaagaTest->sVidEncodeParams.bDblkEnable = InputData;
		printf ("Rate Control Disable(0), Enable (1)\n");
		scanf  ("%ud",&InputData);
		hRaagaTest->sVidEncodeParams.bRateCtrlEnable = InputData;
	}
	
/*	strcpy(hRaagaTest->sRaagaStrParams[0].aStreamName, "/mnt/nfs/gdata/difmus1_ac3_11_14.ts"); */
}
