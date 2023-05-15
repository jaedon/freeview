
#ifndef __NOCS3_SEC_HARNESS_TEST_HMAC_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_HMAC_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* HMAC specific help message */
#define SEC_HELP_M_HMAC_INST	"-hmac = This command allows to perform the HMAC-SHA256 functionality. This command shall be \
followed SHA256."
#define SEC_HELP_M_HMAC_KEY		"-kf KeyFile|-kc = The needed key can be provided through a file or the key can be passed as \
command line argument."
#define SEC_HELP_M_HMAC_IF		"-if InFile = The input file, on which the SHA operation is performed."
#define SEC_HELP_M_HMAC_OF		"-of OutFile = The output file, where the SHA data is stored."
#define SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE 384


#define SEC_HARNESS_TEST_HMAC_KEYSIZE	64
#define SEC_HARNESS_TEST_SHA_TYPE_SHA1_STR		"SHA1"
#define SEC_HARNESS_TEST_SHA_TYPE_SHA256_STR	"SHA256"

#define SEC_HARNESS_TEST_PARSE_SHA_TYPE \
	if ( bopTypeParsered == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_SHA_TYPE_SHA256_STR)==0 )\
			{\
				bopTypeParsered = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
			else \
			{\
				return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
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

	/* Add new enum before this line */
	SEC_HARNESS_TEST_SHA_OPERATION_MAX
}	SEC_HARNESS_TEST_SHA_OPERATION;


typedef struct SEC_Harness_Test_Hmac_Parameters
{
	unsigned char						key[SEC_HARNESS_TEST_HMAC_KEYSIZE];
	unsigned int						sizeKey;
	FILE *								pInFile;
	FILE *								pOutFile;
	FILE *								pKeyFile;
	unsigned int						sizeData;
	unsigned char						data[SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE];

}  SEC_Harness_Test_Hmac_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_HMAC_PRIVATE_H__ */
