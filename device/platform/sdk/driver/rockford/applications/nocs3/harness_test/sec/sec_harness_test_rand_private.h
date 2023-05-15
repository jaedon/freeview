
#ifndef __NOCS3_SEC_HARNESS_TEST_RAND_PRIVATE_H__
#define __NOCS3_SEC_HARNESS_TEST_RAND_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* HMAC specific help message */
#define SEC_HELP_M_RAND_INST	"-rand = This command allows to perform random number generation functionality."
#define SEC_HELP_M_RAND_SIZE	"-n NumOfBytes = Number of random bytes to be generated."
#define SEC_HELP_M_RAND_OFOC	"-of OutFile|-oc = The output generated can be stored in a file or can be returned \
back on the command-line."


typedef struct SEC_Harness_Test_Rand_Parameters
{
	unsigned int						size;
	FILE *								pOutFile;
	bool								bOutPut;
}  SEC_Harness_Test_Rand_Parameters;

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_RAND_PRIVATE_H__ */