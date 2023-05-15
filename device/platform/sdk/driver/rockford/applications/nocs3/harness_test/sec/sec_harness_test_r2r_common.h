
#ifndef __NOCS3_SEC_HARNESS_TEST_R2R_COMMON_H__
#define __NOCS3_SEC_HARNESS_TEST_R2R_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SEC_HARNESS_TEST_R2R_MAX_DATA_SIZE 16
typedef enum SEC_HARNESS_TEST_PARITY_TYPE
{
	SEC_HARNESS_TEST_PARITY_TYPE_EVEN,
	SEC_HARNESS_TEST_PARITY_TYPE_ODD,
	SEC_HARNESS_TEST_PARITY_TYPE_UNDEFINED,
	/* Add new enum before this line */
	SEC_HARNESS_TEST_PARITY_TYPE_MAX
}	SEC_HARNESS_TEST_PARITY_TYPE;

typedef struct SEC_Harness_Test_R2R_Parameters
{
	NOCS3_HARNESS_TEST_OPERATION		operation;
	NOCS3_HARNESS_TEST_KEY				keyType;
	unsigned int						emi;
	unsigned int						sizeIV;
	unsigned char						iv[16];
	unsigned int						sizeData;
	unsigned char						data[16];
	bool								bMonteCarlo;
	unsigned int						numRuns;
	SEC_HARNESS_TEST_PARITY_TYPE	    parity;
	FILE *								pInFile;
	FILE *								pOutFile;
	FILE *								pTvFile;

}  SEC_Harness_Test_R2R_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_R2R_COMMON_H__ */
