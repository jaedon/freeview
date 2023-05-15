
#ifndef __NOCS3_SEC_HARNESS_TEST_FP_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_FP_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* R2R specific options */

/* R2R specific help message */
#define SEC_HELP_M_FP_INST		"-fp KEY|DATA = This defines whether key or data must be encrypted or decrypted."
#define SEC_HELP_M_FP_OP		"-enc|dec = If -enc is an encryption operation.  If -dec is a decryption operation."
#define SEC_HELP_M_FP_IFIC		"[-if InFile|-ic Value] = Value used as an input to Flash Protection. \
Optional parameter.  The value can be taken from DataIn file (-if) or a 64bit/128bit value from the \
command line (-ic).  If both options are not present then it must be taken from .csv file provided with \
option -tv"
#define SEC_HELP_M_FP_OFOC		"-of OutFile|-oc = The path and file name of the output file (-of) or \
computed data value printed on standard out (-oc)"
#define SEC_HELP_M_FP_KEY		"[-emi emi value] = Optional parameter.  Used only when -fp DATA. \
specifyes the emi used to enc/dec data."
#define SEC_HELP_M_FP_TV		"[-tv TestVectorFileName] = Indicates the path and filename of the .CSV files \
which contains the test vector"

#define SEC_HARNESS_TEST_FP_KEY			"KEY"
#define SEC_HARNESS_TEST_FP_DATA		"DATA"

#define SEC_HARNESS_TEST_PARSE_FP_TYPE \
	NOCS3_COMMAND_PARSER_PARSE_BINARY_TYPE_REQUIRED(bFpTypeParsered,\
	pParams->protection,\
	SEC_HARNESS_TEST_FP_KEY,\
	SEC_HARNESS_TEST_FP_PROTCTION_KEY,\
	SEC_HARNESS_TEST_FP_DATA,SEC_HARNESS_TEST_FP_PROTCTION_DATA);


#define SEC_HARNESS_TEST_FP_MAX_DATA_SIZE 16
#define SEC_HARNESS_TEST_FP_MAX_KEY_SIZE 16

typedef enum SEC_HARNESS_TEST_FP_PROTCTION
{
	SEC_HARNESS_TEST_FP_PROTCTION_DATA,
	SEC_HARNESS_TEST_FP_PROTCTION_KEY,

	/* Add new enum before this line */
	SEC_HARNESS_TEST_FP_PROTCTION_MAX
} SEC_HARNESS_TEST_FP_PROTCTION;

typedef struct SEC_Harness_Test_FP_Parameters
{
	SEC_HARNESS_TEST_FP_PROTCTION		protection;
	NOCS3_HARNESS_TEST_OPERATION		operation;
	unsigned int						sizeKey;
	unsigned char						key[SEC_HARNESS_TEST_FP_MAX_KEY_SIZE];
	unsigned int						sizeData;
	unsigned char						*pData;
	unsigned int						emi;
	FILE *								pOutFile;
	FILE *								pTvFile;

}  SEC_Harness_Test_FP_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_FP_PRIVATE_H__ */