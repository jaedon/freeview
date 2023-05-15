#include <bstd.h>
#include <stdio.h>
#include "sec_harness_test.h"
#include "sec_harness_test_utilities.h"

#define SEC_HELP_M				"[SEC][M]"
#define SEC_HELP_M_INST			"To get more information of a command type \"sec -COMMAND help\""
#define SEC_HELP_M_LCHIPID		"-lchipid = Legacy Chipset identification functionality"
#define SEC_HELP_M_CHIPID		"-chipid = Chipset identification functionality"
#define SEC_HELP_M_LR2R			"-lr2r = Legacy RAM2RAM functionality"
#define SEC_HELP_M_R2R			"-r2r = RAM2RAM functionality"
#define SEC_HELP_M_RSA			"-rsa = RSA functionality"
#define SEC_HELP_M_DH			"-dh = Diffie-Hellman (DH) functionality"
#define SEC_HELP_M_SHA			"-sha = SHA functionality"
#define SEC_HELP_M_HMAC			"-hmac = HMAC-SHA 256 functionality"
#define SEC_HELP_M_RAND			"-rand = Random number generation functionality"
#define SEC_HELP_M_ECDSA		"-ecdsa = Elliptic-curve DSA functionality"
#define SEC_HELP_M_ECDH			"-ecdh = Elliptic-curve DH functionality"
#define SEC_HELP_M_VESRSION		"-v = retrieve version number of the app"

#define SEC_HELP_M_VER		"The current version of the SEC App is:"
#define SEC_APP_VERSION		"01.00.00"

void sec_harness_test_help (void)
{
	printf("%s\t%s\n", SEC_HELP_M, SEC_HELP_M_INST);
	printf("\t\t%s\n", SEC_HELP_M_LCHIPID);
	printf("\t\t%s\n", SEC_HELP_M_CHIPID);
	printf("\t\t%s\n", SEC_HELP_M_LR2R);
	printf("\t\t%s\n", SEC_HELP_M_R2R);
	printf("\t\t%s\n", SEC_HELP_M_RSA);
	printf("\t\t%s\n", SEC_HELP_M_DH);
	printf("\t\t%s\n", SEC_HELP_M_SHA);
	printf("\t\t%s\n", SEC_HELP_M_HMAC);
	printf("\t\t%s\n", SEC_HELP_M_RAND);
	printf("\t\t%s\n", SEC_HELP_M_ECDSA);
	printf("\t\t%s\n", SEC_HELP_M_ECDH);
	printf("\t\t%s\n", SEC_HELP_M_VESRSION);
}


void sec_harness_test_version (void)
{
	printf("%s\t%s %s\n", SEC_HELP_M, SEC_HELP_M_VER, SEC_APP_VERSION);
}
