
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"

#include "sec_harness_test.h"
#include "sec_harness_test_ecdh.h"
#include "sec_harness_test_ecdh_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void sec_harness_test_ecdh_help (void);

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_ecdh_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Ecdh_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_ecdh_dump_paramters (SEC_Harness_Test_Ecdh_Parameters * pParams)
{
	printf("SEC_Harness_Test_Ecdh_Parameters is :\n");
	printf("operation: %08x\n", pParams->operation);
	printf("pParamsFile: %08x\n", pParams->pParamsFile);
	printf("pIpvtkFile: %08x\n", pParams->pIpvtkFile);
	printf("pPubkxFile: %08x\n", pParams->pPubkxFile);
	printf("pPubkyFile: %08x\n", pParams->pPubkyFile);
	printf("pOpvtkFile: %08x\n", pParams->pOpvtkFile);
	printf("pOpubkxFile: %08x\n", pParams->pOpubkxFile);
	printf("pOpubkyFile: %08x\n", pParams->pOpubkyFile);
	printf("pSsxFile: %08x\n", pParams->pSsxFile);
	printf("pSsyFile: %08x\n", pParams->pSsyFile);
}
#endif

void sec_harness_test_ecdh (int argc, char* argv[])
{
	SEC_Harness_Test_Ecdh_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NEXUS_PlatformSettings platformSettings;
	
	unsigned char ipvtkey[1024] = {0}; 
	unsigned char Opvtkey[1024] = {0}; 
	unsigned char OpubkeyX[1024] = {0}; 
	unsigned char OpubkeyY[1024] = {0}; 
	unsigned char SSX[1024] = {0}; 
	unsigned char SSY[1024] = {0}; 
	unsigned char out_data[1024]={0};
	TSecFunctionTable *functionTable;
	unsigned int i;
	NOCS3_Harness_Test_Test_Vector inputVector; 
	T8BytesVector  nuid={0}; 
	TSecEcParams	 xParams;
	unsigned int keySize = 0;
	TSecStatus status;
	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		sec_harness_test_ecdh_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_Ecdh_Parameters));
	if ( (nocs3_status=sec_harness_test_ecdh_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("Ram command parser failed.  Please make sure the command line parameters are valid");
		return;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_ecdh_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	functionTable = secGetFunctionTable();
	if(params.pParamsFile!=NULL) {
		nocs3_harness_test_get_test_vector_4_nuid ( nuid, 
															&inputVector,
														params.pParamsFile);
#if 0
		printf("a = \n(size %d\) ", inputVector.sizea); 
		for(i=0; i<inputVector.sizea; i++) printf("%x ", inputVector.a[i]);
#endif
		xParams.a = malloc(inputVector.sizea);
		xParams.q = malloc(inputVector.sizeq);
		xParams.b= malloc(inputVector.sizeb);
		xParams.GX= malloc(inputVector.sizeGX);
		xParams.GY= malloc(inputVector.sizeGY);
		xParams.n= malloc(inputVector.sizen);
		xParams.h= malloc(inputVector.sizeh);

		memcpy(xParams.a, inputVector.a, inputVector.sizea);
		memcpy(xParams.b, inputVector.b, inputVector.sizeb);
		memcpy(xParams.q, inputVector.q, inputVector.sizeq);
		memcpy(xParams.GX, inputVector.GX, inputVector.sizeGX);
		memcpy(xParams.GY, inputVector.GY, inputVector.sizeGY);
		memcpy(xParams.n, inputVector.n, inputVector.sizen);
		memcpy(xParams.h, inputVector.h, inputVector.sizeh);
		
		xParams.keySize = inputVector.sizea;
	}
	
		if(params.pIpvtkFile){
		
			keySize = fread((void *) ipvtkey, sizeof(unsigned char), 1024, params.pIpvtkFile);
			printf("read ipvtk %d\n", keySize);
		}
		if(params.operation == SEC_HARNESS_TEST_ECDH_OPERATION_KEY_GENERATION){
			if(params.pIpvtkFile)status=functionTable->secEcdhGenerateKey(xParams,ipvtkey,Opvtkey, OpubkeyX, OpubkeyY);
			else status=functionTable->secEcdhGenerateKey(xParams,NULL,Opvtkey, OpubkeyX, OpubkeyY);
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secEcdhGenerateKey error %d\n", status);
			
			return;
			}
			if(params.pOpvtkFile){
				
				fwrite((void *) Opvtkey, sizeof(unsigned char),xParams.keySize, params.pOpvtkFile);
				fclose(params.pOpvtkFile); 
			}
			if(params.pOpubkxFile){
				
				fwrite((void *) OpubkeyX, sizeof(unsigned char), xParams.keySize, params.pOpubkxFile);
				fclose(params.pOpubkxFile);
			}
			if(params.pOpubkyFile){
				
				fwrite((void *) OpubkeyY, sizeof(unsigned char), xParams.keySize, params.pOpubkyFile);
				fclose(params.pOpubkyFile);
			}
		}
		else if(params.operation == SEC_HARNESS_TEST_ECDH_OPERATION_SHARED_SECRET){
			if(params.pPubkxFile)fread((void *) OpubkeyX, sizeof(unsigned char), 1024, params.pPubkxFile);
			if(params.pPubkyFile)fread((void *) OpubkeyY, sizeof(unsigned char), 1024, params.pPubkyFile);
			status = functionTable->secEcdhComputeKey(xParams,ipvtkey, OpubkeyX, OpubkeyY, SSX);
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secEcdhComputeKey error %d\n", status);
				return;
			}
			if(params.pSsxFile){
				
				fwrite((void *) SSX, sizeof(unsigned char), xParams.keySize, params.pSsxFile);
				fclose(params.pSsxFile);
			}
			
		}
		else{
			printf("[SEC][Ex] Unknow operation type!\n");
			return;
		}
	
	printf("[SEC][M] ECDH test is done successfully\n");
/*
	if(params.pIpvtkFile)fread((void *) ipvtkey, sizeof(unsigned char), params.sizeKey, params.pIpvtkFile);
*/
	/* Clean up */
	if(params.pParamsFile) fclose(params.pParamsFile); 
	if(params.pIpvtkFile) fclose(params.pIpvtkFile);
	if(params.pPubkxFile) fclose(params.pPubkxFile);
	if(params.pPubkyFile) fclose(params.pPubkyFile);
	NEXUS_Memory_Free(functionTable);
    NEXUS_Platform_Uninit();

}


static void sec_harness_test_ecdh_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_ECDH_INST);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_GEN_COM);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_PARAMS);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_IPVTK);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_PUBKX);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_PUBKY);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_OPVTK);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_OPUBKX);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_OPUBKY);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_SSX);
	printf("\t%s\n\n", SEC_HELP_M_ECDH_SSY);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_ecdh_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Ecdh_Parameters * pParams)
{
	int cmdline_offset = 2;
	bool bOperationParsed = false;
	bool bGFileParsed = false;
	bool bKeySizeParsed = false;
	bool bPFileParsed = false;
	bool bIpvtkFileParsed = false; 
	bool bOpvtkFileParsered = false;
	bool bOpubkxFileParsered = false;
	bool bOpubkyFileParsered = false;
	bool bPpubkxFileParsered = false;
	
	bool bPpubkyFileParsered = false;
	bool bSsxFileParsered = false;
	bool bSsyFileParsered = false;
	bool bParamFileParsed = false; 
	FILE * pInFile = NULL;


	/* command parser state machine */
	do 
	{
		bool bCommandProcessed = false;
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		NOCS3_HARNESS_TEST_PARSE_ECDH_OPERATION_REQUIRED;
		
		NOCS3_COMMAND_PARSER_PARSE_FILE_REQUIRED (bParamFileParsed, NOCS3_HARNESS_TEST_OPTION_PARAM, &pParams->pParamsFile, "r");
		
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIpvtkFileParsed, NOCS3_HARNESS_TEST_OPTION_IPVTK,&pParams->pIpvtkFile, "rb");
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bPpubkxFileParsered, NOCS3_HARNESS_TEST_OPTION_PUBKX,&pParams->pPubkxFile, "rb");
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bPpubkyFileParsered, NOCS3_HARNESS_TEST_OPTION_PUBKY,&pParams->pPubkyFile, "rb");

		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpvtkFileParsered, NOCS3_HARNESS_TEST_OPTION_OPVTK,&pParams->pOpvtkFile, "wb");
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpubkxFileParsered, NOCS3_HARNESS_TEST_OPTION_OPUBKX,&pParams->pOpubkxFile, "wb");
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpubkyFileParsered, NOCS3_HARNESS_TEST_OPTION_OPUBKY,&pParams->pOpubkyFile, "wb");

		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bSsxFileParsered, NOCS3_HARNESS_TEST_OPTION_SSX, &pParams->pSsxFile, "wb");
/*		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bSsyFileParsered, NOCS3_HARNESS_TEST_OPTION_SSY, &pParams->pSsyFile, "wb");
*/
		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;

	} while ( cmdline_offset < argc );

	

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

