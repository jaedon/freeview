
#ifndef __NOCS3_CSD_HARNESS_TEST_SPV_GPV_PRIVATE_H__
#define __NOCS3_CSD_HARNESS_TEST_SPV_GPV_PRIVATE_H__

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct CSD_Harness_Test_SPV_GPV_Parameters
{
	NOCS3_HARNESS_TEST_PV_NAME			pvName;
	unsigned char						*pvValue;
	unsigned int						pvSize;
	unsigned int						pvId;

	NOCS3_HARNESS_TEST_BOOT_MODE			bootMode;
}  CSD_Harness_Test_SPV_GPV_Parameters;


NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_spv_gpv_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_SPV_GPV_Parameters * pParams,
																		bool bIsSpv);

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_SPV_GPV_PRIVATE_H__ */