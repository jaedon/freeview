
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"

#include "sec_harness_test.h"
#include "sec_harness_test_dh.h"
#include "sec_harness_test_dh_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void sec_harness_test_dh_help (void);

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_dh_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Dh_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_dh_dump_paramters (SEC_Harness_Test_Dh_Parameters * pParams)
{
	printf("SEC_Harness_Test_Dh_Parameters is :\n");
	printf("operation: %08x\n", pParams->operation);
	printf("sizeKey: %08x\n", pParams->sizeKey);
	printf("pGFile: %08x\n", pParams->pGFile);
	printf("pPFile: %08x\n", pParams->pPFile);
	printf("pIpvtkFile: %08x\n", pParams->pIpvtkFile);
	printf("pOpvtkFile: %08x\n", pParams->pOpvtkFile);
	printf("pOpubkFile: %08x\n", pParams->pOpubkFile);
	printf("pPpubkFile: %08x\n", pParams->pPpubkFile);
	printf("pSsFile: %08x\n", pParams->pSsFile);
}
#endif

void sec_harness_test_dh (int argc, char* argv[])
{
	SEC_Harness_Test_Dh_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NEXUS_PlatformSettings platformSettings;
	unsigned char p[1024] = {0}; 
	unsigned char g[1024] = {0}; 
	unsigned char ipvtkey[1024] = {0}; 
	unsigned char Opvtkey[1024] = {0}; 
	unsigned char Opubkey[1024] = {0}; 
	unsigned char out_data[1024]={0};
	TSecFunctionTable *functionTable;
	unsigned int i;
	TSecStatus status;

	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		sec_harness_test_dh_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_Dh_Parameters));
	if ( (nocs3_status=sec_harness_test_dh_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("Ram command parser failed.  Please make sure the command line parameters are valid");
		return;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_dh_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

	functionTable = secGetFunctionTable();

	if(params.pPFile)fread((void *) p, sizeof(unsigned char), params.sizeKey, params.pPFile);
	if(params.pIpvtkFile)fread((void *) ipvtkey, sizeof(unsigned char), params.sizeKey, params.pIpvtkFile);

	if(params.operation == SEC_HARNESS_TEST_DH_OPERATION_KEY_GENERATION){
		printf("Key generation...keySize %d\n", params.sizeKey);
		if(params.pGFile)fread((void *) g, sizeof(unsigned char), params.sizeKey, params.pGFile);
		
	if(params.pIpvtkFile)status = functionTable->secDhGenerateKey(g,p, ipvtkey, Opvtkey, Opubkey, params.sizeKey);
	else status = functionTable->secDhGenerateKey(g,p, NULL, Opvtkey, Opubkey, params.sizeKey);
	if(status!=SEC_NO_ERROR){
		printf("[SEC][EX] secDhGenerateKey error %d\n", status);
		return;
	}
	printf("Output private key\n");
		for(i=0; i<params.sizeKey; i++)printf("%02x,", Opvtkey[i]); 
		printf("\n"); 
		printf("Output public key\n");
		for(i=0; i<params.sizeKey; i++)printf("%02x,", Opubkey[i]); 
		printf("\n"); 

		if(params.pOpubkFile){
				fwrite((void *) Opubkey, sizeof(unsigned char), params.sizeKey, params.pOpubkFile);
				fclose(params.pOpubkFile);
		}
		if(params.pOpvtkFile){
				fwrite((void *) Opvtkey, sizeof(unsigned char), params.sizeKey, params.pOpvtkFile);
				fclose(params.pOpvtkFile);
		}
	}else if(params.operation == SEC_HARNESS_TEST_DH_OPERATION_SHARED_SECRET){
		printf("Key computation...\n");
		if(params.pPpubkFile)fread((void *) Opubkey, sizeof(unsigned char), params.sizeKey, params.pPpubkFile);
		status = functionTable->secDhComputeKey(p,ipvtkey,Opubkey,out_data,params.sizeKey);
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secDhComputeKey error %d\n", status);
			return;
				}
		printf("Outputcompute\n");
		for(i=0; i<params.sizeKey; i++)printf("%02x,", out_data[i]); 
		printf("\n");
		if(params.pSsFile){
			fwrite((void *) out_data, sizeof(unsigned char), params.sizeKey, params.pSsFile);
				fclose(params.pSsFile);
			}
	}else{
		printf("Invalid operation\n");
		return;
		}
	printf("[SEC][M] DH test is done successful\n");

	/* Clean up */
	NEXUS_Memory_Free(functionTable);
    NEXUS_Platform_Uninit();

}


static void sec_harness_test_dh_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_DH_INST);
	printf("\t%s\n\n", SEC_HELP_M_DH_GEN_COM);
	printf("\t%s\n\n", SEC_HELP_M_DH_G);
	printf("\t%s\n\n", SEC_HELP_M_DH_P);
	printf("\t%s\n\n", SEC_HELP_M_DH_KEY_SIZE);
	printf("\t%s\n\n", SEC_HELP_M_DH_IPVTK);
	printf("\t%s\n\n", SEC_HELP_M_DH_OPVTK);
	printf("\t%s\n\n", SEC_HELP_M_DH_OPUBK);
	printf("\t%s\n\n", SEC_HELP_M_DH_PPUBK);
	printf("\t%s\n\n", SEC_HELP_M_DH_SS);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_dh_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Dh_Parameters * pParams)
{
#if 1

int cmdline_offset = 2;
		bool bOperationParsed = false;
		bool bGFileParsed = false;
		bool bKeySizeParsed = false;
		bool bPFileParsed = false;
		bool bIpvtkFileParsed = false; 
		bool bOpvtkFileParsered = false;
		bool bOpubkFileParsered = false;
		bool bPpubkFileParsered = false;
		bool bSsFileParsered = false;
		FILE * pInFile = NULL;
	
	
		/* command parser state machine */
		do 
		{
			bool bCommandProcessed = false;
			NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
	
			NOCS3_HARNESS_TEST_PARSE_DH_OPERATION_REQUIRED;
			
			NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bKeySizeParsed, NOCS3_HARNESS_TEST_OPTION_KEYSIZE, &pParams->sizeKey, "0x%x");
			
	
	
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bGFileParsed, NOCS3_HARNESS_TEST_OPTION_GFILE, &pParams->pGFile, "rb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bPFileParsed, NOCS3_HARNESS_TEST_OPTION_PFILE, &pParams->pPFile, "rb");
		
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIpvtkFileParsed, NOCS3_HARNESS_TEST_OPTION_IPVTK,&pParams->pIpvtkFile, "rb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpvtkFileParsered, NOCS3_HARNESS_TEST_OPTION_OPVTK,&pParams->pOpvtkFile, "wb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOpubkFileParsered, NOCS3_HARNESS_TEST_OPTION_OPUBK,&pParams->pOpubkFile, "wb");

			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bPpubkFileParsered, NOCS3_HARNESS_TEST_OPTION_PPUBK,&pParams->pPpubkFile, "rb");

			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bSsFileParsered, NOCS3_HARNESS_TEST_OPTION_SS, &pParams->pSsFile, "wb");

			NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;
	
		} while ( cmdline_offset < argc );
	

	
#endif


	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

