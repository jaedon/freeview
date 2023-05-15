
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"

#include "sec_harness_test.h"
#include "sec_harness_test_rand.h"
#include "sec_harness_test_rand_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void sec_harness_test_rand_help (void);

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_rand_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Rand_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_rand_dump_paramters (SEC_Harness_Test_Rand_Parameters * pParams)
{
	printf("SEC_Harness_Test_Rand_Parameters is :\n");
	printf("size: %08x\n", pParams->size);
	printf("pOutFile: %08x\n", pParams->pOutFile);
}
#endif

void sec_harness_test_rand (int argc, char* argv[])
{
	SEC_Harness_Test_Rand_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NEXUS_PlatformSettings platformSettings;
	TSecFunctionTable *functionTable;
	TUnsignedInt8*  data;
	unsigned int i;
	TSecStatus status;
	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		sec_harness_test_rand_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_Rand_Parameters));
	if ( (nocs3_status=sec_harness_test_rand_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("Ram command parser failed.  Please make sure the command line parameters are valid");
		return;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_rand_dump_paramters (&params);
#endif
	if(params.size)data = malloc(params.size);
	else return;
	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	functionTable = secGetFunctionTable();
	if((status=functionTable->secGenerateRandomBytes(params.size, data))!=SEC_NO_ERROR){
		printf("[SEC][EX] secGenerateRandomBytes error %d\n", status);
		return;
	}
	printf("[SEC][M] Random number generation is successful\n");
#if 0
	for(i=0; i<params.size; i++)printf("%02x ", data[i]);
			printf("\n"); 
#endif
	if(params.pOutFile){
		fwrite((void *) data, sizeof(unsigned char), params.size, params.pOutFile);
		fclose(params.pOutFile);
	}
	else if(params.bOutPut){
		for(i=0; i<params.size; i++)printf("%02x ", data[i]);
		printf("\n"); 
	}
	/* Clean up */
	
	NEXUS_Memory_Free(functionTable);
    NEXUS_Platform_Uninit();

}


static void sec_harness_test_rand_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_RAND_INST);
	printf("\t%s\n\n", SEC_HELP_M_RAND_SIZE);
	printf("\t%s\n\n", SEC_HELP_M_RAND_OFOC);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_rand_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Rand_Parameters * pParams)
{

	int cmdline_offset = 2;
	bool bNumParsered = false;
	bool bOcParsered = false;
	bool bOfParsered = false;


	/* command parser state machine */
	do 
	{
		bool bCommandProcessed = false;
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;

		
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bNumParsered, NOCS3_HARNESS_TEST_OPTION_RANDLEN, &pParams->size, "0x%x");
		/* Oc */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bOcParsered, NOCS3_HARNESS_TEST_OPTION_OC);

		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "wb");


	} while ( cmdline_offset < argc );



	if (bOcParsered)
	{
		pParams->bOutPut = true;
	}

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

