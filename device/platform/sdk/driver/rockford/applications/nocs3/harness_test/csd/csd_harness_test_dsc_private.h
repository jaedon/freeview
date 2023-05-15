
#ifndef __NOCS3_CSD_HARNESS_TEST_DSC_PRIVATE_H__
#define __NOCS3_CSD_HARNESS_TEST_DSC_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* DSC specific options */
#define CSD_HARNESS_TEST_DSC_ECMPID		NOCS3_HARNESS_TEST_DSC_ECMPID
#define CSD_HARNESS_TEST_DSC_PECM		NOCS3_HARNESS_TEST_DSC_PECM

/* DSC specific help message */
#define CSD_HELP_M_DSC_INST		"-dsc = This command allows to perform Descrambling functionality Options are:"
#define CSD_HELP_M_DSC_ECMPID	"[-ecmpid EPID] = Set ECM PID to be processed.  Optional parameter.  If the stream \
is not scrambled or if there are no ECMs in the stream"
#define CSD_HELP_M_DSC_PECM		"-pecm TRUE|FALSE = Whether to processed the ECMs or not.  If set to FALSE, do not \
process ECMs, instead take the key from the TestVector file and set it once at startup for both ODD and EVEN periods. \
I. e. Same key shall be used for both ODD and EVEN periods"

#define CSD_HARNESS_TEST_PARSE_ECM NOCS3_HARNESS_TEST_PARSE_ECM

typedef struct CSD_Harness_Test_DSC_Parameters
{
	unsigned int						videoPid;
	unsigned int						audioPid;
	unsigned int						pcrPid;

	bool								bEcmRequired;
	unsigned int						ecmPid;

	unsigned int						emi;
	unsigned int						sizeIV;
	unsigned char						iv[16];

	FILE *								pInFile;

	FILE *								pOutFile;
	FILE *								pTvFile;

}  CSD_Harness_Test_DSC_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_DSC_PRIVATE_H__ */