
#ifndef __NOCS3_CSD_HARNESS_TEST_FP_PRIVATE_H__
#define __NOCS3_CSD_HARNESS_TEST_FP_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* R2R specific options */

/* R2R specific help message */
#define CSD_HELP_M_FP_INST		"-fp KEY|DATA = This defines whether key or data must be encrypted or decrypted."
#define CSD_HELP_M_FP_OP		"-enc|dec = If -enc is an encryption operation.  If -dec is a decryption operation."
#define CSD_HELP_M_FP_IFIC		"[-if InFile|-ic Value] = Value used as an input to Flash Protection. \
Optional parameter.  The value can be taken from DataIn file (-if) or a 64bit/128bit value from the \
command line (-ic).  If both options are not present then it must be taken from .csv file provided with \
option -tv"
#define CSD_HELP_M_FP_OFOC		"-of OutFile|-oc = The path and file name of the output file (-of) or \
computed data value printed on standard out (-oc)"
#define CSD_HELP_M_FP_KEY		"[-fk FlashProtectionKey] = Optional parameter.  Used only when -fp DATA. \
specifyes the key used to enc/dec data."
#define CSD_HELP_M_FP_TV		"[-tv TestVectorFileName] = Indicates the path and filename of the .CSV files \
which contains the test vector"

#define CSD_HARNESS_TEST_FP_KEY			"KEY"
#define CSD_HARNESS_TEST_FP_DATA		"DATA"

#define CSD_HARNESS_TEST_PARSE_FP_TYPE \
	NOCS3_COMMAND_PARSER_PARSE_BINARY_TYPE_REQUIRED(bFpTypeParsered,\
	pParams->protection,\
	CSD_HARNESS_TEST_FP_KEY,\
	CSD_HARNESS_TEST_FP_PROTCTION_KEY,\
	CSD_HARNESS_TEST_FP_DATA,CSD_HARNESS_TEST_FP_PROTCTION_DATA);


#define CSD_HARNESS_TEST_FP_MAX_DATA_SIZE 16
#define CSD_HARNESS_TEST_FP_MAX_KEY_SIZE 16

typedef enum CSD_HARNESS_TEST_FP_PROTCTION
{
	CSD_HARNESS_TEST_FP_PROTCTION_DATA,
	CSD_HARNESS_TEST_FP_PROTCTION_KEY,

	/* Add new enum before this line */
	CSD_HARNESS_TEST_FP_PROTCTION_MAX
} CSD_HARNESS_TEST_FP_PROTCTION;

typedef struct CSD_Harness_Test_FP_Parameters
{
	CSD_HARNESS_TEST_FP_PROTCTION		protection;
	NOCS3_HARNESS_TEST_OPERATION		operation;
	unsigned int						sizeKey;
	unsigned char						key[CSD_HARNESS_TEST_FP_MAX_KEY_SIZE];
	unsigned int						sizeData;
	unsigned char						*pData;

	FILE *								pOutFile;
	FILE *								pTvFile;

}  CSD_Harness_Test_FP_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_FP_PRIVATE_H__ */