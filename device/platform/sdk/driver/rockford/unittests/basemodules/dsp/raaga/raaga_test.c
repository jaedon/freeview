/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test.c $
 * $brcm_Revision: Hydra_Software_Devel/29 $
 * $brcm_Date: 9/17/12 2:21p $
 *
 * Module Description:
 *          This file contains all structures and function prototypes used 
 *          by raaga_test.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/raaga_test.c $
 * 
 * Hydra_Software_Devel/29   9/17/12 2:21p gprasad
 * SW7425-3952: Fixing build errors with raaga test for emulation build
 * 
 * Hydra_Software_Devel/28   7/16/12 4:19p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/27   5/11/12 3:56p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
 * sys_top_raaga changes
 * 
 * Hydra_Software_Devel/26   4/3/12 11:22a gprasad
 * FWRAA-382: Updating the error messages to logging messages
 * 
 * Hydra_Software_Devel/25   3/27/12 12:01p gprasad
 * FWRAA-398: Adding support for system watchdog timer in raaga test
 * 
 * Hydra_Software_Devel/24   2/29/12 5:54p gprasad
 * FWRAA-398: Adding support in raaga_test for AFAP decoding mode
 * 
 * Hydra_Software_Devel/23   2/23/12 11:45a gprasad
 * FWRAA-398: Added Changes to Raaga test to automatically stop the
 * decoder and exit once stream decode is complete, in manual test mode
 * 
 * Hydra_Software_Devel/22   2/22/12 2:44p gprasad
 * FWRAA-398: Add changes required for running audio certification tests
 * to raaga_test
 * 
 * Hydra_Software_Devel/21   1/27/12 12:01p gprasad
 * SW7360-2: Fixing issue with streamer playback in raaga test app
 * 
 * Hydra_Software_Devel/20   1/25/12 6:20p gprasad
 * SW7360-2: Fixing issue with streamer playback in raaga test app
 * 
 * Hydra_Software_Devel/19   1/10/12 4:37p gprasad
 * FWRAA-370: Adding support for encoder testing and output buffer capture
 * to raaga_test
 * 
 * Hydra_Software_Devel/18   1/5/12 12:47p gprasad
 * FWRAA-370: Adding changes to configure HIFIDAC RM based on sample rate
 * interrupt
 * 
 * Hydra_Software_Devel/17   11/9/11 4:20p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/16   10/7/11 3:31p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/15   10/7/11 2:34p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/14   9/26/11 4:20p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/Raaga_Test_Automation/6   9/26/11 3:35p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/Raaga_Test_Automation/5   9/26/11 1:56p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/Raaga_Test_Automation/4   9/26/11 11:08a purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/Raaga_Test_Automation/3   9/11/11 10:30p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/Raaga_Test_Automation/2   9/7/11 4:18a purush
 * SW7425-1063: [7425] Raaga test development for automation, added
 * changes to include the correct BDSP_Raaga_Audio_xxxStreamInfo
 * variables.
 * 
 * Hydra_Software_Devel/Raaga_Test_Automation/1   8/8/11 4:59p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/13   5/18/11 9:54p srajapur
 * SW7425-572: [7425] Adding VP6 support in raaga test app
 * 
 * Hydra_Software_Devel/12   5/17/11 4:27p srajapur
 * SWDTV-6746 : [35230] Adding support for 35230 and 35233 to raaga_test
 * 
 * Hydra_Software_Devel/11   5/17/11 11:09a srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/10   5/16/11 10:09p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/9   3/2/11 4:24a srajapur
 * SW7425-90 : [7425] Added raaga test app support for 7425
 * 
 * Hydra_Software_Devel/8   1/28/11 8:27a srajapur
 * SW7422-191:[7422] Adding different test modes.
 * 
 * Hydra_Software_Devel/7   1/28/11 2:07a srajapur
 * SW7422-191:[7422] Adding support for 7422 chip.
 * 
 * Hydra_Software_Devel/6   1/27/11 12:41a gautamk
 * SW7422-191:[7422] Correcting scanfs
 * 
 * Hydra_Software_Devel/5   1/25/11 3:35p gdata
 * SW7422-191:[7422] Reading all the parameters from the command line
 * argument so that we can run nightly build
 * 
**************************** Include Files**********************************/ 
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
#include <signal.h>


/* DSP includes */
#include "bdsp.h"
#include "bdsp_raaga.h"
#include "raaga_test.h"
#include "bchp_timer.h"

BDBG_MODULE(raaga_test);

static const BRAAGA_TEST_ChSettings sDefRaagaTestChSettings = 
{

    false,                          /*bEnableTSM*/

    BDSP_AF_P_BufferType_eRAVE, 	/*eAudioSrc*/

    0,                              /*uiItbformat*/
    
    0,                              /*uiNumStages;*/
	false,                          /*bLfeEn;*/
	
	0,                              /*uiTimebase;*/
    
	BAVC_AudioSamplingRate_e48k,    /*eInSR;*/
    2,                              /*uiInputSelect*/
    BRAAGA_TEST_CxtType_eMax
};

void BRAAGA_TEST_SetDefaultChSettings(BRAAGA_TEST_ChHandle   hRaagaTestCh)
{
	/* Assert the function argument(s) */
	BDBG_ASSERT(hRaagaTestCh);
	
	hRaagaTestCh->sSettings= sDefRaagaTestChSettings;

    return;
}

BRAAGA_TEST_Handle		hGlobalRaagaTestHandle = NULL;
BCHP_Handle                     g_hChip      = NULL;
BREG_Handle                     g_hRegister  = NULL;
BMEM_Handle                     g_hMemory    = NULL;
BINT_Handle                     g_hInterrupt = NULL;
BTMR_Handle                     g_hTmr       = NULL;

/* Global ennvironmental variables */
BDSP_AudioEncode     g_encoder_audio_type   = BDSP_AudioEncode_eMax;
BDSP_AudioProcessing g_pp_audio_type        = BDSP_AudioProcessing_eMax;
bool g_enable_echo_canceller                = false;
bool g_capture_encoder_out                  = false;
bool g_capture_decoder_stereo               = false;
bool g_capture_decoder_multich              = false;
bool g_afap_mode                            = false;
bool g_enable_watchdog                      = false;


static void BRGA_TST_EnableWatchdogTimer(int count)
{
    BREG_Write32(g_hRegister, BCHP_TIMER_WDTIMEOUT, count);
    BREG_Write32(g_hRegister, BCHP_TIMER_WDCMD, 0x0000FF00);
    BREG_Write32(g_hRegister, BCHP_TIMER_WDCMD, 0x000000FF);
}

static void BRGA_TST_DisbleWatchdogTimer(void)
{
    BREG_Write32(g_hRegister, BCHP_TIMER_WDCMD, 0x0000EE00);
    BREG_Write32(g_hRegister, BCHP_TIMER_WDCMD, 0x000000EE);
}

static void BRGA_TST_WatchDogTask(BTST_TaskHandle Task, void *Parm)
{
    BSTD_UNUSED(Task);
    BSTD_UNUSED(Parm);
    
    while (1)
    {
        BRGA_TST_DisbleWatchdogTimer();
        
        /* Check this variable everytime since this can be set to false before the program quits to avoid any race condition */
        if (g_enable_watchdog)
        {
            BRGA_TST_EnableWatchdogTimer(0x30479E80); /* 30 seconds for 27 Mhz timer */
        }
        
        sleep (3);
    }
}

void sigkill_handler (int signum)
{
    /* Disable  the watchdog as ctrl+c is received from the user */
    BDBG_LOG (("Disabling the watchdog timer : (%d)", signum));
    BRGA_TST_DisbleWatchdogTimer();
    
    exit (-1);
}

BERR_Code RaageTest_ParseCmd(BRAAGA_TEST_Handle		hRaagaTest, int argc, char **argv)
{
	unsigned int                i, argCnt=0;

	for (argCnt=1; argCnt<(unsigned int)argc; argCnt++)
	{
		if(strcmp(argv[argCnt], "-strnum") == 0)
		{
			hRaagaTest->uiNumStreams = atoi(argv[argCnt+1]);
			printf(" RaageTest_ParseCmd sting num=%d\n",atoi(argv[argCnt+1]));
			break;
		}
	}
	if(hRaagaTest->uiNumStreams > MAX_STREAMS)
	{
		BDBG_ERR(("RaageTest_ParseCmd: NumStreams=%d is more the supported",hRaagaTest->uiNumStreams));
		return BERR_INVALID_PARAMETER;
	}
	
	for(i=0; i<hRaagaTest->uiNumStreams; ++i)
	{
		for (argCnt=1; argCnt<(unsigned int)argc; argCnt++)
		{
			if(strcmp(argv[argCnt], "-pcr") == 0)
			{
				argCnt++;
				hRaagaTest->sRaagaStrParams[i].uiPcrPid = atoi(argv[argCnt]);
			}
			if(strcmp(argv[argCnt], "-audio") == 0)
			{
				argCnt++;
				hRaagaTest->sRaagaStrParams[i].uiAudioPid = atoi(argv[argCnt]);
			}
			if(strcmp(argv[argCnt], "-path") == 0)
			{
				argCnt++;
				strncpy(hRaagaTest->sRaagaStrParams[0].aStreamName, argv[argCnt],300);
				break;
			}	
			if(strcmp(argv[argCnt], "-audiotype") == 0)
			{
				argCnt++;
				hRaagaTest->sRaagaStrParams[i].eAlgoType = (BDSP_AudioType)atoi(argv[argCnt]);
			}
			if(strcmp(argv[argCnt], "-pkttype") == 0) 
			{
				argCnt++;
				hRaagaTest->sRaagaStrParams[i].eStreamType = (BAVC_StreamType)atoi(argv[argCnt]);
			}
		}
		if(!(hRaagaTest->sRaagaStrParams[i].uiPcrPid))
		{
			BDBG_ERR(("RaageTest_ParseCmd: command is not correct"));
			BDBG_ERR(("Enter command as e.g."));
			BDBG_ERR(("raaga_test -cmd -strnum 1 -path /mnt/nfs/difmus1_ac3_11_14.ts -pcr 17 -audio 20 -audiotype 5 -pkttype 0"));
			return BERR_INVALID_PARAMETER;
		}
	}
	for(i=0; i<hRaagaTest->uiNumStreams; ++i)
	{
		printf("pcr =%d\n",hRaagaTest->sRaagaStrParams[i].uiPcrPid);
		printf("audio =%d\n",hRaagaTest->sRaagaStrParams[i].uiAudioPid);
		printf("path =%s\n",hRaagaTest->sRaagaStrParams[i].aStreamName);
		printf("audiotype =%d\n",hRaagaTest->sRaagaStrParams[i].eAlgoType);
		printf("pkttype =%d\n",hRaagaTest->sRaagaStrParams[i].eStreamType);
	}	
    return BERR_SUCCESS;
}

void RaagaTest_readcmd(char *cmd)
{
	int		i=0;
	while((cmd[i++]=getchar())!= '\n')
	{}
	for(i=0; i<BRAAG_TEST_CMD_SIZE; i++)
	{
		if(cmd[i] != '\n')
		{
		}
		else
			break;
	}
	cmd[i]='\0';
}
void	RaagaTest_PrintCmd(void)
{
	printf("Stop Task:          stpt\n");
	printf("Start Task:         strt\n");
	printf("Open Task:          opt\n");
	printf("Close Task:         clt\n");
	printf("Open Context:                opcxt\n");
	printf("Close Context:               clcxt\n");
	printf("quit this menu:     quit\n");
	printf("enter command \n");
}

int RaagaTest_ReadFile(FILE * fin, BRAAGA_TEST_CaseInfo *file_params)
{
	int index=0, MAXLINE = 250;
	char line[250]	;
	FILE *f; 

	if(!feof(fin))
	{
		while( fgets(line,MAXLINE,fin) != NULL )
		{
			if(line[0] != '#')
			{
				
				if( sscanf(line,"%s %u %x %x %u %u",file_params[index].TestStreamInfo.aStreamName ,
												&file_params[index].TestStreamInfo.eStreamType ,
												&file_params[index].TestStreamInfo.uiPcrPid ,
												&file_params[index].TestStreamInfo.uiAudioPid ,
												&file_params[index].TestStreamInfo.eAlgoType , 
												&file_params[index].StreamRefFrameCount)== 6
					)

				{
					if(NULL == (f = fopen (file_params[index].TestStreamInfo.aStreamName, "r"))) 
				  	{
				  		printf("\nError: Cannot open input file %s.", file_params[index].TestStreamInfo.aStreamName);
					}
					else
					{
						index ++;
						fclose(f);
					}
					

				}
			
			}
		
			
		}
	}

	return index;
}

int raaga_main(int argc, char **argv, BFramework_Info *frmInfo)
{
	BERR_Code					err = BERR_SUCCESS;
	/* General Handles */
    BRAAGA_TEST_Handle			hRaagaTest;
    BRAAGA_TEST_ChHandle        hRaagaTestCh;
	BRAAGA_TEST_TaskHandle		hTaskHandle[RAAGA_MAX_CONTEXT];
    BRAAGA_TEST_Mode            testmode = BRAAGA_TEST_Mode_ConfigFile;
	/* Misc variables */
	unsigned int                i, argCnt=0,cxtType;
	char						cmd[BRAAG_TEST_CMD_SIZE];
	bool						bXptPb=false;
	bool						bFilePb=false;
    bool                        bVidEnable=false;
    bool                        bAudEnable=false;    
    BTST_TaskHandle             watchdogTask;

	/*Misc variables to be used for looping through all test cases and reproting test results.  */
	unsigned int				TestCases_Index=0,TotalTestCases=1;
	char 						*TestCaseResult;
	char 						itbfilename[100]="/dev/zero";
	FILE * fptr=NULL;
	char *ch;


	/* BDSP_Raaga_Audio_xxxStreamInfo variables are required for BDSP_Task_GetStageStatus to get  account of the total number for frames decoded.  */
	BDSP_Raaga_Audio_MultiStreamDDPStreamInfo Ac3StreamInfo;	
	BDSP_Raaga_Audio_MultiStreamDDPStreamInfo DdpStreamInfo; 	
	BDSP_Raaga_Audio_MpegStreamInfo MpegStreamInfo; 
#ifdef BDSP_MS10_SUPPORT	
	BDSP_Raaga_Audio_DolbyPulseStreamInfo 	DolbyPulseStreamInfo;
#else    
	BDSP_Raaga_Audio_AacheStreamInfo DolbyPulseStreamInfo;
#endif	
	BDSP_Raaga_Audio_DtsHdStreamInfo	DtshdStreamInfo;
	BDSP_Raaga_Audio_WmaStreamInfo WmaStdStreamInfo; 	
	BDSP_Raaga_Audio_WmaProStreamInfo WmaProStreamInfo; 	
		
	/* Array of structure to hold all test cases information from configuration file. Increment value if based on the number of test cases defined in the Config file.*/ 
	BRAAGA_TEST_CaseInfo AutomationTestCase[RAAGA_TEST_AUTOMATION_MAX_TEST_CASES];
	

    /* Set the enviromental variables here */
    {
        void *ptr;
        
        /* Encoder Type */
        if (NULL != (ptr = getenv("raaga_test_encoder_audio_type")))
        {
            g_encoder_audio_type = atoi(ptr);
            
            if (g_encoder_audio_type >= BDSP_AudioEncode_eMax)
            {
                g_encoder_audio_type = BDSP_AudioEncode_eMax;
            }
        }
        
        /* PP type - Will be enabled only when encode is disabled */
        if (g_encoder_audio_type == BDSP_AudioEncode_eMax)
        {
            if (NULL != (ptr = getenv("raaga_test_pp_audio_type")))
            {
                g_pp_audio_type = atoi(ptr);

                if ((g_pp_audio_type >= BDSP_AudioProcessing_eMax)
                    || (g_pp_audio_type == BDSP_AudioProcessing_eSrc)
                    || (g_pp_audio_type == BDSP_AudioProcessing_eFWMixer)
                    || (g_pp_audio_type == BDSP_AudioProcessing_eSpeexAec)) /* Enable speex aec using export raaga_test_enable_echo_canceller */
                {
                    g_pp_audio_type = BDSP_AudioProcessing_eMax;
                }
            }
        }
        
        /* Echo canceller enable - Check only if speech encoder is enabled */
        if ((g_encoder_audio_type == BDSP_AudioEncode_eG711G726)
            || (g_encoder_audio_type == BDSP_AudioEncode_eG729)
            || (g_encoder_audio_type == BDSP_AudioEncode_eG723_1)
            || (g_encoder_audio_type == BDSP_AudioEncode_eG722)
            || (g_encoder_audio_type == BDSP_AudioEncode_eAmr))
        {
            if (NULL != (ptr = getenv("raaga_test_enable_echo_canceller")))
            {
                g_enable_echo_canceller = atoi(ptr);

                if (g_enable_echo_canceller == 1)
                {
                    g_enable_echo_canceller = true;
                }
            }
        }
        
        /* Encoder capture enable - Check only if encoder is enabled */
        if (g_encoder_audio_type != BDSP_AudioEncode_eMax)
        {
            if (NULL != (ptr = getenv("raaga_test_capture_encoder_out")))
            {
                g_capture_encoder_out = atoi(ptr);

                if (g_capture_encoder_out == 1)
                {
                    g_capture_encoder_out = true;
                }
            }
        }
            
        /* Stereo capture enable */
        if (NULL != (ptr = getenv("raaga_test_capture_decoder_stereo")))
        {
            g_capture_decoder_stereo = atoi(ptr);
            if (g_capture_decoder_stereo == 1)
            {
                g_capture_decoder_stereo = true;
            }
        }

        /* Multi-ch capture enable - Check only if stereo capture is disabled */
        if (!(g_capture_decoder_stereo))
        {
            if (NULL != (ptr = getenv("raaga_test_capture_decoder_multich")))
            {
                g_capture_decoder_multich = atoi(ptr);
                
                if (g_capture_decoder_multich == 1)
                {
                    g_capture_decoder_multich = true;
                }
            }
        }
        
        /* AFAP decoding - Check only if capture is enabled for the decoder */
        if (g_capture_decoder_stereo || g_capture_decoder_multich)
        {
            if (NULL != (ptr = getenv("raaga_test_enable_afap_mode")))
            {
                g_afap_mode = atoi(ptr);
                
                if (g_afap_mode == 1)
                {
                    g_afap_mode = true;
                }
            }
        }
        
        /* Enable watchdog timer to detect any kernel crashes and reboot  the board */
        if (NULL != (ptr = getenv("raaga_test_enable_watchdog_timer")))
        {
            g_enable_watchdog = atoi(ptr);
            
            if (g_enable_watchdog == 1)
            {
                g_enable_watchdog = true;
            }
        }
    }

    BDBG_LOG(("g_pp_audio_type=%d", g_pp_audio_type));
    BDBG_LOG(("g_encoder_audio_type=%d", g_encoder_audio_type));
    BDBG_LOG(("g_enable_echo_canceller=%d", g_enable_echo_canceller));
    BDBG_LOG(("g_capture_encoder_out=%d", g_capture_encoder_out));
    BDBG_LOG(("g_capture_decoder_stereo=%d", g_capture_decoder_stereo));
    BDBG_LOG(("g_capture_decoder_multich=%d", g_capture_decoder_multich));
    BDBG_LOG(("g_enable_watchdog=%d", g_enable_watchdog));
    
    /* Debug Stuff */
#if BDBG_DEBUG_BUILD
    BDBG_Init();
    BRAAGA_TEST_SetModuleDebugLevel(bsettop_get_config("trace_modules"), BDBG_eTrace);
    BRAAGA_TEST_SetModuleDebugLevel(bsettop_get_config("msg_modules"), BDBG_eMsg);
    BRAAGA_TEST_SetModuleDebugLevel(bsettop_get_config("wrn_modules"), BDBG_eWrn);
#endif

	

    if((argc == 0) || (argv == NULL))
    {
        printf("\nNo arguements passed to the program. exiting..\n");
        return 0;
    }

    if (argc == 1)
    {
        /* No arguments passed - entering manual mode by default */
        testmode = BRAAGA_TEST_Mode_Manual;        
    }
    else
		if (argc == 2 && (strcmp(argv[1],"-cdbitb")==0))
		{
			TotalTestCases = 1;
		}
		else
			if (argc == 3 && (strcmp(argv[1],"-cfg")==0)) 
			{
				testmode = BRAAGA_TEST_Mode_ConfigFile;
				bAudEnable = true; 
				if((fptr=fopen(argv[2],"r"))==NULL)
				{
					printf("\nUnable to open file %s for reading",argv[2]);
					return 1;
				}
				else
					printf("\nSuccessfully opened file for reading %s",argv[2]);

				if((TotalTestCases = RaagaTest_ReadFile(fptr,AutomationTestCase)) != 0)
				{
					printf("\n******************************");
					printf("\nSuccessfully read file: %s",argv[2]);
					printf("\nTOTAL TEST CASES = %d", TotalTestCases);
					printf("\n******************************\n");
				}				
				else	
				{
					printf("\nNo test cases detected in config file: ");	
					fclose(fptr);
					return 1; 
				}
					
			}
			else				
		    {
		        for (argCnt=1; argCnt<(unsigned int)argc; argCnt++)
		        {
					TotalTestCases = 1;
					if(strcmp(argv[argCnt], "-m") == 0)
		            {
		                testmode = BRAAGA_TEST_Mode_Manual;
		            }
					else if(strcmp(argv[argCnt], "-a") == 0)
					{
						testmode = BRAAGA_TEST_Mode_Auto;
					}
					else if(strcmp(argv[argCnt], "-pb") == 0)
					{
						bFilePb = true;
					}
					else if(strcmp(argv[argCnt], "-cmd") == 0)
					{
						testmode = BRAAGA_TEST_Mode_ArgCmd;
					}
					else if(strcmp(argv[argCnt], "-vid") == 0)
					{
						bVidEnable = true;
					} 
					else if(strcmp(argv[argCnt], "-aud") == 0)
					{
						bAudEnable = true;
					}                        
		            else if((strcmp(argv[argCnt], "-?") == 0) ||
		                    (strcmp(argv[argCnt], "-help") == 0))
		            {
		                printf ("\n\n\n RAAGA Test -- Help");
						printf ("\n========================================================");
						printf ("\n  -m 	   : Forces Manual Mode. Required for all options below");
						printf ("\n  -a 	   : Forces Auto Mode.");
						printf ("\n  -pb	   : To Enable play pump.");
						printf ("\n  -cdbitb   : Run an ES or valid CDB/ITB files. ");
						printf ("\n  -cfg  file: Run with a config file having details of all test cases. ");
						printf ("\n=========================================================\n\n\n\n");
						
		                return 0;
		            }
		        }
		    }

	/* Allocate the raaga_test Device handle */
	hRaagaTest = (BRAAGA_TEST_Handle)BKNI_Malloc(sizeof(BRAAGA_TEST_Device));
	if(hRaagaTest == NULL)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	/* Reset the structures */
	BKNI_Memset(hRaagaTest, 0, sizeof(BRAAGA_TEST_Device));

	/* Board is already inited */    
	hRaagaTest->hChp 	= frmInfo->hChp;
	hRaagaTest->hMem 	= frmInfo->hMem;
	hRaagaTest->hInt    = frmInfo->hInt;
	hRaagaTest->hReg    = frmInfo->hReg;
    hRaagaTest->hTmr    = frmInfo->hTmr;
	BDBG_LOG(("hRaagaTest->hMem=%x frmInfo->hMem=%x",hRaagaTest->hMem,frmInfo->hMem));
	hGlobalRaagaTestHandle = hRaagaTest;
	g_hChip = frmInfo->hChp;
	g_hMemory = frmInfo->hMem;
	g_hInterrupt = frmInfo->hInt;
	g_hRegister = frmInfo->hReg;
    g_hTmr = frmInfo->hTmr;
    hRaagaTest->eTestmode = testmode;
    hRaagaTest->uiNumStreams = 1;


 
		while(TestCases_Index < TotalTestCases )
		{

	    /* Init Channels */
	    for (i=0; i<RAAGA_MAX_CONTEXT; i++)
	    {
	    	/* Allocate the raaga_test channel handle */
	    	hRaagaTestCh = (BRAAGA_TEST_ChHandle)BKNI_Malloc(sizeof(BRAAGA_TEST_Channel));
	    	if(hRaagaTestCh == NULL)
	    	{
	    		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	    	}        
	        BKNI_Memset(hRaagaTestCh, 0, sizeof(BRAAGA_TEST_Channel));        

	        /* Populate default values of hRaagaTest content */
	        BRAAGA_TEST_SetDefaultChSettings(hRaagaTestCh);
	        hRaagaTest->hRaagaTestCh[i] = hRaagaTestCh;
	    }

		hRaagaTest->bXptPbEn = bXptPb;
		hRaagaTest->bFilePb = bFilePb;
		hRaagaTest->bAudEnable = bAudEnable;
		hRaagaTest->bVidEnable = bVidEnable;


	    switch (hRaagaTest->eTestmode)
	    {
	        case BRAAGA_TEST_Mode_Manual:
	            /* Get Parameters for manual mode from user */
	            printf ("\n Manual Mode is Enabled !\n");
	            BRAAGA_TEST_ManualTestMenu (hRaagaTest);
                if(bAudEnable == true)
                {
					if(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eAudio].eStreamType == BAVC_StreamType_eTsMpeg)
					{		
						bXptPb = true;
					}	
					else
						bXptPb = false; 
                }
                else
                {
                    if(hRaagaTest->sRaagaStrParams[BRAAGA_TEST_CxtType_eVideo].eStreamType == BAVC_StreamType_eTsMpeg)
    				{		
    					bXptPb = true;
    				}	
    				else
    					bXptPb = false; 
                }
				hRaagaTest->bXptPbEn = bXptPb;
				
	            break;

	        case BRAAGA_TEST_Mode_ConfigFile:
	            /* Get Parameters for the config file passed */
	            printf ("\n Config File Mode is Enabled !\n");
				/* BRAAGA_TEST_GetParamsFromConfig (hRaagaTest, hRaagaTestCh, argv[1]); */
				if(AutomationTestCase[TestCases_Index].TestStreamInfo.eStreamType == 0)	
				{		
					bXptPb = true;
				}	
				else
					bXptPb = false; 
				
				/* With a config file the inputs are all file playbacks */
				bFilePb = true;
				hRaagaTest->bFilePb = bFilePb;
				hRaagaTest->bXptPbEn = bXptPb;

				hRaagaTest->uiNumStreams = 1;
				hRaagaTest->sRaagaStrParams[0].eStreamType = AutomationTestCase[TestCases_Index].TestStreamInfo.eStreamType;
				hRaagaTest->sRaagaStrParams[0].uiAudioPid = AutomationTestCase[TestCases_Index].TestStreamInfo.uiAudioPid; 
				hRaagaTest->sRaagaStrParams[0].uiPcrPid = AutomationTestCase[TestCases_Index].TestStreamInfo.uiPcrPid; 
				hRaagaTest->sRaagaStrParams[0].eAlgoType = AutomationTestCase[TestCases_Index].TestStreamInfo.eAlgoType; 
				hRaagaTest->sRaagaStrParams[0].bUsed = true;	
				strncpy(hRaagaTest->sRaagaStrParams[0].aStreamName, AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName,300);	
				if(hRaagaTest->bXptPbEn == false)	
				{					
					printf("\nAssigning values to CDB ITB files\n%s\n%s",AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName,hRaagaTest->sRaagaStrParams[0].aCdbFileName);
					strncpy(hRaagaTest->sRaagaStrParams[0].aCdbFileName, AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName,100);	
					strncpy(hRaagaTest->sRaagaStrParams[0].aItbFileName, itbfilename,100);	
				}
	            break;
			case BRAAGA_TEST_Mode_Auto:
			{
                /* With the command line input currently only file playbacks are supported */
  				bFilePb = true;
				hRaagaTest->bFilePb = bFilePb;

				strncpy(AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName, argv[3],300);
				if(strcmp(argv[4],"0")== 0)						
				{						
					AutomationTestCase[TestCases_Index].TestStreamInfo.eStreamType = BAVC_StreamType_eTsMpeg;						
					printf("\n Assigned TS");						
				}					
				else 						
					if(strcmp(argv[4],"3")== 0)							
					{							
						AutomationTestCase[TestCases_Index].TestStreamInfo.eStreamType = BAVC_StreamType_eEs;
						printf("\n Assigned ES");							
					}						
					else							
						printf("\n*************Invalid argument*******************");

				AutomationTestCase[TestCases_Index].TestStreamInfo.uiPcrPid = atoi(argv[5]);					
				AutomationTestCase[TestCases_Index].TestStreamInfo.uiAudioPid = atoi(argv[6]);					
				AutomationTestCase[TestCases_Index].TestStreamInfo.eAlgoType = atoi(argv[7]);
				AutomationTestCase[TestCases_Index].StreamRefFrameCount = atoi(argv[8]);
				
				
				
				hRaagaTest->uiNumStreams = 1;
				hRaagaTest->sRaagaStrParams[0].eStreamType = AutomationTestCase[TestCases_Index].TestStreamInfo.eStreamType;
				hRaagaTest->sRaagaStrParams[0].uiAudioPid = AutomationTestCase[TestCases_Index].TestStreamInfo.uiAudioPid;
				hRaagaTest->sRaagaStrParams[0].uiPcrPid = AutomationTestCase[TestCases_Index].TestStreamInfo.uiPcrPid;
				hRaagaTest->sRaagaStrParams[0].eAlgoType = AutomationTestCase[TestCases_Index].TestStreamInfo.eAlgoType;
				hRaagaTest->sRaagaStrParams[0].bUsed = true;
				strncpy(hRaagaTest->sRaagaStrParams[0].aStreamName, AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName,300);

				if(hRaagaTest->sRaagaStrParams[0].eStreamType == BAVC_StreamType_eTsMpeg) 					
				{								
					bXptPb = true;					
				}						
				else						
					bXptPb = false; 									
				hRaagaTest->bXptPbEn = bXptPb;					
				if(hRaagaTest->bXptPbEn == false)						
				{											
					printf("\nAssigning values to CDB ITB files\n%s\n%s",AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName,hRaagaTest->sRaagaStrParams[0].aCdbFileName);						
					strncpy(hRaagaTest->sRaagaStrParams[0].aCdbFileName, AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName,100);							
					strncpy(hRaagaTest->sRaagaStrParams[0].aItbFileName, itbfilename,100);						
				}					
				else						
					printf("\n Its a TS Playback file");

				printf("\nCommand Line parameters are as follows:\n");					
				printf("\n Stream Name : %s",hRaagaTest->sRaagaStrParams[0].aStreamName);					
				printf("\n Stream Type : %d",hRaagaTest->sRaagaStrParams[0].eStreamType);					
				printf("\n Stream uiPcrPid : %d",hRaagaTest->sRaagaStrParams[0].uiPcrPid);					
				printf("\n Stream uiAudioPid : %d",hRaagaTest->sRaagaStrParams[0].uiAudioPid);					
				printf("\n Stream eAlgoType : %d : ",hRaagaTest->sRaagaStrParams[0].eAlgoType);
				break;
			}
			case BRAAGA_TEST_Mode_ArgCmd:
			{
                /* With the command line input currently only file playbacks are supported */
  				bFilePb = true;
				hRaagaTest->bFilePb = bFilePb;

				if(hRaagaTest->sRaagaStrParams[TestCases_Index].eStreamType == 0)	
				{		
					bXptPb = true;
				}	
				else
					bXptPb = false; 
                    
                hRaagaTest->bXptPbEn = bXptPb;
                
				RaageTest_ParseCmd(hRaagaTest, argc, argv);
				break;
			}
	        default:
	            printf ("\n Unsupported Mode");
	    }

	if(bXptPb == true)
	{
#if RAAGA_TEST_ENABLE_TRANSPORT
		BDBG_LOG(("hChip =%x hMemory=%x hInt=%x hReg=%x hRaagaTestCh=%x hRaagaTest=%x",
					hRaagaTest->hChp,hRaagaTest->hMem,hRaagaTest->hInt,hRaagaTest->hReg,
					hRaagaTest->hRaagaTestCh[0],hRaagaTest));
	    err = BRAAGA_TEST_XptInit(hRaagaTest, 
	    							hRaagaTest->hRaagaTestCh[0], 
	    							hRaagaTest->hChp, 
	    							hRaagaTest->hMem, 
	    							hRaagaTest->hInt, 
	    							hRaagaTest->hReg);
	    BDBG_ASSERT(err == BERR_SUCCESS);
	    BDBG_MSG(("XPT INIT DONE!!!!"));
		BDBG_MSG(("Done BRAAGA_TEST_XptInit()"));
#endif
	}
		BDBG_MSG(("Calling BRAAGA_TEST_DspInit()"));
	    err = BRAAGA_TEST_DspInit(hRaagaTest,
	    							hRaagaTest->hChp,
	    							hRaagaTest->hMem,
	    							hRaagaTest->hInt,
	    							hRaagaTest->hReg,
	    							hRaagaTest->hTmr);
	    BDBG_ASSERT(err == BERR_SUCCESS);

        if (g_capture_encoder_out || g_capture_decoder_stereo || g_capture_decoder_multich)
        {
            pcaptureInfo->hReg = hRaagaTest->hReg;
            pcaptureInfo->hMem = hRaagaTest->hMem;
            BTST_CreateTask( &( pcaptureInfo->hCaptureTask ), BRGA_TST_BufferCaptureTask, ( void * ) pcaptureInfo);
            BTST_CreateTask( &( pcaptureInfo->hFileWriteTask ), BRGA_TST_FileWriteTask, ( void * ) pcaptureInfo);
        }

        if (g_enable_watchdog)
        {
            BTST_CreateTask( &watchdogTask, BRGA_TST_WatchDogTask, NULL);
            /* Install the signal handlers */
            signal(SIGTERM, sigkill_handler);
            signal(SIGTSTP, sigkill_handler);
            signal(SIGINT, sigkill_handler);
        }

	    if((bAudEnable == true)||(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_ArgCmd))
	    {
	        BDBG_MSG(("Dsp INIT DONE!!!!"));
	        err = BRAAGA_TEST_OpenRaagaContext(hRaagaTest, 
	    					BRAAGA_TEST_CxtType_eAudio);
	        BDBG_ASSERT(err == BERR_SUCCESS);
	        BDBG_MSG(("\n\nRaaga Audio Context OPEN DONE!!!!"));
	        err = BRAAGA_TEST_OpenRaagaTask(hRaagaTest, 
	    					&hTaskHandle[BRAAGA_TEST_CxtType_eAudio],
	    					BRAAGA_TEST_CxtType_eAudio, 
	    					0, 
	    					BRAAGA_TEST_TaskDstType_eRaveBuf);

			BDBG_MSG(("Audio hAudioTaskHandle=%x hTaskInfo=%x",hTaskHandle[BRAAGA_TEST_CxtType_eAudio],
	            hRaagaTest->hRaagaTestCh[BRAAGA_TEST_CxtType_eAudio]->hTaskInfo[0]));
	    	BDBG_ASSERT(hTaskHandle[BRAAGA_TEST_CxtType_eAudio]);
	        BDBG_ASSERT(err == BERR_SUCCESS);
	        BDBG_MSG(("Audio Raaga Task OPEN DONE!!!! hAudioTaskHandle=%x",hTaskHandle[BRAAGA_TEST_CxtType_eAudio]));

            /* Setup the sample rate change interrupt callbacks */
            {
                BDSP_AudioInterruptHandlers interrupts;

                BKNI_EnterCriticalSection();
                BDSP_AudioTask_GetInterruptHandlers_isr(hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, &interrupts);
                interrupts.sampleRateChange.pCallback_isr = BRAAGA_TEST_ConfigureSampleRate;
                interrupts.sampleRateChange.pParam1 = hRaagaTest->hReg;
                BDSP_AudioTask_SetInterruptHandlers_isr(hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, &interrupts);
                BKNI_LeaveCriticalSection();
            }
			
	    	err = BRAAGA_TEST_StartRaagaTask(hRaagaTest,
	    					hTaskHandle[BRAAGA_TEST_CxtType_eAudio],
	    					BRAAGA_TEST_CxtType_eAudio,
	    					0,
	    					BRAAGA_TEST_TaskDstType_eRaveBuf,
	    					hRaagaTest->bFilePb);
	        BDBG_ASSERT(err == BERR_SUCCESS);
			BDBG_LOG(("raaga_main: Audio BRAAGA_TEST_StartRaagaTask() Done\n\n"));
	    }
	    if(bVidEnable == true)
	    {
	        err = BRAAGA_TEST_OpenRaagaContext(hRaagaTest, 
	    					BRAAGA_TEST_CxtType_eVideo);
	        BDBG_ASSERT(err == BERR_SUCCESS);
	        BDBG_MSG(("Raaga Video Context OPEN DONE!!!!"));
	        
	        err = BRAAGA_TEST_OpenRaagaTask(hRaagaTest, 
	    					&hTaskHandle[BRAAGA_TEST_CxtType_eVideo],
	    					BRAAGA_TEST_CxtType_eVideo, 
	    					0, 
	    					BRAAGA_TEST_TaskDstType_eRaveBuf);
	    	BDBG_MSG(("Video hVideoTaskHandle=%x hTaskInfo=%x",hTaskHandle[BRAAGA_TEST_CxtType_eVideo],
	            hRaagaTest->hRaagaTestCh[BRAAGA_TEST_CxtType_eVideo]->hTaskInfo[0]));        
	    	BDBG_ASSERT(hTaskHandle[BRAAGA_TEST_CxtType_eVideo]);
	        BDBG_ASSERT(err == BERR_SUCCESS);
	        BDBG_MSG(("Video Raaga Task OPEN DONE!!!! hVideoTaskHandle=%x",hTaskHandle[BRAAGA_TEST_CxtType_eVideo]));
	        
	    	err = BRAAGA_TEST_StartRaagaTask(hRaagaTest,
	    					hTaskHandle[BRAAGA_TEST_CxtType_eVideo],
	    					BRAAGA_TEST_CxtType_eVideo,
	    					0,
	    					BRAAGA_TEST_TaskDstType_eRaveBuf,
	    					hRaagaTest->bFilePb);
	        BDBG_ASSERT(err == BERR_SUCCESS);
	        BDBG_MSG(("raaga_main: Video BRAAGA_TEST_StartRaagaTask() Done\n\n"));
	    }

	    while(1)
	    {        

			if(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_ConfigFile || hRaagaTest->eTestmode == BRAAGA_TEST_Mode_Auto)
			{
				BDBG_LOG(("\n####################################################################\n\n"));		
				BDBG_LOG(("\n\nNOW PLAYING STREAM %s\n",hRaagaTest->sRaagaStrParams[0].aStreamName));	
								
				
#if RAAGA_TEST_ENABLE_TRANSPORT
				if(bXptPb == true) 
				{	
					err = BRAAGA_TEST_PbLib_WaitForPlayComplete(hRaagaTest->hPbLib);
					BKNI_Sleep(20000);							
				}			
				else		
#endif
				{		
					BKNI_Sleep(120000);
				}	
				switch(hRaagaTest->sRaagaStrParams[0].eAlgoType)	
				{					
					case BDSP_AudioType_eAc3:	
						err = BDSP_Task_GetStageStatus (hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, 0, 0, &Ac3StreamInfo,sizeof(Ac3StreamInfo));
						BDBG_ASSERT(err == BERR_SUCCESS);	
						BDBG_LOG(("\n\n Total Frames Decoded is : %d",Ac3StreamInfo.ui32TotalFramesDecoded));		
						AutomationTestCase[TestCases_Index].StreamDecodedFrameCount = Ac3StreamInfo.ui32TotalFramesDecoded;	
						break;		
					case BDSP_AudioType_eAc3Plus:		
						err = BDSP_Task_GetStageStatus (hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, 0, 0, &DdpStreamInfo,sizeof(DdpStreamInfo));
						BDBG_ASSERT(err == BERR_SUCCESS);		
						BDBG_LOG(("\n\n Total Frames Decoded is : %d",DdpStreamInfo.ui32TotalFramesDecoded));		
						AutomationTestCase[TestCases_Index].StreamDecodedFrameCount = DdpStreamInfo.ui32TotalFramesDecoded;
						break;			
					case BDSP_AudioType_eMpeg:	
						err = BDSP_Task_GetStageStatus (hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, 0, 0, &MpegStreamInfo,sizeof(MpegStreamInfo));	
						BDBG_ASSERT(err == BERR_SUCCESS);		
						BDBG_LOG(("\n\n Total Frames Decoded is : %d",MpegStreamInfo.ui32TotalFramesDecoded));
						
						AutomationTestCase[TestCases_Index].StreamDecodedFrameCount = MpegStreamInfo.ui32TotalFramesDecoded;	
						break; 		
					case BDSP_AudioType_eAacSbrLoas: 
						err = BDSP_Task_GetStageStatus (hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, 0, 0, &DolbyPulseStreamInfo,sizeof(DolbyPulseStreamInfo));	
						BDBG_ASSERT(err == BERR_SUCCESS);	
						BDBG_LOG(("\n\n Total Frames Decoded is : %d",DolbyPulseStreamInfo.ui32TotalFramesDecoded));	
						AutomationTestCase[TestCases_Index].StreamDecodedFrameCount = DolbyPulseStreamInfo.ui32TotalFramesDecoded;		
						break; 					
					case BDSP_AudioType_eDtsBroadcast:	
						err = BDSP_Task_GetStageStatus (hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, 0, 0, &DtshdStreamInfo,sizeof(DtshdStreamInfo));	
						BDBG_ASSERT(err == BERR_SUCCESS);				
						BDBG_LOG(("\n\n Total Frames Decoded is : %d",DtshdStreamInfo.ui32TotalFramesDecoded));	
						AutomationTestCase[TestCases_Index].StreamDecodedFrameCount = DtshdStreamInfo.ui32TotalFramesDecoded;		
						break; 	
					case BDSP_AudioType_eWmaStd:
						err = BDSP_Task_GetStageStatus (hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, 0, 0, &WmaStdStreamInfo,sizeof(WmaStdStreamInfo));	
						BDBG_ASSERT(err == BERR_SUCCESS);				
						BDBG_LOG(("\n\n Total Frames Decoded is : %d",WmaStdStreamInfo.ui32TotalFramesDecoded));	
						AutomationTestCase[TestCases_Index].StreamDecodedFrameCount = WmaStdStreamInfo.ui32TotalFramesDecoded;		
						break;	
					case BDSP_AudioType_eWmaPro: 
						err = BDSP_Task_GetStageStatus (hTaskHandle[BRAAGA_TEST_CxtType_eAudio]->task, 0, 0, &WmaProStreamInfo,sizeof(WmaProStreamInfo));	
						BDBG_ASSERT(err == BERR_SUCCESS);				
						BDBG_LOG(("\n\n Total Frames Decoded is : %d",WmaProStreamInfo.ui32TotalFramesDecoded));	
						AutomationTestCase[TestCases_Index].StreamDecodedFrameCount = WmaProStreamInfo.ui32TotalFramesDecoded;		
						break;
							
					default: 	
						BDBG_ERR(("\nUNSUPPORTED AUDIO TYPE : ALGO TYPE IS : %d",hRaagaTest->sRaagaStrParams[0].eAlgoType));
						break; 			
				}									
				BDBG_LOG(("\n####################################################################\n\n"));	
				BRAAGA_TEST_Uninit(hRaagaTest);		
				break;  
			}
			if(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_Manual)
	        {
	            printf("Enter the option: audio(0), Video(1) exit(2)\t");
                fflush(stdout);
                
                /* Use the CDB buffer depth polling to stop the decoder on stream end only for audio and in mamual test mode */
                if (bAudEnable == true)
                {
                    short int   flags;
                    char c = 0;
                    uint32_t ui32PrevSize = 0;
                    uint32_t ui32Count = 0;
                    int retval;
                    
                    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
                    
                    while (1)
                    {
                        fcntl(STDIN_FILENO, F_SETFL, flags|O_NONBLOCK);
                        retval = read(STDIN_FILENO, &c, 1);
                        if(retval != -1)
                        {
                            if ((c == '0') || (c == '1') || (c == '2'))
                            {
                                cxtType = c - '0';
                                break;
                            }
                            else
                            {
                                if ((c == '\n'))
                                {
                                    printf("Enter the option: audio(0), Video(1) exit(2)\t");
                                    fflush(stdout);
                                }
                            }
                        }
                        
                        /* Check the input buffer threshold to detect end of stream to stop the decoder */
                        {
                            uint32_t ui32BaseAddr, ui32EndAddr, ui32ReadAddr, ui32WriteAddr, ui32Size;
                            
                            ui32BaseAddr = BREG_Read32(hRaagaTest->hReg, BCHP_XPT_RAVE_CX0_AV_CDB_BASE_PTR);
                            ui32EndAddr = BREG_Read32(hRaagaTest->hReg, BCHP_XPT_RAVE_CX0_AV_CDB_WRAPAROUND_PTR);
                            if (!(ui32EndAddr))
                            {
                                ui32EndAddr = BREG_Read32(hRaagaTest->hReg, BCHP_XPT_RAVE_CX0_AV_CDB_END_PTR);
                            }
                            ui32ReadAddr = BREG_Read32(hRaagaTest->hReg, BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR);
                            ui32WriteAddr = BREG_Read32(hRaagaTest->hReg, BCHP_XPT_RAVE_CX0_AV_CDB_VALID_PTR);
                            
                            ui32Size = ui32WriteAddr - ui32ReadAddr;
                            if (ui32WriteAddr < ui32ReadAddr)
                            {
                                ui32Size = (ui32WriteAddr - ui32BaseAddr) + (ui32EndAddr - ui32ReadAddr) + 1;
                            }
                            
                            if (ui32PrevSize != ui32Size)
                            {
                                ui32PrevSize = ui32Size;
                                ui32Count = 0;
                            }
                            else
                            {
                                ui32Count++;
                            }
                            
                            /* Check for a threshold of 4K data and 5 seconds of stall to stop the decoder */
                            if (ui32Size < 4096)
                            {
                                if (ui32Count > 50)
                                {
                                    BDBG_LOG(("End of stream detected : Stopping the decoder \n"));
                                    cxtType = 2;
                                    break;
                                }
                            }
                            /* If the data present is constant for longer than 15 seconds then stop the decoder regardless of the threshold */
                            else
                            {
                                if (ui32Count > 150)
                                {
                                    BDBG_ERR(("Decoder Stall detected!!! Stopping the decoder \n"));
                                    cxtType = 2;
                                    break;
                                }
                            }
                        }
                        BKNI_Sleep(100);
                    }
                }
                else
                {
	            	scanf("%ud",&cxtType); 
                }
                
	            if(cxtType == 2)
	    		{
	    			BRAAGA_TEST_Uninit(hRaagaTest);
	    			break;
	    		}
	            
	        }
	        
	    	while(1)
	    	{    	
	            if(hRaagaTest->eTestmode != BRAAGA_TEST_Mode_Manual)
	            {
	                cxtType = 0;                   
	            }    
	    	
	            if(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_ArgCmd)
	            {
	        		strcpy(cmd, "quit");
	            }
	            else
	            {
	                RaagaTest_readcmd(cmd);
	            }
	            
	    		if(strcmp(cmd, "stpt") == 0)
	    		{
	    			BRAAGA_TEST_StopRaagaTask(hRaagaTest,hTaskHandle[cxtType]);
	    			BDBG_LOG(("BRAAGA_TEST_StopRaagaTask called"));
	    		}
	    		else if(strcmp(cmd, "clt") == 0)
	    		{
	    			BRAAGA_TEST_CloseRaagaTask(hRaagaTest, hRaagaTest->hRaagaTestCh[cxtType], hTaskHandle[cxtType]);
	    			BDBG_LOG(("BRAAGA_TEST_CloseRaagaTask called"));
	    		}
	    		else if(strcmp(cmd, "clcxt") == 0)
	    		{
	    			BRAAGA_TEST_CloseRaagaContext(hRaagaTest, hRaagaTest->hRaagaTestCh[cxtType]);
	    			BDBG_LOG(("BRAAGA_TEST_CloseRaagaContext called"));
	    		}
	    		else if(strcmp(cmd, "strt") == 0)
	    		{
	    			err = BRAAGA_TEST_StartRaagaTask(hRaagaTest,
	    							hTaskHandle[cxtType],
	    							cxtType,
	    							0,
	    							BRAAGA_TEST_TaskDstType_eRaveBuf,
	    							hRaagaTest->bFilePb);
	    			BDBG_LOG(("BRAAGA_TEST_StartRaagaTask called"));
	    		}
	    		else if(strcmp(cmd, "opt") == 0)
	    		{
	    			err = BRAAGA_TEST_OpenRaagaTask(hRaagaTest, 
	    							&hTaskHandle[cxtType],
	    							cxtType, 
	    							0, 
	    							BRAAGA_TEST_TaskDstType_eRaveBuf);
	    			BDBG_LOG(("BRAAGA_TEST_OpenRaagaTask called"));
	    			BDBG_LOG(("hTaskHandle=%x hTaskInfo=%x",hTaskHandle[cxtType],hRaagaTest->hRaagaTestCh[cxtType]->hTaskInfo[0]));
	    		}		
	    		else if(strcmp(cmd, "opcxt") == 0)
	    		{
	    			err = BRAAGA_TEST_OpenRaagaContext(hRaagaTest, 
	    							cxtType);
	    			BDBG_ASSERT(err == BERR_SUCCESS);
	    			BDBG_LOG(("Raaga Context OPEN DONE!!!!"));
	    			BDBG_LOG(("BRAAGA_TEST_OpenRaagaContext called"));
	    		}
	    		else if(strcmp(cmd, "pause") == 0)
	    		{
	    			err = BDSP_AudioTask_Pause(hTaskHandle[cxtType]->task);
	    			BDBG_LOG(("BDSP_Task_Pause !!!!"));
	    		}
	    		else if(strcmp(cmd, "resume") == 0)
	    		{
	    			err = BDSP_AudioTask_Resume(hTaskHandle[cxtType]->task);
	    			BDBG_LOG(("BDSP_Task_Resume!!!!"));
	    		}
	    		else if(strcmp(cmd, "advance") == 0)
	    		{
	    			unsigned int		ms=0;
	    			err = BDSP_AudioTask_Advance(hTaskHandle[cxtType]->task, ms);
	    			BDBG_LOG(("BDSP_Task_Advance!!!!"));
	    		}		
	    		else if(strcmp(cmd, "settsm") == 0)
	    		{
	    			BDSP_AudioTaskTsmSettings 	sTsmSettings;
	    			unsigned				branchId, stageId;
	    			printf("Enter branch Id\t");
	    			scanf("%ud",&branchId);
	    			printf("Enter stage Id\t");
	    			scanf("%ud",&stageId);
	    			
	    			BKNI_EnterCriticalSection();
	    			err = BDSP_AudioTask_GetTsmSettings_isr(hTaskHandle[cxtType]->task, branchId , stageId , &sTsmSettings);
	    			sTsmSettings.eTsmEnable = BDSP_Raaga_eTsmBool_True;
	    			err = BDSP_AudioTask_SetTsmSettings_isr(hTaskHandle[cxtType]->task, branchId , stageId , &sTsmSettings);
	    			BKNI_LeaveCriticalSection();
	    		}
	    		else if(strcmp(cmd, "unsettsm") == 0)
	    		{
	    			BDSP_AudioTaskTsmSettings 	sTsmSettings;
	    			unsigned				branchId, stageId;
	    			printf("Enter branch Id\t");
	    			scanf("%ud",&branchId);
	    			printf("Enter stage Id\t");
	    			scanf("%ud",&stageId);
	    			
	    			BKNI_EnterCriticalSection();
	    			err = BDSP_AudioTask_GetTsmSettings_isr(hTaskHandle[cxtType]->task, branchId , stageId , &sTsmSettings);
	    			sTsmSettings.eTsmEnable = BDSP_Raaga_eTsmBool_False;
	    			err = BDSP_AudioTask_SetTsmSettings_isr(hTaskHandle[cxtType]->task, branchId , stageId , &sTsmSettings);
	    			BKNI_LeaveCriticalSection();
	    		}
	    		else if(strcmp(cmd, "setstc") == 0)
	    		{
	    			BDSP_AudioTaskTsmSettings 	sTsmSettings;
	    			unsigned				branchId, stageId;
	    			printf("Enter branch Id\t");
	    			scanf("%ud",&branchId);
	    			printf("Enter stage Id\t");
	    			scanf("%ud",&stageId);
	    			
	    			BKNI_EnterCriticalSection();
	    			err = BDSP_AudioTask_GetTsmSettings_isr(hTaskHandle[cxtType]->task, branchId , stageId , &sTsmSettings);
	    			sTsmSettings.eSTCValid = BDSP_Raaga_eTsmBool_True;
	    			err = BDSP_AudioTask_SetTsmSettings_isr(hTaskHandle[cxtType]->task, branchId , stageId , &sTsmSettings);
	    			BKNI_LeaveCriticalSection();
	    		}
	    		else if(strcmp(cmd, "setstgsetg") == 0)
	    		{
	    			BDSP_Raaga_Audio_DDPMultiStreamConfigParams	config;
	    			unsigned				branchId, stageId;
	    			printf("Enter branch Id\t");
	    			scanf("%ud",&branchId);
	    			printf("Enter stage Id\t");
	    			scanf("%ud",&stageId);
	    			
	    			BDSP_Task_GetStageSettings(
	    					hTaskHandle[cxtType]->task, branchId, stageId, &config, 
	    					sizeof(BDSP_Raaga_Audio_DDPMultiStreamConfigParams));
	    			BDSP_Task_SetStageSettings(
	    					hTaskHandle[cxtType]->task, branchId, stageId, &config, 
	    					sizeof(BDSP_Raaga_Audio_DDPMultiStreamConfigParams));

	    		}		
	    		else if(strcmp(cmd, "quit") == 0)
	    		{
	    			break;
	    		}
	    		else
	    		{
	    			RaagaTest_PrintCmd();
	    		}
	    	}
			if((strcmp(cmd, "quit") == 0)&&(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_ArgCmd))
			{
	            BRAAGA_TEST_Uninit(hRaagaTest);
				break;
			}

	    }
	    /* Quit the Application */
		printf("\n\n FINISHED %d testcases:",TestCases_Index);
		TestCases_Index++;
	}
	printf ("\n Quitting RAAGA TEST ...\n\n");

    if (g_capture_encoder_out || g_capture_decoder_stereo || g_capture_decoder_multich)
    {
        /* Stop all captures */
        BRGA_TST_Capture_RemoveAll();
        BDBG_LOG(("All captures disabled"));
        /* TBD: Why does this function block ? */
        /*BTST_DestroyTask(pcaptureInfo->hCaptureTask);*/ 
    }

    if (g_enable_watchdog)
    {
        /*BTST_DestroyTask( watchdogTask);*/
    }

    
	if(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_ConfigFile || hRaagaTest->eTestmode == BRAAGA_TEST_Mode_Auto)
	{
		printf("\n***********************************************************************");
		printf("\n|%25s%s%25s|"," ","TEST REPORT SUMMARY"," ");
		/*printf("\n**********************************************************************");*/
		printf("\n-----------------------------------------------------------------------");
		printf("\n|%-10s|%-25s|%9s|%9s|%8s|","Algo Type"," STREAM NAME "," REF_FRAME "," ACT_FRAME "," RESULT ");
		printf("\n-----------------------------------------------------------------------");

		TestCases_Index=0;
		while (TestCases_Index<TotalTestCases)
		{	
			
			/*Check the difference between Decoded Frames and Actual frames. Difference upto 1 frame count is acceptable.*/
			if ( abs(AutomationTestCase[TestCases_Index].StreamDecodedFrameCount - AutomationTestCase[TestCases_Index].StreamRefFrameCount) <= 1)
				TestCaseResult = "Pass";
			else	
				TestCaseResult = "Fail";
			if((ch = strrchr(AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName,'/')) != NULL)
			{
				printf("\n| 0x%-7x|%-25s| %09d | %09d | %6s |",AutomationTestCase[TestCases_Index].TestStreamInfo.eAlgoType,ch,AutomationTestCase[TestCases_Index].StreamRefFrameCount,							
				AutomationTestCase[TestCases_Index].StreamDecodedFrameCount,TestCaseResult);		
			}
			else
			{
				printf("\n| 0x%-7x|%-25s| %09d | %09d | %6s |",AutomationTestCase[TestCases_Index].TestStreamInfo.eAlgoType,AutomationTestCase[TestCases_Index].TestStreamInfo.aStreamName,AutomationTestCase[TestCases_Index].StreamRefFrameCount,							
				AutomationTestCase[TestCases_Index].StreamDecodedFrameCount,TestCaseResult);
			}
			
			TestCases_Index++;	
		}
		printf("\n-----------------------------------------------------------------------");
		printf("\n***********************************************************************\n\n");
	}
	
	if (hRaagaTest)
    {
        BKNI_Free (hRaagaTest);
        hRaagaTest = NULL;
    }

    if (g_enable_watchdog)
    {
        g_enable_watchdog = false;
        BRGA_TST_DisbleWatchdogTimer();
    }
    
    return 0;
}


/***************************************************************************
 * main
 ***************************************************************************/
int app_main( int argc, char **argv )
{
	int             iErr;
	BSystem_Info    sysInfo;
	BFramework_Info frmInfo;

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
    
    /* Configure the Board/FMM related items here. Since we do not use the PI
    we need to write directly to the registers. To limit such config we add only 
    the most basic path in FMM. In this case it would be SRCCH->HIFIDAC */

    /* Board Config / UART setup etc */
	RAAGA_TEST_ProgramGpio(frmInfo.hReg);

    /* Not required as we are doing just a SRCCH->HIFIDAC.
    Keeping the code as it might be required in future */
#if 0    
	BRAAGA_TEST_ProgramGenClk(frmInfo.hReg);
	BRAAGA_TEST_ProgrameVCXO(frmInfo.hReg);
	BRAAGA_TEST_ProgramFmmMisc(frmInfo.hReg);
	BRAAGA_TEST_ProgramMS(frmInfo.hReg);
	BRAAGA_TEST_ProgramOP(frmInfo.hReg);
	BRAAGA_TEST_ProgramDP(frmInfo.hReg);
	BRAAGA_TEST_GroupIOP(frmInfo.hReg);
	BRAAGA_TEST_ProgramSRC(frmInfo.hReg);
#endif    

    BRAAGA_TEST_ProgramBF(frmInfo.hReg);        /* set SRCCH properly */
	BRAAGA_TEST_ProgramHifiDac(frmInfo.hReg);   /* set HIFIDAC properly */
	BRAAGA_TEST_EnableFmmPath(frmInfo.hReg);    /* for the Connections */


    /* call raaga main function now */
    raaga_main(argc, argv, &frmInfo);	

    return 0;
}

