/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test_video.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/14/12 3:04p $
 *
 * Module Description:
 *          This file contains all structures and function prototypes used 
 *          by raaga_test.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/raaga_test_video.c $
 * 
 * Hydra_Software_Devel/1   8/14/12 3:04p rshijith
 * FWRAA-478: Adding video encoder related files to raaga
 * 
 * Hydra_Software_Devel/H264_enc_devel/2   11/7/11 9:39a ashoky
 * SW7231-344: Adding changes in the interface after discussion.
 * 
 * Hydra_Software_Devel/H264_enc_devel/1   9/20/11 7:08p ashoky
 * SW7231-344: Adding initial code changes for video encoder on raaga as
 * per first design.
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
//#include "btst_kni.h"
//#include "btst.h"
#include "bdsp_raaga_fw.h"
//#include "data_feeder.h"
#include "raaga_test_video.h"

#define GISB_TIMEOUT_ENABLED 0

#if GISB_TIMEOUT_ENABLED
#include "bchp_sun_gisb_arb.h"
#include "bchp_sun_l2.h"
#define BCHP_INT_ID_GISB_TIMEOUT_INTR         BCHP_INT_ID_CREATE(BCHP_SUN_L2_CPU_STATUS, BCHP_SUN_L2_CPU_STATUS_GISB_TIMEOUT_INTR_SHIFT)
#if BCHP_CHIP == 7422 || BCHP_CHIP == 7425
static const char *g_pGisbMasters[] =
{    
	"SSP",
	"CPU",
	"reserved",
	"pcie",
	"BSP",
	"RDC",
	"raaga",
	"reserved",
	"AVD1",
	"JTAG",
	"SVD0",
	"reserved",
	"VICE0",
	"reserved"
};

static BINT_CallbackHandle g_gisbInterrupt;

#endif 

#endif 

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
    BRAAGA_TEST_CxtType_eMax,
	BRAAGA_TEST_eTaskType_eDecode	/* TaskType */
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

#if GISB_TIMEOUT_ENABLED
static void RAAGA_TEST_P_GisbTimeout_isr(void *pParam, int iParam)
{
    uint32_t reg;
	BREG_Handle regHandle = pParam;
	reg = BREG_Read32(regHandle, BCHP_SUN_GISB_ARB_ERR_CAP_STATUS);
	if ( reg & BCHP_MASK(SUN_GISB_ARB_ERR_CAP_STATUS, valid) )
	{
		bool writing = (reg & BCHP_MASK(SUN_GISB_ARB_ERR_CAP_STATUS, write))?true:false;
		const char *pCore = "Unknown";
		unsigned i;
		reg = BREG_Read32(regHandle, BCHP_SUN_GISB_ARB_ERR_CAP_MASTER);
		for ( i = 0; i < sizeof(g_pGisbMasters)/sizeof(const char *); i++ )
		{
			if ( reg & 0x1 )
			{
				pCore = g_pGisbMasters[i];
				break;
			}
			reg >>= 1;
		}
		reg = BREG_Read32(regHandle, BCHP_SUN_GISB_ARB_ERR_CAP_ADDR);
		BDBG_ERR(("*****************************************************"));
		BDBG_ERR(("GISB Timeout %s addr 0x%08x by core %s", (writing)?"writing":"reading", reg, pCore));
		if (iParam) 
		{
			BDBG_ERR(("This GISB timeout occured before nexus started."));
		}
		BDBG_ERR(("*****************************************************"));
	}
	/* Clear error status */
	BREG_Write32(regHandle, BCHP_SUN_GISB_ARB_ERR_CAP_CLR, 0x1);

}
#endif 

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
				strcpy(hRaagaTest->sRaagaStrParams[0].aStreamName, argv[argCnt]);
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
			if(strcmp(argv[argCnt], "-resIp") == 0)
			{
				argCnt++;
				hRaagaTest->sVidEncodeParams.ui32IpWidth	= atoi(argv[argCnt]);
				argCnt++;
				hRaagaTest->sVidEncodeParams.ui32IpHeight	= atoi(argv[argCnt]);
				BDBG_ERR(("Input Resolution is %d X %d", hRaagaTest->sVidEncodeParams.ui32IpWidth, hRaagaTest->sVidEncodeParams.ui32IpHeight));
			}
			if(strcmp(argv[argCnt], "-resOp") == 0)
			{
				argCnt++;
				hRaagaTest->sRaagaStrParams[i].ui32OpWidth	= atoi(argv[argCnt]);
				argCnt++;
				hRaagaTest->sRaagaStrParams[i].ui32OpHeight	= atoi(argv[argCnt]);
				BDBG_ERR(("Output Resolution is %d X %d", hRaagaTest->sRaagaStrParams[i].ui32OpWidth, hRaagaTest->sRaagaStrParams[i].ui32OpHeight));
		}

		}
#if 0
		if(!(hRaagaTest->sRaagaStrParams[i].uiPcrPid))
		{
			BDBG_ERR(("RaagaTest_ParseCmd: command is not correct"));
			BDBG_ERR(("Enter command as e.g."));
			BDBG_ERR(("raaga_test -cmd -strnum 1 -path /mnt/nfs/difmus1_ac3_11_14.ts -pcr 17 -audio 20 -audiotype 5 -pkttype 0"));
			return BERR_INVALID_PARAMETER;
		}
#endif 
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
    bool                        bVidEnable=false;
	bool						bVidEncode=false;
    bool                        bAudEnable=false;    
    char 						comd = 'n';
	BDSP_Raaga_VideoBH264UserConfig userConfig;
	
#if BTST_P_USE_MRC_MONITOR
		BMEM_MonitorInterface hMonitorInterface;	
		BMRC_Monitor_Settings monitorSettings;
#endif

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
    {
        for (argCnt=1; argCnt<(unsigned int)argc; argCnt++)
        {
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
				bXptPb = true;
			}
			else if(strcmp(argv[argCnt], "-cmd") == 0)
			{
				testmode = BRAAGA_TEST_Mode_ArgCmd;
			}
			else if(strcmp(argv[argCnt], "-vid") == 0)
			{
				bVidEnable = true;
			} 
			else if(strcmp(argv[argCnt], "-videnc") == 0)
			{
				bVidEncode = true;
				/* testmode = BRAAGA_TEST_Mode_ArgCmd; */
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
                printf ("\n  -m        : Forces Manual Mode. Required for all options below");
                printf ("\n  -a        : Forces Auto Mode.");
                printf ("\n  -pb       : To Enable play pump.");
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
	BDBG_ERR(("hRaagaTest->hMem=%x frmInfo->hMem=%x",hRaagaTest->hMem,frmInfo->hMem));
	hGlobalRaagaTestHandle = hRaagaTest;
	g_hChip = frmInfo->hChp;
	g_hMemory = frmInfo->hMem;
	g_hInterrupt = frmInfo->hInt;
	g_hRegister = frmInfo->hReg;
    g_hTmr = frmInfo->hTmr;
    hRaagaTest->eTestmode = testmode;
    hRaagaTest->uiNumStreams = 1;

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
	hRaagaTest->bAudEnable = bAudEnable;
	hRaagaTest->bVidEnable = bVidEnable;
	hRaagaTest->bVidEncodeEnable = bVidEncode;

    switch (hRaagaTest->eTestmode)
    {
        case BRAAGA_TEST_Mode_Manual:
            /* Get Parameters for manual mode from user */
            printf ("\n Manual Mode is Enabled !\n");
            BRAAGA_TEST_ManualTestMenu (hRaagaTest);
            break;

        case BRAAGA_TEST_Mode_ConfigFile:
            /* Get Parameters for the config file passed */
            printf ("\n Config File Mode is Enabled !\n");
			/* BRAAGA_TEST_GetParamsFromConfig (hRaagaTest, hRaagaTestCh, argv[1]); */
            break;
		case BRAAGA_TEST_Mode_Auto:
		{
			BDBG_ERR(("BRAAGA_TEST_Mode_Auto: taking auto params"));
			hRaagaTest->uiNumStreams = 1;
			hRaagaTest->sRaagaStrParams[0].eStreamType = BAVC_StreamType_eTsMpeg;
			hRaagaTest->sRaagaStrParams[0].uiAudioPid = 0x14;
			hRaagaTest->sRaagaStrParams[0].uiPcrPid = 0x11;
			hRaagaTest->sRaagaStrParams[0].eAlgoType = BDSP_AudioType_eAc3;
			hRaagaTest->sRaagaStrParams[0].bUsed = true;
			strcpy(hRaagaTest->sRaagaStrParams[0].aStreamName, "./difmus1_ac3_11_14.ts");
			break;
		}
		case BRAAGA_TEST_Mode_ArgCmd:
		{
			RaageTest_ParseCmd(hRaagaTest, argc, argv);
		}
        default:
            printf ("\n Unsupported Mode");
    }

#if RAAGA_TEST_ENABLE_TRANSPORT
	BDBG_ERR(("hChip =%x hMemory=%x hInt=%x hReg=%x hRaagaTestCh=%x hRaagaTest=%x",
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

#if BTST_P_USE_MRC_MONITOR	
	BMRC_Open(&hRaagaTest->hMrc, hRaagaTest->hReg, hRaagaTest->hInt, NULL);
	BMRC_Monitor_GetDefaultSettings(&monitorSettings);
	BMRC_GetMaxCheckers(hRaagaTest->hMrc, &monitorSettings.ulNumCheckersToUse);
	/* Control all pmem window */
	BMRC_Monitor_Open(&hRaagaTest->hMonitor, hRaagaTest->hReg, hRaagaTest->hInt, hRaagaTest->hChp, hRaagaTest->hMrc, 0, DRAM_SIZE, &monitorSettings);
	BMRC_Monitor_GetMemoryInterface(hRaagaTest->hMonitor, &hMonitorInterface);
	BMEM_InstallMonitor(hRaagaTest->hMem, &hMonitorInterface);
#endif

#if GISB_TIMEOUT_ENABLED
    /* This can produce a lot of console output if a block is failing. */
    /* Reduce GISB ARB timer to a small enough value for the CPU to trap errors.
    100 milliseconds is preferred by the HW architecture team for DTV/STB systems. */
	BREG_Write32(hRaagaTest->hReg, BCHP_SUN_GISB_ARB_TIMER, 10800000);	 /* 100 milliseconds @ 108 MHz */
    /* INT is valid now.  Register for GISB interrupt to see if anyone is accessing invalid registers */
    err = BINT_CreateCallback(&g_gisbInterrupt, hRaagaTest->hInt, BCHP_INT_ID_GISB_TIMEOUT_INTR, RAAGA_TEST_P_GisbTimeout_isr, hRaagaTest->hReg, 0);
    if (err) {
        err = BERR_TRACE(err);
    }
    else {
        /* Poll for a pending bad access */
        BKNI_EnterCriticalSection();

	    RAAGA_TEST_P_GisbTimeout_isr(hRaagaTest->hReg, 1);
		
        BKNI_LeaveCriticalSection();
        /* Enable the interrupt */
        err = BINT_EnableCallback(g_gisbInterrupt);
        if (err) {
             err = BERR_TRACE(err);
        }
    }

#endif 


	if(true == bVidEncode)
	{
		BDBG_MSG(("Init Video Encode Feeder task "));
		hRaagaTest->hFeeder	= BRAAGA_TEST_Feeder_Init(
											hRaagaTest->hChp,
											hRaagaTest->hMem,
											hRaagaTest->hInt,
											hRaagaTest->hReg
											);
		BDBG_MSG(("Done Init of Video Encoder feeder "));
		hRaagaTest->hFeeder->sPicParams.ui32IpPicHeight = hRaagaTest->sVidEncodeParams.ui32IpHeight;
		hRaagaTest->hFeeder->sPicParams.ui32IpPicWidth = hRaagaTest->sVidEncodeParams.ui32IpWidth;

		/* All cfg cmd has been taken input */
		hRaagaTest->eTestmode = BRAAGA_TEST_Mode_ArgCmd;
	}

	BDBG_MSG(("Calling BRAAGA_TEST_DspInit()"));
    err = BRAAGA_TEST_DspInit(hRaagaTest,
    							hRaagaTest->hChp,
    							hRaagaTest->hMem,
    							hRaagaTest->hInt,
    							hRaagaTest->hReg,
    							hRaagaTest->hTmr);
    BDBG_ASSERT(err == BERR_SUCCESS);

	if((bAudEnable == true))
    {
		if(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_ArgCmd)
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
        
    	err = BRAAGA_TEST_StartRaagaTask(hRaagaTest,
    					hTaskHandle[BRAAGA_TEST_CxtType_eAudio],
    					BRAAGA_TEST_CxtType_eAudio,
    					0,
    					BRAAGA_TEST_TaskDstType_eRaveBuf,
    					hRaagaTest->bXptPbEn);
        BDBG_ASSERT(err == BERR_SUCCESS);
        BDBG_MSG(("raaga_main: Audio BRAAGA_TEST_StartRaagaTask() Done\n\n"));
    }
    }
    if((bVidEnable == true) || (hRaagaTest->bVidEncodeEnable == true))
    {
	    if(hRaagaTest->bVidEncodeEnable == true)
	    {
        err = BRAAGA_TEST_OpenRaagaContext(hRaagaTest, 
    						BRAAGA_TEST_CxtType_eVideoEncode);
        BDBG_ASSERT(err == BERR_SUCCESS);
			hRaagaTest->hRaagaTestCh[0]->sSettings.eTaskType = BRAAGA_TEST_eTaskType_eEncode;

			BDBG_MSG(("Raaga Video Context OPEN DONE!!!!"));
			
        err = BRAAGA_TEST_OpenRaagaTask(hRaagaTest, 
							&hTaskHandle[BRAAGA_TEST_CxtType_eVideoEncode],
							BRAAGA_TEST_CxtType_eVideoEncode, 
    					0, 
    					BRAAGA_TEST_TaskDstType_eRaveBuf);

			BDBG_ASSERT(hTaskHandle[BRAAGA_TEST_CxtType_eVideoEncode]);
			BDBG_MSG(("Video Encode hVideoTaskHandle=%x hTaskInfo=%x",hTaskHandle[BRAAGA_TEST_CxtType_eVideoEncode],
				hRaagaTest->hRaagaTestCh[BRAAGA_TEST_CxtType_eVideoEncode]->hTaskInfo[0]));
			BDBG_MSG(("Video Raaga Task OPEN DONE!!!! hVideoTaskHandle=%x",hTaskHandle[BRAAGA_TEST_CxtType_eVideoEncode]));
			
		/* We got the DspTask Handle here. Assign to feederTask for it's use */
		hRaagaTest->hFeeder->hDspTask	= hRaagaTest->hRaagaTestCh[0]->hTaskInfo[0]->task;

		/* Set any user config param before starting the task */
			BDBG_MSG(("Getting video encode user config "));
		err = BDSP_Task_GetStageSettings(hRaagaTest->hRaagaTestCh[0]->hTaskInfo[0]->task, 0, 0, &userConfig, sizeof(userConfig));
		if(err != BERR_SUCCESS)
		{
			BERR_TRACE(err);
		}
			userConfig.ui32TargetBitRate = hRaagaTest->sVidEncodeParams.ui32TargetBitRate;
			userConfig.eDblkEnable = (BDSP_Raaga_Video_eBOOLEAN)hRaagaTest->sVidEncodeParams.bDblkEnable;
			userConfig.eGopStruct = (BDSP_Raaga_VideoGopStruct)hRaagaTest->sVidEncodeParams.ui32GopStruct;
			userConfig.eProfileIDC = (BDSP_Raaga_VideoH264ProfileIdc)hRaagaTest->sVidEncodeParams.ui32Profile;
			userConfig.ui32LevelIdc = hRaagaTest->sVidEncodeParams.ui32Level;
			userConfig.ui32IntraPeriod = hRaagaTest->sVidEncodeParams.ui32IntraPeriod;
			userConfig.ui32IDRPeriod = hRaagaTest->sVidEncodeParams.ui32IdrPeriod;
			userConfig.eRateControlEnable = (BDSP_Raaga_Video_eBOOLEAN)hRaagaTest->sVidEncodeParams.bRateCtrlEnable;
			userConfig.ui32EncodPicHeight = hRaagaTest->sVidEncodeParams.ui32IpHeight;
			userConfig.ui32EncodPicWidth = hRaagaTest->sVidEncodeParams.ui32IpWidth;

			BDBG_MSG(("Setting video encode user config "));
		err = BDSP_Task_SetStageSettings(hRaagaTest->hRaagaTestCh[0]->hTaskInfo[0]->task, 0, 0, &userConfig, sizeof(userConfig));		
		if(err != BERR_SUCCESS)
		{
			BERR_TRACE(err);
		}
        
			BDBG_MSG(("Starting Raaga Test video encode task "));
			err = BRAAGA_TEST_StartRaagaTask(hRaagaTest,
							hTaskHandle[BRAAGA_TEST_CxtType_eVideoEncode],
							BRAAGA_TEST_CxtType_eVideoEncode,
							0,
							BRAAGA_TEST_TaskDstType_eRaveBuf,
							hRaagaTest->bXptPbEn);
			
			BDBG_ASSERT(err == BERR_SUCCESS);
			BDBG_MSG(("raaga_main: Video BRAAGA_TEST_StartRaagaTask() Done\n\n"));
			
	    }
		else
		{
        	err = BRAAGA_TEST_OpenRaagaContext(hRaagaTest, 
    						BRAAGA_TEST_CxtType_eVideo);
			BDBG_ASSERT(err == BERR_SUCCESS);
			BDBG_MSG(("Raaga Video Encode Context OPEN DONE!!!!"));
			
			err = BRAAGA_TEST_OpenRaagaTask(hRaagaTest, 
							&hTaskHandle[BRAAGA_TEST_CxtType_eVideo],
							BRAAGA_TEST_CxtType_eVideo, 
							0, 
							BRAAGA_TEST_TaskDstType_eRaveBuf);
			
			BDBG_ASSERT(hTaskHandle[BRAAGA_TEST_CxtType_eVideo]);
			BDBG_MSG(("Video hVideoTaskHandle=%x hTaskInfo=%x",hTaskHandle[BRAAGA_TEST_CxtType_eVideo],
				hRaagaTest->hRaagaTestCh[BRAAGA_TEST_CxtType_eVideo]->hTaskInfo[0]));
			BDBG_MSG(("Video Raaga Task OPEN DONE!!!! hVideoTaskHandle=%x",hTaskHandle[BRAAGA_TEST_CxtType_eVideo]));
			
			BDBG_MSG(("Starting Raaga Test video task "));
    	err = BRAAGA_TEST_StartRaagaTask(hRaagaTest,
    					hTaskHandle[BRAAGA_TEST_CxtType_eVideo],
    					BRAAGA_TEST_CxtType_eVideo,
    					0,
    					BRAAGA_TEST_TaskDstType_eRaveBuf,
    					hRaagaTest->bXptPbEn);

        BDBG_ASSERT(err == BERR_SUCCESS);
        BDBG_MSG(("raaga_main: Video BRAAGA_TEST_StartRaagaTask() Done\n\n"));
    }

    }

    while(1)
    {        
        if(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_Manual)
        {
            printf("Enter the option: audio(0), Video(1) exit(2)\t");
            scanf("%ud",&cxtType); 
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
				/* printf("'q' to quit>");
				scanf("%c", &comd); */
        		strcpy(cmd, "quit");
            }
            else
            {
                RaagaTest_readcmd(cmd);
            }
            
    		if(strcmp(cmd, "stpt") == 0)
    		{
    			BRAAGA_TEST_StopRaagaTask(hRaagaTest,hTaskHandle[cxtType]);
    			BDBG_ERR(("BRAAGA_TEST_StopRaagaTask called"));
    		}
    		else if(strcmp(cmd, "clt") == 0)
    		{
    			BRAAGA_TEST_CloseRaagaTask(hRaagaTest, hRaagaTest->hRaagaTestCh[cxtType], hTaskHandle[cxtType]);
    			BDBG_ERR(("BRAAGA_TEST_CloseRaagaTask called"));
    		}
    		else if(strcmp(cmd, "clcxt") == 0)
    		{
    			BRAAGA_TEST_CloseRaagaContext(hRaagaTest, hRaagaTest->hRaagaTestCh[cxtType]);
    			BDBG_ERR(("BRAAGA_TEST_CloseRaagaContext called"));
    		}
    		else if(strcmp(cmd, "strt") == 0)
    		{
    			err = BRAAGA_TEST_StartRaagaTask(hRaagaTest,
    							hTaskHandle[cxtType],
    							cxtType,
    							0,
    							BRAAGA_TEST_TaskDstType_eRaveBuf,
    							hRaagaTest->bXptPbEn);
    			BDBG_ERR(("BRAAGA_TEST_StartRaagaTask called"));
    		}
    		else if(strcmp(cmd, "opt") == 0)
    		{
    			err = BRAAGA_TEST_OpenRaagaTask(hRaagaTest, 
    							&hTaskHandle[cxtType],
    							cxtType, 
    							0, 
    							BRAAGA_TEST_TaskDstType_eRaveBuf);
    			BDBG_ERR(("BRAAGA_TEST_OpenRaagaTask called"));
    			BDBG_ERR(("hTaskHandle=%x hTaskInfo=%x",hTaskHandle[cxtType],hRaagaTest->hRaagaTestCh[cxtType]->hTaskInfo[0]));
    		}		
    		else if(strcmp(cmd, "opcxt") == 0)
    		{
    			err = BRAAGA_TEST_OpenRaagaContext(hRaagaTest, 
    							cxtType);
    			BDBG_ASSERT(err == BERR_SUCCESS);
    			BDBG_ERR(("Raaga Context OPEN DONE!!!!"));
    			BDBG_ERR(("BRAAGA_TEST_OpenRaagaContext called"));
    		}
    		else if(strcmp(cmd, "pause") == 0)
    		{
    			err = BDSP_AudioTask_Pause(hTaskHandle[cxtType]->task);
    			BDBG_ERR(("BDSP_Task_Pause !!!!"));
    		}
    		else if(strcmp(cmd, "resume") == 0)
    		{
    			err = BDSP_AudioTask_Resume(hTaskHandle[cxtType]->task);
    			BDBG_ERR(("BDSP_Task_Resume!!!!"));
    		}
    		else if(strcmp(cmd, "advance") == 0)
    		{
    			unsigned int		ms=0;
    			err = BDSP_AudioTask_Advance(hTaskHandle[cxtType]->task, ms);
    			BDBG_ERR(("BDSP_Task_Advance!!!!"));
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

    		}/*
    		else if(strcmp(cmd, "quit") == 0)
    		{
    			break;
    		}
			*/
			else if ((comd == 'q') || (hRaagaTest->hFeeder->Running == false))
			{
				break;
			}
    		else
    		{
    			/* RaagaTest_PrintCmd(); */
    		}
    	}
		if((strcmp(cmd, "quit") == 0)&&(hRaagaTest->eTestmode == BRAAGA_TEST_Mode_ArgCmd))
		{
            BRAAGA_TEST_Uninit(hRaagaTest);
			break;
		}

    }
	
#if (BTST_P_USE_MRC_MONITOR)	
	BMRC_Monitor_Close(hRaagaTest->hMonitor);
	BMRC_Close(hRaagaTest->hMrc);
#endif 

    /* Quit the Application */
    printf ("\n Quitting RAAGA TEST ...\n\n");

    if (hRaagaTest)
    {
        BKNI_Free (hRaagaTest);
        hRaagaTest = NULL;
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

