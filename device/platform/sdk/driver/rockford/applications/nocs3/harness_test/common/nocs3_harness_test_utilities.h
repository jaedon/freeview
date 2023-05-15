
#ifndef __NOCS3_HARNESS_TEST_UTILITIES_H__
#define __NOCS3_HARNESS_TEST_UTILITIES_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define MESSAGE_MODULE_CSD		"[CSD]"
#define MESSAGE_MODULE_SEC		"[SEC]"
#define MESSAGE_MODULE_CERT		"[CERT]"
#define MESSAGE_MODULE_CTD		"[CTD]"
#define MESSAGE_MODULE_DIF		"[DIF]"

#define MESSAGE_TYPE_M			"[F]"
#define MESSAGE_TYPE_EF			"[EF]"
#define MESSAGE_TYPE_EE			"[CERT]"
#define MESSAGE_TYPE_E_CSD		"[E CSD]"
#define MESSAGE_TYPE_E_CERT		"[E CERT]"
#define MESSAGE_TYPE_E_SEC		"[E SEC]"

unsigned int nocs3_harness_test_utility_ascii_2int (const char * pIn);
unsigned int nocs3_harness_test_utility_ascii2array (const char * pIn, 
											 unsigned char * pOut, 
											 unsigned int size);
void nocs3_harness_test_utility_dump_array (unsigned char * pIn, unsigned int size);
void nocs3_harness_test_utility_increase_16bytes_counter(unsigned char * iv, unsigned int amount);

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_HARNESS_TEST_UTILITIES_H__ */