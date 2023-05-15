#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "NOCS3x_Test_Harness.h"

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_scr.h"
#include "csd_harness_test_scr_private.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "csd_harness_test_ca_pvr.h"
#include "nexus_platform.h"


static void csd_harness_test_scr_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_scr_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_SCR_Parameters * pParams);


#ifdef NOCS3_DEBUG_MESSAGE
static void csd_harness_test_scr_dump_paramters (CSD_Harness_Test_SCR_Parameters * pParams)
{
	printf("CSD_Harness_Test_SCR_Parameters is :\n");

	printf("operation: %d\n", pParams->operation);
	printf("keyType %d\n", pParams->keyType);
	printf("videoPid: 0x%x\n", pParams->videoPid);
	printf("audioPid 0x%x\n", pParams->audioPid);
	printf("pcrPid 0x%x\n", pParams->pcrPid);
	printf("emi %x\n", pParams->emi);
	printf("sizeIV %d\n", pParams->sizeIV);
	if (pParams->sizeIV)
	{
		printf("IV array is:\n");
		nocs3_harness_test_utility_dump_array (pParams->iv, pParams->sizeIV);
	}
	printf("pInFile %p\n", (void *)pParams->pInFile);
	printf("pOutFile %p\n", (void *)pParams->pOutFile);
	printf("pTvFile %p\n", (void *)pParams->pTvFile);
}
#endif

void csd_harness_test_scr (int argc, char* argv[])
{
	CSD_Harness_Test_SCR_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NOCS3_Harness_Test_Test_Vector test_vector, * pVector = &test_vector;
 	TCsdInitParameters csdInit;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsd8BytesVector nuid;
	TCsdScrKeyPathHandle keyPathHandle;
    TCsdUnsignedInt8 key[16];
	unsigned int keySize=16;
	TCsdScrCipheredProtectingKeys protectingKey;
	CSD_Harness_Test_CA_PVR_Configuration caPvrConfig, *pCaPvrConfig = &caPvrConfig;
	TCsdUnsignedInt8 * pIV = NULL; 
	unsigned int ivSize = 0;
	NEXUS_PlatformSettings platformSettings;

	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		csd_harness_test_scr_help();
		return;
	}


	memset (&params, 0, sizeof (CSD_Harness_Test_SCR_Parameters));
	if ( (nocs3_status=csd_harness_test_scr_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("SCR command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up_1;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	csd_harness_test_scr_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

	/* csd_harness_test_scr_dump_paramters (&params);*/
	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	
	status=csdInitialize(&csdInit);
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		CSD_ERR_TRACE (status, "csdInitialize", "Operation failed");
		goto Clean_Up_1;
	}	


	status = csdGetNuid64(nuid);
	if ( status!=CSD_NO_ERROR) 
	{
		CSD_ERR_TRACE (status, "csdGetNuid64", "Cannot find NUID");
		goto Clean_Up;
	}	
	keyPathHandle.initHandle = &csdInit;

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

	if (params.sizeIV !=0 )
	{
		ivSize = params.sizeIV;
		pIV = params.iv;
	}
	else
	{
		switch (params.emi)
		{
		case CSD_Algorithm_AES_128_CBC_CLEAR:
		case CSD_Algorithm_RAW_0x4022:
		case CSD_Algorithm_RAW_0x4023:
			ivSize = pVector->sizeIvCiPlus;
			pIV = pVector->IvCiPlus;
			break;
		case CSD_Algorithm_RAW_AES_128_COUNTER_CLEAR:
		case CSD_Algorithm_RAW_0x4024:
			ivSize = pVector->sizeIvCtr;
			pIV = pVector->IvCtr;
			break;
		default:
			ivSize = pVector->sizeIvCbc;
			pIV = pVector->IvCbc;
			break;
		}
	}

	if ( ( params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION) && (params.keyType==NOCS3_HARNESS_TEST_KEY_SOFTWARE) )
	{
		memcpy (key, pVector->HostKey, keySize*sizeof(unsigned char));
    	if( (status=csdSetClearTextScrEncryptionKey(
        									params.emi,
  	 										key,
        									keySize,
											pIV,
        									ivSize,
        									&keyPathHandle)) != CSD_NO_ERROR)
		{
        	CSD_ERR_TRACE(status, "csdSetClearTextScrEncryptionKey", "Operation failed!"); 
			goto Clean_Up;
		}
		pCaPvrConfig->bEncryption = true;
	}
	else if ( ( params.operation==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION) && (params.keyType==NOCS3_HARNESS_TEST_KEY_SOFTWARE) )
	{
		memcpy (key, pVector->HostKey, keySize*sizeof(unsigned char));
    	if( (status=csdSetClearTextScrDecryptionKey(
        									params.emi,
  	 										key,
        									keySize,
											pIV,
        									ivSize,
        									&keyPathHandle)) != CSD_NO_ERROR)
		{
        	CSD_ERR_TRACE(status, "csdSetClearTextScrDecryptionKey", "Operation failed!"); 
			goto Clean_Up;
		}
		pCaPvrConfig->bEncryption = false;
	}
	else if ( ( params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION) && (params.keyType==NOCS3_HARNESS_TEST_KEY_KEYLADDER) )
	{
		memcpy (key, pVector->cipheredContentKey, keySize*sizeof(unsigned char));
		memcpy (&protectingKey[0][0], pVector->cipheredProtectingKeyL2, 16*sizeof(unsigned char));
 		memcpy (&protectingKey[1][0], pVector->cipheredProtectingKeyL1, 16*sizeof(unsigned char));
   		if( (status=csdSetProtectedScrEncryptionKey(
        									params.emi,
											protectingKey,
  	 										key,
        									keySize,
											pIV,
        									ivSize,
        									&keyPathHandle)) != CSD_NO_ERROR)
		{
        	CSD_ERR_TRACE(status, "csdSetProtectedScrEncryptionKey", "Operation failed!"); 
			goto Clean_Up;
		}
		pCaPvrConfig->bEncryption = true;
	}
	else if ( ( params.operation==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION) && (params.keyType==NOCS3_HARNESS_TEST_KEY_KEYLADDER) )
	{
		memcpy (key, pVector->cipheredContentKey, keySize*sizeof(unsigned char));
		memcpy (&protectingKey[0][0], pVector->cipheredProtectingKeyL2, 16*sizeof(unsigned char));
 		memcpy (&protectingKey[1][0], pVector->cipheredProtectingKeyL1, 16*sizeof(unsigned char));
    	if( (status=csdSetProtectedScrDecryptionKey(
        									params.emi,
											protectingKey,
  	 										key,
        									keySize,
											pIV,
        									ivSize,
        									&keyPathHandle)) != CSD_NO_ERROR)
		{
        	CSD_ERR_TRACE(status, "csdSetProtectedScrDecryptionKey", "Operation failed!"); 
			goto Clean_Up;
		}
		pCaPvrConfig->bEncryption = false;
	}

	pCaPvrConfig->bIsDsc = false;
	pCaPvrConfig->emi = params.emi;
	pCaPvrConfig->audioPID = params.audioPid;
	pCaPvrConfig->videoPID = params.videoPid;
	pCaPvrConfig->pcrPID = params.pcrPid;
	pCaPvrConfig->bEcmEnabled = false;
	pCaPvrConfig->ecmPID = 0;
	pCaPvrConfig->pScrKeyHandle = &keyPathHandle;
	pCaPvrConfig->pDscVideoKeyHandle = NULL;
	pCaPvrConfig->pDscAudioKeyHandle = NULL;
	pCaPvrConfig->pInFile = params.pInFile;
	pCaPvrConfig->pOutFile = params.pOutFile;
	pCaPvrConfig->pVector = pVector;
	pCaPvrConfig->sizeIV = ivSize;
	if (ivSize)
	memcpy (&pCaPvrConfig->iv[0], pIV, ivSize*sizeof(unsigned char));
	nocs3_status = csd_harness_test_ca_pvr (pCaPvrConfig);
	if ( nocs3_status != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE("SCR test failed");
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


static void csd_harness_test_scr_help (void)
{
	printf("\t%s\n\n", CSD_HELP_M_SCR_INST);
	printf("\t\t%s\n\n", CSD_HELP_M_SCR_OP);
	printf("\t\t%s\n\n", CSD_HELP_M_KEY);
	printf("\t\t%s\n\n", CSD_HELP_M_EMI);
	printf("\t\t%s\n\n", CSD_HELP_M_IVC);
	printf("\t\t%s\n\n", CSD_HELP_M_IF);
	printf("\t\t%s\n\n", CSD_HELP_M_OF);
	printf("\t\t%s\n\n", CSD_HELP_M_VPID);
	printf("\t\t%s\n\n", CSD_HELP_M_APID);
	printf("\t\t%s\n\n", CSD_HELP_M_PPID);
	printf("\t\t%s\n\n", CSD_HELP_M_TV);

}

static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_scr_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_SCR_Parameters * pParams)
{
	bool bOperationParsed = false;
	bool bKeyTypeParsed = false;
	bool bVideoPidParsed = false;
	bool bAudioPidParsed = false;
	bool bPcrPidParsed = false;
	bool bEmiParsed = false;
	bool bIvcParsed = false;
	bool bIfParsered = false;
	bool bIfeParsered = false;
	bool bTvParsed = false;
	bool bOfParsered = false;
	int cmdline_offset = 2;

	/* R2R command parser state machine */
	pParams->videoPid = scrServiceVideoPid;
	pParams->audioPid = scrServiceAudioPid;
	pParams->pcrPid = scrServicePCRPid;

	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;

		NOCS3_HARNESS_TEST_PARSE_CRYPTO_OPERATION_REQUIRED;


		/* Key type */
		NOCS3_HARNESS_TEST_PARSE_KEY_TYPE_REQUIRED;

		/* video PID */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL (bVideoPidParsed, NOCS3_HARNESS_TEST_OPTION_VPID, &pParams->videoPid, "0x%x");

		/* Audio PID */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL (bAudioPidParsed, NOCS3_HARNESS_TEST_OPTION_APID, &pParams->audioPid, "0x%x");

		/* PCR PID */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL (bPcrPidParsed, NOCS3_HARNESS_TEST_OPTION_PPID, &pParams->pcrPid, "0x%x");

		/* EMI */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bEmiParsed, NOCS3_HARNESS_TEST_OPTION_EMI, &pParams->emi, "0x%x");

		/* IVC */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bIvcParsed, \
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
	if ( ( bOperationParsed == false) ||
		 ( bKeyTypeParsed == false ) ||
		 ( bEmiParsed == false ) ||
		 ( bTvParsed == false ) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

    return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}



