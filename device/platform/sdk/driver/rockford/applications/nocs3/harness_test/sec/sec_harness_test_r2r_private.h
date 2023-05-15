
#ifndef __NOCS3_SEC_HARNESS_TEST_R2R_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_R2R_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "sec_harness_test_r2r_common.h"

/* R2R specific options */

/* R2R specific help message */
#define SEC_HELP_M_R2R_INST		"-r2r = This command allows to perform RAM2RAM functionality"
#define SEC_HELP_M_R2R_OP		"-enc|dec = This defines operation peformed by RAM2RAM functionality. If \
-enc is an encryption operation.  If -dec is a decryption operation"
#define SEC_HELP_M_R2R_IFIC		"[-if InFile|-ic Value64|Value128] = Value used as an input to RAM2RAM. \
Optional parameter.  The value can be taken from DataIn file (-if) or a 64bit/128bit value from the \
command line (-ic).  If both options are not present then it must be taken from .csv file provided with \
option -tv"
#define SEC_HELP_M_R2R_OFOC		"-of OutFile|-oc = The path and file name of the output file (-of) or \
computed data value printed on standard out (-oc)"
#define SEC_HELP_M_R2R_MT		"[-mt] = Monte Carlo test. Optional parameter"
#define SEC_HELP_M_R2R_RUNS		"-numbruns # = How many times the input data must be sourced by the generated \
output.  This command is mostly used for helping in the Monte Carlo test"

#define SEC_HARNESS_TEST_PARITY_TYPE_EVEN_STR		"EVEN"
#define SEC_HARNESS_TEST_PARITY_TYPE_ODD_STR	"ODD"
#define SEC_HARNESS_TEST_PARITY_TYPE_UNDEFINED_STR	"UNDEFINED"

#define SEC_HARNESS_TEST_KEY_TYPE_SW_STR		"SW"
#define SEC_HARNESS_TEST_KEY_TYPE_KL_STR	"KL"
#define SEC_HARNESS_TEST_KEY_TYPE_CERT_STR	"CERT"

#define SEC_HARNESS_TEST_PARSE_PARITY_TYPE \
	if ( bParityParsed == true )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_PARITY_TYPE_EVEN_STR)==0 )\
			{\
				pParams->parity = SEC_KEY_PARITY_EVEN;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_PARITY_TYPE_ODD_STR)==0 )\
			{\
				pParams->parity = SEC_KEY_PARITY_ODD;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_PARITY_TYPE_UNDEFINED_STR)==0 )\
			{\
				pParams->parity = SEC_KEY_PARITY_UNDEFINED;\
			}\
			if ( pParams->parity != SEC_HARNESS_TEST_PARITY_TYPE_MAX )\
			{\
				bParityTypeParsed = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}\

#define SEC_HARNESS_TEST_PARSE_KEY_TYPE \
	if ( bKeyParsed == true )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_KEY_TYPE_SW_STR)==0 )\
			{\
				pParams->keyType = NOCS3_HARNESS_TEST_KEY_SOFTWARE;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_KEY_TYPE_KL_STR)==0 )\
			{\
				pParams->keyType = NOCS3_HARNESS_TEST_KEY_KEYLADDER;\
			}\
			else if ( strcmp (argv[cmdline_offset], SEC_HARNESS_TEST_KEY_TYPE_CERT_STR)==0 )\
			{\
				pParams->keyType = NOCS3_HARNESS_TEST_KEY_CERT;\
			}\
			if ( pParams->keyType != NOCS3_HARNESS_TEST_KEY_MAX )\
			{\
				bKeyTypeParsed = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}\

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_R2R_PRIVATE_H__ */