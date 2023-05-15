#include <bstd.h>
#include <stdio.h>

#include "sec_harness_test.h"
#include "sec_harness_test_lchipid.h"
#include "sec_harness_test_chipid.h"
#include "sec_harness_test_lr2r.h"
#include "sec_harness_test_r2r.h"
#include "sec_harness_test_rsa.h"
#include "sec_harness_test_dh.h"
#include "sec_harness_test_sha.h"
#include "sec_harness_test_hmac.h"
#include "sec_harness_test_rand.h"
#include "sec_harness_test_ecdh.h"
#include "sec_harness_test_rand.h"
#include "sec_harness_test_parser.h"
#include "sec_harness_test_ecdsa.h"
#include "sec_harness_test_utilities.h"

static NOCS3_HARNESS_TEST sec_harness_test_get_test_type (int argc, char * argv[])
{
	NOCS3_HARNESS_TEST test_type = NOCS3_HARNESS_TEST_MAX;

	if ( argc <2)
	{
		test_type = NOCS3_HARNESS_TEST_HELP;
	}
	else if ( strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_R2R)==0 ) 
	{
		test_type = NOCS3_HARNESS_TEST_R2R;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_LCHIPID)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_LCHIPID;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_CHIPID)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_CHIPID;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_LR2R)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_LR2R;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_RSA)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_RSA;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_DH)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_DH;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_SHA)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_SHA;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_HMAC)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_HMAC;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_RAND)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_RAND;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_ECDSA)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_ECDSA;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_ECDH)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_ECDH;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_FP)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_FP;
	}
	else if (strcmp (argv[1], NOCS3_HARNESS_TEST_TYPE_VERSION)==0 )
	{
		test_type = NOCS3_HARNESS_TEST_VERSION;
	}


	return test_type;
}


int main (int argc, char* argv[])
{
	NOCS3_HARNESS_TEST test_type;

	test_type = sec_harness_test_get_test_type (argc, argv);
	switch (test_type)
	{
		case NOCS3_HARNESS_TEST_LCHIPID:
			sec_harness_test_lchipid (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_CHIPID:
			sec_harness_test_chipid (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_LR2R:
			sec_harness_test_lr2r (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_R2R:
			sec_harness_test_r2r (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_RSA:
			sec_harness_test_rsa (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_DH:
			sec_harness_test_dh (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_SHA:
			sec_harness_test_sha (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_HMAC:
			sec_harness_test_hmac (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_RAND:
			sec_harness_test_rand (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_ECDSA:
			return sec_harness_test_ecdsa (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_ECDH:
			sec_harness_test_ecdh (argc, argv);
			break;


		case NOCS3_HARNESS_TEST_FP:
			sec_harness_test_fp (argc, argv);
			break;

		case NOCS3_HARNESS_TEST_VERSION:
			sec_harness_test_version ();
			break;

		case NOCS3_HARNESS_TEST_HELP:
		default:
			sec_harness_test_help ();
			break;

	}


	return 0;
}
