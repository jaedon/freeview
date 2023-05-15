
#ifndef __NOCS3_SEC_HARNESS_TEST_DH_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_DH_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


#define SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE 2048
/* DH specific help message */
#define SEC_HELP_M_DH_INST		"-dh = This command allows to perform the Diffe-Hellman functionality."
#define SEC_HELP_M_DH_GEN_COM	"-gen|com = -gen defines generation of public/private key pair. -com \
defines computation of shared secret from public, private key."
#define SEC_HELP_M_DH_G			"[-G FileName] = Optional Parameter.  This defines the primitive root mode p \
to generate key pair.  Taken from file input.  This option is not used during the DH computation."
#define SEC_HELP_M_DH_P			"-P FileName = Optional Parameter.  This defines the prime p, used \
to generate key pair.  Taken from file input."
#define SEC_HELP_M_DH_KEY_SIZE	"-keysize Value = Indicates the RSA key size in bytes."
#define SEC_HELP_M_DH_IPVTK		"[-iptvk FileName] = Optional Parameter.  If private key is provided as input, \
this function generates a random private key. Taken from file input.  During DH computation is not used."
#define SEC_HELP_M_DH_OPVTK		"[-optvk FileName] = Optional Parameter.  Generated output private key.  This \
operation is not used during the DH computation."
#define SEC_HELP_M_DH_OPUBK		"[-opubk FileName] = Optional Parameter.  Generated output public key.  This \
operation is not used during the DH computation."
#define SEC_HELP_M_DH_PPUBK		"[-ppubk FileName] = Optional Parameter.  Needed other peer public key.  This \
operation is not used during the DH computation."
#define SEC_HELP_M_DH_SS		"-ss FileName = Optional Parameter.  Computed shared secret.  This \
operation is not used during the DH computation."

#define NOCS3_HARNESS_TEST_PARSE_DH_OPERATION_REQUIRED \
	NOCS3_COMMAND_PARSER_PARSE_BINARY_TYPE_REQUIRED(bOperationParsed,\
	pParams->operation,\
	NOCS3_HARNESS_TEST_OPTION_KEYGEN,\
	SEC_HARNESS_TEST_DH_OPERATION_KEY_GENERATION,\
	NOCS3_HARNESS_TEST_OPTION_KEYCOMP,\
	SEC_HARNESS_TEST_DH_OPERATION_SHARED_SECRET);

typedef enum SEC_HARNESS_TEST_DH_OPERATION
{
	SEC_HARNESS_TEST_DH_OPERATION_KEY_GENERATION,
	SEC_HARNESS_TEST_DH_OPERATION_SHARED_SECRET,

	/* Add new enum before this line */
	SEC_HARNESS_TEST_DH_OPERATION_MAX
}	SEC_HARNESS_TEST_DH_OPERATION;


typedef struct SEC_Harness_Test_Dh_Parameters
{
	SEC_HARNESS_TEST_DH_OPERATION		operation;
	unsigned int						sizeKey;

	FILE *								pGFile;
	FILE *								pPFile;
	FILE *								pIpvtkFile;
	FILE *								pOpvtkFile;
	FILE *								pOpubkFile;
	FILE *								pPpubkFile;
	FILE *								pSsFile;

}  SEC_Harness_Test_Dh_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_DH_PRIVATE_H__ */