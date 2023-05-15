
#ifndef __NOCS3_CSD_HARNESS_TEST_H__
#define __NOCS3_CSD_HARNESS_TEST_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "nocs3_harness_test_common.h"

NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_initialize (void);
void csd_harness_test_deinitialize (void);


#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_H__ */