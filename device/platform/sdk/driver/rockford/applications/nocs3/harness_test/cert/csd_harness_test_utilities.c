#include "csd_harness_test.h"
#include "csd_harness_test_utilities.h"
#include <stdio.h>

#define CSD_HELP_M			"[CSD][M]"
#define CSD_HELP_M_INST		"To get more information of a command type \"csd -COMMAND help\""
#define CSD_HELP_M_R2R		"-r2r = RAM2RAM functionality"
#define CSD_HELP_M_DSC		"-dsc = Transport Stream functionality"
#define CSD_HELP_M_SCR		"-scr = Stream Crypto functionality"
#define CSD_HELP_M_SPV		"-spv = Setting various NOCS related persistent values"
#define CSD_HELP_M_GPV		"-gpv = Getting various NOCS related persistent values"
#define CSD_HELP_M_CN		"-cn = Checknumber computation functionality"
#define CSD_HELP_M_FP		"-fp = Flash Encryption/Decryption functionality"
#define CSD_HELP_M_STATUS	"-Status = Chipset configuration status information"
#define CSD_HELP_M_V		"-v = Version of the CSD App"
#define CSD_HELP_M_VER		"The current version of the CSD App is:"
#define CSD_APP_VERSION		"01.00.00"

void csd_harness_test_help (void)
{
	printf("%s\t%s\n", CSD_HELP_M, CSD_HELP_M_INST);
	printf("\t\t%s\n", CSD_HELP_M_R2R);
	printf("\t\t%s\n", CSD_HELP_M_DSC);
	printf("\t\t%s\n", CSD_HELP_M_SCR);
	printf("\t\t%s\n", CSD_HELP_M_SPV);
	printf("\t\t%s\n", CSD_HELP_M_GPV);
	printf("\t\t%s\n", CSD_HELP_M_CN);
	printf("\t\t%s\n", CSD_HELP_M_FP);
	printf("\t\t%s\n", CSD_HELP_M_STATUS);
	printf("\t\t%s\n", CSD_HELP_M_V);
}


void csd_harness_test_version (void)
{
	printf("%s\t%s %s\n", CSD_HELP_M, CSD_HELP_M_VER, CSD_APP_VERSION);
}
