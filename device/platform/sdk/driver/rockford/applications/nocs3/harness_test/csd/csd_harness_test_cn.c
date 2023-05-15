
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_cn.h"
#include "csd_harness_test_cn_private.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


#define CSD_HARNESS_TEST_CN_MAX_DATA_SIZE 16
static void csd_harness_test_cn_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_cn_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_CN_Parameters * pParams);

static char gCnNames[4][32] =
{
	"NUIDChecknumber",
	"STBCASNChecknumber",
	"CSCDChecknumber",
	"DataIntegrityChecknumber"
};

static char gCnFuncNames[4][32] =
{
	"csdGetNUIDCheckNumber",
	"csdGetSTBCASNCheckNumber",
	"csdGetCSCDCheckNumber",
	"csdGetDataIntegrityCheckNumber"
};			



#ifdef NOCS3_DEBUG_MESSAGE
static void csd_harness_test_cn_dump_paramters (CSD_Harness_Test_CN_Parameters * pParams)
{
	printf("CSD_Harness_Test_SPV_Parameters is :\n");

	printf("cnType: %d\n", pParams->cnType);
	printf("bOutPut: %d\n", pParams->bOutPut);
	printf("sizeData 0x%x\n", pParams->sizeData);
	if ( pParams->sizeData )
	{
		nocs3_harness_test_utility_dump_array (pParams->data, pParams->sizeData);
	}

}
#endif


void csd_harness_test_cn (int argc, char* argv[])
{
	CSD_Harness_Test_CN_Parameters params;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsd4BytesVector xCheckNumber;
	unsigned i; 
	TCsdStatus status;
	NEXUS_PlatformSettings platformSettings;
	NOCS3_Harness_Test_CheckNumber_Test_Vector test_vector, * pVector = &test_vector;
	TCsd8BytesVector nuid;
	
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;

	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		csd_harness_test_cn_help();
		return;
	}

	memset (&params, 0, sizeof (CSD_Harness_Test_CN_Parameters));
	if ( (nocs3_status=csd_harness_test_cn_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("Check number command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up_1;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	csd_harness_test_cn_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	status=csdInitialize(&csdInit);
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		CSD_ERR_TRACE (status, "csdInitialize", "Operation failed");
		goto Clean_Up_1;
	}	

	if ( params.pTvFile )
	{
		status = csdGetNuid64(nuid);
		if ( status!=CSD_NO_ERROR) 
		{
			CSD_ERR_TRACE (status, "csdGetNuid64", "Cannot find NUID");
			goto Clean_Up;
		}	
		/* Get test vector */
		memset (pVector, 0, sizeof (NOCS3_Harness_Test_CheckNumber_Test_Vector));
		if ( (nocs3_status=csd_harness_test_get_cn_test_vector_4_nuid (nuid, pVector, params.pTvFile)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
		{
			/* Parser error code */
			CSD_OTHER_ERR_TRACE("Cannot find required test vector.");
			goto Clean_Up;
		}

	#ifdef NOCS3_DEBUG_MESSAGE
		csd_harness_test_cn_dump_test_vector (pVector);
	#endif
		switch (params.cnType){
		case CSD_HARNESS_TEST_CN_TYPE_CSCD:
			memcpy (params.data, pVector->cscData, pVector->sizeCscData);
			params.sizeData = pVector->sizeCscData;
			break;
		case CSD_HARNESS_TEST_CN_TYPE_STBCASN:
			memcpy (params.data, pVector->stbSn, pVector->sizeStbSn);
			params.sizeData = pVector->sizeStbSn;
			break;
		case CSD_HARNESS_TEST_CN_TYPE_DINTEG:
			memcpy (params.data, pVector->dateIntegrity, pVector->sizeDateIntegrity);
			params.sizeData = pVector->sizeDateIntegrity;
			break;

		default:
	 		CSD_OTHER_ERR_TRACE("Unknow check number type!\n");
			goto Clean_Up;		
		}
	}

    switch (params.cnType){
	case CSD_HARNESS_TEST_CN_TYPE_NUID:
		printf("calculate NUID checknumber...\n"); 
		if( csdGetNUIDCheckNumber(xCheckNumber)!=CSD_NO_ERROR){

			printf("call csdGetNUIDCheckNumber Error\n");
			return;
		}
	 break;

	 case CSD_HARNESS_TEST_CN_TYPE_CSCD:
	 	printf("Check CSCD check number...\n");
		if(csdGetCSCDCheckNumber(params.data, xCheckNumber)){
			printf("call csdGetCSCDCheckNumber Error\n");
			return;
		}
		break;
	 case CSD_HARNESS_TEST_CN_TYPE_STBCASN:
	 	printf("Check STBCASN check number...\n");
		if(csdGetSTBCASNCheckNumber(params.data, xCheckNumber)){
			printf("call csdGetSTBCASNCheckNumber Error\n");
			return;
		}
		break;

	case CSD_HARNESS_TEST_CN_TYPE_DINTEG:
	 	printf("Check STBCASN check number...\n");
		if(csdGetDataIntegrityCheckNumber(params.data, params.sizeData, xCheckNumber)){
			printf("call csdGetDataIntegrityCheckNumber Error\n");
			return;
		}
		break;

	 default:
	 	CSD_OTHER_ERR_TRACE("Unknow check number type!\n");
		goto Clean_Up;

    }

    printf("[CSD][M] - %s - 0x", gCnNames[params.cnType]);
    for(i=0; i<4; i++) 
        printf("%02x", xCheckNumber[i]); 
    printf(" - %s () done successfully\n", gCnFuncNames[params.cnType]); 
 Clean_Up:
   csdTerminateHandle.initParam = &csdInit; 
    csdTerminateHandle.keyHandleToRelease = NULL; 
    csdTerminate(&csdTerminateHandle);

Clean_Up_1:
    fflush(stdout);

	if (params.pTvFile)
	{
		fclose (params.pTvFile);
	}

    NEXUS_Platform_Uninit();

}

static void csd_harness_test_cn_help (void)
{
	printf("\t%s\n\n", CSD_HELP_M_CN_INST);
	printf("\t\t%s\n\n", CSD_HELP_M_CN_OPTIONS);
	printf("\t\t%s\n\n", CSD_HELP_M_CN_IFIC);
	printf("\t\t%s\n\n", CSD_HELP_M_CN_OC);
}


static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_cn_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_CN_Parameters * pParams)
{
	int cmdline_offset = 2;
	bool bCnTypeParsered = false;
	bool bOcParsered = false;
	bool bIfParsered = false;
	bool bIcParsered = false;
	bool bTvParsed = false;
	FILE * pInFile = NULL;

	pParams->cnType = CSD_HARNESS_TEST_CN_TYPE_MAX;

	/* command parser state machine */
	do 
	{
		bool bCommandProcessed = false;
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;

		/* CN type */
		CSD_HARNESS_TEST_PARSE_CN_TYPE;

		/* value */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bIcParsered, NOCS3_HARNESS_TEST_OPTION_IC, &pParams->data[0], &pParams->sizeData, CSD_HARNESS_TEST_CN_MAX_DATA_SIZE);

		/* Oc */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bOcParsered, NOCS3_HARNESS_TEST_OPTION_OC);

		/* If */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pInFile, "rb");
		
		/* TV */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bTvParsed, NOCS3_HARNESS_TEST_OPTION_TV, &pParams->pTvFile, "r");

		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;

	} while ( cmdline_offset < argc );

	if ( ( (bIfParsered==true) && (bIcParsered==true)) || (bCnTypeParsered==false)  )
	{
		if (pInFile!=NULL)
		{
			fclose (pInFile);
		}
		return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;	
	}
	
	if ( (bTvParsed==true) && ((bIcParsered==true) || (bIfParsered==true))  )
	{
		if (pInFile!=NULL)
		{
			fclose (pInFile);
		}
		if (pParams->pTvFile!=NULL)
		{
			fclose (pParams->pTvFile);
		}		
		return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;	
	}

	if (pInFile!=NULL)
	{
		pParams->sizeData = fread (&pParams->data[0], sizeof(unsigned char), CSD_HARNESS_TEST_CN_MAX_DATA_SIZE, pInFile);
		fclose (pInFile);
	}

	if (bOcParsered)
	{
		pParams->bOutPut = true;
	}


	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

static NOCS3_HARNESS_TEST_STATUS_CODE ctd_harness_test_cn_parse_test_vector_line (NOCS3_Harness_Test_CheckNumber_Test_Vector * pVector,
															  char * pString, 
															  unsigned int size_array)
{
	unsigned int start=0, end, size;
	
	/* nuid */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->nuid),\
												pVector->nuid,\
												2,\
												pVector->sizeNuid);

	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->nuidCn),\
												pVector->nuidCn, \
												2,\
												pVector->sizeNuidCn);


	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->stbSn),
												pVector->stbSn,\
												2,\
												pVector->sizeStbSn);

	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->stbSnCn),\
												pVector->stbSnCn,\
												2,\
												pVector->sizeStbSnCn);

	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->cscData),\
												pVector->cscData,\
												2,\
												pVector->sizeCscData);
	
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->cscDataCn),\
												pVector->cscDataCn,\
												2,\
												pVector->sizeCscDataCn);
	
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->dateIntegrity),\
												pVector->dateIntegrity,\
												2,\
												pVector->sizeDateIntegrity);
	
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->dateIntegrityCn),\
												pVector->dateIntegrityCn,\
												2,\
												pVector->sizeDateIntegrityCn);

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}
NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_get_cn_test_vector_4_nuid (T8BytesVector nuid, \
															  NOCS3_Harness_Test_CheckNumber_Test_Vector * pVector,
															  FILE * pTvFile)
{
	char String[512], *pString=String;
	bool bFound = false;

	if ( (nuid==NULL) || (pVector==NULL) || (pTvFile==NULL) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

	/* Seek to beginning of file */
	fseek (pTvFile, 0, SEEK_SET);

	/* Read header.  Just skip */
	fscanf (pTvFile, "%s", pString);
	do 
	{
		/* Read one line */
		if ((fscanf (pTvFile, "%s", pString))==EOF )
		{
			break;
		}

		if ( nocs3_harness_test_find_nuid_test_vector_line (nuid, pString, strlen(String)) == NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS )
		{

			if ( (ctd_harness_test_cn_parse_test_vector_line(pVector, pString, strlen(String))==\
						NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS))
			{
				bFound = true;
				break;
			}
		}

	} while (bFound==false);

	if (bFound==false)
	{
		return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;
	}

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;
}

NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_cn_dump_test_vector (NOCS3_Harness_Test_CheckNumber_Test_Vector * pVector)
{
	printf("sizeNuid is %d\n", pVector->sizeNuid);
	if (pVector->sizeNuid)
	{
		nocs3_harness_test_utility_dump_array(pVector->nuid, pVector->sizeNuid);
	}

	printf("sizeNuidCn is %d\n", pVector->sizeNuidCn);
	if (pVector->sizeNuidCn)
	{
		nocs3_harness_test_utility_dump_array(pVector->nuidCn, pVector->sizeNuidCn);
	}

	printf("sizeStbSn is %d\n", pVector->sizeStbSn);
	if (pVector->sizeStbSn)
	{
		nocs3_harness_test_utility_dump_array(pVector->stbSn, pVector->sizeStbSn);
	}

	printf("sizeStbSnCn is %d\n", pVector->sizeStbSnCn);
	if (pVector->sizeStbSnCn)
	{
		nocs3_harness_test_utility_dump_array(pVector->stbSnCn, pVector->sizeStbSnCn);
	}

	printf("sizeCscData is %d\n", pVector->sizeCscData);
	if (pVector->sizeCscData)
	{
		nocs3_harness_test_utility_dump_array(pVector->cscData, pVector->sizeCscData);
	}

	printf("sizeCscDataCn is %d\n", pVector->sizeCscDataCn);
	if (pVector->sizeCscDataCn)
	{
		nocs3_harness_test_utility_dump_array(pVector->cscDataCn, pVector->sizeCscDataCn);
	}

	printf("sizeDateIntegrity is %d\n", pVector->sizeDateIntegrity);
	if (pVector->sizeDateIntegrity)
	{
		nocs3_harness_test_utility_dump_array(pVector->dateIntegrity, pVector->sizeDateIntegrity);
	}

	printf("sizeDateIntegrityCn is %d\n", pVector->sizeDateIntegrityCn);
	if (pVector->sizeDateIntegrityCn)
	{
		nocs3_harness_test_utility_dump_array(pVector->dateIntegrityCn, pVector->sizeDateIntegrityCn);
	}

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;
}


