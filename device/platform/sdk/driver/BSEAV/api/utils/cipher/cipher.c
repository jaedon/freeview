/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cipher.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 7/11/06 6:09p $
 *
 * Module Description: Use the PROC_CA interface in bcmtransdrv.c
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/cipher/cipher.c $
 * 
 * 2   7/11/06 6:09p mphillip
 * PR19544: Converting keysize to key_length in examples and other related
 * files
 * 
 * 1   2/7/05 7:35p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   11/18/03 1:54p erickson
 * PR8356: got venom2 working and added scrambling_control support
 * 
 * Irvine_BSEAVSW_Devel/5   11/18/03 10:27a erickson
 * PR8356: refactored cipher so that it's a separate subdriver with a
 * consistent name
 * 
 * Irvine_BSEAVSW_Devel/4   11/3/03 12:23p erickson
 * adapted examples and utils to settop api changes
 * 
 * Irvine_BSEAVSW_Devel/3   10/28/03 11:28a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/2   10/22/03 11:23a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/1   9/25/03 1:28p erickson
 * added cpu-based encryption to settop api and util
 *
 * Irvine_BSEAVSW_Devel/2   5/8/03 11:43a erickson
 * moved transport logic into driver
 *
 * Irvine_BSEAVSW_Devel/1   4/30/03 5:02p erickson
 * reworked PROC_CA in driver and test_apps
 *
 ***************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define NOTICE "cipher (c)2003 Broadcom Corp."

void printUsage()
{
	fprintf(stderr,
	NOTICE "\n"
	"\n"
	"Description: cipher uses the Broadcom microprocessor conditional access support.\n"
	"\n"
	"Usage: cipher [OPTIONS] [INPUTFILE [OUTPUTFILE]]\n"
	"  OPTIONS:\n"
	"    --help,-h  Print this help screen.\n"
	"    -ts        INPUTFILE is an MPEG2 transport stream.\n"
	"    -d         Decrypt INPUTFILE, otherwise encrypt it.\n"
	"    -3DES      Use 3DES, otherwise use DES.\n"
	"    -56        Is a 56 bit key, otherwise it's 64 bit.\n"
	"    -key KEY0,KEY1,KEY2,KEY3\n"
	"               Specify hex encryption keys (0x is optional), otherwise\n"
	"               a default test key is used.\n"
	"               KEY2 and KEY3 are only needed for 3DES.\n"
	"\n"
	"  If INPUTFILE is - or blank, stdin is used.\n"
	"  If OUTPUTFILE is - or blank, stdout is used.\n"
	);
}

int main(int argc, char **argv)
{
	int curarg = 1;
	bcipher_t cipher;
	bcipher_settings settings;
#define BUFSIZE 4096
	unsigned char buf[BUFSIZE];
	const char *infile = "-", *outfile = "-";
	FILE *inf, *outf;

	if (bsettop_init(BSETTOP_VERSION))
		return 1;

	cipher = bcipher_open(B_ID(0));
	if (!cipher) return 1;

	bcipher_settings_init(&settings, cipher);
	/* app defaults */
	settings.data_format = bcipher_data_format_raw;
	settings.mode = bcipher_mode_encrypt;

	// read options
	while (curarg < argc && argv[curarg][0] == '-') {
		if (!strcmp(argv[curarg], "--help") || !strcmp(argv[curarg], "-h")) {
			printUsage();
			return 0;
		}
		else if (!strcmp(argv[curarg], "-ts")) {
			settings.data_format = bcipher_data_format_ts;
		}
		else if (!strcmp(argv[curarg], "-56")) {
			settings.encryption.key_length = 56;
		}
		else if (!strcmp(argv[curarg], "-d")) {
			settings.mode = bcipher_mode_decrypt;
		}
		else if (!strcasecmp(argv[curarg], "-3DES")) {
			settings.encryption.type = bencryption_type_3des;
		}
		else if (!strcmp(argv[curarg], "-key") && curarg+1 < argc) {
			char *keys = argv[++curarg];
			int i = 0;

			memset(settings.encryption.key,0,sizeof(settings.encryption.key));
			do {
				char *s;
				settings.encryption.key[i++] = strtoul(keys, &s, 16);
				if (*s)
					keys = s+1;
				else
					keys = NULL;
			} while (keys);
		}
		curarg++;
	}

	fprintf(stderr, NOTICE "\n");
	fprintf(stderr, "Use --help for help.\n");
	
	// read filenames
	if (curarg < argc)
		infile = argv[curarg++];
	if (curarg < argc)
		outfile = argv[curarg++];

	if (!strcmp(infile, "-"))
		inf = stdin;
	else
		inf = fopen(infile, "r");
	if (!inf) {
		fprintf(stderr, "Unable to open %s: %d\n", infile, errno);
		return 1;
	}

	if (!strcmp(outfile, "-"))
		outf = stdout;
	else
		outf = fopen(outfile, "w+");
	if (!inf) {
		fprintf(stderr, "Unable to open %s: %d\n", outfile, errno);
		return 1;
	}

	// print status before beginning work
	fprintf(stderr, " mode: %s %s%s\n", 
		settings.encryption.type == bencryption_type_3des?"3DES":"DES",
		settings.mode == bcipher_mode_encrypt?"encrypt":"decrypt",
		settings.data_format == bcipher_data_format_ts?" (MPEG2 transport)":"(raw data)");
	if (settings.encryption.type == bencryption_type_3des)
		fprintf(stderr, " %d bit key: %08x %08x %08x %08x\n", settings.encryption.key_length*2,
			settings.encryption.key[0], settings.encryption.key[1], settings.encryption.key[2],
			settings.encryption.key[3]);
	else
		fprintf(stderr, " %d bit key: %08x %08x\n", settings.encryption.key_length,
			settings.encryption.key[0], settings.encryption.key[1]);
	fprintf(stderr, " input: %s\n", inf == stdin ? "stdin" : infile);
	fprintf(stderr, " output: %s\n", outf == stdout ? "stdout" : outfile);

	while (!feof(inf)) {
		size_t n = BUFSIZE;
		if (settings.data_format == bcipher_data_format_ts)
			n -= n % 188; /* transport packet aligned */
		else
			n -= n % 8; /* 8 byte aligned */

		n = fread(buf, 1, n, inf);
		if (!n) break;
		
		if (bcipher_convert_data(cipher, buf, buf, n, &settings))
			return 1;

		if (fwrite(buf, 1, n, outf) != n) {
			fprintf(stderr, "fwrite failed: %d\n", errno);
			return 1;
		}
	}

	return 0;
}
