/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: printsc.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 10/28/09 1:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/utils/printsc.c $
 * 
 * 2   10/28/09 1:30p erickson
 * SW7405-3287: move common code to ts_utils
 *
 * Irvine_BSEAVSW_Devel/7   6/30/06 9:39a erickson
 * PR21941: fix start code detection algorithm, and eliminate duplication
 * by moving to ts_utils
 *
 * Irvine_BSEAVSW_Devel/6   1/6/06 10:16a erickson
 * PR17108: updated for magnum basemodules and 64 bit cpus
 *
 * Irvine_BSEAVSW_Devel/5   9/2/05 12:24p erickson
 * PR16138: defaulted to pes/es to make utils more consistent
 *
 * Irvine_BSEAVSW_Devel/4   8/19/05 3:59p erickson
 * PR16208: fixed cmdline, print payload between sc's
 *
 * Irvine_BSEAVSW_Devel/3   8/8/05 3:58p erickson
 * PR16138: added -pes and -es option
 *
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ts_utils.h"

int main(int argc, char **argv) {
    FILE *fin = stdin;
    int fileoffset = 0;
#define BUFSIZE 4096
    unsigned char buf[BUFSIZE];
    int sccount = 0;
    int curarg = 1;
    unsigned payload = 0;
    int num_startcodes = 0;

    if (curarg < argc && !strcmp(argv[curarg], "--help")) {
        printf("Usage: printsc [FILENAME]\n");
        exit(0);
    }

    if (curarg < argc) {
        fin = fopen(argv[curarg], "r");
        if (!fin)
            return printf("Unable to open %s: %d\n", argv[curarg], errno);
    }
    else {
        printf("Reading stdin\n");
    }

    while (!feof(fin)) {
        int i, n;

        n = fread(buf, 1, BUFSIZE, fin);
        if (n <= 0) break;

        for (i=0; i<n; i++) {
            payload++;
            if (sccount == 3) {
                if (num_startcodes > 0) {
                    payload -= 4; /* subtract for 00 00 01 SC */
                    printf("  payload %d\n", payload); /* from previous sc */
                }
                printf("SC %02x at %#x\n", buf[i], fileoffset + i - 3);
                sccount = 0;
                payload = 0;
                num_startcodes++;
            }

            sccount = b_check_for_start_code(buf[i], sccount);
        }
        fileoffset += n;
    }
    return 0;
}

