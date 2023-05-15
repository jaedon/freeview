
#ifndef __NOCS3_CSD_HARNESS_TEST_SCR_PRIVATE_H__
#define __NOCS3_CSD_HARNESS_TEST_SCR_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define CSD_HELP_M_SCR_INST		"-scr = This command allows to perform Stream Crypto functionality"
#define CSD_HELP_M_SCR_OP		"-enc|dec = This defines operation peformed for Stream Processing functionality. If \
-enc then encryption (scrambling) operation and if -dec then a decryption (descrambling) operation"

typedef struct CSD_Harness_Test_SCR_Parameters
{
	NOCS3_HARNESS_TEST_OPERATION		operation;
	NOCS3_HARNESS_TEST_KEY				keyType;
	unsigned int						videoPid;
	unsigned int						audioPid;
	unsigned int						pcrPid;

	unsigned int						emi;
	unsigned int						sizeIV;
	unsigned char						iv[16];
	FILE *								pInFile;
	FILE *								pOutFile;
	FILE *								pTvFile;

}  CSD_Harness_Test_SCR_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_SCR_PRIVATE_H__ */