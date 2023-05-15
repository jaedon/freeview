
#ifndef __NOCS3_SEC_HARNESS_TEST_LR2R_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_LR2R_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "sec_harness_test_r2r_common.h"

/* R2R specific options */

/* R2R specific help message */
#define SEC_HELP_M_LR2R_INST		"-lr2r = This defines the Legacy RAM2RAM functionality command"
#define SEC_HELP_M_LR2R_OP		"-enc|dec = This defines operation peformed by RAM2RAM functionality. If \
-enc is an encryption operation.  If -dec is a decryption operation"
#define SEC_HELP_M_LR2R_IFIC		"[-if InFile|-ic Value64|Value128] = Value used as an input to RAM2RAM. \
Optional parameter.  The value can be taken from DataIn file (-if) or a 64bit/128bit value from the \
command line (-ic).  If both options are not present then it must be taken from .csv file provided with \
option -tv"
#define SEC_HELP_M_LR2R_OFOC		"-of OutFile|-oc = The path and file name of the output file (-of) or \
computed data value printed on standard out (-oc)"
#define SEC_HELP_M_LR2R_MT		"[-mt] = Monte Carlo test. Optional parameter"
#define SEC_HELP_M_LR2R_RUNS		"-numbruns # = How many times the input data must be sourced by the generated \
output.  This command is mostly used for helping in the Monte Carlo test"


#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_LR2R_PRIVATE_H__ */