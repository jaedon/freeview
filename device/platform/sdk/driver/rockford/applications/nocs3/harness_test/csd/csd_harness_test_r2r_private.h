
#ifndef __NOCS3_CSD_HARNESS_TEST_R2R_PRIVATE_H__
#define __NOCS3_CSD_HARNESS_TEST_R2R_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* R2R specific options */

/* R2R specific help message */
#define CSD_HELP_M_R2R_INST		"-r2r = This command allows to perform RAM2RAM functionality"
#define CSD_HELP_M_R2R_OP		"-enc|dec = This defines operation peformed by RAM2RAM functionality. If \
-enc is an encryption operation.  If -dec is a decryption operation"
#define CSD_HELP_M_R2R_IFIC		"[-if InFile|-ic Value64|Value128] = Value used as an input to RAM2RAM. \
Optional parameter.  The value can be taken from DataIn file (-if) or a 64bit/128bit value from the \
command line (-ic).  If both options are not present then it must be taken from .csv file provided with \
option -tv"
#define CSD_HELP_M_R2R_OFOC		"-of OutFile|-oc = The path and file name of the output file (-of) or \
computed data value printed on standard out (-oc)"
#define CSD_HELP_M_R2R_MT		"[-mt] = Monte Carlo test. Optional parameter"
#define CSD_HELP_M_R2R_RUNS		"-numbruns # = How many times the input data must be sourced by the generated \
output.  This command is mostly used for helping in the Monte Carlo test"


#define CSD_HARNESS_TEST_R2R_MAX_DATA_SIZE 16

typedef struct CSD_Harness_Test_R2R_Parameters
{
	NOCS3_HARNESS_TEST_OPERATION		operation;
	NOCS3_HARNESS_TEST_KEY				keyType;
	unsigned int						emi;
	unsigned int						sizeIV;
	unsigned char						iv[16];
	unsigned int						sizeData;
	unsigned char						data[CSD_HARNESS_TEST_R2R_MAX_DATA_SIZE];
	bool								bMonteCarlo;
	unsigned int						numRuns;

	FILE *								pInFile;
	FILE *								pOutFile;
	FILE *								pTvFile;

}  CSD_Harness_Test_R2R_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_R2R_PRIVATE_H__ */