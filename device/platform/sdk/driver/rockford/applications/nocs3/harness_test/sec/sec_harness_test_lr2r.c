#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"

#include "sec_harness_test.h"
#include "sec_harness_test_parser.h"
#include "sec_harness_test_lr2r.h"
#include "sec_harness_test_lr2r_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_memory.h"
#include "nexus_platform.h"

static void sec_harness_test_lr2r_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_lr2r_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_R2R_Parameters * pParams);

static char gKeyFuncName[2][64] = 
{
    "csdSetClearTextRam2RamHostKey",
    "csdSetProtectedRam2RamContentKeys"
};

static char gCryptoFuncName[2][64] = 
{
    "csdEncryptData",
    "csdDecryptData"
};

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_lr2r_dump_paramters (SEC_Harness_Test_R2R_Parameters * pParams)
{
	printf("SEC_Harness_Test_R2R_Parameters is :\n");

	printf("operation: %d\n", pParams->operation);
	printf("keyType %d\n", pParams->keyType);
	printf("emi %x\n", pParams->emi);
	printf("sizeIV %d\n", pParams->sizeIV);
	printf("IV array is:\n");
	nocs3_harness_test_utility_dump_array (pParams->iv, pParams->sizeIV);
	printf("sizeData %d\n", pParams->sizeData);
	printf("Data array is:\n");
	nocs3_harness_test_utility_dump_array (pParams->data, pParams->sizeData);
	printf("bMonteCarlo %d\n", pParams->bMonteCarlo);
	printf("numRuns %d\n", pParams->numRuns);
	printf("pInFile %p\n", (void *)pParams->pInFile);

	printf("pOutFile %p\n", (void *)pParams->pOutFile);
	printf("pTvFile %p\n", (void *)pParams->pTvFile);
}
#endif

void sec_harness_test_lr2r (int argc, char* argv[])
{
	SEC_Harness_Test_R2R_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	TSecStatus status;
#if 0
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsd8BytesVector  nuid; 
	TCsdR2RKeyPathHandle      xR2RKeyPathHandle,  *pxR2RKeyPathHandle = &xR2RKeyPathHandle;
#endif
	unsigned long ulNuidLo; 
	unsigned int i; 
	unsigned char xHostKey[16] = {
			/*0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
			0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01*/
		/*	0x8E, 0x1C,  0x09, 0x83, 0x8D, 0x9E, 0x6E, 0xD2, 0xA1, 0xED, 0x0D, 0xD4, 0x6D, 0xD4, 0x5C, 0x03*/
			/*0x80,  0x76, 0x28, 0xD9, 0xFD, 0x99, 0xA6, 0xB5,
			0x70, 0x0C, 0xA1, 0x49, 0x00, 0xC0, 0xD8, 0x65,

			*/
			
			0x7A, 0x81, 0x11, 0x34, 0x32, 0xC4, 0xCC, 0xED,
			0x2E, 0x92, 0xDA, 0x2A, 0xF1, 0x7D, 0xD9, 0x24
			
		};
	unsigned char DataIn[1024]={0}; 
	unsigned char dataOut[1024] ={0};

	unsigned int sizeDataIn; 
	NEXUS_PlatformSettings platformSettings;
	TSecFunctionTable *functionTable;
	NOCS3_Harness_Test_Test_Vector inputVector; 

	if ( (argc <3) || (strcmp (argv[2], "help")==0 ) )
	{
		sec_harness_test_lr2r_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_R2R_Parameters));
	if ( (nocs3_status=sec_harness_test_lr2r_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("R2R command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_lr2r_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	functionTable = secGetFunctionTable();
	TSecNuid64	nuid; 

	
	status = functionTable->secGetNuid64(nuid);
	if ( status!=SEC_NO_ERROR) 
	{
		SEC_ERR_TRACE (status, "secGetNuid64", "Cannot find NUID");
		goto Clean_Up;
	}		


	if(params.sizeData) {
				BKNI_Memcpy(DataIn, params.data, params.sizeData); 
				sizeDataIn = params.sizeData;
				
		}
	else if(params.pInFile){
				/* read file and get input data */
		sizeDataIn = fread((void *) DataIn, sizeof(unsigned char), 1024, params.pInFile);
		printf("Reading file, size is %d\n", sizeDataIn);
	}else if(params.pTvFile !=NULL){
		nocs3_harness_test_get_test_vector_4_nuid ( nuid, 
									&inputVector,
							params.pTvFile);
			
		if(inputVector.sizeDataIn) BKNI_Memcpy(DataIn, inputVector.dataIn, inputVector.sizeDataIn); 
		
		sizeDataIn = inputVector.sizeDataIn; 
	}

	
	if(params.operation == NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION)
	{
		printf("encrypt result...\n"); 
		printf("Input data...\n");
		
		printf("0x");
		for(i=0; i<sizeDataIn; i++) 
			printf("%02x", DataIn[i]); 
		printf("\n"); 

		for(i=0; i<params.numRuns; i++)
		{
			status = functionTable->secEncryptData(dataOut,DataIn,  sizeDataIn); 
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secEncryptData error %d\n", status);
				return;
				}
			if(params.bMonteCarlo)
			{
				
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
			}
		}
	}
	else if(params.operation ==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION)
	{
		printf("decrypt result...\n"); 
		printf("Input data...\n");
		printf("0x");
		for(i=0; i<sizeDataIn; i++) 
			printf("%02x", DataIn[i]); 
		printf("\n"); 

		
		for(i=0; i<params.numRuns;i++ )
		{
			status = functionTable->secDecryptData(dataOut, DataIn,  sizeDataIn); 
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secDecryptData error %d\n", status);
				return;
				}
			if(params.bMonteCarlo)
			{
	
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
				
			}
		}

	}
	
	if(params.pOutFile!=NULL)
	{
		fwrite(dataOut, sizeof(unsigned char), sizeDataIn,params.pOutFile);
		
		
	}
	else
	{ /* -oc */
		printf("Output:\n"); 


		printf("0x");
		for(i=0; i<sizeDataIn; i++) printf("%02x", dataOut[i]); 
		printf("\n"); 		
		}

	printf("[SEC][M] lr2r test is done successfully\n");
 Clean_Up:
	/* Clean up */


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
 	NEXUS_Memory_Free(functionTable);


 Clean_Up_1:
   NEXUS_Platform_Uninit();
}


static void sec_harness_test_lr2r_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_LR2R_INST);
	printf("\t\t%s\n\n", SEC_HELP_M_LR2R_OP);
	printf("\t\t%s\n\n", SEC_HELP_M_LR2R_IFIC);
	printf("\t\t%s\n\n", SEC_HELP_M_LR2R_OFOC);
	printf("\t\t%s\n\n", SEC_HELP_M_LR2R_MT);
	printf("\t\t%s\n\n", SEC_HELP_M_LR2R_RUNS);
	printf("\t\t%s\n\n", NOCS3_HELP_M_TV);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_lr2r_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_R2R_Parameters * pParams)
{
	bool bOperationParsed = false;
	bool bKeyTypeParsed = false;
	bool bEmiParsed = false;
	bool bIvcParsed = false;
	bool bOcParsered = false;
	bool bIfParsered = false;
	bool bOfParsered = false; 
	bool bIcParsered = false;
	bool bMtParsed = false;
	bool bNumRunsParsed = false;
	bool bTvParsed = false;
	int cmdline_offset = 2;

	/* R2R command parser state machine */
	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;

		/* Crypto Operation */
		NOCS3_HARNESS_TEST_PARSE_CRYPTO_OPERATION_REQUIRED;



		/* Ic */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bIcParsered, \
												NOCS3_HARNESS_TEST_OPTION_IC, \
												&pParams->data[0], \
												&pParams->sizeData, \
												SEC_HARNESS_TEST_R2R_MAX_DATA_SIZE);
		/* If */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pParams->pInFile, "rb");

		/* Oc */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bOcParsered, NOCS3_HARNESS_TEST_OPTION_OC);

		/* Of */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "w");


		/* Monte Carlo */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bMtParsed, NOCS3_HARNESS_TEST_OPTION_MT);


		/* NumRuns */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bNumRunsParsed, NOCS3_HARNESS_TEST_OPTION_RUNS, &pParams->numRuns, "0x%x");


		/* TV */
		NOCS3_COMMAND_PARSER_PARSE_FILE_REQUIRED (bTvParsed, NOCS3_HARNESS_TEST_OPTION_TV, &pParams->pTvFile, "r");


		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;


	} while ( cmdline_offset < argc );

	/* Make sure we got all the required field */
	if ( ( (bIfParsered==true) && (bIcParsered==true)) || \
		 ( (bOfParsered==true) && (bOcParsered==true)) || \
		 ( bOperationParsed == false) ||
		 ( (bOfParsered==false) && (bOcParsered==false) ) ||
		 ( bNumRunsParsed == false )  )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

	/* Further processing */
	if (bMtParsed)
	{
		pParams->bMonteCarlo = true;
	}

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;
}

