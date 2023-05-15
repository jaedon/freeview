#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"

#if (CSDAPI_VERSION_MEDIUM<3)

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_fp.h"
#include "csd_harness_test_fp_private.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"
#include "nexus_memory.h"


static void csd_harness_test_fp_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_fp_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_FP_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void csd_harness_test_fp_dump_paramters (CSD_Harness_Test_FP_Parameters * pParams)
{
	printf("CSD_Harness_Test_FP_Parameters is :\n");

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
	"csdEncryptDataWithFlashProtRootKey",
	"csdEncryptDataWithClearTextFlashProtectionKey",
	"csdDecryptDataWithFlashProtRootKey",
	"csdDecryptDataWithClearTextFlashProtectionKey"
};

typedef enum CDS_HARNESS_TEST_FP_OP
{
	CDS_HARNESS_TEST_FP_OP_ENCKEY,
	CDS_HARNESS_TEST_FP_OP_ENCDATA,
	CDS_HARNESS_TEST_FP_OP_DECKEY,
	CDS_HARNESS_TEST_FP_OP_DECDATA,

	CDS_HARNESS_TEST_FP_OP_MAX
}	CDS_HARNESS_TEST_FP_OP;


void csd_harness_test_fp (int argc, char* argv[])
{
	CSD_Harness_Test_FP_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NOCS3_Harness_Test_Test_Vector test_vector, * pVector = &test_vector;
	NEXUS_PlatformSettings platformSettings;
 	TCsdInitParameters csdInit;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsd8BytesVector nuid;
	TCsdFPKeyPathHandle xFPKeyPathHandle, *pxFPKeyPathHandle=&xFPKeyPathHandle;
	unsigned char * pIn, * pOut, * pData, *pKey;
	unsigned int size, sizeKey;
	CDS_HARNESS_TEST_FP_OP operation;

	if ( (argc <3) || (strcmp (argv[2], "help")==0 ) )
	{
		csd_harness_test_fp_help();
		return;
	}

	memset (&params, 0, sizeof (CSD_Harness_Test_FP_Parameters));
	if ( (nocs3_status=csd_harness_test_fp_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("FP command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up_1;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	csd_harness_test_fp_dump_paramters (&params);
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
	if (status!=CSD_NO_ERROR) 
	{
		CSD_ERR_TRACE (status, "csdGetNuid64", "Cannot find NUID");
		goto Clean_Up;
	}	

	if (params.pTvFile)
	{
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
	}
	else
	{
		CSD_OTHER_ERR_TRACE("Invalid input parameters.");
		goto Clean_Up;
	}
	size = (params.sizeData)? (params.sizeData):(pVector->sizeDataIn);
	if ( size==0 )
	{
		CSD_OTHER_ERR_TRACE ("No input data!\n");
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
			if ( ( params.protection==CSD_HARNESS_TEST_FP_PROTCTION_KEY ) && (params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION))
			{
				CSD_OTHER_ERR_TRACE ("No key data!\n");
				goto Clean_Up;
			}
		}

	}

    NEXUS_Memory_Allocate(size, NULL, (void **)&pIn);
    NEXUS_Memory_Allocate(size, NULL, (void **)&pOut);
	if ( (pIn==NULL) || (pOut==NULL) )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("Out of memory\n");
		goto Clean_Up;
	}

	memcpy (pIn, pData, size*sizeof(unsigned char));
	memset (pOut, 0, size*sizeof(unsigned char));

	memset(pxFPKeyPathHandle, 0, sizeof(xFPKeyPathHandle));
	pxFPKeyPathHandle->initHandle = &csdInit;
	pxFPKeyPathHandle->keyHandle = NULL;
	pxFPKeyPathHandle->bEncrypt = (params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION)? true:false; 

	if ( params.protection==CSD_HARNESS_TEST_FP_PROTCTION_KEY )
	{

		status = csdSetFlashProtRootKey (&xFPKeyPathHandle);
		if ( status != CSD_NO_ERROR )
		{
			CSD_ERR_TRACE (status, "csdSetFlashProtRootKey", "Operation failed");
			goto Clean_Up;
		} 
		if (params.operation==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION)
		{
			status = csdDecryptDataWithFlashProtRootKey(
											pxFPKeyPathHandle,
											pData,
											pOut,
											size);
			operation = CDS_HARNESS_TEST_FP_OP_DECKEY;
		}
		else if (params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION)
		{
			status = csdEncryptDataWithFlashProtRootKey(
											pxFPKeyPathHandle,
											pKey,
											pOut,
											size);
			operation = CDS_HARNESS_TEST_FP_OP_ENCKEY;
		}
		else
		{
			CSD_OTHER_ERR_TRACE("Invalid Parameters.");
			goto Clean_Up;
		}

		if ( status != CSD_NO_ERROR )
		{
			CSD_ERR_TRACE (status, "csdDecryptDataWithFlashProtRootKey", "Operation failed");
			goto Clean_Up;
		} 
	}
	else if ( params.protection==CSD_HARNESS_TEST_FP_PROTCTION_DATA )
	{
		status = csdSetClearTextFlashProtectionKey(
										pxFPKeyPathHandle,
										pKey);
		if ( status != CSD_NO_ERROR )
		{
			CSD_ERR_TRACE (status, "csdSetClearTextFlashProtectionKey", "Operation failed");
			goto Clean_Up;
		} 

		if (params.operation==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION)
		{
			status = csdDecryptDataWithClearTextFlashProtectionKey(
											pxFPKeyPathHandle,
											pIn,
											pOut,
											size);
			operation = CDS_HARNESS_TEST_FP_OP_DECDATA;
		}
#if (CSDAPI_VERSION_MEDIUM!=2)
		else if (params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION)
		{
			status = csdEncryptDataWithClearTextFlashProtectionKey(
											pxFPKeyPathHandle,
											pIn,
											pOut,
											size);
			operation = CDS_HARNESS_TEST_FP_OP_ENCDATA;
		}
#endif
		else
		{
			CSD_OTHER_ERR_TRACE("Invalid Parameters.");
			goto Clean_Up;
		}

		if ( status != CSD_NO_ERROR )
		{
			CSD_ERR_TRACE (status, "csdDecryptDataWithFlashProtRootKey", "Operation failed");
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
		printf("[CSD][M] - %s - 0x", gFlashProtectionOperationStr[operation]);
		for (i=0; i<size; i++ )
		{
			printf("%02x", pOut[i]);
		}
		printf(" - %s() done successfully. \n", gFlashProtectionFunctionStr[operation]);
	}
	/* Clean up */
Clean_Up:

    csdTerminateHandle.initParam = &csdInit; 
    csdTerminateHandle.keyHandleToRelease = NULL; 
    csdTerminate(&csdTerminateHandle);

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


static void csd_harness_test_fp_help (void)
{
	printf("\t%s\n\n", CSD_HELP_M_FP_INST);
	printf("\t\t%s\n\n", CSD_HELP_M_FP_OP);
	printf("\t\t%s\n\n", CSD_HELP_M_FP_IFIC);
	printf("\t\t%s\n\n", CSD_HELP_M_FP_OFOC);
	printf("\t\t%s\n\n", CSD_HELP_M_FP_KEY);
	printf("\t\t%s\n\n", CSD_HELP_M_FP_TV);
}

static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_fp_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_FP_Parameters * pParams)
{
	int cmdline_offset = 2;
	bool bFpTypeParsered = false;
	bool bOcParsered = false;
	bool bIfParsered = false;
	bool bOfParsered = false;
	bool bIcParsered = false;
	bool bFkParsered = false;
	bool bOperationParsed = false;
	bool bTvParsed = false;
	FILE * pInFile = NULL;
 
	/* command parser state machine */
	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;

		/* Fp type */
		CSD_HARNESS_TEST_PARSE_FP_TYPE;

		/* enc/dec */
		NOCS3_HARNESS_TEST_PARSE_CRYPTO_OPERATION_REQUIRED;

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
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bFkParsered, NOCS3_HARNESS_TEST_OPTION_FK, &pParams->key[0], &pParams->sizeKey, CSD_HARNESS_TEST_FP_MAX_KEY_SIZE);

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


#endif /* (CSDAPI_VERSION_MEDIUM<3) */
 
