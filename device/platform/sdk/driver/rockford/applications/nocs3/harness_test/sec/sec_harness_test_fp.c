#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"
#include "ca_sec_impl.h"

#include "sec_harness_test.h"
#include "sec_harness_test_parser.h"
#include "sec_harness_test_fp.h"
#include "sec_harness_test_fp_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"
#include "nexus_memory.h"

static void sec_harness_test_fp_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_fp_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_FP_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_fp_dump_paramters (SEC_Harness_Test_FP_Parameters * pParams)
{
	printf("SEC_Harness_Test_FP_Parameters is :\n");

	printf("protection: %d\n", pParams->protection);
	printf("operation: %d\n", pParams->operation);
	printf("sizeKey %d\n", pParams->sizeKey);
	if (pParams->sizeKey)
	{
		printf("Key array is:\n");
		nocs3_harness_test_utility_dump_array (pParams->key, pParams->sizeKey);
	}
	printf("sizeData %d\n", pParams->sizeData);
	if (pParams->sizeData)
	{
		printf("Data array is:\n");
		nocs3_harness_test_utility_dump_array (pParams->pData, pParams->sizeData);
	}
	printf("pOutFile %p\n", (void *)pParams->pOutFile);
	printf("pTvFile %p\n", (void *)pParams->pTvFile);
}
#endif

static char gFlashProtectionOperationStr[4][16] = 
{
	"ENCKEY",
	"ENCDATA",
	"DECKEY",
	"DECDATA"
};

static char gFlashProtectionFunctionStr[4][64] = 
{
	"secEncryptFlashProtKey",
	"secSessionEncrypt",
	"Not Supported",
	"secSessionDecrypt"
};

typedef enum SEC_HARNESS_TEST_FP_OP
{
	SEC_HARNESS_TEST_FP_OP_ENCKEY,
	SEC_HARNESS_TEST_FP_OP_ENCDATA,
	SEC_HARNESS_TEST_FP_OP_DECKEY,
	SEC_HARNESS_TEST_FP_OP_DECDATA,

	SEC_HARNESS_TEST_FP_OP_MAX
}	SEC_HARNESS_TEST_FP_OP;

void sec_harness_test_fp (int argc, char* argv[])
{
	SEC_Harness_Test_FP_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NOCS3_Harness_Test_Test_Vector test_vector, * pVector = &test_vector;
	NEXUS_PlatformSettings platformSettings;
	TSecFunctionTable *functionTable;
	TSecStatus status;
	unsigned char *pIvCbc; 

	unsigned char * pIn, * pOut, * pData, *pKey;
	unsigned int size, sizeKey, sizeIvCbc;
	SEC_HARNESS_TEST_FP_OP operation;
	TSecCipherSession r2rSession;
	TSecNuid64  nuid; 
	

	if ( (argc <3) || (strcmp (argv[2], "help")==0 ) )
	{
		sec_harness_test_fp_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_FP_Parameters));
	if ( (nocs3_status=sec_harness_test_fp_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE("FP command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up_1;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_fp_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

	functionTable = secGetFunctionTable();


	status = functionTable->secGetNuid64(nuid);
		if ( status!=SEC_NO_ERROR) 
		{
			SEC_ERR_TRACE (status, "secGetNuid64", "Cannot find NUID");
			goto Clean_Up;
		}		

	if (params.pTvFile)
	{
		/* Get test vector */
		memset (pVector, 0, sizeof (NOCS3_Harness_Test_Test_Vector));
		if ( (nocs3_status=nocs3_harness_test_get_test_vector_4_nuid (nuid, pVector, params.pTvFile)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
		{
			/* Parser error code */
			SEC_OTHER_ERR_TRACE("Cannot find required test vector.");
			goto Clean_Up;
		}

	#ifdef NOCS3_DEBUG_MESSAGE
		nocs3_harness_test_dump_test_vector (pVector);
	#endif
	}

	if ( params.sizeData )
	{
		size = params.sizeData;
		pData = params.pData;
	}
	else if (params.pTvFile)
	{
		size = pVector->sizeDataIn;
		pData = pVector->dataIn;
		if(pVector->sizeIvCbc){ 
				pIvCbc = pVector->IvCbc; 
				sizeIvCbc = pVector->sizeIvCbc;
		}else {
			sizeIvCbc = 0;
			pIvCbc = NULL;
			}
		if(pVector->sizeIvCtr &&( (params.emi==SEC_Algorithm_RAW_AES_128_CTR)||(params.emi ==SEC_Algorithm_RAW_0x4024)) ){
			pIvCbc = pVector->IvCtr;
			sizeIvCbc = pVector->sizeIvCtr;
			}
	}
	else
	{
		SEC_OTHER_ERR_TRACE("Invalid input parameters.");
		goto Clean_Up;
	}
	size = (params.sizeData)? (params.sizeData):(pVector->sizeDataIn);
	if ( size==0 )
	{
		SEC_OTHER_ERR_TRACE ("No input data!\n");
		goto Clean_Up;
	}

	if ( params.sizeKey == 16)
	{
		sizeKey= params.sizeKey;
		pKey = params.key;
	}
	else
	{
		if ( pVector->sizeClearTextFlashProtectingKey==16 )
		{
			sizeKey= pVector->sizeClearTextFlashProtectingKey;
			pKey = pVector->clearTextFlashProtectingKey;
		}
		else
		{
			if ( ( params.protection==SEC_HARNESS_TEST_FP_PROTCTION_KEY ) && (params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION))
			{
				SEC_OTHER_ERR_TRACE ("No key data!\n");
				goto Clean_Up;
			}
		}

	}

    NEXUS_Memory_Allocate(size, NULL, (void **)&pIn);
    NEXUS_Memory_Allocate(size, NULL, (void **)&pOut);
	if ( (pIn==NULL) || (pOut==NULL) )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("Out of memory\n");
		goto Clean_Up;
	}
	if ( params.protection==SEC_HARNESS_TEST_FP_PROTCTION_KEY && params.pTvFile )size = pVector->sizeClearTextFlashProtectingKey;

	memcpy (pIn, pData, size*sizeof(unsigned char));
	memset (pOut, 0, size*sizeof(unsigned char));


	if ( params.protection==SEC_HARNESS_TEST_FP_PROTCTION_KEY )
	{

		if (params.operation==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION)
		{
			
			printf("[SEC][EX] secEncryptFlashProtKey error:decryption is not allowed in key mode! \n");
			goto Clean_Up;
		}

		if (params.pTvFile && pVector->sizeClearTextFlashProtectingKey==16 )
		{

			memcpy (pIn, pVector->clearTextFlashProtectingKey, size*sizeof(unsigned char));
		}
		status = functionTable->secEncryptFlashProtKey(pIn, pOut, size);
		if ( status != SEC_NO_ERROR )
		{
			printf("[SEC][EX] secEncryptFlashProtKey error %d\n", status);
			goto Clean_Up;
		} 
		operation = SEC_HARNESS_TEST_FP_OP_ENCKEY;
		
	}
	else if ( params.protection==SEC_HARNESS_TEST_FP_PROTCTION_DATA )
	{
		r2rSession = malloc(sizeof(TSecCipherSession));

		memset (r2rSession, 0, sizeof(TSecCipherSession));
		if(params.operation == NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION){
				status =functionTable->secOpenRam2RamEncryptSession(&r2rSession);
				if(status!=SEC_NO_ERROR){
					printf("[SEC][EX] secOpenRam2RamEncryptSession error %d\n", status);
					return;
					}
		}else{
				status = functionTable->secOpenRam2RamDecryptSession(&r2rSession);
				if(status!=SEC_NO_ERROR){
					printf("[SEC][EX] secOpenRam2RamDecryptSession error %d\n", status);
					return;
					}
		}
if(params.emi==0) params.emi = 0x4021;  /*default is AES ECB */
		status = functionTable->secUseFlashProtKey(
										r2rSession,
										params.emi);
		if ( status != SEC_NO_ERROR )
		{
			printf("[SEC][EX] secUseFlashProtKey error %d\n", status);
			goto Clean_Up;
		} 

		if (params.operation==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION)
		{
			status = functionTable->secSessionDecrypt(
											r2rSession,
											pIn,
											pOut,
											size,pIvCbc, sizeIvCbc);
			operation = SEC_HARNESS_TEST_FP_OP_DECDATA;
		}

		else if (params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION)
		{
			status =functionTable->secSessionEncrypt(
											r2rSession,
											pIn,
											pOut,
											size,pIvCbc, sizeIvCbc);
			operation = SEC_HARNESS_TEST_FP_OP_ENCDATA;
		}

		else
		{
			printf("[SEC][EX] secOpenRam2RamDecryptSession error: Invalid parameters\n");
			goto Clean_Up;
		}

		if ( status != SEC_NO_ERROR )
		{
			printf("[SEC][EX] secOpenRam2RamDecryptSession error %d\n", status);
			goto Clean_Up;
		} 
	}


	if ( params.pOutFile)
	{
		unsigned int i;

		fprintf(params.pOutFile, "0x");
		for (i=0; i<size; i++ )
		{
			fprintf(params.pOutFile, "%02x", pOut[i]);
		}
	}
	else
	{
		unsigned int i;
		printf("[SEC][M] - %s - 0x", gFlashProtectionOperationStr[operation]);
		for (i=0; i<size; i++ )
		{
			printf("%02x", pOut[i]);
		}
	}
	
	printf("\n[SEC][M] - %s - %s() done successfully. \n", gFlashProtectionOperationStr[operation], gFlashProtectionFunctionStr[operation]);
	/* Clean up */
Clean_Up:
	
	if ( params.protection==SEC_HARNESS_TEST_FP_PROTCTION_DATA ){

    	functionTable->secCloseSession(r2rSession);
   		free(r2rSession);
			}
   NEXUS_Memory_Free(functionTable);
   NEXUS_Memory_Free(pIn);
   NEXUS_Memory_Free(pOut);

Clean_Up_1:
	if ( params.pData )
	{
		free (params.pData);
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


static void sec_harness_test_fp_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_FP_INST);
	printf("\t\t%s\n\n", SEC_HELP_M_FP_OP);
	printf("\t\t%s\n\n", SEC_HELP_M_FP_IFIC);
	printf("\t\t%s\n\n", SEC_HELP_M_FP_OFOC);
	printf("\t\t%s\n\n", SEC_HELP_M_FP_KEY);
	printf("\t\t%s\n\n", SEC_HELP_M_FP_TV);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_fp_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_FP_Parameters * pParams)
{
	int cmdline_offset = 2;
	bool bFpTypeParsered = false;
	bool bEmiParsed = false;
	bool bOcParsered = false;
	bool bIfParsered = false;
	bool bOfParsered = false;
	bool bIcParsered = false;
	bool bFkParsered = false;
	bool bOperationParsed = false;
	bool bTvParsed = false;
	FILE * pInFile = NULL;
 pParams->emi = 0;
	/* command parser state machine */
	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;

		/* Fp type */
		SEC_HARNESS_TEST_PARSE_FP_TYPE;

		/* enc/dec */
		NOCS3_HARNESS_TEST_PARSE_CRYPTO_OPERATION_REQUIRED;

		/* EMI */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL (bEmiParsed, NOCS3_HARNESS_TEST_OPTION_EMI, &pParams->emi, "0x%x");

		/* Ic */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL_2(bIcParsered, \
												NOCS3_HARNESS_TEST_OPTION_IC, \
												&pParams->pData, \
												&pParams->sizeData);

		/* Oc */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bOcParsered, NOCS3_HARNESS_TEST_OPTION_OC);

		/* If */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pInFile, "rb");
		
		/* Of */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "w");

		/* fk */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bFkParsered, NOCS3_HARNESS_TEST_OPTION_FK, &pParams->key[0], &pParams->sizeKey, SEC_HARNESS_TEST_FP_MAX_KEY_SIZE);

		/* Tv */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bTvParsed, NOCS3_HARNESS_TEST_OPTION_TV, &pParams->pTvFile, "r");
		
		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;

	} while ( cmdline_offset < argc );

	if ( ( (bIfParsered==true) && (bIcParsered==true)) || \
		 ( (bOfParsered==true) && (bOcParsered==true)) || \
		 (bFpTypeParsered==false) ||\
		 (bOperationParsed==false))
	{
		if (pInFile!=NULL)
		{
			fclose (pInFile);
		}
		if (pParams->pOutFile!=NULL)
		{
			fclose (pParams->pOutFile);
			pParams->pOutFile = NULL;
		}
		return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;	
	}
	
	if (pInFile!=NULL)
	{
		unsigned int size;
		fseek (pInFile, 0, SEEK_END);
		size = ftell (pInFile);
		pParams->pData = malloc(sizeof(unsigned char)*size);
		if ( pParams->pData==NULL )
		{
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;	
		}
		fseek (pInFile, 0, SEEK_SET);
		pParams->sizeData = fread (pParams->pData, sizeof(unsigned char), size, pInFile);
		fclose (pInFile);
	}


	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;
}

