
#ifndef __NOCS3_SEC_HARNESS_TEST_H__
#define __NOCS3_SEC_HARNESS_TEST_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "nocs3_harness_test_common.h"
#include "nocs3_harness_test_parser.h"

NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_initialize (void);
void sec_harness_test_deinitialize (void);


#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_SEC_HARNESS_TEST_H__ */