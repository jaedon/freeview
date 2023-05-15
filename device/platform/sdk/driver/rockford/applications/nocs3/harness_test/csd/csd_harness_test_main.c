#include <bstd.h>
#include <stdio.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"

#include "csd_harness_test.h"
#include "csd_harness_test_cn.h"
#include "csd_harness_test_dsc.h"
#include "csd_harness_test_r2r.h"
#include "csd_harness_test_scr.h"
#include "csd_harness_test_spv.h"
#include "csd_harness_test_gpv.h"
#include "csd_harness_test_fp.h"
#include "csd_harness_test_status.h"
#include "csd_harness_test_utilities.h"
#include "csd_harness_test_parser.h"

int main (int argc, char* argv[])
{
	NOCS3_HARNESS_TEST test_type;

	test_type = nocs3_harness_test_get_test_type (argc, argv);
	switch (test_type)
	{
		case NOCS3_HARNESS_TEST_R2R:
			csd_harness_test_r2r (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_DSC:
			csd_harness_test_dsc (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_SCR:
			csd_harness_test_scr (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_SPV:
			csd_harness_test_spv (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_GPV:
			csd_harness_test_gpv (argc, argv);
			break;

#if (CSDAPI_VERSION_MEDIUM<3)
		case NOCS3_HARNESS_TEST_FP:
			csd_harness_test_fp (argc, argv);
			break;
#endif
		case NOCS3_HARNESS_TEST_CN:
			csd_harness_test_cn (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_STATUS:
			csd_harness_test_status (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_VERSION:
			csd_harness_test_version ();
			break;

		case NOCS3_HARNESS_TEST_HELP:
		default:
			csd_harness_test_help ();
			break;

	}



	return 0;
}
