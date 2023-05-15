#include <bstd.h>
#include <stdio.h>
#include <string.h>


#include "nocs3_harness_test_common.h"
#include "nocs3_harness_test_parser.h"
#include "nocs3_harness_test_utilities.h"

char gHarnessTestBootModeString[4][16] = 
{
	NOCS3_HARNESS_TEST_BOOT_MODE_NAND_STR,		
	NOCS3_HARNESS_TEST_BOOT_MODE_NOR_STR,		
	NOCS3_HARNESS_TEST_BOOT_MODE_SPI_STR,		
	NOCS3_HARNESS_TEST_BOOT_MODE_EMMC_STR		
};

#define NOCS3_HARNESS_TEST_ACTIVATION_FLAG_SET_STR			"SET"
#define NOCS3_HARNESS_TEST_ACTIVATION_FLAG_NOT_SET_STR		"NOT SET"

char gHarnessTestActivationFlagString[2][16] =
{
	NOCS3_HARNESS_TEST_ACTIVATION_FLAG_SET_STR,
	NOCS3_HARNESS_TEST_ACTIVATION_FLAG_NOT_SET_STR
};

#define NOCS3_HARNESS_TEST_DEBUGIF_OPEN_STR				"OPEN"
#define NOCS3_HARNESS_TEST_DEBUGIF_PWD_STR				"PWD"
#define NOCS3_HARNESS_TEST_DEBUGIF_CLOSED_STR			"CLOSE"

char gHarnessTestDebugInterfaceStatusString[3][16] = 
{
	NOCS3_HARNESS_TEST_DEBUGIF_OPEN_STR,
	NOCS3_HARNESS_TEST_DEBUGIF_PWD_STR,
	NOCS3_HARNESS_TEST_DEBUGIF_CLOSED_STR
};

#define NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS_STR			"SUCCESS"
#define NOCS3_HARNESS_TEST_STATUS_CODE_FAIL_STR				"FAIL"
#define NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS_STR		"BAD PARAMETERS"
#define NOCS3_HARNESS_TEST_STATUS_CODE_NOT_SUPPORTED_STR	"NOT SUPPORTED"

char gNocsGenericErrorString [NOCS3_HARNESS_TEST_STATUS_CODE_MAX][64] = 
{
	NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS_STR,
	NOCS3_HARNESS_TEST_STATUS_CODE_FAIL_STR,
	NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS_STR,
	NOCS3_HARNESS_TEST_STATUS_CODE_NOT_SUPPORTED_STR
};

NOCS3_HARNESS_TEST nocs3_harness_test_get_test_type (int argc, char * argv[])
{
	NOCS3_HARNESS_TEST test_type = NOCS3_HARNESS_TEST_MAX;

	if ( argc <2)
	{
		test_type = NOCS3_HARNESS_TEST_HELP;
	}
	else if ( strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_R2R)==0 ) 
	{
		test_type = NOCS3_HARNESS_TEST_R2R;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_DSC)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_DSC;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_SCR)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_SCR;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_SPV)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_SPV;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_GPV)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_GPV;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_CN)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_CN;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_FP)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_FP;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_STATUS)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_STATUS;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_HELP)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_HELP;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_VERSION)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_VERSION;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_RESET)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_RESET;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_FLASH)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_FLASH;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_VERSION)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_VERSION;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_RAM)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_RAM;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_APV)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_APV;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_SUSPEND)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_SUSPEND;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_RAMSCRAM)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_RAMSCRAM;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_SCE)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_SCE;
	}

	return test_type;
}


NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_parse_file (char * arg1, char * arg2, char * argv3, FILE ** ppFile, char * pMode)
{
	NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS;

	if ( (arg1==NULL) || (arg2==NULL) || (argv3==NULL) || (ppFile==NULL) || (pMode==NULL) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

	if ( strcmp (arg1, argv3)==0)
	{
		FILE * pFile;
		pFile = fopen (arg2, pMode);
		if ( pFile==NULL )
		{
			return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
		}
		
		* ppFile = pFile;

	}
	else
	{
		status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_NOT_FOUND;
	}

	return status;

}


NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_parse_number (char * arg1, \
																				 char * arg2, \
																				 char * argv3, \
																				 unsigned int * pInt,\
																				 char * pFormat)
{
	NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS;

	if ( (arg1==NULL) || (arg2==NULL) || (argv3==NULL) || (pInt==NULL) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

	if ( strcmp (arg1, argv3)==0)
	{

		if (sscanf (arg2, pFormat, pInt)==0)
		{
			status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
		}

	}
	else
	{
		status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_NOT_FOUND;
	}

	return status;

}

NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_parse_array (char * arg1, \
																				 char * arg2, \
																				 char * argv3, \
																				 unsigned char * pArray,
																				 unsigned int * pSize,
																				 unsigned int MaxSize)
{
	NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS;

	if ( (arg1==NULL) || (arg2==NULL) || (argv3==NULL) || (pArray==NULL) || (pSize==NULL) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}
	if ( strcmp (arg1, argv3)==0)
	{
		*pSize = nocs3_harness_test_utility_ascii2array (&arg2[2], /* bypass 0x in the front */
											 pArray, 
											 MaxSize);

	}	
	else
	{
		status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_NOT_FOUND;
	}

	return status;
}

NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_parse_array_2 (char * arg1, \
																				 char * arg2, \
																				 char * argv3, \
																				 unsigned char ** pArray,
																				 unsigned int * pSize)
{
	NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS;
	unsigned int MaxSize;

	if ( (arg1==NULL) || (arg2==NULL) || (argv3==NULL) || (pArray==NULL) || (pSize==NULL) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}
	if ( strcmp (arg1, argv3)==0)
	{
		MaxSize = strlen (arg2);
		if (MaxSize<=2) 
		{
			return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
		}

		MaxSize -= 2;/* bypass 0x in the front */
		*pArray = malloc(MaxSize*sizeof(unsigned char));
		if ((*pArray)==NULL )
		{
			return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
		}
		*pSize = nocs3_harness_test_utility_ascii2array (&arg2[2], /* bypass 0x in the front */
											 *pArray, 
											 MaxSize);

	}	
	else
	{
		status = NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_NOT_FOUND;
	}

	return status;
}


NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_find_nuid_test_vector_line (T8BytesVector nuid,
															  char * pString, 
															  unsigned int size_array
															  )
{
	unsigned int start=0, end, size, sizeNuid;
	T8BytesVector nuid2;	

	/* nuid */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(nuid2),\
												nuid2,\
												2,\
												sizeNuid);
	if ((memcmp(nuid, nuid2, sizeof(nuid2))==0) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS;
	}

	return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_NOT_FOUND;
}

static NOCS3_HARNESS_TEST_STATUS_CODE nocs3_harness_test_parse_test_vector_line (NOCS3_Harness_Test_Test_Vector * pVector,
															  char * pString, 
															  unsigned int size_array)
{
	unsigned int start=0, end, size;
	
	/* nuid */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->nuid),\
												pVector->nuid,\
												2,\
												pVector->sizeNuid);

	/* cipheredProtectingKeyL1[16] */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->cipheredProtectingKeyL2),\
												pVector->cipheredProtectingKeyL2, \
												2,\
												pVector->sizeCipheredProtectingKeyL2);

	/* pciphereProtectingKeyL2[16] */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->cipheredProtectingKeyL1),\
												pVector->cipheredProtectingKeyL1,\
												2,\
												pVector->sizeCipheredProtectingKeyL1);
	
	/* cipheredContentKey[16] */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->cipheredContentKey),\
												pVector->cipheredContentKey,\
												2,\
												pVector->sizeCipheredContentKey);

	/* HostKey[16] */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->HostKey),
												pVector->HostKey,\
												2,\
												pVector->sizeHostKey);

	/* IvCbc[16] */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->IvCbc),\
												pVector->IvCbc,\
												2,\
												pVector->sizeIvCbc);

	/* IvCtr[16] */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->IvCtr),\
												pVector->IvCtr,\
												2,\
												pVector->sizeIvCtr);
	
	/* IvCiPlus[16] */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->IvCiPlus),\
												pVector->IvCiPlus,\
												2,\
												pVector->sizeIvCiPlus);
	
	/* dataIn[16] */

	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->dataIn),\
												pVector->dataIn,\
												2,\
												pVector->sizeDataIn);
	
	/* clearTextFlashProtectingKey[16] */
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->clearTextFlashProtectingKey),\
												pVector->clearTextFlashProtectingKey,\
												2,\
												pVector->sizeClearTextFlashProtectingKey);


	/*q*/
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->q),\
												pVector->q,\
												2,\
												pVector->sizeq);
	/*a*/
		NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->a),\
													pVector->a,\
													2,\
													pVector->sizea);

	/*b*/
		NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->b),\
													pVector->b,\
													2,\
													pVector->sizeb);

	/*GX*/
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->GX),\
												pVector->GX,\
												2,\
												pVector->sizeGX);
		/*GY*/
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->GY),\
												pVector->GY,\
												2,\
												pVector->sizeGY);
			/*n*/
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->n),\
												pVector->n,\
												2,\
												pVector->sizen);

				/*h*/
	NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(sizeof(pVector->h),\
												pVector->h,\
												2,\
												pVector->sizeh);

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}
NOCS3_HARNESS_TEST_STATUS_CODE nocs3_harness_test_get_test_vector_4_nuid (T8BytesVector nuid, \
															  NOCS3_Harness_Test_Test_Vector * pVector,
															  FILE * pTvFile)
{
	char String[1024], *pString=String;
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

		if ( nocs3_harness_test_find_nuid_test_vector_line (nuid, pString, strlen(String)) == \
					NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS )
		{

			if ( (nocs3_harness_test_parse_test_vector_line(pVector, pString, strlen(String))==\
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

NOCS3_HARNESS_TEST_STATUS_CODE nocs3_harness_test_dump_test_vector (NOCS3_Harness_Test_Test_Vector * pVector)
{
	printf("sizeNuid is %d\n", pVector->sizeNuid);
	if (pVector->sizeNuid)
	{
		nocs3_harness_test_utility_dump_array(pVector->nuid, pVector->sizeNuid);
	}

	printf("sizeCipheredProtectingKeyL2 is %d\n", pVector->sizeCipheredProtectingKeyL2);
	if (pVector->sizeCipheredProtectingKeyL2)
	{
		nocs3_harness_test_utility_dump_array(pVector->cipheredProtectingKeyL2, pVector->sizeCipheredProtectingKeyL2);
	}

	printf("sizeCipheredProtectingKeyL1 is %d\n", pVector->sizeCipheredProtectingKeyL1);
	if (pVector->sizeCipheredProtectingKeyL1)
	{
		nocs3_harness_test_utility_dump_array(pVector->cipheredProtectingKeyL1, pVector->sizeCipheredProtectingKeyL1);
	}

	printf("sizeCipheredContentKey is %d\n", pVector->sizeCipheredContentKey);
	if (pVector->sizeCipheredContentKey)
	{
		nocs3_harness_test_utility_dump_array(pVector->cipheredContentKey, pVector->sizeCipheredContentKey);
	}

	printf("sizeHostKey is %d\n", pVector->sizeHostKey);
	if (pVector->sizeHostKey)
	{
		nocs3_harness_test_utility_dump_array(pVector->HostKey, pVector->sizeHostKey);
	}

	printf("sizeIvCbc is %d\n", pVector->sizeIvCbc);
	if (pVector->sizeIvCbc)
	{
		nocs3_harness_test_utility_dump_array(pVector->IvCbc, pVector->sizeIvCbc);
	}

	printf("sizeIvCtr is %d\n", pVector->sizeIvCtr);
	if (pVector->sizeIvCtr)
	{
		nocs3_harness_test_utility_dump_array(pVector->IvCtr, pVector->sizeIvCtr);
	}

	printf("sizeIvCiPlus is %d\n", pVector->sizeIvCiPlus);
	if (pVector->sizeIvCiPlus)
	{
		nocs3_harness_test_utility_dump_array(pVector->IvCiPlus, pVector->sizeIvCiPlus);
	}

	printf("sizeDataIn is %d\n", pVector->sizeDataIn);
	if (pVector->sizeDataIn)
	{
		nocs3_harness_test_utility_dump_array(pVector->dataIn, pVector->sizeDataIn);
	}

	printf("sizeClearTextFlashProtectingKey is %d\n", pVector->sizeClearTextFlashProtectingKey);
	if (pVector->sizeClearTextFlashProtectingKey)
	{
		nocs3_harness_test_utility_dump_array(pVector->clearTextFlashProtectingKey, pVector->sizeClearTextFlashProtectingKey);
	}

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;
}
