
#ifndef __NOCS3_SEC_HARNESS_TEST_UTILITIES_H__
#define __NOCS3_SEC_HARNESS_TEST_UTILITIES_H__

#ifdef __cplusplus
extern "C"
{
#endif


extern const unsigned char gSec_ErrorStrings[][128];

#define SEC_ERR_TRACE(code, function, msg) \
	printf("[SEC][SEC E] %s returned %d on %s %d. %s\n",  function, code, __FILE__, __LINE__, msg);

#define SEC_OTHER_ERR_TRACE(msg) \
	printf("[SEC][EF] Failure on %s %d. %s\n",  __FILE__, __LINE__, msg);

void sec_harness_test_help (void);
void sec_harness_test_version (void);

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_UTILITIES_H__ */