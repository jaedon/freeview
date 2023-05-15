#include <stdio.h>
#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "csd_harness_test.h"
#include "csd_harness_test_utilities.h"

#define CSD_HELP_M			"[CSD][M]"
#define CSD_HELP_M_INST		"To get more information of a command type \"csd -COMMAND help\""
#define CSD_HELP_M_R2R		"-r2r = RAM2RAM functionality"
#define CSD_HELP_M_DSC		"-dsc = Transport Stream functionality"
#define CSD_HELP_M_SCR		"-scr = Stream Crypto functionality"
#define CSD_HELP_M_SPV		"-spv = Setting various NOCS related persistent values"
#define CSD_HELP_M_GPV		"-gpv = Getting various NOCS related persistent values"
#define CSD_HELP_M_CN		"-cn = Checknumber computation functionality"
#define CSD_HELP_M_FP		"-fp = Flash Encryption/Decryption functionality"
#define CSD_HELP_M_STATUS	"-status = Chipset configuration status information"
#define CSD_HELP_M_V		"-v = Version of the CSD App"
#define CSD_HELP_M_VER		"The current version of the CSD App is:"

const unsigned char gCsd_ErrorStrings[][128] =
{
  "CSD_NO_ERROR",
  "CSD_ERROR",
  "CSD_ERROR_DRIVER_ALREADY_INITIALIZED",
  "CSD_ERROR_INVALID_PARAMETERS",
  "CSD_ERROR_OPERATION_NOT_ALLOWED",
  "CSD_ERROR_OPERATION_NOT_SUPPORTED",
  "CSD_ERROR_INVALID_KEY_PATH_HANDLE"
  "CSD_ERROE_UNSPECIFIED"
};


void csd_harness_test_help (void)
{
	printf("%s\t%s\n", CSD_HELP_M, CSD_HELP_M_INST);
	printf("\t\t%s\n", CSD_HELP_M_R2R);
	printf("\t\t%s\n", CSD_HELP_M_DSC);
	printf("\t\t%s\n", CSD_HELP_M_SCR);
	printf("\t\t%s\n", CSD_HELP_M_SPV);
	printf("\t\t%s\n", CSD_HELP_M_GPV);
	printf("\t\t%s\n", CSD_HELP_M_CN);
#if (CSDAPI_VERSION_MEDIUM<3)
	printf("\t\t%s\n", CSD_HELP_M_FP);
#endif
	printf("\t\t%s\n", CSD_HELP_M_STATUS);
	printf("\t\t%s\n", CSD_HELP_M_V);
}


void csd_harness_test_version (void)
{
#if 1
	printf("%s\t%s %s\n", CSD_HELP_M, CSD_HELP_M_VER, CSD_APP_VERSION);
#else
	printf("%s\t%s %02x.%02x.%02x.\n", CSD_HELP_M, CSD_HELP_M_VER, CSDAPI_VERSION_MAJOR, CSDAPI_VERSION_MEDIUM, CSDAPI_VERSION_MINOR);
#endif
}
