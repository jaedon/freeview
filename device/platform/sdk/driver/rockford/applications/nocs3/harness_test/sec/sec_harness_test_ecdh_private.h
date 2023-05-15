
#ifndef __NOCS3_SEC_HARNESS_TEST_ECDH_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_ECDH_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE 2048
/* ECDH specific help message */
#define SEC_HELP_M_ECDH_INST		"-ecdh = This command allows to perform the Elliptic curve Diffe-Hellman functionality."
#define SEC_HELP_M_ECDH_GEN_COM	"-gen|com = -gen defines generation of public/private key pair. -com \
defines computation of shared secret from public, private key."
#define SEC_HELP_M_ECDH_PARAMS		"[-params FileName] = Parameter taken from .csv file. Containing the ECDH needed domain \
params such as q, a, b, GX, GY, n and h."
#define SEC_HELP_M_ECDH_IPVTK		"-iptvk FileName = Taken from file input, if key file is provided.  If no private key \
is provided as input, this function generates a random private key. During ECDH computation this is the private key."
#define SEC_HELP_M_ECDH_PUBKX		"[-pubkx FileName] = Optional Input Parameter. The X coordinate of other peer's ECDH \
public key.  This operation is not used during the ECDH computation."
#define SEC_HELP_M_ECDH_PUBKY		"[-pubky FileName] = Optional Input Parameter. The Y coordinate of other peer's ECDH \
public key.  This operation is not used during the ECDH computation."
#define SEC_HELP_M_ECDH_OPVTK		"[-optvk FileName] = Optional Parameter.  Generated output private key.  This \
operation is not used during the ECDH computation."
#define SEC_HELP_M_ECDH_OPUBKX		"[-opubkx FileName] = Optional Parameter.  The X coordinate of generated output public key.  This \
operation is not used during the ECDH computation."
#define SEC_HELP_M_ECDH_OPUBKY		"[-opubky FileName] = Optional Parameter.  The Y coordinate of generated output public key.  This \
operation is not used during the ECDH computation."
#define SEC_HELP_M_ECDH_SSX		"[-ssx FileName] = Optional Parameter.  The X coordinate of computed shared secret.  This \
operation is not used during the ECDH computation."
#define SEC_HELP_M_ECDH_SSY		"[-ssy FileName] = Optional Parameter.  The Y coordinate of computed shared secret.  This \
operation is not used during the ECDH computation."

#define NOCS3_HARNESS_TEST_PARSE_ECDH_OPERATION_REQUIRED \
	NOCS3_COMMAND_PARSER_PARSE_BINARY_TYPE_REQUIRED(bOperationParsed,\
	pParams->operation,\
	NOCS3_HARNESS_TEST_OPTION_KEYGEN,\
	SEC_HARNESS_TEST_ECDH_OPERATION_KEY_GENERATION,\
	NOCS3_HARNESS_TEST_OPTION_KEYCOMP,\
	SEC_HARNESS_TEST_ECDH_OPERATION_SHARED_SECRET);

typedef enum SEC_HARNESS_TEST_ECDH_OPERATION
{
	SEC_HARNESS_TEST_ECDH_OPERATION_KEY_GENERATION,
	SEC_HARNESS_TEST_ECDH_OPERATION_SHARED_SECRET,

	/* Add new enum before this line */
	SEC_HARNESS_TEST_ECDH_OPERATION_MAX
}	SEC_HARNESS_TEST_ECDH_OPERATION;


typedef struct SEC_Harness_Test_Ecdh_Parameters
{
	SEC_HARNESS_TEST_ECDH_OPERATION		operation;

	FILE *								pParamsFile;
	FILE *								pIpvtkFile;
	FILE *								pPubkxFile;
	FILE *								pPubkyFile;
	FILE *								pOpvtkFile;
	FILE *								pOpubkxFile;
	FILE *								pOpubkyFile;
	FILE *								pSsxFile;
	FILE *								pSsyFile;

}  SEC_Harness_Test_Ecdh_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_ECDH_PRIVATE_H__ */