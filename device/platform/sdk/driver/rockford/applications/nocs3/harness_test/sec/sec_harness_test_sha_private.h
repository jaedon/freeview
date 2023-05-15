
#ifndef __NOCS3_SEC_HARNESS_TEST_SHA_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_SHA_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE 384
/* DH specific help message */
#define SEC_HELP_M_SHA_INST		"-sha = This command allows to perform the SHA functionality. This command shall be \
followed by SHA1 or SHA256."
#define SEC_HELP_M_SHA_IF		"-if InFile = The input file, on which the SHA operation is performed."
#define SEC_HELP_M_SHA_OF		"-of OutFile = The output file, where the SHA data is stored."


#define SEC_HARNESS_TEST_SHA_TYPE_SHA1_STR		"SHA1"
#define SEC_HARNESS_TEST_SHA_TYPE_SHA256_STR	"SHA256"
#define SEC_HARNESS_TEST_SHA_TYPE_SHA384_STR	"SHA384"
#define SEC_HARNESS_TEST_PARSE_SHA_TYPE \
	if ( bopTypeParsered == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_SHA_TYPE_SHA1_STR)==0 )\
			{\
				pParams->operation = SEC_HARNESS_TEST_SHA_OPERATION_SHA1;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_SHA_TYPE_SHA256_STR)==0 )\
			{\
				pParams->operation = SEC_HARNESS_TEST_SHA_OPERATION_SHA256;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_SHA_TYPE_SHA384_STR)==0 )\
			{\
				pParams->operation = SEC_HARNESS_TEST_SHA_OPERATION_SHA384;\
			}\
			if ( pParams->operation != SEC_HARNESS_TEST_SHA_OPERATION_MAX )\
			{\
				bopTypeParsered = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}\

typedef enum SEC_HARNESS_TEST_SHA_OPERATION
{
	SEC_HARNESS_TEST_SHA_OPERATION_SHA1,
	SEC_HARNESS_TEST_SHA_OPERATION_SHA256,
	SEC_HARNESS_TEST_SHA_OPERATION_SHA384,
	/* Add new enum before this line */
	SEC_HARNESS_TEST_SHA_OPERATION_MAX
}	SEC_HARNESS_TEST_SHA_OPERATION;


typedef struct SEC_Harness_Test_Sha_Parameters
{
	SEC_HARNESS_TEST_SHA_OPERATION		operation;

	FILE *								pInFile;
	FILE *								pOutFile;
	unsigned int						sizeData;
	unsigned char						data[SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE];

}  SEC_Harness_Test_Sha_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_SHA_PRIVATE_H__ */