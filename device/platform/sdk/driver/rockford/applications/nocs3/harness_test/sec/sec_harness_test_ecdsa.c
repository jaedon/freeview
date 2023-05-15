
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"

#include "sec_harness_test.h"
#include "sec_harness_test_ecdsa.h"
#include "sec_harness_test_ecdsa_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void sec_harness_test_ecdsa_help (void);

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_ecdsa_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Ecdsa_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_ecdsa_dump_paramters (SEC_Harness_Test_Ecdsa_Parameters * pParams)
{
	printf("SEC_Harness_Test_Ecdsa_Parameters is :\n");
	printf("operation: %08x\n", pParams->operation);
	printf("hashType: %08x\n", pParams->hashType);
	printf("messageSize: %08x\n", pParams->messageSize);
	printf("pParamsFile: %08x\n", pParams->pParamsFile);
	printf("pIpvtkFile: %08x\n", pParams->pIpvtkFile);
	printf("pOpvtkFile: %08x\n", pParams->pOpvtkFile);
	printf("pInFile: %08x\n", pParams->pInFile);
}
#endif

int sec_harness_test_ecdsa (int argc, char* argv[])
{
	SEC_Harness_Test_Ecdsa_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NEXUS_PlatformSettings platformSettings;
	unsigned char ipvtkey[1024] = {0}; 
	unsigned char Opvtkey[1024] = {0}; 
	unsigned char OpubkeyX[1024] = {0}; 
	unsigned char OpubkeyY[1024] = {0}; 
	unsigned char SSIG[1024] = {0}; 
	unsigned char RSIG[1024] = {0}; 
	unsigned char in_data[1024] = {0}; 
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
		sec_harness_test_ecdsa_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_Ecdsa_Parameters));
	if ( (nocs3_status=sec_harness_test_ecdsa_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("Ram command parser failed.  Please make sure the command line parameters are valid");
		return 1;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_ecdsa_dump_paramters (&params);
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
	
		if(params.pIpvtkFile)keySize = fread((void *) ipvtkey, sizeof(unsigned char), 1024, params.pIpvtkFile);
		if(params.operation == SEC_HARNESS_TEST_ECDSA_OPERATION_KEY_GENERATION){
			if(params.pIpvtkFile)status=functionTable->secEcdsaGenerateKey(xParams,ipvtkey,Opvtkey, OpubkeyX, OpubkeyY);
			else status = functionTable->secEcdsaGenerateKey(xParams,NULL,Opvtkey, OpubkeyX, OpubkeyY);
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secEcdsaGenerateKey error %d\n", status);
				return 1;
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
		else if(params.operation == SEC_HARNESS_TEST_ECDSA_OPERATION_SIGN){


			if(params.pInFile)fread((void *) in_data, sizeof(unsigned char), 1024, params.pInFile);

			if(params.pIpvtkFile) status = functionTable->secEcdsaSign(xParams, params.hashType, ipvtkey, in_data, params.messageSize, RSIG, SSIG);
			else status = functionTable->secEcdsaSign(xParams, params.hashType, NULL, in_data, params.messageSize, RSIG, SSIG);
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secEcdsaSign error %d\n", status);
				return 1;
			}
			if(params.pSSFile){
				
				fwrite((void *) SSIG, sizeof(unsigned char), xParams.keySize, params.pSSFile);
			}
			if(params.pSRFile){
				
				fwrite((void *) RSIG, sizeof(unsigned char), xParams.keySize, params.pSRFile);
			}
		}
		else if(params.operation == SEC_HARNESS_TEST_ECDSA_OPERATION_VERIFY){
			
			if(params.pSSFile)fread((void *) SSIG, sizeof(unsigned char), 1024, params.pSSFile);
			else {
				printf("[SEC}[EX}S Signature file missing.\n");
				return 1;
			}
			if(params.pSRFile)fread((void *) RSIG, sizeof(unsigned char), 1024, params.pSRFile);
			else{
				
				printf("R Signature file missing.\n");
				return 1;
				}
			if(params.pInFile)fread((void *) in_data, sizeof(unsigned char), 1024, params.pInFile);
			if(params.pOpubkxFile)fread((void *) OpubkeyX, sizeof(unsigned char), 1024, params.pOpubkxFile);

			if(params.pOpubkxFile)fread((void *) OpubkeyY, sizeof(unsigned char), 1024, params.pOpubkyFile);
			status = functionTable->secEcdsaVerify(xParams, params.hashType, OpubkeyX, OpubkeyY, in_data, params.messageSize,RSIG, SSIG); 
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secEcdsaVerify error %d\n", status);
				return status ;
			}
		}
		else{
			printf("[SEC][Ex] Unknow operation type!\n");
			return 1;
		}
	
	printf("[SEC][M] ECDH test is done successfully\n");
/*
	if(params.pIpvtkFile)fread((void *) ipvtkey, sizeof(unsigned char), params.sizeKey, params.pIpvtkFile);
*/
	/* Clean up */
	if(params.pParamsFile) fclose(params.pParamsFile); 
	if(params.pIpvtkFile) fclose(params.pIpvtkFile);
	if(params.pSSFile) fclose(params.pSSFile);
	if(params.pSRFile) fclose(params.pSRFile);
	if(params.pInFile)fclose(params.pInFile); 
	NEXUS_Memory_Free(functionTable);

	/* Clean up */
	fflush(stdout);
    NEXUS_Platform_Uninit();
	return 0;  
}


static void sec_harness_test_ecdsa_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_INST);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_OP);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_PARAMS);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_IPVTK);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_HASH);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_IF);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_VALUE);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_OPVTK);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_PUBKX);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_PUBKY);
	printf("\t%s\n\n", SEC_HELP_M_ECDSA_SOFIF);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_ecdsa_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Ecdsa_Parameters * pParams)
{
	int cmdline_offset = 2;
	bool bOperationParsed = false;
		bool bIpvtkFileParsed = false; 
	bool bOpvtkFileParsered = false;
	bool bOpubkxFileParsered = false;
	bool bOpubkyFileParsered = false;
		bool bSRofileParsered = false;
		bool bSSofileParsered = false;
	bool bsSifileParsered = false;
	bool bsRifileParsered = false;
	bool bParamFileParsed = false; 
	
	bool bHtParsered = false;
	bool bHTypeParsered = false;
	bool bMsParsered = false;
	bool bInParsered = false;
	/* command parser state machine */
	do 
	{
		bool bCommandProcessed = false;
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		NOCS3_HARNESS_TEST_PARSE_ECDSA_OPERATION_REQUIRED;
		
		NOCS3_COMMAND_PARSER_PARSE_FILE_REQUIRED (bParamFileParsed, NOCS3_HARNESS_TEST_OPTION_PARAM, &pParams->pParamsFile, "r");
		
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(bHtParsered, NOCS3_HARNESS_TEST_OPTION_HT);
		if(bHtParsered) SEC_HARNESS_TEST_PARSE_HT_TYPE;
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIpvtkFileParsed, NOCS3_HARNESS_TEST_OPTION_IPVTK,&pParams->pIpvtkFile, "rb");

		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpvtkFileParsered, NOCS3_HARNESS_TEST_OPTION_OPVTK,&pParams->pOpvtkFile, "wb");
		if(pParams->operation == SEC_HARNESS_TEST_ECDSA_OPERATION_KEY_GENERATION){
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpubkxFileParsered, NOCS3_HARNESS_TEST_OPTION_PUBKX,&pParams->pOpubkxFile, "wb");
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpubkyFileParsered, NOCS3_HARNESS_TEST_OPTION_PUBKY,&pParams->pOpubkyFile, "wb");
			}else{
			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpubkxFileParsered, NOCS3_HARNESS_TEST_OPTION_PUBKX,&pParams->pOpubkxFile, "rb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpubkyFileParsered, NOCS3_HARNESS_TEST_OPTION_PUBKY,&pParams->pOpubkyFile, "rb");
				}
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bsSifileParsered, NOCS3_HARNESS_TEST_OPTION_SSIF, &pParams->pSSFile, "rb");
		
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bsRifileParsered, NOCS3_HARNESS_TEST_OPTION_SRIF, &pParams->pSRFile, "rb");

		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bMsParsered, NOCS3_HARNESS_TEST_OPTION_MS, &pParams->messageSize, "0x%x");

		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bSSofileParsered, NOCS3_HARNESS_TEST_OPTION_SSOF, &pParams->pSSFile, "wb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bSRofileParsered, NOCS3_HARNESS_TEST_OPTION_SROF, &pParams->pSRFile, "wb");
/*if*/
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bInParsered, NOCS3_HARNESS_TEST_OPTION_IN, &pParams->pInFile, "rb");

		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;

	} while ( cmdline_offset < argc );



	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

