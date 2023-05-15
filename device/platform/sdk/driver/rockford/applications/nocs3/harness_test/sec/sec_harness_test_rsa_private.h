
#ifndef __NOCS3_SEC_HARNESS_TEST_RSA_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_RSA_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif



/* R2R specific help message */
#define SEC_HELP_M_RSA_INST		"-rsa = This command allows to perform the RSA functionality."
#define SEC_HELP_M_RSA_GEN_PUB_PVT	"-gen|-pub|-pvt = Option -gen is used while generating the RSA key pair. \
Options -pub and -pvt defines the public or private operation respectively, performed by the RSA functionality"
#define SEC_HELP_M_ENC_DEC		"[-enc|-dec] = This defines operation performed by RSA functionality. If -enc, \
then encryption operation, if -dec, then decryption operation.  The option -enc/dec is only applicable when \
-gen is not used."
#define SEC_HELP_M_RSA_KEYSIZE		"-keysize Value = Indicates the key size in bytes."
#define SEC_HELP_M_RSA_EXPE		"[-expE Value] = Optional Parameter. Public RSA exponent e used for RSA operation.\
 NOT applicable if -pvt is used."
#define SEC_HELP_M_RSA_EXPD		"[-expD expDFile] = Optional Parameter. Prviate RSA exponent e used for RSA operation \
provided through a binary file as input.  When used with -gen, it generates the private exponent d."
#define SEC_HELP_M_RSA_MODN		"[-modN ModFile] = RSA module N used for the RSA operation provided through a binary \
file as input."
#define SEC_HELP_M_RSA_PAD		"[-pad PKCA1|OAEP|NO|OTH] = Optional Parameter. One of the valid padding to be used. \
OTH is other padding scheme than PKCS, OAEP. NO is no padding applicable.  The -pad option is only applicable when -gen \
is not used."
#define SEC_HELP_M_RSA_IF		"[-if InFile] = Optional Parameter. The path and name of the input file containing data, \
to be used for RSA enc/dec operation. Not used with -gen."
#define SEC_HELP_M_RSA_OF		"-of OutFile] = The path and name of the output file containing data, \
to be used for RSA enc/dec operation. "


#define SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE 2048

#define SEC_HARNESS_TEST_PARSE_PAD_TYPE \
	if ( bpadTypeParsered == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_PAD_TYPE_PKCA1_STR)==0 )\
			{\
				pParams->padding = SEC_RSA_PKCS1_V1_5_PADDING;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA1_STR)==0 )\
			{\
				pParams->padding = SEC_RSA_OAEP_SHA_1_PADDING;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_PAD_TYPE_NO_STR)==0 )\
			{\
				pParams->padding = SEC_RSA_NO_PADDING;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA256_STR)==0 )\
			{\
				pParams->padding = SEC_RSA_OAEP_SHA_256_PADDING;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA384_STR)==0 )\
			{\
				pParams->padding = SEC_RSA_OAEP_SHA_384_PADDING;\
			}\
			if ( pParams->padding != SEC_HARNESS_TEST_PAD_TYPE_MAX )\
			{\
			bpadTypeParsered = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}\

#define SEC_HARNESS_TEST_PAD_TYPE_PKCA1_STR		"PKCS1_5"
#define SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA1_STR	"OAEP_SHA1"
#define SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA256_STR	"OAEP_SHA256"
#define SEC_HARNESS_TEST_PAD_TYPE_NO_STR		"NO"
#define SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA384_STR	"OAEP_SHA384"


typedef enum SEC_HARNESS_TEST_PAD_TYPE
{
	SEC_HARNESS_TEST_PAD_TYPE_PKCS1_5,
	SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA1,
	SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA256,
	SEC_HARNESS_TEST_PAD_TYPE_NO,
	SEC_HARNESS_TEST_PAD_TYPE_OAEP_SHA384,

	/* Add new enum before this line */
	SEC_HARNESS_TEST_PAD_TYPE_MAX
}	SEC_HARNESS_TEST_PAD_TYPE;

typedef struct SEC_Harness_Test_Rsa_Parameters
{
	NOCS3_HARNESS_TEST_OPERATION		operation;
	unsigned int						sizeKey;
	bool 								keyGen;
	bool 								rsaCmp;
	bool								bPublic;
	bool								bPrivate;
	unsigned long 						expE;
	SEC_HARNESS_TEST_PAD_TYPE	    padding;
	size_t								keySize; 
	FILE *								pExpDFile;
	FILE *								pModFile;
	FILE *								pPFile;
	FILE *								pQFile;
	FILE *								pDPFile;
	FILE *								pDQFile;
	FILE *								pQinvFile;

	FILE *								pInFile;
	FILE *								pOutFile;
	unsigned int						sizeData;
	unsigned char						data[SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE];
}  SEC_Harness_Test_Rsa_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_RSA_PRIVATE_H__ */