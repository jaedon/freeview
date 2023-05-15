#include <bstd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_dsc.h"
#include "csd_harness_test_dsc_private.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "csd_harness_test_ca_pvr.h"
#include "nexus_platform.h"

static void csd_harness_test_dsc_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_dsc_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_DSC_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void csd_harness_test_dsc_dump_paramters (CSD_Harness_Test_DSC_Parameters * pParams)
{
	printf("CSD_Harness_Test_DSC_Parameters is :\n");

	printf("videoPid: 0x%x\n", pParams->videoPid);
	printf("audioPid 0x%x\n", pParams->audioPid);
	printf("pcrPid 0x%x\n", pParams->pcrPid);
	printf("bEcmRequired %d\n", pParams->bEcmRequired);
	printf("ecmPid 0x%x\n", pParams->ecmPid);
	printf("emi %x\n", pParams->emi);
	printf("sizeIV %d\n", pParams->sizeIV);
	printf("IV array is:\n");
	nocs3_harness_test_utility_dump_array (pParams->iv, pParams->sizeIV);
	printf("pInFile %p\n", (void *)pParams->pInFile);
	printf("pOutFile %p\n", (void *)pParams->pOutFile);
	printf("pTvFile %p\n", (void *)pParams->pTvFile);
}
#endif

void csd_harness_test_dsc (int argc, char* argv[])
{
	CSD_Harness_Test_DSC_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NOCS3_Harness_Test_Test_Vector test_vector, * pVector = &test_vector;

 	TCsdInitParameters csdInit;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsd8BytesVector nuid;
	TCsdDscKeyPathHandle keyPathHandle_video;
	TCsdDscKeyPathHandle keyPathHandle_audio;
	CSD_Harness_Test_CA_PVR_Configuration caPvrConfig, *pCaPvrConfig = &caPvrConfig;
	NEXUS_PlatformSettings platformSettings;

	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		csd_harness_test_dsc_help();
		return;
	}

	memset (&params, 0, sizeof (CSD_Harness_Test_DSC_Parameters));
	if ( (nocs3_status=csd_harness_test_dsc_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("DSC command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up_1;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	csd_harness_test_dsc_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	
	status=csdInitialize(&csdInit);
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		CSD_ERR_TRACE (status, "csdInitialize", "Cannot initialize CSD");
		goto Clean_Up_1;
	}		

	status = csdGetNuid64(nuid);
	if (status!=CSD_NO_ERROR) 
	{
		CSD_ERR_TRACE (status, "csdGetNuid64", "Cannot find NUID");
		goto Clean_Up;
	}	
	
	nocs3_harness_test_utility_dump_array(nuid, 8);
	keyPathHandle_video.initHandle = &csdInit;
	keyPathHandle_audio.initHandle = &csdInit;

	/* Get test vector */
	memset (pVector, 0, sizeof (NOCS3_Harness_Test_Test_Vector));
	if ( (nocs3_status=nocs3_harness_test_get_test_vector_4_nuid (nuid, pVector, params.pTvFile)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE("Cannot find required test vector.");
		goto Clean_Up;
	}

	
#ifdef NOCS3_DEBUG_MESSAGE
	nocs3_harness_test_dump_test_vector (pVector);
#endif

	pCaPvrConfig->bIsDsc = true;
	pCaPvrConfig->bEncryption = false;
	pCaPvrConfig->emi = params.emi;
	pCaPvrConfig->audioPID = params.audioPid;
	pCaPvrConfig->videoPID = params.videoPid;
	pCaPvrConfig->pcrPID = params.pcrPid;
	pCaPvrConfig->bEcmEnabled = params.bEcmRequired;
	pCaPvrConfig->ecmPID = params.ecmPid;
	pCaPvrConfig->pDscVideoKeyHandle = &keyPathHandle_video;
	pCaPvrConfig->pDscAudioKeyHandle = &keyPathHandle_audio;
	pCaPvrConfig->pInFile = params.pInFile;
	pCaPvrConfig->pOutFile = params.pOutFile;
	pCaPvrConfig->pVector = pVector;
	pCaPvrConfig->sizeIV = params.sizeIV;
	if ( pCaPvrConfig->sizeIV != 0 )
	{
		memcpy (pCaPvrConfig->iv, params.iv, (pCaPvrConfig->sizeIV)*sizeof(unsigned char));
	}

	nocs3_status = csd_harness_test_ca_pvr (pCaPvrConfig);
	if ( nocs3_status != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE("DSC test failed");
	}

Clean_Up:
	/* Clean up */
  	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);


Clean_Up_1:
	if (params.pInFile)
	{
		fclose (params.pInFile);
	}
	if (params.pOutFile)
	{
		fclose (params.pOutFile);
	}
	if (params.pTvFile)
	{
		fclose (params.pTvFile);
	}

    fflush(stdout);

    NEXUS_Platform_Uninit();
}


static void csd_harness_test_dsc_help (void)
{
	printf("\t%s\n\n", CSD_HELP_M_DSC_INST);
	printf("\t\t%s\n\n", CSD_HELP_M_IVC);
	printf("\t\t%s\n\n", CSD_HELP_M_IF);
	printf("\t\t%s\n\n", CSD_HELP_M_OF);
	printf("\t\t%s\n\n", CSD_HELP_M_VPID);
	printf("\t\t%s\n\n", CSD_HELP_M_APID);
	printf("\t\t%s\n\n", CSD_HELP_M_PPID);
	printf("\t\t%s\n\n", CSD_HELP_M_DSC_ECMPID);
	printf("\t\t%s\n\n", CSD_HELP_M_DSC_PECM);
	printf("\t\t%s\n\n", CSD_HELP_M_TV);

}

static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_dsc_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_DSC_Parameters * pParams)
{
	bool bVideoPidParsed = false;
	bool bAudioPidParsed = false;
	bool bPcrPidParsed = false;
	bool bEcmPidParsed = false;
	bool bEcmParsed = false;
	bool bEmiParsed = false;
	bool bIfParsered = false;
	bool bIfeParsered = false;
	bool bTvParsed = false;
	bool bIvcParsered = false;
	bool bOfParsered = false;
	int cmdline_offset = 2;

	/* R2R command parser state machine */
	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;


		/* video PID */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bVideoPidParsed, NOCS3_HARNESS_TEST_OPTION_VPID, &pParams->videoPid, "0x%x");

		/* Audio PID */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bAudioPidParsed, NOCS3_HARNESS_TEST_OPTION_APID, &pParams->audioPid, "0x%x");

		/* PCR PID */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bPcrPidParsed, NOCS3_HARNESS_TEST_OPTION_PPID, &pParams->pcrPid, "0x%x");

		/* ECM PID */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL (bEcmPidParsed, NOCS3_HARNESS_TEST_OPTION_EPID, &pParams->ecmPid, "0x%x");

		/* ECM */
		CSD_HARNESS_TEST_PARSE_ECM;

		/* EMI */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bEmiParsed, NOCS3_HARNESS_TEST_OPTION_EMI, &pParams->emi, "0x%x");

		/* IVC */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bIvcParsered, \
												NOCS3_HARNESS_TEST_OPTION_IVC, \
												&pParams->iv[0], \
												&pParams->sizeIV, \
												NOCS3_HARNESS_TEST_MAX_IV_SIZE);

		/* If */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pParams->pInFile, "rb");

		/* Ife */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bIfeParsered, NOCS3_HARNESS_TEST_OPTION_IFE);

		/* TV */
		NOCS3_COMMAND_PARSER_PARSE_FILE_REQUIRED (bTvParsed, NOCS3_HARNESS_TEST_OPTION_TV, &pParams->pTvFile, "r");


		/* of  */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "wb");
		
		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;


	} while ( cmdline_offset < argc );

	/* Make sure we got all the required field */
	if ( ( bVideoPidParsed == false) ||
		 ( bAudioPidParsed == false ) ||
		 ( bPcrPidParsed == false ) ||
		 ( bEmiParsed == false ) ||
		 ( bEcmParsed == false ) ||
		 ( bTvParsed == false ) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

	if ( (pParams->bEcmRequired) && (bEcmPidParsed==false))
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

	if ( (bIfParsered==true) && (bIfeParsered==true))
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

    return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

