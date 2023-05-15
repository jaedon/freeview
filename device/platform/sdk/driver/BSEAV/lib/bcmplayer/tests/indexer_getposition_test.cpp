/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: indexer_getposition_test.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:07a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/tests/indexer_getposition_test.cpp $
 * 
 * 2   9/7/12 10:07a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "bcmindexer.h"
#include "bcmplayer.h"

/**
This test creates a NAV file from an SCT file, incrementing a vchip state.
The NAV file is then read back using bcmplayer. The output should be
the same as running the printindex utility as followed:

./printindex -o index,vchip NAVFILE

**/

int do_nothing(const char *ptr, int size, int count, void *fp)
{
	return count;
}

/**
* Create the NAV file from an SCT file, inserting vchip information.
**/
void createindex(const char *sctfile)
{

	FILE *sct = fopen(sctfile, "r");
	if (!sct) {
		printf("Cannot open %s to read: %d\n", sctfile, errno);
		exit(1);
	}

	BNAV_Indexer_Handle bcmi;
	BNAV_Indexer_Settings settings;

	BNAV_Indexer_GetDefaultSettings(&settings);
	settings.writeCallback = (BNAV_WRITE_CB)do_nothing;
	settings.filePointer = (void*)1;

	if (BNAV_Indexer_Open(&bcmi, &settings)) {
		printf("Failed to open BNAV_Indexer\n");
		exit(1);
	}

	int vchip = 0;
	while (!feof(sct)) {
//#define BUFSIZE 4096
#define BUFSIZE sizeof(BSCT_Entry)
		char buf[BUFSIZE];
		BNAV_Indexer_Position position;

		int n = fread(buf, 1, BUFSIZE, sct);
		if (n == -1) {
			printf("fread error: %d\n", errno);
			exit(1);
		}
		if (BNAV_Indexer_SetVChipState(bcmi, (vchip++ | 0x4040)))
			printf("Error setting vchip\n");
		BNAV_Indexer_Feed(bcmi, (BSCT_Entry*)buf, n / sizeof(BSCT_Entry));

		printf("BSCT_Entry's fed: %d\n", n / sizeof(BSCT_Entry));
		if (BNAV_Indexer_GetPosition(bcmi, &position))
			printf("BNAV_Indexer_GetPosition failed\n");
		else
			printf(
			"BNAV_Indexer_Position:\n"
			"  index:      %d\n"
			"  pts:        0x%lx\n"
			"  offsetHi:   0x%lx\n"
			"  offsetLo:   0x%lx\n"
			"  timestamp:  %d\n"
			"  vchipState: 0x%x\n",
			position.index,
			position.pts,
			position.offsetHi,
			position.offsetLo,
			position.timestamp,
			position.vchipState);
	}

	BNAV_Indexer_Close(bcmi);
	fclose(sct);
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf(
		"BNAV_Indexer_GetPosition test\n"
		"Usage: %s SCTFILE\n", argv[0]);
		exit(1);
	}

	const char *sctfile = argv[1];

	createindex(sctfile);
	return 0;
}
