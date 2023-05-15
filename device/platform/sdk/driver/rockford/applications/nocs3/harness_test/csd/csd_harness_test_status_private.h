
#ifndef __NOCS3_CSD_HARNESS_TEST_STATUS_PRIVATE_H__
#define __NOCS3_CSD_HARNESS_TEST_STATUS_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* R2R specific options */

/* R2R specific help message */
#define CSD_HELP_M_STATUS_INST		"-status = This command print status information related to the chipset."
#define CSD_HELP_M_STATUS_OFOC		"-of OutFile|-oc = The path and file name of the output file (-of) or \
computed data value printed on standard out (-oc)"

typedef struct CSD_Harness_Test_STATUS_Parameters
{
	FILE *								pOutFile;

}  CSD_Harness_Test_STATUS_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_STATUS_PRIVATE_H__ */