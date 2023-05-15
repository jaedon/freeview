
#ifndef __NOCS3_CSD_HARNESS_TEST_UTILITIES_H__
#define __NOCS3_CSD_HARNESS_TEST_UTILITIES_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define CSD_HELP_M_KEY		NOCS3_HELP_M_KEY
#define CSD_HELP_M_EMI		NOCS3_HELP_M_EMI
#define CSD_HELP_M_IVC		NOCS3_HELP_M_IVC
#define CSD_HELP_M_TV		NOCS3_HELP_M_TV
#define CSD_HELP_M_VPID		NOCS3_HELP_M_VPID
#define CSD_HELP_M_APID		NOCS3_HELP_M_VPID
#define CSD_HELP_M_PPID		NOCS3_HELP_M_PPID
#define CSD_HELP_M_IF		NOCS3_HELP_M_IF
#define CSD_HELP_M_OF		NOCS3_HELP_M_OF

extern const unsigned char gCsd_ErrorStrings[][128];

#define CSD_ERR_TRACE(code, function, msg) \
	if (code>LAST_CSD_STATUS)\
		code = LAST_CSD_STATUS;\
		printf("[CSD][E CSD] %s returned %s on %s %d. %s\n",  function, gCsd_ErrorStrings[code], __FILE__, __LINE__, msg);

#define CSD_OTHER_ERR_TRACE(msg) \
	printf("[CSD][EF] Failure on %s %d. %s\n",  __FILE__, __LINE__, msg);

#define CSD_HARNESS_TEST_GET_STATUS(func, field, msg)\
	status = func(field);\
	if (status!=CSD_NO_ERROR) \
	{\
		CSD_ERR_TRACE (status, #func, msg);\
		return status;\
	}\

#define CSD_HARNESS_TEST_GET_STATUS_IGNORE_ERROR(func, field, msg)\
	status = func(field);\
	if (status!=CSD_NO_ERROR) \
	{\
		CSD_ERR_TRACE (status, #func, msg);\
	}\

void csd_harness_test_help (void);
void csd_harness_test_version (void);

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_UTILITIES_H__ */