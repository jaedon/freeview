
#ifndef __NOCS3_SEC_HARNESS_TEST_ECDSA_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_ECDSA_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE 4096
/* ECDSA specific help message */
#define SEC_HELP_M_ECDSA_INST		"-ecdsa = This command allows to perform the Elliptic curve Diffe-Hellman functionality."
#define SEC_HELP_M_ECDSA_OP			"-gen|-sign|-verify = -gen defines generation of public/private key pair. -sign \
defines signing a message. -verify defines verifying ECDSA signature."
#define SEC_HELP_M_ECDSA_PARAMS		"-params FileName = Parameter taken from .csv file. Containing the ECDSA needed domain \
params such as q, a, b, GX, GY, n and h."
#define SEC_HELP_M_ECDSA_IPVTK		"-iptvk FileName = Taken from file input, if key file is provided.  If no private key \
is provided as input, this function generates a random private key. During ECDSA sign this is the private key used for signing \
the message.  This option is not used for verify."
#define SEC_HELP_M_ECDSA_HASH		"[-ht SHA1|SHA256|OTH] = Optional Parameter.  One of the hashing method to be used. OTH is \
other hashing type than SHA1 and SHA256."
#define SEC_HELP_M_ECDSA_IF			"[-in InFile] = Optional Parameter.  File containing the message to be signed or verified."
#define SEC_HELP_M_ECDSA_VALUE		"[-ms Value] = Optional Parameter.  Size in bytes of the message to be signed."
#define SEC_HELP_M_ECDSA_OPVTK		"[-optvk FileName] = Optional Parameter.  Generated output private key.  This \
operation is not used during the ECDSA sign and verify."
#define SEC_HELP_M_ECDSA_PUBKX		"[-pubkx FileName] = Optional Parameter. The X coordinate of the generated public key. \
public key.  Output parameter for option -gen.  Input parameter for option -verify. This operation is not used during the ECDSA sign."
#define SEC_HELP_M_ECDSA_PUBKY		"[-pubky FileName] = Optional Parameter. The Y coordinate of the generated public key. \
public key.  Output parameter for option -gen.  Input parameter for option -verify. This operation is not used during the ECDSA sign."
#define SEC_HELP_M_ECDSA_SOFIF		"[-sof|-sif FileName] = Optional Parameter. -sof describes file where the signature is stored \
for the sign. For the option -verify, -sif must be used as input file. Option not used for -gen. "

#define NOCS3_HARNESS_TEST_PARSE_ECDSA_OPERATION_REQUIRED \
	if ( bOperationParsed == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_OPTION_KEYGEN)==0 )\
			{\
				pParams->operation = SEC_HARNESS_TEST_ECDSA_OPERATION_KEY_GENERATION;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_OPTION_SIGN)==0 )\
			{\
				pParams->operation = SEC_HARNESS_TEST_ECDSA_OPERATION_SIGN;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_OPTION_VERIFY)==0 )\
			{\
				pParams->operation = SEC_HARNESS_TEST_ECDSA_OPERATION_VERIFY;\
			}\
			if ( pParams->operation != SEC_HARNESS_TEST_ECDSA_OPERATION_MAX )\
			{\
				bOperationParsed = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}\


#define SEC_HARNESS_TEST_PARSE_HT_TYPE \
	if ( bHTypeParsered == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_HT_TYPE_SHA1_STR)==0 )\
			{\
				pParams->hashType = SEC_HARNESS_TEST_ECDSA_HASH_SHA1;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_HT_TYPE_SHA256_STR)==0 )\
			{\
				pParams->hashType = SEC_HARNESS_TEST_ECDSA_HASH_SHA256;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_HT_TYPE_OTH_STR)==0 )\
			{\
				pParams->hashType = SEC_HARNESS_TEST_ECDSA_HASH_OTHER;\
			}\
			if ( pParams->hashType != SEC_HARNESS_TEST_ECDSA_HASH_MAX )\
			{\
				bHTypeParsered = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}\

#define SEC_HARNESS_TEST_HT_TYPE_SHA1_STR		"SHA1"
#define SEC_HARNESS_TEST_HT_TYPE_SHA256_STR		"SHA256"
#define SEC_HARNESS_TEST_HT_TYPE_OTH_STR		"OTH"

typedef enum SEC_HARNESS_TEST_ECDSA_OPERATION
{
	SEC_HARNESS_TEST_ECDSA_OPERATION_KEY_GENERATION,
	SEC_HARNESS_TEST_ECDSA_OPERATION_SIGN,
	SEC_HARNESS_TEST_ECDSA_OPERATION_VERIFY,

	/* Add new enum before this line */
	SEC_HARNESS_TEST_ECDSA_OPERATION_MAX
}	SEC_HARNESS_TEST_ECDSA_OPERATION;

typedef enum SEC_HARNESS_TEST_ECDSA_HASH
{
	SEC_HARNESS_TEST_ECDSA_HASH_SHA1,
	SEC_HARNESS_TEST_ECDSA_HASH_SHA256,
	SEC_HARNESS_TEST_ECDSA_HASH_OTHER,

	/* Add new enum before this line */
	SEC_HARNESS_TEST_ECDSA_HASH_MAX
}	SEC_HARNESS_TEST_ECDSA_HASH;


typedef struct SEC_Harness_Test_Ecdsa_Parameters
{
	SEC_HARNESS_TEST_ECDSA_OPERATION	operation;
	SEC_HARNESS_TEST_ECDSA_HASH			hashType;
	unsigned int						messageSize;

	FILE *								pParamsFile;
	FILE *								pIpvtkFile;
	FILE *								pOpubkxFile;
	FILE *								pOpubkyFile;
	FILE *								pOpvtkFile;
	FILE *								pInFile;
	FILE *								pSRFile;
	FILE *								pSSFile;

}  SEC_Harness_Test_Ecdsa_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_ECDSA_PRIVATE_H__ */
